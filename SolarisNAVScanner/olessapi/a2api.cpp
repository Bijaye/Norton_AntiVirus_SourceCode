// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/a2api.cpv   1.2   09 Nov 1998 13:42:32   DCHI  $
//
// Description:
//  A2API source file for Access 2.0 API
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/a2api.cpv  $
// 
//    Rev 1.2   09 Nov 1998 13:42:32   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.1   05 May 1998 18:26:24   DCHI
// Modified A2GetFirstMacrosDataPage() to properly get relocated page.
// 
//    Rev 1.0   29 Apr 1998 15:57:14   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "acsapi.h"
#include "crc32.h"

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyA2MSysMacrosObjectsKey[] =
{
    0x7F, 0x8F, 0x00, 0x00, 0x01,
    0x7F, 0x6F, 0x76, 0x7D, 0x76,
    0x6F, 0x60, 0x62, 0x75, 0x72, 0x76, 0x00
};

BYTE FAR gabyA2ModulesObjectsKey[] =
{
    0x7F, 0x8F, 0x00, 0x00, 0x00,
    0x7F, 0x6F, 0x72, 0x64, 0x78, 0x6D, 0x66, 0x76, 0x00
};

BYTE FAR gabyA2LineOffsetIndexes[] =
{
    A2MACRO_MAINMACRONAME,
    A2MACRO_MACRONAME,
    A2MACRO_COMMENT,
    A2MACRO_CONDITION,
    A2MACRO_PARAM1,
    A2MACRO_PARAM2,
    A2MACRO_PARAM3,
    A2MACRO_PARAM4,
    A2MACRO_PARAM5,
    A2MACRO_PARAM6,
    A2MACRO_PARAM7,
    A2MACRO_PARAM8,
    A2MACRO_PARAM9,
    A2MACRO_PARAM10
};

