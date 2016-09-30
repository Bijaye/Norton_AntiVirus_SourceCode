/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// SmartEdit.cpp : implementation file
//

#include "stdafx.h"
#include "avisconsole.h"
#include "SmartEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmartEdit

CSmartEdit::CSmartEdit() : m_bEscapeKey(FALSE),
						   m_bReturnKey(FALSE)
{
}

CSmartEdit::~CSmartEdit()
{
}


BEGIN_MESSAGE_MAP(CSmartEdit, CEdit)
	//{{AFX_MSG_MAP(CSmartEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmartEdit message handlers

void CSmartEdit::OnKillfocus() 
{
	ATLTRACE(_T("Killing smartwindow This = 0x%x, Hwnd = 0x%x\n"),this, m_hWnd);
	if(!m_bEscapeKey)
	{
		CString str;
		GetWindowText(str); 
		if(str.IsEmpty())
			str.LoadString(IDS_ZERO_TIME);
		m_pEventObject->m_sTime = str;
		
		GetOwner()->PostMessage(WM_USER_UPDATE_VALUE,NULL,(LPARAM)m_pEventObject);
	}
	PostMessage(WM_CLOSE, 0, 0);
	GetOwner()->PostMessage(WM_USER_EDIT_CLOSING,NULL,(LPARAM)m_pEventObject);
	
}

void CSmartEdit::PostNcDestroy() 
{
	delete this;
	CEdit::PostNcDestroy();
}


void CSmartEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_RETURN)
	{
		m_bReturnKey = TRUE;
		PostMessage(WM_CLOSE, 0, 0);
	}
	else if (nChar == VK_ESCAPE)
	{
		m_bEscapeKey = TRUE;
		PostMessage(WM_CLOSE, 0, 0);
	}
	else if ((IsCharAlphaNumeric((TCHAR)nChar) && !IsCharAlpha((TCHAR)nChar)) || nChar == VK_BACK)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);  // permitted
	}
	else
	{
		// illegal character - inform parent
		OnBadInput();
	}
}
/////////////////////////////////////////////////////////////////////////////
// default bad input handler, beep (unless parent notification
//    returns -1.  Most parent dialogs will return 0 or 1 for command
//    handlers (i.e. Beep is the default)

void CSmartEdit::OnBadInput()
{
	if (GetParent()->SendMessage(WM_COMMAND,
		MAKELONG(GetDlgCtrlID(), PEN_ILLEGALCHAR), (LPARAM)m_hWnd) != -1)
	{
		MessageBeep((UINT)-1);
	}
}

int CSmartEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CString string;
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetWindowText(m_pEventObject->m_sTime);
	SetFocus();
	LimitText(6); // user defined maximum length of string
	SetSel(0, -1);  // set selection  all text
	ATLTRACE(_T("Creating smartwindow This = 0x%x, Hwnd = 0x%x\n"),this, m_hWnd);
	
	return 0;
}


void CSmartEdit::OnMove(int x, int y) 
{
	CEdit::OnMove(x, y);
	
	// TODO: Add your message handler code here
	
}
