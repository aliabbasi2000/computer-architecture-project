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
#define CELL_SIZE     14
#define GRID_ROWS     18
#define GRID_COLS     16  
#define PILL_COLOR    Yellow
#define WALL_COLOR    Blue
#define BG_COLOR      Black
#define TEXT_COLOR    Red
#define POWER_PILL_COLOR Yellow
#define PACMAN_COLOR  Yellow
#define POWER_PILL_RADIUS 4
#define PAC_MAN_RADIUS 6

// Maze definitions
#define EMPTY 0
#define PILL 1
#define POWER_PILL 2
#define WALL 3

// Game state variables
int pacman_x, pacman_y;
int score = 0;
int game_over_flag = 0;
int countdown = 60;
int lives = 1;
int next_life_score = 1000;
int direction_x = 0;
int direction_y = 0;
int interruptCounter = 0;

volatile int current_direction_x = 0; // X direction (horizontal movement)
volatile int current_direction_y = 0; // Y direction (vertical movement)

// Maze representation
/* real maze */
int mazeGrid[GRID_ROWS][GRID_COLS] = {
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
    {3, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 3},
    {3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 1, 3},
    {3, 1, 3, 0, 0, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3},
    {3, 1, 3, 1, 3, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3},
    {3, 1, 3, 0, 3, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3},
    {3, 1, 3, 1, 3, 3, 3, 3, 3, 3, 1, 3, 1, 3, 1, 3},
    {3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3},
    {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {3, 1, 3, 1, 3, 1, 3, 3, 3, 3, 1, 1, 3, 1, 1, 3},
    {3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3},
    {3, 1, 3, 1, 3, 1, 3, 3, 3, 3, 1, 1, 3, 1, 1, 3},
    {3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 3},
    {3, 1, 3, 3, 3, 1, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3},
    {3, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3},
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}
};


void delay_ms(unsigned int ms) {
    unsigned int i;
    for (i = 0; i < ms * 10000; i++) {
        __NOP();
    }
}

void DrawScore() {
		if(game_over_flag){
			return;
		}
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    LCD_DrawRect(100, 0, 239, 20, BG_COLOR);
    GUI_Text(120, 5, (uint8_t *)scoreText, TEXT_COLOR, BG_COLOR);
}


