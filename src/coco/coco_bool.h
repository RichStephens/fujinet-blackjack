#ifdef _CMOC_VERSION_

#ifndef _COCO_BOOL_H
#define _COCO_BOOL_H

#include <cmoc.h>
#include <coco.h>
#define false 0
#define true 1
#ifndef BOOL_DEF
#if !defined(FN_FUJI_H) && !defined(bool)
#define BOOL_DEF 1
typedef uint8_t  bool;
#endif
#endif

#endif

#endif
