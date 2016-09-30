#include "StdAfx.h"

#include "SSPRunnableJob.h"

#include "ccSerializableEventHelper.h"
#include "ccSubscriberHelper.h"

#include "HPPEventsInterface.h"
#include "HPPAppInterface.h"

#include "SSPChangeRegKey.h"

CSSPChangeRegKey::CSSPChangeRegKey(void) : m_bChangeUserKey(FALSE)
{
}

CSSPChangeRegKey::~CSSPChangeRegKey(void)
{
}

void CSSPChangeRegKey::SetNewHomePage(LPCTSTR szNewHomePageValue, BOOL bChangeUserKey, DWORD dwThreadId)
{
    m_cszNewHomePageValue = szNewHomePageValue;
    m_bChangeUserKey = bChangeUserKey;
    m_dwThreadId = dwThreadId;

    return;
}

HRESULT CSSPChangeRegKey::Run()
{
	HRESULT hrReturn = S_OK;
    CCTRACEI(_T("CSSPChangeRegKey::Run() BEGIN.\r\n"));
    BOOL bWasChanged = FALSE;

	//
	//  Protect ourselves from the evil that
	//   we are capable of.
	//
	try
	{
        HRESULT hr = E_FAIL;
		hr = ChangeHomePageKey(HKEY_CURRENT_USER, m_cszNewHomePageValue); //change HKCU always, whenever this worker thread gets spawned by SwitchHomePage
        if(S_OK == hr)
        {
            bWasChanged = TRUE;
            CCTRACEI(_T("CSSPChangeRegKey::Run - %s home page key successfully changed."), m_bChangeUserKey ? _T("HKCU") : _T("HKLM"));
        }
        else if(S_FALSE == hr)
        {
            CCTRACEI(_T("CSSPChangeRegKey::Run - %s home page key did not need to be changed."), m_bChangeUserKey ? _T("HKCU") : _T("HKLM"));
        }
        else
        {
            CCTRACEE(_T("CSSPChangeRegKey::Run - %s home page key was not changed."), m_bChangeUserKey ? _T("HKCU") : _T("HKLM"));
        }
		if(!m_bChangeUserKey)   //change HKLM as well 
		{
			hr = ChangeHomePageKey(HKEY_LOCAL_MACHINE, m_cszNewHomePageValue);
			if(S_OK == hr)
			{
				CCTRACEI(_T("CSSPChangeRegKey::Run - %s home page key successfully changed."), m_bChangeUserKey ? _T("HKCU") : _T("HKLM"));
			}
			else if(S_FALSE == hr)
			{
				CCTRACEI(_T("CSSPChangeRegKey::Run - %s home page key did not need to be changed."), m_bChangeUserKey ? _T("HKCU") : _T("HKLM"));
			}
			else
			{
				CCTRACEE(_T("CSSPChangeRegKey::Run - %s home page key was not changed."), m_bChangeUserKey ? _T("HKCU") : _T("HKLM"));
			}
		}

        if(bWasChanged && NULL != m_dwThreadId)
        {
            IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
            if(spApp)
            {
                spApp->NavigateToHomePageByThread(m_dwThreadId);
            }
        }
    }
	catch(...)
	{
		CCTRACEE("CSSPChangeRegKey::Run() - Unhandled exception caught.\r\n");
	}

    CCTRACEI(_T("CSSPChangeRegKey::Run() END.\r\n"));

	return hrReturn;
}

HRESULT CSSPChangeRegKey::ChangeHomePageKey(HKEY hKey, LPCTSTR szNewHomePageValue)
{
    HRESULT hrReturn = E_FAIL;

#pragma message(AUTO_FUNCNAME "TODO: Code Review Item")
	//
	// TODO: Code Review Item
	//       Move these hard coded strings into a #define or const LPTSTR

	CString cszKeyPath = _T("Software\\Microsoft\\Internet Explorer\\main");
    CString cszKeyName = _T("Start Page");

    CRegKey rkHomePageKey;
    LONG lRet = NULL;

    lRet = rkHomePageKey.Open(hKey, cszKeyPath, KEY_READ|KEY_WRITE);
    if(ERROR_SUCCESS == lRet)
    {
        CString cszOldValue;
        DWORD dwSize = 0;

        lRet = rkHomePageKey.QueryStringValue(cszKeyName, NULL, &dwSize);
        if( (ERROR_SUCCESS != lRet) || (NULL == dwSize) )
        {
            CCTRACEE(_T("CSSPChangeRegKey::ChangeHomePageKey - Unable to get size of home page value."));
            return E_FAIL;
        }

        lRet = rkHomePageKey.QueryStringValue(cszKeyName, cszOldValue.GetBuffer(dwSize), &dwSize);
        cszOldValue.ReleaseBuffer();
        if( (ERROR_SUCCESS != lRet) || (NULL == dwSize) )
        {
            CCTRACEE(_T("CSSPChangeRegKey::ChangeHomePageKey - Unable to get home page value."));
            return E_FAIL;
        }

        if(0 != cszOldValue.CompareNoCase(szNewHomePageValue))
        {
            // overwrite the old home page with the new one.
            lRet = rkHomePageKey.SetStringValue(cszKeyName, szNewHomePageValue);
            if(ERROR_SUCCESS == lRet)            
            {
                hrReturn = S_OK;
            }
            else
            {
                CCTRACEE(_T("CSSPChangeRegKey::ChangeHomePageKey - Unable to write new home page key."));
            }
        }
        else
        {
            // do nothing.  this is the case when the user opted to allow the change
            // probably wanna log here.

            hrReturn = S_FALSE;
        }
    }
    else
    {
        CCTRACEE(_T("CSSPChangeRegKey::ChangeHomePageKey - Unable to open home page key."));
    }
    
    return hrReturn;
}