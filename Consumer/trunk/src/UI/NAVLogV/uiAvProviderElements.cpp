// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "uiAvProviderElements.h"

#include <shellapi.h>

#include "ISVersion.h"
#include "uiNumberDataInterface.h"
#include "uiStringDataInterface.h"
#include "uiDateDataInterface.h"
#include "AVSubmissionLoader.h"
#include "AVInterfaceLoader.h"
#include "QBackupLoader.h"
#include "..\navlogvres\resource.h"
#include "mcfWIdgets.h"
#include "avQBackupTypes.h"
#include "Message.h"

#include <ccEraserInterface.h>

#include <ScanTaskLoader.h>
#include <switches.h>
#include <NavInfo.h>

#include <SymHtmlEventMapEntry.h>
#include <ScanUILoader.h>
#include "Atlctrlx.h"
#include <AvDataIds.h>

#include <isDataClientLoader.h>
#include <uiNISDataElementGuids.h>
#include <AVccModuleID.h>

#include "ccCoInitialize.h"
#include <StockLists.h> // MCF

#include <ccInstanceFactory.h>

#define MAX_LINE_ELEMENT_PATH_NAME 50
#define MAX_ADVDETAILS_ELEMENT_PATH_NAME 50


#define PAGE_ELEMENT_SUMMARY_FORMAT_STRING \
	L"<div class=\"SummarySection\" id=\"page-element-summary-section\">" \
	L"<table class=\"SummaryTable\" id=\"page-element-summary-table\" cellspacing=\"0\">" \
	L"</table>" \
	L"</div>"

#define SUMMARY_RISK_LEVEL_PLUS_IMAGE_FORMAT_STRING \
	L"%1!s! <IMG class=\"SummaryRiskLevelImage\" src=\"symres://SecurityHistory/MCRES.LOC/%2!s!\"/>" \

#define SUMMARY_ENTRY_FORMAT_STRING \
	L"<tr>" \
	L"<td class=\"SummaryTableCellValue\"    id=\"summary-table-row-%1!s!\">" \
	L"<p class=\"SummaryTableCellKeyText\"   id=\"summary-table-row-key-%1!s!\">%2!s!</p>" \
	L"<p class=\"SummaryTableCellValueText\" id=\"summary-table-row-value-%1!s!\">%3!s!</p>" \
	L"</td>" \
	L"</tr>"

#define SUMMARY_ENTRY_FORMAT_PATH_STRING \
    L"<tr>" \
    L"<td class=\"SummaryTableCellValue\"    id=\"summary-table-row-%1!s!\">" \
    L"<p class=\"SummaryTableCellKeyText\"   id=\"summary-table-row-key-%1!s!\">%2!s!</p>" \
    L"<p class=\"SummaryTableCellPathValueText\" id=\"summary-table-row-value-%1!s!\">%3!s!</p>" \
    L"</td>" \
    L"</tr>"

#define SCANSUMMARY_ENTRY_FORMAT_STRING \
	L"<tr>" \
	L"<td class=\"ScanSummaryTableCellValue\"    id=\"scan-summary-table-row-%1!s!\">" \
	L"<p class=\"ScanSummaryTableCellKeyText\"   id=\"scan-summary-table-row-key-%1!s!\">%2!s!</p>" \
	L"<p class=\"ScanSummaryTableCellValueText\" id=\"scan-summary-table-row-value-%1!s!\">%3!s!</p>" \
	L"</td>" \
	L"</tr>"

#define SCANSUMMARY_SUBENTRY_FORMAT_STRING \
	L"<tr>" \
	L"<td class=\"ScanSummaryTableCellValue\"    id=\"scan-summary-table-row-%1!s!\">" \
	L"<p class=\"ScanSummarySubTableCellKeyText\"   id=\"scan-summary-table-row-key-%1!s!\">%2!s!</p>" \
	L"<p class=\"ScanSummaryTableCellValueText\" id=\"scan-summary-table-row-value-%1!s!\">%3!s!</p>" \
	L"</td>" \
	L"</tr>"

#define SUMMARY_ENTRY_RECOMMENDED_FORMAT_STRING \
	L"<tr>" \
	L"<td class=\"SummaryTableCellValue\"    id=\"summary-table-row-%1!s!\">" \
	L"<div class=\"Recommended\"             id=\"page-element-recommended\">" \
	L"<p class=\"SummaryTableCellKeyText\"   id=\"summary-table-row-key-%1!s!\">%2!s!</p>" \
	L"<p class=\"SummaryTableCellValueText\" id=\"summary-table-row-value-%1!s!\">%3!s!</p>" \
	L"</div>" \
	L"</td>" \
	L"</tr>"

#define PAGE_ELEMENT_ADVANCED_DETAILS_FORMAT_STRING \
    L"<div class=\"AdvDetailsBodyArea\">" \
    L"<div class=\"AdvDetailsAreaLeft\"         id=\"page-element-section-left\">" \
    L"<div class=\"AdvDetailsGradient\"         id=\"AdvDetailsGradientLeft\"></div>" \
	L"<div class=\"AdvDetailsAreaSubBody\"      id=\"page-element-section-left-sub-body\"></div>" \
	L"</div>" \
    L"<div class=\"AdvDetailsAreaRight\"        id=\"page-element-section-right\">" \
    L"<div class=\"AdvDetailsAreaRightTop\"     id=\"page-element-section-right-top\">" \
    L"<div class=\"AdvDetailsGradient\"         id=\"AdvDetailsGradientRightTop\"></div>" \
	L"<div class=\"AdvDetailsAreaSubBody\"      id=\"page-element-section-right-top-sub-body\">" \
    L"<div class=\"AdvDetailsAreaActionsGroup\" id=\"page-element-actions-group\"></div>" \
    L"</div>" \
	L"</div>" \
    L"<div class=\"AdvDetailsAreaRightBottom\"  id=\"page-element-section-right-bottom\">" \
    L"<div class=\"AdvDetailsGradient\"         id=\"AdvDetailsGradientRightBottom\"></div>" \
	L"<div class=\"AdvDetailsAreaSubBody\"      id=\"page-element-section-right-bottom-sub-body\"></div>" \
    L"</div>" \
    L"</div>" \
    L"</div>" \

#define SECTION_HEADER_ELEMENT_FORMAT_STRING \
    L"<DIV id=\"%1!s!\" class=\"AdvDetailsText\">" \
    L"%2!s!" \
    L"</DIV>"

#define TROUBLESHOOT_SECTION_HEADER_FORMAT_STRING \
    L"<p class=\"AdvDetailsRBottomText\">%2!s!</p>" \
    L"<UL class=\"AdvDetailsAreaRightBottomList\" id=\"%1!s!\">" \
    L"</UL>" \

#define TABLE_ELEMENT_FORMAT_STRING \
    L"<TABLE width=\"100%%\" id=\"%1!s!\" class=\"%2!s!\" cellspacing=\"0\"></TABLE>"


#define ADVDETAILS_RISK_LEVEL_PLUS_IMAGE_FORMAT_STRING \
	L"%1!s! <IMG class=\"AdvDetailsRiskLevelImage\" src=\"symres://SecurityHistory/MCRES.LOC/%2!s!\"/>" \

#define ADVSUMMARY_ENTRY_FORMAT_STRING \
    L"<TR>" \
    L"<TD class=\"AdvDetailsTableCellKey\">%2!s!</TD>" \
    L"<TD id=\"%1!s!\" class=\"AdvDetailsTableCellValue\">" \
    L"%3!s!" \
    L"</TD>" \
    L"</TR>" 

#define ADVSUMMARY_SUBENTRY_FORMAT_STRING \
	L"<TR>" \
	L"<TD class=\"AdvDetailsTableSubCellKey\">%2!s!</TD>" \
	L"<TD id=\"%1!s!\" class=\"AdvDetailsTableCellValue\">" \
	L"%3!s!" \
	L"</TD>" \
	L"</TR>" 

#define ADVSUMMARY_ENTRY_FORMAT_PATH_STRING \
    L"<TR>" \
    L"<TD class=\"AdvDetailsTableCellKey\">%2!s!</TD>" \
    L"<TD id=\"%1!s!\" class=\"AdvDetailsTableCellPathValue\">" \
    L"%3!s!" \
    L"</TD>" \
    L"</TR>" 

#define ADVDETAILS_SHOWTHREATSUMMARY_ENTRY_FORMAT_STRING \
    L"<TR>" \
    L"<TD class=\"AdvDetailsTableCellKey\">%2!s!</TD>" \
    L"<TD id=\"%1!s!-tablerow\" class=\"AdvDetailsTableCellValue\">" \
    L"<A href=\"#\" id=\"%1!s!\" class=\"troubleshoot-entry\" tabindex=\"0\">" \
    L"%3!s!" \
    L"</A>" \
    L"</TD>" \
    L"</TR>" 

#define ACTION_SECTION_TEXT_FORMAT_STRING \
    L"<P id=\"%1!s!\" class=\"AdvDetailsActionText\">" \
    L"%2!s!" \
    L"</P>"

#define ACTION_SECTION_RECOMMENDED_TEXT_FORMAT_STRING \
	L"<P id=\"%1!s!\" class=\"AdvDetailsRecommendedText\">" \
	L"%2!s!" \
	L"</P>"

#define ADVDETAILS_TROUBLESHOOT_ENTRY_FORMAT_STRING \
    L"<LI class=\"troubleshoot-list-entry\"><A href=\"#\" id=\"%1!s!\" class=\"troubleshoot-entry\" tabindex=\"0\">" \
    L"%2!s!" \
    L"</A></LI>"

#define ADVDETAILS_ACTION_ENTRY_FORMAT_STRING_OUTER \
    L"<div id=\"%1!s!-outer\">" \
	L"%2!s!" \
	L"</div>"

#define ADVDETAILS_ACTION_ENTRY_FORMAT_STRING \
	L"<BUTTON id=\"%1!s!\" width=\"100%%\" class=\"action-entry\" title=\"%2!s!\" tabindex=\"0\">%2!s!</BUTTON><BR>" \

#define ADVDETAILS_NO_ACTION_ENTRY_FORMAT_STRING \
	L"<div id=\"%1!s!\" class=\"AdvDetailsActionText\">%2!s!</div>"


HRESULT ShellExecuteWorker(HWND hwnd, LPCTSTR szOperation, LPCTSTR szFile, LPCTSTR szParameters, LPCTSTR szDirectory, INT nShowCommand);

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CStockStringListData::CStockStringListData(void)
{
	m_DataType = ui::IData::eStringList;
	MCF::CStockSymBaseList::CreateObject(&m_spStringList);
}

