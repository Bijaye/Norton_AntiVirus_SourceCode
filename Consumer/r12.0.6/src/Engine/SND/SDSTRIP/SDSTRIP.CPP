// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SDSTRIP/VCS/sdstrip.cpv   1.3   04 May 1998 12:17:54   DCHI  $
//
// Description:
//  Scan and Deliver Stripper class implementation
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/SDSTRIP/VCS/sdstrip.cpv  $
// 
//    Rev 1.3   04 May 1998 12:17:54   DCHI
// Modified Excel 95 and Excel 97 strippers to be much more thorough.
// 
//    Rev 1.2   27 Apr 1998 12:42:26   DCHI
// Fixed assumption of OLE files.  Now no assumption.
// 
//    Rev 1.1   23 Apr 1998 15:18:46   DCHI
// Corrected problem with checking PN value.
// 
//    Rev 1.0   31 Mar 1998 16:00:18   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"

#define _SDSTRIP_CPP_

#include "sdstrip.h"

#include "olessapi.h"
#include "olestrnm.h"
#include "wd7api.h"
#include "xl5api.h"
#include "o97api.h"
#include "wddecsig.h"

class CContentStripperI : public CContentStripper
{
    public:
        INIT_STATUS StartUp
        (
            LPCTSTR         lpszDataFileDirectory
        );

        STRIP_STATUS Strip
        (
            LPCTSTR         lpszInputFileName,
            LPCTSTR         lpszOutputFileName
        );

        void ShutDown
        (
            void
        );

        void Release
        (
            void
        );

    private:
        BOOL CopyFile
        (
            LPCTSTR         lpszInputFileName,
            LPCTSTR         lpszOutputFileName
        );

        static int StripCB
        (
            LPSS_DIR_ENTRY  lpstEntry,
            DWORD           dwIndex,
            LPVOID          lpvCookie
        );

        STRIP_STATUS StripWD95
        (
            LPSS_STREAM     lpstStream
        );

        STRIP_STATUS StripWD97
        (
            LPSS_STREAM     lpstStream
        );

        STRIP_STATUS StripXL95
        (
            LPSS_STREAM     lpstStream
        );

        STRIP_STATUS StripXL97
        (
            LPSS_STREAM     lpstStream
        );

        STRIP_STATUS StripContents
        (
            HANDLE          hFile
        );
};

//********************************************************************
//
// Function:
//  INIT_STATUS CContentStripperI::StartUp()
//
// Parameters:
//  lpszDataFileDirectory   Directory without trailing backslash that
//                          contains all stripper support data files.
//
// Description:
//  Initializes and loads all data for the stripper.
//
// Returns:
//  INIT_NO_ERROR               On successful load of data
//  INIT_MALLOC_ERROR           On failure due to failed memory allocation
//  INIT_FILE_NOT_FOUND_ERROR   On failure due to data file not found
//  INIT_GENERAL_FILE_ERROR     On general file error or bad data file contents
//
//********************************************************************

CContentStripper::INIT_STATUS CContentStripperI::StartUp
(
    LPCTSTR         lpszDataFileDirectory
)
{
    (void)lpszDataFileDirectory;

    return(INIT_NO_ERROR);
}


//********************************************************************
//
// Function:
//  STRIP_STATUS CContentStripperI::Strip()
//
// Parameters:
//  lpszInputFileName           Input file name
//  lpszOutputFileName          Output file name
//
// Description:
//  Strips the file of text/data.
//
// Returns:
//  STRIP_NO_ERROR              On successful load of data
//  STRIP_MALLOC_ERROR          On failure due to failed memory allocation
//  STRIP_PASSWORD_ENCRYPTED    Encrypted file, can't strip
//  STRIP_FILE_NOT_FOUND_ERROR  On failure due to data file not found
//  STRIP_GENERAL_FILE_ERROR    On general file error or bad data file contents
//
//********************************************************************

