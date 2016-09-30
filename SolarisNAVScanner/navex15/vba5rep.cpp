//************************************************************************
//
// $Header:   S:/NAVEX/VCS/vba5rep.cpv   1.11   10 Jul 1997 17:46:10   DDREW  $
//
// Description:
//      Contains Word 8.0/Excel 97 macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/vba5rep.cpv  $
// 
//    Rev 1.11   10 Jul 1997 17:46:10   DDREW
// Turn on NLM repairs for NAVEX15
// 
//    Rev 1.10   02 Jun 1997 18:51:48   DCHI
// Removed error return upon failure in deleting Module=MODULENAME line
// from project stream.
// 
//    Rev 1.9   27 May 1997 16:51:22   DCHI
// Modifications to support dynamic allocation of hit memory and new
// Office 97 CRC.
// 
//    Rev 1.8   15 May 1997 11:32:14   DCHI
// Made changes so that Class modules could be properly deleted and so
// that the Macros storage could be deletable even if ThisDocument was
// not present.
// 
//    Rev 1.7   13 May 1997 14:49:24   DCHI
// Corrected problem with MVP repair where crashing would result because
// MVP is a special case that doesn't have a repair signature.
// 
//    Rev 1.6   12 May 1997 16:41:24   DCHI
// Modifications so that MVP repair is always applied regardless of which
// virus was detected.
// 
//    Rev 1.5   08 May 1997 15:35:28   DCHI
// Added office 97 conversion of template to document and added criteria
// for absence of AutoText before converting to document for both Word versions.
// 
//    Rev 1.4   10 Apr 1997 16:51:36   DCHI
// Fixed FullSet checking.
// 
//    Rev 1.3   08 Apr 1997 12:40:40   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.2   07 Apr 1997 18:11:10   DCHI
// Added MVP support.
// 
//    Rev 1.1   18 Mar 1997 11:29:42   DCHI
// Changed record type used for eliminating VBA project record from
// 0xFFFF to 0x00CD.
// 
//    Rev 1.0   14 Mar 1997 16:34:34   DCHI
// Initial revision.
// 
//************************************************************************

#if defined(NAVEX15) || !defined(SYM_NLM)

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"

#include "ctsn.h"
#include "navex.h"

#include "endutils.h"

#include "wdscan.h"
#include "wdrepair.h"

#include "vba5scan.h"
#include "vba5rep.h"

#include "wd8sigs.h"
#include "xl97sigs.h"

#include "vba5dir.h"

#include "mvp.h"

#include "mcrhitmm.h"

//********************************************************************
//
// Function:
//  int VBA5FindProjectStreamsCB()
//
// Description:
//  Searches for the streams PROJECT and PROJECTwm.
//
// Returns:
//  OLE_OPEN_CB_STATUS_CONTINUE     Always
//
//********************************************************************

