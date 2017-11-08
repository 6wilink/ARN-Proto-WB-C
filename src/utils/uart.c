/*
 * uart.c
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include "uart.h"


// default is 9600/8/N/1
int uartOpen(const char *dev, const unsigned long speed)
{
	int invalidUartFd = 2;
	static int uartFd = -1;

	uartFd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	//uartFd = open(dev, O_RDWR | O_NOCTTY);
	if (uartFd < invalidUartFd)
	{
		return UART_ERR_OPEN;
	}

	struct termios opt;
	tcgetattr(uartFd, &opt);
	switch(speed)
	{
	case 115200:
		cfsetispeed(&opt, B115200);
		cfsetospeed(&opt, B115200);
		break;
	case 9600:
	default:
		cfsetispeed(&opt, B9600);
		cfsetospeed(&opt, B9600);
		break;
	}

    opt.c_cflag |= (CLOCAL | CREAD);
    opt.c_cflag &= ~PARENB;
    opt.c_cflag &= ~CSTOPB;
    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |=  CS8;
    //opt.c_cflag &= ~INPCK;
    opt.c_cflag |= INPCK;
    opt.c_cflag &= ~CRTSCTS;

    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //opt.c_lflag |= (ICANON | ECHO | ECHOE | ISIG);

    //opt.c_oflag &= ~OPOST;
    opt.c_oflag &= ~(INLCR | IGNCR | ICRNL | ONLCR | OCRNL); // 706

    opt.c_iflag &= ~IXON; // 706, v1.5 WYY
    opt.c_iflag &= ~IXOFF; // 706, v1.5 WYY
    //opt.c_iflag &= ~INLCR;
    //opt.c_iflag &= ~IGNCR;

    //opt.c_cc[VTIME] = 150;
    //opt.c_cc[VMIN] = 0;

	tcsetattr(uartFd, TCSANOW, &opt);
	tcflush(uartFd, TCIFLUSH);

	while(uartRead(uartFd, (void *) 0, 1) > 0);

	return uartFd;
}

int uartRead(const int uartFd, char *buffer, const unsigned int bufferLength)
{
	int b2rw = 0;
	b2rw = read(uartFd, buffer, bufferLength);
	//printf("uart > read %d bytes\n", b2rw);
	return b2rw;
}

int uartWrite(const int uartFd, const char *data, const unsigned int dataLength)
{
	int b2rw = 0;
	b2rw = write(uartFd, data, dataLength);
	if (b2rw < dataLength)
	{
		return -1;
	}
	return b2rw;
}

void uartClose(const int uartFd)
{
	if (uartFd)
	{
		close(uartFd);
	}
}
