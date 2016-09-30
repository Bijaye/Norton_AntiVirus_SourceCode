/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_VIRUSLISTVIEW_H__49737771_AA02_11D0_BF88_00A0C926E0E1__INCLUDED_)
#define AFX_VIRUSLISTVIEW_H__49737771_AA02_11D0_BF88_00A0C926E0E1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VirusListView.h : header file
//
#import "qserver.tlb" no_namespace

#include "listctrlex.h"
#include "Resource.h"
#include "dcomobject.h" 
/////////////////////////////////////////////////////////////////////////////
// CVirusListView dialog

#define NO_VIRUS_READ	100
enum VirusDataFieldIndex
    {	VDF_NAME	= 0,
	VDF_INFECTS	= 1,
	VDF_COMMON	= 2,
	VDF_INFO	= 3
    };

class CVirusListView : public CDialog
{
// Construction
public:
        CString m_strServer;
        void ReadVirusList();
        CVirusListView(CWnd* pParent = NULL);   // standard constructor
        CWinThread *m_pThread;

	    LPSTREAM    m_pServerStream;
	    IEnumSavVirusInfo*  m_pEnum;
	    LPVOID      m_pSnapin;

//        CBA_Addr m_cbaAddr;
// Dialog Data
        //{{AFX_DATA(CVirusListView)
        enum { IDD = IDD_VIEWVIRUSLIST };
        CButton m_ctlAboutVirus;
        CButton m_ctlClose;
        CListCtrlEx     m_ctlVirusList;
        CString m_strPatternFile;
        //}}AFX_DATA


// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CVirusListView)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

// Implementation
protected:
    
        // Generated message map functions
        //{{AFX_MSG(CVirusListView)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        virtual BOOL OnInitDialog();
        afx_msg void OnColumnclickViruslist(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnDestroy();
        afx_msg void OnAboutvirus();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRUSLISTVIEW_H__49737771_AA02_11D0_BF88_00A0C926E0E1__INCLUDED_)
