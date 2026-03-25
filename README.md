# Word Shooter (C++ OpenGL)

Word Shooter is a C++ arcade-style mini game built with OpenGL/GLUT and SDL2_mixer.
You shoot alphabet tiles into a grid and score points when valid English words are formed.

## Features

- 2D letter-grid gameplay with projectile shooting
- Real-time aiming using mouse movement
- Word detection in rows, columns, and diagonals
- Background music support via SDL2_mixer
- Timer-based round with score display

## Project Structure

- `wordshooter.cpp`: Main game logic, rendering, input handling, timer loop
- `util.cpp`, `util.h`: Utility drawing and helper functions
- `Board.cpp`, `Board.h`: Board-related code (template/legacy support)
- `words_alpha.txt`: Dictionary used to validate words
- `df.mp3`: Background music
- `image-data.bin`: Preprocessed texture data for letters
- `Makefile`: Build instructions
- `install-libraries.sh`: Ubuntu dependency installation helper

## Requirements

This project is set up primarily for Linux (or WSL on Windows) with `make`.

Required libraries:

- OpenGL / GLUT (`freeglut`)
- GLEW
- FreeImage
- SDL2
- SDL2_mixer
- pthread, X11, GLU, GL

For Ubuntu/Debian, use the provided script:

```bash
chmod +x install-libraries.sh
./install-libraries.sh
sudo apt-get install libsdl2-dev libsdl2-mixer-dev
```

## Build

From the project root:

```bash
make clean
make
```

This creates the executable:

- `word-shooter`

## Run

```bash
./word-shooter
```

## Controls

- Move mouse: Aim the shot direction
- Left click: Fire a letter projectile
- `Esc`: Exit game

## Gameplay Notes

- Initial rows are filled with random letters.
- Forming a valid dictionary word clears those tiles.
- Score increases by 7 per detected word.
- The round is timed (about 150 seconds in current settings).

## Before Uploading to GitHub

Recommended cleanup and checks:

1. Remove compiled artifacts from version control (`*.o`, `word-shooter`) if not needed.
2. Ensure required runtime files are included (`words_alpha.txt`, `df.mp3`, `image-data.bin`).
3. Add a `.gitignore` for build outputs.
4. Verify build on a fresh environment (Linux/WSL).

## Author

- Name: MAAZ SHER
- Role: AI/ML Developer
- Email: shermaaz55@gmail.com
