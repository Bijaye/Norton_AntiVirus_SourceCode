// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/CLCB.CPv   1.14   23 Oct 1996 11:55:42   DCHI  $
//
// Description:
//  CMDLINE callback implementation.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/CLCB.CPv  $
// 
//    Rev 1.14   23 Oct 1996 11:55:42   DCHI
// Added support for IPC callbacks.
// 
//    Rev 1.13   27 Aug 1996 15:14:16   DCHI
// Data file names are now lowercase.
// 
//    Rev 1.12   13 Aug 1996 14:32:10   DCHI
// Forgot #endif in SetEOF.
// 
//    Rev 1.11   13 Aug 1996 14:29:58   DCHI
// Modification of SetEOF for SYM_UNIX.
// 
//    Rev 1.10   13 Aug 1996 14:00:46   DCHI
// Correction of FileCopy for SYM_UNIX.
// 
//    Rev 1.9   13 Aug 1996 13:49:30   DCHI
// Correction in FileLength for SYM_UNIX.
// 
//    Rev 1.8   12 Aug 1996 18:46:40   DCHI
// Modifications for UNIX.
// 
//    Rev 1.7   27 Jun 1996 18:02:56   DCHI
// Modified file callbacks to use Win32 calls.
// 
//    Rev 1.6   17 Jun 1996 15:10:02   RAY
// Various bug fixes to Boot Repair stuff
// 
//    Rev 1.5   12 Jun 1996 20:28:52   RAY
// added linear read/write retries and code to zero out sector buffers
// 
//    Rev 1.4   05 Jun 1996 21:01:10   RAY
// Added boot scanning and repair support
// 
//    Rev 1.3   04 Jun 1996 18:14:54   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.2   03 Jun 1996 17:20:20   DCHI
// Changes to support building on BORLAND, VC20, SYM_W32, SYM_DOS, and SYM_DOSX.
// 
//************************************************************************

#ifdef SYM_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#include <sys\stat.h>
#include <dos.h>
#include <io.h>
#endif // !SYM_UNIX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "avenge.h"

#include "clcb.h"

#include "fileinfo.h"
#include "pathname.h"
#include "scanglob.h"

#if defined ( BORLAND)
#include <bios.h>
#endif // BORLAND

#if defined(SYM_DOS) || defined(SYM_DOSX)

#include "xapi.h"

#endif // SYM_DOS || SYM_DOSX

#ifdef SYM_DOS

extern "C" {
long __cdecl __filelength(int);
int __cdecl __fileno(FILE *);
}

#define filelength  _filelength
#define fileno      _fileno

#endif // SYM_DOS

