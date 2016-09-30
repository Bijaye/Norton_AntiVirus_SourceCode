// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/clnavex.cpv   1.1   27 Aug 1996 15:32:38   DCHI  $
//
// Description:
//  NAVEX interface.
//
// Contains:
//  CMDLineNAVEXInit()
//  CMDLineNAVEXClose()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/clnavex.cpv  $
// 
//    Rev 1.1   27 Aug 1996 15:32:38   DCHI
// #ifdef'd out SYM_UNIX NAVEX callbacks.
// 
//    Rev 1.0   27 Aug 1996 15:19:36   DCHI
// Initial revision.
// 
//************************************************************************

#ifdef SYM_UNIX

//#include <dlfcn.h>
//#include <unistd.h>

#endif // SYM_UNIX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanglob.h"

#include "clnavex.h"

//////////////////////////////////////////////////////////////////////
//
// NAVEX callback function definitions
//
//////////////////////////////////////////////////////////////////////

#ifdef SYM_UNIX

// Note: The Unix version of SYMKRNL now has new & improved
// versions of most of these functions.

HFILE NavexFileOpen
(
    LPSTR       lpName,
    UINT        uOpenMode
)
{
	char    s[10];
	HFILE	temp;

    if (uOpenMode == 0)
        strcpy(s,"rb");
    if (uOpenMode & 1)
        strcpy(s,"wb");
    if (uOpenMode & 2)
        strcpy(s,"rb+");

	temp = fopen((const char *)lpName,s);
	if (temp == NULL)
		return((HFILE)-1);

	return(temp);
}

HFILE NavexFileClose
(
    HFILE       hHandle
)
{
    fclose(hHandle);
    return(hHandle);
}

UINT NavexFileRead
(
    HFILE       hHandle,
    LPVOID      lpBuffer,
    UINT        uBytes
)
{
    UINT uRead;

    uRead = fread(lpBuffer,1,uBytes,hHandle);

    return(uRead);
}

UINT NavexFileWrite
(
    HFILE       hHandle,
    LPVOID      lpBuffer,
    UINT        uBytes
)
{
	UINT uWrite;

	if (uBytes == 0)
	{
        // Truncate the file

#if defined(VC20) || defined(SYM_WIN32)

        if (SetEndOfFile(hHandle) == FALSE)
            return((UINT)-1);

#else

        WORD        wValue;

        wValue = fileno(hHandle);

#ifdef BORLAND
        _BX = wValue;
        _AH = 0x40;
        _CX = 0;
        geninterrupt(0x21);

        if (_AX != 0)
            return((UINT)-1);
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
            return((UINT)-1);
#endif // SYM_DOS || SYM_DOSX

#ifdef SYM_UNIX
        int result;
        if (ftruncate(wValue,ftell((FILE *)hHandle)) != 0)
            return((UINT)-1);
#endif SYM_UNIX

#endif

        return(0);
	}

	uWrite = fwrite(lpBuffer,1,uBytes,hHandle);
	if (uWrite != uBytes)
		return((UINT)-1);

    return(uWrite);
}

DWORD NavexFileSeek
(
    HFILE       hHandle,
    LONG        dwOffset,
    int         nFlag
)
{
    if (!fseek(hHandle,dwOffset,nFlag))
        return(ftell(hHandle));

    return((DWORD)-1);
}

BOOL NavexFileGetDateTime
(
    HFILE       hHandle,
    UINT        uType,
    WORD FAR *  lpuDate,
    WORD FAR *  lpuTime
)
{
    // Code required

    return(TRUE);
}

BOOL NavexFileSetDateTime
(
    HFILE       hHandle,
    UINT        uType,
    WORD        uDate,
    WORD        uTime
)
{
    // Code required

    return(TRUE);
}

DWORD NavexFileSize(HFILE hHandle)
{
	DWORD dwOrigOffset, dwNewOffset;

	dwOrigOffset = ftell(hHandle);
	fseek(hHandle,0,SEEK_END);
	dwNewOffset = ftell(hHandle);
	fseek(hHandle,dwOrigOffset,SEEK_SET);
	return(dwNewOffset);
}

BOOL NavexFileDelete
(
    LPSTR       lpFileName
)
{
    return(!remove((const char *)lpFileName));        /* 1 on success, 0 on failure */
}

