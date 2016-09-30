// Copyright 2000 Symantec, Peter Norton Computing Group
// -------------------------------------------------------------------------
// NavCommandLines.cpp
//		This file contains the class which handles registering and manipulating the
// NAV command lines with LiveUpdate.  This class is also responsible for removing
// virus definitions' command lines, it told to do so.
//
// $Header:   
// -------------------------------------------------------------------------
// $Log:   
// 
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"					// Pre-compiled header file.
#include "resource.h"				// Resource symbols definitions

#import "LUCOM.TLB"					// LiveUpdate COM Server type library
#import  "PRODUCTREGCOM.TLB"		// ProductReg COM type library
#include "LuCom.h"		            // The LuComServer COM include file.
#include "SettingsNames.h"

using namespace PRODUCTREGCOMLib;

#include "NavCommandLines.h"		// Nav Command Line manipulation class (this file )

#include "ProductRegCOMNames.h"		// Names for Product Reg COM properties.
#include "AutoUpdateDefines.h"		// Auto LiveUpdate property names.

#include "NAVOPTHelper.H"

const CLSID CLSID_luNavCallBack = {0x09C9DBC1,0x893D,0x11D2,{0xB4,0x0A,0x00,0x60,0x08,0x31,0xDD,0x76}};

//----------------------- Registry Key Constant Strings -----------------------

const TCHAR cstrCommandLineKey[] = 
				_T("SOFTWARE\\Symantec\\Norton AntiVirus\\LiveUpdate\\CmdLines\\CmdLine%d");
const TCHAR cstrOtherSwitchesKey[]  = _T("OtherSwitches");
const TCHAR cstrProductNameKey[]    = _T("ProductName");
const TCHAR cstrProductTypeKey[]    = _T("ProductType");
const TCHAR cstrLanguageKey[]       = _T("ProductLanguage");
const TCHAR cstrProductVersionKey[] = _T("ProductVersion");
const TCHAR cstrProductMonikerKey[] = _T("ProductMoniker");
const TCHAR cstrIsNavCallbackKey[]  = _T("CallbacksHere");
const TCHAR cstrAreGroupedKey[]     = _T("GroupTogether");
const TCHAR cstrSetSKUProperty[]     = _T("SetSKUProperty");
const TCHAR cstrSetEnvironment[]     = _T("SetEnvironment");

//------------------------ Command Line Moniker GUIDS-------------------------
#define FULL_DEFS_GUID   "{E5C8D100-B06E-11d2-9C30-00C04FB59D98}"
#define OVERRIDE_GUID    "{C128CA20-B06F-11d2-9C30-00C04FB59D98}"


//------------------------ LUNAVCallBack GUID----------------------------------
#define LUNAVCallBackGUID	"09C9DBC1-893D-11D2-B40A-00600831DD76"


//-------------------------- CNavCommandLines --------------------------------

/////////////////////////////////////////////////////////////////////////////
// CNavCommandLines (Constructor) -
CNavCommandLines::CNavCommandLines()
{
    //SetResourceHandle ( ghInstance );
} // CNavCommandLines Constructor

/////////////////////////////////////////////////////////////////////////////
// CNavCommandLines (Destructor) -
CNavCommandLines::~CNavCommandLines()
{
} // CNavCommandLines Destructor

