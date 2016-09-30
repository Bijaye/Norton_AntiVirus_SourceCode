/////////////////////////////////////////////////////////////////////////////////////////////
// COptionsMigration.h
//
// --implements the COptionsMigration Class
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NavOptions_i.c"
#include "SymScriptSafe_i.c"
#include "COptionsMigration.h"
#include "OptNames.h"
#include "APOptNames.h"
#include "actions.h"
#include "InstOptsNames.h"
#include "NAVDetection.h"
#include <map>

using namespace std;


//
// Construction/Destruction
//
COptionsMigration::COptionsMigration():
	m_pOptionsObject(NULL),
	m_bAllOK(false),
	m_bReadingOld(false),
	m_bWritingNew(false),
	m_hINIFile(INVALID_HANDLE_VALUE),
	m_hCfgWizDat(NULL)
{

}

COptionsMigration::~COptionsMigration()
{	
	if(m_hINIFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hINIFile);

	if(m_pOptionsObject != NULL)
	{
		m_pOptionsObject->Release();
		
		//make sure we let go of the options dll
		CoFreeUnusedLibraries();
	}

	
	if(m_hCfgWizDat != NULL)
	{
		NavOpts32_Save(m_szCfgWizDatPath, m_hCfgWizDat);
		NavOpts32_Free(m_hCfgWizDat);
	}

	CoUninitialize();
}


bool COptionsMigration::Initialize()
{
	//
	// We need to instaniate an Options object. This object might be the one from the options that
	// is already on the system, or the one from the newly installed product, depending on which phase
	// of the migration you are on.  The basic flow is this:
	//		1.  Create an object of this Class COptionsMigration, that will get Options from currently 
	//			installed NAV through its options object and save them.  Destroy the this object.
	//		2.  Uninstall the current NAV
	//		3.  Install the new NAV
	//		4.  Create a second object of this class, that will get the saved items from step one 
	//			and set then through the new NAV's options object.  Destroy this object.
	//

	try
	{
		//you can't initialize the same object twice!
		if(m_bAllOK)
			throw runtime_error("Class Already initialized.");

		if(!GetNAVVersion())
			throw runtime_error("Couldn't find NAV Version.");
		
		if(!OpenCfgWizDAT())
			throw runtime_error("Couldn't open CfgWiz.dat");
		
		if(!GetOptionsObject())
			throw runtime_error("Couldn't instantiate object.");
	
		if(!OpenIniFile())
			throw runtime_error("Couldn't open Ini file.");

		m_bAllOK = true;
	}
	catch(exception &ex)
	{
		g_Log.Log("COPtionsMigration::Initialize() -- %s", ex.what());
		m_bAllOK = false;
	}
	catch(...)
	{
		//something bad happened...
		g_Log.LogEx("Unknown Exception in COptionsMigration::Initialize()");
		m_bAllOK = false;
	}
	
	return m_bAllOK;
}


//
//This is the "smart" migration function that will either call GetOld()
//or SetNew() depending on the values of m_bReadingOld and m_bWritingNew
//that were set in the Initialize() function.  This way someone can 
//instantiate this class, initialize it, and call Migrate(), then come back
//and do it again and it will automatically pick up the settings retreived from
//the first time and set them in the new product.
//
bool COptionsMigration::Migrate()
{
	try
	{
		//if we weren't initialized properly we can't migrate anything, bail
		if(!m_bAllOK)
			throw runtime_error("Class not initialized correctly");

		//now, depending on the phase we are in we need to call GetOld() or SetNew(),
		//if we are in neither mode, its probably beacuse somebody called migrate twice,
		//there's point in that since you haven't swapped out the options object yet..
		if(m_bReadingOld)
			return GetOld();
		else if(m_bWritingNew)
			return SetNew();
		else
			return false;
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::Migrate() -- %s", ex.what());
		return false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknow Exception in COptionsMigration::Migrate()");
		return false;
	}
}


//
//Get all the options from the currently installed version and save
//them to an ini file.  We will try to get any option we've ever known
//about in any product.  If the Options object tells it doesn't exist
//then we won't save it to ini file and we won't attempt to set later
//
bool COptionsMigration::GetOld()
{
	bool bRet_val = true;
	
	try
	{
		//if the ini file was found then we should writing the new options, not reading them in again.  bail out.
		if(!m_bReadingOld || !m_bAllOK)
			throw runtime_error("Class is not in correct phase.");
/*	
		//get the AP options and put them in the in the ini file
		GetOldAP();
*/
		//get the Manaual Scan options and put them in the ini file
		GetOldManual();

		//get the Home Page Protection options and put them in the ini file
		GetOldHomePageProtection();

		//get the E-mail scanning options and put them in the ini file
		GetOldEmail();

		//get the IM scanning options and put them in the ini file
		GetOldIMScan();
/*
		//get the LU settings and put them in the ini file
		GetOldLU();
*/
		//get the logging options and put them in the ini file
		GetOldLog();

		//get the miscellaneous options and put them in the ini file
		GetOldMisc();

		//get the threat cat options and put them in the ini file
		GetOldTCAT();

		//get the IWP options and put them in the ini file
		GetOldIWP();
/*
		//get the advtools optins from the registry and back them up
		GetOldAdvTools();
*/
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOld() -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknow Exception in COptionsMigration::GetOld()");
		bRet_val = false;
	}

	//we're done reading the old options. you're not allowed to do it again!
	m_bReadingOld = false;

	return bRet_val;
}

//
//Set the options from the ini file to the newly installed product's options
//We only attempt to set options that we found originally.  If the option is
//not in the new product anymore (why would this be??), its ok we just continue
//
bool COptionsMigration::SetNew()
{
	bool bRet_val = true;
	
	try
	{
		//if the ini file was not found during initialization then there is nothing to set, bail out
		if(!m_bWritingNew || !m_bAllOK)
			throw runtime_error("Class is not in correct phase.");
/*		
		//retrieve the ap options from the ini file and set them in the new product
		SetNewAP();
*/
		//retrieve the Manaual scan options from the ini file and set them in the new product
		SetNewManual();

		//retrieve the Home Page Protection options from the ini file and set them in the new product
		SetNewHomePageProtection();

		//retrieve the e-mail scanning options from the ini file and set them in the new product
		SetNewEmail();

		//retrieve the IM Scanning options from the ini file and set them in the new product
		SetNewIMScan();
/*
		//retrieve the LU scanning options from the ini file and set them in the new product
		SetNewLU();
*/
		//retrieve the logging options from the ini file and set them in the new product
		SetNewLog();

		//retrieve the miscellaneous options from the ini file and set them in the new product
		SetNewMisc();
		
		//retrieve the tcat options from the ini file and set them in the new product
		SetNewTCAT();

		//retrieve the IWP options from the ini file and set them in the new product
		SetNewIWP();
/*
		//restore the advtools options from the registry
		SetNewAdvTools();
*/
        // set the business rules flag to false here so any save
        // calls on this object do not use navoptrf and cause a joyous
        // 8 minute hang in the install
        m_pOptionsObject->SetUseBusinessRules(FALSE);

        //save the options we just wrote out.
        m_pOptionsObject->Save();
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNew() -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNew().");
		bRet_val = false;
	}
	
	//delete the ini file. this can't be done in a the destructor b/c we might be in the Reading old mode and we need
	//to save the ini file for next time we get called to write them to the new products options.
	DeleteFile(m_szINIPath);

	//we done writing the new options now.  you're not allowed to do it agian (we've deleted the ini file)!
	m_bWritingNew = false;

	return bRet_val;
}

