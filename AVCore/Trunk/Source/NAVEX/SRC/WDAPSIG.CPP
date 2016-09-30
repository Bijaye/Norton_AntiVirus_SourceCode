//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDAPSIG.CPv   1.4   08 Apr 1997 12:40:48   DCHI  $
//
// Description:
//  Signature application module.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDAPSIG.CPv  $
// 
//    Rev 1.4   08 Apr 1997 12:40:48   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.3   27 Mar 1997 16:32:44   DCHI
// Added VBA5 support to ExtraBufRead().
// 
//    Rev 1.2   13 Feb 1997 13:34:34   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.1   23 Jan 1997 11:11:58   DCHI
// Fixed global variable usage problem.
// 
//    Rev 1.0   17 Jan 1997 11:23:32   DCHI
// Initial revision.
// 
//************************************************************************

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"
#include "wdencrd.h"
#include "wdscan.h"
#include "wdsigcmd.h"
#include "wdapsig.h"
#include "wdsigutl.h"

//*************************************************************************
//
// WD_STATUS WDExtraBufRead()
//
// Parameters:
//  lpstScan            The scan structure with the buffer
//  lOffset             Offset in macro to read
//
// Description:
//  Reads into lpstScan->abyExtraBuf up to WD_EXTRA_BUF_SIZE bytes
//  beginning from lOffset.  The number of bytes read is the maximum
//  of WD_EXTRA_BUF_SIZE and the difference of lpstScan->lSize and
//  lOffset.  If lpstScan->byEncrypt is non-zero, then the function
//  also XORs each byte with the key.
//
//  The function returns error if an attempt is made to read before
//  the beginning of the macro body or after it or if the read fails.
//
// Returns:
//  WD_STATUS_ERROR     If a read failed
//  WD_STATUS_OK        If successful
//
//*************************************************************************

