/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/IconBtn.h_v   1.0   25 Jun 1997 15:14:34   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
//
// IconBtn.h : header file
//
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
// $Log:   S:/N32OPTS/VCS/IconBtn.h_v  $
// 
//    Rev 1.0   25 Jun 1997 15:14:34   DBUCHES
// Initial revision.
//


#if !defined(AFX_ICONBTN_H__708E0015_ED9B_11D0_8AEC_0060977341AC__INCLUDED_)
#define AFX_ICONBTN_H__708E0015_ED9B_11D0_8AEC_0060977341AC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//

/////////////////////////////////////////////////////////////////////////////
// CIconButton window

class CIconButton : public CButton
{
// Construction
public:
	CIconButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetIconID(int iID);
	virtual ~CIconButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIconButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
    HICON m_hIcon;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONBTN_H__708E0015_ED9B_11D0_8AEC_0060977341AC__INCLUDED_)
