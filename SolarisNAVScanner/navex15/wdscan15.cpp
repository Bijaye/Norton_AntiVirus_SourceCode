//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wdscan15.cpv   1.10   15 Dec 1998 12:16:38   DCHI  $
//
// Description:
//  Top-level scan module.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wdscan15.cpv  $
// 
//    Rev 1.10   15 Dec 1998 12:16:38   DCHI
// P97 changes.
// 
//    Rev 1.9   09 Nov 1998 13:58:44   DCHI
// Added #pragma data_seg()'s for global FAR data for SYM_WIN16.
// 
//    Rev 1.8   12 Oct 1998 13:44:28   DCHI
// Added NLM relinquish control calls to WDApplyMacroSigs()
// and WDApplyVirusSignatures().
// 
//    Rev 1.7   09 Sep 1998 12:45:34   DCHI
// Changes for password-protected Excel 97 document repair.
// 
//    Rev 1.6   04 Aug 1998 13:52:36   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.5   07 Jul 1998 12:04:14   DCHI
// Added correction for setup of run buffer for A2 scanning.
// 
//    Rev 1.4   10 Jun 1998 13:23:54   DCHI
// Changed to use XL5ModuleCRCFast() for XL5 CRC calculation.
// 
//    Rev 1.3   11 May 1998 18:15:38   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.2   15 Apr 1998 17:24:44   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.1   10 Mar 1998 13:41:12   DCHI
// Modified to use new O97 CRC.
// 
//    Rev 1.0   09 Jul 1997 16:17:20   DCHI
// Initial revision.
// 
//************************************************************************

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"
#include "crc32.h"
#include "wdscan15.h"
#include "wdsigcmd.h"
#include "wdapsig.h"
#include "wdapvsig.h"
#include "mvp.h"

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

