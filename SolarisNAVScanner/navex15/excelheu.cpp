// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/EXCELHEU.CPv   1.2   09 Dec 1998 17:45:26   DCHI  $
//
// Description:
//  Contains Excel 5.0/95/97 heuristic macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/EXCELHEU.CPv  $
// 
//    Rev 1.2   09 Dec 1998 17:45:26   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.1   09 Nov 1998 13:58:36   DCHI
// Added #pragma data_seg()'s for global FAR data for SYM_WIN16.
// 
//    Rev 1.0   04 Aug 1998 10:57:06   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "xl5api.h"
#include "o97api.h"
#include "excelheu.h"
#include "crc32.h"
#include "mcrxclud.h"

//********************************************************************
//
// Function:
//  LPXL5H XL5HAlloc()
//
// Parameters:
//  lpstRoot        Ptr to root structure
//  lpstDirStream   Ptr to stream structure for dir stream access use
//
// Description:
//  Allocates memory for the XL5H heuristic scanning environment.
//
// Returns:
//  LPXL5H          On success
//  NULL            On error
//
//********************************************************************

LPXL5H XL5HAlloc
(
    LPSS_ROOT       lpstRoot,
    LPSS_STREAM     lpstDirStream
)
{
    LPXL5H          lpstEnv;

    // Allocate the XL5H structure

    if (SSMemoryAlloc(lpstRoot->lpvRootCookie,
                      sizeof(XL5H_T),
                      (LPLPVOID)&lpstEnv) != SS_STATUS_OK)
        return(NULL);

    // Initialize root and module fields

    lpstEnv->lpstRoot = lpstRoot;
    lpstEnv->lpstDirStream = lpstDirStream;

    // Initialize fields to NULL

    lpstEnv->lpstSaveStream = NULL;
    lpstEnv->lpstCopyStream = NULL;
    lpstEnv->lpstSaveSibs = NULL;
    lpstEnv->lpstCopySibs = NULL;
    lpstEnv->lpabyBuf = NULL;

    // Allocate a stream structure for the save stream

    if (SSAllocStreamStruct(lpstRoot,
                            &lpstEnv->lpstSaveStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
    {
        XL5HFree(lpstEnv);
        return(NULL);
    }

    // Allocate a stream structure for the copy stream

    if (SSAllocStreamStruct(lpstRoot,
                            &lpstEnv->lpstCopyStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
    {
        XL5HFree(lpstEnv);
        return(NULL);
    }

    // Allocate a sibiling enumeration structure for the save scan

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstEnv->lpstSaveSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        XL5HFree(lpstEnv);
        return(NULL);
    }

    // Allocate a sibiling enumeration structure for the copy scan

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstEnv->lpstCopySibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        XL5HFree(lpstEnv);
        return(NULL);
    }

    // Allocate the working buffer

    lpstEnv->nBufSize = XL5H_DEF_BUF_SIZE;

    if (SSMemoryAlloc(lpstRoot->lpvRootCookie,
                      lpstEnv->nBufSize,
                      (LPLPVOID)&lpstEnv->lpabyBuf) != SS_STATUS_OK)
    {
        XL5HFree(lpstEnv);
        return(NULL);
    }

    // Set pointer app str id ptr of fast scan state

    lpstEnv->stFastScan.lpawAppStrID = lpstEnv->awAppStrID;

    return(lpstEnv);
}


//********************************************************************
//
// Function:
//  BOOL XL5HInit()
//
// Parameters:
//  LPXL5H          Ptr to XL5H structure to free
//
// Description:
//  Initializes XL5H heuristic scanning environment.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

void XL5HInit
(
    LPXL5H          lpstEnv
)
{
    int             i;

    // Initialize states

    for (i=0;i<XL5H_STATE_SIZE;i++)
        lpstEnv->abyState[i] = 0;

    lpstEnv->wStateNextModToCheck = 0;
    lpstEnv->wStateStartGroup = 0;
    lpstEnv->wStateFold = 0;
}


//********************************************************************
//
// Function:
//  BOOL XL5HFree()
//
// Parameters:
//  LPXL5H          Ptr to XL5H structure to free
//
// Description:
//  Frees XL5H heuristic scanning environment memory.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL XL5HFree
(
    LPXL5H          lpstEnv
)
{
    if (lpstEnv == NULL)
        return(TRUE);

    if (lpstEnv->lpabyBuf != NULL)
        SSMemoryFree(lpstEnv->lpstRoot->lpvRootCookie,lpstEnv->lpabyBuf);

    if (lpstEnv->lpstCopySibs != NULL)
        SSFreeEnumSibsStruct(lpstEnv->lpstRoot,lpstEnv->lpstCopySibs);

    if (lpstEnv->lpstSaveSibs != NULL)
        SSFreeEnumSibsStruct(lpstEnv->lpstRoot,lpstEnv->lpstSaveSibs);

    if (lpstEnv->lpstCopyStream != NULL)
        SSFreeStreamStruct(lpstEnv->lpstCopyStream);

    if (lpstEnv->lpstSaveStream != NULL)
        SSFreeStreamStruct(lpstEnv->lpstSaveStream);

    SSMemoryFree(lpstEnv->lpstRoot->lpvRootCookie,lpstEnv);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HGetAppStrID()
//
// Parameters:
//  lpstEnv         Ptr to XL5H_T structure
//
// Description:
//  The function assumes that the stDirInfo field has been
//  initialized.
//
//  The function retrieves the string IDs using a synchronized
//  lookup of both the index and the string data.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

#define XL5H_X                              0x73

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXL5H_VSTR_activesheet[] =
{
    'a' ^ XL5H_X, 'c' ^ XL5H_X, 't' ^ XL5H_X, 'i' ^ XL5H_X,
    'v' ^ XL5H_X, 'e' ^ XL5H_X, 's' ^ XL5H_X, 'h' ^ XL5H_X,
    'e' ^ XL5H_X, 'e' ^ XL5H_X, 't' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_activeworkbook[] =
{
    'a' ^ XL5H_X, 'c' ^ XL5H_X, 't' ^ XL5H_X, 'i' ^ XL5H_X,
    'v' ^ XL5H_X, 'e' ^ XL5H_X, 'w' ^ XL5H_X, 'o' ^ XL5H_X,
    'r' ^ XL5H_X, 'k' ^ XL5H_X, 'b' ^ XL5H_X, 'o' ^ XL5H_X,
    'o' ^ XL5H_X, 'k' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_add[] =
{
    'a' ^ XL5H_X, 'd' ^ XL5H_X, 'd' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_after[] =
{
    'a' ^ XL5H_X, 'f' ^ XL5H_X, 't' ^ XL5H_X, 'e' ^ XL5H_X,
    'r' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_altstartuppath[] =
{
    'a' ^ XL5H_X, 'l' ^ XL5H_X, 't' ^ XL5H_X, 's' ^ XL5H_X,
    't' ^ XL5H_X, 'a' ^ XL5H_X, 'r' ^ XL5H_X, 't' ^ XL5H_X,
    'u' ^ XL5H_X, 'p' ^ XL5H_X, 'p' ^ XL5H_X, 'a' ^ XL5H_X,
    't' ^ XL5H_X, 'h' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_application[] =
{
    'a' ^ XL5H_X, 'p' ^ XL5H_X, 'p' ^ XL5H_X, 'l' ^ XL5H_X,
    'i' ^ XL5H_X, 'c' ^ XL5H_X, 'a' ^ XL5H_X, 't' ^ XL5H_X,
    'i' ^ XL5H_X, 'o' ^ XL5H_X, 'n' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_before[] =
{
    'b' ^ XL5H_X, 'e' ^ XL5H_X, 'f' ^ XL5H_X, 'o' ^ XL5H_X,
    'r' ^ XL5H_X, 'e' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_copy[] =
{
    'c' ^ XL5H_X, 'o' ^ XL5H_X, 'p' ^ XL5H_X, 'y' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_filename[] =
{
    'f' ^ XL5H_X, 'i' ^ XL5H_X, 'l' ^ XL5H_X, 'e' ^ XL5H_X,
    'n' ^ XL5H_X, 'a' ^ XL5H_X, 'm' ^ XL5H_X, 'e' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_insertfile[] =
{
    'i' ^ XL5H_X, 'n' ^ XL5H_X, 's' ^ XL5H_X, 'e' ^ XL5H_X,
    'r' ^ XL5H_X, 't' ^ XL5H_X, 'f' ^ XL5H_X, 'i' ^ XL5H_X,
    'l' ^ XL5H_X, 'e' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_modules[] =
{
    'm' ^ XL5H_X, 'o' ^ XL5H_X, 'd' ^ XL5H_X, 'u' ^ XL5H_X,
    'l' ^ XL5H_X, 'e' ^ XL5H_X, 's' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_name[] =
{
    'n' ^ XL5H_X, 'a' ^ XL5H_X, 'm' ^ XL5H_X, 'e' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_pathseparator[] =
{
    'p' ^ XL5H_X, 'a' ^ XL5H_X, 't' ^ XL5H_X, 'h' ^ XL5H_X,
    's' ^ XL5H_X, 'e' ^ XL5H_X, 'p' ^ XL5H_X, 'a' ^ XL5H_X,
    'r' ^ XL5H_X, 'a' ^ XL5H_X, 't' ^ XL5H_X, 'o' ^ XL5H_X,
    'r' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_saveas[] =
{
    's' ^ XL5H_X, 'a' ^ XL5H_X, 'v' ^ XL5H_X, 'e' ^ XL5H_X,
    'a' ^ XL5H_X, 's' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_savecopyas[] =
{
    's' ^ XL5H_X, 'a' ^ XL5H_X, 'v' ^ XL5H_X, 'e' ^ XL5H_X,
    'c' ^ XL5H_X, 'o' ^ XL5H_X, 'p' ^ XL5H_X, 'y' ^ XL5H_X,
    'a' ^ XL5H_X, 's' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_sheets[] =
{
    's' ^ XL5H_X, 'h' ^ XL5H_X, 'e' ^ XL5H_X, 'e' ^ XL5H_X,
    't' ^ XL5H_X, 's' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_startuppath[] =
{
    's' ^ XL5H_X, 't' ^ XL5H_X, 'a' ^ XL5H_X, 'r' ^ XL5H_X,
    't' ^ XL5H_X, 'u' ^ XL5H_X, 'p' ^ XL5H_X, 'p' ^ XL5H_X,
    'a' ^ XL5H_X, 't' ^ XL5H_X, 'h' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_thisworkbook[] =
{
    't' ^ XL5H_X, 'h' ^ XL5H_X, 'i' ^ XL5H_X, 's' ^ XL5H_X,
    'w' ^ XL5H_X, 'o' ^ XL5H_X, 'r' ^ XL5H_X, 'k' ^ XL5H_X,
    'b' ^ XL5H_X, 'o' ^ XL5H_X, 'o' ^ XL5H_X, 'k' ^ XL5H_X
};

BYTE FAR gabyXL5H_VSTR_workbooks[] =
{
    'w' ^ XL5H_X, 'o' ^ XL5H_X, 'r' ^ XL5H_X, 'k' ^ XL5H_X,
    'b' ^ XL5H_X, 'o' ^ XL5H_X, 'o' ^ XL5H_X, 'k' ^ XL5H_X,
    's' ^ XL5H_X
};

typedef struct tagXL5H_VSTR
{
    DWORD           dwCRC;
    WORD            wID;
    WORD            wStrLen;
    LPBYTE          lpabyStr;
} XL5H_VSTR_T, FAR *LPXL5H_VSTR;

XL5H_VSTR_T FAR gastXL5H_VSTR[XL5H_VSTR_ID_COUNT] =
{
    { 0x02E58C18, XL5H_VSTR_ID_add,
      3, gabyXL5H_VSTR_add },
    { 0x04B8594D, XL5H_VSTR_ID_pathseparator,
      13, gabyXL5H_VSTR_pathseparator },
    { 0x1102144E, XL5H_VSTR_ID_workbooks,
      9, gabyXL5H_VSTR_workbooks },
    { 0x2196FFC5, XL5H_VSTR_ID_altstartuppath,
      14, gabyXL5H_VSTR_altstartuppath },
    { 0x27C4A251, XL5H_VSTR_ID_before,
      6, gabyXL5H_VSTR_before },
    { 0x346F16F2, XL5H_VSTR_ID_thisworkbook,
      12, gabyXL5H_VSTR_thisworkbook },
    { 0x397D809F, XL5H_VSTR_ID_startuppath,
      11, gabyXL5H_VSTR_startuppath },
    { 0x5BA4223E, XL5H_VSTR_ID_application,
      11, gabyXL5H_VSTR_application },
    { 0x76BBB1BE, XL5H_VSTR_ID_after,
      5, gabyXL5H_VSTR_after },
    { 0xA1DC81F9, XL5H_VSTR_ID_name,
      4, gabyXL5H_VSTR_name },
    { 0xAA728F31, XL5H_VSTR_ID_sheets,
      6, gabyXL5H_VSTR_sheets },
    { 0xB245447D, XL5H_VSTR_ID_copy,
      4, gabyXL5H_VSTR_copy },
    { 0xB91414FD, XL5H_VSTR_ID_insertfile,
      10, gabyXL5H_VSTR_insertfile },
    { 0xC3F4169A, XL5H_VSTR_ID_filename,
      8, gabyXL5H_VSTR_filename },
    { 0xCB868CD6, XL5H_VSTR_ID_activesheet,
      11, gabyXL5H_VSTR_activesheet },
    { 0xD148BC28, XL5H_VSTR_ID_modules,
      7, gabyXL5H_VSTR_modules },
    { 0xD27BF325, XL5H_VSTR_ID_saveas,
      6, gabyXL5H_VSTR_saveas },
    { 0xE955D3EB, XL5H_VSTR_ID_savecopyas,
      10, gabyXL5H_VSTR_savecopyas },
    { 0xF41B4FCB, XL5H_VSTR_ID_activeworkbook,
      14, gabyXL5H_VSTR_activeworkbook }
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL XL5HGetAppStrID
(
    LPXL5H          lpstEnv
)
{
    int             i;
    int             nRecBytesLeft;
    int             nCount;

    DWORD           dwOffset;
    DWORD           dwEndOffset;
    LPBYTE          lpabyRecBuf;
    LPBYTE          lpabyStrBuf;
    LPBYTE          lpabyStr;

    WORD            wIdentOffset;
    WORD            wNextIdentOffset;
    WORD            wOffset;
    WORD            wStrBufStartOffset;
    WORD            wStrBufEndOffset;

    DWORD           dwCount;
    DWORD           dwTemp;
    BYTE            by;
    WORD            wStrLen;
    DWORD           dwCRC;

    BYTE            abyCRCHit[256/8];

    // Initialize all IDs to zero

    for (i=0;i<XL5H_VSTR_ID_COUNT;i++)
        lpstEnv->awAppStrID[i] = 0;

    // Set the CRC hit array

    for (i=0;i<sizeof(abyCRCHit);i++)
        abyCRCHit[i] = 0;

    for (i=0;i<XL5H_VSTR_ID_COUNT;i++)
    {
        by = (BYTE)gastXL5H_VSTR[i].dwCRC;
        abyCRCHit[by >> 3] |= (1 << (by & 7));
    }

    dwOffset = lpstEnv->stDirInfo.dwStringRecordOffset;
    dwEndOffset = dwOffset + lpstEnv->stDirInfo.dwStringRecordLen;
    dwCount = lpstEnv->nBufSize / 2;
    nRecBytesLeft = 0;
    wStrBufEndOffset = 0;
    wNextIdentOffset = 0;
    wStrBufStartOffset = 0;
    lpabyStrBuf = NULL;
    while (1)
    {
        if (nRecBytesLeft < 10)
        {
            // Refill the record buffer

            if (dwOffset + 10 > dwEndOffset)
                break;

            if (dwEndOffset - dwOffset < dwCount)
                nCount = (int)(dwEndOffset - dwOffset);
            else
                nCount = (int)dwCount;

            lpabyRecBuf = lpstEnv->lpabyBuf;
            if (SSSeekRead(lpstEnv->lpstDirStream,
                           dwOffset,
                           lpabyRecBuf,
                           nCount,
                           &dwTemp) != SS_STATUS_OK ||
                dwTemp != nCount)
            {
                // Just continue

                break;
            }

            nRecBytesLeft = nCount;
        }

        // Try the identifier at offset 6

        wIdentOffset = (WORD)lpabyRecBuf[6] |
            (((WORD)lpabyRecBuf[7]) << 8);

        if (wIdentOffset != wNextIdentOffset &&
            wIdentOffset != wNextIdentOffset + 2)
        {
            // Try at offset 8 next

            wIdentOffset = (WORD)lpabyRecBuf[8] |
                (((WORD)lpabyRecBuf[9]) << 8);

            if (wIdentOffset != wNextIdentOffset &&
                wIdentOffset != wNextIdentOffset + 2)
                break;

            nRecBytesLeft -= 2;
            dwOffset += 2;
            lpabyRecBuf += 2;
        }

        // Now get the string at wIdentOffset in the string portion

        lpabyStr = lpabyStrBuf + (wIdentOffset - wStrBufStartOffset);

        wOffset = wIdentOffset;
        CRC32Init(dwCRC);
        while (1)
        {
            if (wOffset >= wStrBufEndOffset)
            {
                // Refill the buffer

                if ((DWORD)wOffset + 2 > lpstEnv->stDirInfo.dwStringLen)
                    break;

                if (lpstEnv->stDirInfo.dwStringLen - wOffset < dwCount)
                    nCount = (int)(lpstEnv->stDirInfo.dwStringLen - wOffset);
                else
                    nCount = (int)dwCount;

                lpabyStrBuf = lpstEnv->lpabyBuf + dwCount;

                if (SSSeekRead(lpstEnv->lpstDirStream,
                               lpstEnv->stDirInfo.dwStringOffset + wOffset,
                               lpabyStrBuf,
                               nCount,
                               &dwTemp) != SS_STATUS_OK ||
                    dwTemp != nCount)
                {
                    // Just continue

                    break;
                }

                wStrBufStartOffset = wOffset;
                wStrBufEndOffset = wOffset + nCount;

                lpabyStr = lpabyStrBuf;
            }

            by = *lpabyStr++;
            if (by == 0)
                break;

            by = SSToLower(by);
            CRC32Continue(dwCRC,by);
            ++wOffset;
        }

        // See if there is a possible hit

        by = (BYTE)dwCRC;
        wStrLen = wOffset - wIdentOffset;
        if (abyCRCHit[by >> 3] & (1 << (by & 7)))
        {
            int     nLow, nMid, nHigh;

            // Now do a binary search

            nLow = 0;
            nHigh = XL5H_VSTR_ID_COUNT - 1;
            while (nLow <= nHigh)
            {
                nMid = (nLow + nHigh) >> 1;

                if (dwCRC < gastXL5H_VSTR[nMid].dwCRC)
                    nHigh = nMid - 1;   // In lower half
                else
                if (dwCRC > gastXL5H_VSTR[nMid].dwCRC)
                    nLow = nMid + 1;
                else
                {
                    // Make sure the length matches

                    if (wStrLen != gastXL5H_VSTR[nMid].wStrLen)
                        break;

                    // Do an actual comparison

                    // Is the entire string in the buffer?

                    if (!(wStrBufStartOffset <= wIdentOffset &&
                          wIdentOffset + wStrLen <= wStrBufEndOffset))
                    {
                        // Refill the buffer beginning with the string

                        if (lpstEnv->stDirInfo.dwStringLen - wIdentOffset <
                            dwCount)
                            nCount = (int)(lpstEnv->stDirInfo.dwStringLen -
                                wIdentOffset);
                        else
                            nCount = (int)dwCount;

                        lpabyStrBuf = lpstEnv->lpabyBuf + dwCount;

                        if (SSSeekRead(lpstEnv->lpstDirStream,
                                       lpstEnv->stDirInfo.dwStringOffset +
                                           wIdentOffset,
                                       lpabyStrBuf,
                                       nCount,
                                       &dwTemp) != SS_STATUS_OK ||
                            dwTemp != nCount)
                        {
                            // Just continue

                            break;
                        }

                        wStrBufStartOffset = wIdentOffset;
                        wStrBufEndOffset = wIdentOffset + nCount;

                        lpabyStr = lpabyStrBuf;
                    }
                    else
                        lpabyStr = lpabyStrBuf +
                            (wIdentOffset - wStrBufStartOffset);

                    // Now compare the strings

                    for (wOffset=0;wOffset<wStrLen;wOffset++)
                    {
                        if (gastXL5H_VSTR[nMid].lpabyStr[wOffset] !=
                            (SSToLower(*lpabyStr++) ^ XL5H_X))
                            break;
                    }

                    if (wOffset == wStrLen)
                    {
                        // Found a match

                        lpstEnv->awAppStrID[gastXL5H_VSTR[nMid].wID] = (WORD)
                            (dwOffset - lpstEnv->stDirInfo.
                                 dwStringRecordOffset);
                    }

                    break;
                }
            }
        }

        // Apparently the length must be at least two

        if (wStrLen == 1)
            ++wStrLen;

        // Account for the zero terminator and round to next
        //  WORD boundary

        if (++wStrLen & 1)
            ++wStrLen;

        wNextIdentOffset = wIdentOffset + wStrLen;

        nRecBytesLeft += 10;
        dwOffset += 10;
        lpabyRecBuf += 10;
    }

    return(TRUE);
}


BOOL XL5H97GetAppStrID
(
    LPXL5H          lpstEnv
)
{
    DWORD           dwIdentOffset;
    DWORD           dwNumIdents;

    int             i;
    BYTE            abyCRCHit[256/8];

    DWORD           dw;
    WORD            wID;

    int             nNumBufBytesLeft;
    LPBYTE          lpabyStr;
    DWORD           dwTemp;
    BYTE            by;
    DWORD           dwEndOffset;
    int             nLen;
    DWORD           dwCRC;
    int             nVersion;
    BOOL            bLittleEndian;
    int             nNotKeywordSkip;

    if (O97Get_VBA_PROJECTEndian(lpstEnv->lpstDirStream,
                                 &nVersion,
                                 &bLittleEndian) == FALSE)
        return(FALSE);

    // Get the identifier table offset

    if (O97GetIdentifierTableOffset(lpstEnv->lpstDirStream,
                                    &dwIdentOffset,
                                    &dwNumIdents) == FALSE)
        return(FALSE);

    // Initialize all IDs to zero

    for (i=0;i<XL5H_VSTR_ID_COUNT;i++)
        lpstEnv->awAppStrID[i] = 0;

    // Set the CRC hit array

    for (i=0;i<sizeof(abyCRCHit);i++)
        abyCRCHit[i] = 0;

    for (i=0;i<XL5H_VSTR_ID_COUNT;i++)
    {
        by = (BYTE)gastXL5H_VSTR[i].dwCRC;
        abyCRCHit[by >> 3] |= (1 << (by & 7));
    }

    wID = 0x200;
    nNumBufBytesLeft = 0;
    dwEndOffset = SSStreamLen(lpstEnv->lpstDirStream);
    nNotKeywordSkip = 4;
    for (dw=0;dw<32700;dw++)
    {
        // Are there enough bytes left in the buffer?

        if (nNumBufBytesLeft >= 2)
        {
            if (bLittleEndian)
            {
                if (nNumBufBytesLeft <
                    (((lpabyStr[1] & 0x80) ? 8 : 2) + lpabyStr[0]))
                    nNumBufBytesLeft = 0;
            }
            else
            {
                if (nNumBufBytesLeft <
                    (((lpabyStr[0] & 0x80) ? 8 : 2) + lpabyStr[1]))
                    nNumBufBytesLeft = 0;
            }
        }

        if (nNumBufBytesLeft < 6)
        {
            if (dwEndOffset < dwIdentOffset)
                break;

            // Need to refill the buffer

            if (dwEndOffset - dwIdentOffset < lpstEnv->nBufSize)
                nNumBufBytesLeft = (int)(dwEndOffset - dwIdentOffset);
            else
                nNumBufBytesLeft = lpstEnv->nBufSize;

            lpabyStr = lpstEnv->lpabyBuf;
            if (SSSeekRead(lpstEnv->lpstDirStream,
                           dwIdentOffset,
                           lpabyStr,
                           nNumBufBytesLeft,
                           &dwTemp) != SS_STATUS_OK ||
                dwTemp != nNumBufBytesLeft)
                break;
        }

        // Make sure there are enough bytes

        if (nNumBufBytesLeft < 6)
            break;

        if (lpabyStr[0] == 0x02 && lpabyStr[1] == 0xFF)
            break;

        // Check for keyword entries

        if (lpabyStr[0] == 0 && lpabyStr[1] == 0)
        {
            nNotKeywordSkip = 0;
            lpabyStr += 4;
            nNumBufBytesLeft -= 4;
            dwIdentOffset += 4;
        }

        if (bLittleEndian)
            nLen = lpabyStr[0];
        else
            nLen = lpabyStr[1];

        if ((bLittleEndian && (lpabyStr[1] & 0x80)) ||
            (bLittleEndian == FALSE && (lpabyStr[0] & 0x80)))
        {
            if (nNumBufBytesLeft < 8 + nLen)
                break;

            lpabyStr += 8;
            nNumBufBytesLeft -= (8 + nNotKeywordSkip + nLen);
            dwIdentOffset += (8 + nNotKeywordSkip + nLen);
        }
        else
        {
            if (nNumBufBytesLeft < 2 + nLen)
                break;

            lpabyStr += 2;
            nNumBufBytesLeft -= (2 + nNotKeywordSkip + nLen);
            dwIdentOffset += (2 + nNotKeywordSkip + nLen);
        }

        CRC32Init(dwCRC);
        for (i=0;i<nLen;i++)
        {
            by = lpabyStr[i] = SSToLower(lpabyStr[i]);
            dwCRC = CRC32Continue(dwCRC,by);
        }

        // See if there is a possible hit

        by = (BYTE)dwCRC;
        if (abyCRCHit[by >> 3] & (1 << (by & 7)))
        {
            int     nLow, nMid, nHigh;

            // Now do a binary search

            nLow = 0;
            nHigh = XL5H_VSTR_ID_COUNT - 1;
            while (nLow <= nHigh)
            {
                nMid = (nLow + nHigh) >> 1;

                if (dwCRC < gastXL5H_VSTR[nMid].dwCRC)
                    nHigh = nMid - 1;   // In lower half
                else
                if (dwCRC > gastXL5H_VSTR[nMid].dwCRC)
                    nLow = nMid + 1;
                else
                {
                    // Make sure the length matches

                    if ((WORD)nLen != gastXL5H_VSTR[nMid].wStrLen)
                        break;

                    // Do an actual comparison

                    for (i=0;i<nLen;i++)
                    {
                        if (gastXL5H_VSTR[nMid].lpabyStr[i] !=
                            (lpabyStr[i] ^ XL5H_X))
                            break;
                    }

                    if (i == nLen)
                    {
                        // Found a match

                        lpstEnv->awAppStrID[gastXL5H_VSTR[nMid].wID] = wID;
                    }

                    break;
                }
            }
        }

        // Move over the string and the end bytes

        lpabyStr += nLen + nNotKeywordSkip;

        if (nNotKeywordSkip == 0)
            nNotKeywordSkip = 4;
        else
            wID += 2;
    }

    // The ID for Name is fixed

    if (nVersion == O97_VERSION_O97)
        lpstEnv->awAppStrID[XL5H_VSTR_ID_name] = 0x0108;
    else
        lpstEnv->awAppStrID[XL5H_VSTR_ID_name] = 0x0106;

    return(TRUE);
}

// The following table translates O97 opcodes to XL5

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXL5HOpO97toXL5[0x100] =
{
    0x00,       // 0x00 // Imp
    0x01,       // 0x01 // Eqv
    0x02,       // 0x02 // Xor
    0x03,       // 0x03 // Or
    0x04,       // 0x04 // And
    0x05,       // 0x05 // Eq
    0x06,       // 0x06 // Ne
    0x07,       // 0x07 // Le
    0x08,       // 0x08 // Ge
    0x09,       // 0x09 // Lt
    0x0A,       // 0x0A // Gt
    0x0B,       // 0x0B // Add
    0x0C,       // 0x0C // Sub
    0x0D,       // 0x0D // Mod
    0x0E,       // 0x0E // IDv
    0x0F,       // 0x0F // Mul
    0x10,       // 0x10 // Div
    0x11,       // 0x11 // Concat
    0x12,       // 0x12 // Like
    0x13,       // 0x13 // Pwr
    0x14,       // 0x14 // Is
    0x15,       // 0x15 // Not
    0x16,       // 0x16 // UMi
    0x17,       // 0x17 // FnAbs
    0x18,       // 0x18 // FnFix
    0x19,       // 0x19 // FnInt
    0x1A,       // 0x1A // FnSgn
    0x1B,       // 0x1B // FnLen
    0x1C,       // 0x1C // FnLenB
    0x1D,       // 0x1D // Paren
    0x1E,       // 0x1E // Sharp
    0x1F,       // 0x1F // LdLHS
    0x20,       // 0x20 // Ld
    0x21,       // 0x21 // MemLd
    0x22,       // 0x22 // DictLd
    0x23,       // 0x23 // IndexLd
    0x24,       // 0x24 // ArgsLd
    0x25,       // 0x25 // ArgsMemLd
    0x26,       // 0x26 // ArgsDictLd
    0x27,       // 0x27 // St
    0x28,       // 0x28 // MemSt
    0x29,       // 0x29 // DictSt
    0x2A,       // 0x2A // IndexSt
    0x2B,       // 0x2B // ArgsSt
    0x2C,       // 0x2C // ArgsMemSt
    0x2D,       // 0x2D // ArgsDictSt
    0x2E,       // 0x2E // Set
    0x2F,       // 0x2F // MemSet
    0x30,       // 0x30 // DictSet
    0x31,       // 0x31 // IndexSet
    0x32,       // 0x32 // ArgsSet
    0x33,       // 0x33 // ArgsMemSet
    0x34,       // 0x34 // ArgsDictSet
    0x35,       // 0x35 // MemLdWith
    0x36,       // 0x36 // DictLdWith
    0x37,       // 0x37 // ArgsMemLdWith
    0x38,       // 0x38 // ArgsDictLdWith
    0x39,       // 0x39 // MemStWith
    0x3A,       // 0x3A // DictStWith
    0x3B,       // 0x3B // ArgsMemStWith
    0x3C,       // 0x3C // ArgsDictStWith
    0x3D,       // 0x3D // MemSetWith
    0x3E,       // 0x3E // DictSetWith
    0x3F,       // 0x3F // ArgsMemSetWith
    0x40,       // 0x40 // ArgsDictSetWith
    0x41,       // 0x41 // ArgsCall
    0x42,       // 0x42 // ArgsMemCall
    0x43,       // 0x43 // ArgsMemCallWith
    0xA4,       // 0x44 // *ArgsArray
    0x44,       // 0x45 // Bos
    0x45,       // 0x46 // BosImplicit
    0x46,       // 0x47 // Bol
    0x47,       // 0x48 // Case
    0x48,       // 0x49 // CaseTo
    0x49,       // 0x4A // CaseGt
    0x4A,       // 0x4B // CaseLt
    0x4B,       // 0x4C // CaseGe
    0x4C,       // 0x4D // CaseLe
    0x4D,       // 0x4E // CaseNe
    0x4E,       // 0x4F // CaseEq
    0x4F,       // 0x50 // CaseElse
    0x50,       // 0x51 // CaseDone
    0x51,       // 0x52 // Circle
    0x52,       // 0x53 // Close
    0x53,       // 0x54 // CloseAll
    0x54,       // 0x55 // Coerce
    0x55,       // 0x56 // CoerceVar
    0x56,       // 0x57 // Context
    0x57,       // 0x58 // Debug
    0x58,       // 0x59 // Deftype
    0x59,       // 0x5A // Dim
    0x5A,       // 0x5B // DimImplicit
    0x5B,       // 0x5C // Do
    0x5C,       // 0x5D // DoEvents
    0x5D,       // 0x5E // DoUntil
    0x5E,       // 0x5F // DoWhile
    0x5F,       // 0x60 // Else
    0x60,       // 0x61 // ElseBlock
    0x61,       // 0x62 // ElseIfBlock
    0x62,       // 0x63 // ElseIfTypeBlock
    0x63,       // 0x64 // End
    0x64,       // 0x65 // EndContext
    0x65,       // 0x66 // EndFunc
    0x66,       // 0x67 // EndIf
    0x67,       // 0x68 // EndIfBlock
    0x68,       // 0x69 // EndImmediate
    0x69,       // 0x6A // EndProp
    0x6A,       // 0x6B // EndSelect
    0x6B,       // 0x6C // EndSub
    0x6C,       // 0x6D // EndType
    0x6D,       // 0x6E // EndWith
    0x6E,       // 0x6F // Erase
    0x6F,       // 0x70 // Error
    0x70,       // 0x71 // ExitDo
    0x71,       // 0x72 // ExitFor
    0x72,       // 0x73 // ExitFunc
    0x73,       // 0x74 // ExitProp
    0x74,       // 0x75 // ExitSub
    0x75,       // 0x76 // FnCurDir
    0x76,       // 0x77 // FnDir
    0x77,       // 0x78 // Empty0
    0x78,       // 0x79 // Empty1
    0x79,       // 0x7A // FnError
    0x7A,       // 0x7B // FnFormat
    0x7B,       // 0x7C // FnFreeFile
    0x7C,       // 0x7D // FnInStr
    0x7D,       // 0x7E // FnInStr3
    0x7E,       // 0x7F // FnInStr4
    0x7F,       // 0x80 // FnInStrB
    0x80,       // 0x81 // FnInStrB3
    0x81,       // 0x82 // FnInStrB4
    0x82,       // 0x83 // FnLBound
    0x83,       // 0x84 // FnMid
    0x84,       // 0x85 // FnMidB
    0x85,       // 0x86 // FnStrComp
    0x86,       // 0x87 // FnStrComp3
    0x87,       // 0x88 // FnStringVar
    0x88,       // 0x89 // FnStringStr
    0x89,       // 0x8A // FnUBound
    0x8A,       // 0x8B // For
    0x8B,       // 0x8C // ForEach
    0x8C,       // 0x8D // ForEachAs
    0x8D,       // 0x8E // ForStep
    0x8E,       // 0x8F // FuncDefn
    0x8F,       // 0x90 // FuncDefnSave
    0x90,       // 0x91 // GetRec
    0x91,       // 0x92 // Gosub
    0x92,       // 0x93 // Goto
    0x93,       // 0x94 // If
    0x94,       // 0x95 // IfBlock
    0x95,       // 0x96 // TypeOf
    0x96,       // 0x97 // IfTypeBlock
    0x97,       // 0x98 // Input
    0x98,       // 0x99 // InputDone
    0x99,       // 0x9A // InputItem
    0x9A,       // 0x9B // Label
    0x9B,       // 0x9C // Let
    0x9C,       // 0x9D // Line
    0x9D,       // 0x9E // LineCont
    0x9E,       // 0x9F // LineInput
    0x9F,       // 0xA0 // LineNum
    0xA0,       // 0xA1 // LitCy
    0xA1,       // 0xA2 // LitDate
    0xA2,       // 0xA3 // LitDefault
    0xA3,       // 0xA4 // LitDI2
    0xA4,       // 0xA5 // LitDI4
    0xA5,       // 0xA6 // LitHI2
    0xA6,       // 0xA7 // LitHI4
    0xA7,       // 0xA8 // LitNothing
    0xA8,       // 0xA9 // LitOI2
    0xA9,       // 0xAA // LitOI4
    0xAA,       // 0xAB // LitR4
    0xAB,       // 0xAC // LitR8
    0xAC,       // 0xAD // LitSmallI2
    0xAD,       // 0xAE // LitStr
    0xAE,       // 0xAF // LitVarSpecial
    0xAF,       // 0xB0 // Lock
    0xB0,       // 0xB1 // Loop
    0xB1,       // 0xB2 // LoopUntil
    0xB2,       // 0xB3 // LoopWhile
    0xB3,       // 0xB4 // Lset
    0xB4,       // 0xB5 // Me
    0xB5,       // 0xB6 // MeImplicit
    0xB6,       // 0xB7 // MemRedim
    0xB7,       // 0xB8 // MemRedimWith
    0xB8,       // 0xB9 // MemRedimAs
    0xB9,       // 0xBA // MemRedimAsWith
    0xBA,       // 0xBB // Mid
    0xBB,       // 0xBC // MidB
    0xBC,       // 0xBD // Name
    0xBD,       // 0xBE // New
    0xBE,       // 0xBF // Next
    0xBF,       // 0xC0 // NextVar
    0xC0,       // 0xC1 // OnError
    0xC1,       // 0xC2 // OnGosub
    0xC2,       // 0xC3 // OnGoto
    0xC3,       // 0xC4 // Open
    0xC4,       // 0xC5 // Option
    0xC5,       // 0xC6 // OptionBase
    0xC6,       // 0xC7 // ParamByval
    0xC7,       // 0xC8 // ParamOmitted
    0xC8,       // 0xC9 // ParamNamed
    0xC9,       // 0xCA // PrintChan
    0xCA,       // 0xCB // PrintComma
    0xCB,       // 0xCC // PrintEos
    0xCC,       // 0xCD // PrintItemComma
    0xCD,       // 0xCE // PrintItemNL
    0xCE,       // 0xCF // PrintItemSemi
    0xCF,       // 0xD0 // PrintNL
    0xD0,       // 0xD1 // PrintObj
    0xD1,       // 0xD2 // PrintSemi
    0xD2,       // 0xD3 // PrintSpc
    0xD3,       // 0xD4 // PrintTab
    0xD4,       // 0xD5 // PrintTabComma
    0xD5,       // 0xD6 // Pset
    0xD6,       // 0xD7 // PutRec
    0xD7,       // 0xD8 // QuoteRem
    0xD8,       // 0xD9 // Redim
    0xD9,       // 0xDA // RedimAs
    0xDA,       // 0xDB // Reparse
    0xDB,       // 0xDC // Rem
    0xDC,       // 0xDD // Resume
    0xDD,       // 0xDE // Return
    0xDE,       // 0xDF // Rset
    0xDF,       // 0xE0 // Scale
    0xE0,       // 0xE1 // Seek
    0xE1,       // 0xE2 // SelectCase
    0xE2,       // 0xE3 // SelectIs
    0xE3,       // 0xE4 // SelectType
    0xE4,       // 0xE5 // SetStmt
    0xE5,       // 0xE6 // Stack
    0xE6,       // 0xE7 // Stop
    0xE7,       // 0xE8 // Type
    0xE8,       // 0xE9 // UnLock
    0xE9,       // 0xEA // VarDefn
    0xEA,       // 0xEB // Wend
    0xEB,       // 0xEC // While
    0xEC,       // 0xED // With
    0xED,       // 0xEE // WriteChan
    0xEE,       // 0xEF // ConstFuncExpr
    0xEF,       // 0xF0 // LbConst
    XL5_OP_MAX, // 0xF1 // *LbIf
    XL5_OP_MAX, // 0xF2 // *LbElse
    XL5_OP_MAX, // 0xF3 // *LbElseif
    XL5_OP_MAX, // 0xF4 // *LbEndif
    XL5_OP_MAX, // 0xF5 // *LbMark
    XL5_OP_MAX, // 0xF6 // *EndForVariable
    XL5_OP_MAX, // 0xF7 // *StartForVariable
    XL5_OP_MAX, // 0xF8 // *NewRedim
    XL5_OP_MAX, // 0xF9 // *StartWithExpr
    0xA3,       // 0xFA // *SetOrSt
    XL5_OP_MAX, // 0xFB // *EndEnum
    XL5_OP_MAX, // 0xFC // *Illegal
    XL5_OP_MAX, // 0xFD // *Illegal
    XL5_OP_MAX, // 0xFE // *Illegal
    XL5_OP_MAX, // 0xFF // *Illegal
};

// The following table translates O98 opcodes to XL5

BYTE FAR gabyXL5HOpO98toXL5[0x105] =
{
    0x00,       // 0x00 // Imp
    0x01,       // 0x01 // Eqv
    0x02,       // 0x02 // Xor
    0x03,       // 0x03 // Or
    0x04,       // 0x04 // And
    0x05,       // 0x05 // Eq
    0x06,       // 0x06 // Ne
    0x07,       // 0x07 // Le
    0x08,       // 0x08 // Ge
    0x09,       // 0x09 // Lt
    0x0A,       // 0x0A // Gt
    0x0B,       // 0x0B // Add
    0x0C,       // 0x0C // Sub
    0x0D,       // 0x0D // Mod
    0x0E,       // 0x0E // IDv
    0x0F,       // 0x0F // Mul
    0x10,       // 0x10 // Div
    0x11,       // 0x11 // Concat
    0x12,       // 0x12 // Like
    0x13,       // 0x13 // Pwr
    0x14,       // 0x14 // Is
    0x15,       // 0x15 // Not
    0x16,       // 0x16 // UMi
    0x17,       // 0x17 // FnAbs
    0x18,       // 0x18 // FnFix
    0x19,       // 0x19 // FnInt
    0x1A,       // 0x1A // FnSgn
    0x1B,       // 0x1B // FnLen
    0x1C,       // 0x1C // FnLenB
    0x1D,       // 0x1D // Paren
    0x1E,       // 0x1E // Sharp
    0x1F,       // 0x1F // LdLHS
    0x20,       // 0x20 // Ld
    0x21,       // 0x21 // MemLd
    0x22,       // 0x22 // DictLd
    0x23,       // 0x23 // IndexLd
    0x24,       // 0x24 // ArgsLd
    0x25,       // 0x25 // ArgsMemLd
    0x26,       // 0x26 // ArgsDictLd
    0x27,       // 0x27 // St
    0x28,       // 0x28 // MemSt
    0x29,       // 0x29 // DictSt
    0x2A,       // 0x2A // IndexSt
    0x2B,       // 0x2B // ArgsSt
    0x2C,       // 0x2C // ArgsMemSt
    0x2D,       // 0x2D // ArgsDictSt
    0x2E,       // 0x2E // Set
    0x2F,       // 0x2F // MemSet
    0x30,       // 0x30 // DictSet
    0x31,       // 0x31 // IndexSet
    0x32,       // 0x32 // ArgsSet
    0x33,       // 0x33 // ArgsMemSet
    0x34,       // 0x34 // ArgsDictSet
    0x35,       // 0x35 // MemLdWith
    0x36,       // 0x36 // DictLdWith
    0x37,       // 0x37 // ArgsMemLdWith
    0x38,       // 0x38 // ArgsDictLdWith
    0x39,       // 0x39 // MemStWith
    0x3A,       // 0x3A // DictStWith
    0x3B,       // 0x3B // ArgsMemStWith
    0x3C,       // 0x3C // ArgsDictStWith
    0x3D,       // 0x3D // MemSetWith
    0x3E,       // 0x3E // DictSetWith
    0x3F,       // 0x3F // ArgsMemSetWith
    0x40,       // 0x40 // ArgsDictSetWith
    0x41,       // 0x41 // ArgsCall
    0x42,       // 0x42 // ArgsMemCall
    0x43,       // 0x43 // ArgsMemCallWith
    0xA4,       // 0x44 // *ArgsArray
    XL5_OP_MAX, // 0x45 // *Assert
    0x44,       // 0x46 // Bos
    0x45,       // 0x47 // BosImplicit
    0x46,       // 0x48 // Bol
    0xA3,       // 0x49 // *LdAddressOf
    0xA3,       // 0x4A // *MemAddressOf
    0x47,       // 0x4B // Case
    0x48,       // 0x4C // CaseTo
    0x49,       // 0x4D // CaseGt
    0x4A,       // 0x4E // CaseLt
    0x4B,       // 0x4F // CaseGe
    0x4C,       // 0x50 // CaseLe
    0x4D,       // 0x51 // CaseNe
    0x4E,       // 0x52 // CaseEq
    0x4F,       // 0x53 // CaseElse
    0x50,       // 0x54 // CaseDone
    0x51,       // 0x55 // Circle
    0x52,       // 0x56 // Close
    0x53,       // 0x57 // CloseAll
    0x54,       // 0x58 // Coerce
    0x55,       // 0x59 // CoerceVar
    0x56,       // 0x5A // Context
    0x57,       // 0x5B // Debug
    0x58,       // 0x5C // Deftype
    0x59,       // 0x5D // Dim
    0x5A,       // 0x5E // DimImplicit
    0x5B,       // 0x5F // Do
    0x5C,       // 0x60 // DoEvents
    0x5D,       // 0x61 // DoUntil
    0x5E,       // 0x62 // DoWhile
    0x5F,       // 0x63 // Else
    0x60,       // 0x64 // ElseBlock
    0x61,       // 0x65 // ElseIfBlock
    0x62,       // 0x66 // ElseIfTypeBlock
    0x63,       // 0x67 // End
    0x64,       // 0x68 // EndContext
    0x65,       // 0x69 // EndFunc
    0x66,       // 0x6A // EndIf
    0x67,       // 0x6B // EndIfBlock
    0x68,       // 0x6C // EndImmediate
    0x69,       // 0x6D // EndProp
    0x6A,       // 0x6E // EndSelect
    0x6B,       // 0x6F // EndSub
    0x6C,       // 0x70 // EndType
    0x6D,       // 0x71 // EndWith
    0x6E,       // 0x72 // Erase
    0x6F,       // 0x73 // Error
    0xA4,       // 0x74 // *EventDecl
    0xA4,       // 0x75 // *RaiseEvent
    0xA4,       // 0x76 // *ArgsMemRaiseEvent
    0xA4,       // 0x77 // *ArgsMemRaiseEventWith
    0x70,       // 0x78 // ExitDo
    0x71,       // 0x79 // ExitFor
    0x72,       // 0x7A // ExitFunc
    0x73,       // 0x7B // ExitProp
    0x74,       // 0x7C // ExitSub
    0x75,       // 0x7D // FnCurDir
    0x76,       // 0x7E // FnDir
    0x77,       // 0x7F // Empty0
    0x78,       // 0x80 // Empty1
    0x79,       // 0x81 // FnError
    0x7A,       // 0x82 // FnFormat
    0x7B,       // 0x83 // FnFreeFile
    0x7C,       // 0x84 // FnInStr
    0x7D,       // 0x85 // FnInStr3
    0x7E,       // 0x86 // FnInStr4
    0x7F,       // 0x87 // FnInStrB
    0x80,       // 0x88 // FnInStrB3
    0x81,       // 0x89 // FnInStrB4
    0x82,       // 0x8A // FnLBound
    0x83,       // 0x8B // FnMid
    0x84,       // 0x8C // FnMidB
    0x85,       // 0x8D // FnStrComp
    0x86,       // 0x8E // FnStrComp3
    0x87,       // 0x8F // FnStringVar
    0x88,       // 0x90 // FnStringStr
    0x89,       // 0x91 // FnUBound
    0x8A,       // 0x92 // For
    0x8B,       // 0x93 // ForEach
    0x8C,       // 0x94 // ForEachAs
    0x8D,       // 0x95 // ForStep
    0x8E,       // 0x96 // FuncDefn
    0x8F,       // 0x97 // FuncDefnSave
    0x90,       // 0x98 // GetRec
    0x91,       // 0x99 // Gosub
    0x92,       // 0x9A // Goto
    0x93,       // 0x9B // If
    0x94,       // 0x9C // IfBlock
    0x95,       // 0x9D // TypeOf
    0x96,       // 0x9E // IfTypeBlock
    0xA4,       // 0x9F // Implements
    0x97,       // 0xA0 // Input
    0x98,       // 0xA1 // InputDone
    0x99,       // 0xA2 // InputItem
    0x9A,       // 0xA3 // Label
    0x9B,       // 0xA4 // Let
    0x9C,       // 0xA5 // Line
    0x9D,       // 0xA6 // LineCont
    0x9E,       // 0xA7 // LineInput
    0x9F,       // 0xA8 // LineNum
    0xA0,       // 0xA9 // LitCy
    0xA1,       // 0xAA // LitDate
    0xA2,       // 0xAB // LitDefault
    0xA3,       // 0xAC // LitDI2
    0xA4,       // 0xAD // LitDI4
    0xA5,       // 0xAE // LitHI2
    0xA6,       // 0xAF // LitHI4
    0xA7,       // 0xB0 // LitNothing
    0xA8,       // 0xB1 // LitOI2
    0xA9,       // 0xB2 // LitOI4
    0xAA,       // 0xB3 // LitR4
    0xAB,       // 0xB4 // LitR8
    0xAC,       // 0xB5 // LitSmallI2
    0xAD,       // 0xB6 // LitStr
    0xAE,       // 0xB7 // LitVarSpecial
    0xAF,       // 0xB8 // Lock
    0xB0,       // 0xB9 // Loop
    0xB1,       // 0xBA // LoopUntil
    0xB2,       // 0xBB // LoopWhile
    0xB3,       // 0xBC // Lset
    0xB4,       // 0xBD // Me
    0xB5,       // 0xBE // MeImplicit
    0xB6,       // 0xBF // MemRedim
    0xB7,       // 0xC0 // MemRedimWith
    0xB8,       // 0xC1 // MemRedimAs
    0xB9,       // 0xC2 // MemRedimAsWith
    0xBA,       // 0xC3 // Mid
    0xBB,       // 0xC4 // MidB
    0xBC,       // 0xC5 // Name
    0xBD,       // 0xC6 // New
    0xBE,       // 0xC7 // Next
    0xBF,       // 0xC8 // NextVar
    0xC0,       // 0xC9 // OnError
    0xC1,       // 0xCA // OnGosub
    0xC2,       // 0xCB // OnGoto
    0xC3,       // 0xCC // Open
    0xC4,       // 0xCD // Option
    0xC5,       // 0xCE // OptionBase
    0xC6,       // 0xCF // ParamByval
    0xC7,       // 0xD0 // ParamOmitted
    0xC8,       // 0xD1 // ParamNamed
    0xC9,       // 0xD2 // PrintChan
    0xCA,       // 0xD3 // PrintComma
    0xCB,       // 0xD4 // PrintEos
    0xCC,       // 0xD5 // PrintItemComma
    0xCD,       // 0xD6 // PrintItemNL
    0xCE,       // 0xD7 // PrintItemSemi
    0xCF,       // 0xD8 // PrintNL
    0xD0,       // 0xD9 // PrintObj
    0xD1,       // 0xDA // PrintSemi
    0xD2,       // 0xDB // PrintSpc
    0xD3,       // 0xDC // PrintTab
    0xD4,       // 0xDD // PrintTabComma
    0xD5,       // 0xDE // Pset
    0xD6,       // 0xDF // PutRec
    0xD7,       // 0xE0 // QuoteRem
    0xD8,       // 0xE1 // Redim
    0xD9,       // 0xE2 // RedimAs
    0xDA,       // 0xE3 // Reparse
    0xDB,       // 0xE4 // Rem
    0xDC,       // 0xE5 // Resume
    0xDD,       // 0xE6 // Return
    0xDE,       // 0xE7 // Rset
    0xDF,       // 0xE8 // Scale
    0xE0,       // 0xE9 // Seek
    0xE1,       // 0xEA // SelectCase
    0xE2,       // 0xEB // SelectIs
    0xE3,       // 0xEC // SelectType
    0xE4,       // 0xED // SetStmt
    0xE5,       // 0xEE // Stack
    0xE6,       // 0xEF // Stop
    0xE7,       // 0xF0 // Type
    0xE8,       // 0xF1 // UnLock
    0xE9,       // 0xF2 // VarDefn
    0xEA,       // 0xF3 // Wend
    0xEB,       // 0xF4 // While
    0xEC,       // 0xF5 // With
    0xED,       // 0xF6 // WriteChan
    0xEE,       // 0xF7 // ConstFuncExpr
    0xEF,       // 0xF8 // LbConst
    XL5_OP_MAX, // 0xF9 // *LbIf
    XL5_OP_MAX, // 0xFA // *LbElse
    XL5_OP_MAX, // 0xFB // *LbElseif
    XL5_OP_MAX, // 0xFC // *LbEndif
    XL5_OP_MAX, // 0xFD // *LbMark
    XL5_OP_MAX, // 0xFE // *EndForVariable
    XL5_OP_MAX, // 0xFF // *StartForVariable
    XL5_OP_MAX, // 0x100 // *NewRedim
    XL5_OP_MAX, // 0x101 // *StartWithExpr
    0xA3,       // 0x102 // *SetOrSt
    XL5_OP_MAX, // 0x103 // *EndEnum
    XL5_OP_MAX, // 0x104 // *Illegal
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif


//********************************************************************
//
// Function:
//  BOOL XL5HResolveFilenameParam()
//
// Parameters:
//  lpabyLine           Ptr to line bytes
//  lpanOpOffsets       Array of opcode offsets
//  lpabyOps            Array of XL5 opcodes
//  nNumOps             Number of opcodes
//  lpawAppStrID        Ptr to application string IDs
//  lpstObj             Object containing filename
//
// Description:
//  Iterates through the opcodes to get the filename parameter.
//  The function only understands the following atoms:
//
//      Atom                Description
//      ----                -----------
//      Application         Application object
//      .StartupPath        Default startup pathname
//      .AltStartupPath     Alternative startup pathname
//      .PathSeparator      Separator for pathname components
//      +                   Concatenation operator
//      &                   Concatenation operator
//      "s"                 Literal string
//      identifier          Any identifier
//
//  The Application and identifier atoms may be represented
//  by any of the following:
//
//      0x20 (single ID)
//      0x24 (single ID with zero parameters)
//
//  The .StartupPath, .AltStartupPath, and .PathSeparator atoms
//  may be represented by any of the following:
//
//      0x21 (qualified ID)
//      0x25 (qualified ID with zero parameters)
//      0x35 (root ID)
//      0x37 (root ID with zero parameters)
//
//  The function ignores the parameter count field for opcodes
//  0x24, 0x25, and 0x37.
//
//  The caller must resolve any identifiers.
//
// Returns:
//  TRUE                If a filename was successfully constructed
//  FALSE               On error
//
//********************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXL5Hxlstart[7] = { 'x', 'l', 's', 't', 'a', 'r', 't' };

#ifdef SYM_WIN16
#pragma data_seg()
#endif

#define XL5H_DEF_PATH_SEPARATOR     '\\'

BOOL XL5HResolveFilenameParam
(
    BOOL                bLittleEndian,
    LPBYTE              lpabyLine,
    LPINT               lpanOpOffsets,
    LPBYTE              lpabyOps,
    int                 nNumOps,
    LPWORD              lpawAppStrID,
    LPXL5H_OBJ          lpstObj
)
{
    int                 i;
    int                 nOffset;
    int                 nTopLevelCount;
    LPBYTE              lpabyDst;
    LPBYTE              lpabySrc;
    int                 nOpcode;

    lpstObj->byNumParts = 0;
    lpstObj->byDataUsed = 0;
    nTopLevelCount = 0;
    for (i=0;i<nNumOps && nTopLevelCount < 3;i++)
    {
        nOffset = lpanOpOffsets[i];
        nOpcode = lpabyOps[i];

        switch (nOpcode)
        {
            case XL5_OP_IDENT:          // 0x20
            case XL5_OP_IDENT_DIM:      // 0x24
            {
                if (lpstObj->byNumParts < XL5H_OBJ_MAX_PARTS)
                {
                    // Store the two byte ID

                    lpstObj->abyInfo[lpstObj->byNumParts][0] =
                        lpabyLine[nOffset + 2];
                    lpstObj->abyInfo[lpstObj->byNumParts][1] =
                        lpabyLine[nOffset + 3];

                    // Store the object type

                    lpstObj->abyType[lpstObj->byNumParts++] =
                        XL5H_OBJ_ID;

                    // Increment top level count

                    ++nTopLevelCount;
                }
                break;
            }
            case XL5_OP_X__IDENT:       // 0x21
            case XL5_OP_X__IDENT_DIM:   // 0x25
            case XL5_OP__IDENT:         // 0x35
            case XL5_OP__IDENT_DIM:     // 0x37
            {
                WORD                wID;
                BYTE                byType;

                wID = SSEndianWORD(lpabyLine+nOffset+2,bLittleEndian);

                if (wID == lpawAppStrID[XL5H_VSTR_ID_altstartuppath] ||
                    wID == lpawAppStrID[XL5H_VSTR_ID_startuppath])
                    byType = XL5H_OBJ_STARTUP_PATH;
                else
                if (wID == lpawAppStrID[XL5H_VSTR_ID_pathseparator])
                    byType = XL5H_OBJ_PATH_SEPARATOR;
                else
                    byType = XL5H_OBJ_UNKNOWN;

                // Check to see if the ID must be qualified

                if (nTopLevelCount >= 1 &&
                    (nOpcode == XL5_OP_X__IDENT ||
                     nOpcode == XL5_OP_X__IDENT_DIM))
                {
                    // Only if it is an ID do we delete it

                    if (lpstObj->abyType[lpstObj->byNumParts - 1] ==
                        XL5H_OBJ_ID)
                    {
                        // Delete the qualifying object

                        lpstObj->byNumParts--;

                        // Decrement top level count

                        --nTopLevelCount;
                    }
                }

                // Store only if there is enough room and
                //  the type is either the startup path or the
                //  path separator

                if (lpstObj->byNumParts < XL5H_OBJ_MAX_PARTS &&
                    byType != XL5H_OBJ_UNKNOWN)
                {
                    // Store the location of the string

                    lpstObj->abyInfo[lpstObj->byNumParts][0] =
                        lpstObj->byDataUsed;

                    // Get the pointer to the destination

                    lpabyDst = lpstObj->abyData + lpstObj->byDataUsed;

                    if (byType == XL5H_OBJ_STARTUP_PATH)
                    {
                        if (lpstObj->byDataUsed + sizeof(gabyXL5Hxlstart) <=
                            XL5H_OBJ_MAX_DATA_SIZE)
                        {
                            int     j;

                            // Store the string length

                            lpstObj->abyInfo[lpstObj->byNumParts][1] =
                                sizeof(gabyXL5Hxlstart);

                            // Store the startup path string

                            for (j=0;j<sizeof(gabyXL5Hxlstart);j++)
                                *lpabyDst++ = gabyXL5Hxlstart[j];

                            lpstObj->byDataUsed += sizeof(gabyXL5Hxlstart);

                            lpstObj->abyType[lpstObj->byNumParts++] =
                                XL5H_OBJ_STRING;

                            // Increment top level count

                            ++nTopLevelCount;
                        }
                    }
                    else
                    if (lpstObj->byDataUsed < XL5H_OBJ_MAX_DATA_SIZE)
                    {
                        // Store the string length

                        lpstObj->abyInfo[lpstObj->byNumParts][1] = 1;
                            
                        // Store the path separator

                        *lpabyDst = XL5H_DEF_PATH_SEPARATOR;
                        lpstObj->byDataUsed++;
                        lpstObj->abyType[lpstObj->byNumParts++] =
                            XL5H_OBJ_STRING;

                        // Increment top level count

                        ++nTopLevelCount;
                    }
                }

                break;
            }
            case XL5_OP_LIT_STR:
            {
                int     nLen;
                BYTE    byChar;

                // Store only if:
                //  - There is enough room for this part
                //  - There is enough space for the string

                if (bLittleEndian)
                    nLen = lpabyLine[nOffset + 2];
                else
                    nLen = lpabyLine[nOffset + 3];

                if (lpstObj->byNumParts < XL5H_OBJ_MAX_PARTS &&
                    lpstObj->byDataUsed + nLen <= XL5H_OBJ_MAX_DATA_SIZE)
                {
                    // Store the location of the string

                    lpstObj->abyInfo[lpstObj->byNumParts][0] =
                        lpstObj->byDataUsed;

                    // Store the length of the string

                    lpstObj->abyInfo[lpstObj->byNumParts][1] = (BYTE)nLen;

                    // Get the source and destination pointers

                    lpabyDst = lpstObj->abyData + lpstObj->byDataUsed;
                    lpabySrc = lpabyLine + nOffset + 4;

                    // Increment the data used

                    lpstObj->byDataUsed += nLen;

                    while (nLen-- != 0)
                    {
                        // Lower case the string

                        byChar = SSToLower(*lpabySrc++);

                        // Convert path separators to canonical form

                        if (byChar == '/' || byChar == ':')
                            byChar = '\\';

                        *lpabyDst++ = byChar;
                    }

                    // Store the type

                    lpstObj->abyType[lpstObj->byNumParts++] = XL5H_OBJ_STRING;

                    // Increment top level count

                    ++nTopLevelCount;
                }
                break;
            }
            case XL5_OP_PLUS:
            case XL5_OP_AMPERSAND:
            {
                // Concatenation

                if (lpstObj->byNumParts >= 2)
                {
                    // If both of the previous parts are strings,
                    //  consolidate them into one

                    if (lpstObj->abyType[lpstObj->byNumParts - 2] ==
                        XL5H_OBJ_STRING &&
                        lpstObj->abyType[lpstObj->byNumParts - 1] ==
                        XL5H_OBJ_STRING)
                    {
                        // Decrement the count

                        lpstObj->byNumParts--;

                        // Add the length of the last string to the previous

                        lpstObj->abyInfo[lpstObj->byNumParts - 1][1] +=
                            lpstObj->abyInfo[lpstObj->byNumParts][1];
                    }

                    // Decrement top level count

                    --nTopLevelCount;
                }

                break;
            }
            default:
                // Ignore everything else
                break;
        }
    }

    // If there is no top level object, return false

    if (nTopLevelCount <= 0)
        return(FALSE);

    // If the top level count is not one, remove parts from the end

    while (nTopLevelCount > 1)
    {
        lpstObj->byNumParts--;

        // We only need to be worried about updating the data used
        //  field for string objects

        if (lpstObj->abyType[lpstObj->byNumParts] == XL5H_OBJ_STRING)
            lpstObj->byDataUsed -= lpstObj->abyInfo[lpstObj->byNumParts][1];

        --nTopLevelCount;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int XL5HGetOpcodeInfo()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  lpabyBuf            Ptr to opcode buffer
//  nCount              Buffer bytes left
//  lpnOpcode           Ptr to int for opcode
//
// Description:
//  The function returns in *lpnOpcode the translated opcode
//  and returns the operand byte count.
//
// Returns:
//  >= 0                Number of operand bytes
//  < 0                 On error
//
//********************************************************************

int XL5HGetOpcodeInfo
(
    LPXL5H              lpstEnv,
    LPBYTE              lpabyBuf,
    int                 nCount,
    LPINT               lpnOpcode
)
{
    int                 nOpSkip;
    int                 nOpcode;

    if (nCount < 2)
        return(-1);

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        // Get the operand byte count

        nOpSkip = XL5GetOpSkip(lpabyBuf,nCount);

        // Take the opcode as is

        *lpnOpcode = lpabyBuf[0];
    }
    else
    if (lpstEnv->stMod.st97.nVersion == O97_VERSION_O97)
    {
        // Get the operand byte count

        nOpSkip = O97GetOpSkip(lpstEnv->bLittleEndian,
                               lpabyBuf,
                               nCount);

        // Translate the opcode

        if (lpstEnv->bLittleEndian)
            nOpcode = lpabyBuf[0];
        else
            nOpcode = lpabyBuf[1];

        *lpnOpcode = gabyXL5HOpO97toXL5[nOpcode];
    }
    else
    {
        // Get the operand byte count

        nOpSkip = O98GetOpSkip(lpstEnv->bLittleEndian,
                               lpabyBuf,
                               nCount);

        // Translate the opcode

        if (lpstEnv->bLittleEndian)
            nOpcode = lpabyBuf[0] | ((lpabyBuf[1] & 0x01) << 8);
        else
            nOpcode = lpabyBuf[1] | ((lpabyBuf[0] & 0x01) << 8);

        if (nOpcode > O98_OP_MAX)
            nOpcode = O98_OP_MAX;

        *lpnOpcode = gabyXL5HOpO98toXL5[nOpcode];
    }

    return(nOpSkip);
}


//********************************************************************
//
// Function:
//  BOOL XL5HFillOpInfo()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  wLine               The line number (zero-based)
//
// Description:
//  The function parses the given line.
//  Upon returning, the following fields of lpstEnv will be set:
//
//      nNumOps         The number of opcode indexes stored
//                      in anOpOffsets[]
//      anOpOffsets     The offsets of the opcodes in the given line
//      lpabyLine       The ptr to the line contents
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HFillOpInfo
(
    LPXL5H              lpstEnv,
    WORD                wLine
)
{
    WORD                wLineSize;
    LPBYTE              lpabyBuf;
    WORD                wCount;
    int                 i;
    int                 nOpcode;
    int                 nOpSkip;

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        WORD            wLineOffset;

        if (XL5ModAccGetLineInfo(&lpstEnv->stMod.st95,
                                 wLine,
                                 &wLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = XL5ModAccGetDataPtr(&lpstEnv->stMod.st95,
                                       wLineOffset,
                                       wLineSize,
                                       &wCount);
    }
    else
    {
        DWORD           dwLineOffset;

        if (O97ModAccGetLineInfo(&lpstEnv->stMod.st97,
                                 wLine,
                                 &dwLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = O97ModAccGetDataPtr(&lpstEnv->stMod.st97,
                                       dwLineOffset,
                                       wLineSize,
                                       &wCount);
    }

    if (lpabyBuf == NULL)
        return(FALSE);

    // Get the opcode offsets

    lpstEnv->nNumOps = 0;
    i = 0;
    while (i < wCount && lpstEnv->nNumOps < XL5H_RESOLVE_MAX_OPS)
    {
        nOpSkip = XL5HGetOpcodeInfo(lpstEnv,
                                    lpabyBuf + i,
                                    (int)wCount - i,
                                    &nOpcode);

        // Forget about the last op if there wasn't room in the
        //  buffer for it and its operand bytes

        if (nOpSkip < 0)
            break;

        // Store the opcode

        lpstEnv->anOpOffsets[lpstEnv->nNumOps] = i;
        lpstEnv->abyOps[lpstEnv->nNumOps] = (BYTE)nOpcode;

        // Skip over the operands

        i += 2 + nOpSkip;

        if (i < 0 || i > wCount)
            break;

        lpstEnv->nNumOps++;
    }

    lpstEnv->lpabyLine = lpabyBuf;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HGetLineInfo()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  bCall               TRUE if it is a call line, FALSE for assign
//  wLine               The line number (zero-based)
//  lpawParamIDs        Array of paramater IDs to search for
//  nNumParamIDs        Number of paramater IDs in array
//
// Description:
//  The function parses the given line looking for the last
//  dotted call or dotted assignment on the line.
//
//  Upon returning, the following fields of lpstEnv will be set:
//
//      nLastCallOp     The opcode index of the call or assignment
//      bCallIsRooted   TRUE if it is a root call/assign
//      nParamStart     The opcode index of the first parameter
//                      or parameter matching one of the given IDs
//      nParamEnd       The opcode index one past the last parameter
//                      opcode
//      nNumOps         The number of opcode indexes stored
//                      in anOpOffsets[]
//      anOpOffsets     The offsets of the opcodes in the given line
//      lpabyLine       The ptr to the line contents
//
//  The dotted calls are the following opcodes:
//
//      XL5_OP_CALL_X__IDENT        0x42
//      XL5_OP_CALL__IDENT          0x43
//
//  The dotted assignments are the following opcodes:
//
//      XL5_OP_ASSIGN_X_IDENT       0x28
//      XL5_OP_ASSIGN_X_IDENT_DIM   0x2C
//      XL5_OP_ASSIGN__IDENT        0x39
//      XL5_OP_ASSIGN__IDENT_DIM    0x3B
//
//  The function interprets the colon operator as a statement
//  separator within the line.  If the dotted call/assignment
//  comes after the colon, then the parameter will also start
//  after the colon.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HGetLineInfo
(
    LPXL5H              lpstEnv,
    BOOL                bCall,
    WORD                wLine,
    LPWORD              lpawParamIDs,
    int                 nNumParamIDs
)
{
    WORD                wLineSize;
    LPBYTE              lpabyBuf;
    WORD                wCount;
    int                 i;
    int                 j;
    int                 nOpcode;
    WORD                wID;
    int                 nPrevBOS;
    int                 nCurBOS;
    int                 nOpSkip;

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        WORD            wLineOffset;

        if (XL5ModAccGetLineInfo(&lpstEnv->stMod.st95,
                                 wLine,
                                 &wLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = XL5ModAccGetDataPtr(&lpstEnv->stMod.st95,
                                       wLineOffset,
                                       wLineSize,
                                       &wCount);
    }
    else
    {
        DWORD           dwLineOffset;

        if (O97ModAccGetLineInfo(&lpstEnv->stMod.st97,
                                 wLine,
                                 &dwLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = O97ModAccGetDataPtr(&lpstEnv->stMod.st97,
                                       dwLineOffset,
                                       wLineSize,
                                       &wCount);
    }

    if (lpabyBuf == NULL)
        return(FALSE);

    // Get the opcode offsets

    lpstEnv->nParamStart = 0;
    lpstEnv->nParamEnd = 0;
    lpstEnv->nNumOps = 0;
    lpstEnv->nLastCallOp = 0;
    nPrevBOS = 0;
    nCurBOS = 0;
    i = 0;
    while (i < wCount && lpstEnv->nNumOps < XL5H_RESOLVE_MAX_OPS)
    {
        nOpSkip = XL5HGetOpcodeInfo(lpstEnv,
                                    lpabyBuf + i,
                                    (int)wCount - i,
                                    &nOpcode);

        // Forget about the last op if there wasn't room in the
        //  buffer for it and its operand bytes

        if (nOpSkip < 0)
            break;

        // Store the opcode

        lpstEnv->anOpOffsets[lpstEnv->nNumOps] = i;
        lpstEnv->abyOps[lpstEnv->nNumOps] = (BYTE)nOpcode;

        // Check for the named parameter for which we are searching

        if (nNumParamIDs != 0 && nOpcode == XL5_OP_ASSIGN_NAMED_PARAM)
        {
            if (lpstEnv->nParamEnd == 0)
            {
                // Haven't yet found the parameter

                // Check the ID

                wID = SSEndianWORD(lpabyBuf+i+2,lpstEnv->bLittleEndian);

                for (j=0;j<nNumParamIDs;j++)
                {
                    if (wID == lpawParamIDs[j])
                    {
                        // Found it

                        lpstEnv->nParamEnd = lpstEnv->nNumOps;
                        break;
                    }
                }

                if (lpstEnv->nParamEnd == 0)
                {
                    // Set the possible param start index

                    lpstEnv->nParamStart = lpstEnv->nNumOps + 1;
                }
            }
        }

        // Check for an assignment call

        switch (nOpcode)
        {
            case XL5_OP_CALL_X__IDENT:
                if (bCall != FALSE)
                {
                    lpstEnv->bCallIsRooted = FALSE;
                    lpstEnv->nLastCallOp = lpstEnv->nNumOps;
                    nCurBOS = nPrevBOS;
                }
                break;


            case XL5_OP_CALL__IDENT:
                if (bCall != FALSE)
                {
                    lpstEnv->bCallIsRooted = TRUE;
                    lpstEnv->nLastCallOp = lpstEnv->nNumOps;
                    nCurBOS = nPrevBOS;
                }
                break;

            case XL5_OP_ASSIGN_X_IDENT:
            case XL5_OP_ASSIGN_X_IDENT_DIM:
                if (bCall == FALSE)
                {
                    lpstEnv->bCallIsRooted = FALSE;
                    lpstEnv->nLastCallOp = lpstEnv->nNumOps;
                    nCurBOS = nPrevBOS;
                }
                break;

            case XL5_OP_ASSIGN__IDENT:
            case XL5_OP_ASSIGN__IDENT_DIM:
                if (bCall == FALSE)
                {
                    lpstEnv->bCallIsRooted = TRUE;
                    lpstEnv->nLastCallOp = lpstEnv->nNumOps;
                    nCurBOS = nPrevBOS;
                }
                break;

            default:
                break;
        }

        // Skip over the operands

        i += 2 + nOpSkip;

        if (i < 0 || i > wCount)
            break;

        lpstEnv->nNumOps++;

        if (nOpcode == XL5_OP_BOS_COLON)
            nPrevBOS = lpstEnv->nNumOps;
    }

    if (lpstEnv->nParamEnd == 0)
        lpstEnv->nParamEnd = lpstEnv->nLastCallOp;

    if (lpstEnv->nParamStart < nCurBOS)
        lpstEnv->nParamStart = nCurBOS;

    lpstEnv->lpabyLine = lpabyBuf;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HFindDottedCallAssign()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  nType               Type of call/assign to search for
//  nMinParamCount      Minimum number of parameters
//  lpstState           Ptr to state
//  lpawIDList          Ptr to list of call/assign IDs
//  nIDListLen          Number of IDs
//  lpbFound            Ptr to bool variable for found status
//
// Description:
//  Finds a dotted call, dotted assignment, or normal sub/func
//  call.
//
// Returns:
//  TRUE                On success
//  FALSE               Otherwise
//
//********************************************************************

#define XL5H_FIND_DOTTED_CALL       0
#define XL5H_FIND_DOTTED_ASSIGN     1
#define XL5H_FIND_NORM_SUBFUNC_CALL 2

BOOL XL5HFindDottedCallAssign
(
    LPXL5H              lpstEnv,
    int                 nType,
    int                 nMinParamCount,
    LPXL5H_SCAN_STATE   lpstState,
    LPWORD              lpawIDList,
    int                 nIDListLen,
    LPBOOL              lpbFound
)
{
    DWORD               dwLine;
    DWORD               dwLineCount;
    WORD                wLineSize;
    LPBYTE              lpabyBuf;
    WORD                wCount;
    WORD                wTemp;
    int                 i;
    int                 j;
    int                 nOpSkip;
    int                 nOpcode;

    *lpbFound = FALSE;
    if (lpstEnv->nType == XL5H_TYPE_95)
        dwLineCount = lpstEnv->stMod.st95.dwTableSize;
    else
        dwLineCount = lpstEnv->stMod.st97.dwTableSize;

    for (dwLine=lpstState->wNextScanLine;dwLine<dwLineCount;dwLine++)
    {
        // Get the line

        if (lpstEnv->nType == XL5H_TYPE_95)
        {
            WORD        wLineOffset;

            if (XL5ModAccGetLineInfo(&lpstEnv->stMod.st95,
                                     dwLine,
                                     &wLineOffset,
                                     &wLineSize) == FALSE)
            {
                // Just go to the next line

                continue;
            }

            lpabyBuf = XL5ModAccGetDataPtr(&lpstEnv->stMod.st95,
                                           wLineOffset,
                                           wLineSize,
                                           &wCount);
        }
        else
        {
            DWORD       dwLineOffset;

            if (O97ModAccGetLineInfo(&lpstEnv->stMod.st97,
                                     dwLine,
                                     &dwLineOffset,
                                     &wLineSize) == FALSE)
            {
                // Just go to the next line

                continue;
            }

            lpabyBuf = O97ModAccGetDataPtr(&lpstEnv->stMod.st97,
                                           dwLineOffset,
                                           wLineSize,
                                           &wCount);
        }

        if (lpabyBuf == NULL)
            continue;

        // Iterate through the line

        i = 0;
        while (i < wCount)
        {
            nOpSkip = XL5HGetOpcodeInfo(lpstEnv,
                                        lpabyBuf + i,
                                        (int)wCount - i,
                                        &nOpcode);

            if (nOpSkip < 0)
                break;

            i += 2;

            switch (nOpcode)
            {
                // Check for the beginning of a sub/func

                case XL5_OP_FUNC_DEFN:
                    lpstState->wFuncBeginLine = (WORD)dwLine;
                    lpstState->nAssignCount = 0;
                    break;

                case XL5_OP_CALL_X__IDENT:
                case XL5_OP_CALL__IDENT:
                    if (nType == XL5H_FIND_DOTTED_CALL)
                    {
                        if (i + 3 <= wCount &&
                            SSEndianWORD(lpabyBuf + i + 2,
                                         lpstEnv->bLittleEndian) >= nMinParamCount)
                        {
                            // Check the ID against call list

                            wTemp = SSEndianWORD(lpabyBuf+i,
                                                 lpstEnv->bLittleEndian);

                            for (j=0;j<nIDListLen;j++)
                            {
                                if (wTemp == lpawIDList[j])
                                {
                                    // Found one

                                    lpstState->wNextScanLine =
                                        (WORD)(dwLine + 1);
                                    *lpbFound = TRUE;
                                    return(TRUE);
                                }
                            }
                        }
                    }
                    break;

                case XL5_OP_ASSIGN_X_IDENT:
                case XL5_OP_ASSIGN_X_IDENT_DIM:
                case XL5_OP_ASSIGN__IDENT:
                case XL5_OP_ASSIGN__IDENT_DIM:
                    if (nType == XL5H_FIND_DOTTED_ASSIGN)
                    {
                        // Check the ID against call list

                        wTemp = SSEndianWORD(lpabyBuf+i,
                                             lpstEnv->bLittleEndian);

                        for (j=0;j<nIDListLen;j++)
                        {
                            if (wTemp == lpawIDList[j])
                            {
                                // Found one

                                lpstState->wNextScanLine = (WORD)(dwLine + 1);
                                *lpbFound = TRUE;
                                return(TRUE);
                            }
                        }
                    }
                    break;

                case XL5_OP_IDENT_DIM:
                case XL5_OP_CALL_IDENT_DIM:
                    if (nType == XL5H_FIND_NORM_SUBFUNC_CALL)
                    {
                        if (i + 3 <= wCount &&
                            SSEndianWORD(lpabyBuf + i + 2,
                                         lpstEnv->bLittleEndian) >= nMinParamCount)
                        {
                            // Check the ID against call list

                            wTemp = SSEndianWORD(lpabyBuf+i,
                                                 lpstEnv->bLittleEndian);

                            for (j=0;j<nIDListLen;j++)
                            {
                                if (wTemp == lpawIDList[j])
                                {
                                    // Found one

                                    lpstState->wNextScanLine = (WORD)(dwLine + 1);
                                    *lpbFound = TRUE;
                                    return(TRUE);
                                }
                            }
                        }
                    }
                    break;

                case XL5_OP_ASSIGN_IDENT:
                case XL5_OP_SET_IDENT:
                    // Is it a simple assignment or a Set

                    if (i + 2 <= wCount)
                    {
                        // Save it

                        if (lpstState->nAssignHead >= XL5H_MAX_ASSIGNS)
                            lpstState->nAssignHead = 0;

                        lpstState->awAssignLines[lpstState->nAssignHead] =
                            (WORD)dwLine;

                        lpstState->
                            abyAssignIDs[lpstState->nAssignHead][0] =
                                lpabyBuf[i];

                        lpstState->
                            abyAssignIDs[lpstState->nAssignHead][1] =
                                lpabyBuf[i+1];

                        lpstState->nAssignHead++;
                        lpstState->nAssignCount++;
                    }
                    break;

                case XL5_OP_WITH:
                    // Save the With line

                    if (lpstState->nWithHead >= XL5H_MAX_WITHS)
                        lpstState->nWithHead = 0;

                    lpstState->
                        awWithLines[lpstState->nWithHead] =
                            (WORD)dwLine;

                    lpstState->
                        awEndWithLines[lpstState->nWithHead] =
                            0xFFFF;

                    lpstState->nWithHead++;
                    lpstState->nWithCount++;
                    break;

                case XL5_OP_END_WITH:
                {
                    int         j;
                    int         nWithI;

                    // Find the most recent open With and close it

                    nWithI = lpstState->nWithHead;
                    for (j=0;j<lpstState->nWithCount;j++)
                    {
                        if (--nWithI < 0)
                            nWithI = XL5H_MAX_WITHS - 1;

                        if (lpstState->
                                awEndWithLines[nWithI] == 0xFFFF)
                        {
                            lpstState->awEndWithLines[nWithI] =
                                (WORD)dwLine;

                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }

            i += nOpSkip;

            if (i < 0)
                break;
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveWithLine()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  wLine               The line number (zero-based)
//
// Description:
//  The function parses the given line looking for the With
//  opcode.
//
//  Upon returning, the following fields of lpstEnv will be set:
//
//      nParamStart     The opcode index of the With parameter
//      nParamEnd       The opcode index of the With opcode
//      nNumOps         The number of opcode indexes stored
//                      in anOpOffsets[]
//      anOpOffsets     The offsets of the opcodes in the given line
//      lpabyLine       The ptr to the line contents
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HResolveWithLine
(
    LPXL5H              lpstEnv,
    WORD                wLine
)
{
    WORD                wLineSize;
    LPBYTE              lpabyBuf;
    WORD                wCount;
    int                 i;
    int                 nOpcode;
    int                 nOpSkip;

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        WORD            wLineOffset;

        if (XL5ModAccGetLineInfo(&lpstEnv->stMod.st95,
                                 wLine,
                                 &wLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = XL5ModAccGetDataPtr(&lpstEnv->stMod.st95,
                                       wLineOffset,
                                       wLineSize,
                                       &wCount);
    }
    else
    {
        DWORD           dwLineOffset;

        if (O97ModAccGetLineInfo(&lpstEnv->stMod.st97,
                                 wLine,
                                 &dwLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = O97ModAccGetDataPtr(&lpstEnv->stMod.st97,
                                       dwLineOffset,
                                       wLineSize,
                                       &wCount);
    }

    if (lpabyBuf == NULL)
        return(FALSE);

    // Get the opcode offsets

    lpstEnv->nParamStart = 0;
    lpstEnv->nParamEnd = 0;
    lpstEnv->nNumOps = 0;
    i = 0;
    while (i < wCount && lpstEnv->nNumOps < XL5H_RESOLVE_MAX_OPS)
    {
        nOpSkip = XL5HGetOpcodeInfo(lpstEnv,
                                    lpabyBuf + i,
                                    (int)wCount - i,
                                    &nOpcode);

        // Forget about the last op if there wasn't room in the
        //  buffer for it and its operand bytes

        if (nOpSkip < 0)
            break;

        // Store the opcode

        lpstEnv->anOpOffsets[lpstEnv->nNumOps] = i;
        lpstEnv->abyOps[lpstEnv->nNumOps] = (BYTE)nOpcode;

        // Is it the With?

        if (nOpcode == XL5_OP_WITH)
        {
            lpstEnv->nParamEnd = lpstEnv->nNumOps;
            break;
        }

        // Skip over the operands

        i += 2 + nOpSkip;

        if (i < 0 || i > wCount)
            break;

        lpstEnv->nNumOps++;
    }

    if (lpstEnv->nParamEnd == 0)
    {
        // Didn't find a With

        lpstEnv->nParamEnd = lpstEnv->nNumOps;
    }

    lpstEnv->lpabyLine = lpabyBuf;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveAssignLine()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  wLine               The line number (zero-based)
//  lpabyID             The ID of the assignment to search for
//
// Description:
//  The function fills in the opcode offsets up until the end
//  or a 0x27 (XL5_OP_ASSIGN_IDENT) or a 0x2E (XL5_OP_ASSIGN_IDENT)
//  is seen of the given ID.
//
//  Upon returning, lpstEnv->nParamStart and lpstEnv->nParamEnd
//  will contain the start and end opcode indexes to search
//  within lpstEnv->anOpOffsets[].  lpstEnv->lpabyLine will
//  contain the contents of the line.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HResolveAssignLine
(
    LPXL5H              lpstEnv,
    WORD                wLine,
    LPBYTE              lpabyID
)
{
    WORD                wLineSize;
    LPBYTE              lpabyBuf;
    WORD                wCount;
    int                 i;
    int                 nOpcode;
    int                 nOpSkip;

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        WORD            wLineOffset;

        if (XL5ModAccGetLineInfo(&lpstEnv->stMod.st95,
                                 wLine,
                                 &wLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = XL5ModAccGetDataPtr(&lpstEnv->stMod.st95,
                                       wLineOffset,
                                       wLineSize,
                                       &wCount);
    }
    else
    {
        DWORD           dwLineOffset;

        if (O97ModAccGetLineInfo(&lpstEnv->stMod.st97,
                                 wLine,
                                 &dwLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = O97ModAccGetDataPtr(&lpstEnv->stMod.st97,
                                       dwLineOffset,
                                       wLineSize,
                                       &wCount);
    }

    if (lpabyBuf == NULL)
        return(FALSE);

    // Get the opcode offsets

    lpstEnv->nParamStart = 0;
    lpstEnv->nParamEnd = 0;
    lpstEnv->nNumOps = 0;
    i = 0;
    while (i < wCount && lpstEnv->nNumOps < XL5H_RESOLVE_MAX_OPS)
    {
        nOpSkip = XL5HGetOpcodeInfo(lpstEnv,
                                    lpabyBuf + i,
                                    (int)wCount - i,
                                    &nOpcode);

        // Forget about the last op if there wasn't room in the
        //  buffer for it and its operand bytes

        if (nOpSkip < 0)
            break;

        // Store the opcode

        lpstEnv->anOpOffsets[lpstEnv->nNumOps] = i;
        lpstEnv->abyOps[lpstEnv->nNumOps] = (BYTE)nOpcode;

        // Check for the assignment ID for which we are searching

        if (nOpcode == XL5_OP_ASSIGN_IDENT ||
            nOpcode == XL5_OP_SET_IDENT)
        {
            // Check the ID

            if (lpabyBuf[i+2] == lpabyID[0] &&
                lpabyBuf[i+3] == lpabyID[1])
            {
                // Found it

                lpstEnv->nParamEnd = lpstEnv->nNumOps;
                break;
            }

            // Set the possible param start index

            lpstEnv->nParamStart = lpstEnv->nNumOps + 1;
        }

        // Skip over the operands

        i += 2 + nOpSkip;

        if (i < 0 || i > wCount)
            break;

        lpstEnv->nNumOps++;
    }

    if (lpstEnv->nParamEnd == 0)
    {
        // Didn't find any assignments

        lpstEnv->nParamEnd = lpstEnv->nNumOps;
    }

    lpstEnv->lpabyLine = lpabyBuf;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveFilenameAssign()
//
// Parameters:
//  bLittleEndian       TRUE if the data is in little endian format
//  lpabyLine           Ptr to line contents
//  lpanOpOffsets       Ptr to opcode offsets
//  lpabyOps            Ptr to opcodes
//  nNumOps             Number of opcode offsets
//  lpawAppStrID        Application string ID array
//  lpstObj             Ptr to object to store filename object
//
// Description:
//  The function resolves the given line as a filename object.
//  The function returns in lpstObj the top-level object
//  after resolving concatenations by '&' and '+'.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HResolveFilenameAssign
(
    BOOL                bLittleEndian,
    LPBYTE              lpabyLine,
    LPINT               lpnOpOffsets,
    LPBYTE              lpabyOps,
    int                 nNumOps,
    LPWORD              lpawAppStrID,
    LPXL5H_OBJ          lpstObj
)
{
    int                 i;
    int                 nOffset;
    int                 nTopLevelCount;
    int                 nStartPart;
    int                 nPrevPart;
    LPBYTE              lpabyDst;
    LPBYTE              lpabySrc;
    int                 nOpcode;

    lpstObj->byNumParts = 0;
    lpstObj->byDataUsed = 0;
    nStartPart = 0;
    nPrevPart = 0;
    nTopLevelCount = 0;
    for (i=0;i<nNumOps;i++)
    {
        nOffset = lpnOpOffsets[i];
        if (bLittleEndian)
            nOpcode = lpabyOps[i];
        else
            nOpcode = lpabyOps[i+1];

        switch (nOpcode)
        {
            case XL5_OP_IDENT:          // 0x20
            case XL5_OP_IDENT_DIM:      // 0x24
            {
                if (lpstObj->byNumParts < XL5H_OBJ_MAX_PARTS)
                {
                    if (lpstObj->byDataUsed + 2 <= XL5H_OBJ_MAX_DATA_SIZE)
                    {
                        // Store the two byte ID

                        lpstObj->abyInfo[lpstObj->byNumParts][0] =
                            lpabyLine[nOffset + 2];
                        lpstObj->abyInfo[lpstObj->byNumParts][1] =
                            lpabyLine[nOffset + 3];

                        // Store the object type

                        lpstObj->abyType[lpstObj->byNumParts++] =
                            XL5H_OBJ_ID;

                        // Increment top level count

                        ++nTopLevelCount;
                    }
                }
                break;
            }
            case XL5_OP_X__IDENT:       // 0x21
            case XL5_OP_X__IDENT_DIM:   // 0x25
            case XL5_OP__IDENT:         // 0x35
            case XL5_OP__IDENT_DIM:     // 0x37
            {
                WORD                wID;
                BYTE                byType;

                wID = SSEndianWORD(lpabyLine+nOffset+2,bLittleEndian);

                if (wID == lpawAppStrID[XL5H_VSTR_ID_altstartuppath] ||
                    wID == lpawAppStrID[XL5H_VSTR_ID_startuppath])
                    byType = XL5H_OBJ_STARTUP_PATH;
                else
                if (wID == lpawAppStrID[XL5H_VSTR_ID_pathseparator])
                    byType = XL5H_OBJ_PATH_SEPARATOR;
                else
                    byType = XL5H_OBJ_UNKNOWN;

                // Check to see if the ID must be qualified

                if (lpstObj->byNumParts != 0 &&
                    (lpabyLine[nOffset] == XL5_OP_X__IDENT ||
                     lpabyLine[nOffset] == XL5_OP_X__IDENT_DIM))
                {
                    // Only if it is an ID do we delete it

                    if (lpstObj->abyType[lpstObj->byNumParts - 1] ==
                        XL5H_OBJ_ID)
                    {
                        // Delete the qualifying object

                        lpstObj->byNumParts--;

                        // Decrement top level count

                        --nTopLevelCount;
                    }
                }

                // Store only if there is enough room and
                //  the type is either the startup path or the
                //  path separator

                if (lpstObj->byNumParts < XL5H_OBJ_MAX_PARTS &&
                    byType != XL5H_OBJ_UNKNOWN)
                {
                    // Store the location of the string

                    lpstObj->abyInfo[lpstObj->byNumParts][0] =
                        lpstObj->byDataUsed;

                    // Get the pointer to the destination

                    lpabyDst = lpstObj->abyData + lpstObj->byDataUsed;

                    if (byType == XL5H_OBJ_STARTUP_PATH)
                    {
                        if (lpstObj->byDataUsed + sizeof(gabyXL5Hxlstart) <=
                            XL5H_OBJ_MAX_DATA_SIZE)
                        {
                            int     j;

                            // Store the string length

                            lpstObj->abyInfo[lpstObj->byNumParts][1] =
                                sizeof(gabyXL5Hxlstart);

                            // Store the startup path string

                            for (j=0;j<sizeof(gabyXL5Hxlstart);j++)
                                *lpabyDst++ = gabyXL5Hxlstart[j];

                            lpstObj->byDataUsed += sizeof(gabyXL5Hxlstart);

                            lpstObj->abyType[lpstObj->byNumParts++] =
                                XL5H_OBJ_STRING;

                            // Increment top level count

                            ++nTopLevelCount;
                        }
                    }
                    else
                    if (lpstObj->byDataUsed < XL5H_OBJ_MAX_DATA_SIZE)
                    {
                        // Store the string length

                        lpstObj->abyInfo[lpstObj->byNumParts][1] = 1;
                            
                        // Store the path separator

                        *lpabyDst = XL5H_DEF_PATH_SEPARATOR;
                        lpstObj->byDataUsed++;
                        lpstObj->abyType[lpstObj->byNumParts++] =
                            XL5H_OBJ_STRING;

                        // Increment top level count

                        ++nTopLevelCount;
                    }
                }

                break;
            }
            case XL5_OP_LIT_STR:
            {
                int     nLen;
                BYTE    byChar;

                // Store only if:
                //  - There is enough room for this part
                //  - There is enough space for the string

                if (bLittleEndian)
                    nLen = lpabyLine[nOffset + 2];
                else
                    nLen = lpabyLine[nOffset + 3];

                if (lpstObj->byNumParts < XL5H_OBJ_MAX_PARTS &&
                    lpstObj->byDataUsed + nLen <= XL5H_OBJ_MAX_DATA_SIZE)
                {
                    // Store the location of the string

                    lpstObj->abyInfo[lpstObj->byNumParts][0] =
                        lpstObj->byDataUsed;

                    // Store the length of the string

                    lpstObj->abyInfo[lpstObj->byNumParts][1] = (BYTE)nLen;

                    // Get the source and destination pointers

                    lpabyDst = lpstObj->abyData + lpstObj->byDataUsed;
                    lpabySrc = lpabyLine + nOffset + 4;

                    // Increment the data used

                    lpstObj->byDataUsed += nLen;

                    while (nLen-- != 0)
                    {
                        // Lower case the string

                        byChar = SSToLower(*lpabySrc++);

                        // Convert path separators to canonical form

                        if (byChar == '/' || byChar == ':')
                            byChar = '\\';

                        *lpabyDst++ = byChar;
                    }

                    // Store the type

                    lpstObj->abyType[lpstObj->byNumParts++] = XL5H_OBJ_STRING;

                    // Increment top level count

                    ++nTopLevelCount;
                }
                break;
            }
            case XL5_OP_PLUS:
            case XL5_OP_AMPERSAND:
            {
                // Concatenation

                if (lpstObj->byNumParts >= 2)
                {
                    // If both of the previous parts are strings,
                    //  consolidate them into one

                    if (lpstObj->abyType[lpstObj->byNumParts - 2] ==
                        XL5H_OBJ_STRING &&
                        lpstObj->abyType[lpstObj->byNumParts - 1] ==
                        XL5H_OBJ_STRING)
                    {
                        // Decrement the count

                        lpstObj->byNumParts--;

                        // Add the length of the last string to the previous

                        lpstObj->abyInfo[lpstObj->byNumParts - 1][1] +=
                            lpstObj->abyInfo[lpstObj->byNumParts][1];
                    }

                    // Decrement top level count

                    --nTopLevelCount;
                }

                break;
            }
            default:
                // Ignore everything else
                break;
        }

        if (lpstObj->byNumParts != 0)
        {
            if (nTopLevelCount >= 3)
            {
                // Need to resync

                nStartPart = nPrevPart;
                --nTopLevelCount;
            }
            nPrevPart = lpstObj->byNumParts - 1;
        }
    }

    // If there is no top level object, return false

    if (nTopLevelCount <= 0)
        return(FALSE);

    // Move the parts to the front

    if (nStartPart != 0)
    {
        for (i=nStartPart;i<lpstObj->byNumParts;i++)
        {
            lpstObj->abyType[i - lpstObj->byNumParts] =
                lpstObj->abyType[i];
            lpstObj->abyInfo[i - lpstObj->byNumParts][0] =
                lpstObj->abyInfo[i][0];
            lpstObj->abyInfo[i - lpstObj->byNumParts][1] =
                lpstObj->abyInfo[i][1];
        }

        lpstObj->byNumParts -= nStartPart;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HGetNormFuncSubCallInfo()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  wLine               The line number (zero-based)
//  wID                 The ID of the func/sub call to search for
//
// Description:
//  The function fills in the opcode offsets up until the end
//  or a 0x24 (XL5_OP_IDENT_DIM) or a 0x41 (XL5_OP_CALL_IDENT_DIM)
//  is seen of the given ID.
//
//  Upon returning, the opcode offsets will be in
//  lpstEnv->anOpOffsets[].  lpstEnv->lpabyLine will
//  contain the contents of the line.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HGetNormFuncSubCallInfo
(
    LPXL5H              lpstEnv,
    WORD                wLine,
    WORD                wID
)
{
    WORD                wLineSize;
    LPBYTE              lpabyBuf;
    WORD                wCount;
    int                 i;
    int                 nOpcode;
    int                 nOpSkip;

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        WORD            wLineOffset;

        if (XL5ModAccGetLineInfo(&lpstEnv->stMod.st95,
                                 wLine,
                                 &wLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = XL5ModAccGetDataPtr(&lpstEnv->stMod.st95,
                                       wLineOffset,
                                       wLineSize,
                                       &wCount);
    }
    else
    {
        DWORD           dwLineOffset;

        if (O97ModAccGetLineInfo(&lpstEnv->stMod.st97,
                                 wLine,
                                 &dwLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        lpabyBuf = O97ModAccGetDataPtr(&lpstEnv->stMod.st97,
                                       dwLineOffset,
                                       wLineSize,
                                       &wCount);
    }

    if (lpabyBuf == NULL)
        return(FALSE);

    // Get the opcode offsets

    lpstEnv->nNumOps = 0;
    i = 0;
    while (i < wCount && lpstEnv->nNumOps < XL5H_RESOLVE_MAX_OPS)
    {
        nOpSkip = XL5HGetOpcodeInfo(lpstEnv,
                                    lpabyBuf + i,
                                    (int)wCount - i,
                                    &nOpcode);

        // Forget about the last op if there wasn't room in the
        //  buffer for it and its operand bytes

        if (nOpSkip < 0)
            break;

        // Store the opcode

        lpstEnv->anOpOffsets[lpstEnv->nNumOps] = i;
        lpstEnv->abyOps[lpstEnv->nNumOps] = (BYTE)nOpcode;

        // Check for the call ID for which we are searching

        if (nOpcode == XL5_OP_IDENT_DIM ||
            nOpcode == XL5_OP_CALL_IDENT_DIM)
        {
            // Check the ID

            if (SSEndianWORD(lpabyBuf + i + 2,lpstEnv->bLittleEndian) == wID)
            {
                // Found it

                lpstEnv->lpabyLine = lpabyBuf;
                return(TRUE);
            }
        }

        // Skip over the operands

        i += 2 + nOpSkip;

        if (i < 0 || i > wCount)
            break;

        if (nOpcode == O97_OP_BOS_COLON)
            lpstEnv->nNumOps = 0;
        else
            lpstEnv->nNumOps++;
    }

    // Couldn't find it

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HGetParamInfo()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  wFuncLine           Function declaration line
//  lpabyParamID        The parameter ID to search for
//  lpabyFuncID         Buffer for function ID if parameter is found
//  lpnParamIndex       Ptr to int for index of the parameter
//
// Description:
//  The function checks the function declaration line to determine
//  whether there is a parameter with the given ID.  If there is,
//  the function returns at lpabyFuncID the ID of the function
//  and returns at lpnParamIndex the index of the matched parameter.
//
//  If the parameter cannot be found *lpnParamIndex is set to -1.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HGetParamInfo
(
    LPXL5H              lpstEnv,
    WORD                wFuncLine,
    LPBYTE              lpabyParamID,
    LPWORD              lpwFuncID,
    LPINT               lpnParamIndex
)
{
    int                 i;
    DWORD               dwByteCount;

    // Initialize to no found

    *lpnParamIndex = -1;

    if (XL5HFillOpInfo(lpstEnv,wFuncLine) == FALSE)
        return(FALSE);

    // Find the function definition opcode

    for (i=0;i<lpstEnv->nNumOps;i++)
    {
        if (lpstEnv->abyOps[i] == XL5_OP_FUNC_DEFN)
            break;
    }

    if (i == lpstEnv->nNumOps)
        return(FALSE);

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        DWORD                   dwDeclareTableOffset;
        DWORD                   dwDeclareTableSize;
        DWORD                   dwTypeTableOffset;
        DWORD                   dwTypeTableSize;
        WORD                    wTypeInfoOffset;
        XL5_TYPE_INFO_T         stType;
        XL5_PARAM_TYPE_INFO_T   stParam;
        int                     nParamCount;
        WORD                    wID;

        // Now get the type info

        if (XL5GetTypeTableInfo(lpstEnv->lpstModStream,
                                &dwDeclareTableOffset,
                                &dwDeclareTableSize,
                                &dwTypeTableOffset,
                                &dwTypeTableSize) == FALSE)
            return(FALSE);

        // Get the offset of the type info

        wTypeInfoOffset =
            SSEndianWORD(lpstEnv->lpabyLine +
                             lpstEnv->anOpOffsets[i] + 2,
                         TRUE);

        // Read the type info

        if (SSSeekRead(lpstEnv->lpstModStream,
                       dwTypeTableOffset + wTypeInfoOffset,
                       &stType,
                       sizeof(XL5_TYPE_INFO_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL5_TYPE_INFO_T))
            return(FALSE);

        stType.wParamCounts = WENDIAN(stType.wParamCounts);
        wTypeInfoOffset = WENDIAN(stType.wFirstParam);
        nParamCount = (stType.wParamCounts >> 3) & 0x3F;

        // Search up to 8 parameters

        if (nParamCount > 8)
            nParamCount = 8;

        wID = SSEndianWORD(lpabyParamID,TRUE);
        for (i=0;i<nParamCount && wTypeInfoOffset != 0xFFFF;i++)
        {
            if (SSSeekRead(lpstEnv->lpstModStream,
                           dwTypeTableOffset + wTypeInfoOffset,
                           &stParam,
                           sizeof(XL5_PARAM_TYPE_INFO_T),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(XL5_PARAM_TYPE_INFO_T))
                return(FALSE);

            // Check the ID

            stParam.wID = WENDIAN(stParam.wID);
            if (stParam.wID == wID)
            {
                // Found it

                *lpwFuncID = WENDIAN(stType.wID);
                *lpnParamIndex = i;
                return(TRUE);
            }

            wTypeInfoOffset = WENDIAN(stParam.wNextParam);
        }
    }
    else
    {
        DWORD                   dwTypeTableOffset;
        DWORD                   dwTypeTableSize;
        DWORD                   dwTypeInfoOffset;
        BYTE                    byParamCount;
        BYTE                    abyParamID[2];
        BYTE                byParamCountOffset;
        BYTE                byFirstParamOffset;

        if (lpstEnv->stMod.st97.nVersion == O97_VERSION_O97)
        {
            byParamCountOffset = 0x37;
            byFirstParamOffset = 0x24;
        }
        else
        {
            byParamCountOffset = 0x3B;
            byFirstParamOffset = 0x28;
        }

        // Now get the type info

        if (O97GetTypeTableOffset(lpstEnv->lpstModStream,
                                  &dwTypeTableOffset,
                                  &dwTypeTableSize) == FALSE)
            return(FALSE);

        // Get the offset of the type info

        dwTypeInfoOffset =
            SSEndianDWORD(lpstEnv->lpabyLine +
                              lpstEnv->anOpOffsets[i] + 2,
                          lpstEnv->bLittleEndian);

        // Read the function ID

        if (SSSeekRead(lpstEnv->lpstModStream,
                       dwTypeTableOffset + dwTypeInfoOffset + 2,
                       lpwFuncID,
                       sizeof(WORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD))
            return(FALSE);


        *lpwFuncID = SSEndianWORD((LPBYTE)lpwFuncID,
                                  lpstEnv->bLittleEndian);

        // Read the parameter count

        if (SSSeekRead(lpstEnv->lpstModStream,
                       dwTypeTableOffset + dwTypeInfoOffset +
                           byParamCountOffset,
                       &byParamCount,
                       sizeof(BYTE),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(BYTE))
            return(FALSE);

        // Return error if there are no parameters

        if (byParamCount == 0)
            return(FALSE);

        // Get the first parameter offset

        if (SSSeekRead(lpstEnv->lpstModStream,
                       dwTypeTableOffset + dwTypeInfoOffset +
                           byFirstParamOffset,
                       &dwTypeInfoOffset,
                       sizeof(DWORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(DWORD))
            return(FALSE);

        dwTypeInfoOffset = SSEndianDWORD((LPBYTE)&dwTypeInfoOffset,
                                         lpstEnv->bLittleEndian);

        // Search up to 8 parameters

        if (byParamCount > 8)
            byParamCount = 8;

        for (i=0;i<byParamCount && dwTypeInfoOffset != 0xFFFFFFFF;i++)
        {
            // Get the parameter ID

            if (SSSeekRead(lpstEnv->lpstModStream,
                           dwTypeTableOffset + dwTypeInfoOffset + 2,
                           abyParamID,
                           2,
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != 2)
                return(FALSE);

            // Check the ID

            if (abyParamID[0] == lpabyParamID[0] &&
                abyParamID[1] == lpabyParamID[1])
            {
                // Found it

                *lpnParamIndex = i;
                return(TRUE);
            }

            // Get the next parameter offset

            if (SSSeekRead(lpstEnv->lpstModStream,
                           dwTypeTableOffset + dwTypeInfoOffset + 0x14,
                           &dwTypeInfoOffset,
                           sizeof(DWORD),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(DWORD))
                return(FALSE);

            dwTypeInfoOffset = SSEndianDWORD((LPBYTE)&dwTypeInfoOffset,
                                             lpstEnv->bLittleEndian);
        }
    }

    return(TRUE);
}



//********************************************************************
//
// Function:
//  BOOL XL5HGetParamLimits()
//
// Parameters:
//  nParamIndex         Index of parameter limits to get
//  lpabyOps            Ptr to opcodes
//  nNumOps             Number of opcodes
//  lpnStart            Ptr to int for start index
//  lpnEnd              Ptr to int for index after end
//
// Description:
//  The function finds the opcode index limits of the parameter
//  at the given index.  The function parses backwards from the
//  given number of opcodes.
//
//  If the line has more than eight parameters, it will not work.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HGetParamLimits
(
    int                 nParamIndex,
    LPBYTE              lpabyOps,
    int                 nNumOps,
    LPINT               lpnStart,
    LPINT               lpnEnd
)
{
    int                 anParamIndexes[8];
    int                 nCount;
    int                 i;
    int                 nTopLevelItemsNeeded;

    nCount = 0;
    nTopLevelItemsNeeded = 1;
    i = nNumOps;
    while (--i >= 0)
    {
        switch (lpabyOps[i])
        {
            case XL5_OP_PLUS:
            case XL5_OP_AMPERSAND:
                ++nTopLevelItemsNeeded;
                break;

            case XL5_OP_IDENT:
            case XL5_OP_LIT_STR:
                --nTopLevelItemsNeeded;
                break;

            case XL5_OP_X__IDENT:
            case XL5_OP__IDENT:
                break;

            default:
                return(FALSE);
        }

        if (nTopLevelItemsNeeded == 0)
        {
            anParamIndexes[nCount++] = i;
            nTopLevelItemsNeeded = 1;

            if (nCount == 8)
                break;
        }
    }

    if (nCount > nParamIndex)
    {
        // Normalize index

        nParamIndex = nCount - nParamIndex - 1;

        *lpnStart = anParamIndexes[nParamIndex];
        if (nParamIndex == 0)
            *lpnEnd = nNumOps;
        else
            *lpnEnd = anParamIndexes[nParamIndex - 1];

        return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HGetParamVal()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  wFuncID             ID of called function/sub
//  nParamIndex         Index of parameter value to get
//  lpstObj             Destination object for parameter value
//
// Description:
//  The function searches for the first call to the function with
//  the given ID and resolves the value of the parameter at the
//  given index to lpstObj.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HGetParamVal
(
    LPXL5H              lpstEnv,
    WORD                wFuncID,
    int                 nParamIndex,
    LPXL5H_OBJ          lpstObj
)
{
    XL5H_SCAN_STATE_T   stState;
    BOOL                bFound;
    int                 nStart;
    int                 nEnd;

    stState.wFuncBeginLine = 0;
    stState.wNextScanLine = 0;
    stState.nAssignHead = 0;
    stState.nAssignCount = 0;
    stState.nWithHead = 0;
    stState.nWithCount = 0;

    // Search for the call line

    if (XL5HFindDottedCallAssign(lpstEnv,
                                 XL5H_FIND_NORM_SUBFUNC_CALL,
                                 1,
                                 &stState,
                                 &wFuncID,
                                 1,
                                 &bFound) == FALSE ||
        bFound == FALSE)
        return(FALSE);

    // Get the line info

    if (XL5HGetNormFuncSubCallInfo(lpstEnv,
                                   stState.wNextScanLine - 1,
                                   wFuncID) == FALSE)
        return(FALSE);

    // Get the parameter limits

    if (XL5HGetParamLimits(nParamIndex,
                           lpstEnv->abyOps,
                           lpstEnv->nNumOps,
                           &nStart,
                           &nEnd) == FALSE)
        return(FALSE);

    // See if the parameter is a top-level identifier only
    //  If so, then resolve its value, otherwise
    //  try to resolve the parameter as is

    if (nStart + 1 == nEnd)
    {
        // Only one parameter; is it a top-level ID?

        if (lpstEnv->abyOps[nStart] == XL5_OP_IDENT)
        {
            int         i;
            int         j;
            LPBYTE      lpabyID;

            // Resolve the assignment to it

            // First find the assignment

            j = stState.nAssignHead - 1;
            lpabyID = lpstEnv->lpabyLine + lpstEnv->anOpOffsets[nStart] + 2;
            for (i=0;i<stState.nAssignCount;i++)
            {
                if (j < 0)
                    j += XL5H_MAX_ASSIGNS;

                if (stState.abyAssignIDs[j][0] == lpabyID[0] &&
                    stState.abyAssignIDs[j][1] == lpabyID[1])
                {
                    // Found it

                    break;
                }

                --j;
            }

            if (i == stState.nAssignCount)
                return(FALSE);

            // Resolve the assignment

            if (XL5HResolveAssignLine(lpstEnv,
                                      stState.awAssignLines[j],
                                      lpabyID) == FALSE)
                return(FALSE);

            // Get the assignment value

            if (XL5HResolveFilenameAssign(lpstEnv->bLittleEndian,
                                          lpstEnv->lpabyLine,
                                          lpstEnv->anOpOffsets +
                                              lpstEnv->nParamStart,
                                          lpstEnv->abyOps +
                                              lpstEnv->nParamStart,
                                          lpstEnv->nParamEnd -
                                              lpstEnv->nParamStart,
                                          lpstEnv->awAppStrID,
                                          lpstObj) == FALSE)
                return(FALSE);

            return(TRUE);
        }
    }

    // Resolve the value as is

    if (XL5HResolveFilenameAssign(lpstEnv->bLittleEndian,
                                  lpstEnv->lpabyLine,
                                  lpstEnv->anOpOffsets + nStart,
                                  lpstEnv->abyOps + nStart,
                                  nEnd - nStart,
                                  lpstEnv->awAppStrID,
                                  lpstObj) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveFilenameIDs()
//
// Parameters:
//  lpstEnv             Ptr to environment
//
// Description:
//  The function resolves the IDs of the given filename object.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HResolveFilenameIDs
(
    LPXL5H              lpstEnv
)
{
    int                 nNumIDsResolved;
    int                 nIDPart;
    int                 nShift;
    int                 i;
    int                 j;
    int                 k;
    WORD                wLine;

    // Resolve at most 256 IDs

    nNumIDsResolved = 0;
    while (1)
    {
        for (nIDPart=0;nIDPart<lpstEnv->stParamObj.byNumParts;nIDPart++)
        {
            if (lpstEnv->stParamObj.abyType[nIDPart] == XL5H_OBJ_ID)
                break;
        }

        if (nIDPart < lpstEnv->stParamObj.byNumParts)
        {
            if (nNumIDsResolved++ >= 256)
                return(FALSE);

            // Resolve the ID

            k = lpstEnv->stSaveState.nAssignHead - 1;
            wLine = 0;
            for (j=0;j<lpstEnv->stSaveState.nAssignCount;j++)
            {
                if (k < 0)
                    k += XL5H_MAX_ASSIGNS;

                if (lpstEnv->stParamObj.abyInfo[nIDPart][0] ==
                    lpstEnv->stSaveState.abyAssignIDs[k][0] &&
                    lpstEnv->stParamObj.abyInfo[nIDPart][1] ==
                    lpstEnv->stSaveState.abyAssignIDs[k][1])
                {
                    wLine = lpstEnv->stSaveState.awAssignLines[k];
                    break;
                }

                --k;
            }

            if (j < lpstEnv->stSaveState.nAssignCount)
            {
                // Found an assignment

                // Find the assignment parts

                if (XL5HResolveAssignLine(lpstEnv,
                                          wLine,
                                          lpstEnv->stParamObj.
                                              abyInfo[nIDPart]) == FALSE)
                    return(FALSE);

                // Get the assignment value

                if (XL5HResolveFilenameAssign(lpstEnv->bLittleEndian,
                                              lpstEnv->lpabyLine,
                                              lpstEnv->anOpOffsets +
                                                  lpstEnv->nParamStart,
                                              lpstEnv->abyOps +
                                                  lpstEnv->nParamStart,
                                              lpstEnv->nParamEnd -
                                                  lpstEnv->nParamStart,
                                              lpstEnv->awAppStrID,
                                              &lpstEnv->stTempObj) == FALSE)
                    return(FALSE);

            }
            else
            {
                WORD        wFuncID;
                int         nParamIndex;

                // Check for a parameter

                if (XL5HGetParamInfo(lpstEnv,
                                     lpstEnv->stSaveState.wFuncBeginLine,
                                     lpstEnv->stParamObj.abyInfo[nIDPart],
                                     &wFuncID,
                                     &nParamIndex) != FALSE &&
                    nParamIndex != -1)
                {
                    // Find param

                    if (XL5HGetParamVal(lpstEnv,
                                        wFuncID,
                                        nParamIndex,
                                        &lpstEnv->stTempObj) == FALSE)
                        return(FALSE);

                    wLine = 1;
                }
            }

            if (wLine != 0)
            {
                // Make sure there is enough room for it

                if (lpstEnv->stParamObj.byNumParts - 1 +
                    lpstEnv->stTempObj.byNumParts > XL5H_OBJ_MAX_PARTS)
                    return(FALSE);

                // Just in case byNumParts is zero,
                //  which it should never be

                lpstEnv->stParamObj.abyType[nIDPart] = XL5H_OBJ_UNKNOWN;

                // Shift stuff to the right

                if (lpstEnv->stTempObj.byNumParts != 0)
                {
                    nShift = lpstEnv->stTempObj.byNumParts - 1;
                    if (nShift != 0)
                    {
                        // Shift stuff to the right

                        for (i=lpstEnv->stParamObj.byNumParts;i>nIDPart;i--)
                        {
                            lpstEnv->stParamObj.abyType[i+nShift] =
                                lpstEnv->stParamObj.abyType[i];
                            lpstEnv->stParamObj.abyInfo[i+nShift][0] =
                                lpstEnv->stParamObj.abyInfo[i][0];
                            lpstEnv->stParamObj.abyInfo[i+nShift][1] =
                                lpstEnv->stParamObj.abyInfo[i][1];
                        }
                    }

                    // Insert the new parts

                    for (i=0;i<lpstEnv->stTempObj.byNumParts;i++)
                    {
                        lpstEnv->stParamObj.abyType[nIDPart+i] =
                            lpstEnv->stTempObj.abyType[i];

                        if (lpstEnv->stTempObj.abyType[i] ==
                            XL5H_OBJ_STRING)
                        {
                            LPBYTE  lpabyDst;
                            LPBYTE  lpabySrc;
                            int     nLen;

                            nLen = lpstEnv->stTempObj.abyInfo[i][1];
                            if (lpstEnv->stParamObj.byDataUsed + nLen >
                                XL5H_OBJ_MAX_DATA_SIZE)
                            {
                                // No more room for the string

                                return(FALSE);
                            }

                            lpstEnv->stParamObj.abyInfo[nIDPart+i][0] =
                                lpstEnv->stParamObj.byDataUsed;
                            lpstEnv->stParamObj.abyInfo[nIDPart+i][1] =
                                (BYTE)nLen;

                            // Get the string ptrs

                            lpabyDst = lpstEnv->stParamObj.abyData +
                                lpstEnv->stParamObj.byDataUsed;

                            lpabySrc = lpstEnv->stTempObj.abyData +
                                lpstEnv->stTempObj.abyInfo[i][0];

                            // Update the data used

                            lpstEnv->stParamObj.byDataUsed += nLen;

                            // Copy the string

                            while (nLen-- > 0)
                                *lpabyDst++ = *lpabySrc++;
                        }
                        else
                        {
                            // Otherwise, just copy the Info bytes

                            lpstEnv->stParamObj.abyInfo[nIDPart+i][0] =
                                lpstEnv->stTempObj.abyInfo[i][0];
                            lpstEnv->stParamObj.abyInfo[nIDPart+i][1] =
                                lpstEnv->stTempObj.abyInfo[i][1];
                        }
                    }

                    lpstEnv->stParamObj.byNumParts += nShift;
                }
            }
            else
            {
                // Could not find assignment

                return(FALSE);
            }
        }
        else
        {
            // No more IDs

            return(TRUE);
        }
    }
}


//********************************************************************
//
// Function:
//  BOOL XL5HObjConsolidateAsStr()
//
// Parameters:
//  lpstObj             Ptr to object to consolidate
//
// Description:
//  The function concatenates the string parts of the object
//  into a single string part.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HObjConsolidateAsStr
(
    LPXL5H_OBJ          lpstObj
)
{
    BYTE                abyName[XL5H_OBJ_MAX_DATA_SIZE];
    int                 nLen;
    int                 i;
    int                 j;
    int                 nSrcLen;
    LPBYTE              lpabySrc;

    // Copy the string parts to the abyName[] buffer

    nLen = 0;
    for (i=0;i<lpstObj->byNumParts;i++)
    {
        if (lpstObj->abyType[i] == XL5H_OBJ_STRING)
        {
            lpabySrc = lpstObj->abyData + lpstObj->abyInfo[i][0];
            nSrcLen = lpstObj->abyInfo[i][1];
            for (j=0;j<nSrcLen;j++)
                abyName[nLen++] = *lpabySrc++;
        }
    }

    // Now copy back to the object

    for (i=0;i<nLen;i++)
        lpstObj->abyData[i] = abyName[i];

    lpstObj->byDataUsed = (BYTE)nLen;

    lpstObj->abyType[0] = XL5H_OBJ_STRING;
    lpstObj->abyInfo[0][0] = 0;
    lpstObj->abyInfo[0][1] = (BYTE)nLen;

    lpstObj->byNumParts = 1;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveXLSTARTFilename()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//  lpstObj             Ptr to object to with filename
//  bStoreName          TRUE if the name should be stored
//
// Description:
//  The function assumes that all values in the object are valid.
//  The function consolidates the object as a string and
//  determines whether the path contains "xlstart".
//
//  If bStoreName is TRUE, the function stores the base filename
//  in lpstEnv->abypsszSaveName[].
//
// Returns:
//  TRUE                If the name is of the form xlstart/filename
//  FALSE               On error
//
//********************************************************************

BOOL XL5HResolveXLSTARTFilename
(
    LPXL5H              lpstEnv,
    LPXL5H_OBJ          lpstObj,
    BOOL                bStoreName
)
{
    int                 i;
    int                 nLen;
    int                 nFileNameI;
    LPBYTE              lpabySrc;
    LPBYTE              lpabyName;

    if (XL5HObjConsolidateAsStr(lpstObj) == FALSE)
        return(FALSE);

    nLen = lpstObj->abyInfo[0][1];
    lpabyName = lpstObj->abyData + lpstObj->abyInfo[0][0];

    // Verify that it is of the form xlstart/filename

    // Find the backslash from the end

    for (i=nLen-1;i>=0;i--)
    {
        if (lpabyName[i] == '\\')
            break;
    }

    nFileNameI = i;

    if (i < 0)
        return(FALSE);

    // Now find the penultimate backslash

    for (--i;i>0;i--)
    {
        if (lpabyName[i] == '\\')
        {
            // Make sure i points to the first character

            ++i;
            break;
        }
    }

    // Does the length match xlstart?

    if (nFileNameI - i != sizeof(gabyXL5Hxlstart))
        return(FALSE);

    // Now compare the strings

    lpabySrc = lpabyName + i;
    for (i=0;i<sizeof(gabyXL5Hxlstart);i++)
    {
        if (*lpabySrc++ != gabyXL5Hxlstart[i])
            return(FALSE);
    }

    // The directory is xlstart, so store the filename

    if (bStoreName == FALSE)
        return(TRUE);

    ++nFileNameI;
    nLen -= nFileNameI;
    if (nLen >= (XL5H_MAX_SAVE_NAME_SIZE - 1))
        nLen = XL5H_MAX_SAVE_NAME_SIZE - 2;

    // Store the name length

    lpstEnv->abypsszSaveName[0] = (BYTE)nLen;

    // Copy the name

    for (i=1;i<=nLen;i++)
        lpstEnv->abypsszSaveName[i] = lpabyName[nFileNameI++];

    // Zero-terminate the name

    lpstEnv->abypsszSaveName[nLen+1] = 0;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveObject()
//
// Parameters:
//  bLittleEndian       TRUE if the module is in little endian format
//  lpabyLine           Ptr to line contents
//  lpanOpOffsets       Ptr to opcode offsets
//  lpabyOps            Ptr to opcodes
//  nEndOp              The index after the last opcode
//  lpawAppStrID        Array of application string IDs
//  lpstObj             Ptr to destination object
//
// Description:
//  The function only handles parameters at the top-level.
//
//  The function works backwards from the end opcode until
//  a top-level object is seen.  A top-level object is
//  one of the following:
//
//      XL5_OP_IDENT        0x20
//      XL5_OP_IDENT_DIM    0x24
//      XL5_OP__IDENT       0x35
//      XL5_OP__IDENT_DIM   0x37
//      XL5_OP_WORD_VALUE   0xA3
//      XL5_OP_LIT_STR      0xAD
//
//  The function then consolidates the top-level object
//  with its subparts into lpstObj.
//
//  Following that, the function consolidates the any parameters
//  in the same manner.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HResolveObject
(
    BOOL                bLittleEndian,
    LPBYTE              lpabyLine,
    LPINT               lpanOpOffsets,
    LPBYTE              lpabyOps,
    int                 nEndOp,
    LPWORD              lpawAppStrID,
    LPXL5H_OBJ          lpstObj
)
{
    int                 i;
    int                 j;
    int                 k;
    int                 nOpcode;
    int                 nOffset;
    int                 nOpIndex;
    int                 nPartCount;
    WORD                wID;

    // Initialize the object

    lpstObj->byNumParts = 0;
    lpstObj->byDataUsed = 0;

    nOpIndex = nEndOp - 1;
    while (nOpIndex >= 0)
    {
        nOffset = lpanOpOffsets[nOpIndex];
        nOpcode = lpabyOps[nOpIndex];
        if (nOpcode == XL5_OP_IDENT ||
            nOpcode == XL5_OP_IDENT_DIM ||
            nOpcode == XL5_OP__IDENT ||
            nOpcode == XL5_OP__IDENT_DIM ||
            nOpcode == XL5_OP_LIT_STR ||
            nOpcode == XL5_OP_WORD_VALUE)
        {
            // Found a top level object

            break;
        }

        if (nOpcode != XL5_OP_X__IDENT &&
            nOpcode != XL5_OP_X__IDENT_DIM)
            return(FALSE);

        --nOpIndex;
    }

    if (nOpIndex < 0)
    {
        // Unable to find the top level object

        return(FALSE);
    }

    // Either the sequence begins with an ID or a string,
    //  or a word value

    if (nOpcode == XL5_OP_WORD_VALUE)
    {
        j = lpstObj->byNumParts++;
        lpstObj->abyType[j] = XL5H_OBJ_WORD_VALUE;
        lpstObj->abyInfo[j][0] = lpabyLine[nOffset + 2];
        lpstObj->abyInfo[j][1] = lpabyLine[nOffset + 3];
        lpstObj->abyParam[j][0] = 0;

        return(TRUE);
    }
    else
    if (nOpcode == XL5_OP_LIT_STR)
    {
        BYTE        byChar;
        int         nLen;

        j = lpstObj->byNumParts++;

        // Make sure there is enough room for the string

        if (bLittleEndian)
            nLen = lpabyLine[nOffset + 2];
        else
            nLen = lpabyLine[nOffset + 3];

        if (lpstObj->byDataUsed + nLen > XL5H_OBJ_MAX_DATA_SIZE)
            return(FALSE);

        // Set the type, index, and length

        lpstObj->abyType[j] = XL5H_OBJ_STRING;
        lpstObj->abyInfo[j][0] = lpstObj->byDataUsed;
        lpstObj->abyInfo[j][1] = (BYTE)nLen;
        lpstObj->abyParam[j][0] = 0;

        // Copy the string

        for (k=0;k<nLen;k++)
        {
            byChar = SSToLower(lpabyLine[nOffset + 4 + k]);

            // Convert path separators to canonical form

            if (byChar == '/' || byChar == ':')
                byChar = '\\';

            lpstObj->abyData[lpstObj->byDataUsed++] = byChar;
        }

        return(TRUE);
    }

    // At this point we are guaranteed a correct sequence
    //  of parts beginning with ID, ID(), .ID, or .ID(),
    //  followed by any number of x.ID and x.ID().

    nPartCount = nEndOp - nOpIndex;

    if (nPartCount > XL5H_OBJ_MAX_PARTS)
        return(FALSE);

    // Add the parts to the object

    for (i=nOpIndex;i<nEndOp;i++)
    {
        nOffset = lpanOpOffsets[i];
        nOpcode = lpabyOps[i];

        // Get the parameter count

        switch (nOpcode)
        {
            case XL5_OP_IDENT_DIM:
            case XL5_OP__IDENT_DIM:
            case XL5_OP_X__IDENT_DIM:
                lpstObj->abyParam[lpstObj->byNumParts][0] =
                    bLittleEndian ? lpabyLine[nOffset + 4] :
                                    lpabyLine[nOffset + 5];
                break;

            default:
            // case XL5_OP_IDENT:
            // case XL5_OP__IDENT:
            // case XL5_OP_X__IDENT:
                lpstObj->abyParam[lpstObj->byNumParts][0] = 0;
                break;
        }

        // Get the ID

        wID = SSEndianWORD(lpabyLine+nOffset+2,bLittleEndian);

        // Check for known IDs

        for (j=0;j<XL5H_VSTR_ID_COUNT;j++)
        {
            if (wID == lpawAppStrID[j])
            {
                lpstObj->abyType[lpstObj->byNumParts] = XL5H_OBJ_VSTR_ID;
                lpstObj->abyInfo[lpstObj->byNumParts][0] = (BYTE)j;
                break;
            }
        }

        // Was it not a known ID?

        if (j == XL5H_VSTR_ID_COUNT)
        {
            if (nOpcode != XL5_OP_IDENT &&
                nOpcode != XL5_OP_IDENT_DIM)
            {
                // Only allow unknown IDs for top-level parts

                return(FALSE);
            }

            // Store the ID

            lpstObj->abyType[lpstObj->byNumParts] = XL5H_OBJ_ID;
            lpstObj->abyInfo[lpstObj->byNumParts][0] =
                lpabyLine[nOffset + 2];
            lpstObj->abyInfo[lpstObj->byNumParts][1] =
                lpabyLine[nOffset + 3];
        }

        lpstObj->byNumParts++;
    }

    // Add the parameters

    for (i=0;i<nPartCount;i++)
    {
        if (lpstObj->abyParam[i][0] == 0)
            continue;

        // Set the index of the parameter

        lpstObj->abyParam[i][0] = lpstObj->byNumParts;

        // Get the parameter

        nEndOp = nOpIndex;
        nOpIndex = nEndOp - 1;
        while (nOpIndex >= 0)
        {
            nOffset = lpanOpOffsets[nOpIndex];
            nOpcode = lpabyOps[nOpIndex];
            if (nOpcode == XL5_OP_WORD_VALUE ||
                nOpcode == XL5_OP_LIT_STR)
            {
                nEndOp = nOpIndex + 1;
                break;
            }

            if (nOpcode == XL5_OP_IDENT ||
                nOpcode == XL5_OP_IDENT_DIM ||
                nOpcode == XL5_OP__IDENT ||
                nOpcode == XL5_OP__IDENT_DIM)
            {
                // Found a top level object

                break;
            }

            if (nOpcode != XL5_OP_X__IDENT &&
                nOpcode != XL5_OP_X__IDENT_DIM)
                return(FALSE);

            --nOpIndex;
        }

        if (nOpIndex < 0)
        {
            // Unable to find the top level object

            return(FALSE);
        }

        // Make sure there is room

        if (lpstObj->byNumParts + (nEndOp - nOpIndex) >
            XL5H_OBJ_MAX_PARTS)
            return(FALSE);

        if (nOpcode == XL5_OP_WORD_VALUE)
        {
            j = lpstObj->byNumParts++;
            lpstObj->abyType[j] = XL5H_OBJ_WORD_VALUE;
            lpstObj->abyInfo[j][0] = lpabyLine[nOffset + 2];
            lpstObj->abyInfo[j][1] = lpabyLine[nOffset + 3];
            lpstObj->abyParam[j][0] = 0;
        }
        else
        if (nOpcode == XL5_OP_LIT_STR)
        {
            BYTE        byChar;
            int         nLen;

            j = lpstObj->byNumParts++;

            // Make sure there is enough room for the string

            if (bLittleEndian)
                nLen = lpabyLine[nOffset + 2];
            else
                nLen = lpabyLine[nOffset + 3];

            if (lpstObj->byDataUsed + nLen > XL5H_OBJ_MAX_DATA_SIZE)
                return(FALSE);

            // Set the type, index, and length

            lpstObj->abyType[j] = XL5H_OBJ_STRING;
            lpstObj->abyInfo[j][0] = lpstObj->byDataUsed;
            lpstObj->abyInfo[j][1] = (BYTE)nLen;
            lpstObj->abyParam[j][0] = 0;

            // Copy the string

            for (k=0;k<nLen;k++)
            {
                byChar = SSToLower(lpabyLine[nOffset + 4 + k]);

                // Convert path separators to canonical form

                if (byChar == '/' || byChar == ':')
                    byChar = '\\';

                lpstObj->abyData[lpstObj->byDataUsed++] = byChar;
            }
        }
        else
        {
            for (k=nOpIndex;k<nEndOp;k++)
            {
                nOffset = lpanOpOffsets[k];
                wID = SSEndianWORD(lpabyLine+nOffset+2,bLittleEndian);

                // Check for known IDs

                for (j=0;j<XL5H_VSTR_ID_COUNT;j++)
                {
                    if (wID == lpawAppStrID[j])
                    {
                        lpstObj->abyType[lpstObj->byNumParts] =
                            XL5H_OBJ_VSTR_ID;
                        lpstObj->abyInfo[lpstObj->byNumParts][0] = (BYTE)j;
                        break;
                    }
                }

                // Was it not a known ID?

                if (j == XL5H_VSTR_ID_COUNT)
                {
                    // Store the ID

                    lpstObj->abyType[lpstObj->byNumParts] = XL5H_OBJ_ID;
                    lpstObj->abyInfo[lpstObj->byNumParts][0] =
                        lpabyLine[nOffset + 2];
                    lpstObj->abyInfo[lpstObj->byNumParts][1] =
                        lpabyLine[nOffset + 3];
                }

                lpstObj->abyParam[lpstObj->byNumParts][0] = 0;
                lpstObj->byNumParts++;
            }
        }

        // Set the parameter part length

        lpstObj->abyParam[i][1] = lpstObj->byNumParts -
            lpstObj->abyParam[i][0];
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveObjectIDs()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  lpstObj             Ptr to object to resolve
//  lpstState           Ptr to state
//
// Description:
//  The function resolves any IDs in the given object.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL5HResolveObjectIDs
(
    LPXL5H              lpstEnv,
    LPXL5H_OBJ          lpstObj,
    LPXL5H_SCAN_STATE   lpstState
)
{
    int                 nNumIDsResolved;
    int                 nIDPart;
    int                 nShift;
    int                 i;
    int                 j;
    int                 k;
    BOOL                bResolveID;
    WORD                wLine;

    // Resolve at most 256 IDs

    nNumIDsResolved = 0;
    while (1)
    {
        for (nIDPart=0;nIDPart<lpstObj->byNumParts;nIDPart++)
        {
            if (lpstObj->abyType[nIDPart] == XL5H_OBJ_ID)
            {
                bResolveID = TRUE;
                break;
            }

            if (lpstObj->abyType[nIDPart] == XL5H_OBJ_ROOTED)
            {
                bResolveID = FALSE;
                break;
            }
        }

        if (nIDPart < lpstObj->byNumParts)
        {
            if (nNumIDsResolved++ >= 256)
                return(FALSE);

            if (bResolveID == FALSE)
            {
                // Find the encompassing With line

                wLine = ((WORD)lpstObj->abyInfo[nIDPart][0]) |
                    (((WORD)lpstObj->abyInfo[nIDPart][1]) << 8);

                k = lpstState->nWithHead;
                for (j=0;j<lpstState->nWithCount;j++)
                {
                    if (--k < 0)
                        k += XL5H_MAX_WITHS;

                    if (lpstState->awWithLines[k] <= wLine &&
                        wLine <= lpstState->awEndWithLines[k])
                        break;
                }

                if (j == lpstState->nWithCount)
                {
                    // Could not find it, change the part to unknown

                    lpstObj->abyType[nIDPart] = XL5H_OBJ_UNKNOWN;
                    continue;
                }

                // Find the With parts

                if (XL5HResolveWithLine(lpstEnv,
                                        lpstState->awWithLines[k]) == FALSE)
                    return(FALSE);
            }
            else
            {
                // Resolve the ID

                k = lpstState->nAssignHead;
                for (j=0;j<lpstState->nAssignCount;j++)
                {
                    if (--k < 0)
                        k += XL5H_MAX_ASSIGNS;

                    if (lpstObj->abyInfo[nIDPart][0] ==
                        lpstState->abyAssignIDs[k][0] &&
                        lpstObj->abyInfo[nIDPart][1] ==
                        lpstState->abyAssignIDs[k][1])
                        break;
                }

                if (j == lpstState->nAssignCount)
                {
                    // Could not find it, change the part to unknown

                    lpstObj->abyType[nIDPart] = XL5H_OBJ_UNKNOWN;
                    continue;
                }

                // Find the assignment parts

                if (XL5HResolveAssignLine(lpstEnv,
                                          lpstState->awAssignLines[k],
                                          lpstState->abyAssignIDs[k]) == FALSE)
                    return(FALSE);

            }

            // Get the value

            if (XL5HResolveObject(lpstEnv->bLittleEndian,
                                  lpstEnv->lpabyLine,
                                  lpstEnv->anOpOffsets,
                                  lpstEnv->abyOps,
                                  lpstEnv->nParamEnd,
                                  lpstEnv->awAppStrID,
                                  &lpstEnv->stTempObj) == FALSE)
                return(FALSE);

            // Make sure there is enough room for it

            if (lpstObj->byNumParts - 1 +
                lpstEnv->stTempObj.byNumParts > XL5H_OBJ_MAX_PARTS)
                return(FALSE);

            // Just in case byNumParts is zero,
            //  which it should never be

            lpstObj->abyType[nIDPart] = XL5H_OBJ_UNKNOWN;

            // Shift stuff to the right

            if (lpstObj->byNumParts != 0)
            {
                nShift = lpstEnv->stTempObj.byNumParts - 1;
                if (nShift != 0)
                {
                    // Shift the parameter indexes

                    for (i=0;i<lpstObj->byNumParts;i++)
                    {
                        if (lpstObj->abyParam[i][0] > nIDPart)
                            lpstObj->abyParam[i][0] += nShift;
                    }

                    // Shift stuff to the right

                    for (i=lpstObj->byNumParts-1;i>nIDPart;i--)
                    {
                        lpstObj->abyType[i+nShift] =
                            lpstObj->abyType[i];
                        lpstObj->abyInfo[i+nShift][0] =
                            lpstObj->abyInfo[i][0];
                        lpstObj->abyInfo[i+nShift][1] =
                            lpstObj->abyInfo[i][1];
                    }

                    // See if the item is a parameter

                    for (i=0;i<nIDPart;i++)
                    {
                        if (lpstObj->abyParam[i][0] != 0 &&
                            lpstObj->abyParam[i][0] <= nIDPart &&
                            nIDPart < lpstObj->abyParam[i][0] +
                                lpstObj->abyParam[i][1])
                        {
                            // Need to add the extra parts

                            lpstObj->abyParam[i][1] +=
                                lpstEnv->stTempObj.byNumParts - 1;
                            break;
                        }
                    }
                }

                // Insert the new parts

                for (i=0;i<lpstEnv->stTempObj.byNumParts;i++)
                {
                    lpstObj->abyType[nIDPart+i] =
                        lpstEnv->stTempObj.abyType[i];
                    lpstObj->abyInfo[nIDPart+i][0] =
                        lpstEnv->stTempObj.abyInfo[i][0];
                    lpstObj->abyInfo[nIDPart+i][1] =
                        lpstEnv->stTempObj.abyInfo[i][1];

                    lpstObj->abyParam[nIDPart+i][0] =
                        lpstEnv->stTempObj.abyParam[i][0];

                    // Rebase the parameters

                    if (lpstObj->abyParam[nIDPart+i][0] != 0)
                    {
                        lpstObj->abyParam[nIDPart+i][0] += nIDPart;
                        lpstObj->abyParam[nIDPart+i][1] =
                            lpstEnv->stTempObj.abyParam[i][1];
                    }

                    if (lpstEnv->stTempObj.abyType[i] ==
                        XL5H_OBJ_STRING)
                    {
                        LPBYTE  lpabyDst;
                        LPBYTE  lpabySrc;
                        int     nLen;

                        nLen = lpstEnv->stTempObj.abyInfo[i][1];
                        if (lpstObj->byDataUsed + nLen >
                            XL5H_OBJ_MAX_DATA_SIZE)
                        {
                            // No more room for the string

                            return(FALSE);
                        }

                        lpstObj->abyInfo[nIDPart+i][0] =
                            lpstObj->byDataUsed;
                        lpstObj->abyInfo[nIDPart+i][1] =
                            (BYTE)nLen;

                        // Get the string ptrs

                        lpabyDst = lpstObj->abyData +
                            lpstObj->byDataUsed;

                        lpabySrc = lpstEnv->stTempObj.abyData +
                            lpstEnv->stTempObj.abyInfo[i][0];

                        // Update the data used

                        lpstObj->byDataUsed += nLen;

                        // Copy the string

                        while (nLen-- > 0)
                            *lpabyDst++ = *lpabySrc++;
                    }
                }

                lpstObj->byNumParts += nShift;
            }
        }
        else
        {
            // No more IDs

            return(TRUE);
        }
    }
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveObjectOfSave()
//
// Parameters:
//  lpstObj             The object to check
//
// Description:
//  Determines whether the given resolved object is one of the
//  following forms:
//
//      [Application].ActiveWorkbook
//      [Application].ThisWorkbook
//      [Application].Workbooks([Application].ActiveWorkbook.Name)
//      [Application].Workbooks([Application].ThisWorkbook.Name)
//      [Application].Workbooks.Add
//
// Returns:
//  TRUE                If the object is either ActiveWorkbook
//                          or ThisWorkbook
//  FALSE               Otherwise
//
//********************************************************************

BOOL XL5HResolveObjectOfSave
(
    LPXL5H_OBJ          lpstObj
)
{
    int                 i;
    int                 nParamCount;

    i = 0;
    if (i >= lpstObj->byNumParts ||
        lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
        return(FALSE);

    // Check for [Application]

    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_application)
    {
        ++i;
        if (i >= lpstObj->byNumParts ||
            lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
            return(FALSE);
    }

    // Check for ActiveWorkbook or ThisWorkbook

    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_activeworkbook ||
        lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_thisworkbook)
    {
        // The object is either ActiveWorkbook or ThisWorkbook

        return(TRUE);
    }

    // Check for Workbooks

    if (lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_workbooks)
        return(FALSE);

    // Is it an Add?

    if (lpstObj->abyParam[i][0] == 0)
    {
        ++i;
        if (i < lpstObj->byNumParts &&
            lpstObj->abyType[i] == XL5H_OBJ_VSTR_ID &&
            lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_add)
            return(TRUE);

        return(FALSE);
    }

    // Now get the parameter for Workbooks

    // Expect either of:
    //  [Application].ActiveWorkbook.Name
    //  [Application].ThisWorkbook.Name

    nParamCount = lpstObj->abyParam[i][1];
    if (nParamCount < 2)
        return(FALSE);

    // Is it Application

    i = lpstObj->abyParam[i][0];
    if (lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
        return(FALSE);

    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_application)
    {
        ++i;
        if (nParamCount < 3)
            return(FALSE);
    }

    // Expect either ActiveWorkbook or ThisWorkbook

    if (lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID ||
        (lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_activeworkbook &&
         lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_thisworkbook))
        return(FALSE);

    // Expect Name

    ++i;
    if (lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID ||
        lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_name)
        return(FALSE);

    // The object is either ActiveWorkbook or ThisWorkbook

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveCallAssignObj()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  bCall               TRUE if call is to be resolved
//  wLine               Line number of the call
//  lpstState           Ptr to state
//  lpstObj             The destination object
//
// Description:
//  Resolves the line as a call/assign and stores the object of the
//  call at lpstObj.
//
// Returns:
//  TRUE                On success
//  FALSE               Otherwise
//
//********************************************************************

BOOL XL5HResolveCallAssignObj
(
    LPXL5H              lpstEnv,
    BOOL                bCall,
    WORD                wLine,
    LPXL5H_SCAN_STATE   lpstState,
    LPXL5H_OBJ          lpstObj
)
{
    if (XL5HGetLineInfo(lpstEnv,
                        bCall,
                        wLine,
                        NULL,
                        0) == FALSE)
        return(FALSE);

    if (lpstEnv->nLastCallOp <= 0)
        return(FALSE);

    if (lpstEnv->bCallIsRooted == FALSE)
    {
        if (XL5HResolveObject(lpstEnv->bLittleEndian,
                              lpstEnv->lpabyLine,
                              lpstEnv->anOpOffsets,
                              lpstEnv->abyOps,
                              lpstEnv->nLastCallOp,
                              lpstEnv->awAppStrID,
                              lpstObj) == FALSE)
            return(FALSE);
    }
    else
    {
        // Add a rooted object

        lpstObj->byNumParts = 1;
        lpstObj->byDataUsed = 0;
        lpstObj->abyType[0] = XL5H_OBJ_ROOTED;

        // Store the line number

        lpstObj->abyInfo[0][0] = (BYTE)(wLine & 0xFF);
        lpstObj->abyInfo[0][1] = (BYTE)((wLine >> 8) & 0xFF);
    }

    if (XL5HResolveObjectIDs(lpstEnv,
                             lpstObj,
                             lpstState) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HResolveSaveCallLine()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  wLine               Line number of the call
//
// Description:
//  Resolves the given save line and determines whether it is a
//  save of the ActiveWorkbook or ThisWorkbook to the startup
//  directory.
//
// Returns:
//  TRUE                On success
//  FALSE               Otherwise
//
//********************************************************************

BOOL XL5HResolveSaveCallLine
(
    LPXL5H              lpstEnv,
    WORD                wLine
)
{
    if (XL5HGetLineInfo(lpstEnv,
                        TRUE,
                        wLine,
                        lpstEnv->awAppStrID + XL5H_VSTR_ID_filename,
                        1) == FALSE)
        return(FALSE);

    // Get the first level expression

    if (XL5HResolveFilenameParam(lpstEnv->bLittleEndian,
                                 lpstEnv->lpabyLine,
                                 lpstEnv->anOpOffsets + lpstEnv->nParamStart,
                                 lpstEnv->abyOps + lpstEnv->nParamStart,
                                 lpstEnv->nParamEnd - lpstEnv->nParamStart,
                                 lpstEnv->awAppStrID,
                                 &lpstEnv->stParamObj) == FALSE)
        return(FALSE);

    // Resolve any variables

    if (XL5HResolveFilenameIDs(lpstEnv) == FALSE)
        return(FALSE);

    // Now consolidate the name

    if (XL5HResolveXLSTARTFilename(lpstEnv,
                                   &lpstEnv->stParamObj,
                                   TRUE) == FALSE)
        return(FALSE);

    // At this point, we are guaranteed that the filename parameter
    //  is a pathname containing xlstart

    // Now get the object of the save

    if (XL5HResolveCallAssignObj(lpstEnv,
                                 TRUE,
                                 wLine,
                                 &lpstEnv->stSaveState,
                                 &lpstEnv->stCallObj) == FALSE)
        return(FALSE);

    if (XL5HResolveObjectOfSave(&lpstEnv->stCallObj) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HModuleFindSave()
//
// Parameters:
//  lpstEnv             Ptr to environment
//  lpbFound            Ptr to boolean variable for found status
//
// Description:
//  Determines whether the module has a save call.
//
// Returns:
//  TRUE                On success
//  FALSE               Otherwise
//
//********************************************************************

BOOL XL5HModuleFindSave
(
    LPXL5H              lpstEnv,
    LPBOOL              lpbFound
)
{
    WORD                awCallList[2];

    awCallList[0] = lpstEnv->awAppStrID[XL5H_VSTR_ID_saveas];
    awCallList[1] = lpstEnv->awAppStrID[XL5H_VSTR_ID_savecopyas];


    return XL5HFindDottedCallAssign(lpstEnv,
                                    XL5H_FIND_DOTTED_CALL,
                                    1,
                                    &lpstEnv->stSaveState,
                                    awCallList,
                                    2,
                                    lpbFound);
}


//********************************************************************
//
// Function:
//  BOOL XL5HFastScanCB()
//
// Parameters:
//  lpvCookie       Ptr to XL5H_FAST_SCAN_T structure
//  lpeState        Ptr to current state variable
//  lpabyBuf        Ptr to buffer to scan
//  nCount          Buffer size
//
// Description:
//  Scans through the buffer looking for ditted calls and assignments
//  to add, copy, insertfile, name, saveas, and savecopyas.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL XL5HFastScanCB
(
    LPVOID              lpvCookie,
    LPEXL5_CRC_STATE    lpeState,
    LPBYTE              lpabyBuf,
    int                 nCount
)
{
    int                 i, j;
    XL5H_FAST_SCAN_T    stScan;
    EXL5_CRC_STATE_T    eState;
    WORD                wOp;

    stScan = *(LPXL5H_FAST_SCAN)lpvCookie;
    eState = *lpeState;

    i = 0;
    j = 0;
    while (0 <= i && i < nCount && j++ < nCount)
    {
        switch (eState)
        {
            case eXL5_CRC_STATE_OPCODE_LO:
                stScan.abyOp[0] = lpabyBuf[i++];
                eState = eXL5_CRC_STATE_OPCODE_HI;
                break;

            case eXL5_CRC_STATE_OPCODE_HI:
                stScan.abyOp[1] = lpabyBuf[i++];
                stScan.nOpcode = stScan.abyOp[0];

                if (stScan.nOpcode > XL5_OP_MAX)
                    stScan.nOpcode = XL5_OP_MAX;

                if (gabyXL5OpSkip[stScan.nOpcode] < 0x80)
                {
                    switch (stScan.nOpcode)
                    {
                        case XL5_OP_CALL_X__IDENT:
                        case XL5_OP_CALL__IDENT:
                        case XL5_OP_ASSIGN_X_IDENT:
                        case XL5_OP_ASSIGN_X_IDENT_DIM:
                        case XL5_OP_ASSIGN__IDENT:
                        case XL5_OP_ASSIGN__IDENT_DIM:
                            eState = eXL5_CRC_STATE_WORDOP_LO;
                            break;

                        default:
                            // Skip over any operands

                            i += gabyXL5OpSkip[stScan.nOpcode];
                            eState = eXL5_CRC_STATE_OPCODE_LO;
                            break;
                    }
                }
                else
                if (stScan.nOpcode == XL5_OP_VAR_DEFN)
                {
                    // Skip variable definitions

                    if ((stScan.abyOp[1] & 0x40) != 0)
                    {
                        // Add space for optional As column offset

                        i += 4;
                    }
                    else
                        i += 2;

                    eState = eXL5_CRC_STATE_OPCODE_LO;
                }
                else
                {
                    // There are a variable number of arguments

                    if (stScan.nOpcode == XL5_OP_QUOTED_COMMENT)
                    {
                        // Skip the next two bytes

                        i += 2;
                    }

                    // Then get the operand length

                    eState = eXL5_CRC_STATE_OP_LEN_LO;
                }
                break;

            case eXL5_CRC_STATE_OP_LEN_LO:
                stScan.wOpLen = lpabyBuf[i++];
                eState = eXL5_CRC_STATE_OP_LEN_HI;
                break;

            case eXL5_CRC_STATE_OP_LEN_HI:
                stScan.wOpLen |= (WORD)lpabyBuf[i++] << 8;

                // Round up to next even number

                i += stScan.wOpLen;
                if (stScan.wOpLen & 1)
                    ++i;

                eState = eXL5_CRC_STATE_OPCODE_LO;
                break;

            case eXL5_CRC_STATE_WORDOP_LO:
                stScan.abyOp[0] = lpabyBuf[i++];
                eState = eXL5_CRC_STATE_WORDOP_HI;
                break;

            case eXL5_CRC_STATE_WORDOP_HI:
                stScan.abyOp[1] = lpabyBuf[i++];
                wOp = ((WORD)stScan.abyOp[0]) | (((WORD)stScan.abyOp[1]) << 8);
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_add])
                    stScan.byModState |= XL5H_STATE_HAS_ADD;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_copy])
                    stScan.byModState |= XL5H_STATE_HAS_COPY;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_insertfile])
                    stScan.byModState |= XL5H_STATE_HAS_INSERTFILE;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_name])
                    stScan.byModState |= XL5H_STATE_HAS_NAME;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_saveas])
                    stScan.byModState |= XL5H_STATE_HAS_SAVE;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_savecopyas])
                    stScan.byModState |= XL5H_STATE_HAS_SAVE;

                switch (stScan.nOpcode)
                {
                    case XL5_OP_CALL_X__IDENT:
                    case XL5_OP_CALL__IDENT:
                    case XL5_OP_IDENT_DIM:
                    case XL5_OP_X__IDENT_DIM:
                    case XL5_OP__IDENT_DIM:
                        // Skip over parameter count

                        i += 2;
                        break;

                    default:
                        break;
                }

                eState = eXL5_CRC_STATE_OPCODE_LO;
                break;

            default:
                // This is an error condition
                return(FALSE);
        }
    }

    *lpeState = eState;
    *(LPXL5H_FAST_SCAN)lpvCookie = stScan;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5H97FastScanCB()
//
// Parameters:
//  lpvCookie       Ptr to XL5H_FAST_SCAN_T structure
//  lpeState        Ptr to current state variable
//  bLittleEndian   TRUE if the stream is in little endian format
//  lpabyBuf        Ptr to buffer to scan
//  nCount          Buffer size
//
// Description:
//  Scans through the buffer looking for ditted calls and assignments
//  to add, copy, insertfile, name, saveas, and savecopyas.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL XL5H97FastScanCB
(
    LPVOID              lpvCookie,
    LPEO97B_CRC_STATE   lpeState,
    int                 nVersion,
    BOOL                bLittleEndian,
    LPBYTE              lpabyBuf,
    int                 nCount
)
{
    int                 i, j, nOpSkip;
    XL5H_FAST_SCAN_T    stScan;
    EO97B_CRC_STATE_T   eState;
    WORD                wOp;

    stScan = *(LPXL5H_FAST_SCAN)lpvCookie;
    eState = *lpeState;

    i = 0;
    j = 0;
    while (0 <= i && i < nCount && j++ < nCount)
    {
        switch (eState)
        {
            case eO97B_CRC_STATE_OPCODE_LO:
                stScan.abyOp[0] = lpabyBuf[i++];
                eState = eO97B_CRC_STATE_OPCODE_HI;
                break;

            case eO97B_CRC_STATE_OPCODE_HI:
                stScan.abyOp[1] = lpabyBuf[i++];
                stScan.nOpcode = (int)(O97_OP_MASK &
                    SSEndianWORD(stScan.abyOp,bLittleEndian));

                if (nVersion == O97_VERSION_O97)
                {
                    if (stScan.nOpcode > O97_OP_MAX)
                        stScan.nOpcode = O97_OP_MAX;

                    nOpSkip = gabyO97OpSkip[stScan.nOpcode];
                    stScan.nOpcode = gabyXL5HOpO97toXL5[stScan.nOpcode];
                }
                else
                {
                    if (stScan.nOpcode > O98_OP_MAX)
                        stScan.nOpcode = O98_OP_MAX;

                    nOpSkip = gabyO98OpSkip[stScan.nOpcode];
                    stScan.nOpcode = gabyXL5HOpO98toXL5[stScan.nOpcode];
                }

                if (nOpSkip < 0x80)
                {
                    switch (stScan.nOpcode)
                    {
                        case XL5_OP_CALL_X__IDENT:
                        case XL5_OP_CALL__IDENT:
                        case XL5_OP_ASSIGN_X_IDENT:
                        case XL5_OP_ASSIGN_X_IDENT_DIM:
                        case XL5_OP_ASSIGN__IDENT:
                        case XL5_OP_ASSIGN__IDENT_DIM:
                            eState = eO97B_CRC_STATE_WORDOP_LO;
                            break;

                        default:
                            // Skip over any operands

                            i += nOpSkip;
                            eState = eO97B_CRC_STATE_OPCODE_LO;
                            break;
                    }
                }
                else
                if (stScan.nOpcode == XL5_OP_VAR_DEFN)
                {
                    // Skip variable definitions

                    if ((bLittleEndian && (stScan.abyOp[1] & 0x40) != 0) ||
                        (bLittleEndian == FALSE &&
                         (stScan.abyOp[0] & 0x40) != 0))
                    {
                        // Add space for optional As column offset

                        i += 6;
                    }
                    else
                        i += 4;

                    eState = eO97B_CRC_STATE_OPCODE_LO;
                }
                else
                {
                    // There are a variable number of arguments

                    if (stScan.nOpcode == XL5_OP_QUOTED_COMMENT)
                    {
                        // Skip the next two bytes

                        i += 2;
                    }

                    // Then get the operand length

                    eState = eO97B_CRC_STATE_OP_LEN_LO;
                }
                break;

            case eO97B_CRC_STATE_OP_LEN_LO:
                stScan.wOpLen = lpabyBuf[i++];
                eState = eO97B_CRC_STATE_OP_LEN_HI;
                break;

            case eO97B_CRC_STATE_OP_LEN_HI:
                if (bLittleEndian)
                    stScan.wOpLen |= (WORD)lpabyBuf[i++] << 8;
                else
                    stScan.wOpLen = (stScan.wOpLen << 8) |
                        ((WORD)lpabyBuf[i++]);

                // Round up to next even number

                i += stScan.wOpLen;
                if (stScan.wOpLen & 1)
                    ++i;

                eState = eO97B_CRC_STATE_OPCODE_LO;
                break;

            case eO97B_CRC_STATE_WORDOP_LO:
                stScan.abyOp[0] = lpabyBuf[i++];
                eState = eO97B_CRC_STATE_WORDOP_HI;
                break;

            case eO97B_CRC_STATE_WORDOP_HI:
                stScan.abyOp[1] = lpabyBuf[i++];
                wOp = SSEndianWORD(stScan.abyOp,bLittleEndian);

                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_add])
                    stScan.byModState |= XL5H_STATE_HAS_ADD;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_copy])
                    stScan.byModState |= XL5H_STATE_HAS_COPY;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_insertfile])
                    stScan.byModState |= XL5H_STATE_HAS_INSERTFILE;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_name])
                    stScan.byModState |= XL5H_STATE_HAS_NAME;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_saveas])
                    stScan.byModState |= XL5H_STATE_HAS_SAVE;
                else
                if (wOp == stScan.lpawAppStrID[XL5H_VSTR_ID_savecopyas])
                    stScan.byModState |= XL5H_STATE_HAS_SAVE;

                switch (stScan.nOpcode)
                {
                    case XL5_OP_CALL_X__IDENT:
                    case XL5_OP_CALL__IDENT:
                    case XL5_OP_IDENT_DIM:
                    case XL5_OP_X__IDENT_DIM:
                    case XL5_OP__IDENT_DIM:
                        // Skip over parameter count

                        i += 2;
                        break;

                    default:
                        break;
                }

                eState = eO97B_CRC_STATE_OPCODE_LO;
                break;

            default:
                // This is an error condition
                return(FALSE);
        }
    }

    *lpeState = eState;
    *(LPXL5H_FAST_SCAN)lpvCookie = stScan;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HGetModuleState()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wModuleNum      Index of module
//  lpbyState       Ptr to BYTE for state
//
// Description:
//  The function returns the scan state for the given module.
//  lpstEnv->lpstModStream should point to the module stream
//  at the given index.
//
//  The function assumes linear requests.
//
// Returns:
//  LPBYTE          On success
//  NULL            On error
//
//********************************************************************

BOOL XL5HGetModuleState
(
    LPXL5H              lpstEnv,
    WORD                wModuleNum,
    LPBYTE              lpbyState
)
{
    DWORD               dwCRC;
    DWORD               dwCRCByteCount;

    // Assume fold of one for now

    if (wModuleNum < lpstEnv->wStateStartGroup)
    {
        *lpbyState = 0;
        return(TRUE);
    }
    else
    if (wModuleNum < lpstEnv->wStateNextModToCheck)
    {
        if (wModuleNum - lpstEnv->wStateStartGroup < XL5H_STATE_SIZE)
        {
            *lpbyState =
                lpstEnv->abyState[wModuleNum - lpstEnv->wStateStartGroup];
            return(TRUE);
        }
    }
    else
    if (wModuleNum != lpstEnv->wStateNextModToCheck)
    {
        // This should never happen

        return(FALSE);
    }

    // Increment to next module expected to be checked

    lpstEnv->wStateNextModToCheck++;

    lpstEnv->stFastScan.byModState = 0;

    // Get the macro info

    if (lpstEnv->nType == XL5H_TYPE_95)
    {
        // Excel 5.0/95

        if (XL5GetMacroLineInfo(lpstEnv->lpstModStream,
                                &lpstEnv->stMod.st95.dwTableOffset,
                                &lpstEnv->stMod.st95.dwTableSize,
                                &lpstEnv->stMod.st95.dwDataOffset,
                                &lpstEnv->stMod.st95.dwDataSize) == FALSE)
            return(FALSE);

        // CRC and scan

        if (XL5ModuleCRCFast(lpstEnv->lpstModStream,
                             lpstEnv->stMod.st95.dwTableOffset,
                             lpstEnv->stMod.st95.dwTableSize,
                             lpstEnv->stMod.st95.dwDataOffset,
                             lpstEnv->stMod.st95.dwDataSize,
                             lpstEnv->lpabyBuf,
                             lpstEnv->nBufSize,
                             XL5HFastScanCB,
                             &lpstEnv->stFastScan,
                             &dwCRC,
                             &dwCRCByteCount) == FALSE)
            return(FALSE);

        // Check the exclusion data

        if (MacroIsExcluded(MACRO_EXCLUDE_TYPE_X95,
                            dwCRC,
                            dwCRCByteCount) != FALSE)
            lpstEnv->stFastScan.byModState = 0;
    }
    else
    {
        // Excel 97

        if (O97GetLineBinaryOffsets(lpstEnv->lpstModStream,
                                    &lpstEnv->stMod.st97.dwTableOffset,
                                    &lpstEnv->stMod.st97.dwTableSize,
                                    &lpstEnv->stMod.st97.dwDataOffset) == FALSE)
            return(FALSE);

        // CRC and scan

        if (O97ModuleCRCFast(lpstEnv->lpstModStream,
                             lpstEnv->stMod.st97.dwTableOffset,
                             lpstEnv->stMod.st97.dwTableSize,
                             lpstEnv->stMod.st97.dwDataOffset,
                             lpstEnv->lpabyBuf,
                             lpstEnv->nBufSize,
                             XL5H97FastScanCB,
                             &lpstEnv->stFastScan,
                             &dwCRC,
                             &dwCRCByteCount) == FALSE)
            return(FALSE);

        // Check the exclusion data

        if (MacroIsExcluded(MACRO_EXCLUDE_TYPE_X97,
                            dwCRC,
                            dwCRCByteCount) != FALSE)
            lpstEnv->stFastScan.byModState = 0;
    }

    // Does it belong to the prefix sequence of no-scan modules?

    if (wModuleNum == lpstEnv->wStateStartGroup &&
        lpstEnv->stFastScan.byModState == 0)
    {
        lpstEnv->wStateStartGroup++;
        *lpbyState = 0;
        return(TRUE);
    }

    // Store the state if there is room

    *lpbyState = lpstEnv->stFastScan.byModState;
    if (wModuleNum - lpstEnv->wStateStartGroup < XL5H_STATE_SIZE)
    {
        // Store the flags

        lpstEnv->abyState[wModuleNum - lpstEnv->wStateStartGroup] =
            *lpbyState;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HModuleNeedsSaveScan()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wModuleNum      Index of module
//  lpbNeedsScan    Ptr to BOOL to store scan status
//
// Description:
//  The function determines whether the module with the given
//  index should be scanned for viral save operations.  The
//  stream pointer (lpstEnv->lpstModStream) should already be
//  pointing to the module stream.
//
// Returns:
//  LPBYTE          On success
//  NULL            On error
//
//********************************************************************

BOOL XL5HModuleNeedsSaveScan
(
    LPXL5H              lpstEnv,
    WORD                wModuleNum,
    LPBOOL              lpbNeedsScan
)
{
    BYTE                byState;

    if (XL5HGetModuleState(lpstEnv,
                           wModuleNum,
                           &byState) == FALSE)
        return(FALSE);

    if (byState & XL5H_STATE_HAS_SAVE)
        *lpbNeedsScan = TRUE;
    else
        *lpbNeedsScan = FALSE;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HModuleNeedsCopyScan()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wModuleNum      Index of module
//  lpbNeedsScan    Ptr to BOOL to store scan status
//
// Description:
//  The function determines whether the module with the given
//  index should be scanned for viral copy operations.  The
//  stream pointer (lpstEnv->lpstModStream) should already be
//  pointing to the module stream.
//
// Returns:
//  LPBYTE          On success
//  NULL            On error
//
//********************************************************************

BOOL XL5HModuleNeedsCopyScan
(
    LPXL5H              lpstEnv,
    WORD                wModuleNum,
    LPBOOL              lpbNeedsScan
)
{
    BYTE                byState;

    if (XL5HGetModuleState(lpstEnv,
                           wModuleNum,
                           &byState) == FALSE)
        return(FALSE);

    if (byState & XL5H_STATE_HAS_COPY)
        *lpbNeedsScan = TRUE;
    else
        *lpbNeedsScan = FALSE;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HModuleNeedsAddNameScan()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wModuleNum      Index of module
//  lpbNeedsScan    Ptr to BOOL to store scan status
//
// Description:
//  The function determines whether the module with the given
//  index should be scanned for viral Add/Name operations.  The
//  stream pointer (lpstEnv->lpstModStream) should already be
//  pointing to the module stream.
//
// Returns:
//  LPBYTE          On success
//  NULL            On error
//
//********************************************************************

BOOL XL5HModuleNeedsAddNameScan
(
    LPXL5H              lpstEnv,
    WORD                wModuleNum,
    LPBOOL              lpbNeedsScan
)
{
    BYTE                byState;

    if (XL5HGetModuleState(lpstEnv,
                           wModuleNum,
                           &byState) == FALSE)
        return(FALSE);

    if ((byState & XL5H_STATE_HAS_ADD) &&
        (byState & XL5H_STATE_HAS_NAME))
        *lpbNeedsScan = TRUE;
    else
        *lpbNeedsScan = FALSE;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HModuleNeedsAddInsertScan()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wModuleNum      Index of module
//  lpbNeedsScan    Ptr to BOOL to store scan status
//
// Description:
//  The function determines whether the module with the given
//  index should be scanned for viral Add/InsertFile operations.
//  The stream pointer (lpstEnv->lpstModStream) should already be
//  pointing to the module stream.
//
// Returns:
//  LPBYTE          On success
//  NULL            On error
//
//********************************************************************

BOOL XL5HModuleNeedsAddInsertScan
(
    LPXL5H              lpstEnv,
    WORD                wModuleNum,
    LPBOOL              lpbNeedsScan
)
{
    BYTE                byState;

    if (XL5HGetModuleState(lpstEnv,
                           wModuleNum,
                           &byState) == FALSE)
        return(FALSE);

    if ((byState & XL5H_STATE_HAS_ADD) &&
        (byState & XL5H_STATE_HAS_INSERTFILE))
        *lpbNeedsScan = TRUE;
    else
        *lpbNeedsScan = FALSE;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HModuleNeedsAddInsertScan()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpbFound        Ptr to BOOL for found status
//
// Description:
//  Looks for a copy call.
//
// Returns:
//  LPBYTE          On success
//  NULL            On error
//
//********************************************************************

BOOL XL5HModuleFindCopy
(
    LPXL5H              lpstEnv,
    LPBOOL              lpbFound
)
{
    return XL5HFindDottedCallAssign(lpstEnv,
                                    XL5H_FIND_DOTTED_CALL,
                                    1,
                                    &lpstEnv->stCopyState,
                                    lpstEnv->awAppStrID +
                                        XL5H_VSTR_ID_copy,
                                    1,
                                    lpbFound);
}

//********************************************************************
//
// Function:
//  BOOL XL5HCopyObjIsViral()
//
// Parameters:
//  lpstObj             The object to check
//
// Description:
//  Determines whether the given resolved object is one of the
//  following forms:
//
//      [Application].ThisWorkbook.Sheets(savesheet)
//      [Application].ThisWorkbook.Modules(savesheet)
//      [Application].Workbooks(viralworkbook).Sheets(savesheet)
//      [Application].Workbooks(viralworkbook).Modules(savesheet)
//
// Returns:
//  TRUE                If the object is a viral object
//  FALSE               Otherwise
//
//********************************************************************

BOOL XL5HCopyObjIsViral
(
    LPXL5H              lpstEnv,
    LPXL5H_OBJ          lpstObj
)
{
    int                 i;
    int                 j;
    int                 nParamI;
    int                 nParamCount;
    int                 nLen;
    LPBYTE              lpabySrc;

    i = 0;
    if (i >= lpstObj->byNumParts ||
        lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
        return(FALSE);

    // Check for [Application]

    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_application)
    {
        ++i;
        if (i >= lpstObj->byNumParts ||
            lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
            return(FALSE);
    }

    // Check for Workbooks

    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_workbooks)
    {
        // Now get the parameter for Workbooks

        // Expect a string

        nParamCount = lpstObj->abyParam[i][1];
        if (nParamCount < 1)
            return(FALSE);

        nParamI = lpstObj->abyParam[i][0];
        if (lpstObj->abyType[nParamI] != XL5H_OBJ_STRING)
            return(FALSE);

        // Compare the workbook name

        nLen = lpstObj->abyInfo[nParamI][1];
        lpabySrc = lpstObj->abyData + lpstObj->abyInfo[nParamI][0];

        if (nLen != lpstEnv->abypsszSaveName[0])
            return(FALSE);

        // Compare the name

        for (j=0;j<nLen;j++)
        {
            if (lpabySrc[j] != lpstEnv->abypsszSaveName[j + 1])
                return(FALSE);
        }

        // Skip to the next part.  There are two cases:
        //  1. Workbooks viralworkbook Modules savesheet
        //  2. Workbooks Modules savesheet viralworkbook
        // The two cases correspond with the two branches
        // of the following if statement.
        //
        // The if statement is used to increment i to point
        // to the Modules/Sheets part of the object.

        if (nParamI == i + 1)
            i = i + 1 + nParamCount;
        else
            ++i;
    }
    else
    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_thisworkbook)
    {
        // It is ThisWorkbook

        ++i;
    }
    else
        return(FALSE);

    // Now look for Modules() or Sheets()

    if (i >= lpstObj->byNumParts)
        return(FALSE);

    if (lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
        return(FALSE);

    if (lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_modules &&
        lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_sheets)
        return(FALSE);

    // Now verify the module/sheet name/index

    nParamCount = lpstObj->abyParam[i][1];
    if (nParamCount < 1)
        return(FALSE);

    nParamI = lpstObj->abyParam[i][0];
    if (lpstObj->abyType[nParamI] == XL5H_OBJ_STRING)
    {
        // Compare the sheet name

        nLen = lpstObj->abyInfo[nParamI][1];
        lpabySrc = lpstObj->abyData + lpstObj->abyInfo[nParamI][0];

        if (nLen != lpstEnv->abypsSaveSheetName[0])
            return(FALSE);

        // Compare the name

        for (j=0;j<nLen;j++)
        {
            if (lpabySrc[j] != lpstEnv->abypsSaveSheetName[j + 1])
                return(FALSE);
        }

        return(TRUE);
    }
    else
    if (lpstObj->abyType[nParamI] == XL5H_OBJ_WORD_VALUE)
    {
        // Assume that it is the viral sheet

        return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HObjectIsAWB()
//
// Parameters:
//  lpstObj         Ptr to object to check
//  lpnAWBSize      Ptr to int for number of AWB parts
//
// Description:
//  Determines whether the given resolved object is one of the
//  following prefix forms:
//
//      [Application].ActiveWorkbook
//      [Application].Workbooks([Application].ActiveWorkbook.Name)
//
//  Otherwise, if the first part is either Sheets or Modules,
//  then it is assumed that it is the ActiveWorkbook.
//
// Returns:
//  TRUE            If yes
//  FALSE           If no
//
//********************************************************************

BOOL XL5HObjectIsAWB
(
    LPXL5H_OBJ          lpstObj,
    LPINT               lpnAWBSize
)
{
    int                 i;
    int                 nParamCount;

    i = 0;
    if (i >= lpstObj->byNumParts ||
        lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
        return(FALSE);

    // Check if the first part is either Modules or Sheets.
    //  If so, assume that it refers to the AWB

    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_modules ||
        lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_sheets)
    {
        if (lpnAWBSize != NULL)
            *lpnAWBSize = 0;

        return(TRUE);
    }

    // Check for [Application]

    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_application)
    {
        ++i;
        if (i >= lpstObj->byNumParts ||
            lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
            return(FALSE);
    }

    // Check for ActiveWorkbook

    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_activeworkbook)
    {
        // The object is ActiveWorkbook

        if (lpnAWBSize != NULL)
            *lpnAWBSize = i + 1;

        return(TRUE);
    }

    // Check for Workbooks

    if (lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_workbooks)
        return(FALSE);

    // Now get the parameter part count for Workbooks

    // Expect [Application].ActiveWorkbook.Name

    nParamCount = lpstObj->abyParam[i][1];
    if (nParamCount < 2)
        return(FALSE);

    // Is it Application

    i = lpstObj->abyParam[i][0];
    if (lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
        return(FALSE);

    if (lpstObj->abyInfo[i][0] == XL5H_VSTR_ID_application)
    {
        ++i;
        if (nParamCount < 3)
            return(FALSE);
    }

    // Look for ActiveWorkbook

    if (lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID ||
        lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_activeworkbook)
        return(FALSE);

    // Look for Name

    ++i;
    if (lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID ||
        lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_name)
        return(FALSE);

    if (lpnAWBSize != NULL)
        *lpnAWBSize = i + 1;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HCopyCallIsViral()
//
// Parameters:
//  lpstEnv         Ptr to environment
//  wLine           Line number of call
//
// Description:
//  Determines whether the given line is a copy of a viral sheet
//  to the ActiveWorkbook.
//
// Returns:
//  TRUE            If yes
//  FALSE           If no
//
//********************************************************************

BOOL XL5HCopyCallIsViral
(
    LPXL5H              lpstEnv,
    WORD                wLine
)
{
    WORD                awParamList[2];

    awParamList[0] = lpstEnv->awAppStrID[XL5H_VSTR_ID_after];
    awParamList[1] = lpstEnv->awAppStrID[XL5H_VSTR_ID_before];

    if (XL5HGetLineInfo(lpstEnv,
                        TRUE,
                        wLine,
                        awParamList,
                        2) == FALSE)
        return(FALSE);

    // Now get the parameter

    if (XL5HResolveObject(lpstEnv->bLittleEndian,
                          lpstEnv->lpabyLine,
                          lpstEnv->anOpOffsets + lpstEnv->nParamStart,
                          lpstEnv->abyOps + lpstEnv->nParamStart,
                          lpstEnv->nParamEnd - lpstEnv->nParamStart,
                          lpstEnv->awAppStrID,
                          &lpstEnv->stParamObj) == FALSE)
        return(FALSE);

    // Now get the object of the copy

    if (XL5HResolveCallAssignObj(lpstEnv,
                                 TRUE,
                                 wLine,
                                 &lpstEnv->stCopyState,
                                 &lpstEnv->stCallObj) == FALSE)
        return(FALSE);

    // Get the destination object

    if (XL5HResolveObjectIDs(lpstEnv,
                             &lpstEnv->stParamObj,
                             &lpstEnv->stCopyState) == FALSE)
        return(FALSE);

    if (XL5HObjectIsAWB(&lpstEnv->stParamObj,NULL) == FALSE)
        return(FALSE);

    if (XL5HCopyObjIsViral(lpstEnv,
                           &lpstEnv->stCallObj) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HHasAWBModulesAdd()
//
// Parameters:
//  lpstEnv         Ptr to environment
//
// Description:
//  Determines whether the module has an Add call of a Module
//  in the ActiveWorkbook.
//
// Returns:
//  TRUE            If yes
//  FALSE           If no
//
//********************************************************************

BOOL XL5HHasAWBModulesAdd
(
    LPXL5H              lpstEnv
)
{
    int                 nAddIter;
    BOOL                bFound;
    int                 i;

    lpstEnv->stCopyState.wFuncBeginLine = 0;
    lpstEnv->stCopyState.wNextScanLine = 0;
    lpstEnv->stCopyState.nAssignHead = 0;
    lpstEnv->stCopyState.nAssignCount = 0;
    lpstEnv->stCopyState.nWithHead = 0;
    lpstEnv->stCopyState.nWithCount = 0;

    // Check all the add calls

    for (nAddIter=0;nAddIter<256;nAddIter++)
    {
        // Find the next Add call

        if (XL5HFindDottedCallAssign(lpstEnv,
                                     XL5H_FIND_DOTTED_CALL,
                                     0,
                                     &lpstEnv->stCopyState,
                                     lpstEnv->awAppStrID +
                                         XL5H_VSTR_ID_add,
                                     1,
                                     &bFound) == FALSE ||
            bFound == FALSE)
            return(FALSE);

        // Now get the object of the add

        if (XL5HResolveCallAssignObj(lpstEnv,
                                     TRUE,
                                     lpstEnv->stCopyState.wNextScanLine - 1,
                                     &lpstEnv->stCopyState,
                                     &lpstEnv->stCallObj) == FALSE)
            continue;

        // Now determine whether it is
        //  [Application].[ActiveWorkbook].Modules().Add
        //  [Application].[Workbooks(ActiveWorkbook.Name)].Modules().Add

        if (XL5HObjectIsAWB(&lpstEnv->stCallObj,
                            &i) == FALSE)
            i = 0;

        if (i >= lpstEnv->stCallObj.byNumParts ||
            lpstEnv->stCallObj.abyType[i] != XL5H_OBJ_VSTR_ID)
            continue;

        // Check for Modules

        if (lpstEnv->stCallObj.abyInfo[i][0] == XL5H_VSTR_ID_modules)
            return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HModuleFindInsertFile()
//
// Parameters:
//  lpstEnv         Ptr to environment
//  lpbFound        Ptr to BOOL variable for found status
//
// Description:
//  Finds the next InsertFile call.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL XL5HModuleFindInsertFile
(
    LPXL5H              lpstEnv,
    LPBOOL              lpbFound
)
{
    return XL5HFindDottedCallAssign(lpstEnv,
                                    XL5H_FIND_DOTTED_CALL,
                                    1,
                                    &lpstEnv->stCopyState,
                                    lpstEnv->awAppStrID +
                                        XL5H_VSTR_ID_insertfile,
                                    1,
                                    lpbFound);
}


//********************************************************************
//
// Function:
//  BOOL XL5HObjIsActiveSheet()
//
// Parameters:
//  lpstObj         Ptr to object to check
//
// Description:
//  Determines whether the given object is an ActiveWorkbook
//  qualified ActiveSheet.
//
// Returns:
//  TRUE            If yes
//  FALSE           If no
//
//********************************************************************

BOOL XL5HObjIsActiveSheet
(
    LPXL5H_OBJ          lpstObj
)
{
    int                 i;

    /////////////////////////////////////////////////////////////
    // Verify that it is
    //  [Application].[ActiveWorkbook].ActiveSheet

    if (XL5HObjectIsAWB(lpstObj,&i) == FALSE)
        i = 0;

    if (i >= lpstObj->byNumParts ||
        lpstObj->abyType[i] != XL5H_OBJ_VSTR_ID)
        return(FALSE);

    // Check for ActiveSheet

    if (lpstObj->abyInfo[i][0] != XL5H_VSTR_ID_activesheet)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HInsertFileCallIsViral()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wLine           Line to check
//
// Description:
//  Determines whether the given object is an InsertFile
//  into the ActiveWorkbook from a file from the startup
//  directory.
//
// Returns:
//  TRUE            If yes
//  FALSE           If no
//
//********************************************************************

BOOL XL5HInsertFileCallIsViral
(
    LPXL5H              lpstEnv,
    WORD                wLine
)
{
    if (XL5HGetLineInfo(lpstEnv,
                        TRUE,
                        wLine,
                        lpstEnv->awAppStrID + XL5H_VSTR_ID_filename,
                        1) == FALSE)
        return(FALSE);

    // Now get the parameter

    if (XL5HResolveFilenameParam(lpstEnv->bLittleEndian,
                                 lpstEnv->lpabyLine,
                                 lpstEnv->anOpOffsets + lpstEnv->nParamStart,
                                 lpstEnv->abyOps + lpstEnv->nParamStart,
                                 lpstEnv->nParamEnd - lpstEnv->nParamStart,
                                 lpstEnv->awAppStrID,
                                 &lpstEnv->stParamObj) == FALSE)
        return(FALSE);

    // Now get the object of the InsertFile

    if (XL5HResolveCallAssignObj(lpstEnv,
                                 TRUE,
                                 wLine,
                                 &lpstEnv->stCopyState,
                                 &lpstEnv->stCallObj) == FALSE)
        return(FALSE);

    // Verify that it is
    //  [Application].[ActiveWorkbook].ActiveSheet

    if (XL5HObjIsActiveSheet(&lpstEnv->stCallObj) == FALSE)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Verify that the parameter is a string containing xlstart

    // Resolve any variables

    if (XL5HResolveFilenameIDs(lpstEnv) == FALSE)
        return(FALSE);

    // Now consolidate the name

    if (XL5HResolveXLSTARTFilename(lpstEnv,
                                   &lpstEnv->stParamObj,
                                   FALSE) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HNameAssignmentIsViral()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wLine           Line to check
//
// Description:
//  Determines whether the given assignment to a Name property
//  is viral.  It is if the name is the same as the module
//  containing the viral save call.
//
// Returns:
//  TRUE            If yes
//  FALSE           If no
//
//********************************************************************

BOOL XL5HNameAssignmentIsViral
(
    LPXL5H              lpstEnv,
    WORD                wLine
)
{
    int                 i;
    LPBYTE              lpabyNewName;

    if (XL5HGetLineInfo(lpstEnv,
                        FALSE,
                        wLine,
                        NULL,
                        0) == FALSE)
        return(FALSE);

    // Get the first level expression

    if (XL5HResolveFilenameParam(lpstEnv->bLittleEndian,
                                 lpstEnv->lpabyLine,
                                 lpstEnv->anOpOffsets + lpstEnv->nParamStart,
                                 lpstEnv->abyOps + lpstEnv->nParamStart,
                                 lpstEnv->nParamEnd - lpstEnv->nParamStart,
                                 lpstEnv->awAppStrID,
                                 &lpstEnv->stParamObj) == FALSE)
        return(FALSE);

    // Resolve any variables

    if (XL5HResolveObjectIDs(lpstEnv,
                             &lpstEnv->stParamObj,
                             &lpstEnv->stCopyState) == FALSE)
        return(FALSE);

    // Now consolidate the name

    if (XL5HObjConsolidateAsStr(&lpstEnv->stParamObj) == FALSE)
        return(FALSE);

    // Verify that the parameter is the one containing the SaveAs

    if (lpstEnv->stParamObj.abyInfo[0][1] != lpstEnv->abypsSaveSheetName[0])
        return(FALSE);

    lpabyNewName = lpstEnv->stParamObj.abyData +
        lpstEnv->stParamObj.abyInfo[0][0];

    // Compare the names

    for (i=0;i<lpstEnv->abypsSaveSheetName[0];i++)
    {
        if (lpabyNewName[i] != lpstEnv->abypsSaveSheetName[i+1])
            return(FALSE);
    }

    // Now get the object of the .Name

    if (XL5HResolveCallAssignObj(lpstEnv,
                                 FALSE,
                                 wLine,
                                 &lpstEnv->stCopyState,
                                 &lpstEnv->stCallObj) == FALSE)
        return(FALSE);

    // Verify that it is
    //  [Application].[ActiveWorkbook].ActiveSheet

    if (XL5HObjIsActiveSheet(&lpstEnv->stCallObj) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL5HModuleFindNameAssign()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpbFound        Ptr to BOOL variable for found status
//
// Description:
//  Finds the next assignment to a Name property.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL XL5HModuleFindNameAssign
(
    LPXL5H              lpstEnv,
    LPBOOL              lpbFound
)
{
    return XL5HFindDottedCallAssign(lpstEnv,
                                    XL5H_FIND_DOTTED_ASSIGN,
                                    0,
                                    &lpstEnv->stCopyState,
                                    lpstEnv->awAppStrID +
                                        XL5H_VSTR_ID_name,
                                    1,
                                    lpbFound);
}

#endif // #ifdef MACROHEU

