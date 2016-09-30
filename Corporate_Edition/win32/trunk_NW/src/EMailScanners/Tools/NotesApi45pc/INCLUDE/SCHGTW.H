// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef SCHGTW_DEFS
#define SCHGTW_DEFS

#ifndef SCHEDULE_DEFS
#include "schedule.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define	SCHMSG_INPUTQ_PREFIX	"MQ$ICSGTW"
#define	SCHMSG_OUTPUTQ_PREFIX	"MQ$OCSGTW"
#define NOTESSCHEDGATEWAY		"NOTESSCHEDGATEWAY"

/* message priority */
#define SCHMSG_RQST_PRIORITY		0x0001
#define SCHMSG_REPLY_PRIORITY	SCHMSG_RQST_PRIORITY

/*  CS (Calendaring and Scheduling) function request codes  */
#define SCHFUNC_RQST_SCHED		0x0001L	/* request schedules */

/* Request/Reply msg structure */
typedef struct 							/* preceded by CSOBJ_HEADER */
 {	
	DWORD 			dwFlags;			/* Flags defined as follows */
	WORD			wFunction;			/* function name */
#define SCHMSG_EndOfTransaction	0x00000001L	/* last buffer in message */
#define SCHMSG_LastBuffer		0x00000001L	/* last buffer in message */
#define SCHMSG_ReplyToRqst		0x00010000L
	HCNTNR			hMsgCntnr;			/* handle to a container object */
	STATUS			error;
	TIMEDATE		tdRequestQueued;	/* time request was queued */
	DWORD			spare[5];
 } SCHMSG;					

/* SCH reply/request objects */
typedef struct 							
 {	DWORD			dwOptions;		/* SCHRQST_xxx option codes (see schedule.h)*/
	WORD			wNumHops;		/* number of hops so far */
 	TIMEDATE_PAIR	Interval;		/* interval */
	UNID			ApptUnid;		/* Unid of appt to ignore busytime for */
	TIMEDATE		ApptOrigDate;	/* (Orginizer 2.x) the orig date of ignored appt */
	DWORD			reserved[10];
	WORD			wClientNamesSize;/* queryer's name length (includes term.) */
	WORD			wServerNameSize;/* notes server name length (includes term.) */
/*	Followed by Client name doing the query (NULL terminated) */
/*	Followed by name of Notes server to forward this request to (NULL terminated). */
 } RQST_SCHED_OBJ;

/* functions on containers */
STATUS LNPUBLIC SchContainer_FreeRequest(HANDLE hCntnr, HCNTNROBJ hRqst);
STATUS LNPUBLIC SchContainer_GetRequest(HANDLE hCntnr, HCNTNROBJ FAR *rethRqst, 
	void FAR * FAR *retpRqst, LIST FAR * FAR *retpNameList, LIST FAR * FAR *repClientNames, char FAR * FAR *retpNS);

/* functions on message queues */
STATUS LNPUBLIC SchMsgQHandles_New(char FAR *szServerName, BOOL fCreate,
					MQHANDLE FAR *rethInputQ, MQHANDLE FAR *rethOutputQ);
void LNPUBLIC SchMsgQHandles_Free(MQHANDLE hInputQ, MQHANDLE hOutputQ);


/* functions on containers */
STATUS LNPUBLIC SchContainer_DupHandle(HCNTNR hOrigCntnr, HCNTNR FAR *phDup);

/* functions on schedules */
STATUS LNPUBLIC Schedule_Free(HCNTNR hCntnr, HSCHEDULE hSched);
STATUS LNPUBLIC Schedule_NewFromSchedList (HCNTNR hCntnr, 
										TIMEDATE_PAIR FAR *pInterval, 
										SCHED_LIST FAR *pSchedList,
										char FAR *pUsername,
										STATUS errStatus, DWORD dwErrGateway,
										HSCHEDULE  FAR *rethSched);
STATUS LNPUBLIC Schedule_NewFromBitMap (HCNTNR hCntnr, 
										TIMEDATE_PAIR FAR *pInterval, 
										WORD IntervalSize,
										BYTE FAR *pBitMap, 
										char FAR *pUsername,
										STATUS errStatus, DWORD dwErrGateway,
										HSCHEDULE  FAR *rethSched);

STATUS LNPUBLIC Schedule_AddSchedList (HCNTNR hCntnr, 
										HCNTNROBJ  hSched,
										TIMEDATE_PAIR FAR *pInterval, 
										SCHED_LIST FAR *pSchedList,
										HCNTNROBJ FAR*rethSchedList);

STATUS LNPUBLIC	Schedule_AddDetail(HCNTNR hCntnr, HSCHEDULE hSchedObj, 
					UNID FAR *pUnid, DWORD dwDetailType, 
					DWORD dwDetailID, DWORD dwDetailChunkNo, 
					WORD wDetailChunkSize, BYTE FAR *pDetailData, 
					HCNTNROBJ *rethDetailObj);

#ifdef __cplusplus
}
#endif
#endif /* SCHGTW_DEFS */
