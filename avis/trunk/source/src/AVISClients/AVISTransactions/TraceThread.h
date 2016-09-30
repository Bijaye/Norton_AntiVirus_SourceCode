// TraceThread.h: interface for the CTraceThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACETHREAD_H__B27882DA_D008_11D2_8A94_00203529AC86__INCLUDED_)
#define AFX_TRACETHREAD_H__B27882DA_D008_11D2_8A94_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TraceClient.h"
#include "tracesinkfile.h"
#include "MTrace.h"

class CTraceThread  
{
private:
	TraceSinkFile m_tsfLog;
	TraceSinkFile m_tsfTrace;
	MTrace m_mtLog;
	MTrace m_mtTrace;
	TraceClient m_tcLog;
	TraceClient m_tcTrace;

public:
	CTraceThread(
		LPCSTR pszLogFileName,
		LPCSTR pszTraceFileName,
		LPCSTR pszTraceLogAgentName);
	virtual ~CTraceThread();

public:
	TraceClient& GetLog();
	TraceClient& GetTrace();
	MTrace& GetMTraceForTrace();
};

#endif // !defined(AFX_TRACETHREAD_H__B27882DA_D008_11D2_8A94_00203529AC86__INCLUDED_)
