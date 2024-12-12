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
#include <stdlib.h>

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320
#define CELL_SIZE     15  // Size of each cell in the labyrinth grid
#define PILL_COLOR    Yellow
#define WALL_COLOR    Blue
#define BG_COLOR      Black
#define TEXT_COLOR    Red
#define POWER_PILL_COLOR Red
#define NUM_RED_PILLS 6 
#define POWER_PILL_RADIUS 5


int redPillPositions[NUM_RED_PILLS][2] = {
    {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13}  // Example positions
};

void DrawMaze() {
    int rows = 16; // Rows for pills (240 pills = 12 rows * 20 cols)
    int cols = 15; // Columns for pills
    int start_x = (SCREEN_WIDTH - cols * CELL_SIZE) / 2; // Center horizontally
    int start_y = 40; // Start drawing below the text area

    // Draw walls (maze structure)
    LCD_DrawLine(start_x, start_y, start_x + cols * CELL_SIZE, start_y, WALL_COLOR); // Top wall
    LCD_DrawLine(start_x, start_y, start_x, start_y + rows * CELL_SIZE, WALL_COLOR); // Left wall
    LCD_DrawLine(start_x + cols * CELL_SIZE, start_y, start_x + cols * CELL_SIZE, start_y + rows * CELL_SIZE, WALL_COLOR); // Right wall
    LCD_DrawLine(start_x, start_y + rows * CELL_SIZE, start_x + cols * CELL_SIZE, start_y + rows * CELL_SIZE, WALL_COLOR); // Bottom wall

    // Additional inner maze walls
    // Horizontal lines
    LCD_DrawLine(start_x + CELL_SIZE * 2, start_y + CELL_SIZE * 2, start_x + CELL_SIZE * 18, start_y + CELL_SIZE * 2, WALL_COLOR);
    LCD_DrawLine(start_x + CELL_SIZE * 4, start_y + CELL_SIZE * 6, start_x + CELL_SIZE * 16, start_y + CELL_SIZE * 6, WALL_COLOR);
    LCD_DrawLine(start_x + CELL_SIZE * 6, start_y + CELL_SIZE * 10, start_x + CELL_SIZE * 14, start_y + CELL_SIZE * 10, WALL_COLOR);

    // Vertical lines
    LCD_DrawLine(start_x + CELL_SIZE * 2, start_y + CELL_SIZE * 2, start_x + CELL_SIZE * 2, start_y + CELL_SIZE * 10, WALL_COLOR);
    LCD_DrawLine(start_x + CELL_SIZE * 18, start_y + CELL_SIZE * 2, start_x + CELL_SIZE * 18, start_y + CELL_SIZE * 10, WALL_COLOR);
    LCD_DrawLine(start_x + CELL_SIZE * 10, start_y + CELL_SIZE * 4, start_x + CELL_SIZE * 10, start_y + CELL_SIZE * 8, WALL_COLOR);

    // Fill the remaining cells with pills
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int pill_x = start_x + j * CELL_SIZE + CELL_SIZE / 2;
            int pill_y = start_y + i * CELL_SIZE + CELL_SIZE / 2;

            // Skip pills where walls are located
            if (!((i == 0 || i == rows - 1 || j == 0 || j == cols - 1) ||  // Boundary walls
                  (i == 2 && j >= 2 && j <= 18) ||                        // Top horizontal wall
                  (i == 6 && j >= 4 && j <= 16) ||                        // Middle horizontal wall
                  (i == 10 && j >= 6 && j <= 14) ||                       // Bottom horizontal wall
                  (j == 2 && i >= 2 && i <= 10) ||                        // Left vertical wall
                  (j == 18 && i >= 2 && i <= 10) ||                       // Right vertical wall
                  (j == 10 && i >= 4 && i <= 8))) {                       // Center vertical wall
                
                // Check if the current position is one of the red pill positions
                int isRedPill = 0;
                for (int k = 0; k < NUM_RED_PILLS; k++) {
                    if (redPillPositions[k][0] == i && redPillPositions[k][1] == j) {
                        isRedPill = 1;
                        break;
                    }
                }

                // Display red pill if this position matches one of the predefined red pill positions
                if (isRedPill) {
                    LCD_DrawCircle(pill_x, pill_y, POWER_PILL_RADIUS, POWER_PILL_COLOR);  // Red pill color
                } else {
                    LCD_SetPoint(pill_x, pill_y, PILL_COLOR);  // Regular pill color
                }
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
    
    // Draw the lives
    GUI_Text(5, SCREEN_HEIGHT - 20, (uint8_t *)"Remaining Lives: 3", TEXT_COLOR, BG_COLOR);

    // Draw the game board
    DrawMaze();
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
