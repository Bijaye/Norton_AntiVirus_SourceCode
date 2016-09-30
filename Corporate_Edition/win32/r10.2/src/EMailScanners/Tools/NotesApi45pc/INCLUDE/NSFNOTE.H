// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#ifndef NSF_NOTE_DEFS
#define NSF_NOTE_DEFS

#ifdef __cplusplus
extern "C" {
#endif


#ifndef NSF_DEFS
#include "nsfdata.h"
#endif

#ifndef POOL_DEFS
#include "pool.h"
#endif


/* Note Storage File Note Definitions */

/*	Item Flags */

#define	ITEM_SIGN						0x0001		/* This field will be signed if requested */
#define	ITEM_SEAL						0x0002		/* This field will be encrypted if requested */
#define	ITEM_SUMMARY					0x0004		/* This field can be referenced in a formula */
#define	ITEM_READWRITERS				0x0020		/* This field identifies subset of users that have read/write access */
#define	ITEM_NAMES						0x0040		/* This field contains user/group names */
#define	ITEM_PLACEHOLDER				0x0100		/* Simply add this item to "item name table", but do not store */
#define	ITEM_PROTECTED					0x0200		/* This field cannot be modified except by "owner" */
#define	ITEM_READERS					0x0400		/* This field identifies subset of users that have read access */
#define ITEM_UNCHANGED					0x1000		/* Item is same as on-disk  */

/*	If the following is ORed in with a note class, the resultant note ID
	may be passed into NSFNoteOpen and may be treated as though you first
	did an NSFGetSpecialNoteID followed by an NSFNoteOpen, all in a single
	transaction. */

#define	NOTE_ID_SPECIAL				0xFFFF0000L

/*	Note Classifications */
/*	If NOTE_CLASS_DEFAULT is ORed with another note class, it is in
	essence specifying that this is the default item in this class.  There
	should only be one DEFAULT note of each class that is ever updated,
	although nothing in the NSF machinery prevents the caller from adding
	more than one.  The file header contains a table of the note IDs of
	the default notes (for efficient access to them).  Whenever a note
	is updated that has the default bit set, the reference in the file
	header is updated to reflect that fact.
	WARNING: NOTE_CLASS_DOCUMENT CANNOT have a "default".  This is precluded
	by code in NSFNoteOpen to make it fast for data notes. 
*/

#define	NOTE_CLASS_DOCUMENT			0x0001		/* document note */
#define	NOTE_CLASS_DATA		NOTE_CLASS_DOCUMENT	/* old name for document note */
#define	NOTE_CLASS_INFO				0x0002		/* notefile info (help-about) note */
#define	NOTE_CLASS_FORM				0x0004		/* form note */
#define	NOTE_CLASS_VIEW				0x0008		/* view note */
#define NOTE_CLASS_ICON				0x0010		/* icon note */
#define NOTE_CLASS_DESIGN			0x0020		/* design note collection */
#define NOTE_CLASS_ACL				0x0040		/* acl note */
#define NOTE_CLASS_HELP_INDEX		0x0080		/* Notes product help index note */
#define	NOTE_CLASS_HELP				0x0100		/* designer's help note */
#define	NOTE_CLASS_FILTER			0x0200		/* filter note */
#define	NOTE_CLASS_FIELD			0x0400		/* field note */
#define NOTE_CLASS_REPLFORMULA		0x0800		/* replication formula */
#define NOTE_CLASS_PRIVATE			0x1000		/* Private design note, use $PrivateDesign view to locate/classify */


#define	NOTE_CLASS_DEFAULT			0x8000		/* MODIFIER - default version of each */

#define	NOTE_CLASS_NOTIFYDELETION	NOTE_CLASS_DEFAULT	/* see SEARCH_NOTIFYDELETIONS */
#define NOTE_CLASS_ALL				0x7fff		/* all note types */
#define NOTE_CLASS_ALLNONDATA		0x7ffe		/* all non-data notes */
#define	NOTE_CLASS_NONE				0x0000		/* no notes */


/* Define symbol for those note classes that allow only one such in a file */

#define NOTE_CLASS_SINGLE_INSTANCE	( \
									NOTE_CLASS_DESIGN | \
									NOTE_CLASS_ACL | \
									NOTE_CLASS_INFO | \
									NOTE_CLASS_ICON | \
									NOTE_CLASS_HELP_INDEX | \
									0)

