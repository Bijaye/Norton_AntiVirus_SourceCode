/* $Header:   V:/INCLUDE/VCS/FILE.H_V   1.15   23 Nov 1992 20:18:12   BASIL  $ */

/*----------------------------------------------------------------------*
 * File constants and structures				 FILE.H *
 *									*
 * Copyright 1991 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   V:/INCLUDE/VCS/FILE.H_V  $
 * 
 *    Rev 1.15   23 Nov 1992 20:18:12   BASIL
 * Added COPY_DISK_IS_FULL returned by FileCopy() and FileAppend()
 * 
 * 
 *    Rev 1.14   18 Aug 1992 12:29:32   MARKK
 * Added MAX_NET_PATH_NAME
 * 
 *    Rev 1.13   14 Feb 1992 10:28:44   PETERD
 * Added parms to function prototypes.
 * 
 *    Rev 1.12   05 Feb 1992 17:52:40   DAVIDH
 * Added TEMP_CLASS_SPECIFIED for FileCreateTemp()
 * 
 *    Rev 1.11   04 Feb 1992 09:54:20   ED
 * Added the AttrIs... macros
 * 
 *    Rev 1.10   29 Jan 1992 15:19:40   ED
 * Added STAR_DOT_STAR
 * 
 *    Rev 1.9   17 Jan 1992 18:33:10   PETERD
 * Added prototype for FileReturnTempDir().
 * 
 *    Rev 1.8   26 Dec 1991 13:37:38   ED
 * Added the FileInitPath, FileInitDrive, NameIsDot, NameIsDoubleDot, 
 * NameHasDriveSpec, and NameIsFull macros.
 * 
 *    Rev 1.7   18 Nov 1991 12:22:32   PETERD
 * Added FileCreateTemp() and definitions for it.
 * 
 *    Rev 1.6   31 Oct 1991 12:52:56   JOHN
 * Added constants for NameInfo() routine, and otherwise merged changes
 * made during NU 6.01.01 into the Passport version of FILE.H.
 * 
 *    Rev 1.5   11 Oct 1991 17:18:32   TORSTEN
 * Updated the definitions of READ_ONLY_FILE, WRITE_ONLY_FILE, and
 * READ_WRITE_FILE to allow file sharing on networks and under Windows.
 * 
 *    Rev 1.4   21 Aug 1991 18:48:02   PETERD
 * Added MAX_FILE_NAME definition
 * 
 *    Rev 1.3   25 Jun 1991 08:21:58   MARKK
 * Added LPFFRECORD and FA_NORMAL.
 * 
 *    Rev 1.2   08 Mar 1991 07:35:08   ED
 * Added constants for the FileCopy() function.
 * 
 *    Rev 1.1   04 Mar 1991 14:03:00   PETERD
 * Added FileInfoRec definition.
 * 
 *    Rev 1.0   29 Aug 1990 11:23:48   DAVID
 * Initial revision.
 */

#ifndef INCL_FILE
#define INCL_FILE

#ifndef INCL_TYPES
#include <types.h>
#endif

/*----------------------------------------------------------------------*
 * File attributes							*
 *----------------------------------------------------------------------*/

#define FA_NORMAL	((Byte)	  0)	/* Normal */
#define FA_READ		((Byte)   1) 	/* read only */
#define FA_READ_ONLY 	FA_READ
#define FA_HDDN    	((Byte)   2) 	/* hidden    */
#define FA_HIDDEN    	FA_HDDN
#define FA_SYST    	((Byte)   4) 	/* system    */
#define FA_SYSTEM  	FA_SYST
#define FA_HS      	((Byte)   6) 	/* hidden and system    */
#define FA_VOLM    	((Byte)   8) 	/* volume    */
#define FA_VOLUME  	FA_VOLM
#define FA_DIR     	((Byte)  16) 	/* directory */
#define FA_ARCH    	((Byte)  32) 	/* archive   */
#define FA_ARCHIVE    	FA_ARCH
#define FA_NONREAD 	((Byte)0xFE) 	/* turn off read (in WF) */
#define FA_NONSHR  	((Byte)0xF8) 	/* turn off system/hidden/readonly in WF */
#define FA_VD      	(FA_DIR | FA_VOLUME)

#define AttrIsDir(a)            ((a) & FA_DIR)
#define AttrIsVolume(a)         ((a) & FA_VOLM)
#define AttrIsArchive(a)        ((a) & FA_ARCH)
#define AttrIsHidden(a)         ((a) & FA_HDDN)
#define AttrIsSystem(a)         ((a) & FA_SYST)
#define AttrIsReadOnly(a)       ((a) & FA_READ)

/*----------------------------------------------------------------------*
 * Different modes files can be opened in				*
 *----------------------------------------------------------------------*/

/*
 * DOS 3.0+ interprets bits 4 through 6 as the sharing mode; we set them so
 * that other processes can't write to the file we are reading from.  Older
 * versions of DOS (below 3.0) only interpret bits 0 through 2.
 */

#define	READ_ONLY_FILE	0x20
#define	WRITE_ONLY_FILE	0x21
#define READ_WRITE_FILE	0x22

/*----------------------------------------------------------------------*
 * Maximum sizes of things						*
 *----------------------------------------------------------------------*/

