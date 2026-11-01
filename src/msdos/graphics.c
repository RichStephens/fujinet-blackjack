#ifdef __WATCOMC__

#include <conio.h>
#include <dos.h>
#include <i86.h>
#include <string.h>
#include "../platform-specific/graphics.h"
#include "../misc.h"

/* CGA 320x200x4 is the CoCo 3 font's own 2bpp layout, so glyphs copy
   straight to video memory. The CoCo 3's 192 lines sit 4 lines down. */
#define VIDEO ((unsigned char far *)0xB8000000UL)
#define TOP 4
#define BOTTOM 175
#define CORNER_TOP 0
#define CORNER_BOTTOM 140
#define RED 0x55
#define RED_RIGHT 0x54

enum { PAL_FELT, PAL_WHITE, PAL_BLACK, PAL_RED };

// 200-line modes read palette values as IRGB: green, teal, brown, gray
static const unsigned char felt[4] = { 0x02, 0x03, 0x06, 0x07 };

extern const unsigned char charset[];

unsigned char colorMode;
static unsigned char oldMode;

static unsigned rowOffset(unsigned char y) {
  y += TOP;
  return (y >> 1) * 80 + ((y & 1) ? 0x2000 : 0);
}

static void hires_Draw(unsigned char x, unsigned char y, unsigned char ylen, unsigned char rop, const unsigned char *src) {
  static unsigned char far *pos;

  while (--ylen) {
    pos = VIDEO + rowOffset(y++) + x * 2;
    pos[0] = *src++ | rop;
    pos[1] = *src++ | rop;
  }
}

static void hires_Mask(unsigned char x, unsigned char y, unsigned char xlen, unsigned char ylen, unsigned char c) {
  while (ylen--)
    _fmemset(VIDEO + rowOffset(y++) + x * 2, c, xlen * 2);
}

static void hires_putc(unsigned char x, unsigned char y, unsigned char rop, unsigned char c) {
  hires_Draw(x, y, 9, rop, &charset[(unsigned)c << 4]);
}

static void hires_putcc(unsigned char x, unsigned char y, unsigned char rop, unsigned cc) {
  hires_putc(x, y, rop, (unsigned char)(cc >> 8));
  hires_putc(x + 1, y, rop, (unsigned char)cc);
}

static void setPalette(unsigned char reg, unsigned char color) {
  union REGS r;

  r.w.ax = 0x1000;
  r.h.bl = reg;
  r.h.bh = color;
  int86(0x10, &r, &r);
}

unsigned char cycleNextColor() {
  setColorMode(colorMode + 1);
  return colorMode;
}

void setColorMode(unsigned char mode) {
  colorMode = mode & 3;
  setPalette(PAL_FELT, felt[colorMode]);
}

void drawTextAt(unsigned char x, unsigned char y, const char *s) {
  static unsigned char c;

  if (y < 180)
    y++;
  while ((c = *s++)) {
    if (c >= 'a' && c <= 'z')
      c -= 32;
    hires_putc(x++, y, 0, c);
  }
}

void drawText(unsigned char x, unsigned char y, const char *s) {
  drawTextAt(x, y * 8, s);
}

void drawBuffer() {
}

void clearStatusBar() {
  hires_Mask(0, 179, WIDTH, 13, 0);
}

void drawStatusTextAt(unsigned char x, const char *s) {
  drawTextAt(x, BOTTOM + 6, s);
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
    hires_Mask(len, 179, WIDTH - len, 13, 0);
}

void drawStatusTimer() {
  hires_putc(WIDTH - 1, BOTTOM + 5, 0, 0x28);
}

void drawChip(unsigned char x, unsigned char y) {
  hires_putc(x, y * 8 + 1, 0, 0x22);
}

void resetScreen() {
  hires_Mask(0, 0, WIDTH, HEIGHT * 8, 0);
}