CContentStripper::STRIP_STATUS CContentStripperI::Strip
(
    LPCTSTR         lpszInputFileName,
    LPCTSTR         lpszOutputFileName
)
{
    HANDLE          hFile;
    STRIP_STATUS    stripStatus;

    // First, create a copy of the file

    if (CopyFile(lpszInputFileName,
                 lpszOutputFileName) == FALSE)
        return(STRIP_GENERAL_FILE_ERROR);

    // Now open the file in read/write mode

    // Open the input file for reading

    hFile = CreateFile(lpszOutputFileName,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return(STRIP_GENERAL_FILE_ERROR);

    // Now strip

    stripStatus = StripContents(hFile);

    // Close the file

    CloseHandle(hFile);

    return(stripStatus);
}


//********************************************************************
//
// Function:
//  void CContentStripperI::ShutDown()
//
// Parameters:
//  None
//
// Description:
//  Unloads all data allocated by StartUp
//
// Returns:
//  Nothing
//
//********************************************************************

void CContentStripperI::ShutDown
(
    void
)
{
}


//********************************************************************
//
// Function:
//  void CContentStripperI::Release()
//
// Parameters:
//  None
//
// Description:
//  Does a delete this basically
//
// Returns:
//  Nothing
//
//********************************************************************

void CContentStripperI::Release
(
    void
)
{
    delete this;
}


//********************************************************************
//
// Function:
//  BOOL CContentStripperI::CopyFile()
//
// Parameters:
//  lpszInputFileName           Input file name
//  lpszOutputFileName          Output file name
//
// Description:
//  Strips the file of text/data.
//
// Returns:
//  TRUE                        On success
//  FALSE                       On error
//
//********************************************************************

BOOL CContentStripperI::CopyFile
(
    LPCTSTR         lpszInputFileName,
    LPCTSTR         lpszOutputFileName
)
{
    HANDLE          hInputFile;
    HANDLE          hOutputFile;
    DWORD           dwFileSize;
    BYTE            abyBuf[1024];
    DWORD           dwChunkSize;
    DWORD           dwByteCount;

    // Open the input file for reading

    hInputFile = CreateFile(lpszInputFileName,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL);

    if (hInputFile == INVALID_HANDLE_VALUE)
        return(FALSE);

    // Open the output file for writing

    hOutputFile = CreateFile(lpszOutputFileName,
                             GENERIC_WRITE,
                             0,
                             NULL,
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

    if (hOutputFile == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hInputFile);
        return(FALSE);
    }

    // Get the file size

    dwFileSize = GetFileSize(hInputFile,NULL);
    if (dwFileSize == 0xFFFFFFFF)
    {
        CloseHandle(hOutputFile);
        CloseHandle(hInputFile);
        return(FALSE);
    }

    // Now copy from the input to the output

    dwChunkSize = sizeof(abyBuf);
    while (dwFileSize != 0)
    {
        if (dwFileSize < dwChunkSize)
            dwChunkSize = dwFileSize;

        if (ReadFile(hInputFile,
                     abyBuf,
                     dwChunkSize,
                     &dwByteCount,
                     NULL) == FALSE ||
            dwByteCount != dwChunkSize)
        {
            CloseHandle(hOutputFile);
            CloseHandle(hInputFile);
            return(FALSE);
        }

        if (WriteFile(hOutputFile,
                      abyBuf,
                      dwByteCount,
                      &dwByteCount,
                      NULL) == FALSE ||
            dwByteCount != dwChunkSize)
        {
            CloseHandle(hOutputFile);
            CloseHandle(hInputFile);
            return(FALSE);
        }

        dwFileSize -= dwChunkSize;
    }

    CloseHandle(hOutputFile);
    CloseHandle(hInputFile);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int CContentStripperI::StripCB()
//
// Description:
//  Looks for the following:
//      WordDocument stream -
//          Sets dwScanType to STRIP_WORD
//      Book stream -
//          Sets dwScanType to STRIP_XL95
//      Workbook stream -
//          Sets dwScanType to STRIP_XL97
//
// Returns:
//  OLE_OPEN_CB_STATUS_RETURN       If one of the above was found
//  OLE_OPEN_CB_STATUS_CONTINUE     If none of the above were found
//
//********************************************************************

#define STRIP_WORD           1
#define STRIP_XL95           2
#define STRIP_XL97           3

int CContentStripperI::StripCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
)
{
    (void)dwIndex;

    if (lpstEntry->byMSE == STGTY_STREAM)
    {
        // Check for "WordDocument"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWordDocument,
                       SS_MAX_NAME_LEN) == 0)
        {
            *(LPDWORD)lpvCookie = STRIP_WORD;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "Book"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszBook,
                       SS_MAX_NAME_LEN) == 0)
        {
            *(LPDWORD)lpvCookie = STRIP_XL95;
            return(SS_ENUM_CB_STATUS_OPEN);
        }

        // Check for "Workbook"

        if (SSWStrNCmp(lpstEntry->uszName,
                       (LPWORD)gabywszWorkbook,
                       SS_MAX_NAME_LEN) == 0)
        {
            *(LPDWORD)lpvCookie = STRIP_XL97;
            return(SS_ENUM_CB_STATUS_OPEN);
        }
    }

    return(SS_ENUM_CB_STATUS_CONTINUE);
}


