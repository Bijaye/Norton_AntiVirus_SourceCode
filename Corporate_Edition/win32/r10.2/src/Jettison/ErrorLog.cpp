// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ErrorLog.cpp: implementation of the ErrorLog class.
//
//
//
//
//  Revision History
//
//  Who                     When        What / Why
//
//  Tedn@Thuridion.com      12/11/03    Added Trace Statements and ShowLastError function
//                                      as debugging aids.
//  Tedn@Thuridion.com      12/11/03    Modified ErrorLog function to eliminate unnecessary statu
//                                      lines.
//                                      as debugging aids.
//  Tedn@Thuridion.com NN   12/17/03    Added WriteTitleToLog function for sending script file
//                                      header/Title to the Status.ini file. 
//  Tedn@Thuridion.com NN   12/19/03    Updated WriteToErrorLog to handle syntax errors
//										for new script commands.
//  Tedn@Thuridion.com NN   12/22/03    Commented param in ErrorLog:OpenFile() to prevent Compiler warning.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S32UI.h"
#include "ErrorLog.h"
#include "trace.h"  //  Add Trace Statement for debugging


extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow (); 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ErrorLog::ErrorLog(LPCSTR Path, LPCSTR nukefilename)
{
  int pos;

  pErrorFile   = new CStdioFile();

  ErrorFileName=Path;
  ErrorFileName += "\\STATUS.INI";
  if (nukefilename) {
    NukeFileName = nukefilename;
    //full path is no longer desired in the status.ini //!!j 02-04
    pos = NukeFileName.ReverseFind('\\');
    if (pos != -1) {
      NukeFileName = NukeFileName.Right(NukeFileName.GetLength() - (pos+1));
    }
  } else {
    NukeFileName = "";
  }
}

ErrorLog::~ErrorLog()
{
	if (pErrorFile) {
		CloseFile();
		delete pErrorFile;
		pErrorFile = NULL;
	}

}

int ErrorLog::OpenFile(BOOL /*NewFile*/)
{
//!!j	was used during development->    NewFile=TRUE;

	//=========================================================
	// AKENNED - Checking to see if status file already exists
	//           if it does, just appened to it.
	BOOL bCreateNewFile = FALSE;
	CFileStatus fs;
	if (!CFile::GetStatus( ErrorFileName, fs ) ) {
		bCreateNewFile = TRUE;
	}

	if ( bCreateNewFile /*NewFile*/) {
		if (!pErrorFile->Open(ErrorFileName, CFile::typeText | CFile::modeCreate | CFile::modeWrite, &Err)) {
			if (Err.m_cause != CFileException::fileNotFound) {
				return Err.m_cause;
			} else {				
				return 0;
			}
		}
	} else {
		if (!pErrorFile->Open(ErrorFileName, CFile::typeText | CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, &Err)) {
			if (Err.m_cause != CFileException::fileNotFound) {
				return Err.m_cause;
			} else {
				pErrorFile->Seek(0, CFile::end);
				return 0;
			}
		}
		pErrorFile->Seek(0, CFile::end);
	}	
	return 0;
}

int ErrorLog::CloseFile()
{
	if (pErrorFile) {
        if( pErrorFile->m_pStream != NULL ) //AKENNED
		    pErrorFile->Close();
	}
  return 0;
}

int ErrorLog::WriteString(CString S)
  //This is used only for debug, this should not be called for any other purpose.
{


  ErrorFileName="Debug.log";
  if (!OpenFile(FALSE)) {
    pErrorFile->WriteString(S);
		CloseFile();
  }
  return 0;
}



int ErrorLog::WriteTitleToLog (CString S)
  //This is used only for debug, this should not be called for any other purpose.
{


  if (!OpenFile(FALSE)) {
    pErrorFile->WriteString(S);
		CloseFile();
  }
  return 0;
}


