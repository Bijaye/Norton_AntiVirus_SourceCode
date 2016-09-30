// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SymHtml.h"
#include "SymHTMLElement.h"
#include "SymHtmlDocumentImpl.h"

#include <uiElementInterface.h>
#include <MessageCenterFrameworkInterface.h>
#include <StockFrameworkObjectsInterface.h>
#include <MCFWidgets.h>
#include <StockData.h>
#include <ScanTaskLoader.h>

class CFormatDataToString
{
public:
    static HRESULT DataToString(ui::IData* pSymData, LPCWSTR szFormat, CString& cszFormatted);
	static HRESULT DataToString(MCF::IMessage* pMessage, const SYMGUID& guidDataId, LPCWSTR szFormatString, CString& cszFormatted);
};

class CFormatDataToBool
{
public:
    static HRESULT DataToBool(ui::IData* pSymData, bool& bData);
};


class CElementBase
{
public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode) = 0;

};

class IStringListData : public ISymBase
{
	// GetDataType should return eString 
public:
	// get the string (return copy)
	virtual HRESULT GetString(DWORD dwIndex, cc::IString*& pString) const throw() = 0; // IClone->Clone()
	virtual HRESULT GetCount(DWORD& dwCount) const throw() = 0;

	// set the string
	virtual HRESULT AddString(LPCWSTR wszString) const throw() = 0;
	virtual HRESULT AddString(const cc::IString* pString) const throw() = 0;
};


// {795D7634-CF70-4f6d-98EF-2B601C234D4B}
SYM_DEFINE_INTERFACE_ID(IID_StringListData, 
			0x795d7634, 0xcf70, 0x4f6d, 0x98, 0xef, 0x2b, 0x60, 0x1c, 0x23, 0x4d, 0x4b);

TYPEDEF_SYM_POINTERS(IStringListData, IID_StringListData);

//
// CStockStringListData Class ID
//
// {8B60551E-04CB-4e9c-8B66-67219AD96E75}
SYM_DEFINE_INTERFACE_ID(CLSID_StockStringListData, 
			0x8b60551e, 0x4cb, 0x4e9c, 0x8b, 0x66, 0x67, 0x21, 0x9a, 0xd9, 0x6e, 0x75);

class CStockStringListData : 
	public ISymBaseImpl<CSymThreadSafeRefCount>,
	public MCF::CSymObjectCreator<&IID_StringListData, IStringListData, CStockStringListData>,
	public IStringListData,
	MCF::CStockDataImpl
{
public:
	CStockStringListData(void);
	virtual ~CStockStringListData(void);

	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY(ui::IID_Data, ui::IData)
		SYM_INTERFACE_ENTRY(MCF::IID_DataInternal, MCF::IDataInternal)
		SYM_INTERFACE_ENTRY(IID_StringListData, IStringListData)
	SYM_INTERFACE_MAP_END()    

public: // IStringListData interface
	// get the string (return copy)
	virtual HRESULT GetString(DWORD dwIndex, cc::IString*& pString) const throw(); // IClone->Clone()
	virtual HRESULT GetCount(DWORD& dwCount) const throw();

	// set the string
	virtual HRESULT AddString(LPCWSTR wszString) const throw();
	virtual HRESULT AddString(const cc::IString* pString) const throw();

protected: // member variables
	MCF::ISymBaseListInternalPtr m_spStringList;

};

class CSymHtmlActionBase
{
public:
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID) = 0;
};

typedef StahlSoft::CRefCountT<CSymHtmlActionBase> CRefCountSymHtmlActionBase;
typedef StahlSoft::CSmartRefCountPtr<CRefCountSymHtmlActionBase> CSymHtmlActionBasePtr;

class CSummaryPageElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CSummaryPageElement(void);
	virtual ~CSummaryPageElement(void);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
};
typedef StahlSoft::CSmartRefCountPtr<CSummaryPageElement> CSummaryPageElementPtr;

class CSummaryEntryElement :
    public StahlSoft::CRefCount,
    public CElementBase
{
public:
    CSummaryEntryElement(void);
    virtual ~CSummaryEntryElement(void);
    
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);
    HRESULT SetLabel(LPCWSTR szLabel);
    HRESULT SetHtmlId(LPCWSTR szHtmlId);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CSummaryEntryElement> CSummaryEntryElementPtr;

class CScanSummaryEntryElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CScanSummaryEntryElement(void);
	virtual ~CScanSummaryEntryElement(void);

	HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);
	HRESULT SetLabel(LPCWSTR szLabel);
	HRESULT SetHtmlId(LPCWSTR szHtmlId);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	ui::IDataPtr m_spData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CScanSummaryEntryElement> CScanSummaryEntryElementPtr;

class CScanSummaryEntrySumElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CScanSummaryEntrySumElement(void);
	virtual ~CScanSummaryEntrySumElement(void);

	HRESULT AddData(ui::IData* pData);
	HRESULT SetLabel(LPCWSTR szLabel);
	HRESULT SetHtmlId(LPCWSTR szHtmlId);
	
public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	LONGLONG m_qdwData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CScanSummaryEntrySumElement> CScanSummaryEntrySumElementPtr;

class CScanSummaryTimeSpanEntryElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CScanSummaryTimeSpanEntryElement(void);
	virtual ~CScanSummaryTimeSpanEntryElement(void);

	HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);
	HRESULT SetLabel(LPCWSTR szLabel);
	HRESULT SetHtmlId(LPCWSTR szHtmlId);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	ui::IDataPtr m_spData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CScanSummaryTimeSpanEntryElement> CScanSummaryTimeSpanEntryElementPtr;

class CSummaryEntryRecommendedElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CSummaryEntryRecommendedElement(void);
	virtual ~CSummaryEntryRecommendedElement(void);

	HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);
	HRESULT SetLabel(LPCWSTR szLabel);
	HRESULT SetHtmlId(LPCWSTR szHtmlId);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	ui::IDataPtr m_spData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CSummaryEntryRecommendedElement> CSummaryEntryRecommendedElementPtr;

class CSummaryEntryCompactPathElement :
    public StahlSoft::CRefCount,
    public CElementBase
{
public:
    CSummaryEntryCompactPathElement(void);
    virtual ~CSummaryEntryCompactPathElement(void);

    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);
    HRESULT SetLabel(LPCWSTR szLabel);
    HRESULT SetHtmlId(LPCWSTR szHtmlId);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CSummaryEntryCompactPathElement> CSummaryEntryCompactPathElementPtr;

class CSummaryEntryRiskLevelElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CSummaryEntryRiskLevelElement(void);
	virtual ~CSummaryEntryRiskLevelElement(void);

public:
	HRESULT SetHtmlId(LPCWSTR wszHtmlId);
	HRESULT SetLabel(LPCWSTR wszLabel);
	HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	ui::IDataPtr m_spData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CSummaryEntryRiskLevelElement> CSummaryEntryRiskLevelElementPtr;

class CAdvancedDetailsPageElement :
    public StahlSoft::CRefCount,
    public CElementBase
{
public:
    CAdvancedDetailsPageElement(void);
    virtual ~CAdvancedDetailsPageElement(void);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
};
typedef StahlSoft::CSmartRefCountPtr<CAdvancedDetailsPageElement> CAdvancedDetailsPageElementPtr;

class CDetailsSectionHeaderElement :
    public StahlSoft::CRefCount,
    public CElementBase
{
public:
    CDetailsSectionHeaderElement(void);
    virtual ~CDetailsSectionHeaderElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    virtual HRESULT SetHeaderText(LPCWSTR wszHeaderText);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
    CString m_cszHtmlId;
    CString m_cszHeaderText;
};
typedef StahlSoft::CSmartRefCountPtr<CDetailsSectionHeaderElement> CDetailsSectionHeaderElementPtr;

class CActionSectionTextElement :
    public StahlSoft::CRefCount,
    public CElementBase
{
public:
    CActionSectionTextElement(void);
    virtual ~CActionSectionTextElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    virtual HRESULT SetLabel(LPCWSTR wszText);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // data members
    MCF::IMessageQIPtr m_spMessage;

protected:
    CString m_cszHtmlId;
    CString m_cszText;
};
typedef StahlSoft::CSmartRefCountPtr<CActionSectionTextElement> CActionSectionTextElementPtr;

class CActionSectionRecommendedTextElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CActionSectionRecommendedTextElement(void);
	virtual ~CActionSectionRecommendedTextElement(void);

public:
	HRESULT SetHtmlId(LPCWSTR wszHtmlId);
	virtual HRESULT SetLabel(LPCWSTR wszText);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // data members
	MCF::IMessageQIPtr m_spMessage;

protected:
	CString m_cszHtmlId;
	CString m_cszText;
};
typedef StahlSoft::CSmartRefCountPtr<CActionSectionRecommendedTextElement> CActionSectionRecommendedTextElementPtr;


class CTroubleshootSectionHeaderElement :
    public StahlSoft::CRefCount,
    public CElementBase
{
public:
    CTroubleshootSectionHeaderElement(void);
    virtual ~CTroubleshootSectionHeaderElement(void);

public:
    HRESULT SetData(ui::IData* pData);
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    virtual HRESULT SetHeaderText(LPCWSTR wszHeaderText);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
    ui::IDataPtr m_spData;
    CString m_cszHtmlId;
    CString m_cszHeaderText;
};
typedef StahlSoft::CSmartRefCountPtr<CTroubleshootSectionHeaderElement> CTroubleshootSectionHeaderElementPtr;

class CTableElement :
    public StahlSoft::CRefCount,
    public CElementBase
{
public:
    CTableElement(void);
    virtual ~CTableElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetClassId(LPCWSTR wszClassId);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
    CString m_cszHtmlId;
    CString m_cszClassId;
};
typedef StahlSoft::CSmartRefCountPtr<CTableElement> CTableElementPtr;

class CAdvDetailsEntryElement :
    public StahlSoft::CRefCount,
    public CElementBase
{
public:
    CAdvDetailsEntryElement(void);
    virtual ~CAdvDetailsEntryElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsEntryElement> CAdvDetailsEntryElementPtr;

class CAdvDetailsEntryTimeSpanElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CAdvDetailsEntryTimeSpanElement(void);
	virtual ~CAdvDetailsEntryTimeSpanElement(void);

public:
	HRESULT SetHtmlId(LPCWSTR wszHtmlId);
	HRESULT SetLabel(LPCWSTR wszLabel);
	HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	ui::IDataPtr m_spData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsEntryTimeSpanElement> CAdvDetailsEntryTimeSpanElementPtr;

class CAdvDetailsEntryStringListElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CAdvDetailsEntryStringListElement(void);
	virtual ~CAdvDetailsEntryStringListElement(void);

public:
	HRESULT SetHtmlId(LPCWSTR wszHtmlId);
	HRESULT SetLabel(LPCWSTR wszLabel);
	HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	ui::IDataPtr m_spData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsEntryStringListElement> CAdvDetailsEntryStringListElementPtr;

class CAdvDetailsEntryRiskLevelElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CAdvDetailsEntryRiskLevelElement(void);
	virtual ~CAdvDetailsEntryRiskLevelElement(void);

public:
	HRESULT SetHtmlId(LPCWSTR wszHtmlId);
	HRESULT SetLabel(LPCWSTR wszLabel);
	HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	ui::IDataPtr m_spData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsEntryRiskLevelElement> CAdvDetailsEntryRiskLevelElementPtr;

class CAdvDetailsShowThreatDetailsElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsShowThreatDetailsElement(void);
    virtual ~CAdvDetailsShowThreatDetailsElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // data members
	MCF::IMessageQIPtr m_spMessage;

protected:
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsShowThreatDetailsElement> CAdvDetailsShowThreatDetailsElementPtr;

class CAdvDetailsCompactPathEntryElement :
    public StahlSoft::CRefCount,
    public CElementBase
{
public:
    CAdvDetailsCompactPathEntryElement(void);
    virtual ~CAdvDetailsCompactPathEntryElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public:
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsCompactPathEntryElement> CAdvDetailsCompactPathEntryElementPtr;

class CAdvDetailsSumEntryElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CAdvDetailsSumEntryElement(void);
	virtual ~CAdvDetailsSumEntryElement(void);

	HRESULT AddData(ui::IData* pData);
	HRESULT SetLabel(LPCWSTR szLabel);
	HRESULT SetHtmlId(LPCWSTR szHtmlId);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	LONGLONG m_qdwData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsSumEntryElement> CAdvDetailsSumEntryElementPtr;

class CAdvDetailsSubSumEntryElement :
	public StahlSoft::CRefCount,
	public CElementBase
{
public:
	CAdvDetailsSubSumEntryElement(void);
	virtual ~CAdvDetailsSubSumEntryElement(void);

	HRESULT AddData(ui::IData* pData);
	HRESULT SetLabel(LPCWSTR szLabel);
	HRESULT SetHtmlId(LPCWSTR szHtmlId);

public:
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

protected:
	LONGLONG m_qdwData;
	CString m_cszFormat;
	CString m_cszLabel;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsSubSumEntryElement> CAdvDetailsSubSumEntryElementPtr;

class CAdvDetailsTroubleshootUrlElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsTroubleshootUrlElement(void);
    virtual ~CAdvDetailsTroubleshootUrlElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);
    HRESULT SetTargetData(ui::IData* pData, LPCWSTR szFormat);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

protected: // data members
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    ui::IDataPtr m_spTargetData;
    CString m_cszTargetFormat;
    CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsTroubleshootUrlElement> CAdvDetailsTroubleshootUrlElementPtr;

class CAdvDetailsTroubleshootHelpElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsTroubleshootHelpElement(void);
    virtual ~CAdvDetailsTroubleshootHelpElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);
    HRESULT SetSymHelpId(DWORD dwSymHelpId);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

protected: // data members
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    DWORD m_dwSymHelpId;
    CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsTroubleshootHelpElement> CAdvDetailsTroubleshootHelpElementPtr;

class CAdvDetailsTroubleshootOptionsElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsTroubleshootOptionsElement(void);
    virtual ~CAdvDetailsTroubleshootOptionsElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);
	HRESULT SetISSharedElementId(const SYMGUID& guidElementId);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // data members
	MCF::IMessageQIPtr m_spMessage;

