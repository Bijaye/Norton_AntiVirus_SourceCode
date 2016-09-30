////////////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/IQUARAN.H_v   1.37   08 Jun 1998 19:41:12   SEDWARD  $
////////////////////////////////////////////////////////////////////////////
//
// IQuaran.h - Contains interface definitions for Quarantine DLL
//
// Interfaces:      IQuarantineDLL
//                  IQuarantineItem
//                  IEnumQuarantineItems
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/IQUARAN.H_v  $
// 
//    Rev 1.37   08 Jun 1998 19:41:12   SEDWARD
// Added prototype for progress bar callback function (used for when forwarding
// files from client to server).
//
//    Rev 1.36   04 Jun 1998 10:42:48   mdunn
// Added a CR to the last line of the file, so the bloody resource compiler
// won't choke.
//
//    Rev 1.35   01 Jun 1998 20:06:18   SEDWARD
// Added 'NeedToForwardFiles'.
//
//    Rev 1.34   26 Apr 1998 22:16:24   jtaylor
// Added function ProcessAllIncomingFiles.
//
//    Rev 1.33   25 Apr 1998 23:47:50   jtaylor
// Removed old SetOriginalFileDate.
//
//    Rev 1.32   25 Apr 1998 23:43:36   jtaylor
// added support for all three original file date/time structures.
//
//    Rev 1.31   25 Apr 1998 23:00:04   jtaylor
// Added ProcessIncomingFile to IQuarantineDLL interface.
//
//    Rev 1.30   24 Apr 1998 19:58:34   jtaylor
// Removed functions that dealt with the original OEM filename
// Added functions to save and get the Original file date/time
//
//    Rev 1.29   21 Apr 1998 14:26:32   SEDWARD
// Put the 'scan at startup' stuff back in; note that this really refers to
// the options to be prompted for a scan at startup if newer defs are found.
//
//    Rev 1.27   18 Apr 1998 23:23:52   JTaylor
// removed the default value from the last argument to AddFileToQuarantine.
//
//    Rev 1.26   18 Apr 1998 23:07:58   JTaylor
// Added support for returning an IQuarantineItem * to the new Quarantine file
// from AddFileToQuarantine.
//
//    Rev 1.25   10 Apr 1998 18:32:04   JTaylor
// Added Get/Set file type to IQuarantineItem interface.
//
//    Rev 1.24   10 Apr 1998 00:03:12   SEDWARD
// IQuarantineItem::ForwardToServer now takes a server name argument.
//
//    Rev 1.23   06 Apr 1998 19:26:52   JTaylor
// Removed the DWORDS from the read/write options calls.
//
//    Rev 1.22   01 Apr 1998 12:04:12   SEDWARD
// y
// Added 'password enabled' methods.
//
//    Rev 1.21   30 Mar 1998 19:50:10   SEDWARD
// Added Get/SetPassword functions.
//
//    Rev 1.20   25 Mar 1998 21:51:24   JTaylor
// Added prototype for Quar32 helper function MapVStatus.
//
//    Rev 1.19   16 Mar 1998 18:53:44   SEDWARD
// Added IQuarantineOpts interface.
//
//    Rev 1.18   13 Mar 1998 22:29:52   JTaylor
// Added initialize member to IQuarantineDLL.  Added checks to verify that it was initialized.
//
//    Rev 1.17   11 Mar 1998 22:42:26   JTaylor
// Added file type and file status arguments to AddFileToQuarantine.
//
//    Rev 1.16   11 Mar 1998 18:45:22   SEDWARD
// Removed 'Get/SetAppID' access functions.
//
//    Rev 1.15   11 Mar 1998 18:12:52   SEDWARD
// 'RestoreItem' prototype now accepts the target path and filename.
//
//    Rev 1.14   11 Mar 1998 15:43:24   JTaylor
// Added GetCurrentDefinitionsDate to get a SYSTEMTIME with the current definitions date.
//
//    Rev 1.13   10 Mar 1998 23:38:14   JTaylor
// Updated argument to GetScanningContext.  Renamed ScanforVirus to ScanItem.  Added arguments to ScanItem and RepairItem.
//
//    Rev 1.12   10 Mar 1998 20:08:58   SEDWARD
// 'UnpackageItem' returns the filename in an 'out' buffer if the filename is
// not specified by the caller.
//
//    Rev 1.11   10 Mar 1998 18:41:06   SEDWARD
// Updated the 'UnpackageItem' prototype.
//
//    Rev 1.10   08 Mar 1998 13:57:14   SEDWARD
// RestoreItem() takes a BOOL overwrite flag, added OriginalFileExists().
//
//    Rev 1.9   07 Mar 1998 21:23:42   SEDWARD
// Modified 'UnpackageItem' to take a BOOL overwrite flag.
//
//    Rev 1.8   06 Mar 1998 19:23:06   JTaylor
// Added GetScanningContext to the interface
//
//    Rev 1.7   04 Mar 1998 17:30:06   DBuches
// Fixed GetN30StructurePointer().  Needs to pass a ** (pointer to pointer).
//
//    Rev 1.6   04 Mar 1998 16:10:26   DBuches
// Changed date fields to SYSTEMTIME structures.
//
//    Rev 1.5   02 Mar 1998 21:10:20   SEDWARD
// Added buffer size arguments to 'Get' access functions, DWORDs now use
// pointers instead of references.
//
//    Rev 1.4   02 Mar 1998 15:48:28   SEDWARD
// Added more access functions.
//
//    Rev 1.3   02 Mar 1998 14:51:10   SEDWARD
// Added access functions; changed 'ForwareToServer' to 'ForwardToServer'.
//
//    Rev 1.2   25 Feb 1998 15:08:22   DBuches
// More interface changes.
//
//    Rev 1.1   25 Feb 1998 13:59:08   DBuches
// Fixed interface definition for IEnumQuarantineItems::Next
//
//    Rev 1.0   24 Feb 1998 17:35:10   DBuches
// Initial revision.
////////////////////////////////////////////////////////////////////////////

