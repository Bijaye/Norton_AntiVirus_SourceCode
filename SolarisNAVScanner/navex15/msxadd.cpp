// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/msxadd.cpv   1.1   16 Dec 1998 14:15:56   DCHI  $
//
// Description:
//  Contains the following functions:
//
//      int  MSXAddCRCCB()
//      BOOL MSXAddWordDocument()
//      BOOL MSXAddBook()
//      BOOL MSXAddWorkbook()
//      BOOL MSXAdd_VBA_PROJECT()
//      BOOL MSXAddVBA()
//      BOOL MSXAddCRCs()
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/msxadd.cpv  $
// 
//    Rev 1.1   16 Dec 1998 14:15:56   DCHI
// Modified MSXAddVBA() to also add a CRC pair with the _VBA_PROJECT stream.
// 
//    Rev 1.0   08 Dec 1998 12:53:16   DCHI
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

typedef struct tagMSXADD_CRC_INFO
{
    int             nType;
    DWORD           dwID;
    DWORD           dwChildID;
} MSXADD_CRC_INFO_T, FAR *LPMSXADD_CRC_INFO;

//********************************************************************
//
// Function:
//  int MSXAddCRCCB()
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
//
//  If the entry is that of a storage named one of the following,
//  then the function returns the return result:
//
//      1. _VBA_PROJECT
//      2. VBA
//
//  Otherwise, the function returns the CONTINUE result.
//
// Returns:
//  SS_ENUM_CB_STATUS_RETURN        If an interesting storage is found
//  SS_ENUM_CB_STATUS_OPEN          If an interesting stream is found
//  SS_ENUM_CB_STATUS_CONTINUE      Otherwise
//
//********************************************************************

