/* window_settings.c - 

* Implements functions to save/load settings from a file.
* Uses atexit() to save settings

* GitHub: https:\\github.com\tommojphillips
*/

#include <stdint.h>
#include <string.h>

#include "SDL.h"

#include "display.h"
#include "chip8_sdl2.h"
#include "chip8.h"
#include "ui.h"
#include "load_ini\loadini.h"

extern UI_STATE ui_state;

static const char* settings_filename = "chip8.ini";
static const LOADINI_SETTING_MAP settings_map[] = {
	
	{ "follow_pc_ram", LOADINI_SETTING_TYPE_INT },
	{ "show_debug_window", LOADINI_SETTING_TYPE_INT },
	{ "show_stats_window", LOADINI_SETTING_TYPE_INT },
	{ "show_video_ram_window", LOADINI_SETTING_TYPE_INT },
	{ "show_ram_window", LOADINI_SETTING_TYPE_INT },
	{ "show_registers_window", LOADINI_SETTING_TYPE_INT },
	{ "show_video_button_window", LOADINI_SETTING_TYPE_INT },
	{ "pc_increment", LOADINI_SETTING_TYPE_INT },
	{ "ram_window_cols", LOADINI_SETTING_TYPE_INT },
	{ "video_window_cols", LOADINI_SETTING_TYPE_INT },
	{ "ram_window_ascii", LOADINI_SETTING_TYPE_INT },
	{ "video_window_ascii", LOADINI_SETTING_TYPE_INT },
	{ "ui_scale", LOADINI_SETTING_TYPE_FLOAT },

	{ "window_x", LOADINI_SETTING_TYPE_INT },
	{ "window_y", LOADINI_SETTING_TYPE_INT },
	{ "window_w", LOADINI_SETTING_TYPE_INT },
	{ "window_h", LOADINI_SETTING_TYPE_INT },
	{ "last_window_state", LOADINI_SETTING_TYPE_INT },
	
	{ "chip8_x", LOADINI_SETTING_TYPE_INT },
	{ "chip8_y", LOADINI_SETTING_TYPE_INT },
	{ "chip8_w", LOADINI_SETTING_TYPE_INT },
	{ "chip8_h", LOADINI_SETTING_TYPE_INT },
	{ "chip8_s", LOADINI_SETTING_TYPE_INT },
	{ "chip8_px", LOADINI_SETTING_TYPE_INT },
	
	{ "cpu_target", LOADINI_SETTING_TYPE_INT },
	{ "timer_target", LOADINI_SETTING_TYPE_INT },
	{ "render_target", LOADINI_SETTING_TYPE_INT },
	
	{ "on_color_r", LOADINI_SETTING_TYPE_CHAR },
	{ "on_color_g", LOADINI_SETTING_TYPE_CHAR },
	{ "on_color_b", LOADINI_SETTING_TYPE_CHAR },
	{ "off_color_r", LOADINI_SETTING_TYPE_CHAR },
	{ "off_color_g", LOADINI_SETTING_TYPE_CHAR },
	{ "off_color_b", LOADINI_SETTING_TYPE_CHAR },
		
	{ "quirk_cls_on_reset", LOADINI_SETTING_TYPE_INT },
	{ "quirk_logical_operator_zero_vf_register", LOADINI_SETTING_TYPE_INT },
	{ "quirk_logical_operator_shift_x_register", LOADINI_SETTING_TYPE_INT },
	{ "quirk_load_store_increment_i_register", LOADINI_SETTING_TYPE_INT },
	{ "quirk_jump", LOADINI_SETTING_TYPE_INT },
	{ "quirk_display_clipping", LOADINI_SETTING_TYPE_INT },
};
static LOADINI_VAR_MAP* var_map;

