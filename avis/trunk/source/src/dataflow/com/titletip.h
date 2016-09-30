#if !defined(AFX_TITLETIP_H_INCLUDED)
#define AFX_TITLETIP_H_INCLUDED
// TitleTip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTitleTip window

#define TITLETIP_CLASSNAME _T("ZAvnadminTitleTip")
class CTitleTip : public CWnd
{
// Construction
public:
	void Show (CRect rectTitle, LPCTSTR lpszTitleText, int xoffset = 0);
	void Hide ();
	CTitleTip();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitleTip)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTitleTip();

	// Generated message map functions
protected:
	CRect m_rectTitle;
	CRect m_oldRectTitle;
	CWnd * m_pParentWnd;
	BOOL m_created;
	//{{AFX_MSG(CTitleTip)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_TITLETIP_H_INCLUDED)
