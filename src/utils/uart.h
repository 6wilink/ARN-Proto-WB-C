/*
 * uart.h
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#ifndef UART_H_
#define UART_H_

#define UART_SPEED B9600

enum UART_ERR {
	UART_ERR_OPEN = -1,
	UART_OK = 0,
};

int uartOpen(const char *device, const unsigned long speed);
void uartClose(const int uartFd);

int uartRead(const int uartFd, char *buffer, const unsigned int bufferLength);
int uartWrite(const int uartFd, const char *data, const unsigned int dataLength);

#endif /* UART_H_ */
