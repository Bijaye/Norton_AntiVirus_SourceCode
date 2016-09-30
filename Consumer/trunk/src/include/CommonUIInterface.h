////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//
// NAV Common UI Header File
//

#ifndef NAV_COMMONUI_INTERFACE_H
#define NAV_COMMONUI_INTERFACE_H

#include "ccScanwInterface.h"
#include "ccEraserInterface.h"
#include "SymInterface.h"

///////////////////////////////////////////////////////////////////////////////
//
// Type of Common UI.

enum CommonUIType
{
    CommonUI_Complete,
    CommonUI_RepairAndSummary,
    CommonUI_ProgressOnly
};

enum ScanInfectionTypes
{
    FileInfectionType = 0,
    CompressedInfectinType,
    MemoryInfectionType,
    BRInfectionType,
    MBRInfectionType
};

enum RemediationActionUserDataKeys
{
    RemediationDescription = 0, // eInterfaceValue(IString*)
    ScanInfectionType,          // eUint32Value (ScanInfectionTypes value)
    RemediationStatusText,      // eInterfaceValue(IString*)
    RemediationRequiresRebooot, // eBoolValue
    RemediationQBItemGUID,      // eGuidValue
    RemediationPresent,         // eBoolValue
    ActiveInMemory,             // eBoolValue
    RemediationQBSetGUID,       // eGuidValue
};

enum AnomalyUserDataKeys
{
    FileVidDetectionPath = 0,  // eInterfaceValue (IString*)
    FileVidDetectionVID,       // eUInt32Value (VID)
    CurrentStatus,             // eUint32Value (CommonUIInfectionStatus value)
    ActionToTake,              // eUint32Value (AnomalyAction value)
    GenericDescription,        // eInterfaceValue (IString*)
    AnomalyRequiresReboot,     // eBoolValue
    AnomalyQBItemGUID,         // eGuidValue
    AnomalyMemBootType,        // eUint32Value (ScanInfectionTypes value)
    AnomalySelectedState,      // eBoolValue
    SingleInfectionAnomaly     // eBoolValue
};

enum AnomalyAction
{
    ACTION_REPAIR,
    ACTION_QUARANTINE,
    ACTION_REPAIRTHENQUARANTINE,
    ACTION_DELETE,
    ACTION_REPAIRTHENDELETE,
    ACTION_EXCLUDE,
    ACTION_IGNORE,
    ACTION_COMPLETE,
    ACTION_NOOPERATIONSAVAILABLE,
    ACTION_REVIEW
};