CBSTATUS CLCBFileOpen
(
    LPVOID      lpvFileInfo,
    WORD        wOpenMode
)
{
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

#if defined(VC20) || defined(SYM_WIN32)

    HANDLE      hFile;
    DWORD       dwAccess;
    DWORD       dwCreate;

    switch (wOpenMode)
    {
        case FILE_OPEN_READ_ONLY:
            dwAccess = GENERIC_READ;
            dwCreate = OPEN_EXISTING;
            break;

        case FILE_OPEN_WRITE_ONLY:
            dwAccess = GENERIC_WRITE;
            dwCreate = CREATE_ALWAYS;
            break;

        case FILE_OPEN_READ_WRITE:
            dwAccess = GENERIC_READ | GENERIC_WRITE;
            dwCreate = OPEN_ALWAYS;
            break;

        default:
            return(CBSTATUS_ERROR);
    }

    hFile = CreateFile(lpFileInfo->lpstrPathName,
                       dwAccess,
                       0,           // No sharing
                       NULL,        // No security descriptor
                       dwCreate,
                       0,           // Normal attributes
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return(CBSTATUS_ERROR);

#else

    FILE *      hFile;
    char        *szMode;

    if (wOpenMode == FILE_OPEN_READ_ONLY)
        szMode = "rb";
	else
    if (wOpenMode == FILE_OPEN_WRITE_ONLY)
    	szMode = "wb";
    else
	if (wOpenMode == FILE_OPEN_READ_WRITE)
        szMode = "rb+";
    else
        return(CBSTATUS_ERROR);

    hFile = fopen(lpFileInfo->lpstrPathName,szMode);
    if (hFile == NULL)
        return(CBSTATUS_ERROR);

#endif

    lpFileInfo->hFile = hFile;

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileClose
(
    LPVOID      lpvFileInfo
)
{
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

#if defined(VC20) || defined(SYM_WIN32)

    if (CloseHandle(lpFileInfo->hFile) == FALSE)
        return(CBSTATUS_ERROR);

#else

    if (fclose(lpFileInfo->hFile) != 0)
        return(CBSTATUS_ERROR);

#endif

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileSeek
(
    LPVOID      lpvFileInfo,
    long        lOffset,
    WORD        wWhence,
    LPDWORD     lpdwNewOffset
)
{
    DWORD       dwResult;
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

#if defined(VC20) || defined(SYM_WIN32)

    DWORD       dwMoveMethod;

    switch (wWhence)
    {
        case SEEK_SET:
            dwMoveMethod = FILE_BEGIN;
            break;

        case SEEK_CUR:
            dwMoveMethod = FILE_CURRENT;
            break;

        case SEEK_END:
            dwMoveMethod = FILE_END;
            break;

        default:
            return(CBSTATUS_ERROR);
    }

    dwResult = SetFilePointer(lpFileInfo->hFile,
                              lOffset,
                              NULL,
                              dwMoveMethod);

    if (dwResult == 0xFFFFFFFF)
        return(CBSTATUS_ERROR);

    *lpdwNewOffset = dwResult;

#else

    dwResult = fseek(lpFileInfo->hFile,lOffset,wWhence);

    if (dwResult != 0)
        return(CBSTATUS_ERROR);

    *lpdwNewOffset = ftell(lpFileInfo->hFile);

#endif

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileRead
(
    LPVOID      lpvFileInfo,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwNumActualBytes
)
{
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

    // Only attempt read if bytes to read is non-zero

    if (dwNumBytes != 0)
    {
#if defined(VC20) || defined(SYM_WIN32)

        if (ReadFile(lpFileInfo->hFile,
                     lpvBuffer,
                     dwNumBytes,
                     lpdwNumActualBytes,
                     NULL) == FALSE)
            return(CBSTATUS_ERROR);

#else

        UINT    uiBytesRead = fread(lpvBuffer,
                                    sizeof(BYTE),
                                    (WORD)dwNumBytes,
                                    lpFileInfo->hFile);

        if (ferror(lpFileInfo->hFile))
            return(CBSTATUS_ERROR);

        *lpdwNumActualBytes = uiBytesRead;

#endif
    }
    else
        *lpdwNumActualBytes = 0;

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileWrite
(
    LPVOID      lpvFileInfo,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwNumActualBytes
)
{
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

    // Only attempt write if bytes to write is non-zero

    if (dwNumBytes != 0)
    {
#if defined(VC20) || defined(SYM_WIN32)

        if (WriteFile(lpFileInfo->hFile,
                      lpvBuffer,
                      dwNumBytes,
                      lpdwNumActualBytes,
                      NULL) == FALSE)
            return(CBSTATUS_ERROR);

#else

        UINT    uiBytesWritten = fwrite(lpvBuffer,
                                        sizeof(BYTE),
                                        (WORD)dwNumBytes,
                                        lpFileInfo->hFile);

        if (ferror(lpFileInfo->hFile))
            return(CBSTATUS_ERROR);

        *lpdwNumActualBytes = uiBytesWritten;

#endif
    }
    else
        *lpdwNumActualBytes = 0;

    return(CBSTATUS_OK);

}

CBSTATUS CLCBFileTell
(
    LPVOID      lpvFileInfo,
    LPDWORD     lpdwOffset
)
{
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;
    DWORD       dwOffset;

#if defined(VC20) || defined(SYM_WIN32)

    dwOffset = SetFilePointer(lpFileInfo->hFile,
                              0,
                              NULL,
                              FILE_CURRENT);

    if (dwOffset == 0xFFFFFFFF)
        return(CBSTATUS_ERROR);

#else

    dwOffset = ftell(lpFileInfo->hFile);

    if (ferror(lpFileInfo->hFile))
        return(CBSTATUS_ERROR);

#endif

    *lpdwOffset = dwOffset;

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileSetEOF
(
    LPVOID      lpvFileInfo
)
{
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

#if defined(VC20) || defined(SYM_WIN32)

    if (SetEndOfFile(lpFileInfo->hFile) == FALSE)
        return(CBSTATUS_ERROR);

#else

    WORD        wValue;

    wValue = fileno(lpFileInfo->hFile);

#ifdef BORLAND
    _BX = wValue;
    _AH = 0x40;
    _CX = 0;
    geninterrupt(0x21);

    if (_AX != 0)
        return(CBSTATUS_ERROR);
#endif // BORLAND

#if defined(SYM_DOS) || defined(SYM_DOSX)
    _asm {
        mov     bx, wValue
        mov     ah, 40h
        xor     cx, cx
        int     21h
        mov     wValue, ax
    }

    if (wValue != 0)
        return(CBSTATUS_ERROR);
#endif // SYM_DOS || SYM_DOSX

#ifdef SYM_UNIX
    if (ftruncate(wValue,ftell(lpFileInfo->hFile)) != 0)
        return(CBSTATUS_ERROR);
#endif

#endif

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileEOF
(
    LPVOID lpvFileInfo,
    LPBOOL lpbEOF
)
{
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

#if defined(VC20) || defined(SYM_WIN32)

    BY_HANDLE_FILE_INFORMATION  stHandleFileInfo;
    DWORD                       dwOffset;

    // Get the file length

    if (GetFileInformationByHandle(lpFileInfo->hFile,
                                   &stHandleFileInfo) == FALSE)
        return(CBSTATUS_ERROR);

    // Check the current position

    dwOffset = SetFilePointer(lpFileInfo->hFile,
                              0,
                              NULL,
                              FILE_CURRENT);

    if (dwOffset == 0xFFFFFFFF)
        return(CBSTATUS_ERROR);

    // Assume 32-bit file length

    if (dwOffset == stHandleFileInfo.nFileSizeLow)
        *lpbEOF = TRUE;
    else
        *lpbEOF = FALSE;

#else

    if (feof(lpFileInfo->hFile))
        *lpbEOF = TRUE;
    else
        *lpbEOF = FALSE;

    // feof is a macro, so no error checking is necessary

#endif

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileGetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPDWORD lpdwInfo
)
{
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;
    LPSTR       lpszFileName = lpFileInfo->lpstrPathName;

	(void)wType;

#if defined(VC20) || defined(SYM_WIN32)

    *lpdwInfo = GetFileAttributes(lpszFileName);

    if (*lpdwInfo == 0xFFFFFFFF)
        return(CBSTATUS_ERROR);

#elif !defined(SYM_UNIX)

    WORD        wResult, wError;

    _asm mov ax, 04300h
    _asm push ds
    _asm lds dx, lpszFileName
    _asm int 21h
    _asm pop ds
    _asm mov wResult, cx
    _asm jc  get_attr_error
    _asm mov wError, FALSE
    _asm jmp get_attr_done

get_attr_error:
    _asm mov wError, TRUE

get_attr_done:

    if (wError)
        return(CBSTATUS_ERROR);

    *lpdwInfo = wResult;

#endif

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileSetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
	DWORD   dwInfo
)
{
    LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;
	LPSTR       lpszFileName = lpFileInfo->lpstrPathName;

#if defined(VC20) || defined(SYM_WIN32)

    if (SetFileAttributes(lpszFileName,dwInfo) == FALSE)
        return(CBSTATUS_ERROR);

#elif !defined(SYM_UNIX)

    WORD        wResult, wError;

    (void)wType;

	wResult = (WORD)dwInfo;

    _asm push ds
    _asm mov ax, 04301h
    _asm lds dx, lpszFileName
    _asm mov cx, wResult
    _asm int 21h
    _asm pop ds
    _asm jc  set_attr_error
    _asm mov wError, FALSE
    _asm jmp set_attr_done

set_attr_error:
    _asm mov wError, TRUE

set_attr_done:

	if (wError)
        return(CBSTATUS_ERROR);

#endif

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileGetDateTime
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPWORD  lpwDate,
    LPWORD  lpwTime
)
{
	LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

	(void)wType;

#if defined(VC20) || defined(SYM_WIN32)

    FILETIME    stLastWriteFileTime;

    // Get the file last write time

    if (GetFileTime(lpFileInfo->hFile,
                    NULL,
                    NULL,
                    &stLastWriteFileTime) == FALSE)
        return(CBSTATUS_ERROR);

    if (FileTimeToDosDateTime(&stLastWriteFileTime,
                              lpwDate,
                              lpwTime) == FALSE)
        return(CBSTATUS_ERROR);

#elif !defined(SYM_UNIX)

    WORD        wFileNo;
    WORD        wDate;
    WORD        wTime;
    WORD        wError;

    wFileNo = fileno(lpFileInfo->hFile);

    // Query date/time of file

    _asm mov     ax, 5700h
    _asm mov     bx, wFileNo
    _asm int     21h
    _asm jc      get_date_time_error

    _asm mov     wError, FALSE
    _asm mov     wDate, dx
    _asm mov     wTime, cx
    _asm jmp     get_date_time_done

get_date_time_error:
    _asm mov     wError, TRUE

get_date_time_done:

    if (wError)
        return(CBSTATUS_ERROR);

    *lpwDate = wDate;
    *lpwTime = wTime;

#endif

	return(CBSTATUS_OK);
}

CBSTATUS CLCBFileSetDateTime
(
	LPVOID  lpvFileInfo,
	WORD    wType,
    WORD    wDate,
    WORD    wTime
)
{
	LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

	(void)wType;

#if defined(VC20) || defined(SYM_WIN32)

    FILETIME    stLastWriteFileTime;

    // Get the file last write time

    if (DosDateTimeToFileTime(wDate,
                              wTime,
                              &stLastWriteFileTime) == FALSE)
        return(CBSTATUS_ERROR);

    if (SetFileTime(lpFileInfo->hFile,
                    NULL,
                    NULL,
                    &stLastWriteFileTime) == FALSE)
        return(CBSTATUS_ERROR);

#elif !defined(SYM_UNIX)
    WORD        wFileNo;
    WORD        wError;

    wFileNo = fileno(lpFileInfo->hFile);

    // Set date/time of file

    _asm mov     ax, 5701h   
    _asm mov     bx, wFileNo
    _asm mov     cx, wTime
    _asm mov     dx, wDate
    _asm int     21h
    _asm jc      set_date_time_error

    _asm mov     wError, FALSE
    _asm jmp     set_date_time_done

set_date_time_error:
    _asm mov     wError, TRUE

set_date_time_done:

    if (wError)
        return(CBSTATUS_ERROR);

#endif

	return(CBSTATUS_OK);

}

CBSTATUS CLCBFileFullNameAndPath
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFileName,
    WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrFileName;
	(void)wBufferLen;

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileName
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFileName,
    WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrFileName;
	(void)wBufferLen;

	return(CBSTATUS_OK);
}

CBSTATUS CLCBFilePath
(
	LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFilePath,
	WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrFilePath;
	(void)wBufferLen;

	return(CBSTATUS_OK);
}

CBSTATUS CLCBFileLogicalDrive
(
	LPVOID  lpvFileInfo,
    LPWSTR  lpwstrDriveLetter,
	WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrDriveLetter;
	(void)wBufferLen;

	return(CBSTATUS_OK);
}

CBSTATUS CLCBFileLength
(
	LPVOID  lpvFileInfo,
	LPDWORD lpdwFileLength
)
{
	LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;
	DWORD       dwLength;

#if defined(VC20) || defined(SYM_WIN32)

    dwLength = GetFileSize(lpFileInfo->hFile,NULL);
    if (dwLength == 0xFFFFFFFF)
        return(CBSTATUS_ERROR);

#elif defined(SYM_UNIX)
    long    lOrigPos;

    lOrigPos = ftell(lpFileInfo->hFile);
    if (fseek(lpFileInfo->hFile,0,SEEK_END) == -1)
        return(CBSTATUS_ERROR);

    dwLength = ftell(lpFileInfo->hFile);
    if (fseek(lpFileInfo->hFile,lOrigPos,SEEK_SET) == -1)
        return(CBSTATUS_ERROR);

#else

    dwLength = filelength(fileno(lpFileInfo->hFile));

	if (dwLength == (DWORD)-1)
		return(CBSTATUS_ERROR);

#endif

	*lpdwFileLength = dwLength;

	return(CBSTATUS_OK);
}

CBSTATUS CLCBFileDelete
(
	LPVOID  lpvFileInfo
)
{
	LPFILEINFO  lpFileInfo = (LPFILEINFO)lpvFileInfo;

    if (remove(lpFileInfo->lpstrPathName) != 0)
		return(CBSTATUS_ERROR);

	return(CBSTATUS_OK);
}

CBSTATUS CLCBFileCopy
(
    LPVOID  lpvSrcFileInfo,
    LPVOID  lpvDstFileInfo
)
{
    char szCmd[SYM_MAX_PATH+SYM_MAX_PATH+10];

#ifdef SYM_UNIX
    sprintf(szCmd,"cp %s %s > nul",
        ((LPFILEINFO)lpvSrcFileInfo)->lpstrPathName,
        ((LPFILEINFO)lpvDstFileInfo)->lpstrPathName);
#else
    sprintf(szCmd,"copy %s %s > nul",
        ((LPFILEINFO)lpvSrcFileInfo)->lpstrPathName,
        ((LPFILEINFO)lpvDstFileInfo)->lpstrPathName);
#endif

    if (system(szCmd) == 0)
        return(CBSTATUS_OK);

    return(CBSTATUS_ERROR);
}

CBSTATUS CLCBFileRename
(
    LPVOID  lpvOldFileInfo,
    LPWSTR  lpwstrNewName
)
{
    char szNewName[PATHNAME_MAX_NAME+PATHNAME_MAX_EXT+1];
    char szName[PATHNAME_MAX_NAME+1];
    char szExt[PATHNAME_MAX_EXT+1];

    PathNameSplit((LPSTR)lpwstrNewName,
    			  NULL,NULL,szName,szExt);

    PathNameMerge(szNewName,NULL,NULL,szName,szExt);

    if (rename(((LPFILEINFO)lpvOldFileInfo)->lpstrPathName,
			   szNewName) == 0)
        return(CBSTATUS_OK);

    return(CBSTATUS_ERROR);
}

#ifdef SYM_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#endif // SYM_UNIX

CBSTATUS CLCBFileExists
(
    LPVOID  lpvFileInfo,
    LPBOOL	lpbExists
)
{
#if defined(BORLAND) || defined(SYM_DOS) || defined(SYM_DOSX)
    struct find_t       stFindData;

    // Do a findfirst

    if (_dos_findfirst(((LPFILEINFO)lpvFileInfo)->lpstrPathName,
        0xFF,&stFindData) == 0)
        *lpbExists = TRUE;
    else
        *lpbExists = FALSE;
#endif // BORLAND || SYM_DOS || SYM_DOSX

#if defined(VC20) || defined(SYM_WIN32)
    long                lFindHandle;
    struct _finddata_t  stFindData;

    lFindHandle = _findfirst(((LPFILEINFO)lpvFileInfo)->lpstrPathName,
                             &stFindData);

    if (lFindHandle != -1)
        *lpbExists = TRUE;
    else
        *lpbExists = FALSE;

    _findclose(lFindHandle);
#endif // VC20 || SYM_WIN32

#ifdef SYM_UNIX

    struct stat stStat;

    // Do a stat on it

    if (stat(((LPFILEINFO)lpvFileInfo)->lpstrPathName,&stStat) != -1)
        *lpbExists = TRUE;
    else
        *lpbExists = FALSE;

#endif // SYM_UNIX

    return(CBSTATUS_OK);
}

CBSTATUS CLCBFileGetErrorCondition
(
	LPVOID  lpvFileInfo,
	LPDWORD lpdwErrorCondition
)
{
	(void)lpvFileInfo;

	*lpdwErrorCondition = 0;

	return(CBSTATUS_OK);
}


CBSTATUS CLCBMemoryAlloc
(
	DWORD        dwSize,
	WORD         wMemType,
	LPLPVOID     lplpvBuffer
)
{
	LPVOID	lpvData;

	(void)wMemType;

	lpvData = malloc((size_t)dwSize);

	if (lpvData == NULL)
		return(CBSTATUS_ERROR);

	*lplpvBuffer = lpvData;

	return(CBSTATUS_OK);
}

CBSTATUS CLCBMemoryFree
(
	LPVOID lpvBuffer
)
{
	free(lpvBuffer);					// always OK.. no return value

	return(CBSTATUS_OK);
}


CBSTATUS CLCBGetDataFileInfo
(
    LPLPVOID        lplpvFileInfo,
    WORD            wDataFileNum
)
{
    LPFILEINFO      lpFileInfo;

    if (wDataFileNum == DATA_FILE_ID_VIRSCAN1DAT ||
		wDataFileNum == DATA_FILE_ID_VIRSCAN2DAT ||
		wDataFileNum == DATA_FILE_ID_VIRSCAN3DAT ||
		wDataFileNum == DATA_FILE_ID_VIRSCAN4DAT)
    {
        lpFileInfo = (LPFILEINFO)malloc(sizeof(FILEINFO_T));
        if (lpFileInfo == NULL)
			return(CBSTATUS_ERROR);

        lpFileInfo->hFile = NULL;
        switch (wDataFileNum)
        {
        	case DATA_FILE_ID_VIRSCAN1DAT:
                sprintf(lpFileInfo->lpstrPathName,"%svirscan1.dat",
                    gstScanGlobal.szHomeDir);
                break;
        	case DATA_FILE_ID_VIRSCAN2DAT:
                sprintf(lpFileInfo->lpstrPathName,"%svirscan2.dat",
                    gstScanGlobal.szHomeDir);
                break;
        	case DATA_FILE_ID_VIRSCAN3DAT:
                sprintf(lpFileInfo->lpstrPathName,"%svirscan3.dat",
                    gstScanGlobal.szHomeDir);
                break;
        	default:
                sprintf(lpFileInfo->lpstrPathName,"%svirscan4.dat",
                    gstScanGlobal.szHomeDir);
                break;
        }

        *lplpvFileInfo = (LPVOID)lpFileInfo;

		return(CBSTATUS_OK);
	}
	else
	{
		printf("Invalid data file ID.\n");

		return(CBSTATUS_ERROR);
    }
}

CBSTATUS CLCBCloseDataFileInfo
(
	LPVOID lpvFileInfo
)
{
    free(lpvFileInfo);

    return(CBSTATUS_OK);
}

CBSTATUS CLCBMemGetPtrToMem
(
    LPVOID       lpvInfo,
    DWORD        dwMemAddress,
    LPBYTE       lpbyMemBuf,
    DWORD        dwMemSize,
    LPLPBYTE     lplpbyMemPtr,
    LPCOOKIETRAY lpCookieTray
)
{
    (void)lpbyMemBuf;
	(void)dwMemSize;
	(void)lpCookieTray;

#if defined(BORLAND) || (defined(SYM_DOS) && !defined(SYM_DOSX))
    *lplpbyMemPtr = (LPBYTE)(
            (dwMemAddress & 0xF) | ((dwMemAddress >> 4) << 16));
#endif // BORLAND || (SYM_DOS && !SYM_DOSX)

#if defined(VC20) || defined(SYM_WIN32)

    *lplpbyMemPtr = lpbyMemBuf;
/*
    // Will fix

    // Doesn't work - memory access violation!?

    *lplpbyMemPtr = (LPBYTE)dwMemAddress;
*/
#endif // VC20 || SYM_WIN32

#ifdef SYM_DOSX

    MemCopyPhysical(lpbyMemBuf,
                    MAKELP(((WORD)(dwMemAddress >> 4)),
                           ((WORD)(dwMemAddress & 0xF))),
                    (WORD)(dwMemSize),
                    0);

    *lplpbyMemPtr = lpbyMemBuf;

#endif // SYM_DOSX

    return(CBSTATUS_OK);
}

CBSTATUS CLCBMemReleasePtrToMem
(
    LPVOID       lpvInfo,
    LPBYTE       lpbyMemPtr,
    LPCOOKIETRAY lpCookieTray
)
{
	(void)lpvInfo,
	(void)lpCookieTray;
	(void)lpbyMemPtr;

    return(CBSTATUS_OK);
}

//********************************************************************
//
// Function:
//  CLCBProgressInit()
//
// Parameters:
//  lptstrObjectName        String containing the object name being worked on
//  dwLevel                 What level of processing is this object?
//  dwProgressValueMax      What's our MAX progress value? (100%, etc?)
//  lpCookieTray            Pointer to a structure of our global, local, and
//                          transient cookies.
//  lplpvData               Set to point to the application's progress data
//
// Description:
//  This function is called to initialize a progress context.  This progress
//  context is used during scanning or any other task that takes noticable
//  time.  The progress context is passed along through the engine and back
//  to the ProgressUpdate and ProgressClose functions below to update the
//  application (and the user) as to the progress of the engine.
//
// Returns:
//  CBSTATUS_OK         On successful progress context creation
//  CBSTATUS_ERROR      If unable to create a progress context
//
//********************************************************************

CBSTATUS CLCBProgressInit
(
    LPWSTR       lpwstrObjectName,
    DWORD        dwLevel,
    DWORD        dwProgressValueMax,
    LPCOOKIETRAY lpCookieTray,
    LPLPVOID     lplpvData
)
{
    LPCMD_PROGRESS      lpProgress;

    // assume the worst

    *lplpvData = NULL;

    // allocate some memory

    lpProgress = (LPCMD_PROGRESS)malloc(sizeof(CMD_PROGRESS_T));
    if (lpProgress == NULL)
    {
        return(CBSTATUS_ERROR);
    }

    lpProgress->stCookieTray = *lpCookieTray;
    lpProgress->dwLevel = dwLevel;
    lpProgress->dwProgressValueMax = dwProgressValueMax;
    lpProgress->dwLastTime = time(NULL);

    if (lpwstrObjectName != NULL)
    {
        lpProgress->lpszObjectName =
            (LPSTR)malloc(strlen((LPSTR)lpwstrObjectName)+1);

        if (lpProgress->lpszObjectName == NULL)
        {
            free(lpProgress);

            return(CBSTATUS_ERROR);
        }
        else
        {
            strcpy(lpProgress->lpszObjectName,(LPSTR)lpwstrObjectName);
        }
    }
    else
        lpProgress->lpszObjectName = NULL;

    *lplpvData = lpProgress;

    return(CBSTATUS_OK);
}


//********************************************************************
//
// Function:
//  CLCBProgressUpdate()
//
// Parameters:
//  lpvData             Point to the application's progress data
//  dwProgressValue     Value between 0 and dwProgressValueMax
//
// Description:
//  This function is called by the engine to update the application on the
//  progress of a given task.  The dwProgressValue is used to indicate how
//  much longer the engine must work before it has completed its task.
//
// Returns:
//  CBSTATUS_OK         If the engine should continue on its merry way
//  CBSTATUS_ABORT      If the application/user wants to abort the current
//                      engine taks.
//  CBSTATUS_ERROR      If there is an error (engine should close immediately)
//
//********************************************************************

CBSTATUS CLCBProgressUpdate
(
    LPVOID       lpvData,
    DWORD        dwProgressValue
)
{
    char            spinner[4] = {'|','/','-','\\'};
    static WORD     wSpinCount;
    long            lCurTime;
    LPCMD_PROGRESS  lpCommand = (LPCMD_PROGRESS)lpvData;

    if (lpCommand == NULL)
        return(CBSTATUS_ERROR);

    // Update last time updated

    lpCommand->dwLastTime = lCurTime;

    // Print out kilobytes scanned and spinning cursor

    printf("%7ldK %c\r",dwProgressValue/1024,spinner[wSpinCount++&3]);

    return(CBSTATUS_OK);
}

//********************************************************************
//
// Function:
//  CLCBProgressClose()
//
// Parameters:
//  lpvData             Point to the application's progress data
//
// Description:
//  This function is called by the engine once a task is completed. It frees
//  all context information and returns.
//
// Returns:
//  CBSTATUS_OK         If the engine should continue on its merry way
//  CBSTATUS_ERROR      If there is an error (engine should close immediately)
//
//********************************************************************


CBSTATUS CLCBProgressClose
(
    LPVOID       lpvData
)
{
    LPCMD_PROGRESS  lpCommand = (LPCMD_PROGRESS)lpvData;

    if (lpCommand != NULL)
    {
        if (lpCommand->lpszObjectName != NULL)
        {
            free(lpCommand->lpszObjectName);
            lpCommand->lpszObjectName = NULL;
        }

        free(lpCommand);
    }

    return(CBSTATUS_OK);
}


CBSTATUS CLCBLinearOpen
(
    LPVOID  lpvInfo,                // [in] Linear object info
    WORD    wOpenMode               // [in] filler
)
{
    return ( CBSTATUS_OK );
}


CBSTATUS CLCBLinearClose
(
    LPVOID  lpvInfo                 // [in] Linear object info
)
{
    return ( CBSTATUS_OK );
}


CBSTATUS CLCBLinearRead
(
    LPVOID  lpvInfo,                // [in] Linear object info
    DWORD   dwSectorLow,            // [in] Sector to begin reading
    DWORD   dwSectorHigh,           // [in] Sector's (high dword)
    LPBYTE  lpbyBuffer,             // [out] Buffer to read into
    DWORD   dwLength,               // [in] Sectors to read
    LPDWORD lpdwActualLength        // [out] Sectors read
)
{
#if !defined ( SYM_WIN32 ) && !defined(SYM_UNIX)
    BYTE bySide;
    WORD wCylinder;
    BYTE bySector;

    BYTE bySectors;
    BYTE byCylinderLow;
    BYTE byCylinderHigh;
    BYTE byDriveNum;
    WORD wBufferSegment;
    WORD wBufferOffset;

    BYTE byRetries = 3;

    CBSTATUS cbResult;
    BYTE     byResult;

    cbResult = CLCBLinearConvertToPhysical ( lpvInfo,
                                             dwSectorLow,
                                             &bySide,
                                             &wCylinder,
                                             &bySector );

    if ( CBSTATUS_OK == cbResult )
        {
        if (( dwLength > 256 ) ||
            ( dwSectorHigh != 0 ))
            {
            cbResult = CBSTATUS_ERROR;
            }
        else while ( byRetries-- > 0 )
            {
            bySectors = (BYTE) dwLength ;
            byDriveNum = (BYTE) lpvInfo;
            bySector = bySector & 0x3F;

            wBufferSegment = FP_SEG ( lpbyBuffer );
            wBufferOffset  = FP_OFF ( lpbyBuffer );

            byCylinderLow = LOBYTE ( wCylinder );
            byCylinderHigh = HIBYTE ( wCylinder ) << 6;

            _asm {
                push    si
                push    di
                mov     ah, 02h
                mov     al, bySectors
                mov     ch, byCylinderLow
                mov     cl, bySector
                or      cl, byCylinderHigh
                mov     dh, bySide
                mov     dl, byDriveNum
                mov     es, wBufferSegment
                mov     bx, wBufferOffset
                int     13h
                mov     byResult, ah
                pop     di
                pop     si
                }

            if ( byResult != 0 )
                {
                cbResult = CBSTATUS_ERROR;
                *lpdwActualLength = 0;
                }
            else
                {
                cbResult = CBSTATUS_OK;
                *lpdwActualLength = bySectors;
                }
            if ( CBSTATUS_OK == cbResult ) break;
            }
        }

    return ( cbResult );

#else

    return ( CBSTATUS_OK );

#endif // !SYM_WIN32
}

CBSTATUS CLCBLinearWrite
(
    LPVOID  lpvInfo,                // [in] Linear object info
    DWORD   dwSectorLow,            // [in] Sector to begin writing
    DWORD   dwSectorHigh,           // [in] Sector (high dword)
    LPBYTE  lpbyBuffer,             // [out] Buffer to write from
    DWORD   dwLength,               // [in] Sectors to write
    LPDWORD lpdwActualLength        // [in/out] Sectors written
)
{
#if !defined ( SYM_WIN32 ) && !defined(SYM_UNIX)
    BYTE bySide;
    WORD wCylinder;
    BYTE bySector;

    BYTE bySectors;
    BYTE byDriveNum;
    WORD wBufferSegment;
    WORD wBufferOffset;

    BYTE byRetries = 3;

    CBSTATUS cbResult;
    BYTE byResult;

    cbResult = CLCBLinearConvertToPhysical ( lpvInfo,
                                             dwSectorLow,
                                             &bySide,
                                             &wCylinder,
                                             &bySector );

    if ( CBSTATUS_OK == cbResult )
        {
        if (( dwLength > 256 ) || ( dwSectorHigh != 0 ))
            {
            cbResult = CBSTATUS_ERROR;
            }
        else while ( byRetries-- > 0 )
            {
            bySectors = (BYTE) dwLength ;
            byDriveNum = (BYTE) lpvInfo;

            wBufferSegment = FP_SEG ( lpbyBuffer );
            wBufferOffset  = FP_OFF ( lpbyBuffer );

            _asm {
                push    si
                push    di
                mov     ah, 03h
                mov     al, bySectors
                mov     cx, wCylinder
                shl     cl, 06h
                or      cl, bySector
                mov     dh, bySide
                mov     dl, byDriveNum
                mov     es, wBufferSegment
                mov     bx, wBufferOffset
                int     13h
                mov     byResult, ah
                pop     di
                pop     si
                }

            if ( byResult != 0 )
                {
                cbResult = CBSTATUS_ERROR;
                *lpdwActualLength = 0;
                }
            else
                {
                cbResult = CBSTATUS_OK;
                *lpdwActualLength = bySectors;
                }
            if ( CBSTATUS_OK == cbResult ) break;
            }
        }

    return ( cbResult );

#else

    return ( CBSTATUS_OK );
#endif  // SYM_WIN32
}

CBSTATUS CLCBLinearGetDimensions
(
    LPVOID  lpvInfo,                // [in] Linear object info
    LPBYTE  lpbySides,              // [out] Total Sides
    LPWORD  lpwCylinders,           // [out] Total Cylinders
    LPBYTE  lpbySectors             // [out] Sectors per cylinder
)
{
#if defined ( BORLAND ) || defined ( SYM_DOS )
    BYTE byDriveNum;
    BYTE byResult;
    BYTE bySides;
    BYTE byCylinders;
    BYTE bySectors;

    BYTE byRetries = 3;

    CBSTATUS cbResult;

    byDriveNum = (BYTE) lpvInfo;

    while ( byRetries-- > 0 )
        {
        _asm {
            push    si
            push    di
            mov     ah, 08h
            mov     dl, byDriveNum
            int     13h
            mov     byResult, ah
            mov     byCylinders, ch
            mov     bySectors, cl
            mov     bySides, dh
            pop     di
            pop     si
            }

        if ( 0 == byResult ) break;
        }


    if ( byResult != 0 )
        {
        cbResult = CBSTATUS_ERROR;
        }
    else
        {
        *lpbySides = bySides;
        *lpwCylinders = byCylinders;
        *lpwCylinders |= (WORD) ( bySectors & 0xC0 ) << 2;
        *lpbySectors = bySectors & 0x3F;
        ++(*lpbySides);
        ++(*lpwCylinders);
        cbResult = CBSTATUS_OK;
        }
    return ( cbResult );
#else
    return ( CBSTATUS_ERROR );
#endif
}

CBSTATUS CLCBLinearConvertFromPhysical
(
    LPVOID      lpvInfo,            // [in] Linear object info
    BYTE        bySide,             // [in] Physical side, ...
    WORD        wCylinder,          // [in] cylinder, and ...
    BYTE        bySector,           // [in] sector to translate
    LPDWORD     lpdwSectorLow       // [out] Translated sector low dword
)
{

    BYTE bySides;
    WORD wCylinders;
    BYTE bySectors;

    CBSTATUS cbResult;

    cbResult = CLCBLinearGetDimensions ( lpvInfo, &bySides, &wCylinders, &bySectors );

    if ( CBSTATUS_OK == cbResult )
        {
        *lpdwSectorLow = ((DWORD) wCylinder * bySectors * bySides ) +
                         ((DWORD) bySide * bySectors ) +
                         ((DWORD) bySector - 1 );
        }

    return ( cbResult );
}

CBSTATUS CLCBLinearConvertToPhysical
(
    LPVOID      lpvInfo,            // [in] Linear object info
    DWORD       dwSectorLow,        // [in] Linear sector low dword
    LPBYTE      lpbySide,             // [out] Translated side, ...
    LPWORD      lpwCylinder,          // [out] cylinder, and ...
    LPBYTE      lpbySector            // [out] sector
)
{
    BYTE bySides;
    WORD wCylinders;
    BYTE bySectors;

    CBSTATUS cbResult;

    cbResult = CLCBLinearGetDimensions ( lpvInfo, &bySides, &wCylinders, &bySectors );

    if ( CBSTATUS_OK == cbResult )
        {
        *lpwCylinder = (WORD) (dwSectorLow / ( (WORD) bySectors * (WORD) bySides ));
        dwSectorLow -= (DWORD) *lpwCylinder * ( (WORD) bySectors * (WORD) bySides );
        *lpbySide = (BYTE) (dwSectorLow / bySectors);
        *lpbySector = (BYTE) (dwSectorLow % bySectors) + 1;
        }

    return ( cbResult );
}

CBSTATUS CLCBLinearIsPartitionedMedia ( LPVOID lpvInfo, LPBOOL lpbPartitioned )
{
    if ( (BYTE) lpvInfo < 0x80 ) *lpbPartitioned = FALSE;
    else *lpbPartitioned = TRUE;

    return ( CBSTATUS_OK );
}

// Mutex

typedef struct tagMUTEX_INFO
{
    DWORD           dwReserved;

#if defined(VC20) || defined(SYM_WIN32)
    HANDLE          hMutex;
#endif
} MUTEX_INFO_T, FAR *LPMUTEX_INFO;

CBSTATUS CLCBMutexCreate
(
    LPLPVOID        lplpvMutexInfo
)
{
    LPMUTEX_INFO    lpstMutexInfo;

    lpstMutexInfo = (LPMUTEX_INFO)malloc(sizeof(MUTEX_INFO_T));
    if (lpstMutexInfo == NULL)
        return(CBSTATUS_OK);

#if defined(VC20) || defined(SYM_WIN32)

    lpstMutexInfo->hMutex = CreateMutex(NULL,   // Default security attributes
                                        FALSE,  // No initial owner
                                        NULL);  // Don't care about its name

    if (lpstMutexInfo->hMutex == NULL)
    {
        free(lpstMutexInfo);
        return(CBSTATUS_ERROR);
    }

#endif

    *lplpvMutexInfo = (LPVOID)lpstMutexInfo;

    return(CBSTATUS_OK);
}

CBSTATUS CLCBMutexDestroy
(
    LPVOID          lpvMutexInfo
)
{
    CBSTATUS        cbStatus = CBSTATUS_OK;

#if defined(VC20) || defined(SYM_WIN32)

    if (CloseHandle(((LPMUTEX_INFO)lpvMutexInfo)->hMutex) == FALSE)
    {
        // Failed to close mutex

        cbStatus = CBSTATUS_ERROR;
    }

#endif

    free(lpvMutexInfo);

    return(CBSTATUS_OK);
}

CBSTATUS CLCBMutexWait
(
    LPVOID          lpvMutexInfo,
    DWORD           dwTimeoutMS
)
{
#if defined(VC20) || defined(SYM_WIN32)

    if (WaitForSingleObject(((LPMUTEX_INFO)lpvMutexInfo)->hMutex,
                            dwTimeoutMS) != WAIT_OBJECT_0)
    {
        // Failure getting mutex

        return(CBSTATUS_ERROR);
    }

#endif

    return(CBSTATUS_OK);
}

CBSTATUS CLCBMutexRelease
(
    LPVOID          lpvMutexInfo
)
{
#if defined(VC20) || defined(SYM_WIN32)

    if (ReleaseMutex(((LPMUTEX_INFO)lpvMutexInfo)->hMutex) == FALSE)
    {
        // Failure releasing mutex

        return(CBSTATUS_ERROR);
    }

#endif

    return(CBSTATUS_OK);
}

// Semaphore

CBSTATUS CLCBSemaphoreCreate
(
    LPLPVOID        lplpvSemaphoreInfo,
    DWORD           dwInitialCount,
    DWORD           dwMaximumCount
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS CLCBSemaphoreDestroy
(
    LPVOID          lpvSemaphoreInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS CLCBSemaphoreWait
(
    LPVOID          lpvSemaphoreInfo,
    DWORD           dwTimeoutMS
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS CLCBSemaphoreRelease
(
    LPVOID          lpvSemaphoreInfo,
    LPDWORD         lpdwPrevCount
)
{
    return(CBSTATUS_ERROR);
}

FILECALLBACKS_T gstCLCBFileCallBacks =
{
    CLCBFileOpen,
    CLCBFileClose,
    CLCBFileSeek,
    CLCBFileRead,
    CLCBFileWrite,
    CLCBFileTell,
    CLCBFileSetEOF,
    CLCBFileEOF,
    CLCBFileGetAttr,
    CLCBFileSetAttr,
    CLCBFileGetDateTime,
    CLCBFileSetDateTime,
    CLCBFileFullNameAndPath,
    CLCBFileName,
    CLCBFilePath,
    CLCBFileLogicalDrive,
    CLCBFileLength,
    CLCBFileDelete,
    CLCBFileRename,
    CLCBFileGetErrorCondition,
};

GENERALCALLBACKS_T gstCLCBGeneralCallBacks =
{
    NULL,
    &gstCLCBFileCallBacks,
    CLCBMemoryAlloc,
    CLCBMemoryFree,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

PROGRESSCALLBACKS_T gstCLCBProgressCallBacks =
{
    CLCBProgressInit,
    CLCBProgressUpdate,
    CLCBProgressClose,
    NULL
};

DATAFILECALLBACKS_T gstCLCBDataFileCallBacks =
{
	&gstCLCBFileCallBacks,
    CLCBGetDataFileInfo,
    CLCBCloseDataFileInfo
};

IPCCALLBACKS_T gstCLCBIPCCallBacks =
{
    CLCBMutexCreate,
    CLCBMutexDestroy,
    CLCBMutexWait,
    CLCBMutexRelease,
    CLCBSemaphoreCreate,
    CLCBSemaphoreDestroy,
    CLCBSemaphoreWait,
    CLCBSemaphoreRelease
};

MEMORYCALLBACKS_T gstCLCBMemoryCallBacks =
{
    NULL,
    NULL,
    CLCBMemGetPtrToMem,
    CLCBMemReleasePtrToMem
};

LINEARCALLBACKS gstCLCBLinearCallBacks =
{
    CLCBLinearOpen,
    CLCBLinearClose,
    CLCBLinearRead,
    CLCBLinearWrite,
    CLCBLinearGetDimensions,
    CLCBLinearConvertFromPhysical,
    CLCBLinearConvertToPhysical,
    CLCBLinearIsPartitionedMedia
};
