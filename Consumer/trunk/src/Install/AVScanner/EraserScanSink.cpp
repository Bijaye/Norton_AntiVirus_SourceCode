////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AVScanObject.h"
#include "EraserScanSink.h"

using namespace ccScanw;

// Utility code
//
// Conversion from ccScanw return codes to ccScan return codes
// this will be removed when ccScan is fully deprecated (once IEraser
// adds support to use the wide ccScan interfaces)
SCANSTATUS ConvertToNarrowStatus(SCANWSTATUS status)
{
    switch(status)
    {
    case SCANW_OK:
        return SCAN_OK;
    case SCANW_FALSE:
        return SCAN_FALSE;
    case SCANW_ABORT:
        return SCAN_ABORT;
    case SCANW_REPLACE:
        return SCAN_REPLACE;
    case SCANW_DELETE:
        return SCAN_DELETE;
    case SCANW_DEEP_DELETE:
        return SCAN_DEEP_DELETE;
    case SCANW_ERROR_NO_INTERFACE:
        return SCAN_ERROR_NO_INTERFACE;
    case SCANW_ERROR_MEMORY:
        return SCAN_ERROR_MEMORY;
    case SCANW_ERROR_NOT_IMPLEMENTED:
        return SCAN_ERROR_NOT_IMPLEMENTED;
    case SCANW_ERROR_LOADING_DEFS:
        return SCAN_ERROR_LOADING_DEFS;
    case SCANW_ERROR_INVALID_ARG:
        return SCAN_ERROR_INVAILD_ARG;
    case SCANW_ERROR_DECOMPOSER:
        return SCAN_ERROR_DECOMPOSER;
    case SCANW_ERROR_FILE_NOT_FOUND:
        return SCAN_ERROR_FILE_NOT_FOUND;
    case SCANW_ERROR_NOT_INITIALIZED:
        return SCAN_ERROR_NOT_INITIALIZED;
    case SCANW_ERROR_SCAN_ENGINE:
        return SCAN_ERROR_SCAN_ENGINE;
    case SCANW_ERROR_TOO_MANY_SCANNERS:
        return SCAN_ERROR_TOO_MANY_SCANNERS;
    case SCANW_ERROR_BUFFER_TOO_SMALL:
        return SCAN_ERROR_BUFFER_TOO_SMALL;
    case SCANW_ERROR_ACCESS_DENIED:
        return SCAN_ERROR_ACCESS_DENIED;
    case SCANW_ERROR_NO_MEDIA:
        return SCAN_ERROR_NO_MEDIA;
    case SCANW_ERROR_DECOMPOSER_TOO_DEEP:
        return SCAN_ERROR_DECOMPOSER_TOO_DEEP;
    case SCANW_ERROR_MEMORY_SCAN:
        return SCAN_ERROR_MEMORY_SCAN;
    case SCANW_ERROR_BOOT_SCAN:
        return SCAN_ERROR_BOOT_SCAN;
    case SCANW_ERROR_MBR_SCAN:
        return SCAN_ERROR_MBR_SCAN;
    case SCANW_ERROR_CANNOT_DELETE:
        return SCAN_ERROR_CANNOT_DELETE;
    case SCANW_ERROR_DISK_FULL:
        return SCAN_ERROR_DISK_FULL;
    case SCANW_ERROR_DEFINITIONS:
        return SCAN_ERROR_DEFINITIONS;
    case SCANW_ERROR_CANT_REPAIR:
        return SCAN_ERROR_CANT_REPAIR;
    case SCANW_ERROR_DEFINITIONS_NOT_AUTHENTIC:
        return SCAN_ERROR_DEFINITIONS_NOT_AUTHENTIC;
    case SCANW_ERROR_UNKNOWN_EXCEPTION:
        return SCAN_ERROR_UNKNOWN_EXCEPTION;
    case SCANW_OK_OLD_DEFINITIONS:
        return SCAN_OK_OLD_DEFINITIONS;
    case SCANW_ERROR_ALTERNATE_DATA_STREAM:
        return SCAN_ERROR_ALTERNATE_DATA_STREAM;
    case SCANW_ERROR_RELOAD_FAILED:
        return SCAN_ERROR_RELOAD_FAILED;
    case SCANW_ERROR_UNKNOWN:
        return SCAN_ERROR_UNKNOWN;
    }

    CCTRACEE(L"Invalid scan code: 0x%X", status);
    return SCAN_FALSE;
}


CEraserScanSink::CEraserScanSink()
	: m_pScanObj( NULL )
{
}

CEraserScanSink::~CEraserScanSink()
{
	UnRegister();
}


// static:
SYMRESULT CEraserScanSink::Create( CEraserScanSink*& pSink, CAVScanObject* pScanObj )
{
	pSink = new (std::nothrow) CEraserScanSink();
	if ( NULL == pSink )
		return SYMERR_OUTOFMEMORY;

	pSink->AddRef();

	if ( pScanObj != NULL )
	{
		SYMRESULT sr = pSink->Register( pScanObj );
		if ( SYM_FAILED(sr) )
		{
			pSink->Release();
			pSink = NULL;
			return sr;
		}
	}

	return SYM_OK;
}


SYMRESULT CEraserScanSink::Register( CAVScanObject* pScanObj )
{
	ASSERT( pScanObj != NULL );
	if ( pScanObj == NULL )
		return SYMERR_INVALIDARG;

	UnRegister();

	CCTRCTXI0(_T("++++"));
	pScanObj->AddRef();
	m_pScanObj = pScanObj;

	return SYM_OK;
}

