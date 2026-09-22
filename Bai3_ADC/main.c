#include <stdint.h>

// RCC
#define RCC_BASE        0x40021000
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

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

// ADC1
#define ADC1_BASE       0x40012400
#define ADC1_SR         (*(volatile uint32_t *)(ADC1_BASE + 0x00))
#define ADC1_CR2        (*(volatile uint32_t *)(ADC1_BASE + 0x08))
#define ADC1_SMPR2      (*(volatile uint32_t *)(ADC1_BASE + 0x10))
#define ADC1_SQR1       (*(volatile uint32_t *)(ADC1_BASE + 0x2C))
#define ADC1_SQR3       (*(volatile uint32_t *)(ADC1_BASE + 0x34))
#define ADC1_DR         (*(volatile uint32_t *)(ADC1_BASE + 0x4C))

// SysTick
#define SYST_CSR        (*(volatile uint32_t *)0xE000E010)
#define SYST_RVR        (*(volatile uint32_t *)0xE000E014)
#define SYST_CVR        (*(volatile uint32_t *)0xE000E018)

volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

void delay_ms(uint32_t ms) {
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms);
}

void systick_init(void) {
    SYST_RVR = 8000 - 1; // 1ms với HSI 8MHz
    SYST_CVR = 0;
    SYST_CSR = (1 << 0) | (1 << 1) | (1 << 2);
}

void uart1_init(void) {
    RCC_APB2ENR |= (1 << 2) | (1 << 14); // Clock GPIOA và USART1

    // PA9 (TX): Alternate function push-pull 50MHz (0x0B)
    GPIOA_CRH &= ~(0x0F << 4);
    GPIOA_CRH |= (0x0B << 4);

    USART1_BRR = 0x45; // 115200 baud với xung 8MHz
    USART1_CR1 = (1 << 13) | (1 << 3); // Enable USART1, Enable Transmitter
}

void uart1_send_char(char c) {
    while (!(USART1_SR & (1 << 7)));
    USART1_DR = c;
}

void uart1_send_str(const char *s) {
    while (*s) {
        uart1_send_char(*s++);
    }
}

void adc1_init(void) {
    RCC_APB2ENR |= (1 << 9) | (1 << 2); // Clock ADC1 và GPIOA

    // PA0: Analog Input (MODE=00, CNF=00)
    GPIOA_CRL &= ~(0x0F << 0);

    // Thời gian lấy mẫu kênh 0: 55.5 cycles (SMPR2 bit 0-2 = 101b -> 0x5)
    ADC1_SMPR2 |= (0x5 << 0);

    // 1 kênh chuyển đổi trong SQR1, kênh đó là ADC Channel 0 trong SQR3
    ADC1_SQR1 &= ~(0x0F << 20);
    ADC1_SQR3 = 0;

    // Đánh thức ADC1
    ADC1_CR2 |= (1 << 0);
    for (volatile int i = 0; i < 10000; i++);

    // Hiệu chuẩn ADC
    ADC1_CR2 |= (1 << 2);
    while (ADC1_CR2 & (1 << 2));
}

uint16_t adc1_read(void) {
    ADC1_CR2 |= (1 << 0); // Kích hoạt chuyển đổi
    while (!(ADC1_SR & (1 << 1))); // Chờ cờ EOC
    return (uint16_t)(ADC1_DR & 0xFFF);
}

int main(void) {
    systick_init();
    uart1_init();
    adc1_init();

    while (1) {
        uint16_t raw_adc = adc1_read();

        // Tính điện áp mV: V = (ADC * 3300) / 4095
        uint32_t voltage_mv = ((uint32_t)raw_adc * 3300) / 4095;
        uint32_t volt_int = voltage_mv / 1000;
        uint32_t volt_dec = voltage_mv % 1000;

        uart1_send_str("<HTN-N01><N15>: ADC = ");

        // In raw ADC
        char buf[8];
        int idx = 0;
        uint16_t tmp = raw_adc;
        if (tmp == 0) {
            uart1_send_char('0');
        } else {
            while (tmp > 0) {
                buf[idx++] = (tmp % 10) + '0';
                tmp /= 10;
            }
            while (idx > 0) {
                uart1_send_char(buf[--idx]);
            }
        }

        // In điện áp dạng X.XXX V
        uart1_send_str(" | Voltage = ");
        uart1_send_char(volt_int + '0');
        uart1_send_char('.');
        uart1_send_char((volt_dec / 100) + '0');
        uart1_send_char(((volt_dec % 100) / 10) + '0');
        uart1_send_char((volt_dec % 10) + '0');
        uart1_send_str(" V\r\n");

        delay_ms(1000); // Gửi mỗi 1 giây
    }
}
