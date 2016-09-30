/* $Header:   V:/INCLUDE/VCS/DOS.H_V   1.9   15 Mar 1993 14:54:38   BASIL  $ */

/*----------------------------------------------------------------------*
 * DOS.H								*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   V:/INCLUDE/VCS/DOS.H_V  $
 * 
 *    Rev 1.9   15 Mar 1993 14:54:38   BASIL
 * Added return codes for DOSGetHostDrive()
 * 
 *    Rev 1.8   11 Mar 1992 13:41:22   CRAIG
 * Changed SYSTEM_HANDLE to SYSTEM_FILE_HANDLE because it was conflicting
 * with another SYSTEM_HANDLE in EMS.H.  Also changed PROCESS_HANDLE to
 * PROCESS_FILE_HANDLE for consistency's sake.
 * 
 *    Rev 1.7   10 Mar 1992 23:06:18   JOHN
 * Added bModified field to OpenFileInfoRec
 * 
 *    Rev 1.6   10 Mar 1992 14:39:48   JOHN
 * Added byDriveLetter to OpenFileInfoRec
 * 
 *    Rev 1.5   09 Mar 1992 21:45:56   JOHN
 * Added data structures related to System File Table (SFT)
 * 
 *    Rev 1.4   17 Feb 1992 11:46:28   ED
 * Added critical error stuff
 * 
 *    Rev 1.3   24 Oct 1991 14:29:24   PETERD
 * Added definitions for the 2 DOS version flags defined in DOS 5.0. 
 * These flags come along with the version number in DOS 5.0+.
 * 
 *    Rev 1.2   22 Apr 1991 14:26:56   PETERD
 * Added DOS 5.0 verison number.
 * 
 *    Rev 1.1   18 Dec 1990 18:25:56   HENRI
 * Modified the MemoryBlockRecord structure definition to
 * include the DOS 4.0 program name in the structure.  Previously
 * the structure contained an 11 byte "unused" array.
 * 
 *    Rev 1.0   29 Aug 1990 11:24:44   DAVID
 * Initial revision.
 */

#ifndef INCL_DOS
#define INCL_DOS

#define		DOS_3_00		0x0300
#define		DOS_3_20		0x0314
#define		DOS_3_21		0x0315
#define		DOS_3_30		0x031E
#define		DOS_3_31		0x031F
#define		DOS_4_00		0x0400
#define		DOS_5_00		0x0500

// Flags in dosVersionFlags
#define		DOS_IN_ROM		0x08
#define		DOS_IN_HMA		0x10

/*----------------------------------------------------------------------*
 * Norton Library Critical Error codes					*
 *									*
 * The global variable "error" will contain a value of 256 or greater	*
 * for critical errors.							*
 *----------------------------------------------------------------------*/

#define ErrorClear()                    (error = 0)
#define ErrorIsCritical()               (error >= 256)

#define CRIT_ERROR_WRITE_PROTECTED      0x100
#define CRIT_ERROR_UNKNOWN_DRIVE        0x101
#define CRIT_ERROR_DRIVE_NOT_READY      0x102
#define CRIT_ERROR_UNKNOWN_COMMAND      0x103
#define CRIT_ERROR_DATA_ERROR           0x104
#define CRIT_ERROR_BAD_REQUEST_LEN      0x105
#define CRIT_ERROR_SEEK_ERROR           0x106
#define CRIT_ERROR_NON_DOS_DISK         0x107
#define CRIT_ERROR_SECTOR_NOT_FOUND     0x108
#define CRIT_ERROR_OUT_OF_PAPER         0x109
#define CRIT_ERROR_WRITE_FAULT          0x10A
#define CRIT_ERROR_READ_FAULT           0x10B
#define CRIT_ERROR_GENERAL_FAILURE      0x10C

/*----------------------------------------------------------------------*
 * Program Segment Prefix Record					*
 *----------------------------------------------------------------------*/

#define PSP_START_BYTES 	0x20CD

