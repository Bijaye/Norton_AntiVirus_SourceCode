// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// UIScript.h: interface for the UIScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UISCRIPT_H__509FEA83_F560_11D1_A79A_0000E8D3EFD6__INCLUDED_)
#define AFX_UISCRIPT_H__509FEA83_F560_11D1_A79A_0000E8D3EFD6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class UIScript  
{
public:
	int RemoveFile(CString FileName);
	CString FileString;
	int RenameFile(CString OldFile, CString NewFile);
	CString UninstallCommandLine;
	UIScript();
	virtual ~UIScript();
  CStdioFile *pFile1;
  CFileException Err;
  CString FileName1;

  int OpenFile(CString FileName1, BOOL create);
  int CloseFile();
	int ReadString();
	int WriteString(CString Str);

};

#endif // !defined(AFX_UISCRIPT_H__509FEA83_F560_11D1_A79A_0000E8D3EFD6__INCLUDED_)
