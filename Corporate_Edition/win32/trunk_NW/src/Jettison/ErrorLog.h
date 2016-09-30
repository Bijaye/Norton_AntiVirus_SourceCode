// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ErrorLog.h: interface for the ErrorLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORLOG_H__C862BB23_EB7A_11D1_A770_0000E8D3EFD6__INCLUDED_)
#define AFX_ERRORLOG_H__C862BB23_EB7A_11D1_A770_0000E8D3EFD6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _DEBUG
    void ShowLastError ();
#endif

#define ERRORTOKEN                            -1
#define COMMENTTOKEN                           1  //; in the line
#define SETREGISTRYTOKEN                       2  //setregistry=key,key,key,value
#define CLEARREGISTRYTOKEN                     3  //clearregistry=key,key,key
#define READREGISTRYTOKEN                      4  //readregistry=key,key,key
#define RESTARTCOMPUTERTOKEN                   5  //restartcomputer
#define RUNPROGRAMTOKEN                        6  //runprogram=key,key,key
#define GETUNINSTALLPROGRAMNAMETOKEN           7  //getuninstallprogramname=key,key,key
#define SILENTTOKEN                            8  //silent
#define RUNROBOTTOKEN                          9  //runrobot[=seconds] (optional num seconds interact)
#define GETUNINSTALLSCRIPTPATHTOKEN            10 //getuninstallscriptpath=key,key,key
#define ADDPROGRAMPARAMETERSTOKEN              11 //addprogramparameters=params
#define PAUSETOKEN                             12 //pause=seconds
#define SENDWINDOWSMESSAGETOKEN                13 //sendwindowsmessage=windowname,parameter
#define SETWINDOWSUNINSTALLPROGRAMTOKEN        14 //setwindowuninstallprogram=filename
#define REMOVEFROMAUTOEXECTOKEN                15 //removefromautoexec=searchstring
#define GETUNINSTALLPROGRAMPATHTOKEN           16 //getuninstallprogrampath=key,key,key
#define SETUNINSTALLSCRIPTNAMETOKEN            17 //setuninstallscriptname=filename
#define SETUNINSTALLPROGRAMNAMETOKEN           18 //setuninstallprogramname=filename
#define PAUSEFORNOTIFICATIONOFCOMPLETION       19 //set registry fro 2nd run and pop messagebox
//#define PAUSEFORNOTIFICATIONOFCOMPLETION       20 //If not running NT
//#define RESTARTCOMPUTERTOKEN                   21 //If not running NT
#define DEBUGTOKEN                             22 //debug=on/off
#define GETUNINSTALLSHIELDPROGRAMNAMETOKEN     23 //getuninstallprogramname=key,key,key
#define SETREGISTRYFORUNINSTALLSECONDRUNTOKEN  24 //setregistryforuninstallsecondrun
#define RUNROBOTSETRUNREGISTRYTOKEN            25 //runrobotsetrunregistry
#define SETUPFORSECONDRUNTOKEN                 26 //setupforsecondrun
#define PLATFORMTOKEN                          27 //platform
#define STOPPROGRAMFORREBOOTTOKEN              28 //stopprogramforreboot
#define SENDWINDOWSMESSAGEANDWAITTOKEN         29 //sendwindowsmessageandwait=windowname,msg, Waittime(sec)
#define RUNROBOTSENDCANCELTOKEN                30 //runrobotsendcancel
#define SENDAFFIRMATIVETOWINDOWTOKEN           31 //sendaffirmativetowindow=windowname,waittime
#define SENDAFFIRMATIVETOWINDOWAFTERENABLETOKEN 32 //sendaffirmativetowindowafterenable=windowname,buttonname,waittime
#define GETUNINSTALLPROGRAMNAMESTUFFPASSWORDTOKEN  33  //getuninstallprogramnamestuffpassord=key,key,key
#define SETUNINSTALLPROGRAMIFREGISTRYEXISTSTOKEN  34  //setuninstallprogramifregistryexists=uninstallpgmname,key,key,key
#define SENDAFFIRMATIVETOWINDOWWITHBUTTONCLICK 35   // sendaffirmativetowindowwithButtonClick=windowname,buttonname,waittime
#define SETINIENTRY							   36	// SetINIEntry
#define SENDTEXTTOEDITWINDOW                   37   // SendTextToEditWindow