/*	Note flag definitions */

#define	NOTE_SIGNED					0x0001		/* signed */
#define	NOTE_ENCRYPTED				0x0002		/* encrypted */

/*	Open Flag Definitions.  These flags are passed to NSFNoteOpen. */

#define	OPEN_SUMMARY			0x0001	/* open only summary info */
#define	OPEN_NOVERIFYDEFAULT	0x0002	/* don't bother verifying default bit */
#define	OPEN_EXPAND				0x0004	/* expand data while opening */
#define	OPEN_NOOBJECTS			0x0008	/* don't include any objects */
#define OPEN_SHARE				0x0020	/* open in a "shared" memory mode */
#define OPEN_CANONICAL			0x0040	/* Return ALL item values in canonical form */
#define	OPEN_MARK_READ			0x0100	/* Mark unread if unread list is currently associated */
#define	OPEN_ABSTRACT			0x0200	/* Only open an abstract of large documents */
#define OPEN_RESPONSE_ID_TABLE	0x1000	/* Return response ID table */

/*	Update Flag Definitions.  These flags are passed to NSFNoteUpdate and
	NSFNoteDelete. See also NOTEID_xxx special definitions in nsfdata.h. */

#define UPDATE_FORCE			0x0001	/* update even if ERR_CONFLICT */
#define	UPDATE_NAME_KEY_WARNING	0x0002	/* give error if new field name defined */
#define	UPDATE_NOCOMMIT			0x0004	/* do NOT do a database commit after update */
#define UPDATE_NOREVISION		0x0100	/* do NOT maintain revision history */
#define	UPDATE_NOSTUB			0x0200	/* update body but leave no trace of note in file if deleted */
#define UPDATE_INCREMENTAL		0x4000	/* Compute incremental note info */
#define	UPDATE_DELETED			0x8000	/* update body DELETED */

#define UPDATE_DUPLICATES		0		/* Obsolete; but in SDK */

/* Conflict Handler defines */
#define CONFLICT_ACTION_MERGE 1
#define CONFLICT_ACTION_HANDLED 2

#define	UPDATE_SHARE_SECOND		0x00200000L	/*	Split the second update of this note with the object store */
#define	UPDATE_SHARE_OBJECTS	0x00400000L	/*	Share objects only, not non-summary items, with the object store */

/*	Structure returned from NSFNoteDecrypt which can be used to decrypt
	file attachment objects, which are not decrypted until necessary. */

typedef struct
	{
	BYTE	Byte1;
	WORD	Word1;
	BYTE	Text[16];
	}
	ENCRYPTION_KEY;


/*	Flags returned (beginning in V3) in the _NOTE_FLAGS */

#define	NOTE_FLAG_READONLY		0x0001	/* TRUE if document cannot be updated */
#define	NOTE_FLAG_ABSTRACTED	0x0002	/* missing some data */
#define NOTE_FLAG_LINKED		0x0020	/* Note contains linked items or linked objects */

/* 	Note structure member IDs for NSFNoteGet&SetInfo. */

#define	_NOTE_DB		0		/* IDs for NSFNoteGet&SetInfo */
#define	_NOTE_ID		1		/* (When adding new values, see the */
#define	_NOTE_OID		2		/*  table in NTINFO.C */
#define	_NOTE_CLASS		3
#define	_NOTE_MODIFIED	4
#define	_NOTE_PRIVILEGES	5	/* For pre-V3 compatibility. Should use $Readers item */
#define	_NOTE_FLAGS		7
#define	_NOTE_ACCESSED	8
#define _NOTE_PARENT_NOTEID 10	/* For response hierarchy */
#define _NOTE_RESPONSE_COUNT 11	/* For response hierarchy */
#define _NOTE_RESPONSES 12		/* For response hierarchy */
#define _NOTE_ADDED_TO_FILE 13	/* For AddedToFile time */
#define _NOTE_OBJSTORE_DB 14	/* DBHANDLE of object store used by linked items */


/*	EncryptFlags used in NSFNoteCopyAndEncrypt */

#define	ENCRYPT_WITH_USER_PUBLIC_KEY	0x0001

/*	DecryptFlags used in NSFNoteDecrypt */

#define DECRYPT_ATTACHMENTS_IN_PLACE	0x0001

/*	Flags used for NSFNoteExtractFileExt */

