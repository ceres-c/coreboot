#ifndef CONSOLE_UART8250MEM_H
#define CONSOLE_UART8250MEM_H

#include <stdint.h>

#if CONFIG(DRIVERS_UART_8250MEM_32)
uint8_t uart8250_read(void *base, uint8_t reg);
void uart8250_write(void *base, uint8_t reg, uint8_t data);
#else
uint8_t uart8250_read(void *base, uint8_t reg);
void uart8250_write(void *base, uint8_t reg, uint8_t data);
#endif

int uart8250_mem_can_tx_byte(void *base);
void uart8250_mem_tx_byte(void *base, unsigned char data);
void uart8250_mem_tx_flush(void *base);
int uart8250_mem_can_rx_byte(void *base);
unsigned char uart8250_mem_rx_byte(void *base);
void uart8250_mem_init(void *base, unsigned int divisor);

#endif /* CONSOLE_UART8250MEM_H */
