// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/acsapi.cpv   1.11   06 Jan 1999 11:53:52   DCHI  $
//
// Description:
//  ACSAPI source file
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/acsapi.cpv  $
// 
//    Rev 1.11   06 Jan 1999 11:53:52   DCHI
// Fixed "if (bFromLast = FALSE" in A97ObjAccessRead().
// 
//    Rev 1.10   08 Dec 1998 12:33:14   DCHI
// Modified A97ObjAccessInit() and A97ObjAccessRead() to optimize access
// to last page accessed.
// 
//    Rev 1.9   12 Oct 1998 13:10:02   DCHI
// Added NLM relinquish control calls to A97EnumItems() and A97ObjAccessRead().
// 
//    Rev 1.8   13 Aug 1998 14:59:18   DCHI
// Correct index validation in A97GetVBAMasterPage().
// 
//    Rev 1.7   11 May 1998 11:15:30   DCHI
// Modified FindIndexedKeyValue() to search one more type 0x04 page.
// 
//    Rev 1.6   11 May 1998 11:04:52   DCHI
// Modified A97EnumItems() to search one additional page if nothing
// is found on the first page.
// 
//    Rev 1.5   05 May 1998 18:25:46   DCHI
// Modified A97GetVBAMasterPage() to properly get relocated master page.
// 
//    Rev 1.4   04 May 1998 16:18:52   DCHI
// Mods to ObjectRead() to handle case where item fits in one entry.
// 
//    Rev 1.3   30 Apr 1998 13:24:10   DCHI
// Corrected incorrect get of macro index prefix byte.
// 
//    Rev 1.2   28 Apr 1998 15:52:36   DCHI
// Modified for general get of macro prefix during macro enumeration.
// 
//    Rev 1.1   20 Apr 1998 13:49:22   DCHI
// Modifications for handling of compacted/repaired databases.
// 
//    Rev 1.0   17 Apr 1998 13:40:52   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"

#include "acsapi.h"

// MSysModules key value

BYTE gabyA97MSysModulesObjectsKey[] =
{
    0x7F, 0x8F, 0x00, 0x00, 0x01,
    0x7F, 0x6F, 0x76, 0x7D, 0x76,
    0x6F, 0x72, 0x64, 0x78, 0x6D, 0x66, 0x76, 0x00
};

BYTE gabyA97ScriptsObjectsKey[] =
{
    0x7F, 0x8F, 0x00, 0x00, 0x00,
    0x7F, 0x76, 0x62, 0x75, 0x6A, 0x73, 0x77, 0x76, 0x00
};

// MSysModules2 key value

BYTE gabyA97MSysModules2ObjectsKey[] =
{
    0x7F, 0x8F, 0x00, 0x00, 0x01,
    0x7F, 0x6F, 0x76, 0x7D, 0x76,
    0x6F, 0x72, 0x64, 0x78, 0x6D, 0x66, 0x76, 0x58, 0x00
};

// Prefix for macro entries in objects table

#define A97_MACRO_PREFIX        6

// Master index fields

BYTE gabypsA97Name[5] = { 4, 'N', 'a', 'm', 'e' };
BYTE gabypsA97ParentId[9] = { 8, 'P', 'a', 'r', 'e', 'n', 't', 'I', 'd' };

