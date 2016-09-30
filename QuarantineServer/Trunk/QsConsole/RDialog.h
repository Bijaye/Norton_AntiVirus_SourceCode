/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_RDIALOG_H__11BED5B1_CB57_11D2_8F46_3078302C2030__INCLUDED_)
#define AFX_RDIALOG_H__11BED5B1_CB57_11D2_8F46_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RDialog.h : header file
//

#include "FetchFileData.h"
#include "ItemData.h"
#include "avscanner.h"
#include "qscon.h"
#include "qcresult.h"

/////////////////////////////////////////////////////////////////////////////
// CRDialog dialog

class CRDialog : public CDialog
{
// Construction
public:
	CRDialog(CWnd* pParent = NULL);   // standard constructor
    virtual ~CRDialog();
// Dialog Data
	//{{AFX_DATA(CRDialog)
	enum { IDD = IDD_RESTORE_DIALOG };
	CListCtrl	m_ListCtrl;
	CProgressCtrl	m_progress;
	CString	m_sProgressText;
	CString	m_sDescr;
	//}}AFX_DATA

    HRESULT Initialize( CItemData** apItems, 
                        DWORD dwNumItems, 
                        LPCTSTR szDestFolder, 
                        LPUNKNOWN pUnk,
                        CSnapInItem* pRootNode,
                        CAVScanner* pScanner = NULL );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRDialog)
	virtual BOOL OnInitDialog();
    LRESULT OnJobDone( WPARAM wParam, LPARAM lParam );
    LRESULT OnSetProgress( WPARAM wParam, LPARAM lParam );
    LRESULT OnFetchFiles( WPARAM wParam, LPARAM lParam );
    LRESULT OnCantRepair( WPARAM wParam, LPARAM lParam );
    LRESULT OnThreadError( WPARAM wParam, LPARAM lParam );
    LRESULT OnErrorNoFile( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    static BOOL CopyCallback( DWORD dwMessage,
                             DWORD dwFileID, 
                             LPCTSTR szTempFileName,
                             DWORD dwUserData );

    BOOL GetSaveLocation( LPCTSTR szFileName, 
                          LPCTSTR szSaveFolder, 
                          CString& sFullSavePath );
    
    CItemData* GetItemByID( DWORD dwID );

    BOOL RepairFile( CItemData* pItem, LPCTSTR szTempFileName, BOOL *pbRepaired );

private:
    // 
    // File fetching object
    // 
    CFetchFileData      m_fetch;
    
    // 
    // Are we reparing?
    // 
    BOOL                m_bRepair;

    // 
    // File IDs.
    // 
    CItemData**         m_apItems;

    // 
    // Number of files.
    // 
    DWORD               m_dwNumItems;

    // 
    // Destination folder for files.
    // 
    CString             m_sDestFolder;

    // 
    // Cancelled flag.
    // 
    BOOL                m_bCancelled;

    // 
    // Number of files fetched so far.
    // 
    DWORD               m_dwFilesSoFar;

    // 
    // Done flag
    // 
    BOOL                m_bDone;

    // 
    // IUnknown for copy file interface.
    // 
    CComPtr< IUnknown>  m_pUnk;

    // 
    // Pointer to Scanner object
    // 
    CAVScanner*         m_pAVScanner;

    // 
    // Stream for interface marshalling.
    // 
    IStream*			m_pStream;

    // 
    // Marshalled Quarantine server interface pointer
    // 
    CComPtr< IQuarantineServer > m_pQServer;

};


#define WM_RDIALOG_FETCH_FILES      (WM_USER+1) 
#define WM_RDIALOG_SET_PROGRESS     (WM_USER+2)
#define WM_RDIALOG_JOB_DONE         (WM_USER+3)
#define WM_RDIALOG_ERROR            (WM_USER+4)
#define WM_RDIALOG_CANT_REPAIR      (WM_USER+5)
#define WM_RDIALOG_ERROR_NO_FILE    (WM_USER+6)    

#define RDIALOG_COL_FILENAME            0
#define RDIALOG_COL_DESTINATION         1
#define RDIALOG_COL_STATUS              2


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RDIALOG_H__11BED5B1_CB57_11D2_8F46_3078302C2030__INCLUDED_)
