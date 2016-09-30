// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/msxcrc.cpv   1.0   08 Dec 1998 12:53:12   DCHI  $
//
// Description:
//  Contains the following functions:
//
//      BOOL MSXCRCStream()
//      BOOL MSXL1WordDocumentCRC()
//      BOOL MSXL2WordDocumentCRC()
//      BOOL MSXXLFindFirstBoundsheet()
//      BOOL MSXXL4SheetNameSetCRC()
//      BOOL MSXXL4SheetCRC()
//      BOOL MSXXL4EncryptedSheetCRC()
//      BOOL MSXXL4SheetSetCRC()
//      BOOL MSXL2_VBA_PROJECTCRC()
//      BOOL MSXL2VBACRC()
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/msxcrc.cpv  $
// 
//    Rev 1.0   08 Dec 1998 12:53:12   DCHI
// Initial revision.
// 
//************************************************************************

#include "wd7api.h"
#include "olestrnm.h"
#include "wddecsig.h"
#include "crc32.h"
#include "xl5api.h"
#include "o97api.h"

#include "msxi.h"

//********************************************************************
//
// Function:
//  BOOL MSXCRCStream()
//
// Parameters:
//  lpstStream          Stream to CRC
//  lpabyWorkBuf        Ptr to working buffer
//  nWorkBufSize        Size of working buffer
//  dwStartOffset       Starting offset in stream to CRC
//  dwEndOffset         Ending offset in stream to CRC
//  lpdwCRC             Ptr to DWORD CRC to continue
//  lpdwCRCSize         Ptr to DWORD CRC size to continue
//
// Description:
//  The function CRCs the bytes from the start offset to the
//  byte before the end offset.  The CRC is continued from
//  the given CRC.  The number of bytes CRCed is added to the
//  given size.
//
//  Before performing the CRC, the function verifies that the
//  start offset does not follow the end offset.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXCRCStream
(
    LPSS_STREAM         lpstStream,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    DWORD               dwStartOffset,
    DWORD               dwEndOffset,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
)
{
    DWORD               dwOffset;
    DWORD               dwCount;
    DWORD               dwResult;

    // Validate start and end offsets

    if (dwStartOffset > dwEndOffset)
        return(FALSE);

    dwOffset = dwStartOffset;
    dwCount = nWorkBufSize;
    while (dwOffset < dwEndOffset)
    {
        if (dwEndOffset - dwOffset < dwCount)
            dwCount = dwEndOffset - dwOffset;

        // Read chunk

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpabyWorkBuf,
                       dwCount,
                       &dwResult) != SS_STATUS_OK ||
            dwResult != dwCount)
            return(FALSE);

        // CRC chunk

        *lpdwCRC = CRC32Compute(dwCount,lpabyWorkBuf,*lpdwCRC);

        // Update offset

        dwOffset += dwCount;
    }

    // Update CRC size

    *lpdwCRCSize += dwEndOffset - dwStartOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXL1WordDocumentCRC()
