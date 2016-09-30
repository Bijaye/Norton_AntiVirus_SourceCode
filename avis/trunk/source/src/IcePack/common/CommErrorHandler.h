
#ifndef COMMERRORHANDLER_H
#define COMMERRORHANDLER_H

class MyTraceClient;
#include <IcePackAgentComm.h>
#include <Logger.h>

class ErrorInfo
{
public:
	enum GatewayError	{	gwNoError, gwAbandoned, gwContent, gwCrumbled, gwDelined, gwInternal,
							gwLost, gwMalformed, gwMissing, gwOverrun, gwSample, gwSuperceded,
							gwType, gwUnderrun, gwUnavailable, gwMissingXError, gwUnknown
	
	};

	IcePackAgentComm::ErrorCode	commErrorCode;
	GatewayError				gatewayErrorCode;
	uint						tries;

	ErrorInfo()	: commErrorCode(IcePackAgentComm::NoError), gatewayErrorCode(gwNoError),
					tries(0)
	{};

	void Reset(void)
	{
		commErrorCode		= IcePackAgentComm::NoError;
		gatewayErrorCode	= gwNoError;
		tries				= 0;
	}

	void CommError(const IcePackAgentComm::ErrorCode newCode)
	{
		if (newCode != commErrorCode)
		{
			commErrorCode	= newCode;
			tries			= 0;
			gatewayErrorCode= gwNoError;
		}
		else
			tries++;
	}

	void GatewayError(const GatewayError newCode)
	{
		if (IcePackAgentComm::GatewayError != commErrorCode ||
			gatewayErrorCode != newCode)
		{
			commErrorCode	= IcePackAgentComm::GatewayError;
			gatewayErrorCode= newCode;
			tries			= 0;
		}
		else
			tries++;
	}
};

class CommErrorHandler
{
public:

	static void	HandleCommError(IcePackAgentComm::ErrorCode errCode,
							ulong systemErrorCode,
							ErrorInfo&	errorInfo,
							bool& retryWithInterval,
							bool& retryWithOutInterval,
							Logger::LogSource caller, const char* errMsgPrefix);
};

#endif
