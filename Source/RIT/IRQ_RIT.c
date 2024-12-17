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
//use variable that defined in sample.c to handle the direction of PAC-MAN
extern volatile int current_direction_x;
extern volatile int current_direction_y;

void RIT_IRQHandler(void) {
    interruptCounter++;
    if (interruptCounter >= 14) {
        if (!isPaused && countdown > 0) {
            countdown--;
            DrawCountdown();  // Update the countdown display
        }
        interruptCounter = 0;
    }

    if (countdown == 0) {
        game_over_flag = 1; // Set the game-over flag when countdown reaches zero
        DrawGameOver();
        LPC_RIT->RICTRL &= ~0x1;
        return;
    }

    if (!game_over_flag) { // Allow joystick movements only if the game is not over
        // Update direction only on joystick input
        if (joystick_read_up()) {
            current_direction_x = 0;
            current_direction_y = -1; // Move up
        } else if (joystick_read_down()) {
            current_direction_x = 0;
            current_direction_y = 1;  // Move down
        } else if (joystick_read_left()) {
            current_direction_x = -1;
            current_direction_y = 0;  // Move left
        } else if (joystick_read_right()) {
            current_direction_x = 1;
            current_direction_y = 0;  // Move right
        }

        // Continue moving Pac-Man in the current direction
        MovePacMan(current_direction_x, current_direction_y);
    }

    LPC_RIT->RICTRL |= 0x1; // Clear interrupt flag
}


/******************************************************************************
**                            End Of File
******************************************************************************/
