// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI.QA/VCS/apicall.cpv   1.1   11 Aug 1998 08:35:34   mmendon  $
//
// Description: Implementation of APICall library file.  The CAPICall
//				class is a family of NAVAPI function call wrappers
//
//************************************************************************
// $Log:   S:/NAVAPI.QA/VCS/apicall.cpv  $
// 
//    Rev 1.1   11 Aug 1998 08:35:34   mmendon
// Added destructor for NAVGetVirusDefInfo due to Win16 compile/link problem.
//
//    Rev 1.0   06 Aug 1998 12:16:18   NMILLER
// Initial revision.
//
//************************************************************************


#include "APICall.h"
#include "structs.h"


// *****************************************************************
//                          CEngineBasedAPICall
// *****************************************************************

// Base class implementation for setting the engine handle
// This field is for internal use only, not to be sent to a server
// via RPC

void	CEngineBasedAPICall::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	if (pRPCSend->dwContext != 0)
		m_hNAVEngine = (HNAVENGINE) pRPCSend->dwContext;
}


// *****************************************************************
//                            CNAVEngineInit
// *****************************************************************

CNAVEngineInit::CNAVEngineInit()
{
	m_pFileCallbacks = NULL;
	m_sDefsPath = NULL;
	m_sNavexInfFile = NULL;
	m_wFlags = 0;
	m_wHeuristicLevel = 0;
	m_dwStatus = 0;
}

CNAVEngineInit::~CNAVEngineInit()
{
	if (m_sDefsPath != NULL)
		delete [] m_sDefsPath;
	if (m_sNavexInfFile != NULL)
		delete [] m_sNavexInfFile;
	if (m_pFileCallbacks != NULL)
		delete m_pFileCallbacks;
}

void CNAVEngineInit::SetDefsPath(LPTSTR szDefsPath)
{
	if (m_sDefsPath != NULL)
	{
		delete [] m_sDefsPath;
		m_sDefsPath = NULL;
	}
	if (szDefsPath != NULL)
	{
		m_sDefsPath = new TCHAR[strlen(szDefsPath) + 1];
		strcpy(m_sDefsPath, szDefsPath);
	}
}

void CNAVEngineInit::SetNavexInfFile(LPTSTR szNavexInfFile)
{
	if (m_sNavexInfFile != NULL)
	{
		delete [] m_sNavexInfFile;
		m_sNavexInfFile = NULL;
	}
	if (szNavexInfFile != NULL)
	{
		m_sNavexInfFile = new TCHAR[strlen(szNavexInfFile) + 1];
		strcpy(m_sNavexInfFile, szNavexInfFile);
	}
}

void CNAVEngineInit::SetFileCallbacks(LPNAVFILEIO lpFileCallbacks)
{
	if (m_pFileCallbacks != NULL)
	{
		delete m_pFileCallbacks;
		m_pFileCallbacks = NULL;
	}
	if (lpFileCallbacks != NULL)
	{
		m_pFileCallbacks = new NAVFILEIO;
		memcpy(m_pFileCallbacks, lpFileCallbacks, sizeof(NAVFILEIO));
	}
}


LPVOID	CNAVEngineInit::MakeAPICall()
{
	return (LPVOID) NAVEngineInit(m_sDefsPath, m_sNavexInfFile, m_pFileCallbacks,
									m_wHeuristicLevel, m_wFlags, &m_dwStatus);
}

void	CNAVEngineInit::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	if (pRPCSend->lpszStr1 != NULL)
	{
		m_sDefsPath = new TCHAR[strlen(pRPCSend->lpszStr1) + 1];
		strcpy(m_sDefsPath, pRPCSend->lpszStr1);
	}

	if (pRPCSend->lpszStr2 != NULL)
	{
		m_sNavexInfFile = new TCHAR[strlen(pRPCSend->lpszStr2) + 1];
		strcpy(m_sNavexInfFile, pRPCSend->lpszStr2);
	}

	m_wHeuristicLevel = (WORD) pRPCSend->wWord1;
	m_wFlags = (WORD) pRPCSend->wWord2;
}


// *****************************************************************
//                          CNAVEngineClose
// *****************************************************************

LPVOID	CNAVEngineClose::MakeAPICall()
{
	return (LPVOID) NAVEngineClose(m_hNAVEngine);
}


// *****************************************************************
//                          CNAVScanBoot
// *****************************************************************


CNAVScanBoot::CNAVScanBoot()
{
	m_cDriveLetter = 'c';
	m_phVirus = NULL;
}

CNAVScanBoot::~CNAVScanBoot()
{
}

LPVOID	CNAVScanBoot::MakeAPICall()
{
	return (LPVOID) NAVScanBoot(m_hNAVEngine, m_cDriveLetter, m_phVirus);
}