#define DESC2                                  "setregistry="
#define DESC3                                  "clearregistry="
#define DESC4                                  "readregistry="
#define DESC5                                  "restartcomputer"
#define DESC6                                  "runprogram="
#define DESC7                                  "getuninstallprogramname="
#define DESC8                                  "silent"
#define DESC9                                  "runrobot="
#define DESC10                                 "getuninstallscriptpath="
#define DESC11                                 "addprogramparameters="
#define DESC12                                 "pause="
#define DESC13                                 "sendwindowsmessage="
#define DESC14                                 "setwindowuninstallprogram="
#define DESC15                                 "removefromautoexec="
#define DESC16                                 "getuninstallprogrampath="
#define DESC17                                 "setuninstallscriptname="
#define DESC18                                 "setuninstallprogramname="
#define DESC19                                 "pausefornotificationofcompletion"
#define DESC20                                 "pausefornotificationofcompletion"
#define DESC21                                 "restartcomputer"
#define DESC23                                 "getuninstallshieldprogramname="
#define DESC24                                 "setregistryforuninstallsecondrun"
#define DESC25                                 "runrobotsetrunregistry="
#define DESC26                                 "setupforsecondrun="
#define DESC27                                 "platform="
#define DESC28                                 "stopprogramforreboot"
#define DESC29                                 "sendwindowsmessageandwait"
#define DESC30                                 "runrobotsendcancel"
#define DESC31                                 "sendaffirmativetowindow"
#define DESC32                                 "sendaffirmativetowindowafterenable"
#define DESC33                                 "getuninstallprogramnamestuffpassword="
#define DESC34                                 "setuninstallprogramifregistryexists="

#define PARAMS2                                4
#define PARAMS3                                3
#define PARAMS4                                3
#define PARAMS5                                0
#define PARAMS6                                3
#define PARAMS7                                3
#define PARAMS8                                0
#define PARAMS9                                104  //0 or 1 used 104 to signify this
#define PARAMS10                               3
#define PARAMS11                               1
#define PARAMS12                               1
#define PARAMS13                               2
#define PARAMS14                               1
#define PARAMS15                               1
#define PARAMS16                               3
#define PARAMS17                               1
#define PARAMS18                               1
#define PARAMS19                               0
#define PARAMS23                               3
#define PARAMS24                               0
#define PARAMS25                               105  //2 or 3 used 105 to signify this
#define PARAMS26                               1
#define PARAMS27                               1  //choices are Win95, Win98, WinNT
#define PARAMS28                               0
#define PARAMS29                               3
#define PARAMS30                               104  //0 or 1 used 104 to signify this
#define PARAMS31                               2
#define PARAMS32                               3
#define PARAMS33                               3

#define MaxString = 512;

class ErrorLog
{
public:
	void DeleteErrorLog();
	int WriteString(CString S);
	int WriteErrorToLog(int Token, int LineNum);
	int WriteTitleToLog (CString S);
	void WriteError(CString ErrorStr, BOOL bWriteOtherInfo = TRUE);
	void WriteErrorSuccess(CString ErrStr);
	void WriteError(const int StrId);
	void WriteErrorSuccess(const int StrId);
	ErrorLog(LPCSTR Path, LPCSTR nukefilename = NULL);
	virtual ~ErrorLog();
	int OpenFile(BOOL NewFile);
	int CloseFile();

	CStdioFile *pErrorFile;
	CFileException Err;
	CString ErrorFileName;
	CString NukeFileName;
};

#endif // !defined(AFX_ERRORLOG_H__C862BB23_EB7A_11D1_A770_0000E8D3EFD6__INCLUDED_)
