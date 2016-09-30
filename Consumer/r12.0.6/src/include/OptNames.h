// OptNames.h

#ifndef __OptNames_h__
#define __OptNames_h__

    /////////////////////////
    // USER Options
    /////////////////////////
static const TCHAR USER_Name[]    = _T("USER:Name");
static const TCHAR USER_Company[] = _T("USER:Company");

    /////////////////////////
    // ACTIVITY Options
    /////////////////////////
static const TCHAR ACTIVITY_LogKnown[]               = _T("ACTIVITY:LogKnown");
static const TCHAR ACTIVITY_LogUnknown[]             = _T("ACTIVITY:LogUnknown");
static const TCHAR ACTIVITY_LogInocChange[]          = _T("ACTIVITY:LogInocChange");
static const TCHAR ACTIVITY_LogVirusLike[]           = _T("ACTIVITY:LogVirusLike");
static const TCHAR ACTIVITY_LogStartEnd[]            = _T("ACTIVITY:LogStartEnd");
static const TCHAR ACTIVITY_LogVirusList[]           = _T("ACTIVITY:LogVirusList");
static const TCHAR ACTIVITY_LogKeepLast[]            = _T("ACTIVITY:LogKeepLast");
static const TCHAR ACTIVITY_LogQuarantine[]          = _T("ACTIVITY:LogQuarantine");
static const TCHAR ACTIVITY_LogEntries[]             = _T("ACTIVITY:LogEntries");
static const TCHAR ACTIVITY_LogKeepUpTo[]            = _T("ACTIVITY:LogKeepUpTo");
static const TCHAR ACTIVITY_LogKiloBytes[]           = _T("ACTIVITY:LogKiloBytes");
static const TCHAR ACTIVITY_LogAccessDenied[]        = _T("ACTIVITY:LogAccessDenied");
static const TCHAR ACTIVITY_LogEMailScanning[]       = _T("ACTIVITY:LogEmailScanning");
static const TCHAR ACTIVITY_LogIMScanning[]			 = _T("ACTIVITY:LogIMScanning");
static const TCHAR ACTIVITY_LogLiveUpdate[]          = _T("ACTIVITY:LogLiveUpdate");
static const TCHAR ACTIVITY_LogFile[]                = _T("ACTIVITY:LogFile");
static const TCHAR ACTIVITY_LogNameVirus[]          = _T("ACTIVITY:LogNameVirus");
static const TCHAR ACTIVITY_LogNameApp[]            = _T("ACTIVITY:LogNameApp");
static const TCHAR ACTIVITY_LogNameError[]          = _T("ACTIVITY:LogNameError");
static const TCHAR ACTIVITY_LogSizeVirus[]          = _T("ACTIVITY:LogSizeVirus");
static const TCHAR ACTIVITY_LogSizeApp[]            = _T("ACTIVITY:LogSizeApp");
static const TCHAR ACTIVITY_LogSizeError[]          = _T("ACTIVITY:LogSizeError");
static const TCHAR ACTIVITY_LogEnabledVirus[]       = _T("ACTIVITY:LogEnabledVirus");
static const TCHAR ACTIVITY_LogEnabledApp[]         = _T("ACTIVITY:LogEnabledApp");
static const TCHAR ACTIVITY_LogEnabledError[]       = _T("ACTIVITY:LogEnabledError");

    /////////////////////////
    // ALERT Options
    /////////////////////////
static const TCHAR ALERT_AlertMsg[]                = _T("ALERT:AlertMsg");
static const TCHAR ALERT_DispAlertMsg[]            = _T("ALERT:DispAlertMsg");
static const TCHAR ALERT_RemoveAfter[]             = _T("ALERT:RemoveAfter");
static const TCHAR ALERT_Seconds[]                 = _T("ALERT:Seconds");
static const TCHAR ALERT_Beep[]                    = _T("ALERT:Beep");
static const TCHAR ALERT_AlertNetUsr[]             = _T("ALERT:AlertNetUsr");
static const TCHAR ALERT_AlertNetCon[]             = _T("ALERT:AlertNetCon");
static const TCHAR ALERT_AlertNLM[]                = _T("ALERT:AlertNLM");
static const TCHAR ALERT_Known[]                   = _T("ALERT:Known");
static const TCHAR ALERT_Unknown[]                 = _T("ALERT:Unknown");
static const TCHAR ALERT_Inoculation[]             = _T("ALERT:Inoculation");
static const TCHAR ALERT_VirusLike[]               = _T("ALERT:VirusLike");
static const TCHAR ALERT_StartEnd[]                = _T("ALERT:StartEnd");
static const TCHAR ALERT_VirListChange[]           = _T("ALERT:VirListChange");
static const TCHAR ALERT_Quarantine[]              = _T("ALERT:Quarantine");

    /////////////////////////
    // SCANNER Options
    /////////////////////////