int MSXAddCRCCB
(
    LPSS_DIR_ENTRY      lpstEntry,
    DWORD               dwIndex,
    LPVOID              lpvCookie
)
{
    LPMSXADD_CRC_INFO   lpstInfo;

    lpstInfo = (LPMSXADD_CRC_INFO)lpvCookie;

    // Set up these fields since they are common

    lpstInfo->dwID = dwIndex;
    lpstInfo->dwChildID = DWENDIAN(lpstEntry->dwSIDChild);

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Check for "WordDocument"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWordDocument,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstInfo->nType = MSX_L1_WORDDOCUMENT;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "Book"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszBook,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstInfo->nType = MSX_L1_BOOK;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "Workbook"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWorkbook,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstInfo->nType = MSX_L1_WORKBOOK;
            return(SS_ENUM_CB_STATUS_OPEN);
        }
    }
    else
    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        // Check for "_VBA_PROJECT"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz_VBA_PROJECT,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstInfo->nType = MSX_L2__VBA_PROJECT;
            return(SS_ENUM_CB_STATUS_RETURN);
        }

        // Check for "VBA"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszVBA,
                       SS_MAX_NAME_LEN) == 0)
        {
            lpstInfo->nType = MSX_L2_VBA;
            return(SS_ENUM_CB_STATUS_RETURN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXAddWordDocument()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstStream          Ptr to WordDocument stream
//
// Description:
//  The function does the following:
//  1. Determines whether it is a Word 6.0/95 document stream.
//  2. If not, then it returns TRUE.
//  3. Obtains the encryption key.
//  4. If the encryption key is not found, it returns TRUE.
//  5. Obtains the offset and size of the TDT.
//  6. Obtains the offset of the MCDs and their count.
//  7. If there are no MCDs, it returns TRUE.
//  8. Computes a CRC on the MCDs.
//  9. Computes a continuous CRC on the macro bodies in MCD order.
//  10. Adds the CRC data to the database.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXAddWordDocument
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
    DWORD               dwL1CRC;
    DWORD               dwL1CRCSize;
    DWORD               dwL2CRC;
    DWORD               dwL2CRCSize;

    ////////////////////////////////////////////////////////
    // Check magic number

    // Read the magic number

    if (SSSeekRead(lpstStream,
                   0,
                   &wTemp,
                   sizeof(WORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(WORD))
        return(FALSE);

    wTemp = WENDIAN(wTemp);

    // If it is not a known magic number, assume it is not
    //  a Word 6.0/95 document stream

    if (WD7IsKnownMagicNumber(wTemp) == FALSE)
        return(TRUE);

    ////////////////////////////////////////////////////////
    // Get encryption key

    // If the key could not be gotten, assume the stream
    //  is clean, since we wouldn't be able to scan it anyway

    if (WD7FindFirstKey(lpstStream,
                        &stKey,
                        &gstRevKeyLocker) != WD7_STATUS_OK)
        return(TRUE);

    // Get the TDT offset and size, if error, return FALSE,
    //  since we aren't able to CRC

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
        return(FALSE);

    dwTDTOffset = DWENDIAN(dwTDTOffset);
    dwTDTSize = DWENDIAN(dwTDTSize);

    // Find the location of the MCDs

    if (WD7GetMCDInfo(lpstStream,
                      &stKey,
                      dwTDTOffset,
                      dwTDTSize,
                      &dwMCDOffset,
                      &wMCDCount) == FALSE)
        return(FALSE);

    // If there were no MCDs, assume clean

    if (wMCDCount == 0)
        return(TRUE);

    // CRC the MCDs

    if (MSXL1WordDocumentCRC(lpstStream,
                             &stKey,
                             dwMCDOffset,
                             wMCDCount,
                             lpstMSX->lpabyWorkBuf,
                             lpstMSX->nWorkBufSize,
                             &dwL1CRC,
                             &dwL1CRCSize) == FALSE)
        return(FALSE);

    // CRC the macro bodies

    if (MSXL2WordDocumentCRC(lpstStream,
                             &stKey,
                             dwMCDOffset,
                             wMCDCount,
                             lpstMSX->lpabyWorkBuf,
                             lpstMSX->nWorkBufSize,
                             &dwL2CRC,
                             &dwL2CRCSize) == FALSE)
        return(FALSE);

    // Add to the database

    if (MSXDBAdd(lpstMSX,
                 dwL1CRC,
                 dwL1CRCSize,
                 dwL2CRC,
                 dwL2CRCSize) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXAddBook()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstStream          Ptr to Book stream
//
// Description:
//  The function does the following:
//  1. Tries to obtain the encryption key.
//  2. If the encryption key could not be obtained,
//      return TRUE.
//  3. Bulk scan, looking for the BOUNDSHEET records
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXAddBook
(
    LPMSX               lpstMSX,
    LPSS_STREAM         lpstStream
)
{
    XL5ENCKEY_T         stKey;
    DWORD               dwOffset;
    DWORD               dwL1CRC;
    DWORD               dwL1CRCSize;
    DWORD               dwL2CRC;
    DWORD               dwL2CRCSize;

    // Get the encryption key first

    if (XL5FindKey(lpstStream,
                   &stKey) == FALSE)
        return(TRUE);

    // Find the first BOUNDSHEET record

    if (MSXXLFindFirstBoundsheet(lpstStream,
                                 lpstMSX->lpabyWorkBuf,
                                 lpstMSX->nWorkBufSize,
                                 &dwOffset) == FALSE)
        return(FALSE);

    // At this point, the record should be a boundsheet

    if (MSXXL4SheetNameSetCRC(lpstStream,
                              &stKey,
                              lpstMSX->lpabyWorkBuf,
                              lpstMSX->nWorkBufSize,
                              dwOffset,
                              &dwL1CRC,
                              &dwL1CRCSize) == FALSE)
        return(FALSE);

    // If the CRC size is zero, then there are no 4.0 macro sheets

    if (dwL1CRCSize == 0)
        return(TRUE);

    // Iterate through the boundsheet records

    if (MSXXL4SheetSetCRC(lpstStream,
                          &stKey,
                          lpstMSX->lpabyWorkBuf,
                          lpstMSX->nWorkBufSize,
                          dwOffset,
                          &dwL2CRC,
                          &dwL2CRCSize) == FALSE)
        return(FALSE);

    // Add to the database

    if (MSXDBAdd(lpstMSX,
                 dwL1CRC,
                 dwL1CRCSize,
                 dwL2CRC,
                 dwL2CRCSize) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXAddWorkbook()
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
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXAddWorkbook
(
    LPMSX               lpstMSX,
    LPSS_STREAM         lpstStream
)
{
    BOOL                bEncrypted;
    DWORD               dwOffset;
    DWORD               dwL1CRC;
    DWORD               dwL1CRCSize;
    DWORD               dwL2CRC;
    DWORD               dwL2CRCSize;

    // Determine whether the stream is encrypted
    //  If error, return error

    if (XL97WorkbookIsEncrypted(lpstStream,
                                &bEncrypted) == FALSE)
        return(FALSE);

    // If it is encrypted, return clean

    if (bEncrypted != FALSE)
        return(TRUE);

    // Find the first BOUNDSHEET record

    if (MSXXLFindFirstBoundsheet(lpstStream,
                                 lpstMSX->lpabyWorkBuf,
                                 lpstMSX->nWorkBufSize,
                                 &dwOffset) == FALSE)
        return(FALSE);

    // At this point, the record should be a boundsheet

    if (MSXXL4SheetNameSetCRC(lpstStream,
                              NULL,
                              lpstMSX->lpabyWorkBuf,
                              lpstMSX->nWorkBufSize,
                              dwOffset,
                              &dwL1CRC,
                              &dwL1CRCSize) == FALSE)
        return(FALSE);

    // If the CRC size is zero, then there are no 4.0 macro sheets

    if (dwL1CRCSize == 0)
        return(TRUE);

    // Iterate through the boundsheet records

    if (MSXXL4SheetSetCRC(lpstStream,
                          NULL,
                          lpstMSX->lpabyWorkBuf,
                          lpstMSX->nWorkBufSize,
                          dwOffset,
                          &dwL2CRC,
                          &dwL2CRCSize) == FALSE)
        return(FALSE);

    // Add to the database

    if (MSXDBAdd(lpstMSX,
                 dwL1CRC,
                 dwL1CRCSize,
                 dwL2CRC,
                 dwL2CRCSize) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXAdd_VBA_PROJECT()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure
//  dwStorageID         ID of _VBA_PROJECT storage
//  dwChildID           ID of child of storage
//  lpstSibs            Ptr to sibling enumeration structure to use
//
// Description:
//  The function does the following:
//  1. Computes the CRC on the dir stream
//  2. Computes the CRC on the module streams
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXAdd_VBA_PROJECT
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwStorageID,
    DWORD               dwChildID,
    LPSS_ENUM_SIBS      lpstSibs
)
{
    DWORD               dwL1CRC;
    DWORD               dwL1CRCSize;
    DWORD               dwL2CRC;
    DWORD               dwL2CRCSize;

    // Find the dir stream

    SSInitEnumSibsStruct(lpstSibs,dwChildID);
    if (SSEnumSiblingEntriesCB(lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywszdir,
                               lpstSibs,
                               lpstStream) != SS_STATUS_OK)
        return(FALSE);

    // Compute the CRC on the dir stream

    CRC32Init(dwL1CRC);
    dwL1CRCSize = 0;
    if (MSXCRCStream(lpstStream,
                     lpstMSX->lpabyWorkBuf,
                     lpstMSX->nWorkBufSize,
                     0,
                     SSStreamLen(lpstStream),
                     &dwL1CRC,
                     &dwL1CRCSize) == FALSE)
        return(FALSE);

    // Compute the CRC on the streams

    if (MSXL2_VBA_PROJECTCRC(lpstRoot,
                             lpstStream,
                             dwChildID,
                             lpstSibs,
                             lpstMSX->lpabyWorkBuf,
                             lpstMSX->nWorkBufSize,
                             &dwL2CRC,
                             &dwL2CRCSize) == FALSE)
        return(FALSE);

    // Add to the database

    if (MSXDBAdd(lpstMSX,
                 dwL1CRC,
                 dwL1CRCSize,
                 dwL2CRC,
                 dwL2CRCSize) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSXAddVBA()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure
//  dwStorageID         ID of VBA storage
//  dwChildID           ID of child of storage
//  lpstSibs            Ptr to sibling enumeration structure to use
//
// Description:
//  The function does the following:
//  1. Computes the CRC on the dir stream
//  2. Computes the CRC on the _VBA_PROJECT stream
//  3. Computes the CRC on the module streams
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXAddVBA
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwStorageID,
    DWORD               dwChildID,
    LPSS_ENUM_SIBS      lpstSibs
)
{
    DWORD               dwL1dirCRC;
    DWORD               dwL1dirCRCSize;
    DWORD               dwL1_VBA_PROJECTCRC;
    DWORD               dwL1_VBA_PROJECTCRCSize;
    DWORD               dwL2CRC;
    DWORD               dwL2CRCSize;

    // Find the dir stream

    SSInitEnumSibsStruct(lpstSibs,dwChildID);
    if (SSEnumSiblingEntriesCB(lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywszdir,
                               lpstSibs,
                               lpstStream) != SS_STATUS_OK)
        return(FALSE);

    // Compute the CRC on the dir stream

    CRC32Init(dwL1dirCRC);
    dwL1dirCRCSize = 0;
    if (MSXCRCStream(lpstStream,
                     lpstMSX->lpabyWorkBuf,
                     lpstMSX->nWorkBufSize,
                     0,
                     SSStreamLen(lpstStream),
                     &dwL1dirCRC,
                     &dwL1dirCRCSize) == FALSE)
        return(FALSE);

    // Find the _VBA_PROJECT stream

    SSInitEnumSibsStruct(lpstSibs,dwChildID);
    if (SSEnumSiblingEntriesCB(lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywsz_VBA_PROJECT,
                               lpstSibs,
                               lpstStream) != SS_STATUS_OK)
        return(FALSE);

    // Compute the CRC on the _VBA_PROJECT stream

    CRC32Init(dwL1_VBA_PROJECTCRC);
    dwL1_VBA_PROJECTCRCSize = 0;
    if (MSXCRCStream(lpstStream,
                     lpstMSX->lpabyWorkBuf,
                     lpstMSX->nWorkBufSize,
                     0,
                     SSStreamLen(lpstStream),
                     &dwL1_VBA_PROJECTCRC,
                     &dwL1_VBA_PROJECTCRCSize) == FALSE)
        return(FALSE);

    // Compute the CRC on the streams

    if (MSXL2VBACRC(lpstRoot,
                    lpstStream,
                    dwChildID,
                    lpstSibs,
                    lpstMSX->lpabyWorkBuf,
                    lpstMSX->nWorkBufSize,
                    &dwL2CRC,
                    &dwL2CRCSize) == FALSE)
        return(FALSE);

    // Add dir CRCs to the database

    if (MSXDBAdd(lpstMSX,
                 dwL1dirCRC,
                 dwL1dirCRCSize,
                 dwL2CRC,
                 dwL2CRCSize) == FALSE)
        return(FALSE);

    // Add _VBA_PROJECT CRCs to the database

    if (MSXDBAdd(lpstMSX,
                 dwL1_VBA_PROJECTCRC,
                 dwL1_VBA_PROJECTCRCSize,
                 dwL2CRC,
                 dwL2CRCSize) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int MSXAddCRCs()
//
// Parameters:
//  lpstMSX             Ptr to MSX_T structure
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure to use
//
// Description:
//  The function looks for streams with the following names:
//      1. WordDocument
//      2. Book
//      3. Workbook
//
//  The function looks for storages with the following names:
//
//      1. _VBA_PROJECT
//      2. VBA
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSXAddCRCs
(
    LPMSX               lpstMSX,
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream
)
{
    int                 i;
    DWORD               dwDirID;
    MSXADD_CRC_INFO_T   stInfo;
    BOOL                bResult;
    LPSS_ENUM_SIBS      lpstSibs;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(FALSE);

    bResult = TRUE;
    dwDirID = 0;
    for (i=0;i<16384;i++)
    {
        if (SSEnumDirEntriesCB(lpstRoot,
                               MSXAddCRCCB,
                               &stInfo,
                               &dwDirID,
                               lpstStream) != SS_STATUS_OK)
            break;

        switch (stInfo.nType)
        {
            case MSX_L1_WORDDOCUMENT:
                bResult = MSXAddWordDocument(lpstMSX,lpstStream);
                break;

            case MSX_L1_BOOK:
                bResult = MSXAddBook(lpstMSX,lpstStream);
                break;

            case MSX_L1_WORKBOOK:
                bResult = MSXAddWorkbook(lpstMSX,lpstStream);
                break;

            case MSX_L2__VBA_PROJECT:
                bResult = MSXAdd_VBA_PROJECT(lpstMSX,
                                             lpstRoot,
                                             lpstStream,
                                             stInfo.dwID,
                                             stInfo.dwChildID,
                                             lpstSibs);
                break;

            case MSX_L2_VBA:
                bResult = MSXAddVBA(lpstMSX,
                                    lpstRoot,
                                    lpstStream,
                                    stInfo.dwID,
                                    stInfo.dwChildID,
                                    lpstSibs);
                break;

            default:
                break;
        }

        // Return immediately if the result is FALSE.

        if (bResult == FALSE)
            break;
    }

    // Successfully added all CRCs to the database

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
    return(bResult);
}   


