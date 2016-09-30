// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/ScheduleDlg.h_v   1.5   25 Jul 1997 20:54:22   JBRENNA  $
//
// Description:
//   Provides the header file for CScheduleDlg.
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/ScheduleDlg.h_v  $
// 
//    Rev 1.5   25 Jul 1997 20:54:22   JBRENNA
// Rework the random days and times. We now pre-compute the random days and
// times. When the user toggles between BiMonthly or overnight, we toggle
// between the pre-computed days / times.
// 
//    Rev 1.4   01 Jul 1997 17:07:24   JBRENNA
// Move InitHelpContextIds()s from data section to function section.
// 
//    Rev 1.3   16 Jun 1997 13:39:46   JBRENNA
// Add context sensitive help prototypes.
// 
//    Rev 1.2   10 Jun 1997 09:19:12   JBRENNA
// Put a "Prompt" checkbox on the ScheduleAdd dialog. When checked, the
// NPS Scheduled LiveUpdate command line includes the /PROMPT option.
// 
//    Rev 1.1   01 Jun 1997 19:22:20   JBRENNA
// 1. Add __SCHEDULEDLG_H wrapper.
// 2. Add AddNpsEvents prototype.
// 3. Change m_strDay1 and m_strDay2 to be intergers (m_nDay?)
// 4. Add m_strTime1 and m_strTime2.
// 5. Add a $log$-ing comment header.
// 
//****************************************************************************

#ifndef __SCHEDULEDLG_H
#define __SCHEDULEDLG_H

/////////////////////////////////////////////////////////////////////////////
// CScheduleDlg dialog

class CScheduleDlg : public CDialog
{
// Construction
public:
	CScheduleDlg(CWnd* pParent = NULL);   // standard constructor

	BOOL ShouldDisplay();
    
// Dialog Data
	//{{AFX_DATA(CScheduleDlg)
	enum { IDD = IDD_SCHEDULE };
	BOOL	m_bNotShowAgain;
	BOOL	m_bNightsOk;
	BOOL	m_bSchedBiMonthly;
	CString	m_strTime1;
	CString	m_strTime2;
	int		m_nDay1;
	int		m_nDay2;
	BOOL	m_bPrompt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScheduleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScheduleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNightsOkChk();
	virtual void OnOK();
	afx_msg void OnBimonthly();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelpBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Private Functions
private:
    void    CalcRandomDays ();
    void    CalcRandomTimes ();
    void    SetInitialDays (BOOL bBiMonthly) const;
    void    SetInitialTimes ();
    CString FormatTime (const CTime &Time) const;
    void    SaveOptions ();
    UINT    AddNpsEvents ();
	void    InitHelpContextIds();

// Private Data
private:
    int     m_nDay1NBi;
    int     m_nDay2NBi;
    int     m_nDay1Bi;
    int     m_nDay2Bi;

    CString m_strDayTime;
    CString m_strNightTime;
};

#endif // __SCHEDULEDLG_H
