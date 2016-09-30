// AVISClientSampleImporter.h: interface for the CAVISClientSampleImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISCLIENTSAMPLEIMPORTER_H__DB48F9BD_B167_11D2_8A8D_00203529AC86__INCLUDED_)
#define AFX_AVISCLIENTSAMPLEIMPORTER_H__DB48F9BD_B167_11D2_8A8D_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISTransactionImportSample.h"
#include "AVISClientStatusReporter.h"
#include "AVISClient.h"

/*****/
class CAVISClientSampleImporter : public CAVISClient
{
private:
	CAVISTransactionImportSample* m_pTxIS;

	// need to make a private instance of the "report status" transaction to be used
	// in the "postSampleStatus" operation.  It is used solely for the initial status
	// report.
	CAVISTransactionReportStatus* m_pTxRS;

public:
	CAVISClientSampleImporter(
		LPCSTR pszGatewayURL, 
		UINT uiIdGateway,
		CAVISGatewayComm* pGwc,
		BOOL boUseSSL);
	virtual ~CAVISClientSampleImporter();

protected:
	DWORD GetSuspectSample();
	BOOL CreateTransactions(CString& strGatewayURL, BOOL boUseSSL);
	DWORD ClientMain();
	DWORD ReportInitialStatusUntilSent(AnalysisRequest& ar, BOOL boRecovering = FALSE);
    DWORD ReportFinalStatusUntilSent(AnalysisResults& results);
	void CleanupIncompleteImportedSamples();

public:
	BOOL End(DWORD, BOOL);

};

#endif // !defined(AFX_AVISCLIENTSAMPLEIMPORTER_H__DB48F9BD_B167_11D2_8A8D_00203529AC86__INCLUDED_)