inline bool AnomalyNeedsAction( AnomalyAction eAction )
{
    if( eAction == ACTION_REPAIR ||
        eAction == ACTION_QUARANTINE ||
        eAction == ACTION_REPAIRTHENQUARANTINE ||
        eAction == ACTION_DELETE ||
        eAction == ACTION_REPAIRTHENDELETE ||
        eAction == ACTION_EXCLUDE )
        return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////
//
// Infection user data status.
//
// Additional status information about an infection is stored in an anomaly
// object's user data field.
//
// +-------------+------------------------------------+--------------------------+
// | STATUS      | USER DATA STATUS                   | EFFECTIVE RESULT         |
// +=============+====================================+==========================+
// | REPAIRED    | InfectionStatus_Repaired           | Repaired.                |
// +-------------+------------------------------------+--------------------------+
// | DELETED     | InfectionStatus_Deleted            | Deleted                  |
// +-------------+------------------------------------+--------------------------+
// | QUARANTINED | InfectionStatus_Quarantined        | Quarantined.             |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_Unhandled or       | Still infected.          |
// |             | no user data field exists          |                          |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_Quarantine_Failed  | Still infected.          |
// |             |                                    | Unable to quarantine.    |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_Repair_Failed      | Still infected.          |
// |             |                                    | Unable to repair.        |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_Delete_Failed      | Still infected.          |
// |             |                                    | Unable to delete.        |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_Excluded           | Non-Viral Threat.        |
// |             |                                    | Excluded forever         |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_Exclude_Failed     | Non-Viral Threat         |
// |             |                                    | Unable to Exclude        |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_CantDelete         | Engine flag to not       |
// |             |                                    | remove is set            |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_AccessDenied       | ccEraser remediation     |
// |             |                                    | returned access denied   |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_NotDetected        | File not found           |
// |             |                                    |                          |
// +-------------+------------------------------------+--------------------------+
// | INFECTED    | InfectionStatus_RebootRequired     | Infected file marked     |
// |             |                                    | for reboot               |
// +-------------+------------------------------------+--------------------------+
enum CommonUIInfectionStatus
{
    InfectionStatus_Unhandled = 0,
    InfectionStatus_Repair_Failed,
    InfectionStatus_Quarantine_Failed,
    InfectionStatus_Delete_Failed,
    InfectionStatus_Exclude_Failed,
    InfectionStatus_Repaired,
    InfectionStatus_Quarantined,
    InfectionStatus_Deleted,
    InfectionStatus_Excluded,
    InfectionStatus_CantDelete,
    InfectionStatus_AccessDenied,
    InfectionStatus_NotDetected,
    InfectionStatus_RebootRequired
};

inline bool InfectionStatusFailure( CommonUIInfectionStatus eStatus )
{
    if( eStatus == InfectionStatus_Unhandled ||
        eStatus == InfectionStatus_Repair_Failed ||
        eStatus == InfectionStatus_Quarantine_Failed ||
        eStatus == InfectionStatus_Delete_Failed ||
        eStatus == InfectionStatus_Exclude_Failed ||
        eStatus == InfectionStatus_CantDelete ||
        eStatus == InfectionStatus_AccessDenied )
        return true;

    return false;
}

enum MemoryScanStatus
{
    MemoryScanned = 1,
    MemoryNotScanned,
    MemoryScanNotAvailable,
};

//
// ICommonUISink
//
// This interface is implemented by clients of the Common UI Dialog. It lets 
// the client know when a user wants to pause, resume and stop a scan. It also
// informs the client when a specific operation (Repair, Quarantine and Delete)
// needs to be performed.
//

struct ICommonUISink : public ISymBase
{
    // Call this to signal UI is up and running.
    virtual HRESULT OnCUIReady() = 0;
    // Stop scanning.
    virtual HRESULT OnCUIAbort() = 0;
    // Pause the scan.
    virtual HRESULT OnCUIPause() = 0;
    // Resume the scan.
    virtual HRESULT OnCUIResume() = 0;

    // Deal with the anomlies.
    virtual HRESULT OnCUIHandleAnomalies() = 0;
    
    // Progress status
    virtual HRESULT OnCUIGetCurrentFolder(LPWSTR szFolder, long nLength) = 0;
    virtual HRESULT OnCUIScanFileCount(long& nScanFileCount) = 0;
    virtual HRESULT OnCUIVirusCount(long& nVirusCount) = 0;
    virtual HRESULT OnCUIRemovedVirusCount(long& nRemovedVirusCount) = 0;
    virtual HRESULT OnCUINonViralCount(long& nNonViralCount) = 0;
    virtual HRESULT OnCUIRemovedNonViralCount(long& nRemovedNonViralCount) = 0;
    virtual HRESULT OnCUIGetQuarantinedCount(long& nQuarantineCount) = 0;
    virtual HRESULT OnCUIAdditionalScan(bool& bAdditionalScan) = 0;
};
SYM_DEFINE_INTERFACE_ID(IID_ICommonUISink, 
0x7737b7f3, 0x4def, 0x4dc9, 0xb1, 0x5c, 0x0, 0xf7, 0x1a, 0xe0, 0x4f, 0x4d);


struct ICommonUI : public ISymBase
{
    virtual HRESULT ShowCUIDlg(/* [in] */ CommonUIType UIType, /* [in] */ ccEraser::IAnomalyList* pAnomalyList, /* [in] */ ICommonUISink* pSink, /* [in] */const long lDamageControlItemsRemaining) = 0;
    virtual HRESULT SetScanComplete(/* [in] */ ccEraser::IAnomalyList* pAnomalyList, const long lDamageControlItemsRemaining ) = 0;
    virtual HRESULT SetEmailInfo(/* [in] */ const WCHAR* szSubject,/* [in] */ const WCHAR* szSender, /* [in] */ const WCHAR* szRecipient) = 0;
    virtual HRESULT SetMemoryScanStatus(/* [in] */ MemoryScanStatus MemScanStatus) = 0;
    virtual HRESULT SetScanCompressedFiles(/* [in] */ bool bScanCompressedFiles) = 0;
    virtual HRESULT SetScanTime(/* [in] */ unsigned long ulScanTime) = 0;
    virtual HRESULT SetRepairProgress(bool bRepairProgress) = 0;
    virtual HRESULT SetScanNonViralThreats(bool bScanNonViralThreats) = 0;
    virtual HRESULT SetQuickScan(bool bAttemptedAuto) = 0;
    virtual HRESULT UpdateRepairProgress(/* [in] */ unsigned long nItemsRemaining, /* [in] */ bool bFailuresOccured, /* [out] */bool& bAbort) = 0;
    virtual HRESULT ShowMemTerminationDlg(bool &bTerminate, bool bEnableTimeout) = 0;
    virtual HRESULT SetParentWindow(HWND hWndParent) = 0;
    virtual HRESULT ShowDetailsDlg(/*[in]*/ ccEraser::IAnomaly* pAnomaly, /*[in]*/ HWND hWndParent) = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_ICommonUI, 
0x8bee3fe1, 0x3fe9, 0x4eb8, 0x88, 0xe1, 0x17, 0xc7, 0xc1, 0xa5, 0x6c, 0x55);

///////////////////////////////////////////////////////////////////////////////
//
// Functions used to create an ICommonUI object.
// This function is exported by NAVComUI.dll.
//

extern "C"
{
    bool WINAPI CreateCommonUI(HWND hWndParent, ICommonUISink* pCommonUISink, ICommonUI** ppCommonUI);
    typedef bool (WINAPI * pfnCREATECOMMONUI)(HWND, ICommonUISink*, ICommonUI**);
}

#endif

