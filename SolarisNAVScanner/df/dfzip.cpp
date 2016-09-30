//************************************************************************
//
// $Header:   S:/DF/VCS/DFZIP.CPv   1.0   11 Nov 1997 16:21:16   DCHI  $
//
// Description:
//  ZIP compression implementation.  See INFOZIP source.
//
//************************************************************************
// $Log:   S:/DF/VCS/DFZIP.CPv  $
// 
//    Rev 1.0   11 Nov 1997 16:21:16   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "df.h"
#include "dfzip.h"

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagDF_ZIP_LOC_HDR
{
    WORD        wVer;       // 00: Version needed to extract
    WORD        wFlg;       // 02: Encrypt, deflate flags
    WORD        wHow;       // 04: Compression method
    WORD        wTime;      // 06: Last modified file time, DOS format
    WORD        wDate;      // 08: Last modified file date, DOS format
    DWORD       dwCRC;      // 0A: Uncompressed CRC-32 for file
    DWORD       dwSize;     // 0E: Compressed size in ZIP file
    DWORD       dwLen;      // 12: Uncompressed size
    WORD        wNameLen;   // 16: Length of filename
    WORD        wExtLen;    // 18: Length of extra field
} DF_ZIP_LOC_HDR_T, FAR *LPDF_ZIP_LOC_HDR;

typedef struct tagDF_ZIP_CEN_HDR
{
    WORD        wVerMade;   // 00: Version made by
    WORD        wVer;       // 02: Version needed to extract
    WORD        wFlg;       // 04: Encrypt, deflate flags
    WORD        wHow;       // 06: Compression method
    WORD        wTime;      // 08: Last modified file time, DOS format
    WORD        wDate;      // 0A: Last modified file date, DOS format
    DWORD       dwCRC;      // 0C: Uncompressed CRC-32 for file
    DWORD       dwSize;     // 10: Compressed size in ZIP file
    DWORD       dwLen;      // 14: Uncompressed size
    WORD        wNameLen;   // 18: Length of filename
    WORD        wExtLen;    // 1A: Length of extra field
    WORD        wComLen;    // 1C: Length of file comment
    WORD        wDsk;       // 1E: Disk number start
    WORD        wAttr;      // 20: Internal file attributes
    DWORD       dwXAttr;    // 22: External file attributes
    DWORD       dwLocOffset;// 26: Offset of local header
} DF_ZIP_CEN_HDR_T, FAR *LPDF_ZIP_CEN_HDR;

typedef struct tagDF_ZIP_END_HDR
{
    WORD        wDsk;       // 00: Number of this disk
    WORD        wBeg;       // 02: Number of the starting disk
    WORD        wSub;       // 04: Entries on this disk
    WORD        wTot;       // 06: Total number of entries
    DWORD       dwSize;     // 08: Size of central directory
    DWORD       dwOffset;   // 0C: Offset of central directory
    WORD        wComLen;    // 0E: Length of zip file comment
} DF_ZIP_END_HDR_T, FAR *LPDF_ZIP_END_HDR;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif

// ZIP header signatures

#define DF_ZIP_HDR_SIG_LEN  4

BYTE gabyDFZipLocHdrSig[DF_ZIP_HDR_SIG_LEN] = { 0x50, 0x4B, 0x03, 0x04 };
BYTE gabyDFZipCenHdrSig[DF_ZIP_HDR_SIG_LEN] = { 0x50, 0x4B, 0x01, 0x02 };
BYTE gabyDFZipEndHdrSig[DF_ZIP_HDR_SIG_LEN] = { 0x50, 0x4B, 0x05, 0x06 };

//********************************************************************
//
// Function:
//  BOOL DFZipFile()
//
// Parameters:
//  lpstDF      Ptr to DF structure
//  lpstIn      Input file ptr
//  wNameLen    Length of file name
//  lpabyName   Ptr to file name
//  dwFileSize  Size of compressed file in bytes
//  wDate       File date in MSDOS format
//  wTime       File time in MSDOS format
//
// Description:
//  Before calling this function, the file pointer is assumed
//  to be at the next position where the next file should be
//  compressed.
//
//  Upon return, the file pointer will be after the just compressed
//  file.
//
//  The function notes the current file position as the location
//  of the local header.  The function then seeks forward past
//  where the name should go.  The function can now call deflate.
//  After deflation, the current file position is noted as the
//  position
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//********************************************************************

