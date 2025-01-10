// main.c
//
// GitHub: https:\\github.com\tommojphillips

// Define OUTPUT_MNEM for debug mnemonics
//#define OUTPUT_MNEM

// Define CYLCE_COUNT for limiting emulation by instuctions per frame
// Undefine CYCLE_COUNT for limiting emulation by clock frequency in hz
//#define CYCLE_COUNT

// std
#include <stdint.h>
#include <stdlib.h> // for rand()
#include <stdio.h>
#include <malloc.h>
#include <string.h>

// SDL
#include "SDL.h"
#include "SDL_image.h"

#include "chip8.h"

#ifdef OUTPUT_MNEM
#include "chip8_mnem.h"
#endif

/* SDL Window state */
typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event e;
	int window_open;
	SDL_Surface* icon_surface;
} SDL_STATE;

/* Chip8 tile color */
typedef struct {
	uint8_t r;
	uint8_t b;
	uint8_t g;
	uint8_t a;
} TILE_COLOR;

/* Chip8 window configuration */
typedef struct {
#ifdef CYCLE_COUNT
	// Instruction target in fpi
	uint32_t instructions_per_frame_target; 
#else
	//  Clock frequency in hz
	uint32_t clock_target;
#endif

	// Display target in fps
	uint32_t render_target;

	TILE_COLOR pixel_on_color;
	TILE_COLOR pixel_off_color;
} CHIP8_WINDOW_CONFIG;

/* Chip8 window stats */
typedef struct {
	double cpu_elasped_time;
	double render_elasped_time;
	double delta_time;
	uint64_t current_frame_time;
	uint64_t last_frame_time;
	uint32_t instructions_per_frame;
} CHIP8_WINDOW_STATS;

SDL_STATE sdl = { 0 };
CHIP8 chip8 = { 0 };
CHIP8_WINDOW_STATS stats = { 0 };
CHIP8_WINDOW_CONFIG config = { 0 };

int load_program(const char* filename) {

	chip8_reset(&chip8);
	chip8_zero_program_memory(&chip8);

	uint32_t size = 0;	
	FILE* file = NULL;
	fopen_s(&file, filename, "rb");
	if (file == NULL) {
		printf("Error: could not open file: %s\n", filename);
		chip8.cpu_state = CHIP8_STATE_HALT;
		return 1;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_ADDR < size) {
		fclose(file);
		printf("Error: program too big\n");
		chip8.cpu_state = CHIP8_STATE_HALT;
		return 1;
	}
	
	fread(chip8.ram + CHIP8_PROGRAM_ADDR, 1, size, file);
	fclose(file);
	printf("loaded %s (%d bytes) into RAM at 0x%x\n", filename, size, CHIP8_PROGRAM_ADDR);
	return 0;
}

void cal_delta_time() {
	stats.last_frame_time = stats.current_frame_time;
	stats.current_frame_time = SDL_GetPerformanceCounter();
	stats.delta_time = (stats.current_frame_time - stats.last_frame_time) * 1000.0 / (double)SDL_GetPerformanceFrequency();
}

/* CHIP8 */
void chip8_render(CHIP8* chip8) {
#define WIDTH 64
#define HEIGHT 32
#define PIXEL_SIZE (win_width / WIDTH)
#define WIDTH_OFFSET (( win_width - ( WIDTH * PIXEL_SIZE ) ) / 2)
#define HEIGHT_OFFSET (( win_height - ( HEIGHT * PIXEL_SIZE ) ) / 2)
	uint8_t x, y;
	SDL_Rect rect = { 0 };
	int win_width = 0, win_height = 0;

	SDL_GetWindowSize(sdl.window, &win_width, &win_height);
		
	for (x = 0; x < WIDTH; ++x) {
		for (y = 0; y < HEIGHT; ++y) {
			
			rect.x = WIDTH_OFFSET + (x * PIXEL_SIZE);
			rect.y = HEIGHT_OFFSET + (y * PIXEL_SIZE);
			rect.w = PIXEL_SIZE;
			rect.h = PIXEL_SIZE;

			if (chip8->video[x+(y * WIDTH)]) {
				SDL_SetRenderDrawColor(sdl.renderer,
					config.pixel_on_color.r,
					config.pixel_on_color.g,
					config.pixel_on_color.b,
					config.pixel_on_color.a);
			}
			else {
				SDL_SetRenderDrawColor(sdl.renderer,
					config.pixel_off_color.r,
					config.pixel_off_color.g,
					config.pixel_off_color.b,
					config.pixel_off_color.a);
			}

			SDL_RenderFillRect(sdl.renderer, &rect);
		}
	}
	SDL_RenderPresent(sdl.renderer);
}
void chip8_beep(CHIP8* chip8) {
	printf("BEEP\n");
}
uint8_t chip8_random() {
	return (rand() % 256);
}

