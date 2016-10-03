//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/o97lznt.cpv   1.8   08 Dec 1998 12:36:42   DCHI  $
//
// Description:
//  Source file containing functions for providing decompressed stream
//  access to a LZNT compressed stream.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/o97lznt.cpv  $
// 
//    Rev 1.8   08 Dec 1998 12:36:42   DCHI
// Improved FindMatch() by changing function to search backwards
// and to allow overlapping matches.
// 
//    Rev 1.7   12 Oct 1998 13:11:52   DCHI
// Added NLM relinquish control calls to LZNTWriteCompressed().
// 
//    Rev 1.6   13 Jul 1998 11:09:44   DCHI
// Added support for Office 9 modules.
// 
//    Rev 1.5   10 Jun 1998 12:40:52   DCHI
// Changed DecompressInit() to return immediately on error and to set
// the size to the number of bytes that have been decompressed so far.
// 
//    Rev 1.4   14 Aug 1997 14:36:12   DCHI
// Fixed boundary problem with DecompressContinue().
// 
//    Rev 1.3   26 Jun 1997 16:34:40   DCHI
// Removed SYM_NLM ifdefs around CopyCompress().
// 
//    Rev 1.2   24 Jun 1997 14:02:42   DCHI
// Corrected problem with getting bytes when the bytes straddled across
// the buffer boundary.
// 
//    Rev 1.1   12 Jun 1997 12:40:58   DCHI
// Fixed problem with DecompressContinue() in which a new chunk would
// be read before the back referenced bytes of the previous chunk were done.
// 
//    Rev 1.0   05 May 1997 14:26:50   DCHI
// Initial revision.
// 
//************************************************************************

#include "o97api.h"

//*************************************************************************
//
// BOOL LZNTAllocStruct()
//
// Parameters:
//  lpvRootCookie   Root cookie for memory allocation
//  lplpstLZNT      Ptr to ptr to store ptr to allocated LZNT structure
//
// Description:
//  Allocates memory for the LZNT structure.
//
// Returns:
//  TRUE            If the allocation succeeded
//  FALSE           If the allocation failed
//
//*************************************************************************

BOOL LZNTAllocStruct
(
    LPVOID          lpvRootCookie,
    LPLPLZNT        lplpstLZNT
)
{
    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(LZNT_T),
                      (LPLPVOID)lplpstLZNT) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL LZNTFreeStruct()
//
// Parameters:
//  lpvRootCookie   Root cookie for memory allocation
//  lpstLZNT        Ptr to LZNT structure to free
//
// Description:
//  Frees memory allocated for the given LZNT structure.
//
// Returns:
//  TRUE            If the free succeeded
//  FALSE           If the free failed
//
//*************************************************************************

