// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//Copyright (c) 2006 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include "SavAssert.h"

#include "VirusFoundProxy.h"


const CLSID CLSID_VirusFoundCOMAdapter = {0xA1987DB8,0x9F0D,0x47D1,0x80,0xC9,0xDF,0xCE,0x76,0x26,0x08,0x41};
const IID	IID_IVirusFoundCOMAdapter  = {0x11D8A19E,0xB7BA,0x46EE,0xBC,0xB7,0x4A,0x8A,0x74,0x1C,0xAC,0x2F};


/** DWORD CreateDlg();
 *
 * Initializes SavUI in the correct session using session monikers
 *
**/
DWORD CVirusFoundProxy::CreateDlg()
{
    //create the com object for the notification dlg wrapper
    CSessionMoniker objSessionMoniker(m_dwSessionId);
    DWORD dwRet = objSessionMoniker.CreateMoniker(CLSID_VirusFoundCOMAdapter);
    if( SUCCEEDED(dwRet) )
    {
        dwRet = objSessionMoniker.BindToObject((void**)&m_IClassFactoryPtr);
        if( ERROR_SUCCESS == dwRet )
            dwRet = m_IClassFactoryPtr->CreateInstance(NULL,
													   IID_IVirusFoundCOMAdapter,
													   (void**)&m_pCOMAdapter);
    }

    if( m_pCOMAdapter != NULL )
    {
		CComBSTR bstrTitle(m_strTitle.c_str());

        dwRet = m_pCOMAdapter->CreateDlg(m_dwSessionId, bstrTitle);

        m_pCOMCallback = new CComObject<CVirusFoundCOMCallback>;
        if( m_pCOMCallback != NULL )
        {
            //initialize the class so the callbacks return more than just not implemented
            ((CVirusFoundCOMCallback*)m_pCOMCallback.GetInterfacePtr())->EstablishTrust();
            dwRet = m_pCOMAdapter->RegisterCallback(m_pCOMCallback);
        }
    }

    return dwRet;
}

/** Adds a message to the notification dlg hosted in SAVUI */
DWORD CVirusFoundProxy::AddMessage(LPCTSTR pcLogline, LPCTSTR pcDescription)
{
    //if we haven't created the COM object in the appropriate session
    //do so
    DWORD dwRet = ERROR_SUCCESS;

	if( m_pCOMAdapter == NULL )
	{
        dwRet = CreateDlg();
		SAVASSERT((NULL == m_pCOMAdapter) == (dwRet != ERROR_SUCCESS));
			// If the pointer is null, CreateDlg() should have returned failure.
		if( NULL == m_pCOMAdapter )
			return dwRet;
	}

    //send the dialog message
    if (m_pCOMAdapter != NULL )
    {
		CComBSTR bstrLogline	(pcLogline);
		CComBSTR bstrDescription(pcDescription);

        dwRet = m_pCOMAdapter->AddMessage(bstrLogline, bstrDescription);
    }

    return dwRet;
}

/** copy constructor 
 * 
 * needs to be here for adding to map in CDlgManager
 *
**/
CVirusFoundProxy::CVirusFoundProxy(const CVirusFoundProxy& objRhs):m_dwSessionId(objRhs.m_dwSessionId)
{
	*this = objRhs;
}

CVirusFoundProxy& CVirusFoundProxy::operator=(const CVirusFoundProxy& objRhs)
{
    DWORD* pTemp = const_cast<DWORD*>(&m_dwSessionId);
    *pTemp = objRhs.m_dwSessionId;
    m_IClassFactoryPtr = objRhs.m_IClassFactoryPtr;
    m_pCOMAdapter	   = objRhs.m_pCOMAdapter;
    m_strTitle		   = objRhs.m_strTitle;
    m_pCOMCallback	   = objRhs.m_pCOMCallback;

    return *this;
}
