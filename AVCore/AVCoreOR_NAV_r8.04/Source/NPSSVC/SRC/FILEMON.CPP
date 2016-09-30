/////////////////////////////////////////////////////////////////////
// FILEMON.CPP - Created - 04/21/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NPSSVC/VCS/FILEMON.CPv   1.3   14 Apr 1998 13:01:58   mdunn  $
//
// Description:
//   This file will house the file monitor class that will be used
//   top monitor files for change.  The application of this class
//   will be such things as watching definition files for change, or
//   watching critical data files for modifications.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NPSSVC/VCS/FILEMON.CPv  $
// 
//    Rev 1.3   14 Apr 1998 13:01:58   mdunn
// Disabled optimization for this file - MSVC 5's optimization was making
// the service crash on startup.
//
//    Rev 1.2   16 Jan 1998 15:53:32   KSACKIN
// Updated the file monitor to exit it's blocking loop if there was a critical
// error in blocking on the watched file.  Previously, it would get into a
// loop of breaking out and trying to start the wait again.
//
//    Rev 1.1   11 Aug 1997 23:32:24   ksackin
// Set the initial value of a return value to TRUE.  Previously, we had an uninitialized
// return value that would get returned as is if all was successfull.  A failure would have
// worked correctly, but sucecss returned an uninitialized variable.
//
//    Rev 1.0   19 May 1997 00:03:12   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Include Files.

#include "platform.h"
#include "tchar.h"
#include "process.h"
#include "FileMon.h"



										// Maximum thread size.
#define  MAX_THREAD_STACK_SIZE  10240

#pragma optimize("", off)

/////////////////////////////////////////////////////////////////////
// CFileMonitor Class.


/////////////////////////////////////////////////////////////////////
// CFileMonitor::CFileMonitor( void )
//
// Purpose:
//    Constructor of the CFileMonitor class.
//
// Parameters:
//    None.
//
// Returns:
//    A constructed CFileMonitor class.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/21/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
CFileMonitor::CFileMonitor( void )
{
                                        // Initialize function callback.
    m_fnFileChangeCallback = NULL;

                                        // Initialize class members.
    m_hWatcherThread = NULL;

                                        // Create our kill event for when
                                        // this object needs to be destroyed.

	m_hKillWaitEvent = CreateEvent( NULL,  // Security Attributes
                                    FALSE, // Manual Reset
                                    FALSE, // Initial State
                                    NULL );

                                        // Reset our event to make sure
                                        // that it is initialized.
//    ResetEvent( m_hKillWaitEvent );
}

/////////////////////////////////////////////////////////////////////
// CFileMonitor::~CFileMonitor( void )
//
// Purpose:
//    Destructor of the CFileMonitor class.
//
// Parameters:
//    None.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/21/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
CFileMonitor::~CFileMonitor( void )
{
DWORD dwRetVal;
                                        // This event will tell all threads
                                        // that are waiting for change events
                                        // to stop waiting and shut down.
    PulseEvent( m_hKillWaitEvent );

                                        // Wait for the thread to terminate
                                        // before continuing with the
                                        // destruction.  If this does not occur
                                        // within the time allotted, then we will
                                        // kill the thread.
    if ( NULL != m_hWatcherThread )
        {
                                        // Wait for the thread to terminate.
                                        // This is merely a grace period before
                                        // we rip out this object.  This should
                                        // never really be needed.  This should
                                        // happen almost immediately, unless
                                        // the thread is currently processing a
                                        // request.
        dwRetVal = WaitForSingleObject( m_hWatcherThread, 3000 );
        }
}

/////////////////////////////////////////////////////////////////////
// void CFileMonitor::SetFolderName( LPCTSTR szFolderName )
//
// Purpose:
//    To set the folder name to watch for changes within.
//
// Parameters:
//    LPCTSTR szFolderName - [IN] Name of the folder to watch.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/21/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void CFileMonitor::SetFolderName(
    LPCTSTR szFolderName
    )
{
                                        // Copy folder name to local CString.
    _tcscpy( m_szFolderName, szFolderName );
}

