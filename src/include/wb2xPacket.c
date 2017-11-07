/*
 * wb2x_packet.c
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */
#include <stdio.h>
#include <string.h>

#include "gws2.h"
#include "wb2x.h"
#include "wb2xPacket.h"
#include "utils/sys.h"

int WBParse(char *data, const unsigned int dataLength,
		char *cmd, const unsigned int cmdLength)
{
	char **p;
	unsigned int i, j, k, start = -1, stop = -1;
	unsigned int tryLength = (dataLength < cmdLength ? dataLength : cmdLength);

	p = &data;
	for(i = 0; i < tryLength; i ++)
	{
		if (**p == '+') {
			start = i + 1;
			break;
		}
		(*p) ++;
	}

	if (start >= 0)
	{
		for(j = start, k = 0;
			j < tryLength && k < cmdLength;
			j ++, k ++)
		{
			if (**p == '\r' || **p == '\n') {
				stop = j;
				cmd[k] = '\0';
				break;
			}
			// save to "*cmd"
			cmd[k] = (**p);
			(*p) ++;
		}
	}

	if (stop < 0)
		cmd[0] = '\0';

	return stop;
}

// +wbcmd:[reboot|wifi|failsafe|gwsmax|gwsmin]
// +wbsetchn:<ch>
// +wbsetpwr:<txpwr>
// +wbsetchnbw:<chbw>
// +wbsetrgn:<rgn>
// +wbsetmod:<0:mesh|1:ear|2:car>
int WBParseCmd(const char *wbCmd)
{
	int val = -1;
	char cmd[WB_DEFAULT_CMD_LENGTH];
	memset(cmd, 0x0, sizeof(cmd));

	if (strstr(wbCmd, "+wbcmd:") > 0) {
		if (sscanf(wbCmd, "+wbcmd:%s", cmd) != -1) {
			DBG("got cmd [%s]\n", cmd);
			if (! strcasecmp(cmd, "reboot")) {
				sysExecute("reboot");
			} else if (! strcasecmp(cmd, "failsafe")) {
				sysExecute("ifconfig eth0 192.168.1.1/24 up");
			} else if (! strcasecmp(cmd, "wifi")) {
				sysExecute("wifi");
			} else if (! strcasecmp(cmd, "gwsmin")) {
				sysExecute("settxpwr 17");
			} else if (! strcasecmp(cmd, "gwsmax")) {
				sysExecute("settxpwr 32");
			} else {
				printf("----> unknown command: %s\n", cmd);
			}
		}
	} else if (strstr(wbCmd, "+wbsetchn:") > 0) {
		if (sscanf(wbCmd, "+wbsetchn:%d", &val) != -1) {
			if (val > 0) {
				snprintf(cmd, sizeof(cmd), "setchan %d\n", val);
				sysExecute(cmd);
			}
		}
	} else if (strstr(wbCmd, "+wbsetpwr:") > 0) {
		if (sscanf(wbCmd, "+wbsetpwr:%d", &val) != -1) {
			if (val > 0) {
				snprintf(cmd, sizeof(cmd), "settxpwr %d\n", val);
				sysExecute(cmd);
			}
		}
	} else if (strstr(wbCmd, "+wbsetrgn:") > 0) {
		if (sscanf(wbCmd, "+wbsetrgn:%d", &val) != -1) {
			if (val > -1) {
				snprintf(cmd, sizeof(cmd), "setregion %d\n", val);
				sysExecute(cmd);
			}
		}
	} else if (strstr(wbCmd, "+wbsetmod:") > 0) {
		if (sscanf(wbCmd, "+wbsetmod:%d", &val) != -1) {
			switch(val) {
			case 0:
				sysExecute("config_mesh");
				break;
			case 1:
				sysExecute("config_ear");
				break;
			case 2:
				sysExecute("config_car");
				break;
			default:
				printf("----> unknown mode: %d\n", val);
				break;
			}
		}
	}
	return 0;
}

// generate report data, and write to "*buffer"
// +wbsts:<ch>,<noise-111>,<txpwr>,<mode>,<region>,<chanbw>,<rxgain>\r\n
// +wbsigs:<p1sig>,<p2sig>,<p3sig>,<p4sig>,<p5sig>\r\n
// +wbextra:<ipaddr>,<hwarch>
int WBReport(char *buffer, const unsigned int bufferLength)
{
	unsigned int b = -1, msgLength = 0;
	char msg[WB_DEFAULT_PROTO_LENGTH];

	// init ABB & Radio
	gws2Sync("wlan0", "/dev/gws5001Radio");

	int i, val, signal[5];
	for(i = 0; i < sizeof(signal); i ++)
	{
		val = gws2AbbPeer(i);
		if (val == GWS_BAR_VAL)
			signal[i] = 99;
		else
			signal[i] = val + 111;
	}
	memset(msg, 0x00, sizeof(msg));
	snprintf(msg, sizeof(msg),
			"+wbsts:%d,%d,%d,%d,%d,%d,%d\r\n"\
			"+wbsigs:%d,%d,%d,%d,%d\r\n"\
			"+wbnw:%s,%s\r\n",
			gws2RadioChan(), gws2AbbNoise() + 101,
			gws2RadioTxPwr(), gws2AbbMode(),
			gws2RadioRgn(), gws2RadioChanBw(), gws2RadioRxGain(),
			signal[0], signal[1], signal[2], signal[3], signal[4],
			gws2SysIpAddr(), gws2SysHwArch()
	);

	msgLength = strlen(msg);
	b = (bufferLength > msgLength ? msgLength : bufferLength);
	memcpy(buffer, msg, b);

	gws2SyncClean();

	return b;
}

