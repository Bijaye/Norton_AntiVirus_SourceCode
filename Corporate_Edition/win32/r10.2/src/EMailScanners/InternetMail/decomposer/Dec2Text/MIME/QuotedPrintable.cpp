// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************
//
// Description:
//
// Functions to support Quoted-Printable and 7/8 bit encoding/decoding.
//
// Contains:
//  IsValidQPChar()
//  GetQPLineLength()
//  DequoteChar()
//  DecodeQuotedPrintable()
//  WriteQP()
//  Decode8bit()
//  Write8bit()
//  DecodeEncodedWords()
//  MakeEncodedWord()
//  DecodeQBuffer()
//  EncodeQBuffer()
//************************************************************************

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"
#include <ctype.h>
#include "dec_assert.h"

#include "SymSaferStrings.h"
#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"

//
// Note: The AS400 build system performs its builds one directory
// level higher than the other platforms.  It is easier to change
// just the specific AS400 cases rather than change every workspace's
// include search path.
//
#if defined(OS400)
	#include "TextFile.h"
#else
	#include "../TextFile.h"
#endif

#include "MIME.h"

// For our binary_stricmp function
#include "bstricmp.h"

extern volatile DWORD g_dwTextMaxExtractSize;
extern volatile bool g_bTextAbort;

#define DECODE_QP_BUFFER_SIZE	8192
#define LENGTH_UNKNOWN			(size_t)-1

#define NUM_CHARSETS			273

/* This code must be able to run on platforms whose native character sets
are not ASCII (e.g. EBCDIC). All of the strings that come from MIME messages
will still be in ASCII on these platforms because the messages themselves
are generated externally, and therefore do not use the platform's native
character set. In order to make this work, all ASCII constants in the code
must be specified in binary form rather than as quoted string literals. That
way these constants will end up in ASCII regardless of the native character
set for the platform. All future modifications to the code must adhere to
this rule.

Just to make matters a little worse, facilities like case-insensitive string
comparisons won't work on ASCII data when the platform's native character set
is not ASCII. All ASCII data must be treated as binary rather than character
data in this code. Our binary_stricmp function takes a variable string and
compares it with two different versions of a constant string, one in upper-
case and one in lowercase. Since the data in the following vector is used
in case insensitive comparisons, we have expanded it into two vectors, one
for the uppercase and one for the lowercase data. Any modifications must be
kept synchronized between these two vectors. */

