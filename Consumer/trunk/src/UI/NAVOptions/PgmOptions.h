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
#include "ProgCtrlUIInterface.h"
#include "ProgCtrlUILoader.h"

 /**
  * This class implements the IDS General Settings dialog
  */

class CPgmOptions :
	public symhtml::CSymHTMLDocumentImpl,
	public symhtml::CSymHTMLDialogDocumentImpl,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    DECLARE_HTML_DOCUMENT(L"optionspgm.htm")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument )
		SYM_INTERFACE_ENTRY( symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument )
    SYM_INTERFACE_MAP_END()

	CPgmOptions ();
	virtual ~CPgmOptions ();

//    BEGIN_MSG_MAP(CPgmOptions)
//    END_MSG_MAP()

	BEGIN_HTML_EVENT_MAP( CPgmOptions  )
		HTML_EVENT_BUTTON_CLICK(L"ok", OnSave)
		HTML_EVENT_BUTTON_CLICK(L"cancel", OnCancel)
		HTML_EVENT_HYPERLINK( L"PGM_HELP", OnHelp)
	END_HTML_EVENT_MAP()

	// SymHTML methods
	virtual HRESULT OnDocumentComplete() throw();
	virtual HRESULT OnNewIFrame( symhtml::ISymHTMLElement* lpElement, symhtml::ISymHTMLWindow* lpNewFrame ) throw();

	// Event handlers
	HRESULT OnSave(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
	HRESULT OnCancel(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
	HRESULT OnHelp(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);

	// helpers
	HRESULT Initialize(HWND hWndParent, LPCWSTR szTitle, HICON hIcon, CSymPtr<CSNDHelper> & sndHelper);
	HRESULT ShowModalDialog();
	HRESULT LoadAppRules();

private:
	// remove from interface
	CPgmOptions (const CPgmOptions  &);
	CPgmOptions  & operator =(const CPgmOptions  &);

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

	CSymPtr<CSNDHelper> m_sndHelper;

	SymNeti::IFWListPtr m_spSNDApps;
	SymNeti::IFWListPtr m_spCopySNDApps;

	symhtml::ISymHTMLWindowPtr m_spIFrameWnd;
	fwui::IFWProgramControlUIPtr m_spPgmUI;
	symhtml::ISymHTMLDocument * m_pDoc;
	bool m_bInitialized;
};
