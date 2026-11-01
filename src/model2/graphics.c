#ifdef __TRS80M2__

#include <string.h>
#include "con.h"
#include "vid.h"
#include "../platform-specific/graphics.h"
#include "../misc.h"

#define STATUS_Y (HEIGHT - 1)
#define FRAME_BOTTOM_Y (HEIGHT - 2)

unsigned char colorMode;

uint8_t cycleNextColor() {
  return 0;
}

void setColorMode(unsigned char mode) {
  (void)mode;
}

void drawTextAt(unsigned char x, unsigned char y, const char *s) {
  vid_text(x, y, s, WIDTH, 0, false);
}

void drawText(unsigned char x, unsigned char y, const char *s) {
  vid_text(x, y, s, WIDTH, 0, false);
}

void drawBuffer() {
  vid_flush();
}

void clearStatusBar() {
  vid_fill(0, STATUS_Y, WIDTH, ' ');
}

// The server sends its text in lower case
static void capitalize(unsigned char x, const char *s) {
  if (s[0] >= 'a' && s[0] <= 'z')
    vid_put(x, STATUS_Y, s[0] - 32);
}

void drawStatusTextAt(unsigned char x, const char *s) {
  vid_text(x, STATUS_Y, s, WIDTH, 0, false);
  capitalize(x, s);
}

void drawStatusText(const char *s) {
  vid_text(0, STATUS_Y, s, WIDTH, 0, true);
  capitalize(0, s);
}

void drawStatusTimer() {
}

void drawChip(unsigned char x, unsigned char y) {
  vid_put(x, y, '$');
}

void m2TextCursor(unsigned char x, unsigned char y) {
  vid_put(x, y, ' ' | REV);
}

static void frame(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  static unsigned char i;

  vid_put(x, y, G_CORNER_UL);
  vid_fill(x + 1, y, w, G_HLINE);
  vid_put(x + w + 1, y, G_CORNER_UR);
  for (i = 1; i <= h; i++) {
    vid_put(x, y + i, G_VLINE);
    vid_put(x + w + 1, y + i, G_VLINE);
  }
  vid_put(x, y + h + 1, G_CORNER_LL);
  vid_fill(x + 1, y + h + 1, w, G_HLINE);
  vid_put(x + w + 1, y + h + 1, G_CORNER_LR);
}

// Leaves the status bar, which a screen may set before clearing the table
void resetScreen() {
  static unsigned char y;

  for (y = 0; y < STATUS_Y; y++)
    vid_fill(0, y, WIDTH, ' ');
  frame(0, 0, WIDTH - 2, FRAME_BOTTOM_Y - 1);
}

// Red suits in reverse video
void drawCard(unsigned char x, unsigned char y, unsigned char style, const char *s) {
  static char r1, r2, suit;

  (void)style;
  frame(x, y, 2, 2);

  if (s[0] == '?') {
    vid_fill(x + 1, y + 1, 2, ' ' | REV);
    vid_fill(x + 1, y + 2, 2, ' ' | REV);
    return;
  }

  r2 = ' ';
  switch (s[0]) {
    case 't': r1 = '1'; r2 = '0'; break;
    case 'a': r1 = 'A'; break;
    case 'j': r1 = 'J'; break;
    case 'q': r1 = 'Q'; break;
    case 'k': r1 = 'K'; break;
    default: r1 = s[0];
  }
  suit = s[1] - 32;
  if (s[1] == 'h' || s[1] == 'd')
    suit |= REV;

  vid_put(x + 1, y + 1, r1);
  vid_put(x + 2, y + 1, r2);
  vid_put(x + 1, y + 2, ' ');
  vid_put(x + 2, y + 2, suit);
}

// The move cursor on the status bar is reverse video; elsewhere a rule
void drawLine(unsigned char x, unsigned char y, unsigned char w) {
  if (y == STATUS_Y)
    vid_rev(x, y, w, true);
  else
    vid_fill(x, y, w, G_HLINE_THIN);
}

void hideLine(unsigned char x, unsigned char y, unsigned char w) {
  if (y == STATUS_Y)
    vid_rev(x, y, w, false);
  else
    vid_fill(x, y, w, ' ');
}

void drawUnderline(unsigned char x, unsigned char y, unsigned char w) {
  vid_rev(x, y, w, true);
}

void clearArea(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  while (h--)
    vid_fill(x, y++, w, ' ');
}

void drawBox(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  frame(x, y, w, h);
}

void drawBorder() {
  drawCard(2, 1, FULL_CARD, "as");
  drawCard(WIDTH - 2 - CARD_W, 1, FULL_CARD, "ah");
  drawCard(2, FRAME_BOTTOM_Y - CARD_HEIGHT, FULL_CARD, "ad");
  drawCard(WIDTH - 2 - CARD_W, FRAME_BOTTOM_Y - CARD_HEIGHT, FULL_CARD, "ac");
}

void drawLogo() {
  static unsigned char y;

  y = 5;
  vid_text(WIDTH/2 - 8, y++, "", 16, REV, true);
  vid_text(WIDTH/2 - 8, y++, "    FUJINET     ", 16, REV, true);
  vid_text(WIDTH/2 - 8, y++, "   BLACKJACK    ", 16, REV, true);
  vid_text(WIDTH/2 - 8, y, "", 16, REV, true);
}

void initGraphics() {
  vid_init();
}

void resetGraphics() {
  vid_exit();
}

// 60ths of a second on P&T's 1/100 s clock: 2, 2, then 1 tick
void waitvsync() {
  static uint16_t last;
  static uint8_t phase, len;

  vid_flush();
  len = phase == 2 ? 1 : 2;
  while ((uint16_t)((uint16_t)con_ticks() - last) < len)
    ;
  last = (uint16_t)con_ticks();
  phase = phase == 2 ? 0 : phase + 1;
}

#endif /* __TRS80M2__ */