int VBA5FindProjectStreamsCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszPROJECT,
                     MAX_OLE_NAME_LEN) == 0)
        {
            ((LPVBA5_SCAN)lpvCookie)->wPROJECTEntry =
                (WORD)dwIndex;
        }
        else
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszPROJECTwm,
                     MAX_OLE_NAME_LEN) == 0)
        {
            ((LPVBA5_SCAN)lpvCookie)->wPROJECTwmEntry =
                (WORD)dwIndex;
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int VBA5FindVBAStreamsCB()
//
// Description:
//  Searches for the streams _VBA_PROJECT and dir.
//
// Returns:
//  OLE_OPEN_CB_STATUS_CONTINUE     Always
//
//********************************************************************

int VBA5FindVBAStreamsCB
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
            ((LPVBA5_SCAN)lpvCookie)->w_VBA_PROJECTEntry =
                (WORD)dwIndex;
        }
        else
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszdir,
                     MAX_OLE_NAME_LEN) == 0)
        {
            ((LPVBA5_SCAN)lpvCookie)->wdirEntry =
                (WORD)dwIndex;
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5FindVBAStreams
//
// Description:
//  On entering, lpstOLEOpenSib->lpawSibs[0] should be set to the
//  root sibling of the project streams.  The function modifies
//  the structure at lpstOLEOpenSib during the course of its
//  execution.
//
//  Finds the following streams of a VBA 5 project:
//      PROJECT
//      PROJECTwm
//      _VBA_PROJECT
//      dir
//
// Returns:
//
//
//********************************************************************

BOOL VBA5FindVBAStreams
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file structure
    LPOLE_OPEN_SIB  lpstOLEOpenSib,         // Sibling structure
    LPVBA5_SCAN     lpstVBA5Scan            // VBA5 project stream info
)
{
    /////////////////////////////////////////////////////////////
    // Find the PROJECT and PROJECTwm streams
    /////////////////////////////////////////////////////////////

    lpstVBA5Scan->wPROJECTEntry = 0xFFFF;
    lpstVBA5Scan->wPROJECTwmEntry = 0xFFFF;
    OLEOpenSiblingCB(lpstCallBack,
                     lpstOLEFile->hFile,
                     lpstOLEFile,
                     VBA5FindProjectStreamsCB,
                     (LPVOID)lpstVBA5Scan,
                     NULL,
                     lpstOLEOpenSib);

    if (lpstVBA5Scan->wPROJECTEntry == 0xFFFF ||
        lpstVBA5Scan->wPROJECTwmEntry == 0xFFFF)
    {
        // Failed to find one or more of the PROJECT streams

        return(FALSE);
    }


    /////////////////////////////////////////////////////////////
    // Find the _VBA_PROJECT and dir streams
    /////////////////////////////////////////////////////////////

    // Initialize sibling structure

    lpstOLEOpenSib->lpawSibs[0] = lpstVBA5Scan->wVBAChildEntry;
    lpstOLEOpenSib->nNumWaitingSibs = 1;

    // Search for desired streams

    lpstVBA5Scan->w_VBA_PROJECTEntry = 0xFFFF;
    lpstVBA5Scan->wdirEntry = 0xFFFF;
    OLEOpenSiblingCB(lpstCallBack,
                     lpstOLEFile->hFile,
                     lpstOLEFile,
                     VBA5FindVBAStreamsCB,
                     (LPVOID)lpstVBA5Scan,
                     NULL,
                     lpstOLEOpenSib);

    if (lpstVBA5Scan->w_VBA_PROJECTEntry == 0xFFFF ||
        lpstVBA5Scan->wdirEntry == 0xFFFF)
    {
        // Failed to find either or both of the _VBA_PROJECT
        //  and the dir stream

        return(FALSE);
    }

    // Found all the streams

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int WD8FindAssociatedStreamsCB()
//
// Description:
//  Searches for the streams WordDocument, 1Table, and the storage
//  Macros and sets the appropriate entry field in the
//  WD8_FIND_STREAMS_T structure if found.
//
// Returns:
//  OLE_OPEN_CB_STATUS_CONTINUE     Always
//
//********************************************************************

typedef struct tagWD8_FIND_STREAMS
{
    WORD        wWordDocumentEntry;
    WORD        w1TableEntry;
    WORD        w0TableEntry;
    WORD        wMacrosEntry;
} WD8_FIND_STREAMS_T, FAR *LPWD8_FIND_STREAMS;

int WD8FindAssociatedStreamsCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gpuszWordDocument,
                     MAX_OLE_NAME_LEN) == 0)
        {
            ((LPWD8_FIND_STREAMS)lpvCookie)->wWordDocumentEntry =
                (WORD)dwIndex;
        }
        else
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gausz1Table,
                     MAX_OLE_NAME_LEN) == 0)
        {
            ((LPWD8_FIND_STREAMS)lpvCookie)->w1TableEntry =
                (WORD)dwIndex;
        }
        else
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gausz0Table,
                     MAX_OLE_NAME_LEN) == 0)
        {
            ((LPWD8_FIND_STREAMS)lpvCookie)->w0TableEntry =
                (WORD)dwIndex;
        }
    }
    else
    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszMacros,
                     MAX_OLE_NAME_LEN) == 0)
        {
            ((LPWD8_FIND_STREAMS)lpvCookie)->wMacrosEntry =
                (WORD)dwIndex;
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8FindAssociatedStreams()
//
// Description:
//  The following fields of lpstVBA5Scan should be valid before
//  entering this function:
//
//          wMacrosEntry
//          wMacrosChildEntry
//          wVBAEntry
//          wVBAChildEntry
//
//  The function will fill in the following fields:
//
//          wParentStorageEntry
//          wWordDocumentEntry
//          w1TableEntry
//          w0TableEntry
//          wPROJECTEntry
//          wPROJECTwmEntry
//          w_VBA_PROJECTEntry
//          wdirEntry
//
//  The following algorithm is used:
//      1. Search for the WordDocument and Table streams:
//          For each substorage do the following:
//              Iterate through each sibling:
//                  Take note of the WordDocument stream index
//                  Take note of the Table stream index
//                  Take note of the Macros substorage index
//              If all three are present and the index of the
//              Macros substorage is the same as the one passed
//              in, then we are done.
//          If the associated WordDocument stream or Table stream
//              can not be found, then return failure.
//      2. Search for the PROJECT, PROJECTwm, _VBA_PROJECT, and
//          dir streams by calling the shared function to do so.
//
// Returns:
//  TRUE        If all of the associated streams were found
//  FALSE       If any of the streams could not be found
//
//********************************************************************

BOOL WD8FindAssociatedStreams
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file structure
    LPVBA5_SCAN     lpstVBA5Scan            // VBA5 project stream info
)
{
    WORD                wMagicNum;
    BOOL                bFound;
    OLE_OPEN_SIB_T      stOpenSib;
    WORD                awSibs[MAX_VBA5_SIB_DEPTH];
    WD8_FIND_STREAMS_T  stWD8FindStreams;

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = awSibs;

    /////////////////////////////////////////////////////////////
    // Find Word application specific streams
    /////////////////////////////////////////////////////////////

    // Go through all storages

    bFound = FALSE;
    wMagicNum = 0;
    while (OLEFindNextStorage(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              &awSibs[0],
                              &wMagicNum) == OLE_OK)
    {
        // Found a storage, so go through children looking
        //  for what we need

        stOpenSib.nNumWaitingSibs = 1;
        stWD8FindStreams.wWordDocumentEntry = 0xFFFF;
        stWD8FindStreams.w1TableEntry = 0xFFFF;
        stWD8FindStreams.w0TableEntry = 0xFFFF;
        stWD8FindStreams.wMacrosEntry = 0xFFFF;
        OLEOpenSiblingCB(lpstCallBack,
                         lpstOLEFile->hFile,
                         lpstOLEFile,
                         WD8FindAssociatedStreamsCB,
                         (LPVOID)&stWD8FindStreams,
                         NULL,
                         &stOpenSib);

        if (stWD8FindStreams.wMacrosEntry ==
                lpstVBA5Scan->u.stWD8.wMacrosEntry &&
            stWD8FindStreams.wWordDocumentEntry != 0xFFFF &&
            stWD8FindStreams.w1TableEntry != 0xFFFF)
        {
            // Also store the parent storage's entry number

            lpstVBA5Scan->u.stWD8.wParentStorageEntry =
                wMagicNum - 1;

            bFound = TRUE;
            break;
        }
    }

    if (bFound == FALSE)
    {
        // Failed finding associated Word application streams

        return(FALSE);
    }

    // Set the found entries in the VBA 5 scan structure

    lpstVBA5Scan->u.stWD8.wWordDocumentEntry =
        stWD8FindStreams.wWordDocumentEntry;

    lpstVBA5Scan->u.stWD8.w1TableEntry = stWD8FindStreams.w1TableEntry;
    lpstVBA5Scan->u.stWD8.w0TableEntry = stWD8FindStreams.w0TableEntry;


    /////////////////////////////////////////////////////////////
    // Find VBA 5 specific streams
    /////////////////////////////////////////////////////////////

    stOpenSib.nNumWaitingSibs = 1;
    awSibs[0] = lpstVBA5Scan->u.stWD8.wMacrosChildEntry;
    if (VBA5FindVBAStreams(lpstCallBack,
                           lpstOLEFile,
                           &stOpenSib,
                           lpstVBA5Scan) == FALSE)
    {
        // Failed finding associated VBA streams

        return(FALSE);
    }

    // Successfully got the indices of all important streams

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int XL97FindAssociatedStreamsCB()
//
// Description:
//  Searches for the Workbook stream and the storage
//  _VBA_PROJECT_CUR and sets the appropriate entry field in the
//  XL97_FIND_STREAMS_T structure if found.
//
// Returns:
//  OLE_OPEN_CB_STATUS_CONTINUE     Always
//
//********************************************************************

typedef struct tagXL97_FIND_STREAMS
{
    WORD        wWorkbookEntry;
    WORD        w_VBA_PROJECT_CUREntry;
} XL97_FIND_STREAMS_T, FAR *LPXL97_FIND_STREAMS;

int XL97FindAssociatedStreamsCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszWorkbook,
                     MAX_OLE_NAME_LEN) == 0)
        {
            ((LPXL97_FIND_STREAMS)lpvCookie)->wWorkbookEntry =
                (WORD)dwIndex;
        }
    }
    else
    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gausz_VBA_PROJECT_CUR,
                     MAX_OLE_NAME_LEN) == 0)
        {
            ((LPXL97_FIND_STREAMS)lpvCookie)->w_VBA_PROJECT_CUREntry =
                (WORD)dwIndex;
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL XL97FindAssociatedStreams()
//
// Description:
//  The following fields of lpstVBA5Scan should be valid before
//  entering this function:
//
//          w_VBA_PROJECT_CUREntry
//          w_VBA_PROJECT_CURChildEntry
//          wVBAEntry
//          wVBAChildEntry
//
//  The function will fill in the following fields:
//
//          wParentStorageEntry
//          wWorkbookEntry
//          wPROJECTEntry
//          wPROJECTwmEntry
//          w_VBA_PROJECTEntry
//          wdirEntry
//
//  The following algorithm is used:
//      1. Search for the Workbook stream:
//          For each substorage do the following:
//              Iterate through each sibling:
//                  Take note of the Workbook stream index
//                  Take note of the _VBA_PROJECT_CUR substorage index
//              If both are present and the index of the _VBA_PROJECT_CUR
//              substorage is the same as the one passed in, then we
//              are done.
//          If the associated Wordbook stream could not be found,
//              return failure.
//      2. Search for the PROJECT, PROJECTwm, _VBA_PROJECT, and
//          dir streams by calling the shared function to do so.
//
// Returns:
//  TRUE        If all of the associated streams were found
//  FALSE       If any of the streams could not be found
//
//********************************************************************

BOOL XL97FindAssociatedStreams
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // OLE file structure
    LPVBA5_SCAN     lpstVBA5Scan            // VBA5 project stream info
)
{
    WORD                wMagicNum;
    BOOL                bFound;
    OLE_OPEN_SIB_T      stOpenSib;
    WORD                awSibs[MAX_VBA5_SIB_DEPTH];
    XL97_FIND_STREAMS_T stXL97FindStreams;

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = awSibs;

    /////////////////////////////////////////////////////////////
    // Find Word application specific streams
    /////////////////////////////////////////////////////////////

    // Go through all storages

    bFound = FALSE;
    wMagicNum = 0;
    while (OLEFindNextStorage(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              &awSibs[0],
                              &wMagicNum) == OLE_OK)
    {
        // Found a storage, so go through children looking
        //  for what we need

        stOpenSib.nNumWaitingSibs = 1;
        stXL97FindStreams.wWorkbookEntry = 0xFFFF;
        stXL97FindStreams.w_VBA_PROJECT_CUREntry = 0xFFFF;
        OLEOpenSiblingCB(lpstCallBack,
                         lpstOLEFile->hFile,
                         lpstOLEFile,
                         XL97FindAssociatedStreamsCB,
                         (LPVOID)&stXL97FindStreams,
                         NULL,
                         &stOpenSib);

        if (stXL97FindStreams.w_VBA_PROJECT_CUREntry ==
                lpstVBA5Scan->u.stXL97.w_VBA_PROJECT_CUREntry &&
            stXL97FindStreams.wWorkbookEntry != 0xFFFF)
        {
            // Also store the parent storage's entry number

            lpstVBA5Scan->u.stXL97.wParentStorageEntry =
                wMagicNum - 1;

            bFound = TRUE;
            break;
        }
    }

    if (bFound == FALSE)
    {
        // Failed finding associated Word application streams

        return(FALSE);
    }

    // Set the found entries in the VBA 5 scan structure

    lpstVBA5Scan->u.stXL97.wWorkbookEntry =
        stXL97FindStreams.wWorkbookEntry;


    /////////////////////////////////////////////////////////////
    // Find VBA 5 specific streams
    /////////////////////////////////////////////////////////////

    stOpenSib.nNumWaitingSibs = 1;
    awSibs[0] = lpstVBA5Scan->u.stXL97.w_VBA_PROJECT_CURChildEntry;
    if (VBA5FindVBAStreams(lpstCallBack,
                           lpstOLEFile,
                           &stOpenSib,
                           lpstVBA5Scan) == FALSE)
    {
        // Failed finding associated VBA streams

        return(FALSE);
    }

    // Successfully got the indices of all important streams

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5RefTableCmp()
//
// Description:
//  Performs a comparison of a single byte buffer against a single
//  byte buffer/double byte buffer.
//
// Returns:
//  TRUE        If the buffers are equal
//  FALSE       If the buffers are different
//
//********************************************************************

BOOL VBA5RefTableCmpI
(
    LPBYTE      lpbyMem0,
    int         nMem0Len,
    LPBYTE      lpbyMem1,
    int         nMem1Len
)
{
    BYTE        by0, by1;

    // Perform the single byte comparison

    if (nMem0Len == nMem1Len)
    {
        int         n;

        for (n=0;n<nMem0Len;n++)
            if (lpbyMem0[n] != lpbyMem1[n])
                return(FALSE);

        // Single byte comparison succeeded

        return(TRUE);
    }

    // Perform the double byte comparison

    if (nMem0Len + nMem0Len == nMem1Len)
    {
        while (nMem0Len-- != 0)
        {
            by0 = *lpbyMem0++;
            by1 = *lpbyMem1++;

            if ('a' <= by0 && by0 <= 'z')
                by0 = by0 - 'a' + 'A';

            if ('a' <= by1 && by1 <= 'z')
                by1 = by1 - 'a' + 'A';

            // The second byte must be zero

            if (by0 != by1)
                return(FALSE);

            if (*lpbyMem1++ != 0)
                return(FALSE);
        }

        return(TRUE);
    }

    // No match

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5PROJECTRemoveModule()
//
// Description:
//  Opens the PROJECT stream and removes the lines containing
//  Module=MODULENAME and MODULENAME={coordinates} from it.
//  The function returns error if the lines containing
//  MODULENAME are not found.  The MODULENAME comparison is
//  a case insensitive search.
//
//  The function also zeroes out the left over bytes of the
//  stream and truncates the stream if lines were deleted.
//
//  The function assumes that lines will never be greater than
//  1024 bytes and that a 0x0D 0x0A ends a line.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL VBA5PROJECTRemoveModule
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // Non NULL if cache
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwOffset;
    DWORD               dwStreamLen;
    DWORD               dwModuleEqualStartOffset;
    DWORD               dwModuleEqualEndOffset;
    DWORD               dwMODULENAMEStartOffset;
    DWORD               dwMODULENAMEEndOffset;
    int                 nDesiredLineLen;
    int                 nDesiredLineLen2;
    int                 nModuleNameLen;
    int                 i, nNextI;
    BOOL                bFound;
    int                 nNumBytes;

    // Open the PROJECT stream

    if (OLEOpenStreamEntryNum(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              lpstVBA5Scan->wPROJECTEntry,
                              lpbyStreamBATCache) != OLE_OK)
    {
        // Failed to open PROJECT stream

        return(FALSE);
    }

    dwOffset = 0;
    dwStreamLen = OLEStreamLen(lpstOLEFile);

    // Determine the length of the module name

    nModuleNameLen = 0;
    while (abyModuleName[nModuleNameLen])
        ++nModuleNameLen;


    /////////////////////////////////////////////////////////////
    // Search for "Module=" or "BaseClass="
    /////////////////////////////////////////////////////////////

    bFound = FALSE;
    nDesiredLineLen = 7 + nModuleNameLen; // 7 = strlen("Module=")
    nDesiredLineLen2 = 10 + nModuleNameLen; // 10 = strlen("BaseClass=")
    while (bFound == FALSE && dwOffset < dwStreamLen)
    {
        if (dwStreamLen - dwOffset < 512)
            nNumBytes = (int)(dwStreamLen - dwOffset);
        else
            nNumBytes = 512;

        // This read should always occur at the beginning of a line

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        lpbyWorkBuffer,
                        (WORD)nNumBytes) != (WORD)nNumBytes)
        {
            return(FALSE);
        }

        // Iterate through lines

        i = 0;
        while (bFound == FALSE)
        {
            // Search for the end of the line

            for (nNextI=i;nNextI<nNumBytes;nNextI++)
            {
                if (lpbyWorkBuffer[nNextI] == 0x0D)
                    break;
            }

            // Was the end of the line found?

            if (nNextI < nNumBytes)
            {
                // Found the end of a line

                // Is the line the right length?

                if (nNextI - i == nDesiredLineLen)
                {
                    // Is it a "Module=MODULENAME" line?

                    if (VBA5MemICmp((LPBYTE)"Module=",
                                    lpbyWorkBuffer+i,
                                    7) == TRUE &&
                        VBA5MemICmp(abyModuleName,
                                    lpbyWorkBuffer+i+7,
                                    nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwModuleEqualStartOffset = dwOffset + i;
                        dwModuleEqualEndOffset = dwOffset + nNextI + 2;
                    }
                }
                else
                if (nNextI - i == nDesiredLineLen - 1)
                {
                    // Is it a "Class=MODULENAME" line?

                    if (VBA5MemICmp((LPBYTE)"Class=",
                                    lpbyWorkBuffer+i,
                                    6) == TRUE &&
                        VBA5MemICmp(abyModuleName,
                                    lpbyWorkBuffer+i+6,
                                    nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwModuleEqualStartOffset = dwOffset + i;
                        dwModuleEqualEndOffset = dwOffset + nNextI + 2;
                    }
                }
                else
                if (nNextI - i == nDesiredLineLen2)
                {
                    // Is it a "BaseClass=MODULENAME" line?

                    if (VBA5MemICmp((LPBYTE)"BaseClass=",
                                    lpbyWorkBuffer+i,
                                    10) == TRUE &&
                        VBA5MemICmp(abyModuleName,
                                    lpbyWorkBuffer+i+10,
                                    nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwModuleEqualStartOffset = dwOffset + i;
                        dwModuleEqualEndOffset = dwOffset + nNextI + 2;
                    }
                }
                else
                {
                    // Is it a "Document=MODULENAME" line?

                    if (VBA5MemICmp((LPBYTE)"Document=",
                                    lpbyWorkBuffer+i,
                                    9) == TRUE &&
                        VBA5MemICmp(abyModuleName,
                                    lpbyWorkBuffer+i+9,
                                    nModuleNameLen) == TRUE &&
                        lpbyWorkBuffer[i+9+nModuleNameLen] == '/')
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwModuleEqualStartOffset = dwOffset + i;
                        dwModuleEqualEndOffset = dwOffset + nNextI + 2;
                    }
                }

                i = nNextI + 2;
            }
            else
            {
                // Begin a new read

                if (i > 0)
                    dwOffset += i;
                else
                {
                    // Line > 512 bytes?

                    dwOffset = dwStreamLen;
                }

                break;
            }
        }
    }

    if (bFound == FALSE)
    {
        // Failed to find "Module=MODULENAME" line

        dwModuleEqualEndOffset = dwModuleEqualStartOffset = 0;
    }


    /////////////////////////////////////////////////////////////
    // Search for [Workspace]
    /////////////////////////////////////////////////////////////

    bFound = FALSE;
    nDesiredLineLen = 11; // 11 = strlen("[Workspace]")
    dwOffset = dwModuleEqualEndOffset;
    while (bFound == FALSE && dwOffset < dwStreamLen)
    {
        if (dwStreamLen - dwOffset < 512)
            nNumBytes = (int)(dwStreamLen - dwOffset);
        else
            nNumBytes = 512;

        // This read should always occur at the beginning of a line

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        lpbyWorkBuffer,
                        (WORD)nNumBytes) != (WORD)nNumBytes)
        {
            return(FALSE);
        }

        // Iterate through lines

        i = 0;
        while (bFound == FALSE)
        {
            // Search for the end of the line

            for (nNextI=i;nNextI<nNumBytes;nNextI++)
            {
                if (lpbyWorkBuffer[nNextI] == 0x0D)
                    break;
            }

            // Was the end of the line found?

            if (nNextI < nNumBytes)
            {
                // Found the end of a line

                // Is the line the right length?

                if (nNextI - i == nDesiredLineLen)
                {
                    // Is it the "[Workspace]" line?

                    if (VBA5MemICmp((LPBYTE)"[Workspace]",
                                    lpbyWorkBuffer+i,
                                    11) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwOffset += i + nDesiredLineLen + 2;
                    }
                }

                i = nNextI + 2;
            }
            else
            {
                // Begin a new read

                if (i > 0)
                    dwOffset += i;
                else
                {
                    // Line > 512 bytes?

                    dwOffset = dwStreamLen;
                }

                break;
            }
        }
    }

    if (bFound == TRUE)
    {
        /////////////////////////////////////////////////////////////
        // Search for MODULENAME=
        /////////////////////////////////////////////////////////////

        // dwOffset should be pointing to the first line after the
        //  line containing "[Workspace]"

        bFound = FALSE;
        nDesiredLineLen = nModuleNameLen + 1; // 1 = strlen("=")
        while (bFound == FALSE && dwOffset < dwStreamLen)
        {
            if (dwStreamLen - dwOffset < 512)
                nNumBytes = (int)(dwStreamLen - dwOffset);
            else
                nNumBytes = 512;

            // This read should always occur at the beginning of a line

            if (OLESeekRead(lpstCallBack,
                            lpstOLEFile,
                            dwOffset,
                            lpbyWorkBuffer,
                            (WORD)nNumBytes) != (WORD)nNumBytes)
            {
                return(FALSE);
            }

            // Iterate through lines

            i = 0;
            while (bFound == FALSE)
            {
                // Search for the end of the line

                for (nNextI=i;nNextI<nNumBytes;nNextI++)
                {
                    if (lpbyWorkBuffer[nNextI] == 0x0D)
                        break;
                }

                // Was the end of the line found?

                if (nNextI < nNumBytes)
                {
                    // Found the end of a line

                    // Is the line the right length?

                    if (nNextI - i >= nDesiredLineLen)
                    {
                        // Is it the "MODULENAME=" line?

                        if (VBA5MemICmp(abyModuleName,
                                        lpbyWorkBuffer+i,
                                        nModuleNameLen) == TRUE &&
                            lpbyWorkBuffer[i+nModuleNameLen] == '=')
                        {
                            // Bingo!

                            bFound = TRUE;
                            dwMODULENAMEStartOffset = dwOffset + i;
                            dwMODULENAMEEndOffset = dwOffset + nNextI + 2;
                        }
                    }

                    i = nNextI + 2;
                }
                else
                {
                    // Begin a new read

                    if (i > 0)
                        dwOffset += i;
                    else
                    {
                        // Line > 512 bytes?

                        dwOffset = dwStreamLen;
                    }

                    break;
                }
            }
        }
    }

    if (bFound == FALSE)
    {
        // No MODULENAME= line

        dwMODULENAMEStartOffset = dwModuleEqualEndOffset;
        dwMODULENAMEEndOffset = dwModuleEqualEndOffset;
    }

    /////////////////////////////////////////////////////////////
    // Delete the found lines
    /////////////////////////////////////////////////////////////

    // Delete the "Module=MODULENAME" line

    if (dwModuleEqualEndOffset != dwModuleEqualStartOffset)
    {
        if (OLECopyBytes(lpstCallBack,
                         lpstOLEFile,
                         dwModuleEqualEndOffset,    // src
                         dwModuleEqualStartOffset,  // dst
                         dwMODULENAMEStartOffset - dwModuleEqualEndOffset,
                         lpbyWorkBuffer) == FALSE)
        {
            // Failed!

            return(FALSE);
        }
    }

    dwOffset = dwMODULENAMEStartOffset -
        (dwModuleEqualEndOffset - dwModuleEqualStartOffset);

    // Delete the "MODULENAME=" line

    if (OLECopyBytes(lpstCallBack,
                     lpstOLEFile,
                     dwMODULENAMEEndOffset,     // src
                     dwOffset,                  // dst
                     dwStreamLen - dwMODULENAMEEndOffset,
                     lpbyWorkBuffer) == FALSE)
    {
        // Failed!

        return(FALSE);
    }

    // Calculate the new length

    dwOffset = dwStreamLen -
        (dwModuleEqualEndOffset - dwModuleEqualStartOffset) -
        (dwMODULENAMEEndOffset - dwMODULENAMEStartOffset);

    // Zero out the remaining bytes

    if (OLEWriteZeroes(lpstCallBack,
                       lpstOLEFile,
                       dwOffset,
                       dwStreamLen - dwOffset,
                       lpbyWorkBuffer) == FALSE)
    {
        // Failed!

        return(FALSE);
    }

    // Truncate the file

    if (OLESetStreamLen(lpstCallBack,
                        lpstOLEFile,
                        OLEStreamEntryNum(lpstOLEFile),
                        dwOffset) == FALSE)
    {
        // Failed!

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5PROJECTwmRemoveModule()
//
// Description:
//  Opens the PROJECTwm stream and removes the lines single
//  byte and double byte strings of the MODULENAME.
//
//  The function also zeroes out the left over bytes of the
//  stream and truncates the stream if lines were deleted.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL VBA5PROJECTwmRemoveModule
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // Non NULL if cache
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwOffset;
    DWORD               dwStreamLen;
    DWORD               dwStartOffset;
    DWORD               dwEndOffset;
    int                 nSingleByteStrLen;
    int                 nModuleNameLen;
    int                 i, nNextI;
    BOOL                bFound;
    int                 nNumBytes;

    // Open the PROJECTwm stream

    if (OLEOpenStreamEntryNum(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              lpstVBA5Scan->wPROJECTwmEntry,
                              lpbyStreamBATCache) != OLE_OK)
    {
        // Failed to open PROJECTwm stream

        return(FALSE);
    }

    dwOffset = 0;
    dwStreamLen = OLEStreamLen(lpstOLEFile);

    // Determine the length of the module name

    nModuleNameLen = 0;
    while (abyModuleName[nModuleNameLen])
        ++nModuleNameLen;


    /////////////////////////////////////////////////////////////
    // Search for the single byte version of the string
    /////////////////////////////////////////////////////////////

    bFound = FALSE;
    while (bFound == FALSE && dwOffset < dwStreamLen)
    {
        if (dwStreamLen - dwOffset < 512)
            nNumBytes = (int)(dwStreamLen - dwOffset);
        else
            nNumBytes = 512;

        // This read should always occur at the beginning of a
        //  single byte string

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        lpbyWorkBuffer,
                        (WORD)nNumBytes) != (WORD)nNumBytes)
        {
            return(FALSE);
        }

        // Iterate through lines

        i = 0;
        while (bFound == FALSE)
        {
            // Search for the end of the single byte string

            for (nNextI=i;nNextI<nNumBytes;nNextI++)
            {
                if (lpbyWorkBuffer[nNextI] == 0)
                    break;
            }

            if (nNextI < nNumBytes)
            {
                nSingleByteStrLen = nNextI - i;
                ++nNextI;

                // Search for the end of the double byte string

                while (nNextI < nNumBytes)
                {
                    if (nNextI < nNumBytes)
                    {
                        if (lpbyWorkBuffer[nNextI++] == 0)
                        {
                            if (nNextI < nNumBytes)
                            {
                                if (lpbyWorkBuffer[nNextI] == 0)
                                    break;
                            }
                        }

                        ++nNextI;
                    }
                }

                if (nNextI < nNumBytes)
                {
                    ++nNextI;

                    // Found the end of the double byte string

                    // See if this is the set of strings

                    if (nSingleByteStrLen == nModuleNameLen &&
                        VBA5MemICmp(abyModuleName,
                                    lpbyWorkBuffer+i,
                                    nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bFound = TRUE;
                        dwStartOffset = dwOffset + i;
                        dwEndOffset = dwOffset + nNextI;
                    }
                }
            }

            if (nNextI >= nNumBytes)
            {
                // Begin a new read

                if (i > 0)
                    dwOffset += i;
                else
                {
                    // String > 512 bytes?

                    dwOffset = dwStreamLen;
                }

                break;
            }

            i = nNextI;
        }
    }

    if (bFound == FALSE)
    {
        // Failed to find string

        return(FALSE);
    }


    /////////////////////////////////////////////////////////////
    // Delete the found strings
    /////////////////////////////////////////////////////////////

    if (OLECopyBytes(lpstCallBack,
                     lpstOLEFile,
                     dwEndOffset,       // src
                     dwStartOffset,     // dst
                     dwStreamLen - dwEndOffset,
                     lpbyWorkBuffer) == FALSE)
    {
        // Failed!

        return(FALSE);
    }

    // Calculate the new length

    dwOffset = dwStreamLen - (dwEndOffset - dwStartOffset);

    // Zero out the remaining bytes

    if (OLEWriteZeroes(lpstCallBack,
                       lpstOLEFile,
                       dwOffset,
                       dwStreamLen - dwOffset,
                       lpbyWorkBuffer) == FALSE)
    {
        // Failed!

        return(FALSE);
    }

    // Truncate the file

    if (OLESetStreamLen(lpstCallBack,
                        lpstOLEFile,
                        OLEStreamEntryNum(lpstOLEFile),
                        dwOffset) == FALSE)
    {
        // Failed!

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5dirRemoveModule()
//
// Description:
//  Opens the dir stream and removes the records pertaining
//  to the given module.
//
//  The function also zeroes out the left over bytes of the
//  stream and truncates the stream if lines were deleted.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL VBA5dirRemoveModule
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // Non NULL if cache
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwOrigFileSize;
    DWORD               dwNewDirOffset;
    DWORD               dwOldDirOffset;
#if !defined(SYM_DOSX)
    LZNT_T              stLZNT;
#endif

    BOOL                bEOF;
    WORD                wType;
    DWORD               dwDataSize;

    WORD                wModuleCount;

    BOOL                bSkippingModuleRecords;
    int                 nModuleNameLen;

    DWORD               dwTemp;
    WORD                wTemp;

    DWORD               dwCompressedStreamSize;

    // Open the dir stream

    if (OLEOpenStreamEntryNum(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              lpstVBA5Scan->wdirEntry,
                              lpbyStreamBATCache) != OLE_OK)
    {
        // Failed to open dir stream

        return(FALSE);
    }

    // Initialize LZNT structure for reading of the old dir stream

    if (VBA5LZNTInit(lpstCallBack,
                     lpstOLEFile,
                     &stLZNT) == FALSE)
    {
        // Failed to initialize for reading of compressed dir stream

        return(FALSE);
    }

    // Get the original size of the file for truncation at the end

    dwOrigFileSize = lpstCallBack->FileSeek(lpstOLEFile->hFile,
                                            0,
                                            SEEK_END);

    if (dwOrigFileSize == (DWORD)-1)
    {
        // Error getting original file size

        return(FALSE);
    }

    // Determine the length of the module name

    nModuleNameLen = 0;
    while (abyModuleName[nModuleNameLen])
        ++nModuleNameLen;


    /////////////////////////////////////////////////////////////
    // Start writing the new uncompressed dir copy at the end
    /////////////////////////////////////////////////////////////

    dwNewDirOffset = dwOrigFileSize;

    bEOF = FALSE;
    dwOldDirOffset = 0;
    bSkippingModuleRecords = FALSE;
    while (bEOF == FALSE)
    {
        // Get the record type

        if (LZNTGetBytes(&stLZNT,
                         dwOldDirOffset,
                         sizeof(WORD),
                         (LPBYTE)&wType) != sizeof(WORD))
        {
            // Failed getting type of record

            return(FALSE);
        }

        dwOldDirOffset += sizeof(WORD);

        // Get the record data size

        if (LZNTGetBytes(&stLZNT,
                         dwOldDirOffset,
                         sizeof(DWORD),
                         (LPBYTE)&dwDataSize) != sizeof(DWORD))
        {
            // Failed getting type of record

            return(FALSE);
        }

        dwOldDirOffset += sizeof(DWORD);

        // Endianize

        wType = ConvertEndianShort(wType);
        dwDataSize = ConvertEndianLong(dwDataSize);

        switch (wType)
        {
            case BIN_PROJ_MODULECOUNT:
            {
                /////////////////////////////////////////////////
                // Get the module count and decrement it
                /////////////////////////////////////////////////

                if (dwDataSize != sizeof(WORD))
                {
                    // Error

                    return(FALSE);
                }

                // Read the module count

                if (LZNTGetBytes(&stLZNT,
                                 dwOldDirOffset,
                                 sizeof(WORD),
                                 (LPBYTE)&wModuleCount) != sizeof(WORD))
                {
                    // Failed getting type of record

                    return(FALSE);
                }

                dwOldDirOffset += sizeof(WORD);

                // Decrement the module count

                if (wModuleCount == 0)
                {
                    // Fatal error

                    return(FALSE);
                }

                wModuleCount = ConvertEndianShort(wModuleCount);
                --wModuleCount;
                wModuleCount = ConvertEndianShort(wModuleCount);

                break;
            }

            case BIN_MOD_NAME:
            {
                if (dwDataSize == nModuleNameLen)
                {
                    if (LZNTGetBytes(&stLZNT,
                                     dwOldDirOffset,
                                     (WORD)nModuleNameLen,
                                     lpbyWorkBuffer) != (WORD)nModuleNameLen)
                    {
                        // Failed reading module name

                        return(FALSE);
                    }

                    // This could be the module

                    if (VBA5MemICmp(abyModuleName,
                                    lpbyWorkBuffer,
                                    nModuleNameLen) == TRUE)
                    {
                        // Bingo!

                        bSkippingModuleRecords = TRUE;
                    }
                }

                break;
            }

            case BIN_PROJ_EOF:
                bEOF = TRUE;
                break;

            default:
                break;
        }

        /////////////////////////////////////////////////////////
        // Skip record if part of module records to delete
        /////////////////////////////////////////////////////////

        if (bSkippingModuleRecords == FALSE)
        {
            if (lpstCallBack->FileSeek(lpstOLEFile->hFile,
                                       dwNewDirOffset,
                                       SEEK_SET) != dwNewDirOffset)
            {
                return(FALSE);
            }

            // Write type

            wTemp = ConvertEndianShort(wType);
            if (lpstCallBack->FileWrite(lpstOLEFile->hFile,
                                        (LPVOID)&wTemp,
                                        sizeof(WORD)) != sizeof(WORD))
            {
                return(FALSE);
            }

            // Write data size

            dwTemp = ConvertEndianLong(dwDataSize);
            if (lpstCallBack->FileWrite(lpstOLEFile->hFile,
                                        (LPVOID)&dwTemp,
                                        sizeof(DWORD)) != sizeof(DWORD))
            {
                return(FALSE);
            }

            // Update offset in new dir stream

            dwNewDirOffset += sizeof(WORD) + sizeof(DWORD);

            // Write data

            if (wType == BIN_PROJ_MODULECOUNT)
            {
                // Write updated module count which should have
                //  already been endianized

                if (lpstCallBack->FileSeek(lpstOLEFile->hFile,
                                           dwNewDirOffset,
                                           SEEK_SET) != dwNewDirOffset)
                {
                    return(FALSE);
                }

                // Write type

                if (lpstCallBack->FileWrite(lpstOLEFile->hFile,
                                            (LPVOID)&wModuleCount,
                                            sizeof(WORD)) != sizeof(WORD))
                {
                    return(FALSE);
                }

                dwNewDirOffset += sizeof(WORD);
            }
            else
            {
                if (wType == BIN_PROJ_VERSION)
                    dwDataSize += sizeof(WORD);

                // Write in 512 byte chunks

                wTemp = 512;
                while (dwDataSize != 0)
                {
                    if (dwDataSize < 512)
                        wTemp = (WORD)dwDataSize;

                    // Read chunk

                    if (LZNTGetBytes(&stLZNT,
                                     dwOldDirOffset,
                                     (int)wTemp,
                                     lpbyWorkBuffer) != (int)wTemp)
                    {
                        // Failed reading chunk

                        return(FALSE);
                    }

                    // Write chunk

                    if (lpstCallBack->FileSeek(lpstOLEFile->hFile,
                                               dwNewDirOffset,
                                               SEEK_SET) != dwNewDirOffset)
                    {
                        return(FALSE);
                    }

                    if (lpstCallBack->FileWrite(lpstOLEFile->hFile,
                                                (LPVOID)lpbyWorkBuffer,
                                                wTemp) != wTemp)
                    {
                        return(FALSE);
                    }

                    dwOldDirOffset += wTemp;
                    dwNewDirOffset += wTemp;
                    dwDataSize -= wTemp;
                }
            }
        }
        else
        {
            if (wType == BIN_MOD_END)
                bSkippingModuleRecords = FALSE;

            if (wType == BIN_PROJ_VERSION)
                dwDataSize += sizeof(WORD);

            dwOldDirOffset += dwDataSize;
        }
    }


    /////////////////////////////////////////////////////////////
    // Compress new dir stream into old dir stream
    /////////////////////////////////////////////////////////////

    if (LZNTCopyCompress(&stLZNT,
                         lpstOLEFile->hFile,
                         dwOrigFileSize,    // Start offset
                         dwNewDirOffset-dwOrigFileSize,
                         &dwCompressedStreamSize) == FALSE)
    {
        // Failed to compress and write new dir stream

        return(FALSE);
    }

    // Set size of dir stream

    if (OLESetStreamLen(lpstCallBack,
                        lpstOLEFile,
                        OLEStreamEntryNum(lpstOLEFile),
                        dwCompressedStreamSize) == FALSE)
    {
        // Failed!

        return(FALSE);
    }

    // Truncate

    if (lpstCallBack->FileSeek(lpstOLEFile->hFile,
                               dwOrigFileSize,
                               SEEK_SET) != dwOrigFileSize)
    {
        // Failed to seek to end of original file

        return(FALSE);
    }

    if (lpstCallBack->FileWrite(lpstOLEFile->hFile,
                                (LPVOID)lpbyWorkBuffer,
                                0) != 0)
    {
        // Failed to truncate

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5DeleteModule()
//
// Description:
//  Deletes a module from a VBA document.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL VBA5DeleteModule
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // non NULL if cache
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    // Zero out the module's stream

    if (OLEWriteZeroes(lpstCallBack,
                       lpstOLEFile,
                       0,
                       OLEStreamLen(lpstOLEFile),
                       lpbyWorkBuffer) == FALSE)
    {
        // Failed to zero out stream

        return(FALSE);
    }

    // Delete the module's stream

    if (OLEUnlinkEntry(lpstCallBack,
                       lpstOLEFile,
                       lpstVBA5Scan->wVBAEntry,
                       (WORD)OLEStreamEntryNum(lpstOLEFile)) == FALSE)
    {
        // Failed to unlink entry

        return(FALSE);
    }

    // Remove lines from the PROJECT stream

    if (VBA5PROJECTRemoveModule(lpstCallBack,
                                lpstOLEFile,
                                lpbyStreamBATCache,
                                abyModuleName,
                                lpstVBA5Scan,
                                lpbyWorkBuffer) == FALSE)
    {
        // Failed to update PROJECT stream

        return(FALSE);
    }

    // Remove strings from PROJECTwm stream

    if (VBA5PROJECTwmRemoveModule(lpstCallBack,
                                  lpstOLEFile,
                                  lpbyStreamBATCache,
                                  abyModuleName,
                                  lpstVBA5Scan,
                                  lpbyWorkBuffer) == FALSE)
    {
        // Failed to update PROJECTwm stream

        return(FALSE);
    }

    // Remove module info from dir stream

    if (VBA5dirRemoveModule(lpstCallBack,
                            lpstOLEFile,
                            lpbyStreamBATCache,
                            abyModuleName,
                            lpstVBA5Scan,
                            lpbyWorkBuffer) == FALSE)
    {
        // Failed to update dir stream

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int VBA5OLEOpen__SRP_CB()
//
// Description:
//  Checks the entry to see if it is a stream with a name
//  that begins with __SRP_.  If so then the function returns
//  OLE_OPEN_CB_STATUS_RETURN.
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If the entry's name is __SRP_*
//  OLE_OPEN_CB_STATUS_CONTINUE     If the entry is not
//
//********************************************************************

BYTE gausz__SRP_[] = { '_', 0, '_', 0, 'S', 0, 'R', 0, 'P', 0, '_', 0, 0, 0 };

int VBA5OLEOpen__SRP_CB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gausz__SRP_,
                     6) == 0)
        {
            // Found a stream whose name begins with __SRP_

            return(OLE_OPEN_CB_STATUS_RETURN);
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int VBA5OLEOpen__SRP_0CB()
//
// Description:
//  Checks the entry to see if it is a stream with the name
//  __SRP_0.  If so then the function returns
//  OLE_OPEN_CB_STATUS_RETURN.
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If the entry's name is __SRP_0
//  OLE_OPEN_CB_STATUS_CONTINUE     If the entry is not
//
//********************************************************************

BYTE gausz__SRP_0[] = { '_', 0, '_', 0, 'S', 0, 'R', 0, 'P', 0, '_', 0, '0', 0, 0, 0 };

int VBA5OLEOpen__SRP_0CB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gausz__SRP_0,
                     MAX_OLE_NAME_LEN) == 0)
        {
            // Found a stream whose name begins with __SRP_0

            return(OLE_OPEN_CB_STATUS_RETURN);
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5UpdateForRecompilation()
//
// Description:
//  Deletes a module from a Word 8.0 document.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL VBA5UpdateForRecompilation
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // non NULL if cache
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    OLE_OPEN_SIB_T      stOpenSib;
    WORD                awSibs[MAX_VBA5_SIB_DEPTH];
    WORD                wSecondUSHORT;

    /////////////////////////////////////////////////////////////
    // Delete __SRP_0 stream
    /////////////////////////////////////////////////////////////

    // Get updated child of VBA storage

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = awSibs;
    awSibs[0] = lpstVBA5Scan->wVBAChildEntry;
    stOpenSib.nNumWaitingSibs = 1;
    if (OLEOpenSiblingCB(lpstCallBack,
                         lpstOLEFile->hFile,
                         lpstOLEFile,
                         VBA5OLEOpen__SRP_0CB,
                         NULL,
                         lpbyStreamBATCache,
                         &stOpenSib) == OLE_OK)
    {
        // Zero out stream

        if (OLEWriteZeroes(lpstCallBack,
                           lpstOLEFile,
                           0,
                           OLEStreamLen(lpstOLEFile),
                           lpbyWorkBuffer) == FALSE)
        {
            // Failed to zero out stream

            return(FALSE);
        }

        // Unlink stream

        if (OLEUnlinkEntry(lpstCallBack,
                           lpstOLEFile,
                           lpstVBA5Scan->wVBAEntry,
                           (WORD)OLEStreamEntryNum(lpstOLEFile)) == FALSE)
        {
            // Failed to unlink entry

            return(FALSE);
        }

        // Update the VBA child entry number

        if (OLEGetChildOfStorage(lpstCallBack,
                                 lpstOLEFile->hFile,
                                 lpstOLEFile,
                                 lpstVBA5Scan->wVBAEntry,
                                 &lpstVBA5Scan->wVBAChildEntry) == FALSE)
        {
            // Error getting the child of the VBA storage

            return(FALSE);
        }
    }

    /////////////////////////////////////////////////////////////
    // Increment second ushort of _VBA_PROJECT stream
    /////////////////////////////////////////////////////////////

    // Open the _VBA_PROJECT stream

    if (OLEOpenStreamEntryNum(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              lpstVBA5Scan->w_VBA_PROJECTEntry,
                              lpbyStreamBATCache) != OLE_OK)
    {
        // Failed to open _VBA_PROJECT stream

        return(FALSE);
    }

    // Read the second ushort

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    2,
                    (LPBYTE)&wSecondUSHORT,
                    sizeof(WORD)) != sizeof(WORD))
    {
        // Failed to read ushort

        return(FALSE);
    }

    // Increment its value

    wSecondUSHORT = ConvertEndianShort(wSecondUSHORT);
    ++wSecondUSHORT;
    wSecondUSHORT = ConvertEndianShort(wSecondUSHORT);

    // Write back the incremented second ushort

    if (OLESeekWrite(lpstCallBack,
                     lpstOLEFile,
                     2,
                     (LPBYTE)&wSecondUSHORT,
                     sizeof(WORD)) != sizeof(WORD))
    {
        // Failed to write ushort

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL VBA5CountNonEmptyModules()
//
// Description:
//  Iterates through each module in the given VBA substorage
//  and increments the module count if it contains lines other
//  than Attribute lines.
//
// Returns:
//  TRUE        On success
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL VBA5CountNonEmptyModules
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    WORD                wVBAChildEntry,     // Entry number of VBA child
    LPWORD              lpwModuleCount,     // Ptr to WORD for count
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 0.5K bytes
)
{
    OLE_OPEN_SIB_T      stOpenSib;
#if !defined(SYM_DOSX)
    LZNT_T              stLZNT;
#endif
    WORD                awSibs[MAX_VBA5_SIB_DEPTH];

    // Count number of non-empty modules

    *lpwModuleCount = 0;
    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = awSibs;
    awSibs[0] = wVBAChildEntry;
    stOpenSib.nNumWaitingSibs = 1;
    while (VBA5OpenCandidateModule(lpstCallBack,
                                   lpstOLEFile,
                                   NULL,
                                   NULL,
                                   &stOpenSib) == TRUE)
    {
        // Determine whether the stream consists of only
        //  Attribute lines

        if (VBA5LZNTModuleInit(lpstCallBack,
                               lpstOLEFile,
                               &stLZNT) == FALSE)
        {
            // Failed to initialize for reading source text string

            return(FALSE);
        }

        if (VBA5LZNTStreamAttributeOnly(&stLZNT,
                                        lpbyWorkBuffer) == FALSE)
        {
            // The stream contains non-attribute lines

            if (*lpwModuleCount < 0xFFFF)
                ++*lpwModuleCount;
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int WD8CheckMacrosDeletableCB()
//
// Description:
//  The callback checks the name and does the following:
//      1. If the name is "", "_*", or "dir", the function just
//         returns CONTINUE.
//      2. If the name is not "ThisDocument", the function
//         returns RETURN.
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If the entry is a candidate
//  OLE_OPEN_CB_STATUS_CONTINUE     If the entry is not
//
//********************************************************************

int WD8CheckMacrosDeletableCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it any of "", "_*", or "dir"

        if (ConvertEndianShort(lpstEntry->uszName[0]) == '_' ||
            lpstEntry->uszName[0] == 0 ||
            WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszdir,
                     MAX_OLE_NAME_LEN) == 0)
        {
            return(OLE_OPEN_CB_STATUS_CONTINUE);
        }

        // It is some other module name besides "ThisDocument"

        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszThisDocument,
                     MAX_OLE_NAME_LEN) != 0)
        {
            return(OLE_OPEN_CB_STATUS_RETURN);
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int WD8OpenThisDocumentCB()
//
// Description:
//  Callback for opening "ThisDocument" stream.
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If the entry is a candidate
//  OLE_OPEN_CB_STATUS_CONTINUE     If the entry is not
//
//********************************************************************

int WD8OpenThisDocumentCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszThisDocument,
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
//  BOOL WD8RepairModule()
//
// Description:
//  Repairs a module of a Word 8.0 virus.  If the module should
//  be deleted, then *lpbDelete is set to TRUE before returning.
//
// Returns:
//  TRUE        If the repair was successful
//  FALSE       If the repair was unsuccessful
//
//********************************************************************

BOOL WD8RepairModule
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    WORD                wModuleCount,       // Count of modules
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPBOOL              lpbDelete,          // Ptr to BOOL to request delete
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,   // Virus sig info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1.5K bytes
)
{
    LPWD_SCAN           lpstScan;
#if !defined(SYM_DOSX)
    LZNT_T              stLZNT;
#endif
    int                 i;

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    BYTE abyNameSigInterMacroHit[WD8_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNameSigIntraMacroHit[WD8_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNamedCRCSigHit[WD8_NAMED_CRC_SIG_HIT_ARRAY_SIZE];
    BYTE abyMacroSigHit[WD8_MACRO_SIG_HIT_ARRAY_SIZE];
    BYTE abyCRCHit[WD8_CRC_HIT_ARRAY_SIZE];
#else
    LPBYTE              lpbyHitMem;
#endif

    /////////////////////////////////////////////////////////////
    // Initialize the scan structure
    /////////////////////////////////////////////////////////////

    lpstScan = (LPWD_SCAN)lpbyWorkBuffer;

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

    lpstScan->wMacroCount = wModuleCount;

    lpstScan->lpstCallBack = lpstCallBack;
    lpstScan->lpstOLEStream = lpstOLEFile;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;
    lpstScan->uScan.stVBA5.lpstLZNT = &stLZNT;

    // Copy the name

    i = 0;
    while (1)
    {
        lpstScan->abyName[i] = abyModuleName[i];
        if (abyModuleName[i++] == 0)
            break;
    }

    WDInitHitBitArrays(&gstWD8SigSet,lpstScan);

    /////////////////////////////////////////////////////////////
    // Scan and apply repair to module
    /////////////////////////////////////////////////////////////

    // Scan module

    VBA5ScanModule(&gstWD8SigSet,lpstScan);

    // Apply repair signature

    *lpbDelete = FALSE;

    if (MVPCheck(MVP_WD8,
                 lpstScan->abyName,
                 lpstScan->dwCRC) == FALSE &&
        VBA5LZNTStreamAttributeOnly(&stLZNT,
                                    lpstScan->abyRunBuf) == FALSE)
    {
        // Found a non-approved macro

        *lpbDelete = TRUE;
    }
    else
    if (lpstVirusSigInfo->wID != VID_MVP)
    {
        // Apply repair signature

        if (WDApplyRepair(&gstWD8SigSet,
                          lpstScan,
                          lpbDelete,
                          lpstVirusSigInfo -
                              gstWD8SigSet.lpastVirusSigInfo,
                          0) ==
            WD_STATUS_ERROR)
        {
            // Error applying repair signature

            FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
            return(FALSE);
        }
    }

    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8Rec0x07ParseItemExtra()
//
// Description:
//  Parses through the extra info of an item of record 0x07 of the
//  template data table.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL WD8Rec0x07ParseItemExtra
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE          lpstOLEFile,
    DWORD               dwOffset,
    LPDWORD             lpdwNewOffset
)
{
    BYTE                abyExtraHdr[4];
    BYTE                byStrLen;

    // abyExtraHdr[4]
    // if (abyHeader[3] != 0)
    // {
    //     byStrLen
    //     wString[byStrLen]
    // }
    // if (abyExtraHdr[3] == 5)
    // {
    //     abyUnknown[7]
    //     byStrLen
    //     wString[byStrLen]
    //     abyUnknown[2]
    // }
    // if (abyExtraHdr[3] == 7)
    // {
    //     byUnknown
    //     byStrLen
    //     wString[byStrLen]
    //     abyUnknown[0x0A]
    // }

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    dwOffset,
                    abyExtraHdr,
                    4) != 4)
        return(FALSE);

    dwOffset += 4;

    if (abyExtraHdr[3] != 0)
    {
        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        &byStrLen,
                        sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE) + byStrLen * 2;
    }

    if (abyExtraHdr[3] == 5)
    {
        dwOffset += 7;

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        &byStrLen,
                        sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE) + byStrLen * 2 + 2;
    }
    else
    if (abyExtraHdr[3] == 7)
    {
        dwOffset += sizeof(BYTE);

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        &byStrLen,
                        sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE) + byStrLen * 2 + 0x0A;
    }

    *lpdwNewOffset = dwOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8Rec0x07ParseItemIcon()
