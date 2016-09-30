//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDSCAN.CPv   1.12   27 May 1997 16:51:16   DCHI  $
//
// Description:
//  Top-level scan module.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDSCAN.CPv  $
// 
//    Rev 1.12   27 May 1997 16:51:16   DCHI
// Modifications to support dynamic allocation of hit memory and new
// Office 97 CRC.
// 
//    Rev 1.11   06 May 1997 11:06:16   DCHI
// Fixed infinite loop when trying to skip over white space lines that
// begin with spaces.
// 
//    Rev 1.10   21 Apr 1997 16:05:40   DCHI
// Reverted back to revision 1.8.
// 
//    Rev 1.9   21 Apr 1997 12:30:36   DCHI
// Reverted CRC on Office 97 modules to not skip over blank lines.
// 
//    Rev 1.8   10 Apr 1997 16:51:52   DCHI
// Fixed FullSet checking.
// 
//    Rev 1.7   09 Apr 1997 16:41:20   DCHI
// Updated CRC computation for Office 97 documents to skip over white
// space lines also.
// 
//    Rev 1.6   08 Apr 1997 12:40:56   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.5   07 Apr 1997 18:10:58   DCHI
// Added MVP support.
// 
//    Rev 1.4   14 Mar 1997 16:35:14   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.3   13 Feb 1997 13:34:48   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.2   23 Jan 1997 11:20:24   DCHI
// Corrected reading of run buffer read count.
// 
//    Rev 1.1   23 Jan 1997 11:12:12   DCHI
// Fixed global variable usage problem.
// 
//    Rev 1.0   17 Jan 1997 11:23:42   DCHI
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
#include "wdapvsig.h"
#include "mvp.h"

WD_VIRUS_SIG_INFO_T FAR gstMVPVirusSigInfo =
{
    VID_MVP,
    NULL
};

DWORD gadwCRC32Table[256] =
{
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

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
        lpstScan->uScan.stWD7.lStartOffset = lpstScan->uScan.stWD7.
            lpstMacroInfo->dwNameOffset;
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
//  Reads into lpstScan->abyRunBuf up to WD_RUN_BUF_SIZE bytes
//  beginning from lOffset.  The number of bytes read is the maximum
//  of WD_RUN_BUF_SIZE and the difference of lpstScan->lSize and
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

    if (WD_RUN_BUF_SIZE > lpstScan->lSize - lOffset)
        wBytesToRead = (WORD)(lpstScan->lSize - lOffset);
    else
        wBytesToRead = WD_RUN_BUF_SIZE;

    if (lpstScan->wScanType == WD_SCAN_TYPE_WD7)
    {
        if (WordDocEncryptedRead(lpstScan->lpstCallBack,
                                 lpstScan->lpstOLEStream,
                                 lpstScan->uScan.stWD7.lpstKey,
                                 (DWORD)(lpstScan->uScan.stWD7.lStartOffset +
                                     lOffset),
                                 lpstScan->abyRunBuf,
                                 wBytesToRead) != wBytesToRead)
        {
            // Failed to read

            return(WD_STATUS_ERROR);
        }

        lpstScan->lRunBufOffset = lOffset;
        lpstScan->nNumRunBufBytes = wBytesToRead;

        // Decrypt if necessary

        if (lpstScan->uScan.stWD7.byEncrypt != 0)
        {
            LPBYTE lpby = lpstScan->abyRunBuf;
            BYTE byEncrypt = lpstScan->uScan.stWD7.byEncrypt;

            while (wBytesToRead-- > 0)
                *lpby++ ^= byEncrypt;
        }
    }
    else
    {
        if (LZNTGetBytes(lpstScan->uScan.stVBA5.lpstLZNT,
                         (DWORD)lOffset,
                         (int)wBytesToRead,
                         lpstScan->abyRunBuf) != (int)wBytesToRead)
        {
            return(WD_STATUS_ERROR);
        }

        lpstScan->lRunBufOffset = lOffset;
        lpstScan->nNumRunBufBytes = wBytesToRead;
    }

    return(WD_STATUS_OK);
}


//***************************************************************
// States for skipping attribute and white space lines
//***************************************************************

#define WD_VBA5_CRC_STATE_BOL       0       // Beginning of line
#define WD_VBA5_CRC_STATE_ATTRIBUTE 1       // Checking for Attribute
#define WD_VBA5_CRC_STATE_SPACES    2       // Checking for all spaces
#define WD_VBA5_CRC_STATE_NORMAL    3       // Normal CRC
#define WD_VBA5_CRC_STATE_SKIP_LINE 4       // Skip the line

