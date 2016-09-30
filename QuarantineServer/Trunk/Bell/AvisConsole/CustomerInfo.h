/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_CUSTOMERINFO_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_)
#define AFX_CUSTOMERINFO_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomerInfo.h : header file
//

////#include "CommonPropPage.h"
#include "ServerConfigData.h"



/////////////////////////////////////////////////////////////////////////////
// CCustomerInfo dialog

class CCustomerInfo : public CPropertyPage
{
	DECLARE_DYNCREATE(CCustomerInfo)

// Construction
public:
	CCustomerInfo();
	~CCustomerInfo();

// Dialog Data
	//{{AFX_DATA(CCustomerInfo)
	enum { IDD = IDD_AVIS_CUSTOMER_PAGE };
	CString	m_szCompanyName;
	CString	m_szContactEmail;
	CString	m_szContactName;
	CString	m_szContactPhone;
	CString	m_szCustomerAccount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCustomerInfo)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCustomerInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditCompanyName();
	afx_msg void OnChangeEditContactName();
	afx_msg void OnChangeEditContactPhone();
	afx_msg void OnChangeEditContactEmail();
	afx_msg void OnChangeEditCustAcct();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


// Implementation
public:
    DWORD Release() 
    { 
        if( --m_dwRef == 0 )
        {
            delete this;
            return 0;
        }
        return m_dwRef;
    }

    BOOL SaveChangedData();
    BOOL ValidateSMTPaddress(); 



    //HRESULT Initialize( void* pSConfigData = NULL, DWORD iExtDataObjectID = 0 )
    HRESULT Initialize( CServerConfigData* pSConfigData = NULL, DWORD iExtDataObjectID = 0 )
    {
        m_iExtDataObjectID = iExtDataObjectID;  // HAS THE OBJECTID from caller
    	m_pSConfigData = pSConfigData;
        if( pSConfigData != NULL )
            m_sServer = pSConfigData->GetServerName();
    	fWidePrintString("CAvisServerConfig Constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
        return(S_OK);
    }


// DATA
public:
    int                  m_iExtDataObjectID;
    LPVOID               m_pSnapin;
    LPSTREAM             m_pServerStream;

    // CServerConfigData  Object
    CServerConfigData*   m_pSConfigData;

    AVIS_HELP_ID_STRUCT* m_lpHelpIdArray;


public:
    DWORD m_dwRef;

    // Server we are attatched to.
    CString              m_sServer;


private:
//    DWORD       m_dwIddPageID;

};





//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_CUSTOMERINFO_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_)
