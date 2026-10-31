#ifdef __ATARI__

#ifndef KEYMAP_H
#define KEYMAP_H

#include <atari.h>
#include <joystick.h>

#define WIDTH 40
#define HEIGHT 26
#define STATUS_TIMER_WIDTH 1

#define CARD_HEIGHT 5
#define CARD_EDGE 1
#define SEAT_TOP_Y 11
#define SEAT_BOTTOM_Y 20

#define QUERY_SUFFIX ""

#define KEY_LEFT_ARROW      CH_CURS_LEFT
#define KEY_LEFT_ARROW_2    43 // +
#define KEY_LEFT_ARROW_3    60 // <

#define KEY_RIGHT_ARROW     CH_CURS_RIGHT
#define KEY_RIGHT_ARROW_2   42 // *
#define KEY_RIGHT_ARROW_3   62 // >

#define KEY_UP_ARROW        CH_CURS_UP
#define KEY_UP_ARROW_2      45 // -
#define KEY_UP_ARROW_3      2

#define KEY_DOWN_ARROW      CH_CURS_DOWN
#define KEY_DOWN_ARROW_2    61 // =
#define KEY_DOWN_ARROW_3    3

#define KEY_ESCAPE       CH_ESC
#define KEY_ESCAPE_ALT   1
#define ESC_KEY_NAME     "esc"

#undef KEY_SPACE
#define KEY_SPACE        0x20 // cc65 atari.h has the wrong value
#define KEY_BACKSPACE    CH_DEL

#undef KEY_RETURN
#define KEY_RETURN       CH_ENTER

#endif /* KEYMAP_H */

#endif /* __ATARI__ */
