// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __VPTOOLBAR_H__
#define __VPTOOLBAR_H__

class CVPToolBar : public CToolBar 
{   
// Construction
public:
	CVPToolBar();
   
// Implementation
public:   
	virtual ~CVPToolBar();

protected:                
	// Generated message map functions
	//{{AFX_MSG(CVPToolBar)
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};        

#endif 



