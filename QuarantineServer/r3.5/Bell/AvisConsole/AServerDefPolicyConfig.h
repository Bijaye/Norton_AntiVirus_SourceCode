#if !defined(AFX_ASERVERDEFPOLICYCONFIG_H__73488194_DE5B_11D2_978C_00C04F688465__INCLUDED_)
#define AFX_ASERVERDEFPOLICYCONFIG_H__73488194_DE5B_11D2_978C_00C04F688465__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AServerDefPolicyConfig.h : header file
//

#include <afxdlgs.h>	//jim
#include <afxcmn.h>	//jim

#include "ServerConfigData.h"



/////////////////////////////////////////////////////////////////////////////
// CAvisServerDefPolicyConfig dialog								   

class CAvisServerDefPolicyConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CAvisServerDefPolicyConfig)

// Construction
public:
	CAvisServerDefPolicyConfig();
	~CAvisServerDefPolicyConfig();

    DWORD CheckAvis1DataDefaults();
    void  SaveChangedData();


	


// DIALOG DATA
	//{{AFX_DATA(CAvisServerDefPolicyConfig)
	enum { IDD = IDD_AVIS_DEFINITION_POLICY_PAGE };
	CSpinButtonCtrl	m_SpinDefsBlessedInterval;
	int		m_iDefsBlessedInterval;
	CString	m_sActiveDefsSeqNum;
	//}}AFX_DATA


// OVERRIDES
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAvisServerDefPolicyConfig)
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
	//{{AFX_MSG(CAvisServerDefPolicyConfig)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChangeEditDefsBlessedInterval();
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
    	fWidePrintString("CAvisServerDefPolicyConfig Constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
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

private:
    DWORD m_dwRef;
    // Server we are attatched to.
    CString              m_sServer;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASERVERDEFPOLICYCONFIG_H__73488194_DE5B_11D2_978C_00C04F688465__INCLUDED_)