typedef struct
    {
    Word exitCall;			/* 0xCD, 0x20			*/
    Word blockLength;
    Byte reserve1;			
    Byte cpmCall[5];			/* 0x9A, ...			*/
    void far *prevExitRoutine;
    void far *prevCtrlCRoutine;
    void far *prevFatalError;
    Word parentID;			/* (undocumented)		*/
    Byte handleTable[20];
    Word environmentSegPtr;
    void far *userStatck;
    Word handleCount;
    void far *handleAddress;
    Byte reserve2[24];
    Byte dosCall[3];			/* 0xCD, 0x21, 0xCB		*/
    Byte reserve3[9];
    Byte defaultFCB[16];
    Byte secondFCB[16];
    Byte reserve4[4];
    Byte commandLineLength;
    Byte commandLine[127];
    } PSPRecord;

/*----------------------------------------------------------------------*
 * DOS Memory Control Block Record definition				*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte id;				/* 'Z' for last block, else 'M' */
    Word pspSeg;			/* Segment pointer to P.S.P.    */
    Word size;				/* Size of this block		*/
    Byte reserved[3];			/* Unused			*/
    Byte owner[8];			/* DOS 4+: Owner name		*/
    } MemoryBlockRecord;

/*----------------------------------------------------------------------*
 * Device Driver Header Structure					*
 *----------------------------------------------------------------------*/

typedef struct DD_struct
    {
    struct DD_struct far *next;    	/* Next device driver in chain	*/
    Word attribute;			/* Attribute of driver		*/
    Word strategyRoutine;		/* Driver strategy routine 	*/
    Word interruptRoutine;		/* Driver interrupt routine 	*/
    Byte name[8];			/* Driver name			*/
    } DeviceDriverRec;

/*----------------------------------------------------------------------*
 * "nType" parameter to DOSGetOpenFileName() and DOSGetOpenFileInfo()	*
 *----------------------------------------------------------------------*/

#define PROCESS_FILE_HANDLE	1
#define SYSTEM_FILE_HANDLE	2

/*----------------------------------------------------------------------*
 * Information on an open file, in a "DOS version neutral" format	*
 *									*
 * This structure is filled by:						*
 *									*
 *	DOSGetOpenFileInfo()						*
 *	DOSGetInfoOnAllOpenFiles()					*
 *									*
 * This information is gathered by Norton Library by inspecting the SFT	*
 * entry for a file.  Not all versions of DOS supply all information.	*
 * Additional information is kept in the SFT, but is not included here	*
 * because it seemed highly unlikely we would ever care about it.	*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Word	wNumHandles;		// Number of handles referring to this file
    Boolean	bCharDevice;		// TRUE if character device
    Boolean	bNetworkFile;		// TRUE if network file
    Boolean	bModified;		// TRUE if data written to file
    Byte	byDriveLetter;		// Drive letter
    Byte	szName[13];		// Filename in NAME.EXT format
    Word	wOpenMode;		// Mode from AL register on open call
    Byte	byAttr;			// File attribute
    Word	wDate;			// File date in packed format
    Word	wTime;			// File time in packed format
    Ulong	dwSize;			// File size
    Ulong	dwPosition;		// Current file position
    Word	wStartCluster;		// First data cluster
    Ulong	dwDirSector;		// Sector containing directory entry
    Byte	byDirEntryNumber;	// Directory entry within sector
    Word	wOwnerPSP;		// PSP that owns file
    void	far *lpDriver;		// Character device driver or DPB
    } OpenFileInfoRec;

/*----------------------------------------------------------------------*
 * DOS internal data structures						*
 *----------------------------------------------------------------------*/

typedef struct SFTHeaderRec SFTHeaderRec;

typedef struct SFTHeaderRec
    {
    SFTHeaderRec far *lpNext;		// Next SFT block
    Word	 wFileCount;		// Number of files in this block
    Byte	 SFT[1];		// First SFT entry
    } SFTHeaderRec;

