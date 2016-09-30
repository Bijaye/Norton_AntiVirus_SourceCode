// AVISClientStatusReporter.cpp: implementation of the CAVISClientStatusReporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <SystemException.h>
#include "AVISClientStatusReporter.h"
#include "FilterException.h"
#include "AVISTransactionException.h"
#include "UtilException.h"
#include "AttributeKeys.h"

// database headers
#include "AnalysisResults.h"
#include "AnalysisRequest.h"
#include "StatusToBeReported.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// can create a new transaction here. will be closed/deleted by base class destructor
CAVISClientStatusReporter::CAVISClientStatusReporter(
                                                     LPCSTR pszGatewayURL,
                                                     UINT uiIdGateway,
                                                     CAVISGatewayComm* pGwc,
                                                     BOOL boUseSSL)
  : CAVISClient(
		pszGatewayURL, 
		uiIdGateway, 
		STATUS_REPORTER_AGENT_NAME,
		pGwc,
		boUseSSL),
  m_pTxRS(NULL)
{
  CAVISACSampleCommApp::s_tc.enter("Entering CAVISClientStatusReporter constructor...");
  CAVISACSampleCommApp::s_tc.exit("Leaving CAVISClientStatusReporter constructor.");
}
/*****/
CAVISClientStatusReporter::~CAVISClientStatusReporter()
{
  CAVISACSampleCommApp::s_tc.enter("Entering CAVISClientStatusReporter destructor...");
  CAVISACSampleCommApp::s_tc.exit("Leaving CAVISClientStatusReporter destructor.");
}
/*****/
DWORD CAVISClientStatusReporter::ReportStatusUntilSent(
                                                       AnalysisRequest& ar,
                                                       CAVISTransactionReportStatus* pTxRS)
{
	//Added a new looping condition on May/02/2000. 
	//Use the Sleep function to check for the pending TM_QUIT message before looping.
    while (Sleep(60)) 
    {
      CAVISACSampleCommApp::s_tc.debug("Attempting to post status to gateway...");
      DWORD dwResult = TryPostSampleStatus(ar, pTxRS);
      CAVISACSampleCommApp::s_tc.debug("Attempted to post status to gateway.");

      switch(dwResult)
        {
          // No errors while sending status
        case PSS_SENT_STATUS:
          CAVISACSampleCommApp::s_tc.msg("Successfully sent status to gateway.");
				// just go ahead and look for the next unreported status...
          return PSS_SENT_STATUS;

          // A non-critical network error occurred -- the gateway may be down, the
          // network itself may be broken, etc. Must wait a period of time until
          // trying again, until the network problem becomes resolved. No initial
          // status needs to be sent.
        case AVISTX_NETWORK_FAILURE:
          CAVISACSampleCommApp::s_tc.warning("Network failure occurred. Waiting %d seconds to resend status to gateway...", CAVISACSampleCommApp::s_iClientDelayAfterNetworkErrorSecs);
          if (!Sleep(CAVISACSampleCommApp::s_iClientDelayAfterNetworkErrorSecs * 1000))
            return APP_QUIT;
          break;

        case AVISTX_TERMINATE:
          CAVISACSampleCommApp::s_tc.msg("Received a terminate request; Shutting down client...");
          return APP_QUIT;

        case PSS_GATEWAY_REJECTED_STATUS:
				// any other return codes indicate that something went wrong
          CAVISACSampleCommApp::CriticalError("Gateway did not accept status posted to it; exiting...");
          return PSS_GATEWAY_REJECTED_STATUS;

        default:
          CAVISACSampleCommApp::CriticalError("Unexpected result from PostSampleStatus(); exiting...");
          return APP_UNKNOWN_ERROR;
        }
    }

  return APP_QUIT;
}
/*****/
DWORD CAVISClientStatusReporter::TryPostSampleStatus(
                                                     AnalysisRequest& ar,
                                                     CAVISTransactionReportStatus* pTxRS)
{
  ushort oldState;
  DateTime datetimeFinished;

  do
    {
      // remove all request headers from previous transaction
      pTxRS->EmptyRequestHeaders();
      CAVISACSampleCommApp::s_tc.debug("Emptied request headers.");

      // add required headers for "postSampleStatus" transaction
      pTxRS->AddRequestHeader(REPORT_STATUS_TX_REQUIRED_HEADERS);

      // set the analysis cookie header
      pTxRS->AddRequestHeader(AttributeKeys::AnalysisCookie(), ar.RemoteCookie());
      CAVISACSampleCommApp::s_tc.debug("Gateway analysis cookie = %d.", ar.RemoteCookie());

      // set the analysis state header
      std::string strState = ar.State();
      pTxRS->AddRequestHeader(AttributeKeys::AnalysisState(), strState);
      CAVISACSampleCommApp::s_tc.debug("Analysis state = %s.", strState.c_str());

      // set the X-error header, if present
//       std::string strXerror = ar.GetAttributes()[AttributeKeys::ErrorImport()];
//       if (!strXerror.empty())
//         {
//           pTxRS->AddRequestHeader(AttributeKeys::ErrorImport(), strXerror);
//           CAVISACSampleCommApp::s_tc.debug("%s = %s.", AttributeKeys::ErrorImport().c_str(), strXerror.c_str());
//         }

      // if the request has a non-null 'finished date/time' then the processing is
      // finished for the sample. Indicate this condition by adding the
      // "X-Date-Analyzed" header with the current date/time.
      datetimeFinished = ar.Finished();
      if (!datetimeFinished.IsNull())
        {
          std::string strDateTime;
          if (!datetimeFinished.AsHttpString(strDateTime))
            {
              CAVISACSampleCommApp::CriticalError("Failed formatting date/time string.");
              return AVISTX_TERMINATE;
            }

          pTxRS->AddRequestHeader(AttributeKeys::DateAnalyzed(), strDateTime);
          CAVISACSampleCommApp::s_tc.debug("Finished date = %s.", strDateTime.c_str());
        }

      // If there is an 'analysis results' record associated with the current
      // sample, get its signature sequence number (which may be zero if no 
      // signature is associated with this result), and include it in the 
      // "X-Signatures-Sequence" header.
      AnalysisResults analRes(ar.GetCheckSum());
      if (!analRes.IsNull())
        {
          UINT uiSigSeqNum = analRes.GetSignatureSequenceNum();
          pTxRS->AddRequestHeader(AttributeKeys::SignatureSequence(), uiSigSeqNum);
          CAVISACSampleCommApp::s_tc.debug("Signature sequence number = %d.", uiSigSeqNum);
        }

      std::string strCheckSum;
      CheckSum chkSum = ar.GetCheckSum();
      strCheckSum = chkSum;

      // add the sample checksum header in X-Sample-Checksum
      pTxRS->AddRequestHeader(AttributeKeys::SampleCheckSum(), strCheckSum);
      CAVISACSampleCommApp::s_tc.debug("Sample checksum = %s", strCheckSum.c_str());

      // execute the transaction
      CAVISACSampleCommApp::s_tc.msg("Reporting status for local cookie = %d to gateway...", ar.LocalCookie());
      DWORD rc = pTxRS->Execute();
      CAVISACSampleCommApp::s_tc.msg("Reported status for local cookie = %d to gateway.", ar.LocalCookie());

      if (rc != AVISTX_SUCCESS)
        {
          CAVISACSampleCommApp::s_tc.error("Error occurred while executing PostSampleStatus request.");
          return rc;
        }

      // if gateway didn't accept status, return
      if (!pTxRS->GatewayAcceptedStatus())
        return PSS_GATEWAY_REJECTED_STATUS;

      CAVISACSampleCommApp::s_tc.msg("Succesfully reported status to gateway.");

      // check if another process has changed the sample state since we sent it to
      // the gateway
      oldState = ar.State().ID();

      // indicate that the sample's current status has successfully been sent
      // to the appropriate gateway.
	  // We don't have to set NotifyClient to false, because it never gets set due to
	  // the new StatusToBeReported table. (Mar/21/2000)
      //ar.NotifyClient(false);
      //CAVISACSampleCommApp::s_tc.debug("Set the 'NotifyClient' flag to 'false'.");

      // Refresh the analysis request with the latest info from the database
      ar.Refresh();

      if (oldState != ar.State().ID())
	  {
		  CAVISACSampleCommApp::s_tc.debug("Resending the status since it changed during the previous post.");
	  }

    } while (oldState != ar.State().ID());	// keep sending status until it hasn't changed

  // if we sent out "final" status (had a "finished date"), then we must mark the
  // sample as having an "informed" date.
  if (!datetimeFinished.IsNull())
    {
      DateTime dtNow;

      ar.Informed(dtNow);
      CAVISACSampleCommApp::s_tc.debug("Marked request with final state as being \"informed\".");
    }

  return PSS_SENT_STATUS;
}
/*****/
/*****/
/*****/
DWORD CAVISClientStatusReporter::ReportFinalStatusUntilSent(
                                                            AnalysisResults& results,
                                                            CAVISTransactionReportStatus* pTxRS)
{
	//Added a new looping condition on May/02/2000. 
	//Use the Sleep function to check for the pending TM_QUIT message before looping.
    while (Sleep(60))
    {
      CAVISACSampleCommApp::s_tc.debug("Attempting to post final status to gateway...");
      DWORD dwResult = TryPostFinalStatus(results, pTxRS);
      CAVISACSampleCommApp::s_tc.debug("Attempted to post final status to gateway.");

      switch(dwResult)
        {
          // No errors while sending status
        case PSS_SENT_STATUS:
          CAVISACSampleCommApp::s_tc.msg("Successfully sent status to gateway.");
				// just go ahead and look for the next unreported status...
          return PSS_SENT_STATUS;

          // A non-critical network error occurred -- the gateway may be down, the
          // network itself may be broken, etc. Must wait a period of time until
          // trying again, until the network problem becomes resolved. No initial
          // status needs to be sent.
        case AVISTX_NETWORK_FAILURE:
          CAVISACSampleCommApp::s_tc.warning("Network failure occurred. Waiting %d seconds to resend status to gateway...", CAVISACSampleCommApp::s_iClientDelayAfterNetworkErrorSecs);
          if (!Sleep(CAVISACSampleCommApp::s_iClientDelayAfterNetworkErrorSecs * 1000))
            return APP_QUIT;
          break;

        case AVISTX_TERMINATE:
          CAVISACSampleCommApp::s_tc.msg("Received a terminate request; Shutting down client...");
          return APP_QUIT;

        case PSS_GATEWAY_REJECTED_STATUS:
				// any other return codes indicate that something went wrong
          CAVISACSampleCommApp::CriticalError("Gateway did not accept status posted to it; exiting...");
          return PSS_GATEWAY_REJECTED_STATUS;

        default:
          CAVISACSampleCommApp::CriticalError("Unexpected result from PostSampleStatus(); exiting...");
          return APP_UNKNOWN_ERROR;
        }
    }

  return APP_QUIT;
}
/*****/
DWORD CAVISClientStatusReporter::TryPostFinalStatus(
                                                    AnalysisResults& results,
                                                    CAVISTransactionReportStatus* pTxRS)
{
  DateTime datetimeFinished; // This is set to "now" by default

  // remove all request headers from previous transaction
  pTxRS->EmptyRequestHeaders();
  CAVISACSampleCommApp::s_tc.debug("Emptied request headers.");
  
  // add required headers for "postSampleStatus" transaction
  pTxRS->AddRequestHeader(REPORT_STATUS_TX_REQUIRED_HEADERS);
  
  // set the sample checksum
  std::string strChecksum = results.GetCheckSum();
  pTxRS->AddRequestHeader(AttributeKeys::SampleCheckSum(), results.GetCheckSum());
  CAVISACSampleCommApp::s_tc.debug("Sample Checksum = %s.", strChecksum.c_str());
  
  // set the analysis state header
  std::string strState = results.SampleState();
  pTxRS->AddRequestHeader(AttributeKeys::AnalysisState(), strState);
  CAVISACSampleCommApp::s_tc.debug("Analysis state = %s.", strState.c_str());
  
  // if the request has a non-null 'finished date/time' then the processing is
  // finished for the sample. Indicate this condition by adding the
  // "X-Date-Analyzed" header with the current date/time.
  std::string strDateTime;
  if (!datetimeFinished.AsHttpString(strDateTime))
    {
      CAVISACSampleCommApp::CriticalError("Failed formatting date/time string.");
      return AVISTX_TERMINATE;
    }
  
  pTxRS->AddRequestHeader(AttributeKeys::DateAnalyzed(), strDateTime);
  CAVISACSampleCommApp::s_tc.debug("Finished date = %s.", strDateTime.c_str());
  
  UINT uiSigSeqNum = results.GetSignatureSequenceNum();
  pTxRS->AddRequestHeader(AttributeKeys::SignatureSequence(), uiSigSeqNum);
  CAVISACSampleCommApp::s_tc.debug("Signature sequence number = %d.", uiSigSeqNum);
  
  // execute the transaction
  CAVISACSampleCommApp::s_tc.msg("Reporting final status for checksum = %s to gateway...", strChecksum.c_str());
  DWORD rc = pTxRS->Execute();
  CAVISACSampleCommApp::s_tc.msg("Reported final status for  checksum = %s to gateway.", strChecksum.c_str());
  
  if (rc != AVISTX_SUCCESS)
    {
      CAVISACSampleCommApp::s_tc.error("Error occurred while executing PostSampleStatus request.");
      return rc;
    }
  
  // if gateway didn't accept status, return
  if (!pTxRS->GatewayAcceptedStatus())
    return PSS_GATEWAY_REJECTED_STATUS;
  
  CAVISACSampleCommApp::s_tc.msg("Succesfully reported status to gateway.");
  
  // check if another process has changed the sample state since we sent it to
  // the gateway
  
  return PSS_SENT_STATUS;
}

