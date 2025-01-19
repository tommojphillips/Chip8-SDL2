## Chip8 Interpreter

A Chip-8 Interpreter For windows written in C using SDL2 and IMGUI

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

  1. Clone the repo  
  
  ```
  git clone https://github.com/tommojphillips/Chip8-SDL2.git
  ```
  
  2. CD
  
  ```
  cd Chip8-SDL2
  ```

  3. Clone Chip8-Core into `Chip8-SDL2/Chip-Core`
  
  ```
  git clone https://github.com/tommojphillips/Chip8-Core.git
  ```
  
  4. Clone imgui into `Chip8-SDL2/imgui` 
  
  ``` 
  git clone --depth 1 --branch v1.91.6 https://github.com/ocornut/imgui.git
  ```
  
  5. Clone imgui-club into `Chip8-SDL2/imgui-club`

  ```
  git clone https://github.com/ocornut/imgui_club.git
  ```
 
  6. Download SDL2 Library [`SDL2-devel-X.X.X-VC.zip`](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.10)
     - Extract and copy the `include` and `lib` folders into `Chip8-SDL2/SDL2` you will have to create the `SDL2` directory

  7. Download SDL2 Image Library [`SDL2_Image-devel-X.X.X-VC.zip`](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.4)
     - Extract the `include` and `lib` folders into `Chip8-SDL2/SDL2_Image` you will have to create the `SDL2_Image` directory
  
  8. Open .Sln in visual studio and build and run

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

Space Invaders (Pixel Spacing = 0)

![Screenshot 2025-01-19 231610](https://github.com/user-attachments/assets/c8299064-18e4-4bf7-b837-81077d468485)

Space Invaders (Pixel Spacing = 2)

![Screenshot 2025-01-19 231629](https://github.com/user-attachments/assets/2d0b2e43-5b5d-4287-957a-dbcf3392b2f7)

Space Invaders (Pixel Spacing = 4)

![Screenshot 2025-01-19 232304](https://github.com/user-attachments/assets/4faddd25-67b4-482c-9a52-c8ec5513c764)

Window / Display Settings

![Screenshot 2025-01-19 231706](https://github.com/user-attachments/assets/e1449ad4-39c9-4625-be24-b2c752d1e11b)

Chip8 Quirks / Setitngs

![Screenshot 2025-01-19 231715](https://github.com/user-attachments/assets/21c1fd50-50ca-437b-89a1-1829e78ae2bc)

Display editor ( click pixel to toggle )

![Screenshot 2025-01-19 231835](https://github.com/user-attachments/assets/1cf31491-5b22-495c-ad94-2e05b8cc30b8)

Display RAM editor 

![Screenshot 2025-01-19 231846](https://github.com/user-attachments/assets/1d8ac9ec-cf69-49d1-a09e-d3ebffb5bc5a)

Instruction Debug & RAM editor

![Screenshot 2025-01-19 231859](https://github.com/user-attachments/assets/d448ba06-5c5e-4a8b-8f6c-c1827a7268f2)

---

