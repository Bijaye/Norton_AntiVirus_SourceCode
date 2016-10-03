// DebugLogs.h: interface for the CDebugLogs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGLOGS_H__ED715C92_FECE_4E28_91F9_8864346E2809__INCLUDED_)
#define AFX_DEBUGLOGS_H__ED715C92_FECE_4E28_91F9_8864346E2809__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Report.h"

class CDebugLogs : public CReport  
{
public:
	CDebugLogs();
	virtual ~CDebugLogs();
    bool CreateReport ( LPCTSTR lpcszFolderPath );
};

#endif // !defined(AFX_DEBUGLOGS_H__ED715C92_FECE_4E28_91F9_8864346E2809__INCLUDED_)
