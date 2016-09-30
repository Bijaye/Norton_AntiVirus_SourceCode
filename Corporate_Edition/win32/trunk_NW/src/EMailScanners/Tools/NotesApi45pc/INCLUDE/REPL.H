// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif

/*  Public definitions for Database Replicator */

#ifndef REPLR_DEFS
#define REPLR_DEFS


/* Wild Card Server Name */
#define WILD_CARD_SERVER_NAME "$$WildCardServer"


/*  Options used when calling ReplicateWithServer */

#define REPL_OPTION_RCV_NOTES		0x00000001	/* Receive notes from server (pull) */
#define REPL_OPTION_SEND_NOTES		0x00000002	/* Send notes to server (push) */
#define REPL_OPTION_ALL_DBS			0x00000004	/* Replicate all database files */
#define REPL_OPTION_CLOSE_SESS		0x00000040	/* Close sessions when done */
#define REPL_OPTION_ALL_NTFS		0x00000400	/* Replicate NTFs as well */
#define REPL_OPTION_PRI_LOW			0x00000000	/* Low, Medium, & High priority databases*/
#define REPL_OPTION_PRI_MED			0x00004000	/* Medium & High priority databases only*/
#define REPL_OPTION_PRI_HI			0x00008000	/* High priority databases only*/
#define REPL_OPTION_ALL_FILES		(REPL_OPTION_ALL_DBS | REPL_OPTION_ALL_NTFS)

/*	Replication statistics, returned by replicator */

typedef struct
	{
	long	TotalFiles;
	long	FilesCompleted;
	long	NotesAdded;
	long	NotesDeleted;
	long	NotesUpdated;
	long	Successful;
	long	Failed;
	long	NumberErrors;
	} REPLFILESTATS;

typedef struct
	{
	REPLFILESTATS Pull;
	REPLFILESTATS Push;
	long	StubsInitialized;
	long	TotalUnreadExchanges;
	long	NumberErrors;
	STATUS	LastError;
	} REPLSERVSTATS;



/* Replication extension structure to allow upwardly compatible changes
   with arbitrary additional parameters in ReplicateWithServer.  Only 
   TimeLimit is defined for V4.  The parameter corresponding to this
   structure in ReplicateWithServer, ExtendedOptions, may be set to 
   NULL as a shorthand for passing a completely zeroed structure. 

*/



typedef struct
    {
	WORD		Size;			/* sizeof(REPLEXTENSIONS), allows for future expansion */
	WORD    	TimeLimit;		/* If non-zero, number of minutes replication
	                        		is allowed to execute before cancellation.
									If not specified, no limit is imposed */
	} REPLEXTENSIONS;

/*	Replicator entry points */

STATUS LNPUBLIC	 ReplicateWithServerExt(char far *PortName,
										char far *ServerName, 
										DWORD Options,
										WORD NumFiles, 
										char far *FileList,
										REPLEXTENSIONS far *ExtendedOptions,
										REPLSERVSTATS far *retStats);

STATUS LNPUBLIC	 ReplicateWithServer(char far *PortName,
										char far *ServerName, 
										WORD Options,
										WORD NumFiles, 
										char far *FileList,
										REPLSERVSTATS far *retStats);

#endif

#ifdef __cplusplus
}
#endif
