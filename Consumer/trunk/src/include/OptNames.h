////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// OptNames.h

#ifndef __OptNames_h__
#define __OptNames_h__

    /////////////////////////
    // ACTIVITY Options
    /////////////////////////
static const TCHAR ACTIVITY_LogSizeVirus[]          = _T("ACTIVITY:LogSizeVirus");
static const TCHAR ACTIVITY_LogSizeApp[]            = _T("ACTIVITY:LogSizeApp");
static const TCHAR ACTIVITY_LogSizeError[]          = _T("ACTIVITY:LogSizeError");
static const TCHAR ACTIVITY_LogEnabledVirus[]       = _T("ACTIVITY:LogEnabledVirus");
static const TCHAR ACTIVITY_LogEnabledApp[]         = _T("ACTIVITY:LogEnabledApp");
static const TCHAR ACTIVITY_LogEnabledError[]       = _T("ACTIVITY:LogEnabledError");

    /////////////////////////
    // Alert Options
    /////////////////////////
    enum {ALERT_MODE_AUTO = 0, ALERT_MODE_ASK = 1};
static const TCHAR ALERT_Mode[]        = _T("ALERT:Mode");

    /////////////////////////
    // Response Mode options
    /////////////////////////
    enum
    {
        RESPONSE_MODE_AUTO = 0,        // Auto remediate/fix/delete
        RESPONSE_MODE_IGNORE = 1,      // Ignore the threat - not to be confused with exclude
        RESPONSE_MODE_ASK = 2,         // Ask the user
    };

static const TCHAR NAVAP_ResponseMode[]			  = _T("NAVAP:ResponseMode");
static const TCHAR *AUTOPROTECT_ResponseMode       = NAVAP_ResponseMode; // Compatibility
static const TCHAR SCANNER_ResponseMode[]         = _T("SCANNER:ResponseMode");
static const TCHAR NAVEMAIL_ResponseMode[]        = _T("NAVEMAIL:ResponseMode");
static const TCHAR IMSCAN_ResponseMode[]          = _T("IMSCAN:ResponseMode");

    /////////////////////////
    // SCANNER Options
    /////////////////////////
static const TCHAR SCANNER_ScanTime[]				= _T("SCANNER:ScanTime");
static const TCHAR SCANNER_ScanDay[]				= _T("SCANNER:ScanDay");

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

static const TCHAR SCANNER_EnableHeuristicScan[] = _T("SCANNER:EnableHeuristicScan");
static const TCHAR SCANNER_HeuristicLevel[]      = _T("SCANNER:HeuristicLevel");
static const TCHAR SCANNER_ThreatCatResponse[]   = _T("SCANNER:ThreatCatResponse");
static const TCHAR SCANNER_ScanThreads[]         = _T("SCANNER:ScanThreads");               // Internal option
static const TCHAR SCANNER_EnableSideEffectScan[]   = _T("SCANNER:EnableSideEffectScan");
static const TCHAR SCANNER_EnableStealthScan[]   = _T("SCANNER:EnableStealthScan");
static const TCHAR SCANNER_DefUpdateScan[]       = _T("SCANNER:DefUpdateScan");             // Quick scan after updates
static const TCHAR SCANNER_TestDefs[]            = _T("SCANNER:TestDefs"); // Internal option
static const TCHAR SCANNER_ScanDlls[]            = _T("SCANNER:ScanDlls"); // Internal option
static const TCHAR SCANNER_ScanDllDepth[]        = _T("SCANNER:ScanDllDepth"); // Internal option
static const TCHAR SCANNER_ScanClientCompliancy[]   = _T("SCANNER:ScanClientCompliancy"); // Internal option
static const TCHAR SCANNER_AlertForExclude[]     = _T("SCANNER:AlertExclude"); // Internal option
static const TCHAR SCANNER_ConfidenceOnlineHeavyEngine[]     = _T("SCANNER:COHEngine"); // TODO: Internal option???
static const TCHAR SCANNER_ScanZipFiles[]   = _T("SCANNER:ScanZipFiles");   // Scan compressed files
static const TCHAR SCANNER_BackgroundScanWhenMin[] = _T("SCANNER:BackgroundScanWhenMin");   // perform a background scan when minimized
static const TCHAR SCANNER_ScanTrackingCookies[] = _T("SCANNER:ScanTrackingCookies");   // scan tracking cookies
static const TCHAR SCANNER_DecomposerMaxExtractSize[] = _T("SCANNER:DecomposerMaxExtractSize"); // maximum file size for decomposer to extract from an archive.
static const TCHAR SCANNER_StealthScan[]		= _T("SCANNER:StealthScan");	// Scan for rootkits, etc.
static const TCHAR SCANNER_NotifyOnCleanStealth[] = _T("SCANNER:NotifyOnCleanStealth"); // setting should match submission option

    /////////////////////////
    // PASSWORD Options
    /////////////////////////
