// chip8_mnem.c
//
// GitHub: https:\\github.com\tommojphillips

#include "chip8.h"

#define X ((chip8->opcode >> 8) & 0x0F)
#define Y ((chip8->opcode >> 4) & 0x00F)
#define NNN (chip8->opcode & 0x0FFF)
#define NN (chip8->opcode & 0x00FF)
#define N (chip8->opcode & 0x000F)

#include <stdio.h>
#define MNEM_0(x) sprintf_s(chip8->mnem, 32, x)
#define MNEM_1(x, x1) sprintf_s(chip8->mnem, 32, x, x1)
#define MNEM_2(x, x1, x2) sprintf_s(chip8->mnem, 32, x, x1, x2)
#define MNEM_3(x, x1, x2, x3) sprintf_s(chip8->mnem, 32, x, x1, x2, x3)

void chip8_mnem_00E0(CHIP8* chip8) {
	// CLS
	MNEM_0("CLS");
}
void chip8_mnem_00EE(CHIP8* chip8) {
	// RET
	MNEM_0("RET");
}
void chip8_mnem_1NNN(CHIP8* chip8) {
	// JMP NNN
	MNEM_1("JMP 0x%x", NNN);
}
void chip8_mnem_2NNN(CHIP8* chip8) {
	// CALL NNN
	MNEM_1("CALL 0x%x", NNN);
}
void chip8_mnem_3XNN(CHIP8* chip8) {
	// SE VX, NN
	MNEM_2("SE V%X, 0x%x", X, NN);
}
void chip8_mnem_4XNN(CHIP8* chip8) {
	// SNE VX, NN
	MNEM_2("SNE V%X, 0x%x", X, NN);
}
void chip8_mnem_5XY0(CHIP8* chip8) {
	// SE VX, VY
	MNEM_2("SE V%X, V%X", X, Y);
}
void chip8_mnem_6XNN(CHIP8* chip8) {
	// LD VX, NN
	MNEM_2("LD V%X, 0x%x", X, NN);
}
void chip8_mnem_7XNN(CHIP8* chip8) {
	// ADD VX, NN
	MNEM_2("ADD V%X, 0x%x", X, NN);
}
void chip8_mnem_8XY0(CHIP8* chip8) {
	// LD VX, VY
	MNEM_2("LD V%X, V%X", X, Y);
}
void chip8_mnem_8XY1(CHIP8* chip8) {
	// VX=VX│VY
	MNEM_2("OR V%X, V%X", X, Y);
}
void chip8_mnem_8XY2(CHIP8* chip8) {
	// VX=VX&VY
	MNEM_2("AND V%X, V%X", X, Y);
}
void chip8_mnem_8XY3(CHIP8* chip8) {
	// VX=VX^VY
	MNEM_2("XOR V%X, V%X", X, Y);
}
void chip8_mnem_8XY4(CHIP8* chip8) {
	// Set Vx = Vx + Vy, set VF = carry.
	MNEM_2("ADD V%X, V%X", X, Y);
}
void chip8_mnem_8XY5(CHIP8* chip8) {
	// SUB VX, VY	
	MNEM_2("SUB V%X, V%X", X, Y);
}
void chip8_mnem_8XY6(CHIP8* chip8) {
	// SHR VX
	MNEM_1("SHR V%X", X);
}
void chip8_mnem_8XY7(CHIP8* chip8) {
	// SUBN VX, VY
	MNEM_2("SUBN V%X, V%X", X, Y);
}
void chip8_mnem_8XYE(CHIP8* chip8) {
	// SHL VX
	MNEM_1("SHL V%X", X);
}
void chip8_mnem_9XY0(CHIP8* chip8) {
	// SNE VX, VY
	MNEM_2("SNE V%X, V%X", X, Y);
}
void chip8_mnem_ANNN(CHIP8* chip8) {
	// LD I, NNN
	MNEM_1("LD I, 0x%x", NNN);
}
void chip8_mnem_BNNN(CHIP8* chip8) {
	// JMP NNN, V0
	MNEM_1("JMP 0x%x, V0", NNN);
}
void chip8_mnem_CXNN(CHIP8* chip8) {
	// RND VX, NN
	MNEM_2("RND V%X, 0x%x", X, NN);
}
void chip8_mnem_DXYN(CHIP8* chip8) {
	// DRW VX, VY, N
	MNEM_3("DRW V%X, V%X, 0x%x", X, Y, N);
}
void chip8_mnem_EX9E(CHIP8* chip8) {
	// SKP VX
	MNEM_1("SKP V%X", X);
}
void chip8_mnem_EXA1(CHIP8* chip8) {
	// SKNP VX
	MNEM_1("SKNP V%X", X);
}
void chip8_mnem_FX07(CHIP8* chip8) {
	// LD VX, DT
	MNEM_1("LD V%X, DT", X);
}
void chip8_mnem_FX0A(CHIP8* chip8) {
	// LD VX, KEY
	MNEM_1("LD V%X, KEY", X);
}
void chip8_mnem_FX15(CHIP8* chip8) {
	// LD DT, VX
	MNEM_1("LD DT, V%X", X);
}
void chip8_mnem_FX18(CHIP8* chip8) {
	// LD ST, VX
	MNEM_1("LD ST, V%X", X);
}
void chip8_mnem_FX1E(CHIP8* chip8) {
	// ADD I, VX
	MNEM_1("ADD I, V%X", X);
}
void chip8_mnem_FX29(CHIP8* chip8) {
	// LD B, VX
	MNEM_1("LD F, V%X", X);
}
void chip8_mnem_FX33(CHIP8* chip8) {
	// LD B, VX
	MNEM_1("LD B, V%X", X);
}
void chip8_mnem_FX55(CHIP8* chip8) {
	//LD [I], VX
	MNEM_1("LD [I], V%X", X);
}
void chip8_mnem_FX65(CHIP8* chip8) {
	// LD VX, [I]
	MNEM_1("LD V%X, [I]", X);
}

