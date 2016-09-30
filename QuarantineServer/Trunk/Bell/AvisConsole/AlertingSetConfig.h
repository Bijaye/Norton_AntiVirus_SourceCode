#if !defined(AFX_ALERTINGSETCONFIG_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_)
#define AFX_ALERTINGSETCONFIG_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlertingSetConfig.h : header file
//

//#include "CommonPropPage.h"
#include "ServerConfigData.h"
#include "ACStateListCtrl.h"



/////////////////////////////////////////////////////////////////////////////
// CAlertingSetConfig dialog

class CAlertingSetConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CAlertingSetConfig)

// Construction
public:
	CAlertingSetConfig();
	~CAlertingSetConfig();

// Dialog Data
	//{{AFX_DATA(CAlertingSetConfig)
	enum { IDD = IDD_ALERTING_SET_CONFIG1 };
	CSpinButtonCtrl	m_SpinAlertInterval;
	CACStateListCtrl m_EventList;
	CButton	m_TestButton1;
	BOOL	m_bCheckEnableAlerts;
	CString	m_sAmsServerAddress;
	long	m_iAlertCheckInterval;
	BOOL	m_bCheckNtEventLog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAlertingSetConfig)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAlertingSetConfig)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnTestButton1();
	afx_msg void OnCheckEnableAlerts();
	afx_msg void OnChangeEditAmsServerAddress();
	afx_msg void OnDestroy();
	afx_msg void OnChangeEditAlertCheckInterval();
	afx_msg void OnCheckNtEventLog();
	afx_msg void OnConfigButton();
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

    HRESULT Initialize( CServerConfigData* pSConfigData = NULL, DWORD iExtDataObjectID = 0 )
    {
        m_iExtDataObjectID = iExtDataObjectID;  // HAS THE OBJECTID from caller
    	m_pSConfigData = pSConfigData;
        if( pSConfigData != NULL )
            m_sServer = pSConfigData->GetServerName();
    	fWidePrintString("CAvisServerConfig Constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
        return(S_OK);
    }

    void SendTestEvent(); 
    void SaveChangedData();
    BOOL FreeDataList();
    BOOL SaveEventTableTableData();
	afx_msg LRESULT OnListDirty(WPARAM wParam, LPARAM lParam);


    // ParseEventData.cpp
    BOOL GetEventDataByIndex( DWORD dwEventRulesTableID, CEventObject *pEventObj, DWORD* pdwIndex );
    BOOL PutEventDataByIndex( CEventObject *pEventObj, DWORD dwIndex, DWORD* pdwChangedCount );



// DATA
public:
    int                  m_iExtDataObjectID;
    LPVOID               m_pSnapin;
    LPSTREAM             m_pServerStream;
    int                  m_iTotalListItems;

    // CServerConfigData  Object
    CServerConfigData*   m_pSConfigData;

	BOOL	             m_bSavedCheckEnableAlerts;
	CString	             m_sSavedAmsServerAddress;
	CString	             m_sSavedAlertCheckInterval;
    long                 m_iSavedAlertCheckInterval;
	BOOL	             m_bSavedCheckNtEventLog;



    AVIS_HELP_ID_STRUCT* m_lpHelpIdArray;    


public:
    DWORD m_dwRef;                          

    // Server we are attatched to.
    CString              m_sServer;

};





//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_ALERTINGSETCONFIG_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_)
