// chip8_mnem.h
//
// GitHub: https:\\github.com\tommojphillips

#ifndef CHIP8_MNEM_H
#define CHIP8_MNEM_H

#include <stdint.h>

#include "chip8.h"

/* Chip8 MNEM state structure*/
typedef struct _CHIP8_MNEM {
	CHIP8* chip8;
	char str[32];

} CHIP8_MNEM;

#ifdef __cplusplus
extern "C" {
#endif

int chip8_mnem(CHIP8_MNEM* mnem);

#ifdef __cplusplus
};
#endif
#endif
