#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <string.h>
#endif /* _CMOC_VERSION_ */

#include "stateclient.h"
#include "misc.h"
#include <fujinet-network.h>

static char urlBuffer[128];

unsigned char apiCall(const char *path) {
  static int16_t read;

  strcpy(urlBuffer, serverEndpoint[0] != 'N' ? "N:" : "");
  strcat(urlBuffer, serverEndpoint);
  strcat(urlBuffer, path);
  strcat(urlBuffer, query);
  strcat(urlBuffer, query[0] ? "&bin=1" QUERY_SUFFIX : "?bin=1" QUERY_SUFFIX);

  // Platforms that buffer the screen show it before the call blocks
  drawBuffer();
  if (network_open(urlBuffer, OPEN_MODE_HTTP_GET, OPEN_TRANS_NONE))
    return 0;
  read = network_read(urlBuffer, &clientState.firstByte, sizeof(clientState.game));
  network_close(urlBuffer);
  return read > 0;
}

#ifdef MIXED_CASE
/* The server sends names in lower case: the player's own goes back to how
   it was typed, and bots ("<name> bot") get capitals */
static void fixNames() {
  static unsigned char p, n;
  static char *s;

  for (p = 0; p < state.playerCount && p < 5; p++) {
    s = state.players[p].name;
    for (n = 0; s[n] && (s[n] | 0x20) == (playerName[n] | 0x20); n++)
      ;
    if (!s[n] && !playerName[n]) {
      strcpy(s, playerName);
      continue;
    }
    n = (unsigned char)strlen(s);
    if (n > 4 && !strcmp(s + n - 4, " bot")) {
      if (s[0] >= 'a' && s[0] <= 'z')
        s[0] -= 32;
      s[n - 3] = 'B';
    }
  }
}
#endif

unsigned char getStateFromServer() {
  if (requestedMove) {
    strcpy(tempBuffer, "move/");
    strcat(tempBuffer, requestedMove);
    requestedMove = NULL;
  } else {
    strcpy(tempBuffer, "state");
  }
#ifdef MIXED_CASE
  if (!apiCall(tempBuffer))
    return 0;
  fixNames();
  return 1;
#else
  return apiCall(tempBuffer);
#endif
}
