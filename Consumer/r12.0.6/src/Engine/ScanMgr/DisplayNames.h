
#ifndef DISPLAYS_NAME_H
#define DISPLAYS_NAME_H

#include "CommonUIInterface.h"
#include "EZEraserObjects.h"

class CDisplayNames
{
public:
	// Constructor.
	CDisplayNames();

	// Sets the scan type.
	void SetScanType(bool bEmailScan, bool bDeepDelete);

    // Sets user data value in the anomaly for generic display information
    bool SetGenericText(ccEraser::IAnomaly* pAnomaly);

    // Sets user data value in scan infection remediation actions for display information
    bool SetScanInfectionRemdiationActionText(ccEraser::IRemediationAction* pScanRem);

    bool UpdateAllRemediationStatusText(CEZAnomaly* pEZAnomaly);
    bool SetRemediationStatusText(ccEraser::IRemediationAction* pAction, CommonUIInfectionStatus eStatus);

    // Return the full path to the scan infection item
    bool GetFullScanInfectionPath(IScanInfection* pInfection, ATL::CAtlString& strPath);

	// Returns the display file name for a given file or compressed file infection.
    static bool GetDisplayNameForFile(IScanInfection* pInfection, std::string& strFileName);

	// Replacement for Win32 GetLongPathName() which is not available on NT4.
	static bool GetLongPathName(const char* szShortPath, char* szLongPath);

private:
	// Is this an email scan?
	bool m_bEmailScan;

	// Are we Performing Deep Deletes?
	bool m_bDeepDelete;

private:
	// Converts an iso-2022-jp string to MBCS.
	static bool ISO2022JPToMBCS(const char* szSource, char* szTarget, int iTargetSize);

    // Converts an ISO_8859_2 string to MBCS.
    // Fix for Defect# 407220 where polish characters within a mime container are
    // not displayed properly
    static bool ISO8859_2ToMBCS(const char* szSource, char* szTarget, int iTargetSize);

	// Convert a utf-8 string to MBCS
	static bool UTF8ToMBCS(const char* szSource, char* szTarget, int iTargetSize);

	// Converts a string to the specified charset.
	static bool ConvertToCharSet(const char* szSource, char* szTarget, int iTargetSize, DWORD dwCharSet, bool bOemPaths);

    // Virus infection or non-viral threat
    bool IsNonViralThreat(IScanInfection* pInfection);

    // Converts forward slashes to backslahes
    void ConvertFwdSlashToBckSlash(LPTSTR pszStrToConvert);

    // Status text helper function
    bool GetRemediationStatus(ccEraser::eResult eResult, ccEraser::IRemediationAction::Operation_Type opType, ATL::CAtlString& strStatus);

};

#endif


