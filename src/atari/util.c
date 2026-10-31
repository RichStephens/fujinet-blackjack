#ifdef __ATARI__

#include <atari.h>
#include <peekpoke.h>
#include <fujinet-fuji.h>
#include "../platform-specific/graphics.h"
#include "../platform-specific/util.h"

#define BOOT_MODE_LOBBY 2

void resetTimer() {
  POKEW(0x13, 0);
}

int getTime() {
  return PEEK(0x13) * 256 + PEEK(0x14);
}

void quit() {
  resetGraphics();
  fuji_set_boot_mode(BOOT_MODE_LOBBY);
  asm("jmp $E477"); // COLDSV
}

#endif /* __ATARI__ */