/////////////////////////////////////////////////////////////////////////////
// RegisterCommandLines -
//		This function will register all of the command lines with LiveUpdate
// The command lines are added to the registery under 
// HKLM\software\Symantec\Norton AntiVirus\LiveUpdate\CmdLines key during
// installation.  This function will add these command lines to LiveUpdate
// during installation and will modify them after installation.
HRESULT CNavCommandLines::RegisterCommandLines()
{
	HRESULT hReturn = S_OK;
	TCommandLine	structCmdLine;
	_variant_t vFullDefsMoniker( _bstr_t( FULL_DEFS_GUID ) );
	IluGroupPtr ptrGroup;

	// Before we start having too much fun, we'll have to set everything up.
	// We need to create the two groups, overridden and coupled, that NAV will
	// use.  The overridden group isn't used right now, but if it were, any 
	// product that registers the same product, version and language as the
	// overridden group would use our commands instead of the ones the registered.
	// The coupled group is used to group the NAV command lines.  This will 
	// force the uses to select or deselect the virus defs as a group.
	try
	{
		_variant_t vOverrideMoniker( _bstr_t( OVERRIDE_GUID ) );

        hReturn = ptrGroup.CreateInstance( __uuidof( PRODUCTREGCOMLib::luGroup ), NULL, 
										   CLSCTX_INPROC );
		if ( FAILED(hReturn) )
			throw runtime_error("CNAVCommandLines couldn't CreateInstance of luGroup");

		hReturn = ptrGroup->RegisterGroup( _bstr_t(LUPR_GT_COUPLED), &vFullDefsMoniker );
		hReturn = ptrGroup->RegisterGroup( _bstr_t(LUPR_GT_OVERRIDDEN), &vOverrideMoniker );
		if ( FAILED(hReturn) )
			throw runtime_error("CNAVCommandLines couldn't ResigsterGroup");

		// Add a command line to the group. It doesn't really matter which one
		// we choses, so we'll use the first command line in the registry.
//		GetNextCommandLine( 1, structCmdLine );
//		hReturn = ptrGroup->AddToGroup( vOverrideMoniker.bstrVal, 
//									_bstr_t( structCmdLine.m_strProductName ), 
//									_bstr_t( structCmdLine.m_strProductVersion ), 
//									_bstr_t( structCmdLine.m_strProductLanguage ), 
//									_bstr_t( "" ) );
	}
	catch(exception &ex)
	{
		// If we got here then we couldn't register the coupled groups info.  This is
		// bad.  Don't go any farther because we have to ensure that the virus defs 
		// command lines are always selected as a group.
		_ASSERT(FALSE);
		if ( SUCCEEDED( hReturn ) )
			hReturn = E_ABORT;
	}
	catch(...)
	{
		// If we got here then we couldn't register the coupled groups info.  This is
		// bad.  Don't go any farther because we have to ensure that the virus defs 
		// command lines are always selected as a group.
		_ASSERT(FALSE);
		if ( SUCCEEDED( hReturn ) )
			hReturn = E_ABORT;
	}

	if ( FAILED(hReturn) )
		return E_ABORT;

	try
	{
		// Now for the fun part.  We have to go through each command line in the
		// registry and check to see if it is installed with LiveUpdate.  We have
		// to update the command line based on what is already installed.  There
		// aren't that may cases, but there is a bunch of checking to do, so I'll
		// put the code in a separate function..

		// For now, we'll go through each of the command lines in the registry,
		// checking to see if the command line is part of the coupled group.  If
		// it is we will add the command to the group.  Then we will call the 
		// function alluded to above.
		int nIdx = 1;
		while ( GetNextCommandLine( nIdx, structCmdLine ) )
		{
			if ( structCmdLine.m_bIsGroupTogether )
			{
				hReturn = ptrGroup->AddToGroup( vFullDefsMoniker.bstrVal, 
												_bstr_t( structCmdLine.m_strProductName ), 
												_bstr_t( structCmdLine.m_strProductVersion ), 
												_bstr_t( structCmdLine.m_strProductLanguage ), 
												_bstr_t( "" ) );	// Type
				if ( FAILED(hReturn) )
					throw hReturn;
			}

			// Now we need to process each command line and determine what to do.
			ProcessCommandLine( structCmdLine, nIdx );

			// Get the next command line.
			nIdx++;
		}
	}
	catch(...)
	{
		_ASSERT(FALSE);
		if ( SUCCEEDED( hReturn ) )
			hReturn = E_ABORT;
	}
	return hReturn;
} // RegisterCommandLines


