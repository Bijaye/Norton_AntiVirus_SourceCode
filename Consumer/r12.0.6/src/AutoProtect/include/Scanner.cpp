// Scanner.cpp: implementation of the CScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVInfo.h"
#include "ccSymCommonClientInfo.h"
#include "navtrust.h"
#include "time.h"       // time functions

#include "Scanner.h"

#include <stdexcept>            // for runtime_error

/////////////////////////////////////////////////////////////////////////////
// CScanner::CScanner()

CScanner::CScanner() :
    m_pTheScanner(NULL)
{
	// Intentionally empty.
    m_bAttemptRepair = true;
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::~CScanner()

CScanner::~CScanner()
{
    if ( m_pTheScanner )
    {
        unsigned long ulRefCount = m_pTheScanner->Release ();
        m_pTheScanner = NULL;
        CCTRACEI ("CScanner::~CScanner() - releasing scanner - RefCount:%d", ulRefCount);

        // Flush out all unused memory
        SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1 );
    }
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::Initialize()

SCANSTATUS CScanner::Initialize( const char* szAppID, int iBloodhoundLevel, bool bAttemptRepair, const char* szTestDefs /*NULL*/ )
{
    CCTRACEI ("CScanner::Initialize");
    SCANSTATUS retStatus = SCAN_OK;
    m_bAttemptRepair = bAttemptRepair;

	if(SYM_FAILED(m_ccScanLoader.CreateObject(&m_pTheScanner)))
    {
        CCTRACEE ( _T("CScanner::Initialize() - ScanGetScanner() failed. err code = %08X"), retStatus );
		return retStatus;
    }

    // Initialize the scanning engine.
	char szTempDir[MAX_PATH];
	GetTempPathA(MAX_PATH, szTempDir);
    long lFlags = IScanner::ISCANNER_AUTHENTICATE_DEFS;

    // If there was a ccSetting key specified to use test defs load from
    // there without checking authentication
    if( szTestDefs && 0 != strlen(szTestDefs) && -1 != GetFileAttributes(szTestDefs) )
    {
        CCTRACEI("CScanner::Initialize() - Loading the scanner using the test defs at %s", szTestDefs);
        // Use the specified defs path
	    retStatus = m_pTheScanner->Initialize ( szTestDefs,
                                                "",
                                                szTempDir,
                                                iBloodhoundLevel,
                                                IScanner::ISCANNER_USE_DEF_PATH  );
    }
    else
    {
        // Enable definition authentication
	    retStatus = m_pTheScanner->Initialize ( szAppID,
                                                "",
                                                szTempDir,
                                                iBloodhoundLevel,
                                                IScanner::ISCANNER_AUTHENTICATE_DEFS  );
    }

    if( retStatus == SCAN_OK_OLD_DEFINITIONS )
    {
        CCTRACEE(_T("CScanner::Initialize() - Virus definitions were reverted"));
        return retStatus;
    }
    else if ( SCAN_OK != retStatus )
    {
		CCTRACEE(_T("CScanner::Initialize() - Failed to initialize scanner object. err code = %08X"), retStatus );
	}

    return retStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::GetDefsDate()

bool CScanner::GetDefsDate ( WORD& wYear, WORD& wMonth, WORD& wDay, unsigned long& ulRevision )
{
    try
    {
        if( !m_pTheScanner )
        {
            CCTRACEE ( _T("CScanner::GetDefsDate - No Scanner object available.") );
            return false;
        }

        CScanPtr <IScanDefinitionsInfo> spDefInfo;

        m_pTheScanner->GetDefinitionsInfo ( &spDefInfo );

        if ( !spDefInfo )
        {
            CCTRACEE ( _T("CScanner::GetDefsDate - No Definition Info") );
            throw std::runtime_error("CScanner::GetDefsDate - No Definition Info");
        }

        //
        // Create time structure containing def date.
        //

        // Time in secs / secs / hours / days
        time_t timeDefTime;
        time_t timeCurrentTime;
        struct tm * ptmDefDate;

        time ( &timeCurrentTime );  // get current time

        // Get def date from the engine
        //
        timeDefTime = spDefInfo->GetDate ();

        // Convert def date
        //
        ptmDefDate = localtime ( &timeDefTime );
        wYear = (ptmDefDate -> tm_year) + 1900;
        wMonth = (ptmDefDate -> tm_mon) + 1;  // talk about retarded, 0-11 for months...
        wDay = ptmDefDate -> tm_mday;
        ulRevision = spDefInfo->GetRevision ();
    }
    catch (...)
    {
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::GetDefinitionsDirectory()

bool CScanner::GetDefinitionsDirectory ( char*& szDefsDir )
{
    try
    {
        if( !m_pTheScanner )
        {
            CCTRACEE ( _T("CScanner::GetDefinitionsDirectory - No Scanner object available.") );
            return false;
        }

        CScanPtr <IScanDefinitionsInfo> spDefInfo;

        m_pTheScanner->GetDefinitionsInfo ( &spDefInfo );

        if ( !spDefInfo )
        {
            CCTRACEE ( _T("CScanner::GetDefinitionsDirectory - No Definition Info") );
            throw std::runtime_error("CScanner::GetDefinitionsDirectory - No Definition Info");
        }

        strcpy ( szDefsDir, spDefInfo->GetDefinitionsDirectory ());
    }
    catch (...)
    {
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::ScanBootRecord()

void CScanner::ScanBootRecord( char dl, IScanResults** ppResults )
{
    if( !m_pTheScanner )
    {
        CCTRACEE ( _T("CScanner::ScanBootRecord - No Scanner object available.") );
        return;
    }

	char szDrive[] = "x:\\";
	szDrive[0] = dl;

	// Save off drive letter.
	m_sDrive =  szDrive;

	// We're not scanning MBRs.
	m_bScanMBR = false;

	// Fire off the scan.  Ignore error return, since the results
	// object will contain any error information.
	m_pTheScanner->Scan( this, this, this, ppResults );
}

void CScanner::ScanMasterBootRecord( BYTE byID, IScanResults** ppResults )
{
    if( !m_pTheScanner )
    {
        CCTRACEE ( _T("CScanner::ScanMasterBootRecord - No Scanner object available.") );
        return;
    }

	// We're scanning MBRs
	m_bScanMBR = true;

	// Fire off the scan.  Ignore error return, since the results
	// object will contain any error information.
	m_pTheScanner->Scan( this, this, this, ppResults );
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::OnBusy()

SCANSTATUS CScanner::OnBusy()
{
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::OnError()

SCANSTATUS CScanner::OnError( IScanError* pError )
{
	UNREFERENCED_PARAMETER(pError);
	return SCAN_ABORT;	
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::OnNewItem()

SCANSTATUS CScanner::OnNewItem( const char * pszItem )
{
	UNREFERENCED_PARAMETER(pszItem);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::OnNewFile()

SCANSTATUS CScanner::OnNewFile( const char * pszLongName, const char * pszShortName )
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::OnNewDirectory()

SCANSTATUS CScanner::OnNewDirectory( const char * pszLongName, const char * pszShortName )
{
	UNREFERENCED_PARAMETER(pszLongName);
	UNREFERENCED_PARAMETER(pszShortName);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::OnInfectionFound()

SCANSTATUS CScanner::OnInfectionFound( IScanInfection* pInfection )
{
	UNREFERENCED_PARAMETER(pInfection);

	// Check to see if we should repair.
	return m_bAttemptRepair ? SCAN_OK : SCAN_FALSE;	
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::OnRepairFailed()

SCANSTATUS CScanner::OnRepairFailed( IScanInfection* pInfection )
{
	UNREFERENCED_PARAMETER(pInfection);
	return SCAN_OK;	
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::OnRemoveMimeComponent()

SCANSTATUS CScanner::OnRemoveMimeComponent( IScanInfection* pInfection, char* pszMessage, 
										  int iMessageBufferSize )
{
	UNREFERENCED_PARAMETER(pInfection);
	UNREFERENCED_PARAMETER(pszMessage);
	UNREFERENCED_PARAMETER(iMessageBufferSize);
	return SCAN_DELETE;	
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::LockVolume()

SCANSTATUS CScanner::LockVolume( const char* szVolume, bool bLock )
{
	UNREFERENCED_PARAMETER(bLock);
	UNREFERENCED_PARAMETER(szVolume);
	return SCAN_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::LockVolume()

int CScanner::GetProperty( const char* szProperty, int iDefault )
{
	if( strcmp( szProperty, SCAN_FILES ) == 0 )
		return 0; // Don't scan files.

    if( strcmp( szProperty, SCAN_COMPRESSED_FILES ) == 0 )
		return 0; // Don't scan compressed files.

	if( strcmp( szProperty, SCAN_BOOT_RECORDS ) == 0 )
		return  m_bScanMBR ? 0 : 1; 

	if( strcmp( szProperty, SCAN_MASTER_BOOT_RECORDS ) == 0 )
		return  m_bScanMBR ? 1 : 0; 

	if( strcmp( szProperty, SCAN_ALL_BOOT_RECORDS ) == 0 )
		return 0; // We don't want to scan them all

	return iDefault;
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::LockVolume()

const char * CScanner::GetProperty( const char* szProperty, const char * szDefault )
{
	return szDefault;
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::LockVolume()

int CScanner::GetScanItemCount()
{
	return m_bScanMBR ? 0 : 1;
}

/////////////////////////////////////////////////////////////////////////////
// CScanner::LockVolume()

const char * CScanner::GetScanItemPath( int iIndex )
{
	return m_sDrive.c_str();
}

SCANSTATUS CScanner::OnCleanFile(const char* pszFileName,
  								 const char* pszTempFileName)
{
	return SCAN_OK;
}