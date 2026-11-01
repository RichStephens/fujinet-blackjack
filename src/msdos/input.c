#ifdef __WATCOMC__

#include <bios.h>
#include "../platform-specific/input.h"

unsigned char readJoystick() {
  return 0;
}

unsigned char biosKbhit(void) {
  return _bios_keybrd(_KEYBRD_READY) != 0;
}

unsigned char cgetc(void) {
  static unsigned short k;

  k = _bios_keybrd(_KEYBRD_READ);
  return (k & 0xFF) ? (unsigned char)k : (unsigned char)(k >> 8) | 0x80;
}

#endif /* __WATCOMC__ */
