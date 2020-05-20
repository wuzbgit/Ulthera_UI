/*
 * ProtocolData.h
 *
 *  Created on: Sep 7, 2017
 *      Author: guoxs
 */

#ifndef _UART_PROTOCOL_DATA_H_
#define _UART_PROTOCOL_DATA_H_

#include <string>
#include "CommDef.h"

typedef enum
{
	FRAME_POWERON = 0,
	FRAME_MACHCHECK,
	FRAME_MACHREADYTEST,
	FRAME_MACHTESTING,
	FRAME_MACHREADY,
	FRAME_READYWORK,
	FRAME_WORKING,
	FRAME_ERRORNOHANDLE,
	FRAME_ERRORMACHCHECK,
	FRAME_ERRORMACHTEST
}FrameChangeCMD;
/******************** CmdID ***********************/
#define CMD_MASK								0x1000
#define CMDID_DISPLAYPOWERON					0x0001
#define CMDID_DISPLAYPOWERON_ACK				0x1001
#define CMDID_FRAMECHANGE						0x0010
#define CMDID_FRAMECHANGE_ACK					0x1010
#define CMDID_GETMINPOWERNUMBER					0x0020
#define CMDID_GETMINPOWERNUMBER_ACK				0x1020
#define CMDID_GETMAXPOWERNUMBER					0x0030
#define CMDID_GETMAXPOWERNUMBER_ACK				0x1030
#define CMDID_SETMINPOWERNUMBER					0x0040
#define CMDID_SETMINPOWERNUMBER_ACK				0x1040
#define CMDID_GETHANDFOOTSWT					0x0050
#define CMDID_GETHANDFOOTSWT_ACK				0x1050
#define CMDID_SETHANDFOOTSWT					0x0060
#define CMDID_SETHANDFOOTSWT_ACK				0x1060
#define CMDID_MACHWORKING						0x0070
#define CMDID_MACHWORKING_ACK					0x1070
/**************************************************/

/******************** 错误码 ***********************/
#define ERROR_CODE_CMDID			1
/**************************************************/

typedef struct {
	bool finishProcess;
	BYTE *orgData;
	UINT16 orgDatalen;
	BYTE *ackData;
	UINT16 acklen;
} SProtocolData;

#endif /* _UART_PROTOCOL_DATA_H_ */
