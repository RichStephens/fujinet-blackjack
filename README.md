# Blackjack

Multiplayer blackjack against the dealer for FujiNet on the Apple II, Atari
8-bit, CoCo 1/2, CoCo 3 and MS-DOS. Up to five players share a table, and
some tables seat bots. The server is `fujinet-game-system/blackjack` in
[FujiNetWIFI/servers](https://github.com/FujiNetWIFI/servers).

## Building

```
defoogi make apple2
defoogi make atari
defoogi make coco-dist
defoogi make msdos
```

These build `r2r/apple2/blkjack.po`, `r2r/atari/blkjack.atr`,
`r2r/coco/blkjack.dsk` and `r2r/msdos/blkjack.img`. The CoCo disk's
loader runs `BLKJACK2.BIN` on a CoCo 1/2 and `BLKJACK3.BIN` on a CoCo 3.
`make coco` and `make coco3` build one model alone.

The client connects to `https://blackjack.carr-designs.com/`, or to the
server picked in the FujiNet lobby (lobby key 0x0C).
