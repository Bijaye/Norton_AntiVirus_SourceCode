// AVISGatewaySampleCommunications.cpp: implementation of the CAVISGatewaySampleComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISGatewaySampleComm.h"
#include "AVISClientSampleImporter.h"
#include "AVISClientStatusReporter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISGatewaySampleComm::CAVISGatewaySampleComm(
	LPCSTR pszGatewayURL,
	UINT uiIdGateway)
  : CAVISGatewayComm(pszGatewayURL, uiIdGateway)
{
	CAVISACSampleCommApp::s_tc.enter("Entering CAVISGatewaySampleComm constructor...");
	CAVISACSampleCommApp::s_tc.exit("Leaving CAVISGatewaySampleComm constructor.");
}
/*****/
BOOL CAVISGatewaySampleComm::StartClients(LPCSTR pszGatewayURL, UINT uiIdGateway)
{
	BOOL boUseSSL = CAVISProfile::IsValueEnabled(
		SAMPLE_IMPORTER_USE_SSL_KEYNAME,
		SAMPLE_IMPORTER_USE_SSL_DEFAULT);

	CAVISClientStatusReporter* pCsr = new CAVISClientStatusReporter(
		pszGatewayURL, 
		uiIdGateway, 
		this,
		boUseSSL);

	if (pCsr == NULL)
		return FALSE;

	if (!StartClient(pCsr))
		return FALSE;

	CAVISClientSampleImporter* pCsi = new CAVISClientSampleImporter(
		pszGatewayURL, 
		uiIdGateway, 
		this, 
		boUseSSL);

	if (pCsi == NULL)
		return FALSE;

	if (!StartClient(pCsi))
		return FALSE;

	return TRUE;
}
/*****/
CAVISGatewaySampleComm::~CAVISGatewaySampleComm()
{
	CAVISACSampleCommApp::s_tc.enter("Entering CAVISGatewaySampleComm destructor...");
	CAVISACSampleCommApp::s_tc.exit("Leaving CAVISGatewaySampleComm destructor.");
}
