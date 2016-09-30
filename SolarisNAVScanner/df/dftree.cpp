//************************************************************************
//
// $Header:   S:/DF/VCS/dftree.cpv   1.5   12 Nov 1998 18:13:18   DCHI  $
//
// Description:
//  Deflation tree manipulation functions.  See RFC 1951.
//
//************************************************************************
// $Log:   S:/DF/VCS/dftree.cpv  $
// 
//    Rev 1.5   12 Nov 1998 18:13:18   DCHI
// Added endianization of LEN value.
// 
//    Rev 1.4   11 Nov 1998 11:16:14   DCHI
// Added change to consolidate initial code length code 0 in 0 chain.
// 
//    Rev 1.3   09 Nov 1998 13:46:20   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data for SYM_WIN16.
// 
//    Rev 1.2   05 Nov 1998 14:46:18   DCHI
// Changed global declarations to FAR.
// 
//    Rev 1.1   29 Jul 1998 10:12:16   DCHI
// Replaced calls to memset/memcpy with calls to internal implementations.
// 
//    Rev 1.0   11 Nov 1997 16:21:10   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "platform.h"
#include "df.h"

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

WORD FAR gawDFLenRangeMin[DF_NUM_LEN_CODES] =
{
      3,   4,   5,   6,   7,   8,   9,  10,  11, 13,
     15,  17,  19,  23,  27,  31,  35,  43,  51, 59,
     67,  83,  99, 115, 131, 163, 195, 227, 258
};

WORD FAR gawDFLenRangeMax[DF_NUM_LEN_CODES] =
{
      3,   4,   5,   6,   7,   8,   9,  10,  12, 14,
     16,  18,  22,  26,  30,  34,  42,  50,  58, 66,
     82,  98, 114, 130, 162, 194, 226, 257, 258
};

BYTE FAR gabyDFLenExtraBits[DF_NUM_LEN_CODES] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
    4, 4, 4, 4, 5, 5, 5, 5, 0
};

WORD FAR gawDFDistRangeMin[DF_NUM_DIST_CODES] =
{
        1,     2,     3,     4,     5,     7,     9,    13,    17,    25,
       33,    49,    65,    97,   129,   193,   257,   385,   513,   769,
     1025,  1537,  2049,  3073,  4097,  6145,  8193, 12289, 16385, 24577
};

WORD FAR gawDFDistRangeMax[DF_NUM_DIST_CODES] =
{
        1,     2,     3,     4,     6,     8,    12,    16,    24,    32,
       48,    64,    96,   128,   192,   256,   384,   512,   768,  1024,
     1536,  2048,  3072,  4096,  6144,  8192, 12288, 16384, 24576, 32768
};

BYTE FAR gabyDFDistExtraBits[DF_NUM_DIST_CODES] =
{
     0,  0,  0,  0,  1,  1,  2,  2,  3,  3,
     4,  4,  5,  5,  6,  6,  7,  7,  8,  8,
     9,  9, 10, 10, 11, 11, 12, 12, 13, 13
};