protected: // data members
	ui::IDataPtr m_spData;
    SYMGUID m_guidElementId;
	CString m_cszFormat;
    CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsTroubleshootOptionsElement> CAdvDetailsTroubleshootOptionsElementPtr;

class CAdvDetailsTroubleshootComponentOptionsElement :
	public CRefCountSymHtmlActionBase,
	public CElementBase
{
public:
	CAdvDetailsTroubleshootComponentOptionsElement(void);
	virtual ~CAdvDetailsTroubleshootComponentOptionsElement(void);

public:
	HRESULT SetHtmlId(LPCWSTR wszHtmlId);
	HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public: // CElementBase
	virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
	virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // data members
	MCF::IMessageQIPtr m_spMessage;

protected: // data members
	ui::IDataPtr m_spData;
	CString m_cszFormat;
	CString m_cszHtmlId;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsTroubleshootComponentOptionsElement> CAdvDetailsTroubleshootComponentOptionsElementPtr;

class CAdvDetailsActionRestoreElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsActionRestoreElement(void);
    virtual ~CAdvDetailsActionRestoreElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // methods
    HRESULT SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc);

public: // data members
    MCF::IMessageProviderSinkPtr m_spMessageProviderSink;
    MCF::IMessageQIPtr m_spMessage;

protected: // data members
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;
    //not smart ptr to prevent circular ref counting
    symhtml::CSymHTMLDocumentImpl* m_pDoc;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsActionRestoreElement> CAdvDetailsActionRestoreElementPtr;

class CAdvDetailsActionDeleteElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsActionDeleteElement(void);
    virtual ~CAdvDetailsActionDeleteElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // methods
    HRESULT SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc);

public: // data members
    MCF::IMessageProviderSinkPtr m_spMessageProviderSink;
    MCF::IMessageQIPtr m_spMessage;

protected: // data members
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;
    
    //not smart ptr to prevent circular ref counting
    symhtml::CSymHTMLDocumentImpl* m_pDoc;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsActionDeleteElement> CAdvDetailsActionDeleteElementPtr;

class CAdvDetailsActionLaunchQuickScanElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsActionLaunchQuickScanElement(void);
    virtual ~CAdvDetailsActionLaunchQuickScanElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // methods
    HRESULT SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc);

public: // data members
    MCF::IMessageProviderSinkPtr m_spMessageProviderSink;
    MCF::IMessageQIPtr m_spMessage;

protected: // methods
    HRESULT GenerateTempScanFile(CString& cszScanFileName);
    HRESULT GetInfectedFiles(GUID guidThreatTrackId, avScanTask::IScanTask* pScanTask);

protected: // data members
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;

    //not smart ptr to prevent circular ref counting
    symhtml::CSymHTMLDocumentImpl* m_pDoc;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsActionLaunchQuickScanElement> CAdvDetailsActionLaunchQuickScanElementPtr;

class CAdvDetailsActionRemoveElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsActionRemoveElement(void);
    virtual ~CAdvDetailsActionRemoveElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // methods
    HRESULT SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc);

public: // data members
    MCF::IMessageProviderSinkPtr m_spMessageProviderSink;
    MCF::IMessageQIPtr m_spMessage;

protected: // data members
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;

    //not smart ptr to prevent circular ref counting
    symhtml::CSymHTMLDocumentImpl* m_pDoc;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsActionRemoveElement> CAdvDetailsActionRemoveElementPtr;

class CAdvDetailsActionReviewRiskDetailsElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsActionReviewRiskDetailsElement(void);
    virtual ~CAdvDetailsActionReviewRiskDetailsElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // methods
    HRESULT SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc);

public: // data members
    MCF::IMessageProviderSinkPtr m_spMessageProviderSink;
    MCF::IMessageQIPtr m_spMessage;

protected: // data members
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;

    //not smart ptr to prevent circular ref counting
    symhtml::CSymHTMLDocumentImpl* m_pDoc;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsActionReviewRiskDetailsElement> CAdvDetailsActionReviewRiskDetailsElementPtr;

class CAdvDetailsActionExcludeElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsActionExcludeElement(void);
    virtual ~CAdvDetailsActionExcludeElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // methods
    HRESULT SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc);

public: // data members
    MCF::IMessageProviderSinkPtr m_spMessageProviderSink;
    MCF::IMessageQIPtr m_spMessage;

protected: // data members
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;

    //not smart ptr to prevent circular ref counting
    symhtml::CSymHTMLDocumentImpl* m_pDoc;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsActionExcludeElement> CAdvDetailsActionExcludeElementPtr;

class CAdvDetailsActionSubmitElement :
    public CRefCountSymHtmlActionBase,
    public CElementBase
{
public:
    CAdvDetailsActionSubmitElement(void);
    virtual ~CAdvDetailsActionSubmitElement(void);

public:
    HRESULT SetHtmlId(LPCWSTR wszHtmlId);
    HRESULT SetLabel(LPCWSTR wszLabel);
    HRESULT SetData(ui::IData* pData, LPCWSTR szFormat);
	HRESULT SetDocument(symhtml::CSymHTMLDocumentImpl* pDoc);

public: // CElementBase
    virtual HRESULT Render(symhtml::ISymHTMLElement* pParentNode);

public: // CSymHtmlActionBase
    virtual HRESULT OnAction(UINT uAction, LPCWSTR wszID);

public: // data members
    MCF::IMessageQIPtr m_spMessage;

protected:
	HRESULT GetHtmlText(ccLib::CString& sText, bool bLoading);
	HRESULT GetHwnd(HWND& hWnd);

protected: // data members
    ui::IDataPtr m_spData;
    CString m_cszFormat;
    CString m_cszLabel;
    CString m_cszHtmlId;
	GUID m_gSetId;
	bool m_bSubmitted;	
	//not smart ptr to prevent circular ref counting
	symhtml::CSymHTMLDocumentImpl* m_pDoc;
};
typedef StahlSoft::CSmartRefCountPtr<CAdvDetailsActionSubmitElement> CAdvDetailsActionSubmitElementPtr;