static BYTE gabyattribute[] =
{
    'a', 't', 't', 'r', 'i', 'b', 'u', 't', 'e', ' '
};


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
    DWORD               dwCRC;
    WORD                wVBA5CRCState;
    DWORD               dwVBA5CRCSubState;
    long                lLow, lMid, lHigh;

    //////////////////////////////////////////////////////////////////
    // Name signature scan initialization
    //////////////////////////////////////////////////////////////////

    // Initialize scan structure

    if (lpstScan->wScanType == WD_SCAN_TYPE_WD7)
    {
        lpstScan->uScan.stWD7.lStartOffset = lpstScan->uScan.stWD7.
            lpstMacroInfo->dwOffset;
        lpstScan->lSize = lpstScan->uScan.stWD7.
            lpstMacroInfo->dwSize;
        lpstScan->uScan.stWD7.byEncrypt = lpstScan->uScan.stWD7.
            lpstMacroInfo->byEncrypt;
    }
    else
    {
        lpstScan->lSize = lpstScan->uScan.stVBA5.lpstLZNT->dwSize;
    }

    lpstScan->nNumRunBufBytes = 0;
    lpstScan->lRunBufOffset = 0;

    lpstScan->lExtraBufOffset = 0xFFFFFFFF;
    lpstScan->nNumExtraBufBytes = 0;

    // Initialize CRC

    dwCRC = 0xFFFFFFFF;

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

    wVBA5CRCState = WD_VBA5_CRC_STATE_BOL;
    i = 0;
    for (l=0;l<lpstScan->lSize;l++)
    {
        // Determine whether we need to read in some more bytes

        if (i == lpstScan->nNumRunBufBytes)
        {
            if (WDRunBufRead(lpstScan,l) != WD_STATUS_OK)
            {
                // Failed to read

                return(WD_STATUS_ERROR);
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

        //////////////////////////////////////////////////////////////
        // Update CRC
        //////////////////////////////////////////////////////////////

        if (lpstScan->wScanType == WD_SCAN_TYPE_WD7)
        {
            dwCRC = ((dwCRC >> 8) & 0x00FFFFFFL) ^
                gadwCRC32Table[(dwCRC ^ byCurByte) & 0x000000FFL];
        }
        else
        {
            if (byCurByte <= 'Z' && byCurByte >= 'A')
                byCurByte = byCurByte + ('a' - 'A');

            switch (wVBA5CRCState)
            {
                case WD_VBA5_CRC_STATE_BOL:
                    if (byCurByte == gabyattribute[0])
                    {
                        // Could it be an attribute line

                        wVBA5CRCState = WD_VBA5_CRC_STATE_ATTRIBUTE;
                        dwVBA5CRCSubState = 1;
                    }
                    else
                    if (byCurByte == ' ')
                    {
                        // Could it be a line of spaces?

                        wVBA5CRCState = WD_VBA5_CRC_STATE_SPACES;
                        dwVBA5CRCSubState = 1;
                    }
                    else
                    if (byCurByte == 0x0D)
                    {
                        wVBA5CRCState = WD_VBA5_CRC_STATE_SPACES;
                        dwVBA5CRCSubState = 0;
                    }
                    else
                    {
                        // It is neither a line of spaces nor an
                        //  attribute line

                        wVBA5CRCState = WD_VBA5_CRC_STATE_NORMAL;

                        dwCRC = ((dwCRC >> 8) & 0x00FFFFFFL) ^
                            gadwCRC32Table[(dwCRC ^ byCurByte) & 0x000000FFL];
                    }
                    break;

                case WD_VBA5_CRC_STATE_ATTRIBUTE:
                    if (byCurByte == gabyattribute[dwVBA5CRCSubState])
                    {
                        if (++dwVBA5CRCSubState == sizeof(gabyattribute))
                        {
                            // It is an attribute line

                            wVBA5CRCState = WD_VBA5_CRC_STATE_SKIP_LINE;
                        }
                    }
                    else
                    {
                        // CRC the attribute bytes seen thus far
                        //  and then continue normally

                        for (wIndex=0;wIndex<dwVBA5CRCSubState;wIndex++)
                        {
                            dwCRC = ((dwCRC >> 8) & 0x00FFFFFFL) ^
                                gadwCRC32Table[(dwCRC ^ gabyattribute[wIndex]) & 0x000000FFL];
                        }

                        dwCRC = ((dwCRC >> 8) & 0x00FFFFFFL) ^
                            gadwCRC32Table[(dwCRC ^ byCurByte) & 0x000000FFL];

                        wVBA5CRCState = WD_VBA5_CRC_STATE_NORMAL;
                    }
                    break;

                case WD_VBA5_CRC_STATE_SPACES:
                    switch (byCurByte)
                    {
                        case 0x20:
                            if (dwVBA5CRCSubState)
                                ++dwVBA5CRCSubState;
                            break;

                        case 0x0D:
                            // Assumption is that 0x0D always
                            //  precedes 0x0A
                            break;

                        case 0x0A:
                            // Saw a line consisting only of spaces

                            // Reset to start state

                            wVBA5CRCState = WD_VBA5_CRC_STATE_BOL;
                            break;

                        default:
                            // Found a non-space character in a supposed
                            //  space line

                            // CRC all the spaces and return to normal

                            while (dwVBA5CRCSubState-- != 0)
                            {
                                dwCRC = ((dwCRC >> 8) & 0x00FFFFFFL) ^
                                    gadwCRC32Table[(dwCRC ^ 0x20) & 0x000000FFL];
                            }

                            dwCRC = ((dwCRC >> 8) & 0x00FFFFFFL) ^
                                gadwCRC32Table[(dwCRC ^ byCurByte) & 0x000000FFL];

                            wVBA5CRCState = WD_VBA5_CRC_STATE_NORMAL;
                            break;
                    }
                    break;

                case WD_VBA5_CRC_STATE_NORMAL:
                    // CRC the byte

                    dwCRC = ((dwCRC >> 8) & 0x00FFFFFFL) ^
                        gadwCRC32Table[(dwCRC ^ byCurByte) & 0x000000FFL];

                    if (byCurByte == 0x0A)
                    {
                        // Assumption is that 0x0A always ends a line

                        wVBA5CRCState = WD_VBA5_CRC_STATE_BOL;
                    }
                    break;

                case WD_VBA5_CRC_STATE_SKIP_LINE:
                    // Skip the byte

                    if (byCurByte == 0x0A)
                    {
                        // Assumption is that 0x0A always ends a line

                        wVBA5CRCState = WD_VBA5_CRC_STATE_BOL;
                    }
                    break;

                default:
                    // This should never happen

                    break;
            }
        }

        // Increment run buf index

        ++i;
    }

    // Store the CRC

    lpstScan->dwCRC = dwCRC;

    //////////////////////////////////////////////////////////////////
    // Update CRC hit bit array
    //////////////////////////////////////////////////////////////////

    // See whether the CRC is in the list

    lLow = 0;
    lHigh = lpstSigSet->lNumCRCs - 1;
    while (lLow <= lHigh)
    {
        lMid = (lLow + lHigh) / 2;

        if (dwCRC < lpstSigSet->lpadwCRCs[lMid])
        {
            // In lower half

            lHigh = lMid - 1;
        }
        else
        if (dwCRC > lpstSigSet->lpadwCRCs[lMid])
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

    return(WD_STATUS_OK);
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
//  If the scan type is not WD7, the second CRC is also tried.
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

    // Try the first CRC

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
// Returns:
//  WD_STATUS_VIRUS_FOUND   If a virus signature matched
//  WD_STATUS_OK            If no virus signature matched
//
//*************************************************************************

WD_STATUS WDScanDoc
(
    LPWD_SIG_SET            lpstSigSet,
    LPWD_SCAN               lpstScan,
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo
)
{
    WORD                    wMacroIdx;
    MACRO_INFO_T            stMacroInfo;
    WD_GET_MACRO_INFO_T     stGetInfo;

    BOOL                    bMVPApproved = TRUE;

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

    lpstScan->uScan.stWD7.lpstMacroInfo = &stMacroInfo;
    stGetInfo.bUseIndex = TRUE;
    stGetInfo.wNextNameIndex = 0;
    for (wMacroIdx=0;
         wMacroIdx<lpstScan->uScan.stWD7.lpstMacroTableInfo->wNumMacros;
         wMacroIdx++)
    {
        if (WordDocGetMacroInfoAtIndex(lpstScan->lpstCallBack,
                                       &stGetInfo,
                                       lpstScan->lpstOLEStream,
                                       lpstScan->uScan.stWD7.lpstKey,
                                       lpstScan->uScan.stWD7.
                                           lpstMacroTableInfo,
                                       &stMacroInfo,
                                       lpstScan->abyRunBuf,
                                       (LPSTR)lpstScan->abyName) != WORDDOC_OK)
        {
            // Error getting this macro, go to the next one

            continue;
        }

        stGetInfo.bUseIndex = FALSE;

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

            bMVPApproved = FALSE;
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

    // Check MVP

    if (bMVPApproved == FALSE)
    {
        *lplpstVirusSigInfo = &gstMVPVirusSigInfo;
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

    if (VBA5LZNTModuleInit(lpstScan->lpstCallBack,
                           lpstScan->lpstOLEStream,
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



