// AVISGatewayManagerSignatureComm.h: interface for the CAVISGatewayManagerSignatureComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISGATEWAYMANAGERSIGNATURECOMM_H__E37E8AC7_E15E_11D2_8A97_00203529AC86__INCLUDED_)
#define AFX_AVISGATEWAYMANAGERSIGNATURECOMM_H__E37E8AC7_E15E_11D2_8A97_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISGatewayManager.h"
#include "AVISGatewayComm.h"

class CAVISGatewayManagerSignatureComm : public CAVISGatewayManager  
{
public:
	CAVISGatewayManagerSignatureComm();
	virtual ~CAVISGatewayManagerSignatureComm();

protected:
	CAVISGatewayComm* CreateGatewayComm(int i);
	int GetNumGateways();
};

#endif // !defined(AFX_AVISGATEWAYMANAGERSIGNATURECOMM_H__E37E8AC7_E15E_11D2_8A97_00203529AC86__INCLUDED_)
