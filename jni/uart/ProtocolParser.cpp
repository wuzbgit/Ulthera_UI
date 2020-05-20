/*
 * ProtocolParser.cpp
 *
 *  Created on: Sep 7, 2017
 *      Author: guoxs
 */
#include <vector>
#include <system/Mutex.h>
#include "CommDef.h"
#include "uart/ProtocolParser.h"
#include "uart/ProtocolSender.h"
#include "utils/Log.h"

static Mutex sLock;
static std::vector<OnProtocolDataUpdateFun> sProtocolDataUpdateListenerList;

void registerProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener) {
	Mutex::Autolock _l(sLock);
	LOGD("registerProtocolDataUpdateListener\n");
	if (pListener != NULL) {
		sProtocolDataUpdateListenerList.push_back(pListener);
	}
}

void unregisterProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener) {
	Mutex::Autolock _l(sLock);
	LOGD("unregisterProtocolDataUpdateListener\n");
	if (pListener != NULL) {
		std::vector<OnProtocolDataUpdateFun>::iterator iter = sProtocolDataUpdateListenerList.begin();
		for (; iter != sProtocolDataUpdateListenerList.end(); iter++) {
			if ((*iter) == pListener) {
				sProtocolDataUpdateListenerList.erase(iter);
				return;
			}
		}
	}
}

static void notifyProtocolDataUpdate(SProtocolData &data) {
	Mutex::Autolock _l(sLock);
	BYTE *pData;
	UINT16 cmd;
	data.ackData = NULL;
	data.acklen = 0;
	data.finishProcess = false;
	std::vector<OnProtocolDataUpdateFun>::const_iterator iter = sProtocolDataUpdateListenerList.begin();
	for (; iter != sProtocolDataUpdateListenerList.end(); iter++) {
		(*iter)(data);
		if(data.finishProcess == true)
		{
			pData = data.orgData;
			cmd = MAKEWORD(pData[2],pData[3]);
			if(!(cmd&CMD_MASK))
			{
				sendProtocol(cmd|CMD_MASK, data.ackData, data.acklen);
			}
			break;
		}
	}
}

static SProtocolData sProtocolData;

SProtocolData& getProtocolData() {
	return sProtocolData;
}

/**
 * 获取校验码
 */
BYTE getCheckSum(const BYTE *pData, int len) {
	int sum = 0;
	for (int i = 0; i < len; ++i) {
		sum += pData[i];
	}

	return (BYTE) (~sum + 1);
}

/**
 * 解析每一帧数据
 */
static void procParse(BYTE *pData, UINT len) {
	// 通知协议数据更新
	sProtocolData.finishProcess = false;
	sProtocolData.orgData = pData;
	sProtocolData.orgDatalen = len;
	notifyProtocolDataUpdate(sProtocolData);
}

/**
 * 功能：解析协议
 * 参数：pData 协议数据，len 数据长度
 * 返回值：实际解析协议的长度
 */
int parseProtocol(BYTE *pData, UINT len) {
	UINT remainLen = len;	// 剩余数据长度
	UINT dataLen;	// 数据包长度
	UINT frameLen;	// 帧长度

	/**
	 * 以下部分需要根据协议格式进行相应的修改，解析出每一帧的数据
	 */
	while (remainLen >= DATA_PACKAGE_MIN_LEN) {
		// 找到一帧数据的数据头
		while ((remainLen >= 2) && ((pData[0] != CMD_HEAD1) || (pData[1] != CMD_HEAD2))) {
			pData++;
			remainLen--;
			continue;
		}

		if (remainLen < DATA_PACKAGE_MIN_LEN) {
			break;
		}

		dataLen = MAKEWORD(pData[4], pData[5]);
		frameLen = dataLen + DATA_PACKAGE_MIN_LEN;
		if (frameLen > remainLen) {
			// 数据内容不全
			break;
		}

		// 打印一帧数据，需要时在CommDef.h文件中打开DEBUG_PRO_DATA宏
#ifdef DEBUG_PRO_DATA
		for (UINT i = 0; i < frameLen; ++i) {
			LOGD("%x ", pData[i]);
		}
		LOGD("\n");
#endif

		// 支持checksum校验，需要时在CommDef.h文件中打开PRO_SUPPORT_CHECK_SUM宏
#ifdef PRO_SUPPORT_CHECK_SUM
		// 检测校验码
		if (getCheckSum(pData, frameLen - 1) == pData[frameLen - 1]) {
			// 解析一帧数据
			procParse(pData, frameLen);
		} else {
			LOGE("CheckSum error!!!!!!\n");
		}
#else
		// 解析一帧数据
		procParse(pData, frameLen);
#endif

		pData += frameLen;
		remainLen -= frameLen;
	}
	return len - remainLen;
}
