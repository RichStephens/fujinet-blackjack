#ifdef __WATCOMC__

#ifndef KEYMAP_H
#define KEYMAP_H

#define WIDTH 40
#define HEIGHT 24
#define STATUS_TIMER_WIDTH 1

#define CARD_HEIGHT 4
#define CARD_EDGE 0
#define SEAT_TOP_Y 10
#define SEAT_BOTTOM_Y 18

#define QUERY_SUFFIX ""

// cgetc returns an arrow's scan code | 0x80
#define KEY_LEFT_ARROW      0xCB
#define KEY_LEFT_ARROW_2    '+'
#define KEY_LEFT_ARROW_3    '<'

#define KEY_RIGHT_ARROW     0xCD
#define KEY_RIGHT_ARROW_2   '*'
#define KEY_RIGHT_ARROW_3   '>'

#define KEY_UP_ARROW        0xC8
#define KEY_UP_ARROW_2      '-'
#define KEY_UP_ARROW_3      0xC9 // PgUp

#define KEY_DOWN_ARROW      0xD0
#define KEY_DOWN_ARROW_2    '='
#define KEY_DOWN_ARROW_3    0xD1 // PgDn

#define KEY_RETURN       0x0D
#define KEY_ESCAPE       0x1B
#define KEY_ESCAPE_ALT   0x03
#define ESC_KEY_NAME     "esc"
#define KEY_SPACE        0x20
#define KEY_BACKSPACE    0x08

#define JOY_UP(v)     (0)
#define JOY_DOWN(v)   (0)
#define JOY_LEFT(v)   (0)
#define JOY_RIGHT(v)  (0)
#define JOY_BTN_1(v)  (0)

// The BIOS keyboard, so DOS never sees a Ctrl-C
#define kbhit biosKbhit
unsigned char biosKbhit(void);
unsigned char cgetc(void);

#endif /* KEYMAP_H */

#endif /* __WATCOMC__ */
