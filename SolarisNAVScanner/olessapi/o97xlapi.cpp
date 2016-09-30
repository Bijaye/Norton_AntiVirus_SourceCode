//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/O97XLAPI.CPv   1.14   09 Nov 1998 13:42:30   DCHI  $
//
// Description:
//  Core Office 97 Excel access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/O97XLAPI.CPv  $
// 
//    Rev 1.14   09 Nov 1998 13:42:30   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.13   11 Aug 1998 15:34:38   DCHI
// Return TRUE from O97XLWorkbookRemoveVBP() even if VBP record not found.
// 
//    Rev 1.12   02 Jul 1998 12:33:06   DCHI
// Added implementation of XL97FindBoundSheet() and modified
// XL97RemoveGlobalNames() to use it.
// 
//    Rev 1.11   29 Jun 1998 11:04:20   DCHI
// Added implementations for XL97WorkbookIsEncrypted()
// and XL97EntryHasEncryptedWBSib().
// 
//    Rev 1.10   04 May 1998 11:54:16   DCHI
// Added XL97WriteBlankSheet().
// 
//    Rev 1.9   01 May 1998 19:33:50   DCHI
// Corrected AssignRndSheetName() for Unicode names.
// 
//    Rev 1.8   02 Apr 1998 11:21:26   DCHI
// Changed O97XLWorkbookVBtoVHWS() to use random name.
// 
//    Rev 1.7   02 Apr 1998 10:44:02   DCHI
// Added O97XLWorkbookVBtoVHWS() implementation.
// 
//    Rev 1.6   17 Mar 1998 13:33:40   DCHI
// Made lpdwCRCByteCount parameter optional in XL97CRC32Expr().
// 
//    Rev 1.5   12 Mar 1998 10:42:44   DCHI
// Various additional functionality for new Excel engine.
// 
//    Rev 1.4   11 Feb 1998 16:20:06   DCHI
// Added XL97CRC32Expr().
// 
//    Rev 1.3   26 Sep 1997 12:32:18   DCHI
// Added O97XLWorkbookVeryHideVBSheets().
// 
//    Rev 1.2   17 Jul 1997 15:12:48   DCHI
// Removed buggy call to SSFreeEnumSibsStruct() in middle of
// O97XLCheckMacrosDeletable().
// 
//    Rev 1.1   08 May 1997 12:57:56   DCHI
// Added VCS headers.
// 
//************************************************************************

#include "o97api.h"
#include "olestrnm.h"
#include "xlrec.h"
#include "crc32.h"
#include "xl5api.h"

//********************************************************************
//
// Function:
//  int O97XLFindAssociatedStreamsCB()
//
// Description:
//  Searches for the Workbook stream and the storage
//  _VBA_PROJECT_CUR and sets the appropriate entry field in the
//  O97XL_FIND_STREAMS_T structure if found.
//
// Returns:
//  OLE_OPEN_CB_STATUS_CONTINUE     Always
//
//********************************************************************

typedef struct tagO97XL_FIND_STREAMS
{
    DWORD       dwWorkbookEntry;
    DWORD       dw_VBA_PROJECT_CUREntry;
} O97XL_FIND_STREAMS_T, FAR *LPO97XL_FIND_STREAMS;

int O97XLFindAssociatedStreamsCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it the Workbook stream?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWorkbook,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97XL_FIND_STREAMS)lpvCookie)->dwWorkbookEntry =
                dwIndex;
        }
    }
    else
    if (lpstEntry->byMSE == STGTY_STORAGE)
    {
        // Is it the <_VBA_PROJECT_CUR> storage?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywsz_VBA_PROJECT_CUR,
                       SS_MAX_NAME_LEN) == 0)
        {
            ((LPO97XL_FIND_STREAMS)lpvCookie)->dw_VBA_PROJECT_CUREntry =
                dwIndex;
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97XLFindAssociatedStreams()
//
// Description:
//  The following fields of lpstO97Scan should be valid before
//  entering this function:
//
//          dw_VBA_PROJECT_CUREntry
//          dw_VBA_PROJECT_CURChildEntry
//          dwVBAEntry
//          dwVBAChildEntry
//
//  The function will fill in the following fields:
//
//          dwParentStorageEntry
//          dwWorkbookEntry
//          dwPROJECTEntry
//          dwPROJECTwmEntry
//          dw_VBA_PROJECTEntry
//          dwdirEntry
//
//  The following algorithm is used:
//      1. Search for the Workbook stream:
//          For each substorage do the following:
//              Iterate through each sibling:
//                  Take note of the Workbook stream index
//                  Take note of the _VBA_PROJECT_CUR substorage index
//              If both are present and the index of the _VBA_PROJECT_CUR
//              substorage is the same as the one passed in, then we
//              are done.
//          If the associated Wordbook stream could not be found,
//              return failure.
//      2. Search for the PROJECT, PROJECTwm, _VBA_PROJECT, and
//          dir streams by calling the shared function to do so.
//
// Returns:
//  TRUE        If all of the associated streams were found
//  FALSE       If any of the streams could not be found
//
//********************************************************************

BOOL O97XLFindAssociatedStreams
(
    LPSS_ROOT           lpstRoot,
    LPO97_SCAN          lpstO97Scan
)
{
    BOOL                    bFound;
    O97XL_FIND_STREAMS_T    stFindStreams;
    DWORD                   dwStorageChildID;
    DWORD                   dwSearchID;
    LPSS_ENUM_SIBS          lpstSibs;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Find Word application specific streams
    /////////////////////////////////////////////////////////////

    // Go through all storages

    bFound = FALSE;
    dwSearchID = 0;
    while (SSEnumDirEntriesCB(lpstRoot,
                              O97EnumStorageEntriesCB,
                              &dwStorageChildID,
                              &dwSearchID,
                              NULL) == SS_STATUS_OK)
    {
        // Found a storage, so go through children looking
        //  for what we need

        SSInitEnumSibsStruct(lpstSibs,
                             dwStorageChildID);

        stFindStreams.dwWorkbookEntry = 0xFFFFFFFF;
        stFindStreams.dw_VBA_PROJECT_CUREntry = 0xFFFFFFFF;
        SSEnumSiblingEntriesCB(lpstRoot,
                               O97XLFindAssociatedStreamsCB,
                               &stFindStreams,
                               lpstSibs,
                               NULL);

        if (stFindStreams.dw_VBA_PROJECT_CUREntry ==
                lpstO97Scan->u.stXL97.dw_VBA_PROJECT_CUREntry &&
            stFindStreams.dwWorkbookEntry != 0xFFFFFFFF)
        {
            // Also store the parent storage's entry number

            lpstO97Scan->u.stXL97.dwParentStorageEntry =
                dwSearchID - 1;

            bFound = TRUE;
            break;
        }
    }

    if (bFound == FALSE)
    {
        // Failed finding associated Word application streams

        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(FALSE);
    }

    // Set the found entries in the VBA 5 scan structure

    lpstO97Scan->u.stXL97.dwWorkbookEntry =
        stFindStreams.dwWorkbookEntry;


    /////////////////////////////////////////////////////////////
    // Find VBA 5 specific streams
    /////////////////////////////////////////////////////////////

    SSInitEnumSibsStruct(lpstSibs,
                         lpstO97Scan->u.stXL97.
                             dw_VBA_PROJECT_CURChildEntry);

    if (O97FindVBAStreams(lpstRoot,
                          lpstSibs,
                          lpstO97Scan) == FALSE)
    {
        // Failed finding associated VBA streams

        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(FALSE);
    }

    // Successfully got the indices of all important streams

    SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  int O97XLCheckMacrosDeletableCB()
//
// Description:
//  The callback checks the name and does the following:
//      1. If the name is "", "_*", or "dir", the function just
//         returns CONTINUE.
//      2. If the name is not "ThisWorkbook" and not Sheet[0-9]+,
//         the function returns RETURN.
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN          If the entry is a candidate
//  SS_ENUM_CB_STATUS_CONTINUE      If the entry is not
//
//********************************************************************

int O97XLCheckMacrosDeletableCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    int                 i;

    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Is it any of "", "_*", or "dir"

        if (WENDIAN(lpstEntry->uszName[0]) == '_' ||
            lpstEntry->uszName[0] == 0 ||
            SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszdir,
                       SS_MAX_NAME_LEN) == 0)
        {
            return(SS_ENUM_CB_STATUS_CONTINUE);
        }

        // Is it "ThisWorkbook"?

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszThisWorkbook,
                       SS_MAX_NAME_LEN) == 0)
        {
            return(SS_ENUM_CB_STATUS_CONTINUE);
        }

        // Does it begin with something other than "Sheet"

        if (WENDIAN(lpstEntry->uszName[0]) != 'S' ||
            WENDIAN(lpstEntry->uszName[1]) != 'h' ||
            WENDIAN(lpstEntry->uszName[2]) != 'e' ||
            WENDIAN(lpstEntry->uszName[3]) != 'e' ||
            WENDIAN(lpstEntry->uszName[4]) != 't')
        {
            // The stream name does not begin with Sheet

            return(SS_ENUM_CB_STATUS_OPEN);
        }

        i = 5;
        do
        {
            WORD wChar;

            wChar = WENDIAN(lpstEntry->uszName[i++]);

            if (wChar < '0' || '9' < wChar)
            {
                // Not a number

                return(SS_ENUM_CB_STATUS_OPEN);
            }
        }
        while (i < SS_MAX_NAME_LEN && lpstEntry->uszName[i] != 0);
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  int O97XLOpenThisWorkbookSheetCB()
//
// Description:
//  Callback for opening "ThisWorkbook" stream and "Sheet" streams.
//
// Returns:
//  SS_ENUM_CB_STATUS_OPEN          If the entry is a candidate
//  SS_ENUM_CB_STATUS_CONTINUE      If the entry is not
//
//********************************************************************

int O97XLOpenThisWorkbookSheetCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;
    (void)lpvCookie;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszThisWorkbook,
                       SS_MAX_NAME_LEN) == 0)
        {
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Does it begin with "Sheet"

        if (WENDIAN(lpstEntry->uszName[0]) == 'S' &&
            WENDIAN(lpstEntry->uszName[1]) == 'h' &&
            WENDIAN(lpstEntry->uszName[2]) == 'e' &&
            WENDIAN(lpstEntry->uszName[3]) == 'e' &&
            WENDIAN(lpstEntry->uszName[4]) == 't')
        {
            // The stream name begins with Sheet

            return(SS_ENUM_CB_STATUS_OPEN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  BOOL O97XLCheckMacrosDeletable()
//
// Description:
//  Checks to see whether there are any modules left in the
//  VBA storage and zeroes out the _VBA_PROJECT_CUR storage name
//  if that is the case.
//
//  The function determines that there are no more modules
//  if all of the following conditions are met:
//      1. The names of streams in the VBA storage only
//         consist of the following:
//          - ""
//          - "_*"
//          - "dir"
//          - "ThisWorkbook"
//          - "Sheet#"
//          - None of the above and the stream length is zero
//      2. The "ThisWorkbook" and "Sheet#" stream consists only
//         of lines that begin with "Attribute ", case insensitive.
//
// Returns:
//  TRUE        If the ThisWorkbook and Sheet streams were
//              successfully analyzed as empty
//  FALSE       Otherwise, including error
//
//********************************************************************

BOOL O97XLCheckMacrosDeletable
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPO97_SCAN          lpstO97Scan         // VBA5 project stream info
)
{
    LPSS_ENUM_SIBS      lpstSibs;

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        return(FALSE);
    }

    // Determine whether there is any other module stream
    //  besides "ThisWorkbook" or "Sheet#"

    SSInitEnumSibsStruct(lpstSibs,
                         lpstO97Scan->dwVBAChildEntry);

    if (SSEnumSiblingEntriesCB(lpstRoot,
                               O97XLCheckMacrosDeletableCB,
                               NULL,
                               lpstSibs,
                               lpstStream) == SS_STATUS_OK)
    {
        // There was a module stream other than "ThisWorkbook" or
        //  "Sheet#"

        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
        return(FALSE);
    }

    // Open the ThisWorkbook and Sheet streams

    SSInitEnumSibsStruct(lpstSibs,
                         lpstO97Scan->dwVBAChildEntry);
    while (1)
    {
        if (SSEnumSiblingEntriesCB(lpstRoot,
                                   O97XLOpenThisWorkbookSheetCB,
                                   NULL,
                                   lpstSibs,
                                   lpstStream) != SS_STATUS_OK)
        {
            // No more streams to check?

            SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
            return(TRUE);
        }

        // Determine whether the stream consists of only
        //  Attribute lines

        if (VBA5LZNTModuleInit(lpstStream,
                               lpstLZNT) == FALSE)
        {
            // Failed to initialize for reading source text string

            SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
            return(FALSE);
        }

        if (O97LZNTStreamAttributeOnly(lpstLZNT) == FALSE)
        {
            // The stream contained something other than
            //  Attribute lines

            SSFreeEnumSibsStruct(lpstRoot,lpstSibs);
            return(FALSE);
        }
    }
}


//********************************************************************
//
// Function:
//  BOOL O97XLWorkbookRemoveVBP()
//
// Description:
//  Opens the Workbook stream at the given entry, parses through
//  it and replaces the OBPROJ: Visual Basic Project (record 0xD3)
//  with record 0xFFFF.
//
//  If there is no VBP record, the function still returns success.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97XLWorkbookRemoveVBP
(
    LPSS_STREAM         lpstStream,
    DWORD               dwWorkbookEntryNum
)
{
    DWORD               dwOffset;
    WORD                wRecType;
    WORD                wRecLen;
    WORD                w;
    BYTE                byTemp;
    DWORD               dwByteCount;

    // Open the Workbook stream

    if (SSOpenStreamAtIndex(lpstStream,
                            dwWorkbookEntryNum) != SS_STATUS_OK)
    {
        // Failed opening Workbook stream

        return(FALSE);
    }

    // Parse through the records

    dwOffset = 0;
    while (dwOffset < SSStreamLen(lpstStream))
    {
        // Read the record type and record length

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       (LPBYTE)&wRecType,
                       sizeof(WORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD) ||
            SSSeekRead(lpstStream,
                       dwOffset+sizeof(WORD),
                       (LPBYTE)&wRecLen,
                       sizeof(WORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD))
        {
            // Failed to read record type and length

            return(FALSE);
        }

        // Endianize

        wRecType = WENDIAN(wRecType);
        wRecLen = WENDIAN(wRecLen);

        if (wRecType == 0xD3)
        {
            // This is the one, convert it to 0x00CD

            wRecType = 0x00CD;

            wRecType = WENDIAN(wRecType);

            if (SSSeekWrite(lpstStream,
                            dwOffset,
                            (LPBYTE)&wRecType,
                            sizeof(WORD),
                            &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(WORD))
            {
                // Failed to rewrite record type as 0xFFFF

                return(FALSE);
            }

            // Zero out the rest of the record

            byTemp = 0;
            for (w=0;w<wRecLen;w++)
            {
                if (SSSeekWrite(lpstStream,
                                dwOffset + sizeof(WORD) + sizeof(WORD) + w,
                                &byTemp,
                                sizeof(BYTE),
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(BYTE))
                {
                    return(FALSE);
                }
            }

            // Success

            return(TRUE);
        }

        // Update offset

        dwOffset += sizeof(WORD) + sizeof(WORD) + wRecLen;
    }

    // No Visual Basic Project record?!

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97XLDeleteModule()
//
// Description:
//  Deletes a module from a Excel 97 document using the following
//  steps:
//
//      1. Delete the entry from the VBA streams
//      2. Open the WordDocument stream to get the template
//         data table location and size.
//      3. Open the 1Table stream to do the following:
//          a. Parse through the template data table to obtain
//             the location, size, and number of subrecords
//             of record types 0x01, 0x10, and 0x11.
//          b. Iterate through the subrecords of record type 0x01
//             to do the following:
//              i. Get the subrecord number for record type 0x10
//                 and 0x11 containing the information.
//              ii. If both records contain PROJECT.MODULENAME.,
//                  then delete all three subrecords.
//              iii. Update subrecord type 0x10 pointers of record
//                   type 0x01 to account for deleted subrecord 0x10.
//              iv. Update record size and number of subrecords.
//          c. If subrecords were eliminated, fill in the gaps
//             between the records.
//      4. If subrecords were eliminated, update the template data
//         table size field in the WordDocument stream.
//
// Returns:
//  TRUE        If the delete was successful
//  FALSE       If the delete was unsuccessful
//
//********************************************************************

BOOL O97XLDeleteModule
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPO97_SCAN          lpstO97Scan,        // VBA5 project stream info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 1K bytes
)
{
    if (O97DeleteModule(lpstRoot,
                        lpstStream,
                        lpstLZNT,
                        abyModuleName,
                        lpstO97Scan,
                        lpbyWorkBuffer) == FALSE)
    {
        // Failed to delete entry from VBA project streams

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97XLWorkbookVeryHideVBSheets()
//
// Description:
//  Opens the Workbook stream at the given entry, parses through
//  it and sets the very hidden bit of the option flags field
//  of every BOUNDSHEET (record 0x85) that has a sheet type
//  of Visual Basic module as specified in the option flags field.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97XLWorkbookVeryHideVBSheets
(
    LPSS_STREAM         lpstStream,
    DWORD               dwWorkbookEntryNum
)
{
    DWORD               dwOffset;
    WORD                wRecType;
    WORD                wRecLen;
    DWORD               dwByteCount;

    // Open the Workbook stream

    if (SSOpenStreamAtIndex(lpstStream,
                            dwWorkbookEntryNum) != SS_STATUS_OK)
    {
        // Failed opening Workbook stream

        return(FALSE);
    }

    // Parse through the records

    dwOffset = 0;
    while (dwOffset < SSStreamLen(lpstStream))
    {
        // Read the record type and record length

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       (LPBYTE)&wRecType,
                       sizeof(WORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD) ||
            SSSeekRead(lpstStream,
                       dwOffset+sizeof(WORD),
                       (LPBYTE)&wRecLen,
                       sizeof(WORD),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(WORD))
        {
            // Failed to read record type and length

            return(FALSE);
        }

        // Endianize

        wRecType = WENDIAN(wRecType);
        wRecLen = WENDIAN(wRecLen);

        if (wRecType == 0x85)
        {
            BYTE        abyOptionFlags[2];

            // This is a BOUNDSHEET record

            // Skip over the record type, record length, and offset
            //  of sheet BOF record

            dwOffset += sizeof(WORD) + sizeof(WORD);

            // Read the option flags

            if (SSSeekRead(lpstStream,
                           dwOffset + sizeof(DWORD),
                           abyOptionFlags,
                           2,
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != 2)
            {
                // Failed to read option flags

                return(FALSE);
            }

            // Is it a Visual Basic module sheet?

            if ((abyOptionFlags[1] & 0x0F) == 0x06)
            {
                // It is a VB module sheet, so set the very hidden bit

                abyOptionFlags[0] &= ~(XL_REC_BOUNDSHEET_STATE_MASK);
                abyOptionFlags[0] |= XL_REC_BOUNDSHEET_VERY_HIDDEN;

                // Write the new flags

                if (SSSeekWrite(lpstStream,
                                dwOffset + sizeof(DWORD),
                                abyOptionFlags,
                                2,
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != 2)
                {
                    // Failed to write new option flags

                    return(FALSE);
                }
            }

            dwOffset += wRecLen;
        }
        else
        {
            // Update offset

            dwOffset += sizeof(WORD) + sizeof(WORD) + wRecLen;
        }
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  DWORD XL97CRC32Expr()
//
// Parameters:
//  lpabyExpr           Ptr to the expression buffer
//  dwExprSize          Number of bytes in the expression buffer
//  dwCRC               Starting CRC value
//  lpdwCRCByteCount    Ptr to DWORD containing beginning count
//
// Description:
//  Continues a CRC32 on an expression.  The expression does not
//  need to be a complete expression.
//
//  The number of bytes used to continue the CRC is added
//  to *lpdwCRCByteCount.  If this value is not desired, the parameter
//  may be passed in as NULL.
//
// Returns:
//  DWORD               The continued CRC value
//
//*************************************************************************

// Number of bytes, including the token byte

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXL97FormulaTokenSkip[] =
{
    1,  // Unknown
    5,  // eXLFM_EXP        0x01 // Array formula or shared formula
    5,  // eXLFM_TBL        0x02 // Data table
    1,  // eXLFM_ADD        0x03 // Addition
    1,  // eXLFM_SUB        0x04 // Subtraction
    1,  // eXLFM_MUL        0x05 // Multiplication
    1,  // eXLFM_DIV        0x06 // Division
    1,  // eXLFM_POWER      0x07 // Exponentiation
    1,  // eXLFM_CONCAT     0x08 // Concatenation
    1,  // eXLFM_LT         0x09 // Less than
    1,  // eXLFM_LE         0x0A // Less than or equal
    1,  // eXLFM_EQ         0x0B // Equal
    1,  // eXLFM_GE         0x0C // Greater than or equal
    1,  // eXLFM_GT         0x0D // Greater than
    1,  // eXLFM_NE         0x0E // Not equal
    1,  // eXLFM_ISECT      0x0F // Intersection
    1,  // eXLFM_UNION      0x10 // Union
    1,  // eXLFM_RANGE      0x11 // Range
    1,  // eXLFM_UPLUS      0x12 // Unary plus
    1,  // eXLFM_UMINUS     0x13 // Unary minus
    1,  // eXLFM_PERCENT    0x14 // Percent sign
    1,  // eXLFM_PAREN      0x15 // Parenthesis
    1,  // eXLFM_MISSARG    0x16 // Missing argument
    1,  // eXLFM_STR        0x17 // String constant
    2,  // eXLFM_EXTEND     0x18 // Extended ptg
    1,  // eXLFM_ATTR       0x19 // Special attribute
    1,  // eXLFM_SHEET      0x1A // Deleted
    1,  // eXLFM_ENDSHEET   0x1B // Deleted
    2,  // eXLFM_ERR        0x1C // Error value
    2,  // eXLFM_BOOL       0x1D // Boolean
    3,  // eXLFM_INT        0x1E // Integer
    9,  // eXLFM_NUM        0x1F // Number
};

// Number of bytes, including the token and extended token byte to skip

BYTE FAR gabyXL97FormulaExtTokenSkip[] =
{
    2 + 0,  // 0x00
    2 + 4,  // 0x01
    2 + 4,  // 0x02
    2 + 4,  // 0x03
    2 + 0,  // 0x04
    2 + 0,  // 0x05
    2 + 4,  // 0x06
    2 + 4,  // 0x07
    2 + 0,  // 0x08
    2 + 0,  // 0x09
    2 + 13, // 0x0A
    2 + 13, // 0x0B
    2 + 4,  // 0x0C
    2 + 4,  // 0x0D
    2 + 4,  // 0x0E
    2 + 4,  // 0x0F
    2 + 4,  // 0x10
    2 + 0,  // 0x11
    2 + 0,  // 0x12
    2 + 0,  // 0x13
    2 + 0,  // 0x14
    2 + 0,  // 0x15
    2 + 0,  // 0x16
    2 + 0,  // 0x17
    2 + 0,  // 0x18
    2 + 0,  // 0x19
    2 + 0,  // 0x1A
    2 + 0,  // 0x1B
    2 + 0,  // 0x1C
    2 + 4,  // 0x1D
    2 + 0,  // 0x1E
};

BYTE FAR gabyXL97FormulaOpTokenSkip[] =
{
    8,  // eXLFM_ARRAY          0x20 // Array constant
    3,  // eXLFM_FUNC           0x21 // Function, fixed number of arguments
    4,  // eXLFM_FUNCVAR        0x22 // Function, variable number of arguments
    7,  // eXLFM_NAME           0x23 // Name
    5,  // eXLFM_REF            0x24 // Cell reference
    9,  // eXLFM_AREA           0x25 // Area reference
    7,  // eXLFM_MEMAREA        0x26 // Constant reference subexpression
    7,  // eXLFM_MEMERR         0x27 // Erroneous constant reference subexpression
    7,  // eXLFM_MEMNOMEM       0x28 // Incomplete constant reference subexpression
    3,  // eXLFM_MEMFUNC        0x29 // Variable reference subexpression
    5,  // eXLFM_REFERR         0x2A // Deleted cell reference
    9,  // eXLFM_AREAERR        0x2B // Deleted area reference
    5,  // eXLFM_REFN           0x2C // Cell reference within a shared formula
    9,  // eXLFM_AREAN          0x2D // Area reference within a shared formula
    3,  // eXLFM_MEMAREAN       0x2E // Reference subexpression within a name
    3,  // eXLFM_MEMNOMEMN      0x2F // Incomplete reference subexpression within a name
    1,  // Unknown              0x30
    1,  // Unknown              0x31
    1,  // Unknown              0x32
    1,  // Unknown              0x33
    1,  // Unknown              0x34
    1,  // Unknown              0x35
    1,  // Unknown              0x36
    1,  // Unknown              0x37
    1,  // eXLFM_FUNCCE         0x38 // FuncCE
    7,  // eXLFM_NAMEX          0x39 // Name or external name
    7,  // eXLFM_REF3D          0x3A // 3-D cell reference
    11, // eXLFM_AREA3D         0x3B // 3-D area reference
    7,  // eXLFM_REFERR3D       0x3C // Deleted 3-D cell reference
    11, // eXLFM_AREAERR3D      0x3D // Deleted 3-D area reference
    1,  // Unknown              0x3E
    1,  // Unknown              0x3F
};

BYTE FAR gabyXL97FormulaOpTokenChange[] =
{
    0x00,  // eXLFM_ARRAY
    0x01,  // eXLFM_FUNC
    0x02,  // eXLFM_FUNCVAR
    0x03,  // eXLFM_NAME
    0x04,  // eXLFM_REF
    0x05,  // eXLFM_AREA
    0x06,  // eXLFM_MEMAREA
    0x06,  // eXLFM_MEMERR      -> eXLFM_MEMAREA
    0x06,  // eXLFM_MEMNOMEM    -> eXLFM_MEMAREA
    0x09,  // eXLFM_MEMFUNC
    0x04,  // eXLFM_REFERR      -> eXLFM_REF
    0x05,  // eXLFM_AREAERR     -> eXLFM_AREA
    0x0C,  // eXLFM_REFN
    0x0D,  // eXLFM_AREAN
    0x0E,  // eXLFM_MEMAREAN
    0x0E,  // eXLFM_MEMNOMEMN   -> eXLFM_MEMAREAN
    0x10,  // Unknown
    0x11,  // Unknown
    0x12,  // Unknown
    0x13,  // Unknown
    0x14,  // Unknown
    0x15,  // Unknown
    0x16,  // Unknown
    0x17,  // Unknown
    0x18,  // eXLFM_FUNCCE
    0x19,  // eXLFM_NAMEX
    0x1A,  // eXLFM_REF3D
    0x1B,  // eXLFM_AREA3D
    0x1A,  // eXLFM_REFERR3D    -> eXLFM_REF3D
    0x1B,  // eXLFM_AREAERR3D   -> eXLFM_AREA3D
    0x1E,  // Unknown
    0x1F,  // Unknown
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

DWORD XL97CRC32Expr
(
    LPBYTE              lpabyExpr,
    DWORD               dwExprSize,
    DWORD               dwCRC,
    LPDWORD             lpdwCRCByteCount
)
{
    BYTE                byToken;
    BYTE                byTemp;
    DWORD               dwTemp;
    DWORD               dwCRCByteCount;

    dwCRCByteCount = 0;
    while (dwExprSize != 0)
    {
        byToken = *lpabyExpr;
        if (byToken < 0x20)
        {
            if (byToken == eXLFM_STR)
            {
                BYTE    byLen;
                BYTE    byFlag;
                WORD    wNumRuns;
                DWORD   dwExtSize;

                // CRC the token byte

                CRC32Continue(dwCRC,byToken);
                ++dwCRCByteCount;
                ++lpabyExpr;
                if (--dwExprSize != 0)
                {
                    // Get the length byte and CRC it

                    byLen = *lpabyExpr++;
                    CRC32Continue(dwCRC,byLen);
                    ++dwCRCByteCount;

                    if (--dwExprSize != 0)
                    {
                        // Get the flag byte

                        byFlag = *lpabyExpr++;
                        --dwExprSize;

                        // Is it a rich string?

                        if ((byFlag & 0x08) != 0)
                        {
                            // It is a rich string, get the
                            //  number of formatting runs

                            if (dwExprSize >= 2)
                            {
                                wNumRuns = (WORD)lpabyExpr[0] |
                                    (((WORD)lpabyExpr[1] << 8) & 0xFF00);

                                lpabyExpr += 2;
                                dwExprSize -= 2;
                            }
                            else
                            {
                                wNumRuns = 0;
                                dwExprSize = 0;
                            }
                        }

                        // Does it contain an extended string?

                        if ((byFlag & 0x04) != 0)
                        {
                            // It contains an extended string.
                            //  Get the extended data size

                            if (dwExprSize >= 4)
                            {
                                dwExtSize =
                                    ((DWORD)lpabyExpr[0] & 0x000000FFUL) |
                                    (((DWORD)lpabyExpr[1] << 8) & 0x0000FF00UL) |
                                    (((DWORD)lpabyExpr[2] << 16) & 0x00FF0000UL) |
                                    (((DWORD)lpabyExpr[3] << 24) & 0xFF000000UL);

                                lpabyExpr += 4;
                                dwExprSize -= 4;
                            }
                            else
                            {
                                wNumRuns = 0;
                                dwExprSize = 0;
                            }
                        }

                        // Time to CRC string and skip over formatting
                        //  runs and extended string info

                        if (dwExprSize != 0)
                        {
                            // Is it a double-byte string?

                            if ((byFlag & 0x01) != 0)
                            {
                                // It is a double-byte string

                                dwTemp = 2 * (DWORD)byLen;
                                if (dwExprSize < dwTemp)
                                    dwTemp = dwExprSize;
                            }
                            else
                            {
                                // It is a single-byte string

                                if (dwExprSize < byLen)
                                    dwTemp = dwExprSize;
                                else
                                    dwTemp = byLen;
                            }

                            dwCRC = CRC32Compute(dwTemp,
                                                 lpabyExpr,
                                                 dwCRC);

                            dwCRCByteCount += dwTemp;
                            dwExprSize -= dwTemp;
                            lpabyExpr += dwTemp;

                            // Skip over formatting runs

                            dwTemp = 4 * wNumRuns;
                            if (dwExprSize < dwTemp)
                                dwExprSize = 0;
                            else
                            {
                                lpabyExpr += dwTemp;
                                dwExprSize -= dwTemp;
                            }

                            // Skip over extended string

                            if (dwExprSize < dwExtSize)
                                dwExprSize = 0;
                            else
                            {
                                lpabyExpr += dwExtSize;
                                dwExprSize -= dwExtSize;
                            }
                        }
                    }
                }
            }
            else
            if (byToken == eXLFM_ATTR)
            {
                // Determine what the special attribute is

                ++lpabyExpr;
                if (--dwExprSize != 0)
                {
                    byTemp = *lpabyExpr++;
                    --dwExprSize;
                    if (byTemp == 0x04)
                    {
                        // bitFAttrChoose

                        // Just CRC the attribute byte

                        CRC32Continue(dwCRC,byTemp);
                        ++dwCRCByteCount;
                        if (dwExprSize != 0)
                        {
                            // CRC the first byte of wCase

                            byTemp = *lpabyExpr++;
                            CRC32Continue(dwCRC,byTemp);
                            ++dwCRCByteCount;
                            if (--dwExprSize != 0)
                            {
                                // CRC the second byte of wCase

                                dwTemp = byTemp;
                                byTemp = *lpabyExpr++;
                                CRC32Continue(dwCRC,byTemp);
                                ++dwCRCByteCount;
                                --dwExprSize;

                                // Form the number of cases

                                dwTemp |= (((DWORD)byTemp) << 8) & 0xFF00;

                                // Skip over the choose offsets

                                dwTemp *= sizeof(WORD);
                                if (dwExprSize < dwTemp)
                                    dwExprSize = 0;
                                else
                                {
                                    dwExprSize -= dwTemp;
                                    lpabyExpr += dwTemp;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (byTemp == 0x02 || byTemp == 0x10)
                        {
                            // bitFAttrIf
                            // bitFAttrSum

                            // Just CRC the attribute byte

                            CRC32Continue(dwCRC,byTemp);
                            ++dwCRCByteCount;
                        }

                        // Skip the other fields

                        if (dwExprSize <= 2)
                            dwExprSize = 0;
                        else
                        {
                            dwExprSize -= 2;
                            lpabyExpr += 2;
                        }
                    }
                }
            }
            else
            if (byToken == eXLFM_EXTEND)
            {
                // CRC the token byte

                CRC32Continue(dwCRC,byToken);
                ++dwCRCByteCount;

                // Skip past the token and the extended info

                if (dwExprSize >= 2)
                {
                    // Get the extended ptg

                    byTemp = lpabyExpr[1];
                    if (byTemp < 0x1F)
                        byTemp = gabyXL97FormulaExtTokenSkip[byTemp];
                    else
                        byTemp = 2;
                }
                else
                    byTemp = 2;

                if (dwExprSize < byTemp)
                    dwExprSize = 0;
                else
                {
                    lpabyExpr += byTemp;
                    dwExprSize -= byTemp;
                }
            }
            else
            {
                // Use the token byte skip table

                byTemp = gabyXL97FormulaTokenSkip[byToken];
                if (dwExprSize < byTemp)
                    byTemp = (BYTE)dwExprSize;

                dwCRC = CRC32Compute(byTemp,
                                     lpabyExpr,
                                     dwCRC);

                dwCRCByteCount += byTemp;
                dwExprSize -= byTemp;
                lpabyExpr += byTemp;
            }
        }
        else
        if (byToken < 0x80)
        {
            // Normalize

            byTemp = byToken & 0x1F;

            // Change the token to a non-error token

            byToken = (byToken & 0x60) | gabyXL97FormulaOpTokenChange[byTemp];

            // CRC the token

            CRC32Continue(dwCRC,byToken);
            ++dwCRCByteCount;

            if (byTemp == 0x01)
            {
                if (dwExprSize > 1)
                {
                    if (dwExprSize == 2)
                        dwTemp = 1;
                    else
                        dwTemp = 2;

                    dwCRC = CRC32Compute(dwTemp,
                                         lpabyExpr + 1,
                                         dwCRC);

                    dwCRCByteCount += dwTemp;
                }
            }
            else
            if (byTemp == 0x02)
            {
                if (dwExprSize > 1)
                {
                    if (dwExprSize == 2)
                        dwTemp = 1;
                    else
                    if (dwExprSize == 3)
                        dwTemp = 2;
                    else
                        dwTemp = 3;

                    dwCRC = CRC32Compute(dwTemp,
                                         lpabyExpr + 1,
                                         dwCRC);

                    dwCRCByteCount += dwTemp;
                }
            }

            if (dwExprSize < gabyXL97FormulaOpTokenSkip[byTemp])
                dwExprSize = 0;
            else
            {
                // Skip the token and its value

                dwExprSize -= gabyXL97FormulaOpTokenSkip[byTemp];
                lpabyExpr += gabyXL97FormulaOpTokenSkip[byTemp];
            }
        }
        else
        {
            // Skip it

            --dwExprSize;
            ++lpabyExpr;
        }
    }

    if (lpdwCRCByteCount != NULL)
        *lpdwCRCByteCount += dwCRCByteCount;

    return(dwCRC);
}


//*************************************************************************
//
// Function:
//  BOOL XL97UnicodeIsStrI()
//
// Parameters:
//  lpstStream              Ptr to stream containing unicode string
//  wCharCnt                Number of characters in unicode string
//  dwUnicodeStrOffset      Offset of unicode string in stream
//  lpbypsStr               Pascal string to compare against
//  lpbEqual                Ptr to BOOL for equality of strings
//
// Description:
//  The function performs a case insensitive comparison of the
//  unicode string and the single-byte Pascal string.  The unicode
//  string at the given offset is assumed to be a BIFF8 compressed
//  unicode string.  If the unicode characters are double-byte
//  characters, only the first byte of each character is used in
//  the comparison against the Pascal-string.
//
//  If wCharCnt is greater than 255, then the function returns error.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XL97UnicodeIsStrI
(
    LPSS_STREAM             lpstStream,
    WORD                    wCharCnt,
    DWORD                   dwUnicodeStrOffset,
    LPBYTE                  lpbypsStr,
    LPBOOL                  lpbEqual
)
{
    BYTE                    byFlags;
    DWORD                   dwByteCount;
    BYTE                    abyStr[256];
    WORD                    w;

    if (wCharCnt != lpbypsStr[0] || wCharCnt > 255)
    {
        // The strings are not equal

        *lpbEqual = FALSE;
        return(TRUE);
    }

    // Both strings have an equal number of characters,
    //  so read in the flag byte first

    if (SSSeekRead(lpstStream,
                   dwUnicodeStrOffset,
                   &byFlags,
                   sizeof(BYTE),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(BYTE))
        return(FALSE);

    // The default offset is right after the flag byte

    ++dwUnicodeStrOffset;

    // If the string has formatting runs, then there is a
    //  WORD after the flag byte describing the number of runs

    if ((byFlags & XL_USTR_FLAG_RICH) != 0)
        dwUnicodeStrOffset += 2;

    // If the string has extended character information, then there
    //  are four additional bytes before the actual string data

    if ((byFlags & XL_USTR_FLAG_EXTENDED) != 0)
        dwUnicodeStrOffset += 4;

    // Now determine whether it is a single-byte or a double-byte
    //  string

    if ((byFlags & XL_USTR_FLAG_DOUBLE_BYTE) == 0)
    {
        // Read the string as is

        if (SSSeekRead(lpstStream,
                       dwUnicodeStrOffset,
                       abyStr + 1,
                       wCharCnt,
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != wCharCnt)
            return(FALSE);
    }
    else
    {
        // Read the Unicode string one character at a time

        for (w=1;w<=wCharCnt;w++)
        {
            if (SSSeekRead(lpstStream,
                           dwUnicodeStrOffset,
                           abyStr + w,
                           sizeof(BYTE),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(BYTE))
                return(FALSE);

            dwUnicodeStrOffset += 2;
        }
    }

    // Compare the strings

    for (w=1;w<=wCharCnt;w++)
    {
        if (SSToLower(abyStr[w]) != SSToLower(lpbypsStr[w]))
        {
            *lpbEqual = FALSE;
            return(TRUE);
        }
    }

    // The strings must be equal

    *lpbEqual = TRUE;
    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL97FindBoundSheet()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpbypsName              Ptr to Pascal string of name
//  byType                  Type of worksheet
//  lpdwSheetIndex          Ptr to DWORD for index of BOUNDSHEET structure
//  lpdwBoundSheetOffset    Ptr to DWORD for offset of BOUNDSHEET structure
//
// Description:
//  The function searches the workbook global records for the
//  boundsheet structure with the given criteria.
//
//  If lpbypsName is not NULL, then lpbypsName should point to a Pascal
//  string containing the name of the sheet for which to search.
//  The string comparison is case-insensitive.
//
//  If byType is not 0xFF then it specifies the sheet type for which
//  to search and should be one of:
//
//      XL_REC_BOUNDSHEET_WORKSHEET
//      XL_REC_BOUNDSHEET_40_MACRO
//      XL_REC_BOUNDSHEET_CHART
//      XL_REC_BOUNDSHEET_VB_MODULE
//
//  If lpdwSheetIndex is not NULL, then it should point to a DWORD
//  specifying the minimum sheet index for which to return a
//  BOUNDSHEET structure.  The first BOUNDSHEET record in the workbook
//  global records has an index of zero.  If lpdwSheetIndex is NULL,
//  then the minimum sheet index is assumed to be zero.
//
//  Either of lpdwSheetIndex and lpdwBoundSheetOffset
//  may be NULL if the desired value is not needed.
//
//  The BOUNDSHEET structure is properly endianized before returning.
//
// Returns:
//  TRUE                    If the sheet was found
//  FALSE                   On error, or the sheet was not found
//
//*************************************************************************

BOOL XL97FindBoundSheet
(
    LPSS_STREAM             lpstStream,
    LPBYTE                  lpbypsName,
    BYTE                    bySheetType,
    LPDWORD                 lpdwSheetIndex,
    LPDWORD                 lpdwBoundSheetOffset
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    DWORD                   dwMinSheetIndex;
    DWORD                   dwSheetIndex;
    XL_REC_HDR_T            stRec;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    DWORD                   dwByteCount;
    BOOL                    bEqual;

    if (lpdwSheetIndex != NULL)
        dwMinSheetIndex = *lpdwSheetIndex;
    else
        dwMinSheetIndex = 0;

    dwSheetIndex = 0;

    // Find the index of the BOUNDSHEET record with the given name

    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    dwSheetIndex = 0;
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(FALSE);

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            if (dwSheetIndex >= dwMinSheetIndex)
            {
                // Read the BOUNDSHEET structure

                if (SSSeekRead(lpstStream,
                               dwOffset + sizeof(XL_REC_HDR_T),
                               &stBoundSheet,
                               sizeof(XL_REC_BOUNDSHEET_T),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(XL_REC_BOUNDSHEET_T))
                    return(FALSE);

                if (bySheetType == 0xFF ||
                    (stBoundSheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                    bySheetType)
                {
                    // Endianize the BOF offset

                    stBoundSheet.dwOffset = DWENDIAN(stBoundSheet.dwOffset);

                    // Now compare the name if non-NULL

                    if (lpbypsName == NULL)
                    {
                        // Return this one

                        if (lpdwSheetIndex != NULL)
                            *lpdwSheetIndex = dwSheetIndex;

                        if (lpdwBoundSheetOffset != NULL)
                            *lpdwBoundSheetOffset = dwOffset;

                        return(TRUE);
                    }

                    // Is this the one?

                    if (XL97UnicodeIsStrI(lpstStream,
                                          stBoundSheet.byNameLen,
                                          dwOffset + sizeof(XL_REC_HDR_T) +
                                              sizeof(XL_REC_BOUNDSHEET_T),
                                          lpbypsName,
                                          &bEqual) == FALSE)
                        return(FALSE);

                    if (bEqual != FALSE)
                    {
                        // Return this one

                        if (lpdwSheetIndex != NULL)
                            *lpdwSheetIndex = dwSheetIndex;

                        if (lpdwBoundSheetOffset != NULL)
                            *lpdwBoundSheetOffset = dwOffset;

                        return(TRUE);
                    }
                }
            }

            // Increment sheet index

            ++dwSheetIndex;
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    // No such sheet

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  BOOL XL97RemoveGlobalNames()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  lpbypsName              Name of sheet
//
// Description:
//  The function invalidates all global NAME records referring to the
//  sheet with the given name.  The comparison on the name is case
//  insensitive.
//
//  The function accomplishes this by setting all fields of the NAME
//  record to zero.  The ixals and itab fields are left as is.
//
//  If no BOUNDSHEET record with the given name is found, the function
//  returns error.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XL97RemoveGlobalNames
(
    LPSS_STREAM             lpstStream,
    LPBYTE                  lpbypsName
)
{
    DWORD                   dwSheetIndex;

    // Find the index of the BOUNDSHEET record with the given name

    if (XL97FindBoundSheet(lpstStream,
                           lpbypsName,
                           0xFF,
                           &dwSheetIndex,
                           NULL) == FALSE)
        return(FALSE);

    // Now erase the NAME records referring to the given sheet

    if (XLEraseNameRecords(lpstStream,
                           NULL,
                           (WORD)dwSheetIndex) == FALSE)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL97AssignRndSheetName()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  dwBoundSheetOffset      Offset of BOUNDSHEET record for sheet
//
// Description:
//  The function assigns the BOUNDSHEET record at the given offset
//  a new unique name.  The unique name is a serialized base 36
//  number using the digits '0'-'9' and 'a'-'z'.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XL97AssignRndSheetName
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwBoundSheetOffset
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stTargetRec;
    XL8_REC_BOUNDSHEET_T    stTargetBoundSheet;
    XL_REC_HDR_T            stRec;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    DWORD                   dwByteCount;
    DWORD                   dwFirstBoundSheetOffset;
    DWORD                   dwLastBoundSheetOffset;
    BYTE                    abyNewName[XL_REC_BOUNDSHEET_MAX_NAME_LEN+1];
    int                     i;
    DWORD                   dwExtraByteCount;
    BOOL                    bEqual;

    dwMaxOffset = SSStreamLen(lpstStream);

    /////////////////////////////////////////////////////////////
    // Find the first and last BOUNDSHEET record

    dwOffset = 0;
    dwFirstBoundSheetOffset = 0;
    dwLastBoundSheetOffset = 0;
    while (dwOffset < dwMaxOffset)
    {
        // Read the record header

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record

            return(FALSE);
        }

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            if (dwFirstBoundSheetOffset == 0)
                dwFirstBoundSheetOffset = dwOffset;

            dwLastBoundSheetOffset = dwOffset;
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    if (dwFirstBoundSheetOffset == 0)
        return(FALSE);


    /////////////////////////////////////////////////////////////
    // Read the target BOUNDSHEET record and record header

    if (SSSeekRead(lpstStream,
                   dwBoundSheetOffset,
                   &stTargetRec,
                   sizeof(XL_REC_HDR_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL_REC_HDR_T))
    {
        // Failed to read record

        return(FALSE);
    }

    stTargetRec.wType = WENDIAN(stTargetRec.wType);
    stTargetRec.wLen = WENDIAN(stTargetRec.wLen);

    // Read the BOUNDSHEET structure

    if (SSSeekRead(lpstStream,
                   dwBoundSheetOffset + sizeof(XL_REC_HDR_T),
                   (LPBYTE)&stTargetBoundSheet,
                   sizeof(XL8_REC_BOUNDSHEET_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL8_REC_BOUNDSHEET_T))
        return(FALSE);

    if (stTargetBoundSheet.byNameLen == 0)
        return(FALSE);


    /////////////////////////////////////////////////////////////
    // Generate a unique name

    // Start with all zeroes

    abyNewName[0] = stTargetBoundSheet.byNameLen;
    if (abyNewName[0] > XL_REC_BOUNDSHEET_MAX_NAME_LEN)
        abyNewName[0] = XL_REC_BOUNDSHEET_MAX_NAME_LEN;

    for (i=1;i<=abyNewName[0];i++)
        abyNewName[i] = '0';

    // Iterate until a unique name is generated

    while (1)
    {
        dwOffset = dwFirstBoundSheetOffset;
        while (dwOffset <= dwLastBoundSheetOffset)
        {
            // Read the record header

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           &stRec,
                           sizeof(XL_REC_HDR_T),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(XL_REC_HDR_T))
            {
                // Failed to read record

                return(FALSE);
            }

            stRec.wType = WENDIAN(stRec.wType);
            stRec.wLen = WENDIAN(stRec.wLen);

            if (stRec.wType == eXLREC_BOUNDSHEET)
            {
                if (SSSeekRead(lpstStream,
                               dwOffset + sizeof(XL_REC_HDR_T),
                               (LPBYTE)&stBoundSheet,
                               sizeof(XL_REC_BOUNDSHEET_T),
                               &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(XL_REC_BOUNDSHEET_T))
                    return(FALSE);

                if (XL97UnicodeIsStrI(lpstStream,
                                      stBoundSheet.byNameLen,
                                      dwOffset + sizeof(XL_REC_HDR_T) +
                                          sizeof(XL_REC_BOUNDSHEET_T),
                                      abyNewName,
                                      &bEqual) == FALSE)
                    return(FALSE);

                if (bEqual == TRUE)
                    break;
            }

            dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
        }

        if (dwOffset > dwLastBoundSheetOffset)
        {
            // Found a unique name

            break;
        }

        // Iterate the name

        for (i=1;i<=abyNewName[0];i++)
        {
            if ('0' <= abyNewName[i] && abyNewName[i] <= '9')
            {
                if (abyNewName[i] == '9')
                    abyNewName[i] = 'a';
                else
                    abyNewName[i]++;

                break;
            }

            if ('a' <= abyNewName[i] && abyNewName[i] <= 'z')
            {
                if (abyNewName[i] == 'z')
                    abyNewName[i] = '0';
                else
                {
                    abyNewName[i]++;
                    break;
                }
            }
        }

        if (i > abyNewName[0])
        {
            // Ran out of unique names

            return(FALSE);
        }
    }

    // Replace the boundsheet record

    stTargetBoundSheet.byNameLen = abyNewName[0];
    stTargetBoundSheet.byNameFlags = 0;

    dwOffset = dwBoundSheetOffset + sizeof(XL_REC_HDR_T);

    if (SSSeekWrite(lpstStream,
                    dwOffset,
                    (LPBYTE)&stTargetBoundSheet,
                    sizeof(XL8_REC_BOUNDSHEET_T),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL8_REC_BOUNDSHEET_T))
        return(FALSE);

    dwOffset += sizeof(XL8_REC_BOUNDSHEET_T);

    // Replace the sheet name

    if (SSSeekWrite(lpstStream,
                    dwBoundSheetOffset + sizeof(XL_REC_HDR_T) +
                        sizeof(XL8_REC_BOUNDSHEET_T),
                    abyNewName + 1,
                    abyNewName[0],
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != abyNewName[0])
        return(FALSE);

    dwOffset += abyNewName[0];

    // Zero out the rest of the record

    dwExtraByteCount = stTargetRec.wLen - sizeof(XL8_REC_BOUNDSHEET_T) -
        abyNewName[0];

    if (dwExtraByteCount != 0)
    {
        if (SSWriteZeroes(lpstStream,
                          dwOffset,
                          dwExtraByteCount) != SS_STATUS_OK)
            return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL97EraseSheet()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  dwOffset                Offset of BOUNDSHEET record for sheet
//  wSheetIndex             The index of the sheet
//
// Description:
//  The function performs the following operations in this order:
//      1. Read the BOUNDSHEET record to get the sheet's BOF record offset
//      2. Blanks out the sheet's cell values
//      3. Gives the sheet the appearance of a standard worksheet
//      4. Makes the sheet very hidden and turns it into a worksheet
//      5. Makes sure the window information is valid
//      6. Removes the VB project record if necessary
//      7. Removes global name references to the sheet
//      8. Replaces the sheet name with a new unique random name
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XL97EraseSheet
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwOffset,
    WORD                    wSheetIndex
)
{
    DWORD                   dwByteCount;
    XL_REC_HDR_T            stRec;
    XL8_REC_BOUNDSHEET_T    stBoundSheet;

    // Read the BOUNDSHEET record header

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &stRec,
                   sizeof(XL_REC_HDR_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL_REC_HDR_T))
    {
        // Failed to read record

        return(FALSE);
    }

    stRec.wType = WENDIAN(stRec.wType);
    stRec.wLen = WENDIAN(stRec.wLen);

    // Read the BOUNDSHEET record

    if (SSSeekRead(lpstStream,
                   dwOffset + sizeof(XL_REC_HDR_T),
                   &stBoundSheet,
                   sizeof(XL8_REC_BOUNDSHEET_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL8_REC_BOUNDSHEET_T))
    {
        // Failed to read record

        return(FALSE);
    }

    // Get the offset of the BOF record for the sheet

    stBoundSheet.dwOffset = DWENDIAN(stBoundSheet.dwOffset);

    // Blank out the contents of the sheet

    if (XLBlankAllCellValues(lpstStream,
                             NULL,
                             stBoundSheet.dwOffset) == FALSE)
        return(FALSE);

    // Give the sheet the appearance of a standard worksheet

    if (XLSetStandardSheetFlags(lpstStream,
                                NULL,
                                stBoundSheet.dwOffset) == FALSE)
        return(FALSE);

    // Make the sheet very hidden and turn it into a worksheet

    if (XLSetSheetStateType(lpstStream,
                            NULL,
                            dwOffset,
                            XL_REC_BOUNDSHEET_VERY_HIDDEN,
                            XL_REC_BOUNDSHEET_WORKSHEET) == FALSE)
        return(FALSE);

    // Make sure the window information is valid

    if (XLMakeWindowInfoValid(lpstStream,
                              NULL) == FALSE)
        return(FALSE);

    // Remove the VB project record if necessary

    if (XLRemoveUnusedVBProj(lpstStream,
                             NULL) == FALSE)
        return(FALSE);

    // Remove global name references

    if (XLEraseNameRecords(lpstStream,
                           NULL,
                           wSheetIndex) == FALSE)
        return(FALSE);

    // Give the sheet a new name

    if (XL97AssignRndSheetName(lpstStream,
                               dwOffset) == FALSE)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL97ReadUnicodeStr()
//
// Parameters:
//  lpstStream              Ptr to Book stream
//  dwOffset                Offset of string option flag byte
//  bReadSingleByte         Set to TRUE if to be read as single-byte
//  lpbyDst                 Destination buffer
//  wSize                   Number of characters
//  lpbDoubleByte           Ptr to BOOL for double-byte status
//
// Description:
//  dwOffset should be the offset in the stream of a BIFF8 Unicode
//  string.  If bReadSingleByte is TRUE, only the first byte of
//  double-byte characters is read into the buffer.
//
//  If lpbDoubleByte is not NULL, then the function sets it to TRUE
//  if the string consists of double-byte characters.  It is set
//  to FALSE if all characters are single-byte characters.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL XL97ReadUnicodeStr
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwOffset,
    BOOL                    bReadSingleByte,
    LPBYTE                  lpbyDst,
    WORD                    wSize,
    LPBOOL                  lpbDoubleByte
)
{
    BYTE                    byFlags;
    DWORD                   dwByteCount;
    WORD                    w;
    size_t                  sChar;

    // Read the flag byte

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &byFlags,
                   sizeof(BYTE),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(BYTE))
        return(FALSE);

    // Skip over flag byte

    ++dwOffset;

    if (byFlags & XL_USTR_FLAG_EXTENDED)
    {
        // Skip over extended string size

        dwOffset += 4;
    }

    if (byFlags & XL_USTR_FLAG_RICH)
    {
        // Skip over formatting run count

        dwOffset += 2;
    }

    if (byFlags & XL_USTR_FLAG_DOUBLE_BYTE)
    {
        // Double-byte

        if (lpbDoubleByte != NULL)
            *lpbDoubleByte = TRUE;

        if (bReadSingleByte == FALSE)
            sChar = sizeof(WORD);
        else
            sChar = sizeof(BYTE);

        for (w=0;w<wSize;w+=2)
        {
            if (SSSeekRead(lpstStream,
                           dwOffset,
                           lpbyDst,
                           sChar,
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sChar)
                return(FALSE);

            lpbyDst += sChar;
        }
    }
    else
    {
        // Single-byte

        if (lpbDoubleByte != NULL)
            *lpbDoubleByte = FALSE;

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       lpbyDst,
                       wSize,
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != wSize)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97XLWorkbookVBtoVHWS()
//
// Parameters:
//  lpstStream          Ptr to stream structure
//  dwWorkbookEntryNum  Workbook entry number
//
// Description:
//  Opens the Workbook stream at the given entry, parses through
//  it and sets the very hidden bit of the option flags field
//  of every BOUNDSHEET (record 0x85) that has a sheet type
//  of Visual Basic module as specified in the option flags field.
//  The sheet type is changed to worksheet and the name is replaced
//  with a unique random name.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97XLWorkbookVBtoVHWS
(
    LPSS_STREAM         lpstStream,
    DWORD               dwWorkbookEntryNum
)
{
    DWORD                   dwOffset;
    DWORD                   dwByteCount;
    XL_REC_HDR_T            stRec;
    XL8_REC_BOUNDSHEET_T    stBoundsheet;

    // Open the Workbook stream

    if (SSOpenStreamAtIndex(lpstStream,
                            dwWorkbookEntryNum) != SS_STATUS_OK)
    {
        // Failed opening Workbook stream

        return(FALSE);
    }

    // Parse through the records

    dwOffset = 0;
    while (dwOffset < SSStreamLen(lpstStream))
    {
        // Read the record type and record length

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
        {
            // Failed to read record type and length

            return(FALSE);
        }

        dwOffset += sizeof(XL_REC_HDR_T);

        // Endianize

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            // Read the BOUNDSHEET record

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           &stBoundsheet,
                           sizeof(XL8_REC_BOUNDSHEET_T),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(XL8_REC_BOUNDSHEET_T))
            {
                // Failed to read BOUNDSHEET record

                return(FALSE);
            }

            if ((stBoundsheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                XL_REC_BOUNDSHEET_VB_MODULE)
            {
                // It is a VB module sheet, so set the very hidden bit

                stBoundsheet.byHiddenState &= ~(XL_REC_BOUNDSHEET_STATE_MASK);
                stBoundsheet.byHiddenState |= XL_REC_BOUNDSHEET_VERY_HIDDEN;

                // Make it a worksheet

                stBoundsheet.bySheetType &= ~(XL_REC_BOUNDSHEET_TYPE_MASK);
                stBoundsheet.bySheetType |= XL_REC_BOUNDSHEET_WORKSHEET;

                // Write the new BOUNDSHEET record

                if (SSSeekWrite(lpstStream,
                                dwOffset,
                                &stBoundsheet,
                                sizeof(XL8_REC_BOUNDSHEET_T),
                                &dwByteCount) != SS_STATUS_OK ||
                    dwByteCount != sizeof(XL8_REC_BOUNDSHEET_T))
                {
                    // Failed to write BOUNDSHEET record

                    return(FALSE);
                }

                // Give it a new name

                if (XL97AssignRndSheetName(lpstStream,
                                           dwOffset -
                                               sizeof(XL_REC_HDR_T)) == FALSE)
                {
                    // Failed to write new name

                    return(FALSE);
                }
            }
        }

        // Advance to next record

        dwOffset += stRec.wLen;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL97WriteBlankSheet()
//
// Parameters:
//  lpstStream          Ptr to Book stream
//  dwBOFOffset         Offset of BOF record of sheet
//
// Description:
//  The function overwrites the given sheet with the records of a
//  blank sheet represented by the gabyXL97BlankSheetContents[]
//  array of record bytes.  Any remaining space in the sheet is
//  overwritten with HCENTER records in chunks of 256 bytes.
//  The function overwrites all records up until either the 0x01BA
//  record or the EOF record.  The 0x01BA record is assumed to
//  associate the sheet with its VBA module.
//
// Returns:
//  TRUE                On success
//  FALSE               One error
//
//*************************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXL97BlankSheetContents[] =
{
    0x0B, 0x02, 0x10, 0x00,     // INDEX
    0x00, 0x00, 0x00, 0x00,     //  (reserved)
    0x00, 0x00, 0x00, 0x00,     //  rwMic
    0x00, 0x00, 0x00, 0x00,     //  rwMac
    0x00, 0x00, 0x00, 0x00,     //  (reserved)
    0x0D, 0x00, 0x02, 0x00,     // CALCMODE
    0x01, 0x00,                 //  fAutoRecalc
    0x0C, 0x00, 0x02, 0x00,     // CALCCOUNT
    0x64, 0x00,                 //  cIter
    0x0F, 0x00, 0x02, 0x00,     // REFMODE
    0x01, 0x00,                 //  fRefA1
    0x11, 0x00, 0x02, 0x00,     // ITERATION
    0x00, 0x00,                 //  fIteration
    0x10, 0x00, 0x08, 0x00,     // DELTA
    0xFC, 0xA9, 0xF1, 0xD2,     //  numDelta
    0x4D, 0x62, 0x50, 0x3F,     //
    0x5F, 0x00, 0x02, 0x00,     // SAVERECALC
    0x01, 0x00,                 //  fSaveRecalc
    0x2A, 0x00, 0x02, 0x00,     // PRINTHEADERS
    0x00, 0x00,                 //  fPrintRwCol
    0x2B, 0x00, 0x02, 0x00,     // PRINTGRIDLINES
    0x00, 0x00,                 //  fPrintGrid
    0x82, 0x00, 0x02, 0x00,     // GRIDSET
    0x01, 0x00,                 //  fGridSet
    0x80, 0x00, 0x08, 0x00,     // GUTS
    0x00, 0x00, 0x00, 0x00,     //  dxRwGut         dyColGut
    0x00, 0x00, 0x00, 0x00,     //  iLevelRwMac     iLevelColMac
    0x25, 0x02, 0x04, 0x00,     // DEFAULTROWHEIGHT
    0x00, 0x00, 0xFF, 0x00,     //  grbit           miyRw
    0x81, 0x00, 0x02, 0x00,     // WSBOOL
    0xC1, 0x04,                 //  grbit
    0x14, 0x00, 0x00, 0x00,     // HEADER
    0x15, 0x00, 0x00, 0x00,     // FOOTER
    0x83, 0x00, 0x02, 0x00,     // HCENTER
    0x00, 0x00,                 //  fHCenter
    0x84, 0x00, 0x02, 0x00,     // VCENTER
    0x00, 0x00,                 //  fVCenter
    0xA1, 0x00, 0x22, 0x00,     // SETUP
    0x00, 0x00, 0xFF, 0x00,     //  iPaperSize      iScale
    0x01, 0x00, 0x01, 0x00,     //  iPageStart      iFitWidth
    0x01, 0x00, 0x06, 0x00,     //  iFitHeight      grBit
    0x00, 0x00, 0x00, 0x00,     //  iRes            iVRes
    0x00, 0x00, 0x00, 0x00,     //  numHdr
    0x00, 0x00, 0xE0, 0x3F,     //
    0x00, 0x00, 0x00, 0x00,     //  numFtr
    0x00, 0x00, 0xE0, 0x3F,     //
    0x00, 0x00,                 //  iCopies
    0x55, 0x00, 0x02, 0x00,     // DEFCOLWIDTH
    0x08, 0x00,                 //  cchdefColWidth
    0x00, 0x02, 0x0E, 0x00,     // DIMENSIONS
    0x00, 0x00, 0x00, 0x00,     //  rwMic
    0x00, 0x00, 0x00, 0x00,     //  rwMac
    0x00, 0x00, 0x00, 0x00,     //  colMic          colMac
    0x00, 0x00,                 //  (reserved)
    0x3E, 0x02, 0x12, 0x00,     // WINDOW2
    0xB6, 0x06, 0x00, 0x00,     //  grbit           rwTop
    0x00, 0x00,                 //  colLeft
    0x40, 0x00, 0x00, 0x00,     //  icvHdr
    0x00, 0x00, 0x00, 0x00,     //  wScaleSLV       wScaleNormal
    0x00, 0x00, 0x00, 0x00,     //  (reserved)
    0x1D, 0x00, 0x0F, 0x00,     // SELECTION
    0x03,                       //  pnn
    0x00, 0x00, 0x00, 0x00,     //  rwAct           colAct
    0x00, 0x00, 0x01, 0x00,     //  irefAct         cref
    0x00, 0x00, 0x00, 0x00,     //  rwFirst         rwLast
    0x00, 0x00,                 //  colFirst        colLast
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL XL97WriteBlankSheet
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwBOFOffset
)
{
    DWORD                   dwByteCount;
    DWORD                   dwOffset;
    DWORD                   dwFirstRecOffset;
    DWORD                   dwMaxOffset;
    DWORD                   dwEOFOffset;
    DWORD                   dwDepth;
    DWORD                   dwBytesLeft;
    DWORD                   dwNumBytes;
    BYTE                    abyBuf[256];
    XL_REC_HDR_T            stBOFRecHdr;
    XL_REC_HDR_T            stRec;

    // Read the BOF record header

    if (SSSeekRead(lpstStream,
                   dwBOFOffset,
                   &stBOFRecHdr,
                   sizeof(XL_REC_HDR_T),
                   &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(XL_REC_HDR_T))
        return(FALSE);

    stBOFRecHdr.wType = WENDIAN(stBOFRecHdr.wType);
    stBOFRecHdr.wLen = WENDIAN(stBOFRecHdr.wLen);

    // Determine the EOF offset;

    dwDepth = 0;
    dwFirstRecOffset = dwBOFOffset + stBOFRecHdr.wLen + sizeof(XL_REC_HDR_T);
    dwOffset = dwFirstRecOffset;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(FALSE);

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOF)
            ++dwDepth;
        else
        if (stRec.wType == eXLREC_EOF)
        {
            if (dwDepth == 0)
                break;

            --dwDepth;
        }
        else
        if (stRec.wType == 0x01BA)
        {
            if (dwDepth == 0)
                break;
        }

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    dwEOFOffset = dwOffset;

    // Enough room for modifications?

    if (dwEOFOffset - dwFirstRecOffset <
        sizeof(gabyXL97BlankSheetContents) + sizeof(XL_REC_HDR_T))
    {
        // Leave it if there is no room for the records of a blank sheet

        return(TRUE);
    }

    // Overwrite the beginning with the blank sheet contents

    if (SSSeekWrite(lpstStream,
                    dwFirstRecOffset,
                    gabyXL97BlankSheetContents,
                    sizeof(gabyXL97BlankSheetContents),
                    &dwByteCount) != SS_STATUS_OK ||
        dwByteCount != sizeof(gabyXL97BlankSheetContents))
        return(FALSE);

    // Now overwrite the rest with empty records

    dwOffset = dwFirstRecOffset + sizeof(gabyXL97BlankSheetContents);
    dwBytesLeft = dwEOFOffset - dwOffset;

    for (dwNumBytes=0;dwNumBytes<256;++dwNumBytes)
        abyBuf[dwNumBytes] = 0;

    abyBuf[0] = eXLREC_HCENTER;
    abyBuf[2] = 256 - sizeof(XL_REC_HDR_T);
    while (dwBytesLeft >= 260)
    {
        if (SSSeekWrite(lpstStream,
                        dwOffset,
                        abyBuf,
                        256,
                        &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != 256)
            return(FALSE);

        dwOffset += 256;
        dwBytesLeft -= 256;
    }

    // Write the next to the last chunk.  This chunk
    //  will only be the size of a record header.

    if (dwBytesLeft > 256)
    {
        abyBuf[2] = 0;
        if (SSSeekWrite(lpstStream,
                        dwOffset,
                        abyBuf,
                        sizeof(XL_REC_HDR_T),
                        &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(FALSE);

        dwOffset += sizeof(XL_REC_HDR_T);
        dwBytesLeft -= sizeof(XL_REC_HDR_T);
    }

    // Write the last chunk

    if (dwBytesLeft != 0)
    {
        abyBuf[2] = (BYTE)(dwBytesLeft - sizeof(XL_REC_HDR_T));
        if (SSSeekWrite(lpstStream,
                        dwOffset,
                        abyBuf,
                        dwBytesLeft,
                        &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != dwBytesLeft)
            return(FALSE);

        dwOffset += dwBytesLeft;
        dwBytesLeft = 0;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL97WorkbookIsEncrypted()
//
// Parameters:
//  lpstStream          Ptr to Workbook stream
//  lpbIsEncrypted      Ptr to BOOL for encrypted state
//
// Description:
//  The function iterates through the records of the given Workbook
//  stream looking for a FILEPASS record.  If it is found, the
//  function sets *lpbIsEncrypted to TRUE.
//
//  The function stops the search and sets *lpbIsEncrypted to FALSE
//  if it sees a WRITEACCESS record, an EOF record, or the end
//  of the stream.
//
// Returns:
//  TRUE                On success
//  FALSE               One error
//
//*************************************************************************

BOOL XL97WorkbookIsEncrypted
(
    LPSS_STREAM             lpstStream,
    LPBOOL                  lpbIsEncrypted
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    DWORD                   dwByteCount;

    // Iterate through records

    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    *lpbIsEncrypted = FALSE;
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(FALSE);

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_FILEPASS)
        {
            *lpbIsEncrypted = TRUE;
            return(TRUE);
        }

        if (stRec.wType == eXLREC_EOF ||
            stRec.wType == eXLREC_WRITEACCESS)
            return(TRUE);

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL XL97EntryHasEncryptedWBSib()
//
// Parameters:
//  dwEntry             Entry to check
//  lpstStream          Ptr to scratch stream to use
//  lpbResult           Ptr to BOOL for WB encrypted sib state
//
// Description:
//  The function determines whether the stream at the given entry
//  has a sibling Workbook stream that is encrypted.
//
// Returns:
//  TRUE                On success
//  FALSE               One error
//
//*************************************************************************

BOOL XL97EntryHasEncryptedWBSib
(
    DWORD                   dwEntry,
    LPSS_STREAM             lpstStream,
    LPBOOL                  lpbResult
)
{
    DWORD                   dwParentID;
    DWORD                   dwChildID;
    LPSS_ENUM_SIBS          lpstSibs;

    // Get the parent storage of the entry

    if (SSGetParentStorage(lpstStream->lpstRoot,
                           dwEntry,
                           &dwParentID,
                           &dwChildID) != SS_STATUS_OK)
        return(FALSE);

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstStream->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
        return(FALSE);

    // Found a storage, so go through children looking
    //  for what we need

    SSInitEnumSibsStruct(lpstSibs,
                         dwChildID);


    if (SSEnumSiblingEntriesCB(lpstStream->lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywszWorkbook,
                               lpstSibs,
                               lpstStream) == SS_STATUS_OK)
    {
        // Found it

        if (XL97WorkbookIsEncrypted(lpstStream,
                                    lpbResult) == FALSE)
        {
            SSFreeEnumSibsStruct(lpstStream->lpstRoot,lpstSibs);
            return(FALSE);
        }
    }
    else
        *lpbResult = FALSE;

    // Free the enumeration structure

    SSFreeEnumSibsStruct(lpstStream->lpstRoot,lpstSibs);

    return(TRUE);
}