WD_VIRUS_SIG_INFO_T FAR gstMVPVirusSigInfo =
{
    VID_MVP,
    NULL
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

WORD WDSigGetWORD
(
    LPBYTE              lpabyWORD
)
{
    return (lpabyWORD[0] | (lpabyWORD[1] << 8));
}

DWORD WDSigGetDWORD
(
    LPBYTE              lpabyDWORD
)
{
    return (((DWORD)lpabyDWORD[0]) | (((DWORD)lpabyDWORD[1]) << 8) |
            (((DWORD)lpabyDWORD[2]) << 16) |
            (((DWORD)lpabyDWORD[2]) << 24));
}


//*************************************************************************
//
// BOOL WDAllocScanStruct()
//
// Parameters:
//  lpvRootCookie   Root cookie for memory allocation
//  lplpstScan      Ptr to ptr to store ptr to allocate structure
//  nRunBufSize     Size for run buffer
//  nExtraBufSize   Size for extra buffer
//
// Description:
//  Allocates memory for the scan structure and the run buffers.
//
// Returns:
//  WD_STATUS_OK    If the allocation succeeded
//  WD_STATUS_ERROR If the allocation failed
//
//*************************************************************************

WD_STATUS WDAllocScanStruct
(
    LPVOID          lpvRootCookie,
    LPLPWD_SCAN     lplpstScan,
    int             nRunBufSize,
    int             nExtraBufSize
)
{
    DWORD           dwNumBytes;
    LPWD_SCAN       lpstScan;

    // Calculate memory requirements

    if (nRunBufSize <= 0)
        nRunBufSize = DEF_WD_RUN_BUF_SIZE;

    if (nExtraBufSize <= 0)
        nExtraBufSize = DEF_WD_EXTRA_BUF_SIZE;

    dwNumBytes = sizeof(WD_SCAN_T) + nRunBufSize + nExtraBufSize;

    if (SSMemoryAlloc(lpvRootCookie,
                      dwNumBytes,
                      (LPLPVOID)&lpstScan) != SS_STATUS_OK)
    {
        return(WD_STATUS_ERROR);
    }

    // Initialize structure

    lpstScan->nRunBufSize = nRunBufSize;
    lpstScan->abyRunBuf = (LPBYTE)(lpstScan+1);

    lpstScan->nExtraBufSize = nExtraBufSize;
    lpstScan->abyExtraBuf = (LPBYTE)(lpstScan+1) + nRunBufSize;

    *lplpstScan = lpstScan;

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// BOOL WDFreeScanStruct()
//
// Parameters:
//  lpvRootCookie   Root cookie for memory allocation
//  lpstScan        Ptr to scan structure to free
//
// Description:
//  Frees memory allocated for the given scan structure.
//
// Returns:
//  WD_STATUS_OK    If the free succeeded
//  WD_STATUS_ERROR If the free failed
//
//*************************************************************************

WD_STATUS WDFreeScanStruct
(
    LPVOID          lpvRootCookie,
    LPWD_SCAN       lpstScan
)
{
    if (SSMemoryFree(lpvRootCookie,
                     lpstScan) != SS_STATUS_OK)
        return(WD_STATUS_ERROR);

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// BOOL WDIsFullSetHit()
//
// Parameters:
//  lFullSetLen     Length of RLE full set array
//  lpabyFullSet    Full set array
//  lIdx            Index of hit to check
//
// Description:
//  Iterates through the full set array to determine whether a hit
//  on the given index is a hit in the full set.
//
// Returns:
//  TRUE        The hit is part of the full set
//  FALSE       The hit is not part of the full set
//
//*************************************************************************

BOOL WDIsFullSetHit
(
    long            lFullSetLen,
    LPBYTE          lpabyFullSet,
    long            lIdx
)
{
    long            l, lCurIdx;
    BYTE            byOctet;
    int             nIndex;

    nIndex = 8;
    lCurIdx = 0;
    for (l=0;l<lFullSetLen;l++)
    {
        if (nIndex == 8)
        {
            byOctet = lpabyFullSet[l];
            nIndex = 0;
        }
        else
        {
            // Is the index within the range of the current chunk?

            if (lCurIdx <= lIdx && lIdx < (lCurIdx + lpabyFullSet[l]))
            {
                // Found the chunk, so check to see whether the chunk
                //  is part of the full set or not

                if ((byOctet & (1 << nIndex)) == 0)
                {
                    // It is not part of the full set

                    return(FALSE);
                }

                // It is part of the full set

                return(TRUE);
            }

            lCurIdx += lpabyFullSet[l];
            ++nIndex;
        }
    }

    // We should never reach this point

    return(FALSE);
}


//*************************************************************************
//
// WD_STATUS WDGetSigSetOffset()
//
// Parameters:
//  lpabySig            Signature
//  lpdwEndOffset
//
// Description:
//  lpabySig points to the beginning of the signature and is assumed
//  to begin with a SeekSet() function unless it is the end signature,
//  indicated by a zero control stream length.  If the signature is
//  an end signature, then the function returns WD_STATUS_ERROR.
//
//  The first byte of the stream is the control stream length.  The
//  second byte begins the control stream.
//
//  SeekSet() is a two nibble function, so the second control nibble
//  is what tells us how to determine the seek set offset.
//
// Returns:
//  WD_STATUS_ERROR     On error in the signature
//  WD_STATUS_OK        On success
//
//*************************************************************************

WD_STATUS WDGetSigSetOffset
(
    LPBYTE              lpabySig,
    LPDWORD             lpdwSetOffset
)
{
    WORD                wControlStreamLen;

    wControlStreamLen = lpabySig[0];
    if (wControlStreamLen == 0x00)
    {
        // End signature

        return(WD_STATUS_ERROR);
    }

    // Now look at the second nibble of the control stream

    switch ((lpabySig[1] >> 4) & 0x0F)
    {
        case NIB1_SEEK_SET_0:
            *lpdwSetOffset = 0;
            break;
        case NIB1_SEEK_SET_1:
            *lpdwSetOffset = 1;
            break;
        case NIB1_SEEK_SET_2:
            *lpdwSetOffset = 2;
            break;
        case NIB1_SEEK_SET_3:
            *lpdwSetOffset = 3;
            break;
        case NIB1_SEEK_SET_4:
            *lpdwSetOffset = 4;
            break;
        case NIB1_SEEK_SET_5:
            *lpdwSetOffset = 5;
            break;
        case NIB1_SEEK_SET_6:
            *lpdwSetOffset = 6;
            break;
        case NIB1_SEEK_SET_BYTE:
            *lpdwSetOffset = lpabySig[wControlStreamLen+2];
            break;
        case NIB1_SEEK_SET_WORD:
            *lpdwSetOffset = WDSigGetWORD(lpabySig+wControlStreamLen+2);
            break;
        case NIB1_SEEK_SET_DWORD:
            *lpdwSetOffset = WDSigGetDWORD(lpabySig+wControlStreamLen+2);
            break;

        default:
            // This should never happen
            return(WD_STATUS_ERROR);
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDGetSigEndOffset()
//
// Parameters:
//  lpabySig            Signature
//  lpdwEndOffset
//
// Description:
//  lpabySig points to the beginning of the signature and is assumed
//  to begin with a SeekEnd() function unless it is the end signature,
//  indicated by a zero control stream length.  If the signature is
//  an end signature, then the function returns WD_STATUS_ERROR.
//
//  The first byte of the stream is the control stream length.  The
//  second byte begins the control stream.
//
//  SeekEnd() is a two nibble function, so the second control nibble
//  is what tells us how to determine the seek end offset.
//
// Returns:
//  WD_STATUS_ERROR     On error in the signature
//  WD_STATUS_OK        On success
//
//*************************************************************************

WD_STATUS WDGetSigEndOffset
(
    LPBYTE              lpabySig,
    LPDWORD             lpdwEndOffset
)
{
    WORD                wControlStreamLen;

    wControlStreamLen = lpabySig[0];
    if (wControlStreamLen == 0x00)
    {
        // End signature

        return(WD_STATUS_ERROR);
    }

    // Now look at the second nibble of the control stream

    switch ((lpabySig[1] >> 4) & 0x0F)
    {
        case NIB1_SEEK_END_BYTE:
            *lpdwEndOffset = lpabySig[wControlStreamLen+2];
            break;
        case NIB1_SEEK_END_WORD:
            *lpdwEndOffset = WDSigGetWORD(lpabySig+wControlStreamLen+2);
            break;
        case NIB1_SEEK_END_DWORD:
            *lpdwEndOffset = WDSigGetDWORD(lpabySig+wControlStreamLen+2);
            break;

        default:
            // This should never happen
            return(WD_STATUS_ERROR);
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDApplyNameSigs()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  Applies the name sigs by performing a scan from the beginning
//  of the name to the end of the name.  The SeekSet()
//  and SeekEnd() sigs are applied in order as the scan advances.
//
//  The name should be zero-terminated and already placed into
//  lpstScan->abyName.
//
// Returns:
//  WD_STATUS_OK            Always
//
//*************************************************************************

WD_STATUS WDApplyNameSigs
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    int                 i;
    int                 nHitArrayByteSize;
    int                 nNameSize;
    DWORD               dwNextSigSetOffset;
    DWORD               dwNextSigEndOffset;
    WORD                wCurSetOffsetSigID;
    LPBYTE              lpabyCurSetOffsetSig;
    WORD                wCurEndOffsetSigID;
    LPBYTE              lpabyCurEndOffsetSig;
    BOOL                bHit;
    WORD                wIndex;
    LPBYTE              lpabyRawSig;
    LPWD_SIG_INFO       lpstSigInfo;
    WORD                wCurRawID;

    //////////////////////////////////////////////////////////////////
    // Name signature scan initialization
    //////////////////////////////////////////////////////////////////

    // Calculate the length of the name and copy it to the run buffer

    nNameSize = 0;
    lpabyRawSig = lpstScan->abyName;
    while (*lpabyRawSig)
        lpstScan->abyRunBuf[nNameSize++] = *lpabyRawSig++;

    // Initialize scan structure

    if (lpstScan->wScanType == WD_SCAN_TYPE_WD7)
    {
        lpstScan->uScan.stWD7.lStartOffset = 0;
        lpstScan->uScan.stWD7.byEncrypt = 0;
    }

    lpstScan->lSize = lpstScan->nNumRunBufBytes = nNameSize;
    lpstScan->lRunBufOffset = 0;

    lpstScan->lExtraBufOffset = 0xFFFFFFFF;
    lpstScan->nNumExtraBufBytes = 0;

    // Determine the next sig seek set offsets

    wCurSetOffsetSigID = 0;
    lpabyCurSetOffsetSig = lpstSigSet->lpabyNameSetOffsetSigs;
    if (WDGetSigSetOffset(lpabyCurSetOffsetSig,
                          &dwNextSigSetOffset) != WD_STATUS_OK)
    {
        dwNextSigSetOffset = 0xFFFFFFFF;
    }

    // Determine the next sig seek end offsets
    //  Find the first one for which the resulting absolute offset
    //  is greater than zero

    wCurEndOffsetSigID = (int)(lpstSigSet->lNumNameEndOffsetSigs);
    lpabyCurEndOffsetSig = lpstSigSet->lpabyNameEndOffsetSigs;
    while (1)
    {
        if (WDGetSigEndOffset(lpabyCurEndOffsetSig,
                              &dwNextSigEndOffset) != WD_STATUS_OK)
        {
            dwNextSigEndOffset = 0xFFFFFFFF;
            break;
        }
        else
        if (dwNextSigEndOffset <= nNameSize)
        {
            // Found one

            dwNextSigEndOffset = nNameSize - dwNextSigEndOffset;
            break;
        }

        // Skip over the control stream of the current sig

        lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

        // Skip over the data stream of the current sig

        lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

        // Increment the ID

        ++wCurEndOffsetSigID;
    }

    // Initialize the intra-macro name sig hit array

    nHitArrayByteSize = (int)(lpstSigSet->lNumNameSigs >> 3);
    if (lpstSigSet->lNumNameSigs & 0x07)
        ++nHitArrayByteSize;

    for (i=0;i<nHitArrayByteSize;i++)
        lpstScan->lpabyNameSigIntraMacroHit[i] = 0;


    //////////////////////////////////////////////////////////////////
    // Perform name signature scan
    //////////////////////////////////////////////////////////////////

    for (i=0;i<nNameSize;i++)
    {
        // Initialize run buffer offset of current step

        lpstScan->nRunBufStepOffset = i;

        //////////////////////////////////////////////////////////////
        // Apply SeekSet() sigs
        //////////////////////////////////////////////////////////////

        while (dwNextSigSetOffset == i)
        {
            // Apply the sig

            if (WDApplySig(lpabyCurSetOffsetSig,
                           lpstSigSet,
                           lpstScan,
                           &bHit) == WD_STATUS_OK)
            {
                if (bHit == TRUE)
                {
                    lpstScan->
                        lpabyNameSigInterMacroHit[wCurSetOffsetSigID>>3] |=
                            (1 << (wCurSetOffsetSigID & 0x7));

                    lpstScan->
                        lpabyNameSigIntraMacroHit[wCurSetOffsetSigID>>3] |=
                            (1 << (wCurSetOffsetSigID & 0x7));

                }
            }

            // Skip over the control stream of the current sig

            lpabyCurSetOffsetSig += lpabyCurSetOffsetSig[0] + 1;

            // Skip over the data stream of the current sig

            lpabyCurSetOffsetSig += lpabyCurSetOffsetSig[0] + 1;

            // Get the next sig set offset

            if (WDGetSigSetOffset(lpabyCurSetOffsetSig,
                                  &dwNextSigSetOffset) != WD_STATUS_OK)
            {
                dwNextSigSetOffset = 0xFFFFFFFF;
            }
            else
                ++wCurSetOffsetSigID;
        }

        //////////////////////////////////////////////////////////////
        // Apply SeekEnd() sigs
        //////////////////////////////////////////////////////////////

        while (dwNextSigEndOffset == i)
        {
            // Apply the sig

            if (WDApplySig(lpabyCurEndOffsetSig,
                           lpstSigSet,
                           lpstScan,
                           &bHit) == WD_STATUS_OK)
            {
                if (bHit == TRUE)
                {
                    lpstScan->
                        lpabyNameSigInterMacroHit[wCurEndOffsetSigID>>3] |=
                            (1 << (wCurEndOffsetSigID & 0x7));

                    lpstScan->
                        lpabyNameSigIntraMacroHit[wCurEndOffsetSigID>>3] |=
                            (1 << (wCurEndOffsetSigID & 0x7));

                }
            }

            // Skip over the control stream of the current sig

            lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

            // Skip over the data stream of the current sig

            lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

            // Get the next sig end offset

            if (WDGetSigEndOffset(lpabyCurEndOffsetSig,
                                  &dwNextSigEndOffset) != WD_STATUS_OK)
            {
                dwNextSigEndOffset = 0xFFFFFFFF;
            }
            else
            {
                dwNextSigEndOffset = nNameSize - dwNextSigEndOffset;
                ++wCurEndOffsetSigID;
            }
        }

        //////////////////////////////////////////////////////////////
        // Apply raw sigs
        //////////////////////////////////////////////////////////////

        wIndex = lpstSigSet->
            lpabyNameSigRawByteHit[lpstScan->abyRunBuf[i]];

        if (wIndex != 0)
        {
            lpstSigInfo = lpstSigSet->lpastRawNameSigIndex + wIndex - 1;

            // Apply the sigs

            wCurRawID = lpstSigInfo->wBaseID;
            lpabyRawSig = lpstSigInfo->lpabySigs;

            // A zero length control stream means there are no more
            //  sigs starting with the current raw byte

            while (lpabyRawSig[0] != 0)
            {
                if (WDApplySig(lpabyRawSig,
                               lpstSigSet,
                               lpstScan,
                               &bHit) == WD_STATUS_OK)
                {
                    if (bHit == TRUE)
                    {
                        lpstScan->
                            lpabyNameSigInterMacroHit[wCurRawID>>3] |=
                                (1 << (wCurRawID & 0x7));

                        lpstScan->
                            lpabyNameSigIntraMacroHit[wCurRawID>>3] |=
                                (1 << (wCurRawID & 0x7));

                    }
                }

                // Skip past the control stream

                lpabyRawSig += lpabyRawSig[0] + 1;

                // Skip past the data stream

                lpabyRawSig += lpabyRawSig[0] + 1;

                // Increment the ID

                ++wCurRawID;
            }
        }
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDRunBufRead()
//
// Parameters:
//  lpstScan                Ptr to scan structure
//  lOffset                 Offset to read from
//
// Description:
//  Reads into lpstScan->abyRunBuf up to lpstScan->nRunBufSize bytes
//  beginning from lOffset.  The number of bytes read is the maximum
//  of lpstScan->nRunBufSize and the difference of lpstScan->lSize and
//  lOffset.  If lpstScan->byEncrypt is non-zero, then the function
//  also XORs each byte with the key.
//
// Returns:
//  WD_STATUS_ERROR         On read error
//  WD_STATUS_OK            On successful read
//
//*************************************************************************

WD_STATUS WDRunBufRead
(
    LPWD_SCAN           lpstScan,
    long                lOffset
)
{
    WORD                wBytesToRead;
    DWORD               dwByteCount;

    if ((long)lpstScan->nRunBufSize > lpstScan->lSize - lOffset)
        wBytesToRead = (WORD)(lpstScan->lSize - lOffset);
    else
        wBytesToRead = (WORD)lpstScan->nRunBufSize;

    switch (lpstScan->wScanType)
    {
        case WD_SCAN_TYPE_WD7:
        {
            if (WD7EncryptedRead(lpstScan->lpstStream,
                                 lpstScan->uScan.stWD7.lpstMacroScan->lpstKey,
                                 (DWORD)(lpstScan->uScan.stWD7.lStartOffset +
                                     lOffset),
                                 lpstScan->abyRunBuf,
                                 wBytesToRead) != wBytesToRead)
                return(WD_STATUS_ERROR);

            // Decrypt if necessary

            if (lpstScan->uScan.stWD7.byEncrypt != 0)
            {
                LPBYTE  lpby = lpstScan->abyRunBuf;
                BYTE    byEncrypt = lpstScan->uScan.stWD7.byEncrypt;
                WORD    w = wBytesToRead;

                while (w-- > 0)
                    *lpby++ ^= byEncrypt;
            }
            break;
        }
        case WD_SCAN_TYPE_XL5_DIR:
        {
            if (SSSeekRead(lpstScan->lpstStream,
                           lpstScan->uScan.stXL5.stDirInfo.
                               dwStringOffset + lOffset,
                           lpstScan->abyRunBuf,
                           wBytesToRead,
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != wBytesToRead)
                return(WD_STATUS_ERROR);

            break;
        }
        case WD_SCAN_TYPE_XL5:
        {
            if (SSSeekRead(lpstScan->lpstStream,
                           lpstScan->uScan.stXL5.dwMacroOffset + lOffset,
                           lpstScan->abyRunBuf,
                           wBytesToRead,
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != wBytesToRead)
                return(WD_STATUS_ERROR);

            break;
        }
        case WD_SCAN_TYPE_VBA5:
        {
            if (LZNTGetBytes(lpstScan->uScan.stVBA5.lpstLZNT,
                             (DWORD)lOffset,
                             (int)wBytesToRead,
                             lpstScan->abyRunBuf) != (int)wBytesToRead)
                return(WD_STATUS_ERROR);

            break;
        }
        case WD_SCAN_TYPE_A2:
        {
            if (A2ObjAccessRead(&lpstScan->uScan.lpstA2->stObj,
                                (DWORD)lOffset,
                                lpstScan->abyRunBuf,
                                wBytesToRead,
                                &dwByteCount) == FALSE ||
                dwByteCount != wBytesToRead)
                return(WD_STATUS_ERROR);

            break;
        }
        case WD_SCAN_TYPE_A97_MACRO:
        {
            if (A97ObjAccessRead(&lpstScan->uScan.lpstA97->stObj,
                                 lpstScan->uScan.lpstA97->dwStartOffset +
                                     (DWORD)lOffset,
                                 lpstScan->abyRunBuf,
                                 wBytesToRead,
                                 &dwByteCount) == FALSE)
                return(WD_STATUS_ERROR);

            // Assumption is that the read is always going forward
            //  sequentially

            if (dwByteCount < wBytesToRead)
            {
                lpstScan->lSize -= (wBytesToRead - dwByteCount);
                wBytesToRead = (WORD)dwByteCount;
            }

            break;
        }
        case WD_SCAN_TYPE_A97_VBMOD:
        {
            if (A97LZNTGetBytes(&lpstScan->uScan.lpstA97->stLZNT,
                                (DWORD)lOffset,
                                (int)wBytesToRead,
                                lpstScan->abyRunBuf) != (int)wBytesToRead)
                return(WD_STATUS_ERROR);

            break;
        }
        case WD_SCAN_TYPE_A97_RAW:
        {
            if (A97ObjAccessRead(&lpstScan->uScan.lpstA97->stObj,
                                 (DWORD)lOffset,
                                 lpstScan->abyRunBuf,
                                 wBytesToRead,
                                 &dwByteCount) == FALSE ||
                dwByteCount != wBytesToRead)
                return(WD_STATUS_ERROR);

            break;
        }
        default:
            break;
    }

    lpstScan->lRunBufOffset = lOffset;
    lpstScan->nNumRunBufBytes = wBytesToRead;
    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS VBA5SkipAttributeWhiteSpace()
//
// Parameters:
//  lpstScan                Ptr to scan structure
//  lpdwNewOffset           Offset after attribute lines and white space
//
// Description:
//  Scans the compressed VBA stream and skips over all lines at the
//  beginning of the stream as long as the lines begin with
//  "Attribute VB_" or white space (i.e., spaces and tabs).
//  Lines are assumed to end with 0x0D 0x0A.
//
// Returns:
//  WD_STATUS_OK            On success
//  WD_STATUS_ERROR         On error reading
//
//*************************************************************************

WD_STATUS VBA5SkipAttributeWhiteSpace
(
    LPWD_SCAN           lpstScan,
    LPDWORD             lpdwNewOffset
)
{
static LPSTR   lpszAttributeVB = "Attribute VB_";
#define VBA_ATTR_STR_LEN    (sizeof("Attribute VB_") - 1)

#define STATE_BEGIN_LINE        0
#define STATE_MATCH_ATTRIBUTE   1
#define STATE_FIND_CR_LF        2
#define STATE_SKIP_WHITE_SPACE  3

    int                 i, j;
    int                 nNumBytes, nMinBytes;
    int                 nState;
    long                l;

    // Parse through beginning lines to skip over lines
    //  beginning with "Attribute VB_" and lines that consist
    //  of only white space

    nNumBytes = lpstScan->nRunBufSize;
    l = 0;
    i = lpstScan->nRunBufSize;
    nMinBytes = 1;
    nState = STATE_BEGIN_LINE;
    while (l < lpstScan->lSize)
    {
        // Check to see if the buffer needs to be refilled

        if (i >= nNumBytes ||
            (nMinBytes > 0 && (nNumBytes - i) < nMinBytes))
        {
            // Need to refill the buffer

            if (lpstScan->lSize - l < (long)lpstScan->nRunBufSize)
                nNumBytes = (int)(lpstScan->lSize - l);

            // Check to see if there are enough bytes

            if (nNumBytes < nMinBytes)
                return(WD_STATUS_OK);

            // Fill the buffer

            if (LZNTGetBytes(lpstScan->uScan.stVBA5.lpstLZNT,
                             (DWORD)l,
                             nNumBytes,
                             lpstScan->abyRunBuf) != nNumBytes)
            {
                return(WD_STATUS_ERROR);
            }

            // Start at the beginning of the buffer

            i = 0;
        }

        switch (nState)
        {
            case STATE_BEGIN_LINE:
            {
                // Take note of the offset of the beginning of the line

                *lpdwNewOffset = l;

                // Determine whether the line is an attribute line,
                //  a white space line, or other

                switch (lpstScan->abyRunBuf[i])
                {
                    case 'A':
                    {
                        // Possible attribute line

                        nMinBytes = VBA_ATTR_STR_LEN;
                        nState = STATE_MATCH_ATTRIBUTE;
                        break;
                    }

                    case ' ':
                    case '\t':
                        nMinBytes = 1;
                        nState = STATE_SKIP_WHITE_SPACE;
                        ++i;
                        ++l;
                        break;

                    case 0x0D:
                        nMinBytes = 2;
                        nState = STATE_FIND_CR_LF;
                        break;

                    default:
                        return(WD_STATUS_OK);
                }

                break;
            }

            case STATE_MATCH_ATTRIBUTE:
            {
                // Match "Attribute VB_"

                for (j=0;j<VBA_ATTR_STR_LEN;j++)
                {
                    if (lpstScan->abyRunBuf[i++] != lpszAttributeVB[j])
                        break;
                }

                if (j < VBA_ATTR_STR_LEN)
                {
                    // The line is not an attribute line

                    return(WD_STATUS_OK);
                }

                l += VBA_ATTR_STR_LEN;

                // Now search for CR/LF

                nMinBytes = 2;
                nState = STATE_FIND_CR_LF;
                break;
            }

            case STATE_FIND_CR_LF:
            {
                while (i < (nNumBytes - 1))
                {
                    if (lpstScan->abyRunBuf[i] == 0x0D &&
                        lpstScan->abyRunBuf[i+1] == 0x0A)
                    {
                        nMinBytes = 1;
                        nState = STATE_BEGIN_LINE;
                        i += 2;
                        l += 2;
                        break;
                    }
                    else
                    {
                        ++i;
                        ++l;
                    }
                }
                break;
            }

            case STATE_SKIP_WHITE_SPACE:
            {
                while (i < nNumBytes)
                {
                    switch (lpstScan->abyRunBuf[i])
                    {
                        case ' ':
                        case '\t':
                            ++i;
                            ++l;
                            break;

                        case 0x0D:
                            nMinBytes = 2;
                            nState = STATE_FIND_CR_LF;
                            break;

                        default:
                            return(WD_STATUS_OK);
                    }

                    if (nState == STATE_FIND_CR_LF)
                        break;
                }
                break;
            }

            default:
                // This should never happen

                return(WD_STATUS_ERROR);
        }
    }

    *lpdwNewOffset = 0xFFFFFFFF;

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDApplyMacroSigs()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  Applies the macro sigs by performing a scan from the beginning
//  of the macro body to the end of the macro body.  The SeekSet()
//  and SeekEnd() sigs are applied in order as the scan advances.
//
// Returns:
//  WD_STATUS_OK            Always
//
//*************************************************************************

WD_STATUS WDApplyMacroSigs
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    long                l;
    int                 i;
    DWORD               dwNextSigSetOffset;
    DWORD               dwNextSigEndOffset;
    WORD                wCurSetOffsetSigID;
    LPBYTE              lpabyCurSetOffsetSig;
    WORD                wCurEndOffsetSigID;
    LPBYTE              lpabyCurEndOffsetSig;
    BOOL                bHit;
    WORD                wIndex;
    LPBYTE              lpabyRawSig;
    LPWD_SIG_INFO       lpstSigInfo;
    WORD                wCurRawID;
    BYTE                byCurByte;
    O97_CRC_STATE_T     stCRCState;
    O97_CRC_STATE_T     stMVPCRCState;

    //////////////////////////////////////////////////////////////////
    // Macro signature scan initialization
    //////////////////////////////////////////////////////////////////

    // Initialize scan structure

    switch (lpstScan->wScanType)
    {
        case WD_SCAN_TYPE_WD7:
        {
            lpstScan->uScan.stWD7.lStartOffset =
                (long)lpstScan->uScan.stWD7.dwMacroOffset;
            lpstScan->lSize =
                (long)lpstScan->uScan.stWD7.dwMacroSize;
            lpstScan->uScan.stWD7.byEncrypt =
                lpstScan->uScan.stWD7.byMacroEncryptByte;
            break;
        }
        case WD_SCAN_TYPE_XL5_DIR:
        {
            lpstScan->lSize = lpstScan->uScan.stXL5.stDirInfo.dwStringLen;
            break;
        }
        case WD_SCAN_TYPE_XL5:
        {
            lpstScan->lSize = lpstScan->uScan.stXL5.dwMacroSize;
            break;
        }
        case WD_SCAN_TYPE_VBA5:
        {
            lpstScan->lSize = lpstScan->uScan.stVBA5.lpstLZNT->dwSize;
            break;
        }
        case WD_SCAN_TYPE_A2:
        {
            lpstScan->lSize = lpstScan->uScan.lpstA2->stObj.dwSize;
            break;
        }
        case WD_SCAN_TYPE_A97_MACRO:
        {
            lpstScan->lSize = lpstScan->uScan.lpstA97->stObj.dwSize -
                lpstScan->uScan.lpstA97->dwStartOffset;
            break;
        }
        case WD_SCAN_TYPE_A97_VBMOD:
        {
            lpstScan->lSize = lpstScan->uScan.lpstA97->stLZNT.dwSize;
            break;
        }
        case WD_SCAN_TYPE_A97_RAW:
        {
            lpstScan->lSize = lpstScan->uScan.lpstA97->stObj.dwSize;
            break;
        }
        default:
            break;
    }

    lpstScan->nNumRunBufBytes = 0;
    lpstScan->lRunBufOffset = 0;

    lpstScan->lExtraBufOffset = 0xFFFFFFFF;
    lpstScan->nNumExtraBufBytes = 0;

    // Initialize CRC

    switch (lpstScan->wScanType)
    {
        case WD_SCAN_TYPE_VBA5:
        case WD_SCAN_TYPE_A97_VBMOD:
        {
            O97ModuleSourceCRCInit(&stMVPCRCState);
            O97ModuleSourceCRCInit(&stCRCState);
            break;
        }
        default:
        {
            CRC32Init(stCRCState.dwCRC);
            break;
        }
    }

    // Determine the next sig seek set offsets

    wCurSetOffsetSigID = 0;
    lpabyCurSetOffsetSig = lpstSigSet->lpabyMacroSetOffsetSigs;
    if (WDGetSigSetOffset(lpabyCurSetOffsetSig,
                          &dwNextSigSetOffset) != WD_STATUS_OK)
    {
        dwNextSigSetOffset = 0xFFFFFFFF;
    }

    // Determine the next sig seek end offsets
    //  Find the first one for which the resulting absolute offset
    //  is greater than zero

    wCurEndOffsetSigID = (WORD)(lpstSigSet->lNumMacroEndOffsetSigs);
    lpabyCurEndOffsetSig = lpstSigSet->lpabyMacroEndOffsetSigs;
    while (1)
    {
        if (WDGetSigEndOffset(lpabyCurEndOffsetSig,
                              &dwNextSigEndOffset) != WD_STATUS_OK)
        {
            dwNextSigEndOffset = 0xFFFFFFFF;
            break;
        }
        else
        if (dwNextSigEndOffset <= lpstScan->lSize)
        {
            // Found one

            dwNextSigEndOffset = lpstScan->lSize - dwNextSigEndOffset;
            break;
        }

        // Skip over the control stream of the current sig

        lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

        // Skip over the data stream of the current sig

        lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

        // Increment the ID

        ++wCurEndOffsetSigID;
    }


    //////////////////////////////////////////////////////////////////
    // Perform macro signature scan
    //////////////////////////////////////////////////////////////////

    i = 0;
    for (l=0;l<lpstScan->lSize;l++)
    {
#ifdef SYM_NLM
        if ((l & 0x7FF) == 0)
        {
            // Relinquish control every 2048th byte

            switch (lpstScan->wScanType)
            {
                case WD_SCAN_TYPE_A2:
                    SSProgress(lpstScan->uScan.lpstA2->stA2.lpvRootCookie);
                    break;

                case WD_SCAN_TYPE_A97_MACRO:
                case WD_SCAN_TYPE_A97_VBMOD:
                case WD_SCAN_TYPE_A97_RAW:
                    SSProgress(lpstScan->uScan.lpstA97->stA97.lpvRootCookie);
                    break;

                default:
                    SSProgress(lpstScan->lpstStream->lpstRoot->lpvRootCookie);
                    break;
            }
        }
#endif // #ifdef SYM_NLM

        // Determine whether we need to read in some more bytes

        if (i == lpstScan->nNumRunBufBytes)
        {
            if (WDRunBufRead(lpstScan,l) != WD_STATUS_OK)
            {
                // Failed to read

                return(WD_STATUS_ERROR);
            }

            // Check for end of read

            if (lpstScan->nNumRunBufBytes <= 0)
                break;

            // Update CRC

            switch (lpstScan->wScanType)
            {
                case WD_SCAN_TYPE_VBA5:
                case WD_SCAN_TYPE_A97_VBMOD:
                {
                    for (i=0;i<lpstScan->nNumRunBufBytes;i++)
                    {
                        byCurByte = lpstScan->abyRunBuf[i];
                        O97ModuleSourceCRCContinue(&stMVPCRCState,byCurByte);
                        O97ModuleSourceCRCContinue2(&stCRCState,byCurByte);
                    }
                    break;
                }
                default:
                {
                    stCRCState.dwCRC = CRC32Compute(lpstScan->nNumRunBufBytes,
                                                    lpstScan->abyRunBuf,
                                                    stCRCState.dwCRC);
                    break;
                }
            }

            lpstScan->nRunBufStepOffset = 0;
            i = 0;
        }
        else
        {
            // Initialize run buffer offset of current step

            lpstScan->nRunBufStepOffset++;
        }

        //////////////////////////////////////////////////////////////
        // Apply SeekSet() sigs
        //////////////////////////////////////////////////////////////

        while (dwNextSigSetOffset == l)
        {
            // Apply the sig

            if (WDApplySig(lpabyCurSetOffsetSig,
                           lpstSigSet,
                           lpstScan,
                           &bHit) == WD_STATUS_OK)
            {
                if (bHit == TRUE)
                {
                    lpstScan->
                        lpabyMacroSigHit[wCurSetOffsetSigID>>3] |=
                            (1 << (wCurSetOffsetSigID & 0x7));

                    // Check to see whether the sig is part
                    //  of the full set

                    if (WDIsFullSetHit(lpstSigSet->lMacroSigFullSetLen,
                                       lpstSigSet->lpabyMacroSigFullSet,
                                       wCurSetOffsetSigID) == TRUE)
                    {
                        // It is part of the full set, so return

                        lpstScan->wFlags |= WD_SCAN_FLAG_MACRO_IS_FULL_SET;
                    }
                }
            }

            // Skip over the control stream of the current sig

            lpabyCurSetOffsetSig += lpabyCurSetOffsetSig[0] + 1;

            // Skip over the data stream of the current sig

            lpabyCurSetOffsetSig += lpabyCurSetOffsetSig[0] + 1;

            // Get the next sig set offset

            if (WDGetSigSetOffset(lpabyCurSetOffsetSig,
                                  &dwNextSigSetOffset) != WD_STATUS_OK)
            {
                dwNextSigSetOffset = 0xFFFFFFFF;
            }
            else
                ++wCurSetOffsetSigID;
        }

        //////////////////////////////////////////////////////////////
        // Apply SeekEnd() sigs
        //////////////////////////////////////////////////////////////

        while (dwNextSigEndOffset == l)
        {
            // Apply the sig

            if (WDApplySig(lpabyCurEndOffsetSig,
                           lpstSigSet,
                           lpstScan,
                           &bHit) == WD_STATUS_OK)
            {
                if (bHit == TRUE)
                {
                    lpstScan->
                        lpabyMacroSigHit[wCurEndOffsetSigID>>3] |=
                            (1 << (wCurEndOffsetSigID & 0x7));

                    // Check to see whether the sig is part
                    //  of the full set

                    if (WDIsFullSetHit(lpstSigSet->lMacroSigFullSetLen,
                                       lpstSigSet->lpabyMacroSigFullSet,
                                       wCurEndOffsetSigID) == TRUE)
                    {
                        // It is part of the full set, so return

                        lpstScan->wFlags |= WD_SCAN_FLAG_MACRO_IS_FULL_SET;
                    }
                }
            }

            // Skip over the control stream of the current sig

            lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

            // Skip over the data stream of the current sig

            lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

            // Get the next sig end offset

            if (WDGetSigEndOffset(lpabyCurEndOffsetSig,
                                  &dwNextSigEndOffset) != WD_STATUS_OK)
            {
                dwNextSigEndOffset = 0xFFFFFFFF;
            }
            else
            {
                dwNextSigEndOffset = lpstScan->lSize - dwNextSigEndOffset;
                ++wCurEndOffsetSigID;
            }
        }

        //////////////////////////////////////////////////////////////
        // Apply raw sigs
        //////////////////////////////////////////////////////////////

        byCurByte = lpstScan->abyRunBuf[i];

        wIndex = lpstSigSet->
            lpabyMacroSigRawByteHit[byCurByte];

        if (wIndex != 0)
        {
            lpstSigInfo = lpstSigSet->lpastRawMacroSigIndex + wIndex - 1;

            // Apply the sigs

            wCurRawID = lpstSigInfo->wBaseID;
            lpabyRawSig = lpstSigInfo->lpabySigs;

            // A zero length control stream means there are no more
            //  sigs starting with the current raw byte

            while (lpabyRawSig[0] != 0)
            {
                if (WDApplySig(lpabyRawSig,
                               lpstSigSet,
                               lpstScan,
                               &bHit) == WD_STATUS_OK)
                {
                    if (bHit == TRUE)
                    {
                        lpstScan->
                            lpabyMacroSigHit[wCurRawID>>3] |=
                                (1 << (wCurRawID & 0x7));

                        // Check to see whether the sig is part
                        //  of the full set

                        if (WDIsFullSetHit(lpstSigSet->lMacroSigFullSetLen,
                                           lpstSigSet->lpabyMacroSigFullSet,
                                           wCurRawID) == TRUE)
                        {
                            // It is part of the full set, so return

                            lpstScan->wFlags |= WD_SCAN_FLAG_MACRO_IS_FULL_SET;
                        }
                    }
                }

                // Skip past the control stream

                lpabyRawSig += lpabyRawSig[0] + 1;

                // Skip past the data stream

                lpabyRawSig += lpabyRawSig[0] + 1;

                // Increment the ID

                ++wCurRawID;
            }
        }

        // Increment run buf index

        ++i;
    }

    // Store the CRCs

    if (lpstScan->wScanType == WD_SCAN_TYPE_XL5)
    {
        if (XL5ModuleCRCFast(lpstScan->lpstStream,
                             lpstScan->uScan.stXL5.dwTableOffset,
                             lpstScan->uScan.stXL5.dwTableSize,
                             lpstScan->uScan.stXL5.dwMacroOffset,
                             lpstScan->uScan.stXL5.dwMacroSize,
                             lpstScan->abyRunBuf,
                             lpstScan->nRunBufSize,
                             NULL,
                             NULL,
                             &stCRCState.dwCRC,
                             NULL) == FALSE)
            return(WD_STATUS_ERROR);
    }

    lpstScan->dwCRC = stCRCState.dwCRC;
    lpstScan->dwMVPCRC = stMVPCRCState.dwCRC;

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDApplyMacroSigsBuffer()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  Applies the macro sigs by performing a scan from the beginning
//  of the macro body to the end of the macro body.  The SeekSet()
//  and SeekEnd() sigs are applied in order as the scan advances.
//
// Returns:
//  Nothing
//
//*************************************************************************

void WDApplyMacroSigsBuffer
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    long                l;
    int                 i;
    DWORD               dwNextSigSetOffset;
    DWORD               dwNextSigEndOffset;
    WORD                wCurSetOffsetSigID;
    LPBYTE              lpabyCurSetOffsetSig;
    WORD                wCurEndOffsetSigID;
    LPBYTE              lpabyCurEndOffsetSig;
    BOOL                bHit;
    WORD                wIndex;
    LPBYTE              lpabyRawSig;
    LPWD_SIG_INFO       lpstSigInfo;
    WORD                wCurRawID;
    BYTE                byCurByte;

    // Determine the next sig seek set offsets

    wCurSetOffsetSigID = 0;
    lpabyCurSetOffsetSig = lpstSigSet->lpabyMacroSetOffsetSigs;
    if (WDGetSigSetOffset(lpabyCurSetOffsetSig,
                          &dwNextSigSetOffset) != WD_STATUS_OK)
    {
        dwNextSigSetOffset = 0xFFFFFFFF;
    }

    // Determine the next sig seek end offsets
    //  Find the first one for which the resulting absolute offset
    //  is greater than zero

    wCurEndOffsetSigID = (WORD)(lpstSigSet->lNumMacroEndOffsetSigs);
    lpabyCurEndOffsetSig = lpstSigSet->lpabyMacroEndOffsetSigs;
    while (1)
    {
        if (WDGetSigEndOffset(lpabyCurEndOffsetSig,
                              &dwNextSigEndOffset) != WD_STATUS_OK)
        {
            dwNextSigEndOffset = 0xFFFFFFFF;
            break;
        }
        else
        if (dwNextSigEndOffset <= lpstScan->lSize)
        {
            // Found one

            dwNextSigEndOffset = lpstScan->lSize - dwNextSigEndOffset;
            break;
        }

        // Skip over the control stream of the current sig

        lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

        // Skip over the data stream of the current sig

        lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

        // Increment the ID

        ++wCurEndOffsetSigID;
    }


    //////////////////////////////////////////////////////////////////
    // Perform macro signature scan
    //////////////////////////////////////////////////////////////////

    i = 0;
    lpstScan->nRunBufStepOffset = 0;
    for (l=0;l<lpstScan->lSize;l++)
    {
        //////////////////////////////////////////////////////////
        // Apply SeekSet() sigs

        while (dwNextSigSetOffset == l)
        {
            // Apply the sig

            if (WDApplySig(lpabyCurSetOffsetSig,
                           lpstSigSet,
                           lpstScan,
                           &bHit) == WD_STATUS_OK)
            {
                if (bHit == TRUE)
                {
                    lpstScan->
                        lpabyMacroSigHit[wCurSetOffsetSigID>>3] |=
                            (1 << (wCurSetOffsetSigID & 0x7));

                    // Check to see whether the sig is part
                    //  of the full set

                    if (WDIsFullSetHit(lpstSigSet->lMacroSigFullSetLen,
                                       lpstSigSet->lpabyMacroSigFullSet,
                                       wCurSetOffsetSigID) == TRUE)
                    {
                        // It is part of the full set, so return

                        lpstScan->wFlags |= WD_SCAN_FLAG_MACRO_IS_FULL_SET;
                    }
                }
            }

            // Skip over the control stream of the current sig

            lpabyCurSetOffsetSig += lpabyCurSetOffsetSig[0] + 1;

            // Skip over the data stream of the current sig

            lpabyCurSetOffsetSig += lpabyCurSetOffsetSig[0] + 1;

            // Get the next sig set offset

            if (WDGetSigSetOffset(lpabyCurSetOffsetSig,
                                  &dwNextSigSetOffset) != WD_STATUS_OK)
            {
                dwNextSigSetOffset = 0xFFFFFFFF;
            }
            else
                ++wCurSetOffsetSigID;
        }

        //////////////////////////////////////////////////////////
        // Apply SeekEnd() sigs

        while (dwNextSigEndOffset == l)
        {
            // Apply the sig

            if (WDApplySig(lpabyCurEndOffsetSig,
                           lpstSigSet,
                           lpstScan,
                           &bHit) == WD_STATUS_OK)
            {
                if (bHit == TRUE)
                {
                    lpstScan->
                        lpabyMacroSigHit[wCurEndOffsetSigID>>3] |=
                            (1 << (wCurEndOffsetSigID & 0x7));

                    // Check to see whether the sig is part
                    //  of the full set

                    if (WDIsFullSetHit(lpstSigSet->lMacroSigFullSetLen,
                                       lpstSigSet->lpabyMacroSigFullSet,
                                       wCurEndOffsetSigID) == TRUE)
                    {
                        // It is part of the full set, so return

                        lpstScan->wFlags |= WD_SCAN_FLAG_MACRO_IS_FULL_SET;
                    }
                }
            }

            // Skip over the control stream of the current sig

            lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

            // Skip over the data stream of the current sig

            lpabyCurEndOffsetSig += lpabyCurEndOffsetSig[0] + 1;

            // Get the next sig end offset

            if (WDGetSigEndOffset(lpabyCurEndOffsetSig,
                                  &dwNextSigEndOffset) != WD_STATUS_OK)
            {
                dwNextSigEndOffset = 0xFFFFFFFF;
            }
            else
            {
                dwNextSigEndOffset = lpstScan->lSize - dwNextSigEndOffset;
                ++wCurEndOffsetSigID;
            }
        }

        //////////////////////////////////////////////////////////
        // Apply raw sigs

        byCurByte = lpstScan->abyRunBuf[i];

        wIndex = lpstSigSet->
            lpabyMacroSigRawByteHit[byCurByte];

        if (wIndex != 0)
        {
            lpstSigInfo = lpstSigSet->lpastRawMacroSigIndex + wIndex - 1;

            // Apply the sigs

            wCurRawID = lpstSigInfo->wBaseID;
            lpabyRawSig = lpstSigInfo->lpabySigs;

            // A zero length control stream means there are no more
            //  sigs starting with the current raw byte

            while (lpabyRawSig[0] != 0)
            {
                if (WDApplySig(lpabyRawSig,
                               lpstSigSet,
                               lpstScan,
                               &bHit) == WD_STATUS_OK)
                {
                    if (bHit == TRUE)
                    {
                        lpstScan->
                            lpabyMacroSigHit[wCurRawID>>3] |=
                                (1 << (wCurRawID & 0x7));

                        // Check to see whether the sig is part
                        //  of the full set

                        if (WDIsFullSetHit(lpstSigSet->lMacroSigFullSetLen,
                                           lpstSigSet->lpabyMacroSigFullSet,
                                           wCurRawID) == TRUE)
                        {
                            // It is part of the full set, so return

                            lpstScan->wFlags |= WD_SCAN_FLAG_MACRO_IS_FULL_SET;
                        }
                    }
                }

                // Skip past the control stream

                lpabyRawSig += lpabyRawSig[0] + 1;

                // Skip past the data stream

                lpabyRawSig += lpabyRawSig[0] + 1;

                // Increment the ID

                ++wCurRawID;
            }
        }

        // Increment run buf index

        ++i;

        lpstScan->nRunBufStepOffset++;
    }
}

//*************************************************************************
//
// void WDMarkCRC()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  Marks the CRC as hit if it is present in the given signature
//  set.  The CRC is from lpstScan->dwCRC.
//
// Returns:
//  Nothing
//
//*************************************************************************

void WDMarkCRC
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    long                lLow, lMid, lHigh;

    //////////////////////////////////////////////////////////////////
    // Update CRC hit bit array
    //////////////////////////////////////////////////////////////////

    // See whether the CRC is in the list

    lLow = 0;
    lHigh = lpstSigSet->lNumCRCs - 1;
    while (lLow <= lHigh)
    {
        lMid = (lLow + lHigh) / 2;

        if (lpstScan->dwCRC < lpstSigSet->lpadwCRCs[lMid])
        {
            // In lower half

            lHigh = lMid - 1;
        }
        else
        if (lpstScan->dwCRC > lpstSigSet->lpadwCRCs[lMid])
        {
            // In upper half

            lLow = lMid + 1;
        }
        else
        {
            // Found a match

            break;
        }
    }

    // Is it within range?

    if (lLow <= lHigh)
    {
        // It's a hit, mark it

        lpstScan->lpabyCRCHit[lMid>>3] |= (1 << (lMid & 0x7));
    }
}


//*************************************************************************
//
// WD_STATUS WDApplyNamedCRCSigs()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  Applies the named CRC sigs by first performing a binary search
//  for the existence of the CRC of the current macro body in the
//  CRC named sig list and then apply the named sig if there is a
//  match.
//
// Returns:
//  WD_STATUS_OK            Always
//
//*************************************************************************

WD_STATUS WDApplyNamedCRCSigs
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    LPWD_NAMED_CRC_SIG_INFO lpstCRCSigInfo;
    DWORD                   dwCRC;
    DWORD                   dwCandCRC;
    WORD                    wID;
    LPBYTE                  lpabySig;
    BOOL                    bHit;
    long                    lLow, lMid, lHigh;

    // Try the CRC

    dwCRC = lpstScan->dwCRC;

    // Perform a binary search for the CRC

    lLow = 0;
    lHigh = lpstSigSet->lNumNamedCRCSigSets - 1;
    while (lLow <= lHigh)
    {
        lMid = (lLow + lHigh) / 2;

        lpstCRCSigInfo = lpstSigSet->lpastNamedCRCSigInfo + lMid;
        dwCandCRC = lpstSigSet->lpadwCRCs[lpstCRCSigInfo->wCRCIndex];

        if (dwCRC < dwCandCRC)
        {
            // In lower half

            lHigh = lMid - 1;
        }
        else
        if (dwCRC > dwCandCRC)
        {
            // In upper half

            lLow = lMid + 1;
        }
        else
        {
            // Found a match

            break;
        }
    }

    // Is it within range?

    if (lLow <= lHigh)
    {
        // Apply the signatures

        wID = lpstCRCSigInfo->wBaseID;
        lpabySig = lpstCRCSigInfo->lpabySigs;

        // A zero length control stream means there are no more
        //  sigs in this CRC set

        while (lpabySig[0] != 0)
        {
            if (WDApplySig(lpabySig,
                           lpstSigSet,
                           lpstScan,
                           &bHit) == WD_STATUS_OK)
            {
                if (bHit == TRUE)
                {
                    lpstScan->lpabyNamedCRCSigHit[wID>>3] |=
                        (1 << (wID & 0x7));

                    // Check to see whether the sig is part
                    //  of the full set

                    if (WDIsFullSetHit(lpstSigSet->lNamedCRCSigFullSetLen,
                                       lpstSigSet->lpabyNamedCRCSigFullSet,
                                       wID) == TRUE)
                    {
                        // It is part of the full set, so return

                        lpstScan->wFlags |= WD_SCAN_FLAG_MACRO_IS_FULL_SET;
                    }
                }
            }

            // Skip past the control stream

            lpabySig += lpabySig[0] + 1;

            // Skip past the data stream

            lpabySig += lpabySig[0] + 1;

            // Increment the ID

            ++wID;
        }
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDScanMacro()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  1. Apply the name sigs
//  2. Apply the macro sigs
//  3. Apply the named CRC sigs
//
// Returns:
//  WD_STATUS_OK            Always
//
//*************************************************************************

WD_STATUS WDScanMacro
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    // Set FullSet status for macro

    lpstScan->wFlags &= ~WD_SCAN_FLAG_MACRO_IS_FULL_SET;

    // Apply name sigs to macro

    WDApplyNameSigs(lpstSigSet,lpstScan);

    // Apply macro sigs to macro

    WDApplyMacroSigs(lpstSigSet,lpstScan);

    // Mark the CRC

    WDMarkCRC(lpstSigSet,lpstScan);

    // Apply named CRC sigs

    WDApplyNamedCRCSigs(lpstSigSet,lpstScan);

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// void WDInitHitBitArrays()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  Initializes the global hit bit arrays.
//
// Returns:
//  Nothing
//
//*************************************************************************

void WDInitHitBitArrays
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan
)
{
    int                     i;
    int                     nHitArrayByteSize;

    // Initialize the name sig hit bit array

    nHitArrayByteSize = (int)(lpstSigSet->lNumNameSigs / 8);
    if (lpstSigSet->lNumNameSigs % 8)
        ++nHitArrayByteSize;

    for (i=0;i<nHitArrayByteSize;i++)
        lpstScan->lpabyNameSigInterMacroHit[i] = 0;

    // Initialize the named CRC sig hit bit array

    nHitArrayByteSize = (int)(lpstSigSet->lNumNamedCRCSigs / 8);
    if (lpstSigSet->lNumNamedCRCSigs % 8)
        ++nHitArrayByteSize;

    for (i=0;i<nHitArrayByteSize;i++)
        lpstScan->lpabyNamedCRCSigHit[i] = 0;

    // Initialize the macro sig hit bit array

    nHitArrayByteSize = (int)(lpstSigSet->lNumMacroSigs / 8);
    if (lpstSigSet->lNumMacroSigs % 8)
        ++nHitArrayByteSize;

    for (i=0;i<nHitArrayByteSize;i++)
        lpstScan->lpabyMacroSigHit[i] = 0;

    // Initialize the CRC hit bit array

    nHitArrayByteSize = (int)(lpstSigSet->lNumCRCs / 8);
    if (lpstSigSet->lNumCRCs % 8)
        ++nHitArrayByteSize;

    for (i=0;i<nHitArrayByteSize;i++)
        lpstScan->lpabyCRCHit[i] = 0;
}


//*************************************************************************
//
// void WDApplyVirusSignatures()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//  lplpstVirusSigInfo      Ptr to ptr to store ptr of virus sig
//                              info in case of hit
//
// Description:
//  Assumes that the global hit bit arrays are correct and applies
//  the virus signatures.
//
// Returns:
//  Nothing
//
//*************************************************************************

WD_STATUS WDApplyVirusSignatures
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo
)
{
    long                    l;
    LPWD_VIRUS_SIG_INFO     lpstVirusSigInfo;

    lpstVirusSigInfo = lpstSigSet->lpastVirusSigInfo;
    for (l=0;l<lpstSigSet->lNumVirusSigs;l++)
    {
#ifdef SYM_NLM
        if ((l & 0x3FF) == 0)
        {
            // Relinquish control every 1024th signature

            switch (lpstScan->wScanType)
            {
                case WD_SCAN_TYPE_A2:
                    SSProgress(lpstScan->uScan.lpstA2->stA2.lpvRootCookie);
                    break;

                case WD_SCAN_TYPE_A97_MACRO:
                case WD_SCAN_TYPE_A97_VBMOD:
                case WD_SCAN_TYPE_A97_RAW:
                    SSProgress(lpstScan->uScan.lpstA97->stA97.lpvRootCookie);
                    break;

                default:
                    SSProgress(lpstScan->lpstStream->lpstRoot->lpvRootCookie);
                    break;
            }
        }
#endif // #ifdef SYM_NLM

        if (WDApplyVirusSig(lpstVirusSigInfo->lpabySig,
                            lpstSigSet,
                            lpstScan,
                            lpstScan->abyRunBuf,
                            NULL) ==
            WD_STATUS_SIG_HIT)
        {
            *lplpstVirusSigInfo = lpstVirusSigInfo;
            return(WD_STATUS_VIRUS_FOUND);
        }

        ++lpstVirusSigInfo;
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDScanDoc()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//  lplpstVirusSigInfo      Ptr to ptr to store ptr of virus sig
//                              info in case of hit
//  lpbMVPApproved          Ptr to BOOL for MVP status
//
// Description:
//  Assumes lpstScan->lpstCallBack and lpstScan->lpstOLEStream are set.
//
//  1. Initialize global hit bit arrays
//  2. For each macro:
//      a. Get the macro info
//      b. Scan the macro
//  3. Apply the virus sigs
//
//  *lpbMVPApproved should have been initialized to TRUE at the
//  very beginning.  It is set to FALSE only if an unapproved
//  macro is seen.
//
// Returns:
//  WD_STATUS_VIRUS_FOUND   If a virus signature matched
//  WD_STATUS_OK            If no virus signature matched
//  WD_STATUS_ERROR         On error
//
//*************************************************************************

WD_STATUS WDScanDoc
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo,
    LPBOOL                  lpbMVPApproved
)
{
    BOOL                    bStatus;

    // Initialize macro count

    lpstScan->wMacroCount = 0;

    // Assume a full set

    lpstScan->wFlags = WD_SCAN_FLAG_FULL_SET;

    //////////////////////////////////////////////////////////////////
    // Initialize global hit bit arrays
    //////////////////////////////////////////////////////////////////

    WDInitHitBitArrays(lpstSigSet,lpstScan);


    //////////////////////////////////////////////////////////////////
    // Iterate through macros
    //////////////////////////////////////////////////////////////////

    if (WD7MacroScanInit(lpstScan->lpstStream,
                         lpstScan->uScan.stWD7.lpstMacroScan->lpstKey,
                         lpstScan->uScan.stWD7.lpstMacroScan,
                         &bStatus) == WD7_STATUS_ERROR)
    {
        // Error initializing for macro scan

        return(WD_STATUS_ERROR);
    }

    if (bStatus == FALSE)
    {
        // No macros

        return(WD_STATUS_OK);
    }

    while (1)
    {
        if (WD7GetNextMacroForScan(lpstScan->uScan.stWD7.lpstMacroScan,
                                   lpstScan->abyName,
                                   &lpstScan->uScan.stWD7.dwMacroOffset,
                                   &lpstScan->uScan.stWD7.dwMacroSize,
                                   &lpstScan->uScan.stWD7.byMacroEncryptByte,
                                   &bStatus) == WD7_STATUS_ERROR)
        {
            return(WD_STATUS_ERROR);
        }

        if (bStatus == TRUE)
        {
            // No more macros to scan

            break;
        };

        if (WDScanMacro(lpstSigSet,
                        lpstScan) != WD_STATUS_OK)
        {
            // Error scanning macro, go to the next one

            continue;
        }

        if (MVPCheck(MVP_WD7,
                     lpstScan->abyName,
                     lpstScan->dwCRC) == FALSE)
        {
            // Found a non-approved macro

            *lpbMVPApproved = FALSE;
        }

        if (lpstScan->wMacroCount < 0xFFFF)
            lpstScan->wMacroCount++;

        // Determine full set status

        if ((lpstScan->wFlags & WD_SCAN_FLAG_MACRO_IS_FULL_SET) == 0)
        {
            // Found a macro that was not part of the full set

            lpstScan->wFlags &= ~WD_SCAN_FLAG_FULL_SET;
        }
    }

    // If macro count is zero, don't assume full set

    if (lpstScan->wMacroCount == 0)
        lpstScan->wFlags &= ~WD_SCAN_FLAG_FULL_SET;

    //////////////////////////////////////////////////////////////////
    // Apply virus signatures
    //////////////////////////////////////////////////////////////////

    if (WDApplyVirusSignatures(lpstSigSet,
                               lpstScan,
                               lplpstVirusSigInfo) == WD_STATUS_VIRUS_FOUND)
    {
        return(WD_STATUS_VIRUS_FOUND);
    }

    // No virus

    return (WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS VBA5ScanModule()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  Assumes lpstScan->lpstCallBack and lpstScan->lpstOLEStream are set.
//  Scans the VBA module.
//
// Returns:
//  WD_STATUS_OK            On success
//  WD_STATUS_ERROR         On error
//
//*************************************************************************

WD_STATUS VBA5ScanModule
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan
)
{
    // Initialize LZNT structure

    if (VBA5LZNTModuleInit(lpstScan->lpstStream,
                           lpstScan->uScan.stVBA5.lpstLZNT) == FALSE)
    {
        // Failed to initialize for reading source text string

        return(WD_STATUS_ERROR);
    }

    if (WDScanMacro(lpstSigSet,
                    lpstScan) != WD_STATUS_OK)
    {
        // Error scanning macro, go to the next one

        return(WD_STATUS_ERROR);
    }

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDApplyXL4MacroSigs()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  Applies the macro sigs by performing a scan from the beginning
//  of the macro body to the end of the macro body.  The SeekSet()
//  and SeekEnd() sigs are applied in order as the scan advances.
//
// Returns:
//  WD_STATUS_OK            Always
//
//*************************************************************************

WD_STATUS WDApplyXL4MacroSigs
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    DWORD               dwOffset;
    DWORD               dwMaxOffset;
    XL_REC_HDR_T        stRec;
    DWORD               dwBytesRead;
    WORD                wExprOffset;
    WORD                wExprLen;
    DWORD               dwCRC;
    LPSS_STREAM         lpstStream;
    DWORD               dwBOFDepth;
    int                 nReadSize;
    int                 i;
    int                 j;
    int                 nMinSize;
    LPBYTE              lpabyBuf;
    LPBYTE              lpabyExpr;
    LPXL5ENCKEY         lpstKey;

    // Initialize CRC

    CRC32Init(dwCRC);

    lpstStream = lpstScan->lpstStream;

    lpstKey = lpstScan->uScan.stXL4.lpstKey;
    if (lpstKey != NULL)
    {
        if (lpstKey->bEncrypted == FALSE)
            lpstKey = NULL;
    }

    /////////////////////////////////////////////////////////////
    // Iterate through all formula record

    dwBOFDepth = 0;
    dwOffset = lpstScan->uScan.stXL4.dwBOFOffset;
    dwMaxOffset = SSStreamLen(lpstScan->lpstStream);
    nReadSize = lpstScan->nRunBufSize;
    lpabyBuf = lpstScan->abyRunBuf;
    nMinSize = sizeof(XL_REC_HDR_T);
    while (dwOffset < dwMaxOffset)
    {
        // Read a chunk

        if (dwMaxOffset - dwOffset < (DWORD)nReadSize)
            nReadSize = (int)(dwMaxOffset - dwOffset);

        if (nReadSize < nMinSize)
            break;

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpabyBuf,
                       nReadSize,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != (DWORD)nReadSize)
            break;

        // Iterate through the buffer

        i = 0;
        stRec.wLen = 0;
        nMinSize = sizeof(XL_REC_HDR_T);
        while (1)
        {
            // Make sure we have enough bytes for the type
            //  and length fields of the record header plus the
            //  size of the previous record's data

            if ((DWORD)(i + 4) + stRec.wLen > (DWORD)nReadSize)
            {
                dwOffset += i + (DWORD)stRec.wLen;
                break;
            }

            // Increment past the previous record's data

            i += stRec.wLen;

            // Get the header

            stRec = *(LPXL_REC_HDR)(lpabyBuf + i);
            stRec.wType = WENDIAN(stRec.wType);
            stRec.wLen = WENDIAN(stRec.wLen);

            if (stRec.wType == eXLREC_BOF)
                ++dwBOFDepth;
            else
            if (stRec.wType == eXLREC_EOF && dwBOFDepth-- <= 1)
            {
                // Store the CRC

                lpstScan->dwCRC = dwCRC;
                return(WD_STATUS_OK);
            }

            switch (stRec.wType)
            {
                case eXLREC_ARRAY:
                case eXLREC_ARRAY2:
                    wExprOffset = 12;
                    break;

                case eXLREC_FORMULA:
                case eXLREC_FORMULA2:
                    wExprOffset = 20;
                    break;

                case eXLREC_SHRFMLA:
                    wExprOffset = 8;
                    break;

                default:
                    wExprOffset = 0;
                    break;
            }

            // Increment past the record header

            i += 4;

            if (wExprOffset == 0)
                continue;

            // Are there enough bytes for the length?

            if (nReadSize - i < wExprOffset + 2)
            {
                nMinSize += wExprOffset + 2;
                dwOffset += i - 4;
                break;
            }

            // Read the length of the parsed expression

            lpabyExpr = lpabyBuf + i + wExprOffset;
            if (lpstKey != NULL)
            {
                XL5DecryptData(dwOffset + i - 4,
                               stRec,
                               lpstKey,
                               wExprOffset,
                               lpabyExpr,
                               sizeof(WORD));
            }

            wExprLen = ((WORD)lpabyExpr[0]) | (((WORD)lpabyExpr[1]) << 8);

            if (wExprLen > 256)
                wExprLen = 256;

            if (nReadSize - i - wExprOffset - 2 < wExprLen)
            {
                nMinSize += wExprOffset + 2 + wExprLen;
                dwOffset += i - 4;
                break;
            }

            lpabyExpr += 2;
            if (lpstKey != NULL)
            {
                XL5DecryptData(dwOffset + i - 4,
                               stRec,
                               lpstKey,
                               wExprOffset + 2,
                               lpabyExpr,
                               wExprLen);
            }

            lpstScan->lRunBufOffset = 0;
            lpstScan->nNumRunBufBytes = (int)wExprLen;
            lpstScan->lSize = wExprLen;

            lpstScan->lExtraBufOffset = 0xFFFFFFFF;
            lpstScan->nNumExtraBufBytes = 0;

            // Copy the expression to the beginning of the run buffer

            for (j=0;j<wExprLen;j++)
                lpabyBuf[j] = lpabyExpr[j];

            // Perform macro signature scan

            WDApplyMacroSigsBuffer(lpstSigSet,lpstScan);

            // Update CRC

            if (lpstScan->wScanType == WD_SCAN_TYPE_XL4_95)
            {
                dwCRC = XL5CRC32Expr(lpstScan->abyRunBuf,
                                     wExprLen,
                                     dwCRC,
                                     NULL);
            }
            else
            {
                dwCRC = XL97CRC32Expr(lpstScan->abyRunBuf,
                                      wExprLen,
                                      dwCRC,
                                      NULL);
            }
        }
    }

    // Store the CRC

    lpstScan->dwCRC = dwCRC;

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDScanXL4Macro()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  1. Apply the name sigs
//  2. Apply the macro sigs
//  3. Apply the named CRC sigs
//
// Returns:
//  WD_STATUS_OK            Always
//
//*************************************************************************

WD_STATUS WDScanXL4Macro
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    // Set FullSet status for macro

    lpstScan->wFlags &= ~WD_SCAN_FLAG_MACRO_IS_FULL_SET;

    // Apply name sigs to macro

    WDApplyNameSigs(lpstSigSet,lpstScan);

    // Apply macro sigs to macro

    WDApplyXL4MacroSigs(lpstSigSet,lpstScan);

    // Mark the CRC

    WDMarkCRC(lpstSigSet,lpstScan);

    // Apply named CRC sigs

    WDApplyNamedCRCSigs(lpstSigSet,lpstScan);

    return(WD_STATUS_OK);
}


//*************************************************************************
//
// WD_STATUS WDScanA2Macros()
//
// Parameters:
//  lpstSigSet              Signature set to apply
//  lpstScan                Ptr to scan structure
//
// Description:
//  1. Apply the name sigs
//  2. Apply the macro sigs
//  3. Apply the named CRC sigs
//
// Returns:
//  WD_STATUS_OK            Always
//
//*************************************************************************

WD_STATUS WDScanA2Macros
(
    LPWD_SIG_SET        lpstSigSet,
    LPWD_SCAN           lpstScan
)
{
    LPA2_P06            lpstPage;
    LPBYTE              lpabyData;
    DWORD               dwByteCount;
    WORD                wItemCount;
    WORD                w;
    WORD                wTemp;
    WORD                wOffset;
    int                 nStart;
    int                 nEnd;
    int                 i;
    int                 j;
    LPBYTE              lpabyAction;
    LPA2MACRO_ACTION    lpstAction;
    int                 nActionSize;
    LPA2MACROE          lpstMacroE;
    A2MACRO_LINE_INFO_T stInfo;
    DWORD               dwCRCSize;
    int                 nMacroCount;
    DWORD               dwPage;

    if (A2GetFirstMacrosDataPage(&lpstScan->uScan.lpstA2->stA2,
                                 &dwPage) == FALSE)
        return(WD_STATUS_OK);

    // Set FullSet status for macro

    lpstScan->wFlags &= ~WD_SCAN_FLAG_MACRO_IS_FULL_SET;

    lpstPage = (LPA2_P06)lpstScan->uScan.lpstA2->stA2.abyPage;
    lpabyData = lpstScan->uScan.lpstA2->stA2.abyPage;

    // Up to 1024 pages

    nMacroCount = 0;
    for (i=0;i<1024 && dwPage != 0;i++)
    {
        // Read the page

        if (ACSFileSeek(lpstScan->uScan.lpstA2->stA2.lpvRootCookie,
                        lpstScan->uScan.lpstA2->stA2.lpvFile,
                        dwPage * A2_PAGE_SIZE,
                        ACS_SEEK_SET,
                        NULL) == FALSE)
            return(WD_STATUS_OK);

        if (ACSFileRead(lpstScan->uScan.lpstA2->stA2.lpvRootCookie,
                        lpstScan->uScan.lpstA2->stA2.lpvFile,
                        lpstPage,
                        A2_PAGE_SIZE,
                        &dwByteCount) == FALSE ||
            dwByteCount != A2_PAGE_SIZE)
            return(WD_STATUS_OK);

        // Validate the item count

        wItemCount = WENDIAN(lpstPage->wItemCount);
        if (wItemCount > A2_P06_MAX_ITEMS)
            return(WD_STATUS_OK);

        for (w=0;w<wItemCount;w++)
        {
            wOffset = WENDIAN(lpstPage->awItemOffsets[w]);
            if ((wOffset & 0xF000) == 0)
                continue;

            wOffset &= 0xFFF;

            // Ignore illegal lines

            if (wOffset > A2_PAGE_SIZE - sizeof(A2MACRO_ACTION_T) - 3)
                continue;

            lpabyAction = lpabyData + wOffset;
            lpstAction = (LPA2MACRO_ACTION)lpabyAction;

            // Validate the item size

            wTemp = WENDIAN(lpstAction->wItemSize);
            if (wTemp > A2_PAGE_SIZE - wOffset)
                continue;

            nActionSize = (int)wTemp;
            if (lpstAction->wLineNumber == 0)
            {
                // Begin a new macro

                if (lpstAction->abyUnknown0[1] == 0x0E)
                {
                    lpstMacroE = (LPA2MACROE)
                        (lpabyAction + lpstAction->wItemSize -
                        sizeof(A2MACROE_T));

                    nStart = lpstMacroE->byMainMacroNameOffset;
                    nEnd = lpstMacroE->byInfoSize;
                }
                else
                {
                    nStart = lpabyAction[nActionSize - 3];
                    nEnd = lpabyAction[nActionSize - 4];
                }

                if (nEnd < nStart || nEnd > nActionSize)
                {
                    // Error

                    continue;
                }

                lpstScan->wMacroCount++;

                // Get the name

                for (j=0;j<nEnd-nStart;j++)
                    lpstScan->abyName[j] = SSToUpper(lpabyAction[nStart+j]);

                lpstScan->abyName[j] = 0;

                // Apply name sigs to macro

                WDApplyNameSigs(lpstSigSet,lpstScan);

                if (nMacroCount++ != 0)
                {
                    // Mark the CRC

                    WDMarkCRC(lpstSigSet,lpstScan);

                    // Apply named CRC sigs

                    WDApplyNamedCRCSigs(lpstSigSet,lpstScan);
                }

                // Initialize the CRC

                CRC32Init(lpstScan->dwCRC);
                dwCRCSize = 0;

                continue;
            }

            if (A2GetMacroLineInfo(lpabyAction,
                                   nActionSize,
                                   &stInfo) != FALSE)
            {
                lpstScan->dwCRC = A2CRCMacroLine(lpstScan->dwCRC,
                                                 lpabyAction,
                                                 &stInfo,
                                                 &dwCRCSize);
            }

            // Set up for scanning

            lpstScan->lSize = nActionSize;
            lpstScan->lRunBufOffset = 0;
            lpstScan->nNumRunBufBytes = nActionSize;
            lpstScan->lExtraBufOffset = 0xFFFFFFFF;
            lpstScan->nNumExtraBufBytes = 0;
            for (j=0;j<nActionSize;j++)
                lpstScan->abyRunBuf[j] = lpabyAction[j];

            WDApplyMacroSigsBuffer(lpstSigSet,lpstScan);
        }

        // Set up for the next page

        dwPage = DWENDIAN(lpstPage->dwNextPage);
    }

    if (nMacroCount != 0)
    {
        // Mark the CRC

        WDMarkCRC(lpstSigSet,lpstScan);

        // Apply named CRC sigs

        WDApplyNamedCRCSigs(lpstSigSet,lpstScan);
    }

    return(TRUE);
}


