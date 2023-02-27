#include <stm32f4xx.h>
#include <system_clock_config.h>
#include <gpio.h>
#include <main.h>
#include <time.h>



// Init function required by __libc_init_array
void _init(void) {}

int main(void)
{
    SystemClock_Config();

    // Enable GPIOD peripheral clock
    RCC->AHB1ENR    |= RCC_AHB1ENR_GPIODEN;
    
    RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOCEN;

    // LED used init
    gpio_config_output_opendrain(LED7);
    timer7_start();

    while (1)
    {
        asm("nop");
    }
    


    // Part 6.4:
    // gpio_config_output_opendrain(LED3);
    // gpio_config_output_opendrain(LED5);
    // gpio_config_output_opendrain(LED7);
    // setup_selector();

    // int num;
    // num = 0b0;
    // while (1) {
    //     num = get_selector();
    //     for (int i = 0; i < 1600000; ++i){
    //         asm("nop");
    //     }

    //     if(num < 8){
    //         gpio_set(LED1);
    //         for (int i = 0; i < 1600000; ++i){
    //         asm("nop");}
    //         gpio_set(LED3);
    //         for (int i = 0; i < 1600000; ++i){
    //         asm("nop");}
    //         gpio_set(LED5);
    //         for (int i = 0; i < 1600000; ++i){
    //         asm("nop");}
    //         gpio_set(LED7);
    //         for (int i = 0; i < 1600000; ++i){
    //         asm("nop");}
    //         gpio_clear(LED1);
    //         gpio_clear(LED3);
    //         gpio_clear(LED5);
    //         gpio_clear(LED7);

    //     }
    //     else{
    //         gpio_set(LED1);
    //         for (int i = 0; i < 1600000; ++i){
    //         asm("nop");}
    //         gpio_clear(LED1);

    //         gpio_set(LED3);
    //         for (int i = 0; i < 1600000; ++i){
    //         asm("nop");}
    //         gpio_clear(LED3);

    //         gpio_set(LED5);
    //         for (int i = 0; i < 1600000; ++i){
    //         asm("nop");}
    //         gpio_clear(LED5);

    //         gpio_set(LED7);
    //         for (int i = 0; i < 1600000; ++i){
    //         asm("nop");}
    //         gpio_clear(LED7);

    //     }

    //     }
        
    }

