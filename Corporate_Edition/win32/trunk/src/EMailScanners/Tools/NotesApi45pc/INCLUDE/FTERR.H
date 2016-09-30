// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef FT_ERR_DEFS
#define FT_ERR_DEFS

/* FT (Full text) error status codes */

#define ERR_FT					PKG_FT+0
	errortext(ERR_FT,			"Unknown full-text error")
#define ERR_FT_RAW				PKG_FT+1
	errortext(ERR_FT_RAW,		"Error from full text package")
#define ERR_FT_NOT_INDEXED		PKG_FT+2
	errortext(ERR_FT_NOT_INDEXED,"Database is not full-text indexed")
#define ERR_FT_INIT_API			PKG_FT+3
	errortext(ERR_FT_INIT_API,	"Unable to initialize full text package")
#define ERR_FT_NOT_FOUND		PKG_FT+4
	errortext(ERR_FT_NOT_FOUND,	"Full text index not found for this database")
#define ERR_FT_NO_EXIST			PKG_FT+5
	errortext(ERR_FT_NO_EXIST,	"Document is not in the search results")
#define ERR_FT_TOO_MANY_DBS		PKG_FT+6
	errortext(ERR_FT_TOO_MANY_DBS,"Too many databases are specified")
#define ERR_FT_DB_NOT_FOUND		PKG_FT+7
	errortext(ERR_FT_DB_NOT_FOUND,"Specified database is not being searched")
#define ERR_FT_HIGHLIGHTS		PKG_FT+8
	errortext(ERR_FT_HIGHLIGHTS,"Unable to obtain word highlights")
#define ERR_FT_BAD_QUERY		PKG_FT+9
	errortext(ERR_FT_BAD_QUERY,	"Query is not understandable")
#define	ERR_STALE_HIGHLIGHTS	PKG_FT+10
	errortext(ERR_STALE_HIGHLIGHTS,	"Highlights are not available; this document has been modified since being indexed")
#define ERR_FT_BAD_SEARCH_HANDLE PKG_FT+11
	errortext(ERR_FT_BAD_SEARCH_HANDLE,	"Unable to obtain search results; re-open database")
#define ERR_FT_IN_USE			PKG_FT+12
	errortext(ERR_FT_IN_USE,	"The full text index for this database is in use")
#define ERR_FT_REBUILD			PKG_FT+13
	errortext(ERR_FT_REBUILD,	"The full text index needs to be rebuilt")
#define ERR_FT_NEWER_VER		PKG_FT+14
	errortext(ERR_FT_NEWER_VER,	"The existing full text index was built by later version of this product")
#define ERR_FT_STRUCT_SIZE		PKG_FT+15
	internaltext(ERR_FT_STRUCT_SIZE,"(The allocated structure size is too small for this engine)")
#define ERR_FT_VER0				PKG_FT+16
	internaltext(ERR_FT_VER0,	"(The full text index is corrupt - version 0)")
#define ERR_FT_TOPIC			PKG_FT+17
	errortext(ERR_FT_TOPIC,		"Full text error from Topic; see log for more information")
#define ERR_FT_DIR_LINK			PKG_FT+18
	errortext(ERR_FT_DIR_LINK,	"Full text directory links must contain a valid directory name")
#define ERR_FT_BEING_INDEXED	PKG_FT+19
	errortext(ERR_FT_BEING_INDEXED,"Database is currently being indexed by another process")


/*	Log message codes */

#define LOG_FT_TOPIC_MSG		PKG_FT+20
	errortext(LOG_FT_TOPIC_MSG,	"Topic message: %s")
#define LOG_FT_TOPIC_ERR_STACK	PKG_FT+21
	errortext(LOG_FT_TOPIC_ERR_STACK,"Topic error stack [%lu]:  %s")
#define LOG_FT_TOPIC_ERR		PKG_FT+22
	errortext(LOG_FT_TOPIC_ERR,	"Topic error number %ld")
#define LOG_FT_DELETE_ERR		PKG_FT+23
	errortext(LOG_FT_DELETE_ERR,"Topic error %ld deleting document from full text index")
#define LOG_FT_LEGACY_ERR		PKG_FT+24
	errortext(LOG_FT_LEGACY_ERR,"Full Text Index structure for %s is incompatible. Index is being Rebuilt")

#define LOG_FT_INDEX_ERR		PKG_FT+25
	errortext(LOG_FT_INDEX_ERR, "Error full text indexing document NT%08lx in database %p")

/*	More error codes */

#define ERR_FT_MORE_THAN_5000_DOCS	PKG_FT+30
	errortext(ERR_FT_MORE_THAN_5000_DOCS,	"More than 5000 documents found; please refine the search")
