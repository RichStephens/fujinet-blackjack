#ifdef __ATARI__

#include <atari.h>
#include <peekpoke.h>
#include <string.h>
#include "../misc.h"

extern unsigned char charset[];
extern void dli(void);
extern unsigned char tableColor;

// All of this sits in BASIC's space, which initGraphics frees
#define PM_BASE ((unsigned char *)0xA000)
#define CHARSET_LOC 0xB000
#define SCREEN_LOC ((unsigned char *)0xB400)
#define DL_LOC ((unsigned char *)0xB810)

#define MISSILES (PM_BASE + 768)
#define PLAYER(n) (PM_BASE + 1024 + (n) * 256)

#define xypos(x, y) (SCREEN_LOC + (x) + (y) * WIDTH)

#define ROW_SCANLINE(y) (24 + (y) * 8)
#define COL_HPOS(x) (48 + (x) * 4)
// Just below the status row, which follows three blank scanlines
#define STATUS_LINE (ROW_SCANLINE(HEIGHT) + 3)

enum {
  CH_WHITE = 0x01,
  CH_TEN_ZERO = 0x0B,
  CH_BOX_TL = 0x1B,
  CH_BOX_TR = 0x1C,
  CH_BOX_BL = 0x1D,
  CH_BOX_BR = 0x1E,
  CH_BOX_SIDE = 0x1F,
  CH_LINE = 0x20,
  CH_PLUS = 0x3B,
  CH_BACK = 0x3C,
  CH_WHITE_EDGE = 0x3E,
  CH_SUIT = 0x40,
  CH_EDGE_LEFT = 0x49,
  CH_EDGE_RIGHT = 0x4A,
  CH_TOP = 0x51,
  CH_BOTTOM = 0x53,
  CH_BACK_TOP = 0x55,
  CH_BACK_BOTTOM = 0x57,
  CH_CORNER = 0x59,
  CH_CLOCK = 0x7A,
  CH_CHIP = 0xEF,
  CH_RED = 0x80
};

static const unsigned char displayList[] = {
  DL_BLK8, DL_DLI(DL_BLK8),
  DL_LMS(DL_CHR40x8x4), 0x00, 0xB4,
  DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4,
  DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4,
  DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4,
  DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4,
  DL_CHR40x8x4, DL_CHR40x8x4, DL_CHR40x8x4,
  DL_DLI(DL_CHR40x8x4),
  DL_BLK3,
  DL_CHR40x8x4,
  DL_JVB, 0x10, 0xB8
};

// Table and card border color per theme: green, blue, brown, gray
static const unsigned char themes[2][8] = {
  { 0xB4, 0x88, 0x84, 0x08, 0x22, 0x28, 0x04, 0x08 }, // NTSC
  { 0xA4, 0x78, 0x74, 0x08, 0x12, 0x18, 0x04, 0x08 }  // PAL
};

unsigned char colorMode = 1;
static unsigned char pal, oldChbas, oldSdmctl;
static void *oldSdlst;

/* Underlines are player/missile bars: P1 for up to 8 columns, M0+M1 for up
   to 4. The status bar cursor is P2, with M2 for columns 9-10. */
typedef struct {
  unsigned char x, y, scanline;
} Underline;
static Underline lines[2];

// cc65 -O miscompiles ~(0xFF >> w) here, shifting by the wrong argument
static const unsigned char leftBits[9] = { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };

static void showTheme() {
  static unsigned char t;

  t = (colorMode - 1) * 2;
  tableColor = themes[pal][t];
  OS.color0 = themes[pal][t + 1];
}

unsigned char cycleNextColor() {
  if (++colorMode > 4)
    colorMode = 1;
  showTheme();
  return colorMode;
}

void setColorMode(unsigned char mode) {
  colorMode = mode ? mode : 1;
  showTheme();
}

static unsigned char screenCode(unsigned char c) {
  if (c >= 'a' && c <= 'z')
    return c - 64;
  if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
    return c - 32;
  switch (c) {
    case '-': return 0x7D;
    case '/': return 0x7C;
    case ':': return 0x1A;
    case '.': return 0x73;
    case ',': return 0x74;
    case '+': return CH_PLUS;
    case '*': return CH_CHIP;
  }
  return 0;
}

void drawText(unsigned char x, unsigned char y, const char *s) {
  static unsigned char *pos;

  pos = xypos(x, y);
  while (*s)
    *pos++ = screenCode(*s++);
}

void drawTextAt(unsigned char x, unsigned char y, const char *s) {
  drawText(x, y, s);
}

void drawBuffer() {
  waitvsync();
}

static void hideUnderline(unsigned char i) {
  if (!lines[i].scanline)
    return;
  if (i)
    MISSILES[lines[i].scanline] = 0;
  else
    PLAYER(1)[lines[i].scanline] = 0;
  lines[i].scanline = 0;
}

static void hideStatusCursor() {
  PLAYER(2)[STATUS_LINE] = PLAYER(2)[STATUS_LINE + 1] = 0;
  MISSILES[STATUS_LINE] = MISSILES[STATUS_LINE + 1] = 0;
}

