// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#include "StdAfx.h"
#include "TestScanner.h"

CTestScanner::CTestScanner() throw()
{
	m_bDecompose = TRUE;
}

CTestScanner::~CTestScanner() throw()
{
}

bool CTestScanner::GetScanner(IScanner*& pScanner) throw()
{
    try
    {
        pScanner = new CScanner;
    }
    catch(bool)
    {
        return false;
    }

	if(!pScanner)
        return false;

    pScanner->AddRef();

    CHAR szTempPath[MAX_PATH];
	::GetTempPathA(MAX_PATH, szTempPath);

	if(SCAN_OK != pScanner->Initialize("Sketchbook", "", szTempPath, AV_HURISTIC_LEVEL, IScanner::ISCANNER_USE_INMEMORY_DECOMPOSER | IScanner::ISCANNER_WOW64FSREDIRECT))
        return false;

    return true;
}

bool CTestScanner::GetScanSink(IScanSink*& pSink) throw()
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

bool CTestScanner::GetScanProperties(IScanProperties*& pScanProps) throw()
{
    try
    {
        pScanProps = new CScanProperties(m_bDecompose);
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
// CTestScanner::CSink::CSink()

CTestScanner::CSink::CSink() throw()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::~CSink()

CTestScanner::CSink::~CSink() throw()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::OnBusy()

SCANSTATUS CTestScanner::CSink::OnBusy() throw()
{
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::OnError()

SCANSTATUS CTestScanner::CSink::OnError( IScanError* pError ) throw()
{
	UNREFERENCED_PARAMETER(pError);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::OnNewItem()

SCANSTATUS CTestScanner::CSink::OnNewItem( const char * pszItem ) throw()
{
	UNREFERENCED_PARAMETER(pszItem);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::OnNewFile()

SCANSTATUS CTestScanner::CSink::OnNewFile( const char * pszLongName, const char * pszShortName ) throw()
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::OnNewDirectory()

SCANSTATUS CTestScanner::CSink::OnNewDirectory( const char * pszLongName, const char * pszShortName ) throw()
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::OnInfectionFound()

SCANSTATUS CTestScanner::CSink::OnInfectionFound( IScanInfection* pInfection ) throw()
{
	UNREFERENCED_PARAMETER(pInfection);

	// We do not want to repair
	return SCAN_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::OnRepairFailed()

SCANSTATUS CTestScanner::CSink::OnRepairFailed( IScanInfection* pInfection ) throw()
{
	UNREFERENCED_PARAMETER(pInfection);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::OnRemoveMimeComponent()

SCANSTATUS CTestScanner::CSink::OnRemoveMimeComponent( IScanInfection* pInfection, char* pszMessage, 
										  int iMessageBufferSize ) throw()
{
	UNREFERENCED_PARAMETER(pInfection);
	UNREFERENCED_PARAMETER(pszMessage);
	UNREFERENCED_PARAMETER(iMessageBufferSize);
	return SCAN_DELETE;	
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::OnRemoveMimeComponent()

SCANSTATUS CTestScanner::CSink::OnCleanFile( const char * pszFileName,
									const char * pszTempFileName ) throw()
{
    UNREFERENCED_PARAMETER(pszFileName);
    UNREFERENCED_PARAMETER(pszTempFileName);
    return SCAN_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CSink::LockVolume()

SCANSTATUS CTestScanner::CSink::LockVolume( const char* szVolume, bool bLock ) throw()
{
	UNREFERENCED_PARAMETER(bLock);
	UNREFERENCED_PARAMETER(szVolume);
	return SCAN_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CScanProperties::CScanProperties

CTestScanner::CScanProperties::CScanProperties(BOOL bDecompose) throw()
{
	m_bDecompose = bDecompose;
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CScanProperties::~CScanProperties

CTestScanner::CScanProperties::~CScanProperties() throw()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CScanProperties::GetProperty

int CTestScanner::CScanProperties::GetProperty( const char* szProperty, int iDefault )
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
		return m_bDecompose;

	if( strcmp( szProperty, SCAN_DISK_ORDER) == 0)
		return 1;
	return iDefault;
}

/////////////////////////////////////////////////////////////////////////////
// CTestScanner::CScanProperties::GetProperty()

const char * CTestScanner::CScanProperties::GetProperty( const char* szProperty, const char * szDefault ) throw()
{
	return szDefault;
}


CTestScanner::CScanner::CScanner()
{
	m_pScanner = NULL;
	m_dwTotalScanTick = 0;

	if(SYM_FAILED(m_ccScanLoader.CreateObject(m_pScanner)))
		throw false;
}

CTestScanner::CScanner::~CScanner() throw()
{
	/*
	CStdioFile output(_T("ccScanTick.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::typeText);
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format(_T("%Y-%m-%d-%H-%M-%S"));
	CString strOut;
	strOut.Format(_T("%s %u\n"), strTime, m_dwTotalScanTick);
	output.SeekToEnd();
	output.WriteString(strOut);

	if(m_pScanner)
	{
		unsigned long ulRefCount = m_pScanner.Release();
		strOut.Format(_T("IScanner ref count after release = %d"), ulRefCount);
		output.WriteString(strOut);
	}

	output.Close();
	*/
}

SCANSTATUS CTestScanner::CScanner::Initialize( const char* pszProductID,
			const char* pszNavexInfFile,
			const char* pszTempDirectory,
			int iBloodhoundLevel,
			long lFlags )
{
	return m_pScanner->Initialize(pszProductID, pszNavexInfFile, pszTempDirectory, iBloodhoundLevel, lFlags | ISCANNER_USE_PERSISTABLE_VIDS);
}

SCANSTATUS CTestScanner::CScanner::SetBloodhoundLevel( int iBloodhoundLevel )
{
	return m_pScanner->SetBloodhoundLevel(iBloodhoundLevel);
}

SCANSTATUS CTestScanner::CScanner::SetMaxDecomposerExtractSize( unsigned long ulMaxSize )
{
	return m_pScanner->SetMaxDecomposerExtractSize(ulMaxSize);
}

SCANSTATUS CTestScanner::CScanner::SetDecomposerMemoryFSSize( unsigned long ulSize )
{
	return m_pScanner->SetDecomposerMemoryFSSize(ulSize);
}

SCANSTATUS CTestScanner::CScanner::Scan( IScanItems* pScanItems,
			IScanProperties* pProps,
			IScanSink* pSink,
			IScanResults** ppResults )
{
//	DWORD dwTemp = ::GetTickCount();

	SCANSTATUS res = m_pScanner->Scan(pScanItems, pProps, pSink, ppResults);

//	m_dwTotalScanTick += ::GetTickCount() - dwTemp;

	return res;
}

SCANSTATUS CTestScanner::CScanner::RepairInfection( IScanInfection* pInfection, IScanResults** ppResults )
{
	return m_pScanner->RepairInfection(pInfection, ppResults);
}

SCANSTATUS CTestScanner::CScanner::DeleteInfection( IScanInfection* pInfection, bool bDeepDelete )
{
	return m_pScanner->DeleteInfection(pInfection, bDeepDelete);
}

SCANSTATUS CTestScanner::CScanner::ExtractInfectedFile( IScanCompressedFileInfection* pInfection,
			char *pszExtractedFile,
			int iSize )
{
	return m_pScanner->ExtractInfectedFile(pInfection, pszExtractedFile, iSize);
}

SCANSTATUS CTestScanner::CScanner::DeleteInfectedFile( const char * szFileName )
{
	return m_pScanner->DeleteInfectedFile(szFileName);
}

SCANSTATUS CTestScanner::CScanner::GetVirusInfo( unsigned long ulVirusID, IScanVirusInfo** ppInfo )
{
	return m_pScanner->GetVirusInfo(ulVirusID, ppInfo);
}

SCANSTATUS CTestScanner::CScanner::ReloadDefinitions()
{
	return m_pScanner->ReloadDefinitions();
}

SCANSTATUS CTestScanner::CScanner::NewDefinitionsAvailaible()
{
	return m_pScanner->NewDefinitionsAvailaible();
}

SCANSTATUS CTestScanner::CScanner::GetDefinitionsInfo( IScanDefinitionsInfo** pDefInfo )
{
	return m_pScanner->GetDefinitionsInfo(pDefInfo);
}

SCANSTATUS CTestScanner::CScanner::PreDefinitionsUpdate( char* pszNewDefsDirectory, int iSize )
{
	return m_pScanner->PreDefinitionsUpdate(pszNewDefsDirectory, iSize);
}

SCANSTATUS CTestScanner::CScanner::PostDefinitionsUpdate()
{
	return m_pScanner->PostDefinitionsUpdate();
}

SCANSTATUS CTestScanner::CScanner::IsAuthenticDefs()
{
	return m_pScanner->IsAuthenticDefs();
}