int ErrorLog::WriteErrorToLog(int Token, int LineNum)
{
	CString ErrStr;
  CString FmtStr;

/*  removed so that the file would get created every time
  if (OpenFile(NewFile)) {
		return -1;
	}

	pErrorFile->SeekToEnd();
*/

  if (OpenFile(TRUE)) {
		return -1;
	}

  VERIFY(ErrStr.LoadString(IDS_STR1106));
	pErrorFile->WriteString(ErrStr);
  VERIFY(FmtStr.LoadString(IDS_STR1107));
  ErrStr.Format(FmtStr, 0);
	pErrorFile->WriteString(ErrStr);

	switch (Token) {
		case -1:
      VERIFY(FmtStr.LoadString(IDS_STR1108));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1);
			pErrorFile->WriteString(ErrStr);
			break;
		case 2:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS2);
			pErrorFile->WriteString(ErrStr);
			break;
		case 3:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS3);
			pErrorFile->WriteString(ErrStr);
			break;
		case 4:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS4);
			pErrorFile->WriteString(ErrStr);
			break;
		case 5:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS5);
			pErrorFile->WriteString(ErrStr);
			break;
		case 6:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS6);
			pErrorFile->WriteString(ErrStr);
			break;
		case 7:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS7);
			pErrorFile->WriteString(ErrStr);
			break;
		case 8:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS8);
			pErrorFile->WriteString(ErrStr);
			break;
		case 9:
      VERIFY(FmtStr.LoadString(IDS_STR1111));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1);
			pErrorFile->WriteString(ErrStr);
			break;
		case 10:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS10);
			pErrorFile->WriteString(ErrStr);
			break;
		case 11:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS11);
			pErrorFile->WriteString(ErrStr);
			break;
		case 12:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS12);
			pErrorFile->WriteString(ErrStr);
			break;
		case 13:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS13);
			pErrorFile->WriteString(ErrStr);
			break;
		case 14:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS14);
			pErrorFile->WriteString(ErrStr);
			break;
		case 15:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS15);
			pErrorFile->WriteString(ErrStr);
			break;
		case 16:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS16);
			pErrorFile->WriteString(ErrStr);
			break;
		case 17:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS17);
			pErrorFile->WriteString(ErrStr);
			break;
		case 18:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS18);
			pErrorFile->WriteString(ErrStr);
			break;
		case 19:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS19);
			pErrorFile->WriteString(ErrStr);
			break;
		case 20:
      VERIFY(FmtStr.LoadString(IDS_STR1112));
      ErrStr.Format(FmtStr, NukeFileName);
			pErrorFile->WriteString(ErrStr);
			break;
		case 21:
      VERIFY(FmtStr.LoadString(IDS_STR1112));
      ErrStr.Format(FmtStr, NukeFileName);
			pErrorFile->WriteString(ErrStr);
			break;
		case 23:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS23);
			pErrorFile->WriteString(ErrStr);
			break;
		case 24:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS24);
			pErrorFile->WriteString(ErrStr);
			break;
		case 25:
      VERIFY(FmtStr.LoadString(IDS_STR1113));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS14);
			pErrorFile->WriteString(ErrStr);
			break;
		case 26:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS26);
			pErrorFile->WriteString(ErrStr);
			break;
		case 27:
      VERIFY(FmtStr.LoadString(IDS_STR1114));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS27);
			pErrorFile->WriteString(ErrStr);
			break;
		case 28:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS28);
			pErrorFile->WriteString(ErrStr);
			break;
		case 29:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS29);
			pErrorFile->WriteString(ErrStr);
			break;
    case 30:
      VERIFY(FmtStr.LoadString(IDS_STR1111));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1);
			pErrorFile->WriteString(ErrStr);
			break;
		case 31:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS31);
			pErrorFile->WriteString(ErrStr);
			break;
		case 32:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS32);
			pErrorFile->WriteString(ErrStr);
			break;
		case SENDAFFIRMATIVETOWINDOWWITHBUTTONCLICK:
		case SETINIENTRY:		
		case SENDTEXTTOEDITWINDOW:
      VERIFY(FmtStr.LoadString(IDS_STR1109));
      ErrStr.Format(FmtStr, NukeFileName, LineNum+1, PARAMS32);
			pErrorFile->WriteString(ErrStr);
			break;

	}

    TraceEx("ErrorLog::WriteErrorToLog - %s\n",ErrStr); 

	ErrStr="\n";
	pErrorFile->WriteString(ErrStr);


	if (CloseFile()) {
		return -1;
	}
	return 0;
}

