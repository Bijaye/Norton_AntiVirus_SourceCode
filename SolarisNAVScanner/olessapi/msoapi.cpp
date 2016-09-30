//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/msoapi.cpv   1.6   14 Jan 1999 15:23:42   DCHI  $
//
// Description:
//  Core Office 9 MSO access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/msoapi.cpv  $
// 
//    Rev 1.6   14 Jan 1999 15:23:42   DCHI
// In MSOIntegrateTDT() made if (dwNewTDTSize <= 2) agree with comments.
// 
//    Rev 1.5   13 Jan 1999 17:24:50   DCHI
// Fixed MSOIntegrateTDT() to write out updated TCG header.
// 
//    Rev 1.4   11 Jan 1999 15:01:18   DCHI
// Re-enabled digital signature removal in MSOIntegrateVBA().
// 
//    Rev 1.3   07 Jan 1999 16:50:44   DCHI
// Reverted back to r1.1.
// 
//    Rev 1.2   07 Jan 1999 16:06:14   DCHI
// Re-enabled digital signature removal in MSOIntegrateVBA().
// 
//    Rev 1.1   19 Nov 1998 15:55:06   DCHI
// Added MSOVBAIsSigned() implementation.
// 
//    Rev 1.0   17 Nov 1998 11:48:26   DCHI
// Initial revision.
// 
//************************************************************************

#include "o97api.h"
#include "msoapi.h"
#include "df.h"
#include "crc32.h"

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#elif defined(__MACINTOSH__)
#pragma options align=packed
#else
#pragma pack(1)
#endif

typedef struct tagMSO_HDR
{
    WORD            wVerInst;
    BYTE            byTypeMin;
    BYTE            byTypeMaj;
    DWORD           dwLength;
} MSO_HDR_T, FAR *LPMSO_HDR;

typedef struct tagMSO_VBA_HDR
{
    DWORD           dwAppSpecificSize;
    DWORD           dwVersion;
    DWORD           dwDigiSigSize;
} MSO_VBA_HDR_T, FAR *LPMSO_VBA_HDR;

