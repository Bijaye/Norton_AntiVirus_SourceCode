////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AVNamesMap.cpp
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVNamesMap.h"
#include "AVccModuleId.h"
#include "AllNavEvents.h"
#include "..\navlogvres\resource.h"
#include "ISVersion.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVFeatureNameMap::CAVFeatureNameMap()
{
    Names[0] = IDS_FEATURE_UNKNOWN;
    Names[AV_MODULE_ID_BASE] = IDS_FEATURE_UNKNOWN;
	Names[AV_MODULE_ID_NAVLNCH] = IDS_FEATURE_NAVLNCH;
	Names[AV_MODULE_ID_STATUS] = IDS_FEATURE_STATUS;
	Names[AV_MODULE_ID_OPTIONS_UI] = IDS_FEATURE_OPTIONS_UI;
    Names[AV_MODULE_ID_OPTIONS_UI_SCRIPT] = IDS_FEATURE_OPTIONS_UI;
	Names[AV_MODULE_ID_SCRIPT_BLOCKING_UI] = IDS_FEATURE_SCRIPT_BLOCKING_UI;
	Names[AV_MODULE_ID_INFO_WIZARD] = IDS_FEATURE_INFO_WIZARD;
	Names[AV_MODULE_ID_DEF_ALERT] = IDS_FEATURE_DEF_ALERT;
	Names[AV_MODULE_ID_NAVW] = IDS_FEATURE_SCANNER;
	Names[AV_MODULE_ID_OFFICEAV] = IDS_FEATURE_OFFICEAV;
	Names[SAVRT_MODULE_ID_NAVAPSVC] = IDS_FEATURE_AP;
	Names[AV_MODULE_ID_NAVAPW32] = IDS_FEATURE_AP;
	Names[AV_MODULE_ID_SCANMANAGER] = IDS_FEATURE_SCANNER;
	Names[SAVRT_MODULE_ID_DRIVER] = IDS_FEATURE_AP_DRIVER;
	Names[AV_MODULE_ID_QUARANTINE] = IDS_FEATURE_QUARANTINE;
	Names[SAVRT_MODULE_ID_NAVAPSCR] = IDS_FEATURE_AP;
    Names[AV_MODULE_ID_EMAIL_SCAN] = IDS_FEATURE_EMAIL_SCAN;
    Names[AV_MODULE_ID_OEH_SCAN] = IDS_FEATURE_EMAIL_WORM_SCAN;
}

CString CAVFeatureNameMap::GetFeatureName(long lFeatureID)
{
    // Need to handle the product name specially
    if( lFeatureID == AV_MODULE_ID_MAIN_UI )
    {
        if( m_csProdName.IsEmpty() )
        {
			m_csProdName = CISVersion::GetProductName();
        }

        return m_csProdName;
    }

    CString csRet = CResourceHelper::LoadResourceString(Names[lFeatureID], _Module.GetResourceInstance()).c_str();
    return csRet;
}


CAVActionTakenMap::CAVActionTakenMap()
{
    Names[AV::Event_Action_Access_Denied] = IDS_ACTION_ACCESS_DENIED;
    Names[AV::Event_Action_Allowed] = IDS_ACTION_ALLOWED;
    Names[AV::Event_Action_Blocked] = IDS_ACTION_BLOCKED;
    Names[AV::Event_Action_Authorized] = IDS_ACTION_AUTHORIZED;
    Names[AV::Event_Action_Repaired] = IDS_ACTION_REPAIRED;
    Names[AV::Event_Action_RepairFailed] = IDS_ACTION_REPAIR_FAILED;
    Names[AV::Event_Action_Quarantined] = IDS_ACTION_QUARANTINED;
    Names[AV::Event_Action_QuarantinedFailed] = IDS_ACTION_QUARANTINE_FAILED;
    Names[AV::Event_Action_Deleted] = IDS_ACTION_DELETED;
    Names[AV::Event_Action_DeleteFailed] = IDS_ACTION_DELETE_FAILED;
    Names[AV::Event_Action_BlockFailed] = IDS_ACTION_BLOCK_FAILED;
    Names[AV::Event_Action_AuthorizedFailed] = IDS_ACTION_AUTHORIZE_FAILED;
    Names[AV::Event_Action_NoActionTaken] = IDS_ACTION_NO_ACTION;
    Names[AV::Event_Action_Excluded] = IDS_ACTION_EXCLUDED;
    Names[AV::Event_Action_Exclude_Failed] = IDS_ACTION_EXCLUDE_FAILED;
    Names[AV::Event_Action_Shutdown] = IDS_ACTION_SHUTDOWN;
    Names[AV::Event_Action_Unknown] = IDS_ACTION_UNKNOWN;
    Names[AV::Event_Action_Allowed_Method] = IDS_ACTION_ALLOWED_METHOD;
    Names[AV::Event_Action_Auto_Deleted] = IDS_ACTION_AUTODELETE;
    Names[AV::Event_Action_Removed] = IDS_ACTION_REMOVED;
	Names[AV::Event_Action_Restored] = IDS_ACTION_RESTORED;
	Names[AV::Event_Action_FullyRemoved] = IDS_ACTION_FULLYREMOVED;	
	Names[AV::Event_Action_PartiallyRemoved] = IDS_ACTION_PARTIALLYREMOVED;
	Names[AV::Event_Action_RemoveNotAttempted] = IDS_ACTION_REMOVENOTATTEMPTED;
	Names[AV::Event_Action_CompressedInProcessing] = IDS_ACTION_COMPRESSEDINPROCESSING;
	Names[AV::Event_Action_BackupOnly] = IDS_ACTION_BACKUPONLY;
	Names[AV::Event_Action_DoNotDelete] = IDS_ACTION_DONOTDELETE;
	Names[AV::Event_Action_RemoveOnRebootFailed] = IDS_ACTION_REMOVEONREBOOTFAILED;
	Names[AV::Event_Action_CannotRemediate] = IDS_ACTION_CANNOTREMEDIATE;
}

CAVActionAttemptedMap::CAVActionAttemptedMap()
{
    Names[AV::Event_VirusLike_Attempted_Action_HD_Format] = IDS_ACTION_HD_FORMAT;
    Names[AV::Event_VirusLike_Attempted_Action_HD_MBR_Write] = IDS_ACTION_HD_MBR_WRITE;
    Names[AV::Event_VirusLike_Attempted_Action_HD_Boot_Write] = IDS_ACTION_HD_BOOT_WRITE;
    Names[AV::Event_VirusLike_Attempted_Action_FD_Boot_Write] = IDS_ACTION_FD_BOOT_WRITE;
}

CAVScanActionMap::CAVScanActionMap()
{
    Names[AV::Event_ScanAction_ScanStarted] = IDS_SCAN_ACTION_STARTED;
    Names[AV::Event_ScanAction_ScanAborted] = IDS_SCAN_ACTION_ABORTED;
    Names[AV::Event_ScanAction_ScanCompleted] = IDS_SCAN_ACTION_COMPLETED;
    Names[AV::Event_ScanAction_MemorySideEffect] = IDS_SCAN_ACTION_MEMORY;
}
