// AVISTransactionReportStatus.cpp: implementation of the CAVISTransactionReportStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISTransactionReportStatus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAVISTransactionReportStatus::CAVISTransactionReportStatus(
	LPCSTR pszAgentName,
	LPCSTR pszGatewayURL,
	UINT uiTimeoutTransactionConnected,
	UINT uiTimeoutTransactionCompleted,
	CTransactionTimeoutMonitorThread* pthreadTransactionTimeoutMonitor,
	BOOL boUseSSL)
  : CAVISTransaction(
		pszAgentName,
		pszGatewayURL,
		CHttpConnection::HTTP_VERB_POST,
		uiTimeoutTransactionConnected,
		uiTimeoutTransactionCompleted,
		pthreadTransactionTimeoutMonitor,
		REPORT_STATUS_SERVLET_NAME,
		boUseSSL)
{
}
/*****/
CAVISTransactionReportStatus::~CAVISTransactionReportStatus()
{
}
/*****/
BOOL CAVISTransactionReportStatus::GatewayAcceptedStatus()
{
	// in this transaction, the gateway returns 'no content' if status was accepted
	return(HTTP_STATUS_NO_CONTENT == GetResponseStatusCode());
}
/*****/