//
// Description:
//  Parses through the icon info of an item of record 0x07 of the
//  template data table.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL WD8Rec0x07ParseItemIcon
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE          lpstOLEFile,
    DWORD               dwOffset,
    LPDWORD             lpdwNewOffset
)
{
    BYTE                byPictType;
    DWORD               dwCount;

    // byPictType
    // if (byPictType & 0x08)
    // {
    //     dwCount
    //     abyBM[dwCount-10]
    //     dwCount
    //     abyBM[dwCount-10]
    // }
    // if (byPictType & 0x10)
    //     wBitmapID

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    dwOffset,
                    &byPictType,
                    sizeof(BYTE)) != sizeof(BYTE))
        return(FALSE);

    dwOffset += sizeof(BYTE);

    if (byPictType & 0x08)
    {
        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&dwCount,
                        sizeof(DWORD)) != sizeof(DWORD))
            return(FALSE);

        dwCount = ConvertEndianLong(dwCount);

        dwOffset += sizeof(DWORD) + dwCount - 10;

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&dwCount,
                        sizeof(DWORD)) != sizeof(DWORD))
            return(FALSE);

        dwCount = ConvertEndianLong(dwCount);

        dwOffset += sizeof(DWORD) + dwCount - 10;
    }

    if (byPictType & 0x10)
        dwOffset += sizeof(WORD);

    *lpdwNewOffset = dwOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8Rec0x07ParseItem()
