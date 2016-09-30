//************************************************************************
//
// $Header:   S:/NAVEX/VCS/VBA5SCAN.CPv   1.7   27 May 1997 16:50:36   DCHI  $
//
// Description:
//      Contains Word 8.0 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/VBA5SCAN.CPv  $
// 
//    Rev 1.7   27 May 1997 16:50:36   DCHI
// Modifications to support dynamic allocation of hit memory and new
// Office 97 CRC.
// 
//    Rev 1.6   10 Apr 1997 16:51:20   DCHI
// Fixed FullSet checking.
// 
//    Rev 1.5   10 Apr 1997 14:28:36   DCHI
// Corrected problem with XL97 scan using work buffer for blank module check.
// 
//    Rev 1.4   10 Apr 1997 11:04:26   DCHI
// OLEOpenVBA5ModuleCB() now checks for a non-NULL lpvCookie before
// copying the name.
// 
//    Rev 1.3   08 Apr 1997 12:40:44   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.2   07 Apr 1997 18:11:14   DCHI
// Added MVP support.
// 
//    Rev 1.1   14 Mar 1997 16:34:40   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.0   13 Feb 1997 13:24:32   DCHI
// Initial revision.
// 
//************************************************************************

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"

#include "ctsn.h"
#include "navex.h"

#include "endutils.h"

#include "wdscan.h"
#include "vba5scan.h"
#include "wd8sigs.h"
#include "xl97sigs.h"

#include "mvp.h"

#include "mcrhitmm.h"

// VBA

BYTE FAR gauszVBA[] = { 'V', 0, 'B', 0, 'A', 0, 0, 0 };

// dir

BYTE FAR gauszdir[] = { 'd', 0, 'i', 0, 'r', 0, 0, 0 };

// _VBA_PROJECT

BYTE FAR gausz_VBA_PROJECT[] =
{
    '_', 0, 'V', 0, 'B', 0, 'A', 0,
    '_', 0, 'P', 0, 'R', 0, 'O', 0, 'J', 0, 'E', 0, 'C', 0, 'T', 0, 0, 0
};

// PROJECT

BYTE FAR gauszPROJECT[] =
{
    'P', 0, 'R', 0, 'O', 0, 'J', 0, 'E', 0, 'C', 0, 'T', 0, 0, 0
};

// PROJECTwm

BYTE FAR gauszPROJECTwm[] =
{
    'P', 0, 'R', 0, 'O', 0, 'J', 0, 'E', 0, 'C', 0, 'T', 0, 'w', 0, 'm', 0, 0, 0
};

// 1Table

BYTE FAR gausz1Table[] =
{
    '1', 0, 'T', 0, 'a', 0, 'b', 0, 'l', 0, 'e', 0, 0, 0
};

// 0Table

BYTE FAR gausz0Table[] =
{
    '0', 0, 'T', 0, 'a', 0, 'b', 0, 'l', 0, 'e', 0, 0, 0
};

// Macros

BYTE FAR gauszMacros[] =
{
    'M', 0, 'a', 0, 'c', 0, 'r', 0, 'o', 0, 's', 0, 0, 0
};

// ThisDocument

BYTE FAR gauszThisDocument[] =
{
    'T', 0, 'h', 0, 'i', 0, 's', 0,
    'D', 0, 'o', 0, 'c', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0,
    0, 0
};

// Workbook

BYTE FAR gauszWorkbook[] =
{
    'W', 0, 'o', 0, 'r', 0, 'k', 0, 'b', 0, 'o', 0, 'o', 0, 'k', 0, 0, 0
};

// _VBA_PROJECT_CUR

BYTE FAR gausz_VBA_PROJECT_CUR[] =
{
    '_', 0, 'V', 0, 'B', 0, 'A', 0,
    '_', 0, 'P', 0, 'R', 0, 'O', 0, 'J', 0, 'E', 0, 'C', 0, 'T', 0,
    '_', 0, 'C', 0, 'U', 0, 'R', 0, 0, 0
};

// ThisWorkbook

