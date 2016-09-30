////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// OfficeAntiVirus.cpp : Implementation of COfficeAntiVirus
//
#include "stdafx.h"
#include "OfficeAV_h.h"
#include "OfficeAntiVirus.h"

#include "NAVOptionNames.h"

#include "ccTrace.h"
#include "ccSymInstalledApps.h"
#include "ccVerifyTrustLoader.h"
#include "ccResourceLoader.h"

#include "isVersion.h"

using namespace ccVerifyTrust;

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

/////////////////////////////////////////////////////////////////////////////
// Constructor and Destructor
//
COfficeAntiVirus::COfficeAntiVirus()
{
}

COfficeAntiVirus::~COfficeAntiVirus()
{
}

/////////////////////////////////////////////////////////////////////////////
// IOfficeAntiVirus method
//
STDMETHODIMP COfficeAntiVirus::Scan(MSOAVINFO* psoavinfo)
{
	WCHAR       szTempFile[MAX_PATH] = {0};
	WCHAR       szShortPath[MAX_PATH] = {0};
	LPCWSTR     lpcszFullPath = NULL;
    LPCWSTR     lpcwszTempFile = NULL;
    LPSTORAGE   lpSource = NULL;
    LPSTORAGE   lpDest = NULL;
    STATSTG     stats;
        
    // Sanity check.    
    if (!psoavinfo)
        return E_UNEXPECTED;
       
	// Make sure the structure looks like the one we are expecting.
	//
	// Office 2007 B2TR added a new field called "LPSTORAGE lpstgVBA" at the
	// end of MSOAVINFO structure. This variable is a pointer to OLE storage
	// of the macros which is not used in NAV 07 and previous versions.
	// This addition increases the size of MSOAVINFO by 4 bytes.
	// To support this change, we will accept versions of MSOAVINFO as 
	// long as default fields of the the structure remain. 
	//
	if (psoavinfo->cbSize < sizeof (MSOAVINFO))
		return E_UNEXPECTED;

    if (psoavinfo->f.fPath)
    {
        // Use pwzFullPath.
		lpcszFullPath = psoavinfo->u.pwzFullPath;
        
        // Attempt to access the file.  If we cannot access it, try OEM mode.
        if(GetFileAttributes( lpcszFullPath ) == INVALID_FILE_ATTRIBUTES)
        {
			// Fetch short path
			if(GetShortPathNameW( psoavinfo->u.pwzFullPath, szShortPath, MAX_PATH ) == 0) 
			{
				CCTRCTXE1(L"Could not convert %S to short path name", psoavinfo->u.pwzFullPath);
				return E_UNEXPECTED;
			}

			lpcszFullPath = szShortPath;
        }
    }
    else
    {
        // Use lpStg (an IStorage pointer).
        // Here we need to manufacture a temporary document file that we can
        // dump this IStorage into so that Scan Manager can do its work on a
        // regular file instead of dealing with an IStorage.
        
        lpSource = psoavinfo->u.lpStg;
        if (!lpSource)
            return E_UNEXPECTED;
        
        // Create a temporary file to dump the IStorage into.
        
        TCHAR szTempPath[MAX_PATH];
        
        if ((GetTempPath(sizeof(szTempPath), szTempPath)) &&
            (GetTempFileName(szTempPath, _T("NAV"), 0, szTempFile)))
        {
            // Create a new document file where the temporary file
            // is now sitting.
            
            lpcwszTempFile = T2CW(szTempFile);
            if (StgCreateDocfile (lpcwszTempFile,
                STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
                0,
                &lpDest) == S_OK)
            {
                if (lpSource->CopyTo (NULL, NULL, NULL, lpDest) == S_OK)
                {
                    // Make sure the temporary object has the same class as
                    // the source.
                    // NOTE: As far as I can tell the SetClass call below
                    //       has absolutely no effect on the repair process.
                    //       I added this in hopes that it would correct the
                    //       problem where the icon associated with the
                    //       package gets reset to point to SHELL32.DLL's
                    //       default icon after a repair is made.
                    //       This may just be the way the AV interface is
                    //       intended to work when repairs are made
                    //       (i.e. the original icon information is thrown
                    //       out because the AV interface has no way of
                    //       knowing what we have done to the object.)
                    //       This is only a guess though.  It could actually
                    //       be a bug that Microsoft would need to deal with.
                    if (lpSource->Stat(&stats, STATFLAG_NONAME) == S_OK)
                        lpDest->SetClass (stats.clsid);
                    
                    // OK, successfully copied the source storage object into
                    // the destination object.  
                    
                    lpcszFullPath = szTempFile;
                }

                // Now release the temporary object
                // so that Scan Manager can work on it.
                
                lpDest->Release();  // This also commits the temporary object file to disk.
                lpDest = NULL;

            }
            
            // If we were unable to dump the IStream into our temp file,
            // delete the temp file and bail.
            
            if (lpcszFullPath == NULL)
            {
                if (lpcwszTempFile)
                    SafeDeleteFile(szTempFile);
                
                return E_UNEXPECTED;
            }
        }
    }
    
    // At this point we should have a file to scan.
    
    if (lpcszFullPath == NULL)
        return E_UNEXPECTED;
    
    // Scan the file.
    
	CCTRCTXI1(L"Scanning %s...", lpcszFullPath);

    NAVWRESULT Result = ScanFile(lpcszFullPath);
    
    // If we the scan succeeded, find out what happened.
	CCTRCTXI1(L"Result: %d", Result);
        
    // Did the scan fail?
    
    if (Result == NAVWRESULT_ERROR)
    {
		CCTRCTXE0(L"Scan failed");
        
        if (lpcwszTempFile)
            SafeDeleteFile(szTempFile);
        
		// Etrack incident 764267:
		// Return S_OK will cause the standard macro warning dialogs 
		// to be displayed by Office, which is the same as if 
		// we didn't scan the file.
        return S_OK;
        
    }
    
    // If no infections were found, we are done
    
    if (Result == NAVWRESULT_NOINFECTIONS)
    {
		CCTRCTXI0(L"No infections were found");

        if (lpcwszTempFile)
            SafeDeleteFile(szTempFile);
        
        return S_OK;
    }
    
    // Take action based on what happened during the scan.
    
    HRESULT hResult = S_FALSE;
    
    if (Result == NAVWRESULT_ALLINFECTIONS_CLEAN)
    {
		CCTRCTXI0(L"All infections were found and repaired");

        // All infections were found and repaired.
        // First, though, see if we need to deal with transferring the repaired
        // file back into an IStorage.
        
        if (!psoavinfo->f.fPath && lpcwszTempFile)
        {
            // Yep, we need to transfer the repaired document back into the
            // source document (still pointed to by lpSource).
            
            if (StgOpenStorage (lpcwszTempFile,
                NULL,
                STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
                NULL,
                0,
                &lpDest) == S_OK)
            {
                if (lpDest->CopyTo (NULL, NULL, NULL, lpSource) != S_OK)
                {
                    // Unable to copy the repaired file back into place.
                    
                    if (lpcwszTempFile)
                        SafeDeleteFile(szTempFile);
                    
                    return E_FAIL;
                }
                
                // Release the temporary object.
                lpDest->Release();
                lpDest = NULL;
            }
            
        }
    }
    else if(Result == NAVWRESULT_INFECTIONS_REMAIN)
    {
		CCTRCTXI0(L"The file is still infected");

        // The file is still infected!
        
        hResult = E_FAIL;
        
        if (psoavinfo->f.fPath != NULL)
        {
            // Give the user the option to delete the (container) file.
            WCHAR szFormat[MAX_PATH];
            if(g_ResLoader.LoadString(IDS_DELETE_CONTAINER, szFormat, CCDIMOF(szFormat)) == false) 
			{
                CCTRCTXE1(L"Could not load resource IDS_DELETE_CONTAINER(%d)", 
                         IDS_DELETE_CONTAINER);

                // If we created a temporary file, delete it.

                if (lpcwszTempFile)
                    SafeDeleteFile(szTempFile);

                return E_UNEXPECTED;
            }

            WCHAR szMessage[MAX_PATH * 2];
            _snwprintf(szMessage, sizeof(szMessage), szFormat, lpcszFullPath);
            
            if (MessageBox(NULL, szMessage, CISVersion::GetProductName(), MB_YESNO | MB_SETFOREGROUND) == IDYES)
            {
                if (SafeDeleteFile(lpcszFullPath) == true)
                {
                    hResult = S_FALSE;
                }
            }
        }
    }
    
    // If we created a temporary file, delete it.
    
    if (lpcwszTempFile)
        SafeDeleteFile(szTempFile);
    
    return hResult;
}