//
//Get all the AP options and write them to an ini file
//
bool COptionsMigration::GetOldAP()
{
	bool bRet_val = true;
	TCHAR szValue[128] = {0};
	int iValue = 0;
	CComVariant vValue;
	
	USES_CONVERSION;

	try
	{
		//create and Autoprotect section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_AUTOPROTECT_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
	
		//check the Enable AP Icon setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(TSR_HideIcon), CComVariant(0), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_HIDE_ICON, szValue, m_szINIPath);
		}

		//check the Response setting
        
        // only do this for 11.5 and higher
        if((11 < m_itbVersion.nMajorHi) || (11 == m_itbVersion.nMajorHi && 5 <= m_itbVersion.nMajorLo))
        {
            // First check for the legacy TSR:ActionKnown setting
            int iFileAction1 = MAINACTION_REPAIR;
            int iFileAction2 = MAINACTION_PROMPT;
            if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR("TSR:ActionKnown"), CComVariant(66), &vValue))
                && vValue.iVal != 66 )
		    {
                // We did get this value so it needs to be converted to the proper AP:FileAction... values
                OSVERSIONINFO osvi;
                osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
                GetVersionEx (&osvi);

                // 1 is the old value for prompt, this should be 9x only
			    if( VER_PLATFORM_WIN32_WINDOWS == osvi.dwPlatformId && 1 == vValue.iVal )
                {
                    // Set both file actions to prompt
                    iFileAction1 = iFileAction2 = MAINACTION_PROMPT;
                }
                // 2 is the old value for deny access
                else if( 2 == vValue.iVal )
                {
                    iFileAction1 = iFileAction2 = MAINACTION_DENYACCESS_NOTIFY;
                }
                // 10 is the old value for Quarantine then Repair
                else if( 10 == vValue.iVal )
                {
                    iFileAction1 = MAINACTION_QUARANTINE;
                    iFileAction2 = MAINACTION_REPAIR;
                }
                // The only thing left is repair
                else
                {
                    if( VER_PLATFORM_WIN32_WINDOWS == osvi.dwPlatformId ) //9x
                    {
                        // On 9x repair is repair then prompt
                        iFileAction1 = MAINACTION_REPAIR;
                        iFileAction2 = MAINACTION_PROMPT;
                    }
                    else // NT
                    {
                        // On NT repair is repair then deny access and notify
                        iFileAction1 = MAINACTION_REPAIR;
                        iFileAction2 = MAINACTION_DENYACCESS_NOTIFY;
                    }
                }
		    }
            // The legacy TSR:ActionKnown value did not exist so get the fileaction values
            else
            {
                m_pOptionsObject->Get(CComBSTR(AP_FileAction1), CComVariant(6), &vValue);
                iFileAction1 = vValue.iVal;
                m_pOptionsObject->Get(CComBSTR(AP_FileAction2), CComVariant(1), &vValue);
                iFileAction2 = vValue.iVal;
            }

            // Write the action values to the ini file
            _itot(iFileAction1, szValue, 10);
            WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_FILE_ACTION1, szValue, m_szINIPath);
            _itot(iFileAction2, szValue, 10);
            WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_FILE_ACTION2, szValue, m_szINIPath);
        }

		//check the Scan All setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(TSR_ScanAll), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_SCANALL, szValue, m_szINIPath);
		}

		//check the Enable Bloodhound setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(TSR_EnableHeuristicScan), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_ENABLE_HEURISTIC, szValue, m_szINIPath);
		}

		//check the BloodHound level setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(TSR_HeuristicLevel), CComVariant(2), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_HEURISTIC_LEVEL, szValue, m_szINIPath);
		}

		//check the Check Floppy Mount setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(TSR_ChkFlopAccess), CComVariant(2), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_FLOOPY_MOUNT, szValue, m_szINIPath);
		}

		//check the Check Floppy on reboot setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(TSR_ChkFlopOnBoot), CComVariant(2), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_FLOOPY_BOOT, szValue, m_szINIPath);
		}

		//check the AP extension list setting
        CComBSTR bstrAPExt;
		if(SUCCEEDED (m_pOptionsObject->get_APExtensionList(&bstrAPExt)) )
		{
			WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_SCANEXT, OLE2T(bstrAPExt), m_szINIPath);
		}

		//check the BackRep setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(AP_BackupRep), CComVariant(2), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_BACKUPREP, szValue, m_szINIPath);
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldAP -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldAP -- %s");
		bRet_val = false;
	}

	return bRet_val;
}

//
//Retrive all AP options from the ini file and set them in the new product
//
bool COptionsMigration::SetNewAP()
{
	bool bRet_val = true;
	TCHAR szValue[4096] = {0};
	int iValue = 0;
	
	try
	{	
		//set the Hide Icon setting
		GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_HIDE_ICON, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(TSR_HideIcon), CComVariant(iValue));
		}
	
		//set the Response settings
		GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_FILE_ACTION1, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(AP_FileAction1), CComVariant(iValue));
            m_pOptionsObject->Put(CComBSTR(AP_MacroAction1), CComVariant(iValue));
		}
        GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_FILE_ACTION2, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(AP_FileAction2), CComVariant(iValue));
            m_pOptionsObject->Put(CComBSTR(AP_MacroAction2), CComVariant(iValue));
		}
		
		//set the Scan All setting
		GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_SCANALL, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(TSR_ScanAll), CComVariant(iValue));
		}

		//set the Enable Bloodhound setting
		GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_ENABLE_HEURISTIC, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(TSR_EnableHeuristicScan), CComVariant(iValue));
		}

		//set the Bloodhound Level setting
		GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_HEURISTIC_LEVEL, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(TSR_HeuristicLevel), CComVariant(iValue));
		}

		//set the Floppy Mount setting
		GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_FLOOPY_MOUNT, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(TSR_ChkFlopAccess), CComVariant(iValue));
		}

		//set the Floppy Boot setting
		GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_FLOOPY_BOOT, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(TSR_ChkFlopOnBoot), CComVariant(iValue));
		}

		//set the AP extension list setting
		GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_SCANEXT, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			m_pOptionsObject->put_APExtensionList(CComBSTR(szValue));
		}

		//set the BackupRep setting
		GetPrivateProfileString(INI_AUTOPROTECT_SECTION, INI_AUTOPROTECT_BACKUPREP, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(AP_BackupRep), CComVariant(iValue));
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewAP -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewAP");
		bRet_val = false;
	}

	return bRet_val;
}

