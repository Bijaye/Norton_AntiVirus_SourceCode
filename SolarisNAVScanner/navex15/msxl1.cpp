// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/msxl1.cpv   1.2   16 Dec 1998 14:16:38   DCHI  $
//
// Description:
//  Contains the following functions:
//
//      int MSXL1CB()
//      int MSXWordDocument()
//      int MSXBook()
//      int MSXWorkbook()
//      int MSXL1dir()
//      int MSXL1Filter()
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/msxl1.cpv  $
// 
//    Rev 1.2   16 Dec 1998 14:16:38   DCHI
// Filtering now also checks _VBA_PROJECT stream.
// 
//    Rev 1.1   15 Dec 1998 12:14:24   DCHI
// If "PowerPoint Document" is seen, don't do MSX.
// 
//    Rev 1.0   08 Dec 1998 12:53:14   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

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
//  int MSXL1CB()
//
// Parameters:
//  lpstEntry           Ptr to the entry
//  dwIndex             The entry's index in the directory
//  lpvCookie           Cookie ptr
//
// Description:
//  If the entry is that of a stream named one of the following,
//  then the stream is opened:
//
//      1. WordDocument
//      2. Book
//      3. Workbook
//      4. dir
//      5. _VBA_PROJECT
//
//  Otherwise, the function returns the CONTINUE result.
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN          If an interesting stream is found
//  SS_ENUM_CB_STATUS_CONTINUE      Otherwise
//
//********************************************************************

