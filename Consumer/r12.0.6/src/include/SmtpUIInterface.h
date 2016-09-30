
////////////////////////////////////////////////////////////////////////////////
//
// SMTP UI Interface
//

#ifndef SMTP_UI_INTERFACE_H
#define SMTP_UI_INTERFACE_H

struct ISmtpUIInterface
{
	// Display the SMTP UI dialog.
	virtual bool ShowSmtpDlg(HWND hWndParent) = 0;

	// Dispaly an error message.
	virtual bool SetErrorMessage(const char* szErrMsg) = 0;

	// Total number of email messages.
	virtual bool SetTotal(int iTotal) = 0;

	// Current email message.
	virtual bool SetCurrent(int iCurrent) = 0;

	// Current progress.
	virtual bool SetPercent(int iPercent) = 0;

	// Let Smtp UI know that all messages have been sent.
	virtual bool SendComplete() = 0;
	
	// Delete Smtp UI object.
	virtual void DeleteObject() = 0;
};

struct ISmtpUISink
{
	// Let our caller know that the Smtp UI is ready.
	virtual void OnSmtpUIReady() = 0;
};

extern "C"
{
	// Create a Smtp UI object
	bool WINAPI CreateSmtpUI(/* [in] */ ISmtpUISink* pSink, /* [out] */ ISmtpUIInterface** ppSMTPProgress);
	typedef bool (WINAPI * pfnCREATESMTPUI)(ISmtpUISink*, ISmtpUIInterface**);
}

#endif