void clearStatusBar() {
  memset(xypos(0, HEIGHT - 1), 0, WIDTH);
  hideStatusCursor();
}

void drawStatusTextAt(unsigned char x, const char *s) {
  drawText(x, HEIGHT - 1, s);
}

void drawStatusText(const char *s) {
  static unsigned char len;

  len = (unsigned char)strlen(s);
  if (len > WIDTH)
    len = WIDTH;
  memcpy(tempBuffer, s, len);
  tempBuffer[len] = 0;
  drawStatusTextAt(0, tempBuffer);
  memset(xypos(len, HEIGHT - 1), 0, WIDTH - len);
}

void drawStatusTimer() {
  POKE(xypos(WIDTH - 1, HEIGHT - 1), CH_CLOCK);
}

void drawChip(unsigned char x, unsigned char y) {
  POKE(xypos(x, y), CH_CHIP);
}

void resetScreen() {
  memset(SCREEN_LOC, 0, WIDTH * HEIGHT);

  // Round off the table's corners
  POKE(xypos(0, 0), CH_CORNER);
  POKE(xypos(WIDTH - 1, 0), CH_CORNER + 1);
  POKE(xypos(0, HEIGHT - 2), CH_CORNER + 2);
  POKE(xypos(WIDTH - 1, HEIGHT - 2), CH_CORNER + 3);

  hideUnderline(0);
  hideUnderline(1);
  hideStatusCursor();
}

/* x is the left edge column; the card face is the two columns after it. A
   full card's left edge merges into the card it overlaps. A fanned card has
   no left edge, leaving the rank column of the card beneath showing. */
void drawCard(unsigned char x, unsigned char y, unsigned char style, const char *s) {
  static unsigned char *pos, *side;
  static unsigned char rank, rank2, suit, red, i, c;

  pos = xypos(x + 1, y);

  if (s[0] == '?') {
    pos[0] = CH_BACK_TOP;
    pos[1] = CH_BACK_TOP + 1;
    pos[40] = pos[81] = pos[120] = CH_BACK;
    pos[41] = pos[80] = pos[121] = CH_BACK | CH_RED;
    pos[160] = CH_BACK_BOTTOM;
    pos[161] = CH_BACK_BOTTOM + 1;
  } else {
    red = (s[1] == 'h' || s[1] == 'd') ? CH_RED : 0;
    rank2 = CH_WHITE;
    switch (s[0]) {
      case 't': rank = 0x0A; rank2 = CH_TEN_ZERO | red; break;
      case 'j': rank = 0x0C; break;
      case 'q': rank = 0x0D; break;
      case 'k': rank = 0x0E; break;
      case 'a': rank = 0x0F; break;
      default: rank = s[0] - '0';
    }
    switch (s[1]) {
      case 'h': suit = CH_SUIT | CH_RED; break;
      case 'd': suit = (CH_SUIT + 2) | CH_RED; break;
      case 'c': suit = CH_SUIT + 4; break;
      default: suit = CH_SUIT + 6;
    }

    pos[0] = CH_TOP;
    pos[1] = CH_TOP + 1;
    pos[40] = rank | red;
    pos[41] = rank2;
    pos[80] = pos[81] = CH_WHITE;
    pos[120] = suit;
    pos[121] = suit + 1;
    pos[160] = CH_BOTTOM;
    pos[161] = CH_BOTTOM + 1;
  }

  side = pos + 39;
  for (i = 0; i < 3; i++, side += 40) {
    if (style != FANNED_CARD) {
      c = *side;
      if ((c & 0x7F) == CH_BACK)
        ++c;
      else if (c == CH_WHITE)
        c = CH_WHITE_EDGE;
      else if ((c > CH_SUIT && c < CH_SUIT + 8) || (c > (CH_SUIT | CH_RED) && c < (CH_SUIT | CH_RED) + 8)) {
        c += 32;
        side[-1] += 32;
      } else if ((c & 0x7F) == CH_TEN_ZERO)
        c += 52;
      else
        c = CH_EDGE_LEFT;
      *side = c;
    }
    side[3] = CH_EDGE_RIGHT;
  }
}

void drawLine(unsigned char x, unsigned char y, unsigned char w) {
  static unsigned char m;

  if (y < HEIGHT - 1) {
    memset(xypos(x, y), CH_LINE, w);
    return;
  }

  GTIA_WRITE.hposp2 = COL_HPOS(x);
  GTIA_WRITE.hposm2 = COL_HPOS(x + 8);
  PLAYER(2)[STATUS_LINE] = PLAYER(2)[STATUS_LINE + 1] = leftBits[w > 8 ? 8 : w];
  m = w > 8 ? (w > 9 ? 0x30 : 0x20) : 0;
  MISSILES[STATUS_LINE] = MISSILES[STATUS_LINE + 1] = m;
}

void hideLine(unsigned char x, unsigned char y, unsigned char w) {
  if (y < HEIGHT - 1)
    memset(xypos(x, y), 0, w);
  else
    hideStatusCursor();
}