static char *szTextMIMECharsetUpper[NUM_CHARSETS] =
{
	"\x55\x53\x2D\x41\x53\x43\x49\x49",                                                 /* US-ASCII */
	"\x41\x53\x43\x49\x49",                                                             /* ASCII */
	"\x49\x53\x4F\x36\x34\x36\x2D\x55\x53",                                             /* ISO646-US */
	"\x49\x53\x4F\x5F\x36\x34\x36\x2E\x49\x52\x56\x3A\x31\x39\x39\x31",                 /* ISO_646.IRV:1991 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x36",                                                 /* ISO-IR-6 */
	"\x41\x4E\x53\x49\x5F\x58\x33\x2E\x34\x2D\x31\x39\x36\x38",                         /* ANSI_X3.4-1968 */
	"\x43\x50\x33\x36\x37",                                                             /* CP367 */
	"\x49\x42\x4D\x33\x36\x37",                                                         /* IBM367 */
	"\x55\x53",                                                                         /* US */
	"\x43\x53\x41\x53\x43\x49\x49",                                                     /* CSASCII */
	"\x55\x54\x46\x2D\x38",                                                             /* UTF-8 */
	"\x55\x43\x53\x2D\x32",                                                             /* UCS-2 */
	"\x49\x53\x4F\x2D\x31\x30\x36\x34\x36\x2D\x55\x43\x53\x2D\x32",                     /* ISO-10646-UCS-2 */
	"\x43\x53\x55\x4E\x49\x43\x4F\x44\x45",                                             /* CSUNICODE */
	"\x55\x43\x53\x2D\x34",                                                             /* UCS-4 */
	"\x49\x53\x4F\x2D\x31\x30\x36\x34\x36\x2D\x55\x43\x53\x2D\x34",                     /* ISO-10646-UCS-4 */
	"\x43\x53\x55\x43\x53\x34",                                                         /* CSUCS4 */
	"\x55\x54\x46\x2D\x31\x36",                                                         /* UTF-16 */
	"\x55\x54\x46\x2D\x37",                                                             /* UTF-7 */
	"\x55\x4E\x49\x43\x4F\x44\x45\x2D\x31\x2D\x31\x2D\x55\x54\x46\x2D\x37",             /* UNICODE-1-1-UTF-7 */
	"\x43\x53\x55\x4E\x49\x43\x4F\x44\x45\x31\x31\x55\x54\x46\x37",                     /* CSUNICODE11UTF7 */
	"\x55\x43\x53\x2D\x32\x2D\x49\x4E\x54\x45\x52\x4E\x41\x4C",                         /* UCS-2-INTERNAL */
	"\x55\x43\x53\x2D\x32\x2D\x53\x57\x41\x50\x50\x45\x44",                             /* UCS-2-SWAPPED */
	"\x55\x43\x53\x2D\x32\x2D\x42\x45",                                                 /* UCS-2-BE */
	"\x55\x4E\x49\x43\x4F\x44\x45\x42\x49\x47",                                         /* UNICODEBIG */
	"\x55\x4E\x49\x43\x4F\x44\x45\x2D\x31\x2D\x31",                                     /* UNICODE-1-1 */
	"\x43\x53\x55\x4E\x49\x43\x4F\x44\x45\x31\x31",                                     /* CSUNICODE11 */
	"\x55\x43\x53\x2D\x32\x2D\x4C\x45",                                                 /* UCS-2-LE */
	"\x55\x4E\x49\x43\x4F\x44\x45\x4C\x49\x54\x54\x4C\x45",                             /* UNICODELITTLE */
	"\x55\x43\x53\x2D\x34\x2D\x49\x4E\x54\x45\x52\x4E\x41\x4C",                         /* UCS-4-INTERNAL */
	"\x55\x43\x53\x2D\x34\x2D\x53\x57\x41\x50\x50\x45\x44",                             /* UCS-4-SWAPPED */
	"\x55\x43\x53\x2D\x34\x2D\x42\x45",                                                 /* UCS-4-BE */
	"\x55\x43\x53\x2D\x34\x2D\x4C\x45",                                                 /* UCS-4-LE */
	"\x4A\x41\x56\x41",                                                                 /* JAVA */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x31",                                         /* ISO-8859-1 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x31",                                         /* ISO_8859-1 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x31\x3A\x31\x39\x38\x37",                     /* ISO_8859-1:1987 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x30\x30",                                         /* ISO-IR-100 */
	"\x43\x50\x38\x31\x39",                                                             /* CP819 */
	"\x49\x42\x4D\x38\x31\x39",                                                         /* IBM819 */
	"\x4C\x41\x54\x49\x4E\x31",                                                         /* LATIN1 */
	"\x4C\x31",                                                                         /* L1 */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x31",                                     /* CSISOLATIN1 */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x32",                                         /* ISO-8859-2 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x32",                                         /* ISO_8859-2 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x32\x3A\x31\x39\x38\x37",                     /* ISO_8859-2:1987 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x30\x31",                                         /* ISO-IR-101 */
	"\x4C\x41\x54\x49\x4E\x32",                                                         /* LATIN2 */
	"\x4C\x32",                                                                         /* L2 */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x32",                                     /* CSISOLATIN2 */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x33",                                         /* ISO-8859-3 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x33",                                         /* ISO_8859-3 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x33\x3A\x31\x39\x38\x38",                     /* ISO_8859-3:1988 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x30\x39",                                         /* ISO-IR-109 */
	"\x4C\x41\x54\x49\x4E\x33",                                                         /* LATIN3 */
	"\x4C\x33",                                                                         /* L3 */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x33",                                     /* CSISOLATIN3 */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x34",                                         /* ISO-8859-4 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x34",                                         /* ISO_8859-4 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x34\x3A\x31\x39\x38\x38",                     /* ISO_8859-4:1988 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x31\x30",                                         /* ISO-IR-110 */
	"\x4C\x41\x54\x49\x4E\x34",                                                         /* LATIN4 */
	"\x4C\x34",                                                                         /* L4 */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x34",                                     /* CSISOLATIN4 */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x35",                                         /* ISO-8859-5 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x35",                                         /* ISO_8859-5 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x35\x3A\x31\x39\x38\x38",                     /* ISO_8859-5:1988 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x34\x34",                                         /* ISO-IR-144 */
	"\x43\x59\x52\x49\x4C\x4C\x49\x43",                                                 /* CYRILLIC */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x43\x59\x52\x49\x4C\x4C\x49\x43",         /* CSISOLATINCYRILLIC */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x36",                                         /* ISO-8859-6 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x36",                                         /* ISO_8859-6 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x36\x3A\x31\x39\x38\x37",                     /* ISO_8859-6:1987 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x32\x37",                                         /* ISO-IR-127 */
	"\x45\x43\x4D\x41\x2D\x31\x31\x34",                                                 /* ECMA-114 */
	"\x41\x53\x4D\x4F\x2D\x37\x30\x38",                                                 /* ASMO-708 */
	"\x41\x52\x41\x42\x49\x43",                                                         /* ARABIC */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x41\x52\x41\x42\x49\x43",                 /* CSISOLATINARABIC */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x37",                                         /* ISO-8859-7 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x37",                                         /* ISO_8859-7 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x37\x3A\x31\x39\x38\x37",                     /* ISO_8859-7:1987 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x32\x36",                                         /* ISO-IR-126 */
	"\x45\x43\x4D\x41\x2D\x31\x31\x38",                                                 /* ECMA-118 */
	"\x45\x4C\x4F\x54\x5F\x39\x32\x38",                                                 /* ELOT_928 */
	"\x47\x52\x45\x45\x4B\x38",                                                         /* GREEK8 */
	"\x47\x52\x45\x45\x4B",                                                             /* GREEK */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x47\x52\x45\x45\x4B",                     /* CSISOLATINGREEK */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x38",                                         /* ISO-8859-8 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x38",                                         /* ISO_8859-8 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x38\x3A\x31\x39\x38\x38",                     /* ISO_8859-8:1988 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x33\x38",                                         /* ISO-IR-138 */
	"\x48\x45\x42\x52\x45\x57",                                                         /* HEBREW */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x48\x45\x42\x52\x45\x57",                 /* CSISOLATINHEBREW */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x39",                                         /* ISO-8859-9 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x39",                                         /* ISO_8859-9 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x39\x3A\x31\x39\x38\x39",                     /* ISO_8859-9:1989 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x34\x38",                                         /* ISO-IR-148 */
	"\x4C\x41\x54\x49\x4E\x35",                                                         /* LATIN5 */
	"\x4C\x35",                                                                         /* L5 */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x35",                                     /* CSISOLATIN5 */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x31\x30",                                     /* ISO-8859-10 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x31\x30",                                     /* ISO_8859-10 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x31\x30\x3A\x31\x39\x39\x32",                 /* ISO_8859-10:1992 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x35\x37",                                         /* ISO-IR-157 */
	"\x4C\x41\x54\x49\x4E\x36",                                                         /* LATIN6 */
	"\x4C\x36",                                                                         /* L6 */
	"\x43\x53\x49\x53\x4F\x4C\x41\x54\x49\x4E\x36",                                     /* CSISOLATIN6 */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x31\x33",                                     /* ISO-8859-13 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x31\x33",                                     /* ISO_8859-13 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x37\x39",                                         /* ISO-IR-179 */
	"\x4C\x41\x54\x49\x4E\x37",                                                         /* LATIN7 */
	"\x4C\x37",                                                                         /* L7 */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x31\x34",                                     /* ISO-8859-14 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x31\x34",                                     /* ISO_8859-14 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x31\x34\x3A\x31\x39\x39\x38",                 /* ISO_8859-14:1998 */
	"\x4C\x41\x54\x49\x4E\x38",                                                         /* LATIN8 */
	"\x4C\x38",                                                                         /* L8 */
	"\x49\x53\x4F\x2D\x38\x38\x35\x39\x2D\x31\x35",                                     /* ISO-8859-15 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x31\x35",                                     /* ISO_8859-15 */
	"\x49\x53\x4F\x5F\x38\x38\x35\x39\x2D\x31\x35\x3A\x31\x39\x39\x38",                 /* ISO_8859-15:1998 */
	"\x4B\x4F\x49\x38\x2D\x52",                                                         /* KOI8-R */
	"\x43\x53\x4B\x4F\x49\x38\x52",                                                     /* CSKOI8R */
	"\x4B\x4F\x49\x38\x2D\x55",                                                         /* KOI8-U */
	"\x4B\x4F\x49\x38\x2D\x52\x55",                                                     /* KOI8-RU */
	"\x43\x50\x31\x32\x35\x30",                                                         /* CP1250 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x31\x32\x35\x30",                                 /* WINDOWS-1250 */
	"\x4D\x53\x2D\x45\x45",                                                             /* MS-EE */
	"\x43\x50\x31\x32\x35\x31",                                                         /* CP1251 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x31\x32\x35\x31",                                 /* WINDOWS-1251 */
	"\x4D\x53\x2D\x43\x59\x52\x4C",                                                     /* MS-CYRL */
	"\x43\x50\x31\x32\x35\x32",                                                         /* CP1252 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x31\x32\x35\x32",                                 /* WINDOWS-1252 */
	"\x4D\x53\x2D\x41\x4E\x53\x49",                                                     /* MS-ANSI */
	"\x43\x50\x31\x32\x35\x33",                                                         /* CP1253 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x31\x32\x35\x33",                                 /* WINDOWS-1253 */
	"\x4D\x53\x2D\x47\x52\x45\x45\x4B",                                                 /* MS-GREEK */
	"\x43\x50\x31\x32\x35\x34",                                                         /* CP1254 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x31\x32\x35\x34",                                 /* WINDOWS-1254 */
	"\x4D\x53\x2D\x54\x55\x52\x4B",                                                     /* MS-TURK */
	"\x43\x50\x31\x32\x35\x35",                                                         /* CP1255 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x31\x32\x35\x35",                                 /* WINDOWS-1255 */
	"\x4D\x53\x2D\x48\x45\x42\x52",                                                     /* MS-HEBR */
	"\x43\x50\x31\x32\x35\x36",                                                         /* CP1256 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x31\x32\x35\x36",                                 /* WINDOWS-1256 */
	"\x4D\x53\x2D\x41\x52\x41\x42",                                                     /* MS-ARAB */
	"\x43\x50\x31\x32\x35\x37",                                                         /* CP1257 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x31\x32\x35\x37",                                 /* WINDOWS-1257 */
	"\x57\x49\x4E\x42\x41\x4C\x54\x52\x49\x4D",                                         /* WINBALTRIM */
	"\x43\x50\x31\x32\x35\x38",                                                         /* CP1258 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x31\x32\x35\x38",                                 /* WINDOWS-1258 */
	"\x43\x50\x38\x35\x30",                                                             /* CP850 */
	"\x49\x42\x4D\x38\x35\x30",                                                         /* IBM850 */
	"\x38\x35\x30",                                                                     /* 850 */
	"\x43\x53\x50\x43\x38\x35\x30\x4D\x55\x4C\x54\x49\x4C\x49\x4E\x47\x55\x41\x4C",     /* CSPC850MULTILINGUAL */
	"\x43\x50\x38\x36\x36",                                                             /* CP866 */
	"\x49\x42\x4D\x38\x36\x36",                                                         /* IBM866 */
	"\x38\x36\x36",                                                                     /* 866 */
	"\x43\x53\x49\x42\x4D\x38\x36\x36",                                                 /* CSIBM866 */
	"\x4D\x41\x43\x52\x4F\x4D\x41\x4E",                                                 /* MACROMAN */
	"\x4D\x41\x43\x49\x4E\x54\x4F\x53\x48",                                             /* MACINTOSH */
	"\x4D\x41\x43",                                                                     /* MAC */
	"\x43\x53\x4D\x41\x43\x49\x4E\x54\x4F\x53\x48",                                     /* CSMACINTOSH */
	"\x4D\x41\x43\x43\x45\x4E\x54\x52\x41\x4C\x45\x55\x52\x4F\x50\x45",                 /* MACCENTRALEUROPE */
	"\x4D\x41\x43\x49\x43\x45\x4C\x41\x4E\x44",                                         /* MACICELAND */
	"\x4D\x41\x43\x43\x52\x4F\x41\x54\x49\x41\x4E",                                     /* MACCROATIAN */
	"\x4D\x41\x43\x52\x4F\x4D\x41\x4E\x49\x41",                                         /* MACROMANIA */
	"\x4D\x41\x43\x43\x59\x52\x49\x4C\x4C\x49\x43",                                     /* MACCYRILLIC */
	"\x4D\x41\x43\x55\x4B\x52\x41\x49\x4E\x45",                                         /* MACUKRAINE */
	"\x4D\x41\x43\x47\x52\x45\x45\x4B",                                                 /* MACGREEK */
	"\x4D\x41\x43\x54\x55\x52\x4B\x49\x53\x48",                                         /* MACTURKISH */
	"\x4D\x41\x43\x48\x45\x42\x52\x45\x57",                                             /* MACHEBREW */
	"\x4D\x41\x43\x41\x52\x41\x42\x49\x43",                                             /* MACARABIC */
	"\x4D\x41\x43\x54\x48\x41\x49",                                                     /* MACTHAI */
	"\x48\x50\x2D\x52\x4F\x4D\x41\x4E\x38",                                             /* HP-ROMAN8 */
	"\x52\x4F\x4D\x41\x4E\x38",                                                         /* ROMAN8 */
	"\x52\x38",                                                                         /* R8 */
	"\x43\x53\x48\x50\x52\x4F\x4D\x41\x4E\x38",                                         /* CSHPROMAN8 */
	"\x4E\x45\x58\x54\x53\x54\x45\x50",                                                 /* NEXTSTEP */
	"\x41\x52\x4D\x53\x43\x49\x49\x2D\x38",                                             /* ARMSCII-8 */
	"\x47\x45\x4F\x52\x47\x49\x41\x4E\x2D\x41\x43\x41\x44\x45\x4D\x59",                 /* GEORGIAN-ACADEMY */
	"\x47\x45\x4F\x52\x47\x49\x41\x4E\x2D\x50\x53",                                     /* GEORGIAN-PS */
	"\x4D\x55\x4C\x45\x4C\x41\x4F\x2D\x31",                                             /* MULELAO-1 */
	"\x43\x50\x31\x31\x33\x33",                                                         /* CP1133 */
	"\x49\x42\x4D\x2D\x43\x50\x31\x31\x33\x33",                                         /* IBM-CP1133 */
	"\x54\x49\x53\x2D\x36\x32\x30",                                                     /* TIS-620 */
	"\x54\x49\x53\x36\x32\x30",                                                         /* TIS620 */
	"\x54\x49\x53\x36\x32\x30\x2D\x30",                                                 /* TIS620-0 */
	"\x54\x49\x53\x36\x32\x30\x2E\x32\x35\x32\x39\x2D\x31",                             /* TIS620.2529-1 */
	"\x54\x49\x53\x36\x32\x30\x2E\x32\x35\x33\x33\x2D\x30",                             /* TIS620.2533-0 */
	"\x54\x49\x53\x36\x32\x30\x2E\x32\x35\x33\x33\x2D\x31",                             /* TIS620.2533-1 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x36\x36",                                         /* ISO-IR-166 */
	"\x43\x50\x38\x37\x34",                                                             /* CP874 */
	"\x57\x49\x4E\x44\x4F\x57\x53\x2D\x38\x37\x34",                                     /* WINDOWS-874 */
	"\x56\x49\x53\x43\x49\x49",                                                         /* VISCII */
	"\x56\x49\x53\x43\x49\x49\x31\x2E\x31\x2D\x31",                                     /* VISCII1.1-1 */
	"\x43\x53\x56\x49\x53\x43\x49\x49",                                                 /* CSVISCII */
	"\x54\x43\x56\x4E",                                                                 /* TCVN */
	"\x54\x43\x56\x4E\x2D\x35\x37\x31\x32",                                             /* TCVN-5712 */
	"\x54\x43\x56\x4E\x35\x37\x31\x32\x2D\x31",                                         /* TCVN5712-1 */
	"\x54\x43\x56\x4E\x35\x37\x31\x32\x2D\x31\x3A\x31\x39\x39\x33",                     /* TCVN5712-1:1993 */
	"\x4A\x49\x53\x5F\x58\x30\x32\x30\x31",                                             /* JIS_X0201 */
	"\x4A\x49\x53\x58\x30\x32\x30\x31\x2D\x31\x39\x37\x36",                             /* JISX0201-1976 */
	"\x4A\x49\x53\x58\x30\x32\x30\x31\x2E\x31\x39\x37\x36\x2D\x30",                     /* JISX0201.1976-0 */
	"\x58\x30\x32\x30\x31",                                                             /* X0201 */
	"\x43\x53\x48\x41\x4C\x46\x57\x49\x44\x54\x48\x4B\x41\x54\x41\x4B\x41\x4E\x41",     /* CSHALFWIDTHKATAKANA */
	"\x4A\x49\x53\x5F\x58\x30\x32\x30\x38",                                             /* JIS_X0208 */
	"\x4A\x49\x53\x5F\x58\x30\x32\x30\x38\x2E\x31\x39\x38\x33\x2D\x30",                 /* JIS_X0208.1983-0 */
	"\x4A\x49\x53\x5F\x58\x30\x32\x30\x38\x2E\x31\x39\x38\x33\x2D\x31",                 /* JIS_X0208.1983-1 */
	"\x4A\x49\x53\x5F\x58\x30\x32\x30\x38\x2D\x31\x39\x38\x33",                         /* JIS_X0208-1983 */
	"\x4A\x49\x53\x5F\x58\x30\x32\x30\x38\x2D\x31\x39\x39\x30",                         /* JIS_X0208-1990 */
	"\x4A\x49\x53\x30\x32\x30\x38",                                                     /* JIS0208 */
	"\x58\x30\x32\x30\x38",                                                             /* X0208 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x38\x37",                                             /* ISO-IR-87 */
	"\x43\x53\x49\x53\x4F\x38\x37\x4A\x49\x53\x58\x30\x32\x30\x38",                     /* CSISO87JISX0208 */
	"\x4A\x49\x53\x5F\x58\x30\x32\x31\x32",                                             /* JIS_X0212 */
	"\x4A\x49\x53\x5F\x58\x30\x32\x31\x32\x2E\x31\x39\x39\x30\x2D\x30",                 /* JIS_X0212.1990-0 */
	"\x4A\x49\x53\x5F\x58\x30\x32\x31\x32\x2D\x31\x39\x39\x30",                         /* JIS_X0212-1990 */
	"\x58\x30\x32\x31\x32",                                                             /* X0212 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x35\x39",                                         /* ISO-IR-159 */
	"\x43\x53\x49\x53\x4F\x31\x35\x39\x4A\x49\x53\x58\x30\x32\x31\x32\x31\x39\x39\x30", /* CSISO159JISX02121990 */
	"\x47\x42\x5F\x32\x33\x31\x32\x2D\x38\x30",                                         /* GB_2312-80 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x35\x38",                                             /* ISO-IR-58 */
	"\x43\x53\x49\x53\x4F\x35\x38\x47\x42\x32\x33\x31\x32\x38\x30",                     /* CSISO58GB231280 */
	"\x43\x48\x49\x4E\x45\x53\x45",                                                     /* CHINESE */
	"\x4B\x53\x43\x5F\x35\x36\x30\x31",                                                 /* KSC_5601 */
	"\x4B\x53\x43\x35\x36\x30\x31\x2E\x31\x39\x38\x37\x2D\x30",                         /* KSC5601.1987-0 */
	"\x4B\x53\x5F\x43\x5F\x35\x36\x30\x31\x2D\x31\x39\x38\x37",                         /* KS_C_5601-1987 */
	"\x4B\x53\x5F\x43\x5F\x35\x36\x30\x31\x2D\x31\x39\x38\x39",                         /* KS_C_5601-1989 */
	"\x49\x53\x4F\x2D\x49\x52\x2D\x31\x34\x39",                                         /* ISO-IR-149 */
	"\x43\x53\x4B\x53\x43\x35\x36\x30\x31\x31\x39\x38\x37",                             /* CSKSC56011987 */
	"\x4B\x4F\x52\x45\x41\x4E",                                                         /* KOREAN */
	"\x43\x50\x39\x34\x39",                                                             /* CP949 */
	"\x45\x55\x43\x2D\x4A\x50",                                                         /* EUC-JP */
	"\x45\x55\x43\x4A\x50",                                                             /* EUCJP */
	"\x45\x58\x54\x45\x4E\x44\x45\x44\x5F\x55\x4E\x49\x58\x5F\x43\x4F\x44\x45\x5F\x50"
	"\x41\x43\x4B\x45\x44\x5F\x46\x4F\x52\x4D\x41\x54\x5F\x46\x4F\x52\x5F\x4A\x41\x50"
	"\x41\x4E\x45\x53\x45",
					 /* EXTENDED_UNIX_CODE_PACKED_FORMAT_FOR_JAPANESE */
	"\x43\x53\x45\x55\x43\x50\x4B\x44\x46\x4D\x54\x4A\x41\x50\x41\x4E\x45\x53\x45",     /* CSEUCPKDFMTJAPANESE */
	"\x53\x4A\x49\x53",                                                                 /* SJIS */
	"\x53\x48\x49\x46\x54\x5F\x4A\x49\x53",                                             /* SHIFT_JIS */
	"\x53\x48\x49\x46\x54\x2D\x4A\x49\x53",                                             /* SHIFT-JIS */
	"\x4D\x53\x5F\x4B\x41\x4E\x4A\x49",                                                 /* MS_KANJI */
	"\x43\x53\x53\x48\x49\x46\x54\x4A\x49\x53",                                         /* CSSHIFTJIS */
	"\x43\x50\x39\x33\x32",                                                             /* CP932 */
	"\x49\x53\x4F\x2D\x32\x30\x32\x32\x2D\x4A\x50",                                     /* ISO-2022-JP */
	"\x43\x53\x49\x53\x4F\x32\x30\x32\x32\x4A\x50",                                     /* CSISO2022JP */
	"\x49\x53\x4F\x2D\x32\x30\x32\x32\x2D\x4A\x50\x2D\x31",                             /* ISO-2022-JP-1 */
	"\x49\x53\x4F\x2D\x32\x30\x32\x32\x2D\x4A\x50\x2D\x32",                             /* ISO-2022-JP-2 */
	"\x45\x55\x43\x2D\x43\x4E",                                                         /* EUC-CN */
	"\x45\x55\x43\x43\x4E",                                                             /* EUCCN */
	"\x47\x42\x32\x33\x31\x32",                                                         /* GB2312 */
	"\x43\x53\x47\x42\x32\x33\x31\x32",                                                 /* CSGB2312 */
	"\x47\x42\x4B",                                                                     /* GBK */
	"\x43\x50\x39\x33\x36",                                                             /* CP936 */
	"\x49\x53\x4F\x2D\x32\x30\x32\x32\x2D\x43\x4E",                                     /* ISO-2022-CN */
	"\x43\x53\x49\x53\x4F\x32\x30\x32\x32\x43\x4E",                                     /* CSISO2022CN */
	"\x49\x53\x4F\x2D\x32\x30\x32\x32\x2D\x43\x4E\x2D\x45\x58\x54",                     /* ISO-2022-CN-EXT */
	"\x48\x5A",                                                                         /* HZ */
	"\x48\x5A\x2D\x47\x42\x2D\x32\x33\x31\x32",                                         /* HZ-GB-2312 */
	"\x45\x55\x43\x2D\x54\x57",                                                         /* EUC-TW */
	"\x45\x55\x43\x54\x57",                                                             /* EUCTW */
	"\x43\x53\x45\x55\x43\x54\x57",                                                     /* CSEUCTW */
	"\x42\x49\x47\x35",                                                                 /* BIG5 */
	"\x42\x49\x47\x2D\x35",                                                             /* BIG-5 */
	"\x42\x49\x47\x2D\x46\x49\x56\x45",                                                 /* BIG-FIVE */
	"\x42\x49\x47\x46\x49\x56\x45",                                                     /* BIGFIVE */
	"\x43\x53\x42\x49\x47\x35",                                                         /* CSBIG5 */
	"\x43\x50\x39\x35\x30",                                                             /* CP950 */
	"\x45\x55\x43\x2D\x4B\x52",                                                         /* EUC-KR */
	"\x45\x55\x43\x4B\x52",                                                             /* EUCKR */
	"\x43\x53\x45\x55\x43\x4B\x52",                                                     /* CSEUCKR */
	"\x4A\x4F\x48\x41\x42",                                                             /* JOHAB */
	"\x43\x50\x31\x33\x36\x31",                                                         /* CP1361 */
	"\x49\x53\x4F\x2D\x32\x30\x32\x32\x2D\x4B\x52",                                     /* ISO-2022-KR */
	"\x43\x53\x49\x53\x4F\x32\x30\x32\x32\x4B\x52"                                      /* CSISO2022KR */
};