int chip8_mnem(CHIP8* chip8) {

	switch (chip8->opcode >> 12) {

		case 0x0: {
			switch (chip8->opcode & 0x00FF) {

				case 0xE0: // CLS
					chip8_mnem_00E0(chip8);
					break;
				case 0xEE: // RET
					chip8_mnem_00EE(chip8);
					break;
				default:
					return 1;
			}
		} break;

		case 0x1: // JMP NNN
			chip8_mnem_1NNN(chip8);
			break;

		case 0x2: // CALL NNN
			chip8_mnem_2NNN(chip8);
			break;

		case 0x3: // SE VX, NN
			chip8_mnem_3XNN(chip8);
			break;

		case 0x4: // SNE VX, NN
			chip8_mnem_4XNN(chip8);
			break;

		case 0x5: // SE VX, VY
			chip8_mnem_5XY0(chip8);
			break;

		case 0x6: // LD VX, NN
			chip8_mnem_6XNN(chip8);
			break;

		case 0x7: // ADD VX, NN
			chip8_mnem_7XNN(chip8);
			break;

		case 0x8: {
			switch (chip8->opcode & 0x000F) {

				case 0x00: // LD VX, VY
					chip8_mnem_8XY0(chip8);
					break;
				case 0x01: // OR VX, VY
					chip8_mnem_8XY1(chip8);
					break;
				case 0x02: // AND VX, VY
					chip8_mnem_8XY2(chip8);
					break;
				case 0x03: // XOR VX, VY
					chip8_mnem_8XY3(chip8);
					break;
				case 0x04: // ADD VX, VY
					chip8_mnem_8XY4(chip8);
					break;
				case 0x05: // SUB VX, VY
					chip8_mnem_8XY5(chip8);
					break;
				case 0x06: // SHL VX, VY
					chip8_mnem_8XY6(chip8);
					break;
				case 0x07: // SUBN VX, VY
					chip8_mnem_8XY7(chip8);
					break;
				case 0x0E: // SHR VX, VY
					chip8_mnem_8XYE(chip8);
					break;
				default:
					return 1;
			}
		} break;

		case 0x9: // SNE VX, VY
			chip8_mnem_9XY0(chip8);
			break;
		case 0xA: // LD I, NNN 
			chip8_mnem_ANNN(chip8);
			break;
		case 0xB: // JMP NNN, V0
			chip8_mnem_BNNN(chip8);
			break;
		case 0xC: // RND VX, NN
			chip8_mnem_CXNN(chip8);
			break;
		case 0xD: // DSP VX, VY, N
			chip8_mnem_DXYN(chip8);
			break;

		case 0xE: {
			switch (chip8->opcode & 0x00FF) {

				case 0x9E: // SKP VX
					chip8_mnem_EX9E(chip8);
					break;
				case 0xA1: // SKNP VX 
					chip8_mnem_EXA1(chip8);
					break;
				default:
					return 1;
			}
		} break;

		case 0xF: {
			switch (chip8->opcode & 0x00FF) {

				case 0x07: // LD VX, DT
					chip8_mnem_FX07(chip8);
					break;
				case 0x0A: // LD VX, KEY
					chip8_mnem_FX0A(chip8);
					break;
				case 0x15: // LD DT, VX
					chip8_mnem_FX15(chip8);
					break;
				case 0x18: // LD ST, VX
					chip8_mnem_FX18(chip8);
					break;
				case 0x1E: // ADD I, VX
					chip8_mnem_FX1E(chip8);
					break;
				case 0x29: // LD F, VX 
					chip8_mnem_FX29(chip8);
					break;
				case 0x33: // LD B, VX
					chip8_mnem_FX33(chip8);
					break;
				case 0x55: // LD [I], VX 
					chip8_mnem_FX55(chip8);
					break;
				case 0x65: // LD VX, [I] 
					chip8_mnem_FX65(chip8);
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