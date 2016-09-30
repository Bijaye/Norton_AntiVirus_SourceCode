/* $Header:   I:/INCLUDE/VCS/NUCONFIG.H_V   1.31   16 Mar 1993 16:05:26   CRAIG  $ */

/*----------------------------------------------------------------------*
 * NUCONFIG.H								*
 *									*
 * Structure used to store Norton Utilities configuration in NORTON.INI	*
 *									*
 * Also contains structure used by Norton Integrator to launch programs	*
 * and save integrator state information				*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   I:/INCLUDE/VCS/NUCONFIG.H_V  $
 * 
 *    Rev 1.31   16 Mar 1993 16:05:26   CRAIG
 * Added a byte to the Diskreet record.
 *
 *    Rev 1.30   26 Jan 1993 14:17:30   JOHN
 * Changed NDDConfigRec to match Quake version of the code.
 *
 *    Rev 1.29   30 Dec 1992 11:16:12   SKURTZ
 * Added byAreaToSearch to FileFind structure
 *
 *    Rev 1.28   04 Dec 1992 11:36:16   CRAIG
 * Added versions #defines, changed NUConfigRec to use old structures.
 *
 *    Rev 1.27   03 Dec 1992 17:54:32   BASIL
 * Removed the drivestype record, put as part of the sformat record.
 *
 *    Rev 1.26   03 Dec 1992 11:48:38   BASIL
 * Added DrivesTypeRec, PrinterConfigRec and protos for NUConfigRead/Write()
 *
 *    Rev 1.25   02 Dec 1992 17:53:50   BASIL
 * Updated the safe format config record
 *
 *    Rev 1.24   02 Dec 1992 15:48:08   SKURTZ
 * nothing done
 *
 *    Rev 1.23   02 Dec 1992 15:18:20   SKURTZ
 * Added new config strucs for FILEFIND,NCD,NI,and WIPEINFO
 *
 *    Rev 1.22   02 Dec 1992 13:46:22   KEVIN
 * added info for diskedit
 *
 *    Rev 1.20   02 Dec 1992 12:39:46   JOEP
 * Added info for ndiags
 *
 *    Rev 1.19   02 Dec 1992 11:44:42   CRAIG
 * Changes for new Config Manager.
 *
 *    Rev 1.18   20 Oct 1992 16:50:12   CRAIG
 * Added another field to NCryptRec; no effect on structure.
 *
 *    Rev 1.17   16 Oct 1992 20:40:56   JOHN
 * [John]  Updated NULoaderDataRec for NU 7.0.
 * [Craig] Added noConfirmSuccess to NCryptConfigRec.
 *
 *    Rev 1.16   28 Sep 1992 09:53:42   CRAIG
 * Added a field to the NCrypt structure.  This does not change the
 * overall structure of the file, as the field was previously reserved.
 *
 *    Rev 1.15   27 Feb 1992 18:59:38   DAVIDH
 * Added printerCfgID byte to NULoaderDataRec, borrowing from existing
 * structure space.
 *
 *    Rev 1.14   27 Jan 1992 22:36:00   TORSTEN
 * Changed the #include statements to use angle brackets.
 *
 *    Rev 1.13   09 Dec 1991 10:12:40   ED
 * Added saveOnExit to the Safe Format record
 *
 *    Rev 1.12   26 Mar 1991 15:09:18   JOHN
 * Split NUCONFIG.H into two files.  NUCONFIG.H now has structures for
 * Norton Utilities, and NLIBCFG.H has structures for the Norton Library.
 *
 *    Rev 1.11   05 Mar 1991 20:14:56   BARRY
 * Added prototypes for ConfigRead.
 *
 *    Rev 1.10   01 Mar 1991 18:25:48   PETERD
 * Added canCopyTree to NCDRec.
 *
 *    Rev 1.9   22 Feb 1991 19:20:32   PETERD
 * Changed the drive list in the FileFind structure to a long (32-bits).
 *
 *    Rev 1.8   22 Feb 1991 13:41:42   PETERD
 * Added options for Speedisk (sound on/off for now).
 *
 *    Rev 1.7   22 Feb 1991 10:19:46   HENRI
 * Modified the DiskExplorerConfigRec adding the escapePrompt field.
 *
 *    Rev 1.6   19 Feb 1991 20:13:34   JOHN
 * Added configuration item for NCD.
 *
 *    Rev 1.5   19 Feb 1991 20:02:04   JOHN
 * Undid changes made in version 1.4.  The extra space caused some
 * programs to exceed the maximum DGROUP size.
 *
 *    Rev 1.4   19 Feb 1991 19:53:46   JOHN
 * Added 3500 bytes of reserved area at the end of the NUConfigRec.
 * This will allow NC, NBU, and NAV setup data to be added later.
 *
 *    Rev 1.3   29 Jan 1991 23:50:50   JOHN
 * Added "ctrlEnterAccepts" "arrowsOnButtons" and "intenseBackground"
 * fields to the NUConfigRec structure.
 *
 *    Rev 1.2   25 Sep 1990 13:13:22   JOHN
 * Changed SFORMAT options to include new bit flags and reserved bytes.
 * There used to be space for a default volume label, but it wasn't being used.
 *
 *    Rev 1.1   05 Sep 1990 15:27:46   DAVID
 * Added NUFastInit() prototype.
 *
 *    Rev 1.0   29 Aug 1990 11:24:14   DAVID
 * Initial revision.
 *
 *
 * History prior to PVCS:
 *
 * 01/31/90  David: created file
 * 02/28/90  Henri: added DE config info
 * 03/01/90  David: moved NU loader shared data info to here from
 *           Norton Integrator code; needed for NCC
 * 04/12/90  Keith: Added DistributionFileRec fields: 'size' &
 *	     'doInstall'.
 * 04/26/90  David: Added 'pause' field to NULoaderDataRec and cut
 *	     the max loader commands from 5 to 1.
 * 04/27/90  David: Added 'mouseAvail' and 'sortByName' fields to
 *	     NULoaderDataRec
 * 05/11/90  Keith: Added 'id', 'pwd' 'files' and 'exeAlias' to
 *	     DistributionFileRec.
 * 06/27/90  Keith: Trimmed 'description' field of DistributionFileRec
 *	     from 40 bytes to 30 bytes.
 */

