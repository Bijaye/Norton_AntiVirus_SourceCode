// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#ifndef NSF_DB_DEFS
#define NSF_DB_DEFS

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NSF_DEFS
#include "nsfdata.h"
#endif

#ifndef EVENT_DEFS
#include "event.h"
#endif


/* Note Storage File Database Definitions */

/*	NSF File Information Buffer size.  This buffer is defined to contain
	Text (host format) that is NULL-TERMINATED.  This is the ONLY null-terminated
	field in all of NSF. */

#define	NSF_INFO_SIZE 128
#define NSF_INFO_WILD_CARD_SERVER_CHAR '*'

/*  Define NSFDbOpenExtended option bits.  These bits select individual
	open options. */

#define DBOPEN_WITH_SCAN_LOCK		0x0001	/* Open with scan lock to prevent
												other opens with scan lock
												(used by replicator) */
#define DBOPEN_PURGE				0x0002	/* DbPurge while opening */
#define DBOPEN_NO_USERINFO			0x0004	/* No user info may be available, so don't ask for it */
#define DBOPEN_FORCE_FIXUP			0x0008	/* Force a database fixup */
#define DBOPEN_FIXUP_FULL_NOTE_SCAN	0x0010	/* Scan all notes and all items (not incremental) */
#define DBOPEN_FIXUP_NO_NOTE_DELETE	0x0020	/* Do not delete bad notes during note scan */
#define DBOPEN_CLUSTER_FAILOVER		0x0080	/* If open fails try cluster failover */
#define DBOPEN_CLOSE_SESS_ON_ERROR	0x0100	/* Close session on error paths */
#define DBOPEN_NOLOG				0x0200	/* don't log errors - used when opening log database! */

#define DBCOMPACT_MAILBOX	0x0080		/* Compact XXXX.BOX for mail router and other MTAs */

/*	Define NSF DB Classes - These all begin with 0xf000 for no good
	reason other than to ENSURE that callers of NSFDbCreate call the
	routine with valid parameters, since in earlier versions of NSF
	the argument to the call was typically 0. */

#define	DBCLASS_BY_EXTENSION	0	/* automatically figure it out */

#define	DBCLASS_NSFTESTFILE			0xff00
#define	DBCLASS_NOTEFILE			0xff01
#define	DBCLASS_DESKTOP				0xff02
#define	DBCLASS_NOTECLIPBOARD		0xff03
#define	DBCLASS_TEMPLATEFILE		0xff04
#define	DBCLASS_GIANTNOTEFILE		0xff05
#define	DBCLASS_HUGENOTEFILE		0xff06
#define	DBCLASS_ONEDOCFILE			0xff07	/* Not a mail message */
#define	DBCLASS_V2NOTEFILE			0xff08
#define	DBCLASS_ENCAPSMAILFILE		0xff09	/* Specifically used by alt mail */
#define	DBCLASS_LRGENCAPSMAILFILE	0xff0a	/* Specifically used by alt mail */
#define	DBCLASS_V3NOTEFILE			0xff0b
#define	DBCLASS_OBJSTORE			0xff0c	/* Object store */
#define DBCLASS_V3ONEDOCFILE		0xff0d
#define	DBCLASS_V4NOTEFILE			0xff0e
#define	DBCLASS_V5NOTEFILE			0xff0f

#define	DBCLASS_MASK			0x00ff
#define	DBCLASS_VALID_MASK		0xff00

/*	Define NSF Special Note ID Indices.  The first 16 of these are reserved
	for "default notes" in each of the 16 note classes.  In order to access
	these, use SPECIAL_ID_NOTE+NOTE_CLASS_XXX.  This is generally used
	when calling NSFDbGetSpecialNoteID. NOTE: NSFNoteOpen, NSFDbReadObject
	and NSFDbWriteObject support reading special notes or objects directly
	(without calling NSFDbGetSpecialNoteID).  They use a DIFFERENT flag
	with a similar name: NOTE_ID_SPECIAL (see nsfnote.h).  Remember this
	rule:

	SPECIAL_ID_NOTE is a 16 bit mask and is used as a NoteClass argument.
	NOTE_ID_SPECIAL is a 32 bit mask and is used as a NoteID or RRV argument.
*/

#define	SPECIAL_ID_NOTE			0x8000	/* use in combination w/NOTE_CLASS 
											when calling NSFDbGetSpecialNoteID */
