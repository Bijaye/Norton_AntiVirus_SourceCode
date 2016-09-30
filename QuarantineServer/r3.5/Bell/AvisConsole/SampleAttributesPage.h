/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_PLATFORMATTRIBUTESPAGE_H__F1B31F3C_E3AC_11D2_9790_00C04F688464__INCLUDED_)
#define AFX_PLATFORMATTRIBUTESPAGE_H__F1B31F3C_E3AC_11D2_9790_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SampleAttributesPage.h : header file
//

#include <afxtempl.h>     // for CList
#include <afxdlgs.h>	//jhill
#include "mmc.h"
#include "myListCtrl.h"	//jhill


// FORWARD DECLARATIONS
//class CSampleAttributesPage;
class CSampleAttributesPage; 
class CSampleActionsPage;
class CSampleErrorPage;   

#include "SampleAttributesPage.h"
#include "SampleActionsPage.h"
#include "SampleErrorPage.h"
#include "IconButton.h"





/////////////////////////////////////////////////////////////////////////////
// CSampleAttributesPage dialog

class CSampleAttributesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSampleAttributesPage)

// CONSTRUCTION
public:
	CSampleAttributesPage();
	~CSampleAttributesPage();



// Dialog Data
	//{{AFX_DATA(CSampleAttributesPage)
	//enum { IDD = IDD_SAMPLE_PLATFORM };
	enum { IDD = IDD_SAMPLE_ATTRIBUTES };
	CIconButton	m_IconButton;
	CMyListCtrl	m_ListCtrl;
	CString	m_sFilename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSampleAttributesPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL


// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSampleAttributesPage)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
    HRESULT Initialize( CAttributeAccess* pAttributeAccess, IDataObject* pDataObject, DWORD iExtDataObjectID = 0 )
    {
        m_pAttributeAccess = pAttributeAccess;
        m_iExtDataObjectID = iExtDataObjectID;  // HAS THE OBJECTID from caller
        m_pDataObject = pDataObject;
    	//m_pQServerGetAllAttributes = pQServerGetAllAttributes;
    	fWidePrintString("CSampleAttributesPage constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
        //GetQServerAttributeData();
        return(S_OK);
    }
    // DID THIS PAGE CALL m_pAttributeAccess->Initialize()
    BOOL IsPageRegistered() { return( m_dwRegistered ); }

    BOOL HookPropPageCallback();
    DWORD Release(); 
    BOOL VerifyCorrectPage(CSampleAttributesPage*  pSampleAttributesPage);
    //HRESULT GetQServerAttributeData();
    BOOL FillListWithData();
    BOOL AddWildcardHeadersAndValuesToListCtrl(LPCTSTR lpszWildcard);


//  DATA
public:
    LPVOID                m_pSnapin;
    long                  m_lParam;
    AVIS_HELP_ID_STRUCT*  m_lpHelpIdArray;
    int                   m_iExtDataObjectID;
	IDataObject*          m_pDataObject;
    void*                 m_pQServerGetAllAttributes;
    CAttributeAccess*     m_pAttributeAccess;

//    CPlatformAttributesPage*   m_pPlatformAttributesPage;
    CSampleAttributesPage*     m_pSampleAttributesPage;
    CSampleActionsPage*        m_pSampleActionsPage;
    CSampleErrorPage*          m_pSampleErrorPage;


private:
    DWORD                      m_dwRef;
    LONG                       m_dwRegistered;


};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLATFORMATTRIBUTESPAGE_H__F1B31F3C_E3AC_11D2_9790_00C04F688464__INCLUDED_)
