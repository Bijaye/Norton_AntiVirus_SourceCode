// Copyright 1993 Symantec, Peter Norton Group
////////////////////////////////////////////////////////////////////////  
// SYMINTEG.H
//   
// $Header:
//   
// Description:
//   
//   
//   
////////////////////////////////////////////////////////////////////////  
// $Log:   S:/INCLUDE/VCS/syminteg.h_v  $
// 
//    Rev 1.17   12 Sep 1996 12:53:10   JBRENNA
// Update the IntegAddEntry() prototype.
// 
//    Rev 1.16   20 Jun 1995 10:24:16   BARRY
// Added wFlags to VerifyEntry and AddEntry to tell when to ignore volume info
// 
//    Rev 1.15   07 Apr 1995 06:43:32   SZIADEH
// increased szID size to 32 bytes.
// 
//    Rev 1.14   30 Jan 1995 19:57:44   MFALLEN
// Function prototypes that used some kinda file handle were completelly wrong.
// The file handles were being defined as WORD or UINT and this is causing
// problems when checking for conditions such as HFILE_ERROR. This has been
// fixed and all TAB's that somebody put it were removed.
// 
//    Rev 1.13   12 Dec 1994 20:13:50   MFALLEN
// Fixed W32 compile errors.
// 
//    Rev 1.12   01 Dec 1994 11:52:24   MFALLEN
// Changed BYTE arrays to char arrays.
// 
//    Rev 1.11   01 Dec 1994 11:02:16   SZIADEH
// modified code to work under WIN95.
// 
//    Rev 1.5.3.0   19 Oct 1993 11:26:08   SCOTTP
// Branch base for version QAK2
// 
//    Rev 1.5   16 Aug 1993 21:32:34   MFALLEN
// Semicolon missing
// 
//    Rev 1.4   16 Aug 1993 21:05:54   CGUC
// Added signature check
// 
//    Rev 1.3   22 Jul 1993 22:07:58   BRADR
// Added INTEG_INVALID_BOOT define.
// 
//    Rev 1.2   20 Jul 1993 08:48:36   CGUC
// Added wAttr to IntegRecord.
// 
//    Rev 1.1   14 Jun 1993 18:45:00   CGUC
// Added IntegSelfCheck.
// 
//    Rev 1.0   09 Jun 1993 03:34:10   CGUC
// Initial revision.


#ifndef _SYMINTEG_H
#define _SYMINTEG_H


//************************************************
#ifndef MAX_PATH_SIZE
#define MAX_PATH_SIZE 144
#endif
#define ID_SIZE   32

typedef struct {
    WORD    wStatus;
    char    byFilename[SYM_MAX_PATH];
} INTEG, FAR *LPINTEG;
#define sINTEG sizeof(INTEG)

typedef struct {
    char    lpName[128];
    char    szID[ID_SIZE];
    WORD    wEntries;
    DWORD   dwStartEntry;
    char    byReserved[256-4-2-128-16];
    } INTEGHEADER;
#define sINTEGHEADER sizeof(INTEGHEADER)

typedef struct {
    char    lpName[SYM_MAX_PATH];
    DWORD   dwSize;
    DWORD   dwNextEntry;
    WORD    wCRC;
    WORD    wDate;
    WORD    wTime;
    WORD    wAttr;
    char    byReserved[256- MAX_PATH_SIZE-4-4-2-2-2-2];
    } INTEGRECORD;
#define sINTEGRECORD sizeof(INTEGRECORD)

#define InDosDbaseInWindows     1
#define InWindowsDbaseInDos     2
#define InDosDbaseInDos         3
#define InWindowsDbaseInWindows 4

#define INTEG_SUCCESS_STORE  10
#define INTEG_FAIL_GET       11
#define INTEG_FAIL_STORE     12
#define INTEG_FAIL_VERIFY    13
#define INTEG_SUCCESS_VERIFY 14
#define INTEG_FAIL_REPAIR    15
#define INTEG_SUCCESS_REPAIR 16
#define INTEG_INVALID_BOOT   17
#define INTEG_FILE_NOT_FOUND 18
#define INTEG_UNKOWN_SYSTEM_STATE 19

