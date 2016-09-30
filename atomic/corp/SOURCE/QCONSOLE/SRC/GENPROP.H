/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/genprop.h_v   1.2   08 Jun 1998 14:54:54   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// GenProp.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/genprop.h_v  $
// 
//    Rev 1.2   08 Jun 1998 14:54:54   DBuches
// Added extra code to classify compressed items.
// 
//    Rev 1.1   06 Mar 1998 11:20:24   DBuches
// Updated to use IQuarantineItem pointer to update data.
// 
//    Rev 1.0   27 Feb 1998 15:10:24   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_GENPROP_H__3E560267_A6FB_11D1_A521_0000C06F46D0__INCLUDED_)
#define AFX_GENPROP_H__3E560267_A6FB_11D1_A521_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "proppage.h"
#include "imagebutton.h"

/////////////////////////////////////////////////////////////////////////////
// CGenProp dialog

class CGenProp : public CItemPage
{
	DECLARE_DYNCREATE(CGenProp)

// Construction
public:
	BOOL FileIsCompressed( LPCTSTR szFileName );
	CGenProp();
	~CGenProp();

// Dialog Data
	//{{AFX_DATA(CGenProp)
	enum { IDD = IDD_GENERAL_PAGE };
	CImageButton	m_ImageButton;
	CString	m_sItemName;
	CString	m_sTypeName;
	CString	m_sFileSize;
	CString	m_sLocation;
	CString	m_sQuarantineDate;
	CString	m_sSubDate;
	CString	m_sStatus;
    CString m_sStatusInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGenProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGenProp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENPROP_H__3E560267_A6FB_11D1_A521_0000C06F46D0__INCLUDED_)
