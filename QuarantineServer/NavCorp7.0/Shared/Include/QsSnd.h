// QsSnd.h: interface for the CQsSnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QSSND_H__66E0781F_CD31_11D2_A89B_00C04F980248__INCLUDED_)
#define AFX_QSSND_H__66E0781F_CD31_11D2_A89B_00C04F980248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef INITGUID
#include <initguid.h>
#endif

#include "iquaran.h"
#include "qspak.h"


#ifdef CQSSND_EXPORTS
#define CQSSNDAPI __declspec(dllexport)
#else
#define CQSSNDAPI __declspec(dllimport)
#endif

///////////////////////////////////////////////////////////////////////////////
// Class IDs
///////////////////////////////////////////////////////////////////////////////

// {01ED2EB1-D0EB-11d2-A89B-00C04F980248}
DEFINE_GUID(CLSID_QsSnd, 
0x1ed2eb1, 0xd0eb, 0x11d2, 0xa8, 0x9b, 0x0, 0xc0, 0x4f, 0x98, 0x2, 0x48);

// {01ED2EB2-D0EB-11d2-A89B-00C04F980248}
DEFINE_GUID(CLSID_QsSndDLL, 
0x1ed2eb2, 0xd0eb, 0x11d2, 0xa8, 0x9b, 0x0, 0xc0, 0x4f, 0x98, 0x2, 0x48);


///////////////////////////////////////////////////////////////////////////////
// defines
///////////////////////////////////////////////////////////////////////////////

#define QSSND_PREFIX    _T("qssnd")


///////////////////////////////////////////////////////////////////////////////
// typedefs
///////////////////////////////////////////////////////////////////////////////

typedef QSPAKSTATUS (*PFNQsPakQueryItemValue)( HQSERVERITEM hItem,
                                const char * szValue,
                                QSPAKDATATYPE eType,
                                void * pBuffer,
                                unsigned long * pulBufferLength );

typedef QSPAKSTATUS (*PFNQsPakSetItemValue)( HQSERVERITEM hItem,
                                const char * szValue,
                                QSPAKDATATYPE eType,
                                void * pData,
                                unsigned long ulDataLength );

typedef QSPAKSTATUS (*PFNQsPakIsQserverFile)( const char * pszFileName );

typedef QSPAKSTATUS (*PFNQsPakOpenItem)(const char * pszFile,
                                HQSERVERITEM* phItem );

typedef QSPAKSTATUS (*PFNQsPakReleaseItem)( HQSERVERITEM hItem );

typedef QSPAKSTATUS (*PFNQsPakSaveItem)( HQSERVERITEM hItem );

typedef QSPAKSTATUS (*PFNQsPakUnpackageItem)(HQSERVERITEM hItem,
                                const char * pszDestFileName );


///////////////////////////////////////////////////////////////////////////////
// Class definition
///////////////////////////////////////////////////////////////////////////////

class CQSSNDAPI CQsSnd : public IQuarantineItem
{
public:

    // Construction
	CQsSnd();
	~CQsSnd();

    // IUnknown
    STDMETHOD( QueryInterface(REFIID, void**) );
    STDMETHOD_( ULONG, AddRef() );
    STDMETHOD_( ULONG, Release() );

    // IQuarantineItem
    STDMETHOD( SaveItem() );

    STDMETHOD( DeleteItem() );

    STDMETHOD( ScanItem( IUnknown * pUnk ) );

    STDMETHOD( RepairItem(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize) );

    STDMETHOD( ForwardToServer(LPSTR  lpszServerName, FPFORWARDPROGRESS  lpfnForwardProgress
                                        , DWORD*  dwPtrBytesSent, DWORD*  dwPtrTotalBytes) );

    STDMETHOD( RestoreItem( LPSTR lpszDestPath, LPSTR lpszDestFilename, BOOL  bOverwrite ) );

    STDMETHOD( UnpackageItem( LPSTR lpszDestinationFolder                               // [in]
                                                , LPSTR lpszDestinationFileName = NULL  // [in]
                                                , LPSTR lpszUnpackagedFileName = NULL   // [out]
                                                , DWORD dwBufSize = 0                   // [in]
                                                , BOOL bOverwrite = FALSE               // [in]
                                                , BOOL bUseTemporaryFile = FALSE) );    // [in]