//
//Get all of the Manual Scan options and save them to the ini file
//
bool COptionsMigration::GetOldManual()
{
	bool bRet_val = true;
	int iValue = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;
	
	USES_CONVERSION;

	try
	{
		//create a ManualScan section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_MANUALSCAN_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		//check the Boot Record scan setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_BootRecs), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_BOOTRECS, szValue, m_szINIPath);
		}
		
		//check the Master boot record scan setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_MasterBootRec), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_MASTERBOOTRECS, szValue, m_szINIPath);
		}
		
		//check the response mode setting
		// only do this for 11.5 and higher
        if((11 < m_itbVersion.nMajorHi) || (11 == m_itbVersion.nMajorHi && 5 <= m_itbVersion.nMajorLo))
        {
            if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_ResponseMode), CComVariant(1), &vValue)))
		    {
			    iValue = vValue.iVal;
			    _itot(iValue, szValue, 10);
			    WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_RESPONSEMODE, szValue, m_szINIPath);
		    }
        }

		//check the heuristic level setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_HeuristicLevel), CComVariant(2), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_HEURISTIC_LEVEL, szValue, m_szINIPath);
		}
	
		//check the ScanZipFiles setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(GENERAL_ScanZipFiles), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_SCANZIPFILES, szValue, m_szINIPath);
		}

		//check the ScanAll setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_ScanAll), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_SCANALL, szValue, m_szINIPath);
		}
		
		//check the ScanExtensions setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_Ext), CComVariant(""), &vValue)))
		{
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_SCANEXT, OLE2T(vValue.bstrVal), m_szINIPath);
		}

		//check the BackupRep setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_BackupRep), CComVariant(2), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_BACKUPREP, szValue, m_szINIPath);
		}
		
		//check the Scan Memory setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_Memory), CComVariant(2), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_MEMORY, szValue, m_szINIPath);
		}
		
		//check the backup threat option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_BackUpThreat), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_BACKUPTHREAT, szValue, m_szINIPath);
		}
		
		//check the threat cat enabled option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_ThreatCatEnabled), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_THREATCATENABLED, szValue, m_szINIPath);
		}

		//check the threat can response option
        // only do this for 11.5 and higher
        if((11 < m_itbVersion.nMajorHi) || (11 == m_itbVersion.nMajorHi && 5 <= m_itbVersion.nMajorLo))
        {
		    if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(SCANNER_ThreatCatResponse), CComVariant(1), &vValue)))
		    {
			    iValue = vValue.iVal;
			    _itot(iValue, szValue, 10);
			    WritePrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_THREATCATRESPONSE, szValue, m_szINIPath);
		    }
        }

	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldManual -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldManual");
		bRet_val = false;
	}

	return bRet_val;
}


//
//Retrive all Manual Scan options from the ini file and set them in the new product
//
bool COptionsMigration::SetNewManual()
{
	bool bRet_val = true;
	TCHAR szValue[4096] = {0};
	int iValue = 0;
	
	try
	{	
		//set the scan bootrecord setting
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_BOOTRECS, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_BootRecs), CComVariant(iValue));
		}
		
		//set the scan master bootrecord setting
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_MASTERBOOTRECS, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_MasterBootRec), CComVariant(iValue));
		}

		//set the response mode setting
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_RESPONSEMODE, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_ResponseMode), CComVariant(iValue));
		}
		
		//set the bloodhound level setting
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_HEURISTIC_LEVEL, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_HeuristicLevel), CComVariant(iValue));
		}
		
		//set the scan zip files setting
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_SCANZIPFILES, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(GENERAL_ScanZipFiles), CComVariant(iValue));
		}

		//set the scan all setting
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_SCANALL, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_ScanAll), CComVariant(iValue));
		}
		
		//set the scanextensions setting
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_SCANEXT, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			m_pOptionsObject->Put(CComBSTR(SCANNER_Ext), CComVariant(szValue));
		}

		//set the BackupRep setting
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_BACKUPREP, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_BackupRep), CComVariant(iValue));
		}
		
		//set the Scan Memory setting
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_MEMORY, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_Memory), CComVariant(iValue));
		}

		//set the backup threat
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_BACKUPTHREAT, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_BackUpThreat), CComVariant(iValue));
		}

		//set the threatcat enabled
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_THREATCATENABLED, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_ThreatCatEnabled), CComVariant(iValue));
		}
		
		//set the threat cat response
		GetPrivateProfileString(INI_MANUALSCAN_SECTION, INI_MANUALSCAN_THREATCATRESPONSE, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(SCANNER_ThreatCatResponse), CComVariant(iValue));
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewManual -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewManual");
		bRet_val = false;
	}

	return bRet_val;
}




//
//Get all of the Home Page Protection options and save them to the ini file
//
bool COptionsMigration::GetOldHomePageProtection()
{
	bool bRet_val = true;
	int iValue = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;
	
	USES_CONVERSION;

	try
	{
		//create a Home Page Protection section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_HOMEPAGEPROTECTION_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		//check the Home Page Protection Enabled setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(HOMEPAGEPROTECTION_Enabled), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_HOMEPAGEPROTECTION_SECTION, INI_HOMEPAGEPROTECTION_ENABLED, szValue, m_szINIPath);
		}
		
		//check the Home Page Protection Respond setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(HOMEPAGEPROTECTION_Respond), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_HOMEPAGEPROTECTION_SECTION, INI_HOMEPAGEPROTECTION_RESPOND, szValue, m_szINIPath);
		}
		
		//check the Home Page Protection Control setting
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(HOMEPAGEPROTECTION_Control), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_HOMEPAGEPROTECTION_SECTION, INI_HOMEPAGEPROTECTION_CONTROL, szValue, m_szINIPath);
		}

        //check the Home Page Protection FirstRunDialog setting
        if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(HOMEPAGEPROTECTION_FirstRunDialog), CComVariant(1), &vValue)))
        {
            iValue = vValue.iVal;
            _itot(iValue, szValue, 10);
            WritePrivateProfileString(INI_HOMEPAGEPROTECTION_SECTION, INI_HOMEPAGEPROTECTION_FIRST_RUN_DIALOG, szValue, m_szINIPath);
        }

	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldHomePageProtection -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldHomePageProtection");
		bRet_val = false;
	}

	return bRet_val;
}