//********************************************************************
//
// Function:
//  STRIP_STATUS CContentStripperI::StripWD95()
//
// Parameters:
//  LPSS_STREAM                 Ptr to stream to strip
//
// Description:
//  Strips the Word 6.0/95 stream of text/data.
//
// Returns:
//  STRIP_NO_ERROR              On successful load of data
//  STRIP_MALLOC_ERROR          On failure due to failed memory allocation
//  STRIP_PASSWORD_ENCRYPTED    Encrypted file, can't strip
//  STRIP_FILE_NOT_FOUND_ERROR  On failure due to data file not found
//  STRIP_GENERAL_FILE_ERROR    On general file error or bad data file contents
//
//********************************************************************

CContentStripper::STRIP_STATUS CContentStripperI::StripWD95
(
    LPSS_STREAM     lpstStream
)
{
    WD7ENCKEY_T     stKey;
    DWORD           dwOffset;
    WORD            wPN;
    WD7FIB_T        stFIB;
    int             i;

    // Get the encryption key

    if (WD7FindFirstKey(lpstStream,
                        &stKey,
                        &gstRevKeyLocker) != WD7_STATUS_OK)
        return(STRIP_PASSWORD_ENCRYPTED);

    // Iterate through each of the documents in the stream

    dwOffset = 0;
    for (i=0;i<1024;i++)
    {
        // Read the FIB

        if (WD7EncryptedRead(lpstStream,
                             &stKey,
                             dwOffset,
                             (LPBYTE)&stFIB,
                             sizeof(WD7FIB_T)) != sizeof(WD7FIB_T))
            return(STRIP_GENERAL_FILE_ERROR);

        // Do not remove the text if this is a glossary document

        if ((stFIB.byFlags0 & WD7FIB_FLAGS0_GLSY) == 0)
        {
            if (WD7ClearDocument(lpstStream,
                                 &stKey,
                                 &stFIB,
                                 dwOffset) == FALSE)
                return(STRIP_GENERAL_FILE_ERROR);
        }

        wPN = WENDIAN(stFIB.wPN);
        if (wPN == 0 || (DWORD)wPN * (DWORD)512 < dwOffset)
            break;

        dwOffset = (DWORD)wPN * (DWORD)512;
    }

    return(STRIP_NO_ERROR);
}


//********************************************************************
//
// Function:
//  STRIP_STATUS CContentStripperI::StripWD97()
//
// Parameters:
//  LPSS_STREAM                 Ptr to stream to strip
//
// Description:
//  Strips the Word 97 stream of text/data.
//
// Returns:
//  STRIP_NO_ERROR              On successful load of data
//  STRIP_MALLOC_ERROR          On failure due to failed memory allocation
//  STRIP_PASSWORD_ENCRYPTED    Encrypted file, can't strip
//  STRIP_FILE_NOT_FOUND_ERROR  On failure due to data file not found
//  STRIP_GENERAL_FILE_ERROR    On general file error or bad data file contents
//
//********************************************************************

