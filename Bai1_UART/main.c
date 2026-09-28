#include <stdint.h>
#include "stm32f103.h"
#include "uart.h"

#define BUF_SIZE 128
volatile char rx_buf[BUF_SIZE];
volatile uint8_t rx_idx = 0;
volatile uint8_t msg_ready = 0;

void USART1_IRQHandler(void) {
    if (USART1_SR & (1 << 5)) {
        char c = (char)(USART1_DR & 0xFF);
        if (c == '!') {
            rx_buf[rx_idx] = '\0';
            msg_ready = 1;
        } else if (c != '\r' && c != '\n') {
            if (rx_idx < BUF_SIZE - 1) {
                rx_buf[rx_idx++] = c;
            }
        }
    }
}

int main(void) {
    uart1_init();

    while (1) {
        if (msg_ready) {
            uart1_send_string("<HTN-N01><N15>: ");
            uart1_send_string((char *)rx_buf);
            uart1_send_string("\r\n");

            rx_idx = 0;
            msg_ready = 0;
        }
    }
    return 0;
}
