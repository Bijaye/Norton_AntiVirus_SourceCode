// AVISClientStatusReporter.h: interface for the CAVISClientStatusReporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISCLIENTSTATUSREPORTER_H__DB48F9BB_B167_11D2_8A8D_00203529AC86__INCLUDED_)
#define AFX_AVISCLIENTSTATUSREPORTER_H__DB48F9BB_B167_11D2_8A8D_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnalysisRequest.h"
#include "AnalysisResults.h"
#include "AVISTransactionReportStatus.h"
#include "AVISClient.h"

/*****/
class CAVISClientStatusReporter : public CAVISClient
{
private:
	CAVISTransactionReportStatus* m_pTxRS;

public:
	CAVISClientStatusReporter(
		LPCSTR pszGatewayURL, 
		UINT uiIdGateway,
		CAVISGatewayComm* pGwc,
		BOOL boUseSSL);
	virtual ~CAVISClientStatusReporter();

protected:
	BOOL CreateTransactions(CString& strGatewayURL, BOOL boUseSSL);
	DWORD ClientMain();

private:
	static DWORD TryPostSampleStatus(
		AnalysisRequest& request,
		CAVISTransactionReportStatus* pTxRS);

        static DWORD TryPostFinalStatus(
                AnalysisResults& results,
                CAVISTransactionReportStatus* pTxRS);
public:
	static DWORD ReportStatusUntilSent(
		AnalysisRequest& ar,
		CAVISTransactionReportStatus* pTxRS);

        static DWORD ReportFinalStatusUntilSent(
                 AnalysisResults& results,
                 CAVISTransactionReportStatus* pTxRS);
	BOOL End(DWORD, BOOL);
};

#endif // !defined(AFX_AVISCLIENTSTATUSREPORTER_H__DB48F9BB_B167_11D2_8A8D_00203529AC86__INCLUDED_)
