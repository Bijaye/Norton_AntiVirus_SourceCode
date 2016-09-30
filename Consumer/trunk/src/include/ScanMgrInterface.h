////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SCANMGR_INTERFACE_H
#define SCANMGR_INTERFACE_H

#include "SymInterface.h"

#define SYM_MAX_LONG_PATH 65536

///////////////////////////////////////////////////////////////////////////////
//
// SMRESULT (Scan Manager Result)
//

enum SMRESULT
{
	SMRESULT_OK = 0,
	SMRESULT_ABORT,
	SMRESULT_ERROR_SCANNER,
	SMRESULT_ERROR_SCANNER_CRITICAL,
	SMRESULT_ERROR_VIRUS_DEFS,
	SMRESULT_ERROR_OPTIONS,
	SMRESULT_ERROR_EXCLUSIONS,
	SMRESULT_ERROR_EXTENSION_LIST,
	SMRESULT_ERROR_MEMORY,
	SMRESULT_ERROR_COMMON_UI,
    SMRESULT_ERROR_INVALID_SIGNATURE,
	SMRESULT_ERROR_UNKNOWN,
    SMRESULT_ERROR_DEF_AUTHENTICATION_FAILURE,
    SMRESULT_ERROR_ERASER,
    SMRESULT_ERROR_ERASER_REBOOT_REQUIRED
};

///////////////////////////////////////////////////////////////////////////////
//
// SMSETTING (Scan Manager Options)
//

enum SMSETTING
{
	// Scan in OEM mode?
	//SMSETTING_OEM, // Deprecated!!!!

	// Is this an email scan?
	SMSETTING_EMAIL,

	// Is this an Instant Messanger scan?
	SMSETTING_IM,

	// Enable Outgoing Email Heurisitics?
	SMSETTING_OEH,

	// Perform a silent quarantine/delete for OEH infections?
	// Only used if SMSETTING_OEH is set.
	SMSETTING_OEH_SILENT,

	// Show the Common UI as an alert (i.e. always on top)?
	SMSETTING_ALERT,

	// Enable quarantine repair backups?
	SMSETTING_BACKUP_REPAIRS,

	// Use in-memory decompsition?
	SMSETTING_IN_MEMORY_DECOMPOSITION,

	// Traverse through subfolders?
	SMSETTING_SCAN_SUBFOLDERS,

	// Scan memory?
	SMSETTING_SCAN_MEMORY,

	// Scan boot records?
	SMSETTING_SCAN_BOOTRECS,

	// Scan master boot records?
	SMSETTING_SCAN_MBR,

	// Enable the decomposers?
	SMSETTING_SCAN_COMPRESSED_FILES,

	// Use SmartScan?
	//SMSETTING_SMARTSCAN, // deprecated

	// Log the beginning and end of a scan?
	SMSETTING_LOG_START_STOP,

	// Log infections?
	SMSETTING_LOG_INFECTIONS,

	// Show the progress bar during repairs?
	SMSETTING_SHOW_REPAIR_PROGRESS,

	// Scan files?
	SMSETTING_SCAN_FILES,

	// Scan files on network drives?
	SMSETTING_SCAN_NETWORK_FILES,

    // Scan for non-viral threats?
    SMSETTING_SCAN_NON_VIRAL_THREATS,

    // Backup non-viral threats before attempting delete?
    SMSETTING_BACKUP_DELETED_THREATS,

    // THIS DEBUG SETTING IS NO LONGER USED, THE SAME FUNCTIONALITY CAN BE
    // ACCOMPLISHED BY CREATING THE CCSETTING /Norton Antivirus/Navopts.dat/SCANNER/ScanThreads
    // and putting the value to 0
    SMSETTING_DISABLE_MULTITHREADING,

    // Eraser Scan enabled?
    SMSETTING_RUN_ERASER_SCAN,

    // Disable memory side effect scanning?
    SMSETTING_DONT_SCAN_MEMORY,

    // Scan DLLs?
    SMSETTING_SCAN_DLLS,

    // Client Compliancy?
    SMSETTING_SCAN_CLIENT_COMPLIANCY,

    // Enable generic load point scanning in the eraser filter?
    SMSETTING_SCAN_GENERIC_LOAD_POINTS,

    // Is this an office scan?
    SMSETTING_OFFICE
};

///////////////////////////////////////////////////////////////////////////////
//
// SMUSERINTEFFACE (Scan Manager User Interface)
//

enum SMUSERINTERFACE
{
	// Display full UI (progress, repair, summary).
	SMUI_COMPLETE = 0,

	// Display no UI no matter what.
	SMUI_NO_UI,

	// Display UI (repair, summary) only if one or more infections were detected.
	SMUI_NO_UI_UNLESS_INFECTIONS,

	// Display UI (repair, summary) only if one or more infections remain (after repairs).
	SMUI_NO_UI_UNLESS_INFECTIONS_REMAIN,

	// Display progress only (no repair or summary).
	SMUI_NO_RESULTS
};

