#include <stm32f4xx.h>
#include "gpio.h"

#define PRESCALER_TIM7      8400        // timer frequency: 10kHz
#define COUNTER_MAX_TIM7    10000       // timer max counter -> 1Hz

#define TIMER_CLOCK         84000000    // APB1 clock
#define PRESCALER_TIM4      8400        // timer frequency: 10kHz
#define COUNTER_MAX_TIM4    100       // timer max counter -> 100Hz

void timer7_start(void)
{
    // Enable TIM7 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    // Enable TIM7 interrupt vector
    NVIC_EnableIRQ(TIM7_IRQn);

    // Configure TIM7
    TIM7->PSC = PRESCALER_TIM7 - 1;      // Note: final timer clock  = timer clock / (prescaler + 1)
    TIM7->ARR = COUNTER_MAX_TIM7 - 1;    // Note: timer reload takes 1 cycle, thus -1
    TIM7->DIER |= TIM_DIER_UIE;          // Enable update interrupt
    TIM7->CR1 |= TIM_CR1_CEN;            // Enable timer
}

void timer4_start(void)
{
    // Enable TIM4 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // Enable TIM4 interrupt vector
    // Configure TIM4
    TIM4->PSC = PRESCALER_TIM4 - 1;      // Note: final timer clock  = timer clock / (prescaler + 1)
    TIM4->ARR = COUNTER_MAX_TIM4 - 1;    // Note: timer reload takes 1 cycle, thus -1
    TIM4->DIER |= TIM_DIER_UIE;          // Enable update interrupt
    TIM4->CR1 |= TIM_CR1_CEN;            // Enable timer

	// Enable PWM mode 1 (110)

	TIM4->CCMR2 |=  (TIM_CCMR2_OC3M & ~TIM_CCMR2_OC3M_0);

    // 
	TIM4->CCR3 = 50;


    // Enable output circuit (CC3E)

	TIM4->CCER |= TIM_CCER_CC3E;
}

void TIM4_IRQHandler(void){
	gpio_toggle(GPIOD, 14);
    TIM4->SR &= ~TIM_SR_UIF;
    TIM4->SR;
}

/*
*   Commented because used for the motors
*/

// // Timer 7 Interrupt Service Routine
// void TIM7_IRQHandler(void)
// {
	/*
	*
	*   BEWARE !!
	*   Based on STM32F40x and STM32F41x Errata sheet - 2.1.13 Delay after an RCC peripheral clock enabling
	*
	*   As there can be a delay between the instruction of clearing of the IF (Interrupt Flag) of corresponding register (named here CR) and
	*   the effective peripheral IF clearing bit there is a risk to enter again in the interrupt if the clearing is done at the end of ISR.
	*
	*   As tested, only the workaround 3 is working well, then read back of CR must be done before leaving the ISR
	*
	*/

//     /* do something ... */
//     gpio_toggle(BODY_LED);

//     // Clear interrupt flag
//     TIM7->SR &= ~TIM_SR_UIF;
//     TIM7->SR;	// Read back in order to ensure the effective IF clearing
// }
