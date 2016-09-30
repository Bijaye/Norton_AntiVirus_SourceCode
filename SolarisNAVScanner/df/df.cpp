//************************************************************************
//
// $Header:   S:/DF/VCS/df.cpv   1.6   06 Nov 1998 19:13:12   DCHI  $
//
// Description:
//  Deflation implementation.  See RFC 1951.
//
//************************************************************************
// $Log:   S:/DF/VCS/df.cpv  $
// 
//    Rev 1.6   06 Nov 1998 19:13:12   DCHI
// Modified DeflateBlock() to handle case where block < 3 bytes.
// 
//    Rev 1.5   05 Nov 1998 16:55:58   DCHI
// Mark final block when last block is even multiple of 16K.
// 
//    Rev 1.4   28 Oct 1998 17:24:52   DCHI
// Modified DFDeflateBlock() to limit hashing to end of block - 2.
// 
//    Rev 1.3   29 Jul 1998 10:04:30   DCHI
// Replaced calls to memset/memcpy with calls to internal implementations.
// 
//    Rev 1.2   26 Nov 1997 15:17:16   DCHI
// Added DFInit().
// 
//    Rev 1.1   11 Nov 1997 17:53:30   DCHI
// Corrected incorrect constants used in memsets.
// 
//    Rev 1.0   11 Nov 1997 16:21:20   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "platform.h"
#include "df.h"
#include "crc32.h"

//********************************************************************
//
// Function:
//  void DFZeroBuf()
//
// Parameters:
//  lpabyBuf    Ptr to buffer
//  nSize       Size of buffer
//
// Description:
//  Fills the given buffer with zeroes.
//
// Returns:
//  Nothing
//
//********************************************************************

void DFZeroBuf
(
    LPBYTE      lpabyBuf,
    int         nSize
)
{
    while (nSize-- != 0)
        *lpabyBuf++ = 0;
}


//********************************************************************
//
// Function:
//  void DFCopyBuf()
//
// Parameters:
//  lpabyDst    Ptr to destination buffer
//  lpabySrc    Ptr to source buffer
//  nSize       Size of buffer
//
// Description:
//  Copies from the source to the destination.
//
// Returns:
//  Nothing
//
//********************************************************************

void DFCopyBuf
(
    LPBYTE      lpabyDst,
    LPBYTE      lpabySrc,
    int         nSize
)
{
    while (nSize-- != 0)
        *lpabyDst++ = *lpabySrc++;
}


//********************************************************************
//
// Function:
//  LPDF DFAllocDF()
//
// Parameters:
//  None
//
// Description:
//  Allocates memory for the DF structure.
//
// Returns:
//  LPDF        On success
//  NULL        On error
//
//********************************************************************

LPDF DFAllocDF
(
    LPVOID      lpvRootCookie
)
{
    LPDF        lpstDF;

    if (DFMemoryAlloc(lpvRootCookie,
                      sizeof(DF_T),
                      (LPLPVOID)&lpstDF) != DF_STATUS_OK)
        return(NULL);

    if (DFMemoryAlloc(lpvRootCookie,
                      DF_HIST_SIZE * sizeof(BYTE),
                      (LPLPVOID)&lpstDF->lpabyLitLen) != DF_STATUS_OK)
        lpstDF->lpabyLitLen = NULL;

    if (DFMemoryAlloc(lpvRootCookie,
                      DF_HIST_SIZE * sizeof(WORD),
                      (LPLPVOID)&lpstDF->lpawDist) != DF_STATUS_OK)
        lpstDF->lpawDist = NULL;

    if (DFMemoryAlloc(lpvRootCookie,
                      DF_HASH_SIZE * sizeof(WORD),
                      (LPLPVOID)&lpstDF->stHash.lpawFirst0) != DF_STATUS_OK)
        lpstDF->stHash.lpawFirst0 = NULL;

    if (DFMemoryAlloc(lpvRootCookie,
                      DF_HIST_SIZE * sizeof(WORD),
                      (LPLPVOID)&lpstDF->stHash.lpawPrev0) != DF_STATUS_OK)
        lpstDF->stHash.lpawPrev0 = NULL;

    if (DFMemoryAlloc(lpvRootCookie,
                      DF_HASH_SIZE * sizeof(WORD),
                      (LPLPVOID)&lpstDF->stHash.lpawFirst1) != DF_STATUS_OK)
        lpstDF->stHash.lpawFirst1 = NULL;

    if (DFMemoryAlloc(lpvRootCookie,
                      DF_HIST_SIZE * sizeof(WORD),
                      (LPLPVOID)&lpstDF->stHash.lpawPrev1) != DF_STATUS_OK)
        lpstDF->stHash.lpawPrev1 = NULL;

    if (lpstDF->lpabyLitLen == NULL ||
        lpstDF->lpawDist == NULL ||
        lpstDF->stHash.lpawFirst0 == NULL ||
        lpstDF->stHash.lpawPrev0 == NULL ||
        lpstDF->stHash.lpawFirst1 == NULL ||
        lpstDF->stHash.lpawPrev1 == NULL)
    {
        DFFreeDF(lpstDF);
        return(NULL);
    }

    lpstDF->lpvRootCookie = lpvRootCookie;

    return(lpstDF);
}


