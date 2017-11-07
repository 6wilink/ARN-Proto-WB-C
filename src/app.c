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

#include "include/wb2x.h"
#include "include/utils/daemon.h"

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
			env.idle = (int) atoi(optarg);
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

	if (env.flag.daemon)
		runDaemon();

	// these params must valid when running
	if (strlen(env.device) < 1)
		snprintf(env.device, sizeof(env.device), "%s", WB_DEFAULT_DEVICE);
	if (env.speed < 1)
		env.speed = WB_DEFAULT_SPEED;

	if (env.idle < 1)
		env.idle = WB_DEFAULT_IDLE;

	env.pid = getpid();
	ret = ARNProtoWb(&env);
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
