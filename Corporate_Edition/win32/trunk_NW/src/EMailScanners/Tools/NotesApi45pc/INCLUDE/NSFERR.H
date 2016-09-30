// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef NSF_ERR_DEFS
#define NSF_ERR_DEFS


/*	Formula Compiler Error Codes */

#define	ERR_FORMULA_COMPILATION	PKG_FORMULA+1
	errortext(ERR_FORMULA_COMPILATION, "Formula Error")
#define	ERR_INVALID_OPERATOR	PKG_FORMULA+2
	errortext(ERR_INVALID_OPERATOR, "Invalid operator")
#define	ERR_INVALID_ARG_TYPE	PKG_FORMULA+3
	errortext(ERR_INVALID_ARG_TYPE, "Invalid type of argument")
#define	ERR_TOO_MANY_ARGS		PKG_FORMULA+4
	errortext(ERR_TOO_MANY_ARGS, "Too many arguments for @Function")
#define	ERR_FN_OR_OP_EXPECTED	PKG_FORMULA+5
	errortext(ERR_FN_OR_OP_EXPECTED, "@Function or operator expected, or @Function does not require an argument")
#define	ERR_INSUFFICIENT_ARGS	PKG_FORMULA+6
	errortext(ERR_INSUFFICIENT_ARGS, "Insufficient arguments for @Function")
#define	ERR_NOT_UNARY			PKG_FORMULA+7
	errortext(ERR_NOT_UNARY, 	"Inappropriate (unary) usage of an operator")
#define	ERR_NOT_BINARY			PKG_FORMULA+8
	errortext(ERR_NOT_BINARY,	"Inappropriate (binary) usage of an operator")
#define	ERR_PREMATURE_EOI		PKG_FORMULA+10
	errortext(ERR_PREMATURE_EOI,"Formula incomplete")
#define	ERR_OP_EXPECTED			PKG_FORMULA+11
	errortext(ERR_OP_EXPECTED, "An operator or semicolon was expected but none was encountered")
#define	ERR_INVALID_FORMULA		PKG_FORMULA+12
	errortext(ERR_INVALID_FORMULA, "Invalid formula structure")
#define	ERR_PAREN_EXPECTED		PKG_FORMULA+13
	errortext(ERR_PAREN_EXPECTED, "Left parenthesis expected")
#define	ERR_EXTRA_RPAREN		PKG_FORMULA+14
	errortext(ERR_EXTRA_RPAREN, "Extra right parenthesis")
#define	ERR_MISSING_RPAREN		PKG_FORMULA+15
	errortext(ERR_MISSING_RPAREN, "Missing right parenthesis")
#define	ERR_ASSIGNMENT_POS		PKG_FORMULA+16
	errortext(ERR_ASSIGNMENT_POS, ":= can only appear at the beginning of an expression")
#define	ERR_ASSIGNMENT_TARGET	PKG_FORMULA+17
	errortext(ERR_ASSIGNMENT_TARGET, ":= must be immediately preceded by a field or variable name")
#define	ERR_ASSIGNMENT_MODIFIER	PKG_FORMULA+18
	errortext(ERR_ASSIGNMENT_MODIFIER, "Only ENVIRONMENT, DEFAULT, or FIELD can be used as a modifier to :=")
#define	ERR_MISSING_ASS_SEP		PKG_FORMULA+19
	errortext(ERR_MISSING_ASS_SEP, "Missing semicolon")
#define	ERR_REM_FORMAT			PKG_FORMULA+20
	errortext(ERR_REM_FORMAT, "REM must be followed by a quoted string containing the remark.")
#define	ERR_NO_MAIN_EXPRESSION	PKG_FORMULA+21
	errortext(ERR_NO_MAIN_EXPRESSION, "No main or selection expression in formula")
#define	ERR_NOT_A_FUNCTION		PKG_FORMULA+22
	errortext(ERR_NOT_A_FUNCTION, "Passing arguments to a non-@Function or to an @Function that doesn't require arguments")
#define	ERR_UNTERMINATED_LITERAL PKG_FORMULA+23
	errortext(ERR_UNTERMINATED_LITERAL, "End of formula reached before end of quoted string or date/time")
#define	ERR_IF_ODD_ARGS			PKG_FORMULA+24
	errortext(ERR_IF_ODD_ARGS, "@If must have an odd number of arguments")
#define	ERR_ONE_MAIN_EXPR		PKG_FORMULA+25
	errortext(ERR_ONE_MAIN_EXPR, "Only one SELECT expression is allowed")
#define	ERR_NO_FORMULA			PKG_FORMULA+26
	errortext(ERR_NO_FORMULA,   "No formula specified")
#define	ERR_UNKNOWN_FUNCTION	PKG_FORMULA+27
	errortext(ERR_UNKNOWN_FUNCTION, "Unknown @Function")
#define	ERR_SUMMITEMS_MERGE		PKG_FORMULA+28
	debugtext(ERR_SUMMITEMS_MERGE, "EXPRESSION_SUMMITEMS present multiple times!")
#define	ERR_DUPLICATE_IVAR		PKG_FORMULA+29
	errortext(ERR_DUPLICATE_IVAR, "Variable already defined")
#define	ERR_ARGS_EXPECTED		PKG_FORMULA+30
	errortext(ERR_ARGS_EXPECTED, "@Function arguments expected but none were supplied")
#define	ERR_BIG_LITERAL PKG_FORMULA+31
	errortext(ERR_BIG_LITERAL, "Quoted string is too long.")
#define	ERR_MISSING_ASS			PKG_FORMULA+32
	errortext(ERR_MISSING_ASS, ":= must be used in conjunction with FIELD, ENVIRONMENT, or DEFAULT")
#define	ERR_UNKNOWN_FKWD		PKG_FORMULA+33
	errortext(ERR_UNKNOWN_FKWD, "Unknown [KeyWord] for @Function")
#define	ERR_NO_SELECT_EXPR		PKG_FORMULA+34
	errortext(ERR_NO_SELECT_EXPR, "SELECT is inappropriate for this type of formula")
#define ERR_FORMULA_HIDDEN		PKG_FORMULA+35
	errortext(ERR_FORMULA_HIDDEN, "Formula is hidden")
#define ERR_IVAR_NAME_TOO_LONG		PKG_FORMULA+36
	errortext(ERR_IVAR_NAME_TOO_LONG, "Variable name is too long")

/*	Compute Error Codes */

#define	ERR_OP_INVALID			PKG_FORMULA+50
	errortext(ERR_OP_INVALID, "Invalid formula: unknown function/operator")
#define	ERR_OP_UNIMPLEMENTED	PKG_FORMULA+51
	errortext(ERR_OP_UNIMPLEMENTED, "Invalid formula: unimplemented function/operator")
#define	ERR_INVALID_TYPE_FOR_OP	PKG_FORMULA+52
	errortext(ERR_INVALID_TYPE_FOR_OP, "Incorrect data type for operator or @Function")
#define	ERR_ARGS_INVALID		PKG_FORMULA+53
	errortext(ERR_ARGS_INVALID, "Invalid formula format: bad number of arguments")
#define	ERR_FORMULA_GENERATED	PKG_FORMULA+54
	errortext(ERR_FORMULA_GENERATED, "ERROR specified in formula")
#define	ERR_REPEAT_LENGTH		PKG_FORMULA+55
	errortext(ERR_REPEAT_LENGTH, "@Repeat yielded string too large (1024 characters maximum)")
#define	ERR_NO_MAIN_RESULT		PKG_FORMULA+56
	errortext(ERR_NO_MAIN_RESULT, "unable to compute formula value")
#define	ERR_USERNAME			PKG_FORMULA+57
	errortext(ERR_USERNAME,		 "USERNAME is not configured properly")
#define	ERR_SUBSET				PKG_FORMULA+58
	errortext(ERR_SUBSET,		 "The second argument to @Subset must not be zero")
#define	ERR_FORMULA_EXEC		PKG_FORMULA+59
	errortext(ERR_FORMULA_EXEC,	 "Indirect Formula Computation not allowed")
#define	ERR_TEXT_EXPECTED		PKG_FORMULA+60
	errortext(ERR_TEXT_EXPECTED, "Incorrect data type for operator or @Function: Text expected")
#define	ERR_NUMBER_EXPECTED		PKG_FORMULA+61
	errortext(ERR_NUMBER_EXPECTED, "Incorrect data type for operator or @Function: Number expected")
#define	ERR_TIME_EXPECTED		PKG_FORMULA+62
	errortext(ERR_TIME_EXPECTED, "Incorrect data type for operator or @Function: Time/Date expected")
#define	ERR_NTD_EXPECTED		PKG_FORMULA+63
	errortext(ERR_NTD_EXPECTED, "Incorrect data type: Text, Number, or Time/Date expected")
#define	ERR_DATETIME_FMT		PKG_FORMULA+64
	errortext(ERR_DATETIME_FMT, "The @Date and @Time functions must have exactly 1, 3, or 6 arguments.")
#define	ERR_BINOP_TYPES			PKG_FORMULA+65
	errortext(ERR_BINOP_TYPES, 	"Comparison operators must be supplied two values of the same data type.")
#define	ERR_NONAMES				PKG_FORMULA+66
	errortext(ERR_NONAMES,	 	"This view is obsolete and requires missing field list.  Please edit and re-save the view.")
#define	ERR_COMPUTE_LOCALMEM	PKG_FORMULA+67
	errortext(ERR_COMPUTE_LOCALMEM,	"Formula or value requires too much memory")
#define	ERR_COMPUTE_FULL		PKG_FORMULA+68
	errortext(ERR_COMPUTE_FULL,	"Too many concurrent formula evaluations; please retry")
#define	ERR_COMPUTE_EXTENSION	PKG_FORMULA+69
	errortext(ERR_COMPUTE_EXTENSION,"Unknown @Function Extension")
#define	ERR_KEYWORD_EXPECTED	PKG_FORMULA+70
	errortext(ERR_KEYWORD_EXPECTED, "Incorrect data type for operator or @Function: [KeyWord] expected")
#define	ERR_KEYWORD_CONTEXT		PKG_FORMULA+71
	errortext(ERR_KEYWORD_CONTEXT, "The specified [KeyWord] is not appropriate when used with this @Function")
#define	ERR_PROMPT				PKG_FORMULA+72
	errortext(ERR_PROMPT,		"The first argument to @Prompt must be between 1 and 6")
#define	ERR_COMPUTE_RETURN		PKG_FORMULA+73
	internaltext(ERR_COMPUTE_RETURN,"formula has terminated due to @return")
