#include "uart.h"
#include "stm32f103.h"

void uart1_init(void) {
    RCC_APB2ENR |= (1 << 2) | (1 << 14); // Bật Clock GPIOA, USART1

    // PA9 (TX): Alternate function push-pull (0xB)
    // PA10 (RX): Input floating (0x4)
    GPIOA_CRH &= ~((0xF << 4) | (0xF << 8));
    GPIOA_CRH |= (0xB << 4) | (0x4 << 8);

    USART1_BRR = 0x45; // 115200 baud với 8MHz
    USART1_CR1 = (1 << 13) | (1 << 3) | (1 << 2); // UE, TE, RE
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

void uart1_send_uint(uint32_t val) {
    char buf[11];
    int i = 0;
    if (val == 0) {
        uart1_send_char('0');
        return;
    }
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (i > 0) {
        uart1_send_char(buf[--i]);
    }
}