#ifndef _IQUARANTINE_H
#define _IQUARANTINE_H

#ifdef INITGUID
#include <initguid.h>
#endif


#include "n30type.h"
#include "avapi.h"

#define QUARANTINE_APP_ID "NAV_50_QUAR"


// This typedef is for the progress control callback function, should the caller
// wish to display progress while forwarding files.  The first DWORD pointer will
// contain the number of bytes forwarded so far, the second DWORD pointer will
// contain the total number of bytes to send.  The function should return TRUE
// unless the user clicks on the "Abort" button, in which case a return value of
// FALSE should be used.
typedef BOOL (WINAPI *FPFORWARDPROGRESS) (DWORD*, DWORD*);


// Define for Quar32 project helper function to map an AVAPI error to a
// Quarantine interface HRESULT
HRESULT MapVStatus( VSTATUS vStat );

////////////////////////////////////////////////////////////////////////////
// IQuaranineDLL

// Foreward declaration
class IEnumQuarantineItems;
class IQuarantineItem;

// {A7B79460-ACB0-11d1-9104-00C04FAC114A}
DEFINE_GUID(CLSID_QuarantineDLL,
    0xa7b79460, 0xacb0, 0x11d1, 0x91, 0x4, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);

// {A7B79461-ACB0-11d1-9104-00C04FAC114A}
DEFINE_GUID(IID_QuarantineDLL,
    0xa7b79461, 0xacb0, 0x11d1, 0x91, 0x4, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);


class IQuarantineDLL : public IUnknown
{
public:
    STDMETHOD_( UINT, GetVersion() )
        PURE;

    STDMETHOD( GetQuarantineFolder( LPSTR lpszFolder,
        UINT uBufferSize ) )
        PURE;

    STDMETHOD( Initialize( ) )
        PURE;

    STDMETHOD( GetServerName( LPSTR lpszName,
        UINT uBufferSize ) )
        PURE;

    STDMETHOD( GetScanningContext( HVCONTEXT *lphContext ) )
        PURE;

    STDMETHOD( GetCurrentDefinitionsDate( SYSTEMTIME *lptTime ) )
        PURE;

    STDMETHOD( SetServerName( LPSTR lpszName ) )
        PURE;

    STDMETHOD( AddFileToQuarantine( LPSTR lpszSourceFileName,
                                    DWORD dwFileType,
                                    DWORD dwFileStatus,
                                    IQuarantineItem **pNewItem ))
        PURE;

    STDMETHOD( ProcessIncomingFile( LPSTR lpszSourceFileName ) )
        PURE;

    STDMETHOD( ProcessAllIncomingFiles() )
        PURE;

    STDMETHOD( DeleteAllFiles() )
        PURE;

    STDMETHOD( ForwardAllFiles( FPFORWARDPROGRESS  lpfnForwardProgress = NULL ) )
        PURE;

    STDMETHOD( Enum( IEnumQuarantineItems** pEnumObj ) )
        PURE;

    STDMETHOD( NeedToForwardFiles( BOOL*  bNeedToForward ) )
        PURE;
};


////////////////////////////////////////////////////////////////////////////
// IQuarantineItem


