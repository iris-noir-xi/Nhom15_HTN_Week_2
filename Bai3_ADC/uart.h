#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart1_init(void);
void uart1_send_char(char c);
void uart1_send_string(const char *str);
void uart1_send_uint(uint32_t val);

#endif