static char *szTextMIMECharsetLower[NUM_CHARSETS] =
{
	"\x75\x73\x2D\x61\x73\x63\x69\x69",                                                 /* us-ascii */
	"\x61\x73\x63\x69\x69",                                                             /* ascii */
	"\x69\x73\x6F\x36\x34\x36\x2D\x75\x73",                                             /* iso646-us */
	"\x69\x73\x6F\x5F\x36\x34\x36\x2E\x69\x72\x76\x3A\x31\x39\x39\x31",                 /* iso_646.irv:1991 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x36",                                                 /* iso-ir-6 */
	"\x61\x6E\x73\x69\x5F\x78\x33\x2E\x34\x2D\x31\x39\x36\x38",                         /* ansi_x3.4-1968 */
	"\x63\x70\x33\x36\x37",                                                             /* cp367 */
	"\x69\x62\x6D\x33\x36\x37",                                                         /* ibm367 */
	"\x75\x73",                                                                         /* us */
	"\x63\x73\x61\x73\x63\x69\x69",                                                     /* csascii */
	"\x75\x74\x66\x2D\x38",                                                             /* utf-8 */
	"\x75\x63\x73\x2D\x32",                                                             /* ucs-2 */
	"\x69\x73\x6F\x2D\x31\x30\x36\x34\x36\x2D\x75\x63\x73\x2D\x32",                     /* iso-10646-ucs-2 */
	"\x63\x73\x75\x6E\x69\x63\x6F\x64\x65",                                             /* csunicode */
	"\x75\x63\x73\x2D\x34",                                                             /* ucs-4 */
	"\x69\x73\x6F\x2D\x31\x30\x36\x34\x36\x2D\x75\x63\x73\x2D\x34",                     /* iso-10646-ucs-4 */
	"\x63\x73\x75\x63\x73\x34",                                                         /* csucs4 */
	"\x75\x74\x66\x2D\x31\x36",                                                         /* utf-16 */
	"\x75\x74\x66\x2D\x37",                                                             /* utf-7 */
	"\x75\x6E\x69\x63\x6F\x64\x65\x2D\x31\x2D\x31\x2D\x75\x74\x66\x2D\x37",             /* unicode-1-1-utf-7 */
	"\x63\x73\x75\x6E\x69\x63\x6F\x64\x65\x31\x31\x75\x74\x66\x37",                     /* csunicode11utf7 */
	"\x75\x63\x73\x2D\x32\x2D\x69\x6E\x74\x65\x72\x6E\x61\x6C",                         /* ucs-2-internal */
	"\x75\x63\x73\x2D\x32\x2D\x73\x77\x61\x70\x70\x65\x64",                             /* ucs-2-swapped */
	"\x75\x63\x73\x2D\x32\x2D\x62\x65",                                                 /* ucs-2-be */
	"\x75\x6E\x69\x63\x6F\x64\x65\x62\x69\x67",                                         /* unicodebig */
	"\x75\x6E\x69\x63\x6F\x64\x65\x2D\x31\x2D\x31",                                     /* unicode-1-1 */
	"\x63\x73\x75\x6E\x69\x63\x6F\x64\x65\x31\x31",                                     /* csunicode11 */
	"\x75\x63\x73\x2D\x32\x2D\x6C\x65",                                                 /* ucs-2-le */
	"\x75\x6E\x69\x63\x6F\x64\x65\x6C\x69\x74\x74\x6C\x65",                             /* unicodelittle */
	"\x75\x63\x73\x2D\x34\x2D\x69\x6E\x74\x65\x72\x6E\x61\x6C",                         /* ucs-4-internal */
	"\x75\x63\x73\x2D\x34\x2D\x73\x77\x61\x70\x70\x65\x64",                             /* ucs-4-swapped */
	"\x75\x63\x73\x2D\x34\x2D\x62\x65",                                                 /* ucs-4-be */
	"\x75\x63\x73\x2D\x34\x2D\x6C\x65",                                                 /* ucs-4-le */
	"\x6A\x61\x76\x61",                                                                 /* java */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x31",                                         /* iso-8859-1 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x31",                                         /* iso_8859-1 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x31\x3A\x31\x39\x38\x37",                     /* iso_8859-1:1987 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x30\x30",                                         /* iso-ir-100 */
	"\x63\x70\x38\x31\x39",                                                             /* cp819 */
	"\x69\x62\x6D\x38\x31\x39",                                                         /* ibm819 */
	"\x6C\x61\x74\x69\x6E\x31",                                                         /* latin1 */
	"\x6C\x31",                                                                         /* l1 */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x31",                                     /* csisolatin1 */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x32",                                         /* iso-8859-2 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x32",                                         /* iso_8859-2 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x32\x3A\x31\x39\x38\x37",                     /* iso_8859-2:1987 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x30\x31",                                         /* iso-ir-101 */
	"\x6C\x61\x74\x69\x6E\x32",                                                         /* latin2 */
	"\x6C\x32",                                                                         /* l2 */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x32",                                     /* csisolatin2 */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x33",                                         /* iso-8859-3 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x33",                                         /* iso_8859-3 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x33\x3A\x31\x39\x38\x38",                     /* iso_8859-3:1988 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x30\x39",                                         /* iso-ir-109 */
	"\x6C\x61\x74\x69\x6E\x33",                                                         /* latin3 */
	"\x6C\x33",                                                                         /* l3 */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x33",                                     /* csisolatin3 */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x34",                                         /* iso-8859-4 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x34",                                         /* iso_8859-4 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x34\x3A\x31\x39\x38\x38",                     /* iso_8859-4:1988 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x31\x30",                                         /* iso-ir-110 */
	"\x6C\x61\x74\x69\x6E\x34",                                                         /* latin4 */
	"\x6C\x34",                                                                         /* l4 */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x34",                                     /* csisolatin4 */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x35",                                         /* iso-8859-5 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x35",                                         /* iso_8859-5 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x35\x3A\x31\x39\x38\x38",                     /* iso_8859-5:1988 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x34\x34",                                         /* iso-ir-144 */
	"\x63\x79\x72\x69\x6C\x6C\x69\x63",                                                 /* cyrillic */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x63\x79\x72\x69\x6C\x6C\x69\x63",         /* csisolatincyrillic */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x36",                                         /* iso-8859-6 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x36",                                         /* iso_8859-6 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x36\x3A\x31\x39\x38\x37",                     /* iso_8859-6:1987 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x32\x37",                                         /* iso-ir-127 */
	"\x65\x63\x6D\x61\x2D\x31\x31\x34",                                                 /* ecma-114 */
	"\x61\x73\x6D\x6F\x2D\x37\x30\x38",                                                 /* asmo-708 */
	"\x61\x72\x61\x62\x69\x63",                                                         /* arabic */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x61\x72\x61\x62\x69\x63",                 /* csisolatinarabic */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x37",                                         /* iso-8859-7 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x37",                                         /* iso_8859-7 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x37\x3A\x31\x39\x38\x37",                     /* iso_8859-7:1987 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x32\x36",                                         /* iso-ir-126 */
	"\x65\x63\x6D\x61\x2D\x31\x31\x38",                                                 /* ecma-118 */
	"\x65\x6C\x6F\x74\x5F\x39\x32\x38",                                                 /* elot_928 */
	"\x67\x72\x65\x65\x6B\x38",                                                         /* greek8 */
	"\x67\x72\x65\x65\x6B",                                                             /* greek */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x67\x72\x65\x65\x6B",                     /* csisolatingreek */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x38",                                         /* iso-8859-8 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x38",                                         /* iso_8859-8 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x38\x3A\x31\x39\x38\x38",                     /* iso_8859-8:1988 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x33\x38",                                         /* iso-ir-138 */
	"\x68\x65\x62\x72\x65\x77",                                                         /* hebrew */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x68\x65\x62\x72\x65\x77",                 /* csisolatinhebrew */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x39",                                         /* iso-8859-9 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x39",                                         /* iso_8859-9 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x39\x3A\x31\x39\x38\x39",                     /* iso_8859-9:1989 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x34\x38",                                         /* iso-ir-148 */
	"\x6C\x61\x74\x69\x6E\x35",                                                         /* latin5 */
	"\x6C\x35",                                                                         /* l5 */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x35",                                     /* csisolatin5 */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x31\x30",                                     /* iso-8859-10 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x31\x30",                                     /* iso_8859-10 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x31\x30\x3A\x31\x39\x39\x32",                 /* iso_8859-10:1992 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x35\x37",                                         /* iso-ir-157 */
	"\x6C\x61\x74\x69\x6E\x36",                                                         /* latin6 */
	"\x6C\x36",                                                                         /* l6 */
	"\x63\x73\x69\x73\x6F\x6C\x61\x74\x69\x6E\x36",                                     /* csisolatin6 */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x31\x33",                                     /* iso-8859-13 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x31\x33",                                     /* iso_8859-13 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x37\x39",                                         /* iso-ir-179 */
	"\x6C\x61\x74\x69\x6E\x37",                                                         /* latin7 */
	"\x6C\x37",                                                                         /* l7 */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x31\x34",                                     /* iso-8859-14 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x31\x34",                                     /* iso_8859-14 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x31\x34\x3A\x31\x39\x39\x38",                 /* iso_8859-14:1998 */
	"\x6C\x61\x74\x69\x6E\x38",                                                         /* latin8 */
	"\x6C\x38",                                                                         /* l8 */
	"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x31\x35",                                     /* iso-8859-15 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x31\x35",                                     /* iso_8859-15 */
	"\x69\x73\x6F\x5F\x38\x38\x35\x39\x2D\x31\x35\x3A\x31\x39\x39\x38",                 /* iso_8859-15:1998 */
	"\x6B\x6F\x69\x38\x2D\x72",                                                         /* koi8-r */
	"\x63\x73\x6B\x6F\x69\x38\x72",                                                     /* cskoi8r */
	"\x6B\x6F\x69\x38\x2D\x75",                                                         /* koi8-u */
	"\x6B\x6F\x69\x38\x2D\x72\x75",                                                     /* koi8-ru */
	"\x63\x70\x31\x32\x35\x30",                                                         /* cp1250 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x31\x32\x35\x30",                                 /* windows-1250 */
	"\x6D\x73\x2D\x65\x65",                                                             /* ms-ee */
	"\x63\x70\x31\x32\x35\x31",                                                         /* cp1251 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x31\x32\x35\x31",                                 /* windows-1251 */
	"\x6D\x73\x2D\x63\x79\x72\x6C",                                                     /* ms-cyrl */
	"\x63\x70\x31\x32\x35\x32",                                                         /* cp1252 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x31\x32\x35\x32",                                 /* windows-1252 */
	"\x6D\x73\x2D\x61\x6E\x73\x69",                                                     /* ms-ansi */
	"\x63\x70\x31\x32\x35\x33",                                                         /* cp1253 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x31\x32\x35\x33",                                 /* windows-1253 */
	"\x6D\x73\x2D\x67\x72\x65\x65\x6B",                                                 /* ms-greek */
	"\x63\x70\x31\x32\x35\x34",                                                         /* cp1254 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x31\x32\x35\x34",                                 /* windows-1254 */
	"\x6D\x73\x2D\x74\x75\x72\x6B",                                                     /* ms-turk */
	"\x63\x70\x31\x32\x35\x35",                                                         /* cp1255 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x31\x32\x35\x35",                                 /* windows-1255 */
	"\x6D\x73\x2D\x68\x65\x62\x72",                                                     /* ms-hebr */
	"\x63\x70\x31\x32\x35\x36",                                                         /* cp1256 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x31\x32\x35\x36",                                 /* windows-1256 */
	"\x6D\x73\x2D\x61\x72\x61\x62",                                                     /* ms-arab */
	"\x63\x70\x31\x32\x35\x37",                                                         /* cp1257 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x31\x32\x35\x37",                                 /* windows-1257 */
	"\x77\x69\x6E\x62\x61\x6C\x74\x72\x69\x6D",                                         /* winbaltrim */
	"\x63\x70\x31\x32\x35\x38",                                                         /* cp1258 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x31\x32\x35\x38",                                 /* windows-1258 */
	"\x63\x70\x38\x35\x30",                                                             /* cp850 */
	"\x69\x62\x6D\x38\x35\x30",                                                         /* ibm850 */
	"\x38\x35\x30",                                                                     /* 850 */
	"\x63\x73\x70\x63\x38\x35\x30\x6D\x75\x6C\x74\x69\x6C\x69\x6E\x67\x75\x61\x6C",     /* cspc850multilingual */
	"\x63\x70\x38\x36\x36",                                                             /* cp866 */
	"\x69\x62\x6D\x38\x36\x36",                                                         /* ibm866 */
	"\x38\x36\x36",                                                                     /* 866 */
	"\x63\x73\x69\x62\x6D\x38\x36\x36",                                                 /* csibm866 */
	"\x6D\x61\x63\x72\x6F\x6D\x61\x6E",                                                 /* macroman */
	"\x6D\x61\x63\x69\x6E\x74\x6F\x73\x68",                                             /* macintosh */
	"\x6D\x61\x63",                                                                     /* mac */
	"\x63\x73\x6D\x61\x63\x69\x6E\x74\x6F\x73\x68",                                     /* csmacintosh */
	"\x6D\x61\x63\x63\x65\x6E\x74\x72\x61\x6C\x65\x75\x72\x6F\x70\x65",                 /* maccentraleurope */
	"\x6D\x61\x63\x69\x63\x65\x6C\x61\x6E\x64",                                         /* maciceland */
	"\x6D\x61\x63\x63\x72\x6F\x61\x74\x69\x61\x6E",                                     /* maccroatian */
	"\x6D\x61\x63\x72\x6F\x6D\x61\x6E\x69\x61",                                         /* macromania */
	"\x6D\x61\x63\x63\x79\x72\x69\x6C\x6C\x69\x63",                                     /* maccyrillic */
	"\x6D\x61\x63\x75\x6B\x72\x61\x69\x6E\x65",                                         /* macukraine */
	"\x6D\x61\x63\x67\x72\x65\x65\x6B",                                                 /* macgreek */
	"\x6D\x61\x63\x74\x75\x72\x6B\x69\x73\x68",                                         /* macturkish */
	"\x6D\x61\x63\x68\x65\x62\x72\x65\x77",                                             /* machebrew */
	"\x6D\x61\x63\x61\x72\x61\x62\x69\x63",                                             /* macarabic */
	"\x6D\x61\x63\x74\x68\x61\x69",                                                     /* macthai */
	"\x68\x70\x2D\x72\x6F\x6D\x61\x6E\x38",                                             /* hp-roman8 */
	"\x72\x6F\x6D\x61\x6E\x38",                                                         /* roman8 */
	"\x72\x38",                                                                         /* r8 */
	"\x63\x73\x68\x70\x72\x6F\x6D\x61\x6E\x38",                                         /* cshproman8 */
	"\x6E\x65\x78\x74\x73\x74\x65\x70",                                                 /* nextstep */
	"\x61\x72\x6D\x73\x63\x69\x69\x2D\x38",                                             /* armscii-8 */
	"\x67\x65\x6F\x72\x67\x69\x61\x6E\x2D\x61\x63\x61\x64\x65\x6D\x79",                 /* georgian-academy */
	"\x67\x65\x6F\x72\x67\x69\x61\x6E\x2D\x70\x73",                                     /* georgian-ps */
	"\x6D\x75\x6C\x65\x6C\x61\x6F\x2D\x31",                                             /* mulelao-1 */
	"\x63\x70\x31\x31\x33\x33",                                                         /* cp1133 */
	"\x69\x62\x6D\x2D\x63\x70\x31\x31\x33\x33",                                         /* ibm-cp1133 */
	"\x74\x69\x73\x2D\x36\x32\x30",                                                     /* tis-620 */
	"\x74\x69\x73\x36\x32\x30",                                                         /* tis620 */
	"\x74\x69\x73\x36\x32\x30\x2D\x30",                                                 /* tis620-0 */
	"\x74\x69\x73\x36\x32\x30\x2E\x32\x35\x32\x39\x2D\x31",                             /* tis620.2529-1 */
	"\x74\x69\x73\x36\x32\x30\x2E\x32\x35\x33\x33\x2D\x30",                             /* tis620.2533-0 */
	"\x74\x69\x73\x36\x32\x30\x2E\x32\x35\x33\x33\x2D\x31",                             /* tis620.2533-1 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x36\x36",                                         /* iso-ir-166 */
	"\x63\x70\x38\x37\x34",                                                             /* cp874 */
	"\x77\x69\x6E\x64\x6F\x77\x73\x2D\x38\x37\x34",                                     /* windows-874 */
	"\x76\x69\x73\x63\x69\x69",                                                         /* viscii */
	"\x76\x69\x73\x63\x69\x69\x31\x2E\x31\x2D\x31",                                     /* viscii1.1-1 */
	"\x63\x73\x76\x69\x73\x63\x69\x69",                                                 /* csviscii */
	"\x74\x63\x76\x6E",                                                                 /* tcvn */
	"\x74\x63\x76\x6E\x2D\x35\x37\x31\x32",                                             /* tcvn-5712 */
	"\x74\x63\x76\x6E\x35\x37\x31\x32\x2D\x31",                                         /* tcvn5712-1 */
	"\x74\x63\x76\x6E\x35\x37\x31\x32\x2D\x31\x3A\x31\x39\x39\x33",                     /* tcvn5712-1:1993 */
	"\x6A\x69\x73\x5F\x78\x30\x32\x30\x31",                                             /* jis_x0201 */
	"\x6A\x69\x73\x78\x30\x32\x30\x31\x2D\x31\x39\x37\x36",                             /* jisx0201-1976 */
	"\x6A\x69\x73\x78\x30\x32\x30\x31\x2E\x31\x39\x37\x36\x2D\x30",                     /* jisx0201.1976-0 */
	"\x78\x30\x32\x30\x31",                                                             /* x0201 */
	"\x63\x73\x68\x61\x6C\x66\x77\x69\x64\x74\x68\x6B\x61\x74\x61\x6B\x61\x6E\x61",     /* cshalfwidthkatakana */
	"\x6A\x69\x73\x5F\x78\x30\x32\x30\x38",                                             /* jis_x0208 */
	"\x6A\x69\x73\x5F\x78\x30\x32\x30\x38\x2E\x31\x39\x38\x33\x2D\x30",                 /* jis_x0208.1983-0 */
	"\x6A\x69\x73\x5F\x78\x30\x32\x30\x38\x2E\x31\x39\x38\x33\x2D\x31",                 /* jis_x0208.1983-1 */
	"\x6A\x69\x73\x5F\x78\x30\x32\x30\x38\x2D\x31\x39\x38\x33",                         /* jis_x0208-1983 */
	"\x6A\x69\x73\x5F\x78\x30\x32\x30\x38\x2D\x31\x39\x39\x30",                         /* jis_x0208-1990 */
	"\x6A\x69\x73\x30\x32\x30\x38",                                                     /* jis0208 */
	"\x78\x30\x32\x30\x38",                                                             /* x0208 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x38\x37",                                             /* iso-ir-87 */
	"\x63\x73\x69\x73\x6F\x38\x37\x6A\x69\x73\x78\x30\x32\x30\x38",                     /* csiso87jisx0208 */
	"\x6A\x69\x73\x5F\x78\x30\x32\x31\x32",                                             /* jis_x0212 */
	"\x6A\x69\x73\x5F\x78\x30\x32\x31\x32\x2E\x31\x39\x39\x30\x2D\x30",                 /* jis_x0212.1990-0 */
	"\x6A\x69\x73\x5F\x78\x30\x32\x31\x32\x2D\x31\x39\x39\x30",                         /* jis_x0212-1990 */
	"\x78\x30\x32\x31\x32",                                                             /* x0212 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x35\x39",                                         /* iso-ir-159 */
	"\x63\x73\x69\x73\x6F\x31\x35\x39\x6A\x69\x73\x78\x30\x32\x31\x32\x31\x39\x39\x30", /* csiso159jisx02121990 */
	"\x67\x62\x5F\x32\x33\x31\x32\x2D\x38\x30",                                         /* gb_2312-80 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x35\x38",                                             /* iso-ir-58 */
	"\x63\x73\x69\x73\x6F\x35\x38\x67\x62\x32\x33\x31\x32\x38\x30",                     /* csiso58gb231280 */
	"\x63\x68\x69\x6E\x65\x73\x65",                                                     /* chinese */
	"\x6B\x73\x63\x5F\x35\x36\x30\x31",                                                 /* ksc_5601 */
	"\x6B\x73\x63\x35\x36\x30\x31\x2E\x31\x39\x38\x37\x2D\x30",                         /* ksc5601.1987-0 */
	"\x6B\x73\x5F\x63\x5F\x35\x36\x30\x31\x2D\x31\x39\x38\x37",                         /* ks_c_5601-1987 */
	"\x6B\x73\x5F\x63\x5F\x35\x36\x30\x31\x2D\x31\x39\x38\x39",                         /* ks_c_5601-1989 */
	"\x69\x73\x6F\x2D\x69\x72\x2D\x31\x34\x39",                                         /* iso-ir-149 */
	"\x63\x73\x6B\x73\x63\x35\x36\x30\x31\x31\x39\x38\x37",                             /* csksc56011987 */
	"\x6B\x6F\x72\x65\x61\x6E",                                                         /* korean */
	"\x63\x70\x39\x34\x39",                                                             /* cp949 */
	"\x65\x75\x63\x2D\x6A\x70",                                                         /* euc-jp */
	"\x65\x75\x63\x6A\x70",                                                             /* eucjp */
	"\x65\x78\x74\x65\x6E\x64\x65\x64\x5F\x75\x6E\x69\x78\x5F\x63\x6F\x64\x65\x5F\x70"
	"\x61\x63\x6B\x65\x64\x5F\x66\x6F\x72\x6D\x61\x74\x5F\x66\x6F\x72\x5F\x6A\x61\x70"
	"\x61\x6E\x65\x73\x65",
					 /* extended_unix_code_packed_format_for_japanese */
	"\x63\x73\x65\x75\x63\x70\x6B\x64\x66\x6D\x74\x6A\x61\x70\x61\x6E\x65\x73\x65",     /* cseucpkdfmtjapanese */
	"\x73\x6A\x69\x73",                                                                 /* sjis */
	"\x73\x68\x69\x66\x74\x5F\x6A\x69\x73",                                             /* shift_jis */
	"\x73\x68\x69\x66\x74\x2D\x6A\x69\x73",                                             /* shift-jis */
	"\x6D\x73\x5F\x6B\x61\x6E\x6A\x69",                                                 /* ms_kanji */
	"\x63\x73\x73\x68\x69\x66\x74\x6A\x69\x73",                                         /* csshiftjis */
	"\x63\x70\x39\x33\x32",                                                             /* cp932 */
	"\x69\x73\x6F\x2D\x32\x30\x32\x32\x2D\x6A\x70",                                     /* iso-2022-jp */
	"\x63\x73\x69\x73\x6F\x32\x30\x32\x32\x6A\x70",                                     /* csiso2022jp */
	"\x69\x73\x6F\x2D\x32\x30\x32\x32\x2D\x6A\x70\x2D\x31",                             /* iso-2022-jp-1 */
	"\x69\x73\x6F\x2D\x32\x30\x32\x32\x2D\x6A\x70\x2D\x32",                             /* iso-2022-jp-2 */
	"\x65\x75\x63\x2D\x63\x6E",                                                         /* euc-cn */
	"\x65\x75\x63\x63\x6E",                                                             /* euccn */
	"\x67\x62\x32\x33\x31\x32",                                                         /* gb2312 */
	"\x63\x73\x67\x62\x32\x33\x31\x32",                                                 /* csgb2312 */
	"\x67\x62\x6B",                                                                     /* gbk */
	"\x63\x70\x39\x33\x36",                                                             /* cp936 */
	"\x69\x73\x6F\x2D\x32\x30\x32\x32\x2D\x63\x6E",                                     /* iso-2022-cn */
	"\x63\x73\x69\x73\x6F\x32\x30\x32\x32\x63\x6E",                                     /* csiso2022cn */
	"\x69\x73\x6F\x2D\x32\x30\x32\x32\x2D\x63\x6E\x2D\x65\x78\x74",                     /* iso-2022-cn-ext */
	"\x68\x7A",                                                                         /* hz */
	"\x68\x7A\x2D\x67\x62\x2D\x32\x33\x31\x32",                                         /* hz-gb-2312 */
	"\x65\x75\x63\x2D\x74\x77",                                                         /* euc-tw */
	"\x65\x75\x63\x74\x77",                                                             /* euctw */
	"\x63\x73\x65\x75\x63\x74\x77",                                                     /* cseuctw */
	"\x62\x69\x67\x35",                                                                 /* big5 */
	"\x62\x69\x67\x2D\x35",                                                             /* big-5 */
	"\x62\x69\x67\x2D\x66\x69\x76\x65",                                                 /* big-five */
	"\x62\x69\x67\x66\x69\x76\x65",                                                     /* bigfive */
	"\x63\x73\x62\x69\x67\x35",                                                         /* csbig5 */
	"\x63\x70\x39\x35\x30",                                                             /* cp950 */
	"\x65\x75\x63\x2D\x6B\x72",                                                         /* euc-kr */
	"\x65\x75\x63\x6B\x72",                                                             /* euckr */
	"\x63\x73\x65\x75\x63\x6B\x72",                                                     /* cseuckr */
	"\x6A\x6F\x68\x61\x62",                                                             /* johab */
	"\x63\x70\x31\x33\x36\x31",                                                         /* cp1361 */
	"\x69\x73\x6F\x2D\x32\x30\x32\x32\x2D\x6B\x72",                                     /* iso-2022-kr */
	"\x63\x73\x69\x73\x6F\x32\x30\x32\x32\x6B\x72"                                      /* csiso2022kr */
};

