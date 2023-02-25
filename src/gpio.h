#ifndef GPIO_H
#define GPIO_H

#include <stm32f407xx.h>

void gpio_config_output_opendrain(GPIO_TypeDef *port, unsigned int pin);
void gpio_config_output_h_pushpull(GPIO_TypeDef *port, unsigned int pin);
void gpio_set(GPIO_TypeDef *port, unsigned int pin);
void gpio_clear(GPIO_TypeDef *port, unsigned int pin);
void gpio_toggle(GPIO_TypeDef *port, unsigned int pin);
void gpio_config_input_opendrain(GPIO_TypeDef *port, unsigned int pin);
void setup_selector();
int get_selector();
#endif /* GPIO_H */