void loadini_load_settings() {
	/* Load settings from file */
	FILE* file = NULL;
	fopen_s(&file, settings_filename, "rb");
	if (file == NULL) {
		printf("Could not load settings from %s\n", settings_filename);
	}
	else {
		printf("Loaded settings from %s\n", settings_filename);
		loadini_load_from_stream(file, settings_map, var_map, sizeof(settings_map) / sizeof(LOADINI_SETTING_MAP));
		fclose(file);
	}

	if (window_state->win_x < 5)
		window_state->win_x = 5;

	if (window_state->win_y < 50)
		window_state->win_y = 50;

	set_quirks();

}
void loadini_save_settings() {
	/* Save settings to file */

	if (window_state->last_window_state == SDL_WINDOW_MAXIMIZED) {
		window_state->win_x = window_state->last_win_x;
		window_state->win_y = window_state->last_win_y;
		window_state->win_w = window_state->last_win_w;
		window_state->win_h = window_state->last_win_h;
	}

	if (window_state->last_window_state == SDL_WINDOW_FULLSCREEN_DESKTOP) {
		window_state->last_window_state = 0;
		window_state->win_x = window_state->last_win_x;
		window_state->win_y = window_state->last_win_y;
		window_state->win_w = window_state->last_win_w;
		window_state->win_h = window_state->last_win_h;
	}

	get_quirks(); 
	imgui_refresh_ui_state();

	FILE* file = NULL;
	fopen_s(&file, settings_filename, "wb");
	if (file == NULL) {
		printf("Could not save settings to %s\n", settings_filename);
	}
	else {
		loadini_save_to_stream(file, settings_map, var_map, sizeof(settings_map) / sizeof(LOADINI_SETTING_MAP));
		printf("Saved settings to %s\n", settings_filename);
		fclose(file);
	}
}

void loadini_init() {
	var_map = (LOADINI_VAR_MAP*)malloc(sizeof(settings_map) / sizeof(LOADINI_SETTING_MAP) * sizeof(LOADINI_VAR_MAP));
	if (var_map == NULL) {
		printf("Failed to allocate memory for var map\n");
		exit(1);
	}

	#define set_var(address) (var_map[i++].var = address)

	int i = 0;
	set_var(&ui_state.follow_pc_ram);
	set_var(&ui_state.show_debug_window);
	set_var(&ui_state.show_stats_window);
	set_var(&ui_state.show_video_ram_window);
	set_var(&ui_state.show_ram_window);
	set_var(&ui_state.show_registers_window);
	set_var(&ui_state.show_video_button_window);
	set_var(&ui_state.pc_increment);
	set_var(&ui_state.cols_ram_window);
	set_var(&ui_state.cols_video_ram_window);
	set_var(&ui_state.ascii_ram_window);
	set_var(&ui_state.ascii_video_ram_window);
	set_var(&ui_state.window_scale);

	set_var(&window_state->win_x);
	set_var(&window_state->win_y);
	set_var(&window_state->win_w);
	set_var(&window_state->win_h);
	set_var(&window_state->last_window_state);

	set_var(&chip8_config.win_x);
	set_var(&chip8_config.win_y);
	set_var(&chip8_config.win_w);
	set_var(&chip8_config.win_h);
	set_var(&chip8_config.win_s);
	set_var(&chip8_config.pixel_spacing);

	set_var(&chip8_config.cpu_target);
	set_var(&chip8_config.timer_target);
	set_var(&chip8_config.render_target);

	set_var(&chip8_config.on_color.r);
	set_var(&chip8_config.on_color.g);
	set_var(&chip8_config.on_color.b);
	set_var(&chip8_config.off_color.r);
	set_var(&chip8_config.off_color.g);
	set_var(&chip8_config.off_color.b);

	set_var(&chip8_config.quirk_cls_on_reset);
	set_var(&chip8_config.quirk_zero_vf_register);
	set_var(&chip8_config.quirk_shift_x_register);
	set_var(&chip8_config.quirk_increment_i_register);
	set_var(&chip8_config.quirk_jump);
	set_var(&chip8_config.quirk_display_clipping);
}
void loadini_destroy() {
	if (var_map != NULL) {
		free(var_map);
		var_map = NULL;
	}
}