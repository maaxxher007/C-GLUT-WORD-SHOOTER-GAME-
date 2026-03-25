/*
 * Maintainer Comment:
 * Name: MAAZ SHER
 * Role: AI/ML Developer
 * Email: shermaaz55@gmail.com
 */

     #ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP
#include <cstdlib>

#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "util.h"
using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B))
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 100
string *dictionary;
int dictionarysize = 370099;
#define KEY_ESC 27
const int awidth = 60, aheight = 60;
const int topMargin = 100; 
const int maxRows = 6; 
const int maxCols = 13; 
const int bradius = 25;
int width = 930, height = 660;
int byoffset = bradius;
int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset) / (2 * bradius);
int nfrows = 2;
float score = 0;
int **board;
int bwidth = 130;
int bheight = 10;
int bsx, bsy;
float remainingTime = 150; 
const int nalphabets = 26;
enum alphabets {
    AL_NONE = -1, 
    AL_A, AL_B, AL_C, AL_D, AL_E, AL_F, AL_G, AL_H, AL_I, AL_J, AL_K, AL_L, AL_M, AL_N, AL_O, AL_P, AL_Q, AL_R, AL_S, AL_T, AL_U, AL_W, AL_X, AL_y, AL_Z
};
alphabets cookieRows[maxRows][maxCols];
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = {"a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp", "k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp", "x.bmp", "y.bmp", "z.bmp"};
GLuint mtid[nalphabets];
bool shotfired = false; 
float shotX, shotY;              
float shotDX, shotDY;            
alphabets currentProjectile;     
void RegisterTextures_Write() {
    glGenTextures(nalphabets, tid);
    vector<unsigned char> data;
    ofstream ofile("image-data.bin", ios::binary | ios::out);
    for (int i = 0; i < nalphabets; ++i) {
        ReadImage(tnames[i], data);
        if (i == 0) {
            int length = data.size();
            ofile.write((char*)&length, sizeof(int));
        }
        ofile.write((char*)&data[0], sizeof(char) * data.size());
        mtid[i] = tid[i];
        glBindTexture(GL_TEXTURE_2D, tid[i]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        bool wrap = true;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
    }
    ofile.close();
}
void RegisterTextures() {
    glGenTextures(nalphabets, tid);
    vector<unsigned char> data;
    ifstream ifile("image-data.bin", ios::binary | ios::in);
    if (!ifile) {
        cout << " Couldn't Read the Image Data file ";
    }
    int length;
    ifile.read((char*)&length, sizeof(int));
    data.resize(length, 0);
    for (int i = 0; i < nalphabets; ++i) {
        ifile.read((char*)&data[0], sizeof(char)* length);
        mtid[i] = tid[i];
        glBindTexture(GL_TEXTURE_2D, tid[i]);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        bool wrap = true;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
    }
    ifile.close();
}
void DrawAlphabet(const alphabets &cname, int sx, int sy, int cwidth = 60, int cheight = 60) {
    if (cname == AL_NONE) return; 
    float fwidth = (float)cwidth / width * 2, fheight = (float)cheight / height * 2;
    float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mtid[cname]);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex2d(fx, fy);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(fx + fwidth, fy);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(fx + fwidth, fy + fheight);
    glTexCoord2d(0.0, 1.0);
    glVertex2d(fx, fy + fheight);
    glEnd();
    glColor4f(1, 1, 1, 1);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
int GetAlphabet() {
    return rand() % 26; 
}
void Pixels2Cell(float px, float py, int &row, int &col) {
    col = static_cast<int>(px / (awidth + 10)); 
    row = static_cast<int>((height - py - topMargin) / (aheight + 10)); 
    if (row < 0) row = 0; 
}
void Cell2Pixels(int row, int col, int &px, int &py) {
    px = col * (awidth + 10);
    py = height - (topMargin + row * (aheight + 10));
}
void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60) {
    float fwidth = (float)cwidth / width * 2, fheight = (float)cheight / height * 2;
    float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, -1);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex2d(fx, fy);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(fx + fwidth, fy);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(fx + fwidth, fy + fheight);
    glTexCoord2d(0.0, 1.0);
    glVertex2d(fx, fy + fheight);
    glEnd();
    glColor4f(1, 1, 1, 1);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
