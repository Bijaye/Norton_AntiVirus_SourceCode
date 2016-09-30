// SMTPProcess.h: interface for the SMTPProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMTPPROCESS_H__914065E6_D216_41D4_90C8_A12E379C77B3__INCLUDED_)
#define AFX_SMTPPROCESS_H__914065E6_D216_41D4_90C8_A12E379C77B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SMTPProcess: public ISmtpUISink
{
public:
	SMTPProcess(int seconds, int messages);
	virtual ~SMTPProcess();
	void Process();


	friend UINT SendMail(LPVOID pParam);
	int m_iSeconds;
	int m_iMessages;
	ISmtpUIInterface* m_pSmtpUI;
	HANDLE m_hUIEvent;

// ISmtpUISink Method
	virtual void OnSmtpUIReady();
};



#endif // !defined(AFX_SMTPPROCESS_H__914065E6_D216_41D4_90C8_A12E379C77B3__INCLUDED_)
