#include <stdint.h>

// Thanh ghi RCC và GPIOA
#define RCC_BASE        0x40021000
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define GPIOA_BASE      0x40010800
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

// Thanh ghi SysTick (Core Cortex-M3)
#define SYST_CSR        (*(volatile uint32_t *)0xE000E010)
#define SYST_RVR        (*(volatile uint32_t *)0xE000E014)
#define SYST_CVR        (*(volatile uint32_t *)0xE000E018)

// Biến đếm thời gian
volatile uint32_t count_led1 = 0;
volatile uint32_t count_led2 = 0;
volatile uint32_t count_led3 = 0;

// Ngắt SysTick định kỳ mỗi 1ms
void SysTick_Handler(void) {
    // LED 3 (PA2): 10Hz -> chu kỳ 100ms -> toggle mỗi 50ms
    count_led3++;
    if (count_led3 >= 50) {
        GPIOA_ODR ^= (1 << 2);
        count_led3 = 0;
    }

    // LED 2 (PA1): 1Hz -> chu kỳ 1000ms -> toggle mỗi 500ms
    count_led2++;
    if (count_led2 >= 500) {
        GPIOA_ODR ^= (1 << 1);
        count_led2 = 0;
    }

    // LED 1 (PA0): 0.1Hz -> chu kỳ 10000ms -> toggle mỗi 5000ms
    count_led1++;
    if (count_led1 >= 5000) {
        GPIOA_ODR ^= (1 << 0);
        count_led1 = 0;
    }
}

void gpio_init(void) {
    // Cấp clock cho Port A
    RCC_APB2ENR |= (1 << 2);

    // Cấu hình PA0, PA1, PA2 làm Output Push-Pull 50MHz
    GPIOA_CRL &= ~(0xFFF << 0);
    GPIOA_CRL |= (0x333 << 0);

    // Ban đầu tắt cả 3 LED
    GPIOA_ODR &= ~(0x7 << 0);
}

void systick_init(void) {
    // HSI mặc định 8MHz -> 1ms tương ứng 8000 tick (RVR = 7999)
    SYST_RVR = 8000 - 1;
    SYST_CVR = 0;

    // Bật SysTick: Bit 0 (ENABLE), Bit 1 (TICKINT), Bit 2 (CLKSOURCE)
    SYST_CSR = (1 << 0) | (1 << 1) | (1 << 2);
}

int main(void) {
    gpio_init();
    systick_init();

    while (1) {
        // Xử lý hoàn toàn trong ngắt SysTick_Handler
    }
}
