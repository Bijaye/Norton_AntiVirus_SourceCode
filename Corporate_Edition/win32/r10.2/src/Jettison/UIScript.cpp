// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// UIScript.cpp: implementation of the UIScript class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S32UI.h"
#include "UIScript.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UIScript::UIScript()
{
  pFile1   = new CStdioFile();
	UninstallCommandLine="";
}

UIScript::~UIScript()
{
	if (pFile1) {
		delete pFile1;
    pFile1 = NULL;
	}
}

int UIScript::OpenFile(CString FileName1, BOOL create)
{
	if (create) {
		if (!pFile1->Open(FileName1, CFile::modeCreate | CFile::modeWrite, &Err)) {
			if (!Err.m_cause) {
				return 0;
			} else {
				return -1;
			}
		}
	} else {
		if (!pFile1->Open(FileName1, CFile::modeRead, &Err)) {
			if (!Err.m_cause) {
				return 0;
			} else {
				return -1;
			}
		}
	}
	return 0;
}

int UIScript::CloseFile()
{
	if (pFile1) {
		pFile1->Close();
	}
  return 0;
}


int UIScript::WriteString(CString Str)
{
	Str=Str+"\n";
	pFile1->WriteString(Str);
  return 0;
}


int UIScript::ReadString()
{
	if (pFile1->ReadString(FileString)) {
    return 0;
	} else {
		return -1;
	}
}

int UIScript::RenameFile(CString OldFile, CString NewFile)
{
	CFileException e;

	TRY{    
		pFile1->Rename(OldFile, NewFile);
	}
	CATCH( CFileException, e ){
		return -1;

	}END_CATCH

  return 0;
}

int UIScript::RemoveFile(CString FileName)
{
	CFileException e;

	TRY{    
		pFile1->Remove(FileName);
	}
	CATCH( CFileException, e ){
		return -1;

	}END_CATCH

  return 0;
}