//
//Retrive all Home Page Protection options from the ini file and set them in the new product
//
bool COptionsMigration::SetNewHomePageProtection()
{
	bool bRet_val = true;
	TCHAR szValue[4096] = {0};
	int iValue = 0;
	
	try
	{	
		//set the Home Page Protection enabled setting
		GetPrivateProfileString(INI_HOMEPAGEPROTECTION_SECTION, INI_HOMEPAGEPROTECTION_ENABLED, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(HOMEPAGEPROTECTION_Enabled), CComVariant(iValue));
		}
		
		//set the Home Page Protection Respond setting
		GetPrivateProfileString(INI_HOMEPAGEPROTECTION_SECTION, INI_HOMEPAGEPROTECTION_RESPOND, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(HOMEPAGEPROTECTION_Respond), CComVariant(iValue));
		}

		//set the Home Page Protection Control setting
		GetPrivateProfileString(INI_HOMEPAGEPROTECTION_SECTION, INI_HOMEPAGEPROTECTION_CONTROL, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(HOMEPAGEPROTECTION_Control), CComVariant(iValue));
		}
		
		//set the Home Page Protection FirstRunDialog setting
		GetPrivateProfileString(INI_HOMEPAGEPROTECTION_SECTION, INI_HOMEPAGEPROTECTION_FIRST_RUN_DIALOG, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(HOMEPAGEPROTECTION_FirstRunDialog), CComVariant(iValue));
		}

	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewHomePageProtection -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewPageProtection");
		bRet_val = false;
	}

	return bRet_val;
}


//
//Get the E-mail scanning options and set them in the ini file
//
bool COptionsMigration::GetOldEmail()
{
	//NOTE: Abu made changes to these options.  In NAV8 the e-mail options we called 
	//NAVPROXY_XXXX in the optnames.h.  Now they are called NAVEMAIL_XXXX which basically
	//breaks any options migration for e-mail scanning from nav8 to nav9..need to address this.
	
	bool bRet_val = true;
	int iValue = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;

	try
	{
		//create and Manual Scan section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_EMAIL_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		//check the Scan Out Going option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVEMAIL_ScanOutgoing), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_SCANOUTGOING, szValue, m_szINIPath);
		}

		//check the Scan Incomming option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVEMAIL_ScanIncoming), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_SCANINCOMMING, szValue, m_szINIPath);
		}

		//check the Response mode option
        // only do this for 11.5 and higher
        if((11 < m_itbVersion.nMajorHi) || (11 == m_itbVersion.nMajorHi && 5 <= m_itbVersion.nMajorLo))
        {
		    if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVEMAIL_ResponseMode), CComVariant(1), &vValue)))
		    {
			    iValue = vValue.iVal;
			    _itot(iValue, szValue, 10);
			    WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_RESPONSEMODE, szValue, m_szINIPath);
		    }
        }

		//
		//although the following options changed in their persistence method (which DAT file), the header 
		//names did not change and options object takes care of resolving this for me, so I continue to
		//use the same header names
		//

		//check the Protect Against Timeouts option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVPROXY_TimeOutProtection), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_TIMEOUT_PROTECTION, szValue, m_szINIPath);
		}
		
		//check the Show Tray Icon option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVPROXY_ShowTrayIcon), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_SHOW_TRAY_ICON, szValue, m_szINIPath);
		}
		
		//check the ShowProgress Out option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVPROXY_ShowProgressOut), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_SHOW_PROGRESS_OUT, szValue, m_szINIPath);
		}

		//check the threat cat enabled
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVEMAIL_ThreatCatEnabled), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_THREATCATENABLED, szValue, m_szINIPath);
		}
		
		//check the threat cat response
		// only do this for 11.5 and higher
        if((11 < m_itbVersion.nMajorHi) || (11 == m_itbVersion.nMajorHi && 5 <= m_itbVersion.nMajorLo))
        {
            if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVEMAIL_ThreatCatResponse), CComVariant(1), &vValue)))
		    {
			    iValue = vValue.iVal;
			    _itot(iValue, szValue, 10);
			    WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_THREATCATRESPONSE, szValue, m_szINIPath);
		    }
        }

		//check the enable OEH scanning optin
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVEMAIL_OEH), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_OEH, szValue, m_szINIPath);
		}

		//check the OEH ResponseMode options
		// only do this for 11.5 and higher
        if((11 < m_itbVersion.nMajorHi) || (11 == m_itbVersion.nMajorHi && 5 <= m_itbVersion.nMajorLo))
        {
            if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(NAVEMAIL_OEHResponseMode), CComVariant(1), &vValue)))
		    {
			    iValue = vValue.iVal;
			    _itot(iValue, szValue, 10);
			    WritePrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_OEH_RESPONSE, szValue, m_szINIPath);
		    }
        }
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldEmail -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldEmail");
		bRet_val = false;
	}
	return bRet_val;
}

//
//Retrive all E-mail Scanning options from the ini file and set them in the new product
//
bool COptionsMigration::SetNewEmail()
{
	bool bRet_val = true;
	TCHAR szValue[4096] = {0};
	int iValue = 0;
	
	try
	{	
		//set the scan SMTP setting
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_SCANOUTGOING, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			
			//set the options setting
			m_pOptionsObject->Put(CComBSTR(NAVEMAIL_ScanOutgoing), CComVariant(iValue));

			//set the cfgwiz.dat setting
			NavOpts32_SetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_EnableEmailScannerOut, iValue);
		}
		
		//set the scan POP setting
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_SCANINCOMMING, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			
			//set the options setting
			m_pOptionsObject->Put(CComBSTR(NAVEMAIL_ScanIncoming), CComVariant(iValue));

			//set the cfgwiz.dat setting
			NavOpts32_SetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_EnableEmailScannerIn, iValue);
		}

		//set the scan response mode setting
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_RESPONSEMODE, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(NAVEMAIL_ResponseMode), CComVariant(iValue));
		}
		
		//
		//although the following options changed in their persistence method (which DAT file), the header 
		//names did not change and options object takes care of resolving this for me, so I continue to
		//use the same header names accros all versions of nav
		//

		//set the timeout protection setting
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_TIMEOUT_PROTECTION, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(NAVPROXY_TimeOutProtection), CComVariant(iValue));
		}

		//set the timeout protection setting
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_SHOW_TRAY_ICON, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(NAVPROXY_ShowTrayIcon), CComVariant(iValue));
		}

		//set the timeout protection setting
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_SHOW_PROGRESS_OUT, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(NAVPROXY_ShowProgressOut), CComVariant(iValue));
		}

		//set the threat cat enabled
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_THREATCATENABLED, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(NAVEMAIL_ThreatCatEnabled), CComVariant(iValue));
		}

		//set the threat cat enabled
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_THREATCATRESPONSE, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(NAVEMAIL_ThreatCatResponse), CComVariant(iValue));
		}

		//set the OEH Enabled Option
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_OEH, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(NAVEMAIL_OEH), CComVariant(iValue));
		}

		//set the OEH Response Mode
		GetPrivateProfileString(INI_EMAIL_SECTION, INI_EMAIL_OEH_RESPONSE, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(NAVEMAIL_OEHResponseMode), CComVariant(iValue));
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewEmail -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewEmail");
		bRet_val = false;
	}

	return bRet_val;
}