///////////////////////////////////////////////////////////////////////////////
// Quoted-Printable decoding...
///////////////////////////////////////////////////////////////////////////////

//************************************************************************
// bool IsValidQPChar(cNextChar)
//
// Purpose
//
//  This function returns true if the specified character is a valid quoted
//  printable character and false otherwise.
//
// Parameters:
//
//  cNextChar: IN=The character to check.
//
// Returns:
//
//  true if the character is a valid QP character
//  false otherwise
//************************************************************************
bool CMIMEParser::IsValidQPChar(char cNextChar)
{
	if ((cNextChar >= '\x21') && (cNextChar <= '\x3C'))  /* exclamation point, less than */
	{	
		return true;
	}

	if ((cNextChar >= '\x3E') && (cNextChar <= '\x7E'))  /* greater than, tilde */
	{	
		return true;
	}

	if ((cNextChar == '\x20') || (cNextChar == '\x09'))  /* space, tab */
	{	
		return true;
	}

	if (cNextChar == '\x3D')  /* = */
	{	
		return true;
	}

	if ((cNextChar == '\x0A') || (cNextChar == '\x0D'))  /* LF, CR */
	{	
		return true;
	}

	return false;
}


//************************************************************************
// size_t GetQPLineLength(pBuffer, nCount, pnWhitespaceLength)
//
// Purpose
//
//  This function strips extra white-spaces from the end of
//  quoted-printable lines of text in a buffer.
//
// Parameters:
//
//  lpszLine: IN/OUT=The line to strip
//
// Returns:
//
//  nothing
//************************************************************************
size_t CMIMEParser::GetQPLineLength(char *pBuffer, size_t nCount, size_t *pnWhitespaceLength)
{
	size_t	nIndex = 0;
	size_t	nWhitespaceOffset = LENGTH_UNKNOWN;
	*pnWhitespaceLength = 0;
	char	ch;

	dec_assert(pnWhitespaceLength);

	while (nIndex < nCount)
	{
		ch = pBuffer[nIndex];
		if (ch == ASC_CHR_SPACE ||
			ch == ASC_CHR_TAB)
		{
			if (nWhitespaceOffset == LENGTH_UNKNOWN)
			    nWhitespaceOffset = nIndex;
		}
		else if (ch == ASC_CHR_CR ||
			     ch == ASC_CHR_LF)
		{
			if (nWhitespaceOffset == LENGTH_UNKNOWN)
				*pnWhitespaceLength = 0;
			else
				*pnWhitespaceLength = nIndex - nWhitespaceOffset;

			if (nWhitespaceOffset == LENGTH_UNKNOWN)
				return (nIndex);

			return (nWhitespaceOffset);
		}
		else
		{
			nWhitespaceOffset = LENGTH_UNKNOWN;
		}

		nIndex++;
	}

	return (LENGTH_UNKNOWN);
}


