// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI.QA/VCS/apicall.h_v   1.3   11 Aug 1998 08:40:48   mmendon  $
//
// Description: Header file for APICall library file.  The CAPICall
//                              class is a family of NAVAPI function call wrappers
//
//************************************************************************
// $Log:   S:/NAVAPI.QA/VCS/apicall.h_v  $
// 
//    Rev 1.3   11 Aug 1998 08:40:48   mmendon
// Added a blank line after the endif so Win16 compiler would be happy
//
//    Rev 1.2   11 Aug 1998 07:28:50   mmendon
// Win16 didn't like the inline destructor.  Moved it to the source file.
//
//    Rev 1.1   10 Aug 1998 12:06:20   NMILLER
// Added inline destructor for CNAVGetVirusDBInfo
//
//    Rev 1.0   06 Aug 1998 12:15:40   NMILLER
// Initial revision.
//
//************************************************************************

#ifndef _APICALL_H
#define _APICALL_H

#include "platform.h"

#include "windows.h"
#include "navapi.h"


// *****************************************************************
//                          CAPICall
// *****************************************************************

// BASE CLASS--Pure virtual

class CAPICall
{
public:
	CAPICall() {};
	~CAPICall() {};

	virtual         LPVOID  MakeAPICall()           =       0;
	virtual         void    Log()                           =       0;
	virtual         void    SetOptions(LPBYTE)      =       0;
};


// *****************************************************************
//                       CEngineBasedAPICall
// *****************************************************************

// Secondary Base class -- Engine-Based API call

class CEngineBasedAPICall : public CAPICall
{
protected:
	HNAVENGINE      m_hNAVEngine;
public:
	CEngineBasedAPICall() {};
	~CEngineBasedAPICall() {};

	void    SetNAVEngine(HNAVENGINE hNAVEngine) {m_hNAVEngine = hNAVEngine;};
	virtual LPVOID  MakeAPICall()           =       0;
	virtual void    Log()                   =       0;
	void    SetOptions(LPBYTE);
};


// Derived classes, one for each API Call


// *****************************************************************
//                            CNAVEngineInit
// *****************************************************************

