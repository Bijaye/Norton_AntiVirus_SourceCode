// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//-----------------------------------------------------------------------
//
//  CWindowMessage
//
//
//
//  Revision History
//
//  Who                     When        What / Why
//
//  Tedn@Thuridion.com      12/12/03    Added Trace Statements for debugging aid.
//                                      Created new functions 
//                                          CWindowMessageAffirm:SendButtonClick
//                                          CWindowMessageAffirm:SendEditText
//
//
//------------------------------------------------------------------------
#include "stdafx.h"
#include "wndmsg.h"
#include "errorlog.h"
#include "trace.h"
#include "SymSaferStrings.h"

CWindowMessage::CWindowMessage(LPCTSTR className, LPCTSTR windowName)
{
    fWindow = FindWindowEx(NULL, NULL, className, windowName);
}

CWindowMessage::~CWindowMessage()
{
}

BOOL CWindowMessage::Send(UINT message, WPARAM wparam, LPARAM lparam)
{
    BOOL ret = FALSE;
    if (fWindow != NULL)
    {
        SendMessage(fWindow, message, wparam, lparam);
        ret = TRUE;
    }
    return ret;
}

//-----------------------------------------------------------------------------------

CWindowMessageAndWait::CWindowMessageAndWait(LPCTSTR className, LPCTSTR windowName, int duration)
//duration is in seconds, loop until window is found or time expires
{
    WinName=windowName;
    ClassName=className;
    WaitTime=duration;

}

CWindowMessageAndWait::~CWindowMessageAndWait()
{
}

BOOL CWindowMessageAndWait::Send(UINT message, WPARAM wparam, LPARAM lparam)
{
    BOOL ret = FALSE;
    BOOL NotDone = TRUE;
    CTime CurTime;
    CTime EndTime = CTime::GetCurrentTime();


    EndTime += CTimeSpan( 0, 0, 0, WaitTime ); 
    //loop until the window is found or the time expires.
    while (NotDone)
    {
        if (ClassName.GetLength() == 0)
        {
            fWindow = FindWindowEx(NULL, NULL, NULL, WinName);
        } else
        {
            fWindow = FindWindowEx(NULL, NULL, ClassName, WinName);
        }
        if (fWindow != NULL)
        {
            SendMessage(fWindow, message, wparam, lparam);
            break;
            ret = TRUE;
        }
        CurTime = CTime::GetCurrentTime();
        if (CurTime > EndTime)
        {
            break;
        }
    }

    return ret;
}

//-----------------------------------------------------------------------------------

CWindowMessageAffirm::CWindowMessageAffirm(LPCTSTR className, LPCTSTR windowName, int duration)
//duration is in seconds, loop until window is found or time expires
{
    WinName=windowName;
    ClassName=className;
    WaitTime=duration;

}

CWindowMessageAffirm::~CWindowMessageAffirm()
{
}

BOOL CWindowMessageAffirm::Send()
{
    BOOL ret = FALSE;
    BOOL Done = FALSE;
    CTime CurTime;

    CTime EndTime = CTime::GetCurrentTime();
    EndTime += CTimeSpan( 0, 0, 0, WaitTime ); 
    //loop until the window is found or the time expires.

    Sleep(1000);//This was put in for Cheyenne we were posting the message before the 
    //window was ready to receive it

    while (!Done)
    {
        if (ClassName.GetLength() == 0)
        {
            fWindow = FindWindowEx(NULL, NULL, NULL, WinName);
        } else
        {
            fWindow = FindWindowEx(NULL, NULL, ClassName, WinName);
        }
        if (fWindow != NULL)
        {
            ret = TRUE;
            SendMessage(fWindow, WM_COMMAND, IDYES, 0);
            SendMessage(fWindow, WM_COMMAND, IDOK, 0);
            TraceEx("Message sent to window %s\n",WinName);

            while (IsWindow(fWindow))
            {
                CurTime = CTime::GetCurrentTime();
                if (CurTime > EndTime)
                {
                    ret=FALSE;
                    break;
                }
            }
        }
        break;
        CurTime = CTime::GetCurrentTime();
        if (CurTime > EndTime)
        {
            ret = FALSE;
            break;
        }
    }
    return ret;
}


