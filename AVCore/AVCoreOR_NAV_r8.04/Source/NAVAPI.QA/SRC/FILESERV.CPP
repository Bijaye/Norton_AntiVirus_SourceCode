// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI.QA/VCS/fileserv.cpv   1.1   18 Aug 1998 17:04:30   mmendon  $
//
// Description: Implementation of CFileServer class.
//
//************************************************************************
// $Log:   S:/NAVAPI.QA/VCS/fileserv.cpv  $
// 
//    Rev 1.1   18 Aug 1998 17:04:30   mmendon
// Added MTRequestFile for multithreaded use
// 
//    Rev 1.0   05 Aug 1998 15:19:16   mmendon
// Initial revision.
//
//    Rev 1.0   04 Aug 1998 11:21:58   NMILLER
// Initial revision.
//
//************************************************************************

#include "FileServ.h"
#include "file.h"


// Local function prototypes

void PathAppendName(
    LPTSTR lpszFullPath,
    LPTSTR lpszFileName);


// CFileServer public and private implementations


CFileServer::CFileServer()
{
	dwCount = 0;
	dwCurrentFile = 0;
    for (int i = 0; i < FILE_ARRAY_MAX_SIZE; i++)
            lpFilenameArray[i] = NULL;
}



BOOL CFileServer::ServerInit(LPTSTR lpszRootDirectory)
{
    int i;

#ifdef SYM_WIN32
    try
    {   lpLog = new CLog(FILE_SERVER_LOGFILE);
        for (i = 0; i < FILE_ARRAY_MAX_SIZE; i++)
        {
            lpFilenameArray[i] = new TCHAR[SYM_MAX_PATH];
            *lpFilenameArray[i] = NULL;
        }
    }
    catch(...)
    {
           for (int j = 0; j < i; j++)
        {
             if(lpFilenameArray[j])
                delete [] ((LPTSTR) lpFilenameArray[j]);
        }
        if(lpLog)
            delete lpLog;
        return(FALSE);
    }
#endif

#ifndef SYM_WIN32

        BOOL bMemAllocErr = FALSE;

        lpLog = new CLog(FILE_SERVER_LOGFILE);
        if(lpLog == NULL)
            return(FALSE);

        for (i = 0; i < FILE_ARRAY_MAX_SIZE; i++)
        {
            lpFilenameArray[i] = new TCHAR[SYM_MAX_PATH];
            if(lpFilenameArray[i] == NULL)
            {
                bMemAllocErr = TRUE;
                break;
            }
            *lpFilenameArray[i] = NULL;
        }

        if(bMemAllocErr)
        {
            for (int j = 0; j < i; j++)
              if(lpFilenameArray[j])
                  delete [] ((LPTSTR) lpFilenameArray[j]);
            if(lpLog)
                delete lpLog;
            return(FALSE);
        }

#endif

    return(ConstructList(lpszRootDirectory));

}

CFileServer::~CFileServer()
{

	for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
        if(lpFilenameArray[dwIndex])
            delete [] ((LPTSTR) lpFilenameArray[dwIndex]);

    if(lpLog)
        delete lpLog;
}

LPTSTR CFileServer::RequestFile(BOOL bNewFile)
{
    LPTSTR lpszFileToReturn = new TCHAR[SYM_MAX_PATH];

	if (!bNewFile)
	{
		// copy in current file
        STRCPY(lpszFileToReturn, lpFilenameArray[dwCurrentFile]); //
	}
	else
	{
		if (++dwCurrentFile < dwCount)	// can we get another file?
		{
            STRCPY(lpszFileToReturn, lpFilenameArray[dwCurrentFile]);
		}
		else							// we've reached the end of the list
		{
			delete [] lpszFileToReturn;
			lpszFileToReturn = NULL;
		}
	}

	return lpszFileToReturn;
}


#ifdef SYM_WIN32