//
// Description:
//  Parses through record 0x07 of the template data table.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL WD8Rec0x07ParseItem
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE          lpstOLEFile,
    DWORD               dwOffset,
    LPDWORD             lpdwNewOffset,
    LPBYTE              lpbyWorkBuffer      // >= 256 bytes
)
{
    BYTE                abyHeader[0x0C];
    BYTE                byStrLen;

    // abyHeader[0x0C]
    // if (abyHeader[0] != 0x03 ||
    //     abyHeader[1] != 0x01)
    //     FAIL

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    dwOffset,
                    abyHeader,
                    0x0C) != 0x0C)
        return(FALSE);

    dwOffset += 0x0C;

    if (abyHeader[0] != 0x03 || abyHeader[1] != 0x01)
        return(FALSE);

    // if (abyHeader[3] == 0x0A &&
    //     abyHeader[4] == 0x01 &&
    //     abyHeader[5] == 0x00)
    // {
    //     // New menu
    //
    //     byStrLen
    //     wString[byStrLen]
    //     abyUnknown[4]
    //     byStrLen
    //     wString[byStrLen]
    // }

    if (abyHeader[3] == 0x0A && abyHeader[4] == 0x01 && abyHeader[5] == 0x00)
    {
        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        &byStrLen,
                        sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE) + byStrLen * 2;

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        lpbyWorkBuffer,
                        4) != 4)
            return(FALSE);

        dwOffset += 4;

        if (lpbyWorkBuffer[0] != 0)
        {
            if (OLESeekRead(lpstCallBack,
                            lpstOLEFile,
                            dwOffset,
                            &byStrLen,
                            sizeof(BYTE)) != sizeof(BYTE))
                return(FALSE);

            dwOffset += sizeof(BYTE) + byStrLen * 2;
        }
    }

    // else
    // if (abyHeader[3] == 0x01 &&
    //     abyHeader[4] == 0x01 &&
    //     abyHeader[5] == 0x00)
    // {
    //     // Macro association
    //
    //     byStrLen
    //     wString[byStrLen]        // button text
    //     if (byStrLen > 0)
    //         byUnknown
    //     byStrLen
    //     wString[byStrLen]        // balloon text
    //     if (byStrLen > 0)
    //         abyUnknown[0x06]
    //     else
    //         abyUnknown[0x04]
    //     byStrLen
    //     wString[byStrLen]        // macro association
    //     abyUnknown[3]
    //     ParseIcon
    // }

    else
    if (abyHeader[3] == 0x01 && abyHeader[4] == 0x01 && abyHeader[5] == 0x00)
    {
        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        &byStrLen,
                        sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE) + byStrLen * 2;

        if (byStrLen > 0)
            dwOffset += sizeof(BYTE);

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        &byStrLen,
                        sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE) + byStrLen * 2;

        if (byStrLen > 0)
            dwOffset += 6;
        else
            dwOffset += 4;

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        &byStrLen,
                        sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE) + byStrLen * 2 + 3;

        if (WD8Rec0x07ParseItemIcon(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    &dwOffset) == FALSE)
            return(FALSE);
    }

    // else
    // if (abyHeader[3] & 0x08)
    // {
    //     // Built-in menu
    //
    //     if (abyHeader[8] == 2)
    //         abyUnknown[0x0C]
    //     else
    //         abyUnknown[0x08]
    // }

    else
    if (abyHeader[3] & 0x08)
    {
        if (abyHeader[8] == 2)
            dwOffset += 0x0C;
        else
            dwOffset += 0x08;
    }

    // else
    // if (abyHeader[3] == 0x02 ||
    //     abyHeader[3] == 0x04 ||
    //     abyHeader[3] == 0x0D ||
    //     abyHeader[3] == 0x14)
    // {
    //     // Special 0
    //
    //     abyUnknown[0x08]
    // }

    else
    if (abyHeader[3] == 0x02 ||
        abyHeader[3] == 0x04 ||
        abyHeader[3] == 0x0D ||
        abyHeader[3] == 0x14)
    {
        dwOffset += 8;
    }

    // else
    // if (abyHeader[3] == 0x06 ||
    //     abyHeader[3] == 0x07 ||
    //     abyHeader[3] == 0x13)
    // {
    //     // Special 1
    //
    //     ParseExtraStrings
    // }

    else
    if (abyHeader[3] == 0x06 ||
        abyHeader[3] == 0x07 ||
        abyHeader[3] == 0x13)
    {
        if (WD8Rec0x07ParseItemExtra(lpstCallBack,
                                     lpstOLEFile,
                                     dwOffset,
                                     &dwOffset) == FALSE)
            return(FALSE);
    }

    // else
    // if (abyHeader[8] == 0x22)
    // {
    //     // Fonts/AutoText/Styles
    //
    //     abyUnknown[0x0B]
    //     byStrLen
    //     wString[byStrLen]
    //     abyUnknown[0x02]
    //     ParseIcon
    // }

    else
    if (abyHeader[8] == 0x22)
    {
        dwOffset += 0x0B;

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        &byStrLen,
                        sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE) + byStrLen * 2 + 2;

        if (WD8Rec0x07ParseItemIcon(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    &dwOffset) == FALSE)
            return(FALSE);
    }

    // else
    // {
    //     // Command
    //
    //     ParseExtraStrings
    //     ParseIcon
    // }

    else
    {
        if (WD8Rec0x07ParseItemExtra(lpstCallBack,
                                     lpstOLEFile,
                                     dwOffset,
                                     &dwOffset) == FALSE)
            return(FALSE);

        if (WD8Rec0x07ParseItemIcon(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    &dwOffset) == FALSE)
            return(FALSE);
    }

    // Update the new offset

    *lpdwNewOffset = dwOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8GetTDTRecordInfo()
//
// Description:
//  Parse through the template data table to obtain
//             the location, size, and number of subrecords
//             of record types 0x01, 0x03, 0x10, and 0x11.
//
//  The function returns failure if either of record types
//  0x01, 0x03, 0x10, or 0x11 have more than one record, if any
//  record does not conform to the assumed format, or if
//  an unknown record type is encountered.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

typedef struct tagWD8_TDT_INFO
{
    DWORD           dwTDTOffset;
    DWORD           dwTDTSize;

    DWORD           dwRecord0x01Offset;
    DWORD           dwRecord0x01Size;
    WORD            wNum0x01Subrecords;

    DWORD           dwRecord0x03Offset;
    DWORD           dwRecord0x03Size;

    DWORD           dwRecord0x07Offset;
    DWORD           dwRecord0x07Size;

    DWORD           dwRecord0x10Offset;
    DWORD           dwRecord0x10Size;
    WORD            wNum0x10Subrecords;
    DWORD           dwRecord0x11Offset;
    DWORD           dwRecord0x11Size;
    WORD            wNum0x11Subrecords;
} WD8_TDT_INFO_T, FAR *LPWD8_TDT_INFO;

BOOL WD8GetTDTRecordInfo
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for 1Table stream
    LPWD8_TDT_INFO      lpstTDTInfo,        // Ptr to TDT info structure
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 256 bytes
)
{
    DWORD               dwOffset;
    DWORD               dwEndOffset;
    DWORD               dwCount;
    WORD                wCount;
    WORD                wSubCount;
    WORD                wLen;
    BYTE                byLen;
    BYTE                byTemp;
    BYTE                byRecType;

    // Verify that the first byte is 0xFF

    dwOffset = lpstTDTInfo->dwTDTOffset;
    dwEndOffset = dwOffset + lpstTDTInfo->dwTDTSize;
    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    dwOffset++,
                    &byTemp,
                    sizeof(BYTE)) != sizeof(BYTE) ||
        byTemp != 0xFF)
    {
        return(FALSE);
    }

    // Initialize fields to uninitialized state

    lpstTDTInfo->dwRecord0x01Offset = 0xFFFFFFFF;
    lpstTDTInfo->dwRecord0x03Offset = 0xFFFFFFFF;
    lpstTDTInfo->dwRecord0x07Offset = 0xFFFFFFFF;
    lpstTDTInfo->dwRecord0x10Offset = 0xFFFFFFFF;
    lpstTDTInfo->dwRecord0x11Offset = 0xFFFFFFFF;

    // Now begin parsing

    byRecType = 0;
    while (dwOffset < dwEndOffset && byRecType != 0x40)
    {
        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset++,
                        &byRecType,
                        sizeof(BYTE)) != sizeof(BYTE))
        {
            return(FALSE);
        }

        switch (byRecType)
        {
            case 0x01:
                // Macro record format:
                //  byRecType
                //  wMacroCount
                //  wZero
                //  repeat wMacroCount times
                //  {
                //      abyUnknown[2]
                //      wRecord0x11ID
                //      wRecord0x10Index
                //      abyUnknown[18]
                //  }

                if (lpstTDTInfo->dwRecord0x01Offset != 0xFFFFFFFF)
                {
                    // Double 0x01 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x01Offset = dwOffset - 1;

                // Read the number of records

                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset,
                                (LPBYTE)&lpstTDTInfo->wNum0x01Subrecords,
                                sizeof(WORD)) != sizeof(WORD))
                {
                    return(FALSE);
                }

                // Skip past the count and zero word

                dwOffset += sizeof(WORD) + sizeof(WORD);

                lpstTDTInfo->wNum0x01Subrecords =
                    ConvertEndianShort(lpstTDTInfo->wNum0x01Subrecords);

                dwOffset += lpstTDTInfo->wNum0x01Subrecords * (DWORD)24;

                lpstTDTInfo->dwRecord0x01Size = dwOffset -
                    lpstTDTInfo->dwRecord0x01Offset;

                break;

            case 0x02:
                // Font? Format:
                //  byRecType
                //  dwCount
                //  repeat dwCount times
                //  {
                //      abyUnknown[4]
                //  }

                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset,
                                (LPBYTE)&dwCount,
                                sizeof(DWORD)) != sizeof(DWORD))
                {
                    return(FALSE);
                }

                dwCount = ConvertEndianLong(dwCount);

                dwOffset += sizeof(DWORD) + dwCount * 4;

                break;

            case 0x03:
                // Keyboard command format:
                //  byRecType
                //  dwCount
                //  repeat dwCount times
                //  {
                //      abyUnknown[4] = 0x00 0x00 0x00 0x00
                //      byASCIIKey
                //      byModKey:   0x01 = Shift
                //                  0x02 = Ctrl
                //                  0x04 = Alt
                //      byASCIIKey2 = 0xFF
                //      byModKey2 = 0x00
                //      abyUnknown[4] = 0x00 0x00 0x02 0x00
                //      wRecord0x11ID
                //  }

                if (lpstTDTInfo->dwRecord0x03Offset != 0xFFFFFFFF)
                {
                    // Double 0x03 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x03Offset = dwOffset - 1;

                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset,
                                (LPBYTE)&dwCount,
                                sizeof(DWORD)) != sizeof(DWORD))
                {
                    return(FALSE);
                }

                dwCount = ConvertEndianLong(dwCount);

                dwOffset += sizeof(DWORD) + dwCount * 14;

                lpstTDTInfo->dwRecord0x03Size = dwOffset -
                    lpstTDTInfo->dwRecord0x03Offset;

                break;

            case 0x07:
                // Toolbars format:
                //  byRecType
                //  wUnknown = 0x0006
                //  wUnknown = 0x000C
                //  wUnknown = 0x0012
                //  wCount
                //  dwSectionSize
                //  abySection[dwMenubarSectionSize]
                //  repeat wCount times
                //  {
                //      abyUnknown[6]
                //      wSubcount
                //      if (abyUnknown[0] != 0x00)
                //      {
                //          // abyUnknown[0]:
                //          //     0x09 To existing toolbar?
                //          //     0x0A To existing menu?
                //          //     0x25 New menu?
                //          repeat wSubcount times
                //              abyUnknown[0x12]
                //          continue
                //      }
                //      wStrLen
                //      wString[wStrLen]    // Item name
                //      abyUnknown[0x14]
                //      byStrLen
                //      wString[byStrLen]   // Item name
                //      abyUnknown[0x6C]
                //      dwCount
                //      repeat dwCount times
                //          WD8Rec0x07ParseItem()
                //  }

                // awUnknown[3] = 0x0006 0x000C 0x0012

                if (lpstTDTInfo->dwRecord0x07Offset != 0xFFFFFFFF)
                {
                    // Double 0x07 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x07Offset = dwOffset - 1;

                dwOffset += sizeof(WORD) + sizeof(WORD) + sizeof(WORD);

                // Read the number of toolbar items

                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset,
                                (LPBYTE)&wCount,
                                sizeof(WORD)) != sizeof(WORD))
                {
                    return(FALSE);
                }

                wCount = ConvertEndianShort(wCount);

                dwOffset += sizeof(WORD);

                //  dwMenubarSectionSize
                //  abyMenubarSection[dwMenubarSectionSize]

                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset,
                                (LPBYTE)&dwCount,
                                sizeof(DWORD)) != sizeof(DWORD))
                {
                    return(FALSE);
                }

                dwCount = ConvertEndianLong(dwCount);

                dwOffset += sizeof(DWORD) + dwCount;

                while (wCount-- != 0)
                {
                    // abyUnknown[6]
                    // wSubcount

                    if (OLESeekRead(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    lpbyWorkBuffer,
                                    8) != 8)
                    {
                        return(FALSE);
                    }

                    dwOffset += 8;

                    // if (abyUnknown[0] != 0x00)
                    // {
                    //     repeat wSubcount times
                    //         abyUnknown[0x12]
                    //     continue
                    // }

                    if (lpbyWorkBuffer[0] != 0x00)
                    {
                        wSubCount = (lpbyWorkBuffer[7] << 8) |
						    lpbyWorkBuffer[6];
                        dwOffset += wSubCount * (DWORD)0x12;
                        continue;
                    }
                    else
                    if (lpbyWorkBuffer[0] == 0x0A)
                        continue;

                    // wStrLen
                    // wString[wStrLen]
                    // abyUnknown[0x14]

                    if (OLESeekRead(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    (LPBYTE)&wLen,
                                    sizeof(WORD)) != sizeof(WORD))
                    {
                        return(FALSE);
                    }

                    wLen = ConvertEndianShort(wLen);

                    dwOffset += sizeof(WORD) + wLen * (DWORD)2 + 0x14;

                    // byStrLen
                    // wString[byStrLen]
                    // abyUnknown[0x6C]

                    if (OLESeekRead(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    (LPBYTE)&byLen,
                                    sizeof(BYTE)) != sizeof(BYTE))
                    {
                        return(FALSE);
                    }

                    dwOffset += sizeof(BYTE) + byLen * (DWORD)2 + 0x6C;

                    // dwCount

                    if (OLESeekRead(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    (LPBYTE)&dwCount,
                                    sizeof(DWORD)) != sizeof(DWORD))
                    {
                        return(FALSE);
                    }

                    dwCount = ConvertEndianLong(dwCount);

                    dwOffset += sizeof(DWORD);
                    while (dwCount-- != 0)
                    {
                        if (WD8Rec0x07ParseItem(lpstCallBack,
                                                lpstOLEFile,
                                                dwOffset,
                                                &dwOffset,
                                                lpbyWorkBuffer) == FALSE)
                        {
                            return(FALSE);
                        }
                    }
                }

                lpstTDTInfo->dwRecord0x07Size = dwOffset -
                    lpstTDTInfo->dwRecord0x07Offset;

                break;

            case 0x10:
                // Mixed case macro string format:
                //  byRecType
                //  w0xFFFF
                //  wSubrecordCount
                //  w0x0002
                //  repeat wSubrecordCount times
                //  {
                //      wStrLen
                //      wString[wStrLen]
                //      wUnknown
                //  }

                if (lpstTDTInfo->dwRecord0x10Offset != 0xFFFFFFFF)
                {
                    // Double 0x10 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x10Offset = dwOffset - 1;

                // Read the next six bytes

                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset,
                                lpbyWorkBuffer,
                                6) != 6)
                {
                    return(FALSE);
                }

                dwOffset += 6;

                // Verify that the first word is 0xFFFF and
                //  the second is 0x0002

                if (lpbyWorkBuffer[0] != 0xFF ||
                    lpbyWorkBuffer[1] != 0xFF ||
                    lpbyWorkBuffer[4] != 0x02 ||
                    lpbyWorkBuffer[5] != 0x00)
                {
                    return(FALSE);
                }

                // Get the count

                lpstTDTInfo->wNum0x10Subrecords =
                    (lpbyWorkBuffer[3] << 8) | lpbyWorkBuffer[2];

                // Now parse through the strings

                wCount = lpstTDTInfo->wNum0x10Subrecords;
                while (wCount-- != 0)
                {
                    // Read the length

                    if (OLESeekRead(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    (LPBYTE)&wLen,
                                    sizeof(WORD)) != sizeof(WORD))
                    {
                        return(FALSE);
                    }

                    wLen = ConvertEndianShort(wLen);

                    // Skip past this subrecord

                    dwOffset += sizeof(WORD) + wLen * 2 + 2;
                }

                lpstTDTInfo->dwRecord0x10Size = dwOffset -
                    lpstTDTInfo->dwRecord0x10Offset;

                break;

            case 0x11:
                // Uppercase macro string format:
                //  byRecType
                //  wSubrecordCount
                //  repeat wSubrecordCount times
                //  {
                //      wID
                //      wStrLen
                //      wString[wStrLen]
                //      wUnknown
                //  }

                if (lpstTDTInfo->dwRecord0x11Offset != 0xFFFFFFFF)
                {
                    // Double 0x11 record!

                    return(FALSE);
                }
                lpstTDTInfo->dwRecord0x11Offset = dwOffset - 1;

                // Read the subrecord count

                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset,
                                (LPBYTE)&lpstTDTInfo->wNum0x11Subrecords,
                                sizeof(WORD)) != sizeof(WORD))
                {
                    return(FALSE);
                }

                dwOffset += sizeof(WORD);

                lpstTDTInfo->wNum0x11Subrecords =
                    ConvertEndianShort(lpstTDTInfo->wNum0x11Subrecords);

                // Now parse through the strings

                wCount = lpstTDTInfo->wNum0x11Subrecords;
                while (wCount-- != 0)
                {
                    // Skip past the ID

                    dwOffset += sizeof(WORD);

                    // Read the length

                    if (OLESeekRead(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    (LPBYTE)&wLen,
                                    sizeof(WORD)) != sizeof(WORD))
                    {
                        return(FALSE);
                    }

                    wLen = ConvertEndianShort(wLen);

                    // Skip past this subrecord

                    dwOffset += sizeof(WORD) + wLen * 2 + 2;
                }

                lpstTDTInfo->dwRecord0x11Size = dwOffset -
                    lpstTDTInfo->dwRecord0x11Offset;

                break;

            case 0x12:
                // Format:
                //  byRecType
                //  wUnknown

                dwOffset += sizeof(WORD);
                break;

            case 0x40:
                break;

            default:
                // Unknown record type

                return(FALSE);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8TDTCheckModuleString()