#define	ERR_UNDEF				PKG_FORMULA+74
	errortext(ERR_UNDEF,		"The variable name specified in @Set must be declared prior to use.  To declare it, place VarName:=VarName; at the top of your formula.")
#define	ERR_UNDEFFIELD			PKG_FORMULA+75
	errortext(ERR_UNDEFFIELD,	"The field name specified in @SetField must be declared prior to use.  To declare it, place FIELD FieldName:=FieldName; at the top of your formula.")
#define ERR_GETDOCFIELD_NOTENOTFOUND	PKG_FORMULA+76
	errortext(ERR_GETDOCFIELD_NOTENOTFOUND,	"Document could not be found.")
#define	ERR_COMPUTE_FIELD_TOO_BIG PKG_FORMULA+77
	errortext(ERR_COMPUTE_FIELD_TOO_BIG,"Fields referenced or generated by a formula may not exceed 20,000 bytes.")
/*	78 unused */
#define	ERR_DESCENDANTS			PKG_FORMULA+79
	errortext(ERR_DESCENDANTS,"This database must be compacted to support the use of @AllChildren or @AllDescendants in a formula.")
#define	ERR_PICK_LIST_STYLE		PKG_FORMULA+80
	errortext(ERR_PICK_LIST_STYLE, "The first argument to @PickList must specify a valid @PickList style.")
#define ERR_GETDOCFIELD_BADUNID	PKG_FORMULA+81
	errortext(ERR_GETDOCFIELD_BADUNID, "Invalid UNID; UNID must be a 32-digit hex string.")
#define	ERR_ABSTRACT_BADSIZE	PKG_FORMULA+82
	errortext(ERR_ABSTRACT_BADSIZE, "The buffer size argument of @Abstract must be greater than zero")
#define	ERR_EXPLODE_BADRANGE	PKG_FORMULA+83
	errortext(ERR_EXPLODE_BADRANGE, "The time-date range argument of @Explode spans too many days")
#define ERR_SAMENOTE			PKG_FORMULA+84
	errortext(ERR_SAMENOTE,		"@SetDocField and @GetDocField cannot access the document currently being computed")
#define ERR_ADMIN_SELECTION		PKG_FORMULA+85
	errortext(ERR_ADMIN_SELECTION,	"The currently selected note is not from the correct view.")
#define	ERR_URLHISTORY			PKG_FORMULA+86
	errortext(ERR_URLHISTORY,	"The first argument to @URLHistory must be between 1 and 5")

/*	87 unused */

#define ERR_FUNCTION_CONTEXT	PKG_FORMULA+88
	errortext(ERR_FUNCTION_CONTEXT, "@Function is not valid in this context")
#define ERR_FUNCTION_NOUICOMMANDS	PKG_FORMULA+89
	errortext(ERR_FUNCTION_NOUICOMMANDS,	"@Commands and other UI functions are not allowed in this context")
#define ERR_NSF_COMPUTE_NOASSIGN		PKG_FORMULA+90
	errortext(ERR_NSF_COMPUTE_NOASSIGN,	"You are not allowed to modify fields in this context.")
#define ERR_NSF_COMPUTE_NOSIDE			PKG_FORMULA+91
	errortext(ERR_NSF_COMPUTE_NOSIDE,	"Formulas with side-effects are not allowed in this context.")
#define LOG_MAILSEND_NO_MATCH			PKG_FORMULA+92
	errortext(LOG_MAILSEND_NO_MATCH,   	"Unable to send mail to %s, no match found in Name & Address Book(s)")
#define LOG_MAILSEND_AMBIG_MATCH	   	PKG_FORMULA+93
	errortext(LOG_MAILSEND_AMBIG_MATCH,	"Unable to send mail to %s, multiple matches found in Name & Address Book(s)")

/* Note PKG_FORMULA is limited to 0 - 127 */

/* Note Storage File Error Code Definitions */

#define	ERR_NOT_NSF			PKG_NSF+1
	errortext(ERR_NOT_NSF,	"File is not a database")
#define	ERR_IGNORE_NOTE		PKG_NSF+2
	internaltext(ERR_IGNORE_NOTE,	"ignore this note during copy")
#define ERR_WRITEONLY		PKG_NSF+3
	errortext(ERR_WRITEONLY,"Write-only database")
#define	ERR_READONLY		PKG_NSF+4
	errortext(ERR_READONLY,	"Read-only database")
#define ERR_CONFLICT		PKG_NSF+5
	errortext(ERR_CONFLICT,	"Someone else modified this document at the same time")
#define ERR_STRUCT			PKG_NSF+6
	errortext(ERR_STRUCT,	"The program cannot interpret this file due to an invalid structure")
#define ERR_DOC_STRUCT		PKG_NSF+7
	errortext(ERR_DOC_STRUCT,"Document has invalid structure")
#define ERR_VALUE_FLAGS		PKG_NSF+8
	internaltext(ERR_VALUE_FLAGS,"Invalid combination of flags")
#define ERR_BUCKET_FULL		PKG_NSF+9
	errortext(ERR_BUCKET_FULL,"The text fields of the document are too large to be stored.  Please reduce their size.")
#define	ERR_UNK				PKG_NSF+10
	internaltext(ERR_UNK,	"UNK does not exist")
#define ERR_BUCKET				PKG_NSF+11
	internaltext(ERR_BUCKET,"Invalid pos, not in BUCKET areas")
#define ERR_BUCKET_NOT_IN_MEM	PKG_NSF+12
	internaltext(ERR_BUCKET_NOT_IN_MEM, "(bucket not found)")
#define ERR_NO_FILE_SHARING	PKG_NSF+13
	errortext(ERR_NO_FILE_SHARING,	"This database is currently being used by someone else.  In order to share a Notes database, all users must use a Notes Server instead of a File Server.")
#define ERR_NSF_NOTE_POOLFULL	PKG_NSF+14
	errortext(ERR_NSF_NOTE_POOLFULL, "Insufficient memory - Open Note Pool is full.")
#define ERR_NOTE_EMPTY		PKG_NSF+15
	errortext(ERR_NOTE_EMPTY,	"Document has nothing to undelete")
#define ERR_NO_MORE_MATCHES	PKG_NSF+16
	internaltext(ERR_NO_MORE_MATCHES, "(Search status - no more to do)")
#define ERR_FORMULA			PKG_NSF+17
	errortext(ERR_FORMULA,	"Invalid formula format")
#define ERR_NSF_NEWNOTE_POOLFULL	PKG_NSF+18
	errortext(ERR_NSF_NEWNOTE_POOLFULL, "Insufficient memory - New Note Pool is full.")
#define ERR_OPERATOR		PKG_NSF+19
	errortext(ERR_OPERATOR,	"Invalid operator for data type")
#define ERR_NOTE_ID			PKG_NSF+20
	errortext(ERR_NOTE_ID,	"Invalid document identifier")
#define ERR_NEW_NAME_KEY	PKG_NSF+21
	internaltext(ERR_NEW_NAME_KEY,"New name key being added")
#define ERR_EVENT_TOO_BIG	PKG_NSF+22
	apitext(ERR_EVENT_TOO_BIG,"Event struct size plus data is bigger than 256 bytes")
#define DBD_NOCACHE_STRING	PKG_NSF+23
	stringtext(DBD_NOCACHE_STRING,"NoCache")
#define ERR_DBD_TOO_MANY_DRIVERS PKG_NSF+24
	errortext(ERR_DBD_TOO_MANY_DRIVERS,"You may not use that many different database drivers in any one application.")
#define ERR_NSF_VERSION		PKG_NSF+25
	errortext(ERR_NSF_VERSION, "Invalid NSF version")
#define	ERR_SEARCH_FLAGS		PKG_NSF+26
	apitext(ERR_SEARCH_FLAGS, "Search flags are incompatible")
#define	ERR_FULL_RECALC		PKG_NSF+27
	internaltext(ERR_FULL_RECALC, "(full recalc necessary)")
#define	ERR_DIRECTORY		PKG_NSF+28
	errortext(ERR_DIRECTORY,	"This function is inappropriate for directories.")
#define	ERR_NOT_DIRECTORY	PKG_NSF+29
	errortext(ERR_NOT_DIRECTORY, "This function is only appropriate for directories.")
#define	ERR_ITEM_DEF_TYPE	PKG_NSF+30
	internaltext(ERR_ITEM_DEF_TYPE, "Cannot add item def - type unknown")
#define	ERR_BUCKET_STRUCT		PKG_NSF+31
	internaltext(ERR_BUCKET_STRUCT,	"Invalid structure in bucket")
#define	ERR_NOT_LOCAL		PKG_NSF+32
	apitext(ERR_NOT_LOCAL,	"Function is not supported for remote databases")
#define	ERR_DUP_SUMM_ITEM	PKG_NSF+33
	errortext(ERR_DUP_SUMM_ITEM, "Duplicate field in document")
#define	ERR_ITEM_NOT_FOUND	PKG_NSF+34
	apitext(ERR_ITEM_NOT_FOUND,"Note item not found")
#define ERR_ENTRY_NOT_FOUND		PKG_NSF+35
	apitext(ERR_ENTRY_NOT_FOUND,	"Cache entry not found")
#define	ERR_FMARKER			PKG_NSF+36
	internaltext(ERR_FMARKER,"Missing formula marker in merge formula")
#define	ERR_NOTE_DELETED	PKG_NSF+37
	errortext(ERR_NOTE_DELETED,	"Document has been deleted")
#define	ERR_UNK_TYPE		PKG_NSF+38
	errortext(ERR_UNK_TYPE,	"Field type (UNK) stored in document is invalid")
#define	ERR_INVALID_NOTE	PKG_NSF+39
	errortext(ERR_INVALID_NOTE,	"Invalid or nonexistent document")
#define	ERR_AUXCLASS		PKG_NSF+40
	internaltext(ERR_AUXCLASS,	"Invalid usage of Auxiliary Class Field")
#define	ERR_NSFOPEN			PKG_NSF+41
	errortext(ERR_NSFOPEN,	"Database is currently in use by you or another user")
#define	ERR_NOTE_NOT_EMPTY	PKG_NSF+42
	internaltext(ERR_NOTE_NOT_EMPTY,"note pool not deleted because it is not empty")
#define	ERR_NOTE_NONDISCARDABLE PKG_NSF+43
	internaltext(ERR_NOTE_NONDISCARDABLE,"note pool not discardable")
#define	ERR_NOTE_DISCARDED	PKG_NSF+44
	internaltext(ERR_NOTE_DISCARDED,"note pool has been discarded")
#define	ERR_NSF_INFO_SIZE	PKG_NSF+45
	errortext(ERR_NSF_INFO_SIZE,"Database title is too large; please use a shorter title.")