CContentStripper::STRIP_STATUS CContentStripperI::StripWD97
(
    LPSS_STREAM     lpstStream
)
{
    DWORD           dwOffset;
    WORD            wPN;
    W97FIB_T        stFIB;
    DWORD           dwByteCount;
    LPSS_STREAM     lpstTableStream;
    DWORD           dwID;
    int             i;

    // Allocate a stream structure for the table stream

    if (SSAllocStreamStruct(lpstStream->lpstRoot,
                            &lpstTableStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
        return(STRIP_GENERAL_FILE_ERROR);

    // Iterate through each of the documents in the stream

    dwOffset = 0;
    for (i=0;i<1024;i++)
    {
        // Read the FIB

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       (LPBYTE)&stFIB,
                       sizeof(W97FIB_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(W97FIB_T))
        {
            SSFreeStreamStruct(lpstTableStream);
            return(STRIP_GENERAL_FILE_ERROR);
        }

        // If it is encrypted, return error

        if ((stFIB.byFlags1 & W97FIB_FLAGS1_ENCRYPTED) != 0)
        {
            SSFreeStreamStruct(lpstTableStream);
            return(STRIP_PASSWORD_ENCRYPTED);
        }

        // Get the appropriate Table stream ID

        if ((stFIB.byFlags1 & W97FIB_FLAGS1_WHICH_TBL_STREAM) == 0)
        {
            // Get the 0Table stream

            if (SSGetNamedSiblingID(lpstStream->lpstRoot,
                                    SSStreamID(lpstStream),
                                    gabywsz0Table,
                                    &dwID) == FALSE)
            {
                SSFreeStreamStruct(lpstTableStream);
                return(STRIP_GENERAL_FILE_ERROR);
            }
        }
        else
        {
            // Get the Table1 stream

            if (SSGetNamedSiblingID(lpstStream->lpstRoot,
                                    SSStreamID(lpstStream),
                                    gabywsz1Table,
                                    &dwID) == FALSE)
            {
                SSFreeStreamStruct(lpstTableStream);
                return(STRIP_GENERAL_FILE_ERROR);
            }
        }

        // Open the Table stream

        if (SSOpenStreamAtIndex(lpstTableStream,
                                dwID) != SS_STATUS_OK)
        {
            SSFreeStreamStruct(lpstTableStream);
            return(STRIP_GENERAL_FILE_ERROR);
        }

        // Do not remove the text if this is a glossary document

        if ((stFIB.byFlags0 & W97FIB_FLAGS0_GLSY) == 0)
        {
            if (W97ClearDocument(lpstStream,
                                 lpstTableStream,
                                 &stFIB,
                                 dwOffset) == FALSE)
            {
                SSFreeStreamStruct(lpstTableStream);
                return(STRIP_GENERAL_FILE_ERROR);
            }
        }

        wPN = WENDIAN(stFIB.wPN);
        if (wPN == 0 || (DWORD)wPN * (DWORD)512 < dwOffset)
            break;

        dwOffset = (DWORD)wPN * (DWORD)512;
    }

    SSFreeStreamStruct(lpstTableStream);

    return(STRIP_NO_ERROR);
}


//********************************************************************
//
// Function:
//  STRIP_STATUS CContentStripperI::StripXL95()
//
// Parameters:
//  LPSS_STREAM                 Ptr to stream to strip
//
// Description:
//  Strips the Excel 5.0/95 stream of text/data.
//
// Returns:
//  STRIP_NO_ERROR              On successful load of data
//  STRIP_MALLOC_ERROR          On failure due to failed memory allocation
//  STRIP_PASSWORD_ENCRYPTED    Encrypted file, can't strip
//  STRIP_FILE_NOT_FOUND_ERROR  On failure due to data file not found
//  STRIP_GENERAL_FILE_ERROR    On general file error or bad data file contents
//
//********************************************************************

CContentStripper::STRIP_STATUS CContentStripperI::StripXL95
(
    LPSS_STREAM             lpstStream
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    XL_REC_BOUNDSHEET_T     stBoundSheet;
    DWORD                   dwByteCount;
    DWORD                   dwSheetNum;
    XL5ENCKEY_T             stKey;

    // Get the encryption key

    if (XL5FindKey(lpstStream,
                   &stKey) == FALSE)
        return(STRIP_PASSWORD_ENCRYPTED);

    dwSheetNum = 0;
    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(STRIP_GENERAL_FILE_ERROR);

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            // Read the BOUNDSHEET structure

            if (XL5EncryptedRead(lpstStream,
                                 dwOffset,
                                 stRec,
                                 &stKey,
                                 0,
                                 (LPBYTE)&stBoundSheet,
                                 sizeof(XL_REC_BOUNDSHEET_T)) == FALSE)
                return(STRIP_GENERAL_FILE_ERROR);

            // Print out the sheet type

            if ((stBoundSheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                XL_REC_BOUNDSHEET_WORKSHEET ||
                (stBoundSheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                XL_REC_BOUNDSHEET_40_MACRO)
            {
                // Replace the name with a random name

                if (XL5AssignRndSheetName(lpstStream,
                                          &stKey,
                                          dwOffset) == FALSE)
                    return(STRIP_GENERAL_FILE_ERROR);

                // Endianize boundsheet offset

                stBoundSheet.dwOffset = DWENDIAN(stBoundSheet.dwOffset);

                // Blank out cell values

                if (XL5WriteBlankSheet(lpstStream,
                                       &stKey,
                                       stBoundSheet.dwOffset) == FALSE)
                    return(STRIP_GENERAL_FILE_ERROR);
            }

            ++dwSheetNum;
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    return(STRIP_NO_ERROR);
}


//********************************************************************
//
// Function:
//  STRIP_STATUS CContentStripperI::StripXL97()
//
// Parameters:
//  LPSS_STREAM                 Ptr to stream to strip
//
// Description:
//  Strips the Excel 97 stream of text/data.
//
// Returns:
//  STRIP_NO_ERROR              On successful load of data
//  STRIP_MALLOC_ERROR          On failure due to failed memory allocation
//  STRIP_PASSWORD_ENCRYPTED    Encrypted file, can't strip
//  STRIP_FILE_NOT_FOUND_ERROR  On failure due to data file not found
//  STRIP_GENERAL_FILE_ERROR    On general file error or bad data file contents
//
//********************************************************************

CContentStripper::STRIP_STATUS CContentStripperI::StripXL97
(
    LPSS_STREAM     lpstStream
)
{
    DWORD                   dwOffset;
    DWORD                   dwMaxOffset;
    XL_REC_HDR_T            stRec;
    XL8_REC_BOUNDSHEET_T    stBoundSheet;
    DWORD                   dwByteCount;
    DWORD                   dwSheetNum;

    // If the document is encrypted, don't bother

    if (XLFindFirstGlobalRec(lpstStream,
                             eXLREC_FILEPASS,
                             &dwOffset,
                             &stRec) != FALSE)
        return(STRIP_PASSWORD_ENCRYPTED);

    dwSheetNum = 0;
    dwOffset = 0;
    dwMaxOffset = SSStreamLen(lpstStream);
    while (dwOffset < dwMaxOffset)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &stRec,
                       sizeof(XL_REC_HDR_T),
                       &dwByteCount) != SS_STATUS_OK ||
            dwByteCount != sizeof(XL_REC_HDR_T))
            return(STRIP_GENERAL_FILE_ERROR);

        stRec.wType = WENDIAN(stRec.wType);
        stRec.wLen = WENDIAN(stRec.wLen);

        if (stRec.wType == eXLREC_BOUNDSHEET)
        {
            // Read the BOUNDSHEET structure

            if (SSSeekRead(lpstStream,
                           dwOffset + sizeof(XL_REC_HDR_T),
                           &stBoundSheet,
                           sizeof(XL8_REC_BOUNDSHEET_T),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(XL8_REC_BOUNDSHEET_T))
                return(STRIP_GENERAL_FILE_ERROR);

            // Print out the sheet type

            if ((stBoundSheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                XL_REC_BOUNDSHEET_WORKSHEET ||
                (stBoundSheet.bySheetType & XL_REC_BOUNDSHEET_TYPE_MASK) ==
                XL_REC_BOUNDSHEET_40_MACRO)
            {
                if (XL97AssignRndSheetName(lpstStream,
                                           dwOffset) == FALSE)
                    return(STRIP_GENERAL_FILE_ERROR);

                // Endianize boundsheet offset

                stBoundSheet.dwOffset = DWENDIAN(stBoundSheet.dwOffset);

                // Blank out cell values

                if (XL97WriteBlankSheet(lpstStream,
                                        stBoundSheet.dwOffset) == FALSE)
                    return(STRIP_GENERAL_FILE_ERROR);
            }

            ++dwSheetNum;
        }
        else
        if (stRec.wType == eXLREC_EOF)
            break;

        dwOffset += sizeof(XL_REC_HDR_T) + stRec.wLen;
    }

    return(STRIP_NO_ERROR);
}


//********************************************************************
//
// Function:
//  STRIP_STATUS CContentStripperI::StripContents()
//
// Parameters:
//  hFile                       Handle to file to strip
//
// Description:
//  Strips the file of text/data.
//
// Returns:
//  STRIP_NO_ERROR              On successful load of data
//  STRIP_MALLOC_ERROR          On failure due to failed memory allocation
//  STRIP_PASSWORD_ENCRYPTED    Encrypted file, can't strip
//  STRIP_FILE_NOT_FOUND_ERROR  On failure due to data file not found
//  STRIP_GENERAL_FILE_ERROR    On general file error or bad data file contents
//
//********************************************************************

CContentStripper::STRIP_STATUS CContentStripperI::StripContents
(
    HANDLE          hFile
)
{
    LPSS_ROOT           lpstRoot;
    LPSS_STREAM         lpstStream;
    DWORD               dwScanType;
    DWORD               dwEntry;
    STRIP_STATUS        stripStatus;
    int                 i;
    BYTE                abyHdr[8];
    DWORD               dwNumBytes;
    long                lNewOffset;

    // Make sure it is an OLE file first

    lNewOffset = SetFilePointer(hFile,
                                0,
                                NULL,
                                SS_SEEK_SET);

    if (lNewOffset != 0)
        return(STRIP_NO_ERROR);

    if (ReadFile(hFile,
                 abyHdr,
                 8,
                 &dwNumBytes,
                 NULL) == FALSE)
        return(STRIP_NO_ERROR);

    if (abyHdr[0] != 0xD0 ||
        abyHdr[1] != 0xCF ||
        abyHdr[2] != 0x11 ||
        abyHdr[3] != 0xE0 ||
        abyHdr[4] != 0xA1 ||
        abyHdr[5] != 0xB1 ||
        abyHdr[6] != 0x1A ||
        abyHdr[7] != 0xE1)
        return(STRIP_NO_ERROR);

    if (SSCreateRoot(&lpstRoot,
                     NULL,
                     hFile,
                     SS_ROOT_FLAG_DIR_CACHE_DEF |
                     SS_ROOT_FLAG_FAT_CACHE_DEF ) != SS_STATUS_OK)
        return(STRIP_MALLOC_ERROR);

    // Allocate a stream structure

    if (SSAllocStreamStruct(lpstRoot,
                            &lpstStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
        return(STRIP_MALLOC_ERROR);

    // Find all Excel and Word streams

    dwEntry = 0;
    for (i=0;i<1024;i++)
    {
        if (SSEnumDirEntriesCB(lpstRoot,
                               StripCB,
                               &dwScanType,
                               &dwEntry,
                               lpstStream) != SS_STATUS_OK)
            break;

        if (dwScanType == STRIP_WORD)
        {
            WORD            wFIBVersion;
            DWORD           dwByteCount;

            if (SSSeekRead(lpstStream,
                           2,
                           &wFIBVersion,
                           sizeof(WORD),
                           &dwByteCount) != SS_STATUS_OK ||
                dwByteCount != sizeof(WORD))
                return(STRIP_GENERAL_FILE_ERROR);

            wFIBVersion = WENDIAN(wFIBVersion);

            if (wFIBVersion < 0x92)
            {
                // Word 95

                stripStatus = StripWD95(lpstStream);
            }
            else
            {
                // Assume Word 97

                stripStatus = StripWD97(lpstStream);
            }
        }
        else
        if (dwScanType == STRIP_XL95)
        {
            // Excel 95

            stripStatus = StripXL95(lpstStream);
        }
        else
        if (dwScanType == STRIP_XL97)
        {
            // Excel 97

            stripStatus = StripXL97(lpstStream);
        }

        if (stripStatus != STRIP_NO_ERROR)
            return(stripStatus);

        if (SSClearSummaryInfo(lpstStream) == FALSE)
            return(STRIP_GENERAL_FILE_ERROR);
    }

    SSFreeStreamStruct(lpstStream);
    SSDestroyRoot(lpstRoot);

    return(STRIP_NO_ERROR);
}


//********************************************************************
//
// Function:
//  CContentStripper *CContentStripperFactory::CreateInstance()
//
// Parameters:
//  None
//
// Description:
//  Creates an instance of the stripper
//
// Returns:
//  CContentStripper *      On success
//  NULL                    On failure
//
//********************************************************************

CContentStripper *CContentStripperFactory::CreateInstance
(
    void
)
{
    return (new CContentStripperI);
}

