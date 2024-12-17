/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           RIT.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the lib_RIT, funct_RIT, IRQ_RIT .c files
** Correlated files:    lib_RIT.c, funct_RIT.c, IRQ_RIT.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef IRQ_RIT_H
#define IRQ_RIT_H

void RIT_IRQHandler(void);
void DrawCountdown(void);
void MovePacMan(int dx, int dy);
extern int game_over_flag; 
extern int countdown;   
#endif

/* init_RIT.c */
extern uint32_t init_RIT( uint32_t RITInterval );
extern void enable_RIT( void );
extern void disable_RIT( void );
extern void reset_RIT( void );
extern int interruptCounter;

/*****************************************************************************
**                            End Of File
******************************************************************************/