bool COfficeAntiVirus::SafeDeleteFile(LPCWSTR szFileName)
{
    HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL,
        TRUNCATE_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE)
        return false;
    
    CloseHandle(hFile);
    
    return true;
}

NAVWRESULT COfficeAntiVirus::ScanFile(LPCWSTR szFileName)
{
    // Build the command line string to do the scan
    
    ccLib::CString szNAVDir;
    //DWORD dwNAVDirSize = MAX_PATH;
    //ccSym::CInstalledApps Apps;

    if ((ccSym::CInstalledApps::GetNAVDirectory(szNAVDir))==false)
        return NAVWRESULT_ERROR;

    TCHAR szCmdLine[MAX_PATH];
    wnsprintf(szCmdLine, sizeof(szCmdLine), _T("\"%s\\navw32.exe\" /OFFICEAV \"%s\""), szNAVDir, szFileName);
    
    TCHAR szPath[MAX_PATH];
    wnsprintf(szPath, sizeof(szPath), _T("%s\\navw32.exe"), szNAVDir);

    // Verify that navw32.exe is Symantec signed
    IVerifyTrustPtr pVerifyTrust;

    if( SYM_SUCCEEDED(ccVerifyTrustMgd_IVerifyTrust::CreateObject(GETMODULEMGR(),&pVerifyTrust)) && 
        (pVerifyTrust->Create(true) == eNoError) )
    {
        if(eVerifyError == pVerifyTrust->VerifyFile(szPath, eSymantecSignature))
        {
            CCTRACEE(_T("Navw32.exe is not signed. Failed to scan file.\n"));
            return NAVWRESULT_ERROR;
        }
    }

	CCTRCTXI1(L"Launching %s", szCmdLine);

    // Create a process for NAVW and scan the file
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if(!CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        return NAVWRESULT_ERROR; // If CreateProcess Fails, Error out
    
    // Wait for the process to exit
    SymMsgWaitSingle(pi.hThread, INFINITE);
    
    // Get the return code from NAVW and return that value
    DWORD dwExitCode = NAVWRESULT_ERROR;
    if(!GetExitCodeProcess(pi.hProcess, &dwExitCode))
        dwExitCode = NAVWRESULT_ERROR;
    
	CCTRCTXI1(L"Scan Exit code: %d", dwExitCode);

    // Clean up
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return (NAVWRESULT)dwExitCode;
}


// Port from CoreOR MSGWAIT.CPP


// Copyright 1996 Symantec, Peter Norton Computing Group                  
////////////////////////////////////////////////////////////////////////  
// MSGWAIT.CPP
//   
// $Header:   S:/SYMKRNL/VCS/msgwait.cpv   1.2   27 Nov 1996 10:38:28   MSPIEGE  $ 
//   
// Description:
//   
//   This file contains wait functions that are safe for use in OLE/COM
//   threads.  The standard WaitForSingle/MultipleObject() calls cause
//   problems in OLE threads because CoInitialize() creates a hidden
//   window.  If messages get sent or otherwise broadcast to that window,
//   they have to be processed or the system will hang.  
//
//   
//   
// Contains:
//   SymMsgWaitSingle()
//   SymMsgWaitMultiple()
//   
// See Also:
//   KB Article: OLE Threads Must Dispatch Messages
//   MsgWaitForMultipleObjects()
////////////////////////////////////////////////////////////////////////  
// $Log:   S:/SYMKRNL/VCS/msgwait.cpv  $ 
// 
//    Rev 1.2   27 Nov 1996 10:38:28   MSPIEGE
// 1) Changed the way the SymMsgWaitMultiple() function treats its timeout
//    value to more closely approximate MsgWaitMultiple()
// 2) Removed the message loop and replaced it with a single call to 
//    PeekMessage() so that only SendMessage() messages will get processed.
//    See MSDN, Books .../Advanced Windows NT/Chapter 6/Thread Info...
// 
//    Rev 1.1   18 Sep 1996 14:52:56   BMCCORK
// Convered SymMsgWaitMultiple to return MsgWaitForMultipleObjects return codes
// 
//    Rev 1.0   17 Sep 1996 18:39:02   BMCCORK
// Initial revision.
////////////////////////////////////////////////////////////////////////  

//#include "OfficeAntiVirus.h"

//#ifdef SYM_WIN32                        // Only supported under Win32 environments

////////////////////////////////////////////////////////////////////////  
//   
//  SymMsgWaitMultiple()
//   
// Description:
//   
// Wiat for one of the objects given to be signaled, while allowing 
// messages to be dispatched.
// 
// Returns:
//     Returns the same values as MsgWaitForMultipleObjects() with fWaitAll = FALSE.
//     Will not return WAIT_OBJECT_0 + dwCount, since this function will perform a PeekMessage loop each
//     time there is input in the queue.
//   
// Return Values
//  If the function succeeds, the return value indicates the event that caused the function to return. 
//  The successful return value is one of the following: 
//  Value                                               Meaning
//  WAIT_OBJECT_0 to (WAIT_OBJECT_0 + nCount - 1)       The return value minus WAIT_OBJECT_0 indicates the 
//                                                      lpHandles array index of the object that satisfied the wait. 
//  WAIT_ABANDONED_0 to (WAIT_ABANDONED_0 + nCount - 1) The return value minus WAIT_ABANDONED_0 indicates the lpHandles 
//                                                      array index of an abandoned mutex object that satisfied the wait.
//  WAIT_TIMEOUT                                        The time-out interval elapsed and no objects were signaled.
//  
// If the function fails, the return valueis 0xFFFFFFFF. To get extended error information, call GetLastError. 
//
// See Also:
//   MsgWaitForMultipleObjects()
//   SymMsgWaitSingle() inline/macro
//   SYM_WAIT_XXX() and SYM_WAIT_GET_XXX() inline/macro helpers
//   
////////////////////////////////////////////////////////////////////////  
//  9/17/96 BEM Function created. 
////////////////////////////////////////////////////////////////////////  

// Wait for one of the given objects to be signaled, while allowing messages to be dispatched:
// Returns same as MsgWaitForMultipleObjects()
DWORD WINAPI SymMsgWaitMultiple(
    DWORD dwCount,                      // Number of handles in lpHandles
    LPHANDLE lpHandles,                 // Array of handles - wiat for a signal from 
                                        // one of these. 
    DWORD dwMilliseconds)               // Timeout value (use INFINITE for none) 
{
    //MWS CODE CHANGE...11/25/96  This comment covers all individual changes.
    //Made the following modifications to this function.  First, it now manages
    //  the remaining timeout value when a message is received so that if a lot
    //  of messages are coming in, the function will stay with the spirit of 
    //  the original timeout.
    //  Second, there is no longer a message loop.  The only messages that we 
    //  are really concerned with here are messages received as a result of a 
    //  call to SendMessage().  Turns out that merely calling PeekMessage 
    //  causes these messages to be dispatched to the appropriate winproc() 
    //  without any further work on our part.  Also, by not processing messages
    //  that are not necessary to process, we are lowering the danger of 
    //  problems calling our code in a reentrant manner.  (Symdb at least!)  
    DWORD       dwRet;
    MSG         msg;
    DWORD       dwWaitRemaining = dwMilliseconds;
    __int64     i64StartTime = (__int64)GetTickCount();
    __int64     i64NowTime;
    
    while(TRUE)
        {
        dwRet = MsgWaitForMultipleObjects( 
            dwCount,                    // Number of events to wait for
            lpHandles,                  // The array of events
            FALSE,                      // Don't wait for all events since we need to 
                                        // know as soon as there are messages.
            dwWaitRemaining,            // Timeout value
            QS_ALLINPUT);               // Any message wakes up


        //Check and act on the return of the wait function.
        if (dwRet == WAIT_OBJECT_0 + dwCount)
            {
            //There is a window message available.  Note that due to the way
            //  that Win95 and WinNT appear to work, merely making the call
            //  to PeekMessage() causes a message created with SendMessage()
            //  to be dispatched to the appropriate winproc().  Since the real
            //  problem we are trying to address has to deal with messages
            //  recieved as a result of a call to SendMessage() we don't have 
            //  to do any more work.
            PeekMessage(&msg,NULL,(UINT)0,(UINT)0,PM_NOREMOVE);
            } 
        else 
            {
            // An event was signaled, wait was abandoned, or timeout
            return dwRet;
            } 

        //If the timeout is infinite, we can continue right away.  All the 
        //  subsequent tests and calc are meaningless.
        if (dwWaitRemaining == INFINITE)
            continue;

        //If dwWaitRemaining is 0, either the user was just testing the objects
        //  or we have already been here and the timeout has expired, but 
        //  someone has managed to stick another message in our queue while we 
        //  were deciding that the dwMilliseconds value should be 0 and we 
        //  retried the wait.  If someone is pounding us hard enough with 
        //  messages, this could happen not letting the  WaitForMultipleObjects
        //  call return timeout for an extended  period, not letting this 
        //  function return.  This is unlikely but possible, so we'll break and
        //  return WAIT_TIMEOUT forcing ourselves out to the caller.
        if (dwWaitRemaining == (DWORD)0)
            return WAIT_TIMEOUT;

        //Get the current tick count being careful to watch for a rollover.
        i64NowTime = (__int64)GetTickCount();
        if (i64NowTime < i64StartTime)
            i64NowTime += 0x100000000;

        //Recalculate a new time out value based on how much of the existing
        //  timeout was used.  If the timeout is exhausted, it may have happened
        //  due to the processing of messages so set it to 0 and try the 
        //  wait one more time.  That will give us the handles we are waiting on
        //  a fair chance at being signalled or abandoned.  
        if (i64NowTime >= i64StartTime + dwMilliseconds)
            {
            dwWaitRemaining = 0;
            }
        else
            {
            //NOTE: The right hand side of the subtraction can never be larger 
            //  than dwMilliseconds due to the inequality that gets us here.
            dwWaitRemaining = dwMilliseconds - (DWORD)(i64NowTime - i64StartTime);           
            }
        } 
}