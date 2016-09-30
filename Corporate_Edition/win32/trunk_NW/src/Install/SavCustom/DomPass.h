// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// CDomPass dialog
#ifndef DOM_PASSWORD
#define DOM_PASSWORD

class CDomPass : public CDialog
{
// Construction
public:
	CDomPass(CString sServer, CString *sPassword = NULL, CWnd* pParent = NULL);
	CDomPass(CString sServer, CString sPassword, BOOL bServer = TRUE, CWnd* pParent = NULL);

	CString m_Server;
	CString m_Domain;
	CString m_Password;
	DWORD m_Error;

	void SetToDefault();
	void Decript();
	BOOL Valid();
	void DisplayError();

// Dialog Data
	//{{AFX_DATA(CDomPass)
	enum { IDD = IDD_DOMPASSWD_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDomPass)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL VerifyPassword(CString);

	// Generated message map functions
	//{{AFX_MSG(CDomPass)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
CStringArray* InitPasswords();
void CleanupPasswords();
void AddPasswordToList(CString pPassword);
BOOL PasswordInList(CString pPassword);

#endif