//************************************************************************
// void DequoteChar(lpszHex, lpcChar)
//
// Purpose
//
//  This function converts a 2-character ascii string in HEX format to the
//  corresponding character.  "0D" would be converted to a CR character.
//
// Parameters:
//
//  lpszHex: IN=The 2-character hex string
//  lpcChar: OUT=The corresponding character
//
// Returns:
//
//  true if the input string was a valid hex #.
//  false otherwise
//************************************************************************
bool CMIMEParser::DequoteChar(char *lpszHex, char *lpcChar)
{
	char	cLow, cHigh, c0, c1;

	cLow = cHigh = 0;
	c0 = lpszHex[0];
	c1 = lpszHex[1];
	if ((c0 >= '\x41') && (c0 <= '\x46'))  /* A - F */
	{	
		cHigh = c0 - '\x41' + 10;
	}
	else if ((c0 >= '\x61') && (c0 <= '\x66'))  /* a - f */
	{	
		cHigh = c0 - '\x61' + 10;
	}
	else if ((c0 >= '\x30') && (c0 <= '\x39'))  /* 0 - 9 */
	{	
		cHigh = c0 - '\x30';
	}
	else 
	{
		return false;
	}

	if ((c1 >= '\x41') && (c1 <= '\x46')) 
	{
		cLow = c1 - '\x41' + 10;
	}
	else if ((c1 >= '\x61') && (c1 <= '\x66'))
	{
		cLow = c1 - '\x61' + 10;
	}
	else if ((c1 >= '\x30') && (c1 <= '\x39'))
	{
		cLow = c1 - '\x30';
	}
	else
	{
		return false;
	}

	*lpcChar = (cHigh << 4) | cLow;
	return (true);
}


//************************************************************************
// int DecodeQuotedPrintable(lpstInput,
//                           dwStartOffset,
//                           dwEndOffset,
//                           lpstOutput)
//
// Purpose
//
//  This function decodes a QP stream of data starting at offset
//  dwStartOffset and ending at dwEndOffset.  It writes the output file to
//  lpstOutput.
//
// Parameters:
//
//  lpstInput: IN/OUT=The MIME input stream
//  dwStartOffset: IN=Start offset of the encoded QP data in the MIME stream
//  dwEndOffset: IN=End offset of the encoded QP data in the MIME stream
//  lpstOutput: IN/OUT=The destination stream where to write our data
//
// Returns:
//
//  PARSER_OK if the QP section is properly decoded
//  PARSER_ERROR otherwise
//************************************************************************
int CMIMEParser::DecodeQuotedPrintable(
	FILE	*lpstInput,
	DWORD	dwStartOffset,
	DWORD	dwEndOffset,
	FILE	*lpstOutput)
{
	int		rc = PARSER_ERROR;
	size_t	nBytesLeft;
	char	*pin = NULL;
	char	*pout = NULL;
	DWORD	dwTotalBytesWritten = 0;
	size_t	nIndex;

	pin = (char *)malloc(DECODE_QP_BUFFER_SIZE);
	pout = (char *)malloc(DECODE_QP_BUFFER_SIZE);
	if (!pin || !pout)
		goto decode_qp_done;

	// Seek to the start of the QP encoded data in the input stream
	if (dec_fseek(lpstInput, dwStartOffset, SEEK_SET) != 0)
		goto decode_qp_done;

	// Seek to the top of the output file
	if (dec_fseek(lpstOutput, 0, SEEK_SET) != 0)
		goto decode_qp_done;

	nIndex = 0;
	nBytesLeft = (dwEndOffset - dwStartOffset) + 1;

	while (nBytesLeft != 0)
	{
		size_t	nCount;
		size_t	nDecoded;

		// Fill the input buffer.
		nCount = DECODE_QP_BUFFER_SIZE - nIndex;
		if (nCount > nBytesLeft - nIndex)
			nCount = nBytesLeft - nIndex;

		if (nCount > 0)
		{
			nCount = dec_fread(&pin[nIndex], 1, nCount, lpstInput);
			if (nCount == 0)
			{
				if (dec_ferror(lpstInput))
					goto decode_qp_done;	// Read error - return decoding failed.

				// Hit EOF so we're done decoding - return decoding succeeded.
				break;
			}
			else if (	(nCount+nIndex == DECODE_QP_BUFFER_SIZE)
					 &&	(pin[DECODE_QP_BUFFER_SIZE-1] == ASC_CHR_CR) )
			{
				// We're here if the entire buffer was filled and the last
				// character was a CR.  We need to exclude the last CR to avoid
				// the possibly splitting a CR/LF sequence across two buffer 
				// reads.  The last CR will be picked up as the first character 
				// on the next iteration through the loop.

				if ( dec_fseek(lpstInput,dec_ftell(lpstInput)-1,SEEK_SET) != 0 )
				{
					// seek failed.
					rc = PARSER_ERROR;
					goto decode_qp_done;
				}
				else
				{
					nCount--; // act as if we read one less byte than we actually read above
				}
			}
		}

		nCount += nIndex;
		nDecoded = DecodeQuotedPrintableBuffer(pin, nCount, pout, DECODE_QP_BUFFER_SIZE, &nIndex);

		nBytesLeft -= nIndex;
		if (nBytesLeft == 0)
		{
			// This is the end of the input data.  If the last two input characters are <CRLF>,
			// do not write them to the output.
			if (nCount >= 2 && nDecoded >= 2)
			{
				if (pin[nCount - 2] == ASC_CHR_CR &&
					pin[nCount - 1]  == ASC_CHR_LF &&
					pout[nDecoded-2] == ASC_CHR_CR &&
					pout[nDecoded-1] == ASC_CHR_LF)
				{
					nDecoded -= 2;
				}
			}
			else if (nCount == 1 && nDecoded == 1)
			{
				// If the last character is a lone <CR> or <LF>,
				// do not write it to the output.
				if (pin[0] == ASC_CHR_CR ||
					pin[0] == ASC_CHR_LF)
				{
					nDecoded--;
				}
			}
		}

		// Write the decoded buffer.
		if (dec_fwrite(pout, 1, nDecoded, lpstOutput) != nDecoded)
			goto decode_qp_done;	// Return decoding error.

		dwTotalBytesWritten += nDecoded;

		// Does this object violate our MaxExtractSize policy?
		if (g_dwTextMaxExtractSize &&
			dwTotalBytesWritten >= g_dwTextMaxExtractSize)
		{
			rc = PARSER_MAX_EXTRACT_SIZE;
			goto decode_qp_done;
		}

		// Should we abort?
		if (g_bTextAbort)
		{
			rc = PARSER_USER_ABORT;
			goto decode_qp_done;
		}

		if (nIndex < nCount)
		{
			memcpy(pin, &pin[nIndex], nCount - nIndex);
			nIndex = nCount - nIndex;
		}
		else
		{
			nIndex = 0;
		}
	}

	rc = PARSER_OK;

decode_qp_done:
	if (pin)
		free(pin);
	if (pout)
		free(pout);

	return (rc);
}