WD_STATUS WDExtraBufRead
(
    LPWD_SCAN           lpstScan,
    long                lOffset

)
{
    WORD                wBytesToRead;

    // Return error if attempt to read before start or after end

    if (lOffset < 0 || lOffset >= lpstScan->lSize)
        return(WD_STATUS_ERROR);

    if (WD_EXTRA_BUF_SIZE > lpstScan->lSize - lOffset)
        wBytesToRead = (WORD)(lpstScan->lSize - lOffset);
    else
        wBytesToRead = WD_EXTRA_BUF_SIZE;

    if (lpstScan->wScanType == WD_SCAN_TYPE_WD7)
    {
        if (WordDocEncryptedRead(lpstScan->lpstCallBack,
                                 lpstScan->lpstOLEStream,
                                 lpstScan->uScan.stWD7.lpstKey,
                                 (DWORD)(lpstScan->uScan.stWD7.lStartOffset +
                                     lOffset),
                                 lpstScan->abyExtraBuf,
                                 wBytesToRead) != wBytesToRead)
        {
            // Failed to read

            return(WD_STATUS_ERROR);
        }

        lpstScan->lExtraBufOffset = lOffset;
        lpstScan->nNumExtraBufBytes = wBytesToRead;

        // Decrypt if necessary

        if (lpstScan->uScan.stWD7.byEncrypt != 0)
        {
            LPBYTE  lpby = lpstScan->abyExtraBuf;
            BYTE    byEncrypt = lpstScan->uScan.stWD7.byEncrypt;

            while (wBytesToRead-- > 0)
                *lpby++ ^= byEncrypt;
        }
    }
    else
    {
        // VBA5

        if (LZNTGetBytes(lpstScan->uScan.stVBA5.lpstLZNT,
                         (DWORD)lOffset,
                         (int)wBytesToRead,
                         lpstScan->abyExtraBuf) != (int)wBytesToRead)
        {
            return(WD_STATUS_ERROR);
        }

        lpstScan->lExtraBufOffset = lOffset;
        lpstScan->nNumExtraBufBytes = wBytesToRead;
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDSigFuncHotByteSkip()
//
// Parameters:
//  lpstScan            The scan structure
//  lplCurOffset        Ptr to long var storing current macro offset
//  lpbHit              Ptr to BOOL var that is set to TRUE if the
//                          skip was successful, otherwise set to FALSE
//
// Description:
//  The new offset is the sum of the current offset, one, and the
//  value of the hot byte.  If the hot byte does not lie in any
//  of the current buffers, then if it is within bounds of the macro
//  body, it is read into the extra buffer and the new offset is
//  calculated appropriately.
//
// Returns:
//  WD_STATUS_ERROR     If the read failed
//  WD_STATUS_OK        If the read succeeded
//
//*************************************************************************

WD_STATUS WDSigFuncHotByteSkip
(
    LPWD_SCAN           lpstScan,
    LPLONG              lplCurOffset,
    LPBOOL              lpbHit
)
{
    long                lCurOffset = *lplCurOffset;

    // Is the byte in the running buffer

    if (lpstScan->lRunBufOffset <= lCurOffset &&
        lCurOffset < (lpstScan->lRunBufOffset + lpstScan->nNumRunBufBytes))
    {
        // The byte is in the running buffer

        *lplCurOffset += lpstScan->abyRunBuf[lCurOffset -
                                             lpstScan->lRunBufOffset] + 1;

        *lpbHit = TRUE;
        return(WD_STATUS_OK);
    }

    if (lCurOffset < 0 || lpstScan->lSize <= lCurOffset)
    {
        // Out of bounds

        *lpbHit = FALSE;
        return(WD_STATUS_OK);
    }

    // Read new data into the extra buffer
    //  if needed

    if (lCurOffset < lpstScan->lExtraBufOffset ||
        (lpstScan->lExtraBufOffset +
         lpstScan->nNumExtraBufBytes) < lCurOffset)
    {
        if (WDExtraBufRead(lpstScan,
                           lCurOffset) != WD_STATUS_OK)
        {
            // There was an error reading

            return(WD_STATUS_ERROR);
        }
    }

    // The byte is guaranteed to be in the
    //  extra buffer now

    *lplCurOffset += lpstScan->abyExtraBuf[lCurOffset -
                                           lpstScan->lExtraBufOffset] + 1;

    *lpbHit = TRUE;

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDSigMatch()
//
// Parameters:
//  lpstScan            The scan structure
//  lplCurOffset        Ptr to long var storing current macro offset
//  nStrLen             Length of string to match
//  lpabyString         Ptr to string to match
//  lpbHit              Ptr to BOOL var that is set to TRUE if the
//                          match was successful, otherwise set to FALSE
//
// Description:
//  Attempts to match the string at the current position.  If the match
//  is successful, *lplCurOffset is set to point to the offset after
//  the last byte of the matched string and *lpbHit is set to TRUE.
//
//  If the match is attempted such that any byte would match outside
//  the bounds of the stream, the function sets *lpbHit to FALSE.
//
//  The function returns WD_STATUS_ERROR only on a failure in reading
//  from the stream.
//
// Returns:
//  WD_STATUS_ERROR     If a read failed
//  WD_STATUS_OK        If successful
//
//*************************************************************************

WD_STATUS WDSigMatch
(
    LPWD_SCAN           lpstScan,
    LPLONG              lplCurOffset,
    int                 nStrLen,
    LPBYTE              lpabyString,
    LPBOOL              lpbHit
)
{
    long                lCurOffset = *lplCurOffset;
    long                lEndOffset = lCurOffset + nStrLen;
    int                 i;

    //////////////////////////////////////////////////////////////////
    // Comparison within run buffer
    //////////////////////////////////////////////////////////////////

    // See if the whole string lies within the run buffer

    if (lpstScan->lRunBufOffset <= lCurOffset &&
        lEndOffset <= (lpstScan->lRunBufOffset + lpstScan->nNumRunBufBytes))
    {
        i = (int)(lCurOffset - lpstScan->lRunBufOffset);
        while (lCurOffset < lEndOffset)
        {
            if ((BYTE)(lpstScan->abyRunBuf[i++] + 1) != *lpabyString++)
            {
                // No match

                *lpbHit = FALSE;
                return(WD_STATUS_OK);
            }

            ++lCurOffset;
        }

        // It was a match

        *lplCurOffset = lCurOffset;
        *lpbHit = TRUE;
        return(WD_STATUS_OK);
    }

    //////////////////////////////////////////////////////////////////
    // Comparison within extra buffer
    //////////////////////////////////////////////////////////////////

    // If the string lies outside the stream, then fail it

    if (lCurOffset < 0 || lpstScan->lSize < lEndOffset)
    {
        // Out of bounds

        *lpbHit = FALSE;
        return(WD_STATUS_OK);
    }

    // If any part of the string could lie outside the buffer,
    //  do a read

    if (lCurOffset < lpstScan->lExtraBufOffset ||
        (lpstScan->lExtraBufOffset +
         lpstScan->nNumExtraBufBytes) < lEndOffset)
    {
        if (WDExtraBufRead(lpstScan,
                           lCurOffset) != WD_STATUS_OK)
        {
            // There was an error reading

            return(WD_STATUS_ERROR);
        }
    }

    // Now do the comparison within the extra buffer

    i = (int)(lCurOffset - lpstScan->lExtraBufOffset);
    while (lCurOffset < lEndOffset)
    {
        if ((BYTE)(lpstScan->abyExtraBuf[i++] + 1) != *lpabyString++)
        {
            // No match

            *lpbHit = FALSE;
            return(WD_STATUS_OK);
        }

        ++lCurOffset;
    }

    // It was a match

    *lplCurOffset = lCurOffset;
    *lpbHit = TRUE;
    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDFuncScanSlide()
//
// Parameters:
//  nCommandNibble          Command nibble describing scan slide
//  lplCurOffset            Ptr to long var storing current macro offset
//  lpabyControlStream      Control stream
//  lpnControlStreamIndex   Ptr to current index in control stream
//  lpstScan                The scan structure
//  lpbHit                  Ptr to BOOL var that is set to TRUE if the
//                              scan slide was a hit, otherwise set to FALSE
//
// Description:
//
//  If B2 is 0, then it is just a scan slide.
//  If B2 is 1, then it is a scan mask.
//
//  If B1 is 0, then it is a single byte scan.
//  If B1 is 1, then it is a multi-byte scan.
//
//  If B0 is 0, then it is a BYTE window.
//  If B0 is 1, then it is a WORD window.
//
//  B3 B2 B1 B0
//  0  0  0  0
//  0  0  0  1
//  0  0  1  0
//  0  0  1  1
//  0  1  0  0
//  0  1  0  1
//  0  1  1  0
//  0  1  1  1
//
//  The function positions *lplCurOffset after the matched
//  byte upon success.
//
// Returns:
//  WD_STATUS_ERROR     If a read failed
//  WD_STATUS_OK        If successful
//
//*************************************************************************

WD_STATUS WDFuncScanSlide
(
    int                 nCommandNibble,
    LPLONG              lplCurOffset,
    LPBYTE              lpabyControlStream,
    LPINT               lpnControlStreamIndex,
    LPLPBYTE            lplpabyDataStream,
    LPWD_SCAN           lpstScan,
    LPBOOL              lpbHit
)
{
    long                lCurOffset;
    long                lWindowSize;
    BYTE                byMask;
    BYTE                byCand;
    int                 i, j;
    int                 nNumSingleBytes;
    int                 nNumRanges;
    BYTE                abySingle[MAX_SCAN_BYTES];
    BYTE                abyRange[MAX_SCAN_RANGES][2];
    LPBYTE              lpabySearch;
    int                 nBufLen;

    lCurOffset = *lplCurOffset;

    //////////////////////////////////////////////////////////////////
    // Determine the window
    //////////////////////////////////////////////////////////////////

    if ((nCommandNibble & 0x01) == 0)
    {
        // BYTE window size specifier

        lWindowSize = WDGetDataStreamBYTE(lplpabyDataStream) + 1;
    }
    else
    {
        // WORD window size specifier

        lWindowSize = WDGetDataStreamWORD(lplpabyDataStream) + 1;
    }


    //////////////////////////////////////////////////////////////////
    // Determine whether is a ScanSlide() or a ScanSlideMask()
    //////////////////////////////////////////////////////////////////

    if ((nCommandNibble & 0x04) == 0)
    {
        // ScanSlide()

        byMask = 0xFF;
    }
    else
    {
        // ScanSlideMask()

        byMask = WDGetDataStreamBYTE(lplpabyDataStream);
    }


    //////////////////////////////////////////////////////////////////
    // Determine whether it is a single byte or multi-byte scan
    //  and get the bytes and/or ranges to search for.
    //////////////////////////////////////////////////////////////////

    if ((nCommandNibble & 0x02) == 0)
    {
        // Single byte

        nNumSingleBytes = 1;
        nNumRanges = 0;

        abySingle[0] = WDGetDataStreamBYTE(lplpabyDataStream);
    }
    else
    {
        // Multi-byte/ranges

        int             nNibble;

        // Determine number of each

        nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                           lpnControlStreamIndex);

        switch (nNibble)
        {
            case NIB2_COMB_R0_B2:
            case NIB2_COMB_R0_B3:
            case NIB2_COMB_R0_B4:
            case NIB2_COMB_R0_B5:
                nNumRanges = 0;
                nNumSingleBytes = 2 + nNibble - NIB2_COMB_R0_B2;
                break;

            case NIB2_COMB_R1_B0:
            case NIB2_COMB_R1_B1:
            case NIB2_COMB_R1_B2:
            case NIB2_COMB_R1_B3:
            case NIB2_COMB_R1_B4:
            case NIB2_COMB_R1_B5:
                nNumRanges = 1;
                nNumSingleBytes = nNibble - NIB2_COMB_R1_B0;
                break;

            case NIB2_COMB_R2_B0:
            case NIB2_COMB_R2_B1:
            case NIB2_COMB_R2_B2:
            case NIB2_COMB_R2_B3:
            case NIB2_COMB_R2_B4:
            case NIB2_COMB_R2_B5:
                nNumRanges = 2;
                nNumSingleBytes = nNibble - NIB2_COMB_R2_B0;
                break;

            default:
                // This should never happen
                return(WD_STATUS_ERROR);
        }

        // Get single bytes

        for (i=0;i<nNumSingleBytes;i++)
            abySingle[i] = WDGetDataStreamBYTE(lplpabyDataStream);

        // Get byte ranges

        for (i=0;i<nNumRanges;i++)
        {
            abyRange[i][0] = WDGetDataStreamBYTE(lplpabyDataStream);
            abyRange[i][1] = WDGetDataStreamBYTE(lplpabyDataStream);
        }
    }

    //////////////////////////////////////////////////////////////////
    // Scan 
    //////////////////////////////////////////////////////////////////

    // See which buffer the current offset is in

    // See if the current offset lies within the run buffer

    if (lpstScan->lRunBufOffset <= lCurOffset &&
        lCurOffset < (lpstScan->lRunBufOffset + lpstScan->nNumRunBufBytes))
    {
        i = (int)(lCurOffset - lpstScan->lRunBufOffset);
        lpabySearch = lpstScan->abyRunBuf + i;
        nBufLen = lpstScan->nNumRunBufBytes - i;
    }
    else
    if (lpstScan->lExtraBufOffset <= lCurOffset &&
        lCurOffset < (lpstScan->lExtraBufOffset +
                      lpstScan->nNumExtraBufBytes))
    {
        i = (int)(lCurOffset - lpstScan->lExtraBufOffset);
        lpabySearch = lpstScan->abyExtraBuf + i;
        nBufLen = lpstScan->nNumExtraBufBytes - i;
    }
    else
    {
        // Going to have to do a read

        nBufLen = 0;
    }

    while (1)
    {
        for (i=0;i<nBufLen;i++)
        {
            if (lWindowSize-- == 0)
            {
                // No match

                *lpbHit = FALSE;
                return(WD_STATUS_OK);
            }

            byCand = *lpabySearch++ & byMask;

            // Search single bytes

            for (j=0;j<nNumSingleBytes;j++)
                if (byCand == abySingle[j])
                {
                    // It's a match

                    // Set the new position to after the matched byte

                    *lplCurOffset = lCurOffset + i + 1;
                    *lpbHit = TRUE;
                    return(WD_STATUS_OK);
                }

            // Search byte ranges

            for (j=0;j<nNumRanges;j++)
                if (abyRange[j][0] <= byCand && byCand <= abyRange[j][1])
                {
                    // It's a match

                    // Set the new position to after the matched byte

                    *lplCurOffset = lCurOffset + i + 1;
                    *lpbHit = TRUE;
                    return(WD_STATUS_OK);
                }
        }

        if (lWindowSize == 0)
        {
            // No match

            *lpbHit = FALSE;
            return(WD_STATUS_OK);
        }

        // Update current position

        lCurOffset += nBufLen;

        if (lCurOffset < 0 || lpstScan->lSize <= lCurOffset)
        {
            // Out of bounds

            *lpbHit = FALSE;
            return(WD_STATUS_OK);
        }

        // Read into the buffer

        if (WDExtraBufRead(lpstScan,
                           lCurOffset) != WD_STATUS_OK)
        {
            // There was an error reading

            return(WD_STATUS_ERROR);
        }

        lpabySearch = lpstScan->abyExtraBuf;
        nBufLen = lpstScan->nNumExtraBufBytes;
    }
}


//*************************************************************************
//
// WD_STATUS WDFuncScanString()
//
// Parameters:
//  nCommandNibble          Command nibble describing scan string
//  lplCurOffset            Ptr to long var storing current macro offset
//  lpabyControlStream      Control stream
//  lpnControlStreamIndex   Ptr to current index in control stream
//  lpstScan                The scan structure
//  lpbHit                  Ptr to BOOL var that is set to TRUE if the
//                              scan string was a hit, otherwise set to FALSE
//
// Description:
//
//  If B2 is 0, then it is just a ScanString().
//  If B2 is 1, then it is a ScanStringI().
//
//  If B1 is 0, then it is a show string
//  If B1 is 1, then it is a long string
//
//  If B0 is 0, then it is a BYTE window.
//  If B0 is 1, then it is a WORD window.
//
//  B3 B2 B1 B0
//  1  0  0  0
//  1  0  0  1
//  1  0  1  0
//  1  0  1  1
//  1  1  0  0
//  1  1  0  1
//  1  1  1  0
//  1  1  1  1
//
//  The function positions *lplCurOffset after the matched
//  string upon success.
//
// Returns:
//  WD_STATUS_ERROR     If a read failed
//  WD_STATUS_OK        If successful
//
//*************************************************************************

WD_STATUS WDFuncScanString
(
    int                 nCommandNibble,
    LPLONG              lplCurOffset,
    LPBYTE              lpabyControlStream,
    LPINT               lpnControlStreamIndex,
    LPLPBYTE            lplpabyDataStream,
    LPWD_SCAN           lpstScan,
    LPBOOL              lpbHit
)
{
    long                lCurOffset;
    long                lEndOffset;
    long                lWindowSize;
    BOOL                bCaseInsensitive;
    BYTE                byCand;
    int                 i, j;
    LPBYTE              lpabyString;
    int                 nStrLen;
    LPBYTE              lpabySearch;
    int                 nBufLen;

    lCurOffset = *lplCurOffset;

    //////////////////////////////////////////////////////////////////
    // Determine the window
    //////////////////////////////////////////////////////////////////

    if ((nCommandNibble & 0x01) == 0)
    {
        // BYTE window size specifier

        lWindowSize = WDGetDataStreamBYTE(lplpabyDataStream) + 1;
    }
    else
    {
        // WORD window size specifier

        lWindowSize = WDGetDataStreamWORD(lplpabyDataStream) + 1;
    }

    //////////////////////////////////////////////////////////////////
    // Determine whether it is a ScanString() or a ScanStringI()
    //////////////////////////////////////////////////////////////////

    if ((nCommandNibble & 0x04) == 0)
    {
        // ScanString()

        bCaseInsensitive = FALSE;
    }
    else
    {
        // ScanStringI()

        bCaseInsensitive = TRUE;
    }


    //////////////////////////////////////////////////////////////////
    // Determine whether it is a single byte or multi-byte scan
    //  and get the bytes and/or ranges to search for.
    //////////////////////////////////////////////////////////////////

    if ((nCommandNibble & 0x02) == 0)
    {
        int nNibble;

		// Short string

        // The next control nibble determines how long it is

        nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                           lpnControlStreamIndex);

        nStrLen = 2 + nNibble - NIB2_SHORT_STR_2;
    }
    else
    {
        nStrLen = WDGetDataStreamBYTE(lplpabyDataStream);
    }

    //////////////////////////////////////////////////////////////////
    // Scan
    //////////////////////////////////////////////////////////////////

    // See which buffer the current offset is in

    // See if the current offset lies within the run buffer

    lEndOffset = lCurOffset + nStrLen;
    if (lpstScan->lRunBufOffset <= lCurOffset &&
        lEndOffset <= (lpstScan->lRunBufOffset + lpstScan->nNumRunBufBytes))
    {
        i = (int)(lCurOffset - lpstScan->lRunBufOffset);
        lpabySearch = lpstScan->abyRunBuf + i;
        nBufLen = lpstScan->nNumRunBufBytes - i - nStrLen + 1;
    }
    else
    if (lpstScan->lExtraBufOffset <= lCurOffset &&
        lEndOffset <= (lpstScan->lExtraBufOffset +
                      lpstScan->nNumExtraBufBytes))
    {
        i = (int)(lCurOffset - lpstScan->lExtraBufOffset);
        lpabySearch = lpstScan->abyExtraBuf + i;
        nBufLen = lpstScan->nNumExtraBufBytes - i - nStrLen + 1;
    }
    else
    {
        // Going to have to do a read

        nBufLen = 0;
    }

    lpabyString = *lplpabyDataStream;
    while (1)
    {
        for (i=0;i<nBufLen;i++)
        {
            if (lWindowSize-- == 0)
            {
                // No match

                *lpbHit = FALSE;
                return(WD_STATUS_OK);
            }

            for (j=0;j<nStrLen;j++)
            {
                byCand = lpabySearch[j];

                if (bCaseInsensitive)
                {
                    // Convert to uppercase

                    if ('a' <= byCand && byCand <= 'z')
                        byCand = byCand - 'a' + 'A';
                }

                if ((BYTE)(byCand + 1) != lpabyString[j])
                    break;
            }

            if (j == nStrLen)
            {
                // It's a match

                // Set the new position to after the matched string

                *lplCurOffset = lCurOffset + i + nStrLen;
                *lpbHit = TRUE;

                // Set the data stream position after the matched string

                *lplpabyDataStream += nStrLen;

                return(WD_STATUS_OK);
            }

            ++lpabySearch;
        }

        if (lWindowSize == 0)
        {
            // No match

            *lpbHit = FALSE;
            return(WD_STATUS_OK);
        }

        // Update current position

        lCurOffset += nBufLen;
        lEndOffset = lCurOffset + nStrLen;

        if (lpstScan->lSize <= lEndOffset)
        {
            // Out of bounds

            *lpbHit = FALSE;
            return(WD_STATUS_OK);
        }

        // Read into the buffer

        if (WDExtraBufRead(lpstScan,
                           lCurOffset) != WD_STATUS_OK)
        {
            // There was an error reading

            return(WD_STATUS_ERROR);
        }

        lpabySearch = lpstScan->abyExtraBuf;
        nBufLen = lpstScan->nNumExtraBufBytes - nStrLen + 1;
    }
}


//*************************************************************************
//
// WD_STATUS WDBoolFuncSkip()
//
// Parameters:
//  nBoolNibble             The boolean command nibble to look for
//  lpabyControlStream      Ptr to the control stream
//  lpnControlStreamIndex   Ptr to index of current control nibble
//  lplpabyDataStream       Ptr to ptr to current data byte
//
// Description:
//  Parses through the control and data streams of a signature
//  starting from *lpnControlStreamIndex looking for nBoolNibble.
//
//  *lpnControlStreamIndex and *lplpabyDataStream are updated to point
//  to the given control nibble.
//
// Returns:
//  WD_STATUS_ERROR     If the nibble could not be found or a parse error
//  WD_STATUS_OK        If successful
//
//*************************************************************************

WD_STATUS WDBoolFuncSkip
(
    int                 nBoolNibble,
    LPBYTE              lpabyControlStream,
    LPINT               lpnControlStreamIndex,
    LPLPBYTE            lplpabyDataStream
)
{
    int                 nCommand, nNibble;

    while (*lpnControlStreamIndex < 512)
    {
        nCommand = WDGetControlStreamNibble(lpabyControlStream,
                                            lpnControlStreamIndex);

        switch (nCommand)
        {
            case NIB0_SEEK:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   lpnControlStreamIndex);

                switch (nNibble)
                {
                    case NIB1_SEEK_SET_0:
                    case NIB1_SEEK_SET_1:
                    case NIB1_SEEK_SET_2:
                    case NIB1_SEEK_SET_3:
                    case NIB1_SEEK_SET_4:
                    case NIB1_SEEK_SET_5:
                    case NIB1_SEEK_SET_6:
                        break;

                    case NIB1_SEEK_SET_BYTE:
                    case NIB1_SEEK_CUR_BYTE:
                    case NIB1_SEEK_END_BYTE:
                        *lplpabyDataStream += 1;
                        break;

                    case NIB1_SEEK_SET_WORD:
                    case NIB1_SEEK_CUR_WORD:
                    case NIB1_SEEK_END_WORD:
                        *lplpabyDataStream += 2;
                        break;

                    case NIB1_SEEK_SET_DWORD:
                    case NIB1_SEEK_CUR_DWORD:
                    case NIB1_SEEK_END_DWORD:
                        *lplpabyDataStream += 4;
                        break;

                    default:
                        return(WD_STATUS_ERROR);
                }

                break;
            }

            case NIB0_SEEK_CUR_POS:
            case NIB0_SEEK_CUR_NEG:
            {
                WDGetControlStreamNibble(lpabyControlStream,
                                         lpnControlStreamIndex);
                break;
            }

            case NIB0_SCAN:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   lpnControlStreamIndex);

                switch (nNibble)
                {
                    case NIB1_SCAN_SLIDE_BYTE_SINGLE:
                    case NIB1_SCAN_SLIDE_WORD_SINGLE:
                    case NIB1_SCAN_SLIDE_BYTE_COMB:
                    case NIB1_SCAN_SLIDE_WORD_COMB:
                    case NIB1_SCAN_MASK_BYTE_SINGLE:
                    case NIB1_SCAN_MASK_WORD_SINGLE:
                    case NIB1_SCAN_MASK_BYTE_COMB:
                    case NIB1_SCAN_MASK_WORD_COMB:
                        // Window size

                        if (nNibble & 0x01)
                            *lplpabyDataStream += 2;
                        else
                            *lplpabyDataStream += 1;

                        // ScanSlide() or ScanSlideMask()

                        if (nNibble & 0x04)
                            *lplpabyDataStream += 1;

                        // Single byte or multi-byte

                        if (nNibble & 0x02)
                        {
                            // Multi-byte

                            int nNumRanges, nNumSingleBytes;

                            nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                               lpnControlStreamIndex);

                            switch (nNibble)
                            {
                                case NIB2_COMB_R0_B2:
                                case NIB2_COMB_R0_B3:
                                case NIB2_COMB_R0_B4:
                                case NIB2_COMB_R0_B5:
                                    nNumRanges = 0;
                                    nNumSingleBytes = 2 + nNibble - NIB2_COMB_R0_B2;
                                    break;

                                case NIB2_COMB_R1_B0:
                                case NIB2_COMB_R1_B1:
                                case NIB2_COMB_R1_B2:
                                case NIB2_COMB_R1_B3:
                                case NIB2_COMB_R1_B4:
                                case NIB2_COMB_R1_B5:
                                    nNumRanges = 1;
                                    nNumSingleBytes = nNibble - NIB2_COMB_R1_B0;
                                    break;

                                case NIB2_COMB_R2_B0:
                                case NIB2_COMB_R2_B1:
                                case NIB2_COMB_R2_B2:
                                case NIB2_COMB_R2_B3:
                                case NIB2_COMB_R2_B4:
                                case NIB2_COMB_R2_B5:
                                    nNumRanges = 2;
                                    nNumSingleBytes = nNibble - NIB2_COMB_R2_B0;
                                    break;

                                default:
                                    // This should never happen
                                    return(WD_STATUS_ERROR);
                            }

                            // Skip single bytes and byte ranges

                            *lplpabyDataStream += nNumSingleBytes +
                                2 * nNumRanges;
                        }
                        else
                        {
                            // Single byte

                            *lplpabyDataStream += 1;
                        }

                        break;

                    case NIB1_SCAN_STRING_BYTE_S:
                    case NIB1_SCAN_STRING_WORD_S:
                    case NIB1_SCAN_STRING_BYTE_L:
                    case NIB1_SCAN_STRING_WORD_L:
                    case NIB1_SCAN_STRINGI_BYTE_S:
                    case NIB1_SCAN_STRINGI_WORD_S:
                    case NIB1_SCAN_STRINGI_BYTE_L:
                    case NIB1_SCAN_STRINGI_WORD_L:
                        // Window size

                        if (nNibble & 0x01)
                            *lplpabyDataStream += 2;
                        else
                            *lplpabyDataStream += 1;

                        // ScanString() or ScanStringI()

                        if (nNibble & 0x04)
                            *lplpabyDataStream += 1;

                        // Short or long string

                        if (nNibble & 0x02)
                        {
                            // Long string

                            *lplpabyDataStream += 1 + **lplpabyDataStream;
                        }
                        else
                        {
                            // Short string

                            *lplpabyDataStream += 2 -  NIB2_SHORT_STR_2 +
                                WDGetControlStreamNibble(lpabyControlStream,
                                                         lpnControlStreamIndex);
                        }

                        break;

                    default:
                        // This should never happen
                        return(WD_STATUS_ERROR);
                }

                break;
            }

            case NIB0_SIZE:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   lpnControlStreamIndex);

                switch (nNibble)
                {
                    case NIB1_SIZE_BOUND_BYTE:
                    case NIB1_SIZE_EQUAL_WORD:
                    case NIB1_SIZE_LESS_THAN_WORD:
                    case NIB1_SIZE_GREATER_THAN_WORD:
                        *lplpabyDataStream += 2;
                        break;

                    case NIB1_SIZE_BOUND_WORD:
                        *lplpabyDataStream += 4;
                        break;

                    case NIB1_SIZE_EQUAL_BYTE:
                    case NIB1_SIZE_LESS_THAN_BYTE:
                    case NIB1_SIZE_GREATER_THAN_BYTE:
                        *lplpabyDataStream += 1;
                        break;

                    default:
                        // This should never happen
                        return(WD_STATUS_ERROR);
                }

                break;
            }

            case NIB0_MISC:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   lpnControlStreamIndex);

                switch (nNibble)
                {
                    case NIB1_CUSTOM_SCAN_BYTE:
                    case NIB1_NAME_SIG_BYTE:
                        *lplpabyDataStream += 1;
                        break;

                    case NIB1_CUSTOM_SCAN_WORD:
                    case NIB1_NAME_SIG_WORD:
                        *lplpabyDataStream += 2;
                        break;


                    case NIB1_HOT_BYTE_SKIP:
                    case NIB1_AT_END:
                    case NIB1_NOT:
                    case NIB1_BEG_OR:
                        break;

                    case NIB1_END_NOT:
                        if (nBoolNibble == NIB1_END_NOT)
                        {
                            *lpnControlStreamIndex -= 2;
                            return(WD_STATUS_OK);
                        }
                        break;

                    case NIB1_OR:
                        if (nBoolNibble == NIB1_OR)
                        {
                            *lpnControlStreamIndex -= 2;
                            return(WD_STATUS_OK);
                        }
                        break;

                    case NIB1_END_OR:
                        if (nBoolNibble == NIB1_OR ||
                            nBoolNibble == NIB1_END_OR)
                        {
                            *lpnControlStreamIndex -= 2;
                            return(WD_STATUS_OK);
                        }
                        break;

                    default:
                        // This should never happen
                        return(WD_STATUS_ERROR);
                }

                break;
            }

            case NIB0_RESERVED:
                // This should never happen
                break;

            case NIB0_END:
                return(WD_STATUS_ERROR);

            case NIB0_MATCH_1:
            case NIB0_MATCH_2:
            case NIB0_MATCH_3:
            case NIB0_MATCH_4:
            case NIB0_MATCH_5:
            case NIB0_MATCH_6:
            case NIB0_MATCH_7:
            case NIB0_MATCH_N:
                if (nCommand == NIB0_MATCH_N)
                    *lplpabyDataStream += 1 + **lplpabyDataStream;
                else
                    *lplpabyDataStream += nCommand + 1 - NIB0_MATCH_1;
                break;

            default:
                // This should never happen
                return(WD_STATUS_ERROR);
        }
    }

    // Couldn't find what we were looking for

    return(WD_STATUS_ERROR);
}


