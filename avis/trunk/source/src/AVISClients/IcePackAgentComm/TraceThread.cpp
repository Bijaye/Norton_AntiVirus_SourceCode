// TraceThread.cpp: implementation of the CTraceThread class.
//
//////////////////////////////////////////////////////////////////////

//------ pragmas ------------------------------------------------------
#pragma warning ( disable : 4786 ) // identifier truncated

#include "base.h"
#include "TraceThread.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTraceThread::CTraceThread(
	LPCSTR pszLogFileName,
	LPCSTR pszTraceFileName,
	LPCSTR pszTraceLogAgentName)
 :	m_tsfLog(pszLogFileName),
	m_tsfTrace(pszTraceFileName),
	m_mtLog(m_tsfLog),
	m_mtTrace(m_tsfTrace),
	m_tcLog(m_mtLog, pszTraceLogAgentName),
	m_tcTrace(m_mtTrace, pszTraceLogAgentName)
{
}
/*****/
CTraceThread::~CTraceThread()
{
}
/*****/
TraceClient& CTraceThread::GetLog()
{
	return m_tcLog;
}
/*****/
TraceClient& CTraceThread::GetTrace()
{
	return m_tcTrace;
}

