// ScannerSubmittor.h: interface for the ScannerSubmittor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANNERSUBMITTOR_H__E4E7A0A5_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_)
#define AFX_SCANNERSUBMITTOR_H__E4E7A0A5_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <CMclThread.h>

#include "QuarantineMonitor.h"
#include <MyTraceClient.h>
#include <IcePackAgentComm.h>
#include <WinRegistry.h>
#include <Logger.h>

#include "CommErrorHandler.h"

class ScannerSubmittor : public CMclThreadHandler  
{
public:
	ScannerSubmittor(QuarantineMonitor& qMon);
	virtual ~ScannerSubmittor();

	unsigned ThreadHandlerProc(void);

private:
	static uint			submittedSamples;
	QuarantineMonitor&	quarantineMonitor;
	MyTraceClient		traceSampleClient, traceSigDwnldClient;
	
	bool							traceSampleComm;
	bool							traceSigComm;
	IcePackAgentComm::SampleComm	sampleComm;
	IcePackAgentComm::SignatureComm	sigComm;

	bool	SubmitNewSample(std::list<Sample>& listCopy, bool& signalNewJob);
	bool	UploadSample(Sample& sample, std::string& attrs, std::string& url);
	void	CreateAttributes(Sample& sample);
	bool	HandleSubmissionError(IcePackAgentComm::SampleComm& sampComm,
								std::string& attrs, Sample& sample,
								ErrorInfo& errorInfo);
public:
	static uint	UpgradeSequenceNumber(IcePackAgentComm::SignatureComm& sig,
										uint minSequenceNumber,
										bool& blessed);
	static bool	DownloadSig(IcePackAgentComm::SignatureComm& sig,
							MyTraceClient& traceClient, bool& traceSigCom,
							uint sigSeqNum, bool blessed);

	static bool	HandleDownloadError(IcePackAgentComm::SignatureComm& sig,
								std::string& attrs, uint seqNum,
								bool& blessed, ErrorInfo& errorInfo);

	static bool	ScanPendingSamples(std::list<Sample>& listCopy, bool& signalNewJob);
};

#endif // !defined(AFX_SCANNERSUBMITTOR_H__E4E7A0A5_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_)
