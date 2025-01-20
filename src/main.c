/* main.c
* GitHub: https:\\github.com\tommojphillips 
*/

#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "ui.h"
#include "chip8_sdl2.h"
#include "chip8.h" // chip8 cpu core
#include "display.h"

void loadini_init(); 
void loadini_destroy();
void loadini_save_settings();
void loadini_load_settings();

static void process_command_line(int argc, char* argv[]) {
	if (argc > 1) {
		load_program(argv[1]);
	}
	else {
		chip8->cpu_state = CHIP8_STATE_HLT;
	}
}

void start_frame() {
	window_stats->start_frame_time = SDL_GetPerformanceCounter();
}
void end_frame() {
	window_stats->end_frame_time = SDL_GetPerformanceCounter();
	window_stats->frame_ticks = window_stats->end_frame_time - window_stats->start_frame_time;
	window_stats->delta_time = window_stats->frame_ticks / (double)SDL_GetPerformanceFrequency() * 1000.0;
}

int main(int argc, char* argv[]) {

	sdl_init();
	imgui_init();
	chip8_init();
	loadini_init();

	loadini_load_settings();
	process_command_line(argc, argv); 
	
	sdl_create_window();
	imgui_create_renderer();

	while (window_state->window_open) {
		
		start_frame();

		sdl_update();
		imgui_update();
		chip8_update();
		
		const double render_duration = (1000.0 / chip8_config.render_target);
		window_stats->render_elapsed_time += window_stats->delta_time;
		window_stats->render_fps = 1.0 / window_stats->render_elapsed_time * 1000.0;
		if (render_duration < window_stats->render_elapsed_time) {
			window_stats->render_elapsed_time -= render_duration;
			window_stats->instructions_per_frame = 0;
				sdl_render();
				chip8_render(chip8);
		}

		end_frame();
	}

	loadini_save_settings();

	// Cleanup
	loadini_destroy();
	chip8_destroy();
	imgui_destroy();
	sdl_destroy();

	return 0;
}
