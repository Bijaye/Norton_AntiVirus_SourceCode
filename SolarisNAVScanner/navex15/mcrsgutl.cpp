//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/Mcrsgutl.cpv   1.4   15 Dec 1998 12:16:20   DCHI  $
//
// Description:
//      Contains WordDocument macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/Mcrsgutl.cpv  $
// 
//    Rev 1.4   15 Dec 1998 12:16:20   DCHI
// Added support for PowerPoint signature set.
// 
//    Rev 1.3   11 May 1998 18:15:32   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.2   15 Apr 1998 17:23:48   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.1   26 Nov 1997 18:35:08   DCHI
// Added #ifdef'd out PowerPoint scanning code.
// 
//    Rev 1.0   09 Jul 1997 16:26:02   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#include "platform.h"
#include "callback.h"

#include "ctsn.h"
#include "navex.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "wdscan15.h"
#include "offcsc15.h"
#include "o97scn15.h"

#include "mcrsgutl.h"

//********************************************************************
//
// Function:
//  long CalcHitMemSize()
//
// Parameters:
//  lNumSigs            Number of signatures
//
// Description:
//  Calculates the amount of memory required for the signature
//  hit array for a set of signatures with lNumSigs signatures.
//
// Returns:
//  long                Size of hit memory required
//
//********************************************************************

long CalcHitMemSize
(
    long                lNumSigs
)
{
    long                lSize;

    // Inter-macro and intra-macro name sig hit arrays

    if (lNumSigs < 8)
        lSize = 1;
    else
    {
        lSize = lNumSigs >> 3;
        if (lNumSigs & 7)
            ++lSize;
    }

    return(lSize);
}


//********************************************************************
//
// Function:
//  long SigSetCalcHitMemSize()
//
// Parameters:
//  lpstSigSet          Ptr to signature set
//
// Description:
//  Calculates the amount of memory required for the signature
//  hit arrays for the given signature set.
//
// Returns:
//  long                Size of hit memory required
//
//********************************************************************

long SigSetCalcHitMemSize
(
    LPWD_SIG_SET        lpstSigSet
)
{
    long                lSize = 0;

    // Inter-macro and intra-macro name sig hit arrays

    lSize += 2 * CalcHitMemSize(lpstSigSet->lNumNameSigs);

    // Named CRC sig hit array

    lSize += CalcHitMemSize(lpstSigSet->lNumNamedCRCSigs);

    // Macro sig hit array

    lSize += CalcHitMemSize(lpstSigSet->lNumMacroSigs);

    // CRC hit array

    lSize += CalcHitMemSize(lpstSigSet->lNumCRCs);

    return(lSize);
}


//********************************************************************
//
// Function:
//  BOOL AllocMacroSigHitMem()
//
// Parameters:
//  lpvRootCookie       Root cookie for memory allocation call
//  lpstMSOfficeScan    MS office scan structure
//
// Description:
//  Determines the maximum amount of memory needed for the hit
//  arrays for all categories and then allocates that much memory,
//  storing the pointer to the memory at
//  lpstMSOfficeScan->lpbyHitMem.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory allocation error
//
//********************************************************************

