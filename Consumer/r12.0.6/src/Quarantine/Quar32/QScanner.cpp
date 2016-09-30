//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// Scanner.cpp: implementation of the CScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ccSymCommonClientInfo.h"

#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("Quar32"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Module lifetime management for trust cache
#include "ccSymModuleLifetimeMgrHelper.h"
cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);

//#define INITIIDS
#include "QScanner.h"
#include "iquaran.h"
//#undef INITIIDS

#include "quarantinedll.h"
#include "EnumQuarantineItems.h"
#include "QuarantineItem.h"
#include "Quaropts.h"

#include "QuarFileApi.h"

// Object map for syminterface

SYM_OBJECT_MAP_BEGIN()                          
    SYM_OBJECT_ENTRY(IID_QuarantineDLL, CQuarantineDLL)
    SYM_OBJECT_ENTRY(IID_QuarantineItem, CQuarantineItem)
    SYM_OBJECT_ENTRY(IID_EnumQuarantineItems, CEnumQuarantineItems)
    SYM_OBJECT_ENTRY(IID_QuarantineOpts, CQuarantineOptions)
SYM_OBJECT_MAP_END()

   
CQScanner::CQScanner( ) :
	m_pScanner( NULL )
{
	// Intentionally empty.
}


CQScanner::~CQScanner()
{
	// Cleanup
	if( m_pScanner )
	{
		m_pScanner->Release();
	}
}


HRESULT CQScanner::Initialize(char* szAppID)
{

	char szTempDir[ _MAX_PATH ];
	GetTempPathA( _MAX_PATH, szTempDir );
	szTempDir[ strlen( szTempDir ) -1 ] = 0;
	
    if(m_pScanner)
        return ERROR_ALREADY_INITIALIZED;

	try
	{
		// Get scanner interface, must do this in ANSI mode
        CQuarFileAPI fileApi;
        fileApi.SwitchAPIToANSI();

		if(SYM_FAILED(m_ccScanLoader.CreateObject(&m_pScanner)))
			throw std::runtime_error("Could not create scanner object.");

        fileApi.RestoreOriginalAPI();
		
		// Finally, initialize the scanner.
        SCANSTATUS retInitialize =  m_pScanner->Initialize(szAppID,
                                                "", 
                                                szTempDir, 
                                                3,
                                                IScanner::ISCANNER_USE_PERSISTABLE_VIDS );

		if(SCAN_OK != retInitialize)
		{
            CCTRACEE(_T("CQScanner::Initialize() - IScanner::Initialize() failed with return code = 0x%X"), retInitialize);
			m_pScanner->Release();
			m_pScanner = NULL;
			throw std::runtime_error("Failed to initialize scanner object.");	
		}
	}
	catch(exception& Ex)
	{
        CCTRACEE(Ex.what());
		
		return E_FAIL;
	}

	return S_OK;
}

// CDAVIS: This was added so the PreInstall scanner can pass in its own Scanner pointer
// when quarantine is being used from the CD, it won't be able to load ccScan.
// the scanner that PreInstall scan passes in is already Initialized.
HRESULT CQScanner::InitializeWithExternalScanner(IScanner* pScanner)
{   
    // this needs to be called before initialize
    if(m_pScanner)
    {
        return ERROR_ALREADY_INITIALIZED;
    }
    
    if(pScanner != NULL)
    {
        m_pScanner = pScanner;
        m_pScanner->AddRef();
        return S_OK;
    }
    else
    {
        return ERROR_INVALID_PARAMETER;
    }
}


SCANSTATUS CQScanner::OnBusy()
{
	return SCAN_OK;	
}


SCANSTATUS CQScanner::OnError( IScanError* pError )
{
	UNREFERENCED_PARAMETER(pError);
	return SCAN_ABORT;	
}


SCANSTATUS CQScanner::OnNewItem( const char * pszItem )
{
	UNREFERENCED_PARAMETER(pszItem);
	return SCAN_OK;	
}


SCANSTATUS CQScanner::OnNewFile( const char * pszLongName, const char * pszShortName )
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}