#ifndef INCL_NUCONFIG
#define	INCL_NUCONFIG

#ifndef INCL_TYPES
#include <types.h>
#endif

#ifndef INCL_DISK
#include <disk.h>
#endif

#ifndef INCL_NLIBCFG
#include <nlibcfg.h>
#endif

/*----------------------------------------------------------------------*
 * Signature and version in NORTON.INI file (obsolete)                  *
 *----------------------------------------------------------------------*/

#define NU_SIG    	"NU"		/* Product sig for Utilities	*/
#define NU_VER		0x0001		/* Version of NU data		*/

/*----------------------------------------------------------------------*
 * Each program which uses the config file should define a record here	*
 * describing its section of the file.					*
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*
 * Disk Edit configuration information					*
 *----------------------------------------------------------------------*/

#define CFGVER_DISKEDIT 0x100

typedef struct
    {
    Byte byVersion;
    Byte readOnly        : 1;
    Byte quickMode       : 1;
    Byte autoView        : 1;
    Byte quickLinks      : 1;
    Byte charTranslation : 2;
    Byte escapePrompt 	 : 1;
    Byte warnUserCalc	 : 1;
    } DiskExplorerConfigRec;

typedef struct
    {
    Byte readOnly        : 1;
    Byte quickMode       : 1;
    Byte autoView        : 1;
    Byte quickLinks      : 1;
    Byte charTranslation : 2;
    Byte escapePrompt 	 : 1;
    Byte reserved  	 : 1;
    } oldDiskExplorerConfigRec;

/*----------------------------------------------------------------------*
 * Diskreet configuration information		       			*
 *----------------------------------------------------------------------*/

#define CFGVER_DISKREET 0x101

typedef struct
    {
    Byte byVersion;

    Byte wipeOriginal    : 1;
    Byte makeHidden      : 1;
    Byte makeReadOnly    : 1;
    Byte samePassword    : 1;
    Byte method          : 1;
    Byte noNotLoadedMsg  : 1;
    Byte noConfirmSuccess: 1;
    Byte noFileWipeQuery : 1;

    Byte noDblSpcMsg     : 1;
    Byte reserved        : 7;           // fill out the last byte
    } NCryptConfigRec;

