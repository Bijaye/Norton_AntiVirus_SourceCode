////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/quarantinedll.h_v   1.16   08 Jun 1998 19:47:44   SEDWARD  $
////////////////////////////////////////////////////////////////////////////
//
// QuarantineDLL.h - Contains interface definitions for Quarantine DLL
//
//
//
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/quarantinedll.h_v  $
// 
//    Rev 1.16   08 Jun 1998 19:47:44   SEDWARD
// Added support for the callback function in ForwardAllFiles().
//
//    Rev 1.15   01 Jun 1998 20:06:50   SEDWARD
// Added 'NeedToForwardFiles' and 'IsValidForwardFile'.
//
//    Rev 1.14   18 May 1998 21:17:58   jtaylor
// Removed duplicate code, improved modularity and readability.
//
//    Rev 1.13   15 May 1998 18:40:38   jtaylor
// Cleaned up code, moved security functions to global location.
// Moved compressed file check to global location.
//
//    Rev 1.12   12 May 1998 13:09:34   DBuches
// 1st pass at support for NT file security.
//
//    Rev 1.11   11 May 1998 12:41:52   jtaylor
// -- added InitializeQuarOptsObject() function -- added IsOkayToForwardFiles()
//
//    Rev 1.10   26 Apr 1998 22:17:14   jtaylor
// Added ProcessAllIncomingFiles.
//
//    Rev 1.9   25 Apr 1998 22:59:48   jtaylor
// Added ProcessIncomingFile to IQuarantineDLL interface and class.
//
//    Rev 1.8   18 Apr 1998 23:07:56   JTaylor
// Added support for returning an IQuarantineItem * to the new quarantine file
// from AddFileToQuarantine.
//
//    Rev 1.7   10 Apr 1998 00:04:36   SEDWARD
// Added FetchQuarantineServeName and FetchQuarantineOptionsPath.
//
//    Rev 1.6   30 Mar 1998 19:52:50   JTaylor
// Update the method for enabling/dissabling AP.
//
//    Rev 1.5   13 Mar 1998 22:29:50   JTaylor
// Added initialize member to IQuarantineDLL.  Added checks to verify that it was initialized.
//
//    Rev 1.4   11 Mar 1998 22:42:24   JTaylor
// Added file type and file status arguments to AddFileToQuarantine.
//
//    Rev 1.3   11 Mar 1998 15:43:20   JTaylor
// Added GetCurrentDefinitionsDate to get a SYSTEMTIME with the current definitions date.
//
//    Rev 1.2   10 Mar 1998 23:41:14   JTaylor
// Added code to get the real quarantine path in the constructor, changed the argument of GetScanningContext, fixed a bug in GetScanningContext.
//
//    Rev 1.1   06 Mar 1998 19:35:08   JTaylor
// Added virus definitions usage and virus scanning initialization.
//
//    Rev 1.0   24 Feb 1998 17:33:40   DBuches
// Initial revision.
////////////////////////////////////////////////////////////////////////////

#ifndef _QUARANTINEDLL_H_
#define _QUARANTINEDLL_H_

#include "iquaran.h"
#include "quaradd.h"
#include "DefUtils.h"

class CQuarantineDLL : public IQuarantineDLL
{
public:
    // IUnknown
    STDMETHOD( QueryInterface(REFIID, void**) );
    STDMETHOD_( ULONG, AddRef() );
    STDMETHOD_( ULONG, Release() );

    // IQuarantineDLL
    STDMETHOD_( UINT, GetVersion() );

    STDMETHOD( GetQuarantineFolder( LPSTR lpszFolder,
        UINT uBufferSize ) );

    STDMETHOD( GetScanningContext( HVCONTEXT *lphContext ) );

    STDMETHOD( GetCurrentDefinitionsDate( SYSTEMTIME *lptTime ) );

    STDMETHOD( GetServerName( LPSTR lpszName,
        UINT uBufferSize ) );

    STDMETHOD( SetServerName( LPSTR lpszName ) );

    STDMETHOD( SetupQuarantineFile( LPQFILE_HEADER_STRUCT lpQuarantineHeader,
                                    LPSTR lpszSourceFileName,
                                    LPSTR lpszQuarDirectory,
                                    DWORD dwFileType,
                                    DWORD dwFileStatus,
                                    LPHANDLE lpNewQuarFileHandle,
                                    LPSTR lpszQuarFileName,
                                    DWORD dwBuffSize ) );

    STDMETHOD( WriteHeaderBuffer(   LPQFILE_HEADER_STRUCT lpQuarantineHeader,
                                    HANDLE NewQuarFileHandle ) );

    STDMETHOD( AddFileToQuarantine( LPSTR lpszSourceFileName,
                                    DWORD dwFileType,
                                    DWORD dwFileStatus,
                                    IQuarantineItem **pNewItem ) );

    STDMETHOD( ScanNewQuarantineItem( LPSTR lpszDestFileName,
                                      IQuarantineItem **pNewItem ) );

    STDMETHOD( ProcessIncomingFile( LPSTR lpszSourceFileName ) );

    STDMETHOD( ProcessAllIncomingFiles() );

    STDMETHOD( DeleteAllFiles() );

    STDMETHOD( ForwardAllFiles( FPFORWARDPROGRESS  lpfnForwardProgress = NULL) );

    STDMETHOD( Initialize( ) );

    STDMETHOD( Enum( IEnumQuarantineItems** pEnumObj ) );

    STDMETHOD( NeedToForwardFiles( BOOL*  bNeedToForward ) );

    // Construction;
    CQuarantineDLL();
    ~CQuarantineDLL();


protected:
    BOOL    FetchQuarantineOptionsPath(LPSTR  lpzsPathBuf, DWORD  dwBufSize);
    BOOL    FetchQuarantineServerName(void);
    BOOL    InitializeQuarOptsObject(IQuarantineOpts**  pQuarOpts);
    BOOL    IsOkayToForwardFiles(void);
    BOOL    IsValidForwardFile(IQuarantineItem*  pItem);


private:
	STDMETHOD( APProtectProcess(BOOL bEnable) );

    // This HINSTANCE is used to keep the APCOMM dll loaded.  That DLL is used to
    // dissable AP while this process is running.  If we unload the DLL while AP is
    // dissabled then it asserts.
    HINSTANCE       m_hAPDll;
    BOOL            m_bDissabledAP;

	BOOL m_bInitialized;
    // Reference count.
    DWORD m_dwRef;

    // Quarantine folder name.
    char m_szQuarantineFolder[MAX_PATH];

    // Quarantine server name
    char m_szServerName[MAX_PATH];

    // Pointer to the scanner context for the application
    // This needs to be sent to IQuarantineItem to scan/repair files
    HVCONTEXT m_hContext;

    // Used for maintaining and updating Virus Definition usage.
    CDefUtils* m_pDefObject;
};

////////////////////////////////////////////////////////////////////////////

#endif