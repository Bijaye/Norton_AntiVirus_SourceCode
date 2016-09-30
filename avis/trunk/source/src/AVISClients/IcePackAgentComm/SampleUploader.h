//
//	IBM AntiVirus Immune System
//
//	File Name:	SampleUploader.h
//	Author:		Milosz Muszynski
//
//	This class is based on the CommClient and Thread classes
//	and provides a thread for a specific communication function
//	In this case the function is uploading the sample
//	to the gateway
//
//	$Log: $
//



#if !defined(AFX_SampleUploader_H__BBA640F0_F1D4_11d2_8A97_9EAE3C000000__INCLUDED_)
#define AFX_SampleUploader_H__BBA640F0_F1D4_11d2_8A97_9EAE3C000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommClient.h"
#include "SampleUploaderTransaction.h"
#include "Config.h"
#include "HttpConnection.h"

class SampleUploader : public CommClient
{
private:
	IcePackAgentComm::Stream *		_pstream;

public:
	SampleUploader(
		const std::string&				logFileName,
		const std::string&				traceFileName,
		const std::string&				agentName,
		RawEvent&						eventStop,
		MyTraceClient&					traceClient);
	virtual ~SampleUploader();

protected:
	IcePackAgentComm::ErrorCode ClientMain();

public:
	void				Go(	const std::string&				gatewayURL,
							const std::string&				headers,
							IcePackAgentComm::Stream&		stream );

	BOOL				End(DWORD, BOOL);

private:
  IcePackAgentComm::ErrorCode _reallyLaunch(const std::string                    &gateway,
                                            const std::string                    &target,
                                            const HttpConnection::connectionVerb  verb);

};

#endif // !defined(AFX_SampleUploader_H__BBA640F0_F1D4_11d2_8A97_9EAE3C000000__INCLUDED_)