CStockStringListData::~CStockStringListData(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CStockStringListData::GetString(DWORD dwIndex, cc::IString*& pString) const throw()
{
	if(!m_spStringList)
		return E_UNEXPECTED;
	
	ISymBasePtr spString;
	HRESULT hr = m_spStringList->GetItem(dwIndex, spString);
	if(FAILED(hr))
		return hr;

	if(!spString)
		return E_UNEXPECTED;

	cc::ICloneQIPtr spClone = spString;
	
	ISymBasePtr spNewString;
	if(!spClone->Clone(spNewString))
		return E_UNEXPECTED;

	return MCF::HRESULT_FROM_SYMRESULT(spNewString->QueryInterface(cc::IID_String, (void**)&pString));
}

//****************************************************************************
//****************************************************************************
HRESULT CStockStringListData::AddString(LPCWSTR wszString) const throw()
{
	if(!m_spStringList)
		return E_UNEXPECTED;

	cc::IStringPtr spTempString;
	spTempString.Attach(ccSym::CStringImpl::CreateStringImpl(wszString));
	
	return AddString(spTempString);
}

//****************************************************************************
//****************************************************************************
HRESULT CStockStringListData::AddString(const cc::IString* pString) const throw()
{
	if(!m_spStringList)
		return E_UNEXPECTED;
	
	if(!pString)
		return E_INVALIDARG;

	ISymBaseQIPtr spTemp = pString;
	if(!spTemp)
		return E_INVALIDARG;

	return m_spStringList->Add(spTemp);
}

//****************************************************************************
//****************************************************************************
HRESULT CStockStringListData::GetCount(DWORD& dwCount) const throw()
{
	return m_spStringList->GetCount(dwCount);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CSummaryPageElement::CSummaryPageElement(void)
{
}

CSummaryPageElement::~CSummaryPageElement(void)
{
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
HRESULT CSummaryPageElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    return pParentNode->AppendElementHtml(PAGE_ELEMENT_SUMMARY_FORMAT_STRING, -1);
}


//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CSummaryEntryElement::CSummaryEntryElement(void)
{
}

CSummaryEntryElement::~CSummaryEntryElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;
    
    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_SUMMARY_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;

    CString cszData;
    hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
    if(FAILED(hr) || cszData.IsEmpty())
    {
        return S_FALSE;
    }

    CString cszRenderString;
    cszRenderString.FormatMessage(SUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);
    
    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CScanSummaryEntryElement::CScanSummaryEntryElement(void)
{
}

CScanSummaryEntryElement::~CScanSummaryEntryElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryEntryElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;
    
    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryEntryElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_SUMMARY_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryEntryElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryEntryElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;

    CString cszData;
    hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
    if(FAILED(hr) || cszData.IsEmpty())
    {
        return S_FALSE;
    }

    CString cszRenderString;
    cszRenderString.FormatMessage(SCANSUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);
    
    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CScanSummaryEntrySumElement::CScanSummaryEntrySumElement(void) :
m_qdwData(NULL)
{
}

CScanSummaryEntrySumElement::~CScanSummaryEntrySumElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryEntrySumElement::AddData(ui::IData* pData)
{
    if(!pData)
        return E_FAIL;

	ui::INumberDataQIPtr spNumberData = pData;
	if(!spNumberData)
		return E_INVALIDARG;

	LONGLONG qdwNumber = 0;
	spNumberData->GetNumber(qdwNumber);
	m_qdwData += qdwNumber;
    
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryEntrySumElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_SUMMARY_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryEntrySumElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryEntrySumElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    CString cszData;
	cszData.Format(_T("%I64d"), m_qdwData);

	CString cszRenderString;
    cszRenderString.FormatMessage(SCANSUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);
    
    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CScanSummaryTimeSpanEntryElement::CScanSummaryTimeSpanEntryElement(void)
{
}

CScanSummaryTimeSpanEntryElement::~CScanSummaryTimeSpanEntryElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryTimeSpanEntryElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;
    
    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryTimeSpanEntryElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_SUMMARY_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryTimeSpanEntryElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanSummaryTimeSpanEntryElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
	ui::IDateDataQIPtr spTimeSpan = m_spData;
	if(!m_spData)
		return E_INVALIDARG;

	SYSTEMTIME stTimeSpan;
	hr = spTimeSpan->GetDate(stTimeSpan);
	if(FAILED(hr))
		return E_UNEXPECTED;

	SYSTEMTIME stZeroTime;
	ZeroMemory(&stZeroTime, sizeof(SYSTEMTIME));

	CTimeSpan ctTimeSpan = CTime(stTimeSpan) - CTime(stZeroTime);
	CString cszData;
	cszData = ctTimeSpan.Format(m_cszFormat);

    CString cszRenderString;
    cszRenderString.FormatMessage(SCANSUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);
    
    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CSummaryEntryRecommendedElement::CSummaryEntryRecommendedElement(void)
{
}

CSummaryEntryRecommendedElement::~CSummaryEntryRecommendedElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryRecommendedElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
	if(!pData)
		return E_FAIL;

	m_spData = pData;

	if(szFormat)
		m_cszFormat = szFormat;

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryRecommendedElement::SetLabel(LPCWSTR szLabel)
{
	m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_SUMMARY_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryRecommendedElement::SetHtmlId(LPCWSTR szHtmlId)
{
	m_cszHtmlId = CW2T(szHtmlId);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryRecommendedElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
	HRESULT hr;

	CString cszData;
	hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
	if(FAILED(hr) || cszData.IsEmpty())
	{
		return S_FALSE;
	}

	CString cszRenderString;
	cszRenderString.FormatMessage(SUMMARY_ENTRY_RECOMMENDED_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);

	return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CSummaryEntryCompactPathElement::CSummaryEntryCompactPathElement(void)
{
}

CSummaryEntryCompactPathElement::~CSummaryEntryCompactPathElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryCompactPathElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;
    
    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryCompactPathElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_SUMMARY_SUFFIX);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryCompactPathElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryCompactPathElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    CString cszData;
    hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
    if(FAILED(hr) || cszData.IsEmpty())
    {
        return S_FALSE;
    }

    CString cszRenderString;
    cszRenderString.FormatMessage(SUMMARY_ENTRY_FORMAT_PATH_STRING, m_cszHtmlId, m_cszLabel, cszData);
    
    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CSummaryEntryRiskLevelElement::CSummaryEntryRiskLevelElement(void)
{
}

CSummaryEntryRiskLevelElement::~CSummaryEntryRiskLevelElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryRiskLevelElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
	if(!pData)
		return E_FAIL;

	m_spData = pData;

	if(szFormat)
		m_cszFormat = szFormat;

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryRiskLevelElement::SetLabel(LPCWSTR szLabel)
{
	m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_SUMMARY_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryRiskLevelElement::SetHtmlId(LPCWSTR szHtmlId)
{
	m_cszHtmlId = CW2T(szHtmlId);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CSummaryEntryRiskLevelElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
	HRESULT hr;
	CString cszRiskLevelString, cszRiskLevelImage;

	ui::INumberDataQIPtr spRiskLevel = m_spData;
	if(!spRiskLevel)
	{
		CCTRACEE( CCTRCTX _T("Problem getting risk level info."));
		return S_FALSE;
	}

	LONGLONG qdwRiskLevel = 0xFF;
	hr = spRiskLevel->GetNumber(qdwRiskLevel);
	if(FAILED(hr) || !(qdwRiskLevel >= 0 && qdwRiskLevel <= 2))
	{
		CCTRACEE( CCTRCTX _T("Invalid risk level. 0x%16I64X"), qdwRiskLevel);
		return S_FALSE;
	}

	UINT pRiskLevelMap[3] = {IDS_TEXT_LOW, IDS_TEXT_MEDIUM, IDS_TEXT_HIGH};
	LPCTSTR pszRiskLevelImages[] = {_T("LOWRISK.GIF"), _T("MEDRISK.GIF"), _T("HIGHRISK.GIF")};

	CString cszValueFmt;
	cszValueFmt.Format(m_cszFormat, _S(pRiskLevelMap[qdwRiskLevel]));

	CString cszValueString;
	cszValueString.FormatMessage(SUMMARY_RISK_LEVEL_PLUS_IMAGE_FORMAT_STRING, cszValueFmt, pszRiskLevelImages[qdwRiskLevel]);

	CString cszRenderString;
	cszRenderString.FormatMessage(SUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszValueString);

	return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvancedDetailsPageElement::CAdvancedDetailsPageElement(void)
{
}

CAdvancedDetailsPageElement::~CAdvancedDetailsPageElement(void)
{
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
HRESULT CAdvancedDetailsPageElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    return pParentNode->AppendElementHtml(PAGE_ELEMENT_ADVANCED_DETAILS_FORMAT_STRING, -1);
}


//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CDetailsSectionHeaderElement::CDetailsSectionHeaderElement(void)
{
}

CDetailsSectionHeaderElement::~CDetailsSectionHeaderElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CDetailsSectionHeaderElement::SetHtmlId(LPCWSTR wszHtmlId)
{
    m_cszHtmlId = CW2T(wszHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CDetailsSectionHeaderElement::SetHeaderText(LPCWSTR wszHeaderText)
{
    m_cszHeaderText = CW2T(wszHeaderText);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CDetailsSectionHeaderElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    CString cszRenderString;
    cszRenderString.FormatMessage(SECTION_HEADER_ELEMENT_FORMAT_STRING, m_cszHtmlId, m_cszHeaderText);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CTroubleshootSectionHeaderElement::CTroubleshootSectionHeaderElement(void)
{
}

CTroubleshootSectionHeaderElement::~CTroubleshootSectionHeaderElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CTroubleshootSectionHeaderElement::SetData(ui::IData* pData)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CTroubleshootSectionHeaderElement::SetHtmlId(LPCWSTR wszHtmlId)
{
    m_cszHtmlId = CW2T(wszHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CTroubleshootSectionHeaderElement::SetHeaderText(LPCWSTR wszHeaderText)
{
    m_cszHeaderText = CW2T(wszHeaderText);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CTroubleshootSectionHeaderElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    bool bSupportsThisSection = false;
    CFormatDataToBool::DataToBool(m_spData, bSupportsThisSection);
    if(!bSupportsThisSection)
        return S_FALSE;

    CString cszRenderString;
    cszRenderString.FormatMessage(TROUBLESHOOT_SECTION_HEADER_FORMAT_STRING, m_cszHtmlId, m_cszHeaderText);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CTableElement::CTableElement(void)
{
}

CTableElement::~CTableElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CTableElement::SetHtmlId(LPCWSTR wszHtmlId)
{
    m_cszHtmlId = CW2T(wszHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CTableElement::SetClassId(LPCWSTR wszClassId)
{
    m_cszClassId = CW2T(wszClassId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CTableElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    CString cszRenderString;
    cszRenderString.FormatMessage(TABLE_ELEMENT_FORMAT_STRING, m_cszHtmlId, m_cszClassId);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsEntryElement::CAdvDetailsEntryElement(void)
{
}

CAdvDetailsEntryElement::~CAdvDetailsEntryElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_ADVDETAILS_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;

    CString cszData;
    hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
    if(FAILED(hr) || cszData.IsEmpty())
    {
        return S_FALSE;
    }

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVSUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsEntryTimeSpanElement::CAdvDetailsEntryTimeSpanElement(void)
{
}

CAdvDetailsEntryTimeSpanElement::~CAdvDetailsEntryTimeSpanElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryTimeSpanElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryTimeSpanElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_ADVDETAILS_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryTimeSpanElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryTimeSpanElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
	HRESULT hr;
	ui::IDateDataQIPtr spTimeSpan = m_spData;
	if(!m_spData)
		return E_INVALIDARG;

	SYSTEMTIME stTimeSpan;
	hr = spTimeSpan->GetDate(stTimeSpan);
	if(FAILED(hr))
		return E_UNEXPECTED;

	SYSTEMTIME stZeroTime;
	ZeroMemory(&stZeroTime, sizeof(SYSTEMTIME));

	CTimeSpan ctTimeSpan = CTime(stTimeSpan) - CTime(stZeroTime);
	CString cszData;
	cszData = ctTimeSpan.Format(m_cszFormat);

	CString cszRenderString;
	cszRenderString.FormatMessage(ADVSUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);

	return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsEntryStringListElement::CAdvDetailsEntryStringListElement(void)
{
}

CAdvDetailsEntryStringListElement::~CAdvDetailsEntryStringListElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryStringListElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryStringListElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_ADVDETAILS_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryStringListElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryStringListElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;

	IStringListDataQIPtr spStringList = m_spData;
	if(!spStringList)
	{
		CCTRACEE( CCTRCTX _T("Problem getting string list from m_spData"));
		return S_FALSE;
	}

	CString cszRenderString;
	DWORD dwStringCount = NULL, dwStringIndex = NULL;
	hr = spStringList->GetCount(dwStringCount);
	if(FAILED(hr) || !dwStringCount)
	{
		CCTRACEE( CCTRCTX _T("Problem getting string list count. hr=0x%08X, dwStringCount=%d"), dwStringCount);
		return S_FALSE;
	}
	
	for(dwStringIndex = 0; dwStringIndex < dwStringCount; dwStringIndex++)
	{
		cc::IStringPtr spString;
		hr = spStringList->GetString(dwStringIndex, spString);
		if(FAILED(hr) || !spString)
		{
			CCTRACEE( CCTRCTX _T("Problem getting string list count. hr=0x%08X, dwStringIndex=%d"), dwStringIndex);
			continue;
		}

		CString cszHtmlId;
		cszHtmlId.Format(_T("%s-%02d"), m_cszHtmlId);

		CString cszData;
		cszData.Format(m_cszFormat, spString->GetStringW());

		CString cszStringEntry;
		cszStringEntry.FormatMessage(ADVSUMMARY_ENTRY_FORMAT_PATH_STRING, cszHtmlId, m_cszLabel, cszData);
		cszRenderString += cszStringEntry;
	}

	if(cszRenderString.IsEmpty())
		return S_FALSE;

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsEntryRiskLevelElement::CAdvDetailsEntryRiskLevelElement(void)
{
}

CAdvDetailsEntryRiskLevelElement::~CAdvDetailsEntryRiskLevelElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryRiskLevelElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
	if(!pData)
		return E_FAIL;

	m_spData = pData;

	if(szFormat)
		m_cszFormat = szFormat;

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryRiskLevelElement::SetLabel(LPCWSTR szLabel)
{
	m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_ADVDETAILS_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryRiskLevelElement::SetHtmlId(LPCWSTR szHtmlId)
{
	m_cszHtmlId = CW2T(szHtmlId);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsEntryRiskLevelElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
	HRESULT hr;
	CString cszRiskLevelString, cszRiskLevelImage;

	ui::INumberDataQIPtr spRiskLevel = m_spData;
	if(!spRiskLevel)
	{
		CCTRACEE( CCTRCTX _T("Problem getting risk level info."));
		return S_FALSE;
	}

	LONGLONG qdwRiskLevel = 0xFF;
	hr = spRiskLevel->GetNumber(qdwRiskLevel);
	if(FAILED(hr) || !(qdwRiskLevel >= 0 && qdwRiskLevel <= 2))
	{
		CCTRACEE( CCTRCTX _T("Invalid risk level. 0x%16I64X"), qdwRiskLevel);
		return S_FALSE;
	}

	UINT pRiskLevelMap[3] = {IDS_TEXT_LOW, IDS_TEXT_MEDIUM, IDS_TEXT_HIGH};
	LPCTSTR pszRiskLevelImages[] = {_T("LOWRISK.GIF"), _T("MEDRISK.GIF"), _T("HIGHRISK.GIF")};
	
	CString cszValueFmt;
	cszValueFmt.Format(m_cszFormat, _S(pRiskLevelMap[qdwRiskLevel]));

	CString cszValueString;
	cszValueString.FormatMessage(ADVDETAILS_RISK_LEVEL_PLUS_IMAGE_FORMAT_STRING, cszValueFmt, pszRiskLevelImages[qdwRiskLevel]);

	CString cszRenderString;
	cszRenderString.FormatMessage(ADVSUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszValueString);

	return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsShowThreatDetailsElement::CAdvDetailsShowThreatDetailsElement(void)
{
}

CAdvDetailsShowThreatDetailsElement::~CAdvDetailsShowThreatDetailsElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsShowThreatDetailsElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsShowThreatDetailsElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_ADVDETAILS_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsShowThreatDetailsElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsShowThreatDetailsElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
	ui::IDataPtr spData;
	hr = m_spMessage->GetData(AV::AVDATA_SUPPORTS_VIEW_DETAILS, spData);
	if(FAILED(hr))
		return S_FALSE;

	bool bSupportsViewDetails = false;
	CFormatDataToBool::DataToBool(spData, bSupportsViewDetails);
	if(!bSupportsViewDetails)
		return S_FALSE;

	CString cszData;
    hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
	if(FAILED(hr) || cszData.IsEmpty())
	{
		return S_FALSE;
	}

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVDETAILS_SHOWTHREATSUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsShowThreatDetailsElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::HYPERLINK_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;

	bool bAccessControl = false;
	do // intentional scoping
	{
		ui::IDataPtr spData;
		HRESULT hr = m_spMessage->GetData(AV::AVDATA_USER_SID_STRING, spData);
		if(FAILED(hr))
			break;

		CString cszMessageDataSID ;
		CFormatDataToString::DataToString(spData, _T("%s"), cszMessageDataSID);
		bAccessControl = CAntivirusMessageBase::m_AccessControl.AllowViewItemDetails(cszMessageDataSID);

	}while(false);
	
	if(!bAccessControl)
	{
		::MessageBox(::GetActiveWindow(), _S(IDS_ERR_INSUFFICIENT_RIGHTS), _S(IDS_TITLE_VIEW_SECURITY_RISK_DETAILS), MB_OK);
		return S_OK;
	}

    CString cszData;
    HRESULT hr = CFormatDataToString::DataToString(m_spData, _T("%s"), cszData);
    if(FAILED(hr))
    {
        CCTRACEE( CCTRCTX _T("CFormatDataToString::DataToString() failed. hr=0x%08X"), hr);
        return E_UNEXPECTED;
    }

    GUID guidThreatTrackId;
    hr = CLSIDFromString(CT2OLE(cszData), (LPCLSID)&guidThreatTrackId);
    if(FAILED(hr))
    {
        CCTRACEE( CCTRCTX _T("CLSIDFromString() failed. hr=0x%08X"), hr);
        return E_UNEXPECTED;
    }

    avScanUI::sui_IScanUIMisc ScanUIMiscLoader;
    avScanUI::IScanUIMiscPtr spScanUIMisc;
    ScanUIMiscLoader.CreateObject(spScanUIMisc);
    if(!spScanUIMisc)
    {
        CCTRACEE( CCTRCTX _T("Unable to create AvScanUIMisc object."));
        return E_UNEXPECTED;
    }

    hr = spScanUIMisc->Initialize();
    if(FAILED(hr))
    {
        CCTRACEE( CCTRCTX _T("IScanUIMisc::Initialize() failed. hr=0x%08X"), hr);
        return E_UNEXPECTED;
    }

    hr = spScanUIMisc->DisplayThreatDetails(NULL, guidThreatTrackId);
    if(FAILED(hr))
    {
        CCTRACEE( CCTRCTX _T("IScanUIMisc::DisplayThreatDetails(%s) failed. hr=0x%08X"), cszData, hr);
        return E_UNEXPECTED;
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsCompactPathEntryElement::CAdvDetailsCompactPathEntryElement(void)
{
}

CAdvDetailsCompactPathEntryElement::~CAdvDetailsCompactPathEntryElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsCompactPathEntryElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;
    
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsCompactPathEntryElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_ADVDETAILS_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsCompactPathEntryElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsCompactPathEntryElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    CString cszData;
    hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
    if(FAILED(hr) || cszData.IsEmpty())
    {
        return S_FALSE;
    }

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVSUMMARY_ENTRY_FORMAT_PATH_STRING, m_cszHtmlId, m_cszLabel, cszData);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsSumEntryElement::CAdvDetailsSumEntryElement(void) :
m_qdwData(NULL)
{
}

CAdvDetailsSumEntryElement::~CAdvDetailsSumEntryElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsSumEntryElement::AddData(ui::IData* pData)
{
	if(!pData)
		return E_FAIL;

	ui::INumberDataQIPtr spNumberData = pData;
	if(!spNumberData)
		return E_INVALIDARG;

	LONGLONG qdwNumber = 0;
	spNumberData->GetNumber(qdwNumber);
	m_qdwData += qdwNumber;

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsSumEntryElement::SetLabel(LPCWSTR szLabel)
{
	m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_SUMMARY_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsSumEntryElement::SetHtmlId(LPCWSTR szHtmlId)
{
	m_cszHtmlId = CW2T(szHtmlId);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsSumEntryElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
	CString cszData;
	cszData.Format(_T("%I64d"), m_qdwData);

	CString cszRenderString;
	cszRenderString.FormatMessage(ADVSUMMARY_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);

	return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsSubSumEntryElement::CAdvDetailsSubSumEntryElement(void) :
m_qdwData(NULL)
{
}

CAdvDetailsSubSumEntryElement::~CAdvDetailsSubSumEntryElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsSubSumEntryElement::AddData(ui::IData* pData)
{
	if(!pData)
		return E_FAIL;

	ui::INumberDataQIPtr spNumberData = pData;
	if(!spNumberData)
		return E_INVALIDARG;

	LONGLONG qdwNumber = 0;
	spNumberData->GetNumber(qdwNumber);
	m_qdwData += qdwNumber;

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsSubSumEntryElement::SetLabel(LPCWSTR szLabel)
{
	m_cszLabel = CW2T(szLabel);
	m_cszLabel += _S(IDS_MCLABEL_SUMMARY_SUFFIX);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsSubSumEntryElement::SetHtmlId(LPCWSTR szHtmlId)
{
	m_cszHtmlId = CW2T(szHtmlId);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsSubSumEntryElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
	if(!m_qdwData)
		return S_FALSE; // supress values that are zero

	CString cszData;
	cszData.Format(_T("%I64d"), m_qdwData);

	CString cszRenderString;
	cszRenderString.FormatMessage(ADVSUMMARY_SUBENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel, cszData);

	return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CActionSectionTextElement::CActionSectionTextElement(void)
{
}

CActionSectionTextElement::~CActionSectionTextElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CActionSectionTextElement::SetHtmlId(LPCWSTR wszHtmlId)
{
    m_cszHtmlId = CW2T(wszHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CActionSectionTextElement::SetLabel(LPCWSTR wszText)
{
    m_cszText = CW2T(wszText);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CActionSectionTextElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    ui::IDataPtr spData;
    hr = m_spMessage->GetData(AV::AVDATA_HAS_NO_ACTIONS, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bHasNoActions = false;
    CFormatDataToBool::DataToBool(spData, bHasNoActions);
    if(!bHasNoActions)
        return S_FALSE;

    CString cszRenderString;
    cszRenderString.FormatMessage(ACTION_SECTION_TEXT_FORMAT_STRING, m_cszHtmlId, m_cszText);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CActionSectionRecommendedTextElement::CActionSectionRecommendedTextElement(void)
{
}

CActionSectionRecommendedTextElement::~CActionSectionRecommendedTextElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CActionSectionRecommendedTextElement::SetHtmlId(LPCWSTR wszHtmlId)
{
	m_cszHtmlId = CW2T(wszHtmlId);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CActionSectionRecommendedTextElement::SetLabel(LPCWSTR wszText)
{
	m_cszText = CW2T(wszText);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CActionSectionRecommendedTextElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
	HRESULT hr;
	ui::IDataPtr spData;

	hr = m_spMessage->GetData(AV::AVDATA_RECOMMENDED_ACTION, spData);
	if(FAILED(hr))
		return S_FALSE;

	bool bRecommendedAction = false;
	ui::INumberDataQIPtr spRecommendedAction = spData;
	if(spRecommendedAction)
	{
		LONGLONG qdwNumberData;
		hr = spRecommendedAction->GetNumber(qdwNumberData);
		if(SUCCEEDED(hr))
		{
			switch(qdwNumberData)
			{
				case AV::RecommendedAction::eExcludeThreat:
				case AV::RecommendedAction::eRunQuickScan:
				case AV::RecommendedAction::eRunFullSystemScan:
                case AV::RecommendedAction::eRemoveNow:
				case AV::RecommendedAction::eDelete:
				case AV::RecommendedAction::eSubmit:
				case AV::RecommendedAction::eManualRemovalRequired:
					break; // continue

				default:
					return S_FALSE;
					break;
			}
		}

	}
	
	CString cszRenderString;
	cszRenderString.FormatMessage(ACTION_SECTION_RECOMMENDED_TEXT_FORMAT_STRING, m_cszHtmlId, m_cszText);

	return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsTroubleshootUrlElement::CAdvDetailsTroubleshootUrlElement(void)
{
}

CAdvDetailsTroubleshootUrlElement::~CAdvDetailsTroubleshootUrlElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootUrlElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootUrlElement::SetTargetData(ui::IData* pTargetData, LPCWSTR szTargetFormat)
{
    if(!pTargetData)
        return E_FAIL;

    m_spTargetData = pTargetData;

    if(szTargetFormat)
        m_cszTargetFormat = szTargetFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootUrlElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootUrlElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;

    CString cszData;
    hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
	if(FAILED(hr) || cszData.IsEmpty())
	{
		return S_FALSE;
	}

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVDETAILS_TROUBLESHOOT_ENTRY_FORMAT_STRING, m_cszHtmlId, cszData);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootUrlElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::HYPERLINK_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;
    
	if(NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;

    CString cszData;
    HRESULT hr = CFormatDataToString::DataToString(m_spTargetData, m_cszTargetFormat, cszData);
    if(SUCCEEDED(hr))
    {
        hr = ShellExecuteWorker(NULL, _T("open"), cszData, NULL, NULL, SW_SHOWNORMAL);
        if(FAILED(hr))
            CCTRACEE( CCTRCTX _T("ShellExecute(%s) failed. dwRet = %d"), (LPCTSTR)cszData, hr);

    }
    else
    {
        CCTRACEE( CCTRCTX _T("Unable to format string data"));
    }
    
    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsTroubleshootHelpElement::CAdvDetailsTroubleshootHelpElement(void) :
m_dwSymHelpId(NULL)
{
}

CAdvDetailsTroubleshootHelpElement::~CAdvDetailsTroubleshootHelpElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootHelpElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootHelpElement::SetSymHelpId(DWORD dwSymHelpId)
{
    m_dwSymHelpId = dwSymHelpId;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootHelpElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootHelpElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;

    CString cszData;
    hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
	if(FAILED(hr) || cszData.IsEmpty())
	{
		return S_FALSE;
	}

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVDETAILS_TROUBLESHOOT_ENTRY_FORMAT_STRING, m_cszHtmlId, cszData);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootHelpElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::HYPERLINK_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;

    isshared::CHelpLauncher Help;
    Help.LaunchHelp ( m_dwSymHelpId );

    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsTroubleshootOptionsElement::CAdvDetailsTroubleshootOptionsElement(void)
{
	ZeroMemory(&m_guidElementId, sizeof(SYMGUID));
}

CAdvDetailsTroubleshootOptionsElement::~CAdvDetailsTroubleshootOptionsElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootOptionsElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
	if(!pData)
		return E_FAIL;

	m_spData = pData;

	if(szFormat)
		m_cszFormat = szFormat;

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootOptionsElement::SetISSharedElementId(const SYMGUID& guidElementId)
{
	memcpy(&m_guidElementId, &guidElementId, sizeof(SYMGUID));

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootOptionsElement::SetHtmlId(LPCWSTR szHtmlId)
{
	m_cszHtmlId = CW2T(szHtmlId);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootOptionsElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
	HRESULT hr;

	CString cszData;
	hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
	if(FAILED(hr) || cszData.IsEmpty())
	{
		return S_FALSE;
	}

	CString cszRenderString;
	cszRenderString.FormatMessage(ADVDETAILS_TROUBLESHOOT_ENTRY_FORMAT_STRING, m_cszHtmlId, cszData);

	return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootOptionsElement::OnAction(UINT uAction, LPCWSTR wszID)
{
	if(symhtml::event::HYPERLINK_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
		return S_FALSE;

	StahlSoft::HRX hrx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		SYMRESULT sr;
		ISShared::ISShared_IProvider ProviderLoader;
		ui::IProviderPtr spISElementProvider;
		sr = ProviderLoader.CreateObject(GETMODULEMGR(), &spISElementProvider);
		hrx << MCF::HRESULT_FROM_SYMRESULT(sr);

		ui::IElementPtr pElement;
		hrx << spISElementProvider->GetElement ( m_guidElementId, pElement);
		hrx << pElement->Configure (::GetActiveWindow(), NULL);

		CCTRACEI( CCTRCTX _T("Element successfully launched"));
	}
	CCCATCHMEM(exceptionInfo)
	CCCATCHCOM(exceptionInfo);

	if(exceptionInfo.IsException())
	{
		// Do not display errors to user, allow the Element to handle error display
		CCTRACEE( CCTRCTX _T("Unable to launch element. Error: %s"), exceptionInfo.GetDescription());
	}

	return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsTroubleshootComponentOptionsElement::CAdvDetailsTroubleshootComponentOptionsElement(void)
{
}

CAdvDetailsTroubleshootComponentOptionsElement::~CAdvDetailsTroubleshootComponentOptionsElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootComponentOptionsElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
	if(!pData)
		return E_FAIL;

	m_spData = pData;

	if(szFormat)
		m_cszFormat = szFormat;

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootComponentOptionsElement::SetHtmlId(LPCWSTR szHtmlId)
{
	m_cszHtmlId = CW2T(szHtmlId);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootComponentOptionsElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
	HRESULT hr;

	CString cszData;
	hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, cszData);
	if(FAILED(hr) || cszData.IsEmpty())
	{
		return S_FALSE;
	}

	CString cszRenderString;
	cszRenderString.FormatMessage(ADVDETAILS_TROUBLESHOOT_ENTRY_FORMAT_STRING, m_cszHtmlId, cszData);

	return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsTroubleshootComponentOptionsElement::OnAction(UINT uAction, LPCWSTR wszID)
{
	if(symhtml::event::HYPERLINK_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
		return S_FALSE;

	StahlSoft::HRX hrx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		ui::IDataPtr spData;
		hrx << m_spMessage->GetData(AV::AVDATA_COMPONENT_ID, spData);
		ui::INumberDataQIPtr spRecommendedAction = spData;
		if(!spRecommendedAction)
			hrx << E_INVALIDARG;

		LONGLONG qdwComponentId;
		hrx << spRecommendedAction->GetNumber(qdwComponentId);

		SYMGUID* pElementId = NULL;

		if(SAVRT_MODULE_ID_NAVAPSVC == qdwComponentId)
			pElementId = (SYMGUID*)&ISShared::CLSID_NIS_AutoProtect;
		else if(AV_MODULE_ID_NAVW == qdwComponentId)
			pElementId = (SYMGUID*)&ISShared::CLSID_NAV_OptionsManualScan;
		else if(AV_MODULE_ID_EMAIL_SCAN == qdwComponentId)
			pElementId = (SYMGUID*)&ISShared::CLSID_NIS_EmailScanningInbound;

		if(!pElementId)
			hrx << E_INVALIDARG;

		SYMRESULT sr;
		ISShared::ISShared_IProvider ProviderLoader;
		ui::IProviderPtr spISElementProvider;
		sr = ProviderLoader.CreateObject(GETMODULEMGR(), &spISElementProvider);
		hrx << MCF::HRESULT_FROM_SYMRESULT(sr);

		ui::IElementPtr pElement;
		hrx << spISElementProvider->GetElement ( *pElementId, pElement);
		hrx << pElement->Configure (::GetActiveWindow(), NULL);

		CCTRACEI( CCTRCTX _T("Element successfully launched"));
	}
	CCCATCHMEM(exceptionInfo)
		CCCATCHCOM(exceptionInfo);

	if(exceptionInfo.IsException())
	{
		CCTRACEE( CCTRCTX _T("Unable to launch element. Error: %s"), exceptionInfo.GetDescription());
		::MessageBox(GetActiveWindow(), _S(IDS_ERR_CANNOT_LAUNCH_OPTIONS), _S(IDS_TITLE_LAUNCH_OPTIONS), MB_OK|MB_ICONSTOP);
	}

	return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsActionRestoreElement::CAdvDetailsActionRestoreElement(void) :
m_pDoc(NULL)
{
}

CAdvDetailsActionRestoreElement::~CAdvDetailsActionRestoreElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRestoreElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRestoreElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRestoreElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRestoreElement::SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc)
{
    m_pDoc = pDoc;
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRestoreElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    ui::IDataPtr spData;
    hr = m_spMessage->GetData(AV::AVDATA_SUPPORTS_RESTORE, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bSupportsRestore = false;
    CFormatDataToBool::DataToBool(spData, bSupportsRestore);
    if(!bSupportsRestore)
        return S_FALSE;

    spData.Release();
    hr = m_spMessage->GetData(AV::AVDATA_RECOMMENDED_ACTION, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bRecommendedAction = false;
    ui::INumberDataQIPtr spRecommendedAction = spData;
    if(spRecommendedAction)
    {
        LONGLONG qdwNumberData;
        hr = spRecommendedAction->GetNumber(qdwNumberData);
        if(SUCCEEDED(hr))
        {
            if(qdwNumberData == AV::RecommendedAction::eRestore)
                bRecommendedAction = true;
        }

    }

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVDETAILS_ACTION_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRestoreElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::BUTTON_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;
    
	bool bAccessControl = false;
	do // intentional scoping
	{
		ui::IDataPtr spData;
		HRESULT hr = m_spMessage->GetData(AV::AVDATA_USER_SID_STRING, spData);
		if(FAILED(hr))
			break;

		CString cszMessageDataSID ;
		CFormatDataToString::DataToString(spData, _T("%s"), cszMessageDataSID);
		bAccessControl = CAntivirusMessageBase::m_AccessControl.AllowRestoreThisItem(cszMessageDataSID);

	}while(false);

	if(!bAccessControl)
	{
		::MessageBox(::GetActiveWindow(), _S(IDS_ERR_INSUFFICIENT_RIGHTS), _S(IDS_TITLE_RESTORE_SECURITY_RISK), MB_OK);
		return S_OK;
	}

    CString cszData;
    HRESULT hr = CFormatDataToString::DataToString(m_spData, _T("%s"), cszData);
    if(FAILED(hr))
        return E_UNEXPECTED;

    GUID guidThreatTrackId;
    hr = CLSIDFromString(CT2OLE(cszData), (LPCLSID)&guidThreatTrackId);
    if(FAILED(hr))
        return E_UNEXPECTED;
    
    for(;;)
    {
        avScanUI::sui_IScanUIMisc ScanUIMiscLoader;
        avScanUI::IScanUIMiscPtr spScanUIMisc;
        ScanUIMiscLoader.CreateObject(spScanUIMisc);
        if(!spScanUIMisc)
        {
            CCTRACEE( CCTRCTX _T("Unable to create AvScanUIMisc object."));
            break;
        }

        hr = spScanUIMisc->Initialize();
        if(FAILED(hr))
        {
            CCTRACEE( CCTRCTX _T("IScanUIMisc::Initialize() failed. hr=0x%08X"), hr);
            break;
        }

        hr = spScanUIMisc->QuarantineRestore(NULL, guidThreatTrackId);
        if(FAILED(hr))
        {
            CCTRACEE( CCTRCTX _T("IScanUIMisc::QuarantineRestore(%s) failed. hr=0x%08X"), cszData, hr);
            break;
        }
        
        if(S_OK == hr) // S_FALSE == user abort
            m_spMessageProviderSink->OnRemoveMessage(m_spMessage);

        break;
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsActionDeleteElement::CAdvDetailsActionDeleteElement(void) :
m_pDoc(NULL)
{
}

CAdvDetailsActionDeleteElement::~CAdvDetailsActionDeleteElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionDeleteElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionDeleteElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionDeleteElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionDeleteElement::SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc)
{
    m_pDoc = pDoc;
    return S_OK;
}


//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionDeleteElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    ui::IDataPtr spData;
    hr = m_spMessage->GetData(AV::AVDATA_SUPPORTS_DELETE_ITEM, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bSupportsDeleteItem = false;
    CFormatDataToBool::DataToBool(spData, bSupportsDeleteItem);
    if(!bSupportsDeleteItem)
        return S_FALSE;

    spData.Release();
    hr = m_spMessage->GetData(AV::AVDATA_RECOMMENDED_ACTION, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bRecommendedAction = false;
    ui::INumberDataQIPtr spRecommendedAction = spData;
    if(spRecommendedAction)
    {
        LONGLONG qdwNumberData;
        hr = spRecommendedAction->GetNumber(qdwNumberData);
        if(SUCCEEDED(hr))
        {
            if(qdwNumberData == AV::RecommendedAction::eDelete)
                bRecommendedAction = true;
        }

    }

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVDETAILS_ACTION_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionDeleteElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::BUTTON_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;

	bool bAccessControl = false;
	do // intentional scoping
	{
		ui::IDataPtr spData;
		HRESULT hr = m_spMessage->GetData(AV::AVDATA_USER_SID_STRING, spData);
		if(FAILED(hr))
			break;

		CString cszMessageDataSID ;
		CFormatDataToString::DataToString(spData, _T("%s"), cszMessageDataSID);
		bAccessControl = CAntivirusMessageBase::m_AccessControl.AllowDeleteThisItem(cszMessageDataSID);

	}while(false);

	if(!bAccessControl)
	{
		::MessageBox(::GetActiveWindow(), _S(IDS_ERR_INSUFFICIENT_RIGHTS), _S(IDS_TITLE_DELETE_ITEM), MB_OK);
		return S_OK;
	}

    CString cszData;
    HRESULT hr = CFormatDataToString::DataToString(m_spData, _T("%s"), cszData);
    if(FAILED(hr))
        return E_UNEXPECTED;

    GUID guidThreatTrackId;
    hr = CLSIDFromString(CT2OLE(cszData), (LPCLSID)&guidThreatTrackId);
    if(FAILED(hr))
        return E_UNEXPECTED;

    INT nConfirm = ::MessageBox(GetActiveWindow(), _S(IDS_CONFIRM_DELETE_QITEM), _S(IDS_TEXT_DELETE_QUARANTINE_ITEM), MB_YESNO);

    if(IDYES == nConfirm)
    {
        for(;;)
        {
            if(m_pDoc == NULL)
            {
                CCTRCTXE0(L"m_pDoc == NULL");
                break;
            }
            
            symhtml::ISymHTMLElementPtr pElement;
            HRESULT hr = m_pDoc->GetElement(CT2A(m_cszHtmlId), &pElement);
            if(FAILED(hr) || pElement == NULL)
            {
                CCTRCTXE1(L"Failed to get element - 0x%08X", hr);
                break;
            }
            
            AVModule::AVLoader_IAVThreatInfo AvThreatInfoLoader;
            AVModule::IAVThreatInfoPtr spThreatInfo;
            AvThreatInfoLoader.CreateObject(spThreatInfo);
            if(!spThreatInfo)
            {
                CCTRACEW( CCTRCTX _T("AvModuleLoader::CreateObject() failed."));
                break;
            }
            
            spThreatInfo->RemoveThreat(guidThreatTrackId);
            m_spMessageProviderSink->OnRemoveMessage(m_spMessage);

            break;
        }
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsActionLaunchQuickScanElement::CAdvDetailsActionLaunchQuickScanElement(void) :
m_pDoc(NULL)
{
}

CAdvDetailsActionLaunchQuickScanElement::~CAdvDetailsActionLaunchQuickScanElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionLaunchQuickScanElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionLaunchQuickScanElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionLaunchQuickScanElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionLaunchQuickScanElement::SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc)
{
    m_pDoc = pDoc;
    return S_OK;
}


//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionLaunchQuickScanElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    ui::IDataPtr spData;
    hr = m_spMessage->GetData(AV::AVDATA_SUPPORTS_QUICKSCAN, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bSupportsQuickScan = false;
    CFormatDataToBool::DataToBool(spData, bSupportsQuickScan);
    if(!bSupportsQuickScan)
        return S_FALSE;

    spData.Release();
    hr = m_spMessage->GetData(AV::AVDATA_RECOMMENDED_ACTION, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bRecommendedAction = false;
    ui::INumberDataQIPtr spRecommendedAction = spData;
    if(spRecommendedAction)
    {
        LONGLONG qdwNumberData;
        hr = spRecommendedAction->GetNumber(qdwNumberData);
        if(SUCCEEDED(hr))
        {
            if(qdwNumberData == AV::RecommendedAction::eRunQuickScan)
                bRecommendedAction = true;
        }

    }
    
    CString cszLabel;
    if(bRecommendedAction)
        cszLabel.FormatMessage(_S(IDS_FMT_ACTION_RECOMMENDED), m_cszLabel);
    else
        cszLabel = m_cszLabel;

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVDETAILS_ACTION_ENTRY_FORMAT_STRING, m_cszHtmlId, cszLabel);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionLaunchQuickScanElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::BUTTON_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;
	
	HRESULT hrReturn = S_FALSE;

    CString cszData;
    HRESULT hr = CFormatDataToString::DataToString(m_spData, _T("%s"), cszData);
    if(FAILED(hr))
        return E_UNEXPECTED;

    GUID guidThreatTrackId;
    hr = CLSIDFromString(CT2OLE(cszData), (LPCLSID)&guidThreatTrackId);
    if(FAILED(hr))
        return E_UNEXPECTED;

    ::MessageBox(GetActiveWindow(), _S(IDS_TEXT_CONFIRM_QUICKSCAN), _S(IDS_TITLE_RUN_QUICKSCAN), MB_OK);
	StahlSoft::HRX hrx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		SYMRESULT sr;
        avScanTask::ScanTask_Loader ScanTaskLoader;
        avScanTask::IScanTaskPtr spScanTask;
        sr = ScanTaskLoader.CreateObject(spScanTask);
        hrx << MCF::HRESULT_FROM_SYMRESULT(sr);

        hr = GetInfectedFiles(guidThreatTrackId, spScanTask);
        if(S_FALSE == hr)
        {
            CCTRACEE( CCTRCTX _T("There are no infected files to scan. hr=%08X"), hr);
            hrx << E_FAIL;
        }
        else if(FAILED(hr))
        {
            CCTRACEE( CCTRCTX _T("Problem getting infected files. hr=%08X"), hr);
        }

        CString cszScanTaskFilename;
        hrx << GenerateTempScanFile(cszScanTaskFilename);
        hrx << spScanTask->SetPath ( cszScanTaskFilename );
        hrx << spScanTask->SetType(avScanTask::scanCustom);

        hr = spScanTask->Save();
        if(FAILED(hr))
        {
            DeleteFile ( cszScanTaskFilename );
            CCTRACEE( CCTRCTX _T("Problem saving task file. hr=%08X"), hr);
        }

        // Command line is c:\progra~1\norton~1\navw32.exe /SE- /ttask:"c:\...\task.sca"
        CString cszExePath, cszParameters;
        CNAVInfo NAVInfo;
        TCHAR szShortNAVDir [MAX_PATH] = {0};
        TCHAR szShortTaskName [MAX_PATH] = {0};
        TCHAR SZ_NAVW32_EXE[] = {_T("NAVW32.exe")};

        ::GetShortPathName ( NAVInfo.GetNAVDir (), szShortNAVDir, MAX_PATH );
        ::GetShortPathName ( cszScanTaskFilename, szShortTaskName, MAX_PATH );

        ::PathCombine(cszExePath.GetBuffer(_tcslen(szShortNAVDir) + _tcslen(SZ_NAVW32_EXE) + 1), szShortNAVDir, SZ_NAVW32_EXE);
        cszExePath.ReleaseBuffer();

        cszParameters.Format(_T("%s %s\"%s\""), SWITCH_DISABLE_ERASER_SCAN, SWITCH_TTASKFILE, szShortTaskName);

        CCTRACEW( CCTRCTX _T("ShellExecute(\"Open %s %s\")"), cszExePath, cszParameters);
        hr = ShellExecuteWorker(   ::GetActiveWindow(), _T("open"), cszExePath, cszParameters, NULL, SW_SHOW);
        if(FAILED(hr))
        {
            CCTRACEE( CCTRCTX _T("Problem launching ShellExecuteWorker(). hr=0x%08X"));
            hrx << hr;
        }
    }
	CCCATCHMEM(exceptionInfo)
	CCCATCHCOM(exceptionInfo);

	if(exceptionInfo.IsException())
	{
        CCTRACEE( CCTRCTX _T("%s"), exceptionInfo.GetFullDescription());
		CCTRACEE( CCTRCTX _T("Unable to launch element. Error: %s"), exceptionInfo.GetDescription());
		::MessageBox(GetActiveWindow(), _S(IDS_ERR_CANNOT_RUN_QUICKSCAN), _S(IDS_TITLE_RUN_QUICKSCAN), MB_OK|MB_ICONSTOP);
	}

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionLaunchQuickScanElement::GetInfectedFiles(GUID guidThreatTrackId, avScanTask::IScanTask* pScanTask)
{
    HRESULT hrReturn = S_FALSE;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        HRESULT hr;
        AVModule::AVLoader_IAVThreatInfo AvThreatInfoLoader;
        AvThreatInfoLoader.Initialize();

        AVModule::IAVThreatInfoPtr spThreatInfo;
        AvThreatInfoLoader.CreateObject(spThreatInfo);
        if(!spThreatInfo)
        {
            CCTRACEW( CCTRCTX _T("AvModuleLoader::CreateObject() failed."));
            hrx << E_UNEXPECTED;
        }

        AVModule::AVLoader_IAVMapDwordData AVMapDwordDataLoader;
        AVModule::IAVMapDwordDataPtr spFilter;
        AVMapDwordDataLoader.CreateObject(spFilter);
        if(!spFilter)
        {
            CCTRACEW( CCTRCTX _T("AVLoader_IAVMapDwordData::CreateObject() failed."));
            hrx << E_UNEXPECTED;
        }
        
        // Get ThreatTrackInfo
        AVModule::IAVMapDwordDataPtr spArrThreatTrackInfo;
        hr = spThreatInfo->GetSingleThreat(guidThreatTrackId, spArrThreatTrackInfo);
        if(FAILED(hr) || !spArrThreatTrackInfo)
        {
            CCTRACEE( CCTRCTX _T("Unable to retrieve threat info for threat. hr = 0x%08X"), hr);
            hrx << E_UNEXPECTED;
        }
        
        DWORD dwVID = NULL;
        spArrThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_VID, dwVID);

        // Get display info
        AVModule::IAVArrayDataPtr spArrDispInfo;
        hr = spThreatInfo->GetAnomalyDisplayData(guidThreatTrackId, false, spArrDispInfo);
        if(FAILED(hr) || !spArrDispInfo)
        {
            CCTRACEE( CCTRCTX _T("Unable to retrieve display info for threat. hr = 0x%08X"), hr);
            hrx << E_UNEXPECTED;
        }
        
        DWORD dwDispInfoCount = 0, dwDispInfoIndex = 0;
        spArrDispInfo->GetCount(dwDispInfoCount);
        
        for(dwDispInfoIndex = 0; dwDispInfoIndex < dwDispInfoCount; dwDispInfoIndex++)
        {
            ISymBasePtr spTempData;
            hr = spArrDispInfo->GetValue(dwDispInfoIndex, AVModule::AVDataTypes::eTypeMapDword, spTempData);
            if(FAILED(hr) || !spTempData)
            {
                CCTRACEE(_T("Unable to retrieve array info.  Unexpected."));
                continue;
            }

            AVModule::IAVMapDwordDataQIPtr spMapCurrentDispInfo = spTempData;
            if(!spMapCurrentDispInfo)
            {
                CCTRACEE(_T("Unable to retrieve array type.  Unexpected."));
                continue;
            }

            DWORD dwRemediationType = NULL;
            spMapCurrentDispInfo->GetValue(AVModule::ThreatTracking::RemDisp_RemediationType, dwRemediationType);

            if(ccEraser::InfectionRemediationActionType == dwRemediationType)        
            {
                cc::IStringPtr spFilePath;
                hr = spMapCurrentDispInfo->GetValue(AVModule::ThreatTracking::RemDisp_Target, spFilePath);
                if(SUCCEEDED(hr) && spFilePath)
                {

                    pScanTask->AddItem(avScanTask::typeFileVID, avScanTask::subtypeNone, NULL, spFilePath->GetStringW(), dwVID);
                    CCTRACEI( CCTRCTX _T("Found infected file: %s"), spFilePath->GetStringW());
                    hrReturn = S_OK;
                }
            }
        }

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRACEE( CCTRCTX _T("%s"), exceptionInfo.GetFullDescription());
        return E_FAIL;
    }

    return hrReturn;
}

// GHS: 362840 I am returning the reason for the failure if it is NOT ERROR_FILE_EXISTS
HRESULT CAdvDetailsActionLaunchQuickScanElement::GenerateTempScanFile(CString& cszScanFileName)
{
    TCHAR szTempDir[ MAX_PATH*2 ];
    TCHAR szTempFile[ 32 ];

    DWORD dwLastError = NOERROR;
    
    // Fetch temp directory.
    if( 0 == ::GetTempPath(MAX_PATH*2, szTempDir) )
    {
        dwLastError = ::GetLastError();
        return (dwLastError == NOERROR)? E_FAIL:HRESULT_FROM_WIN32(dwLastError);
    }

    int iIndex = 0;
    for(;; ++iIndex)
    {
        // Construct file name.  File will be in the form
        // scanxxxx.sca.
        _stprintf( szTempFile, _T("nvscan%02X.%s"), iIndex, _T("sca"));
        ::PathCombine(cszScanFileName.GetBuffer(_tcslen(szTempDir) + _tcslen(szTempFile) + 1), szTempDir, szTempFile);
        cszScanFileName.ReleaseBuffer();

        // Try to create the file.
        HANDLE hFile = CreateFile( cszScanFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL );
        if( hFile != INVALID_HANDLE_VALUE )
        {
            // Success.  Close the handle and return.
            CloseHandle( hFile );
            return S_OK;
        }

        // An error occured.  If the error states that the file
        // already exists, keep processing, else bail out.
        DWORD dwLastError = ::GetLastError();
        if( dwLastError != ERROR_FILE_EXISTS )
            return HRESULT_FROM_WIN32(dwLastError);
    }

    // Return error status.
    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsActionRemoveElement::CAdvDetailsActionRemoveElement(void) :
m_pDoc(NULL)
{
}

CAdvDetailsActionRemoveElement::~CAdvDetailsActionRemoveElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRemoveElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRemoveElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRemoveElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRemoveElement::SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc)
{
    m_pDoc = pDoc;
    return S_OK;
}


//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRemoveElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    ui::IDataPtr spData;
    hr = m_spMessage->GetData(AV::AVDATA_SUPPORTS_REMOVENOW, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bSupportsFix = false;
    CFormatDataToBool::DataToBool(spData, bSupportsFix);
    if(!bSupportsFix)
        return S_FALSE;

    spData.Release();
    hr = m_spMessage->GetData(AV::AVDATA_RECOMMENDED_ACTION, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bRecommendedAction = false;
    ui::INumberDataQIPtr spRecommendedAction = spData;
    if(spRecommendedAction)
    {
        LONGLONG qdwNumberData;
        hr = spRecommendedAction->GetNumber(qdwNumberData);
        if(SUCCEEDED(hr))
        {
            if(qdwNumberData == AV::RecommendedAction::eRemoveNow)
                bRecommendedAction = true;
        }

    }

    CString cszLabel;
    if(bRecommendedAction)
        cszLabel.FormatMessage(_S(IDS_FMT_ACTION_RECOMMENDED), m_cszLabel);
    else
        cszLabel = m_cszLabel;

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVDETAILS_ACTION_ENTRY_FORMAT_STRING, m_cszHtmlId, cszLabel);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionRemoveElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::BUTTON_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;

    HRESULT hrReturn = S_FALSE;

    CString cszData;
    HRESULT hr = CFormatDataToString::DataToString(m_spData, _T("%s"), cszData);
    if(FAILED(hr))
        return E_UNEXPECTED;

    GUID guidThreatTrackId;
    hr = CLSIDFromString(CT2OLE(cszData), (LPCLSID)&guidThreatTrackId);
    if(FAILED(hr))
        return E_UNEXPECTED;

    for(;;)
    {
        avScanUI::sui_IScanUIMisc ScanUIMiscLoader;
        avScanUI::IScanUIMiscPtr spScanUIMisc;
        ScanUIMiscLoader.CreateObject(spScanUIMisc);
        if(!spScanUIMisc)
        {
            CCTRACEE( CCTRCTX _T("Unable to create AvScanUIMisc object."));
            break;
        }

        hr = spScanUIMisc->Initialize();
        if(FAILED(hr))
        {
            CCTRACEE( CCTRCTX _T("IScanUIMisc::Initialize() failed. hr=0x%08X"), hr);
            break;
        }

        hr = spScanUIMisc->ProcessSingleThreat(GetActiveWindow(), guidThreatTrackId);
        if(FAILED(hr))
        {
            CCTRACEE( CCTRCTX _T("IScanUIMisc::QuarantineRestore(%s) failed. hr=0x%08X"), cszData, hr);
            break;
        }

        if(S_OK == hr) // S_FALSE == user abort
            m_spMessageProviderSink->OnRemoveMessage(m_spMessage);

        hrReturn = S_OK;
        break;
    }


    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsActionReviewRiskDetailsElement::CAdvDetailsActionReviewRiskDetailsElement(void) :
m_pDoc(NULL)
{
}

CAdvDetailsActionReviewRiskDetailsElement::~CAdvDetailsActionReviewRiskDetailsElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionReviewRiskDetailsElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionReviewRiskDetailsElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionReviewRiskDetailsElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionReviewRiskDetailsElement::SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc)
{
    m_pDoc = pDoc;
    return S_OK;
}


//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionReviewRiskDetailsElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    ui::IDataPtr spData;
    hr = m_spMessage->GetData(AV::AVDATA_SUPPORTS_REVIEW_RISK_DETAILS, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bSupportsReviewRiskDetails = false;
    CFormatDataToBool::DataToBool(spData, bSupportsReviewRiskDetails);
    if(!bSupportsReviewRiskDetails)
        return S_FALSE;

    spData.Release();
    hr = m_spMessage->GetData(AV::AVDATA_RECOMMENDED_ACTION, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bRecommendedAction = false;
    ui::INumberDataQIPtr spRecommendedAction = spData;
    if(spRecommendedAction)
    {
        LONGLONG qdwNumberData;
        hr = spRecommendedAction->GetNumber(qdwNumberData);
        if(SUCCEEDED(hr))
        {
            if(qdwNumberData == AV::RecommendedAction::eManualRemovalRequired)
                bRecommendedAction = true;
        }

    }
    
    CString cszLabel;
    if(bRecommendedAction)
        cszLabel.FormatMessage(_S(IDS_FMT_ACTION_RECOMMENDED), m_cszLabel);
    else
        cszLabel = m_cszLabel;

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVDETAILS_ACTION_ENTRY_FORMAT_STRING, m_cszHtmlId, cszLabel);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionReviewRiskDetailsElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::BUTTON_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;
	
	if(NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
		return S_FALSE;
	
	::MessageBox(GetActiveWindow(), _S(IDS_TEXT_REVIEW_RISK_DETAILS_MESSAGE), _S(IDS_TITLE_REVIEW_RISK_DETAILS), MB_OK);

	HRESULT hr;
	ui::IDataPtr spData;
	hr = m_spMessage->GetData(AV::AVDATA_RESPONSE_THREATINFO_URL, spData);
	if(FAILED(hr) || !spData)
	{
		CCTRACEE( CCTRCTX _T("Problem getting threatinfo url"));
		return S_FALSE;
	}
	
	CString cszData;
	hr = CFormatDataToString::DataToString(spData, _T("%s"), cszData);
	if(SUCCEEDED(hr))
	{
		hr = ShellExecuteWorker(NULL, _T("open"), cszData, NULL, NULL, SW_SHOWNORMAL);
		if(FAILED(hr))
			CCTRACEE( CCTRCTX _T("ShellExecute(%s) failed. dwRet = %d"), (LPCTSTR)cszData, hr);

	}
	else
	{
		CCTRACEE( CCTRCTX _T("Unable to format string data"));
	}

	return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsActionExcludeElement::CAdvDetailsActionExcludeElement(void) :
m_pDoc(NULL)
{
}

CAdvDetailsActionExcludeElement::~CAdvDetailsActionExcludeElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionExcludeElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
    if(!pData)
        return E_FAIL;

    m_spData = pData;

    if(szFormat)
        m_cszFormat = szFormat;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionExcludeElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionExcludeElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionExcludeElement::SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc)
{
    m_pDoc = pDoc;
    return S_OK;
}


//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionExcludeElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    ui::IDataPtr spData;
    hr = m_spMessage->GetData(AV::AVDATA_SUPPORTS_EXCLUDE, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bSupportsExclude = false;
    CFormatDataToBool::DataToBool(spData, bSupportsExclude);
    if(!bSupportsExclude)
        return S_FALSE;
    
    spData.Release();
    hr = m_spMessage->GetData(AV::AVDATA_RECOMMENDED_ACTION, spData);
    if(FAILED(hr))
        return S_FALSE;
    
    bool bRecommendedAction = false;
    ui::INumberDataQIPtr spRecommendedAction = spData;
    if(spRecommendedAction)
    {
        LONGLONG qdwNumberData;
        hr = spRecommendedAction->GetNumber(qdwNumberData);
        if(SUCCEEDED(hr))
        {
            if(qdwNumberData == AV::RecommendedAction::eExcludeThreat)
                bRecommendedAction = true;
        }

    }

    CString cszRenderString;
    cszRenderString.FormatMessage(ADVDETAILS_ACTION_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel);

    return pParentNode->AppendElementHtml(cszRenderString, -1);
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionExcludeElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::BUTTON_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;

	bool bAccessControl = false;
	do // intentional scoping
	{
		ui::IDataPtr spData;
		HRESULT hr = m_spMessage->GetData(AV::AVDATA_USER_SID_STRING, spData);
		if(FAILED(hr))
			break;

		CString cszMessageDataSID ;
		CFormatDataToString::DataToString(spData, _T("%s"), cszMessageDataSID);
		bAccessControl = CAntivirusMessageBase::m_AccessControl.AllowExcludeItem(cszMessageDataSID);

	}while(false);

	if(!bAccessControl)
	{
		::MessageBox(::GetActiveWindow(), _S(IDS_ERR_INSUFFICIENT_RIGHTS), _S(IDS_TITLE_EXCLUDE_SECURITY_RISK), MB_OK);
		return S_OK;
	}

    CString cszData;
    HRESULT hr = CFormatDataToString::DataToString(m_spData, _T("%s"), cszData);
    if(FAILED(hr))
        return E_UNEXPECTED;

    GUID guidThreatTrackId;
    hr = CLSIDFromString(CT2OLE(cszData), (LPCLSID)&guidThreatTrackId);
    if(FAILED(hr))
        return E_UNEXPECTED;

    INT nConfirm = ::MessageBox(GetActiveWindow(), _S(IDS_TEXT_EXCLUDE_AREYOUSURE), _S(IDS_TITLE_EXCLUDE_CONFIRMATION), MB_YESNO);

    if(IDYES == nConfirm)
    {
        for(;;)
        {
            if(m_pDoc == NULL)
            {
                CCTRCTXE0(L"m_pDoc == NULL");
                break;
            }

            symhtml::ISymHTMLElementPtr pElement;
            HRESULT hr = m_pDoc->GetElement(CT2A(m_cszHtmlId), &pElement);
            if(FAILED(hr) || pElement == NULL)
            {
                CCTRCTXE1(L"Failed to get element - 0x%08X", hr);
                break;
            }

            AVModule::AVLoader_IAVThreatInfo AvThreatInfoLoader;
            AVModule::IAVThreatInfoPtr spThreatInfo;
            AvThreatInfoLoader.CreateObject(spThreatInfo);
            if(!spThreatInfo)
            {
                CCTRACEW( CCTRCTX _T("AvModuleLoader::CreateObject() failed."));
                break;
            }

            AVModule::IAVMapDwordDataPtr spReturnInfo;
            spThreatInfo->ProcessAndWait(AVModule::ThreatProcessor::ProcessingAction_Exclude, guidThreatTrackId, spReturnInfo);
            m_spMessageProviderSink->OnMessageDataChanged(m_spMessage);

            break;
        }
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAdvDetailsActionSubmitElement::CAdvDetailsActionSubmitElement(void) : m_gSetId(GUID_NULL), m_bSubmitted(false), m_pDoc(NULL)
{
}

CAdvDetailsActionSubmitElement::~CAdvDetailsActionSubmitElement(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionSubmitElement::SetData(ui::IData* pData, LPCWSTR szFormat)
{
	ccLib::CExceptionInfo exInfo;
	HRESULT hr = S_OK;
	try 
	{
		for(;;)
		{
			if(pData == NULL)
			{
				hr = E_INVALIDARG;
				CCTRCTXE1(L"pData = NULL: 0x%08X", hr);
				break;
			}

			m_spData = pData;

			if(szFormat)
				m_cszFormat = szFormat;
			else
				CCTRCTXW0(L"szFormat == NULL");

			ccLib::CString sData;
			hr = CFormatDataToString::DataToString(m_spData, m_cszFormat, sData);
			if(FAILED(hr))
			{
				CCTRCTXE1(L"Failed to get string: 0x%08X", hr);
				break;
			}
			CCTRCTXI1(L"Set Id: %s", sData);
			hr = ::IIDFromString((LPOLESTR)ATL::CT2WEX<>(sData), &m_gSetId);
			if(FAILED(hr))
			{
				CCTRCTXE1(L"Failed to get IID: 0x%08X", hr);
				break;
			}	
			
			//we've carefully declared everything here such that:
			// - child objects from QB get destroyed first
			// - QB gets uninitialized
			// - QB gets destroyed
			// - QB gets unloaded

			QBackup::qb_IQBackup qbLoader;
			QBackup::IQBackupPtr pBackup;
			
			//be sure uninit gets called
			struct _smart_qb {
				QBackup::IQBackup*& p;
				_smart_qb(QBackup::IQBackup*& pp) : p(pp) {}
				~_smart_qb() {if(p!=NULL) p->Uninitialize();}
			} smart_qb(pBackup.m_p);

			QBackup::IQBackupSetPtr pSet;
			cc::IKeyValueCollectionPtr pUserData;

			SYMRESULT sr = qbLoader.CreateObject(pBackup.m_p);
			if(SYM_FAILED(sr))
			{
				CCTRCTXE1(L"Failed to create QBackup manager: 0x%08X", sr);
				hr = MCF::HRESULT_FROM_SYMRESULT(sr);
				break;
			}
			hr = pBackup->Initialize(NULL);
			if(FAILED(hr))
			{
				CCTRCTXE1(L"Failed to init QBackup Manager: 0x%08X", hr);
				break;
			}

			hr = pBackup->RetrieveBackupSetByID(m_gSetId, pSet.m_p);
			if(FAILED(hr) || !pSet)
			{
				CCTRCTXE1(L"Failed to get backup set: 0x%08X", hr);
				break;
			}
			hr = pSet->GetProperties(pUserData.m_p);
			if(FAILED(hr) || pUserData == NULL)
			{
				CCTRCTXW1(L"No User Data: 0x%08X", hr);
				break;
			}

			(void) pUserData->GetValue(avQBTypes::avThreatProp_Submitted, m_bSubmitted);

			CCTRCTXI2(L"Success: 0x%08X. Submitted = %s", hr, (m_bSubmitted ? L"TRUE" : L"FALSE"));
			break;
		}
	}
	CCCATCHMEM(exInfo)
	if(exInfo.IsException())
		return E_OUTOFMEMORY;

	if(FAILED(hr))
		CCTRCTXE1(L"SetData fails: 0x%08X",  hr);

    return hr;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionSubmitElement::SetLabel(LPCWSTR szLabel)
{
    m_cszLabel = CW2T(szLabel);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionSubmitElement::SetHtmlId(LPCWSTR szHtmlId)
{
    m_cszHtmlId = CW2T(szHtmlId);
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionSubmitElement::SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc)
{
    m_pDoc = pDoc;
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionSubmitElement::Render(symhtml::ISymHTMLElement* pParentNode)
{
    HRESULT hr;
    ui::IDataPtr spData;
    hr = m_spMessage->GetData(AV::AVDATA_SUPPORTS_SUBMISSION, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bSupportsSubmission = false;
    CFormatDataToBool::DataToBool(spData, bSupportsSubmission);
    if(!bSupportsSubmission)
        return S_FALSE;
	
    spData.Release();
    hr = m_spMessage->GetData(AV::AVDATA_RECOMMENDED_ACTION, spData);
    if(FAILED(hr))
        return S_FALSE;

    bool bRecommendedAction = false;
    ui::INumberDataQIPtr spRecommendedAction = spData;
    if(spRecommendedAction)
    {
        LONGLONG qdwNumberData;
        hr = spRecommendedAction->GetNumber(qdwNumberData);
        if(SUCCEEDED(hr))
        {
            if(qdwNumberData == AV::RecommendedAction::eSubmit)
                bRecommendedAction = true;
        }

    }

    if(pParentNode == NULL)
	{
		CCTRCTXE0(L"pParentNode == NULL");
		return E_INVALIDARG;
	}

	ccLib::CString sText;
	hr = GetHtmlText(sText, true);
	if(SUCCEEDED(hr))
		hr = pParentNode->AppendElementHtml(sText, -1);
	return hr;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionSubmitElement::OnAction(UINT uAction, LPCWSTR wszID)
{
    if(symhtml::event::BUTTON_CLICK != uAction || NULL == wszID || 0 != m_cszHtmlId.CompareNoCase(CW2T(wszID)))
        return S_FALSE;

	AVModule::AVLoader_IAVThreatInfo avLoader;
	ccLib::CExceptionInfo exInfo;
	HRESULT hr = S_OK;
	try
	{
		for(;;)
		{
			WTL::CWaitCursor wc;
			
			AVModule::IAVThreatInfoPtr pThreatInfo;
			AVModule::IAVMapDwordDataPtr pDataMap;
			SYMRESULT sr = avLoader.CreateObject(pThreatInfo);
			if(SYM_FAILED(sr) || pThreatInfo == NULL)
			{
				CCTRCTXE1(L"avLoader::CreateObject() failed.", sr);
				hr = HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
				break;
			}
			hr = pThreatInfo->GetSingleThreat(m_gSetId, pDataMap.m_p);
			if(FAILED(hr) || pDataMap == NULL)
			{
				if(SUCCEEDED(hr)) hr = E_POINTER;
				CCTRCTXE1(L"Failed to get threat: 0x%08X", hr);
				break;
			}
		    
			AVSubmit::IAVSubmissionManagerPtr pSubmission;
			sr = AVSubmit::AVSubmit_IAVSubmissionManagerMgd::CreateObject(GETMODULEMGR(), pSubmission.m_p);
			if(SYM_FAILED(sr))
			{
				CCTRCTXE1(L"failed to load submission mgr: 0x%08X", sr);
				hr = MCF::HRESULT_FROM_SYMRESULT(sr);
				break;
			}

			hr = pSubmission->Initialize(AVSubmit::IAVSubmissionManager::eSynchronous);
			if(FAILED(hr))
			{
				CCTRCTXE1(L"Failed to init submission manager: 0x%08X", hr);
				break;
			}

			if(pDataMap != NULL)
			{
				hr = pSubmission->ManualAdd(pDataMap);
			}
			else
			{
				CCTRCTXE0(_T("pDataMap ==  NULL"));
				hr = E_POINTER;
				//fall through, display error
			}

			wc.Restore();

			HWND hWnd = NULL;
			HRESULT hrHwnd = GetHwnd(hWnd);
			if(FAILED(hrHwnd))
			{
				CCTRCTXE1(_T("Failed to get hWnd: 0x%08X"), hr);
				hWnd = NULL;
			}
			else
				CCTRCTXI1(_T("got hWnd = %p"), hWnd);

			if(FAILED(hr))//&& hr != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
			{
				//TODO: CED?
				CCTRCTXE1(L"Failed to add submission: 0x%08X", hr);
				ccLib::CString sError;
				sError.LoadString(IDS_TEXT_SUBMISSION_REJECTED);
				MessageBox(hWnd, sError, CISVersion::GetProductName(), MB_OK);
				break;
			}

			hr = S_OK;

			//since our submission is successful, drawing ui will not effect the returnval of this function
			m_bSubmitted = true;

			ccLib::CString sMessage;
			sMessage.LoadString(IDS_TEXT_SUBMISSION_ACCEPTED);
			MessageBox(hWnd, sMessage, CISVersion::GetProductName(), MB_OK);

			if(m_pDoc == NULL)
			{
				CCTRCTXE0(L"m_pDoc == NULL");
				break;
			}
			symhtml::ISymHTMLElementPtr pElm, pParent;
			ccLib::CStringA sId = ATL::CT2AEX<>(m_cszHtmlId);
			sId += "-outer";
			HRESULT hrInternal = m_pDoc->GetElement(sId, &pElm.m_p);
			if(FAILED(hrInternal) || pElm == NULL)
			{
				CCTRCTXE1(L"Failed to get element - 0x%08X", hrInternal);
				break;
			}

			ccLib::CString sText;
			hrInternal = GetHtmlText(sText, false);
			if(FAILED(hrInternal))
				break; //out of memory;

			hrInternal = pElm->ReplaceElementHtml(sText, sText.GetLength());
			if(FAILED(hrInternal))
			{
				CCTRCTXE1(L"ReplaceElementHtml == 0x%08X", hrInternal);
				break;
			}

			hrInternal = pElm->UpdateElement(true);
			if(FAILED(hrInternal))
			{
				CCTRCTXE1(_T("Render == 0x%08X"), hrInternal);
				break;
			}

            
            symhtml::ISymHTMLElementPtr spRootElement;
            hrInternal = m_pDoc->GetRootElement(&spRootElement);
            if(FAILED(hr) || !spRootElement)
            {
                CCTRACEE( CCTRCTX _T("Problem getting document root element. hr=0x%08X"), hrInternal);
                break;
            }

            cc::IStringPtr spHtmlString;
            spRootElement->GetElementHtml(&spHtmlString, TRUE);

            CCTRACEI( CCTRCTX _T("%s"), CW2T(spHtmlString->GetStringW()) );

			break;
		}
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
		hr = E_OUTOFMEMORY;

    return hr;
}

//****************************************************************************
//****************************************************************************
HRESULT CAdvDetailsActionSubmitElement::GetHtmlText(ccLib::CString& sText, bool bLoading)
{
	ccLib::CExceptionInfo exInfo;
	try
	{
		if(m_bSubmitted)
		{
			ccLib::CStringW sCaption;
			sCaption.LoadString(IDS_TEXT_SUBMITTED);
			sText.FormatMessage(ADVDETAILS_NO_ACTION_ENTRY_FORMAT_STRING, m_cszHtmlId, sCaption);
		}
		else
		{
			sText.FormatMessage(ADVDETAILS_ACTION_ENTRY_FORMAT_STRING, m_cszHtmlId, m_cszLabel);
		}
		if(bLoading)
		{
			ccLib::CString sTemp;
			sTemp.FormatMessage(ADVDETAILS_ACTION_ENTRY_FORMAT_STRING_OUTER, m_cszHtmlId, sText);
			sText = sTemp;
		}
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
		return E_OUTOFMEMORY;

	return S_OK;
}

HRESULT CAdvDetailsActionSubmitElement::GetHwnd(HWND& hWnd)
{
	if(m_pDoc == NULL)
	{
		CCTRCTXE0(_T("m_pDoc == NULL"));
		return E_POINTER;
	}

	HWND hWndTemp = NULL;
	symhtml::ISymHTMLElementPtr pRoot;
    HRESULT hr = m_pDoc->GetRootElement(&pRoot);
    if(FAILED(hr) || pRoot == NULL)
    {
		hr = SUCCEEDED(hr) ? E_POINTER : hr;
		CCTRCTXE1(_T("Failed to get root element: 0x%08X)"), hr);
        return hr;
    }

    hr = pRoot->GetElementHwnd(&hWndTemp, TRUE);
    if(FAILED(hr))
    {
		CCTRCTXE1(_T("Failed to get HWND from the root element: 0x%08X"), hr);
        return hr;
    }

	CCTRCTXI1(_T("Root hWnd = %p"), hWndTemp);
	hWnd = hWndTemp;
	if(hWnd == NULL)
	{
		CCTRCTXE0(_T("hWnd == NULL"));
		hr = E_POINTER;
	}
    return hr;

}


//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
HRESULT CFormatDataToString::DataToString(ui::IData* pSymData, LPCWSTR szFormatString, CString& cszFormatted)
{
    HRESULT hrReturn = E_INVALIDARG, hr;

    ui::IData::DataType iDataType = ui::IData::eInvalidDataType;
    hr = pSymData->GetDataType(iDataType);
    if(FAILED(hr))
        return E_INVALIDARG;
    
    if(ui::IData::eNumber == iDataType)
    {
        for(;;)
        {
            LONGLONG qdwNumberData;
            ui::INumberDataQIPtr spNumberData = pSymData;
            if(!spNumberData)
                break;

            hr = spNumberData->GetNumber(qdwNumberData);
            if(FAILED(hr))
                break;
            
            if(!szFormatString)
                break;
            
            cszFormatted.Format(szFormatString, qdwNumberData);
            hrReturn = S_OK;
            break;
        }
    }
    else if(ui::IData::eString == iDataType)
    {
        for(;;)
        {
            cc::IStringPtr spString;
            ui::IStringDataQIPtr spStringData = pSymData;
            if(!spStringData)
                break;

            hr = spStringData->GetString(spString);
            if(FAILED(hr))
                break;

            if(!szFormatString)
                break;

            cszFormatted.Format(szFormatString, spString->GetStringW());
            hrReturn = S_OK;
            break;
        }
    }
    else if(ui::IData::eDate == iDataType)
    {
        for(;;)
        {
            SYSTEMTIME stTime;
            ui::IDateDataQIPtr spDateData = pSymData;
            if(!spDateData)
                break;

            hr = spDateData->GetDate(stTime);
            if(FAILED(hr))
                break;
            
            if(!szFormatString)
                break;

            CTime ctTime;
            cszFormatted = ctTime.Format(szFormatString);
            hrReturn = S_OK;
            break;
        }
    }

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CFormatDataToString::DataToString(MCF::IMessage* pMessage, const SYMGUID& guidDataId, LPCWSTR szFormatString, CString& cszFormatted)
{
    if (!pMessage || !szFormatString)
		return E_INVALIDARG;

    HRESULT hrReturn = E_INVALIDARG;

    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
		ui::IData::DataType iDataType;
		pMessage->GetDataType(guidDataId, iDataType);
		unsigned nType = static_cast<unsigned>(iDataType);

		switch(nType)
		{
			case ui::IData::eNumber:
				{
					ui::IDataPtr spData;
					pMessage->GetData(guidDataId, spData);
					ui::INumberDataQIPtr spNumberData(spData);
					if(!spNumberData)
						throw _com_error(E_FAIL);

					LONGLONG qdwNumberData = 0;
					hrx << spNumberData->GetNumber(qdwNumberData);
			        
					cszFormatted.Format(szFormatString, qdwNumberData);
					hrReturn = S_OK;
				}
				break;
			
			case ui::IData::eString:
				{
					//
					// Optimized for lots of calls and preventing heap allocations.
					//
					LPCWSTR szValue = NULL;
					hrx << pMessage->GetString(guidDataId, szValue);
					cszFormatted.Format(szFormatString, szValue);

					hrReturn = S_OK;
					break;
				}

			case ui::IData::eDate:
				{
					ui::IDataPtr spData;
					pMessage->GetData(guidDataId, spData);
					ui::IDateDataQIPtr spDateData(spData);
					if(!spDateData)
						throw _com_error(E_FAIL);

					SYSTEMTIME stTime;
					hrx << spDateData->GetDate(stTime);

					CTime ctTime;
					cszFormatted = ctTime.Format(szFormatString);
					hrReturn = S_OK;
				}
				break;
			default:
				CCTRCTXE0(L"Unhandled data type");
		}
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}
//****************************************************************************
//****************************************************************************
HRESULT CFormatDataToBool::DataToBool(ui::IData* pSymData, bool& bData)
{
    HRESULT hrReturn = E_INVALIDARG, hr;

    ui::IData::DataType iDataType = ui::IData::eInvalidDataType;
    hr = pSymData->GetDataType(iDataType);
    if(FAILED(hr))
        return E_INVALIDARG;

    if(ui::IData::eNumber == iDataType)
    {
        for(;;)
        {
            LONGLONG qdwNumberData;
            ui::INumberDataQIPtr spNumberData = pSymData;
            if(!spNumberData)
                break;

            hr = spNumberData->GetNumber(qdwNumberData);
            if(FAILED(hr))
                break;

            if(0 == qdwNumberData)
                bData = false;
            else
                bData = true;

            hrReturn = S_OK;
            break;
        }
    }
    else if(ui::IData::eString == iDataType)
        hrReturn = E_INVALIDARG; // doesn't handle strings
    else if(ui::IData::eDate == iDataType)
        hrReturn = E_INVALIDARG; // doesn't handle time/date
    else
        hrReturn = E_INVALIDARG; // doesn't handle unknown

    return hrReturn;
}

//****************************************************************************
//  ThreadProc for ShellExecuteWorker
//****************************************************************************
unsigned __stdcall _ShellExecuteWorkerFunc( void* pArguments )
{
    // make sure some data was passed in
    if(NULL == pArguments)
        return ERROR_FILE_NOT_FOUND;

    // init COM STA, required for Shell functions
    ccLib::CCoInitialize cCoInit(ccLib::CCoInitialize::eSTAModel);

    // make the call and return
    SHELLEXECUTEINFO* pInfo = (SHELLEXECUTEINFO*)pArguments;
    
    UINT uRet = (UINT)::ShellExecute(pInfo->hwnd, pInfo->lpVerb, pInfo->lpFile, pInfo->lpParameters, pInfo->lpDirectory, pInfo->nShow);
    
    return uRet;
}

//****************************************************************************
//  This function is required because ShellExecute() fails on MTA threads.
//   This will spawn a worker thread, and init it STA, and make the call.  The
//   return value of ShellExecute() will be in the thread exit code.
//****************************************************************************
HRESULT ShellExecuteWorker(HWND hwnd, LPCTSTR szOperation, LPCTSTR szFile, LPCTSTR szParameters, LPCTSTR szDirectory, INT nShowCommand)
{
    HRESULT hrReturn = E_FAIL;

    // Use a SHELLEXECUTEINFO structure to store the data temporarily for the thread
    SHELLEXECUTEINFO cInfo;
    ZeroMemory(&cInfo, sizeof(SHELLEXECUTEINFO));
    cInfo.cbSize = sizeof(cInfo);

    cInfo.hwnd = hwnd;
    cInfo.lpVerb = szOperation;
    cInfo.lpFile = szFile;
    cInfo.lpParameters = szParameters;
    cInfo.lpDirectory = szDirectory;
    cInfo.nShow = nShowCommand;

    // start a thread and pass in the data
    //  (start using CREATE_SUSPENDED to guarantee a valid handle)
    DWORD dwThreadId = NULL;
    StahlSoft::CSmartHandle shThread = (HANDLE)_beginthreadex(NULL, 0, _ShellExecuteWorkerFunc, (LPVOID)&cInfo, CREATE_SUSPENDED, (UINT*)&dwThreadId);
    
    if(0 == shThread.m_h)
    {
        DWORD dwErr = GetLastError();
        CCTRACEE( CCTRCTX _T("Failed to create thread!! GetLastError() == %d"), dwErr);
        return E_FAIL;
    }
    
    // let the thread go
    ResumeThread(shThread);

    // Wait for the thread to return, shouldn't take very long
    DWORD dwRet = StahlSoft::WaitForSingleObjectWithMessageLoop(shThread, 30000);
    if(WAIT_OBJECT_0 == dwRet) // all good!!
    {
        BOOL bRet = GetExitCodeThread(shThread, &dwRet);
        if(bRet) // all good!!
        {
            if(32 > dwRet) // ShellExecute() must have failed, see MSDN
                hrReturn = HRESULT_FROM_WIN32(dwRet);
            else
                hrReturn = S_OK;
        }
        else // shouldn't happen
        {
            CCTRACEE( CCTRCTX _T("Unable to retrieve exit code from thread %d"), dwThreadId);
        }
    }
    else
    {
        // not good!!
        CCTRACEE( CCTRCTX _T("ShellExecuteWorkerFunc() did not return in time!!  Terminating."));
        TerminateThread(shThread, NULL);
    }

    return hrReturn;
}
