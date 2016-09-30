// NoCtxtPS.h : header file
//

#ifndef NO_CONTEXT_PROPERTY_SHEET_HEADER
#define NO_CONTEXT_PROPERTY_SHEET_HEADER


/////////////////////////////////////////////////////////////////////////////
// CNoContextPropertySheet

class CNoContextPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CNoContextPropertySheet)

// Construction
public:
	CNoContextPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CNoContextPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoContextPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNoContextPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNoContextPropertySheet)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif  // NO_CONTEXT_PROPERTY_SHEET_HEADER
