/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/HELPDLG.H_v   1.0   20 May 1998 17:15:34   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
//
// HLPropPg.h : header file
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/HELPDLG.H_v  $
// 
//    Rev 1.0   20 May 1998 17:15:34   DBuches
// Initial revision.
// 
//    Rev 1.1   29 Dec 1997 16:53:58   DBuches
// Added help support.
// 
//    Rev 1.0   29 Dec 1997 16:33:38   DBuches
// Initial revision.
// 
//    Rev 1.0   20 Aug 1997 18:32:28   DBUCHES
// Initial revision.
//
//    Rev 1.4   22 Jul 1997 13:13:52   DBUCHES
// Added call to NG_SetFontsInDialog()
//
//    Rev 1.3   20 Jun 1997 12:01:08   DBUCHES
// Added Help support.
//
//    Rev 1.2   27 May 1997 20:56:20   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.1   16 May 1997 15:28:14   DBUCHES
// Fixed up pragmas
//
//    Rev 1.0   14 May 1997 15:02:32   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#ifndef _CHelperDialog_CLASS
#define _CHelperDialog_CLASS

//#include "platform.h"
#include "resource.h"

typedef struct tCHILDWINDOW {
    HWND hWnd;
    BOOL bEnabled;
    } CHILDWINDOW;


/////////////////////////////////////////////////////////////////////////////
// CHelperDialog dialog

class CHelperDialog : public CDialog
{

// Construction
public:
	DWORD m_dwHelpButtonHelpID;
	CHelperDialog(UINT nIDTemplate, CWnd * pParent = NULL);
	~CHelperDialog();
	void Initialize();
	void SetHowToMenuIndex(UINT uiHTIndex)
		{m_uiMenuIndex = uiHTIndex; };
    virtual LRESULT OnHelpMessage(WPARAM wParam, LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CHelperDialog)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHelperDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHelperDialog)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

    virtual afx_msg BOOL MainDlgWhatsThisHelp(int ID);
	afx_msg void OnHelpInfoDesk();
	afx_msg void OnWhatsThis();
    afx_msg void ShutdownHelp(void);

	DECLARE_MESSAGE_MAP()

protected:
	CWnd*	m_pCurrentContextWnd;
	DWORD * m_pHelpHowToMap;
	DWORD * m_pHelpWhatsThisMap;
    DWORD * m_pHelpWhatsThisGrayMap;
	UINT	m_uiMenuIndex;

};

/////////////////////////////////////////////////////////////////////////////
// CHelperPage dialog

class CHelperPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CHelperPage)

// Construction
public:
    DWORD m_dwHelpButtonHelpID;
    BOOL OnInitDialog();

    virtual BOOL HelpTopicExists(int ID);
    CHelperPage(UINT nIDTemplate);
    CHelperPage();
    ~CHelperPage();
    void Initialize();
    virtual LRESULT OnHelpMessage(WPARAM wParam, LPARAM lParam);


// Dialog Data
    //{{AFX_DATA(CHelperPage)
        // NOTE - ClassWizard will add data members here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CHelperPage)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CHelperPage)
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    //}}AFX_MSG
    afx_msg void OnHelpInfoDesk();
    afx_msg void OnWhatsThis();

    DECLARE_MESSAGE_MAP()

protected:
    CWnd*   m_pCurrentContextWnd;
    DWORD * m_pHelpHowToMap;
    DWORD * m_pHelpWhatsThisMap;
    UINT    m_uiMenuIndex;

};




#endif
