// AVISClientSignatureExporter.h: interface for the CAVISClientSignatureExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISCLIENTSIGNATUREEXPORTER_H__DB48F9BB_B167_11D2_8A8D_00203529AC86__INCLUDED_)
#define AFX_AVISCLIENTSIGNATUREEXPORTER_H__DB48F9BB_B167_11D2_8A8D_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnalysisRequest.h"
#include "AVISTransactionExportSignature.h"
#include "AVISClient.h"
#include "AVISGatewayComm.h"
#include "SigsToBeExported.h"

/*****/
class CAVISClientSignatureExporter : public CAVISClient
{
private:
	CAVISTransactionExportSignature* m_pTxES;

public:
	CAVISClientSignatureExporter(
		LPCSTR pszGatewayURL, 
		UINT uiIdGateway,
		CAVISGatewayComm* pGwc,
		BOOL boUseSSL);
	virtual ~CAVISClientSignatureExporter();

protected:
	DWORD ClientMain();
	DWORD PostSignatureSetUntilSent(SigsToBeExported& stbe);
	BOOL CreateTransactions(CString& strGatewayURL, BOOL boUseSSL);

public:
	BOOL End(DWORD, BOOL);
};

#endif // !defined(AFX_AVISCLIENTSIGNATUREEXPORTER_H__DB48F9BB_B167_11D2_8A8D_00203529AC86__INCLUDED_)