BYTE FAR gauszThisWorkbook[] =
{
    'T', 0, 'h', 0, 'i', 0, 's', 0,
    'W', 0, 'o', 0, 'r', 0, 'k', 0, 'b', 0, 'o', 0, 'o', 0, 'k', 0,
    0, 0
};

// For NAVBOOT 2.0 DX, NAVEX.EXP, it looks like we don't have enough
//  stack, so use global memory for the 4.5K LZNT_T structure, since
//  it is not multithreaded

#if defined(SYM_DOSX)
LZNT_T FAR stLZNT;
#endif

//********************************************************************
//
// Function:
//  int OLEOpenVBACB()
//
// Description:
//  Checks the entry to see if it is the VBA storage.
//  If so then the function sets the following fields of the VBA5
//  structure:
//      wVBAEntry
//      wVBAChildEntry
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If the entry is a VBA storage
//  OLE_OPEN_CB_STATUS_CONTINUE     If the entry is not
//
//********************************************************************


int OLEOpenVBACB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszVBA,
                     MAX_OLE_NAME_LEN) == 0)
        {
            LPVBA5_SCAN     lpstVBA5Scan = (LPVBA5_SCAN)lpvCookie;

            lpstVBA5Scan->wVBAEntry = (WORD)dwIndex;
            lpstVBA5Scan->wVBAChildEntry =
                (WORD)ConvertEndianLong(lpstEntry->dwSIDChild);

            return(OLE_OPEN_CB_STATUS_RETURN);
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5OpenCB()
//
// Description:
//  Starts from the given child and searches for a VBA storage.
//
// Returns:
//  TRUE                    If a VBA storage was found
//  FALSE                   If a VBA storage was not found
//
//********************************************************************

BOOL VBA5OpenVBAStorage
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // WordDocument stream
    LPVBA5_SCAN     lpstVBA5Scan            // VBA5 project stream info
)
{
    OLE_OPEN_SIB_T  stOpenSib;
    WORD            awSibs[MAX_VBA5_SIB_DEPTH];

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.nNumWaitingSibs = 1;
    stOpenSib.lpawSibs = awSibs;
    switch (lpstVBA5Scan->wType)
    {
        case VBA5_WD8:
            awSibs[0] = lpstVBA5Scan->u.stWD8.wMacrosChildEntry;
            break;

        case VBA5_XL97:
            awSibs[0] = lpstVBA5Scan->u.stXL97.w_VBA_PROJECT_CURChildEntry;
            break;

        default:
            return(FALSE);
    }

    if (OLEOpenSiblingCB(lpstCallBack,
                         lpstOLEFile->hFile,
                         lpstOLEFile,
                         OLEOpenVBACB,
                         (LPVOID)lpstVBA5Scan,
                         NULL,
                         &stOpenSib) == OLE_OK)
    {
        return(TRUE);
    }

    return(FALSE);
}

#if 0

//********************************************************************
//
// Function:
//  int OLEOpen_VBA_PROJECTCB()
//
// Description:
//  Checks the entry to see if it is the _VBA_PROJECT stream.
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If the entry is _VBA_PROJECT
//  OLE_OPEN_CB_STATUS_CONTINUE     If the entry is not
//
//********************************************************************

BYTE gausz_VBA_PROJECT[] =
{
    '_', 0, 'V', 0, 'B', 0, 'A', 0,
    '_', 0, 'P', 0, 'R', 0, 'O', 0, 'J', 0, 'E', 0, 'C', 0, 'T', 0, 0, 0
};

