#ifdef _CMOC_VERSION_
#include <cmoc.h>
#include <coco.h>
unsigned char kbhit(void);
char cgetc(void);
#else
#include <string.h>
#include <conio.h>
#endif /* _CMOC_VERSION_ */

#include "misc.h"
#include "screens.h"
#include "stateclient.h"
#include "gamelogic.h"
#include "platform-specific/appkey.h"
#include "platform-specific/sound.h"
#include "platform-specific/util.h"

#define PLAYER_NAME_MAX 8
#define TABLE_LEFT (WIDTH/2 - 13)
#define TABLE_TOP 5

static void centerText(unsigned char y, const char *text) {
  drawText((unsigned char)(WIDTH/2 - strlen(text)/2), y, text);
}

void centerStatusText(const char *text) {
  clearStatusBar();
  drawStatusTextAt((unsigned char)((WIDTH - strlen(text)) >> 1), text);
}

static void resetScreenWithBorder() {
  resetScreen();
  drawBorder();
}

static void progressAnim(unsigned char y) {
  for (i = 0; i < 3; ++i) {
    pause(10);
    drawChip(WIDTH/2 - 2 + i*2, y);
  }
}

#if WIDTH >= 40
#define HELP_X (WIDTH/2 - 15)
#define HELP_COL 10
#else
#define HELP_X 1
#define HELP_COL 7
#endif

static void helpMove(unsigned char y, const char *move, const char *text) {
  drawText(HELP_X, y, move);
  drawText(HELP_X + HELP_COL, y, text);
}

static void showHelpScreen() {
  resetScreenWithBorder();

  centerText(3, "how to play blackjack");
  centerText(5, "get closer to 21 than the");
  centerText(6, "dealer without going over.");
  centerText(7, "faces count 10, aces 1 or 11.");
  centerText(8, "blackjack pays 3 to 2.");
  centerText(9, "dealer stands on all 17s.");

  helpMove(11, "hit", "take a card");
#if WIDTH >= 40
  helpMove(12, "stand", "keep the hand");
#else
  helpMove(12, "stay", "keep the hand");
#endif
  helpMove(13, "double", "double bet, one card");
  helpMove(14, "split", "play a pair as 2 hands");
#if WIDTH >= 40
  helpMove(15, "surrender", "give up half your bet");
#else
  helpMove(15, "fold", "give up half your bet");
#endif

  centerText(17, "left/right picks a move,");
  centerText(18, "return plays it.");

  centerStatusText("press a key to continue");

  clearCommonInput();
  cgetc();
  resetScreen();
  redrawGameScreen = 1;
}

static void welcomeActionVerifyServerDetails() {
  // The lobby stores "server?table=..."; keep only the server
  read_appkey(AK_LOBBY_CREATOR_ID, AK_LOBBY_APP_ID, AK_LOBBY_KEY_SERVER, tempBuffer);
  i = (unsigned char)strlen(tempBuffer);
  if (i) {
    while (--i) {
      if (tempBuffer[i] == '?') {
        tempBuffer[i] = 0;
        break;
      }
    }
    strcpy(serverEndpoint, tempBuffer);
  }

  // Always pick a table at launch
  query[0] = 0;
}

static bool inputFieldCycle(uint8_t x, uint8_t y, uint8_t max, char *buffer) {
  static uint8_t done, curx, lastY;

  if (done == 1 || lastY != y) {
    done = 0;
    lastY = y;
    curx = (unsigned char)strlen(buffer);
    drawText(x, y, buffer);
    drawChip(x + curx, y);
  }

  if (!kbhit())
    return false;

  inputKey = cgetc();
  if (inputKey == KEY_RETURN && curx > 1) {
    done = 1;
    drawText(x + curx, y, " ");
  } else if (inputKey == KEY_BACKSPACE && curx > 0) {
    buffer[--curx] = 0;
    drawText(x + 1 + curx, y, " ");
  } else if (curx < max && ((curx > 0 && inputKey == KEY_SPACE) ||
             (inputKey >= 'a' && inputKey <= 'z') ||
             (inputKey >= '0' && inputKey <= '9') ||
             (inputKey >= 'A' && inputKey <= 'Z'))) {
    buffer[curx] = inputKey;
    buffer[++curx] = 0;
  }

  drawText(x, y, buffer);
  drawChip(x + curx, y);
  return done == 1;
}

static void showPlayerNameScreen() {
  resetScreenWithBorder();
  clearStatusBar();
  drawLogo();

  centerText(HEIGHT/2, "enter your name");
  drawBox(WIDTH/2 - PLAYER_NAME_MAX/2 - 1, HEIGHT/2 + 2, PLAYER_NAME_MAX + 1, 1);
  drawText(WIDTH/2 - PLAYER_NAME_MAX/2, HEIGHT/2 + 3, playerName);

  clearCommonInput();
  while (!inputFieldCycle(WIDTH/2 - PLAYER_NAME_MAX/2, HEIGHT/2 + 3, PLAYER_NAME_MAX, playerName))
    ;

  for (y = HEIGHT/2; y < HEIGHT/2 + 5; ++y)
    centerText(y, "                 ");

  write_appkey(AK_LOBBY_CREATOR_ID, AK_LOBBY_APP_ID, AK_LOBBY_KEY_USERNAME, playerName);
}

static void welcomeActionVerifyPlayerName() {
  read_appkey(AK_LOBBY_CREATOR_ID, AK_LOBBY_APP_ID, AK_LOBBY_KEY_USERNAME, tempBuffer);
  tempBuffer[PLAYER_NAME_MAX] = 0;
  if (tempBuffer[0])
    strcpy(playerName, tempBuffer);

  if (!playerName[0])
    showPlayerNameScreen();
}