BYTE FAR gabyA2ELineOffsetIndexes[] =
{
    A2MACRO_PARAM1,
    A2MACRO_PARAM2,
    A2MACRO_PARAM3,
    A2MACRO_PARAM4,
    A2MACRO_PARAM5,
    A2MACRO_PARAM6,
    A2MACRO_PARAM7,
    A2MACRO_PARAM8,
    A2MACRO_PARAM9,
    A2MACRO_PARAM10,
    A2MACRO_COMMENT,
    A2MACRO_CONDITION,
    A2MACRO_MACRONAME,
    A2MACRO_MAINMACRONAME,
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

//********************************************************************
//
// Function:
//  BOOL A2Create()
//
// Parameters:
//  lplpstA2            Ptr to ptr to store A2_T ptr
//  lpvRootCookie       Root cookie for OS routines
//  lpvFile             Pointer to file object for file ops
//
// Description:
//  The function allocates memory for an A2_T structure.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2Create
(
    LPLPA2              lplpstA2,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile
)
{
    LPA2                lpstA2;

    // Allocate root structure

    if (ACSMemoryAlloc(lpvRootCookie,
                       sizeof(A2_T),
                       (LPVOID FAR *)&lpstA2) == FALSE)
        return(FALSE);

    lpstA2->lpvRootCookie = lpvRootCookie;
    lpstA2->lpvFile = lpvFile;

    *lplpstA2 = lpstA2;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2Destroy()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure to free
//
// Description:
//  The function deallocates the memory of an A2_T structure.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2Destroy
(
    LPA2               lpstA2
)
{
    if (ACSMemoryFree(lpstA2->lpvRootCookie,
                      lpstA2) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2P05GetFirstIndexPage()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  dwMasterPage        Type 0x05 master page containing index
//  lpdwPage            Ptr to DWORD for first index page number
//
// Description:
//  The function returns the page number of the first index page
//  for the given master page.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2P05GetFirstIndexPage
(
    LPA2                lpstA2,
    DWORD               dwMasterPage,
    LPDWORD             lpdwPage
)
{
    DWORD               dwByteCount;

    // Read the DWORD at offset 0x2C of the page

    if (ACSFileSeek(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    dwMasterPage * A97_PAGE_SIZE + 0x2C,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileRead(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    lpdwPage,
                    sizeof(DWORD),
                    &dwByteCount) == FALSE ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    *lpdwPage = DWENDIAN(*lpdwPage);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2P04FindIndexEntry()
//
// Parameters:
//  lpstPage            Ptr to type 0x04/0x03 page
//  bWeirdIndex         TRUE if it is a weird index
//  nKeyValueLen        Length of key value to search
//  lpabyKeyValue       Ptr to key value to search
//  lpdwPage            Ptr to DWORD for return page number
//  lpnIndex            Ptr to int for return index
//  lpnResult           Ptr to int for return result
//
// Description:
//  The function searches through the given type 0x04/0x03 page
//  for the given key value.
//
//  If the page is of type 0x04, then if the item is not present
//  the function returns TRUE and a result of -1.  If the item
//  is found, the function returns TRUE and a result of 1.  In
//  this case, *lpdwPage will contain the page number of the
//  item and *lpnIndex will contain the index of the item.
//
//  If the page is of type 0x03, then the function returns the
//  index page number of the item whose key value is the greatest
//  key value less than or equal to the key.  If the given key value
//  is less than all the key values on the page, the function
//  returns the page number as the dwBranchPage field of the page.
//  The function returns TRUE and a result of 1 in this case.
//
//  The function performs a binary search.
//
// Returns:
//  TRUE        0       Equal (returns *lpdwPage and *lpnIndex)
//           == 1       Try page (*lpdwPage)
//             -1       Not present
//  FALSE               Error
//
//********************************************************************

BOOL A2P04FindIndexEntry
(
    LPA2_P04            lpstPage,
    BOOL                bWeirdIndex,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex,
    LPINT               lpnResult
)
{
    WORD                w;
    int                 nLow, nMid, nHigh;
    LPBYTE              lpabyData;
    int                 nCandKeyValueLen;
    LPBYTE              lpabyCandKeyValue;
    int                 i;
    int                 nResult;

    w = WENDIAN(lpstPage->wItemCount);
    if (w > A2_P04_MAX_ITEMS)
        return(FALSE);

    // Perform a binary search

    lpabyData = (LPBYTE)lpstPage;
    nLow = 0;
    nHigh = w - 1;
    while (nLow <= nHigh)
    {
        nMid = (nLow + nHigh) >> 1;

        w = WENDIAN(lpstPage->awItemOffsets[nMid]);

        // Make sure the offset is within bounds

        if (w > A2_PAGE_SIZE)
            return(FALSE);

        // Check the key length
        //  The -4 is for the page/index field

        nCandKeyValueLen = lpabyData[w];
        if (w + nCandKeyValueLen >= A2_PAGE_SIZE - 4)
            return(FALSE);

        // Compare the keys

        lpabyCandKeyValue = lpabyData + w + 1;
        nResult = 0;
        for (i=0;i < nKeyValueLen && i < nCandKeyValueLen;i++)
        {
            nResult = lpabyKeyValue[i] - lpabyCandKeyValue[i];
            if (nResult != 0)
                break;
        }

        if (nResult == 0)
        {
            if (nKeyValueLen == nCandKeyValueLen)
            {
                // Found an exact match

                lpabyCandKeyValue += nCandKeyValueLen;
                if (lpstPage->abyType[0] == 0x03)
                {
                    // Return the next index page

                    *lpdwPage = ((DWORD)lpabyCandKeyValue[0]) |
                                ((DWORD)lpabyCandKeyValue[1] << 8) |
                                ((DWORD)lpabyCandKeyValue[2] << 16);

                    *lpnResult = 1;
                    return(TRUE);
                }

                // Return the index and page

                *lpnIndex = ((DWORD)lpabyCandKeyValue[0]);
                *lpdwPage = ((DWORD)lpabyCandKeyValue[1]) |
                            ((DWORD)lpabyCandKeyValue[2] << 8) |
                            ((DWORD)lpabyCandKeyValue[3] << 16);

                *lpnResult = 0;
                return(TRUE);
            }

            if (nKeyValueLen < nCandKeyValueLen)
                nResult = -1;
            else
                nResult = 1;
        }

        if (nResult < 0)
            nHigh = nMid - 1;   // Search lower half
        else
            nLow = nMid + 1;   // Search upper half
    }

    if (lpstPage->abyType[0] == 0x04)
    {
        // No exact matches

        *lpnResult = -1;
        return(TRUE);
    }

    // Otherwise, it is a type 0x03 page, so determine what the next
    //  index page is

    if (nHigh == -1)
    {
        if (bWeirdIndex == FALSE)
        {
            // Return the branch page

            w = WENDIAN(lpstPage->wBranchPageOffset);
        }
        else
        {
            // Return the next index page that is at the highest index
            //  Assume this is the branch page in the weird case

            w = WENDIAN(lpstPage->wItemCount) - 1;
            w = WENDIAN(lpstPage->awItemOffsets[w]);

            // Make sure the offset is within bounds

            if (w > A2_PAGE_SIZE)
                return(FALSE);

            w += 1 + lpabyData[w];
        }
    }
    else
    {
        if (bWeirdIndex == FALSE)
        {
            // Return the next index page that is at the nHigh index

            w = WENDIAN(lpstPage->awItemOffsets[nHigh]);

            // Make sure the offset is within bounds

            if (w > A2_PAGE_SIZE)
                return(FALSE);

            w += 1 + lpabyData[w];
        }
        else
        {
            if (nHigh == 0)
            {
                // Return the branch page

                w = WENDIAN(lpstPage->wBranchPageOffset);
            }
            else
            {
                // Return the next index page that is at the nHigh-1 index

                --nHigh;
                w = WENDIAN(lpstPage->awItemOffsets[nHigh]);

                // Make sure the offset is within bounds

                if (w > A2_PAGE_SIZE)
                    return(FALSE);

                w += 1 + lpabyData[w];
            }
        }
    }

    // Validate the offset, 4 is for the page number

    if (w > A2_PAGE_SIZE - 4)
        return(FALSE);

    *lpdwPage = ((DWORD)lpabyData[w]) |
                ((DWORD)lpabyData[w+1] << 8) |
                ((DWORD)lpabyData[w+2] << 16);

    *lpnResult = 1;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2P03CheckForWeirdIndex()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  lpstPage            Ptr to type 0x04/0x03 page
//  lpbWeirdIndex       Ptr to BOOL, set to TRUE if weird index
//
// Description:
//  The index is weird if the branch page actually follows
//  the last item in the given index page.
//
//  For a weird index, the page containing the given key value
//  is assumed to be specified in the preceding index entry.
//  For the first index entry, the page containing the given
//  key value is assumed to be in the normal branch page.
//
// Returns:
//  TRUE                On success
//  FALSE               Error
//
//********************************************************************

BOOL A2P03CheckForWeirdIndex
(
    LPA2                lpstA2,
    LPA2_P04            lpstPage,
    LPBOOL              lpbWeirdIndex
)
{
    DWORD               dwBranchPage;
    DWORD               dwOffset;
    LPBYTE              lpabyData;
    WORD                wOffset;
    DWORD               dwByteCount;
    BYTE                abyBuf[32];
    int                 nLen;
    int                 i;
    int                 nCount;

    // Validate offset of branch page

    wOffset = WENDIAN(lpstPage->wBranchPageOffset);
    if (wOffset > A2_PAGE_SIZE - 3)
        return(FALSE);

    lpabyData = (LPBYTE)lpstPage;
    dwBranchPage = (DWORD)lpabyData[wOffset] |
                   ((DWORD)lpabyData[wOffset + 1] << 8) |
                   ((DWORD)lpabyData[wOffset + 2] << 16);

    dwOffset = dwBranchPage * A2_PAGE_SIZE;

    // Validate first item offset

    wOffset = WENDIAN(lpstPage->awItemOffsets[0]);
    if (wOffset > A2_PAGE_SIZE - 1 ||
        wOffset + 1 + lpabyData[wOffset] > A2_PAGE_SIZE)
        return(FALSE);

    // Get the length and set pointer to data

    nLen = lpabyData[wOffset];
    lpabyData += 1 + wOffset;

    // Get the offset of the first item at the given offset

    if (ACSFileSeek(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    dwOffset + 0x18,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileRead(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    &wOffset,
                    sizeof(WORD),
                    &dwByteCount) == FALSE ||
        dwByteCount != sizeof(WORD))
        return(FALSE);

    // Validate the offset

    wOffset = WENDIAN(wOffset);
    if (wOffset > A2_PAGE_SIZE - 1)
        return(FALSE);

    // Read the item length

    dwOffset += wOffset;
    if (ACSFileSeek(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    dwOffset,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileRead(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    abyBuf,
                    sizeof(BYTE),
                    &dwByteCount) == FALSE ||
        dwByteCount != sizeof(BYTE))
        return(FALSE);

    if (abyBuf[0] != nLen)
    {
        *lpbWeirdIndex = FALSE;
        return(TRUE);
    }

    // The lengths are equal, so compare the values

    nCount = sizeof(abyBuf);
    ++dwOffset;
    while (nLen != 0)
    {
        if (nLen < nCount)
            nCount = nLen;

        if (ACSFileSeek(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        dwOffset,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        abyBuf,
                        nCount,
                        &dwByteCount) == FALSE ||
            dwByteCount != nCount)
            return(FALSE);

        for (i=0;i<nCount;i++)
        {
            if (abyBuf[i] != lpabyData[i])
            {
                *lpbWeirdIndex = FALSE;
                return(TRUE);
            }
        }

        dwOffset += nCount;
        lpabyData += nCount;
        nLen -= nCount;
    }

    *lpbWeirdIndex = TRUE;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2P04FindIndexedKeyValue()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  dwPage              Starting index page to search
//  nKeyValueLen        Length of key value to search
//  lpabyKeyValue       Ptr to key value to search
//  lpdwPage            Ptr to DWORD for return page number
//  lpnIndex            Ptr to int for return index
//  lpbFound            Ptr to BOOL for found result
//
// Description:
//  The function searches through the index starting with the
//  given index page for the given item.  If the item is
//  found, the function returns TRUE and sets *lpbFound to FALSE.
//  If the item is not in the index, the function returns TRUE
//  and sets *lpbFound to FALSE.
//
// Returns:
//  TRUE                On success, *lpbFound is TRUE if the given
//                      key value is found in the index and FALSE
//                      otherwise.
//  FALSE               Error
//
//********************************************************************

BOOL A2P04FindIndexedKeyValue
(
    LPA2                lpstA2,
    DWORD               dwPage,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex,
    LPBOOL              lpbFound
)
{
    LPA2_P04            lpstPage;
    DWORD               dwByteCount;
    int                 i;
    int                 nResult;
    BOOL                bFirstPage;
    BOOL                bWeirdIndex;

    lpstPage = (LPA2_P04)(lpstA2->abyTempPage);

    // Search up to 256 pages

    bFirstPage = TRUE;
    bWeirdIndex = FALSE;
    for (i=0;i<256;i++)
    {
        if (ACSFileSeek(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        dwPage * A2_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        lpstPage,
                        A2_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A2_PAGE_SIZE)
            return(FALSE);

        if (bFirstPage == TRUE)
        {
            if (lpstPage->abyType[0] == 0x03)
            {
                // Check to see whether it is a weird index:
                //  e.g., the branch page is actually the
                //        page for the first index entry
                //        and the branch page actually
                //        follows the last index entry

                if (A2P03CheckForWeirdIndex(lpstA2,
                                            lpstPage,
                                            &bWeirdIndex) == FALSE)
                    return(FALSE);
            }

            bFirstPage = FALSE;
        }

        // Find object

        if (A2P04FindIndexEntry(lpstPage,
                                bWeirdIndex,
                                nKeyValueLen,
                                lpabyKeyValue,
                                lpdwPage,
                                lpnIndex,
                                &nResult) == FALSE)
            return(FALSE);

        if (nResult == 0)
        {
            *lpbFound = TRUE;
            return(TRUE);
        }

        if (nResult == -1)
        {
            *lpbFound = FALSE;
            return(TRUE);
        }

        if (nResult != 1)
            return(FALSE);

        // Search the next page

        dwPage = *lpdwPage;
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A2GetFirstIndexPagePrefix()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  dwPage              Root index page to start search
//  nKeyValueLen        Length of key value to search
//  lpabyKeyValue       Ptr to key value to search
//  lpdwPage            Ptr for type 0x04 page containing 1st item
//
// Description:
//  Returns the page number of the type 0x04 page containing the
//  smallest key value item with the given prefix.
//
//  If there is no such page, then the function returns TRUE
//  and sets *lpdwPage to 0.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2GetFirstIndexPagePrefix
(
    LPA2                lpstA2,
    DWORD               dwPage,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage
)
{
    LPA2_P04            lpstPage;
    DWORD               dwByteCount;
    int                 i;
    int                 nIndex;
    int                 nResult;
    WORD                w;
    BOOL                bFirstPage;
    BOOL                bWeirdIndex;

    lpstPage = (LPA2_P04)(lpstA2->abyTempPage);

    // Search up to 256 pages

    bFirstPage = TRUE;
    bWeirdIndex = FALSE;
    for (i=0;i<256;i++)
    {
        if (ACSFileSeek(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        dwPage * A2_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        lpstPage,
                        A2_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A2_PAGE_SIZE)
            return(FALSE);

        // If it is a type 0x04 page, then return it

        if (lpstPage->abyType[0] == 0x04)
        {
            // Get the offset of the first item

            w = WENDIAN(lpstPage->awItemOffsets[0]);
            if (w > A2_PAGE_SIZE - 1 - nKeyValueLen)
                return(FALSE);

            // Get the length of the first item

            nIndex = lpstA2->abyTempPage[++w];

            // Check the prefix of the first item
            //  If it is less than the first item and the
            //  prefix doesn't match, then return FALSE

            for (i=0;i<nKeyValueLen && i < nIndex;i++)
            {
                nResult = lpabyKeyValue[i] - lpstA2->abyTempPage[w++];
                if (nResult != 0)
                {
                    if (nResult < 0)
                    {
                        *lpdwPage = 0;
                        return(TRUE);
                    }

                    break;
                }
            }

            *lpdwPage = dwPage;
            return(TRUE);
        }

        if (bFirstPage == TRUE)
        {
            if (lpstPage->abyType[0] == 0x03)
            {
                // Check to see whether it is a weird index:
                //  e.g., the branch page is actually the
                //        page for the first index entry
                //        and the branch page actually
                //        follows the last index entry

                if (A2P03CheckForWeirdIndex(lpstA2,
                                            lpstPage,
                                            &bWeirdIndex) == FALSE)
                    return(FALSE);
            }

            bFirstPage = FALSE;
        }

        // Find object

        if (A2P04FindIndexEntry(lpstPage,
                                bWeirdIndex,
                                nKeyValueLen,
                                lpabyKeyValue,
                                lpdwPage,
                                &nIndex,
                                &nResult) == FALSE)
            return(FALSE);

        if (nResult != 1)
            return(FALSE);

        // Search the next page

        dwPage = *lpdwPage;
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A2P04GetItemList()
//
// Parameters:
//  lpstPage            Ptr to type 0x04 page
//  lpfnItemListMatch   Ptr to item list matching function
//  lpabyPrefix         Ptr to prefix
//  nPrefixLen          Length of prefix
//  lpvItemListCookie   Item list cookie
//  lpdwPage            Ptr to DWORD for object page
//  lpadwItems          Ptr to 256-bit array for matching items
//  lpdwNextPage        Ptr to DWORD for next page
//
// Description:
//  The function retrieves a set of item indexes for items matching
//  the given criteria.  The set of item indexes will reside on
//  one page.  The function should be called multiple times to
//  retrieve items on other pages.  Each subsequent call with the
//  same criteria will return a page that is guaranteed to be
//  greater than the previous page.  *lpdwPage should be the
//  previous page number (zero to start).
//
//  If this is the last set, then *lpdwNextPage is set to 0.
//  Otherwise, it is non-zero
//
// Returns:
//  TRUE                On success
//  FALSE               Error
//
//********************************************************************

BOOL A2P04GetItemList
(
    LPA2_P04            lpstPage,
    LPFNA2ItemMatch     lpfnItemMatch,
    LPBYTE              lpabyPrefixValue,
    int                 nPrefixLen,
    LPVOID              lpvItemListCookie,
    LPDWORD             lpdwPage,
    LPDWORD             lpadwItems,
    LPDWORD             lpdwNextPage
)
{
    WORD                w;
    int                 nItemCount;
    int                 i;
    int                 j;
    DWORD               dwMinPage;
    DWORD               dwMatchPage;
    DWORD               dwNextMatchPage;
    DWORD               dwCandPage;
    LPBYTE              lpabyData;
    int                 nLen;

    // Validate page type

    if (lpstPage->abyType[0] != 0x04)
        return(FALSE);

    // Validate item count

    w = WENDIAN(lpstPage->wItemCount);
    if (w > A2_P06_MAX_ITEMS)
        return(FALSE);

    nItemCount = (int)w;

    // Check all the items

    lpabyData = (LPBYTE)lpstPage;

    // Find the first match

    *lpdwNextPage = 0;
    dwMinPage = *lpdwPage;
    dwMatchPage = 0;
    dwNextMatchPage = 0;
    for (i=0;i<nItemCount;i++)
    {
        w = WENDIAN(lpstPage->awItemOffsets[i]);
        if (w >= A2_PAGE_SIZE || w + lpabyData[w] + 1 + 4 > A2_PAGE_SIZE)
            continue;

        nLen = lpabyData[w++];

        // Get page of candidate

        dwCandPage = ((DWORD)lpabyData[w + nLen + 1]) |
                     ((DWORD)lpabyData[w + nLen + 2] << 8) |
                     ((DWORD)lpabyData[w + nLen + 3] << 16);

        // We are always going forward

        if (dwCandPage > dwMinPage)
        {
            // Looking for the first one

            if (lpfnItemMatch(lpvItemListCookie,
                              lpabyPrefixValue,
                              nPrefixLen,
                              lpabyData + w,
                              nLen) != FALSE)
            {
                // It was a match

                if (dwMatchPage == 0 || dwCandPage < dwMatchPage)
                {
                    // Zero out set bit array

                    for (j=0;j<8;j++)
                        lpadwItems[j] = 0;

                    if (dwCandPage < dwMatchPage)
                        dwNextMatchPage = dwMatchPage;

                    // Match this page

                    dwMatchPage = dwCandPage;
                }

                // It was a match, so set the index

                if (dwCandPage == dwMatchPage)
                {
                    lpadwItems[(lpabyData[w+nLen] >> 5) & 7] |=
                        (DWORD)1 << (lpabyData[w+nLen] & 0x1F);
                }
                else
                if (dwNextMatchPage == 0 || dwCandPage < dwNextMatchPage)
                    dwNextMatchPage = dwCandPage;
            }
        }
    }

    *lpdwPage = dwMatchPage;
    *lpdwNextPage = dwNextMatchPage;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2EnumItems()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  dwPage              Type 0x04 index page with first item
//  lpfnP04ItemMatch    Ptr to item matching function
//  lpabyPrefix         Ptr to prefix
//  nPrefixLen          Length of prefix
//  lpfnObjectParse     Ptr to object parsing function
//  lpfnEnumCB          Ptr to enumeration callback
//  lpvEnumCBCookie     Enumeration callback cookie
//
// Description:
//  The function enumerates all the objects starting with the
//  object on the given index page, calling the given callback
//  function for each object.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2EnumItems
(
    LPA2                lpstA2,
    DWORD               dwPage,
    LPFNA2ItemMatch     lpfnItemMatch,
    LPBYTE              lpabyPrefixValue,
    int                 nPrefixLen,
    LPFNA2ObjectParse   lpfnObjectParse,
    LPFNA2ObjectEnumCB  lpfnEnumCB,
    LPVOID              lpvEnumCBCookie
)
{
    LPA2_P04            lpstPage;
    LPA2_P06            lpstItemPage;
    DWORD               dwByteCount;
    DWORD               dwNextSetPage;
    int                 i;
    int                 j;
    int                 nItemCount;
    WORD                wItemOffset;
    WORD                wEndOffset;
    DWORD               dwSetPage;
    DWORD               adwSetItems[8];
    LPBYTE              lpabypsszName;
    DWORD               dwObjectPage;
    int                 nIndex;
    BOOL                bReread;

    // Iterate through all items (at most 1024 sets)

    dwSetPage = 0;
    dwNextSetPage = 0;
    lpstPage = (LPA2_P04)(lpstA2->abyTempPage);
    lpstItemPage = (LPA2_P06)(lpstA2->abyPage);
    for (i=0;i<1024 && dwPage!=0;i++)
    {
        // Read the page

        if (ACSFileSeek(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        dwPage * A2_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        lpstPage,
                        A2_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A2_PAGE_SIZE)
            return(FALSE);

        // Get the matching item list

        if (A2P04GetItemList(lpstPage,
                             lpfnItemMatch,
                             lpabyPrefixValue,
                             nPrefixLen,
                             NULL,
                             &dwSetPage,
                             adwSetItems,
                             &dwNextSetPage) == FALSE)
            break;

        if (dwNextSetPage == 0)
        {
            if (dwSetPage == 0)
            {
                // No more

                break;
            }

            // Get the next index page number

            dwPage = DWENDIAN(lpstPage->dwNextPage);
        }

        nItemCount = 256;
        bReread = TRUE;
        for (j=0;j<nItemCount;j++)
        {
            if (bReread != FALSE)
            {
                // Read the item set page

                if (ACSFileSeek(lpstA2->lpvRootCookie,
                                lpstA2->lpvFile,
                                dwSetPage * A2_PAGE_SIZE,
                                ACS_SEEK_SET,
                                NULL) == FALSE)
                    break;

                if (ACSFileRead(lpstA2->lpvRootCookie,
                                lpstA2->lpvFile,
                                lpstItemPage,
                                A2_PAGE_SIZE,
                                &dwByteCount) == FALSE ||
                    dwByteCount != A2_PAGE_SIZE)
                    break;

                // Iterate through the items

                nItemCount = WENDIAN(lpstItemPage->wItemCount);

                // Limit item count

                if (nItemCount > 256)
                    nItemCount = 256;

                bReread = FALSE;
            }

            if ((adwSetItems[(j >> 5) & 7] & ((DWORD)1 << (j & 0x1F))) == 0)
                continue;

            wItemOffset = WENDIAN(lpstItemPage->awItemOffsets[j]);

            if ((wItemOffset & 0xF000) == 0x2000)
            {
                bReread = TRUE;

                wItemOffset &= 0x0FFF;
                if (wItemOffset > A97_PAGE_SIZE - 4)
                    continue;

                nIndex = lpstA2->abyPage[wItemOffset];
                dwObjectPage = ((DWORD)lpstA2->abyPage[wItemOffset+1]) |
                               ((DWORD)lpstA2->abyPage[wItemOffset+2] << 8) |
                               ((DWORD)lpstA2->abyPage[wItemOffset+3] << 16);

                // Read the item set page

                if (ACSFileSeek(lpstA2->lpvRootCookie,
                                lpstA2->lpvFile,
                                dwObjectPage * A2_PAGE_SIZE,
                                ACS_SEEK_SET,
                                NULL) == FALSE)
                    continue;

                if (ACSFileRead(lpstA2->lpvRootCookie,
                                lpstA2->lpvFile,
                                lpstItemPage,
                                A2_PAGE_SIZE,
                                &dwByteCount) == FALSE ||
                    dwByteCount != A2_PAGE_SIZE)
                    continue;

                if (nIndex >= WENDIAN(lpstItemPage->wItemCount))
                    continue;

                wItemOffset =
                    WENDIAN(lpstItemPage->awItemOffsets[nIndex]);
            }
            else
                nIndex = j;

            // Ignore deleted items

            if ((wItemOffset & 0xF000) == 0)
                continue;

            wItemOffset &= 0xFFF;
            if (wItemOffset > A2_PAGE_SIZE)
                wItemOffset = A2_PAGE_SIZE;

            // Get the end offset

            if (nIndex == 0)
                wEndOffset = A2_PAGE_SIZE;
            else
            {
                wEndOffset = 0xFFF &
                    WENDIAN(lpstItemPage->awItemOffsets[nIndex-1]);

                if (wEndOffset > A2_PAGE_SIZE)
                    wEndOffset = A2_PAGE_SIZE;
            }

            if (wEndOffset < wItemOffset)
                continue;

            // Get the item data

            if (lpfnObjectParse(lpstA2->abyPage + wItemOffset,
                                (int)(wEndOffset - wItemOffset),
                                &lpabypsszName,
                                &dwByteCount,
                                &dwObjectPage,
                                &nIndex) == FALSE)
                continue;

#ifdef SYM_NLM
            ACSProgress(lpstA2->lpvRootCookie);
#endif

            // Call the callback function

            if (lpfnEnumCB(lpstA2,
                           lpvEnumCBCookie,
                           lpabypsszName,
                           dwByteCount,
                           dwObjectPage,
                           nIndex) == FALSE)
                return(FALSE);
        }

        // If it is a new index page, we need to restart

        if (dwNextSetPage == 0)
        {
            // Restart for page

            dwSetPage = 0;
            dwNextSetPage = 0;
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2P04GenericItemMatch()
//
// Parameters:
//  lpvItemListCookie   Item list cookie
//  lpabyPrefix         Ptr to prefix
//  nPrefixLen          Length of prefix
//  lpabyItem           Item key value
//  nItemLen            Item key value length
//
// Description:
//  The function checks to see whether the given item is a
//  module item.  It is if it begins with the byte sequence
//  0x7F 0x90 0x00 0x00 0x04 0x7F.
//
// Returns:
//  TRUE                If the item is a match
//  FALSE               If the item is not a match
//
//********************************************************************

BOOL A2P04GenericItemMatch
(
    LPVOID              lpvItemListCookie,
    LPBYTE              lpabyPrefixValue,
    int                 nPrefixLen,
    LPBYTE              lpabyItem,
    int                 nItemLen
)
{
    int         i;

    (void)lpvItemListCookie;

    for (i=0;i<nItemLen && i<nPrefixLen;i++)
    {
        if (lpabyItem[i] != lpabyPrefixValue[i])
            return(FALSE);
    }

    if (i == nPrefixLen)
        return(TRUE);

    // Assume it is not a module

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A2ModuleObjectParse()
//
// Parameters:
//  lpabyObject         Ptr to object info byte array
//  nObjectSize         Size of object info in bytes
//  lplpabypsszName     Ptr to ptr for Pascal/C object name
//  lpdwSize            Ptr to DWORD for object size
//  lpdwPage            Ptr to DWORD for object start page
//  lpnIndex            Ptr to int for object start index
//
// Description:
//  The function assumes that the object info is for a module and
//  is in the MSysObjects table.
//
//  The form of the object info is assumed to be:
//
//      wObjectSize             // 0x00
//      abyUnknown0[10]         // 0x02
//      wType                   // 0x0C
//
//  The last bytes of the object are assumed to be in this form:
//
//      byOffsetsSize           // wObjectSize - 3
//      byNameOffset            // wObjectSize - 4
//      byUnknown0              // wObjectSize - 5
//      byUnknown1              // wObjectSize - 6
//      byUnknown2              // wObjectSize - 7
//      byUnknown3              // wObjectSize - 8
//      byUnknown4              // wObjectSize - 9
//      byUnknown5              // wObjectSize - 10
//      byUnknown6              // wObjectSize - 11
//      byUnknown7              // wObjectSize - 12
//      bySizeLocationOffset    // wObjectSize - 13
//      byInfoSize              // wObjectSize - 14
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2ModuleObjectParse
(
    LPBYTE              lpabyObject,
    int                 nObjectSize,
    LPBYTE FAR *        lplpabypsszName,
    LPDWORD             lpdwSize,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex
)
{
    int                 nNameBegOffset;
    int                 nNameEndOffset;
    int                 nSizeLocationOffset;

    // Check object size: Offsets + ObjectSize + SizeLocation + Type

    if (nObjectSize < 14 + 2 + 8 + 2)
        return(FALSE);

    // Make sure the type is 0x07 0x80

    if (lpabyObject[0x0C] != 0x07 || lpabyObject[0x0D] != 0x80)
        return(FALSE);

    nNameBegOffset = lpabyObject[nObjectSize - 4];
    nNameEndOffset = lpabyObject[nObjectSize - 5];
    nSizeLocationOffset = lpabyObject[nObjectSize - 13];

    // Validate offsets

    if (nNameBegOffset < 2 ||
        nNameBegOffset >= nObjectSize ||
        nNameEndOffset >= nObjectSize ||
        nNameBegOffset > nNameEndOffset ||
        nSizeLocationOffset + 8 > nObjectSize)
        return(FALSE);

    // Get the size and location

    *lpdwSize = ((DWORD)lpabyObject[nSizeLocationOffset]) |
                ((DWORD)lpabyObject[nSizeLocationOffset + 1] << 8) |
                ((DWORD)lpabyObject[nSizeLocationOffset + 2] << 16);

    // Get the page and index containing the object

    *lpnIndex = lpabyObject[nSizeLocationOffset + 4];

    *lpdwPage = ((DWORD)lpabyObject[nSizeLocationOffset + 5]) |
                ((DWORD)lpabyObject[nSizeLocationOffset + 6] << 8) |
                ((DWORD)lpabyObject[nSizeLocationOffset + 7] << 16);

    // Set the Pascal length of the name

    lpabyObject[nNameBegOffset - 1] = nNameEndOffset - nNameBegOffset;

    // Zero terminate the name

    lpabyObject[nNameEndOffset] = 0;

    // Set the name pointer

    *lplpabypsszName = lpabyObject + nNameBegOffset - 1;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2GetVBModulePrefixID()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  dwPage              Root objects index page
//  lpabyPrefix         Ptr to filled six-byte prefix buffer
//
// Description:
//  Sets the fourth byte of the prefix buffer to the fourth
//  byte of the Modules object entry.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2GetVBModulePrefixID
(
    LPA2                lpstA2,
    DWORD               dwPage,
    LPBYTE              lpabyPrefix
)
{
    LPA2_P06            lpstPage;
    int                 nIndex;
    BOOL                bFound;
    DWORD               dwByteCount;
    WORD                wOffset;

    lpstPage = (LPA2_P06)lpstA2->abyTempPage;

    // Get the Modules ID

    if (A2P04FindIndexedKeyValue(lpstA2,
                                 dwPage,
                                 sizeof(gabyA2ModulesObjectsKey),
                                 gabyA2ModulesObjectsKey,
                                 &dwPage,
                                 &nIndex,
                                 &bFound) == FALSE ||
        bFound == FALSE)
        return(FALSE);

    // Read the page containing the Modules object

    if (ACSFileSeek(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    dwPage * A2_PAGE_SIZE,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileRead(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    lpstPage,
                    A2_PAGE_SIZE,
                    &dwByteCount) == FALSE ||
        dwByteCount != A2_PAGE_SIZE)
        return(FALSE);

    // Validate index

    if (nIndex >= WENDIAN(lpstPage->wItemCount) ||
        nIndex >= A2_P06_MAX_ITEMS)
        return(FALSE);

    wOffset = WENDIAN(lpstPage->awItemOffsets[nIndex]);

    // Assume that the object will never be a reference to another object

    if ((wOffset & 0xF000) != 0x1000)
        return(FALSE);

    // Validate offset

    wOffset &= 0xFFF;
    if (wOffset >= A2_PAGE_SIZE - 4)
        return(FALSE);

    lpabyPrefix[4] = lpstA2->abyTempPage[wOffset + 4];

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2EnumVBModules()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  lpfnEnumCB          Ptr to enumeration callback
//  lpvEnumCBCookie     Enumeration callback cookie
//
// Description:
//  The function enumerates all the VB modules in the given
//  Access 2 object, calling the given callback function
//  for each such object.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2EnumVBModules
(
    LPA2                lpstA2,
    LPFNA2ObjectEnumCB  lpfnEnumCB,
    LPVOID              lpvEnumCBCookie
)
{
    DWORD               dwFirstIndexPage;
    DWORD               dwPage;
    BYTE                abyPrefix[] = { 0x7F, 0x90, 0x00, 0x00, 0x04, 0x7F };

    // Get the first Objects index page

    if (A2P05GetFirstIndexPage(lpstA2,
                               A2_OBJECTS_MASTER_PAGE,
                               &dwFirstIndexPage) == FALSE)
        return(FALSE);

    // Get the VB module prefix

    if (A2GetVBModulePrefixID(lpstA2,
                              dwFirstIndexPage,
                              abyPrefix) == FALSE)
        return(FALSE);

    // Try the first prefix: Get the first module index page

    if (A2GetFirstIndexPagePrefix(lpstA2,
                                  dwFirstIndexPage,
                                  sizeof(abyPrefix),
                                  abyPrefix,
                                  &dwPage) == FALSE)
        return(FALSE);

    // Were there any such pages?

    if (dwPage != 0)
    {
        // Enumerate through the modules

        if (A2EnumItems(lpstA2,
                        dwPage,
                        A2P04GenericItemMatch,
                        abyPrefix,
                        sizeof(abyPrefix),
                        A2ModuleObjectParse,
                        lpfnEnumCB,
                        lpvEnumCBCookie) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2P05GetFirstDataPage()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  dwMasterPage        Type 0x05 master page containing index
//  lpdwPage            Ptr to DWORD for first data page number
//
// Description:
//  The function returns the page number of the first data page
//  for the given master page.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2P05GetFirstDataPage
(
    LPA2                lpstA2,
    DWORD               dwMasterPage,
    LPDWORD             lpdwPage
)
{
    DWORD               dwByteCount;

    // Read the DWORD at offset 0x0C of the page

    if (ACSFileSeek(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    dwMasterPage * A2_PAGE_SIZE + 0x0C,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileRead(lpstA2->lpvRootCookie,
                    lpstA2->lpvFile,
                    lpdwPage,
                    sizeof(DWORD),
                    &dwByteCount) == FALSE ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    *lpdwPage = DWENDIAN(*lpdwPage);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2GetFirstMacrosDataPage()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  lpdwPage            Ptr to DWORD for first macros data page number
//
// Description:
//  The function returns the page number of the first macros
//  data page.
//
//  If the MSysObjects entry cannot be found, the function sets
//  *lpdwPage = 0 and returns TRUE.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2GetFirstMacrosDataPage
(
    LPA2                lpstA2,
    LPDWORD             lpdwPage
)
{
    DWORD               dwByteCount;
    DWORD               dwPage;
    int                 nIndex;
    BOOL                bFound;
    LPA2_P06            lpstPage;
    WORD                wOffset;
    int                 i;

    // Get the first Objects index page

    if (A2P05GetFirstIndexPage(lpstA2,
                               A2_OBJECTS_MASTER_PAGE,
                               &dwPage) == FALSE)
        return(FALSE);

    // Get the page and index of the MSysObjects entry

    if (A2P04FindIndexedKeyValue(lpstA2,
                                 dwPage,
                                 sizeof(gabyA2MSysMacrosObjectsKey),
                                 gabyA2MSysMacrosObjectsKey,
                                 &dwPage,
                                 &nIndex,
                                 &bFound) == FALSE)
        return(FALSE);

    if (bFound == FALSE)
    {
        *lpdwPage = 0;
        return(TRUE);
    }

    lpstPage = (LPA2_P06)(lpstA2->abyTempPage);

    for (i=0;i<256;i++)
    {
        // Read the page containing the item

        if (ACSFileSeek(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        dwPage * A2_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstA2->lpvRootCookie,
                        lpstA2->lpvFile,
                        lpstPage,
                        A2_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A2_PAGE_SIZE)
            return(FALSE);

        // Validate the index

        lpstPage->wItemCount = WENDIAN(lpstPage->wItemCount);
        if (nIndex > lpstPage->wItemCount ||
            lpstPage->wItemCount > A2_P06_MAX_ITEMS)
            return(FALSE);

        // Validate the item offset

        wOffset = WENDIAN(lpstPage->awItemOffsets[nIndex]);

        // Has the item been deleted?

        // Has the item been relocated or deleted?

        if ((wOffset & 0xF000) == 0x2000)
        {
            wOffset &= 0x0FFF;
            if (wOffset > A97_PAGE_SIZE - 4)
                return(FALSE);

            nIndex = lpstA2->abyTempPage[wOffset];
            dwPage = ((DWORD)lpstA2->abyTempPage[wOffset+1]) |
                     ((DWORD)lpstA2->abyTempPage[wOffset+2] << 8) |
                     ((DWORD)lpstA2->abyTempPage[wOffset+3] << 16);

            continue;
        }
        else
        if ((wOffset & 0xF000) == 0)
        {
            *lpdwPage = 0;
            return(TRUE);
        }

        // Is it out of bounds?

        wOffset &= 0xFFF;
        if (wOffset + 7 > A2_PAGE_SIZE)
            return(FALSE);

        // Bytes [4..6] give the page containing the master index

        dwPage = ((DWORD)lpstA2->abyTempPage[wOffset + 4]) |
                 ((DWORD)lpstA2->abyTempPage[wOffset + 5] << 8) |
                 ((DWORD)lpstA2->abyTempPage[wOffset + 6] << 16);

        // Now get the first macros data page

        if (A2P05GetFirstDataPage(lpstA2,
                                  dwPage,
                                  lpdwPage) == FALSE)
            return(FALSE);

        return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A2ObjAccessInit()
//
// Parameters:
//  lpstA2              Ptr to A2_T structure
//  dwSize              Size of object
//  dwPage              First page of object
//  nIndex              Index of object on first page
//  lpstObj             Ptr to A2_OBJ_ACCESS_T structure to init
//
// Description:
//  Initializes an A2_OBJ_ACCESS_T structure for random access
//  to the given object.
//
// Returns:
//  TRUE                Always
//
//********************************************************************

BOOL A2ObjAccessInit
(
    LPA2                lpstA2,
    DWORD               dwSize,
    DWORD               dwPage,
    int                 nIndex,
    LPA2_OBJ_ACCESS     lpstObj
)
{
    lpstObj->lpstA2 = lpstA2;
    lpstObj->dwSize = dwSize;
    lpstObj->nNumCachedPages = 0;
    lpstObj->nNextIndex = nIndex;
    lpstObj->dwNextPage = dwPage;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2ObjAccessRead()
//
// Parameters:
//  lpstObj             Ptr to A2_OBJ_ACCESS_T structure
//  dwOffset            Offset in object to read from
//  lpvBuffer           Destination buffer
//  dwNumBytes          Number of bytes to read
//  lpdwBytesRead       Ptr to DWORD for # of byte read
//
// Description:
//  The function reads the given number of bytes from the given
//  offset of the object into the given buffer.
//
//  If the end of the object is reached before the given number
//  of bytes is read, *lpdwBytesRead will indicate the actual
//  number of bytes read and the function will return TRUE.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2ObjAccessRead
(
    LPA2_OBJ_ACCESS     lpstObj,
    DWORD               dwOffset,
    LPVOID              lpvBuffer,
    DWORD               dwNumBytes,
    LPDWORD             lpdwBytesRead
)
{
    LPA2_P06            lpstPage;
    DWORD               dwByteCount;
    DWORD               dwSize;
    int                 nPageI;
    DWORD               dwNextPage;
    int                 i;
    WORD                wSize;
    DWORD               dwCurOffset;
    LPBYTE              lpabyBuf;
    LPBYTE              lpabySrc;
    int                 nIndex;
    DWORD               dwPage;
    WORD                wEnd;
    WORD                wStart;

    *lpdwBytesRead = dwNumBytes;

    // Search through the cached pages first

    lpabyBuf = (LPBYTE)lpvBuffer;
    dwCurOffset = 0;
    for (i=0;i<lpstObj->nNumCachedPages;i++)
    {
        if (dwCurOffset <= dwOffset &&
            dwOffset < dwCurOffset + lpstObj->awPageSize[i])
        {
            if (dwOffset + dwNumBytes >
                dwCurOffset + lpstObj->awPageSize[i])
                dwSize = dwCurOffset + lpstObj->awPageSize[i] - dwOffset;
            else
                dwSize = dwNumBytes;

            // Read into the buffer

            if (ACSFileSeek(lpstObj->lpstA2->lpvRootCookie,
                            lpstObj->lpstA2->lpvFile,
                            lpstObj->adwPageOffset[i] +
                                (dwOffset - dwCurOffset),
                            ACS_SEEK_SET,
                            NULL) == FALSE)
                return(FALSE);

            if (ACSFileRead(lpstObj->lpstA2->lpvRootCookie,
                            lpstObj->lpstA2->lpvFile,
                            lpabyBuf,
                            dwSize,
                            &dwByteCount) == FALSE ||
                dwByteCount != dwSize)
                return(FALSE);

            // Update number of bytes left to read, offset of next
            //  chunk, and destination buffer ptr

            dwNumBytes -= dwSize;
            dwOffset += dwSize;
            lpabyBuf += dwSize;

            if (dwNumBytes == 0)
                return(TRUE);
        }

        dwCurOffset += lpstObj->awPageSize[i];
    }

    nPageI = i;

    // Start reading from the next page and index

    lpstPage = (LPA2_P06)(lpstObj->lpstA2->abyTempPage);
    nIndex = lpstObj->nNextIndex;
    dwPage = lpstObj->dwNextPage;
    while (nPageI < 1024 && dwPage != 0)
    {
        // Read the page

        if (ACSFileSeek(lpstObj->lpstA2->lpvRootCookie,
                        lpstObj->lpstA2->lpvFile,
                        dwPage * A2_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstObj->lpstA2->lpvRootCookie,
                        lpstObj->lpstA2->lpvFile,
                        lpstPage,
                        A2_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A2_PAGE_SIZE)
            return(FALSE);

        // Validate page type

        if (lpstPage->abyType[0] != 0x06 || lpstPage->abyType[1] != 0x00)
            return(FALSE);

        if (nIndex >= A2_P06_MAX_ITEMS ||
            nIndex >= WENDIAN(lpstPage->wItemCount))
            return(FALSE);

        // Get the start offset

        wStart = WENDIAN(lpstPage->awItemOffsets[nIndex]) & 0xFFF;
        if (wStart > A2_PAGE_SIZE)
            wStart = A2_PAGE_SIZE;

        // Get the end offset

        if (nIndex == 0)
            wEnd = A2_PAGE_SIZE;
        else
        {
            wEnd = WENDIAN(lpstPage->awItemOffsets[nIndex - 1]) & 0xFFF;
            if (wEnd > A2_PAGE_SIZE)
                wEnd = A2_PAGE_SIZE;
        }

        // Get the size of the object's data on this page

        if (wStart + 10 >= wEnd)
        {
            // No data

            wSize = 0;
            dwNextPage = 0;
        }
        else
        {
            wSize = (int)(wEnd - wStart - 10);
            nIndex = lpstObj->lpstA2->abyTempPage[wStart + 2];
            dwNextPage =
                ((DWORD)lpstObj->lpstA2->abyTempPage[wStart + 3]) |
                ((DWORD)lpstObj->lpstA2->abyTempPage[wStart + 4] << 8) |
                ((DWORD)lpstObj->lpstA2->abyTempPage[wStart + 5] << 16);
        }

        // Are there any bytes in this buffer to copy?

        if (dwCurOffset <= dwOffset && dwOffset < dwCurOffset + wSize)
        {
            if (dwOffset + dwNumBytes > dwCurOffset + wSize)
                dwByteCount = dwCurOffset + wSize - dwOffset;
            else
                dwByteCount = dwNumBytes;

            lpabySrc = lpstObj->lpstA2->abyTempPage + wStart + 10 +
                (dwOffset - dwCurOffset);

            // Update number of bytes left and offset

            dwNumBytes -= dwByteCount;
            dwOffset += dwByteCount;

            while (dwByteCount-- != 0)
                *lpabyBuf++ = *lpabySrc++;

            if (dwNumBytes == 0)
                return(TRUE);
        }

        // Is it cacheable?

        if (nPageI < A2_OBJ_MAX_CACHED_PAGES)
        {
            lpstObj->nNumCachedPages++;
            lpstObj->awPageSize[nPageI] = wSize;
            lpstObj->adwPageOffset[nPageI] =
                dwPage * A2_PAGE_SIZE + wStart + 10;
            lpstObj->nNextIndex = nIndex;
            lpstObj->dwNextPage = dwNextPage;
        }

        // Update current offset and next page

        dwCurOffset += wSize;
        dwPage = dwNextPage;

        // Update page index

        ++nPageI;
    }

    if (dwNumBytes != 0)
        *lpdwBytesRead -= dwNumBytes;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2GetMacroLineInfo()
//
// Parameters:
//  lpabyMacroLine      Ptr to macro line bytes
//  nSize               Number of bytes on the line
//  lpstInfo            Ptr to structure for item info
//
// Description:
//  Validates the info item offsets and places them along with the
//  item sizes into a standard form.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A2GetMacroLineInfo
(
    LPBYTE              lpabyMacroLine,
    int                 nSize,
    LPA2MACRO_LINE_INFO lpstInfo
)
{
    int         nOffsetCount;
    int         nJumpCount;
    int         nJumpValue;
    int         anOffsets[A2MACRO_MAX_INFO_ITEMS + 1];
    LPBYTE      lpabyData;
    LPBYTE      lpabyJumps;
    int         i;

    ////////////////////////////////////////////////////////
    // Validate counts

    if (nSize < 4)
        return(FALSE);

    // Validate the jump count and type

    nJumpCount = lpabyMacroLine[1];
    nOffsetCount = lpabyMacroLine[3];

    if (nOffsetCount > 0xE)
        return(FALSE);

    if (nJumpCount > 0x10 ||
        nJumpCount * 0x100 + lpabyMacroLine[0] > nSize)
        return(FALSE);

    // 10 header bytes, 1 info size, 1 offset count, 1 unknown,
    //  nOffsetCount, nJumpCount

    if (10 + 1 + 1 + 1 + nOffsetCount + nJumpCount > nSize)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Write adjusted offsets to local array

    // Set the pointer to the offset and jump data,
    //  starting at the end

    lpabyData = lpabyMacroLine + nSize - 1 - nJumpCount - 1 - 1;
    lpabyJumps = lpabyMacroLine + nSize - 1 - 1;

    nJumpValue = 0;
    for (i=0;i<=nOffsetCount;i++)
    {
        if (nJumpCount != 0)
        {
            if (*lpabyJumps == i)
            {
                nJumpValue += 0x100;
                --nJumpCount;
                --lpabyJumps;
            }
        }

        anOffsets[i] = nJumpValue + *lpabyData--;
    }

    // Set the rest of the offsets

    for (;i<=A2MACRO_MAX_INFO_ITEMS;i++)
        anOffsets[i] = anOffsets[nOffsetCount];

    ////////////////////////////////////////////////////////
    // Place offsets and sizes in array in canonical form

    if (nOffsetCount == 0xE)
        lpabyData = gabyA2ELineOffsetIndexes;
    else
        lpabyData = gabyA2LineOffsetIndexes;

    for (i=0;i<nOffsetCount;i++)
    {
        if (anOffsets[i] < anOffsets[i+1])
        {
            // First validate end offset

            if (anOffsets[i+1] > nSize)
                return(FALSE);

            lpstInfo->anOffsets[lpabyData[i]] = anOffsets[i];
            lpstInfo->anSizes[lpabyData[i]] =
                anOffsets[i+1] - anOffsets[i];
        }
        else
        {
            // Assume zero

            lpstInfo->anSizes[lpabyData[i]] = 0;
        }
    }

    // Set the rest of the items to a size of zero

    for (;i<A2MACRO_MAX_INFO_ITEMS;i++)
        lpstInfo->anSizes[lpabyData[i]] = 0;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A2CRCMacroLine()
//
// Parameters:
//  dwCRC               Starting CRC
//  lpabyMacroLine      Ptr to macro line bytes
//  nSize               Number of bytes on the line
//  lpstInfo            Ptr to structure with item info
//
// Description:
//  CRCs the action byte at offset four and then CRCs the items
//  on the line in the following order:
//
//      1.  Macro Name
//      2.  Condition
//      3.  Param 1
//      4.  Param 2
//      5.  Param 3
//      6.  Param 4
//      7.  Param 5
//      8.  Param 6
//      9.  Param 7
//      10. Param 8
//      11. Param 9
//      12. Param 10
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

DWORD A2CRCMacroLine
(
    DWORD               dwCRC,
    LPBYTE              lpabyMacroLine,
    LPA2MACRO_LINE_INFO lpstInfo,
    LPDWORD             lpdwCRCSize
)
{
    int                 i;
    int                 nSize;

    if ((nSize = lpstInfo->anSizes[A2MACRO_MACRONAME]) != 0)
    {
        dwCRC = CRC32Compute(nSize,
                             lpabyMacroLine +
                                 lpstInfo->anOffsets[A2MACRO_MACRONAME],
                             dwCRC);

        *lpdwCRCSize += nSize;
    }

    if ((nSize = lpstInfo->anSizes[A2MACRO_CONDITION]) != 0)
    {
        dwCRC = CRC32Compute(nSize,
                             lpabyMacroLine +
                                 lpstInfo->anOffsets[A2MACRO_CONDITION],
                             dwCRC);

        *lpdwCRCSize += nSize;
    }

    for (i=A2MACRO_PARAM1;i<=A2MACRO_PARAM10;i++)
    {
        if ((nSize = lpstInfo->anSizes[i]) != 0)
        {
            dwCRC = CRC32Compute(nSize,
                                 lpabyMacroLine +
                                     lpstInfo->anOffsets[i],
                                 dwCRC);

            *lpdwCRCSize += nSize;
        }
    }

    return(dwCRC);
}