int MSXL1CB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Check for "WordDocument"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWordDocument,
                       SS_MAX_NAME_LEN) == 0)
        {
            *(LPINT)lpvCookie = MSX_L1_WORDDOCUMENT;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "Book"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszBook,
                       SS_MAX_NAME_LEN) == 0)
        {
            *(LPINT)lpvCookie = MSX_L1_BOOK;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "Workbook"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWorkbook,
                       SS_MAX_NAME_LEN) == 0)
        {
            *(LPINT)lpvCookie = MSX_L1_WORKBOOK;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "dir"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszdir,
                       SS_MAX_NAME_LEN) == 0)
        {
            *(LPINT)lpvCookie = MSX_L1_DIR;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "_VBA_PROJECT"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz_VBA_PROJECT,
                       SS_MAX_NAME_LEN) == 0)
        {
            *(LPINT)lpvCookie = MSX_L1__VBA_PROJECT;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "PowerPoint Document"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszPowerPointDocument,
                       SS_MAX_NAME_LEN) == 0)
        {
            *(LPINT)lpvCookie = MSX_L1_POWERPOINT;
            return(SS_ENUM_CB_STATUS_OPEN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int MSXWordDocument()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstStream          Ptr to WordDocument stream
//
// Description:
//  The function does the following:
//  1. Determines whether it is a Word 6.0/95 document stream.
//  2. If not, then it returns MSX_ALL_CLEAN.
//  3. Obtains the encryption key.
//  4. If the encryption key is not found, it returns MSX_ALL_CLEAN.
//  5. Obtains the offset and size of the TDT.
//  6. Obtains the offset of the MCDs and their count.
//  7. If there are no MCDs, it returns MSX_ALL_CLEAN.
//  8. Computes a CRC on the MCDs.
//  9. Checks the database with the CRC.
//  10. If the database does not have this L1 CRC,
//      returns MSX_NORMAL_SCAN.
//  11. Computes a continuous CRC on the macro bodies in MCD order.
//  12. If the database does not have this L2 CRC,
//      returns MSX_NORMAL_SCAN.
//
// Returns:
//  MSX_ALL_CLEAN       If the stream is clean
//  MSX_NORMAL_SCAN     If the CRC did not match
//
//********************************************************************

int MSXWordDocument
(
    LPMSX               lpstMSX,
    LPSS_STREAM         lpstStream
)
{
    WORD                wTemp;
    DWORD               dwCount;
    DWORD               dwTDTOffset;
    DWORD               dwTDTSize;
    WD7ENCKEY_T         stKey;
    DWORD               dwMCDOffset;
    WORD                wMCDCount;
    DWORD               dwCRC;
    DWORD               dwCRCByteCount;

    ////////////////////////////////////////////////////////
    // Check magic number

    // Read the magic number

    if (SSSeekRead(lpstStream,
                   0,
                   &wTemp,
                   sizeof(WORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(WORD))
        return(MSX_ALL_CLEAN);

    wTemp = WENDIAN(wTemp);

    // If it is not a known magic number, assume it is not
    //  a Word 6.0/95 document stream

    if (WD7IsKnownMagicNumber(wTemp) == FALSE)
        return(MSX_ALL_CLEAN);

    ////////////////////////////////////////////////////////
    // Get encryption key

    // If the key could not be gotten, assume the stream
    //  is clean, since we wouldn't be able to scan it anyway

    if (WD7FindFirstKey(lpstStream,
                        &stKey,
                        &gstRevKeyLocker) != WD7_STATUS_OK)
        return(MSX_ALL_CLEAN);

    // Get the TDT offset and size, if error, return
    //  normal scan, since the actual key might come later

    if (WD7EncryptedRead(lpstStream,
                         &stKey,
                         280,
                         (LPBYTE)&dwTDTOffset,
                         sizeof(DWORD)) != sizeof(DWORD) ||
        WD7EncryptedRead(lpstStream,
                         &stKey,
                         284,
                         (LPBYTE)&dwTDTSize,
                         sizeof(DWORD)) != sizeof(DWORD))
        return(MSX_NORMAL_SCAN);

    dwTDTOffset = DWENDIAN(dwTDTOffset);
    dwTDTSize = DWENDIAN(dwTDTSize);

    // Find the location of the MCDs

    if (WD7GetMCDInfo(lpstStream,
                      &stKey,
                      dwTDTOffset,
                      dwTDTSize,
                      &dwMCDOffset,
                      &wMCDCount) == FALSE)
        return(MSX_NORMAL_SCAN);

    // If there were no MCDs, assume clean

    if (wMCDCount == 0)
        return(MSX_ALL_CLEAN);

    // CRC the MCDs

    if (MSXL1WordDocumentCRC(lpstStream,
                             &stKey,
                             dwMCDOffset,
                             wMCDCount,
                             lpstMSX->lpabyWorkBuf,
                             lpstMSX->nWorkBufSize,
                             &dwCRC,
                             &dwCRCByteCount) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Check the database

    if (MSXDBL1Match(lpstMSX,
                     dwCRC,
                     dwCRCByteCount) == FALSE)
        return(MSX_NORMAL_SCAN);

    // CRC the macro bodies

    if (MSXL2WordDocumentCRC(lpstStream,
                             &stKey,
                             dwMCDOffset,
                             wMCDCount,
                             lpstMSX->lpabyWorkBuf,
                             lpstMSX->nWorkBufSize,
                             &dwCRC,
                             &dwCRCByteCount) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Check the database

    if (MSXDBL2Match(lpstMSX,
                     dwCRC,
                     dwCRCByteCount) == FALSE)
        return(MSX_NORMAL_SCAN);

    return(MSX_ALL_CLEAN);
}


//********************************************************************
//
// Function:
//  int MSXBook()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstStream          Ptr to Book stream
//
// Description:
//  The function does the following:
//  1. Tries to obtain the encryption key.
//  2. If the encryption key could not be obtained,
//      return MSX_ALL_CLEAN.
//  3. Bulk scan, looking for the BOUNDSHEET records
//  4. CRC all XL 4.0 sheet names
//  5. Check the database for an L1 match on the CRC if the
//      CRC byte count is non-zero
//  6. CRC all XL 4.0 sheets
//  7. Check the database for an L2 match on the CRC if the
//      CRC byte count is non-zero
//
// Returns:
//  MSX_ALL_CLEAN       If the stream is clean
//  MSX_NORMAL_SCAN     If the CRC did not match
//
//********************************************************************

int MSXBook
(
    LPMSX               lpstMSX,
    LPSS_STREAM         lpstStream
)
{
    XL5ENCKEY_T         stKey;
    DWORD               dwOffset;
    DWORD               dwCRC;
    DWORD               dwCRCSize;

    // Get the encryption key first

    if (XL5FindKey(lpstStream,
                   &stKey) == FALSE)
        return(MSX_ALL_CLEAN);

    // Find the first BOUNDSHEET record

    if (MSXXLFindFirstBoundsheet(lpstStream,
                                 lpstMSX->lpabyWorkBuf,
                                 lpstMSX->nWorkBufSize,
                                 &dwOffset) == FALSE)
        return(MSX_ALL_CLEAN);

    // At this point, the record should be a boundsheet

    if (MSXXL4SheetNameSetCRC(lpstStream,
                              &stKey,
                              lpstMSX->lpabyWorkBuf,
                              lpstMSX->nWorkBufSize,
                              dwOffset,
                              &dwCRC,
                              &dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // If the CRC size is zero, then there are no 4.0 macro sheets

    if (dwCRCSize == 0)
        return(MSX_ALL_CLEAN);

    // Check the database

    if (MSXDBL1Match(lpstMSX,
                     dwCRC,
                     dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Iterate through the boundsheet records

    if (MSXXL4SheetSetCRC(lpstStream,
                          &stKey,
                          lpstMSX->lpabyWorkBuf,
                          lpstMSX->nWorkBufSize,
                          dwOffset,
                          &dwCRC,
                          &dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Check the database

	if (dwCRCSize != 0)
	{
		if (MSXDBL2Match(lpstMSX,
			             dwCRC,
				         dwCRCSize) == FALSE)
			return(MSX_NORMAL_SCAN);
	}

    return(MSX_ALL_CLEAN);
}


//********************************************************************
//
// Function:
//  int MSXWorkbook()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstStream          Ptr to Workbook stream
//
// Description:
//  The function does the following:
//  1. Determines whether the stream is encrypted.
//  2. If the stream is encrypted, returns MSX_ALL_CLEAN
//  3. Bulk scan, looking for the BOUNDSHEET records
//  4. CRC all XL 4.0 sheet names
//  5. Check the database for an L1 match on the CRC if the
//      CRC byte count is non-zero
//  6. CRC all XL 4.0 sheets
//  7. Check the database for an L2 match on the CRC if the
//      CRC byte count is non-zero
//
// Returns:
//  MSX_ALL_CLEAN       If the stream is clean
//  MSX_NORMAL_SCAN     If the CRC did not match
//
//********************************************************************

int MSXWorkbook
(
    LPMSX               lpstMSX,
    LPSS_STREAM         lpstStream
)
{
    BOOL                bEncrypted;
    DWORD               dwOffset;
    DWORD               dwCRC;
    DWORD               dwCRCSize;

    // Determine whether the stream is encrypted
    //  If error or if it is encrypted, return clean

    if (XL97WorkbookIsEncrypted(lpstStream,
                                &bEncrypted) == FALSE ||
        bEncrypted != FALSE)
        return(MSX_ALL_CLEAN);

    // Find the first BOUNDSHEET record

    if (MSXXLFindFirstBoundsheet(lpstStream,
                                 lpstMSX->lpabyWorkBuf,
                                 lpstMSX->nWorkBufSize,
                                 &dwOffset) == FALSE)
        return(MSX_ALL_CLEAN);

    // At this point, the record should be a boundsheet

    if (MSXXL4SheetNameSetCRC(lpstStream,
                              NULL,
                              lpstMSX->lpabyWorkBuf,
                              lpstMSX->nWorkBufSize,
                              dwOffset,
                              &dwCRC,
                              &dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // If the CRC size is zero, then there are no 4.0 macro sheets

    if (dwCRCSize == 0)
        return(MSX_ALL_CLEAN);

    // Check the database

    if (MSXDBL1Match(lpstMSX,
                     dwCRC,
                     dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Iterate through the boundsheet records

    if (MSXXL4SheetSetCRC(lpstStream,
                          NULL,
                          lpstMSX->lpabyWorkBuf,
                          lpstMSX->nWorkBufSize,
                          dwOffset,
                          &dwCRC,
                          &dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Check the database

	if (dwCRCSize != 0)
	{
	    if (MSXDBL2Match(lpstMSX,
		                 dwCRC,
			             dwCRCSize) == FALSE)
			return(MSX_NORMAL_SCAN);
	}

    return(MSX_ALL_CLEAN);
}


//********************************************************************
//
// Function:
//  int MSXL1dir()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstStream          Ptr to dir stream
//
// Description:
//  The function does the following:
//  1. CRCs the entire stream
//  2. Looks for the CRC in the database.
//  3. If the CRC is not present in the database,
//      returns MSX_NORMAL_SCAN.
//  4. Otherwise, retursn MSX_L2_FILTER
//
// Returns:
//  MSX_NORMAL_SCAN     If the CRC did not match
//  MSX_L2_FILTER       If the L2 filter should be applied
//
//********************************************************************

int MSXL1dir
(
    LPMSX               lpstMSX,
    LPSS_STREAM         lpstStream
)
{
    DWORD               dwCRC;
    DWORD               dwCRCSize;

    // Initialize the CRC

    CRC32Init(dwCRC);
    dwCRCSize = 0;

    // CRC the stream

    if (MSXCRCStream(lpstStream,
                     lpstMSX->lpabyWorkBuf,
                     lpstMSX->nWorkBufSize,
                     0,
                     SSStreamLen(lpstStream),
                     &dwCRC,
                     &dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Check the database

    if (MSXDBL1Match(lpstMSX,
                     dwCRC,
                     dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    return(MSX_L2_FILTER);
}


//********************************************************************
//
// Function:
//  int MSXL1_VBA_PROJECT()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstStream          Ptr to _VBA_PROJECT stream
//
// Description:
//  The function does the following:
//  1. CRCs the entire stream
//  2. Looks for the CRC in the database.
//  3. If the CRC is not present in the database,
//      returns MSX_NORMAL_SCAN.
//  4. Otherwise, retursn MSX_L2_FILTER
//
// Returns:
//  MSX_NORMAL_SCAN     If the CRC did not match
//  MSX_L2_FILTER       If the L2 filter should be applied
//
//********************************************************************

int MSXL1_VBA_PROJECT
(
    LPMSX               lpstMSX,
    LPSS_STREAM         lpstStream
)
{
    DWORD               dwCRC;
    DWORD               dwCRCSize;

    // Initialize the CRC

    CRC32Init(dwCRC);
    dwCRCSize = 0;

    // CRC the stream

    if (MSXCRCStream(lpstStream,
                     lpstMSX->lpabyWorkBuf,
                     lpstMSX->nWorkBufSize,
                     0,
                     SSStreamLen(lpstStream),
                     &dwCRC,
                     &dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    // Check the database

    if (MSXDBL1Match(lpstMSX,
                     dwCRC,
                     dwCRCSize) == FALSE)
        return(MSX_NORMAL_SCAN);

    return(MSX_L2_FILTER);
}


//********************************************************************
//
// Function:
//  int MSXL1Filter()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure to use
//
// Description:
//  The function looks for streams with the following names:
//      1. WordDocument
//      2. Book
//      3. Workbook
//      4. dir
//
//  If all streams are clean and there were no CRC matches,
//  assume the file is clean.
//
//  If all streams are clean and there all CRCs are matches,
//  assume perform second-level filter.
//
//  Otherwise, perform normal scan.
//
// Returns:
//  MSX_ALL_CLEAN       If all streams are clean
//  MSX_NORMAL_SCAN     If there was a CRC that did not match
//  MSX_L2_FILTER       If the L2 filter should be applied
//
//********************************************************************

int MSXL1Filter
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,

    LPSS_STREAM         lpstStream
)
{
    int                 i;
    int                 nType;
    int                 nResult;
    DWORD               dwDirID;
    BOOL                bCRCComputed;

    bCRCComputed = FALSE;
    dwDirID = 0;
    for (i=0;i<16384;i++)
    {
        if (SSEnumDirEntriesCB(lpstRoot,
                               MSXL1CB,
                               &nType,
                               &dwDirID,
                               lpstStream) != SS_STATUS_OK)
            break;

        switch (nType)
        {
            case MSX_L1_WORDDOCUMENT:
                nResult = MSXWordDocument(lpstMSX,lpstStream);
                break;

            case MSX_L1_BOOK:
                nResult = MSXBook(lpstMSX,lpstStream);
                break;

            case MSX_L1_WORKBOOK:
                nResult = MSXWorkbook(lpstMSX,lpstStream);
                break;

            case MSX_L1_DIR:
                nResult = MSXL1dir(lpstMSX,lpstStream);
                break;

            case MSX_L1__VBA_PROJECT:
                nResult = MSXL1_VBA_PROJECT(lpstMSX,lpstStream);
                break;

            case MSX_L1_POWERPOINT:
                return(MSX_NORMAL_SCAN);
                
            default:
                break;
        }

        // Return immediately if the result is MSX_NORMAL_SCAN.
        //  If the result is MSX_L2_FILTER, note that a CRC
        //  has been computed.  Otherwise, do nothing.

        switch (nResult)
        {
            case MSX_NORMAL_SCAN:
                return(MSX_NORMAL_SCAN);

            case MSX_L2_FILTER:
                bCRCComputed = TRUE;
                break;

            case MSX_ALL_CLEAN:
            default:
                break;
        }
    }

    // At this point, we have gone through the entire directory,
    //  so the result must be either MSX_ALL_CLEAN or
    //  MSX_L2_FILTER

    if (bCRCComputed == FALSE)
    {
        // All examined streams are clean

        return(MSX_ALL_CLEAN);
    }

    // All computed CRCs are in the database

    return(MSX_L2_FILTER);
}   


