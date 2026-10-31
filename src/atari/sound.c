#ifdef __ATARI__
/*
  The sounds from Eric Carr's Atari 5 Card Stud
*/

#include <atari.h>
#include "../misc.h"
#include "../platform-specific/sound.h"

static void soundOff() {
  static uint8_t v;
  for (v = 0; v < 4; ++v)
    _sound(v, 0, 0, 0);
}

static void fade(uint8_t freq, uint8_t frames) {
  static int8_t vol;
  for (vol = 8; vol >= 0; --vol) {
    _sound(0, freq, 10, vol);
    pause(frames);
  }
}

static void blip(uint8_t freq, uint8_t frames) {
  _sound(1, freq, 10, 8);
  pause(frames);
}

void initSound() {
  // Silence SIO noise
  OS.soundr = 0;
  OS.noclik = 255;
}

void soundJoinGame() {
  fade(80, 1);
  fade(100, 1);
  fade(65, 1);
  soundOff();
}

void soundMyTurn() {
  static int8_t vol;
  static uint8_t i;

  vol = 8;
  for (i = 0; i < 13; ++i) {
    _sound(0, 80, 10, vol);
    pause(1);
    if (!--vol)
      vol = 8;
  }
  for (; vol >= 0; --vol) {
    _sound(0, 80, 10, vol);
    pause(2);
  }
}

void soundGameDone() {
  blip(200, 2);
  blip(170, 2);
  blip(150, 2);
  blip(140, 3);
  blip(135, 2);
  blip(132, 3);
  soundOff();
}

void soundDealCard() {
  static int8_t vol;
  for (vol = 2; vol >= 0; --vol) {
    _sound(1, 0, 0, vol);
    pause(1);
  }
  soundOff();
}

void soundTick() {
  _sound(0, 200, 8, 3);
  pause(1);
  soundOff();
}

void soundPlayerJoin() {
  static int8_t vol;
  static uint8_t j;
  for (j = 1; j < 31; j += 5) {
    _sound(j & 3, 20, 0, 1 + j / 5);
    pause(2);
    for (vol = j / 5; vol >= 0; --vol) {
      _sound(j & 3, 20, 0, vol);
      pause(1);
    }
    pause(3);
  }
  soundOff();
}

void soundPlayerLeft() {
  static uint8_t j;
  for (j = 8; j > 1; j -= 2) {
    blip(255 - j * j, 2);
    soundOff();
    pause(8);
  }
}

void soundSelectMove() {
  blip(100, 1);
  blip(80, 2);
  blip(70, 2);
  soundOff();
}

void soundCursor() {
  blip(100, 1);
  soundOff();
}

void soundCursorInvalid() {
  blip(255, 2);
  soundOff();
}

void soundTakeChip(uint16_t counter) {
  _sound(1, 250 - (uint8_t)counter * 5, 10, 6);
  pause(1);
  soundOff();
}

#endif /* __ATARI__ */