void	CNAVScanBoot::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	m_cDriveLetter = (char) pRPCSend->wWord1;

	if (pRPCSend->dwVirus != 0)
		m_phVirus = (HNAVVIRUS *) pRPCSend->dwVirus;
}


// *****************************************************************
//                          CNAVScanMasterBoot
// *****************************************************************

CNAVScanMasterBoot::CNAVScanMasterBoot()
{
	m_phVirus = NULL;
}

CNAVScanMasterBoot::~CNAVScanMasterBoot()
{
}

LPVOID	CNAVScanMasterBoot::MakeAPICall()
{
	return (LPVOID) NAVScanMasterBoot(m_hNAVEngine, m_uPhysDriveNum, m_phVirus);
}

void	CNAVScanMasterBoot::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	m_uPhysDriveNum = (UINT) pRPCSend->wWord1;

	if (pRPCSend->dwVirus != 0)
		m_phVirus = (HNAVVIRUS *) pRPCSend->dwVirus;
}


// *****************************************************************
//                          CNAVScanMemory
// *****************************************************************

CNAVScanMemory::CNAVScanMemory()
{
	m_phVirus = NULL;
}

LPVOID	CNAVScanMemory::MakeAPICall()
{
	return (LPVOID) NAVScanMemory(m_hNAVEngine, m_phVirus);
}



// *****************************************************************
//                          CNAVRepairFile
// *****************************************************************

CNAVRepairFile::CNAVRepairFile()
{
	m_lpszFileName = NULL;
	m_bExecutable = TRUE;
}

CNAVRepairFile::~CNAVRepairFile()
{
	if (m_lpszFileName != NULL)
		delete [] m_lpszFileName;
}

void CNAVRepairFile::SetFileId(LPVOID  lpszFileId)
{
	if (m_lpszFileName != NULL)
	{
		delete [] m_lpszFileName;
		m_lpszFileName = NULL;
	}
	if (lpszFileId != NULL)
	{
		m_lpszFileName = new TCHAR[strlen((LPTSTR) lpszFileId) + 1];
		strcpy(m_lpszFileName, (LPTSTR) lpszFileId);
	}
}

LPVOID	CNAVRepairFile::MakeAPICall()
{
	return (LPVOID) NAVRepairFile(m_hNAVEngine, (LPVOID) m_lpszFileName, m_bExecutable);
}

void	CNAVRepairFile::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	if (pRPCSend->lpszStr1 != NULL)
	{
		m_lpszFileName = new TCHAR[strlen(pRPCSend->lpszStr1) + 1];
		strcpy(m_lpszFileName, pRPCSend->lpszStr1);
	}

	if (0 == pRPCSend->wWord1)
		m_bExecutable = FALSE;
	else
		m_bExecutable = TRUE;
}


// *****************************************************************
//                          CNAVDeleteFile
// *****************************************************************

CNAVDeleteFile::CNAVDeleteFile()
{
	m_lpszFileName = NULL;
	m_bExecutable = TRUE;
}

CNAVDeleteFile::~CNAVDeleteFile()
{
	if (m_lpszFileName != NULL)
		delete [] m_lpszFileName;
}

void CNAVDeleteFile::SetFileId(LPVOID  lpszFileId)
{
	if (m_lpszFileName != NULL)
	{
		delete [] m_lpszFileName;
		m_lpszFileName = NULL;
	}
	if (lpszFileId != NULL)
	{
		m_lpszFileName = new TCHAR[strlen((LPTSTR) lpszFileId) + 1];
		strcpy(m_lpszFileName, (LPTSTR) lpszFileId);
	}
}

LPVOID	CNAVDeleteFile::MakeAPICall()
{
	return (LPVOID) NAVDeleteFile(m_hNAVEngine, (LPVOID) m_lpszFileName, m_bExecutable);
}

void	CNAVDeleteFile::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	if (pRPCSend->lpszStr1 != NULL)
	{
		m_lpszFileName = new TCHAR[strlen(pRPCSend->lpszStr1) + 1];
		strcpy(m_lpszFileName, pRPCSend->lpszStr1);
	}

	if (0 == pRPCSend->wWord1)
		m_bExecutable = FALSE;
	else
		m_bExecutable = TRUE;
}


// *****************************************************************
//                          CNAVRepairBoot
// *****************************************************************

CNAVRepairBoot::CNAVRepairBoot()
{
	m_cDriveLetter = 'c';
}

CNAVRepairBoot::~CNAVRepairBoot()
{
}

LPVOID	CNAVRepairBoot::MakeAPICall()
{
	return (LPVOID) NAVRepairBoot(m_hNAVEngine, m_cDriveLetter);
}

void	CNAVRepairBoot::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	m_cDriveLetter = (char) pRPCSend->wWord1;
}


