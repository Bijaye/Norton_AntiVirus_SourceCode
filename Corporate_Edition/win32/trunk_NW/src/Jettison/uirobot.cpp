/****************************************************************
UIRobot.Cpp
A component of the MacNuke Project
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
Copyright 1997, 2005 (c) Symantec Corp.. All rights reserved.
SYMANTEC CONFIDENTIAL

Author:     Richard Sadowsky
Created:    December 8, 1997

Purpose:
Contains function to press buttons in uninstall user interface
to facilitate unattended operation.
//
//  Revision History
//
//  Who                     When        What / Why
//
//  Richard Sadowsky        12/08/97    Created
//  Tedn@Thuridion.com      12/15/03    Replace int return code for RunProgram 
//                                          with enumerated return type.
//                                      Removed casting of return type from DWORD
//                                          to BOOL so calling program can determine
//                                          cause of error.
//                                      Added a Sleep time period to RunProgram if
//                                          WaitForInput fails.  This is to give it
//                                          a "chance" of working for wait failures
****************************************************************/
//#include <windows.h>
#include "stdafx.h"
#include "uirobot.h"
#include "errorlog.h"
#include "regkey.h"
#include "SymSaferStrings.h"

/*  Not Used.
int AutomateProgram(LPCTSTR programName)
{
    const DWORD waitTimeOut = 1000L * 60L * 30L; //half hour

    CProgramRobot   robot(programName, waitTimeOut, FALSE);

    int ret = robot.RunProgram();


    return ret;
}
*/

CProgramRobot::CProgramRobot(LPCTSTR programName, DWORD timeout, BOOL SendCancel)
{
    fProgramName = new TCHAR[lstrlen(programName) + 1];
    lstrcpy(fProgramName, programName);
    fTimeout = timeout;
    mSendCancel=SendCancel;

    memset (&fProcessInfo, 0, sizeof (fProcessInfo));
    fProcessInfo.hProcess = INVALID_HANDLE_VALUE;
    fProcessInfo.hThread  = INVALID_HANDLE_VALUE;
}

CProgramRobot::~CProgramRobot()
{
    delete [] fProgramName;
    if (fProcessInfo.hProcess != INVALID_HANDLE_VALUE)
    CloseHandle (fProcessInfo.hProcess);
    if (fProcessInfo.hThread != INVALID_HANDLE_VALUE)
    CloseHandle (fProcessInfo.hThread);
}

enumRunRetCodes CProgramRobot::RunProgram()
{
    long count = 0;
    DWORD dwRet;
    BOOL Processing = TRUE;
    //DWORD dwError;

    // To cover cases where the caller calls RunProgram several times...
    if (fProcessInfo.hProcess != INVALID_HANDLE_VALUE)
    {
        CloseHandle (fProcessInfo.hProcess);
        fProcessInfo.hProcess = INVALID_HANDLE_VALUE;
    }
    if (fProcessInfo.hThread != INVALID_HANDLE_VALUE)
    {
        CloseHandle (fProcessInfo.hThread);
        fProcessInfo.hThread = INVALID_HANDLE_VALUE;
    }

    GetStartupInfo(&fStartupInfo);

    const size_t nCmdLineSize=_tcslen(fProgramName)+3;
	TCHAR *szCmdLine= new TCHAR[ nCmdLineSize ];

	// double quote command to prevent possible execution of unexpected program
	if (! _tcschr( fProgramName, _T('\"')))
		sssnprintf( szCmdLine, nCmdLineSize, "\"%s\"", fProgramName );
	else
		strcpy( szCmdLine, fProgramName );

    BOOL boolRet = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &fStartupInfo, &fProcessInfo);

	delete [] szCmdLine;

    if (!boolRet)
        return Error_CantCreateProcess;

    Sleep(5000);  //For Cheyenne NT
    while (Processing)
    {
        dwRet = (WaitForInput());
        switch (dwRet)
        {
            case WAIT_TIMEOUT :
                return Error_TimeOut;
                break;
            case WAIT_FAILED :
                /////dwError = GetLastError();
                /////ShowLastError();  /// debug
                //// return Error_TimeOut;

                //  Wait failed for some reason, so wait the fTimeout period of
                //  time before just exiting and checking to see if the program was
                //  removed.
                Sleep (fTimeout);

                return NoError;  ///  TNN - Not sure why the code wants to return 0 for this condition, but I will leave it for now.
                break;
            case 0 :
                break;
            default :
                return NoError;
        }
        if (!(count % 500))
            Affirmation();

        count++;
    }
    return NoError;
}

//  TNN - changed return code from BOOL to DWORD
DWORD CProgramRobot::WaitForInput()
{
    DWORD dwRet = WaitForInputIdle(fProcessInfo.hProcess, fTimeout);

    return dwRet;
}

void CProgramRobot::Affirmation()
{
    HWND hwnd = GetForegroundWindow();
    PostMessage(hwnd, WM_COMMAND, IDYES, 0);
    PostMessage(hwnd, WM_COMMAND, IDOK, 0);
    if (mSendCancel)
        PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
}