/////////////////////////////////////////////////////////////////////////////
// GetNextCommandLine -
//		This function will get the next NAV command line.  The master version of 
// all of NAV's command lines are stored in the registry.  These command lines
// are used to update LiveUpdate's command lines.  The command lines are referenced
// by unique assending numbers, starting with 1.
BOOL CNavCommandLines::GetNextCommandLine( DWORD dwCmdLineNumber, TCommandLine &structCmdLine )
{
	BOOL bReturn = TRUE;

	try
	{
		// First, we need to build the registry key that corresponds to this command line
		// number.
		ATL::CString strCommandLineKey;
		strCommandLineKey.Format( (LPCTSTR)cstrCommandLineKey, dwCmdLineNumber );

		// Open the registry key for this command line.
		HKEY hKey;
		if(ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, strCommandLineKey, 0, KEY_QUERY_VALUE, &hKey ))
		{
			// the key doesn't exist so fail
			return FALSE;
		}

		// Fill in the given structure with all of the values of the current command line.
		TCHAR szValue[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
        if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrProductNameKey, 0, NULL, (BYTE*)szValue, &dwSize) )
		    structCmdLine.m_strProductName = szValue;
        else
            structCmdLine.m_strProductName = "";

		dwSize = MAX_PATH;
        if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrProductVersionKey, 0, NULL, (BYTE*)szValue, &dwSize) )
		    structCmdLine.m_strProductVersion = szValue;
        else
            structCmdLine.m_strProductVersion = "";

		dwSize = MAX_PATH;
        if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrLanguageKey, 0, NULL, (BYTE*)szValue, &dwSize) )
		    structCmdLine.m_strProductLanguage = szValue;
        else
            structCmdLine.m_strProductLanguage = "";

		dwSize = MAX_PATH;
		if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrProductTypeKey, 0, NULL, (BYTE*)szValue, &dwSize) )
            structCmdLine.m_strProductType = szValue;
        else
            structCmdLine.m_strProductType = "";

		dwSize = MAX_PATH;
		if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrProductMonikerKey, 0, NULL, (BYTE*)szValue, &dwSize) )
            structCmdLine.m_strProductMoniker = szValue;
        else
            structCmdLine.m_strProductLanguage = "";

		dwSize = MAX_PATH;
		if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrOtherSwitchesKey, 0, NULL, (BYTE*)szValue, &dwSize) )
            structCmdLine.m_strOtherSwitches = szValue;
        else
            structCmdLine.m_strOtherSwitches = "";

		// Convert the group and Nav callback flags into booleans.
		dwSize = MAX_PATH;
		if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrIsNavCallbackKey, 0, NULL, (BYTE*)szValue, &dwSize) )
		    structCmdLine.m_bIsNAVCalback = static_cast<BOOL>( _ttol( szValue ) );
        else
            structCmdLine.m_bIsNAVCalback = FALSE;
		
		dwSize = MAX_PATH;
		if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrAreGroupedKey, 0, NULL, (BYTE*)szValue, &dwSize) )
		    structCmdLine.m_bIsGroupTogether = static_cast<BOOL>( _ttol( szValue ) );
        else
            structCmdLine.m_bIsGroupTogether = FALSE;
		
        // The SKU and ENVIORNMENT properties will only exist for NAV 2004
		dwSize = MAX_PATH;
		if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrSetSKUProperty, 0, NULL, (BYTE*)szValue, &dwSize) )
		    structCmdLine.m_bSetSKUProperty = static_cast<BOOL>( _ttol( szValue ) );
        else
            structCmdLine.m_bSetSKUProperty = FALSE;
		
		dwSize = MAX_PATH;
		if( ERROR_SUCCESS == RegQueryValueEx(hKey, cstrSetEnvironment, 0, NULL, (BYTE*)szValue, &dwSize) )
		    structCmdLine.m_bSetEnvironment = static_cast<BOOL>( _ttol( szValue ) );
        else
            structCmdLine.m_bSetEnvironment = FALSE;

		RegCloseKey(hKey);
	}
	catch (...)
	{
		// If we're getting this ASSERT either we don't have access to the registry
		// or we didn't ask for enough permissions to these keys.
		_ASSERT (FALSE);
		bReturn = FALSE;
	}

    return bReturn;
} // GetNextCommandLine

