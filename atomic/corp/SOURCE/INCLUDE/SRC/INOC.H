// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/inoc.h_v   1.0   06 Feb 1997 20:52:04   RFULLER  $
//
// Description:
//      Header file for the INOCULAT project for NAV 3.0
//
// Contains:
//      SIZE_INDEX  size of the inoculation database hash index table.
//      Various other constants and typedefs for NAV's inoculation code.
//
// See Also:
//************************************************************************
// $Log:   S:/include/VCS/inoc.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:52:04   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:00   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:22   BARRY
// Initial revision.
// 
//    Rev 1.12   19 Jun 1995 14:59:52   KEITH
// Added SIZE_INOCULATE_WORKMEM and SIZE_INOCULATE_WORKMEM2 so that VNAVAPD 
// and NAVINOC could use the same sizes for the INOCULATE struct.
// 
//    Rev 1.11   26 Apr 1995 16:36:34   MFALLEN
// Added callback capability to uninoculations.
// 
//    Rev 1.10   12 Apr 1995 15:38:34   MFALLEN
// Specific internal error define.
// 
//    Rev 1.9   31 Mar 1995 11:48:58   MFALLEN
// Put back uninoc stuff for my buddy Dave.
// 
//    Rev 1.8   31 Mar 1995 09:47:40   MFALLEN
// Moved inoc/uninoc file UI to NAVW project.
// 
//    Rev 1.7   23 Mar 1995 17:00:06   SZIADEH
// changed CHAR to char
// 
//    Rev 1.6   23 Mar 1995 14:49:38   SZIADEH
// added UNC support
// 
//    Rev 1.5   10 Feb 1995 09:37:56   DALLEE
// Uninoc callback now returns BOOL -- TRUE to continue, FALSE to cancel.
// 
//    Rev 1.4   06 Feb 1995 15:01:52   MFALLEN
// All function prototypes with extern 'C' around it.
// 
//    Rev 1.3   06 Feb 1995 14:10:56   BRAD
// Moved external functions to .C format
// 
//    Rev 1.2   03 Feb 1995 15:54:00   BRAD
// Added LongFile support
// 
//    Rev 1.1   28 Nov 1994 14:14:50   MFALLEN
// W16 platform specific stuff.
// 
//    Rev 1.0   31 Oct 1994 12:48:36   MFALLEN
// Initial revision.
// 
//    Rev 1.35   02 Jun 1994 14:51:58   RSTANEV
// Prototype for InocGetVersion.
//
//    Rev 1.34   02 Jun 1994 14:47:20   RSTANEV
// Added INOC_VERSION_??? defines.
//
//    Rev 1.33   01 Mar 1994 12:09:14   DALLEE
// RegisterFile to RegFile() to better mirror naming conventions with
// inoculation routines.
//
//    Rev 1.32   24 Feb 1994 13:57:02   DALLEE
// Uncomment #ifdef BANKERS.
//
//    Rev 1.31   23 Feb 1994 21:10:18   DALLEE
// RegisterFile() and IsRegistered() prototypes.
//
//    Rev 1.30   20 Dec 1993 11:13:26   MFALLEN
// Added SmartInocGetStatus()
//
//    Rev 1.29   07 Sep 1993 01:02:34   DALLEE
// Added INOC_CHANGED #define for IsInoculated return value.
//
//    Rev 1.28   24 Aug 1993 13:37:10   BARRY
// Removed Inoc.lpQuick.  Locks on the fly now so DOS VMM can cope better
//
//    Rev 1.27   04 Aug 1993 15:05:34   DALLEE
// Added SIZE_FLOPPY_INDEX #define of 512.
//
//    Rev 1.26   30 Jul 1993 12:56:20   CGUC
// Added hMemWork2 to INOCULATE structe.
//
//    Rev 1.25   07 Jul 1993 22:11:08   CJKUO
// Change prototype of InocInit to use LPNAVOPTIONS
// Add prototype for RememberInocFloppy
//
//    Rev 1.24   30 Jun 1993 16:12:16   MFALLEN
// SmartInocOpen()
//
//    Rev 1.23   22 Jun 1993 00:49:24   DALLEE
// extern bInocOpenOK and #define INOC_OPEN_RW 2
//
//    Rev 1.22   18 Jun 1993 15:43:12   MFALLEN
// Moved init code from LibMain and WEP
//
//    Rev 1.21   16 Jun 1993 18:09:20   CJKUO
// Change prototype of GetUnusedInocStruct
//
//
//    Rev 1.20   31 May 1993 22:28:50   MFALLEN
// Directory uninoculation
//
//    Rev 1.19   25 May 1993 13:40:38   CJKUO
// Remove variable names out of prototype lines because Patrick said so.
//
//    Rev 1.18   25 May 1993 01:40:38   DALLEE
// Added InocGetNumEntries() and InocGetEntry() prototypes.
//
//    Rev 1.17   19 May 1993 20:43:54   BRAD
// Added GetInocStructGivenFile().
//
//    Rev 1.16   16 May 1993 15:40:28   CJKUO
// Add prototype for BuildInocDBName, its Flag defines, and alter
// InocInit prototype
//
//    Rev 1.15   12 May 1993 18:51:38   CJKUO
// Change prototypes of InocHash and InocComp
//
//    Rev 1.14   12 May 1993 11:17:58   MFALLEN
// Changed tagINOCInstances *_ii_next; to FAR *
//
//    Rev 1.13   11 May 1993 15:01:08   CJKUO
// Add prototypes for InocInit and InocEnd
//
//    Rev 1.12   10 May 1993 16:47:14   CJKUO
// Change prototype for GetUnusedInocStruct to match InocOpen
//
//    Rev 1.11   05 May 1993 20:38:46   CJKUO
// Changed prototype line of a second GetInocStruct to be
// GetUnusedInocStruct.
//
//    Rev 1.10   05 May 1993 20:31:34   CJKUO
// To match inocdb.c introductions.
//
//    Rev 1.9   30 Apr 1993 13:20:58   MFALLEN
//
//    Rev 1.8   21 Apr 1993 14:26:36   MFALLEN
//
//    Rev 1.7   21 Apr 1993 12:56:32   DALLEE
// Prototypes for InocRepairFile and InocGetEntry.
//
//    Rev 1.6   16 Apr 1993 15:19:10   DALLEE
// Increased size of the inoculation database index.
//************************************************************************