/*	Get/Set DbOption routines */

STATUS LNPUBLIC NSFDbGetOptions (DBHANDLE hDB, DWORD far *retDbOptions);
STATUS LNPUBLIC NSFDbSetOptions (DBHANDLE hDB, DWORD DbOptions, DWORD Mask);

#define	DBOPTION_FT_INDEX			0x00000001	/* Enable full text indexing */
#define DBOPTION_IS_OBJSTORE		0x00000002	/* TRUE if database is being used
												   as an object store - for garbage collection */
#define DBOPTION_USES_OBJSTORE		0x00000004	/* TRUE if database has notes which refer to an
												   object store - for garbage collection*/
#define DBOPTION_OBJSTORE_NEVER		0x00000008	/* TRUE if NoteUpdate of notes in this db should
												   never use an object store. */
#define DBOPTION_IS_LIBRARY			0x00000010  /* TRUE if database is a library */
#define DBOPTION_UNIFORM_ACCESS		0x00000020	/* TRUE if uniform access control
												   across all replicas */
#define DBOPTION_OBJSTORE_ALWAYS	0x00000040	/* TRUE if NoteUpdate of notes in this db should
												   always try to use an object store. */
#define DBOPTION_NO_BGAGENT			0x00000200	/* TRUE if db has no background agent */
#define DBOPTION_OUT_OF_SERVICE		0x00000400	/* TRUE is db is out-of-service, no new opens allowed,
													unless DBOPEN_IGNORE_OUTOFSERVICE is specified */
#define DBOPTION_IS_PERSONALJOURNAL	0x00000800	/* TRUE if db is personal journal */
#define DBOPTION_MARKED_FOR_DELETE	0x00001000	/* TRUE if db is marked for delete.  no new opens allowed,
													cldbdir will delete the database when ref count = 0 */
#define DBOPTION_HAS_CALENDAR		0x00002000	/* TRUE if db stores calendar events */
#define DBOPTION_IS_CATALOG_INDEX	0x00004000	/* TRUE if db is a catalog index */
#define DBOPTION_IS_ADDRESS_BOOK	0x00008000	/* TRUE if db is an address book */
#define DBOPTION_IS_SEARCH_SCOPE	0x00010000	/* TRUE if db is a "multi-db-search" repository */
#define DBOPTION_IS_UA_CONFIDENTIAL	0x00020000	/* TRUE if db's user activity log is confidential, only
												*  		viewable by designer and manager */
/*	This is the structure used for the Item Definition Table */

typedef struct {
	WORD Length;				/* total length of this buffer */
	WORD Items;					/* number of items in the table */
								/* now come the ITEM_DEFINITION structures */
								/* now comes the packed text */
} ITEM_DEFINITION_TABLE;

typedef struct {
	WORD Spare;					/* unused */
	WORD ItemType;				/* default data type of the item */
	WORD NameLength;			/* length of the item's name */
} ITEM_DEFINITION;

/*	Database Activity Structure */

typedef struct {
	TIMEDATE First;				/* Beginning of reporting period */
	TIMEDATE Last;				/* End of reporting period */
	DWORD Uses;					/* # of uses in reporting period */
	DWORD Reads;				/* # of reads in reporting period */
	DWORD Writes;				/* # of writes in reporting period */
	DWORD PrevDayUses;			/* # of uses in previous 24 hours */
	DWORD PrevDayReads;			/* # of reads in previous 24 hours */
	DWORD PrevDayWrites;		/* # of writes in previous 24 hours */
	DWORD PrevWeekUses;			/* # of uses in previous week */
	DWORD PrevWeekReads;		/* # of reads in previous week */
	DWORD PrevWeekWrites;		/* # of writes in previous week */
	DWORD PrevMonthUses;		/* # of uses in previous month */
	DWORD PrevMonthReads;		/* # of reads in previous month */
	DWORD PrevMonthWrites;		/* # of writes in previous month */
} DBACTIVITY;

typedef struct {
	TIMEDATE Time;				/* Time of record */
	WORD Reads;					/* # of data notes read */
 	WORD Writes;				/* # of data notes written */
	DWORD UserNameOffset;		/* Offset of the user name from the beginning
								   of this memory block */
								/* User names follow -- '\0' terminated */	
} DBACTIVITY_ENTRY;

#define NSFDbGetActivityUserNamePtr(DbActivity, Index) 	\
				(((char FAR *) DbActivity) + DbActivity[Index].UserNameOffset)

