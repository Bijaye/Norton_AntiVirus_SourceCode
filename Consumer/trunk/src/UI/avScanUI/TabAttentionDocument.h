////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//*****************************************************************************
#pragma once
#include "GridBaseDocument.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CTabAttentionDocument :
    public CGridBaseDocument
{
public:

    DECLARE_HTML_DOCUMENT(L"TABATTENTION.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CTabAttentionDocument)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CTabAttentionDocument)
        HTML_EVENT_TABLE_HEADER_CLICK(L"thrisk", OnHeaderClick)
        HTML_EVENT_TABLE_HEADER_CLICK(L"thtitle", OnHeaderClick)
        HTML_EVENT_TABLE_HEADER_CLICK(L"thstatus", OnHeaderClick)
        HTML_EVENT_TABLE_HEADER_CLICK(L"thaction", OnHeaderClick)
        HTML_EVENT_HYPERLINK_CLICK(L"sechist", OnSecHistClick)
    END_HTML_EVENT_MAP()

public:
    CTabAttentionDocument() :
        CGridBaseDocument(),
        m_bEMailMode(false)
    {
    }

    virtual ~CTabAttentionDocument()
    {
        CCTRCTXI0(L"dtor");
    }

    // Events
    virtual HRESULT OnHtmlEvent(symhtml::ISymHTMLElement* lpElement, LPCWSTR szEventName, UINT nEventType, ISymBase* pExtra) throw()
    {
        HRESULT hr = S_FALSE;

        // Is this an "apply" button click?
        if(nEventType == symhtml::event::BUTTON_CLICK && szEventName && wcsnicmp(szEventName, L"apply", 5) == 0)
        {
            // Get the index
            size_t iIndex = _wtol(szEventName + 5);

            // Apply the selected action
            ApplyAction(iIndex);

            // Don't call the default handler
            return S_OK;
        }
        // Is this an action dropdown change?
        else if(nEventType == symhtml::event::SELECT_SELECTION_CHANGED && szEventName && wcsnicmp(szEventName, L"ACT", 3) == 0)
        {
            // Get the index
            size_t iIndex = _wtol(szEventName + 3);

            // Get the current selection
            int nCurSel = 0;
            hr = GetCBSelection(lpElement, nCurSel);
            if(SUCCEEDED(hr))
            {
                // Build the element name
                CStringA sApplyBtn;
                sApplyBtn.Format("apply%d", iIndex);

                // Figure out the current selection
                bool bEnabled = true;
                if(nCurSel == CThreat::eIntAct_None)
                    bEnabled = false;

                // Update the threat tracker with the current selection
                m_spThreatTracker->SetThreatAction(iIndex, (CThreat::eIntThreatAction)nCurSel);

                // Apply button doesn't exist in email mode
                if(!m_bEMailMode)
                {
                    SetElementEnabled(sApplyBtn, bEnabled, TRUE);
                }
            }

            // Don't call the default handler
            return S_OK;
        }
        else if(nEventType == symhtml::event::HYPERLINK_CLICK && szEventName)
        {
            // Get the root element
            HWND hWnd;
            GetDocHwnd(hWnd);

            if(wcsnicmp(szEventName, L"VDETAIL", 7) == 0)
            {
                // Get the index
                size_t iIndex = _wtol(szEventName + 7);

                // Open the details page for this item
                hr = m_spThreatTracker->DisplayThreatDetails(hWnd, iIndex);

                // Don't call the default handler
                return S_OK;
            }
            else if(wcsnicmp(szEventName, L"VINFO", 5) == 0)
            {
                // Get the index
                size_t iIndex = _wtol(szEventName + 5);

                // Open the details->info page for this item
                hr = m_spThreatTracker->DisplayThreatDetails(hWnd, iIndex, false);

                // Don't call the default handler
                return S_OK;
            }
        }

        // Call the base handler
        return symhtml::CSymHTMLDocumentImpl::OnHtmlEvent(lpElement, szEventName, nEventType, pExtra);
    }

    virtual HRESULT CreateComparatorForColumn(UINT nColumn, bool bAsc, symhtml::ISymHTMLComparator*& pComparator)
    {
        // Create the alpha comparator... it is the secondary comparator for all
        // column sorts, except the description ... where it is the primary
        symhtml::ISymHTMLComparatorPtr spSecondaryComparator = new CAlphaComparator(1, bAsc);
        if(!spSecondaryComparator)
        {
            LOG_FAILURE_AND_RETURN(L"Failed to create secondary comparator!", E_FAIL);
        }

        symhtml::ISymHTMLComparatorPtr spCompTmp;
        switch(nColumn)
        {
            case 0:     // Risk Level
            {
                spCompTmp = new CHMLComparator(nColumn, bAsc, spSecondaryComparator);
                break;
            }
            case 1:     // Description
            {
                spCompTmp = spSecondaryComparator;
                break;
            }
            case 2:     // Status
            {
                spCompTmp = new CAlphaComparator(nColumn, bAsc, spSecondaryComparator);
                break;
            }
            case 3:     // Dropdown - Sort by selection?...
            {
                spCompTmp = new CDropdownComparator(nColumn, bAsc, spSecondaryComparator);
                break;
            }
            default:
            {
                LOG_FAILURE_AND_RETURN(L"Unexpected column index", E_INVALIDARG);
                break;
            }
        }

        if(!spCompTmp)
        {
            LOG_FAILURE_AND_RETURN(L"Failed to create comparator", E_OUTOFMEMORY);
        }

        // Free the callers input
        if(pComparator)
        {
            pComparator->Release();
            pComparator = NULL;
        }
    
        // Give our ref to the caller
        pComparator = spCompTmp.Detach();

        // AddRef and return success
        return S_OK;
    }

    // Control methods
    void SetEMailMode(bool bEMailMode)
    {
        m_bEMailMode = bEMailMode;
    }

    void ApplyAction(size_t iIndex)
    {
        HWND hWnd;
        GetDocHwnd(hWnd);
        if(m_spThreatTracker)
            m_spThreatTracker->TakeThreatAction(iIndex, hWnd);
    }

protected:
    virtual CThreat::eTTState GetListType()
    {
        return CThreat::eTTState_RequireAttention;
    }

    virtual UINT GetTHTemplateID()
    {
        return IDS_ATTN_TH;
    }

    virtual UINT GetRowTemplateID()
    {
        if(m_bEMailMode)
            return IDS_ATTN_TEMPLATE_EMAIL;

        return IDS_ATTN_TEMPLATE;
    }

    virtual UINT GetEmptyStringID()
    {
        return IDS_ATTN_TEMPLATE_NONE;
    }

    virtual HRESULT GetCBSelection(symhtml::ISymHTMLElement* lpElement, int& nValue)
    {
        CCTRACEI(CCTRCTX L"Enter");
        HRESULT hr = S_OK;

        if(!lpElement)
        {
            CCTRACEE(CCTRCTX L"Element pointer NULL");
            return E_POINTER;
        }

        cc::IStringPtr pString;

        symhtml::ISymHTMLElementPtr pChildElement;
        hr = lpElement->FindFirstElement("caption", &pChildElement);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX L"Could not get caption element");
            return hr;
        }

        hr = pChildElement->GetElementInnerText(&pString);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX L"Could not get caption text");
            return hr;
        }

        hr = lpElement->FindFirstElement("option:checked", &pChildElement);
        if(FAILED(hr) || S_FALSE == hr)
        {
            CCTRACEE(CCTRCTX L"Could not get option element");
            return hr;
        }

        LPCWSTR szValue = NULL;
        hr = pChildElement->GetAttributeByName("value", &szValue);
        if(FAILED(hr) || S_FALSE == hr)
        {
            CCTRACEE(CCTRCTX L"Could not get value attribute");
            return hr;
        }

        nValue = _wtoi(szValue);

        return hr;
    }

protected:
    bool m_bEMailMode;
};