BYTE FAR gabyDFCodeLenCodeOrder[DF_NUM_CODE_LEN_CODES] =
{
    16, 17, 18,  0,  8,  7,  9,  6, 10,  5,
    11,  4, 12,  3, 13,  2, 14,  1, 15
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif


//********************************************************************
//
// Function:
//  BYTE DFLLCodeExtraBits()
//
// Parameters:
//  nCode           Code of which to get the extra bits
//
// Description:
//  Returns the number of extra bits required to complete the
//  given literal/length code.
//
// Returns:
//  BYTE            Extra bits required
//
//********************************************************************

BYTE DFLLCodeExtraBits
(
    int             nCode
)
{
    if (nCode < DF_NUM_LITERAL_CODES)
        return 0;

    return(gabyDFLenExtraBits[nCode - DF_NUM_LITERAL_CODES]);
}


//********************************************************************
//
// Function:
//  BYTE DFDistCodeExtraBits()
//
// Parameters:
//  nCode           Code of which to get the extra bits
//
// Description:
//  Returns the number of extra bits required to complete the
//  given distance code.
//
// Returns:
//  BYTE            Extra bits required
//
//********************************************************************

BYTE DFDistCodeExtraBits
(
    int             nCode
)
{
    return(gabyDFDistExtraBits[nCode]);
}


//********************************************************************
//
// Function:
//  BYTE DFDistCodeLenCodeExtraBits()
//
// Parameters:
//  nCode           Code of which to get the extra bits
//
// Description:
//  Returns the number of extra bits required to complete the
//  given code length code.
//
// Returns:
//  BYTE            Extra bits required
//
//********************************************************************

BYTE DFDistCodeLenCodeExtraBits
(
    int             nCode
)
{
    if (nCode == 16)
        return(2);

    if (nCode == 17)
        return(3);

    if (nCode == 18)
        return(7);

    return(0);
}


//********************************************************************
//
// Function:
//  void DFInsertItems()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  lpwCodeFreq     Array of code frequencies
//
// Description:
//  Inserts the code frequencies from lpwCodeFreq into the heap
//  lpstDF->an and into the item list lpstDF->astItems.
//  Before calling this function, lpstDF->nNumCodes should be set
//  to the number of codes.
//
// Returns:
//  Nothing.
//
//********************************************************************

void DFInsertItems
(
    LPDF            lpstDF,
    LPWORD          lpwCodeFreq
)
{
    LPDFITEM        lpastItems  = lpstDF->astItems;
    LPINT           lpan        = lpstDF->an;

    int             i;          // Item iterator
    int             nItem;      // Current item
    int             nParent;    // Parent of current item
    WORD            wFreq;      // Item frequency
    int             nMaxCode;

    /////////////////////////////////////////////////////////////
    // Insert items into the heap
    //  The heap is ordered such that the root has the
    //  lowest frequency.

    nMaxCode = 0;
    lpstDF->nNumItems = 0;
    for (i=0;i<lpstDF->nNumCodes;i++)
    {
        // Don't add zero frequency items to the tree

        if ((wFreq = lpwCodeFreq[i]) == 0)
        {
            // Set the length to zero
            //  This is important because this value is checked
            //  during the code generation step.

            lpastItems[i].uPL.nLen = 0;
            continue;
        }

        nItem = ++(lpstDF->nNumItems);
        nParent = nItem >> 1;

        // Bubble the item up

        while (nParent != 0)
        {
            if (lpastItems[lpan[nParent]].wFreq <= wFreq)
            {
                // The parent has lower frequency, so insert
                //  the item here

                break;
            }

            // Otherwise, move the parent here

            lpan[nItem] = lpan[nParent];

            // Move up to the parent

            nItem = nParent;

            // Get the next parent

            nParent >>= 1;
        }

        nMaxCode = (WORD)(lpan[nItem] = i);
        lpastItems[i].wFreq = wFreq;
        lpastItems[i].wDepth = 0;
    }


    /////////////////////////////////////////////////////////////
    // Ensure there are >= 2 items

    // If the number of items is < 2, then force it to be >= 2,
    //  since pkzip requires this

    if (lpstDF->nNumItems < 2)
    {
        // Need to create the first item?

        if (lpstDF->nNumItems == 0)
        {
            // Set the first item to be code 0

            lpan[1] = 0;
            lpastItems[0].wFreq = 1;
            lpastItems[0].wDepth = 0;
        }

        // Do the second item

        if (lpan[1] == 0)
        {
            // Set the second item to be code 1

            lpan[2] = 1;
            nMaxCode = 1;
        }
        else
        {
            // Set the second item to be code 0

            lpan[2] = 0;
            nMaxCode = lpan[1];
        }

        // Set the frequency of the two items equal

        lpastItems[lpan[2]].wFreq = lpastItems[lpan[1]].wFreq;
        lpastItems[lpan[2]].wDepth = 0;

        // There are now two items

        lpstDF->nNumItems = 2;
    }

    lpstDF->nMaxCode = nMaxCode;
}


//********************************************************************
//
// Function:
//  void DFHeapSinkItem()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  nCodeToSink     Code to sink
//
// Description:
//  The function assumes that the root position (e.g., index one)
//  of the heap lpstDF->an is empty and that the code to sink
//  is to be sunk starting from that position.  Thus the code
//  to sink should not appear anywhere in the heap.
//
// Returns:
//  Nothing.
//
//********************************************************************

void DFHeapSinkItem
(
    LPDF            lpstDF,
    int             nCodeToSink
)
{
    int             nChild;         // Index of child being examined
    int             nItem;          // Index of the candidate item
    LPINT           lpan;           // lpstDF->lpan
    LPDFITEM        lpstChild;      // The left child
    LPDFITEM        lpstChild2;     // The right child
    LPDFITEM        lpstItemToSink; // Item to sink

    // Sink the element starting from the number one position

    lpstItemToSink = lpstDF->astItems + nCodeToSink;
    lpan = lpstDF->an;
    nItem = 1;
    nChild = 2;
    while (nChild <= lpstDF->nNumItems)
    {
        lpstChild = lpstDF->astItems + lpan[nChild];

        // Find the smaller child, if necessary

        if (nChild < lpstDF->nNumItems)
        {
            lpstChild2 = lpstDF->astItems + lpan[nChild + 1];

            // If the frequencies are equal, the child with
            //  the smaller depth is used because it tends
            //  to minimize the maximum depth.

            if (lpstChild2->wFreq < lpstChild->wFreq ||
                (lpstChild2->wFreq == lpstChild->wFreq &&
                 lpstChild2->wDepth < lpstChild->wDepth))
            {
                // Set the child to the smaller child

                ++nChild;
                lpstChild = lpstChild2;
            }
        }

        // If the frequency of the item to sink is smaller
        //  than the frequency of the child, stop

        if (lpstItemToSink->wFreq < lpstChild->wFreq ||
            (lpstItemToSink->wFreq == lpstChild->wFreq &&
             lpstItemToSink->wDepth <= lpstChild->wDepth))
            break;

        // Exchange the item with the smaller child

        lpan[nItem] = lpan[nChild];

        // Move to the child

        nItem = nChild;

        // Move to the next child

        nChild <<= 1;
    }

    // Store the item

    lpan[nItem] = nCodeToSink;
}


//********************************************************************
//
// Function:
//  void DFConstructHTree()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//
// Description:
//  lpstDF->an should be heap ordered such that the item with
//  lowest frequency is at the root of the tree.  The function
//  constructs the Huffman tree based on the heap ordered
//  items.
//
// Returns:
//  Nothing.
//
//********************************************************************

void DFConstructHTree
(
    LPDF            lpstDF
)
{
    LPDFITEM        lpastItems  = lpstDF->astItems;
    LPINT           lpan        = lpstDF->an;

    int             nNextSlot;
    int             nCode;
    LPDFITEM        lpstItem;
    LPDFITEM        lpst2ndItem;
    LPDFITEM        lpstParent;

    /////////////////////////////////////////////////////////////
    // Construct the Huffman tree

    //  As subtrees are taken out of the heap, they are stored
    //  in the same array at the end of the heap array.
    //  The variable nNextSlot is the index of the
    //  next array element in which to store the next
    //  removed subtree.

    nNextSlot = 2 * lpstDF->nNumCodes - 1;
    nCode = lpstDF->nNumCodes + 1;
    do
    {
        // Get the subtree with the lowest frequency

        lpstItem = lpastItems + (lpan[nNextSlot--] = lpan[1]);

        // Restore the heap order

        DFHeapSinkItem(lpstDF,
                       lpan[lpstDF->nNumItems--]);

        // Get the subtree with the next lowest frequency

        lpst2ndItem = lpastItems + (lpan[nNextSlot--] = lpan[1]);

        // Create the subtree node

        lpstParent = lpastItems + nCode;
        lpstParent->wFreq = lpstItem->wFreq + lpst2ndItem->wFreq;

        if (lpstItem->wDepth > lpst2ndItem->wDepth)
            lpstParent->wDepth = lpstItem->wDepth + 1;
        else
            lpstParent->wDepth = lpst2ndItem->wDepth + 1;

        DFHeapSinkItem(lpstDF,nCode);

        lpstItem->uPL.nParent = lpst2ndItem->uPL.nParent = nCode++;
    }
    while (lpstDF->nNumItems >= 2);

    // lpstDF->nNumItems is one now, so it is the root of the tree

    lpan[nNextSlot] = lpan[1];

    lpstDF->nRootIndex = nNextSlot;
}


//********************************************************************
//
// Function:
//  void DFGenBitLen()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  nMaxBitLen      Maximum bit length for each code
//
// Description:
//  lpstDF->nRootIndex should be the index in lpstDF->an of the
//  root item of the Huffman tree.  The function traverses the
//  tree and computes the bit length of each leaf node.  After
//  doing so, if there were nodes with bit lengths greater
//  than nMaxBitLen, then the tree is readjusted so that
//  no bit length exceeds the maximum.
//
// Returns:
//  Nothing.
//
//********************************************************************

void DFGenBitLen
(
    LPDF            lpstDF,
    int             nMaxBitLen
)
{
    LPDFITEM        lpastItems  = lpstDF->astItems;
    LPINT           lpan        = lpstDF->an;
    LPINT           lpanBLCount = lpstDF->anBLCount;

    int             nOverflow;  // Number of overflowed items
    int             nItem;      // Current item
    int             nCode;      // Current code
    int             nBits;      // Current bits
    int             m, n;
    LPDFITEM        lpstItem;   // Current item

    // Initialize bit length counts to zero

    for (n=0;n<=DF_MAX_BITS;n++)
        lpanBLCount[n] = 0;

    // Compute the optimal bit lengths

    nOverflow = 0;
    nItem = lpstDF->nRootIndex;
    lpastItems[lpan[nItem]].uPL.nLen = 0; // Root has zero length
    for (++nItem;nItem < 2 * lpstDF->nNumCodes;nItem++)
    {
        lpstItem = lpastItems + (nCode = lpan[nItem]);
        nBits = lpastItems[lpstItem->uPL.nParent].uPL.nLen + 1;
        if (nBits > nMaxBitLen)
        {
            nBits = nMaxBitLen;
            ++nOverflow;
        }
        lpstItem->uPL.nLen = nBits;

        // If it's not a leaf node, do not increment the count

        if (nCode >= lpstDF->nNumCodes)
            continue;

        // Update the count

        lpanBLCount[nBits]++;
    }

    if (nOverflow == 0)
        return;

    // Find the first bit length which could increase

    do
    {
        nBits = nMaxBitLen - 1;
        while (lpanBLCount[nBits] == 0)
            --nBits;

        lpanBLCount[nBits]--;       // Move one leaf down the tree
        lpanBLCount[nBits+1] += 2;  // Move one overflow item as it's sibling
        lpanBLCount[nMaxBitLen]--;

        // The sibling of the overflow item also moves one step up,
        //  but this does not affect anBLCount[nMaxLen];

        nOverflow -= 2;
    }
    while (nOverflow > 0);

    // Now recompute all bit lengths, scanning in increasing frequency.

    nItem = 2 * lpstDF->nNumCodes;
    for (nBits=nMaxBitLen;nBits!=0;nBits--)
    {
        n = lpanBLCount[nBits];
        while (n != 0)
        {
            m = lpan[--nItem];
            if (m >= lpstDF->nNumCodes)
                continue;

            lpastItems[m].uPL.nLen = nBits;

            --n;
        }
    }
}


//********************************************************************
//
// Function:
//  void DFGenCodes()
//
// Parameters:
//  lpstDF              Ptr to DF structure
//  lpbyCodeLen         Ptr to array for code lengths
//  lpwCode             Ptr to array for bit codes
//  pfnCodeExtraBits    Ptr to function to get the extra bits
//
// Description:
//  Based on the bit length of the items, the function computes
//  the bit codes for each code, storing the length and bit code
//  for each code in corresponding entries of the arrays pointed
//  to by lpbyCodeLen and lpwCode.  The codes are reversed as
//  they are stored for easier output later.
//
// Returns:
//  Nothing.
//
//********************************************************************

void DFGenCodes
(
    LPDF            lpstDF,
    LPBYTE          lpbyCodeLen,
    LPWORD          lpwCode,
    BYTE            (*pfnCodeExtraBits)(int)
)
{
    LPDFITEM        lpastItems  = lpstDF->astItems;

    WORD            awNextCode[DF_MAX_BITS+1];  // Next code value
    WORD            wCode;                      // Running code value
    WORD            wRevCode;                   // The code reversed
    int             nBits;                      // Bit index
    int             n;                          // Code Index
    int             nLen;                       // Current bit length

    // First generate the code values based on the bit length counts

    wCode = 0;
    for (nBits=1;nBits<=DF_MAX_BITS;nBits++)
        awNextCode[nBits] = wCode =
            (wCode + lpstDF->anBLCount[nBits - 1]) << 1;

    // Now we can generate the codes

    for (n=0;n<lpstDF->nNumCodes;n++)
    {
        nLen = lpastItems[n].uPL.nLen;

        if (nLen == 0)
        {
            // We can use this value later to verify the
            //  correctness of the implementation

            lpbyCodeLen[n] = 0;
            continue;
        }

        // Update the compressed length

        lpstDF->dwCompressedLen +=
            (DWORD)nLen * (DWORD)pfnCodeExtraBits(n);

        // Store the length

        lpbyCodeLen[n] = (BYTE)nLen;

        // Reverse the code

        wCode = awNextCode[nLen]++;
        wRevCode = 0;
        while (1)
        {
            wRevCode |= (wCode & 1);
            if (--nLen == 0)
                break;

            wRevCode <<= 1;
            wCode >>= 1;
        }

        // Store the reversed code

        lpwCode[n] = wRevCode;
    }
}


//********************************************************************
//
// Function:
//  void DFBuildTree()
//
// Parameters:
//  lpstDF              Ptr to DF structure
//  nNumCodes           Number of codes
//  lpwCodeFreq         Array of frequencies for each code
//  nMaxBitLen          Maximum bit length for each code
//  lpbyCodeLen         Ptr to array for code lengths
//  lpwCode             Ptr to array for bit codes
//  pfnCodeExtraBits    Ptr to function to get the extra bits
//
// Description:
//  Generates the optimum Huffman code lengths and bit codes given
//  the frequencies and the maximum code length limit of
//  DF_MAX_BITS(15).
//
// Returns:
//  Nothing.
//
//********************************************************************

void DFBuildTree
(
    LPDF        lpstDF,
    int         nNumCodes,
    LPWORD      lpwCodeFreq,
    int         nMaxBitLen,
    LPBYTE      lpbyCodeLen,
    LPWORD      lpwCode,
    BYTE        (*pfnCodeExtraBits)(int)
)
{
    lpstDF->nNumCodes = nNumCodes;

    DFInsertItems(lpstDF,lpwCodeFreq);
    DFConstructHTree(lpstDF);
    DFGenBitLen(lpstDF,nMaxBitLen);
    DFGenCodes(lpstDF,lpbyCodeLen,lpwCode,pfnCodeExtraBits);
}


//********************************************************************
//
// Function:
//  void DFCodeLenCodeFreq()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  nNumCodes       Number of codes
//  lpabyCodeLen    Ptr to array of code lengths
//
// Description:
//  The algorithm is to count in groups of consecutive equal
//  code lengths.
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
//  Nothing.
//
//********************************************************************

void DFCodeLenCodeFreq
(
    LPDF        lpstDF,
    int         nNumCodes,
    LPBYTE      lpabyCodeLen
)
{
    int         i;
    int         nCurCodeLen;
    int         nCount;

    // Start with the first code

    nCurCodeLen = lpabyCodeLen[0];
    nCount = 1;

    // Go through remaining codes

    i = 1;
    while (1)
    {
        if (nCurCodeLen == lpabyCodeLen[i])
            ++nCount;

        // Count the code length code frequencies for the group
        //  if a new group will begin or if this is the last group

        if (nCurCodeLen != lpabyCodeLen[i] || (i + 1) == nNumCodes)
        {
            // Handle large sets of code length 0

            if (nCurCodeLen == 0 && nCount > 2)
            {
                while (nCount > 2)
                {
                    // nCount >= 3

                    if (nCount <= 10)
                    {
                        // Code 17: Repeat 0 for 3-10 times

                        lpstDF->awCodeLen[17]++;
                        nCount = 0;
                    }
                    else
                    {
                        // Code 18: Repeat 0 for 11-138 times

                        lpstDF->awCodeLen[18]++;
                        if (nCount <= 138)
                            nCount = 0;
                        else
                            nCount -= 138;
                    }
                }
            }
            else
            {
                // Count the initial code

                lpstDF->awCodeLen[nCurCodeLen]++;
                --nCount;
            }

            // Handle remaining code lengths

            while (nCount != 0)
            {
                if (nCount < 3)
                {
                    lpstDF->awCodeLen[nCurCodeLen]++;
                    --nCount;
                }
                else
                {
                    // Code 16: Repeat previous for 3-6 times

                    lpstDF->awCodeLen[16]++;
                    if (nCount <= 6)
                        nCount = 0;
                    else
                        nCount -= 6;
                }
            }

            // Terminate if there are no more codes

            if (i + 1 == nNumCodes)
            {
                // Count the last one if necessary

                if (nCurCodeLen != lpabyCodeLen[i])
                    lpstDF->awCodeLen[lpabyCodeLen[i]]++;

                break;
            }

            // Now start a new group with the next code length

            nCurCodeLen = lpabyCodeLen[i];
            nCount = 1;
        }

        // Go to the next code length

        ++i;
    }
}


//********************************************************************
//
// Function:
//  void DFBuildCodeLenTree()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//
// Description:
//  The function builds the code length tree based on the
//  computed freqencies of the use of the code length codes
//  to compress the literal/length and distance Huffman trees.
//
// Returns:
//  Nothing.
//
//********************************************************************

void DFBuildCodeLenTree
(
    LPDF        lpstDF
)
{
    int         i;

    // Initialize the code length code frequencies to zero

    DFZeroBuf((LPBYTE)lpstDF->awCodeLen,sizeof(lpstDF->awCodeLen));

    // Now compute the frequencies for the code lengths codes
    //  for the literal/length and distance codes

    DFCodeLenCodeFreq(lpstDF,
                      lpstDF->nMaxLLCode + 1,
                      lpstDF->abyLLLen);

    DFCodeLenCodeFreq(lpstDF,
                      lpstDF->nMaxDistCode + 1,
                      lpstDF->abyDistLen);

    // Build the tree

    DFBuildTree(lpstDF,
                DF_NUM_CODE_LEN_CODES,
                lpstDF->awCodeLen,
                DF_MAX_CODE_LEN_CODE_BITS,
                lpstDF->abyCodeLenLen,
                lpstDF->awCodeLen,
                DFDistCodeLenCodeExtraBits);

    // Find the index in the ordering given by
    //  gabyDFCodeLenCodeOrder[] of the maximum
    //  code length code used

    for (i=DF_NUM_CODE_LEN_CODES-1;i>=0;i--)
        if (lpstDF->abyCodeLenLen[gabyDFCodeLenCodeOrder[i]] != 0)
            break;

    // At least four code length code lengths must be given

    if (i < 3)
        lpstDF->nMaxCodeLenCodeIndex = 3;
    else
        lpstDF->nMaxCodeLenCodeIndex = i;

    // Add code length code lengths size

    lpstDF->dwCompressedLen += (lpstDF->nMaxCodeLenCodeIndex + 1) * 3;
}


//********************************************************************
//
// Function:
//  void DFBuildTrees()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//
// Description:
//  The function assumes that the following fields at lpstDF
//  are valid:
//
//      awLL[]          Array of literal/length code frequencies
//      awDist[]        Array of distance code frequencies
//
//  The function generates the bit length and bit codes for the
//  literal/length, distance, and code length code alphabets.
//  The bit length and bit codes are stored in the following
//  arrays:
//
//      abyLLLen[],     awLL[]          Literal/length codes
//      abyDistLen[]    awDist[]        Distance codes
//      abyCodeLenLen[] awCodeLen[]     Code length codes
//
//  Note that awLL[] and awDist[] are changed by the function.
//
//  The function also sets lpstDF->dwCompressedLen equal to the
//  number of bits required to compress the block.  This value
//  can be used to determine whether to deflate the block or
//  to store the data in non-compressed form.
//
// Returns:
//  Nothing.
//
//********************************************************************

void DFBuildTrees
(
    LPDF    lpstDF
)
{
    lpstDF->dwCompressedLen = 0;

    // We always have literal/length code 256 to indicate the
    //  end of the block and there should only be one such
    //  symbol in the compressed block

    lpstDF->awLL[256] = 1;

    // Build the literal/length tree

    DFBuildTree(lpstDF,
                DF_NUM_LL_CODES,
                lpstDF->awLL,
                DF_MAX_BITS,
                lpstDF->abyLLLen,
                lpstDF->awLL,
                DFLLCodeExtraBits);

    lpstDF->nMaxLLCode = lpstDF->nMaxCode;

    // Build the distance tree

    DFBuildTree(lpstDF,
                DF_NUM_DIST_CODES,
                lpstDF->awDist,
                DF_MAX_BITS,
                lpstDF->abyDistLen,
                lpstDF->awDist,
                DFDistCodeExtraBits);

    lpstDF->nMaxDistCode = lpstDF->nMaxCode;

    // Build the code length tree

    DFBuildCodeLenTree(lpstDF);

    // Add in the sizes for:
    //  5 Bits: HLIT, # of Literal/Length codes - 257 (257-286)
    //  5 Bits: HDIST, # of Distance codes - 1        (1 - 32)
    //  4 Bits: # of Code Length codes - 4            (4 - 19)

    lpstDF->dwCompressedLen += 5 + 5 + 4;
}


//********************************************************************
//
// Function:
//  BOOL DFOutputValue()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  dwValue         Value to output
//  nNumBits        Number of bits to output
//
// Description:
//  The function outputs the least significant nNumBits bits of the
//  given value dwValue to the output buffer.  The function flushes
//  the output buffer when it becomes full.
//
// Returns:
//  TRUE            On success
//  FALSE           On error writing
//
//********************************************************************

BOOL DFOutputValue
(
    LPDF        lpstDF,
    DWORD       dwValue,
    int         nNumBits
)
{
    LPBYTE      lpbyCurByte;
    BYTE        byBit;
    int         nCurBit;
    int         i;
    DWORD       dwBytesWritten;

    assert(nNumBits > 0);

    lpbyCurByte = lpstDF->abyOutBuf + lpstDF->nCurByte;
    nCurBit = lpstDF->nCurBit;
    byBit = 1 << nCurBit;
    for (i=0;i<nNumBits;i++)
    {
        if (nCurBit == 8)
        {
            lpstDF->nCurByte++;
            if (lpstDF->nCurByte == DF_OUT_BUF_SIZE)
            {
                // Write the buffer

                if (DFFileWrite(lpstDF->lpvRootCookie,
                                lpstDF->lpvFile,
                                lpstDF->abyOutBuf,
                                DF_OUT_BUF_SIZE,
                                &dwBytesWritten) != DF_STATUS_OK ||
                    dwBytesWritten != DF_OUT_BUF_SIZE)
                    return(FALSE);

                // Update the compressed size

                lpstDF->dwCompressedSize += DF_OUT_BUF_SIZE;

                // Zero out the buffer

                DFZeroBuf(lpstDF->abyOutBuf,DF_OUT_BUF_SIZE);

                // Start at the beginning of the buffer

                lpstDF->nCurByte = 0;
            }
            lpbyCurByte = lpstDF->abyOutBuf + lpstDF->nCurByte;
            nCurBit = 0;
            byBit = 1;
        }

        if (dwValue & 1)
            *lpbyCurByte |= byBit;

        dwValue >>= 1;
        byBit <<= 1;

        nCurBit++;
    }

    lpstDF->nCurBit = nCurBit;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL DFOutputFlush()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//
// Description:
//  The function flushes the buffer to the output.
//
// Returns:
//  TRUE            On success
//  FALSE           On error writing
//
//********************************************************************

BOOL DFOutputFlush
(
    LPDF        lpstDF
)
{
    DWORD       dwBytesWritten;

    if (lpstDF->nCurBit != 0)
        lpstDF->nCurByte++;

    if (lpstDF->nCurByte != 0)
    {
        if (DFFileWrite(lpstDF->lpvRootCookie,
                        lpstDF->lpvFile,
                        lpstDF->abyOutBuf,
                        lpstDF->nCurByte,
                        &dwBytesWritten) != DF_STATUS_OK ||
            dwBytesWritten != (DWORD)lpstDF->nCurByte)
            return(FALSE);

        // Update the compressed size

        lpstDF->dwCompressedSize += lpstDF->nCurByte;

        lpstDF->nCurByte = 0;
        lpstDF->nCurBit = 0;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL DFOutputNoCompressHdr()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  wLen            Length of uncompressed block
//
// Description:
//  After calling this function, the caller can just blast the
//  uncompressed data because the function flushes the output
//  buffer before returning.
//
//
// Returns:
//  TRUE            On success
//  FALSE           On error writing
//
//********************************************************************

BOOL DFOutputNoCompressHdr
(
    LPDF        lpstDF,
    WORD        wLen
)
{
    DWORD       dwBytesWritten;

    // Output BFINAL

    if (lpstDF->bFinalBlock == FALSE)
    {
        // Not final block

        if (DFOutputValue(lpstDF,
                          0,
                          1) == FALSE)
            return(FALSE);
    }
    else
    {
        // Is final block

        if (DFOutputValue(lpstDF,
                          1,
                          1) == FALSE)
            return(FALSE);
    }

    // Output BTYPE of no compression (00)

    if (DFOutputValue(lpstDF,
                      0,
                      2) == FALSE)
        return(FALSE);

    // Flush the output buffer because the caller will send the
    //  uncompressed data to the output directly.  This also
    //  serves to align the output of the next length and one's
    //  complement of the length to the next byte boundary.

    if (DFOutputFlush(lpstDF) == FALSE)
        return(FALSE);

    // Endianize

    wLen = WENDIAN(wLen);

    // Output length

    if (DFFileWrite(lpstDF->lpvRootCookie,
                    lpstDF->lpvFile,
                    &wLen,
                    sizeof(WORD),
                    &dwBytesWritten) != DF_STATUS_OK ||
        dwBytesWritten != sizeof(WORD))
        return(FALSE);

    // Output one's complement of length

    wLen = ~wLen;
    if (DFFileWrite(lpstDF->lpvRootCookie,
                    lpstDF->lpvFile,
                    &wLen,
                    sizeof(WORD),
                    &dwBytesWritten) != DF_STATUS_OK ||
        dwBytesWritten != sizeof(WORD))
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL DFOutputCodeLengths()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  nNumCodes       Number of codes
//  lpabyCodeLen    Ptr to array of code lengths
//
// Description:
//  The function outputs the code lengths using the code length
//  codes.
//
//  The algorithm is to count in groups of consecutive equal
//  code lengths.
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
//  TRUE            On success
//  FALSE           On error writing
//
//********************************************************************

BOOL DFOutputCodeLengths
(
    LPDF        lpstDF,
    int         nNumCodes,
    LPBYTE      lpabyCodeLen
)
{
    int         i;
    int         nCurCodeLen;
    int         nCount;

    // Start with the first code

    nCurCodeLen = lpabyCodeLen[0];
    nCount = 1;

    // Go through remaining codes

    i = 1;
    while (1)
    {
        if (nCurCodeLen == lpabyCodeLen[i])
            ++nCount;

        // Count the code length code frequencies for the group
        //  if a new group will begin or if this is the last group

        if (nCurCodeLen != lpabyCodeLen[i] || (i + 1) == nNumCodes)
        {
            // Handle large sets of code length 0

            if (nCurCodeLen == 0 && nCount > 2)
            {
                while (nCount > 2)
                {
                    // nCount >= 3

                    if (nCount <= 10)
                    {
                        // Code 17: Repeat 0 for 3-10 times

                        if (DFOutputValue(lpstDF,
                                          lpstDF->awCodeLen[17],
                                          lpstDF->
                                              abyCodeLenLen[17]) == FALSE)
                            return(FALSE);

                        if (DFOutputValue(lpstDF,
                                          nCount - 3,
                                          3) == FALSE)
                            return(FALSE);

                        nCount = 0;
                    }
                    else
                    {
                        // Code 18: Repeat 0 for 11-138 times

                        if (DFOutputValue(lpstDF,
                                          lpstDF->awCodeLen[18],
                                          lpstDF->
                                              abyCodeLenLen[18]) == FALSE)
                            return(FALSE);

                        if (nCount <= 138)
                        {
                            if (DFOutputValue(lpstDF,
                                              nCount - 11,
                                              7) == FALSE)
                                return(FALSE);

                            nCount = 0;
                        }
                        else
                        {
                            if (DFOutputValue(lpstDF,
                                              127,
                                              7) == FALSE)
                                return(FALSE);

                            nCount -= 138;
                        }
                    }
                }
            }
            else
            {
                // Count the initial code

                if (DFOutputValue(lpstDF,
                                  lpstDF->awCodeLen[nCurCodeLen],
                                  lpstDF->abyCodeLenLen[nCurCodeLen]) == FALSE)
                    return(FALSE);

                --nCount;
            }

            // Handle remaining code lengths

            while (nCount != 0)
            {
                if (nCount < 3)
                {
                    if (DFOutputValue(lpstDF,
                                      lpstDF->awCodeLen[nCurCodeLen],
                                      lpstDF->
                                          abyCodeLenLen[nCurCodeLen]) == FALSE)
                        return(FALSE);

                    --nCount;
                }
                else
                {
                    // Code 16: Repeat previous for 3-6 times

                    if (DFOutputValue(lpstDF,
                                      lpstDF->awCodeLen[16],
                                      lpstDF->
                                          abyCodeLenLen[16]) == FALSE)
                        return(FALSE);

                    if (nCount <= 6)
                    {
                        if (DFOutputValue(lpstDF,
                                          nCount - 3,
                                          2) == FALSE)
                        return(FALSE);

                        nCount = 0;
                    }
                    else
                    {
                        if (DFOutputValue(lpstDF,
                                          3,
                                          2) == FALSE)
                        return(FALSE);

                        nCount -= 6;
                    }
                }
            }

            // Terminate if there are no more codes

            if (i + 1 == nNumCodes)
            {
                // Output the last one if necessary

                if (nCurCodeLen != lpabyCodeLen[i] &&
                    DFOutputValue(lpstDF,
                                  lpstDF->awCodeLen[lpabyCodeLen[i]],
                                  lpstDF->
                                      abyCodeLenLen[lpabyCodeLen[i]]) == FALSE)
                    return(FALSE);

                break;
            }

            // Now start a new group with the next code length

            nCurCodeLen = lpabyCodeLen[i];
            nCount = 1;
        }

        // Go to the next code length

        ++i;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL DFOutputDeflateHdr()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//
// Description:
//  Outputs the block header to indicate a deflated block and
//  then outputs the Huffman code information.
//
// Returns:
//  TRUE            On success
//  FALSE           On error writing
//
//********************************************************************

BOOL DFOutputDeflateHdr
(
    LPDF        lpstDF
)
{
    int         i;
    int         nCode;

    // Output BFINAL

    if (lpstDF->bFinalBlock == FALSE)
    {
        // Not final block

        if (DFOutputValue(lpstDF,
                          0,
                          1) == FALSE)
            return(FALSE);
    }
    else
    {
        // Is final block

        if (DFOutputValue(lpstDF,
                          1,
                          1) == FALSE)
            return(FALSE);
    }

    // Output BTYPE of dynamic Huffman code compression (10)

    if (DFOutputValue(lpstDF,
                      2,
                      2) == FALSE)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Output Huffman trees

    // Output HLIT

    if (DFOutputValue(lpstDF,
                      lpstDF->nMaxLLCode - DF_NUM_LITERAL_CODES + 1,
                      5) == FALSE)
        return(FALSE);

    // Output HDIST

    if (DFOutputValue(lpstDF,
                      lpstDF->nMaxDistCode,
                      5) == FALSE)
        return(FALSE);

    // Output HCLEN

    if (DFOutputValue(lpstDF,
                      lpstDF->nMaxCodeLenCodeIndex - 3,
                      4) == FALSE)
        return(FALSE);

    // Output code length code lengths

    for (i=0;i<=lpstDF->nMaxCodeLenCodeIndex;i++)
    {
        nCode = gabyDFCodeLenCodeOrder[i];

        assert(lpstDF->abyCodeLenLen[nCode] <= 7);

        if (DFOutputValue(lpstDF,
                          lpstDF->abyCodeLenLen[nCode],
                          3) == FALSE)
            return(FALSE);
    }

    // Output literal/length code code lengths using code length codes

    if (DFOutputCodeLengths(lpstDF,
                            lpstDF->nMaxLLCode + 1,
                            lpstDF->abyLLLen) == FALSE)
        return(FALSE);

    // Output distance code code lengths using code length codes

    if (DFOutputCodeLengths(lpstDF,
                            lpstDF->nMaxDistCode + 1,
                            lpstDF->abyDistLen) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL DFOutputLitLenDist()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  wLitLen         Literal if wDist is 0, otherwise length
//  wDist           Distance of repeated sequence or 0 if literal
//
// Description:
//  Outputs the required bits according to the distance
//  literal/length values.
//
// Returns:
//  TRUE            On success
//  FALSE           On error writing
//
//********************************************************************

BOOL DFOutputLitLenDist
(
    LPDF        lpstDF,
    WORD        wLitLen,
    WORD        wDist
)
{
    if (wDist == 0)
    {
        // wLitLen is a literal <=  256

        assert(wLitLen <= 256);

        // Output the Huffman code for the literal

        if (DFOutputValue(lpstDF,
                          lpstDF->awLL[wLitLen],
                          lpstDF->abyLLLen[wLitLen]) == FALSE)
            return(FALSE);
    }
    else
    {
        int nLow, nHigh, nMid;

        /////////////////////////////////////////////////////////
        // wLitLen is a length between 3 and 258

        assert(wLitLen >= 3);
        assert(wLitLen <= 258);

        // Find length

        nLow = 0;
        nHigh = DF_NUM_LEN_CODES - 1;
        while (nLow <= nHigh)
        {
            nMid = (nLow + nHigh) >> 1;

            if (wLitLen < gawDFLenRangeMin[nMid])
            {
                // In lower half

                nHigh = nMid - 1;
            }
            else
            if (wLitLen > gawDFLenRangeMax[nMid])
            {
                // In upper half

                nLow = nMid + 1;
            }
            else
            {
                // Found the range

                break;
            }
        }

        // We must have found the right range

        assert(nLow <= nHigh);

        // Output the Huffman code for the length

        if (DFOutputValue(lpstDF,
                          lpstDF->awLL[DF_NUM_LITERAL_CODES + nMid],
                          lpstDF->
                              abyLLLen[DF_NUM_LITERAL_CODES + nMid]) == FALSE)
            return(FALSE);

        // Now we need to output the extra bits if any

        if (gabyDFLenExtraBits[nMid] != 0)
        {
            if (DFOutputValue(lpstDF,
                              wLitLen - gawDFLenRangeMin[nMid],
                              gabyDFLenExtraBits[nMid]) == FALSE)
                return(FALSE);
        }

        /////////////////////////////////////////////////////////
        // wDist is between 1 and 32768

        assert(wDist >= 1);
        assert(wDist <= 32768);

        // Find distance

        nLow = 0;
        nHigh = DF_NUM_DIST_CODES - 1;
        while (nLow <= nHigh)
        {
            nMid = (nLow + nHigh) >> 1;

            if (wDist < gawDFDistRangeMin[nMid])
            {
                // In lower half

                nHigh = nMid - 1;
            }
            else
            if (wDist > gawDFDistRangeMax[nMid])
            {
                // In upper half

                nLow = nMid + 1;
            }
            else
            {
                // Found the range

                break;
            }
        }

        // We must have found the right range

        assert(nLow <= nHigh);

        // Output the Huffman code for the distance

        if (DFOutputValue(lpstDF,
                          lpstDF->awDist[nMid],
                          lpstDF->abyDistLen[nMid]) == FALSE)
            return(FALSE);

        // Now we need to output the extra bits if any

        if (gabyDFDistExtraBits[nMid] != 0)
        {
            if (DFOutputValue(lpstDF,
                              wDist - gawDFDistRangeMin[nMid],
                              gabyDFDistExtraBits[nMid]) == FALSE)
                return(FALSE);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  void DFUpdateLenDistFreq()
//
// Parameters:
//  lpstDF          Ptr to DF structure
//  wLen            Length to update
//  wDist           Distance of repeated sequence or 0 if literal
//
// Description:
//  Increments by one the code for the given length and distance.
//
// Returns:
//  Nothing.
//
//********************************************************************

void DFUpdateLenDistFreq
(
    LPDF        lpstDF,
    WORD        wLen,
    WORD        wDist
)
{
    int nLow, nHigh, nMid;

    /////////////////////////////////////////////////////////
    // wLen is a length between 3 and 258

    assert(wLen >= 3);
    assert(wLen <= 258);

    // Find length

    nLow = 0;
    nHigh = DF_NUM_LEN_CODES - 1;
    while (nLow <= nHigh)
    {
        nMid = (nLow + nHigh) >> 1;

        if (wLen < gawDFLenRangeMin[nMid])
        {
            // In lower half

            nHigh = nMid - 1;
        }
        else
        if (wLen > gawDFLenRangeMax[nMid])
        {
            // In upper half

            nLow = nMid + 1;
        }
        else
        {
            // Found the range

            break;
        }
    }

    // We must have found the right range

    assert(nLow <= nHigh);

    // Update the length frequency

    lpstDF->awLL[DF_NUM_LITERAL_CODES + nMid]++;

    /////////////////////////////////////////////////////////
    // wDist is between 1 and 32768

    assert(wDist >= 1);

    // Find distance

    nLow = 0;
    nHigh = DF_NUM_DIST_CODES - 1;
    while (nLow <= nHigh)
    {
        nMid = (nLow + nHigh) >> 1;

        if (wDist < gawDFDistRangeMin[nMid])
        {
            // In lower half

            nHigh = nMid - 1;
        }
        else
        if (wDist > gawDFDistRangeMax[nMid])
        {
            // In upper half

            nLow = nMid + 1;
        }
        else
        {
            // Found the range

            break;
        }
    }

    // We must have found the right range

    assert(nLow <= nHigh);

    // Update the length frequency

    lpstDF->awDist[nMid]++;
}