/////////////////////////////////////////////////////////////////////
// BOOL CFileMonitor::SetFile( LPCTSTR szFileToWatch )
//
// Purpose:
//    To set the file item to wait for in our file list.
//
// Parameters:
//    LPCTSTR szFileToWatch - [IN] File to watch for change in.
//
// Returns:
//    TRUE  - New file object was set successfully.
//    FALSE - Error setting file entry.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/21/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void CFileMonitor::SetFile(
    LPCTSTR szFileToWatch
    )
{
TCHAR szFolderName[MAX_PATH];
                                        // We will not accept a NULL string
                                        // pointer.
    SYM_ASSERT( szFileToWatch );

										// Set the file name of the file we
										// are to watch.
    m_cWatchedFile.SetFileName( szFileToWatch );

                                        // Copy the folder name of the file we
                                        // are watching to our local folder
                                        // string variable.  This needs to be
                                        // local so that we can block on file
                                        // change notifications.
    m_cWatchedFile.GetFolderName( szFolderName );

                                        // If there was a folder associated
                                        // with the file, we should copy into
                                        // our folder name variable the folder
                                        // name of the file.  Otherwise, we
                                        // assume that the file already has a 
                                        // folder set up for it.
    if ( _tcslen( szFolderName ) )
        {
        _tcscpy( m_szFolderName, szFolderName );
        }
}

/////////////////////////////////////////////////////////////////////
// void CFileMonitor::SetCallback(
//          FM_FILE_CHANGE_CALLBACK fnFileChangeCallback )
//
// Purpose:
//    To set the file item to wait for in our file list.
//
// Parameters:
//    FM_FILE_CHANGE_CALLBACK fnFileChangeCallback - [IN] Function
//                         pointer to the file change notification
//                         function.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 05/08/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void CFileMonitor::SetCallback(
    FM_FILE_CHANGE_CALLBACK  fnFileChangeCallback
    )
{
    m_fnFileChangeCallback = fnFileChangeCallback;
}


/////////////////////////////////////////////////////////////////////
// void FileWatcherThreadProc( LPVOID pParam )
//
// Purpose:
//    This is the thread proc for the folder monitor class.  This will
//    accept a folder monitor class as a parameter and block for it
//    on it's own thread.
//
// Parameters:
//    LPVOID pParam - [IN] LPARAM of this thread proc.
//
// Returns:
//    None.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 04/21/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void FileWatcherThreadProc( LPVOID pParam )
{
CFileMonitor *lpcFileMonitor;
                                        // Verify that parameter is not NULL.
    if ( NULL != pParam )
        {
                                        // Set out local Folder Monitor class
                                        // pointer equal to the parameter
                                        // passed up.
        lpcFileMonitor = (CFileMonitor *) pParam;

                                        // Block on change event.  This will
                                        // not return until a change has
                                        // occurred.
        lpcFileMonitor->EnterBlockingLoop();
        }
    else
        {
                                        // Parameter was NULL, give error.
        MessageBox( NULL, _T("pParam was NULL in watcher thread!"), _T("Error"), MB_OK );
        }
}

