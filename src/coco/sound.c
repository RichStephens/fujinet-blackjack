#ifdef _CMOC_VERSION_
/*
  Platform-specific sound functions
*/
#include <coco.h>
#include "../misc.h"
#include "../platform-specific/sound.h"

uint16_t ii;


void tone(uint8_t period, uint8_t dur, uint8_t wait) {
  sound(period, dur);
  
  while (wait--)
    for (ii=0; ii<60; ii++) ;
}

void initSound() {}


void soundJoinGame() {
  tone(40,1,50);
  tone(2,1,50);
  tone(40,1,0);
}

void soundMyTurn() {
  tone(100,1,40);
  tone(100,2,0);
}

void soundGameDone() {
  tone(0,2,20);
  tone(70,3,100);
  tone(90,3,20);
  tone(110,5,20);
}

void soundDealCard() {
  static uint8_t i;
  for(i=0;i<7;i++)
    tone((i*13+8)%100,0,0);
}

void soundTick() {
  tone(0,0,0);
}

void soundPlayerJoin() {
  tone(60,1,10);
  tone(100,1,0);
}

void soundPlayerLeft() {
  tone(100,1,10);
  tone(60,1,0);
}

void soundSelectMove() {
  tone(60,1,0);
  tone(100,1,0);
}

void soundCursor() {
  tone(80,1,0);
}

void soundCursorInvalid() {
  tone(0,1,0);
}

void soundTakeChip(uint16_t counter) {
  tone((uint8_t)counter*3,1,4);
}
#endif