#define	ERR_NSF_IN_USE		PKG_NSF+46
	errortext(ERR_NSF_IN_USE,"Cannot do that to an NSF that may be in use")
#define	ERR_INVALID_NAME	PKG_NSF+47
	errortext(ERR_INVALID_NAME,"Remote pathname must be relative to Data Directory")
#define ERR_NSF_POOLFULL	PKG_NSF+48
	errortext(ERR_NSF_POOLFULL, "Insufficient memory - NSF pool is full.")
#define	ERR_SUMMARY_TOO_BIG PKG_NSF+49
	errortext(ERR_SUMMARY_TOO_BIG,"Field is too large (15K) or View's column & selection formulas are too large")
#define	ERR_DBCLASS			PKG_NSF+50
	errortext(ERR_DBCLASS,	"Invalid NSF Class")
#define	ERR_TOOHUGE			PKG_NSF+51
	errortext(ERR_TOOHUGE,	"Object too large to allocate given file params")
#define	ERR_NIFNOTE			PKG_NSF+52
	errortext(ERR_NIFNOTE,	"Cannot create document in this type of file")
#define	ERR_NOFILESPACE		PKG_NSF+53
	errortext(ERR_NOFILESPACE,"Cannot allocate database object - database space exhausted")
#define	ERR_BADOBJECT		PKG_NSF+54
	internaltext(ERR_BADOBJECT,"Invalid Object ID")
#define	ERR_RQST_PENDING	PKG_NSF+55
	internaltext(ERR_RQST_PENDING,"Request is still pending")
#define	ERR_NSF_CORRUPT		PKG_NSF+56
	errortext(ERR_NSF_CORRUPT,"Database is corrupt -- Cannot allocate space")
#define	ERR_SOURCE_DB_CLOSED PKG_NSF+57
	errortext(ERR_SOURCE_DB_CLOSED,"Cannot access the document's original database to copy attachments")
#define	ERR_ENCODING		PKG_NSF+58
	errortext(ERR_ENCODING,"Unknown type of compression technique")
#define	ERR_NO_CLIENT		PKG_NSF+59
	debugtext(ERR_NO_CLIENT,"Remote operations not allowed in standalone programs")
#define	ERR_NO_MODIFIED_NOTES PKG_NSF+60
	errortext(ERR_NO_MODIFIED_NOTES,"No documents have been modified since specified time.")
#define	ERR_NO_STAMPED_NOTES PKG_NSF+61
	errortext(ERR_NO_STAMPED_NOTES,"No documents were categorized")
#define	ERR_NO_ITEM_IN_NOTE	PKG_NSF+62
	internaltext(ERR_NO_ITEM_IN_NOTE,"(no such item in document)")
#define	ERR_ITEM_DATATYPE	PKG_NSF+63
	errortext(ERR_ITEM_DATATYPE,"field in note has wrong datatype")
#define	ERR_ITEM_LENGTH		PKG_NSF+64
	errortext(ERR_ITEM_LENGTH,"Field length stored in document is incorrect")
#define	ERR_BAD_PARAM		PKG_NSF+65
	internaltext(ERR_BAD_PARAM,	 "(invalid usage - see NSF documentation)")
#define	ERR_SPECIAL_ID		PKG_NSF+66
	errortext(ERR_SPECIAL_ID,	 "Special database object cannot be located")
#define	ERR_INVALID_ITEMLEN	PKG_NSF+67
	errortext(ERR_INVALID_ITEMLEN,	"Document is damaged or obsolete (incorrect field length)")
#define	ERR_INVALID_ITEMTYPE	PKG_NSF+68
	errortext(ERR_INVALID_ITEMTYPE,	"Document is damaged or obsolete (unrecognized data type)")
#define	ERR_COMPILER_LOAD		PKG_NSF+69
	errortext(ERR_COMPILER_LOAD,	"Insufficient memory to load Formula Compiler/Decompiler, or cannot locate the appropriate program file")
#define	ERR_NOACCESS			PKG_NSF+70
	errortext(ERR_NOACCESS,			"You are not authorized to perform that operation")
#define	ERR_NO_HELP_INDEX		PKG_NSF+71
	errortext(ERR_NO_HELP_INDEX,	"Help Index cannot be located")
#define	ERR_HELP_SECTION		PKG_NSF+72
	errortext(ERR_HELP_SECTION,		"Notes can't automatically display Help on this section, but you can open the Notes Help database and search its Index.")
#define	ERR_HELP_CATEGORY		PKG_NSF+73
	errortext(ERR_HELP_CATEGORY,	"Notes can't automatically display Help on this subject, but you can open the Notes Help database and search its Index.  For Help on server administration, add and open the Notes Administration Help database.")
#define	ERR_HELP_TOOBIG			PKG_NSF+74
	internaltext(ERR_HELP_TOOBIG,	"(help string too large)")
#define	ERR_OPEN_FILE			PKG_NSF+75
	errortext(ERR_OPEN_FILE,		"You are not authorized to access that database")
#define	ERR_DEL_FILE			PKG_NSF+76
	errortext(ERR_DEL_FILE,			"You are not authorized to delete that database")
#define	ERR_UPDATE_CLASS		PKG_NSF+77
	internaltext(ERR_UPDATE_CLASS,	"(you cannot change the class of a note once stored)")
#define	ERR_NOT_AUTHOR			PKG_NSF+78
	errortext(ERR_NOT_AUTHOR,		"You cannot update or delete the document(s) since you are not listed as an allowable Author for this document")
#define	ERR_OBJECT_TRUNCATED	PKG_NSF+79
	errortext(ERR_OBJECT_TRUNCATED,	"File object is truncated - file may have been damaged")
#define	ERR_DBFREE_ARGS			PKG_NSF+80
	debugtext(ERR_DBFREE_ARGS,	"(DbFree: Invalid Disk Position/Size)")
#define	ERR_NTUPDATE_0OID		PKG_NSF+81
	debugtext(ERR_NTUPDATE_0OID,"(NSFNoteUpdate: 0 OID field!)")
#define	ERR_NO_HELP_LINKS		PKG_NSF+82
	errortext(ERR_NO_HELP_LINKS,"Help Indexing Information is not available")
#define	ERR_FMARKERSRC			PKG_NSF+83
	apitext(ERR_FMARKERSRC,		"Formula markers not allowed in merge source formula")
#define	ERR_EXISTS_DIR			PKG_NSF+84
	errortext(ERR_EXISTS_DIR,	"Cannot create database - the specified filename is a directory")
#define	ERR_NTCREATE_LICENSE	PKG_NSF+85
	errortext(ERR_NTCREATE_LICENSE, "Cannot create a document without a valid user license")
#define	ERR_CREATE_FILE			PKG_NSF+86
	errortext(ERR_CREATE_FILE,	"You are not authorized to create new databases on this server")
#define	ERR_CREATE_REP_FILE		PKG_NSF+87
	errortext(ERR_CREATE_REP_FILE,"You are not authorized to create new replica databases on this server")
#define	ERR_DIGEST_LENGTH		PKG_NSF+88
	debugtext(ERR_DIGEST_LENGTH,"Message digest overflowed buffer!")
#define	ERR_NOTE_NOT_SIGNED		PKG_NSF+89
	errortext(ERR_NOTE_NOT_SIGNED,"Document is not signed.")
#define	ERR_NOTE_INVSIG1		PKG_NSF+90
	errortext(ERR_NOTE_INVSIG1,	"Signature on document is invalid (digest length)")
#define	ERR_NOTE_INVSIG2		PKG_NSF+91
	errortext(ERR_NOTE_INVSIG2,	"Document has been modified or corrupted since signed! (data)")
#define	ERR_NOTE_INVSIG3		PKG_NSF+92
	errortext(ERR_NOTE_INVSIG3,"You and signer have no Certificates in common; signer cannot be assumed to be trustworthy.")
#define	ERR_NOTE_INVSIG4		PKG_NSF+93
	errortext(ERR_NOTE_INVSIG4,	"Signature on document is invalid (certifier digest length)")
#define	ERR_NOTE_INVSIG5		PKG_NSF+94
	errortext(ERR_NOTE_INVSIG5,	"Signature has been modified or corrupted since document was signed!")
#define	ERR_NOTE_INVSIG6		PKG_NSF+95
	errortext(ERR_NOTE_INVSIG6,	"Signature on document is invalid (digest length)")
#define	ERR_NOTE_INVSIG7		PKG_NSF+96
	errortext(ERR_NOTE_INVSIG7,	"Document has been modified or corrupted since signed! (signature)")
#define	ERR_NOTE_BADATTSIGN		PKG_NSF+97
	errortext(ERR_NOTE_BADATTSIGN,	"Attachment has been modified or corrupted since signed!")
#define	ERR_NOTE_NOT_SEALED		PKG_NSF+98
	errortext(ERR_NOTE_NOT_SEALED,	"Document is not encrypted.")
#define	ERR_NOTE_ALREADY_SEALED	PKG_NSF+99
	errortext(ERR_NOTE_ALREADY_SEALED,"You cannot encrypt a document that is already encrypted.")
#define	ERR_NOEXTRACT_ENCRYPTED	PKG_NSF+100
	errortext(ERR_NOEXTRACT_ENCRYPTED,"You must supply the bulk decryption key in order to extract this file object.")
#define	ERR_NOT_SEALED_FOR_YOU	PKG_NSF+101
	errortext(ERR_NOT_SEALED_FOR_YOU,"You cannot access portions of this document because it is encrypted and was not intended for you.")
#define	ERR_ALREADY_LOCKED		PKG_NSF+102
	errortext(ERR_ALREADY_LOCKED,"Database is currently being replicated or copied elsewhere")
#define	LOG_OVERDBSIZEWARNTHRESHOLD PKG_NSF+103
	errortext(LOG_OVERDBSIZEWARNTHRESHOLD, "Warning, database %s has exceeded its warning size threshold of %d Kb by %d bytes.")
#define	ERR_DB_NOT_OPEN			PKG_NSF+104
	errortext(ERR_DB_NOT_OPEN,"Specified database is not currently open")
#define	ERR_STS_STRINGTOOLONG	PKG_NSF+105
	errortext(ERR_STS_STRINGTOOLONG,"Text search string is too long")
#define	ERR_NOACTIVITY			PKG_NSF+106
	errortext(ERR_NOACTIVITY,	"User activity recording is disabled")
#define	ERR_FIELDNAME_TOO_LONG	PKG_NSF+107
	errortext(ERR_FIELDNAME_TOO_LONG,"Field names must be 250 characters or less")
#define	ERR_COMPHALT1			PKG_NSF+108
	debugtext(ERR_COMPHALT1,	"(not enough items written)")
