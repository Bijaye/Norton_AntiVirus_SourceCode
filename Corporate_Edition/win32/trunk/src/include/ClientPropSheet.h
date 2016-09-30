// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ClientPropSheet.h
//  Purpose: Base classes for Schedule Sheet & Page
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined (SCHEDULE_PAGE_INCLUDED )
#define SCHEDULE_PAGE_INCLUDED

#include "LDVPPropSheet.h"
#include "Clientreg.h"
#include "Taskbase.h"
#include "Resource.h"
#include "vpcommon.h"

#define IDC_MSC	80322

#define DAILY	1
#define WEEKLY	2
#define MONTHLY	3

#define UWM_DESTROYING	6324

//----------------------------------------------------------------
// 
// class CClientPropSheet
//
//----------------------------------------------------------------
class CClientPropSheet : public CLDVPPropSheet
{
	DECLARE_DYNAMIC(CClientPropSheet)

private:
	CButton			m_btnMsc;
	IConfig			*m_ptrConfig;
	IScanConfig		*m_ptrScanConfig;
	BOOL			m_bDeleteOnClose;
	BOOL			m_bStartedEngine;
	BOOL			m_bScanOpened;
	HINSTANCE		m_hScanDialogs;

	void SetButtonsPos();
	void InitMembers( CWnd *pParent, IConfig *ptrScan );

	virtual BOOL Initialized() = 0;

public:
	CClientPropSheet( LPCTSTR szTitle, CWnd *pParent, UINT flags, IConfig *ptrEngine = NULL);
	CClientPropSheet( DWORD dwTitle, CWnd *pParent, UINT flags, IConfig *ptrEngine = NULL);
	virtual ~CClientPropSheet();

	CTaskBase		*m_ptrCtrl;

	CString			m_strTitle;
	CString			m_strDescription;
	HWND			m_resultsHWnd;
	BOOL			m_bUserTask;
	CString			m_strTaskName;
	DWORD			m_dwScheduleType;
	CString			m_strTime;
	DWORD			m_dwDay;

	IConfig		*GetScanConfig(){ return m_ptrConfig; };
	IScan		*GetScan( IScanCallback* pCallback = NULL );
	void HideBack();
	void ShowBack();
	void ShowMscButton( DWORD dwTitleID );
	void SetFinishText( int ID );
	void SetRecordedData( CEdit *pName, CEdit *pDesc);
	void RenameTask( CEdit *pName, CEdit *pDesc);
	void OnScanStarting() ;
	void OnScanEnding() ;


	void OpenScan(  BOOL bDeleteOnClose, CString strCopyOf = "", DWORD dwRoot = HKEY_VP_USER_SCANS );
	void CloseScan();
	BOOL InitEngine();
	void DeInitEngine();

	virtual BOOL IsWizard() = 0;

protected:
	// Generated message map functions
	//{{AFX_MSG(CWizard)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	virtual void OnMscButton();

};

#endif

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
