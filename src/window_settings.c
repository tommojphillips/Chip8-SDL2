/* window_settings.c - 

* Implements functions to save/load settings from a file.
* Uses atexit() to save settings

* GitHub: https:\\github.com\tommojphillips
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "ui.h"
#include "display.h"
#include "chip8_sdl2.h"
#include "chip8.h"
#include "load_ini\loadini.h"
#include "main.h"

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
	{ "window_x", LOADINI_SETTING_TYPE_INT },
	{ "window_y", LOADINI_SETTING_TYPE_INT },
	{ "window_w", LOADINI_SETTING_TYPE_INT },
	{ "window_h", LOADINI_SETTING_TYPE_INT },
	{ "chip8_x", LOADINI_SETTING_TYPE_INT },
	{ "chip8_y", LOADINI_SETTING_TYPE_INT },
	{ "chip8_w", LOADINI_SETTING_TYPE_INT },
	{ "chip8_h", LOADINI_SETTING_TYPE_INT },
	{ "chip8_s", LOADINI_SETTING_TYPE_INT },
	{ "cpu_target", LOADINI_SETTING_TYPE_INT },
	{ "timer_target", LOADINI_SETTING_TYPE_INT },
	{ "render_target", LOADINI_SETTING_TYPE_INT },
	{ "on_color_r", LOADINI_SETTING_TYPE_CHAR },
	{ "on_color_g", LOADINI_SETTING_TYPE_CHAR },
	{ "on_color_b", LOADINI_SETTING_TYPE_CHAR },
	{ "off_color_r", LOADINI_SETTING_TYPE_CHAR },
	{ "off_color_g", LOADINI_SETTING_TYPE_CHAR },
	{ "off_color_b", LOADINI_SETTING_TYPE_CHAR },
	{ "pixel_spacing", LOADINI_SETTING_TYPE_INT },
	{ "quirk_cls_on_reset", LOADINI_SETTING_TYPE_INT },
	{ "quirk_logical_operator_zero_vf_register", LOADINI_SETTING_TYPE_INT },
	{ "quirk_logical_operator_shift_x_register", LOADINI_SETTING_TYPE_INT },
	{ "quirk_load_store_increment_i_register", LOADINI_SETTING_TYPE_INT },
	{ "quirk_jump", LOADINI_SETTING_TYPE_INT },
	{ "quirk_display_clipping", LOADINI_SETTING_TYPE_INT },
	{ "ram_window_cols", LOADINI_SETTING_TYPE_INT },
	{ "video_window_cols", LOADINI_SETTING_TYPE_INT },
	{ "ram_window_ascii", LOADINI_SETTING_TYPE_INT },
	{ "video_window_ascii", LOADINI_SETTING_TYPE_INT },
	{ "last_window_state", LOADINI_SETTING_TYPE_INT },
	{ "window_scale", LOADINI_SETTING_TYPE_FLOAT },
};
static LOADINI_VAR_MAP var_map[] = {
	{ &ui_state.follow_pc_ram },
	{ &ui_state.show_debug_window },
	{ &ui_state.show_stats_window },
	{ &ui_state.show_video_ram_window },
	{ &ui_state.show_ram_window },
	{ &ui_state.show_registers_window },
	{ &ui_state.show_video_button_window },
	{ &ui_state.pc_increment },
	{ &window_state.win_x },
	{ &window_state.win_y },
	{ &window_state.win_w },
	{ &window_state.win_h },
	{ &chip8_config.win_x },
	{ &chip8_config.win_y },
	{ &chip8_config.win_w },
	{ &chip8_config.win_h },
	{ &chip8_config.win_s },
	{ &chip8_config.cpu_target },
	{ &chip8_config.timer_target },
	{ &chip8_config.render_target },
	{ &chip8_config.on_color.r },
	{ &chip8_config.on_color.g },
	{ &chip8_config.on_color.b },
	{ &chip8_config.off_color.r },
	{ &chip8_config.off_color.g },
	{ &chip8_config.off_color.b },
	{ &chip8_config.pixel_spacing },
	{ &chip8_config.quirk_cls_on_reset },
	{ &chip8_config.quirk_zero_vf_register },
	{ &chip8_config.quirk_shift_x_register },
	{ &chip8_config.quirk_increment_i_register },
	{ &chip8_config.quirk_jump },
	{ &chip8_config.quirk_display_clipping },
	{ &ui_state.cols_ram_window },
	{ &ui_state.cols_video_ram_window },
	{ &ui_state.ascii_ram_window },
	{ &ui_state.ascii_video_ram_window },
	{ &window_state.last_window_state },
	{ &ui_state.window_scale },
};

static void set_default_settings() {

	/* Set initial setting values here so loaded settings overwrite them */
	
	/* Window State */
	window_state.win_x = SDL_WINDOWPOS_CENTERED;
	window_state.win_y = SDL_WINDOWPOS_CENTERED;
	window_state.last_window_state = 0; // Windowed

	/* UI State */
	ui_state.pc_increment = 2;
	ui_state.follow_pc_ram = 0;
	ui_state.window_scale = 1.0f;
	ui_state.show_menu_window = 0;
	ui_state.show_debug_window = 0;
	ui_state.show_stats_window = 0;
	ui_state.show_registers_window = 0;
	ui_state.show_video_button_window = 0;
	ui_state.show_ram_window = 0;
	ui_state.cols_ram_window = 16;
	ui_state.ascii_ram_window = 0;
	ui_state.show_video_ram_window = 0;
	ui_state.cols_video_ram_window = 64;
	ui_state.ascii_video_ram_window = 0;

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
	chip8_config.win_w = 0;
	chip8_config.win_h = 0;
	chip8_config.pixel_spacing = 0;
	chip8_config.win_s = 1.0f;

	chip8_config.quirk_cls_on_reset = 1;
	chip8_config.quirk_zero_vf_register = 1;
	chip8_config.quirk_shift_x_register = 0;
	chip8_config.quirk_increment_i_register = 0;
	chip8_config.quirk_jump = 0;
	chip8_config.quirk_display_clipping = 1;
}

