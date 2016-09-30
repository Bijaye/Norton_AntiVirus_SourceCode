// AVISGatewayManagerSampleComm.cpp: implementation of the CAVISGatewayManagerSampleComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISGatewayManagerSampleComm.h"
#include "AVISGatewaySampleComm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISGatewayManagerSampleComm::CAVISGatewayManagerSampleComm()
{
	CAVISACSampleCommApp::s_tc.enter("Entering CAVISGatewayManagerSampleComm constructor...");
	CAVISACSampleCommApp::s_tc.exit("Leaving CAVISGatewayManagerSampleComm constructor.");
}
/*****/
CAVISGatewayManagerSampleComm::~CAVISGatewayManagerSampleComm()
{
	CAVISACSampleCommApp::s_tc.enter("Entering CAVISGatewayManagerSampleComm destructor...");
	CAVISACSampleCommApp::s_tc.exit("Leaving CAVISGatewayManagerSampleComm destructor.");
}
/*****/
CAVISGatewayComm* CAVISGatewayManagerSampleComm::CreateGatewayComm(int i)
{
	CString strKeyName;
	std::string strGatewayURL;

	strKeyName.Format(GATEWAY_ADDRESS_BASE_KEYNAME "%u", i);
	strGatewayURL = CAVISProfile::GetStringValue((LPCTSTR )strKeyName);

	if (0 == strGatewayURL.length())
		return NULL;

	CAVISACSampleCommApp::s_tc.msg("Retrieved gateway URL for '%s' of '%s'.", strKeyName, strGatewayURL.c_str());

	// create a new CAVISGatewaySampleCommunications for the current gateway name
	return new CAVISGatewaySampleComm(strGatewayURL.c_str(), i);}
/*****/
int CAVISGatewayManagerSampleComm::GetNumGateways()
{
	// retrieve the number of gateways from the profile
	int iGatewayCount = CAVISProfile::GetIntValue(GATEWAY_COUNT_KEYNAME);

	if (iGatewayCount < 1)
	{
		CAVISACSampleCommApp::CriticalError("Must specify 'gatewayCount' of greater than zero.");
		return -1;
	}

	return iGatewayCount;
}
