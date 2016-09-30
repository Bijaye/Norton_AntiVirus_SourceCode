// SMTPProcess.cpp: implementation of the SMTPProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hammertime.h"
#include "SmtpUIInterface.h"
#include "SMTPProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SMTPProcess::SMTPProcess(int seconds, int messages)
{
	m_hUIEvent = NULL;
	m_iSeconds = seconds;
	m_iMessages = messages;
}

SMTPProcess::~SMTPProcess()
{

}

void SMTPProcess::Process()
{
	m_hUIEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (CreateSmtpUI(this, &m_pSmtpUI) == true)
	{
		CWinThread* pThread = AfxBeginThread(SendMail, (LPVOID)this);
		HANDLE hThread = pThread->m_hThread;
		m_pSmtpUI->SetTotal(m_iMessages);
		m_pSmtpUI->ShowSmtpDlg(NULL);
		WaitForSingleObject( hThread, INFINITE);
		if (m_hUIEvent != NULL)
		{
			CloseHandle(m_hUIEvent);
			m_hUIEvent = NULL;
		}
		m_pSmtpUI->DeleteObject();
		m_pSmtpUI = NULL;
	}
}

void SMTPProcess::OnSmtpUIReady()
{
	SetEvent(m_hUIEvent);
}

UINT SendMail(LPVOID pParam)
{
	SMTPProcess* pThis = (SMTPProcess*)pParam;
	WaitForSingleObject(pThis->m_hUIEvent, INFINITE);

	int j =1;
	pThis->m_pSmtpUI->SetCurrent(j);
	int iSleepIntervalMs = (1000*(pThis->m_iSeconds)*(pThis->m_iMessages)) / 100;
	int interval = 100/(pThis->m_iMessages);
	for(int i=0; i < 100; i++)
	{
		if(i > (interval*j))
			j++;
		pThis->m_pSmtpUI->SetCurrent(j);
		pThis->m_pSmtpUI->SetPercent(i);
		Sleep(iSleepIntervalMs);
	}
	
	pThis->m_pSmtpUI->SendComplete();
	return 1;
}