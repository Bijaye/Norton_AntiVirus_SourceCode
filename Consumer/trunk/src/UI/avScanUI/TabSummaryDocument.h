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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CTabSummaryDocument :
    public symhtml::CSymHTMLDocumentImpl,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    DECLARE_HTML_DOCUMENT(L"TABSUMMARY.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CTabSummaryDocument)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CTabSummaryDocument)
        HTML_EVENT_HYPERLINK(L"ReqAttn", OnReqAttn)
    END_HTML_EVENT_MAP()

    CTabSummaryDocument()
    {
		InvalidateDisplayStats(m_prevDisplayStats);
    }

    virtual ~CTabSummaryDocument()
    {
        CCTRCTXI0(L"dtor");
        TRACE_REF_COUNT(m_spThreatTracker);
        TRACE_REF_COUNT(m_spScanUI);
    }

    // Events
    virtual HRESULT OnDocumentComplete() throw()
    {
        HRESULT hr = symhtml::CSymHTMLDocumentImpl::OnDocumentComplete();
        if(FAILED(hr))
            return hr;

        // Update our stats
        DisplayStats();
        RefreshThreatList();

        return hr;
    }

    HRESULT OnReqAttn(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        if(m_spScanUI)
            return m_spScanUI->SwitchToTab("tabattention");
        else
            LOG_FAILURE(L"Unable to switch tabs ... pScanUI=NULL", E_NOINTERFACE);

        return S_OK;
    }

    // Control methods
    void RefreshScanStats()
    {
        // Refresh the stats
        DisplayStats();
    }

    void RefreshScanStats(const avScanUI::ScanStatistics& scanStats)
	{
        // Refresh the stats
        DisplayStats(scanStats);
	}

    void SetThreatTracker(CThreatTracker* pThreatTracker)
    {
        m_spThreatTracker = pThreatTracker;

        // Get the IScanUICallbackInt object
        if(m_spThreatTracker)
            m_spScanUI = m_spThreatTracker->GetScanUI();
        else if(m_spScanUI)
            m_spScanUI.Release();
    }

    void RefreshThreatList()
    {
        if(S_FALSE == IsDocumentComplete() || !m_spThreatTracker)
            return;

        // Update the "found" anomaly stats
        CStringW sSubStats;
        m_spThreatTracker->GetAnomalyCategoryStats(sSubStats, CThreat::eTTState_All);
        ReplaceElementHtml("containerfound", sSubStats);

        // Update the "fixed" anomaly stats
        m_spThreatTracker->GetAnomalyCategoryStats(sSubStats, CThreat::eTTState_Done);
        ReplaceElementHtml("containerfixed", sSubStats);

        // Refresh the page
        symhtml::ISymHTMLElementPtr spRoot;
        HRESULT hr = GetRootElement(spRoot);
        if(SUCCEEDED(hr) && spRoot)
            spRoot->UpdateElement(TRUE);
    }


protected:

	// DisplayStatistics are the numbers computed from the ScanStatistics
	// for display in the UI.
	typedef struct DisplayStatistics
	{
        int nFound;
        int nRemoved;
		int nTotalScanned;
		int nAttention;
		int nFiles;
        int nRegistry;
		int nProcesses;
        int nBrowserNet;
        int nHeuristic;
        int nOther;
	} DisplayStatistics;

    void DisplayStats()
    {
        // Get the scan statistics
        avScanUI::IScanUICallbacksIntPtr spScanUI = m_spThreatTracker->GetScanUI();
        if(!spScanUI)
        {
            LOG_FAILURE(L"Can't display stats ... spScanUI = NULL", E_FAIL);
            return;
        }
        avScanUI::ScanStatistics scanStats;
        spScanUI->GetScanStats(scanStats);

		// Pass them to the method that does the work.
		DisplayStats(scanStats);
	}

	void DisplayStats(const avScanUI::ScanStatistics& scanStats)
    {
        if(S_FALSE == IsDocumentComplete())
            return;

		// Get the display stats from the scan stats
		DisplayStatistics displayStats;
		GetDisplayStats(scanStats, displayStats);

        // Update all the changed top-level page stats
		SetElementInt("valuescanned", displayStats.nTotalScanned, m_prevDisplayStats.nTotalScanned);
        SetElementInt("valuefound", displayStats.nFound, m_prevDisplayStats.nFound);
        SetElementInt("valuefixed", displayStats.nRemoved, m_prevDisplayStats.nRemoved);
        SetElementInt("valueattn", displayStats.nAttention, m_prevDisplayStats.nAttention);

		// The attention state is whether or not any items need attention.
		// If the attention state has changed, update the "optsummary" state in the HTML.
        HRESULT hr;
		bool bNeedRefresh = false;
		if ( (displayStats.nAttention > 0) != (m_prevDisplayStats.nAttention > 0) )
		{
			symhtml::ISymHTMLElementPtr spElem;
			hr = GetElement("optsummary", &spElem);
			if(SUCCEEDED(hr) && spElem)
			{
				if(displayStats.nAttention > 0)
				{
					hr = spElem->SetAttributeByName("state", L"alert");
					LOG_FAILURE(L"Failed to set optsummary element state to alert", hr);
				}
				else
				{
					hr = spElem->SetAttributeByName("state", NULL);
					LOG_FAILURE(L"Failed to clear optsummary element state", hr);
				}

				// This HTML change requires a page refresh.
				bNeedRefresh = true;
			}
		}

        // Update the changed eraser (per item) stats
        SetElementInt("valuefiles", displayStats.nFiles, m_prevDisplayStats.nFiles);
        SetElementInt("valuereg", displayStats.nRegistry, m_prevDisplayStats.nRegistry);
        SetElementInt("valueproc", displayStats.nProcesses, m_prevDisplayStats.nProcesses);
        SetElementInt("valuenet", displayStats.nBrowserNet, m_prevDisplayStats.nBrowserNet);
        SetElementInt("valueah", displayStats.nHeuristic, m_prevDisplayStats.nHeuristic);
        SetElementInt("valueother", displayStats.nOther, m_prevDisplayStats.nOther);

        // Refresh the page
		if (bNeedRefresh)
		{
			symhtml::ISymHTMLElementPtr spRoot;
			hr = GetRootElement(spRoot);
			if(SUCCEEDED(hr) && spRoot)
	            spRoot->UpdateElement(TRUE);
		}

		// Remember the display stats for next time
		m_prevDisplayStats = displayStats;
    }

    void SetElementInt(LPCSTR szID, int nVal, int nPrevVal)
    {
		// If the new value differs from the previous value...
		if (nVal != nPrevVal)
		{
			// Convert the new value to an LPCWSTR.
			// Avoid dynamic allocation (CAtlString) for performance.
			WCHAR szValue[64];
			swprintf(szValue, L"%d", nVal);

			// Change the HTML element text and repaint.
			SetElementText(szID, szValue, -1, TRUE);
		}
    }

	void InvalidateDisplayStats(DisplayStatistics& displayStats)
	{
		// Reset all display statistic values to an invalid value
		// that will always result in a change detection when compared
		// to a new valid value.
        displayStats.nFound = 
        displayStats.nRemoved = 
		displayStats.nTotalScanned = 
		displayStats.nAttention = 
		displayStats.nFiles = 
        displayStats.nRegistry = 
		displayStats.nProcesses = 
        displayStats.nBrowserNet = 
        displayStats.nHeuristic = 
        displayStats.nOther = -1;
	}

	void GetDisplayStats(const avScanUI::ScanStatistics& scanStats, DisplayStatistics& displayStats)
	{
		// Convert the scan statistics into the statistics that are actually displayed.
        displayStats.nFound = scanStats.nVirDetected + scanStats.nNonVirDetected;
        displayStats.nRemoved = scanStats.nVirRemoved + scanStats.nNonVirRemoved;
		displayStats.nTotalScanned = scanStats.nTotalScanned;
		displayStats.nAttention = displayStats.nFound - displayStats.nRemoved;
		displayStats.nFiles = scanStats.nFileItems + scanStats.nDirectoryItems;
        displayStats.nRegistry = scanStats.nRegistryItems;
		displayStats.nProcesses = scanStats.nProcessItems + scanStats.nServiceItems + scanStats.nStartupItems + scanStats.nCOMItems;
        displayStats.nBrowserNet = scanStats.nBrowserCacheItems + scanStats.nCookieItems + scanStats.nHostsItems + scanStats.nLSPItems;
        displayStats.nHeuristic = scanStats.nApplicationHeuristicItems;
        displayStats.nOther = scanStats.nBatchItems + scanStats.nINIItems;
	}

protected:
    CSymPtr<CThreatTracker> m_spThreatTracker;
    avScanUI::IScanUICallbacksIntPtr m_spScanUI;
	DisplayStatistics m_prevDisplayStats;
};

