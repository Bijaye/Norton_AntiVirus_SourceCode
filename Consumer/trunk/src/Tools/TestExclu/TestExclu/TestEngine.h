////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#define _WIN32_DCOM
#ifdef WIN32
    #pragma warning ( disable : 4250 )
    #pragma warning( disable : 4290 )
#endif
#include "stdafx.h"
#include "ccLibStd.h"


class TestEngine
{
public:

	TestEngine(void);
	~TestEngine(void);
	bool Addfs(char* strPath , time_t tTime, int iState, bool bIsOem);
	bool Adda(ULONG ulVid, char* cGuid, time_t tTime, char* cName, int iState);
	bool Findfs(char* strPath, bool isOem);
	bool Finda(ULONG ulVid, char* cGuid);
	bool Get(ULONG tempId);
	bool Isfs(char* strPath, bool isOem, int iState);
	bool Isa(ULONG ulVid, char* cGuid, int iState);
	bool Remove(ULONG tempId);
	bool List();
	bool ReplaceFS(ULONG tempId, char* strPath, time_t tTime, int iState);
	bool ReplaceA(ULONG tempId, ULONG ulVid, char* cGuid, time_t tTime, char* cName, int iState);
	bool Reload();
	bool Save();
	bool ToFile(char* cFilePath);
	bool FromFile(char* cFilePath, int iState, int iType);
	bool SetAuto(bool isAuto );
	bool IsAuto();
	bool SetStateFlag(ULONG tempId, int iState, bool bFlag );
	bool Default(int iState, int iType);
	bool ViewSettings();


	int GetType(ULONG tempId);
	bool ReadFromFile(CString strInputFile);
	bool Process();


	

	
};
