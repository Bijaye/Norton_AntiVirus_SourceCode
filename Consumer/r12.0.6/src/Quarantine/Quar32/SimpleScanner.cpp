// /////////////////////////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
// /////////////////////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "SimpleScanner.h"

CSimpleScanner::CSimpleScanner() throw()
{
}

CSimpleScanner::~CSimpleScanner() throw()
{
}

bool CSimpleScanner::GetScanner(IScanner*& pScanner) throw()
{
    if(SYM_FAILED(m_ccScanLoader.CreateObject(pScanner)))
        return false;

    TCHAR szTempPath[MAX_PATH];
    GetTempPath(MAX_PATH, szTempPath);

    if(SCAN_OK != pScanner->Initialize("QuarantineItem", "", szTempPath, AV_HURISTIC_LEVEL, IScanner::ISCANNER_USE_PERSISTABLE_VIDS))
        return false;

    return true;
}

bool CSimpleScanner::GetScanSink(IScanSink*& pSink) throw()
{
    try
    {
        pSink = new CSink;
    }
    catch(std::bad_alloc)
    {
        return false;
    }

    if(!pSink)
        return false;

    pSink->AddRef();
    return true;
}

bool CSimpleScanner::GetScanProperties(IScanProperties*& pScanProps) throw()
{
    try
    {
        pScanProps = new CScanProperties;
    }
    catch(std::bad_alloc)
    {
        return false;
    }

    if(!pScanProps)
        return false;

    pScanProps->AddRef();
    return true;
}



/////////////////////////////////////////////////////////////////////////////
// CSimpleScanner::CSink

CSimpleScanner::CSink::CSink() throw()
{
}


CSimpleScanner::CSink::~CSink() throw()
{
}


SCANSTATUS CSimpleScanner::CSink::OnBusy() throw()
{
	return SCAN_OK;	
}


SCANSTATUS CSimpleScanner::CSink::OnError( IScanError* pError ) throw()
{
	UNREFERENCED_PARAMETER(pError);
	return SCAN_OK;	
}


SCANSTATUS CSimpleScanner::CSink::OnNewItem( const char * pszItem ) throw()
{
	UNREFERENCED_PARAMETER(pszItem);
	return SCAN_OK;	
}


SCANSTATUS CSimpleScanner::CSink::OnNewFile( const char * pszLongName, const char * pszShortName ) throw()
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}


SCANSTATUS CSimpleScanner::CSink::OnNewDirectory( const char * pszLongName, const char * pszShortName ) throw()
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}


SCANSTATUS CSimpleScanner::CSink::OnInfectionFound( IScanInfection* pInfection ) throw()
{
	UNREFERENCED_PARAMETER(pInfection);

	// We do not want to repair
	return SCAN_FALSE;
}


SCANSTATUS CSimpleScanner::CSink::OnRepairFailed( IScanInfection* pInfection ) throw()
{
	UNREFERENCED_PARAMETER(pInfection);
	return SCAN_OK;	
}


SCANSTATUS CSimpleScanner::CSink::OnRemoveMimeComponent( IScanInfection* pInfection, char* pszMessage, 
										  int iMessageBufferSize ) throw()
{
	UNREFERENCED_PARAMETER(pInfection);
	UNREFERENCED_PARAMETER(pszMessage);
	UNREFERENCED_PARAMETER(iMessageBufferSize);
	return SCAN_DELETE;	
}


SCANSTATUS CSimpleScanner::CSink::OnCleanFile( const char * pszFileName,
									const char * pszTempFileName ) throw()
{
    UNREFERENCED_PARAMETER(pszFileName);
    UNREFERENCED_PARAMETER(pszTempFileName);
    return SCAN_OK;
}


SCANSTATUS CSimpleScanner::CSink::LockVolume( const char* szVolume, bool bLock ) throw()
{
	UNREFERENCED_PARAMETER(bLock);
	UNREFERENCED_PARAMETER(szVolume);
	return SCAN_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CSimpleScanner::CScanProperties

CSimpleScanner::CScanProperties::CScanProperties() throw()
{
}


CSimpleScanner::CScanProperties::~CScanProperties() throw()
{
}


int CSimpleScanner::CScanProperties::GetProperty( const char* szProperty, int iDefault )
{
	if( strcmp( szProperty, SCAN_FILES ) == 0 )
		return 1; // Scan files.

    if( strcmp( szProperty, SCAN_NON_VIRAL_THREATS ) == 0 )
		return 1; // Use Threat Cat info.

	if( strcmp( szProperty, SCAN_BOOT_RECORDS ) == 0 )
		return  0; 

	if( strcmp( szProperty, SCAN_MASTER_BOOT_RECORDS ) == 0 )
		return  0; 

	if( strcmp( szProperty, SCAN_ALL_BOOT_RECORDS ) == 0 )
		return 0; 

	if( strcmp( szProperty, SCAN_COMPRESSED_FILES ) == 0 )
		return 1;
	return iDefault;
}


const char * CSimpleScanner::CScanProperties::GetProperty( const char* szProperty, const char * szDefault ) throw()
{
	return szDefault;
}