/////////////////////////////////////////////////////////////////////////////
// SetCommandLine -
//		This function will set the given NAV command lines product, version, language, type,
// moniker or other switches fields.  If any of these parameters are NULL, those parameters
// will not be set.
BOOL CNavCommandLines::SetCommandLine( DWORD dwCmdLineNumber, TCommandLine &structCmdLine )
{
	BOOL bReturn = FALSE;

	try
	{
		// First, we need to build the registry key that corresponds to this command line
		// number.
		ATL::CString strCommandLineKey;
		strCommandLineKey.Format( cstrCommandLineKey, dwCmdLineNumber );

		// Open the registry key for this command line.
		CRegKey	stRegKey;
		if(ERROR_SUCCESS != stRegKey.Open( HKEY_LOCAL_MACHINE, strCommandLineKey, KEY_SET_VALUE ))
		{
			//the key doesn't exist
			return FALSE;
		}

		ATL::CString	strValue;
		
		// Set the command line information in the registry, if the field is set in the
		// structure.
		if ( !structCmdLine.m_strProductName.IsEmpty() )
			stRegKey.SetValue( cstrProductNameKey, REG_SZ, (void*)(LPCSTR)structCmdLine.m_strProductName, 
				structCmdLine.m_strProductName.GetAllocLength() );
		
		if ( !structCmdLine.m_strProductVersion.IsEmpty() )
			stRegKey.SetValue( cstrProductVersionKey, REG_SZ, (void*)(LPCSTR)structCmdLine.m_strProductVersion, 
				structCmdLine.m_strProductVersion.GetAllocLength() );
		
		if ( !structCmdLine.m_strProductLanguage.IsEmpty() )
			stRegKey.SetValue( cstrLanguageKey, REG_SZ, (void*)(LPCSTR)structCmdLine.m_strProductLanguage, 
				structCmdLine.m_strProductLanguage.GetAllocLength() );
		
		if ( !structCmdLine.m_strProductType.IsEmpty() )
			stRegKey.SetValue( cstrProductTypeKey, REG_SZ, (void*)(LPCSTR)structCmdLine.m_strProductType, 
				structCmdLine.m_strProductType.GetAllocLength() );
		
		if ( !structCmdLine.m_strProductMoniker.IsEmpty() )
			stRegKey.SetValue( cstrProductMonikerKey, REG_SZ, (void*)(LPCSTR)structCmdLine.m_strProductMoniker, 
				structCmdLine.m_strProductMoniker.GetAllocLength() );

		if ( !structCmdLine.m_strOtherSwitches.IsEmpty() )
			stRegKey.SetValue( cstrOtherSwitchesKey, REG_SZ, (void*)(LPCSTR)structCmdLine.m_strOtherSwitches, 
				structCmdLine.m_strOtherSwitches.GetAllocLength() );
				
		bReturn = TRUE;
	}
	catch (...)
	{
		// If we're getting this ASSERT either we don't have access to the registry
		// or we didn't ask for enough permissions to these keys.
		_ASSERT (FALSE);
		bReturn = FALSE;
	}

    return bReturn; 
} // SetCommandLine

