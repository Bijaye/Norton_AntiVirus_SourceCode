////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/ScanDeliverDLL.h_v   1.18   10 Jun 1998 23:21:48   jtaylor  $
////////////////////////////////////////////////////////////////////////////
//
// ScanDeliverDLL.h - Contains the definition of CScanDeliverDLL class.
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/ScanDeliverDLL.h_v  $
// 
//    Rev 1.18   10 Jun 1998 23:21:48   jtaylor
// Added a function called TriggerAutodial which tries to establish and email connection to the SARC mailserver in order to trigger the IE4 AutoDial feature.
// 
//    Rev 1.17   08 Jun 1998 16:21:34   SEDWARD
// Added SD_CHECKED_BITMAP and SD_UNCHECKED_BITMAP.
//
//    Rev 1.16   19 May 1998 16:03:18   SEDWARD
// Added FOTD values, made 'IsFileOfTheDayModeValid' a public method.
//
//    Rev 1.15   30 Apr 1998 20:20:10   jtaylor
// added an extra argument to SendToSarc.
//
//    Rev 1.14   01 May 1998 22:20:00   jtaylor
// Added progress dialog control.
//
//    Rev 1.13   27 Apr 1998 02:07:08   SEDWARD
// Moved 'UpdateTimeLapseRegistryData' and 'IsFileOfTheDayMode' to the
// public section.
//
//    Rev 1.12   27 Apr 1998 00:47:50   jtaylor
// Added tracking to check and see if a file was sent to SARC.
//
//    Rev 1.11   27 Apr 1998 00:24:20   jtaylor
// Enhanced File of the Day support.
// Fixed double quotes in the resources.
//
//    Rev 1.10   24 Apr 1998 18:22:06   SEDWARD
// Added support for "file of the day" mode.
//
//    Rev 1.9   24 Apr 1998 00:37:00   jtaylor
// Added CheckBackendStatus.  Made the SARC API functions public (so the UI can
// call them).
//
//    Rev 1.8   23 Apr 1998 17:46:28   jtaylor
// Created a new function for generating uninque files in Quarantine directory.
// Implemented calls to that function.
//
//    Rev 1.7   20 Apr 1998 00:23:16   JTaylor
// Added more error handling.  Made function calls more robust.
// The SendToSarc function still needs more help with the error conditions.
//
//    Rev 1.6   19 Apr 1998 22:16:20   JTaylor
// Added support for SMTP deliver to CScanDeliverDLL
// Added a new return type to LaunchUI, it returns TRUE if the user finished
//   the wizard panels (i.e. no cancel)
// Cleared a variable before the call to UnpackageItem.
//
//    Rev 1.5   18 Apr 1998 19:59:10   JTaylor
// Added packaging API call and function CreatePackage.
//
//    Rev 1.4   16 Apr 1998 23:40:18   JTaylor
// Added greater support for filtering files and increased the data structures for tracking files.
//
//    Rev 1.3   15 Apr 1998 23:46:38   SEDWARD
// Added LaunchUI() and GetScanConfigOptionsPath().
//
//    Rev 1.2   14 Apr 1998 22:54:56   JTaylor
// Added implementation for maintaining strings and item pointer arrays for the items marked for submission.  (accepted/rejected lists).
//
//    Rev 1.1   13 Apr 1998 15:52:50   SEDWARD
// Changed 'DeliverFileToSarc' to 'DeliverFilesToSarc'.
//
//    Rev 1.0   13 Apr 1998 15:15:40   JTaylor
// Initial revision.
//
////////////////////////////////////////////////////////////////////////////

#ifndef _SCANDELIVERDLL_H_
#define _SCANDELIVERDLL_H_

#include "SymInterface.h"

#include "iquaran.h"
#include "IScanDeliver.h"
#include "sdpack.h"
#include "Quaradd.h"
#include <afxtempl.h>
#include <list>

#include "SDPACK_i.h"
#include "SDPackDllLoader.h"
#include "ccSymMemoryStreamImpl.h"

// value where the file of the day value is stored
#define FILE_OF_THE_DAY_VALUE       "Fdj"   // < File du jour >

