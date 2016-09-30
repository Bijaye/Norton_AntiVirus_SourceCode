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
#include <SymHTMLComparator.h>
#include <SymHTMLElement.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CAlphaComparator :
    public symhtml::ISymHTMLComparator,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLComparator, symhtml::ISymHTMLComparator)
    SYM_INTERFACE_MAP_END()

    CAlphaComparator(UINT iColumn, bool bAsc, symhtml::ISymHTMLComparator* pSecondaryComparator = NULL) :
        m_iColumn(iColumn),
        m_bAsc(bAsc),
        m_spSecondaryComparator(pSecondaryComparator)
    {
    }

    virtual ~CAlphaComparator()
    {
        CCTRCTXI0(L"dtor");
        TRACE_REF_COUNT(m_spSecondaryComparator);
    }

    // ISymHTMLComparator methods
    virtual HRESULT Compare(symhtml::ISymHTMLElement* lpRowElement1, symhtml::ISymHTMLElement* lpRowElement2, int& nResult) throw()
    {
        // Get the two cells to compare
        symhtml::ISymHTMLElementPtr spElem1, spElem2;
        HRESULT hr = lpRowElement1->GetNthChild(m_iColumn, &spElem1);
        LOG_FAILURE_AND_RETURN(L"GetNthChild failed for row 1", hr);
        hr = lpRowElement2->GetNthChild(m_iColumn, &spElem2);
        LOG_FAILURE_AND_RETURN(L"GetNthChild failed for row 2", hr);

        // Get the text from the two cells
        cc::IStringPtr spText1, spText2;
        hr = spElem1->GetElementInnerText(&spText1);
        LOG_FAILURE_AND_RETURN(L"GetElementInnerText failed for cell 1", hr);
        hr = spElem2->GetElementInnerText(&spText2);
        LOG_FAILURE_AND_RETURN(L"GetElementInnerText failed for cell 2", hr);

        // Do the compare
        if(m_bAsc)
            nResult = wcsicmp(spText1->GetStringW(), spText2->GetStringW());
        else
            nResult = wcsicmp(spText2->GetStringW(), spText1->GetStringW());

        // If we matched and we have a secondary, sort by the secondary
        if(nResult == 0 && m_spSecondaryComparator)
            return m_spSecondaryComparator->Compare(lpRowElement1, lpRowElement2, nResult);

        // Succeeded
        return S_OK;
    }

protected:
    // no protected methods

protected:
    UINT m_iColumn;
    bool m_bAsc;
    symhtml::ISymHTMLComparatorPtr m_spSecondaryComparator;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHMLComparator :
    public symhtml::ISymHTMLComparator,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLComparator, symhtml::ISymHTMLComparator)
    SYM_INTERFACE_MAP_END()

    CHMLComparator(UINT iColumn, bool bAsc, symhtml::ISymHTMLComparator* pSecondaryComparator) :
        m_iColumn(iColumn),
        m_bAsc(bAsc),
        m_spSecondaryComparator(pSecondaryComparator)
    {
        // Load our high/medium/low strings
        m_sHigh.LoadString(IDS_TL_HIGH);
        m_sMedium.LoadString(IDS_TL_MED);
        m_sLow.LoadString(IDS_TL_LOW);
    }

    virtual ~CHMLComparator()
    {
        CCTRCTXI0(L"dtor");
        TRACE_REF_COUNT(m_spSecondaryComparator);
    }

    // ISymHTMLComparator methods
    virtual HRESULT Compare(symhtml::ISymHTMLElement* lpRowElement1, symhtml::ISymHTMLElement* lpRowElement2, int& nResult) throw()
    {
        // Get the two cells to compare
        symhtml::ISymHTMLElementPtr spElem1, spElem2;
        HRESULT hr = lpRowElement1->GetNthChild(m_iColumn, &spElem1);
        LOG_FAILURE_AND_RETURN(L"GetNthChild failed for row 1", hr);
        hr = lpRowElement2->GetNthChild(m_iColumn, &spElem2);
        LOG_FAILURE_AND_RETURN(L"GetNthChild failed for row 2", hr);

        // Get the two hml attribute values
        LPCWSTR szHML1, szHML2;
        spElem1->GetAttributeByName("hml", &szHML1);
        spElem2->GetAttributeByName("hml", &szHML2);

        // Make sure we have both... if not, use the fallback
        if(!szHML1 || !szHML2)
        {
            if(m_spSecondaryComparator)
                return m_spSecondaryComparator->Compare(lpRowElement1, lpRowElement2, nResult);
            else
            {
                nResult = 0;
                return S_OK;
            }
        }

        // Do the compare
        if(m_bAsc)
            nResult = wcsicmp(szHML1, szHML2);
        else
            nResult = wcsicmp(szHML2, szHML1);

        // If we matched and we have a secondary, sort by the secondary
        if(nResult == 0 && m_spSecondaryComparator)
            return m_spSecondaryComparator->Compare(lpRowElement1, lpRowElement2, nResult);

        // Succeeded
        return S_OK;
    }

