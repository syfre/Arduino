#include "proc.h"

#define _FONT_STORAGE_  rom far char
#define _FONT_PTR_  rom far char *

//#define FONT_5x8
#define FONT_5x7

#ifdef FONT_5x8
extern _FONT_STORAGE_ font_5x8[];
#define FONT_BASE       0x20
#define FONT_WIDTH	5
#define FONT_HEIGHT	8
#define FONT_SEP_WIDTH	0
#define FONT_SEP_HEIGHT	2
#define TFT_FONT        font_5x8
#endif

#ifdef FONT_5x7
extern _FONT_STORAGE_ font_5x7[];
#define FONT_BASE       0
#define FONT_WIDTH	5
#define FONT_HEIGHT	7
#define FONT_SEP_WIDTH	0
#define FONT_SEP_HEIGHT	2
#define TFT_FONT        font_5x7
#endif


