
#ifndef ACTIVITY_LOG_H
#define ACTIVITY_LOG_H

#include <lmcons.h>
#include "DisplayNames.h"
#include "avevents.h"
#include "FileAPI.h"
#include "navversion.h"     // Toolbox
#include "NAVEventFactoryLoader.h"
#include "EZEraserObjects.h"
#include "ScanManagerCounts.h"

enum ActivityState
{
	AState_Infected,
	AState_Repaired,
	AState_Repair_Failed,
	AState_Quarantined,
	AState_Quarantine_Failed,
	AState_Deleted,
	AState_Auto_Deleted,
	AState_Delete_Failed,
	AState_Allowed,
	AState_Blocked,
	AState_Authorized,
    AState_Excluded,
    AState_Exclude_Failed,
    AState_Removed
};

enum ActivityScanType
{
    AType_ManualScanner = 0,
    AType_EmailScanner,
    AType_OfficeScanner,
    AType_IMScanner
};

class CActivityLog
{
public:
	// Constructor.
	CActivityLog();
	
	//Destructor
	~CActivityLog();

	// Logs the start of a scan.
	bool LogScanStart(const ::std::string strScanName);

	// Logs an aborted scan.
	bool LogScanAbort(const ::std::string strScanName);

	// Logs a failed scan.
	bool LogScanError();

	// Logs the end of a scan (summary).
	bool LogScanSummary(CScanManagerCounts* pScanCounts, DWORD dwScanTime, const ::std::string strScanName);

	// Logs all infections found during a scan.
    bool LogAllInfections(ccEraser::IAnomalyList* pAnomList);

	// Log an OEH infection.
	bool LogOEHInfection(const char* szWormPath, ActivityState State);

    // Log process termination actions
    bool LogProcessTermination(ccEraser::IRemediationAction* pProcessRemediation, LPCTSTR pcszProdName, LPCSTR pcszThreatName);

    // Set the rev of the defs that found the infection. It's here so you
    // don't have to pass it in each time, only when it changes.
    void SetDefsRevision ( const ::std::string strDefsRevision  );

    // Set the scan type for display in the Source area when logging infections.
    // This defaults to manual scanner, other possible values defined in the ActivityScanType
    // enum above
    void SetScanType( int nType );

private:
	// The event logger object.
    AV::IAvEventFactoryPtr  m_pLogger;

    // Current defs revision
    ::std::string m_strDefsRevision;

    // Toolbox
    CNAVVersion m_Version;

private:

    // Builds the affected areas description to log for an anomaly using the remediation statistics
    void BuildAnomalyDescription(CRemediationStats* pRemStats, CommonUIInfectionStatus eAnomalyStatus, CAtlString& strDescription);

	// Returns the activity state of an infection.
	ActivityState GetActivityState(CommonUIInfectionStatus eStatus, bool bNonViral);

    // Converts the ActivityState into a loggable action value.
    long GetLogAction (ActivityState AState);

	// Creates the logger object if needed
	bool GetLogger();

    ActivityScanType m_ScanType;
};

#endif

