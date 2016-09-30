////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SymInterface.h>
#include <StockFrameworkObjectsInterface.h>
#include <MCFWidgets.h>

class CQuarantineMenu :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public MCF::CSymObjectCreator<&MCF::IID_Menu, MCF::IMenu, CQuarantineMenu>,
    public MCF::IMenu

{
public:
    CQuarantineMenu(void);
    virtual ~CQuarantineMenu(void);

public: // Message Map
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(MCF::IID_Menu, MCF::IMenu)
    SYM_INTERFACE_MAP_END()    

public: // MCF::IMenu
    virtual HRESULT GetMenuHandle(HMENU &hMenu) throw();    
    virtual HRESULT OnMenuCommand(UINT id) throw();
    virtual HRESULT OnCloseMenu() throw();
    virtual HRESULT SetAppServer(ISymBase* pIApplication) throw();
    virtual HRESULT GetTooltipText(UINT nId, cc::IString*& pString) throw();
	virtual HRESULT OnStatus(DWORD dwListCnt) throw();

public: // methods
    void SetMessageProviderSink(MCF::IMessageProviderSink* pSink);
    void CreateQuarantineItem(REFGUID guidThreatTrackId);

protected:
    CMenu m_Menu;
    MCF::IMessageProviderSinkPtr m_pMessageProviderSink;
};
