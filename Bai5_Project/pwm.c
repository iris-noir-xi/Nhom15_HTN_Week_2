#include "pwm.h"
#include "stm32f103.h"

void pwm_tim2_ch1_init(void) {
    RCC_APB2ENR |= (1 << 2); // Bật Clock GPIOA
    RCC_APB1ENR |= (1 << 0); // Bật Clock TIM2

    // PA0: Alternate function output Push-Pull (0xB)
    GPIOA_CRL &= ~(0xF << 0);
    GPIOA_CRL |=  (0xB << 0);

    TIM2_PSC = 7;
    TIM2_ARR = 999;
    TIM2_CCR1 = 0; // Mặc định tắt (0%)

    TIM2_CCMR1 = (0x68 << 0); // PWM mode 1, preload enable CH1
    TIM2_CCER |= (1 << 0);    // CC1E bật ngõ ra
    TIM2_CR1  |= (1 << 7) | (1 << 0); // ARPE, CEN
}

void pwm_set_duty(uint8_t duty_percent) {
    if (duty_percent > 100) duty_percent = 100;
    TIM2_CCR1 = (duty_percent * 1000) / 100;
}