//********************************************************************
//
// Function:
//  BOOL A97Create()
//
// Parameters:
//  lplpstA97           Ptr to ptr to store A97_T ptr
//  lpvRootCookie       Root cookie for OS routines
//  lpvFile             Pointer to file object for file ops
//
// Description:
//  The function allocates memory for an A97_T structure.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97Create
(
    LPLPA97             lplpstA97,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile
)
{
    LPA97               lpstA97;

    // Allocate root structure

    if (ACSMemoryAlloc(lpvRootCookie,
                       sizeof(A97_T),
                       (LPVOID FAR *)&lpstA97) == FALSE)
        return(FALSE);

    lpstA97->lpvRootCookie = lpvRootCookie;
    lpstA97->lpvFile = lpvFile;

    *lplpstA97 = lpstA97;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97Destroy()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure to free
//
// Description:
//  The function deallocates the memory of an A97_T structure.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97Destroy
(
    LPA97               lpstA97
)
{
    if (ACSMemoryFree(lpstA97->lpvRootCookie,
                      lpstA97) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  A97P01DeleteItemAtIndex()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  dwPage              Type 0x01 page containing item
//  nItemIndex          Zero-based index of item to delete
//
// Description:
//  The function deletes the item at the given index from the given
//  type 0x01 page.
//
//  The function uses the temporary page buffer as working space.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97P01DeleteItemAtIndex
(
    LPA97               lpstA97,
    DWORD               dwPage,
    int                 nItemIndex
)
{
    LPA97_P01           lpstPage;
    DWORD               dwByteCount;
    int                 i;
    int                 nDist;
    WORD                w;
    WORD                wItemCount;
    int                 nStartOffset;
    int                 nEndOffset;
    int                 nLastItemOffset;
    LPBYTE              lpabyDst;

    lpstPage = (LPA97_P01)(lpstA97->abyTempPage);

    ////////////////////////////////////////////////////////
    // Read the page

    if (ACSFileSeek(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    dwPage * A97_PAGE_SIZE,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileRead(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    lpstPage,
                    A97_PAGE_SIZE,
                    &dwByteCount) == FALSE ||
        dwByteCount != A97_PAGE_SIZE)
        return(FALSE);

    // Validate page type

    if (lpstPage->abyType[0] != 0x01 || lpstPage->abyType[1] != 0x01)
        return(FALSE);

    wItemCount = WENDIAN(lpstPage->wItemCount);
    if (nItemIndex >= A97_P01_MAX_ITEMS ||
        nItemIndex >= wItemCount)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Get the start offset and end offset of the item

    // If the item has already been deleted, just return

    w = WENDIAN(lpstPage->awItemOffsets[nItemIndex]);
    if ((w & 0xC000) == 0xC000)
    {
        // Item already deleted

        return(TRUE);
    }

    nStartOffset = (int)w;

    // Delete the item

    if (nItemIndex == 0)
        nEndOffset = 0x800;
    else
    {
        w = WENDIAN(lpstPage->awItemOffsets[nItemIndex-1]);
        nEndOffset = w & 0xFFF;
    }

    w = 0xC000 | (WORD)nEndOffset;
    lpstPage->awItemOffsets[nItemIndex] = WENDIAN(w);

    // Validate start and end offsets

    if (nStartOffset > nEndOffset || nEndOffset > A97_PAGE_SIZE)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Delete the item

    // Calculate the move distance

    lpabyDst = (LPBYTE)lpstPage;
    if (nItemIndex + 1 < wItemCount)
    {
        nDist = nEndOffset - nStartOffset;

        // First adjust all the offsets

        nLastItemOffset = (int)
            (WENDIAN(lpstPage->awItemOffsets[wItemCount-1]) & 0x0FFF);

        for (i=nItemIndex+1;i<wItemCount;i++)
        {
            w = WENDIAN(lpstPage->awItemOffsets[i]);
            w += nDist;
            lpstPage->awItemOffsets[i] = WENDIAN(w);
        }

        // Now move the items by the adjustment amount

        for (i=nStartOffset-1;i>=nLastItemOffset;i--)
            lpabyDst[i + nDist] = lpabyDst[i];
    }

    // Zero out the remaining bytes

    nLastItemOffset = (int)
        (WENDIAN(lpstPage->awItemOffsets[wItemCount-1]) & 0x0FFF);

    if (nLastItemOffset > A97_PAGE_SIZE)
        nLastItemOffset = A97_PAGE_SIZE;

    for (i=10+wItemCount*sizeof(WORD);i<nLastItemOffset;i++)
        lpabyDst[i] = 0;

    ////////////////////////////////////////////////////////
    // Write back the modified page

    if (ACSFileSeek(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    dwPage * A97_PAGE_SIZE,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileWrite(lpstA97->lpvRootCookie,
                     lpstA97->lpvFile,
                     lpstPage,
                     A97_PAGE_SIZE,
                     &dwByteCount) == FALSE ||
        dwByteCount != A97_PAGE_SIZE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97P02GetFirstIndexPage()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  dwMasterPage        Type 0x02 master page containing index
//  lpabypsFieldName    Name of first field
//  lpdwPage            Ptr to DWORD for first index page number
//
// Description:
//  The function returns the page number at offsets 0x22-0x24
//  of the item with the given index from the
//  abyArray2[dwCount4][0x27] array of the given type 0x02 page.
//
//  The function uses the temporary page buffer as working space.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97P02GetFirstIndexPage
(
    LPA97               lpstA97,
    DWORD               dwMasterPage,
    LPBYTE              lpabypsFieldName,
    LPDWORD             lpdwPage
)
{
    LPA97_P02           lpstPage;
    DWORD               dwByteCount;
    WORD                w;
    int                 i;
    int                 j;
    int                 nDataLeft;
    int                 nFieldIndex;
    DWORD               dwSize;

    lpstPage = (LPA97_P02)(lpstA97->abyTempPage);

    ////////////////////////////////////////////////////////
    // Read the page

    if (ACSFileSeek(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    dwMasterPage * A97_PAGE_SIZE,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileRead(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    lpstPage,
                    A97_PAGE_SIZE,
                    &dwByteCount) == FALSE ||
        dwByteCount != A97_PAGE_SIZE)
        return(FALSE);

    // Validate page type

    if (lpstPage->abyType[0] != 0x02 || lpstPage->abyType[1] != 0x01)
        return(FALSE);

    // Endianize header values to use

    lpstPage->wCount0 = WENDIAN(lpstPage->wCount0);
    lpstPage->dwCount4 = DWENDIAN(lpstPage->dwCount4);

    // Initialize parsing

    nDataLeft = A97_P02_DATA_SIZE;
    i = 0;

    // adwArray0[dwCount4][2];

    dwSize = lpstPage->dwCount4 * (sizeof(DWORD) + sizeof(DWORD));
    if (dwSize > nDataLeft)
        return(FALSE);

    nDataLeft -= dwSize;
    i += dwSize;

    // abyArray1[wCount0][0x12]

    dwSize = lpstPage->wCount0 * (DWORD)0x12;
    if (dwSize > nDataLeft)
        return(FALSE);

    nDataLeft -= dwSize;
    i += dwSize;

    // abypsStr0[wCount0]

    nFieldIndex = -1;
    for (w=0;w<lpstPage->wCount0;w++)
    {
        if (lpstPage->abyData[i] > --nDataLeft)
            return(FALSE);

        if (nFieldIndex == -1 && lpstPage->abyData[i] == lpabypsFieldName[0])
        {
            // Compare the field names

            for (j=1;j<=lpstPage->abyData[i];j++)
            {
                if (lpstPage->abyData[i+j] != lpabypsFieldName[j])
                    break;
            }

            if (j > lpstPage->abyData[i])
                nFieldIndex = (int)w;
        }

        nDataLeft -= lpstPage->abyData[i];
        i += 1 + lpstPage->abyData[i];
    }

    // abyArray2[dwCount4][0x27]

    while (lpstPage->dwCount4-- != 0 && nDataLeft >= 0x27)
    {
        if (nFieldIndex == lpstPage->abyData[i])
        {
            *lpdwPage = ((DWORD)lpstPage->abyData[i+0x22]) |
                        ((DWORD)lpstPage->abyData[i+0x23] << 8) |
                        ((DWORD)lpstPage->abyData[i+0x24] << 16);

            return(TRUE);
        }

        nDataLeft -= 0x27;
        i += 0x27;
    }

    // Not found

    return(FALSE);
}


//********************************************************************
//
// Function:
//  void A97XlatASCIItoA97()
//
// Parameters:
//  lpabyBuf            Buffer to translate
//  nBufLen             Length of buffer
//
// Description:
//  Translate the given buffer from the ASCII character set
//  to the Access 97 index character set.
//
// Returns:
//  Nothing
//
//********************************************************************

BYTE gabyA97XlatASCIItoA97[0x80] =
{
    0x00,   // 0x00
    0x01,   // 0x01
    0x02,   // 0x02
    0x03,   // 0x03
    0x04,   // 0x04
    0x05,   // 0x05
    0x06,   // 0x06
    0x07,   // 0x07
    0x08,   // 0x08
    0x09,   // 0x09
    0x0A,   // 0x0A
    0x0B,   // 0x0B
    0x0C,   // 0x0C
    0x0D,   // 0x0D
    0x0E,   // 0x0E
    0x0F,   // 0x0F
    0x10,   // 0x10
    0x11,   // 0x11
    0x12,   // 0x12
    0x13,   // 0x13
    0x14,   // 0x14
    0x15,   // 0x15
    0x16,   // 0x16
    0x17,   // 0x17
    0x18,   // 0x18
    0x19,   // 0x19
    0x1A,   // 0x1A
    0x1B,   // 0x1B
    0x1C,   // 0x1C
    0x1D,   // 0x1D
    0x1E,   // 0x1E
    0x1F,   // 0x1F
    0x11,   // 0x20 space
    0x21,   // 0x21
    0x13,   // 0x22 "
    0x14,   // 0x23 #
    0x15,   // 0x24 $
    0x16,   // 0x25 %
    0x17,   // 0x26 &
    0x18,   // 0x27 '
    0x19,   // 0x28 (
    0x1A,   // 0x29 )
    0x1B,   // 0x2A *
    0x1C,   // 0x2B +
    0x1D,   // 0x2C ,
    0x1E,   // 0x2D -
    0x2E,   // 0x2E
    0x20,   // 0x2F /
    0x56,   // 0x30 0
    0x57,   // 0x31 1
    0x58,   // 0x32 2
    0x59,   // 0x33 3
    0x5A,   // 0x34 4
    0x5B,   // 0x35 5
    0x5C,   // 0x36 6
    0x5D,   // 0x37 7
    0x5E,   // 0x38 8
    0x5F,   // 0x39 9
    0x21,   // 0x3A :
    0x22,   // 0x3B ;
    0x23,   // 0x3C <
    0x24,   // 0x3D =
    0x25,   // 0x3E >
    0x26,   // 0x3F ?
    0x27,   // 0x40 @
    0x60,   // 0x41 A
    0x61,   // 0x42 B
    0x62,   // 0x43 C
    0x64,   // 0x44 D
    0x66,   // 0x45 E
    0x67,   // 0x46 F
    0x68,   // 0x47 G
    0x69,   // 0x48 H
    0x6A,   // 0x49 I
    0x6B,   // 0x4A J
    0x6C,   // 0x4B K
    0x6D,   // 0x4C L
    0x6F,   // 0x4D M
    0x70,   // 0x4E N
    0x72,   // 0x4F O
    0x73,   // 0x50 P
    0x74,   // 0x51 Q
    0x75,   // 0x52 R
    0x76,   // 0x53 S
    0x77,   // 0x54 T
    0x78,   // 0x55 U
    0x7A,   // 0x56 V
    0x7B,   // 0x57 W
    0x7C,   // 0x58 X
    0x7D,   // 0x59 Y
    0x7E,   // 0x5A Z
    0x5B,   // 0x5B
    0x29,   // 0x5C backslash
    0x5D,   // 0x5D
    0x2B,   // 0x5E ^
    0x2C,   // 0x5F _
    0x60,   // 0x60
    0x60,   // 0x61 a
    0x61,   // 0x62 b
    0x62,   // 0x63 c
    0x64,   // 0x64 d
    0x66,   // 0x65 e
    0x67,   // 0x66 f
    0x68,   // 0x67 g
    0x69,   // 0x68 h
    0x6A,   // 0x69 i
    0x6B,   // 0x6A j
    0x6C,   // 0x6B k
    0x6D,   // 0x6C l
    0x6F,   // 0x6D m
    0x70,   // 0x6E n
    0x72,   // 0x6F o
    0x73,   // 0x70 p
    0x74,   // 0x71 q
    0x75,   // 0x72 r
    0x76,   // 0x73 s
    0x77,   // 0x74 t
    0x78,   // 0x75 u
    0x7A,   // 0x76 v
    0x7B,   // 0x77 w
    0x7C,   // 0x78 x
    0x7D,   // 0x79 y
    0x7E,   // 0x7A z
    0x2E,   // 0x7B {
    0x2F,   // 0x7C |
    0x30,   // 0x7D }
    0x31,   // 0x7E ~
    0x7F    // 0x7F
};

void A97XlatASCIItoA97
(
    LPBYTE              lpabyBuf,
    int                 nBufLen
)
{
    int             i;

    for (i=0;i<nBufLen;i++)
    {
        if (lpabyBuf[i] < sizeof(gabyA97XlatASCIItoA97))
            lpabyBuf[i] = gabyA97XlatASCIItoA97[lpabyBuf[i]];
    }
}


//********************************************************************
//
// Function:
//  void A97XlatA97toASCII()
//
// Parameters:
//  lpabyBuf            Buffer to translate
//  nBufLen             Length of buffer
//
// Description:
//  Translate the given buffer from the Access 97 index character
//  set to the ASCII character set.
//
// Returns:
//  Nothing
//
//********************************************************************

BYTE gabyA97XlatA97toASCII[0x80] =
{
    0x00,       // 0x00
    0x01,       // 0x01
    0x02,       // 0x02
    0x03,       // 0x03
    0x04,       // 0x04
    0x05,       // 0x05
    0x06,       // 0x06
    0x07,       // 0x07
    0x08,       // 0x08
    0x09,       // 0x09
    0x0A,       // 0x0A
    0x0B,       // 0x0B
    0x0C,       // 0x0C
    0x0D,       // 0x0D
    0x0E,       // 0x0E
    0x0F,       // 0x0F
    0x10,       // 0x10
    ' ',        // 0x11
    0x12,       // 0x12
    '\"',       // 0x13
    '#',        // 0x14
    '$',        // 0x15
    '%',        // 0x16
    '&',        // 0x17
    '\'',       // 0x18
    '(',        // 0x19
    ')',        // 0x1A
    '*',        // 0x1B
    '+',        // 0x1C
    ',',        // 0x1D
    '-',        // 0x1E
    0x1F,       // 0x1F
    '/',        // 0x20
    ':',        // 0x21
    ';',        // 0x22
    '<',        // 0x23
    '=',        // 0x24
    '>',        // 0x25
    '?',        // 0x26
    '@',        // 0x27
    0x28,       // 0x28
    '\\',       // 0x29
    0x2A,       // 0x2A
    '^',        // 0x2B
    '_',        // 0x2C
    0x2D,       // 0x2D
    '{',        // 0x2E
    '|',        // 0x2F
    '}',        // 0x30
    '~',        // 0x31
    0x32,       // 0x32
    0x33,       // 0x33
    0x34,       // 0x34
    0x35,       // 0x35
    0x36,       // 0x36
    0x37,       // 0x37
    0x38,       // 0x38
    0x39,       // 0x39
    0x3A,       // 0x3A
    0x3B,       // 0x3B
    0x3C,       // 0x3C
    0x3D,       // 0x3D
    0x3E,       // 0x3E
    0x3F,       // 0x3F
    0x40,       // 0x40
    0x41,       // 0x41
    0x42,       // 0x42
    0x43,       // 0x43
    0x44,       // 0x44
    0x45,       // 0x45
    0x46,       // 0x46
    0x47,       // 0x47
    0x48,       // 0x48
    0x49,       // 0x49
    0x4A,       // 0x4A
    0x4B,       // 0x4B
    0x4C,       // 0x4C
    0x4D,       // 0x4D
    0x4E,       // 0x4E
    0x4F,       // 0x4F
    0x50,       // 0x50
    0x51,       // 0x51
    0x52,       // 0x52
    0x53,       // 0x53
    0x54,       // 0x54
    0x55,       // 0x55
    '0',        // 0x56
    '1',        // 0x57
    '2',        // 0x58
    '3',        // 0x59
    '4',        // 0x5A
    '5',        // 0x5B
    '6',        // 0x5C
    '7',        // 0x5D
    '8',        // 0x5E
    '9',        // 0x5F
    'A',        // 0x60
    'B',        // 0x61
    'C',        // 0x62
    0x63,       // 0x63
    'D',        // 0x64
    0x65,       // 0x65
    'E',        // 0x66
    'F',        // 0x67
    'G',        // 0x68
    'H',        // 0x69
    'I',        // 0x6A
    'J',        // 0x6B
    'K',        // 0x6C
    'L',        // 0x6D
    0x6E,       // 0x6E
    'M',        // 0x6F
    'N',        // 0x70
    0x71,       // 0x71
    'O',        // 0x72
    'P',        // 0x73
    'Q',        // 0x74
    'R',        // 0x75
    'S',        // 0x76
    'T',        // 0x77
    'U',        // 0x78
    0x79,       // 0x79
    'V',        // 0x7A
    'W',        // 0x7B
    'X',        // 0x7C
    'Y',        // 0x7D
    'Z',        // 0x7E
    0x7F,       // 0x7F
};

void A97XlatA97toASCII
(
    LPBYTE              lpabyBuf,
    int                 nBufLen
)
{
    int             i;

    for (i=0;i<nBufLen;i++)
    {
        if (lpabyBuf[i] < sizeof(gabyA97XlatA97toASCII))
            lpabyBuf[i] = gabyA97XlatA97toASCII[lpabyBuf[i]];
    }
}


//********************************************************************
//
// Function:
//  BOOL A97P04FindIndexEntry()
//
// Parameters:
//  lpstPage            Ptr to type 0x04/0x03 page
//  lpfnPrefixFlags     Ptr to prefix flags function
//  lpfnKeyValueLen     Ptr to key value length function
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
//  index page number of the first item whose key value is
//  greater than or equal to the key.  If the given key value
//  is greater than all the key values on the page, the function
//  returns the page number as the dwBranchPage field of the page.
//  The function returns TRUE and a result of 1 in this case.
//
// Returns:
//  TRUE        0       Equal (returns *lpdwPage and *lpnIndex)
//           == 1       Try page (*lpdwPage)
//             -1       Not present
//  FALSE               Error
//
//********************************************************************

BOOL A97P04FindIndexEntry
(
    LPA97_P04           lpstPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex,
    LPINT               lpnResult
)
{
    int                 i;
    int                 j;
    int                 nPrefixFlags;
    int                 nLen;
    int                 nMaxOffset;
    int                 nMaxCandOffset;
    int                 nResult;
    int                 nExtraBytes;
    LPBYTE              lpabyData;

    ////////////////////////////////////////////////////////
    // Determine page type

    // If the page is of type 0x03, then it is an index page

    if (lpstPage->abyType[0] == 0x03)
        nExtraBytes = 8;
    else
    if (lpstPage->abyType[0] == 0x04)
        nExtraBytes = 4;
    else
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Validate data

    if (WENDIAN(lpstPage->wEmptySpace) > A97_P04_DATA_SIZE)
        return(FALSE);

    nMaxOffset = A97_P04_DATA_SIZE - WENDIAN(lpstPage->wEmptySpace);

    ////////////////////////////////////////////////////////
    // Check against the prefix of the first object

    // Set the pointer to the data

    lpabyData = lpstPage->abyData;

    // Compare against the prefix

    i = 0;
    nResult = 0;
    while (i < nKeyValueLen &&
           i < nMaxOffset &&
           i < lpstPage->byCommonPrefixLen)
    {
        nResult = lpabyKeyValue[i] - lpabyData[i];
        if (nResult != 0)
        {
            if (lpstPage->abyType[0] == 0x04)
            {
                // Not found

                *lpnResult = -1;
                return(TRUE);
            }

            if (nResult > 0)
            {
                // Try the next index page

                *lpdwPage = DWENDIAN(lpstPage->dwBranchPage);
                *lpnResult = 1;
                return(TRUE);
            }

            break;
        }

        ++i;
    }

    // Invalid format?

    if (i >= nMaxOffset)
        return(FALSE);

    // Did at least the prefix match

    if (i < lpstPage->byCommonPrefixLen || nResult < 0)
    {
        // The search key value is less than the first key value
        //  on this index page

        if (lpfnKeyValueLen(lpabyData,
                            0,
                            nMaxOffset,
                            &nLen) == FALSE)
            return(FALSE);

        // The object is less than the first entry

        // Skip over the key value and the index page and entry

        lpabyData += nLen + 4;

        // Return the next page to search

        *lpdwPage = ((DWORD)lpabyData[1] << 16) |
                    ((DWORD)lpabyData[2] << 8) |
                    ((DWORD)lpabyData[3]);

        *lpnResult = 1;
        return(TRUE);
    }

    // Get the prefix flags

    if (lpfnPrefixFlags(lpabyData,
                        lpstPage->byCommonPrefixLen,
                        &nPrefixFlags) == FALSE)
        return(FALSE);

    // The prefix matched otherwise

    i = lpstPage->byCommonPrefixLen;
    lpabyData += i;

    // No need to match against the prefix anymore

    nKeyValueLen -= lpstPage->byCommonPrefixLen;
    lpabyKeyValue += lpstPage->byCommonPrefixLen;

    ////////////////////////////////////////////////////////
    // Now check all the items

    nMaxCandOffset = nMaxOffset - nExtraBytes;
    while (i + nKeyValueLen <= nMaxCandOffset)
    {
        // Get the length of the current object

        if (lpfnKeyValueLen(lpabyData,
                            nPrefixFlags,
                            nMaxCandOffset - i,
                            &nLen) == FALSE)
            return(FALSE);

        j = 0;
        nResult = 0;
        while (j < nKeyValueLen && j < nLen)
        {
            nResult = lpabyKeyValue[j] - lpabyData[j];
            if (nResult != 0)
                break;

            ++j;
        }

        // Skip over the candidate key value

        lpabyData += nLen;
        i += nLen;

        if (nResult == 0)
        {
            if (j < nLen)
                nResult = -1;
            else
            if (j < nKeyValueLen)
                nResult = 1;
            else
            if (i + 4 <= nMaxOffset)
            {
                if (lpstPage->abyType[0] == 0x04)
                {
                    // Return the page and index of this object

                    *lpdwPage = ((DWORD)lpabyData[0] << 16) |
                                ((DWORD)lpabyData[1] << 8) |
                                ((DWORD)lpabyData[2]);

                    *lpnIndex = lpabyData[3];

                    *lpnResult = 0;
                    return(TRUE);
                }

                // It is a type 0x03 page

                nResult = -1;
            }
            else
                return(FALSE);
        }

        if (nResult < 0)
        {
            if (lpstPage->abyType[0] == 0x04)
            {
                *lpnResult = -1;
                return(TRUE);
            }

            // Get the next page to search

            if (i + 8 <= nMaxOffset)
            {
                // Return the next page

                *lpdwPage = ((DWORD)lpabyData[5] << 16) |
                            ((DWORD)lpabyData[6] << 8) |
                            ((DWORD)lpabyData[7]);

                *lpnResult = 1;
                return(TRUE);
            }

            // Not enough data

            return(FALSE);
        }

        lpabyData += nExtraBytes;
        i += nExtraBytes;
    }

    // Assume the item must be greater

    if (lpstPage->abyType[0] == 0x04)
    {
        *lpnResult = -1;
        return(TRUE);
    }

    *lpdwPage = DWENDIAN(lpstPage->dwBranchPage);
    *lpnResult = 1;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97P04FindIndexedKeyValue()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  dwPage              Starting index page to search
//  lpfnPrefixFlags     Ptr to prefix flags function
//  lpfnKeyValueLen     Ptr to key value length function
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

BOOL A97P04FindIndexedKeyValue
(
    LPA97               lpstA97,
    DWORD               dwPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex,
    LPBOOL              lpbFound
)
{
    LPA97_P04           lpstPage;
    DWORD               dwByteCount;
    int                 i;
    int                 nResult;
    BOOL                bHadFirstTry;

    lpstPage = (LPA97_P04)(lpstA97->abyTempPage);

    // Search up to 256 pages

    bHadFirstTry = FALSE;
    for (i=0;i<256;i++)
    {
        if (ACSFileSeek(lpstA97->lpvRootCookie,
                        lpstA97->lpvFile,
                        dwPage * A97_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstA97->lpvRootCookie,
                        lpstA97->lpvFile,
                        lpstPage,
                        A97_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A97_PAGE_SIZE)
            return(FALSE);

        // Find object

        if (A97P04FindIndexEntry(lpstPage,
                                 lpfnPrefixFlags,
                                 lpfnKeyValueLen,
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
        else
        if (nResult == -1)
        {
            // Search the following page

            dwPage = DWENDIAN(lpstPage->dwNextPage);

            if (bHadFirstTry != FALSE || dwPage == 0)
            {
                *lpbFound = FALSE;
                return(TRUE);
            }

            bHadFirstTry = TRUE;
        }
        else
        if (nResult != 1)
            return(FALSE);
        else
        {
            // Search the next page

            dwPage = *lpdwPage;
        }
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A97ObjectIDPrefixFlags()
//
// Parameters:
//  lpabyKeyValue       Ptr to prefix
//  nPrefixLen          Length of prefix
//  lpnPrefixFlags      Ptr to int to receive prefix flags
//
// Description:
//  Returns the prefix flags for an Object ID index page.
//
// Returns:
//  TRUE                Always
//
//********************************************************************

BOOL A97ObjectIDPrefixFlags
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixLen,
    LPINT               lpnPrefixFlags
)
{
    // Is the prefix a complete key value?

    if (nPrefixLen > 5 &&
        lpabyKeyValue[nPrefixLen] == 0)
    {
        *lpnPrefixFlags = -1;
        return(TRUE);
    }

    *lpnPrefixFlags = nPrefixLen;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97ObjectIDKeyValueLen()
//
// Parameters:
//  lpabyKeyValue       Ptr to key value
//  nPrefixFlags        Prefix flags
//  nMaxOffset          Maximum offset of key value
//  lpnLen              Ptr to int for key value length
//
// Description:
//  The ID key value length includes the first five bytes,
//  and the string including the zero-terminator.
//
//  Returns the key value length for an Object ID index page.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97ObjectIDKeyValueLen
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixFlags,
    int                 nMaxOffset,
    LPINT               lpnLen
)
{
    int         i;

    if (nPrefixFlags == -1)
        return(0);

    if (nPrefixFlags <= 5)
    {
        // Don't search for the zero-terminator within the first
        //  five characters

        i = 5 - nPrefixFlags;
    }
    else
        i = 0;

    // Find the zero-terminator

    while (i < nMaxOffset)
    {
        if (lpabyKeyValue[i++] == 0)
        {
            *lpnLen = i;
            return(TRUE);
        }
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A97P04DeleteEntry()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  dwPage              Type 0x04/0x03 page number with entry
//  lpfnPrefixFlags     Ptr to prefix flags function
//  lpfnKeyValueLen     Ptr to key value length function
//  nKeyValueLen        Length of key value to search
//  lpabyKeyValue       Ptr to key value to search
//
// Description:
//  The function deletes the given key value from the given
//  index page.
//
//  If the function does not find the given key value, it returns
//  error.
//
// Returns:
//  TRUE                On success
//  FALSE               Error
//
//********************************************************************

BOOL A97P04DeleteEntry
(
    LPA97               lpstA97,
    DWORD               dwPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue
)
{
    LPA97_P04           lpstPage;
    DWORD               dwByteCount;
    int                 i;
    int                 j;
    int                 nPrefixFlags;
    int                 nLen;
    int                 nMaxOffset;
    int                 nMaxCandOffset;
    int                 nResult;
    int                 nExtraBytes;
    LPBYTE              lpabyData;
    int                 nBitOffset;

    lpstPage = (LPA97_P04)(lpstA97->abyTempPage);

    ////////////////////////////////////////////////////////
    // Read the page

    if (ACSFileSeek(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    dwPage * A97_PAGE_SIZE,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileRead(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    lpstPage,
                    A97_PAGE_SIZE,
                    &dwByteCount) == FALSE ||
        dwByteCount != A97_PAGE_SIZE)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Determine page type

    // If the page is of type 0x03, then it is an index page

    if (lpstPage->abyType[0] == 0x03)
        nExtraBytes = 8;
    else
    if (lpstPage->abyType[0] == 0x04)
        nExtraBytes = 4;
    else
        return(FALSE);


    ////////////////////////////////////////////////////////
    // Validate data

    lpstPage->wEmptySpace = WENDIAN(lpstPage->wEmptySpace);
    if (lpstPage->wEmptySpace > A97_P04_DATA_SIZE)
        return(FALSE);

    nMaxOffset = A97_P04_DATA_SIZE - lpstPage->wEmptySpace;

    ////////////////////////////////////////////////////////
    // Check against the prefix of the first object

    // Get the prefix flags

    lpabyData = lpstPage->abyData;
    if (lpfnPrefixFlags(lpabyData,
                        lpstPage->byCommonPrefixLen,
                        &nPrefixFlags) == FALSE)
        return(FALSE);

    // Skip the prefix

    i = lpstPage->byCommonPrefixLen;
    lpabyData += i;

    // No need to match against the prefix anymore

    nKeyValueLen -= i;
    lpabyKeyValue += i;

    ////////////////////////////////////////////////////////
    // Now check all the items

    nMaxCandOffset = nMaxOffset - nExtraBytes;
    while (i + nKeyValueLen <= nMaxCandOffset)
    {
        // Get the length of the current object

        if (lpfnKeyValueLen(lpabyData,
                            nPrefixFlags,
                            nMaxCandOffset - i,
                            &nLen) == FALSE)
            return(FALSE);

        if (nKeyValueLen == nLen)
        {
            j = 0;
            nResult = 0;
            while (j < nKeyValueLen && j < nLen)
            {
                nResult = lpabyKeyValue[j] - lpabyData[j];
                if (nResult != 0)
                    break;

                ++j;
            }

            if (nResult == 0)
                break;
        }

        // Skip over the candidate key value

        lpabyData += nLen;
        i += nLen;

        lpabyData += nExtraBytes;
        i += nExtraBytes;
    }

    // The item was not found?

    if (i + nKeyValueLen > nMaxCandOffset)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Delete the item

    lpabyData = lpstPage->abyData;

    // Move the items below forward

    nLen += nExtraBytes;
    for (i+=nLen;i<nMaxOffset;i++)
        lpabyData[i - nLen] = lpabyData[i];

    // Zero out the remaining bytes

    for (i-=nLen;i<nMaxOffset;i++)
        lpabyData[i] = 0;

    // Set the new maximum offset

    nMaxOffset -= nLen;

    // Modify the empty space WORD

    lpstPage->wEmptySpace += nLen;
    lpstPage->wEmptySpace = WENDIAN(lpstPage->wEmptySpace);

    ////////////////////////////////////////////////////////
    // Readjust the table

    // First, zero out the table

    for (i=0;i<A97_P04_TABLE_SIZE;i++)
        lpstPage->abyTable[i] = 0;

    // Now, set the table bits

    i = lpstPage->byCommonPrefixLen;
    lpabyData += lpstPage->byCommonPrefixLen;
    nMaxCandOffset = nMaxOffset - nExtraBytes;
    nBitOffset = 0;
    while (i < nMaxOffset)
    {
        // Get the length of the current object

        if (lpfnKeyValueLen(lpabyData,
                            nPrefixFlags,
                            nMaxCandOffset - i,
                            &nLen) == FALSE)
            return(FALSE);

        // Update the index and object pointer

        nLen += nExtraBytes;
        i += nLen;
        lpabyData += nLen;

        // Make sure to add the prefix length for the first object

        if (i == lpstPage->byCommonPrefixLen)
            nLen += lpstPage->byCommonPrefixLen;

        // Set the bit

        nBitOffset += 16 * (nLen >> 3) - nLen - 1;
        if (i - nBitOffset >= 16)
            nBitOffset += 16;

        lpstPage->abyTable[nBitOffset >> 3] |= 1 << ((~nBitOffset) & 7);

        ++nBitOffset;
    }

    ////////////////////////////////////////////////////////
    // Write back the modified page

    if (ACSFileSeek(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    dwPage * A97_PAGE_SIZE,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileWrite(lpstA97->lpvRootCookie,
                     lpstA97->lpvFile,
                     lpstPage,
                     A97_PAGE_SIZE,
                     &dwByteCount) == FALSE ||
        dwByteCount != A97_PAGE_SIZE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97GetFirstIndexPagePrefix()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  lpfnPrefixFlags     Ptr to prefix flags function
//  lpfnKeyValueLen     Ptr to key value length function
//  nKeyValueLen        Length of key value to search
//  lpabyKeyValue       Ptr to key value to search
//  dwPage              Root index page to start search
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

BOOL A97GetFirstIndexPagePrefix
(
    LPA97               lpstA97,
    DWORD               dwPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    int                 nKeyValueLen,
    LPBYTE              lpabyKeyValue,
    LPDWORD             lpdwPage
)
{
    LPA97_P04           lpstPage;
    DWORD               dwByteCount;
    int                 i;
    int                 nIndex;
    int                 nResult;

    lpstPage = (LPA97_P04)(lpstA97->abyTempPage);

    // Search up to 256 pages

    for (i=0;i<256;i++)
    {
        if (ACSFileSeek(lpstA97->lpvRootCookie,
                        lpstA97->lpvFile,
                        dwPage * A97_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstA97->lpvRootCookie,
                        lpstA97->lpvFile,
                        lpstPage,
                        A97_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A97_PAGE_SIZE)
            return(FALSE);

        // If it is a type 0x04 page, then return it

        if (lpstPage->abyType[0] == 0x04)
        {
            // Get the length of the first item

            if (lpfnKeyValueLen(lpstPage->abyData,
                                0,
                                A97_P04_DATA_SIZE,
                                &nIndex) == FALSE)
                return(FALSE);

            // Check the prefix of the first item
            //  If it is less than the first item and the
            //  prefix doesn't match, then return FALSE

            for (i=0;i<nKeyValueLen && i < nIndex;i++)
            {
                nResult = lpabyKeyValue[i] - lpstPage->abyData[i];
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

        // Find object

        if (A97P04FindIndexEntry(lpstPage,
                                 lpfnPrefixFlags,
                                 lpfnKeyValueLen,
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
//  BOOL A97P04GetItemList()
//
// Parameters:
//  lpstPage            Ptr to type 0x04 page
//  lpfnPrefixFlags     Ptr to prefix flags function
//  lpfnKeyValueLen     Ptr to key value length function
//  lpfnItemListMatch   Ptr to item list matching function
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

BOOL A97P04GetItemList
(
    LPA97_P04           lpstPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    LPFNA97ItemMatch    lpfnItemMatch,
    LPVOID              lpvItemListCookie,
    LPDWORD             lpdwPage,
    LPDWORD             lpadwItems,
    LPDWORD             lpdwNextPage
)
{
    int                 i;
    int                 j;
    DWORD               dwMinPage;
    DWORD               dwMatchPage;
    DWORD               dwNextMatchPage;
    DWORD               dwCandPage;
    LPBYTE              lpabyData;
    int                 nMaxOffset;
    int                 nPrefixFlags;
    int                 nLen;

    // Validate page type

    if (lpstPage->abyType[0] != 0x04)
        return(FALSE);


    // Validate data

    if (WENDIAN(lpstPage->wEmptySpace) > A97_P04_DATA_SIZE)
        return(FALSE);

    nMaxOffset = A97_P04_DATA_SIZE - WENDIAN(lpstPage->wEmptySpace);

    // Get the prefix flags

    lpabyData = lpstPage->abyData;
    if (lpfnPrefixFlags(lpabyData,
                        lpstPage->byCommonPrefixLen,
                        &nPrefixFlags) == FALSE)
        return(FALSE);

    // Skip the prefix

    i = lpstPage->byCommonPrefixLen;
    lpabyData += i;

    ////////////////////////////////////////////////////////
    // Now check all the items

    // Adjust to maximum offset of last item, excluding
    //  page and index size

    nMaxOffset -= 4;

    // Find the first match

    *lpdwNextPage = 0;
    dwMinPage = *lpdwPage;
    dwMatchPage = 0;
    dwNextMatchPage = 0;
    while (i < nMaxOffset)
    {
        // Get the length of the current object

        if (lpfnKeyValueLen(lpabyData,
                            nPrefixFlags,
                            nMaxOffset - i,
                            &nLen) == FALSE)
            return(FALSE);

        // Get page of candidate

        dwCandPage = ((DWORD)lpabyData[nLen] << 16) |
                     ((DWORD)lpabyData[nLen + 1] << 8) |
                     ((DWORD)lpabyData[nLen + 2]);

        // We are always going forward

        if (dwCandPage > dwMinPage)
        {
            // Looking for the first one

            if (lpfnItemMatch(lpvItemListCookie,
                              lpstPage->abyData,
                              lpstPage->byCommonPrefixLen,
                              lpabyData,
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
                    lpadwItems[(lpabyData[nLen+3] >> 5) & 7] |=
                        (DWORD)1 << (lpabyData[nLen+3] & 0x1F);
                }
                else
                if (dwNextMatchPage == 0 || dwCandPage < dwNextMatchPage)
                    dwNextMatchPage = dwCandPage;
            }
        }

        // Skip over the candidate value

        lpabyData += nLen + 4;
        i += nLen + 4;
    }

    *lpdwPage = dwMatchPage;
    *lpdwNextPage = dwNextMatchPage;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97P04MacroItemMatch()
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
//  macro item.  It is if it begins with the byte sequence
//  0x7F 0x00 0x00 0x00 0x?? 0x7F given by the item list cookie.
//
// Returns:
//  TRUE                If the item is a match
//  FALSE               If the item is not a match
//
//********************************************************************

BOOL A97P04MacroItemMatch
(
    LPVOID              lpvItemListCookie,
    LPBYTE              lpabyPrefix,
    int                 nPrefixLen,
    LPBYTE              lpabyItem,
    int                 nItemLen
)
{
    int         i;
    LPBYTE      lpabyMacroPrefix;

    lpabyMacroPrefix = (LPBYTE)lpvItemListCookie;

    // Try prefix

    for (i=0;i<nPrefixLen && i<A97_MACRO_PREFIX;i++)
    {
        if (lpabyPrefix[i] != lpabyMacroPrefix[i])
            return(FALSE);
    }

    for (;i<nItemLen && i<A97_MACRO_PREFIX;i++)
    {
        if (lpabyItem[i-nPrefixLen] != lpabyMacroPrefix[i])
            return(FALSE);
    }

    if (i == A97_MACRO_PREFIX)
        return(TRUE);

    // Assume it is not a macro

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A97MacroObjectParse()
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
//  The function assumes that the object info is for a macro and
//  is in the MSysObjects table.
//
//  The form of the object info is assumed to be:
//
//      abyUnknown[0x1F];       // 0x00
//      abyName[];              // 0x1F
//      by0x03;
//      byUnknown0;
//      abySize[3];
//      byUnknown1;
//      byIndex;
//      abyPage[3];
//
//  The last 16 bytes of the object are assumed to be in this form:
//
//      BYTE    byInfoSize;      // 00: Size of object info
//      BYTE    byDescOffset0;   // 01: Description size and page
//      BYTE    byDescOffset1;   // 02: Same as byDescOffset0
//      BYTE    abyUnknown0[6];  // 03:
//      BYTE    byObjectOffset;  // 09: Object size and page
//      BYTE    byNameEndOffset; // 0A: Object name end offset
//      BYTE    byNameBegOffset; // 0B: Object name begin offset
//      BYTE    abyUnknown1[4];  // 0C:
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97MacroObjectParse
(
    LPBYTE              lpabyObject,
    int                 nObjectSize,
    LPBYTE FAR *        lplpabypsszName,
    LPDWORD             lpdwSize,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex
)
{
    int                     i;
    LPA97OBJECTID_OBJ_INFO  lpstInfo;

    // Make sure it is of the correct type

    if (nObjectSize > 9)
    {
        if (lpabyObject[9] != 0x02)
        {
            // Not of the correct type

            return(FALSE);
        }
    }

    // Get pointer to object info

    lpstInfo = (LPA97OBJECTID_OBJ_INFO)(lpabyObject + nObjectSize -
        sizeof(A97OBJECTID_OBJ_INFO_T));

    // Validate offsets

    if (nObjectSize < sizeof(A97OBJECTID_OBJ_INFO_T) ||
        lpstInfo->byInfoSize > nObjectSize ||
        lpstInfo->byObjectOffset0 + 8 > lpstInfo->byInfoSize ||
        lpstInfo->byNameEndOffset > lpstInfo->byInfoSize ||
        lpstInfo->byNameBegOffset > lpstInfo->byNameEndOffset ||
        lpstInfo->byNameBegOffset < 1)
        return(FALSE);

    // Set the Pascal length of the name

    lpabyObject[lpstInfo->byNameBegOffset - 1] =
        lpstInfo->byNameEndOffset - lpstInfo->byNameBegOffset;

    // Zero terminate the name

    lpabyObject[lpstInfo->byNameEndOffset] = 0;

    // Set the name pointer

    *lplpabypsszName = lpabyObject + lpstInfo->byNameBegOffset - 1;

    // Set the size and location

    i = lpstInfo->byObjectOffset0;

    // Get the size including the next pointers

    *lpdwSize = ((DWORD)lpabyObject[i]) |
                ((DWORD)lpabyObject[i+1] << 8) |
                ((DWORD)lpabyObject[i+2] << 16);

    // Get the page and index containing the object

    *lpnIndex = lpabyObject[i+4];
    *lpdwPage = ((DWORD)lpabyObject[i+5]) |
                ((DWORD)lpabyObject[i+6] << 8) |
                ((DWORD)lpabyObject[i+7] << 16);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97EnumItems()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  dwPage              Type 0x04 index page with first item
//  lpfnPrefixFlags     Ptr to prefix flags function
//  lpfnKeyValueLen     Ptr to key value length function
//  lpfnP04ItemMatch    Ptr to item match function
//  lpvItemMatchCookie  Item match cookie
//  lpfnObjectParse     Ptr to object parsing function
//  lpfnEnumCB          Ptr to enumeration callback
//  lpvEnumCBCookie     Enumeration callback cookie
//
// Description:
//  The function enumerates all the objects starting from with the
//  object on the given index page, calling the given callback
//  function for each object.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97EnumItems
(
    LPA97               lpstA97,
    DWORD               dwPage,
    LPFNA97PrefixFlags  lpfnPrefixFlags,
    LPFNA97KeyValueLen  lpfnKeyValueLen,
    LPFNA97P04ItemMatch lpfnP04ItemMatch,
    LPVOID              lpvItemMatchCookie,
    LPFNA97ObjectParse  lpfnObjectParse,
    LPFNA97ObjectEnumCB lpfnEnumCB,
    LPVOID              lpvEnumCBCookie
)
{
    LPA97_P04           lpstPage;
    LPA97_P01           lpstItemPage;
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
    lpstPage = (LPA97_P04)(lpstA97->abyTempPage);
    lpstItemPage = (LPA97_P01)(lpstA97->abyPage);
    for (i=0;i<1024 && dwPage!=0;i++)
    {
        // Read the page

        if (ACSFileSeek(lpstA97->lpvRootCookie,
                        lpstA97->lpvFile,
                        dwPage * A97_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstA97->lpvRootCookie,
                        lpstA97->lpvFile,
                        lpstPage,
                        A97_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A97_PAGE_SIZE)
            return(FALSE);

        // Get the matching item list

        if (A97P04GetItemList(lpstPage,
                              lpfnPrefixFlags,
                              lpfnKeyValueLen,
                              lpfnP04ItemMatch,
                              lpvItemMatchCookie,
                              &dwSetPage,
                              adwSetItems,
                              &dwNextSetPage) == FALSE)
            break;

        if (dwNextSetPage == 0)
        {
            // Get the next index page number

            dwPage = DWENDIAN(lpstPage->dwNextPage);

            if (dwSetPage == 0)
            {
                // If it is the first page, always go one additional

                if (i != 0)
                {
                    // No more

                    break;
                }
                else
                {
                    // Restart for page

                    dwSetPage = 0;
                    dwNextSetPage = 0;
                    continue;
                }
            }
        }

        nItemCount = 256;
        bReread = TRUE;
        for (j=0;j<nItemCount;j++)
        {
            if (bReread != FALSE)
            {
                // Read the item set page

                if (ACSFileSeek(lpstA97->lpvRootCookie,
                                lpstA97->lpvFile,
                                dwSetPage * A97_PAGE_SIZE,
                                ACS_SEEK_SET,
                                NULL) == FALSE)
                    break;

                if (ACSFileRead(lpstA97->lpvRootCookie,
                                lpstA97->lpvFile,
                                lpstItemPage,
                                A97_PAGE_SIZE,
                                &dwByteCount) == FALSE ||
                    dwByteCount != A97_PAGE_SIZE)
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

            if ((wItemOffset & 0xC000) == 0x4000)
            {
                bReread = TRUE;

                wItemOffset &= 0x0FFF;
                if (wItemOffset > A97_PAGE_SIZE - 4)
                    continue;

                nIndex = lpstA97->abyPage[wItemOffset];
                dwObjectPage = ((DWORD)lpstA97->abyPage[wItemOffset+1]) |
                               ((DWORD)lpstA97->abyPage[wItemOffset+2] << 8) |
                               ((DWORD)lpstA97->abyPage[wItemOffset+3] << 16);

                // Read the item set page

                if (ACSFileSeek(lpstA97->lpvRootCookie,
                                lpstA97->lpvFile,
                                dwObjectPage * A97_PAGE_SIZE,
                                ACS_SEEK_SET,
                                NULL) == FALSE)
                    continue;

                if (ACSFileRead(lpstA97->lpvRootCookie,
                                lpstA97->lpvFile,
                                lpstItemPage,
                                A97_PAGE_SIZE,
                                &dwByteCount) == FALSE ||
                    dwByteCount != A97_PAGE_SIZE)
                    continue;

                if (nIndex >= WENDIAN(lpstItemPage->wItemCount))
                    continue;

                wItemOffset = 0x7FFF &
                    WENDIAN(lpstItemPage->awItemOffsets[nIndex]);
            }
            else
                nIndex = j;

            // Ignore deleted items

            if (wItemOffset >= A97_PAGE_SIZE)
                continue;

            // Get the end offset

            if (nIndex == 0)
                wEndOffset = A97_PAGE_SIZE;
            else
            {
                wEndOffset = 0xFFF &
                    WENDIAN(lpstItemPage->awItemOffsets[nIndex-1]);

                if (wEndOffset > A97_PAGE_SIZE)
                    wEndOffset = A97_PAGE_SIZE;
            }

            if (wEndOffset < wItemOffset)
                continue;

            // Get the item data

            if (lpfnObjectParse(lpstA97->abyPage + wItemOffset,
                                (int)(wEndOffset - wItemOffset),
                                &lpabypsszName,
                                &dwByteCount,
                                &dwObjectPage,
                                &nIndex) == FALSE)
                continue;

#ifdef SYM_NLM
            ACSProgress(lpstA97->lpvRootCookie);
#endif

            // Call the callback function

            if (lpfnEnumCB(lpstA97,
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
//  BOOL A97GetMacroPrefixID()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  dwPage              Root objects index page
//  lpabyPrefix         Ptr to filled six-byte prefix buffer
//
// Description:
//  Sets the fourth byte of the prefix buffer to the fourth
//  byte of the Scripts object entry.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97GetMacroPrefixID
(
    LPA97               lpstA97,
    DWORD               dwPage,
    LPBYTE              lpabyPrefix
)
{
    LPA97_P01           lpstPage;
    int                 nIndex;
    BOOL                bFound;
    DWORD               dwByteCount;
    WORD                wOffset;

    lpstPage = (LPA97_P01)lpstA97->abyTempPage;

    // Get the Scripts ID

    if (A97P04FindIndexedKeyValue(lpstA97,
                                  dwPage,
                                  A97ObjectIDPrefixFlags,
                                  A97ObjectIDKeyValueLen,
                                  sizeof(gabyA97ScriptsObjectsKey),
                                  gabyA97ScriptsObjectsKey,
                                  &dwPage,
                                  &nIndex,
                                  &bFound) == FALSE ||
        bFound == FALSE)
        return(FALSE);

    // Read the page containing the Scripts object

    if (ACSFileSeek(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    dwPage * A97_PAGE_SIZE,
                    ACS_SEEK_SET,
                    NULL) == FALSE)
        return(FALSE);

    if (ACSFileRead(lpstA97->lpvRootCookie,
                    lpstA97->lpvFile,
                    lpstPage,
                    A97_PAGE_SIZE,
                    &dwByteCount) == FALSE ||
        dwByteCount != A97_PAGE_SIZE)
        return(FALSE);

    // Validate index

    if (nIndex >= WENDIAN(lpstPage->wItemCount) ||
        nIndex >= A97_P01_MAX_ITEMS)
        return(FALSE);

    wOffset = WENDIAN(lpstPage->awItemOffsets[nIndex]);

    // Assume that the object will never be a reference to another object

    if ((wOffset & 0xF000) != 0x0000)
        return(FALSE);

    // Validate offset

    if (wOffset > A97_PAGE_SIZE - 2)
        return(FALSE);

    lpabyPrefix[4] = lpstA97->abyTempPage[wOffset + 1];

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97EnumMacros()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  lpfnEnumCB          Ptr to enumeration callback
//  lpvEnumCBCookie     Enumeration callback cookie
//
// Description:
//  The function enumerates all the VBA modules in the given
//  Access 97 object, calling the given callback function
//  for each such object.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97EnumMacros
(
    LPA97               lpstA97,
    LPFNA97ObjectEnumCB lpfnEnumCB,
    LPVOID              lpvEnumCBCookie
)
{
    DWORD               dwFirstIndexPage;
    DWORD               dwPage;
    BYTE                abyPrefix[A97_MACRO_PREFIX] =
                            { 0x7F, 0, 0, 0, 4, 0x7F };

    // Get the first index page

    if (A97P02GetFirstIndexPage(lpstA97,
                                A97_OBJECTS_MASTER_PAGE,
                                gabypsA97ParentId,
                                &dwFirstIndexPage) == FALSE)
        return(FALSE);

    // Get the Macro prefix

    if (A97GetMacroPrefixID(lpstA97,
                            dwFirstIndexPage,
                            abyPrefix) == FALSE)
        return(FALSE);

    // Try the first prefix: Get the first macro index page

    if (A97GetFirstIndexPagePrefix(lpstA97,
                                   dwFirstIndexPage,
                                   A97ObjectIDPrefixFlags,
                                   A97ObjectIDKeyValueLen,
                                   sizeof(abyPrefix),
                                   abyPrefix,
                                   &dwPage) == FALSE)
        return(FALSE);

    // Were there any such pages?

    if (dwPage != 0)
    {
        // Enumerate through the macros

        if (A97EnumItems(lpstA97,
                         dwPage,
                         A97ObjectIDPrefixFlags,
                         A97ObjectIDKeyValueLen,
                         A97P04MacroItemMatch,
                         abyPrefix,
                         A97MacroObjectParse,
                         lpfnEnumCB,
                         lpvEnumCBCookie) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97GetVBAMasterPage()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  b95                 TRUE for VBA 95 master page
//  lpdwPage            Ptr to DWORD for master VBA page
//
// Description:
//  The function retrieves the master VBA page by looking up the
//  MSysModules2 item in the Objects table and using bytes [1..3]
//  as the master VBA page number.
//
// Returns:
//  TRUE                On success
//  FALSE               Error
//
//********************************************************************

BOOL A97GetVBAMasterPage
(
    LPA97               lpstA97,
    BOOL                b95,
    LPDWORD             lpdwPage
)
{
    LPA97_P01           lpstPage;
    DWORD               dwByteCount;
    DWORD               dwPage;
    int                 nIndex;
    BOOL                bFound;
    WORD                wOffset;
    int                 i;

    // Get the first index page

    if (A97P02GetFirstIndexPage(lpstA97,
                                A97_OBJECTS_MASTER_PAGE,
                                gabypsA97ParentId,
                                &dwPage) == FALSE)
        return(FALSE);

    if (b95 == FALSE)
    {
        // First find the object page containing the MSysModules2 item

        if (A97P04FindIndexedKeyValue(lpstA97,
                                      dwPage,
                                      A97ObjectIDPrefixFlags,
                                      A97ObjectIDKeyValueLen,
                                      sizeof(gabyA97MSysModules2ObjectsKey),
                                      gabyA97MSysModules2ObjectsKey,
                                      &dwPage,
                                      &nIndex,
                                      &bFound) == FALSE)
            return(FALSE);
    }
    else
    {
        // First find the object page containing the MSysModules item

        if (A97P04FindIndexedKeyValue(lpstA97,
                                      dwPage,
                                      A97ObjectIDPrefixFlags,
                                      A97ObjectIDKeyValueLen,
                                      sizeof(gabyA97MSysModulesObjectsKey),
                                      gabyA97MSysModulesObjectsKey,
                                      &dwPage,
                                      &nIndex,
                                      &bFound) == FALSE)
            return(FALSE);
    }

    if (bFound == FALSE)
    {
        *lpdwPage = 0;
        return(TRUE);
    }

    lpstPage = (LPA97_P01)(lpstA97->abyTempPage);

    for (i=0;i<256;i++)
    {
        // Read the page containing the item

        if (ACSFileSeek(lpstA97->lpvRootCookie,
                        lpstA97->lpvFile,
                        dwPage * A97_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstA97->lpvRootCookie,
                        lpstA97->lpvFile,
                        lpstPage,
                        A97_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A97_PAGE_SIZE)
            return(FALSE);

        // Validate the index

        lpstPage->wItemCount = WENDIAN(lpstPage->wItemCount);
        if (nIndex >= lpstPage->wItemCount ||
            lpstPage->wItemCount > A97_P01_MAX_ITEMS)
            return(FALSE);

        // Validate the item offset

        wOffset = WENDIAN(lpstPage->awItemOffsets[nIndex]);

        // Has the item been relocated?

        if (wOffset & 0xC000)
        {
            switch (wOffset & 0xC000)
            {
                case 0x4000:
                    // The item has been relocated

                    wOffset &= 0x0FFF;
                    if (wOffset > A97_PAGE_SIZE - 4)
                        return(FALSE);

                    nIndex = lpstA97->abyTempPage[wOffset];
                    dwPage = ((DWORD)lpstA97->abyTempPage[wOffset+1]) |
                             ((DWORD)lpstA97->abyTempPage[wOffset+2] << 8) |
                             ((DWORD)lpstA97->abyTempPage[wOffset+3] << 16);

                    continue;

                case 0xC000:
                    return(FALSE);

                default:
                    wOffset &= 0x0FFF;
                    break;
            }
        }

        // Is it out of bounds?

        if (wOffset + 4 > A97_PAGE_SIZE)
            return(FALSE);

        // Bytes [1..3] give the page containing the master index

        *lpdwPage = ((DWORD)lpstA97->abyTempPage[wOffset + 1]) |
                    ((DWORD)lpstA97->abyTempPage[wOffset + 2] << 8) |
                    ((DWORD)lpstA97->abyTempPage[wOffset + 3] << 16);

        return(TRUE);
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A97VBAModulePrefixFlags()
//
// Parameters:
//  lpabyKeyValue       Ptr to prefix
//  nPrefixLen          Length of prefix
//  lpnPrefixFlags      Ptr to int to receive prefix flags
//
// Description:
//  Returns the prefix flags for a VBA module index page.
//  The prefix flags are equal to the prefix length.
//
// Returns:
//  TRUE                Always
//
//********************************************************************

BOOL A97VBAModulePrefixFlags
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixLen,
    LPINT               lpnPrefixFlags
)
{
    (void)lpabyKeyValue;

    *lpnPrefixFlags = nPrefixLen;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97VBAModuleKeyValueLen()
//
// Parameters:
//  lpabyKeyValue       Ptr to key value
//  nPrefixFlags        Prefix flags
//  nMaxOffset          Maximum offset of key value
//  lpnLen              Ptr to int for key value length
//
// Description:
//  The ID key value length includes the first five bytes,
//  and the string including the zero-terminator.
//
//  The function assumes that the zero terminator is not
//  in the prefix.
//
//  Returns the key value length for a VBA module index page.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97VBAModuleKeyValueLen
(
    LPBYTE              lpabyKeyValue,
    int                 nPrefixFlags,
    int                 nMaxOffset,
    LPINT               lpnLen
)
{
    int         i;

    (void)nPrefixFlags;

    i = 0;

    // Find the zero-terminator

    while (i < nMaxOffset)
    {
        if (lpabyKeyValue[i++] == 0)
        {
            if (i + 10 <= nMaxOffset)
            {
                *lpnLen = i + 10;
                return(TRUE);
            }

            return(FALSE);
        }
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A97P04VBAModuleItemMatch()
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
//  VBA module item.  The byte at six bytes before the end
//  of the item key value will have value 0x07.
//
// Returns:
//  TRUE                If the item is a match
//  FALSE               If the item is not a match
//
//********************************************************************

BOOL A97P04VBAModuleItemMatch
(
    LPVOID              lpvItemListCookie,
    LPBYTE              lpabyPrefix,
    int                 nPrefixLen,
    LPBYTE              lpabyItem,
    int                 nItemLen
)
{
    (void)lpvItemListCookie;
    (void)lpabyPrefix;
    (void)nPrefixLen;

    if (nItemLen - 6 >= 0 && lpabyItem[nItemLen - 6] == 0x07)
    {
        // It is most likely a VBA module

        return(TRUE);
    }

    // Assume it is not a VBA module

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL A97VBAModuleObjectParse()
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
//  The function assumes that the object info is for a VBA module
//  and is in the MSysModules2 table.
//
//  The form of the object info is assumed to be:
//
//      byUnknown0;             // 0x00
//      abyName[];              // 0x01
//      by0x07;
//      abyUnknown[15];
//      abySize[3];
//      byUnknown1;
//      byIndex;
//      abyPage[3];
//
//  The last 9 bytes of the object are assumed to be in this form:
//
//      BYTE    byInfoSize;      // 00: Size of object info
//      BYTE    abyUnknown0[2];  // 01:
//      BYTE    byObjectOffset;  // 03: Object size and page
//      BYTE    abyUnknown1[3];  // 04:
//      BYTE    byTypeOffset;    // 07: Object name end offset
//      BYTE    byNameOffset;    // 08: Object name begin offset
//      BYTE    abyUnknown2[2];  // 09:
//
//      BYTE    byInfoSize;      // 00: Size of object info
//      BYTE    abyUnknown0[2];  // 01:
//      BYTE    byTypeOffset;    // 03:
//      BYTE    byNameEndOffset; // 04: Object name end offset
//      BYTE    byNameBegOffset; // 05: Object name begin offset
//      BYTE    byObjectOffset;  // 06: Object size and page
//      BYTE    abyUnknown1[4];  // 07:
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97VBAModuleObjectParse
(
    LPBYTE              lpabyObject,
    int                 nObjectSize,
    LPBYTE FAR *        lplpabypsszName,
    LPDWORD             lpdwSize,
    LPDWORD             lpdwPage,
    LPINT               lpnIndex
)
{
    LPA97MODULE_OBJ_INFO    lpstInfo;
    int                     nNameBegOffset;
    int                     nNameEndOffset;
    int                     nObjectOffset;

    lpstInfo = (LPA97MODULE_OBJ_INFO)(lpabyObject + nObjectSize -
        sizeof(A97MODULE_OBJ_INFO_T));

    if (nObjectSize < sizeof(A97MODULE_OBJ_INFO_T) ||
        lpstInfo->byInfoSize > nObjectSize)
        return(FALSE);

    if (lpstInfo->byInfoSize == lpstInfo->abyUnknown0[0])
    {
        nObjectOffset = lpstInfo->i.a.byObjectOffset;
        nNameBegOffset = lpstInfo->i.a.byNameBegOffset;
        nNameEndOffset = lpstInfo->i.a.byNameEndOffset;
    }
    else
    {
        nObjectOffset = lpstInfo->i.b.byObjectOffset;
        nNameBegOffset = lpstInfo->i.b.byNameBegOffset;
        nNameEndOffset = lpstInfo->i.b.byNameEndOffset;
    }

    // Validate offsets

    if (nObjectOffset + 8 > lpstInfo->byInfoSize ||
        nNameEndOffset > lpstInfo->byInfoSize ||
        nNameBegOffset > nNameEndOffset ||
        nNameBegOffset == 0)
        return(FALSE);

    // Set the Pascal length of the name

    lpabyObject[nNameBegOffset - 1] = nNameEndOffset - nNameBegOffset;

    // Zero terminate the name

    lpabyObject[nNameEndOffset] = 0;

    // Set the name pointer

    *lplpabypsszName = lpabyObject + nNameBegOffset - 1;

    // Get the size including the next pointers

    *lpdwSize = ((DWORD)lpabyObject[nObjectOffset]) |
                ((DWORD)lpabyObject[nObjectOffset+1] << 8) |
                ((DWORD)lpabyObject[nObjectOffset+2] << 16);

    // Get the page and index containing the object

    *lpnIndex = lpabyObject[nObjectOffset+4];
    *lpdwPage = ((DWORD)lpabyObject[nObjectOffset+5]) |
                ((DWORD)lpabyObject[nObjectOffset+6] << 8) |
                ((DWORD)lpabyObject[nObjectOffset+7] << 16);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97EnumVBAModules()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  b95                 TRUE if to enumerate VBA 95 modules
//  lpfnEnumCB          Ptr to enumeration callback
//  lpvEnumCBCookie     Enumeration callback cookie
//
// Description:
//  The function enumerates all the VBA modules in the given
//  Access 97 object, calling the given callback function
//  for each such object.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL A97EnumVBAModules
(
    LPA97               lpstA97,
    BOOL                b95,
    LPFNA97ObjectEnumCB lpfnEnumCB,
    LPVOID              lpvEnumCBCookie
)
{
    DWORD               dwPage;
    BYTE                byPrefix;

    // Get the master VBA page

    if (A97GetVBAMasterPage(lpstA97,b95,&dwPage) == FALSE)
        return(FALSE);

    if (dwPage == 0)
        return(TRUE);

    // Get the first index page

    if (A97P02GetFirstIndexPage(lpstA97,
                                dwPage,
                                gabypsA97Name,
                                &dwPage) == FALSE)
        return(FALSE);

    // Get the first VBA module index page

    byPrefix = 0x7F;
    if (A97GetFirstIndexPagePrefix(lpstA97,
                                   dwPage,
                                   A97VBAModulePrefixFlags,
                                   A97VBAModuleKeyValueLen,
                                   1,
                                   &byPrefix,
                                   &dwPage) == FALSE)
        return(FALSE);

    // Were there any such pages?

    if (dwPage == 0)
        return(TRUE);

    // Enumerate through the modules

    if (A97EnumItems(lpstA97,
                     dwPage,
                     A97VBAModulePrefixFlags,
                     A97VBAModuleKeyValueLen,
                     A97P04VBAModuleItemMatch,
                     NULL,
                     A97VBAModuleObjectParse,
                     lpfnEnumCB,
                     lpvEnumCBCookie) == FALSE)
        return(FALSE);


    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97ObjAccessInit()
//
// Parameters:
//  lpstA97             Ptr to A97_T structure
//  dwSize              Size of object
//  dwPage              First page of object
//  nIndex              Index of object on first page
//  lpstObj             Ptr to A97_OBJ_ACCESS_T structure to init
//
// Description:
//  Initializes an A97_OBJ_ACCESS_T structure for random access
//  to the given object.
//
// Returns:
//  TRUE                Always
//
//********************************************************************

BOOL A97ObjAccessInit
(
    LPA97               lpstA97,
    DWORD               dwSize,
    DWORD               dwPage,
    int                 nIndex,
    LPA97_OBJ_ACCESS    lpstObj
)
{
    lpstObj->lpstA97 = lpstA97;
    lpstObj->dwSize = dwSize;
    lpstObj->nNumCachedPages = 0;
    lpstObj->nNextIndex = nIndex;
    lpstObj->dwNextPage = dwPage;
    lpstObj->wLastPageSize = 0;
    lpstObj->dwLastPageRelOffset = 0xFFFFFFFF;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL A97ObjAccessRead()
//
// Parameters:
//  lpstObj             Ptr to A97_OBJ_ACCESS_T structure
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

BOOL A97ObjAccessRead
(
    LPA97_OBJ_ACCESS    lpstObj,
    DWORD               dwOffset,
    LPVOID              lpvBuffer,
    DWORD               dwNumBytes,
    LPDWORD             lpdwBytesRead
)
{
    LPA97_P01           lpstPage;
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
    BOOL                bFromLast;

    *lpdwBytesRead = dwNumBytes;

    // Search through the cached pages first

    lpabyBuf = (LPBYTE)lpvBuffer;

    dwCurOffset = lpstObj->dwLastPageRelOffset;
    if (dwCurOffset <= dwOffset &&
        dwOffset < dwCurOffset + lpstObj->wLastPageSize)
    {
        if (dwOffset + dwNumBytes >
            dwCurOffset + lpstObj->wLastPageSize)
            dwSize = dwCurOffset + lpstObj->wLastPageSize - dwOffset;
        else
            dwSize = dwNumBytes;

#ifdef SYM_NLM
        // Call progress before reading

        ACSProgress(lpstObj->lpstA97->lpvRootCookie);
#endif

        // Read into the buffer

        if (ACSFileSeek(lpstObj->lpstA97->lpvRootCookie,
                        lpstObj->lpstA97->lpvFile,
                        lpstObj->dwLastPageOffset +
                            (dwOffset - dwCurOffset),
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstObj->lpstA97->lpvRootCookie,
                        lpstObj->lpstA97->lpvFile,
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

        bFromLast = TRUE;
    }
    else
    if (dwOffset == dwCurOffset + lpstObj->wLastPageSize)
        bFromLast = TRUE;
    else
        bFromLast = FALSE;

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

#ifdef SYM_NLM
            // Call progress before reading

            ACSProgress(lpstObj->lpstA97->lpvRootCookie);
#endif

            // Read into the buffer

            if (ACSFileSeek(lpstObj->lpstA97->lpvRootCookie,
                            lpstObj->lpstA97->lpvFile,
                            lpstObj->adwPageOffset[i] +
                                (dwOffset - dwCurOffset),
                            ACS_SEEK_SET,
                            NULL) == FALSE)
                return(FALSE);

            if (ACSFileRead(lpstObj->lpstA97->lpvRootCookie,
                            lpstObj->lpstA97->lpvFile,
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

    // Start reading from the next page and index

    lpstPage = (LPA97_P01)(lpstObj->lpstA97->abyTempPage);
    if (bFromLast == FALSE || i < A97_OBJ_MAX_CACHED_PAGES)
    {
        nPageI = i;
        nIndex = lpstObj->nNextIndex;
        dwPage = lpstObj->dwNextPage;
    }
    else
    {
        nPageI = A97_OBJ_MAX_CACHED_PAGES;
        nIndex = lpstObj->nLastNextIndex;
        dwPage = lpstObj->dwLastNextPage;
        dwCurOffset = lpstObj->dwLastPageRelOffset +
            lpstObj->wLastPageSize;
    }

    while (nPageI < 1024 && dwPage != 0)
    {
#ifdef SYM_NLM
        // Call progress every 8th page

        if ((nPageI & 0x7) == 0)
            ACSProgress(lpstObj->lpstA97->lpvRootCookie);
#endif

        // Read the page

        if (ACSFileSeek(lpstObj->lpstA97->lpvRootCookie,
                        lpstObj->lpstA97->lpvFile,
                        dwPage * A97_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(FALSE);

        if (ACSFileRead(lpstObj->lpstA97->lpvRootCookie,
                        lpstObj->lpstA97->lpvFile,
                        lpstPage,
                        A97_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A97_PAGE_SIZE)
            return(FALSE);

        // Validate page type

        if (lpstPage->abyType[0] != 0x01 || lpstPage->abyType[1] != 0x01)
            return(FALSE);

        if (nIndex >= A97_P01_MAX_ITEMS ||
            nIndex >= WENDIAN(lpstPage->wItemCount))
            return(FALSE);

        // Get the start offset

        wStart = WENDIAN(lpstPage->awItemOffsets[nIndex]);
        if (wStart > A97_PAGE_SIZE - 4)
            wStart = A97_PAGE_SIZE;

        // Get the end offset

        if (nIndex == 0)
            wEnd = A97_PAGE_SIZE;
        else
        {
            wEnd = WENDIAN(lpstPage->awItemOffsets[nIndex - 1]) & 0xFFF;
            if (wEnd > A97_PAGE_SIZE)
                wEnd = A97_PAGE_SIZE;
        }

        // Get the size of the object's data on this page

        if (wStart + 4 >= wEnd)
        {
            // No data

            if (wStart >= wEnd)
                wSize = 0;
            else
                wSize = wEnd - wStart;

            nIndex = 0;
            dwNextPage = 0;
        }
        else
        {
            wSize = wEnd - wStart;
            if (nPageI == 0 &&
                wSize == lpstObj->dwSize &&
                lpstObj->lpstA97->abyTempPage[wStart] != 0)
            {
                // Assume the whole item fits in one entry

                nIndex = 0;
                dwNextPage = 0;
            }
            else
            {
                wSize -= 4;
                nIndex = lpstObj->lpstA97->abyTempPage[wStart];
                dwNextPage =
                    ((DWORD)lpstObj->lpstA97->abyTempPage[wStart + 1]) |
                    ((DWORD)lpstObj->lpstA97->abyTempPage[wStart + 2] << 8) |
                    ((DWORD)lpstObj->lpstA97->abyTempPage[wStart + 3] << 16);
                wStart += 4;
            }
        }

        // Is it cacheable?

        if (nPageI < A97_OBJ_MAX_CACHED_PAGES)
        {
            lpstObj->nNumCachedPages++;
            lpstObj->awPageSize[nPageI] = wSize;
            lpstObj->adwPageOffset[nPageI] =
                dwPage * A97_PAGE_SIZE + wStart;
            lpstObj->nNextIndex = nIndex;
            lpstObj->dwNextPage = dwNextPage;
        }

        // Always cache the info for the last page

        lpstObj->wLastPageSize = wSize;
        lpstObj->dwLastPageRelOffset = dwCurOffset;
        lpstObj->dwLastPageOffset = dwPage * A97_PAGE_SIZE + wStart;
        lpstObj->nLastNextIndex = nIndex;
        lpstObj->dwLastNextPage = dwNextPage;

        // Are there any bytes in this buffer to copy?

        if (dwCurOffset <= dwOffset && dwOffset < dwCurOffset + wSize)
        {
            if (dwOffset + dwNumBytes > dwCurOffset + wSize)
                dwByteCount = dwCurOffset + wSize - dwOffset;
            else
                dwByteCount = dwNumBytes;

            lpabySrc = lpstObj->lpstA97->abyTempPage + wStart +
                (dwOffset - dwCurOffset);

            // Update number of bytes left and offset

            dwNumBytes -= dwByteCount;
            dwOffset += dwByteCount;

            while (dwByteCount-- != 0)
                *lpabyBuf++ = *lpabySrc++;

            // Return if no more bytes

            if (dwNumBytes == 0)
                return(TRUE);
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


