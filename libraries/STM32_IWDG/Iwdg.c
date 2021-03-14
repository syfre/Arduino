/*----------------------------------------------------------------------------
 * Name:    Timer.c
 * Purpose: Timer usage for STM32
 * Version: V1.00
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stm32f10x_lib.h>                        // STM32F10x Library Definitions
#include "STM32_Init.h"                           // STM32 Initialization


unsigned int ledPosIwdg  = 4;                     // led position for IWDG reset
unsigned int ledPosEnd   = 7;                     // led position loop end

/*----------------------------------------------------------------------------
  delay
  insert a delay time.
 *----------------------------------------------------------------------------*/
void delay(unsigned int nCount)
{
  for(; nCount != 0; nCount--);
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
  int i;

  stm32_Init ();                                  // STM32 setup

  if (RCC->CSR & (1<<29)) {                       // IWDG Reset Flag set
    RCC->CSR |= (1<<24);                          // Clear Reset Flags
	GPIOB->BSRR = 1 << (ledPosIwdg+8);            // LED on
  }
  else {
    GPIOB->BRR  = 1 << (ledPosIwdg+8);            // LED off
  }

  for (i = 0; i < 10; i++) {
    delay(1000000);                               // wait less than watchdog interval
    IWDG->KR  = 0xAAAA;                           // reload the watchdog
  }
  GPIOB->BSRR = 1 << (ledPosEnd+8);               // LED on

  while (1) {                                     // Loop forever
  } // end while
} // end main
