// NAVOptionRefresh.h: interface for the CNAVOptionRefresh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVOPTIONREFRESH_H__DFDC798E_5BCD_4503_A258_CBBE92BAC6E3__INCLUDED_)
#define AFX_NAVOPTIONREFRESH_H__DFDC798E_5BCD_4503_A258_CBBE92BAC6E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <simon.h>
#include "NavOptionRefreshHelperInterface.h"
#include "cltLicenseHelper.h"
#include "NavSettings.h"

#define REFRESH_DEFAULT (REFRESH_COMMIT_ON_CHANGE|REFRESH_EVENT_CHANGE|REFRESH_UPDATE_AP)

class CNAVOptionRefresh:
	public SIMON::CSimonClass<CNAVOptionRefresh,&CLSID_CNAVOptionRefresh>  
	,public SIMON::IInterfaceManagementImpl<false>
	,public INAVOptionRefresh  
{
public:
	CNAVOptionRefresh();
	virtual ~CNAVOptionRefresh();
	SIMON_INTERFACE_MAP_BEGIN()
		SIMON_INTERFACE_ENTRY( IID_INAVOptionRefresh ,INAVOptionRefresh  )
	SIMON_INTERFACE_MAP_END()

	SIMON_STDMETHOD(Refresh)(DWORD);

private:
	HRESULT _IT_DoRefresh(DWORD flag = REFRESH_DEFAULT);             //_IT_ == INSIDE THREAD
	HRESULT _EnableAutoProtect(BOOL bFeatureEnabled);
	HRESULT _EnableAutoLiveUpdate(BOOL bEnableALUProduct, BOOL bEnableALUVirusDefs);
	HRESULT _EnableIWP(BOOL bFeatureEnabled);
	HRESULT _IT_FireTamperEvent(UINT uErrorID);
	HRESULT _IT_ErrorMessageBox(HWND hWnd, LPCTSTR lpszText, LPCTSTR lpszCaption, UINT uErrorID);

	// cached data
	DJSMAR00_LicenseState m_cachedLicenseState;
	long  m_cachedLicenseZone;
	DWORD m_cachedSubState;
    LONG  m_cachedSubDays;

	NAVToolbox::CCSettings m_ccSettings;
};

#endif // !defined(AFX_NAVOPTIONREFRESH_H__DFDC798E_5BCD_4503_A258_CBBE92BAC6E3__INCLUDED_)
