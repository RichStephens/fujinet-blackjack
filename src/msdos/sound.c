#ifdef __WATCOMC__
/*
  The tones from Texas Hold'em
*/

#include <conio.h>
#include <stdint.h>
#include "../platform-specific/sound.h"

#define PIT_CONTROL_PORT 0x43
#define PIT_CHANNEL2_PORT 0x42
#define SPEAKER_CONTROL_PORT 0x61
#define PIT_FREQUENCY 1193180UL

extern unsigned int pitTicksPerMs;
unsigned int pitRead(void);
void calibratePit(void);

// One frame per pass, so counter 0 cannot wrap mid-wait
static void waitFrames(unsigned int frames) {
  unsigned int start, want;

  if (!pitTicksPerMs)
    calibratePit();

  want = (unsigned int)(((unsigned long)pitTicksPerMs * 50UL) / 3UL);
  while (frames--) {
    start = pitRead();
    while ((unsigned int)(start - pitRead()) < want)
      ;
  }
}

static void beep(unsigned int frequency, unsigned int frames, unsigned int wait) {
  unsigned int divisor;

  divisor = (unsigned int)(PIT_FREQUENCY / frequency);
  outp(PIT_CONTROL_PORT, 0xB6);
  outp(PIT_CHANNEL2_PORT, divisor & 0xFF);
  outp(PIT_CHANNEL2_PORT, divisor >> 8);

  outp(SPEAKER_CONTROL_PORT, inp(SPEAKER_CONTROL_PORT) | 0x03);
  waitFrames(frames);
  outp(SPEAKER_CONTROL_PORT, inp(SPEAKER_CONTROL_PORT) & ~0x03);
  waitFrames(wait);
}

void initSound() {
}

void soundJoinGame() {
  beep(430, 5, 8);
  beep(340, 5, 0);
  beep(500, 5, 0);
}

void soundMyTurn() {
  beep(430, 4, 2);
  beep(430, 4, 2);
}

void soundGameDone() {
  beep(311, 10, 0);
  beep(330, 20, 0);
  beep(392, 10, 0);
  beep(415, 20, 0);
}

void soundDealCard() {
  beep(150, 1, 5);
}

void soundTick() {
  beep(50, 2, 0);
}

void soundPlayerJoin() {
  static uint8_t i;

  for (i = 50; i <= 80; i += 10)
    beep(i, 2, 15);
}

void soundPlayerLeft() {
  static uint8_t i;

  for (i = 80; i >= 50; i -= 10)
    beep(i, 2, 15);
}

void soundSelectMove() {
  beep(300, 3, 1);
  beep(350, 3, 0);
}

void soundCursor() {
  beep(300, 2, 0);
}

void soundCursorInvalid() {
  beep(100, 2, 0);
}

void soundTakeChip(uint16_t counter) {
  beep(50 + counter * 20, 2, 2);
}

#endif /* __WATCOMC__ */