// return values for IsFileOfTheDayModeValid()
const   DWORD   FOTD_IS_VALID                       = 0;
const   DWORD   FOTD_UNKOWN_ERROR                   = 1;
const   DWORD   FOTD_FILE_IS_COMPRESSED             = 2;
const   DWORD   FOTD_MORE_THAN_ONE_REJECTED_FILE    = 3;
const   DWORD   FOTD_INVALID_TIME_LAPSE             = 4;

// indices into the bitmap checkboxes
const   int     SD_UNCHECKED_BITMAP                 = 0;
const   int     SD_CHECKED_BITMAP                   = 1;

// some convenient list objects
typedef std::list<CString> CSTRING_LIST;

class CScanDeliverProgressImpl : public ISNDProgress,
                                 public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_ISNDProgress, ISNDProgress)
    SYM_INTERFACE_MAP_END()
public:
    virtual HRESULT UpdateProgress(DWORD_PTR dwpProgressCookie, int  nPercentageComplete);
};

class CScanDeliverDLL : public IScanDeliverDLL
{
public:
    // General functions
    int  GetNumberAcceptedFiles( void );
    int  GetNumberRejectedFiles( void );
    BOOL LaunchUI( void );
    IQuarantineItem * GetRejectedQuarantineItem( int nIndex );
    IQuarantineItem * GetAcceptedQuarantineItem( int nIndex );
    CString GetRejectionReason( int nIndex );
    BOOL    UpdateTimeLapseRegistryData(void);
    BOOL    IsFileOfTheDayMode(void);
    DWORD   IsFileOfTheDayModeValid(void);

    // IUnknown
    STDMETHOD( QueryInterface(REFIID, void**) );
    STDMETHOD_( ULONG, AddRef() );
    STDMETHOD_( ULONG, Release() );
    STDMETHOD_( UINT,  GetVersion() );

    // IScanDeliverDLL
    STDMETHOD( DeliverFilesToSarc( IQuarantineItem **pQuarantineItemArray, int nNumberItems ) );

    // Construction;
    CScanDeliverDLL();
    ~CScanDeliverDLL();

    STDMETHOD( TriggerAutoDial( ) );
    STDMETHOD( SendToSarc( HWND hWnd ) );
    STDMETHOD( CreatePackage( HWND hWnd ) );
    STDMETHOD( CheckBackendStatus( ) );

private:
    void MIMEEncode64TriToQuad( LPBYTE lpbyTri, LPSTR lpszQuad );
    BOOL MIMEEncodeZip( CFile *pInput, CFile *pOutput );
    STDMETHOD( CreateNewFileInQuarantineTemp( LPSTR lpszFilename, LPSTR lpszNewFilename, UINT uBufferSize ) );
    BOOL    CheckFileOfTheDayTimeLapse(void);
    BOOL    CreateTimeLapseRegistryData(HKEY  hRegKey);
    BOOL    ConvertToFileOfTheDayMode(void);
    
    BOOL    AddGeneralInformationToSubmission(ISNDSubmission* pSubmission);
    BOOL    AddSamplesToSubmission(ISNDSubmission *pSubmission);
    
    BOOL    AddInformationToSample(ISNDSample *pSample, IQuarantineItem *pItem);
    BOOL    AddDataToSample(ISNDSample *pSample, IQuarantineItem *pItem);
    BOOL    AddMainFileToSample(ISNDSample *pSample, IQuarantineItem *pItem, INT iIndex);
    
    void CleanupTemporaryFiles();

    BOOL    m_bInFileOfTheDayMode;
    BOOL    m_bPackageSent;
    int     m_nAcceptedFiles;
    int     m_nRejectedFiles;
    ccSym::CMemoryStreamImplPtr m_pPreSubmissionData;
    char    m_szPackageFilename[MAX_QUARANTINE_FILENAME_LEN];
    CArray<IQuarantineItem *, IQuarantineItem *>m_RejectedFileInterfacePointers;
    CArray<IQuarantineItem *, IQuarantineItem *>m_AcceptedFileInterfacePointers;
    CArray<CString> m_RejectedFileStrings;
    CArray<CString> m_AcceptedFileNames;

  
    // Temporary files list
    CSTRING_LIST m_lstTemporaryFiles;

	CString m_csProductName;

    // Reference count.
    DWORD m_dwRef;

};

////////////////////////////////////////////////////////////////////////////

#endif