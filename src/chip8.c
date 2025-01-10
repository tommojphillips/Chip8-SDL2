// chip8.c
//
// GitHub: https:\\github.com\tommojphillips

#include <stdint.h>

#include "chip8.h"

#define X ((chip8->opcode >> 8) & 0x0F)
#define Y ((chip8->opcode >> 4) & 0x00F)
#define NNN (chip8->opcode & 0x0FFF)
#define NN (chip8->opcode & 0x00FF)
#define N (chip8->opcode & 0x000F)

#define VX chip8->v[X]
#define VY chip8->v[Y]
#define VF chip8->v[0xF]

#define GET_KEY(x, n) ((x >> n) & 0x1U)
#define SET_KEY(x, n, v) x = ((x & ~(0x1U << n)) | (v << n))

const uint8_t chip8_font[CHIP8_FONT_SIZE] = {
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
	/* Initialize chip8 cpu state */
	chip8_reset(chip8);
	chip8_zero_memory(chip8);
	chip8_load_font(chip8, chip8_font);
}
void chip8_reset(CHIP8* chip8) {
	/* Reset chip8 cpu state */
	chip8->i = 0;
	chip8->pc = CHIP8_PROGRAM_ADDR;
	chip8->sp = 0;
	chip8->opcode = 0;
	chip8->cpu_state = CHIP8_STATE_RUN;
	
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

	chip8->config.quirks = CHIP8_QUIRK_SHIFT_X_REGISTER | CHIP8_QUIRK_lOGICAL_OPERATOR_ZERO_VF;

	chip8_zero_video_memory(chip8);
}
void chip8_load_font(CHIP8* chip8, const uint8_t* font) {
	/* Load font into chip memory */
	for (int i = 0; i < CHIP8_FONT_SIZE; ++i) {
		chip8->ram[i] = font[i];
	}
}

void chip8_zero_memory(CHIP8* chip8) {
	/* Zero entire memory space */
	for (int i = 0; i < CHIP8_MEMORY_SIZE; ++i) {
		chip8->ram[i] = 0;
	}
}
void chip8_zero_program_memory(CHIP8* chip8) {
	/* Zero program memory space */
	for (int i = CHIP8_PROGRAM_ADDR; i < CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_ADDR; ++i) {
		chip8->ram[i] = 0;
	}
}
void chip8_zero_video_memory(CHIP8* chip8) {
	/* Zero video memory space */
	for (int i = 0; i < CHIP8_VIDEO_SIZE; ++i) {
		chip8->video[i] = 0;
	}
}

void chip8_step_timers(CHIP8* chip8) {

	if (chip8->delay_timer > 0) {
		chip8->delay_timer -= 1;
	}

	if (chip8->sound_timer > 0) {
		chip8->sound_timer -= 1;
		if (chip8->sound_timer == 0) {
			chip8_beep(chip8);
		}
	}
}
int chip8_execute(CHIP8* chip8) {
	/* Decode and execute the next instruction */
	chip8->opcode = (chip8->ram[chip8->pc] << 8) | chip8->ram[chip8->pc + 1];
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

		case 0x1: // JMP NNN
			chip8_1NNN(chip8);
			break;

		case 0x2: // CALL NNN
			chip8_2NNN(chip8); 
			break;

		case 0x3: // SE VX, NN
			chip8_3XNN(chip8);
			break;

		case 0x4: // SNE VX, NN
			chip8_4XNN(chip8);
			break;

		case 0x5: // SE VX, VY
			chip8_5XY0(chip8);
			break;

		case 0x6: // LD VX, NN
			chip8_6XNN(chip8);
			break;

		case 0x7: // ADD VX, NN
			chip8_7XNN(chip8);
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
				case 0x06: // SHL VX, VY
					chip8_8XY6(chip8);
					break;				
				case 0x07: // SUBN VX, VY
					chip8_8XY7(chip8);
					break;
				case 0x0E: // SHR VX, VY
					chip8_8XYE(chip8);
					break;

				default:
					return 1;
			}
		} break;

		case 0x9: // SNE VX, VY
			chip8_9XY0(chip8);
			break;
		case 0xA: // LD I, NNN 
			chip8_ANNN(chip8);
			break;
		case 0xB: // JMP NNN, V0
			chip8_BNNN(chip8);
			break;
		case 0xC: // RND VX, NN
			chip8_CXNN(chip8);
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

