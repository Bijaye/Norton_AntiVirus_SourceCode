// NAVEngine.cpp: implementation of the CNAVEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nvlb.h"
#include "NAVEngine.h"
#include "navapi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		CNAVEngine()
//
//	Description:
//		Constructor
//
//	Parameters:
//		NONE
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
CNAVEngine::CNAVEngine()
{
	m_szDefLocation[0]		= 0;
	m_szINIFile[0]			= 0;
	m_iHeurLevel			= 0;
	
	m_hNAVEngine			= 0;
	
	m_hVirus				= 0;
	m_wVirusID				= 0;

	m_szVirusName[0]		= 0;
	m_szVirusAlias[0]		= 0;	
	m_szVirusInfo[256]		= 0;
	m_dwVirusSize			= 0;

	m_dwInfoFlags			= 0;
	m_dwControlFlags		= 0;
	m_dwQueryFlags			= 0; 

	m_dwPhysicalDriveNum	= 0;
	m_dwPartitionNum		= 0;
	m_dwPartitionType		= 0;	

	m_bIsInfected			= 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		~CNAVEngine()
//
//	Description:
//		Destructor
//
//	Parameters:
//		NONE
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
CNAVEngine::~CNAVEngine()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
VOID CNAVEngine::ClearVirusInfo()
{

	m_hVirus				= 0;
	m_wVirusID				= 0;

	m_szVirusName[0]		= 0;
	m_szVirusAlias[0]		= 0;	
	m_szVirusInfo[0]		= 0;
	m_dwVirusSize			= 0;

	m_dwInfoFlags			= 0;
	m_dwControlFlags		= 0;
	m_dwQueryFlags			= 0;

	m_dwPhysicalDriveNum	= 0;
	m_dwPartitionNum		= 0;
	m_dwPartitionType		= 0;	
	
	m_bIsInfected			= 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		SetDefLocation()
//
//	Description:
//		Set virus definition location
//
//	Parameters:
//		LPAVENGINE_CONFIG	lpAVEngine_Config
//		LPSTR				lpszDefLocation
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
VOID CNAVEngine::SetDefLocation( LPCSTR lpszDefLocation )
{
	strcpy( (LPSTR)m_szDefLocation, lpszDefLocation );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		SetINIFile()
//
//	Description:
//		Set INI file (Full path)
//
//	Parameters:
//		LPAVENGINE_CONFIG	lpAVEngine_Config
//		LPSTR				lpszINIFile
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
VOID CNAVEngine::SetINIFile( LPCSTR lpszINIFile )
{
	if( lpszINIFile == NULL )
	{
		m_szINIFile[0] = NULL;	
	}else
	{
		strcpy( (LPSTR)m_szINIFile, lpszINIFile );	
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		SetHeuristicsLevel()
//
//	Description:
//		Set virus definition location
//
//	Parameters:
//		LPAVENGINE_CONFIG	lpAVEngine_Config
//		WORD				wHeurLevel
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
VOID CNAVEngine::SetHeuristicsLevel( INT iHeurLevel )
{
	m_iHeurLevel = iHeurLevel;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		AVInitializeEngine()
//
//	Description:
//		Initialize AV engine
//
//	Parameters:
//		NONE
//
//	Returns:
//
//		NAV_OK                      0
//		NAV_ERROR                   1
//		NAV_INSUFFICIENT_BUFFER     2
//		NAV_INVALID_ARG             3
//		NAV_MEMORY_ERROR            4
//		NAV_NO_ACCESS               5
//		NAV_CANT_REPAIR             6
//		NAV_CANT_DELETE             7
//		NAV_ENGINE_IN_USE           8
//		NAV_VXD_INIT_FAILURE        9
//		NAV_DEFINITIONS_ERROR       10
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::InitNAVEngine()
{

	NAVSTATUS	dwNAVStatus;


	m_hNAVEngine = NAVEngineInit( m_szDefLocation,
								  m_szINIFile,
								  NULL,
								  m_iHeurLevel,
								  0,
								  &dwNAVStatus );

	return( dwNAVStatus );

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		AVUnloadEngine()
//
//	Description:
//		Unload AV engine
//
//	Parameters:
//		NONE
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::UnloadNAVEngine()
{

	NAVSTATUS	dwNAVStatus;

	dwNAVStatus = NAVEngineClose( m_hNAVEngine );

	return( dwNAVStatus );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		ScanMemory()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::ScanMemory()
{
	NAVSTATUS	dwNAVStatus;

	// Check for valid engine handle
	if( m_hNAVEngine == NULL )
		return( NAV_ERROR );

	// Scan memory
	dwNAVStatus = NAVScanMemory( m_hNAVEngine,
		                         &m_hVirus );
	return( dwNAVStatus );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		ScanFile()
//
//	Description:
//		Scan a file for viruses
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::ScanFile( LPCSTR lpszFileName )
{
	NAVSTATUS	dwNAVStatus;
	CHAR		szFileExt[4];
	INT			iStrLen;
	INT			iCnt;

	// Check for valid engine handle
	if( m_hNAVEngine == NULL )
		return( NAV_ERROR );


	// Clear Virus info
	ClearVirusInfo();

	// Extract file extension
	szFileExt[0] = NULL;

	iStrLen = strlen( lpszFileName );

	for( iCnt = iStrLen; iCnt >= 0; iCnt--)
	{
		if( (lpszFileName[iCnt] == '.') || ( iStrLen - 4 == iCnt ) )
		{
			sprintf( szFileExt, "%s", &lpszFileName[iCnt+1] );
			break;
		}

	}


	// Scan file with NAV engine
	dwNAVStatus =  NAVScanFile( m_hNAVEngine,
								(LPVOID)lpszFileName,
								szFileExt,
								TRUE,
								&m_hVirus );



	if( m_hVirus != NULL )
	{
		// Set virus information
		SetVirusInfo();

		// Set infected bit
		m_bIsInfected = TRUE;

		// Virus Handle
		NAVFreeVirusHandle( m_hVirus );
	}
	
	
	
	return( dwNAVStatus );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		ScanBoot()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::ScanBoot( CHAR cDriveLetter )
{
	NAVSTATUS	dwNAVStatus;
	
	// Check for valid engine handle
	if( m_hNAVEngine == NULL )
		return( NAV_ERROR );

	// Clear Virus info
	ClearVirusInfo();

	dwNAVStatus = NAVScanBoot( m_hNAVEngine,
		                       cDriveLetter,
							   &m_hVirus );


	if( m_hVirus != NULL )
	{
		// Set virus information
		SetVirusInfo();

		// Set infected bit
		m_bIsInfected = TRUE;

		// Virus Handle
		NAVFreeVirusHandle( m_hVirus );
	}


	return( dwNAVStatus );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		ScanMasterBoot()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::ScanMasterBoot( UINT uPhysDriveNum )
{
	NAVSTATUS	dwNAVStatus;

	// Check for valid engine handle
	if( m_hNAVEngine == NULL )
		return( NAV_ERROR );	

	// Clear Virus info
	ClearVirusInfo();

	dwNAVStatus = NAVScanMasterBoot( m_hNAVEngine,
		                             uPhysDriveNum,
							         &m_hVirus );

	if( m_hVirus != NULL )
	{
		// Set virus information
		SetVirusInfo();

		// Set infected bit
		m_bIsInfected = TRUE;

		// Virus Handle
		NAVFreeVirusHandle( m_hVirus );
	}

	return( dwNAVStatus );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		RepairFile()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::RepairFile( LPCSTR lpszFileName )
{
	NAVSTATUS	dwNAVStatus;
	CHAR		szFileExt[4];
	INT			iStrLen;
	INT			iCnt;

	// Check for valid engine handle
	if( m_hNAVEngine == NULL )
		return( NAV_ERROR );

	// Extract file extension
	szFileExt[0] = NULL;

	iStrLen = strlen( lpszFileName );

	for( iCnt = iStrLen; iCnt >= 0; iCnt--)
	{
		if( (lpszFileName[iCnt] == '.') || ( iStrLen - 4 == iCnt ) )
		{
			sprintf( szFileExt, "%s", &lpszFileName[iCnt+1] );
			break;
		}

	}


	// Repair file with NAV engine
	dwNAVStatus =  NAVRepairFile( m_hNAVEngine,
								  (LPVOID)lpszFileName,
								  szFileExt );
	
	return( dwNAVStatus );

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		RepairBoot()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::RepairBoot( CHAR cDriveLetter )
{
	NAVSTATUS	dwNAVStatus;

	// Check for valid engine handle
	if( m_hNAVEngine == NULL )
		return( NAV_ERROR );

	dwNAVStatus = NAVRepairBoot( m_hNAVEngine,
								 cDriveLetter );

	return( dwNAVStatus);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		RepairMasterBoot()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::RepairMasterBoot( UINT uPhysDriveNum )
{
	NAVSTATUS	dwNAVStatus;

	// Check for valid engine handle
	if( m_hNAVEngine == NULL )
		return( NAV_ERROR );

	dwNAVStatus = NAVRepairMasterBoot( m_hNAVEngine,
								       uPhysDriveNum );

	return( dwNAVStatus);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		SetVirusInfo()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::SetVirusInfo()
{
	DWORD	dwBufferSize;
	DWORD	dwResult;
	CHAR	szTempBuffer[256];
	INT		iCnt;

	// Check for valid engine handle
	if( m_hVirus == NULL )
		return( NAV_ERROR );

	/////////////////////////////////////////////////////////////////
	// Obtain virus name
	/////////////////////////////////////////////////////////////////
	dwBufferSize = sizeof(m_szVirusName);
	NAVGetVirusInfo( m_hVirus,
		             NAV_VI_VIRUS_NAME,
					 m_szVirusName,
					 &dwBufferSize );


	/////////////////////////////////////////////////////////////////
	// Obtain virus alias list
	/////////////////////////////////////////////////////////////////
	dwBufferSize = sizeof(m_szVirusAlias);
	NAVGetVirusInfo( m_hVirus,
		             NAV_VI_VIRUS_ALIAS,
					 m_szVirusAlias,
					 &dwBufferSize );


	/////////////////////////////////////////////////////////////////
	// Obtain virus alias list
	/////////////////////////////////////////////////////////////////
	dwBufferSize = sizeof(m_szVirusInfo);
	NAVGetVirusInfo( m_hVirus,
		             NAV_VI_VIRUS_INFO,
					 m_szVirusInfo,
					 &dwBufferSize );


    /////////////////////////////////////////////////////////////////
	// Obtain virus's size
	/////////////////////////////////////////////////////////////////
	dwBufferSize = sizeof( szTempBuffer );
	NAVGetVirusInfo( m_hVirus,
	        	     NAV_VI_VIRUS_SIZE,
					 (LPTSTR)szTempBuffer,
					 &dwBufferSize );
	
	m_dwVirusSize = atol( szTempBuffer );
	

	/////////////////////////////////////////////////////////////////
	// Obtain Info flags
	/////////////////////////////////////////////////////////////////
	for( iCnt = 0; iCnt < ( sizeof( dwaInfoItems ) / ( sizeof( DWORD ) * 2 ) ); iCnt++ )
	{
		dwResult = NAVGetVirusInfo( m_hVirus,
			                        (enumNAVVirusInfo)dwaInfoItems[iCnt][1],
									NULL,
									NULL );

		if( dwResult )
		{
			m_dwInfoFlags |= dwaInfoItems[iCnt][0];
		}

	}
	

	/////////////////////////////////////////////////////////////////
	// Obtain Control flags
	/////////////////////////////////////////////////////////////////
	for( iCnt = 0; iCnt < ( sizeof( dwaControlItems ) / ( sizeof( DWORD ) * 2 ) ); iCnt++ )
	{
		dwResult = NAVGetVirusInfo( m_hVirus,
			                        (enumNAVVirusInfo)dwaControlItems[iCnt][1],
									NULL,
									NULL );

		if( dwResult )
		{
			m_dwControlFlags |= dwaControlItems[iCnt][0];
		}

	}


	/////////////////////////////////////////////////////////////////
	// Obtain Query flags
	/////////////////////////////////////////////////////////////////
	for( iCnt = 0; iCnt < ( sizeof( dwaQueryItems ) / ( sizeof( DWORD ) * 2 ) ); iCnt++ )
	{
		dwResult = NAVGetVirusInfo( m_hVirus,
			                        (enumNAVVirusInfo)dwaQueryItems[iCnt][1],
									NULL,
									NULL );

		if( dwResult )
		{
			m_dwQueryFlags |= dwaQueryItems[iCnt][0];
		}

	}


	/////////////////////////////////////////////////////////////////
	// Obtain Virus ID
	/////////////////////////////////////////////////////////////////
	dwBufferSize = sizeof( m_wVirusID );
	NAVGetVirusInfo( m_hVirus,
		             NAV_VI_VIRUS_ID,
					 (LPTSTR)szTempBuffer,
					 &dwBufferSize );

	m_wVirusID = atoi( szTempBuffer );


	/////////////////////////////////////////////////////////////////
	// Get physical drive # of BR or MBR item
	/////////////////////////////////////////////////////////////////
	m_dwPhysicalDriveNum = NAVGetVirusInfo( m_hVirus,
		                                    NAV_VI_PHYSICAL_DRIVE_NUM,
					                        NULL,
					                        NULL );


	/////////////////////////////////////////////////////////////////
	// Get partition num of BR item
	/////////////////////////////////////////////////////////////////
	m_dwPartitionNum = NAVGetVirusInfo( m_hVirus,
		                                NAV_VI_PARTITION_NUM,
					                    NULL,
					                    NULL );


	////////////////////////////////////////////////////////////////
	// Get partition type of BR item
	/////////////////////////////////////////////////////////////////
	m_dwPartitionType = NAVGetVirusInfo( m_hVirus,
		                                 NAV_VI_PARTITION_TYPE,
					                     NULL,
					                     NULL );


	return( NAV_OK );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetVirusName()	
//	
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
LPSTR CNAVEngine::GetVirusName()
{
	
	return( m_szVirusName );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetVirusInfo()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
LPSTR CNAVEngine::GetVirusInfo()
{
	
	return( m_szVirusInfo );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetVirusAlias()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
LPSTR CNAVEngine::GetVirusAlias()
{
	
	return( m_szVirusAlias );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetVirusSize()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::GetVirusSize()
{
	return( m_dwVirusSize );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetVirusID()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
WORD CNAVEngine::GetVirusID()
{
	return( m_wVirusID );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetQueryFlags()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::GetQueryFlags()
{
	return( m_dwQueryFlags );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetControlFlags()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::GetControlFlags()
{
	return( m_dwControlFlags );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetInfoFlags()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::GetInfoFlags()
{
	return( m_dwInfoFlags );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetPhysicalDriveNum()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::GetPhysicalDriveNum()
{
	return( m_dwPhysicalDriveNum );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetPartitionNum()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::GetPartitionNum()
{
	return( m_dwPartitionNum );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		GetPartitionType()	
//
//	Description:
//		
//
//	Parameters:
//		
//
//	Returns:
//
//
///////////////////////////////////////////////////////////////////////////////
DWORD CNAVEngine::GetPartitionType()
{
	return( m_dwPartitionType );
}

