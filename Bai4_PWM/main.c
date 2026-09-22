#include <stdint.h>

// Thanh ghi RCC
#define RCC_BASE        0x40021000
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))

// Thanh ghi GPIOA
#define GPIOA_BASE      0x40010800
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))

// Thanh ghi TIM2
#define TIM2_BASE       0x40000000
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_EGR        (*(volatile uint32_t *)(TIM2_BASE + 0x14))
#define TIM2_CCMR1      (*(volatile uint32_t *)(TIM2_BASE + 0x18))
#define TIM2_CCMR2      (*(volatile uint32_t *)(TIM2_BASE + 0x1C))
#define TIM2_CCER       (*(volatile uint32_t *)(TIM2_BASE + 0x20))
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_CCR1       (*(volatile uint32_t *)(TIM2_BASE + 0x34))
#define TIM2_CCR2       (*(volatile uint32_t *)(TIM2_BASE + 0x38))
#define TIM2_CCR3       (*(volatile uint32_t *)(TIM2_BASE + 0x3C))
#define TIM2_CCR4       (*(volatile uint32_t *)(TIM2_BASE + 0x40))

void pwm_tim2_init(void) {
    // 1. Cấp xung clock cho GPIOA và TIM2
    RCC_APB2ENR |= (1 << 2);
    RCC_APB1ENR |= (1 << 0);

    // 2. PA0 -> PA3: Alternate Function Output Push-Pull 50MHz (MODE=11b, CNF=10b -> 0xB)
    GPIOA_CRL &= ~0x0000FFFF;
    GPIOA_CRL |=  0x0000BBBB;

    // 3. Cài đặt tần số PWM = 1 kHz:
    // HSI mặc định = 8 MHz -> PSC = 7 (xung đếm 1 MHz)
    // ARR = 999 -> Chu kỳ 1000 tick = 1 ms (1 kHz)
    TIM2_PSC = 8 - 1;
    TIM2_ARR = 1000 - 1;

    // 4. Cấu hình PWM Mode 1 (110b) và bật Output Preload cho cả 4 kênh
    TIM2_CCMR1 = (0x68 << 0) | (0x68 << 8); // CH1, CH2
    TIM2_CCMR2 = (0x68 << 0) | (0x68 << 8); // CH3, CH4

    // 5. Cài đặt giá trị độ rộng xung (Duty Cycle)
    TIM2_CCR1 = 100; // 10%
    TIM2_CCR2 = 300; // 30%
    TIM2_CCR3 = 500; // 50%
    TIM2_CCR4 = 700; // 70%

    // 6. Bật ngõ ra tín hiệu trên 4 kênh (CC1E, CC2E, CC3E, CC4E)
    TIM2_CCER = (1 << 0) | (1 << 4) | (1 << 8) | (1 << 12);

    // 7. Tạo sự kiện cập nhật (UG) để load ngay ARR và CCR
    TIM2_EGR |= (1 << 0);

    // 8. Bật bộ đếm Timer (CEN = 1) và bật Auto-reload preload (ARPE = 1)
    TIM2_CR1 |= (1 << 7) | (1 << 0);
}

int main(void) {
    pwm_tim2_init();

    while (1) {
        // Chạy PWM bằng phần cứng Timer
    }
}