// {A7B79462-ACB0-11d1-9104-00C04FAC114A}
DEFINE_GUID(CLSID_QuarantineItem,
    0xa7b79462, 0xacb0, 0x11d1, 0x91, 0x4, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);

// {A7B79463-ACB0-11d1-9104-00C04FAC114A}
DEFINE_GUID(IID_QuarantineItem,
    0xa7b79463, 0xacb0, 0x11d1, 0x91, 0x4, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);


class IQuarantineItem : public IUnknown
{
public:
    STDMETHOD( SaveItem() )
        PURE;

    STDMETHOD( DeleteItem() )
        PURE;

    STDMETHOD( RepairItem(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize) )
        PURE;

    STDMETHOD( ScanItem(IUnknown * pUnk) )
        PURE;

    STDMETHOD( ForwardToServer(LPSTR  lpszServerName, FPFORWARDPROGRESS  lpfnForwardProgress
                                        , DWORD*  dwPtrBytesSent, DWORD*  dwPtrTotalBytes) )
        PURE;

    STDMETHOD( RestoreItem( LPSTR lpszDestPath, LPSTR lpszDestFilename, BOOL  bOverwrite ) )
        PURE;

    STDMETHOD( UnpackageItem( LPSTR lpszDestinationFolder, LPSTR lpszDestinationFileName
                                                        , LPSTR lpszUnpackagedFileName
                                                        , DWORD dwBufSize
                                                        , BOOL bOverwrite
                                                        , BOOL bUseTemporaryFile ) )
        PURE;

    STDMETHOD( Initialize( LPSTR lpszFileName ) )
        PURE;

    STDMETHOD( OriginalFileExists( BOOL*  bResult ) )
        PURE;

    // Note: Added accessors for all members of QFILE_INFO_STRUCT
    STDMETHOD( GetUniqueID( UUID&  uuid ) )
        PURE;

    STDMETHOD( SetUniqueID( UUID  newID ) )
        PURE;

    STDMETHOD( GetFileStatus( DWORD*  fileStatus ) )
        PURE;

    STDMETHOD( SetFileStatus( DWORD  newFileStatus ) )
        PURE;

    STDMETHOD( GetFileType( DWORD*  fileType ) )
        PURE;

    STDMETHOD( SetFileType( DWORD  newFileType ) )
        PURE;

    STDMETHOD( GetDateQuarantined( SYSTEMTIME* pstdateQuarantined ) )
        PURE;

    STDMETHOD( SetDateQuarantined( SYSTEMTIME* pstnewQuarantineDate ) )
        PURE;

    STDMETHOD( GetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                     SYSTEMTIME* pstOriginalFileDateAccessed,
                                     SYSTEMTIME* pstOriginalFileDateWritten ) )
        PURE;

    STDMETHOD( SetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                     SYSTEMTIME* pstOriginalFileDateAccessed,
                                     SYSTEMTIME* pstOriginalFileDateWritten ) )
        PURE;

    STDMETHOD( GetDateOfLastScan( SYSTEMTIME* pstdateOfLastScanned ) )
        PURE;

    STDMETHOD( SetDateOfLastScan( SYSTEMTIME* pstnewDateLastScan ) )
        PURE;

    STDMETHOD( GetDateOfLastScanDefs( SYSTEMTIME* pstdateOfLastScannedDefs ) )
        PURE;

    STDMETHOD( SetDateOfLastScanDefs( SYSTEMTIME* pstnewDateLastScanDefs ) )
        PURE;

    STDMETHOD( GetDateSubmittedToSARC( SYSTEMTIME* pstdateSubmittedToSARC ) )
        PURE;

    STDMETHOD( SetDateSubmittedToSARC( SYSTEMTIME* pstdateSubmittedToSARC ) )
        PURE;

    STDMETHOD( GetOriginalFilesize( DWORD*  originalFileSize ) )
        PURE;

    STDMETHOD( SetOriginalFilesize( DWORD  newOriginalFileSize ) )
        PURE;

    STDMETHOD( GetCurrentFilename( char*  szDestBuf, DWORD  bufSize ) )
        PURE;

    STDMETHOD( SetCurrentFilename( char*  szNewCurrentFilename ) )
        PURE;

    STDMETHOD( GetOriginalAnsiFilename( char*  szDestBuf, DWORD  bufSize ) )
        PURE;

    STDMETHOD( SetOriginalAnsiFilename( char*  szNewOriginalAnsiFilename ) )
        PURE;

    STDMETHOD( GetOriginalOwnerName( char*  szDestBuf, DWORD  bufSize ) )
        PURE;

    STDMETHOD( SetOriginalOwnerName( char*  szNewOwnerName ) )
        PURE;

    STDMETHOD( GetOriginalMachineName( char*  szDestBuf, DWORD  bufSize ) )
        PURE;

    STDMETHOD( SetOriginalMachineName( char*  szNewMachineName ) )
        PURE;

    STDMETHOD( GetOriginalMachineDomain( char*  szDestBuf, DWORD  bufSize ) )
        PURE;

    STDMETHOD( SetOriginalMachineDomain( char*  szNewMachineName ) )
        PURE;

    STDMETHOD( GetN30StructPointer( N30**  n30Ptr ) )
        PURE;

};



