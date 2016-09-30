// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef NSF_DATA_DEFS
#define NSF_DATA_DEFS

#ifdef __cplusplus
extern "C" {
#endif

/* Note Storage File Data Definitions */

/*	Define some synonyms for self-documentation of templates */

#define DBHANDLE HANDLE
#define NOTEHANDLE HANDLE
#define ITEMDEFTABLEHANDLE HANDLE
#define FORMULAHANDLE HANDLE

/*	Reserved NoteIDs */

#define NOTEID_RESERVED		0x80000000L		/*	Reserved Note ID, used for 
												categories in NIFReadEntries
												and for deleted notes in a
												lot of interfaces. */
#define NOTEID_ADD			0x00000000L		/*	Reserved NoteID used as input
												to NoteUpdate, to add a new
												note (gets error if UNID assigned
												to new note already exists). */
#define NOTEID_ADD_OR_REPLACE 0x80000000L	/*	Reserved NoteID used as input
												to NoteUpdate, to update if
												note UNID already exists, or
												add note if doesn't exist. */
#define NOTEID_ADD_UNID		0x80000001L		/*	Reserved NoteID used as input
												to NoteUpdate.  Try to preserve
												the specified note UNID, but if
												it already exists, assign a new
												one.  (Note that the UNID in the
												hNote IS updated.) */

/*	An RRV "file position" is defined to be a DWORD, 4 bytes long. */

#define RRV_ALIGNMENT 4L			/* most typical RRV alignment (DBTABLE.C) */
#define	RRV_DELETED NOTEID_RESERVED	/* indicates a deleted note (DBTABLE.C) */

#define	NOTEID_NO_PARENT	0x00000000L		/*	Reserved Note ID, used to indicate
												that this note has no parent in the
												response hierarchy. */

/*	This is the structure that identifies a database.  It is used for both
	the creation date/time and the originator date/time. */

typedef TIMEDATE DBID;

/*	This is the structure that identifies a note within a database.  It is
	simply a file position (RRV) that is guaranteed never to change WITHIN
	this file.  A replicated note, however, may have a different note id
	in two separate files. */

typedef DWORD NOTEID;

/*	This is the structure that identifies ALL replicas of the same note.  The
	"File" member contains a totally unique (random) number, generated at
	the time the note is created.  The "Note" member contains the date/time
	when the very first copy of the note was stored into the first NSF.  The
	"Sequence" member is a sequence number used to keep track of the most
	recent version of the note for replicated data purposes.  The
	"SequenceTime" member is a sequence number qualifier, that allows the
	replicator to determine which note is later given identical Sequence's. 
	Both are required for the following reason.  The sequence number is needed
	to prevent someone from locking out future edits by setting the time/date
	to the future.  The sequence time qualifies the sequence number for two
	reasons:  1)  It prevents two concurrent updates from looking like
	no update at all and 2) it forces all systems to reach the same decision
	as to which update is the "latest" version.

	Time/dates associated with notes:

		OID.Note				Timedate when the note was created
								Obtained by NSFNoteGetInfo(_NOTE_OID) or
								OID in SEARCH_MATCH.
		OID.SequenceTime		Timedate of last revision
								Obtained by NSFNoteGetInfo(_NOTE_OID) or
								OID in SEARCH_MATCH.
		NOTE.EditModified		Timedate when added to (or last updated in) 
								this database.
								(Obtained by NSFNoteGetInfo(_NOTE_MODIFIED) or
								ID.Note in SEARCH_MATCH.


*/

#define	OID_SEQNO_MASK		0x0000FFFFL	/* Mask used to extract sequence # */
#define	OID_NO_REPLICATE	0x80000000L	/* Never replicate outward, currently used ONLY for deleted stubs */

typedef struct {
	DBID File;					/* Unique (random) number */
								/* (Even though this field is called "File", */
								/*  it doesn't have anything to do with the file!) */
	TIMEDATE Note;				/* Original Note Creation time/date */
								/* (THE ABOVE 2 FIELDS MUST BE FIRST - UNID */
								/*  COPIED FROM HERE ASSUMED AT OFFSET 0) */
	DWORD Sequence;				/* LOW ORDER: sequence number, 1 for first version */
								/* HIGH ORDER WORD: flags, as above */
	TIMEDATE SequenceTime;		/* time/date when sequence number was bumped */
} ORIGINATORID;					/* 28 bytes */

#define OID ORIGINATORID


/*  Replication flags 

	NOTE:  Please note the distinction between REPLFLG_DISABLE and
	REPLFLG_NEVER_REPLICATE.  The former is used to temporarily disable
	replication.  The latter is used to indicate that this database should
	NEVER be replicated.  The former may be set and cleared by the Notes
	user interface.  The latter is intended to be set programmatically
	and SHOULD NEVER be able to be cleared by the user interface.

	The latter was invented to avoid having to set the replica ID to
	the known value of REPLICA_ID_NEVERREPLICATE.  This latter method has
	the failing that DBs that use it cannot have DocLinks to them.  */

/*								0x0001	spare was COPY_ACL */
/*								0x0002	spare */
#define REPLFLG_DISABLE			0x0004	/* Disable replication */
#define REPLFLG_IGNORE_DELETES	0x0010	/* Don't propagate deleted notes when
											replicating from this database */
#define REPLFLG_HIDDEN_DESIGN	0x0020	/* UI does not allow perusal of Design */
#define REPLFLG_DO_NOT_CATALOG	0x0040	/* Do not list in catalog */
#define	REPLFLG_CUTOFF_DELETE	0x0080	/* Auto-Delete documents prior to cutoff date */
#define REPLFLG_NEVER_REPLICATE	0x0100	/* DB is not to be replicated at all */
#define REPLFLG_ABSTRACT		0x0200	/* Abstract during replication */
#define REPLFLG_DO_NOT_BROWSE	0x0400	/* Do not list in database add */
#define REPLFLG_NO_CHRONOS		0x0800	/* Do not run chronos on database */
#define REPLFLG_IGNORE_DEST_DELETES	0x1000	/* Don't replicate deleted notes
											 into destination database */
#define REPLFLG_MULTIDB_INDEX	0x2000	/* Include in Multi Database indexing */
#define REPLFLG_PRIORITY_LOW	0xC000	/* Low priority */
#define REPLFLG_PRIORITY_MED	0x0000	/* Medium priority */
#define REPLFLG_PRIORITY_HI		0x4000	/* High priority */
#define REPLFLG_PRIORITY_SHIFT	14		/* Shift count for priority field */
#define REPLFLG_PRIORITY_MASK	0x0003	/* Mask for priority field after shifting*/
#define REPLFLG_PRIORITY_INVMASK 0x3fff	/* Mask for clearing the field */
#define REPLFLG_USED_MASK		(0x4|0x8|0x10|0x40|0x80|0x100|0x200|0xC000|0x1000|0x2000|0x4000)


/*  Replication priority values are stored in the high bits of the
	replication flags.  The stored value is biased by -1 so that
	an encoding of 0 represents medium priority (-1 is low and +1 is high).
	The following macros make getting and setting the priority easy. 
	They return and accept normalized values of 0 - 2. */

#define REPL_GET_PRIORITY(Flags) \
	(((Flags >> REPLFLG_PRIORITY_SHIFT)+1) & REPLFLG_PRIORITY_MASK)
#define REPL_SET_PRIORITY(Pri) \
	(((Pri - 1) & REPLFLG_PRIORITY_MASK) << REPLFLG_PRIORITY_SHIFT)

/*  Reserved ReplicaID.Date.  Used in ID.Date field in ReplicaID to escape
	to reserved REPLICA_ID_xxx
*/
#define REPLICA_DATE_RESERVED		0		/* If used, see REPLICA_ID_xxx */

/*  Known Replica IDs.  Used in ID.Time field in ReplicaID.  Date
	subfield must be REPLICA_DATE_RESERVED).  NOTE:  If you add to this list,
	you should check the code in \catalog\search.c to see if the new one(s)
	need to be added to that code (probably not - but worth checking).

	The format is as follows.  Least sig. byte is version number.  2nd
	byte represents package code but is hard-coded to protect against
	changes in the package code.  Most sig. 2 bytes are reserved for future
	use.
*/
#define REPLICA_ID_UNINITIALIZED	0x00000000 /*  Uninitialized ID */
#define REPLICA_ID_CATALOG			0x00003301 /* Database Catalog (Version 2) */
#define REPLICA_ID_EVENT			0x00003302 /* Stats & Events Config DB */


/*  The following known replica ID is now obsolete.  Although the replicator
	still supports it, the problem is that DBs that use it cannot have  
	DocLinks to them.  Instead use the replica flag REPLFLG_NEVER_REPLICATE. */
#define REPLICA_ID_NEVERREPLICATE	0x00001601 /* Do not allow replicas */

/*	Number of times within cutoff interval that we purge deleted stubs.
	For example, if the cutoff interval is 90 days, we purge every 30
	days. */

#define	CUTOFF_CHANGES_DURING_INTERVAL 3

/*	This is the structure that identifies a replica database. */

typedef struct {
	TIMEDATE ID;				/* ID that is same for all replica files */
	WORD Flags; 				/* Replication flags */
	WORD CutoffInterval;		/* Automatic Replication Cutoff Interval (Days) */
	TIMEDATE Cutoff;			/* Replication cutoff date */
} DBREPLICAINFO;


typedef struct {
	DWORD WarningThreshold; /* Database size warning threshold in kbyte units */
	DWORD SizeLimit;		/* Database size limit in kbyte units */
	DWORD CurrentDbSize;	/* Current size of database (in kbyte units) */
	DWORD MaxDbSize;		/* Max database file size possible (in kbyte units) */
} DBQUOTAINFO;


/*	This is the structure that globally identifies an INSTANCE of a note,
	that is, if we are doing a SEARCH_ALL_VERSIONS, the one with the
	latest modification date is the one that is the "most recent" instance. */

typedef struct {
	DBID File;					/* database Creation time/date */
	TIMEDATE Note;				/* note Modification time/date */
	NOTEID NoteID;				/* note ID within database */
} GLOBALINSTANCEID;

/*	This is the structure that universally (across all servers) describes
	a note (ALL INSTANCES of the same note), but without the information
	necessary to directly access the note in a given database.  It is used
	for referencing a specific note from another note (response notes and
	hot buttons are examples of its use) by storing this structure in the
	referencing note itself.  It is intended to work properly on any server,
	and even if the note being referenced is updated.  Matching of notes
	to other notes is done via the NIF machinery. */

typedef struct {
	DBID File;					/* Unique (random) number */
								/* (Even though this field is called "File", */
								/*  it doesn't have anything to do with the file!) */
	TIMEDATE Note;				/* Original Note Creation time/date */
} UNIVERSALNOTEID;

#define UNID UNIVERSALNOTEID

/*	This is the structure that universally (across all servers) describes
	a note LINK. */

typedef struct {
	TIMEDATE File;					/* File's replica ID */
	UNID View;						/* View's Note Creation TIMEDATE */
	UNID Note;						/* Note's Creation TIMEDATE */
} NOTELINK;


/*	Data Type Definitions. */


/*	Class definitions.  Classes are defined to be the
	"generic" classes of data type that the internal formula computation
	mechanism recognizes when doing recalcs. */

#define	CLASS_NOCOMPUTE				(0 << 8)
#define	CLASS_ERROR					(1 << 8)
#define	CLASS_UNAVAILABLE			(2 << 8)
#define CLASS_NUMBER				(3 << 8)
#define CLASS_TIME					(4 << 8)
#define CLASS_TEXT					(5 << 8)
#define	CLASS_FORMULA				(6 << 8)
#define	CLASS_USERID				(7 << 8)

#define CLASS_MASK					0xff00

/*  All datatypes below are passed to NSF in either host (machine-specific
	byte ordering and padding) or canonical form (Intel 86 packed form).
	The format of each datatype, as it is passed to and from NSF functions,
	is listed below in the comment field next to each of the data types.
	(This host/canonical issue is NOT applicable to Intel86 machines,
	because on that machine, they are the same and no conversion is required).
	On all other machines, use the ODS subroutine package to perform
	conversions of those datatypes in canonical format before they can
	be interpreted. */

/*	"Computable" Data Types */

#define	TYPE_ERROR					0 + CLASS_ERROR			/* Host form */
#define	TYPE_UNAVAILABLE			0 + CLASS_UNAVAILABLE	/* Host form */
#define TYPE_TEXT					0 + CLASS_TEXT			/* Host form */
#define TYPE_TEXT_LIST				1 + CLASS_TEXT			/* Host form */
#define TYPE_NUMBER					0 + CLASS_NUMBER		/* Host form */
#define TYPE_NUMBER_RANGE			1 + CLASS_NUMBER		/* Host form */
#define TYPE_TIME					0 + CLASS_TIME			/* Host form */
#define TYPE_TIME_RANGE				1 + CLASS_TIME			/* Host form */
#define TYPE_FORMULA				0 + CLASS_FORMULA		/* Canonical form */
#define	TYPE_USERID					0 + CLASS_USERID		/* Host form */

/*	"Non-Computable" Data Types */

#define	TYPE_INVALID_OR_UNKNOWN		0 + CLASS_NOCOMPUTE		/* Host form */
#define	TYPE_COMPOSITE				1 + CLASS_NOCOMPUTE		/* Canonical form, >64K handled by more than one item of same name concatenated */
#define TYPE_COLLATION				2 + CLASS_NOCOMPUTE		/* Canonical form */
#define TYPE_OBJECT					3 + CLASS_NOCOMPUTE		/* Canonical form */
#define TYPE_NOTEREF_LIST			4 + CLASS_NOCOMPUTE		/* Host form */
#define TYPE_VIEW_FORMAT			5 + CLASS_NOCOMPUTE		/* Canonical form */
#define TYPE_ICON					6 + CLASS_NOCOMPUTE		/* Canonical form */
#define TYPE_NOTELINK_LIST			7 + CLASS_NOCOMPUTE		/* Host form */
#define TYPE_SIGNATURE				8 + CLASS_NOCOMPUTE		/* Canonical form */
#define TYPE_SEAL					9 + CLASS_NOCOMPUTE		/* Canonical form */
#define TYPE_SEALDATA				10 + CLASS_NOCOMPUTE	/* Canonical form */
#define TYPE_SEAL_LIST				11 + CLASS_NOCOMPUTE	/* Canonical form */
#define TYPE_HIGHLIGHTS				12 + CLASS_NOCOMPUTE	/* Host form */
#define TYPE_WORKSHEET_DATA			13 + CLASS_NOCOMPUTE	/* Used ONLY by Chronicle product */
															/* Canonical form */
#define TYPE_USERDATA				14 + CLASS_NOCOMPUTE	/* Arbitrary format data - see format below */
															/* Canonical form */
#define TYPE_QUERY					15 + CLASS_NOCOMPUTE	/* Saved query CD records; Canonical form */
#define TYPE_ACTION					16 + CLASS_NOCOMPUTE	/* Saved action CD records; Canonical form */
#define TYPE_ASSISTANT_INFO			17 + CLASS_NOCOMPUTE	/* Saved assistant info */
#define TYPE_VIEWMAP_DATASET		18 + CLASS_NOCOMPUTE	/* Saved ViewMap dataset; Canonical form */
#define TYPE_VIEWMAP_LAYOUT			19 + CLASS_NOCOMPUTE	/* Saved ViewMap layout; Canonical form */
#define TYPE_LSOBJECT				20 + CLASS_NOCOMPUTE	/* Saved LS Object code for an agent.	*/
#define	TYPE_HTML					21 + CLASS_NOCOMPUTE	/* LMBCS-encoded HTML, >64K handled by more than one item of same name concatenated */
#define TYPE_SCHED_LIST				22 + CLASS_NOCOMPUTE	/* Busy time schedule entries list; Host form */
#define TYPE_CALENDAR_FORMAT		24 + CLASS_NOCOMPUTE	/* Canonical form */

/*	This is the structure used for summary buffers */

typedef struct {
	USHORT Length;				/* total length of this buffer */
	USHORT Items;				/* number of items in the table */
								/* now come the ITEMs */
								/* now comes the packed text */
} ITEM_TABLE;

typedef struct {				/* used for item names and values */
	USHORT NameLength;			/* length of the item's name */
	USHORT ValueLength;			/* length of the value field */
} ITEM;

typedef struct {
	USHORT Length;				/* total length of this buffer */
	USHORT Items;				/* number of items in the table */
								/* now comes an array of WORDS representing
									the lengths of the item names. */
								/* now comes the item names as packed text */
} ITEM_NAME_TABLE;

typedef struct {
	USHORT Length;				/* total length of this buffer */
	USHORT Items;				/* number of items in the table */
								/* now comes an array of WORDS representing
									the lengths of the item values. */
								/* now comes the item values as packed bytes */
} ITEM_VALUE_TABLE;

/*	Here is the format of the various LIST data types:

	TYPE_TEXT_LIST:

		LIST				* list header *
		USHORT ...			* array of text string lengths following *
		text				* now comes the packed text for all strings *

	TYPE_NUMBER_RANGE:

		RANGE				* range header *
		NUMBER ...			* array of NUMBERs *
		NUMBER_PAIR ...		* array of NUMBER_PAIRs *

	TYPE_TIME_RANGE:

		RANGE				* range header *
		TIMEDATE ...		* array of time/date's *
		TIMEDATE_PAIR ...	* array of time/date pairs *

	TYPE_NOTEREF_LIST:

		LIST				* list header *
		UNIVERSALNOTEID		* array of UNIVERSALNOTEIDs *


	TYPE_NOTELINK_LIST:

		LIST				* list header *
		NOTELINK			* array of NOTELINKs *
		
	TYPE_USERDATA:

		BYTE Length			* length of LMBCS "format-name" string *
		char[Length];		* LMBCS "format-name" string used to distinguish *
							* various formats of user data that follows. *
							* ("format-name" string is NOT NULL-TERMINATED!) *
		data				* next is variable-length data that corresponds *
							* to the format specified by the string *

*/

/*	This is the data structure used for TYPE_OBJECT values */

typedef struct {
	WORD ObjectType;			/* Type of object (OBJECT_xxx) */
	DWORD RRV;					/* Object ID of the object in THIS FILE */
} OBJECT_DESCRIPTOR;

/*	These must be OR-ed into the ObjectType below in order to get the
	desired behavior.  Note that OBJECT_COLLECTION implicitly has
	both of these bits implied, because that was the desired behavior
	before these bits were invented. */

#define	OBJECT_NO_COPY	0x8000	/* do not copy object when updating to new note or database */
#define	OBJECT_PRESERVE	0x4000	/* keep object around even if hNote doesn't have it when NoteUpdating */

/*	Object Types, a sub-category of TYPE_OBJECT */

#define	OBJECT_FILE					0		/* File Attachment */
#define OBJECT_FILTER_LEFTTODO 		3		/* IDTable of "done" docs attached to filter */
#define OBJECT_ASSIST_RUNDATA		8		/* Assistant run data object */
#define OBJECT_UNKNOWN	0xffff		/* Used as input to NSFDbCopyObject, */
									/* NSFDbGetObjectInfo and NSFDbGetObjectSize. */

/*	File Attachment definitions */

#define HOST_MASK		0x0f00	/* used for NSFNoteAttachFile Encoding arg */
#define	HOST_MSDOS		(0 << 8)/* CRNL at EOL, optional ^Z at EOF */
#define	HOST_OLE		(1 << 8)/* unknown internal representation, up to app */
#define	HOST_MAC		(2 << 8)/* potentially has resource forks, etc. */
#define	HOST_UNKNOWN	(3 << 8)/* came inbound thru a gateway */
#define HOST_HPFS		(4 << 8)/* HPFS. Contains EAs and long filenames */
#define	HOST_OLELIB		(5 << 8)/* OLE 1 Library encapsulation */
#define HOST_BYTEARRAY_EXT	(6 << 8)/* OLE 2 ILockBytes byte array extent table */
#define HOST_BYTEARRAY_PAGE	(7 << 8)/* OLE 2 ILockBytes byte array page */
#define HOST_CDSTORAGE	(8 << 8)/* externally stored CD records */ 
#define HOST_STREAM		(9 << 8)/* Binary private stream */

#define	HOST_LOCAL		0x0f00	/* ONLY used as argument to NSFNoteAttachFile */
								/* means "use MY os's HOST_ type */

#define EFLAGS_MASK		0xf000	/* used for NSFNoteAttachFile encoding arg */
#define EFLAGS_INDOC	0x1000	/* used to pass FILEFLAG_INDOC flag to NSFNoteAttachFile */

/* changed below from 0x00ff to 0x000f to make room for flags defined below */
#define	COMPRESS_MASK	0x000f	/* used for NSFNoteAttachFile Encoding arg */
#define	COMPRESS_NONE	0		/* no compression */
#define	COMPRESS_HUFF	1		/* huffman encoding for compression */

#define NTATT_FTYPE_MASK	0x0070	/*	File type mask */
#define NTATT_FTYPE_FLAT	0x0000	/*	Normal one fork file */
#define NTATT_FTYPE_MACBIN	0x0010	/*	MacBinaryII file */
#define NTATT_NODEALLOC		0x0080	/*	Don't deallocate object when item is deleted */

#define	ATTRIB_READONLY	0x0001	/* file was read-only */
#define	ATTRIB_PRIVATE	0x0002	/* file was private or public */

#define	FILEFLAG_SIGN		0x0001	/* file object has object digest appended */
#define FILEFLAG_INDOC		0x0002	/* file is represented by an editor run in the document */

typedef struct {
	OBJECT_DESCRIPTOR Header;	/* object header */
	WORD FileNameLength;		/* length of file name */
	WORD HostType;		 		/* identifies type of text file delimeters (HOST_) */
	WORD CompressionType;		/* compression technique used (COMPRESS_) */
	WORD FileAttributes;		/* original file attributes (ATTRIB_) */
	WORD Flags;					/* miscellaneous flags (FILEFLAG_) */
	DWORD FileSize;				/* original file size */
	TIMEDATE FileCreated;		/* original file date/time of creation, 0 if unknown */
	TIMEDATE FileModified;		/* original file date/time of modification */
								/* Now comes the file name... It is the original */
								/* RELATIVE file path with no device specifiers */
} FILEOBJECT;

typedef struct {
	char FileCreator[4];		/* application that created the file */
	char FileType[4];			/* type of file */
	DWORD ResourcesStart;		/* offset into the object at which resources begin */
	DWORD ResourcesLen;			/* length of the resources section in bytes */
	WORD CompressionType;		/* compression used for Mac resources */
	DWORD Spare;				/* 0 */
} FILEOBJECT_MACEXT;

typedef struct {
	DWORD EAStart;				/* offset into the object at which EAs begin */
	DWORD EALen;				/* length of EA section */
	DWORD Spare;				/* 0 */
} FILEOBJECT_HPFSEXT;


/* @SPECIAL Escape Codes */

#define	ESCBEGIN 0x7f
#define	ESCEND	 0xff

/*	Index information structure passed into NSFTranslateSpecial to provide
	index-related information for certain @INDEX functions, if specified. */


typedef struct
	{
	DWORD	IndexSiblings;		/* # siblings of entry */
	DWORD	IndexChildren;		/* # direct children of entry */
	DWORD	IndexDescendants;	/* # descendants of entry */
	WORD	IndexAnyUnread;		/* TRUE if entry "unread, or any descendants "unread" */
	} INDEXSPECIALINFO;

/*	Calendar busy time schedule list structure */

/* These are application ID's for the SCHED_LIST wApplicationID field.
** This is used to interpret the application specific UserAttr field.
** Notes ignores the UserAttr field, howerver application specific
** information can be returned by application specific gateways.
** If you need an ID, please register it with Lotus.
*/
#define SCHEDAPPLID_ORGANIZER2X	0x0001
#define SCHEDAPPLID_ORGANIZER4X	0x0002
#define SCHEDAPPLID_OV			0x0003


/* Define the maximum number of sched entries allowed per note item. 
 * Calculated from maximum size of a note item (MAXONESEGSIZE) minus the size
 * of a sched list,  divided by the size of a sched entry.  Subtract 100 of
 * the number for some breathing room */

#define MAXSCHEDLISTSIZE (MAXONESEGSIZE - 100 * sizeof(SCHED_ENTRY))
#define MAXENTRIESPERSCHEDLIST ((MAXSCHEDLISTSIZE - sizeof (SCHED_LIST)) / sizeof(SCHED_ENTRY))

typedef struct
	{
	DWORD	NumEntries;			/* Total number of schedule entries follow */
	WORD	wApplicationID;		/* application id for UserAttr interpretation */
	WORD	Spare;				/* spare */
								/* now come the schedule entries... */
	} SCHED_LIST;


/* scheduling busy time entry */
typedef struct
	{
    UNID            Unid;	 	/* unique ID of the entry */
	TIMEDATE_PAIR   Interval;	/* interval of the entry */
    BYTE            Attr;		/* SCHED_ATTR_xxx attributes defined by Notes */
	BYTE			UserAttr;	/* application specific attributes */
	BYTE			spare[2];
 	} SCHED_ENTRY;

/* The Attr field contains these bits.  Note that if bit7 is set
** then the entry will take up busy time. 
** The lower nibble of the attributes defines the the entry type.
** NOTE!!!!!We're reserving the upper 3 bits of the Attr field for future use.

*/
#define SCHED_ATTR_FOREIGN_UNID	0x10 /* used by gateways to return foreign UNIDs */
#define SCHED_ATTR_RESERVED2	0x20
#define SCHED_ATTR_RESERVED4	0x40
#define SCHED_ATTR_RESERVED8	0x80

/* these are the entry type bits */
#define SCHED_ATTR_TYPE_BITS	0x0F
#define	SCHED_ATTR_FREE_BASE	0x00
#define	SCHED_ATTR_BUSY_BASE	0x08

/* entry types that don't block off busy time */
#define	SCHED_ATTR_NULL					(SCHED_ATTR_FREE_BASE + 0x00) 
#define	SCHED_ATTR_PENCILED				(SCHED_ATTR_FREE_BASE + 0x01) 
#define	SCHED_ATTR_FREE_RESERVED2		(SCHED_ATTR_FREE_BASE + 0x02) 
#define	SCHED_ATTR_FREE_RESERVED3		(SCHED_ATTR_FREE_BASE + 0x03) 
#define	SCHED_ATTR_FREE_RESERVED4		(SCHED_ATTR_FREE_BASE + 0x04) 
#define	SCHED_ATTR_FREE_RESERVED5		(SCHED_ATTR_FREE_BASE + 0x05) 
#define	SCHED_ATTR_FREE_RESERVED6		(SCHED_ATTR_FREE_BASE + 0x06) 
#define	SCHED_ATTR_FREE_RESERVED7		(SCHED_ATTR_FREE_BASE + 0x07) 

/* entry types that block off busy time */
#define	SCHED_ATTR_APPT					(SCHED_ATTR_BUSY_BASE + 0x00)
#define	SCHED_ATTR_NONWORK				(SCHED_ATTR_BUSY_BASE + 0x01)
#define	SCHED_ATTR_BUSY_RESERVED2		(SCHED_ATTR_BUSY_BASE + 0x02) 
#define	SCHED_ATTR_BUSY_RESERVED3		(SCHED_ATTR_BUSY_BASE + 0x03) 
#define	SCHED_ATTR_BUSY_RESERVED4		(SCHED_ATTR_BUSY_BASE + 0x04) 
#define	SCHED_ATTR_BUSY_RESERVED5		(SCHED_ATTR_BUSY_BASE + 0x05) 
#define	SCHED_ATTR_BUSY_RESERVED6		(SCHED_ATTR_BUSY_BASE + 0x06) 
#define	SCHED_ATTR_BUSY_RESERVED7		(SCHED_ATTR_BUSY_BASE + 0x07) 


#define	SCHED_ATTR_TYPE(attr) ((attr) & SCHED_ATTR_TYPE_BITS)
#define	SCHED_ATTR_AVAILABLE(attr) (!((attr) & SCHED_ATTR_BUSY_BASE))


/* Function templates */


STATUS LNPUBLIC   NSFTranslateSpecial(void far *InputString,
							WORD InputStringLength,
							void far *OutputString,
							WORD OutputStringBufferLength,
							NOTEID NoteID,
							void far *IndexPosition,
							INDEXSPECIALINFO far *IndexInfo,
							HANDLE hUnreadList,
							HANDLE hCollapsedList,
							char far *FileTitle,
							char far *ViewTitle,
							WORD far *retLength);
 
/* End of Note Storage File Data Definitions */

#ifdef __cplusplus
}
#endif


#endif