/*	Define NSF DB open modes */

#define DB_LOADED 1					/* hDB refers to a normal database file */
#define DB_DIRECTORY 2				/* hDB refers to a "directory" and not a file */

/*	Define argument to NSFDbInfoParse/Modify to manipulate components from DbInfo */

#define INFOPARSE_TITLE			0
#define INFOPARSE_CATEGORIES	1
#define INFOPARSE_CLASS			2
#define INFOPARSE_DESIGN_CLASS	3


/* 	Array entry returned from NSFDbGetReplHistorySummary */

typedef struct {
	TIMEDATE		ReplicationTime;	/*	Time returned from last replciation */
	WORD			AccessLevel;		/*	Access level at time of replication */
	WORD			AccessFlags;		/*	Access flags at time of replication */
	WORD			Direction;			/*	NEVER, SEND, RECEIVE */
	DWORD			ServerNameOffset;	/*	Server name offset in packed data */
	WORD			ServerNameLength;	/*	Server name length in packed data */
	WORD			FileNameLength;		/*	File name length in packed data */
										/*	Total server!!file length is */ 
										/*	ServerNameLength + FileNameLength + 2 */
										/*	to compensate for the !! separator */
	DWORD			Spare [2];			/*	Room for growth */
										/*	Server/file name pairs follow as */
										/*	"<server name>!!<file name>" with */
										/*	NULL after each pairing */
} REPLHIST_SUMMARY;

/*	Types for REPLHISTORY.Direction */

#define	DIRECTION_NEVER		0
#define	DIRECTION_SEND		1
#define	DIRECTION_RECEIVE	2

/*	Macros for accessing packed data following the REPLHISTORY_SUMMARY array.

	NOTE: The server name pointer is NOT null-terminated, but the file name
	pointer is.  This is so the caller can use the "server file" string 
	directly.
*/

#define NSFGetSummaryServerNamePtr(Summary, Index) 	\
				(((char FAR *) Summary) + Summary[Index].ServerNameOffset)
#define NSFGetSummaryFileNamePtr(Summary, Index) 	\
				(((char FAR *) Summary) + Summary[Index].ServerNameOffset + \
					Summary[Index].ServerNameLength + 2)

/*	Option flags for NSFDbGetReplHistorySummary */

#define REPLHIST_REMOVE_WILDCARDS	0x00000001L	/*	Don't copy wild card entries */
#define REPLHIST_SORT_BY_DATE		0x00000002L	/*	Sort by date.  Default is
													by server name */

/* 	Option flags for NSFDbCreateExtended */

#define	DBCREATE_LOCALSECURITY			0x0001
#define	DBCREATE_OBJSTORE_NEVER			0x0002
#define DBCREATE_MAX_SPECIFIED			0x0004

/* Values for EncryptStrength of NSFDbCreateExtended */

#define	DBCREATE_ENCRYPT_NONE			0x00	
#define	DBCREATE_ENCRYPT_SIMPLE			0x01	
#define DBCREATE_ENCRYPT_MEDIUM			0x02
#define DBCREATE_ENCRYPT_STRONG			0x03
STATUS LNPUBLIC NSFDbGetReplHistorySummary (HANDLE hDb, DWORD Flags,
							HANDLE *rethSummary, DWORD * retNumEntries);

/* Option flags for NSFDbCopyCreate */

#define	DBCOPY_REPLICA					0x00000001L
#define	DBCOPY_SUBCLASS_TEMPLATE		0x00000002L
#define	DBCOPY_DBINFO2					0x00000004L	
#define	DBCOPY_SPECIAL_OBJECTS			0x00000008L
#define	DBCOPY_NO_ACL					0x00000010L
#define	DBCOPY_NO_FULLTEXT				0x00000020L
#define DBCOPY_ENCRYPT_SIMPLE			0x00000040L
#define DBCOPY_ENCRYPT_MEDIUM			0x00000080L
#define DBCOPY_ENCRYPT_STRONG			0x00000100L
#define DBCOPY_KEEP_NOTE_MODTIME		0x00000200L

STATUS LNPUBLIC NSFDbCopyExtended (DBHANDLE hSrcDB, DBHANDLE hDstDB, 
						TIMEDATE Since, WORD NoteClassMask, DWORD Flags, 
						TIMEDATE far *retUntil);
