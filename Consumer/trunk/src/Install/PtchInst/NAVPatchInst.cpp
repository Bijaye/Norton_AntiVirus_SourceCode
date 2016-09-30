////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"
#include "NAVPatchInst.h"
#include "build.h"
#include "ISVersion.h"
#include "FileAction.h"

//cc
#include "ccSplitPath.h"
#include "ccSymInstalledApps.h"

const LPCTSTR g_cszProductName = _T("Norton AntiVirus 2007");
const LPCTSTR g_cszProductID = _T("NAV");
const InstallToolBox::ITB_VERSION g_citbVersion = { sizeof(InstallToolBox::ITB_VERSION), 
													VER_NUM_PRODVERMAJOR,
													VER_NUM_PRODVERMINOR,
													VER_NUM_PRODVERSUBMINOR,
													VER_NUM_BUILDNUMBER, 0, 4};

/////////////////////////////////////////////////////////
// CPatchInstImpl::GetFullProductName() 
//	get product name from AVRES.DLL
// 
bool CNAVPatchInst::GetFullProductName(LPSTR pszProductName, DWORD& dwSize)
{
	CStringA csProductName = CISVersion::GetProductName();

	if(csProductName.GetLength() > 0)
	{
		strcpy(pszProductName, csProductName);
		dwSize = csProductName.GetLength();
	}

	return true;
}

//Unregister the NSW One Button Check COM object that talks to NAV
//NAV 2007 cannot interface with NSW 2006's COM object - NAVPreC.dll
void CNAVPatchInst::UnRegisterNSW_NAVPreC()
{
	CString sRegSvr32Path;
	//c:\windows\system32\regsve32.exe
	if(ccLib::CSplitPath::GetSystemDirectory(sRegSvr32Path))
	{
		sRegSvr32Path += _T("\\regsvr32.exe");

		//"c:\program files\common files\symantec shared\navprec.dll"
		ccLib::CString sCCDirectory;
		if(ccSym::CInstalledApps::GetCCDirectory(sCCDirectory))
		{
			sCCDirectory.Append(_T("\\navprec.dll\""));
			sCCDirectory.Insert(0,_T("\""));

			CString sParms = _T("/u /s ");
			sParms += sCCDirectory;

			HRESULT hr = InstallToolBox::ExecuteProgram(sRegSvr32Path, sParms,  FALSE);
			if(FAILED(hr))
			{
				CCTRACEE(CCTRCTX _T("ExecuteProgram( %s %s) Failed. HR = 0x%x"),sRegSvr32Path,sParms,hr);
			}
		}
	}
}

bool CNAVPatchInst::PostInstall(LPCSTR pcszPatchInstID, const InstallToolBox::LP_ITB_VERSION lpitbVersion)
{
	//Check for Trust
	static bool bTrustVerified = false;

	if(!bTrustVerified) 
	{
		CSymDLL_ReverseTrustCheck trustcheck;
		HRESULT hrTrust = trustcheck.DoCheck();	// check the calling process
		if (hrTrust == S_OK) 
		{
			bTrustVerified = true;
		}
		else
		{
			return false;	// the trustcheck failed
		}
	}
	//if NSW?
	// so that we can use CString's compare
	CString sPatchInstID = pcszPatchInstID;
	if(sPatchInstID == "NSW")
	{
		UnRegisterNSW_NAVPreC();
	}
}