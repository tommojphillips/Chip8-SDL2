/* window_sdl2.h
* GitHub: https:\\github.com\tommojphillips
*/

#ifndef WINDOW_SDL2_H
#define WINDOW_SDL2_H

#include <stdint.h>

#include "SDL.h"

/* SDL state */
typedef struct {
	SDL_Window* game_window;
	SDL_Renderer* game_renderer;
	SDL_Surface* icon_surface;
	SDL_Event e;
} SDL_STATE;

/* Window state */
typedef struct {
	int window_open;
	int win_x;
	int win_y;
	int win_h;
	int win_w;
	int last_win_x;
	int last_win_y;
	int last_win_h;
	int last_win_w;
	int last_window_state;
} WINDOW_STATE;

/* Window stats */
typedef struct {
	double delta_time;
	double render_fps;

	double render_elapsed_time;
	double cpu_elapsed_time;
	double timer_elapsed_time;

	uint64_t start_frame_time;
	uint64_t end_frame_time;
	uint64_t frame_ticks;	

	int instructions_per_frame;
	int single_step_instruction_count;
} WINDOW_STATS;

#ifdef __cplusplus
extern "C" {
#endif

extern SDL_STATE sdl;
extern WINDOW_STATS* window_stats;
extern WINDOW_STATE* window_state;

/* SDL2 Init */
void sdl_init();

/* SDL2 Create window */
void sdl_create_window();

/* SDL2 Destroy */
void sdl_destroy();

/* SDL2 Update */
void sdl_update();

/* SDL2 Render */
void sdl_render();

#ifdef __cplusplus
};
#endif

#endif