STATUS LNPUBLIC NSFDbOpen (char far *PathName, DBHANDLE far *rethDB);
STATUS LNPUBLIC NSFDbOpenExtended (char far *PathName, WORD Options,
								HANDLE hNames,
								TIMEDATE far *ModifiedTime,
								DBHANDLE far *rethDB,
								TIMEDATE far *retDataModified,
								TIMEDATE far *retNonDataModified);
STATUS LNPUBLIC NSFDbClose (DBHANDLE hDB);
STATUS LNPUBLIC NSFDbCreate (char far *PathName, USHORT DbClass, BOOL ForceCreation);
STATUS LNPUBLIC NSFDbCreateObjectStore (char far *PathName, BOOL ForceCreation);
STATUS LNPUBLIC NSFDbDelete (char far *PathName);
STATUS LNPUBLIC NSFDbCreateExtended (char far *PathName, WORD DbClass, 
									 BOOL ForceCreation, WORD Options, 
									 BYTE EncryptStrength, DWORD MaxFileSize);

STATUS LNPUBLIC NSFDbCopy (DBHANDLE hSrcDB, DBHANDLE hDstDB, TIMEDATE Since, WORD NoteClassMask);
STATUS LNPUBLIC NSFDbCopyNote (DBHANDLE hSrcDB, DBID far *SrcDbID, DBID far *SrcReplicaID,
								NOTEID SrcNoteID,
								DBHANDLE hDstDB, DBID far *DstDbID, DBID far *DstReplicaID,
								NOTEID far *retDstNoteID, WORD far *retNoteClass);

STATUS LNPUBLIC NSFDbCreateAndCopy(char far* srcDb,char far* dstDb, WORD NoteClass, WORD limit, DWORD flags, DBHANDLE far* retHandle);
STATUS LNPUBLIC NSFDbMarkForDelete(char far* dbPathPtr);
STATUS LNPUBLIC NSFDbMarkInService(char far* dbPathPtr);
STATUS LNPUBLIC NSFDbMarkOutOfService(char far* dbPathPtr);

STATUS LNPUBLIC NSFDbCopyACL (DBHANDLE hSrcDB, DBHANDLE hDstDB);
STATUS LNPUBLIC NSFDbCopyTemplateACL (DBHANDLE hSrcDB, DBHANDLE hDstDB, char far *Manager, WORD DefaultAccessLevel);
STATUS LNPUBLIC NSFDbCreateACLFromTemplate (DBHANDLE hNTF, DBHANDLE hNSF,
										char far *Manager, WORD DefaultAccess,
										HANDLE far *rethACL);
STATUS LNPUBLIC NSFDbStoreACL (DBHANDLE hDB, HANDLE hACL, DWORD ObjectID, WORD Method);
STATUS LNPUBLIC NSFDbReadACL(DBHANDLE hDB, HANDLE far *rethACL);
STATUS LNPUBLIC NSFDbGenerateOID (DBHANDLE hDB, OID far *retOID);
STATUS LNPUBLIC NSFDbModifiedTime (DBHANDLE hDB, TIMEDATE far *retDataModified, TIMEDATE far *retNonDataModified);
STATUS LNPUBLIC NSFDbPathGet (DBHANDLE hDB, char far *retCanonicalPathName, char far *retExpandedPathName);
STATUS LNPUBLIC NSFDbInfoGet (DBHANDLE hDB, char far *retBuffer);
STATUS LNPUBLIC NSFDbInfoSet (DBHANDLE hDB, char far *Buffer);
void LNPUBLIC   NSFDbInfoParse(char far *Info, WORD What, char far *Buffer, WORD Length);
void LNPUBLIC   NSFDbInfoModify(char far *Info, WORD What, char far *Buffer);
STATUS LNPUBLIC NSFDbGetSpecialNoteID (DBHANDLE hDB, WORD Index, NOTEID far *retNoteID);
STATUS LNPUBLIC NSFDbIDGet (DBHANDLE hDB, DBID far *retDbID);
STATUS LNPUBLIC NSFDbReplicaInfoGet (DBHANDLE hDB, DBREPLICAINFO far *retReplicationInfo);
STATUS LNPUBLIC NSFDbReplicaInfoSet (DBHANDLE hDB, DBREPLICAINFO far *ReplicationInfo);