typedef struct tagMSO_WORD_TCG_HDR
{
    DWORD           dwUnknown0;
    DWORD           dwTDTSize;
    DWORD           dwUnknown1;
    DWORD           dwUnknown2;
} MSO_WORD_TCG_HDR_T, FAR *LPMSO_WORD_TCG_HDR;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#elif defined(__MACINTOSH__)
#pragma options align=reset
#else
#pragma pack()
#endif

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyMSOHdrSig[10] =
{
    'A', 'c', 't', 'i', 'v', 'e', 'M', 'i', 'm', 'e'
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

//********************************************************************
//
// Function:
//  BOOL MSOInit()
//
// Parameters:
//  lpstMSO             Pointer to MSO structure to initialize
//  lpvRootCookie       Ptr to root cookie
//  lpvFile             Ptr to file
//
// Description:
//  The function initializes the MSO structure for access.
//  It first verifies that the given file begins with the
//  ten-byte string "ActiveMime".  It assumes that the first
//  chunk immediately follows the signature string.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOInit
(
    LPMSO               lpstMSO,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile
)
{
    BYTE                abyBuf[10];
    DWORD               dw;
    int                 i;

    // Read the header

    if (SSFileSeek(lpvRootCookie,
                   lpvFile,
                   0,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileRead(lpvRootCookie,
                   lpvFile,
                   abyBuf,
                   sizeof(gabyMSOHdrSig),
                   &dw) != SS_STATUS_OK ||
        dw != sizeof(gabyMSOHdrSig))
        return(FALSE);

    // Verify header

    for (i=0;i<sizeof(gabyMSOHdrSig);i++)
    {
        if (abyBuf[i] != gabyMSOHdrSig[i])
            return(FALSE);
    }

    lpstMSO->lpvRootCookie = lpvRootCookie;
    lpstMSO->lpvFile = lpvFile;

    lpstMSO->nChunkType = MSO_TYPE_UNKNOWN;
    lpstMSO->dwChunkOffset = sizeof(gabyMSOHdrSig);
    lpstMSO->dwChunkSize = 0;

    // Get the file size

    if (SSFileSeek(lpvRootCookie,
                   lpvFile,
                   0,
                   SS_SEEK_END,
                   (LPLONG)&lpstMSO->dwFileLen) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOGetFirstChunk()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//
// Description:
//  The function assumes that the MSO structure is current.
//
//  The function gets the offset, size, and type of the first
//  chunk.
//
// Returns:
//  TRUE                On success
//  FALSE               On error or no more chunks
//
//********************************************************************

BOOL MSOGetFirstChunk
(
    LPMSO               lpstMSO
)
{
    lpstMSO->nChunkType = MSO_TYPE_UNKNOWN;
    lpstMSO->dwChunkOffset = sizeof(gabyMSOHdrSig);
    lpstMSO->dwChunkSize = 0;

    return MSOGetNextChunk(lpstMSO);
}


//********************************************************************
//
// Function:
//  BOOL MSOGetNextChunk()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//
// Description:
//  The function assumes that the MSO structure is current.
//
//  The function gets the offset, size, and type of the next
//  chunk.
//
// Returns:
//  TRUE                On success
//  FALSE               On error or no more chunks
//
//********************************************************************

BOOL MSOGetNextChunk
(
    LPMSO               lpstMSO
)
{
    MSO_HDR_T           stHdr;
    DWORD               dw;

    // Verify that the next chunk would be within bounds
    //  by checking the offset and size of the current chunk
    //  against the file length

    if (lpstMSO->dwChunkOffset >= lpstMSO->dwFileLen)
        return(FALSE);

    // Update the current chunk offset

    lpstMSO->dwChunkOffset += lpstMSO->dwChunkSize;

    // Now read the chunk header

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   lpstMSO->dwChunkOffset,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileRead(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   &stHdr,
                   sizeof(MSO_HDR_T),
                   &dw) != SS_STATUS_OK ||
        dw != sizeof(MSO_HDR_T))
        return(FALSE);

    // Store the version and instance fields

    lpstMSO->wVerInst = WENDIAN(stHdr.wVerInst);

    // Get the chunk type

    lpstMSO->nChunkType = stHdr.byTypeMin;

    // Get the length of the chunk

    lpstMSO->dwChunkSize = DWENDIAN(stHdr.dwLength);

    // Set the offset of the chunk

    lpstMSO->dwChunkOffset += sizeof(MSO_HDR_T);

    // Validate the chunk size and limit to EOF

    if (lpstMSO->dwChunkOffset >= lpstMSO->dwFileLen)
        lpstMSO->dwChunkSize = 0;
    else
    if (lpstMSO->dwFileLen - lpstMSO->dwChunkOffset <
        lpstMSO->dwChunkSize)
        lpstMSO->dwChunkSize =
            lpstMSO->dwFileLen - lpstMSO->dwChunkOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  void MSOGetChunkHdrOffset()
//
// Parameters:
//  dwChunkOffset       Offset ot chunk data
//  lpdwHdrOffset       Ptr to DWORD for chunk header offset
//
// Description:
//  The function returns the offset of the chunk header given
//  the chunk data offset.
//
// Returns:
//  Nothing
//
//********************************************************************

void MSOGetChunkHdrOffset
(
    DWORD               dwChunkOffset,
    LPDWORD             lpdwHdrOffset
)
{
    *lpdwHdrOffset = dwChunkOffset - sizeof(MSO_HDR_T);
}


//********************************************************************
//
// Function:
//  BOOL MSOGetChunkInfo()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//  dwChunkHdrOffset    Offset of chunk header
//  lpbyType            Ptr to BYTE for chunk type
//  lpdwSize            Ptr to DWORD for chunk size
//
// Description:
//  Reads the chunk header and returns the value of the chunk
//  type and size field.
//
//  Either or both of the type or size pointer may be NULL
//  if the corresponding value is not desired.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOGetChunkInfo
(
    LPMSO               lpstMSO,
    DWORD               dwChunkHdrOffset,
    LPBYTE              lpbyType,
    LPDWORD             lpdwSize
)
{
    MSO_HDR_T           stHdr;
    DWORD               dw;

    // Read the chunk header

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   dwChunkHdrOffset,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileRead(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   &stHdr,
                   sizeof(MSO_HDR_T),
                   &dw) != SS_STATUS_OK ||
        dw != sizeof(MSO_HDR_T))
        return(FALSE);

    // Get the chunk type

    if (lpbyType != NULL)
        *lpbyType = stHdr.byTypeMin;

    // Get the length of the chunk

    if (lpdwSize != NULL)
        *lpdwSize = DWENDIAN(stHdr.dwLength);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOFileCopyContent()
//
// Parameters:
//  lpvRootCookie       Root cookie
//  lpvSrcFile          Source file pointer
//  lpvDstFile          Destination file pointer
//  dwSrcOffset         Source of the content to shift
//  dwDstOffset         Destination for the content
//  dwNumBytes          Size of the content
//  bTruncate           TRUE if file should be truncated after
//                      destination end
//
// Description:
//  Copy the contents at the given offset of the given file
//  to the given destination.  If the given content size is
//  0xFFFFFFFF, the contents are from the given offset to the
//  end of the file.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOFileCopyContent
(
    LPVOID              lpvRootCookie,
    LPVOID              lpvSrcFile,
    LPVOID              lpvDstFile,
    DWORD               dwSrcOffset,
    DWORD               dwDstOffset,
    DWORD               dwNumBytes,
    BOOL                bTruncate
)
{
    BOOL                bForward;
    DWORD               dwEndOffset;
    DWORD               dwNumBytesToCopy;
    DWORD               dwNumBytesCopied;
    BYTE                abyWorkBuffer[512];

    if (dwNumBytes == 0xFFFFFFFF)
    {
        // Get the end offset

        if (SSFileSeek(lpvRootCookie,
                       lpvSrcFile,
                       0,
                       SS_SEEK_END,
                       (LPLONG)&dwEndOffset) != SS_STATUS_OK)
            return(FALSE);

        if (dwEndOffset < dwSrcOffset)
            return(FALSE);

        dwNumBytes = dwEndOffset - dwSrcOffset;
    }

    dwEndOffset = dwDstOffset + dwNumBytes;

    if (dwSrcOffset < dwDstOffset)
    {
        // Forward copy, so copy starting from end

        bForward = TRUE;

        dwSrcOffset = dwSrcOffset + dwNumBytes;
        dwDstOffset = dwDstOffset + dwNumBytes;
    }
    else
    {
        // Backward copy, so copy starting from beginning

        bForward = FALSE;
    }

    dwNumBytesToCopy = sizeof(abyWorkBuffer);
    while (dwNumBytes != 0)
    {
        if (dwNumBytes < dwNumBytesToCopy)
            dwNumBytesToCopy = dwNumBytes;

        // Update source and destination offsets for back copy

        if (bForward != FALSE)
        {
            dwSrcOffset -= dwNumBytesToCopy;
            dwDstOffset -= dwNumBytesToCopy;
        }

        // Read from source

        if (SSFileSeek(lpvRootCookie,
                       lpvSrcFile,
                       dwSrcOffset,
                       SS_SEEK_SET,
                       NULL) != SS_STATUS_OK)
            return(FALSE);

        if (SSFileRead(lpvRootCookie,
                       lpvSrcFile,
                       abyWorkBuffer,
                       dwNumBytesToCopy,
                       &dwNumBytesCopied) != SS_STATUS_OK ||
            dwNumBytesCopied != dwNumBytesToCopy)
            return(FALSE);

        // Write to destination

        if (SSFileSeek(lpvRootCookie,
                       lpvDstFile,
                       dwDstOffset,
                       SS_SEEK_SET,
                       NULL) != SS_STATUS_OK)
            return(FALSE);

        if (SSFileWrite(lpvRootCookie,
                        lpvDstFile,
                        abyWorkBuffer,
                        dwNumBytesToCopy,
                        &dwNumBytesCopied) != SS_STATUS_OK ||
            dwNumBytesCopied != dwNumBytesToCopy)
            return(FALSE);

        // Update source and destination offsets for forward copy

        if (bForward == FALSE)
        {
            dwSrcOffset += dwNumBytesToCopy;
            dwDstOffset += dwNumBytesToCopy;
        }

        dwNumBytes -= dwNumBytesToCopy;
    }

    if (bTruncate != FALSE)
    {
        // Truncate the file

        if (SSFileSeek(lpvRootCookie,
                       lpvDstFile,
                       dwEndOffset,
                       SS_SEEK_SET,
                       NULL) != SS_STATUS_OK)
            return(FALSE);

        if (SSFileTruncate(lpvRootCookie,
                           lpvDstFile) != SS_STATUS_OK)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOGetVBAOffsetSize()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//  dwChunkOffset       Offset of the start of the VBA chunk
//  dwChunkSize         Size of the chunk
//  lpdwSigOffset       Ptr to DWORD for digital sig offset
//  lpdwSigSize         Ptr to DWORD for digital sig size
//  lpdwVBAOffset       Ptr to DWORD for compressed VBA offset
//  lpdwVBASize         Ptr to DWORD for compressed VBA size
//
// Description:
//  The function assumes that the MSO structure is current.
//
//  The function then parses to the beginning of the compressed
//  VBA storage and stores the offset and size of the compressed
//  storage at *lpdwVBAOffset and *lpdwVBASize, respectively.
//
//  If lpdwSigOffset or lpdwSigSize or both are not NULL,
//  then the digital sig offset and/or size is returned also.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOGetVBAOffsetSize
(
    LPMSO               lpstMSO,
    DWORD               dwChunkOffset,
    DWORD               dwChunkSize,
    LPDWORD             lpdwSigOffset,
    LPDWORD             lpdwSigSize,
    LPDWORD             lpdwVBAOffset,
    LPDWORD             lpdwVBASize
)
{
    MSO_VBA_HDR_T       stHdr;
    DWORD               dw;

    // Validate that there are enough bytes for the VBA header

    if (dwChunkSize < sizeof(MSO_VBA_HDR_T))
        return(FALSE);

    // Read the VBA header

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   dwChunkOffset,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileRead(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   &stHdr,
                   sizeof(MSO_VBA_HDR_T),
                   &dw) != SS_STATUS_OK ||
        dw != sizeof(MSO_VBA_HDR_T))
        return(FALSE);

    dwChunkSize -= sizeof(MSO_VBA_HDR_T);

    // Validate the app specific size

    stHdr.dwAppSpecificSize = DWENDIAN(stHdr.dwAppSpecificSize);
    if (dwChunkSize < stHdr.dwAppSpecificSize)
        return(FALSE);

    dwChunkSize -= stHdr.dwAppSpecificSize;

    // Validate the digital signature size

    stHdr.dwDigiSigSize = DWENDIAN(stHdr.dwDigiSigSize);
    if (dwChunkSize < stHdr.dwDigiSigSize)
        return(FALSE);

    dwChunkSize -= stHdr.dwDigiSigSize;

    // Validate the presence of the size DWORD and the two-byte
    //  compression header

    if (dwChunkSize < 6)
        return(FALSE);

    // Store the digital signature info

    if (lpdwSigOffset != NULL)
        *lpdwSigOffset = dwChunkOffset + sizeof(MSO_VBA_HDR_T) +
        stHdr.dwAppSpecificSize;

    if (lpdwSigSize != NULL)
        *lpdwSigSize = stHdr.dwDigiSigSize;

    // Skip past the VBA header, app specific data, digital sig,
    //  inflated size DWORD, and two-byte compression header

    *lpdwVBAOffset = dwChunkOffset + sizeof(MSO_VBA_HDR_T) +
        stHdr.dwAppSpecificSize + stHdr.dwDigiSigSize +
        sizeof(DWORD) + 2;

    // dwChunkSize should be the size of the LZ77 data

    *lpdwVBASize = dwChunkSize - sizeof(DWORD) - 2;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOExtractVBA()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//  lpvDstFile          Destination file
//
// Description:
//  The function assumes that the MSO structure is current.
//  If the current chunk is not one of the VBA project types,
//  then the function returns FALSE.
//
//  The function then parses to the beginning of the compressed
//  VBA storage and extracts it to the given destination file
//  at its current offset.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOExtractVBA
(
    LPMSO               lpstMSO,
    LPVOID              lpvDstFile
)
{
    DWORD               dwOffset;
    DWORD               dwSize;
    LPIF                lpstIF;
    BOOL                bResult;

    // Verify that the chunk is one of the right types

    if (lpstMSO->nChunkType != MSO_TYPE_VBA_PROJECT_XL &&
        lpstMSO->nChunkType != MSO_TYPE_VBA_PROJECT_WD &&
        lpstMSO->nChunkType != MSO_TYPE_VBA_PROJECT_PPT)
        return(FALSE);

    // Get the compressed VBA offset and size

    if (MSOGetVBAOffsetSize(lpstMSO,
                            lpstMSO->dwChunkOffset,
                            lpstMSO->dwChunkSize,
                            NULL,
                            NULL,
                            &dwOffset,
                            &dwSize) == FALSE)
        return(FALSE);

    // Validate the destination file ptr

    if (lpvDstFile == NULL)
        return(FALSE);

    // Truncate the destination file to zero bytes

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpvDstFile,
                   0,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK ||
        SSFileTruncate(lpstMSO->lpvRootCookie,
                       lpvDstFile) != SS_STATUS_OK)
        return(FALSE);

    // Allocate the IF structure

    lpstIF = IFAllocIF(lpstMSO->lpvRootCookie);
    if (lpstIF == NULL)
        return(FALSE);

    // Initialize return value

    bResult = TRUE;

    // Initialize for inflation

    if (IFInit(lpstIF,
               lpstMSO->lpvFile,
               dwOffset,
               dwSize) == FALSE)
        bResult = FALSE;

    // Inflate

    if (bResult != FALSE &&
        IFInflate(lpstIF,lpvDstFile) == FALSE)
        bResult = FALSE;

    // Free the IF structure

    if (IFFreeIF(lpstIF) == FALSE)
        bResult = FALSE;

    return(bResult);
}


//********************************************************************
//
// Function:
//  BOOL MSOVBAIsSigned()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//  dwChunkHdrOffset    Offset of VBA chunk header
//  lpbSigned           Ptr to BOOL for result
//
// Description:
//  The function assumes that the MSO structure is current.
//  If the current chunk is not one of the VBA project types,
//  then the function returns FALSE.
//
//  The function then parses to the beginning of the compressed
//  VBA storage and extracts it to the given destination file
//  at its current offset.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOVBAIsSigned
(
    LPMSO               lpstMSO,
    DWORD               dwChunkHdrOffset,
    LPBOOL              lpbSigned
)
{
    BYTE                byType;
    DWORD               dwChunkSize;
    MSO_VBA_HDR_T       stHdr;
    DWORD               dw;

    // Get the type and size of the VBA chunk

    if (MSOGetChunkInfo(lpstMSO,
                        dwChunkHdrOffset,
                        &byType,
                        &dwChunkSize) == FALSE)
        return(FALSE);

    // Verify that the chunk is one of the right types

    if (byType != MSO_TYPE_VBA_PROJECT_XL &&
        byType != MSO_TYPE_VBA_PROJECT_WD &&
        byType != MSO_TYPE_VBA_PROJECT_PPT)
        return(FALSE);

    // Validate that there are enough bytes for the VBA header

    if (dwChunkSize < sizeof(MSO_VBA_HDR_T))
        return(FALSE);

    // Read the VBA header

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   dwChunkHdrOffset + sizeof(MSO_HDR_T),
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileRead(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   &stHdr,
                   sizeof(MSO_VBA_HDR_T),
                   &dw) != SS_STATUS_OK ||
        dw != sizeof(MSO_VBA_HDR_T))
        return(FALSE);

    // Is the digital signature of non-zero size?

    if (stHdr.dwDigiSigSize != 0)
    {
        *lpbSigned = TRUE;
        return(TRUE);
    }

    *lpbSigned = FALSE;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOExtractTDT()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//  lpvDstFile          Destination file
//
// Description:
//  The function assumes that the MSO structure is current.
//  If the current chunk is not the TDT type,
//  then the function returns FALSE.
//
//  The function then parses to the beginning of the TDT
//  and extracts it to the given destination file.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOExtractTDT
(
    LPMSO               lpstMSO,
    LPVOID              lpvDstFile
)
{
    MSO_WORD_TCG_HDR_T  stHdr;
    DWORD               dw;

    // Verify that the chunk is one of the right types

    if (lpstMSO->nChunkType != MSO_TYPE_WORD_TCG)
        return(FALSE);

    // Validate that there are enough bytes for the header

    if (lpstMSO->dwChunkSize < sizeof(MSO_WORD_TCG_HDR_T))
        return(FALSE);

    // Read the header

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   lpstMSO->dwChunkOffset,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileRead(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   &stHdr,
                   sizeof(MSO_WORD_TCG_HDR_T),
                   &dw) != SS_STATUS_OK ||
        dw != sizeof(MSO_WORD_TCG_HDR_T))
        return(FALSE);

    // Validate the TDT size

    stHdr.dwTDTSize = DWENDIAN(stHdr.dwTDTSize);
    if ((lpstMSO->dwChunkSize - sizeof(MSO_WORD_TCG_HDR_T)) <
        stHdr.dwTDTSize)
        return(FALSE);

    // Copy the TDT to the destination file

    if (lpvDstFile == NULL)
        return(FALSE);

    if (MSOFileCopyContent(lpstMSO->lpvRootCookie,
                           lpstMSO->lpvFile,
                           lpvDstFile,
                           lpstMSO->dwChunkOffset +
                               sizeof(MSO_WORD_TCG_HDR_T),
                           0,
                           stHdr.dwTDTSize,
                           TRUE) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOResizeChunk()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//  dwChunkHdrOffset    Offset of chunk header
//  dwSize              New chunk size
//  lpdwOffsetToAdjust  Ptr to DWORD offset to adjust
//
// Description:
//  Reads the chunk header and determines whether or not the chunk
//  needs to be expanded, shrunk, or just deleted.  The chunks
//  following the chunk to resize are relocated as well to
//  immediately follow the resized chunk.
//
//  If the new size is zero, the chunk is deleted.
//
//  If the offset to adjust follows the chunk header offset,
//  then it is either increased or decreased according to whether
//  the chunk is increased or decreased in size.
//
//  If the offset to adjust comes before the chunk header offset,
//  it is not modified.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOResizeChunk
(
    LPMSO               lpstMSO,
    DWORD               dwChunkHdrOffset,
    DWORD               dwSize,
    LPDWORD             lpdwOffsetToAdjust
)
{
    MSO_HDR_T           stHdr;
    DWORD               dw;

    ////////////////////////////////////////////////////////
    // Read the chunk header

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   dwChunkHdrOffset,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileRead(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   &stHdr,
                   sizeof(MSO_HDR_T),
                   &dw) != SS_STATUS_OK ||
        dw != sizeof(MSO_HDR_T))
        return(FALSE);

    // Get the length of the chunk

    stHdr.dwLength = DWENDIAN(stHdr.dwLength);

    ////////////////////////////////////////////////////////
    // Adjust the offset to adjust

    // Do this first, since we will be modifying
    //  the length field of the header

    if (lpdwOffsetToAdjust != NULL)
    {
        // Only adjust if the offset follows the chunk header offset

        if (*lpdwOffsetToAdjust >= dwChunkHdrOffset)
        {
            if (dwSize == 0)
            {
                // Decrease the offset by both the header
                //  size and the data size

                *lpdwOffsetToAdjust -=
                    sizeof(MSO_HDR_T) + stHdr.dwLength;
            }
            else
            if (dwSize < stHdr.dwLength)
            {
                // Decrease the offset

                *lpdwOffsetToAdjust -= stHdr.dwLength - dwSize;
            }
            else
            {
                // Increase the offset

                *lpdwOffsetToAdjust += dwSize - stHdr.dwLength;
            }
        }
    }

    ////////////////////////////////////////////////////////
    // Resize the chunk

    // If the new size is zero, just delete the chunk

    if (dwSize == 0)
    {
        if (MSOFileCopyContent(lpstMSO->lpvRootCookie,
                               lpstMSO->lpvFile,
                               lpstMSO->lpvFile,
                               dwChunkHdrOffset +
                                   sizeof(MSO_HDR_T) +
                                   stHdr.dwLength,
                               dwChunkHdrOffset,
                               0xFFFFFFFF,
                               TRUE) == FALSE)
            return(FALSE);
    }
    else
    {
        // Non-zero size

        // Adjust the location of the contents after the chunk

        if (MSOFileCopyContent(lpstMSO->lpvRootCookie,
                               lpstMSO->lpvFile,
                               lpstMSO->lpvFile,
                               dwChunkHdrOffset +
                                   sizeof(MSO_HDR_T) +
                                   stHdr.dwLength,
                               dwChunkHdrOffset +
                                   sizeof(MSO_HDR_T) + dwSize,
                               0xFFFFFFFF,
                               TRUE) == FALSE)
            return(FALSE);

        // Set the new size in the header

        stHdr.dwLength = DWENDIAN(dwSize);

        // Write out the new header

        if (SSFileSeek(lpstMSO->lpvRootCookie,
                       lpstMSO->lpvFile,
                       dwChunkHdrOffset,
                       SS_SEEK_SET,
                       NULL) != SS_STATUS_OK)
            return(FALSE);

        if (SSFileWrite(lpstMSO->lpvRootCookie,
                        lpstMSO->lpvFile,
                        &stHdr,
                        sizeof(MSO_HDR_T),
                        &dw) != SS_STATUS_OK ||
            dw != sizeof(MSO_HDR_T))
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOComputeAdlerCRC()
//
// Parameters:
//  lpvRootCookie       Root cookie ptr
//  lpvFile             File ptr
//  lpabyCRC            Ptr to four byte array for big endian CRC
//
// Description:
//  The function computes the Adler CRC for the given file
//  and stores the CRC in lpabyCRC in big endian order.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOComputeAdlerCRC
(
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile,
    LPBYTE              lpabyCRC
)
{
    DWORD               dwSize;
    DWORD               dwCRC;
    DWORD               dwCount;
    DWORD               dwBytesRead;
    BYTE                abyBuf[512];

    // Get the size of the file

    if (SSFileSeek(lpvRootCookie,
                   lpvFile,
                   0,
                   SS_SEEK_END,
                   (LPLONG)&dwSize) != SS_STATUS_OK)
        return(FALSE);

    // Start at the beginning

    if (SSFileSeek(lpvRootCookie,
                   lpvFile,
                   0,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    // Iterate

    dwCRC = ADLER_CRC_SEED;
    dwCount = sizeof(abyBuf);
    while (dwSize != 0)
    {
        if (dwSize < dwCount)
            dwCount = dwSize;

        // Read a chunk

        if (SSFileRead(lpvRootCookie,
                       lpvFile,
                       abyBuf,
                       dwCount,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwCount)
            return(FALSE);

        // Continue the CRC on the chunk

        dwCRC = AdlerCRC32(dwCRC,abyBuf,dwCount);

        // Update the number of bytes left

        dwSize -= dwCount;
    }

    lpabyCRC[0] = (BYTE)((dwCRC >> 24) & 0xFF);
    lpabyCRC[1] = (BYTE)((dwCRC >> 16) & 0xFF);
    lpabyCRC[2] = (BYTE)((dwCRC >> 8) & 0xFF);
    lpabyCRC[3] = (BYTE)(dwCRC & 0xFF);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOIntegrateVBA()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//  dwChunkHdrOffset    Offset of VBA chunk header
//  lpvVBAFile          Ptr to VBA file to integrate
//  lpvLZ77File         Ptr to destination file for LZ77 raw binary
//  lpdwOffsetToAdjust  Partner offset to adjust
//
// Description:
//  The function assumes that the MSO structure is current.
//  The function compresses the VBA file to the LZ77 file
//  and then integrates the result into the MSO file, updating
//  the structure and the file appropriately.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOIntegrateVBA
(
    LPMSO               lpstMSO,
    DWORD               dwChunkHdrOffset,
    LPVOID              lpvVBAFile,
    LPVOID              lpvLZ77File,
    LPDWORD             lpdwOffsetToAdjust
)
{
    BYTE                byType;
    DWORD               dwChunkSize;
    DWORD               dwSigOffset;
    DWORD               dwSigSize;
    DWORD               dwVBALZ77Offset;
    DWORD               dwCurVBALZ77Size;
    DWORD               dwNewVBASize;
    DWORD               dwNewVBALZ77Size;
    LPDF                lpstDF;
    BYTE                abyCRC[4];

    // Validate parameters

    if (lpvVBAFile == NULL ||
        lpvLZ77File == NULL)
        return(FALSE);

    // Make sure the LZ77 file has zero length

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpvLZ77File,
                   0,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileTruncate(lpstMSO->lpvRootCookie,
                       lpvLZ77File) != SS_STATUS_OK)
        return(FALSE);

    // Get the type and size of the VBA chunk

    if (MSOGetChunkInfo(lpstMSO,
                        dwChunkHdrOffset,
                        &byType,
                        &dwChunkSize) == FALSE)
        return(FALSE);

    // Get the size of the new VBA

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpvVBAFile,
                   0,
                   SS_SEEK_END,
                   (LPLONG)&dwNewVBASize) != SS_STATUS_OK)
        return(FALSE);

    // If the size is zero, just delete the chunk

    if (dwNewVBASize == 0)
    {
        // Remove the chunk

        if (MSOResizeChunk(lpstMSO,
                           dwChunkHdrOffset,
                           0,
                           lpdwOffsetToAdjust) == FALSE)
            return(FALSE);

        return(TRUE);
    }

    ////////////////////////////////////////////////////////
    // Compress VBA to LZ77

    // Allocate DF structure

    lpstDF = DFAllocDF(lpstMSO->lpvRootCookie);
    if (lpstDF == NULL)
        return(FALSE);

    // Initialize for deflation

    if (DFInit(lpstDF,
               lpvLZ77File,
               dwNewVBASize) == FALSE)
    {
        DFFreeDF(lpstDF);
        return(FALSE);
    }

    // Deflate

    if (DFDeflate(lpstDF,
                  lpvVBAFile) == FALSE)
    {
        DFFreeDF(lpstDF);
        return(FALSE);
    }

    // Free the DF structure

    DFFreeDF(lpstDF);

    // Get the size of the new VBA LZ77

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpvLZ77File,
                   0,
                   SS_SEEK_END,
                   (LPLONG)&dwNewVBALZ77Size) != SS_STATUS_OK)
        return(FALSE);


    ////////////////////////////////////////////////////////
    // Integrate new LZ77

    // Get the compressed VBA offset and size

    if (MSOGetVBAOffsetSize(lpstMSO,
                            dwChunkHdrOffset + sizeof(MSO_HDR_T),
                            dwChunkSize,
                            &dwSigOffset,
                            &dwSigSize,
                            &dwVBALZ77Offset,
                            &dwCurVBALZ77Size) == FALSE)
        return(FALSE);

    // Calculate the new size of the chunk

    if (dwNewVBALZ77Size < dwCurVBALZ77Size)
        dwChunkSize -= dwCurVBALZ77Size - dwNewVBALZ77Size;
    else
        dwChunkSize += dwNewVBALZ77Size - dwCurVBALZ77Size;

    // Adjust for the digital signature too

    if (dwSigSize != 0)
    {
        DWORD           dw;

        // Copy the size and LZ77 header over the digital signature

        if (MSOFileCopyContent(lpstMSO->lpvRootCookie,
                               lpstMSO->lpvFile,
                               lpstMSO->lpvFile,
                               dwVBALZ77Offset - 6,
                               dwSigOffset,
                               6,
                               FALSE) == FALSE)
            return(FALSE);

        // Decrement the chunk size by the size of the digital
        //  signature

        dwChunkSize -= dwSigSize;

        // Adjust the VBA LZ77 offset

        dwVBALZ77Offset -= dwSigSize;

        // Set the signature size to zero

        dwSigSize = 0;
        if (SSFileSeek(lpstMSO->lpvRootCookie,
                       lpstMSO->lpvFile,
                       dwChunkHdrOffset + sizeof(MSO_HDR_T) +
                            sizeof(MSO_VBA_HDR_T) - sizeof(DWORD),
                       SS_SEEK_SET,
                       NULL) != SS_STATUS_OK)
            return(FALSE);

        if (SSFileWrite(lpstMSO->lpvRootCookie,
                        lpstMSO->lpvFile,
                        &dwSigSize,
                        sizeof(DWORD),
                        &dw) != SS_STATUS_OK ||
            dw != sizeof(DWORD))
            return(FALSE);
    }

    // Add four to the size for the CRC?

    dwChunkSize += 4;

    // Resize the chunk first

    if (MSOResizeChunk(lpstMSO,
                       dwChunkHdrOffset,
                       dwChunkSize,
                       lpdwOffsetToAdjust) == FALSE)
        return(FALSE);

    // Copy the LZ77

    if (MSOFileCopyContent(lpstMSO->lpvRootCookie,
                           lpvLZ77File,
                           lpstMSO->lpvFile,
                           0,
                           dwVBALZ77Offset,
                           0xFFFFFFFF,
                           FALSE) == FALSE)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Update the CRC

    // Compute the CRC

    if (MSOComputeAdlerCRC(lpstMSO->lpvRootCookie,
                           lpvVBAFile,
                           abyCRC) == FALSE)
        return(FALSE);

    // Write the new CRC

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   dwVBALZ77Offset + dwNewVBALZ77Size,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileWrite(lpstMSO->lpvRootCookie,
                    lpstMSO->lpvFile,
                    abyCRC,
                    4,
                    &dwChunkSize) != SS_STATUS_OK ||
        dwChunkSize != 4)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOIntegrateTDT()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//  lpabyWorkBuf        Ptr to work buffer
//  dwWorkBufSize       Size of work buffer
//  lpvTDTFile          Ptr to TDT file to integrate
//
// Description:
//  The function assumes that the MSO structure is current.
//  The function integrates the contents of the given file into
//  the TDT section of the MSO file, updating the structure and
//  the file appropriately.
//
//  If the new TDT file has a size of zero, the function removes
//  the current TDT, if it exists, from the MSO file.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOIntegrateTDT
(
    LPMSO               lpstMSO,
    DWORD               dwChunkHdrOffset,
    LPVOID              lpvTDTFile,
    LPDWORD             lpdwOffsetToAdjust
)
{
    DWORD               dwChunkSize;
    MSO_WORD_TCG_HDR_T  stHdr;
    DWORD               dw;
    DWORD               dwNewTDTSize;

    // Validate parameters

    if (lpvTDTFile == NULL)
        return(FALSE);

    // Get the size of the new TDT

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpvTDTFile,
                   0,
                   SS_SEEK_END,
                   (LPLONG)&dwNewTDTSize) != SS_STATUS_OK)
        return(FALSE);

    // If the size is <= 2, just delete the chunk

    if (dwNewTDTSize <= 2)
    {
        if (MSOResizeChunk(lpstMSO,
                           dwChunkHdrOffset,
                           0,
                           lpdwOffsetToAdjust) == FALSE)
            return(FALSE);

        return(TRUE);
    }

    // Get the size of the TDT chunk

    if (MSOGetChunkInfo(lpstMSO,
                        dwChunkHdrOffset,
                        NULL,
                        &dwChunkSize) == FALSE)
        return(FALSE);

    // Validate that there are enough bytes for the header

    if (dwChunkSize < sizeof(MSO_WORD_TCG_HDR_T))
        return(FALSE);

    // Read the header

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   dwChunkHdrOffset + sizeof(MSO_HDR_T),
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileRead(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   &stHdr,
                   sizeof(MSO_WORD_TCG_HDR_T),
                   &dw) != SS_STATUS_OK ||
        dw != sizeof(MSO_WORD_TCG_HDR_T))
        return(FALSE);

    // Validate the TDT size

    stHdr.dwTDTSize = DWENDIAN(stHdr.dwTDTSize);
    if (stHdr.dwTDTSize != dwChunkSize - sizeof(MSO_WORD_TCG_HDR_T))
    {
        // Since we don't know the meaning of all the fields

        return(FALSE);
    }

    ////////////////////////////////////////////////////////
    // Integrate new TDT

    // Calculate the new size of the chunk

    if (dwNewTDTSize < stHdr.dwTDTSize)
        dwChunkSize -= stHdr.dwTDTSize - dwNewTDTSize;
    else
        dwChunkSize += dwNewTDTSize - stHdr.dwTDTSize;

    // Resize the chunk first

    if (MSOResizeChunk(lpstMSO,
                       dwChunkHdrOffset,
                       dwChunkSize,
                       lpdwOffsetToAdjust) == FALSE)
        return(FALSE);

    // Copy the new TDT

    if (MSOFileCopyContent(lpstMSO->lpvRootCookie,
                           lpvTDTFile,
                           lpstMSO->lpvFile,
                           0,
                           dwChunkHdrOffset + sizeof(MSO_HDR_T) +
                               sizeof(MSO_WORD_TCG_HDR_T),
                           0xFFFFFFFF,
                           FALSE) == FALSE)
        return(FALSE);

    // Update the TCG header TDT size field

    stHdr.dwTDTSize = DWENDIAN(dwNewTDTSize);

    // Write the updated header

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   dwChunkHdrOffset + sizeof(MSO_HDR_T),
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileWrite(lpstMSO->lpvRootCookie,
                    lpstMSO->lpvFile,
                    &stHdr,
                    sizeof(MSO_WORD_TCG_HDR_T),
                    &dw) != SS_STATUS_OK ||
        dw != sizeof(MSO_WORD_TCG_HDR_T))
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL MSOTruncateIfEmpty()
//
// Parameters:
//  lpstMSO             Pointer to initialized MSO structure
//
// Description:
//  The function iterates through the chunks of the MSO file.
//  If consists only of the header chunk and the end block chunk,
//  both of size four, then the function truncates the file to zero
//  length and returns success.
//
//  If there is any other chunk type, the function leaves the
//  file alone and returns success.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL MSOTruncateIfEmpty
(
    LPMSO               lpstMSO
)
{
    // Get the first chunk

    if (MSOGetFirstChunk(lpstMSO) == FALSE)
        return(FALSE);

    if (lpstMSO->nChunkType != MSO_TYPE_FILE_HEADER)
        return(TRUE);

    // Get the next chunk

    if (MSOGetNextChunk(lpstMSO) == FALSE)
        return(FALSE);

    if (lpstMSO->nChunkType != MSO_TYPE_END_BLOCK)
        return(TRUE);

    // There should be no next chunk

    if (MSOGetNextChunk(lpstMSO) != FALSE)
        return(FALSE);

    // The MSO file has been verified to consist of only two chunks,
    //  the header and the end block chunks.

    // Truncate the file to zero length

    if (SSFileSeek(lpstMSO->lpvRootCookie,
                   lpstMSO->lpvFile,
                   0,
                   SS_SEEK_SET,
                   NULL) != SS_STATUS_OK)
        return(FALSE);

    if (SSFileTruncate(lpstMSO->lpvRootCookie,
                       lpstMSO->lpvFile) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}