int OLEOpen_VBA_PROJECTCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gausz_VBA_PROJECT,
                     MAX_OLE_NAME_LEN) == 0)
        {
            return(OLE_OPEN_CB_STATUS_RETURN);
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5Open_VBA_PROJECT()
//
// Description:
//  Starts from the given child and searches for a _VBA_PROJECT
//  stream.
//
// Returns:
//  TRUE                    If a _VBA_PROJECT storage was found
//  FALSE                   If a _VBA_PROJECT storage was not found
//
//********************************************************************

BOOL VBA5Open_VBA_PROJECT
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // WordDocument stream
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    WORD            wRootChild              // The root child sibling
)
{
    OLE_OPEN_SIB_T  stOpenSib;
    WORD            awSibs[MAX_VBA5_SIB_DEPTH];

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.nNumWaitingSibs = 1;
    stOpenSib.lpawSibs = awSibs;
    awSibs[0] = wRootChild;

    if (OLEOpenSiblingCB(lpstCallBack,
                         lpstOLEFile->hFile,
                         lpstOLEFile,
                         OLEOpen_VBA_PROJECTCB,
                         (LPVOID)NULL,
                         lpbyStreamBATCache,
                         &stOpenSib) == OLE_OK)
    {
        return(TRUE);
    }

    return(FALSE);
}

#endif

//********************************************************************
//
// Function:
//  int OLEOpenVBA5ModuleCB()
//
// Description:
//  If the name of the entry is not "dir" and it does not begin
//  with an underscore, and the name is not the empty string,
//  then the function returns success, indicating a candidate.
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If the entry is a candidate
//  OLE_OPEN_CB_STATUS_CONTINUE     If the entry is not
//
//********************************************************************

int OLEOpenVBA5ModuleCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    int                 i;
    WORD                wTemp;

    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (ConvertEndianShort(lpstEntry->uszName[0]) == '_' ||
            lpstEntry->uszName[0] == 0 ||
            WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszdir,
                     MAX_OLE_NAME_LEN) == 0)
        {
            return(OLE_OPEN_CB_STATUS_CONTINUE);
        }

        if (lpvCookie != NULL)
        {
            // Copy the name and uppercase it

            for (i=0;i<MAX_OLE_NAME_LEN;i++)
            {
                wTemp = ConvertEndianShort(lpstEntry->uszName[i]);
                if (wTemp == 0)
                    break;

                if ('a' <= wTemp && wTemp <= 'z')
                    ((LPBYTE)lpvCookie)[i] = (BYTE)(wTemp - 'a' + 'A');
                else
                    ((LPBYTE)lpvCookie)[i] = (BYTE)wTemp;
            }

            // Zero terminate it

            ((LPBYTE)lpvCookie)[i] = 0;
        }

        // Try it

        return(OLE_OPEN_CB_STATUS_RETURN);
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5OpenCandidateModule()
//
// Description:
//  Uses the OLE sibling structure to search for VBA module
//  candidates.  A stream is a VBA module candidate if its
//  name neither begins with an underscore nor is the name "dir".
//
// Returns:
//  TRUE                    If a candidate is found
//  FALSE                   If a candidate is not found
//
//********************************************************************

