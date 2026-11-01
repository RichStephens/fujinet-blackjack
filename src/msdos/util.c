#ifdef __WATCOMC__

#include <dos.h>
#include <i86.h>
#include <process.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fujinet-fuji.h>
#include "../platform-specific/graphics.h"
#include "../platform-specific/util.h"

#define HOST_SLOTS 8
#define DEVICE_SLOTS 8
#define LOBBY_HOST "ec.tnfs.io"
#define LOBBY_DISK "msdos/lobby.img"

typedef struct {
  uint8_t query;
  char signature[4];
  uint8_t unit;
} FujiIoctlQuery;

static HostSlot hostSlots[HOST_SLOTS];
static DeviceSlot deviceSlots[DEVICE_SLOTS];
static clock_t startClock;

void resetTimer() {
  startClock = clock();
}

int getTime() {
  return (int)((clock() - startClock) * 60 / CLOCKS_PER_SEC);
}

// The DOS drive the FujiNet driver gives device slot 0
static int slotDrive(void) {
  static FujiIoctlQuery query;
  union REGS r;
  struct SREGS sr;
  int drive;

  for (drive = 3; drive <= 26; drive++) {
    memset(&query, 0, sizeof(query));
    r.w.ax = 0x4404;
    r.h.bl = (unsigned char)drive;
    r.w.cx = sizeof(query);
    r.w.dx = FP_OFF(&query);
    sr.ds = FP_SEG(&query);
    int86x(0x21, &r, &r, &sr);
    if (!r.w.cflag && !memcmp(query.signature, "FUJI", 4) && query.unit == 0)
      return drive;
  }
  return -1;
}

// Mount the lobby disk in slot 0 and run its AUTOEXEC.BAT
void quit() {
  static uint8_t slot;
  unsigned int drives;
  int drive;

  resetGraphics();
  printf("Loading Lobby...\n");

  fuji_get_host_slots((unsigned char *)hostSlots, HOST_SLOTS);
  for (slot = 0; slot < HOST_SLOTS; slot++)
    if (!stricmp(LOBBY_HOST, (char *)hostSlots[slot]))
      break;
  if (slot == HOST_SLOTS) {
    slot = HOST_SLOTS - 1;
    strcpy((char *)hostSlots[slot], LOBBY_HOST);
    fuji_put_host_slots((unsigned char *)hostSlots, HOST_SLOTS);
  }
  fuji_mount_host_slot(slot);

  fuji_get_device_slots(deviceSlots, DEVICE_SLOTS);
  deviceSlots[0].hostSlot = slot;
  deviceSlots[0].mode = 0;
  strcpy((char *)deviceSlots[0].file, LOBBY_DISK);
  fuji_put_device_slots(deviceSlots, DEVICE_SLOTS);
  fuji_set_device_filename(0, slot, 0, LOBBY_DISK);
  fuji_mount_disk_image(0, 1);

  drive = slotDrive();
  if (drive >= 0) {
    _dos_setdrive(drive, &drives);
    chdir("\\");
    spawnlp(P_OVERLAY, "COMMAND.COM", "COMMAND.COM", "/C", "AUTOEXEC.BAT", NULL);
  }
  exit(0);
}

#endif /* __WATCOMC__ */