BOOL DFZipFile
(
    LPDF                lpstDF,
    LPVOID              lpvFileIn,
    WORD                wNameLen,
    LPBYTE              lpabyName,
    DWORD               dwFileSize,
    WORD                wDate,
    WORD                wTime
)
{
    long                lHdrOffset;
    long                lNextOffset;
    long                lNewOffset;
    DF_ZIP_LOC_HDR_T    stHdr;
    DWORD               dwBytesWritten;

    lpstDF->dwUncompressedSize = dwFileSize;

    // Write the local header signature

    if (DFFileWrite(lpstDF->lpvRootCookie,
                    lpstDF->lpvFile,
                    gabyDFZipLocHdrSig,
                    DF_ZIP_HDR_SIG_LEN,
                    &dwBytesWritten) != DF_STATUS_OK ||
        dwBytesWritten != DF_ZIP_HDR_SIG_LEN)
        return(FALSE);

    // Remember location of local header

    if (DFFileSeek(lpstDF->lpvRootCookie,
                   lpstDF->lpvFile,
                   0,
                   DF_SEEK_CUR,
                   &lHdrOffset) != DF_STATUS_OK)
        return(FALSE);

    // Seek past the header

    if (DFFileSeek(lpstDF->lpvRootCookie,
                   lpstDF->lpvFile,
                   sizeof(DF_ZIP_LOC_HDR_T),
                   DF_SEEK_CUR,
                   &lNewOffset) != DF_STATUS_OK)
        return(FALSE);

    // Write the name

    if (DFFileWrite(lpstDF->lpvRootCookie,
                    lpstDF->lpvFile,
                    lpabyName,
                    wNameLen,
                    &dwBytesWritten) != DF_STATUS_OK ||
        dwBytesWritten != wNameLen)
        return(FALSE);

    // Now deflate

    if (DFDeflate(lpstDF,
                  lpvFileIn) == FALSE)
        return(FALSE);

    // Get the offset for the next file

    if (DFFileSeek(lpstDF->lpvRootCookie,
                   lpstDF->lpvFile,
                   0,
                   DF_SEEK_CUR,
                   &lNextOffset) != DF_STATUS_OK)
        return(FALSE);

    // Update the header

    stHdr.wVer = 20;
    stHdr.wFlg = 0;
    stHdr.wHow = 8;
    stHdr.wTime = wTime;
    stHdr.wDate = wDate;
    stHdr.dwCRC = ~lpstDF->dwCRC;
    stHdr.dwSize = lpstDF->dwCompressedSize;
    stHdr.dwLen = lpstDF->dwUncompressedSize;
    stHdr.wNameLen = wNameLen;
    stHdr.wExtLen = 0;

    if (DFFileSeek(lpstDF->lpvRootCookie,
                   lpstDF->lpvFile,
                   lHdrOffset,
                   DF_SEEK_SET,
                   &lNewOffset) != DF_STATUS_OK)
        return(FALSE);

    if (DFFileWrite(lpstDF->lpvRootCookie,
                    lpstDF->lpvFile,
                    &stHdr,
                    sizeof(DF_ZIP_LOC_HDR_T),
                    &dwBytesWritten) != DF_STATUS_OK ||
        dwBytesWritten != sizeof(DF_ZIP_LOC_HDR_T))
        return(FALSE);

    // Go to the next file offset

    if (DFFileSeek(lpstDF->lpvRootCookie,
                   lpstDF->lpvFile,
                   lNextOffset,
                   DF_SEEK_SET,
                   &lNewOffset) != DF_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL DFCreateCentralDirectory()
//
// Parameters:
//  pf          Output file ptr
//
// Description:
//  The function creates the central directory at the end of the
//  file, assuming that the file is a sequential set of compressed
//  streams, each with a local header.
//
//  Assumes that the file pointer is after the last compressed file.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//
//********************************************************************

BOOL DFZipCreateCentralDirectory
(
    LPDF                lpstDF
)
{
    DF_ZIP_END_HDR_T    stEndHdr;
    DF_ZIP_CEN_HDR_T    stCenHdr;
    DF_ZIP_LOC_HDR_T    stLocHdr;
    DWORD               dwCenHdrOffset;
    WORD                wNameLen;
    WORD                wNumBytesToRead;
    BYTE                abyBuf[512];
    long                lNewOffset;
    DWORD               dwBytes;

    // Initialize central directory size and offset

    if (DFFileSeek(lpstDF->lpvRootCookie,
                   lpstDF->lpvFile,
                   0,
                   DF_SEEK_CUR,
                   &lNewOffset) != DF_STATUS_OK)
        return(FALSE);

    stEndHdr.dwSize = 0;
    stEndHdr.dwOffset = dwCenHdrOffset = (DWORD)lNewOffset;

    // Initialize remaining fields of end header

    stEndHdr.wDsk = 0;
    stEndHdr.wBeg = 0;
    stEndHdr.wSub = 0;
    stEndHdr.wTot = 0;
    stEndHdr.wComLen = 0;

    // Initialize static fields of central header

    stCenHdr.wVerMade = 20;
    stCenHdr.wVer = 20;
    stCenHdr.wFlg = 0;
    stCenHdr.wHow = 8;
    stCenHdr.wExtLen = 0;
    stCenHdr.wComLen = 0;
    stCenHdr.wDsk = 0;
    stCenHdr.wAttr = 1;
    stCenHdr.dwXAttr = 0x20;
    stCenHdr.dwLocOffset = 0;

    while (stCenHdr.dwLocOffset < stEndHdr.dwOffset)
    {
        // Increment total

        stEndHdr.wTot++;

        // Seek to the next local header

        if (DFFileSeek(lpstDF->lpvRootCookie,
                       lpstDF->lpvFile,
                       stCenHdr.dwLocOffset + DF_ZIP_HDR_SIG_LEN,
                       DF_SEEK_SET,
                       &lNewOffset) != DF_STATUS_OK)
            return(FALSE);

        // Read the local header

        if (DFFileRead(lpstDF->lpvRootCookie,
                       lpstDF->lpvFile,
                       &stLocHdr,
                       sizeof(DF_ZIP_LOC_HDR_T),
                       &dwBytes) != DF_STATUS_OK ||
            dwBytes != sizeof(DF_ZIP_LOC_HDR_T))
            return(FALSE);

        // Set the fields of the central header

        stCenHdr.wTime = stLocHdr.wTime;
        stCenHdr.wDate = stLocHdr.wDate;
        stCenHdr.dwCRC = stLocHdr.dwCRC;
        stCenHdr.dwSize = stLocHdr.dwSize;
        stCenHdr.dwLen = stLocHdr.dwLen;
        stCenHdr.wNameLen = stLocHdr.wNameLen;

        ////////////////////////////////////////////////////
        // Write the central header

        if (DFFileSeek(lpstDF->lpvRootCookie,
                       lpstDF->lpvFile,
                       dwCenHdrOffset,
                       DF_SEEK_SET,
                       &lNewOffset) != DF_STATUS_OK)
            return(FALSE);

        // Write the central header signature

        if (DFFileWrite(lpstDF->lpvRootCookie,
                        lpstDF->lpvFile,
                        gabyDFZipCenHdrSig,
                        DF_ZIP_HDR_SIG_LEN,
                        &dwBytes) != DF_STATUS_OK ||
            dwBytes != DF_ZIP_HDR_SIG_LEN)
            return(FALSE);

        dwCenHdrOffset += DF_ZIP_HDR_SIG_LEN;

        // Write the header

        if (DFFileWrite(lpstDF->lpvRootCookie,
                        lpstDF->lpvFile,
                        &stCenHdr,
                        sizeof(DF_ZIP_CEN_HDR_T),
                        &dwBytes) != DF_STATUS_OK ||
            dwBytes != sizeof(DF_ZIP_CEN_HDR_T))
            return(FALSE);

        dwCenHdrOffset += sizeof(DF_ZIP_CEN_HDR_T);

        // Write the name

        stCenHdr.dwLocOffset += DF_ZIP_HDR_SIG_LEN + sizeof(DF_ZIP_LOC_HDR_T);
        wNumBytesToRead = sizeof(abyBuf);
        wNameLen = stLocHdr.wNameLen;
        while (wNameLen != 0)
        {
            if (wNameLen < wNumBytesToRead)
                wNumBytesToRead = wNameLen;

            // Read from local header

            if (DFFileSeek(lpstDF->lpvRootCookie,
                           lpstDF->lpvFile,
                           stCenHdr.dwLocOffset,
                           DF_SEEK_SET,
                           &lNewOffset) != DF_STATUS_OK)
                return(FALSE);

            if (DFFileRead(lpstDF->lpvRootCookie,
                           lpstDF->lpvFile,
                           abyBuf,
                           wNumBytesToRead,
                           &dwBytes) != DF_STATUS_OK ||
                dwBytes != wNumBytesToRead)
                return(FALSE);

            stCenHdr.dwLocOffset += wNumBytesToRead;

            // Write to central header

            if (DFFileSeek(lpstDF->lpvRootCookie,
                           lpstDF->lpvFile,
                           dwCenHdrOffset,
                           DF_SEEK_SET,
                           &lNewOffset) != DF_STATUS_OK)
                return(FALSE);

            if (DFFileWrite(lpstDF->lpvRootCookie,
                            lpstDF->lpvFile,
                            abyBuf,
                            wNumBytesToRead,
                            &dwBytes) != DF_STATUS_OK ||
                dwBytes != wNumBytesToRead)
                return(FALSE);

            dwCenHdrOffset += wNumBytesToRead;

            // Update number of bytes in name left

            wNameLen -= wNumBytesToRead;
        }

        // Update to next local header offset

        stCenHdr.dwLocOffset += stCenHdr.dwSize;
    }

    /////////////////////////////////////////////////////////////
    // Write end header

    // Compute central directory size

    stEndHdr.dwSize = dwCenHdrOffset - stEndHdr.dwOffset;

    // Set subtotal equal to total

    stEndHdr.wSub = stEndHdr.wTot;

    if (DFFileSeek(lpstDF->lpvRootCookie,
                   lpstDF->lpvFile,
                   dwCenHdrOffset,
                   DF_SEEK_SET,
                   &lNewOffset) != DF_STATUS_OK)
        return(FALSE);

    // Write the end header signature

    if (DFFileWrite(lpstDF->lpvRootCookie,
                    lpstDF->lpvFile,
                    gabyDFZipEndHdrSig,
                    DF_ZIP_HDR_SIG_LEN,
                    &dwBytes) != DF_STATUS_OK ||
        dwBytes != DF_ZIP_HDR_SIG_LEN)
        return(FALSE);

    // Write the end header

    if (DFFileWrite(lpstDF->lpvRootCookie,
                    lpstDF->lpvFile,
                    &stEndHdr,
                    sizeof(DF_ZIP_END_HDR_T),
                    &dwBytes) != DF_STATUS_OK ||
        dwBytes != sizeof(DF_ZIP_END_HDR_T))
        return(FALSE);

    return(TRUE);
}

//***************************************************************

#if 0

void main(int argc, char *argv[])
{
    FILE *  pfIn;
    FILE *  pfOut;
    LPDF    lpstDF;
    unsigned uiDate, uiTime;
    int     i;

    if (argc < 3)
    {
        printf("Usage: df output input\n");
        return;
    }

    lpstDF = DFAllocDF(NULL);
    if (lpstDF == NULL)
    {
        printf("Failed allocating DF structure!\n");
        return;
    }

    pfIn = fopen(argv[2],"rb");
    if (pfIn == NULL)
    {
        printf("Failed to open %s for reading\n",argv[2]);
        return;
    }

    // Get the file's date and time

    _dos_getftime(fileno(pfIn),&uiDate,&uiTime);

    fseek(pfIn,0,SEEK_END);

    pfOut = fopen(argv[1],"wb+");
    if (pfOut == NULL)
    {
        printf("Failed to open %s for writing\n",argv[1]);
        return;
    }

    lpstDF->lpvFile = pfOut;

/*
    if (DFDeflate(lpstDF,pfIn) == FALSE)
    {
        printf("Failed to deflate\n");
    }
*/

    for (i=2;i<argc;i++)
    {
        pfIn = fopen(argv[i],"rb");
        if (pfIn == NULL)
        {
            printf("Failed to open %s for reading\n",argv[2]);
            return;
        }

        // Get the file's date and time

        _dos_getftime(fileno(pfIn),&uiDate,&uiTime);

        fseek(pfIn,0,SEEK_END);
        lpstDF->dwUncompressedSize = ftell(pfIn);

        if (DFZipFile(lpstDF,
                      pfIn,
                      strlen(argv[i]),
                      (LPBYTE)argv[i],
                      lpstDF->dwUncompressedSize,
                      uiDate,
                      uiTime) == FALSE)
        {
            printf("Failed to deflate\n");
        }
        else
            printf("Deflate successful: CRC=0x%08lX Size=%ld\n",
                ~lpstDF->dwCRC,lpstDF->dwCompressedSize);

        fclose(pfIn);
    }

    if (DFZipCreateCentralDirectory(lpstDF) == FALSE)
    {
        printf("Failed to create central directory\n");
    }

    fclose(pfOut);

    free(lpstDF);
}

#endif

//***************************************************************


