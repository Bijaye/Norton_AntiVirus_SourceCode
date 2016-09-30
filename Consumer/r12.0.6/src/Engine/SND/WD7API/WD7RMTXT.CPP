//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/wd7rmtxt.cpv   1.1   26 Mar 1998 18:40:42   DCHI  $
//
// Description:
//  Code to remove the text from Word 6.0/95 documents.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/wd7rmtxt.cpv  $
// 
//    Rev 1.1   26 Mar 1998 18:40:42   DCHI
// Removed WD7EncryptedWriteByte().
// 
//    Rev 1.0   26 Mar 1998 18:36:22   DCHI
// Initial revision.
// 
//************************************************************************

#include "wd7api.h"

BOOL WDRemoveTextPiece95
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwOffset,
    DWORD               dwSize,
    BOOL                bExtChar
)
{
    BYTE                abyBuf[512];
    DWORD               dwNumBytes;
    int                 i;

    if (bExtChar == TRUE)
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

        if (WD7EncryptedWrite(lpstStream,
                              lpstKey,
                              dwOffset,
                              (LPBYTE)&abyBuf[0],
                              dwNumBytes) != dwNumBytes)
            return(FALSE);

        dwOffset += dwNumBytes;
        dwSize -= dwNumBytes;
    }

    return(TRUE);
}

BOOL WDRemoveNonComplexText95
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7FIB            lpstFIB,
    BOOL                bExtChar
)
{
    DWORD               dwOffset;
    DWORD               dwSize;

    // Get the offset of the start of the text

    dwOffset = DWENDIAN(lpstFIB->dwFCMin);

    // Get the offset of the BYTE following the last
    //  character of text

    dwSize = DWENDIAN(lpstFIB->dwFCMac);

    // To get the size, just subtract the beginning offset

    dwSize -= dwOffset;

    // Now space out the text

    return WDRemoveTextPiece95(lpstStream,
                               lpstKey,
                               dwOffset,
                               dwSize,
                               bExtChar);
}

BOOL WDRemoveComplexText95
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7FIB            lpstFIB,
    BOOL                bExtChar
)
{
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

    // Get the offset of the complex file information

    dwOffset = DWENDIAN(lpstFIB->dwFCClx);

    // Get complex information size

    dwSize = DWENDIAN(lpstFIB->dwLCBClx);

    // Find the plcfpcd

    while (1)
    {
        if (dwSize < sizeof(BYTE))
        {
            // Corruption in file?

            return(FALSE);
        }

        // Get the data block type

        if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwOffset,
                             (LPBYTE)&byType,
                             sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        dwOffset += sizeof(BYTE);
        dwSize -= sizeof(BYTE);

        // Get the block size

        if (byType == 1)
        {
            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&wBlockSize,
                                 sizeof(WORD)) != sizeof(WORD))
                return(FALSE);

            dwBlockSize = WENDIAN(wBlockSize);

            dwOffset += sizeof(WORD) + dwBlockSize;
            dwSize -= sizeof(WORD) + dwBlockSize;
        }
        else
        if (byType == 2)
        {
            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&dwBlockSize,
                                 sizeof(DWORD)) != sizeof(DWORD))
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

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         dwOffset,
                         (LPBYTE)&dwCPStart,
                         sizeof(DWORD)) != sizeof(DWORD))
        return(FALSE);

    dwCPStart = DWENDIAN(dwCPStart);

    // Set the CP and FC offsets

    dwCPOffset = dwOffset + sizeof(DWORD);

    dwFCOffset = dwOffset + dwNumPieces * 4 + 4 + 2;

    // Now iterate through each piece

    for (dwPieceNum=0;dwPieceNum<dwNumPieces;dwPieceNum++)
    {
        // Read the ending character position

        if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwCPOffset,
                             (LPBYTE)&dwCPEnd,
                             sizeof(DWORD)) != sizeof(DWORD))
            return(FALSE);

        dwCPOffset += sizeof(DWORD);

        dwCPEnd = DWENDIAN(dwCPEnd);

        if (dwCPEnd < dwCPStart)
        {
            // Corruption in file?

            return(FALSE);
        }

        // Read the stream position of the piece

        if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwFCOffset,
                             (LPBYTE)&dwOffset,
                             sizeof(DWORD)) != sizeof(DWORD))
            return(FALSE);

        dwFCOffset += 8;

        // We now have the offset and size of the piece

        dwOffset = DWENDIAN(dwOffset);
        dwSize = dwCPEnd - dwCPStart;

        if (WDRemoveTextPiece95(lpstStream,
                                lpstKey,
                                dwOffset,
                                dwSize,
                                bExtChar) == FALSE)
            return(FALSE);

        // The CP of the end of the current piece is the
        //  CP of the start of the next piece

        dwCPStart = dwCPEnd;
    }

    return(TRUE);
}