void DrawLives(){
		char scoreText[20];
		sprintf(scoreText, "Remaining Lives: %d", lives);
    LCD_DrawRect(10, SCREEN_HEIGHT - 20, 100, 20, BG_COLOR);
		GUI_Text(5, SCREEN_HEIGHT - 20, (uint8_t *)scoreText, TEXT_COLOR, BG_COLOR);
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



void DrawCountdown() {
    char countdownText[20];
    sprintf(countdownText, "Time: %02d", countdown);  // Use %02d for two-digit formatting
    LCD_DrawRect(100, 20, 239, 40, BG_COLOR);         // Clear the area where the countdown will be displayed
    GUI_Text(40, 5, (uint8_t *)countdownText, TEXT_COLOR, BG_COLOR);  // Display the countdown text
}

void DrawGameOver() {
    char gameOverText[] = "Game Over!";
    LCD_DrawRect(80, 140, 160, 40, BG_COLOR);  // Clear the area
    GUI_Text(90, 150, (uint8_t *)gameOverText, TEXT_COLOR, BG_COLOR);  // Display Game Over message
}

int checkVictory() {
    // Check if all pills and power pills have been eaten
    int i , j;
    for ( i = 0; i < GRID_ROWS; i++) {
        for ( j = 0; j < GRID_COLS; j++) {
            if (mazeGrid[i][j] == PILL || mazeGrid[i][j] == POWER_PILL) {
                return 0;  // Pills are still present, not victory
            }
        }
    }
    return 1;  // All pills collected
}

void DrawVictoryScreen() {
    LCD_Clear(White);  // Clear the screen
    char victoryText[] = "Victory!";
    LCD_DrawRect(80, 140, 160, 40, White);  // Clear the area
    GUI_Text(90, 150, (uint8_t *)victoryText, TEXT_COLOR, White);  // Display Victory message
}


void DrawMaze() {
    int start_x = (SCREEN_WIDTH - GRID_COLS * CELL_SIZE) / 2;
    int start_y = 40;

		int i;
    for (i = 0; i < GRID_ROWS; i++) {
      int j;  
			for (j = 0; j < GRID_COLS; j++) {
            if (mazeGrid[i][j] == 3) { // If the grid cell is a wall
                int rect_x = start_x + j * CELL_SIZE;
                int rect_y = start_y + i * CELL_SIZE;
                LCD_FillRect(rect_x, rect_y, CELL_SIZE-3, CELL_SIZE-3, WALL_COLOR);
            } else if (mazeGrid[i][j] == 1) { // Draw normal pill
                int pill_x = start_x + j * CELL_SIZE + CELL_SIZE / 2;
                int pill_y = start_y + i * CELL_SIZE + CELL_SIZE / 2;
                LCD_SetPoint(pill_x, pill_y, PILL_COLOR);
            } else if (mazeGrid[i][j] == 2) { // Draw power pill
                int pill_x = start_x + j * CELL_SIZE + CELL_SIZE / 2;
                int pill_y = start_y + i * CELL_SIZE + CELL_SIZE / 2;
                LCD_DrawCircle(pill_x, pill_y, POWER_PILL_RADIUS, POWER_PILL_COLOR);
            }
        }
    }
}




void MovePacMan(int dx, int dy) {
    if (game_over_flag || isPaused) {
        return;
    }

    int new_x = pacman_x + dx;
    int new_y = pacman_y + dy;

    // Check if Pac-Man hits a wall
    if (new_y >= 0 && new_y < GRID_ROWS && new_x >= 0 && new_x < GRID_COLS) {
        if (mazeGrid[new_y][new_x] == WALL) {
            // Stop Pac-Man if hitting a wall
            current_direction_x = 0;
            current_direction_y = 0;
            return;
        }
    } else {
        // Teleport logic if out of bounds
        if (new_x < 0) new_x = GRID_COLS - 1;
        else if (new_x >= GRID_COLS) new_x = 0;
        if (new_y < 0) new_y = GRID_ROWS - 1;
        else if (new_y >= GRID_ROWS) new_y = 0;
    }

    // Update Pac-Man's position
    ErasePacMan(pacman_x, pacman_y);  // Erase old position
    pacman_x = new_x;
    pacman_y = new_y;
    DrawPacMan(pacman_x, pacman_y);

    // Collect pills or power pills
    if (mazeGrid[pacman_y][pacman_x] == PILL) {
        score += 10;
        mazeGrid[pacman_y][pacman_x] = EMPTY;
        DrawScore();
    } else if (mazeGrid[pacman_y][pacman_x] == POWER_PILL) {
        score += 50;
        mazeGrid[pacman_y][pacman_x] = EMPTY;
        DrawScore();
    }

		
		// Check for extra life
    if (score >= next_life_score) {
    lives++;
    next_life_score += 1000;  // Update the next milestone
		}

    ErasePacMan(pacman_x, pacman_y); // Erase current position
    pacman_x = new_x;
    pacman_y = new_y;
    DrawPacMan(pacman_x, pacman_y); // Draw new position
    DrawScore();
		DrawLives();
		
		if (checkVictory()) {
			game_over_flag = 1;
			DrawVictoryScreen();
		}
}


// Function to generate a power pill
void GeneratePowerPill() {
    int generated = 0;
    while (generated < 1) {
        int row = rand() % GRID_ROWS;
        int col = rand() % GRID_COLS;
        if (mazeGrid[row][col] == PILL) {
            mazeGrid[row][col] = POWER_PILL;
            DrawMaze();
            generated++;
        }
    }
}


void InitializeDisplay() {
    LCD_Clear(BG_COLOR);
    DrawScore();
		DrawCountdown();
		DrawLives();
    DrawMaze();
		

    pacman_x = 1;
    pacman_y = 1;
    DrawPacMan(pacman_x, pacman_y);
}

int main(void) {
    SystemInit();
    LCD_Initialization();
    joystick_init();
    InitializeDisplay();

    init_RIT(2500000); //25MHz
    enable_RIT();
		init_INT0();

    while (1) {
        if (game_over_flag) {

            break;
					}

				else if (!isPaused){
					if (direction_x != 0 || direction_y != 0) {  // Only move if a direction is set
							MovePacMan(direction_x, direction_y);
					}
				}
				

				
        __ASM("wfi"); // Wait for interrupt
    }
}


/*********************************************************************************************************

      END FILE

*********************************************************************************************************/