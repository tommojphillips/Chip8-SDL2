// chip8_mnem.c
//
// GitHub: https:\\github.com\tommojphillips

#include <stdio.h>

#include "chip8.h"
#include "chip8_mnem.h"

#define X ((mnem->chip8->opcode >> 8) & 0x0F)
#define Y ((mnem->chip8->opcode >> 4) & 0x00F)
#define NNN (mnem->chip8->opcode & 0x0FFF)
#define NN (mnem->chip8->opcode & 0x00FF)
#define N (mnem->chip8->opcode & 0x000F)

#define MNEM_0(x) sprintf_s(mnem->str, 32, x)
#define MNEM_1(x, x1) sprintf_s(mnem->str, 32, x, x1)
#define MNEM_2(x, x1, x2) sprintf_s(mnem->str, 32, x, x1, x2)
#define MNEM_3(x, x1, x2, x3) sprintf_s(mnem->str, 32, x, x1, x2, x3)

void chip8_mnem_00E0(CHIP8_MNEM* mnem) {
	// CLS
	MNEM_0("CLS");
}
void chip8_mnem_00EE(CHIP8_MNEM* mnem) {
	// RET
	MNEM_0("RET");
}
void chip8_mnem_1NNN(CHIP8_MNEM* mnem) {
	// JMP NNN
	MNEM_1("JMP 0x%x", NNN);
}
void chip8_mnem_2NNN(CHIP8_MNEM* mnem) {
	// CALL NNN
	MNEM_1("CALL 0x%x", NNN);
}
void chip8_mnem_3XNN(CHIP8_MNEM* mnem) {
	// SE VX, NN
	MNEM_2("SE V%X, 0x%x", X, NN);
}
void chip8_mnem_4XNN(CHIP8_MNEM* mnem) {
	// SNE VX, NN
	MNEM_2("SNE V%X, 0x%x", X, NN);
}
void chip8_mnem_5XY0(CHIP8_MNEM* mnem) {
	// SE VX, VY
	MNEM_2("SE V%X, V%X", X, Y);
}
void chip8_mnem_6XNN(CHIP8_MNEM* mnem) {
	// LD VX, NN
	MNEM_2("LD V%X, 0x%x", X, NN);
}
void chip8_mnem_7XNN(CHIP8_MNEM* mnem) {
	// ADD VX, NN
	MNEM_2("ADD V%X, 0x%x", X, NN);
}
void chip8_mnem_8XY0(CHIP8_MNEM* mnem) {
	// LD VX, VY
	MNEM_2("LD V%X, V%X", X, Y);
}
void chip8_mnem_8XY1(CHIP8_MNEM* mnem) {
	// VX=VX│VY
	MNEM_2("OR V%X, V%X", X, Y);
}
void chip8_mnem_8XY2(CHIP8_MNEM* mnem) {
	// VX=VX&VY
	MNEM_2("AND V%X, V%X", X, Y);
}
void chip8_mnem_8XY3(CHIP8_MNEM* mnem) {
	// VX=VX^VY
	MNEM_2("XOR V%X, V%X", X, Y);
}
void chip8_mnem_8XY4(CHIP8_MNEM* mnem) {
	// Set Vx = Vx + Vy, set VF = carry.
	MNEM_2("ADD V%X, V%X", X, Y);
}
void chip8_mnem_8XY5(CHIP8_MNEM* mnem) {
	// SUB VX, VY	
	MNEM_2("SUB V%X, V%X", X, Y);
}
void chip8_mnem_8XY6(CHIP8_MNEM* mnem) {
	// SHR VX
	MNEM_1("SHR V%X", X);
}
void chip8_mnem_8XY7(CHIP8_MNEM* mnem) {
	// SUBN VX, VY
	MNEM_2("SUBN V%X, V%X", X, Y);
}
void chip8_mnem_8XYE(CHIP8_MNEM* mnem) {
	// SHL VX
	MNEM_1("SHL V%X", X);
}
void chip8_mnem_9XY0(CHIP8_MNEM* mnem) {
	// SNE VX, VY
	MNEM_2("SNE V%X, V%X", X, Y);
}
void chip8_mnem_ANNN(CHIP8_MNEM* mnem) {
	// LD I, NNN
	MNEM_1("LD I, 0x%x", NNN);
}
void chip8_mnem_BNNN(CHIP8_MNEM* mnem) {
	// JMP NNN, V0
	MNEM_1("JMP 0x%x, V0", NNN);
}
void chip8_mnem_CXNN(CHIP8_MNEM* mnem) {
	// RND VX, NN
	MNEM_2("RND V%X, 0x%x", X, NN);
}
void chip8_mnem_DXYN(CHIP8_MNEM* mnem) {
	// DRW VX, VY, N
	MNEM_3("DRW V%X, V%X, 0x%x", X, Y, N);
}
void chip8_mnem_EX9E(CHIP8_MNEM* mnem) {
	// SKP VX
	MNEM_1("SKP V%X", X);
}
void chip8_mnem_EXA1(CHIP8_MNEM* mnem) {
	// SKNP VX
	MNEM_1("SKNP V%X", X);
}
void chip8_mnem_FX07(CHIP8_MNEM* mnem) {
	// LD VX, DT
	MNEM_1("LD V%X, DT", X);
}
void chip8_mnem_FX0A(CHIP8_MNEM* mnem) {
	// LD VX, KEY
	MNEM_1("LD V%X, KEY", X);
}
void chip8_mnem_FX15(CHIP8_MNEM* mnem) {
	// LD DT, VX
	MNEM_1("LD DT, V%X", X);
}
void chip8_mnem_FX18(CHIP8_MNEM* mnem) {
	// LD ST, VX
	MNEM_1("LD ST, V%X", X);
}
void chip8_mnem_FX1E(CHIP8_MNEM* mnem) {
	// ADD I, VX
	MNEM_1("ADD I, V%X", X);
}
void chip8_mnem_FX29(CHIP8_MNEM* mnem) {
	// LD B, VX
	MNEM_1("LD F, V%X", X);
}
void chip8_mnem_FX33(CHIP8_MNEM* mnem) {
	// LD B, VX
	MNEM_1("LD B, V%X", X);
}
void chip8_mnem_FX55(CHIP8_MNEM* mnem) {
	//LD [I], VX
	MNEM_1("LD [I], V%X", X);
}
void chip8_mnem_FX65(CHIP8_MNEM* mnem) {
	// LD VX, [I]
	MNEM_1("LD V%X, [I]", X);
}

