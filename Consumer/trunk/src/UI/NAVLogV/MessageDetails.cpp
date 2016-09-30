// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MessageDetails.h"

#include "uiNumberDataInterface.h"
#include "uiStringDataInterface.h"
#include "uiDateDataInterface.h"

#include "AvDataIds.h"

#include "uiAvProviderElements.h"
#include <uiNISDataElementGuids.h>

#include "..\navlogvres\resource.h"

#include <ccEraserInterface.h>
#include <ThreatCatInfo.h>

#define START_PAGE() \
    HRESULT hr; \
    symhtml::ISymHTMLElementPtr spRootElement; \
    hr = GetRootElement(&spRootElement); \
    if(FAILED(hr)) \
    return E_FAIL; \
    symhtml::ISymHTMLElementPtr pBodyElement; \
    hr = spRootElement->FindFirstElement("body", &pBodyElement); \
    if(FAILED(hr)) \
	    return E_FAIL;
    
#define START_SUMMARY_SECTION() \
    ui::IDataPtr spData; \
	CSummaryPageElementPtr spSummaryPage = new CSummaryPageElement; \
	spSummaryPage->Render(pBodyElement); \
	symhtml::ISymHTMLElementPtr spParent; \
	hr = spRootElement->GetElementByID("page-element-summary-table", &spParent);

#define ADD_SUMMARY_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
    hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
    { \
        CSummaryEntryElementPtr spAvElement = new CSummaryEntryElement; \
        spAvElement->SetData(spData, fmt); \
        spAvElement->SetLabel(label); \
        spAvElement->SetHtmlId(htmlId); \
        spAvElement->Render(spParent); \
    }

#define ADD_SCANSUMMARY_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
	CScanSummaryEntryElementPtr spAvElement = new CScanSummaryEntryElement; \
	spAvElement->SetData(spData, fmt); \
	spAvElement->SetLabel(label); \
	spAvElement->SetHtmlId(htmlId); \
	spAvElement->Render(spParent); \
	}

#define ADD_SCANSUMMARY_SUM_ENTRY_START(label, htmlId) \
	{ \
		CScanSummaryEntrySumElementPtr spAvElement = new CScanSummaryEntrySumElement; \
		spAvElement->SetLabel(label); \
		spAvElement->SetHtmlId(htmlId); \

#define ADD_SCANSUMMARY_SUM_ENTRY_ITEM(dataId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
		spAvElement->AddData(spData); \
	} \

#define ADD_SCANSUMMARY_SUM_ENTRY_END() \
		spAvElement->Render(spParent); \
	} \

#define ADD_SCANSUMMARY_SUB_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
	CScanSummaryEntryElementPtr spAvElement = new CScanSummaryEntryElement; \
	spAvElement->SetData(spData, fmt); \
	spAvElement->SetLabel(label); \
	spAvElement->SetHtmlId(htmlId); \
	spAvElement->Render(spParent); \
	}

#define ADD_SCANSUMMARY_TIMESPAN_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
	CScanSummaryTimeSpanEntryElementPtr spAvElement = new CScanSummaryTimeSpanEntryElement; \
	spAvElement->SetData(spData, fmt); \
	spAvElement->SetLabel(label); \
	spAvElement->SetHtmlId(htmlId); \
	spAvElement->Render(spParent); \
	}

#define ADD_SUMMARY_RISK_LEVEL_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
	CSummaryEntryRiskLevelElementPtr spAvElement = new CSummaryEntryRiskLevelElement; \
	spAvElement->SetData(spData, fmt); \
	spAvElement->SetLabel(label); \
	spAvElement->SetHtmlId(htmlId); \
	spAvElement->Render(spParent); \
	}

#define ADD_SUMMARY_PATH_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
    hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
    { \
		CSummaryEntryCompactPathElementPtr spAvElement = new CSummaryEntryCompactPathElement; \
		spAvElement->SetData(spData, fmt); \
		spAvElement->SetLabel(label); \
		spAvElement->SetHtmlId(htmlId); \
		spAvElement->Render(spParent); \
    }

#define ADD_SUMMARY_RECOMMENDED_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
		CSummaryEntryRecommendedElementPtr spAvElement = new CSummaryEntryRecommendedElement; \
		spAvElement->SetData(spData, fmt); \
		spAvElement->SetLabel(label); \
		spAvElement->SetHtmlId(htmlId); \
		spAvElement->Render(spParent); \
	}

#define END_SUMMARY_SECTION()

#define START_ADVDETAILS_SECTION() \
    CAdvancedDetailsPageElementPtr spAdvancedDetailsPage = new CAdvancedDetailsPageElement; \
    spAdvancedDetailsPage->Render(pBodyElement); \
    symhtml::ISymHTMLElementPtr spParent; \
    hr = spRootElement->GetElementByID("AdvDetailsGradientLeft", &spParent); \
    if(SUCCEEDED(hr) && spParent) \
    { \
        CDetailsSectionHeaderElementPtr spHeader = new CDetailsSectionHeaderElement; \
        spHeader->SetHtmlId(L"section-header-details"); \
        ccLib::CString sTitle; \
        sTitle.LoadString(IDS_ADVDETAILS_TILE); \
        spHeader->SetHeaderText(sTitle); \
        spHeader->Render(spParent); \
    } \
    spParent.Release(); \
	hr = spRootElement->GetElementByID("page-element-section-left-sub-body", &spParent); \
    if(SUCCEEDED(hr) && spParent) \
    { \
        CTableElementPtr spHeader = new CTableElement; \
        spHeader->SetHtmlId(L"elementid-desc-table"); \
        spHeader->SetClassId(L"AdvDetailsTable"); \
        spHeader->Render(spParent); \
    } \
	spParent.Release(); \
    hr = spRootElement->GetElementByID("elementid-desc-table", &spParent); \
    if(SUCCEEDED(hr) && spParent) \
    { \
        ui::IDataPtr spData;

#define ADD_ADVDETAILS_ENTRY(dataId, fmt, label, htmlId) \
    spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
    { \
        CAdvDetailsEntryElementPtr spAvElement = new CAdvDetailsEntryElement; \
        spAvElement->SetData(spData, fmt); \
        spAvElement->SetLabel(label); \
        spAvElement->SetHtmlId(htmlId); \
        spAvElement->Render(spParent); \
    }

#define ADD_ADVDETAILS_TIMESPAN_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
	CAdvDetailsEntryTimeSpanElementPtr spAvElement = new CAdvDetailsEntryTimeSpanElement; \
	spAvElement->SetData(spData, fmt); \
	spAvElement->SetLabel(label); \
	spAvElement->SetHtmlId(htmlId); \
	spAvElement->Render(spParent); \
	}

