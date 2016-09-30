// MyTraceClient.h: interface for the MyTraceClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYTRACECLIENT_H__B27882D9_D008_11D2_8A94_00203529AC86__INCLUDED_)
#define AFX_MYTRACECLIENT_H__B27882D9_D008_11D2_8A94_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef ICEPACKAGENTCOMM_EXPORTS
#define TRACE_API __declspec(dllexport)
#else
#define TRACE_API __declspec(dllimport)
#endif

#include <string>

class TRACE_API MyTraceClient
{
public:
	MyTraceClient();
	virtual ~MyTraceClient();

public:
	virtual void RegisterThread(	const std::string& pszLogFileName,
									const std::string& pszTraceFileName,
									const std::string& pszAgentName );
	virtual void UnregisterThread();

	virtual void msg		(const char * szFormat, ...);
	virtual void enter		(const char * szFormat, ...);
	virtual void exit		(const char * szFormat, ...);
	virtual void debug		(const char * szFormat, ...);
	virtual void info		(const char * szFormat, ...);
	virtual void warning	(const char * szFormat, ...);
	virtual void error		(const char * szFormat, ...);
	virtual void critical	(const char * szFormat, ...);

	virtual void EnableTracing( bool enableTracing = true );

private:
	//CMapWordToPtr m_map;
	void *			_map;
	bool			_tracingEnabled;
};

#endif // !defined(AFX_MYTRACECLIENT_H__B27882D9_D008_11D2_8A94_00203529AC86__INCLUDED_)
