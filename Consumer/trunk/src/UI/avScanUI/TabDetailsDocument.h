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
class CTabDetailsDocument :
    public CGridBaseDocument
{
public:

    DECLARE_HTML_DOCUMENT(L"TABDETAILS.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CTabDetailsDocument)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CTabDetailsDocument)
        HTML_EVENT_TABLE_HEADER_CLICK(L"thrisk", OnHeaderClick)
        HTML_EVENT_TABLE_HEADER_CLICK(L"thtitle", OnHeaderClick)
        HTML_EVENT_TABLE_HEADER_CLICK(L"thstatus", OnHeaderClick)
        HTML_EVENT_HYPERLINK_CLICK(L"sechist", OnSecHistClick)
    END_HTML_EVENT_MAP()

public:
    CTabDetailsDocument() :
        CGridBaseDocument()
    {
    }

    virtual ~CTabDetailsDocument()
    {
        CCTRCTXI0(L"dtor");
    }

    // Events
    virtual HRESULT OnHtmlEvent(symhtml::ISymHTMLElement* lpElement, LPCWSTR szEventName, UINT nEventType, ISymBase* pExtra) throw()
    {
        HRESULT hr = S_FALSE;
        if(nEventType == symhtml::event::HYPERLINK_CLICK && szEventName)
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

protected:
    virtual CThreat::eTTState GetListType()
    {
        return CThreat::eTTState_Done;
    }

    virtual UINT GetTHTemplateID()
    {
        return IDS_DETAILS_TH;
    }

    virtual UINT GetRowTemplateID()
    {
        return IDS_DETAILS_TEMPLATE;
    }

    virtual UINT GetEmptyStringID()
    {
        return IDS_DETAILS_TEMPLATE_NONE;
    }

protected:
    // No protected members
};