///////////////////////////////////////////////////////////////////////////////
//
// SMRESPONSE (Scan Manager Response to Infections)
//

enum SMRESPONSE
{
	// Do not perform any automatic repairs.
	SCAN_MODE_MANUAL = 0,

	// Repair infections automatically.
	SCAN_MODE_AUTO_REPAIR,

	// Repair infections automatically. If unable to repair, quarantine the infection.
	SCAN_MODE_AUTO_REPAIR_QUARANTINE,

	// Repair infections automatically. If unable to repair, delete the infection.
	SCAN_MODE_AUTO_REPAIR_DELETE
};

///////////////////////////////////////////////////////////////////////////////
//
// SMNONVIRALRESPONSE (Scan Manager Response to Non-Viral threats)
//

enum SMNONVIRALRESPONSE
{
	// Do not perform any automatic deletes.
	THREAT_SCAN_MODE_MANUAL = 0,

	// Deletes threats automatically.
	THREAT_SCAN_MODE_AUTO_DELETE,

    // Uses the damage flag to determine the action
    THREAT_SCAN_MODE_USE_DAMAGE_FLAG
};

///////////////////////////////////////////////////////////////////////////////
//
// IScanManagerW

class IScanManagerW : public ISymBase
{
public:
	// Kicks off the scan/repair.
	virtual SMRESULT ScanAndRepair(const wchar_t* szAppID) = 0;

	// Adds an item to the items to scan list.
	// An item must be a full path to a file, folder or drive.
	virtual bool AddItemToScanList(const wchar_t* szItem) = 0;

    virtual bool AddItemToScanList(const wchar_t* szItem, unsigned long ulVID, bool bCompressed) = 0;

	// Removes all items from the items to scan list.
	virtual void ResetScanList() = 0;

	// Returns the number of items in the scan list.
	virtual int GetScanListItemCount() = 0;

	// Overrides a specific setting.
	// Defaults to showing progress and results.
	// Other default settings are read from NAV Options.
	virtual void OverrideSetting(SMSETTING Setting, bool bEnable) = 0;

	// Overrides Scan Manager's response setting.
	// Default setting is read from NAV Options.
	virtual void OverrideResponse(SMRESPONSE Response) = 0;

    // Overrides Scan Manager's non-viral threat response setting.
	// Default setting is manual.
	virtual void OverrideNonViralResponse(SMNONVIRALRESPONSE Response) = 0;

	// Overrides Bloodhound protection level (0-3).
	// Default setting is read from NAV Options.
	virtual void OverrideBloundhoundLevel(int iLevel) = 0;

	// Overrides the maximum decomposer extraction size.
	// Default setting is 2GB.
	virtual void OverrideMaxDecomposerExtractSize(unsigned long uBytes) = 0;

	// Overrides the maximum decomposer extraction depth.
	// Default setting is 10 levels.
	virtual void OverrideMaxDecomposerExtractDepth(int iLevels) = 0;

	// Sets the user-interface (if any) desired.
	// Defaults to SMUI_Default.
	virtual void SetUserInterface(SMUSERINTERFACE UserInterface) = 0;

	// Sets e-mail information (only used by NAVProxy).
	// This information is used only if the SMSETTING_EMAIL option is set.
	virtual void SetEmailInfo(const wchar_t* szSubject, const wchar_t* szSender,
		const wchar_t* szRecipient) = 0;

	// Sets OEH scanning information.
	// This information is used only if SMSETTING_OEH is enabled.
	virtual void SetOEHInfo(DWORD dwClientPID, const wchar_t* szClientImage) = 0;

	// Sets the parent window handle for the Common UI.
	// By default, Scan Manager uses NULL (i.e. the desktop window).
	virtual void SetParentWindow(HWND hWnd) = 0;

	// Returns the number of infections that were found during the last scan/repair.
	virtual int GetTotalInfectionsFound() = 0;

	// Returns the number of infections that were fixed during the last scan/repair.
	// This number includes infections that were deleted and/or quarantined.
	virtual int GetTotalInfectionsFixed() = 0;

	// Set the name of the scan for UI purposes
	virtual void SetScanName(const wchar_t* szScanName) = 0;

	// Deletes the object.
	virtual void DeleteObject() = 0;
};
// {25468BC7-8428-44bb-886F-CD4283D76E90}
SYM_DEFINE_INTERFACE_ID(IID_IScanManagerW, 
		0x25468bc7, 0x8428, 0x44bb, 0x88, 0x6f, 0xcd, 0x42, 0x83, 0xd7, 0x6e, 0x90);


///////////////////////////////////////////////////////////////////////////////
//
// Function used to create a Scan Manager object. Exported by ScanMgr.dll
//

extern "C"
{
	SMRESULT WINAPI GetScanManager(IScanManagerW**);
	typedef SMRESULT (WINAPI * pfnGETSCANMANAGER)(IScanManagerW**);
}

#endif

