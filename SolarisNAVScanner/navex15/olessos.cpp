//************************************************************************
//
// $Header:   S:/NAVEX/VCS/olessos.cpv   1.0   09 Jul 1997 16:24:44   DCHI  $
//
// Description:
//  OLE structured storage callback implementation.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/olessos.cpv  $
// 
//    Rev 1.0   09 Jul 1997 16:24:44   DCHI
// Initial revision.
// 
//************************************************************************

#include "olessapi.h"

#include "platform.h"
#include "callback.h"

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
#ifdef SYM_WIN32
    (void)lpvRootCookie;

    *lplpvBuffer = (LPBYTE)GlobalAlloc(GMEM_FIXED,
                                       dwNumBytes);
#else
    *lplpvBuffer = ((LPCALLBACKREV1)lpvRootCookie)->
        PermMemoryAlloc(dwNumBytes);
#endif
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
#ifdef SYM_WIN32
    (void)lpvRootCookie;

    GlobalFree(lpvBuffer);
#else
    ((LPCALLBACKREV1)lpvRootCookie)->PermMemoryFree(lpvBuffer);
#endif

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
    long        lNewOffset;

    switch (nWhence)
    {
        case SS_SEEK_SET:
            nWhence = 0;
            break;

        case SS_SEEK_CUR:
            nWhence = 1;
            break;

        case SS_SEEK_END:
            nWhence = 2;
            break;

        default:
            return(SS_STATUS_ERROR);
    }

    lNewOffset = ((LPCALLBACKREV1)lpvRootCookie)->
        FileSeek((HFILE)lpvFile,
                 lOffset,
                 nWhence);

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
    if (((LPCALLBACKREV1)lpvRootCookie)->
        FileRead((HFILE)lpvFile,
                 lpvBuffer,
                 dwNumBytes) != dwNumBytes)
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
    if (((LPCALLBACKREV1)lpvRootCookie)->
        FileWrite((HFILE)lpvFile,
                  lpvBuffer,
                  dwNumBytes) != dwNumBytes)
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
    if (((LPCALLBACKREV1)lpvRootCookie)->
        FileWrite((HFILE)lpvFile,
                  NULL,
                  0) == (UINT)-1)
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
    ((LPCALLBACKREV1)lpvRootCookie)->Progress(0,0);

    return(SS_STATUS_OK);
}
#endif

