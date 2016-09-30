#include "StdAfx.h"
#include ".\traynotifyui.h"
#include "resource.h"
#include "AVRESBranding.h"

CString CTrayNotifyUI::m_csProductName;

CTrayNotifyUI::CTrayNotifyUI(void)
{
	if(m_csProductName.IsEmpty())
	{
		CBrandingRes BrandRes;
		m_csProductName = BrandRes.ProductName();
	}

    // Load ccAlert
    if(SYM_FAILED(NotifyFactory.CreateObject(&m_pAlert)))
    {
        m_pAlert = NULL;
    }
    else
    {
        m_pAlert->SetTitle ( m_csProductName );
        m_pAlert->SetTimeout ( 1000*30 ); // 30 secs
    }
}

CTrayNotifyUI::~CTrayNotifyUI(void)
{
    // Release ccAlert
    m_pAlert.Release();
}

bool CTrayNotifyUI::Start (LPCSTR lpszText, bool bMoreInfo, bool bTaskbar)
{
    if ( !m_pAlert )
        return false;

    m_bUserWantsMoreInfo = false;

    if ( bMoreInfo )
    {
        if ( !m_pAlert->SetProperty ( cc::INotify::PROPERTY_MOREINFO_BUTTON, true ))
            CCTRACEE ("CTrayNotifyUI::Start - error setting More Info property");

        if ( !m_pAlert->SetButton ( cc::INotify::BUTTONTYPE_MOREINFO, g_hInstance, IDS_MORE_INFO, this ))
            CCTRACEE ("CTrayNotifyUI::Start - error setting More Info button");
    }

    if ( bTaskbar )
    {
        m_pAlert->SetTaskBarText (m_csProductName);
        m_pAlert->SetProperty ( cc::INotify::PROPERTY_TASKBAR_ENTRY, true );
    }

    m_pAlert->SetText ( lpszText );

    m_pAlert->Display (::GetDesktopWindow());

    return true;
}

bool CTrayNotifyUI::Run(HWND hWndParent, unsigned long nID, cc::INotify* pAlert)
{
    if ( !m_pAlert )
        return false;

    // Close the alert
    m_bUserWantsMoreInfo = true;
    m_pAlert->Hide ();
    return true;
}
