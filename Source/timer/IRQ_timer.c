/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "LPC17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include <stdio.h> /*for sprintf*/

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


#include <stdbool.h>
#include "GLCD/GLCD.h"
#define BG_COLOR    0x0000  // Example background color
#define TEXT_COLOR  0xFFFF  // Example text color
#define WALL_COLOR_BLUE 0x001F
#define WALL_COLOR    Blue

//volatile uint8_t last_button_state = 1;  // Assume button is not pressed initially (logic high)

volatile bool isPaused = false;  // Game starts in PAUSE mode

uint16_t pauseArea[160 * 40];  // Example size (adjust as per your screen and message size)

#define PAUSE_X 90  // X coordinate of the "PAUSE" message
#define PAUSE_Y 150  // Y coordinate of the "PAUSE" message
#define PAUSE_WIDTH 160
#define PAUSE_HEIGHT 40


// Function to capture the background where the "PAUSE" message will appear
void CapturePauseArea(void) {
    int i;
    for ( i = 0; i < PAUSE_HEIGHT; i++) {
            int j;
        for ( j = 0; j < PAUSE_WIDTH; j++) {
            pauseArea[i * PAUSE_WIDTH + j] = LCD_GetPoint(PAUSE_X + j, PAUSE_Y + i);
        }
    }
}



// Function to restore the background when resuming
void RestorePauseArea(void) {
    int i;
    for ( i = 0; i < PAUSE_HEIGHT; i++) {
            int j;
        for ( j = 0; j < PAUSE_WIDTH; j++) {
            LCD_SetPoint(PAUSE_X + j, PAUSE_Y + i, pauseArea[i * PAUSE_WIDTH + j]);
        }
    }
}


// Function to initialize INT0 (External Interrupt 0)
void init_INT0(void) {
    LPC_PINCON->PINSEL4 |= (1 << 20);  // Set P2.10 as EINT0
    LPC_SC->EXTMODE |= (1 << 0);       // Edge-sensitive mode
    LPC_SC->EXTPOLAR &= ~(1 << 0);     // Falling-edge sensitive
    NVIC_EnableIRQ(EINT0_IRQn);        // Enable EINT0 interrupt in NVIC
}


// EINT0 Interrupt Service Routine (ISR)
void EINT0_IRQHandler(void) {
    // Clear the interrupt flag
    LPC_SC->EXTINT |= (1 << 0);
    // Toggle pause state
    isPaused = !isPaused;
    if (isPaused) {
				CapturePauseArea();
        // Display "PAUSE" message
        GUI_Text(PAUSE_X, PAUSE_Y, (uint8_t *)"PAUSE", White, Black);
    } else {
        // Clear the "PAUSE" message
        RestorePauseArea();
    }
}










/*
void TIMER0_IRQHandler (void)
{
	static int clear = 0;
	char time_in_char[5] = "";
	int mosse[6][2]={{1,1},{-1,-1},{1,0},{-1,0},{0,1},{0,-1}};
	int i=0;
	
  if(getDisplayPoint(&display, Read_Ads7846(), &matrix )){
		if(display.y < 280){
			for(i=0;i<6;i++)
				TP_DrawPoint(display.x+mosse[i][0],display.y+mosse[i][1]);
			TP_DrawPoint(display.x,display.y);
			GUI_Text(200, 0, (uint8_t *) "     ", Blue, Blue);
			clear = 0;
		}
		else{			
			if(display.y <= 0x13E){			
				clear++;
				if(clear%20 == 0){
					sprintf(time_in_char,"%4d",clear/20);
					GUI_Text(200, 0, (uint8_t *) time_in_char, White, Blue);
					if(clear == 200){	// 1 seconds = 200 times * 500 us
						LCD_Clear(Black);
						GUI_Text(0, 280, (uint8_t *) " touch here : 1 sec to clear ", Blue, White);			
						clear = 0;
					}
				}
			}
		}
	}
	else{
		//do nothing if touch returns values out of bounds
	}
  LPC_TIM0->IR = 1;			
  return;
}
*/

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}




/******************************************************************************
**                            End Of File
******************************************************************************/
