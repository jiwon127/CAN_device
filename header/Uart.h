/*
 * uart.h
 *
 *  Created on: 2021. 2. 1.
 *      Author: kjm-hsh
 */

#ifndef HEADER_UART_H_
#define HEADER_UART_H_


void uart_init(void);
void uart_tx(char data);
unsigned char uart_rx(void);
int uart_tx_str(char *str);
void uart_rx_str(char *str,unsigned short max);
void uart_tx_hex(int number);


#endif /* HEADER_UART_H_ */
