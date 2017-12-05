/*
 * app.c
 *
 *  Created on: Nov 6, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <getopt.h>
#include <signal.h>
#include <syslog.h>

#include "wb2x.h"
#include "utils/daemon.h"
#include "utils/time.h"

// app version, help
static void appHelp(const char *self);
static void appVersion(const char *self);

// handle user input, prepare environment, call main tasklet
int main(int argc, char **argv)
{
	int ret = 0;
	struct appConf env;
	memset(&env, 0x0, sizeof(env));
	snprintf(env.self, sizeof(env.self), "%s", argv[0]);

	int c = 0;
	while((c = getopt(argc, argv, "vhdDF:S:I:")) != -1)
	{
		switch(c) {
		case 'v':
			env.flag.version = 1;
			break;
		case 'h':
			env.flag.help = 1;
			break;
		case 'd':
			env.flag.debug = 1;
			break;
		case 'D':
			env.flag.daemon = 1;
			break;
		case 'F':
			snprintf(env.device, sizeof(env.device), "%s", optarg);
			break;
		case 'S':
			env.speed = (int) atoi(optarg);
			break;
		case 'I':
			env.interval = (int) atoi(optarg);
			break;
		default:
			printf(" * using default settings\n");
			break;
		}
	}

	DBG(" * check flags\n");
	if (env.flag.help) {
		appHelp(env.self);
		return 0;
	}
	if (env.flag.version) {
		appVersion(env.self);
		return 0;
	}
	if (env.flag.debug)
		appVersion(env.self);

	if (env.flag.daemon) {
		DBG(" * run as service\n");
		runDaemon();
	}

	// these params must valid when running
	env.pid = getpid();
	if (strlen(env.device) < 1) {
		DBG(" * using default device %s\n", WB_DEFAULT_DEVICE);
		snprintf(env.device, sizeof(env.device), "%s", WB_DEFAULT_DEVICE);
	}
	if (env.speed < 1) {
		DBG(" * using default speed %d\n", WB_DEFAULT_SPEED);
		env.speed = WB_DEFAULT_SPEED;
	}

	if (env.interval < 1) {
		DBG(" * using default interval %d\n", WB_DEFAULT_INTERVAL);
		env.interval = WB_DEFAULT_INTERVAL;
	}

	DBG(" # start main task (pid %d)\n", env.pid);
	LOG("ARN-WB %d started at %s", env.pid, currentDateTime());
	ret = ARNProtoWb(&env);
	LOG("ARN-WB %d stopped at %s", env.pid, currentDateTime());
	return ret;
}

static void appVersion(const char *self)
{
	printf("%s\n(%s)\n", self, WB_VERSION);
}

static void appHelp(const char *self)
{
	printf(" usage: %s [-v|-h|-d]\n", self);
	printf(" usage: %s [-F device] [-D]\n", self);
}
