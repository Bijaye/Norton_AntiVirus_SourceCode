// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/datafile.h_v   1.5   20 Nov 1996 15:21:18   AOONWAL  $
//
// Description:
//
//  This source file contains data structures and constants used in PAM
//  data file handling.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/datafile.h_v  $
// 
//    Rev 1.5   20 Nov 1996 15:21:18   AOONWAL
// No change.
// 
//    Rev 1.4   22 Oct 1996 12:11:00   AOONWAL
// No change.
// 
//    Rev 1.3   06 Sep 1996 14:01:18   CNACHEN
// Added proper #pragma pack() commands for NTK/WIN32 platforms.
// 
//    Rev 1.2   29 May 1996 17:28:54   CNACHEN
// Changed data file signature to "SYMC"...
// 
//    Rev 1.1   20 Feb 1996 11:27:24   CNACHEN
// Changed all LPSTRs to LPTSTRs.
// 
//    Rev 1.0   01 Feb 1996 10:17:38   CNACHEN
// Initial revision.
// 
//    Rev 1.4   23 Oct 1995 12:51:28   CNACHEN
// Added #pragma pack(1) directives around all structure definitions
// 
//    Rev 1.3   19 Oct 1995 18:39:14   CNACHEN
// Initial revision... With comment headers... :)
//************************************************************************



typedef unsigned int DFSTATUS;

#define DFSTATUS_OK             0
#define DFSTATUS_FILE_ERROR     1
#define DFSTATUS_MEM_ERROR      2
#define DFSTATUS_INVALID_FORMAT 3
#define DFSTATUS_NO_ENTRY_FOUND 4


#define SIG_VALUE               0x434D5953UL

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif


typedef struct
{
    DWORD       dwSignature;                /* signature for data file */
    DWORD       dwVersionNumber;
    BYTE        byMon, byDay, byYear;       /* last revision */
    BYTE        byFiller;
    WORD        wNumSections;
    DWORD       dwTableOffset;
    DWORD       dwCRC;                      /* integrity CRC */
    BYTE        byPadding[10];             /* pad to 32 bytes */
} HeaderType;

typedef struct
{
    DWORD       dwIdent;
    DWORD       dwVersionNumber;
    DWORD       dwStartOffset;
    DWORD       dwLength;
    BYTE        byMon, byDay, byYear;       /* last revision */
} TableType;

#define sTABLETYPE  sizeof(TableType)

#define sHEADERTYPE     sizeof(HeaderType)


typedef struct
{
    HFILE       hHandle;
    HeaderType  sHeader;
} DataFileHandleType;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif


typedef DataFileHandleType *DATAFILEHANDLE;

/* protos */

DFSTATUS DFOpenDataFile(LPTSTR lpDataFile,
                        UINT uMode,
                        DATAFILEHANDLE *lphInfo);

HFILE DFGetHandle(DATAFILEHANDLE hInfo);

DFSTATUS DFLookUp(DATAFILEHANDLE hInfo,
                  DWORD dwIdent,
                  DWORD *lpdwVersionNumber,
                  DWORD *lpdwStartOffset,
				  DWORD *lpdwLength,
                  BYTE  *lpbyMon, BYTE *lpbyDay, BYTE *lpbyYear);

void DFCloseDataFile(DATAFILEHANDLE hInfo);


