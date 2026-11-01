#ifdef __APPLE2__

#include <peekpoke.h>
#include <string.h>
#include "hires.h"
#include "text.h"
#include "../platform-specific/graphics.h"
#include "../misc.h"

extern unsigned char charset[];

#define ROW(y) ((y) * 8)
#define TABLE_BOTTOM 177
#define STATUS_TOP 179
#define STATUS_Y 180
#define CURSOR_Y 189

#define BLACK ROP_CONST(0)
#define WHITE ROP_WHITE
// Orange artifact color on the card ink, by column parity
#define RED_ODD ROP_OR(0b11010100)
#define RED_EVEN ROP_OR(0b10101010)

enum {
  CH_FACE = 0x00,
  CH_TABLE_TL = 0x01,
  CH_TABLE_TR = 0x02,
  CH_TABLE_BL = 0x03,
  CH_TABLE_BR = 0x04,
  CH_FACE_LEFT = 0x09,
  CH_SUIT = 0x0A,
  CH_RANK = 0x12,
  CH_TEN = 0x1B,
  CH_JACK = 0x1C,
  CH_TEN_ZERO = 0x1F,
  CH_CHIP = 0x22,
  CH_CLOCK = 0x28,
  CH_BOX_TL = 0x3B,
  CH_BOX_TR = 0x3C,
  CH_BOX_BL = 0x3D,
  CH_BOX_BR = 0x3E,
  CH_BOX_SIDE = 0x3F,
  CH_LINE = 0x40,
  CH_BACK = 0x7B
};

// 0 = red suits (the default), 1 = mono
unsigned char colorMode;

unsigned char cycleNextColor() {
  colorMode = !colorMode;
  return colorMode;
}

void setColorMode(unsigned char mode) {
  colorMode = mode;
}

// Text leaves the blank top row of its cell alone, so underlines survive
void drawTextAt(unsigned char x, unsigned char y, const char *s) {
  static unsigned char c;

  ++y;
  while ((c = *s++)) {
    if (c >= 'a' && c <= 'z')
      c -= 32;
    else if (c == '*')
      c = CH_CHIP;
    hires_Draw(x++, y, 1, 7, ROP_CPY, &charset[(c << 3) + 1]);
  }
}

void drawText(unsigned char x, unsigned char y, const char *s) {
  drawTextAt(x, ROW(y), s);
}

void drawBuffer() {
}

void clearStatusBar() {
  hires_Mask(0, STATUS_TOP, WIDTH, 192 - STATUS_TOP, BLACK);
}

void drawStatusTextAt(unsigned char x, const char *s) {
  drawTextAt(x, STATUS_Y, s);
}

void drawStatusText(const char *s) {
  static unsigned char len;

  len = (unsigned char)strlen(s);
  if (len > WIDTH)
    len = WIDTH;
  memcpy(tempBuffer, s, len);
  tempBuffer[len] = 0;
  drawStatusTextAt(0, tempBuffer);
  if (len < WIDTH)
    hires_Mask(len, STATUS_TOP, WIDTH - len, 192 - STATUS_TOP, BLACK);
}

void drawStatusTimer() {
  hires_putcc(WIDTH - 2, STATUS_Y, ROP_CPY, CH_CLOCK << 8 | (CH_CLOCK + 1));
}

void drawChip(unsigned char x, unsigned char y) {
  hires_putc(x, ROW(y), ROP_CPY, CH_CHIP);
}

void resetScreen() {
  hires_Mask(0, 0, WIDTH, STATUS_TOP, BLACK);

  hires_putc(0, 0, ROP_CPY, CH_TABLE_TL);
  hires_putc(WIDTH - 1, 0, ROP_CPY, CH_TABLE_TR);
  hires_putc(0, TABLE_BOTTOM - 6, ROP_CPY, CH_TABLE_BL);
  hires_putc(WIDTH - 1, TABLE_BOTTOM - 6, ROP_CPY, CH_TABLE_BR);
  hires_Mask(1, 1, WIDTH - 2, 1, WHITE);
  hires_Mask(1, TABLE_BOTTOM, WIDTH - 2, 1, WHITE);
  hires_Mask(0, 8, 1, TABLE_BOTTOM - 14, ROP_CONST(0b00000110));
  hires_Mask(WIDTH - 1, 8, 1, TABLE_BOTTOM - 14, ROP_CONST(0b00110000));
}

/* Ranks are one column wide, so a fanned card drawn a column to the right
   leaves the rank of the card beneath showing. */
