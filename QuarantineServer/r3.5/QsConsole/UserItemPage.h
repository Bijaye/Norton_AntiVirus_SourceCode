/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_USERITEMPAGE_H__BFCF87FA_C10E_11D2_8F46_3078302C2030__INCLUDED_)
#define AFX_USERITEMPAGE_H__BFCF87FA_C10E_11D2_8F46_3078302C2030__INCLUDED_

#include "itemdata.h"
#include "iconbutton.h"
#include "QPropPage.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserItemPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserItemPage dialog

class CUserItemPage : public CQPropPage
{
	DECLARE_DYNCREATE(CUserItemPage)

// Construction
public:
	HRESULT Initialize( CItemData* pItemData );
	CUserItemPage();
	~CUserItemPage();

// Dialog Data
	//{{AFX_DATA(CUserItemPage)
	enum { IDD = IDD_CLIENT_PAGE };
	CIconButton	m_iconBtn;
	CString	m_sComputer;
	CString	m_sDomain;
	CString	m_sFileName;
	CString	m_sUserName;
	CString	m_sIPAddress;
	CString	m_sNACAddress;
	CString	m_sPlatform;
	CString	m_sProduct;
	//}}AFX_DATA

    DWORD Release() 
        { 
        if( --m_dwRef == 0 )
            {
            delete this;
            return 0;
            }
        return m_dwRef;
        }

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUserItemPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUserItemPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    void GetAddressFields( LPCTSTR szAddress );

    DWORD m_dwRef;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERITEMPAGE_H__BFCF87FA_C10E_11D2_8F46_3078302C2030__INCLUDED_)