SCANSTATUS CQScanner::OnNewDirectory( const char * pszLongName, const char * pszShortName )
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}


SCANSTATUS CQScanner::OnInfectionFound( IScanInfection* pInfection )
{
	UNREFERENCED_PARAMETER(pInfection);

	// We do not want to repair
	return SCAN_FALSE;
}


SCANSTATUS CQScanner::OnRepairFailed( IScanInfection* pInfection )
{
	UNREFERENCED_PARAMETER(pInfection);
	return SCAN_OK;	
}


SCANSTATUS CQScanner::OnRemoveMimeComponent( IScanInfection* pInfection, char* pszMessage, 
										  int iMessageBufferSize )
{
	UNREFERENCED_PARAMETER(pInfection);
	UNREFERENCED_PARAMETER(pszMessage);
	UNREFERENCED_PARAMETER(iMessageBufferSize);
	return SCAN_DELETE;	
}


SCANSTATUS CQScanner::LockVolume( const char* szVolume, bool bLock )
{
	UNREFERENCED_PARAMETER(bLock);
	UNREFERENCED_PARAMETER(szVolume);
	return SCAN_OK;
}


SCANSTATUS CQScanner::OnCleanFile( const char * pszFileName, const char * pszTempFileName )
{
    UNREFERENCED_PARAMETER(pszFileName);
    UNREFERENCED_PARAMETER(pszTempFileName);
    return SCAN_OK;
}


int CQScanner::GetProperty( const char* szProperty, int iDefault )
{
	if( strcmp( szProperty, SCAN_FILES ) == 0 )
		return 1; // Scan files.

	if( strcmp( szProperty, SCAN_BOOT_RECORDS ) == 0 )
		return  0; 

	if( strcmp( szProperty, SCAN_MASTER_BOOT_RECORDS ) == 0 )
		return  0; 

	if( strcmp( szProperty, SCAN_ALL_BOOT_RECORDS ) == 0 )
		return 0; 

	if( strcmp( szProperty, SCAN_COMPRESSED_FILES ) == 0 )
		return 0;

    if (strcmp(szProperty, SCAN_NON_VIRAL_THREATS) == 0)
        return 1; // Scan non-viral threats

    // Always disable the AP property so the engines don't disable AP before
	// each file operation since we already handle unprotecting our own process
    if (strcmp(szProperty, SCAN_DISABLE_AP) == 0)
        return 0;

	return iDefault;
}


const char * CQScanner::GetProperty( const char* szProperty, const char * szDefault )
{
	return szDefault;
}


int CQScanner::GetScanItemCount()
{
	// We always want to scan just one file
	return 1;
}


const char * CQScanner::GetScanItemPath( int iIndex )
{
	return m_ItemsToScan.at(iIndex).c_str();
}


SCANSTATUS CQScanner::RepairInfection(IScanInfection* pInfection)
{
    if( m_pScanner == NULL )
    {
        CCTRACEE(_T("CQScanner::RepairInfection() - Error the scanner object is not initialized."));
        return E_UNINITIALIZED;
    }

	return m_pScanner->RepairInfection(pInfection);
}


SCANSTATUS CQScanner::Scan(char* szFileName, IScanInfection** ppInfection)
{
    if( m_pScanner == NULL )
    {
        CCTRACEE(_T("CQScanner::Scan() - Error the scanner object is not initialized."));
        return E_UNINITIALIZED;
    }

    // Clear any previous scan items
    m_ItemsToScan.clear();

	// Save off the file name
	m_ItemsToScan.push_back(szFileName);
	IScanResults* pIScanResults = NULL;
	if(SCAN_OK != m_pScanner->Scan(this, this, this, &pIScanResults))
	{
		*ppInfection = NULL;
		pIScanResults->Release();
		return E_FAIL;
	}

	// Check for a Null results object
	if(pIScanResults == NULL)
	{
		*ppInfection = NULL;
		pIScanResults->Release();
		return E_FAIL;
	}

	// Now see if we found an infection
	if(0 == pIScanResults->GetTotalInfectionCount())
	{
		*ppInfection = NULL;
		pIScanResults->Release();
		return SCAN_OK;
	}

	// So we found an infection, just get this infection and return
	else
	{
		HRESULT retVal = pIScanResults->GetInfection(0, ppInfection);
		pIScanResults->Release();
		return retVal;
	}
}