//************************************************************************
// size_t DecodeQuotedPrintableBuffer(pIn,
//									  nInBytes,
//									  pOut,
//									  nOutBytes,
//									  pnIndex)
//
// Purpose
//
//  This function decodes a buffer containing quoted-printable data.
//
// Parameters:
//
//  pIn:		Pointer to input buffer
//  nInBytes:	Size in bytes of the pIn input buffer
//  pOut:		Pointer to output buffer
//  nOutBytes:	Size in bytes of the pOut output buffer
//
// Returns:
//
//  The number of bytes decoded/returned in the pOut output buffer.
//************************************************************************
size_t CMIMEParser::DecodeQuotedPrintableBuffer(
	char	*pIn,
	size_t	nInBytes,
	char	*pOut,
	size_t	nOutBytes,
	size_t	*pnIndex)
{
	size_t	nBytesReturned = 0;  // Number of bytes returned in the pOut buffer.
	size_t	nWhitespaceLength;
	size_t	nLength;
	size_t	nInOffset;
	size_t	nOutOffset;
	size_t	i;

	nInOffset = 0;	// Current input buffer offset.
	nOutOffset = 0;	// Current output buffer offset.

	while (nInOffset < nInBytes)
	{
		// According to RFC1521 section 5.1, we must not decode white space
		// that appears at the end of an encoded quoted-printable line.
		nLength = GetQPLineLength(&pIn[nInOffset], nInBytes - nInOffset, &nWhitespaceLength);
		if (nLength == LENGTH_UNKNOWN)
		{
			// Could not locate the end of the current line within the buffer.
			if (nInOffset != 0)
			{
				// The assumption here is that the caller did not split this buffer
				// on a multi-byte encoding sequence (like "=0D").
				// Return what we were able to decode so far and have the caller
				// send in another buffer.
				break;
			}

			// The buffer does not terminate quite the way we would like it to,
			// but since the entire buffer contains a single line of quoted-printable,
			// go ahead and decode the entire line as best we can.
			nLength = nInBytes;
		}

		bool	bIndexSet;

		bIndexSet = false;

		if (nLength == 0)
		{
			// We are on a <CR> or <LF> hard line-break.  Just emit the character.
			i = nInOffset++;
			goto write_qp_byte;
		}
		else
		{
			// Run through the buffer and output the literal data.
			for (i = nInOffset; i < nInOffset + nLength; i++)
			{
				if (IsValidQPChar(pIn[i]) == false)
					goto write_qp_byte;

				// =<CR> or =<LF> or =<CRLF> is a soft line-break.
				if (pIn[i] == ASC_CHR_EQ)
				{
					if (pIn[i+1] == ASC_CHR_CR)
					{
						if (pIn[i+2] == ASC_CHR_LF)
						{
							// =<CRLF>
							nInOffset = i + 3;		// Point to start of next line.
							bIndexSet = true;
							break;				// last char on line...
						}

						// =<CR>
						nInOffset = i + 2;		// Point to start of next line.
						bIndexSet = true;
						break;				// last char on line...
					}

					if (pIn[i+1] == ASC_CHR_LF)
					{
						// =<LF>
						nInOffset = i + 2;		// Point to start of next line.
						bIndexSet = true;
						break;				// last char on line...
					}

					if (pIn[i+2] == 0)
						goto write_qp_byte;

					// We now know we have enough information to at least try
					// to dequote the character.
					char    cChar;

					if (DequoteChar(&pIn[i + 1], &cChar) == false)
						goto write_qp_byte;

					pOut[nOutOffset] = cChar;
					nOutOffset++;
					i += 2;	// Skip over the hex representation.
					continue;
				}

				if (pIn[i] == ASC_CHR_CR ||
					pIn[i] == ASC_CHR_LF)
				{
					nInOffset = i + 1;		// Point to start of next line.
					bIndexSet = true;
				}

write_qp_byte:
				pOut[nOutOffset] = pIn[i];
				nOutOffset++;
			}

			if (!bIndexSet)
				nInOffset = i + nWhitespaceLength;
		}
	}

	*pnIndex = nInOffset;
	return (nOutOffset);
}


//************************************************************************
// int WriteQP(dwCur,
//             dwNext,
//             lpstFile,
//             lpszOutputFileName)
//
// Purpose
//
//  This function opens the specified output file and truncates its contents.
//  It then proceeds to decode a QP encoded section of the file that starts
//  at dwCur and ends at dwNext.  Finally, it closes the output file.
//
// Parameters:
//
//  dwCur: IN=Start of the QP section
//  dwNext: OUT=Start of the next section (end of the QP section)
//  lpstFile: IN/OUT=MIME data stream
//  lpszOutputFileName: IN=Filename of the file to store the attachment to
//
// Returns:
//
//  PARSER_OK if the QP section is properly decoded
//  PARSER_ERROR otherwise
//************************************************************************
int CMIMEParser::WriteQP(
	DWORD		dwCur,
	DWORD		dwNext,
	FILE		*lpstFile,
	const char	*lpszOutputFileName)
{
	FILE		*stream;
	int			rc;

	stream = dec_fopen(lpszOutputFileName,"wb");
	if (!stream)
		return (PARSER_ERROR);

	rc = DecodeQuotedPrintable(lpstFile,
							  dwCur,
							  dwNext,
							  stream);
	if (EOF == dec_fclose(stream))
	{
		rc = PARSER_ERROR;
	}

	if (rc != PARSER_OK)
	{
		// kill the file!
		dec_remove(lpszOutputFileName);
	}

	stream = NULL;
	return (rc);
}


//************************************************************************
// int Decode8bit(lpstInput,
//                dwStartOffset,
//                dwEndOffset,
//                lpstOutput)
//
// Purpose
//
//  This function decodes an 8bit stream of data starting at offset
//  dwStartOffset and ending at dwEndOffset.  It writes the output file to
//  lpstOutput.  It will strip all trailing crlf's from the end of the stream
//  as netscape (and possibly other programs) may append these...
//  This also handles 7bit content-type.
//
// Parameters:
//
//  lpstInput: IN/OUT=The MIME input stream
//  dwStartOffset: IN=Start offset of the encoded 8bit data in the MIME
//                 stream
//  dwEndOffset: IN=End offset of the encoded 8bit data in the MIME stream
//  lpstOutput: IN/OUT=The destination stream where to write our data
//
// Returns:
//
//  PARSER_OK if the 8bit section is properly decoded
//  PARSER_ERROR otherwise
//************************************************************************
int CMIMEParser::Decode8bit(
	FILE				*lpstInput,
	DWORD				dwStartOffset,
	DWORD				dwEndOffset,
	FILE				*lpstOutput)
{
	int		nChar;

	// Find the end of the binary data; ignore trailing newlines; make sure
	// to start scanning backwards from just before the --
	dwEndOffset--;			// sitting on '--'

	while (dwEndOffset >= dwStartOffset)
	{
		if (dec_fseek(lpstInput,dwEndOffset,SEEK_SET) != 0)
			return (PARSER_ERROR);

		nChar = dec_fgetc(lpstInput);
		if (nChar == EOF)
			return (PARSER_ERROR);

		if (nChar == '\x0D' || nChar == '\x0A')
			dwEndOffset--;
		else
			break;
	}

	// dwStartOffset MUST be > 0, so no wrapping worries
	// Special case when there is a zero byte message body
	if (dwStartOffset - dwEndOffset == 1)
		return PARSER_OK;

	if (dwEndOffset < dwStartOffset)
		return (PARSER_ERROR);

	dwEndOffset++;

	// now seek to the start and copy our binary stream out...
	if (dec_fseek(lpstInput, dwStartOffset, SEEK_SET) != 0)
		return (PARSER_ERROR);

	// We have calculated the size of the stream, so see if
	// it will violate policy...
	if (g_dwTextMaxExtractSize &&
		(dwEndOffset - dwStartOffset) >= g_dwTextMaxExtractSize)
	{
		return (PARSER_MAX_EXTRACT_SIZE);
	}

	// Should we abort?
	if (g_bTextAbort)
		return (PARSER_USER_ABORT);

	return (CopyBytes(lpstInput,
					 lpstOutput,
					 dwStartOffset,
					 dwEndOffset,
					 NULL,
					 NULL));
}


//************************************************************************
// int Write8bit(dwCur,
//               dwNext,
//               lpstFile,
//               lpszOutputFileName)
//
// Purpose
//
//  This function opens the specified output file and truncates its contents.
//  It then proceeds to decode a 8bit encoded section of the file that starts
//  at dwCur and ends at dwNext.  Finally, it closes the output file.
//
// Parameters:
//
//  dwCur: IN=Start of the 8bit section
//  dwNext: OUT=Start of the next section (end of the 8bit section)
//  lpstFile: IN/OUT=MIME data stream
//  lpszOutputFileName: IN=Filename of the file to store the attachment to
//
// Returns:
//
//  PARSER_OK if the 8bit section is properly decoded
//  PARSER_ERROR otherwise
//************************************************************************
int CMIMEParser::Write8bit(
	DWORD		dwCur,
	DWORD		dwNext,
	FILE		*lpstFile,
	const char	*lpszOutputFileName)
{
	FILE		*stream;
	int			rc;

	stream = dec_fopen(lpszOutputFileName,"wb");
	if (!stream)
		return (PARSER_ERROR);

	rc = Decode8bit(lpstFile,
					dwCur,
					dwNext,
					stream);
	if (EOF == dec_fclose(stream))
		rc = PARSER_ERROR;
	
	if (rc != PARSER_OK)
	{
		// kill the file!
		dec_remove(lpszOutputFileName);
	}
	
	stream = NULL;
	return (rc);
}


// Encoded-word state machine states.
#define EW_STATE_UNKNOWN		0
#define EW_STATE_SPACE			1
#define EW_STATE_START_EQUAL	2
#define EW_STATE_START_QMARK	3
#define EW_STATE_CHARSET		4
#define EW_STATE_SEPARATOR1		5
#define EW_STATE_ENCODING		6
#define EW_STATE_SEPARATOR2		7
#define EW_STATE_ENCODEDTEXT	8
#define EW_STATE_END_QMARK		9

#define MAX_ENCODED_WORD_STRING (16 * 1024)

/////////////////////////////////////////////////////////////////////////////
// DecodeEncodedWords()

bool CMIMEParser::DecodeEncodedWords(char *pszDest, DWORD dwDestSize, const char *pszSource, DWORD *pdwCharset, bool* pfFound)
{
	DWORD	dwState = EW_STATE_SPACE;  // Always start assuming SPACE was the last character.
	DWORD	dwTextLen = 0;
	char	*pStr = (char *)pszSource;
	char	*pDest = pszDest;
	char	ch;
	char	szCharset [RFC822_MAX_LINE_LENGTH+1];
	char	*pszText = NULL;
	char	*pCharset = NULL;
	char	*pText = NULL;
	char	chEncoding;

	*pfFound = false;

	// Allocate a buffer to hold an encoded word string (if any).
	pszText = (char *)malloc(MAX_ENCODED_WORD_STRING);
	if (!pszText)
		return (false);

	// Initialize destination string to a NULL-string.
	*pDest = '\0';

	// As per RFC 1522, always assume charset is US-ASCII and encoding
	// is Q type.
	*pdwCharset = DEC_CHARSET_US_ASCII;
	chEncoding = 'Q';

	// Stay in state machine until we reach the end of the source string.
	while (*pStr)
	{
		ch = *pStr;
		switch (dwState)
		{
			case EW_STATE_UNKNOWN:
				switch (ch)
				{
					case '\x20':
						dwState = EW_STATE_SPACE;
						break;

					default:
						// Copy the source character to the destination buffer.
						if (dwDestSize > 1)
						{
							*pDest++ = ch;
							dwDestSize--;
						}
						break;
				}
				break;

			case EW_STATE_SPACE:
				// Initialize encoded-word charset and text values.
				pCharset = &szCharset[0];
				pText = pszText;
				pszText[0] = szCharset[0] = 0;
				dwTextLen = 0;

				switch (ch)
				{
					case '\x3D':
						dwState = EW_STATE_START_EQUAL;
						break;

					default:
						// Copy the source character to the destination buffer.
						if (dwDestSize > 1)
						{
							*pDest++ = ch;
							dwDestSize--;
						}
						break;
				}
				break;

			case EW_STATE_START_EQUAL:
				switch (ch)
				{
					case '\x3F':
						dwState = EW_STATE_START_QMARK;
						break;

					default:
						// Previous character was an equal sign that is not
						// followed by a question mark.  Assume this is not
						// an encoded-word sequence yet.
						dwState = EW_STATE_UNKNOWN;

						// Copy an equal sign character to the destination buffer.
						if (dwDestSize > 1)
						{
							*pDest++ = '\x3D'; /* = */
							dwDestSize--;
						}

						// Copy the source character to the destination buffer.
						if (dwDestSize > 1)
						{
							*pDest++ = ch;
							dwDestSize--;
						}
						break;
				}
				break;

			case EW_STATE_START_QMARK:
			case EW_STATE_CHARSET:
				// We have started an encoded-word sequence and are in the
				// charset field.  The rest of the characters from here up
				// to the next '?' character are part of the charset value.
				switch (ch)
				{
					case '\x3F': /* ? */
						dwState = EW_STATE_SEPARATOR1;
						break;

					default:
						// This should be part of the charset value.
						// Copy the source character to the szCharset buffer.
						*pCharset++ = ch;
						*pCharset = 0;
						dwState = EW_STATE_CHARSET;
						break;
				}
				break;

			case EW_STATE_SEPARATOR1:
				switch (ch)
				{
					// Last character was the '?' separator.  This character
					// then must be the encoding type.  Return only valid
					// encoding types.
					case '\x71': /* q */
					case '\x51': /* Q */
						// Similar to Quoted-Printable encoding.
						chEncoding = 'Q';
						dwState = EW_STATE_ENCODING;
						break;

					case '\x62': /* b */
					case '\x42': /* B */
						// Base64 encoding.
						chEncoding = 'B';
						dwState = EW_STATE_ENCODING;
						break;

					case '\x3F': /* ? */
						dwState = EW_STATE_SEPARATOR2;
						break;

					default:
						// Badly formed encoded word!
						dwState = EW_STATE_UNKNOWN;
						break;
				}
				break;

			case EW_STATE_ENCODING:
				switch (ch)
				{
					// Last character was the encoding type.  This
					// character should be a '?' separator.
					case '\x3F': /* ? */
						dwState = EW_STATE_SEPARATOR2;
						break;

					default:
						// Badly formed encoded word!
						dwState = EW_STATE_UNKNOWN;
						break;
				}
				break;

			case EW_STATE_SEPARATOR2:
			case EW_STATE_ENCODEDTEXT:
				switch (ch)
				{
					// We are in the encoded text field.  Copy all characters
					// in this field to the pszText buffer until we reach a
					// '?' separator.
					case '\x3F': /* ? */
						dwState = EW_STATE_END_QMARK;
						break;

					default:
						if (dwTextLen < MAX_ENCODED_WORD_STRING)
						{
							// Copy the source character to the pszText buffer.
							*pText++ = ch;
							*pText = 0;
							dwTextLen++;
						}

						dwState = EW_STATE_ENCODEDTEXT;
						break;
				}
				break;

			case EW_STATE_END_QMARK:

				// We are now sure that we have an encoded word.
				*pfFound = true;

				switch (ch)
				{
					case '\x3D': /* = */
						dwState = EW_STATE_SPACE;
						*pdwCharset = GetCharset(szCharset);

						// Now decode the pszText buffer and output the results
						// into the pDest buffer.  Note that we must deal
						// with two different encoding methods here:
						// Base64 and Quoted-Printable.
						if (dwDestSize > 1)
						{
							ch = chEncoding;
							switch (ch)
							{
								case 'Q':
								{
									if (dwDestSize > 1)
									{
										DWORD dwCount;
										
										dwCount = DecodeQBuffer(pszText, dwTextLen, pDest, dwDestSize - 1);
										if (dwCount == 0 ||
											pszText[0] == 0)
										{
											GenerateUnknownName(pDest, dwDestSize);
											dwCount = strlen(pDest);
										}

										pDest = pDest + dwCount;
										dwDestSize -= dwCount;
									}
									break;
								}

								case 'B':
								{
									if (dwDestSize > 1)
									{
										DWORD dwCount;
										
										dwCount = DecodeBase64Buffer(pszText, dwTextLen, pDest, dwDestSize - 1);
										if (dwCount == 0 ||
											pszText[0] == 0)
										{
											GenerateUnknownName(pDest, dwDestSize);
											dwCount = strlen(pDest);
										}

										pDest = pDest + dwCount;
										dwDestSize -= dwCount;
									}
									break;
								}
							}
						}
						break;

					default:
						// Badly formed encoded word!
						dwState = EW_STATE_UNKNOWN;
						break;
				}
				break;
		}
		// Move to next character in source string.
		pStr++;
	}

	// Append a NULL terminator.
	*pDest = '\0';
	free(pszText);
	return (true);
}


