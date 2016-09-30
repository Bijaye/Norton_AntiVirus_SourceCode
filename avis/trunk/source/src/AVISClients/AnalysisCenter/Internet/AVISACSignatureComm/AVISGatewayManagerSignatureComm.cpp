// AVISGatewayManagerSignatureComm.cpp: implementation of the CAVISGatewayManagerSignatureComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISGatewayManagerSignatureComm.h"
#include "AVISGatewaySignatureComm.h"
#include "Server.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISGatewayManagerSignatureComm::CAVISGatewayManagerSignatureComm()
{
	CAVISACSignatureCommApp::s_tc.enter("Entering CAVISGatewayManagerSignatureComm constructor...");

	// Set all gateways in Server table to "inactive".
	// As each gateway URL is read from the profile, an AVISGatewaySignatureComm object will
	// get created. That object will mark its associated gateway as "active".
	Server::ClearAllActive();
	CAVISACSignatureCommApp::s_tc.exit("Leaving CAVISGatewayManagerSignatureComm constructor.");
}
/*****/
CAVISGatewayManagerSignatureComm::~CAVISGatewayManagerSignatureComm()
{
	CAVISACSignatureCommApp::s_tc.enter("Entering CAVISGatewayManagerSignatureComm destructor...");
	CAVISACSignatureCommApp::s_tc.exit("Leaving CAVISGatewayManagerSignatureComm destructor.");
}
/*****/
CAVISGatewayComm* CAVISGatewayManagerSignatureComm::CreateGatewayComm(int i)
{
	CString strKeyName;
	std::string strGatewayURL;

	strKeyName.Format(GATEWAY_ADDRESS_BASE_KEYNAME "%u", i);
	strGatewayURL = CAVISProfile::GetStringValue((LPCTSTR )strKeyName);

	if (0 == strGatewayURL.length())
		return NULL;

	CAVISACSignatureCommApp::s_tc.msg("Retrieved gateway URL for '%s' of '%s'.", strKeyName, strGatewayURL.c_str());

	// create a new CAVISGatewaySignatureCommunications for the current gateway name
	return new CAVISGatewaySignatureComm(strGatewayURL.c_str(), i);
}
/*****/
int CAVISGatewayManagerSignatureComm::GetNumGateways()
{
	// retrieve the number of gateways from the profile
	int iGatewayCount = CAVISProfile::GetIntValue(GATEWAY_COUNT_KEYNAME);

	if (iGatewayCount < 1)
	{
		CAVISACSignatureCommApp::s_tc.critical("Must specify 'gatewayCount' of greater than zero.");
		return -1;
	}

	return iGatewayCount;
}
