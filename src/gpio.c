#include <stm32f407xx.h>
#include <gpio.h>
#include <main.h>

void gpio_config_output_opendrain(GPIO_TypeDef *port, unsigned int pin)
{
    // Output type open-drain : OTy = 1
    port->OTYPER |= (1 << pin);

    // Output data low : ODRy = 0
    port->ODR &= ~(0 << pin);

    // Floating, no pull-up/down : PUPDRy = 00
    port->PUPDR &= ~(3 << (pin * 2));

    // Output speed highest : OSPEEDRy = 11
    port->OSPEEDR |= (3 << (pin * 2));

    // Output mode : MODERy = 01
    port->MODER = (port->MODER & ~(3 << (pin * 2))) | (1 << (pin * 2));
}

void gpio_config_output_h_pushpull(GPIO_TypeDef *port, unsigned int pin)
{
    // Output type open-drain : OTy = 1
    port->OTYPER |= (0 << pin);

    // Output data low : ODRy = 0
    port->ODR &= ~(0 << pin);

    // Floating, no pull-up/down : PUPDRy = 00
    port->PUPDR &= ~(3 << (pin * 2));

    // Output speed highest : OSPEEDRy = 11
    port->OSPEEDR |= (3 << (pin * 2));

    // Output mode : MODERy = 01
    port->MODER = (port->MODER & ~(3 << (pin * 2))) | (1 << (pin * 2));
}

void gpio_set(GPIO_TypeDef *port, unsigned int pin)
{
    port->BSRR = (1 << pin);
}

void gpio_clear(GPIO_TypeDef *port, unsigned int pin)
{
    port->BSRR = (1 << (pin + 16));
}

void gpio_toggle(GPIO_TypeDef *port, unsigned int pin)
{
    if (port->ODR & (1<<pin)) {
        gpio_clear(port, pin);
    } else {
        gpio_set(port, pin);
    }
}


void setup_selector(void){
    
    GPIOC->MODER &= ~(3 << (13 * 2));
    GPIOC->PUPDR &= ~(1 << (13 * 2));
    GPIOC->PUPDR |= (1 << (13*2 + 1));
    
    GPIOC->MODER &= ~(3 << (14 * 2));
    GPIOC->PUPDR &= ~(1 << (14 * 2));
    GPIOC->PUPDR |= (1 << (14*2 + 1));
    
    GPIOC->MODER &= ~(3 << (15 * 2));
    GPIOC->PUPDR &= ~(1 << (15 * 2));
    GPIOC->PUPDR |= (1 << (15*2 + 1));
    
    GPIOD->MODER &= ~(3 << (4 * 2));
    GPIOD->PUPDR &= ~(1 << (4 * 2));
    GPIOD->PUPDR |= (1 << (4*2 + 1));

}

int get_selector(void){
    int num0,num1,num2,num3,result;
    num0 = ((GPIOC->IDR & (1 << 13)) >> 13);
    num1 = ((GPIOC->IDR & (1 << 14)) >> 14);
    num2 = ((GPIOC->IDR & (1 << 15)) >> 15);
    num3 = ((GPIOD->IDR & (1 << 4)) >> 4);

    result = 8*num3+4*num2+2*num1+num0;
    return result;

}