void ErrorLog::DeleteErrorLog()
{
  pErrorFile->Remove(ErrorFileName);
}

void ErrorLog::WriteError(CString ErrStr, BOOL bWriteOtherInfo)
//write the specified string to the error log file.
{
	CString EStr;
	CString EStr2;

	if (!OpenFile(TRUE)) {
		if( TRUE == bWriteOtherInfo ) {
			VERIFY(EStr.LoadString(IDS_STR1106));
			pErrorFile->WriteString(EStr);
			VERIFY(EStr2.LoadString(IDS_STR1107));
			EStr.Format(EStr2, 0);
			pErrorFile->WriteString(EStr);			
		}
		VERIFY(EStr2.LoadString(IDS_BLANKSTR));
		EStr2.TrimRight();
		EStr.Format(EStr2, NukeFileName, ErrStr);
		pErrorFile->WriteString(EStr);
		CloseFile();
	}

}

void ErrorLog::WriteErrorSuccess(CString ErrStr)
//write the specified string to the error log file.
{
	CString EStr;
  CString EStr2;

  if (!OpenFile(TRUE)) {
    VERIFY(EStr.LoadString(IDS_STR1106));
		pErrorFile->WriteString(EStr);
    VERIFY(EStr2.LoadString(IDS_STR1107));
    EStr.Format(EStr2, 1);
		pErrorFile->WriteString(EStr);
    VERIFY(EStr2.LoadString(IDS_STR1116));
    EStr.Format(EStr2, NukeFileName, ErrStr);
		pErrorFile->WriteString(EStr);
		CloseFile();
	}

}

void ErrorLog::WriteError(const int StrId)
//write the specified string to the error log file.
{
	CString EStr;
  CString EStr2;
  CString EStr3;

  if (!OpenFile(TRUE)) {

#if 0  //  TNN - Moved this header to the start of processing of this script file.  Otherwise, when there are 
       //        multiple error messages the header does not appear multiple times (which can be confusing to
       //        anybody reading the Status.ini file)
    VERIFY(EStr.LoadString(IDS_STR1106));
		pErrorFile->WriteString(EStr);
#endif 

#if 0  //  TNN - This is not telling the user anything since it is hard coded to 0 and it just clutters up the status file.
	   //        The reason for the failure is given in the StrID string.
    VERIFY(EStr2.LoadString(IDS_STR1107));
    EStr.Format(EStr2, 0);
	pErrorFile->WriteString(EStr);
#endif
		
    VERIFY(EStr3.LoadString(StrId));   //!!j
    VERIFY(EStr2.LoadString(IDS_STR1115));
    EStr.Format(EStr2, NukeFileName, EStr3);
		pErrorFile->WriteString(EStr);
		CloseFile();

	}

}

void ErrorLog::WriteErrorSuccess(const int StrId)
//write the specified string to the error log file.
{
	CString EStr;
  CString EStr2;
  CString EStr3;

  if (!OpenFile(TRUE)) {
    VERIFY(EStr.LoadString(IDS_STR1106));
		pErrorFile->WriteString(EStr);
    VERIFY(EStr2.LoadString(IDS_STR1107));
    EStr.Format(EStr2, 1);
		pErrorFile->WriteString(EStr);

    VERIFY(EStr3.LoadString(StrId));
    VERIFY(EStr2.LoadString(IDS_STR1116));
    EStr.Format(EStr2, NukeFileName, EStr3);
		pErrorFile->WriteString(EStr);
		CloseFile();
	}

}



//-----------------------

#ifdef _DEBUG
void ShowLastError ()
{  

LPVOID lpMsgBuf;
if (!FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL ))
{
   // Handle the error.
   return;
}

// Process any inserts in lpMsgBuf.
// ...

// Display the string.
MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

// Free the buffer.
LocalFree( lpMsgBuf );
 
}
#endif