//
// Description:
//  The function reads the string at the given offset, searches
//  for the first period, then performs an uppercase match
//  against the given module name.  The given module name is
//  assumed to be all uppercase.  If there is a match, the
//  next character must be a period.
//
//  The function assumes that the string at the given offset
//  is made of double-byte characters.
//
//  If there is a match, the function sets *lpbMatch to TRUE.
//  Otherwise, *lpbMatch is set to FALSE.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL WD8TDTCheckModuleString
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for 1Table stream
    LPBYTE              abyModuleName,      // Zero terminated module name
    DWORD               dwStrOffset,        // Offset of string to check
    WORD                wLen,               // Length of the string
    LPBOOL              lpbMatch,           // Ptr to BOOL for match status
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    BYTE                byTemp;
    WORD                w;
    int                 i;

    *lpbMatch = FALSE;

    // For now, if the length is greater than 256, just return
    //  failure

    if (wLen > 256)
        return(FALSE);

    // Multiply by 2 to account for double-byte characters

    wLen *= 2;

    // Read in the string

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    dwStrOffset,
                    lpbyWorkBuffer,
                    wLen) != wLen)
    {
        return(FALSE);
    }

    // Now search for the first period

    for (w=0;w<wLen;w+=2)
    {
        if (lpbyWorkBuffer[w] == '.')
        {
            w += 2;
            break;
        }
    }

    if (w >= wLen)
    {
        // No period!?

        return(FALSE);
    }

    // Now do a comparison

    for (i=0;abyModuleName[i];i++)
    {
        if (w >= wLen)
            return(FALSE);

        byTemp = lpbyWorkBuffer[w];
        if ('a' <= byTemp && byTemp <= 'z')
            byTemp = (byTemp - 'a') + 'A';

        if (byTemp != abyModuleName[i])
            return(TRUE);

        w += 2;
    }

    // At this point, the module name must have matched,
    //  so make sure that the next item is a period

    if (w >= wLen)
        return(FALSE);

    if (lpbyWorkBuffer[w] != '.')
        return(TRUE);

    // It was a match

    *lpbMatch = TRUE;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8TDTDelete0x11Entry()