//
//Get the IM scanning options and set them in the ini file
//
bool COptionsMigration::GetOldIMScan()
{
	bool bRet_val = true;
	int iValue = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;

	try
	{
		//create and Manual Scan section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_IMSCAN_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		//check the enable MSN
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(IMSCAN_ScanMIM), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_ENABLE_MSN, szValue, m_szINIPath);
		}

		//check the enbale AOL
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(IMSCAN_ScanAIM), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_ENABLE_AOL, szValue, m_szINIPath);
		}

		//check the enbale YIM
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(IMSCAN_ScanYIM), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_ENABLE_YIM, szValue, m_szINIPath);
		}

		//check the response mode
        // only do this for 11.5 and higher
        if((11 < m_itbVersion.nMajorHi) || (11 == m_itbVersion.nMajorHi && 5 <= m_itbVersion.nMajorLo))
        {
		    if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(IMSCAN_ResponseMode), CComVariant(1), &vValue)))
		    {
			    iValue = vValue.iVal;
			    _itot(iValue, szValue, 10);
			    WritePrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_RESPONSE, szValue, m_szINIPath);
		    }
        }

		//check reply to sender for MSN option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(IMSCAN_MessageSender), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_ENABLE_MSN_REPLY, szValue, m_szINIPath);
		}

		//check threat cat enabled option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(IMSCAN_ThreatCatEnabled), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_THREATCATENABLED, szValue, m_szINIPath);
		}

		//check threat cat response option
        // only do this for 11.5 and higher
        if((11 < m_itbVersion.nMajorHi) || (11 == m_itbVersion.nMajorHi && 5 <= m_itbVersion.nMajorLo))
        {
		    if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(IMSCAN_ThreatCatResponse), CComVariant(1), &vValue)))
		    {
			    iValue = vValue.iVal;
			    _itot(iValue, szValue, 10);
			    WritePrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_THREATCATRESPONSE, szValue, m_szINIPath);
		    }
        }
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldEmail -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldEmail");
		bRet_val = false;
	}
	return bRet_val;
}


//
//Retrive all E-mail Scanning options from the ini file and set them in the new product
//
bool COptionsMigration::SetNewIMScan()
{
	bool bRet_val = true;
	TCHAR szValue[4096] = {0};
	int iValue = 0;
	
	try
	{	
		//set the scan MSN setting
		GetPrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_ENABLE_MSN, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			
			//set the options setting
			m_pOptionsObject->Put(CComBSTR(IMSCAN_ScanMIM), CComVariant(iValue));
		}
		
		//set the scan AOL setting
		GetPrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_ENABLE_AOL, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			
			//set the options setting
			m_pOptionsObject->Put(CComBSTR(IMSCAN_ScanAIM), CComVariant(iValue));
		}

		//set the scan YIM setting
		GetPrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_ENABLE_YIM, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(IMSCAN_ScanYIM), CComVariant(iValue));
		}
		
		//set the response mode setting
		GetPrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_RESPONSE, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(IMSCAN_ResponseMode), CComVariant(iValue));
		}

		//set the Message sender setting
		GetPrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_ENABLE_MSN_REPLY, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(IMSCAN_MessageSender), CComVariant(iValue));
		}

		//set threat cat enabled setting
		GetPrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_THREATCATENABLED, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(IMSCAN_ThreatCatEnabled), CComVariant(iValue));
		}

		//set threat cat enabled setting
		GetPrivateProfileString(INI_IMSCAN_SECTION, INI_IMSCAN_THREATCATRESPONSE, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(IMSCAN_ThreatCatResponse), CComVariant(iValue));
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewEmail -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewEmail");
		bRet_val = false;
	}

	return bRet_val;
}


//
//Get all the LU options and write them to the ini file
//
bool COptionsMigration::GetOldLU()
{
	bool bRet_val = true;
	int iValue = 0;
	TCHAR szValue[128] = {0};
	
	try
	{
		//create and ALU section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_LU_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		//check the enable ALU setting
		if(SUCCEEDED (m_pOptionsObject->get_LiveUpdate(&iValue)))
		{
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_LU_SECTION, INI_LU_ENABLEALU, szValue, m_szINIPath);
		}

		//check the ALU Mode setting
		if(SUCCEEDED (m_pOptionsObject->get_LiveUpdateMode((EAutoUpdateMode*)&iValue)))
		{
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_LU_SECTION, INI_LU_ALUMODE, szValue, m_szINIPath);
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldLU -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldLU");
		bRet_val = false;
	}

	return bRet_val;
}

//
//Retrive all LiveUpdate options from the ini file and set them in the new product
//
bool COptionsMigration::SetNewLU()
{
	bool bRet_val = true;
	TCHAR szValue[4096] = {0};
	int iValue = 0;
	CComVariant vValue;
	
	try
	{
		//set the enable ALU setting
		GetPrivateProfileString(INI_LU_SECTION, INI_LU_ENABLEALU, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);

			//set the options setting
			m_pOptionsObject->put_LiveUpdate(iValue);

			//set it in cfgwiz.dat
			NavOpts32_SetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_EnableAutoLiveUpdate, iValue);
		}
			
		//set the ALU Mode setting
		GetPrivateProfileString(INI_LU_SECTION, INI_LU_ALUMODE, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->put_LiveUpdateMode((EAutoUpdateMode)iValue);
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewLU -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewLU");
		bRet_val = false;
	}

	return bRet_val;
}


//
//Get all the logging options and set them in the ini file
//
bool COptionsMigration::GetOldLog()
{
	bool bRet_val = true;
	int iValue = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;

	try
	{
		//create and Loggin section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_LOGGING_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		//check the Keep up To (n bytes) option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(ACTIVITY_LogKeepUpTo), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_KEEPUPTO, szValue, m_szINIPath);
		}
		
		//check the Log Start End option
        //
        // Force these to ON since we removed them from the UI.
        // See defect # 404518.
        //
		WritePrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_STARTEND, _T("1"), m_szINIPath);

		//check the Log E-mail Scanning option
        //
        // Force these to ON since we removed them from the UI.
        // See defect # 404518.
        //
		WritePrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_EMAIL, _T("1"), m_szINIPath);

		//check the Log Known option
        //
        // Force these to ON since we removed them from the UI.
        // See defect # 404518.
        //
		WritePrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_KNOWN, _T("1"), m_szINIPath);
		
		//check the LogKiloBytes option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(ACTIVITY_LogKiloBytes), CComVariant(50), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_KILOBYES, szValue, m_szINIPath);
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldLog -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldLog");
		bRet_val = false;
	}
	return bRet_val;
}

