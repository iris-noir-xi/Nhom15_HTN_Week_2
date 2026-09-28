#include <stdint.h>
#include "stm32f103.h"
#include "uart.h"
#include "adc.h"

static void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 800; j++);
    }
}

int main(void) {
    uart1_init();
    adc1_init();

    while (1) {
        uint16_t raw = adc1_read();
        uint32_t mv = (raw * 3300) / 4095;
        uint32_t v_int = mv / 1000;
        uint32_t v_dec = (mv % 1000) / 10;

        uart1_send_string("<HTN-N01><N15>: ADC = ");
        uart1_send_uint(raw);
        uart1_send_string(" | Voltage = ");
        uart1_send_uint(v_int);
        uart1_send_char('.');
        if (v_dec < 10) uart1_send_char('0');
        uart1_send_uint(v_dec);
        uart1_send_string(" V\r\n");

        delay_ms(1000);
    }

    return 0;
}
