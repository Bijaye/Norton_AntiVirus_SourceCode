#ifndef __TRANSACTIONEXECUTOR__H__
#define __TRANSACTIONEXECUTOR__H__

#include "Transaction.h"

class TransactionExecutor : public Transaction
{
public:
	TransactionExecutor(		const std::string&	agentName,
								const std::string&	gatewayURL,
								int					verb,
								MyTraceClient&		tc,
								RawEvent&			eventStop,
								const std::string&	target = "",
								bool				useSSL = false	)
								:
								Transaction(		agentName,
													gatewayURL,
													verb,
													tc,
													eventStop,
													target,
													useSSL	)
								{}

	int	Execute			(		int cbRequestContent = 0, 
								PVOID pvRequestContent = NULL	);	// call to do request and response processing

	// allow any initialization to be done before the request is executed
	virtual DWORD InitRequest();

	// override to send any request content in blocks
	// the overridden function should return the following
	// AVISTX_SUCCESS			: no errors
	// AVISTX_NETWORK_FAILURE	: network error occurred
	virtual int StartRequest(DWORD cbSendContent, PVOID pvSendContent);

	// override to perform any post-send request cleanup
	// the overridden function should return the following
	// AVISTX_SUCCESS			: no errors
	// AVISTX_NETWORK_FAILURE	: network error occurred
	virtual void EndRequest();

	// override to read any response content
	// the overridden function should return the following
	// AVISTX_SUCCESS			: no errors
	// AVISTX_NETWORK_FAILURE	: network error occurred
	virtual int ReadResponseContent();
};


#endif __TRANSACTIONEXECUTOR__H__