//
//Retrive all logging options from the ini file and set them in the new product
//
bool COptionsMigration::SetNewLog()
{
	bool bRet_val = true;
	TCHAR szValue[4096] = {0};
	int iValue = 0;
	
	try
	{	
		//set the keep up to n bytes setting
		GetPrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_KEEPUPTO, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(ACTIVITY_LogKeepUpTo), CComVariant(iValue));
		}
		
		//set the log start end setting
		GetPrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_STARTEND, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(ACTIVITY_LogStartEnd), CComVariant(iValue));
		}
		
		//set the log email setting
		GetPrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_EMAIL, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(ACTIVITY_LogEMailScanning), CComVariant(iValue));
		}

		//set the log know setting
		GetPrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_KNOWN, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(ACTIVITY_LogKnown), CComVariant(iValue));
		}

		//set the LogKilobytes setting
		GetPrivateProfileString(INI_LOGGING_SECTION, INI_LOGGING_KILOBYES, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(ACTIVITY_LogKiloBytes), CComVariant(iValue));
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewLog -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewLog");
		bRet_val = false;
	}
	return bRet_val;
}

//
//Get all the misc options and set them in the ini file
//
bool COptionsMigration::GetOldMisc()
{
	bool bRet_val = true;
	int iValue = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;
	BOOL bOfficePlugin = FALSE;
	BOOL bStartupScan = FALSE;

	try
	{
		//create and Misc section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_MISC_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		//check the Alert Definitions option
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(DEFALERT_EnableOldDefs), CComVariant(1), &vValue)))
		{
			iValue = vValue.iVal;
			_itot(iValue, szValue, 10);
			WritePrivateProfileString(INI_MISC_SECTION, INI_MISC_ALERTDEFS, szValue, m_szINIPath);
		}
		
		//check the OfficePlugin option
		if(SUCCEEDED (m_pOptionsObject->get_OfficePlugin(&bOfficePlugin)))
		{
			WritePrivateProfileString(INI_MISC_SECTION, INI_MISC_OFFICEPLUGIN, (bOfficePlugin ? "1" : "0"), m_szINIPath);
		}

		//check the startupscan option
		if(SUCCEEDED (m_pOptionsObject->get_StartupScan(&bStartupScan)))
		{
			WritePrivateProfileString(INI_MISC_SECTION, INI_MISC_STARTUPSCAN, (bStartupScan ? "1" : "0"), m_szINIPath);
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldMisc -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldMisc");
		bRet_val = false;
	}
	return bRet_val;
}


//
//Retrive all misc options from the ini file and set them in the new product
//
bool COptionsMigration::SetNewMisc()
{
	bool bRet_val = true;
	TCHAR szValue[4096] = {0};
	int iValue = 0;
	BOOL bOfficePlugin = TRUE;
	BOOL bStartupScan = FALSE;
	
	try
	{	
		//set the Alert Defs setting
		GetPrivateProfileString(INI_MISC_SECTION, INI_MISC_ALERTDEFS, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			iValue = _ttoi(szValue);
			m_pOptionsObject->Put(CComBSTR(DEFALERT_EnableOldDefs), CComVariant(iValue));
		
			//set the cfgwiz.dat setting
			NavOpts32_SetDwordValue(m_hCfgWizDat, InstallToolBox::CFGWIZ_EnableAlertDefs, iValue);
		}

		//set the Office Plugin setting
		GetPrivateProfileString(INI_MISC_SECTION, INI_MISC_OFFICEPLUGIN, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			bOfficePlugin = _ttoi(szValue);
			m_pOptionsObject->put_OfficePlugin(bOfficePlugin);
		}

		//set the Startup Scan
		GetPrivateProfileString(INI_MISC_SECTION, INI_MISC_STARTUPSCAN, _T(""), szValue, sizeof(szValue), m_szINIPath);
		if(_tcscmp(_T(""), szValue) != 0)
		{
			bStartupScan = _ttoi(szValue);
			m_pOptionsObject->put_StartupScan(bStartupScan);
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewMisc -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewMisc");
		bRet_val = false;
	}

	return bRet_val;
}

//
//Get all the threat categories
//
bool COptionsMigration::GetOldTCAT()
{
	bool bRet_val = true;
	
	int iValue = 0;
	int iFirstVal = 0;
	int iLastVal = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;
	
	try
	{
		//threat cat options are different from other options.  there isn't finite set of values, intead there
		//is first and last value that define the boundaries that we need to loop through.

		//create and Misc section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_THREAT_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		// get the first non-viral threat number
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(THREAT_FirstKnownNonViral), CComVariant(1), &vValue)))
		{
			iFirstVal = vValue.iVal;
		}
		else
		{
			throw runtime_error("Couldn't read first non-viral threat index");
		}
		
		// get the last non-viral threat number
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(THREAT_LastKnownCategory), CComVariant(1), &vValue)))
		{
			iLastVal = vValue.iVal;
		}
		else
		{
			throw runtime_error("Couldn't read last non-viral threat index");
		}

		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		for(int i=iFirstVal; i<=iLastVal; i++)
		{
			// construct the option name from the base name and the current index
			TCHAR szCurrentOption[MAX_PATH] = {0};
			wsprintf(szCurrentOption, _T("%s%d"), _T("THREAT:Threat"), i);
			
			if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(szCurrentOption), CComVariant(1), &vValue)))
			{
				//construct a an ini entry name to store the current value
				TCHAR szCurrentIniEntry[MAX_PATH] = {0};
				wsprintf(szCurrentIniEntry, _T("%s%d"), INI_THREAT_ENTRYNAME, i);

				iValue = vValue.iVal;
				_itot(iValue, szValue, 10);
				WritePrivateProfileString(INI_THREAT_SECTION, szCurrentIniEntry, szValue, m_szINIPath);
			}
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldTCAT -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldTCAT");
		bRet_val = false;
	}
	
	return bRet_val;
}


//
//Retrive all misc options from the ini file and set them in the new product
//
bool COptionsMigration::SetNewTCAT()
{
	bool bRet_val = true;
	
	int iValue = 0;
	int iFirstVal = 0;
	int iLastVal = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;
	
	try
	{
		// get the first non-viral threat number, we get these values from the newly install dat file
		// so that we only end up setting valid values. if we used the range from the previously installed
		// product we could get some wierd results.
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(THREAT_FirstKnownNonViral), CComVariant(1), &vValue)))
		{
			iFirstVal = vValue.iVal;
		}
		else
		{
			throw runtime_error("Couldn't read first non-viral threat index");
		}
		
		// get the last non-viral threat number
		if(SUCCEEDED (m_pOptionsObject->Get(CComBSTR(THREAT_LastKnownCategory), CComVariant(1), &vValue)))
		{
			iLastVal = vValue.iVal;
		}
		else
		{
			throw runtime_error("Couldn't read last non-viral threat index");
		}

		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		for(int i=iFirstVal; i<=iLastVal; i++)
		{
			//construct a an ini entry name to store the current value
			TCHAR szCurrentIniEntry[MAX_PATH] = {0};
			wsprintf(szCurrentIniEntry, _T("%s%d"), INI_THREAT_ENTRYNAME, i);
			
			GetPrivateProfileString(INI_THREAT_SECTION, szCurrentIniEntry, _T(""), szValue, 128, m_szINIPath);
			if(_tcscmp(_T(""), szValue) != 0)
			{
				// construct the option name from the base name and the current index
				TCHAR szCurrentOption[MAX_PATH] = {0};
				wsprintf(szCurrentOption, _T("%s%d"), _T("THREAT:Threat"), i);
				
				iValue = _ttoi(szValue);
				m_pOptionsObject->Put(CComBSTR(szCurrentOption), CComVariant(iValue));		
			}
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewTCAT -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewTCAT");
		bRet_val = false;
	}
	return bRet_val;
}

