#include "pwm.h"
#include "stm32f103.h"

void pwm_tim2_init(void) {
    // Bật Clock cho GPIOA (bit 2 APB2) và TIM2 (bit 0 APB1)
    RCC_APB2ENR |= (1 << 2);
    RCC_APB1ENR |= (1 << 0);

    // PA0, PA1, PA2, PA3: Alternate Function Push-Pull, max speed 10MHz (0x9 hoặc 0xB)
    // Mode 0xB = 1011 (AF Output Push-Pull 50MHz)
    GPIOA_CRL &= ~0x0000FFFF;
    GPIOA_CRL |=  0x0000BBBB;

    // Cấu hình tần số PWM 1kHz: HSI 8MHz / (7 + 1) = 1MHz -> ARR = 1000 - 1
    TIM2_PSC = 7;
    TIM2_ARR = 999;

    // Đặt duty cycle ban đầu:
    // Kênh 1 (PA0): 10% -> CCR1 = 100
    // Kênh 2 (PA1): 30% -> CCR2 = 300
    // Kênh 3 (PA2): 50% -> CCR3 = 500
    // Kênh 4 (PA3): 70% -> CCR4 = 700
    TIM2_CCR1 = 100;
    TIM2_CCR2 = 300;
    TIM2_CCR3 = 500;
    TIM2_CCR4 = 700;

    // Cấu hình PWM mode 1 (OCxM = 110) và bật preload (OCxPE = 1) cho cả 4 kênh
    TIM2_CCMR1 = (0x68 << 0) | (0x68 << 8); // CH1 & CH2
    TIM2_CCMR2 = (0x68 << 0) | (0x68 << 8); // CH3 & CH4

    // Kích hoạt ngõ ra cho cả 4 kênh (CC1E, CC2E, CC3E, CC4E)
    TIM2_CCER = (1 << 0) | (1 << 4) | (1 << 8) | (1 << 12);

    // Bật Counter (CEN) và auto-reload preload (ARPE)
    TIM2_CR1 |= (1 << 7) | (1 << 0);
}
