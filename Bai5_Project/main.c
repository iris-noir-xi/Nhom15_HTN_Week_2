#include <stdint.h>
#include "stm32f103.h"
#include "uart.h"
#include "pwm.h"

#define BUF_SIZE 64
volatile char rx_buf[BUF_SIZE];
volatile uint8_t rx_idx = 0;
volatile uint8_t msg_ready = 0;

static uint8_t is_on = 0;
static uint8_t current_duty = 0;

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

static uint8_t str_equal(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (*s1 == *s2);
}

int main(void) {
    uart1_init();
    pwm_tim2_ch1_init();

    while (1) {
        if (msg_ready) {
            char *cmd = (char *)rx_buf;

            if (str_equal(cmd, "Status")) {
                uart1_send_string("<HTN-N01><N15>: State=");
                uart1_send_string(is_on ? "ON" : "OFF");
                uart1_send_string(", Duty=");
                uart1_send_uint(current_duty);
                uart1_send_string("%\r\n");
            } 
            else if (str_equal(cmd, "ON")) {
                is_on = 1;
                if (current_duty == 0) current_duty = 100;
                pwm_set_duty(current_duty);
                uart1_send_string("<HTN-N01><N15>: LED Turned ON\r\n");
            } 
            else if (str_equal(cmd, "OFF")) {
                is_on = 0;
                pwm_set_duty(0);
                uart1_send_string("<HTN-N01><N15>: LED Turned OFF\r\n");
            } 
            else if (cmd[0] == 'P' && cmd[1] == 'W' && cmd[2] == 'M' && cmd[3] == ':') {
                uint8_t val = 0;
                int i = 4;
                while (cmd[i] >= '0' && cmd[i] <= '9') {
                    val = val * 10 + (cmd[i] - '0');
                    i++;
                }
                if (cmd[i] == '%') {
                    if (val > 100) val = 100;
                    current_duty = val;
                    if (is_on) {
                        pwm_set_duty(current_duty);
                    }
                    uart1_send_string("<HTN-N01><N15>: Duty set to ");
                    uart1_send_uint(current_duty);
                    uart1_send_string("%\r\n");
                }
            }

            rx_idx = 0;
            msg_ready = 0;
        }
    }

    return 0;
}