#define INTEG_FLAG_NONE      0x0000
#define INTEG_FLAG_SKIP_VOL  0x0001     // Ignore volume label on boot records

//this is the common set
#define szIoSys      "C:\\IO.SYS"
#define szMsdosSys   "C:\\MSDOS.SYS"
#define szIbmbioSys  "C:\\IBMBIO.SYS"
#define szIbmdosSys  "C:\\IBMDOS.SYS"
#define szCommandCom "C:\\COMMAND.COM"

// this set of files is found when Windows is active
#define szIoDos         "C:\\IO.DOS"
#define szMsdosDos      "C:\\MSDOS.DOS"
#define szIbmbioDos     "C:\\IBMBIO.DOS"
#define szIbmdosDos     "C:\\IBMDOS.DOS"
#define szCommandDos    "C:\\COMMAND.DOS"

// this set of files is found when DOS is active
#define szWinbootSys    "C:\\WINBOOT.SYS"
#define szMsdosW40      "C:\\MSDOS.W40"
#define szCommandW40    "C:\\COMMAND.W40"
#define szSkipThisFile  "Skip This File"



#ifdef __cplusplus
extern "C" {
#endif

//  ********************************************************************** //
//  *****************      Function Prototypes     *********************** //
//  ********************************************************************** //

WORD SYM_EXPORT WINAPI IntegSelfTest (
    LPSTR   lpFilename,
    BYTE    bFlag,
    LPSTR   lpBuffer
    );

WORD SYM_EXPORT WINAPI IntegCreate (
    LPSTR   lpStore,
    LPINTEG lpArray
    );

WORD SYM_EXPORT WINAPI IntegGetSystemNames (
    LPINTEG lpArray
    );

WORD SYM_EXPORT WINAPI IntegVerifyEntry (
    HFILE   hFile,
    HFILE   hFile2,
    LPSTR   lpName,
    LPSTR   lpBuffer,
    WORD    uBuffer,
    WORD    wFlags
    );

WORD SYM_EXPORT WINAPI IntegVerify (
    LPSTR   lpStore,
    LPINTEG lpArray
    );

WORD SYM_EXPORT WINAPI IntegRepairEntry (
    HFILE   hFile,
    HFILE   hFile2,
    LPSTR   lpName,
    LPSTR   lpBuffer,
    WORD    uBuffer,
    WORD    wFlags
    );

WORD SYM_EXPORT WINAPI IntegRepair (
    LPSTR   lpStore,
    LPINTEG lpArray
    );

WORD SYM_EXPORT WINAPI GetCurrentSystemFiles (
    HFILE   hFile
    );

WORD SYM_EXPORT WINAPI MapSystemFiles (
    WORD    FilesToUse,
    LPSTR   lpBuffer
    );

BOOL SYM_EXPORT WINAPI IntegVerifyDatabaseMatch (
    LPSTR   lpDatabase
    );

BOOL SYM_EXPORT WINAPI GetIntegId (
    LPSTR   lpStoreHere
    );

BOOL SYM_EXPORT WINAPI CompareBytes (
    LPSTR   lpSource1,
    LPSTR   lpSource2,
    WORD    wLength
    );

BOOL SYM_EXPORT WINAPI IntegAddEntry (  // Adds entry to integ database
    HFILE   hFile,                      // [in] handle to database to write to
    HFILE   hFile2,                     // [in] handle to read file
    LPSTR   lpName,                     // [in] name of file being added
    LPSTR   lpOrigName,                 // [in] original version of "lpName"
    LPSTR   lpBuffer,                   // [in] buffer to use to write
    WORD    uBuffer ,                   // [in] size of buffer
    WORD    wFlags );                    // [in] INTEG_FLAG_* (syminteg.h)

#if defined( __cplusplus )
}
#endif  // #if defined( __cplusplus )
#endif