#define	ERR_COMPHALT2			PKG_NSF+109
	debugtext(ERR_COMPHALT2,	"(incorrect length written)")
#define	ERR_COMPHALT3			PKG_NSF+110
	debugtext(ERR_COMPHALT3,	"(unevaluated default summary value)")
#define	ERR_NO_DELETED_NOTES	PKG_NSF+111
	errortext(ERR_NO_DELETED_NOTES,"No documents were deleted")
#define	ERR_DATA_ONLY			PKG_NSF+112
	internaltext(ERR_DATA_ONLY,	"(only data notes may be deleted using this call)")
#define	ERR_REMOTE_UNID			PKG_NSF+113
	internaltext(ERR_REMOTE_UNID,"(you cannot obtain the UNIDs using this call to remote databases)")
#define	ERR_MISSING_PRIVS		PKG_NSF+114
	errortext(ERR_MISSING_PRIVS,"All privileges are required for this operation")
#define	ERR_INVALID_ITEMUNK		PKG_NSF+115
	errortext(ERR_INVALID_ITEMUNK,	"Document is damaged or obsolete (unrecognized field)")
#define ERR_FIXUP_DOC			PKG_NSF+116
	errortext(ERR_FIXUP_DOC,	"Document NT%08lx in database %p is damaged: %e")
#define ERR_FIXUP_DOC_DELETED	PKG_NSF+117
	errortext(ERR_FIXUP_DOC_DELETED,"Document NT%08lx in database %p has been deleted")
#define ERR_INVALID_UNK_TBL		PKG_NSF+118
	errortext(ERR_INVALID_UNK_TBL,	"Field name table (UNK table) is damaged")
#define ERR_UNK_TBL_REPAIRED	PKG_NSF+119
	errortext(ERR_UNK_TBL_REPAIRED,	"Field name table (UNK table) has been repaired")
#define	ERR_NO_NAMED_KEY		PKG_NSF+120
	errortext(ERR_NO_NAMED_KEY,	"You cannot access portions of this document because it was encrypted and you do not have any of the keys")
#define	ERR_BAD_KEY_DATA		PKG_NSF+121
	errortext(ERR_BAD_KEY_DATA,	"You are unable to decrypt this document because your key is incorrect")
#define ERR_FIXUP_DOC_ITEM		PKG_NSF+122
	errortext(ERR_FIXUP_DOC_ITEM,	"Document NT%08lx in database %p is damaged: %e (Field %s, Datatype %04x)")
#define ERR_PURGING				PKG_NSF+123
	stringtext(ERR_PURGING,			"Releasing unused storage in database %s...")
#define ERR_PURGING_DOCS		PKG_NSF+124
	stringtext(ERR_PURGING_DOCS,		"Purging old documents from database %s...")
#define ERR_PURGE_QUERY			PKG_NSF+125
	errortext(ERR_PURGE_QUERY,		"The Replication Cutoff Date indicates that documents before %z should be purged from the database %s.  Would you like it to be done now?")
#define ERR_ITEM_NAME_HUGE		PKG_NSF+126
	errortext(ERR_ITEM_NAME_HUGE,	"You can only Sign documents whose Field names are < 512 bytes.")
#define ERR_NO_ENCRYPT_FIELDS	PKG_NSF+127
	errortext(ERR_NO_ENCRYPT_FIELDS,"Document does not specify any fields to be encrypted")
#define ERR_FIXING				PKG_NSF+128
	stringtext(ERR_FIXING,			"Performing consistency check on %s...")
#define	ERR_OBJ_GONE			PKG_NSF+129
	errortext(ERR_OBJ_GONE,		"Database object has been deleted")
#define ERR_FORM_DELETED		PKG_NSF+130
	errortext(ERR_FORM_DELETED,"Form has been deleted")
#define ERR_NULL_ITEM_HANDLE	PKG_NSF+131
	debugtext(ERR_NULL_ITEM_HANDLE,"NSFItemAppendByBLOCKID: bhValue of NULL")
#define ERR_FIXUP_BUCKET_REPAIRED	PKG_NSF+132
	errortext(ERR_FIXUP_BUCKET_REPAIRED,"Partially-saved document in database %p has been deleted")
#define	ERR_FILEMAX				PKG_NSF+133
	errortext(ERR_FILEMAX,		"Database (.nsf) has grown too large; use File New Replica to recreate your file with larger capacity.")
#define	ERR_NSF_CORRUPT2		PKG_NSF+134
	errortext(ERR_NSF_CORRUPT2,"Database has been corrupted and can't be repaired; cannot open")
#define	ERR_NSF_UNKTBL2BIG		PKG_NSF+135
	errortext(ERR_NSF_UNKTBL2BIG,"Cannot store document; database has too many unique field names.  Please ask your administrator to compact the database.")
#define	ERR_FILEMAXV1			PKG_NSF+136
	errortext(ERR_FILEMAXV1,	"Notes Version 1 database (.nsf) has grown larger than 40MB; use File Replication New Replica to recreate your file as a Notes Version 4 database (.nsf) capable of 4GB.")
#define	ERR_DESKMAXV1			PKG_NSF+137
	errortext(ERR_DESKMAXV1,	"Notes Version 1 workspace file (DESKTOP.DSK) has grown larger than 1MB; you can delete DESKTOP.DSK if you wish a new one to be created that is capable of 50MB of private views.")
#define	ERR_DESKMAX				PKG_NSF+138
	errortext(ERR_DESKMAX,		"Workspace file (DESKTOP.DSK) has grown too large; remove some of your private views or icons.")
#define ERR_TBD					PKG_NSF+139
	errortext(ERR_TBD,			"Function to-be-defined")
#define ERR_BTNODE_TOO_SMALL	PKG_NSF+140
	errortext(ERR_BTNODE_TOO_SMALL,	"B-tree node too small - Can't hold 3 entries")
#define ERR_BTNODE_TOO_BIG		PKG_NSF+141
	errortext(ERR_BTNODE_TOO_BIG,	"B-tree node is larger than the maximum slot size")
#define ERR_INVALID_BTREE		PKG_NSF+142
	errortext(ERR_INVALID_BTREE, "B-tree structure is invalid")
#define ERR_NO_BUFFER			PKG_NSF+143
#ifdef MAC
	errortext(ERR_NO_BUFFER,"There is not enough memory for a view or database buffer.  Please close any unneeded windows.  You may also increase Notes' available memory by quitting and choosing 'Get Info...' from the Finder.")
#else
	errortext(ERR_NO_BUFFER,"There is not enough memory for a view or database buffer. Increase NSF_BUFFER_POOL_SIZE.")
#endif
#define ERR_INVALID_PAGE		PKG_NSF+144
	errortext(ERR_INVALID_PAGE,	"Page format is incorrect")
#define ERR_MAXBTDEPTH			PKG_NSF+145
	errortext(ERR_MAXBTDEPTH,	"Maximum depth of b-tree index reached")
#define ERR_SHARE_CONFLICT		PKG_NSF+146
	errortext(ERR_SHARE_CONFLICT, "Operation can't be performed while container is in use")
#define ERR_BAD_SNO				PKG_NSF+147
	errortext(ERR_BAD_SNO,		"Attempt to use an invalid slot number")
#define ERR_BAD_CNO				PKG_NSF+148
	errortext(ERR_BAD_CNO,		"Attempt to use an invalid cluster number")
#define ERR_BAD_DBP				PKG_NSF+149
	errortext(ERR_BAD_DBP,		"Attempt to use an invalid database pointer")
#define ERR_BT_NOTOPEN			PKG_NSF+150
	errortext(ERR_BT_NOTOPEN,	"B-tree index has not been opened")
#define ERR_BAD_CNOVEC			PKG_NSF+151
	errortext(ERR_BAD_CNOVEC,	"Invalid CNO vector - position == 0")
#define ERR_NOTA_IDB			PKG_NSF+152
	debugtext(ERR_NOTA_IDB,		"Block is not a IDB")
#define ERR_NOTA_DBBUF			PKG_NSF+153
	debugtext(ERR_NOTA_DBBUF,	"Block is not a DBBUF")
#define ERR_NOTA_DBBUFCTL		PKG_NSF+154
	debugtext(ERR_NOTA_DBBUFCTL, "Block is not a DBBUFCTL")
#define ERR_NOTA_DBCONT			PKG_NSF+155
	debugtext(ERR_NOTA_DBCONT,	"Block is not a DBCONT")
#define ERR_FILE_ALLOCATED		PKG_NSF+156
	errortext(ERR_FILE_ALLOCATED, "Container not found - new context allocated")
#define ERR_NOTA_DBB_HASH		PKG_NSF+157
	debugtext(ERR_NOTA_DBB_HASH, "Block is not a DBB_HASH")
#define ERR_NO_SPACE			PKG_NSF+158
	errortext(ERR_NO_SPACE,		"No space on page for slot")
#define ERR_INTEGRITY			PKG_NSF+159
	internaltext(ERR_INTEGRITY,	"Container integrity has been lost - rebuild")
#define	ERR_NSF_NOV2  			PKG_NSF+160
	errortext(ERR_NSF_NOV2,	"This function cannot be performed in conjunction with a Notes Version 2 Server")
#define	ERR_NSF_INTERRUPT  		PKG_NSF+161
	errortext(ERR_NSF_INTERRUPT,"Your connection to the server unexpectedly dropped; please retry the operation")
#define NSFSTR_LINK_STR1		PKG_NSF+162
	stringtext(NSFSTR_LINK_STR1,"Database")
#define NSFSTR_LINK_STR2		PKG_NSF+163
	stringtext(NSFSTR_LINK_STR2,"View")
#define NSFSTR_LINK_STR3		PKG_NSF+164
	stringtext(NSFSTR_LINK_STR3,"Document")
#define ERR_ALREADY_ALLOC		PKG_NSF+165
	internaltext(ERR_ALREADY_ALLOC,	"(RRV already allocated)")
#define ERR_COMPACT_MODE		PKG_NSF+166
	internaltext(ERR_COMPACT_MODE,	"(Function cannot be performed while database is being compacted)")
#define ERR_DBD_NON_NOTES		PKG_NSF+167
	errortext(ERR_DBD_NON_NOTES,"You may not use that database driver")
#define NSFSTR_COMPACT_PROGRESS	PKG_NSF+168
	stringtext(NSFSTR_COMPACT_PROGRESS,"Compacting database")
#define STR_FILTER_NEVER		PKG_NSF+169
	stringtext(STR_FILTER_NEVER,"Never")
#define STR_FILTER_HOURLY		PKG_NSF+170
	stringtext(STR_FILTER_HOURLY,"hourly")