//********************************************************************
//
// Function:
//  BOOL DFFreeDF()
//
// Parameters:
//  lpstDF      DF structure to free
//
// Description:
//  Frees memory allocated for the DF structure.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//********************************************************************

BOOL DFFreeDF
(
    LPDF        lpstDF
)
{
    BOOL        bResult = TRUE;

    if (lpstDF == NULL)
        return(TRUE);

    if (lpstDF->stHash.lpawPrev1 != NULL)
    {
        if (DFMemoryFree(lpstDF->lpvRootCookie,
                         lpstDF->stHash.lpawPrev1) != DF_STATUS_OK)
            bResult = FALSE;
    }

    if (lpstDF->stHash.lpawFirst1 != NULL)
    {
        if (DFMemoryFree(lpstDF->lpvRootCookie,
                         lpstDF->stHash.lpawFirst1) != DF_STATUS_OK)
            bResult = FALSE;
    }

    if (lpstDF->stHash.lpawPrev0 != NULL)
    {
        if (DFMemoryFree(lpstDF->lpvRootCookie,
                         lpstDF->stHash.lpawPrev0) != DF_STATUS_OK)
            bResult = FALSE;
    }

    if (lpstDF->stHash.lpawFirst0 != NULL)
    {
        if (DFMemoryFree(lpstDF->lpvRootCookie,
                         lpstDF->stHash.lpawFirst0) != DF_STATUS_OK)
            bResult = FALSE;
    }

    if (lpstDF->lpawDist != NULL)
    {
        if (DFMemoryFree(lpstDF->lpvRootCookie,
                         lpstDF->lpawDist) != DF_STATUS_OK)
            bResult = FALSE;
    }

    if (lpstDF->lpabyLitLen != NULL)
    {
        if (DFMemoryFree(lpstDF->lpvRootCookie,
                         lpstDF->lpabyLitLen) != DF_STATUS_OK)
            bResult = FALSE;
    }

    if (DFMemoryFree(lpstDF->lpvRootCookie,
                     lpstDF) != DF_STATUS_OK)
        bResult = FALSE;

    return(bResult);
}


//********************************************************************
//
// Function:
//  void DFHash()
//
// Parameters:
//  lpawFirst       Ptr to first entry in chain array
//  lpawPrev        Ptr to prev entry in chain array
//  wOffset         Offset of first byte to hash
//  lpbyStrm        Pts to first byte to hash
//  nCount          Number of bytes to hash
//
// Description:
//  Hashes the given bytes.
//
// Returns:
//  Nothing
//
//********************************************************************

void DFHash
(
    LPWORD      lpawFirst,
    LPWORD      lpawPrev,
    WORD        wOffset,
    LPBYTE      lpbyStrm,
    int         nCount
)
{
    int         nHash;

    assert(wOffset + nCount <= DF_HIST_SIZE);

    while (nCount-- > 0)
    {
        nHash = DF_HASH(lpbyStrm);

        if (lpawFirst[nHash] != 0)
            lpawPrev[wOffset] = lpawFirst[nHash];

        lpawFirst[nHash] = ++wOffset;
        ++lpbyStrm;
    }
}


