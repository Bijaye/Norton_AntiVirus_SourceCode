/*******************************************************************
*                                                                  *
* File name:     rc.cpp                                            *
*                                                                  *
* Description:   Main header file for the RC application           *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
* Author:        Alla Segal                                        *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      The definition of RC application class            *
*                                                                  *
*******************************************************************/

#if !defined(AFX_RC_H__FB9B0684_7D99_11D2_981B_000629167334__INCLUDED_)
#define AFX_RC_H__FB9B0684_7D99_11D2_981B_000629167334__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRcApp:
// See rc.cpp for the implementation of this class
//

class CRcApp : public CWinApp
{
private:
	char ResultPath[MAXPATH];
	PHANDLE VirToken;
    RCError Error;
	RCCommon Common;
	CString AppName;
	int AppVersion;
	int IdleCount;
	void Triage(CString AppName);
	void Clean(char *szDir);
	friend class RCMessageFilter;
protected:
	 PRCMessageFilter m_pMsgFilter;       //Our message filter
public:
	CRcApp();
    
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRcApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRcApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RC_H__FB9B0684_7D99_11D2_981B_000629167334__INCLUDED_)
