
#include "LPC17xx.h"
#include "joystick.h"

void joystick_init(void) {
    // Configure pins for joystick directions as GPIO inputs
    LPC_PINCON->PINSEL3 &= ~(3 << 26); // P1.29 for UP
    LPC_GPIO1->FIODIR &= ~(1 << 29);   // Input

    LPC_PINCON->PINSEL3 &= ~(3 << 26); // P1.26 for DOWN
    LPC_GPIO1->FIODIR &= ~(1 << 26);   // Input

    LPC_PINCON->PINSEL3 &= ~(3 << 26); // P1.27 for LEFT
    LPC_GPIO1->FIODIR &= ~(1 << 27);   // Input

    LPC_PINCON->PINSEL3 &= ~(3 << 26); // P1.28 for RIGHT
    LPC_GPIO1->FIODIR &= ~(1 << 28);   // Input
}

// Read joystick UP state
int joystick_read_up(void) {
    return !(LPC_GPIO1->FIOPIN & (1 << 29));
}

// Read joystick DOWN state
int joystick_read_down(void) {
    return !(LPC_GPIO1->FIOPIN & (1 << 26));
}

// Read joystick LEFT state
int joystick_read_left(void) {
    return !(LPC_GPIO1->FIOPIN & (1 << 27));
}

// Read joystick RIGHT state
int joystick_read_right(void) {
    return !(LPC_GPIO1->FIOPIN & (1 << 28));
}