BOOL WDModifySED
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7FIB            lpstFIB
)
{
    DWORD               dwOffset;
    DWORD               dwSize;
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

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)abySED,
                          sizeof(abySED)) != sizeof(abySED))
        return(FALSE);

    dwSize = sizeof(abySED);
    lpstFIB->dwLCBPlcfsed = DWENDIAN(dwSize);

    return(TRUE);
}

BOOL WDModifyCHP
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7FIB            lpstFIB
)
{
    DWORD               dwOffset;
    DWORD               dwSize;
    DWORD               dwNumFKPs;
    DWORD               dwFCLimit;
    WORD                wPN;
    BYTE                byTemp;

    dwOffset = DWENDIAN(lpstFIB->dwFCPlcfbteChpx);
    dwSize = DWENDIAN(lpstFIB->dwLCBPlcfbteChpx);

    dwNumFKPs = (dwSize - sizeof(DWORD)) / (sizeof(DWORD) + sizeof(WORD));

    if (dwNumFKPs == 0)
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Read the limit FC and the PN of the first CHP FKP

    // Read the first FC

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         dwOffset,
                         (LPBYTE)&dwFCLimit,
                         sizeof(DWORD)) != sizeof(DWORD))
        return(FALSE);

    dwFCLimit = DWENDIAN(dwFCLimit);
    ++dwFCLimit;
    dwFCLimit = DWENDIAN(dwFCLimit);

    dwOffset += dwNumFKPs * sizeof(DWORD);
    dwOffset += sizeof(DWORD);

    // Read the PN of the first CHP FKP

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         dwOffset,
                         (LPBYTE)&wPN,
                         sizeof(WORD)) != sizeof(WORD))
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Modify the bintable to have only one FC range

    dwOffset = DWENDIAN(lpstFIB->dwFCPlcfbteChpx) + sizeof(DWORD);

    // Write the limit of the first CHP

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)&dwFCLimit,
                          sizeof(DWORD)) != sizeof(DWORD))
        return(FALSE);

    dwOffset += sizeof(DWORD);

    // Write the PN of the first CHP FKP

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)&wPN,
                          sizeof(WORD)) != sizeof(WORD))
        return(FALSE);

    // Set the first CHP FKP and count

    lpstFIB->wPNChpFirst = wPN;
    lpstFIB->wCPNBteChp = 1;

    //////////////////////////////////////////////////////////////////
    // Modify the first PN to have dwFCFirst, dwFCLimit, (BYTE)0
    //  at the beginning and (BYTE)1 at the end

    wPN = WENDIAN(wPN);

    dwOffset = (DWORD)wPN * (DWORD)0x200 + sizeof(DWORD);

    // Write dwFCLimit

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)&dwFCLimit,
                          sizeof(DWORD)) != sizeof(DWORD))
        return(FALSE);

    dwOffset += sizeof(DWORD);

    // Write (BYTE)0

    byTemp = 0;

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)&byTemp,
                          sizeof(BYTE)) != sizeof(BYTE))
        return(FALSE);

    // Write (BYTE)1;

    dwOffset = ((DWORD)wPN + 1) * (DWORD)0x200 - 1;
    byTemp = 1;
    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)&byTemp,
                          sizeof(BYTE)) != sizeof(BYTE))
        return(FALSE);

    // Set the size of the CHP bintable

    lpstFIB->dwLCBPlcfbteChpx = 0xA;
    lpstFIB->dwLCBPlcfbteChpx = DWENDIAN(lpstFIB->dwLCBPlcfbteChpx);

    return(TRUE);
}


