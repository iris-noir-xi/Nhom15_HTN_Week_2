#include "gpio.h"
#include "stm32f103.h"

void led_gpio_init(void) {
    // Bật clock GPIOA
    RCC_APB2ENR |= (1 << 2);

    // Cấu hình PA0, PA1, PA2: General purpose output push-pull, 10MHz (0x1)
    GPIOA_CRL &= ~0x00000FFF;
    GPIOA_CRL |=  0x00000111;
}

void led_toggle(uint8_t pin) {
    GPIOA_ODR ^= (1 << pin);
}
