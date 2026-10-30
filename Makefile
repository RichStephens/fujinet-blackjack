PRODUCT = blkjack
#PLATFORMS += adam
#PLATFORMS += adam_cpm
#PLATFORMS += apple2
#PLATFORMS += atari
#PLATFORMS += c64
PLATFORMS += coco
#PLATFORMS += msdos
#PLATFORMS += msxrom

# You can run 'make <platform>' to build for a specific platform,
# or 'make <platform>/<target>' for a platform-specific target.
# Example shortcuts:
#   make coco        → build for coco
#   make apple2/disk → build the 'disk' target for apple2

# SRC_DIRS may use the literal %PLATFORM% token.
# It expands to the chosen PLATFORM plus any of its combos.
SRC_DIRS = src src/%PLATFORM%

# FUJINET_LIB can be
# - a version number such as 4.7.6
# - a directory which contains the libs for each platform
# - a zip file with an archived fujinet-lib
# - a URL to a git repo
# - empty which will use whatever is the latest
# - undefined, no fujinet-lib will be used
FUJINET_LIB =

# HIRESTXT_LIB can be
# - a version number such as 0.5.1.7
# - a directory which contains the built library
# - a URL to a git repo
# - empty which will use whatever is the latest
# - undefined, no hirestxt-mod will be used
# Releases 0.5.1.7 and later default to no VT52; append +vt52 to get
# it (e.g. 0.5.1.7+vt52, or vt52 alone for the latest). Older releases,
# directories and git URLs always include it.
# Only used for coco/dragon builds.
#HIRESTXT_LIB =

# Define extra dirs ("combos") that expand with a platform.
# Format: platform+=combo1,combo2
PLATFORM_COMBOS = \
  c64+=commodore \
  atarixe+=atari \
  msxrom+=msx \
  msxdos+=msx \
  adam_cpm+=adam

# CoCo 3 build: same sources as CoCo 1/2, compiled with -DCOCO3 for the
# 320x200x16 GIME mode. The framebuffer lives at $8000 via MMU Task 1.
ifeq ($(MAKE_COCO3),COCO3)
  CFLAGS_EXTRA_COCO += -DCOCO3
  LDFLAGS_EXTRA_COCO += --org=1000 --limit=7000
  COCO_CHARSET_SRC := support/coco/pmode3_coco3.fnt
else
  # The CoCo 1/2 hires screen is fixed at $6000 (src/coco/hires.h)
  LDFLAGS_EXTRA_COCO += --org=1000 --limit=6000
  COCO_CHARSET_SRC := support/coco/pmode3.fnt
endif

# src/coco/charset.s includes a fixed path, so stage the model's font there
build/charset_active.fnt: $(COCO_CHARSET_SRC) | build
	cp $< $@

build:
	mkdir -p $@

build/$(PRODUCT)/coco/src/coco/charset.o: build/charset_active.fnt

include mekkogx/toplevel-rules.mk

# If you need to add extra platform-specific steps, do it below:
#   coco/r2r:: coco/custom-step1
#   coco/r2r:: coco/custom-step2
# or
#   apple2/disk: apple2/custom-step1 apple2/custom-step2

# CoCo targets:
#   make coco        → CoCo 1/2 build
#   make coco3       → CoCo 3 build
#   make coco-dist   → combined disk with loader + both CoCo binaries

.PHONY: coco3 coco-dist

coco3:
	$(MAKE) coco MAKE_COCO3=COCO3

# The loader (support/coco/loader.c) runs BLKJACK2 or BLKJACK3 by model
R2R_PRODUCT = r2r/coco/$(PRODUCT)
COCO_DISK   = $(R2R_PRODUCT).dsk

coco-dist:
	$(MAKE) clean
	rm -rf build
	$(MAKE) coco
	mv $(R2R_PRODUCT).bin $(R2R_PRODUCT)2.bin

	rm -rf build
	$(MAKE) coco3
	mv $(R2R_PRODUCT).bin $(R2R_PRODUCT)3.bin

	cmoc -o $(R2R_PRODUCT).bin support/coco/loader.c

	$(RM) $(COCO_DISK)
	decb dskini $(COCO_DISK)
	decb copy -t -0 support/coco/autoexec.bas $(COCO_DISK),AUTOEXEC.BAS
	decb copy -b -2 $(R2R_PRODUCT).bin   $(COCO_DISK),BLKJACK.BIN
	decb copy -b -2 $(R2R_PRODUCT)2.bin  $(COCO_DISK),BLKJACK2.BIN
	decb copy -b -2 $(R2R_PRODUCT)3.bin  $(COCO_DISK),BLKJACK3.BIN