BOOL CWindowMessageAffirm::SendAfterEnable(CString ButtonName)
{
    BOOL ret = FALSE;
    BOOL NotDone = TRUE;
    BOOL Done = FALSE;
    HWND            fButton = NULL;
    CTime CurTime;

    CTime EndTime = CTime::GetCurrentTime();
    EndTime += CTimeSpan( 0, 0, 0, WaitTime ); 
    //loop until the window is found or the time expires.

    Sleep(1000);//This was put in for Cheyenne we were posting the message before the 
    //window was ready to receive it

    while (NotDone)
    {
        if (ClassName.GetLength() == 0)
        {
            fWindow = FindWindowEx(NULL, NULL, NULL, WinName);
        } else
        {
            fWindow = FindWindowEx(NULL, NULL, ClassName, WinName);
        }
        if (fWindow != NULL)
        {
            fButton = FindWindowEx(fWindow, NULL, NULL, ButtonName);
            if (fButton != NULL)
            {
                NotDone=FALSE;
                while (!Done)
                {
                    if (IsWindowEnabled(fButton))
                    {
                        Done=TRUE;
                        ret = TRUE;
                        SendMessage(fWindow, WM_COMMAND, IDYES, 0);
                        SendMessage(fWindow, WM_COMMAND, IDOK, 0);
                    }
                    CurTime = CTime::GetCurrentTime();
                    if (CurTime > EndTime)
                    {
                        ret=FALSE;
                        break;
                    }
                } 

                while (IsWindow(fWindow))
                {
                    CurTime = CTime::GetCurrentTime();
                    if (CurTime > EndTime)
                    {
                        ret=FALSE;
                        break;
                    }
                }
            } else
            {
                ret = FALSE;
            }
            break;
        }
        CurTime = CTime::GetCurrentTime();
        if (CurTime > EndTime)
        {
            ret = FALSE;
            break;
        }
    }
    return ret;
}



BOOL CWindowMessageAffirm::SendButtonClick (CString ButtonName)
{
    BOOL ret = FALSE;
    BOOL NotDone = TRUE;
    BOOL Done = FALSE;
    HWND            fButton = NULL;
    CTime CurTime;

    CTime EndTime = CTime::GetCurrentTime();
    EndTime += CTimeSpan( 0, 0, 0, WaitTime ); 
    //loop until the window is found or the time expires.

    Sleep(1000);//This was put in for Cheyenne we were posting the message before the 
    //window was ready to receive it

    while (NotDone)
    {
        if (ClassName.GetLength() == 0)
        {
            fWindow = FindWindowEx(NULL, NULL, NULL, WinName);
        } else
        {
            fWindow = FindWindowEx(NULL, NULL, ClassName, WinName);
        }
        if (fWindow != NULL)
        {
            fButton = FindWindowEx(fWindow, NULL, NULL, ButtonName);
            if (fButton != NULL)
            {
                NotDone=FALSE;
                while (!Done)
                {
                    if (IsWindowEnabled(fButton))
                    {
                        Done=TRUE;
                        ret = TRUE;

						//int nCtrlID;
						int nCtrlID = GetDlgCtrlID (fButton);
						//HWND hSee = GetDlgItem (GetParent (fButton), nCtrlID);
                        SendMessage (fWindow, WM_COMMAND, MAKEWPARAM(nCtrlID, BN_CLICKED), (LPARAM)fButton);
                    }
                    CurTime = CTime::GetCurrentTime();
                    if (CurTime > EndTime)
                    {
                        ret=FALSE;
                        break;
                    }
                } 

                while (IsWindow(fWindow))
                {
                    CurTime = CTime::GetCurrentTime();
                    if (CurTime > EndTime)
                    {
                        ret=FALSE;
                        break;
                    }
                }
            } else
            {
                ret = FALSE;
            }
            break;
        }
        CurTime = CTime::GetCurrentTime();
        if (CurTime > EndTime)
        {
            ret = FALSE;
            break;
        }
    }
    return ret;
}


//  TNN
//  Sends a text string to an edit control in a dialog.
//  NOTE:  This will only work for a dialog with a single edit control.
//         For example, it will work with a simple Password prompt.
BOOL CWindowMessageAffirm::SendEditText (CString DialogName, CString strText)
{
    BOOL  ret = FALSE;
    BOOL  NotDone = TRUE;
    BOOL  Done = FALSE;
    HWND  fEdit = NULL;
    CTime CurTime;

    CTime EndTime = CTime::GetCurrentTime();
    EndTime += CTimeSpan( 0, 0, 0, WaitTime ); 
    //loop until the window is found or the time expires.

    Sleep(1000);//This was put in for Cheyenne we were posting the message before the 
    //window was ready to receive it

    while (NotDone)
    {
		//  Get the dialog window
        fWindow = FindWindowEx(NULL, NULL, NULL, WinName);
			
        if (fWindow != NULL)
        {
			//  Get the edit box.
			fEdit = FindWindowEx(fWindow, NULL, "Edit", NULL);    

            if (fEdit != NULL)
            {
                NotDone=FALSE;
                while (!Done)
                {
                    if (IsWindowEnabled(fEdit))
                    {
                        Done=TRUE;
                        ret = TRUE;

						SendMessage (fEdit, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)strText);
						return true;  //  SUCCESS
	                }
                    CurTime = CTime::GetCurrentTime();
                    if (CurTime > EndTime)
                    {
                        ret=FALSE;
                        break;
                    }
                } 

                while (IsWindow(fWindow))
                {
                    CurTime = CTime::GetCurrentTime();
                    if (CurTime > EndTime)
                    {
                        ret=FALSE;
                        break;
                    }
                }
            } else
            {
                ret = FALSE;
            }
            break;
        }
        CurTime = CTime::GetCurrentTime();
        if (CurTime > EndTime)
        {
            ret = FALSE;
            break;
        }
    }
   return ret;
}