//
// Description:
//  The function parses through record 0x11 and deletes the record
//  with the given ID if the module name matches.
//
//  The function returns failure if no records are deleted.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL WD8TDTDelete0x11Entry
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for 1Table stream
    LPBYTE              abyModuleName,      // Zero terminated module name
    WORD                wID,                // ID of subrecord to delete
    DWORD               dwRecordOffset,     // Offset of record 0x11
    WORD                wNumSubrecords,     // Number of subrecords
    LPDWORD             lpdwRecordSize,     // Ptr to DWORD of record size
    LPBOOL              lpbDeleted,         // Ptr to BOOL for delete status
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    WORD                wCandID;
    WORD                wLen;
    WORD                w;
    DWORD               dwOffset;
    DWORD               dwDelRecordOffset;
    DWORD               dwDelRecordEndOffset;
    BOOL                bMatch;

    *lpbDeleted = FALSE;

    // Skip past the record type field and subrecord count field

    dwOffset = dwRecordOffset + sizeof(BYTE) + sizeof(WORD);

    // Iterate through subrecords looking for given ID

    for (w=0;w<wNumSubrecords;w++)
    {
        // Read the ID

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&wCandID,
                        sizeof(WORD)) != sizeof(WORD))
        {
            return(FALSE);
        }

        wCandID = ConvertEndianShort(wCandID);
        dwOffset += sizeof(WORD);

        // Read the length

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&wLen,
                        sizeof(WORD)) != sizeof(WORD))
        {
            return(FALSE);
        }

        wLen = ConvertEndianShort(wLen);
        dwOffset += sizeof(WORD);

        if (wCandID == wID)
        {
            // Found it, so check to see whether it is one we
            //  want to delete

            if (WD8TDTCheckModuleString(lpstCallBack,
                                        lpstOLEFile,
                                        abyModuleName,
                                        dwOffset,
                                        wLen,
                                        &bMatch,
                                        lpbyWorkBuffer) == FALSE)
            {
                return(FALSE);
            }

            if (bMatch == TRUE)
            {
                dwDelRecordOffset = dwOffset - sizeof(WORD) - sizeof(WORD);
                dwDelRecordEndOffset = dwOffset + wLen * 2 + 2;
                break;
            }
            else
                return(TRUE);
        }

        // Skip past this subrecord

        dwOffset += wLen * 2 + 2;
    }

    // Was there a matching record?

    if (w < wNumSubrecords)
    {
        // Copy over the record to delete

        if (OLECopyBytes(lpstCallBack,
                         lpstOLEFile,
                         dwDelRecordEndOffset,
                         dwDelRecordOffset,
                         dwRecordOffset + *lpdwRecordSize -
                             dwDelRecordEndOffset,
                         lpbyWorkBuffer) == FALSE)
        {
            return(FALSE);
        }

        // Update the size of the record

        *lpdwRecordSize -= (dwDelRecordEndOffset - dwDelRecordOffset);

        // Success

        *lpbDeleted = TRUE;
    }

    // Didn't find a matching record

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8TDTDelete0x10Entry()
//
// Description:
//  The function parses through record 0x10 and deletes the record
//  at the given index if the module name matches.
//
//  The function returns failure if no records are deleted.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL WD8TDTDelete0x10Entry
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for 1Table stream
    LPBYTE              abyModuleName,      // Zero terminated module name
    WORD                wIndex,             // Index of subrecord to delete
    DWORD               dwRecordOffset,     // Offset of record 0x10
    WORD                wNumSubrecords,     // Number of subrecords
    LPDWORD             lpdwRecordSize,     // Ptr to DWORD of record size
    LPBOOL              lpbDeleted,         // Ptr to BOOL for delete status
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    WORD                wLen;
    WORD                w;
    DWORD               dwOffset;
    DWORD               dwDelRecordOffset;
    DWORD               dwDelRecordEndOffset;
    BOOL                bMatch;

    *lpbDeleted = FALSE;

    // Skip past the record type field, 0xFFFF, subrecord count field,
    //  and 0x0002

    dwOffset = dwRecordOffset + sizeof(BYTE) +
        sizeof(WORD) + sizeof(WORD) + sizeof(WORD);

    // Iterate through subrecords looking for given index

    for (w=0;w<wNumSubrecords;w++)
    {
        // Read the length

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&wLen,
                        sizeof(WORD)) != sizeof(WORD))
        {
            return(FALSE);
        }

        wLen = ConvertEndianShort(wLen);
        dwOffset += sizeof(WORD);

        if (w == wIndex)
        {
            // Found it

            if (WD8TDTCheckModuleString(lpstCallBack,
                                        lpstOLEFile,
                                        abyModuleName,
                                        dwOffset,
                                        wLen,
                                        &bMatch,
                                        lpbyWorkBuffer) == FALSE)
            {
                return(FALSE);
            }

            if (bMatch == TRUE)
            {
                dwDelRecordOffset = dwOffset - sizeof(WORD);
                dwDelRecordEndOffset = dwOffset + wLen * 2 + 2;
                break;
            }
            else
                return(TRUE);
        }

        // Skip past this subrecord

        dwOffset += wLen * 2 + 2;
    }

    // Was there a matching record?

    if (w < wNumSubrecords)
    {
        // Copy over the record to delete

        if (OLECopyBytes(lpstCallBack,
                         lpstOLEFile,
                         dwDelRecordEndOffset,
                         dwDelRecordOffset,
                         dwRecordOffset + *lpdwRecordSize -
                             dwDelRecordEndOffset,
                         lpbyWorkBuffer) == FALSE)
        {
            return(FALSE);
        }

        // Update the size of the record

        *lpdwRecordSize -= (dwDelRecordEndOffset - dwDelRecordOffset);

        // Success

        *lpbDeleted = TRUE;
    }

    // Didn't find a matching record

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8TDTDelete0x03Entries()
//
// Description:
//  The function parses through record 0x03 and deletes any of the
//  key assignment records referencing the given ID.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL WD8TDTDelete0x03Entries
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for 1Table stream
    WORD                wID,                // ID
    DWORD               dwRecordOffset,     // Offset of record 0x10
    LPDWORD             lpdwRecordSize,     // Ptr to DWORD of record size
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwCount;
    DWORD               dwNumLeft;
    DWORD               dwOffset;
    DWORD               dwDstOffset;
    WORD                wCandID;
    BOOL                bDeleted;

    if (dwRecordOffset == 0xFFFFFFFF || *lpdwRecordSize == 0)
    {
        // No keyboard assignments

        return(TRUE);
    }

    // Keyboard command format:
    //  byRecType
    //  dwCount
    //  repeat dwCount times
    //  {
    //      abyUnknown[4] = 0x00 0x00 0x00 0x00
    //      byASCIIKey
    //      byModKey:   0x01 = Shift
    //                  0x02 = Ctrl
    //                  0x04 = Alt
    //      byASCIIKey2 = 0xFF
    //      byModKey2 = 0x00
    //      abyUnknown[4] = 0x00 0x00 0x02 0x00
    //      wRecord0x11ID
    //  }

    // Read the count

    dwOffset = dwRecordOffset + sizeof(BYTE);
    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    dwOffset,
                    (LPBYTE)&dwCount,
                    sizeof(DWORD)) != sizeof(DWORD))
    {
        return(FALSE);
    }

    dwOffset += sizeof(DWORD);

    dwCount = ConvertEndianLong(dwCount);

    // Iterate through subrecords

    dwNumLeft = dwCount;
    dwDstOffset = dwOffset;
    bDeleted = FALSE;
    while (dwNumLeft-- != 0)
    {
        // Read the ID of the next record

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset+12,
                        (LPBYTE)&wCandID,
                        sizeof(WORD)) != sizeof(WORD))
        {
            return(FALSE);
        }

        wCandID = ConvertEndianShort(wCandID);
        if (wCandID != wID)
        {
            // Copy only if we have deleted something so far

            if (dwDstOffset != dwOffset)
            {
                // Read record

                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset,
                                lpbyWorkBuffer,
                                14) != 14)
                {
                    return(FALSE);
                }

                // Write record

                if (OLESeekWrite(lpstCallBack,
                                 lpstOLEFile,
                                 dwDstOffset,
                                 lpbyWorkBuffer,
                                 14) != 14)
                {
                    return(FALSE);
                }
            }

            dwDstOffset += 14;
        }
        else
        {
            // This is one to delete

            --dwCount;
            bDeleted = TRUE;
        }

        dwOffset += 14;
    }

    // Was at least one record deleted?

    if (bDeleted == TRUE)
    {
        // Update the size of the record

        if (dwCount == 0)
            *lpdwRecordSize = 0;
        else
        {
            *lpdwRecordSize = sizeof(BYTE) + sizeof(DWORD) + dwCount * 14;

            // Write the new count

            dwCount = ConvertEndianLong(dwCount);
            if (OLESeekWrite(lpstCallBack,
                             lpstOLEFile,
                             dwRecordOffset + sizeof(BYTE),
                             (LPBYTE)&dwCount,
                             sizeof(DWORD)) != sizeof(DWORD))
            {
                return(FALSE);
            }
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8UpdateRecord0x07AfterMove()
//
// Description:
//  The function parses through record 0x07 and shifts the offsets
//  in the records that contain offsets to the top-level section.
//  The offsets are shifted by the given amount.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL WD8UpdateRecord0x07AfterMove
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for 1Table stream
    DWORD               dwRecordOffset,     // Old offset of the record
    DWORD               dwRecordSize,       // Size of the record
    DWORD               dwShiftOffset,      // Amount shifted
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
)
{
    DWORD               dwOffset;
    WORD                wCount;
    WORD                wSubCount;
    DWORD               dwCount;
    BYTE                byLen;
    BYTE                wLen;

    (void)dwRecordSize;

    if (dwRecordOffset == 0xFFFFFFFF)
    {
        // No record 0x07

        return(TRUE);
    }

    dwOffset = dwRecordOffset - dwShiftOffset + 1;

    // Toolbars format:
    //  byRecType
    //  wUnknown = 0x0006
    //  wUnknown = 0x000C
    //  wUnknown = 0x0012
    //  wCount
    //  dwSectionSize
    //  abySection[dwMenubarSectionSize]
    //  repeat wCount times
    //  {
    //      abyUnknown[6]
    //      wSubcount
    //      if (abyUnknown[0] != 0x00)
    //      {
    //          // abyUnknown[0]:
    //          //     0x09 To existing toolbar?
    //          //     0x0A To existing menu?
    //          //     0x25 New menu?
    //          repeat wSubcount times
    //              abyUnknown[0x12]
    //          continue
    //      }
    //      wStrLen
    //      wString[wStrLen]    // Item name
    //      abyUnknown[0x14]
    //      byStrLen
    //      wString[byStrLen]   // Item name
    //      abyUnknown[0x6C]
    //      dwCount
    //      repeat dwCount times
    //          WD8Rec0x07ParseItem()
    //  }

    // awUnknown[3] = 0x0006 0x000C 0x0012

    dwOffset += sizeof(WORD) + sizeof(WORD) + sizeof(WORD);

    // Read the number of toolbar items

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    dwOffset,
                    (LPBYTE)&wCount,
                    sizeof(WORD)) != sizeof(WORD))
    {
        return(FALSE);
    }

    wCount = ConvertEndianShort(wCount);

    dwOffset += sizeof(WORD);

    //  dwSectionSize
    //  abySection[dwMenubarSectionSize]

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    dwOffset,
                    (LPBYTE)&dwCount,
                    sizeof(DWORD)) != sizeof(DWORD))
    {
        return(FALSE);
    }

    dwCount = ConvertEndianLong(dwCount);

    dwOffset += sizeof(DWORD) + dwCount;

    while (wCount-- != 0)
    {
        // abyUnknown[6]
        // wSubcount

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        lpbyWorkBuffer,
                        8) != 8)
        {
            return(FALSE);
        }

        dwOffset += 8;

        // if (abyUnknown[0] != 0x00)
        // {
        //     repeat wSubcount times
        //         abyUnknown[0x12]
        //     continue
        // }

        if (lpbyWorkBuffer[0] != 0x00)
        {
            DWORD   dwSectionOffset;

            wSubCount = (lpbyWorkBuffer[7] << 8) |
                lpbyWorkBuffer[6];
            while (wSubCount-- != 0)
            {
                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset + 0x0A,
                                (LPBYTE)&dwSectionOffset,
                                sizeof(DWORD)) != sizeof(DWORD))
                    return(FALSE);

                dwSectionOffset = ConvertEndianLong(dwSectionOffset);

                if (dwSectionOffset != 0)
                {
                    if (dwSectionOffset < dwShiftOffset)
                        return(FALSE);

                    dwSectionOffset -= dwShiftOffset;

                    dwSectionOffset = ConvertEndianLong(dwSectionOffset);

                    if (OLESeekWrite(lpstCallBack,
                                     lpstOLEFile,
                                     dwOffset + 0x0A,
                                     (LPBYTE)&dwSectionOffset,
                                     sizeof(DWORD)) != sizeof(DWORD))
                        return(FALSE);
                }

                dwOffset += 0x12;
            }
            continue;
        }
        else
        if (lpbyWorkBuffer[0] == 0x0A)
            continue;

        // wStrLen
        // wString[wStrLen]
        // abyUnknown[0x14]

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&wLen,
                        sizeof(WORD)) != sizeof(WORD))
        {
            return(FALSE);
        }

        wLen = ConvertEndianShort(wLen);

        dwOffset += sizeof(WORD) + wLen * (DWORD)2 + 0x14;

        // byStrLen
        // wString[byStrLen]
        // abyUnknown[0x6C]

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&byLen,
                        sizeof(BYTE)) != sizeof(BYTE))
        {
            return(FALSE);
        }

        dwOffset += sizeof(BYTE) + byLen * (DWORD)2 + 0x6C;

        // dwCount

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&dwCount,
                        sizeof(DWORD)) != sizeof(DWORD))
        {
            return(FALSE);
        }

        dwCount = ConvertEndianLong(dwCount);

        dwOffset += sizeof(DWORD);
        while (dwCount-- != 0)
        {
            if (WD8Rec0x07ParseItem(lpstCallBack,
                                    lpstOLEFile,
                                    dwOffset,
                                    &dwOffset,
                                    lpbyWorkBuffer) == FALSE)
            {
                return(FALSE);
            }
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8TDTDeleteModule()
//
// Description:
//  The function parses through record 0x01 and deletes all
//  subrecords belonging to the given module.
//
//  The function returns with *lpbDeleted == FALSE if no records
//  are deleted.  If a record was deleted, then the function
//  returns with *lpbDeleted = TRUE.
//
//  On success, the function updates the subrecord counts
//  for record types 0x01, 0x10, and 0x11.  On return,
//  lpstTDTInfo->dwTDTSize is updated to reflect the new
//  size of the template data table after compaction.
//
// Returns:
//  TRUE        If successful
//  FALSE       If unsuccessful
//
//********************************************************************

BOOL WD8TDTDeleteModule
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for 1Table stream
    LPWD8_TDT_INFO      lpstTDTInfo,        // Ptr to TDT info structure
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPBOOL              lpbDeleted,         // Ptr to BOOL for delete status
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    DWORD               dwRecord0x01Size;
    WORD                wNum0x01Subrecords;
    DWORD               dwRecord0x03Size;
    DWORD               dwRecord0x10Size;
    WORD                wNum0x10Subrecords;
    DWORD               dwRecord0x11Size;
    WORD                wNum0x11Subrecords;

    DWORD               dwRecord0x07ShiftOffset;

    WORD                w;
    WORD                wTemp;
    DWORD               dwTemp;

    WORD                wDeleteIndex;
    WORD                w0x11ID;
    WORD                w0x10Index;

    BOOL                bAtLeastOneDeleted;

    BOOL                b0x11Deleted;
    BOOL                b0x10Deleted;

    DWORD               dwOffset;
    DWORD               dwDstOffset;

#define NUM_REGIONS 4

    DWORD               dwRegionOffset[NUM_REGIONS];
    DWORD               dwRegionNewSize[NUM_REGIONS];
    DWORD               dwRegionOrigSize[NUM_REGIONS];
    int                 nNumRegions;

    int                 i, j;

    // There is no macro info if the record offset of the
    //  0x01 record is 0xFFFFFFFF

    if (lpstTDTInfo->dwRecord0x01Offset == 0xFFFFFFFF)
    {
        *lpbDeleted = FALSE;
        return(TRUE);
    }

    dwRecord0x01Size = lpstTDTInfo->dwRecord0x01Size;
    wNum0x01Subrecords = lpstTDTInfo->wNum0x01Subrecords;
    dwRecord0x03Size = lpstTDTInfo->dwRecord0x03Size;
    dwRecord0x10Size = lpstTDTInfo->dwRecord0x10Size;
    wNum0x10Subrecords = lpstTDTInfo->wNum0x10Subrecords;
    dwRecord0x11Size = lpstTDTInfo->dwRecord0x11Size;
    wNum0x11Subrecords = lpstTDTInfo->wNum0x11Subrecords;

    // Iterate through the subrecords of record type 0x01

    bAtLeastOneDeleted = FALSE;
    while (wNum0x01Subrecords != 0)
    {
        dwOffset = lpstTDTInfo->dwRecord0x01Offset + 7;
        for (w=0;w<wNum0x01Subrecords;w++)
        {
            // Get the 0x11 record ID and 0x10 record index

            if (OLESeekRead(lpstCallBack,
                            lpstOLEFile,
                            dwOffset,
                            (LPBYTE)&w0x11ID,
                            sizeof(WORD)) != sizeof(WORD) ||
                OLESeekRead(lpstCallBack,
                            lpstOLEFile,
                            dwOffset + 2,
                            (LPBYTE)&w0x10Index,
                            sizeof(WORD)) != sizeof(WORD))
            {
                return(FALSE);
            }

            // Endianize

            w0x11ID = ConvertEndianShort(w0x11ID);
            w0x10Index = ConvertEndianShort(w0x10Index);

            // Delete 0x11 entry

            if (WD8TDTDelete0x11Entry(lpstCallBack,
                                      lpstOLEFile,
                                      abyModuleName,
                                      w0x11ID,
                                      lpstTDTInfo->dwRecord0x11Offset,
                                      wNum0x11Subrecords,
                                      &dwRecord0x11Size,
                                      &b0x11Deleted,
                                      lpbyWorkBuffer) == FALSE)
            {
                return(FALSE);
            }

            // Delete 0x10 entry

            if (WD8TDTDelete0x10Entry(lpstCallBack,
                                      lpstOLEFile,
                                      abyModuleName,
                                      w0x10Index,
                                      lpstTDTInfo->dwRecord0x10Offset,
                                      wNum0x10Subrecords,
                                      &dwRecord0x10Size,
                                      &b0x10Deleted,
                                      lpbyWorkBuffer) == FALSE)
            {
                return(FALSE);
            }

            if (b0x11Deleted == TRUE && b0x10Deleted == TRUE)
            {
                --wNum0x11Subrecords;
                --wNum0x10Subrecords;
                wDeleteIndex = w;
                break;
            }

            if (b0x11Deleted == TRUE || b0x10Deleted == TRUE)
            {
                // Only one deleted is an error condition

                return(FALSE);
            }

            dwOffset += 24;
        }

        if (w == wNum0x01Subrecords)
        {
            // No records were deleted, so that's it

            break;
        }

        // Delete the keyboard associations if any

        if (WD8TDTDelete0x03Entries(lpstCallBack,
                                    lpstOLEFile,
                                    w0x11ID,
                                    lpstTDTInfo->dwRecord0x03Offset,
                                    &dwRecord0x03Size,
                                    lpbyWorkBuffer) == FALSE)
        {
            return(FALSE);
        }

        // Update the record count

        --wNum0x01Subrecords;
        w = ConvertEndianShort(wNum0x01Subrecords);
        if (OLESeekWrite(lpstCallBack,
                         lpstOLEFile,
                         lpstTDTInfo->dwRecord0x01Offset + 1,
                         (LPBYTE)&w,
                         sizeof(WORD)) != sizeof(WORD))
        {
            return(FALSE);
        }

        // Go through the records and update the 0x10 index
        //  of those that are greater than the deleted index,
        //  also rewriting the records so that the entry
        //  to delete is not written

        dwOffset = lpstTDTInfo->dwRecord0x01Offset + 5;
        dwDstOffset = dwOffset;
        for (w=0;w<=wNum0x01Subrecords;w++)
        {
            if (w != wDeleteIndex)
            {
                // Get the whole record

                if (OLESeekRead(lpstCallBack,
                                lpstOLEFile,
                                dwOffset,
                                lpbyWorkBuffer,
                                0x18) != 0x18)
                {
                    return(FALSE);
                }

                // Check the index

                wTemp = *(LPWORD)(lpbyWorkBuffer + 4);
                wTemp = ConvertEndianShort(wTemp);
                if (wTemp > w0x10Index)
                {
                    --wTemp;
                    wTemp = ConvertEndianShort(wTemp);
                    *(LPWORD)(lpbyWorkBuffer+4) = wTemp;
                }

                // Write the whole record

                if (OLESeekWrite(lpstCallBack,
                                 lpstOLEFile,
                                 dwDstOffset,
                                 lpbyWorkBuffer,
                                 0x18) != 0x18)
                {
                    return(FALSE);
                }

                dwDstOffset += 24;
            }

            dwOffset += 24;
        }

        dwRecord0x01Size -= 24;
        bAtLeastOneDeleted = TRUE;
    }

    if (bAtLeastOneDeleted == FALSE)
    {
        // Nothing was deleted

        *lpbDeleted = FALSE;

        return(TRUE);
    }

    // Something was deleted

    *lpbDeleted = TRUE;


    /////////////////////////////////////////////////////////////
    // Update subrecord counts for record types 0x01, 0x10, 0x11
    /////////////////////////////////////////////////////////////

    // Update subrecord count of record type 0x01

    wTemp = ConvertEndianShort(wNum0x01Subrecords);
    if (OLESeekWrite(lpstCallBack,
                     lpstOLEFile,
                     lpstTDTInfo->dwRecord0x01Offset + 1,
                     (LPBYTE)&wTemp,
                     sizeof(WORD)) != sizeof(WORD))
    {
        return(FALSE);
    }

    // Update subrecord count of record type 0x10

    wTemp = ConvertEndianShort(wNum0x10Subrecords);
    if (OLESeekWrite(lpstCallBack,
                     lpstOLEFile,
                     lpstTDTInfo->dwRecord0x10Offset + 3,
                     (LPBYTE)&wTemp,
                     sizeof(WORD)) != sizeof(WORD))
    {
        return(FALSE);
    }

    // Update subrecord count of record type 0x11

    wTemp = ConvertEndianShort(wNum0x11Subrecords);
    if (OLESeekWrite(lpstCallBack,
                     lpstOLEFile,
                     lpstTDTInfo->dwRecord0x11Offset + 1,
                     (LPBYTE)&wTemp,
                     sizeof(WORD)) != sizeof(WORD))
    {
        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Compact everything
    /////////////////////////////////////////////////////////////

    if (wNum0x01Subrecords == 0)
        dwRecord0x01Size = 0;

    if (wNum0x10Subrecords == 0)
        dwRecord0x10Size = 0;

    if (wNum0x11Subrecords == 0)
        dwRecord0x11Size = 0;

    // Get regions

    dwRegionOffset[0] = lpstTDTInfo->dwRecord0x01Offset;
    dwRegionNewSize[0] = dwRecord0x01Size;
    dwRegionOrigSize[0] = lpstTDTInfo->dwRecord0x01Size;
    dwRegionOffset[1] = lpstTDTInfo->dwRecord0x10Offset;
    dwRegionNewSize[1] = dwRecord0x10Size;
    dwRegionOrigSize[1] = lpstTDTInfo->dwRecord0x10Size;
    dwRegionOffset[2] = lpstTDTInfo->dwRecord0x11Offset;
    dwRegionNewSize[2] = dwRecord0x11Size;
    dwRegionOrigSize[2] = lpstTDTInfo->dwRecord0x11Size;

    if (lpstTDTInfo->dwRecord0x03Offset != 0xFFFFFFFF)
    {
        nNumRegions = 4;

        // Get 0x03 region

        dwRegionOffset[3] = lpstTDTInfo->dwRecord0x03Offset;
        dwRegionNewSize[3] = dwRecord0x03Size;
        dwRegionOrigSize[3] = lpstTDTInfo->dwRecord0x03Size;
    }
    else
        nNumRegions = 3;


    // Sort regions by offset using bubble sort

    for (i=0;i<nNumRegions;i++)
    {
        for (j=i+1;j<nNumRegions;j++)
        {
            if (dwRegionOffset[j] < dwRegionOffset[i])
            {
                // Do a swap

                dwTemp = dwRegionOffset[j];
                dwRegionOffset[j] = dwRegionOffset[i];
                dwRegionOffset[i] = dwTemp;

                dwTemp = dwRegionNewSize[j];
                dwRegionNewSize[j] = dwRegionNewSize[i];
                dwRegionNewSize[i] = dwTemp;

                dwTemp = dwRegionOrigSize[j];
                dwRegionOrigSize[j] = dwRegionOrigSize[i];
                dwRegionOrigSize[i] = dwTemp;
            }
        }
    }

    // Do compaction

    dwDstOffset = dwRegionOffset[0];
    for (i=0;i<nNumRegions;i++)
    {
        // Copy the region

        if (OLECopyBytes(lpstCallBack,
                         lpstOLEFile,
                         dwRegionOffset[i],
                         dwDstOffset,
                         dwRegionNewSize[i],
                         lpbyWorkBuffer) == FALSE)
        {
            return(FALSE);
        }

        dwDstOffset += dwRegionNewSize[i];

        // Copy the bytes after the region

        if (i < (nNumRegions - 1))
        {
            // The bytes consist of the bytes after the region
            //  up to the byte immediately before the next region.

            dwTemp = dwRegionOffset[i + 1] -
                (dwRegionOffset[i] + dwRegionOrigSize[i]);
        }
        else
        {
            // The bytes consist of the bytes after the region
            //  up to the end of the template data table

            dwTemp = (lpstTDTInfo->dwTDTOffset + lpstTDTInfo->dwTDTSize) -
                (dwRegionOffset[i] + dwRegionOrigSize[i]);
        }

        // Check to see if record 0x07 is within this region

        if (lpstTDTInfo->dwRecord0x07Offset != 0xFFFFFFFF)
        {
            if (dwRegionOffset[i] + dwRegionOrigSize[i] <=
                lpstTDTInfo->dwRecord0x07Offset &&
                lpstTDTInfo->dwRecord0x07Offset <
                dwRegionOffset[i] + dwRegionOrigSize[i] + dwTemp)
            {
                // The new offset of record 0x07 is lessened
                //  by the distance moved

                dwRecord0x07ShiftOffset =
                    dwRegionOffset[i] + dwRegionOrigSize[i] - dwDstOffset;
            }
        }

        if (OLECopyBytes(lpstCallBack,
                         lpstOLEFile,
                         dwRegionOffset[i] + dwRegionOrigSize[i],
                         dwDstOffset,
                         dwTemp,
                         lpbyWorkBuffer) == FALSE)
        {
            return(FALSE);
        }

        dwDstOffset += dwTemp;
    }

    // Zero out the left over bytes

    if (OLEWriteZeroes(lpstCallBack,
                       lpstOLEFile,
                       dwDstOffset,
                       lpstTDTInfo->dwTDTOffset + lpstTDTInfo->dwTDTSize -
                           dwDstOffset,
                       lpbyWorkBuffer) == FALSE)
    {
        return(FALSE);
    }

    // Update the records in record 0x07

    if (WD8UpdateRecord0x07AfterMove(lpstCallBack,
                                     lpstOLEFile,
                                     lpstTDTInfo->dwRecord0x07Offset,
                                     lpstTDTInfo->dwRecord0x07Size,
                                     dwRecord0x07ShiftOffset,
                                     lpbyWorkBuffer) == FALSE)
    {
        return(FALSE);
    }

    // Update the template data table size

    lpstTDTInfo->dwTDTSize = dwDstOffset - lpstTDTInfo->dwTDTOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8OpenTDT()
//
// Description:
//  Opens the WordDocument stream to get the TDT offset and size.
//  The function then tries to get the TDT info from the 1Table
//  stream.  If not successful, the function tries the 0Table
//  stream.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL WD8OpenTDT
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // non NULL if cache
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPWD8_TDT_INFO      lpstTDTInfo,        // Ptr to TDT structure
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 256 bytes
)
{
    BOOL                bResult;

    // Open the WordDocument stream to get the template data
    //  table location and size

    if (OLEOpenStreamEntryNum(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              lpstVBA5Scan->u.stWD8.wWordDocumentEntry,
                              lpbyStreamBATCache) != OLE_OK)
    {
        // Failed to open WordDocument stream

        return(FALSE);
    }

    // Get the template data table offset

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    346,
                    (LPBYTE)&lpstTDTInfo->dwTDTOffset,
                    sizeof(DWORD)) != sizeof(DWORD))
    {
        return(FALSE);
    }

    lpstTDTInfo->dwTDTOffset = ConvertEndianLong(lpstTDTInfo->dwTDTOffset);

    // Get the template data table size

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    350,
                    (LPBYTE)&lpstTDTInfo->dwTDTSize,
                    sizeof(DWORD)) != sizeof(DWORD))
    {
        return(FALSE);
    }

    lpstTDTInfo->dwTDTSize = ConvertEndianLong(lpstTDTInfo->dwTDTSize);

    // Try the 1Table stream first, and then the 0Table stream

    // Open the 1Table stream to get the template data
    //  table location and size

    if (OLEOpenStreamEntryNum(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              lpstVBA5Scan->u.stWD8.w1TableEntry,
                              lpbyStreamBATCache) != OLE_OK)
    {
        // Failed to open 1Table stream

        return(FALSE);
    }

    // Get the info for record types 0x01, 0x10, and 0x11

    bResult = WD8GetTDTRecordInfo(lpstCallBack,
                                  lpstOLEFile,
                                  lpstTDTInfo,
                                  lpbyWorkBuffer);

    if (bResult == FALSE)
    {
        if (lpstVBA5Scan->u.stWD8.w0TableEntry == 0xFFFF)
            return(FALSE);

        if (OLEOpenStreamEntryNum(lpstCallBack,
                                  lpstOLEFile->hFile,
                                  lpstOLEFile,
                                  lpstVBA5Scan->u.stWD8.w0TableEntry,
                                  lpbyStreamBATCache) != OLE_OK)
            return(FALSE);

        if (WD8GetTDTRecordInfo(lpstCallBack,
                                lpstOLEFile,
                                lpstTDTInfo,
                                lpbyWorkBuffer) == FALSE)
        {
            // Failed to get TDT info

            return(FALSE);
        }
    }

    // Success

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8DeleteModule()
//
// Description:
//  Deletes a module from a Word 8.0 document using the following
//  steps:
//
//      1. Delete the entry from the VBA streams
//      2. Open the WordDocument stream to get the template
//         data table location and size.
//      3. Open the 1Table stream to do the following:
//          a. Parse through the template data table to obtain
//             the location, size, and number of subrecords
//             of record types 0x01, 0x10, and 0x11.
//          b. Iterate through the subrecords of record type 0x01
//             to do the following:
//              i. Get the subrecord number for record type 0x10
//                 and 0x11 containing the information.
//              ii. If both records contain PROJECT.MODULENAME.,
//                  then delete all three subrecords.
//              iii. Update subrecord type 0x10 pointers of record
//                   type 0x01 to account for deleted subrecord 0x10.
//              iv. Update record size and number of subrecords.
//          c. If subrecords were eliminated, fill in the gaps
//             between the records.
//      4. If subrecords were eliminated, update the template data
//         table size field in the WordDocument stream.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL WD8DeleteModule
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // non NULL if cache
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 0.75K bytes
)
{
    BOOL                bDeleted;
    LPWD8_TDT_INFO      lpstTDTInfo;

    lpstTDTInfo = (LPWD8_TDT_INFO)lpbyWorkBuffer;
    lpbyWorkBuffer += sizeof(WD8_TDT_INFO_T);

    if (VBA5DeleteModule(lpstCallBack,
                         lpstOLEFile,
                         lpbyStreamBATCache,
                         abyModuleName,
                         lpstVBA5Scan,
                         lpbyWorkBuffer) == FALSE)
    {
        // Failed to delete entry from VBA project streams

        return(FALSE);
    }

    if (WD8OpenTDT(lpstCallBack,
                   lpstOLEFile,
                   lpbyStreamBATCache,
                   lpstVBA5Scan,
                   lpstTDTInfo,
                   lpbyWorkBuffer) == FALSE)
    {
        // Failed to open TDT

        return(FALSE);
    }

    // Delete the desired module entries

    if (WD8TDTDeleteModule(lpstCallBack,
                           lpstOLEFile,
                           lpstTDTInfo,
                           abyModuleName,
                           &bDeleted,
                           lpbyWorkBuffer) == FALSE)
    {
        return(FALSE);
    }

    if (bDeleted == FALSE)
    {
        // Nothing deleted

        return(TRUE);
    }

    // Update the template data table size in the WordDocument stream

    if (OLEOpenStreamEntryNum(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              lpstVBA5Scan->u.stWD8.wWordDocumentEntry,
                              lpbyStreamBATCache) != OLE_OK)
    {
        // Failed to open WordDocument stream

        return(FALSE);
    }

    // Update the template data table size

    lpstTDTInfo->dwTDTSize = ConvertEndianLong(lpstTDTInfo->dwTDTSize);
    if (OLESeekWrite(lpstCallBack,
                     lpstOLEFile,
                     350,
                     (LPBYTE)&lpstTDTInfo->dwTDTSize,
                     sizeof(DWORD)) != sizeof(DWORD))
    {
        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8CheckMacrosDeletable()
//
// Description:
//  Checks to see whether there are any modules left in the
//  VBA storage and zeroes out the Macros storage name if that
//  is the case.
//
//  The function determines that there are no more modules
//  if all of the following conditions are met:
//      1. The names of streams in the VBA storage only
//         consist of the following:
//          - ""
//          - "_*"
//          - "dir"
//          - "ThisDocument"
//          - None of the above and the stream length is zero
//      2. The "ThisDocument" stream consists only of lines
//         that begin with "Attribute ", case insensitive.
//      3. The TDT does not contain any 0x01, 0x10, nor 0x11
//         records.
//
// Returns:
//  TRUE        If the ThisDocument stream was successfully
//              analyzed as empty
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL WD8CheckMacrosDeletable
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // non NULL if cache
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 0.5K bytes
)
{
    OLE_OPEN_SIB_T      stOpenSib;
#if !defined(SYM_DOSX)
    LZNT_T              stLZNT;
#endif
    LPWD8_TDT_INFO      lpstTDTInfo;

    lpstTDTInfo = (LPWD8_TDT_INFO)lpbyWorkBuffer;
    lpbyWorkBuffer += sizeof(WD8_TDT_INFO_T);

    // Determine whether there is any other module stream
    //  besides "ThisDocument"

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = (LPWORD)lpbyWorkBuffer;

    // Get child of VBA storage

    stOpenSib.lpawSibs[0] = lpstVBA5Scan->wVBAChildEntry;
    stOpenSib.nNumWaitingSibs = 1;
    while (1)
    {
        if (OLEOpenSiblingCB(lpstCallBack,
                             lpstOLEFile->hFile,
                             lpstOLEFile,
                             WD8CheckMacrosDeletableCB,
                             NULL,
                             lpbyStreamBATCache,
                             &stOpenSib) == OLE_OK)
        {
            // If the stream length is not zero, return

            if (OLEStreamLen(lpstOLEFile) != 0)
                return(FALSE);

            return(FALSE);
        }
        else
            break;
    }

    // Open the ThisDocument stream

    stOpenSib.lpawSibs[0] = lpstVBA5Scan->wVBAChildEntry;
    stOpenSib.nNumWaitingSibs = 1;

    if (OLEOpenSiblingCB(lpstCallBack,
                         lpstOLEFile->hFile,
                         lpstOLEFile,
                         WD8OpenThisDocumentCB,
                         NULL,
                         lpbyStreamBATCache,
                         &stOpenSib) == OLE_OK)
    {
        // Determine whether the ThisDocument stream consists of only
        //  Attribute lines

        if (VBA5LZNTModuleInit(lpstCallBack,
                               lpstOLEFile,
                               &stLZNT) == FALSE)
        {
            // Failed to initialize for reading source text string

            return(FALSE);
        }

        if (VBA5LZNTStreamAttributeOnly(&stLZNT,
                                        lpbyWorkBuffer) == FALSE)
        {
            // The ThisDocument stream contains non-attribute lines

            return(FALSE);
        }
    }

    // Check the TDT

    if (WD8OpenTDT(lpstCallBack,
                   lpstOLEFile,
                   lpbyStreamBATCache,
                   lpstVBA5Scan,
                   lpstTDTInfo,
                   lpbyWorkBuffer) == FALSE)
    {
        // Failed to open TDT

        return(FALSE);
    }

    if (lpstTDTInfo->dwRecord0x01Offset != 0xFFFFFFFF ||
        lpstTDTInfo->dwRecord0x10Offset != 0xFFFFFFFF ||
        lpstTDTInfo->dwRecord0x11Offset != 0xFFFFFFFF)
    {
        // TDT contains at least one of record 0x01, 0x10, or 0x11

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8HasAutoText()
//
// Parameters:
//  lpstStream              Ptr to open WordDocument stream
//  lpbHasAutoText          Ptr to BOOL to store status
//
// Description:
//  Checks the value at offset 0xE6.  This value stores the
//  size of the glossary string table.  If the value is non-zero,
//  the function sets *lpbHasAutoText to TRUE.
//
//  Otherwise, the function checks the second header.  The offset
//  of the second header is 0x200 multiplied by the WORD value at
//  offset 0x08.
//
//  If the value at offset 0xE6 relative to the second header is
//  non-zero, the function sets *lpbHasAutoText to TRUE.
//
//  Otherwise the function sets *lpbHasAutoText to FALSE.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL WD8HasAutoText
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBOOL              lpbHasAutoText
)
{
    DWORD               dwValue;
    WORD                w2ndHdrBlk;

    // Read DWORD at offset 0xE6

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    0xE6,
                    (LPBYTE)&dwValue,
                    sizeof(DWORD)) != sizeof(DWORD))
    {
        // Failed to read DWORD at offset 0xE6

        return(FALSE);
    }

    dwValue = DWENDIAN(dwValue);
    if (dwValue != 0)
    {
        *lpbHasAutoText = TRUE;
        return(TRUE);
    }

    // Get the block number of the second header

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    0x08,
                    (LPBYTE)&w2ndHdrBlk,
                    sizeof(WORD)) != sizeof(WORD))
    {
        // Failed to read WORD at offset 0x08

        return(FALSE);
    }

    w2ndHdrBlk = WENDIAN(w2ndHdrBlk);

    // Read DWORD at offset 0xE6 relative to the second header

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    (DWORD)w2ndHdrBlk * (DWORD)0x200 + (DWORD)0xE6,
                    (LPBYTE)&dwValue,
                    sizeof(DWORD)) != sizeof(DWORD))
    {
        // Failed to read DWORD at offset 0xE6

        return(FALSE);
    }

    dwValue = DWENDIAN(dwValue);
    if (dwValue != 0)
    {
        *lpbHasAutoText = TRUE;
        return(TRUE);
    }

    *lpbHasAutoText = FALSE;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL WD8ChangeToDocument()
