#ifdef __APPLE2__

#include <stdint.h>
#include <joystick.h>
#include <fujinet-fuji.h>
#include "../platform-specific/graphics.h"
#include "../platform-specific/util.h"

#define BOOT_MODE_LOBBY 2

extern uint16_t jiffies;

void resetTimer() {
  jiffies = 0;
}

int getTime() {
  return jiffies;
}

// Cold start: ROM in, power-up byte spoiled, through the reset vector
void quit() {
  joy_uninstall();
  resetGraphics();
  fuji_set_boot_mode(BOOT_MODE_LOBBY);
  __asm__ ("bit $c082");
  __asm__ ("dec $03f4");
  __asm__ ("jmp ($fffc)");
}

#endif /* __APPLE2__ */