/////////////////////////////////////////////////////////////////////////////
// GenerateUnknownName()
size_t CMIMEParser::GenerateUnknownName(char *pBuffer, size_t nBufferSize)
{
	size_t	idx;
	char	hex_buf[20];
	int		hex_idx;
	int		hex_len;
	DWORD	dwTemp;
	int		this_digit;

	// We don't expect this function to fail.
	dec_assert(nBufferSize >= 16);

	if (nBufferSize < 16)
		return (0);

	// Build a name string that is (in ASCII) the word "Unknown" followed
	// by the 8-digit hex value of the section offset with a .data extension.
	strcpy(pBuffer, "\x55\x6E\x6B\x6E\x6F\x77\x6E"); // "Unknown"
	idx = 7; // strlen("Unknown");

	// Build the hex part in a separate buffer because we have to build it
	// from right to left.
	hex_idx = sizeof(hex_buf);	// Pre-decremented, so start one past the end of the buffer.
	hex_len = 0;

	// We assume only one item per boundary can be extracted. In order
	// to work like the old MIME engine, name it based on the offset of 
	// the section rather than the actual data being extracted. QA
	// requested this to make validation easier. Also, if m_nBoundaryStart
	// is zero then this must be a single-part MIME message, since we
	// are going to extract an item that is not in a multipart MIME section.
	// In that case, QA would like to see names that have a zero offset, since
	// that's how the old MIME engine did it.
	dwTemp = m_nBoundaryStart;

	do 
	{
		this_digit = dwTemp & 0x0F;
		dwTemp = (dwTemp >> 4);
		--hex_idx;

		if (this_digit > 9) 
		{
			hex_buf[hex_idx] = (this_digit - 10) + 0x41; /* A */
		}
		else 
		{
			hex_buf[hex_idx] = this_digit + 0x30; /* 0 */
		}

		++hex_len;

	} while (dwTemp != 0);

	// Pad the hex string to the left with zeros to 8 digits..
	while (hex_len < 8)
	{
		--hex_idx;
		hex_buf[hex_idx] = '\x30'; // "0"
		++hex_len;
	}

	// Append the hex string to the name.
	while (hex_len > 0) 
	{
		pBuffer[idx] = hex_buf[hex_idx];
		--hex_len;
		++idx;
		++hex_idx;
	}

	// Append the extension.
	pBuffer[idx] = '\x2E'; // "."
	++idx;
	pBuffer[idx] = '\x64'; // "d"
	++idx;
	pBuffer[idx] = '\x61'; // "a"
	++idx;
	pBuffer[idx] = '\x74'; // "t"
	++idx;
	pBuffer[idx] = '\x61'; // "a"
	++idx;
	pBuffer[idx] = 0;

	return(idx);
}


/////////////////////////////////////////////////////////////////////////////
// GetCharset()

DWORD CMIMEParser::GetCharset(char *pszCharset)
{
	DWORD	dwIndex;

	// Determine which character set applies.
	for (dwIndex = 0; dwIndex < NUM_CHARSETS; dwIndex++)
	{
		if (binary_stricmp(pszCharset, szTextMIMECharsetUpper[dwIndex], szTextMIMECharsetLower[dwIndex]) == 0)
			return (dwIndex);
	}

	return (DEC_CHARSET_US_ASCII);
}


/////////////////////////////////////////////////////////////////////////////
// MIMEMakeEncodedWord()

#if 0
void MIMEMakeEncodedWord(char *pszDest, DWORD dwDestSize, char *pszSource, DWORD dwCharset, char chEncoding)
{
	// As per RFC 1522, an encoded word is formatted as follows:
	// "=?" charset "?" encoding "?" encoded-text "?="
	strcpy (pszDest, "\x3D\x3F"); /* =? */
	strcat(pszDest, szTextMIMECharsetUpper[dwCharset]);

	// Append a separator.
	strcat (pszDest, "\x3F"); /* ? */

	// Now append the encoding type (and a separator).
	// Also append the encoded text.
	switch (chEncoding)
	{
		case 'b':
		case 'B':
			strcat (pszDest, "\x42\x3F"); /* B? */

			// Append the encoded text.
			MIMEEncodeBase64Buffer (pszSource,
									strlen (pszSource),
									pszDest,
									dwDestSize);
			break;

		case 'q':
		case 'Q':
		default:
			strcat (pszDest, "\x51\x3F"); /* Q? */

			// Append the encoded text
			MIMEEncodeQBuffer (pszSource,
							  strlen (pszSource),
							  pszDest,
							  dwDestSize);
			break;
	}

	// Append the encoded-word terminator
	strcat (pszDest, "\x3F\x3D"); /* ?= */
}

#endif /* 0 */

/////////////////////////////////////////////////////////////////////////////
// MIMEDecodeQBuffer()

DWORD CMIMEParser::DecodeQBuffer(char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize)
{
	char	ch;
	BYTE	byHex = 0;
	int		iHexDigit = 0;
	DWORD	dwOutputCount = 0;

	while ( dwSize > 0 &&
			dwOutputBufferSize > (dwOutputCount + 1) )
	{
		ch = *pszInput++;

		// See if previous character was an equal sign.
		// If so, translate the next two characters from
		// hexadecimal to decimal.
		if(iHexDigit > 0) 
		{
			switch(ch) 
			{
				case '\x30': /* digit 0 */
				case '\x31': /* digit 1 */
				case '\x32': /* digit 2 */
				case '\x33': /* digit 3 */
				case '\x34': /* digit 4 */
				case '\x35': /* digit 5 */
				case '\x36': /* digit 6 */
				case '\x37': /* digit 7 */
				case '\x38': /* digit 8 */
				case '\x39': /* digit 9 */
					if(iHexDigit == 2)
					{
						byHex += (16 * (ch - '\x30')); /* digit 0 */
					}

					if(iHexDigit == 1)
					{
						byHex += (ch - '\x30'); /* digit 0 */
					}
					break;

				case '\x41': /* A */
				case '\x42': /* B */
				case '\x43': /* C */
				case '\x44': /* D */
				case '\x45': /* E */
				case '\x46': /* F */
					if(iHexDigit == 2)
					{
						byHex += (16 * (ch - '\x41' + 10)); /* letter A */
					}

					if(iHexDigit == 1) 
					{
						byHex += (ch - '\x41' + 10); /* letter A */
					}
					break;

				case '\x61': /* a */
				case '\x62': /* b */
				case '\x63': /* c */
				case '\x64': /* d */
				case '\x65': /* e */
				case '\x66': /* f */
					if(iHexDigit == 2) 
					{
						byHex += (16 * (ch - '\x61' + 10)); /* letter a */
					}

					if(iHexDigit == 1) 
					{
						byHex += (ch - '\x61' + 10); /* letter a */
					}
					break;

				default:
					// Bad format for hexadecimal value!
					// We'll just assume this character is corrupt and use '0' by default.
					break;
			}

			iHexDigit--;
			if (iHexDigit == 0)
			{
				// Got 2nd hex digit just now so write out
				// the character.
				*pszOutput++ = (char)byHex;
				dwOutputCount++;
				byHex = 0;
			}
		}
		else if(ch == '\x3D')  /* = */
		{	
			// Get the next two hexadecimal characters.
			iHexDigit = 2;
		}
		else
		{
			// Translate underscore into space
			if( ch == 0x5F )
			{
				ch = 0x20;
			}

			*pszOutput++ = ch;
			dwOutputCount++;
		}

		dwSize--;
	}

	// Append a NULL-terminator.
	*pszOutput++ = '\0';

	return (dwOutputCount);
}


/////////////////////////////////////////////////////////////////////////////
// MIMEEncodeQBuffer()

DWORD CMIMEParser::EncodeQBuffer(char *pszInput, DWORD dwSize, char *pszOutput, DWORD dwOutputBufferSize)
{
	char	ch;
	DWORD	dwOutputCount = 0;

	while ( dwSize > 0 &&
			dwOutputBufferSize > (dwOutputCount + 1) )
	{
		ch = *pszInput++;

		if((ch >= '\x41' && ch <= '\x5A') || /* A - Z */
		  (ch >= '\x61' && ch <= '\x7A') || /* a - z */
		  (ch >= '\x30' && ch <= '\x39') || /* 0 - 9 */
		  (ch == '\x21') || /* ! */
		  (ch == '\x2A') || /* * */
		  (ch == '\x2B') || /* + */
		  (ch == '\x2D') || /* - */
		  (ch == '\x2F') || /* / */
		  (ch == '\x5F')    /* _ */ ) 
		{
			// Source character fits into common US-ASCII character set
			// so just output the raw character.
			*pszOutput++ = ch;
			dwOutputCount++;
		}
		else
		{
			// Source character can not be output in its raw form.
			// Must convert it to hex.
			*pszOutput++ = '\x3D'; /* = */
			sssnprintf (pszOutput, dwOutputBufferSize, "%02X", (unsigned int)(unsigned char)ch);
			pszOutput += 2;
			dwOutputCount += 3;
		}

		dwSize--;
	}

	// Append a NULL-terminator.
	*pszOutput++ = '\0';

	return (dwOutputCount);
}