BOOL WDModifyPAP
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7FIB            lpstFIB,
    BOOL                bExtChar

)
{
    DWORD               dwOffset;
    DWORD               dwSize;
    DWORD               dwNumFKPs;
    DWORD               dwFCLimit;
    WORD                wPN;
    BYTE                byTemp;
    int                 i;

    dwOffset = DWENDIAN(lpstFIB->dwFCPlcfbtePapx);
    dwSize = DWENDIAN(lpstFIB->dwLCBPlcfbtePapx);

    dwNumFKPs = (dwSize - sizeof(DWORD)) / (sizeof(DWORD) + sizeof(WORD));

    if (dwNumFKPs == 0)
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Read the limit FC and the PN of the first CHP FKP

    // Read the first FC

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         dwOffset,
                         (LPBYTE)&dwFCLimit,
                         sizeof(DWORD)) != sizeof(DWORD))
        return(FALSE);

    dwFCLimit = DWENDIAN(dwFCLimit);
    ++dwFCLimit;
    if (bExtChar != FALSE)
        ++dwFCLimit;
    dwFCLimit = DWENDIAN(dwFCLimit);

    dwOffset += dwNumFKPs * sizeof(DWORD);
    dwOffset += sizeof(DWORD);

    // Read the PN of the first CHP FKP

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         dwOffset,
                         (LPBYTE)&wPN,
                         sizeof(WORD)) != sizeof(WORD))
        return(FALSE);

    //////////////////////////////////////////////////////////////////
    // Modify the bintable to have only one FC range

    dwOffset = DWENDIAN(lpstFIB->dwFCPlcfbtePapx) + sizeof(DWORD);

    // Write the limit of the first CHP

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)&dwFCLimit,
                          sizeof(DWORD)) != sizeof(DWORD))
        return(FALSE);

    dwOffset += sizeof(DWORD);

    // Write the PN of the first CHP FKP

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)&wPN,
                          sizeof(WORD)) != sizeof(WORD))
        return(FALSE);

    // Set the first PAP FKP and count

    lpstFIB->wPNPapFirst = wPN;
    lpstFIB->wCPNBtePap = 1;

    //////////////////////////////////////////////////////////////////
    // Modify the first PN to have dwFCFirst, dwFCLimit, (BYTE)0
    //  at the beginning and (BYTE)1 at the end

    wPN = WENDIAN(wPN);

    dwOffset = (DWORD)wPN * (DWORD)0x200 + sizeof(DWORD);

    // Write dwFCLimit

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)&dwFCLimit,
                          sizeof(DWORD)) != sizeof(DWORD))
        return(FALSE);

    dwOffset += sizeof(DWORD);

    // Write (BYTE)0 seven times

    byTemp = 0;
    for (i=0;i<7;i++)
    {
        if (WD7EncryptedWrite(lpstStream,
                              lpstKey,
                              dwOffset,
                              (LPBYTE)&byTemp,
                              sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        ++dwOffset;
    }

    // Write (BYTE)1;

    dwOffset = ((DWORD)wPN + 1) * (DWORD)0x200 - 1;
    byTemp = 1;
    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwOffset,
                          (LPBYTE)&byTemp,
                          sizeof(BYTE)) != sizeof(BYTE))
        return(FALSE);

    // Set the size of the PAP bintable

    lpstFIB->dwLCBPlcfbtePapx = 0xA;
    lpstFIB->dwLCBPlcfbtePapx = DWENDIAN(lpstFIB->dwLCBPlcfbtePapx);

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL WD7ClearDocument()
//
// Parameters:
//  lpstStream          Ptr to WordDocument stream
//  lpstKey             Ptr to decryption key
//  lpstFIB             FIB of the document
//  dwFIBOffset         Offset of the FIB
//
// Description:
//  Clears text from the given Word 6.0/95 document.  The function
//  leaves autotext and document variables.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//*************************************************************************