STATUS LNPUBLIC NSFDbGetNoteInfo (DBHANDLE hDB, NOTEID NoteID,
									OID far *retNoteOID,
									TIMEDATE far *retModified,
									WORD far *retNoteClass);

STATUS LNPUBLIC NSFDbQuotaGet (char far *Filename, DBQUOTAINFO far *retQuotaInfo);

STATUS LNPUBLIC NSFDbGetNoteInfoByUNID(HANDLE hDB, UNID far *pUNID, 
									NOTEID far *retNoteID, OID far *retOID, 
									TIMEDATE far *retModTime, WORD far *retClass);
STATUS LNPUBLIC NSFDbGetModifiedNoteTable (DBHANDLE hDB, WORD NoteClassMask, TIMEDATE Since,
									TIMEDATE far *retUntil,
									HANDLE far *rethTable);
STATUS LNPUBLIC NSFApplyModifiedNoteTable (HANDLE hModifiedNotes, HANDLE hTargetTable);
STATUS LNPUBLIC NSFDbLocateByReplicaID (DBHANDLE hDB, DBID far *ReplicaID, char far *retPathName, WORD PathMaxLen);

STATUS LNPUBLIC NSFDbStampNotes (DBHANDLE hDB, HANDLE hTable,
									char far *ItemName, WORD ItemNameLength,
									void far *Data, WORD DataLength);
STATUS LNPUBLIC NSFDbDeleteNotes (DBHANDLE hDB, HANDLE hTable, UNID far *retUNIDArray);
STATUS LNPUBLIC NSFDbUserNameGet (DBHANDLE hDB, char far *retUserName, WORD BufferLength);
void LNPUBLIC NSFDbAccessGet(HANDLE hDB, WORD far *retAccessLevel, WORD far *retAccessFlag);
STATUS LNPUBLIC NSFDbClassGet (DBHANDLE hDB, WORD far *retClass);
STATUS LNPUBLIC NSFDbModeGet (DBHANDLE hDB, USHORT far *retMode);
STATUS LNPUBLIC NSFDbGetUserActivity(DBHANDLE hDB, DWORD Flags, DBACTIVITY far *retDbActivity,
									HANDLE far *rethUserInfo, WORD far *retUserCount);
STATUS LNPUBLIC NSFDbCloseSession (DBHANDLE hDB);
STATUS LNPUBLIC NSFDbReopen (DBHANDLE hDB, DBHANDLE far *rethDB);
STATUS LNPUBLIC NSFDbMajorMinorVersionGet (DBHANDLE hDB, WORD far *retMajorVersion, 
											 WORD far *retMinorVersion);
STATUS LNPUBLIC NSFDbItemDefTable (DBHANDLE hDB,ITEMDEFTABLEHANDLE far *retItemNameTable);
STATUS LNPUBLIC NSFDbGetBuildVersion (DBHANDLE hDB,WORD far *retVersion);
STATUS LNPUBLIC NSFDbSpaceUsage (DBHANDLE hDB, DWORD far *retAllocatedBytes, DWORD far *retFreeBytes);
DWORD LNPUBLIC NSFDbGetOpenDatabaseID (DBHANDLE hDB);
STATUS LNPUBLIC NSFGetServerStats(char far *ServerName, char far *Facility, char far *StatName, HANDLE far *rethTable, DWORD far *retTableSize);
STATUS LNPUBLIC NSFGetServerLatency(char far *ServerName, DWORD Timeout,
								DWORD far *retClientToServerMS,
								DWORD far *retServerToClientMS,
								WORD far *ServerVersion);
STATUS LNPUBLIC NSFRemoteConsole(char far *ServerName, char far *ConsoleCommand, HANDLE far *hResponseText);
STATUS LNPUBLIC NSFDbUpdateUnread (DBHANDLE hDataDB, HANDLE hUnreadList);

STATUS LNPUBLIC NSFDbGetUnreadNoteTable(DBHANDLE hDB,
						char far *UserName, WORD UserNameLength, BOOL fCreateIfNotAvailable,
						HANDLE far *rethUnreadList);

STATUS LNPUBLIC NSFDbSetUnreadNoteTable(DBHANDLE hDB, char far *UserName, WORD UserNameLength,
						BOOL fFlushToDisk, HANDLE hOriginalUnreadList, HANDLE hUnreadList);