//
//Get all the IWP options
//
bool COptionsMigration::GetOldIWP()
{
  /*
    WE CANNOT MIGRATE IWP OPTIONS BECAUSE AT THIS POINT WE DONT KNOW IF
    IF WE'RE ABOUT TO BE TOLD TO YIELD OR NOT BY AN EXISTING PRODUCT
    IF WE TELL IWP TO CHANGE FW STATE HE JUST MIGHT DO IT.

	bool bRet_val = true;
	
	int iValue = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;
	BOOL bValue = true;
	
	try
	{
		//create an IWP section in our ini file, if we can't create this there is no point in continuing so throw
		if(WritePrivateProfileSection(INI_IWP_SECTION, NULL, m_szINIPath) == 0)
			throw runtime_error("Couldn't Write to the INI file.");
		
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
        // This version must be 2005 or greater to propogate the IWP setting since
        // IWP does not exist pre-NAV 2005
        if( m_dwVersion < NAV2005 )
        {
            g_Log.Log("COptionsMigration::GetOldIWP() - NAV version is pre-NAV 2005, not getting IWP setting. Version = %d. Setting the ini value to 1", m_dwVersion);
            strcat(szValue, _T("1"));
            WritePrivateProfileString(INI_IWP_SECTION, INI_IWP_USERWANTSON, szValue, m_szINIPath);
        }
		else if(SUCCEEDED (m_pOptionsObject->get_IWPUserWantsOn(&bValue)))
		{
			if(bValue)
				strcat(szValue, _T("1"));
			else
				strcat(szValue, _T("0"));
			
			WritePrivateProfileString(INI_IWP_SECTION, INI_IWP_USERWANTSON, szValue, m_szINIPath);
		}
	
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldIWP -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldIWP");
		bRet_val = false;
	}
	
	return bRet_val;
    */
    
    return true;
}


//
//Retrive all IWP options
//
bool COptionsMigration::SetNewIWP()
{
/*
    WE CANNOT MIGRATE IWP OPTIONS BECAUSE AT THIS POINT WE DONT KNOW IF
    IF WE'RE ABOUT TO BE TOLD TO YIELD OR NOT BY AN EXISTING PRODUCT
    IF WE TELL IWP TO CHANGE FW STATE HE JUST MIGHT DO IT.
	bool bRet_val = true;

    if( m_dwVersion < NAV2005 )
    {
        g_Log.Log("COptionsMigration::SetNewIWP() - NAV version is pre-NAV 2005, not setting IWP setting. Version = %d.", m_dwVersion);
        return true;
    }
	
	int iValue = 0;
	TCHAR szValue[128] = {0};
	CComVariant vValue;
	
	try
	{
		//NOTE:from now on we won't throw on errors b/c we might as well check to see if we can get other options
		
		BOOL bUserWantsOn = true;
		
		GetPrivateProfileString(INI_IWP_SECTION, INI_IWP_USERWANTSON, _T(""), szValue, sizeof(szValue), m_szINIPath);

		if(_tcscmp(_T("0"), szValue) == 0)
			bUserWantsOn = false;
		else
			bUserWantsOn = true;

		m_pOptionsObject->put_IWPUserWantsOn(bUserWantsOn);
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewIWP -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewIWP");
		bRet_val = false;
	}
	return bRet_val;
*/

    return true;
}

//
//Retrieve all the old AdvTools options and store them off in the registry
//
bool COptionsMigration::GetOldAdvTools()
{
	bool bRet_val = true;
	
	try
	{
		//we dont want to migrate the old NAVPRO options if they aren't installing NAVPRO anymore
		if(!IsNavPro())
		{
			return bRet_val;
		}

		//we only want to migrate these options if NU is not installed, otherwise we don't want to muck around with somebody
		//else's options
		HKEY hInstalledAppsKey = NULL;
		if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYM_INSTALLED_APPS_KEY, 0, KEY_READ, &hInstalledAppsKey))
		{
			if(ERROR_SUCCESS != RegQueryValueEx(hInstalledAppsKey, NU_INSTALLED_APPS_NAME, NULL, NULL, NULL, NULL))
			{
				RegCopyKey(HKEY_LOCAL_MACHINE, ADVTOOLS_KEY, HKEY_LOCAL_MACHINE, ADVTOOLS_BACKUP_KEY, false);
				RegCopyKey(HKEY_LOCAL_MACHINE, RECYCLE_BIN_KEY, HKEY_LOCAL_MACHINE, RECYCLE_BIN_BACKUP_KEY, false);
			}
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOldAdvTools -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOldAdvTools");
		bRet_val = false;
	}

	return bRet_val;

}

//
//Restore the backup registry key to the advtools options key
//
bool COptionsMigration::SetNewAdvTools()
{
	bool bRet_val = true;

	try
	{
		//we dont want to migrate the old NAVPRO options if they aren't installing NAVPRO anymore
		//we also need to make sure to delete any backup keys we might have made in case the previous version was pro
		if(!IsNavPro())
		{
			SHDeleteKey(HKEY_LOCAL_MACHINE, ADVTOOLS_BACKUP_KEY);
			SHDeleteKey(HKEY_LOCAL_MACHINE, RECYCLE_BIN_BACKUP_KEY);
			return bRet_val;
		}
		
		//we only want to migrate these options if NU is not installed, otherwise we don't want to muck around with somebody
		//else's options
		HKEY hInstalledAppsKey = NULL;
		if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, SYM_INSTALLED_APPS_KEY, 0, KEY_READ, &hInstalledAppsKey))
		{
			if(ERROR_SUCCESS != RegQueryValueEx(hInstalledAppsKey, NU_INSTALLED_APPS_NAME, NULL, NULL, NULL, NULL))
			{
				RegCopyKey(HKEY_LOCAL_MACHINE, ADVTOOLS_BACKUP_KEY, HKEY_LOCAL_MACHINE, ADVTOOLS_KEY, true);
				RegCopyKey(HKEY_LOCAL_MACHINE, RECYCLE_BIN_BACKUP_KEY, HKEY_LOCAL_MACHINE, RECYCLE_BIN_KEY, true);
			}
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::SetNewAdvTools -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::SetNewAdvTools");
		bRet_val = false;
	}
	
	return bRet_val;
}

