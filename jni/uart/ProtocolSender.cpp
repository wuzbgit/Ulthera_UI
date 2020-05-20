/*
 * ProtocolSender.cpp
 *
 *  Created on: Sep 8, 2017
 *      Author: guoxs
 */

#include "uart/ProtocolSender.h"
#include "uart/UartContext.h"
#include "utils/Log.h"
#include <stdio.h>

extern BYTE getCheckSum(const BYTE *pData, int len);

/**
 * 需要根据协议格式进行拼接，以下只是个模板
 */
bool sendProtocol(const UINT16 cmdID, const BYTE *pData, BYTE len) {
	if (len + DATA_PACKAGE_MIN_LEN > 256) {
		LOGE("sendProtocol data is too len !!!\n");
		return false;
	}

	BYTE dataBuf[256];

	dataBuf[0] = CMD_HEAD1;
	dataBuf[1] = CMD_HEAD2;			// 同步帧头

	dataBuf[2] = LOBYTE(cmdID);
	dataBuf[3] = HIBYTE(cmdID);		// 命令字节

	dataBuf[4] = LOBYTE(len);
	dataBuf[5] = HIBYTE(len);

	UINT frameLen = 6;

	// 数据
	for (int i = 0; i < len; ++i) {
		dataBuf[frameLen] = pData[i];
		frameLen++;
	}

	dataBuf[frameLen++] = 0x0A;

#ifdef PRO_SUPPORT_CHECK_SUM
	// 校验码
	dataBuf[frameLen] = getCheckSum(dataBuf, frameLen);
	frameLen++;
#endif

	return UARTCONTEXT->send(dataBuf, frameLen);
}