static void load_settings() {
	/* Load settings from file */
	FILE* file = NULL;
	fopen_s(&file, settings_filename, "rb");
	if (file == NULL) {
		printf("Could not load settings from %s\n", settings_filename);
		return;
	}
	printf("Loaded settings from %s\n", settings_filename);
	loadini_load_from_stream(file, settings_map, var_map, sizeof(settings_map) / sizeof(LOADINI_SETTING_MAP));
	fclose(file);
}
static void save_settings(void) {
	/* Save settings to file */
	FILE* file = NULL;
	fopen_s(&file, settings_filename, "wb");
	if (file == NULL) {
		printf("Could not save settings to %s\n", settings_filename);
		return;
	}

	if (window_state.last_window_state == SDL_WINDOW_MAXIMIZED) {
		window_state.win_x = window_state.last_win_x;
		window_state.win_y = window_state.last_win_y;
		window_state.win_w = window_state.last_win_w;
		window_state.win_h = window_state.last_win_h;
	}

	if (window_state.last_window_state == SDL_WINDOW_FULLSCREEN_DESKTOP) {
		window_state.last_window_state = 0;
		window_state.win_x = window_state.last_win_x;
		window_state.win_y = window_state.last_win_y;
		window_state.win_w = window_state.last_win_w;
		window_state.win_h = window_state.last_win_h;
	}

	loadini_save_to_stream(file, settings_map, var_map, sizeof(settings_map) / sizeof(LOADINI_SETTING_MAP));
	printf("Saved settings to %s\n", settings_filename);
	fclose(file);
}

void loadini_init() {
	set_default_settings();
	load_settings();
	if (atexit(save_settings) != 0) {
		printf("atexit() failed for save_settings\n");
	}
}
