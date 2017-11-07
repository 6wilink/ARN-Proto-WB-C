/*
 * wb2x.c
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <time.h>
#include <signal.h>

#include "utils/uart.h"

#include "wb2x.h"
#include "wb2xPacket.h"


static int SG_FLAG_EXIT = 0;
static void handleSysSignal(void);

static int ARNProtoWbServ(const int uartFd);
static int ARNProtoWbReport(const int uartFd);

int ARNProtoWb(const struct appConf *env)
{
	//signal(SIGINT, (void (*) (int)) handleSysSignal);
	//signal(SIGTERM, (void (*) (int)) handleSysSignal);
	//signal(SIGQUIT, (void (*) (int)) handleSysSignal);

	int uartFd = -1;
	int idle = env->idle;

	time_t now;
	struct tm *timenow;

	// try to open uart: /dev/ttyS0
	uartFd = uartOpen(env->device, env->speed);
	if (uartFd < 0)
		return WB2X_ERR_UART_DEVICE;

	// main loop
	for(;;)
	{
		if (SG_FLAG_EXIT)
			break;

		ARNProtoWbServ(uartFd);
		time(&now);
		timenow = localtime(&now);
		DBG("cmd read > %s\n", asctime(timenow));

		ARNProtoWbReport(uartFd);
		time(&now);
		timenow = localtime(&now);
		DBG("report sent > %s\n", asctime(timenow));

		sleep(idle);
	}

	// free before exit
	uartClose(uartFd);
	return WB2X_OK;
}

// +wbcmd:[reboot|wifi|failsafe|gwsmax|gwsmin]
// +wbsetchn:<ch>
// +wbsetpwr:<txpwr>
// +wbsetchnbw:<chbw>
// +wbsetrgn:<rgn>
// +wbsetmod:<0:mesh|1:ear|2:car>
static int ARNProtoWbServ(const int uartFd)
{
	char cmd[WB_DEFAULT_CMD_LENGTH], buffer[WB_DEFAULT_PROTO_LENGTH];
	memset(buffer, 0x0, sizeof(buffer));
	int b = 0, bFound = 0;
	b = uartRead(uartFd, buffer, sizeof(buffer));
	if (b > 0) {
		DBG("%d bytes: %s\n", b, buffer);
		memset(cmd, 0x0, sizeof(cmd));
		bFound = WBParse(buffer, sizeof(buffer), cmd, sizeof(cmd));
		if (bFound) {
			DBG("cmd [%s] found! end byte %d\n", cmd, bFound);
			WBParseCmd(cmd);
		}
	}
	return 0;
}

static int ARNProtoWbReport(const int uartFd)
{
	int b = -1;
	char msg[WB_DEFAULT_PROTO_LENGTH];
	// +wbsts:<ch>,<noise-111>,<txpwr>,<mode>,<rxgain>,<chanbw>\r\n
	// +wbsigs:<p1sig>,<p2sig>,<p3sig>,<p4sig>,<p5sig>\r\n
	// +wbextra:<ipaddr>,<hwarch>
	memset(msg, 0x00, sizeof(msg));
	b = WBReport(msg, sizeof(msg));
	if (b > 0) {
		DBG("report [%s] sent! %d bytes\n", msg, b);
		uartWrite(uartFd, msg, strlen(msg));
	}

	return 0;
}

static void handleSysSignal(void)
{
	SG_FLAG_EXIT = 1;

	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
