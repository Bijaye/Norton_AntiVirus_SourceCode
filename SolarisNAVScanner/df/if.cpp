//************************************************************************
//
// $Header:   S:/DF/VCS/if.cpv   1.6   24 Dec 1998 21:21:00   DCHI  $
//
// Description:
//  Inflate implementation.  See RFC 1951.
//
//************************************************************************
// $Log:   S:/DF/VCS/if.cpv  $
// 
//    Rev 1.6   24 Dec 1998 21:21:00   DCHI
// Added additional error checking.
// 
//    Rev 1.5   23 Dec 1998 14:14:24   DCHI
// 
//    Rev 1.4   23 Dec 1998 14:12:30   DCHI
// Added support for fixed Huffman trees.
// 
//    Rev 1.3   12 Nov 1998 18:09:22   DCHI
// Added endianization of LEN value.
// 
//    Rev 1.2   09 Nov 1998 13:45:50   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data for SYM_WIN16.
// 
//    Rev 1.1   05 Nov 1998 14:46:34   DCHI
// Changed abyCLAOrder[] to be global FAR.
// 
//    Rev 1.0   11 Nov 1997 16:21:18   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "df.h"
#include "crc32.h"

// The literal alphabet of byte values (0..255) is represented by
// codes 0 through 255.  The code 256 indicates end-of-block.
// The length alphabet uses codes 257 through 285:
//
//      Extra               Extra               Extra
// Code Bits Length(s) Code Bits Lengths   Code Bits Length(s)
// ---- ---- ------     ---- ---- -------   ---- ---- -------
//  257   0     3       267   1   15,16     277   4   67-82
//  258   0     4       268   1   17,18     278   4   83-98
//  259   0     5       269   2   19-22     279   4   99-114
//  260   0     6       270   2   23-26     280   4  115-130
//  261   0     7       271   2   27-30     281   5  131-162
//  262   0     8       272   2   31-34     282   5  163-194
//  263   0     9       273   3   35-42     283   5  195-226
//  264   0    10       274   3   43-50     284   5  227-257
//  265   1  11,12      275   3   51-58     285   0    258
//  266   1  13,14      276   3   59-66

// Distances are represented using the following codes:
//
//      Extra           Extra               Extra
// Code Bits Dist  Code Bits   Dist     Code Bits Distance
// ---- ---- ----  ---- ----  ------    ---- ---- --------
//   0   0    1     10   4     33-48    20    9   1025-1536
//   1   0    2     11   4     49-64    21    9   1537-2048
//   2   0    3     12   5     65-96    22   10   2049-3072
//   3   0    4     13   5     97-128   23   10   3073-4096
//   4   1   5,6    14   6    129-192   24   11   4097-6144
//   5   1   7,8    15   6    193-256   25   11   6145-8192
//   6   2   9-12   16   7    257-384   26   12  8193-12288
//   7   2  13-16   17   7    385-512   27   12 12289-16384
//   8   3  17-24   18   8    513-768   28   13 16385-24576
//   9   3  25-32   19   8   769-1024   29   13 24577-32768

//********************************************************************
//
// int IFGetBit()
//
// Parameters:
//  lpstIF          Ptr to IF structure
//
// Description:
//  Retrieves a single bit from the compressed stream.
//
// Returns:
//  0               If the next bit is a zero
//  1               If the next bit is a one
//  -1              On error
//
//********************************************************************

