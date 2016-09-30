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
#include "helper.h"

// installtoolbox headers
#include "Navdetection.h"
#include "instopts.h"

using namespace InstallToolBox;

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
		g_Log.LogEx(ex.what());
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
			hReturn = ProcessCommandLine( structCmdLine, nIdx );
			if ( FAILED(hReturn) )
				throw hReturn;

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
// EnableVirusDefinitionsCommandLines -
//		This function will enable or disable the virus definitions's command lines
// based on the enable flag.  
HRESULT CNavCommandLines::EnableVirusDefinitionsCommandLines(HRESULT hEnable)
{
	HRESULT hr = S_OK;

	try
	{
		// Loop through the command lines, looking for ones that have their Group Together
		// flag set.  These command lines are the virus definitions.
		int nIdx = 1;
		TCommandLine	structCmdLine;

		while ( GetNextCommandLine( nIdx++, structCmdLine ) )
		{
			if ( structCmdLine.m_bIsGroupTogether )
			{
				// We've found one of the virus definitions' command lines.  See if we're
				// supposed to enable or disable the command line.

				IluProductRegPtr ptrProductReg;

				hr = ptrProductReg.CreateInstance(__uuidof( PRODUCTREGCOMLib::luProductReg ), 
												NULL, CLSCTX_INPROC);
				if (SUCCEEDED(hr))
				{

					if ( hEnable == S_OK )
					{
						// Enable the command line by deleting the Abort property. (We're not
						// required to do this, I'm just doing it to be safe.)
						ptrProductReg->DeleteProperty(_bstr_t(structCmdLine.m_strProductMoniker),
													_bstr_t(LUPR_ABORT));
					}
					else
					{
						// We need to disable the command line, so set the abort property.
						ptrProductReg->SetProperty(_bstr_t(structCmdLine.m_strProductMoniker), 
												_bstr_t( LUPR_ABORT ), _bstr_t("YES"));
					}
				}
	
			}
		}
	}
	catch(...)
	{
//		ATLASSERT(FALSE);
		if ( SUCCEEDED(hr) )
			hr = E_FAIL;
	}
	return hr;
} // EnableVirusDefinitionsCommandLines

/////////////////////////////////////////////////////////////////////////////
// ModifyVirusDefsVersion -
//		This function will change the version string for the MicroDefs command lines.
// The change will happen in the registry copy of the command lines, not in
// LiveUpdate's settings.  NOTE: The version string contains the text needed
// to identify which one of the virus defs' command lines to update.
HRESULT CNavCommandLines::ModifyVirusDefsVersion( LPCTSTR szVersion )
{
	ATL::CString strVersion( szVersion );

	// The Virus Defs' version string have format of MicroDefsB.<month or full> or
	// MicroDefsT.<month or full>.  To find the correct command line, we need to 
	// look for the virus definition line that has the same text before the period in
	// the version field.  To do this, we need to find the period in the string.
	int nPeriodIdx = strVersion.Find( "." );
	if ( nPeriodIdx == -1 )
		return S_FALSE;

	ATL::CString strVersionId = strVersion.Left( nPeriodIdx );

	// Loop through the command lines, looking for ones that have their Group Together
	// flag set.  These command lines are the virus definitions.  From the grouped 
	// command lines, find the one that matches the first part of the version string.
	int nIdx = 1;
	TCommandLine	structCmdLine;
	HRESULT hReturn = S_FALSE;

	while ( GetNextCommandLine( nIdx, structCmdLine ) )
	{
		if ( structCmdLine.m_bIsGroupTogether )
		{
			// We've got a virus definition's command line, to a text compare
			// to see if it is the one we want.
			if ( !strVersionId.CompareNoCase( 
						structCmdLine.m_strProductVersion.Left( nPeriodIdx ) ) )
			{
				TCommandLine	structChangeCmdLine;
				structChangeCmdLine.m_strProductVersion = strVersion;
			
				// We've found the command line we're looking for.  Now we need to update
				// the version string.
				SetCommandLine( nIdx, structChangeCmdLine );
				hReturn = S_OK;

				// There will be only one command line that matches the given version
				// string.  There is no point in continuing our command line search, so
				// break out of the loop.
				break;
			}
		}

		// Bump the index to look at the next command line.
		nIdx++;
	}

	return hReturn;

} // ModifyVirusDefsVersion