#define STR_FILTER_DAILY		PKG_NSF+171
	stringtext(STR_FILTER_DAILY,"daily")
#define STR_FILTER_WEEKLY		PKG_NSF+172
	stringtext(STR_FILTER_WEEKLY,"weekly")
#define	ERR_OBJECT_LENGTH		PKG_NSF+173
	errortext(ERR_OBJECT_LENGTH,"Document attachment is invalid")
#define ERR_BUFPOOL_TOO_SMALL	PKG_NSF+174
#ifdef MAC
	errortext(ERR_BUFPOOL_TOO_SMALL,"Insufficient memory for a view or database buffer.  Please close any unneeded windows.  You may also increase Notes' available memory by quitting and choosing 'Get Info...' from the Finder.")
#else
	errortext(ERR_BUFPOOL_TOO_SMALL,"Insufficient memory for a view or database buffer.")
#endif
#define	ERR_NSF_CORRUPT_RRVMAP	PKG_NSF+175
	errortext(ERR_NSF_CORRUPT_RRVMAP,"Database is damaged and can't be repaired (RRV buckets bad)")
#define	ERR_HDL_LVL0			PKG_NSF+176
	errortext(ERR_HDL_LVL0,		"Maximum number of concurrently open objects has been exceeded")
#define	ERR_HDL_PARAM			PKG_NSF+177
	errortext(ERR_HDL_PARAM,	"No associated pointer to object")
#define	ERR_HDL_NULL			PKG_NSF+178
	errortext(ERR_HDL_NULL,		"Null object handle")
#define	ERR_HDL_OOR				PKG_NSF+179
	errortext(ERR_HDL_OOR,		"Object handle out of range")
#define	ERR_HDL_DEALLOC		 	PKG_NSF+180
	errortext(ERR_HDL_DEALLOC,	"Object handle is invalid")
#define ERR_CANNOT_OPEN_NSF		PKG_NSF+181
	errortext(ERR_CANNOT_OPEN_NSF, "File not found or not a Notes database")
#define ERR_REALLOC_CNOVEC		PKG_NSF+182
	errortext(ERR_REALLOC_CNOVEC,	"Unable to enlarge a view container.  Increase NSF_BUFFER_POOL_SIZE.")
#define ERR_BAD_PNO				PKG_NSF+183
	errortext(ERR_BAD_PNO,		"Attempt to use an invalid page number")
#define ERR_DUPLICATE_UNID		PKG_NSF+184
	errortext(ERR_DUPLICATE_UNID,"Database already contains a document with this ID (UNID)")
#define ERR_DB_NOT_LOADED		PKG_NSF+185
	internaltext(ERR_DB_NOT_LOADED,"(DbDemandLoadByHandle must be called)")
#define ERR_FIXUP_UNID_CHANGED	PKG_NSF+186
	errortext(ERR_FIXUP_UNID_CHANGED,"Document NT%08lx in database %p has been assigned a new UNID")
#define ERR_NO_UNID_INDEX		PKG_NSF+187
	internaltext(ERR_NO_UNID_INDEX,"(UNID index has not been created or opened)")
#define	ERR_NSF_CORRUPT_UNIDIDX	PKG_NSF+188
	errortext(ERR_NSF_CORRUPT_UNIDIDX,"Database is damaged and can't be repaired (UNID index)")
#define ERR_FIXUP_REBUILD_UNID	PKG_NSF+189
	errortext(ERR_FIXUP_REBUILD_UNID,"Rebuild UNID index in database %p")
#define ERR_INVALID_EHASH		PKG_NSF+190
	errortext(ERR_INVALID_EHASH, "Extendible hash index structure is invalid")
#define ERR_USERDATA_TRUNC		PKG_NSF+191
	errortext(ERR_USERDATA_TRUNC, "Container user data buffer will be truncated")
#define ERR_EH_CORRUPT			PKG_NSF+192
	errortext(ERR_EH_CORRUPT,	"Extendible hash index is corrupt and can't be used")
#define ERR_CANT_CHANGE_UNID	PKG_NSF+193
	apitext(ERR_CANT_CHANGE_UNID,"Can't change document's UNID")
#define ERR_EXP_DATE			PKG_NSF+194
	errortext(ERR_EXP_DATE,		"Database's expiration date has passed")
#define ERR_DUP_TPL				PKG_NSF+195
	errortext(ERR_DUP_TPL,		"WARNING: Both %p and %p claim to be Design Template '%.*s'")
#define ERR_CONTAINER_LIMIT		PKG_NSF+196
	errortext(ERR_CONTAINER_LIMIT, "Maximum size of a storage container (130MB) reached")
#define ERR_OBJECT_CANNOT_BE_ZERO		PKG_NSF+197
	errortext(ERR_OBJECT_CANNOT_BE_ZERO, "Object size cannot be zero")
#define	ERR_NOTE_INVSIG8		PKG_NSF+198
	errortext(ERR_NOTE_INVSIG8,	"The Address Book does not contain any cross certificates capable of validating the signature.")
#define ERR_BC_POOL_FULL		PKG_NSF+199
	errortext(ERR_BC_POOL_FULL, "Insufficient memory - The NSF buffer control pool is full.")
#define	ERR_UPDATE_ATT			PKG_NSF+200
	errortext(ERR_UPDATE_ATT,	"One or more of the source document's attachment are missing.  Run Fixup to delete the document in the source database.")
#define ERR_LINK_STR1		   	PKG_NSF+201
	stringtext(ERR_LINK_STR1,	"Database")
#define ERR_LINK_STR2		   	PKG_NSF+202
	stringtext(ERR_LINK_STR2,	"View")
#define ERR_LINK_STR3		   	PKG_NSF+203
	stringtext(ERR_LINK_STR3,	"Document")
#define	ERR_NSFOPENEXCLUSIVE	PKG_NSF+204
	errortext(ERR_NSFOPENEXCLUSIVE,	"Database is being Compacted; Compact must finish before use.")
/* The following status is 'sent' to the replicator via a break procedure */
/* to partially cancel the replication.  The replicator should stop */
/* processing the current database and start processing the next database. */
/* This is used by background/briefcase to skip to the next database in */
/* the replication list.  As it turns out, the replicator carries on for */
/* errors other than CANCEL and SERVER_NOT_RESPONDING.  It does not */
/* specifically look for this error code. */
#define ERR_SKIP				PKG_NSF+205
	errortext(ERR_SKIP,		"Next operation started at your request")
#define ERR_ASYNC_DELIVERY_SEQNO PKG_NSF+206
	internaltext(ERR_ASYNC_DELIVERY_SEQNO,"(no need to do this transaction in V4)")
#define ERR_DBCLOSE_THREAD		PKG_NSF+207
	errortext(ERR_DBCLOSE_THREAD,"NSFDbClose THREADID != NSFDbOpen THREADID!")
#define ERR_EM_ILLEGALAID  		PKG_NSF+208
	errortext(ERR_EM_ILLEGALAID,"Illegal or uninitialized addin id")
#define ERR_EM_ILLEGAL	 		PKG_NSF+209
	errortext(ERR_EM_ILLEGAL,"Extension number out of bounds")
#define ERR_EM_ILLEGALFLAGS 	PKG_NSF+210
	errortext(ERR_EM_ILLEGALFLAGS,"Extension registration flags illegal")
#define ERR_EM_TOOMANYADDINS 	PKG_NSF+211
	errortext(ERR_EM_TOOMANYADDINS,"Extension Manager has maximum addins already")
#define ERR_EM_NOTAUTHORIZED PKG_NSF+212
	errortext(ERR_EM_NOTAUTHORIZED,"Not authorized to register extension")
#define ERR_EM_CONTINUE 		PKG_NSF+213
	internaltext(ERR_EM_CONTINUE,"continue")
#define ERR_EVENTQUEUE_NOTOPENED PKG_NSF+214
	errortext(ERR_EVENTQUEUE_NOTOPENED,"Event Manager Queue not opened")
#define ERR_EVENTQNEEDS_RESET 	PKG_NSF+215
	errortext(ERR_EVENTQNEEDS_RESET,"Event Manager data structures in error")
#define ERR_EVENTQUEUE_ISFULL 	PKG_NSF+216
	errortext(ERR_EVENTQUEUE_ISFULL,"Event Manager Queue is full")
#define ERR_EVENTQUEUE_MEMORY 	PKG_NSF+217
	errortext(ERR_EVENTQUEUE_MEMORY,"Event Manager Queue memory full")
#define ERR_REPL_UPDATE_RACE	PKG_NSF+218
	errortext(ERR_REPL_UPDATE_RACE,	"Multiple replicator update race")
#define ERR_NO_INCR_UPDATE		PKG_NSF+219
	errortext(ERR_NO_INCR_UPDATE, "Incremental updates disallowed")
#define	ERR_NOTE_INVSIG9		PKG_NSF+220
	errortext(ERR_NOTE_INVSIG9,	"Signature on document is invalid (inconsistent field signatures)")
#define ERR_BTVERIFY			PKG_NSF+221
	errortext(ERR_BTVERIFY,		"NSF BT: Verification error in view %s in database %s.  See NOTES.BRP for dump.")
#define ERR_BTVERIFY_NODESIZE	PKG_NSF+222
	errortext(ERR_BTVERIFY_NODESIZE,	"BTVerifyNode: ERROR in %s, node size is %d - expected %d")
#define ERR_BTVERIFY_LEVEL		PKG_NSF+223
	errortext(ERR_BTVERIFY_LEVEL, "BTVerifyNode: ERROR in %s, level is %d - expected %d")
#define ERR_BTVERIFY_PASTEND	PKG_NSF+224
	errortext(ERR_BTVERIFY_PASTEND, "BTVerifyNode: ERROR in %s, entry %d is past the end of the node")
#define ERR_BTVERIFY_DATA_PASTEND	PKG_NSF+225
	errortext(ERR_BTVERIFY_DATA_PASTEND, "BTVerifyNode: ERROR in %s, Data for entry %d is past the end of the node")
#define ERR_BTVERIFY_PIN_DATA	PKG_NSF+226
	errortext(ERR_BTVERIFY_PIN_DATA,	"BTVerifyNode: ERROR %e pinning BT_DATA %s in node %s")
#define ERR_BTVERIFY_LDATA_SIZE	PKG_NSF+227
	errortext(ERR_BTVERIFY_LDATA_SIZE, "BTVerifyNode: ERROR BT_DATA %s in node %s has size %d, sum of chunks is %d")
#define ERR_BTVERIFY_UNPIN_DATA	PKG_NSF+228
	errortext(ERR_BTVERIFY_UNPIN_DATA,	"BTVerifyNode: ERROR %e unpinning BT_DATA %s in node %s")
