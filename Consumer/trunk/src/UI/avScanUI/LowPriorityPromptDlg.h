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

class CLowPriorityPromptDlg :
    public symhtml::CSymHTMLDocumentImpl,
    public symhtml::CSymHTMLDialogDocumentImpl,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{

public:
    DECLARE_HTML_DOCUMENT(L"LOWPRIORITYPROMPT.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CLowPriorityPromptDlg)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CLowPriorityPromptDlg)
        HTML_EVENT_BUTTON_CLICK(L"yes", OnYes)
        HTML_EVENT_BUTTON_CLICK(L"no", OnNo)
        HTML_EVENT_BUTTON_STATE_CHANGE(L"remember", OnRemember)
    END_HTML_EVENT_MAP()

public:
    CLowPriorityPromptDlg(void) :
        m_bRemember(false),
        m_hIcon(NULL)
    {
    }
    virtual ~CLowPriorityPromptDlg(void)
    {
        CCTRCTXI0(L"dtor");
        if(m_hIcon)
            DestroyIcon(m_hIcon);
    }

    bool GetRemember()
    {
        return m_bRemember;
    }

    // Event handlers
    HRESULT OnYes(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDYES);
        return S_OK;
    }

    HRESULT OnNo(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDNO);
        return S_OK;
    }

    HRESULT OnRemember(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        m_bRemember = !m_bRemember;
        return S_OK;
    }

    virtual HRESULT OnDocumentComplete() throw()
    {
        // Call the base OnDocumentComplete
        HRESULT hr = symhtml::CSymHTMLDocumentImpl::OnDocumentComplete();
        if(FAILED(hr))
            return hr;

        // Get/Set our title
        CStringW sTitle;
        sTitle.LoadString(IDS_TITLE_LOWPRIORITY);
        hr = SetWindowText(sTitle);
        
        // Load our icons
        CISVersion::GetProductIcon(m_hIcon);
        hr = SetIcon(m_hIcon, false);

        return hr;
    }

protected:
    bool m_bRemember;
    HICON m_hIcon;
};