// Retrieves the date of the current definitions for Quarantine.  This Puts the
// correct day of the defs in the SYSTEMTIME and zeros out the time fields then
// puts the revision of the defs into the hours slot.
// Input: SYSTEMTIME *lptTime -- Pointer to a structure receiving the date.
// Output: HRESULT - S_OK on success
HRESULT CQScanner::GetCurrentDefinitionsDate( SYSTEMTIME *lptTime )
{
    if( m_pScanner == NULL )
    {
        CCTRACEE(_T("CQScanner::GetCurrentDefinitionsDate() - Error the scanner object is not initialized."));
        return E_UNINITIALIZED;
    }

	CSymPtr<IScanDefinitionsInfo> pDefInfo;
	if(SCAN_OK != m_pScanner->GetDefinitionsInfo(&pDefInfo))
		return E_UNABLE_TO_GET_CURRENT_DEFS_DATE;
    
	time_t lTime = pDefInfo->GetDate();
	tm *tmTime= gmtime(&lTime);

    if( tmTime )
    {
	    lptTime->wYear = tmTime->tm_year + 1900;
        lptTime->wMonth = tmTime->tm_mon + 1;
        lptTime->wDay = tmTime->tm_mday;
        lptTime->wHour = (WORD)pDefInfo->GetRevision();
	    lptTime->wMinute = 0;
        lptTime->wSecond = 0;
        lptTime->wMilliseconds = 0;
        lptTime->wDayOfWeek = 0;
    }
    else
    {
        return E_UNABLE_TO_GET_CURRENT_DEFS_DATE;
    }

    return S_OK;
}


// Added for NAV 2005.5/r11.5
HRESULT CQScanner::GetCurrentDefinitionsDirectory(LPTSTR szDefinitionsDirectory)
{
    if(m_pScanner == NULL)
    {
        CCTRACEE(_T("CQScanner::GetCurrentDefinitionsDirectory() - Error, scanner object not initialized."));
        return E_UNINITIALIZED;
    }

	CSymPtr<IScanDefinitionsInfo> pDefInfo;
	if((SCAN_OK != m_pScanner->GetDefinitionsInfo(&pDefInfo)) || (pDefInfo == NULL))
	{
		CCTRACEE(_T("CQScanner::GetCurrentDefinitionsDirectory() - Error getting definitions info object."));
		return E_UNABLE_TO_GET_CURRENT_DEFS_DIR;
	}

	std::string strCCEraserPath;
	strCCEraserPath = pDefInfo->GetDefinitionsDirectory();
	if(strCCEraserPath.length() <= 0)
	{
		CCTRACEE(_T("CQScanner::GetCurrentDefinitionsDirectory() - Error calling pDefInfo->GetDefinitionsDirectory()"));
		return E_UNABLE_TO_GET_CURRENT_DEFS_DIR;
	}

	_tcscpy(szDefinitionsDirectory, strCCEraserPath.c_str());

    return S_OK;
} // END CQScanner::GetCurrentDefinitionsDirectory()


HRESULT CQScanner::GetVirusInformation( unsigned long ulVID, IScanVirusInfo** ppInfo )
{
    if( m_pScanner == NULL )
    {
        CCTRACEE(_T("CQScanner::GetVirusInformation() - Error the scanner object is not initialized."));
        return E_UNINITIALIZED;
    }

    if( ppInfo == NULL )
    {
        CCTRACEE(_T("CQScanner::GetVirusInformation() - Error invalid argument."));
        return E_INVALIDARG;
    }

    SCANSTATUS retVal = m_pScanner->GetVirusInfo(ulVID, ppInfo);
    if( SYM_FAILED(retVal) )
    {
        CCTRACEE(_T("CQScanner::GetVirusInformation() - Error getting virus info. Error = 0x%X"), retVal);
        return E_FAIL;
    }

    return S_OK;
}
