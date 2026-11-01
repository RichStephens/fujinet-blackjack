#ifdef _CMOC_VERSION_
#include <cmoc.h>
#include <coco.h>
#else
#include <string.h>
#endif /* _CMOC_VERSION_ */

#include "misc.h"
#include "gamelogic.h"
#include "table.h"
#include "screens.h"
#include "platform-specific/sound.h"
#include "platform-specific/util.h"

// The last state drawn, so only what changed is redrawn
static Game prev;
static unsigned char prevCards, wasViewing, justJoined;

static unsigned char cardCount() {
  static unsigned char p, c, n;

  n = (unsigned char)strlen(state.dealer);
  for (p = 0; p < state.playerCount; p++)
    for (c = 0; c < state.players[p].handCount; c++)
      n += (unsigned char)strlen(state.players[p].hands[c].cards);
  return n / 2;
}

void clearGameState(unsigned char joining) {
  memset(&prev, 0, sizeof(prev));
  justJoined = joining;
  prevCards = 255;
  wasViewing = 0;
  redrawGameScreen = 1;
}

void showGameScreen() {
  static unsigned char p, n;

  if (state.viewing != wasViewing) {
    wasViewing = state.viewing;
    if (state.viewing) {
      centerStatusText("table full - watching");
      pause(80);
    }
  }

  if (redrawGameScreen || state.playerCount != prev.playerCount ||
      (state.round != prev.round && state.round <= ROUND_BETTING)) {
    drawTable();
    redrawGameScreen = 0;
  } else {
    if (strcmp(state.dealer, prev.dealer) || state.dealerFlags != prev.dealerFlags)
      redrawDealer();

    for (p = 0; p < state.playerCount; p++)
      if (state.round != prev.round ||
          memcmp(&state.players[p], &prev.players[p], sizeof(Player)) ||
          (state.activePlayer == (int8_t)p) != (prev.activePlayer == (int8_t)p) ||
          (state.activePlayer == (int8_t)p && state.activeHand != prev.activeHand))
        redrawSeat(p);
  }

  n = cardCount();
  if (n > prevCards)
    soundDealCard();
  prevCards = n;

  if (state.activePlayer != 0 || state.viewing)
    drawGameStatus();

  if (justJoined) {
    justJoined = 0;
    if (!state.viewing)
      soundJoinGame();
  } else if (prev.playerCount) {
    if (state.playerCount > prev.playerCount)
      soundPlayerJoin();
    else if (state.playerCount < prev.playerCount)
      soundPlayerLeft();

    for (p = 0; p < state.playerCount; p++)
      if (state.players[p].purse < prev.players[p].purse &&
          !strcmp(state.players[p].name, prev.players[p].name))
        soundTakeChip(p);

    if (state.round == ROUND_OVER && prev.round != ROUND_OVER)
      soundGameDone();
  }

  memcpy(&prev, &state, sizeof(Game));
}

void requestPlayerMove() {
  static unsigned char sel, count, secs;
  static uint16_t maxJifs, t;

  requestedMove = NULL;
  if (state.viewing || state.activePlayer != 0 || !state.validMoveCount)
    return;

  count = layoutMoveMenu();
  sel = defaultStop();
  drawMoveCursor(sel, true);
  soundMyTurn();

  maxJifs = 60 * (uint16_t)state.moveTime;
  clearCommonInput();
  resetTimer();

  while (state.moveTime) {
    waitvsync();

    t = (uint16_t)getTime();
    secs = t >= maxJifs ? 0 : (unsigned char)((maxJifs - t) / 60);
    if (secs != state.moveTime) {
      state.moveTime = secs;
      drawStatusTimeLeft();
      soundTick();
    }

    readCommonInput();

    if (inputTrigger) {
      requestedMove = stopMove(sel);
      showChosenMove(sel);
      soundSelectMove();
      return;
    }

    if (inputDirX) {
      if ((unsigned char)(sel + inputDirX) < count) {
        drawMoveCursor(sel, false);
        sel += inputDirX;
        drawMoveCursor(sel, true);
        soundCursor();
      } else {
        soundCursorInvalid();
      }
    }

    if (inputKey == KEY_ESCAPE || inputKey == KEY_ESCAPE_ALT) {
      showInGameMenuScreen();
      return;
    }
  }

  // Out of time: the server plays the default
  clearStatusBar();
}
