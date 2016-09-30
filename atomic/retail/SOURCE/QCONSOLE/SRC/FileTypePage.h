/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/filetypepage.h_v   1.2   21 May 1998 11:29:06   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// FileTypePage.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/filetypepage.h_v  $
// 
//    Rev 1.2   21 May 1998 11:29:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.1   20 May 1998 17:11:18   DBuches
// 1st pass at help.
// 
//    Rev 1.0   03 Apr 1998 13:30:20   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILETYPEPAGE_H__54EAACA4_C906_11D1_9114_00C04FAC114A__INCLUDED_)
#define AFX_FILETYPEPAGE_H__54EAACA4_C906_11D1_9114_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "iquaran.h"
#include "helpdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CFileTypePage dialog

class CFileTypePage : public CHelperPage
{
	DECLARE_DYNCREATE(CFileTypePage)

// Construction
public:
	CFileTypePage();
	~CFileTypePage();

    void Initialize( IQuarantineOpts * pOpts, int iType );

// Help maps.
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];

    CImageList m_Images;

public:    
    // Pointer to options data
    IQuarantineOpts *m_pOpts;

    int m_iFileType;

// Dialog Data
	//{{AFX_DATA(CFileTypePage)
	enum { IDD = IDD_OPTIONS_ACTIONS_PAGE };
	BOOL	m_bDelete;
	BOOL	m_bRepair;
	BOOL	m_bRestore;
	BOOL	m_bSubmit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFileTypePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFileTypePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

// 
// File panel types
#define FILE_TYPE_QUARANTINE        0
#define FILE_TYPE_BACKUP            1
#define FILE_TYPE_SUBMITTED         2

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETYPEPAGE_H__54EAACA4_C906_11D1_9114_00C04FAC114A__INCLUDED_)