static const TCHAR SCANNER_Memory[]              = _T("SCANNER:Memory");
static const TCHAR SCANNER_MasterBootRec[]       = _T("SCANNER:MasterBootRec");
static const TCHAR SCANNER_BootRecs[]            = _T("SCANNER:BootRecs");
static const TCHAR SCANNER_ScanAll[]             = _T("SCANNER:ScanAll");
static const TCHAR SCANNER_ResponseMode[]        = _T("SCANNER:ResponseMode");
static const TCHAR SCANNER_ScanNetwork_Files[]	 = _T("SCANNER:ScanNetworkFiles");

/*************************************************
 * The followinf define's are for the uAction member
 *************************************************/
enum { SCAN_ACTION_PROMPT = 1
     , SCAN_ACTION_REPORT
     , SCAN_ACTION_REPAUTO
     , SCAN_ACTION_DELAUTO
     , SCAN_ACTION_HALTCPU
     , SCAN_ACTION_MOVE
     };

static const TCHAR SCANNER_Action[]              = _T("SCANNER:Action");
static const TCHAR SCANNER_DispRepair[]          = _T("SCANNER:DispRepair");
static const TCHAR SCANNER_DispDelete[]          = _T("SCANNER:DispDelete");
static const TCHAR SCANNER_DispCont[]            = _T("SCANNER:DispCont");
static const TCHAR SCANNER_DispExcl[]            = _T("SCANNER:DispExcl");
static const TCHAR SCANNER_DispQuarantine[]      = _T("SCANNER:DispQuarantine");
static const TCHAR SCANNER_AllowNetScan[]        = _T("SCANNER:AllowNetScan");
static const TCHAR SCANNER_AllowScanStop[]       = _T("SCANNER:AllowScanStop");
static const TCHAR SCANNER_PrestoNotify[]        = _T("SCANNER:PrestoNotify");
static const TCHAR SCANNER_PresFlop[]            = _T("SCANNER:PresFlop");
static const TCHAR SCANNER_PresHard[]            = _T("SCANNER:PresHard");
static const TCHAR SCANNER_PresNet[]             = _T("SCANNER:PresNet");
static const TCHAR SCANNER_EnableHeuristicScan[] = _T("SCANNER:EnableHeuristicScan");
static const TCHAR SCANNER_HeuristicLevel[]      = _T("SCANNER:HeuristicLevel");
static const TCHAR SCANNER_BackupRep[]           = _T("SCANNER:BackupRep");
static const TCHAR SCANNER_Ext[]                 = _T("SCANNER:Ext");
static const TCHAR SCANNER_BackUpThreat[]        = _T("SCANNER:BackUpThreat");
static const TCHAR SCANNER_ThreatCatEnabled[]    = _T("SCANNER:ThreatCatEnabled");
static const TCHAR SCANNER_ThreatCatResponse[]   = _T("SCANNER:ThreatCatResponse");
static const TCHAR SCANNER_DisableMultithreading[]   = _T("SCANNER:DisableMultithreading");
static const TCHAR SCANNER_ScanThreads[]         = _T("SCANNER:ScanThreads");
static const TCHAR SCANNER_EnableSideEffectScan[]   = _T("SCANNER:EnableSideEffectScan");
static const TCHAR SCANNER_DefUpdateScan[]       = _T("SCANNER:DefUpdateScan");
static const TCHAR SCANNER_DisableMemoryScan[]   = _T("SCANNER:DisableMemoryScan");
static const TCHAR SCANNER_TestDefs[]            = _T("SCANNER:TestDefs");
static const TCHAR SCANNER_ScanDlls[]            = _T("SCANNER:ScanDlls");
static const TCHAR SCANNER_ScanDllDepth[]        = _T("SCANNER:ScanDllDepth");
static const TCHAR SCANNER_ScanClientCompliancy[]   = _T("SCANNER:ScanClientCompliancy");
static const TCHAR SCANNER_AlertForExclude[]     = _T("SCANNER:AlertExclude");

    /////////////////////////
    // PASSWORD Options
    /////////////////////////
