/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/helpdlg.h_v   1.4   22 Jul 1997 13:13:52   DBUCHES  $
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
// $Log:   S:/N32OPTS/VCS/helpdlg.h_v  $
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

#include "platform.h"
#pragma pack(push, 1)
#include "options.h"
#pragma pack(pop)

#include "afxtempl.h"

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
	void EnableChildWindows(BOOL bEnable);
	BOOL m_bPassDisabled;

	BOOL OnSetActive();
	virtual BOOL GetData(LPNAVOPTIONS pOptions);
	virtual BOOL SetData(LPNAVOPTIONS pOptions);
	virtual BOOL HelpTopicExists(int ID);
	CHelperPage(UINT nIDTemplate);
	CHelperPage();
	~CHelperPage();
	void Initialize();
	inline void SetHowToMenuIndex(UINT uiHTIndex)
		{m_uiMenuIndex = uiHTIndex; };
    virtual LRESULT OnHelpMessage(WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnPasswordCheck(WPARAM wParam, LPARAM lParam);
    CList<CHILDWINDOW, CHILDWINDOW &> m_WindowList;

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
	CWnd*	m_pCurrentContextWnd;
	DWORD * m_pHelpHowToMap;
	DWORD * m_pHelpWhatsThisMap;
	UINT	m_uiMenuIndex;
	int     m_iPasswordFeature;



};

#endif
