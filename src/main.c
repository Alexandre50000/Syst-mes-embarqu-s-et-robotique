#include <stm32f4xx.h>
#include <system_clock_config.h>
#include "gpio.h"
#include "main.h"
#include "timer.h"
#include "motor.h"
#include "selector.h"


#define PI                  3.1415926536f
//TO ADJUST IF NECESSARY. NOT ALL THE E-PUCK2 HAVE EXACTLY THE SAME WHEEL DISTANCE
#define WHEEL_DISTANCE      5.35f    //cm
#define PERIMETER_EPUCK     (PI * WHEEL_DISTANCE)

// Init function required by __libc_init_array
void _init(void) {}

// Simple delay function
void delay(unsigned int n)
{
    while (n--) {
        __asm__ volatile ("nop");
    }
}


int main(void)
{
    SystemClock_Config();
    RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIODEN;
    motor_init();
    motor_set_position(20, 20, 8, 8);
    motor_set_position(PI*WHEEL_DISTANCE/2, -PI*WHEEL_DISTANCE/2, 8, -8);
    motor_set_position(20, 20, 8, 8);
}