/////////////////////////////////////////////////////////////////////////////
// SetSequenceNumberOnVirusDefsCommandLines -
//		This function will set the sequence number for all of the virus definitions'
// command lines.
/*
HRESULT CNavCommandLines::SetSequenceNumberOnVirusDefsCommandLines(DWORD dwSequence)
{
	// Loop through the command lines, looking for ones that have their Group Together
	// flag set.  These command lines are the virus definitions.  Set the sequence
	// number of these command lines.
	int nIdx = 1;
	TCommandLine	structCmdLine;
	IluProductRegPtr ptrProductReg;

	HRESULT hReturn = ptrProductReg.CreateInstance(__uuidof(PRODUCTREGCOMLib::luProductReg), 
									NULL, CLSCTX_INPROC);
	if (SUCCEEDED(hReturn))
	{

		while (GetNextCommandLine(nIdx++, structCmdLine))
		{
			if (structCmdLine.m_bIsGroupTogether)
			{
				// Update the sequence number on this virus definitions' command line.
				hReturn = ptrProductReg.SetSequenceNumber( structCmdLine.m_strProductName, 
  														   structCmdLine.m_strProductVersion,
														   structCmdLine.m_strProductLanguage,
														   structCmdLine.m_strProductType,
														   dwSequence );
				if (FAILED(hReturn))
					break;
			}
		}
	}

	return hReturn;
} // SetSequenceNumberOnVirusDefsCommandLines
*/

/////////////////////////////////////////////////////////////////////////////
// UnregisterCommandLines -
//		This function will remove all of NAV's command lines from LiveUpdate.
HRESULT CNavCommandLines::UnregisterCommandLines()
{
	//CoInitialize( NULL );

	HRESULT hReturn;

	try
	{
		IluProductRegPtr ptrProductReg;

		hReturn = ptrProductReg.CreateInstance(__uuidof(PRODUCTREGCOMLib::luProductReg), 
												NULL, CLSCTX_INPROC);
		if (SUCCEEDED(hReturn))
		{
			// First we will remove our overriden group and our coupled group.  The
			// overriden group make sure that anyone asking for virus defs will use 
			// our callbacks instead of their own.  The coupled group makes ensures
			// that all of the virus definitions' command lines are selected and 
			// deselected as a group.	
			IluGroupPtr ptrGroup;
            hReturn = ptrGroup.CreateInstance(__uuidof( PRODUCTREGCOMLib::luGroup ), NULL, CLSCTX_INPROC);

			// If CreateInstance() failed, LU was uninstalled before NAV, so
			// bail out.
			if (FAILED(hReturn))
				throw hReturn;

			// Delete the coupled group.
			hReturn = ptrGroup->DeleteGroup(_bstr_t(FULL_DEFS_GUID));

			// Delete the overriden group.
			hReturn = ptrGroup->DeleteGroup(_bstr_t(OVERRIDE_GUID));

			// Now we need to iterate through each command line and remove it from
			// LiveUpdate.
			int nIdx = 1;
			TCommandLine structCmdLine;

			while (GetNextCommandLine(nIdx++, structCmdLine))
			{
				// Delete the command line from LiveUpdate.
				ptrProductReg->DeleteProduct(_bstr_t(structCmdLine.m_strProductMoniker));
			}

		}
	}
	catch(HRESULT hr)
	{
//		ATLASSERT(FALSE);
		hReturn = hr;
	}
	catch(...)
	{
//		ATLASSERT(FALSE);
		if (SUCCEEDED(hReturn))
			hReturn = E_FAIL;
	}

//	CoUninitialize();

	return hReturn;
} // UnregisterCommandLines

