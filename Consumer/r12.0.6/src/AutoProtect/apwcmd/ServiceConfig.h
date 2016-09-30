#pragma once

#include "readwritesync.h"
#include <vector>

// APWCMDNT only needs a fraction of the options that the service does
class CServiceConfig  
{
public:
	CServiceConfig();
	virtual ~CServiceConfig();
	void Load();
	
	// Accessors.
	bool GetAttemptRepair();
	DWORD GetBloodhoundLevel();
    
	LPCTSTR GetProductVersion();

private:
	// Repair flag.
	bool		m_bAttemptRepair;

	// Bloodhound level
	DWORD		m_dwBloodhoundLevel;

	// Synchronization object.
	CReadWriteSync	m_Sync;

    // Last known defs path
    std::basic_string<char> m_sDefsPath;
	
	// product version
	std::string		m_sProductVersion;
};
