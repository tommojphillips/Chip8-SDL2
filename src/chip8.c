// chip8.c
//
// GitHub: https:\\github.com\tommojphillips

#include <stdint.h>

#include "chip8.h"

uint8_t chip8_font[CHIP8_FONT_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(CHIP8* chip8) {

	chip8_reset(chip8);
	chip8_zero_memory(chip8);
	chip8_load_font(chip8, chip8_font);
}
void chip8_reset(CHIP8* chip8) {

	chip8->i = 0;
	chip8->pc = CHIP8_PROGRAM_ADDR;
	chip8->sp = 0;
	chip8->opcode = 0;
	chip8->cpu_state = CHIP8_STATE_RUNNING;
	
	for (int i = 0; i < CHIP8_REGISTER_COUNT; ++i) {
		chip8->v[i] = 0;
	}

	for (int i = 0; i < CHIP8_STACK_SIZE; ++i) {
		chip8->stack[i] = 0;
	}
	
	chip8->keypad = 0;
	chip8->fxoa_state = 0;
	
	chip8->delay_timer = 0;
	chip8->sound_timer = 0;

	chip8->config.quirks = CHIP8_QUIRK_SHIFT;
	
	chip8_zero_video_memory(chip8);
}

void chip8_zero_memory(CHIP8* chip8) {

	for (int i = 0; i < CHIP8_MEMORY_SIZE; ++i) {
		chip8->ram[i] = 0;
	}
}
void chip8_zero_program_memory(CHIP8* chip8) {

	for (int i = CHIP8_PROGRAM_ADDR; i < CHIP8_MEMORY_SIZE; ++i) {
		chip8->ram[i] = 0;
	}
}
void chip8_zero_video_memory(CHIP8* chip8) {

	for (int i = 0; i < CHIP8_GFX_SIZE; ++i) {
		chip8->video[i] = 0;
	}
}

void chip8_load_font(CHIP8* chip8, uint8_t* font) {

	for (int i = 0; i < CHIP8_FONT_SIZE; ++i) {
		chip8->ram[i] = font[i];
	}
}
int chip8_load_rom(CHIP8* chip8, uint8_t* rom, uint16_t size) {

	if (size > CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_ADDR)
		return 1;

	for (int i = 0; i < size; ++i) { 
		chip8->ram[CHIP8_PROGRAM_ADDR + i] = rom[i];
	}

	return 0;
}

void chip8_dec_timers(CHIP8* chip8) {

	if (chip8->delay_timer > 0) {
		chip8->delay_timer -= 1;
	}
	if (chip8->sound_timer > 0) {
		chip8->sound_timer -= 1;
	}
}

int chip8_execute(CHIP8* chip8) {
	
	chip8->opcode = chip8->ram[chip8->pc] << 8 | chip8->ram[chip8->pc + 1];
	
	switch ((chip8->opcode >> 12) & 0x0F) {

		case 0x0: {
			switch (chip8->opcode & 0x00FF) {
				
				case 0xE0: // CLS
					chip8_00E0(chip8);
					break;
				case 0xEE: // RET
					chip8_00EE(chip8);
					break;
				default:
					return 1;
			}
		} break;

		case 0x1: // JMP MMM
			chip8_1MMM(chip8);
			break;

		case 0x2: // CALL MMM
			chip8_2MMM(chip8); 
			break;

		case 0x3: // SE VX, KK
			chip8_3XKK(chip8);
			break;

		case 0x4: // SNE VX, KK
			chip8_4XKK(chip8);
			break;

		case 0x5: // SE VX, VY
			chip8_5XY0(chip8);
			break;

		case 0x6: // LD VX, KK
			chip8_6XKK(chip8);
			break;

		case 0x7: // ADD VX, KK
			chip8_7XKK(chip8);
			break;

		case 0x8: {
			switch (chip8->opcode & 0x000F) {

				case 0x00: // LD VX, VY
					chip8_8XY0(chip8);
					break;
				case 0x01: // OR VX, VY
					chip8_8XY1(chip8);
					break;
				case 0x02: // AND VX, VY
					chip8_8XY2(chip8);
					break;
				case 0x03: // XOR VX, VY
					chip8_8XY3(chip8);
					break;
				case 0x04: // ADD VX, VY
					chip8_8XY4(chip8);
					break;
				case 0x05: // SUB VX, VY
					chip8_8XY5(chip8);
					break;

				case 0x06:
					if (chip8->config.quirks & CHIP8_QUIRK_SHIFT) {
						chip8_8X06(chip8); // SHL VX
					}
					else {
						chip8_8XY6(chip8); // SHL VX, VY
					}
					break;
				
				case 0x07: // SUBN VX, VY
					chip8_8XY7(chip8);
					break;

				case 0x0E:
					if (chip8->config.quirks & CHIP8_QUIRK_SHIFT) {
						chip8_8X0E(chip8); // SHR VX
					}
					else {
						chip8_8XYE(chip8); // SHR VX, VY
					}
					break;

				default:
					return 1;
			}
		} break;

		case 0x9: // SNE VX, VY
			chip8_9XY0(chip8);
			break;
		case 0xA: // LD I, MMM 
			chip8_AMMM(chip8);
			break;
		case 0xB: // JMP MMM, V0
			chip8_BMMM(chip8);
			break;
		case 0xC: // RND VX, KK
			chip8_CXKK(chip8);
			break;
		case 0xD: // DSP VX, VY, N
			chip8_DXYN(chip8);
			break;

		case 0xE: {
			switch (chip8->opcode & 0x00FF) {

				case 0x9E: // SKP VX
					chip8_EX9E(chip8);
					break;
				case 0xA1: // SKNP VX 
					chip8_EXA1(chip8);
					break;
				default:
					return 1;
			}
		} break;

		case 0xF: {
			switch (chip8->opcode & 0x00FF) {

				case 0x07: // LD VX, DT
					chip8_FX07(chip8);
					break;
				case 0x0A: // LD VX, KEY
					chip8_FX0A(chip8);
					break;
				case 0x15: // LD DT, VX
					chip8_FX15(chip8);
					break;
				case 0x18: // LD ST, VX
					chip8_FX18(chip8);
					break;
				case 0x1E: // ADD I, VX
					chip8_FX1E(chip8);
					break;
				case 0x29: // LD F, VX 
					chip8_FX29(chip8);
					break;
				case 0x33: // LD B, VX
					chip8_FX33(chip8);
					break;
				case 0x55: // LD [I], VX 
					chip8_FX55(chip8);
					break;
				case 0x65: // LD VX, [I] 
					chip8_FX65(chip8);
					break;
				default:
					return 1;
			}
		} break;

		default:
			return 1;
	}

	return 0;
}

int chip8_emulate_cycle(CHIP8* chip8) {
	int result = chip8_execute(chip8);
	if (result != 0)
		return result;

	chip8_dec_timers(chip8);
	chip8_render_display(chip8);

	return 0;
}