typedef struct
    {
    Byte wipeOriginal    : 1;
    Byte makeHidden      : 1;
    Byte makeReadOnly    : 1;
    Byte samePassword    : 1;
    Byte method          : 1;
    Byte noNotLoadedMsg  : 1;
    Byte noConfirmSuccess: 1;
    Byte noFileWipeQuery : 1;
    } oldNCryptConfigRec;

/*----------------------------------------------------------------------*
 * Norton Disk Doctor configuration information				*
 *----------------------------------------------------------------------*/

#define CFGVER_NDD      0x100

typedef struct NDDConfigRec
    {
    WORD        wVersion;               /* Config record version number */

    // Surface Test options
    BYTE        test;                   /* Test Type (DISK or FILE)     */
    BYTE        testType;               /* Daily or Weekly test         */
    BYTE        repairSetting;          /* How to Repair error          */
    BYTE        repetitionMode;         /* CONTINUOUS or PASS           */
    WORD        maxPasses;              /* # of passes (0-999)          */

    // User Message options
    BYTE        userMsg;                /* User message prompt          */

    // Test To Skip options
    BYTE        skipPartitionTests;     /* Skip partition tests         */
    BYTE        skipCMOSTests;          /* Skip CMOS tests              */
    BYTE        skipCompressionTests;   /* Don't test compressed drive  */
    BYTE        skipHostDriveTest;      /* Don't test compressed host   */
    BYTE        skipSurfaceTests;       /* Skip Surface Tests           */
    BYTE        only1HardDisk;          /* Only 1 hard disk             */
    BYTE        szExcludedDrives[27];   /* List of excluded drives      */
    BYTE        byAutoRepair[10];       /* Prompt/Skip/AutoRepair       */
    BYTE        byDeleteLostClusters;   /* Auto delete lost clusters?   */
    } NDDConfigRec;

typedef struct
    {
    Byte test		: 1;		/* Test Type (DISK or FILE)	*/
    Byte testType	: 2;		/* Daily or Weekly test		*/
    Byte repairSetting	: 2;		/* How to Repair error		*/
    Byte repetitionMode	: 1;		/* CONTINUOUS or PASS		*/
    Byte userMsg	: 1;		/* User message prompt		*/
    Byte skipPartitionTests : 1;	/* Skip partition tests		*/
    Byte skipCMOSTests	: 1;		/* Skip CMOS tests		*/
    Byte skipSurfaceTests   : 1;	/* Skip Surface Tests		*/
    Byte only1HardDisk	: 1;		/* Only 1 hard disk		*/
    Word maxPasses;			/* # of passes (0-999)		*/
    } oldNDDConfigRec;

/*----------------------------------------------------------------------*
 * Safe Format configuration information			        *
 *----------------------------------------------------------------------*/

#define CFGVER_SFORMAT  0x100

typedef struct
    {
    Byte byVersion;
    Byte saveFRInfo	: 1;		/* run IMAGE before format      */
    Byte formatType	: 3;		/* SAFE, QUICK or DOS           */
    Byte system         : 3;		/* DON'T SYS, SYS or LEAVE SPC  */
    Byte allowHDFormat	: 1;		/* Allow hard disk formatting   */
    Byte drive;                 	/* default drive (unused)       */
    Byte size;                   	/* default size                 */
    Byte timeoutBiosBug : 1;		/* Toshiba 5100 BIOS bug?	*/
    Byte saveOnExit     : 1;            /* Save settings on exit        */
    Byte reservedFlags  : 6;		/* Reserved bit flags		*/
    Byte drivesType[MAX_DOS_DRIVES];    /* Previously in NUConfigRec    */
    } SFConfigRec;

