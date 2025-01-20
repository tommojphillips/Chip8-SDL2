/* window_ui.cpp
* GitHub: https:\\github.com\tommojphillips
*/

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_memory_editor/imgui_memory_editor.h"
using namespace ImGui;

#include "ui.h"
#include "chip8_sdl2.h"
#include "chip8.h"
#include "chip8_mnem.h"
#include "display.h"

#define renderer_new_frame \
	ImGui_ImplSDLRenderer2_NewFrame(); \
	ImGui_ImplSDL2_NewFrame

#define renderer_draw_data(renderer) \
	ImGui_ImplSDLRenderer2_RenderDrawData(GetDrawData(), renderer)

#define imgui_new_frame \
	renderer_new_frame(); \
	NewFrame

#define imgui_render_frame(renderer) \
	Render(); \
	renderer_draw_data(renderer)

#define init_renderer(window, renderer) \
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer); \
	ImGui_ImplSDLRenderer2_Init(renderer)

#define destroy_renderer ImGui_ImplSDLRenderer2_Shutdown(); \
	ImGui_ImplSDL2_Shutdown

#define process_event ImGui_ImplSDL2_ProcessEvent(&sdl.e)

#define WINDOW_SETTINGS 1
#define CHIP8_SETTINGS 2

/* Imgui state */
typedef struct {
	ImGuiContext* context;
	MemoryEditor* mem_editor;
	MemoryEditor* video_editor; 
	ImGuiIO* io;
	char tmp_s[32];
} IMGUI_STATE;

static IMGUI_STATE imgui = { 0 };

extern "C" UI_STATE ui_state = { 0 };

static void ram_window_follow_pc(uint16_t pc, int force);
static void outline_test();
static void stats_window();
static void registers_window();
static void debug_window();
static void window_settings_window();
static void chip8_settings_window();
static void menu_window();
static void menu_window();
static void video_memory_window();
static void resize_display();

static void set_default_settings() {
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
}

void imgui_refresh_ui_state() {
	/* Save off ui state so they can be saved to a file */
	ui_state.show_ram_window = imgui.mem_editor->Open;
	ui_state.cols_ram_window = imgui.mem_editor->Cols;
	ui_state.ascii_ram_window = imgui.mem_editor->OptShowAscii;
	ui_state.show_video_ram_window = imgui.video_editor->Open;
	ui_state.cols_video_ram_window = imgui.video_editor->Cols;
	ui_state.ascii_video_ram_window = imgui.video_editor->OptShowAscii;
	ui_state.window_scale = imgui.io->FontGlobalScale;
}