static const TCHAR PASSWORD_Protect[]       = _T("PASSWORD:Protect");
static const TCHAR PASSWORD_MaxProtection[] = _T("PASSWORD:MaxProtection");
static const TCHAR PASSWORD_Password[]      = _T("PASSWORD:Password");

    /////////////////////////
    // GENERAL Options
    /////////////////////////
static const TCHAR GENERAL_ScanZipFiles[]   = _T("GENERAL:ScanZipFiles");
static const TCHAR GENERAL_ScanHighMemory[] = _T("GENERAL:ScanHighMemory");
static const TCHAR GENERAL_BackExt[]        = _T("GENERAL:BackExt");
static const TCHAR GENERAL_Def[]            = _T("GENERAL:Def");

    /////////////////////////
    // QUARANTINE Options
    /////////////////////////
// These first 4 seem to be unused
static const TCHAR QUARANTINE_QuarantineAP[]   = _T("QUARANTINE:QuarantineAP");
static const TCHAR QUARANTINE_QuarantineScan[] = _T("QUARANTINE:QuarantineScan");
static const TCHAR QUARANTINE_QuarantineInoc[] = _T("QUARANTINE:QuarantineInoc");
static const TCHAR QUARANTINE_QuarantineDir[]  = _T("QUARANTINE:QuarantineDir");

static const TCHAR QUARANTINE_QuarantineSnapShot[] = _T("QUARANTINE:QuarantineSnapShot");

    /////////////////////////
    // Custom RESPONSE Options
    /////////////////////////
static const TCHAR RESPONSE_Selected[] = _T("RESPONSE:Selected");
static const TCHAR RESPONSE_Macro[]    = _T("RESPONSE:Macro");
static const TCHAR RESPONSE_File[]     = _T("RESPONSE:File");
static const TCHAR RESPONSE_Boot[]     = _T("RESPONSE:Boot");

    /////////////////////////
    // Custom NAVPROXY Options
    /////////////////////////
static const TCHAR NAVEMAIL_ResponseMode[]        = _T("NAVEMAIL:ResponseMode");
static const TCHAR NAVEMAIL_ScanIncoming[]        = _T("NAVEMAIL:ScanIncoming");
static const TCHAR NAVEMAIL_ScanOutgoing[]        = _T("NAVEMAIL:ScanOutgoing");
static const TCHAR NAVEMAIL_OEH[]                 = _T("NAVEMAIL:OEH");
static const TCHAR NAVEMAIL_OEHResponseMode[]     = _T("NAVEMAIL:OEHResponseMode");
static const TCHAR NAVEMAIL_ThreatCatEnabled[]    = _T("NAVEMAIL:ThreatCatEnabled");
static const TCHAR NAVEMAIL_ThreatCatResponse[]   = _T("NAVEMAIL:ThreatCatResponse");

// The following option names are still used internally by the
// NAV Options UI to make the options scriptable, however the
// actually options are read and written using the ccEmlPxy.dll
// exported functions. So you ask the NAVOptions COM object to
// get or set any of these three options and you will actually be
// getting or setting the values from ccEmlPxy.dll
static const TCHAR NAVPROXY_ShowTrayIcon[]        = _T("NAVPROXY:ShowTrayIcon");
static const TCHAR NAVPROXY_TimeOutProtection[]   = _T("NAVPROXY:TimeOutProtection");
static const TCHAR NAVPROXY_ShowProgressOut[]     = _T("NAVPROXY:ShowProgressOut");

	   /////////////////////////////////////
    // Custom Home Page Protection Options
    ///////////////////////////////////////

static const TCHAR HOMEPAGEPROTECTION_Enabled[] = _T("HPP:Enabled");
static const TCHAR HOMEPAGEPROTECTION_Respond[] = _T("HPP:Respond");
static const TCHAR HOMEPAGEPROTECTION_Control[] = _T("HPP:Control");
static const TCHAR HOMEPAGEPROTECTION_FirstRunDialog[] = _T("HPP:FirstRunDialog");



    /////////////////////////
    // Custom IMSCAN Options
    /////////////////////////
