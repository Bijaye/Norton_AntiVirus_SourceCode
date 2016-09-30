/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/fop_win.c_v   1.1   06 Feb 1996 18:25:30   CNACHEN  $ *
 *                                                                      *
 * Description:                                                         *
 *      WIN & W32 equivalents for Windows file operations.                    *
 *                                                                      *
 * Contains:                                                            *
 *      DoEnvironmentSubst      ???????????????????????????             *
 *      FileClose                                                       *
 *      FileSeek                                                        *
 *      FileRead                                                        *
 *      FileWrite                                                       *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/fop_win.c_v  $ *
// 
//    Rev 1.1   06 Feb 1996 18:25:30   CNACHEN
// Updated WIN_FileWrite to truncate files if provided with a byte count (wCount)
// of 0.
// 
//    Rev 1.0   26 Jan 1996 20:22:54   JREARDON
// Initial revision.
// 
//    Rev 1.1   30 Nov 1995 15:58:02   ASUZDALT
// hFile type casted for W32
// 
//    Rev 1.0   30 Nov 1995 13:27:32   ASUZDALT
// Initial revision.
// 
 ************************************************************************/

#include "platform.h"

#if defined(SYM_WIN)

#include "file.h"
#include "xapi.h"
//#include "dbcs.h"

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      HFILE SYM_EXPORT WINAPI WIN_FileClose (HFILE hFile)                              *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns 0 for success, -1 for error.                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/02/1992 ED Created.                                               *
 ************************************************************************/

HFILE SYM_EXPORT WINAPI WIN_FileClose(HFILE hFile)
{
#ifdef SYM_WIN32

 	return (HFILE)( CloseHandle( (HANDLE)hFile ) - 1 );

#else

	return _lclose( hFile );

#endif
}

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      LONG SYM_EXPORT WINAPI WIN_FileSeek(HFILE hFile, LONG lOffset, int iOrigin)     *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns the new file offset, or -1 for error.                   *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/02/1992 ED Created.                                               *
 ************************************************************************/

LONG SYM_EXPORT WINAPI WIN_FileSeek(HFILE hFile, LONG lOffset, int iOrigin)
{
#ifdef SYM_WIN32
				  						// Anyway can't move father then DWORD
	return SetFilePointer( (HANDLE)hFile, lOffset, NULL, (DWORD)iOrigin );

#else
	return _llseek(hFile, lOffset, iOrigin);

#endif
}

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      UINT SYM_EXPORT WINAPI WIN_FileRead(HFILE hFile, 				*
 *											void _huge *lpBuffer,       *
 *  				                        UINT wBytes)                *
 *  	For W32 flag FILE_FLAG_OVERLAPPED for CreateFile should 		*
 *		not be not set! 												*
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns the number of bytes actually read, or -1 for error.     *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/02/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI WIN_FileRead(HFILE hFile, void _huge *lpBuffer, UINT wBytes)
{
#ifdef SYM_WIN32

	 BOOL 	bRet;
	 DWORD 	dwBytesDone;

	 bRet = ReadFile( (HANDLE)hFile, (LPVOID)lpBuffer, wBytes, &dwBytesDone, NULL );
										// In case of error QUAKE finally calls 
										// SetLastError() for W32, so it looks like 
										// we don't need to do anything in this case
	 if( !bRet )
		return 0xFFFFFFFF;
	 
	return dwBytesDone;

#else	// WIN16

    return _lread(hFile, lpBuffer, wBytes);

#endif	// SYM_WIN32
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      UINT SYM_EXPORT WINAPI WIN_FileWrite(							*
 * 								HFILE hFile, 							*
 *								const void _huge *lpBuffer,     		*
 *                         		UINT wBytes)                            *
 *  	For W32 flag FILE_FLAG_OVERLAPPED for CreateFile should 		*
 *		not be not set! 												*
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns the number of bytes actually written, or -1 for error.  *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/02/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI WIN_FileWrite(HFILE hFile, const void _huge *lpBuffer, UINT wBytes)
{
#ifdef SYM_WIN32

	 BOOL 	bRet;
	 DWORD 	dwBytesDone;

     if (0 == wBytes)
        {
            bRet = SetEndOfFile((HANDLE)hFile);

            if( !bRet )
                return 0xFFFFFFFF;

            return 0;
        }

	 bRet = WriteFile( (HANDLE)hFile, (LPCVOID)lpBuffer, wBytes, &dwBytesDone, NULL );
										// In case of error QUAKE finally calls 
										// SetLastError() for W32, so it looks like 
										// we don't need to do anything in this case
     if( !bRet )
		return 0xFFFFFFFF;
	 
	return dwBytesDone;

#else	// WIN16

	return _lwrite(hFile, (LPCSTR)lpBuffer, wBytes);
#endif	// SYM_WIN32
}
#endif  // SYM_WIN

