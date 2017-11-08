/*
 * wb2x_packet.h
 *
 *  Created on: Nov 7, 2017
 *      Author: Qige <qigezhao@gmail.com>
 */

#ifndef WB2X_PACKET_H_
#define WB2X_PACKET_H_

int WBParse(char *data, const unsigned int dataLength,
		char *cmd, const unsigned int cmdLength);

int WBParseCmd(const char *wbCmd);
int WBReport(char *buffer, const unsigned int bufferLength);

#endif /* WB2X_PACKET_H_ */
