// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/file.cpv   1.12   15 Sep 1998 14:13:18   MKEATIN  $
//
// Description: NAVAPI's file scan and repair routines
//
// Contains:
//      NAVScanFile()
//************************************************************************
// $Log:   S:/NAVAPI/VCS/file.cpv  $
// 
//    Rev 1.12   15 Sep 1998 14:13:18   MKEATIN
// Changed bExecutable to lpstFileExt
//
//    Rev 1.11   28 Aug 1998 15:02:44   MKEATIN
// Forcing bPreserveLastAccess to FALSE in NAVScanFile.
//
//    Rev 1.10   30 Jul 1998 21:51:50   DHERTEL
// Changes for NLM packaging of NAVAPI.
//
//    Rev 1.9   29 Jul 1998 16:19:50   MKEATIN
// In NAVRepairFile and NAVDeleteFile, we pass TRUE for the bPreserveLastAccess
// parameter.
//
//    Rev 1.8   29 Jul 1998 16:12:34   MKEATIN
// Added bPreserveLastAccess to NAVScanFile.  If it is FALSE, we will not
// attemp to save the last access date and will therefor open the file with
// READ_ONLY access.
//
//    Rev 1.7   22 Jul 1998 14:16:42   MKEATIN
// Enable/disable auto-protect on WIN32 only.
//
//    Rev 1.6   24 Jun 1998 13:38:02   DALLEE
// Added NAVDeleteFile() with NAV Auto-Protect disabling.
//
//    Rev 1.5   23 Jun 1998 15:25:58   DALLEE
// Updated error return codes.
// Now support mem, access, no repair, and invalid arg returns.
//
//    Rev 1.4   22 Jun 1998 22:28:32   DALLEE
// Added calls to enable/disable NAV auto-protect around scanning.
//
//    Rev 1.3   19 Jun 1998 17:54:46   DALLEE
// Eek! Was forgetting to free virus handle from rescan of file
// within NAVRepairFile().
//
//    Rev 1.2   19 Jun 1998 17:28:30   DALLEE
// Removed cookie parameters - none of the file io callbacks takes
// user data.
// Added bExecutable parameter to ScanFile and RepairFile to turn
// on/off bloodhound on a per item basis.
//
//    Rev 1.1   15 Jun 1998 16:14:06   DALLEE
// Added NAVRepairFile().
//
//    Rev 1.0   11 Jun 1998 17:32:38   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "avapi_l.h"
#include "nlm_nav.h"
#include "navapi.h"
#include "virtcb.h"
#include "apenable.h"

//*************************************************************************
// NAVScanFile()
//
// NAVSTATUS NAVCALLAPI NAVScanFile (
//      HNAVENGINE   hNAVEngine,        [in] NAV engine handle
//      LPVOID       lpvFileId,         [in] pointer to client-defined file ID.
//      LPTSTR       lpstFileExt        [in] The file's extention (COM, EXE, DOT or NULL)
//      BOOL         bPreserveLastAccess,[in] if TRUE, preserves last access date
//      HNAVVIRUS*   lphVirus )         [out] receives handle to infected item.
//
// This routine scans the specified file item for viruses.
// If a virus is found, *lphVirus will receive a handle to information
// on the infection. If no virus is found, *lphVirus will be NULL.
//
// Returns:
//      NAV_OK on success.
//
//      On error one of:
//          NAV_MEMORY_ERROR
//          NAV_INVALID_ARG
//          NAV_NO_ACCESS
//          NAV_ERROR
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