BOOL LZNTFreeStruct
(
    LPVOID          lpvRootCookie,
    LPLZNT          lpstLZNT
)
{
    if (SSMemoryFree(lpvRootCookie,
                     lpstLZNT) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//
//  The Copy token is two bytes in size.
//
//  Copy Token
//
//         low bits            high bits
//          Length            Displacement
//
//      12 bits 3 to 4098    4 bits 1 to 16
//      11 bits 3 to 2050    5 bits 1 to 32
//      10 bits 3 to 1026    6 bits 1 to 64
//       9 bits 3 to 514     7 bits 1 to 128
//       8 bits 3 to 258     8 bits 1 to 256
//       7 bits 3 to 130     9 bits 1 to 512
//       6 bits 3 to 66     10 bits 1 to 1024
//       5 bits 3 to 34     11 bits 1 to 2048
//       4 bits 3 to 18     12 bits 1 to 4096
//

#define FORMAT412 0
#define FORMAT511 1
#define FORMAT610 2
#define FORMAT79  3
#define FORMAT88  4
#define FORMAT97  5
#define FORMAT106 6
#define FORMAT115 7
#define FORMAT124 8

//                                      4/12  5/11  6/10   7/9   8/8   9/7  10/6  11/5  12/4

WORD gwLZNTFormatMaxLength[]       = { 4098, 2050, 1026,  514,  258,  130,   66,   34,   18 };
WORD gwLZNTFormatMaxDisplacement[] = {   16,   32,   64,  128,  256,  512, 1024, 2048, 4096 };
WORD gwLZNTCopyLengthMask[] =        {0xFFF,0x7FF,0x3FF,0x1FF,0x0FF,0x07F,0x03F,0x01F,0x00F };
WORD gwLZNTCopyDisplacementShift[] = {   12,   11,   10,    9,    8,    7,    6,    5,    4 };


//*************************************************************************
//
// int LZNTBufGetBYTE()
//
// Parameters:
//  lpstBuf         Ptr to the LZNT buffer structure
//
// Description:
//  This function returns the next byte in the compressed stream.
//  The function will reload the buffer if it becomes empty.
//
// Returns:
//  -1      If there is an error reading
//  BYTE    The value of the next byte
//
//*************************************************************************

int LZNTBufGetBYTE
(
    LPLZNT_BUF  lpstBuf
)
{
    if (lpstBuf->nNextByteI == lpstBuf->nNumBytesInBuf)
    {
        int     nNumBytesToRead;
        DWORD   dwNumBytesLeftInStream;
        DWORD   dwBytesRead;

        // Calculate how many bytes to read into the buffer

        dwNumBytesLeftInStream =
            lpstBuf->dwEndReadOffsetPlus1 - lpstBuf->dwNextReadOffset;

        if (dwNumBytesLeftInStream < LZNT_BUF_SIZE)
        {
            if (dwNumBytesLeftInStream == 0)
                return(-1);

            nNumBytesToRead = (int)dwNumBytesLeftInStream;
        }
        else
            nNumBytesToRead = LZNT_BUF_SIZE;

        // Fill the buffer

        if (SSSeekRead(lpstBuf->lpstStream,
                       lpstBuf->dwNextReadOffset,
                       lpstBuf->abyBuf,
                       (DWORD)nNumBytesToRead,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != (DWORD)nNumBytesToRead)
        {
            // Error reading

            return(-1);
        }

        // Update the fields

        lpstBuf->dwNextReadOffset += nNumBytesToRead;
        lpstBuf->nNumBytesInBuf = nNumBytesToRead;
        lpstBuf->nNextByteI = 0;
    }

    return(lpstBuf->abyBuf[lpstBuf->nNextByteI++]);
}


//*************************************************************************
//
// DWORD LZNTDecompressInit2()
//
// Parameters:
//  lpstLZNT        Ptr to the LZNT state structure
//  bIsRestart      If FALSE, then this is an initialization
//                  If TRUE, then it is just a read of the first
//                      LZNT_MAX_COPY_DISPLACEMENT bytes
//  lpdwEndOffset   Ptr to DWORD for end offset of data.
//                      Only valid if bIsRestart == FALSE
//
// Description:
//  If bIsRestart == FALSE, this function reads and buffers the
//  first LZNT_MAX_COPY_DISPLACEMENT bytes of the stream starting
//  from the offset given in lpstLZNT->dwFirstChunkOffset.
//  The function then continues parsing the stream to determine
//  the total number of bytes in the stream, storing the result
//  in lpstLZNT->dwSize.
//
//  If bIsRestart == TRUE, this function reads and buffers the
//  first LZNT_MAX_COPY_DISPLACEMENT bytes of the stream.
//
//  In both cases, the state variables of lpstLZNT are set so that
//  reading can be resumed from the LZNT_MAX_COPY_DISPLACEMENT + 1st
//  byte.
//
//  This function should never be called with bIsRestart == TRUE
//  if the size of the stream (e.g., lpstLZNT->dwSize) is less than
//  LZNT_MAX_COPY_DISPLACEMENT bytes.
//
// Returns:
//  LZNT_MAX_COPY_DISPLACEMENT  On success if bIsRestart == TRUE
//  dwSize      Number of bytes in stream if bIsRestart == FALSE
//
//*************************************************************************

DWORD LZNTDecompressInit2
(
    LPLZNT      lpstLZNT,
    BOOL        bIsRestart,
    LPDWORD     lpdwEndOffset
)
{
    WORD        w;
    WORD        wChunkHdr;
    WORD        wCopyDescriptor;
    int         nCopyLen;
    int         nCopyDisplacement;
    LZNT_BUF_T  stBuf;

    DWORD       dwSize;
    BYTE        byFlag;
    int         nFlagBit;
    int         nFormat;
    int         nChunkBytesLeft;
    BOOL        bChunkIsCompressed;
    WORD        wCopyMaxDisplacement;
    DWORD       dwNumUncompressedChunkBytes;
    DWORD       dwBytesRead;

    // Initialize the LZNT structure

    lpstLZNT->i = 0;
    lpstLZNT->dwOffsetAtI = 0;
    lpstLZNT->nCopyLen = 0;
    lpstLZNT->lpbyNextDst = lpstLZNT->abyUncompressed;

    // Initialize default value for end offset

    if (lpdwEndOffset != NULL)
        *lpdwEndOffset = SSStreamLen(lpstLZNT->stLZNTBuf.lpstStream);

    // Check for no compression

    if (lpstLZNT->bCompressed == FALSE)
    {
        lpstLZNT->dwSize = SSStreamLen(lpstLZNT->stLZNTBuf.lpstStream) -
            lpstLZNT->dwFirstChunkOffset;

        if (lpstLZNT->dwSize > LZNT_MAX_COPY_DISPLACEMENT)
            nCopyLen = LZNT_MAX_COPY_DISPLACEMENT;
        else
            nCopyLen = (int)(lpstLZNT->dwSize);

        // Now read in the bytes

        if (SSSeekRead(lpstLZNT->stLZNTBuf.lpstStream,
                       lpstLZNT->dwFirstChunkOffset,
                       lpstLZNT->abyUncompressed,
                       (DWORD)nCopyLen,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != (DWORD)nCopyLen)
        {
            // Error reading

            lpstLZNT->dwSize = 0;
            return(0);
        }

        return(lpstLZNT->dwSize);
    }

    // Initialize the read buffer

    stBuf.lpstStream = lpstLZNT->stLZNTBuf.lpstStream;
    stBuf.nNumBytesInBuf = 0;
    stBuf.nNextByteI = 0;
    stBuf.dwNextReadOffset = lpstLZNT->dwFirstChunkOffset;
    stBuf.dwEndReadOffsetPlus1 =
        SSStreamLen(lpstLZNT->stLZNTBuf.lpstStream);

    dwSize = 0;

    /////////////////////////////////////////////////////////////
    // Get the first MAX_LZNT_COPY_DISPLACEMENT bytes
    /////////////////////////////////////////////////////////////

    // This outermost loop iterates through the compressed chunks

    lpstLZNT->nFormat = FORMAT412;
    while (1)
    {
        // Get the chunk header

        wChunkHdr = (WORD)LZNTBufGetBYTE(&stBuf);
        if (wChunkHdr == 0xFFFF)
            break;

        w = (WORD)LZNTBufGetBYTE(&stBuf);
        if (w == 0xFFFF)
            break;

        wChunkHdr |= w << 8;

        // No more chunks?

        if (wChunkHdr == 0)
            break;

        // Find out the chunk size and compressed fields

        nChunkBytesLeft = (wChunkHdr & 0x0FFF) + 1;

        if ((wChunkHdr & 0x1000) == 0)
        {
            bChunkIsCompressed = FALSE;

            while (nChunkBytesLeft > 0)
            {
                w = (WORD)LZNTBufGetBYTE(&stBuf);
                if (w == 0xFFFF)
                    break;

                nChunkBytesLeft--;
                dwNumUncompressedChunkBytes++;

                if (dwSize < LZNT_MAX_COPY_DISPLACEMENT)
                {
                    lpstLZNT->abyUncompressed[dwSize] = (BYTE)w;
                    if (dwSize == (LZNT_MAX_COPY_DISPLACEMENT - 1))
                    {
                        // Save the current state

                        lpstLZNT->bChunkIsCompressed = bChunkIsCompressed;
                        lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;

                        lpstLZNT->dwNumUncompressedChunkBytes =
                            dwNumUncompressedChunkBytes;

                        lpstLZNT->stLZNTBuf = stBuf;

                        if (bIsRestart == TRUE)
                            return(LZNT_MAX_COPY_DISPLACEMENT);
                    }
                }

                ++dwSize;
            }

            if (w == 0xFFFF)
                break;

            continue;
        }
        else
            bChunkIsCompressed = TRUE;

        // Iterate through the chunk components

        nFlagBit = 8;
        nFormat = FORMAT412;
        wCopyMaxDisplacement = gwLZNTFormatMaxDisplacement[FORMAT412];
        dwNumUncompressedChunkBytes = 0;
        while (nChunkBytesLeft > 0)
        {
            if (nFlagBit == 8)
            {
                // Get a new flag byte

                w = (WORD)LZNTBufGetBYTE(&stBuf);
                if (w == 0xFFFF)
                    break;

                byFlag = (BYTE)w;
                nChunkBytesLeft--;

                nFlagBit = 0;
                continue;
            }

            // If the bit is zero, then the next byte comes directly
            //  from the compressed stream

            if ((byFlag & (1 << nFlagBit++)) == 0)
            {
                // Literal byte

                w = (WORD)LZNTBufGetBYTE(&stBuf);
                if (w == 0xFFFF)
                    break;

                nChunkBytesLeft--;
                dwNumUncompressedChunkBytes++;

                if (dwSize < LZNT_MAX_COPY_DISPLACEMENT)
                {
                    lpstLZNT->abyUncompressed[dwSize] = (BYTE)w;
                    if (dwSize == (LZNT_MAX_COPY_DISPLACEMENT - 1))
                    {
                        // Save the current state

                        lpstLZNT->bChunkIsCompressed = bChunkIsCompressed;
                        lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;

                        lpstLZNT->nFlagBit = nFlagBit;
                        lpstLZNT->byFlag = byFlag;
                        lpstLZNT->nFormat = nFormat;
                        lpstLZNT->dwNumUncompressedChunkBytes =
                            dwNumUncompressedChunkBytes;

                        lpstLZNT->stLZNTBuf = stBuf;

                        if (bIsRestart == TRUE)
                            return(LZNT_MAX_COPY_DISPLACEMENT);
                    }
                }

                ++dwSize;
            }

            // If the bit is non-zero, then the next byte comes
            //  from somewhere in the already decompressed bytes

            else
            {
                // Get the copy descriptor

                wCopyDescriptor = (WORD)LZNTBufGetBYTE(&stBuf);
                if (wCopyDescriptor == 0xFFFF)
                    break;

                w = (WORD)LZNTBufGetBYTE(&stBuf);
                if (w == 0xFFFF)
                    break;

                wCopyDescriptor |= w << 8;

                nChunkBytesLeft -= 2;

                if (nFormat != FORMAT124 &&
                    wCopyMaxDisplacement <
                    dwNumUncompressedChunkBytes)
                {
                    // Update maximum copy displacement and length

                    do
                    {
                        if (++nFormat == FORMAT124)
                            break;
                    }
                    while (gwLZNTFormatMaxDisplacement[nFormat] <
                        dwNumUncompressedChunkBytes);

                    wCopyMaxDisplacement =
                        gwLZNTFormatMaxDisplacement[nFormat];
                }

                // Get the length

                nCopyLen = 3 +
                    (wCopyDescriptor & gwLZNTCopyLengthMask[nFormat]);
                nCopyDisplacement = 1 +
                    (wCopyDescriptor >> gwLZNTCopyDisplacementShift[nFormat]);

                if (dwSize < LZNT_MAX_COPY_DISPLACEMENT)
                {
                    int     nNumBytesToCopy;
                    LPBYTE  lpbySrc;
                    LPBYTE  lpbyDst;

                    if (nCopyDisplacement > (int)dwSize)
                    {
                        // Error in compressed stream

                        if (bIsRestart == FALSE)
                        {
                            if (lpdwEndOffset != NULL)
                                *lpdwEndOffset = stBuf.dwNextReadOffset -
                                    (stBuf.nNumBytesInBuf - stBuf.nNextByteI);

                            return(lpstLZNT->dwSize = dwSize);
                        }

                        break;
                    }

                    lpbySrc = lpstLZNT->abyUncompressed + dwSize -
                        nCopyDisplacement;
                    lpbyDst = lpstLZNT->abyUncompressed + dwSize;

                    if (dwSize + nCopyLen >= LZNT_MAX_COPY_DISPLACEMENT)
                    {
                        nNumBytesToCopy =
                            (int)(LZNT_MAX_COPY_DISPLACEMENT - dwSize);

                        // Save the current state

                        lpstLZNT->bChunkIsCompressed = bChunkIsCompressed;
                        lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;

                        lpstLZNT->nFlagBit = nFlagBit;
                        lpstLZNT->byFlag = byFlag;
                        lpstLZNT->nFormat = nFormat;
                        lpstLZNT->dwNumUncompressedChunkBytes =
                            dwNumUncompressedChunkBytes +
                            nNumBytesToCopy;

                        lpstLZNT->stLZNTBuf = stBuf;

                        while (nNumBytesToCopy-- > 0)
                            *lpbyDst++ = *lpbySrc++;

                        if (bIsRestart == TRUE)
                            return(LZNT_MAX_COPY_DISPLACEMENT);
                    }
                    else
                    {
                        nNumBytesToCopy = nCopyLen;

                        while (nNumBytesToCopy-- > 0)
                            *lpbyDst++ = *lpbySrc++;
                    }
                }

                dwSize += nCopyLen;
                dwNumUncompressedChunkBytes += nCopyLen;
            }
        }
    }

    if (bIsRestart == FALSE)
    {
        if (lpdwEndOffset != NULL)
        {
            *lpdwEndOffset = stBuf.dwNextReadOffset -
                (stBuf.nNumBytesInBuf - stBuf.nNextByteI);
        }

        lpstLZNT->dwSize = dwSize;
    }

    return(dwSize);
}


//*************************************************************************
//
// int LZNTDecompressContinue()
//
// Parameters:
//  lpstLZNT        Ptr to the LZNT state structure
//  nNumBytes       Number of bytes to seek forward
//
// Description:
//  This function should never be called if lpstLZNT->dwSize is less
//  than LZNT_MAX_COPY_DISPLACEMENT or if the number of bytes left
//  in the stream is less than nNumBytes or if nNumBytes == 0.
//  In other words, the caller is responsible for not requesting bytes
//  that extend beyond the end of the stream or requesting zero bytes.
//
//  The function continues reading nNumBytes from the stream starting
//  from the point where the last call to this function or the last
//  call to LNZTDecompressInit() left off.
//
//  After reading the given number of bytes, the function sets the
//  state variables of lpstLZNT so that further calls to this function
//  will continue reading appropriately from the last byte.
//
// Returns:
//  nNumBytes   The number of bytes successfully continued further
//              in lpstLZNT's buffer.
//  -1          On error (i.e., failure in reading or fewer than
//                  nNumBytes left in the stream)
//
//*************************************************************************

int LZNTDecompressContinue
(
    LPLZNT  lpstLZNT,
    int     nNumBytes
)
{
    WORD        w;
    WORD        wChunkHdr;
    WORD        wCopyDescriptor;
    int         i;
    int         nCopyLen;
    int         nCopyDisplacement;
    LPLZNT_BUF  lpstBuf;
    int         nNumBytesToCopy;
    int         nNumBytesLeft;
    int         nSrcI;

    LPBYTE      lpbySrc;
    LPBYTE      lpbyDst;
    LPBYTE      lpbyLimit;

    BYTE        byFlag;
    int         nFlagBit;
    int         nFormat;
    int         nChunkBytesLeft;
    BOOL        bChunkIsCompressed;
    WORD        wCopyMaxDisplacement;
    DWORD       dwNumUncompressedChunkBytes;

    /////////////////////////////////////////////////////////////
    // Non-compressed stream continuation
    /////////////////////////////////////////////////////////////

    if (lpstLZNT->bCompressed == FALSE)
    {
        DWORD   dwStartOffset;
        DWORD   dwBytesRead;

        i = lpstLZNT->i;

        lpstLZNT->i += nNumBytes;
        if (lpstLZNT->i >= LZNT_MAX_COPY_DISPLACEMENT)
            lpstLZNT->i %= LZNT_MAX_COPY_DISPLACEMENT;

        dwStartOffset = lpstLZNT->dwOffsetAtI +
            LZNT_MAX_COPY_DISPLACEMENT;
        lpstLZNT->dwOffsetAtI += nNumBytes;

        if (lpstLZNT->dwSize > LZNT_MAX_COPY_DISPLACEMENT)
            nCopyLen = LZNT_MAX_COPY_DISPLACEMENT;
        else
            nCopyLen = (int)(lpstLZNT->dwSize);

        // Now read in the bytes

        if (i + nNumBytes > LZNT_MAX_COPY_DISPLACEMENT)
            nCopyLen = LZNT_MAX_COPY_DISPLACEMENT - i;
        else
            nCopyLen = nNumBytes;

        if (SSSeekRead(lpstLZNT->stLZNTBuf.lpstStream,
                       dwStartOffset,
                       lpstLZNT->abyUncompressed + i,
                       (DWORD)nCopyLen,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != (DWORD)nCopyLen)
        {
            // Error reading

            return(-1);
        }

        if (i + nNumBytes > LZNT_MAX_COPY_DISPLACEMENT)
        {
            // Copy the remaining bytes

            nCopyLen = nNumBytes - nCopyLen;

            if (SSSeekRead(lpstLZNT->stLZNTBuf.lpstStream,
                           dwStartOffset + i,
                           lpstLZNT->abyUncompressed,
                           (DWORD)nCopyLen,
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != (DWORD)nCopyLen)
            {
                // Error reading

                return(-1);
            }
        }

        return(nNumBytes);
    }

    /////////////////////////////////////////////////////////////
    // Compressed stream continuation
    /////////////////////////////////////////////////////////////

    // This outermost loop iterates through the compressed chunks

    nNumBytesLeft = nNumBytes;
    nChunkBytesLeft = lpstLZNT->nChunkBytesLeft;
    lpbyDst = lpstLZNT->lpbyNextDst;
    lpstBuf = &lpstLZNT->stLZNTBuf;
    lpbyLimit = lpstLZNT->abyUncompressed + LZNT_MAX_COPY_DISPLACEMENT;
    while (1)
    {
        if (nChunkBytesLeft <= 0 && lpstLZNT->nCopyLen == 0)
        {
            // Get the chunk header

            wChunkHdr = (WORD)LZNTBufGetBYTE(lpstBuf);
            if (wChunkHdr == 0xFFFF)
            {
                // This is an error condition
                return(-1);
            }

            w = (WORD)LZNTBufGetBYTE(lpstBuf);
            if (w == 0xFFFF)
            {
                // This is an error condition
                return(-1);
            }

            wChunkHdr |= w << 8;

            // No more chunks?

            if (wChunkHdr == 0)
            {
                // We should never reach this point
                return(-1);
            }

            // Find out the chunk size and compressed fields

            nChunkBytesLeft = (wChunkHdr & 0x0FFF) + 1;
            if ((wChunkHdr & 0x1000) == 0)
                bChunkIsCompressed = FALSE;
            else
                bChunkIsCompressed = TRUE;

            // Iterate through the chunk components

            nFlagBit = 8;
            nFormat = FORMAT412;

            wCopyMaxDisplacement = gwLZNTFormatMaxDisplacement[FORMAT412];
            dwNumUncompressedChunkBytes = 0;
        }
        else
        {
            // Resume from last

            dwNumUncompressedChunkBytes =
                lpstLZNT->dwNumUncompressedChunkBytes;

            // Were there more bytes left to copy?

            if (lpstLZNT->nCopyLen > 0)
            {
                if (lpstLZNT->nCopyLen > nNumBytesLeft)
                    nNumBytesToCopy = nNumBytesLeft;
                else
                    nNumBytesToCopy = lpstLZNT->nCopyLen;

                // The displacement is assumed to be valid

                nSrcI = (lpbyDst - lpstLZNT->abyUncompressed) -
                    lpstLZNT->nCopyDisplacement;
                if (nSrcI < 0)
                    nSrcI += LZNT_MAX_COPY_DISPLACEMENT;

                lpbySrc = lpstLZNT->abyUncompressed + nSrcI;

                // Copy the bytes

                i = nNumBytesToCopy;
                while (i-- > 0)
                {
                    *lpbyDst++ = *lpbySrc++;

                    if (lpbyDst == lpbyLimit)
                        lpbyDst = lpstLZNT->abyUncompressed;

                    if (lpbySrc == lpbyLimit)
                        lpbySrc = lpstLZNT->abyUncompressed;
                }

                // Update the number of uncompressed bytes
                //  in this chunk

                dwNumUncompressedChunkBytes += nNumBytesToCopy;

                lpstLZNT->nCopyLen -= nNumBytesToCopy;
                if (lpstLZNT->nCopyLen != 0)
                {
                    // Save the current state and return

                    lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;
                    lpstLZNT->lpbyNextDst = lpbyDst;
                    lpstLZNT->dwNumUncompressedChunkBytes =
                        dwNumUncompressedChunkBytes;

                    lpstLZNT->i += nNumBytes;
                    if (lpstLZNT->i >= LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= LZNT_MAX_COPY_DISPLACEMENT;
                    lpstLZNT->dwOffsetAtI += nNumBytes;
                    return(nNumBytes);
                }

                // Update the number of bytes left

                nNumBytesLeft -= nNumBytesToCopy;
                if (nNumBytesLeft == 0)
                {
                    // Save the current state and return

                    lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;
                    lpstLZNT->lpbyNextDst = lpbyDst;
                    lpstLZNT->dwNumUncompressedChunkBytes =
                        dwNumUncompressedChunkBytes;

                    lpstLZNT->i += nNumBytes;
                    if (lpstLZNT->i >= LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= LZNT_MAX_COPY_DISPLACEMENT;
                    lpstLZNT->dwOffsetAtI += nNumBytes;
                    return(nNumBytes);
                }
            }

            // At this point, there are still more bytes to copy,
            //  so restore the necessary state variables

            bChunkIsCompressed = lpstLZNT->bChunkIsCompressed;
            nFlagBit = lpstLZNT->nFlagBit;
            byFlag = lpstLZNT->byFlag;
            nFormat = lpstLZNT->nFormat;

            wCopyMaxDisplacement = gwLZNTFormatMaxDisplacement[nFormat];
        }

        /////////////////////////////////////////////////////////
        // Compressed chunk continuation
        /////////////////////////////////////////////////////////

        if (bChunkIsCompressed == FALSE)
        {
            while (nChunkBytesLeft > 0)
            {
                w = (WORD)LZNTBufGetBYTE(lpstBuf);
                if (w == 0xFFFF)
                    break;

                *lpbyDst++ = (BYTE)w;
                if (lpbyDst == lpbyLimit)
                    lpbyDst = lpstLZNT->abyUncompressed;

                nChunkBytesLeft--;
                dwNumUncompressedChunkBytes++;

                if (--nNumBytesLeft == 0)
                {
                    // Save the current state

                    lpstLZNT->bChunkIsCompressed = bChunkIsCompressed;
                    lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;
                    lpstLZNT->lpbyNextDst = lpbyDst;

                    lpstLZNT->dwNumUncompressedChunkBytes =
                        dwNumUncompressedChunkBytes;

                    lpstLZNT->i += nNumBytes;
                    if (lpstLZNT->i >= LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= LZNT_MAX_COPY_DISPLACEMENT;
                    lpstLZNT->dwOffsetAtI += nNumBytes;
                    return(nNumBytes);
                }
            }

            if (w == 0xFFFF)
            {
                // This is an error condition
                return(-1);
            }

            continue;
        }

        /////////////////////////////////////////////////////////
        // Non-compressed chunk continuation
        /////////////////////////////////////////////////////////

        while (nChunkBytesLeft > 0)
        {
            if (nFlagBit == 8)
            {
                // Get a new flag byte

                w = (WORD)LZNTBufGetBYTE(lpstBuf);
                if (w == 0xFFFF)
                {
                    // This is an error condition
                    return(-1);
                }

                byFlag = (BYTE)w;
                nChunkBytesLeft--;

                nFlagBit = 0;
                continue;
            }

            // If the bit is zero, then the next byte comes directly
            //  from the compressed stream

            if ((byFlag & (1 << nFlagBit++)) == 0)
            {
                // Literal byte

                w = (WORD)LZNTBufGetBYTE(lpstBuf);
                if (w == 0xFFFF)
                {
                    // This is an error condition
                    return(-1);
                }

                *lpbyDst++ = (BYTE)w;
                if (lpbyDst == lpbyLimit)
                    lpbyDst = lpstLZNT->abyUncompressed;

                nChunkBytesLeft--;
                dwNumUncompressedChunkBytes++;

                // Update the number of bytes left

                if (--nNumBytesLeft == 0)
                {
                    // Save the current state

                    lpstLZNT->bChunkIsCompressed = bChunkIsCompressed;
                    lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;
                    lpstLZNT->lpbyNextDst = lpbyDst;

                    lpstLZNT->nFlagBit = nFlagBit;
                    lpstLZNT->byFlag = byFlag;
                    lpstLZNT->nFormat = nFormat;
                    lpstLZNT->dwNumUncompressedChunkBytes =
                        dwNumUncompressedChunkBytes;

                    lpstLZNT->i += nNumBytes;
                    if (lpstLZNT->i >= LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= LZNT_MAX_COPY_DISPLACEMENT;
                    lpstLZNT->dwOffsetAtI += nNumBytes;
                    return(nNumBytes);
                }
            }

            // If the bit is non-zero, then the next byte comes
            //  from somewhere in the already decompressed bytes

            else
            {
                // Get the copy descriptor

                wCopyDescriptor = (WORD)LZNTBufGetBYTE(lpstBuf);
                if (wCopyDescriptor == 0xFFFF)
                {
                    // This is an error condition
                    return(-1);
                }

                w = (WORD)LZNTBufGetBYTE(lpstBuf);
                if (w == 0xFFFF)
                {
                    // This is an error condition
                    return(-1);
                }

                wCopyDescriptor |= w << 8;

                nChunkBytesLeft -= 2;

                if (nFormat != FORMAT124 &&
                    wCopyMaxDisplacement <
                    dwNumUncompressedChunkBytes)
                {
                    // Update maximum copy displacement and length

                    do
                    {
                        if (++nFormat == FORMAT124)
                            break;
                    }
                    while (gwLZNTFormatMaxDisplacement[nFormat] <
                        dwNumUncompressedChunkBytes);

                    wCopyMaxDisplacement =
                        gwLZNTFormatMaxDisplacement[nFormat];
                }

                // Get the length

                nCopyLen = 3 +
                    (wCopyDescriptor & gwLZNTCopyLengthMask[nFormat]);
                nCopyDisplacement = 1 +
                    (wCopyDescriptor >> gwLZNTCopyDisplacementShift[nFormat]);

                // Limit the number of bytes to copy

                if (nCopyLen > nNumBytesLeft)
                    nNumBytesToCopy = nNumBytesLeft;
                else
                    nNumBytesToCopy = nCopyLen;

                // The displacement is assumed to be valid

                nSrcI = (lpbyDst - lpstLZNT->abyUncompressed) -
                    nCopyDisplacement;

                if (nSrcI < 0)
                    nSrcI += LZNT_MAX_COPY_DISPLACEMENT;

                lpbySrc = lpstLZNT->abyUncompressed + nSrcI;

                // Copy the bytes

                i = nNumBytesToCopy;
                while (i-- > 0)
                {
                    *lpbyDst++ = *lpbySrc++;

                    if (lpbyDst == lpbyLimit)
                        lpbyDst = lpstLZNT->abyUncompressed;

                    if (lpbySrc == lpbyLimit)
                        lpbySrc = lpstLZNT->abyUncompressed;
                }

                // Update the number of uncompressed bytes
                //  in this chunk

                dwNumUncompressedChunkBytes += nNumBytesToCopy;

                nCopyLen -= nNumBytesToCopy;

                if (nCopyLen != 0)
                {
                    // Save the current state and return

                    lpstLZNT->nCopyLen = nCopyLen;
                    lpstLZNT->nCopyDisplacement = nCopyDisplacement;

                    lpstLZNT->bChunkIsCompressed = bChunkIsCompressed;
                    lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;
                    lpstLZNT->lpbyNextDst = lpbyDst;

                    lpstLZNT->nFlagBit = nFlagBit;
                    lpstLZNT->byFlag = byFlag;
                    lpstLZNT->nFormat = nFormat;
                    lpstLZNT->dwNumUncompressedChunkBytes =
                        dwNumUncompressedChunkBytes;

                    lpstLZNT->i += nNumBytes;
                    if (lpstLZNT->i >= LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= LZNT_MAX_COPY_DISPLACEMENT;
                    lpstLZNT->dwOffsetAtI += nNumBytes;
                    return(nNumBytes);
                }

                // Update the number of bytes left

                nNumBytesLeft -= nNumBytesToCopy;
                if (nNumBytesLeft == 0)
                {
                    // Save the current state and return

                    lpstLZNT->bChunkIsCompressed = bChunkIsCompressed;
                    lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;
                    lpstLZNT->lpbyNextDst = lpbyDst;

                    lpstLZNT->nFlagBit = nFlagBit;
                    lpstLZNT->byFlag = byFlag;
                    lpstLZNT->nFormat = nFormat;
                    lpstLZNT->dwNumUncompressedChunkBytes =
                        dwNumUncompressedChunkBytes;

                    lpstLZNT->i += nNumBytes;
                    if (lpstLZNT->i >= LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= LZNT_MAX_COPY_DISPLACEMENT;
                    lpstLZNT->dwOffsetAtI += nNumBytes;
                    return(nNumBytes);
                }
            }
        }
    }
}


//*************************************************************************
//
// int LZNTGetBytes()
//
// Parameters:
//  lpstLZNT        Ptr to the LZNT state structure
//  dwOffset        Offset in stream from which to get the bytes
//  nNumBytes       Number of bytes to seek forward
//                      This should never be greater than
//                      LZNT_MAX_COPY_DISPLACEMENT.
//  lpbyDst         The buffer in which to store the bytes
//
// Description:
//  This function stores into lpbyDst up to nNumBytes from the
//  given offset.
//
//  The number of bytes requested should never be greater than
//  LZNT_MAX_COPY_DISPLACEMENT because the function readjusts
//  the contents of the buffer in lpstLZNT so that all the bytes
//  requested lie within the buffer before the bytes are copied
//  to the destination.
//
//  If the requested offset lies before the first byte of the
//  buffer, then a restart is performed first.  The restart
//  is potentially very expensive.
//
// Returns:
//  nNumBytes   The number of bytes successfully gotten
//  -1          On error (i.e., failure in reading or requested offset
//                  is past end of stream)
//
//*************************************************************************

int LZNTGetBytes
(
    LPLZNT          lpstLZNT,
    DWORD           dwOffset,
    int             nNumBytes,
    LPBYTE          lpbyDst
)
{
    int             nSrcI;
    int             nNumBytesToCopy;
    LPBYTE          lpbySrc;
    LPBYTE          lpbyLimit;

    // Don't continue if no bytes are to be read

    if (nNumBytes == 0)
        return 0;

    // Limit read to the number of bytes in the stream

    if (dwOffset + nNumBytes > lpstLZNT->dwSize)
    {
        // Can not read pass the size of the stream

        if (dwOffset >= lpstLZNT->dwSize)
            return -1;

        nNumBytes = (int)(lpstLZNT->dwSize - dwOffset);
    }

    // Are the bytes to get going forward from the earliest byte
    //  in the buffer?

    if (lpstLZNT->dwOffsetAtI <= dwOffset)
    {
        // Make sure that all the bytes are in the buffer

        if (dwOffset + nNumBytes >
            (lpstLZNT->dwOffsetAtI + LZNT_MAX_COPY_DISPLACEMENT))
        {
            DWORD dwNumSeekForwardBytes;

            dwNumSeekForwardBytes = (dwOffset + nNumBytes) -
                (lpstLZNT->dwOffsetAtI + LZNT_MAX_COPY_DISPLACEMENT);

            // Need to seek forward until all the bytes are within the
            //  buffer.  Seek forward in 16384 byte increments to
            //  definitely stay within a 16-bit int range

            while (1)
            {
                if (dwNumSeekForwardBytes > 16384)
                {
                    if (LZNTDecompressContinue(lpstLZNT,
                                               16384) == -1)
                    {
                        // Error
                        return -1;
                    }

                    dwNumSeekForwardBytes -= 16384;
                }
                else
                {
                    if (LZNTDecompressContinue(lpstLZNT,
                                               (int)dwNumSeekForwardBytes) == -1)
                    {
                        // Error
                        return -1;
                    }
                    break;
                }
            }
        }
    }
    else
    {
        // Have to seek starting from the beginning

        // Reinit and then recurse

        if (LZNTDecompressInit2(lpstLZNT,TRUE,NULL) !=
            LZNT_MAX_COPY_DISPLACEMENT)
        {
            // Error
            return -1;
        }

        return LZNTGetBytes(lpstLZNT,
                            dwOffset,
                            nNumBytes,
                            lpbyDst);
    }

    // At this point all the bytes to get should be in the buffer

    nSrcI = (int)(lpstLZNT->i + (dwOffset - lpstLZNT->dwOffsetAtI));
    if (nSrcI >= LZNT_MAX_COPY_DISPLACEMENT)
        nSrcI -= LZNT_MAX_COPY_DISPLACEMENT;
    lpbySrc = lpstLZNT->abyUncompressed + nSrcI;
    lpbyLimit = lpstLZNT->abyUncompressed + LZNT_MAX_COPY_DISPLACEMENT;

    // Copy the bytes

    nNumBytesToCopy = nNumBytes;
    while (nNumBytesToCopy-- > 0)
    {
        *lpbyDst++ = *lpbySrc++;
        if (lpbySrc == lpbyLimit)
            lpbySrc = lpstLZNT->abyUncompressed;
    }

    return(nNumBytes);
}


typedef struct tagLZNT_CMP_BUF
{
    LPSS_STREAM     lpstStream;

    // The offset in the stream where the chunk starts

    DWORD           dwChunkStartOffset;

    // The offset in the chunk where the buffer starts

    int             nBufStartOffset;

    // The next index in the buffer to which to write

    int             nNextBufWriteI;

    // Buffer to write to

    LPBYTE          lpbyBuf;
} LZNT_CMP_BUF_T, FAR *LPLZNT_CMP_BUF;


//*************************************************************************
//
// BOOL LZNTAddCompressedByte()
//
// Parameters:
//  lpstLZNTCmpBuf  Ptr to LZNT_CMP_BUF_T structure
//  by              The new byte value
//
// Description:
//  Adds a given byte to the compressed stream.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//*************************************************************************

BOOL LZNTAddCompressedByte
(
    LPLZNT_CMP_BUF  lpstLZNTCmpBuf,
    BYTE            by
)
{
    DWORD           dwBytesWritten;

    lpstLZNTCmpBuf->lpbyBuf[lpstLZNTCmpBuf->nNextBufWriteI++] = by;
    if (lpstLZNTCmpBuf->nNextBufWriteI == LZNT_BUF_SIZE)
    {
        // Write it out

        if (SSSeekWrite(lpstLZNTCmpBuf->lpstStream,
                        lpstLZNTCmpBuf->dwChunkStartOffset +
                            lpstLZNTCmpBuf->nBufStartOffset,
                        lpstLZNTCmpBuf->lpbyBuf,
                        LZNT_BUF_SIZE,
                        &dwBytesWritten) != SS_STATUS_OK ||
            dwBytesWritten != LZNT_BUF_SIZE)
        {
            // Error writing

            return(FALSE);
        }

        lpstLZNTCmpBuf->nBufStartOffset += LZNT_BUF_SIZE;
        lpstLZNTCmpBuf->nNextBufWriteI = 0;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL LZNTSetCompressedByte()
//
// Parameters:
//  lpstLZNTCmpBuf  Ptr to LZNT_CMP_BUF_T structure
//  nOffset         Offset in chunk of byte to modify
//  by              The new byte value
//
// Description:
//  Sets the byte value at the given offset to the given value.
//
//  The function assumes that the byte to modify is in the buffer
//  or comes before the first byte of the buffer.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//*************************************************************************

BOOL LZNTSetCompressedByte
(
    LPLZNT_CMP_BUF  lpstLZNTCmpBuf,
    int             nOffset,
    BYTE            by
)
{
    DWORD           dwBytesWritten;

    if (nOffset < lpstLZNTCmpBuf->nBufStartOffset)
    {
        // Write the byte directly

        if (SSSeekWrite(lpstLZNTCmpBuf->lpstStream,
                        lpstLZNTCmpBuf->dwChunkStartOffset + nOffset,
                        &by,
                        sizeof(BYTE),
                        &dwBytesWritten) != SS_STATUS_OK ||
            dwBytesWritten != sizeof(BYTE))
        {
            // Error writing

            return(FALSE);
        }
    }
    else
    {
        // Modify the buffer

        lpstLZNTCmpBuf->
            lpbyBuf[nOffset -lpstLZNTCmpBuf->nBufStartOffset] = by;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL LZNTFlushCompressedBytes()
//
// Parameters:
//  lpstLZNTCmpBuf  Ptr to LZNT_CMP_BUF_T structure
//
// Description:
//  Writes any leftover bytes to the stream.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//*************************************************************************

BOOL LZNTFlushCompressedBytes
(
    LPLZNT_CMP_BUF  lpstLZNTCmpBuf
)
{
    DWORD           dwBytesWritten;

    if (lpstLZNTCmpBuf->nNextBufWriteI > 0)
    {
        // Write it out

        if (SSSeekWrite(lpstLZNTCmpBuf->lpstStream,
                        lpstLZNTCmpBuf->dwChunkStartOffset +
                            lpstLZNTCmpBuf->nBufStartOffset,
                        lpstLZNTCmpBuf->lpbyBuf,
                        (DWORD)lpstLZNTCmpBuf->nNextBufWriteI,
                        &dwBytesWritten) != SS_STATUS_OK ||
            dwBytesWritten != (DWORD)lpstLZNTCmpBuf->nNextBufWriteI)
        {
            // Error writing

            return(FALSE);
        }

        lpstLZNTCmpBuf->nBufStartOffset += lpstLZNTCmpBuf->nNextBufWriteI;
        lpstLZNTCmpBuf->nNextBufWriteI = 0;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL LZNTFindMatch()
//
// Parameters:
//  lpbySrcBuf      Ptr to buffer containing bytes to find match
//  nSrcIdx         The index of the bytes to match
//  nNumSrcBytes    The total number of bytes in the buffer
//  lpwLen          Ptr to WORD to store the best match length
//  lpwDist         Ptr to WORD to store the distance of the best match
//  wMaxLen         The maximum length of the best match
//
// Description:
//  Performs a best case match against the bytes already seen.
//
//  The function assumes that the buffer has a history of at least
//  three bytes and a future of at least three bytes.
//
// Returns:
//  TRUE        If there is a match
//  FALSE       If there is no match
//
//*************************************************************************

BOOL LZNTFindMatch
(
    LPBYTE          lpbySrcBuf,
    int             nSrcIdx,
    int             nNumSrcBytes,
    LPWORD          lpwLen,
    LPWORD          lpwDist,
    WORD            wMaxLen
)
{
    WORD            wBestLen, wDist, wLen;
    int             n;

    wBestLen = 0;

    // Go backwards

    for (n=nSrcIdx-1;n>=0;n--)
    {
        if (lpbySrcBuf[n] == lpbySrcBuf[nSrcIdx] &&
            lpbySrcBuf[n+1] == lpbySrcBuf[nSrcIdx+1] &&
            lpbySrcBuf[n+2] == lpbySrcBuf[nSrcIdx+2])
        {
            wLen = 3;
            while ((nSrcIdx + wLen) < nNumSrcBytes)
            {
                if (lpbySrcBuf[n+wLen] != lpbySrcBuf[nSrcIdx+wLen])
                    break;

                ++wLen;
            }

            if (wLen > wBestLen)
            {
                // Save distance

                wDist = (WORD)(nSrcIdx - n);

                // Limit length to best length

                if (wLen >= wMaxLen)
                {
                    // We'll take this

                    wBestLen = wMaxLen;
                    break;
                }
                else
                    wBestLen = wLen;
            }
        }
    }

    if (wBestLen >= 3)
    {
        *lpwLen = wBestLen;
        *lpwDist = wDist;
        return(TRUE);
    }

    // No matches

    return(FALSE);
}


//*************************************************************************
//
// BOOL LZNTWriteCompressed()
//
// Parameters:
//  lpstLZNT        Ptr to the LZNT state structure, already initialized
//  lpdwOffset      Ptr to DWORD storing offset in stream of next
//                      offset to compress to.  Before exiting, this
//                      is updated to reflect the next offset.
//
// Description:
//  The function compresses the lpstLZNT->dwNumUncompressedChunkBytes
//  bytes in lpstLZNT->abyUncompressed to the stream
//  lpstLZNT->stLZNTBuf.lpstOLEFile.
//
//  Assumption is that lpstLZNT->dwNumUncompressedChunkBytes is
//  greater than zero and no greater than 4096.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//*************************************************************************

BOOL LZNTWriteCompressed
(
    LPLZNT          lpstLZNT,   // Stream to compress to
    LPDWORD         lpdwOffset  // Next offset to compress to
)
{
    int             nSrcIdx;
    int             nNumSrcBytes;
    int             nDstIdx;
    BYTE            byFlag;
    int             nFlagBit;
    int             nFlagIdx;
    WORD            wLen, wDist;
    int             nFormat;
    BYTE            byTemp;
    LZNT_CMP_BUF_T  stLZNTCmpBuf;
    BYTE            abyChunkHdr[2];
    DWORD           dwBytesWritten;
#ifdef SYM_NLM
    int             nLastProgressIdx;
#endif // #ifdef SYM_NLM

    // Initialize LZNT_CMP_BUF_T structure

    stLZNTCmpBuf.lpstStream = lpstLZNT->stLZNTBuf.lpstStream;
    stLZNTCmpBuf.dwChunkStartOffset = *lpdwOffset;
    stLZNTCmpBuf.nBufStartOffset = 0;
    stLZNTCmpBuf.nNextBufWriteI = 3;
    stLZNTCmpBuf.lpbyBuf = lpstLZNT->stLZNTBuf.abyBuf;

    // Compress

    nSrcIdx = 0;
    nNumSrcBytes = (int)lpstLZNT->dwNumUncompressedChunkBytes;
    byFlag = 0;
    nFlagBit = 0;
    nFlagIdx = 2;

    // The destination points to right after the chunk header
    //  and the first flag byte

    nDstIdx = 3;

#ifdef SYM_NLM
    nLastProgressIdx = 0;
    SSProgress(lpstLZNT->stLZNTBuf.lpstStream->
        lpstRoot->lpvRootCookie);
#endif // #ifdef SYM_NLM

    nFormat = FORMAT412;
    while (nSrcIdx < nNumSrcBytes)
    {
#ifdef SYM_NLM
        if (((long)nSrcIdx * (long)nSrcIdx) -
            ((long)nLastProgressIdx * (long)nLastProgressIdx) >
            524288l)
        {
            nLastProgressIdx = nSrcIdx;
            SSProgress(lpstLZNT->stLZNTBuf.lpstStream->
                lpstRoot->lpvRootCookie);
        }
#endif // #ifdef SYM_NLM

        // Update format of copy descriptor

        while ((int)gwLZNTFormatMaxDisplacement[nFormat] < nSrcIdx)
        {
            if (nFormat == FORMAT124)
                break;

            ++nFormat;
        }

        // To find a match, there must be at least three bytes
        //  to match in the bytes already seen against at least
        //  three bytes to be seen.

        if (nSrcIdx >= 3 && (nSrcIdx + 3) <= nNumSrcBytes &&
            LZNTFindMatch(lpstLZNT->abyUncompressed,
                          nSrcIdx,
                          nNumSrcBytes,
                          &wLen,
                          &wDist,
                          gwLZNTFormatMaxLength[nFormat]) == TRUE)
        {
            WORD wCopyDescriptor;

            // Indicate copy

            byFlag |= (1 << nFlagBit);

            // Create copy descriptor

            wCopyDescriptor = (wLen - 3) |
                ((wDist - 1) << gwLZNTCopyDisplacementShift[nFormat]);

            // Write copy descriptor

            byTemp = (BYTE)(wCopyDescriptor & 0xFF);
            if (LZNTAddCompressedByte(&stLZNTCmpBuf,
                                      byTemp) == FALSE)
            {
                return(FALSE);
            }

            byTemp = (BYTE)((wCopyDescriptor >> 8) & 0xFF);
            if (LZNTAddCompressedByte(&stLZNTCmpBuf,
                                      byTemp) == FALSE)
            {
                return(FALSE);
            }

            nSrcIdx += wLen;
            nDstIdx += 2;
        }
        else
        {
            // Write a literal BYTE

            if (LZNTAddCompressedByte(&stLZNTCmpBuf,
                                      lpstLZNT->abyUncompressed[nSrcIdx++]) ==
                FALSE)
            {
                return(FALSE);
            }

            ++nDstIdx;
        }

        if (++nFlagBit == 8)
        {
            // Update flag byte

            if (LZNTSetCompressedByte(&stLZNTCmpBuf,
                                      nFlagIdx,
                                      byFlag) == FALSE)
            {
                return(FALSE);
            }

            nFlagBit = 0;
            byFlag = 0;
            nFlagIdx = nDstIdx++;

            // Make room for the next flag byte

            if (LZNTAddCompressedByte(&stLZNTCmpBuf,
                                      0) == FALSE)
            {
                return(FALSE);
            }
        }
    }

    if (nFlagBit == 0)
    {
        // The last flag byte allocated was not used

        --nDstIdx;
    }
    else
    {
        // Store the last flag byte

        if (LZNTSetCompressedByte(&stLZNTCmpBuf,
                                  nFlagIdx,
                                  byFlag) == FALSE)
        {
            return(FALSE);
        }
    }

    // Was the size of the compressed chunk > 4098 bytes?

    if (nDstIdx > 4098)
    {
        // Yes.  So write out the chunk uncompressed

        // The chunk header is:
        //  0 0 1 1   1 1 1 1   1 1 1 1   1 1 1 1
        //     3
        //  Signature   Length of chunk - 1

        abyChunkHdr[0] = (BYTE)((nNumSrcBytes-1) & 0xFF);
        abyChunkHdr[1] = 0x30 | (((nNumSrcBytes-1) >> 8) & 0x0F);

        if (SSSeekWrite(stLZNTCmpBuf.lpstStream,
                        stLZNTCmpBuf.dwChunkStartOffset,
                        abyChunkHdr,
                        2,
                        &dwBytesWritten) != SS_STATUS_OK ||
            dwBytesWritten != 2)
        {
            // Error writing

            return(FALSE);
        }

        // Write the bytes uncompressed

        if (SSSeekWrite(stLZNTCmpBuf.lpstStream,
                        stLZNTCmpBuf.dwChunkStartOffset+2,
                        lpstLZNT->abyUncompressed,
                        (DWORD)nNumSrcBytes,
                        &dwBytesWritten) != SS_STATUS_OK ||
            dwBytesWritten != (DWORD)nNumSrcBytes)
        {
            // Error writing

            return(FALSE);
        }

        // Update starting offset for next chunk

        *lpdwOffset = stLZNTCmpBuf.dwChunkStartOffset + 2 + nNumSrcBytes;
        return(TRUE);
    }

    // Flush any leftover bytes

    if (LZNTFlushCompressedBytes(&stLZNTCmpBuf) == FALSE)
    {
        return(FALSE);
    }

    // Create and output the chunk header

    abyChunkHdr[0] = (BYTE)((nDstIdx - 3) & 0xFF);
    abyChunkHdr[1] = 0xB0 | (((nDstIdx - 3) >> 8) & 0x0F);

    if (SSSeekWrite(stLZNTCmpBuf.lpstStream,
                    stLZNTCmpBuf.dwChunkStartOffset,
                    abyChunkHdr,
                    2,
                    &dwBytesWritten) != SS_STATUS_OK ||
        dwBytesWritten != 2)
    {
        // Error writing

        return(FALSE);
    }

    *lpdwOffset = stLZNTCmpBuf.dwChunkStartOffset + nDstIdx;

    return(TRUE);
}


//*************************************************************************
//
// BOOL LZNTCopyCompress()
//
// Parameters:
//  lpstLZNT        Ptr to the LZNT state structure, already initialized
//  hFile           Handle to file containing bytes to copy/compress
//  dwOffset        Offset in file of bytes to copy/compress
//  dwNumBytes      Number of bytes to copy/compress
//  lpdwLen         Ptr to DWORD for compressed stream len
//
// Description:
//  The function compresses the bytes at the given offset and copies
//  the compressed bytes to the given stream.
//
//  The LZNT structure is invalidated by this function, so
//  VBA5LZNTInit() must be called before the newly compressed bytes
//  can be read.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//*************************************************************************

BOOL LZNTCopyCompress
(
    LPLZNT          lpstLZNT,   // Stream to compress to
    DWORD           dwOffset,   // Offset in file of bytes to compress
    DWORD           dwNumBytes, // Number of bytes to copy/compress
    LPDWORD         lpdwLen     // Ptr to DWORD for compressed stream len
)
{
    BYTE            byTemp;
    WORD            wTemp;
    WORD            wChunkSize;
    DWORD           dwNextLZNTWriteOffset;
    DWORD           dwBytes;

    /////////////////////////////////////////////////////////////
    // Specify that this is a compressed stream
    /////////////////////////////////////////////////////////////

    byTemp = 1;
    if (SSSeekWrite(lpstLZNT->stLZNTBuf.lpstStream,
                    0,
                    &byTemp,
                    sizeof(BYTE),
                    &dwBytes) != SS_STATUS_OK ||
        dwBytes != sizeof(BYTE))
    {
        return(FALSE);
    }


    /////////////////////////////////////////////////////////////
    // Write compressed chunks
    /////////////////////////////////////////////////////////////

    // Read in LZNT_MAX_COPY_DISPLACEMENT byte chunks

    wChunkSize = LZNT_MAX_COPY_DISPLACEMENT;
    dwNextLZNTWriteOffset = 1;
    while (dwNumBytes != 0)
    {
        // Determine the chunk size

        if (dwNumBytes < LZNT_MAX_COPY_DISPLACEMENT)
            wChunkSize = (WORD)dwNumBytes;

        // Read the uncompressed chunk

        if (SSFileSeek(lpstLZNT->stLZNTBuf.lpstStream->
                           lpstRoot->lpvRootCookie,
                       lpstLZNT->stLZNTBuf.lpstStream->
                           lpstRoot->lpvFile,
                       dwOffset,
                       SS_SEEK_SET,
                       (LPLONG)&dwBytes) != SS_STATUS_OK)
        {
            // Error seeking

            return(FALSE);
        }

        if (SSFileRead(lpstLZNT->stLZNTBuf.lpstStream->
                           lpstRoot->lpvRootCookie,
                       lpstLZNT->stLZNTBuf.lpstStream->
                           lpstRoot->lpvFile,
                       lpstLZNT->abyUncompressed,
                       (DWORD)wChunkSize,
                       &dwBytes) != SS_STATUS_OK ||
            dwBytes != (DWORD)wChunkSize)
        {
            // Error reading

            return(FALSE);
        }

        // Compress and write

        lpstLZNT->dwNumUncompressedChunkBytes = wChunkSize;
        if (LZNTWriteCompressed(lpstLZNT,
                                &dwNextLZNTWriteOffset) == FALSE)
        {
            // Failed to compress and write

            return(FALSE);
        }

        // Update pointers

        dwOffset += wChunkSize;
        dwNumBytes -= wChunkSize;
    }


    /////////////////////////////////////////////////////////////
    // Indicate the end of the compressed stream
    /////////////////////////////////////////////////////////////

    wTemp = 0;
    if (SSSeekWrite(lpstLZNT->stLZNTBuf.lpstStream,
                    dwNextLZNTWriteOffset,
                    (LPBYTE)&wTemp,
                    sizeof(WORD),
                    &dwBytes) != SS_STATUS_OK ||
        dwBytes != sizeof(WORD))
    {
        return(FALSE);
    }

    *lpdwLen = dwNextLZNTWriteOffset + sizeof(WORD);

    return(TRUE);
}


//*************************************************************************
//
// BOOL LZNTCompressBufWrite()
//
// Parameters:
//  lpstLZNT        Ptr to the LZNT state structure, already initialized
//  dwOffset        Offset in file to write compress bytes
//  lpabyBuf        Buffer containing uncompressed data
//  dwNumBytes      Number of bytes to copy/compress
//  lpdwEndOffset   Ptr to DWORD for end offset of compressed stream
//
// Description:
//  The function compresses the bytes from the given buffer
//  and writes the compressed bytes to the given stream at the
//  given offset.
//
//  The LZNT structure is invalidated by this function, so
//  VBA5LZNTInit() must be called before the newly compressed bytes
//  can be read.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//*************************************************************************

BOOL LZNTCompressBufWrite
(
    LPLZNT          lpstLZNT,
    DWORD           dwOffset,
    LPBYTE          lpabyBuf,
    DWORD           dwNumBytes,
    LPDWORD         lpdwEndOffset
)
{
    BYTE            byTemp;
    WORD            wTemp;
    WORD            wChunkSize;
    DWORD           dwNextLZNTWriteOffset;
    DWORD           dwBytes;

    /////////////////////////////////////////////////////////////
    // Specify that this is a compressed stream
    /////////////////////////////////////////////////////////////

    byTemp = 1;
    if (SSSeekWrite(lpstLZNT->stLZNTBuf.lpstStream,
                    dwOffset,
                    &byTemp,
                    sizeof(BYTE),
                    &dwBytes) != SS_STATUS_OK ||
        dwBytes != sizeof(BYTE))
    {
        return(FALSE);
    }


    /////////////////////////////////////////////////////////////
    // Write compressed chunks
    /////////////////////////////////////////////////////////////

    // Read in LZNT_MAX_COPY_DISPLACEMENT byte chunks

    wChunkSize = LZNT_MAX_COPY_DISPLACEMENT;
    dwNextLZNTWriteOffset = dwOffset + 1;
    while (dwNumBytes != 0)
    {
        // Determine the chunk size

        if (dwNumBytes < LZNT_MAX_COPY_DISPLACEMENT)
            wChunkSize = (WORD)dwNumBytes;

        // Copy to the compression buffer

        for (wTemp=0;wTemp<wChunkSize;wTemp++)
            lpstLZNT->abyUncompressed[wTemp] = lpabyBuf[wTemp];

        // Compress and write

        lpstLZNT->dwNumUncompressedChunkBytes = wChunkSize;
        if (LZNTWriteCompressed(lpstLZNT,
                                &dwNextLZNTWriteOffset) == FALSE)
        {
            // Failed to compress and write

            return(FALSE);
        }

        // Update pointers

        lpabyBuf += wChunkSize;
        dwNumBytes -= wChunkSize;
    }


    /////////////////////////////////////////////////////////////
    // Indicate the end of the compressed stream
    /////////////////////////////////////////////////////////////

    wTemp = 0;
    if (SSSeekWrite(lpstLZNT->stLZNTBuf.lpstStream,
                    dwNextLZNTWriteOffset,
                    (LPBYTE)&wTemp,
                    sizeof(WORD),
                    &dwBytes) != SS_STATUS_OK ||
        dwBytes != sizeof(WORD))
    {
        return(FALSE);
    }

    *lpdwEndOffset = dwNextLZNTWriteOffset + sizeof(WORD);

    return(TRUE);
}


//*************************************************************************
//
// int VBA5LZNTInit()
//
// Parameters:
//  lpstStream      Module stream
//  lpstLZNT        Ptr to the LZNT state structure
//
// Description:
//  The function reads the first byte to determine whether
//  the stream is compressed.  If so, then the function calls
//  LZNTDecompressInit() to start the buffering of the bytes
//  of the stream.
//
// Returns:
//  TRUE        On success
//  FALSE       On error reading
//
//*************************************************************************

BOOL VBA5LZNTInit
(
    LPSS_STREAM     lpstStream,
    LPLZNT          lpstLZNT
)
{
    BYTE            by;
    DWORD           dwBytesRead;

    // Get the first byte

    if (SSSeekRead(lpstStream,
                   0,
                   &by,
                   sizeof(BYTE),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(BYTE))
    {
        // Error reading

        return(FALSE);
    }

    lpstLZNT->stLZNTBuf.lpstStream = lpstStream;
    lpstLZNT->dwFirstChunkOffset = 1;

    if ((by & 0x01) == 0)
    {
        // Not compressed

        lpstLZNT->bCompressed = FALSE;
    }
    else
    {
        // Compressed

        lpstLZNT->bCompressed = TRUE;
    }

    LZNTDecompressInit2(lpstLZNT,FALSE,NULL);

    return(TRUE);
}


//*************************************************************************
//
// void VBA5LZNTFindNextAttribute()
//
// Parameters:
//  dwOffset        Offset from which to start searching
//  lpstStream      Module stream
//  lpstLZNT        Ptr to the LZNT state structure
//
// Description:
//  The function starts searching from the given offset for the string:
//
//      0x01 0x?? 0x?? 0x?? A t t r i b u t 0x?? e
//
//  If it is found, the function sets lpstLZNT->dwFirstChunkOffset
//  to the offset after the 0x01.
//
// Returns:
//  Nothing
//
//*************************************************************************

BYTE gabyVBA5Attribute[] = // Attribute
{
    0x41, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65
};

void VBA5LZNTFindNextAttribute
(
    DWORD           dwOffset,
    LPSS_STREAM     lpstStream,
    LPLZNT          lpstLZNT
)
{
    DWORD           dwEndOffset;
    int             i;
    WORD            w, wLimit;
    DWORD           dwNumBytes;
    DWORD           dwBytesRead;

    dwEndOffset = SSStreamLen(lpstStream);
    dwNumBytes = LZNT_MAX_COPY_DISPLACEMENT;
    while (dwOffset < dwEndOffset)
    {
        // Read a chunk of bytes

        if (dwOffset + LZNT_MAX_COPY_DISPLACEMENT > dwEndOffset)
            dwNumBytes = dwEndOffset - dwOffset;

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpstLZNT->abyUncompressed,
                       dwNumBytes,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwNumBytes)
        {
            // Error reading

            break;
        }

        if (dwNumBytes < 14)
        {
            // Not enough bytes

            break;
        }

        // Search for the string

        wLimit = (WORD)(dwNumBytes - 13);
        for (w=0;w<wLimit;w++)
        {
            if (lpstLZNT->abyUncompressed[w] == 0x01)
            {
                // Check those bytes

                for (i=0;i<8;i++)
                {
                    if (lpstLZNT->abyUncompressed[w+4+i] !=
                        gabyVBA5Attribute[i])
                        break;
                }

                if (i == 8 &&
                    lpstLZNT->abyUncompressed[w+13] == 0x65) // 'e'
                {
                    // Found a match

                    lpstLZNT->dwFirstChunkOffset = dwOffset + w + 1;
                    return;
                }
            }
        }

        dwOffset += wLimit;
    }

    // Not found

    lpstLZNT->dwFirstChunkOffset = 0xFFFFFFFF;
}


//*************************************************************************
//
// int VBA5LZNTModuleInit()
//
// Parameters:
//  lpstStream      Module stream
//  lpstLZNT        Ptr to the LZNT state structure
//
// Description:
//  The function seeks to offset 0xD0 of the stream to determine
//  the offset of the compressed source text stream.
//
//  The function then reads the first byte to determine whether
//  the stream is compressed.  If so, then the function calls
//  LZNTDecompressInit() to start the buffering of the bytes
//  of the stream.
//
// Returns:
//  TRUE        On success
//  FALSE       On error reading
//
//*************************************************************************

BOOL VBA5LZNTModuleInit
(
    LPSS_STREAM     lpstStream,
    LPLZNT          lpstLZNT
)
{
    BYTE            by;
    DWORD           dwAt0x0B;
    DWORD           dwOffset;
    DWORD           dwBytesRead;

    // Get the BYTE at offset 5

    SSSeekRead(lpstStream,
               0x05,
               &by,
               sizeof(BYTE),
               &dwBytesRead);

    if (by == 0xB6)
    {
        // Assume Office 97/Mac O98

        // Get the offset of the possibly compressed text stream

        SSSeekRead(lpstStream,
                   0xB,
                   (LPBYTE)&dwAt0x0B,
                   sizeof(DWORD),
                   &dwBytesRead);

        dwAt0x0B = DWENDIAN(dwAt0x0B);

        // Get the offset of the possibly compressed text stream

        SSSeekRead(lpstStream,
                   0xD0+dwAt0x0B,
                   (LPBYTE)&dwOffset,
                   sizeof(DWORD),
                   &dwBytesRead);

        dwOffset = DWENDIAN(dwOffset);

        // Could the offset be at 0xE0 instead?

        if (dwOffset == 0xFFFF)
        {
            SSSeekRead(lpstStream,
                       0xE0+dwAt0x0B,
                       (LPBYTE)&dwOffset,
                       sizeof(DWORD),
                       &dwBytesRead);

            dwOffset = DWENDIAN(dwOffset);
        }
    }
    else
    {
        // Assume Office 9

        // Get the offset of the possibly compressed text stream

        SSSeekRead(lpstStream,
                   0x1D,
                   (LPBYTE)&dwOffset,
                   sizeof(DWORD),
                   &dwBytesRead);

        dwOffset = DWENDIAN(dwOffset) + 6;
    }

    // Get the first byte

    SSSeekRead(lpstStream,
               dwOffset,
               &by,
               sizeof(BYTE),
               &dwBytesRead);

    lpstLZNT->stLZNTBuf.lpstStream = lpstStream;
    lpstLZNT->dwFirstChunkOffset = dwOffset + 1;

    if ((by & 0x01) == 0)
    {
        // Not compressed

        lpstLZNT->bCompressed = FALSE;
    }
    else
    {
        // Compressed

        lpstLZNT->bCompressed = TRUE;
    }

    dwOffset = 0;
    do
    {
        // Zero out the first 9 bytes to insure no mistakes

        for (by=0;by<9;by++)
            lpstLZNT->abyUncompressed[by] = 0;

        LZNTDecompressInit2(lpstLZNT,FALSE,NULL);

        // Make sure that the first bytes are "Attribute"

        if (lpstLZNT->dwSize > 9)
        {
            for (by=0;by<9;by++)
                if (lpstLZNT->abyUncompressed[by] != gabyVBA5Attribute[by])
                    break;

            if (by == 9)
                break;
        }

        // Find the next offset

        VBA5LZNTFindNextAttribute(dwOffset,
                                  lpstStream,
                                  lpstLZNT);

        if (lpstLZNT->dwFirstChunkOffset == 0xFFFFFFFF)
            return(FALSE);

        dwOffset = lpstLZNT->dwFirstChunkOffset + 1;

        // Assume compressed

        lpstLZNT->bCompressed = TRUE;
    }
    while (dwOffset < SSStreamLen(lpstStream));

    return(TRUE);
}


//*************************************************************************
//
// int VBA5LZNTModuleInit2()
//
// Parameters:
//  lpstStream      Module stream
//  lpstLZNT        Ptr to the LZNT state structure
//  lpdwEndOffset   Starting offset for search (0 to init)
//
// Description:
//  The function seeks to offset 0xD0 of the stream to determine
//  the offset of the compressed source text stream.
//
//  The function then reads the first byte to determine whether
//  the stream is compressed.  If so, then the function calls
//  LZNTDecompressInit() to start the buffering of the bytes
//  of the stream.
//
// Returns:
//  TRUE        On success
//  FALSE       On error reading
//
//*************************************************************************

BOOL VBA5LZNTModuleInit2
(
    LPSS_STREAM     lpstStream,
    LPLZNT          lpstLZNT,
    LPDWORD         lpdwEndOffset
)
{
    BYTE            by;
    DWORD           dwAt0x0B;
    DWORD           dwOffset;
    DWORD           dwEndOffset;
    DWORD           dwBytesRead;

    if (lpdwEndOffset == NULL)
        dwEndOffset = 0;
    else
        dwEndOffset = *lpdwEndOffset;

    if (dwEndOffset == 0)
    {
        // Get the BYTE at offset 5

        SSSeekRead(lpstStream,
                   0x05,
                   &by,
                   sizeof(BYTE),
                   &dwBytesRead);

        if (by == 0xB6)
        {
            // Assume Office 97/Mac O98

            // Get the offset of the possibly compressed text stream

            SSSeekRead(lpstStream,
                       0xB,
                       (LPBYTE)&dwAt0x0B,
                       sizeof(DWORD),
                       &dwBytesRead);

            dwAt0x0B = DWENDIAN(dwAt0x0B);

            // Get the offset of the possibly compressed text stream

            SSSeekRead(lpstStream,
                       0xD0+dwAt0x0B,
                       (LPBYTE)&dwOffset,
                       sizeof(DWORD),
                       &dwBytesRead);

            dwOffset = DWENDIAN(dwOffset);

            // Could the offset be at 0xE0 instead?

            if (dwOffset == 0xFFFF)
            {
                SSSeekRead(lpstStream,
                           0xE0+dwAt0x0B,
                           (LPBYTE)&dwOffset,
                           sizeof(DWORD),
                           &dwBytesRead);

                dwOffset = DWENDIAN(dwOffset);
            }
        }
        else
        {
            // Assume Office 9

            // Get the offset of the possibly compressed text stream

            SSSeekRead(lpstStream,
                       0x1D,
                       (LPBYTE)&dwOffset,
                       sizeof(DWORD),
                       &dwBytesRead);

            dwOffset = DWENDIAN(dwOffset) + 6;
        }
    }
    else
        dwOffset = dwEndOffset;

    // Get the first byte

    SSSeekRead(lpstStream,
               dwOffset,
               &by,
               sizeof(BYTE),
               &dwBytesRead);

    lpstLZNT->stLZNTBuf.lpstStream = lpstStream;
    lpstLZNT->dwFirstChunkOffset = dwOffset + 1;

    if ((by & 0x01) == 0)
    {
        // Not compressed

        lpstLZNT->bCompressed = FALSE;
    }
    else
    {
        // Compressed

        lpstLZNT->bCompressed = TRUE;
    }

    dwOffset = dwEndOffset;
    do
    {
        // Zero out the first 9 bytes to insure no mistakes

        for (by=0;by<9;by++)
            lpstLZNT->abyUncompressed[by] = 0;

        LZNTDecompressInit2(lpstLZNT,FALSE,&dwEndOffset);

        // Make sure that the first bytes are "Attribute"

        if (lpstLZNT->dwSize > 9)
        {
            for (by=0;by<9;by++)
                if (lpstLZNT->abyUncompressed[by] != gabyVBA5Attribute[by])
                    break;

            if (by == 9)
                break;
        }

        // Find the next offset

        VBA5LZNTFindNextAttribute(dwOffset,
                                  lpstStream,
                                  lpstLZNT);

        if (lpstLZNT->dwFirstChunkOffset == 0xFFFFFFFF)
            return(FALSE);

        dwOffset = lpstLZNT->dwFirstChunkOffset + 1;

        // Assume compressed

        lpstLZNT->bCompressed = TRUE;
    }
    while (dwOffset < SSStreamLen(lpstStream));

    if (lpdwEndOffset != NULL)
        *lpdwEndOffset = dwEndOffset;

    return(TRUE);
}

