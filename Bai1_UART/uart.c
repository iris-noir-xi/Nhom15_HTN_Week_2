#include "uart.h"
#include "stm32f103.h"

void uart1_init(void) {
    RCC_APB2ENR |= (1 << 2) | (1 << 14);

    GPIOA_CRH &= ~((0xF << 4) | (0xF << 8));
    GPIOA_CRH |= (0xB << 4) | (0x4 << 8);

    USART1_BRR = 0x45;
    USART1_CR1 = (1 << 13) | (1 << 3) | (1 << 2) | (1 << 5);

    NVIC_ISER1 |= (1 << (37 - 32));
}

void uart1_send_char(char c) {
    while (!(USART1_SR & (1 << 7)));
    USART1_DR = (uint8_t)c;
}

void uart1_send_string(const char *str) {
    while (*str) {
        uart1_send_char(*str++);
    }
}