void rowmaker() {
    for (int i = 0; i < maxRows; ++i) {
        for (int j = 0; j < maxCols; ++j) {
            if (i < 2) { // For the first two rows
                cookieRows[i][j] = static_cast<alphabets>(rand() % nalphabets); 
            } else {
                cookieRows[i][j] = AL_NONE;  
    }
}
    }
}

void DisplayFunction() {
    glClear(GL_COLOR_BUFFER_BIT); 

    for (int row = 0; row < maxRows; ++row) {
        for (int col = 0; col < maxCols; ++col) {
            int x = col * (awidth + 10);
            int y = height - (topMargin + row * (aheight + 10));
            DrawAlphabet(cookieRows[row][col], x, y, awidth, aheight);
        }
    }
    if (shotfired) {
        DrawAlphabet(currentProjectile, shotX, shotY, awidth, aheight);
    }
    DrawString(40, height - 20, width, height + 5, "Score " + Num2Str(score), colors[BLUE_VIOLET]);
    DrawString(width / 2 - 30, height - 25, width, height, 
               "Time Left: " + Num2Str(remainingTime), colors[RED]);
    glClearColor(1.0 /*Red Component*/, 1.0 /*Green Component*/, 1.0 /*Blue Component*/, 1.0 /*Alpha component*/);

    DrawString(width - 300, height - 20, width, height, "Maaz Sher 24I0083", colors[BLACK]);
    DrawShooter((width / 2) - 35, 0, bwidth, bheight);
    glutSwapBuffers();
}





void SetCanvasSize(int width, int height) {}
void NonPrintableKeys(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {}
    else if (key == GLUT_KEY_RIGHT) {}
    else if (key == GLUT_KEY_UP) {}
    else if (key == GLUT_KEY_DOWN) {}
}
void MouseMoved(int x, int y) {
    if (!shotfired) {
        float aimX = x - width / 2;   
        float aimY = height - y;     
        float magnitude = sqrt(aimX * aimX + aimY * aimY);
        shotDX = aimX / magnitude;
        shotDY = aimY / magnitude;
    }
}
void MouseClicked(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && !shotfired) {
        shotfired = true;
        shotX = width / 2 - awidth / 2;  
        shotY = 0;
        shotDX = shotDY = 0;  
        currentProjectile = static_cast<alphabets>(GetAlphabet()); 
        float aimX = x - (width / 2);
        float aimY = height - y;
        float magnitude = sqrt(aimX * aimX + aimY * aimY);
        if (magnitude != 0) {
            shotDX = aimX / magnitude;
            shotDY = aimY / magnitude;
        }
        cout << "gola  fired: " << static_cast<char>('A' + currentProjectile) << endl;
        glutPostRedisplay();
    }
}