static const TCHAR IMSCAN_ScanMIM[]				= _T("IMSCAN:ScanMIM");
static const TCHAR IMSCAN_ScanAIM[]				= _T("IMSCAN:ScanAIM");
static const TCHAR IMSCAN_ScanYIM[]				= _T("IMSCAN:ScanYIM");
static const TCHAR IMSCAN_ScanTOL[]				= _T("IMSCAN:ScanTOL");
static const TCHAR IMSCAN_ResponseMode[]        = _T("IMSCAN:ResponseMode");
static const TCHAR IMSCAN_MessageSender[]       = _T("IMSCAN:MessageSender");
static const TCHAR IMSCAN_FirstRunFlag[]		= _T("IMSCAN:FirstRunFlag");
static const TCHAR IMSCAN_ThreatCatEnabled[]    = _T("IMSCAN:ThreatCatEnabled");
static const TCHAR IMSCAN_ThreatCatResponse[]   = _T("IMSCAN:ThreatCatResponse");

    /////////////////////////
    // Default Options value
    /////////////////////////
static const TCHAR DEFAULT_LiveUpdate[]             = _T("DEFAULT:LiveUpdate");
static const TCHAR DEFAULT_LiveUpdateMode[]         = _T("DEFAULT:LiveUpdateMode");
static const TCHAR DEFAULT_OfficePlugin[]           = _T("DEFAULT:OfficePlugin");
static const TCHAR DEFAULT_DefinitionsAlert[]       = _T("DEFAULT:DefinitionsAlert");
static const TCHAR DEFAULT_StartupScan[]            = _T("DEFAULT:StartupScan");

    /////////////////////////
    // DEFALERT Options
    /////////////////////////
static const TCHAR DEFALERT_EnableOldDefs[]			= _T("DEFALERT:EnableOldDefs");
static const TCHAR DEFALERT_MaxDefsAge[]			= _T("DEFALERT:MaxDefsAge");
static const TCHAR DEFALERT_StartSubWarning[]		= _T("DEFALERT:StartSubWarning");
static const TCHAR DEFALERT_CycleSubWarning[]		= _T("DEFALERT:CycleSubWarning");
static const TCHAR DEFALERT_CycleSubExpired[]		= _T("DEFALERT:CycleSubExpired");
static const TCHAR DEFALERT_CycleSubWarningAutoRenew[]		= _T("DEFALERT:CycleSubWarningAR");
static const TCHAR DEFALERT_CycleSubExpiredAutoRenew[]		= _T("DEFALERT:CycleSubExpiredAR");
static const TCHAR DEFALERT_CycleLicWarning[]		= _T("DEFALERT:CycleLicWarning");
static const TCHAR DEFALERT_CycleLicExpired[]		= _T("DEFALERT:CycleLicExpired");
static const TCHAR DEFALERT_FirstAlertDelay[]		= _T("DEFALERT:FirstAlertDelay");		// Number of seconds first licensing alert to be delayed
static const TCHAR DEFALERT_VirusDefsDelay[]		= _T("DEFALERT:VirusDefsDelay");		// Number of secons virus definitions alert to be delayed
static const TCHAR DEFALERT_EnableSubsAlert[]		= _T("DEFALERT:EnableSubsAlert");
static const TCHAR DEFALERT_EnableALUDisabledAlert[]		= _T("DEFALERT:EnableALUDisabledAlert");

    /////////////////////////
    // VIRUSDEFS Options
    /////////////////////////
static const TCHAR VIRUSDEFS_ThreatCount[]			= _T("VIRUSDEFS:ThreatCount");
static const TCHAR VIRUSDEFS_DownloadCount[]		= _T("VIRUSDEFS:DownloadCount");

	/////////////////////////
    // Common Error Handling Options
    /////////////////////////
// this option's value is passed to the tech support server allows things like "NAV2003 brought to you by T-ONLINE"
static const TCHAR CC_ERR_BUILD_TYPE[]				= _T("CCERR:Build_Type"); 	

    /////////////////////////
    // ALU Options
    /////////////////////////
static const TCHAR ALU_Product[]                    = _T("ALU:Product");
static const TCHAR ALU_Virusdefs[]                  = _T("ALU:Virusdefs");

    /////////////////////////
    // NAV Error forced error values
    // These are for QA to force errors to occur. They
    // are not normally present in the file.
	// NAVError reads these values if HKEY_LOCAL_MACHINE\SOFTWARE\Symantec\Norton AntiVirus\Forced Error == 1
    /////////////////////////
