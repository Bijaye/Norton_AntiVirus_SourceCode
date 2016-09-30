// ServiceConfig.h: interface for the CServiceConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICECONFIG_H__752E550D_2305_4CB0_B960_F9EE49858B45__INCLUDED_)
#define AFX_SERVICECONFIG_H__752E550D_2305_4CB0_B960_F9EE49858B45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "readwritesync.h"
#include <vector>


class CServiceConfig
{
public:
	CServiceConfig();
	virtual ~CServiceConfig();
	void Load();
	
	// Accessors.
    bool GetDetectSpyware();
    bool GetLoadAtStartup();
	bool GetStartupScan();
	bool GetShutdownScan();
	bool GetScanBootRecords();
	bool GetScanMasterBootRecords();
	bool GetAttemptRepair();
	bool GetShouldLog();
	DWORD GetBloodhoundLevel();
	LPCTSTR GetComputerName();
    bool GetShowBlockedAppAlerts();
    LPCSTR GetCurrentDefsPath();
    
	LPCTSTR GetProductVersion();

    LPCTSTR GetDefsRevision ();
    bool SetDefsRevision ( LPCTSTR lpszRevision );

    bool UpdateSavrtDefsPath ( LPCSTR lpszDefPath );

    LPCSTR GetTestDefsPath();
    bool GetFeatureEnabled();

private:
    // Send blocked app notifications
    bool        m_bShowBlockedApp;

	// Startup scan flag.
	bool		m_bStartupScan;

	// Shutdown scan flag.
	bool		m_bShutdownScan;

	// Scan Boot records flag;
	bool		m_bScanBootRecords;

    // AP is set to load at startup;
    bool        m_bLoadAtStartup;

	// Scan Master Boot records flag;
	bool		m_bScanMasterBootRecords;

	// Repair flag.
	bool		m_bAttemptRepair;

	// Log flag.
	bool		m_bLog;

	// Bloodhound level
	DWORD		m_dwBloodhoundLevel;

    // Spyware detection on/off (used to reload the drivers)
    bool       m_bDetectSpyware;

	// This computer's name.
    std::string		m_sComputerName;

	// Synchronization object.
	CReadWriteSync	m_Sync;

    // Currently used defs revision "YYYYMMDDRRRR"
    std::string     m_sDefsRevision;

    // Last known defs path
    std::basic_string<char> m_sDefsPath;
	
    // Full path to temporary unsigned defs
    std::string m_strTestDefsPath;

	// product version
	std::string		m_sProductVersion;

    // Feature enabled, from licensing
    bool m_bFeatureEnabled;
};

#endif // !defined(AFX_SERVICECONFIG_H__752E550D_2305_4CB0_B960_F9EE49858B45__INCLUDED_)
