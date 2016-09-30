// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/A97LZNT.CPv   1.0   17 Apr 1998 13:40:54   DCHI  $
//
// Description:
//  Source file containing functions for providing decompressed stream
//  access to a LZNT compressed stream of an Access 97 VBA module.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/A97LZNT.CPv  $
// 
//    Rev 1.0   17 Apr 1998 13:40:54   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "acsapi.h"

//*************************************************************************
//
// BOOL A97LZNTAllocStruct()
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

BOOL A97LZNTAllocStruct
(
    LPVOID          lpvRootCookie,
    LPLPA97LZNT     lplpstLZNT
)
{
    if (ACSMemoryAlloc(lpvRootCookie,
                       sizeof(A97LZNT_T),
                       (LPVOID FAR *)lplpstLZNT) == FALSE)
    {
        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL A97LZNTFreeStruct()
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

BOOL A97LZNTFreeStruct
(
    LPVOID          lpvRootCookie,
    LPA97LZNT       lpstLZNT
)
{
    if (ACSMemoryFree(lpvRootCookie,
                      lpstLZNT) == FALSE)
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

//                                        4/12  5/11  6/10   7/9   8/8   9/7  10/6  11/5  12/4

WORD gwA97LZNTFormatMaxLength[]       = { 4098, 2050, 1026,  514,  258,  130,   66,   34,   18 };
WORD gwA97LZNTFormatMaxDisplacement[] = {   16,   32,   64,  128,  256,  512, 1024, 2048, 4096 };
WORD gwA97LZNTCopyLengthMask[] =        {0xFFF,0x7FF,0x3FF,0x1FF,0x0FF,0x07F,0x03F,0x01F,0x00F };
WORD gwA97LZNTCopyDisplacementShift[] = {   12,   11,   10,    9,    8,    7,    6,    5,    4 };


//*************************************************************************
//
// int A97LZNTBufGetBYTE()
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

int A97LZNTBufGetBYTE
(
    LPA97LZNT_BUF   lpstBuf
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

        if (dwNumBytesLeftInStream < A97LZNT_BUF_SIZE)
        {
            if (dwNumBytesLeftInStream == 0)
                return(-1);

            nNumBytesToRead = (int)dwNumBytesLeftInStream;
        }
        else
            nNumBytesToRead = A97LZNT_BUF_SIZE;

        // Fill the buffer

        if (A97ObjAccessRead(lpstBuf->lpstObj,
                             lpstBuf->dwNextReadOffset,
                             lpstBuf->abyBuf,
                             nNumBytesToRead,
                             &dwBytesRead) == FALSE ||
            dwBytesRead != nNumBytesToRead)
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
// DWORD A97LZNTDecompressInit()
//
// Parameters:
//  lpstLZNT        Ptr to the LZNT state structure
//  bIsRestart      If FALSE, then this is an initialization
//                  If TRUE, then it is just a read of the first
//                      A97LZNT_MAX_COPY_DISPLACEMENT bytes
//
// Description:
//  If bIsRestart == FALSE, this function reads and buffers the
//  first A97LZNT_MAX_COPY_DISPLACEMENT bytes of the stream starting
//  from the offset given in lpstLZNT->dwFirstChunkOffset.
//  The function then continues parsing the stream to determine
//  the total number of bytes in the stream, storing the result
//  in lpstLZNT->dwSize.
//
//  If bIsRestart == TRUE, this function reads and buffers the
//  first A97LZNT_MAX_COPY_DISPLACEMENT bytes of the stream.
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

DWORD A97LZNTDecompressInit
(
    LPA97LZNT   lpstLZNT,
    BOOL        bIsRestart
)
{
    WORD            w;
    WORD            wChunkHdr;
    WORD            wCopyDescriptor;
    int             nCopyLen;
    int             nCopyDisplacement;
    A97LZNT_BUF_T   stBuf;

    DWORD           dwSize;
    BYTE            byFlag;
    int             nFlagBit;
    int             nFormat;
    int             nChunkBytesLeft;
    BOOL            bChunkIsCompressed;
    WORD            wCopyMaxDisplacement;
    DWORD           dwNumUncompressedChunkBytes;
    DWORD           dwBytesRead;

    // Initialize the LZNT structure

    lpstLZNT->i = 0;
    lpstLZNT->dwOffsetAtI = 0;
    lpstLZNT->nCopyLen = 0;
    lpstLZNT->lpbyNextDst = lpstLZNT->abyUncompressed;

    // Check for no compression

    if (lpstLZNT->bCompressed == FALSE)
    {
        lpstLZNT->dwSize = lpstLZNT->stLZNTBuf.lpstObj->dwSize -
            lpstLZNT->dwFirstChunkOffset;

        if (lpstLZNT->dwSize > A97LZNT_MAX_COPY_DISPLACEMENT)
            nCopyLen = A97LZNT_MAX_COPY_DISPLACEMENT;
        else
            nCopyLen = (int)(lpstLZNT->dwSize);

        // Now read in the bytes

        if (A97ObjAccessRead(lpstLZNT->stLZNTBuf.lpstObj,
                             lpstLZNT->dwFirstChunkOffset,
                             lpstLZNT->abyUncompressed,
                             nCopyLen,
                             &dwBytesRead) == FALSE ||
            dwBytesRead != nCopyLen)
        {
            // Error reading

            lpstLZNT->dwSize = 0;
            return(0);
        }

        return(lpstLZNT->dwSize);
    }

    // Initialize the read buffer

    stBuf.lpstObj = lpstLZNT->stLZNTBuf.lpstObj;
    stBuf.nNumBytesInBuf = 0;
    stBuf.nNextByteI = 0;
    stBuf.dwNextReadOffset = lpstLZNT->dwFirstChunkOffset;
    stBuf.dwEndReadOffsetPlus1 =
        lpstLZNT->stLZNTBuf.lpstObj->dwSize;

    dwSize = 0;

    /////////////////////////////////////////////////////////////
    // Get the first A97LZNT_MAX_COPY_DISPLACEMENT bytes
    /////////////////////////////////////////////////////////////

    // This outermost loop iterates through the compressed chunks

    while (1)
    {
        // Get the chunk header

        wChunkHdr = (WORD)A97LZNTBufGetBYTE(&stBuf);
        if (wChunkHdr == 0xFFFF)
            break;

        w = (WORD)A97LZNTBufGetBYTE(&stBuf);
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
                w = (WORD)A97LZNTBufGetBYTE(&stBuf);
                if (w == 0xFFFF)
                    break;

                nChunkBytesLeft--;
                dwNumUncompressedChunkBytes++;

                if (dwSize < A97LZNT_MAX_COPY_DISPLACEMENT)
                {
                    lpstLZNT->abyUncompressed[dwSize] = (BYTE)w;
                    if (dwSize == (A97LZNT_MAX_COPY_DISPLACEMENT - 1))
                    {
                        // Save the current state

                        lpstLZNT->bChunkIsCompressed = bChunkIsCompressed;
                        lpstLZNT->nChunkBytesLeft = nChunkBytesLeft;

                        lpstLZNT->dwNumUncompressedChunkBytes =
                            dwNumUncompressedChunkBytes;

                        lpstLZNT->stLZNTBuf = stBuf;

                        if (bIsRestart == TRUE)
                            return(A97LZNT_MAX_COPY_DISPLACEMENT);
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
        wCopyMaxDisplacement = gwA97LZNTFormatMaxDisplacement[FORMAT412];
        dwNumUncompressedChunkBytes = 0;
        while (nChunkBytesLeft > 0)
        {
            if (nFlagBit == 8)
            {
                // Get a new flag byte

                w = (WORD)A97LZNTBufGetBYTE(&stBuf);
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

                w = (WORD)A97LZNTBufGetBYTE(&stBuf);
                if (w == 0xFFFF)
                    break;

                nChunkBytesLeft--;
                dwNumUncompressedChunkBytes++;

                if (dwSize < A97LZNT_MAX_COPY_DISPLACEMENT)
                {
                    lpstLZNT->abyUncompressed[dwSize] = (BYTE)w;
                    if (dwSize == (A97LZNT_MAX_COPY_DISPLACEMENT - 1))
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
                            return(A97LZNT_MAX_COPY_DISPLACEMENT);
                    }
                }

                ++dwSize;
            }

            // If the bit is non-zero, then the next byte comes
            //  from somewhere in the already decompressed bytes

            else
            {
                // Get the copy descriptor

                wCopyDescriptor = (WORD)A97LZNTBufGetBYTE(&stBuf);
                if (wCopyDescriptor == 0xFFFF)
                    break;

                w = (WORD)A97LZNTBufGetBYTE(&stBuf);
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
                    while (gwA97LZNTFormatMaxDisplacement[nFormat] <
                        dwNumUncompressedChunkBytes);

                    wCopyMaxDisplacement =
                        gwA97LZNTFormatMaxDisplacement[nFormat];
                }

                // Get the length

                nCopyLen = 3 +
                    (wCopyDescriptor & gwA97LZNTCopyLengthMask[nFormat]);
                nCopyDisplacement = 1 +
                    (wCopyDescriptor >> gwA97LZNTCopyDisplacementShift[nFormat]);

                if (dwSize < A97LZNT_MAX_COPY_DISPLACEMENT)
                {
                    int     nNumBytesToCopy;
                    LPBYTE  lpbySrc;
                    LPBYTE  lpbyDst;

                    if (nCopyDisplacement > dwSize)
                    {
                        // Error in compressed stream

                        break;
                    }

                    lpbySrc = lpstLZNT->abyUncompressed + dwSize -
                        nCopyDisplacement;
                    lpbyDst = lpstLZNT->abyUncompressed + dwSize;

                    if (dwSize + nCopyLen >= A97LZNT_MAX_COPY_DISPLACEMENT)
                    {
                        nNumBytesToCopy =
                            (int)(A97LZNT_MAX_COPY_DISPLACEMENT - dwSize);

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
                            return(A97LZNT_MAX_COPY_DISPLACEMENT);
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
        lpstLZNT->dwSize = dwSize;

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
//  than A97LZNT_MAX_COPY_DISPLACEMENT or if the number of bytes left
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

int A97LZNTDecompressContinue
(
    LPA97LZNT       lpstLZNT,
    int             nNumBytes
)
{
    WORD            w;
    WORD            wChunkHdr;
    WORD            wCopyDescriptor;
    int             i;
    int             nCopyLen;
    int             nCopyDisplacement;
    LPA97LZNT_BUF  lpstBuf;
    int             nNumBytesToCopy;
    int             nNumBytesLeft;
    int             nSrcI;

    LPBYTE          lpbySrc;
    LPBYTE          lpbyDst;
    LPBYTE          lpbyLimit;

    BYTE            byFlag;
    int             nFlagBit;
    int             nFormat;
    int             nChunkBytesLeft;
    BOOL            bChunkIsCompressed;
    WORD            wCopyMaxDisplacement;
    DWORD           dwNumUncompressedChunkBytes;

    /////////////////////////////////////////////////////////////
    // Non-compressed stream continuation
    /////////////////////////////////////////////////////////////

    if (lpstLZNT->bCompressed == FALSE)
    {
        DWORD   dwStartOffset;
        DWORD   dwBytesRead;

        i = lpstLZNT->i;

        lpstLZNT->i += nNumBytes;
        if (lpstLZNT->i >= A97LZNT_MAX_COPY_DISPLACEMENT)
            lpstLZNT->i %= A97LZNT_MAX_COPY_DISPLACEMENT;

        dwStartOffset = lpstLZNT->dwOffsetAtI +
            A97LZNT_MAX_COPY_DISPLACEMENT;
        lpstLZNT->dwOffsetAtI += nNumBytes;

        if (lpstLZNT->dwSize > A97LZNT_MAX_COPY_DISPLACEMENT)
            nCopyLen = A97LZNT_MAX_COPY_DISPLACEMENT;
        else
            nCopyLen = (int)(lpstLZNT->dwSize);

        // Now read in the bytes

        if (i + nNumBytes > A97LZNT_MAX_COPY_DISPLACEMENT)
            nCopyLen = A97LZNT_MAX_COPY_DISPLACEMENT - i;
        else
            nCopyLen = nNumBytes;

        if (A97ObjAccessRead(lpstLZNT->stLZNTBuf.lpstObj,
                             dwStartOffset,
                             lpstLZNT->abyUncompressed + i,
                             nCopyLen,
                             &dwBytesRead) == FALSE ||
            dwBytesRead != nCopyLen)
        {
            // Error reading

            return(-1);
        }

        if (i + nNumBytes > A97LZNT_MAX_COPY_DISPLACEMENT)
        {
            // Copy the remaining bytes

            nCopyLen = nNumBytes - nCopyLen;

            if (A97ObjAccessRead(lpstLZNT->stLZNTBuf.lpstObj,
                                 dwStartOffset + i,
                                 lpstLZNT->abyUncompressed,
                                 nCopyLen,
                                 &dwBytesRead) == FALSE ||
                dwBytesRead != nCopyLen)
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
    lpbyLimit = lpstLZNT->abyUncompressed + A97LZNT_MAX_COPY_DISPLACEMENT;
    while (1)
    {
        if (nChunkBytesLeft <= 0 && lpstLZNT->nCopyLen == 0)
        {
            // Get the chunk header

            wChunkHdr = (WORD)A97LZNTBufGetBYTE(lpstBuf);
            if (wChunkHdr == 0xFFFF)
            {
                // This is an error condition
                return(-1);
            }

            w = (WORD)A97LZNTBufGetBYTE(lpstBuf);
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

            wCopyMaxDisplacement = gwA97LZNTFormatMaxDisplacement[FORMAT412];
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
                    nSrcI += A97LZNT_MAX_COPY_DISPLACEMENT;

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
                    if (lpstLZNT->i >= A97LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= A97LZNT_MAX_COPY_DISPLACEMENT;
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
                    if (lpstLZNT->i >= A97LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= A97LZNT_MAX_COPY_DISPLACEMENT;
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

            wCopyMaxDisplacement = gwA97LZNTFormatMaxDisplacement[nFormat];
        }

        /////////////////////////////////////////////////////////
        // Compressed chunk continuation
        /////////////////////////////////////////////////////////

        if (bChunkIsCompressed == FALSE)
        {
            while (nChunkBytesLeft > 0)
            {
                w = (WORD)A97LZNTBufGetBYTE(lpstBuf);
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
                    if (lpstLZNT->i >= A97LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= A97LZNT_MAX_COPY_DISPLACEMENT;
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

                w = (WORD)A97LZNTBufGetBYTE(lpstBuf);
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

                w = (WORD)A97LZNTBufGetBYTE(lpstBuf);
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
                    if (lpstLZNT->i >= A97LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= A97LZNT_MAX_COPY_DISPLACEMENT;
                    lpstLZNT->dwOffsetAtI += nNumBytes;
                    return(nNumBytes);
                }
            }

            // If the bit is non-zero, then the next byte comes
            //  from somewhere in the already decompressed bytes

            else
            {
                // Get the copy descriptor

                wCopyDescriptor = (WORD)A97LZNTBufGetBYTE(lpstBuf);
                if (wCopyDescriptor == 0xFFFF)
                {
                    // This is an error condition
                    return(-1);
                }

                w = (WORD)A97LZNTBufGetBYTE(lpstBuf);
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
                    while (gwA97LZNTFormatMaxDisplacement[nFormat] <
                        dwNumUncompressedChunkBytes);

                    wCopyMaxDisplacement =
                        gwA97LZNTFormatMaxDisplacement[nFormat];
                }

                // Get the length

                nCopyLen = 3 +
                    (wCopyDescriptor & gwA97LZNTCopyLengthMask[nFormat]);
                nCopyDisplacement = 1 +
                    (wCopyDescriptor >> gwA97LZNTCopyDisplacementShift[nFormat]);

                // Limit the number of bytes to copy

                if (nCopyLen > nNumBytesLeft)
                    nNumBytesToCopy = nNumBytesLeft;
                else
                    nNumBytesToCopy = nCopyLen;

                // The displacement is assumed to be valid

                nSrcI = (lpbyDst - lpstLZNT->abyUncompressed) -
                    nCopyDisplacement;

                if (nSrcI < 0)
                    nSrcI += A97LZNT_MAX_COPY_DISPLACEMENT;

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
                    if (lpstLZNT->i >= A97LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= A97LZNT_MAX_COPY_DISPLACEMENT;
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
                    if (lpstLZNT->i >= A97LZNT_MAX_COPY_DISPLACEMENT)
                        lpstLZNT->i %= A97LZNT_MAX_COPY_DISPLACEMENT;
                    lpstLZNT->dwOffsetAtI += nNumBytes;
                    return(nNumBytes);
                }
            }
        }
    }
}


//*************************************************************************
//
// int A97LZNTGetBytes()
//
// Parameters:
//  lpstLZNT        Ptr to the LZNT state structure
//  dwOffset        Offset in stream from which to get the bytes
//  nNumBytes       Number of bytes to seek forward
//                      This should never be greater than
//                      A97LZNT_MAX_COPY_DISPLACEMENT.
//  lpbyDst         The buffer in which to store the bytes
//
// Description:
//  This function stores into lpbyDst up to nNumBytes from the
//  given offset.
//
//  The number of bytes requested should never be greater than
//  A97LZNT_MAX_COPY_DISPLACEMENT because the function readjusts
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

int A97LZNTGetBytes
(
    LPA97LZNT       lpstLZNT,
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
            (lpstLZNT->dwOffsetAtI + A97LZNT_MAX_COPY_DISPLACEMENT))
        {
            DWORD dwNumSeekForwardBytes;

            dwNumSeekForwardBytes = (dwOffset + nNumBytes) -
                (lpstLZNT->dwOffsetAtI + A97LZNT_MAX_COPY_DISPLACEMENT);

            // Need to seek forward until all the bytes are within the
            //  buffer.  Seek forward in 16384 byte increments to
            //  definitely stay within a 16-bit int range

            while (1)
            {
                if (dwNumSeekForwardBytes > 16384)
                {
                    if (A97LZNTDecompressContinue(lpstLZNT,
                                                  16384) == -1)
                    {
                        // Error
                        return -1;
                    }

                    dwNumSeekForwardBytes -= 16384;
                }
                else
                {
                    if (A97LZNTDecompressContinue(lpstLZNT,
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

        if (A97LZNTDecompressInit(lpstLZNT,TRUE) !=
            A97LZNT_MAX_COPY_DISPLACEMENT)
        {
            // Error
            return -1;
        }

        return A97LZNTGetBytes(lpstLZNT,
                               dwOffset,
                               nNumBytes,
                               lpbyDst);
    }

    // At this point all the bytes to get should be in the buffer

    nSrcI = (int)(lpstLZNT->i + (dwOffset - lpstLZNT->dwOffsetAtI));
    if (nSrcI >= A97LZNT_MAX_COPY_DISPLACEMENT)
        nSrcI -= A97LZNT_MAX_COPY_DISPLACEMENT;
    lpbySrc = lpstLZNT->abyUncompressed + nSrcI;
    lpbyLimit = lpstLZNT->abyUncompressed + A97LZNT_MAX_COPY_DISPLACEMENT;

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


//*************************************************************************
//
// void A97LZNTFindNextAttribute()
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

BYTE gabyA97Attribute[] = // Attribute
{
    0x41, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65
};

void A97LZNTFindNextAttribute
(
    DWORD               dwOffset,
    LPA97_OBJ_ACCESS    lpstObj,
    LPA97LZNT           lpstLZNT
)
{
    DWORD           dwEndOffset;
    int             i;
    WORD            w, wLimit;
    DWORD           dwNumBytes;
    DWORD           dwBytesRead;

    dwEndOffset = lpstObj->dwSize;
    dwNumBytes = A97LZNT_MAX_COPY_DISPLACEMENT;
    while (dwOffset < dwEndOffset)
    {
        // Read a chunk of bytes

        if (dwOffset + A97LZNT_MAX_COPY_DISPLACEMENT > dwEndOffset)
            dwNumBytes = dwEndOffset - dwOffset;

        if (A97ObjAccessRead(lpstObj,
                             dwOffset,
                             lpstLZNT->abyUncompressed,
                             dwNumBytes,
                             &dwBytesRead) == FALSE ||
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
                        gabyA97Attribute[i])
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
// int A97LZNTModuleInit()
//
// Parameters:
//  lpstObj         Module object
//  dwOffset        Offset of VBA module in object
//  dwSize          Size of module
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

BOOL A97LZNTModuleInit
(
    LPA97_OBJ_ACCESS    lpstObj,
    DWORD               dwOffset,
    LPA97LZNT           lpstLZNT
)
{
    BYTE            by;
    DWORD           dwAt0x0B;
    DWORD           dwCandOffset;
    DWORD           dwBytesRead;

    // Get the offset of the possibly compressed text stream

    if (A97ObjAccessRead(lpstObj,
                         dwOffset + 0xB,
                         &dwAt0x0B,
                         sizeof(DWORD),
                         &dwBytesRead) == FALSE ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwAt0x0B = DWENDIAN(dwAt0x0B);

    // Get the offset of the possibly compressed text stream

    if (A97ObjAccessRead(lpstObj,
                         dwOffset + 0xD0 + dwAt0x0B,
                         &dwCandOffset,
                         sizeof(DWORD),
                         &dwBytesRead) == FALSE ||
        dwBytesRead != sizeof(DWORD))
        dwCandOffset = 0xFFFF;
    else
        dwCandOffset = DWENDIAN(dwCandOffset);

    // Could the offset be at 0xE0 instead?

    if (dwCandOffset == 0xFFFF)
    {
        if (A97ObjAccessRead(lpstObj,
                             dwOffset + 0xE0 + dwAt0x0B,
                             &dwCandOffset,
                             sizeof(DWORD),
                             &dwBytesRead) == FALSE ||
            dwBytesRead != sizeof(DWORD))
            dwCandOffset = 0xFFFF;
        else
            dwCandOffset = DWENDIAN(dwCandOffset);
    }

    // Get the first byte

    if (A97ObjAccessRead(lpstObj,
                         dwOffset + dwCandOffset,
                         &by,
                         sizeof(BYTE),
                         &dwBytesRead) == FALSE ||
        dwBytesRead != sizeof(BYTE))
        return(FALSE);

    lpstLZNT->stLZNTBuf.lpstObj = lpstObj;
    lpstLZNT->dwFirstChunkOffset = dwOffset + dwCandOffset + 1;

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

    dwCandOffset = 0;
    do
    {
        // Zero out the first 9 bytes to insure no mistakes

        for (by=0;by<9;by++)
            lpstLZNT->abyUncompressed[by] = 0;

        A97LZNTDecompressInit(lpstLZNT,FALSE);

        // Make sure that the first bytes are "Attribute"

        if (lpstLZNT->dwSize > 9)
        {
            for (by=0;by<9;by++)
                if (lpstLZNT->abyUncompressed[by] != gabyA97Attribute[by])
                    break;

            if (by == 9)
                break;
        }

        // Find the next offset

        A97LZNTFindNextAttribute(dwOffset + dwCandOffset,
                                 lpstObj,
                                 lpstLZNT);

        if (lpstLZNT->dwFirstChunkOffset == 0xFFFFFFFF)
            return(FALSE);

        dwCandOffset = lpstLZNT->dwFirstChunkOffset + 1;

        // Assume compressed

        lpstLZNT->bCompressed = TRUE;
    }
    while (dwCandOffset < lpstObj->dwSize);

    return(TRUE);
}

