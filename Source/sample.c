/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define CELL_SIZE     20  // Size of each cell in the labyrinth grid
#define PILL_COLOR    White
#define WALL_COLOR    Blue
#define BG_COLOR      Black
#define TEXT_COLOR    Red

void DrawLabyrinth() {
    int rows = SCREEN_HEIGHT / CELL_SIZE;
    int cols = SCREEN_WIDTH / CELL_SIZE;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Draw walls or pills based on desired labyrinth layout
            if ((i == 0 || i == rows - 1) || (j == 0 || j == cols - 1)) {
                // Draw walls at the boundaries
                LCD_DrawLine(j * CELL_SIZE, i * CELL_SIZE, (j + 1) * CELL_SIZE, i * CELL_SIZE, WALL_COLOR); // Top border
                LCD_DrawLine(j * CELL_SIZE, i * CELL_SIZE, j * CELL_SIZE, (i + 1) * CELL_SIZE, WALL_COLOR); // Left border
            } else {
                // Draw pills in the labyrinth cells
                LCD_SetPoint(j * CELL_SIZE + CELL_SIZE / 2, i * CELL_SIZE + CELL_SIZE / 2, PILL_COLOR);
            }
        }
    }
}

void InitializeDisplay() {
    // Clear the screen and set the background color
    LCD_Clear(BG_COLOR);

    // Display the Score
    GUI_Text(120, 5, (uint8_t *)"Score: 00", TEXT_COLOR, BG_COLOR);

    // Display the Timer
    GUI_Text(5, 5, (uint8_t *)"Timer: 60s", TEXT_COLOR, BG_COLOR);

    // Draw the game board
    DrawLabyrinth();
}

int main(void) {
    SystemInit();
    LCD_Initialization();

    InitializeDisplay();

    while (1) {
        // Main loop
    }

    return 0;
}


/*
int main(void)
{
  SystemInit();  												
	
  LCD_Initialization();
	
  	TP_Init();
	TouchPanel_Calibrate();
	
	LCD_Clear(Black);
	GUI_Text(0, 280, (uint8_t *) " touch here : 1 sec to clear  ", Red, White);
	//LCD_DrawLine(0, 0, 200, 200, White);
	//init_timer(0, 0x1312D0 ); 						
	//init_timer(0, 0x6108 ); 						 
	init_timer(0, 0x4E2 ); 						   
	//init_timer(0, 0xC8 ); 						   
	
	enable_timer(0);
	
	LPC_SC->PCON |= 0x1;						
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}
*/

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