BOOL AllocMacroSigHitMem
(
    LPVOID                  lpvRootCookie,
    LPMSOFFICE_SCAN         lpstMSOfficeScan
)
{
    DWORD                   dwMaxRequiredHitMem;
    DWORD                   dwHitMemRequirement;

    dwMaxRequiredHitMem = 0;

    // Word 6.0/7.0/95

    dwHitMemRequirement = SigSetCalcHitMemSize(&gstWD7SigSet);

    if (dwHitMemRequirement > dwMaxRequiredHitMem)
        dwMaxRequiredHitMem = dwHitMemRequirement;

    // Word 8.0/97

    dwHitMemRequirement = SigSetCalcHitMemSize(&gstWD8SigSet);

    if (dwHitMemRequirement > dwMaxRequiredHitMem)
        dwMaxRequiredHitMem = dwHitMemRequirement;

    // Excel 97

    dwHitMemRequirement = SigSetCalcHitMemSize(&gstXL97SigSet);

    if (dwHitMemRequirement > dwMaxRequiredHitMem)
        dwMaxRequiredHitMem = dwHitMemRequirement;

    // Excel 95

    dwHitMemRequirement = SigSetCalcHitMemSize(&gstXL95SigSet);

    if (dwHitMemRequirement > dwMaxRequiredHitMem)
        dwMaxRequiredHitMem = dwHitMemRequirement;

    // Excel 4

    dwHitMemRequirement = SigSetCalcHitMemSize(&gstXL4SigSet);

    if (dwHitMemRequirement > dwMaxRequiredHitMem)
        dwMaxRequiredHitMem = dwHitMemRequirement;

#ifdef PP_SCAN
    // PowerPoint 97

    dwHitMemRequirement = SigSetCalcHitMemSize(&gstP97SigSet);
#endif // #ifdef PP_SCAN

    if (dwHitMemRequirement > dwMaxRequiredHitMem)
        dwMaxRequiredHitMem = dwHitMemRequirement;

    // Allocate the memory

    if (SSMemoryAlloc(lpvRootCookie,
                      dwMaxRequiredHitMem,
                      (LPLPVOID)&lpstMSOfficeScan->lpbyHitMem) !=
        SS_STATUS_OK)
    {
        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL FreeMacroSigHitMem()
//
// Parameters:
//  lpvRootCookie       Root cookie for memory free call
//  lpstMSOfficeScan    MS office scan structure
//
// Description:
//  Frees the memory allocated for the signature hit arrays.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory free error
//
//********************************************************************

BOOL FreeMacroSigHitMem
(
    LPVOID                  lpvRootCookie,
    LPMSOFFICE_SCAN         lpstMSOfficeScan
)
{
    if (SSMemoryFree(lpvRootCookie,
                     lpstMSOfficeScan->lpbyHitMem) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL AssignMacroSigHitMem()
//
// Parameters:
//  lpstMSOfficeScan    MS office scan structure
//  lpstScan            Scan structure to assign memory to
//
// Description:
//  Switches on the type of scan and assigns the hit memory
//  accordingly.
//
// Returns:
//  TRUE                On success
//  FALSE               On memory free error
//
//********************************************************************

BOOL AssignMacroSigHitMem
(
    LPMSOFFICE_SCAN         lpstMSOfficeScan,
    LPWD_SCAN               lpstScan
)
{
    LPBYTE                  lpbyMem;
    LPWD_SIG_SET            lpstSigSet;

    switch (lpstMSOfficeScan->wType)
    {
        case OLE_SCAN_WD7:
            lpstSigSet = &gstWD7SigSet;
            break;

        case OLE_SCAN_WD8:
            lpstSigSet = &gstWD8SigSet;
            break;

        case OLE_SCAN_XL97:
            lpstSigSet = &gstXL97SigSet;
            break;

        case OLE_SCAN_XL5:
            lpstSigSet = &gstXL95SigSet;
            break;

        case OLE_SCAN_XL4_95:
        case OLE_SCAN_XL4_97:
            lpstSigSet = &gstXL4SigSet;
            break;

        case OLE_SCAN_A2:
            lpstSigSet = &gstA2SigSet;
            break;

        case OLE_SCAN_A97:
            lpstSigSet = &gstA97SigSet;
            break;

#ifdef PP_SCAN
        case OLE_SCAN_PP:
            lpstSigSet = &gstP97SigSet;
            break;
#endif // #ifdef PP_SCAN

        default:
            return(FALSE);
    }

    lpbyMem = lpstMSOfficeScan->lpbyHitMem;
    lpstScan->lpabyNameSigInterMacroHit = lpbyMem;
    lpbyMem += CalcHitMemSize(lpstSigSet->lNumNameSigs);
    lpstScan->lpabyNameSigIntraMacroHit = lpbyMem;
    lpbyMem += CalcHitMemSize(lpstSigSet->lNumNameSigs);
    lpstScan->lpabyNamedCRCSigHit = lpbyMem;
    lpbyMem += CalcHitMemSize(lpstSigSet->lNumNamedCRCSigs);
    lpstScan->lpabyMacroSigHit = lpbyMem;
    lpbyMem += CalcHitMemSize(lpstSigSet->lNumMacroSigs);
    lpstScan->lpabyCRCHit = lpbyMem;
    lpbyMem += CalcHitMemSize(lpstSigSet->lNumCRCs);

    return(TRUE);
}

