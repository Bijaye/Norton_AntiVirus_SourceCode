// AVISGatewaySignatureComm.cpp: implementation of the CAVISGatewaySignatureComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISGatewaySignatureComm.h"
#include "AVISClientSignatureExporter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISGatewaySignatureComm::CAVISGatewaySignatureComm(
	LPCSTR pszGatewayURL,
	UINT uiIdGateway)
  : CAVISGatewayComm(pszGatewayURL, uiIdGateway)
{
	CAVISACSignatureCommApp::s_tc.enter("Entering CAVISGatewaySignatureComm constructor...");

	// mark the gateway as active
	GetGateway().SetActive(true);
	CAVISACSignatureCommApp::s_tc.exit("Leaving CAVISGatewaySignatureComm constructor.");
}
/*****/
BOOL CAVISGatewaySignatureComm::StartClients(LPCSTR pszGatewayURL, UINT uiIdGateway)
{
	BOOL boUseSSL = CAVISProfile::IsValueEnabled(
		SIGNATURE_EXPORTER_USE_SSL_KEYNAME,
		SIGNATURE_EXPORTER_USE_SSL_DEFAULT);

	CAVISClientSignatureExporter* pCse = new CAVISClientSignatureExporter(
		pszGatewayURL, 
		uiIdGateway, 
		this, 
		boUseSSL);

	if (pCse == NULL)
		return FALSE;

	if (!StartClient(pCse))
		return FALSE;

	return TRUE;
}
/*****/
CAVISGatewaySignatureComm::~CAVISGatewaySignatureComm()
{
	CAVISACSignatureCommApp::s_tc.enter("Entering CAVISGatewaySignatureComm destructor...");
	CAVISACSignatureCommApp::s_tc.exit("Leaving CAVISGatewaySignatureComm destructor.");
}
