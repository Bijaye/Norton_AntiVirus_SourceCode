/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_GENERALITEMPAGE_H__21B90A05_C046_11D2_8F46_3078302C2030__INCLUDED_)
#define AFX_GENERALITEMPAGE_H__21B90A05_C046_11D2_8F46_3078302C2030__INCLUDED_

#include <afxdlgs.h>
#include "itemdata.h"
#include "iconbutton.h"
#include "QPropPage.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GeneralItemPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGeneralItemPage dialog

class CGeneralItemPage : public CQPropPage
{
	DECLARE_DYNCREATE(CGeneralItemPage)

// Construction
public:
	CGeneralItemPage();
	~CGeneralItemPage();

    HRESULT Initialize( CItemData* pItemData );

// Dialog Data
	//{{AFX_DATA(CGeneralItemPage)
	enum { IDD = IDD_GENERAL_PAGE };
	CIconButton	m_iconBtn;
	CString	m_sFileName;
	CString	m_sFilePath;
	CString	m_sType;
	CString	m_sSubmitDate;
	CString	m_sQuarantineDate;
	CString	m_sFileSize;
	CString	m_sStatusText;
	CString	m_sInformation;
	CString	m_sSubmittedBy;
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
	//{{AFX_VIRTUAL(CGeneralItemPage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

public:
    LONG        m_lNotifyHandle;
    LPARAM      m_lParam;
    DWORD       m_dwQserverVersion;
   
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGeneralItemPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    HICON   m_hIcon;
    DWORD   m_dwRef;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALITEMPAGE_H__21B90A05_C046_11D2_8F46_3078302C2030__INCLUDED_)
