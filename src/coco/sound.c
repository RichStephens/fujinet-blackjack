#ifdef _CMOC_VERSION_
/*
  Platform specific sound functions
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

}

void soundMyTurn() {
  tone(100,1,40);
  tone(100,2,0);
}

void soundGameDone() {

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

}

void soundPlayerLeft() {

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
