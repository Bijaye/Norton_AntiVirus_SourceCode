/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_FETCHFILEDLG_H__680CF630_DBE5_11D2_ADE4_00104BF3F36A__INCLUDED_)
#define AFX_FETCHFILEDLG_H__680CF630_DBE5_11D2_ADE4_00104BF3F36A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FetchFileDlg.h : header file
//
#include "resource.h"
#include "fetchfileData.h"
#include "itemdata.h"
#include "qsconsole.h"

class CFetchedFileData
{
public:
    CFetchedFileData( DWORD dwID, LPCTSTR szFileName )
        {
        dwFileID = dwID;
        sFileName = szFileName;
        }
    
    // 
    // Temporary file created by file fetcher.
    // 
    CString sFileName;
    
    // 
    // File ID.
    // 
    DWORD dwFileID;
};


/////////////////////////////////////////////////////////////////////////////
// CFetchFileDlg dialog

class CFetchFileDlg : public CDialog
{
// Construction
public:
	CFetchFileDlg(CWnd* pParent = NULL);   // standard constructor
    ~CFetchFileDlg();
    HRESULT Initialize( CSnapInItem* pRootNode,
                        CItemData** apItem, 
                        DWORD dwNumItems,
                        LPUNKNOWN pCopyUnk );

    DWORD GetNumFilesFetched(){ return m_dwFilesSoFar; };

// Dialog Data
	//{{AFX_DATA(CFetchFileDlg)
	enum { IDD = IDD_FETCH_FILE_DIALOG };
	CProgressCtrl	m_progress;
	CString	m_sProgressText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFetchFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnCancel();
	//}}AFX_VIRTUAL

public:
    // 
    // Array of file info structures.
    // 
    CTypedPtrArray< CPtrArray, CFetchedFileData* > m_aFileDataArray;


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFetchFileDlg)
        afx_msg LRESULT OnEndJob( WPARAM wParam, LPARAM lParam );
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    static BOOL CopyCallback( DWORD dwMessage,
                             DWORD dwFileID, 
                             LPCTSTR szTempFileName,
                             DWORD dwUserData );

private:
    // 
    // Pointer to copy interface unknown
    // 
    CComPtr< IUnknown > m_pUnk;

    // 
    // List of item IDs to fetch.
    // 
    DWORD*              m_adwItemIDs;

    // 
    // Number of items we are fetching.
    // 
    DWORD               m_dwNumItems;

    // 
    // Cancel flag
    // 
    BOOL                m_bCancelled;

    // 
    // File fetching object.
    // 
    CFetchFileData      m_fetch;

    // 
    // Count of files fetched so far.
    // 
    DWORD               m_dwFilesSoFar;
};

// 
// Dialog specific messages.
// 
#define WM_FETCHDLG_END_JOB         (WM_USER+1)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_FETCHFILEDLG_H__680CF630_DBE5_11D2_ADE4_00104BF3F36A__INCLUDED_)
