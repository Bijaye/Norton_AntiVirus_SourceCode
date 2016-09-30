/////////////////////////////////////////////////////////////////////
// AlertOpt.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/alertopt.h_v   1.5   10 Jun 1997 20:11:56   DBUCHES  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/alertopt.h_v  $
// 
//    Rev 1.5   10 Jun 1997 20:11:56   DBUCHES
// Added password support.
// 
//    Rev 1.4   04 Jun 1997 21:32:14   DBUCHES
// Added OnHandleSNMPEdit().
// 
//    Rev 1.3   04 Jun 1997 21:14:50   DBUCHES
// Changed to support SNMP Alerts.
// 
//    Rev 1.2   27 May 1997 20:14:06   DBUCHES
// Changes to support integration with N32OPTS.
// 
//    Rev 1.1   25 Apr 1997 11:03:50   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.1   15 Apr 1997 20:02:04   sedward
// Removed pager target stuff.
//
//    Rev 1.22   05 Feb 1997 23:04:58   SEDWARD
// Increased NUM_ALERTOPT_GROUPBOX_CONTROLS to 6.
//
//    Rev 1.21   02 Feb 1997 16:35:36   SEDWARD
// Added support for context menus that are grayed out AND in groupbox controls.
//
//    Rev 1.20   28 Jan 1997 20:00:44   SEDWARD
// InitControls is now called by the corresponding export function.
//
//    Rev 1.19   27 Jan 1997 21:39:36   SEDWARD
// Allow removal of the 'master NLM item' from the main tree control.
//
//    Rev 1.17   27 Jan 1997 15:30:36   SEDWARD
// Removed 'AdjustPopupMenuItems' function.
//
//    Rev 1.16   22 Jan 1997 16:56:12   SEDWARD
// Added 'OnSelChanged' function (fix for #79159).
//
//    Rev 1.15   21 Jan 1997 17:24:38   SEDWARD
// Nuked the 'ActivateTreeItem' function.
//
//    Rev 1.14   21 Jan 1997 13:43:04   SEDWARD
// Right-menu clicks now make the item selected in the tree control.
//
//    Rev 1.13   19 Jan 1997 23:47:40   SEDWARD
// Added prototype for 'synchronize' function.
//
//    Rev 1.12   13 Jan 1997 20:41:44   SEDWARD
// Added support for help.
//
//    Rev 1.11   06 Jan 1997 00:43:04   SEDWARD
// Both Add and Modify buttons enabled for parent branches.
//
//    Rev 1.8   09 Dec 1996 20:48:52   SEDWARD
//
//    Rev 1.7   04 Dec 1996 16:16:24   SEDWARD
// Enabled the Remove right-menu item from the main tree control.
//
//    Rev 1.6   03 Dec 1996 21:07:24   SEDWARD
// Added Net Messenger icon for main tree control.
//
//    Rev 1.4   02 Dec 1996 16:34:36   SEDWARD
// Big changes, nearing UI freeze.
//
//    Rev 1.3   27 Nov 1996 18:48:24   SEDWARD
// Hooked up NAVOPTS to local alert message configuration.
//
//    Rev 1.2   26 Nov 1996 00:54:40   SEDWARD
// More sweeping changes.
//
//    Rev 1.1   25 Nov 1996 15:45:46   SEDWARD
// Major overhaul (more to come).
//
//    Rev 1.0   21 Nov 1996 17:26:08   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#ifndef ALERT_OPT_HEADER
#define ALERT_OPT_HEADER

#include "AlertUI.h"
#include "at_opts.h"                // For CAlertOptions class
#include "helpdlg.h"                // base class for default help support
#include "afxtempl.h"

// general defined values
#define DOUBLE_BACKSLASH            _T("\\\\")
#define INVALID_LPARAM_VALUE        0xffffffff
#define SINGLE_BACKSLASH            _T('\\')
#define NUM_ALERTOPT_GROUPBOX_CONTROLS  6

