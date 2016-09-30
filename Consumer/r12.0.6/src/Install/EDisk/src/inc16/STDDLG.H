/* $Header:   S:/INCLUDE/VCS/STDDLG.H_V   1.19   28 Apr 1992 11:48:38   MARKK  $ */

/*
 * $Log:   S:/INCLUDE/VCS/STDDLG.H_V  $
 * 
 *    Rev 1.19   28 Apr 1992 11:48:38   MARKK
 * Added new browse flags
 * 
 *    Rev 1.18   20 Apr 1992 18:50:22   ED
 * Added a bunch of new Browse flags
 * 
 *    Rev 1.17   26 Mar 1992 21:19:16   TORSTEN
 * Updated MAX_NETWORK_USERS.
 * 
 *    Rev 1.16   26 Mar 1992 09:26:54   ED
 * Added STDDLG_ALLOW_NON_EXISTANT
 * 
 *    Rev 1.15   14 Feb 1992 10:27:26   PETERD
 * Added flags for StdDlgConfigVideoAndMouse().  Someone added StdDlgSaveChanges.
 * 
 *    Rev 1.14   12 Jan 1992 03:35:02   TORSTEN
 * Moved the SelectDriveInfo typedef to LINT.H to accomodate some prototypes.
 * 
 *    Rev 1.13   04 Dec 1991 19:28:40   PETERD
 * Removed DRIVE_FIELD_ entries.
 * 
 *    Rev 1.12   20 Nov 1991 15:14:42   PETERD
 * Removed uneeded items
 * 
 *    Rev 1.11   14 Nov 1991 10:11:06   ED
 * Modified SelectDriveInfo so we can pass in the selected drives
 * 
 *    Rev 1.10   08 Nov 1991 10:41:04   PETERD
 * Removed constants referenced only in one module.
 * 
 *    Rev 1.9   05 Nov 1991 15:41:46   PETERD
 * Added disabling flags to the SelectDriveRec structure.  These allow
 * specific types drives to be excluded to the list.
 * 
 *    Rev 1.8   29 Oct 1991 16:27:46   ED
 * Added FILE_OVERWRITE and FILE_APPEND
 * 
 *    Rev 1.7   22 Oct 1991 18:16:06   PETERD
 * Added StdDlgChangePassword() and related items.
 * 
 *    Rev 1.6   16 Oct 1991 09:41:16   ED
 * Modified the MATCHFUNC prototype
 * 
 *    Rev 1.5   01 Oct 1991 19:38:54   DOUG
 * added some stuff for multiple local drive list selection
 * 
 *    Rev 1.4   20 Sep 1991 16:09:30   DOUG
 * Added som stuff for StdDlgSelectDriveList
 * 
 *    Rev 1.3   20 Sep 1991 15:06:02   ED
 * Removed some definitions added that were too generic, so they
 * screwed up many other programs
 * 
 *    Rev 1.2   04 Sep 1991 13:10:42   ED
 * Added the SelectDriveInfo data structure
 * 
 *    Rev 1.1   20 Feb 1991 10:42:46   PETERD
 * Added StdDlgTaskSwitch().
 * 
 *    Rev 1.0   29 Aug 1990 11:24:18   DAVID
 * Initial revision.
 */


/*            Copyright 1989 by Peter Norton Computing, Inc.	 	*/

/*----------------------------------------------------------------------*
 * 				STDDLG.H 				*
 *									*
 * This file contains stuff for the standard dialog boxes.		*
 *									*
 * 09/06/89  David: created file					*
 * 11/07/89  David: moved to library					*
 *----------------------------------------------------------------------*/

#ifndef INCL_STD_DIALOG
#define INCL_STD_DIALOG

#ifndef	INCL_DIALOG
#include <dialog.h>
#endif


/************************************************************************
 * StdDlgSaveChanges
 ************************************************************************/

#define SC_YES          0
#define SC_NO           1
#define SC_CANCEL       2

/*----------------------------------------------------------------------*
 * This is the stuff for invoking StdDlgSelectFile dialog.		*
 *----------------------------------------------------------------------*/
#define BYTES_PER_FILE 11

#define STDDLG_NO_PRESERVE              1	/* don't preserve default drive/dir */
#define STDDLG_FIND_HID_SYS             2	/* find hidden/system files too */
#define STDDLG_ALLOW_SPEC               4	/* Allow selection of file spec */
#define STDDLG_ALLOW_NON_EXISTANT       8       /* file spec doesn't have to exist */
#define STDDLG_DONT_RESET               16      /* don't reset filename TE              */
#define STDDLG_DONT_RESET_FOCUS		32	/* Don't move button focus */

        // ---------------------------------------------------------------
        // Some combinations to make things a little easier (yeah, right!)
        // ---------------------------------------------------------------

#define STDDLG_SAVE_AS          (STDDLG_ALLOW_NON_EXISTANT | STDDLG_DONT_RESET)
#define STDDLG_OPEN_FILE        0
#define STDDLG_NEW_FILE         (STDDLG_ALLOW_NON_EXISTANT)


#define PATH_ERROR	     2

// Dialog button values
#define SEL_BTN_OK           0
#define SEL_BTN_CANCEL       1
#define SEL_BTN_NOBUTTON     (Word)(-1)

#define MAX_REPORT_NAME		70		// Length of report file name

/************************************************************************
 * StdDlgSelectFile Data Structures                                     *
 ************************************************************************/

typedef struct {
    char name[8];
    char ext[3];
    }  FileNameRec;

typedef struct {
    FileNameRec far    *names;
    int                 lastNum;
    }  NameListStruct;


/*----------------------------------------------------------------------*
 * This is the stuff for invoking StdDlgOpenNewFile dialog.		*
 *----------------------------------------------------------------------*/
