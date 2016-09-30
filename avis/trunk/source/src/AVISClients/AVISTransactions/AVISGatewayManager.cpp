// AVISGatewayManager.cpp: implementation of the CAVISGatewayManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISTransactions.h"
#include "AVISGatewayManager.h"

#include "Server.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISGatewayManager::CAVISGatewayManager()
{
	CAVISTransactionsApp::s_pTc->enter("Entering CAVISGatewayManager constructor...");
	CAVISTransactionsApp::s_pTc->exit("Leaving CAVISGatewayManager constructor.");
}
/*****/
CAVISGatewayManager::~CAVISGatewayManager()
{
	CAVISTransactionsApp::s_pTc->enter("Entering CAVISGatewayManager destructor...");
	StopAllGatewayComm();
	CAVISTransactionsApp::s_pTc->exit("Leaving CAVISGatewayManager destructor.");
}
/*****/
BOOL CAVISGatewayManager::StartAllGatewayComm()
{
	CAVISTransactionsApp::s_pTc->msg("Launching clients for all gateways...");

	int iGatewayCount = GetNumGateways();

	if (iGatewayCount < 1)
		return FALSE;

	CAVISGatewayComm* pGatewayComm;

// don't need to catch exception, since they are all fatal; caller should catch fatal
// exceptions.
//		OR
// catch all exceptions and re-throw all caught types as a single custom type

//	try
//	{
		// lookup gateway address entries in the form of "gatewayAddress<n>", where
		// n starts at 1 and ends at iGatewayCount
		for (UINT i = 1; i <= iGatewayCount; i++)
		{
			// create a derived gateway communications object
			pGatewayComm = CreateGatewayComm(i);
			if (NULL == pGatewayComm)
			{
				CAVISTransactionsApp::s_pTc->critical("Failed creating gateway communications object.");
				return FALSE;
			}

			// add the newly created object to the gateway list
			m_gwList.AddTail(pGatewayComm);

			// Let the derived gateway communications object start its clients
			if (!pGatewayComm->StartAllClients())
			{
				CAVISTransactionsApp::s_pTc->critical("Failed starting clients for gateway (URL = %s).", pGatewayComm->GetURL());
				return FALSE;
			}
		}
/*	}
	catch (AVISDBException* pExcept)
	{
		AfxMessageBox("Caught exception in Server.");

		delete pExcept;
	}
*/
	CAVISTransactionsApp::s_pTc->msg("Launched clients for all gateways.");

	return TRUE;
}
/*****/
BOOL CAVISGatewayManager::StopAllGatewayComm()
{
	CAVISTransactionsApp::s_pTc->msg("Stopping all gateway clients...");

	CAVISGatewayComm* pGatewayComm;

	// stop all of the clients for all of the gateways
	for (POSITION pos = m_gwList.GetHeadPosition(); pos != NULL; )
	{
		// get the next gateway in the list
		pGatewayComm = (CAVISGatewayComm* )m_gwList.GetNext(pos);

		// delete the gateway object (the destructor for each gateway object will
		// automatically stop its clients).
		delete pGatewayComm;
	}

	// empty the list
	m_gwList.RemoveAll();

	CAVISTransactionsApp::s_pTc->msg("All gateway clients have been stopped.");

	return TRUE;
}
