#ifndef STM32F103_H
#define STM32F103_H

#include <stdint.h>

#define RCC_BASE        0x40021000
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define GPIOA_BASE      0x40010800
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))

#define TIM2_BASE       0x40000000
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_CCMR1      (*(volatile uint32_t *)(TIM2_BASE + 0x18))
#define TIM2_CCMR2      (*(volatile uint32_t *)(TIM2_BASE + 0x1C))
#define TIM2_CCER       (*(volatile uint32_t *)(TIM2_BASE + 0x20))
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_CCR1       (*(volatile uint32_t *)(TIM2_BASE + 0x34))
#define TIM2_CCR2       (*(volatile uint32_t *)(TIM2_BASE + 0x38))
#define TIM2_CCR3       (*(volatile uint32_t *)(TIM2_BASE + 0x3C))
#define TIM2_CCR4       (*(volatile uint32_t *)(TIM2_BASE + 0x40))

#endif
