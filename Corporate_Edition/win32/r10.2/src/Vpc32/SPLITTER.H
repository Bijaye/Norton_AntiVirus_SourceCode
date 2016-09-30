// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: splitter.h
//  Purpose: CSplitter Definition file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#ifndef SPLITTER_INCLUDED
#define SPLITTER_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// CSplitter frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CSplitter : public CSplitterWnd
{
	DECLARE_DYNCREATE(CSplitter)
public:
	CSplitter();		

// Attributes
protected:
	CSplitterWnd	m_wndSplitter;
public:
	BOOL ReplaceView(int row, int col,CRuntimeClass * pViewClass, SIZE size, BOOL bIgnoreSame = TRUE );

// Operations
public:

// Implementation
public:
	virtual ~CSplitter();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	// Generated message map functions
	//{{AFX_MSG(CSplitter)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif

/*------------------------------------------------------------------
*
* $ History: $
*
*
  ------------------------------------------------------------------*/
