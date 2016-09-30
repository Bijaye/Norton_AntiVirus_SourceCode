#include "Stdafx.h"
#include "global.h"
#include "CommonClientInfo.h"
#include "NAVInfo.h"
#include "resource.h"

#include "ccModuleId.h"
#include "ccErrorDisplay.h"

HINSTANCE g_hInstance;

void ReportFatalError(UINT uMessageID)
{
	char szTitle[MAX_PATH];
	LoadString(g_hInstance, IDS_NAV_TITLE, szTitle, sizeof(szTitle));	

	char szMessage[MAX_PATH];
	LoadString(g_hInstance, uMessageID, szMessage, sizeof(szMessage));	
	
	HMODULE hDll = NULL;
	char szErrorDll[MAX_PATH];

	// Build the path to ccErrDsp.dll
	CCommonClientInfo CCInfo;
	if(!CCInfo.GetCCFolder(szErrorDll))
		return;
	_tcscat(szErrorDll, "\\ccErrDsp.dll");

	hDll = LoadLibrary(szErrorDll);
	if( hDll)
	{
		pfnCCDISPLAYERROR pfnCcDisplayError = NULL;
		
		pfnCcDisplayError = (pfnCCDISPLAYERROR)GetProcAddress(hDll, "ccDisplayError");
		if(pfnCcDisplayError != NULL)
			pfnCcDisplayError(NULL, AV_MODULE_ID_CHKNAVFILES, CHKNAVFILES_ERR_START, szTitle, szMessage, NULL, NULL, 0, NULL, NULL);
		
		FreeLibrary(hDll);
	}
}

bool IsNAVFileRemoved()
{

	// Get the NAV install path.
	CNAVInfo NAVInfo;
	TCHAR szNAVDir[MAX_PATH] = {0};	
	DWORD dwSize = MAX_PATH;
	
	_tcscpy(szNAVDir, NAVInfo.GetNAVDir());
	_tcscat(szNAVDir, _T("\\navw32.exe"));	

	if(GetFileAttributes(szNAVDir) == -1)
	{
		return TRUE;
	}
		
	return FALSE;
}