int chip8_mnem(CHIP8_MNEM* mnem) {

	switch (mnem->chip8->opcode >> 12) {

		case 0x0: {
			switch (mnem->chip8->opcode & 0x00FF) {

				case 0xE0: // CLS
					chip8_mnem_00E0(mnem);
					break;
				case 0xEE: // RET
					chip8_mnem_00EE(mnem);
					break;
				default:
					return 1;
			}
		} break;

		case 0x1: // JMP NNN
			chip8_mnem_1NNN(mnem);
			break;

		case 0x2: // CALL NNN
			chip8_mnem_2NNN(mnem);
			break;

		case 0x3: // SE VX, NN
			chip8_mnem_3XNN(mnem);
			break;

		case 0x4: // SNE VX, NN
			chip8_mnem_4XNN(mnem);
			break;

		case 0x5: // SE VX, VY
			chip8_mnem_5XY0(mnem);
			break;

		case 0x6: // LD VX, NN
			chip8_mnem_6XNN(mnem);
			break;

		case 0x7: // ADD VX, NN
			chip8_mnem_7XNN(mnem);
			break;

		case 0x8: {
			switch (mnem->chip8->opcode & 0x000F) {

				case 0x00: // LD VX, VY
					chip8_mnem_8XY0(mnem);
					break;
				case 0x01: // OR VX, VY
					chip8_mnem_8XY1(mnem);
					break;
				case 0x02: // AND VX, VY
					chip8_mnem_8XY2(mnem);
					break;
				case 0x03: // XOR VX, VY
					chip8_mnem_8XY3(mnem);
					break;
				case 0x04: // ADD VX, VY
					chip8_mnem_8XY4(mnem);
					break;
				case 0x05: // SUB VX, VY
					chip8_mnem_8XY5(mnem);
					break;
				case 0x06: // SHL VX, VY
					chip8_mnem_8XY6(mnem);
					break;
				case 0x07: // SUBN VX, VY
					chip8_mnem_8XY7(mnem);
					break;
				case 0x0E: // SHR VX, VY
					chip8_mnem_8XYE(mnem);
					break;
				default:
					return 1;
			}
		} break;

		case 0x9: // SNE VX, VY
			chip8_mnem_9XY0(mnem);
			break;
		case 0xA: // LD I, NNN 
			chip8_mnem_ANNN(mnem);
			break;
		case 0xB: // JMP NNN, V0
			chip8_mnem_BNNN(mnem);
			break;
		case 0xC: // RND VX, NN
			chip8_mnem_CXNN(mnem);
			break;
		case 0xD: // DSP VX, VY, N
			chip8_mnem_DXYN(mnem);
			break;

		case 0xE: {
			switch (mnem->chip8->opcode & 0x00FF) {

				case 0x9E: // SKP VX
					chip8_mnem_EX9E(mnem);
					break;
				case 0xA1: // SKNP VX 
					chip8_mnem_EXA1(mnem);
					break;
				default:
					return 1;
			}
		} break;

		case 0xF: {
			switch (mnem->chip8->opcode & 0x00FF) {

				case 0x07: // LD VX, DT
					chip8_mnem_FX07(mnem);
					break;
				case 0x0A: // LD VX, KEY
					chip8_mnem_FX0A(mnem);
					break;
				case 0x15: // LD DT, VX
					chip8_mnem_FX15(mnem);
					break;
				case 0x18: // LD ST, VX
					chip8_mnem_FX18(mnem);
					break;
				case 0x1E: // ADD I, VX
					chip8_mnem_FX1E(mnem);
					break;
				case 0x29: // LD F, VX 
					chip8_mnem_FX29(mnem);
					break;
				case 0x33: // LD B, VX
					chip8_mnem_FX33(mnem);
					break;
				case 0x55: // LD [I], VX 
					chip8_mnem_FX55(mnem);
					break;
				case 0x65: // LD VX, [I] 
					chip8_mnem_FX65(mnem);
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