    STDMETHOD( Initialize( LPSTR lpszFileName ) );

    STDMETHOD( OriginalFileExists( BOOL*  bResult ) );


    // access functions
    STDMETHOD( GetUniqueID( UUID&  uuid ) );
    STDMETHOD( SetUniqueID( UUID  newID ) );
    STDMETHOD( GetFileStatus( DWORD*  fileStatus ) );
    STDMETHOD( SetFileStatus( DWORD  newFileStatus ) );
    STDMETHOD( GetFileType( DWORD*  fileType ) );
    STDMETHOD( SetFileType( DWORD  newFileType ) );
    STDMETHOD( GetDateQuarantined( SYSTEMTIME* dateQuarantined ) );
    STDMETHOD( SetDateQuarantined( SYSTEMTIME* newQuarantineDate ) );
    STDMETHOD( GetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                     SYSTEMTIME* pstOriginalFileDateAccessed,
                                     SYSTEMTIME* pstOriginalFileDateWritten ) );
    STDMETHOD( SetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                     SYSTEMTIME* pstOriginalFileDateAccessed,
                                     SYSTEMTIME* pstOriginalFileDateWritten ) );
    STDMETHOD( GetDateOfLastScan( SYSTEMTIME* dateOfLastScanned ) );
    STDMETHOD( SetDateOfLastScan( SYSTEMTIME* newDateLastScan ) );
    STDMETHOD( GetDateOfLastScanDefs( SYSTEMTIME* dateOfLastScannedDefs ) );
    STDMETHOD( SetDateOfLastScanDefs( SYSTEMTIME* newDateLastScanDefs ) );
    STDMETHOD( GetDateSubmittedToSARC( SYSTEMTIME* dateSubmittedToSARC ) );
    STDMETHOD( SetDateSubmittedToSARC( SYSTEMTIME* newDateSubmittedToSARC ) );
    STDMETHOD( GetOriginalFilesize( DWORD*  originalFileSize ) );
    STDMETHOD( SetOriginalFilesize( DWORD  newOriginalFileSize ) );
    STDMETHOD( GetCurrentFilename( char*  szDestBuf, DWORD  bufSize ) );
    STDMETHOD( SetCurrentFilename( char*  szNewCurrentFilename ) );
    STDMETHOD( GetOriginalAnsiFilename( char*  szDestBuf, DWORD  bufSize ) );
    STDMETHOD( SetOriginalAnsiFilename( char*  szNewOriginalAnsiFilename ) );
    STDMETHOD( GetOriginalOwnerName( char*  szDestBuf, DWORD  bufSize ) );
    STDMETHOD( SetOriginalOwnerName( char*  szNewOwnerName ) );
    STDMETHOD( GetOriginalMachineName( char*  szDestBuf, DWORD  bufSize ) );
    STDMETHOD( SetOriginalMachineName( char*  szNewMachineName ) );
    STDMETHOD( GetOriginalMachineDomain( char*  szDestBuf, DWORD  bufSize ) );
    STDMETHOD( SetOriginalMachineDomain( char*  szNewMachineName ) );
    STDMETHOD( GetN30StructPointer( N30**  n30Ptr ) );

    BOOL    IsInitialized( void );
    BOOL    IsQuarantineFile( void );

private:

    // Reference count
    DWORD   m_dwRef;
    void*   m_pN30data;

protected:
	HQSERVERITEM m_hQSItem;
	HINSTANCE m_hQsPak;

    PFNQsPakQueryItemValue  pfnQsPakQueryItemValue;
    PFNQsPakSetItemValue    pfnQsPakSetItemValue;
    PFNQsPakIsQserverFile   pfnQsPakIsQserverFile;
    PFNQsPakOpenItem        pfnQsPakOpenItem;
    PFNQsPakReleaseItem     pfnQsPakReleaseItem;
    PFNQsPakUnpackageItem   pfnQsPakUnpackageItem;
    PFNQsPakSaveItem        pfnQsPakSaveItem;
};

#endif // !defined(AFX_QSSND_H__66E0781F_CD31_11D2_A89B_00C04F980248__INCLUDED_)
