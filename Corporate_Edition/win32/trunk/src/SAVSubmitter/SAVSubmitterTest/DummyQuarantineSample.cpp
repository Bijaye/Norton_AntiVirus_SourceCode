// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.
//
// DummyQuarantineSample.cpp: implementation of the CDummyQuarantineSample class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DummyQuarantineSample.h"
#include "VPExceptionHandling.h"
#include "SymSaferStrings.h"
#include "ccEraserInterface.h"
#include <atlbase.h>

#define REF(x) (x)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDummyQuarantineSample::CDummyQuarantineSample(enumSampleType eSampleType) : 
					m_ulRef(0),
					m_eSampleType(eSampleType)
{
	::memset(&m_sDateSubmitted, 0, sizeof(m_sDateSubmitted));
}

CDummyQuarantineSample::~CDummyQuarantineSample()
{
}

STDMETHODIMP CDummyQuarantineSample::QueryInterface( REFIID , void**  )
{
    return S_OK;
}


STDMETHODIMP_(ULONG) CDummyQuarantineSample::AddRef()
{
    return ++m_ulRef;
}


STDMETHODIMP_(ULONG) CDummyQuarantineSample::Release()
{
    if( --m_ulRef )
        return m_ulRef;

    delete this;
    return 0;
}

STDMETHODIMP CDummyQuarantineSample::Initialize( LPSTR lpszFileName)
{
	if (NULL != lpszFileName && 0 != lpszFileName[0])
	{
//		m_strFileName = lpszFileName;
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

STDMETHODIMP CDummyQuarantineSample::InitializeItem(LPCSTR lpszSessionID, LPCSTR lpszRecordID)
{
	REF(lpszSessionID);
	REF(lpszRecordID);
	return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SaveItem( void )
{
	return E_NOTIMPL;
}


STDMETHODIMP CDummyQuarantineSample::UnpackageItem( LPSTR lpszDestinationFolder                  // [in]
                                            , LPSTR lpszDestinationFileName         // [in]
                                            , LPSTR lpszUnpackagedFileName          // [out]
                                            , DWORD dwBufSize                       // [in]
                                            , BOOL bOverwrite                       // [in]
                                            , BOOL bUseTemporaryFile )              // [in]

{
	REF(lpszDestinationFolder);
	REF(lpszDestinationFileName);
	REF(lpszUnpackagedFileName);
	REF(dwBufSize);
	REF(bOverwrite);
	REF(bUseTemporaryFile);
	return E_NOTIMPL;
}


STDMETHODIMP CDummyQuarantineSample::GetFileType( DWORD*  fileType )
{
	REF(fileType);
	return E_NOTIMPL;
}
 
STDMETHODIMP CDummyQuarantineSample::SetDateSubmittedToSARC( SYSTEMTIME*  newDateSubmittedToSARC )
{
	if (newDateSubmittedToSARC != NULL)
	{
		m_sDateSubmitted = *newDateSubmittedToSARC;
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

STDMETHODIMP CDummyQuarantineSample::GetOriginalAnsiFilename( char*  szDestBuf, DWORD  bufSize )
{
	ssStrnCpy(szDestBuf, "Some File Name.exe", bufSize);
	return S_OK;
}


STDMETHODIMP CDummyQuarantineSample::GetOriginalMachineName( char*  szDestBuf, DWORD  bufSize )
{
	ssStrnCpy(szDestBuf, "MACHINE_NAME", bufSize);
	return S_OK;
}

STDMETHODIMP CDummyQuarantineSample::SetOriginalMachineName( char*  szNewMachineName )
{
	if (NULL == szNewMachineName)
		return E_INVALIDARG;
    return S_OK;
}


STDMETHODIMP CDummyQuarantineSample::GetN30StructPointer( N30**  n30Ptr )
{
	if (NULL == n30Ptr)
		return E_INVALIDARG;
	return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetFileStatus( DWORD*  fileStatus )
{
	if (NULL == fileStatus)
		return E_INVALIDARG;
    return S_OK;
}

STDMETHODIMP CDummyQuarantineSample::SetFileStatus( DWORD  newFileStatus )
{
	REF(newFileStatus);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetUniqueID( UUID&  uuid )
{
	uuid.Data1 = 0xDEAD;
	uuid.Data3 = 0x1234;
	uuid.Data1 = 0x5678;
	uuid.Data1 = 0xBEEF;
	return S_OK;
}

STDMETHODIMP CDummyQuarantineSample::DeleteItem( void )
{
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::RepairItem(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize)
{
	REF(pUnk);
	REF(lpszDestBuf);
	REF(bufSize);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::ScanItem(IUnknown * pUnk)
{
	REF(pUnk);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::ForwardToServer(LPSTR  lpszServerName
                                        , FPFORWARDPROGRESS  lpfnForwardProgress
                                        , DWORD*  dwPtrBytesSent
                                        , DWORD*  dwPtrTotalBytes)
{
	REF(lpszServerName);
	REF(lpfnForwardProgress);
	REF(dwPtrBytesSent);
	REF(dwPtrTotalBytes);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::RestoreItem( LPSTR lpszDestPath
                                                    , LPSTR lpszDestFilename
                                                    , BOOL  bOverwrite )

{
	REF(lpszDestPath);
	REF(lpszDestFilename);
	REF(bOverwrite);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::OriginalFileExists( BOOL*  bResult )
{
	REF(bResult);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetUniqueID( UUID  newID )
{
	REF(newID);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetFileType( DWORD  newFileType )
{
	REF(newFileType);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetDateQuarantined( SYSTEMTIME*  dateQuarantined )
{
	if (dateQuarantined != NULL)
	{
		dateQuarantined->wYear = 2006;
		dateQuarantined->wMonth = 2;
		dateQuarantined->wDayOfWeek = 5;
		dateQuarantined->wDay = 3;
		dateQuarantined->wHour = 4;
		dateQuarantined->wMinute = 5;
		dateQuarantined->wSecond = 6;
		dateQuarantined->wMilliseconds = 7;
		return S_OK;
	}
	else
		return E_POINTER;
}

STDMETHODIMP CDummyQuarantineSample::SetDateQuarantined( SYSTEMTIME*  newQuarantineDate )
{
	REF(newQuarantineDate);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
	REF(pstOriginalFileDateCreated);
	REF(pstOriginalFileDateAccessed);
	REF(pstOriginalFileDateWritten);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
	REF(pstOriginalFileDateCreated);
	REF(pstOriginalFileDateAccessed);
	REF(pstOriginalFileDateWritten);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetDateOfLastScan( SYSTEMTIME*  dateOfLastScanned )
{
	REF(dateOfLastScanned);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetDateOfLastScan( SYSTEMTIME*  newDateLastScan )
{
	REF(newDateLastScan);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetDateOfLastScanDefs( SYSTEMTIME*  dateOfLastScannedDefs )
{
	REF(dateOfLastScannedDefs);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetDateOfLastScanDefs( SYSTEMTIME*  newDateLastScanDefs )
{
	REF(newDateLastScanDefs);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetDateSubmittedToSARC( SYSTEMTIME*  dateSubmittedToSARC )
{
	REF(dateSubmittedToSARC);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetOriginalFilesize( DWORD*  originalFileSize )
{
	REF(originalFileSize);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetOriginalFilesize( DWORD  newOriginalFileSize )
{
	REF(newOriginalFileSize);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetCurrentFilename( char*  szDestBuf, DWORD  bufSize )
{
	REF(szDestBuf);
	REF(bufSize);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetCurrentFilename( char*  szNewCurrentFilename )
{
	REF(szNewCurrentFilename);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetOriginalAnsiFilename( char*  szNewOriginalAnsiFilename )
{
	REF(szNewOriginalAnsiFilename);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetOriginalOwnerName( char*  szDestBuf, DWORD  bufSize )
{
	REF(szDestBuf);
	REF(bufSize);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetOriginalOwnerName( char*  szNewOwnerName )
{
	REF(szNewOwnerName);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetOriginalMachineDomain( char*  szDestBuf, DWORD  bufSize )
{
	REF(szDestBuf);
	REF(bufSize);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::SetOriginalMachineDomain( char*  szNewMachineDomain )
{
	REF(szNewMachineDomain);
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetItemInfo( PVBININFO pvbiSessionInfo)
{
	if (NULL == pvbiSessionInfo)
		return E_INVALIDARG;
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetItemType( DWORD *lpdwType)
{
	if (NULL != lpdwType)
	{
		*lpdwType = VBIN_RECORD_REMEDIATION;
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

//Method to get the Remediation Type for the Remediation Items
STDMETHODIMP CDummyQuarantineSample::GetRemediationType( DWORD *lpdwType)
{
	if (NULL != lpdwType)
	{
		*lpdwType = ccEraser::FileRemediationActionType;
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

//Method to get the Remediation Action File for a Remediation Record File
STDMETHODIMP CDummyQuarantineSample::GetRemediationAction(LPCSTR lpszRemediationFile,  LPCSTR lpszRemedActionFile)
{
	if (NULL == lpszRemediationFile || NULL == lpszRemedActionFile ) 
	{
		return E_INVALIDARG;
	}
    return E_NOTIMPL;
}

//Method to get the Remediation Undo for the Remediation Items
STDMETHODIMP CDummyQuarantineSample::GetRemediationUndo(LPCSTR lpszRemediationFile, LPCSTR lpszRemedUndoFile) 
{
	if (NULL == lpszRemediationFile || NULL == lpszRemedUndoFile ) 
	{
		return E_INVALIDARG;
	}
    return E_NOTIMPL;
}

STDMETHODIMP CDummyQuarantineSample::GetVirusID( DWORD *lpdwVID) 
{
	if (NULL != lpdwVID)
	{
		switch(m_eSampleType)
		{
		case eManualSample:
			*lpdwVID = 0;
			break;
		case eViralSample:
			*lpdwVID = 0x4321;	// A non-bloodhound virus ID
			break;
		case eViralBloodhoundSample:
			*lpdwVID = 18945;	// A bloodhound virus ID
			break;
		default:
			*lpdwVID = 0;
			SAVASSERT(false);
			break;
		}
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

STDMETHODIMP CDummyQuarantineSample::GetVirusType( DWORD *lpdwVType) 
{
	if (NULL != lpdwVType)
	{
		*lpdwVType = 0x00000100L;	// Same as VEFILEVIRUS in VEAPI.h
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

STDMETHODIMP CDummyQuarantineSample::GetVirusName( LPSTR lpszVirusName, DWORD *lpdwSize)
{
	static const char* DUMMY_VIRUS_NAME = "W32.Some.Dummy.Virus.For.Testing";
	
	const char* pszVirusName;

	switch(m_eSampleType)
	{
	case eManualSample:
		pszVirusName = "";
		break;
	case eViralSample:
		pszVirusName = DUMMY_VIRUS_NAME;
		break;
	case eViralBloodhoundSample:
		pszVirusName = "Bloodhound.DirActCOM";
		break;
	default:
		pszVirusName = "";
		SAVASSERT(false);
		break;
	}

	HRESULT hrRet = S_OK;

	if (lpdwSize == NULL)
		return E_INVALIDARG;

	if (NULL == lpszVirusName)
	{
		*lpdwSize = (DWORD) strlen(pszVirusName) + 1;
	}
	else
	{
		if (strlen(pszVirusName) >= *lpdwSize)
		{
			*lpdwSize = (DWORD) strlen(lpszVirusName) + 1;
			hrRet = DISP_E_BUFFERTOOSMALL;
		}
		else
		{
			ssStrnCpy(lpszVirusName, pszVirusName, *lpdwSize);
			*lpdwSize = (DWORD) strlen(pszVirusName) + 1;
		}
	}
	return hrRet;
}