static void drawCardAt(unsigned char x, unsigned char y, const char *s) {
  static unsigned char rank, rank2, suit, back;
  static unsigned rop, rop2;

  back = s[0] == '?';
  rank2 = CH_FACE;
  switch (s[0]) {
    case 'a': rank = CH_RANK; break;
    case 't': rank = CH_TEN; rank2 = CH_TEN_ZERO; break;
    case 'j': rank = CH_JACK; break;
    case 'q': rank = CH_JACK + 1; break;
    case 'k': rank = CH_JACK + 2; break;
    default: rank = CH_RANK + s[0] - '1';
  }
  switch (s[1]) {
    case 'h': suit = CH_SUIT; break;
    case 'd': suit = CH_SUIT + 2; break;
    case 'c': suit = CH_SUIT + 4; break;
    default: suit = CH_SUIT + 6;
  }

  rop = rop2 = ROP_CPY;
  if (!colorMode && suit < CH_SUIT + 4) {
    rop = (x & 1) ? RED_ODD : RED_EVEN;
    rop2 = (x & 1) ? RED_EVEN : RED_ODD;
  }

  y += 2;
  hires_Mask(x, y, 1, 1, ROP_CONST(0b01111100));
  hires_Mask(x + 1, y, 1, 1, ROP_CONST(0b00111111));
  ++y;
  if (back) {
    hires_putcc(x, y, ROP_CPY, CH_BACK << 8 | (CH_BACK + 1));
    hires_putcc(x, y += 8, ROP_CPY, CH_BACK << 8 | (CH_BACK + 1));
    hires_putcc(x, y += 8, ROP_CPY, CH_BACK << 8 | (CH_BACK + 1));
  } else {
    hires_putc(x, y, rop, rank);
    hires_putc(x + 1, y, rop2, rank2);
    hires_putcc(x, y += 8, ROP_CPY, CH_FACE_LEFT << 8 | CH_FACE);
    hires_putc(x, y += 8, rop, suit);
    hires_putc(x + 1, y, rop2, suit + 1);
  }
  y += 8;
  hires_Mask(x, y, 1, 1, ROP_CONST(0b01111100));
  hires_Mask(x + 1, y, 1, 1, ROP_CONST(0b00111111));
}

void drawCard(unsigned char x, unsigned char y, unsigned char style, const char *s) {
  (void)style;
  drawCardAt(x, ROW(y), s);
}

void drawLine(unsigned char x, unsigned char y, unsigned char w) {
  hires_Mask(x, y < HEIGHT - 1 ? ROW(y) + 4 : CURSOR_Y, w, 2, WHITE);
}

void hideLine(unsigned char x, unsigned char y, unsigned char w) {
  hires_Mask(x, y < HEIGHT - 1 ? ROW(y) + 4 : CURSOR_Y, w, 2, BLACK);
}

void drawUnderline(unsigned char x, unsigned char y, unsigned char w) {
  hires_Mask(x, ROW(y + 1), w, 1, WHITE);
}

void clearArea(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  hires_Mask(x, ROW(y), w, ROW(h), BLACK);
}

void drawBox(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  static unsigned char i;

  y = ROW(y);
  hires_putc(x, y, ROP_CPY, CH_BOX_TL);
  hires_putc(x + w + 1, y, ROP_CPY, CH_BOX_TR);
  for (i = 1; i <= w; i++) {
    hires_putc(x + i, y, ROP_CPY, CH_LINE);
    hires_putc(x + i, y + ROW(h + 1), ROP_CPY, CH_LINE);
  }
  for (i = 0; i < h; i++) {
    y += 8;
    hires_putc(x, y, ROP_CPY, CH_BOX_SIDE);
    hires_putc(x + w + 1, y, ROP_CPY, CH_BOX_SIDE);
  }
  y += 8;
  hires_putc(x, y, ROP_CPY, CH_BOX_BL);
  hires_putc(x + w + 1, y, ROP_CPY, CH_BOX_BR);
}

void drawBorder() {
  drawCardAt(1, 0, "as");
  drawCardAt(WIDTH - 3, 0, "ah");
  drawCardAt(1, TABLE_BOTTOM - 31, "ad");
  drawCardAt(WIDTH - 3, TABLE_BOTTOM - 31, "ac");
}

void drawLogo() {
  drawText(WIDTH/2 - 4, 7, "fuji net");
  drawText(WIDTH/2 - 4, 9, "blackjack");
}

void initGraphics() {
  hires_Init();
  POKE(0xC054, 0);
  resetScreen();
  clearStatusBar();
}

void resetGraphics() {
  hires_Done();
}

uint16_t jiffies;

// No vblank on the II+, so this is a timed loop
void waitvsync() {
  static uint16_t i;

  for (i = 0; i < 760; i++)
    ;
  ++jiffies;
}

#endif /* __APPLE2__ */