BOOL WD7ClearDocument
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7FIB            lpstFIB,
    DWORD               dwFIBOffset
)
{
    DWORD               dwTemp;
    BOOL                bExtChar;
    BOOL                bResult;
    DWORD               dwSize;
    DWORD               dwOffset;

    // Does it use the extended character set?

    if ((lpstFIB->byFlags1 & WD7FIB_FLAGS1_EXT_CHAR) == 0)
        bExtChar = FALSE;
    else
        bExtChar = TRUE;

    if ((lpstFIB->byFlags0 & WD7FIB_FLAGS0_COMPLEX) == 0)
    {
        // Non-complex file

        bResult = WDRemoveNonComplexText95(lpstStream,
                                           lpstKey,
                                           lpstFIB,
                                           bExtChar);
    }
    else
    {
        // Complex file

        bResult = WDRemoveComplexText95(lpstStream,
                                        lpstKey,
                                        lpstFIB,
                                        bExtChar);
    }

    if (bResult == FALSE)
        return(FALSE);

    if (WDModifySED(lpstStream,
                    lpstKey,
                    lpstFIB) == FALSE)
        return(FALSE);

    if (WDModifyCHP(lpstStream,
                    lpstKey,
                    lpstFIB) == FALSE)
        return(FALSE);

    if (WDModifyPAP(lpstStream,
                    lpstKey,
                    lpstFIB,
                    bExtChar) == FALSE)
        return(FALSE);

    // wIdent;                 //   0   0: Magic number
    // wFIB;                   //   2   2: FIB version
    // wProduct;               //   4   4: Product version
    // wLID;                   //   6   6: Language stamp
    // wPN;                    //   8   8:

    //////////////////////////////////////////////////////////////////
    // Set to non-complex

    // byFlags0;               //  10   A:

    lpstFIB->byFlags0 &= ~(BYTE)WD7FIB_FLAGS0_COMPLEX;

    // byFlags1;               //  11   B:
    // wFIBBack;               //  12   C:
    // dwKey;                  //  14   E: File encrypted key
    // byEnvr;                 //  18  12: Creation environment
    // byReserved0;            //  19  13:
    // wChSe;                  //  20  14: Default ext text char set
    // wChSeTables;            //  22  16: Default ext data char set

    //////////////////////////////////////////////////////////////////
    // Set to have only one character

    // dwFCMin;                //  24  18: First char file offset
    // dwFCMac;                //  28  1C: Last char file offset

    dwTemp = DWENDIAN(lpstFIB->dwFCMin);
    ++dwTemp;
    if (bExtChar != FALSE)
        ++dwTemp;
    dwTemp = DWENDIAN(dwTemp);
    lpstFIB->dwFCMac = dwTemp;

    // dwCBMac;                //  32  20: Last byte file offset
    // dwFCSpare0;             //  36  24:
    // dwFCSpare1;             //  40  28:
    // dwFCSpare2;             //  44  2C:
    // dwFCSpare3;             //  48  30:

    // dwCCPText;              //  52  34: Length of main doc text
    // dwCCPFtn;               //  56  38: Length of footnote text
    // dwCCPHdd;               //  60  3C: Length of header text
    // dwCCPMcr;               //  64  40: Length of macro text
    // dwCCPAtn;               //  68  44: Length of annotation text
    // dwCCPEdn;               //  72  48: Length of endnote text
    // dwCCPTxbx;              //  76  4C: Length of textbox text
    // dwCCPHdrTxbx;           //  80  50: Length of hdr textbox text

    dwTemp = DWENDIAN((DWORD)1);
    lpstFIB->dwCCPText = dwTemp;
    lpstFIB->dwCCPFtn = 0;
    lpstFIB->dwCCPHdd = 0;
    lpstFIB->dwCCPMcr = 0;
    lpstFIB->dwCCPAtn = 0;
    lpstFIB->dwCCPEdn = 0;
    lpstFIB->dwCCPTxbx = 0;
    lpstFIB->dwCCPHdrTxbx = 0;

    // dwCCPSpare2;            //  84  54:
    // dwFCStshfOrig;          //  88  58: Orig STSH file offset
    // dwLCBStshfOrig;         //  92  5C: Orig STSH size
    // dwFCStshf;              //  96  60: STSH file offset
    // dwLCBStshf;             // 100  64: STSH size

    // dwFCPlcffndRef;         // 104  68: Footnote ref PLC offset
    // dwLCBPlcffndRef;        // 108  6C: Footnote ref PLC size
    // dwFCPlcffndTxt;         // 112  70: Footnote text PLC offset
    // dwLCBPlcffndTxt;        // 116  74: Footnote text PLC size
    // dwFCPlcfandRef;         // 120  78: Annotation ref PLC offset
    // dwLCBPlcfandRef;        // 124  7C: Annotation ref PLC size
    // dwFCPlcfandTxt;         // 128  80: Annotation text PLC offset
    // dwLCBPlcfandTxt;        // 132  84: Annotation text PLC size

    lpstFIB->dwLCBPlcffndRef = 0;
    lpstFIB->dwLCBPlcffndTxt = 0;
    lpstFIB->dwLCBPlcfandRef = 0;
    lpstFIB->dwLCBPlcfandTxt = 0;

    // dwFCPlcfsed;            // 136  88: Section desc PLC offset
    // dwLCBPlcfsed;           // 140  8C: Section desc PLC size

    // dwFCPlcfpad;            // 144  90: Paragraph desc PLC offset
    // dwLCBPlcfpad;           // 148  94: Paragraph desc PLC size
    // dwFCPlcfphe;            // 152  98: Paragraph height PLC offset
    // dwLCBPlcfphe;           // 156  9C: Paragraph height PLC size

    lpstFIB->dwLCBPlcfpad = 0;
    lpstFIB->dwLCBPlcfphe = 0;

    // dwFCSttbfglsy;          // 160  A0: Glossary str table offset
    // dwLCBSttbfglsy;         // 164  A4: Glossary str table size
    // dwFCPlcfglsy;           // 168  A8: Glossary PLC offset
    // dwLCBPlcfglsy;          // 172  AC: Glossary PLC size

    // dwFCPlcfhdd;            // 176  B0: Header PLC offset
    // dwLCBPlcfhdd;           // 180  B4: Header PLC size

    lpstFIB->dwLCBPlcfhdd = 0;

    // dwFCPlcfbteChpx;        // 184  B8: Char prop bin table offset
    // dwLCBPlcfbteChpx;       // 188  BC: Char prop bin table size
    // dwFCPlcfbtePapx;        // 192  C0: Para prop bin table offset
    // dwLCBPlcfbtePapx;       // 196  C4: Para prop bin table size

    // dwFCPlcfsea;            // 200  C8: Private PLC offset
    // dwLCBPlcfsea;           // 204  CC: Private PLC size
    // dwFCSttbfffn;           // 208  D0: Font info table offset
    // dwLCBSttbfffn;          // 212  D4: Font info table size

    //////////////////////////////////////////////////////////////////
    // Eliminate fields

    // dwFCPlcffldMom;         // 216  D8: Main doc field PLC offset
    // dwLCBPlcffldMom;        // 220  DC: Main doc field PLC size
    // dwFCPlcffldHdr;         // 224  E0: Header field PLC offset
    // dwLCBPlcffldHdr;        // 228  E4: Header field PLC size
    // dwFCPlcffldFtn;         // 232  E8: Footnote field PLC offset
    // dwLCBPlcffldFtn;        // 236  EC: Footnote field PLC size
    // dwFCPlcffldAtn;         // 240  F0: Annotation field PLC offset
    // dwLCBPlcffldAtn;        // 244  F4: Annotation field PLC size
    // dwFCPlcffldMcr;         // 248  F8: Macro field PLC offset
    // dwLCBPlcffldMcr;        // 252  FC: Macro field PLC size

    lpstFIB->dwLCBPlcffldMom = 0;       // Main document field PLC size
    lpstFIB->dwLCBPlcffldHdr = 0;       // Header field PLC size
    lpstFIB->dwLCBPlcffldFtn = 0;       // Footnote field PLC size
    lpstFIB->dwLCBPlcffldAtn = 0;       // Annotation field PLC size
    lpstFIB->dwLCBPlcffldMcr = 0;       // Macro field PLC size

    //////////////////////////////////////////////////////////////////
    // Bookmarks to zero

    // dwFCSttbfbkmk;          // 256 100: Bookmark name table offset
    // dwLCBSttbfbkmk;         // 260 104: Bookmark name table size
    // dwFCPlcfbkf;            // 264 108: Bookmark begin PLC offset
    // dwLCBPlcfbkf;           // 268 10C: Bookmark begin PLC size
    // dwFCPlcfbkl;            // 272 110: Bookmark end PLC offset
    // dwLCBPlcfbkl;           // 276 114: Bookmark end PLC size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfbkmk);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfbkmk);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfbkmk = 0;
    lpstFIB->dwLCBPlcfbkf = 0;
    lpstFIB->dwLCBPlcfbkl = 0;

    // dwFCCmds;               // 280 118: Macro commands offset
    // dwLCBCmds;              // 284 11C: Macro commands size
    // dwFCUnused1;            // 288 120:
    // dwLCBUnused1;           // 292 124:
    // dwFCUnused2;            // 296 128:
    // dwLCBUnused2;           // 300 12C:
    // dwFCPrDrvr;             // 304 130: Printer driver info offset
    // dwLCBPrDrvr;            // 308 134: Printer driver info size
    // dwFCPrEnvPort;          // 312 138: Portrait print env offset
    // dwLCBPrEnvPort;         // 316 13C: Portrait print env size
    // dwFCPrEnvLand;          // 320 140: Landscape print env offset
    // dwLCBPrEnvLand;         // 324 144: Landscape print env size
    // dwFCWss;                // 328 148: Window save state offset
    // dwLCBWss;               // 332 14C: Window save state size
    // dwFCDop;                // 336 150: Document property offset
    // dwLCBDop;               // 340 154: Document property size

    //////////////////////////////////////////////////////////////////
    // Associated strings to zero

    // dwFCSttbfAssoc;         // 344 158: Assoc str table offset
    // dwLCBSttbfAssoc;        // 348 15C: Assoc str table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfAssoc);
    if (dwSize > 2)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfAssoc);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset + 2,
                                  0,
                                  dwSize - 2) != dwSize - 2)
            return(FALSE);
    }
    //lpstFIB->dwLCBSttbfAssoc = 0;

    //////////////////////////////////////////////////////////////////
    // Set size to 0

    // dwFCClx;                // 352 160: Complex file info offset
    // dwLCBClx;               // 356 164: Complex file info size

    lpstFIB->dwLCBClx = 0;

    // dwFCReserved0;          // 360 168:
    // dwLCBReserved1;         // 364 16C:

    //////////////////////////////////////////////////////////////////
    // Original filename to zero

    // dwFCAutosaveSource;     // 368 170: Orig file name offset
    // dwLCBAutosaveSource;    // 372 174: Orig file name size

    dwSize = DWENDIAN(lpstFIB->dwLCBAutosaveSource);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCAutosaveSource);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBAutosaveSource = 0;

    //////////////////////////////////////////////////////////////////
    // Annotation owners to zero

    // dwFCGrpStAtnOwners;     // 376 178: Annotation owners offset
    // dwLCBGrpStAtnOwners;    // 380 17C: Annotation owners size

    dwSize = DWENDIAN(lpstFIB->dwLCBGrpStAtnOwners);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCGrpStAtnOwners);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBGrpStAtnOwners = 0;

    //////////////////////////////////////////////////////////////////
    // Annotation bookmark names to zero

    // dwFCSttbfAtnbkmk;       // 384 180: Annotation bkmk name offset
    // dwLCBSttbfAtnbkmk;      // 388 184: Annotation bkmk name size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfAtnbkmk);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfAtnbkmk);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfAtnbkmk = 0;

    // wSpare4Fib;             // 392 188:
    // wPNChpFirst;            // 394 18A: First page w/CHPX FKP info
    // wPNPapFirst;            // 396 18C: First page w/PAPX FKP info
    // wCPNBteChp;             // 398 18E: CHPX FKP count
    // wCPNBtePap;             // 400 190: PAPX FKP count

    //////////////////////////////////////////////////////////////////
    // Eliminate drawn objects

    // dwFCPlcfdoaMom;         // 402 192: Drawn object PLC offset
    // dwLCBPlcfdoaMom;        // 406 196: Drawn object PLC size
    // dwFCPlcfdoaHdr;         // 410 19A: Hdr drawn object PLC offset
    // dwLCBPlcfdoaHdr;        // 414 19E: Hdr drawn object PLC size

    lpstFIB->dwLCBPlcfdoaMom = 0;
    lpstFIB->dwLCBPlcfdoaHdr = 0;

    // dwFCUnused3;            // 418 1A2:
    // dwLCBUnused3;           // 422 1A6:
    // dwFCUnused4;            // 426 1AA:
    // dwLCBUnused4;           // 430 1AE:

    //////////////////////////////////////////////////////////////////
    // Annotation bookmarks to zero

    // dwFCPlcfAtnbkf;         // 434 1B2: Annotation BKF PLC offset
    // dwLCBPlcfAtnbkf;        // 438 1B6: Annotation BKF PLC size
    // dwFCPlcfAtnbkl;         // 442 1BA: Annotation BKL PLC offset
    // dwLCBPlcfAtnbkl;        // 446 1BE: Annotation BKL PLC size

    lpstFIB->dwLCBPlcfAtnbkf = 0;
    lpstFIB->dwLCBPlcfAtnbkl = 0;

    // dwFCPMS;                // 450 1C2: Print merge state offset
    // dwLCBPMS;               // 454 1C6: Print merge state size

    //////////////////////////////////////////////////////////////////
    // Form field table to zero

    // dwFCFormFldSttbf;       // 458 1CA: Form field table offset
    // dwLCBFormFldSttbf;      // 462 1CE: Form field table size

    dwSize = DWENDIAN(lpstFIB->dwLCBFormFldSttbf);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCFormFldSttbf);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBFormFldSttbf = 0;

    //////////////////////////////////////////////////////////////////
    // Set endnote table sizes to zero

    // dwFCPlcfendRef;         // 466 1D2: Endnote ref PLC offset
    // dwLCBPlcfendRef;        // 470 1D6: Endnote ref PLC size
    // dwFCPlcfendTxt;         // 474 1DA: Endnote text PLC offset
    // dwLCBPlcfendTxt;        // 478 1DE: Endnote text PLC size

    lpstFIB->dwLCBPlcfendRef = 0;
    lpstFIB->dwLCBPlcfendTxt = 0;

    //////////////////////////////////////////////////////////////////
    // Eliminate endnote fields

    // dwFCPlcffldEdn;         // 482 1E2: Endnote field PLC offset
    // dwLCBPlcffldEdn;        // 486 1E6: Endnote field PLC size

    lpstFIB->dwLCBPlcffldEdn = 0;       // Endnote field PLC size

    // dwFCUnused5;            // 490 1EA:
    // dwLCBUnused5;           // 494 1EE:
    // dwFCUnused6;            // 498 1F2:
    // dwLCBUnused6;           // 502 1F6:

    //////////////////////////////////////////////////////////////////
    // Author abbreviation table to zero

    // dwFCSttbfRMark;         // 506 1FA: Author abbrev table offset
    // dwLCBSttbfRMark;        // 510 1FE: Author abbrev table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfRMark);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfRMark);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfRMark = 0;

    //////////////////////////////////////////////////////////////////
    // Caption titles table to zero

    // dwFCSttbfCaption;       // 514 202: Caption titles table offset
    // dwLCBSttbfCaption;      // 518 206: Caption titles table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfCaption);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfCaption);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfCaption = 0;

    // dwFCAutoCaption;        // 522 20A: Auto caption table offset
    // dwLCBAutoCaption;       // 526 20E: Auto caption table size

    dwSize = DWENDIAN(lpstFIB->dwLCBAutoCaption);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCAutoCaption);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBAutoCaption = 0;

    //////////////////////////////////////////////////////////////////
    // Contributing docs to zero

    // dwFCPlcfwkb;            // 530 212: Contrib doc PLC offset
    // dwLCBPlcfwkb;           // 534 216: Contrib doc PLC size

    lpstFIB->dwLCBPlcfwkb = 0;

    // dwFCUnused7;            // 538 21A:
    // dwLCBUnused7;           // 542 21E:

    // dwFCPlcftxbxTxt;        // 546 222: Textbox text PLC offset
    // dwLCBPlcftxbxTxt;       // 550 226: Textbox text PLC size

    lpstFIB->dwLCBPlcftxbxTxt = 0;

    // dwFCPlcffldTxbx;        // 554 22A: Textbox field PLC offset
    // dwLCBPlcffldTxbx;       // 558 22E: Textbox field PLC size

    lpstFIB->dwLCBPlcffldTxbx = 0;      // Textbox field PLC size

    // dwFCPlcfHdrtxbxTxt;     // 562 232: Header textbox PLC offset
    // dwLCBPlcfHdrtxbxTxt;    // 566 236: Header textbox PLC size

    lpstFIB->dwLCBPlcfHdrtxbxTxt = 0;

    // dwFCPlcffldHdrTxbx;     // 570 23A: Hdr txtbox field PLC offset
    // dwLCBPlcffldHdrTxbx;    // 574 23E: Hdr txtbox field PLC size

    lpstFIB->dwLCBPlcffldHdrTxbx = 0;   // Header textbox field PLC size

    // dwFCStwUser;            // 578 242: Macro user storage offset
    // dwLCBStwUser;           // 582 246: Macro user storage size
    // dwFCSttbttmbd;          // 586 24A: Embedded TTF table offset
    // dwLCBSttbttmbd;         // 590 24E: Embedded TTF table size
    // dwFCUnused8;            // 594 252:
    // dwLCBUnused8;           // 598 256:
    // dwFCPgdMotherFCPgd;     // 602 25A: Main text PGD PLC offset
    // dwFCPgdMotherLCBPgd;    // 606 25E: Main text PGD PLC size
    // dwFCPgdMotherFCBkd;     // 610 262: Main txt brk PGD PLC offset
    // dwFCPgdMotherLCBBkd;    // 614 266: Main txt brk PGD PLC size
    // dwFCPgdFtnFCPgd;        // 618 26A: Footnote PGD PLC offset
    // dwFCPgdFtnLCBPgd;       // 622 26E: Footnote PGD PLC size
    // dwFCPgdFtnFCBkd;        // 626 272: Foonote brk PGD PLC offset
    // dwFCPgdFtnLCBBkd;       // 630 276: Foonote brk PGD PLC size
    // dwFCPgdEdnFCPgd;        // 634 27A: Endnote PGD PLC offset
    // dwFCPgdEdnLCBPgd;       // 638 27E: Endnote PGD PLC size
    // dwFCPgdEdnFCBkd;        // 642 282: Endnote brk PGD PLC offset
    // dwFCPgdEdnLCBBkd;       // 646 286: Endnote brk PGD PLC size

    //////////////////////////////////////////////////////////////////
    // Field keyword table to zero

    // dwFCSttbfIntlFld;       // 650 28A: Field keyword table offset
    // dwLCBSttbfIntlFld;      // 654 28E: Field keyword table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbfIntlFld);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbfIntlFld);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbfIntlFld = 0;

    //////////////////////////////////////////////////////////////////
    // Routing slip to zero

    // dwFCRouteSlip;          // 658 292: Mailer route slip offset
    // dwLCBRouteSlip;         // 662 296: Mailer route slip size

    dwSize = DWENDIAN(lpstFIB->dwLCBRouteSlip);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCRouteSlip);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBRouteSlip = 0;

    //////////////////////////////////////////////////////////////////
    // Saved by to zero

    // dwFCSttbSavedBy;        // 666 29A: Saved by users table offset
    // dwLCBSttbSavedBy;       // 670 29E: Saved by users table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbSavedBy);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbSavedBy);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbSavedBy = 0;

    //////////////////////////////////////////////////////////////////
    // Filename reference table to zero

    // dwFCSttbFnm;            // 674 2A2: Filename ref table offset
    // dwLCBSttbFnm;           // 678 2A6: Filename ref table size

    dwSize = DWENDIAN(lpstFIB->dwLCBSttbFnm);
    if (dwSize != 0)
    {
        dwOffset = DWENDIAN(lpstFIB->dwFCSttbFnm);
        if (WD7EncryptedWriteByte(lpstStream,
                                  lpstKey,
                                  dwOffset,
                                  0,
                                  dwSize) != dwSize)
            return(FALSE);
    }
    lpstFIB->dwLCBSttbFnm = 0;

    //////////////////////////////////////////////////////////////////
    // Write back the modified FIB

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          dwFIBOffset,
                          (LPBYTE)lpstFIB,
                          sizeof(WD7FIB_T)) != sizeof(WD7FIB_T))
        return(FALSE);

    return(TRUE);
}

