//************************************************************************
//
// $Header:   S:/NAVEX/VCS/dfos.cpv   1.2   24 Dec 1998 05:27:04   relniti  $
//
// Description:
//  DF callback implementation for NAVEX.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/dfos.cpv  $
// 
//    Rev 1.2   24 Dec 1998 05:27:04   relniti
// changes for W32.RemoteExplore repair
// 
//    Rev 1.1   15 Dec 1998 12:14:48   DCHI
// - Changed PP_SCAN #ifdef to NAVEX15 or SYM_WIN #ifdef.
// - Added progress call.
// 
//    Rev 1.0   26 Nov 1997 18:37:20   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#include "ssbstype.h"
#include "df.h"
#include "callback.h"

#include "remotex.h"



//********************************************************************
//
// Function:
//  DF_STATUS DFMemoryAlloc()
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
//  DF_STATUS_OK        On success
//  DF_STATUS_ERROR     On error
//
//********************************************************************

DF_STATUS DFMemoryAlloc
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
        return(DF_STATUS_ERROR);

    return(DF_STATUS_OK);
}


//********************************************************************
//
// Function:
//  DF_STATUS DFMemoryFree()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  lpvBuffer           Ptr to the memory buffer to free
//
// Description:
//  Frees the given memory that was allocated using DFMemoryAlloc().
//
// Returns:
//  DF_STATUS_OK        On success
//  DF_STATUS_ERROR     On error
//
//********************************************************************

DF_STATUS DFMemoryFree
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

    return(DF_STATUS_OK);
}


//********************************************************************
//
// Function:
//  DF_STATUS DFFileSeek()
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
//      DF_SEEK_SET     Relative to the beginning of the file
//      DF_SEEK_CUR     Relative to the current position of the file
//      DF_SEEK_END     Relative to the end of the file
//
//  lplNewOffset can be NULL, in which case the function does not
//  store the new offset.
//
// Returns:
//  DF_STATUS_OK        On success
//  DF_STATUS_ERROR     On error
//
//********************************************************************

DF_STATUS DFFileSeek
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    long        lOffset,
    int         nWhence,
    LPLONG      lplNewOffset
)
{
    if (((LPCALLBACKREV1)lpvRootCookie)->FileOpen != NULL)
    {
        long    lNewOffset;

        // Normal

        switch (nWhence)
        {
            case DF_SEEK_SET:
                nWhence = 0;
                break;

            case DF_SEEK_CUR:
                nWhence = 1;
                break;

            case DF_SEEK_END:
                nWhence = 2;
                break;

            default:
                return(DF_STATUS_ERROR);
        }

        lNewOffset = ((LPCALLBACKREV1)lpvRootCookie)->
            FileSeek((HFILE)lpvFile,
                     lOffset,
                     nWhence);

        if (lNewOffset == -1)
            return(DF_STATUS_ERROR);

        if (lplNewOffset != NULL)
            *lplNewOffset = lNewOffset;
    }
    else
    {
        LPREMOTEXCB     lpstRemoteXCB;

        // Inflate to same file

        lpstRemoteXCB = (LPREMOTEXCB)(((LPCALLBACKREV1)lpvRootCookie) + 1);

        if (0 == (int)lpvFile)
        {
            // Source

            switch (nWhence)
            {
                case DF_SEEK_SET:
                    lpstRemoteXCB->dwSrcCurOffset =
                        lpstRemoteXCB->dwSrcBaseOffset + lOffset;
                    break;

                case DF_SEEK_CUR:
                    lpstRemoteXCB->dwSrcCurOffset -= lOffset;
                    break;

                case DF_SEEK_END:
                default:
                    return(DF_STATUS_ERROR);
            }

            if (lplNewOffset != NULL)
                *lplNewOffset = lpstRemoteXCB->dwSrcCurOffset;
        }
        else
        {
            // Destination

            switch (nWhence)
            {
                case DF_SEEK_SET:
                    lpstRemoteXCB->dwDstCurOffset =
                        lpstRemoteXCB->dwDstBaseOffset + lOffset;
                    break;

                case DF_SEEK_CUR:
                    lpstRemoteXCB->dwDstCurOffset -= lOffset;
                    break;

                case DF_SEEK_END:
                default:
                    return(DF_STATUS_ERROR);
            }

            if (lplNewOffset != NULL)
                *lplNewOffset = lpstRemoteXCB->dwDstCurOffset;
        }
    }

    return(DF_STATUS_OK);
}


//********************************************************************
//
// Function:
//  DF_STATUS DFFileRead()
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
//  DF_STATUS_OK        On success
//  DF_STATUS_ERROR     On error
//
//********************************************************************