/////////////////////////////////////////////////////////////////////////////
// ProcessCommandLine -
//		This function will take a command line from the registry and compare it to 
// the corresponding command line in LiveUpdate.  Based on the results of the
// comparison, this function will decide how to update the command line in
// LiveUpdate.
HRESULT CNavCommandLines::ProcessCommandLine( TCommandLine &structCmdLine, 
											  DWORD dwCommandLineNum )
{
	HRESULT hReturn;

	try
	{
		IluProductRegPtr ptrProductReg;

		hReturn = ptrProductReg.CreateInstance(__uuidof(PRODUCTREGCOMLib::luProductReg), 
									NULL, CLSCTX_INPROC);
		if (FAILED(hReturn))
			throw hReturn;

		_variant_t vMoniker;

		// First we need to find if the this command line is already registered 
		// with LiveUpdate.
		vMoniker = ptrProductReg->FindMoniker(_bstr_t( structCmdLine.m_strProductName ),
										 _bstr_t( structCmdLine.m_strProductVersion ),
										 _bstr_t( structCmdLine.m_strProductLanguage ),
										 _bstr_t( "" ) );

		// Now we need to compare what is register with LiveUpdate to what is in the 
		// registry.  The first set of cases will deal with what happens if LiveUpdate
		// does not have a moniker registered for the given product, version, language
		// and type of the registry command line.
		if ( vMoniker.vt == VT_EMPTY )
		{
			// So, now we know that there is no product, version, language, and type
			// registered with LiveUpdate that matches the given command line.  We
			// will need to register this command line with LiveUpdate.

			try
			{
				// Our moniker is not registered with LiveUpdate.  Register it 
				// before updating all of the properties.
				_variant_t vNewMoniker( _bstr_t( structCmdLine.m_strProductMoniker ) );
				ptrProductReg->RegisterProduct( 
									 _bstr_t( structCmdLine.m_strProductName ),
									 _bstr_t( structCmdLine.m_strProductVersion ),
									 _bstr_t( structCmdLine.m_strProductLanguage ),
									 _bstr_t( "" ), 
									 &vNewMoniker );
			}
			catch(...)
			{
				// LiveUpdate may throw an exception here if the product is already 
				// registered.  We are just trying to make sure it is, so this
				// shouldn't be a problem.
			}

			// The command line isn't registered with LiveUpdate so register it.
			hReturn = RegisterCommandLine(structCmdLine);

            // If this command line is the command line that has the callback, register
		    // the callback guid with LiveUpdate.
            if (structCmdLine.m_bIsNAVCalback)
		    {

			    // Register the callback on this command line
			    hReturn = ptrProductReg->RegisterCallback(_bstr_t(structCmdLine.m_strProductMoniker), 
                    CLSID_luNavCallBack, (PRODUCTREGCOMLib::tagLU_CALLBACK_TYPE)( PRODUCTREGCOMLib::PreSession | PRODUCTREGCOMLib::PostSession | 
                                        PRODUCTREGCOMLib::WelcomeText | PRODUCTREGCOMLib::FinishText));

			    // Clear the status text just in case something was left over
			    ptrProductReg->DeleteProperty(_bstr_t( structCmdLine.m_strProductMoniker), 
									    _bstr_t(LUPR_STATUSTEXT));
		    }
		}
		
	}
	catch(HRESULT hr)
	{
		// If we're getting this ASSERT it's because a call to ProductRegCOM failed.
		_ASSERT(FALSE);
		hReturn = hr;
	}
	catch( _com_error &err )
	{
		// If we're getting this ASSERT it's because a call to ProductRegCOM failed.
		_ASSERT(FALSE);
		hReturn = err.Error();
	}
	catch(...)
	{
		// If we're getting this ASSERT it's because a call to ProductRegCOM failed.
		_ASSERT (FALSE);
		if ( SUCCEEDED(hReturn) )
			hReturn = E_FAIL;
	}

	return hReturn;
} // ProcessCommandLine

