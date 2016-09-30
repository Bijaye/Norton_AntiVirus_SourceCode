// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif

/*
	Public Definitions for Full Text Package
*/

#ifndef FT_DEFS
#define FT_DEFS

#ifndef NIF_DEFS
#include "nif.h"
#endif


/*	Define Indexing options */

#define FT_INDEX_REINDEX		0x0002		/* Re-index from scratch */
#define FT_INDEX_CASE_SENS		0x0004		/* Build case sensitive index */
#define FT_INDEX_STEM_INDEX		0x0008		/* Build stem index */
#define FT_INDEX_PSW			0x0010		/* Index paragraph & sentence breaks*/
#define FT_INDEX_OPTIMIZE		0x0020		/* Optimize index (e.g. for CDROM) */
#define FT_INDEX_ATT			0x0040		/* Index Attachments */
#define FT_INDEX_ENCRYPTED_FIELDS	0x0080		/* Index Encrypted Fields */
#define FT_INDEX_AUTOOPTIONS	0x0100		/* Get options from database */
#define FT_INDEX_SUMMARY_ONLY	0x0200		/* Index summary data only */

/*	Define Search options */

#define FT_SEARCH_SET_COLL		0x00000001	/* Store search results in NIF collections;
												Don't return them to caller */
#define FT_SEARCH_NUMDOCS_ONLY	0x00000002	/* Return # hits only; not the documents */
#define FT_SEARCH_REFINE		0x00000004	/* Refine the query using the IDTABLE */
#define FT_SEARCH_SCORES		0x00000008	/* Return document scores (default sort) */
#define FT_SEARCH_RET_IDTABLE	0x00000010	/* Return ID table */
#define FT_SEARCH_SORT_DATE		0x00000020	/* Sort results by date */
#define FT_SEARCH_SORT_ASCEND	0x00000040	/* Sort in ascending order */
#define FT_SEARCH_TOP_SCORES	0x00000080	/* Use Limit arg. to return only top scores */
#define FT_SEARCH_STEM_WORDS	0x00000200	/* Stem words in this query */
#define FT_SEARCH_THESAURUS_WORDS	0x00000400	/* Thesaurus words in this query */

/*	Define search results data structure */

#define FT_RESULTS_SCORES		0x0001	/* Array of scores follows */
#define FT_RESULTS_EXPANDED		0x0002  /* Search results are series of FT_SEARCH_RESULT_ENTRY structures */

/*	Define search results entries for catalog index entries */

#define FT_RESULT_ENTRY_RESTRICTED			0x0001	/* Entry has read fields */
#define FT_RESULT_ENTRY_TITLE_RESTRICTED	0x0002	/* Title of Db is restricted */


/*	Struct returned by FTSearch when the FT_RESULTS_EXPANDED is specified.
	The rethResults parameters contains an array of these following the 
	FT_RESULTS struct. */

typedef struct
	{
	DBID		ReplicaID;
	UNID		Unid;
	UNID		ViewUnid;	
	TIMEDATE	ModifiedTime;		/* Last modified time of note */
	WORD		Flags;				/* Flags (FT_RESULT_ENTRY_xxxx) */
	WORD		ServerHintLength;	/* Length of Server hint */
	WORD		TitleLength;		/* Length of Databaes Title */
	WORD		CategoriesLength;	/* Length of Database Categories */
	WORD		HeadingLength;		/* Length of View Column Heading */
	WORD		SummaryLength;		/* Length of View Summary */
	BYTE		Score;				
	BYTE		Spare;				
	}
FT_SEARCH_RESULT_ENTRY;

/*	Struct returned by FTSearch in the rethResults parameter.  The data 
	following this struct depends on the FT_RESULTS_xxx flags returned
	from FTSearch.
	
		No Flags			An array of NoteIDs follows.

		FT_RESULTS_SCORES	A BYTE array of scores follows the NoteID
							array.  NOTE: A no-op if FT_RESULTS_EXPANDED
							is specified.

		FT_RESULTS_EXPANDED	An array of FT_SEARCH_RESULT_ENTRY's follows.
							That array is followed by Server, Title,
							Categories, Heading and Summary string fields 
*/

typedef struct
	{
	DWORD		NumHits;			/* Number of search hits following */
	WORD		Flags;				/* Flags (FT_RESULTS_xxx) */
	WORD		VarLength;			/* Length of variable strings after FT_SEARCH_RESULT_ENTRY array */
									/* Variable data follows (see above) */
	}
FT_SEARCH_RESULTS;


typedef struct
	{
	DWORD		DocsAdded;			/* # of new documents */
	DWORD		DocsUpdated;		/* # of revised documents */
	DWORD		DocsDeleted;		/* # of deleted documents */
	DWORD		BytesIndexed;		/* # of bytes indexed */
	}
FT_INDEX_STATS;


/*	Function prototypes */

STATUS LNPUBLIC FTIndex(HANDLE hDB, WORD Options, 
						char far *StopFile, FT_INDEX_STATS far *retStats);
STATUS LNPUBLIC FTDeleteIndex(HANDLE hDB);
STATUS LNPUBLIC FTGetLastIndexTime(HANDLE hDB, TIMEDATE far *retTime);

STATUS LNPUBLIC FTOpenSearch(HANDLE far *rethSearch);
STATUS LNPUBLIC FTSearch(HANDLE hDB, HANDLE far *phSearch, HCOLLECTION hColl,
							char far *Query,
							DWORD Options, WORD Limit, HANDLE hIDTable,
							DWORD far *retNumDocs,
							HANDLE far *Reserved,
							HANDLE far *rethResults);
STATUS LNPUBLIC FTCloseSearch(HANDLE hSearch);


#endif	/* FT_DEFS */

#ifdef __cplusplus
}
#endif

