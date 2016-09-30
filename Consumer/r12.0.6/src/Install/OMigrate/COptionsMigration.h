/////////////////////////////////////////////////////////////////////////////////////////////
// COptionsMigration.h
//
// --Defines a class to be used for migration of options from one product to another during and
//   upgrade scenario.
//
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _NAVINST_DLL_COPTIONSMIGRATION__H
#define _NAVINST_DLL_COPTIONSMIGRATION__H

#include "NavOptions.h"
#include "SymScriptSafe.h"
#include "NavOpt32.h"
#include "OptNames.h"
#include "atlbase.h"
#include "Shellapi.h"
#include "CommonStructs.h"
#include <winreg.h>
#include <map>
#include <string>

#define NAV2002 8
#define NAV2003 9
#define NAV2004 10
#define NAV2005 11

//define the names used for ap options
const TCHAR INI_AUTOPROTECT_SECTION[]			=	_T("AutoProtect");
const TCHAR INI_AUTOPROTECT_ENABLED_ONBOOT[]	=	_T("EnableOnBoot");
const TCHAR INI_AUTOPROTECT_HIDE_ICON[]			=	_T("HideIcon");
const TCHAR INI_AUTOPROTECT_FILE_ACTION1[]		=	_T("FileAction1");
const TCHAR INI_AUTOPROTECT_FILE_ACTION2[]		=	_T("FileAction2");
const TCHAR INI_AUTOPROTECT_SCANALL[]			=	_T("ScanAll");
const TCHAR INI_AUTOPROTECT_ENABLE_HEURISTIC[]	=	_T("EnableBloodHound");
const TCHAR INI_AUTOPROTECT_HEURISTIC_LEVEL[]	=	_T("BloodHoundLevel");
const TCHAR INI_AUTOPROTECT_FLOOPY_MOUNT[]		=	_T("ScanFloppyMount");
const TCHAR INI_AUTOPROTECT_FLOOPY_BOOT[]		=	_T("ScanFloppyBoot");
const TCHAR INI_AUTOPROTECT_SCANEXT[]			=	_T("APScanExt");
const TCHAR INI_AUTOPROTECT_BACKUPREP[]			=	_T("APBackupRepair");
											  
//define the names used for manual scan options
const TCHAR INI_MANUALSCAN_SECTION[]			=	_T("ManualScan");
const TCHAR INI_MANUALSCAN_BOOTRECS[]			=	_T("ScanBootRecs");
const TCHAR INI_MANUALSCAN_MASTERBOOTRECS[]		=	_T("ScanMasterBootRecs");
const TCHAR INI_MANUALSCAN_RESPONSEMODE[]		=	_T("ResponseMode");
const TCHAR INI_MANUALSCAN_HEURISTIC_LEVEL[]	=	_T("HeuristicsLevel");
const TCHAR INI_MANUALSCAN_SCANZIPFILES[]		=	_T("ScanZipFiles");
const TCHAR INI_MANUALSCAN_SCANALL[]			=	_T("ScanAll");
const TCHAR INI_MANUALSCAN_SCANEXT[]			=	_T("ScanExt");
const TCHAR INI_MANUALSCAN_BACKUPREP[]			=	_T("BackupRepair");
const TCHAR INI_MANUALSCAN_MEMORY[]				=	_T("ScanMemory");
const TCHAR INI_MANUALSCAN_BACKUPTHREAT[]		=	_T("BackUpThreat");
const TCHAR INI_MANUALSCAN_THREATCATENABLED[]	=	_T("ThreatCatEnabled");
const TCHAR INI_MANUALSCAN_THREATCATRESPONSE[]	=	_T("ThreatCatResponse");

//define the names used for Home Page Protection
const TCHAR INI_HOMEPAGEPROTECTION_SECTION[] = _T("HomePageProtection");
const TCHAR INI_HOMEPAGEPROTECTION_ENABLED[] = _T("Enabled");
const TCHAR INI_HOMEPAGEPROTECTION_RESPOND[] = _T("Respond");
const TCHAR INI_HOMEPAGEPROTECTION_CONTROL[] = _T("Control");
const TCHAR INI_HOMEPAGEPROTECTION_FIRST_RUN_DIALOG[] = _T("FirstRunDialog");

//define the names used for the e-mail scanning options
const TCHAR INI_EMAIL_SECTION[]					=	_T("EmailScanning");
const TCHAR INI_EMAIL_SCANOUTGOING[]			=	_T("ScanOutGoing");
const TCHAR INI_EMAIL_SCANINCOMMING[]			=	_T("ScanIcomming");
const TCHAR INI_EMAIL_RESPONSEMODE[]			=	_T("ResponseMode");
const TCHAR INI_EMAIL_TIMEOUT_PROTECTION[]		=	_T("ProtectAgainstTimeouts");
const TCHAR INI_EMAIL_SHOW_TRAY_ICON[]			=   _T("ShowTrayIcon");
const TCHAR INI_EMAIL_SHOW_PROGRESS_OUT[]		=	_T("ShowProgressOut");
const TCHAR INI_EMAIL_THREATCATENABLED[]		=	_T("ThreatCatEnabled");
const TCHAR INI_EMAIL_THREATCATRESPONSE[]		=	_T("ThreatCatResponse");
const TCHAR INI_EMAIL_OEH[]						=	_T("EnableOEH");
const TCHAR INI_EMAIL_OEH_RESPONSE[]			=	_T("OEHResponse");

