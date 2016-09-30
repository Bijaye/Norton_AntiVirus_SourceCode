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
#include "ThreatTracker.h"
#include "GridComparators.h"

// ISData Includes
#include <isDataClientLoader.h>


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class CGridBaseDocument :
    public symhtml::CSymHTMLDocumentImpl,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CGridBaseDocument() :
      CSymHTMLDocumentImpl(false)
    {
        // Empty
    }

    virtual ~CGridBaseDocument()
    {
        CCTRCTXI0(L"dtor");
        TRACE_REF_COUNT(m_spThreatTracker);
        TRACE_REF_COUNT(m_spComparator);
    }

    // Common event handlers
    virtual HRESULT OnDocumentComplete() throw()
    {
        HRESULT hr = symhtml::CSymHTMLDocumentImpl::OnDocumentComplete();
        if(FAILED(hr))
            return hr;

        // Set the sort column
        symhtml::ISymHTMLElementPtr spSortCol;
        hr = GetElement("thrisk", &spSortCol);
        bool bTmp;
        OnHeaderClick(spSortCol, NULL, bTmp);

        RefreshThreatList();

        return hr;
    }

    HRESULT OnSecHistClick(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Added code to launch Message Center here rather than call CScanUI::LaunchMsgCenter
        //  because there is no ScanUI instance in the EmailScanUI Dialog.

        ISShared::ISShared_IProvider ProviderLoader;
        ui::IProviderPtr spISElementProvider;

        if (SYM_SUCCEEDED(ProviderLoader.CreateObject(GETMODULEMGR(), &spISElementProvider)))
        {
            ui::IElementPtr pElement;
            HRESULT hr = spISElementProvider->GetElement(ISShared::CLSID_NIS_ManageQuarantine, pElement);
            if (SUCCEEDED(hr))
                hr = pElement->Configure(::GetDesktopWindow(), NULL);
            else
                LOG_FAILURE(L"Failed to get Quarantine element", hr);
        }
        else
            CCTRCTXE0(L"Failed to create the element provider");

        return S_OK;
    }

    virtual HRESULT OnHeaderClick(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Validate the input
        if(!lpElement)
            return E_INVALIDARG;

        // Get the <th>'s ID
        LPCWSTR pszColID = NULL;
        HRESULT hr = lpElement->GetAttributeByName("id", &pszColID);
        LOG_FAILURE_AND_RETURN(L"Failed to get elements id!", hr);
        CStringW sID = pszColID;

        // Get the column index
        UINT iIdx = -1;
        hr = lpElement->GetElementIndex(&iIdx);
        LOG_FAILURE_AND_RETURN(L"Failed to get element index", hr);

        if(!m_sSortCol.IsEmpty())
        {
            // Remove the old asc/desc attribute
            symhtml::ISymHTMLElementPtr spOldCol;
            hr = GetElement(m_sSortCol, &spOldCol);
            LOG_FAILURE(L"Failed to get old sort column element", hr);
            if(spOldCol)
            {
                hr = spOldCol->SetAttributeByName("order", NULL);
                LOG_FAILURE(L"Failed to clear old sort column's order attribute", hr);
            }
        }

        // If the column hasn't changed, but it is being clicked again, swap the order
        // Otherwise, this is a first click on a column, descending
        if(m_sSortCol == pszColID)
            m_bAsc = !m_bAsc;
        else
            m_bAsc = true;

        // Save the new sort column
        m_sSortCol = pszColID;

        // Setup the comparator member based on the column
        // Scope for CSingleLock
        {
            ccLib::CSingleLock csl(&m_csSortLock, INFINITE, FALSE);
            CreateComparatorForColumn(iIdx, m_bAsc, m_spComparator);
        }

        // Do the actual sort...
        return SortTable(true);
    }

    // Control methods
    void SetThreatTracker(CThreatTracker* pThreatTracker)
    {
        m_spThreatTracker = pThreatTracker;
    }

    void RefreshThreatList()
    {
        if(S_FALSE == IsDocumentComplete() || !m_spThreatTracker)
            return;

        // Get the new array of indexes
        CThreatTracker::ARRAY_INDEXES arrIndexesNew;
        m_spThreatTracker->GetIndexArrayForState(GetListType(), arrIndexesNew);

        if(arrIndexesNew.IsEmpty() && !m_arrIndexes.IsEmpty())
        {
            // Hide the text about the apply button
	        SetElementHidden("applyinst", FALSE, TRUE);

            // Load the table body
            CStringW sContent, sData;
            sContent.LoadString(GetEmptyStringID());
            sData.LoadString(GetTHTemplateID());
            sData += sContent;
            ReplaceElementHtml("itemtable", sData, -1, TRUE);  // redraw
        }
        else if(!arrIndexesNew.IsEmpty())
        {
            // Show the text about the apply button
	        SetElementHidden("applyinst", TRUE, TRUE);

            // Load the template
            CStringW sTemplate;
            sTemplate.LoadString(GetRowTemplateID());

            // Get the table element
            symhtml::ISymHTMLElementPtr spTable;
            GetElement("itemtable", &spTable);
            if(!spTable)
            {
                CCTRCTXE0(L"Failed to get itemtable");
                return;
            }

            // if our member has no items, we need to remove the empty item
            if(m_arrIndexes.IsEmpty())
            {
                symhtml::ISymHTMLElementPtr spEmptyElem;
                GetElement("rowempty", &spEmptyElem);
                if(spEmptyElem)
                    spEmptyElem->DetachElement();
            }

            // Walk the two arrays ... applying the changes
            CompareArrays(m_arrIndexes, arrIndexesNew, sTemplate, spTable);

            // Sort the table using the current comparator, and redraw
            SortTable(true);
        }

        // Replace our member copy
        m_arrIndexes.RemoveAll();
        m_arrIndexes.Copy(arrIndexesNew);
    }

