/*
 * sys.c
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>

void sysExecute(const char *cmd)
{
	//printf(">>>> sysExecute(%s)\n", cmd);
	system(cmd);
}
