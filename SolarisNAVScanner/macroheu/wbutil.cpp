//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/WBUTIL.CPv   1.5   09 Nov 1998 13:43:52   DCHI  $
//
// Description:
//  Macro emulation environment utility functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/WBUTIL.CPv  $
// 
//    Rev 1.5   09 Nov 1998 13:43:52   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.4   05 Aug 1997 16:04:28   DCHI
// Added PascalStrHasNonNumChars() and corrected PascalStrToLong().
// 
//    Rev 1.3   29 Jul 1997 19:23:36   DCHI
// Fixed backwards conversion in LongToPascalStr().
// 
//    Rev 1.2   15 Jul 1997 19:15:54   DCHI
// Added Japanese "global" string.
// 
//    Rev 1.1   11 Jul 1997 18:02:56   DCHI
// Corrected problem postincrementation problem with Mid$().
// 
//    Rev 1.0   30 Jun 1997 16:15:50   DCHI
// Initial revision.
// 
//************************************************************************

#include "ssbstype.h"
#include "wbutil.h"

//*************************************************************************
//
// int PascalStrCmp()
//
// Parameters:
//  lpbyStr0            Ptr to string 0
//  lpbyStr1            Ptr to string 1
//
// Description:
//  This function performs a case sensitive compare of two Pascal strings.
//
// Returns:
//  -1          String 0 less than string 1
//  0           String 0 equal to string 1
//  1           String 0 greater than string 1
//
//*************************************************************************

int PascalStrCmp
(
    LPBYTE      lpbyStr0,
    LPBYTE      lpbyStr1
)
{
    int         nLen0, nLen1;
    BYTE        by0, by1;

    nLen0 = *lpbyStr0++;
    nLen1 = *lpbyStr1++;

    while (nLen0 != 0 && nLen1 != 0)
    {
        by0 = *lpbyStr0++;
        by1 = *lpbyStr1++;

        if (by0 < by1)
            return(-1);

        if (by0 > by1)
            return(1);

        nLen0--;
        nLen1--;
    }

    if (nLen0 < nLen1)
        return(-1);

    if (nLen0 > nLen1)
        return(1);

    return(0);
}


//*************************************************************************
//
// int PascalStrICmp()
//
// Parameters:
//  lpbyStr0            Ptr to string 0
//  lpbyStr1            Ptr to string 1
//
// Description:
//  This function performs a case insensitive compare of two
//  Pascal strings.
//
// Returns:
//  -1          String 0 less than string 1
//  0           String 0 equal to string 1
//  1           String 0 greater than string 1
//
//*************************************************************************

int PascalStrICmp
(
    LPBYTE      lpbyStr0,
    LPBYTE      lpbyStr1
)
{
    int         nLen0, nLen1;
    BYTE        by0, by1;

    nLen0 = *lpbyStr0++;
    nLen1 = *lpbyStr1++;

    while (nLen0 != 0 && nLen1 != 0)
    {
        by0 = *lpbyStr0++;
        by1 = *lpbyStr1++;

        if (by0 <= 'Z' && by0 >= 'A')
            by0 = by0 + ('a' - 'A');

        if (by1 <= 'Z' && by1 >= 'A')
            by1 = by1 + ('a' - 'A');

        if (by0 < by1)
            return(-1);

        if (by0 > by1)
            return(1);

        nLen0--;
        nLen1--;
    }

    if (nLen0 < nLen1)
        return(-1);

    if (nLen0 > nLen1)
        return(1);

    return(0);
}


//*************************************************************************
//
// BOOL PascalStrIEq()
//
// Parameters:
//  lpbyStr0            Ptr to string 0
//  lpbyStr1            Ptr to string 1
//
// Description:
//  This function performs a case insensitive compare of two
//  Pascal strings.
//
// Returns:
//  TRUE        String 0 equal to string 1
//  FALSE       String 0 not equal to string 1
//
//*************************************************************************

