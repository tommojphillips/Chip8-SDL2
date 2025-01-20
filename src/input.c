/* input.c
 */

#include <stdint.h>

#include "SDL.h"

#include "display.h"
#include "chip8_sdl2.h"
#include "chip8.h"

void imgui_toggle_menu();

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
void input_process_event() {
	switch (sdl.e.type) {

	case SDL_KEYDOWN:
		keypad_input(CHIP8_KEY_STATE_KEY_DOWN);
		system_input();
		break;

	case SDL_KEYUP:
		keypad_input(CHIP8_KEY_STATE_KEY_UP);
		break;
	}
}
