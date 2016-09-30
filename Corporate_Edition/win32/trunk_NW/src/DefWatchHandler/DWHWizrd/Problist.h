// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved. 
//*************************************************************************
// ProbList.h - created 02/20/1998 10:43:48 PM
//
// $Header:   S:/NAVWORK/VCS/ProbList.h_v   1.0   09 Mar 1998 23:47:50   DALLEE  $
//
// Description:
//      Declaration of CProblemListCtrl.  Repair Wizard list view control
//      for displaying infected items and virus names.
//
// Contains:
//      CProblemListCtrl
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/ProbList.h_v  $
// 
//    Rev 1.0   09 Mar 1998 23:47:50   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __PROBLIST_H
#define __PROBLIST_H

#include "sortlist.h"
#include "vpcommon.h"

/////////////////////////////////////////////////////////////////////////////
// CProblemListCtrl window
//
// Provides automatic sorting list control filled with problem items.
//
// Usage should be:
//      CProblemListCtrl()
//      Init()
//      FillList()

class CProblemListCtrl : public CSortedListCtrl
{
// Construction
public:
	CProblemListCtrl();

// Attributes
public:
    enum
    {
        COLUMN_NAME = 0,
        COLUMN_VIRUS
    };

// Operations
public:
    virtual BOOL FillList( DWORD dwTypes );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProblemListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL Init( LPVOID lpItemBlockList, int iNumColumns );
	virtual ~CProblemListCtrl();

protected:
    virtual int CompareFunc( LPARAM lParam1, LPARAM lParam2 );
    LPVOID      m_lpItemBlockList;
    CPtrArray*  m_lpVBinItemArray;
    PVBININFO   m_lpVBinInfo;

private:
    BOOL AdjustColumnWidths(int iNumColumns);
    BOOL GetDisplayName( LV_DISPINFO *pDispInfo );

	// Generated message map functions
protected:
	int m_iNumColumns;
	//{{AFX_MSG(CProblemListCtrl)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // !__PROBLIST_H

