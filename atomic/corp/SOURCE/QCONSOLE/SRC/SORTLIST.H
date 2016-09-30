// Copyright 1998 Symantec Corporation. 
//*************************************************************************
//
// $Header:   S:/QCONSOLE/VCS/SortList.h_v   1.0   11 Mar 1998 15:19:00   DBuches  $
//
// Description:
//      Declaration of CSortedListCtrl. Base class for automatic sorting
//      CListCtrl with nifty arrows on the columns (Thanks, DBUCHES).
//
// Contains:
//      CSortedListCtrl
//*************************************************************************
// $Log:   S:/QCONSOLE/VCS/SortList.h_v  $
// 
//    Rev 1.0   11 Mar 1998 15:19:00   DBuches
// Initial revision.
//*************************************************************************

#ifndef __SORTLIST_H
#define __SORTLIST_H

/////////////////////////////////////////////////////////////////////////////
// CSortedListCtrl window
//
// Class which handles ascending and descending sorts as list control 
// columns are clicked.
//
// Derive a class from CSortedListCtrl and use instead of CListCtrl for your 
// MFC dialog control member variable.
//
// You will need to provide an implementation of CompareFunc() in your
// derived class for sorting based on your list control's item data.
//
// If you override OnDrawItem(), be sure to chain through to CSortedListCtrl's
// version for drawing the column headers.

class CSortedListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC( CSortedListCtrl )

// Construction
public:
	CSortedListCtrl();

// Attributes
public:

// Operations
public:
    int InsertColumn( int nCol, const LV_COLUMN* pColumn );
    int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, 
                      int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1 );
    BOOL SortItems();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortedListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortedListCtrl();

protected:
    BOOL    m_bSortAscending;
    int     m_nSortColumn;
    virtual int CompareFunc( LPARAM lParam1, LPARAM lParam2 ) = 0;

private:
	static int CALLBACK StaticCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lData );
    void DrawArrow( CDC* pDC, int x, int y, BOOL bUp);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSortedListCtrl)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // !__SORTLIST_H