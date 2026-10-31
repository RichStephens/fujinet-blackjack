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
  if (network_open(urlBuffer, OPEN_MODE_HTTP_GET, OPEN_TRANS_NONE))
    return 0;
  read = network_read(urlBuffer, &clientState.firstByte, sizeof(clientState.game));
  network_close(urlBuffer);
  return read > 0;
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
