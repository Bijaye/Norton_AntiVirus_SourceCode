// AVISTransactionImportSample.cpp: implementation of the CAVISTransactionImportSample class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISTransactionImportSample.h"
#include "AnalysisRequest.h"
#include "AnalysisResults.h"
#include "AVISTransactionImportSampleException.h"
#include "AVISSendMail.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// new constructor!
CAVISTransactionImportSample::CAVISTransactionImportSample(
                                                           LPCSTR pszAgentName,
                                                           LPCSTR pszGatewayURL,
                                                           CAVISClient* pClient,
                                                           UINT uiTimeoutTransactionConnected,
                                                           UINT uiTimeoutTransactionCompleted,
                                                           CTransactionTimeoutMonitorThread* pthreadTransactionTimeoutMonitor,
                                                           BOOL boUseSSL)
  : CAVISTransaction(
                     pszAgentName,
                     pszGatewayURL,
                     CHttpConnection::HTTP_VERB_GET,
                     uiTimeoutTransactionConnected,
                     uiTimeoutTransactionCompleted,
                     pthreadTransactionTimeoutMonitor,
                     IMPORT_SAMPLE_SERVLET_NAME,
                     boUseSSL),
  m_csSampleChecksum(std::string("")),
  m_pClient(pClient),
  m_uiLocalCookie(0),
  m_boSetImportedDate(FALSE),
  m_asi(AnalysisStateInfo::UnknownSampleState())
{
  CAVISACSampleCommApp::s_tc.enter("Entering CAVISTransactionImportSample constructor...");

  m_strSampleStoragePathBase = CAVISProfile::GetStringValue(SAMPLE_STORAGE_PATH_KEYNAME);

  if ("" == m_strSampleStoragePathBase)
    {
      // This entry is required!
      throw new CAVISTransactionImportSampleException(
                                                      CAVISTransactionImportSampleException::noSampleStoragePathBase,
                                                      "The profile entry for the sample storage path was not present, and is required.");
    }
		
  m_strSampleStoragePathBase += "\\";

  CAVISACSampleCommApp::s_tc.exit("Leaving CAVISTransactionImportSample constructor.");
}
/*****/
CAVISTransactionImportSample::~CAVISTransactionImportSample()
{
  CAVISACSampleCommApp::s_tc.enter("Entering CAVISTransactionImportSample destructor...");
  CAVISACSampleCommApp::s_tc.exit("Leaving CAVISTransactionImportSample destructor.");
}
/*****/
BOOL CAVISTransactionImportSample::ProcessFilterStatus(Filter* pFilter)
{
  CAVISACSampleCommApp::s_tc.enter("Entering ProcessFilterStatus()...");

  // errors which should cause this process to terminate itself
  CString strCriticalErrorDescription;
  int iCriticalErrorCause = CAVISTransactionImportSampleException::noError;

  // flag which should be set to true if the filter state indicates that the 
  // analysis request has reached a final state. In this case the "informed date/time"
  // field should be set to "now"
  BOOL boFinalState = FALSE;

  // remember analysis state when entering
  AnalysisStateInfo asi_old = m_asi;
  CAVISACSampleCommApp::s_tc.debug("Current analysis state is \"%s\".", ((std::string )m_asi).c_str());

  // remember the current time/date
  DateTime now;

  // value for X-error header
  std::string strXErrorImport;

  BOOL boKeepSample = FALSE;

  CAVISACSampleCommApp::s_tc.debug("Current filter status is \"%s\".", pFilter->StatusAsString().c_str());

  switch (pFilter->Status())
    {
      // Filter needs more data to complete its filtering
    case Filter::NeedMoreData:
      // sample is being received from the gateway
      m_asi = AnalysisStateInfo::Receiving();

      // set boKeepSample to TRUE so that it doesn't look like an error, because
      // this boKeepSample represents this function's return value.
      boKeepSample = TRUE;
      break;

      // *** The following filter statuses indicate that the file will be imported
      // into the analysis center for automated analysis

    case Filter::ScanError:
    case Filter::Done_NAVoverrun:
    case Filter::Done_NoRepair:
    case Filter::Done_BadScan:
    case Filter::Done_BadRepair:
    case Filter::Done_UnRepairable:
      //		case Filter::Done_NeedsQuickLook:
    case Filter::Done_FoundByHeuristic:
    case Filter::Done_NeedsHarderLook:
    case Filter::Done_NoDetect:
      // If the state is not "importing", then this the first call when the
      // filter has this value; otherwise, this is the second call when the
      // filter has this value, and means that the caller has imported the
      // sample.
      if (m_asi != AnalysisStateInfo::Importing())
        {
				// When this method returns, the caller should save the sample to the 
				// file server. We set the state to "importing", indicating that we 
				// are about to save the file to the file server.
          m_asi = AnalysisStateInfo::Importing();
        }
      else
        {
				// We now set the state to "imported", indicating that the sample has
				// been stored on the file server.
          m_asi = AnalysisStateInfo::Imported();

				// we cannot actually mark the "imported date" until after initial
				// status has been sent, so mark this flag, so the client can take
				// care of marking the "imported date" after it has successfully sent
				// out the initial status.
          m_boSetImportedDate = TRUE;
        }

      boKeepSample = TRUE;
      break;

      // *** the following filter statuses indicate that the submitted sample is
      // a duplicate and will not be stored in the analysis center; however, it
      // will be linked to the original sample so that status reports will be
      // conistent among duplicates ***

	/*
	Added this particular case block on June/02/2000. 
	On June/01/2000, it was decided that the filter code (Filter.cpp)
	would return "Done_Misfired" when a sample was submitted hueristically and
	the scan of that file returns "Not_infected". This is a sample that
	was falsely detected as infected at the client. Under this situation,
	the filter will put the sample in the "Done_Misfired" state.
	That is	the reason why this case block was added.
    */
    case Filter::Done_Misfired:
      // This is a final state
      boFinalState = TRUE;

      // mark it as imported so it can be linked to future duplicates if it is
      // new, or so that it can be linked to the original, if it is a duplicate.
      m_boSetImportedDate = TRUE;
			
      // Mark the sample status as "misfired"
      m_asi = AnalysisStateInfo::Misfired();
      break;

    case Filter::Done_Infected:
      // We definitely know that its infected (may match a previously imported
      // sample *OR* received sample was scanned using latest or blessed 
      // signatures and found it to be infected). No further status changes will
      // occur on this request.

      // This is a final state
      boFinalState = TRUE;

      // mark it as imported so it can be linked to future duplicates if it is
      // new, or so that it can be linked to the original, if it is a duplicate.
      m_boSetImportedDate = TRUE;
			
      // Mark the sample status as "infected"
      m_asi = AnalysisStateInfo::Infected();
      break;

    case Filter::Done_UnInfected:
      // We definitely know that its NOT infected, because it matches a previously
      // imported sample which was manually-analyzed and determined to be clean.

      // This is a final state
      boFinalState = TRUE;

      // mark an imported date, in order to link the request to the original.
      m_boSetImportedDate = TRUE;

      // Mark the sample status as "uninfected"
      m_asi = AnalysisStateInfo::UnInfected();
      break;

    case Filter::Done_Piggybacked:
      // This sample has already been imported into the system through a
      // different request and is currently the process of being analyzed
      // (no final status has been reached yet). Don't need to set a status, 
      // since the "piggybacking" should automatically link the status of the 
      // current request to the original request for the sample.

      // Must mark an "imported" date to link the piggybacked request to the
      // original. The database will only link a piggybacked request to the
      // original if their checksums match and the new request has been imported.

      // we cannot actually mark the "imported date" until after initial
      // status has been sent, so mark this flag, so the client can take
      // care of marking the "imported date" when it has successfully sent
      // out the initial status.
      m_boSetImportedDate = TRUE;

      CAVISACSampleCommApp::s_tc.warning("Attempt was made to receive a sample which is a duplicate of a sample in progress (piggybacked the request).");
      break;

      // *** the following filter statuses indicate that the submitted sample is
      // not a candidate for import ***
    case Filter::Done_NotInfectable:
      // File cannot be infected (i.e. is a data file which doesn't use macros).
      m_asi = AnalysisStateInfo::UnInfectible();

      // This is a final state
      boFinalState = TRUE;

      // mark the imported date after initial status has been sent
      m_boSetImportedDate = TRUE;
      break;

    case Filter::Done_UnSubmittable:
      m_asi = AnalysisStateInfo::UnSubmitable();

      boFinalState = TRUE;

      m_boSetImportedDate = TRUE;
      break;

      // The following filter statuses indicate that something was wrong with the
      // sample. In each case, the sample will not be imported into the analysis
      // center.  Initial status must still be sent back to the gateway, so that
      // the same sample will not be resent when we ask it for the next sample. In
      // addition, the "finished date" will be set in each case
    case Filter::BadContentCheckSum:
      boFinalState = TRUE;
      m_asi = AnalysisStateInfo::BadContentCheckSum();
      // should provde X-error as "content <actual checksum> <expected checksum>"
      strXErrorImport = "content";
      break;

    case Filter::BadSampleCheckSum:
      boFinalState = TRUE;
      m_asi = AnalysisStateInfo::BadSampleCheckSum();
      strXErrorImport = "sample";
      break;

      // CRC check failed on the reconstructed sample
    case Filter::CRCsampleFailure:
      boFinalState = TRUE;
      m_asi = AnalysisStateInfo::CRCSampleFailure();
      strXErrorImport = "sample";
      break;

      // CRC check failed on the scrambled content stream
    case Filter::CRCcontentFailure:
      boFinalState = TRUE;
      m_asi = AnalysisStateInfo::CRCContentFailure();
      strXErrorImport = "content";
      break;

      // one or more attributes are missing
    case Filter::MissingAttributes:
      boFinalState = TRUE;
      m_asi = AnalysisStateInfo::MissingAttributes();
      strXErrorImport = "missing";
      break;

    case Filter::BadSubmittorID:
      boFinalState = TRUE;
      m_asi = AnalysisStateInfo::BadSubmittorID();
      strXErrorImport = "malformed (";
      strXErrorImport += AttributeKeys::CustomerID();
      strXErrorImport += ": ";
      //strXErrorImport += m_arSample.GetAttributes()[AttributeKeys::CustomerID()];
      strXErrorImport += ")";
      break;

    case Filter::DataOverflow:
      boFinalState = TRUE;
      m_asi = AnalysisStateInfo::DataOverflow();
      strXErrorImport = "overrun";
      break;

    case Filter::UnknownSampleType:
      boFinalState = TRUE;
      m_asi = AnalysisStateInfo::UnknownSampleType();
      strXErrorImport = "type (";
      //strXErrorImport += m_arSample.GetAttributes()[AttributeKeys::SampleType()];
      strXErrorImport += ")";
      break;

    case Filter::UnpackageFailure:
      boFinalState = TRUE;
      m_asi = AnalysisStateInfo::UnpackageFailure();
      strXErrorImport = "package";
      break;
			
      // *** Critical Errors; we will throw an exception at the end of this method ***
    case Filter::DatabaseError:
      iCriticalErrorCause = CAVISTransactionImportSampleException::filterCritical;
      strCriticalErrorDescription = "A database error occurred while the filter was processing the sample.";

      // can't set the analysis state, since the database is broken!
      break;

    case Filter::UnableToCreateMappedMem:
      iCriticalErrorCause = CAVISTransactionImportSampleException::filterCritical;
      strCriticalErrorDescription = "The filter was unable to allocate sufficient storage for the sample.";
      m_asi = AnalysisStateInfo::PausedFilter();
      break;

    case Filter::UnableToScan:
      iCriticalErrorCause = CAVISTransactionImportSampleException::filterCritical;
      strCriticalErrorDescription = "The filter was unable to reach the scanner process.";
      m_asi = AnalysisStateInfo::InternalError();
      break;

    case Filter::UnknownError:
      iCriticalErrorCause = CAVISTransactionImportSampleException::filterCritical;
      strCriticalErrorDescription = "An unknown error occurred while creating the filter.";
      m_asi = AnalysisStateInfo::PausedFilterUnknown();
      break;

      // should never get here!
    default:
      iCriticalErrorCause = CAVISTransactionImportSampleException::unhandledFilterState;
      strCriticalErrorDescription.Format("The filter returned a filter state that was unhandled: %s", (pFilter->StatusAsString()).c_str());
      break;
    }

  // Only perform this block of processing if we have actually created a new sample row in the AnalysisRequest table.
  if (pFilter->LocalCookie() != 0)
    {

      DateTime dtFinished = m_arSample.Finished();
	
      // set status if changed and not set by FinalState() or FinalErrorState();
      if (asi_old != m_asi)
        {
          if (dtFinished.IsNull())
            {
              m_arSample.State(m_asi);
              CAVISACSampleCommApp::s_tc.debug("Changed analysis state of current analysis request to \"%s\".", ((std::string )m_asi).c_str());
            }
          else
            {
              CAVISACSampleCommApp::s_tc.info("No need to set analysis state on sample -- it is a duplicate of a sample that has already been completely analyzed.");
            }
        }

      if (boFinalState)
        {
          if (dtFinished.IsNull())
            {
              CAVISACSampleCommApp::s_tc.debug("Setting the finished date for current AnalysisRequest.");
              if (!m_arSample.AnalysisFinished(now))
                {
                  iCriticalErrorCause = CAVISTransactionImportSampleException::errorSettingFinishedDate;
                  strCriticalErrorDescription = "Failed setting the finished date.";
                }
            }
          else
            {
              CAVISACSampleCommApp::s_tc.info("No need to set finished date for sample -- it is a duplicate of a sample that has already been completely analyzed.");
            }

          // if the new state has an associated "X-Error-Import" string, then set it 
          // into the request's attributes
          if (!strXErrorImport.empty())
            {
              // need to set the "X-Error-Import" attribute
              // No we don't, MPW - Wed Apr 05 12:54:42 2000
              // m_arSample.GetAttributes()[AttributeKeys::ErrorImport()] = strXErrorImport;

              CString strMsg;
              strMsg.Format("While importing a sample, the filter detected a non-critical sample error; filter state = %s", pFilter->StatusAsString().c_str());

              // generate alert via SMTP
              CAVISSendMail sendMail;
              CString strSMTPMessage;
              strSMTPMessage.Format("%s\n\nSee log and trace files on EXECUTIVE machine for more details.", strMsg);
              if (!sendMail.AVISSendMail(AVISSENDMAIL_PROFILE_ANALYSIS_COMM_TYPE_INDEX, (LPCSTR )strSMTPMessage))
                CAVISACSampleCommApp::s_tc.warning("Unable to send alert via SMTP!");

              CAVISACSampleCommApp::s_tc.warning(strMsg);
            }
        }
    }
  // throw exception if a filter status indicates a critical error
  if (iCriticalErrorCause != CAVISTransactionImportSampleException::noError)
    {
      throw new CAVISTransactionImportSampleException(
                                                      iCriticalErrorCause, 
                                                      strCriticalErrorDescription);
    }

  return boKeepSample;
}
/*****/
DWORD CAVISTransactionImportSample::InitRequest()
{
  m_boSetImportedDate = FALSE;

  return AVISTX_SUCCESS;
}
/*****/
int CAVISTransactionImportSample::CreateSampleDirectory()
{
  CString strErrorDescription;

  // the directory name
  m_strNewSampleDirectory.Format("%s\\%08u", m_strSampleStoragePathBase.c_str(), m_uiLocalCookie);

  m_strImportCompleteFilePath = m_strNewSampleDirectory + "\\" IMPORT_COMPLETE_FILENAME;

  CAVISACSampleCommApp::s_tc.debug("Creating new sample directory, \"%s\"...", (LPCSTR )m_strNewSampleDirectory);
  if (!::CreateDirectory(m_strNewSampleDirectory, NULL))
    {
      int err = GetLastError();

      // check if error resulting from creating a new directory is "already exists"
      if (ERROR_ALREADY_EXISTS == err)
        {
          // check to see if the "import complete" file is present
          WIN32_FIND_DATA fd;
          HANDLE hDir = ::FindFirstFile(m_strImportCompleteFilePath, &fd);

          // if the "import complte" file is present
          if (INVALID_HANDLE_VALUE != hDir)
            {
				// finished with the "FindFirstFile()" handle
              ::FindClose(hDir);

				// we cannot use this directory; it is already being used by an
				// imported sample; must exit
              strErrorDescription = "Unable to create new directory for new sample; the directory already exists and contains an imported sample.";
              CAVISACSampleCommApp::CriticalError(strErrorDescription);
              throw new CAVISTransactionImportSampleException(
                                                              CAVISTransactionImportSampleException::sampleStorageError,
                                                              strErrorDescription);
            }

          // check why FindFirstFile() failed
          err = GetLastError();

          // if the error is NOT "file not found", then it is a critical error; must exit
          if (ERROR_FILE_NOT_FOUND != err)
            {
              strErrorDescription.Format("Error occurred while checking for \"import complete\" file in sample directory, err = %d.", err);
              throw new CAVISTransactionImportSampleException(
                                                              CAVISTransactionImportSampleException::unableToCreateSampleDirectory,
                                                              strErrorDescription);
            }

          // We CAN use this directory; the absence of the "import complete" file
          // indicates that no file was ever imported into this directory,
          // so we can use it to import the current sample.
          CAVISACSampleCommApp::s_tc.warning("Using an existing sample directory for new sample; directory is not in use by an imported sample.");
        }
      else
        {
          // when trying to create a new sample directory, the error we got was NOT
          // "already" exists, and so is an unrecoverable error; must exit
          strErrorDescription.Format("Unable to create new directory for new sample; err = %d.", err);
          throw new CAVISTransactionImportSampleException(
                                                          CAVISTransactionImportSampleException::unableToCreateSampleDirectory,
                                                          strErrorDescription);
        }
    }
  else
    {
      m_strNewSampleDirectory += "\\sample";
      if (!::CreateDirectory(m_strNewSampleDirectory, NULL))
        {
          // when trying to create a new sample directory, the error we got was NOT
          // "already" exists, and so is an unrecoverable error; must exit
          strErrorDescription.Format("Unable to create new directory for new sample.");
          throw new CAVISTransactionImportSampleException(
                                                          CAVISTransactionImportSampleException::unableToCreateSampleDirectory,
                                                          strErrorDescription);
        }

      // no problem creating a new sample directory
      CAVISACSampleCommApp::s_tc.debug("Created new sample directory, \"%s\".", (LPCSTR )m_strNewSampleDirectory);
    }

  return AVISTX_SUCCESS;
}
/*****/
void CAVISTransactionImportSample::CreateImportCompleteFlagFile()
{
  TRY
    {
      CAVISACSampleCommApp::s_tc.debug("Creating \"import complete\" flag file...");
      CFile fileImportComplete(
                               m_strImportCompleteFilePath,
                               CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
      fileImportComplete.Close();
      CAVISACSampleCommApp::s_tc.debug("Created \"import complete\" flag file.");
    }
  CATCH (CFileException, e)
    {
      CString strErrorDescription = "Unable to create \"Import Complete\" flag file.";
      throw new CAVISTransactionImportSampleException(
                                                      CAVISTransactionImportSampleException::errorCreatingImportCompleteFlagFile,
                                                      strErrorDescription);
    }
  END_CATCH
    }
/*****/
int CAVISTransactionImportSample::FeedContentToFilter(Filter* pFilter)
{
  // Read response content
  DWORD cbResponseContentLength = GetResponseContentLength();

  if (cbResponseContentLength == -1)
    return AVISTX_TERMINATE;

  CAVISACSampleCommApp::s_tc.debug("Response content length = %d.", cbResponseContentLength);

  // allocate buffer for content block
  char bufResponseContent[READ_RESPONSE_BLOCK_SIZE];
  CAVISACSampleCommApp::s_tc.debug("Block size for reading content = %d.", READ_RESPONSE_BLOCK_SIZE);

  BOOL done = FALSE;
  UINT cbReadFromBlock;
  UINT cbToRead = READ_RESPONSE_BLOCK_SIZE;
  UINT cbTotalRead = 0;
  UINT cbRemaining = cbResponseContentLength;

  SetReadBufferSize(0);

	// stop when fewer than the requested # of bytes have been read (EOF) or
	// the filter needs no more data
  do
    {
#ifdef _DEBUG
      ZeroMemory(bufResponseContent, READ_RESPONSE_BLOCK_SIZE);
#endif
      // read the number of bytes reamining, up to the block size
      if (cbRemaining < READ_RESPONSE_BLOCK_SIZE)
        cbToRead = cbRemaining;

      CAVISACSampleCommApp::s_tc.debug("Reading next response content block...");
      cbReadFromBlock = ReadNextResponseContentBlock(bufResponseContent, cbToRead);

      // *** check if error during read
      if (cbReadFromBlock == RNRCB_NETWORK_ERROR) // network error
        {
          CAVISACSampleCommApp::s_tc.error("Network error occurred while reading content.");

          // set request state to "lost"
          m_arSample.State(AnalysisStateInfo(AnalysisStateInfo::Lost()));

          return AVISTX_NETWORK_FAILURE;
        }
      else
        if (cbReadFromBlock == RNRCB_TERMINATE_TX)
          {
            CAVISACSampleCommApp::s_tc.error("Terminating \"Import Sample\" operation...");
            return AVISTX_TERMINATE;
          }

      CAVISACSampleCommApp::s_tc.debug("Successfully read %d bytes of response content.", cbReadFromBlock);

      // recalculate total bytes of content read
      cbTotalRead += cbReadFromBlock;
      cbRemaining -= cbReadFromBlock;
      CAVISACSampleCommApp::s_tc.debug("Read a total of %d bytes of response content, %d remaining to be read", cbTotalRead, cbRemaining);

      // Send the block of response content to the filter (filter.AppendData()).
      // If the filter doesn't respond by indicating "NeedsMoreData", then we can
      // stop reading content and sending it to the filter.
      // This indicates that either some error has occurred in the filter while 
      // processing the data we just sent to it, or the filter has received all the 
      // data for the sample (and automatically attempted to scan it, resulting in 
      // success or an error).
	  //Moved the following line from the while statement on Mar/28/2000.
	  pFilter->AppendData(bufResponseContent, cbReadFromBlock);
	  //Keep appending the data to the filter until we drain the last byte of it.
	} while (cbRemaining > 0);

  CAVISACSampleCommApp::s_tc.debug("Finished appending content to filter.");

	// If there were no errors while feeding data to the filter, the filter will have
	// automaticaly begun to scan the file at the final call to filter.AppendData().

	// returns FALSE if we shouldn't keep the sample
  if (!ProcessFilterStatus(pFilter))
    {
      return GSS_SAMPLE_NOT_IMPORTED;
    }

  return AVISTX_SUCCESS;
}
/*****/
void CAVISTransactionImportSample::SaveSampleToFileServer(Filter* pFilter)
{
  // Store the reconstructed file on the file server if it was fully received
  std::string strReconstructedFilename = m_strNewSampleDirectory + "\\" + NEW_SAMPLE_FILENAME;
  CAVISACSampleCommApp::s_tc.msg("Saving sample to \"%s\"...", strReconstructedFilename.c_str());

  // for some reason, the WriteSampleToDisk() method returns "true" when it fails!
  if (false == pFilter->WriteSampleToDisk(strReconstructedFilename))
    {
      throw new CAVISTransactionImportSampleException(
                                                      CAVISTransactionImportSampleException::errorStoringSample,
                                                      "Failed writing sample to file server.");
    }
  CAVISACSampleCommApp::s_tc.msg("Saved sample to \"%s\".", strReconstructedFilename.c_str());
}
/*****/
int CAVISTransactionImportSample::ReadResponseContent()
{
  CAVISACSampleCommApp::s_tc.debug("Starting to read content...");
  CAVISACSampleCommApp::s_tc.debug("Response analysis cookie = %d", GetResponseAnalysisCookie());

  // retrieve the response headers
  char szHeaders[1024*10];
  DWORD dwSize = sizeof(szHeaders);

  // the headers are retrieved in a format which has each header null-terminated
  // and the final header has an additional null-terminating character following it.
  GetResponseHeaders(szHeaders, &dwSize);

  CAVISACSampleCommApp::s_tc.debug("Retrieved response headers:\n%s", szHeaders);

  // construct a new filter object and send it the headers for the sample currently
  // being imported
  CAVISACSampleCommApp::s_tc.debug("Instantiating filter...");

  // Create the filter(throws FilterException and AVISDBException exceptions)
  Filter filter(szHeaders);

  //Get the SampleChecksum value from the filter. This value is required in the
  //SampleImporter client if we have to report the final state as the initial status 
  //in the sample importer client. This will happen if we have the final result in the
  //results table for an earlier sample with a matching checksum.
  //Added on Mar/16/2000.
  m_csSampleChecksum = filter.GetSampleCheckSum();
  std::string checkSumStr = m_csSampleChecksum;
  CAVISACSampleCommApp::s_tc.debug("X-SampleChecksum read from filter = %s.", checkSumStr.c_str());

  // remember the address of the filter
  CAVISACSampleCommApp::s_tc.debug("Instantiated filter.");

  // get the server object for the gateway on which this transaction is being run
  Server srvGateway = m_pClient->GetGateway();

  // get the local (i.e. the Analysis Center cookie) cookie
  m_uiLocalCookie = filter.LocalCookie();
  CAVISACSampleCommApp::s_tc.debug("New analysis request cookie = %d.", m_uiLocalCookie);

  // If the cookie returned by the filter is zero, it has found a match by checksum 
  // in the AnalysisResults table.  Therefore, we MUST NOT create an AnalysisRequest
  // object (i.e. record)

  if (m_uiLocalCookie != 0)
    {
      // get analysis request for the sample we are importing
      // (throws AVISDBException exceptions)
      m_arSample = AnalysisRequest(m_uiLocalCookie);

      // The following if block was added on MAR/16/2000.
	  //Before assigning the RemoteCookie, check if the analysis record already has
	  //a non-zero remote cookie. If it already has a non-zero remote cookie, then we
	  //are currently dealing with a duplicate cookie. We don't have to receive 
	  //the contents of the duplicate sample again. Instead we can report the initial
	  //status using the analysis state of an earlier sample with the checksum same as
	  //the current sample.
	  if (m_arSample.RemoteCookie() != 0)
	  {
         CAVISACSampleCommApp::s_tc.debug("Skip importing this sample since it is a duplicate sample that has an intermediate state.");
		 //Set the done flag in the filter to true. Otherwise, the return from this function
		 //will cause the filter destructor to set the sample state to "lost".
		 filter.SetDone();
		 return GSS_SAMPLE_NOT_IMPORTED;
	  }      

      // update the analysis request with the gateway id and the gateway cookie
      CAVISACSampleCommApp::s_tc.debug("Associating the current gateway with the analysis request...");
      // throws AVISDBException exceptions
      m_arSample.RemoteServerAndCookie(srvGateway, GetResponseAnalysisCookie());
      CAVISACSampleCommApp::s_tc.debug("Associated the current gateway with the analysis request.");
      
      // if the filter has determined that the sample doesn't meet the requirements for
      // importing via analyzing the sample headers, then don't bother reading the
      // response content (the sample).
    }
    else
	{
		//This else block was added on MAR/16/2000.
		//If we got a zero cookieid, then we have the final result for an earlier
		//sample with the same checksum as the current sample. In that case,
		//there is no need to import the sample again. We can directly report the
		//final state as the initial status for this sample.
		CAVISACSampleCommApp::s_tc.debug("Skip importing this sample since it is a duplicate sample that has a final state.");
		//Set the done flag in the filter to true. Otherwise, the return from this function
		//will cause the filter destructor to set the sample state to "lost".
		filter.SetDone();
	    return GSS_SAMPLE_NOT_IMPORTED; 
	}

  // define the initial state as an "unknown state"
  // throws AVISDBException exceptions
  m_asi = AnalysisStateInfo::UnknownSampleState();

  // process the filter status; if the return is false, we shouldn't keep the sample
  if (!ProcessFilterStatus(&filter))
    {
      return GSS_SAMPLE_NOT_IMPORTED;
    }

  // If we've reached this point, this means that we can should create a directory
  // in which to store the sample, receive the content data, and then store the
  // reconstructed sample to the sample directory.

  // create the sample directory
  int rc = CreateSampleDirectory();
  if (AVISTX_SUCCESS != rc)
    return rc;

  // receive the content and feed it to the filter
  rc = FeedContentToFilter(&filter);
  if (AVISTX_SUCCESS != rc)
  {
	//Added on May/16/2000. 
	//Don't leave any stray sample directories in the file server.
    RemoveSampleDirectory();
	return rc;
  }

  // If we've reached this point, this means that there were no errors in receiving,
  // reconstructing, nor scanning the sample; now we can save the sample to the 
  // file server.
  SaveSampleToFileServer(&filter);

  //Save the sample attributes to a file.
  SaveSampleAttributesToFileServer (szHeaders);

  // Calling ProcessFilterStatus() now can ONLY change the analysis request's state
  // No the call CANNOT fail, and we don't care about the return value since we know 
  // we've already stored the sample.
  ProcessFilterStatus(&filter);

  // must create the "import complete" flag file in the sample directory. This will
  // mark the directory as "used" so than no other sample can be imported into this
  // directory.
  CreateImportCompleteFlagFile();

  // we've imported the sample; report it back to the client
  return GSS_SAMPLE_IMPORTED;
}
/*****/
AnalysisRequest CAVISTransactionImportSample::GetAnalysisRequest()
{
  return m_arSample;
}
/*****/
CheckSum CAVISTransactionImportSample::GetCheckSum()
{
  return m_csSampleChecksum;
}
/*****/
BOOL CAVISTransactionImportSample::SampleWasAvailableAtGateway()
{
  // for this transaction, the gateway sends 'ok' when there is a sample available
  return(HTTP_STATUS_OK == GetResponseStatusCode());
}
/*****/
UINT CAVISTransactionImportSample::GetLocalCookie()
{
  return m_uiLocalCookie;
}
/*****/
BOOL CAVISTransactionImportSample::NeedToSetImportedDate()
{
  return m_boSetImportedDate;
}
/*****/
void CAVISTransactionImportSample::SetNeedToSetImportedDate(BOOL boNeedToSetImportedDate)
{
  m_boSetImportedDate = boNeedToSetImportedDate;
}
/*****/
//This function was added on Mar/14/2000.
//Whenever a sample is imported and stored in the fileserver, we have to
//create a text file with all the sample attributes. This function creates a
//text file under the "\\server\SampleStorage\<cookieID>\SampleAttributes" directory.
void CAVISTransactionImportSample::SaveSampleAttributesToFileServer (char *szHeaders)
{
	CString strSampleDirectory;

	strSampleDirectory.Format("%s\\%08u", m_strSampleStoragePathBase.c_str(), m_uiLocalCookie);
	std::string strSampleAttributesDirectory = strSampleDirectory + "\\" + SAMPLE_ATTRIBUTE_DIR; 

	//Create the SampleAttributes directory now.
	CAVISACSampleCommApp::s_tc.debug("Creating sample attributes directory, \"%s\"...", (LPCSTR )strSampleAttributesDirectory.c_str());
	if (::CreateDirectory(strSampleAttributesDirectory.c_str(), NULL))
	{
		std::string strSampleAttributesFile = strSampleAttributesDirectory + "\\" + SAMPLE_ATTRIBUTE_FILE;

		Handle	handle(CreateFile(strSampleAttributesFile.c_str(), GENERIC_WRITE, 0, NULL,
								  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));

		if (INVALID_HANDLE_VALUE != handle)
		{
			DWORD	bytesWritten	= 0;
			WriteFile(handle, szHeaders, strlen (szHeaders), &bytesWritten, NULL);
			CString gatewayAddress = GetGatewayAddress();
			CloseHandle (handle);
			CAVISACSampleCommApp::s_tc.debug("Saved sample attributes to \"%s\"", (LPCSTR )strSampleAttributesFile.c_str());
		}
		else
		{
			CAVISACSampleCommApp::s_tc.debug("Unable to create file \"%s\"", (LPCSTR )strSampleAttributesFile.c_str());
		}
	}
	else
	{
		CAVISACSampleCommApp::s_tc.debug("Unable to create directory \"%s\"", (LPCSTR )strSampleAttributesDirectory.c_str());
	}
}

/*****/
//Added this method on May/16/2000. 
//It blindly removes the sample directory from the fileserver.
//This method is called whenever the imported sample is already
//detected and repaired by the existing definitions. In that case,
//we don't have to store the sample in the fileserver. We will remove
//the sample directory that was created before feeding the filter with
//the sample contents.
void CAVISTransactionImportSample::RemoveSampleDirectory()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char cmdLine[1024];

	//Form the command line to remove the sample directory.
	//Use /s option to remove all subdirectories and /q to do it quietly.
	sprintf(cmdLine, "cmd /q /c rmdir %s\\%08u /s /q",
                m_strSampleStoragePathBase.c_str(), 
				m_uiLocalCookie);

    memset (&si, 0, sizeof (STARTUPINFO));
    si.cb = sizeof (STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    si.lpTitle = cmdLine;
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	CAVISACSampleCommApp::s_tc.debug("Attempting to remove the sample directory using command: \"%s\".", cmdLine);

    if (CreateProcess(NULL,
            cmdLine,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi))
    {
        CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
        CAVISACSampleCommApp::s_tc.debug("Successfully removed the sample directory.");
    }
	else
	{
		CAVISACSampleCommApp::s_tc.debug("Unable to remove the sample directory.");
	}
}
