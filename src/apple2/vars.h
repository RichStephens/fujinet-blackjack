#ifdef __APPLE2__

#ifndef KEYMAP_H
#define KEYMAP_H

#include <joystick.h>

#define WIDTH 40
#define HEIGHT 24
#define STATUS_TIMER_WIDTH 2

#define CARD_HEIGHT 4
#define CARD_EDGE 0
#define SEAT_TOP_Y 10
#define SEAT_BOTTOM_Y 18

#define QUERY_SUFFIX ""

#define KEY_LEFT_ARROW      0x08
#define KEY_LEFT_ARROW_2    0x9D
#define KEY_LEFT_ARROW_3    0x2C // ,

#define KEY_RIGHT_ARROW     0x15
#define KEY_RIGHT_ARROW_2   0x1D
#define KEY_RIGHT_ARROW_3   0x2E // .

#define KEY_UP_ARROW        0x0B
#define KEY_UP_ARROW_2      0x91
#define KEY_UP_ARROW_3      0x2D // -

#define KEY_DOWN_ARROW      0x0A
#define KEY_DOWN_ARROW_2    0x11
#define KEY_DOWN_ARROW_3    0x3D // =

#define KEY_RETURN       0x0D
#define KEY_ESCAPE       0x1B
#define KEY_ESCAPE_ALT   0x03
#define ESC_KEY_NAME     "esc"
#define KEY_SPACE        0x20
#define KEY_BACKSPACE    0x08

#endif /* KEYMAP_H */

#endif /* __APPLE2__ */
