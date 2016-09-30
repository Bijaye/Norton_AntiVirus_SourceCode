// AVISGatewayComm.h: interface for the CAVISGatewayComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISGATEWAY_H__E37E8AC6_E15E_11D2_8A97_00203529AC86__INCLUDED_)
#define AFX_AVISGATEWAY_H__E37E8AC6_E15E_11D2_8A97_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "impexp.h"
#include "AVISClient.h"
#include "Server.h"

class IMPEXP CAVISGatewayComm  
{
private:
	CString m_strURL;
	UINT m_uiIdGateway;
	CPtrList m_clientList;
	Server m_srvGateway;

public:
	CAVISGatewayComm(LPCSTR pszGatewayURL, UINT uiIdGateway);
	virtual ~CAVISGatewayComm();

public:
	BOOL StartAllClients();
	CString& GetURL();
	Server GetGateway();

protected:
	BOOL StartClient(CAVISClient* pClient);

private:
	virtual BOOL StartClients(LPCSTR pszGatewayURL, UINT uiIdGateway) = 0;
	void DestroyClients();
};

#endif // !defined(AFX_AVISGATEWAY_H__E37E8AC6_E15E_11D2_8A97_00203529AC86__INCLUDED_)