//********************************************************************
//
// Function:
//  BOOL DFDeflateBlock()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//
// Description:
//  Deflates the block.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL DFDeflateBlock
(
    LPDF        lpstDF
)
{
    int         nBestLen, nLen;
    int         nChainLen;
    WORD        w, wEnd, wCand;
    WORD        wOffset, wBestOffset;
    int         nHash;
    LPBYTE      lpbyCur, lpbyCurCand, lpbyTry;
    LPWORD      lpawFirstPas, lpawPrevPas, lpawFirstAct, lpawPrevAct;
    DWORD       dwCRC;

    // Zero out the frequency counts

    DFZeroBuf((LPBYTE)lpstDF->awLL,sizeof(lpstDF->awLL));
    DFZeroBuf((LPBYTE)lpstDF->awDist,sizeof(lpstDF->awDist));

    dwCRC = lpstDF->dwCRC;

    if (lpstDF->stHash.bActive0 == FALSE)
    {
        lpawFirstPas = lpstDF->stHash.lpawFirst0;
        lpawPrevPas = lpstDF->stHash.lpawPrev0;
        lpawFirstAct = lpstDF->stHash.lpawFirst1;
        lpawPrevAct = lpstDF->stHash.lpawPrev1;
    }
    else
    {
        lpawFirstPas = lpstDF->stHash.lpawFirst1;
        lpawPrevPas = lpstDF->stHash.lpawPrev1;
        lpawFirstAct = lpstDF->stHash.lpawFirst0;
        lpawPrevAct = lpstDF->stHash.lpawPrev0;
    }

    w = 0;
    if (lpstDF->wBlkSize < 3)
        wEnd = 0;
    else
        wEnd = (WORD)(lpstDF->wBlkSize - 2);
    lpbyCur = lpstDF->abyStrm + DF_HIST_SIZE;
    while (w < wEnd)
    {
        // Compute the hash on the next three bytes

        nHash = DF_HASH(lpbyCur);

        nBestLen = 2;
        wBestOffset = 0;

        // First try the passive hash

        wOffset = lpawFirstPas[nHash];
        nChainLen = 0;
        while (wOffset != 0 && nChainLen++ < DF_PAS_CHAIN_LEN)
        {
            lpbyTry = lpstDF->abyStrm + wOffset - 1;

            if (lpbyTry != lpbyCur)
            {
                lpbyCurCand = lpbyCur;

                wCand = w;
                nLen = 0;
                while (wCand < lpstDF->wBlkSize)
                {
                    if (*lpbyTry++ != *lpbyCurCand++)
                        break;

                    ++wCand;

                    if (++nLen == DF_MAX_LEN)
                        break;
                }

                if (nLen >= DF_MIN_LEN)
                {
                    if (nLen > nBestLen ||
                        nLen == nBestLen && wOffset > wBestOffset)
                    {
                        nBestLen = nLen;
                        wBestOffset = wOffset;
                    }
                }
            }

            wOffset = lpawPrevPas[wOffset - 1];
        }

        wOffset = lpawFirstAct[nHash];
        nChainLen = 0;
        while (wOffset != 0 && nChainLen++ < DF_ACT_CHAIN_LEN)
        {
            lpbyTry = lpstDF->abyStrm + wOffset + DF_HIST_SIZE - 1;

            if (lpbyTry != lpbyCur)
            {
                lpbyCurCand = lpbyCur;

                wCand = w;
                nLen = 0;
                while (wCand < lpstDF->wBlkSize)
                {
                    if (*lpbyTry++ != *lpbyCurCand++)
                        break;

                    ++wCand;

                    if (++nLen == DF_MAX_LEN)
                        break;
                }

                if (nLen >= DF_MIN_LEN)
                {
                    if (nLen > nBestLen ||
                        (nLen == nBestLen &&
                         (wOffset + DF_HIST_SIZE) > wBestOffset))
                    {
                        nBestLen = nLen;
                        wBestOffset = wOffset + DF_HIST_SIZE;
                    }
                }
            }

            wOffset = lpawPrevAct[wOffset - 1];
        }

        // Was a match found?

        if (wBestOffset == 0)
        {
            // Hash the BYTE

            DFHash(lpawFirstAct,
                   lpawPrevAct,
                   w,
                   lpbyCur,
                   1);

            // Set it up as a literal

            CRC32Continue(dwCRC,*lpbyCur);
            if (lpstDF->lpabyLitLen != NULL)
            {
                lpstDF->lpabyLitLen[lpstDF->wNumLitLenDistPairs] = *lpbyCur;
                lpstDF->lpawDist[lpstDF->wNumLitLenDistPairs++] = 0;
            }

            lpstDF->awLL[*lpbyCur++]++;

            ++w;
        }
        else
        {
            // Set up as a length distance pair

            DFUpdateLenDistFreq(lpstDF,
                                nBestLen,
                                w + DF_HIST_SIZE + 1 - wBestOffset);

            if (lpstDF->lpabyLitLen != NULL)
            {
                lpstDF->lpabyLitLen[lpstDF->wNumLitLenDistPairs] = nBestLen - DF_MIN_LEN;
                lpstDF->lpawDist[lpstDF->wNumLitLenDistPairs++] = w + DF_HIST_SIZE + 1 - wBestOffset;
            }

            // Insert the items into the hash table

            if (w + nBestLen + 2 >= lpstDF->wBlkSize)
            {
                // The last two bytes of the block are not inserted here

                DFHash(lpawFirstAct,
                       lpawPrevAct,
                       w,
                       lpbyCur,
                       lpstDF->wBlkSize - w - 2);
            }
            else
            {
                DFHash(lpawFirstAct,
                       lpawPrevAct,
                       w,
                       lpbyCur,
                       nBestLen);
            }

            w += nBestLen;
            while (nBestLen-- > 0)
            {
                CRC32Continue(dwCRC,*lpbyCur);
                ++lpbyCur;
            }
        }
    }


    //////////////////////////////////////////////////////////////////
    // Output the remaining bytes as literals

    while (w < lpstDF->wBlkSize)
    {
        // Still enough room?

        ++w;

        CRC32Continue(dwCRC,*lpbyCur);
        lpstDF->lpabyLitLen[lpstDF->wNumLitLenDistPairs] = *lpbyCur;
        lpstDF->lpawDist[lpstDF->wNumLitLenDistPairs++] = 0;
        lpstDF->awLL[*lpbyCur++]++;
    }

    lpstDF->dwCRC = dwCRC;

    return(TRUE);
}