HFILE NavexFileCreate
(
    LPSTR       lpFileName,
    UINT        uAttr
)
{
    HFILE temp;

	temp = fopen((const char *)lpFileName,"wb");
    if (NULL == temp)
        return(NULL);
    fclose(temp);

    temp = FileOpen(lpFileName,uAttr);

	if (NULL == temp)
		return((HFILE)-1);
	else
		return(temp);
}

UINT NavexFileGetAttr
(
    LPTSTR      lpFilename,
    UINT FAR *  lpuAttr
)
{
    // Code required

    return(TRUE);
}

UINT NavexFileSetAttr
(
    LPTSTR      lpFilename,
    UINT        uAttr
)
{
    // Code required

    return(TRUE);
}


CALLBACKREV1 gstNAVEXCallBacks =
{
    NavexFileOpen,
    NavexFileClose,
    NavexFileRead,
    NavexFileWrite,
    NavexFileSeek,
    NavexFileSize,
    NavexFileGetDateTime,
    NavexFileSetDateTime,
    NavexFileDelete,
    NavexFileGetAttr,
    NavexFileSetAttr,
    NavexFileCreate,
/*
    NULL,               // FileLock,
    NULL,               // FileUnlock,
    NULL,               // Progress,

    NULL,               // SDiskReadPhysical,
    NULL,               // SDiskWritePhysical,

    NULL,               // TempMemoryAlloc,
    NULL,               // TempMemoryFree,
    NULL,               // PermMemoryAlloc,
    NULL                // PermMemoryFree
*/
};

#endif // #ifdef SYM_UNIX

//*************************************************************************
//
// Function:
//  CMDLineNAVEXInit()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//
// Description:
//  Initializes system for NAVEX.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL CMDLineNAVEXInit
(
	LPSCANGLOBAL	lpScanGlobal
)
{
#if 0 // was: #ifdef SYM_UNIX

    void *hNAVEXSharedObject;
    char szPathName[SYM_MAX_PATH+1];

    // Initialize to uninitialized state

    lpScanGlobal->hNAVEXSharedObject = NULL;

    // Get the path name of the NAVEX shared object

    sprintf(szPathName,"%snavex.so",lpScanGlobal->szHomeDir);

    // Open the shared object

    hNAVEXSharedObject = dlopen(szPathName,RTLD_LAZY);
    if (hNAVEXSharedObject == NULL)
    {
        // Failed to open dynamic link to NAVEX

        return(FALSE);
    }

    //////////////////////////////////////////////////////////////////
    // Load the symbols
    //////////////////////////////////////////////////////////////////

    // EXTScanFile

    lpScanGlobal->stNAVEXFunctionTable.EXTScanFile =
        (EXTSTATUS (*)(LPCALLBACKREV1,
                       LPTSTR,
                       HFILE,
                       LPBYTE,
                       LPBYTE,
                       WORD,
                       LPWORD))dlsym(hNAVEXSharedObject,"EXTScanFile");

    if (lpScanGlobal->stNAVEXFunctionTable.EXTScanFile == NULL)
    {
        // Failed to get link for EXTScanFile()

        dlclose(hNAVEXSharedObject);

        return(FALSE);
    }

    // EXTRepairFile

    lpScanGlobal->stNAVEXFunctionTable.EXTRepairFile =
        (EXTSTATUS (*)(LPCALLBACKREV1,
                       WORD,
                       LPN30,
                       LPTSTR,
                       LPBYTE))dlsym(hNAVEXSharedObject,"EXTRepairFile");

    if (lpScanGlobal->stNAVEXFunctionTable.EXTRepairFile == NULL)
    {
        // Failed to get link for EXTRepairFile()

        dlclose(hNAVEXSharedObject);

        return(FALSE);
    }

    // Remember the handle to the shared object

    lpScanGlobal->hNAVEXSharedObject = hNAVEXSharedObject;
    lpScanGlobal->lpNAVEXCallBacks = &gstNAVEXCallBacks;

#endif // SYM_UNIX

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  CMDLineNAVEXClose()
//
// Parameters:
//  lpScanGlobal    Pointer to global data
//
// Description:
//  Closes down NAVEX system.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL CMDLineNAVEXClose
(
	LPSCANGLOBAL	lpScanGlobal
)
{
#if 0 // was: #ifdef SYM_UNIX

    if (lpScanGlobal->hNAVEXSharedObject != NULL)
    {
        if (dlclose(lpScanGlobal->hNAVEXSharedObject) != 0)
        {
            // Failed to close it

            return(FALSE);
        }
    }

#endif // SYM_UNIX

    return(TRUE);
}

