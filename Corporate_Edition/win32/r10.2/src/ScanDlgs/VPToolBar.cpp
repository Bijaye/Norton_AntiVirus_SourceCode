// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#include "stdafx.h" 
#include <afxpriv.h>

#include "vptoolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVPToolBar

BEGIN_MESSAGE_MAP(CVPToolBar, CToolBar)
	//{{AFX_MSG_MAP(CVPToolBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
                 
/////////////////////////////////////////////////////////////////////////////
// CVPToolBar Construction/Destruction

CVPToolBar::CVPToolBar()
{
}

CVPToolBar::~CVPToolBar()
{
} 

LRESULT CVPToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM) 
{
	if (IsWindowVisible()) 
	{
		CFrameWnd *pParent = (CFrameWnd *)GetParent();
		if (pParent)
			OnUpdateCmdUI(pParent, (BOOL)wParam);
	}
	return 0L;
}
