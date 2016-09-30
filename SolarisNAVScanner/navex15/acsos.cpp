//************************************************************************
//
// $Header:   S:/NAVEX/VCS/acsos.cpv   1.0   17 Apr 1998 13:42:50   DCHI  $
//
// Description:
//  Access API callback implementation.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/acsos.cpv  $
// 
//    Rev 1.0   17 Apr 1998 13:42:50   DCHI
// Initial revision.
// 
//************************************************************************

#include <stdio.h>
#include <stdlib.h>

#include "platform.h"
#include "acsapi.h"
#include "callback.h"

//********************************************************************
//
// Function:
//  BOOL ACSMemoryAlloc()
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
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL ACSMemoryAlloc
(
    LPVOID      lpvRootCookie,
    DWORD       dwNumBytes,
    LPVOID FAR *lplpvBuffer
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
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL ACSMemoryFree()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  lpvBuffer           Ptr to the memory buffer to free
//
// Description:
//  Frees the given memory that was allocated using SSMemoryAlloc().
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL ACSMemoryFree
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

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL ACSFileSeek()
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
//      ACS_SEEK_SET    Relative to the beginning of the file
//      ACS_SEEK_CUR    Relative to the current position of the file
//      ACS_SEEK_END    Relative to the end of the file
//
//  lplNewOffset can be NULL, in which case the function does not
//  store the new offset.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL ACSFileSeek
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
        case ACS_SEEK_SET:
            nWhence = 0;
            break;

        case ACS_SEEK_CUR:
            nWhence = 1;
            break;

        case ACS_SEEK_END:
            nWhence = 2;
            break;

        default:
            return(FALSE);
    }

    lNewOffset = ((LPCALLBACKREV1)lpvRootCookie)->
        FileSeek((HFILE)lpvFile,
                 lOffset,
                 nWhence);

    if (lNewOffset == -1)
        return(FALSE);

    if (lplNewOffset != NULL)
        *lplNewOffset = lNewOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL ACSFileRead()
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
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL ACSFileRead
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
        return(FALSE);

    if (lpdwBytesRead != NULL)
        *lpdwBytesRead = dwNumBytes;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL ACSFileWrite()
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
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL ACSFileWrite
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
        return(FALSE);

    if (lpdwBytesWritten != NULL)
        *lpdwBytesWritten = dwNumBytes;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL ACSFileTruncate()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  lpvFile             Handle to the file
//
// Description:
//  Truncates the file at the current offset.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL ACSFileTruncate
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile
)
{
    if (((LPCALLBACKREV1)lpvRootCookie)->
        FileWrite((HFILE)lpvFile,
                  NULL,
                  0) == (UINT)-1)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL ACSProgress()
//
// Parameters:
//  lpvRootCookie       The root cookie
//
// Description:
//  Calls the Progress callback.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

#ifdef SYM_NLM
BOOL ACSProgress
(
    LPVOID      lpvRootCookie
)
{
    ((LPCALLBACKREV1)lpvRootCookie)->Progress(0,0);

    return(TRUE);
}
#endif

