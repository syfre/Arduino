#ifndef _CANVAS_
#define _CANVAS_

#include "./core/Canvas.h"
#include "./XPT2046/XPT2046.h"

#if defined(ARDUINO_ARCH_STM32F1)
#include "./Canvas9341_stm32/Canvas9341_stm32.h"
#elif defined(ARDUINO_ARCH_AVR)
  // AVR-specific code
#elif defined(ARDUINO_ARCH_SAM)
  // SAM-specific code
#else
  // generic, non-platform specific code
#endif

#endif