BOOL VBA5OpenCandidateModule
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // WordDocument stream
    LPBYTE          lpbyName,               // Storage for stream name
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPOLE_OPEN_SIB  lpstOLEOpenSib          // Sibling structure
)
{
    if (OLEOpenSiblingCB(lpstCallBack,
                         lpstOLEFile->hFile,
                         lpstOLEFile,
                         OLEOpenVBA5ModuleCB,
                         (LPVOID)lpbyName,
                         lpbyStreamBATCache,
                         lpstOLEOpenSib) == OLE_OK)
    {
        return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5MemICmp()
//
// Description:
//  Performs a case insensitive compare of two memory buffers
//  up to n bytes.
//
// Returns:
//  TRUE        If the buffers are both the same
//  FALSE       If the buffers are different
//
//********************************************************************

BOOL VBA5MemICmp
(
    LPBYTE      lpbyMem0,
    LPBYTE      lpbyMem1,
    int         n
)
{
    BYTE        by0, by1;

    while (n-- > 0)
    {
        by0 = *lpbyMem0++;
        by1 = *lpbyMem1++;

        if ('a' <= by0 && by0 <= 'z')
            by0 = by0 - 'a' + 'A';

        if ('a' <= by1 && by1 <= 'z')
            by1 = by1 - 'a' + 'A';

        if (by0 != by1)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5LZNTStreamAttributeOnly()
//
// Description:
//  Checks the given LZNT compressed stream to see whether it
//  contains lines that begin with something other than
//  "Attribute VB_".
//
// Returns:
//  TRUE        If the stream was successfully analyzed as
//              containing only "Attribute VB_" lines
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL VBA5LZNTStreamAttributeOnly
(
    LPLZNT          lpstLZNT,
    LPBYTE          lpbyWorkBuffer      // >= 256 bytes
)
{
    DWORD           dwOffset;
    int             nNumBytes;
    int             nNextI;
    int             i;

    // Assume lines are no longer than 256 bytes

    dwOffset = 0;
    nNumBytes = 256;
    while (dwOffset < lpstLZNT->dwSize)
    {
        if (dwOffset + 256 > lpstLZNT->dwSize)
            nNumBytes = (int)(lpstLZNT->dwSize - dwOffset);

        if (LZNTGetBytes(lpstLZNT,
                         dwOffset,
                         nNumBytes,
                         lpbyWorkBuffer) != nNumBytes)
        {
            return(FALSE);
        }

        // Iterate through lines while the line begins with
        //  "Attribute VB_"

        i = 0;
        while (i < nNumBytes)
        {
            for (nNextI=i;nNextI<nNumBytes;nNextI++)
            {
                if (lpbyWorkBuffer[nNextI] == 0x0D)
                    break;
            }

            // Was the end of the line found?

            if (nNextI < nNumBytes)
            {
                // Found the end of a line

                // Terminate it

                if (VBA5MemICmp((LPBYTE)"Attribute VB_",
                                lpbyWorkBuffer+i,
                                sizeof("Attribute VB_") - 1) == FALSE &&
                    lpbyWorkBuffer[i] != 0x0D)
                {
                    // Non attribute line found

                    return(FALSE);
                }

                i = nNextI + 2;
            }
            else
            {
                break;
            }
        }

        if (i == 0)
        {
            // The end of the line was not found, meaning that
            //  the line was not terminated.  Special condition!?

            return(FALSE);
        }

        dwOffset += i;
    }

    // Only attribute lines found

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS WD8Scan()
//
// Description:
//  Scans a file for the presence of Word 8.0 Macro viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS WD8Scan
(
    LPCALLBACKREV1          lpstCallBack,       // File op callbacks
    LPOLE_FILE              lpstOLEFile,        // OLE file structure
    LPBYTE                  lpbyStreamBATCache, // Non NULL if cache, 512 bytes
    LPVBA5_SCAN             lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 870 bytes
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
)
{
    LPWD_SCAN           lpstScan;
#if !defined(SYM_DOSX)
    LZNT_T              stLZNT;
#endif

    OLE_OPEN_SIB_T      stOpenSib;

    BOOL                bMVPApproved = TRUE;

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    BYTE abyNameSigInterMacroHit[WD8_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNameSigIntraMacroHit[WD8_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNamedCRCSigHit[WD8_NAMED_CRC_SIG_HIT_ARRAY_SIZE];
    BYTE abyMacroSigHit[WD8_MACRO_SIG_HIT_ARRAY_SIZE];
    BYTE abyCRCHit[WD8_CRC_HIT_ARRAY_SIZE];
#else
    LPBYTE              lpbyHitMem;
#endif

    lpstScan = (LPWD_SCAN)lpbyWorkBuffer;
    lpbyWorkBuffer += sizeof(WD_SCAN_T);

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    lpstScan->lpabyNameSigInterMacroHit = abyNameSigInterMacroHit;
    lpstScan->lpabyNameSigIntraMacroHit = abyNameSigIntraMacroHit;
    lpstScan->lpabyNamedCRCSigHit = abyNamedCRCSigHit;
    lpstScan->lpabyMacroSigHit = abyMacroSigHit;
    lpstScan->lpabyCRCHit = abyCRCHit;
#else
    if (AllocMacroSigHitMem(lpstCallBack,
                            &lpbyHitMem,
                            HIT_MEM_WD8) == FALSE)
        return(EXTSTATUS_MEM_ERROR);

    AssignMacroSigHitMem(lpbyHitMem,lpstScan);
#endif

    // Find the <VBA> storage

    if (VBA5OpenVBAStorage(lpstCallBack,
                           lpstOLEFile,
                           lpstVBA5Scan) == FALSE)
    {
        // No VBA storage!

        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(EXTSTATUS_OK);
    }

    // Initialize persistent members of the scan structure

    lpstScan->lpstCallBack = lpstCallBack;
    lpstScan->lpstOLEStream = lpstOLEFile;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;
    lpstScan->uScan.stVBA5.lpstLZNT = &stLZNT;

    // Initialize macro count

    lpstScan->wMacroCount = 0;

    // Assume a full set

    lpstScan->wFlags = WD_SCAN_FLAG_FULL_SET;

    WDInitHitBitArrays(&gstWD8SigSet,lpstScan);

    // awSibs[0] contains the index of the child of the VBA storage

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.nNumWaitingSibs = 1;
    stOpenSib.lpawSibs = (LPWORD)lpbyWorkBuffer;

    stOpenSib.lpawSibs[0] = lpstVBA5Scan->wVBAChildEntry;

    while (VBA5OpenCandidateModule(lpstCallBack,
                                   lpstOLEFile,
                                   lpstScan->abyName,
                                   lpbyStreamBATCache,
                                   &stOpenSib) == TRUE)
    {
        VBA5ScanModule(&gstWD8SigSet,lpstScan);

        if (VBA5LZNTStreamAttributeOnly(&stLZNT,
                                        lpstScan->abyRunBuf) == FALSE)
        {
            if (MVPCheck(MVP_WD8,
                         lpstScan->abyName,
                         lpstScan->dwCRC) == FALSE)
            {
                // Found a non-approved macro

                bMVPApproved = FALSE;
            }

            // The stream contains non-attribute lines

            if (lpstScan->wMacroCount < 0xFFFF)
                lpstScan->wMacroCount++;

            // Determine full set status

            if ((lpstScan->wFlags & WD_SCAN_FLAG_MACRO_IS_FULL_SET) == 0)
            {
                // Found a macro that was not part of the full set

                lpstScan->wFlags &= ~WD_SCAN_FLAG_FULL_SET;
            }
        }
    }

    // If macro count is zero, don't assume full set

    if (lpstScan->wMacroCount == 0)
        lpstScan->wFlags &= ~WD_SCAN_FLAG_FULL_SET;

    // Apply virus signatures

    if (WDApplyVirusSignatures(&gstWD8SigSet,
                               lpstScan,
                               lplpstVirusSigInfo) == WD_STATUS_VIRUS_FOUND)
    {
        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(EXTSTATUS_VIRUS_FOUND);
    }

    // Check MVP

    if (bMVPApproved == FALSE)
    {
        *lplpstVirusSigInfo = &gstMVPVirusSigInfo;
        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(EXTSTATUS_VIRUS_FOUND);
    }

    // No virus

    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
    return (EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL97Scan()
//
// Description:
//  Scans a file for the presence of Excel 97 Macro viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS XL97Scan
(
    LPCALLBACKREV1          lpstCallBack,       // File op callbacks
    LPOLE_FILE              lpstOLEFile,        // OLE file structure
    LPBYTE                  lpbyStreamBATCache, // Non NULL if cache, 512 bytes
    LPVBA5_SCAN             lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 870 bytes
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
)
{
    LPWD_SCAN           lpstScan;
#if !defined(SYM_DOSX)
    LZNT_T              stLZNT;
#endif

    OLE_OPEN_SIB_T      stOpenSib;

    BOOL                bMVPApproved = TRUE;

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    BYTE abyNameSigInterMacroHit[XL97_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNameSigIntraMacroHit[XL97_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNamedCRCSigHit[XL97_NAMED_CRC_SIG_HIT_ARRAY_SIZE];
    BYTE abyMacroSigHit[XL97_MACRO_SIG_HIT_ARRAY_SIZE];
    BYTE abyCRCHit[XL97_CRC_HIT_ARRAY_SIZE];
#else
    LPBYTE              lpbyHitMem;
#endif

    lpstScan = (LPWD_SCAN)lpbyWorkBuffer;
    lpbyWorkBuffer += sizeof(WD_SCAN_T);

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    lpstScan->lpabyNameSigInterMacroHit = abyNameSigInterMacroHit;
    lpstScan->lpabyNameSigIntraMacroHit = abyNameSigIntraMacroHit;
    lpstScan->lpabyNamedCRCSigHit = abyNamedCRCSigHit;
    lpstScan->lpabyMacroSigHit = abyMacroSigHit;
    lpstScan->lpabyCRCHit = abyCRCHit;
#else
    if (AllocMacroSigHitMem(lpstCallBack,
                            &lpbyHitMem,
                            HIT_MEM_XL97) == FALSE)
        return(EXTSTATUS_MEM_ERROR);

    AssignMacroSigHitMem(lpbyHitMem,lpstScan);
#endif

    // Find the <VBA> storage

    if (VBA5OpenVBAStorage(lpstCallBack,
                           lpstOLEFile,
                           lpstVBA5Scan) == FALSE)
    {
        // No VBA storage!

        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(EXTSTATUS_OK);
    }

    // Initialize persistent members of the scan structure

    lpstScan->lpstCallBack = lpstCallBack;
    lpstScan->lpstOLEStream = lpstOLEFile;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;
    lpstScan->uScan.stVBA5.lpstLZNT = &stLZNT;

    // Initialize macro count

    lpstScan->wMacroCount = 0;

    // Assume a full set

    lpstScan->wFlags = WD_SCAN_FLAG_FULL_SET;

    WDInitHitBitArrays(&gstXL97SigSet,lpstScan);

    // awSibs[0] contains the index of the child of the VBA storage

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.nNumWaitingSibs = 1;
    stOpenSib.lpawSibs = (LPWORD)lpbyWorkBuffer;

    stOpenSib.lpawSibs[0] = lpstVBA5Scan->wVBAChildEntry;

    while (VBA5OpenCandidateModule(lpstCallBack,
                                   lpstOLEFile,
                                   lpstScan->abyName,
                                   lpbyStreamBATCache,
                                   &stOpenSib) == TRUE)
    {
        VBA5ScanModule(&gstXL97SigSet,lpstScan);

        if (VBA5LZNTStreamAttributeOnly(&stLZNT,
                                        lpstScan->abyRunBuf) == FALSE)
        {
            if (MVPCheck(MVP_XL97,
                         lpstScan->abyName,
                         lpstScan->dwCRC) == FALSE)
            {
                // Found a non-approved macro

                bMVPApproved = FALSE;
            }

            // The stream contains non-attribute lines

            if (lpstScan->wMacroCount < 0xFFFF)
                lpstScan->wMacroCount++;

            // Determine full set status

            if ((lpstScan->wFlags & WD_SCAN_FLAG_MACRO_IS_FULL_SET) == 0)
            {
                // Found a macro that was not part of the full set

                lpstScan->wFlags &= ~WD_SCAN_FLAG_FULL_SET;
            }
        }
    }

    // If macro count is zero, don't assume full set

    if (lpstScan->wMacroCount == 0)
        lpstScan->wFlags &= ~WD_SCAN_FLAG_FULL_SET;

    // Apply virus signatures

    if (WDApplyVirusSignatures(&gstXL97SigSet,
                               lpstScan,
                               lplpstVirusSigInfo) == WD_STATUS_VIRUS_FOUND)
    {
        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(EXTSTATUS_VIRUS_FOUND);
    }

    // Check MVP

    if (bMVPApproved == FALSE)
    {
        *lplpstVirusSigInfo = &gstMVPVirusSigInfo;
        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(EXTSTATUS_VIRUS_FOUND);
    }

    // No virus

    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
    return (EXTSTATUS_OK);
}




