/* display.c
* GitHub: https:\\github.com\tommojphillips
*/

#include <stdint.h>
#include <stdio.h>

#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_image.h"

#include "display.h"
#include "chip8_sdl2.h"
#include "chip8.h" // chip8 cpu core

SDL_STATE sdl = { 0 };
WINDOW_STATS* window_stats = NULL;
WINDOW_STATE* window_state = NULL;

void input_process_event();
void imgui_process_event();

static void resize_display_keep_aspect_ratio();
static void set_default_settings();
static void draw_display_buffer();
static void display_process_event();

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
void sdl_create_window() {

	sdl.game_window = SDL_CreateWindow("Chip-8",
		window_state->win_x, window_state->win_y, 
		window_state->win_w, window_state->win_h,
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
void sdl_update() {
	while (SDL_PollEvent(&sdl.e)) {

		display_process_event();
		input_process_event();
		imgui_process_event();

		switch (sdl.e.type) {

			case SDL_DROPFILE:
				load_program(sdl.e.drop.file);
				break;
		}
	}
}
void sdl_render() {
	SDL_RenderPresent(sdl.game_renderer);
	SDL_SetRenderDrawColor(sdl.game_renderer, chip8_config.off_color.r, chip8_config.off_color.g, chip8_config.off_color.b, 0xFF);
	SDL_RenderClear(sdl.game_renderer);
	draw_display_buffer();
}

static void set_default_settings() {

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
static void display_process_event() {
	switch (sdl.e.type) {

	case SDL_QUIT:
		window_state->window_open = 0;
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
static void draw_display_buffer() {
	SDL_Rect px = { 0 };
	for (int i = 0; i < CHIP8_NUM_PIXELS; ++i) {
		px.x = PX_X(i);
		px.y = PX_Y(i);
		px.w = PX_H;
		px.h = PX_W;

		if (CHIP8_DISPLAY_GET_PX(window_state->display_buffer, i)) {
			SDL_SetRenderDrawColor(sdl.game_renderer,
				chip8_config.on_color.r, chip8_config.on_color.g,
				chip8_config.on_color.b, 0xFF);
		}
		else {
			SDL_SetRenderDrawColor(sdl.game_renderer,
				chip8_config.off_color.r, chip8_config.off_color.g,
				chip8_config.off_color.b, 0xFF);
		}

		SDL_RenderFillRect(sdl.game_renderer, &px);
	}
}