static void drawCardAt(unsigned char x, unsigned char y, unsigned char style, const char *s) {
  static unsigned char val, red, redR, i, suit, rightDigit;
  static unsigned char far *pos;

  switch (s[1]) {
    case 'h': suit = 0x0A; red = RED; redR = RED_RIGHT; break;
    case 'd': suit = 0x0C; red = RED; redR = RED_RIGHT; break;
    case 'c': suit = 0x0E; red = redR = 0; break;
    default: suit = 0x10; red = redR = 0; break;
  }

  if (s[0] == '?') {
    hires_Draw(x, y + 5, 4, 0, &charset[(0x01 << 4) + 10]);
    hires_Draw(x + 1, y + 5, 4, 0, &charset[(0x02 << 4) + 10]);
    hires_putcc(x, y += 8, 0, 0x1E1F);
    hires_putcc(x, y += 8, 0, 0x1E1F);
    hires_Draw(x, y += 8, 7, 0, &charset[0x1E << 4]);
    hires_Draw(x + 1, y, 7, 0, &charset[0x1F << 4]);
    hires_Draw(x, y += 6, 4, 0, &charset[0x03 << 4]);
    hires_Draw(x + 1, y, 4, 0, &charset[0x04 << 4]);
    return;
  }

  rightDigit = 0x13;
  switch (s[0]) {
    case 't': val = 0x1C; rightDigit = 0x1D; break;
    case 'j': val = 0x23; break;
    case 'q': val = 0x24; break;
    case 'k': val = 0x25; break;
    case 'a': val = 0x26; break;
    default: val = 0x14 + (s[0] - '2');
  }

  // Card left edge, in the last pixel of the column to the left
  if (x > 0 && style != FANNED_CARD && !(VIDEO[rowOffset(y + 6) + x * 2 - 1] & 0x03)) {
    for (i = 0; i < 25; i++) {
      pos = VIDEO + rowOffset(y + 6 + i) + x * 2 - 1;
      *pos = (*pos & 0xFC) | 0x02;
    }
  }

  hires_Draw(x, y + 5, 4, 0, &charset[(0x05 << 4) + 10]);
  hires_Draw(x + 1, y + 5, 4, 0, &charset[(0x06 << 4) + 10]);

  hires_putc(x, y += 8, red, val);
  hires_putc(x + 1, y, redR, rightDigit);

  hires_Draw(x, y += 8, 7, 0, &charset[0x12 << 4]);
  hires_Draw(x + 1, y, 7, 0, &charset[0x13 << 4]);

  hires_putc(x, y += 6, 0, suit);
  hires_putc(x + 1, y, 0, suit + 1);

  hires_Draw(x, y += 8, 4, 0, &charset[0x07 << 4]);
  hires_Draw(x + 1, y, 4, 0, &charset[0x08 << 4]);

  // A fanned card's edge goes in its own first pixel, which is blank face in every glyph
  if (style == FANNED_CARD) {
    for (i = 0; i < 25; i++) {
      pos = VIDEO + rowOffset(y - 23 + i) + x * 2;
      *pos = (*pos & 0x3F) | 0x80;
    }
  }
}

void drawCard(unsigned char x, unsigned char y, unsigned char style, const char *s) {
  drawCardAt(x, y * 8 - 1, style, s);
}

void drawLine(unsigned char x, unsigned char y, unsigned char w) {
  y = y * 8 + 1;
  hires_Mask(x, y > 181 ? 189 : y, w, 2, 0xFF);
}

void hideLine(unsigned char x, unsigned char y, unsigned char w) {
  y = y * 8 + 1;
  hires_Mask(x, y > 181 ? 189 : y, w, 2, 0);
}

void drawUnderline(unsigned char x, unsigned char y, unsigned char w) {
  hires_Mask(x, y * 8 + 8, w, 1, 0xFF);
}

void clearArea(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  hires_Mask(x, y * 8, w, h * 8, 0);
}

