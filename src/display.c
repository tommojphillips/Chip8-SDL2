/* window_sdl2.c
* GitHub: https:\\github.com\tommojphillips
*/

#include <stdint.h>
#include <stdio.h>

#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_image.h"

#include "display.h"
#include "ui.h"
#include "chip8_sdl2.h"
#include "chip8.h" // chip8 cpu core

SDL_STATE sdl = { 0 };
WINDOW_STATS* window_stats = NULL;
WINDOW_STATE* window_state = NULL;

static void keypad_input(uint8_t v);
static void system_input();
static void resize_display_keep_aspect_ratio();

static void set_default_settings() {

	/* Window State */
	window_state->win_x = SDL_WINDOWPOS_CENTERED;
	window_state->win_y = SDL_WINDOWPOS_CENTERED;
	window_state->last_window_state = 0;

	SDL_DisplayMode display_mode;
	if (SDL_GetCurrentDisplayMode(0, &display_mode) == 0) {
		window_state->win_w = (int)(display_mode.w * 0.60f);
		window_state->win_h = (int)(display_mode.h * 0.60f);
	}
	else {
		window_state->win_w = 640;
		window_state->win_h = 480;
	}
}

// Init SDL2
void sdl_init() {

	window_stats = (WINDOW_STATS*)malloc(sizeof(WINDOW_STATS));
	if (window_stats == NULL) {
		printf("Failed to allocate window stats\n");
		exit(1);
	}
	memset(window_stats, 0, sizeof(WINDOW_STATS));

	window_state = (WINDOW_STATE*)malloc(sizeof(WINDOW_STATE));
	if (window_state == NULL) {
		printf("Failed to allocate window state\n");
		exit(1);
	}
	memset(window_state, 0, sizeof(WINDOW_STATE));

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to initialize SDL\n");
		exit(1);
	}

	set_default_settings();
}

// Create window SDL2
void sdl_create_window() {

	sdl.game_window = SDL_CreateWindow("Chip-8",
		window_state->win_x, window_state->win_y, window_state->win_w, window_state->win_h,
		SDL_WINDOW_RESIZABLE | window_state->last_window_state);

	if (sdl.game_window == NULL) {
		printf("Failed to create game window\n");
		exit(1);
	}

	sdl.game_renderer = SDL_CreateRenderer(sdl.game_window, -1, 0);
	if (sdl.game_renderer == NULL) {
		printf("Failed to create game renderer\n");
		exit(1);
	}

	// Window icon
	sdl.icon_surface = IMG_Load("icon.ico");
	if (sdl.icon_surface != NULL) {
		SDL_SetWindowIcon(sdl.game_window, sdl.icon_surface);
	}
	else {
		printf("Failed to load icon.ico\n");
	}


	window_state->window_open = 1;
}

// Destroy SDL2
void sdl_destroy() {

	if (window_stats != NULL) {
		free(window_stats);
		window_stats = NULL;
	}

	if (window_state != NULL) {
		free(window_state);
		window_state = NULL;
	}

	if (sdl.icon_surface != NULL) {
		SDL_FreeSurface(sdl.icon_surface);
		sdl.icon_surface = NULL;
	}

	if (sdl.game_renderer != NULL) {
		SDL_DestroyRenderer(sdl.game_renderer);
		sdl.game_renderer = NULL;
	}

	if (sdl.game_window != NULL) {
		SDL_DestroyWindow(sdl.game_window);
		sdl.game_window = NULL;
	}

	SDL_Quit();
}

// SDL2 update
void sdl_update() {
	while (SDL_PollEvent(&sdl.e) && window_state->window_open) {

		imgui_process_event(&sdl.e);

		switch (sdl.e.type) {

			case SDL_QUIT:
				window_state->window_open = 0;
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

			case SDL_WINDOWEVENT:
				switch (sdl.e.window.event) {
					
					case SDL_WINDOWEVENT_RESIZED:
						window_state->last_win_w = window_state->win_w;
						window_state->last_win_h = window_state->win_h;
						SDL_GetWindowSize(sdl.game_window, &window_state->win_w, &window_state->win_h);
						resize_display_keep_aspect_ratio();
						break;

					case SDL_WINDOWEVENT_MOVED:
						window_state->last_win_x = window_state->win_x;
						window_state->last_win_y = window_state->win_y;
						SDL_GetWindowPosition(sdl.game_window, &window_state->win_x, &window_state->win_y);
						break;

					case SDL_WINDOWEVENT_MAXIMIZED:
						window_state->last_window_state = SDL_WINDOW_MAXIMIZED;
						break;

					case SDL_WINDOWEVENT_RESTORED:
						window_state->last_window_state = 0;
						break;
				}
		}
	}
}