/////////////////////////////////////////////////////////////////////////////
// RegisterCommandLine -
//		This function will register the given registry command line with LiveUpdate.
HRESULT CNavCommandLines::RegisterCommandLine(TCommandLine &structCmdLine)
{
	HRESULT hReturn = S_OK;
	ATL::CString strDescriptiveText;

	try
	{
		IluProductRegPtr ptrProductReg;

		hReturn = ptrProductReg.CreateInstance(__uuidof( PRODUCTREGCOMLib::luProductReg ), 
										NULL, CLSCTX_INPROC);
		if (SUCCEEDED(hReturn))
		{
            ptrProductReg->SetProperty(_bstr_t( structCmdLine.m_strProductMoniker ),
										_bstr_t( LUPR_PRODUCT ), 
										_bstr_t( structCmdLine.m_strProductName ) );

			ptrProductReg->SetProperty(_bstr_t( structCmdLine.m_strProductMoniker ),
											_bstr_t( LUPR_VERSION ), 
											_bstr_t( structCmdLine.m_strProductVersion ) );

			ptrProductReg->SetProperty(_bstr_t( structCmdLine.m_strProductMoniker ),
											_bstr_t( LUPR_LANGUAGE ), 
											_bstr_t( structCmdLine.m_strProductLanguage ) );

			// NAV isn't registering types right now.  All types are "" strings.
			ptrProductReg->SetProperty(_bstr_t( structCmdLine.m_strProductMoniker ),
											_bstr_t( LUPR_TYPE ), 
											_bstr_t( "" ) );

			BOOL IsAutoUpdateSafe = FALSE;
            BOOL bIsSilentMode = FALSE;
			// Get all of the data that is dependent on the command line but is not
			// stored in the registry.
			GetCommandLineGeneratedData( structCmdLine.m_strProductName, 
										 structCmdLine.m_bIsNAVCalback, 
										 strDescriptiveText, IsAutoUpdateSafe, bIsSilentMode );

			ptrProductReg->SetProperty(_bstr_t( structCmdLine.m_strProductMoniker ),
											_bstr_t( LUPR_DESCRIPTIVENAME ), 
											_bstr_t( strDescriptiveText ) );

			// Set the AutoUpdate flag for this command line.
			if ( IsAutoUpdateSafe )
			{
				ptrProductReg->SetProperty(_bstr_t( structCmdLine.m_strProductMoniker ), 
											_bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
											_bstr_t( AU_REG_USE_AU_FOR_UPDATES_ON ) );
			}
			else
			{
                ptrProductReg->SetProperty(_bstr_t( structCmdLine.m_strProductMoniker ), 
                    _bstr_t( AU_REG_USE_AU_FOR_UPDATES ),
                    _bstr_t( AU_REG_USE_AU_FOR_UPDATES_OFF ) );
            }
            if( bIsSilentMode )
            {
                ptrProductReg->SetProperty(_bstr_t( structCmdLine.m_strProductMoniker ), 
                    _bstr_t( AU_REG_UPDATE_TYPE ),
                    _bstr_t( AU_REG_PATCH_SILENT ) );
            }
            else
            {
                ptrProductReg->SetProperty(_bstr_t( structCmdLine.m_strProductMoniker ), 
                    _bstr_t( AU_REG_UPDATE_TYPE ),
                    _bstr_t( AU_REG_PATCH_NOISY ) );
            }

			// if we're supposed to set the SKU propery let's do it
			if(structCmdLine.m_bSetSKUProperty)
			{
				TCHAR szSKU[MAX_PATH] = {0};
				CNAVOptFile NAVOptDat;
				NAVOptDat.Init();
				NAVOptDat.GetValue(_T("SKU:PAIDSKU"), szSKU, MAX_PATH, "");
				NAVOptDat.Cleanup();
				ptrProductReg->SetProperty( _bstr_t(structCmdLine.m_strProductMoniker), _bstr_t( "SKU" ), _bstr_t( szSKU ) );
			}

			// if we're supposed to set the Target Environment let's do it
			if(structCmdLine.m_bSetEnvironment)
			{
                try
	            {
                    STLIVEUPDATECOMLib::IstCheckForUpdatesPtr		ptrLiveUpdate;

		            // Create the the LiveUpdate CheckForUpdates object
		            HRESULT hr = ptrLiveUpdate.CreateInstance( __uuidof( stCheckForUpdates ), NULL, 
										            CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER );
		            if ( FAILED( hr ) )
			            throw hr;

     		        // Load the settings file from disk.
		            hr = ptrLiveUpdate->LoadSettings();
		            if ( FAILED( hr ) )
			            throw hr;
            	
		            // Get a pointer to the Settings file object.
		            STLIVEUPDATECOMLib::IstSettingsPtr ptrSettings = ptrLiveUpdate->GetSettings();
		            if ( ptrSettings == NULL )
			            throw E_FAIL;

		            // Set the Environment property
		            ptrSettings->PutProperty( bstrPREFS_ENVIRONMENT, bstrENVIROMENT_RETAIL );
	            }
	            catch(exception &ex)
	            {
		            // Handle fatal errors here.  I wouldn't do more than log an error here.  This failure will not be fatal to LiveUpdate or your product's 
		            // registration, unless LiveUpdate is not correctly installed.  If LiveUpdate is not working, then your Product Registration code will
		            // probably already handle this type of error correctly.
                  
	            }
                catch(...)
                {
                   
                }
			}
        }

	}
	catch (...)
	{
		// If we're getting this ASSERT either we don't have access to the registry
		// or we didn't ask for enough permissions to these keys.
		_ASSERT (FALSE);
		hReturn = E_FAIL;
	}

    return hReturn;
} // RegisterCommandLine