void drawBox(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  static unsigned char i;

  y = y * 8 - 1;
  hires_putc(x, y, 0, 0x3B);
  hires_putc(x + w + 1, y, 0, 0x3C);
  for (i = x + w; i > x; --i) {
    hires_putc(i, y, 0, 0x40);
    hires_putc(i, y + (h + 1) * 8, 0, 0x40);
  }
  for (i = 0; i < h; ++i) {
    y += 8;
    hires_putc(x, y, 0, 0x3F);
    hires_putc(x + w + 1, y, 0, 0x3F);
  }
  y += 8;
  hires_putc(x, y, 0, 0x3D);
  hires_putc(x + w + 1, y, 0, 0x3E);
}

void drawBorder() {
  static unsigned char i;

  drawCardAt(1, CORNER_TOP, FULL_CARD, "as");
  drawCardAt(WIDTH - 3, CORNER_TOP, FULL_CARD, "ah");
  drawCardAt(1, CORNER_BOTTOM, FULL_CARD, "ad");
  drawCardAt(WIDTH - 3, CORNER_BOTTOM, FULL_CARD, "ac");

  for (i = 4; i < WIDTH - 4; i += 4) {
    hires_putcc(i, CORNER_TOP + 6, 0, 0x7475);
    hires_putcc(i + 2, CORNER_TOP + 6, 0, 0x7677);
    hires_putcc(i, CORNER_BOTTOM + 24, 0, 0x7475);
    hires_putcc(i + 2, CORNER_BOTTOM + 24, 0, 0x7677);
  }
}

void drawLogo() {
  static unsigned char i;

  i = 4;
  drawText(WIDTH/2 - 7, ++i, "              ");
  drawText(WIDTH/2 - 7, ++i, "   FUJI NET   ");
  drawText(WIDTH/2 - 7, ++i, "              ");
  drawText(WIDTH/2 - 7, ++i, "  BLACKJACK   ");
  drawText(WIDTH/2 - 7, ++i, "              ");
}

void initGraphics() {
  union REGS r;

  r.h.ah = 0x0F;
  int86(0x10, &r, &r);
  oldMode = r.h.al;

  r.w.ax = 0x0004;
  int86(0x10, &r, &r);

  setColorMode(colorMode);
  setPalette(PAL_WHITE, 0x17);
  setPalette(PAL_BLACK, 0x00);
  setPalette(PAL_RED, 0x04);
  // Black border
  r.w.ax = 0x1001;
  r.h.bh = 0;
  int86(0x10, &r, &r);

  _fmemset(VIDEO, 0, 0x4000);
}

void resetGraphics() {
  union REGS r;

  r.h.ah = 0x00;
  r.h.al = oldMode;
  int86(0x10, &r, &r);
}

/* Never time off the retrace bit at 0x3DA: QEMU toggles it on every read.
   PIT counter 0 counts down by 1 per clock in mode 2 and 2 in mode 3, so
   it is measured against the 18.2Hz BIOS tick at 0040:006C. */
unsigned int pitTicksPerMs;

unsigned int pitRead(void) {
  unsigned int t;

  outp(0x43, 0x00);
  t = inp(0x40);
  t |= inp(0x40) << 8;
  return t;
}

void calibratePit(void) {
  volatile unsigned long far *biosTick;
  unsigned long units, mark;
  unsigned int prev, now, perMs;

  biosTick = (volatile unsigned long far *)MK_FP(0x40, 0x6C);

  mark = *biosTick;
  while (*biosTick == mark)
    ;

  mark = *biosTick;
  units = 0;
  prev = pitRead();
  while (*biosTick == mark) {
    now = pitRead();
    units += (unsigned int)(prev - now);
    prev = now;
  }

  perMs = (unsigned int)((units * 1000UL) / 54925UL);
  pitTicksPerMs = (perMs >= 900 && perMs <= 3000) ? perMs : 1193;
}

// One frame since the last call, not a frame from now
void waitvsync() {
  static unsigned int last;
  unsigned int now, frame;

  if (!pitTicksPerMs)
    calibratePit();

  frame = (unsigned int)(((unsigned long)pitTicksPerMs * 50UL) / 3UL);
  do
    now = pitRead();
  while ((unsigned int)(last - now) < frame);
  last = now;
}

#endif /* __WATCOMC__ */
