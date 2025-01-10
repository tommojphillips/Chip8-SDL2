// chip8.h
//
// GitHub: https:\\github.com\tommojphillips

#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define CHIP8_REGISTER_COUNT 16
#define CHIP8_STACK_SIZE 16
#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_PROGRAM_ADDR 512
#define CHIP8_VIDEO_SIZE (64 * 32)
#define CHIP8_FONT_SIZE 80

typedef enum {
	CHIP8_STATE_RUN = 0,
	CHIP8_STATE_HALT = 1,
	CHIP8_STATE_OPCODE_ERROR = 2,
} CHIP8_STATE;

typedef enum {
	CHIP8_KEY_STATE_KEY_UP = 0,
	CHIP8_KEY_STATE_KEY_DOWN = 1
} CHIP8_KEY_STATE;

typedef enum {
	CHIP8_QUIRK_NONE = 0,
	CHIP8_QUIRK_SHIFT_X_REGISTER = 2,
	CHIP8_QUIRK_lOGICAL_OPERATOR_ZERO_VF = 4
} CHIP8_QUIRKS;

/* Chip8 config structure*/
typedef struct _CHIP8_CONFIG {
	int quirks;	
} CHIP8_CONFIG;

/* Chip8 state structure*/
typedef struct _CHIP8 {
	uint16_t i;				// I register
	uint16_t pc;			// program counter
	uint16_t sp;			// stack pointer
	uint16_t opcode;		// current opcode
	uint16_t keypad;		// keypad state; 1 bit per key
	uint16_t fxoa_state;	// FX0A state structure
	
	uint8_t delay_timer;
	uint8_t sound_timer;

	uint8_t v[CHIP8_REGISTER_COUNT]; // registers
	uint16_t stack[CHIP8_STACK_SIZE];
	uint8_t ram[CHIP8_MEMORY_SIZE];
	uint8_t video[CHIP8_VIDEO_SIZE];
	
	CHIP8_STATE cpu_state;
	CHIP8_CONFIG config;



} CHIP8;

#ifdef __cplusplus
extern "C" {
#endif

// Initialize chip8 cpu state
void chip8_init(CHIP8* chip8);

// Reset chip8 cpu state
void chip8_reset(CHIP8* chip8);

// Load font into chip8 memory space
void chip8_load_font(CHIP8* chip8, const uint8_t* font);

// Zero chip8 memory space
void chip8_zero_memory(CHIP8* chip8);

// Zero chip8 program space
void chip8_zero_program_memory(CHIP8* chip8);

// Zero chip8 video space
void chip8_zero_video_memory(CHIP8* chip8);

// Step timers
void chip8_step_timers(CHIP8* chip8);

// Decode and execute next instruction
int chip8_execute(CHIP8* chip8);

/* OPCODES */

void chip8_00E0(CHIP8* chip8);
void chip8_00EE(CHIP8* chip8);
void chip8_1NNN(CHIP8* chip8);
void chip8_2NNN(CHIP8* chip8);
void chip8_3XNN(CHIP8* chip8);
void chip8_4XNN(CHIP8* chip8);
void chip8_5XY0(CHIP8* chip8);
void chip8_6XNN(CHIP8* chip8);
void chip8_7XNN(CHIP8* chip8);
void chip8_8XY0(CHIP8* chip8);
void chip8_8XY1(CHIP8* chip8);
void chip8_8XY2(CHIP8* chip8);
void chip8_8XY3(CHIP8* chip8);
void chip8_8XY4(CHIP8* chip8);
void chip8_8XY5(CHIP8* chip8);
void chip8_8XY6(CHIP8* chip8);
void chip8_8XY7(CHIP8* chip8);
void chip8_8XYE(CHIP8* chip8);
void chip8_9XY0(CHIP8* chip8);
void chip8_ANNN(CHIP8* chip8);
void chip8_BNNN(CHIP8* chip8);
void chip8_CXNN(CHIP8* chip8);
void chip8_DXYN(CHIP8* chip8);
void chip8_EX9E(CHIP8* chip8);
void chip8_EXA1(CHIP8* chip8);
void chip8_FX07(CHIP8* chip8);
void chip8_FX0A(CHIP8* chip8);
void chip8_FX15(CHIP8* chip8);
void chip8_FX18(CHIP8* chip8);
void chip8_FX1E(CHIP8* chip8);
void chip8_FX29(CHIP8* chip8);
void chip8_FX33(CHIP8* chip8);
void chip8_FX55(CHIP8* chip8);
void chip8_FX65(CHIP8* chip8);

/*
 *
 * IMPLEMENTATION DEPENDENT
 *
 */

 /* Render implementation */
void chip8_render(CHIP8* chip8);

/* Beep implementation */
void chip8_beep(CHIP8* chip8);

/* Random byte implementation */
uint8_t chip8_random();

#ifdef __cplusplus
};
#endif

#endif
