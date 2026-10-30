/*
 Platform specific graphics commands
*/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef _CMOC_VERSION_
#include "../coco/coco_bool.h"
#else
#include <stdint.h>
#include <stdbool.h>
#endif /* _CMOC_VERSION_  */

/* A fanned card is drawn one column right of the card it covers, leaving
   only that card's rank showing. */
enum { FULL_CARD = 0, FANNED_CARD = 1 };

void drawBuffer();

// Call to clear the screen to an empty table
void resetScreen();

void clearStatusBar();
void drawStatusTextAt(unsigned char x, const char* s);
void drawStatusText(const char* s);
void drawStatusTimer();

// For optional HI-RES Y enabled platforms
void drawTextAt(unsigned char x, unsigned char y, const char* s);

void drawText(unsigned char x, unsigned char y, const char* s);
void drawCard(unsigned char x, unsigned char y, unsigned char style, const char* s);
void drawChip(unsigned char x, unsigned char y);
void drawLine(unsigned char x, unsigned char y, unsigned char w);
void hideLine(unsigned char x, unsigned char y, unsigned char w);
void drawUnderline(unsigned char x, unsigned char y, unsigned char w);
void clearArea(unsigned char x, unsigned char y, unsigned char w, unsigned char h);
void drawBox(unsigned char x, unsigned char y, unsigned char w, unsigned char h);
void drawBorder();
void drawLogo();

void initGraphics();
void resetGraphics();
void waitvsync();

uint8_t cycleNextColor();
void setColorMode(unsigned char mode);
extern unsigned char colorMode;

#endif /* GRAPHICS_H */
