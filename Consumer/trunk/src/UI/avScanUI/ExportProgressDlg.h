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
#include <resource.h>

class CExportProgressDlg :
    public symhtml::CSymHTMLDocumentImpl,
    public symhtml::CSymHTMLDialogDocumentImpl,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{

public:
    DECLARE_HTML_DOCUMENT(L"EXPORTPROGRESS.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CExportProgressDlg)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CExportProgressDlg)
        HTML_EVENT_BUTTON_CLICK(L"cancel", OnCancel)
    END_HTML_EVENT_MAP()

public:
    CExportProgressDlg(void) :
        m_bCanceled(false),
        m_hIcon(NULL)
    {
    }
    virtual ~CExportProgressDlg(void)
    {
        CCTRCTXI0(L"dtor");
        if(m_hIcon)
            DestroyIcon(m_hIcon);
    }

    // Control methods
    void SetProgress(int nPct)
    {
        if(S_FALSE == IsDocumentComplete())
            return;


        symhtml::ISymHTMLElementPtr spProg;
        HRESULT hr = GetElement("TPprogress", &spProg);
        LOG_FAILURE(L"Failed to get TPprogress element", hr);
        if(spProg)
        {
            CStringW sValue;
            sValue.Format(L"%d", nPct);
            spProg->SetAttributeByName("value", sValue);
            spProg->UpdateElement(TRUE);
        }
    }

    bool GetCanceled()
    {
        return m_bCanceled;
    }

    // Event handlers
    HRESULT OnCancel(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        m_bCanceled = true;
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
        sTitle.LoadString(IDS_TITLE_EXPORTPROG);
        hr = SetWindowText(sTitle);
        
        // Load our icons
        CISVersion::GetProductIcon(m_hIcon);
        hr = SetIcon(m_hIcon, false);

        return hr;
    }

protected:
    bool m_bCanceled;
    HICON m_hIcon;
};