int IFGetBit
(
    LPIF        lpstIF
)
{
    int         nBit;
    long        lNewOffset;
    DWORD       dwByteCount;

    if (lpstIF->nBitMask == 256)
    {
        if (lpstIF->nReadAheadI == lpstIF->nReadAheadByteCount)
        {
            if (lpstIF->dwNextReadOffset >= lpstIF->dwEndReadOffset)
                return(-1);

            // Need to refill the buffer

            if (lpstIF->dwEndReadOffset - lpstIF->dwNextReadOffset <
                IF_READ_AHEAD_SIZE)
                lpstIF->nReadAheadByteCount =
                    (int)(lpstIF->dwEndReadOffset - lpstIF->dwNextReadOffset);
            else
                lpstIF->nReadAheadByteCount = IF_READ_AHEAD_SIZE;

            if (DFFileSeek(lpstIF->lpvRootCookie,
                           lpstIF->lpvSrcFile,
                           lpstIF->dwNextReadOffset,
                           DF_SEEK_SET,
                           &lNewOffset) != DF_STATUS_OK)
                return(-1);

            if (DFFileRead(lpstIF->lpvRootCookie,
                           lpstIF->lpvSrcFile,
                           lpstIF->abyReadAhead,
                           lpstIF->nReadAheadByteCount,
                           &dwByteCount) != DF_STATUS_OK ||
                dwByteCount != (DWORD)lpstIF->nReadAheadByteCount)
                return(-1);

            lpstIF->dwNextReadOffset += lpstIF->nReadAheadByteCount;

            lpstIF->nReadAheadI = 0;
        }

        lpstIF->byCurByte = lpstIF->abyReadAhead[lpstIF->nReadAheadI++];
        lpstIF->nBitMask = 1;
    }

    if (lpstIF->byCurByte & lpstIF->nBitMask)
        nBit = 1;
    else
        nBit = 0;

    lpstIF->nBitMask <<= 1;

    return(nBit);
}


//********************************************************************
//
// int IFGetMulipleBits()
//
// Parameters:
//  lpstIF          Ptr to IF structure
//  nNumBits        Number of bits to get
//
// Description:
//  Retrieves the next nNumBits bits from the compressed stream,
//  shifting them into a DWORD.
//
// Returns:
//  DWORD               On success
//  0xFFFFFFFF          On failure
//
//********************************************************************

DWORD IFGetMultipleBits
(
    LPIF        lpstIF,
    int         nNumBits
)
{
    int         i, n;
    DWORD       dwValue;

    dwValue = 0;
    for (i=0;i<nNumBits;i++)
    {
        n = IFGetBit(lpstIF);
        if (n == 0)
            continue;

        if (n == 1)
            dwValue |= ((DWORD)1) << i;
        else
            return(0xFFFFFFFF);
    }

    return(dwValue);
}


//********************************************************************
//
// int IFGetValueFromTree()
//
// Parameters:
//  lpstIF          Ptr to IF structure
//  lpaw0           Ptr to '0' branch array of Huffman tree
//  lpaw1           Ptr to '0' branch array of Huffman tree
//  nNumCodes       Number of codes in tree
//
// Description:
//  Retrieves the next code from the stream given using the given
//  Huffman tree as a guide.
//
// Returns:
//  0..(nNumCodes - 1)  On success
//  -1                  On failure
//
//********************************************************************

int IFGetValueFromTree
(
    LPIF        lpstIF,
    LPWORD      lpaw0,
    LPWORD      lpaw1,
    int         nNumCodes
)
{
    int         nCurSlot;
    int         i;

    // Start at

    nCurSlot = 0;
    for (i=0;i<16;i++)
    {
        switch (IFGetBit(lpstIF))
        {
            case 0:
                // Look at the left

                nCurSlot = (int)lpaw0[nCurSlot];
                break;

            case 1:
                // Look at the right

                nCurSlot = (int)lpaw1[nCurSlot];
                break;

            default:
                return(-1);
        }

        if (nCurSlot >= nNumCodes)
        {
            // Found a leaf

            return(nCurSlot-nNumCodes);
        }
    }

    return(-1);
}


