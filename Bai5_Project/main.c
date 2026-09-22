#include <stdint.h>
#include <stdbool.h>

// RCC
#define RCC_BASE        0x40021000
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))

// GPIOA
#define GPIOA_BASE      0x40010800
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_CRH       (*(volatile uint32_t *)(GPIOA_BASE + 0x04))

// USART1
#define USART1_BASE     0x40013800
#define USART1_SR       (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR       (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR      (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1      (*(volatile uint32_t *)(USART1_BASE + 0x0C))

// TIM2
#define TIM2_BASE       0x40000000
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_EGR        (*(volatile uint32_t *)(TIM2_BASE + 0x14))
#define TIM2_CCMR1      (*(volatile uint32_t *)(TIM2_BASE + 0x18))
#define TIM2_CCER       (*(volatile uint32_t *)(TIM2_BASE + 0x20))
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_CCR1       (*(volatile uint32_t *)(TIM2_BASE + 0x34))

// NVIC ISER1 (IRQ 37: USART1)
#define NVIC_ISER1      (*(volatile uint32_t *)0xE000E104)

volatile uint8_t led_state = 0;        // 0: OFF, 1: ON
volatile uint8_t configured_duty = 50;  // Mặc định 50%

#define RX_BUF_SIZE 64
volatile char rx_buf[RX_BUF_SIZE];
volatile uint8_t rx_idx = 0;
volatile bool cmd_ready = false;

void uart1_send_char(char c) {
    while (!(USART1_SR & (1 << 7)));
    USART1_DR = c;
}

void uart1_send_str(const char *s) {
    while (*s) {
        uart1_send_char(*s++);
    }
}

void uart1_send_num(uint32_t n) {
    char buf[10];
    int i = 0;
    if (n == 0) {
        uart1_send_char('0');
        return;
    }
    while (n > 0) {
        buf[i++] = (n % 10) + '0';
        n /= 10;
    }
    while (i > 0) {
        uart1_send_char(buf[--i]);
    }
}

void pwm_set_duty(uint8_t duty) {
    if (duty > 100) duty = 100;
    TIM2_CCR1 = duty * 10; // ARR = 999 -> CCR1 = 1000 * duty / 100
}

void pwm_tim2_init(void) {
    RCC_APB2ENR |= (1 << 2);
    RCC_APB1ENR |= (1 << 0);

    // PA0: Alternate Function Output Push-Pull 50MHz (0x0B)
    GPIOA_CRL &= ~(0x0F << 0);
    GPIOA_CRL |=  (0x0B << 0);

    TIM2_PSC = 8 - 1;   // 1MHz
    TIM2_ARR = 1000 - 1; // 1kHz

    TIM2_CCMR1 |= (0x68 << 0); // PWM mode 1 + Preload enable
    TIM2_CCER  |= (1 << 0);      // Bật output CH1
    TIM2_CCR1   = 0;             // Ban đầu tắt

    TIM2_EGR   |= (1 << 0);
    TIM2_CR1   |= (1 << 7) | (1 << 0);
}

void uart1_init(void) {
    RCC_APB2ENR |= (1 << 14);

    // PA9 (TX): Alternate Function Output Push-Pull 50MHz (0x0B)
    GPIOA_CRH &= ~(0x0F << 4);
    GPIOA_CRH |=  (0x0B << 4);

    // PA10 (RX): Input Floating (0x04)
    GPIOA_CRH &= ~(0x0F << 8);
    GPIOA_CRH |=  (0x04 << 8);

    USART1_BRR = 0x45; // 115200 baud với 8MHz

    // Bật USART1, TE, RE, RXNEIE
    USART1_CR1 = (1 << 13) | (1 << 3) | (1 << 2) | (1 << 5);

    // Kích hoạt ngắt IRQ 37 trên NVIC
    NVIC_ISER1 = (1 << (37 - 32));
}

void USART1_IRQHandler(void) {
    if (USART1_SR & (1 << 5)) {
        char ch = USART1_DR;
        if (!cmd_ready) {
            if (ch == '!') {
                rx_buf[rx_idx] = '\0';
                cmd_ready = true;
            } else if (rx_idx < RX_BUF_SIZE - 1) {
                if (ch != '\r' && ch != '\n') {
                    rx_buf[rx_idx++] = ch;
                }
            }
        }
    }
}

int str_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return (*a == *b);
}

void process_command(void) {
    if (!cmd_ready) return;

    if (str_equal((char *)rx_buf, "ON")) {
        led_state = 1;
        pwm_set_duty(configured_duty);
        uart1_send_str("<HTN-N01><N15>: LED ON (Duty: ");
        uart1_send_num(configured_duty);
        uart1_send_str("%)\r\n");
    }
    else if (str_equal((char *)rx_buf, "OFF")) {
        led_state = 0;
        pwm_set_duty(0);
        uart1_send_str("<HTN-N01><N15>: LED OFF\r\n");
    }
    else if (str_equal((char *)rx_buf, "Status")) {
        uart1_send_str("<HTN-N01><N15>: Status -> State: ");
        if (led_state) {
            uart1_send_str("ON | Current Duty: ");
            uart1_send_num(configured_duty);
            uart1_send_str("%\r\n");
        } else {
            uart1_send_str("OFF | Configured Duty: ");
            uart1_send_num(configured_duty);
            uart1_send_str("%\r\n");
        }
    }
    else if (rx_buf[0] == 'P' && rx_buf[1] == 'W' && rx_buf[2] == 'M' && rx_buf[3] == ':') {
        uint32_t val = 0;
        int i = 4;
        bool valid = false;

        while (rx_buf[i] >= '0' && rx_buf[i] <= '9') {
            val = val * 10 + (rx_buf[i] - '0');
            i++;
            valid = true;
        }

        if (valid && rx_buf[i] == '%') {
            if (val > 100) val = 100;
            configured_duty = (uint8_t)val;

            if (led_state == 1) {
                pwm_set_duty(configured_duty);
                uart1_send_str("<HTN-N01><N15>: PWM set to ");
                uart1_send_num(configured_duty);
                uart1_send_str("%\r\n");
            } else {
                uart1_send_str("<HTN-N01><N15>: PWM saved ");
                uart1_send_num(configured_duty);
                uart1_send_str("% (LED is OFF)\r\n");
            }
        } else {
            uart1_send_str("<HTN-N01><N15>: Syntax Error! Format: PWM:xx%!\r\n");
        }
    }
    else {
        uart1_send_str("<HTN-N01><N15>: Unknown Command!\r\n");
    }

    rx_idx = 0;
    cmd_ready = false;
}

int main(void) {
    pwm_tim2_init();
    uart1_init();

    while (1) {
        process_command();
    }
}