NAVSTATUS NAVCALLAPI NAVScanFile
(
    HNAVENGINE   hNAVEngine,
    LPVOID       lpvFileId,
    LPTSTR       lpstFileExt,
    BOOL         bPreserveLastAccess,
    HNAVVIRUS*   lphVirus
)
{
    auto    TCHAR       szEncodedName [ SYM_MAX_PATH ];
    auto    VSTATUS     vStatus;
    auto    NAVSTATUS   nStatus;

    SYM_ASSERT( lphVirus );
    SYM_ASSERT( hNAVEngine );

    *lphVirus = NULL;

    // !!!! forcing bPreserveLastAccess to FALSE - since we know we are
    // working with virtual files this functionality doesn't make sense
    // and could cause unpredictable behaviour.

    bPreserveLastAccess = FALSE;

    EncodeVirtFileId( szEncodedName,
                      lpvFileId,
                      lpstFileExt );

    // NOTE: Currently translating freely between HNAVVIRUS and HVLVIRUS.

#ifdef SYM_WIN32
    NavapUnprotectProcess();
#endif

    vStatus = VLScanFile( (HVCONTEXT) hNAVEngine,
                          szEncodedName,
                          lphVirus,
                          bPreserveLastAccess);

#ifdef SYM_WIN32
    NavapProtectProcess();
#endif

    switch ( vStatus )
    {
    case VS_OK:
        nStatus = NAV_OK;
        break;

    case VS_MEMALLOC:
        nStatus = NAV_MEMORY_ERROR;
        break;

    case VS_INVALID_ARG:
        nStatus = NAV_INVALID_ARG;
        break;

    case VS_FILE_ERROR:
    case VS_NO_ACCESS:
        nStatus = NAV_NO_ACCESS;
        break;

    default:
        nStatus = NAV_ERROR;
        break;
    }

    return ( nStatus );
} // NAVScanFile()


//*************************************************************************
// NAVRepairFile()
//
// NAVSTATUS NAVCALLAPI NAVRepairFile (
//      HNAVENGINE   hNAVEngine,        [in] NAV engine handle
//      LPVOID       lpvFileId,         [in] pointer to client-defined file ID.
//      LPSTR        lpstFileExt )      [in] File extention (EXT, COM, DOT, or NULL)
//
// This routine repairs the specified file.
//
// NOTE: Not taking HNAVVIRUS from NAVScanFile() as input since the
// file identification information is only a pointer and the client may
// have moved or freed the information referenced by the pointer.
// NAVRepairFile() will rescan then repair the specified file.
//
// Returns:
//      NAV_OK on success.
//
//      On error one of:
//          NAV_MEMORY_ERROR
//          NAV_INVALID_ARG
//          NAV_NO_ACCESS
//          NAV_CANT_REPAIR
//          NAV_ERROR
//*************************************************************************
// 6/15/98 DALLEE, created.
//*************************************************************************

NAVSTATUS NAVCALLAPI NAVRepairFile
(
    HNAVENGINE   hNAVEngine,
    LPVOID       lpvFileId,
    LPTSTR       lpstFileExt
)
{
    auto    HNAVVIRUS   hVirus;
    auto    VSTATUS     vStatus;
    auto    NAVSTATUS   nStatus;

    SYM_ASSERT( hNAVEngine );

    // Disable NAV Auto-Protect.

#ifdef SYM_WIN32
    NavapUnprotectProcess();
#endif

    // Get new valid HVLVIRUS

    nStatus = NAVScanFile( hNAVEngine, lpvFileId, lpstFileExt, FALSE, &hVirus );

    if ( NAV_OK == nStatus )
    {
        if ( NULL != hVirus )
        {
            // NOTE: Currently translating freely between HNAVVIRUS and HVLVIRUS.

            // Repair item.

            vStatus = VLRepairFile( hVirus, NULL );

            NAVFreeVirusHandle( hVirus );

            // Translate from VSTATUS to NAVSTATUS result.

            switch ( vStatus )
            {
            case VS_OK:
                nStatus = NAV_OK;
                break;

            case VS_MEMALLOC:
                nStatus = NAV_MEMORY_ERROR;
                break;

            case VS_INVALID_ARG:
                nStatus = NAV_INVALID_ARG;
                break;

            case VS_FILE_ERROR:
            case VS_NO_ACCESS:
                nStatus = NAV_NO_ACCESS;
                break;

            case VS_CANT_REPAIR:
                nStatus = NAV_CANT_REPAIR;
                break;

            default:
                nStatus = NAV_ERROR;
                break;
            }
        }
        else
        {
            // Item wasn't infected...

            nStatus = NAV_CANT_REPAIR;
        }
    }

    // Reenable NAV Auto-Protect

#ifdef SYM_WIN32
    NavapProtectProcess();
#endif

    return ( nStatus );
} // NAVRepairFile()


