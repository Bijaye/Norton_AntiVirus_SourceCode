// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRSCAN/VCS/scn_mem.cpv   1.23   07 Aug 1996 10:38:34   MKEATIN  $
//
// Description:
//      Scan memory for viruses.
//
// Contains:
//      VirScanMemory()
//
// See Also:
//      SCN_INIT.C for VirScanInit()
//************************************************************************
// $Log:   S:/VIRSCAN/VCS/scn_mem.cpv  $
// 
//    Rev 1.23   07 Aug 1996 10:38:34   MKEATIN
// Ported code from Virlib 8
// 
//    Rev 1.22   28 Jun 1996 11:42:54   JBRENNA
// Merge LuigiPlus into Gryphon:
//     MKeatin - Now, pass lpgCallBack to VLScanInit so it doesn't get
//     changed.
// 
//    Rev 1.21   06 Feb 1996 21:24:32   BARRY
// Fix LPSTR to LPBYTE problems due to Unicode effort
// 
//    Rev 1.20   12 Jan 1996 07:11:44   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.19.1.5   14 Nov 1995 18:29:26   MKEATIN
// Added NavExInit() to VirusScanMemory for NAVBOOT
// 
//    Rev 1.19.1.4   13 Oct 1995 14:38:14   DCHI
// Changed avex.h to navex.h.
// 
//    Rev 1.19.1.3   12 Oct 1995 14:18:20   DCHI
// EXTScanMemory takes bScanHighMemory parameter.
// 
//    Rev 1.19.1.2   09 Oct 1995 12:41:14   DCHI
// Use MEMCPY instead of STRCPY to copy virus name.
// 
//    Rev 1.19.1.1   07 Sep 1995 13:49:24   CNACHEN
// Added external virus detection code.
// 
//    Rev 1.19.1.0   12 Jul 1995 18:59:52   MPYE
// Branch base for version QAK8
// 
//    Rev 1.19   17 May 1995 11:29:34   SZIADEH
// call get infection data whenever a virus in memory is found. The 
// function itslef checks whether it's one_half.
// 
//    Rev 1.18   16 May 1995 11:25:08   SZIADEH
// if one_half virus is in memory, write out repiar info. Repair info 
// will only be written out in DOS mode.
// 
//    Rev 1.17   08 Feb 1995 13:02:26   SZIADEH
// - Copy virus name in MapHvirusToLpnotes()
// - NULL out lpInSCanMBuffer prior to calling VLScanClose() in the case
//   of no virus.
// 
//    Rev 1.16   07 Feb 1995 20:58:58   SZIADEH
// removed the check on InitCertlib() return value.
// 
//    Rev 1.15   01 Feb 1995 16:55:30   MFALLEN
// Added default flag to InitCertlib() function.
// 
//    Rev 1.14   13 Jan 1995 11:00:26   SZIADEH
// removed the call to VLRegisterVirusDB. Doctored up an hContext.
// 
//    Rev 1.13   29 Dec 1994 11:25:42   SZIADEH
// added error checking after registering virus dbase.
// 
//    Rev 1.12   27 Dec 1994 19:06:00   DALLEE
// Use CERTLIBN for DOS platform now, too.
// 
//    Rev 1.11   19 Dec 1994 22:05:18   DALLEE
// Convert .WIN to CERTLIBN
// 
//    Rev 1.10   19 Dec 1994 13:06:20   SZIADEH
// changed lphVirus to hVirus, 
// registered database files,
// rleased hVirus before closing.
// 
//    Rev 1.9   18 Dec 1994 20:10:56   SZIADEH
// modified the code so that AVAPI VLScanMemory is called to perform 
// the memory scan.
// 
//    Rev 1.7   30 Nov 1994 12:39:10   DALLEE
// Use CTS4.H for SYM_DOS, too.
// 
//    Rev 1.6   29 Nov 1994 15:02:30   MFALLEN
// wControl -> wControl2
// 
//    Rev 1.5   28 Nov 1994 15:29:18   MFALLEN
// include sts4.h if W16 platform.
// 
//    Rev 1.4   12 May 1994 11:04:04   MFALLEN
// cts4 2 ctsn
// 
//    Rev 1.2   02 May 1994 12:52:54   BRAD
// Changed _loadds to SYM_EXPORT
// 
//    Rev 1.1   22 Apr 1994 18:16:04   MFALLEN
// Changed _loadds to SYM_EXPORT. If the TSR will need _loadds then this must
// be a macro. Without this change the LINK32 will not generate an import lib.
// 
//    Rev 1.0   22 Apr 1994 10:59:04   MFALLEN
// Initial revision.
// 
//    Rev 1.16   15 Mar 1994 16:52:56   BRAD
// Cleaned up for SYM_WIN32
// 
//    Rev 1.15   15 Mar 1994 11:09:40   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.14   02 Mar 1994 17:12:32   BRAD
// AWK script applied
// 
//    Rev 1.13   11 Feb 1994 15:55:04   MARKL
// Quake 2 changes moved to trunk.
// 
//    Rev 1.12.1.0   19 Oct 1993 08:56:22   SCOTTP
// Branch base for version QAK2
// 
//    Rev 1.12   14 Oct 1993 13:24:28   MFALLEN
// #ifndef SYM_WIN-ed all the FileCheckError since this function can't be called
// from a dll outside SYMKRNL. The function GP's in Windows since the DS is
// from the calling program. How did this work before ???
// 
//    Rev 1.11   12 Oct 1993 16:42:54   PATRICKM
// Baseline from QAK1 branch
// 
//    Rev 1.10.2.3   23 Sep 1993 16:15:26   keith
// Stub functions not yet supported for OS/2 Mirrors
// 
//    Rev 1.10.2.2   12 Sep 1993 01:32:34   MFALLEN
// Removed N30NOTES.bFlag since it's not used anymore.
// 
//    Rev 1.10.2.1   07 Sep 1993 02:06:48   DALLEE
// Placed calls to FileCheckError after CERTLIB4 calls in order to clear
// the critical error handler (DOS) and set the _error variable.
// 
//    Rev 1.10.2.0   03 Sep 1993 22:52:48   SCOTTP
// Branch base for version QAK1
// 
//    Rev 1.10   03 Aug 1993 13:33:54   MFALLEN
// We were verifying the virus name from the BootScanData buffer instead of
// the memory scan data buffer.
// 
//    Rev 1.9   18 Jun 1993 00:58:26   DALLEE
// Return the Signiture in Memory Index value and set N30NOTES.wVirID
// to the VIRSCAN.INF lookup index value.
// 
//    Rev 1.8   10 Jun 1993 02:51:14   CGUC
// Changed SYM_EXPORT to _loadds
// 
//    Rev 1.7   09 Jun 1993 14:41:36   MFALLEN
// Memory scan function now takes BOOL instad of BYTE
// 
//    Rev 1.6   03 Jun 1993 04:27:34   BRAD
// Added high memory parameter to VIrusScanMemory().
// 
//    Rev 1.5   28 Apr 1993 09:10:26   MFALLEN
// 
//    Rev 1.4   22 Apr 1993 10:39:44   MFALLEN
// N30Notes
// 
//    Rev 1.3   04 Apr 1993 17:39:14   BRAD
// Removed all references to N30.*
// 
//    Rev 1.2   26 Mar 1993 14:53:40   DALLEE
// Support for DOS.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"

