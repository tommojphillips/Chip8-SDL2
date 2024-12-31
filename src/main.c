// main.c
//
// GitHub: https:\\github.com\tommojphillips

#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "SDL.h"

#include "chip8.h"

#include "file.h"

typedef struct _SDL_STATE {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event e;
	int window_open;
} SDL_STATE;

SDL_STATE sdl;

/* CHIP8 */
int chip8_load_file(CHIP8* chip8, const char* file) {

	uint32_t size = 0;
	int result = readFileIntoBuffer(file, chip8->ram + CHIP8_PROGRAM_ADDR, (CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_ADDR), &size, 0);
	if (result == 0) {
		printf("loaded %s (%d bytes) into RAM at 0x%x\n", file, size, CHIP8_PROGRAM_ADDR);
	}

	return result;
}

/* CHIP8 DISPLAY */
void chip8_render_display(CHIP8* chip8) {

	uint32_t* pixels;
	int pitch;
	int x = 0;
	int y = 0;

	SDL_LockTexture(sdl.texture, NULL, &pixels, &pitch);
	for (int i = 0; i < CHIP8_GFX_SIZE; ++i) {
		uint32_t val = chip8->video[i] ? -1 : 0;
		pixels[(128 * (2 * y + 0) + (2 * x + 0))] = val;
		pixels[(128 * (2 * y + 0) + (2 * x + 1))] = val;
		pixels[(128 * (2 * y + 1) + (2 * x + 0))] = val;
		pixels[(128 * (2 * y + 1) + (2 * x + 1))] = val;
		if (++x == 64) {
			x = 0;
			y++;
		}
	}
	SDL_UnlockTexture(sdl.texture);
	SDL_RenderClear(sdl.renderer);
	SDL_RenderCopy(sdl.renderer, sdl.texture, NULL, NULL);
	SDL_RenderPresent(sdl.renderer);
}

/* CHIP8 INPUT */
void chip8_keypad_input(CHIP8* chip8, uint8_t v) {
#define SET_KEY(n, v) chip8->keypad = (chip8->keypad & ~(0x1U << n)) | (v << n)
	switch (sdl.e.key.keysym.sym) {

		case SDLK_1:
			SET_KEY(1, v);
			break;
		case SDLK_2:
			SET_KEY(2, v);
			break;
		case SDLK_3:
			SET_KEY(3, v);
			break;
		case SDLK_4:
			SET_KEY(12, v);
			break;

		case SDLK_q:
			SET_KEY(4, v);
			break;
		case SDLK_w:
			SET_KEY(5, v);
			break;
		case SDLK_e:
			SET_KEY(6, v);
			break;
		case SDLK_r:
			SET_KEY(13, v);
			break;

		case SDLK_a:
			SET_KEY(7, v);
			break;
		case SDLK_s:
			SET_KEY(8, v);
			break;
		case SDLK_d:
			SET_KEY(9, v);
			break;
		case SDLK_f:
			SET_KEY(14, v);
			break;

		case SDLK_z:
			SET_KEY(10, v);
			break;
		case SDLK_x:
			SET_KEY(0, v);
			break;
		case SDLK_c:
			SET_KEY(11, v);
			break;
		case SDLK_v:
			SET_KEY(15, v);
			break;
	}
}
void chip8_sys_input(CHIP8* chip8) {

	if (sdl.e.key.keysym.mod & KMOD_LCTRL) {
		switch (sdl.e.key.keysym.sym) {
			case SDLK_r: { // RESET - CTRL + R
				chip8_reset(chip8);
			} break;
		}
	}

	switch (sdl.e.key.keysym.sym) {
		case SDLK_SPACE: { // PAUSE / UNPAUSE - SPACE
			if (chip8->cpu_state == CHIP8_STATE_RUNNING)
				chip8->cpu_state = CHIP8_STATE_HALT;
			else
				chip8->cpu_state = CHIP8_STATE_RUNNING;
		} break;
	}
}

/* SDL */
void init_sdl() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to initialize SDL\n");
		exit(1);
	}

	sdl.window = SDL_CreateWindow("Chip-8", 100, 100, 640, 320, SDL_WINDOW_RESIZABLE);
	if (sdl.window == NULL) {
		printf("Failed to create window\n");
		exit(1);
	}

	sdl.renderer = SDL_CreateRenderer(sdl.window, -1, SDL_RENDERER_ACCELERATED);
	if (sdl.renderer == NULL) {
		printf("Failed to create renderer\n");
		exit(1);
	}

	sdl.texture = SDL_CreateTexture(sdl.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 128, 64);
	if (sdl.texture == NULL) {
		printf("Failed to create screen texture\n");
		exit(1);
	}

	sdl.window_open = 1;
}
void destroy_sdl() {

	SDL_DestroyTexture(sdl.texture);
	sdl.texture = NULL;

	SDL_DestroyRenderer(sdl.renderer);
	sdl.window = NULL;

	SDL_DestroyWindow(sdl.window);
	sdl.renderer = NULL;

	SDL_Quit();
}
void update_sdl(CHIP8* chip8) {
	while (SDL_PollEvent(&sdl.e)) {

		switch (sdl.e.type) {

			case SDL_QUIT:
				sdl.window_open = 0;
				break;

			case SDL_DROPFILE:
				chip8_reset(chip8);
				chip8_zero_program_memory(chip8);
				int result = chip8_load_file(chip8, sdl.e.drop.file);
				if (result != 0) {
					chip8->cpu_state = CHIP8_STATE_HALT;
				}
				break;

			case SDL_KEYDOWN:
				chip8_keypad_input(chip8, CHIP8_KEY_STATE_KEY_DOWN);
				chip8_sys_input(chip8);
				break;

			case SDL_KEYUP:
				chip8_keypad_input(chip8, CHIP8_KEY_STATE_KEY_UP);
				break;
		}
	}

	SDL_UpdateWindowSurface(sdl.window);
}

int main(int argc, char* argv[]) {

	int result = 0;
	CHIP8* chip8 = NULL;
	uint8_t* program = NULL;

	init_sdl();

	// Chip8
	chip8 = (CHIP8*)malloc(sizeof(CHIP8));
	if (chip8 == NULL) {
		printf("Failed to allocate chip8 instance\n");
		exit(1);
	}

	program = (uint8_t*)malloc(CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_ADDR);
	if (program == NULL) {
		printf("Failed to allocate program memory\n");
		exit(1);
	}

	chip8_init(chip8);
	if (argc > 1) {
		result = chip8_load_file(chip8, argv[1]);
		if (result != 0) {
			chip8->cpu_state = CHIP8_STATE_HALT;
		}
	}

	// Window Loop
	while (sdl.window_open) {
		
		update_sdl(chip8);
				
		if (chip8->cpu_state == CHIP8_STATE_RUNNING) {
						
			result = chip8_emulate_cycle(chip8);
			switch (result) {
				case 1:
					printf("Error unknown opcode at pc: %x, op: %x\n", chip8->pc, chip8->opcode);
					chip8->cpu_state = CHIP8_STATE_HALT;
					break;
			}

			chip8->mnem[0] = '\0';
			chip8_mnem(chip8);
			if (chip8->mnem[0] != '\0') {
				printf("%s\n", chip8->mnem);
			}
		}
	}

	// Cleanup

	free(chip8);
	chip8 = NULL;

	free(program);
	program = NULL;

	destroy_sdl(&sdl);

	return result;
}