// Render SDL2
void sdl_render() {

	SDL_RenderPresent(sdl.game_renderer);
	SDL_SetRenderDrawColor(sdl.game_renderer, chip8_config.off_color.r, chip8_config.off_color.g, chip8_config.off_color.b, 0xFF);
	SDL_RenderClear(sdl.game_renderer);
}

static void keypad_input(uint8_t v) {
	switch (sdl.e.key.keysym.sym) {
		case SDLK_1:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x1, v);
			break;
		case SDLK_2:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x2, v);
			break;
		case SDLK_3:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x3, v);
			break;
		case SDLK_4:
			CHIP8_KEYPAD_SET(chip8->keypad, 0xD, v);
			break;

		case SDLK_q:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x4, v);
			break;
		case SDLK_w:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x5, v);
			break;
		case SDLK_e:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x6, v);
			break;
		case SDLK_r:
			CHIP8_KEYPAD_SET(chip8->keypad, 0xC, v);
			break;

		case SDLK_a:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x7, v);
			break;
		case SDLK_s:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x8, v);
			break;
		case SDLK_d:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x9, v);
			break;
		case SDLK_f:
			CHIP8_KEYPAD_SET(chip8->keypad, 0xE, v);
			break;

		case SDLK_z:
			CHIP8_KEYPAD_SET(chip8->keypad, 0xA, v);
			break;
		case SDLK_x:
			CHIP8_KEYPAD_SET(chip8->keypad, 0x0, v);
			break;
		case SDLK_c:
			CHIP8_KEYPAD_SET(chip8->keypad, 0xB, v);
			break;
		case SDLK_v:
			CHIP8_KEYPAD_SET(chip8->keypad, 0xF, v);
			break;
	}
}
static void system_input() {
	if (sdl.e.key.keysym.mod & KMOD_LCTRL) {

		switch (sdl.e.key.keysym.sym) {
			case SDLK_r: { // RESET
				chip8_reset();
			} break;
		}
	}

	switch (sdl.e.key.keysym.sym) {

		case SDLK_SPACE: { // PAUSE
			if (chip8->cpu_state == CHIP8_STATE_EXE) {
				chip8->cpu_state = CHIP8_STATE_HLT;
			}
			else {
				chip8->cpu_state = CHIP8_STATE_EXE;
			}
		} break;

		case SDLK_KP_PLUS: // INC SPEED
			if (chip8_config.cpu_target < 0xFFFFFF) {
				chip8_config.cpu_target += 1;
			}
			break;

		case SDLK_KP_MINUS: // DEC SPEED
			if (chip8_config.cpu_target > 0) {
				chip8_config.cpu_target -= 1;
			}
			break;

		case SDLK_ESCAPE: { // MENU
			imgui_toggle_menu();
		} break;

		case SDLK_RETURN:
		case SDLK_KP_ENTER: { // STEP
			if (chip8->cpu_state == CHIP8_STATE_HLT) {
				chip8_state.single_step = SINGLE_STEP_EXE;
			}
		} break;

		case SDLK_F11: { // Full Screen
			if (window_state->last_window_state != SDL_WINDOW_FULLSCREEN_DESKTOP) {
				window_state->last_window_state = SDL_WINDOW_FULLSCREEN_DESKTOP;
				SDL_SetWindowFullscreen(sdl.game_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
			else {
				window_state->last_window_state = 0;
				SDL_SetWindowFullscreen(sdl.game_window, 0);
			}
		} break;
	}
}

static void resize_display_keep_aspect_ratio() {

	int win_w = (int)(window_state->win_w * chip8_config.win_s);
	int win_h = (int)(window_state->win_h * chip8_config.win_s);

	double ratioX = win_w / (double)DISPLAY_WIDTH;
	double ratioY = win_h / (double)DISPLAY_HEIGHT;
	double ratio = ratioX < ratioY ? ratioX : ratioY;

	/* maintain chip8 display window pos */
	chip8_config.win_w = (int)(DISPLAY_WIDTH * ratio);
	chip8_config.win_h = (int)(DISPLAY_HEIGHT * ratio);
}
