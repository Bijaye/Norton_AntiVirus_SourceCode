// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.
//
// DummyQuarantineSample.h: interface for a hard coded implementation of IQuarantineItem2.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUMMYQUARANTINESAMPLE_H__B0B2D703_E141_11D2_A455_00A02438707D__INCLUDED_)
#define AFX_DUMMYQUARANTINESAMPLE_H__B0B2D703_E141_11D2_A455_00A02438707D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "vpcommon.h"
#include "iquaran.h"
#include "tstring.h"

class CDummyQuarantineSample : public IQuarantineItem2 
{
public:
	typedef enum enumSampleType { eManualSample, eViralSample, eViralBloodhoundSample };

    // Construction
	CDummyQuarantineSample(enumSampleType eSampleType);
	~CDummyQuarantineSample();

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
	STDMETHOD( GetItemInfo( PVBININFO pvbiSessionInfo) );
	STDMETHOD( GetItemType( DWORD *lpdwType) );

	STDMETHOD( InitializeItem(LPCSTR lpszSessionID, LPCSTR lpszRecordID) );
	//Method to get the Remediation Type for the Remediation Items
	STDMETHOD( GetRemediationType( DWORD *lpdwType) );

	//Method to get the Remediation Action for the Remediation Items
	STDMETHOD( GetRemediationAction(LPCSTR lpszRemediation,  LPCSTR lpszRemedActionFile) );

	//Method to get the Remediation Undo for the Remediation Items
	STDMETHOD( GetRemediationUndo(LPCSTR lpszRemediation, LPCSTR lpszRemedUndoFile) );	

	//Method to get the Virus ID
	STDMETHOD( GetVirusID( DWORD *lpdwVID) );
	
	//Method to get the Virus Type or Threat Category
	STDMETHOD( GetVirusType( DWORD *lpdwVType) );
    
	//Method to get the Virus Name
	STDMETHOD( GetVirusName( LPSTR		lpszVirusName,		//[OUT]		Buffer
							DWORD		*lpdwSize			//[IN/OUT]	Buffer Size
							) );

protected:
	CDummyQuarantineSample();
	CDummyQuarantineSample(const CDummyQuarantineSample&);
	CDummyQuarantineSample& operator=(const CDummyQuarantineSample&);
	
private:
    ULONG				m_ulRef;
	enumSampleType		m_eSampleType;
	//tstring::tstring	m_strFileName;
	SYSTEMTIME			m_sDateSubmitted;
};

#endif // !defined(AFX_DUMMYQUARANTINESAMPLE_H__B0B2D703_E141_11D2_A455_00A02438707D__INCLUDED_)