void wordformed() {
    for (int col = 0; col < maxCols; col++) {
    for (int startRow = 0; startRow < maxRows; startRow++) {
    string vertical = ""; 
    for (int row = startRow; row < maxRows; row++) {
    if (cookieRows[row][col] != AL_NONE) {
    char currentChar = 'a' + cookieRows[row][col];
    vertical += currentChar;
    for (int i = 0; i < dictionarysize; i++) {
    if (dictionary[i] == vertical) {
    for (int r = startRow; r <= row; r++) {
    cookieRows[r][col] = AL_NONE;
                            }
    score += 7; 
    break;
    }
    }
    } else {
    break;
    }
     }
    }
    }


    for (int row = 0; row < maxRows; row++) {
    for (int startCol = 0; startCol < maxCols; startCol++) {
    string horizontal = ""; 
    for (int col = startCol; col < maxCols; col++) {
    if (cookieRows[row][col] != AL_NONE) {
    char currentChar = 'a' + cookieRows[row][col];
    horizontal += currentChar;
    for (int i = 0; i < dictionarysize; i++) {
    if (dictionary[i] == horizontal) {
    for (int c = startCol; c <= col; c++) {
    cookieRows[row][c] = AL_NONE;
                            }
    score += 7; 
    break;
    }
    }
    } else {
    break;
    }
    }
    }
    }

   



 for (int startRow = 0; startRow < maxRows; startRow++) {
    for (int startCol = 0; startCol < maxCols; startCol++) {
        string leftDiagonal = "";
    for (int row = startRow; row < maxRows; row++) {
    for (int col = startCol; col < maxCols; col++) {
    if (row - startRow == col - startCol) { 
    if (cookieRows[row][col] != AL_NONE) {
    char currentChar = 'a' + cookieRows[row][col];
    leftDiagonal += currentChar;

    for (int i = 0; i < dictionarysize; i++) {
    if (dictionary[i] == leftDiagonal) {
    for (int r = startRow, c = startCol; r <= row && c <= col; r++, c++) {
    cookieRows[r][c] = AL_NONE;
     }
    score += 7;
    break;
    }
    }
    } else {
    break; 
    }
 }
    }
    }
    }
}



}


void collosionwithrows() {
    int col, row;
    Pixels2Cell(shotX, shotY, row, col);
    if (row >= 0 && row < maxRows && col >= 0 && col < maxCols) {
    if (cookieRows[row][col] != AL_NONE) {
    for (int jointRow = row; jointRow  >= 0; --jointRow ) {
        if (cookieRows[jointRow ][col] != AL_NONE) {
    if (jointRow  + 1 < maxRows && cookieRows[jointRow  + 1][col] == AL_NONE) {
    cookieRows[jointRow  + 1][col] = currentProjectile; 
    shotfired = false; 
    cout << "gola placed at " << jointRow  + 1 << " " << col << " " << endl;
    return;
    }
    }
    }
    cookieRows[row][col] = currentProjectile;
    shotfired = false;
    cout << "gola  placed at " << row << " " << col << "" << endl;
        }
    }
}

void PrintableKeys(unsigned char key, int x, int y) {
    if (key == KEY_ESC) {
        exit(1);
    }
}

void shotpath() {
    if (shotfired) {
        glBegin(GL_LINES);
        glVertex2f(shotX, shotY);
        glVertex2f(shotX + shotDX * 100, shotY + shotDY * 100); 
        glEnd();
    }
}



void Timer(int m) {
    remainingTime-=0.4;
    if (remainingTime <= 0) {
        cout << " Final S " << score << endl;
        exit(0); 
    }
    if (shotfired) {
        shotX += shotDX * 100;
        shotY += shotDY * 100;
        if (shotX <= 0) {
            shotX = 0;
            shotDX *= -1; 
        } else if (shotX >= width - awidth) {
            shotX = width - awidth;
            shotDX *= -1;  
        }
        if (shotY >= height) {
            shotfired = false;
        }
        collosionwithrows();
    }
    glutPostRedisplay();
    glutTimerFunc(75, Timer, 0);
    wordformed();
}
int main(int argc, char *argv[]) {
     srand(time(0)); 

	dictionary = new string[dictionarysize]; 
	ReadWords("words_alpha.txt", dictionary); 
	for(int i=0; i < 5; ++i)
		cout<< " word "<< i << " =" << dictionary[i] <<endl;
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) ;
    Mix_Music* bgMusic = Mix_LoadMUS("df.mp3") ; 
    Mix_PlayMusic(bgMusic, -1) ;
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); 
	glutInitWindowPosition(50, 50); 
	glutInitWindowSize(width, height); 
	glutCreateWindow("ITCs Word Shooter"); 
	rowmaker() ;
	RegisterTextures();
	glutDisplayFunc(DisplayFunction);
	glutSpecialFunc(NonPrintableKeys);
	glutKeyboardFunc(PrintableKeys); 
	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); 
	glutTimerFunc(1000.0/FPS, Timer, 0);
	glutMainLoop();
    return 1;
}
#endif
