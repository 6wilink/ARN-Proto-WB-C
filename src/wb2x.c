/*
 * wb2x.c
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <signal.h>
#include <syslog.h>

#include "utils/uart.h"
#include "utils/time.h"

#include "wb2x.h"
#include "wb2xPacket.h"


static int SG_FLAG_EXIT = 0;
static void handleSysSignal(void);

static int ARNProtoWbServ(const int uartFd);
static int ARNProtoWbReport(const int uartFd);

int ARNProtoWb(const struct appConf *env)
{
	DBG(" --> take care of SIGINT/SIGTERM/SIGQUIT\n");
	signal(SIGINT, (void (*) (int)) handleSysSignal);
	signal(SIGTERM, (void (*) (int)) handleSysSignal);
	signal(SIGQUIT, (void (*) (int)) handleSysSignal);

	int uartFd = -1;
	int interval = env->interval;

	// try to open uart: /dev/ttyS0
	DBG(" --> try to open device %s\n", env->device);
	uartFd = uartOpen(env->device, env->speed);
	if (uartFd < 0) {
		LOG("device %s is empty or busy\n", env->device);
		return WB2X_ERR_UART_DEVICE;
	}

	// idle when system just online
	sleep(10);

	// main loop
	DBG(" --# major loop\n");
	for(;;)
	{
		DBG(" ----> checking flags\n");
		if (SG_FLAG_EXIT) {
			DBG(" ----# flags detected, exit major loop\n");
			break;
		}

		ARNProtoWbServ(uartFd);
		DBG(" ----> LCD served | %s\n", currentDateTime());

		ARNProtoWbReport(uartFd);
		DBG(" ----> Report sent | %s\n", currentDateTime());

		DBG(" ----> take a break | %ds\n", interval);
		sleep(interval);
	}

	// free before exit
	DBG(" --> close device %s\n", env->device);
	uartClose(uartFd); uartFd = -1;
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
