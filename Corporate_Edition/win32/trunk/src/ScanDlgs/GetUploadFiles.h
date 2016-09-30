// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_GETUPLOADFILES_H__0E98874F_B1F4_11D1_AB27_00A0C90F8F6F__INCLUDED_)
#define AFX_GETUPLOADFILES_H__0E98874F_B1F4_11D1_AB27_00A0C90F8F6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GetUploadFiles.h : header file
//


#include "sendtointel.h"
#include "Resource.h"

class CDebugUpload : public IMessages
{
public:
	virtual ULONG   __stdcall AddRef() 	{return InterlockedIncrement(&m_cRef);}
	virtual ULONG   __stdcall Release() {if (InterlockedDecrement(&m_cRef) == 0) {delete this ;return 0 ;}return m_cRef ;}
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv) {*ppv = NULL;
		if ((iid != IID_IUnknown) && (iid != IID_IMessages))	return E_NOINTERFACE;
		*ppv = this;AddRef();return S_OK ;
		};
	VI(ULONG) Send(char *o) {return 0;};
	VI(ULONG) BytesTransferred(DWORD Bytes) { return 0;}
	CDebugUpload(class CGetUploadFiles *status) {Status = status;return;};
	long m_cRef;
	class CGetUploadFiles *Status;
};



/////////////////////////////////////////////////////////////////////////////
// CGetUploadFiles dialog

class CGetUploadFiles : public CDialog
{
// Construction
public:
	CGetUploadFiles(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetUploadFiles)
	enum { IDD = IDD_GETUPLOADFILES };
	CButton	m_OK;
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetUploadFiles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetUploadFiles)
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETUPLOADFILES_H__0E98874F_B1F4_11D1_AB27_00A0C90F8F6F__INCLUDED_)
