// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/scanglob.h_v   1.2   27 Aug 1996 15:12:48   DCHI  $
//
// Description:
//  Command line global structure definition.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/scanglob.h_v  $
// 
//    Rev 1.2   27 Aug 1996 15:12:48   DCHI
// Addition of fields for UNIX NAVEX integration.
// 
//    Rev 1.1   03 Jun 1996 17:44:38   DCHI
// Modifications for using difftime to calculate time used.
// 
//    Rev 1.0   21 May 1996 12:28:00   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _SCANGLOB_H

#define _SCANGLOB_H

#include <time.h>

#include "avenge.h"
#ifdef SYM_UNIX
#include "pamapi.h"
#endif

#include "cmdparse.h"

#include "fileinfo.h"

#ifdef SYM_UNIX
#include "callback.h"
#include "navex.h"
#endif

#define NO_EVENT                        0x00000000ul
#define EVENT_VIRUS_FOUND_MEMORY        0x00000001ul
#define EVENT_VIRUS_FOUND_BOOT          0x00000002ul
#define EVENT_VIRUS_FOUND_MBR           0x00000004ul
#define EVENT_VIRUS_FOUND_FILE          0x00000008ul
#define EVENT_ERROR                     0x00000010ul
#define EVENT_MODIFIED_EXECUTABLE       0x00000020ul

#define SCANGLOB_WORK_BUF_SIZE          4096

typedef struct
{
    LPCMD_OPTIONS       lpCmdOptions;

    HLENG               hLEng;
    HGENG               hGEng;

#ifdef SYM_UNIX
    PAMGHANDLE          hGPAM;
    PAMLHANDLE          hLPAM;
#endif

    DWORD				dwFilesInfected;
    DWORD				dwFilesScanned;

    time_t              tStartTime;
    time_t              tEndTime;

    FILEINFO_T          stReportFileInfo;
    FILEINFO_T          stFileInfo;
    FILEOBJECT_T        stFileObject;

	long				lMutex;

    char                szInitialDir[SYM_MAX_PATH+1];
    char                szHomeDir[SYM_MAX_PATH+1];
    char                szLastCWDOnScannedVol[SYM_MAX_PATH+1];

    DWORD               dwEventFlags;

    BYTE                byWorkBuffer[4096];

#ifdef SYM_UNIX
    void *              hNAVEXSharedObject;
    LPCALLBACKREV1      lpNAVEXCallBacks;
    EXPORT_TABLE_TYPE   stNAVEXFunctionTable;
#endif // SYM_UNIX

} SCANGLOBAL_T, FAR *LPSCANGLOBAL;

extern SCANGLOBAL_T    gstScanGlobal;


#endif  // _SCANGLOB_H