////////////////////////////////////////////////////////////////////////////
// IEnumQuarantineItems

// {A7B79464-ACB0-11d1-9104-00C04FAC114A}
DEFINE_GUID(CLSID_EnumQuarantineItems,
    0xa7b79464, 0xacb0, 0x11d1, 0x91, 0x4, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);

// {A7B79465-ACB0-11d1-9104-00C04FAC114A}
DEFINE_GUID(IID_EnumQuarantineItems,
    0xa7b79465, 0xacb0, 0x11d1, 0x91, 0x4, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);

class IEnumQuarantineItems : public IUnknown
{
public:
    STDMETHOD( Next( ULONG celt,
        IQuarantineItem ** pItems,
        ULONG * pceltFetched ) )
        PURE;

    STDMETHOD( Skip( ULONG celt ) )
        PURE;

    STDMETHOD( Reset( void ) )
        PURE;

    STDMETHOD( Clone( IEnumQuarantineItems ** ppvOut ) )
        PURE;

};


////////////////////////////////////////////////////////////////////////////
// IQuarantineOpts


// {9143ED71-B92F-11d1-A7A0-0000E8D34392}
DEFINE_GUID(CLSID_QuarantineOpts,
    0x9143ed71, 0xb92f, 0x11d1, 0xa7, 0xa0, 0x0, 0x0, 0xe8, 0xd3, 0x43, 0x92);

// {A89310A1-B92F-11d1-A7A0-0000E8D34392}
DEFINE_GUID(IID_QuarantineOpts,
    0xa89310a1, 0xb92f, 0x11d1, 0xa7, 0xa0, 0x0, 0x0, 0xe8, 0xd3, 0x43, 0x92);


class IQuarantineOpts : public IUnknown
{
public:
    STDMETHOD( ReadQuarOptsFile( ) )
        PURE;

    STDMETHOD( WriteQuarOptsFile( ) )
        PURE;

    STDMETHOD( Initialize(LPSTR lpszFileName, BOOL  bCreate = FALSE ) )
        PURE;

    STDMETHOD( GetScanAtStartup( BOOL*  bValue ) )
        PURE;

    STDMETHOD( SetScanAtStartup( BOOL  bResult ) )
        PURE;

    STDMETHOD( GetForwardingEnabled( BOOL*  bValue ) )
        PURE;

    STDMETHOD( SetForwardingEnabled( BOOL  bResult ) )
        PURE;

    STDMETHOD( GetScanRepairedFiles( BOOL*  bValue ) )
        PURE;

    STDMETHOD( SetScanRepairedFiles( BOOL  bResult ) )
        PURE;

    STDMETHOD( GetSarcSubmissionEnabled( BOOL*  bValue ) )
        PURE;

    STDMETHOD( SetSarcSubmissionEnabled( BOOL  bResult ) )
        PURE;

    STDMETHOD( GetQuarantineServer( LPTSTR  szCharBuf, int  nBufSize ) )
        PURE;

    STDMETHOD( SetQuarantineServer( LPTSTR  szCharBuf ) )
        PURE;

    STDMETHOD( GetValidForwardTypes( DWORD*  dwValidTypes ) )
        PURE;

    STDMETHOD( SetValidForwardTypes( DWORD  dwValidTypes ) )
        PURE;

    STDMETHOD( GetValidFileOperations(DWORD*  dwFileOps, int  nIndex) )
        PURE;

    STDMETHOD( SetValidFileOperations(DWORD  dwFileOps, int  nIndex) )
        PURE;

    STDMETHOD( GetPassword(LPTSTR  szCharBuf, int  nBufSize) )
        PURE;

    STDMETHOD( SetPassword(LPTSTR  szCharBuf) )
        PURE;

    STDMETHOD( GetPasswordEnabled( BOOL*  bValue ) )
        PURE;

    STDMETHOD( SetPasswordEnabled( BOOL  bValue ) )
        PURE;
};


////////////////////////////////////////////////////////////////////////////
#endif

