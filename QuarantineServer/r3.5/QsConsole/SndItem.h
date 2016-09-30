/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// SndItem.h: interface for the CSndItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNDITEM_H__77005E30_D8CA_11D2_ADE2_A8AF066C1D40__INCLUDED_)
#define AFX_SNDITEM_H__77005E30_D8CA_11D2_ADE2_A8AF066C1D40__INCLUDED_

#include "IQuaran.h"
#include "itemdata.h"
#include "mmc.h"
#import "qserver.tlb" no_namespace

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSndItem : public IQuarantineItem2  
{
public:
	CSndItem( CItemData* pItemData, LPCTSTR szFile, CSnapInItem* pItem );
	virtual ~CSndItem();

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

	/**** IQuarantineItem2 ****/
	//Initialization Method.
	STDMETHOD( InitializeItem(LPCSTR lpszSessionID, LPCSTR lpszRecordID) );

	//Method to get the VBININFO of the item
	STDMETHOD( GetItemInfo( PVBININFO pvbiSessionInfo) );

	//Method to get the Item Type (VBIN_RECORD_LEGACY_INFECTION = 0, VBIN_RECORD_SESSION_OBJECT = 1, VBIN_RECORD_REMEDIATION = 2)
	STDMETHOD( GetItemType( DWORD *lpdwType) );

	//Method to get the Remediation Type for the Remediation Items
	STDMETHOD( GetRemediationType( DWORD *lpdwType) );

	//Method to get the Remediation Action for the Remediation Items
	STDMETHOD( GetRemediationAction(LPCSTR lpszRemediationFile,  LPCSTR lpszRemedActionFile) ) ;

	//Method to get the Remediation Undo for the Remediation Items
	STDMETHOD( GetRemediationUndo(LPCSTR lpszRemediationFile, LPCSTR lpszRemedUndoFile) ) ;

	//Method to get the Virus ID
	STDMETHOD( GetVirusID( DWORD *lpdwVID) ) ;
	
	//Method to get the Virus Type or Threat Category
	STDMETHOD( GetVirusType( DWORD *lpdwVType) ) ;
    
	//Method to get the Virus Name
	STDMETHOD( GetVirusName( LPSTR		lpszVirusName,		//[OUT]		Buffer
							DWORD		*lpdwSize			//[IN/OUT]	Buffer Size
							) );


private:
    // Reference count
    DWORD   m_dwRef;
//    void*   m_pN30data; not used anymore

    // 
    // Unpackaged file.
    // 
    CString m_sFileName;

    // 
    // Pointer to real item data.
    // 
    CItemData* m_pItem;

    // 
    // Pointer to our item on Quarantine server
    // 
    CComPtr< IQuarantineServerItem > m_pQserverItem;
};

#endif // !defined(AFX_SNDITEM_H__77005E30_D8CA_11D2_ADE2_A8AF066C1D40__INCLUDED_)