#define ADD_ADVDETAILS_STRINGLIST_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
	CAdvDetailsEntryStringListElementPtr spAvElement = new CAdvDetailsEntryStringListElement; \
	spAvElement->SetData(spData, fmt); \
	spAvElement->SetLabel(label); \
	spAvElement->SetHtmlId(htmlId); \
	spAvElement->Render(spParent); \
	}

#define ADD_ADVDETAILS_RISK_LEVEL_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
	CAdvDetailsEntryRiskLevelElementPtr spAvElement = new CAdvDetailsEntryRiskLevelElement; \
	spAvElement->SetData(spData, fmt); \
	spAvElement->SetLabel(label); \
	spAvElement->SetHtmlId(htmlId); \
	spAvElement->Render(spParent); \
	}

#define ADD_ADVDETAILS_PATH_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
    hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
    { \
        CAdvDetailsCompactPathEntryElementPtr spAvElement = new CAdvDetailsCompactPathEntryElement; \
        spAvElement->SetData(spData, fmt); \
        spAvElement->SetLabel(label); \
        spAvElement->SetHtmlId(htmlId); \
        spAvElement->Render(spParent); \
    }

#define ADD_ADVDETAILS_SUM_ENTRY_START(label, htmlId) \
	{ \
	CAdvDetailsSumEntryElementPtr spAvElement = new CAdvDetailsSumEntryElement; \
	spAvElement->SetLabel(label); \
	spAvElement->SetHtmlId(htmlId); \

#define ADD_ADVDETAILS_SUM_ENTRY_ITEM(dataId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
	spAvElement->AddData(spData); \
	} \

#define ADD_ADVDETAILS_SUM_ENTRY_END() \
	spAvElement->Render(spParent); \
	} \


#define ADD_ADVDETAILS_SUM_SUBENTRY_START(label, htmlId) \
	{ \
	CAdvDetailsSubSumEntryElementPtr spAvElement = new CAdvDetailsSubSumEntryElement; \
	spAvElement->SetLabel(label); \
	spAvElement->SetHtmlId(htmlId); \

#define ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(dataId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
	spAvElement->AddData(spData); \
	} \

#define ADD_ADVDETAILS_SUM_SUBENTRY_END() \
	spAvElement->Render(spParent); \
	} \


#define END_ADVDETAILS_SECTION() \
	spParent.Release(); \
	}

#define START_TROUBLESHOOT_SECTION(parentElementId, headerElementId, headerText) \
	spParent.Release(); \
	hr = spRootElement->GetElementByID(parentElementId, &spParent); \
	if(S_OK == hr) \
	{ \
		CDetailsSectionHeaderElementPtr spHeader = new CDetailsSectionHeaderElement; \
		spHeader->SetHtmlId(headerElementId); \
		spHeader->SetHeaderText(headerText); \
		spHeader->Render(spParent); \
	}

#define START_TROUBLESHOOT_SUB_SECTION(dataId, parentElementId, headerElementId, headerText) \
    spParent.Release(); \
	hr = spRootElement->GetElementByID(CW2A(parentElementId), &spParent); \
    if(S_OK == hr) \
    { \
        ui::IDataPtr spData; \
        hr = m_spMessage->GetData(dataId, spData); \
        if(S_OK == hr) \
        { \
            CTroubleshootSectionHeaderElementPtr spTroubleshoot = new CTroubleshootSectionHeaderElement; \
            spTroubleshoot->SetData(spData); \
            spTroubleshoot->SetHtmlId(headerElementId); \
            spTroubleshoot->SetHeaderText(headerText); \
            hr = spTroubleshoot->Render(spParent); \
            if(S_OK == hr) \
            { \
                ui::IDataPtr spData; \
                spParent.Release(); \
                hr = spRootElement->GetElementByID(CW2A(headerElementId), &spParent); \
        
#define ADD_TROUBLESHOOT_ENTRY_URL(dataId, fmt, targetDataId, targetFmt, htmlId) \
    spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
    { \
        CAdvDetailsTroubleshootUrlElementPtr spEntry = new CAdvDetailsTroubleshootUrlElement; \
        spEntry->SetHtmlId(htmlId); \
        spEntry->SetData(spData, fmt); \
        spData.Release(); \
        hr = m_spMessage->GetData(targetDataId, spData); \
        if(S_OK == hr) \
        { \
            spEntry->SetTargetData(spData, targetFmt); \
            spEntry->Render(spParent); \
            m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
        } \
    }

#define ADD_TROUBLESHOOT_ENTRY_HELP(dataId, fmt, targetDataId, htmlId) \
    spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
    { \
		CAdvDetailsTroubleshootHelpElementPtr spEntry = new CAdvDetailsTroubleshootHelpElement; \
		spEntry->SetHtmlId(htmlId); \
		spEntry->SetData(spData, fmt); \
		spEntry->SetSymHelpId(targetDataId); \
		spEntry->Render(spParent); \
		m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
    }

#define ADD_TROUBLESHOOT_ENTRY_COMP_OPTIONS(dataId, fmt, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
		CAdvDetailsTroubleshootComponentOptionsElementPtr spEntry = new CAdvDetailsTroubleshootComponentOptionsElement; \
		spEntry->SetHtmlId(htmlId); \
		spEntry->SetData(spData, fmt); \
		spEntry->m_spMessage = m_spMessage; \
		spEntry->Render(spParent); \
		m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
	}

#define ADD_TROUBLESHOOT_ENTRY_ELEMENT_LAUNCH(dataId, fmt, targetDataId, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
	{ \
		CAdvDetailsTroubleshootOptionsElementPtr spEntry = new CAdvDetailsTroubleshootOptionsElement; \
		spEntry->SetHtmlId(htmlId); \
		spEntry->SetData(spData, fmt); \
		spEntry->SetISSharedElementId(targetDataId); \
		spEntry->Render(spParent); \
		m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
	}

#define END_TROUBLESHOOT_SUB_SECTION() \
            }\
        }\
    }

