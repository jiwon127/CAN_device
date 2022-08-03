/*
 * uart.h
 *
 *  Created on: 2021. 2. 1.
 *      Author: kjm-hsh
 */

#ifndef HEADER_UART7_H_
#define HEADER_UART7_H_


void uart_7_init(void);
void uart_7_tx(char data);
unsigned char uart_7_rx(void);
int uart_7_tx_str(char *str);
void uart_7_rx_str(char *str,unsigned short max);
void uart_7_tx_hex(int number);


#endif /* HEADER_UART_H_ */
