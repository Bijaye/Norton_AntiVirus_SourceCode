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
#include <ccEraserInterface.h>
#include <AVInterfaces.h>
#include "ThreatTracker.h"
#include <resource.h>

// Help
#include <SymHelpLauncher.h>
#include <SymHelp.h>

#define CASE_LOAD_STRING_PLURAL(xCase, xStr, xValue, xResSingle, xResPlural) case xCase: \
                                                                               if(xValue == 1) { xStr.LoadString(xResSingle); } \
                                                                               else { xStr.LoadString(xResPlural); } \
                                                                               break;
#define  TAB_RISK    "RiskTab"   
#define  TAB_DETAILS "DetailsTab"


class CRiskDetailsDlg :
    public symhtml::CSymHTMLDocumentImpl,
    public symhtml::CSymHTMLDialogDocumentImpl,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{

public:
    DECLARE_HTML_DOCUMENT(L"RISKDETAILS.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CRiskDetailsDlg)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CRiskDetailsDlg)
        HTML_EVENT_BUTTON_CLICK(L"close", OnClose)
        HTML_EVENT_HYPERLINK(L"matrix_risk_name", OnVirName)
        HTML_EVENT_HYPERLINK(L"details_risk_name", OnVirName)
        HTML_EVENT_HYPERLINK(L"overallTitle", OnOverall)
        HTML_EVENT_HYPERLINK(L"performaceTitle", OnPerformance)
        HTML_EVENT_HYPERLINK(L"privacyTitle", OnPrivacy)
        HTML_EVENT_HYPERLINK(L"removalTitle", OnRemoval)
        HTML_EVENT_HYPERLINK(L"stealthTitle", OnStealth)
        HTML_EVENT_HYPERLINK(L"moreInfo", OnMoreInfo)
    END_HTML_EVENT_MAP()

public:
    CRiskDetailsDlg(CThreatTracker* pThreatTracker, size_t iThreatIndex,
                    AVModule::IAVMapDwordData* pThreatMap, AVModule::IAVArrayData* pDisplayData,
                    AVModule::ThreatTracking::eThreatTypes eType, LPCSTR pszStartTab = TAB_RISK)
    {
        m_spThreatTracker = pThreatTracker;
        m_iThreatIndex = iThreatIndex;
        m_spThreatMap = pThreatMap;
        m_spDisplayData = pDisplayData;
        m_eType = eType;
        m_hIcon = NULL;
        m_sStartTab = pszStartTab;
    }
    virtual ~CRiskDetailsDlg(void)
    {
        CCTRCTXI0(L"dtor");

        if(m_hIcon)
            DestroyIcon(m_hIcon);

        TRACE_REF_COUNT(m_spThreatTracker);
        TRACE_REF_COUNT(m_spThreatMap);
        TRACE_REF_COUNT(m_spDisplayData);
    }

    // Event handlers
    HRESULT OnClose(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        EndDialog(IDYES);
        return S_OK;
    }

    HRESULT OnVirName(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        return m_spThreatTracker->LaunchResponsePage(m_iThreatIndex);
    }

    HRESULT OnOverall(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Copy the level and the graphic
        HRESULT hr = DuplicateAttribute("matrix_imageThreatOverall", "imageDetails", "src");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate attribute", hr);
        hr = DuplicateElementText("matrix_overall", "threatImpactDetails");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate text", hr);

        // Update the details title
        CStringW sTitle;
        sTitle.LoadString(IDS_OVERALL);
        hr = SetElementText("threatImpactTitle", sTitle);
        LOG_FAILURE_AND_RETURN(L"Failed to update text", hr);

        // Get the description
        CStringW sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{DESCTLEVEL}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
        return SetElementText("ThreatDescription", sTmp);
    }

    HRESULT OnPerformance(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Copy the level and the graphic
        HRESULT hr = DuplicateAttribute("imageThreatPerformace", "imageDetails", "src");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate attribute", hr);
        hr = DuplicateElementText("performace", "threatImpactDetails");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate text", hr);

        // Update the details title
        CStringW sTitle;
        sTitle.LoadString(IDS_PERFORMANCE);
        hr = SetElementText("threatImpactTitle", sTitle);
        LOG_FAILURE_AND_RETURN(L"Failed to update text", hr);

        // Get the description
        CStringW sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{DESCTLEVEL[PROP=6]}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
        return SetElementText("ThreatDescription", sTmp);
    }

    HRESULT OnPrivacy(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Copy the level and the graphic
        HRESULT hr = DuplicateAttribute("imageThreatPrivacy", "imageDetails", "src");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate attribute", hr);
        hr = DuplicateElementText("privacy", "threatImpactDetails");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate text", hr);

        // Update the details title
        CStringW sTitle;
        sTitle.LoadString(IDS_PRIVACY);
        hr = SetElementText("threatImpactTitle", sTitle);
        LOG_FAILURE_AND_RETURN(L"Failed to update text", hr);

        // Get the description
        CStringW sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{DESCTLEVEL[PROP=7]}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
        return SetElementText("ThreatDescription", sTmp);
    }

    HRESULT OnRemoval(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Copy the level and the graphic
        HRESULT hr = DuplicateAttribute("imageThreatRemoval", "imageDetails", "src");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate attribute", hr);
        hr = DuplicateElementText("removal", "threatImpactDetails");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate text", hr);

        // Update the details title
        CStringW sTitle;
        sTitle.LoadString(IDS_REMOVAL);
        hr = SetElementText("threatImpactTitle", sTitle);
        LOG_FAILURE_AND_RETURN(L"Failed to update text", hr);

        // Get the description
        CStringW sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{DESCTLEVEL[PROP=5]}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
        return SetElementText("ThreatDescription", sTmp);
    }

    HRESULT OnStealth(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Copy the level and the graphic
        HRESULT hr = DuplicateAttribute("imageThreatStealth", "imageDetails", "src");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate attribute", hr);
        hr = DuplicateElementText("stealth", "threatImpactDetails");
        LOG_FAILURE_AND_RETURN(L"Failed to duplicate text", hr);

        // Update the details title
        CStringW sTitle;
        sTitle.LoadString(IDS_STEALTH);
        hr = SetElementText("threatImpactTitle", sTitle);
        LOG_FAILURE_AND_RETURN(L"Failed to update text", hr);

        // Get the description
        CStringW sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{DESCTLEVEL[PROP=4]}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
        return SetElementText("ThreatDescription", sTmp);
    }

    HRESULT OnMoreInfo(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
    {
        // Figure out which tab is selected
        symhtml::ISymHTMLElementPtr spRiskTab;
        HRESULT hr = GetElement("RiskTab", &spRiskTab);
        LOG_FAILURE_AND_RETURN(L"Failed to get RiskTab element", hr);

        LPCWSTR pszValue;
        hr = spRiskTab->GetAttributeByName("selected", &pszValue);

        // Launch the appropriate help
        if(SUCCEEDED(hr))
            hr = LaunchHelp(NAV_THREAT_MATRIX, NULL);
        else
            hr = LaunchHelp(NAV_AFFECTED_AREAS, NULL);

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

        // Set our title
        hr = SetWindowText(sBrandingProductName);

        // Load our icons
        CISVersion::GetProductIcon(m_hIcon);
        hr = SetIcon(m_hIcon, false);

        // Was this a manual quarantine item?
        AVModule::ThreatTracking::eScannerTypes eDetectedBy = AVModule::ThreatTracking::ScannerTypeManualScanner;
        hr = m_spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_SCANNERTYPES_DETECTED_BY, (DWORD&)eDetectedBy);
        LOG_FAILURE(L"Failed to get SCANNERTYPES_DETECTED_BY", hr);

        // If we are missing anomaly display data, hide the details tab
        // unless this is a compressed threat... in which case we don't use anomaly data
        if(!m_spDisplayData && m_eType != AVModule::ThreatTracking::ThreatType_Compressed)
        {
            SetElementHidden("DetailsTab", false, TRUE);
            SetElementHidden("DetailsBody", false, TRUE);
            m_sStartTab = TAB_RISK;
        }

        // If this is a manual quarantine item, handle it differently
        if(eDetectedBy == AVModule::ThreatTracking::ScannerTypeManualQuarantine)
        {
            // hide the summary page
            m_sStartTab = TAB_DETAILS;
            SetElementHidden("RiskTab", false, TRUE);
            SetElementHidden("MatrixBody", false, TRUE);

            // Hide the unused elements...
            SetElementHidden("drisk_level_title", false, TRUE);
            SetElementHidden("drisk_dependencies_title", false, TRUE);

            CStringW sTmp;

            // Replace the "Risk Name" text with, Description
            sTmp.LoadString(IDS_MQUAR_DESCRIPTION);
            ReplaceElementHtml("drisk_name_title", sTmp, -1, TRUE);

            // Fill in the users description
            sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{NAME}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("details_risk_name", sTmp);

            // Set the type to "Manual Quarantine"
            sTmp.LoadString(IDS_TITLE_MQUARANTINE);
            SetElementText("details_risk_type", sTmp);

            // Set the long description
            sTmp.LoadString(IDS_MQUAR_LONGDESC);
            SetElementText("details_description", sTmp);
        }
        else
        {
            //
            // Fill in all the blanks...
            //
            // Name
            CStringW sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{NAME}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("matrix_risk_name", sTmp);
            SetElementText("details_risk_name", sTmp);

            // Threat Categories
            sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{TCATS}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("matrix_risk_type", sTmp);
            SetElementText("details_risk_type", sTmp);

            // Threat category description
            sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{DESCTCATS}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("details_description", sTmp);

            // Overall threat level
            sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{TLEVEL}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("matrix_overall", sTmp);
            SetElementText("details_overall", sTmp);

            // Dependency flag
            sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{DEPENDS}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("matrix_risk_dependencies", sTmp);
            SetElementText("details_risk_dependencies", sTmp);

            // Individual TLevel text
            sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{TLEVEL[PROP=4]}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("stealth", sTmp);
            sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{TLEVEL[PROP=5]}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("removal", sTmp);
            sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{TLEVEL[PROP=6]}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("performace", sTmp);
            sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{TLEVEL[PROP=7]}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);
            SetElementText("privacy", sTmp);

            // Update all the threat level graphics
            UpdateTLevelGraphics();

            // Show the overall description
            bool bTmp = true;
            OnOverall(NULL, NULL, bTmp);
        }

        // Switch to the correct tab
        SwitchToTab(m_sStartTab);

        // Fill in the types list
        FillTypesList();

        // Get a count of each type
        CThreatTracker::MAP_TYPE_COUNT mapTypes;
        m_spThreatTracker->GetTypeCountsForThreat(m_iThreatIndex, mapTypes);

        // Find the first item that will be displayed
        for(DWORD dwType = ccEraser::FirstRemediationType; dwType <= ccEraser::LastRemediationType; ++dwType)
        {
            if(mapTypes.Lookup((ccEraser::eObjectType)dwType) != NULL)
                break;
        }

        // Start with first type selected
        if(dwType <= ccEraser::LastRemediationType)
            FillDetailsList((ccEraser::eObjectType)dwType);

        return hr;
    }

    virtual HRESULT OnHtmlEvent(symhtml::ISymHTMLElement* lpElement, LPCWSTR szEventName, UINT nEventType, ISymBase* pExtra) throw()
    {
        HRESULT hr = S_FALSE;

        if(nEventType == symhtml::event::TABLE_ROW_CLICK && szEventName && wcsnicmp(szEventName, L"TypeRow", 7) == 0)
        {
            // Get the type value
            DWORD dwType = _wtol(szEventName + 7);

            // Display the proper details
            hr = FillDetailsList((ccEraser::eObjectType)dwType);
        }
        else
        {
            // Call the base handler
            hr = symhtml::CSymHTMLDocumentImpl::OnHtmlEvent(lpElement, szEventName, nEventType, pExtra);
        }

        return hr;
    }

protected:
    HRESULT SwitchToTab(LPCSTR pszTabID)
    {
        // Get the tab container
        symhtml::ISymHTMLElementPtr spTabContainer;
        HRESULT hr = GetElement("tabcontainer", &spTabContainer);
        LOG_FAILURE_AND_RETURN(L"Failed to get the tabcontainer element", hr);

        // Get the new tab
        symhtml::ISymHTMLElementPtr spNewTab;
        hr = GetElement(pszTabID, &spNewTab);
        LOG_FAILURE_AND_RETURN(L"Failed to find the new tab", hr);

        // Check if the new tab is already selected
        UINT nState = 0;
        hr = spNewTab->GetElementState(&nState);
        LOG_FAILURE_AND_RETURN(L"Failed to get new tabs state", hr);
        if(nState & symhtml::state::current)
        {
            // nothing to do... tab is already selected
            return S_OK;
        }

        // Find the currently selected tab and panel
        symhtml::ISymHTMLElementPtr spCurTab, spCurPage;
        hr = spTabContainer->FindFirstElement("[name]:expanded", &spCurPage);
        LOG_FAILURE_AND_RETURN(L"Failed to find the currently expanded page", hr);
        hr = spTabContainer->FindFirstElement("[panel]:current", &spCurTab);
        LOG_FAILURE_AND_RETURN(L"Failed to find the current tab", hr);

        // Get the new panel name
        LPCWSTR pszPanelName;
        hr = spNewTab->GetAttributeByName("panel", &pszPanelName);

        // Find the new panel
        CStringA sSelector;
        sSelector.Format("[name=\"%S\"]", pszPanelName);
        symhtml::ISymHTMLElementPtr spNewPage;
        hr = spTabContainer->FindFirstElement(sSelector, &spNewPage);
        LOG_FAILURE_AND_RETURN(L"Failed to find the matching new page", hr);

        // "Unselect" the current tab and page
        spCurPage->SetAttributeByName("selected", NULL);                // remove selected attribute - just in case somone is using attribute selectors
        spCurPage->SetElementState(symhtml::state::collapsed, 0, TRUE); // collapse it
        spCurTab->SetAttributeByName("selected", NULL);                 // remove selected attribute
        spCurTab->SetElementState(0, symhtml::state::current, TRUE);    // reset also state flag, :current

        // "Select" the new tab and page
        spNewPage->SetAttributeByName("selected", L"");                 // set selected attribute (empty)
        spNewPage->SetElementState(symhtml::state::expanded, 0, TRUE);  // expand it
        spNewTab->SetAttributeByName("selected", L"");                  // set selected attribute (empty)
        spNewTab->SetElementState(symhtml::state::current, 0, TRUE);  // set also state flag, :current

        // Redraw
        spTabContainer->UpdateElement(TRUE);

        return S_OK;
    }

    HRESULT UpdateTLevelGraphics()
    {
        HRESULT hr = E_FAIL;
        AVModule::ThreatTracking::eThreatLevels eLevel;

        // Update the individual threat levels
        AVModule::IAVMapDwordDataPtr spMapLevels;
        hr = m_spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_INDIVIDUAL_THREAT_LEVELS, AVModule::AVDataTypes::eTypeMapDword, reinterpret_cast<ISymBase*&>(spMapLevels));
        if(SUCCEEDED(hr) && spMapLevels)
        {
            hr = spMapLevels->GetValue(ccEraser::IAnomaly::Performance, (DWORD&)eLevel);
            if(SUCCEEDED(hr))
                UpdateTLevelGraphic(eLevel, "imageThreatPerformace");

            hr = spMapLevels->GetValue(ccEraser::IAnomaly::Privacy, (DWORD&)eLevel);
            if(SUCCEEDED(hr))
                UpdateTLevelGraphic(eLevel, "imageThreatPrivacy");

            hr = spMapLevels->GetValue(ccEraser::IAnomaly::Removal, (DWORD&)eLevel);
            if(SUCCEEDED(hr))
                UpdateTLevelGraphic(eLevel, "imageThreatRemoval");

            hr = spMapLevels->GetValue(ccEraser::IAnomaly::Stealth, (DWORD&)eLevel);
            if(SUCCEEDED(hr))
                UpdateTLevelGraphic(eLevel, "imageThreatStealth");
        }

        // Overall threat level
        hr = m_spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, (DWORD&)eLevel);
        if(SUCCEEDED(hr))
        {
            UpdateTLevelGraphic(eLevel, "matrix_imageThreatOverall");
            UpdateTLevelGraphic(eLevel, "details_imageThreatOverall");
        }

        return hr;
    }

    HRESULT UpdateTLevelGraphic(AVModule::ThreatTracking::eThreatLevels eLevel, LPCSTR pszElem)
    {
        CStringW sGraphic;
        sGraphic.Format(L"damage_%d.gif", ((DWORD)eLevel) + 1);

        symhtml::ISymHTMLElementPtr spImg;
        HRESULT hr = GetElement(pszElem, &spImg);
        if(FAILED(hr))
            return hr;

        return spImg->SetAttributeByName("src", sGraphic);
    }

    HRESULT FillTypesList()
    {
        // Get the type count table rows
        CStringW sTmp = m_spThreatTracker->GenerateOutputForItem(L"%{REMDETAILS[FMT=T]}%", m_iThreatIndex, m_spThreatMap, m_eType, m_spDisplayData);

        // Replace the details HTML
        return ReplaceElementHtml("RemDetailsTypes", sTmp, -1, TRUE);
    }

    HRESULT FillDetailsList(ccEraser::eObjectType eType)
    {
        HRESULT hr = E_FAIL;
        CStringW sDetailsHTML, sTmp;

        // Load the header...
        sDetailsHTML.LoadString(IDS_REMDETAILS_TH);

        // Get the array of targets for this threat
        CAtlArray<CStringW> arrTargets;
        hr = m_spThreatTracker->GetTargetsForThreat(m_spDisplayData, m_eType, m_spThreatMap, eType, arrTargets, true);
        LOG_FAILURE_AND_RETURN(L"Failed to get list of targets", hr);

        // Get the graphic for these target
        CStringW sGraphic = GetTypeGraphic(eType);

        // Loop through all the targets
        for(size_t iIdx = 0; iIdx < arrTargets.GetCount(); ++iIdx)
        {
            // Build this table row
            sTmp.Format(L"<tr><td><img src=\"%s\"/>%s</td></tr>", sGraphic.GetString(), arrTargets[iIdx].GetString());

            // Add it to the details list...
            sDetailsHTML += sTmp;
        }

        // Replace the details HTML
        hr = ReplaceElementHtml("RemDetailsTable", sDetailsHTML, -1, TRUE);
        return hr;
    }

    CStringW GetTypeGraphic(ccEraser::eObjectType eRemType)
    {
        switch(eRemType)
        {
            case ccEraser::RegistryRemediationActionType:
                return L"type_reg.png";
            case ccEraser::FileRemediationActionType:
                return L"type_file.png";
            case ccEraser::ProcessRemediationActionType:
                return L"type_proc.png";
            case ccEraser::BatchRemediationActionType:
                return L"type_batch.png";
            case ccEraser::INIRemediationActionType:
                return L"type_ini.png";
            case ccEraser::ServiceRemediationActionType:
                return L"type_serv.png";
            case ccEraser::COMRemediationActionType:
                return L"type_com.png";
            case ccEraser::HostsRemediationActionType:
                return L"type_host.png";
            case ccEraser::LSPRemediationActionType:
                return L"type_lsp.png";
            case ccEraser::BrowserCacheRemediationActionType:
                return L"type_cache.png";
            case ccEraser::SystemRemediationActionType:
                return L"type_sys.png";
            case ccEraser::CookieRemediationActionType:
                return L"type_cookie.png";
            default:
                return L"type_unk.png";
        }

        return L"type_unk.png";
    }

    HRESULT DuplicateAttribute(LPCSTR pszElemFrom, LPCSTR pszElemTo, LPCSTR pszAttrib)
    {
        symhtml::ISymHTMLElementPtr spFrom, spTo;
        HRESULT hr = GetElement(pszElemFrom, &spFrom);
        if(FAILED(hr))
            return hr;

        hr = GetElement(pszElemTo, &spTo);
        if(FAILED(hr))
            return hr;
        
        // Get the attribute value from the "from" element
        LPCWSTR pszVal = NULL;
        hr = spFrom->GetAttributeByName(pszAttrib, &pszVal);
        if(FAILED(hr))
            return hr;

        // Set the new value
        return spTo->SetAttributeByName(pszAttrib, pszVal);
    }

    HRESULT DuplicateElementText(LPCSTR pszElemFrom, LPCSTR pszElemTo)
    {
        symhtml::ISymHTMLElementPtr spFrom, spTo;
        HRESULT hr = GetElement(pszElemFrom, &spFrom);
        if(FAILED(hr))
            return hr;

        hr = GetElement(pszElemTo, &spTo);
        if(FAILED(hr))
            return hr;
        
        // Get the attribute value from the "from" element
        CStringW sVal;
        UINT uLen = 1024;
        hr = spFrom->GetElementText(sVal.GetBuffer(uLen), &uLen);
        sVal.ReleaseBuffer(uLen);
        if(FAILED(hr))
            return hr;

        // Set the new value
        return spTo->SetElementInnerText(sVal, -1);
    }

    HRESULT LaunchHelp(DWORD dwHelpID, HWND hParentWnd)
    {
        CCTRCTXI1(L"Launching help with ID %d", dwHelpID);

        isshared::CHelpLauncher Help;
        HWND hWndHelp = Help.LaunchHelp(dwHelpID, hParentWnd);
        if(!hWndHelp)
        {
            CCTRCTXE0(L"Failed to launch Help");
            return E_FAIL;
        }

        return S_OK;
    }

    HRESULT SetElementHidden(LPCSTR szID, BOOL bDisplay, BOOL bRenderNow = FALSE)
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
    size_t m_iThreatIndex;
    AVModule::IAVMapDwordDataPtr m_spThreatMap;
    AVModule::IAVArrayDataPtr m_spDisplayData;
    AVModule::ThreatTracking::eThreatTypes m_eType;
    HICON m_hIcon;
    CStringA m_sStartTab;
};