int chip8_emulate_cycle() {
#ifdef CYCLE_COUNT
	if (stats.instructions_per_frame < config.instructions_per_frame_target) {
		stats.instructions_per_frame++;
		int result = chip8_execute(&chip8);
		if (result != 0) {
			return result;
		}
	}
#else
	const double cpu_duration = (1.0 / config.clock_target) * 1000.0;
	stats.cpu_elasped_time += stats.delta_time;
	if (cpu_duration < stats.cpu_elasped_time) {
		stats.cpu_elasped_time = 0;
		int result = chip8_execute(&chip8);
		if (result != 0) {
			return result;
		}
	}
#endif

	const double render_duration = (1.0 / config.render_target) * 1000.0;
	stats.render_elasped_time += stats.delta_time;
	if (render_duration < stats.render_elasped_time) {
		stats.render_elasped_time = 0;
		stats.instructions_per_frame = 0;
		chip8_render(&chip8);
		chip8_step_timers(&chip8);
	}

	return 0;
}
void chip8_set_config() {
#ifdef CYCLE_COUNT
	config.instructions_per_frame_target = 540 / 60;
#else
	config.clock_target = 500;
#endif
	config.render_target = 60;

	config.pixel_on_color.r = 255;
	config.pixel_on_color.g = 204;
	config.pixel_on_color.b = 102;
	config.pixel_on_color.a = 0xFF;

	config.pixel_off_color.r = 0x0;
	config.pixel_off_color.g = 0x0;
	config.pixel_off_color.b = 0x0;
	config.pixel_off_color.a = 0x0;
}
void chip8_update() {

	int result = chip8_emulate_cycle();
	switch (result) {
		case 0:
			break;

		case 1:
			printf("Error unknown opcode at pc: %x, op: %x\n", chip8.pc, chip8.opcode);
			chip8.cpu_state = CHIP8_STATE_HALT;
			break;

		default:
			printf("Error %x\n", result);
			chip8.cpu_state = CHIP8_STATE_HALT;
			break;
	}

#ifdef OUTPUT_MNEM
	CHIP8_MNEM mnem = { 0 };
	mnem.chip8 = chip8;
	mnem.str[0] = '\0';
	chip8_mnem(&mnem);
	if (mnem.str[0] != '\0') {
		printf("%s\n", mnem.str);
	}
#endif
}

/* INPUT */
void keypad_input(uint8_t v) {
#define SET_KEY(n, v) chip8.keypad = (chip8.keypad & ~(0x1U << n)) | (v << n)
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
void system_input() {
	if (sdl.e.key.keysym.mod & KMOD_LCTRL) {
		switch (sdl.e.key.keysym.sym) {
			case SDLK_r: { // RESET - CTRL + R
				chip8_reset(&chip8);
				printf("RESET\n");
			} break;
		}
	}

	switch (sdl.e.key.keysym.sym) {
		case SDLK_SPACE: { // PAUSE / UNPAUSE - SPACE
			if (chip8.cpu_state == CHIP8_STATE_RUN) {
				chip8.cpu_state = CHIP8_STATE_HALT;
				printf("PAUSE\n");
			}
			else {
				chip8.cpu_state = CHIP8_STATE_RUN;
				printf("UNPAUSE\n");
			}
		} break;
#ifdef CYCLE_COUNT
		case SDLK_KP_PLUS: { // INC SPEED
			if (config.instructions_per_frame_target < 0xFFFFFF) {
				printf("CYCLE TARGET: %u / frame\n", config.instructions_per_frame_target += 1);
			}
		} break;
		case SDLK_KP_MINUS: { // DEC SPEED
			if (config.instructions_per_frame_target > 0) {
				printf("CYCE TARGET: %u / frame\n", config.instructions_per_frame_target -= 1);
			}
		} break;			  
#else
		case SDLK_KP_PLUS: { // INC SPEED
			if (config.clock_target < 0xFFFFFF) {
				printf("CLOCK TARGET: %u hz\n", config.clock_target += 1);
			}
		} break;
		case SDLK_KP_MINUS: { // DEC SPEED
			if (config.clock_target > 0) {
				printf("CLOCK TARGET: %u hz\n", config.clock_target -= 1);
			}
		} break;
#endif
	}
}

/* SDL */
void sdl_init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to initialize SDL\n");
		exit(1);
	}

	sdl.window = SDL_CreateWindow("Chip-8", 100, 100, 660, 340, SDL_WINDOW_RESIZABLE);
	if (sdl.window == NULL) {
		printf("Failed to create window\n");
		exit(1);
	}

	sdl.icon_surface = IMG_Load("icon.ico");
	if (sdl.icon_surface != NULL) {
		SDL_SetWindowIcon(sdl.window, sdl.icon_surface);
	}
	else {
		printf("Failed to load icon.ico\n");
	}

	sdl.renderer = SDL_CreateRenderer(sdl.window, -1, 0);
	if (sdl.renderer == NULL) {
		printf("Failed to create renderer\n");
		exit(1);
	}

	sdl.window_open = 1;
}
void sdl_destroy() {

	if (sdl.icon_surface != NULL) {
		SDL_FreeSurface(sdl.icon_surface);
		sdl.icon_surface = NULL;
	}

	if (sdl.renderer != NULL) {
		SDL_DestroyRenderer(sdl.renderer);
		sdl.renderer = NULL;
	}

	if (sdl.window != NULL) {
		SDL_DestroyWindow(sdl.window);
		sdl.window = NULL;
	}

	SDL_Quit();
}
void sdl_update() {
	while (SDL_PollEvent(&sdl.e)) {

		switch (sdl.e.type) {

			case SDL_QUIT:
				sdl.window_open = 0;
				break;

			case SDL_DROPFILE:
				load_program(sdl.e.drop.file);
				break;

			case SDL_KEYDOWN:
				keypad_input(CHIP8_KEY_STATE_KEY_DOWN);
				system_input();
				break;

			case SDL_KEYUP:
				keypad_input(CHIP8_KEY_STATE_KEY_UP);
				break;
		}
	}
}

/* IMGUI */
int main(int argc, char* argv[]) {

	sdl_init();

	chip8_set_config();
	chip8_init(&chip8);
	if (argc > 1) {
		load_program(argv[1]);
	}
	else {
		chip8.cpu_state = CHIP8_STATE_HALT; // start halted; wait for user to load program.
	}

	while (sdl.window_open) {
		cal_delta_time();
		sdl_update();

		if (chip8.cpu_state == CHIP8_STATE_RUN) {
			chip8_update();
		}
	}

	// Cleanup

	sdl_destroy();

	return 0;
}
