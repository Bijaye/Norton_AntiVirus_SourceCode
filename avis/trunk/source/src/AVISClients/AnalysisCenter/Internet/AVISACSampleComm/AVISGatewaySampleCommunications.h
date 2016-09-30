// AVISGatewaySampleCommunications.h: interface for the CAVISGatewaySampleCommunications class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISGATEWAYSAMPLECOMMUNICATIONS_H__49143D97_B7CF_11D2_8A8F_00203529AC86__INCLUDED_)
#define AFX_AVISGATEWAYSAMPLECOMMUNICATIONS_H__49143D97_B7CF_11D2_8A8F_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISClientSampleImporter.h"
#include "AVISClientStatusReporter.h"

class CAVISGatewaySampleCommunications  
{
private:
	CAVISClientSampleImporter* m_pCsi;
	CAVISClientStatusReporter* m_pCsr;
	CString m_strGatewayURL;
	UINT m_uiIdGateway;

public:
	CAVISGatewaySampleCommunications(LPCSTR pszGatewayURL, UINT uiIdGateway);
	virtual ~CAVISGatewaySampleCommunications();

public:
	BOOL StopClients();
	BOOL StartClients();
	void Close();
};

#endif // !defined(AFX_AVISGATEWAYSAMPLECOMMUNICATIONS_H__49143D97_B7CF_11D2_8A8F_00203529AC86__INCLUDED_)
