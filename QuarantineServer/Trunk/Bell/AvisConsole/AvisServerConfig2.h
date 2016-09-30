/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_AVISSERVERCONFIG2_H__73488199_DE5B_11D2_978C_00C04F688464__INCLUDED_)
#define AFX_AVISSERVERCONFIG2_H__73488199_DE5B_11D2_978C_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AvisServerConfig2.h : header file
//
#include <afxdlgs.h>
#include <afxcmn.h>	//jim

#include "ServerConfigData.h"
#include "AvisServerConfig1.h"
#include "AServerDefPolicyConfig.h"
#include "CustomerInfo.h"
#include "AServerFirewallConfig2.h"
#include "ServerGeneralError.h"
#include "InstallDefinitions.h"
#include "AlertingSetConfig.h"




/////////////////////////////////////////////////////////////////////////////
// CAvisServerConfig2 dialog

class CAvisServerConfig2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CAvisServerConfig2)

// Construction
public:
	CAvisServerConfig2();
	~CAvisServerConfig2();

    //DWORD CheckAvis2DataDefaults();
    
    void SaveChangedData(); 


// Dialog Data
	//{{AFX_DATA(CAvisServerConfig2)
	enum { IDD = IDD_AVIS_WEB_COMM_PAGE };
	CComboBox	m_GatewayCombo;
	BOOL	m_bCheckSecureDefDownload;
	BOOL	m_bCheckSecureSampleSubmission;
	CString	m_sGatewayAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAvisServerConfig2)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAvisServerConfig2)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCheckSecureDefDownload();
	afx_msg void OnCheckSecureSampleSubmission();
	afx_msg void OnEditchangeComboGatewayAddress();
	afx_msg void OnSelchangeComboGatewayAddress();
	afx_msg void OnEditupdateComboGatewayAddress();
	afx_msg void OnSelendokComboGatewayAddress();
	afx_msg void OnKillfocusComboGatewayAddress();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



public:
    DWORD Release(); 
    BOOL HookPropPageCallback();
    BOOL VerifyCorrectPage(CAvisServerConfig2* pAvisServerConfig2);

    //HRESULT Initialize( void* pSConfigData = NULL, DWORD iExtDataObjectID = 0 )
    HRESULT Initialize( CServerConfigData* pSConfigData = NULL, DWORD iExtDataObjectID = 0 )
    {
        m_iExtDataObjectID = iExtDataObjectID;  // HAS THE OBJECTID from caller
    	m_pSConfigData = pSConfigData;
        if( pSConfigData != NULL )
            m_sServer = pSConfigData->GetServerName();
    	fWidePrintString("CAvisServerConfig2 Constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
        return(S_OK);
    }


// DATA
public:
    int        m_iExtDataObjectID;
    LPVOID     m_pSnapin;
    long       m_lParam;
    LPSTREAM   m_pServerStream;

    // CServerConfigData  Object
    CServerConfigData*           m_pSConfigData;

    CAvisServerConfig2*          m_pAvisServerConfig2;
    CAServerFirewallConfig2*     m_pAServerFirewallConfig2;
    CAvisServerConfig*           m_pAvisServerConfig;
    CAvisServerDefPolicyConfig*  m_pAvisServerDefPolicyConfig;
    CInstallDefinitions*         m_pInstallDefinitions;
    CCustomerInfo*               m_pAvisCustomerInfo;
    CServerGeneralError*         m_pServerGeneralError;
    CAlertingSetConfig*          m_pAlertingSetConfig;


    AVIS_HELP_ID_STRUCT* m_lpHelpIdArray;

    int        m_bGatewayAddressModified;
    int        m_bFirewallAddressModified;

private:
    DWORD         m_dwRef;
    // Server we are attatched to.
    CString       m_sServer;

	// History list vars.
#define MAX_NO_HISTORY_ITEMS	20
	CStringList  m_GatewayAddressList;
	BOOL		m_bGatewayListModifed;

	// History List APIs  (tm)	
	HRESULT SetGatewayHistory(void);
	HRESULT UpdateGatewayHistoryList(LPCTSTR psGateWayAddress);
	HRESULT ParseHistoryList(LPTSTR szGateHistory);
	HRESULT SaveHistoryList(void);
	
//  IAvisConfig*  m_pConfig;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVISSERVERCONFIG2_H__73488199_DE5B_11D2_978C_00C04F688464__INCLUDED_)
