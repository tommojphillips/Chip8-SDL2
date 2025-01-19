## Chip8 Interpreter

A Chip-8 Interpreter For windows written in C using SDL2 and IMGUI

![tommojphillips_chip8_pixel_art](https://github.com/user-attachments/assets/9d9008cc-10ca-44aa-9652-2beed2d306e2)

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
 - *Drag n drop* c8 files onto the window to load them
 - Launch a program using: `Chip8.exe <c8_file>

 ---

 ### Dependencies
 
The only dependencies are IMGUI, SDL2 and SDL2-image libraries
  - Chip8 Core - https://github.com/tommojphillips/Chip8-Core
  - IMGUI v1.91.6 - https://github.com/ocornut/imgui/releases/tag/v1.91.6
  - SDL2 v2.30.10 - https://github.com/libsdl-org/SDL/releases/tag/release-2.30.10
  - SDL2-Image v2.8.4 - https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.4

  ---

### Building

The project is built in Visual Studio 2022

  1. Clone the repo  `git clone https://github.com/tommojphillips/Chip8-SDL2.git`

  2. Clone Chip8-Core into `Chip8-SDL2/Chip-Core` using `git clone https://github.com/tommojphillips/Chip8-Core.git`

  3. Download SDL2 Library [`SDL2-devel-X.X.X-VC.zip`](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.10)
     - Extract and copy the `include` and `lib` folders into `Chip8-SDL2/SDL2`

  4. Download SDL2 Image Library [`SDL2_Image-devel-X.X.X-VC.zip`](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.4)
     - Extract the `include` and `lib` folders into `Chip8-SDL2/SDL2`
  
  5. Clone IMGUI into `Chip8-SDL2/imgui` using `git clone --depth 1 --branch v1.91.6 https://github.com/ocornut/imgui.git`

The project directory structure should look like this:

```
Chip8-SDL2\
      |
      | -- Chip8-Core\
      |               | -- chip8.h
      |               | -- chip8.c
      |               | -- chip8_defines.h
      |               | -- chip8_mnem.c
      |
      | -- imgui\
      |          | -- backends\
      |
      | -- SDL2\
      |         | -- include\
      |         | -- lib\
      |
      | -- SDL2_Image\
      |               | -- include\
      |               | -- lib\
      | 
      | -- src\
      |
      | -- Chip8.sln
      | -- Chip8.vcxproj

```

 ---

#### Sources
 - [Chip 8 on the COSMAC VIP](https://www.laurencescotford.net/2020/07/25/chip-8-on-the-cosmac-vip-instruction-index/) by Laurence Scotford
 - [Chip8 Test Suite](https://github.com/Timendus/chip8-test-suite) by Timendus
 - [Chip8 Links](https://chip-8.github.io/links/)

 ---

#### Screenshots

Space Invaders
![Screenshot 2025-01-01 105501](https://github.com/user-attachments/assets/9f6ca31a-438c-4077-8d84-8d0d98cf2e6b)
![Screenshot 2025-01-01 105413](https://github.com/user-attachments/assets/e5d8cce6-d1a7-47b3-a273-ca3b2c5e33d1)

---

