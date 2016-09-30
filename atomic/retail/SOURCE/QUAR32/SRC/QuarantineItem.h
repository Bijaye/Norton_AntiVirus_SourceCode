/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/QuarantineItem.h_v   1.20   08 Jun 1998 19:48:44   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// QuarantineItem.h: interface for the CQuarantineItem class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/QuarantineItem.h_v  $
// 
//    Rev 1.20   08 Jun 1998 19:48:44   SEDWARD
// Added support for progress callback function in ForwardToServer().
//
//    Rev 1.19   18 May 1998 21:22:22   jtaylor
// Removed duplicate code, improved modularity and readability.
//
//    Rev 1.18   15 May 1998 18:40:42   jtaylor
// Cleaned up code, moved security functions to global location.
// Moved compressed file check to global location.
//
//    Rev 1.17   12 May 1998 13:09:34   DBuches
// 1st pass at support for NT file security.
//
//    Rev 1.16   26 Apr 1998 00:00:28   jtaylor
// Added support for preserving the original files date/times.
//
//    Rev 1.15   24 Apr 1998 21:04:58   jtaylor
// Added new functions for getting/setting original file date/time
// Added code to check to see if the file is compressed.
//
//    Rev 1.14   10 Apr 1998 18:25:42   JTaylor
// Added Get/setFileType
// Fixed the declaration of all methods in QuarantineItem.h to use STDMTHD
// Added Type maintentance code to ScanAndRepairItem.  Added Repairable bit
// maintenence to the function as well. (Needed for Scan and Deliver).
//
//    Rev 1.13   10 Apr 1998 00:05:44   SEDWARD
// ForwardToServer now takes a server name argument.
//
//    Rev 1.12   11 Mar 1998 18:46:48   SEDWARD
// Removed appID access functions.
//
//    Rev 1.11   11 Mar 1998 18:14:12   SEDWARD
// 'RestoreItem' now accepts target path and filename; removed the
// 'SplitPathAndFilename' function.
//
//    Rev 1.10   10 Mar 1998 23:39:32   JTaylor
// Added implementation of ScanItem and RepairItem.  Added ScanRepairItem internal function.  Renamed ScanForVirus to ScanItem.
//
//    Rev 1.9   10 Mar 1998 20:06:52   SEDWARD
// 'UnpackageItem' returns the filename in an 'out' buffer if the filename is
// not specified by the caller.
//
//    Rev 1.8   10 Mar 1998 18:40:40   SEDWARD
// Removed 'm_fileInfoStruct' macro, replaced it with a reference variable;
// updated the 'UnpackageItem' prototype.
//
//    Rev 1.7   08 Mar 1998 13:58:44   SEDWARD
// Added m_fileInfoStrut macro, RestoreItem() takes a BOOL argument,
// added OriginalFileExists(), SplitPathAndFielname() and
// m_originalPath/m_originalFilename members.
//
//    Rev 1.6   07 Mar 1998 21:25:16   SEDWARD
// Updated 'UnpackageItem()' prototype, added 'IsQuarantineFile()' and
// 'IsInitialized()'.
//
//    Rev 1.5   04 Mar 1998 17:32:10   DBuches
// Fixed implementation of GetN30StructPointer().
//
//    Rev 1.4   04 Mar 1998 16:14:26   DBuches
// Changed DWORD date fields to SYSTEMTIME structures.
//
//    Rev 1.3   02 Mar 1998 21:10:56   SEDWARD
// Added buffer size arguments to 'Get' access functions, DWORDs now use
// pointers instead of references.
//
//    Rev 1.2   02 Mar 1998 15:48:38   SEDWARD
// Added more access functions.
//
//    Rev 1.1   02 Mar 1998 14:52:02   SEDWARD
// Added access functions, QFILE_HEADER_STRUCT and QFILE_INFO_STRUCT.
//
//    Rev 1.0   24 Feb 1998 17:33:40   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUARANTINEITEM_H__6F9F4C24_AD73_11D1_9105_00C04FAC114A__INCLUDED_)
#define AFX_QUARANTINEITEM_H__6F9F4C24_AD73_11D1_9105_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "iquaran.h"
#include "QuarAdd.h"

class CQuarantineItem : public IQuarantineItem
{
public:
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


    // Construction
	CQuarantineItem();
	~CQuarantineItem();

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
    STDMETHOD( ScanRepairItem(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize, BOOL bRepair) );

    // Reference count
    DWORD   m_dwRef;

    // Note: Added QFILE_INFO_STRUCT here.
    QFILE_HEADER_STRUCT     m_fileHeader;
    QFILE_INFO_STRUCT&      m_fileInfoStruct;
    HANDLE                  m_fileHandle;
};

#endif // !defined(AFX_QUARANTINEITEM_H__6F9F4C24_AD73_11D1_9105_00C04FAC114A__INCLUDED_)