#define ERR_BTVERIFY_CARDINALITY	PKG_NSF+229
	errortext(ERR_BTVERIFY_CARDINALITY,	"BTVerifyNode: ERROR in %s, cardinality is %lu - expected %lu")
#define ERR_BTVERIFY_MAXFALSE	PKG_NSF+230
	errortext(ERR_BTVERIFY_MAXFALSE,	"BTVerifyNode: ERROR in %s, MaxEntry is FALSE - expected TRUE")
#define ERR_BTVERIFY_MAXTRUE	PKG_NSF+231
	errortext(ERR_BTVERIFY_MAXTRUE,		"BTVerifyNode: ERROR in %s, MaxEntry is TRUE - expected FALSE")
#define ERR_BTVERIFY_MAXKEYSIZE	PKG_NSF+232
	errortext(ERR_BTVERIFY_MAXKEYSIZE,	"BTVerifyNode: ERROR in %s, MaxKeySize is %d - expected %d")
#define ERR_BTVERIFY_MAXKEYVALUE	PKG_NSF+233
	errortext(ERR_BTVERIFY_MAXKEYVALUE,	"BTVerifyNode: ERROR in %s, maximum key value is wrong")
#define ERR_BTVERIFY_PINSUBINDEX	PKG_NSF+234
	errortext(ERR_BTVERIFY_PINSUBINDEX,	"BTVerifyNode: ERROR %e pinning sub-index descriptor %s in node %s")
#define ERR_BTVERIFY_UNPINSUBINDEX	PKG_NSF+235
	errortext(ERR_BTVERIFY_UNPINSUBINDEX,	"BTVerifyNode: ERROR %e unpinning sub-index descriptor %s in node %s")
#define ERR_FILEEXT_INVALID_READCOUNT	PKG_NSF+236
	errortext(ERR_FILEEXT_INVALID_READCOUNT, "Extendable File object: invalid read count")
#define ERR_FILEEXT_ALREADY_OPEN	PKG_NSF+237
	errortext(ERR_FILEEXT_ALREADY_OPEN, "Extendable File object: already open")
#define ERR_FILEEXT_NOT_OPEN		PKG_NSF+238
	errortext(ERR_FILEEXT_NOT_OPEN, "Extendable File object: object must be open to perform this function")
#define	ERR_DIRMAN_NOT_INITIALIZED	PKG_NSF+239
	errortext(ERR_DIRMAN_NOT_INITIALIZED,"Directory Manager Not Initialized.")
#define	ERR_NOSTUB_HAS_RESPONSES	PKG_NSF+240
	errortext(ERR_NOSTUB_HAS_RESPONSES,	"A note with responses cannot be deleted with UPDATE_NOSTUB")
#define	ERR_RESPONSE_CYCLE			PKG_NSF+241
	errortext(ERR_RESPONSE_CYCLE,		"This operation on %p (NoteID = %lu) creates a cycle in the response hierarchy")
#define ERR_NSF_DIRMAN_POOLFULL		PKG_NSF+242
	errortext(ERR_NSF_DIRMAN_POOLFULL, "Insufficient memory - NSF directory manager pool is full.")
#define ERR_NSF_BUF_POOLFULL		PKG_NSF+243
	errortext(ERR_NSF_BUF_POOLFULL, "Insufficient memory - NSF buffer pool is full.  Increase the value of the NSF_BUFFER_POOL_SIZE environment variable.")
#define ERR_EVENTQ_POOLFULL			PKG_NSF+244
	errortext(ERR_EVENTQ_POOLFULL, "Insufficient memory - Event Manager Pool is full.")
#define ERR_DBINDEX_POOLFULL		PKG_NSF+245
	errortext(ERR_DBINDEX_POOLFULL, "Insufficient memory - too many databases on server - Database List Pool is full.")
#define ERR_SYNC_POOLFULL			PKG_NSF+246
	errortext(ERR_SYNC_POOLFULL, 	"Insufficient memory - Database Sync Pool is full.")
#define ERR_EM_POOLFULL				PKG_NSF+247
	errortext(ERR_EM_POOLFULL,	 	"Insufficient memory - Extension Manager Data Pool is full.")
#define	ERR_LOCALSEC_DBOPENFAILURE		PKG_NSF+248
	errortext(ERR_LOCALSEC_DBOPENFAILURE,	"This database has local access protection and you are not authorized to access it locally")
#define	ERR_LOCALSEC_NOTSUPPORTED		PKG_NSF+249
	errortext(ERR_LOCALSEC_NOTSUPPORTED, "The local security feature is not supported for the database or server")
#define	ERR_LOCALSEC_ALREADYSET			PKG_NSF+250
	errortext(ERR_LOCALSEC_ALREADYSET, "This database already has local security set up and can not be set again")
#define ERR_LOCALSEC_NOUSERPUBKEY		PKG_NSF+251
	errortext(ERR_LOCALSEC_NOUSERPUBKEY, "Can not find the user or public key information in the Name and Address Book")
#define	ERR_LOCALSEC_INVALIDSTATE	PKG_NSF+252
	errortext(ERR_LOCALSEC_INVALIDSTATE, "Invalid local security state")
#define ERR_FIXUP_DUP_DELETED		PKG_NSF+253
	errortext(ERR_FIXUP_DUP_DELETED,"A duplicate of document NT%08lx in database %p has been deleted")
#define	ERR_OVERDISKQUOTA		PKG_NSF+254
	errortext(ERR_OVERDISKQUOTA,"Cannot allocate database object - database would exceed its disk quota.")
#define	LOG_OVERDISKQUOTA		PKG_NSF+255
	errortext(LOG_OVERDISKQUOTA,"Cannot allocate database object - database %s would exceed its disk quota of %d Kb by %d bytes.")

/* We're full here for PKG_NSF - Max is 255! Start using PKG_NSF2 below...*/


/* More Note Storage File Error Code Definitions */

#define	ERR_LOCAL_ENCRYPT_MUST_COMPACT	PKG_NSF2+1
	errortext(ERR_LOCAL_ENCRYPT_MUST_COMPACT, "You must compact the database in order to encrypt all documents.")
#define	ERR_LOCAL_DECRYPT_MUST_COMPACT	PKG_NSF2+2
	errortext(ERR_LOCAL_DECRYPT_MUST_COMPACT, "You must compact the database in order to decrypt all documents.")
#define	ERR_IS_OBJSTORE					PKG_NSF2+3
	errortext(ERR_IS_OBJSTORE,			 "The database required for this operation is an object store.")
#define	ERR_OBJSTORE_UPD_REPLICA		PKG_NSF2+4
	errortext(ERR_OBJSTORE_UPD_REPLICA, "The database and the object store for this update are replicas.")
#define	ERR_ITEM_MISMATCH				PKG_NSF2+5
	errortext(ERR_ITEM_MISMATCH,		"An item's value or datatype differs from the same item in the object store note.")
#define	ERR_LOCALSEC_NEEDCOMPACT		PKG_NSF2+6
	errortext(ERR_LOCALSEC_NEEDCOMPACT, "You should compact your database in order to complete the local security setting")
#define	ERR_LOCALSEC_UNKNOWNMETHOD		PKG_NSF2+7
	errortext(ERR_LOCALSEC_UNKNOWNMETHOD, "The specified encryption method is invalid or not supported by this version of software")
#define	ERR_OBJSTORE_NOTSPLIT			PKG_NSF2+8
	errortext(ERR_OBJSTORE_NOTSPLIT,	"Cannot split note (NoteID %d) between database %p and object store %p ")
#define	ERR_BAD_DBUHASHTBL_DELETION		PKG_NSF2+9
	internaltext(ERR_BAD_DBUHASHTBL_DELETION, "The wrong entry was deleted from the dbu hash table.")
#define ERR_EM_MAX_EXTENSIONS			PKG_NSF2+10
	errortext(ERR_EM_MAX_EXTENSIONS,	"Extension Manager exceeded maximum number of Recursion IDs")
#define	ERR_NAMED_OBJECT_CORRUPT		PKG_NSF2+11
	errortext(ERR_NAMED_OBJECT_CORRUPT, "Named Object corrupt.")
#define	ERR_COMPACT_INTERRUPTED			PKG_NSF2+12
	errortext(ERR_COMPACT_INTERRUPTED,	"Compaction of the database was stopped prematurely because another user modified it while it was being compacted.")
#define	ERR_VALIDATION_FAILED			PKG_NSF2+13
	apitext(ERR_VALIDATION_FAILED,	"Validation failed.")
#define	ERR_NSF_HEADER_CORRUPTED		PKG_NSF2+14
	errortext(ERR_NSF_HEADER_CORRUPTED,	"The Notes database file header is corrupted")
#define	ERR_NO_SHARED_ITEMS				PKG_NSF2+15
	errortext(ERR_NO_SHARED_ITEMS,		"The note contains no items that can be shared with the note in the object store.")
#define ERR_LOCAL_ACCESS_AUTHN			PKG_NSF2+16
	errortext(ERR_LOCAL_ACCESS_AUTHN,	"Your ID failed authentication check. Access is denied")
#define ERR_IDTABLE_LENGTH_MISMATCH		PKG_NSF2+17
	errortext(ERR_IDTABLE_LENGTH_MISMATCH,	"The internal length of the ID table does not match the length expected")
#define ERR_FOLDER_DIRECTORY_CORRUPT	PKG_NSF2+18
	errortext(ERR_FOLDER_DIRECTORY_CORRUPT,	"Folder is damaged. Please close and reopen database to repair it.")
#define ERR_FOLDER_NOFREESPACE			PKG_NSF2+19
	errortext(ERR_FOLDER_NOFREESPACE,	"Folder has no free space but should have.")
#define ERR_REPL_BLOCK_FULL				PKG_NSF2+20
	errortext(ERR_REPL_BLOCK_FULL,		"Folder replication block is full, will be reallocated.")
#define ERR_ILLEGAL_FOLDER_COPY			PKG_NSF2+21
	errortext(ERR_ILLEGAL_FOLDER_COPY,	"Source and destination folders must reside in same DB.")
#define ERR_FOLDER_CORRUPT				PKG_NSF2+22
	errortext(ERR_FOLDER_CORRUPT,	"Folder has been damaged.  Please close and reopen database to have it repaired.")
#define ERR_FOLDER_TOO_BIG				PKG_NSF2+23
	errortext(ERR_FOLDER_TOO_BIG,	"Folder is larger than supported, cannot perform operation.")
#define ERR_NO_FOLDER_REPLICATION		PKG_NSF2+24
	errortext(ERR_NO_FOLDER_REPLICATION,	"Folder replication not supported by remote server.")
