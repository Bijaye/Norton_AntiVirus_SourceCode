#include "stdafx.h"
#include "NAVPatchInst.h"
#include "build.h"
#include "AVRESBranding.h"

const LPCTSTR g_cszProductName = _T("Norton AntiVirus 2006");
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
	CBrandingRes BrandRes;
	CString csProductName = BrandRes.ProductNameWithVersion();

	if(csProductName.GetLength() > 0)
	{
		_tcscpy(pszProductName, csProductName);
		dwSize = csProductName.GetLength();
	}

	return true;		
}
