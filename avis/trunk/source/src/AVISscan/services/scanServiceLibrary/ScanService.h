// ScanService.h: interface for the ScanService class.
// ver. avis200.135
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANSERVICE_H__0D88D1EB_84B2_11D2_ACF1_00A0C9C71BBC__INCLUDED_)
#define AFX_SCANSERVICE_H__0D88D1EB_84B2_11D2_ACF1_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory>
#include <string>
#include <vector>

#include <AVIS.h>
#include <CGrowableThreadPool.h>
#include <Handle.h>
#include <CMclSemaphore.h>
#include <CMclEvent.h>

#include "Scan.h"
//#include "ScanMonitor.h"
#include "NAVEngine.h"
#include "../../../icepack/common/registrydefaults.h"

class ScanService  
{
protected:
	ScanService();
	virtual ~ScanService();

public:
	void CreateSecurityDesc(SECURITY_DESCRIPTOR& securityDesc);

	static	TCHAR*	ShortName(void)			{ return shortName; }
	static	TCHAR*	DisplayName(void)		{ return displayName; }

	bool	Initialize(SERVICE_STATUS_HANDLE sHandle, SERVICE_STATUS& sStatus);
	bool	Run();
	bool	DeInitialize(SERVICE_STATUS_HANDLE sHandle, SERVICE_STATUS& sStatus);

	enum {  threadTimeOut = 6000,
			maxMessageSize = 512, maxGetPipeWait = 2000,
			monitorInterval = 60000,
			absMaximumNumberOfThreads = ICEP_MAX_DEFINITIONCONCURRENCYMAXIMUM };

	enum ServiceType { ScanLatest, ScanBlessed, ScanExplicit };

	static ServiceType GetServiceType(void) { return serviceType; };

	NAVEngine::Status ChangeDefs(uint newSigSeqNum);

	static ScanService&	Current(void);

	static	DWORD	GetDefs(BOOL waitAll, DWORD milliseconds);
	static	DWORD	ReleaseDefs(void);

	//NAVEngine::Status ChangeDefs_(uint newSigSeqNum);

	void	Pause(void)	{ serviceClients.Reset(); };
	void	Continue(void)	{ serviceClients.Set();	};

	void	Stop(void);

								// is this being run as a service or not
	void	Service(bool s)	{ service = s; }
	bool	Service()		{ return service; }

//	Scan*	ScanJobs(uint& number) { number = maxThreads; return scanJobs; }


protected:
	static SECURITY_ATTRIBUTES	defaultInheritiable;

#ifdef LOG_TIME
//	ScanMonitor			scanMonitor;
#endif

	CGrowableThreadPool		threadPool;
	Scan					scanJobs[absMaximumNumberOfThreads];
	Handle					pipeArray[absMaximumNumberOfThreads];
	Handle					pipeEvents[absMaximumNumberOfThreads];
	OVERLAPPED				oLapArray[absMaximumNumberOfThreads];
	std::string				pipeName;
	bool					service;

	CMclEvent			serviceClients;		// used to prevent new requests
											// from being accepted
	CMclEvent			changingDefs;		// unsignaled when waiting
											// for a def update to finish
	CMclSemaphore		usingDefs;			// semiphore that counts users
											// of the def file (threads scanning)

	static ServiceType	serviceType;
	static TCHAR*		shortName;			// these get created in the sub class
	static TCHAR*		displayName;		// implementation file

	static const char*	defChngTimeoutTag;
	static const char*	maxThreadsTag;
	static const char*	minThreadsTag;

	static const uint	maxThreads;
	static const uint	minThreads;

	static const uint	defChangeTimeout;	// how long to wait for other scans to finish so
											// the current definition set can be replaced with a
											// different definition set.

	bool			InitFromProfiles();
	static int		GetDefChangeTimeout(void)
					{ return GetValue("definitionChangeTimeout", 
                                                          ICEP_MIN_DEFINITIONCHANGETIMEOUT, 
                                                          ICEP_MAX_DEFINITIONCHANGETIMEOUT,
                                                          ICEP_DEF_DEFINITIONCHANGETIMEOUT)*60*1000; }
        static int		GetMaxThreadCount(void)
                                        { return GetValue("definitionConcurrencyMaximum",
                                                          ICEP_MIN_DEFINITIONCONCURRENCYMAXIMUM,
                                                          ICEP_MAX_DEFINITIONCONCURRENCYMAXIMUM,
                                                          ICEP_DEF_DEFINITIONCONCURRENCYMAXIMUM); }
         static int		GetMinThreadCount(void)
                                        { return GetValue("definitionConcurrencyMinimum", 
                                                          ICEP_MIN_DEFINITIONCONCURRENCYMINIMUM,
                                                          ICEP_MAX_DEFINITIONCONCURRENCYMINIMUM,
                                                          ICEP_DEF_DEFINITIONCONCURRENCYMINIMUM); }
  // 	static int		GetDefChangeTimeout(void)
  // 					{ return GetValue("definitionChangeTimeout", 1, 1440, 5)*60*1000; }
  // 	static int		GetMaxThreadCount(void)
  // 					{ return GetValue("definitionConcurrencyMaximum", 1, absMaximumNumberOfThreads, 10); }
  // 	static int		GetMinThreadCount(void)
  // 					{ return GetValue("definitionConcurrencyMinimum", 1, absMaximumNumberOfThreads, 2); }
  static int		GetValue(const char* key,
									const int min, const int max, const int defaultV);

	virtual void	TypeSpecificInitialization(void)	= 0;

	friend class	WaitToScan;
};


#endif // !defined(AFX_SCANSERVICE_H__0D88D1EB_84B2_11D2_ACF1_00A0C9C71BBC__INCLUDED_)
