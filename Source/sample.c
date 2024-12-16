#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include <stdlib.h>
#include "timer.h"
#include <stdio.h>
#include "joystick/joystick.h"
#include "RIT/RIT.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag;
#endif

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320
#define CELL_SIZE     15
#define GRID_ROWS     16
#define GRID_COLS     15
#define PILL_COLOR    Yellow
#define WALL_COLOR    Blue
#define BG_COLOR      Black
#define TEXT_COLOR    Red
#define POWER_PILL_COLOR Yellow
#define PACMAN_COLOR  Blue
#define NUM_RED_PILLS 6
#define POWER_PILL_RADIUS 5
#define PAC_MAN_RADIUS 9

void delay_ms(unsigned int ms) {
    unsigned int i;
    for (i = 0; i < ms * 10000; i++) {
        __NOP();
    }
}

int redPillPositions[NUM_RED_PILLS][2] = {
    {1, 1}, {1, 13}, {7, 2}, {7, 12}, {14, 3}, {14, 11}
};

int pacman_x, pacman_y;
int score = 0;
int pillGrid[GRID_ROWS][GRID_COLS] = {0};

void DrawScore() {
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    LCD_DrawRect(100, 0, 239, 20, BG_COLOR);
    GUI_Text(120, 5, (uint8_t *)scoreText, TEXT_COLOR, BG_COLOR);
}

void DrawPacMan(int x, int y) {
    int screen_x = (SCREEN_WIDTH - GRID_COLS * CELL_SIZE) / 2 + x * CELL_SIZE + CELL_SIZE / 2;
    int screen_y = 40 + y * CELL_SIZE + CELL_SIZE / 2;
    LCD_DrawCircle(screen_x, screen_y, PAC_MAN_RADIUS, PACMAN_COLOR);
}

void ErasePacMan(int x, int y) {
    int screen_x = (SCREEN_WIDTH - GRID_COLS * CELL_SIZE) / 2 + x * CELL_SIZE + CELL_SIZE / 2;
    int screen_y = 40 + y * CELL_SIZE + CELL_SIZE / 2;
    LCD_DrawCircle(screen_x, screen_y, PAC_MAN_RADIUS, BG_COLOR);
}

void MovePacMan(int dx, int dy) {
    int new_x = pacman_x + dx;
    int new_y = pacman_y + dy;

    if (new_x < 0 || new_x >= GRID_COLS || new_y < 0 || new_y >= GRID_ROWS) {
        return;
    }

    if (pillGrid[new_y][new_x] == 1) {
        score += 10;
        pillGrid[new_y][new_x] = 0;
    } else if (pillGrid[new_y][new_x] == 2) {
        score += 50;
        pillGrid[new_y][new_x] = 0;
    }

    ErasePacMan(pacman_x, pacman_y);
    pacman_x = new_x;
    pacman_y = new_y;
    DrawPacMan(pacman_x, pacman_y);
    DrawScore();
}

void DrawMaze() {
    int start_x = (SCREEN_WIDTH - GRID_COLS * CELL_SIZE) / 2;
    int start_y = 40;

    // Draw maze borders
    LCD_DrawLine(start_x, start_y, start_x + GRID_COLS * CELL_SIZE, start_y, WALL_COLOR);
    LCD_DrawLine(start_x, start_y, start_x, start_y + GRID_ROWS * CELL_SIZE, WALL_COLOR);
    LCD_DrawLine(start_x + GRID_COLS * CELL_SIZE, start_y, start_x + GRID_COLS * CELL_SIZE, start_y + GRID_ROWS * CELL_SIZE, WALL_COLOR);
    LCD_DrawLine(start_x, start_y + GRID_ROWS * CELL_SIZE, start_x + GRID_COLS * CELL_SIZE, start_y + GRID_ROWS * CELL_SIZE, WALL_COLOR);

    // Draw walls
    LCD_DrawLine(start_x + CELL_SIZE * 2, start_y + CELL_SIZE * 2, start_x + CELL_SIZE * 13, start_y + CELL_SIZE * 2, WALL_COLOR);
    LCD_DrawLine(start_x + CELL_SIZE * 4, start_y + CELL_SIZE * 6, start_x + CELL_SIZE * 11, start_y + CELL_SIZE * 6, WALL_COLOR);
    LCD_DrawLine(start_x + CELL_SIZE * 6, start_y + CELL_SIZE * 10, start_x + CELL_SIZE * 9, start_y + CELL_SIZE * 10, WALL_COLOR);

    LCD_DrawLine(start_x + CELL_SIZE * 2, start_y + CELL_SIZE * 2, start_x + CELL_SIZE * 2, start_y + CELL_SIZE * 10, WALL_COLOR);
    LCD_DrawLine(start_x + CELL_SIZE * 13, start_y + CELL_SIZE * 2, start_x + CELL_SIZE * 13, start_y + CELL_SIZE * 10, WALL_COLOR);

    // Draw pills
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            if (pillGrid[i][j] == 0) {
                continue;
            }

            int pill_x = start_x + j * CELL_SIZE + CELL_SIZE / 2;
            int pill_y = start_y + i * CELL_SIZE + CELL_SIZE / 2;
            if (pillGrid[i][j] == 1) {
                LCD_SetPoint(pill_x, pill_y, PILL_COLOR);
            } else if (pillGrid[i][j] == 2) {
                LCD_DrawCircle(pill_x, pill_y, POWER_PILL_RADIUS, POWER_PILL_COLOR);
            }
        }
    }
}

void InitializeDisplay() {
    LCD_Clear(BG_COLOR);
    DrawScore();
    GUI_Text(5, SCREEN_HEIGHT - 20, (uint8_t *)"Remaining Lives: 3", TEXT_COLOR, BG_COLOR);

    // Initialize grid and pills
		int i;
    for (i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            pillGrid[i][j] = 1;
        }
    }
		
		//--------------------------------------------------
		//int i; 
    for (i = 0; i < NUM_RED_PILLS; i++) {
        pillGrid[redPillPositions[i][0]][redPillPositions[i][1]] = 2;
    }

    DrawMaze();

    pacman_x = 7;
    pacman_y = 8;
    DrawPacMan(pacman_x, pacman_y);
}

int main(void) {
    SystemInit();
    LCD_Initialization();
    joystick_init();
    InitializeDisplay();

    init_RIT(0x004C4B40);
    enable_RIT();

    while (1) {
        __ASM("wfi"); // Wait for interrupt
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