//********************************************************************
//
// BOOL DFInit()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  lpvDstFile      Destination file
//  dwSize          Size of uncompressed stream
//
// Description:
//  Initializes the DF structure for compression.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL DFInit
(
    LPDF        lpstDF,
    LPVOID      lpvDstFile,
    DWORD       dwSize
)
{
    lpstDF->lpvFile = lpvDstFile;
    lpstDF->dwUncompressedSize = dwSize;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL DFDeflate()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  pfIn            The input stream
//
// Description:
//  Deflates the stream.
//
//  The output pointer should be positioned where the compressed
//  stream should be written.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL DFDeflate
(
    LPDF            lpstDF,
    LPVOID          lpvFileIn
)
{
    DWORD           dwSize, dwBlkOffset;
    WORD            w;
    DWORD           dwByteCount;
    long            lNewOffset;

    dwBlkOffset = 0;

    dwSize = lpstDF->dwUncompressedSize;
    if (dwSize == 0)
        return(TRUE);

    lpstDF->bFinalBlock = FALSE;
    lpstDF->wBlkSize = DF_HIST_SIZE;
    lpstDF->nCurBit = 0;
    lpstDF->nCurByte = 0;
    CRC32Init(lpstDF->dwCRC);
    lpstDF->dwCompressedSize = 0;
    lpstDF->stHash.bActive0 = FALSE;
    DFZeroBuf(lpstDF->abyOutBuf,DF_OUT_BUF_SIZE);
    DFZeroBuf((LPBYTE)lpstDF->stHash.lpawFirst0,DF_HASH_SIZE * sizeof(WORD));
    DFZeroBuf((LPBYTE)lpstDF->stHash.lpawPrev0,DF_HIST_SIZE * sizeof(WORD));
    DFZeroBuf((LPBYTE)lpstDF->stHash.lpawFirst1,DF_HASH_SIZE * sizeof(WORD));
    DFZeroBuf((LPBYTE)lpstDF->stHash.lpawPrev1,DF_HIST_SIZE * sizeof(WORD));
    while (dwBlkOffset < dwSize)
    {
        if (dwSize - dwBlkOffset <= DF_HIST_SIZE)
        {
            lpstDF->bFinalBlock = TRUE;
            lpstDF->wBlkSize = (WORD)(dwSize - dwBlkOffset);
        }

        // Read in the block

        if (DFFileSeek(lpstDF->lpvRootCookie,
                       lpvFileIn,
                       dwBlkOffset,
                       DF_SEEK_SET,
                       &lNewOffset) != DF_STATUS_OK)
            return(FALSE);

        if (DFFileRead(lpstDF->lpvRootCookie,
                       lpvFileIn,
                       lpstDF->abyStrm+DF_HIST_SIZE,
                       lpstDF->wBlkSize,
                       &dwByteCount) != DF_STATUS_OK ||
            dwByteCount != lpstDF->wBlkSize)
            return(FALSE);

        // If this is not the first block, then add the hash
        //  of the last two bytes of the previous block

        if (dwBlkOffset != 0)
        {
            if (lpstDF->stHash.bActive0 == FALSE)
            {
                DFHash(lpstDF->stHash.lpawFirst1,
                       lpstDF->stHash.lpawPrev1,
                       DF_HIST_SIZE - 2,
                       lpstDF->abyStrm + DF_HIST_SIZE - 2,
                       2);
                lpstDF->stHash.bActive0 = TRUE;
                DFZeroBuf((LPBYTE)lpstDF->stHash.lpawFirst0,
                          DF_HASH_SIZE * sizeof(WORD));
                DFZeroBuf((LPBYTE)lpstDF->stHash.lpawPrev0,
                          DF_HIST_SIZE * sizeof(WORD));
            }
            else
            {
                DFHash(lpstDF->stHash.lpawFirst0,
                       lpstDF->stHash.lpawPrev0,
                       DF_HIST_SIZE - 2,
                       lpstDF->abyStrm + DF_HIST_SIZE - 2,
                       2);
                lpstDF->stHash.bActive0 = FALSE;
                DFZeroBuf((LPBYTE)lpstDF->stHash.lpawFirst1,
                          DF_HASH_SIZE * sizeof(WORD));
                DFZeroBuf((LPBYTE)lpstDF->stHash.lpawPrev1,
                          DF_HIST_SIZE * sizeof(WORD));
            }
        }

        // Make a copy of the current state of the hash table

        lpstDF->wNumLitLenDistPairs = 0;

        // Get the stats from deflating the block

        DFDeflateBlock(lpstDF);
        DFBuildTrees(lpstDF);

        if ((lpstDF->dwCompressedLen >> 3) < lpstDF->wBlkSize)
        {
            if (DFOutputDeflateHdr(lpstDF) == FALSE)
                return(FALSE);

            for (w=0;w<lpstDF->wNumLitLenDistPairs;w++)
            {
                if (lpstDF->lpawDist[w] == 0)
                {
                    // Literal

                    if (DFOutputLitLenDist(lpstDF,
                                           lpstDF->lpabyLitLen[w],
                                           0) == FALSE)
                        return(FALSE);
                }
                else
                {
                    // Length/distance

                    if (DFOutputLitLenDist(lpstDF,
                                           lpstDF->lpabyLitLen[w] + 3,
                                           lpstDF->lpawDist[w]) == FALSE)
                        return(FALSE);
                }
            }

            // Output the block terminator

            if (DFOutputLitLenDist(lpstDF,
                                   256,
                                   0) == FALSE)
                return(FALSE);
        }
        else
        {
            // Output as a literal block

            if (DFOutputNoCompressHdr(lpstDF,
                                      lpstDF->wBlkSize) == FALSE)
                return(FALSE);

            if (DFFileWrite(lpstDF->lpvRootCookie,
                            lpstDF->lpvFile,
                            lpstDF->abyStrm+DF_HIST_SIZE,
                            lpstDF->wBlkSize,
                            &dwByteCount) != DF_STATUS_OK ||
                dwByteCount != lpstDF->wBlkSize)
                return(FALSE);
        }

        // Copy the block from the second half to the first half

        DFCopyBuf(lpstDF->abyStrm,lpstDF->abyStrm+DF_HIST_SIZE,DF_HIST_SIZE);

        dwBlkOffset += lpstDF->wBlkSize;

        if (DFProgress(lpstDF->lpvRootCookie,
                       dwBlkOffset) != DF_STATUS_OK)
            return(FALSE);
    }

    if (DFOutputFlush(lpstDF) == FALSE)
        return(FALSE);

    return(TRUE);
}

