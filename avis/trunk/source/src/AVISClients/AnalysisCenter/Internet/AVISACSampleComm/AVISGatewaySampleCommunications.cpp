// AVISGatewaySampleCommunications.cpp: implementation of the CAVISGatewaySampleCommunications class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISACSampleComm.h"
#include "AVISGatewaySampleCommunications.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISGatewaySampleCommunications::CAVISGatewaySampleCommunications(
	LPCSTR pszGatewayURL,
	UINT uiIdGateway)
  : m_pCsr(NULL),
	m_pCsi(NULL),
	m_strGatewayURL(pszGatewayURL),
	m_uiIdGateway(uiIdGateway)
{
	CAVISACSampleCommApp::s_tc.enter("Entering CAVISGatewaySampleCommunications constructor...");

	TRY
	{
		m_pCsr = new CAVISClientStatusReporter(pszGatewayURL, uiIdGateway);
		m_pCsi = new CAVISClientSampleImporter(pszGatewayURL, uiIdGateway);

	} CATCH (CInternetException, pExcept)
	{
		char stError[512];
		pExcept->GetErrorMessage(stError,512);
		TRACE("*** Error: failed creating an AVISClient object: %s", stError);

		// close any dynamically-created objects
		Close();			

		// any exceptions which reach this point will probably mean that the program
		// should terminate.

		// throw the caught exception up to the object which instantiated 'this' to 
		// deal with exceptions and terminate program.
		THROW_LAST();
	}
	END_CATCH

	CAVISACSampleCommApp::s_tc.exit("Leaving CAVISGatewaySampleCommunications constructor.");
}
/*****/
CAVISGatewaySampleCommunications::~CAVISGatewaySampleCommunications()
{
	CAVISACSampleCommApp::s_tc.enter("Entering CAVISGatewaySampleCommunications destructor...");
	Close();
	CAVISACSampleCommApp::s_tc.exit("Leaving CAVISGatewaySampleCommunications destructor.");
}
/*****/
BOOL CAVISGatewaySampleCommunications::StartClients()
{

	CAVISACSampleCommApp::s_tc.msg("Starting all clients for gateway \"%s\"...", m_strGatewayURL);

	// run the sample importer client thread
	if (m_pCsi != NULL)
	{
		CAVISACSampleCommApp::s_tc.msg("Starting Sample Importer client for gateway \"%s\"...", m_strGatewayURL);

		if (!m_pCsi->Run())
		{
			CAVISACSampleCommApp::s_tc.critical("Failed running Sample Importer client.");
			return FALSE;
		}
		CAVISACSampleCommApp::s_tc.msg("Started Sample Importer client for gateway \"%s\".", m_strGatewayURL);
	}

	// run the status reporter client thread
	if (m_pCsr != NULL)
	{
		CAVISACSampleCommApp::s_tc.msg("Starting Status Reporter client for gateway \"%s\"...", m_strGatewayURL);
		if (!m_pCsr->Run())
		{
			CAVISACSampleCommApp::s_tc.critical("Failed running Status Reporter client.");
			return FALSE;
		}
		CAVISACSampleCommApp::s_tc.msg("Started Status Reporter client for gateway \"%s\".", m_strGatewayURL);
	}
	CAVISACSampleCommApp::s_tc.msg("Started all clients for gateway '%s'.", m_strGatewayURL);

	return TRUE;
}
/*****/
BOOL CAVISGatewaySampleCommunications::StopClients()
{
	// no need to do anything if the clients are already stopped
	if ((m_pCsi == NULL) && (m_pCsr == NULL))
		return TRUE;

	CAVISACSampleCommApp::s_tc.msg("Stopping all clients for gateway '%s'...", m_strGatewayURL);

	// stop the sample importer client thread, timeout after 2 seconds
	if (m_pCsi != NULL)
	{
		CAVISACSampleCommApp::s_tc.msg("Stopping Sample Importer client...");
		m_pCsi->End(SAMPLE_IMPORTER_SHUTDOWN_TIMEOUT * 1000);
		CAVISACSampleCommApp::s_tc.msg("Stopped Sample Importer client.");
	}

	// stop the status reporter client thread, timeout after 2 seconds
	if (m_pCsr != NULL)
	{
		CAVISACSampleCommApp::s_tc.msg("Stopping Status Reporter client...");
		m_pCsr->End(STATUS_REPORTER_SHUTDOWN_TIMEOUT * 1000);
		CAVISACSampleCommApp::s_tc.msg("Stopped Status Reporter client.");
	}

	CAVISACSampleCommApp::s_tc.msg("Stopped all clients for gateway '%s'...", m_strGatewayURL);

	return TRUE;
}
/*****/
void CAVISGatewaySampleCommunications::Close()
{
	StopClients();

	if (m_pCsr != NULL)
	{
		delete m_pCsr;
		m_pCsr = NULL;
	}

	if (m_pCsi != NULL)
	{
		delete m_pCsi;
		m_pCsi = NULL;
	}
}