#define	MAX_FILE_NAME		13	// Max file name length
#define	MAX_PATH_NAME		82	// Max drive name, path, and file
#define	MAX_NET_PATH_NAME	(128+13)// Max network name

/*----------------------------------------------------------------------*
 * Seek methods used by the FileSeek(...) routine			*
 *----------------------------------------------------------------------*/

#define SEEK_FROM_BEGINNING	0
#define SEEK_FROM_CURRENT_LOC	1
#define SEEK_TO_END		2

/*----------------------------------------------------------------------*
 * DOS file error codes							*
 *----------------------------------------------------------------------*/

#define FILE_NOT_FOUND		2
#define PATH_NOT_FOUND		3
#define ACCESS_DENIED		5

/*----------------------------------------------------------------------*
 * Constants for FileCopy()						*
 *----------------------------------------------------------------------*/

#define	COPY_DISK_IS_FULL 	(-4)
#define	COPY_CRITICAL_ERROR	(-3)
#define	COPY_MEMORY_ERROR	(-2)
#define	COPY_BREAK_PRESSED	(-1)
#define	COPY_SUCCESSFUL		(0)

/*----------------------------------------------------------------------*
 * Constants for NameInfo()						*
 *----------------------------------------------------------------------*/

#define NAME_IS_INVALID		0
#define NAME_IS_DRIVE		1
#define NAME_IS_DIR		2
#define NAME_IS_FILE		3
#define NAME_IS_DEVICE		4
#define NAME_IS_WILDCARD	5
#define NAME_HAS_BAD_DRIVE	6
#define NAME_HAS_BAD_FILE	7

#define USE_DIR_RULES		0x0001
#define ALLOW_ERROR_DIALOG	0x0002

/*----------------------------------------------------------------------*
 * Misc constants							*
 *----------------------------------------------------------------------*/

#define	GET_DATE_AND_TIME	0
#define SET_DATE_AND_TIME	1

/*----------------------------------------------------------------------*
 * Basic specs for file and path functions.  These are DWORD 
 * representations of strings that are used to form drive and path
 * specs.  Instead of doing this:
 *                      StringCopy (path, "x:\\");
 *                      StringCopy (drive, "x:");
 *         do this:
 *                      FileInitPath (path);
 *                      FileInitDrive (drive);
 *----------------------------------------------------------------------*/

#define STAR_DOT_STAR           0x002A2E2AL     // equal to "*.*"

#define BASIC_PATH              0x005C3A78L     // "x:\\"
#define BASIC_DRIVE             0x3A78          // "x:"

#define FileInitPath(string)    (*(Ulong *)(string) = BASIC_PATH)
#define FileInitDrive(string)   { *(Word *)(string) = BASIC_DRIVE;(string)[2]='\0'; }

#define NameIsDot(filename)        (*(Word *)(filename)==0x002E)
#define NameIsDoubleDot(filename)  ((*(Word *)(filename)==0x2E2E)&&((filename)[2]=='\0'))

#define NameHasDriveSpec(string)   ((string)[0]!='\0' && (string)[1]==':')

#define NameIsFull(path)           (*(path)=='\\' || ((path)[1]==':' && (path)[2]=='\\'))

/*----------------------------------------------------------------------*
 * Find first / Find next structure					*
 *----------------------------------------------------------------------*/

struct dos2find 			/* find-first/next file */
    {
    Byte 	dos_work[21];
    Byte 	fileattr;
    Word  	filetime;
    Word  	filedate;
    Ulong	filesize;
    Byte 	filename[13];
    };

typedef struct dos2find FileInfoRec;

/*----------------------------------------------------------------------*
 * FindFirstRec								*
 *									*
 * Record used when searching for a file using DOS's FindFirst routines *
 *----------------------------------------------------------------------*/

typedef struct
    {
    					/* This top portion must be	*/
					/* exactly 21 bytes long	*/
    Byte	pattern[11];
    Byte	searchAttribute;
    struct dir_struct huge *directoryPtr;
    Word	directoryCount;
    Word	index;
    Byte	filler;

    Byte 	attribute;
    Word  	time;
    Word  	date;
    Ulong      	size;
    Byte 	name[13];

    Boolean	readDirectly;		/* Can we read directly from disk? */
    } FindFirstRecord;

typedef FindFirstRecord FindFirstRec;
typedef FindFirstRecord FAR *LPFFRECORD;

//-----------------------------------------------------------------------
// Temporary file classes.  Used by FileCreateTemp() to create temp files
// in the appropriate directory
//-----------------------------------------------------------------------

#define	TEMP_CLASS_SWAP		0	// VM Swap file
#define	TEMP_CLASS_SAVE		1	// VM Save file
#define	TEMP_CLASS_OVERLAY	2	// Code overlay swap area
#define	TEMP_CLASS_TEMP		3	// Other temp files
#define TEMP_CLASS_SPECIFIED    4       // Use path specified in input name

Word PASCAL FileCreateTemp(Byte byClass, Byte *pszRoot, Byte *pszVar, Ulong dwSize, Byte *pszFile);
void PASCAL FileReturnTempDir(Byte byClass, Byte *pszVar, Byte *pszDirName);

#endif
