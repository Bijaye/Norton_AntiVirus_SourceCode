// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CERTLIBN/VCS/fkillc.cpv   1.6   06 Feb 1996 15:25:20   RSTANEV  $
//
// Description:
//      Code to wipe and delete a file.
//
// Contains:
//      FIL4_KILL()
//
// See Also:
//************************************************************************
// $Log:   S:/CERTLIBN/VCS/fkillc.cpv  $
// 
//    Rev 1.6   06 Feb 1996 15:25:20   RSTANEV
// Changed the prototype of FIL4_KILL() to accept LPVOID as buffer.
//
//    Rev 1.5   05 Feb 1996 16:22:52   RSTANEV
// Fixed the prototype to match CTSN.H.
//
//    Rev 1.4   02 Feb 1996 13:50:30   RSTANEV
// Added Unicode support.
//
//    Rev 1.3   20 Dec 1994 14:30:28   JMILLARD
//
// fix SYM_NLM problem
//
//    Rev 1.2   15 Dec 1994 20:46:40   DALLEE
// Changed to use CERTLIBN's callbacks instead of QUAKE IO.
// And added file and function headers accidentally left off in r1.0
//************************************************************************

#include "platform.h"
#include "ctsn.h"

#ifdef __cplusplus
extern "C" {
#endif

// this overrides the forced convention there are no _ in front of names
// this is a Novell convention

#ifdef SYM_NLM
#define lpgCallBack _lpgCallBack
#endif

extern LPCALLBACKREV1 lpgCallBack;

#ifdef __cplusplus
}
#endif

/*@API:**********************************************************************
@Declaration:
BOOL WINAPI FIL4_KILL(LPTSTR lpszFile,
                      LPVOID lpBuffer,
                      WORD wBufferSize,
                      WORD wTimesToOverwrite)

@Description:
This function overwrites the file with zeros, then deletes it.
Attributes are cleared so R/O will be deleted as well.

@Parameters:
$lpszFile$          File to delete.
$lpBuffer$          Buffer to write 0's to file.
$wBufferSize$
$wTimeToOverwrite$

@Returns: On success, TRUE.  Otherwise FALSE.
@See:
@Include:   ctsn.h
@Compatibility: DOS DX WIN W32 VXD NLM
****************************************************************************/

BOOL WINAPI FIL4_KILL(LPTSTR lpszFile, LPVOID lpBuffer, WORD wBufferSize, WORD wTimesToOverwrite)
{
    WORD            i;
    UINT            uSizeToWrite, uWritten;
    DWORD           dwFileSize, dwBytesToWrite;
    HFILE           hFile;

                                        // Make sure we can write to the file
    if (FIL4_ATTR(lpszFile, 0, SET_INFO) != NOERR)
        {
        return(FALSE);
        }

    if (wTimesToOverwrite)
        {
        hFile = lpgCallBack->FileOpen(lpszFile, OF_WRITE);
        if (hFile != HFILE_ERROR)
            {
            dwFileSize = lpgCallBack->FileSize(hFile);

            MEM4_SET(lpBuffer, 0, wBufferSize);

            for (i = 0; i < wTimesToOverwrite; i++)
                {
                FIL4_SEEK(hFile, 0, fSEEK_BEG);
                dwBytesToWrite = dwFileSize;

                while (dwBytesToWrite)
                    {
                    uSizeToWrite = (wBufferSize > 0xFFFC) ? 0xFFFC : wBufferSize;

                    if (uSizeToWrite > dwBytesToWrite)
                        {
                        uSizeToWrite = (UINT) dwBytesToWrite;
                        }

                    uWritten = FIL4_WRITE(hFile, lpBuffer, uSizeToWrite);

                    if (uWritten == HFILE_ERROR)
                        {
                        break;
                        }

                    dwBytesToWrite -= uWritten;
                    }
                }

            FIL4_SEEK(hFile, 0, fSEEK_BEG);
            FIL4_WRITE(hFile, lpBuffer, 0);

            FIL4_CLOSE(hFile);
            }
        }

    return(FIL4_DELETE(lpszFile));
}