//define the names used for the IMScanner
const TCHAR INI_IMSCAN_SECTION[]				=	_T("IMScanning");
const TCHAR INI_IMSCAN_ENABLE_MSN[]				=	_T("EnableMSN");
const TCHAR INI_IMSCAN_ENABLE_AOL[]				=	_T("EnableAOL");
const TCHAR INI_IMSCAN_ENABLE_YIM[]				=	_T("EnableYIM");
const TCHAR INI_IMSCAN_RESPONSE[]				=	_T("Response");
const TCHAR INI_IMSCAN_ENABLE_MSN_REPLY[]		=	_T("EnableMSNReply");
const TCHAR INI_IMSCAN_THREATCATENABLED[]		=	_T("ThreatCatEnabled");
const TCHAR INI_IMSCAN_THREATCATRESPONSE[]		=	_T("ThreatCatResponse");

//define the names used for the LiveUpdate settings
const TCHAR INI_LU_SECTION[]					=	_T("LiveUpdate");
const TCHAR INI_LU_ENABLEALU[]					=	_T("EnableALU");
const TCHAR INI_LU_ALUMODE[]					=	_T("ALUMode");

//define the names used for logging options
const TCHAR INI_LOGGING_SECTION[]				=	_T("Logging");
const TCHAR INI_LOGGING_KEEPUPTO[]				=	_T("KeepUpTo");
const TCHAR INI_LOGGING_STARTEND[]				=	_T("StartEnd");
const TCHAR INI_LOGGING_EMAIL[]					=	_T("Email");
const TCHAR INI_LOGGING_KNOWN[]					=	_T("Known");
const TCHAR INI_LOGGING_KILOBYES[]				=	_T("MaxKiloBytes");

//define the names used for miscellaneous options
const TCHAR INI_MISC_SECTION[]					=	_T("Miscellaneous");
const TCHAR INI_MISC_ALERTDEFS[]				=	_T("AlertDefs");
const TCHAR INI_MISC_OFFICEPLUGIN[]				=	_T("OfficePlugin");
const TCHAR INI_MISC_STARTUPSCAN[]				=	_T("StartupScan");

//define the names used for AdvTools options
const TCHAR SYM_INSTALLED_APPS_KEY[]			=	_T("Software\\Symantec\\InstalledApps");
const TCHAR NU_INSTALLED_APPS_NAME[]			=	_T("Norton Utilities");
const TCHAR ADVTOOLS_KEY[]						=	_T("Software\\Symantec\\Norton Utilities\\Norton Protection");
const TCHAR ADVTOOLS_BACKUP_KEY[]				=	_T("Software\\Symantec\\Norton Protection");
const TCHAR RECYCLE_BIN_KEY[]					=	_T("Software\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}");
const TCHAR RECYCLE_BIN_BACKUP_KEY[]			=	_T("Software\\Symantec\\{645FF040-5081-101B-9F08-00AA002F954E}");

//define the names used for the TCAT options
const TCHAR INI_THREAT_SECTION[]				=	_T("ThreatCat");
const TCHAR INI_THREAT_ENTRYNAME[]				=	_T("Threat");

//define the names used for the IWP options
const TCHAR INI_IWP_SECTION[]					=	_T("IWP");
const TCHAR INI_IWP_USERWANTSON[]				=	_T("UserWantsOn");

//COptionsMigration class definition
class COptionsMigration
{
public:
	//construction/destruction
	COptionsMigration();
	~COptionsMigration();
	
	//exposed functions for migration process.
	bool Initialize();
	bool GetOld();
	bool SetNew();
	bool Migrate();

private:

	//these functions are organized by page in the options UI
	bool GetOldAP();
	bool GetOldManual();
	bool GetOldHomePageProtection();
	bool GetOldEmail();
	bool GetOldIMScan();
	bool GetOldLU();
	bool GetOldLog();
	bool GetOldMisc();
	bool GetOldTCAT();
	bool GetOldAdvTools();
	bool GetOldIWP();
	bool SetNewAP();
	bool SetNewManual();
	bool SetNewHomePageProtection();
	bool SetNewEmail();
	bool SetNewIMScan();
	bool SetNewLU();
	bool SetNewLog();
	bool SetNewMisc();
	bool SetNewAdvTools();
	bool SetNewTCAT();
	bool SetNewIWP();

	//utility functions
	bool GetNAVVersion();
	bool OpenCfgWizDAT();
	bool GetOptionsObject();
	bool OpenIniFile();
	bool COptionsMigration::RegCopyKey(HKEY hSourceKeyParent, LPCTSTR szSourceKey, HKEY hDestKeyParent, LPCTSTR szDestKey, BOOL bDeleteSource);
	bool IsNavPro();

	//priave data members
	INAVOptions* m_pOptionsObject;
	TCHAR m_szINIPath[MAX_PATH];
	HANDLE m_hINIFile;
	InstallToolBox::ITB_VERSION m_itbVersion;
	bool m_bReadingOld;
	bool m_bWritingNew;
	bool m_bAllOK;
	TCHAR m_szCfgWizDatPath[MAX_PATH];
	HNAVOPTS32 m_hCfgWizDat;
};

#endif //_NAVINST_DLL_COPTIONSMIGRATION__H