#define END_TROUBLESHOOT_SECTION()

#define START_ACTION_SECTION(parentElementId, headerElementId, headerTextResID) \
    spParent.Release(); \
	hr = spRootElement->GetElementByID(parentElementId, &spParent); \
    if(S_OK == hr) \
        { \
        ccLib::CString sHeader; \
        sHeader.LoadString(headerTextResID); \
        CDetailsSectionHeaderElementPtr spHeader = new CDetailsSectionHeaderElement; \
        spHeader->SetHtmlId(headerElementId); \
        spHeader->SetHeaderText(sHeader); \
        spHeader->Render(spParent); \
        ui::IDataPtr spData; \
        hr = spRootElement->GetElementByID("page-element-actions-group", &spParent);

#define ADD_NO_ACTIONS_TEXT(htmlId, label) \
        { \
			CActionSectionTextElementPtr spEntry = new CActionSectionTextElement; \
			spEntry->SetHtmlId(htmlId); \
			spEntry->SetLabel(label); \
            spEntry->m_spMessage = m_spMessage; \
			spEntry->Render(spParent); \
		}

#define ADD_RECOMMENDED_ACTIONS_TEXT(htmlId, label) \
		{ \
		CActionSectionRecommendedTextElementPtr spEntry = new CActionSectionRecommendedTextElement; \
		spEntry->SetHtmlId(htmlId); \
		spEntry->SetLabel(label); \
		spEntry->m_spMessage = m_spMessage; \
		spEntry->Render(spParent); \
		}

#define ADD_ACTION_RESTORE_ENTRY(dataId, fmt, label, htmlId) \
    spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
        { \
        CAdvDetailsActionRestoreElementPtr spEntry = new CAdvDetailsActionRestoreElement; \
        spEntry->SetHtmlId(htmlId); \
        spEntry->SetLabel(label); \
        spEntry->SetData(spData, fmt); \
        spEntry->SetDocument(this); \
        spEntry->m_spMessage = m_spMessage; \
        spEntry->m_spMessageProviderSink = m_spMessageProviderSink; \
        spEntry->Render(spParent); \
        m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
        }

#define ADD_ACTION_DELETE_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
    hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
        { \
        CAdvDetailsActionDeleteElementPtr spEntry = new CAdvDetailsActionDeleteElement; \
        spEntry->SetHtmlId(htmlId); \
        spEntry->SetLabel(label); \
        spEntry->SetData(spData, fmt); \
        spEntry->SetDocument(this); \
        spEntry->m_spMessage = m_spMessage; \
        spEntry->m_spMessageProviderSink = m_spMessageProviderSink; \
        spEntry->Render(spParent); \
        m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
        }

#define ADD_ACTION_QUICKSCAN_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
    hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
        { \
        CAdvDetailsActionLaunchQuickScanElementPtr spEntry = new CAdvDetailsActionLaunchQuickScanElement; \
        spEntry->SetHtmlId(htmlId); \
        spEntry->SetLabel(label); \
        spEntry->SetData(spData, fmt); \
        spEntry->SetDocument(this); \
        spEntry->m_spMessage = m_spMessage; \
        spEntry->m_spMessageProviderSink = m_spMessageProviderSink; \
        spEntry->Render(spParent); \
        m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
        }

#define ADD_ACTION_REMOVENOW_ENTRY(dataId, fmt, label, htmlId) \
    spData.Release(); \
    hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
        { \
        CAdvDetailsActionRemoveElementPtr spEntry = new CAdvDetailsActionRemoveElement; \
        spEntry->SetHtmlId(htmlId); \
        spEntry->SetLabel(label); \
        spEntry->SetData(spData, fmt); \
        spEntry->SetDocument(this); \
        spEntry->m_spMessage = m_spMessage; \
        spEntry->m_spMessageProviderSink = m_spMessageProviderSink; \
        spEntry->Render(spParent); \
        m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
        }

#define ADD_ACTION_REVIEW_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
	hr = m_spMessage->GetData(dataId, spData); \
	if(S_OK == hr) \
		{ \
		CAdvDetailsActionReviewRiskDetailsElementPtr spEntry = new CAdvDetailsActionReviewRiskDetailsElement; \
		spEntry->SetHtmlId(htmlId); \
		spEntry->SetLabel(label); \
		spEntry->SetData(spData, fmt); \
		spEntry->SetDocument(this); \
		spEntry->m_spMessage = m_spMessage; \
		spEntry->m_spMessageProviderSink = m_spMessageProviderSink; \
		spEntry->Render(spParent); \
		m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
		}

#define ADD_ACTION_EXCLUDE_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
    hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
        { \
        CAdvDetailsActionExcludeElementPtr spEntry = new CAdvDetailsActionExcludeElement; \
        spEntry->SetHtmlId(htmlId); \
        spEntry->SetLabel(label); \
        spEntry->SetData(spData, fmt); \
        spEntry->SetDocument(this); \
        spEntry->m_spMessage = m_spMessage; \
        spEntry->m_spMessageProviderSink = m_spMessageProviderSink; \
        spEntry->Render(spParent); \
        m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
        }

#define ADD_ACTION_SUBMIT_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
    hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
        { \
        CAdvDetailsActionSubmitElementPtr spEntry = new CAdvDetailsActionSubmitElement; \
        spEntry->SetHtmlId(htmlId); \
        spEntry->SetLabel(label); \
        spEntry->SetData(spData, fmt); \
		spEntry->SetDocument(this); \
        spEntry->m_spMessage = m_spMessage; \
        spEntry->Render(spParent); \
        m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
        }

#define ADD_ADVDETAILS_SHOWTHREATDETAILS_ENTRY(dataId, fmt, label, htmlId) \
	spData.Release(); \
    hr = m_spMessage->GetData(dataId, spData); \
    if(S_OK == hr) \
    { \
        CAdvDetailsShowThreatDetailsElementPtr spEntry = new CAdvDetailsShowThreatDetailsElement; \
        spEntry->SetHtmlId(htmlId); \
        spEntry->SetLabel(label); \
        spEntry->SetData(spData, fmt); \
		spEntry->m_spMessage = m_spMessage; \
        spEntry->Render(spParent); \
        m_lstActions.push_back((CRefCountSymHtmlActionBase*)spEntry.m_p); \
    }

#define END_ACTION_SECTION() \
    spParent.Release(); \
    }

