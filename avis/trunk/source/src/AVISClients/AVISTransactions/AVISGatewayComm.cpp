// AVISGatewayComm.cpp: implementation of the CAVISGatewayComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "afxinet.h"
#include "AVISTransactions.h"
#include "AVISGatewayComm.h"

#include "DateTimeException.h"
#include "UtilException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISGatewayComm::CAVISGatewayComm(
	LPCSTR pszGatewayURL,
	UINT uiIdGateway)
 :	m_strURL(pszGatewayURL),
	m_uiIdGateway(uiIdGateway)
{
	CAVISTransactionsApp::s_pTc->enter("Entering CAVISGatewayComm constructor...");

// don't need to catch exception, since they are all fatal; caller should catch fatal
// exceptions.
//		OR
// catch all exceptions and re-throw all caught types as a single custom type

//	try
//	{
		DWORD dwServiceTypeDummy;
		CString strTargetDummy;
		INTERNET_PORT nPortDummy;

		CString strGatewayAddress;

		::AfxParseURL(
			pszGatewayURL,
			dwServiceTypeDummy,				// dont care (should be "http")
			strGatewayAddress, 
			strTargetDummy,					// should be empty
			nPortDummy);					// don't care

		// cast the gateway address
		std::string strNewGatewayAddress = (LPCSTR )strGatewayAddress;

		Server srv(strNewGatewayAddress);

		// automatically add the server to the server table if it is not found.
		if (srv.IsNull())
		{
			CAVISTransactionsApp::s_pTc->warning("Gateway address '%s' not found in database; attempting to add it...", (LPCSTR )strGatewayAddress);

			Server::AddNew(strNewGatewayAddress);
			m_srvGateway = Server(strNewGatewayAddress);

			// *** log that we added a new entry to the server table
			CAVISTransactionsApp::s_pTc->info("Added new gateway address '%s' to the database.", (LPCSTR )strGatewayAddress);
		}
		else
			m_srvGateway = srv;
//	}
/*
	catch (std::exception e)
	{
//		AfxMessageBox("Caught std::exception");
	}
	catch (std::exception* e)
	{
//		AfxMessageBox("Caught std::exception*");
	}
	catch (InvalidConfigVariable&)
	{
//		AfxMessageBox("Caught InvalidConfigVariable exception");
	}
	catch (int e)
	{
//		AfxMessageBox("Caught int exception");
	}
	catch (DateTimeException* pExcept)
	{
//		AfxMessageBox("Caught AVISDBException*");
	}
	catch (AVISDBException* pExcept)
	{
//		AfxMessageBox("Caught AVISDBException*");
	}
	catch (AVISDBException except)
	{
//		AfxMessageBox("Caught AVISDBException");
	}
	catch (...)
	{
//		AfxMessageBox("Caught unknown exception.");
	}
*/
	CAVISTransactionsApp::s_pTc->exit("Leaving CAVISGatewayComm constructor.");
}
/*****/
CAVISGatewayComm::~CAVISGatewayComm()
{
	CAVISTransactionsApp::s_pTc->enter("Entering CAVISGatewayComm destructor...");
	DestroyClients();
	CAVISTransactionsApp::s_pTc->exit("Leaving CAVISGatewayComm destructor.");
}
/*****/
BOOL CAVISGatewayComm::StartClient(CAVISClient* pClient)
{
	m_clientList.AddTail(pClient);

	CAVISTransactionsApp::s_pTc->msg("Starting \"%s\" client for gateway \"%s\"...", pClient->GetAgentName(), m_strURL);
	if (!pClient->Run())
	{
		CAVISTransactionsApp::s_pTc->critical("Failed running \"%s\" client.", pClient->GetAgentName());
		return FALSE;
	}
	CAVISTransactionsApp::s_pTc->msg("Started \"%s\" client for gateway \"%s\".", pClient->GetAgentName(), m_strURL);

	return TRUE;
}
/*****/
BOOL CAVISGatewayComm::StartAllClients()
{
	CAVISTransactionsApp::s_pTc->msg("Starting all clients for gateway \"%s\"...", m_strURL);

	TRY
	{
		// Let the derived gateway communications object create and add its clients
		// to the client list for this gateway
		if (!StartClients(m_strURL, m_uiIdGateway))
		{
			CAVISTransactionsApp::s_pTc->critical("Failed creating clients for gateway (URL = %s).", m_strURL);
			return FALSE;
		}
	}
	CATCH (CInternetException, pExcept)
	{
		char stError[512];
		pExcept->GetErrorMessage(stError,512);
//		TRACE("*** Error: failed creating an AVISClient object: %s", stError);

		// close any dynamically-created objects
		DestroyClients();			

		// any exceptions which reach this point will probably mean that the program
		// should terminate.

		// throw the caught exception up to the object which instantiated 'this' to 
		// deal with exceptions and terminate program.
		THROW_LAST();
	}
	END_CATCH

	CAVISTransactionsApp::s_pTc->msg("Started all clients for gateway '%s'.", m_strURL);

	return TRUE;
}
/*****/
void CAVISGatewayComm::DestroyClients()
{
	CAVISClient* pClient;

	// stop all of the clients for all of the gateways
	for (POSITION pos = m_clientList.GetHeadPosition(); pos != NULL; )
	{
		// get the next gateway in the list
		pClient = (CAVISClient* )m_clientList.GetNext(pos);

		CAVISTransactionsApp::s_pTc->msg("Stopping client...");
		pClient->End();
		CAVISTransactionsApp::s_pTc->msg("Stopped client.");

		delete pClient;
	}

	// empty the list
	m_clientList.RemoveAll();
}
/*****/
CString& CAVISGatewayComm::GetURL()
{
	return m_strURL;
}
/*****/
Server CAVISGatewayComm::GetGateway()
{
	return m_srvGateway;
}
