// MyTraceClient.cpp: implementation of the MyTraceClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyTraceClient.h"
#include "Logger.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// macros to be used in the message methods
#define TRACE_ONLY		TRUE
#define TRACE_AND_LOG	FALSE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyTraceClient::MyTraceClient()
 :	m_boTracingEnabled(FALSE)
{
}
/*****/
MyTraceClient::~MyTraceClient()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	WORD nIdThread;
	CTraceThread* pTraceThread;

	// delete all the CTraceThread objects in the map
	for (POSITION pos = m_map.GetStartPosition(); pos != NULL;)
	{
		m_map.GetNextAssoc(pos, nIdThread, (PVOID& )pTraceThread);

		// since the pointer can be on the list more than once, don't delete it if
		// it has been deleted already
		if (pTraceThread != NULL)
		{
			delete pTraceThread;

			// go through the list and NULL out any other elements that have refer to
			// the same CTraceThread object (since we don't want to delete the same object
			// multiple times).
			WORD nDuplicateIdThread;
			CTraceThread* pDuplicateTraceThread;

			// start iterating using the first element following the current (pos) one.
			for (POSITION posDuplicates = pos; posDuplicates != NULL;)
			{
				m_map.GetNextAssoc(posDuplicates, nDuplicateIdThread, (PVOID& )pDuplicateTraceThread);

				// found a match?
				if (pDuplicateTraceThread == pTraceThread)
					m_map.SetAt(nDuplicateIdThread, NULL);
			}
		}
	}

	m_map.RemoveAll();
}
/*****/
void MyTraceClient::RegisterThread(
	LPCSTR pszLogFileName,
	LPCSTR pszTraceFileName,
	LPCSTR pszAgentName,
	DWORD nIdOtherThread)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD nIdThread = ::GetCurrentThreadId();

	if (m_map[nIdThread] != NULL)
	{
//		debug("Cannot register current thread more than once as a user of MyTraceClient.");
		return;
	}

	CTraceThread* pTraceThread;

	if (nIdOtherThread == 0)
	{
		// create a new TraceThread object
		pTraceThread = new CTraceThread(pszLogFileName, pszTraceFileName, pszAgentName);

		if (pTraceThread == NULL)
		{
//			TRACE("Failed creating new CTraceThread object.");
			AfxThrowMemoryException();
		}

		// enable logging in Andy's libraries
		Logger::SendTo(&pTraceThread->GetMTraceForTrace());
		Logger::SeverityFilter(Logger::LogEntry);
		Logger::SourceFilter(Logger::LogEverything);
	}
	else
	{
		// get pointer to TraceThread object for other thread
		pTraceThread = (CTraceThread* )m_map[nIdOtherThread];
	}

	m_map[nIdThread] = pTraceThread;

	debug("Registered current thread (id = %d) as a user of MyTraceClient.", nIdThread);
}
/*****/

#define IMPLEMENT_METHOD(TraceMethod, boWriteToTraceOnly) \
void MyTraceClient::TraceMethod(LPCTSTR szFormat, ...) \
{ \
	ASSERT(this); \
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); \
	if (!m_boTracingEnabled && boWriteToTraceOnly) \
		return; \
	char szFormatted[10*1024]; \
	va_list valist; \
	va_start( valist, szFormat ); \
	::vsprintf( szFormatted, szFormat, valist ); \
	va_end( valist ); \
	CTraceThread* pTraceThread = (CTraceThread* )m_map[GetCurrentThreadId()]; \
	if (pTraceThread == NULL) \
		return; \
	if (!boWriteToTraceOnly) \
		pTraceThread->GetLog().TraceMethod(szFormatted); \
	if (m_boTracingEnabled) \
		pTraceThread->GetTrace().TraceMethod(szFormatted); \
}

#ifdef _DEBUG
void MyTraceClient::debug(LPCTSTR szFormat, ...)
{
	ASSERT(this);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!m_boTracingEnabled && TRACE_ONLY)
		return;
	char szFormatted[10*1024];
	va_list valist;
	va_start( valist, szFormat );
	wvsprintf( szFormatted, szFormat, valist );
	va_end( valist );
	CTraceThread* pTraceThread = (CTraceThread* )m_map[GetCurrentThreadId()];
	if (pTraceThread == NULL)
		return;
	if (!TRACE_ONLY)
		pTraceThread->GetLog().debug(szFormatted);
	if (m_boTracingEnabled)
		pTraceThread->GetTrace().debug(szFormatted);
}
#else
IMPLEMENT_METHOD(debug, TRACE_ONLY)
#endif

IMPLEMENT_METHOD(msg, TRACE_AND_LOG)
IMPLEMENT_METHOD(enter, TRACE_ONLY)
IMPLEMENT_METHOD(exit, TRACE_ONLY)
IMPLEMENT_METHOD(info, TRACE_AND_LOG)
IMPLEMENT_METHOD(warning, TRACE_AND_LOG)
IMPLEMENT_METHOD(error, TRACE_AND_LOG)
IMPLEMENT_METHOD(critical, TRACE_AND_LOG)

/*****/
void MyTraceClient::EnableTracing(BOOL boEnableTracing)
{
	m_boTracingEnabled = boEnableTracing;
}