//
// Parameters:
//  lpstStream          Ptr to WordDocument stream
//  lpstKey             Ptr to encryption key
//  dwMCDOffset         First MCD offset
//  wMCDCount           MCD count
//  lpabyWorkBuf        Ptr to work buffer
//  nWorkBufSize        Size of work buffer
//  lpdwCRC             Ptr to DWORD for CRC
//  lpdwCRCByteCount    Ptr to DWORD for CRC byte count
//
// Description:
//  The function iterates through the MCDs in order and CRCs
//  the encryption and size fields.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXL1WordDocumentCRC
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwMCDOffset,
    WORD                wMCDCount,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCByteCount
)
{
    DWORD               dwCRC;
    DWORD               dwCRCByteCount;
    BYTE                abyMCD[sizeof(WD7_MCD_T)];
    LPBYTE              lpabyBuf;
    LPBYTE              lpabyMCD;
    int                 nMCDsPerRead;
    int                 i;

    // Determine how many MCDs we can read at once

    if (nWorkBufSize < sizeof(WD7_MCD_T))
    {
        lpabyBuf = abyMCD;
        nMCDsPerRead = 1;
    }
    else
    {
        lpabyBuf = lpabyWorkBuf;
        nMCDsPerRead = nWorkBufSize / sizeof(WD7_MCD_T);
    }

    // Iterate in chunks

    CRC32Init(dwCRC);
    dwCRCByteCount = 0;
    while (wMCDCount != 0)
    {
        if (wMCDCount < nMCDsPerRead)
            nMCDsPerRead = (int)wMCDCount;

        if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwMCDOffset,
                             lpabyBuf,
                             nMCDsPerRead * sizeof(WD7_MCD_T)) !=
            nMCDsPerRead * sizeof(WD7_MCD_T))
            return(FALSE);

        lpabyMCD = lpabyBuf;
        for (i=0;i<nMCDsPerRead;i++)
        {
            // Only CRC valid macros

            if (((LPWD7_MCD)lpabyMCD)->dwN != 0)
            {
                // CRC encryption byte and size fields

                CRC32Continue(dwCRC,lpabyMCD[0x01]);
                CRC32Continue(dwCRC,lpabyMCD[0x0C]);
                CRC32Continue(dwCRC,lpabyMCD[0x0D]);
                CRC32Continue(dwCRC,lpabyMCD[0x0E]);
                CRC32Continue(dwCRC,lpabyMCD[0x0F]);

                dwCRCByteCount += 5;
            }

            lpabyMCD += sizeof(WD7_MCD_T);
        }

        dwMCDOffset += nMCDsPerRead * sizeof(WD7_MCD_T);
        wMCDCount -= nMCDsPerRead;
    }

    // Return the CRC and CRC byte count

    *lpdwCRC = dwCRC;
    *lpdwCRCByteCount = dwCRCByteCount;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXL2WordDocumentCRC()
//
// Parameters:
//  lpstStream          Ptr to WordDocument stream
//  lpstKey             Ptr to encryption key
//  dwMCDOffset         First MCD offset
//  wMCDCount           MCD count
//  lpabyWorkBuf        Ptr to work buffer
//  nWorkBufSize        Size of work buffer
//  lpdwCRC             Ptr to DWORD for CRC
//  lpdwCRCByteCount    Ptr to DWORD for CRC byte count
//
// Description:
//  The function iterates through and CRCs the macro bodies
//  in MCD order.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXL2WordDocumentCRC
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwMCDOffset,
    WORD                wMCDCount,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCByteCount
)
{
    DWORD               dwCRC;
    DWORD               dwCRCByteCount;
    WD7_MCD_T           stMCD;
    DWORD               dwCount;

    CRC32Init(dwCRC);
    dwCRCByteCount = 0;
    while (wMCDCount-- != 0)
    {
        if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwMCDOffset,
                             (LPBYTE)&stMCD,
                             sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
            return(FALSE);

        // Only CRC valid macro bodies

        if (stMCD.dwN != 0)
        {
            // Endianize the offset and size

            stMCD.dwOffset = DWENDIAN(stMCD.dwOffset);
            stMCD.dwSize = DWENDIAN(stMCD.dwSize);

            dwCRCByteCount += stMCD.dwSize;

            // CRC the body

            dwCount = nWorkBufSize;
            while (stMCD.dwSize != 0)
            {
                if (stMCD.dwSize < dwCount)
                    dwCount = stMCD.dwSize;

                // Read the chunk

                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     stMCD.dwOffset,
                                     lpabyWorkBuf,
                                     dwCount) != dwCount)
                    return(FALSE);

                // CRC the chunk

                dwCRC = CRC32Compute(dwCount,
                                     lpabyWorkBuf,
                                     dwCRC);

                stMCD.dwSize -= dwCount;
                stMCD.dwOffset += dwCount;
            }
        }

        dwMCDOffset += sizeof(WD7_MCD_T);
    }

    // Return the CRC and CRC byte count

    *lpdwCRC = dwCRC;
    *lpdwCRCByteCount = dwCRCByteCount;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXXLFindFirstBoundsheet()
//
// Parameters:
//  lpstStream           Ptr to Book/Workbook stream
//  lpabyWorkBuf         Ptr to work buffer
//  nWorkBufSize         Size of work buffer
//  lpdwBoundSheetOffset Ptr to DWORD for BOUNDSHEET offset
//
// Description:
//  The function performs a bulk scan for the first BOUNDSHEET
//  record.
//
// Returns:
//  FALSE               On error or no BOUNDSHEETs found
//  TRUE                If a BOUNDSHEET record is found
//
//********************************************************************

