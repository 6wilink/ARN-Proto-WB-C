/*
 * wb2x.h
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#ifndef WB2X_H_
#define WB2X_H_


#ifdef RELEASE_DEBUG
#define DBG(fmt, ...) { fprintf(stderr, "dbg> "fmt, ##__VA_ARGS__); }
#else
#define DBG(fmt, ...) {}
#endif

#define LOG(fmt, ...) 	{ char _bf[1024] = {0}; snprintf(_bf, sizeof(_bf)-1, fmt, ##__VA_ARGS__); \
fprintf(stderr, "> %s", _bf); syslog(LOG_INFO, "%s", _bf); }


#define WB_VERSION ("v2.0-071117. Buildtime: "__DATE__", "__TIME__)

#define WB_DEFAULT_DEVICE 		"/dev/ttyS0"
#define WB_DEFAULT_SPEED 		9600
#define WB_DEFAULT_INTERVAL 	2

#define WB_DEFAULT_CMD_LENGTH	32
#define WB_DEFAULT_PROTO_LENGTH 128


// app environment
enum {
	WB2X_OK = 0,
	WB2X_ERR_UART_DEVICE,
	WB2X_ERR_UART
};

#define WB2X_BASE_LENGTH 16
#define WB2X_DEVICE_LENGTH 32

struct appConf
{
	int pid;
	char self[WB2X_BASE_LENGTH];
	char device[WB2X_DEVICE_LENGTH];
	unsigned long speed;
	unsigned int interval;
	struct {
		int version;
		int help;
		int debug;
		int daemon;
	} flag;
};


int ARNProtoWb(const struct appConf *env);


#endif /* WB2X_H_ */
