//
//	IBM AntiVirus Immune System
//
//	File Name:	SignatureInquirer.h
//	Author:		Milosz Muszynski
//
//	This class is based on the CommClient and Thread classes
//	and provides a thread for a specific communication function
//	In this case the function is inquiring the gateway for the
//	availability of the given signature set
//
//	$Log: $
//



#if !defined(AFX_SignatureInquirer_H__BBA640F0_F1D4_11d2_8A97_9EAE3C000000__INCLUDED_)
#define AFX_SignatureInquirer_H__BBA640F0_F1D4_11d2_8A97_9EAE3C000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommClient.h"
#include "SignatureInquirerTransaction.h"
#include "Config.h"

class SignatureInquirer : public CommClient
{
private:
	IcePackAgentComm::SignatureType	_signatureType;
	std::string						_sequenceNumber;
public:
	static std::string				NameForLatest		;
	static std::string				NameForLatestBlessed;

public:
	SignatureInquirer(
		const std::string&				logFileName,
		const std::string&				traceFileName,
		const std::string&				agentName,
		RawEvent&						eventStop,
		MyTraceClient&					traceClient );
	virtual ~SignatureInquirer();

protected:
	IcePackAgentComm::ErrorCode ClientMain();

public:
  void				Go	(	const std::string&				gatewayURL,
							const std::string&				headers,
								const std::string&				sequenceNumber,
								IcePackAgentComm::SignatureType	signatureType	);
	BOOL				End(DWORD, BOOL);
	std::string			SequenceNumber() const { return _sequenceNumber; }

private:
  IcePackAgentComm::ErrorCode _reallyLaunch(const std::string                    &gateway,
                                            const std::string                    &target,
                                            const HttpConnection::connectionVerb  verb);
};

#endif // !defined(AFX_SignatureInquirer_H__BBA640F0_F1D4_11d2_8A97_9EAE3C000000__INCLUDED_)
