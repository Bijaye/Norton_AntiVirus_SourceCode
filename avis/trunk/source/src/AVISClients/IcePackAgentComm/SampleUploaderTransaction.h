//
//	IBM AntiVirus Immune System
//
//	File Name:	SampleUploaderTransaction.h
//	Author:		Milosz Muszynski
//
//	This class is based on the CAVISTransaction class
//	and overrides some of its virtual functions in order
//	to perform transaction to upload a suspect sample
//	to the gateway
//
//	$Log: $
//


#if !defined(AFX_SampleUploaderTransaction_H__19492900_F0E9_11d2_8A97_9EAE3C000000__INCLUDED_)
#define AFX_SampleUploaderTransaction_H__19492900_F0E9_11d2_8A97_9EAE3C000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyTraceClient.h"
#include "TransactionExecutor.h"
#include "IcePackAgentComm.h"

class SampleUploaderTransaction : public TransactionExecutor  
{
private:
	IcePackAgentComm::Stream&		_stream;
	IcePackAgentComm::ErrorCode		_errorCode;

	enum							{ ReadBufferSize = 16384 };

public:
	SampleUploaderTransaction(
		const std::string&				agentName,
		const std::string&				gatewayURL,
		IcePackAgentComm::Stream&		stream,
		RawEvent&						eventStop,
		MyTraceClient&					traceClient,
		bool							useSSL,
    const std::string&        target,
    const HttpConnection::connectionVerb    verb);

	virtual ~SampleUploaderTransaction();

	virtual DWORD		InitRequest					();
	virtual int 		StartRequest				( DWORD, PVOID );
	virtual void 		EndRequest					();
	virtual int 		GetNextRequestContentBlock	( PVOID pvRequestContentBlock );
};

#endif // !defined(AFX_SampleUploaderTransaction_H__19492900_F0E9_11d2_8A97_9EAE3C000000__INCLUDED_)