//*************************************************************************
// NAVDeleteFile()
//
// NAVSTATUS NAVCALLAPI NAVDeleteFile (
//      HNAVENGINE   hNAVEngine,        [in] NAV engine handle
//      LPVOID       lpvFileId,         [in] pointer to client-defined file ID.
//      LPSTR        lpstFileExt )      [in] File extention (EXT, COM, DOT, or NULL)
//
// This routine deletes the specified file.
//
// NOTE: Not taking HNAVVIRUS from NAVScanFile() as input since the
// file identification information is only a pointer and the client may
// have moved or freed the information referenced by the pointer.
// NAVDeleteFile() will rescan then delete the specified file if not
// infected by a cluster-infector or other non-deletable virus.
//
// Returns:
//      NAV_OK on success.
//
//      On error one of:
//          NAV_MEMORY_ERROR
//          NAV_INVALID_ARG
//          NAV_NO_ACCESS
//          NAV_CANT_DELETE
//          NAV_ERROR
//*************************************************************************
// 6/24/98 DALLEE, created.
//*************************************************************************

NAVSTATUS NAVCALLAPI NAVDeleteFile
(
    HNAVENGINE   hNAVEngine,
    LPVOID       lpvFileId,
    LPTSTR       lpstFileExt
)
{
    auto    HNAVVIRUS   hVirus;
    auto    VSTATUS     vStatus;
    auto    NAVSTATUS   nStatus;

    auto    LPNAVFILEIO lpFileCallbacks;

    SYM_ASSERT( hNAVEngine );

    // Disable NAV Auto-Protect.

#ifdef SYM_WIN32
    NavapUnprotectProcess();
#endif

    // Get new valid HVLVIRUS

    nStatus = NAVScanFile( hNAVEngine, lpvFileId, lpstFileExt, FALSE, &hVirus );

    if ( NAV_OK == nStatus )
    {
        if ( NULL != hVirus )
        {
            // NOTE: Currently translating freely between HNAVVIRUS and HVLVIRUS.

            // Delete item.

            vStatus = VLDeleteFile( hVirus );

            NAVFreeVirusHandle( hVirus );

            // Translate from VSTATUS to NAVSTATUS result.

            switch ( vStatus )
            {
            case VS_OK:
                nStatus = NAV_OK;
                break;

            case VS_MEMALLOC:
                nStatus = NAV_MEMORY_ERROR;
                break;

            case VS_INVALID_ARG:
                nStatus = NAV_INVALID_ARG;
                break;

            case VS_FILE_ERROR:
            case VS_NO_ACCESS:
                nStatus = NAV_NO_ACCESS;
                break;

            case VS_CANT_DELETE:
                nStatus = NAV_CANT_DELETE;
                break;

            default:
                nStatus = NAV_ERROR;
                break;
            }
        }
        else
        {
            // Item wasn't infected...
            // Just delete through client callback.

            lpFileCallbacks = GetFileIoCallbacks();
            SYM_ASSERT( lpFileCallbacks->Delete );

            nStatus = lpFileCallbacks->Delete( lpvFileId ) ? NAV_OK : NAV_NO_ACCESS;
        }
    }

    // Reenable NAV Auto-Protect

#ifdef SYM_WIN32
    NavapProtectProcess();
#endif

    return ( nStatus );
} // NAVDeleteFile()

