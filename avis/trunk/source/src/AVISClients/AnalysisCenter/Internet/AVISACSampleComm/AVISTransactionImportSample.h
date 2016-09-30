// AVISTransactionImportSample.h: interface for the CAVISTransactionImportSample class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISTRANSACTIONIMPORTSAMPLE_H__160D5FF6_9112_11D2_8A7C_00203529AC86__INCLUDED_)
#define AFX_AVISTRANSACTIONIMPORTSAMPLE_H__160D5FF6_9112_11D2_8A7C_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISTransaction.h"
#include "..\\AVISFilter\\filter.h"
#include "../avisdb/checksum.h"
#include "AnalysisRequest.h"
#include "AVISClient.h"
#include "TransactionTimeoutMonitorThread.h"

class CAVISTransactionImportSample : public CAVISTransaction  
{
private:
	AnalysisRequest m_arSample;
        CheckSum        m_csSampleChecksum;
	std::string m_strSampleStoragePathBase;
	CAVISClient* m_pClient;
	AnalysisStateInfo m_asi;

	// the analysis center cookie
	UINT m_uiLocalCookie;

	BOOL m_boSetImportedDate;

	CString m_strImportCompleteFilePath;
	CString m_strNewSampleDirectory;

public:
	CAVISTransactionImportSample(
		LPCSTR pszAgentName,
		LPCSTR pszGatewayURL,
		CAVISClient* pClient,
		UINT uiTimeoutTransactionConnected,
		UINT uiTimeoutTransactionCompleted,
		CTransactionTimeoutMonitorThread* pthreadTransactionTimeoutMonitor,
		BOOL boUseSSL);

	virtual ~CAVISTransactionImportSample();

public:
	AnalysisRequest GetAnalysisRequest();
        CheckSum        GetCheckSum();
	BOOL SampleWasAvailableAtGateway();
	UINT GetLocalCookie();
	BOOL NeedToSetImportedDate();
	void SetNeedToSetImportedDate(BOOL boNeedToSetImportedDate);

protected:
	DWORD InitRequest();
	virtual int ReadResponseContent();
	BOOL ProcessFilterStatus(Filter* pFilter);

private:
	int CreateSampleDirectory();
	void CreateImportCompleteFlagFile();
	int FeedContentToFilter(Filter* pFilter);
	void SaveSampleToFileServer(Filter* pFilter);
	//This function was added on Mar/16/2000.
	void SaveSampleAttributesToFileServer(char *szHeaders);
	//This method was added on May/16/2000.
	void RemoveSampleDirectory();
};

// GetSuspectSample return codes
#define GSS_SAMPLE_IMPORTED						GSS_RC_BASE + 1
#define GSS_SAMPLE_NOT_IMPORTED					GSS_RC_BASE + 2
#define GSS_NO_SAMPLES_AVAILABLE				GSS_RC_BASE + 3

#define IMPORT_COMPLETE_FILENAME				"import.complete"

#endif // !defined(AFX_AVISTRANSACTIONIMPORTSAMPLE_H__160D5FF6_9112_11D2_8A7C_00203529AC86__INCLUDED_)