//
// Parameters:
//  lpstStream              Ptr to open WordDocument stream
//
// Description:
//  Resets the zeroth bit of the byte at offset ten to zero.
//  This ensures that Word 97 interprets the document as
//  a document and not a template.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL WD8ChangeToDocument
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile         // OLE file for module to repair
)
{
    BYTE                byFlags;

    // Read Word Document flag byte

    if (OLESeekRead(lpstCallBack,
                    lpstOLEFile,
                    10,
                    &byFlags,
                    sizeof(BYTE)) != sizeof(BYTE))
    {
        // Failed to read flag byte

        return(FALSE);
    }

    // Make it a document

    byFlags &= 0xFE;

    // Write Word Document flag byte

    if (OLESeekWrite(lpstCallBack,
                     lpstOLEFile,
                     10,
                     &byFlags,
                     sizeof(BYTE)) != sizeof(BYTE))
    {
        // Failed to write flag byte

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS WD8MacroVirusRepair()
//
// Description:
//  Repairs a file of Word 8.0 macro viruses.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS WD8MacroVirusRepair
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file structure
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,   // Virus sig info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
)
{
    LPBYTE          lpbyStreamBATCache;
    OLE_OPEN_SIB_T  stOpenSib;
    WORD            awSibs[MAX_VBA5_SIB_DEPTH];
    BYTE            abyModuleName[MAX_OLE_NAME_LEN+1];
    WORD            wModuleCount;

    BOOL            bDelete;
    BOOL            bMore;

    lpbyStreamBATCache = NULL;

    if (WD8FindAssociatedStreams(lpstCallBack,
                                 lpstOLEFile,
                                 lpstVBA5Scan) == FALSE)
    {
        // Could not find associated streams

        return(EXTSTATUS_NO_REPAIR);
    }

    // Iterate through the modules to delete

    if (VBA5CountNonEmptyModules(lpstCallBack,
                                 lpstOLEFile,
                                 lpstVBA5Scan->wVBAChildEntry,
                                 &wModuleCount,
                                 lpbyWorkBuffer) == FALSE)
    {
        // Failed to get module count

        return(EXTSTATUS_NO_REPAIR);
    }

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = awSibs;
    bMore = TRUE;
    while (bMore)
    {
        // awSibs[0] contains the index of the child of the VBA storage

        awSibs[0] = lpstVBA5Scan->wVBAChildEntry;
        stOpenSib.nNumWaitingSibs = 1;
        while (1)
        {
            if (VBA5OpenCandidateModule(lpstCallBack,
                                        lpstOLEFile,
                                        abyModuleName,
                                        lpbyStreamBATCache,
                                        &stOpenSib) == TRUE)
            {
                // Scan module

                if (WD8RepairModule(lpstCallBack,
                                    lpstOLEFile,
                                    wModuleCount,
                                    abyModuleName,
                                    &bDelete,
                                    lpstVirusSigInfo,
                                    lpbyWorkBuffer) == FALSE)
                {
                    // Error repairing this module

                    return(EXTSTATUS_NO_REPAIR);
                }

                if (bDelete == TRUE)
                {
                    if (WD8DeleteModule(lpstCallBack,
                                        lpstOLEFile,
                                        lpbyStreamBATCache,
                                        abyModuleName,
                                        lpstVBA5Scan,
                                        lpbyWorkBuffer) == FALSE)
                    {
                        // Error deleting the module

                        return(EXTSTATUS_NO_REPAIR);
                    }

                    if (OLEGetChildOfStorage(lpstCallBack,
                                             lpstOLEFile->hFile,
                                             lpstOLEFile,
                                             lpstVBA5Scan->wVBAEntry,
                                             &lpstVBA5Scan->wVBAChildEntry) ==
                        FALSE)
                    {
                        // Error getting the child of the VBA storage

                        return(EXTSTATUS_NO_REPAIR);
                    }

                    // Need to resynchronize

                    // Resynchronization assumes that repairs without
                    //  deletions will not do a blind repair.

                    break;
                }
            }
            else
            {
                // No more candidate modules

                bMore = FALSE;
                break;
            }
        }
    }

    // Delete __SRP_ streams and increment second ushort
    //  of _VBA_PROJECT stream

    if (VBA5UpdateForRecompilation(lpstCallBack,
                                   lpstOLEFile,
                                   lpbyStreamBATCache,
                                   lpstVBA5Scan,
                                   lpbyWorkBuffer) == FALSE)
    {
        // Error updating for recompilation

        return(EXTSTATUS_NO_REPAIR);
    }

    // Determine whether to get rid of the Macros storage

    if (WD8CheckMacrosDeletable(lpstCallBack,
                                lpstOLEFile,
                                lpbyStreamBATCache,
                                lpstVBA5Scan,
                                lpbyWorkBuffer) == TRUE)
    {
        // Delete the Macros storage

        if (OLEUnlinkEntry(lpstCallBack,
                           lpstOLEFile,
                           lpstVBA5Scan->u.stWD8.
                               wParentStorageEntry,
                           lpstVBA5Scan->u.stWD8.
                               wMacrosEntry) == FALSE)
        {
            // Failed to delete Macros storage

            return(FALSE);
        }
    }

    return(EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  int XL97CheckMacrosDeletableCB()
//
// Description:
//  The callback checks the name and does the following:
//      1. If the name is "", "_*", or "dir", the function just
//         returns CONTINUE.
//      2. If the name is not "ThisWorkbook" and not Sheet[0-9]+,
//         the function returns RETURN.
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If the entry is a candidate
//  OLE_OPEN_CB_STATUS_CONTINUE     If the entry is not
//
//********************************************************************

int XL97CheckMacrosDeletableCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    int                 i;

    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it any of "", "_*", or "dir"

        if (ConvertEndianShort(lpstEntry->uszName[0]) == '_' ||
            lpstEntry->uszName[0] == 0 ||
            WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszdir,
                     MAX_OLE_NAME_LEN) == 0)
        {
            return(OLE_OPEN_CB_STATUS_CONTINUE);
        }

        // Is it "ThisWorkbook"?

        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszThisWorkbook,
                     MAX_OLE_NAME_LEN) == 0)
        {
            return(OLE_OPEN_CB_STATUS_CONTINUE);
        }

        // Does it begin with something other than "Sheet"

        if (ConvertEndianShort(lpstEntry->uszName[0]) != 'S' ||
            ConvertEndianShort(lpstEntry->uszName[1]) != 'h' ||
            ConvertEndianShort(lpstEntry->uszName[2]) != 'e' ||
            ConvertEndianShort(lpstEntry->uszName[3]) != 'e' ||
            ConvertEndianShort(lpstEntry->uszName[4]) != 't')
        {
            // The stream name does not begin with Sheet

            return(OLE_OPEN_CB_STATUS_RETURN);
        }

        i = 5;
        do
        {
            WORD wChar;

            wChar = ConvertEndianShort(lpstEntry->uszName[i++]);

            if (wChar < '0' || '9' < wChar)
            {
                // Not a number

                return(OLE_OPEN_CB_STATUS_RETURN);
            }
        }
        while (i < MAX_OLE_NAME_LEN && lpstEntry->uszName[i] != 0);
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int XL97OpenThisWorkbookSheetCB()
//
// Description:
//  Callback for opening "ThisDocument" stream and "Sheet" streams.
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If the entry is a candidate
//  OLE_OPEN_CB_STATUS_CONTINUE     If the entry is not
//
//********************************************************************

int XL97OpenThisWorkbookSheetCB
(
    LPOLE_DIR_ENTRY     lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (WStrNCmp(lpstEntry->uszName,
                     (LPWORD)gauszThisDocument,
                     MAX_OLE_NAME_LEN) == 0)
        {
            return(OLE_OPEN_CB_STATUS_RETURN);
        }

        // Does it begin with "Sheet"

        if (ConvertEndianShort(lpstEntry->uszName[0]) == 'S' &&
            ConvertEndianShort(lpstEntry->uszName[1]) == 'h' &&
            ConvertEndianShort(lpstEntry->uszName[2]) == 'e' &&
            ConvertEndianShort(lpstEntry->uszName[3]) == 'e' &&
            ConvertEndianShort(lpstEntry->uszName[4]) == 't')
        {
            // The stream name begins with Sheet

            return(OLE_OPEN_CB_STATUS_RETURN);
        }
    }

    return(OLE_OPEN_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL XL97CheckMacrosDeletable()
//
// Description:
//  Checks to see whether there are any modules left in the
//  VBA storage and zeroes out the _VBA_PROJECT_CUR storage name
//  if that is the case.
//
//  The function determines that there are no more modules
//  if all of the following conditions are met:
//      1. The names of streams in the VBA storage only
//         consist of the following:
//          - ""
//          - "_*"
//          - "dir"
//          - "ThisDocument"
//          - None of the above and the stream length is zero
//      2. The "ThisWorkbook" and "Sheet#" stream consists only
//         of lines that begin with "Attribute ", case insensitive.
//
// Returns:
//  TRUE        If the ThisWorkbook and Sheet streams were
//              successfully analyzed as empty
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL XL97CheckMacrosDeletable
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // non NULL if cache
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 256 bytes
)
{
    OLE_OPEN_SIB_T      stOpenSib;
    WORD                awSibs[MAX_VBA5_SIB_DEPTH];
#if !defined(SYM_DOSX)
    LZNT_T              stLZNT;
#endif

    // Determine whether there is any other module stream
    //  besides "ThisWorkbook" or "Sheet#"

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = awSibs;

    // Get child of VBA storage

    awSibs[0] = lpstVBA5Scan->wVBAChildEntry;
    stOpenSib.nNumWaitingSibs = 1;
    while (1)
    {
        if (OLEOpenSiblingCB(lpstCallBack,
                             lpstOLEFile->hFile,
                             lpstOLEFile,
                             XL97CheckMacrosDeletableCB,
                             NULL,
                             lpbyStreamBATCache,
                             &stOpenSib) == OLE_OK)
        {
            // If the stream length is not zero, return

            if (OLEStreamLen(lpstOLEFile) != 0)
                return(FALSE);

            return(FALSE);
        }
        else
            break;
    }

    // Open the ThisWorkbook and Sheet streams

    awSibs[0] = lpstVBA5Scan->wVBAChildEntry;
    stOpenSib.nNumWaitingSibs = 1;
    while (1)
    {
        if (OLEOpenSiblingCB(lpstCallBack,
                             lpstOLEFile->hFile,
                             lpstOLEFile,
                             XL97OpenThisWorkbookSheetCB,
                             NULL,
                             lpbyStreamBATCache,
                             &stOpenSib) != OLE_OK)
        {
            // No more streams to check?

            return(TRUE);
        }

        // Determine whether the stream consists of only
        //  Attribute lines

        if (VBA5LZNTModuleInit(lpstCallBack,
                               lpstOLEFile,
                               &stLZNT) == FALSE)
        {
            // Failed to initialize for reading source text string

            return(FALSE);
        }

        if (VBA5LZNTStreamAttributeOnly(&stLZNT,
                                        lpbyWorkBuffer) == FALSE)
        {
            // The stream contained something other than
            //  Attribute lines

            return(FALSE);
        }
    }
}


//********************************************************************
//
// Function:
//  BOOL XL97WorkbookRemoveVBP()
//
// Description:
//  Opens the Workbook stream at the given entry, parses through
//  it and replaces the OBPROJ: Visual Basic Project (record 0xD3)
//  with record 0xFFFF.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL XL97WorkbookRemoveVBP
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // non NULL if cache
    WORD                wWorkbookEntryNum   // Workbook entry number
)
{
    DWORD               dwOffset;
    WORD                wRecType;
    WORD                wRecLen;
    WORD                w;
    BYTE                byTemp;

    // Open the Workbook stream

    if (OLEOpenStreamEntryNum(lpstCallBack,
                              lpstOLEFile->hFile,
                              lpstOLEFile,
                              wWorkbookEntryNum,
                              lpbyStreamBATCache) != OLE_OK)
    {
        // Failed opening Workbook stream

        return(FALSE);
    }

    // Parse through the records

    dwOffset = 0;
    while (dwOffset < OLEStreamLen(lpstOLEFile))
    {
        // Read the record type and record length

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&wRecType,
                        sizeof(WORD)) != sizeof(WORD) ||
            OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset+sizeof(WORD),
                        (LPBYTE)&wRecLen,
                        sizeof(WORD)) != sizeof(WORD))
        {
            // Failed to read record type and length

            return(FALSE);
        }

        // Endianize

        wRecType = ConvertEndianShort(wRecType);
        wRecLen = ConvertEndianShort(wRecLen);

        if (wRecType == 0xD3)
        {
            // This is the one, convert it to 0x00CD

            wRecType = 0x00CD;

            wRecType = ConvertEndianShort(wRecType);

            if (OLESeekWrite(lpstCallBack,
                             lpstOLEFile,
                             dwOffset,
                             (LPBYTE)&wRecType,
                             sizeof(WORD)) != sizeof(WORD))
            {
                // Failed to rewrite record type as 0xFFFF

                return(FALSE);
            }

            // Zero out the rest of the record

            byTemp = 0;
            for (w=0;w<wRecLen;w++)
            {
                if (OLESeekWrite(lpstCallBack,
                                 lpstOLEFile,
                                 dwOffset + sizeof(WORD) + sizeof(WORD) + w,
                                 &byTemp,
                                 sizeof(BYTE)) != sizeof(BYTE))
                {
                    return(FALSE);
                }
            }

            // Success

            return(TRUE);
        }

        // Update offset

        dwOffset += sizeof(WORD) + sizeof(WORD) + wRecLen;
    }

    // No Visual Basic Project record?!

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL97RepairModule()
//
// Description:
//  Repairs a module of an Excel 97 virus.  If the module should
//  be deleted, then *lpbDelete is set to TRUE before returning.
//
// Returns:
//  TRUE        If the repair was successful
//  FALSE       If the repair was unsuccessful
//
//********************************************************************