BOOL MSXXLFindFirstBoundsheet
(
    LPSS_STREAM         lpstStream,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwBoundSheetOffset
)
{
    DWORD               dwLimit;
    DWORD               dwOffset;
    int                 nBytesLeft;
    LPXL_REC_HDR        lpstHdr;
    DWORD               dwCount;

    dwLimit = SSStreamLen(lpstStream);

    // Bulk scan for the BOUNDSHEET records

    nBytesLeft = 0;
    dwOffset = 0;
    while (1)
    {
        // Do we need to read a new chunk?

        if (nBytesLeft < sizeof(XL_REC_HDR_T))
        {
            // Has the end of the stream been reached?

            if (dwOffset >= dwLimit)
                return(FALSE);

            // Determine how many bytes to read

            if (dwLimit - dwOffset < (DWORD)nWorkBufSize)
                nBytesLeft = (int)(dwLimit - dwOffset);
            else
                nBytesLeft = nWorkBufSize;

            // Make sure we have at least a header left

            if (nBytesLeft < sizeof(XL_REC_HDR_T))
                return(FALSE);

            // Read a new chunk

            lpstHdr = (LPXL_REC_HDR)lpabyWorkBuf;
            if (SSSeekRead(lpstStream,
                           dwOffset,
                           lpstHdr,
                           nBytesLeft,
                           &dwCount) != SS_STATUS_OK ||
                (int)dwCount != nBytesLeft)
                return(FALSE);
        }

        // Endianize the header

        lpstHdr->wType = WENDIAN(lpstHdr->wType);
        lpstHdr->wLen = WENDIAN(lpstHdr->wLen);

        if (lpstHdr->wType == eXLREC_EOF)
            return(FALSE);

        if (lpstHdr->wType == eXLREC_BOUNDSHEET)
            break;

        // Update offset and bytes left

        dwOffset += sizeof(XL_REC_HDR_T);
        dwOffset += lpstHdr->wLen;
        nBytesLeft -= sizeof(XL_REC_HDR_T);
        if (nBytesLeft < lpstHdr->wLen)
            nBytesLeft = 0;
        else
		{
            nBytesLeft -= lpstHdr->wLen;
			lpstHdr = (LPXL_REC_HDR)((LPBYTE)(lpstHdr + 1) + lpstHdr->wLen);
		}
    }

    *lpdwBoundSheetOffset = dwOffset;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXXL4SheetNameSetCRC()
//
// Parameters:
//  lpstStream          Ptr to Book/Workbook stream
//  lpstKey             Ptr to encryption key
//  lpabyWorkBuf        Ptr to work buffer
//  nWorkBufSize        Size of work buffer
//  dwBoundSheetOffset  First BOUNDSHEET record offset
//  lpdwCRC             Ptr to DWORD of CRC to continue
//  lpdwCRCSize         Ptr to DWORD of CRC size
//
// Description:
//  The function CRCs the entire set of names for the Excel 4.0
//  sheets.
//
// Returns:
//  FALSE               On error or no BOUNDSHEETs found
//  TRUE                If a BOUNDSHEET record is found
//
//********************************************************************

BOOL MSXXL4SheetNameSetCRC
(
    LPSS_STREAM         lpstStream,
    LPXL5ENCKEY         lpstKey,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    DWORD               dwBoundSheetOffset,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
)
{
    DWORD               dwLimit;
    DWORD               dwCount;
    int                 nNumBytes;
    LPXL_REC_HDR        lpstHdr;
    WORD                wNameBytesLeft;
    XL_REC_HDR_T        stHdr;
    DWORD               dwHdrOffset;
    int                 i;
    BYTE                bySheetType;
    DWORD               dwOffset;

    // Initialize the CRC

    CRC32Init(*lpdwCRC);
    *lpdwCRCSize = 0;

    // Iterate through all the BOUNDSHEET records

    dwOffset = dwBoundSheetOffset;
    dwLimit = SSStreamLen(lpstStream);
    while (dwOffset < dwLimit)
    {
        // Read a chunk

        if (dwLimit - dwOffset < (DWORD)nWorkBufSize)
            nNumBytes = (int)(dwLimit - dwOffset);
        else
            nNumBytes = nWorkBufSize;

        // Make sure that there are at least enough bytes
        //  for a record header

        if (nNumBytes < sizeof(XL_REC_HDR_T))
            return(FALSE);

        // Refill the buffer

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpabyWorkBuf,
                       nNumBytes,
                       &dwCount) != SS_STATUS_OK ||
            (int)dwCount != nNumBytes)
            return(FALSE);

        // Iterate through the buffer looking for an EOF

        i = 0;
        while (1)
        {
            if ((nNumBytes - i) < sizeof(XL_REC_HDR_T))
            {
                dwOffset += i;
                break;
            }

            lpstHdr = (LPXL_REC_HDR)(lpabyWorkBuf + i);
            lpstHdr->wType = WENDIAN(lpstHdr->wType);
            lpstHdr->wLen = WENDIAN(lpstHdr->wLen);

            if (lpstHdr->wType == eXLREC_EOF)
                return(TRUE);

            // Increment past the header

            i += sizeof(XL_REC_HDR_T);

            if (lpstHdr->wType == eXLREC_BOUNDSHEET &&
                lpstHdr->wLen > 6)
            {
                dwHdrOffset = dwOffset + i - sizeof(XL_REC_HDR_T);
                stHdr = *lpstHdr;

                // Determine whether it is an Excel 4.0 macro sheet

                if (nNumBytes - i < 6)
                {
                    // Read the sheet type byte

                    if (SSSeekRead(lpstStream,
                                   dwOffset + i + 5,
                                   &bySheetType,
                                   sizeof(BYTE),
                                   &dwCount) != SS_STATUS_OK ||
                        dwCount != sizeof(BYTE))
                        return(FALSE);
                }
                else
                    bySheetType = lpabyWorkBuf[i + 5];

                // Decrypt the sheet type if necessary

                if (lpstKey != NULL && lpstKey->bEncrypted != FALSE)
                {
                    XL5DecryptData(dwHdrOffset,
                                   stHdr,
                                   lpstKey,
                                   5,
                                   &bySheetType,
                                   1);
                }
            }
            else
            {
                // Set dummy sheet type

                bySheetType = XL_REC_BOUNDSHEET_WORKSHEET;
            }

            if (bySheetType == XL_REC_BOUNDSHEET_40_MACRO)
            {
                /////////////////////////////////////////////////
                // CRC the name

                wNameBytesLeft = lpstHdr->wLen - 6;

                // Update the number of bytes to CRC

                *lpdwCRCSize += wNameBytesLeft;

                // Determine whether we have a hanging record

                if (nNumBytes - i < lpstHdr->wLen)
                {
                    // Get the offset of the data to CRC

                    dwOffset += i + 6;

                    while (wNameBytesLeft != 0)
                    {
                        // Read a chunk

                        if (wNameBytesLeft < (WORD)nWorkBufSize)
                            nNumBytes = (int)wNameBytesLeft;
                        else
                            nNumBytes = nWorkBufSize;

                        // Fill the buffer

                        if (SSSeekRead(lpstStream,
                                       dwOffset,
                                       lpabyWorkBuf,
                                       nNumBytes,
                                       &dwCount) != SS_STATUS_OK ||
                            (int)dwCount != nNumBytes)
                            return(FALSE);

                        // Decrypt the buffer if necessary

                        if (lpstKey != NULL && lpstKey->bEncrypted != FALSE)
                        {
                            XL5DecryptData(dwHdrOffset,
                                           stHdr,
                                           lpstKey,
                                           stHdr.wLen - wNameBytesLeft,
                                           lpabyWorkBuf,
                                           (WORD)nNumBytes);
                        }

                        // CRC the buffer

                        *lpdwCRC = CRC32Compute(nNumBytes,
                                                lpabyWorkBuf,
                                                *lpdwCRC);

                        // Update number of bytes left and offset

                        wNameBytesLeft -= nNumBytes;
                        dwOffset += nNumBytes;
                    }
                    break;
                }
                else
                {

                    // Decrypt the buffer if necessary

                    if (lpstKey != NULL && lpstKey->bEncrypted != FALSE)
                    {
                        XL5DecryptData(dwHdrOffset,
                                       *lpstHdr,
                                       lpstKey,
                                       6,
                                       lpabyWorkBuf + i + 6,
                                       wNameBytesLeft);
                    }

                    *lpdwCRC = CRC32Compute(wNameBytesLeft,
                                            lpabyWorkBuf + i + 6,
                                            *lpdwCRC);
                }
            }
            else
            {
                // Determine whether we have a hanging record

                if (nNumBytes - i < lpstHdr->wLen)
                {
                    dwOffset += i + lpstHdr->wLen;
                    break;
                }
            }

            // Increment past the record data

            i += lpstHdr->wLen;
        }
    }

    // Did not find EOF

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL MSXXL4SheetCRC()
//
// Parameters:
//  lpstStream          Ptr to Book/Workbook stream
//  lpabyWorkBuf        Ptr to work buffer
//  nWorkBufSize        Size of work buffer
//  dwOffset            Offset of sheet BOF record
//  lpdwCRC             Ptr to DWORD of CRC to continue
//  lpdwCRCSize         Ptr to DWORD of CRC size
//
// Description:
//  The function CRCs the following record types for a given
//  Excel 4.0 sheet: ARRAY, FORMULA, SHRFMLA.
//
//  The function does not do any decryption.
//
// Returns:
//  FALSE               On error
//  TRUE                On success
//
//********************************************************************

