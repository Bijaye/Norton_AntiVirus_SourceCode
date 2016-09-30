// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#ifndef NIF_ERR_DEFS
#define NIF_ERR_DEFS

/* NIF error status codes */

#define	ERR_SORT_DATATYPE			PKG_NIF+1
	errortext(ERR_SORT_DATATYPE,	"Cannot collate using unsortable datatype")
/*	The ERR_BAD_COLLECTION error is obsolete is only retained for 
	compatibility with pre-109 systems. */
#define	ERR_BAD_COLLECTION			PKG_NIF+2
	errortext(ERR_BAD_COLLECTION,	"Index corrupted - will be rebuilt automatically when database is closed or server is restarted")
#define	ERR_COLLECTION_HANDLE		PKG_NIF+3
	debugtext(ERR_COLLECTION_HANDLE, "Collection not open")
#define ERR_MORE_TO_DO				PKG_NIF+4
	internaltext(ERR_MORE_TO_DO,	"(more to do - call again)")
#define	ERR_COLLECTION_DELETED		PKG_NIF+5
	errortext(ERR_COLLECTION_DELETED, "Collection object was deleted")
#define ERR_UNSUPPORTED_FLAGS		PKG_NIF+6
	errortext(ERR_UNSUPPORTED_FLAGS, "Unsupported return flag(s)")
#define ERR_LOOKUP_ERROR			PKG_NIF+7
	errortext(ERR_LOOKUP_ERROR,		"Error looking up name in Name & Address Book (names.nsf)")
#define ERR_TOO_MANY_LEVELS			PKG_NIF+8
	errortext(ERR_TOO_MANY_LEVELS,	"Index entry has too many levels")
#define ERR_NO_SUCH_NAMESPACE		PKG_NIF+9
	errortext(ERR_NO_SUCH_NAMESPACE,"Name & Address Book (names.nsf) does not contain a required view")
#define ERR_NO_NAMES_FILE			PKG_NIF+10
	errortext(ERR_NO_NAMES_FILE,	"Name & Address Book database (names.nsf) does not exist")
#define ERR_COLLECTION_NOT_CREATED	PKG_NIF+11
	internaltext(ERR_COLLECTION_NOT_CREATED, "(Collection does not exist, and was not created)")
#define	ERR_NAVIGATE_FAILED			PKG_NIF+12
	errortext(ERR_NAVIGATE_FAILED,	"No document to navigate to")
#define	ERR_VIEW_NOACCESS			PKG_NIF+13
	errortext(ERR_VIEW_NOACCESS,	"You are not authorized to access the view")
#define ERR_WRITE_COLLECTION_ERR	PKG_NIF+14
	debugtext(ERR_WRITE_COLLECTION_ERR,	"Error writing %p (%s) index: %e")
#define ERR_NO_SUCH_NAMES_BOOK		PKG_NIF+15
	errortext(ERR_NO_SUCH_NAMES_BOOK,	"No more Name & Address Book databases")
#define ERR_NO_SUCH_ITEM			PKG_NIF+16
	errortext(ERR_NO_SUCH_ITEM,		"Specified item is not present")
#define ERR_NO_MORE_MEMBERS			PKG_NIF+17
	internaltext(ERR_NO_MORE_MEMBERS,	"(No more members in list)")
#define ERR_UNSUPPORTED_TYPE		PKG_NIF+18
	errortext(ERR_UNSUPPORTED_TYPE,	"Unsupported datatype in Name & Address Book document")
#define ERR_NO_MORE_MATCH			PKG_NIF+19
	internaltext(ERR_NO_MORE_MATCH,	 "(No more matches in table)")
#define ERR_NO_REMOTE_INDEX			PKG_NIF+20
	errortext(ERR_NO_REMOTE_INDEX,	 "Index is not to be generated on server.")