void showWelcomeScreen() {
  resetScreenWithBorder();
  drawLogo();

  welcomeActionVerifyPlayerName();
  welcomeActionVerifyServerDetails();

  strcpy(tempBuffer, "welcome ");
  strcat(tempBuffer, playerName);
  centerText(HEIGHT/2, tempBuffer);
  pause(45);

  if (prefs[PREF_HELP] != 2) {
    prefs[PREF_HELP] = 2;
    savePrefs();
    showHelpScreen();
  }
}

static void tableActionJoinServer() {
  clearGameState();

  strcat(query, "&player=");
  strcat(query, playerName);

  i = (unsigned char)strlen(query);
  while (--i)
    if (query[i] == ' ')
      query[i] = '+';
}

void showTableSelectionScreen() {
  static uint8_t shownChip, skipApiCall;
  static unsigned char tableIndex = 0;
  static Table *table;

  skipApiCall = 0;
  while (!query[0]) {
    if (!skipApiCall)
      centerStatusText("refreshing table list..");

    resetScreenWithBorder();
    centerText(3, "choose a table to join");
    drawText(TABLE_LEFT, TABLE_TOP, "table");
    drawText(TABLE_LEFT + 19, TABLE_TOP, "players");
    drawLine(TABLE_LEFT, TABLE_TOP + 1, 26);

    if (!skipApiCall)
      apiCall("tables");
    skipApiCall = 0;

    if (clientState.tables.count) {
      for (i = 0; i < clientState.tables.count; ++i) {
        table = &clientState.tables.table[i];
        drawText(TABLE_LEFT, TABLE_TOP + 2 + i*2, table->name);
        drawText((unsigned char)(TABLE_LEFT + 26 - strlen(table->players)), TABLE_TOP + 2 + i*2, table->players);
        if (table->players[0] > '0')
          drawText((unsigned char)(TABLE_LEFT + 24 - strlen(table->players)), TABLE_TOP + 2 + i*2, "*");
      }
    } else {
      centerText(12, "no tables are available");
    }

#if WIDTH >= 40
    drawStatusText("r-efresh   h-elp  c-olor   n-ame   q-uit");
#else
    drawStatusText("r-efresh   h-elp   n-ame   q-uit");
#endif
    shownChip = 0;

    clearCommonInput();
    while (!inputTrigger || !clientState.tables.count) {
      readCommonInput();
      if (inputKey == 'h' || inputKey == 'H') {
        showHelpScreen();
        skipApiCall = 1;
        break;
      } else if (inputKey == 'r' || inputKey == 'R') {
        break;
      } else if (inputKey == 'c' || inputKey == 'C') {
        prefs[PREF_COLOR] = cycleNextColor() + 1;
        savePrefs();
        skipApiCall = 1;
        break;
      } else if (inputKey == 'n' || inputKey == 'N') {
        showPlayerNameScreen();
        skipApiCall = 1;
        break;
      } else if (inputKey == 'q' || inputKey == 'Q') {
        quit();
      }

      if (clientState.tables.count && (!shownChip || inputDirY)) {
        drawText(TABLE_LEFT - 1, TABLE_TOP + 2 + tableIndex*2, " ");
        tableIndex += inputDirY;
        if (tableIndex == 255)
          tableIndex = clientState.tables.count - 1;
        else if (tableIndex >= clientState.tables.count)
          tableIndex = 0;
        drawChip(TABLE_LEFT - 1, TABLE_TOP + 2 + tableIndex*2);
        soundCursor();
        shownChip = 1;
      }
    }

    if (inputTrigger) {
      soundSelectMove();
      resetScreenWithBorder();
      clearStatusBar();
      centerText(HEIGHT/2, clientState.tables.table[tableIndex].name);

      strcpy(query, "?table=");
      strcat(query, clientState.tables.table[tableIndex].table);
    }
  }

  centerText(HEIGHT - 7, "connecting to server");
  progressAnim(HEIGHT - 5);

  tableActionJoinServer();
}

void showInGameMenuScreen() {
  i = 1;
  while (i) {
    clearStatusBar();
    resetScreenWithBorder();

    x = WIDTH/2 - 9;
    y = HEIGHT/2 - 3;

    drawBox(x - 3, y - 2, 22, 9);
    drawText(x, y, "  q: quit table");
    drawText(x, y += 2, "  h: how to play");
#if WIDTH >= 40
    drawText(x, y += 2, "  c: color toggle");
#endif
    drawText(x, y += 2, ESC_KEY_NAME ": keep playing");

    clearCommonInput();
    i = 1;
    while (i == 1) {
      readCommonInput();
      switch (inputKey) {
#if WIDTH >= 40
        case 'c':
        case 'C':
          prefs[PREF_COLOR] = cycleNextColor() + 1;
          savePrefs();
          i = 2;
          break;
#endif
        case 'h':
        case 'H':
          showHelpScreen();
          i = 0;
          break;
        case KEY_ESCAPE:
        case KEY_ESCAPE_ALT:
          i = 0;
          break;
        case 'q':
        case 'Q':
          resetScreenWithBorder();
          centerText(10, "please wait");
          apiCall("leave");
          progressAnim(12);
          clearCommonInput();
          query[0] = 0;
          showTableSelectionScreen();
          return;
      }
    }
  }

  clearGameState();
  clearCommonInput();
}
