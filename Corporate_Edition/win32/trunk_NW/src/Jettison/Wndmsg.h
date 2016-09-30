// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
class CWindowMessage
{
public:
	CWindowMessage(LPCTSTR className, LPCTSTR windowName);
	~CWindowMessage();

	BOOL Send(UINT message, WPARAM wparam = 0, LPARAM lparam = 0);
	BOOL FoundWindow() { return fWindow != NULL; }

private:
	HWND			fWindow;

};

class CWindowMessageAndWait
{
public:
	int WaitTime;
	CString ClassName;
	CString WinName;
	CWindowMessageAndWait(LPCTSTR className, LPCTSTR windowName, int duration);
	~CWindowMessageAndWait();

	BOOL Send(UINT message, WPARAM wparam = 0, LPARAM lparam = 0);
	BOOL FoundWindow() { return fWindow != NULL; }

private:
	HWND			fWindow;

};

class CWindowMessageAffirm
{
public:
	int WaitTime;
	CString ClassName;
	CString WinName;
	CWindowMessageAffirm(LPCTSTR className, LPCTSTR windowName, int duration);
	~CWindowMessageAffirm();

	BOOL Send();
	BOOL SendAfterEnable(CString ButtonName);
	BOOL SendButtonClick(CString ButtonName);        //  TNN - Added to send clicked message to parent dialog.
	BOOL SendEditText (CString DialogName, CString strText);
	BOOL FoundWindow() { return fWindow != NULL; }

private:
	HWND			fWindow;

};
