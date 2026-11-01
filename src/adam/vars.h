#ifdef __ADAM__

#ifndef KEYMAP_H
#define KEYMAP_H

#define WIDTH 32
#define HEIGHT 24
#define STATUS_TIMER_WIDTH 1

#define CARD_HEIGHT 5
#define CARD_EDGE 0
#define CARD_SPILL 1
#define SEAT_TOP_Y 9
#define SEAT_BOTTOM_Y 18

#define QUERY_SUFFIX ""

// One short of the bottom right cell, which stays blank
#define TABLE_HINTS "r-efresh  h-elp   n-ame   q-uit"

// Adam arrow codes, 5cardstud's alternates, then , . - =
#define KEY_LEFT_ARROW      0xA3
#define KEY_LEFT_ARROW_2    0x9D
#define KEY_LEFT_ARROW_3    0x2C

#define KEY_RIGHT_ARROW     0xA1
#define KEY_RIGHT_ARROW_2   0x1D
#define KEY_RIGHT_ARROW_3   0x2E

#define KEY_UP_ARROW        0xA0
#define KEY_UP_ARROW_2      0x91
#define KEY_UP_ARROW_3      0x2D

#define KEY_DOWN_ARROW      0xA2
#define KEY_DOWN_ARROW_2    0x11
#define KEY_DOWN_ARROW_3    0x3D

#define KEY_RETURN       0x0D
#define KEY_ESCAPE       0x1B
#define KEY_ESCAPE_ALT   0x03
#define ESC_KEY_NAME     "esc"
#define KEY_SPACE        0x20
#define KEY_BACKSPACE    0x08

#define JOY_UP(v)     ((v) & 0x01)
#define JOY_RIGHT(v)  ((v) & 0x02)
#define JOY_DOWN(v)   ((v) & 0x04)
#define JOY_LEFT(v)   ((v) & 0x08)
#define JOY_BTN_1(v)  ((v) & 0x10)

// The EOS keyboard, not the z88dk console's
#undef cgetc
#undef kbhit
#define cgetc adamCgetc
#define kbhit adamKbhit
unsigned char adamKbhit(void);
unsigned char adamCgetc(void);

#endif /* KEYMAP_H */

#endif /* __ADAM__ */
