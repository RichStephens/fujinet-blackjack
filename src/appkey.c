/**
 * @brief   App Key Functions
 * @author  Eric Carr
 * @license gpl v. 3
 */

#ifdef _CMOC_VERSION_
#include <cmoc.h>
#include <coco.h>
#else
#include <stdint.h>
#include <string.h>
#endif /* _CMOC_VERSION_ */

#include <fujinet-fuji.h>
#include "platform-specific/appkey.h"

void read_appkey(unsigned int creator_id, unsigned char app_id, unsigned char key_id, char *destination) {
  uint16_t read = 0;

  fuji_set_appkey_details(creator_id, app_id, DEFAULT);
  if (!fuji_read_appkey(key_id, &read, (uint8_t *)destination))
    read = 0;

  destination[read] = 0;
}

void write_appkey(unsigned int creator_id, unsigned char app_id, unsigned char key_id, const char *inputString) {
  fuji_set_appkey_details(creator_id, app_id, DEFAULT);
  fuji_write_appkey(key_id, strlen(inputString), (uint8_t *)inputString);
}