static const TCHAR ERROR_ForcedModuleID[]           = _T("ERROR:ForcedModuleID");
static const TCHAR ERROR_ForcedErrorID[]            = _T("ERROR:ForcedErrorID");

    /////////////////////////
    // Feature On/Off
    /////////////////////////
static const TCHAR NAVSHEXT_FeatureEnabled[]		= _T("NAVSHEXT:FeatureEnabled");
static const TCHAR OFFICEAV_FeatureEnabled[]		= _T("OFFICEAV:FeatureEnabled");
static const TCHAR NAVOPTION_FeatureEnabled[]		= _T("NAVOPTION:FeatureEnabled");
static const TCHAR IMSCANSTUB_FeatureEnabled[]		= _T("IMSCANSTUB:FeatureEnabled");
static const TCHAR IMSCAN_FeatureEnabledScanAIM[]	= _T("IMSCAN:FeatureEnabledScanAIM");
static const TCHAR IMSCAN_FeatureEnabledScanMIM[]	= _T("IMSCAN:FeatureEnabledScanMIM");
static const TCHAR IMSCAN_FeatureEnabledScanYIM[]	= _T("IMSCAN:FeatureEnabledScanYIM");
static const TCHAR IMSCAN_FeatureEnabledScanTOL[]	= _T("IMSCAN:FeatureEnabledScanTOL");
static const TCHAR NAVEMAIL_FeatureEnabledScanIn[]	= _T("NAVEMAIL:FeatureEnabledScanIn");
static const TCHAR NAVEMAIL_FeatureEnabledScanOut[]	= _T("NAVEMAIL:FeatureEnabledScanOut");
static const TCHAR NAVEMAIL_FeatureEnabledOEH[]		= _T("NAVEMAIL:FeatureEnabledOEH");
static const TCHAR ALU_FeatureEnabledProduct[]		= _T("ALU:FeatureEnabledProduct");
static const TCHAR ALU_FeatureEnabledVirusdefs[]	= _T("ALU:FeatureEnabledVirusdefs");
static const TCHAR SUBSCRIPTION_Activated[]			= _T("SUBSCRIPTION:Activated");
static const TCHAR HOMEPAGEPROTECTION_FeatureEnabled[] = _T("HPP:FeatureEnabled");
static const TCHAR NSC_FeatureEnabled[] =			_T("NSC:FeatureEnabled");
static const TCHAR SYMPROTECT_FeatureEnabled[]		= _T("SP:FeatureEnabled");

    /////////////////////////
    // Licensing Info
    /////////////////////////
static const TCHAR LICENSE_Licensed[]       = _T("USERL:Scroll"); /*hs 423509*/
static const TCHAR LICENSE_FeatureEnabled[]	= _T("DRM:FeatureEnabled");
static const TCHAR LICENSE_Aggregated[]     = _T("DRM:Aggregated");
static const TCHAR LICENSE_DownloadUpgradeURL[]		= _T("DRM:DownloadUpgradeURL");

// ******************
//
// Obsolete! DO NOT USE THESE FOR CURRENT OR NEW OPTIONS. These are only for detecting old installs, etc.
// Use DRM names defined in DRMNamedProperties.h
//
static const TCHAR LICENSE_Type[]           = _T("LICENSE:Type");
static const TCHAR LICENSE_State[]          = _T("LICENSE:State");
static const TCHAR LICENSE_Zone[]           = _T("LICENSE:Zone");
static const TCHAR LICENSE_SubType[]        = _T("LICENSE:SubType");
static const TCHAR LICENSE_VendorId[]       = _T("LICENSE:VendorId");
static const TCHAR LICENSE_DaysRemaining[]  = _T("LICENSE:DaysRemaining");
static const TCHAR LICENSE_KeyStored[]     	= _T("LICENSE:KeyStored");
static const TCHAR LICENSE_DieTime[]        = _T("LICENSE:DieTime");
static const TCHAR LICENSE_ActGuid[]        = _T("LICENSE:ActGuid");
static const TCHAR LICENSE_ActKey[]         = _T("LICENSE:ActKey");
static const TCHAR LICENSE_MaxDays[]        = _T("LICENSE:MaxDays");
static const TCHAR LICENSE_CancelCode[]		= _T("LICENSE:CancelCode");