static const TCHAR PASSWORD_Protect[]       = _T("PASSWORD:Protect");
static const TCHAR PASSWORD_MaxProtection[] = _T("PASSWORD:MaxProtection");
static const TCHAR PASSWORD_Password[]      = _T("PASSWORD:Password");

    /////////////////////////
    // GENERAL Scanner Options
    /////////////////////////
static const TCHAR *GENERAL_ScanZipFiles   = SCANNER_ScanZipFiles;   // Scan compressed files

    /////////////////////////
    // Custom NAVPROXY Options
    /////////////////////////
static const TCHAR NAVEMAIL_OEHResponseMode[]     = _T("NAVEMAIL:OEHResponseMode");
static const TCHAR NAVEMAIL_ThreatCatResponse[]   = _T("NAVEMAIL:ThreatCatResponse");

	// AV Component Based
static const TCHAR AVEMAIL_ScanIncoming[]			= _T("AVEMAIL:ScanIncoming");
static const TCHAR AVEMAIL_ScanOutgoing[]			= _T("AVEMAIL:ScanOutgoing");
static const TCHAR AVEMAIL_OEH[]					= _T("AVEMAIL:OEH");

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
static const TCHAR SYMPROTECT_FeatureEnabled[]		= _T("SP:FeatureEnabled");
static const TCHAR COH_FeatureEnabled[]				= _T("COH:FeatureEnabled");
static const TCHAR SUBMISSIONS_FeatureEnabled[]		= _T("SUBMISSIONS:FeatureEnabled"); //global enable/disable of automatic submissions

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
static const CHAR LICENSE_MEDIA_SKU[]          = "SKU:OPSKU";
static const CHAR LICENSE_FAMILY_SKU[]         = "SKU:SPEFSKU";
//
// end obsolete
// ******************

    /////////////////////////
    // Threat Categories
    /////////////////////////
static const TCHAR THREAT_LastKnownCategory[]   = _T("THREAT:LastKnownCategory");
static const TCHAR THREAT_FirstKnownNonViral[]  = _T("THREAT:FirstKnownNonViral");
static const TCHAR THREAT_NoThreatCat[]		    = _T("THREAT:NoThreatCat");
static const char THREAT_NoThreatCatA[]		    = "THREAT:NoThreatCat"; // Single byte version for navopts.dat
static const TCHAR THREAT_RealTime[]		    = _T("THREAT:RealTime");
static const TCHAR THREAT_Threat4[]				= _T("THREAT:Threat4");
static const TCHAR THREAT_Threat5[]				= _T("THREAT:Threat5");
static const TCHAR THREAT_Threat6[]				= _T("THREAT:Threat6");
static const TCHAR THREAT_Threat7[]				= _T("THREAT:Threat7");
static const TCHAR THREAT_Threat8[]				= _T("THREAT:Threat8");
static const TCHAR THREAT_Threat9[]				= _T("THREAT:Threat9");
static const TCHAR THREAT_Threat10[]			= _T("THREAT:Threat10");
static const TCHAR THREAT_Threat11[]			= _T("THREAT:Threat11");


    /////////////////////////
    // Auto-protect
    /////////////////////////
static const TCHAR AUTOPROTECT_EnableAP[]				= _T("AUTOPROTECT:EnableAP");
static const TCHAR AUTOPROTECT_CheckFloppyOnMount[]	= _T("AUTOPROTECT:ScanBR");
static const TCHAR AUTOPROTECT_EnableHeuristicScan[]	= _T("AUTOPROTECT:EnableHeuristicScan");
static const TCHAR AUTOPROTECT_AutoScanSideEffects[]    = _T("AUTOPROTECT:AutoScanSideEffects");
static const TCHAR AUTOPROTECT_TestDefs[]               = _T("AUTOPROTECT:TestDefs");
static const TCHAR AUTOPROTECT_DriversSystemStart[]		= _T("AUTOPROTECT:DriversSystemStart");

	/////////////////////////
	// NAV-based AP options
	/////////////////////////
static const TCHAR NAVAP_CheckFloppyOnBoot[]			= _T("NAVAP:CheckFloppyOnBoot");
static const TCHAR NAVAP_ShowIcon[]						= _T("NAVAP:ShowIcon");
static const TCHAR NAVAP_DelayLoadLock[]                = _T("NAVAP:DelayLoadLock");

    /////////////////////////
    // Instant Messenger
    /////////////////////////
static const TCHAR IMSCAN_ScanAIM[]	= _T("IMSCAN:ScanAIM");
static const TCHAR IMSCAN_ScanMIM[]	= _T("IMSCAN:ScanMIM");
static const TCHAR IMSCAN_YIM[]		= _T("IMSCAN:ScanYIM");

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
static const TCHAR HELPMENU_NoUpsellIcon[]			= _T("HELPMENU:NoUpsellIcon");


#endif // __OptNames_h__

