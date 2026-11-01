#ifndef VARS_H
#define VARS_H

// Each platform's vars.h is guarded by its own compiler macro
#include "../coco/vars.h"
#include "../atari/vars.h"
#include "../apple2/vars.h"
#include "../msdos/vars.h"
#include "../model2/vars.h"

#ifndef WIDTH
#error "no platform vars.h matched this build"
#endif

#endif /* VARS_H */