//********************************************************************
//
// BOOL IFCreateTree()
//
// Parameters:
//  lpaw0           Ptr to a WORD array of size (nNumCodes - 1)
//  lpaw1           Ptr to a WORD array of size (nNumCodes - 1)
//  lpawCodeLen     Ptr to a WORD array of size nNumCodes
//  nNumCodes       Number of codes
//
// Description:
//  This function generates the array representation of the
//  Huffman tree, given the array of code lengths and the number
//  of codes.
//
//  The function assumes that no code is greater than 16 bits
//  in length.
//
//  The data structure used to represent the Huffman tree consists
//  of two arrays of WORDs, one array representing the '0'
//  branch of each node and the other array representing the
//  '1' branch of each node.
//
//  Each of the branches of each node either represents a pointer
//  to another node or is a leaf branch, thus representing a code.
//  The range of the WORD value is used to determine whether it
//  represents a pointer to another node or is a leaf code value.
//  If the value is less than nNumCodes, then it represents the
//  index of the node to which it points.  Otherwise, the value
//  of the code is equal to the value minus nNumCodes.
//
//  Here is an example:
//
//      Symbol  Huffman Code
//      ------  ------------
//         A         00
//         B          1
//         C        011
//         D        010
//
//      Using the algorithm of the function generates the
//      following in the two arrays:
//
//          lpaw0   1 4 7
//          lpaw1   5 2 6
//
//      The entries with values 4, 5, 6, and 7 represent the
//      symbols A, B, C, and D, respectively.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL IFCreateTree
(
    LPWORD      lpaw0,
    LPWORD      lpaw1,
    LPWORD      lpawCodeLen,
    int         nNumCodes
)
{
    WORD        wNextCode;
    WORD        wBits, wBitNum;
    WORD        wNextOpenSlot, wCurSlot;
    int         i;

    // Initialize tree

    for (i=0;i<(nNumCodes-1);i++)
        lpaw0[i] = lpaw1[i] = 0xFFFF;

    wNextCode = 0;
    wNextOpenSlot = 1;
    for (wBits=1;wBits<=16;wBits++)
    {
        for (i=0;i<nNumCodes;i++)
        {
            if (lpawCodeLen[i] == wBits)
            {
                // Insert the code

                wBitNum = wBits;
                wCurSlot = 0;

                // The while loop traverses the interior nodes
                //  of the Huffman tree

                while (--wBitNum != 0)
                {
                    if (wNextCode & (1 << wBitNum))
                    {
                        // Go to the right or create if necessary

                        if (lpaw1[wCurSlot] == 0xFFFF)
                        {
                            // Create

                            lpaw1[wCurSlot] = wNextOpenSlot;
                            wCurSlot = wNextOpenSlot++;
                        }
                        else
                            wCurSlot = lpaw1[wCurSlot];
                    }
                    else
                    {
                        // Go to the left or create if necessary

                        if (lpaw0[wCurSlot] == 0xFFFF)
                        {
                            // Create

                            lpaw0[wCurSlot] = wNextOpenSlot;
                            wCurSlot = wNextOpenSlot++;
                        }
                        else
                            wCurSlot = lpaw0[wCurSlot];
                    }

                    if (wCurSlot >= (nNumCodes-1))
                        return(FALSE);
                }

                // The following code adds the leaf element

                if (wNextCode & (1 << wBitNum))
                {
                    // Insert in the right slot

                    lpaw1[wCurSlot] = (WORD)(nNumCodes + i);
                }
                else
                {
                    // Insert in the left slot

                    lpaw0[wCurSlot] = (WORD)(nNumCodes + i);
                }

                // Move to next code

                ++wNextCode;
            }
        }

        wNextCode <<= 1;
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL IFGetCodeLengthSet()
//
// Parameters:
//  lpstIF          Ptr to IF structure with file pointer
//                  initialized
//  lpawCLT0        '0' branch array of code length Huffman tree
//  lpawCLT1        '1' branch array of code length Huffman tree
//  lpawCL          Destination array for code lengths
//  nCount          Number of code lengths to get
//
// Description:
//  Using the code length Huffman tree, retrieves nCount
//  code lengths into lpawCL.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL IFGetCodeLengthSet
(
    LPIF            lpstIF,
    LPWORD          lpawCLT0,
    LPWORD          lpawCLT1,
    LPWORD          lpawCL,
    int             nCount
)
{
    int             n, nValue, nCodeLen;
    DWORD           dwBits;

    n = 0;
    while (n < nCount)
    {
        nValue = IFGetValueFromTree(lpstIF,
                                    lpawCLT0,
                                    lpawCLT1,
                                    IF_CLT_SIZE+1);

        switch (nValue)
        {
            case 0: case 1: case 2: case 3: case 4:
            case 5: case 6: case 7: case 8: case 9:
            case 10: case 11: case 12:
            case 13: case 14: case 15:
                lpawCL[n++] = nCodeLen = nValue;
                break;

            case 16:
                dwBits = IFGetMultipleBits(lpstIF,2);
                if (dwBits == 0xFFFFFFFF)
                    return(FALSE);

                nValue = (int)dwBits + 3;
                while (nValue-- != 0)
                    lpawCL[n++] = nCodeLen;
                break;

            case 17:
                dwBits = IFGetMultipleBits(lpstIF,3);
                if (dwBits == 0xFFFFFFFF)
                    return(FALSE);

                nValue = (int)dwBits + 3;
                while (nValue-- != 0)
                    lpawCL[n++] = 0;
                break;

            case 18:
                dwBits = IFGetMultipleBits(lpstIF,7);
                if (dwBits == 0xFFFFFFFF)
                    return(FALSE);

                nValue = (int)dwBits + 11;
                while (nValue-- != 0)
                    lpawCL[n++] = 0;
                break;

            default:
                return(FALSE);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL IFGetTrees()
//
// Parameters:
//  lpstIF          Ptr to IF structure with file pointer
//                  initialized
//
// Description:
//  Reads the dynamic huffman codes and generates the trees.
//
//  Format of Huffman codes
//  -----------------------
//  5 Bits: HLIT, # of Literal/Length codes - 257 (257 - 286)
//  5 Bits: HDIST, # of Distance codes - 1        (1 - 32)
//  4 Bits: HCLEN, # of Code Length codes - 4     (4 - 19)
//
//  (HCLEN + 4) x 3 bits: code lengths for the code length
//    alphabet given just above, in the order: 16, 17, 18,
//    0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
//
//    These code lengths are interpreted as 3-bit integers
//    (0-7); as above, a code length of 0 means the
//    corresponding symbol (literal/length or distance code
//    length) is not used.
//
//  HLIT + 257 code lengths for the literal/length alphabet,
//    encoded using the code length Huffman code
//
//  HDIST + 1 code lengths for the distance alphabet,
//    encoded using the code length Huffman code
//
//  Code length codes
//  -----------------
//  0 - 15: Represent code lengths of 0 - 15
//  16: Copy the previous code length 3 - 6 times.
//      The next 2 bits indicate repeat length
//            (0 = 3, ... , 3 = 6)
//         Example:  Codes 8, 16 (+2 bits 11),
//                   16 (+2 bits 10) will expand to
//                   12 code lengths of 8 (1 + 6 + 5)
//  17: Repeat a code length of 0 for 3 - 10 times.
//      (3 bits of length)
//  18: Repeat a code length of 0 for 11 - 138 times
//      (7 bits of length)
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyCLAOrder[IF_CLT_SIZE+1] =
{ 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL IFGetTrees
(
    LPIF    lpstIF
)
{
    // Code length alphabet order as defined above

    int     nHLIT;  // Number of literal/length codes
    int     nHDIST; // Number of distance codes
    int     nHCLEN; // Number of code length codes
    int     n;
    WORD    awCLT0[IF_CLT_SIZE];
    WORD    awCLT1[IF_CLT_SIZE];
    WORD    awCL[IF_LLT_SIZE+1];

    // Get counts

    nHLIT = (int)IFGetMultipleBits(lpstIF,5);
    if (nHLIT == -1)
        return(FALSE);

    nHLIT += 257;

    nHDIST = (int)IFGetMultipleBits(lpstIF,5);
    if (nHDIST == -1)
        return(FALSE);

    nHDIST += 1;

    nHCLEN = (int)IFGetMultipleBits(lpstIF,4);
    if (nHCLEN == -1)
        return(FALSE);

    nHCLEN += 4;

    /////////////////////////////////////////////////////////////
    // Get code length Huffman tree

    // Initialize code length alphabet code lengths to zero

    for (n=0;n<=IF_CLT_SIZE;n++)
        awCL[n] = 0;

    // Read code lengths

    for (n=0;n<nHCLEN;n++)
        awCL[gabyCLAOrder[n]] = (WORD)IFGetMultipleBits(lpstIF,3);

    // Create code length Huffman tree

    if (IFCreateTree(awCLT0,
                     awCLT1,
                     awCL,
                     IF_CLT_SIZE+1) == FALSE)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Get literal/length Huffman tree

    // Read code lengths

    if (IFGetCodeLengthSet(lpstIF,awCLT0,awCLT1,awCL,nHLIT) == FALSE)
        return(FALSE);

    // Initialize remaining literal/length alphabet code lengths to zero

    for (n=nHLIT;n<=IF_LLT_SIZE;n++)
        awCL[n] = 0;

    // Create literal/length Huffman tree

    if (IFCreateTree(lpstIF->awLLT0,
                     lpstIF->awLLT1,
                     awCL,
                     IF_LLT_SIZE+1) == FALSE)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Get distance Huffman tree

    // Read code lengths

    if (IFGetCodeLengthSet(lpstIF,awCLT0,awCLT1,awCL,nHDIST) == FALSE)
        return(FALSE);

    // Initialize remaining literal/length alphabet code lengths to zero

    for (n=nHDIST;n<=IF_DT_SIZE;n++)
        awCL[n] = 0;

    // Create distance Huffman tree

    if (IFCreateTree(lpstIF->awDT0,
                     lpstIF->awDT1,
                     awCL,
                     IF_DT_SIZE+1) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL IFGetFixedTrees()
//
// Parameters:
//  lpstIF          Ptr to IF structure with file pointer
//                  initialized
//
// Description:
//  Generates the trees using the fixed Huffman codes.
//
//  The Huffman codes for the two alphabets are fixed, and are not
//  represented explicitly in the data.  The Huffman code lengths
//  for the literal/length alphabet are:
//
//      Lit Value    Bits        Codes
//      ---------    ----        -----
//        0 - 143     8          00110000 through 10111111
//      144 - 255     9          110010000 through 111111111
//      256 - 279     7          0000000 through 0010111
//      280 - 287     8          11000000 through 11000111
//
//  The code lengths are sufficient to generate the actual codes,
//  as described above; we show the codes in the table for added
//  clarity.  Literal/length values 286-287 will never actually
//  occur in the compressed data, but participate in the code
//  construction.
//
//  Distance codes 0-31 are represented by (fixed-length) 5-bit
//  codes, with possible additional bits as shown in the table
//  shown in Paragraph 3.2.5, above.  Note that distance codes 30-
//  31 will never actually occur in the compressed data.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL IFGetFixedTrees
(
    LPIF    lpstIF
)
{
    int     i;
    WORD    awCL[IF_LLT_SIZE+3];

    /////////////////////////////////////////////////////////////
    // Get literal/length Huffman tree

    // Initialize literal/length code lengths

    for (i=0;i<=143;i++)
        awCL[i] = 8;

    for (i=144;i<=255;i++)
        awCL[i] = 9;

    for (i=256;i<=279;i++)
        awCL[i] = 7;

    for (i=280;i<=287;i++)
        awCL[i] = 8;

    // Create literal/length Huffman tree

    if (IFCreateTree(lpstIF->awLLT0,
                     lpstIF->awLLT1,
                     awCL,
                     IF_LLT_SIZE+3) == FALSE)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Get distance Huffman tree

    // Distance code lengths are all five bits

    for (i=0;i<(IF_DT_SIZE+3);i++)
        awCL[i] = 5;

    // Create distance Huffman tree

    if (IFCreateTree(lpstIF->awDT0,
                     lpstIF->awDT1,
                     awCL,
                     IF_DT_SIZE+3) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL IFInflateBlock()
//
// Parameters:
//  lpstIF      Ptr to IF structure
//  lpvDstFile  Ptr to destination file
//  nLLTSize    Size of length/literal tree
//  nDTSize     Size of distance tree
//
// Description:
//  Inflates the next block, which is assumed to be compressed.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL IFInflateBlock
(
    LPIF        lpstIF,
    LPVOID      lpvDstFile,
    int         nLLTSize,
    int         nDTSize
)
{
    int         nValue, nExtraBits;
    DWORD       dwNextI;
    DWORD       dwSrcI;
    WORD        wLen, wDist;
    DWORD       dwByteCount;

    dwNextI = lpstIF->dwNextI;
    while (1)
    {
        nValue = IFGetValueFromTree(lpstIF,
                                    lpstIF->awLLT0,
                                    lpstIF->awLLT1,
                                    nLLTSize);

        if (nValue < 256)
        {
            if (nValue < -1)
                return(FALSE);

            lpstIF->abyHistory[dwNextI++] = (BYTE)nValue;
            if (dwNextI == IF_HISTORY_SIZE)
            {
                if (DFFileWrite(lpstIF->lpvRootCookie,
                                lpvDstFile,
                                lpstIF->abyHistory,
                                IF_HISTORY_SIZE,
                                &dwByteCount) != DF_STATUS_OK ||
                    dwByteCount != IF_HISTORY_SIZE)
                    return(FALSE);

                lpstIF->dwCRC = CRC32Compute(IF_HISTORY_SIZE,
                                             lpstIF->abyHistory,
                                             lpstIF->dwCRC);

                dwNextI = 0;
            }
        }
        else
        if (nValue == 256)
            break;
        else
        {
            // Length

            if (257 <= nValue)
            {
                if (nValue <= 264)
                    wLen = (WORD)(nValue - 254);
                else
                if (nValue <= 284)
                {
                    nExtraBits = (nValue - 261) >> 2;

                    wLen = (WORD)((1 << nExtraBits) *
                                  ((nValue - 261) & 3) +
                                  11 + (0x78 & (0x78 >> (5 - nExtraBits))) +
                                  IFGetMultipleBits(lpstIF,nExtraBits));
                }
                else
                if (nValue == 285)
                    wLen = 258;
                else
                    return(FALSE);
            }
            else
                return(FALSE);

            // Distance

            nValue = IFGetValueFromTree(lpstIF,
                                        lpstIF->awDT0,
                                        lpstIF->awDT1,
                                        nDTSize);

            if (0 <= nValue)
            {
                if (nValue <= 3)
                    wDist = (WORD)(nValue + 1);
                else
                if (nValue <= 29)
                {
                    nExtraBits = (nValue - 2) >> 1;

                    wDist = (WORD)((1 << nExtraBits) * (nValue & 1) +
                                   5 + (0x3FFC & (0x3FFC >> (13 - nExtraBits))) +
                                   IFGetMultipleBits(lpstIF,nExtraBits));
                }
                else
                    return(FALSE);
            }
            else
                return(FALSE);

            // Copy

            if (wDist > dwNextI)
                dwSrcI = dwNextI + IF_HISTORY_SIZE - wDist;
            else
                dwSrcI = dwNextI - wDist;

            while (wLen-- != 0)
            {
                nValue = lpstIF->abyHistory[dwSrcI++];
                if (dwSrcI == IF_HISTORY_SIZE)
                    dwSrcI = 0;

                lpstIF->abyHistory[dwNextI++] = (BYTE)nValue;
                if (dwNextI == IF_HISTORY_SIZE)
                {
                    if (DFFileWrite(lpstIF->lpvRootCookie,
                                    lpvDstFile,
                                    lpstIF->abyHistory,
                                    IF_HISTORY_SIZE,
                                    &dwByteCount) != DF_STATUS_OK ||
                        dwByteCount != IF_HISTORY_SIZE)
                        return(FALSE);

                    lpstIF->dwCRC = CRC32Compute(IF_HISTORY_SIZE,
                                                 lpstIF->abyHistory,
                                                 lpstIF->dwCRC);

                    dwNextI = 0;
                }
            }
        }
    }

    lpstIF->dwNextI = dwNextI;
    return(TRUE);
}


//********************************************************************
//
// BOOL IFExtractBlock()
//
// Parameters:
//  lpstIF      Ptr to IF structure
//  lpvDstFile  Ptr to destination file
//
// Description:
//  Extracts the next block, which is assumed to be in
//  non-compressed form.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL IFExtractBlock
(
    LPIF        lpstIF,
    LPVOID      lpvDstFile
)
{
    DWORD       dwOffset;
    DWORD       dwNumBytes;
    WORD        wLen;
    long        lNewOffset;
    DWORD       dwByteCount;

    // Flush the current byte

    dwOffset = lpstIF->dwNextReadOffset -
        lpstIF->nReadAheadByteCount +
        lpstIF->nReadAheadI;

    // Get the length

    if (DFFileSeek(lpstIF->lpvRootCookie,
                   lpstIF->lpvSrcFile,
                   dwOffset,
                   DF_SEEK_SET,
                   &lNewOffset) != DF_STATUS_OK)
        return(FALSE);

    if (DFFileRead(lpstIF->lpvRootCookie,
                   lpstIF->lpvSrcFile,
                   &wLen,
                   sizeof(WORD),
                   &dwByteCount) != DF_STATUS_OK ||
        dwByteCount != sizeof(WORD))
        return(FALSE);

    // Endianize

    wLen = WENDIAN(wLen);

    // Skip over LEN and NLEN

    dwOffset += sizeof(WORD) + sizeof(WORD);

    while (wLen != 0)
    {
        dwNumBytes = IF_HISTORY_SIZE - lpstIF->dwNextI;
        if (wLen < dwNumBytes)
            dwNumBytes = wLen;

        // Read from the source

        if (DFFileSeek(lpstIF->lpvRootCookie,
                       lpstIF->lpvSrcFile,
                       dwOffset,
                       DF_SEEK_SET,
                       &lNewOffset) != DF_STATUS_OK)
            return(FALSE);

        if (DFFileRead(lpstIF->lpvRootCookie,
                       lpstIF->lpvSrcFile,
                       lpstIF->abyHistory + lpstIF->dwNextI,
                       dwNumBytes,
                       &dwByteCount) != DF_STATUS_OK ||
            dwByteCount != dwNumBytes)
            return(FALSE);

        wLen -= (WORD)dwNumBytes;

        dwOffset += dwNumBytes;

        lpstIF->dwNextI += dwNumBytes;
        if (lpstIF->dwNextI == IF_HISTORY_SIZE)
        {
            // Write to the destination

            if (DFFileWrite(lpstIF->lpvRootCookie,
                            lpvDstFile,
                            lpstIF->abyHistory,
                            IF_HISTORY_SIZE,
                            &dwByteCount) != DF_STATUS_OK ||
                dwByteCount != IF_HISTORY_SIZE)
                return(FALSE);

            lpstIF->dwCRC = CRC32Compute(IF_HISTORY_SIZE,
                                         lpstIF->abyHistory,
                                         lpstIF->dwCRC);

            lpstIF->dwNextI = 0;
        }
    }

    // Reset the structure

    lpstIF->nReadAheadI = lpstIF->nReadAheadByteCount = 0;
    lpstIF->dwNextReadOffset = dwOffset;
    lpstIF->nBitMask = 256;

    return(TRUE);
}


//********************************************************************
//
// BOOL IFInit()
//
// Parameters:
//  lpstIF          Ptr to IF structure
//  lpvRootCookie   Root cookie
//  lpvSrcFile      Source file
//  dwStartOffset   Start offset of compressed stream
//  dwSize          Size of compressed stream
//
// Description:
//  Initializes the IF structure for decompression.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL IFInit
(
    LPIF        lpstIF,
    LPVOID      lpvSrcFile,
    DWORD       dwStartOffset,
    DWORD       dwSize
)
{
    lpstIF->lpvSrcFile = lpvSrcFile;
    lpstIF->nReadAheadI = lpstIF->nReadAheadByteCount = 0;
    lpstIF->dwNextReadOffset = dwStartOffset;
    lpstIF->dwEndReadOffset = dwStartOffset + dwSize;
    lpstIF->nBitMask = 256;
    lpstIF->dwNextI = 0;

    return(TRUE);
}


//********************************************************************
//
// BOOL IFInflate()
//
// Parameters:
//  lpstIF          Ptr to IF structure
//  lpvDstFile      Destination file
//
// Description:
//  Decompresses the stream to the given file.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL IFInflate
(
    LPIF        lpstIF,
    LPVOID      lpvDstFile
)
{
    BOOL        bFinal;
    WORD        wType;
    DWORD       dwByteCount;

    CRC32Init(lpstIF->dwCRC);

    do
    {
        // Get the header

        // Get final bit

        bFinal = IFGetBit(lpstIF);
        wType = (WORD)IFGetMultipleBits(lpstIF,2);

        if (wType == 2)
        {
            // It is a compressed block using dynamic Huffman codes

            if (IFGetTrees(lpstIF) == FALSE)
                return(FALSE);

            if (IFInflateBlock(lpstIF,
                               lpvDstFile,
                               IF_LLT_SIZE + 1,
                               IF_DT_SIZE + 1) == FALSE)
                return(FALSE);
        }
        else
        if (wType == 1)
        {
            // It is a compressed block using fixed Huffman codes

            if (IFGetFixedTrees(lpstIF) == FALSE)
                return(FALSE);

            if (IFInflateBlock(lpstIF,
                               lpvDstFile,
                               IF_LLT_SIZE + 3,
                               IF_DT_SIZE + 3) == FALSE)
                return(FALSE);
        }
        else
        if (wType == 0)
        {
            // Non-compressed block

            if (IFExtractBlock(lpstIF,lpvDstFile) == FALSE)
                return(FALSE);
        }
        else
        {
            // Unhandled type

            return(FALSE);
        }
    }
    while (bFinal == 0);

    if (lpstIF->dwNextI > 0)
    {
        if (DFFileWrite(lpstIF->lpvRootCookie,
                        lpvDstFile,
                        lpstIF->abyHistory,
                        lpstIF->dwNextI,
                        &dwByteCount) != DF_STATUS_OK ||
            dwByteCount != lpstIF->dwNextI)
            return(FALSE);

        lpstIF->dwCRC = CRC32Compute(lpstIF->dwNextI,
                                     lpstIF->abyHistory,
                                     lpstIF->dwCRC);

    }

    return(TRUE);
}


//********************************************************************
//
// LPIF IFAllocIF()
//
// Parameters:
//  lpvRootCookie   Root cookie
//
// Description:
//  Allocates memory for the IF structure.
//
// Returns:
//  LPIF        On success
//  NULL        On failure
//
//********************************************************************

LPIF IFAllocIF
(
    LPVOID      lpvRootCookie
)
{
    LPIF        lpstIF;


    if (DFMemoryAlloc(lpvRootCookie,
                      sizeof(IF_T),
                      (LPLPVOID)&lpstIF) != DF_STATUS_OK)
        return(NULL);

    lpstIF->lpvRootCookie = lpvRootCookie;

    return(lpstIF);
}


//********************************************************************
//
// BOOL IFFreeIF()
//
// Parameters:
//  lpstIF      Ptr to IF structure to free.
//
// Description:
//  Frees memory allocated for the IF structure.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//********************************************************************

BOOL IFFreeIF
(
    LPIF        lpstIF
)
{
    if (lpstIF == NULL)
        return(TRUE);

    if (DFMemoryFree(lpstIF->lpvRootCookie,
                     lpstIF) != DF_STATUS_OK)
        return(FALSE);

    return(TRUE);
}

//***************************************************************

#if 0

IF_T gstLZ;

void main(int argc, char *argv[])
{
    FILE *      pfSrc;
    DWORD       dwSize;

    if (argc < 3)
    {
        printf("Usage: hufflz77 srcname dstname\n");
        return;
    }

    pfSrc = fopen(argv[1],"rb");
    if (pfSrc == NULL)
    {
        printf("Failed to open %s\n",argv[1]);
        return;
    }

    fseek(pfSrc,0,SEEK_END);
    dwSize = ftell(pfSrc);

    if (IFInit(&gstLZ,pfSrc,0,dwSize) == FALSE)
    {
        fclose(pfSrc);
        return;
    }

    FILE *pfDst = fopen(argv[2],"wb");
    if (pfDst == NULL)
    {
        printf("Failed opening %s for writing!\n",argv[2]);
        fclose(gstLZ.pf);
        return;
    }

    IFInflate(&gstLZ,pfDst);

    fclose(pfDst);
    fclose(pfSrc);
}

#endif

//***************************************************************

