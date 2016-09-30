////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "MCFWidgets.h"
#include "StockFrameworkObjectsInterface.h"

#include "SymHTML.h"
#include "SymHTMLDocumentImpl.h"

#include "resource.h"

#include "uiAvProviderElements.h"


class CMessageDetails :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public symhtml::CSymHTMLDocumentImpl,
    public MCF::IApplicationClient

{
public:
    CMessageDetails(void);
    virtual ~CMessageDetails(void);

    
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
        SYM_INTERFACE_ENTRY(MCF::IID_ApplicationClient, MCF::IApplicationClient)
    SYM_INTERFACE_MAP_END()    
    
    BEGIN_MSG_MAP(CMessageDetails)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CMessageDetails)
    END_HTML_EVENT_MAP()

public: // IApplicationClient
    virtual HRESULT OnApply() throw();    
    virtual HRESULT OnClose() throw();
    virtual HRESULT OnCancel() throw();
    virtual HRESULT SetAppServer(MCF::IApplication* pAppServer) throw();
    virtual HRESULT SupportsApply() throw();
    virtual HRESULT SupportsClose() throw();
    virtual HRESULT SupportsCancel() throw();

    virtual HRESULT OnHtmlEvent( symhtml::ISymHTMLElement* lpElement, LPCWSTR szEventName, UINT nEventType, ISymBase* pExtra ) throw();

public:
    MCF::IMessageQIPtr m_spMessage;
    MCF::IMessageProviderSinkPtr m_spMessageProviderSink;

    std::vector<CSymHtmlActionBasePtr> m_lstActions;

protected:
    MCF::IApplicationPtr m_spAppServer;

};

class CGenericDetails : 
    public MCF::CSymObjectCreator<&symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument, CGenericDetails>,
    public CMessageDetails
{
public: // CSymHTMLDocumentImpl overrides
    DECLARE_HTML_DOCUMENT(L"MCUI_DETAILS_MAIN.HTM");

public:
    virtual HRESULT OnDocumentComplete();

};

class CAutoProtectDetails : 
    public MCF::CSymObjectCreator<&symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument, CAutoProtectDetails>,
    public CMessageDetails
{
public:
    DECLARE_HTML_DOCUMENT(L"MCUI_DETAILS_MAIN.HTM");

public: // CSymHTMLDocumentImpl overrides
    virtual HRESULT OnDocumentComplete();

};

class CAutoProtectAdvancedDetails : 
    public MCF::CSymObjectCreator<&symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument, CAutoProtectAdvancedDetails>,
    public CMessageDetails
{
public:
    DECLARE_HTML_DOCUMENT(L"MCUI_ADVANCEDDETAILS_MAIN.HTM");

public: // CSymHTMLDocumentImpl overrides
    virtual HRESULT OnDocumentComplete();

};

class CScanResultsDetails : 
    public MCF::CSymObjectCreator<&symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument, CScanResultsDetails>,
    public CMessageDetails
{
public:
    DECLARE_HTML_DOCUMENT(L"MCUI_DETAILS_MAIN.HTM");

public: // CSymHTMLDocumentImpl overrides
    virtual HRESULT OnDocumentComplete();

};

class CScanResultsAdvancedDetails : 
    public MCF::CSymObjectCreator<&symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument, CScanResultsAdvancedDetails>,
    public CMessageDetails
{
public:
    DECLARE_HTML_DOCUMENT(L"MCUI_ADVANCEDDETAILS_MAIN.HTM");

public: // CSymHTMLDocumentImpl overrides
    virtual HRESULT OnDocumentComplete();

};

class CAvModuleItemDetails : 
    public MCF::CSymObjectCreator<&symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument, CAvModuleItemDetails>,
    public CMessageDetails
{
public:
    DECLARE_HTML_DOCUMENT(L"MCUI_DETAILS_MAIN.HTM");

public: // CSymHTMLDocumentImpl overrides
    virtual HRESULT OnDocumentComplete();

};

class CAvModuleItemAdvancedDetails : 
    public MCF::CSymObjectCreator<&symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument, CAvModuleItemAdvancedDetails>,
    public CMessageDetails
{
public:
    DECLARE_HTML_DOCUMENT(L"MCUI_ADVANCEDDETAILS_MAIN.HTM");

public: // CSymHTMLDocumentImpl overrides
    virtual HRESULT OnDocumentComplete();

};