// alert tree control (ATC) bitmap indices (to index the image list items)
const   int         ATC_NET_MESSAGE_BITMAP      = 0;
const   int         ATC_EMAIL_BITMAP            = 1;
const   int         ATC_PAGER_BITMAP            = 2;
const   int         ATC_NLM_BITMAP              = 3;
const   int         ATC_FORWARD_BITMAP          = 4;
const   int         ATC_CHECKED_BITMAP          = 5;
const   int         ATC_UNCHECKED_BITMAP        = 6;
const   int         ATC_LOCAL_MACHINE_BITMAP    = 7;
const   int         ATC_MASTER_NLM_BITMAP       = 8;
const   int         ATC_GRAY_UNCHECKED_BITMAP   = 9;
const   int         ATC_GRAY_CHECKED_BITMAP     = 10;
const   int         ATC_SNMP_BITMAP             = 11;


// NOTE: "local machine" isn't really an alert type target, but the value
//       defined here is arbitrarily chosen not to collide with the "real"
//       alert target types defined in "at_tgt.h"
#define TARGET_LOCAL_MACHINE        0x00000200
#define TARGET_NONE                 0x00000000

// these are used to enable/disable the main dialog buttons
#define ENABLE_ADD_BUTTON           0x00000001
#define ENABLE_MODIFY_BUTTON        0x00000002
#define ENABLE_REMOVE_BUTTON        0x00000004

// Childwindow enum struct
typedef struct tCHILDWINDOW {
    HWND hWnd;
    BOOL bEnabled;
    } CHILDWINDOW;


/////////////////////////////////////////////////////////////////////////////
// CAlertOptsDlg dialog

class AFX_EXT_CLASS CAlertOptsDlg : public CHelpPropertyPage
{
// Construction
public:
    CAlertOptsDlg();                      // standard constructor
    CAlertOptsDlg(HWND  parentHandle);

    ~CAlertOptsDlg();
    
    

// Dialog Data
    //{{AFX_DATA(CAlertOptsDlg)
	enum { IDD = IDD_ALERT_OPTS };
	CButton	m_addButton;
	CButton	m_modifyButton;
	CButton	m_removeButton;
	CButton	m_scannerCheckbox;
	CButton	m_apCheckbox;
	CTreeCtrl	m_alertTree;
	BOOL	m_IPXCheckBox;
	BOOL	m_SAMCheckBox;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAlertOptsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
    // data members
    DWORD           m_alertType;
    HTREEITEM       m_localMachineTreeHandle;
    HTREEITEM       m_netMessageTreeHandle;
    HTREEITEM       m_SMTPTreeHandle;
    HTREEITEM       m_nlmTreeHandle;
    HTREEITEM       m_forwardTreeHandle;
    HTREEITEM       m_SNMPTreeHandle;
    HTREEITEM       m_rightClickHandle;
	BOOL 			m_bPassDisabled;    
    CList<CHILDWINDOW, CHILDWINDOW &> m_WindowList;
	
    static DWORD    m_dwWhatsThisMap[];
    static DWORD    m_dwWhatsThisGrayMap[];
    static DWORD    m_dwHowToMap[];

    // functions
    BOOL            InitTreeControl(void);

protected:
    // data members
    CImageList      m_imageList;
    CWnd*           m_groupboxControls[NUM_ALERTOPT_GROUPBOX_CONTROLS + 1];