typedef struct
    {
    Byte saveFRInfo	: 1;		/* run IMAGE before format      */
    Byte formatType	: 3;		/* SAFE, QUICK or DOS           */
    Byte system         : 3;		/* DON'T SYS, SYS or LEAVE SPC  */
    Byte allowHDFormat	: 1;		/* Allow hard disk formatting   */
    Byte drive;                 	/* default drive (unused)       */
    Byte size;                   	/* default size                 */
    Byte timeoutBiosBug : 1;		/* Toshiba 5100 BIOS bug?	*/
    Byte saveOnExit     : 1;            /* Save settings on exit        */
    Byte reservedFlags  : 6;		/* Reserved bit flags		*/
    Byte reserved[11];			/* Reserved bytes		*/
    } oldSFConfigRec;

/*----------------------------------------------------------------------*
 * Wipe Info configuration information				        *
 *----------------------------------------------------------------------*/

#define CFGVER_WIPEINFO 0x100

typedef struct
    {
    Byte byVersion;
    Byte includeSubdirs	: 1;		/* Include subdirs when wiping  */
    Byte confirm	: 1;		/* Confirm each file            */
    Byte readonly	: 1;		/* Wipe read only files ?       */
    Byte hidden 	: 1;		/* Wipe hidden files?           */
    Byte wipeMode	: 1;		/* Govern or Fast               */
    Byte wipeValue;                     /* Fast wipe value              */
    Byte dodWipeValue;                  /* Government wipe value        */
    Byte dodRepeatCount;                /* Government repeat count (3)  */
    Byte repeatCount;                   /* Total repeat count           */
    Byte diskMode;                 	/* Wipe entire or Wipe erased   */
    Byte fileMode;                   	/* Wipe, delete or wipe slack   */
    } WipeInfoConfigRec;

typedef struct
    {
    Byte includeSubdirs	: 1;		/* Include subdirs when wiping  */
    Byte confirm	: 1;		/* Confirm each file            */
    Byte readonly	: 1;		/* Wipe read only files ?       */
    Byte hidden 	: 1;		/* Wipe hidden files?           */
    Byte wipeMode	: 1;		/* Govern or Fast               */
    Byte wipeValue;                     /* Fast wipe value              */
    Byte dodWipeValue;                  /* Government wipe value        */
    Byte dodRepeatCount;                /* Government repeat count (3)  */
    Byte repeatCount;                   /* Total repeat count           */
    Byte diskMode;                 	/* Wipe entire or Wipe erased   */
    Byte fileMode;                   	/* Wipe, delete or wipe slack   */
    } oldWipeInfoConfigRec;

/*----------------------------------------------------------------------*
 * Norton Integrator configuration information				*
 *----------------------------------------------------------------------*/

#define CFGVER_NORTON   0x100

typedef struct
    {
    Byte 	byVersion;
    Boolean	sortByName;		/* Command list sort order flag	*/
    } NIRec;

typedef struct
    {
    Boolean	sortByName;		/* Command list sort order flag	*/
    } oldNIRec;

/*----------------------------------------------------------------------*
 * File Find configuration information					*
 *----------------------------------------------------------------------*/

#define CFGVER_FILEFIND 0x101

typedef struct
    {
    Byte byVersion;
    Ulong userDrives;			/* A packed strcture of user 	*/
 					/* search drives         	*/
    Byte byAreaToSearch;		/* what area to search 		*/
    } FileFindRec;

typedef struct
    {
    Ulong userDrives;			/* A packed strcture of user 	*/
 					/* search drives         	*/
    } oldFileFindRec;

/*----------------------------------------------------------------------*
 * NCD configuration information					*
 *----------------------------------------------------------------------*/

#define CFGVER_NCD      0x100

typedef struct
    {
    Byte 	byVersion;
    Byte	canRemoveTree    : 1;
    Byte	canPruneAndGraft : 1;
    Byte	canCopyTree      : 1;
    } NcdRec;

typedef struct
    {
    Byte	canRemoveTree    : 1;
    Byte	canPruneAndGraft : 1;
    Byte	canCopyTree      : 1;
    } oldNcdRec;

/*----------------------------------------------------------------------*
 * SPEEDISK configuration information					*
 *----------------------------------------------------------------------*/

#define CFGVER_SPEEDISK 0x100

typedef struct
    {
    Byte        byVersion;
    Byte	beepOff : 1;
    } SpeediskRec;

