#ifndef MISC_H
#define MISC_H

#ifdef _CMOC_VERSION_
#include "coco/coco_bool.h"
#include "coco/joystick.h"
#include <cmoc.h>
#include <coco.h>
#else
#include <stdbool.h>
#include <stdint.h>
#endif /* _CMOC_VERSION_ */

#include "platform-specific/graphics.h"
#include "platform-specific/input.h"

// FujiNet AppKey settings. These should not be changed
#define AK_LOBBY_CREATOR_ID 1     // FUJINET Lobby
#define AK_LOBBY_APP_ID 1         // Lobby Enabled Game
#define AK_LOBBY_KEY_USERNAME 0   // Lobby Username key
#define AK_LOBBY_KEY_SERVER 0x0C  // Blackjack's Lobby key

#define AK_CREATOR_ID 0x0901      // Rich Stephens' creator id
#define AK_APP_ID 4               // Blackjack
#define AK_KEY_PREFS 0

#define PREF_HELP 0  // 1/2 seen help screen no/yes
#define PREF_COLOR 1 // color mode + 1

#if defined(__CC65__) || defined(_CMOC_VERSION_) || defined(__Z88DK)
#define _Packed
#endif

/* Mirrors the server's bin=1 layout (blackjack/server/util.go) */

typedef _Packed struct {
  char table[9];
  char name[21];
  char players[6];
} Table;

typedef _Packed struct {
  uint8_t count;
  Table table[10];
} Tables;

typedef _Packed struct {
  char move[3];
  char name[10];
} ValidMove;

typedef _Packed struct {
  uint16_t bet;
  uint8_t total;
  uint8_t flags;
  uint8_t result;
  char cards[17];
} Hand;

typedef _Packed struct {
  char name[9];
  uint8_t status;
  uint16_t purse;
  char move[8];
  uint8_t handCount;
  Hand hands[2];
} Player;

typedef _Packed struct {
  char lastResult[41];
  uint8_t round;
  int8_t activePlayer;
  uint8_t activeHand;
  uint8_t moveTime;
  uint8_t viewing;
  char dealer[21];
  uint8_t dealerTotal;
  uint8_t dealerFlags;
  uint8_t validMoveCount;
  ValidMove validMoves[5];
  uint8_t playerCount;
  Player players[5];
} Game;

typedef union {
  uint8_t firstByte;
  Game game;
  Tables tables;
} ClientState;

enum { ROUND_WAITING = 0, ROUND_BETTING = 1, ROUND_PLAYING = 2, ROUND_DEALER = 3, ROUND_OVER = 5 };
enum { STATUS_WAITING = 0, STATUS_PLAYING = 1, STATUS_LEFT = 3 };
enum { FLAG_SOFT = 1, FLAG_BLACKJACK = 2, FLAG_BUST = 4, FLAG_DOUBLED = 8, FLAG_SURRENDERED = 16, FLAG_DONE = 32 };
enum { RESULT_NONE = 0, RESULT_WIN = 1, RESULT_LOSE = 2, RESULT_PUSH = 3, RESULT_BLACKJACK = 4, RESULT_SURRENDER = 5 };

extern ClientState clientState;
#define state clientState.game

extern unsigned char inputKey;
extern signed char inputDirX, inputDirY;
extern bool inputTrigger;

extern char tempBuffer[128];
extern char query[50];
extern char playerName[12];
extern char serverEndpoint[50];
extern char prefs[4];
extern char *requestedMove;
extern unsigned char redrawGameScreen;

// Common local scope temp variables
extern unsigned char h, i, j, k, x, y;

void pause(unsigned char frames);
void clearCommonInput();
void readCommonInput();
void loadPrefs();
void savePrefs();

#endif /* MISC_H */
