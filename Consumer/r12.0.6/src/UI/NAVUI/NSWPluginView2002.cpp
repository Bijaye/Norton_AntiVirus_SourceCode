// NSWPluginView2002.cpp : Implementation of CNSWPluginView2002
#include "stdafx.h"
#include "NAVUI.h"
#include "NSWPluginView2002.h"
#import "NAVUI.tlb"
#include "OEMProductIntegrationDefines.h"
/////////////////////////////////////////////////////////////////////////////
// CNSWPluginView2002

HRESULT CNSWPluginView2002::FinalConstruct()
{
	HRESULT hr = S_OK;
    StahlSoft::HRX hrx;
	try
	{
		DWORD dwRet = 0;

        hrx << CBrandingImpl::Initialize();
        hrx << CBrandingImpl::GetValue(OEMPQV_UniqueOEMID,&dwRet);
		if(dwRet != OEMUUID_Symantec_Retail)
		{
			hrx << CO_E_CLASS_CREATE_FAILED;	// we got instantiate by someone who is legacy
												// Legacy == Retail
												// AND we are NOT Retail, hence FAIL
		}
    
        m_pInner.CoCreateInstance(__uuidof(NAVUILib::NSWPluginView),GetUnknown());
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}
	catch(...)
	{
		hr = E_UNEXPECTED;
	}
	return hr;
}
