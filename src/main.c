#ifdef _CMOC_VERSION_
#include <cmoc.h>
#include <coco.h>
#endif /* _CMOC_VERSION_ */

#include "misc.h"
#include "stateclient.h"
#include "gamelogic.h"
#include "screens.h"
#include "platform-specific/sound.h"

// Replaced by the lobby's endpoint when there is one
char serverEndpoint[50] = "https://blackjack.carr-designs.com/";

char query[50] = "";
char playerName[12] = "";

ClientState clientState;

unsigned char inputKey;
signed char inputDirX, inputDirY;
bool inputTrigger;

char tempBuffer[128];
char prefs[4];
char *requestedMove;
unsigned char redrawGameScreen;

unsigned char h, i, j, k, x, y;

int main(void) {
  loadPrefs();
  initGraphics();
  initSound();

  showWelcomeScreen();
  showTableSelectionScreen();

  while (true) {
    if (getStateFromServer()) {
      showGameScreen();
      requestPlayerMove();
    } else {
      // Don't hammer a server that is sending bad responses
      pause(30);
    }

    readCommonInput();
    if (inputKey == KEY_ESCAPE || inputKey == KEY_ESCAPE_ALT)
      showInGameMenuScreen();
  }
  return 0;
}