BOOL IsNavPro()
{
	BOOL bReturn = FALSE;
	CRegKey rk;
	if(ERROR_SUCCESS == rk.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus")))
	{
		TCHAR szNAVInstType[2] = {0};
		DWORD dwSize = sizeof(szNAVInstType);

#if _ATL_VER < 0x0700
		if(ERROR_SUCCESS == rk.QueryValue(szNAVInstType, _T("Type"), &dwSize))
#else // _ATL_VER < 0x0700
		if(ERROR_SUCCESS == rk.QueryStringValue(_T("Type"), szNAVInstType, &dwSize))
#endif // _ATL_VER < 0x0700
		{
			// If it's NAV Professional, Type should be set to 1
			if(1 == _ttoi(szNAVInstType))
			{
				bReturn = TRUE;
			}
		}
	}
	
	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
// GetCommandLineGeneratedData -
//		This function will get the descriptive name for the command line and whether
// or not the command line is Auto LiveUpdate safe.  All this information is 
// generated depending on the product name of the command line.
void CNavCommandLines::GetCommandLineGeneratedData( ATL::CString strProductName, BOOL bNavCallback, 
						 ATL::CString& strDescriptiveText, BOOL &IsAutoUpdateSafe, BOOL &bIsSilentMode )
{
	if ( bNavCallback )
	{	
		if (IsNavPro() == TRUE)
		{			
			strDescriptiveText = "Norton AntiVirus Professional";
			IsAutoUpdateSafe = TRUE;
			bIsSilentMode = FALSE;
		}
		else
		{
			strDescriptiveText = "Norton AntiVirus";
			IsAutoUpdateSafe = TRUE;
			bIsSilentMode = FALSE;
		}
		return;
	}
	if ( strProductName.Find("Avenge") >= 0 )
	{
		strDescriptiveText = "Norton AntiVirus Virus Definitions";
		IsAutoUpdateSafe = TRUE;
        bIsSilentMode = TRUE;
		return;
	}

	if (strProductName.Find("Scan") >= 0) 
	{		
		strDescriptiveText = "Symantec Security Response Submission Software Updates";
		IsAutoUpdateSafe = FALSE;
        bIsSilentMode = FALSE;
		return;
	}

   	strDescriptiveText = strProductName;
	IsAutoUpdateSafe = FALSE;
    bIsSilentMode = FALSE;
} // GetCommandLineGeneratedData
