// AVISClientSignatureExporter.cpp: implementation of the CAVISClientSignatureExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <SystemException.h>
#include "AVISClientSignatureExporter.h"
#include "AVISException.h"
#include "AVISTransactionException.h"

// database headers
#include "AnalysisResults.h"
#include "Signature.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// can create a new transaction here. will be closed/deleted by base class destructor
CAVISClientSignatureExporter::CAVISClientSignatureExporter(
	LPCSTR pszGatewayURL,
	UINT uiIdGateway,
	CAVISGatewayComm* pGwc,
	BOOL boUseSSL)
  : CAVISClient(
		pszGatewayURL, 
		uiIdGateway, 
		SIGNATURE_EXPORTER_AGENT_NAME, 
		pGwc,
		boUseSSL),
    m_pTxES(NULL)
{
	CAVISACSignatureCommApp::s_tc.enter("Entering CAVISClientSignatureExporter constructor...");
	CAVISACSignatureCommApp::s_tc.exit("Leaving CAVISClientSignatureExporter constructor.");
}
/*****/
CAVISClientSignatureExporter::~CAVISClientSignatureExporter()
{
	CAVISACSignatureCommApp::s_tc.enter("Entering CAVISClientSignatureExporter destructor...");
	CAVISACSignatureCommApp::s_tc.exit("Leaving CAVISClientSignatureExporter destructor.");
}
/*****/
DWORD CAVISClientSignatureExporter::PostSignatureSetUntilSent(SigsToBeExported& stbe)
{
	//Added a new looping condition on May/02/2000. 
	//Use the Sleep function to check for the pending TM_QUIT message before looping.
	while (Sleep(60))
	{
		CAVISACSignatureCommApp::s_tc.debug("Attempting to post signature to gateway...");
		// must eventually get the signature sequence number from polling the database
		m_pTxES->SetSignatureSeqNum(stbe.SignatureID());
		DWORD dwResult = m_pTxES->Execute();
		CAVISACSignatureCommApp::s_tc.debug("Attempted to post signature to gateway.");

		switch(dwResult)
		{
			// No errors while sending signature
			case AVISTX_SUCCESS:
				CAVISACSignatureCommApp::s_tc.msg("Successfully sent signature to gateway.");
				return AVISTX_SUCCESS;

			// A non-critical network error occurred -- the gateway may be down, the
			// network itself may be broken, etc. Must wait a period of time until
			// trying again, until the network problem becomes resolved.
			case AVISTX_NETWORK_FAILURE:
				CAVISACSignatureCommApp::s_tc.warning("Network failure occurred. Waiting %d seconds to resend signature to gateway...", CAVISACSignatureCommApp::s_iClientDelayAfterNetworkErrorSecs);
				if (!Sleep(CAVISACSignatureCommApp::s_iClientDelayAfterNetworkErrorSecs * 1000))
					return APP_QUIT;
				break;

			case AVISTX_TERMINATE:
				CAVISACSignatureCommApp::s_tc.msg("Received a terminate request; Shutting down client...");
				return APP_QUIT;

			default:
				CAVISACSignatureCommApp::CriticalError("Unexpected result; exiting...");
				return APP_UNKNOWN_ERROR;
		}
	}

	return APP_QUIT;
}
/*****/
BOOL CAVISClientSignatureExporter::CreateTransactions(CString& strGatewayURL, BOOL boUseSSL)
{
	UINT uiTimeoutTransactionConnected = CAVISProfile::GetIntValue(
		TIMEOUT_TRANSACTION_CONNECTED_KEYNAME,
		TIMEOUT_TRANSACTION_CONNECTED_DEFAULT) * 1000;

	UINT uiTimeoutImportSampleTransactionCompleted = CAVISProfile::GetIntValue(
		TIMEOUT_EXPORT_SIGNATURE_TRANSACTION_COMPLETED_KEYNAME,
		TIMEOUT_EXPORT_SIGNATURE_TRANSACTION_COMPLETED_DEFAULT) * 1000;

	m_pTxES = new CAVISTransactionExportSignature(
		SIGNATURE_EXPORTER_AGENT_NAME, 
		(LPCSTR )strGatewayURL,
		uiTimeoutTransactionConnected,
		uiTimeoutImportSampleTransactionCompleted,
		GetTransactionTimeoutMonitorThread(),
		boUseSSL);

	if (m_pTxES == NULL)
		return FALSE;

	AddTransaction(m_pTxES);
	return TRUE;
}
/*****/
DWORD CAVISClientSignatureExporter::ClientMain()
{
	try
	{
		int iSignatureExporterPollingInterval = CAVISProfile::GetIntValue(
			SIGNATURE_EXPORTER_POLLING_INTERVAL_KEYNAME,
			SIGNATURE_EXPORTER_POLLING_INTERVAL_DEFAULT);

		// Empty object to be filled in with entry during GetOldestByServer()
		SigsToBeExported stbe;

		// get the server object for this gateway
		Server srvGateway = GetGateway();

		// keep looping until received a quit message
		//Added a new looping condition on May/02/2000. 
		//Use the Sleep function to check for the pending TM_QUIT message before looping.
		while (Sleep(60))
		{
			CAVISACSignatureCommApp::s_tc.msg("Checking database for signatures that need to be expored...");
			if (!SigsToBeExported::GetOldestByServer(stbe, srvGateway))
			{
				CAVISACSignatureCommApp::s_tc.msg("No signatures currently need to be reported; wating %d seconds to recheck...", iSignatureExporterPollingInterval);
				if (!Sleep(iSignatureExporterPollingInterval * 1000))
					break;

				continue;
			}
			CAVISACSignatureCommApp::s_tc.msg("Found signature that needs to be exported (seq num = %u.",stbe.SignatureID());

			if (APP_QUIT == PostSignatureSetUntilSent(stbe))
				return 0;

			// Signature Set was successfully sent to the gateway, so remove the signature
			// from the SigsToBeExported table (for the this gateway)
			CAVISACSignatureCommApp::s_tc.debug("Removing entry from \"SignaturesToBeExported\" database...");
			if (!stbe.RemoveFromDatabase())
			{
				CAVISACSignatureCommApp::s_tc.msg("Failed removing entry from \"SignaturesToBeExported\" database; exiting...");
				AfxGetApp()->PostThreadMessage(WM_QUIT, 0, 0);
				return -1;
			}
			CAVISACSignatureCommApp::s_tc.msg("Removed entry from \"SignaturesToBeExported\" database.");
		}

		return ERROR_SUCCESS;
	}
	catch (SystemException& except)
	{
		CAVISACSignatureCommApp::CriticalError("A SystemException exception occurred; desc = %s", except.TypeAsString().c_str());
	}
	catch(AVISException& except)
	{
		CAVISACSignatureCommApp::CriticalError(
			"A \"%s\" exception occured; type = %s, desc = %s", 
			except.ClassAsString().c_str(),
			except.TypeAsString().c_str(), 
			except.DetailedInfo().c_str());
	}
	catch(CInternetException* pExcept)
	{
		char szMsg[500];
		pExcept->GetErrorMessage(szMsg, sizeof(szMsg), NULL);
		CString strMsg;
		strMsg.Format("An Internet exception occured; err = %d, desc = %s", pExcept->m_dwError, szMsg);
		pExcept->Delete();

		CAVISACSignatureCommApp::CriticalError(strMsg);
	}
	catch(CException* pExcept)
	{
		char szMsg[500];
		pExcept->GetErrorMessage(szMsg, sizeof(szMsg), NULL);
		CString strMsg;
		strMsg.Format("An MFC exception occured; desc = %s", szMsg);
		pExcept->Delete();

		CAVISACSignatureCommApp::CriticalError(strMsg);
	}
	catch(CAVISTransactionException* pExcept)
	{
		CString strMsg;
		strMsg.Format("An AVISTransaction exception occured; desc = %s", pExcept->GetReasonText());
		delete pExcept;
		CAVISACSignatureCommApp::CriticalError(strMsg);
	}
	catch(exception* pExcept)
	{
		CAVISACSignatureCommApp::CriticalError("An exception from the Standard C++ library was caught; desc = %s", pExcept->what());
		delete pExcept;
	}
	catch (exception& except)
	{
		CAVISACSignatureCommApp::CriticalError("An exception from the Standard C++ library was caught; desc = %s", except.what());
	}
	catch(...)
	{
		CAVISACSignatureCommApp::CriticalError("An exception of unknown type occurred.");
	}

	// can't reach here
	return 0;
}
/*****/
BOOL CAVISClientSignatureExporter::End(DWORD, BOOL)
{
	return CAVISClient::End(SIGNATURE_EXPORTER_SHUTDOWN_TIMEOUT * 1000);
}
