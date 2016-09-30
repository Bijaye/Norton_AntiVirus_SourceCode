// AVISGatewayManager.h: interface for the CAVISGatewayManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATEWAYMANAGER_H__0A69E656_C75E_11D2_8A92_00203529AC86__INCLUDED_)
#define AFX_GATEWAYMANAGER_H__0A69E656_C75E_11D2_8A92_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "impexp.h"
#include "AVISGatewayComm.h"

class IMPEXP CAVISGatewayManager  
{
private:
	// list of gateways
	CPtrList m_gwList;

public:
	CAVISGatewayManager();
	virtual ~CAVISGatewayManager();

	BOOL StartAllGatewayComm();
	BOOL StopAllGatewayComm();

protected:
	virtual CAVISGatewayComm* CreateGatewayComm(int i) = 0;
	virtual int GetNumGateways() = 0;
};

#endif // !defined(AFX_GATEWAYMANAGER_H__0A69E656_C75E_11D2_8A92_00203529AC86__INCLUDED_)
