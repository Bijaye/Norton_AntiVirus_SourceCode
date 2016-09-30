#include "StdAfx.h"

#include "LiveUpdateAction.h"
#include "FileAction.h"
#include "Utilities.h"
#include "ProductRegCOMNames.h"

using namespace InstallToolBox;

class AVRES_OPTRF_LUReg : public CLiveUpdateAction
{
public:
	AVRES_OPTRF_LUReg(MSIHANDLE hInstall, LPCTSTR szActionName, LPCTSTR szCondition = NULL, LPCTSTR szUninstallCondition = NULL) : 
	  CLiveUpdateAction(hInstall, szActionName, szCondition, szUninstallCondition) {};
	virtual ~AVRES_OPTRF_LUReg() {};

protected:

	BOOL AVRES_OPTRF_LUReg::Setup()
	{
		// this function gets called during Immediate execution, so we can get the property we setted
		// earlier, and use SetProperty to put in the property so that we will have access to
		// it during the deferred execution by calling GetProperty
		TCHAR szAVRESType[MAX_PATH] = {0};
		TCHAR szNAVOPTRFType[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;

		dwSize = sizeof(szAVRESType);
		if(ERROR_SUCCESS == MsiGetProperty(m_hInstall, _T("AVRESTYPE"), szAVRESType, &dwSize))
			SetProperty(_T("AVRESTYPE"), szAVRESType);

		dwSize = sizeof(szNAVOPTRFType);
		if(ERROR_SUCCESS == MsiGetProperty(m_hInstall, _T("NAVOPTRFTYPE"), szNAVOPTRFType, &dwSize))
			SetProperty(_T("NAVOPTRFTYPE"), szNAVOPTRFType);

		return TRUE;
	}

	BOOL AVRES_OPTRF_LUReg::Init(BOOL &bKillInstall)
	{
		ATL::CString sLang, sLUAVRES, sLUNAVOPTRF;
		sLang.LoadString(IDS_LU_LANGUAGE);
		sLUAVRES.LoadString(IDS_LU_AVRES);
		sLUNAVOPTRF.LoadString(IDS_LU_NAVOPTRF);

		// this function only gets called during deferred action
		bKillInstall = FALSE;
		
		TCHAR szAVRESType[MAX_PATH] = {0};
		TCHAR szNAVOPTRFType[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;

        // wait for 10 seconds.
        m_liveUpdate.CheckForLUMutex();

		// add the commandlines by looking at the property we setted in Setup()
		dwSize = sizeof(szAVRESType);
		if(GetProperty(_T("AVRESTYPE"), szAVRESType, dwSize))
			m_liveUpdate.AddProduct("{FCF0A267-B6BD-46b0-9E7A-1090844983E3}", szAVRESType, "NAV 2006", sLang, sLUAVRES);

		dwSize = sizeof(szNAVOPTRFType);
		if(GetProperty(_T("NAVOPTRFTYPE"), szNAVOPTRFType, dwSize))
			m_liveUpdate.AddProduct("{CFEA5CA0-B0FE-4055-89AB-5D372E9E9184}", szNAVOPTRFType, "NAV 2006", sLang, sLUNAVOPTRF);
        
        // wait for 10 seconds.
        m_liveUpdate.CheckForLUMutex();

		return TRUE;
	}
};

extern "C" __declspec(dllexport) UINT __stdcall AVRES_OPTRF_LiveUpdate(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug AVRES_OPTRF_LiveUpdate()", "Debug Break", NULL);
#endif

	HRESULT hr = E_FAIL;
 
	try
	{
		hr = CoInitialize(NULL);

		if(SUCCEEDED(hr))
		{
			AVRES_OPTRF_LUReg actionLU(hInstall, "AVRES_OPTRF_LiveUpdate");

			CCustomAction* actionArray[] = {&actionLU};
			int nActionCount = sizeof(actionArray) / sizeof(*actionArray);

			CCustomAction::BaseAction(hInstall, actionArray, nActionCount, "AVRES_OPTRF_LiveUpdate");
		}
	}
	catch(...)
	{
		g_Log.Log("Unknown error attempting to register AVRES and NAVOPRF with LiveUpdate.");
	}


	if(SUCCEEDED(hr))
		CoUninitialize();

	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) UINT _stdcall AVRES_OPTRF_Type(MSIHANDLE hInstall)
{   
    TCHAR szSourceDir[MAX_PATH] = {0};
	TCHAR szDatPath[MAX_PATH]= {0};
	TCHAR szAVRESType[MAX_PATH] = {0};
	TCHAR szNAVOPTRFType[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH;

	try
	{
		CInstoptsDat Instopts;

		//read AVRESTYPE and set it in the MSI
        Instopts.ReadString(_T("INSTOPTS:AVRESTYPE"), szAVRESType, dwSize);
		MsiSetProperty(hInstall, _T("AVRESTYPE") ,szAVRESType);

		//read NAVOPTRF and set it in the MSI
		Instopts.ReadString(_T("INSTOPTS:NAVOPTRFTYPE"), szNAVOPTRFType, dwSize);
		MsiSetProperty(hInstall, _T("NAVOPTRFTYPE"), szNAVOPTRFType);
	}
	catch(...)
	{
		//catch all exceptions here
	}

	return ERROR_SUCCESS;
}