/////////////////////////////////////////////////////////////////////
// BOOL CFileMonitor::BeginWatchThread( void )
//
// Purpose:
//    This function will create a watcher thread for blocking on a
//    folder change event.  This needs to be done on a separate
//    thread to allow for other events to happen concurently.
//
// Parameters:
//    None.
//
// Returns:
//    Success of the thread create.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 04/21/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CFileMonitor::BeginWatchThread( void )
{
BOOL bRetVal = TRUE;

                                        // Already watching.  Assert for now.
    SYM_ASSERT( NULL == m_hWatcherThread );

                                        // Create watcher thread that will
                                        // signal us when event occurs by
                                        // calling our completion routine.
    m_hWatcherThread = (HANDLE) _beginthread( FileWatcherThreadProc,
		                                      MAX_THREAD_STACK_SIZE,
		                                      (void *)this );

                                        // Check for thread creation success.
    if ( (HANDLE)INVALID_HANDLE_VALUE == m_hWatcherThread )
        {
                                        // Thread creation failed! Return FALSE.
        MessageBox( NULL, "Could not create worker thread to wait for file change event!", "Error!", MB_OK );
        bRetVal = FALSE;
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CFileMonitor::EnterBlockingLoop( void )
//
// Purpose:
//  This function will block for a file change event on any of the
//  files that it is currently watching.
//
//  WARNING : This call will halt the current thread until this event
//            occurs, or the wakeup event is signaled.  This should
//            only be called on a worker thread, and not on the
//            main thread of your application.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/22/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void CFileMonitor::EnterBlockingLoop( void )
{
HANDLE  hChangeNotificationHandle;
DWORD   dwReturnValue;
HANDLE  lpEventArray[2];
BOOL    bContinue=TRUE;

                                        // Set up the change notification handler.
    hChangeNotificationHandle = FindFirstChangeNotification(
        m_szFolderName,                 // Folder to wait on.
        FALSE,                          // Sub Directories?
        FILE_NOTIFY_CHANGE_LAST_WRITE   // Notify when...
        );

    if ( INVALID_HANDLE_VALUE != hChangeNotificationHandle )
        {
                                        // Set up our array of handles to be
                                        // first, our change notification, and
                                        // second, our kill event for when we
                                        // need to shut down.
        lpEventArray[0] = hChangeNotificationHandle;
        lpEventArray[1] = m_hKillWaitEvent;

        do
            {
                                        // Wait for either the event to
                                        // be signaled, or our kill event
                                        // to happen so we can shut
                                        // ourselves down.
            dwReturnValue = WaitForMultipleObjects(
                2,
                lpEventArray,           // Array of events to wait on.
                FALSE,                  // Wait until one object is signaled. (Not all)
                INFINITE                // Wait Forever.
                );

                                        // Handle the event.
            switch ( dwReturnValue )
                {
                case WAIT_OBJECT_0:
                                        // Change event is signaled.
                                        // Check currently watched objects
                                        // and dispatch the event notifications
                                        // for those files.
                    DispatchNotifications();

                                        // Reset this event to happen again.
                    if ( FALSE == FindNextChangeNotification( lpEventArray[0] ) )
                        {
                                        // Reset event failed.  Exit loop.
                        MessageBox( NULL, "Failed to reset wait event!", "Error!", MB_OK );
                        bContinue = FALSE;
                        }

                    break;

                case WAIT_OBJECT_0 + 1:
                                        // Shutting down event...
                                        // Drop straight out of this thread.
                    bContinue = FALSE;
                    break;

                default:
                                        // We should not get anything but
                                        // our wait objects.  If this happens
                                        // we should assert and exit immediately.
                    bContinue = FALSE;
                    SYM_ASSERT( 1 );
                    break;
                }
            }
        while ( TRUE == bContinue );

                                        // Close the notification handler
                                        // watching this directory.
        FindCloseChangeNotification( lpEventArray[0] );
        }
    else
        {
        MessageBox( NULL, "Could not wait on selected object!", "Error!", MB_OK );
        }
}

/////////////////////////////////////////////////////////////////////
// BOOL CFileMonitor::ChangeNotification( LPCTSTR szFileName );
//
// Purpose:
//    This is the callback that is called when a change notification
//    occurs.  Instead of actually calling a C style callback, you
//    would just derive your class from this class, and then provide
//    your own change notification function.
//
// Parameters:
//    LPCTSTR szFileName - [IN] Name of the file that has changed
//                              since you started to watch this set
//                              of files.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/22/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void CFileMonitor::ChangeNotification(
    LPCTSTR szFileName
    )
{
}

/////////////////////////////////////////////////////////////////////
// void CFileMonitor::DispatchNotification( void );
//
// Purpose:
//    This function will dispatch a notification to the function
//    we have been configured to alert.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/23/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void CFileMonitor::DispatchNotifications( void )
{

    if ( TRUE == m_cWatchedFile.HasChanged() )
        {
                                        // Call through to the change
                                        // notification function.
        ChangeNotification( m_cWatchedFile.GetFileName() );

                                        // If we have a change notification
                                        // function...
        if ( NULL != m_fnFileChangeCallback )
            {
            m_fnFileChangeCallback( m_cWatchedFile.GetFileName() );
            }

                                        // Update the time we have on this file
                                        // to the actual time of the file now.
        m_cWatchedFile.UpdateTime();
        }
}

/////////////////////////////////////////////////////////////////////
// CWatchedFile Class.


/////////////////////////////////////////////////////////////////////
// CWatchedFile::CWatchedFile( void )
//
// Purpose:
//    Constructor for individual watched file items.
//
// Parameters:
//    None.
//
// Returns:
//    Constructed class.
//
// See Also:
//    CFileMonitor.  This class uses these objects to wait for
//    change event notifications.
//
/////////////////////////////////////////////////////////////////////
// 04/22/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
CWatchedFile::CWatchedFile( void )
{
}

/////////////////////////////////////////////////////////////////////
// CWatchedFile::~CWatchedFile( void )
//
// Purpose:
//    This is the destructor of a CWatchedFile class.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/22/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
CWatchedFile::~CWatchedFile( void )
{
}

/////////////////////////////////////////////////////////////////////
// BOOL CWatchedFile::SetFileName( LPCTSTR szFileName );
//
// Purpose:
//    To set the file name of this watched file.  This will also set
//    up the initial file date of this object so that we may monitor
//    it for change.
//
// Parameters:
//    LPCTSTR szFileName - [IN] New file name for this watched file
//                              object.
//
// Returns:
//    TRUE  - New file is a valid file and the time was set correctly.
//    FALSE - Invalid file name.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/22/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void CWatchedFile::SetFileName(
    LPCTSTR szFileName
    )
{
                                        // Set the internal file name to the
                                        // name that was passed up to us.
    _tcscpy( m_szFileName, szFileName );

                                        // Update the file time of this file.
    UpdateTime();
}

