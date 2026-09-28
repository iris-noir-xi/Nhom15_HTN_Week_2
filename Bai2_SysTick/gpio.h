#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

void led_gpio_init(void);
void led_toggle(uint8_t pin);

#endif