#define ERR_UPDATE_INTERRUPTED		PKG_NIF+21
	errortext(ERR_UPDATE_INTERRUPTED,"Someone else deleted this index while you were updating it.")
#define ERR_CONTAINER_ITEM_NOT_FOUND PKG_NIF+22
	internaltext(ERR_CONTAINER_ITEM_NOT_FOUND,"($Container item not found in view note)")
#define ERR_PARTIAL_MATCH			PKG_NIF+23
	internaltext(ERR_PARTIAL_MATCH,	"(Partial match found with TEXT_PARTIALCOMPARE)")
#define ERR_USER_NOT_FOUND 			PKG_NIF+24
	errortext(ERR_USER_NOT_FOUND,"User or server name not found in Name & Address Book")
#define ERR_IDFILE_NOT_FOUND		PKG_NIF+25
	errortext(ERR_IDFILE_NOT_FOUND,"ID file not found in Name & Address Book")
#define ERR_WAS_PERMUTED			PKG_NIF+26
	debugtext(ERR_WAS_PERMUTED,		"Entry had multiple permutations")
#define ERR_BAD_COLLATION			PKG_NIF+27
	errortext(ERR_BAD_COLLATION,	"View has a bad collation description.  It can't be read.")
#define ERR_BAD_COLLECTION_DATA		PKG_NIF+28
	internaltext(ERR_BAD_COLLECTION_DATA,"Something rebuildable that ReadCollection read was found to be trash")
#define ERR_REBUILD_VIEW			PKG_NIF+29
	errortext(ERR_REBUILD_VIEW,		"View is damaged.  Please rebuild it (by pressing shift-F9 or running UPDALL -r).")
#define ERR_RDONLY_DIFF_PLATFORM	PKG_NIF+30
	errortext(ERR_RDONLY_DIFF_PLATFORM,"View cannot be created.  This is a read-only database from a different platform.")
#define ERR_NIF_COLLREBUILDPDL 		PKG_NIF+31
	errortext(ERR_NIF_COLLREBUILDPDL,	"Informational, rebuilding view - notes have been purged since last update (reading %s %s note Title:'%s')")
#define ERR_NIF_POOLFULL			PKG_NIF+32
	errortext(ERR_NIF_POOLFULL,		"Insufficient memory - index pool is full.")
