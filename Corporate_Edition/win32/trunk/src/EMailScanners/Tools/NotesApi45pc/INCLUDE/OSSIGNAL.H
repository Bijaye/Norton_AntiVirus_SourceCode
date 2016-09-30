// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif

#ifndef OSSIGNAL_DEFS
#define OSSIGNAL_DEFS

/*	OS Signal Definitions */


#define	OS_SIGNAL_MESSAGE			3	/* Indirect way to call NEMMessageBox */
										/*	STATUS = Proc(Message, OSMESSAGETYPE_xxx) */
typedef STATUS (LNCALLBACKPTR OSSIGMSGPROC)(char far *Message, WORD Type);

#define	OS_SIGNAL_BUSY				4	/* Paint busy indicator on screen */
										/*	STATUS = Proc(BUSY_xxx) */
typedef STATUS (LNCALLBACKPTR OSSIGBUSYPROC)(WORD BusyType);

#define	OS_SIGNAL_CHECK_BREAK		5	/* Called from NET to see if user cancelled I/O */
										/*	STATUS = Proc(void) */
typedef STATUS (LNCALLBACKPTR OSSIGBREAKPROC)(void);


#define	OS_SIGNAL_DIAL				10	/* Prompt to dial a remote system */
										/*  pServer = Desired server name (or NULL) */
										/*  pPort = Desired port name (or NULL) */
										/*  pDialParams = Reserved */
										/*  pRetServer = Actual server name to be called */
										/*				(or NULL if not desired) */
										/*  pRetPort = Actual port name being used */
										/*				(or NULL if not desired) */

typedef STATUS (LNCALLBACKPTR OSSIGDIALPROC)(char far *pServer, 
											   char far *pPort,
											   void far *pDialParams, 
											   char far *pRetServer, 
											   char far *pRetPort);


typedef void (LNCALLBACKPTR OSSIGPROC)();	/* Defined only for following lines */
OSSIGPROC LNPUBLIC	OSSetSignalHandler(WORD, OSSIGPROC);
OSSIGPROC LNPUBLIC	OSGetSignalHandler(WORD);

/*	Definitions specific to message signal handler */

#define	OSMESSAGETYPE_OK			0
#define	OSMESSAGETYPE_OKCANCEL		1
#define	OSMESSAGETYPE_YESNO			2
#define	OSMESSAGETYPE_YESNOCANCEL	3
#define	OSMESSAGETYPE_RETRYCANCEL	4
#define	OSMESSAGETYPE_POST			5
#define	OSMESSAGETYPE_POST_NOSERVER	6


/*	Definitions specific to busy signal handler */

#define	BUSY_SIGNAL_FILE_INACTIVE	0
#define	BUSY_SIGNAL_FILE_ACTIVE		1
#define	BUSY_SIGNAL_NET_INACTIVE	2
#define	BUSY_SIGNAL_NET_ACTIVE		3
#define	BUSY_SIGNAL_POLL			4
#define BUSY_SIGNAL_WAN_SENDING		5
#define	BUSY_SIGNAL_WAN_RECEIVING	6


/*	End of OS Signal Definitions */

#endif

#ifdef __cplusplus
}
#endif

