// AVISGatewaySignatureComm.h: interface for the CAVISGatewaySignatureComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISGATEWAYSAMPLECOMM_H__49143D97_B7CF_11D2_8A8F_00203529AC86__INCLUDED_)
#define AFX_AVISGATEWAYSAMPLECOMM_H__49143D97_B7CF_11D2_8A8F_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISGatewayComm.h"

class CAVISGatewaySignatureComm: public CAVISGatewayComm
{
public:
	CAVISGatewaySignatureComm(LPCSTR pszGatewayURL, UINT uiIdGateway);
	virtual ~CAVISGatewaySignatureComm();

protected:
	BOOL StartClients(LPCSTR pszGatewayURL, UINT uiIdGateway);
};

#endif // !defined(AFX_AVISGATEWAYSAMPLECOMM_H__49143D97_B7CF_11D2_8A8F_00203529AC86__INCLUDED_)