/*****/
BOOL CAVISClientStatusReporter::CreateTransactions(CString& strGatewayURL, BOOL boUseSSL)
{
  UINT uiTimeoutTransactionConnected = CAVISProfile::GetIntValue(
                                                                 TIMEOUT_TRANSACTION_CONNECTED_KEYNAME,
                                                                 TIMEOUT_TRANSACTION_CONNECTED_DEFAULT) * 1000;

  UINT uiTimeoutReportStatusTransactionCompleted = CAVISProfile::GetIntValue(
                                                                             TIMEOUT_REPORT_STATUS_TRANSACTION_COMPLETED_KEYNAME,
                                                                             TIMEOUT_REPORT_STATUS_TRANSACTION_COMPLETED_DEFAULT) * 1000;

  m_pTxRS = new CAVISTransactionReportStatus(
                                             STATUS_REPORTER_AGENT_NAME, 
                                             (LPCSTR )strGatewayURL,
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
DWORD CAVISClientStatusReporter::ClientMain()
{
  try
    {
      int iStatusReporterPollingInterval = CAVISProfile::GetIntValue(
                                                                     STATUS_REPORTER_POLLING_INTERVAL_KEYNAME,
                                                                     STATUS_REPORTER_POLLING_INTERVAL_DEFAULT);

      // ignore the "crumbled" value in the "X-Error" header if configured to do so
      // from the configuration profile.
      if (CAVISProfile::IsValueEnabled(
                                       IGNORE_XERROR_CRUMBLED_KEYNAME,
                                       IGNORE_XERROR_CRUMBLED_DEFAULT))
        m_pTxRS->SetIgnoreXErrorCrumbled();

      // get the server object for this gateway
      Server srvGateway = GetGateway();

	  // Empty object to be filled in with entry during GetOldestByServer() [Mar/21/2000]
	  StatusToBeReported stbr;

      // keep looping until received a quit message
	  //Added a new looping condition on May/02/2000. 
	  //Use the Sleep function to check for the pending TM_QUIT message before looping.
        while (Sleep(60))
        {
          // Query the database on the current gateway for samples whose status
          // is unreported. 

          // *** if no samples available for notification, wait, then check again later ***
          CAVISACSampleCommApp::s_tc.msg("Checking database for unreported status...");
		  //The following logic was changed on Mar/21/2000 to read the pending staus
		  //report items for a given gateway from the StatusToBeReported table in the 
		  //database.
		  //Clear the StatusToBeReported object.
		  stbr.Clear();
          if (!StatusToBeReported::GetOldestByServer(stbr, srvGateway))
            {
              CAVISACSampleCommApp::s_tc.msg("No status currently needs to be reported; wating %d seconds to recheck...", iStatusReporterPollingInterval);
              if (!Sleep(iStatusReporterPollingInterval * 1000))
                break;

              continue;
            }

		  CheckSum chkSum(std::string(""));

		  try
		  {
			//Get the checksum of the sample for which we have to report the status.
			chkSum = stbr.Checksum();
		  }
		  catch(AVISDBException& except)
		  {
             CAVISACSampleCommApp::s_tc.msg("A \"%s\" exception occured; type = %s, desc = %s", 
                                          except.ClassAsString().c_str(),
                                          except.TypeAsString().c_str(), 
                                          except.DetailedInfo().c_str());
			 stbr.RemoveFromDatabase();
			 continue;
		  }
	  
		  std::string strChecksum = chkSum;
		  CAVISACSampleCommApp::s_tc.msg("Found status which needs to be reported for checksum = '%s'.", strChecksum.c_str());
		  // create an analysis request using checksum for checking for unreported status
          AnalysisRequest	ar(chkSum);
          if (!ar.IsNull())
		  {
			CAVISACSampleCommApp::s_tc.msg("Reporting status using analysisrequest (cookie = %d).", ar.LocalCookie());

			if (APP_QUIT == ReportStatusUntilSent(ar, m_pTxRS))
				break;
          }
		  else
		  {
			  AnalysisResults results(chkSum);

			  if (!results.IsNull())
			  {
				  // Post status
				  CAVISACSampleCommApp::s_tc.msg("Reporting status using analysisresults.");
                  if (APP_QUIT == ReportFinalStatusUntilSent(results, m_pTxRS))
                      break;
			  }                                            
		  }

		  stbr.RemoveFromDatabase();
        }

      return ERROR_SUCCESS;
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
      strMsg.Format(
                    "An Internet exception occured; err = %d, desc = %s", 
                    pExcept->m_dwError, 
                    szMsg);
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
  catch(CAVISTransactionException* pExcept)
    {
      CString strMsg;
      strMsg.Format(
                    "An AVISTransaction exception occured; desc = %s", 
                    pExcept->GetReasonText());
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

/*****/
/*****/
BOOL CAVISClientStatusReporter::End(DWORD, BOOL)
{
  return CAVISClient::End(STATUS_REPORTER_SHUTDOWN_TIMEOUT * 1000);
}
