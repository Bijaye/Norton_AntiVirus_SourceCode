////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/w97rmtxt.cpv   1.1   23 Apr 1998 15:34:12   DCHI  $
//
// Description:
//  Code to remove the text from Word 97 documents.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/w97rmtxt.cpv  $
// 
//    Rev 1.1   23 Apr 1998 15:34:12   DCHI
// Corrected unicode compressed text wiping.
// 
//    Rev 1.0   26 Mar 1998 18:36:26   DCHI
// Initial revision.
// 
//************************************************************************

#include "stdafx.h"
#include "o97api.h"

BOOL W97RemoveText
(
    LPSS_STREAM         lpstStream,
    LPSS_STREAM         lpstTableStream,
    LPW97FIB            lpstFIB
)
{
    DWORD               dwByteCount;
    DWORD               dwOffset;
    DWORD               dwSize;
    BYTE                byType;
    DWORD               dwBlockSize;
    WORD                wBlockSize;
    DWORD               dwNumPieces;
    DWORD               dwPieceNum;
    DWORD               dwCPStart;
    DWORD               dwCPEnd;
    DWORD               dwCPOffset;
    DWORD               dwFCOffset;
    BOOL                bUnicode;
    BYTE                abyBuf[512];
    DWORD               dwNumBytes;
    int                 i;

    // Get the offset of the complex file information

    dwOffset = DWENDIAN(lpstFIB->dwFCClx);

    // Get complex information size

    dwSize = DWENDIAN(lpstFIB->dwLCBClx);

    // Find the plcfpcd

    for(;;)
    {
        if (dwSize < sizeof(BYTE))
        {
            // Corruption in file?

            return(FALSE);
        }

        // Get the data block type

        if (SSSeekRead(lpstTableStream,
                       dwOffset,
                       (LPBYTE)&byType,
                       sizeof(BYTE),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE);
        dwSize -= sizeof(BYTE);

        // Get the block size

        if (byType == 1)
        {
            if (SSSeekRead(lpstTableStream,
                           dwOffset,
                           (LPBYTE)&wBlockSize,
                           sizeof(WORD),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(WORD))
                return(FALSE);

            dwBlockSize = WENDIAN(wBlockSize);

            dwOffset += sizeof(WORD) + dwBlockSize;
            dwSize -= sizeof(WORD) + dwBlockSize;
        }
        else
        if (byType == 2)
        {
            if (SSSeekRead(lpstTableStream,
                           dwOffset,
                           (LPBYTE)&dwBlockSize,
                           sizeof(DWORD),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(DWORD))
                return(FALSE);

            dwBlockSize = DWENDIAN(dwBlockSize);

            dwOffset += sizeof(DWORD);
            dwSize -= sizeof(DWORD);
        }
        else
        {
            // Corruption in file?

            return(FALSE);
        }

        if (dwBlockSize > dwSize)
        {
            // Corruption in file?

            return(FALSE);
        }

        if (byType == 2)
            break;
    }

    // dwOffset is now the offset of the plcfpcd
    // dwBlockSize is its size
    // (dwBlockSize - 4) / 12 is the number of pieces

    dwNumPieces = (dwBlockSize - 4) / 12;
    if (dwNumPieces * 12 + 4 != dwBlockSize)
    {
        // Corruption in file?

        return(FALSE);
    }

    // Read the starting character position

    if (SSSeekRead(lpstTableStream,
                   dwOffset,
                   (LPBYTE)&dwCPStart,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwCPStart = DWENDIAN(dwCPStart);

    // Set the CP and FC offsets

    dwCPOffset = dwOffset + sizeof(DWORD);

    dwFCOffset = dwOffset + dwNumPieces * 4 + 4 + 2;

    // Now iterate through each piece

    for (dwPieceNum=0;dwPieceNum<dwNumPieces;dwPieceNum++)
    {
        // Read the ending character position

        if (SSSeekRead(lpstTableStream,
                       dwCPOffset,
                       (LPBYTE)&dwCPEnd,
                       sizeof(DWORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(DWORD))
            return(FALSE);

        dwCPOffset += sizeof(DWORD);

        dwCPEnd = DWENDIAN(dwCPEnd);

        if (dwCPEnd < dwCPStart)
        {
            // Corruption in file?

            return(FALSE);
        }

        // Read the stream position of the piece

        if (SSSeekRead(lpstTableStream,
                       dwFCOffset,
                       (LPBYTE)&dwOffset,
                       sizeof(DWORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(DWORD))
            return(FALSE);

        dwFCOffset += 8;

        dwOffset = DWENDIAN(dwOffset);

        // See whether it is Unicode compressed (e.g., codepage-1252)

        if ((dwOffset & 0x40000000) != 0)
        {
            // It is Unicode compressed

            dwOffset &= 0xBFFFFFFF;
            dwOffset >>= 1;
            bUnicode = FALSE;
        }
        else
            bUnicode = TRUE;

        // We now have the offset and size of the piece

        dwSize = dwCPEnd - dwCPStart;

        /////////////////////////////////////////////////////////
        // Replace the piece with 0x0Ds

        if (bUnicode == TRUE)
        {
            dwSize *= 2;
            for (i=0;i<sizeof(abyBuf);i+=2)
            {
                abyBuf[i] = 0x0D;
                abyBuf[i+1] = 0;
            }
        }
        else
        {
            for (i=0;i<sizeof(abyBuf);i++)
                abyBuf[i] = 0x0D;
        }

        dwNumBytes = sizeof(abyBuf);
        while (dwSize != 0)
        {
            if (dwSize < dwNumBytes)
                dwNumBytes = dwSize;

            if (SSSeekWrite(lpstStream,
                            dwOffset,
                            (LPBYTE)&abyBuf[0],
                            dwNumBytes,
                            &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != dwNumBytes)
                return(FALSE);

            dwOffset += dwNumBytes;
            dwSize -= dwNumBytes;
        }

        // The CP of the end of the current piece is the
        //  CP of the start of the next piece

        dwCPStart = dwCPEnd;
    }

    return(TRUE);
}

BOOL W97ModifyClxInfo
(
    LPSS_STREAM         lpstTableStream,
    LPW97FIB            lpstFIB
)
{
    BYTE                abyClx[0x15] = {
                                         0x02,                    // 0x00
                                         0x10, 0x00, 0x00, 0x00,  // 0x01
                                         0x00, 0x00, 0x00, 0x00,  // 0x05
                                         0x01, 0x00, 0x00, 0x00,  // 0x09
                                         0x00, 0x00,              // 0x0D
                                         0x00, 0x00, 0x00, 0x00,  // 0x0F
                                         0x00, 0x00               // 0x13
                                       };

    DWORD               dwOffset;
    DWORD               dwSize;

    abyClx[0x0F] = ((LPBYTE)&lpstFIB->dwFCMin)[0];
    abyClx[0x10] = ((LPBYTE)&lpstFIB->dwFCMin)[1];
    abyClx[0x11] = ((LPBYTE)&lpstFIB->dwFCMin)[2];
    abyClx[0x12] = ((LPBYTE)&lpstFIB->dwFCMin)[3];

    abyClx[0x12] <<= 1;
    if (abyClx[0x11] & 0x80)
        abyClx[0x12] |= 0x01;
    abyClx[0x11] <<= 1;
    if (abyClx[0x10] & 0x80)
        abyClx[0x11] |= 0x01;
    abyClx[0x10] <<= 1;
    if (abyClx[0x0F] & 0x80)
        abyClx[0x10] |= 0x01;
    abyClx[0x0F] <<= 1;
    abyClx[0x12] |= 0x40;

    // Write the new complex info

    dwOffset = DWENDIAN(lpstFIB->dwFCClx);
    if (SSSeekWrite(lpstTableStream,
                    dwOffset,
                    abyClx,
                    sizeof(abyClx),
                    &dwSize) != SS_STATUS_OK ||
        dwSize != sizeof(abyClx))
        return(FALSE);

    // Set the new size of the clx info

    dwSize = DWENDIAN(sizeof(abyClx));
    lpstFIB->dwLCBClx = dwSize;

    return(TRUE);
}

BOOL W97ModifySED
(
    LPSS_STREAM         lpstTableStream,
    LPW97FIB            lpstFIB
)
{
    DWORD               dwOffset;
    DWORD               dwSize;
    DWORD               dwByteCount;
    BYTE                abySED[0x14] = { 0x00, 0x00, 0x00, 0x00,
                                         0x01, 0x00, 0x00, 0x00,
                                         0x03, 0x00,
                                         0xFF, 0xFF, 0xFF, 0xFF,
                                         0x00, 0x00,
                                         0xFF, 0xFF, 0xFF, 0xFF };


    dwOffset = DWENDIAN(lpstFIB->dwFCPlcfsed);
    dwSize = DWENDIAN(lpstFIB->dwLCBPlcfsed);

    if (dwSize < sizeof(abySED))
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Modify to have only one section

    if (SSSeekWrite(lpstTableStream,
                    dwOffset,
                    (LPBYTE)abySED,
                    sizeof(abySED),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(abySED))
        return(FALSE);

    dwSize = sizeof(abySED);
    lpstFIB->dwLCBPlcfsed = DWENDIAN(dwSize);

    return(TRUE);
}

BOOL W97ModifyCHP
(
    LPSS_STREAM         lpstStream,
    LPSS_STREAM         lpstTableStream,
    LPW97FIB            lpstFIB
)
{
    DWORD               dwOffset;
    DWORD               dwSize;
    DWORD               dwNumFKPs;
    DWORD               dwFCLimit;
    DWORD               dwPN;
    BYTE                byTemp;
    DWORD               dwByteCount;

    dwOffset = DWENDIAN(lpstFIB->dwFCPlcfbteChpx);
    dwSize = DWENDIAN(lpstFIB->dwLCBPlcfbteChpx);

    dwNumFKPs = (dwSize - sizeof(DWORD)) / (sizeof(DWORD) + sizeof(DWORD));

    if (dwNumFKPs == 0)
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Read the limit FC and the PN of the first CHP FKP

    // Read the first FC

    if (SSSeekRead(lpstTableStream,
                   dwOffset,
                   (LPBYTE)&dwFCLimit,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwFCLimit = DWENDIAN(dwFCLimit);
    ++dwFCLimit;
    dwFCLimit = DWENDIAN(dwFCLimit);

    dwOffset += dwNumFKPs * sizeof(DWORD);
    dwOffset += sizeof(DWORD);

    // Read the PN of the first CHP FKP

    if (SSSeekRead(lpstTableStream,
                   dwOffset,
                   (LPBYTE)&dwPN,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Modify the bintable to have only one FC range

    dwOffset = DWENDIAN(lpstFIB->dwFCPlcfbteChpx) + sizeof(DWORD);

    // Write the limit of the first CHP

    if (SSSeekWrite(lpstTableStream,
                    dwOffset,
                    (LPBYTE)&dwFCLimit,
                    sizeof(DWORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwOffset += sizeof(DWORD);

    // Write the PN of the first CHP FKP

    if (SSSeekWrite(lpstTableStream,
                    dwOffset,
                    (LPBYTE)&dwPN,
                    sizeof(DWORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Modify the first PN to have dwFCFirst, dwFCLimit, (BYTE)0
    //  at the beginning and (BYTE)1 at the end

    dwPN = DWENDIAN(dwPN);

    dwOffset = dwPN * (DWORD)0x200 + sizeof(DWORD);

    // Write dwFCLimit

    if (SSSeekWrite(lpstStream,
                    dwOffset,
                    (LPBYTE)&dwFCLimit,
                    sizeof(DWORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwOffset += sizeof(DWORD);

    // Write (BYTE)0

    byTemp = 0;

    if (SSSeekWrite(lpstStream,
                    dwOffset,
                    (LPBYTE)&byTemp,
                    sizeof(BYTE),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(BYTE))
        return(FALSE);

    // Write (BYTE)1;

    dwOffset = (dwPN + 1) * (DWORD)0x200 - 1;
    byTemp = 1;
    if (SSSeekWrite(lpstStream,
                    dwOffset,
                    (LPBYTE)&byTemp,
                    sizeof(BYTE),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(BYTE))
        return(FALSE);

    // Set the size of the CHP bintable

    lpstFIB->dwLCBPlcfbteChpx = 0xC;
    lpstFIB->dwLCBPlcfbteChpx = DWENDIAN(lpstFIB->dwLCBPlcfbteChpx);

    return(TRUE);
}

BOOL W97ModifyPAP
(
    LPSS_STREAM         lpstStream,
    LPSS_STREAM         lpstTableStream,
    LPW97FIB            lpstFIB

)
{
    DWORD               dwOffset;
    DWORD               dwSize;
    DWORD               dwNumFKPs;
    DWORD               dwFCLimit;
    DWORD               dwPN;
    BYTE                byTemp;
    int                 i;
    DWORD               dwByteCount;

    dwOffset = DWENDIAN(lpstFIB->dwFCPlcfbtePapx);
    dwSize = DWENDIAN(lpstFIB->dwLCBPlcfbtePapx);

    dwNumFKPs = (dwSize - sizeof(DWORD)) / (sizeof(DWORD) + sizeof(DWORD));

    if (dwNumFKPs == 0)
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Read the limit FC and the PN of the first CHP FKP

    // Read the first FC

    if (SSSeekRead(lpstTableStream,
                   dwOffset,
                   (LPBYTE)&dwFCLimit,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwFCLimit = DWENDIAN(dwFCLimit);
    ++dwFCLimit;
    dwFCLimit = DWENDIAN(dwFCLimit);

    dwOffset += dwNumFKPs * sizeof(DWORD);
    dwOffset += sizeof(DWORD);

    // Read the PN of the first CHP FKP

    if (SSSeekRead(lpstTableStream,
                   dwOffset,
                   (LPBYTE)&dwPN,
                   sizeof(DWORD),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Modify the bintable to have only one FC range

    dwOffset = DWENDIAN(lpstFIB->dwFCPlcfbtePapx) + sizeof(DWORD);

    // Write the limit of the first CHP

    if (SSSeekWrite(lpstTableStream,
                    dwOffset,
                    (LPBYTE)&dwFCLimit,
                    sizeof(DWORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwOffset += sizeof(DWORD);

    // Write the PN of the first CHP FKP

    if (SSSeekWrite(lpstTableStream,
                    dwOffset,
                    (LPBYTE)&dwPN,
                    sizeof(DWORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Modify the first PN to have dwFCFirst, dwFCLimit, (BYTE)0
    //  at the beginning and (BYTE)1 at the end

    dwPN = DWENDIAN(dwPN);

    dwOffset = dwPN * (DWORD)0x200 + sizeof(DWORD);

    // Write dwFCLimit

    if (SSSeekWrite(lpstStream,
                    dwOffset,
                    (LPBYTE)&dwFCLimit,
                    sizeof(DWORD),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(DWORD))
        return(FALSE);

    dwOffset += sizeof(DWORD);

    // Write (BYTE)0 seven times

    byTemp = 0;
    for (i=0;i<7;i++)
    {
        if (SSSeekWrite(lpstStream,
                        dwOffset,
                        (LPBYTE)&byTemp,
                        sizeof(BYTE),
                        &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(BYTE))
            return(FALSE);

        ++dwOffset;
    }

    // Write (BYTE)1;

    dwOffset = (dwPN + 1) * (DWORD)0x200 - 1;
    byTemp = 1;
    if (SSSeekWrite(lpstStream,
                    dwOffset,
                    (LPBYTE)&byTemp,
                    sizeof(BYTE),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(BYTE))
        return(FALSE);

    // Set the size of the PAP bintable

    lpstFIB->dwLCBPlcfbtePapx = 0xC;
    lpstFIB->dwLCBPlcfbtePapx = DWENDIAN(lpstFIB->dwLCBPlcfbtePapx);

    return(TRUE);
}

//*************************************************************************
//
// Function:
//  BOOL W97ClearDocument()
//
// Parameters:
//  lpstStream          Ptr to WordDocument stream
//  lpstTableStream     Ptr to the associated Table stream
//  lpstFIB             FIB of the document
//  dwFIBOffset         Offset of the FIB
//
// Description:
//  Clears text from the given Word 97 document.  The function
//  leaves autotext and document variables.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//*************************************************************************

BOOL W97ClearDocument
(
    LPSS_STREAM         lpstStream,
    LPSS_STREAM         lpstTableStream,
    LPW97FIB            lpstFIB,
    DWORD               dwFIBOffset
)
{
    DWORD               dwByteCount;
    DWORD               dwTemp;
    DWORD               dwSize;
    DWORD               dwOffset;

    if (W97RemoveText(lpstStream,
                      lpstTableStream,
                      lpstFIB) == FALSE)
        return(FALSE);

    if (W97ModifyClxInfo(lpstTableStream,
                         lpstFIB) == FALSE)
        return(FALSE);

    if (W97ModifySED(lpstTableStream,
                     lpstFIB) == FALSE)
        return(FALSE);

    if (W97ModifyCHP(lpstStream,
                     lpstTableStream,
                     lpstFIB) == FALSE)
        return(FALSE);

    if (W97ModifyPAP(lpstStream,
                     lpstTableStream,
                     lpstFIB) == FALSE)
        return(FALSE);

    // wIdent;                 //   0   0: Magic number
    // wFIB;                   //   2   2: FIB version
    // wProduct;               //   4   4: Product version
    // wLID;                   //   6   6: Language stamp
    // wPN;                    //   8   8:

    //////////////////////////////////////////////////////////////////
    // Set to non-complex

    // byFlags0;               //  10   A:

    lpstFIB->byFlags0 &= ~(BYTE)W97FIB_FLAGS0_COMPLEX;

    // byFlags1;               //  11   B:
    // wFIBBack;               //  12   C:
    // dwKey;                  //  14   E: File encrypted key
    // byEnvr;                 //  18  12: Creation environment
    // byFlagsMac;             //  19  13:
    // wChSe;                  //  20  14: Default ext text char set
    // wChSeTables;            //  22  16: Default ext data char set

    //////////////////////////////////////////////////////////////////
    // Set to have only one character

    // dwFCMin;                //  24  18: First char file offset
    // dwFCMac;                //  28  1C: Last char file offset

    dwTemp = DWENDIAN(lpstFIB->dwFCMin);
    ++dwTemp;
    dwTemp = DWENDIAN(dwTemp);
    lpstFIB->dwFCMac = dwTemp;

    // wCSW;                   //  32  20: Count of fields in "shorts"
    // wMagicCreated;          //  34  22: File creator
    // wMagicRevised;          //  36  24: File's last modifier
    // wMagicCreatedPrivate;   //  38  26: Private data
    // wMagicRevisedPrivate;   //  40  28: Private data
    // wPNFbpChpFirst_W6;      //  42  2A: Not used
    // wPNChpFirst_W6;         //  44  2C: Not used
    // wCPNBteChp_W6;          //  46  2E: Not used
    // wPNFbpPapFirst_W6;      //  48  30: Not used
    // wPNPapFirst_W6;         //  50  32: Not used
    // wCPNBtePap_W6;          //  52  34: Not used
    // wPNFbpLvcFirst_W6;      //  54  36: Not used
    // wPNLvcFirst_W6;         //  56  38: Not used
    // wCPNBteLvc_W6;          //  58  3A: Not used
    // wLIDFE;                 //  60  3C: Far East language ID

    // wCLW;                   //  62  3E: Count of fields in "longs"

    // dwCBMac;                //  64  40: Last byte file offset
    // dwProductCreated;       //  68  44: Build date of creator
    // dwProductRevised;       //  72  48: Build date of modifier

    // dwCCPText;              //  76  4C: Length of main doc text
    // dwCCPFtn;               //  80  50: Length of footnote text
    // dwCCPHdd;               //  84  54: Length of header text
    // dwCCPMcr;               //  88  58: Length of macro text
    // dwCCPAtn;               //  92  5C: Length of annotation text
    // dwCCPEdn;               //  96  60: Length of endnote text
    // dwCCPTxbx;              // 100  64: Length of textbox text
    // dwCCPHdrTxbx;           // 104  68: Length of hdr textbox text

    dwTemp = DWENDIAN((DWORD)1);
    lpstFIB->dwCCPText = dwTemp;
    lpstFIB->dwCCPFtn = 0;
    lpstFIB->dwCCPHdd = 0;
    lpstFIB->dwCCPMcr = 0;
    lpstFIB->dwCCPAtn = 0;
    lpstFIB->dwCCPEdn = 0;
    lpstFIB->dwCCPTxbx = 0;
    lpstFIB->dwCCPHdrTxbx = 0;

    // dwPNFbpChpFirst;        // 108  6C: First plcfbte block
    // dwPNChpFirst;           // 112  70: First page w/CHPX FKP info
    // dwCPNBteChp;            // 116  74: CHPX FKP count
    // dwPNFbpPapFirst;        // 120  78: First plcfbte block
    // dwPNPapFirst;           // 124  7C: First page w/PAPX FKP info
    // dwCPNBtePap;            // 128  80: PAPX FKP count
    // dwPNFbpLvcFirst;        // 132  84: First plcfbte block
    // dwPNLvcFirst;           // 136  88: First page w/LVC FKP info
    // dwCPNBteLvc;            // 140  8C: LVC FKP count

    dwTemp = 0x000FFFFF;
    dwTemp = DWENDIAN(dwTemp);
    lpstFIB->dwPNFbpChpFirst = dwTemp;
    lpstFIB->dwPNChpFirst = 0;
    lpstFIB->dwCPNBteChp = 0;
    lpstFIB->dwPNFbpPapFirst = dwTemp;
    lpstFIB->dwPNPapFirst = 0;
    lpstFIB->dwCPNBtePap = 0;
    lpstFIB->dwPNLvcFirst = dwTemp;
    lpstFIB->dwPNLvcFirst = 0;
    lpstFIB->dwCPNBteLvc = 0;

    // dwFCIslandFirst;        // 144  90:
    // dwFCIslandLim;          // 148  94:

    // wCFCLCB;                // 152  98: Count of FC/LCB pairs.

    // dwFCStshfOrig;          // 154  9A: Orig STSH offset
    // dwLCBStshfOrig;         // 158  9E: Orig STSH size
    // dwFCStshf;              // 162  A2: STSH offset
    // dwLCBStshf;             // 166  A6: STSH size

    // dwFCPlcffndRef;         // 170  AA: Footnote ref PLC offset
    // dwLCBPlcffndRef;        // 174  AE: Footnote ref PLC size
    // dwFCPlcffndTxt;         // 178  B2: Footnote text PLC offset
    // dwLCBPlcffndTxt;        // 182  B6: Footnote text PLC size
    // dwFCPlcfandRef;         // 186  BA: Annotation ref PLC offset
    // dwLCBPlcfandRef;        // 190  BE: Annotation ref PLC size
    // dwFCPlcfandTxt;         // 194  C2: Annotation text PLC offset
    // dwLCBPlcfandTxt;        // 198  C6: Annotation text PLC size

    lpstFIB->dwLCBPlcffndRef = 0;
    lpstFIB->dwLCBPlcffndTxt = 0;
    lpstFIB->dwLCBPlcfandRef = 0;
    lpstFIB->dwLCBPlcfandTxt = 0;

    // dwFCPlcfsed;            // 202  CA: Section desc PLC offset
    // dwLCBPlcfsed;           // 206  CE: Section desc PLC size

    // dwFCPlcpad;             // 210  D2: No longer used
    // dwLCBPlcpad;            // 214  D6: No longer used
    // dwFCPlcfphe;            // 218  DA: Paragraph height PLC offset
    // dwLCBPlcfphe;           // 222  DE: Paragraph height PLC size

    lpstFIB->dwLCBPlcfphe = 0;

    // dwFCSttbfglsy;          // 226  E2: Glossary str table offset
    // dwLCBSttbfglsy;         // 230  E6: Glossary str table size
    // dwFCPlcfglsy;           // 234  EA: Glossary PLC offset
    // dwLCBPlcfglsy;          // 238  EE: Glossary PLC size

    // dwFCPlcfhdd;            // 242  F2: Header PLC offset
    // dwLCBPlcfhdd;           // 246  F6: Header PLC size

    lpstFIB->dwLCBPlcfhdd = 0;

    // dwFCPlcfbteChpx;        // 250  FA: Char prop bin table offset
    // dwLCBPlcfbteChpx;       // 254  FE: Char prop bin table size
    // dwFCPlcfbtePapx;        // 258 102: Para prop bin table offset
    // dwLCBPlcfbtePapx;       // 262 106: Para prop bin table size

    // dwFCPlcfsea;            // 266 10A: Private PLC offset
    // dwLCBPlcfsea;           // 270 10E: Private PLC size
    // dwFCSttbfffn;           // 274 112: Font info table offset
    // dwLCBSttbfffn;          // 278 116: Font info table size

    //////////////////////////////////////////////////////////////////
    // Eliminate fields

    // dwFCPlcffldMom;         // 282 11A: Main doc field PLC offset
    // dwLCBPlcffldMom;        // 286 11E: Main doc field PLC size
    // dwFCPlcffldHdr;         // 290 122: Header field PLC offset
    // dwLCBPlcffldHdr;        // 294 126: Header field PLC size
    // dwFCPlcffldFtn;         // 298 12A: Footnote field PLC offset
    // dwLCBPlcffldFtn;        // 302 12E: Footnote field PLC size
    // dwFCPlcffldAtn;         // 306 132: Annotation field PLC offset
    // dwLCBPlcffldAtn;        // 310 136: Annotation field PLC size

    lpstFIB->dwLCBPlcffldMom = 0;       // Main document field PLC size
    lpstFIB->dwLCBPlcffldHdr = 0;       // Header field PLC size
    lpstFIB->dwLCBPlcffldFtn = 0;       // Footnote field PLC size
    lpstFIB->dwLCBPlcffldAtn = 0;       // Annotation field PLC size

    // dwFCPlcffldMcr;         // 314 13A: No longer used
    // dwLCBPlcffldMcr;        // 318 13E: No longer used

    //////////////////////////////////////////////////////////////////
    // Bookmarks to zero

    // dwFCSttbfbkmk;          // 322 142: Bookmark name table offset
    // dwLCBSttbfbkmk;         // 326 146: Bookmark name table size
    // dwFCPlcfbkf;            // 330 14A: Bookmark begin PLC offset
    // dwLCBPlcfbkf;           // 334 14E: Bookmark begin PLC size
    // dwFCPlcfbkl;            // 338 152: Bookmark end PLC offset
    // dwLCBPlcfbkl;           // 342 156: Bookmark end PLC size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfbkmk);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfbkmk);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfbkmk = 0;
    lpstFIB->dwLCBPlcfbkf = 0;
    lpstFIB->dwLCBPlcfbkl = 0;

    // dwFCCmds;               // 346 15A: Macro commands offset
    // dwLCBCmds;              // 350 15E: Macro commands size
    // dwFCPlcmcr;             // 354 162: No longer used
    // dwLCBPlcmcr;            // 358 166: No longer used
    // dwFCSttbfmcr;           // 362 16A: No longer used
    // dwLCBSttbfmcr;          // 366 16E: No longer used
    // dwFCPrDrvr;             // 370 172: Printer driver info offset
    // dwLCBPrDrvr;            // 374 176: Printer driver info size
    // dwFCPrEnvPort;          // 378 17A: Portrait print env offset
    // dwLCBPrEnvPort;         // 382 17E: Portrait print env size
    // dwFCPrEnvLand;          // 386 182: Landscape print env offset
    // dwLCBPrEnvLand;         // 390 186: Landscape print env size
    // dwFCWss;                // 394 18A: Window save state offset
    // dwLCBWss;               // 398 18E: Window save state size
    // dwFCDop;                // 402 192: Document property offset
    // dwLCBDop;               // 406 196: Document property size

    //////////////////////////////////////////////////////////////////
    // Associated strings to zero

    // dwFCSttbfAssoc;         // 410 19A: Assoc str table offset
    // dwLCBSttbfAssoc;        // 414 19E: Assoc str table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfAssoc);
    if (dwSize > 2)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfAssoc);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset + 2,
                          dwSize - 2) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfAssoc = 0;

    // dwFCClx;                // 418 1A2: Complex file info offset
    // dwLCBClx;               // 422 1A6: Complex file info size

    // dwFCPlcfpgdFtn;         // 426 1AA: Not used
    // dwLCBPlcfpgdFtn;        // 430 1AE: Not used

    //////////////////////////////////////////////////////////////////
    // Original filename to zero

    // dwFCAutosaveSource;     // 434 1B2: Orig file name offset
    // dwLCBAutosaveSource;    // 438 1B6: Orig file name size

    dwSize = DWENDIAN(lpstFIB->dwLCBAutosaveSource);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCAutosaveSource);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBAutosaveSource = 0;

    //////////////////////////////////////////////////////////////////
    // Annotation owners to zero

    // dwFCGrpXstAtnOwners;    // 442 1BA: Annotation owners offset
    // dwLCBGrpXstAtnOwners;   // 446 1BE: Annotation owners size

    dwSize = DWENDIAN(lpstFIB->dwLCBGrpXstAtnOwners);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCGrpXstAtnOwners);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBGrpXstAtnOwners = 0;

    //////////////////////////////////////////////////////////////////
    // Annotation bookmark names to zero

    // dwFCSttbfAtnbkmk;       // 450 1C2: Annotation bkmk name offset
    // dwLCBSttbfAtnbkmk;      // 454 1C6: Annotation bkmk name size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfAtnbkmk);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfAtnbkmk);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfAtnbkmk = 0;

    // dwFCPlcdoaMom;          // 458 1CA: No longer used
    // dwLCBPlcdoaMom;         // 462 1CE: No longer used
    // dwFCPlcdoaHdr;          // 466 1D2: No longer used
    // dwLCBPlcdoaHdr;         // 470 1D6: No longer used

    // dwFCPlcspaMom;          // 474 1DA: Main doc FSPA PLC offset
    // dwLCBPlcspaMom;         // 478 1DE: Main doc FSPA PLC size
    // dwFCPlcspaHdr;          // 482 1E2: Header FSPA PLC offset
    // dwLCBPlcspaHdr;         // 486 1E6: Header FSPA PLC size

    lpstFIB->dwLCBPlcspaMom = 0;
    lpstFIB->dwLCBPlcspaHdr = 0;

    //////////////////////////////////////////////////////////////////
    // Annotation bookmarks to zero

    // dwFCPlcfAtnbkf;         // 490 1EA: Annotation BKF PLC offset
    // dwLCBPlcfAtnbkf;        // 494 1EE: Annotation BKF PLC size
    // dwFCPlcfAtnbkl;         // 498 1F2: Annotation BKL PLC offset
    // dwLCBPlcfAtnbkl;        // 502 1F6: Annotation BKL PLC size

    lpstFIB->dwLCBPlcfAtnbkf = 0;
    lpstFIB->dwLCBPlcfAtnbkl = 0;

    // dwFCPms;                // 506 1FA: Print merge state offset
    // dwLCBPms;               // 510 1FE: Print merge state size

    //////////////////////////////////////////////////////////////////
    // Form field table to zero

    // dwFCFormFldSttbs;       // 514 202: Form field table offset
    // dwLCBFormFldSttbs;      // 518 206: Form field table size

    dwSize = DWENDIAN(lpstFIB->dwLCBFormFldSttbs);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCFormFldSttbs);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBFormFldSttbs = 0;

    //////////////////////////////////////////////////////////////////
    // Set endnote table sizes to zero

    // dwFCPlcfendRef;         // 522 20A: Endnote ref PLC offset
    // dwLCBPlcfendRef;        // 526 20E: Endnote ref PLC size
    // dwFCPlcfendTxt;         // 530 212: Endnote text PLC offset
    // dwLCBPlcfendTxt;        // 534 216: Endnote text PLC size

    lpstFIB->dwLCBPlcfendRef = 0;
    lpstFIB->dwLCBPlcfendTxt = 0;

    //////////////////////////////////////////////////////////////////
    // Eliminate endnote fields

    // dwFCPlcffldEdn;         // 538 21A: Endnote field PLC offset
    // dwLCBPlcffldEdn;        // 542 21E: Endnote field PLC size

    lpstFIB->dwLCBPlcffldEdn = 0;       // Endnote field PLC size

    // dwFCPlcfpgdEdn;         // 546 222: Not used
    // dwLCBPlcfpgdEdn;        // 550 226: Not used

    // dwFCDggInfo;            // 554 22A: Art object table offset
    // dwLCBDggInfo;           // 558 22E: Art object table size

    lpstFIB->dwLCBDggInfo = 0;

    //////////////////////////////////////////////////////////////////
    // Author abbreviation table to zero

    // dwFCSttbfRMark;         // 562 232: Author abbrev table offset
    // dwLCBSttbfRMark;        // 566 236: Author abbrev table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfRMark);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfRMark);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfRMark = 0;

    //////////////////////////////////////////////////////////////////
    // Caption titles table to zero

    // dwFCSttbCaption;        // 570 23A: Caption titles table offset
    // dwLCBSttbCaption;       // 574 23E: Caption titles table size
    // dwFCSttbAutoCaption;    // 578 242: Auto caption table offset
    // dwLCBSttbAutoCaption;   // 582 246: Auto caption table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbCaption);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbCaption);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbCaption = 0;

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbAutoCaption);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbAutoCaption);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbAutoCaption = 0;

    //////////////////////////////////////////////////////////////////
    // Contributing docs to zero

    // dwFCPlcfwkb;            // 586 24A: Contrib doc PLC offset
    // dwLCBPlcfwkb;           // 590 24E: Contrib doc PLC size

    lpstFIB->dwLCBPlcfwkb = 0;

    // dwFCPlcfspl;            // 594 252: Spell check PLC offset
    // dwLCBPlcfspl;           // 598 256: Spell check PLC size

    lpstFIB->dwLCBPlcfspl = 0;

    // dwFCPlcftxbxTxt;        // 602 25A: Textbox text PLC offset
    // dwLCBPlcftxbxTxt;       // 606 25E: Textbox text PLC size

    lpstFIB->dwLCBPlcftxbxTxt = 0;

    // dwFCPlcffldTxbx;        // 610 262: Textbox field PLC offset
    // dwLCBPlcffldTxbx;       // 614 266: Textbox field PLC size

    lpstFIB->dwLCBPlcffldTxbx = 0;      // Textbox field PLC size

    // dwFCPlcfhdrtxbxTxt;     // 618 26A: Header textbox PLC offset
    // dwLCBPlcfhdrtxbxTxt;    // 622 26E: Header textbox PLC size

    lpstFIB->dwLCBPlcfhdrtxbxTxt = 0;

    // dwFCPlcffldHdrTxbx;     // 626 272: Hdr txtbox field PLC offset
    // dwLCBPlcffldHdrTxbx;    // 630 276: Hdr txtbox field PLC size

    lpstFIB->dwLCBPlcffldHdrTxbx = 0;   // Header textbox field PLC size

    // dwFCStwUser;            // 634 27A: Macro user storage offset
    // dwLCBStwUser;           // 638 27E: Macro user storage size
    // dwFCSttbttmbd;          // 642 282: Embedded TTF table offset
    // dwcbSttbttmbd;          // 646 286: Embedded TTF table size
    // dwFCUnused;             // 650 28A:
    // dwLCBUnused;            // 654 28E:

    // dwFCPgdMother;          // 658 292: Main text PGD PLC offset
    // dwLCBPgdMother;         // 662 296: Main text PGD PLC size
    // dwFCBkdMother;          // 666 29A: Main text BKD PLC offset
    // dwLCBBkdMother;         // 670 29E: Main text BKD PLC size
    // dwFCPgdFtn;             // 674 2A2: Footnote PGD PLC offset
    // dwLCBPgdFtn;            // 678 2A6: Footnote PGD PLC size
    // dwFCBkdFtn;             // 682 2AA: Footnote BKD PLC offset
    // dwLCBBkdFtn;            // 686 2AE: Footnote BKD PLC size
    // dwFCPgdEdn;             // 690 2B2: Endnote PGD PLC offset
    // dwLCBPgdEdn;            // 694 2B6: Endnote PGD PLC size
    // dwFCBkdEdn;             // 698 2BA: Endnote BKD PLC offset
    // dwLCBBkdEdn;            // 702 2BE: Endnote BKD PLC size

    lpstFIB->dwLCBPgdMother = 0;
    lpstFIB->dwLCBBkdMother = 0;
    lpstFIB->dwLCBPgdFtn = 0;
    lpstFIB->dwLCBBkdFtn = 0;
    lpstFIB->dwLCBPgdEdn = 0;
    lpstFIB->dwLCBBkdEdn = 0;

    //////////////////////////////////////////////////////////////////
    // Field keyword table to zero

    // dwFCSttbfIntlFld;       // 706 2C2: Field keyword table offset
    // dwLCBSttbfIntlFld;      // 710 2C6: Field keyword table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfIntlFld);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfIntlFld);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfIntlFld = 0;

    //////////////////////////////////////////////////////////////////
    // Routing slip to zero

    // dwFCRouteSlip;          // 714 2CA: Mailer route slip offset
    // dwLCBRouteSlip;         // 718 2CE: Mailer route slip size

    dwSize = DWENDIAN(lpstFIB->dwLCBRouteSlip);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCRouteSlip);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBRouteSlip = 0;

    //////////////////////////////////////////////////////////////////
    // Saved by to zero

    // dwFCSttbSavedBy;        // 722 2D2: Saved by users table offset
    // dwLCBSttbSavedBy;       // 726 2D6: Saved by users table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbSavedBy);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbSavedBy);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbSavedBy = 0;

    //////////////////////////////////////////////////////////////////
    // Filename reference table to zero

    // dwFCSttbFnm;            // 730 2DA: Filename ref table offset
    // dwLCBSttbFnm;           // 734 2DE: Filename ref table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbFnm);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbFnm);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbFnm = 0;

    // dwFCPlcfLst;            // 738 2E2: List format PLC offset
    // dwLCBPlcfLst;           // 742 2E6: List format PLC size
    // dwFCPlfLfo;             // 746 2EA: Lst fmt override PLC offset
    // dwLCBPlfLfo;            // 750 2EE: Lst fmt override PLC size
    // dwFCPlcftxbxBkd;        // 754 2F2: Mn doc txbx BKD PLC offset
    // dwLCBPlcftxbxBkd;       // 758 2F6: Mn doc txbx BKD PLC size
    // dwFCPlcftxbxHdrBkd;     // 762 2FA: Header txbx BKD PLC offset
    // dwLCBPlcftxbxHdrBkd;    // 766 2FE: Header txbx BKD PLC size

    lpstFIB->dwLCBPlcfLst = 0;
    lpstFIB->dwLCBPlfLfo = 0;
    lpstFIB->dwLCBPlcftxbxBkd = 0;
    lpstFIB->dwLCBPlcftxbxHdrBkd = 0;

    // dwFCDocUndo;            // 770 302: Main strm undo data offset
    // dwLCBDocUndo;           // 774 306:
    // dwFCRgbuse;             // 778 30A: Main strm undocumented
    // dwLCBRgbuse;            // 782 30E:
    // dwFCUsp;                // 786 312: Main strm undocumented
    // dwLCBUsp;               // 790 316:
    // dwFCUskf;               // 794 31A: Table strm undocumented
    // dwLCBUskf;              // 798 31E:
    // dwFCPlcupcRgbuse;       // 802 322: Table strm undocumented
    // dwLCBPlcupcRgbuse;      // 806 326:
    // dwFCPlcupcUsp;          // 810 32A: Table strm undocumented
    // dwLCBPlcupcUsp;         // 814 32E:

    lpstFIB->dwLCBDocUndo = 0;
    lpstFIB->dwLCBRgbuse = 0;
    lpstFIB->dwLCBUsp = 0;
    lpstFIB->dwLCBUskf = 0;
    lpstFIB->dwLCBPlcupcRgbuse = 0;
    lpstFIB->dwLCBPlcupcUsp = 0;

    // dwFCSttbGlsyStyle;      // 818 332: Glossary style table offset
    // dwLCBSttbGlsyStyle;     // 822 336: Glossary style table size
    // dwFCPlgosl;             // 826 33A: Grammar options offset
    // dwLCBPlgosl;            // 830 33E: Grammar options size
    // dwFCPlcocx;             // 834 342: OCX table offset
    // dwLCBPlcocx;            // 838 346: OCX table size

    lpstFIB->dwLCBSttbGlsyStyle = 0;
    lpstFIB->dwLCBPlgosl = 0;
    lpstFIB->dwLCBPlcocx = 0;

    // dwFCPlcfbteLvc;         // 842 34A: Char prop bin table offset
    // dwLCBPlcfbteLvc;        // 846 34E: Char prop bin table size

    // dwLowDateTime;          // 850 352:
    // dwHighDateTime;         // 854 356:

    // dwFCPlcflvc;            // 858 35A: LVC PLC offset
    // dwLCBPlcflvc;           // 862 35E: LVC PLC size

    lpstFIB->dwLCBPlcflvc = 0;

    // dwFCPlcasumy;           // 866 362: Autosummary PLC offset
    // dwLCBPlcasumy;          // 870 366: Autosummary PLC size
    // dwFCPlcfgram;           // 874 36A: Grammar state PLC offset
    // dwLCBPlcfgram;          // 878 36E: Grammar state PLC size

    lpstFIB->dwLCBPlcasumy = 0;
    lpstFIB->dwLCBPlcfgram = 0;

    // dwFCSttbListNames;      // 882 372: List name table offset
    // dwLCBSttbListNames;     // 886 376: List name table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbListNames);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbListNames);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbListNames = 0;

    // dwFCSttbfUssr;          // 890 37A: Table strm undocumented
    // dwLCBSttbfUssr;         // 894 37E:

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfUssr);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfUssr);
        if (SSWriteZeroes(lpstTableStream,
                          dwOffset,
                          dwSize) != SS_STATUS_OK)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfUssr = 0;

    //////////////////////////////////////////////////////////////////
    // Write back the modified FIB

    if (SSSeekWrite(lpstStream,
                    dwFIBOffset,
                    (LPBYTE)lpstFIB,
                    sizeof(W97FIB_T),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(W97FIB_T))
        return(FALSE);

    return(TRUE);
}