#define ERR_FT_BAD_FIELD		PKG_FT+31
	errortext(ERR_FT_BAD_FIELD,	"The field name used in the query can not be found in the database.")
#define ERR_FT_BAD_PAREN		PKG_FT+32
	errortext(ERR_FT_BAD_PAREN,	"Unbalanced parentheses in query.")
#define ERR_FT_BAD_OPERATOR		PKG_FT+33
	errortext(ERR_FT_BAD_OPERATOR,	"Unknown operator in field query.")
#define ERR_FT_NOMATCHES		PKG_FT+34
	errortext(ERR_FT_NOMATCHES,	"No documents found")
#define ERR_FT_TEXT_FIELD		PKG_FT+35
	errortext(ERR_FT_TEXT_FIELD,	"Relational operators are not supported in text fields")
#define ERR_FT_FIELD_NOT_INDEXED PKG_FT+36
	errortext(ERR_FT_FIELD_NOT_INDEXED,	"The query contains a new number or date field that was added after the full text index was created.  Please re-create the full text index to use this field in queries")
#define ERR_FT_INDEX_BAD		PKG_FT+37
	errortext(ERR_FT_INDEX_BAD,	"Full text index is missing a necessary field")
#define ERR_FT_TOPIC_NOSEE_LOG	PKG_FT+38
	errortext(ERR_FT_TOPIC_NOSEE_LOG,  "Full text error from Topic")
#define ERR_FT_TOPIC_CORRUPT_INDEX	PKG_FT+39
	errortext(ERR_FT_TOPIC_CORRUPT_INDEX,  "Full text index Corrupt, Try to Delete and Re-add Index")


/* String definitions */

#define STR_FT_LIBNAME_KEY		PKG_FT+40
	stringtext(STR_FT_LIBNAME_KEY,"FT")
#define STR_FT_DEF_LIBNAME		PKG_FT+41
	stringtext(STR_FT_DEF_LIBNAME,"LIBFTP")
#define STR_FT_PROGRESS			PKG_FT+42
	stringtext(STR_FT_PROGRESS,	"Indexing %lu documents")
#define STR_FT_MERGING			PKG_FT+43
	stringtext(STR_FT_MERGING,	"Merging full text index; this may take a while...")
#define STR_FT_FINISH			PKG_FT+44
	stringtext(STR_FT_FINISH,	"Finishing full text index; this may take a while...")
#define STR_FT_KEYWORDS			PKG_FT+45
	stringtext(STR_FT_KEYWORDS,	"ACCRUE AND CONTAINS FIELD NEAR NOT OR PARAGRAPH SENTENCE TOPIC TERMWEIGHT EXACTCASE VERITYQUERY USETHESAURUS")
#define ERR_FT_NEEDQUOTES		PKG_FT+46
	errortext(ERR_FT_NEEDQUOTES,	"Expected Verity format query must be inside double quotes")
#define ERR_FT_BADARG_STRUCT	PKG_FT+47
	errortext(ERR_FT_BADARG_STRUCT,	"Invalid Argument Structure")
#define ERR_FT_BADHANDLE_TYPE	PKG_FT+48
	errortext(ERR_FT_BADHANDLE_TYPE,	"Wrong Handle Type")
#define ERR_FT_NO_VDKHOME		PKG_FT+49
	errortext(ERR_FT_NO_VDKHOME,	"Incomplete Setup. VDKHOME Directory Missing from Executable Path")
/*	More error codes */

#define ERR_FT_NO_DOCS			PKG_FT+50
	errortext(ERR_FT_NO_DOCS,	"No documents specified")
#define ERR_FT_INVALIDDOC	PKG_FT+51
	errortext(ERR_FT_INVALIDDOC,	"Bad document ID/key")
#define ERR_FT_LOW_MEMORY	PKG_FT+52
	errortext(ERR_FT_LOW_MEMORY,	"Memory Is Low for Indexing")
#define ERR_FT_DOWN		PKG_FT+53
	errortext(ERR_FT_DOWN,	"Full Text Index is Down for Repairs")
#define ERR_FT_INDEXING_FILE				PKG_FT+54
	errortext(ERR_FT_INDEXING_FILE,		"Full text indexing documents in (%p) '%p' for multi db index")
#define ERR_FT_INDEXING_DONE				PKG_FT+55
	errortext(ERR_FT_INDEXING_DONE,		"%lu documents (%lu bytes) indexed in (%p) '%p' for multi db index")


/*  Only have up to PKG_FT+127 */

#endif /* FT_ERR_DEFS */