protected:
    // Used by RefreshThreatList
    virtual CThreat::eTTState GetListType() = 0;
    virtual UINT GetTHTemplateID() = 0;
    virtual UINT GetRowTemplateID() = 0;
    virtual UINT GetEmptyStringID() = 0;

    // Used by OnHeaderClick
    virtual HRESULT CreateComparatorForColumn(size_t iColumn, bool bAsc, symhtml::ISymHTMLComparator*& pComparator) = 0;

    HRESULT SortTable(bool bRedraw)
    {
        // Make a local copy of the comparator
        symhtml::ISymHTMLComparatorPtr spComparator;

        // Scope for CSingleLock
        {
            ccLib::CSingleLock csl(&m_csSortLock, INFINITE, FALSE);
            if(!m_spComparator)
            {
                LOG_FAILURE_AND_RETURN(L"No comparator!  Unable to sort", E_UNEXPECTED);
            }

            spComparator = m_spComparator;
        }

        // Set the sort columns asc/desc attribute, as required
        symhtml::ISymHTMLElementPtr spColHeader;
        HRESULT hr = GetElement(m_sSortCol, &spColHeader);
        LOG_FAILURE(L"Failed to get the sort column header element", hr);
        if(spColHeader)
        {
            if(m_bAsc)
                hr = spColHeader->SetAttributeByName("order", L"asc");
            else
                hr = spColHeader->SetAttributeByName("order", L"desc");

            LOG_FAILURE(L"Failed to set the sort column's order attribute", hr);
        }

        // Get the table element
        symhtml::ISymHTMLElementPtr spTable;
        hr = GetElement("itemtable", &spTable);
        LOG_FAILURE_AND_RETURN(L"Failed to get itemtable element", hr);

        // Get a count of fixed rows
        LPCWSTR pszFixed;
        hr = spTable->GetAttributeByName("fixedrows", &pszFixed);
        LOG_FAILURE_AND_RETURN(L"Failed to get a count of fixedrows", hr);

        // Convert the count to an int
        UINT iFixed = _wtol(pszFixed);

        // get a child count
        UINT iChildren = 0;
        hr = spTable->GetChildrenCount(&iChildren);
        LOG_FAILURE_AND_RETURN(L"Failed to get a count of children", hr);

        // Do the sort
        hr = spTable->SortElements(iFixed, iChildren, spComparator);
        LOG_FAILURE_AND_RETURN(L"SortElements failed!", hr);

        // Update the table
        if(bRedraw)
        {
            hr = spTable->UpdateElement(TRUE);
            LOG_FAILURE_AND_RETURN(L"UpdateElement failed!", hr);
        }

        return hr;
    }

    void AddItem(size_t iThreatIndex, CString& sTemplate, symhtml::ISymHTMLElement* pTable)
    {
        CCTRCTXI1(L"Adding item %d", iThreatIndex);
        CString sItemData = m_spThreatTracker->GenerateOutputForItem(sTemplate, iThreatIndex);
        HRESULT hr = pTable->AppendElementHtml(sItemData, sItemData.GetLength());
        LOG_FAILURE(L"Failed to append element html", hr);

        // Set as no longer dirty
        m_spThreatTracker->SetThreatDirty(iThreatIndex, false);
    }

    void DeleteItem(size_t iThreatIndex, CString& sTemplate, symhtml::ISymHTMLElement* pTable)
    {
        CCTRCTXI1(L"Deleting item %d", iThreatIndex);

        // Build the row name
        CStringA sRowName;
        sRowName.Format("row%d", iThreatIndex);

        // Get the row
        symhtml::ISymHTMLElementPtr spEmptyElem;
        HRESULT hr = GetElement(sRowName, &spEmptyElem);
        LOG_FAILURE(L"Failed to get row", hr);
        if(spEmptyElem)
        {
            hr = spEmptyElem->DetachElement();
            LOG_FAILURE(L"Failed to delete row", hr);
        }
    }

    void CompareArrays(CThreatTracker::ARRAY_INDEXES& arrCur, CThreatTracker::ARRAY_INDEXES& arrNew, CString& sTemplate, symhtml::ISymHTMLElement* pTable)
    {
        size_t iLenCur = arrCur.GetCount();
        size_t iLenNew = arrNew.GetCount();
        size_t iPosCur = 0;
        size_t iPosNew = 0;

        // While we still have items in both arrays...
        while(iPosCur < iLenCur && iPosNew < iLenNew)
        {
            // Do the items match?
            if(arrCur[iPosCur] == arrNew[iPosNew])
            {
                // Match ... check if the item is dirty
                size_t iItem = arrCur[iPosCur];
                if(m_spThreatTracker->GetThreatDirty(iItem))
                {
                    // Item is dirty ... remove it from the list and re-add it
                    DeleteItem(iItem, sTemplate, pTable);
                    AddItem(iItem, sTemplate, pTable);
                }

                // Move both forward
                ++iPosCur;
                ++iPosNew;
                continue;
            }

            // Check if items have been removed at this position
            if(iPosNew < iLenNew)
            {
                while(iPosCur < iLenCur && arrCur[iPosCur] < arrNew[iPosNew])
                {
                    DeleteItem(arrCur[iPosCur], sTemplate, pTable);

                    // Move the current pointer forward
                    ++iPosCur;
                }
            }

            // Check if items have been added at this position
            if(iPosCur < iLenCur)
            {
                while(iPosNew < iLenNew && arrNew[iPosNew] < arrCur[iPosCur])
                {
                    AddItem(arrNew[iPosNew], sTemplate, pTable);

                    // Move the new pointer forward
                    ++iPosNew;
                }
            }
        }

        // If items only remain in the current array, all remaining items have been deleted
        while(iPosCur < iLenCur)
        {
            // Deleted
            DeleteItem(arrCur[iPosCur], sTemplate, pTable);

            // Next
            ++iPosCur;
        }

        // If items only remain in the new array, all remaining items have been added
        while(iPosNew < iLenNew)
        {
            // Deleted
            AddItem(arrNew[iPosNew], sTemplate, pTable);

            // Next
            ++iPosNew;
        }
    }

    HRESULT SetElementHidden(LPCSTR szID, BOOL bDisplay, BOOL bRenderNow)
    {
        HRESULT hr = S_OK;

        symhtml::ISymHTMLElementPtr spElement;
        hr = GetElement(szID, &spElement);
        if(FAILED(hr))
            return hr;

        LPCWSTR szwValue = bDisplay ? NULL : L"1";
        hr = spElement->SetAttributeByName("hide", szwValue);

        if(SUCCEEDED(hr))
            spElement->UpdateElement(bRenderNow);

        return hr;
    }

protected:
    CSymPtr<CThreatTracker> m_spThreatTracker;

    // Display array
    CThreatTracker::ARRAY_INDEXES m_arrIndexes;

    // Sort data
    ccLib::CCriticalSection m_csSortLock;
    symhtml::ISymHTMLComparatorPtr m_spComparator;
    CStringA m_sSortCol;
    bool m_bAsc;
};