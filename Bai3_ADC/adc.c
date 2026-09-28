#include "adc.h"
#include "stm32f103.h"

void adc1_init(void) {
    RCC_APB2ENR |= (1 << 2) | (1 << 9); // Bật Clock GPIOA, ADC1

    // PA0: Analog input (0x0)
    GPIOA_CRL &= ~(0xF << 0);

    ADC1_CR2 |= (1 << 0); // Bật ADON lần 1 để khởi động ADC
    for (volatile int i = 0; i < 1000; i++); // Chờ ổn định

    // Hiệu chuẩn ADC
    ADC1_CR2 |= (1 << 2); // RSTCAL
    while (ADC1_CR2 & (1 << 2));
    ADC1_CR2 |= (1 << 3); // CAL
    while (ADC1_CR2 & (1 << 3));

    ADC1_SQR1 &= ~(0xF << 20); // 1 chuyển đổi duy nhất (L = 0)
    ADC1_SQR3 = 0;             // Đọc kênh 0 (PA0)
}

uint16_t adc1_read(void) {
    ADC1_CR2 |= (1 << 0); // Kích hoạt chuyển đổi (ADON lần 2)
    while (!(ADC1_SR & (1 << 1))); // Chờ cờ EOC
    return (uint16_t)ADC1_DR;
}
