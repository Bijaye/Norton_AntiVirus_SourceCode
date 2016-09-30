//
//	IBM AntiVirus Immune System
//
//	File Name:	SignatureImporterTransaction.h
//	Author:		Milosz Muszynski
//
//	This class is based on the CAVISTransaction class
//	and overrides some of its virtual functions in order
//	to perform transaction to import a signature set
//	from a gateway
//
//	$Log: $
//


#if !defined(AFX_SignatureImporterTransaction_H__19492900_F0E9_11d2_8A97_9EAE3C000000__INCLUDED_)
#define AFX_SignatureImporterTransaction_H__19492900_F0E9_11d2_8A97_9EAE3C000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyTraceClient.h"
#include "TransactionExecutor.h"
#include "IcePackAgentComm.h"
#include "Config.h"
#include "HttpConnection.h"

class SignatureImporterTransaction : public TransactionExecutor
{
private:
	std::string 					_signatureStoragePath;
	bool							_withUnpacking;
	bool							_headersRetrieved;
	enum							{ ReadResponseBlockSize = 16384 };

	enum PackageType				{ FatAlbert, VDB };
	PackageType						_packageType;

public:
	SignatureImporterTransaction(
		const std::string&				agentName,
		const std::string&				gatewayURL,
		const std::string&				targetDirectory,
		bool							withUnpacking,
		RawEvent&						eventStop,
		MyTraceClient&					traceClient,
		bool							useSSL,
    const std::string&        target,
    const HttpConnection::connectionVerb);

	virtual ~SignatureImporterTransaction();

	virtual	int			StartRequest(	DWORD	cbSendContent,	// how much data to send
										PVOID	pvSendContent);	// data to send
	virtual void		EndRequest	();
	int					ReadResponseContent();
	bool				SignatureWasAvailableAtGateway();

private:
	IcePackAgentComm::ErrorCode DetermineFileNameAndType( std::string& fileName );
	IcePackAgentComm::ErrorCode ReceiveContentIntoFile  ( const std::string& fileName );

#if 0 /* inw 2000-02-16 removed FatAlbert support */
	IcePackAgentComm::ErrorCode FatAlbertUnpack			( const std::string& fileName );
#endif /* 0 */

	IcePackAgentComm::ErrorCode VDBUnpack				( const std::string& fileName );
};

#endif // !defined(AFX_SignatureImporterTransaction_H__19492900_F0E9_11d2_8A97_9EAE3C000000__INCLUDED_)

