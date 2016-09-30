// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

/* ODS Error Codes */

#ifndef ODS_ERR_DEFS
#define ODS_ERR_DEFS

#define ERR_ODS			PKG_ODS+1
	errortext(ERR_ODS,		"This database cannot be read due to an invalid on disk structure")
#define ERR_DATATYPE	PKG_ODS+2
	errortext(ERR_DATATYPE,	"Cannot convert field - unsupported datatype")
#define ERR_ODS_TEXT_TOOBIG		PKG_ODS+3
	errortext(ERR_ODS_TEXT_TOOBIG,"Text is too big")
#define ERR_ODS_ENUM_COMPLETE	PKG_ODS+4
	internaltext(ERR_ODS_ENUM_COMPLETE,"Done enumerating CD buffer")
#define ERR_ODS_NO_SUCH_ENTRY	PKG_ODS+5
	apitext(ERR_ODS_NO_SUCH_ENTRY,"No such entry")
#define ERR_ODS_FILE_ZEROLENGTH		PKG_ODS+6
	errortext(ERR_ODS_FILE_ZEROLENGTH,"Zero length scratch file")
#define ERR_ODS_REC_ZEROLENGTH		PKG_ODS+7
	errortext(ERR_ODS_REC_ZEROLENGTH,"Encountered zero length record.")
#define ERR_ODS_REC_UNKNOWN			PKG_ODS+8
	errortext(ERR_ODS_REC_UNKNOWN,	"Encountered unknown record type.")
#define ERR_ODS_SHORT_INPUT			PKG_ODS+9
	debugtext(ERR_ODS_SHORT_INPUT,	"Input text is too short to be canonicalized")
#define ERR_ODS_SHORT_BUFFER		PKG_ODS+10
	errortext(ERR_ODS_SHORT_BUFFER,	"Input buffer is too small for read")

/* NOTE:	PKG_CODE+127 is the LAST code available for this file.  See */
/* globerr.h for dependency which restricts this string ID space. */

#endif

