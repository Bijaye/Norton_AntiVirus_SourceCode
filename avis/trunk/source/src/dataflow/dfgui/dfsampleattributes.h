#if !defined(AFX_DFSAMPLEATTRIBUTES_H__4FD66113_8C9C_11D3_A568_0004ACECC1E1__INCLUDED_)
#define AFX_DFSAMPLEATTRIBUTES_H__4FD66113_8C9C_11D3_A568_0004ACECC1E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFSampleAttributes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DFSampleAttributes dialog

class DFSampleAttributes : public CDialog
{
// Construction
public:
	DFSampleAttributes(CWnd* pParent = NULL);   // standard constructor
	BOOL SetFileText(char *fileName, CString id);

		// Dialog Data
	//{{AFX_DATA(DFSampleAttributes)
	enum { IDD = IDD_SAMPLEATTRIBUTES };
	CString	m_strAttrText;
	CString	m_strCookieId;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFSampleAttributes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DFSampleAttributes)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFSAMPLEATTRIBUTES_H__4FD66113_8C9C_11D3_A568_0004ACECC1E1__INCLUDED_)
