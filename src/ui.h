/* window_ui.h
* GitHub: https:\\github.com\tommojphillips
*/

#ifndef IMGUI_WINDOW_UI_H
#define IMGUI_WINDOW_UI_H

typedef struct {
	int show_menu_window;
	int show_stats_window;
	int show_debug_window;
	int show_ram_window;
	int show_video_ram_window;
	int show_registers_window;
	int show_video_button_window;
	int settings_window;
	int cols_video_ram_window;
	int ascii_video_ram_window;
	int cols_ram_window;
	int ascii_ram_window;
	int pc_increment;
	int follow_pc_ram;
	float window_scale;
} UI_STATE;

#ifdef __cplusplus
extern "C" {
#endif

// init imgui
void imgui_init();

void imgui_create_renderer();

// destroy imgui
void imgui_destroy();

// update imgui
void imgui_update();

// imgui process event
void imgui_process_event();

// imgui toggle menu ui
void imgui_toggle_menu();

void imgui_refresh_ui_state();

#ifdef __cplusplus
};
#endif

#endif
