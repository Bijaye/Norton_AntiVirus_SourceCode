////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SymHTML.h>
#include <SymHTMLDocumentImpl.h>
#include <SymHTMLDialogDocumentImpl.h>
#include <AVInterfaces.h>
#include "ThreatTracker.h"
#include <resource.h>
#include "TabAttentionDocument.h"
#include <SymHelpLauncher.h>
#include "SymHelp.h"

class CEMailThreatDlg :
    public symhtml::CSymHTMLDocumentImpl,
    public symhtml::CSymHTMLDialogDocumentImpl,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{

public:
    DECLARE_HTML_DOCUMENT(L"EMAIL-THREAT.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CEMailThreatDlg)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CEMailThreatDlg)
        HTML_EVENT_BUTTON_CLICK(L"apply", OnApply)
        HTML_EVENT_BUTTON_CLICK(L"stop", OnStop)
        HTML_EVENT_HYPERLINK(L"helpLink", OnHelp)
    END_HTML_EVENT_MAP()

public:
    CEMailThreatDlg(CThreatTracker* pThreatTracker, LPCWSTR pszSubject, LPCWSTR pszSender, LPCWSTR pszRecipient) :
        m_sSubject(pszSubject),
        m_sSender(pszSender),
        m_sRecipient(pszRecipient)
    {
        m_spThreatTracker = pThreatTracker;
    }
    virtual ~CEMailThreatDlg(void)
    {
        CCTRCTXI0(L"dtor");
    }

    // Event handlers
    HRESULT OnApply(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDYES);
        return S_OK;
    }

    HRESULT OnStop(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDNO);
        return S_OK;
    }

    HRESULT OnHelp(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        HWND hWnd;
        HRESULT hr = GetHwnd(&hWnd);
        if(SUCCEEDED(hr))
        {
            isshared::CHelpLauncher Help;
            HWND hWndHelp = Help.LaunchHelp(NAVW_CSH_EMAIL_RISK_DETECTED, hWnd);
            if(!hWndHelp)
                hr = E_FAIL;
        }

        return hr;
    }

    virtual HRESULT OnDocumentComplete() throw()
    {
        // Call the base OnDocumentComplete
        HRESULT hr = symhtml::CSymHTMLDocumentImpl::OnDocumentComplete();
        if(FAILED(hr))
            return hr;

        // Get the product title
        CString sBrandingProductName = CISVersion::GetProductName();

        // Get/Set our scan title
        CStringW sTitle;
        sTitle.LoadString(IDS_TITLE_EMAIL);
        SetWindowText(sBrandingProductName + L" - " + sTitle);

        // Fill in the email info
        SetElementText("EMailSubject", m_sSubject);
        SetElementText("EMailFrom", m_sSender);
        SetElementText("EMailTo", m_sRecipient);

        // Create the attention tab
        m_spDocAttentionTab = new CTabAttentionDocument();
        if(!m_spDocAttentionTab)
        {
            LOG_FAILURE_AND_RETURN(L"Failed to new CTabAttentionDocument object", E_OUTOFMEMORY);
        }

        // Setup the attention tab
        m_spDocAttentionTab->SetEMailMode(true);
        m_spDocAttentionTab->SetThreatTracker(m_spThreatTracker);

        // Render the attention tab
        m_spSymHtmlWndActiveTab->Render(m_spDocAttentionTab);

        return hr;
    }

    virtual HRESULT OnNewIFrame(symhtml::ISymHTMLElement* lpElement, symhtml::ISymHTMLWindow* lpNewFrame) throw()
    {
        m_spSymHtmlWndActiveTab = lpNewFrame;
        return S_OK;
    }

protected:
    // No protected methods

protected:
    CSymPtr<CThreatTracker> m_spThreatTracker;
    symhtml::ISymHTMLWindowPtr m_spSymHtmlWndActiveTab;
    CStringW m_sSubject;
    CStringW m_sSender;
    CStringW m_sRecipient;

    // Tab Documents
    CSymPtr<CTabAttentionDocument> m_spDocAttentionTab;
};