#ifndef _INOC_STUFF
#define _INOC_STUFF
#include "symcfg.h"


//************************************************************************
// INOCULATION file version numbers
//************************************************************************

#define INOC_VERSION_UNKNOWN        ( (WORD) 0 )
#define INOC_VERSION_30             ( (WORD) 1 )
#define INOC_VERSION_10_NLM         ( (WORD) 2 )


//************************************************************************
// DEFINES
//************************************************************************

#define SIZE_INDEX          16382
#define SIZE_FLOPPY_INDEX   512

enum {
     SMARTOPEN_RW = 1,
     SMARTOPEN_RO,
     SMARTOPEN_ERROR,
     SMARTOPEN_CLOSED
     };

//*** RETURN VALUES FOR IsInoculated()
// Not inoculated    == FALSE
// Inoculated        ==  TRUE
#define INOC_CHANGED        2


//************************************************************************
// EXTERNAL VARIABLES (and a #define for it)
//************************************************************************

extern BOOL bInocOpenOK;
#define INOC_OPEN_RW        2


// ************************************** //
// Defines for different INOCusion errors //
// ************************************** //

#define ERR_YOU_ARE_NUTS 1              // Error not implemented
#define ERR_INOC_ADD     2              // Adding to the Inoc database
#define ERR_INOC_CLOSE   3              // Closing database
#define ERR_INOC_OPEN    4              // Opening database
#define ERR_INOC_DEL     5              // Deleting from the Inoc database
#define ERR_INOC_INTERNAL 6             // Internal Error
#define ERR_INOC_PROTECT 7              // Couldn't obtain protect stuff


// ************************************** //

                                        // -----------------------------------
                                        // Callback for uninoculate.
                                        // Receives percent (0 - 100) done,
                                        // and lParam from UNINOCSTRUCT.
                                        // Return TRUE to continue, FALSE
                                        // to abort.
                                        // -----------------------------------

typedef BOOL (WINAPI * INOCALLBACK)(
    UINT    uPercent,                   // Percent completed
    DWORD   lParam                      // User defined hui
    );

typedef BOOL (WINAPI * UNINOCCALLBACK)(
    LPCSTR  lpszFileName,               // Name of the file to uninoculate
    BOOL    bUnInoculated,              // completion status for this file
    DWORD   dwUnInocCookie              // application defined value
    );

typedef struct tagUNINOCSTRUCT
{
    LPSTR           lpFileSpec;         // Filespec to uninoculate
    BOOL            bSubDirs;           // Include subdirectories
    INOCALLBACK     lpfn;               // Callback function
    DWORD           lParam;             // User defined hui
    UNINOCCALLBACK  lpfnUnInoc;         // Uninoc status callback function
    DWORD           dwUnInocCookie;     // application defined value

} UNINOCSTRUCT, FAR * LPUNINOCSTRUCT;
                                        // Values used are derived from the 
