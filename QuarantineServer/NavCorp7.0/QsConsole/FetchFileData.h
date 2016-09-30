// FetchFileData.h: interface for the CFetchFileData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FETCHFILEDATA_H__11BED365_CB57_11D2_8F46_3078302C2030__INCLUDED_)
#define AFX_FETCHFILEDATA_H__11BED365_CB57_11D2_8F46_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Afxtempl.h"


// 
// Callback function
// 
typedef BOOL (*LPFNFETCHCALLBACK)( DWORD dwMessage,
                                   DWORD dwFileID, 
                                   LPCTSTR szTempFileName,
                                   DWORD dwUserData );

// 
// Copy Message values
// 
#define COPY_STATUS_FILE_DONE               0
#define COPY_STATUS_JOB_DONE                1
#define COPY_STATUS_ERROR_DISK_SPACE        2
#define COPY_STATUS_ERROR                   3
#define COPY_STATUS_OK                      4
#define COPY_STATUS_FILE_DOES_NOT_EXIST     5


class CFetchFileData  
{
public:
	CFetchFileData();
	virtual ~CFetchFileData();

    // 
    // Methods
    // 
    HRESULT GetFiles( DWORD* adwFileIDs, 
                      DWORD dwNumFiles, 
                      LPUNKNOWN pUnk, 
                      LPCTSTR szDestPath, 
                      LPFNFETCHCALLBACK pCallback, 
                      DWORD dwUserData );

    void Abort();

    static void __cdecl ThreadFunc( void* pData );
    
    // 
    // List of files created.
    //
    CStringList     m_FileList;

    // 
    // Pointer identity.  Need this for call to CoSetProxyBlanket();
    // 
    SEC_WINNT_AUTH_IDENTITY *m_pIdentity;


private:
    // 
    // Handle to thread
    // 
    DWORD           m_dwThread;

    // 
    // Callback function.
    // 
    LPFNFETCHCALLBACK m_pCallback;

    // 
    // Destination path.
    // 
    CString         m_sPath;

    // 
    // File IDs
    // 
    DWORD*          m_adwFileIDs;
    
    // 
    // Number of files.
    // 
    DWORD           m_dwNumFiles;

    // 
    // Stream for interface marshalling.
    // 
    LPSTREAM        m_pStream;

    // 
    // User data.
    // 
    DWORD           m_dwUserData;

    // 
    // Handle to abort event.
    // 
    HANDLE          m_hAbort;



};



#endif // !defined(AFX_FETCHFILEDATA_H__11BED365_CB57_11D2_8F46_3078302C2030__INCLUDED_)
