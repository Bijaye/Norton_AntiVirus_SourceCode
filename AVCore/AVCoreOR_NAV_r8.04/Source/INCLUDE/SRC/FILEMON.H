

#ifndef __FILEMON_H
#define __FILEMON_H

#include "platform.h"


///////////////////////////////////////////////////////////////////////////////
// CWatchedFile Class.

class CWatchedFile
{
public:
                                        // Constructor/Destructor.
    CWatchedFile();
    ~CWatchedFile();

                                        // Access function to the file name of
                                        // the file being watched.
    void    SetFileName( LPCTSTR szFileName );
	LPCTSTR GetFileName( void ) { return (LPCTSTR)m_szFileName; };

                                        // This function will return the folder
                                        // name of the file being watched.
    LPTSTR  GetFolderName( LPTSTR szFolderName );

                                        // Function to check if the file has
                                        // changed.
    BOOL HasChanged( void );

                                        // Update the time stamp stored for
                                        // this file.
    void UpdateTime( void );

protected:
                                        // Returns the currently stored file
                                        // time for this file.
    BOOL GetCurrentFileTime( FILETIME *lpFileTime );

private:
    TCHAR       m_szFileName[MAX_PATH];
    FILETIME    m_rFileTime;
};


///////////////////////////////////////////////////////////////////////////////
// CFileMonitor Class.

typedef void (WINAPI *FM_FILE_CHANGE_CALLBACK) ( LPCTSTR );


class CFileMonitor
{
public:
    CFileMonitor();
    ~CFileMonitor();

                                        // Access function for local variables.
    void    SetFolderName( LPCTSTR szFolderName );
    void    SetFile( LPCTSTR szFileToWatch );
    void    SetCallback( FM_FILE_CHANGE_CALLBACK fnFileChangeCallback );

                                        // This function will create a thread to
                                        // block on these events and not use the
                                        // callers thread to watch.
    BOOL    BeginWatchThread( void );

                                        // Users of this class should not call
                                        // this function unless they would like
                                        // for their currently executing thread
                                        // to block.  If they call
                                        // BeginWatchThread() it will create the
                                        // thread and do all syncronization.
    void    EnterBlockingLoop( void );


protected:
	void    DispatchNotifications( void );

    virtual void ChangeNotification( LPCTSTR szFileName );

private:
    TCHAR			m_szFolderName[MAX_PATH];
    HANDLE			m_hKillWaitEvent;

    HANDLE			m_hWatcherThread;

	CWatchedFile	m_cWatchedFile;

    FM_FILE_CHANGE_CALLBACK m_fnFileChangeCallback;
};

#endif // __FILEMON_H