#define ERR_NIF_COLLREBUILD			PKG_NIF+33
	errortext(ERR_NIF_COLLREBUILD,		"Informational, rebuilding view - validation error (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDNCOI		PKG_NIF+34
	errortext(ERR_NIF_COLLREBUILDNCOI,	"Informational, rebuilding view - no container or index (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDCIL 		PKG_NIF+35
	errortext(ERR_NIF_COLLREBUILDCIL,	"Informational, rebuilding view - container integrity lost (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDCS  		PKG_NIF+36
	errortext(ERR_NIF_COLLREBUILDCS,	"Informational, rebuilding view - user specified REBUILD (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDDC  		PKG_NIF+37						 
	errortext(ERR_NIF_COLLREBUILDDC,	"Informational, rebuilding view - database ID changed (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDCDP  		PKG_NIF+38
	errortext(ERR_NIF_COLLREBUILDCDP,	"Informational, rebuilding view - database had cutoff-delete performed (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDCDL 		PKG_NIF+39
	errortext(ERR_NIF_COLLREBUILDCDL,	"Informational, rebuilding view - database cutoff date later than last update (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDRC  		PKG_NIF+40
	errortext(ERR_NIF_COLLREBUILDRC,	"Informational, rebuilding view - user roles changed (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDFC  		PKG_NIF+41
	errortext(ERR_NIF_COLLREBUILDFC,	"Informational, rebuilding view - selection or column formula changed (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDCC  		PKG_NIF+42
	errortext(ERR_NIF_COLLREBUILDCC,	"Informational, rebuilding view - collation changed (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLREBUILDSCC 		PKG_NIF+43
	errortext(ERR_NIF_COLLREBUILDSCC,	"Informational, rebuilding view - subtotal codes changed (reading %s %s note '%s')")
#define ERR_BAD_COLLATION_NUM		PKG_NIF+45
	errortext(ERR_BAD_COLLATION_NUM,"Collation number specified negative or greater than number of collations in view.")
#define ERR_BAD_CHASHTBL_DELETION	PKG_NIF+46
	internaltext(ERR_BAD_CHASHTBL_DELETION,	"The wrong entry was deleted from the collection hash table.")
#define ERR_BAD_CUHASHTBL_DELETION	PKG_NIF+47
	internaltext(ERR_BAD_CUHASHTBL_DELETION,"The wrong entry was deleted from the collection user hash table.")
#define ERR_NIF_BAD_KEYBUFFER		PKG_NIF+48
	internaltext(ERR_NIF_BAD_KEYBUFFER,"Too many Items in Key Buffer pass to NIFFindKey.")
#define ERR_NIF_TOO_MANY_NAMES		PKG_NIF+49
	errortext(ERR_NIF_TOO_MANY_NAMES,"Recipient name not unique, too many found in Name & Address Book, buffer exceeded 64K byte limit.")
#define ERR_NAMED_FOLDER_NOT_FOUND	PKG_NIF+50
	errortext(ERR_NAMED_FOLDER_NOT_FOUND, 	"Named folder not found, operation not done.")
#define ERR_COLLHASH_POOLFULL		PKG_NIF+51
	errortext(ERR_COLLHASH_POOLFULL,		"NIF collection hash table is full.")
#define ERR_NIF_COLLOBJ		 		PKG_NIF+52
	errortext(ERR_NIF_COLLOBJ,				"Informational, rebuild view needed - collection object cannot be read (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLHDR		 		PKG_NIF+53
	errortext(ERR_NIF_COLLHDR,				"Informational, rebuild view needed - invalid collection header (reading %s %s note Title:'%s')")
#define ERR_NIF_COLLTBL		 		PKG_NIF+54
	errortext(ERR_NIF_COLLTBL,				"Informational, rebuild view needed - collation table has changed (reading %s %s note Title:'%s')")
#define	ERR_NIF_REMOTE_NAB			PKG_NIF+55
	errortext(ERR_NIF_REMOTE_NAB,"Notes ini variable NAMES contains a remote NAB it is unable to read")
#define ERR_NIF_FULLSEARCH	 		PKG_NIF+56
	errortext(ERR_NIF_FULLSEARCH,	"Informational, rebuilding view - previous rebuild did not complete (reading %s %s note Title:'%s')")
#define ERR_NIF_UPDATE_NA_VIEW		PKG_NIF+57
	errortext(ERR_NIF_UPDATE_NA_VIEW,	"Updating Name & Address book view '%s'")
#define ERR_DA_BUF_INSUF			PKG_NIF+58
	errortext(ERR_DA_BUF_INSUF,		"Directory Assistance buffer insufficient.")
#define ERR_DA_INIT_FAILED		PKG_NIF+59
	errortext(ERR_DA_INIT_FAILED,	"Directory Assistance tables failed to load properly.")
#define ERR_DA_RELOAD_FAILED		PKG_NIF+60
	errortext(ERR_DA_RELOAD_FAILED,	"Directory Assistance tables failed to reload properly.")
#define ERR_DA_INVALID_RULE			PKG_NIF+61
	errortext(ERR_DA_INVALID_RULE,	"Master Address Book has one or more invalid rules for domain %s.")
#define ERR_DA_INVALID_REPLICA			PKG_NIF+62
	errortext(ERR_DA_INVALID_REPLICA,	"Master Address Book has one or more invalid replicas for domain %s.")
		
/* 	NOTE: NIF errors can only go up to PKG_NIF+121 - higher values are used in NSF as PKG_NSF2 */
	
#endif
