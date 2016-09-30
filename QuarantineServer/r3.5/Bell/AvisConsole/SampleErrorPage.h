#if !defined(AFX_SAMPLEERRORPAGE_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_)
#define AFX_SAMPLEERRORPAGE_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SampleErrorPage.h : header file
//

#include <afxdlgs.h>	//jhill
#include <Afxcmn.h>
#include "AttributeAccess.h"
#include "IconButton.h"



/////////////////////////////////////////////////////////////////////////////
// CSampleErrorPage dialog

class CSampleErrorPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSampleErrorPage)

// Construction
public:
	CSampleErrorPage();
	~CSampleErrorPage();

// Dialog Data
	//{{AFX_DATA(CSampleErrorPage)
	enum { IDD = IDD_SAMPLE_ERROR };
	CIconButton	m_IconButton;
	CString	m_sSampleErrorString;
	CString	m_sFilename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSampleErrorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSampleErrorPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


// Implementation
public:
    LONG Release(); 
    BOOL FillAttributeData(); 
    // DID THIS PAGE CALL m_pAttributeAccess->Initialize()
    BOOL IsPageRegistered() { return( m_dwRegistered ); }

    HRESULT Initialize( CAttributeAccess* pAttributeAccess, IDataObject* pDataObject, DWORD iExtDataObjectID = 0 )
    {
        m_pAttributeAccess = pAttributeAccess;
        m_iExtDataObjectID = iExtDataObjectID;  // HAS THE OBJECTID from caller
        m_pDataObject = pDataObject;
        return(S_OK);
    }

    BOOL ParseTokensToStrings(void);



// DATA
public:
    LPVOID                m_pSnapin;
    int                   m_iExtDataObjectID;
	IDataObject*          m_pDataObject;
    void*                 m_pQServerGetAllAttributes;
    CAttributeAccess*     m_pAttributeAccess;

    // "Error Message Tokens"
    CString               m_sSampleErrorTokens;



    AVIS_HELP_ID_STRUCT* m_lpHelpIdArray;    


public:
    DWORD                 m_dwRef;                          
    LONG                  m_dwRegistered;


};





//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_SAMPLEERRORPAGE_H__941A50E5_F75D_11D2_9795_00C04F688464__INCLUDED_)