#define NTEXT_RESONLY		0x0001	/*	If a Mac attachment, extract resource fork only. */
#define NTEXT_FTYPE_MASK	0x0070	/*	File type mask */
#define NTEXT_FTYPE_FLAT	0x0000	/*	Normal one fork file */
#define NTEXT_FTYPE_MACBIN	0x0010	/*	MacBinaryII file */

/* 	Possible return values from the callback routine specified in
	NSFNoteComputeWithForm() */
	
#define CWF_ABORT					1
#define CWF_NEXT_FIELD				2
#define CWF_RECHECK_FIELD			3

/* 	Possible validation phases for NSFNoteComputeWithForm()  */
	
#define CWF_DV_FORMULA				1 
#define CWF_IT_FORMULA				2
#define CWF_IV_FORMULA				3
#define CWF_COMPUTED_FORMULA		4 
#define CWF_DATATYPE_CONVERSION		5	
#define CWF_COMPUTED_FORMULA_LOAD	CWF_COMPUTED_FORMULA
#define CWF_COMPUTED_FORMULA_SAVE	6

/*	Function pointer type for NSFNoteComputeWithForm() callback */

typedef	WORD (LNCALLBACKPTR CWF_ERROR_PROC) (const void far *pCDField, 
					 					   WORD phase, STATUS error, 
										   HANDLE ErrorText,
							 			   WORD wErrorTextSize,
							 			   void far *ctx);

/*	Options to NSFNoteComputeWithForm() */

#define	CWF_CONTINUE_ON_ERROR	0x0001		/*	Ignore compute errors */


/* function templates */

STATUS LNPUBLIC NSFItemAppend (NOTEHANDLE hNote, WORD ItemFlags,
								char far *Name, WORD NameLength,
								WORD DataType,
								void far *Value, DWORD ValueLength);
STATUS LNPUBLIC NSFItemAppendByBLOCKID (NOTEHANDLE hNote, WORD ItemFlags,
								char far *Name, WORD NameLength,
								BLOCKID bhValue, DWORD ValueLength,
								BLOCKID far *retbhItem);

STATUS LNPUBLIC NSFItemAppendObject (NOTEHANDLE hNote, WORD ItemFlags,
								char far *Name, WORD NameLength,
								BLOCKID bhValue, DWORD ValueLength, BOOL fDealloc);
STATUS LNPUBLIC NSFItemDelete (NOTEHANDLE hNote, char far *Name, WORD NameLength);
STATUS LNPUBLIC NSFItemDeleteByBLOCKID (NOTEHANDLE hNote, BLOCKID bhItem);

STATUS LNPUBLIC NSFItemRealloc (BLOCKID bhItem, BLOCKID far *bhValue, DWORD ValueLength);

STATUS LNPUBLIC NSFItemCopy (NOTEHANDLE hNote, BLOCKID bhItem);
STATUS LNPUBLIC NSFItemInfo (NOTEHANDLE hNote,
								char far *Name, WORD NameLength,
								BLOCKID far *retbhItem,
								WORD far *retDataType,
								BLOCKID far *retbhValue,
								DWORD far *retValueLength);

#define NSFItemIsPresent(hNote, Name, NameLength) \
		((BOOL) (NSFItemInfo(hNote, Name, NameLength, NULL, NULL, NULL, NULL) == NOERROR))

STATUS LNPUBLIC NSFItemInfoNext (NOTEHANDLE hNote, BLOCKID NextItem,
								char far *Name, WORD NameLength,
								BLOCKID far *retbhItem,
								WORD far *retDataType,
								BLOCKID far *retbhValue,
								DWORD far *retValueLength);

void LNPUBLIC NSFItemQuery (NOTEHANDLE hNote, BLOCKID bhItem,
								char far *retItemName, WORD ItemNameBufferLength,
								WORD far *retItemNameLength,
								WORD far *retItemFlags,
								WORD far *retDataType,
								BLOCKID far *retbhValue, DWORD far *retValueLength);

WORD LNPUBLIC NSFItemGetText (NOTEHANDLE hNote, char far *ItemName, char far *retBuffer, WORD BufferLength);
BOOL LNPUBLIC NSFItemGetTime (NOTEHANDLE hNote, char far *ItemName, TIMEDATE far *retTime);
BOOL LNPUBLIC NSFItemGetNumber (NOTEHANDLE hNote, char far *ItemName, NUMBER far *retNumber);
long LNPUBLIC NSFItemGetLong (NOTEHANDLE hNote, char far *ItemName, long DefaultNumber);

