#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <string.h>
#endif /* _CMOC_VERSION_ */

#include "stateclient.h"
#include "misc.h"
#include "platform-specific/network.h"

static char urlBuffer[128];

unsigned char apiCall(const char *path) {
  strcpy(urlBuffer, serverEndpoint[0] != 'N' ? "N:" : "");
  strcat(urlBuffer, serverEndpoint);
  strcat(urlBuffer, path);
  strcat(urlBuffer, query);
  strcat(urlBuffer, query[0] ? "&bin=1" QUERY_SUFFIX : "?bin=1" QUERY_SUFFIX);
  return getResponse(urlBuffer, &clientState.firstByte, sizeof(clientState.game));
}

unsigned char getStateFromServer() {
  if (requestedMove) {
    strcpy(tempBuffer, "move/");
    strcat(tempBuffer, requestedMove);
    requestedMove = NULL;
  } else {
    strcpy(tempBuffer, "state");
  }
  return apiCall(tempBuffer);
}
