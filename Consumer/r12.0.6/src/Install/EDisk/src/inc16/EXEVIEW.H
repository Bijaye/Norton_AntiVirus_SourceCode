/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/EXEVIEW.H_V   1.0   13 Mar 1992 20:40:26   TORSTEN  $ *
 *                                                                      *
 * Description: Constants and Data Types used by the Executable File    *
 *              Viewer                                                  *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************ 
 * $Log:   S:/INCLUDE/VCS/EXEVIEW.H_V  $ *
 * 
 *    Rev 1.0   13 Mar 1992 20:40:26   TORSTEN
 * Initial revision.
 ************************************************************************/

#ifndef INCL_SV_EXEVIEW
#define INCL_SV_EXEVIEW

// ------------------------------------------------------------------------
// Old Executable Header
//
//
// ------------------------------------------------------------------------
 
typedef	struct
    {
    Word	wSignature;		// signature "MZ" (actually "ZM")
    Word	wExtraBytes;		// bytes in last (partial) page
    Word	wPages;			// whole and partial pages in file
    Word	wRelocItems;		// pointers in relocation table
    Word	wHeaderParagraphs;	// header size in paragraphs
    Word	wMinAlloc;		// minimum allocation
    Word	wMaxAlloc;		// maximum allocation
    Word	wInitialSS;		// initial stack segment value
    Word	wInitialSP;		// initial stack pointer value
    Word	wChecksum;		// complemented checksum
    Word	wInitialIP;		// initial IP value
    Word	wInitialCS;		// initial CS value
    Word	wRelocTableOffset;	// offset to relocation table
    Word	wOverlayNumber;		// overlay number
    Byte	wastedSpace [32];	// used to get to offset 3Ch
    Ulong	dwNewEXEHeaderOffset;	// offset of New Executable header
    } EXEHEADER;

typedef	EXEHEADER		*PEXEHEADER;
typedef	EXEHEADER NEAR		*NPEXEHEADER;
typedef	EXEHEADER FAR		*LPEXEHEADER;

#define	EXE_SIGNATURE	0x5A4D		// 'MZ' signature

// ------------------------------------------------------------------------
// New Executable Header
//
// Some of the information about the new-style header appears in a 
// document published in the Windows 3.1 SDK documentation.
// 
// All offsets are relative to the beginning of the New Executable header.
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// Module Flag Word
// ------------------------------------------------------------------------

typedef struct tagMODULEFLAG		/* Bits in fKeyword field	*/
    {
    Word nDGroupType	: 2;		// DGROUP configuration (see below)
    Word bReserved0	: 1;
    Word bProtMode	: 1;		// .EXE runs only in protected mode
    Word bReserved1	: 4;
    Word nType		: 2;		// OS/2 Compatibility (see below)
    Word bReserved2	: 1;
    Word bPrivateLoader	: 1;		// .EXE has a private loader
    Word bReserved3	: 1;
    Word bLinkErrors	: 1;		// link errors detected at link time
    Word bEMSLibrary	: 1;		// library resides above EMS bank line
    Word bLibrary	: 1;		// .EXE is a library module
    } MODULEFLAG;

typedef	struct
    {
    Word	wSignature;		// signature "NE" (actually "EN")
    Byte	byLinkVersion;		// link version number
    Byte	byLinkRevision;		// link revision number
    Word	wEntryTableOffset;	// beginning of the entry table
    Word	wEntryTableLen;		// length of the entry table
    Ulong	dwContentsChecksum;	// 32-bit checksum of the entire file
    MODULEFLAG	ModuleFlag;		// module flag word (see below)
    Word	wInitalDSNumber;	// segment number for automatic data
    Word	wLocalHeapSize;		// size of local heap
    Word	wStackSize;		// size of stack
    Word	wInitialIP;		// initial value of the IP register
    Word	wInitialCSNumber;	// initial code segment number
    Word	wInitialSP;		// initial value of SP
    Word	wInitialSSNumber;	// initial stack segment number
    Word	wSegmentTableEntries;	// segment table entries
    Word	wModuleTableEntries;	// module reference table entries
    Word	wNonResidentTableSize;	// bytes in the non-resident table
    Word	wSegmentTableOffset;	// offset of segment table
    Word	wResourceTableOffset;	// offset of resource table
    Word	wResidentNamesOffset;	// offset of resident names table
    Word	wModuleReferenceOffset;	// offset of module reference table
    Word	wImportedNamesOffset;	// offset of imported names table
    Ulong	dwNonResidentNamesOffset; // offset of non-resident names table
    Word	wMovableEntryPoints;	// movable entry points in entry table
    Word	wAlignmentShiftCount;	// alignment shift count (/ALIGN switch)
    Word	wResourceSegments;	// number of resource segments
    Byte	byOperatingSystem;	// operating system (see below)
    Byte	byWindowsFlags;		// Windows flags (see below)
    Word	wFastLoadArea;		// start of the fast load area (Windows)
    Word	wFastLoadAreaSize;	// length of the fast load area
    Word	wReserved;
    Word	wWindowsVersion;	// expected Windows version number
    } NEWEXEHEADER;

typedef	NEWEXEHEADER		*PNEWEXEHEADER;
typedef	NEWEXEHEADER NEAR	*NPNEWEXEHEADER;
typedef	NEWEXEHEADER FAR	*LPNEWEXEHEADER;


#define	NEW_EXE_SIGNATURE	0x454E	// 'NE' signature

#define	NO_LOCAL_HEAP		0	// possible wLocalHeapSize value
#define	LIBRARY_MODULE		0	// possible value for wStackSize
					// and wInitialSSNumber
#define	AUTOMATIC_STACK		0	// possible value for wInitialSP

// -----------------------------------------
// Values for byOperatingSystem
// -----------------------------------------

#define	OS_UNKNOWN		0	// Unknown format
#define	OS_OS2			1	// OS/2
#define	OS_WINDOWS		2	// Windows 3.x
#define	OS_DOS			3	// DOS? (Reserved)
#define	OS_RESERVED		4	// Reserved

// -----------------------------------------
// Bit Masks for byWindowsFlags
// -----------------------------------------

#define	WIN_OLD_REAL		0x00	// 2.x app runs only in real mode
#define	WIN_OLD_PROTECTED	0x01	// 2.x app runs in 3.x protected mode
#define	WIN_OLD_PROPORTIONAL	0x02	// 2.x app supports proportional fonts
#define	WIN_FAST_LOAD_AREA	0x04	// .EXE contains a fast load area

// -----------------------------------------
// DGROUP Type (in MODULEFLAG)
// -----------------------------------------

#define	NOAUTODATA	0
#define	SINGLEDATA	1
#define	MULTIPLEDATA	2
 
// -----------------------------------------
// OS/2 Compatibility (in MODULEFLAG)
// -----------------------------------------

#define	OS2_NOT_WINDOWS_COMPATIBLE	1	// Windows Libraries for OS/2
#define	OS2_WINDOWS_COMPATIBLE		2
#define	OS2_PM_API			3


#endif