//*************************************************************************
//
// WD_STATUS WDApplySig()
//
// Parameters:
//  lpabySig                Ptr to signature to apply
//  lpstSigSet              Ptr to signature set
//  lpstScan                The scan structure
//  lpbHit                  Ptr to BOOL var that is set to TRUE if the
//                              signature was a hit, otherwise set to FALSE
//
// Description:
//
// Returns:
//  WD_STATUS_ERROR     If a read failed
//  WD_STATUS_OK        If successful
//
//*************************************************************************

#define WD_SIG_COMPONENT_HIT    0x0001
#define WD_SIG_IN_NOT           0x0002
#define WD_SIG_IN_OR            0x0004
#define WD_SIG_NOT_IN_OR        0x0008
#define WD_SIG_OR_IN_NOT        0x0010
#define WD_SIG_NOT_IS_TRUE      0x0020  // A Not() expression is assumed
                                        //  to be FALSE before evaluation.
                                        //  This bit is set to one when
                                        //  a literal in the Not()
                                        //  expression evaluates to FALSE.
#define WD_SIG_OR_IS_TRUE       0x0040  // An Or() expression is assumed
                                        //  to be FALSE before evaluation.
                                        //  This bit is set to one when
                                        //  a term in the Or() expression
                                        //  evaluates to TRUE.
