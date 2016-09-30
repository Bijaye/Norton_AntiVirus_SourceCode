/////////////////////////////////////////////////////////////////////
// NLMPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/NLMPG.H_v   1.1   25 Apr 1997 11:03:54   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/NLMPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:03:54   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:26   RFULLER
// Initial revision
// 
//    Rev 1.10   14 Jan 1997 13:24:20   SEDWARD
// Fixed class inheritance (now from CHelpDialog instead of CDialog).
//
//    Rev 1.8   07 Jan 1997 15:08:54   SEDWARD
// Removed ALERT_ALL_NLMS_DATA_STR #define.
//
//    Rev 1.5   09 Dec 1996 20:48:26   SEDWARD
//
//    Rev 1.4   03 Dec 1996 19:50:48   SEDWARD
// Substantial changes (not frozen yet...)
//
//    Rev 1.3   02 Dec 1996 16:34:28   SEDWARD
// Big changes, nearing UI freeze.
//
//    Rev 1.2   26 Nov 1996 00:54:26   SEDWARD
// More sweeping changes.
//
//    Rev 1.1   25 Nov 1996 15:45:30   SEDWARD
// Major overhaul (more to come).
//
//    Rev 1.0   21 Nov 1996 17:26:14   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include "helpdlg.h"                // base class for default help support


#define MASTER_NLM_ITEM_INDEX       0   // position in the list control


/////////////////////////////////////////////////////////////////////////////
// CNLMPg dialog
/////////////////////////////////////////////////////////////////////////////

class CNLMPg : public CHelpPropertyPage
{
	DECLARE_DYNCREATE(CNLMPg)

// Construction
public:
	CNLMPg();
	~CNLMPg();

// Dialog Data
	//{{AFX_DATA(CNLMPg)
	enum { IDD = IDD_NLM_PROP_PAGE };
	CButton	m_allNLMCheckbox;
	CListCtrl	m_nlmList;
	CButton	m_nlmRemoveButton;
	CButton	m_nlmModifyButton;
	CButton	m_nlmAddButton;
	//}}AFX_DATA

    static DWORD    m_dwWhatsThisMap[];
    static DWORD    m_dwHowToMap[];


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNLMPg)
	public:
    virtual void OnOK(void);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNLMPg)
	afx_msg void OnNlmAddButton(void);
	afx_msg void OnClickNlmTargetList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog(void);
    afx_msg void OnNlmRemoveButton(void);
    afx_msg void OnIncludeAllNlmsCheckbox(void);
	afx_msg void OnNlmModifyButton(void);
    afx_msg void OnHandleNLMModify(void);
    afx_msg void OnHandleNLMAdd(void);
    afx_msg void OnHandleNLMRemove(void);
	afx_msg void OnDblclkNlmTargetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnWhatsThisNLMListCtrl(void);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    void    HandleRclickNLMTargetList(void);

    CAlertOptions*  m_targetsListPtr;

public:
// data members
    int         m_clickedItemIndex;
    CImageList  m_imageList;

// functions
    int         InitListbox(void);
    BOOL        MasterNLMItemInit(void);
    BOOL        UpdateMasterNLMItem(BOOL  checkedState);
    int         UpdateNLMItemIcons(BOOL  checkedState);
};