void imgui_init() {
	set_default_settings();
}
void imgui_create_renderer() {

	IMGUI_CHECKVERSION();
	imgui.context = CreateContext();
	if (imgui.context == NULL) {
		printf("Failed to create IMGUI Context");
		return;
	}

	StyleColorsDark();
	//StyleColorsLight();

	imgui.io = &GetIO();
	imgui.io->FontGlobalScale = ui_state.window_scale;

	static MemoryEditor mem_edit;
	mem_edit.Open = ui_state.show_ram_window;
	mem_edit.Cols = ui_state.cols_ram_window;
	mem_edit.OptShowAscii = ui_state.ascii_ram_window;
	mem_edit.GotoAddr = CHIP8_PROGRAM_ADDR;
	imgui.mem_editor = &mem_edit;

	static MemoryEditor video_edit;
	video_edit.Open = ui_state.show_video_ram_window;
	video_edit.Cols = ui_state.cols_video_ram_window;
	video_edit.OptShowAscii = ui_state.ascii_video_ram_window;
	imgui.video_editor = &video_edit;

	ImGui_ImplSDL2_InitForSDLRenderer(sdl.game_window, sdl.game_renderer);
	ImGui_ImplSDLRenderer2_Init(sdl.game_renderer);
}
void imgui_destroy() {

	
	/* Cleanup */
	ImGui_ImplSDLRenderer2_Shutdown(); 
	ImGui_ImplSDL2_Shutdown();
	DestroyContext(imgui.context);
}
void imgui_update() {
	ImGui_ImplSDLRenderer2_NewFrame(); 
	ImGui_ImplSDL2_NewFrame();
	NewFrame();

	if (ui_state.show_menu_window) {
		menu_window();
	}
	else {
		if (ui_state.show_registers_window) {
			registers_window();
		}

		if (imgui.mem_editor->Open) {
			imgui.mem_editor->DrawWindow("RAM", chip8->ram, CHIP8_MEMORY_BYTES, 0);
		}

		if (imgui.video_editor->Open) {
			imgui.video_editor->DrawWindow("Display RAM", chip8->display, CHIP8_DISPLAY_BYTES, 0);
		}

		if (ui_state.show_video_button_window) {
			video_memory_window();
		}
	}

	if (ui_state.show_stats_window) {
		stats_window();
	}
	if (ui_state.show_debug_window) {
		debug_window();
	}
	
	
	Render();	
	ImGui_ImplSDLRenderer2_RenderDrawData(GetDrawData(), sdl.game_renderer);
}
void imgui_process_event() {
	ImGui_ImplSDL2_ProcessEvent(&sdl.e);
}
void imgui_toggle_menu() {
	ui_state.show_menu_window ^= 1;
}

