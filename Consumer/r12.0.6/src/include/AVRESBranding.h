// AVRESBranding.h

#pragma once

#include "StahlSoft.h"
#include "AVRES.h"
#include "ccSymInstalledApps.h"

class CBrandingRes
{
public:
	CBrandingRes() {};
	~CBrandingRes() {};

	CString ProductName()
	{
		return GetString(IDS_AV_PRODUCT_NAME);
	}    

	CString ProductNameWithVersion()
	{
		return GetString(IDS_AV_PRODUCT_NAME_VERSION);
	}   
    
	CString GetString(UINT uResourceID)
	{
		CString csBuffer;

		if(InitAVRes())
		{
			csBuffer.LoadString(m_shMod, uResourceID);
		}

		return csBuffer;
	}

private:
	BOOL InitAVRes()
	{
		if((HANDLE) m_shMod == (HANDLE) NULL)
		{
			ccLib::CString csAVRES;

			if(ccSym::CInstalledApps::GetNAVDirectory(csAVRES))
			{
				csAVRES += _T("\\AVRES.DLL");

				m_shMod = LoadLibrary(csAVRES);

				if((HANDLE) m_shMod == (HANDLE) NULL)
				{
					return FALSE;
				}
			}
		}

		return TRUE;
	}

private:
	StahlSoft::CSmartModuleHandle m_shMod;
};