STATUS LNPUBLIC NSFItemSetText (NOTEHANDLE hNote, char far *ItemName, char far *Text, WORD TextLength);
STATUS LNPUBLIC NSFItemSetTextSummary (NOTEHANDLE hNote, char far *ItemName, char far *Text, WORD TextLength, BOOL Summary);
STATUS LNPUBLIC NSFItemSetTime (NOTEHANDLE hNote, char far *ItemName, TIMEDATE far *Time);
STATUS LNPUBLIC NSFItemSetNumber (NOTEHANDLE hNote, char far *ItemName, NUMBER far *Number);

WORD LNPUBLIC NSFItemGetTextListEntries (NOTEHANDLE hNote, char far *ItemName);
WORD LNPUBLIC NSFItemGetTextListEntry (NOTEHANDLE hNote, char far *ItemName, WORD EntryPos, char far *retBuffer, WORD BufferLength);
STATUS LNPUBLIC NSFItemCreateTextList (NOTEHANDLE hNote, char far *ItemName, char far *Text, WORD TextLength);
STATUS LNPUBLIC NSFItemAppendTextList (NOTEHANDLE hNote, char far *ItemName, char far *Text, WORD TextLength, BOOL fAllowDuplicates);

STATUS LNPUBLIC NSFItemGetModifiedTime(HANDLE hNote, char *ItemName, WORD ItemNameLength, DWORD Flags, TIMEDATE *retTime);
STATUS LNPUBLIC NSFItemGetModifiedTimeByBLOCKID (HANDLE hNote, BLOCKID bhItem, DWORD Flags, TIMEDATE *retTime);



BOOL LNPUBLIC NSFItemTextEqual(NOTEHANDLE hNote, char far *ItemName, char far *Text, WORD TextLength, BOOL fCaseSensitive);
BOOL LNPUBLIC NSFItemTimeCompare(NOTEHANDLE hNote, char far *ItemName, TIMEDATE far *Time, int far *retVal);
BOOL LNPUBLIC NSFItemLongCompare(NOTEHANDLE hNote, char far *ItemName, long Value, int far *retVal);

WORD LNPUBLIC NSFItemConvertValueToText (WORD DataType, BLOCKID bhValue, DWORD ValueLength, char far *retBuffer, WORD BufferLength, char SepChar);
WORD LNPUBLIC NSFItemConvertToText (NOTEHANDLE hNote, char far *ItemName, char far *retBuffer, WORD BufferLength, char SepChar);

BOOL LNPUBLIC NSFGetSummaryValue (void far *SummaryBuffer, char far *Name, char far *retValue, WORD ValueBufferLength);
BOOL LNPUBLIC NSFLocateSummaryValue (void far *SummaryBuffer, char far *Name,
								void far *retValuePointer,
								WORD far *retValueLength,
								WORD far *retDataType);
typedef STATUS (LNCALLBACKPTR NSFITEMSCANPROC)(WORD Spare, WORD ItemFlags,
											char far *Name, WORD NameLength,
											void far *Value, DWORD ValueLength,
											void far *RoutineParameter);
STATUS LNPUBLIC NSFItemScan (NOTEHANDLE hNote,
								NSFITEMSCANPROC ActionRoutine,
								void far *RoutineParameter);

void LNPUBLIC NSFNoteGetInfo (NOTEHANDLE hNote, WORD Type, void far *Value);
void LNPUBLIC NSFNoteSetInfo (NOTEHANDLE hNote, WORD Type, void far *Value);
STATUS LNPUBLIC NSFNoteClose (NOTEHANDLE hNote);
STATUS LNPUBLIC NSFNoteCreate (DBHANDLE hDB, NOTEHANDLE far *rethNote);
STATUS LNPUBLIC NSFNoteDelete (DBHANDLE hDB, NOTEID NoteID, WORD UpdateFlags);
STATUS LNPUBLIC NSFNoteOpen (DBHANDLE hDB, NOTEID NoteID, WORD OpenFlags, NOTEHANDLE far *rethNote);
STATUS LNPUBLIC NSFNoteOpenByUNID(HANDLE hDB, UNID far *pUNID, WORD flags, HANDLE far *rtn);
STATUS LNPUBLIC NSFNoteUpdate (NOTEHANDLE hNote, WORD UpdateFlags);
STATUS LNPUBLIC NSFNoteUpdateExtended (NOTEHANDLE hNote, DWORD UpdateFlags);
STATUS LNPUBLIC NSFNoteComputeWithForm (NOTEHANDLE hNote, NOTEHANDLE hFormNote,
								DWORD dwFlags, CWF_ERROR_PROC ErrorRoutine,  
								void far *CallersContext);