#define ERR_TOO_MANY_FOLDER_UPDATES		PKG_NSF2+25
	errortext(ERR_TOO_MANY_FOLDER_UPDATES,	"Too many folder additions or removals at one time")
#define ERR_FOLDER_ACCESS_DENIED		PKG_NSF2+26
	errortext(ERR_FOLDER_ACCESS_DENIED,	"No privilege to update folder")
#define ERR_NOTE_CANT_BE_FOLDER			PKG_NSF2+27
	errortext(ERR_NOTE_CANT_BE_FOLDER,	"Note must be public or private view to be a folder")
#define	ERR_MAXSIZE_NOTSUPPORTED 		PKG_NSF2+28
	errortext(ERR_MAXSIZE_NOTSUPPORTED,		"The server does not support Notes database with maximum size greater than 1 gigabytes")
#define	ERR_OBJSTORE_NOACCESS			PKG_NSF2+29
	errortext(ERR_OBJSTORE_NOACCESS,	"You do not have access to the object store that is used by this note")
#define	ERR_OBJSTORE_NOT_FOUND			PKG_NSF2+30
	errortext(ERR_OBJSTORE_NOT_FOUND,	"The object store note that is used by this note was not found.  Run the object store COLLECT task on this database.")
#define	ERR_CANNOT_OPEN_OBJSTORE		PKG_NSF2+31
	errortext(ERR_CANNOT_OPEN_OBJSTORE,	"The object store that is used by this note was not found or is not a Notes database")
#define	ERR_OBJSTORE_NOTE_NOACCESS			PKG_NSF2+32
	errortext(ERR_OBJSTORE_NOTE_NOACCESS,	"You do not have access to the object store note that is used by this note")
#define	ERR_OBJSTORE_NOTE_DELETED			PKG_NSF2+33
	errortext(ERR_OBJSTORE_NOTE_DELETED,	"The object store note that is used by this note has been deleted.  Run the object store COLLECT task on this database.")
#define	ERR_USER_HANDLED_CONFLICT		PKG_NSF2+34
	errortext(ERR_USER_HANDLED_CONFLICT,"User Has Handled their own Conflict.")
#define ERR_FIXUP_GHOST					PKG_NSF2+35
	errortext(ERR_FIXUP_GHOST,			"Fixed header of document NT%08lx in database %p so it will replicate properly.")
#define	ERR_INVALID_CD_FILE				PKG_NSF2+36
	errortext(ERR_INVALID_CD_FILE,		"Incorrectly formatted Composite records in file.")
#define ERR_FIXUP_FOUND_UNREADLIST		PKG_NSF2+37
	errortext(ERR_FIXUP_FOUND_UNREADLIST,	"Unread list for user %a in database %p is damaged: %e")
#define ERR_DELETE_ENTRY				PKG_NSF2+38
	errortext(ERR_DELETE_ENTRY,			"Internal only - delete named table entry.")
#define ERR_UPDATE_ENTRY				PKG_NSF2+39
	errortext(ERR_UPDATE_ENTRY,			"Internal only - update named table entry.")
#define ERR_TOO_MANY_FOLDERS			PKG_NSF2+40
	errortext(ERR_TOO_MANY_FOLDERS,		"Exceeded maximum folder count in database.")
#define ERR_FIXUP_DEL_UNREADLIST		PKG_NSF2+41
	errortext(ERR_FIXUP_DEL_UNREADLIST,	"Unread list for user %a in database %p is damaged and has been deleted: %e")
#define ERR_FIXUP_NAMEDNOTE				PKG_NSF2+42
	errortext(ERR_FIXUP_NAMEDNOTE,		"Named note NT%08lx in database %p is damaged.")
#define ERR_NSF_FOLDER_POOLFULL			PKG_NSF2+43
	errortext(ERR_NSF_FOLDER_POOLFULL, "Insufficient memory - NSF folder pool is full.")
#define ERR_FIXUP_FOLDER_PURGED			PKG_NSF2+44
	errortext(ERR_FIXUP_FOLDER_PURGED,	"Folder NT%08lx in database %p has been purged due to corruption: %s")
#define ERR_FIXUP_FOLDER_IDTABLE		PKG_NSF2+45
	errortext(ERR_FIXUP_FOLDER_IDTABLE,	"Folder NT%08lx in database %p has corrupt ID table, will be rebuilt.  Error: %e.")
#define ERR_FOLDERS_UPTODATE			PKG_NSF2+46
	errortext(ERR_FOLDERS_UPTODATE,		"Folders in database are up to date")
#define ERR_RTR_NOTSET					PKG_NSF2+47
	errortext(ERR_RTR_NOTSET,			"RTR Context not set for this database")
#define ERR_RTR_ALREADYSET 				PKG_NSF2+48
	errortext(ERR_RTR_ALREADYSET,		"RTR Context already set for this database")
#define ERR_NSF_COMPUTE_NOENVIRONMENT	PKG_NSF2+49
	errortext(ERR_NSF_COMPUTE_NOENVIRONMENT,	"You are not allowed to modify environment variables.")
#define ERR_NSF_REMOTE_URL_QUEUE		PKG_NSF2+50
	errortext(ERR_NSF_REMOTE_URL_QUEUE, "Cannot queue a URL Note Open to a remote database.")
#define ERR_NOTE_NOT_FOLDER				PKG_NSF2+51
	errortext(ERR_NOTE_NOT_FOLDER,		"Attempt to perform folder operation on non-folder note.")
#define ERR_NO_VIEWS_IN_OBJSTORE	  	PKG_NSF2+52
	errortext(ERR_NO_VIEWS_IN_OBJSTORE, "Views cannot be added to an object store.")
#define	ERR_NOTE_INVSIG10				PKG_NSF2+53
	errortext(ERR_NOTE_INVSIG10,		"Document has been modified or corrupted since signed! (extended signature)")
#define ERR_NEEDS_FIXUP					PKG_NSF2+54
	errortext(ERR_NEEDS_FIXUP,			"This database cannot be opened because a consistency check of it is needed.")
#define ERR_FIXUP_IN_PROGRESS	   		PKG_NSF2+55
	errortext(ERR_FIXUP_IN_PROGRESS,	"This database cannot be opened because a consistency check of it is in progress.")
#define	ERR_FILTER_CONTINUING	 		PKG_NSF2+56
	errortext(ERR_FILTER_CONTINUING,	"Error running agent on NoteID %lu - %e.  Continuing...")
#define ERR_OBJSTORE_TITLE				PKG_NSF2+57
	errortext(ERR_OBJSTORE_TITLE,		"Object Store for ")
#define	ERR_NSF_CORRUPT_ACL				PKG_NSF2+58
	errortext(ERR_NSF_CORRUPT_ACL,		"Database ACL is damaged and can't be repaired.")
#define ERR_CONTVERIFY_FREESPACEWRONG	PKG_NSF2+59
	errortext(ERR_CONTVERIFY_FREESPACEWRONG,	"BufAllocSpace: Mismatch in free space, container: %d, (%s).")
#define ERR_DBUHASH_POOLFULL			PKG_NSF2+60
	errortext(ERR_DBUHASH_POOLFULL,		"NSF DBU hash table pool is full.")
#define ERR_RSP_ADDTO_HIERFOLD			PKG_NSF2+61
	errortext(ERR_RSP_ADDTO_HIERFOLD, "To move a response document to a folder that shows response hierarchy you need to move the topmost parent of the response")
#define ERR_RSP_REMFROM_HIERFOLD		PKG_NSF2+62
	errortext(ERR_RSP_REMFROM_HIERFOLD, "To remove a response document from a folder that shows response hierarchy you must also remove the topmost parent of the response")
#define ERR_RSP_REMFROM_HIERFOLD_WARN	PKG_NSF2+63
	errortext(ERR_RSP_REMFROM_HIERFOLD_WARN, "All related response documents are being removed from this folder along with their parents.")
#define ERR_RSP_ADDTO_HIERFOLD_WARN		PKG_NSF2+64
	errortext(ERR_RSP_ADDTO_HIERFOLD_WARN, "All related response documents are being added to this folder along with their parents.")
#define NSFSTR_ENCRYPT_PROGRESS			PKG_NSF2+65
	stringtext(NSFSTR_ENCRYPT_PROGRESS,"Encrypting database")
#define NSFSTR_DECRYPT_PROGRESS			PKG_NSF2+66
	stringtext(NSFSTR_DECRYPT_PROGRESS,"Decrypting database")
#define	ERR_NOT_OBJSTORE				PKG_NSF2+67
	errortext(ERR_NOT_OBJSTORE,			 "The object store required for this operation is not an object store.")
#define	ERR_VIEW_UPDATE					PKG_NSF2+68
	errortext(ERR_VIEW_UPDATE,			 "View and Design notes can only be deleted by using NIFUpdateNote or NIFDeleteNote.")
#define	ERR_NOTE_CLASS					PKG_NSF2+69
	errortext(ERR_NOTE_CLASS,			"Document has invalid Note Class")
#define	ERR_BDB_CORRUPT					PKG_NSF2+71
	errortext(ERR_BDB_CORRUPT,			"Database is damaged and can't be repaired (BDB)")
#define	ERR_INVALID_PARENT_NOTE			PKG_NSF2+72
	errortext(ERR_INVALID_PARENT_NOTE,	"Invalid or nonexistent parent document")
#define	ERR_NOT_CHILD_NOTE				PKG_NSF2+73
	errortext(ERR_NOT_CHILD_NOTE,		"This document is not a child of its parent document")
#define	ERR_LINK_FORMAT					PKG_NSF2+74
	errortext(ERR_LINK_FORMAT,			"Notes Document Link is not formatted properly, or it is not a Notes Document Link")
#define	ERR_DB_MARKED_FOR_DELETE		PKG_NSF2+75
	errortext(ERR_DB_MARKED_FOR_DELETE,	"This database has been marked for delete and cannot be modified")
#define	ERR_RETRY_RW					PKG_NSF2+76
	internaltext(ERR_RETRY_RW,			"Retry this operation with the RW sem")
#define	ERR_UNAME_TEXT					PKG_NSF2+77
	errortext(ERR_UNAME_TEXT,			"Invalid type of data for a Unique Document Name")
#define	ERR_UNAME_LENGTH	  			PKG_NSF2+78
	errortext(ERR_UNAME_LENGTH,			"Unique Document Name is too long")
#define	ERR_UNAME_DIRFULL	  			PKG_NSF2+79
	errortext(ERR_UNAME_DIRFULL,		"Insufficient space in Unique Document Name Directory Pool")
