// BlessedMonitor.h: interface for the BlessedMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLESSEDMONITOR_H__B47A8E5F_0877_11D3_ADDE_00A0C9C71BBC__INCLUDED_)
#define AFX_BLESSEDMONITOR_H__B47A8E5F_0877_11D3_ADDE_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVIS.h>
#include <CMclThread.h>
#include <Logger.h>

class MyTraceClient; //#include <MyTraceClient.h>
#include <IcePackAgentComm.h>

class BlessedMonitor : public CMclThreadHandler  
{
public:
	BlessedMonitor();
	virtual ~BlessedMonitor();

	unsigned ThreadHandlerProc(void);

	static bool	GetSeqNum(IcePackAgentComm::SignatureComm& sigComm,
						uint& seqNum, const Logger::LogSource source, bool blessed);

	static bool	GetLatestBlessedSeqNum(IcePackAgentComm::SignatureComm& sigComm,
									uint& seqNum, const Logger::LogSource source)
			{ return GetSeqNum(sigComm, seqNum, source, true); }

	static bool	GetLatestSeqNum(IcePackAgentComm::SignatureComm& sigComm,
									uint& seqNum, const Logger::LogSource& source)
			{ return GetSeqNum(sigComm, seqNum, source, false); }
};

#endif // !defined(AFX_BLESSEDMONITOR_H__B47A8E5F_0877_11D3_ADDE_00A0C9C71BBC__INCLUDED_)
