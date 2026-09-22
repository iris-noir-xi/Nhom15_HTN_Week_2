#include <stdint.h>

#define RCC_BASE        0x40021000
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define GPIOA_BASE      0x40010800
#define GPIOA_CRH       (*(volatile uint32_t *)(GPIOA_BASE + 0x04))

#define USART1_BASE     0x40013800
#define USART1_SR       (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR       (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR      (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1      (*(volatile uint32_t *)(USART1_BASE + 0x0C))

#define NVIC_ISER1      (*(volatile uint32_t *)(0xE000E104))

#define RX_BUFFER_SIZE  128

volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_index = 0;
volatile uint8_t msg_ready = 0;

void USART1_IRQHandler(void) {
    if (USART1_SR & (1 << 5)) { // RXNE
        char c = (char)(USART1_DR & 0xFF);
        if (c == '!') {
            rx_buffer[rx_index] = '\0';
            msg_ready = 1;
        } else {
            if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = c;
            } else {
                rx_index = 0;
            }
        }
    }
}

void uart1_send_char(char c) {
    while (!(USART1_SR & (1 << 7)));
    USART1_DR = c;
}

void uart1_send_str(const char *str) {
    while (*str) {
        uart1_send_char(*str++);
    }
}

void uart1_init(void) {
    RCC_APB2ENR |= (1 << 2) | (1 << 14);

    GPIOA_CRH &= ~(0xFF << 4);
    GPIOA_CRH |= (0x0B << 4) | (0x04 << 8);

    USART1_BRR = 0x45; // 115200 với xung HSI 8MHz

    USART1_CR1 |= (1 << 13) | (1 << 3) | (1 << 2) | (1 << 5);

    NVIC_ISER1 |= (1 << (37 - 32));
}

int main(void) {
    uart1_init();

    // Mã lớp và mã nhóm mới:
    const char header[] = "\r\n<HTN-N01><N15>: ";

    while (1) {
        if (msg_ready) {
            uart1_send_str(header);
            uart1_send_str((const char *)rx_buffer);
            uart1_send_str("\r\n");

            rx_index = 0;
            msg_ready = 0;
        }
    }
}