#define	ERR_UNAME_NO_DB		  			PKG_NSF2+80
	errortext(ERR_UNAME_NO_DB,			"Document Database Not Found")
#define	ERR_UNAME_NO_DOC	   			PKG_NSF2+81
	errortext(ERR_UNAME_NO_DOC,			"Document Not Found")
#define ERR_NSF_SCHED_POOLFULL			PKG_NSF2+82
	errortext(ERR_NSF_SCHED_POOLFULL,	"Insufficient memory - Schedule vpool is full.")
#define    ERR_NSF_NO_WEB_CACHE			PKG_NSF2+83
errortext( ERR_NSF_NO_WEB_CACHE,		"The Web retriever database cache could not be opened.  Please check your InterNotes Server value in your current location.")
#define ERR_FIXUP_FOUND_HUGE_UNREADLIST		PKG_NSF2+84
	errortext(ERR_FIXUP_FOUND_HUGE_UNREADLIST,	"Unread list for user %a in database %p is in the new format")
#define ERR_FIXUP_UNLATCH_FAILED		PKG_NSF2+85
	errortext(ERR_FIXUP_UNLATCH_FAILED,	"Unable to change unread list for user %a in database %p to the old format: %e")
#define ERR_FIXUP_UNLATCHED				PKG_NSF2+86
	errortext(ERR_FIXUP_UNLATCHED,		"Changed unread list for user %a in database %p to the old format")
/*	NOTE class names */
#define ERR_NOTE_DESIGN_CLASS_NAME		PKG_NSF2+87
	errortext(ERR_NOTE_DESIGN_CLASS_NAME,"design")
#define ERR_NOTE_VIEW_CLASS_NAME		PKG_NSF2+88
	errortext(ERR_NOTE_VIEW_CLASS_NAME,	"view")
#define ERR_NOTE_DVIEW_CLASS_NAME		PKG_NSF2+89
	errortext(ERR_NOTE_DVIEW_CLASS_NAME,"default view")
#define ERR_NOTE_CLASS_NAME				PKG_NSF2+90
	errortext(ERR_NOTE_CLASS_NAME,		"class")
#define ERR_NOTE_DDESIGN_CLASS_NAME		PKG_NSF2+91
	errortext(ERR_NOTE_DDESIGN_CLASS_NAME,"default design")
#define ERR_NOTE_PVIEW_CLASS_NAME		PKG_NSF2+92
	errortext(ERR_NOTE_PVIEW_CLASS_NAME,"private view")
#define ERR_NOTE_NO_CLASS_NAME			PKG_NSF2+93
	errortext(ERR_NOTE_NO_CLASS_NAME,	"private design")
#define ERR_FTM_NOTSET					PKG_NSF2+94
	errortext(ERR_FTM_NOTSET,			"FTM Context not set for this database")
#define ERR_FTM_ALREADYSET 				PKG_NSF2+95
	errortext(ERR_FTM_ALREADYSET,		"FTM Context already set for this database")
#define ERR_SCHOBJCNTNR					PKG_NSF2+96
	errortext(ERR_SCHOBJCNTNR,	"schedule object container error")
#define ERR_SCHOBJCNTNR_GATEWAY		 	PKG_NSF2+97
	errortext(ERR_SCHOBJCNTNR_GATEWAY,	"schedule object container gateway error")
#define ERR_SCHOBJCNTNR_PARTIAL		 	PKG_NSF2+98
	errortext(ERR_SCHOBJCNTNR_PARTIAL,	"schedule container partially filled")
#define ERR_SCHED_INVALIDDOMAIN			PKG_NSF2+99
	errortext(ERR_SCHED_INVALIDDOMAIN,	"Invalid domain type for scheduling request")
#define ERR_SCHED_NAMENOTUNIQUE			PKG_NSF2+100
	errortext(ERR_SCHED_NAMENOTUNIQUE,	"User name in scheduling request not unique in Name and Address Book")
#define ERR_SCHED_NOSUCHUSER			PKG_NSF2+101
	errortext(ERR_SCHED_NOSUCHUSER,		"User name in scheduling request not found in Name and Address Book")
#define ERR_SCHED_NOGROUP				PKG_NSF2+102
	errortext(ERR_SCHED_NOGROUP,		"Error processing scheduling request, group name not supported")
#define ERR_SCHED_DOMAINNOTFOUND		PKG_NSF2+103
	errortext(ERR_SCHED_DOMAINNOTFOUND,	"Can't find domain record in Name and Address Book for the scheduling request")
#define ERR_SCHED_CANTFINDSCHEDULE		PKG_NSF2+104
	errortext(ERR_SCHED_CANTFINDSCHEDULE, "Can't find schedule record for requested user")
#define ERR_SCHOBJ_NOCOMPOSITE			PKG_NSF2+105
	errortext(ERR_SCHOBJ_NOCOMPOSITE,	"No composite schedule was generated")
#define ERR_SCHOBJ_NOSCHEDLIST			PKG_NSF2+106
	errortext(ERR_SCHOBJ_NOSCHEDLIST,	"No schedule list entry is available for this object")
#define ERR_SCHOBJ_LOCALNOCOMPOSITE		PKG_NSF2+107
	errortext(ERR_SCHOBJ_LOCALNOCOMPOSITE,	"No composite schedule was generated locally")
#define ERR_SCHED_APPT_NOTFOUND			PKG_NSF2+108
	errortext(ERR_SCHED_APPT_NOTFOUND,	"Appointment record not found")
#define ERR_SCHRQST_TIMEOUT				PKG_NSF2+109
	errortext(ERR_SCHRQST_TIMEOUT,		"schedule request timed out")
#define ERR_SCHOBJ_NOTEXIST				PKG_NSF2+110
	errortext(ERR_SCHOBJ_NOTEXIST,		"schedule object doesn't exist")
#define ERR_SCHED_CHAIN_LOOP			PKG_NSF2+111
	errortext(ERR_SCHED_CHAIN_LOOP,		"Schedule request chaining loop detected")
#define	ERR_NSF_CORRUPT_ECL				PKG_NSF2+112
	errortext(ERR_NSF_CORRUPT_ECL,		"ECL is damaged and can't be repaired")
#define	ERR_NSF_COMPUTE_ECL_ABORT		PKG_NSF2+113
	errortext(ERR_NSF_COMPUTE_ECL_ABORT,	"Operation aborted at your request")
#define ERR_BITMAPCHECKSUM_BAD			PKG_NSF2+114
	errortext(ERR_BITMAPCHECKSUM_BAD,	"Bitmap checksum is incorrect")
#define ERR_BAD_BUCKETPOS				PKG_NSF2+115
	internaltext(ERR_BAD_BUCKETPOS,		"Bad bucket position")
#define ERR_DBCACHE_ENTRY_RESERVED		PKG_NSF2+116
	internaltext(ERR_DBCACHE_ENTRY_RESERVED, "DbCache entry reserved")
#define ERR_BUCKET_CORRUPT				PKG_NSF2+117
	internaltext(ERR_BUCKET_CORRUPT, 	"Bucket is corrupt, perhaps partially written")
#define ERR_BAD_BUCKET_SIZE				PKG_NSF2+118
	internaltext(ERR_BAD_BUCKET_SIZE, 	"Bucket size passed to bucket buffer code illegal")
#define ERR_NSF_BUCKET_POOLFULL			PKG_NSF2+119
	errortext(ERR_NSF_BUCKET_POOLFULL, "Insufficient memory - NSF bucket buffer pool is full.")
#define ERR_INVALID_BUCKET_TYPE			PKG_NSF2+120
	errortext(ERR_INVALID_BUCKET_TYPE, "Invalid Bucket Type")
#define	ERR_NETACCOUNT_LASTNAME			PKG_NSF2+121
	errortext(ERR_NETACCOUNT_LASTNAME,	"A valid last name string must be supplied for the second parameter of @NetAccount")
#define ERR_CANTINVALIDATE_BUFFER		PKG_NSF2+122
	internaltext(ERR_CANTINVALIDATE_BUFFER,	"Can't invalidate buffer, pin count > 1")
#define ERR_WRONG_PROCESSGROUP			PKG_NSF2+123
	internaltext(ERR_WRONG_PROCESSGROUP,		"Ager is in wrong process group") 
#define ERR_ITEM_TIME_CORRUPT			PKG_NSF2+124
	internaltext(ERR_ITEM_TIME_CORRUPT,	"Item does not have a valid time") 
#define ERR_SYNC_TIME_NOT_FOUND			PKG_NSF2+125
	internaltext(ERR_SYNC_TIME_NOT_FOUND, "Documents do not have a common revision time") 
#define STR_FREE_TIME_DB_TITLE			PKG_NSF2+126
	stringtext(STR_FREE_TIME_DB_TITLE, 	"Local free time info") 
#define ERR_SCHED_MULTIPLE_DOMAINS		PKG_NSF2+127
	errortext(ERR_SCHED_MULTIPLE_DOMAINS, "Warning: Multiple records detected for domain '%s' in Name & Address Book for the scheduling request.")
/* PKG_NSF2 codes limited to 0-127 */


#define ERR_SCHED_QUERY_SMALLER_DATE_RANGE		PKG_NSF3+1
	errortext(ERR_SCHED_QUERY_SMALLER_DATE_RANGE, "The date range for this busy time query contains too much busy time data. Query a shorter range of days.")
#define ERR_SCHED_RANGEFULL				PKG_NSF3+2
	errortext(ERR_SCHED_RANGEFULL, "The range is full.")
#define ERR_SCHED_TOOMUCHDATA			PKG_NSF3+3
	errortext(ERR_SCHED_TOOMUCHDATA, 	"Too much data is being added to this object. Please break it up into chunks.")
#define ERR_NULL_DBHANDLE				PKG_NSF3+4
	errortext(ERR_NULL_DBHANDLE,		"The database handle is NULL")
#define ERR_INVALID_BUCKET_NUMBER  		PKG_NSF3+5
	errortext(ERR_INVALID_BUCKET_NUMBER,"Invalid Bucket Number")
#define ERR_SCHED_WARNING	  			PKG_NSF3+6
	errortext(ERR_SCHED_WARNING,		"Warning: schedule retrieval error for %s - ")
#define ERR_PROFILE_ENUM_POOLFULL  		PKG_NSF3+7
	errortext(ERR_PROFILE_ENUM_POOLFULL,"Profile document enumeration pool is full")
#define ERR_OBJSTORE_NOT_ENCRYPTED		PKG_NSF3+8
	internaltext(ERR_OBJSTORE_NOT_ENCRYPTED,"Object Store not marked encrypted and must be")

/* PKG_NSF3 codes limited to 0-127 */

/*	End of Error Codes */

#endif

