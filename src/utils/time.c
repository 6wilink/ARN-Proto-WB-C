/*
 * time.c
 *
 *  Created on: Nov 8, 2017
 *      Author: qige
 */

#include <time.h>

char *currentDateTime(void)
{
	time_t now;
	struct tm *timenow;

	time(&now);
	timenow = localtime(&now);

	return asctime(timenow);
}