STATUS LNPUBLIC NSFDbGetObjectStoreID(DBHANDLE dbhandle, BOOL far *Specified, DBID far *ObjStoreReplicaID);
STATUS LNPUBLIC NSFDbSetObjectStoreID(DBHANDLE dbhandle, DBID far *ObjStoreReplicaID);
STATUS LNPUBLIC NSFDbFilter(DBHANDLE hFilterDB, NOTEHANDLE hFilterNote,
								HANDLE hNotesToFilter, BOOL fIncremental,
								void far *Reserved1, void far *Reserved2,
								char far *DbTitle, char far *ViewTitle,
								void far *Reserved3, void far *Reserved4,
								HANDLE hDeletedList,
								HANDLE hSelectedList);
STATUS LNPUBLIC NSFDbCompact(char far *Pathname, WORD Options, DWORD far *retStats);

/*	Database Hook Drivers
*
* NSF hook drivers enable the API programmer to construct a set of
* subroutines, packaged as a DLL, that "hook" several NSF functions
* related to reading, modifying, and deleting notes.  The principal
* application of these hook drivers is intended to be auditing, that is,
* logging accesses to a database, however they can also be programmed
* to modify the contents of the notes and/or refuse requests.
*
* Hook Driver Packaging and Concurrency Requirements
*
* Like all Notes low-level subsystems, hook drivers must obey strict 
* rules to fit into their computing environment:
*
*    1. The drivers should be available on multiple computing platforms
*
*    2. The drivers are packaged as Dynamic Link Libraries under 
*    Windows, NT, and OS/2. Under Windows, the DEF file must specify the data 
*    as being MOVEABLE SINGLE so that it doesn't occupy GlobalDOS memory, and 
*    because Windows doesn't support multi-instance dynamic link libraries. 
*    For NT and OS/2, we recommend MULTIPLE data, but you can use whatever you 
*    wish.
*
*        For all four environments, Notes uses either LoadLibrary or 
*    DosLoadModule to load the driver. You must decide upon a mnemonic name 
*    for your driver, a maximum of 7 characters that is preferably 
*    nationality-neutral (obscure in all languages), such as NTAUDIT. In order
*    to load the library, Notes prefixes this name with 
*    _ under Windows, $ under OS/2 1.x, I under OS/2 2, and N under NT, 
*    and post-fixes the name with .DLL. The DLL is installed into Notes by 
*    simply placing its executable into the Notes Program Directory.
*
*        Notes automatically loads the hook drivers at initialization.
*    The list of hook drivers to be loaded is specified in NOTES.INI in a
*    variable called NSF_HOOKS with a comma separator. Thus, in order to load
*    three hook drivers, the variable might be specified as
*    NSF_HOOKS=drivera,driverb,driverc. Note that it isn't necessary to
*    specify the prefix character or the suffix .DLL.
*
*        Because of instantiation requirements of the operating systems, 
*    the library is loaded once per process. At load time, the first entry 
*    point address (entry point @1) is obtained by ordinal, and the driver's 
*    Init() function is invoked. The principal use of this function is to plug 
*    a data structure with vectors to other hook driver subroutines, 
*    including its Term() function which will be called by Notes when the 
*    process exits (or performs NotesTerm()).
*
*        Remember that in some environments (such as Windows), you must
*    export, in your DEF file, all of your functions that Notes will call
*    through these vectors. This is because Windows uses the fact that it's
*    exported to generate a thunk that sets up the DLL's DS appropriately on
*    all of these entry points.
*
*    3. All code executed by all entry points to the drivers must 
*    be 100% re-entrant by both multiple threads within a single process and 
*    multiple processes. In Windows, if you Yield() or make a call to a
*    Notes API function, you may also be preempted by another process that
*    does a hook driver call, so reentrancy is a requirement even in Windows.
*    (Notes makes heavy use of multiple threads and multiple processes, so
*    this must be treated as a required feature, not as something that can
*    be done "later".)
*
* Following is the data structure passed to your hook driver's Init routine.
* When your routine is called, the hModule is filled in (just in case you
* need it to load resources), and the rest of the vectors are zero'ed.
* If you don't need to hook any of these vectors, leave them NULL.  Otherwise,
* plug the vectors in your Init routine.
*
*/