    // functions
    HTREEITEM   AddEmailTargetBranch(HTREEITEM  parentHandle
                                                , CAlertTarget*  recPtr);
    HTREEITEM   AddMachineTargetBranch(HTREEITEM  parentHandle
                                                , CAlertTarget*  pAlertTarget);
    void        EnableEditButtons(DWORD  flags);
    HTREEITEM   GetChildTreeHandle(HTREEITEM  parentHandle
                                                , CString  childStr);
    HTREEITEM   GetHitTreeItemHandle(UINT*  treeItemFlags = NULL);
    int         GetRecMenuID(HTREEITEM  treeHandle);
    BOOL        HandleAlertTypeEdit(void);
    BOOL        HandleChildItemEdit(HTREEITEM  treeHandle);
    void        HandleChildCheckbox(HTREEITEM  treeHandle);
    BOOL        HandleChildItemClick(HTREEITEM  treeHandle, UINT  itemFlags);
    BOOL        HandleEmailListEdit(void);
    BOOL        HandleForwardListEdit(void);
    BOOL        HandleForwardModify(HTREEITEM  treeHandle);
    BOOL        HandleLocalMachineEdit(void);
    BOOL        HandleNetMessageListEdit(void);
    BOOL        HandleNetMessageModify(HTREEITEM  treeHandle);
    BOOL        HandleNLMListEdit(void);
    BOOL        HandleNLMModify(HTREEITEM  treeHandle);
    BOOL        HandleParentEdit(HTREEITEM  treeHandle);
    void        HandleRclickAlertTree(void);
    BOOL        HandleRemoveTreeChild(HTREEITEM  treeHandle);
    BOOL        HandleSMTPModify(HTREEITEM  treeHandle);
    BOOL        HandleSNMPModify(HTREEITEM  treeHandle);
    void        InsertParentBranches(void);
    BOOL        IsMasterNLMTreeHandle(HTREEITEM  treeHandle);
    BOOL        IsTreeControlCommandID(int  cmdID);
    BOOL        IsParentTreeHandle(HTREEITEM  treeHandle);
    DWORD       MapTreeHandleToAlertType(HTREEITEM  treeHandle);
    BOOL        ReadAlertTargetRecords(void);
    void        SynchronizeEditButtonsAndContextMenu(CMenu  *menuPtr);
    void        UpdateEditButtons(HTREEITEM  treeHandle);
    BOOL        UpdateEmailTreeDisplay(void);
    BOOL        UpdateMachineTargetTreeDisplay(HTREEITEM  parentHandle);
    BOOL        UpdateMasterNLMTreeBranch(BOOL  masterPresent);
    void        UpdateNLMTreeChildren(BOOL  masterPresent);
    BOOL        HandleSNMPListEdit(void);
    void        OnHandleSNMPAdd(void);
	void 		EnableChildWindows(BOOL bEnable);
// Implementation
public:
    BOOL        CreateDlg(HWND  parentHandle);
    void        InitControls(void);
    BOOL 		GetData(LPNAVOPTIONS pOptions);
    BOOL 		SetData(LPNAVOPTIONS pOptions);
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

protected:
	HICON m_hIcon;

	// Generated message map functions
    //{{AFX_MSG(CAlertOptsDlg)
	virtual BOOL OnInitDialog(void);
	virtual BOOL OnSetActive();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint(void);
	afx_msg HCURSOR OnQueryDragIcon(void);
	afx_msg void OnClickAlertTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnHandleEmailListEdit(void);
    afx_msg void OnHandleForwardAdd(void);
    afx_msg void OnHandleSMTPAdd(void);
    afx_msg void OnHandleNetMessageAdd(void);
    afx_msg void OnHandleNetMessageListEdit(void);
    afx_msg void OnHandleNLMAdd(void);
    afx_msg void OnHandleNLMListEdit(void);
    afx_msg void OnHandleForwardListEdit(void);
    afx_msg void OnHandleLocalMachineEdit(void);
	afx_msg void OnDblclkAlertTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRmenuForwardModify(void);
    afx_msg void OnRmenuNetMessageModify(void);
    afx_msg void OnRmenuSMTPModify(void);
    afx_msg void OnRmenuNLMModify(void);
    afx_msg void OnRmenuRemoveTreeChild(void);
    afx_msg void OnScannerCheckbox(void);
    afx_msg void OnAutoProtectCheckbox(void);
	afx_msg void OnAddButton();
	afx_msg void OnModifyButton();
	afx_msg void OnRemoveButton();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnWhatsThisAlertTree(void);
	afx_msg void OnSelchangedAlertTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRmenuSNMPModify(void);
	afx_msg void OnIpxCheck();
	afx_msg void OnSamCheck();
	afx_msg void OnHandleSNMPListEdit();
	afx_msg LRESULT OnPasswordCheck(WPARAM wParam, LPARAM lParam);
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


// global variable
extern  CAlertOptsDlg*      gAlertOptsDlgPtr;


#endif  //  ALERT_OPT_HEADER
