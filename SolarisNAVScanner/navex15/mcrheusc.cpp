//************************************************************************
//
// $Header:   S:/NAVEX/VCS/MCRHEUSC.CPv   1.6   09 Dec 1998 17:45:24   DCHI  $
//
// Description:
//      Contains Microsoft Office macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MCRHEUSC.CPv  $
// 
//    Rev 1.6   09 Dec 1998 17:45:24   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.5   04 Aug 1998 13:52:24   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.4   10 Jun 1998 13:20:38   DCHI
// Added W7H2
// 
//    Rev 1.3   11 May 1998 17:53:20   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.2   15 Apr 1998 17:23:18   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.1   15 Oct 1997 18:15:34   DCHI
// Added Word 97 heuristics.
// 
//    Rev 1.0   09 Jul 1997 16:16:06   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "platform.h"
#include "callback.h"

#include "ctsn.h"
#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "macrovid.h"

#include "olestrnm.h"
#include "wdscan15.h"
#include "offcsc15.h"
#include "mcrheusc.h"
#include "wd7heusc.h"
#include "w97heusc.h"
#include "w7h2scan.h"
#include "xlheusc.h"

//********************************************************************
//
// Function:
//  EXTSTATUS MSOfficeHeuristicScan()
//
// Description:
//  Scans a file for the presence of Microsoft Office macro viruses.
//
//  When the function returns, the following stream indices are known
//  if a virus is detected:
//      WD7:    wWordDocumentEntry
//      WD8:    wMacrosEntry
//              wMacrosChildEntry
//              wVBAEntry
//              wVBAChildEntry
//      XL97:   w_VBA_PROJECT_CUREntry
//              w_VBA_PROJECT_CURChildEntry
//              wVBAEntry
//              wVBAChildEntry
//      XL5:    wHexNameEntry
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//  of the virus.
//
//********************************************************************

EXTSTATUS MSOfficeHeuristicScan
(
    LPMSOFFICE_SCAN         lpstMSOfficeScan,
    LPWORD                  lpwHeuristicVID
)
{
    DWORD                   dwDirID;

	// Verify that it is a Word file and that it has macros

    dwDirID = 0;
    while (SSEnumDirEntriesCB(lpstMSOfficeScan->lpstRoot,
                              MicrosoftOfficeScanCB,
                              lpstMSOfficeScan,
                              &dwDirID,
                              lpstMSOfficeScan->lpstStream) == SS_STATUS_OK)
    {
        switch (lpstMSOfficeScan->wType)
        {
            case OLE_SCAN_WD7:
            {
                if (W7H2Scan(lpstMSOfficeScan->lpstStream) ==
                    EXTSTATUS_VIRUS_FOUND)
                {
                    *lpwHeuristicVID = VID_MACRO_HEURISTICS;
                    return(EXTSTATUS_VIRUS_FOUND);
                }

                if (*lpwHeuristicVID == VID_MACRO_HEURISTICS)
                {
                    if (WD7HeuristicScan(lpstMSOfficeScan) ==
                        EXTSTATUS_VIRUS_FOUND)
                        return(EXTSTATUS_VIRUS_FOUND);
                }

                break;
            }

            case OLE_SCAN_WD8:
            {
                if (Word97HeuristicScan(lpstMSOfficeScan,
                                        FALSE) ==
                    EXTSTATUS_VIRUS_FOUND)
                    return(EXTSTATUS_VIRUS_FOUND);

                break;
            }

            case OLE_SCAN_XL97:
            {
                if (XL97HeuristicScan(lpstMSOfficeScan,
                                      FALSE) == EXTSTATUS_VIRUS_FOUND)
                {
                    *lpwHeuristicVID = VID_XH;
                    return(EXTSTATUS_VIRUS_FOUND);
                }
                break;
            }

            case OLE_SCAN_XL5:
                if (XL5HeuristicScan(lpstMSOfficeScan,
                                     FALSE) == EXTSTATUS_VIRUS_FOUND)
                {
                    *lpwHeuristicVID = VID_XH;
                    return(EXTSTATUS_VIRUS_FOUND);
                }
                break;

            default:
                break;
        }
    }

	// No virus

    return (EXTSTATUS_OK);
}

#endif // #ifdef MACROHEU

