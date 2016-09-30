// AVISTransactionReportStatus.h: interface for the CAVISTransactionReportStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISTRANSACTIONREPORTSTATUS_H__1A788AE4_ACAB_11D2_8A8C_00203529AC86__INCLUDED_)
#define AFX_AVISTRANSACTIONREPORTSTATUS_H__1A788AE4_ACAB_11D2_8A8C_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISTransaction.h"
#include "TransactionTimeoutMonitorThread.h"

class CAVISTransactionReportStatus : public CAVISTransaction  
{
public:
	CAVISTransactionReportStatus(
		LPCSTR pszAgentName,
		LPCSTR pszGatewayURL,
		UINT uiTimeoutTransactionConnected,
		UINT uiTimeoutTransactionCompleted,
		CTransactionTimeoutMonitorThread* pthreadTransactionTimeoutMonitor,
		BOOL boUseSSL = FALSE);

	virtual ~CAVISTransactionReportStatus();

	BOOL GatewayAcceptedStatus();
};

// PostSampleStatus return codes
#define PSS_GATEWAY_REJECTED_STATUS	PSS_RC_BASE + 1
#define PSS_SENT_STATUS				PSS_RC_BASE + 2

#endif // !defined(AFX_AVISTRANSACTIONREPORTSTATUS_H__1A788AE4_ACAB_11D2_8A8C_00203529AC86__INCLUDED_)
