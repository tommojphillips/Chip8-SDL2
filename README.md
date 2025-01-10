## Chip8 Interpreter

A Chip-8 Interpreter written in C using SDL2 to render the graphics.Passes all tests in Timendus's Chip8 Test Suite. 
The Interpreter is limited by `instructions per frame` ( FPI ) or by a clock frequency in Hz, both are adjustable
The frame rate is targeted at 60 fps

---

#### Input

| Keyboard | ---> |  Chip8   |
| -------- | ---- | -------- | 
| 1 2 3 4  | ---> | 1 2 3 C  |
| Q W E R  | ---> | 4 5 6 D  |
| A S D F  | ---> | 7 8 9 E  |
| Z X C V  | ---> | A 0 B F  |

| Key         | Desc                   |
| ---         | ---------------------- | 
| Ctrl+R      | Reset current program  |
| Space       | Pause/Unpause          |
| Plus ( + )  | Increment Clock        |
| Minus ( - ) | Decrment Clock         |

---

### Loading Chip8 Programs
 - *Drag n drop* files onto the window to load them
 - Launch a program using: `Chip8.exe <program_file>

 ---

 #### Dependencies
 
The only dependencies are SDL2 and SDL2-image libraries
  - SDL2 v2.30.10 - https://github.com/libsdl-org/SDL/releases/tag/release-2.30.10
  - SDL2-Image v2.8.4 - https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.4

 ---

#### Sources
 - [corax89's Chip8 Test Rom](https://github.com/corax89/chip8-test-rom)
 - [Timendus's Chip8 Test Suite](https://github.com/Timendus/chip8-test-suite)
 - [Chip8 Links](https://chip-8.github.io/links/)
 - [Wiki](https://en.wikipedia.org/wiki/CHIP-8)

 ---

#### Screenshots

Space Invaders
![Screenshot 2025-01-01 105501](https://github.com/user-attachments/assets/9f6ca31a-438c-4077-8d84-8d0d98cf2e6b)
![Screenshot 2025-01-01 105413](https://github.com/user-attachments/assets/e5d8cce6-d1a7-47b3-a273-ca3b2c5e33d1)

---