#define STDDLG_NO_APPEND	1	/* Don't allow append file option */

		// Result codes
#define FILE_OVERWRITE	0
#define FILE_APPEND	1

// ------------------------------------------------------------------------
// Constants for StdDlgNetGroups() and StdDlgNetUsers()
// ------------------------------------------------------------------------

#define	STDDLG_DEFAULT_SERVER	(Byte)(-1)

#define	MAX_NETWORK_GROUPS	64	// maximum network groups in the list
#define	MAX_NETWORK_USERS	1000	// maximum network users in the list

/*----------------------------------------------------------------------*
 * These functions must be provided by the programs using the NCD tree	*
 * module.  This lets each program store the directory tree however	*
 * they wish.								*								*
 *									*
 * TreeCount() must return the number of directories on the disk.	*
 *									*
 * For the remaining functions, entryNum is a directory list index from	*
 * zero (indicating the root dir) to TreeCount() - 1.			*
 *									*
 * TreeEntryName() must return the name of the directory as a 13-byte 	*
 * null terminated string.						*
 *									*
 * TreeEntryMoreInDir() must return TRUE iff the directory has more	*
 * siblings.								*		
 *									*
 * TreeEntryDepth() must return the depth of the directory--zero for	*
 * the root, one for dirs off the root, etc.				*
 *									*
 * TreeEntryParent() must return the list index number (entryNum)	*
 * of the directory's parent (the root is its own parent).		*
 *									*
 * TreeEntryPath() must return the full name of the directory, e.g.	*
 * "C:\DIR\SUBDIR".							*
 *----------------------------------------------------------------------*/
	
Word	TreeDirCount (void);
void	TreeEntryName (Word entryNum, Byte *name);
Boolean	TreeEntryMoreInDir (Word entryNum);
Word	TreeEntryDepth (Word entryNum);
Word	TreeEntryParent (Word entryNum);
void	TreeEntryPath (Word entryNum, Byte *path);


#ifdef INCL_DISK
/*----------------------------------------------------------------------*
 * These are constants used by routine which obtain ranges of clusters	*
 * sectors, or absolute sectors, e.g. StdDlgGetClusterRange()		*
 *----------------------------------------------------------------------*/
#define STD_DLG_GET_RANGE	0
#define	STD_DLG_GET_START_ONLY	1
#define	STD_DLG_NO_DEFAULT	2	/* Return -1 for blank fields	*/

#endif

/*----------------------------------------------------------------------*
 * These constants are returned by StdDlgConfirmDelete()
 *----------------------------------------------------------------------*/
#define DIRS_IN_DIRECTORY       -1
#define NO_FILES_IN_DIR         -2

extern Byte near STDDLG_HELP_SELECT_DRIVE_STR[];

/*----------------------------------------------------------------------*
 * This is the stuff for invoking StdDlgSelectDriveList dialog		*
 *----------------------------------------------------------------------*/
#define	FIELD_SIZE			55
#define	LOCAL_DRIVE_FIELD_SIZE		25

#define	DTYPE_END	-1
#define DTYPE_FLOPPY	1
#define DTYPE_LOCAL	2
#define DTYPE_NETWORK	4

#define	MAX_DRIVES_IN_LIST		MAX_DOS_DRIVES

#define SEL_DRIVE_LIST_HEIGHT		6
#define SEL_DRIVE_LIST_WIDTH		22

#define	TAG_TOGGLE	0
#define	TAG_ON		1
#define	TAG_OFF		2

#define SELDRV_FLAGS_ALL_DRIVES	1

#define	_IsTagged(a)	((a)->selected)
#define	_TagClear(a)	((a)->selected = FALSE)
#define	_TagSet(a)	((a)->selected = TRUE)

#define	MOUSE_NONE	0
#define	MOUSE_LEFT	1
#define	MOUSE_RIGHT	2
#define	MOUSE_BOTH	3

typedef struct 
    {
    Byte	dl;
    Byte	type;
    Byte 	driveIcon[4];
    Boolean	selected;
    } SelectDriveRec;

typedef struct
    {
    Byte    byFirst;
    Byte    byLast;
    } DRIVESDEF;


//***********************************************************************
// Things for Speed Search - SSEARCH.C
//***********************************************************************

#define	SPEED_SEARCH_MAX	30	// Maximum size of speed search box
#define	SPEED_SEARCH_FILE	12	// Size of a filename speed search box

		// -----------------------------------------------
		// Speed Search Matching callback function.
		// 
		// Returns NULL	if string matched (yes, matched)
		// -----------------------------------------------

typedef Word (MATCHFUNC)(ListRec 	*pList, 	\
			 void far 	*pListData, 	\
			 Ulong 		dwEntryNum, 	\
			 Byte 		*pszMatch, 	\
			 Word 		wLen);

Word PASCAL StdDlgSpeedSearch (
    ListRec		*pList, 
    Word 		*wEvent, 
    Word		wMaxChars,
    MATCHFUNC 		*pMatchFunc);

Boolean PASCAL StdDlgSpeedSearchDialog (
    ListRec 		*pList, 
    Byte 		byKey, 
    Word		wMaxChars,
    MATCHFUNC 		*pMatchFunc);


//-----------------------------------------------------------------------
// Flags returned by StdDlgConfigVideoAndMouse() 
//-----------------------------------------------------------------------

#ifdef INCL_NLIBCFG

#define	VID_GRAPHICS	0x0001
#define	VID_LINES	0x0002
#define	VID_COLORS	0x0004
#define	VID_DESKTOP	0x0008
#define	VID_DIALOGS	0x0010
#define	VID_MOUSE	0x0100
#define	VID_GMOUSE	0x0200

Word PASCAL StdDlgConfigVideoAndMouse(NLibConfigRec *configPtr);

#endif

#endif
