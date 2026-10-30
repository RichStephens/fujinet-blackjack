#ifdef _CMOC_VERSION_
#include <cmoc.h>
#include <coco.h>
char cgetc(void);
unsigned char kbhit(void);
#else
#include <string.h>
#include <conio.h>
#endif /* _CMOC_VERSION_ */

#include "misc.h"
#include "platform-specific/appkey.h"

static unsigned char lastJoy, joy;

void pause(unsigned char frames) {
  while (frames--)
    waitvsync();
}

void clearCommonInput() {
  inputTrigger = false;
  inputKey = 0;
  inputDirX = 0;
  inputDirY = 0;
  lastJoy = 0;
  joy = 0;

  while (kbhit())
    cgetc();
}

void readCommonInput() {
  inputTrigger = false;
  inputKey = 0;
  inputDirX = 0;
  inputDirY = 0;

  joy = readJoystick();
  if (joy != lastJoy) {
    lastJoy = joy;
    if (JOY_LEFT(joy))
      inputDirX = -1;
    else if (JOY_RIGHT(joy))
      inputDirX = 1;
    if (JOY_UP(joy))
      inputDirY = -1;
    else if (JOY_DOWN(joy))
      inputDirY = 1;
    if (JOY_BTN_1(joy))
      inputTrigger = true;
    return;
  }

  if (!kbhit())
    return;

  inputKey = cgetc();
  switch (inputKey) {
    case KEY_LEFT_ARROW:
    case KEY_LEFT_ARROW_2:
    case KEY_LEFT_ARROW_3:
      inputDirX = -1;
      break;
    case KEY_RIGHT_ARROW:
    case KEY_RIGHT_ARROW_2:
    case KEY_RIGHT_ARROW_3:
      inputDirX = 1;
      break;
    case KEY_UP_ARROW:
    case KEY_UP_ARROW_2:
    case KEY_UP_ARROW_3:
      inputDirY = -1;
      break;
    case KEY_DOWN_ARROW:
    case KEY_DOWN_ARROW_2:
    case KEY_DOWN_ARROW_3:
      inputDirY = 1;
      break;
    case KEY_RETURN:
    case KEY_SPACE:
      inputTrigger = true;
      break;
  }
}

void loadPrefs() {
  read_appkey(AK_CREATOR_ID, AK_APP_ID, AK_KEY_PREFS, tempBuffer);

  if (!tempBuffer[0])
    memset(prefs, 1, 3);
  else
    strncpy(prefs, tempBuffer, 3);

  setColorMode(prefs[PREF_COLOR] - 1);
}

void savePrefs() {
  prefs[3] = 0;
  write_appkey(AK_CREATOR_ID, AK_APP_ID, AK_KEY_PREFS, prefs);
}
