/* chip8_sdl2.c
* GitHub: https:\\github.com\tommojphillips
*/

#include <stdlib.h> // for rand()
#include <stdio.h>
#include <time.h>
#include <malloc.h>

#include "chip8_sdl2.h"
#include "chip8.h" // chip8 cpu core
#include "display.h"

CHIP8* chip8 = NULL;
CHIP8_CONFIG chip8_config = { 0 };
CHIP8_STATE chip8_state = { 0 };

static void chip8_emulate_cycle();
static void chip8_single_step_cycle();

static void set_default_settings() {
	/* Chip8 State */
	chip8_config.cpu_target = 540; // 540hz
	chip8_config.timer_target = 60; // 60hz
	chip8_config.render_target = 60; // 60hz

	chip8_config.on_color.r = 100;
	chip8_config.on_color.g = 255;
	chip8_config.on_color.b = 105;

	chip8_config.off_color.r = 0x0;
	chip8_config.off_color.g = 0x0;
	chip8_config.off_color.b = 0x0;

	chip8_config.win_x = 0;
	chip8_config.win_y = 0;
	chip8_config.win_w = window_state->win_w;
	chip8_config.win_h = window_state->win_h;
	chip8_config.win_s = 1.0f;
	chip8_config.pixel_spacing = 0;

	chip8_config.quirk_cls_on_reset = 1;
	chip8_config.quirk_zero_vf_register = 1;
	chip8_config.quirk_display_clipping = 1;
	chip8_config.quirk_shift_x_register = 0;
	chip8_config.quirk_increment_i_register = 0;
	chip8_config.quirk_jump = 0;
}

void chip8_init() {

	srand((uint32_t)time(NULL));

	chip8 = (CHIP8*)malloc(sizeof(CHIP8));
	if (chip8 == NULL) {
		printf("Failed to allocate chip8 cpu instance.\n");
		exit(1);
	}

	chip8_init_cpu(chip8);

	set_default_settings();
}
void chip8_destroy() {

	if (chip8 != NULL) {
		free(chip8);
		chip8 = NULL;
	}
}
void chip8_update() {

	if (chip8_state.single_step == SINGLE_STEP_EXE) {
		chip8_state.single_step = SINGLE_STEP_NONE;
		chip8_single_step_cycle();
	}
	else {
		if (chip8->cpu_state == CHIP8_STATE_EXE) {
			chip8_emulate_cycle();
		}
	}
}
void chip8_reset() {
	CHIP8_CPU_STATE s = chip8->cpu_state;
	if (s == CHIP8_STATE_ERROR_OPCODE) 
		s = CHIP8_STATE_HLT;
	chip8_reset_cpu(chip8);
	chip8->cpu_state = s;
	chip8_state.mnem_str[0] = '\0';
}

void chip8_render(CHIP8* chip8) {
		
	SDL_Rect px = { 0 };

	/* Render pixels */
	for (int i = 0; i < CHIP8_NUM_PIXELS; ++i) {
				
		px.x = PX_X(i);
		px.y = PX_Y(i);
		px.w = PX_H;
		px.h = PX_W;

		if (CHIP8_DISPLAY_GET_PX(chip8->display, i)) {
			SDL_SetRenderDrawColor(sdl.game_renderer,
				chip8_config.on_color.r,
				chip8_config.on_color.g,
				chip8_config.on_color.b, 0xFF);
		}
		else {
			SDL_SetRenderDrawColor(sdl.game_renderer,
				chip8_config.off_color.r,
				chip8_config.off_color.g,
				chip8_config.off_color.b, 0xFF);
		}

		SDL_RenderFillRect(sdl.game_renderer, &px);
	}
}
void chip8_beep(CHIP8* chip8) {
	printf("BEEP **Not Implemented\n");
}
uint8_t chip8_random() {
	return (rand() % 256);
}

static void chip8_emulate_cycle() {

//#define cycle_loop

#ifdef cycle_loop
	while (window_stats->instructions_per_frame < (chip8_config.cpu_target)) {
		window_stats->instructions_per_frame++;
		chip8_execute(chip8);
	}
#else
	const double cpu_duration = (1000.0 / chip8_config.cpu_target);
	window_stats->cpu_elapsed_time += window_stats->delta_time;
	if (cpu_duration < window_stats->cpu_elapsed_time) {
		window_stats->cpu_elapsed_time -= cpu_duration;
		window_stats->instructions_per_frame++;
		chip8_execute(chip8);
	}	
#endif

	const double timer_duration = (1000.0 / chip8_config.timer_target);
	window_stats->timer_elapsed_time += window_stats->delta_time;
	if (timer_duration < window_stats->timer_elapsed_time) {
		window_stats->timer_elapsed_time -= timer_duration;
		chip8_step_timers(chip8);
	}
}
static void chip8_single_step_cycle() {
	chip8_execute(chip8);
	chip8_step_timers(chip8);
}

int load_program(const char* filename) {

	chip8_reset();
	chip8_zero_program_memory(chip8);

	uint32_t size = 0;
	FILE* file = NULL;
	fopen_s(&file, filename, "rb");
	if (file == NULL) {
		printf("Error: could not open file: %s\n", filename);
		chip8->cpu_state = CHIP8_STATE_HLT;
		return 1;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (CHIP8_MEMORY_BYTES - CHIP8_PROGRAM_ADDR < size) {
		fclose(file);
		printf("Error: program too big\n");
		chip8->cpu_state = CHIP8_STATE_HLT;
		return 1;
	}

	fread(chip8->ram + CHIP8_PROGRAM_ADDR, 1, size, file);
	fclose(file);
	printf("Loaded %s (%d bytes) into RAM at 0x%x\n", filename, size, CHIP8_PROGRAM_ADDR);
	chip8->cpu_state = CHIP8_STATE_EXE;
	return 0;
}

void set_quirks() {
	/* set cpu quirks from config */
	if (chip8_config.quirk_cls_on_reset)
		chip8->quirks |= CHIP8_QUIRK_CLS_ON_RESET;
	if (chip8_config.quirk_zero_vf_register)
		chip8->quirks |= CHIP8_QUIRK_ZERO_VF_REGISTER;
	if (chip8_config.quirk_shift_x_register)
		chip8->quirks |= CHIP8_QUIRK_SHIFT_X_REGISTER;
	if (chip8_config.quirk_increment_i_register)
		chip8->quirks |= CHIP8_QUIRK_INCREMENT_I_REGISTER;
	if (chip8_config.quirk_jump)
		chip8->quirks |= CHIP8_QUIRK_JUMP_VX;
	if (chip8_config.quirk_display_clipping)
		chip8->quirks |= CHIP8_QUIRK_DISPLAY_CLIPPING;
}
void get_quirks() {
	/* get cpu quirks and set config */
	chip8_config.quirk_cls_on_reset = (chip8->quirks & CHIP8_QUIRK_CLS_ON_RESET);
	chip8_config.quirk_zero_vf_register = (chip8->quirks & CHIP8_QUIRK_ZERO_VF_REGISTER);
	chip8_config.quirk_shift_x_register = chip8->quirks & CHIP8_QUIRK_SHIFT_X_REGISTER;
	chip8_config.quirk_increment_i_register = (chip8->quirks & CHIP8_QUIRK_INCREMENT_I_REGISTER);
	chip8_config.quirk_jump = (chip8->quirks & CHIP8_QUIRK_JUMP_VX);
	chip8_config.quirk_display_clipping = (chip8->quirks & CHIP8_QUIRK_DISPLAY_CLIPPING);
}