protected:
    // no protected methods

protected:
    UINT m_iColumn;
    bool m_bAsc;
    symhtml::ISymHTMLComparatorPtr m_spSecondaryComparator;
    CStringW m_sHigh;
    CStringW m_sMedium;
    CStringW m_sLow;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CDropdownComparator :
    public symhtml::ISymHTMLComparator,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLComparator, symhtml::ISymHTMLComparator)
    SYM_INTERFACE_MAP_END()

    CDropdownComparator(UINT iColumn, bool bAsc, symhtml::ISymHTMLComparator* pSecondaryComparator = NULL) :
        m_iColumn(iColumn),
        m_bAsc(bAsc),
        m_spSecondaryComparator(pSecondaryComparator)
    {
    }

    virtual ~CDropdownComparator()
    {
        CCTRCTXI0(L"dtor");
        TRACE_REF_COUNT(m_spSecondaryComparator);
    }

    // ISymHTMLComparator methods
    virtual HRESULT Compare(symhtml::ISymHTMLElement* lpRowElement1, symhtml::ISymHTMLElement* lpRowElement2, int& nResult) throw()
    {
        // Get the two cells to compare
        symhtml::ISymHTMLElementPtr spElem1, spElem2;
        HRESULT hr = lpRowElement1->GetNthChild(m_iColumn, &spElem1);
        LOG_FAILURE_AND_RETURN(L"GetNthChild failed for row 1", hr);
        hr = lpRowElement2->GetNthChild(m_iColumn, &spElem2);
        LOG_FAILURE_AND_RETURN(L"GetNthChild failed for row 2", hr);

        // Get the dropdown items
        symhtml::ISymHTMLElementPtr spSelect1, spSelect2;
        hr = spElem1->FindFirstElement("select", &spSelect1);
        LOG_FAILURE_AND_RETURN(L"FindFirstElement(select) failed for cell 1", hr);
        hr = spElem2->FindFirstElement("select", &spSelect2);
        LOG_FAILURE_AND_RETURN(L"FindFirstElement(select) failed for cell 2", hr);

        // Get the text from the two cells
        cc::IStringPtr spText1, spText2;
        hr = GetCBSelection(spSelect1, spText1);
        LOG_FAILURE_AND_RETURN(L"GetCBSelection failed for select 1", hr);
        hr = GetCBSelection(spSelect2, spText2);
        LOG_FAILURE_AND_RETURN(L"GetCBSelection failed for select 2", hr);

        // Do the compare
        if(m_bAsc)
            nResult = wcsicmp(spText1->GetStringW(), spText2->GetStringW());
        else
            nResult = wcsicmp(spText2->GetStringW(), spText1->GetStringW());

        // If we matched and we have a secondary, sort by the secondary
        if(nResult == 0 && m_spSecondaryComparator)
            return m_spSecondaryComparator->Compare(lpRowElement1, lpRowElement2, nResult);

        // Succeeded
        return S_OK;
    }

protected:
    virtual HRESULT GetCBSelection(symhtml::ISymHTMLElement* lpElement, cc::IString*& pString)
    {
        CCTRACEI(CCTRCTX L"Enter");
        HRESULT hr = S_OK;

        if(!lpElement)
        {
            CCTRACEE(CCTRCTX L"Element pointer NULL");
            return E_POINTER;
        }

        symhtml::ISymHTMLElementPtr pChildElement;
        hr = lpElement->FindFirstElement("caption", &pChildElement);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX L"Could not get caption element");
            return hr;
        }

        if(pString)
        {
            pString->Release();
            pString = NULL;
        }

        hr = pChildElement->GetElementInnerText(&pString);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX L"Could not get caption text");
            return hr;
        }

        return hr;
    }
protected:
    UINT m_iColumn;
    bool m_bAsc;
    symhtml::ISymHTMLComparatorPtr m_spSecondaryComparator;
};