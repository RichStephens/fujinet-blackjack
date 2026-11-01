#ifdef __APPLE2__

#include <joystick.h>

static unsigned char installed, canRead;

// A missing stick reads as pegged, so only use it if it starts centered
unsigned char readJoystick() {
  if (!installed) {
    installed = 1;
    if (joy_install(joy_static_stddrv) == JOY_ERR_OK)
      canRead = joy_read(JOY_1) == 0;
  }

  return canRead ? joy_read(JOY_1) : 0;
}

#endif /* __APPLE2__ */
