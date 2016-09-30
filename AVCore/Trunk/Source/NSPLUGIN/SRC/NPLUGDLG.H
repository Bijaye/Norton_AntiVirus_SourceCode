//************************************************************************
// Copyright 1997 Symantec Corporation
// _______________________________________________________________________
//
// $Header:   S:/NSPLUGIN/VCS/nplugdlg.h_v   1.3   10 Sep 1997 17:12:02   KSACKIN  $
//
// Description: Header file for the main dialog class.
//
//************************************************************************
//  $Log:   S:/NSPLUGIN/VCS/nplugdlg.h_v  $
// 
//    Rev 1.3   10 Sep 1997 17:12:02   KSACKIN
// Ported changes from QAKG.
// 
//    Rev 1.2.1.1   22 Aug 1997 00:28:58   KSACKIN
// Added a check box for whether to use NAV as a Netscape plugin or not,
// reworded some text, and added a frame around the advanced section.
// 
//    Rev 1.2.1.0   12 Aug 1997 22:57:14   DALVIDR
// Branch base for version QAKG
// 
//    Rev 1.2   28 Jul 1997 10:41:42   DALLEE
// Added SetTrustedAppValue() to register NAV as a Netscape trusted external
// application. Removed default prompt before helper app in Netscape 4.0.
// 
//    Rev 1.1   09 Jun 1997 13:59:24   MDUNN
// Made the various state variables global, instead of having them be member
// variables of the app class.  This makes the code way more readable and
// intuitive.
// Added the /r and /n switches, with supporting code.
// Added in logic to handle the case where NAV isn't installed (the only
// thing we can do is uninstall NAV as a plug-in).
// 
//    Rev 1.0   21 May 1997 16:32:30   MDUNN
// Initial revision.
//************************************************************************

#ifndef __NPLUGDLG_H__
#define __NPLUGDLG_H__

#define NUM_CONTEXT_HELP_CTRLS           5

// This enum holds return values for CNPlugInDlg::GetHelperAppString()

enum { GHA_OK = 0,                      // No error.
       GHA_NO_VIEWERS_KEY,              // No viewers key (Netscape was badly
                                        // installed or its reg keys are
                                        // munged).
       GHA_NO_VIEWER_VALUE              // The MIME type does not have an
                                        // entry in the helper app list.
     };


// This enum holds constants that reflect the state of the Netscape
// helper app string values when the Plug-in Setup app is launched.

enum { HA_ERROR = 0,                    // Netscape is badly installed, or
                                        // some other bad error.
       HA_NONE,                         // Helper app string does not exist.
       HA_NAV,                          // NAV is the helper app
       HA_OTHER,                        // There is a helper app, but it's
                                        // not NAV.
       HA_EMPTY                         // There is a helper app string, but
                                        // the string is empty.
     };


/////////////////////////////////////////////////////////////////////////////
// CNPlugInDlg dialog

class CNPlugInDlg : public CDialog
{
// Construction
public:
	CNPlugInDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNPlugInDlg)
	enum { IDD = IDD_NPLUGIN_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNPlugInDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

protected:
	void UpdateUseNAVCheckStatus();
	HICON m_hIcon;

    // Member variable that will subclass the listbox.
    CCheckListBox m_checklist;

    // Array of control IDs that respond to a right-lick or context 
    // help click (ie, the [?] button in the dlg caption bar).
    int  m_anContextHelpCtrlIDs [ NUM_CONTEXT_HELP_CTRLS ];

    // Array of MIME types that NAV scans.  This is currently hard-coded
    // in CNPlugInDlg::InitMIMETypesArray(), but can pretty easily be
    // changed to read a list of MIME types from the string table at
    // runtime.
    CStringArray m_acstrMIMETypes;

    // Array of MIME types which have extensions associated with them.
    // These types require special handling.
    CStringArray m_acstrMIMETypesWithExtensions;

    // Array of extension lists.  Each element goes with the 
    // corresponding element of m_aszMIMETypesWithExtensions.
    CStringArray m_acstrExtensions;

    // Array of default values to use when the app is in install mode (/i
    // switch).  For each i, if m_abyDefaults[i] is TRUE then
    // m_acstrMIMETypes[i] will be checked by default, and unchecked if
    // m_abyDefaults[i] is FALSE.
    CByteArray m_abyDefaults;

    // Control ID of a right-clicked control.  Used for What's This? help.
    int m_nRightClickedCtrlID;


    // Helper functions
    BOOL CheckNAVInstalled();
    BOOL CheckNetscapeInstalled();
    void InitHelpIDArray();
    BOOL InitMIMETypesArrays();
    BOOL InitArraysWithHardCodedStrings();
    BOOL FillListbox();
    int  GetPluginItemState ( LPCTSTR szMIMEType );
    
    HKEY GetNetscapeViewersKey ( BOOL bWriteAccess );
    BOOL GetNAVInstallDir      ( LPTSTR szNAVDir );
    BOOL SetTrustedAppValue    ( LPCTSTR szNavCmdLine );
    int  GetHelperAppString    ( LPCTSTR szMIMEType, LPTSTR szHelperAppString );
    BOOL SetHelperAppString    ( LPCTSTR szMIMEType, LPCTSTR szNewString );
    BOOL SetExtensionString    ( LPCTSTR szMIMEType, BOOL bAddExtensions );
    
    BOOL CtrlHasContextHelp    ( int nCtrlID );

    BOOL CreateNetscapeViewerValues ( LPCTSTR szMIMEType,
                                      BOOL    bCreateMIMETypeString,
                                      BOOL    bCreateTYPExxString );


	// Generated message map functions
	//{{AFX_MSG(CNPlugInDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCheckAll();
	afx_msg void OnUncheckAll();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnWhatsThis();
	virtual void OnOK();
    virtual void OnCancel();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnUseNavAsPlugin();
	//}}AFX_MSG
    afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif  // ndef __NPLUGDLG_H__

