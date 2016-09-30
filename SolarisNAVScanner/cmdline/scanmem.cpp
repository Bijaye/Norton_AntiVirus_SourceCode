// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/scanmem.cpv   1.1   28 May 1996 14:35:28   DCHI  $
//
// Description:
//  Scan memory function.
//
// Contains:
//  CMDLineScanMemory()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/scanmem.cpv  $
// 
//    Rev 1.1   28 May 1996 14:35:28   DCHI
// Added support for memory objects.
// 
//    Rev 1.0   21 May 1996 12:25:44   DCHI
// Initial revision.
// 
//************************************************************************

#include "avenge.h"

#include "scanmem.h"
#include "clcb.h"

#include "report.h"
#include "msgstr.h"

//*************************************************************************
//
// Function:
//  CMDLineScanMemory()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//
// Description:
//  Loads memory scanning signatures, scans memory, and then unloads
//  memory scanning signatures
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL CMDLineScanMemory
(
    LPSCANGLOBAL    lpScanGlobal
)
{
    DWORD           dwEndAddress;
    BOOL            bFoundVirus;
    HVIRUS          hVirus;
    MEMORYOBJECT_T  stMemoryObject;

    stMemoryObject.lpCallBacks = &gstCLCBMemoryCallBacks;

    // First initialize scanning data

    if (EngGlobalMemoryInit(lpScanGlobal->hGEng) == ENGSTATUS_ERROR)
    {
        ReportError(lpScanGlobal,
                    gszMsgStrScanMemoryInit,
                    REPORT_ERROR_INIT);

        return(FALSE);
    }

    if (lpScanGlobal->lpCmdOptions->dwFlags & OPTION_FLAG_SCANHIGH)
        dwEndAddress = 0x100000ul;
    else
        dwEndAddress = 0xA0000ul;

    EngScanMemory(lpScanGlobal->hLEng,
                  &stMemoryObject,
                  0,
                  dwEndAddress,
                  LENG_SCAN_TOP_OF_MEMORY,
                  0,
                  &bFoundVirus,
                  &hVirus);

    // Now close scanning data

    if (EngGlobalMemoryClose(lpScanGlobal->hGEng) == ENGSTATUS_ERROR)
    {
        ReportError(lpScanGlobal,
                    gszMsgStrScanMemoryClose,
                    REPORT_ERROR_CLOSE);

        return(FALSE);
    }

    if (bFoundVirus == TRUE)
    {
        ReportVirusInMemory(lpScanGlobal,hVirus);
        return(FALSE);
    }

    return(TRUE);
}