BOOL MSXXL4SheetCRC
(
    LPSS_STREAM         lpstStream,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    DWORD               dwOffset,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
)
{
    DWORD               dwLimit;
    DWORD               dwCount;
    int                 nBOFDepth;
    int                 nNumBytes;
    LPXL_REC_HDR        lpstHdr;
    WORD                wExprOffset;
    WORD                wExprBytesLeft;
    int                 i;

    // Iterate through all the records

    nBOFDepth = 0;
    dwLimit = SSStreamLen(lpstStream);
    while (dwOffset < dwLimit)
    {
        // Read a chunk

        if (dwLimit - dwOffset < (DWORD)nWorkBufSize)
            nNumBytes = (int)(dwLimit - dwOffset);
        else
            nNumBytes = nWorkBufSize;

        // Make sure that there are at least enough bytes
        //  for a record header

        if (nNumBytes < sizeof(XL_REC_HDR_T))
            return(FALSE);

        // Refill the buffer

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpabyWorkBuf,
                       nNumBytes,
                       &dwCount) != SS_STATUS_OK ||
            (int)dwCount != nNumBytes)
            return(FALSE);

        // Iterate through the buffer looking for an EOF

        i = 0;
        while (1)
        {
            if ((nNumBytes - i) < sizeof(XL_REC_HDR_T))
            {
                dwOffset += i;
                break;
            }

            lpstHdr = (LPXL_REC_HDR)(lpabyWorkBuf + i);
            lpstHdr->wType = WENDIAN(lpstHdr->wType);
            lpstHdr->wLen = WENDIAN(lpstHdr->wLen);

            wExprOffset = lpstHdr->wLen;
            switch (lpstHdr->wType)
            {
                case eXLREC_BOF:
                    ++nBOFDepth;
                    break;

                case eXLREC_EOF:
                    if (nBOFDepth == 0)
                        return(FALSE);      // Unmatched EOF

                    if (--nBOFDepth == 0)
                        return(TRUE);

                    break;

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
                    break;
            }

            // Increment past the header

            i += sizeof(XL_REC_HDR_T);

            // Do not need to CRC if the expression offset
            //  is >= the length of the record

            if (wExprOffset >= lpstHdr->wLen)
            {
                // Determine whether we have a hanging record

                if (nNumBytes - i < lpstHdr->wLen)
                {
                    dwOffset += i + lpstHdr->wLen;
                    break;
                }
            }
            else
            {
                /////////////////////////////////////////////////
                // CRC the expression

                wExprBytesLeft = lpstHdr->wLen - wExprOffset;

                // Update the number of bytes to CRC

                *lpdwCRCSize += wExprBytesLeft;

                // Determine whether we have a hanging record

                if (nNumBytes - i < lpstHdr->wLen)
                {
                    // Get the offset of the data to CRC

                    dwOffset += i + wExprOffset;

                    while (wExprBytesLeft != 0)
                    {
                        // Read a chunk

                        if (wExprBytesLeft < (WORD)nWorkBufSize)
                            nNumBytes = (int)wExprBytesLeft;
                        else
                            nNumBytes = nWorkBufSize;

                        // Fill the buffer

                        if (SSSeekRead(lpstStream,
                                       dwOffset,
                                       lpabyWorkBuf,
                                       nNumBytes,
                                       &dwCount) != SS_STATUS_OK ||
                            (int)dwCount != nNumBytes)
                            return(FALSE);

                        // CRC the buffer

                        *lpdwCRC = CRC32Compute(nNumBytes,
                                                lpabyWorkBuf,
                                                *lpdwCRC);

                        // Update number of bytes left and offset

                        wExprBytesLeft -= nNumBytes;
                        dwOffset += nNumBytes;
                    }
                    break;
                }
                else
                {
                    *lpdwCRC = CRC32Compute(wExprBytesLeft,
                                            lpabyWorkBuf + i + wExprOffset,
                                            *lpdwCRC);
                }
            }

            // Increment past the record data

            i += lpstHdr->wLen;
        }
    }

    // Did not find EOF

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL MSXXL4EncryptedSheetCRC()
//
// Parameters:
//  lpstStream          Ptr to Book stream
//  lpstKey             Ptr to encryption key
//  lpabyWorkBuf        Ptr to work buffer
//  nWorkBufSize        Size of work buffer
//  dwOffset            Offset of sheet BOF record
//  lpdwCRC             Ptr to DWORD of CRC to continue
//  lpdwCRCSize         Ptr to DWORD of CRC size
//
// Description:
//  The function CRCs the entire set of records for a given
//  encrypted Excel 4.0 sheet.
//
//  The function assumes that the sheet is actually encrypted,
//  so lpstKey must be valid.
//
// Returns:
//  FALSE               On error
//  TRUE                On success
//
//********************************************************************