//************************************************************************
// int CopyBytes(lpstInputFile,
//               lpstOutputFile,
//               dwStart,
//               dwEnd,
//               lpszRenameName,
//               lpszContentType)
//
// Purpose
//
//  This function copies the specified range of bytes from the input file to
//  the current write pointer in the output file.
//
// Parameters:
//
//  lpstInputFile: IN/OUT=Input stream
//  lpstOutputFile: IN/OUT=Output stream
//  dwStart: IN=Starting offset in input stream where to find data
//  dwEnd: IN=Ending offset in input stream
//  lpszRenameName: IN=If a non-NULL string, the name of the item
//                  is changed.
//  lpszContentType: IN=If a non-NULL string, the Content-Type of the item
//                   is changed.
//
// Returns:
//
//  PARSER_OK if the bytes were successfully copied
//  PARSER_ERROR otherwise
//************************************************************************
int CMIMEParser::CopyBytes(
	FILE	*lpstInputFile,
	FILE	*lpstOutputFile,
	DWORD	dwStart,
	DWORD	dwEnd,
	char	*lpszRenameName,
	char	*lpszContentType)
{
	DWORD	dwNumBytes, dwActual, dwActual2;
	BYTE	*byBuffer;
	bool	bFirst;
	int		nReturn = PARSER_ERROR;

	byBuffer = (BYTE *)malloc(RFC822_MAX_LINE_LENGTH + 1);
	if (!byBuffer)
		return nReturn;

	if (dwEnd < dwStart)
		goto bailOut;

	dwNumBytes = dwEnd - dwStart;
	if (dec_fseek(lpstInputFile, dwStart, SEEK_SET) != 0)
		goto bailOut;

	bFirst = true;
	while (dwNumBytes)
	{
		dwActual = dwNumBytes > RFC822_MAX_LINE_LENGTH ? RFC822_MAX_LINE_LENGTH : dwNumBytes;
		dwActual2 = dec_fread(byBuffer,1,(int)dwActual,lpstInputFile);
		if (dwActual != dwActual2)
			goto bailOut;

		// Make sure the buffer is NULL-terminated.
		byBuffer[dwActual] = '\0';

		// See if there is a new name to be applied to this entry.
		if (bFirst &&
			( (lpszRenameName && lpszRenameName[0] != '\0') ||
			 (lpszContentType && lpszContentType[0] != '\0') ))
		{
			// Only process the first buffer in a given section.
			// If the name is not in there then assume that it
			// is not present.  The risk is that the 2nd or later
			// buffers could contain lines that appear to be
			// MIME header lines, but are actually not.  We do not
			// want to be modifying that data on accident.
			bFirst = false;

			// Yes, there is.  Write the line, modifying the "name="
			// and "filename=" fields as needed.
			if (!WriteRenamedContentLine(byBuffer,
										dwActual,
										lpszRenameName,
										lpszContentType,
										lpstOutputFile))
			{
				goto bailOut;
			}
		}
		else
		{
			dwActual2 = dec_fwrite(byBuffer,1,(int)dwActual,lpstOutputFile);
			if (dwActual2 != dwActual)
				goto bailOut;
		}

		dwNumBytes -= dwActual;
	}

	nReturn = PARSER_OK;

bailOut:
	if (byBuffer)
		free(byBuffer);

	return nReturn;
}


bool CMIMEParser::WriteRenamedContentLine(
	BYTE  *pBuffer,
	DWORD dwBufSize,
	char  *lpszRenameName,
	char  *lpszContentType,
	FILE  *lpstOutputFile)
{
/*
	BYTE	*pNewBuffer = NULL;
	char	*ptrSrc;
	char	*ptrDest;
	DWORD	dwNewBufSize;
	DWORD	dwNewBytes;
	int		iBytesWritten;
	int		iType;
	int		iStartIndex;
	bool	bFound;
	size_t	iLen;

	char *szTemp = new char[RFC822_MAX_LINE_LENGTH+1];
	char *szFieldType = new char[RFC822_MAX_LINE_LENGTH+1];
	char *szFieldSubType = new char[RFC822_MAX_LINE_LENGTH+1];

	if(!szTemp || !szFieldType || !szFieldSubType)
	{
		goto write_failed;
	}

	if (!pBuffer || !dwBufSize || !lpstOutputFile ||
		dwBufSize > (0x0000ffff - MAX_PATH - 16))
	{
		// Invalid input parameter(s).
		goto write_failed;
	}

	// Make sure at least one of the two input strings is pointing to
	// something.  If not then there is nothing for us to do here
	// except write the line as-is.
	if (!lpszRenameName && !lpszContentType)
		goto write_as_is;

	// See if this line is either "Content-Type" or "Content-Disposition".
	MIMEIdentifyLine((char *)pBuffer, dwBufSize, &iType);
	if (iType != MIME_SECTION_HEADER_TYPE &&
		iType != MIME_SECTION_HEADER_DISPOSITION)
		goto write_as_is;

	// Compute the maximum size buffer we potentially need to do the job.
	// We need room for the original input line plus the maximum size of
	// the rename string plus the maximum size of the content-type string
	// plus a little bit of slack for adding quotation marks and the like.
	dwNewBufSize = dwBufSize + MAX_PATH + 64 + 16;
	pNewBuffer = new BYTE[dwNewBufSize];
	if (!pNewBuffer)
		goto write_failed;	// Failed to allocate memory

	// OK, the line is either a "Content-Type" or "Content-Disposition"
	// line.  Now, look for the corresponding "name=" or "filename=" parameter.
	if (iType == MIME_SECTION_HEADER_TYPE)
	{
		int		iTypeStart, iTypeLength, iSourceIndex;

		// This is a Content-Type line.  Get the type and sub-type.
		// This call also retrieves the position and length of the
		// type/sub-type field.
		if (MIMEGetFieldTypeValue((char *)pBuffer,
								 szFieldType,
								 szFieldSubType,
								 &iTypeStart,
								 &iTypeLength) == MIMESTATUS_ERROR)
			goto write_as_is;

		// It looks like a "Content-Type" line.
		// See if there is a "name=" parameter on the line.
		if (MIMEGetFieldParameter((char *)pBuffer,
								 STR_NAME,
								 STR_name,
								 szTemp,
								 &bFound,
								 &iStartIndex,
								 false) == MIMESTATUS_ERROR)
			goto write_as_is;

		// OK, we need to construct a new Content-Type line.
		ptrSrc	= (char *)pBuffer;
		ptrDest = (char *)pNewBuffer;
		iSourceIndex = 0;

		// NOTE: At this point we know that at least one of
		//       the input strings is non-NULL.
		if (lpszContentType)
		{
			// We have a new type/sub-type field so start by
			// copying from the source only up to where the
			// type/sub-type field starts.
			dwNewBytes = (DWORD)iTypeStart;
		}
		else
		{
			// We are not changing the type/sub-type field.
			if (!bFound)
			{
				// We did not locate a name= parameter on the line
				// so there is nothing left to do except write the
				// line as-is.
				goto write_as_is;
			}

			// We did locate a name= parameter so start by
			// copying from the source up to where the name=
			// parameter starts.
			dwNewBytes = (DWORD)iStartIndex;
		}

		// Copy the beginning of the original Content-Type line.
		memcpy (ptrDest, ptrSrc, dwNewBytes);
		ptrDest += dwNewBytes;
		ptrSrc += dwNewBytes;
		iSourceIndex += dwNewBytes;

		if (lpszContentType)
		{
			// We have a new type/sub-type field so copy it into the
			// destination buffer now.
			iLen = strlen(lpszContentType);
			memcpy(ptrDest, lpszContentType, iLen);
			ptrDest += iLen;
			dwNewBytes += iLen;

			// Skip over the original type/sub-type.
			ptrSrc += iTypeLength;
			iSourceIndex += iTypeLength;

			if (bFound)
			{
				// We found a name= parameter.
				if (iStartIndex >= iSourceIndex)
				{
					// There is some data between the type/sub-type field
					// and the name= parameter so copy it.
					iLen = iStartIndex - iSourceIndex;
					memcpy(ptrDest, ptrSrc, iLen);
					ptrSrc += iLen;
					ptrDest += iLen;
					dwNewBytes += iLen;
				}
				else
				{
					// This should never happen.
					goto write_as_is;
				}
			}
			else
			{
				// We did not find a name= parameter so all that's left
				// to do is to copy the remainder of the original line.
				iLen = strlen(ptrSrc);
				if (iLen)
				{
					memcpy(ptrDest, ptrSrc, iLen);
					dwNewBytes += iLen;
				}
				else
				{
					// There is nothing left after the type/sub-type field, so be
					// sure to append a CRLF to the end of the Content-Type.
					strcpy (ptrDest, "\x0D\x0A");
					dwNewBytes += 2;
				}

				goto write_new_buffer;
			}
		}

		// If we get here, ptrSrc should be pointing to the beginning of
		// the name= parameter and bFound should be true (if not, then the
		// logic above is broken).
		if (!bFound)
			goto write_as_is;

		if (!lpszRenameName)
		{
			// No new name is being requested, so all that is left
			// to do is to copy the remainder of the original line.
			iLen = strlen(ptrSrc);
			if (iLen)
			{
				memcpy(ptrDest, ptrSrc, iLen);
				dwNewBytes += iLen;
			}
			else
			{
				// There is nothing left after the type/sub-type field, so be
				// sure to append a CRLF to the end of the Content-Type.
				strcpy (ptrDest, "\x0D\x0A");
				dwNewBytes += 2;
			}

			goto write_new_buffer;
		}

		// A new name is being requested, so copy it into the destination buffer.
		// First, skip over the name= parameter in the source buffer.
		if (*ptrSrc == '\x22') // Quote
			ptrSrc++;
		ptrSrc += strlen(szTemp);
		if (*ptrSrc == '\x22') // Quote
			ptrSrc++;

		// Get the length of the new name.
		iLen = strlen(lpszRenameName);

		// Always enclose the new name in quotation marks.
		*ptrDest++ = '\x22'; // Quote
		dwNewBytes++;

		// Copy in the new name.
		memcpy(ptrDest, lpszRenameName, iLen);
		ptrDest += iLen;
		dwNewBytes += iLen;
		
		// Close the quotation marks around the new name.
		*ptrDest++ = '\x22';
		dwNewBytes++;

		iLen = strlen(ptrSrc);
		if (iLen)
		{
			// There is something that follows the name= field.
			// We have to assume that the line is already correctly
			// formatted, so just copy it over into the output buffer.
			strcpy (ptrDest, ptrSrc);
			dwNewBytes += iLen;
		}
		else
		{
			// There is nothing left after the name= field, so be
			// sure to append a CRLF to the end of the Content-Type.
			strcpy (ptrDest, "\x0D\x0A");
			dwNewBytes += 2;
		}
	}
	else
	{
		// It looks like a "Content-Disposition" line.
		// See if there is a "filename=" parameter on the line.
		if (MIMEGetFieldParameter((char *)pBuffer,
								 STR_FILENAME,
								 STR_filename,
								 szTemp,
								 &bFound,
								 &iStartIndex,
								 false) == MIMESTATUS_ERROR)
			goto write_as_is;

		if (!bFound)
		{
			if (MIMEGetFieldParameter((LPSTR)pBuffer,
									 STR_FILENAME0,
									 STR_filename0,
									 szTemp,
									 &bFound,
									 &iStartIndex,
									 false) == MIMESTATUS_ERROR)
				goto write_as_is;

			if (!bFound)
				goto write_as_is;
		}

		// Yes, we found the filename parameter.  Now, construct
		// the new output buffer using the input name instead.
		ptrSrc	= (char *)pBuffer;
		ptrDest = (char *)pNewBuffer;
		dwNewBytes = (DWORD)iStartIndex;
		memcpy (ptrDest, ptrSrc, dwNewBytes);
		ptrSrc += dwNewBytes;
		if (*ptrSrc == '\x22') // Quote
			ptrSrc++;
		ptrSrc += strlen(szTemp);
		if (*ptrSrc == '\x22') // Quote
			ptrSrc++;
		ptrDest += dwNewBytes;
		*ptrDest++ = '\x22'; // Quote
		dwNewBytes++;
		strcpy (ptrDest, lpszRenameName);
		ptrDest += strlen(lpszRenameName);
		dwNewBytes += strlen(lpszRenameName);
		*ptrDest++ = '\x22'; // Quote
		dwNewBytes++;

		iLen = strlen(ptrSrc);
		if (iLen)
		{
			// There is something that follows the filename= field.
			// We have to assume that the line is already correctly
			// formatted, so just copy it over into the output buffer.
			strcpy (ptrDest, ptrSrc);
			dwNewBytes += iLen;
		}
		else
		{
			// There is nothing left after the filename= field, so be
			// sure to append a CRLF to the end of the Content-Disposition.
			strcpy (ptrDest, "\x0D\x0A");
			dwNewBytes += 2;
		}
	}

write_new_buffer:
	iBytesWritten = dec_fwrite(pNewBuffer,1,(int)dwNewBytes,lpstOutputFile);
	if (iBytesWritten != (int)dwNewBytes)
		goto write_failed;

	goto write_ok;

write_as_is:
	// Write out the line as-is.
	iBytesWritten = dec_fwrite(pBuffer,1,(int)dwBufSize,lpstOutputFile);
	if (iBytesWritten != (int)dwBufSize)
		goto write_failed;

write_ok:
	if(szTemp)
	{
		delete [] szTemp;
		szTemp = NULL;
	}

	if(szFieldType)
	{
		delete [] szFieldType;	 
		szFieldType = NULL;
	}
	
	if(szFieldSubType)
	{
		delete [] szFieldSubType;
		szFieldSubType = NULL;
	}

	if(pNewBuffer)
	{  
		delete [] pNewBuffer;
		pNewBuffer = NULL;
	}

	return true;

write_failed:
	if(szTemp)
	{
		delete [] szTemp;
		szTemp = NULL;
	}

	if(szFieldType)
	{
		delete [] szFieldType;	 
		szFieldType = NULL;
	}
	
	if(szFieldSubType)
	{
		delete [] szFieldSubType;
		szFieldSubType = NULL;
	}

	if(pNewBuffer)
	{
		delete [] pNewBuffer;
		pNewBuffer = NULL;
	}

	return false;
*/

	int		iBytesWritten;

	// Write out the line as-is.
	iBytesWritten = dec_fwrite(pBuffer, 1, (int)dwBufSize, lpstOutputFile);
	if (iBytesWritten != (int)dwBufSize)
		return false;
	return true;
}

