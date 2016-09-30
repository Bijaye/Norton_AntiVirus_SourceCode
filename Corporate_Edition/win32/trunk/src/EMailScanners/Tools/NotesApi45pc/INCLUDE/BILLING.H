// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

/* Billing Package Public Definitions */

#ifndef BILLING_DEFS
#define BILLING_DEFS

#ifdef __cplusplus
extern "C" {
#endif

#include "net.h"			/* for SESSIONID */

/*  Billing Queue Name */

#define BILL_QUEUE_NAME		"MQ$BILLING"   


/*  Billing Classes: 32 bit mask */

#define BILL_CLASS_SESSION				0x00000001		/* Session */
#define BILL_CLASS_REPLICATION			0x00000002		/* Replication*/
#define BILL_CLASS_DOCUMENT				0x00000004		/* Document Charge */
#define BILL_CLASS_MAIL 				0x00000008      /* Mail*/
#define BILL_CLASS_DATABASE				0x00000010		/* Database*/
#define BILL_CLASS_AGENT				0x00000020		/* Agent */

	
/* Billing Structure Types:  Reserved 1-32000;  Unused 32001-64K */

#define BILL_SESSIONREC			1		/* Session Class: Range 1-1000 */
#define BILL_REPLREC			1001	/* Replication Class: Range 1001-2000 */
#define BILL_DOCCHARGE			2001	/* Document Class: Range 2001-3000 */
#define BILL_MAILREC			3001	/* Mail Class:  Range 3001-4000 */
#define BILL_DBREC				4001	/* Database Class: Range 4001-5000*/
#define BILL_AGENTREC 			5001	/* Agent Class:  Range 5001-6000 */


/* Actions */

#define BILL_SESSION_START		1	/* Session Start record */
#define	BILL_DB_OPEN 			2	/* DB Open */
#define BILL_DB_CLOSE			3	/* DB Close by User  */
#define BILL_SESSION_STAMP		4	/* Periodic session record */
#define BILL_DB_STAMP			5   /* Periodic database stamp */
#define BILL_DB_CLOSE_END		6   /* Physical close when session terminates. */
#define	BILL_SESSION_END		255 /* Session End */


typedef struct						/*  Session Billing Record */
	{
    SESSIONID	SessionID;				/*	Session ID */
	WORD		Action;					/*  Start, Stamp, End */
  	char		Username[MAXUSERNAME+1];/*	Username */
    DWORD       BytesIn;				/*  Bytes read thus far in this session */
    DWORD       BytesOut;				/*  Bytes written thus far in this session */
   	char		NetAdr[MAXNETADR];		/*  IP Address of client */
	} SESSIONREC;

typedef struct						/*  Session Billing Record */
	{
    SESSIONID	SessionID;				/*	Session ID */
    WORD		Action;					/*  DbOpen, Stamp, DbClose */
	char		Username[MAXUSERNAME+1];/*	Username */
    DWORD		DBOpenTime;				/*  DB Open Time */
    TIMEDATE	ReplicaID;				/*  Replica Id,  Database Opened */
   	} DBREC;
    
typedef struct						/*  Session Billing Record message */
	{
    SESSIONID	SessionID;				/*	Session ID */
    DWORD       BytesIn;				/*  Bytes read to replicate this database. */
    DWORD       BytesOut;				/*  Bytes written to replicate this database. */
    TIMEDATE	ReplicaID;				/*  Replica ID */
    char		Source[MAXUSERNAME+1]; /*  Replication source */
    char		Destination[MAXUSERNAME+1];	/* Replication Destination */
    WORD		Priority;				/*  Replication priority*/
	} REPLREC;
    
#define	BILLCHARGEREAD		1
#define BILLCHARGEWRITE		2

typedef struct
	{
	WORD		Type;					/* $ChargeUpdate or $ChargeRead */
   	TIMEDATE    ReplicaID;				/* Database replica id */
    char		Username[MAXUSERNAME+1];/*	Username */
    OID			OriginatorID;			/*  Universally unique Note ID */
	NUMBER		Charge; 				/*  $Charge Fieldd */
    } DOCUMENT;

typedef struct
	{
	char  	FormType[DESIGN_NAME_MAX]; 		/* Type of Form */
	OID  	OriginatorID; 					/* Message Id */
	UNID  	OrigMessageID;					/* $Ref if NonDelivery Report */
	DWORD 	MessageSize;					/* Size of message */
	char 	HopName[MAXUSERNAME+1];			/* Next Server Name */
  	WORD 	Priority; 						/* Delivery priority */
	WORD 	RoutingState;					/* DEAD_STATE */
	WORD 	Report;							/* Delivery report request */
	char 	Originator[MAXUSERNAME+1];		/* From: */
   	WORD 	RecipientCount;					/* Recipients count */
	WORD 	RecipientSize;					/* Recipients string size */
	TIMEDATE PostedDate;					/* Uniquely identify instance of Mail message */
	/* Recipients char string follows structure */
	} MAILREC;

/* Agent Flags */
#define BILL_AGENT_WARNINGS				0x00000001		/* Non-fatal errors encountered */
#define BILL_AGENT_MAIL					0x00000002		/* Triggered by Mail */
#define BILL_AGENT_PUBLIC				0x00000004		/* Public Agent */


typedef struct						/*  Agent Billing Record */
	{
	WORD		ULen;					/*  UserName Len */
	WORD		TLen;					/*  TaskName Len */
	WORD		DLen;					/*  DatabaseName Len */
    DWORD		ElapsedRunTime;		  	/*	Elapsed run-time for agent */
	DWORD		Flags;					/*  Agent Flags */
	/* UserName, TaskName, Database Name strings follow */
   	} AGENTREC;


typedef union
	{
    SESSIONREC	sess;
    REPLREC		repl;
    DOCUMENT	doc;
	MAILREC		mail;
	DBREC		db;
	AGENTREC	agent;
    } BILLREC;
    

typedef struct
	{
	WORD		Len;					/*  Len of Billing Message */
	WORD		StructType;				/*  Type of structure (BILL_SESSIONREC) */
    DWORD		Class;					/*  Class of request (BILL_SESSION) */
    char		ServerName[MAXUSERNAME+1];/*  Server Name*/
    TIMEDATE	TimeStamp;			    /*  Timestamp */
    BILLREC		rec;
	} BILLMSG;

    
STATUS LNPUBLIC BillingWrite (DWORD Class, WORD StructType, WORD Len, void far *pMessage, char far *MQName);

STATUS LNPUBLIC BillingGetClass (DWORD far *BillingClass);


#ifdef __cplusplus
}
#endif

#endif /* BILLING_DEFS */


