/////////////////////////////////////////////////////////////////////////////////////////////
//
// CDefinst.h: lists non-exported functions and variables for Definstaller Functionality
//
/////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
//
// REVISION HISTORY:
//
// Revision 1: Feb 27, 2001
//		Initial Revision - COLLIN DAVIS
//
/////////////////////////////////////////////////////////////////////////////////////////////


#ifndef CDefinst_H
#define CDefinst_H

#include "stdafx.h"

typedef struct	_DefAppInfo
{
	TCHAR	szAppPath[MAX_PATH*2];
	TCHAR	szAppID[MAX_PATH*2];
} DefAppInfo;

typedef std::list<DefAppInfo> DefAppInfoList;

class CDefinst
{

//
//PUBLIC SECTION
//
public:

	////////////////////////////////
	//
	// Public Functions
	//
	////////////////////////////////
	CDefinst();
	UINT Inst(MSIHANDLE hInstall);
	UINT UnInst(MSIHANDLE hInstall);


//
//PRIVATE SECTION
//
private:

	LPTSTR SharedDefs;
	TCHAR* cstrLatestDefsPath;
	TCHAR* cstrTestFile;


	/////////////////////////////////////////////////
	//
	// Private variables
	//
	/////////////////////////////////////////////////
	DefAppInfoList m_InstApps;
	DefAppInfoList::iterator m_iInstApps;
	

	/////////////////////////////////////////////////
	//
	// Private Functions
	//
	/////////////////////////////////////////////////
	BOOL CopyDefs(TCHAR* szSrcPath, TCHAR *szTempPath);
	BOOL CopyDefs(TCHAR *szTempPath, TCHAR** file);
	BOOL ParseProperties(MSIHANDLE hInstall);
	BOOL InsertInstAppInfo(TCHAR* szAppID, TCHAR* szAppPath);
};

#endif