#define END_PAGE() 


//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CMessageDetails::CMessageDetails(void)
{
}

CMessageDetails::~CMessageDetails(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CMessageDetails::OnHtmlEvent( symhtml::ISymHTMLElement* lpElement, LPCWSTR szEventName, UINT nEventType, ISymBase* pExtra ) throw()
{
    for(size_t nIndex = 0; nIndex < m_lstActions.size(); nIndex++)
    {
		HRESULT hr = m_lstActions[nIndex]->OnAction(nEventType, szEventName);
        if(S_FALSE != hr)
		{
			return hr;
		}
    }
    
    return __super::OnHtmlEvent(lpElement, szEventName, nEventType, pExtra);
}

//****************************************************************************
//****************************************************************************
HRESULT CMessageDetails::OnApply()
{
    CCTRCTXI0(_T("CMessageDetails::OnApply"));
    return S_OK;
}


//****************************************************************************
//****************************************************************************
HRESULT CMessageDetails::OnCancel()
{
    CCTRCTXI0(_T("CMessageDetails::OnCancel"));
    if (m_spAppServer != NULL)
    {
        m_spAppServer->Close();
    }

    m_spAppServer = NULL;
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CMessageDetails::OnClose()
{
    CCTRCTXI0(_T("CMessageDetails::OnClose"));
    if (m_spAppServer != NULL)
    {
        m_spAppServer->Close();
    }

    m_spAppServer = NULL;
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CMessageDetails::SetAppServer(MCF::IApplication* pAppServer)
{
    CCTRCTXI0(_T("CMessageDetails::SetAppServer"));
    m_spAppServer = pAppServer;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CMessageDetails::SupportsApply() throw()
{
    return S_FALSE;
}

//****************************************************************************
//****************************************************************************
HRESULT CMessageDetails::SupportsClose() throw()
{
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CMessageDetails::SupportsCancel() throw()
{
    return S_FALSE;
}

//****************************************************************************
//****************************************************************************
HRESULT CGenericDetails::OnDocumentComplete()
{
    __super::OnDocumentComplete();


    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAutoProtectDetails::OnDocumentComplete()
{
    __super::OnDocumentComplete();

    START_PAGE();    
        START_SUMMARY_SECTION()
            ADD_SUMMARY_ENTRY(AV::AVDATA_RISK_NAME, L"%s", _S(IDS_MCLABEL_RISK_NAME), L"details-risk-name");
            ADD_SUMMARY_ENTRY(AV::AVDATA_RISK_CATEGORIES_STRING, L"%s", _S(IDS_MCLABEL_RISK_CATEGORIES), L"details-risk-type");
            ADD_SUMMARY_RISK_LEVEL_ENTRY(AV::AVDATA_THREAT_MATRIX_OVERALL, L"%s", _S(IDS_MCLABEL_RISK_LEVEL), L"details-risk-impact");
            ADD_SUMMARY_ENTRY(AV::AVDATA_THREAT_DEPENDENCIES, L"%s", _S(IDS_MCLABEL_DEPENDENCIES), L"details-risk-dependencies");
            ADD_SUMMARY_ENTRY(AV::AVDATA_COMPONENT_NAME, L"%s", _S(IDS_MCLABEL_COMPONENT), L"details-component");
            ADD_SUMMARY_ENTRY(AV::AVDATA_ACTION_TAKEN, L"%s", _S(IDS_MCLABEL_ACTION_TAKEN), L"details-action-taken");
            ADD_SUMMARY_ENTRY(MCF::MCFDATA_STATUS, L"%s", _S(IDS_MCLABEL_STATUS), L"details-action-status");
            ADD_SUMMARY_ENTRY(AV::AVDATA_FILE_DESC, L"%s", _S(IDS_MCLABEL_DETAILS), L"details-details");
			ADD_SUMMARY_RECOMMENDED_ENTRY(MCF::MCFDATA_RECOMMENDED_ACTION, L"%s", _S(IDS_MCLABEL_RECOMMENDED_ACTION), L"details-details");
        END_SUMMARY_SECTION();
    END_PAGE();

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAutoProtectAdvancedDetails::OnDocumentComplete()
{
    __super::OnDocumentComplete();

    START_PAGE();    
        START_ADVDETAILS_SECTION();
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_RISK_NAME, L"%s", _S(IDS_MCLABEL_RISK_NAME), L"details-risk-name");
			ADD_ADVDETAILS_ENTRY(AV::AVDATA_RISK_CATEGORIES_STRING, L"%s", _S(IDS_MCLABEL_RISK_CATEGORIES), L"details-risk-type");
			ADD_ADVDETAILS_RISK_LEVEL_ENTRY(AV::AVDATA_THREAT_MATRIX_OVERALL, L"%s", _S(IDS_MCLABEL_RISK_LEVEL), L"details-risk-impact");
			ADD_ADVDETAILS_ENTRY(AV::AVDATA_THREAT_DEPENDENCIES, L"%s", _S(IDS_MCLABEL_DEPENDENCIES), L"details-risk-dependencies");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_COMPONENT_NAME, L"%s", _S(IDS_MCLABEL_COMPONENT), L"details-component");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_ACTION_TAKEN, L"%s", _S(IDS_MCLABEL_ACTION_TAKEN), L"details-action-taken");
            ADD_ADVDETAILS_ENTRY(MCF::MCFDATA_STATUS, L"%s", _S(IDS_MCLABEL_STATUS), L"details-action-status");
			ADD_ADVDETAILS_PATH_ENTRY(AV::AVDATA_FILE_NAME, L"%s", _S(IDS_MCLABEL_FILE_NAME), L"details-file-name");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_FILE_DESC, L"%s", _S(IDS_MCLABEL_DETAILS), L"details-details");
        END_ADVDETAILS_SECTION();

		START_TROUBLESHOOT_SECTION("AdvDetailsGradientRightBottom", L"section-header-help", _S(IDS_MCTEXT_ADVDETAILS_TROUBLESHOOT_TITLE));

            START_TROUBLESHOOT_SUB_SECTION(AV::AVDATA_SUPPORTS_HELP_AND_SUPPORT, L"page-element-section-right-bottom-sub-body", L"AdvDetailsRightBottomHelpLinkList", _S(IDS_MCTEXT_ADVDETAILS_HELP_AND_SUPPORT));
				ADD_TROUBLESHOOT_ENTRY_URL(AV::AVDATA_RISK_NAME, L"%s", AV::AVDATA_RESPONSE_THREATINFO_URL, L"%s", L"help-risk-name");
				ADD_TROUBLESHOOT_ENTRY_URL(AV::AVDATA_RISK_CATEGORIES_STRING, L"%s", AV::AVDATA_RISK_CATEGORIES_STRING, _S(IDS_MCURL_THREAT_CATEGORIES_WEB_PAGE), L"help-category");
				ADD_TROUBLESHOOT_ENTRY_HELP(AV::AVDATA_COMPONENT_NAME, _S(IDS_MCLABEL_HELP_RISK_DETECTION), NAV_CSH_DET_METH, L"help-component-name");
			END_TROUBLESHOOT_SUB_SECTION();

			START_TROUBLESHOOT_SUB_SECTION(AV::AVDATA_SUPPORTS_OPTIONS, L"page-element-section-right-bottom-sub-body", L"AdvDetailsRightBottomOptionsLinkList", _S(IDS_MCTEXT_ADVDETAILS_OPTIONS));
				ADD_TROUBLESHOOT_ENTRY_COMP_OPTIONS(AV::AVDATA_COMPONENT_NAME, L"%s", L"troubleshoot-options-component");
				ADD_TROUBLESHOOT_ENTRY_ELEMENT_LAUNCH(AV::AVDATA_COMPONENT_NAME, _S(IDS_MCLABEL_LAUNCH_EXCLUSIONS), ISShared::CLSID_NAV_OptionsVIDexclusions, L"troubleshoot-options-exclusions");
			END_TROUBLESHOOT_SUB_SECTION();

		END_TROUBLESHOOT_SECTION();

        START_ACTION_SECTION("AdvDetailsGradientRightTop", L"section-header-action", IDS_MCTEXT_ADVDETAILS_ACTIONS)
            ADD_NO_ACTIONS_TEXT(L"action-no-actions", _S(IDS_ADVDETAILS_ACTION_NOACTIONS))
        END_ACTION_SECTION()

    END_PAGE();

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsDetails::OnDocumentComplete()
{
    __super::OnDocumentComplete();

    START_PAGE();    
        START_SUMMARY_SECTION()
            
			ADD_SCANSUMMARY_ENTRY(AV::AVDATA_COMPONENT_NAME, L"%s", _S(IDS_MCLABEL_COMPONENT_NAME), L"details-component-name");
			ADD_SCANSUMMARY_ENTRY(AV::AVDATA_SCANRESULTS_TASK_NAME, L"%s", _S(IDS_MCLABEL_TASK_NAME), L"details-task-name");
			ADD_SCANSUMMARY_TIMESPAN_ENTRY(AV::AVDATA_SCANRESULTS_SCAN_TIME, _S(IDS_MCFMT_SCAN_TIME), _S(IDS_MCLABEL_SCAN_TIME), L"details-scan-time");
	
			ADD_SCANSUMMARY_SUM_ENTRY_START(_S(IDS_MCLABEL_TOTAL_ITEMS_SCANNED), L"details-items-scanned");
				ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_SCANRESULTS_ITEMS_TOTAL_SCANNED);
			ADD_SCANSUMMARY_SUM_ENTRY_END();

			ADD_SCANSUMMARY_SUM_ENTRY_START(_S(IDS_MCLABEL_STAT_RISKS_DETECTED), L"details-risks-detected");
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_VIRAL);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_VIRAL);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_MALICIOUS);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_MALICIOUS);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_RESERVED_MALICIOUS);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_RESERVED_MALICIOUS);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HEURISTIC);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_HEURISTIC);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SECURITY_RISK);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_SECURITY_RISK);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HACKTOOL);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_HACKTOOL);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SPYWARE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_SPYWARE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_TRACKWARE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_TRACKWARE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_DIALER);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_DIALER);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_REMOTE_ACCESS);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_REMOTE_ACCESS);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_ADWARE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_ADWARE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_JOKE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_JOKE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_APPHEURISTIC);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_APPHEURISTIC);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_COOKIE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_COOKIE);
			ADD_SCANSUMMARY_SUM_ENTRY_END();
			
			ADD_SCANSUMMARY_SUM_ENTRY_START(_S(IDS_MCLABEL_STAT_RISKS_RESOLVED), L"details-risks-removed");
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_VIRAL);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_MALICIOUS);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_RESERVED_MALICIOUS);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HEURISTIC);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SECURITY_RISK);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HACKTOOL);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SPYWARE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_TRACKWARE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_DIALER);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_REMOTE_ACCESS);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_ADWARE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_JOKE);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_APPHEURISTIC);
                ADD_SCANSUMMARY_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_COOKIE);
			ADD_SCANSUMMARY_SUM_ENTRY_END();

			ADD_SUMMARY_RECOMMENDED_ENTRY(MCF::MCFDATA_RECOMMENDED_ACTION, L"%s", _S(IDS_MCLABEL_RECOMMENDED_ACTION), L"details-recommended-action");
		
		END_SUMMARY_SECTION();
    END_PAGE();

	cc::IStringPtr spHtmlString;
	spRootElement->GetElementHtml(&spHtmlString, TRUE);

	CCTRACEI( CCTRCTX _T("%s"), CW2T(spHtmlString->GetStringW()) );

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsAdvancedDetails::OnDocumentComplete()
{
    __super::OnDocumentComplete();
	CThreatCatInfo cThreatCatInfo;

	TCHAR szThreatCatList[ccEraser::IAnomaly::Last_Category][512];
	for(int nIndex = ccEraser::IAnomaly::Viral; nIndex < ccEraser::IAnomaly::Last_Category; nIndex++)
	{
		cThreatCatInfo.GetCategoryText(nIndex, szThreatCatList[nIndex], 512);
	}
	
	START_PAGE();
        START_ADVDETAILS_SECTION();
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_COMPONENT_NAME, L"%s",  _S(IDS_MCLABEL_COMPONENT_NAME), L"details-component-name");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_SCANRESULTS_TASK_NAME, L"%s",  _S(IDS_MCLABEL_TASK_NAME), L"details-task-name");
			ADD_ADVDETAILS_TIMESPAN_ENTRY(AV::AVDATA_SCANRESULTS_SCAN_TIME, _S(IDS_MCFMT_SCAN_TIME), _S(IDS_MCLABEL_SCAN_TIME), L"details-scan-time");
			
			ADD_ADVDETAILS_ENTRY(AV::AVDATA_SCANRESULTS_ITEMS_TOTAL_SCANNED, L"%d", _S(IDS_MCLABEL_TOTAL_ITEMS_SCANNED), L"details-total-items-scanned");
			ADD_ADVDETAILS_SUM_SUBENTRY_START(_S(IDS_MCLABEL_STAT_FILES_AND_DIRECTORIES), L"details-files-and-dirs");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_FILE_DETECTION_ACTION);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_DIRECTORY_DETECTION_ACTION);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(_S(IDS_MCLABEL_STAT_REGISTRY_ITEMS), L"details-registry-items");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_REGISTRY_DETECTION_ACTION);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(_S(IDS_MCLABEL_STAT_PROCESSES_AND_STARTUP_ITEMS), L"details-startup-items");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_PROCESS_DETECTION_ACTION);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_SERVICE_DETECTION_ACTION);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_COM_DETECTION_ACTION);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_STARTUP_DETECTION_ACTION);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(_S(IDS_MCLABEL_STAT_BROWSER_NET), L"details-browser-network");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_BROWSER_CACHE_DETECTION_ACTION);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_COOKIE_DETECTION_ACTION);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_HOSTS_DETECTION_ACTION);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_LSP_DETECTION_ACTION);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
				ADD_ADVDETAILS_SUM_SUBENTRY_START(_S(IDS_MCLABEL_STAT_APPHEURISTIC), L"details-app-heuristic");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_APPHEURISTIC_DETECTION_ACTION);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(_S(IDS_MCLABEL_STAT_OTHER), L"details-other");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_BATCH_DETECTION_ACTION);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_ERASERSTAT_INI_DETECTION_ACTION);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();

			ADD_ADVDETAILS_SUM_ENTRY_START(_S(IDS_MCLABEL_STAT_RISKS_DETECTED), L"details-risks-detected");
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_VIRAL);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_VIRAL);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_MALICIOUS);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_MALICIOUS);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_RESERVED_MALICIOUS);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_RESERVED_MALICIOUS);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HEURISTIC);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_HEURISTIC);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SECURITY_RISK);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_SECURITY_RISK);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HACKTOOL);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_HACKTOOL);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SPYWARE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_SPYWARE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_TRACKWARE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_TRACKWARE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_DIALER);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_DIALER);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_REMOTE_ACCESS);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_REMOTE_ACCESS);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_ADWARE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_ADWARE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_JOKE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_JOKE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_APPHEURISTIC);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_APPHEURISTIC);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_COOKIE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_COOKIE);
			ADD_ADVDETAILS_SUM_ENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Viral], L"details-detected-viral");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_VIRAL);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_MALICIOUS);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_RESERVED_MALICIOUS);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HEURISTIC);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_VIRAL);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_MALICIOUS);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_RESERVED_MALICIOUS);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_HEURISTIC);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::SecurityRisk], L"details-detected-SecurityRisk");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SECURITY_RISK);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_SECURITY_RISK);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Hacktool], L"details-detected-Hacktool");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HACKTOOL);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_HACKTOOL);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::SpyWare], L"details-detected-SpyWare");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SPYWARE);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_SPYWARE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Trackware], L"details-detected-Trackware");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_TRACKWARE);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_TRACKWARE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Dialer], L"details-detected-Dialer");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_DIALER);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_DIALER);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::RemoteAccess], L"details-detected-RemoteAccess");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_REMOTE_ACCESS);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_REMOTE_ACCESS);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Adware], L"details-detected-Adware");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_ADWARE);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_ADWARE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Joke], L"details-detected-Joke");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_JOKE);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_JOKE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::ApplicationHeuristic], L"details-detected-ApplicationHeuristic");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_APPHEURISTIC);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_APPHEURISTIC);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Cookie], L"details-detected-Cookie");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_COOKIE);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_UNRESOLVEDSTATS_COOKIE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();

			ADD_ADVDETAILS_SUM_ENTRY_START(_S(IDS_MCLABEL_STAT_RISKS_RESOLVED), L"details-risks-resolved");
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_VIRAL);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_MALICIOUS);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_RESERVED_MALICIOUS);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HEURISTIC);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SECURITY_RISK);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HACKTOOL);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SPYWARE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_TRACKWARE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_DIALER);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_REMOTE_ACCESS);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_ADWARE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_JOKE);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_APPHEURISTIC);
				ADD_ADVDETAILS_SUM_ENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_COOKIE);
			ADD_ADVDETAILS_SUM_ENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Viral], L"details-resolved-viral");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_VIRAL);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_MALICIOUS);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_RESERVED_MALICIOUS);
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HEURISTIC);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::SecurityRisk], L"details-resolved-SecurityRisk");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SECURITY_RISK);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Hacktool], L"details-resolved-Hacktool");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_HACKTOOL);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::SpyWare], L"details-resolved-SpyWare");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_SPYWARE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Trackware], L"details-resolved-Trackware");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_TRACKWARE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Dialer], L"details-resolved-Dialer");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_DIALER);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::RemoteAccess], L"details-resolved-RemoteAccess");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_REMOTE_ACCESS);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Adware], L"details-resolved-Adware");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_ADWARE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Joke], L"details-resolved-Joke");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_JOKE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::ApplicationHeuristic], L"details-resolved-ApplicationHeuristic");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_APPHEURISTIC);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();
			ADD_ADVDETAILS_SUM_SUBENTRY_START(szThreatCatList[ccEraser::IAnomaly::Cookie], L"details-resolved-Cookie");
				ADD_ADVDETAILS_SUM_SUBENTRY_ITEM(AV::AVDATA_RESOLVEDSTATS_COOKIE);
			ADD_ADVDETAILS_SUM_SUBENTRY_END();

			END_ADVDETAILS_SECTION();

		START_TROUBLESHOOT_SECTION("AdvDetailsGradientRightBottom", L"section-header-help", _S(IDS_MCTEXT_ADVDETAILS_TROUBLESHOOT_TITLE));

			START_TROUBLESHOOT_SUB_SECTION(AV::AVDATA_SUPPORTS_HELP_AND_SUPPORT, L"page-element-section-right-bottom-sub-body", L"AdvDetailsRightBottomHelpLinkList", _S(IDS_MCTEXT_ADVDETAILS_HELP_AND_SUPPORT));
				ADD_TROUBLESHOOT_ENTRY_HELP(AV::AVDATA_COMPONENT_NAME, _S(IDS_MCLABEL_HELP_RISK_DETECTION), NAV_CSH_DET_METH, L"help-component-name");
			END_TROUBLESHOOT_SUB_SECTION();

			START_TROUBLESHOOT_SUB_SECTION(AV::AVDATA_SUPPORTS_OPTIONS, L"page-element-section-right-bottom-sub-body", L"AdvDetailsRightBottomOptionsLinkList", _S(IDS_MCTEXT_ADVDETAILS_OPTIONS));
				ADD_TROUBLESHOOT_ENTRY_COMP_OPTIONS(AV::AVDATA_COMPONENT_NAME, L"%s", L"troubleshoot-options-component");
				ADD_TROUBLESHOOT_ENTRY_ELEMENT_LAUNCH(AV::AVDATA_COMPONENT_NAME, _S(IDS_MCLABEL_LAUNCH_EXCLUSIONS), ISShared::CLSID_NAV_OptionsVIDexclusions, L"troubleshoot-options-exclusions");
			END_TROUBLESHOOT_SUB_SECTION();

		END_TROUBLESHOOT_SECTION();

        START_ACTION_SECTION("AdvDetailsGradientRightTop", L"section-header-action", IDS_MCTEXT_ADVDETAILS_ACTIONS)
            ADD_NO_ACTIONS_TEXT(L"action-no-actions", _S(IDS_ADVDETAILS_ACTION_NOACTIONS))
        END_ACTION_SECTION()

    END_PAGE();

    cc::IStringPtr spHtmlString;
    spRootElement->GetElementHtml(&spHtmlString, TRUE);

    CCTRACEI( CCTRCTX _T("%s"), CW2T(spHtmlString->GetStringW()) );

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAvModuleItemDetails::OnDocumentComplete()
{
    __super::OnDocumentComplete();

    START_PAGE();    
        START_SUMMARY_SECTION()
            ADD_SUMMARY_ENTRY(AV::AVDATA_RISK_NAME, L"%s", _S(IDS_MCLABEL_RISK_NAME), L"details-risk-name");
            ADD_SUMMARY_ENTRY(AV::AVDATA_RISK_CATEGORIES_STRING, L"%s", _S(IDS_MCLABEL_RISK_CATEGORIES), L"details-threat-categories");
			ADD_SUMMARY_RISK_LEVEL_ENTRY(AV::AVDATA_THREAT_MATRIX_OVERALL, L"%s", _S(IDS_MCLABEL_RISK_LEVEL), L"details-risk-impact");
            ADD_SUMMARY_ENTRY(AV::AVDATA_COMPONENT_NAME, L"%s", _S(IDS_MCLABEL_COMPONENT), L"details-component");
            ADD_SUMMARY_ENTRY(AV::AVDATA_COMPONENT_VERSION, L"%s", _S(IDS_MCLABEL_COMPONENT_VERSION), L"details-component-version");
            ADD_SUMMARY_ENTRY(AV::AVDATA_THREAT_STATE_NAME, L"%s", _S(IDS_MCLABEL_THREAT_STATE), L"details-component-version");
			ADD_SUMMARY_RECOMMENDED_ENTRY(MCF::MCFDATA_RECOMMENDED_ACTION, L"%s", _S(IDS_MCLABEL_RECOMMENDED_ACTION), L"details-details");
        END_SUMMARY_SECTION();
    END_PAGE();

    cc::IStringPtr spHtmlString;
    spRootElement->GetElementHtml(&spHtmlString, TRUE);

    CCTRACEI( CCTRCTX _T("%s"), CW2T(spHtmlString->GetStringW()) );

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAvModuleItemAdvancedDetails::OnDocumentComplete()
{
    __super::OnDocumentComplete();
		
    int safe_mode = ::GetSystemMetrics(SM_CLEANBOOT);

    START_PAGE();
        START_ADVDETAILS_SECTION();
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_COMPONENT_NAME, L"%s",  _S(IDS_MCLABEL_COMPONENT), L"details-component");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_COMPONENT_VERSION, L"%s",  _S(IDS_MCLABEL_RISK_NAME), L"details-risk-name");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_DEFINITIONS_VERSION, L"%s",  _S(IDS_MCLABEL_DEFINITIONS_VERSION), L"details-def-ver");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_ERASER_VERSION, L"%s", _S(IDS_MCLABEL_ERASER_VERSION), L"details-eraser-ver");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_RISK_NAME, L"%s", _S(IDS_MCLABEL_RISK_NAME), L"details-risk-name");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_RISK_CATEGORIES_STRING, L"%s",  _S(IDS_MCLABEL_RISK_CATEGORIES), L"details-risk-categories");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_OBJECT_TYPE_NAME, L"%s",  _S(IDS_MCLABEL_RISK_TYPE), L"details-risk-object-type");
			ADD_ADVDETAILS_RISK_LEVEL_ENTRY(AV::AVDATA_THREAT_MATRIX_OVERALL, L"%s", _S(IDS_MCLABEL_RISK_LEVEL), L"details-risk-impact");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_HEURISTIC_RANK_STRING, L"%s",  _S(IDS_MCLABEL_APPHEUR_RANK), L"details-appheur-rank");
            ADD_ADVDETAILS_ENTRY(AV::AVDATA_THREAT_STATE_NAME, L"%s",  _S(IDS_MCLABEL_THREAT_STATE), L"details-component-version");
			ADD_ADVDETAILS_ENTRY(AV::AVDATA_THREAT_DEPENDENCIES, _S(IDS_MCTEXT_HAS_DEPENDENCY), _S(IDS_MCLABEL_DEPENDENCIES), L"details-risk-dependencies");
			ADD_ADVDETAILS_ENTRY(AV::AVDATA_EMAIL_SUBJECT, L"%s",  _S(IDS_MCLABEL_SUBJECT), L"details-email-subject");
			ADD_ADVDETAILS_ENTRY(AV::AVDATA_EMAIL_SENDER, L"%s",  _S(IDS_MCLABEL_SENDER), L"details-email-sender");
			ADD_ADVDETAILS_ENTRY(AV::AVDATA_EMAIL_RECIPIENT, L"%s",  _S(IDS_MCLABEL_RECIPIENT), L"details-email-recipient");
            ADD_ADVDETAILS_SHOWTHREATDETAILS_ENTRY(AV::AVDATA_THREATTRACK_ID, _S(IDS_MCLABEL_VIEW_DETAILS), _S(IDS_MCTEXT_VIEW_THREAT_DETAILS), L"details-threat-details");
			ADD_ADVDETAILS_STRINGLIST_ENTRY(AV::AVDATA_FILE_LIST, L"%s",  _S(IDS_MCLABEL_AFFECTED_FILE), L"details-file-list");
        END_ADVDETAILS_SECTION()

		START_TROUBLESHOOT_SECTION("AdvDetailsGradientRightBottom", L"section-header-help", _S(IDS_MCTEXT_ADVDETAILS_TROUBLESHOOT_TITLE));

            START_TROUBLESHOOT_SUB_SECTION(AV::AVDATA_SUPPORTS_HELP_AND_SUPPORT, L"page-element-section-right-bottom-sub-body", L"AdvDetailsRightBottomHelpLinkList", _S(IDS_MCTEXT_ADVDETAILS_HELP_AND_SUPPORT));
				ADD_TROUBLESHOOT_ENTRY_URL(AV::AVDATA_RISK_NAME, L"%s", AV::AVDATA_RESPONSE_THREATINFO_URL, L"%s", L"help-risk-name");
				ADD_TROUBLESHOOT_ENTRY_URL(AV::AVDATA_RISK_CATEGORIES_STRING, L"%s", AV::AVDATA_RISK_CATEGORIES_STRING, _S(IDS_MCURL_THREAT_CATEGORIES_WEB_PAGE), L"help-category");
				ADD_TROUBLESHOOT_ENTRY_HELP(AV::AVDATA_COMPONENT_NAME, _S(IDS_MCLABEL_HELP_RISK_DETECTION), NAV_CSH_DET_METH, L"help-component-name");
				ADD_TROUBLESHOOT_ENTRY_HELP(AV::AVDATA_COMPONENT_NAME, _S(IDS_MCLABEL_HELP_MANAGE_QUARITEMS), NAVW_MANAGE_SEC_HIST_QUAR, L"help-manage-qitems");
			END_TROUBLESHOOT_SUB_SECTION();

            START_TROUBLESHOOT_SUB_SECTION(AV::AVDATA_SUPPORTS_OPTIONS, L"page-element-section-right-bottom-sub-body", L"AdvDetailsRightBottomOptionsLinkList", _S(IDS_MCTEXT_ADVDETAILS_OPTIONS));
				ADD_TROUBLESHOOT_ENTRY_COMP_OPTIONS(AV::AVDATA_COMPONENT_NAME, L"%s", L"troubleshoot-options-component");
				ADD_TROUBLESHOOT_ENTRY_ELEMENT_LAUNCH(AV::AVDATA_COMPONENT_NAME, _S(IDS_MCLABEL_LAUNCH_EXCLUSIONS), ISShared::CLSID_NAV_OptionsVIDexclusions, L"troubleshoot-options-exclusions");
			END_TROUBLESHOOT_SUB_SECTION();

		END_TROUBLESHOOT_SECTION();

        START_ACTION_SECTION("AdvDetailsGradientRightTop", L"section-header-action", IDS_MCTEXT_ADVDETAILS_ACTIONS)
            ADD_ACTION_RESTORE_ENTRY(AV::AVDATA_THREATTRACK_ID, L"%s", _S(IDS_MCLABEL_RESTORE_THIS_THREAT), L"action-restore-risk");
		    ADD_ACTION_REVIEW_ENTRY(AV::AVDATA_THREATTRACK_ID, L"%s", _S(IDS_MCLABEL_REVIEW_RISK_DETAILS), L"action-review-risk-details");
            ADD_ACTION_DELETE_ENTRY(AV::AVDATA_THREATTRACK_ID, L"%s", _S(IDS_MCLABEL_DELETE_THIS_THREAT), L"action-delete-risk");
            ADD_ACTION_REMOVENOW_ENTRY(AV::AVDATA_THREATTRACK_ID, L"%s", _S(IDS_MCLABEL_REMOVENOW), L"action-fix-now");
            ADD_ACTION_QUICKSCAN_ENTRY(AV::AVDATA_THREATTRACK_ID, L"%s", _S(IDS_MCLABEL_LAUNCH_QUICKSCAN), L"action-launch-quickscan");
            ADD_ACTION_EXCLUDE_ENTRY(AV::AVDATA_THREATTRACK_ID, L"%s", _S(IDS_MCLABEL_EXCLUDE_THIS_THREAT), L"action-exclude");
			
			if(safe_mode == 0) //normal boot
				ADD_ACTION_SUBMIT_ENTRY(AV::AVDATA_THREATTRACK_ID, L"%s", _S(IDS_MCLABEL_SUBMIT_TO_SYMANTEC), L"action-submit-risk");
			ADD_RECOMMENDED_ACTIONS_TEXT(L"action-recommended-action", _S(IDS_MCLABEL_RECOMMENDED_ACTION_SUBTEXT))
            ADD_NO_ACTIONS_TEXT(L"action-no-actions", _S(IDS_ADVDETAILS_ACTION_NOACTIONS))
        END_ACTION_SECTION()

    END_PAGE();

    cc::IStringPtr spHtmlString;
    spRootElement->GetElementHtml(&spHtmlString, TRUE);

    CCTRACEI( CCTRCTX _T("%s"), CW2T(spHtmlString->GetStringW()) );

    return S_OK;
}