/////////////////////////////////////////////////////////////////////
// LPTSTR CWatchedFile::GetFolderName( LPTSTR szFolderName );
//
// Purpose:
//    Return a static string of the folder name of this watched file
//    object.
//
// Parameters:
//    LPTSTR szFolderName - [OUT] Folder name of this watched file
//                                object.
//
// Returns:
//    The same string pointer passed in for convenience.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/22/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
LPTSTR CWatchedFile::GetFolderName(
    LPTSTR szFolderName
    )
{
LPTSTR szMovePtr;
                                        // Copy the file name of this watched
                                        // file object to the return string.
    _tcscpy( szFolderName, m_szFileName );

                                        // If we have a folder specification...
    if ( szMovePtr = _tcsrchr( szFolderName, _T('\\') ) )
        {
                                        // We have a valid '\\' character, and
                                        // we are going to chop the folder name
                                        // here.
        *(CharNext(szMovePtr)) = NULL;
        }
    else
        {
                                        // If we could not find any slashes,
                                        // we should return an empty string.  This
                                        // is so to avoid confusion.  We have no
                                        // folder name in this scenario.
        *szFolderName = NULL;
        }

                                        // Return a pointer to the string
                                        // passed in.
    return szFolderName;
}
/////////////////////////////////////////////////////////////////////
// CWatchedFile::UpdateTime( void )
//
// Purpose:
//    This function will update the time of the file object that we
//    are watching.  This will typically be used to reset a file
//    watch event after a change has occurred in order to set up
//    for the next change.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/22/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void CWatchedFile::UpdateTime( void )
{
                                        // Update our time with the
                                        // current time of the file.
    GetCurrentFileTime( &m_rFileTime );
}

/////////////////////////////////////////////////////////////////////
// CWatchedFile::HasChanged( void )
//
// Purpose:
//    This function will get the current file time of the file we are
//    checking and compare it against the time of the file when last
//    we chacked.
//
// Parameters:
//    None.
//
// Returns:
//    TRUE  - This file has changed since the previously recorded
//            time.
//    FALSE - No change has occurred in this file.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/22/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CWatchedFile::HasChanged( void )
{
FILETIME rCurrentFileTime;
BOOL bRetVal=FALSE;

                                        // Get the current file time of the
                                        // configured file.
    GetCurrentFileTime( &rCurrentFileTime );

                                        // Compare the saved and the current
                                        // file times.
    if ( 0 != CompareFileTime( &rCurrentFileTime, &m_rFileTime ) )
        {
                                        // File time is different.
        bRetVal = TRUE;
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CWatchedFile::UpdateTime( void )
//
// Purpose:
//    This function will update the time of the file object that we
//    are watching.  This will typically be used to reset a file
//    watch event after a change has occurred in order to set up
//    for the next change.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 04/22/97 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CWatchedFile::GetCurrentFileTime( FILETIME *lpFileTime )
{
HANDLE hFile;
BOOL bRetVal=TRUE;

    hFile = CreateFile(
               m_szFileName,            // FileName.
               GENERIC_READ,            // dwDesiredAccess
               FILE_SHARE_READ |
                 FILE_SHARE_WRITE,      // dwShareMode
               NULL,                    // lpSecurityAttributes
               OPEN_EXISTING,           // dwCreation Distributeion
               FILE_ATTRIBUTE_NORMAL,   // dwFlagsAndAttributes
               0);                      // hTemplateFile


    if ((HANDLE)INVALID_HANDLE_VALUE == hFile)
        {
                                        // Set return to FALSE and exit function
        bRetVal = FALSE;
        goto Cleanup;
        }

    if (!GetFileTime(
            hFile,                      // hFile
            NULL,                       // lpCreationTime
            NULL,                       // lpLastAccessTime
            lpFileTime))                // lpLastWriteTime
        {
        bRetVal = FALSE;
        }

Cleanup:

                                        // If still open...
    if ( hFile != INVALID_HANDLE_VALUE )
        {
                                        // Close the file.
        CloseHandle( hFile );
        }

                                        // If we could not get the file time...
    if ( bRetVal == FALSE )
        {
                                        // Zero out the file time.
        memset( lpFileTime, 0, sizeof( *lpFileTime ) );
        }

    return bRetVal;
}

#pragma optimize("", on)