#include "ctsn.h"
#include "avapi.h"
#include "virscan.h"
#include "callback.h"
#include "navex.h"
#include "avexprot.h"


//========================================================================
// local functions prototypes
//========================================================================

UINT LOCAL PASCAL MapHvirusToLpnotes (
     HVIRUS      hVirus,
     LPN30NOTES  lpNotes );

//========================================================================
// externals
//========================================================================
extern VIRSCAN ScanData;

#ifdef __cplusplus
extern "C" {
#endif

extern LPCALLBACKREV1 lpgCallBack;

#ifdef __cplusplus
}
#endif

//************************************************************************
// VirusScanMemory()
//
// Scans either the first 640K or the first 1 Meg. of memory.
//
// Parameters:
//  lpVirusName; 40 byte minimum buffer size where to place
//  virus name.
//
// Returns:
//  uVirusID;       0 == not infected
//           non-zero == memory infected
//========================================================================
UINT SYM_EXPORT WINAPI VirusScanMemory (
     LPN30NOTES lpNotes,
     BOOL bScanHighMemory)
{
    auto    WORD        wFlags;
    auto    HVIRUS      hVirus;
    auto    HVCONTEXT   hContext;
    auto    VSTATUS     vsMemScanResult;
    auto    UINT        uResult;
    auto    LPBYTE      lpBuffer;

    auto    EXTSTATUS   extResult;
    auto    WORD        wVirusID, wVirusIndex;
    auto    N30         stN30;

    hContext = VLScanInit(1, lpgCallBack, 0);

    if (NULL == hContext)
        {
        return (FALSE);
        }

    ///////////////////////////////////////////////////////////////
    // doctor up hContext
    ///////////////////////////////////////////////////////////////

    hContext->lpInScanMBuffer = (LPBYTE)ScanData.lpMemoryScanData;
    STRCPY(hContext->szVirScanDataFile, ScanData.szVirscanFile);

    ///////////////////////////////////////////////////////////////

    wFlags = (bScanHighMemory ? 1 : 0);

    vsMemScanResult = VLScanMemory(hContext, wFlags, &hVirus);

    if ( (VS_OK != vsMemScanResult) || (NULL == hVirus) )
        {
                                        // memory scanning failed
                                        // or no virus in memory
        hContext->lpInScanMBuffer = NULL;

        VLScanClose(hContext);

        InitCertlib ( (LPCALLBACKREV1)ScanData.lpCallbacks,
                      INIT_CERTLIB_USE_DEFAULT );

#ifdef SYM_DOS
        NavExInit();
#endif
  
        extResult = EXTScanMemory((LPCALLBACKREV1)ScanData.lpCallbacks,
                                  EXTERNAL_DETECT_VERSION,
                                  bScanHighMemory,
                                  &wVirusID);

        if (EXTSTATUS_VIRUS_FOUND == extResult)
            {

            if (EXTSTATUS_OK == DetermineVirusIndex(ScanData.szVirscanFile,
                                                    wVirusID,
                                                    &wVirusIndex,
                                                    &stN30))
                {
                    lpNotes->wVirID    = wVirusID;
                    lpNotes->wType     = stN30.wType;
                    lpNotes->wControl2 = stN30.wControl2;
                    lpNotes->wInfo     = stN30.wInfo;
                    lpNotes->wSize     = stN30.wInfSize;

                    MEMCPY ( lpNotes->lpVirName, stN30.lpVName, SIZE_VIRUSNAME);
                    lpNotes->lpVirName[SIZE_VIRUSNAME-1] = 0;

                    return (wVirusIndex);
                }
            else
                return(FALSE);
            }
        else
            return (FALSE);
        }

                                        // fill lpNotes with virus info
    uResult = MapHvirusToLpnotes(hVirus, lpNotes);

                                        // if the one_half virus was found
                                        // in memory, write repair info
                                        // to the hardisk.
    //if ( lpNotes->wVirID == VL_ONE_HALF_ID ||
    //     lpNotes->wVirID == 0x0f00 )

    lpBuffer = (LPBYTE) VLTempMemoryAlloc (SIZE_WORK_BUFFER);
    if (lpBuffer != NULL)
        {
        GetInfectionData(lpBuffer, NULL, 0);
        VLTempMemoryFree(lpBuffer);
        }

    ////////////////////////////////////////////////////////////////
    // NULL out  hContext->lpInScanMBuffer so that VLScanClose wont
    // corrupt/free ScanData.lpMemoryScanData
    ////////////////////////////////////////////////////////////////

    hContext->lpInScanMBuffer = NULL;

    VLReleaseVirusHandle(hVirus);       // release hVirus
    VLScanClose(hContext);              // clean up

    /////////////////////////////////////////////////////////////////
    // reinit Certlib because the above two functions corrupt
    // lpgCallBack
    /////////////////////////////////////////////////////////////////

    InitCertlib( (LPCALLBACKREV1)ScanData.lpCallbacks,
                             INIT_CERTLIB_USE_DEFAULT);


    return (uResult);
} // VirusScanMemory()


//========================================================================
//
//
//========================================================================
UINT LOCAL PASCAL MapHvirusToLpnotes (
     HVIRUS      hVirus,
     LPN30NOTES  lpNotes )
{

     lpNotes->wVirID    = (WORD) hVirus->stVirusInfoBlock.wVirID;
     lpNotes->wType     = (WORD) hVirus->stVirusInfoBlock.wType;
     lpNotes->wControl2 = (WORD) hVirus->stVirusInfoBlock.wControl2;
     lpNotes->wInfo     = (WORD) hVirus->stVirusInfoBlock.wInfo;
     lpNotes->wSize     = (WORD) hVirus->stVirusInfoBlock.wInfSize;

     STRCPY ( lpNotes->lpVirName, hVirus->stVirusInfoBlock.lpVName );

     return ( hVirus->uDBEntryNum );        // virus id
}

