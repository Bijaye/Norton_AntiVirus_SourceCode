#if !defined(AFX_ASERVERFIREWALLCONFIG2_H__73488199_DE5B_11D2_978C_00C04F688464__INCLUDED_)
#define AFX_ASERVERFIREWALLCONFIG2_H__73488199_DE5B_11D2_978C_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AServerFirewallConfig2.h : header file
//
#include <afxdlgs.h>
#include <afxcmn.h>	//jim

#include "ServerConfigData.h"




/////////////////////////////////////////////////////////////////////////////
// CAvisServerConfig2 dialog

class CAServerFirewallConfig2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CAServerFirewallConfig2)

// Construction
public:
	CAServerFirewallConfig2();
	~CAServerFirewallConfig2();

    //DWORD CheckAvis2DataDefaults();
    
    void SaveChangedData(); 


// Dialog Data
	//{{AFX_DATA(CAServerFirewallConfig2)
	enum { IDD = IDD_AVIS_FIREWALL_COMM_PAGE };
	CSpinButtonCtrl	m_SpinFirewallPort;
	CString	m_szProxyFirewall;
	long	m_iFirewallPort;
	CString	m_sFirewallUsername;
	CString	m_sFirewallPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAServerFirewallConfig2)
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
	//{{AFX_MSG(CAServerFirewallConfig2)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditProxyFirewall();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChangeEditFirewallPort();
	afx_msg void OnChangeEditFirewallPassword();
	afx_msg void OnChangeEditFirewallUsername();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



public:
    //DWORD Release(); 
    DWORD Release() 
    { 
        if( --m_dwRef == 0 )
        {
            delete this;
            return 0;
        }
        return m_dwRef;
    }

    //BOOL HookPropPageCallback();
    //BOOL VerifyCorrectPage(CAServerFirewallConfig2* pAServerFirewallConfig2);

    //HRESULT Initialize( void* pSConfigData = NULL, DWORD iExtDataObjectID = 0 )
    HRESULT Initialize( CServerConfigData* pSConfigData = NULL, DWORD iExtDataObjectID = 0 )
    {
        m_iExtDataObjectID = iExtDataObjectID;  // HAS THE OBJECTID from caller
    	m_pSConfigData = pSConfigData;
        if( pSConfigData != NULL )
            m_sServer = pSConfigData->GetServerName();
    	fWidePrintString("CAServerFirewallConfig2 Constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
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

    //CAvisServerConfig*           m_pAvisServerConfig;
    //CAvisServerDefPolicyConfig*  m_pAvisServerDefPolicyConfig;
    //CAvisServerConfig2*          m_pAvisServerConfig2;
    //CCustomerInfo*               m_pAvisCustomerInfo;


    AVIS_HELP_ID_STRUCT* m_lpHelpIdArray;

    int        m_bGatewayAddressModified;
    int        m_bFirewallAddressModified;

private:
    DWORD         m_dwRef;
    // Server we are attatched to.
    CString       m_sServer;

//  IAvisConfig*  m_pConfig;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASERVERFIREWALLCONFIG2_H__73488199_DE5B_11D2_978C_00C04F688464__INCLUDED_)
