//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// PseudoHyperlink.h : Header for CPseudoHyperlink class
// Makes a static text control into a pseudo hyperlink.
// Either launches a web page with specified URL or calls a callback.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PseudoHyperlink_H__4F3D4B83_5A45_43a4_813A_DD4C75D39E3A__INCLUDED_)
#define AFX_PseudoHyperlink_H__4F3D4B83_5A45_43a4_813A_DD4C75D39E3A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// CPseudoHyperlink

class CPseudoHyperlink : public CStatic
{
	DECLARE_DYNAMIC(CPseudoHyperlink)

public:	
	CPseudoHyperlink();
	virtual ~CPseudoHyperlink();

	typedef BOOL (*fnPseudoHyperlinkCallback)(void* pData, void* pParam);

public:
    void SetLinkColors(COLORREF crLinkColor = -1, COLORREF crHoverColor = -1);
	COLORREF GetLinkColor() const;
	COLORREF GetLinkHoverColor() const;

    void SetLinkCursor(HCURSOR hCursor);
	HCURSOR GetLinkCursor() const;

	void SetUnderline(BOOL bUnderline = TRUE);
    BOOL GetUnderline() const;

    void SetAutoSize(bool bAutoSize = true);
    bool GetAutoSize() const;
	
	void SetLinkText(CString sLinkText);
	CString GetLinkText() const;

	void SetURL(CString sURL);
	CString GetURL() const;
	
	void SetLink(CString sDisplayText);
	void SetLink(CString sDisplayText, CString sURL);
	void SetLink(CString sDisplayText, fnPseudoHyperlinkCallback pfnCallBack, void *pUserData, void *pParam);
	
	// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPseudoHyperlink)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
private:
	void ResizeAndPositionWindow();
	void SetDefaultLinkCursor();
	void ShowFocus();
	void RedrawControlRect();

	void TurnOffLinkEffects();
	void StoreLinkData(fnPseudoHyperlinkCallback pfnCallBack, void *pUserData, void *pParam);
	void DoLinkAction();

// Protected attributes
private:
    COLORREF m_crLinkColor;  // Hyperlink Colors
    COLORREF m_crHoverColor; // Hover Color

    bool     m_bHoveringOverLink; // Is cursor over the control?
	HCURSOR  m_hLinkCursor;   // Cursor for hyperlink

    BOOL     m_bUnderline;    // Underline hyperlink?
	bool     m_bBold;		  // Bold font?
	CFont    m_Font;          // Underline font if necessary
    bool     m_bAutoSize;	  // Auto adjust window size to fit text?
	bool	 m_bFocusDrawn;
	bool	 m_bLinkEffects;

	CBrush   m_brBackground;

	CString  m_sLinkText;
    CString  m_sURL;          // URL to launch on click
    
	enum LinkType
	{
		LinkType_None = 0,
		LinkType_URL = 1,
		LinkType_Callback
	} m_LinkType;

	typedef struct
	{
		fnPseudoHyperlinkCallback m_pfnCallback;
		void *m_pData;
		void *m_pParam;
	} LinkData;

	LinkData *m_pLinkData;


// Generated message map functions
protected:
    //{{AFX_MSG(CPseudoHyperlink)
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    //}}AFX_MSG
    afx_msg void OnClicked();
	DECLARE_MESSAGE_MAP()	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PseudoHyperlink_H__4F3D4B83_5A45_43a4_813A_DD4C75D39E3A__INCLUDED_)