void CSpecialProgramRobot::SpecialAffirmation()
{
    char wintxt[512];
    CString WinText;
    CRegistryKey *RegKey = NULL;
    unsigned long VLen;
    char skey[512];
    int ret;
    BOOL Found = FALSE;

    HWND hwnd = GetForegroundWindow();
    GetWindowText(hwnd, wintxt, 511);
    WinText=wintxt;
    WinText.MakeUpper();

    //const MAX_STR = 512;

    if (WinText.Find(mHoldWinName) > -1)
    {
        while (!Found)
        {
            RegKey = new CRegistryKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", TRUE);
            if (RegKey)
            {
                ret = RegKey->Open();
                if (!ret)
                {
                    //Read Their run key
                    VLen=511;
                    skey[0]=0;
                    if (!RegKey->ReadValue(mAVUKey, skey, &VLen))
                    {
                        if (skey[0])
                        {
                            Found=TRUE;
                            //delete their run key
                            RegKey->DeleteValue(mAVUKey);
                        }
                    }
                }
                delete RegKey;
            }
            if (!Found)
            {
                RegKey = new CRegistryKey(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows", TRUE);
                if (RegKey)
                {
                    ret = RegKey->Open();
                    if (!ret)
                    {
                        //Read Their run key
                        VLen=511;
                        skey[0]=0;
                        if (!RegKey->ReadValue(mAVUKey, skey, &VLen))
                        {
                            if (skey[0])
                            {
                                Found=TRUE;
                                //delete their run key
                                RegKey->DeleteValue(mAVUKey);
                            }
                        }
                    }
                    delete RegKey;
                }
            }
            PostMessage(hwnd, WM_COMMAND, IDYES, 0);   //!!test
            PostMessage(hwnd, WM_COMMAND, IDOK, 0);    //!!test
        }
        if (Found)
        {
            if (skey[0])
            {
                //write their run key to our Nuke
                RegKey = new CRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nuke", TRUE);
                if (RegKey)
                {
                    ret = RegKey->Open();
                    if (!ret)
                        RegKey->WriteValue("SecondRunProgramName", (LPTSTR)skey);

                    delete RegKey;
                }
            }
        }
    }

    PostMessage(hwnd, WM_COMMAND, IDYES, 0);
    PostMessage(hwnd, WM_COMMAND, IDOK, 0);
}


CSpecialProgramRobot::CSpecialProgramRobot(LPCTSTR programName, DWORD timeout, CString OurFileName, CString AVUKey, BOOL IsNT, CString HoldWinName) :
     CProgramRobot(programName, timeout, FALSE)
{
    mOurFileName = OurFileName;
    mAVUKey = AVUKey;
    mIsNT = IsNT;
    mHoldWinName=HoldWinName;
    mHoldWinName.MakeUpper();
    fTimeout = timeout;
}

enumRunRetCodes CSpecialProgramRobot::RunProgram()
{
  long count = 0;
    BOOL Processing = TRUE;
    DWORD dwRet;

    // To cover cases where the caller calls RunProgram several times...
    if (fProcessInfo.hProcess != INVALID_HANDLE_VALUE)
    {
        CloseHandle (fProcessInfo.hProcess);
        fProcessInfo.hProcess = INVALID_HANDLE_VALUE;
    }
    if (fProcessInfo.hThread != INVALID_HANDLE_VALUE)
    {
        CloseHandle (fProcessInfo.hThread);
        fProcessInfo.hThread = INVALID_HANDLE_VALUE;
    }

    GetStartupInfo(&fStartupInfo);

    const size_t nCmdLineSize=_tcslen(fProgramName)+3;
	TCHAR *szCmdLine= new TCHAR[ nCmdLineSize ];

	if (! _tcschr( fProgramName, _T('\"')))
		sssnprintf( szCmdLine, nCmdLineSize, "\"%s\"", fProgramName );
	else
		strcpy( szCmdLine, fProgramName );
	
	BOOL boolRet = CreateProcess(NULL, fProgramName, NULL, NULL, FALSE, 0, NULL, NULL, &fStartupInfo, &fProcessInfo);

	delete [] szCmdLine;

    if (!boolRet)
        return Error_CantCreateProcess;
    Sleep(5000);  //For Cheyenne NT
    while (Processing)
    {
        dwRet = (WaitForInput());
        switch (dwRet)
        {
        case WAIT_TIMEOUT :
            return Error_TimeOut;
            break;

        case WAIT_FAILED :
            /////dwError = GetLastError();
            /////ShowLastError();  /// debug
            //// return Error_TimeOut;

            //  Wait failed for some reason, so wait the fTimeout period of
            //  time before just exiting and checking to see if the program was
            //  removed.
            Sleep (fTimeout);

            return NoError;  ///  TNN - Not sure why the code wants to return 0 for this condition, but I will leave it for now.
            break;

        case 0 :
            break;
        default :
            return NoError;
        }

        if (!(count % 500))
            SpecialAffirmation();

        count++;
    }
    return NoError;
}
