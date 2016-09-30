/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_INSTALLDEFINITIONS_H__3F28CF0E_B29D_11D3_97A9_00C04F688464__INCLUDED_)
#define AFX_INSTALLDEFINITIONS_H__3F28CF0E_B29D_11D3_97A9_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InstallDefinitions.h : header file
//


//#include <afxdlgs.h>	//jim
//#include <afxcmn.h>	//jim


////#include "CommonPropPage.h"
#include "ServerConfigData.h"





/////////////////////////////////////////////////////////////////////////////
// CInstallDefinitions dialog

class CInstallDefinitions : public CPropertyPage      // CPropertyPage
{
	DECLARE_DYNCREATE(CInstallDefinitions)

// Construction
public:
	CInstallDefinitions();
	~CInstallDefinitions();



// Dialog Data
	//{{AFX_DATA(CInstallDefinitions)
	enum { IDD = IDD_INSTALL_DEFS_PAGE };
	CSpinButtonCtrl	m_SpinDeliveryInterval;
	BOOL	m_bCheckBlessedBroadcast;
	BOOL	m_bCheckUnblessedBroadcast;
	BOOL	m_bCheckUnblessedNarrowCast;
	BOOL	m_bCheckUnblessedPointCast;
	int		m_iDefDeliveryInterval;
    CString    m_sDefSecureUsername;
    CString    m_sDefSecurePassword;
    CString    m_sDefSecureConfirm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInstallDefinitions)
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
	//{{AFX_MSG(CInstallDefinitions)
	afx_msg void OnCheckBlessedBroadcast();
	afx_msg void OnCheckUnblessedBroadcast();
	afx_msg void OnCheckUnblessedNarrowcast();
	afx_msg void OnCheckUnblessedPointcast();
	afx_msg void OnChangeEditDefDeliveryInterval();
	afx_msg void OnChangeEditDefUsername();
	afx_msg void OnChangeEditDefPassword();
	afx_msg void OnChangeEditDefConfirm();
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnButtonBlesedSelectTargets();
	afx_msg void OnButtonUnblesedSelectTargets();
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

    HRESULT Initialize( CServerConfigData* pSConfigData = NULL, DWORD iExtDataObjectID = 0 )
    {
        m_iExtDataObjectID = iExtDataObjectID;  // HAS THE OBJECTID from caller
    	m_pSConfigData = pSConfigData;
        if( pSConfigData != NULL )
            m_sServer = pSConfigData->GetServerName();
    	fWidePrintString("CInstallDefinitions Constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
        return(S_OK);
    }


    void  SaveChangedData();



public:
    int                  m_iExtDataObjectID;
    LPVOID               m_pSnapin;
//    LPSTREAM             m_pServerStream;

    // CServerConfigData  Object
    CServerConfigData*   m_pSConfigData;
    CString	             m_sDefBlessedTargets;
    CString	             m_sDefUnblessedTargets;
    CString	             m_sLastDefBlessedTargets;
    CString	             m_sLastDefUnblessedTargets;
    CString	             m_sLastDefSecureUsername;
    CString	             m_sLastDefSecurePassword;
	CString				 m_sCurrentDefDate;
	CString				 m_sCurrentDefVersion;



    AVIS_HELP_ID_STRUCT* m_lpHelpIdArray;  



private:
    DWORD       m_dwRef;                      
    // Server we are attatched to.
    CString              m_sServer;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTALLDEFINITIONS_H__3F28CF0E_B29D_11D3_97A9_00C04F688464__INCLUDED_)
