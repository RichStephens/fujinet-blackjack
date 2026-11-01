#ifdef __ADAM__

/**
 * @brief Adam input routines
 * @author Thomas Cherryhomes, Geoff Oltmans
 * @license gpl v.3
 */

#include <eos.h>
#include "../platform-specific/input.h"

static GameControllerData cont;
static unsigned char started, pending;

unsigned char readJoystick()
{
  unsigned char temp = 0;
  eos_read_game_controller(0x03,&cont);
  temp = cont.joystick1;
  temp |= (cont.joystick1_button_left || cont.joystick1_button_right)<<4;

  return temp;
}

static int getPlatformKey(void)
{
  int ch;

  if (!started)
  {
    eos_start_read_keyboard();
    started = 1;
  }
  ch=eos_end_read_keyboard();

  // <=1 means the asynchronous read is still pending (no key yet).
  if (ch>1)
  {
    eos_start_read_keyboard();
    return ch;
  }
  return 0;
}

unsigned char adamKbhit(void)
{
  if (!pending)
    pending = (unsigned char)getPlatformKey();
  return pending != 0;
}

unsigned char adamCgetc(void)
{
  static unsigned char c;

  while (!adamKbhit())
    ;
  c = pending;
  pending = 0;
  return c;
}

#endif /* __ADAM__ */
