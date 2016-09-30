// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1994, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************
//
// $Header:   S:/include/VCS/navstart.h_v   1.0   06 Feb 1997 20:51:54   RFULLER  $
//
// Description:
//      Defines the header for NAVSTART.DAT - This file contains the startup
//      options and list of DOS and Windows system files to scan.
//
// Contains:
//
// See Also:
//      navopts.h   navbnhdr.h  options.h
//************************************************************************
// $Log:   S:/include/VCS/navstart.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:54   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:46:42   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:02   BARRY
// Initial revision.
// 
//    Rev 1.5   21 Sep 1995 13:16:56   YVANG
// Added an entry, 'bProtectFeature' inside the STARTOPT structure.  This entry
// will hold the password protect feature value for the 'View Startup Settings'
// option on the 'Password' control panel.
// 
// 
//    Rev 1.4   28 Feb 1995 11:24:58   DALLEE
// Routines to read/write lists of system files in NAVSTART.DAT.
// Format of NAVSTART.DAT slightly modified (okay since we weren't yet
// writing the list of system files).
// 
//    Rev 1.3   23 Jan 1995 15:39:56   DALLEE
// Copied archive from NAVBOOT work area and renamed to NAVSTART.H.
// 
//    Rev 1.2   21 Dec 1994 15:09:58   DALLEE
// Updated default header to include sizeof header.
// 
//    Rev 1.1   19 Dec 1994 13:36:26   DALLEE
// prototype for NAVSTART.DAT routines.
// 
//    Rev 1.0   16 Dec 1994 16:04:20   DALLEE
// Initial revision.
//************************************************************************

#ifndef __NAVSTART_H
#define __NAVSTART_H

/*
Structure of NAVSTART.DAT file:

    NAVSTARTHEADER                  64 bytes
    NAVSTARTBINDATA                 64 bytes (STARTOPT + filler)

        // Static file list with substitution codes.
    NAVSTARTBLOCKHEADER             12
      {
      FILELIST                      8
      SZZ Static list of sys files  FILELIST.dwSizeEntries
      }

        // Substitution list.  Directories to plug in for codes.
    NAVSTARTBLOCKHEADER             12
      {
      FILELIST                      8
      SZZ Substitution list.        FILELIST.dwSizeEntries
      }

        // Modified list with substitutions made for directories.
    NAVSTARTBLOCKHEADER             12
      {
      FILELIST                      8
      SZZ This machine's sys files  FILELIST.dwSizeEntries
      }
*/

                                        //-------------------------------
                                        // Defines for dwVersion field.
                                        //-------------------------------
// NAVSTART_VERSION for NAV for Chicago = 2
#define NAVSTART_VERSION          2


typedef struct tagNAVSTARTHEADER
    {
    char        szSignature[15];        // Signature "NAVSTART.DAT",^Z,00,00
    DWORD       dwVersion;              // Version number.
    DWORD       dwSize;                 // sizeof NAVSTARTHEADER.
    BYTE        byFiller[64-15-8];      // Fill with '\0' and bake 'til crisp.
    } NAVSTARTHEADER, FAR *LPNAVSTARTHEADER;

#define NAVSTART_SIGNATURE  "NAVSTART.DAT\x1A"

                                        // Default header for NAVSTART.DAT
#define DEFAULT_NAVSTART_HEADER  \
    { NAVSTART_SIGNATURE,        \
      NAVSTART_VERSION,          \
      sizeof(NAVSTARTHEADER),    \
      0 }

                                        //&? Move to NAVOPTS.H?
                                        // Startup CPL
                                        //  stored in NAVSTART.DAT
typedef struct tagSTARTOPT
{
    BYTE        bScanDOSFiles;
    BYTE        bScanWindowsFiles;
    BYTE        bLoadTSR;
    BYTE        bLoadVxD;
    BYTE        bProtectFeature;        // Password protect feature for
                                        // 'View Startup Setting'. Added into
                                        // this structure because of the 
                                        // fixed PASWOPT structure size.
} STARTOPT, *PSTARTOPT, FAR *LPSTARTOPT;

                                        // Allow filler in NAVSTART.DAT file
                                        // for extra options later.
