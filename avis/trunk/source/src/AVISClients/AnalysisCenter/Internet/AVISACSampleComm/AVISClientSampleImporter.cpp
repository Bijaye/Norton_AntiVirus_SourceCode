// AVISClientSampleImporter.cpp: implementation of the CAVISClientSampleImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxmt.h>
#include <SystemException.h>
#include "AVISClientSampleImporter.h"
#include "FilterException.h"
#include "AVISTransactionException.h"
#include "analysisresults.h"
#include "analysisrequest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISClientSampleImporter::CAVISClientSampleImporter(
	LPCSTR pszGatewayURL, 
	UINT uiIdGateway,
	CAVISGatewayComm* pGwc,
	BOOL boUseSSL)
  : CAVISClient(
		pszGatewayURL, 
		uiIdGateway, 
		SAMPLE_IMPORTER_AGENT_NAME, 
		pGwc,
		boUseSSL),
    m_pTxIS(NULL),
    m_pTxRS(NULL)
{
	CAVISACSampleCommApp::s_tc.enter("Entering CAVISClientSampleImporter constructor...");
	CAVISACSampleCommApp::s_tc.exit("Leaving CAVISClientSampleImporter constructor.");
}
/*****/
CAVISClientSampleImporter::~CAVISClientSampleImporter()
{
	CAVISACSampleCommApp::s_tc.enter("Entering CAVISClientSampleImporter destructor...");
	CAVISACSampleCommApp::s_tc.exit("Leaving CAVISClientSampleImporter destructor.");
}
/*****/
DWORD CAVISClientSampleImporter::GetSuspectSample()
{
	//Added the following mutex on Mar/16/2000.
	//This will prevent multiple threads from entering this function.
	//If we don't do this, it might cause racing conditions on 2 threads trying to
	//submit the same sample at exactly the same time.
	CMutex Mutex(FALSE, "AVISClientSampleImporterGetSuspectSample");
    CSingleLock sLock (&Mutex);
	//If it can't get the lock, it will wait infinitely until the lock is released by
	//the other thread.
	sLock.Lock();

	CAVISACSampleCommApp::s_tc.msg("Running GetSuspectSample operation...");

	// clear 'em out
	m_pTxIS->EmptyRequestHeaders();
	CAVISACSampleCommApp::s_tc.debug("Emptied request headers.");

	// add required headers for "getSuspectSample" transaction
	m_pTxIS->AddRequestHeader(IMPORT_SAMPLE_TX_REQUIRED_HEADERS);
	CAVISACSampleCommApp::s_tc.debug("Added required constant headers.");

	// dont need to send any content.
	// execute the transaction; handle errors
	DWORD rc = m_pTxIS->Execute();

	// if the transaction ran successfully, but there was no content, 
	// then no samples were imported
	if ((AVISTX_SUCCESS == rc) && (!m_pTxIS->SampleWasAvailableAtGateway()))
		rc = GSS_NO_SAMPLES_AVAILABLE;

	CAVISACSampleCommApp::s_tc.msg("GetSuspectSample has completed.");

	//Release the Mutex lock.
	sLock.Unlock();
	return rc;
}
/*****/
DWORD CAVISClientSampleImporter::ReportInitialStatusUntilSent(AnalysisRequest& ar, BOOL boRecovering)
{
	int iCookie = ar.LocalCookie();
	int boNeedToSetImportedDate = FALSE;

	// only need to mark the INI file if we are not recovering
	if (!boRecovering)
	{
		AfxGetApp()->WriteProfileInt("UnreportedInitialStatus", m_pTxIS->GetGatewayAddress(), iCookie);
		AfxGetApp()->WriteProfileInt("NeedToSetImportedDate", m_pTxIS->GetGatewayAddress(), m_pTxIS->NeedToSetImportedDate());
		CAVISACSampleCommApp::s_tc.debug("Marked INI file with cookie (id = %d) as having UNreported status.", iCookie);
	}
	else
	{
		boNeedToSetImportedDate = AfxGetApp()->GetProfileInt("NeedToSetImportedDate", m_pTxIS->GetGatewayAddress(), -1);
		m_pTxIS->SetNeedToSetImportedDate(boNeedToSetImportedDate);
	}

	// send out initial status for the current sample
	CAVISACSampleCommApp::s_tc.msg("Posting initial status to gateway...");
	DWORD rc = CAVISClientStatusReporter::ReportStatusUntilSent(ar, m_pTxRS);

	if (PSS_SENT_STATUS == rc)
	{
		CAVISACSampleCommApp::s_tc.msg("Successfully posted initial status to gateway.");

		// delete the entry for this gateway
		AfxGetApp()->WriteProfileString("UnreportedInitialStatus", m_pTxIS->GetGatewayAddress(), NULL);
		AfxGetApp()->WriteProfileString("NeedToSetImportedDate", m_pTxIS->GetGatewayAddress(), NULL);
		CAVISACSampleCommApp::s_tc.debug("Marked INI file with cookie (id = %d) as having REPORTED status.", iCookie);

		if (m_pTxIS->NeedToSetImportedDate())
		{
			// We do NOT want the Status Reporter to report INITIAL status for new samples.
			// The Sample Importer will be responsible for sending out initial status for
			// new samples. The sample importer will not attempt to retrieve another sample 
			// from the gateway until it successfully sends the initial status for the 
			// current sample (allows the gateway to remove the current sample from its 
			// output queue). To ensure that the status reporter does not attempt to send 
			// out this initial status, we must mark the 'notify client' field to 'false'.
			// This is necessary, because the "notify client" field is automatically set to
			// "true" whenever the state is changed (as above), which would trigger the
			// Status Reporter to try to send the initial status.
     		// We don't have to set NotifyClient to false, because it never gets set due to
		    // the new StatusToBeReported table. (Mar/21/2000)
			//ar.NotifyClient(false);
			//CAVISACSampleCommApp::s_tc.debug("Set the 'NotifyClient' flag to 'false' for the current analysis request.");

			// Update the analysis request's "imported" date. The status reporter will NOT 
			// attempt to send out status for samples who have NOT been fully received and
			// stored in the analysis center (i.e. have a non-null "imported" date/time).
			// The Imported() method does not change the status, it only sets the 
			// "imported" date/time field.

			// *** This step MUST be done after the ar.NotifyClient(false) above. This ensures
			// that the "NotifyClient" flag is not "true" while there is a non-null "imported"
			// date/time; otherwise, the Status Reporter client might be triggered to report
			// the status also.
			DateTime now;

			CAVISACSampleCommApp::s_tc.debug("Sample was imported; marking the \"Imported date/time\" as \"now\".");
			ar.Imported(now);
		}
	}

	return rc;
}
/*****/
BOOL CAVISClientSampleImporter::CreateTransactions(CString& strGatewayURL, BOOL boUseSSL)
{
	UINT uiTimeoutTransactionConnected = CAVISProfile::GetIntValue(
		TIMEOUT_TRANSACTION_CONNECTED_KEYNAME,
		TIMEOUT_TRANSACTION_CONNECTED_DEFAULT) * 1000;

	UINT uiTimeoutImportSampleTransactionCompleted = CAVISProfile::GetIntValue(
		TIMEOUT_IMPORT_SAMPLE_TRANSACTION_COMPLETED_KEYNAME,
		TIMEOUT_IMPORT_SAMPLE_TRANSACTION_COMPLETED_DEFAULT) * 1000;

	UINT uiTimeoutReportStatusTransactionCompleted = CAVISProfile::GetIntValue(
		TIMEOUT_REPORT_STATUS_TRANSACTION_COMPLETED_KEYNAME,
		TIMEOUT_REPORT_STATUS_TRANSACTION_COMPLETED_DEFAULT) * 1000;

	// add "import sample" transaction
	m_pTxIS = new CAVISTransactionImportSample(
		SAMPLE_IMPORTER_AGENT_NAME, 
		strGatewayURL,
		this,
		uiTimeoutTransactionConnected,
		uiTimeoutImportSampleTransactionCompleted,
		GetTransactionTimeoutMonitorThread(),
		boUseSSL);

	if (m_pTxIS == NULL)
		return FALSE;

	AddTransaction(m_pTxIS);

	// add "report status" transaction
	m_pTxRS = new CAVISTransactionReportStatus(
		INITIAL_STATUS_AGENT_NAME, 
		strGatewayURL,
		uiTimeoutTransactionConnected,
		uiTimeoutReportStatusTransactionCompleted,
		GetTransactionTimeoutMonitorThread(),
		boUseSSL);

	if (m_pTxRS == NULL)
		return FALSE;

	AddTransaction(m_pTxRS);

	return TRUE;
}
/*****/
DWORD CAVISClientSampleImporter::ClientMain()
{
	try
	{
		//Delete the samples that are left in receiving state during the 
		//previous session of this program.
		CleanupIncompleteImportedSamples();

		int iDelayNoSamplesAvailableSecs = CAVISProfile::GetIntValue(
			SAMPLE_IMPORTER_DELAY_AFTER_NO_SAMPLES_AT_GATEWAY_KEYNAME,
			SAMPLE_IMPORTER_DELAY_AFTER_NO_SAMPLES_AT_GATEWAY_DEFAULT);

		// Must check to see if there is an unreported initial status for the last sample
		// imported.
		int iUnreportedCookie = AfxGetApp()->GetProfileInt("UnreportedInitialStatus", m_pTxIS->GetGatewayAddress(), 0);

		// if iUnreportedCookie is 0, then the last sample processed had its initial 
		// status report successfully sent to the gateway; otherwise we might need to 
		// send an initial status report for that sample.
		if (iUnreportedCookie != 0) 
		{
			CAVISACSampleCommApp::s_tc.msg("Found a sample (cookie = %d) which was imported, but whose initial status was never sent; Sending unreported initial status...", iUnreportedCookie);

			if (APP_QUIT == ReportInitialStatusUntilSent(AnalysisRequest(iUnreportedCookie), TRUE))
				return ERROR_SUCCESS;
		}

		bool bExitClientMain = FALSE;

		//Added a new looping condition on May/02/2000. 
		//Use the Sleep function to check for the pending TM_QUIT message before looping.
		while ((bExitClientMain == FALSE) && Sleep(60))
		{
			CAVISACSampleCommApp::s_tc.msg("Checking for new samples at gateway...");
			switch(GetSuspectSample())
			{
				// successfully imported a new sample to analysis center
				case GSS_SAMPLE_IMPORTED:
					CAVISACSampleCommApp::s_tc.debug("Sample was imported.");

					// Since we have successfully received/imported the sample, we must 
					// send the initial status report to the gateway, so that the gateway 
					// will not give us the same sample the next time we do a GetSuspectSample.
					if (APP_QUIT == ReportInitialStatusUntilSent(m_pTxIS->GetAnalysisRequest()))
                        bExitClientMain = TRUE;

					// *** must check for other critical error ***
					break;

				// for one reason or another, we've determined that the sample does
				// not need to be imported (duplicate?)
				case GSS_SAMPLE_NOT_IMPORTED:
					CAVISACSampleCommApp::s_tc.msg("No need to import sample.");

					// send out initial status for the current sample
					// don't try to get another sample until initial status for this
					// sample has been sent to the gateway
                                        if (m_pTxIS->GetLocalCookie() == 0)
                                          {
                                            // If the cookie of the incoming sample is zero, then we have achieved FINAL status
                                            // by way of a CheckSum compare with the AnalysisResults table.  
                                            // NO COOKIE OR ANALYSISREQUEST ROW EXISTS!!! Post status using CheckSum
                                            // Create an AnalysisResults object with the sample checksum
                                            AnalysisResults results(m_pTxIS->GetCheckSum());
                                            // Post status
                                            if (APP_QUIT == ReportFinalStatusUntilSent(results))
                                                bExitClientMain = TRUE;
                                          }
                                        else
                                          {
                                            if (APP_QUIT == ReportInitialStatusUntilSent(m_pTxIS->GetAnalysisRequest()))
                                                bExitClientMain = TRUE;
                                          }
					// *** must check for other critical error ***
					break;


				// The gateway responded by indicating that it had no samples to give to us.
				// Just wait for a period of time until checking again.
				case GSS_NO_SAMPLES_AVAILABLE:
					CAVISACSampleCommApp::s_tc.msg("No samples were available at the gateway; Wating %d seconds to check for new samples...", iDelayNoSamplesAvailableSecs);
					if (!Sleep(iDelayNoSamplesAvailableSecs * 1000))
                        bExitClientMain = TRUE;
					break;

				// A non-critical network error occurred -- the gateway may be down, the
				// network itself may be broken, etc. Must wait a period of time until
				// trying again, until the network problem becomes resolved. No initial
				// status needs to be sent.
				case AVISTX_NETWORK_FAILURE:
					CAVISACSampleCommApp::s_tc.warning("Network failure occurred. Waiting %d seconds to recheck for new samples on gateway...", CAVISACSampleCommApp::s_iClientDelayAfterNetworkErrorSecs);
					if (!Sleep(CAVISACSampleCommApp::s_iClientDelayAfterNetworkErrorSecs * 1000))
                        bExitClientMain = TRUE;
					break;

				case AVISTX_TERMINATE:
					CAVISACSampleCommApp::s_tc.msg("Received a terminate request; shutting down client...");
                    bExitClientMain = TRUE;
					break;

				default:
					CAVISACSampleCommApp::CriticalError("Unexpected result from GetSuspectSample(); exiting...");
                    bExitClientMain = TRUE;
					break;
			}
		}

		//Delete the samples that are left in receiving state before we 
		//exit from this thread.
		CleanupIncompleteImportedSamples();
		return AVISTX_SUCCESS;
	}
	catch(AVISException& except)
	{
		CAVISACSampleCommApp::CriticalError(
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

		CAVISACSampleCommApp::CriticalError(strMsg);
	}
	catch(CException* pExcept)
	{
		char szMsg[500];
		pExcept->GetErrorMessage(szMsg, sizeof(szMsg), NULL);
		pExcept->Delete();

		CAVISACSampleCommApp::CriticalError("An MFC exception occured; desc = %s", szMsg);
	}
//	catch(CAVISTransactionImportSampleException* pExcept)
//	{
//		CAVISACSampleCommApp::CriticalError("An AVISTransactionImportSample exception occured; desc = %s", pExcept->GetReasonText());
//		delete pExcept;
//	}
	catch(CAVISTransactionException* pExcept)
	{
		CString strMsg;
		strMsg.Format("An AVISTransaction exception occured; desc = %s", pExcept->GetReasonText());
		delete pExcept;
		CAVISACSampleCommApp::CriticalError(strMsg);
	}
	catch(exception* pExcept)
	{
		CAVISACSampleCommApp::CriticalError("An exception from the Standard C++ library was caught; desc = %s", pExcept->what());
		delete pExcept;
	}
	catch (exception& except)
	{
		CAVISACSampleCommApp::CriticalError("An exception from the Standard C++ library was caught; desc = %s", except.what());
	}
	catch(...)
	{
		CAVISACSampleCommApp::CriticalError("An exception of unknown type occurred.");
	}

	// can't reach here
	return 0;
}
/*****/
DWORD CAVISClientSampleImporter::ReportFinalStatusUntilSent(AnalysisResults& results)
{
  
  // send out initial status for the current sample
  CAVISACSampleCommApp::s_tc.msg("Posting final state as the initial status to gateway...");
  DWORD rc = CAVISClientStatusReporter::ReportFinalStatusUntilSent(results, m_pTxRS);
  
  if (PSS_SENT_STATUS == rc)
    {
      CAVISACSampleCommApp::s_tc.msg("Successfully posted final state as the initial status to gateway.");
    }
  return rc;
}
/*****/
BOOL CAVISClientSampleImporter::End(DWORD, BOOL)
{
	return CAVISClient::End(SAMPLE_IMPORTER_SHUTDOWN_TIMEOUT * 1000);
}
/*****/
/*
This function was added on May/03/2000. If the user closes this application in between a
sample import operation, it may sometimes leave the sample in a state called 'receiving'.
This state indicates that we have not received the sample fully. We have to clear all the
samples in such a state from the database, before we exit the application. This function
performs that operation.
*/
void CAVISClientSampleImporter::CleanupIncompleteImportedSamples()
{
	Server srvGateway = GetGateway();
	AnalysisRequest::DeleteSamplesInReceivingState(srvGateway);
}
