# Blackjack

Multiplayer blackjack against the dealer for FujiNet on the CoCo 1/2 and
CoCo 3. Up to five players share a table, and some tables seat bots. The
server is `fujinet-game-system/blackjack` in
[FujiNetWIFI/servers](https://github.com/FujiNetWIFI/servers).

## Building

```
defoogi make coco-dist
```

This builds `r2r/coco/blkjack.dsk`. Its loader runs `BLKJACK2.BIN` on a
CoCo 1/2 and `BLKJACK3.BIN` on a CoCo 3. `make coco` and `make coco3`
build one model alone.

The client connects to `https://blackjack.carr-designs.com/`, or to the
server picked in the FujiNet lobby (lobby key 0x0C).
