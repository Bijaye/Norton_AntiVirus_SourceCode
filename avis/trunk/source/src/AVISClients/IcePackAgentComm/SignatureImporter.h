//
//	IBM AntiVirus Immune System
//
//	File Name:	SignatureImporter.h
//	Author:		Milosz Muszynski
//
//	This class is based on the CommClient and Thread classes
//	and provides a thread for a specific communication function
//	In this case the function is importing the signature set 
//	from the gateway
//
//	$Log: $
//



#if !defined(AFX_SIGNATUREIMPORTER_H__BBA640F0_F1D4_11d2_8A97_9EAE3C000000__INCLUDED_)
#define AFX_SIGNATUREIMPORTER_H__BBA640F0_F1D4_11d2_8A97_9EAE3C000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommClient.h"
#include "SignatureImporterTransaction.h"

class SignatureImporter : public CommClient
{
private:
	IcePackAgentComm::SignatureType	_signatureType;
	std::string						_sequenceNumber;
	std::string						_targetDirectory;
	bool							_withUnpacking;

	static std::string				NameForLatest		;
	static std::string				NameForLatestBlessed;

public:
	SignatureImporter(
		const std::string&				logFileName,
		const std::string&				traceFileName,
		const std::string&				agentName,
		RawEvent&						eventStop,
		MyTraceClient&					traceClient );
	virtual ~SignatureImporter();

protected:
	IcePackAgentComm::ErrorCode ClientMain();

public:
	void				Go	(	const std::string&				gatewayURL,
                    const std::string&        headers,
								const std::string&				sequenceNumber,
								const std::string&				targetDirectory,
								IcePackAgentComm::SignatureType	signatureType,
								bool							withUnpacking	);
	BOOL				End(DWORD, BOOL);

private:
  IcePackAgentComm::ErrorCode _reallyLaunch(const std::string                    &gateway,
                                            const std::string                    &target,
                                            const HttpConnection::connectionVerb  verb);
};

#endif // !defined(AFX_SIGNATUREIMPORTER_H__BBA640F0_F1D4_11d2_8A97_9EAE3C000000__INCLUDED_)