typedef struct
    {
    Byte	byNumHandles;		// Number of handles referring to this file
    Byte	byOpenMode;		// Mode from AL register on open call
    Byte	byAttr;			// File attribute
    Byte	byDrive;		// 0 = Device, 1 = A:, 2 = B:, etc.
    Byte	sName[11];		// Filename in FCB format
    Word	wUnknown1;
    Word	wUnknown2;
    Ulong	dwSize;			// File size
    Word	wDate;			// File date in packed format
    Word	wTime;			// File time in packed format
    Byte	byDevAttr;		// Device attribute
    union
	{
	struct
	    {
	    void far *lpDriver;		// Character device driver
	    } Char;
	struct
	    {
	    Word wStartCluster;		// First data cluster
	    Word wLastAccessed;		// Relative cluster of last access
	    } Block;
	} CB;
    Word	wCurrentCluster;	// Current cluster number
    Word	wUnknown3;
    Ulong	dwPosition;		// Current file position
    } Dos2SFT;

typedef struct
    {
    Word	wNumHandles;		// Number of handles referring to this file
    Word	wOpenMode;		// Mode from AL register on open call
    Byte	byAttr;			// File attribute
    Word	wDeviceInfo;		// Device information word
    void	far *lpDriver;		// Character device driver or DPB
    Word	wStartCluster;		// First data cluster
    Word	wTime;			// File time in packed format
    Word	wDate;			// File date in packed format
    Ulong	dwSize;			// File size
    Ulong	dwPosition;		// Current file position
    Word	wLastAccessed;		// Relative cluster of last access
    Word	wCurrentCluster;	// Current cluster number
    Word	wDirSector;		// Sector containing directory entry
    Word	wDirEntryOffset;	// Directory entry within sector
    Byte	sName[11];		// Filename in FCB format
    Ulong	dwShare1;
    Word	wShare2;
    Word	wOwnerPSP;		// PSP that owns file
    Word	wShare3;
    } Dos30SFT;

typedef struct
    {
    Word	wNumHandles;		// Number of handles referring to this file
    Word	wOpenMode;		// Mode from AL register on open call
    Byte	byAttr;			// File attribute
    Word	wDeviceInfo;		// Device information word
    void	far *lpDriver;		// Character device driver or DPB
    Word	wStartCluster;		// First data cluster
    Word	wTime;			// File time in packed format
    Word	wDate;			// File date in packed format
    Ulong	dwSize;			// File size
    Ulong	dwPosition;		// Current file position
    Word	wLastAccessed;		// Relative cluster of last access
    Word	wCurrentCluster;	// Current cluster number
    Word	wDirSector;		// Sector containing directory entry
    Byte	byDirEntryNumber;	// Directory entry within sector
    Byte	sName[11];		// Filename in FCB format
    Ulong	dwShare1;
    Word	wShare2;
    Word	wOwnerPSP;		// PSP that owns file
    Word	wShare3;
    } Dos31SFT;

typedef struct
    {
    Word	wNumHandles;		// Number of handles referring to this file
    Word	wOpenMode;		// Mode from AL register on open call
    Byte	byAttr;			// File attribute
    Word	wDeviceInfo;		// Device information word
    void	far *lpDriver;		// Character device driver or DPB
    Word	wStartCluster;		// First data cluster
    Word	wTime;			// File time in packed format
    Word	wDate;			// File date in packed format
    Ulong	dwSize;			// File size
    Ulong	dwPosition;		// Current file position
    Word	wLastAccessed;		// Relative cluster of last access
    Ulong	dwDirSector;		// Sector containing directory entry
    Byte	byDirEntryNumber;	// Directory entry within sector
    Byte	sName[11];		// Filename in FCB format
    Ulong	dwShare1;
    Word	wShare2;
    Word	wOwnerPSP;		// PSP that owns file
    Word	wShare3;
    Word	wShare4;
    Ulong	dwIFS1;
    } Dos4SFT;

typedef union
    {
    Dos2SFT	Dos2;
    Dos30SFT	Dos30;
    Dos31SFT	Dos31;
    Dos4SFT	Dos4;
    } SFTEntryRec;

/************************************************************************
 * Return codes for DOSGetHostDrive() (\LIBSRC\DOS\REALBOOT.C)          *
 ************************************************************************/

#define  HOSTDRIVE_FOUND                0
#define  HOSTDRIVE_NOT_SWAPPED          1
#define  HOSTDRIVE_NULLED               2
#define  HOSTDRIVE_DOUBLESPACE_ERROR    3
#define  HOSTDRIVE_STACKER_ERROR        4

#endif /* INCL_DOS */