/* OPCODES*/

inline void chip8_00E0(CHIP8* chip8) {
	// CLS
	chip8_zero_video_memory(chip8);
	chip8->pc += 2;
}
inline void chip8_00EE(CHIP8* chip8) {
	// RET
	chip8->pc = chip8->stack[chip8->sp];
	chip8->sp -= 1;
	chip8->pc += 2;
}
inline void chip8_1NNN(CHIP8* chip8) {
	// JMP NNN
	chip8->pc = NNN;
}
inline void chip8_2NNN(CHIP8* chip8) {
	// CALL NNN
	chip8->sp += 1;
	chip8->stack[chip8->sp] = chip8->pc;
	chip8->pc = NNN;
}
inline void chip8_3XNN(CHIP8* chip8) {
	// SE VX, NN
	if (VX == NN) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
inline void chip8_4XNN(CHIP8* chip8) {
	// SNE VX, NN
	if (VX != NN) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
inline void chip8_5XY0(CHIP8* chip8) {
	// SE VX, VY
	if (VX == VY) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
inline void chip8_6XNN(CHIP8* chip8) {
	// LD VX, NN
	VX = NN;
	chip8->pc += 2;
}
inline void chip8_7XNN(CHIP8* chip8) {
	// ADD VX, NN
	VX += NN;
	chip8->pc += 2;
}
inline void chip8_8XY0(CHIP8* chip8) {
	// LD VX, VY
	VX = VY;
	chip8->pc += 2;
}
inline void chip8_8XY1(CHIP8* chip8) {
	// OR VX, VY
	VX |= VY;
	if (chip8->config.quirks & CHIP8_QUIRK_lOGICAL_OPERATOR_ZERO_VF) {
		VF = 0;
	}
	chip8->pc += 2;
}
inline void chip8_8XY2(CHIP8* chip8) {
	// AND VX, VY
	VX &= VY;
	if (chip8->config.quirks & CHIP8_QUIRK_lOGICAL_OPERATOR_ZERO_VF) {
		VF = 0;
	}
	chip8->pc += 2;
}
inline void chip8_8XY3(CHIP8* chip8) {
	// XOR VX, VY
	VX ^= VY;
	if (chip8->config.quirks & CHIP8_QUIRK_lOGICAL_OPERATOR_ZERO_VF) {
		VF = 0;
	}
	chip8->pc += 2;
}
inline void chip8_8XY4(CHIP8* chip8) {
	// ADD VX, VY ; set VF = carry.
	uint16_t r = VX + VY;
	VX = r % 256;
	VF = r > 0xFF ? 1 : 0;
	chip8->pc += 2;
}
inline void chip8_8XY5(CHIP8* chip8) {
	// SUB VX, VY ; set VF = NOT borrow.
	uint8_t vf = VX < VY ? 0 : 1;
	VX -= VY;
	VF = vf;
	chip8->pc += 2;
}
inline void chip8_8XY6(CHIP8* chip8) {
	// SHR VX, VY
	uint8_t vf = VX & 0x1;
	if (chip8->config.quirks & CHIP8_QUIRK_SHIFT_X_REGISTER) {
		VX >>= 1;
	}
	else {
		VX = VY >> 0x1;
	}
	VF = vf;
	chip8->pc += 2;
}
inline void chip8_8XY7(CHIP8* chip8) {
	// SUBN VX, VY
	uint8_t vf = VY > VX ? 1 : 0;
	VX = VY - VX;
	VF = vf;
	chip8->pc += 2;
}
inline void chip8_8XYE(CHIP8* chip8) {
	// SHL VX, VY
	uint8_t vf = (VX >> 7) & 1;
	if (chip8->config.quirks & CHIP8_QUIRK_SHIFT_X_REGISTER) {
		VX <<= 1;
	}
	else {
		VX = VY << 0x1;
	}
	VF = vf;
	chip8->pc += 2;
}
inline void chip8_9XY0(CHIP8* chip8) {
	// SNE VX, VY
	if (VX != VY) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
inline void chip8_ANNN(CHIP8* chip8) {
	// LD I, NNN
	chip8->i = NNN;
	chip8->pc += 2;
}
inline void chip8_BNNN(CHIP8* chip8) {
	// JMP NNN, V0
	chip8->pc = NNN + chip8->v[0];
}
inline void chip8_CXNN(CHIP8* chip8) {
	// RND VX, NN
	VX = (chip8_random() & NN);
	chip8->pc += 2;
}
inline void chip8_DXYN(CHIP8* chip8) {
	// DRW VX, VY, N
	uint16_t px;
	VF = 0;
	for (int y = 0; y < N; ++y) {
		px = chip8->ram[chip8->i + y];
		for (int x = 0; x < 8; ++x) {
			if ((px & (0x80 >> x)) != 0) {
				if (chip8->video[VX + x + ((VY + y) * 64)] == 1) {
					VF = 1;
				}
				chip8->video[VX + x + ((VY + y) * 64)] ^= 1;
			}
		}
	}
	chip8->pc += 2;
}
inline void chip8_EX9E(CHIP8* chip8) {
	// SKP VX
	if (GET_KEY(chip8->keypad, VX) == 1) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
inline void chip8_EXA1(CHIP8* chip8) {
	// SKNP VX
	if (GET_KEY(chip8->keypad, VX) == 0) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
inline void chip8_FX07(CHIP8* chip8) {
	// LD VX, DT
	VX = chip8->delay_timer;
	chip8->pc += 2;
}
inline void chip8_FX0A(CHIP8* chip8) {
	// LD VX, KEY
	
	for (int i = 0; i < 16; ++i) {
		/* Set key state */
		if (GET_KEY(chip8->keypad, i) == CHIP8_KEY_STATE_KEY_DOWN) {
			SET_KEY(chip8->fxoa_state, i, CHIP8_KEY_STATE_KEY_DOWN);
		}

		/* Check key state */
		if (GET_KEY(chip8->fxoa_state, i) == CHIP8_KEY_STATE_KEY_DOWN) {
			if (GET_KEY(chip8->keypad, i) == CHIP8_KEY_STATE_KEY_UP) {
				VX = i;
				chip8->pc += 2;

				/* Reset FX0A state */
				chip8->fxoa_state = 0;
				break;
			}
		}
	}
}
inline void chip8_FX15(CHIP8* chip8) {
	// LD DT, VX
	chip8->delay_timer = VX;
	chip8->pc += 2;
}
inline void chip8_FX18(CHIP8* chip8) {
	// LD ST, VX
	chip8->sound_timer = VX;
	chip8->pc += 2;
}
inline void chip8_FX1E(CHIP8* chip8) {
	// ADD I, VX
	chip8->i += VX;
	chip8->pc += 2;
}
inline void chip8_FX29(CHIP8* chip8) {
	// LD B, VX 
	// Set I = location of sprite for digit Vx.
	chip8->i = VX * 5;
	chip8->pc += 2;
}
inline void chip8_FX33(CHIP8* chip8) {
	// LD B, VX
	// Store BCD representation of Vx in memory locations I, I+1, and I+2.
	chip8->ram[chip8->i] = (VX % 1000) / 100;
	chip8->ram[chip8->i + 1] = (VX % 100) / 10;
	chip8->ram[chip8->i + 2] = (VX % 10);
	chip8->pc += 2;
}
inline void chip8_FX55(CHIP8* chip8) {
	//LD [I], VX
	for (int i = 0; i <= X; ++i) {
		chip8->ram[chip8->i + i] = chip8->v[i];
	}
	chip8->pc += 2;
}
inline void chip8_FX65(CHIP8* chip8) {
	// LD VX, [I]
	for (int i = 0; i <= X; ++i) {
		chip8->v[i] = chip8->ram[chip8->i + i];
	}
	chip8->pc += 2;
}