bool COptionsMigration::RegCopyKey(HKEY hSourceKeyParent, LPCTSTR szSourceKey, HKEY hDestKeyParent, LPCTSTR szDestKey, BOOL bDeleteSource)
{
	bool bRet_val = true;

	try
	{	
		HKEY hDestKey = NULL;
		if(ERROR_SUCCESS == RegCreateKeyEx(hDestKeyParent, szDestKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hDestKey, NULL))
		{	
			//now copy the source key to destination key
			bRet_val = ( (ERROR_SUCCESS == SHCopyKey(hSourceKeyParent, szSourceKey, hDestKey, 0)) ? true : false );
			
			//now, if specified, delete the source key to simulate a "move"
			if(bDeleteSource)
				SHDeleteKey(hSourceKeyParent, szSourceKey);
		
			RegCloseKey(hDestKey);
		}
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::RegCopyKey -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::RegCopyKey");
		bRet_val = false;
	}

	return bRet_val;
}


//finds out which version of NAV is installed by looking in the installed apps key
bool COptionsMigration::GetNAVVersion()
{	
	HKEY hKey = NULL;
	bool bRet_val = true;

	try
	{
		InstallToolBox::ITB_VERSION m_itbVersion;
		if(InstallToolBox::GetNAVVersion(&m_itbVersion))
		{
            g_Log.Log("COptionsMigration::GetNAVVersion() - Version is %d.%d", m_itbVersion.nMajorHi, m_itbVersion.nMajorLo);
		}
		
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetNAVVersion -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetNAVVersion");
		bRet_val = false;
	}
	
	if(hKey)
	{ 
		RegCloseKey(hKey);
	}
	
	return bRet_val;
}


//uses the options primitives to open up cfgwiz.dat
bool COptionsMigration::OpenCfgWizDAT()
{
	HKEY hKey = NULL;
	bool bRet_val = true;	

	try
	{
		//open the installed apps key
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\InstalledApps"), REG_OPTION_NON_VOLATILE, KEY_READ, &hKey) != ERROR_SUCCESS)
        {       
             throw runtime_error("Unable to find NAV Install key.");
        }
                 
		//find where NAV is installed to
		DWORD dwSize = sizeof(m_szCfgWizDatPath);
		if(RegQueryValueEx(hKey, _T("NAV"), NULL, NULL, (BYTE*)m_szCfgWizDatPath, &dwSize) != ERROR_SUCCESS)
		{                   
			throw runtime_error("Unable to find NAV Install key value.");
		}   
		
		//get the name of CfgWiz.dat
		_tcscat(m_szCfgWizDatPath, "\\CfgWiz.dat");
		
		//open an options handle to CfgWiz.dat
		NAVOPTS32_STATUS Status = NavOpts32_Allocate(&m_hCfgWizDat);
        if (Status != NAVOPTS32_OK)
            throw runtime_error("Unable to initialize the options library.");
        
        //load the CfgWizDat option file.
        Status = NavOpts32_Load(m_szCfgWizDatPath, m_hCfgWizDat, true);
        if (Status != NAVOPTS32_OK)
            throw runtime_error("Unable to load the CfgWiz.dat option file.");
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::OpenCfgWizDAT -- %s", ex.what());
		bRet_val = false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::OpenCfgWizDAT");
		bRet_val = false;
	}
	
	if(hKey)
	{ 
		RegCloseKey(hKey);
	}
	
	return bRet_val;
}


//instantiates the options com object we will need for migration
bool COptionsMigration::GetOptionsObject()
{
	try
	{
		//initialize COM
		if ( FAILED (CoInitialize(NULL)) )
			throw runtime_error("Couldn't Initialize COM");
		
		//instantiate the Options com object
		if( FAILED (CoCreateInstance(CLSID_NAVOptions, NULL, CLSCTX_INPROC_SERVER, 
			IID_INAVOptions, (void**) &m_pOptionsObject)) )
			throw runtime_error("Couldn't Open an Options Object");

		//set security
		ISymScriptSafe* pss = NULL;
		if(!SUCCEEDED (m_pOptionsObject->QueryInterface( IID_ISymScriptSafe, (void**) &pss )) )
			throw runtime_error("Couldn't Set SymScriptSafe");
		
		if(!SUCCEEDED( pss->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB)) )
			throw runtime_error("Couldn't Set SymScriptSafe");
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::GetOptionsObject -- %s", ex.what());
		return false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::GetOptionsObject");
		return false;
	}
	
	return true;
}


//opens up or creates the ini files we will need for migration
bool COptionsMigration::OpenIniFile()
{
	try
	{
		//get the path where we want to have the ini file
		GetWindowsDirectory(m_szINIPath, sizeof(m_szINIPath));
		_tcscat(m_szINIPath, _T("\\TEMP"));	

		//check if the \WINDOWS\TEMP directory exists
		if(GetFileAttributes(m_szINIPath) == -1)
			if(CreateDirectory(m_szINIPath, NULL) == 0)
				throw runtime_error("Couldn't Create Temp Directory");

		//append the file name
		_tcscat(m_szINIPath, _T("\\migration.ini"));

		//open the file or create it if it doesnt exist
		m_hINIFile = CreateFile(m_szINIPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(m_hINIFile == INVALID_HANDLE_VALUE)
		{
			HRESULT hrError = GetLastError();
			g_Log.Log("Create File returned: %d", hrError);
			throw runtime_error("Couldn't Open Migration.ini File");
		}

		//if the file already existed assume we are in 2nd phase of migration and we now allow calling of SetNewNew()
		if(ERROR_ALREADY_EXISTS == GetLastError())
			m_bWritingNew = true;
		else
			m_bReadingOld = true;

		//we don't need this handle anymore
		CloseHandle(m_hINIFile);
		m_hINIFile = INVALID_HANDLE_VALUE;
	}
	catch(exception &ex)
	{
		g_Log.Log("COptionsMigration::OpenIniFile -- %s", ex.what());
		return false;
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in COptionsMigration::OpenIniFile");
		return false;
	}
	
	return true;
}

// checks to see if NAVPro is on the system...
bool COptionsMigration::IsNavPro()
{
	LONG lResult;
	HKEY hKey;
	bool bReturn = false;
	
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"), 0, KEY_QUERY_VALUE, &hKey);
	
	if (lResult == ERROR_SUCCESS)
	{
		TCHAR szNAVInstType[2] = {0};
		DWORD dwSize = sizeof(szNAVInstType);
		
		lResult = RegQueryValueEx(hKey, _T("Type"), NULL, NULL, (BYTE* ) &szNAVInstType, &dwSize);
		
		if (lResult == ERROR_SUCCESS)
		{
			// If it's NAV Professional, Type should be set to 1
			if (_ttoi(szNAVInstType) == 1)
			{
				bReturn = true;
			}
		}
		
		RegCloseKey(hKey);
	}

	return bReturn;
}
