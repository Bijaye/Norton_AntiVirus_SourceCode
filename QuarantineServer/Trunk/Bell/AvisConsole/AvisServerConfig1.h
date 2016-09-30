/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_AVISSERVERCONFIG1_H__73488194_DE5B_11D2_978C_00C04F688464__INCLUDED_)
#define AFX_AVISSERVERCONFIG1_H__73488194_DE5B_11D2_978C_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AvisServerConfig1.h : header file
//

#include <afxdlgs.h>	//jim
#include <afxcmn.h>	//jim

#include "ServerConfigData.h"



/////////////////////////////////////////////////////////////////////////////
// CAvisServerConfig dialog								   

class CAvisServerConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CAvisServerConfig)

// Construction
public:
	CAvisServerConfig();
	~CAvisServerConfig();

    DWORD CheckAvis1DataDefaults();
    void  SaveChangedData();


	


// DIALOG DATA
	//{{AFX_DATA(CAvisServerConfig)
	enum { IDD = IDD_AVIS_POLICY_PAGE };
	CSpinButtonCtrl	m_SpinStatusInterval;
	CSpinButtonCtrl	m_SpinQueueCheckInterval;
	BOOL	m_bStripUserData;
	DWORD	m_dwQueueCheckInterval;
	int		m_iStatusInterval;
	BOOL	m_bAutoSubmit;
	//}}AFX_DATA


// OVERRIDES
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAvisServerConfig)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// IMPLEMENTATION
protected:
	// Generated message map functions
	//{{AFX_MSG(CAvisServerConfig)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditQueueCheckInterval();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChangeEditStatusInterval2();
	afx_msg void OnCheckAutoSubmit();
	afx_msg void OnCheckStripUserData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


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
    BOOL                 m_bCustEmailFilledIn;

    AVIS_HELP_ID_STRUCT* m_lpHelpIdArray;

private:
    DWORD m_dwRef;
    // Server we are attatched to.
    CString              m_sServer;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVISSERVERCONFIG1_H__73488194_DE5B_11D2_978C_00C04F688464__INCLUDED_)