BOOL PascalStrIEq
(
    LPBYTE      lpbyStr0,
    LPBYTE      lpbyStr1
)
{
    BYTE        byLen;
    BYTE        by0;
    BYTE        by1;

    byLen = *lpbyStr0++;
    if (byLen != *lpbyStr1++)
        return(FALSE);

    while (byLen-- != 0)
    {
        by0 = *lpbyStr0++;
        by1 = *lpbyStr1++;

        if (by0 <= 'Z' && by0 >= 'A')
            by0 = by0 + ('a' - 'A');

        if (by1 <= 'Z' && by1 >= 'A')
            by1 = by1 + ('a' - 'A');

        if (by0 != by1)
            return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL PascalTemplateStrEq()
//
// Parameters:
//  lpbyCand            Ptr to candidate
//  lpbyCheck           Ptr zero-terminated check string
//
// Description:
//  This function performs a case insensitive comparison of a
//  Pascal string against a C string.
//
// Returns:
//  TRUE        Candidate string equal to check string
//  FALSE       Candidate string not equal to check string
//
//*************************************************************************

BOOL PascalTemplateStrEq
(
    LPBYTE      lpbyCand,
    LPBYTE      lpszCheck
)
{
    LPBYTE      lpszCurCheck;
    BYTE        by;
    int         i, nLen;

    nLen = *lpbyCand++;
    lpszCurCheck = lpszCheck;
    for (i=1;i<=nLen && *lpszCurCheck != '\0';i++)
    {
        by = *lpbyCand++;
        if (by <= 'Z' && by >= 'A')
            by += ('a' - 'A');

        if (by != *lpszCurCheck++)
        {
            // No match

            lpszCurCheck = lpszCheck;
        }
    }

    if (*lpszCurCheck == '\0' && nLen > 0 && *lpbyCand == ':')
        return(TRUE);

    return(FALSE);
}


//*************************************************************************
//
// BOOL PascalIsGlobalMacroName()
//
// Parameters:
//  lpbyName            Ptr to qualified name
//
// Description:
//  Determines whether a qualified name refers to a global macro name.
//
// Returns:
//  TRUE        Name refers to a global macro name
//  FALSE       Name does not refer to a global macro name
//
//*************************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

LPBYTE FAR galpbyszGlobalPrefixes[] =
{
    (LPBYTE)"global",
    (LPBYTE)"normal.dot",
    (LPBYTE)"normal",
    (LPBYTE)"algemeen",
    (LPBYTE)"generale",

    // The following is the Japanese global
    //  0x53 0x91  --> 0x73 0x91  (0x73 is lower case 's', 0x53 is 'S')
    //  0xB6 0x95
    //  0x91 0x8F
    //  0xCE 0x91
    //  0xDB 0x8F

    (LPBYTE)"\x73\xB6\x91\xCE\xDB",
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL PascalIsGlobalMacroName
(
    LPBYTE      lpbyName
)
{
    int         i;

    for (i=0;i<sizeof(galpbyszGlobalPrefixes)/sizeof(LPBYTE);i++)
    {
        if (PascalTemplateStrEq(lpbyName,galpbyszGlobalPrefixes[i]) == TRUE)
            return(TRUE);
    }

    for (i=1;i<=lpbyName[0];i++)
        if (lpbyName[i] == ':')
            return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// BOOL PascalIsEqErrStr()
//
// Parameters:
//  lpbyName            Ptr to Pascal string name
//
// Description:
//  Determines whether a Pascal string identifier is equal to the
//  Err identifier.
//
// Returns:
//  TRUE        Name is equal to the Err string
//  FALSE       Name does not equal the Err string
//
//*************************************************************************

BOOL PascalEqErrStr
(
    LPBYTE      lpbyStr
)
{
    if (lpbyStr[0] == 3 &&
        (lpbyStr[1] == 'e' || lpbyStr[1] == 'E') &&
        (lpbyStr[2] == 'r' || lpbyStr[2] == 'R') &&
        (lpbyStr[3] == 'r' || lpbyStr[3] == 'R'))
        return(TRUE);

    return(FALSE);
}


//*************************************************************************
//
// BOOL PascalStrHasNonNumChars()
//
// Parameters:
//  lpbyStr         Ptr to Pascal string name
//
// Description:
//  Determines whether the string is not convertible to a number.
//
// Returns:
//  TRUE            If string is convertible to a long value
//  FALSE           Otheriwse
//
//*************************************************************************

BOOL PascalStrHasNonNumChars
(
    LPBYTE      lpbyStr
)
{
    int         nLen;
    int         i;

    nLen = *lpbyStr++;
    i = 0;

    // First skip leading spaces

    while (i < nLen)
    {
        if (lpbyStr[i] != ' ' && lpbyStr[i] != '\t')
            break;

        ++i;
    }

    if (i < nLen)
    {
        // Is it a negative number?

        if (lpbyStr[i] == '-')
            ++i;

        while (i < nLen)
        {
            if ((lpbyStr[i] < '0' || '9' < lpbyStr[i]) && lpbyStr[i] != ' ')
            {
                // Found a non-numeric character

                return(FALSE);
            }

            ++i;
        }

        return(TRUE);
    }

    return(FALSE);
}


//*************************************************************************
//
// long PascalStrToLong()
//
// Parameters:
//  lpbyStr         Ptr to Pascal string name
//
// Description:
//  Converts the Pascal string to a long value.
//
// Returns:
//  long            Value
//
//*************************************************************************

long PascalStrToLong
(
    LPBYTE      lpbyStr
)
{
    long        lValue;
    int         nLen;
    int         i;
    BOOL        bNegative;

    lValue = 0;

    nLen = *lpbyStr++;
    i = 0;

    // First skip leading spaces

    while (i < nLen)
    {
        if (lpbyStr[i] != ' ' && lpbyStr[i] != '\t')
            break;

        ++i;
    }

    if (i < nLen)
    {
        // Is it a negative number?

        if (lpbyStr[i] == '-')
        {
            ++i;
            bNegative = TRUE;
        }
        else
            bNegative = FALSE;

        while (i < nLen)
        {
            if (lpbyStr[i] < '0' || '9' < lpbyStr[i])
            {
                // Found a non-numeric character

                break;
            }

            lValue = lValue * 10 + (lpbyStr[i++] - '0');
        }

        if (bNegative == TRUE)
            lValue = -lValue;
    }

    return(lValue);
}


//*************************************************************************
//
// void LongToPascalStr()
//
// Parameters:
//  lValue          Value to convert
//  lpbyStr         Ptr to Pascal string buffer
//
// Description:
//  Converts the long value to a Pascal string.
//
// Returns:
//  Nothing
//
//*************************************************************************

void LongToPascalStr
(
    long        lValue,
    LPBYTE      lpbyStr
)
{
    if (lValue != 0)
    {
        int         nDigitCount;
        BOOL        bNegative;
        long        lTemp;

        if (lValue < 0)
        {
            bNegative = TRUE;
            lValue = -lValue;
        }
        else
            bNegative = FALSE;

        lTemp = lValue;

        // Count how many digits there are

        nDigitCount = 0;
        do
        {
            ++nDigitCount;
            lTemp /= 10;
        }
        while (lTemp != 0);

        // Store the length

        *lpbyStr++ = (BYTE)(nDigitCount + 1);

        // Store the sign

        if (bNegative == FALSE)
            *lpbyStr++ = (BYTE)' ';
        else
            *lpbyStr++ = (BYTE)'-';

        // Store the digits

        do
        {
            lpbyStr[--nDigitCount] = (BYTE)('0' + (lValue % 10));
            lValue /= 10;
        }
        while (lValue != 0);
    }
    else
    {
        // Store " 0"

        *lpbyStr++ = (BYTE)2;
        *lpbyStr++ = (BYTE)' ';
        *lpbyStr = (BYTE)'0';
    }
}


//*************************************************************************
//
// void PascalStrToLower()
//
// Parameters:
//  lpbySrc         Ptr to source string
//  lpbyDst         Ptr to destination string
//
// Description:
//  Converts a string to all lower case characters.
//  Destination and source can be the same.
//
// Returns:
//  Nothing
//
//*************************************************************************

void PascalStrToLower
(
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
)
{
    int         nLen;
    BYTE        bySrc;

    nLen = *lpbyDst++ = *lpbySrc++;

    while (nLen-- > 0)
    {
        bySrc = *lpbySrc++;
        if (bySrc <= 'Z' && bySrc >= 'A')
            *lpbyDst++ = bySrc + ('a' - 'A');
        else
            *lpbyDst++ = bySrc;
    }
}


//*************************************************************************
//
// void PascalStrToUpper()
//
// Parameters:
//  lpbySrc         Ptr to source string
//  lpbyDst         Ptr to destination string
//
// Description:
//  Converts a string to all upper case characters.
//  Destination and source can be the same.
//
// Returns:
//  Nothing
//
//*************************************************************************

void PascalStrToUpper
(
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
)
{
    int         nLen;
    BYTE        bySrc;

    nLen = *lpbyDst++ = *lpbySrc++;

    while (nLen-- > 0)
    {
        bySrc = *lpbySrc++;
        if ('a' <= bySrc && bySrc <= 'z')
            *lpbyDst++ = (bySrc - 'a') + 'A';
        else
            *lpbyDst++ = bySrc;
    }
}


//*************************************************************************
//
// void PascalStrLeft()
//
// Parameters:
//  lpbySrc         Ptr to source string
//  lCount          Number of leftmost bytes to get
//  lpbyDst         Ptr to destination string
//
// Description:
//  Places the leftmost lCount characters of the source string
//  into the destination string.
//
// Returns:
//  Nothing
//
//*************************************************************************

void PascalStrLeft
(
    LPBYTE      lpbySrc,
    long        lCount,
    LPBYTE      lpbyDst
)
{
    int         nLen;

    if (lCount < 0)
    {
        // Count must be at least 0

        nLen = 0;
    }
    else
    if (lCount > *lpbySrc)
    {
        // Count cannot be greater than length of source

        nLen = (int)*lpbySrc;
    }
    else
        nLen = (int)lCount;

    *lpbyDst++ = (BYTE)nLen;
    while (nLen-- > 0)
        *lpbyDst++ = *++lpbySrc;
}


//*************************************************************************
//
// void PascalStrRight()
//
// Parameters:
//  lpbySrc         Ptr to source string
//  lCount          Number of rightmost bytes to get
//  lpbyDst         Ptr to destination string
//
// Description:
//  Places the rightmost lCount characters of the source string
//  into the destination string.
//
// Returns:
//  Nothing
//
//*************************************************************************

void PascalStrRight
(
    LPBYTE      lpbySrc,
    long        lCount,
    LPBYTE      lpbyDst
)
{
    int         nLen;

    if (lCount < 0)
    {
        // Count must be at least 0

        nLen = 0;
    }
    else
    if (lCount > *lpbySrc)
    {
        // Count cannot be greater than length of source

        nLen = (int)*lpbySrc;
    }
    else
        nLen = (int)lCount;

    // Advance to character before start

    lpbySrc += *lpbySrc - nLen;

    *lpbyDst++ = (BYTE)nLen;
    while (nLen-- > 0)
        *lpbyDst++ = *++lpbySrc;
}


//*************************************************************************
//
// void PascalStrMid()
//
// Parameters:
//  lpbySrc         Ptr to source string
//  lStart          Starting location
//  lCount          Number of characters to get
//  lpbyDst         Ptr to destination string
//
// Description:
//  Places the lCount characters starting at lStart of the source string
//  into the destination string.
//
// Returns:
//  Nothing
//
//*************************************************************************

void PascalStrMid
(
    LPBYTE      lpbySrc,
    long        lStart,
    long        lCount,
    LPBYTE      lpbyDst
)
{
    int         nStart, nLen;

    // Limit start

    if (lStart < 1)
    {
        // Start must be at least 1

        nStart = 1;
    }
    else
    if (lStart > *lpbySrc)
    {
        // Start cannot be greater than length of source

        nStart = (int)*lpbySrc;
    }
    else
        nStart = (int)lStart;

    // Limit count

    if (lCount < 0)
        nLen = 0;
    else
    if (lCount > *lpbySrc)
        nLen = (int)*lpbySrc;
    else
        nLen = (int)lCount;

    // Limit copy length

    if (nStart + nLen > *lpbySrc + 1)
        nLen = (*lpbySrc + 1) - nStart;

    // Advance to start character

    lpbySrc += nStart;

    *lpbyDst++ = (BYTE)nLen;
    while (nLen-- > 0)
        *lpbyDst++ = *lpbySrc++;
}


//*************************************************************************
//
// void PascalStrLTrim()
//
// Parameters:
//  lpbySrc         Ptr to source string
//  lpbyDst         Ptr to destination string
//
// Description:
//  Copies the source to the destination except for leading spaces.
//
// Returns:
//  Nothing
//
//*************************************************************************

void PascalStrLTrim
(
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
)
{
    int         nLen;

    // Starting from the beginning, find the first non-space character

    nLen = *lpbySrc++;
    while (nLen > 0)
    {
        if (*lpbySrc != ' ')
            break;

        ++lpbySrc;
        --nLen;
    }

    // Copy the rest of the characters to the destination

    *lpbyDst++ = nLen;
    while (nLen-- > 0)
        *lpbyDst++ = *lpbySrc++;
}


//*************************************************************************
//
// void PascalStrRTrim()
//
// Parameters:
//  lpbySrc         Ptr to source string
//  lpbyDst         Ptr to destination string
//
// Description:
//  Copies the source to the destination except for trailing spaces.
//
// Returns:
//  Nothing
//
//*************************************************************************

void PascalStrRTrim
(
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
)
{
    int         nLen;

    // Starting from the end, find the first non-space character

    nLen = *lpbySrc;
    while (nLen > 0)
    {
        if (lpbySrc[nLen] != ' ')
            break;

        --nLen;
    }

    // Copy the rest of the characters to the destination

    *lpbyDst++ = nLen;
    while (nLen-- > 0)
        *lpbyDst++ = *++lpbySrc;
}


//*************************************************************************
//
// void PascalStrInStr()
//
// Parameters:
//  lpbySrc         Ptr to source string
//  lIndex          Index to start search
//  lpbySearch      Ptr to search string
//
// Description:
//  Returns the character position in the source string where the
//  search string begins.  If the source string does not contain
//  the search string, the function returns 0.
//
// Returns:
//  long            Position where the search string begins
//  0               If the source does not have the search string
//
//*************************************************************************

long PascalStrInStr
(
    LPBYTE      lpbySrc,
    long        lIndex,
    LPBYTE      lpbySearch
)
{
    int         i, j;

    // Return 0 result if search past the end or an empty string

    if (lIndex <= 0 || lIndex > lpbySrc[0] || lpbySearch[0] == 0)
        return(0);

    // Starting from the given position, search

    for (i=(BYTE)lIndex;i<=lpbySrc[0];i++)
    {
        if (lpbySrc[i] == lpbySearch[1])
        {
            // Compare the two strings

            for (j=2;j<=lpbySearch[0];j++)
            {
                if (i + j - 1 > lpbySrc[0])
                {
                    // Search string is longer

                    return(0);
                }

                if (lpbySrc[i+j-1] != lpbySearch[j])
                    break;
            }

            if (j > lpbySearch[0])
            {
                // It was a match

                return(i);
            }
        }
    }

    return(0);
}