#define SIZE_INOCULATE_WORKMEM  2048    // larger of those used by Tasks.CPP
#define SIZE_INOCULATE_WORKMEM2  512    // and InocOpen.CPP

typedef struct tagINOCULATE
{
   HGLOBAL  hWorkMem;
   LPSTR    lpWorkMem;
   HGLOBAL  hWorkMem2;
   LPSTR    lpWorkMem2;

   HGLOBAL  hMem;
   HGLOBAL  hQuick;                     // 8/24/93 BarryG: Removed lpQuick
                                        // because hQuick is now kept unlocked
                                        // to help DOS memory management.
                                        // Should be OK for Win too but isn't
                                        // actually necessary.
                                        // Note also that hQuick ISN'T REQUIRED
                                        // so check for hQuick == 0 before using.

   LPDBC4_HEADER lpHeader;
} INOCULATE;
typedef INOCULATE FAR * LPINOCULATE;


// ------------------------------------------------------------------------- //


#define II_INUSE        1
#define II_ISOPEN       2
#define II_ISWRITE      4
#define II_LONG         8

typedef struct tagINOCInstances
{
    struct tagINOCInstances FAR *_ii_next;
    UINT                        _ii_flags;
    char                        _ii_drive;
    char                        _ii_UNCName[SYM_MAX_PATH];
    INOCULATE                   _ii_inoculate;
} INOCInstances, FAR * LPINOCINSTANCES;


// ***************************************************************** //
// Used by DOS component, so must be .C format.
// ***************************************************************** //

#ifdef __cplusplus
extern "C" {
#endif

VOID SYM_EXPORT WINAPI InocInit        (LPNAVOPTIONS);
VOID SYM_EXPORT WINAPI InocEnd         (VOID);
UINT SYM_EXPORT WINAPI SmartInocOpen    (LPSTR lpszFile, BOOL bReadWrite);
VOID SYM_EXPORT WINAPI SmartInocClose   (VOID);

BOOL  SYM_EXPORT WINAPI InocFile        (LPSTR);
UINT  SYM_EXPORT WINAPI IsInoculated    (LPSTR);


UINT SYM_EXPORT WINAPI SmartInocGetStatus(VOID);

VOID SYM_EXPORT WINAPI InoculateEnd     (VOID);
VOID SYM_EXPORT WINAPI InoculateInit    (VOID);

VOID  SYM_EXPORT WINAPI RememberInocFloppy (BOOL);
VOID  SYM_EXPORT WINAPI BuildInocDBName (LPSTR, BOOL bLongFile);
BOOL  SYM_EXPORT WINAPI InocOpen        (LPSTR, BOOL);
BOOL  SYM_EXPORT WINAPI InocClose       (VOID);
VOID  SYM_EXPORT WINAPI InocError       (UINT);
BOOL  SYM_EXPORT WINAPI InocDelFile     (LPSTR);
BOOL  SYM_EXPORT WINAPI InocRepairFile  (LPSTR);
LPINOCULATE SYM_EXPORT WINAPI GetInocStruct (LPSTR, BOOL bLongFile);
LPINOCULATE SYM_EXPORT WINAPI GetInocStructGivenFile(LPSTR);
LPINOCULATE SYM_EXPORT WINAPI GetUnusedInocStruct(LPSTR InocDB, BOOL bCreate, BOOL bLongFile);
VOID  SYM_EXPORT WINAPI ReleaseInocStruct   (LPINOCULATE);
DWORD SYM_EXPORT WINAPI InocGetNumEntries   (LPSTR, BOOL bLongFile);
BOOL  SYM_EXPORT WINAPI InocGetEntry        (LPSTR,
                                            DWORD,
                                            LPVOID,
                                            BOOL bLongFile);

UINT  WINAPI InocComp               (LPFILE_PROTECT, LPFILE_PROTECT);
UINT  WINAPI InocQuick              (LPFILE_PROTECT);
UINT  WINAPI InocHash               (LPFILE_PROTECT);

WORD WINAPI InocGetVersion ( LPINOCULATE lpInoculate );

VOID SYM_EXPORT WINAPI DoUnInocFiles (  // Do uninoc stuff
    LPUNINOCSTRUCT  lpUninoc
    );

#ifdef __cplusplus
}
#endif

// ***************************************************************** //

#endif