static const TCHAR LICENSE_SUB_DaysMax[]        = _T("SUBSCRIPTION:DaysMax");
static const TCHAR LICENSE_SUB_DaysRemaining[]  = _T("SUBSCRIPTION:DaysRemaining");
static const TCHAR LICENSE_SUB_DaysWarning[]    = _T("SUBSCRIPTION:DaysWarning");
static const TCHAR LICENSE_SUB_Disposition[]    = _T("SUBSCRIPTION:Disposition");
static const TCHAR LICENSE_SUB_BeginDate[]      = _T("SUBSCRIPTION:BeginDate");
static const TCHAR LICENSE_SUB_EndDate[]        = _T("SUBSCRIPTION:EndDate");
static const TCHAR LICENSE_SUB_SKU[]            = _T("SUBSCRIPTION:SubSku");
static const TCHAR LICENSE_SUB_PNU[]            = _T("SUBSCRIPTION:PartNumber");
static const TCHAR LICENSE_MEDIA_SKU[]          = _T("SKU:OPSKU");
static const TCHAR LICENSE_FAMILY_SKU[]         = _T("SKU:SPEFSKU");
//
// end obsolete
// ******************

    /////////////////////////
    // Threat Categories
    /////////////////////////
static const TCHAR THREAT_LastKnownCategory[]   = _T("THREAT:LastKnownCategory");
static const TCHAR THREAT_FirstKnownNonViral[]  = _T("THREAT:FirstKnownNonViral");
static const TCHAR THREAT_NoThreatCat[]		    = _T("THREAT:NoThreatCat");
static const TCHAR THREAT_RealTime[]		    = _T("THREAT:RealTime");

    /////////////////////////
    // Compressed AP
    /////////////////////////
static const TCHAR APCOMPRESSED_BlockedAppUI[]          = _T("APCOMPRESSED:BlockedAppUI");
static const TCHAR APCOMPRESSED_BlockedDelay[]          = _T("APCOMPRESSED:BlockedDelay");
static const TCHAR APCOMPRESSED_MaxActiveBlocked[]      = _T("APCOMPRESSED:MaxActiveBlocked");
static const TCHAR APCOMPRESSED_WaitTimeMinDisplayBlocked[] = _T("APCOMPRESSED:WaitTimeMinDisplay");
static const TCHAR APCOMPRESSED_WaitTimeMaxDisplayBlocked[] = _T("APCOMPRESSED:WaitTimeMaxDisplay");
static const TCHAR APCOMPRESSED_DisplayAlert[]          = _T("APCOMPRESSED:DisplayAlert");
static const TCHAR APCOMPRESSED_ForcedAction[]          = _T("APCOMPRESSED:ForcedAction");

    /////////////////////////
    // Auto-protect
    /////////////////////////
static const TCHAR AUTOPROTECT_DelayLoad[]              = _T("AUTOPROTECT:DelayLoad");
static const TCHAR AUTOPROTECT_AutoScanSideEffects[]    = _T("AUTOPROTECT:AutoScanSideEffects");
static const TCHAR AUTOPROTECT_TestDefs[]               = _T("AUTOPROTECT:TestDefs");

    /////////////////////////
    // Main Help Menu
    /////////////////////////
static const TCHAR HELPMENU_ShowProductReg[]	    = _T("HELPMENU:ShowProductReg");
static const TCHAR HELPMENU_ShowSymWebsite[]	    = _T("HELPMENU:ShowSymWebsite");
static const TCHAR HELPMENU_ShowSupport[]		    = _T("HELPMENU:ShowSupport");
static const TCHAR HELPMENU_ShowResponse[]		    = _T("HELPMENU:ShowResponse");
static const TCHAR HELPMENU_ShowManageService[]		= _T("HELPMENU:ShowManageService");
static const TCHAR HELPMENU_ShowActivation[]	    = _T("HELPMENU:ShowActivation");
static const TCHAR HELPMENU_ShowRenewalCenter[]		= _T("HELPMENU:ShowRenewalCenter");
static const TCHAR HELPMENU_ShowClubSymantec[]		= _T("HELPMENU:ShowClubSymantec");


    ////////////////////////
    // Options options
    ///////////////////////
static const TCHAR NAVOPTION_HideAPIconOption[]		= _T("NAVOPTION:HideAPIconOption");

#endif // __OptNames_h__