// A line along the blank top scanline of the row beneath
void drawUnderline(unsigned char x, unsigned char y, unsigned char w) {
  static unsigned char i, sl;

  i = lines[0].scanline ? 1 : 0;
  hideUnderline(i);
  sl = ROW_SCANLINE(y + 1);
  lines[i].x = x;
  lines[i].y = y;
  lines[i].scanline = sl;

  if (i) {
    GTIA_WRITE.hposm0 = COL_HPOS(x);
    GTIA_WRITE.hposm1 = COL_HPOS(x + 2);
    MISSILES[sl] = w >= 4 ? 0x0F : w == 3 ? 0x0B : w == 2 ? 0x03 : 0x02;
  } else {
    GTIA_WRITE.hposp1 = COL_HPOS(x);
    PLAYER(1)[sl] = leftBits[w > 8 ? 8 : w];
  }
}

void clearArea(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  static unsigned char i;
  static unsigned char *pos;

  pos = xypos(x, y);
  for (i = 0; i < h; i++, pos += WIDTH)
    memset(pos, 0, w);

  for (i = 0; i < 2; i++)
    if (lines[i].scanline && lines[i].x >= x && lines[i].x < x + w && lines[i].y >= y && lines[i].y < y + h)
      hideUnderline(i);
}

void drawBox(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  static unsigned char i;
  static unsigned char *pos;

  pos = xypos(x, y);
  pos[0] = CH_BOX_TL;
  memset(pos + 1, CH_LINE, w);
  pos[w + 1] = CH_BOX_TR;
  for (i = 0; i < h; i++) {
    pos += WIDTH;
    pos[0] = pos[w + 1] = CH_BOX_SIDE;
  }
  pos += WIDTH;
  pos[0] = CH_BOX_BL;
  memset(pos + 1, CH_LINE, w);
  pos[w + 1] = CH_BOX_BR;
}

void drawBorder() {
  static unsigned char i;
  static unsigned char *pos;

  drawCard(0, 0, FULL_CARD, "as");
  drawCard(WIDTH - 4, 0, FULL_CARD, "ah");
  drawCard(0, HEIGHT - 6, FULL_CARD, "ad");
  drawCard(WIDTH - 4, HEIGHT - 6, FULL_CARD, "ac");

  for (i = 4; i < WIDTH - 4; i += 4) {
    pos = xypos(i, 1);
    pos[0] = 0xDC; pos[1] = 0xD9; pos[2] = 0x5A; pos[3] = 0x5B;
    pos = xypos(i, HEIGHT - 4);
    pos[0] = 0xDC; pos[1] = 0xD9; pos[2] = 0x5A; pos[3] = 0x5B;
  }
}

void drawLogo() {
  drawText(WIDTH/2 - 4, 7, "fuji net");
  drawText(WIDTH/2 - 4, 9, "blackjack");
}

void initGraphics() {
  // Free BASIC's space when it is switched in
  if (PEEKW(0x2E5) < 0xBC1F && !(PEEK(0xD301) & 2)) {
    waitvsync();
    POKE(0xD301, PEEK(0xD301) | 2);
    POKE(0x3F8, 1);
  }

  pal = PEEK(0xD014) == 1;

  memset(PM_BASE, 0, 0x800);
  memset(PLAYER(0), 0xFF, 256);
  memset(PLAYER(3), 0xFF, 256);
  memcpy((void *)CHARSET_LOC, charset, 1024);
  memcpy(DL_LOC, displayList, sizeof(displayList));
  resetScreen();

  OS.color1 = 0x0E;
  OS.color2 = 0x00;
  OS.color3 = pal ? 0x24 : 0x34;
  OS.color4 = 0x00;
  OS.pcolr0 = OS.pcolr3 = 0x00;
  OS.pcolr1 = OS.pcolr2 = OS.color3;
  showTheme();

  // Players 0 and 3 cover the border either side of the table
  GTIA_WRITE.hposp0 = 16;
  GTIA_WRITE.hposp3 = 208;
  GTIA_WRITE.sizep0 = GTIA_WRITE.sizep1 = GTIA_WRITE.sizep2 = GTIA_WRITE.sizep3 = 3;
  GTIA_WRITE.sizem = 0xFF;
  ANTIC.pmbase = (unsigned)PM_BASE >> 8;
  // Playfield over players, missiles in the red of COLOR3
  OS.gprior = 0x14;

  oldChbas = OS.chbas;
  oldSdmctl = OS.sdmctl;
  oldSdlst = OS.sdlst;

  waitvsync();
  OS.sdmctl = 0;
  OS.sdlst = DL_LOC;
  OS.chbas = CHARSET_LOC >> 8;
  OS.vdslst = (void *)dli;
  ANTIC.nmien = NMIEN_DLI | NMIEN_VBI;
  waitvsync();
  OS.sdmctl = 0x3E;
  GTIA_WRITE.gractl = 3;
}

void resetGraphics() {
  waitvsync();
  ANTIC.nmien = NMIEN_VBI;
  GTIA_WRITE.gractl = 0;
  OS.sdmctl = oldSdmctl;
  OS.sdlst = oldSdlst;
  OS.chbas = oldChbas;
  OS.gprior = 0;
  OS.color4 = 0;
  waitvsync();
}

#endif /* __ATARI__ */
