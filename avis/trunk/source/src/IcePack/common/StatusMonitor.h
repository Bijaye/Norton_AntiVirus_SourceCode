// StatusMonitor.h: interface for the StatusMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSMONITOR_H__E4E7A0A2_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_)
#define AFX_STATUSMONITOR_H__E4E7A0A2_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVIS.h>

#include <string>

#include <CMclThread.h>

class MyTraceClient; //#include <MyTraceClient.h>
#include <IcePackAgentComm.h>
#include <Logger.h>
#include <Sample.h>
#include <CommErrorHandler.h>

class StatusMonitor : public CMclThreadHandler  
{
public:
	StatusMonitor();
	virtual ~StatusMonitor();

	unsigned ThreadHandlerProc(void);

private:
	void HandleGatewayError(IcePackAgentComm::SampleComm& sampComm,
								   std::string& attrs, Sample& sample,
								   ErrorInfo& errorInfo,
								   bool& retryWithInterval, bool& retryWithoutInterval);

};

#endif // !defined(AFX_STATUSMONITOR_H__E4E7A0A2_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_)
