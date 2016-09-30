//
//	IBM AntiVirus Immune System
//
//	File Name:	StatusReceiverTransaction.h
//	Author:		Milosz Muszynski
//
//	This class is based on the CAVISTransaction class
//	and overrides some of its virtual functions in order
//	to perform transaction to inquire for a sample
//	status
//
//	$Log: $
//


#if !defined(AFX_StatusReceiverTransaction_H__19492900_F0E9_11d2_8A97_9EAE3C000000__INCLUDED_)
#define AFX_StatusReceiverTransaction_H__19492900_F0E9_11d2_8A97_9EAE3C000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyTraceClient.h"
#include "TransactionExecutor.h"
#include "IcePackAgentComm.h"

class StatusReceiverTransaction : public TransactionExecutor
{
public:
	StatusReceiverTransaction(
		const std::string&				agentName,
		const std::string&				gatewayURL,
		const std::string&				cookie,
		RawEvent&						eventStop,
		MyTraceClient&					traceClient,
		bool							useSSL,
    const std::string&        target,
  const HttpConnection::connectionVerb    verb );

	virtual ~StatusReceiverTransaction();

	virtual	int			StartRequest(	DWORD	cbSendContent,	// how much data to send
										PVOID	pvSendContent);	// data to send
	virtual void		EndRequest	();
};

#endif // !defined(AFX_StatusReceiverTransaction_H__19492900_F0E9_11d2_8A97_9EAE3C000000__INCLUDED_)

