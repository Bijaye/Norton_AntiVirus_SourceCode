// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/heurapi.h_v   1.10   08 Jun 1998 19:14:08   AOONWAL  $
//
// Description:
//
//  This file contains the prototypes for the heuristics functions as well as
//  PAMGHANDLE and PAMLHANDLE data structure definitions required for proper
//  PAM use.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/heurapi.h_v  $
// 
//    Rev 1.10   08 Jun 1998 19:14:08   AOONWAL
// Moved #include hcover.h towards the top
// 
//    Rev 1.9   08 Jun 1998 18:15:42   AOONWAL
// revert back to rev 1.7
// 
//    Rev 1.7   08 Jun 1998 16:39:08   CNACHEN
// Updated to define READ_ONLY_FILE and READ_WRITE_FILE if necessary.
// 
// 
//    Rev 1.6   08 Jun 1998 14:40:24   MKEATIN
// Reverted back to 1.4
// 
//    Rev 1.4   05 Jun 1998 16:57:40   MKEATIN
// include "hcover.h" at the end of the file to pick up PAM structer info
// 
//    Rev 1.3   09 Jul 1997 16:35:04   CNACHEN
// Added 3 config sets
// 
//    Rev 1.2   03 Jun 1997 17:42:14   CNACHEN
// 
//    Rev 1.4   29 May 1997 13:30:10   CNACHEN
// Added support for heuristic sensitivity level
// 
//    Rev 1.3   27 May 1997 14:07:22   CNACHEN
// Changed page.h to heurpage.h
// 
//    Rev 1.2   23 May 1997 18:13:52   CNACHEN
// Added variable to store tof for behavior checking.
// 
//    Rev 1.1   22 May 1997 15:06:52   CNACHEN
// Added support for new behaviors.
// 
//    Rev 1.0   14 May 1997 17:14:40   CNACHEN
// Initial revision.
// 
//    Rev 1.1   14 May 1997 16:57:32   CNACHEN
// Changed to use hproto.h
// 
// 
//    Rev 1.0   14 May 1997 16:49:50   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _HEURAPI_H
#define _HEURAPI_H

#include "avtypes.h"

#include "avendian.h"
#include "datafile.h"
#include "heurcpu.h"
#include "opcode.h"
#include "heurpage.h"
#include "sigex.h"

#include "callfake.h"
#include "hcover.h"

/* the PAMConfigType structure contains various configuration options */

#include "heurcfg.h"
#include "heurid.h"

// these headers have dependencies on the PAM config info, HLOCAL and HGLOBAL

#include "heur2.h"
#include "behav.h"
#include "banalyze.h"

/* the GlobalDataType structure contains all data which is not specific to
   a current scanning session.  IE the static data which is the same over all
   threads
*/

typedef struct
{
    PAMConfigType       config_info[MAX_CONFIG_LEVELS]; /* config options    */
    ExcludeContext      sig_exclude_info;               /* exclude info      */

    WORD                wHeurStreamLen;
    LPBYTE              lpbyHeurCode;

    BYTE                low_mem_area[LOW_MEM_SIZE];     /* read only swap area*/
} GlobalPAMType;

typedef GlobalPAMType FAR   *PAMGHANDLE;
typedef PAMGHANDLE FAR      *LPPAMGHANDLE;


// HLOCAL flags for proper operation

#define LOCAL_FLAG_FILE_ERROR               0x00000001
#define LOCAL_FLAG_MEM_ERROR                0x00000002
#define LOCAL_FLAG_OTHER_ERROR              0x00000004

/////////////////////////////////////////////////////////////////////////////
// Heuristics
/////////////////////////////////////////////////////////////////////////////

#define LOCAL_FLAG_PUSH_IN_PROGRESS         0x00000400

/////////////////////////////////////////////////////////////////////////////
// Heuristics
/////////////////////////////////////////////////////////////////////////////


#define LOCAL_FLAG_ERROR                    (LOCAL_FLAG_FILE_ERROR | \
                                             LOCAL_FLAG_MEM_ERROR |  \
                                             LOCAL_FLAG_OTHER_ERROR)

typedef struct
{
    PAMGHANDLE              hGPAM;

    /* CPU state for this thread */

    CPU_TYPE                CPU;

    /* page table information for this thread. */

    page_info_type          page_global;

    DWORD                   dwIteration;    // iteration count during exec

    DWORD                   dwFlags;        // exclusion and other flags
    
    EXEC_MANAGER_T          stEM;           // exec manager
    BEHAVE_MONITOR_T        stBM;           // Behavior monitor

    BYTE                    byVirusBuffers[MAX_ASSUMED_VIRUS_SIZE];
    BYTE                    byTOF[32];      // first 32 bytes of file

    BOOL                    bReset;         // should we discontinue our
                                            // current execution and
                                            // dequeue
    WORD                    wMOVSDelta;     // use to detect relocating
                                            // programs

    int                     nCurHeurLevel;  // current heuristic level

} LocalPAMType;


typedef LocalPAMType FAR    *PAMLHANDLE;
typedef PAMLHANDLE FAR      *LPPAMLHANDLE;


typedef unsigned int    PAMSTATUS;

#define PAMSTATUS_OK            0
#define PAMSTATUS_FILE_ERROR    1
#define PAMSTATUS_MEM_ERROR     2
#define PAMSTATUS_VIRUS_FOUND   3
#define PAMSTATUS_NO_SCAN       4
#define PAMSTATUS_NEED_TO_SCAN  5

/* pam protos */

#ifdef __cplusplus
extern "C" {
#endif

PAMSTATUS PAMGlobalInit(LPTSTR szDataFile,LPPAMGHANDLE hPtr);

PAMSTATUS PAMLocalInit(PAMGHANDLE hGPAM,
                       LPPAMLHANDLE hLPtr);

PAMSTATUS PAMLocalClose(PAMLHANDLE hLocal);

PAMSTATUS PAMGlobalClose(PAMGHANDLE hGlobal);

PAMSTATUS PAMScanFileH(PAMLHANDLE   hLocal,
                       HFILE        hFile,
                       int          nHeuristicLevel,    // 1 to 10
                       LPBOOL       lpbVirusFound,
                       LPWORD       lpwVirusID);

#ifdef __cplusplus
}
#endif


#include "hproto.h"

#ifndef READ_WRITE_FILE
#define READ_WRITE_FILE 2
#endif

#ifndef READ_ONLY_FILE
#define READ_ONLY_FILE  0
#endif

#endif
