#ifdef __APPLE2__
/*
  The tones from Texas Hold'em
*/

#include <stdint.h>
#include <stdlib.h>
#include "../platform-specific/sound.h"

#define CLICK __asm__ ("sta $c030")

static uint16_t ii;

static void tone(uint16_t period, uint8_t dur, uint8_t wait) {
  while (dur--) {
    for (ii = 0; ii < period; ii++)
      ;
    CLICK;
  }

  while (wait--)
    for (ii = 0; ii < 40; ii++)
      ;
}

void initSound() {
}

void soundJoinGame() {
  tone(34, 40, 115);
  tone(44, 40, 40);
  tone(28, 40, 0);
}

void soundMyTurn() {
  tone(34, 40, 50);
  tone(34, 40, 0);
}

void soundGameDone() {
  tone(83, 20, 20);
  tone(79, 50, 30);
  tone(65, 20, 20);
  tone(61, 40, 50);
}

void soundDealCard() {
  static uint8_t i, n;

  for (n = 0; n < 7; n++) {
    for (i = rand() >> 4; i > 0; i--)
      ;
    CLICK;
  }
}

void soundTick() {
  tone(80, 2, 0);
}

void soundPlayerJoin() {
  static uint8_t i;

  for (i = 255; i >= 235; i -= 10)
    tone(i, 5, 255);
}

void soundPlayerLeft() {
  static uint8_t i;

  for (i = 215; i < 255; i += 10)
    tone(i, 5, 255);
}

void soundSelectMove() {
  tone(35, 40, 20);
  tone(30, 40, 0);
}

void soundCursor() {
  tone(45, 10, 0);
}

void soundCursorInvalid() {
  tone(118, 5, 0);
}

void soundTakeChip(uint16_t counter) {
  tone(118 - counter * 3, 5, 60);
}

#endif /* __APPLE2__ */