class CNAVEngineInit : public CAPICall
{
private:
	LPTSTR          m_sDefsPath;
	LPTSTR          m_sNavexInfFile;
	LPNAVFILEIO m_pFileCallbacks;
	WORD            m_wHeuristicLevel;
	WORD            m_wFlags;
	NAVSTATUS       m_dwStatus;
public:
	CNAVEngineInit();
	~CNAVEngineInit();
	void            SetDefsPath(LPTSTR szDefsPath);
	void            SetNavexInfFile(LPTSTR szNavexInfFile);
	void            SetFileCallbacks(LPNAVFILEIO lpFileCallbacks);
	void            SetHeuristicLevel(WORD wHeuristicLevel) {m_wHeuristicLevel = wHeuristicLevel;};
	void            SetFlags(WORD wFlags) {m_wFlags = wFlags;};
	NAVSTATUS       GetStatus() {return m_dwStatus;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVEngineClose
// *****************************************************************

class CNAVEngineClose : public CEngineBasedAPICall
{
public:
	CNAVEngineClose() {};
	~CNAVEngineClose() {};
	LPVOID  MakeAPICall();
	void    Log() {};
	void    SetOptions(LPBYTE) {};
};


// *****************************************************************
//                          CNAVScanBoot
// *****************************************************************

class CNAVScanBoot : public CEngineBasedAPICall
{
private:
	char            m_cDriveLetter;
	HNAVVIRUS * m_phVirus;
public:
	CNAVScanBoot();
	~CNAVScanBoot();
	void            SetDriveLetter(char cDriveLetter) {m_cDriveLetter = cDriveLetter;};
	void            SetVirus(HNAVVIRUS * lphVirus) {m_phVirus = lphVirus;};
	HNAVVIRUS               GetVirus(void) {return *m_phVirus;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVScanMasterBoot
// *****************************************************************

class CNAVScanMasterBoot : public CEngineBasedAPICall
{
private:
	UINT            m_uPhysDriveNum;
	HNAVVIRUS * m_phVirus;
public:
	CNAVScanMasterBoot();
	~CNAVScanMasterBoot();
	void            SetPhysDriveNum(UINT uPhysDriveNum) {m_uPhysDriveNum = uPhysDriveNum;};
	void            SetVirus(HNAVVIRUS * lphVirus) {m_phVirus = lphVirus;};
	HNAVVIRUS               GetVirus(void) {return *m_phVirus;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVScanMemory
// *****************************************************************

class CNAVScanMemory : public CEngineBasedAPICall
{
private:
	HNAVVIRUS * m_phVirus;
public:
	CNAVScanMemory();
	~CNAVScanMemory() {};
	void            SetVirus(HNAVVIRUS * lphVirus) {m_phVirus = lphVirus;};
	HNAVVIRUS               GetVirus(void) {return *m_phVirus;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE) {};
};


// *****************************************************************
//                          CNAVScanFile
// *****************************************************************

class CNAVScanFile : public CEngineBasedAPICall
{
private:
	HNAVVIRUS *             m_phVirus;
	LPTSTR          m_lpszFileName;
	BOOL            m_bExecutable;
	BOOL            m_bPreserveLastAccess;
public:
	CNAVScanFile();
	~CNAVScanFile();
	void            SetVirus(HNAVVIRUS * lphVirus) {m_phVirus = lphVirus;};
	HNAVVIRUS               GetVirus(void) {return *m_phVirus;};
	void            SetFileId(LPVOID lpFileId);
	void            SetExecutable(BOOL bExecutable) {m_bExecutable = bExecutable;};
	void            SetPreserveLastAccess(BOOL bPreserveLastAccess) {m_bPreserveLastAccess = bPreserveLastAccess;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVRepairFile
// *****************************************************************

class CNAVRepairFile : public CEngineBasedAPICall
{
private:
	LPTSTR          m_lpszFileName;
	BOOL            m_bExecutable;
public:
	CNAVRepairFile();
	~CNAVRepairFile();
	void            SetFileId(LPVOID lpFileId);
	void            SetExecutable(BOOL bExecutable) {m_bExecutable = bExecutable;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVDeleteFile
// *****************************************************************

class CNAVDeleteFile : public CEngineBasedAPICall
{
private:
	LPTSTR          m_lpszFileName;
	BOOL            m_bExecutable;
public:
	CNAVDeleteFile();
	~CNAVDeleteFile();
	void            SetFileId(LPVOID lpFileId);
	void            SetExecutable(BOOL bExecutable) {m_bExecutable = bExecutable;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVRepairBoot
// *****************************************************************

class CNAVRepairBoot : public CEngineBasedAPICall
{
private:
	char            m_cDriveLetter;
public:
	CNAVRepairBoot();
	~CNAVRepairBoot();
	void            SetDriveLetter(char cDriveLetter) {m_cDriveLetter = cDriveLetter;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};

// *****************************************************************
//                          CNAVRepairMasterBoot
// *****************************************************************

class CNAVRepairMasterBoot : public CEngineBasedAPICall
{
private:
	UINT            m_uPhysDriveNum;
public:
	CNAVRepairMasterBoot();
	~CNAVRepairMasterBoot();
	void            SetPhysDriveNum(UINT uPhysDriveNum) {m_uPhysDriveNum = uPhysDriveNum;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVFreeVirusHandle
// *****************************************************************

class CNAVFreeVirusHandle : public CAPICall
{
private:
	HNAVVIRUS   m_hVirus;
public:
	CNAVFreeVirusHandle() {};
	~CNAVFreeVirusHandle() {};
	void            SetVirus(HNAVVIRUS hVirus) {m_hVirus = hVirus;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVGetVirusDefCount
// *****************************************************************

class CNAVGetVirusDefCount : public CEngineBasedAPICall
{
private:
	DWORD           m_dwNumViruses;
public:
	CNAVGetVirusDefCount() {};
	~CNAVGetVirusDefCount() {};
	void            SetNumViruses(DWORD  dwNumViruses) {m_dwNumViruses = dwNumViruses;};
	DWORD           GetNumViruses(void) {return m_dwNumViruses;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE) {};
};


// *****************************************************************
//                          CNAVLoadVirusDefTable
// *****************************************************************

class CNAVLoadVirusDefTable : public CEngineBasedAPICall
{
public:
	CNAVLoadVirusDefTable() {};
	~CNAVLoadVirusDefTable() {};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE) {};
};


// *****************************************************************
//                          CNAVReleaseVirusDefTable
// *****************************************************************

class CNAVReleaseVirusDefTable : public CAPICall
{
private:
	HNAVVIRUSDEFTABLE       m_hVirusDefTable;
public:
	CNAVReleaseVirusDefTable() {};
	~CNAVReleaseVirusDefTable() {};
	void            SetVirusDefTable(HNAVVIRUSDEFTABLE hVirusDefTable) {m_hVirusDefTable = hVirusDefTable;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVLoadVirusDef
// *****************************************************************

class CNAVLoadVirusDef : public CEngineBasedAPICall
{
private:
	UINT            m_uEntryNum;
public:
	CNAVLoadVirusDef() {};
	~CNAVLoadVirusDef() {};
	void            SetEntryNum(UINT uEntryNum) {m_uEntryNum = uEntryNum;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVReleaseVirusDef
// *****************************************************************

class CNAVReleaseVirusDef : public CAPICall
{
private:
	HNAVVIRUSDEF            m_hVirusDef;
public:
	CNAVReleaseVirusDef() {};
	~CNAVReleaseVirusDef() {};
	void            SetVirusDef(HNAVVIRUSDEF hVirusDef) {m_hVirusDef = hVirusDef;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVGetVirusDefInfo
// *****************************************************************

class CNAVGetVirusDefInfo : public CAPICall
{
private:
	DWORD                           m_dwBufferSize;
	HNAVVIRUSDEF                                    m_hVirusDef;
	NAVVIRINFO                      m_eVirInfo;
	LPTSTR                          m_lpszResultBuffer;
public:
	CNAVGetVirusDefInfo();
	~CNAVGetVirusDefInfo() {};
	void            SetVirusDef(HNAVVIRUSDEF hVirusDef) {m_hVirusDef = hVirusDef;};
	void            SetVirInfo(NAVVIRINFO eVirInfo) {m_eVirInfo = eVirInfo;};
	void            SetResultBuffer(LPTSTR lpszResultBuffer) {m_lpszResultBuffer = lpszResultBuffer;};
	void            SetBufferSize(DWORD dwBufferSize) {m_dwBufferSize = dwBufferSize;};
	DWORD           GetBufferSize(void) {return m_dwBufferSize;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVGetVirusInfo
// *****************************************************************

class CNAVGetVirusInfo : public CAPICall
{
private:
	DWORD                           m_dwBufferSize;
	HNAVVIRUS                       m_hVirus;
	NAVVIRINFO                      m_eVirInfo;
	LPTSTR                          m_lpszResultBuffer;
public:
	CNAVGetVirusInfo();
	~CNAVGetVirusInfo() {};
	void            SetVirus(HNAVVIRUS hVirus) {m_hVirus = hVirus;};
	void            SetVirInfo(NAVVIRINFO eVirInfo) {m_eVirInfo = eVirInfo;};
	void            SetResultBuffer(LPTSTR lpszResultBuffer) {m_lpszResultBuffer = lpszResultBuffer;};
	void            SetBufferSize(DWORD dwBufferSize) {m_dwBufferSize = dwBufferSize;};
	DWORD           GetBufferSize(void) {return m_dwBufferSize;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVGetVirusDefTableInfo
// *****************************************************************

class CNAVGetVirusDefTableInfo : public CAPICall
{
private:
	DWORD               m_dwBufferSize;
	HNAVVIRUSDEFTABLE       m_hVirusDefTable;
	DWORD               m_dwIndex;
	NAVVIRINFO          m_eVirInfo;
	LPTSTR              m_lpszResultBuffer;
public:
	CNAVGetVirusDefTableInfo();
	~CNAVGetVirusDefTableInfo() {};
	void            SetVirusDefTable(HNAVVIRUSDEFTABLE hVirusDefTable)
										{m_hVirusDefTable = hVirusDefTable;};
	void            SetIndex(DWORD dwIndex) {m_dwIndex = dwIndex;};
	void            SetVirInfo(NAVVIRINFO eVirInfo) {m_eVirInfo = eVirInfo;};
	void            SetResultBuffer(LPTSTR lpszResultBuffer) {m_lpszResultBuffer = lpszResultBuffer;};
	void            SetBufferSize(DWORD dwBufferSize) {m_dwBufferSize = dwBufferSize;};
	DWORD           GetBufferSize(void) {return m_dwBufferSize;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                          CNAVGetVirusDefTableCount
// *****************************************************************

class CNAVGetVirusDefTableCount : public CAPICall
{
private:
	DWORD                                   m_dwVirusDefCount;
	HNAVVIRUSDEFTABLE       m_hVirusDefTable;
public:
	CNAVGetVirusDefTableCount() {};
	~CNAVGetVirusDefTableCount() {};
	void            SetNumViruses(DWORD  dwVirusDefCount) {m_dwVirusDefCount = dwVirusDefCount;};
	DWORD           GetNumViruses(void) {return m_dwVirusDefCount;};
	void            SetVirusDefTable(HNAVVIRUSDEFTABLE hVirusDefTable) {m_hVirusDefTable = hVirusDefTable;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE);
};


// *****************************************************************
//                     CNAVGetNavapiVersionString
// *****************************************************************

class CNAVGetNavapiVersionString : public CAPICall
{
private:
	UINT            m_uBufferSize;
	LPTSTR          m_lpszVersionBuffer;
public:
	CNAVGetNavapiVersionString();
    ~CNAVGetNavapiVersionString(){};
	void            SetVersionBuffer(LPTSTR lpszVersionBuffer) {m_lpszVersionBuffer = lpszVersionBuffer;};
	void            SetBufferSize(UINT uBufferSize) {m_uBufferSize = uBufferSize;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE) {};
};


// *****************************************************************
//                          CNAVGetVirusDBInfo
// *****************************************************************

class CNAVGetVirusDBInfo : public CEngineBasedAPICall
{
private:
	VIRUSDBINFO     m_VirusDBInfo;
public:
	CNAVGetVirusDBInfo();
    ~CNAVGetVirusDBInfo();
	LPVIRUSDBINFO   GetVirusDBInfo(void) {return &m_VirusDBInfo;};
	LPVOID          MakeAPICall();
	void            Log() {};
	void            SetOptions(LPBYTE) {};
};


#endif
