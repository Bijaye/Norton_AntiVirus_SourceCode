// AVISTransactionExportSignature.h: interface for the CAVISTransactionExportSignature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISTRANSACTIONEXPORTSIGNATURE_H__160D5FF6_9112_11D2_8A7C_00203529AC86__INCLUDED_)
#define AFX_AVISTRANSACTIONEXPORTSIGNATURE_H__160D5FF6_9112_11D2_8A7C_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISTransaction.h"
#include "TransactionTimeoutMonitorThread.h"

class CAVISTransactionExportSignature : public CAVISTransaction  
{
private:
	std::string m_strSignatureStoragePathBase;
	CFile m_fileSignature;
	UINT m_uiSignatureSeqNum;
	UINT m_cbSignatureFile;

public:
	CAVISTransactionExportSignature(
		LPCSTR pszAgentName,
		LPCSTR pszGatewayURL,
		UINT uiTimeoutTransactionConnected,
		UINT uiTimeoutTransactionCompleted,
		CTransactionTimeoutMonitorThread* pthreadTransactionTimeoutMonitor,
		BOOL boUseSSL);

	virtual ~CAVISTransactionExportSignature();

public:
	void SetSignatureSeqNum(UINT uiSignatureSeqNum);

protected:
	DWORD InitRequest();
	int StartRequest(DWORD, PVOID);
	void EndRequest();
	int GetNextRequestContentBlock(PVOID pvRequestContentBlock);
};

// GetSuspectSample return codes
//#define GSS_IMPORTED_SAMPLE						GSS_RC_BASE + 1
//#define GSS_NO_SAMPLES_AVAILABLE				GSS_RC_BASE + 2
//#define GSS_UNABLE_TO_CREATE_SAMPLE_DIRECTORY	GSS_RC_BASE + 3
//#define GSS_FILTER_ERROR						GSS_RC_BASE + 4
//#define GSS_SAMPLE_ERROR						GSS_RC_BASE + 5
//#define GSS_ERROR_STORING_SAMPLE				GSS_RC_BASE + 6
//#define GSS_PIGGYBACKED_REQUEST					GSS_RC_BASE + 7

#endif // !defined(AFX_AVISTRANSACTIONEXPORTSIGNATURE_H__160D5FF6_9112_11D2_8A7C_00203529AC86__INCLUDED_)
