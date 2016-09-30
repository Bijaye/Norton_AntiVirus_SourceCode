// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
enum enumRunRetCodes { NoError = 0, Error_CantCreateProcess = -1, Error_TimeOut = -2 };  //  TNN - Added additional enum type.


class CProgramRobot 
{
public:
	BOOL mSendCancel;
	CProgramRobot(LPCTSTR programName, DWORD timeout, BOOL SendCancel);
	virtual ~CProgramRobot();
	virtual enumRunRetCodes RunProgram();
	
	DWORD	GetTimeout() { return fTimeout; }
	LPCTSTR	GetProgramName() { return fProgramName; }

	DWORD	WaitForInput();
	virtual void	Affirmation();  //This routine attempts to press the OK/Yes or other affirmative button for the dialog
	
	LPTSTR				fProgramName;
	DWORD				fTimeout;
	PROCESS_INFORMATION fProcessInfo;
	STARTUPINFO			fStartupInfo;
};

class CSpecialProgramRobot : public CProgramRobot 
{
public:
	CString mHoldWinName;
	CString mOurFileName;
	CString mAVUKey;
	BOOL mIsNT;
	DWORD				fTimeout;
	CSpecialProgramRobot(LPCTSTR programName, DWORD timeout, CString OurFileName, CString AVUKey, BOOL IsNT, CString HoldWinName);
	virtual enumRunRetCodes RunProgram();
	virtual void	SpecialAffirmation();  //This routine attempts to press the OK/Yes or other affirmative button for the dialog
};
