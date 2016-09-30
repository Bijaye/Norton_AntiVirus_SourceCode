//************************************************************************
//
// $Header:   S:/SDSTRIP/VCS/OLESSOS.CPv   1.0   31 Mar 1998 16:00:16   DCHI  $
//
// Description:
//  OLE structured storage callback implementation.
//
//************************************************************************
// $Log:   S:/SDSTRIP/VCS/OLESSOS.CPv  $
// 
//    Rev 1.0   31 Mar 1998 16:00:16   DCHI
// Initial revision.
// 
//************************************************************************

#include "olessapi.h"

#include "platform.h"

//********************************************************************
//
// Function:
//  SS_STATUS SSMemoryAlloc()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  dwNumBytes          Number of bytes to allocate
//  lplpvBuffer         Ptr to ptr to store allocated memory ptr
//
// Description:
//  Allocates dwNumBytes of memory.  The ptr to the allocated memory
//  is stored at *lplpvBuffer.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSMemoryAlloc
(
    LPVOID      lpvRootCookie,
    DWORD       dwNumBytes,
    LPLPVOID    lplpvBuffer
)
{
    (void)lpvRootCookie;

    *lplpvBuffer = (LPBYTE)GlobalAlloc(GMEM_FIXED,
                                       dwNumBytes);

    if (*lplpvBuffer == NULL)
        return(SS_STATUS_ERROR);

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSMemoryFree()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  lpvBuffer           Ptr to the memory buffer to free
//
// Description:
//  Frees the given memory that was allocated using SSMemoryAlloc().
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSMemoryFree
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvBuffer
)
{
    (void)lpvRootCookie;

    GlobalFree(lpvBuffer);

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSFileSeek()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  lpvFile             Handle to the file
//  lOffset             Relative offset to seek to
//  nWhence             The reference point of the seek
//  lplNewOffset        Ptr to long to store offset after seek
//
// Description:
//  Seeks to the given offset relative to the given reference point
//  which may be one of the following:
//
//      SS_SEEK_SET     Relative to the beginning of the file
//      SS_SEEK_CUR     Relative to the current position of the file
//      SS_SEEK_END     Relative to the end of the file
//
//  lplNewOffset can be NULL, in which case the function does not
//  store the new offset.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSFileSeek
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    long        lOffset,
    int         nWhence,
    LPLONG      lplNewOffset
)
{
    DWORD       dwMoveMethod;
    long        lNewOffset;

    (void)lpvRootCookie;

    switch (nWhence)
    {
        case SS_SEEK_SET:
            dwMoveMethod = FILE_BEGIN;
            break;

        case SS_SEEK_CUR:
            dwMoveMethod = FILE_CURRENT;
            break;

        case SS_SEEK_END:
            dwMoveMethod = FILE_END;
            break;

        default:
            return(SS_STATUS_ERROR);
    }

    lNewOffset = SetFilePointer((HANDLE)lpvFile,
                                lOffset,
                                NULL,
                                dwMoveMethod);

    if (lNewOffset == -1)
        return(SS_STATUS_ERROR);

    if (lplNewOffset != NULL)
        *lplNewOffset = lNewOffset;

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSFileRead()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  lpvFile             Handle to the file
//  lpvBuffer           Destination buffer
//  dwNumBytes          Number of bytes to read
//  lpdwBytesRead       Ptr to DWORD to store number of bytes read
//
// Description:
//  Reads dwNumBytes at the current position.  The function sets
//  *lpdwBytesRead to the number of bytes read on success.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSFileRead
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwBytesRead
)
{
    (void)lpvRootCookie;

    if (ReadFile((HANDLE)lpvFile,
                 lpvBuffer,
                 dwNumBytes,
                 &dwNumBytes,
                 NULL) == FALSE)
        return(SS_STATUS_ERROR);

    if (lpdwBytesRead != NULL)
        *lpdwBytesRead = dwNumBytes;

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSFileWrite()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  lpvFile             Handle to the file
//  lpvBuffer           Source buffer
//  dwNumBytes          Number of bytes to write
//  lpdwBytesWritten    Ptr to DWORD to store number of bytes written
//
// Description:
//  Writes dwNumBytes at the current position.  The function sets
//  *lpdwBytesWritten to the number of bytes written on success.
//
//  The function should not truncate the file if the number of bytes
//  to write is zero.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSFileWrite
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwBytesWritten
)
{
    (void)lpvRootCookie;

    if (WriteFile((HANDLE)lpvFile,
                  lpvBuffer,
                  dwNumBytes,
                  &dwNumBytes,
                  NULL) == FALSE)
        return(SS_STATUS_ERROR);

    if (lpdwBytesWritten != NULL)
        *lpdwBytesWritten = dwNumBytes;

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSFileTruncate()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  lpvFile             Handle to the file
//
// Description:
//  Truncates the file at the current offset.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

SS_STATUS SSFileTruncate
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile
)
{
    (void)lpvRootCookie;

    if (SetEndOfFile((HANDLE)lpvFile) == FALSE)
        return(SS_STATUS_ERROR);

    return(SS_STATUS_OK);
}


//********************************************************************
//
// Function:
//  SS_STATUS SSProgress()
//
// Parameters:
//  lpvRootCookie       The root cookie
//
// Description:
//  Calls the Progress callback.
//
// Returns:
//  SS_STATUS_OK        On success
//  SS_STATUS_ERROR     On error
//
//********************************************************************

#ifdef SYM_NLM
SS_STATUS SSProgress
(
    LPVOID      lpvRootCookie
)
{
    (void)lpvRootCookie;

    return(SS_STATUS_OK);
}
#endif

