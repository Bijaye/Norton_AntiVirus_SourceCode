// MyTraceClient.cpp: implementation of the MyTraceClient class.
//
//////////////////////////////////////////////////////////////////////

//------ pragmas ------------------------------------------------------
#pragma warning ( disable : 4786 ) // identifier truncated

#include "base.h"
#include "TraceThread.h"
#include "MyTraceClient.h"


// macros to be used in the message methods
#define TRACE_ONLY		TRUE
#define TRACE_AND_LOG	FALSE

// ------ collection --------------------------------------------------
#pragma warning ( disable : 4786 ) // identifier truncated
#include <map>
typedef std::map<DWORD, CTraceThread*> IntThreadMap;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyTraceClient::MyTraceClient()
 :	_tracingEnabled( false ),
	_map( static_cast<IntThreadMap*>(new IntThreadMap) )
{
	if ( _map == NULL)
	{
//		TRACE("Failed creating new MyTraceClient::IntThreadMap object.");
//		AfxThrowMemoryException();
	}
}
/*****/
MyTraceClient::~MyTraceClient()
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	for ( IntThreadMap::iterator iter = static_cast<IntThreadMap*>(_map)->begin(); iter != static_cast<IntThreadMap*>(_map)->end(); iter++ )
	{
		delete iter->second;
	}

	delete static_cast<IntThreadMap*>(_map);
}
/*****/
void MyTraceClient::RegisterThread(	const std::string& logFileName,
									const std::string& traceFileName,
									const std::string& agentName )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DWORD id = GetCurrentThreadId();

	if ( static_cast<IntThreadMap*>(_map)->find( id ) != static_cast<IntThreadMap*>(_map)->end() )
	{
//		TRACE("Cannot register current thread more than once as a user of MyTraceClient.");
		return;
	}

	CTraceThread* pTraceThread = new CTraceThread( logFileName.c_str(), traceFileName.c_str(), agentName.c_str() );

	if (pTraceThread == NULL)
	{
//		TRACE("Failed creating new CTraceThread object.");
//		AfxThrowMemoryException();
	}

	(*static_cast<IntThreadMap*>(_map))[ id ] = pTraceThread;

	debug("registered current thread (id = %d) as a user of MyTraceClient", id );
}


void MyTraceClient::UnregisterThread()
{
	DWORD id = GetCurrentThreadId();

	IntThreadMap::iterator iter = static_cast<IntThreadMap*>(_map)->find( id );
	if ( iter != static_cast<IntThreadMap*>(_map)->end() )
	{
		delete iter->second;
		static_cast<IntThreadMap*>(_map)->erase( iter );
	}
}


//	ASSERT(this); \
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

/*****/

#define IMPLEMENT_METHOD(TraceMethod, boWriteToTraceOnly) \
void MyTraceClient::TraceMethod( const char * szFormat, ...) \
{ \
	if (!_tracingEnabled && boWriteToTraceOnly) \
		return; \
	char szFormatted[10*1024]; \
	va_list valist; \
	va_start( valist, szFormat ); \
	vsprintf( szFormatted, szFormat, valist ); \
	va_end( valist ); \
	CTraceThread* pTraceThread = NULL; \
	DWORD id = GetCurrentThreadId(); \
	pTraceThread = (*static_cast<IntThreadMap*>(_map))[ id ]; \
	if (pTraceThread == NULL) \
		return; \
	if (!boWriteToTraceOnly) \
		pTraceThread->GetLog().TraceMethod(szFormatted); \
	if (_tracingEnabled) \
		pTraceThread->GetTrace().TraceMethod(szFormatted); \
}

void MyTraceClient::msg( const char * szFormat, ...)
{
	if (!_tracingEnabled && TRACE_AND_LOG)
		return;
	char szFormatted[10*1024];
	va_list valist;
	va_start( valist, szFormat );
	vsprintf( szFormatted, szFormat, valist );
	va_end( valist );
	CTraceThread* pTraceThread = NULL;
	DWORD id = GetCurrentThreadId();
	pTraceThread = (*static_cast<IntThreadMap*>(_map))[ id ];
	if (pTraceThread == NULL)
		return;
	if (!TRACE_AND_LOG)
		pTraceThread->GetLog().msg(szFormatted);
	if (_tracingEnabled)
		pTraceThread->GetTrace().msg(szFormatted);
}

void MyTraceClient::debug( const char * szFormat, ...)
{
//	ASSERT(this);
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!_tracingEnabled && TRACE_ONLY)
		return;
	char szFormatted[10*1024];
	va_list valist;
	va_start( valist, szFormat );
	vsprintf( szFormatted, szFormat, valist );
	va_end( valist );
	CTraceThread* pTraceThread = NULL;
	DWORD id = GetCurrentThreadId();
	pTraceThread = (*static_cast<IntThreadMap*>(_map))[ id ];
	if (pTraceThread == NULL)
		return;
	if (!TRACE_ONLY)
		pTraceThread->GetLog().debug(szFormatted);
	if (_tracingEnabled)
		pTraceThread->GetTrace().debug(szFormatted);
}

#define IMPLEMENT_NOOP(TraceMethod, boWriteToTraceOnly) \
void MyTraceClient::TraceMethod( const char * szFormat, ...) \
{ \
	return; \
}

//IMPLEMENT_METHOD(msg, TRACE_AND_LOG)
IMPLEMENT_NOOP  (enter, TRACE_ONLY)
IMPLEMENT_NOOP  (exit, TRACE_ONLY)
//IMPLEMENT_METHOD(debug, TRACE_ONLY)
IMPLEMENT_METHOD(info, TRACE_AND_LOG)
IMPLEMENT_METHOD(warning, TRACE_AND_LOG)
IMPLEMENT_METHOD(error, TRACE_AND_LOG)
IMPLEMENT_METHOD(critical, TRACE_AND_LOG)

/*****/
void MyTraceClient::EnableTracing( bool enableTracing)
{
	_tracingEnabled = enableTracing;
}
