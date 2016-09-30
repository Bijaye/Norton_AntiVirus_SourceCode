// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/* 	Calendaring and Scheduling structures
*/
#ifndef SCHEDULE_DEFS
#define SCHEDULE_DEFS

#ifdef __cplusplus
extern "C" {
#endif


#ifndef NSF_DEFS
#include "nsfdata.h"	/* for UNID */
#endif

#ifndef MQ_DEFS
#include "mq.h"
#endif

/* these are magic cookies */
typedef	HANDLE			HCNTNR;			/* handle to a container */
typedef DWORD			HCNTNROBJ;		/* handle to an object in a container */
#define NULLCNTNROBJ	0L				/* null container  object handle */
#define HSCHEDULE		HCNTNROBJ		/* handle to a schedule */


typedef struct
 {	DWORD			reserved[10];
 	TIMEDATE_PAIR	Interval;			/* events etc. are in this interval */
	DWORD			dwErrGateway;		/* gateway error retrieving this schedule */
	STATUS			error;				/* error retrieving this schedule */
	WORD			wReserved;			/* unused at this time */
	WORD			wOwnerNameSize;		/* size of owner name (includes term.)*/
										/* followed by owner name */
 } SCHEDULE;




/* detail info */
#define SCHDETAILTYPE_VCAL		0x00000001L /* details coded in Versit VCAL format */

#define MAXSCHEDENTRYDETAILSIZE (MAXONESEGSIZE - 1024 - sizeof(SCHED_ENTRY_DETAIL))

typedef struct 							
 {	DWORD			reserved[10];		/* Must be zeroes */
	UNID			Unid;				/* Unique id (same as in SCHED_ENTRY) */
	DWORD			dwDetailType;		/* SCHDETAILTYPE_xxx */
	DWORD			dwDetailID;			/* Default to zero, incremented only
										** - if we have more than one detail
										** - object of this same type 
										** - corresponding to this appointment.
										*/
	DWORD			dwDetailChunkNo;	/* Default to zero, used only if this 
										** - detail object is broken up into 
										** - multiple chunks all corresponding
										** - to the same dwDetailTypeSubID
										*/
	WORD			wDetailChunkSize;	/* Size of detail chunk that follows 
										** - this structure. Note that
										** - max is MAXSCHEDENTRYDETAILSIZE 
										*/
 } SCHED_ENTRY_DETAIL;

/* functions on containers */

STATUS LNPUBLIC SchContainer_New(HCNTNR FAR *rethCntnr);

void LNPUBLIC SchContainer_Free(HCNTNR hCntnr);

STATUS LNPUBLIC SchContainer_FindSchedule(HCNTNR hCntnr, 
		char FAR *pszUserName, STATUS FAR *retErrStatus, DWORD FAR *retdwErrGateway, 
		HSCHEDULE FAR * retvbObj, SCHEDULE FAR * FAR *retpSched);

STATUS LNPUBLIC SchContainer_GetFirstSchedule(HANDLE hCntnr, HSCHEDULE FAR *rethObj, SCHEDULE FAR * FAR *retpSchedule);

STATUS LNPUBLIC SchContainer_GetNextSchedule(HANDLE hCntnr, HSCHEDULE hCurSchedule, HSCHEDULE FAR *rethNextSchedule, SCHEDULE FAR * FAR *retpNextSchedule);

/* functions on schedules */

STATUS LNPUBLIC Schedule_ExtractBusyTimeRange(HCNTNR hCntnr, 
		HCNTNROBJ hSchedObj, UNID FAR *punidIgnore,  
		TIMEDATE_PAIR FAR *pInterval, DWORD FAR *retdwSize, HANDLE FAR *rethRange, 
		HCNTNROBJ *rethMoreCtx);

STATUS LNPUBLIC Schedule_ExtractMoreBusyTimeRange(HCNTNR hCntnr, 
		HCNTNROBJ hMoreCtx, UNID *punidIgnore,  
		TIMEDATE_PAIR *pInterval, DWORD *retdwSize, HANDLE *rethRange, 
		HCNTNROBJ *rethMore);

STATUS LNPUBLIC Schedule_ExtractFreeTimeRange(HCNTNR hCntnr, 
		HCNTNROBJ hSchedObj, UNID FAR *punidIgnore, BOOL fFindFirstFit, WORD wDuration,
		TIMEDATE_PAIR FAR *pInterval, 
		DWORD FAR *retdwSize, HANDLE FAR *rethRange);

STATUS LNPUBLIC Schedule_ExtractSchedList(HCNTNR hCntnr, 
		HCNTNROBJ hSchedObj,TIMEDATE_PAIR FAR *pInterval, DWORD FAR *retdwSize, 
		HANDLE FAR *rethSchedList, HCNTNROBJ FAR *rethMore);

STATUS LNPUBLIC Schedule_ExtractMoreSchedList(HCNTNR hCntnr, 
		HCNTNROBJ hMoreCtx, TIMEDATE_PAIR FAR *pInterval, DWORD FAR *retdwSize, 
		HANDLE FAR *rethSchedList, HCNTNROBJ FAR *rethMore);



STATUS LNPUBLIC Schedule_GetFirstDetail(HCNTNR hCntnr, HSCHEDULE hSchedObj, 
			HCNTNROBJ *rethDetailObj, 
			SCHED_ENTRY_DETAIL FAR * FAR * retpDetail);

STATUS LNPUBLIC Schedule_GetNextDetail(HCNTNR hCntnr, 
			HCNTNROBJ hDetailObj, 
			HCNTNROBJ *rethNextDetailObj, 
			SCHED_ENTRY_DETAIL FAR * FAR * retpNextDetail);


/*	Schedule Query APIs */

/* option flags for scheduling */
#define SCHRQST_COMPOSITE		0x0001L	/* return composite sched */
#define	SCHRQST_EACHPERSON		0x0002L	/* return each person's sched */
#define SCHRQST_LOCAL			0x0004L /* do only local lookup */
#define SCHRQST_FORCEREMOTE		0x0008L /* force remote lookups even if workstation based mail */
#define SCHRQST_DETAIL			0x0010L /* get detailed info on appts (MUST BE USED WITH the SCHRQST_EACHPERSON */


STATUS LNPUBLIC SchFreeTimeSearch(
						UNID  			FAR *pApptUnid,
						TIMEDATE 		FAR *pApptOrigDate,
						WORD			fFindFirstFit,
						DWORD			dwReserved, 		
						TIMEDATE_PAIR 	FAR *pInterval,
						WORD 			Duration,
						LIST 			FAR *pNames,
						HANDLE 			FAR *rethRange);

STATUS LNPUBLIC SchRetrieve(
						UNID			FAR *pApptUnid,
						TIMEDATE 		FAR *pApptOrigDate,
						DWORD 			dwOptions,
						TIMEDATE_PAIR FAR *pInterval,
						LIST 			FAR	*pNames,
						HCNTNR 			FAR	*rethCntnr,
						void			FAR *MustBeNull1, 
						void 			FAR *MustBeNull2,
						void 			FAR* FAR *MustBeNull3);

STATUS LNPUBLIC SchSrvRetrieve(
						LIST 	FAR *pClientNames,
						UNID	FAR *pApptUnid,
						TIMEDATE FAR *pApptOrigDate,
						DWORD dwOptions,
						TIMEDATE_PAIR FAR *pInterval,
						LIST FAR *pNames,
						HCNTNR FAR *rethCntnr);

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULE_DEFS */

