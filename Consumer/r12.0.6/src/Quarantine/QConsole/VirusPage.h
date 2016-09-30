/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/VirusPage.h_v   1.2   06 Mar 1998 11:20:26   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// VirusPage.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/VirusPage.h_v  $
// 
//    Rev 1.2   06 Mar 1998 11:20:26   DBuches
// Updated to use IQuarantineItem pointer to update data.
// 
//    Rev 1.1   02 Mar 1998 15:25:30   DBuches
// Checked in work in progress.
// 
//    Rev 1.0   27 Feb 1998 15:10:28   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIRUSPAGE_H__47B210C4_A968_11D1_A523_0000C06F46D0__INCLUDED_)
#define AFX_VIRUSPAGE_H__47B210C4_A968_11D1_A523_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "proppage.h"
#include "imagebutton.h"

/////////////////////////////////////////////////////////////////////////////
// CVirusPage dialog

class CVirusPage : public CItemPage
{
	DECLARE_DYNCREATE(CVirusPage)

// Construction
public:
	CVirusPage();
	~CVirusPage();

// Dialog Data
	//{{AFX_DATA(CVirusPage)
	enum { IDD = IDD_VIRUS_PAGE };
	CImageButton	m_Trigger;
	CImageButton	m_SizeStealth;
	CImageButton	m_Poly;
	CImageButton	m_Memory;
	CImageButton	m_FullStealth;
	CImageButton	m_Encrypting;
	CString	m_sAliases;
	CString	m_sComments;
	CString	m_sInfects;
	CString	m_sLength;
	CString	m_sLikelihood;
	CString	m_sVirusName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVirusPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CVirusPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    void SetVirusIndex( unsigned long ulVID ) { m_ulVID = ulVID; }

private:
    CImageList m_ImageList;

    void BuildInfectsString();

    CSymPtr<IScanVirusInfo> m_pScanVirusInfo;

    unsigned long m_ulVID;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRUSPAGE_H__47B210C4_A968_11D1_A523_0000C06F46D0__INCLUDED_)
