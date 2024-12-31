// chip8_opcodes.c
//
// GitHub: https:\\github.com\tommojphillips

#include <stdlib.h> // for rand() in CXNN

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

void chip8_00E0(CHIP8* chip8) {
	// CLS
	chip8_zero_video_memory(chip8);
	chip8->pc += 2;
}
void chip8_00EE(CHIP8* chip8) {
	// RET
	chip8->pc = chip8->stack[chip8->sp];
	chip8->sp -= 1;
	chip8->pc += 2;
}
void chip8_1NNN(CHIP8* chip8) {
	// JMP NNN
	chip8->pc = NNN;
}
void chip8_2NNN(CHIP8* chip8) {
	// CALL NNN
	chip8->sp += 1;
	chip8->stack[chip8->sp] = chip8->pc;
	chip8->pc = NNN;
}
void chip8_3XNN(CHIP8* chip8) {
	// SE VX, NN
	if (VX == NN) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
void chip8_4XNN(CHIP8* chip8) {
	// SNE VX, NN
	if (VX != NN) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
void chip8_5XY0(CHIP8* chip8) {
	// SE VX, VY
	if (VX == VY) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
void chip8_6XNN(CHIP8* chip8) {
	// LD VX, NN
	VX = NN;
	chip8->pc += 2;
}
void chip8_7XNN(CHIP8* chip8) {
	// ADD VX, NN
	VX += NN;
	chip8->pc += 2;
}
void chip8_8XY0(CHIP8* chip8) {
	// LD VX, VY
	VX = VY;
	chip8->pc += 2;
}
void chip8_8XY1(CHIP8* chip8) {
	// OR VX, VY
	VX |= VY;
	if (chip8->config.quirks & CHIP8_QUIRK_lOGICAL_OPERATOR_ZERO_VF) {
		VF = 0;
	}
	chip8->pc += 2;
}
void chip8_8XY2(CHIP8* chip8) {
	// AND VX, VY
	VX &= VY;
	if (chip8->config.quirks & CHIP8_QUIRK_lOGICAL_OPERATOR_ZERO_VF) {
		VF = 0;
	}
	chip8->pc += 2;
}
void chip8_8XY3(CHIP8* chip8) {
	// XOR VX, VY
	VX ^= VY;
	if (chip8->config.quirks & CHIP8_QUIRK_lOGICAL_OPERATOR_ZERO_VF) {
		VF = 0;
	}
	chip8->pc += 2;
}
void chip8_8XY4(CHIP8* chip8) {
	// ADD VX, VY ; set VF = carry.
	uint16_t r = VX + VY;
	VX = r % 256;
	VF = r > 0xFF ? 1 : 0;
	chip8->pc += 2;
}
void chip8_8XY5(CHIP8* chip8) {
	// SUB VX, VY ; set VF = NOT borrow.
	uint8_t vf = VX < VY ? 0 : 1;
	VX -= VY;
	VF = vf;
	chip8->pc += 2;
}
void chip8_8XY6(CHIP8* chip8) {
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
void chip8_8XY7(CHIP8* chip8) {
	// SUBN VX, VY
	uint8_t vf = VY > VX ? 1 : 0;
	VX = VY - VX;
	VF = vf;
	chip8->pc += 2;
}
void chip8_8XYE(CHIP8* chip8) {
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
void chip8_9XY0(CHIP8* chip8) {
	// SNE VX, VY
	if (VX != VY) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
void chip8_ANNN(CHIP8* chip8) {
	// LD I, NNN
	chip8->i = NNN;
	chip8->pc += 2;
}
void chip8_BNNN(CHIP8* chip8) {
	// JMP NNN, V0
	chip8->pc = NNN + chip8->v[0];
}
void chip8_CXNN(CHIP8* chip8) {
	// RND VX, NN
	VX = (rand() % 256) & NN;
	chip8->pc += 2;
}
void chip8_DXYN(CHIP8* chip8) {
	// DRW VX, VY, N
	uint16_t px;
	VF = 0;
	for (int yline = 0; yline < N; yline++) {

		px = chip8->ram[chip8->i + yline];
		for (int xline = 0; xline < 8; xline++) {

			if ((px & (0x80 >> xline)) != 0) {

				if (chip8->video[VX + xline + ((VY + yline) * 64)] == 1) {
					VF = 1;
				}

				chip8->video[VX + xline + ((VY + yline) * 64)] ^= 1;
			}
		}
	}
	chip8->pc += 2;
}
void chip8_EX9E(CHIP8* chip8) {
	// SKP VX
	if (GET_KEY(chip8->keypad, VX) == 1) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
void chip8_EXA1(CHIP8* chip8) {
	// SKNP VX
	if (GET_KEY(chip8->keypad, VX) == 0) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
void chip8_FX07(CHIP8* chip8) {
	// LD VX, DT
	VX = chip8->delay_timer;
	chip8->pc += 2;
}
void chip8_FX0A(CHIP8* chip8) {
	// LD VX, KEY
	
	for (int i = 0; i < 16; ++i) {

		if (GET_KEY(chip8->keypad, i) == CHIP8_KEY_STATE_KEY_DOWN) {
			SET_KEY(chip8->fxoa_state, i, CHIP8_KEY_STATE_KEY_DOWN);
		}

		if (GET_KEY(chip8->fxoa_state, i) == CHIP8_KEY_STATE_KEY_DOWN) {
			if (GET_KEY(chip8->keypad, i) == CHIP8_KEY_STATE_KEY_UP) {
				VX = i;
				chip8->pc += 2;

				// reset FX0A state
				chip8->fxoa_state = 0;
				break;
			}
		}
	}
}
void chip8_FX15(CHIP8* chip8) {
	// LD DT, VX
	chip8->delay_timer = VX;
	chip8->pc += 2;
}
void chip8_FX18(CHIP8* chip8) {
	// LD ST, VX
	chip8->sound_timer = VX;
	chip8->pc += 2;
}
void chip8_FX1E(CHIP8* chip8) {
	// ADD I, VX
	chip8->i += VX;
	chip8->pc += 2;
}
void chip8_FX29(CHIP8* chip8) {
	// LD B, VX 
	// Set I = location of sprite for digit Vx.
	chip8->i = VX * 5;
	chip8->pc += 2;
}
void chip8_FX33(CHIP8* chip8) {
	// LD B, VX
	// Store BCD representation of Vx in memory locations I, I+1, and I+2.
	chip8->ram[chip8->i] = (VX % 1000) / 100;
	chip8->ram[chip8->i + 1] = (VX % 100) / 10;
	chip8->ram[chip8->i + 2] = (VX % 10);
	chip8->pc += 2;
}
void chip8_FX55(CHIP8* chip8) {
	//LD [I], VX
	for (int i = 0; i <= X; ++i) {
		chip8->ram[chip8->i + i] = chip8->v[i];
	}	
	chip8->pc += 2;
}
void chip8_FX65(CHIP8* chip8) {
	// LD VX, [I]
	for (int i = 0; i <= X; ++i) {
		chip8->v[i] = chip8->ram[chip8->i + i];
	}	
	chip8->pc += 2;
}
