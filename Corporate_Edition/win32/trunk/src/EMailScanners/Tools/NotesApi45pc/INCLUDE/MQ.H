// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

/* Message Queue Package Public Definitions */

#ifndef MQ_DEFS
#define MQ_DEFS

#ifdef __cplusplus
extern "C" {
#endif

typedef DWORD MQHANDLE;	

#define MQ_MAX_MSGSIZE 		(MAXONESEGSIZE - 0x40)
#define NOPRIORITY			MAXWORD

/*	Function pointer type for MQScan() callback */

typedef	STATUS (LNCALLBACKPTR MQSCAN_CALLBACK) (const char far *pBuffer, 
		 						 			  WORD Length, WORD Priority,
								 			  void far *Ctx);

/*	Options to MQGet */

#define	MQ_WAIT_FOR_MSG			0x0001

																	

/*	Routine definitions */

STATUS LNPUBLIC	 MQCreate(char far *QueueName, WORD Quota, DWORD Options);
STATUS LNPUBLIC	 MQOpen(char far *QueueName, DWORD Options, MQHANDLE far *RetQueue);
STATUS LNPUBLIC	 MQClose(MQHANDLE Queue, DWORD Options);
STATUS LNPUBLIC	 MQPut(MQHANDLE Queue, WORD Priority, char far *Buffer, WORD Length, 
						DWORD Options);
STATUS LNPUBLIC	 MQGet(MQHANDLE Queue, char far *Buffer, WORD BufLength,
					  	DWORD Options, DWORD Timeout, WORD far *retMsgLength);
STATUS LNPUBLIC	 MQScan(MQHANDLE Queue, char far *Buffer, WORD BufLength, 
						 DWORD Options, MQSCAN_CALLBACK ActionRoutine,
						 void far *Ctx, WORD far *retMsgLength);
void LNPUBLIC	 MQPutQuitMsg(MQHANDLE Queue);
BOOL LNPUBLIC	 MQIsQuitPending(MQHANDLE Queue);
WORD LNPUBLIC	 MQGetCount(MQHANDLE Queue);

#ifdef __cplusplus
}
#endif

#endif /* MQ_DEFS */

