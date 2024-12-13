																			 /*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           joystick.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the lib_joystick, funct_joystick .c files
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#ifndef JOYSTICK_H
#define JOYSTICK_H

void joystick_init(void);
int joystick_read_up(void);
int joystick_read_down(void);
int joystick_read_left(void);
int joystick_read_right(void);

#endif