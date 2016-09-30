/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_SAMPLEACTIONSPAGE_H__F1B31F3E_E3AC_11D2_9790_00C04F688464__INCLUDED_)
#define AFX_SAMPLEACTIONSPAGE_H__F1B31F3E_E3AC_11D2_9790_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SampleActionsPage.h : header file
//

#include <afxdlgs.h>	//jhill
#include <Afxcmn.h>
#include "AttributeAccess.h"
#include "IconButton.h"


// Define
typedef struct tagSAMPLEACTIONS
{
	DWORD	dwSubmissionPriority;
	BOOL	m_bAutoDelivery;

}   SAMPLE_ACTIONS_CONFIG;


/////////////////////////////////////////////////////////////////////////////
// CSampleActionsPage dialog

class CSampleActionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSampleActionsPage)

// Construction
public:
	CSampleActionsPage();
	~CSampleActionsPage();
	BOOL	m_bAutoDelivery;


// Dialog Data
	//{{AFX_DATA(CSampleActionsPage)
	enum { IDD = IDD_SAMPLE_ACTIONS };
	CIconButton	m_IconButton;
	CSpinButtonCtrl	m_SpinSubmissionPriority;
	CString	m_szAnalysisState;
	CString	m_szDefSequenceNeeded;
	CString	m_VirusName;
	DWORD	m_dwSubmissionPriority;
	CString	m_sFilename;
	CString	m_sSampleStatus;
	CString	m_sTrackingNumber;
	CString	m_sStatusEx;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSampleActionsPage)
	public:
	virtual void OnOK();
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
	//{{AFX_MSG(CSampleActionsPage)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCheckSampleAutoDelivery();
	afx_msg void OnChangeSampleSubmissionPriority();
	afx_msg void OnActionsSubmitButton();
	afx_msg void OnActionsDeliverButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
    void SaveChangedData();
    BOOL FillAttributeData(); 
    LONG Release(); 
    // DID THIS PAGE CALL m_pAttributeAccess->Initialize()
    BOOL IsPageRegistered() { return( m_dwRegistered ); }

    HRESULT Initialize( CAttributeAccess* pAttributeAccess, IDataObject* pDataObject, DWORD iExtDataObjectID = 0 )
    {
        m_pAttributeAccess = pAttributeAccess;
        m_iExtDataObjectID = iExtDataObjectID;  // HAS THE OBJECTID from caller
        m_pDataObject = pDataObject;
        //fWidePrintString("CSampleActionsPage constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
        return(S_OK);
    }

    BOOL ParseTokensToStrings();
    BOOL SetExtendedStatusText(); 




//  DATA
public:
    LPVOID                m_pSnapin;
    AVIS_HELP_ID_STRUCT*  m_lpHelpIdArray;
    int                   m_iExtDataObjectID;
    void*                 m_pQServerGetAllAttributes;
	IDataObject*          m_pDataObject;
    CAttributeAccess*     m_pAttributeAccess;
    DWORD                 m_dwStatus;
    BOOL                  m_bIsXdateSubmitted;
    BOOL                  m_bOkToReSubmit;
    DWORD                 m_dwSubmissionCount;
    DWORD                 m_dwActionToTake;

private:
    LONG                  m_dwRef;
    LONG                  m_dwRegistered;
    DWORD                 m_dwSignaturesPriority;
    SAMPLE_ACTIONS_CONFIG  SampleActionsConfig;
    //DWORD dwSampleActionsDefaultsSet;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEACTIONSPAGE_H__F1B31F3E_E3AC_11D2_9790_00C04F688464__INCLUDED_)