DF_STATUS DFFileRead
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwBytesRead
)
{
    HFILE       hFile;

    if (((LPCALLBACKREV1)lpvRootCookie)->FileOpen == NULL)
    {
        DWORD       dwOffset;
        long        lNewOffset;
        LPREMOTEXCB lpstRemoteXCB;

        // Inflate to same file

        lpstRemoteXCB = (LPREMOTEXCB)(((LPCALLBACKREV1)lpvRootCookie) + 1);

        if (0 == (int)lpvFile)
        {
            // Source

            dwOffset = lpstRemoteXCB->dwSrcCurOffset;
    
            // Also update the offset

            lpstRemoteXCB->dwSrcCurOffset += dwNumBytes;
        }
        else
        {
            // Destination

            dwOffset = lpstRemoteXCB->dwDstCurOffset;
    
            // Also update the offset

            lpstRemoteXCB->dwDstCurOffset += dwNumBytes;
        }

        // Do the seek

        lNewOffset = ((LPCALLBACKREV1)lpvRootCookie)->
            FileSeek(lpstRemoteXCB->hFile,
                     dwOffset,
                     0);

        if (lNewOffset == -1)
            return(DF_STATUS_ERROR);

        hFile = lpstRemoteXCB->hFile;
    }
    else
        hFile = (HFILE)lpvFile;

    if (((LPCALLBACKREV1)lpvRootCookie)->
            FileRead(hFile,
                     lpvBuffer,
                     dwNumBytes) != dwNumBytes)
        return(DF_STATUS_ERROR);

    if (lpdwBytesRead != NULL)
        *lpdwBytesRead = dwNumBytes;

    return(DF_STATUS_OK);
}


//********************************************************************
//
// Function:
//  DF_STATUS DFFileWrite()
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
//  DF_STATUS_OK        On success
//  DF_STATUS_ERROR     On error
//
//********************************************************************

DF_STATUS DFFileWrite
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwBytesWritten
)
{
    HFILE       hFile;

    if (((LPCALLBACKREV1)lpvRootCookie)->FileOpen == NULL)
    {
        DWORD       dwOffset;
        long        lNewOffset;
        LPREMOTEXCB lpstRemoteXCB;

        // Inflate to same file

        lpstRemoteXCB = (LPREMOTEXCB)(((LPCALLBACKREV1)lpvRootCookie) + 1);

        if (0 == (int)lpvFile)
        {
            // Source

            dwOffset = lpstRemoteXCB->dwSrcCurOffset;
    
            // Also update the offset

            lpstRemoteXCB->dwSrcCurOffset += dwNumBytes;
        }
        else
        {
            // Destination

            dwOffset = lpstRemoteXCB->dwDstCurOffset;
    
            // Also update the offset

            lpstRemoteXCB->dwDstCurOffset += dwNumBytes;
        }

        // Do the seek

        lNewOffset = ((LPCALLBACKREV1)lpvRootCookie)->
            FileSeek(lpstRemoteXCB->hFile,
                     dwOffset,
                     0);

        if (lNewOffset == -1)
            return(DF_STATUS_ERROR);

        hFile = lpstRemoteXCB->hFile;
    }
    else
        hFile = (HFILE)lpvFile;

    if (((LPCALLBACKREV1)lpvRootCookie)->
        FileWrite(hFile,
                  lpvBuffer,
                  dwNumBytes) != dwNumBytes)
        return(DF_STATUS_ERROR);

    if (lpdwBytesWritten != NULL)
        *lpdwBytesWritten = dwNumBytes;

    return(DF_STATUS_OK);
}


//********************************************************************
//
// Function:
//  DF_STATUS DFFileTruncate()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  lpvFile             Handle to the file
//
// Description:
//  Truncates the file at the current offset.
//
// Returns:
//  DF_STATUS_OK        On success
//  DF_STATUS_ERROR     On error
//
//********************************************************************

DF_STATUS DFFileTruncate
(
    LPVOID      lpvRootCookie,
    LPVOID      lpvFile
)
{
    (void)lpvRootCookie;
    (void)lpvFile;

    if (((LPCALLBACKREV1)lpvRootCookie)->
        FileWrite((HFILE)lpvFile,
                  NULL,
                  0) == (UINT)-1)
        return(DF_STATUS_ERROR);

    return(DF_STATUS_OK);
}


//********************************************************************
//
// Function:
//  DF_STATUS DFProgress()
//
// Parameters:
//  lpvRootCookie       The root cookie
//  dwValue             The progress value
//
// Description:
//  Callled to update the progress.
//
// Returns:
//  DF_STATUS_OK        On success
//  DF_STATUS_ERROR     On error
//
//********************************************************************

DF_STATUS DFProgress
(
    LPVOID      lpvRootCookie,
    DWORD       dwValue
)
{
    (void)lpvRootCookie;
    (void)dwValue;

#ifdef SYM_NLM
    ((LPCALLBACKREV1)lpvRootCookie)->Progress(0,0);
#endif

    return(DF_STATUS_OK);
}