BOOL MSXXL4EncryptedSheetCRC
(
    LPSS_STREAM         lpstStream,
    LPXL5ENCKEY         lpstKey,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    DWORD               dwOffset,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
)
{
    DWORD               dwLimit;
    DWORD               dwCount;
    int                 nBOFDepth;
    int                 nNumBytes;
    LPXL_REC_HDR        lpstHdr;
    WORD                wExprOffset;
    WORD                wExprBytesLeft;
    XL_REC_HDR_T        stHdr;
    DWORD               dwHdrOffset;
    int                 i;

    // Iterate through all the records

    nBOFDepth = 0;
    dwLimit = SSStreamLen(lpstStream);
    while (dwOffset < dwLimit)
    {
        // Read a chunk

        if (dwLimit - dwOffset < (DWORD)nWorkBufSize)
            nNumBytes = (int)(dwLimit - dwOffset);
        else
            nNumBytes = nWorkBufSize;

        // Make sure that there are at least enough bytes
        //  for a record header

        if (nNumBytes < sizeof(XL_REC_HDR_T))
            return(FALSE);

        // Refill the buffer

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpabyWorkBuf,
                       nNumBytes,
                       &dwCount) != SS_STATUS_OK ||
            (int)dwCount != nNumBytes)
            return(FALSE);

        // Iterate through the buffer looking for an EOF

        i = 0;
        while (1)
        {
            if ((nNumBytes - i) < sizeof(XL_REC_HDR_T))
            {
                dwOffset += i;
                break;
            }

            lpstHdr = (LPXL_REC_HDR)(lpabyWorkBuf + i);
            lpstHdr->wType = WENDIAN(lpstHdr->wType);
            lpstHdr->wLen = WENDIAN(lpstHdr->wLen);

            wExprOffset = lpstHdr->wLen;
            switch (lpstHdr->wType)
            {
                case eXLREC_BOF:
                    ++nBOFDepth;
                    break;

                case eXLREC_EOF:
                    if (nBOFDepth == 0)
                        return(FALSE);      // Unmatched EOF

                    if (--nBOFDepth == 0)
                        return(TRUE);

                    break;

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
                    break;
            }

            // Increment past the header

            i += sizeof(XL_REC_HDR_T);

            // Do not need to CRC if the expression offset
            //  is >= the length of the record

            if (wExprOffset >= lpstHdr->wLen)
            {
                // Determine whether we have a hanging record

                if (nNumBytes - i < lpstHdr->wLen)
                {
                    dwOffset += i + lpstHdr->wLen;
                    break;
                }
            }
            else
            {
                /////////////////////////////////////////////////
                // CRC the expression

                wExprBytesLeft = lpstHdr->wLen - wExprOffset;

                dwHdrOffset = dwOffset + i - sizeof(XL_REC_HDR_T);

                // Update the number of bytes to CRC

                *lpdwCRCSize += wExprBytesLeft;

                // Determine whether we have a hanging record

                if (nNumBytes - i < lpstHdr->wLen)
                {
                    // Get the offset of the data to CRC

                    dwOffset += i + wExprOffset;

                    stHdr = *lpstHdr;

                    while (wExprBytesLeft != 0)
                    {
                        // Read a chunk

                        if (wExprBytesLeft < (WORD)nWorkBufSize)
                            nNumBytes = (int)wExprBytesLeft;
                        else
                            nNumBytes = nWorkBufSize;

                        // Fill the buffer

                        if (SSSeekRead(lpstStream,
                                       dwOffset,
                                       lpabyWorkBuf,
                                       nNumBytes,
                                       &dwCount) != SS_STATUS_OK ||
                            (int)dwCount != nNumBytes)
                            return(FALSE);

                        // Decrypt the buffer

                        XL5DecryptData(dwHdrOffset,
                                       stHdr,
                                       lpstKey,
                                       stHdr.wLen - wExprBytesLeft,
                                       lpabyWorkBuf,
                                       (WORD)nNumBytes);

                        // CRC the buffer

                        *lpdwCRC = CRC32Compute(nNumBytes,
                                                lpabyWorkBuf,
                                                *lpdwCRC);

                        // Update number of bytes left and offset

                        wExprBytesLeft -= nNumBytes;
                        dwOffset += nNumBytes;
                    }
                    break;
                }
                else
                {
                    XL5DecryptData(dwHdrOffset,
                                   *lpstHdr,
                                   lpstKey,
                                   wExprOffset,
                                   lpabyWorkBuf + i + wExprOffset,
                                   wExprBytesLeft);

                    *lpdwCRC = CRC32Compute(wExprBytesLeft,
                                            lpabyWorkBuf + i + wExprOffset,
                                            *lpdwCRC);
                }
            }

            // Increment past the record data

            i += lpstHdr->wLen;
        }
    }

    // Did not find EOF

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL MSXXL4SheetSetCRC()
//
// Parameters:
//  lpstStream          Ptr to Book/Workbook stream
//  lpstKey             Ptr to encryption key
//  lpabyWorkBuf        Ptr to work buffer
//  nWorkBufSize        Size of work buffer
//  dwBoundSheetOffset  First BOUNDSHEET record offset
//  lpdwCRC             Ptr to DWORD of CRC to continue
//  lpdwCRCSize         Ptr to DWORD of CRC size
//
// Description:
//  The function CRCs the entire set of records for each Excel 4.0
//  sheet.
//
// Returns:
//  FALSE               On error or no BOUNDSHEETs found
//  TRUE                If a BOUNDSHEET record is found
//
//********************************************************************

