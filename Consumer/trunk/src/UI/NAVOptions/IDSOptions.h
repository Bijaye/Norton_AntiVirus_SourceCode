////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SymInterface.h"
#include "SymHTMLLoader.h"
#include "SymHTMLMouseEvent.h"
#include "SymHTML.h"
#include "SymHTMLDocumentImpl.h"
#include "SymHTMLDialogDocumentImpl.h"
#include "ccSettingsInterface.h"
#include "IDSUIInterface.h"
#include "IDSUILoader.h"
#include "SNDHelper.h"

 /**
  * This class implements the IDS General Settings dialog
  */

class CIDSOptions :
	public symhtml::CSymHTMLDocumentImpl,
	public symhtml::CSymHTMLDialogDocumentImpl,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    DECLARE_HTML_DOCUMENT(L"optionsids.htm")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument )
		SYM_INTERFACE_ENTRY( symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument )
    SYM_INTERFACE_MAP_END()

	CIDSOptions();
	virtual ~CIDSOptions();

//    BEGIN_MSG_MAP(CIDSOptions)
//    END_MSG_MAP()

	BEGIN_HTML_EVENT_MAP( CIDSOptions )
		HTML_EVENT_BUTTON_CLICK(L"ok", OnSave)
		HTML_EVENT_BUTTON_CLICK(L"cancel", OnCancel)
		HTML_EVENT_BUTTON_CLICK(L"default", OnDefault)
		HTML_EVENT_HYPERLINK( L"IDS_HELP", OnHelp)
	END_HTML_EVENT_MAP()

	// SymHTML methods
	virtual HRESULT OnDocumentComplete() throw();
	virtual HRESULT OnNewIFrame( symhtml::ISymHTMLElement* lpElement, symhtml::ISymHTMLWindow* lpNewFrame ) throw();

	// main interface
	HRESULT Initialize(HWND hWndParent, LPCWSTR szTitle, HICON hIcon);
	HRESULT ShowModalDialog();

protected:

	// Event handlers
	HRESULT OnSave(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
	HRESULT OnCancel(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
	HRESULT OnDefault(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
	HRESULT OnHelp(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);

private:
	// remove from interface
	CIDSOptions(const CIDSOptions &);
	CIDSOptions & operator =(const CIDSOptions &);

	//The parent windows
	HWND m_hWndParent;
	//The wizard title
	CString m_strTitle;
	//The dialog icon
	HICON m_hIcon;
	//The dialog width
	int m_nWidth;
	//The dialog height
	int m_nHeight;

	idsui::IDSAutoBlockLoader m_loader_IDSUI;

	symhtml::ISymHTMLWindowPtr m_spIFrameWnd;
	idsui::IIDSAutoBlockUIPtr m_spIDS;
	symhtml::ISymHTMLDocument * m_pIDSDoc;
	bool m_bInitialized;
};
