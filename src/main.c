/* main.c
* GitHub: https:\\github.com\tommojphillips 
*/

#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "ui.h"
#include "display.h"
#include "chip8_sdl2.h"
#include "chip8.h" // chip8 cpu core
#include "main.h"

void loadini_init();

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

	loadini_init();
	sdl_init();
	imgui_init();
	chip8_init();

	process_command_line(argc, argv);

	while (window_state.window_open) {
		
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

	// Cleanup
	chip8_destroy();
	imgui_destroy();
	sdl_destroy();

	return 0;
}