void CEraserScanSink::UnRegister()
{
	if ( m_pScanObj != NULL )
	{
		CCTRCTXI0(_T("---"));
		m_pScanObj->Release();
		m_pScanObj = NULL;
	}
}

// IScanwSink overrides.
SCANWSTATUS CEraserScanSink::OnBusyW()
{
	if ( IsAborted() )
		return SCANW_ABORT;

	return SCANW_OK;	
}

SCANWSTATUS CEraserScanSink::OnError(IScanwError* pError)
{
	return SCANW_OK;	
}

SCANWSTATUS CEraserScanSink::OnNewItem(const WCHAR* pszItem)
{
	if ( IsAborted() )
		return SCANW_ABORT;

	if ( m_pScanObj != NULL )
		return m_pScanObj->OnNewItem( pszItem );

	CCTRACEE( _T("CEraserScanSink::OnNewItem() - Scan object invalid.") );

	return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::OnNewFile(const WCHAR* pszLongName, ScanwFileAttribute lAttribute)
{
	if ( IsAborted() )
		return SCANW_ABORT;

	return SCANW_OK;	
}

SCANWSTATUS CEraserScanSink::OnNewDirectory(const WCHAR* pszLongName, ScanwFileAttribute lAttribute)
{
	if ( IsAborted() )
		return SCANW_ABORT;

	return SCANW_OK;	
}

SCANWSTATUS CEraserScanSink::OnInfectionFound(IScanwInfection* pInfection)
{
	if ( IsAborted() )
		return SCANW_ABORT;
	return SCANW_FALSE;
}

SCANWSTATUS CEraserScanSink::OnRepairFailed(IScanwInfection* pInfection)
{
	if ( IsAborted() )
		return SCANW_ABORT;
    return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::OnRemoveMimeComponent(IScanwInfection* pInfection, WCHAR* pszMessage,
    int iMessageBufferSize)
{
	if ( IsAborted() )
		return SCANW_ABORT;
    return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::LockVolume(const WCHAR* szVolume, bool bLock)
{
	if ( IsAborted() )
		return SCANW_ABORT;
    return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::OnCleanFile(const wchar_t * pszFileName, const wchar_t * pszTempFileName, 
										 ScanwFileAttribute lAttribute, IScanwCompressedFile *pCompressedFile)
{
	if ( IsAborted() )
		return SCANW_ABORT;
    return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::OnBeginThreadW()
{
    return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::OnEndThreadW()
{
    return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::OnInfectionHandlingComplete(IScanwInfection* pInfection)
{
	if ( IsAborted() )
		return SCANW_ABORT;
	return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::OnDeleteFailed( IScanwInfection* pInfection ) 
{
	if ( IsAborted() )
		return SCANW_ABORT;
	return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::OnPostProcessContainer( const wchar_t * pszFileName ) 
{
	if ( IsAborted() )
		return SCANW_ABORT;
	return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::OnNewCompressedChild( ::decABI::IDecomposerContainerObject * pIDecomposerContainerObject, const wchar_t * pszChildName ) 
{
	if ( IsAborted() )
		return SCANW_ABORT;
	return SCANW_OK;
}

// IScanwProperties overrides
SCANWSTATUS CEraserScanSink::GetProperty( const WCHAR* szProperty, int iDefault, int &riProperty ) const
{
	if ( m_pScanObj != NULL )
		return m_pScanObj->GetProperty( szProperty, iDefault, riProperty );

	CCTRACEE( _T("CEraserScanSink::GetProperty() - Scan object invalid - returning default value.") );

	riProperty = iDefault;

	return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::GetProperty( const WCHAR* szProperty, const WCHAR * szDefault, const wchar_t *&prwszProperty  ) const
{
	prwszProperty = szDefault;
	return SCANW_OK;
}

// IScanwItems
SCANWSTATUS CEraserScanSink::GetScanItemCount( int &riScanItemCount ) const
{
	riScanItemCount = (int) m_vItems.size();
	return SCANW_OK;
}

SCANWSTATUS CEraserScanSink::GetScanItemPath( int iIndex, const wchar_t *&prwszScanItemPath ) const
{
	prwszScanItemPath = m_vItems[iIndex].c_str();
	return SCANW_OK;
}

// ICallback implementation.
ccEraser::eResult CEraserScanSink::PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext)
{
	if ( IsAborted() )
        return ccEraser::Abort;

    return ccEraser::Continue;
}

ccEraser::eResult CEraserScanSink::PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext)
{
	if ( IsAborted() )
        return ccEraser::Abort;

	if ( m_pScanObj != NULL )
		return m_pScanObj->PostDetection( pDetectAction, eDetectionResult, pContext );

	CCTRACEE( _T("CEraserScanSink::PostDetection() - Scan object invalid.") );

    return ccEraser::Success;
}

ccEraser::eResult CEraserScanSink::OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext)
{
	if ( IsAborted() )
        return ccEraser::Abort;

	if ( m_pScanObj != NULL )
		return m_pScanObj->OnAnomalyDetected( pAnomaly, pContext );

	CCTRACEE( _T("CEraserScanSink::OnAnomalyDetected() - Scan object invalid.") );

    return ccEraser::Continue;
}

// Abort scan mechanisms
bool CEraserScanSink::IsAborted()
{
	if ( m_pScanObj != NULL )
		return m_pScanObj->CheckScanAbort();

	CCTRACEW( _T("CEraserScanSink::IsAborted() - Scan object invalid.") );

	return false;
}