typedef struct tagNAVSTARTBINDATA
{
    STARTOPT    startup;
    BYTE        byFiller [64 - sizeof(STARTOPT)];
} NAVSTARTBINDATA, *PNAVSTARTBINDATA, FAR *LPNAVSTARTBINDATA;

                                        // Header for variable size blocks
                                        // following binary data.
#define NAVSTARTBLOCKID         ((DWORD)0x4C42534E)     // "NSBL"

typedef struct tagNAVSTARTBLOCKHEADER
{
    DWORD       dwID;                   // Always NAVSTARTBLOCKID
    DWORD       dwBlockType;            // Type info stored in block.
    DWORD       dwSize;                 // Size of following block data.
    // Free form data follows
} NAVSTARTBLOCKHEADER, *PNAVSTARTBLOCKHEADER, FAR *LPNAVSTARTBLOCKHEADER;


// The FILELIST structure keeps track of the list of system files to scan.
// The structure will be immediately followed by an SZZ of entries.
// There are three different lists with the following block type codes:
//      1. STATIC LIST - This is the list created when we ship and is
//              never modified.  It contains our list of "system files"
//              with substitution codes for where to insert windows dir,
//              system dir, etc. for a specific machine.
//      2. SUBSTITUTION LIST -  This contains the list of directories we
//              will use once installed on a given machine.  This list is
//              used to quickly check if we need to update NAVSTART.DAT.
//      3. MODIFIED LIST - This is the list of files to scan as they
//              actually appear on the machine where NAV is installed.

typedef struct tagFILELIST
{
    DWORD       dwNumEntries;       // Number of file list entries.
    DWORD       dwSizeEntries;      // Size of file list from 1st byte of SZZ
                                    // to 2nd EOS inclusive.
} FILELIST, *PFILELIST, FAR *LPFILELIST;

// defines for NAVSTARTBLOCKHEADER.dwBlockType field.
#define FILELIST_STATIC         ((DWORD)0x4C465453)     // "STFL"
#define FILELIST_SUBSTITUTION   ((DWORD)0x4C464253)     // "SBFL"
#define FILELIST_MODIFIED       ((DWORD)0x4C46444D)     // "MDFL"


// FILELIST_SUBSTITUTION information:

#define MAX_SUBSTITUTIONS   3

#define SUB_ROOT_FOLDER     0           // Root folder of boot drive.
#define SUB_WINDOWS_FOLDER  1           // Windows folder.
#define SUB_SYSTEM_FOLDER   2           // Windows system folder.

                                        // Substitution character codes.
#define SUBSTITUTION_CHAR   '<'         // Signals substitution for system file.
#define SUB_ROOT_CHAR       'R'
#define SUB_WINDOWS_CHAR    'W'
#define SUB_SYSTEM_CHAR     'S'


#ifdef __cplusplus
extern "C" {
#endif

// startup.cpp in NAVXUTIL

STATUS SYM_EXPORT WINAPI ReadStartupFileList ( DWORD        dwListID,
                                               LPHGLOBAL    lphFileList,
                                               LPDWORD      lpdwNumEntries,
                                               LPDWORD      lpdwSizeEntries );

STATUS SYM_EXPORT WINAPI WriteStartupFileList ( DWORD       dwListID,
                                                LPSTR       lpszzEntries,
                                                DWORD       dwNumEntries,
                                                DWORD       dwSizeEntries );


BOOL SYM_EXPORT WINAPI NeedToUpdateStartupFileLists (VOID);
STATUS SYM_EXPORT WINAPI UpdateStartupFileLists (VOID);

#ifdef __cplusplus
}
#endif

#endif  // __NAVSTART_H