// *****************************************************************
//                          CNAVRepairMasterBoot
// *****************************************************************

CNAVRepairMasterBoot::CNAVRepairMasterBoot()
{
	m_uPhysDriveNum = 0x80;
}

CNAVRepairMasterBoot::~CNAVRepairMasterBoot()
{
}


LPVOID	CNAVRepairMasterBoot::MakeAPICall()
{
	return (LPVOID) NAVRepairMasterBoot(m_hNAVEngine, m_uPhysDriveNum);
}

void	CNAVRepairMasterBoot::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	m_uPhysDriveNum = (UINT) pRPCSend->wWord1;
}


// *****************************************************************
//                          CNAVFreeVirusHandle
// *****************************************************************

LPVOID	CNAVFreeVirusHandle::MakeAPICall()
{
	return (LPVOID) NAVFreeVirusHandle(m_hVirus);
}

void	CNAVFreeVirusHandle::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	if (pRPCSend->dwVirus != 0)
		m_hVirus = (HNAVVIRUS) pRPCSend->dwVirus;
}

// *****************************************************************
//                          CNAVGetNumVirusEntries
// *****************************************************************

LPVOID	CNAVGetVirusDefCount::MakeAPICall()
{
	return (LPVOID) NAVGetVirusDefCount(m_hNAVEngine, &m_dwNumViruses);
}


// *****************************************************************
//                          CNAVLoadVirusDefTable
// *****************************************************************

LPVOID	CNAVLoadVirusDefTable::MakeAPICall()
{
	return (LPVOID) NAVLoadVirusDefTable(m_hNAVEngine);
}


// *****************************************************************
//                          CNAVReleaseVirusDefTable
// *****************************************************************

LPVOID	CNAVReleaseVirusDefTable::MakeAPICall()
{
	NAVReleaseVirusDefTable(m_hVirusDefTable);
	return NULL;
}


void	CNAVReleaseVirusDefTable::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	if (pRPCSend->dwVirusDefTable != 0)
		m_hVirusDefTable = (HNAVVIRUSDEFTABLE) pRPCSend->dwVirusDefTable;
}


// *****************************************************************
//                          CNAVLoadVirusDef
// *****************************************************************

LPVOID	CNAVLoadVirusDef::MakeAPICall()
{
	return (LPVOID) NAVLoadVirusDef(m_hNAVEngine, m_uEntryNum);
}

void	CNAVLoadVirusDef::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	m_uEntryNum = (UINT) pRPCSend->wWord1;
}


// *****************************************************************
//                          CNAVReleaseVirusDef
// *****************************************************************

LPVOID	CNAVReleaseVirusDef::MakeAPICall()
{
	NAVReleaseVirusDef(m_hVirusDef);
	return NULL;
}

void	CNAVReleaseVirusDef::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	if (0 != pRPCSend->dwVirusDef)
		m_hVirusDef = (HNAVVIRUSDEF) pRPCSend->dwVirusDef;
}

// *****************************************************************
//                          CNAVGetVirusDefInfo
// *****************************************************************

CNAVGetVirusDefInfo::CNAVGetVirusDefInfo()
{
	m_dwBufferSize = 0;
	m_hVirusDef = NULL;
	m_lpszResultBuffer = NULL;
	m_eVirInfo = (NAVVIRINFO) 0;
}

LPVOID	CNAVGetVirusDefInfo::MakeAPICall()
{
	return (LPVOID) NAVGetVirusDefInfo(m_hVirusDef, m_eVirInfo, m_lpszResultBuffer, &m_dwBufferSize);
}

void	CNAVGetVirusDefInfo::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	m_eVirInfo = (NAVVIRINFO) pRPCSend->wWord1;

	if (0 != pRPCSend->dwVirusDef)
		m_hVirusDef = (HNAVVIRUSDEF) pRPCSend->dwVirusDef;
}


// *****************************************************************
//                          CNAVGetVirusInfo
// *****************************************************************

CNAVGetVirusInfo::CNAVGetVirusInfo()
{
	m_dwBufferSize = 0;
	m_hVirus = NULL;
	m_lpszResultBuffer = NULL;
	m_eVirInfo = (NAVVIRINFO) 0;
}

LPVOID	CNAVGetVirusInfo::MakeAPICall()
{
	return (LPVOID) NAVGetVirusInfo(m_hVirus, m_eVirInfo, m_lpszResultBuffer, &m_dwBufferSize);
}

void	CNAVGetVirusInfo::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	m_eVirInfo = (NAVVIRINFO) pRPCSend->wWord1;

	if (0 != pRPCSend->dwVirus)
		m_hVirus = (HNAVVIRUS) pRPCSend->dwVirus;
}