LPTSTR CFileServer::MTRequestFile()
{
    LPTSTR lpszFileToReturn = new TCHAR[SYM_MAX_PATH];

    //updating member var dwCurrentFile
    //wrap in critical section.  Locks until function exits
    cCriticalSectionLock csl(&CritSection);

    if(dwCurrentFile < dwCount)
    {
        // copy in current file
        STRCPY(lpszFileToReturn, lpFilenameArray[dwCurrentFile]); //
        ++dwCurrentFile;
    }

   else                            // we've reached the end of the list
   {
        delete [] lpszFileToReturn;
        lpszFileToReturn = NULL;
   }
    return lpszFileToReturn;
}

#endif


BOOL CFileServer::Log(LPCTSTR lpszFileName)
{
	lpLog->LogEvent(lpszFileName);
	return TRUE;
}


BOOL CFileServer::ConstructList(LPCTSTR lpszPath)
{
    FINDDATA        rFindData;
    HFIND           hFindHandle;
	BOOL            bContinue = TRUE;
	BOOL            bReturn = TRUE;

    TCHAR szFileName[SYM_MAX_PATH];

                                        // Take our local copy first.
    STRCPY(
        szFileName,
        lpszPath);

                                        // Add the wildcard character to the
                                        // filename
    PathAppendName(
        szFileName,
        _T("*.*"));

    hFindHandle = FileFindFirst(szFileName, &rFindData);

    if ( ((HFIND)INVALID_HANDLE_VALUE) == hFindHandle)
        goto Cleanup;

                                        // Make sure that we are to continue,
                                        // and that we are still successful.
    while (bContinue && bReturn)
    {
                                        // Construct the file to delete

        STRCPY (
            szFileName,
            lpszPath);

        PathAppendName(
            szFileName,
            rFindData.szFileName);

                                        // Check to see if it is a directory,
                                        // and if it is, recurse.
        if ( (FA_DIR & rFindData.dwFileAttributes) ==
              FA_DIR)
		{
                                        // Don't do anything with the "DOTS"
            if ( STRCMP(rFindData.szFileName, _T(".") ) &&
                 STRCMP(rFindData.szFileName, _T("..") ) )

                bReturn = ConstructList(szFileName);
		}
        else
        {
            STRCPY( lpFilenameArray[dwCount], szFileName);
            dwCount++;
			Log(szFileName);
        }

        bContinue = FileFindNext(
                        hFindHandle,
                        &rFindData);
	}

Cleanup:
                                        // Close the handle.
    if (INVALID_HANDLE_VALUE != hFindHandle)
        {
        FileFindClose(
            hFindHandle);

        hFindHandle = (HFIND)INVALID_HANDLE_VALUE;
        }

    return bReturn;
}






/////////////////////////////////////////////////////////////////////
// void PathAppendName(
//
// Purpose:
//    This function will take a path and a filename and append the
//    filename to the path adding a backslash if neccessary.
//
// Parameters:
//    LPTSTR lpszFullPath - [in/out] - Starting path and also full
//                                     filename upon exit
//    LPTSTR lpszFileName - [in]     - Filename to append.
//
// Returns:
//    none.
//
// Notes:
//    It is ASSUMED that there is enough space in lpszFullPath
//    for the full path and any filename appended to it.
//
/////////////////////////////////////////////////////////////////////
// 01-05-98 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void PathAppendName(
    LPTSTR lpszFullPath,
    LPTSTR lpszFileName)
{
    LPTSTR  lpszPtr;
    BOOL    bBackSlash = FALSE;

    lpszPtr = lpszFullPath;

    while (*lpszPtr)
        {
        bBackSlash = (_T('\\') == *lpszPtr);

        lpszPtr = AnsiNext(lpszPtr);
        }

    if (!bBackSlash)
        {
        STRCAT(
            lpszFullPath,
            _T("\\"));
        }

    STRCAT(
        lpszFullPath,
        lpszFileName);
}