typedef struct dbhookvec
	{

	/*	Module handle of the hook library */

	HMODULE hModule;

	/*	Initialization vector.  This is simply a pointer to the routine
		exported as ordinal #1 in your DLL.  You needn't plug this
		vector because it's plugged before calling through it to do
		driver initializations.  This routine is called once for each
		process that does a NotesInit(). */

	STATUS (LNCALLBACKPTR Init)(struct dbhookvec far *vec);

	/*	Termination vector.  This is called once for each process that
		did an Init call, just prior to the process's exiting.  */

	STATUS (LNCALLBACKPTR Term)(struct dbhookvec far *vec);

	/*	Note open hook.  This routine is called just AFTER a note is opened
		by the NSF subsystem.  It is called for both local and remote
		databases.  UserName is the name of the user doing the open.
		GroupList may be NULL, but when it's not it contains a list of
		the groups that the user is in.  hDB and NoteID can be used
		to identify the database and note (use NSFDbPathGet to get the
		pathname of the database).  OpenFlags are the flags originally
		passed into NSFNoteOpen.  hNote is the handle to the note that
		is about to be returned to the caller.  Your hook driver may
		modify the contents of the note, or may return an error from
		this routine if you wish to fail the open call.  You may use any
		standard Notes error code to fail the call. */

	STATUS (LNCALLBACKPTR NoteOpen)(struct dbhookvec far *vec, char far *UserName, LIST far *GroupList, DBHANDLE hDB, NOTEID NoteID, NOTEHANDLE hNote, WORD OpenFlags);

	/*	Note add/update hook, also handles deletion
		This routine is called just BEFORE a note is updated
		by the NSF subsystem.  It is called for both local and remote
		databases.  UserName is the name of the user doing the open.
		GroupList may be NULL, but when it's not it contains a list of
		the groups that the user is in.  hDB and NoteID can be used
		to identify the database and note (use NSFDbPathGet to get the
		pathname of the database).  pUpdateFlags is a pointer to a WORD
		of UpdateFlags originally passed into NSFNoteUpdate.  The hook
		may examine and modify the UpdateFlags WORD before returning,
		thus modifying the flags that will be passed into NSFNoteUpdate.
		hNote is the handle to the note that is about to be updated.
		Your hook driver may modify the contents of the note, or may
		return an error from this routine if you wish to fail the update
		call.  You may use any standard Notes error code to fail the call.
		Note that you can use the following algorithm to determine
		precisely the operation being performed:

		*	if (NoteID == NOTEID_ADD
		*			|| NoteID == NOTEID_ADD_OR_REPLACE
		*			|| NoteID == NOTEID_ADD_UNID)
		*		{
		*		if ((*pUpdateFlags) & UPDATE_DELETED)
		*			; ** Adding a new "deleted note stub" to a database **
		*		else
		*			; ** Adding a new document to a database **
		*		}
		*	else
		*		{
		*		if ((*pUpdateFlags) & UPDATE_DELETED)
		*			; ** Deleting an existing document from the database. **
		*			  ** Note that in this case, the contents of the hNote **
		*			  ** may be nil and should be disregarded; because of **
	    *			  ** this, there's no way to tell the class of a note **
		*			  ** being deleted at this point **
		*		else
		*			; ** Updating an existing document in the database **
		*		}

		*/

	STATUS (LNCALLBACKPTR NoteUpdate)(struct dbhookvec far *vec, char far *UserName, LIST far *GroupList, DBHANDLE hDB, NOTEID NoteID, NOTEHANDLE hNote, WORD far *pUpdateFlags);

	/*	Note stamp (Categorization) hook.
		This routine is called just BEFORE a set of notes is Categorized
		by the NSF subsystem.  It is called for both local and remote
		databases.  UserName is the name of the user doing the categorization.
		GroupList may be NULL, but when it's not it contains a list of
		the groups that the user is in.  hDB and hTable can be used
		to identify the database and notes (use NSFDbPathGet to get the
		pathname of the database).  ItemName and Data indicate the data
		being used in the categorization.  Your hook routine may return
		an error if you wish to fail the Stamp call.  You may use any
		standard Notes error code to fail the call. */

	STATUS (LNCALLBACKPTR DbStampNotes)(struct dbhookvec far *vec, char far *UserName, LIST far *GroupList, DBHANDLE hDB, HANDLE hIDTable, char far *ItemName, WORD ItemNameLength, void far *Data, WORD Length);

	/*	Flags used by Notes to describe the hook driver -- RESERVED */

	DWORD	Flags;

	} DBHOOKVEC;


/* End of Note Storage File Database Definitions */

#ifdef __cplusplus
}
#endif

#endif