#define WD_SIG_OR_TERM_IS_TRUE  0x0080  // Each term in an Or() expression
                                        //  is assumed to be TRUE before
                                        //  evaluation.  This bit is reset
                                        //  to zero when a literal in the
                                        //  term evaluates to FALSE.

WD_STATUS WDApplySig
(
    LPBYTE              lpabySig,
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan,
    LPBOOL              lpbHit
)
{
    int                 nControlStreamLen;
    int                 nCommand;
    int                 nNibble;
    int                 nControlStreamIndex;
    LPBYTE              lpabyControlStream;
    LPBYTE              lpabyDataStream;
    long                lCurOffset;
    long                lNotOffset;
    long                lBegOrOffset;
    BOOL                bHit;
    WORD                wSigStatus;
    WORD                wID;
    WD_STATUS           wdStatus;

    *lpbHit = FALSE;

    nControlStreamLen = lpabySig[0];
    lpabyControlStream = lpabySig + 1;
    lpabyDataStream = lpabyControlStream + nControlStreamLen + 1;

    // Multiply by two to get number of control nibbles

    nControlStreamLen *= 2;

    lCurOffset = lpstScan->lRunBufOffset + lpstScan->nRunBufStepOffset;

    wSigStatus = 0;

    nControlStreamIndex = 0;
    while (nControlStreamIndex < nControlStreamLen)
    {
        nCommand = WDGetControlStreamNibble(lpabyControlStream,
                                            &nControlStreamIndex);

        // Assume the component is a hit

        wSigStatus |= WD_SIG_COMPONENT_HIT;

        switch (nCommand)
        {
            case NIB0_SEEK:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   &nControlStreamIndex);

                switch (nNibble)
                {
                    case NIB1_SEEK_SET_0:
                    case NIB1_SEEK_SET_1:
                    case NIB1_SEEK_SET_2:
                    case NIB1_SEEK_SET_3:
                    case NIB1_SEEK_SET_4:
                    case NIB1_SEEK_SET_5:
                    case NIB1_SEEK_SET_6:
                        lCurOffset = nNibble - NIB1_SEEK_SET_0;
                        break;

                    case NIB1_SEEK_SET_BYTE:
                        lCurOffset = WDGetDataStreamBYTE(&lpabyDataStream);
                        break;

                    case NIB1_SEEK_SET_WORD:
                        lCurOffset = WDGetDataStreamWORD(&lpabyDataStream);
                        break;

                    case NIB1_SEEK_SET_DWORD:
                        lCurOffset = WDGetDataStreamDWORD(&lpabyDataStream);
                        break;

                    case NIB1_SEEK_CUR_BYTE:
                        lCurOffset += WDGetDataStreamSBYTE(&lpabyDataStream);
                        break;

                    case NIB1_SEEK_CUR_WORD:
                        lCurOffset += WDGetDataStreamSWORD(&lpabyDataStream);
                        break;

                    case NIB1_SEEK_CUR_DWORD:
                        lCurOffset += WDGetDataStreamSDWORD(&lpabyDataStream);
                        break;

                    case NIB1_SEEK_END_BYTE:
                        lCurOffset = lpstScan->lSize -
                            WDGetDataStreamBYTE(&lpabyDataStream);
                        break;

                    case NIB1_SEEK_END_WORD:
                        lCurOffset = lpstScan->lSize -
                            WDGetDataStreamWORD(&lpabyDataStream);
                        break;

                    case NIB1_SEEK_END_DWORD:
                        lCurOffset = lpstScan->lSize -
                            WDGetDataStreamDWORD(&lpabyDataStream);
                        break;

                    default:
                        // This should never happen
                        return(WD_STATUS_ERROR);
                }

                break;
            }

            case NIB0_SEEK_CUR_POS:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   &nControlStreamIndex);

                lCurOffset += nNibble + 1 - NIB1_SEEK_CUR_1;
                break;
            }

            case NIB0_SEEK_CUR_NEG:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   &nControlStreamIndex);

                lCurOffset -= nNibble + 1 - NIB1_SEEK_CUR_N1;
                break;
            }


            case NIB0_SCAN:
            {
                WD_STATUS   wdStatus;

                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   &nControlStreamIndex);

                switch (nNibble)
                {
                    case NIB1_SCAN_SLIDE_BYTE_SINGLE:
                    case NIB1_SCAN_SLIDE_WORD_SINGLE:
                    case NIB1_SCAN_SLIDE_BYTE_COMB:
                    case NIB1_SCAN_SLIDE_WORD_COMB:
                    case NIB1_SCAN_MASK_BYTE_SINGLE:
                    case NIB1_SCAN_MASK_WORD_SINGLE:
                    case NIB1_SCAN_MASK_BYTE_COMB:
                    case NIB1_SCAN_MASK_WORD_COMB:
                        wdStatus = WDFuncScanSlide(nNibble,
                                                   &lCurOffset,
                                                   lpabyControlStream,
                                                   &nControlStreamIndex,
                                                   &lpabyDataStream,
                                                   lpstScan,
                                                   &bHit);
                        break;

                    case NIB1_SCAN_STRING_BYTE_S:
                    case NIB1_SCAN_STRING_WORD_S:
                    case NIB1_SCAN_STRING_BYTE_L:
                    case NIB1_SCAN_STRING_WORD_L:
                    case NIB1_SCAN_STRINGI_BYTE_S:
                    case NIB1_SCAN_STRINGI_WORD_S:
                    case NIB1_SCAN_STRINGI_BYTE_L:
                    case NIB1_SCAN_STRINGI_WORD_L:
                        wdStatus = WDFuncScanString(nNibble,
                                                    &lCurOffset,
                                                    lpabyControlStream,
                                                    &nControlStreamIndex,
                                                    &lpabyDataStream,
                                                    lpstScan,
                                                    &bHit);
                        break;

                    default:
                        // This should never happen
                        return(WD_STATUS_ERROR);
                }

                if (wdStatus == WD_STATUS_ERROR)
                {
                    // Error reading

                    return(WD_STATUS_ERROR);
                }

                if (bHit == FALSE)
                {
                    // No match

                    wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                }

                break;
            }

            case NIB0_SIZE:
            {
                long        lLowerBound;
                long        lUpperBound;

                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   &nControlStreamIndex);

                switch (nNibble)
                {
                    case NIB1_SIZE_BOUND_BYTE:
                        lLowerBound =
                            WDGetDataStreamBYTE(&lpabyDataStream);

                        lUpperBound =
                            WDGetDataStreamBYTE(&lpabyDataStream);

                        break;

                    case NIB1_SIZE_BOUND_WORD:
                        lLowerBound =
                            WDGetDataStreamWORD(&lpabyDataStream);

                        lUpperBound =
                            WDGetDataStreamWORD(&lpabyDataStream);

                        break;

                    case NIB1_SIZE_EQUAL_BYTE:
                        lLowerBound = lUpperBound =
                            WDGetDataStreamBYTE(&lpabyDataStream);

                        break;

                    case NIB1_SIZE_EQUAL_WORD:
                        lLowerBound = lUpperBound =
                            WDGetDataStreamWORD(&lpabyDataStream);

                        break;

                    case NIB1_SIZE_LESS_THAN_BYTE:
                        lLowerBound = -1;
                        lUpperBound =
                            WDGetDataStreamBYTE(&lpabyDataStream);

                        break;

                    case NIB1_SIZE_LESS_THAN_WORD:
                        lLowerBound = -1;
                        lUpperBound =
                            WDGetDataStreamWORD(&lpabyDataStream);

                        break;

                    case NIB1_SIZE_GREATER_THAN_BYTE:
                        lLowerBound =
                            WDGetDataStreamBYTE(&lpabyDataStream) + 1;
                        lUpperBound = 0x7FFFFFFFl;

                        break;

                    case NIB1_SIZE_GREATER_THAN_WORD:
                        lLowerBound =
                            WDGetDataStreamWORD(&lpabyDataStream) + 1;
                        lUpperBound = 0x7FFFFFFFl;

                        break;

                    default:
                        // This should never happen
                        return(WD_STATUS_ERROR);
                }

                // Now check the bounds

                if (lpstScan->lSize < lLowerBound ||
                    lpstScan->lSize > lUpperBound)
                {
                    // Size() failed

                    wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                }

                break;
            }

            case NIB0_MISC:
            {
                nNibble = WDGetControlStreamNibble(lpabyControlStream,
                                                   &nControlStreamIndex);

                switch (nNibble)
                {
                    case NIB1_CUSTOM_SCAN_BYTE:
                    {
                        wID = WDGetDataStreamBYTE(&lpabyDataStream);
                        wdStatus = lpstSigSet->lpapfCustomScan[wID]
                            (lpstSigSet,lpstScan);

                        if (wdStatus == WD_STATUS_ERROR)
                            return(WD_STATUS_ERROR);

                        if (wdStatus == WD_STATUS_OK)
                        {
                            // No hit

                            wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                        }

                        break;
                    }

                    case NIB1_CUSTOM_SCAN_WORD:
                    {
                        wID = WDGetDataStreamWORD(&lpabyDataStream);
                        wdStatus = lpstSigSet->lpapfCustomScan[wID]
                            (lpstSigSet,lpstScan);

                        if (wdStatus == WD_STATUS_ERROR)
                            return(WD_STATUS_ERROR);

                        if (wdStatus == WD_STATUS_OK)
                        {
                            // No hit

                            wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                        }

                        break;
                    }

                    case NIB1_NAME_SIG_BYTE:
                    {
                        wID = WDGetDataStreamBYTE(&lpabyDataStream);
                        if ((lpstScan->lpabyNameSigIntraMacroHit[wID >> 3] &
                             (1 << (wID & 0x07))) == 0)
                        {
                            // No hit on intra macro name sig

                            wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                        }
                        break;
                    }

                    case NIB1_NAME_SIG_WORD:
                    {
                        wID = WDGetDataStreamWORD(&lpabyDataStream);
                        if ((lpstScan->lpabyNameSigIntraMacroHit[wID >> 3] &
                             (1 << (wID & 0x07))) == 0)
                        {
                            // No hit on intra macro name sig

                            wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                        }
                        break;
                    }

                    case NIB1_HOT_BYTE_SKIP:
                    {
                        if (WDSigFuncHotByteSkip(lpstScan,
                                                 &lCurOffset,
                                                 &bHit) != WD_STATUS_OK)
                        {
                            // Error reading

                            return(WD_STATUS_ERROR);
                        }

                        if (bHit == FALSE)
                        {
                            // Out of bounds

                            wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                        }

                        // Successful HotByteSkip()

                        break;
                    }

                    case NIB1_AT_END:
                        if (lCurOffset != lpstScan->lSize)
                        {
                            // Not at the end

                            wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                        }
                        break;

                    case NIB1_NOT:
                        lNotOffset = lCurOffset;
                        wSigStatus |= WD_SIG_IN_NOT;
                        if (wSigStatus & WD_SIG_IN_OR)
                            wSigStatus |= WD_SIG_NOT_IN_OR;
                        break;

                    case NIB1_END_NOT:
                        lCurOffset = lNotOffset;
                        if ((wSigStatus & WD_SIG_NOT_IS_TRUE) == 0)
                        {
                            // Not() expression already false

                            wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                        }

                        wSigStatus &= ~(WD_SIG_IN_NOT |
                                        WD_SIG_NOT_IN_OR |
                                        WD_SIG_NOT_IS_TRUE);
                        break;

                    case NIB1_BEG_OR:
                        lBegOrOffset = lCurOffset;

                        // Assume entire OR is FALSE and
                        //  each OR term is TRUE

                        wSigStatus |= WD_SIG_IN_OR |
                                      WD_SIG_OR_TERM_IS_TRUE;

                        if (wSigStatus & WD_SIG_IN_NOT)
                            wSigStatus |= WD_SIG_OR_IN_NOT;
                        break;

                    case NIB1_OR:
                        if (wSigStatus & WD_SIG_OR_TERM_IS_TRUE)
                        {
                            // Skip to the EndOr()

                            if (WDBoolFuncSkip(NIB1_END_OR,
                                               lpabyControlStream,
                                               &nControlStreamIndex,
                                               &lpabyDataStream) ==
                                WD_STATUS_ERROR)
                            {
                                return(WD_STATUS_ERROR);
                            }

                            wSigStatus |= WD_SIG_OR_IS_TRUE;
                        }
                        else
                            lCurOffset = lBegOrOffset;

                        wSigStatus |= WD_SIG_OR_TERM_IS_TRUE;
                        break;

                    case NIB1_END_OR:
                        if (wSigStatus & WD_SIG_OR_TERM_IS_TRUE)
                            wSigStatus |= WD_SIG_OR_IS_TRUE;

                        if ((wSigStatus & WD_SIG_OR_IS_TRUE) == 0)
                        {
                            // Or() expression is false

                            wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                        }

                        wSigStatus &= ~(WD_SIG_IN_OR |
                                        WD_SIG_OR_IN_NOT |
                                        WD_SIG_OR_IS_TRUE |
                                        WD_SIG_OR_TERM_IS_TRUE);

                        break;

                    default:
                        // This should never happen
                        return(WD_STATUS_ERROR);
                }

                break;
            }

            case NIB0_RESERVED:
                // This should never happen
                break;

            case NIB0_END:
                // NOP
                break;

            case NIB0_MATCH_1:
            case NIB0_MATCH_2:
            case NIB0_MATCH_3:
            case NIB0_MATCH_4:
            case NIB0_MATCH_5:
            case NIB0_MATCH_6:
            case NIB0_MATCH_7:
            case NIB0_MATCH_N:
            {
                int nStrLen;

                // Get the length of the string to match

                if (nCommand == NIB0_MATCH_N)
                    nStrLen = *lpabyDataStream++;
                else
                    nStrLen = nCommand + 1 - NIB0_MATCH_1;

                // Try it

                if (WDSigMatch(lpstScan,
                               &lCurOffset,
                               nStrLen,
                               lpabyDataStream,
                               &bHit) == WD_STATUS_ERROR)
                {
                    // Error reading

                    return(WD_STATUS_ERROR);
                }

                if (bHit == FALSE)
                {
                    // String did not match

                    wSigStatus &= ~WD_SIG_COMPONENT_HIT;
                }

                // The string matched

                lpabyDataStream += nStrLen;
                break;
            }

            default:
                // This should never happen
                return(WD_STATUS_ERROR);
        }

        // Take special action only if it wasn't a hit

        if ((wSigStatus & WD_SIG_COMPONENT_HIT) == 0)
        {
            if (wSigStatus & WD_SIG_NOT_IN_OR)
            {
                // A literal within a Not() expression in an Or()
                //  expression is FALSE (D or E is FALSE)
                //    BegOr() AB Or() C Not() DE EndNot() EndOr()

                wSigStatus |= WD_SIG_NOT_IS_TRUE;

                // Skip to the EndNot()

                nNibble = NIB1_END_NOT;
            }
            else
            if (wSigStatus & WD_SIG_OR_IN_NOT)
            {
                // A literal within an Or() expression in a Not()
                //  expression is FALSE (C or D or E or F is FALSE)
                //    Not() AB BegOr() CD Or() EF EndOr() EndNot()

                wSigStatus &= ~WD_SIG_OR_TERM_IS_TRUE;

                // Skip to the Or()

                nNibble = NIB1_OR;
            }
            else
            if (wSigStatus & WD_SIG_IN_NOT)
            {
                // A literal within a top-level Not() expression
                //  is FALSE (A or B is FALSE)
                //    Not() AB EndNot()

                wSigStatus |= WD_SIG_NOT_IS_TRUE;

                // Skip to the EndNot()

                nNibble = NIB1_END_NOT;
            }
            else
            if (wSigStatus & WD_SIG_IN_OR)
            {
                // A literal within a top-level Or() expression
                //  is FALSE (A or B or C or D is FALSE)
                //    BegOr() AB Or() CD EndOr()

                wSigStatus &= ~WD_SIG_OR_TERM_IS_TRUE;

                // Skip to the Or()

                nNibble = NIB1_OR;
            }
            else
            {
                // A top-level literal is FALSE

                return(WD_STATUS_OK);
            }

            if (WDBoolFuncSkip(nNibble,
                               lpabyControlStream,
                               &nControlStreamIndex,
                               &lpabyDataStream) == WD_STATUS_ERROR)
            {
                return(WD_STATUS_ERROR);
            }
        }
    }

    *lpbHit = TRUE;

    return(WD_STATUS_OK);
}