BOOL MSXXL4SheetSetCRC
(
    LPSS_STREAM         lpstStream,
    LPXL5ENCKEY         lpstKey,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    DWORD               dwBoundSheetOffset,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
)
{
    DWORD               dwLimit;
    XL_REC_HDR_T        stRec;
    XL_REC_BOUNDSHEET_T stBoundSheet;
    DWORD               dwCount;
    WORD                wType;

    // Initialize the CRC

    CRC32Init(*lpdwCRC);
    *lpdwCRCSize = 0;

    // Iterate through all the BOUNDSHEET records

    dwLimit = SSStreamLen(lpstStream);
    while (dwBoundSheetOffset < dwLimit)
    {
        if (SSSeekRead(lpstStream,
                       dwBoundSheetOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwCount) != SS_STATUS_OK ||
            dwCount != sizeof(XL_REC_HDR_T))
            return(FALSE);

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_EOF)
            return(TRUE);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            // Read the BOUNDSHEET record

            if (XL5EncryptedRead(lpstStream,
                                 dwBoundSheetOffset,
                                 stRec,
                                 lpstKey,
                                 0,
                                 (LPBYTE)&stBoundSheet,
                                 sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
                return(FALSE);

            stBoundSheet.dwOffset = DWENDIAN(stBoundSheet.dwOffset);
            if (lpstKey == NULL || lpstKey->bEncrypted == FALSE)
            {
                // Is it an Excel 4.0 sheet?

                if ((stBoundSheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                    XL_REC_BOUNDSHEET_40_MACRO)
                {
                    // CRC the sheet records

                    if (MSXXL4SheetCRC(lpstStream,
                                       lpabyWorkBuf,
                                       nWorkBufSize,
                                       stBoundSheet.dwOffset,
                                       lpdwCRC,
                                       lpdwCRCSize) == FALSE)
                        return(FALSE);
                }
            }
            else
            {
                // Read the WORD at offset six of the BOF record

                if (SSSeekRead(lpstStream,
                               stBoundSheet.dwOffset + 6,
                               &wType,
                               sizeof(WORD),
                               &dwCount) != SS_STATUS_OK ||
                    dwCount != sizeof(WORD))
                    return(FALSE);

                wType = WENDIAN(wType);

                if (wType == 0x0040)
                {
                    if (MSXXL4EncryptedSheetCRC(lpstStream,
                                                lpstKey,
                                                lpabyWorkBuf,
                                                nWorkBufSize,
                                                stBoundSheet.dwOffset,
                                                lpdwCRC,
                                                lpdwCRCSize) == FALSE)
                        return(FALSE);
                }
            }
        }

		dwBoundSheetOffset += (DWORD)sizeof(XL_REC_HDR_T) + stRec.wLen;
	}

    // Did not find EOF

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL MSXL2_VBA_PROJECTCRC()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure
//  dwChildID           ID of child of _VBA_PROJECT storage
//  lpstSibs            Ptr to sibling enumeration structure to use
//  lpabyWorkBuf        Ptr to work buffer
//  nWorkBufSize        Size of work buffer
//  lpdwCRC             Ptr to DWORD for CRC
//  lpdwCRCSize         Ptr to DWORD for CRC size
//
// Description:
//  The function does performs a sibling enumeration ordered
//  CRC of all the hexadecimally named streams.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXL2_VBA_PROJECTCRC
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwChildID,
    LPSS_ENUM_SIBS      lpstSibs,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
)
{
    int                 i;

    // Initialize the CRC

    CRC32Init(*lpdwCRC);
    *lpdwCRCSize = 0;

    // Start with the child

    SSInitEnumSibsStruct(lpstSibs,dwChildID);

    for (i=0;i<16384;i++)
    {
        if (SSEnumSiblingEntriesCB(lpstRoot,
                                   XL5OpenHexNamedStreamCB,
                                   NULL,
                                   lpstSibs,
                                   lpstStream) != SS_STATUS_OK)
            break;

        // CRC the stream

        if (MSXCRCStream(lpstStream,
                         lpabyWorkBuf,
                         nWorkBufSize,
                         0,
                         SSStreamLen(lpstStream),
                         lpdwCRC,
                         lpdwCRCSize) == FALSE)
            return(FALSE);
    }

    if (i == 16384)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXL2VBACRC()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure
//  dwChildID           ID of child of VBA storage
//  lpstSibs            Ptr to sibling enumeration structure to use
//  lpabyWorkBuf        Ptr to work buffer
//  nWorkBufSize        Size of work buffer
//  lpdwCRC             Ptr to DWORD for CRC
//  lpdwCRCSize         Ptr to DWORD for CRC size
//
// Description:
//  The function does performs a sibling enumeration ordered
//  CRC of all the module streams.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXL2VBACRC
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwChildID,
    LPSS_ENUM_SIBS      lpstSibs,
    LPBYTE              lpabyWorkBuf,
    int                 nWorkBufSize,
    LPDWORD             lpdwCRC,
    LPDWORD             lpdwCRCSize
)
{
    int                 i;

    // Initialize the CRC

    CRC32Init(*lpdwCRC);
    *lpdwCRCSize = 0;

    // Start with the child

    SSInitEnumSibsStruct(lpstSibs,dwChildID);

    for (i=0;i<16384;i++)
    {
        if (SSEnumSiblingEntriesCB(lpstRoot,
                                   O97OpenVBA5ModuleCB,
                                   NULL,
                                   lpstSibs,
                                   lpstStream) != SS_STATUS_OK)
            break;

        // CRC the stream

        if (MSXCRCStream(lpstStream,
                         lpabyWorkBuf,
                         nWorkBufSize,
                         0,
                         SSStreamLen(lpstStream),
                         lpdwCRC,
                         lpdwCRCSize) == FALSE)
            return(FALSE);
    }

    if (i == 16384)
        return(FALSE);

    return(TRUE);
}