typedef struct
    {
    Byte	beepOff : 1;
    } oldSpeediskRec;

//
// NDIAGS configuration information
//
// Due to problems with H2INC, all configuration
// information for NDiags is stored locally
//

/************************************************************************
 * Printer configuration record                                         *
 ************************************************************************/

#define CFGVER_PRINTER  0x100

typedef  struct
    {
    Byte        byVersion;
    Byte        byPrinterCfgID;
    } PrinterConfigRec;

/*----------------------------------------------------------------------*
 * Format of NUConfigRec						*
 *									*
 * If you make changes here, be sure to update NUConfigReset().		*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte                           drivesType[MAX_DOS_DRIVES];
    oldDiskExplorerConfigRec       DE;
    oldNDDConfigRec                NDD;
    oldSFConfigRec                 SF;
    oldWipeInfoConfigRec           WipeInfo;
    oldNCryptConfigRec             NCRYPT;
    oldNIRec                       NI;
    oldFileFindRec                 FileFind;
    oldNcdRec                      NCD;
    oldSpeediskRec                 SD;

		                        /* When you add fields to this	*/
					/* record, take the space from	*/
					/* the reserved bytes, so that	*/
					/* a recompile by everyone is	*/
					/* not needed immediately.	*/
    Byte        reserved[ 32 ];
    } NUConfigRec;

/*----------------------------------------------------------------------*
 * Format of the Norton Integrator Loader's shared data area.		*
 *----------------------------------------------------------------------*/

#define NU7_LOADER_VERSION		0x7FAC

#define NU7_LIBRARY_VERSION		0x0400

#define NU_LOADER_MAX_CMD_LEN		128
#define NU_LOADER_MAX_CMDS		1	/* was 5 */

typedef struct
    {
    Byte	signature[2];		/* Make sure this is "ni"	*/
    Byte	firstLoad;		/* Always 1 on first load	*/
    Byte	originalAttr;		/* ColorGetOldAttr() value	*/
    Word	dosCursorSize;		/* dosCursorSize value		*/
    Byte	sortByName;		/* sortByName 			*/
    Ulong	mainListTop;		/* mainList.top_entry		*/
    Ulong	mainListActive;		/* mainList.active_entry	*/
    Byte	passwordOK;		/* User has supplied the pw	*/
    Byte	skipPassword;		/* Checked by launched utils	*/
    Byte	pause;			/* Pause after each command	*/
    Byte	mouseAvail;		/* Test mouse clicks at pause	*/
    Word	mouseRow;		/* Mouse position saved across	*/
    Word	mouseCol;		/*  launch/return to/from NI	*/
    Word	numCmds;
    Byte	cmdBuffers[NU_LOADER_MAX_CMDS][NU_LOADER_MAX_CMD_LEN];
    Byte	commandFile[80];	/* name of menu command file	*/
    Byte        printerCfgID;           /* Printer config file ID num   */
    Word	wLoaderDataVersion;	/* NULoaderDataRec version	*/
    Word	wChildLibraryVersion;	/* Library version of child	*/
    Boolean	bScreenWasCleared;	/* Child program cleared screen */
    } NULoaderDataRec;

// NOTE: A number of the fields in NULoaderDataRec are no longer used,
//	 but we can't remove them for reasons of backwards compatibility.
//	 The ones currently unused are:
//
//		signature
//		firstLoad
//		pause
//		mouseAvail
//		numCmds
//		cmdBuffers
//
//	 New data items should be added at the end.

/*----------------------------------------------------------------------*
 * Function prototypes							*
 *----------------------------------------------------------------------*/

Word PASCAL NUFastInitScreen(void);
void PASCAL NUFastRestoreScreen(void);
void PASCAL NUConfigError(Word errNum, Boolean reportCritErrs);

NULoaderDataRec far* NULoaderPresent(void);

/* These routines are only used for standalone .INI files (e.g. NDD.INI) */
Word PASCAL ConfigRead(Byte *fileName, void *data, Word sizeOfData);
Word PASCAL ConfigWrite(Byte *fileName, void *data, Word sizeOfData);

#endif