/////////////////////////////////////////////////////////////////////////////
// GetNextCommandLine -
//		This function will get the next NAV command line.  The master version of 
// all of NAV's command lines are stored in the registry.  These command lines
// are used to update LiveUpdate's command lines.  The command lines are referenced
// by unique assending numbers, starting with 1.
BOOL CNavCommandLines::GetNextCommandLine( DWORD dwCmdLineNumber, TCommandLine &structCmdLine )
{
	BOOL bReturn = FALSE;

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
        RegQueryValueEx(hKey, cstrProductNameKey, 0, NULL, (BYTE*)szValue, &dwSize);
		structCmdLine.m_strProductName = szValue;

		dwSize = MAX_PATH;
        RegQueryValueEx(hKey, cstrProductVersionKey, 0, NULL, (BYTE*)szValue, &dwSize);
		structCmdLine.m_strProductVersion = szValue;

		dwSize = MAX_PATH;
        RegQueryValueEx(hKey, cstrLanguageKey, 0, NULL, (BYTE*)szValue, &dwSize);
		structCmdLine.m_strProductLanguage = szValue;

		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, cstrProductTypeKey, 0, NULL, (BYTE*)szValue, &dwSize);
        structCmdLine.m_strProductType = szValue;

		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, cstrProductMonikerKey, 0, NULL, (BYTE*)szValue, &dwSize);
        structCmdLine.m_strProductMoniker = szValue;

		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, cstrOtherSwitchesKey, 0, NULL, (BYTE*)szValue, &dwSize);
        structCmdLine.m_strOtherSwitches = szValue;

		// Convert the group and Nav callback flags into booleans.
		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, cstrIsNavCallbackKey, 0, NULL, (BYTE*)szValue, &dwSize);
		structCmdLine.m_bIsNAVCalback = static_cast<BOOL>( _ttol( szValue ) );
		
		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, cstrAreGroupedKey, 0, NULL, (BYTE*)szValue, &dwSize);
		structCmdLine.m_bIsGroupTogether = static_cast<BOOL>( _ttol( szValue ) );
		
		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, cstrSetSKUProperty, 0, NULL, (BYTE*)szValue, &dwSize);
		structCmdLine.m_bSetSKUProperty = static_cast<BOOL>( _ttol( szValue ) );
		
		dwSize = MAX_PATH;
		RegQueryValueEx(hKey, cstrSetEnvironment, 0, NULL, (BYTE*)szValue, &dwSize);
		structCmdLine.m_bSetEnvironment = static_cast<BOOL>( _ttol( szValue ) );

		RegCloseKey(hKey);

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
			// will need to register this command line with LiveUpdate.  However,
			// there still is one thing for us to check before we register this command
			// line; We need to see the moniker of this command line is already registered
			// with LiveUpdate.  If the moniker is not in use, when have to register the
			// moniker with LiveUpdate before registering all of the other information.
			try
			{
				_variant_t vValue;
				// We don't want to abort everything because of this.
				HRESULT hr = ptrProductReg->GetProperty( _bstr_t( structCmdLine.m_strProductMoniker ),
														_bstr_t(LUPR_PRODUCT), &vValue );

				if ( hr == S_OK )
				{
					// There's a command line already registered with LiveUpdate that
					// has the same moniker as the current command line but has
					// a different product name.  We are about to overwrite a 
					// command line that we probably shouldn't be...
					
					// I'm commenting this out because we'll get this assertion
					// every time we change the version of the MicroDefs command line
					// we do this once every month and right after installation.
					//ASSERT(FALSE);
				}
			}
			catch(...)
			{
				// Don't put an ASSERT here.  GetProperty may throw an exception if the
				// moniker isn't present.  This is OK because the moniker isn't supposed
				// to be there.
			}

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
		}
		else
		{
			// The command line is already registered with LiveUpdate.  We need to
			// update the LiveUpdate command line with the information in the given
			// command line.  The only thing we have to watch for is if the moniker
			// of the given command line is different than LiveUpdate's command line.
			//		If the monikers are different, we have to use the moniker that is 
			// current registered with LiveUpdate.  We have to do this because 
			// LiveUpdate is tracking the update status of the command line based on
			// the moniker.  Yet, we will eventually unregister this command line from
			// LiveUpdate based on the moniker in the registry.  That means, if the
			// monikers are different, we will have to update the moniker in the
			// registry.
			if ( _bstr_t( structCmdLine.m_strProductMoniker ) != _bstr_t( vMoniker ) )
			{
				// The registry moniker does not match the moniker that is register
				// with LiveUpdate.  Update the registry's moniker.
				structCmdLine.m_strProductMoniker = (LPCTSTR)_bstr_t( vMoniker );
				SetCommandLine( dwCommandLineNum, structCmdLine );
			}

			// Update the LiveUpdate command line with the data in the registry
			// command line.
			hReturn = RegisterCommandLine( structCmdLine);
		}

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
				DWORD dwSize = MAX_PATH;
				InstallToolBox::CInstoptsDat Instopts;
				Instopts.ReadString("PAIDSKU", szSKU, dwSize);

				ptrProductReg->SetProperty(_bstr_t(structCmdLine.m_strProductMoniker),
					_bstr_t( "SKU" ),
					_bstr_t( szSKU ) );
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
                    g_Log.LogEx("Error attempting to set the ENVIRONMENT setting for LiveUpdate. Error: %s", ex.what());
	            }
                catch(...)
                {
                    g_Log.Log("Unknown error attempting to set the ENVIRONMENT setting for LiveUpdate.");
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
		strDescriptiveText.LoadString(IDS_DESC_NAV_PRODUCT_UPDATE);
		IsAutoUpdateSafe = TRUE;
		bIsSilentMode = FALSE;
		
		return;
	}
	if ( strProductName.Find("Avenge") >= 0 )
	{
		strDescriptiveText.LoadString(IDS_DESC_VIRUS_DEFINITION);
		IsAutoUpdateSafe = TRUE;
        bIsSilentMode = TRUE;
		return;
	}

	if (strProductName.Find("Scan") >= 0) 
	{		
		strDescriptiveText.LoadString(IDS_DESC_SCAN_DELIVER);
		IsAutoUpdateSafe = FALSE;
        bIsSilentMode = FALSE;
		return;
	}

   	strDescriptiveText = strProductName;
	IsAutoUpdateSafe = FALSE;
    bIsSilentMode = FALSE;
} // GetCommandLineGeneratedData
