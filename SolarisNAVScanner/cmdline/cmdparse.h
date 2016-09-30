// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/cmdparse.h_v   1.3   12 Jun 1996 12:10:50   RAY  $
//
// Description:
//  Prototypes for command line parsing functions.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/cmdparse.h_v  $
// 
//    Rev 1.3   12 Jun 1996 12:10:50   RAY
// Added /NOFILE switch to skip file scanning
// 
//    Rev 1.2   05 Jun 1996 16:42:20   CNACHEN
// Added a def dump field.
// 
//    Rev 1.1   04 Jun 1996 18:18:16   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.0   21 May 1996 12:26:28   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _CMDPARSE_H

#define _CMDPARSE_H

#include "avtypes.h"

typedef struct scan_node_tag
{
	LPSTR                   		lpszScanThis;
	struct	scan_node_tag   FAR     *lpNext;
} SCAN_NODE_T, FAR *LPSCAN_NODE;


#define     OPTION_FLAG_REPORT                  0x00000001UL
#define     OPTION_FLAG_DOALL                   0x00000002UL
#define     OPTION_FLAG_HELP                    0x00000004UL
#define     OPTION_FLAG_NOBOOT                  0x00000008UL
#define     OPTION_FLAG_NOPART                  0x00000010UL
#define     OPTION_FLAG_NOMEM                   0x00000020UL
#define     OPTION_FLAG_REPAIR                  0x00000040UL
#define     OPTION_FLAG_DELETE                  0x00000080UL
#define     OPTION_FLAG_RENAME                  0x00000100UL
#define     OPTION_FLAG_SCANHIGH                0x00000200UL
#define     OPTION_FLAG_RECURSE                 0x00000400UL
#define     OPTION_FLAG_NOPOLY                  0x00000800UL
#define     OPTION_FLAG_SCANWILD                0x00001000UL
#define     OPTION_FLAG_REMVIR                  0x00002000UL
#define     OPTION_FLAG_VIRLIST                 0x00004000UL
#define     OPTION_FLAG_MEMAVAIL                0x00008000UL
#define     OPTION_FLAG_WATCH                   0x00010000UL
#define     OPTION_FLAG_DUMP                    0x00020000UL
#define     OPTION_FLAG_NOFILE                  0x00040000UL

typedef struct
{
    char                    szReportFile[256];
    WORD                    wRemVirVID;
    WORD                    wDebugVID;
    WORD                    wDumpVID;
    DWORD                   dwFlags;
    LPSCAN_NODE             lpScanList;
} CMD_OPTIONS_T, FAR *LPCMD_OPTIONS;

#define FIRST_COMMAND_LINE_OBJECT               1

#define OPTION_CHAR1                            '-'
#define OPTION_CHAR2                            '/'


typedef struct
{
	LPSTR           lpszOption;
	DWORD           dwID;
} COPTS_T;

typedef unsigned int CMDSTATUS;

#define CMDSTATUS_OK                            0
#define CMDSTATUS_MEM_ERROR                     1
#define CMDSTATUS_INVALID_OPTION                2

// prototypes

void FreeCommandLineData
(
    LPCMD_OPTIONS  lpCommandOptions
);

CMDSTATUS CommandLineParse
(
    int                 argc,
    char                *argv[],
    LPCMD_OPTIONS       lpCommandOptions,
    LPWORD              lpwInvalidOption
);

void PrintOptions
(
    LPCMD_OPTIONS       lpCommandOptions
);

#endif