STATUS LNPUBLIC NSFNoteAttachFile (NOTEHANDLE hNOTE,
								char far *ItemName, WORD ItemNameLength,
								char far *PathName,
								char far *OriginalPathName,
								WORD Encoding);
STATUS LNPUBLIC NSFNoteExtractFile (NOTEHANDLE hNote, BLOCKID bhItem,
								char far *FileName,
								ENCRYPTION_KEY far *DecryptionKey);
STATUS LNPUBLIC NSFNoteExtractFileExt (NOTEHANDLE hNote, BLOCKID bhItem,
								char far *FileName,
								ENCRYPTION_KEY far *DecryptionKey,
								WORD wFlags);
STATUS LNPUBLIC NSFNoteDetachFile (NOTEHANDLE hNote, BLOCKID bhItem);
BOOL LNPUBLIC	  NSFNoteHasObjects (NOTEHANDLE hNote, BLOCKID far *bhFirstObjectItem);
STATUS LNPUBLIC NSFNoteGetAuthor (NOTEHANDLE hNote, char far *retName, WORD far *retNameLength, 
									BOOL far *retIsItMe);
STATUS LNPUBLIC NSFNoteCopy (NOTEHANDLE hSrcNote, NOTEHANDLE far *rethDstNote);
STATUS LNPUBLIC NSFNoteSignExt(NOTEHANDLE hNote, 
								char far *SignatureItemName,
								WORD ItemCount, HANDLE hItemIDs);
STATUS LNPUBLIC NSFNoteSign (NOTEHANDLE hNote);
STATUS LNPUBLIC NSFNoteVerifySignature (NOTEHANDLE hNote, 
								char far *Reserved,
								TIMEDATE far *retWhenSigned, 
								char far *retSigner, char far *retCertifier);
STATUS LNPUBLIC NSFVerifyFileObjSignature (DBHANDLE hDB, BLOCKID bhItem);
STATUS LNPUBLIC NSFNoteUnsign (NOTEHANDLE hNote);
STATUS LNPUBLIC NSFNoteCopyAndEncrypt (NOTEHANDLE hSrcNote, WORD EncryptFlags, NOTEHANDLE far *rethDstNote);
STATUS LNPUBLIC NSFNoteDecrypt (NOTEHANDLE hNote, WORD DecryptFlags, ENCRYPTION_KEY far *retKeyForAttachments);

BOOL LNPUBLIC NSFNoteIsSignedOrSealed (NOTEHANDLE hNote, BOOL far *retfSigned, BOOL far *retfSealed);
STATUS LNPUBLIC NSFNoteCheck (HANDLE hNote);

STATUS LNPUBLIC NSFNoteFindDivergenceTime(NOTEHANDLE hNote1, NOTEHANDLE hNote2,	DWORD dwFlags, TIMEDATE *tdLastSyncTime);
STATUS LNPUBLIC NSFNoteFindMatchingItem (NOTEHANDLE hNote1, BLOCKID bhItem1, NOTEHANDLE hNote2, DWORD dwFlags, BLOCKID *retbhItem2);

/*	External (text) link routines */


#define LINKFLAG_ADD_TEMPORARY		0x00000002L
#define LINKFLAG_NO_REPL_SEARCH		0x00000004L

STATUS LNPUBLIC NSFNoteLinkFromText(HANDLE hLinkText, WORD LinkTextLength,
									NOTELINK far *NoteLink,
									char far *ServerHint,
									char far *LinkText, WORD MaxLinkText,
									DWORD far *retFlags);
STATUS LNPUBLIC NSFNoteLinkToText(char far *Title,
									NOTELINK far *NoteLink,
									char far *ServerHint,
									char far *LinkText,
									HANDLE far *phLinkText,
									WORD far *pLinkTextLength,
									DWORD Flags);

/* End of Note Storage File Note Definitions */

#ifdef __cplusplus
}
#endif

#endif
