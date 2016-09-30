/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/ServerProp.h_v   1.1   06 Mar 1998 11:20:26   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ServerProp.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/ServerProp.h_v  $
// 
//    Rev 1.1   06 Mar 1998 11:20:26   DBuches
// Updated to use IQuarantineItem pointer to update data.
// 
//    Rev 1.0   27 Feb 1998 15:10:28   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERPROP_H__61B899C4_AEFA_11D1_A53D_0000C06F46D0__INCLUDED_)
#define AFX_SERVERPROP_H__61B899C4_AEFA_11D1_A53D_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "proppage.h"
#include "Imagebutton.h"

/////////////////////////////////////////////////////////////////////////////
// CServerProp dialog

class CServerProp : public CItemPage
{
	DECLARE_DYNCREATE(CServerProp)

// Construction
public:
	CServerProp();
	~CServerProp();

// Dialog Data
	//{{AFX_DATA(CServerProp)
	enum { IDD = IDD_SERVER_PAGE };
	CImageButton	m_ImageButton;
	CString	m_sItemName;
	CString	m_sUserName;
	CString	m_sDomainName;
	CString	m_sComputerName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CServerProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CServerProp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERPROP_H__61B899C4_AEFA_11D1_A53D_0000C06F46D0__INCLUDED_)
