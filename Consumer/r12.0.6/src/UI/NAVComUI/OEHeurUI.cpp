// OEHeurUI.cpp: implementation of the COEHeurUI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OEHeurUI.h"
#include "OEHUIInterface.h"
#include "OEHeurUIDlg.h"
#include "QuarantinedDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COEHeurUI::COEHeurUI()
{

}

COEHeurUI::~COEHeurUI()
{

}

void COEHeurUI::SetProcessName(const char* szProcessName)
{
	m_strProcessName = szProcessName;
}

void COEHeurUI::SetEmailInfo(const char* szSubject,
							 const char* szSender, 
							 const char* szRecipient)
{
	// Save the email info.
	
	m_strSubject = szSubject;
	m_strSender = szSender;
	m_strRecipient = szRecipient;
}

bool COEHeurUI::Show(OEHUI_TYPE UIType, OEHACTION* pAction)
{
	TCHAR szAlertText[128] = {0};

	if (UIType == OEHUI_TYPE_INITIAL)
	{
		COEHeurUIDlg dlg;

		dlg.SetProcessName(m_strProcessName.c_str());
		dlg.SetEmailInfo(m_strSubject.c_str(), m_strSender.c_str(), 
			m_strRecipient.c_str());
		dlg.DoModal();
		*pAction = dlg.GetUserAction();
	}
	else if (UIType == OEHUI_TYPE_QUARANTINE_FAILED)
	{
		COEHeurUIDlg dlg;

		dlg.SetProcessName(m_strProcessName.c_str());
		dlg.SetEmailInfo(m_strSubject.c_str(), m_strSender.c_str(), 
			m_strRecipient.c_str());
		dlg.SetActionQuar(false);
		dlg.DoModal();
		*pAction = dlg.GetUserAction();
	}
	else if (UIType == OEHUI_TYPE_QUARANTINE_SUCCEEDED)
	{
		CQuarantinedDlg dlg;

		dlg.DoModal();
		*pAction = dlg.GetUserAction();
	}

	return true;
}

void COEHeurUI::DeleteObject()
{
	// Call Release and let SymInterface destroy this object
	Release();
}
