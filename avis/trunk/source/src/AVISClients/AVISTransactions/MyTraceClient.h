// MyTraceClient.h: interface for the MyTraceClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYTRACECLIENT_H__B27882D9_D008_11D2_8A94_00203529AC86__INCLUDED_)
#define AFX_MYTRACECLIENT_H__B27882D9_D008_11D2_8A94_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "impexp.h"

#include "TraceThread.h"

class IMPEXP MyTraceClient
{
private:
	CMapWordToPtr m_map;
	BOOL m_boTracingEnabled;

public:
	MyTraceClient();
	virtual ~MyTraceClient();

public:
	void RegisterThread(
		LPCSTR pszLogFileName,
		LPCSTR pszTraceFileName,
		LPCSTR pszAgentName,
		DWORD nIdOtherThread = 0);

public:
	void msg(LPCTSTR szFormat, ...);
	void enter(LPCTSTR szFormat, ...);
	void exit(LPCTSTR szFormat, ...);
	void debug(LPCTSTR szFormat, ...);
	void info(LPCTSTR szFormat, ...);
	void warning(LPCTSTR szFormat, ...);
	void error(LPCTSTR szFormat, ...);
	void critical(LPCTSTR szFormat, ...);
	void EnableTracing(BOOL boEnableTracing = TRUE);
};

#endif // !defined(AFX_MYTRACECLIENT_H__B27882D9_D008_11D2_8A94_00203529AC86__INCLUDED_)
