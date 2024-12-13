/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "joystick/joystick.h"
#include <stdlib.h>
#include <stdio.h>


/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void RIT_IRQHandler(void) {
    if (joystick_read_up()) {
        MovePacMan(0, -1); // Move up
    } else if (joystick_read_down()) {
        MovePacMan(0, 1); // Move down
    } else if (joystick_read_left()) {
        MovePacMan(-1, 0); // Move left
    } else if (joystick_read_right()) {
        MovePacMan(1, 0); // Move right
    }

    LPC_RIT->RICTRL |= 0x1; // Clear interrupt flag
}


/******************************************************************************
**                            End Of File
******************************************************************************/