static void stats_window() {
	Begin("Stats", (bool*)&ui_state.show_stats_window);
	Text("%.2f dt", window_stats->delta_time);
	Text("%.2f ms/frame ", window_stats->render_elapsed_time);
	Text("%.2f fps ", window_stats->render_fps);
	Text("Instr/frame  %u", window_stats->instructions_per_frame);
	Text("cycles/frame  %u", chip8->cycles);
	End();
}
static void registers_window() {
	Begin("Registers", (bool*)&ui_state.show_registers_window);

	int tmp_int;
	PushItemWidth(GetFontSize() * 8);
	Text("SP: ");
	SameLine();
	tmp_int = chip8->sp;
	if (InputInt("###stack_pointer", &tmp_int, 0, 1, ImGuiInputTextFlags_CharsHexadecimal)) {
		chip8->sp = tmp_int % 65536;
	}

	Text("I:  ");
	SameLine();
	tmp_int = chip8->i;
	if (InputInt("###i_register", &tmp_int, 0, 1, ImGuiInputTextFlags_CharsHexadecimal)) {
		chip8->i = tmp_int % 65536;
	}

	Text("DT: ");
	SameLine();
	tmp_int = chip8->delay_timer;
	if (InputInt("###dt", &tmp_int, 0, 1, ImGuiInputTextFlags_CharsHexadecimal)) {
		chip8->delay_timer = tmp_int % 256;
	}

	Text("ST: ");
	SameLine();
	tmp_int = chip8->sound_timer;
	if (InputInt("###st", &tmp_int, 0, 1, ImGuiInputTextFlags_CharsHexadecimal)) {
		chip8->sound_timer = tmp_int % 256;
	}

	for (int i = 0; i < CHIP8_REGISTER_COUNT; ++i) {
		Text("V%X: ", i);
		PushID(i);
		SameLine();
		tmp_int = chip8->v[i];
		if (InputInt("", &tmp_int, 0, 1, ImGuiInputTextFlags_CharsHexadecimal)) {
			if (tmp_int < 256 && tmp_int > 0) {
				chip8->v[i] = tmp_int % 256;
			}
		}
		PopID();
	}
	PopItemWidth();
	End();
}
static void debug_window() {
	Begin("Debug", (bool*)&ui_state.show_debug_window);

	Checkbox("RAM", (bool*)&imgui.mem_editor->Open);
	SameLine();
	Checkbox("Video RAM", (bool*)&imgui.video_editor->Open);
	SameLine();
	Checkbox("Video Editor", (bool*)&ui_state.show_video_button_window);
	SameLine();
	Checkbox("Registers", (bool*)&ui_state.show_registers_window);

	if (chip8->cpu_state == CHIP8_STATE_ERROR_OPCODE) {
		SeparatorText("Opcode error!");
	}
	else {
		
		switch (chip8->cpu_state) {
			case CHIP8_STATE_HLT:
				SeparatorText("Halt");
				break;
			case CHIP8_STATE_EXE:
				SeparatorText("Run");
				break;
		}
	}

	sprintf_s(imgui.tmp_s, "%04x", chip8->pc);
	if (Button(imgui.tmp_s)) {
		imgui.mem_editor->Open = true;
		ram_window_follow_pc(0, 1);
	}
	SetItemTooltip("Program Counter ( PC ) Click to view in RAM");

	chip8_mnem(chip8, 0, chip8_state.mnem_str);
	if (chip8_state.mnem_str[0] != '\0') {
		SameLine();
		Text("%s", chip8_state.mnem_str);
		SetItemTooltip("Instruction mnemonic");
	}

	if (chip8->cpu_state != CHIP8_STATE_EXE) {
		if (ArrowButton("Continue", ImGuiDir_Right)) {
			chip8->cpu_state = CHIP8_STATE_EXE;
		}
		SetItemTooltip("Continue");
	}
	else {
		if (Button("||")) {
			chip8->cpu_state = CHIP8_STATE_HLT;
		}

		SetItemTooltip("Break All");
	}

	SameLine();
	if (Button("Reset")) {
		chip8_reset();
	}
	SetItemTooltip("Restart");

	if (chip8->cpu_state != CHIP8_STATE_EXE) {

		SameLine();
		if (Button(">>")) {
			chip8_state.single_step = SINGLE_STEP_EXE;
			uint16_t next_pc = 0;
			chip8_mnem_find_next(chip8, &next_pc);
			ram_window_follow_pc(next_pc, 0);
		}
		SetItemTooltip("Step");

		uint32_t tmp_uint = chip8->pc;
		InputInt("###program_counter", (int*)&tmp_uint, 0, 100, ImGuiInputTextFlags_CharsHexadecimal);
		if (IsItemDeactivatedAfterEdit()) {
			if (tmp_uint < CHIP8_MEMORY_BYTES) {
				chip8->pc = (uint32_t)tmp_uint;
				ram_window_follow_pc(0, 1);
			}
		}

		SameLine();
		if (ArrowButton("pc_dec", ImGuiDir_Left)) {
			if (chip8->pc >= ui_state.pc_increment)
				chip8->pc -= ui_state.pc_increment;
			ram_window_follow_pc(0, 1);
		}
		SetItemTooltip("Decrement program counter ( PC )");

		SameLine();
		if (ArrowButton("pc_inc", ImGuiDir_Right)) {
			if (chip8->pc < CHIP8_MEMORY_BYTES - ui_state.pc_increment)
				chip8->pc += ui_state.pc_increment;
			ram_window_follow_pc(0, 1);
		}
		SetItemTooltip("Increment program counter ( PC )");

		SameLine();
		int tmp_i = ui_state.pc_increment;
		if (InputInt("###program_counter_increment", &tmp_i, 0)) {
			if (tmp_i > 0 && tmp_i < CHIP8_MEMORY_BYTES - 2) {
				ui_state.pc_increment = tmp_i % 16;
			}
		}

		Checkbox("Follow PC", (bool*)&ui_state.follow_pc_ram);
		SetItemTooltip("Follow program counter in RAM window ( PC )");
	}

	End();
}
static void keypad_window() {

}
static void video_memory_window() {
	Begin("video_editor", (bool*)&ui_state.show_video_button_window);

	const ImU32 on = IM_COL32(chip8_config.on_color.r, chip8_config.on_color.g, chip8_config.on_color.b, 255);
	const ImU32 off = IM_COL32(chip8_config.off_color.r, chip8_config.off_color.g, chip8_config.off_color.b, 255);
	const ImVec2 scale = ImVec2(10, 10);
	int click = 0;
	int click_id = 0;
	int pixel_state = 0;
	for (int i = 0; i < CHIP8_NUM_PIXELS; ++i) {
		PushID(i);
		PushStyleColor(ImGuiCol_Button, CHIP8_DISPLAY_GET_PX(chip8->display, i) ? on : off);
		Button("", scale);

		if (IsItemActivated()) {
			CHIP8_DISPLAY_TOGGLE_PX(chip8->display, i);
		}

		PopStyleColor();
		PopID();

		if ((i+1) % CHIP8_DISPLAY_WIDTH != 0)
			SameLine();
	}
	End();
}
static void window_settings_window() {

	/* chip8 window */
		
	if (SliderFloat("GUI Scale", &ui_state.window_scale, .3f, 2.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
		imgui.io->FontGlobalScale = ui_state.window_scale;
	}
	
	/* chip8 window */

	int limit = DISPLAY_X_LIMIT;
	SliderInt("Display X", &chip8_config.win_x, -limit, limit);

	limit = DISPLAY_Y_LIMIT;
	SliderInt("Display Y", &chip8_config.win_y, -limit, limit);

	SliderFloat("Display Scale", &chip8_config.win_s, 0.1f, 1.0f);

	int pixel_spacing = chip8_config.pixel_spacing;
	limit = DISPLAY_PX_LIMIT;
	if (SliderInt("Pixel Spacing", &pixel_spacing, 0, limit)) {
		chip8_config.pixel_spacing = pixel_spacing;
	}

	resize_display();

	/* pixel colors */
	ImVec4 tmp_vec4;
	tmp_vec4.x = (float)(chip8_config.on_color.r / 255.0f);
	tmp_vec4.y = (float)(chip8_config.on_color.g / 255.0f);
	tmp_vec4.z = (float)(chip8_config.on_color.b / 255.0f);
	if (ColorEdit3("Pixel On", (float*)&tmp_vec4)) {
		chip8_config.on_color.r = (uint8_t)(255.0f * tmp_vec4.x);
		chip8_config.on_color.g = (uint8_t)(255.0f * tmp_vec4.y);
		chip8_config.on_color.b = (uint8_t)(255.0f * tmp_vec4.z);
	}

	tmp_vec4.x = (float)(chip8_config.off_color.r / 255.0f);
	tmp_vec4.y = (float)(chip8_config.off_color.g / 255.0f);
	tmp_vec4.z = (float)(chip8_config.off_color.b / 255.0f);
	if (ColorEdit3("Pixel Off", (float*)&tmp_vec4)) {
		chip8_config.off_color.r = (uint8_t)(255.0f * tmp_vec4.x);
		chip8_config.off_color.g = (uint8_t)(255.0f * tmp_vec4.y);
		chip8_config.off_color.b = (uint8_t)(255.0f * tmp_vec4.z);
	}	
}
static void chip8_settings_window() {

	bool tmp;

	/* Chip8 quirks */

	tmp = (chip8->quirks & CHIP8_QUIRK_CLS_ON_RESET);
	if (Checkbox("CLS On reset", &tmp)) {
		chip8->quirks ^= CHIP8_QUIRK_CLS_ON_RESET;
		chip8_config.quirk_cls_on_reset = tmp;
	}
	SetItemTooltip("Clear the display on reset and on program load.");

	SameLine();
	tmp = (chip8->quirks & CHIP8_QUIRK_ZERO_VF_REGISTER);
	if (Checkbox("VF Zero", &tmp)) {
		chip8->quirks ^= CHIP8_QUIRK_ZERO_VF_REGISTER;
		chip8_config.quirk_zero_vf_register = tmp;
	}
	if (chip8_config.quirk_zero_vf_register) {
		SetItemTooltip("Zero register VF in instructions 'AND', 'OR' and 'XOR'");
	}
	else {
		SetItemTooltip("Preserve register VF in instructions 'AND', 'OR' and 'XOR'");
	}

	SameLine();
	tmp = (chip8->quirks & CHIP8_QUIRK_DISPLAY_CLIPPING);
	if (Checkbox("Sprite Clipping", &tmp)) {
		chip8->quirks ^= CHIP8_QUIRK_DISPLAY_CLIPPING;
		chip8_config.quirk_display_clipping = tmp;
	}
	SetItemTooltip("Clip sprites that are off screen (out of bounds)");

	tmp = (chip8->quirks & CHIP8_QUIRK_SHIFT_X_REGISTER);
	if (Checkbox("Shift X Register", &tmp)) {
		chip8->quirks ^= CHIP8_QUIRK_SHIFT_X_REGISTER;
		chip8_config.quirk_shift_x_register = tmp;
	}
	if (chip8_config.quirk_shift_x_register) {
		SetItemTooltip("Shift register X  in 'SHL' and 'SHR' instructions");
	}
	else {
		SetItemTooltip("Shift register Y and store results in register X in 'SHL' and 'SHR' instructions");
	}

	SameLine();
	tmp = (chip8->quirks & CHIP8_QUIRK_INCREMENT_I_REGISTER);
	if (Checkbox("LD Inc I", &tmp)) {
		chip8->quirks ^= CHIP8_QUIRK_INCREMENT_I_REGISTER;
		chip8_config.quirk_increment_i_register = tmp;
	}
	if (chip8_config.quirk_increment_i_register) {
		SetItemTooltip("Increment I register in LD/STR instructions 'LD [I], Vx' and 'LD Vx, [I]'");
	}
	else {
		SetItemTooltip("Preserve I register in LD/STR instructions 'LD [I], Vx' and 'LD Vx, [I]'");
	}

	SameLine();
	tmp = (chip8->quirks & CHIP8_QUIRK_JUMP_VX);
	if (Checkbox("JUMP VX", &tmp)) {
		chip8->quirks ^= CHIP8_QUIRK_JUMP_VX;
		chip8_config.quirk_jump = tmp;
	}
	SetItemTooltip("JMP NNN, V0 instead of JMP XNN, VX");
	

	const int limit = 1000;

	Text("Clock Hz:  ");
	SameLine();
	if (ArrowButton("CLOCK_DOWN", ImGuiDir_Left)) {
		if (chip8_config.cpu_target > 0) {
			chip8_config.cpu_target--;
		}
	}
	SetItemTooltip("Decrease Clock Hz Target");
	SameLine();
	if (ArrowButton("CLOCK_UP", ImGuiDir_Right)) {
		if (chip8_config.cpu_target < limit) {
			chip8_config.cpu_target++;
		}
	}
	SetItemTooltip("Increase Clock Hz Target");
	SameLine();
	PushItemWidth(GetFontSize() * 15);
	SliderInt("###Clock_Target_Hz", &chip8_config.cpu_target, 1, limit);
	PopItemWidth();
	SetItemTooltip("Clock Hz Target");

	Text("Timer Hz:  ");
	SameLine();
	if (ArrowButton("TIMER_DOWN", ImGuiDir_Left)) {
		if (chip8_config.timer_target > 0) {
			chip8_config.timer_target--;
		}
	}
	SetItemTooltip("Decrease Timer Hz Target");
	SameLine();
	if (ArrowButton("TIMER_UP", ImGuiDir_Right)) {
		if (chip8_config.timer_target < limit) {
			chip8_config.timer_target++;
		}
	}
	SetItemTooltip("Increase Timer Hz Target");
	SameLine();
	PushItemWidth(GetFontSize() * 15);
	SliderInt("###Timer_Target_Hz", &chip8_config.timer_target, 1, limit);
	PopItemWidth();
	SetItemTooltip("Timer Hz Target");

	Text("Render Hz: ");
	SameLine();
	if (ArrowButton("RENDER_DOWN", ImGuiDir_Left)) {
		if (chip8_config.render_target > 0) {
			chip8_config.render_target--;
		}
	}
	SetItemTooltip("Decrease Render Hz Target");
	SameLine();
	if (ArrowButton("RENDER_UP", ImGuiDir_Right)) {
		if (chip8_config.render_target < limit) {
			chip8_config.render_target++;
		}
	}
	SetItemTooltip("Increase Render Hz Target"); 
	SameLine();	
	PushItemWidth(GetFontSize() * 15);
	SliderInt("###Render_Target_Hz", &chip8_config.render_target, 1, limit);
	PopItemWidth();
	SetItemTooltip("Render Hz Target");
}
static void menu_window() {
	Begin("Menu", (bool*)&ui_state.show_menu_window);

	if (Button("Stats")) {
		ui_state.show_stats_window ^= 1;
	}
	SameLine();
	if (Button("Debug")) {
		ui_state.show_debug_window ^= 1;
	}
	SameLine();
	if (Button("Window Settings")) {
		if (ui_state.settings_window != WINDOW_SETTINGS)
			ui_state.settings_window = WINDOW_SETTINGS;
		else
			ui_state.settings_window = 0;
	}
	SameLine();
	if (Button("Chip8 Settings")) {
		if (ui_state.settings_window != CHIP8_SETTINGS)
			ui_state.settings_window = CHIP8_SETTINGS;
		else
			ui_state.settings_window = 0;
	}

	switch (ui_state.settings_window) {
		case WINDOW_SETTINGS:
			window_settings_window();
			break;
		case CHIP8_SETTINGS:
			chip8_settings_window();
			break;
	}

	End();
}

static void ram_window_follow_pc(uint16_t pc, int force) {
	if (imgui.mem_editor->Open && (ui_state.follow_pc_ram || force)) {
		if (pc == 0)
			pc = chip8->pc;
		imgui.mem_editor->GotoAddrAndHighlight(pc, 2);
	}
}
static void outline_test() {

	for (int i = 0; i < CHIP8_DISPLAY_WIDTH; ++i) {
		CHIP8_DISPLAY_SET_PX(chip8->display, i);
		CHIP8_DISPLAY_SET_PX(chip8->display, CHIP8_NUM_PIXELS - 1 - i);
	}

	for (int i = 0; i < CHIP8_NUM_PIXELS; i += CHIP8_DISPLAY_WIDTH) {
		CHIP8_DISPLAY_SET_PX(chip8->display, i);
		CHIP8_DISPLAY_SET_PX(chip8->display, CHIP8_NUM_PIXELS - 1 - i);
	}
}

static void resize_display() {

	int win_w = (int)(window_state->win_w * chip8_config.win_s);
	int win_h = (int)(window_state->win_h * chip8_config.win_s);

	double ratioX = win_w / (double)(64 * (((chip8_config.win_w) / 64) + (chip8_config.pixel_spacing)));
	double ratioY = win_h / (double)(32 * (((chip8_config.win_w) / 64) + (chip8_config.pixel_spacing)));
	double ratio = ratioX < ratioY ? ratioX : ratioY;

	chip8_config.win_w = (int)(DISPLAY_WIDTH * ratio);
	chip8_config.win_h = (int)(DISPLAY_HEIGHT * ratio);

	int limit = DISPLAY_X_LIMIT;
	if (chip8_config.win_x < -limit) {
		chip8_config.win_x = -limit;
	}
	if (chip8_config.win_x > limit) {
		chip8_config.win_x = limit;
	}

	limit = DISPLAY_Y_LIMIT;
	if (chip8_config.win_y < -limit) {
		chip8_config.win_y = -limit;
	}
	if (chip8_config.win_y > limit) {
		chip8_config.win_y = limit;
	}
}