BOOL XL97RepairModule
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    WORD                wModuleCount,       // Count of modules
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPBOOL              lpbDelete,          // Ptr to BOOL to request delete
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,   // Virus sig info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
)
{
    LPWD_SCAN           lpstScan;
#if !defined(SYM_DOSX)
    LZNT_T              stLZNT;
#endif
    int                 i;

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    BYTE abyNameSigInterMacroHit[XL97_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNameSigIntraMacroHit[XL97_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNamedCRCSigHit[XL97_NAMED_CRC_SIG_HIT_ARRAY_SIZE];
    BYTE abyMacroSigHit[XL97_MACRO_SIG_HIT_ARRAY_SIZE];
    BYTE abyCRCHit[XL97_CRC_HIT_ARRAY_SIZE];
#else
    LPBYTE              lpbyHitMem;
#endif

    /////////////////////////////////////////////////////////////
    // Initialize the scan structure
    /////////////////////////////////////////////////////////////

    lpstScan = (LPWD_SCAN)lpbyWorkBuffer;

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

    lpstScan->wMacroCount = wModuleCount;

    lpstScan->lpstCallBack = lpstCallBack;
    lpstScan->lpstOLEStream = lpstOLEFile;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;
    lpstScan->uScan.stVBA5.lpstLZNT = &stLZNT;

    // Copy the name

    i = 0;
    while (1)
    {
        lpstScan->abyName[i] = abyModuleName[i];
        if (abyModuleName[i++] == 0)
            break;
    }

    WDInitHitBitArrays(&gstXL97SigSet,lpstScan);

    /////////////////////////////////////////////////////////////
    // Scan and apply repair to module
    /////////////////////////////////////////////////////////////

    // Scan module

    VBA5ScanModule(&gstXL97SigSet,lpstScan);

    // Apply repair signature

    *lpbDelete = FALSE;

    if (MVPCheck(MVP_XL97,
                 lpstScan->abyName,
                 lpstScan->dwCRC) == FALSE &&
        VBA5LZNTStreamAttributeOnly(&stLZNT,
                                    lpstScan->abyRunBuf) == FALSE)
    {
        // Found a non-approved macro

        *lpbDelete = TRUE;
    }
    else
    if (lpstVirusSigInfo->wID != VID_MVP)
    {
        // Apply repair signature

        if (WDApplyRepair(&gstXL97SigSet,
                          lpstScan,
                          lpbDelete,
                          lpstVirusSigInfo -
                              gstXL97SigSet.lpastVirusSigInfo,
                          0) ==
            WD_STATUS_ERROR)
        {
            // Error applying repair signature

            FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
            return(FALSE);
        }
    }

    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL97DeleteModule()
//
// Description:
//  Deletes a module from a Excel 97 document using the following
//  steps:
//
//      1. Delete the entry from the VBA streams
//      2. Open the WordDocument stream to get the template
//         data table location and size.
//      3. Open the 1Table stream to do the following:
//          a. Parse through the template data table to obtain
//             the location, size, and number of subrecords
//             of record types 0x01, 0x10, and 0x11.
//          b. Iterate through the subrecords of record type 0x01
//             to do the following:
//              i. Get the subrecord number for record type 0x10
//                 and 0x11 containing the information.
//              ii. If both records contain PROJECT.MODULENAME.,
//                  then delete all three subrecords.
//              iii. Update subrecord type 0x10 pointers of record
//                   type 0x01 to account for deleted subrecord 0x10.
//              iv. Update record size and number of subrecords.
//          c. If subrecords were eliminated, fill in the gaps
//             between the records.
//      4. If subrecords were eliminated, update the template data
//         table size field in the WordDocument stream.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL XL97DeleteModule
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file for module to repair
    LPBYTE              lpbyStreamBATCache, // non NULL if cache
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
)
{
    if (VBA5DeleteModule(lpstCallBack,
                         lpstOLEFile,
                         lpbyStreamBATCache,
                         abyModuleName,
                         lpstVBA5Scan,
                         lpbyWorkBuffer) == FALSE)
    {
        // Failed to delete entry from VBA project streams

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL97MacroVirusRepair()
//
// Description:
//  Repairs a file of Excel 97 macro viruses.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS XL97MacroVirusRepair
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file structure
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo,   // Virus sig info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
)
{
    LPBYTE          lpbyStreamBATCache;
    OLE_OPEN_SIB_T  stOpenSib;
    WORD            awSibs[MAX_VBA5_SIB_DEPTH];
    BYTE            abyModuleName[MAX_OLE_NAME_LEN+1];
    WORD            wModuleCount;

    BOOL            bDelete;
    BOOL            bMore;

    lpbyStreamBATCache = NULL;

    if (XL97FindAssociatedStreams(lpstCallBack,
                                  lpstOLEFile,
                                  lpstVBA5Scan) == FALSE)
    {
        // Could not find associated streams

        return(EXTSTATUS_NO_REPAIR);
    }

    // Count number of non-empty modules

    if (VBA5CountNonEmptyModules(lpstCallBack,
                                 lpstOLEFile,
                                 lpstVBA5Scan->wVBAChildEntry,
                                 &wModuleCount,
                                 lpbyWorkBuffer) == FALSE)
    {
        // Failed to get module count

        return(EXTSTATUS_NO_REPAIR);
    }

    // Iterate through the modules to delete

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = awSibs;
    bMore = TRUE;
    while (bMore)
    {
        // awSibs[0] contains the index of the child of the VBA storage

        awSibs[0] = lpstVBA5Scan->wVBAChildEntry;
        stOpenSib.nNumWaitingSibs = 1;
        while (1)
        {
            if (VBA5OpenCandidateModule(lpstCallBack,
                                        lpstOLEFile,
                                        abyModuleName,
                                        lpbyStreamBATCache,
                                        &stOpenSib) == TRUE)
            {
                // Scan module

                if (XL97RepairModule(lpstCallBack,
                                     lpstOLEFile,
                                     wModuleCount,
                                     abyModuleName,
                                     &bDelete,
                                     lpstVirusSigInfo,
                                     lpbyWorkBuffer) == FALSE)
                {
                    // Error repairing this module

                    return(EXTSTATUS_NO_REPAIR);
                }

                if (bDelete == TRUE)
                {
                    if (XL97DeleteModule(lpstCallBack,
                                         lpstOLEFile,
                                         lpbyStreamBATCache,
                                         abyModuleName,
                                         lpstVBA5Scan,
                                         lpbyWorkBuffer) == FALSE)
                    {
                        // Error deleting the module

                        return(EXTSTATUS_NO_REPAIR);
                    }

                    if (OLEGetChildOfStorage(lpstCallBack,
                                             lpstOLEFile->hFile,
                                             lpstOLEFile,
                                             lpstVBA5Scan->wVBAEntry,
                                             &lpstVBA5Scan->wVBAChildEntry) ==
                        FALSE)
                    {
                        // Error getting the child of the VBA storage

                        return(EXTSTATUS_NO_REPAIR);
                    }

                    // Need to resynchronize

                    // Resynchronization assumes that repairs without
                    //  deletions will not do a blind repair.

                    break;
                }
            }
            else
            {
                // No more candidate modules

                bMore = FALSE;
                break;
            }
        }
    }

    // Delete __SRP_ streams and increment second ushort
    //  of _VBA_PROJECT stream

    if (VBA5UpdateForRecompilation(lpstCallBack,
                                   lpstOLEFile,
                                   lpbyStreamBATCache,
                                   lpstVBA5Scan,
                                   lpbyWorkBuffer) == FALSE)
    {
        // Error updating for recompilation

        return(EXTSTATUS_NO_REPAIR);
    }

    // Determine whether to get rid of the _VBA_PROJECT_CUR storage

    if (XL97CheckMacrosDeletable(lpstCallBack,
                                 lpstOLEFile,
                                 lpbyStreamBATCache,
                                 lpstVBA5Scan,
                                 lpbyWorkBuffer) == TRUE)
    {
        // Remove Visual Basic Project record from Workbook stream

        if (XL97WorkbookRemoveVBP(lpstCallBack,
                                  lpstOLEFile,
                                  lpbyStreamBATCache,
                                  lpstVBA5Scan->u.stXL97.
                                      wWorkbookEntry) == FALSE)
        {
            // Failed to remove Visual Basic Project record

            return(FALSE);
        }

        // Delete the _VBA_PROJECT_CUR storage

        if (OLEUnlinkEntry(lpstCallBack,
                           lpstOLEFile,
                           lpstVBA5Scan->u.stXL97.
                               wParentStorageEntry,
                           lpstVBA5Scan->u.stXL97.
                               w_VBA_PROJECT_CUREntry) == FALSE)
        {
            // Failed to delete Macros storage

            return(FALSE);
        }
    }

    return(EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  BOOL VBA5IsFullSet()
//
// Parameters:
//  lpstSigSet          Signature set to apply
//  lpstScan            Ptr to scan structure
//  wVBAChildEntry      Child entry of VBA storage
//  lpstOpenSib         OLE open sib structure
//
// Description:
//  Determines whether all macros are part of the full set.
//
// Returns:
//  TRUE        If all non-empty modules belong to the full set
//  FALSE       If at least one non-empty module does not belong
//              to the full set
//
//********************************************************************

BOOL VBA5IsFullSet
(
    LPWD_SIG_SET        lpstSigSet,         // Signature set to apply
    LPWD_SCAN           lpstScan,           // Ptr to scan structure
    WORD                wVBAChildEntry,     // Child of VBA storage
    LPOLE_OPEN_SIB      lpstOpenSib         // OLE Open sib structure
)
{
#if !defined(SYM_DOSX)
    LZNT_T              stLZNT;
#endif

    lpstScan->uScan.stVBA5.lpstLZNT = &stLZNT;

    //////////////////////////////////////////////////////////////////
    // Iterate through modules to determine full set status
    //////////////////////////////////////////////////////////////////

    lpstOpenSib->lpawSibs[0] = wVBAChildEntry;
    lpstOpenSib->nNumWaitingSibs = 1;
    while (VBA5OpenCandidateModule(lpstScan->lpstCallBack,
                                   lpstScan->lpstOLEStream,
                                   lpstScan->abyName,
                                   NULL,
                                   lpstOpenSib) == TRUE)
    {
        WDInitHitBitArrays(lpstSigSet,lpstScan);

        // Scan module

        VBA5ScanModule(lpstSigSet,lpstScan);

        if (VBA5LZNTStreamAttributeOnly(&stLZNT,
                                        lpstScan->abyRunBuf) == TRUE)
        {
            // Empty modules are ignored when checking for membership
            //  in the full set

            continue;
        }

        if ((lpstScan->wFlags & WD_SCAN_FLAG_MACRO_IS_FULL_SET) == 0)
        {
            // Found a macro that was not part of the full set,
            //  so just return

            return(FALSE);
        }
    }

    // At this point, all the modules have been verified to be part
    //  of the full set

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS WD8DoFullSetRepair()
//
// Description:
//  Performs a full set repair on a Word 97 document.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS WD8DoFullSetRepair
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file structure
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
)
{
    OLE_OPEN_SIB_T      stOpenSib;
    WORD                awSibs[MAX_VBA5_SIB_DEPTH];
    BOOL                bMore;
    LPWD_SCAN           lpstScan;

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    BYTE abyNameSigInterMacroHit[WD8_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNameSigIntraMacroHit[WD8_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNamedCRCSigHit[WD8_NAMED_CRC_SIG_HIT_ARRAY_SIZE];
    BYTE abyMacroSigHit[WD8_MACRO_SIG_HIT_ARRAY_SIZE];
    BYTE abyCRCHit[WD8_CRC_HIT_ARRAY_SIZE];
#else
    LPBYTE              lpbyHitMem;
#endif

    /////////////////////////////////////////////////////////////
    // Initialize the scan structure
    /////////////////////////////////////////////////////////////

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

    lpstScan->lpstCallBack = lpstCallBack;
    lpstScan->lpstOLEStream = lpstOLEFile;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;

    // Initialize sibling open structure

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = awSibs;

    // Check to see if it is a full set

    if (VBA5IsFullSet(&gstWD8SigSet,
                      lpstScan,
                      lpstVBA5Scan->wVBAChildEntry,
                      &stOpenSib) == FALSE)
    {
        // The set of modules is not a full set

        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(WD_STATUS_OK);
    }

    // At this point, all the modules have been verified to be part
    //  of the full set, so delete them all

    if (WD8FindAssociatedStreams(lpstCallBack,
                                 lpstOLEFile,
                                 lpstVBA5Scan) == FALSE)
    {
        // Could not find associated streams

        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Iterate through the modules to delete

    bMore = TRUE;
    while (bMore)
    {
        // awSibs[0] contains the index of the child of the VBA storage

        awSibs[0] = lpstVBA5Scan->wVBAChildEntry;
        stOpenSib.nNumWaitingSibs = 1;
        while (1)
        {
            if (VBA5OpenCandidateModule(lpstCallBack,
                                        lpstOLEFile,
                                        lpstScan->abyName,
                                        NULL,
                                        &stOpenSib) == TRUE)
            {
                if (WD8DeleteModule(lpstCallBack,
                                     lpstOLEFile,
                                     NULL,
                                     lpstScan->abyName,
                                     lpstVBA5Scan,
                                     lpbyWorkBuffer) == FALSE)
                {
                    // Error deleting the module

                    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
                    return(EXTSTATUS_NO_REPAIR);
                }

                if (OLEGetChildOfStorage(lpstCallBack,
                                         lpstOLEFile->hFile,
                                         lpstOLEFile,
                                         lpstVBA5Scan->wVBAEntry,
                                         &lpstVBA5Scan->wVBAChildEntry) ==
                    FALSE)
                {
                    // Error getting the child of the VBA storage

                    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
                    return(EXTSTATUS_NO_REPAIR);
                }

                // Need to resynchronize

                // Resynchronization assumes that repairs without
                //  deletions will not do a blind repair.

                break;
            }
            else
            {
                // No more candidate modules

                bMore = FALSE;
                break;
            }
        }
    }

    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
    return(EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL97DoFullSetRepair()
//
// Description:
//  Performs a full set repair on an Excel 97 document.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS XL97DoFullSetRepair
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE          lpstOLEFile,        // OLE file structure
    LPVBA5_SCAN         lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
)
{
    OLE_OPEN_SIB_T      stOpenSib;
    WORD                awSibs[MAX_VBA5_SIB_DEPTH];
    BOOL                bMore;
    LPWD_SCAN           lpstScan;

#if !defined(ALLOC_MACR_SIG_HIT_MEM)
    BYTE abyNameSigInterMacroHit[XL97_NAME_SIG_INTER_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNameSigIntraMacroHit[XL97_NAME_SIG_INTRA_MACRO_HIT_ARRAY_SIZE];
    BYTE abyNamedCRCSigHit[XL97_NAMED_CRC_SIG_HIT_ARRAY_SIZE];
    BYTE abyMacroSigHit[XL97_MACRO_SIG_HIT_ARRAY_SIZE];
    BYTE abyCRCHit[XL97_CRC_HIT_ARRAY_SIZE];
#else
    LPBYTE              lpbyHitMem;
#endif

    /////////////////////////////////////////////////////////////
    // Initialize the scan structure
    /////////////////////////////////////////////////////////////

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

    lpstScan->lpstCallBack = lpstCallBack;
    lpstScan->lpstOLEStream = lpstOLEFile;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;

    // Initialize sibling open structure

    stOpenSib.nMaxWaitingSibs = MAX_VBA5_SIB_DEPTH;
    stOpenSib.lpawSibs = awSibs;

    // Check to see if it is a full set

    if (VBA5IsFullSet(&gstXL97SigSet,
                      lpstScan,
                      lpstVBA5Scan->wVBAChildEntry,
                      &stOpenSib) == FALSE)
    {
        // The set of modules is not a full set

        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(WD_STATUS_OK);
    }

    // At this point, all the modules have been verified to be part
    //  of the full set, so delete them all

    if (XL97FindAssociatedStreams(lpstCallBack,
                                  lpstOLEFile,
                                  lpstVBA5Scan) == FALSE)
    {
        // Could not find associated streams

        FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Iterate through the modules to delete

    bMore = TRUE;
    while (bMore)
    {
        // awSibs[0] contains the index of the child of the VBA storage

        awSibs[0] = lpstVBA5Scan->wVBAChildEntry;
        stOpenSib.nNumWaitingSibs = 1;
        while (1)
        {
            if (VBA5OpenCandidateModule(lpstCallBack,
                                        lpstOLEFile,
                                        lpstScan->abyName,
                                        NULL,
                                        &stOpenSib) == TRUE)
            {
                if (XL97DeleteModule(lpstCallBack,
                                     lpstOLEFile,
                                     NULL,
                                     lpstScan->abyName,
                                     lpstVBA5Scan,
                                     lpbyWorkBuffer) == FALSE)
                {
                    // Error deleting the module

                    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
                    return(EXTSTATUS_NO_REPAIR);
                }

                if (OLEGetChildOfStorage(lpstCallBack,
                                         lpstOLEFile->hFile,
                                         lpstOLEFile,
                                         lpstVBA5Scan->wVBAEntry,
                                         &lpstVBA5Scan->wVBAChildEntry) ==
                    FALSE)
                {
                    // Error getting the child of the VBA storage

                    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
                    return(EXTSTATUS_NO_REPAIR);
                }

                // Need to resynchronize

                // Resynchronization assumes that repairs without
                //  deletions will not do a blind repair.

                break;
            }
            else
            {
                // No more candidate modules

                bMore = FALSE;
                break;
            }
        }
    }

    FreeMacroSigHitMem(lpstCallBack,lpbyHitMem);
    return(EXTSTATUS_OK);
}

#endif  // #ifndef SYM_NLM



