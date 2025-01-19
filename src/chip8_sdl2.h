/* chip8_sdl2.h
* GitHub: https:\\github.com\tommojphillips
*/

#include <stdint.h>

#include "chip8.h" // chip8 cpu core

/* Window width*/
#define CFG_WINDOW_W (window_state.win_w)

/* Window height*/
#define CFG_WINDOW_H (window_state.win_h)

/* Display width */
#define CFG_DISPLAY_W (chip8_config.win_w)

/* Display height */
#define CFG_DISPLAY_H (chip8_config.win_h)

/* Display Y offset */
#define CFG_DISPLAY_X (chip8_config.win_x)

/* Display X offset */
#define CFG_DISPLAY_Y (chip8_config.win_y)

/* Display pixel space */
#define CFG_PX_SPACE (chip8_config.pixel_spacing)

#define DISPLAY_PX_SIZE (CFG_DISPLAY_W / CHIP8_DISPLAY_WIDTH )

#define DISPLAY_WIDTH (CHIP8_DISPLAY_WIDTH * (DISPLAY_PX_SIZE + CFG_PX_SPACE))
#define DISPLAY_HEIGHT (CHIP8_DISPLAY_HEIGHT * (DISPLAY_PX_SIZE + CFG_PX_SPACE))

#define DISPLAY_WIDTH_OFFSET (((CFG_WINDOW_W - DISPLAY_WIDTH ) >> 1) - CFG_DISPLAY_X)
#define DISPLAY_HEIGHT_OFFSET (((CFG_WINDOW_H - DISPLAY_HEIGHT ) >> 1) - CFG_DISPLAY_Y)

#define PX_OFFSET(i) ((i) * (DISPLAY_PX_SIZE + CFG_PX_SPACE))

#define PX_X(i) (DISPLAY_WIDTH_OFFSET + PX_OFFSET((i) & (CHIP8_DISPLAY_WIDTH-1)))
#define PX_Y(i) (DISPLAY_HEIGHT_OFFSET + PX_OFFSET((i) / CHIP8_DISPLAY_WIDTH))
#define PX_W (DISPLAY_PX_SIZE)
#define PX_H (DISPLAY_PX_SIZE)

#define DISPLAY_X_LIMIT ((CFG_WINDOW_W - DISPLAY_WIDTH) >> 1)
#define DISPLAY_Y_LIMIT ((CFG_WINDOW_H - DISPLAY_HEIGHT) >> 1)
#define DISPLAY_W_LIMIT (CFG_WINDOW_W - CFG_DISPLAY_X - (CHIP8_DISPLAY_WIDTH * CFG_PX_SPACE))
#define DISPLAY_PX_LIMIT ((CFG_WINDOW_W / CHIP8_DISPLAY_WIDTH) - 1)

/* Single Step State */
enum {
	/* Not single stepping */
	SINGLE_STEP_NONE = 0,

	/* Step instruction */
	SINGLE_STEP_EXE = 1
};

/* Chip8 pixel color */
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} PIXEL_COLOR;

/* Chip8 configuration */
typedef struct {
	int render_target; // renderer update target in hz
	int cpu_target; // cpu update target in hz
	int timer_target; // timer update target in hz
	int win_x;
	int win_y;
	int win_h;
	int win_w;
	float win_s;
	int pixel_spacing;
	int quirk_cls_on_reset;
	int quirk_zero_vf_register;
	int quirk_shift_x_register;
	int quirk_increment_i_register;
	int quirk_jump;
	int quirk_display_clipping;
	PIXEL_COLOR on_color;
	PIXEL_COLOR off_color;
} CHIP8_CONFIG;

/* Chip8 state */
typedef struct {
	int single_step;
	char mnem_str[32];
} CHIP8_STATE;


#ifdef __cplusplus
extern "C" {
#endif

extern CHIP8* chip8;
extern CHIP8_CONFIG chip8_config;
extern CHIP8_STATE chip8_state;

void chip8_init();
void chip8_destroy();

void chip8_update();
void chip8_reset();

int load_program(const char* filename);

#ifdef __cplusplus
};
#endif
