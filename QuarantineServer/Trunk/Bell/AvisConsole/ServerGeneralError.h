#if !defined(AFX_SERVERGENERALERROR_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_)
#define AFX_SERVERGENERALERROR_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerGeneralError.h : header file
//

//#include "CommonPropPage.h"
#include "ServerConfigData.h"



/////////////////////////////////////////////////////////////////////////////
// CServerGeneralError dialog

class CServerGeneralError : public CPropertyPage
{
	DECLARE_DYNCREATE(CServerGeneralError)

// Construction
public:
	CServerGeneralError();
	~CServerGeneralError();

// Dialog Data
	//{{AFX_DATA(CServerGeneralError)
	enum { IDD = IDD_GENERAL_ERROR };
	CString	m_sGeneralErrorString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CServerGeneralError)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CServerGeneralError)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
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

    BOOL ParseTokensToStrings(void);



// DATA
public:
    int                  m_iExtDataObjectID;
    LPVOID               m_pSnapin;
    LPSTREAM             m_pServerStream;

    // CServerConfigData  Object
    CServerConfigData*   m_pSConfigData;

    // "Attention Message Tokens"
    CString              m_sGeneralAttentionTokens;
    CString              m_sLastAlertString;
    CString              m_sLastAlertDate;



    AVIS_HELP_ID_STRUCT* m_lpHelpIdArray;    // moved to CCommonPropPage


public:
    DWORD m_dwRef;                          // moved to CCommonPropPage

    // Server we are attatched to.
    CString              m_sServer;

};





//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_SERVERGENERALERROR_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_)