// *****************************************************************
//                          CNAVGetVirusDefTableInfo
// *****************************************************************

CNAVGetVirusDefTableInfo::CNAVGetVirusDefTableInfo()
{
	m_dwBufferSize = 0;
	m_dwIndex = 0;
	m_hVirusDefTable = NULL;
	m_lpszResultBuffer = NULL;
	m_eVirInfo = (NAVVIRINFO) 0;
}

LPVOID	CNAVGetVirusDefTableInfo::MakeAPICall()
{
	return (LPVOID) NAVGetVirusDefTableInfo(m_hVirusDefTable, m_dwIndex, m_eVirInfo, m_lpszResultBuffer, &m_dwBufferSize);
}

void	CNAVGetVirusDefTableInfo::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	m_eVirInfo = (NAVVIRINFO) pRPCSend->wWord1;
	m_dwIndex = (DWORD) pRPCSend->wWord2;

	if (0 != pRPCSend->dwVirusDefTable)
		m_hVirusDefTable = (HNAVVIRUSDEFTABLE) pRPCSend->dwVirusDefTable;
}


// *****************************************************************
//                          CNAVGetVirusDefTableCount
// *****************************************************************

LPVOID	CNAVGetVirusDefTableCount::MakeAPICall()
{
	return (LPVOID) NAVGetVirusDefTableCount(m_hVirusDefTable, &m_dwVirusDefCount);
}

void	CNAVGetVirusDefTableCount::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	if (0 != pRPCSend->dwVirusDefTable)
		m_hVirusDefTable = (HNAVVIRUSDEFTABLE) pRPCSend->dwVirusDefTable;
}

// *****************************************************************
//                          CNAVScanFile
// *****************************************************************

CNAVScanFile::CNAVScanFile()
{
	m_phVirus = NULL;
	m_bExecutable = TRUE;
	m_bPreserveLastAccess = TRUE;
	m_lpszFileName = NULL;
}

CNAVScanFile::~CNAVScanFile()
{
	if (m_lpszFileName != NULL)
		delete [] m_lpszFileName;
}

void CNAVScanFile::SetFileId(LPVOID  lpszFileId)
{
	if (m_lpszFileName != NULL)
	{
		delete [] m_lpszFileName;
		m_lpszFileName = NULL;
	}
	if (lpszFileId != NULL)
	{
		m_lpszFileName = new TCHAR[strlen( (LPTSTR) lpszFileId) + 1];
		strcpy(m_lpszFileName, (LPTSTR) lpszFileId);
	}
}

LPVOID	CNAVScanFile::MakeAPICall()
{
	return (LPVOID) NAVScanFile(m_hNAVEngine, (LPVOID) m_lpszFileName, m_bExecutable, m_bPreserveLastAccess, m_phVirus);
}

void	CNAVScanFile::SetOptions(LPBYTE lpbStruct)
{
	SENDSTRUCT * pRPCSend = (SENDSTRUCT *) lpbStruct;

	if (pRPCSend->lpszStr1 != NULL)
	{
		m_lpszFileName = new TCHAR[strlen(pRPCSend->lpszStr1) + 1];
		strcpy(m_lpszFileName, pRPCSend->lpszStr1);
	}

	if (0 != pRPCSend->dwVirus)
		m_phVirus = (HNAVVIRUS *) pRPCSend->dwVirus;

	if (0 == pRPCSend->wWord1)
		m_bExecutable = FALSE;
	else
		m_bExecutable = TRUE;

	if (0 == pRPCSend->wWord2)
		m_bPreserveLastAccess = FALSE;
	else
		m_bPreserveLastAccess = TRUE;
}


// *****************************************************************
//                       CNAVGetNavapiVersionString
// *****************************************************************

CNAVGetNavapiVersionString::CNAVGetNavapiVersionString()
{
	m_uBufferSize = 0;
	m_lpszVersionBuffer = NULL;
}

LPVOID	CNAVGetNavapiVersionString::MakeAPICall()
{
	return (LPVOID) NAVGetNavapiVersionString(m_lpszVersionBuffer, m_uBufferSize);
}


// *****************************************************************
//                       CNAVGetVirusDBInfo
// *****************************************************************

CNAVGetVirusDBInfo::CNAVGetVirusDBInfo()
{
	m_VirusDBInfo.dwVersion = 0;
	m_VirusDBInfo.wDay = 0;
	m_VirusDBInfo.wMonth = 0;
	m_VirusDBInfo.wYear = 0;
}

LPVOID	CNAVGetVirusDBInfo::MakeAPICall()
{
	return (LPVOID) NAVGetVirusDBInfo(m_hNAVEngine, &m_VirusDBInfo);
}

CNAVGetVirusDBInfo::~CNAVGetVirusDBInfo()
{

}