// chip8_opcodes.c
//
// GitHub: https:\\github.com\tommojphillips

#include <stdlib.h> // for rand() in CXKK

#include "chip8.h"

#define X ((chip8->opcode >> 8) & 0x0F)
#define Y ((chip8->opcode >> 4) & 0x00F)
#define MMM (chip8->opcode & 0x0FFF)
#define KK (chip8->opcode & 0x00FF)
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
void chip8_1MMM(CHIP8* chip8) {
	// JMP MMM
	chip8->pc = MMM;
}
void chip8_2MMM(CHIP8* chip8) {
	// CALL MMM
	chip8->sp += 1;
	chip8->stack[chip8->sp] = chip8->pc;
	chip8->pc = MMM;
}
void chip8_3XKK(CHIP8* chip8) {
	// SE VX, KK
	if (VX == KK) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
void chip8_4XKK(CHIP8* chip8) {
	// SNE VX, KK
	if (VX != KK) {
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
void chip8_6XKK(CHIP8* chip8) {
	// LD VX, KK
	VX = KK;
	chip8->pc += 2;
}
void chip8_7XKK(CHIP8* chip8) {
	// ADD VX, KK
	VX += KK;
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
	chip8->pc += 2;
}
void chip8_8XY2(CHIP8* chip8) {
	// AND VX, VY
	VX &= VY;
	chip8->pc += 2;
}
void chip8_8XY3(CHIP8* chip8) {
	// XOR VX, VY
	VX ^= VY;
	chip8->pc += 2;
}
void chip8_8XY4(CHIP8* chip8) {
	// Set Vx = Vx + Vy, set VF = carry.
	uint16_t r = VX + VY;
	VX = r % 256;
	VF = r > 0xFF ? 1 : 0;
	chip8->pc += 2;
}
void chip8_8XY5(CHIP8* chip8) {
	// Set Vx = Vx - Vy, set VF = NOT borrow.
	uint8_t vx = VX;
	VX -= VY;
	VF = VY < VX ? 0 : 1;
	chip8->pc += 2;
}
void chip8_8XY6(CHIP8* chip8) {
	// SHR VX, VY
	uint8_t r = VX;
	VX = VY >> 0x1;
	VF = r & 0x1;
	chip8->pc += 2;
}
void chip8_8X06(CHIP8* chip8) {
	// SHR VX
	uint8_t r = VX;
	VX >>= 0x1;
	VF = r & 0x1;
	chip8->pc += 2;
}
void chip8_8XY7(CHIP8* chip8) {
	// VX=VY-VX ; if VY > VX, then VF = 1
	VX = VY - VX;
	VF = VY > VX ? 1 : 0;
	chip8->pc += 2;
}
void chip8_8XYE(CHIP8* chip8) {
	// SHL VX, VY
	uint8_t r = VX;
	VX = VY << 0x1;
	VF = (r >> 7) & 1;
	chip8->pc += 2;
}
void chip8_8X0E(CHIP8* chip8) {
	// SHL VX
	uint8_t r = VX;
	VX <<= 1;
	VF = (r >> 7) & 1;
	chip8->pc += 2;
}
void chip8_9XY0(CHIP8* chip8) {
	// SNE VX, VY
	if (VX != VY) {
		chip8->pc += 2;
	}
	chip8->pc += 2;
}
void chip8_AMMM(CHIP8* chip8) {
	// LD I, MMM
	chip8->i = MMM;
	chip8->pc += 2;
}
void chip8_BMMM(CHIP8* chip8) {
	// JMP MMM, V0
	chip8->pc = MMM + chip8->v[0];
}
void chip8_CXKK(CHIP8* chip8) {
	// RND VX, KK
	VX = (rand() % 256) & KK;
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
