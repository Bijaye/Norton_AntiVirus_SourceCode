// StatusMonitor.cpp: implementation of the StatusMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <list>
#include <algorithm>
#include <iostream>

#include <CMclAutoLock.h>
#include <SystemException.h>

#include "StatusMonitor.h"
#include "Sample.h"
#include "GlobalData.h"
#include "ScannerSubmittor.h"
#include "CommErrorHandler.h"
#include <AttributeKeys.h>
#include <XAnalysisState.h>
#include <XError.h>
#include <AttributesParser.h>
#include "EventLog.h"
#include "Attention.h"


#include <MyTraceClient.h>
#include <IcePackAgentComm.h>

#include <Logger.h>
//#include "IcePackMonitor.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef TIMEING_TESTS
#define TT_VARS						\
	int		queryCount;				\
	int		loopCount;				\
	clock_t	loopStart, loopEnd;		\
	char	messageBuffer[512];		

#define	TT_LOOP_TOP					\
	loopStart	= clock();			\
	loopCount	= 0;				\
	queryCount	= 0;

#define TT_LOOP_INC1	queryCount = listCopy.size();
#define	TT_LOOP_INC2	loopCount++;

#define	TT_LOOP_BOTTOM				\
	loopEnd	= clock();				\
	sprintf(messageBuffer, "StatusMonitor with %d samples in submitted state (%d queries) took %8.4f seconds\n",	\
			queryCount, loopCount, ((float)(loopEnd - loopStart))/((float) CLOCKS_PER_SEC));	\
	cout << messageBuffer;			\
	cout.flush();

#else
#define TT_VARS
#define	TT_LOOP_TOP
#define TT_LOOP_INC1
#define TT_LOOP_INC2
#define TT_LOOP_BOTTOM
#endif


StatusMonitor::StatusMonitor() : CMclThreadHandler()
{

}

StatusMonitor::~StatusMonitor()
{

}

//
//	StatusMonitor
//
//	Overview:
//		This thread takes samples in the "submitted" state and queries the gateway
//		to see if the analysis state has changed from a non-final to final state.
//		When the gateway returns that the sample has changed to a final analysis state
//		this thread changes the sample's state to one of three states;
//			unneeded	if no new definitions are needed (signatures = 0, usally
//						because the sample was not infected in the first place).
//			needed		if new definitions are needed and the required definition
//						or newer is not already on the IcePack server.
//			available	if new definitions are needed and are already on the IcePack
//						server ready to be downloaded to the client.
//
//	Algorithm:
//		(Simplified version, real code is slightly different)
//		Do
//			Make a list of samples in the "submitted" state from the global sample list
//			Sort the list in decending priority
//			For each element in the list
//				ask the gateway for the latest sample status
//				update the sample based on the attributes returned by the gateway
//			If any items changed update the master sample list.
//			If any items changed from the submitted state to another state set the
//				submitSample event
//			If any items changed to the "available" state set the deliverSignature event.
//			If any items changed to the "needed" state ask for the newer signatures to be
//				downloaded.
//		While service not shutting down, sleep for statusCheckInterval
//
//	Globals static methods/members Used
//		stop					bool		if true exit this thread.  Set by SCM when service is
//											told to stop or the system is shutting down.
//		critSamplesList			CMclCritSec	used to serialize access to the samplesList
//		samplesList				std::list	List of samples that IcePack might deal with (samples not
//											already in a final state or submitted via Scan and Deliver)
//		submitSample			CMclEvent	used to signal the SubmittorDownload thread to submit
//											new samples to the gateway.
//		deliverSignature		CMclEvent	used to signal the JobPackager to send a definition
//											package to a client machine.
//		SecureStatusQuery		bool		Should request be made through a SLL port?
//		GatewayURL				std::string	the URL of the gateway. This value comes from two
//											registry values, webGatewayName and webGatewayPort
//		GatewaySSLURL			std::string	the URL of the gateway through the SSL port.  This value
//											comes from two registry values, webGatewayName and
//											webGatewayPortSSL.
//		StatusCheckInterval		uint		number of milliseconds to wait between status update
//											calls to the gateway.  This value comes from the
//											webStatusInterval registry entry multiplied by 60000.
//

unsigned StatusMonitor::ThreadHandlerProc()
{
	EntryExit		entryExit(Logger::LogIcePackStatusMonitor, "ThreadHandlerProc");
//	EnterExitThread	eet(IcePackMonitor::statusMonitor);

	SystemException::Init();

	QSThreadLife	qsThreadLife;		// initializes COM for this thread
	list<Sample>	listCopy;
	Sample			sample;
	list<Sample>::iterator	i, j;
	string			newStatus;
	string			attrs;				// attributes
  string      headers     = "";
#if 0 /* inw 2000-02-09 use in conjunction with header code beneath */
  bool        doneHeaders = false;
#endif /* 0 */

	bool			submitNewSample;
	bool			newJobPackage;
	bool			changed;
	bool			tracingEnabled = false;

	MyTraceClient					traceClient;
	IcePackAgentComm::SampleComm	sampleComm;

	if (GlobalData::stop)
		return -1;

	TT_VARS

	do
	{
		try
		{
			TT_LOOP_TOP

			Logger::Log(Logger::LogDebug, Logger::LogIcePackStatusMonitor,
						"Checking for samples in \"submitted\" state to check current status for");

			if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackStatusMonitor) &&
				Logger::WillLog(Logger::LogDebug, Logger::LogComm))
			{
				if (!tracingEnabled)
				{
					string	traceFileName(GlobalData::TraceFileName());

					traceClient.EnableTracing();
					sampleComm.SetTraceClient(traceClient, traceFileName.c_str(),
												traceFileName.c_str(), "IcePack");
					tracingEnabled = true;
				}
			}
			else if (tracingEnabled)
			{
				traceClient.EnableTracing(false);
				tracingEnabled	= false;
			}

			submitNewSample	= false;
			newJobPackage	= false;
			changed			= false;

			// make copy of samples list
			listCopy.clear();
			{
				CMclAutoLock	lock(GlobalData::critSamplesList);
				i = GlobalData::samplesList.begin();

				for (; i != GlobalData::samplesList.end(); i++)
				{
					if (SampleStatus::submitted == i->Status())
					{
						listCopy.push_back(*i);
						listCopy.back().Mark(Sample::sUnchanged);
					}
				}
			}

			TT_LOOP_INC1

			string	url;

			bool		okay;
			bool		retryWithDelay;
			bool		retryWithoutDelay;
			bool		commError;
			ErrorInfo	errorInfo;

			for (i = listCopy.begin(); i != listCopy.end() && !GlobalData::stop; i++)
			{
				if (!i->IsNull() && SampleStatus::submitted == i->Status())
				{
					errorInfo.Reset();
					changed	= false;

#if 0 /* inw 2000-02-09 collective decision made to skip contact headers here. */
          if (false == doneHeaders)
          {
            headers     = GlobalData::GetContactHeaders();
            doneHeaders = true;
          }
#endif /* 0 */

					do
					{
						retryWithDelay		= false;
						retryWithoutDelay	= false;
						commError		= false;

						// get status, attrs returned in attrs

						// build the URL 
						// <http-type> ":\\" <server-address> ":" <port-number>

						// check if we need to use "http:" or "https:"
						url = (GlobalData::SecureStatusQuery() ? "https://" : "http://");

						// retrieve the server name (from the "X-Analysis-Gateway" attribute)
						string strAnalysisGateway;
						if (!i->GetAttribute(AttributeKeys::AnalysisGateway(), strAnalysisGateway))
						{
							// error: unable to retrieve the "X-Analysis-Gateway" attribute
							i->SetAttribute(AttributeKeys::Attention(),
											"Unable to execute GetStatus() -- the 'X-Analysis-Gateway' attribute is missing for the current sample.");
							i->Status(SampleStatus::attention);

							// set changed flag; forces the above "error" attribute and status change to 
							// be updated in quarantine console
							changed = true;

							// break out of the retry loop; goes directly to the "if (changed)" conditional below
							break;
						}

						// append the server name to the url
						url += strAnalysisGateway;

						// append the port number to the url
						uint uPort = (GlobalData::SecureStatusQuery() ? GlobalData::GatewayPortSSL() : GlobalData::GatewayPort());

						if (0 != uPort)
						{
							char buffer[32];
							::sprintf(buffer, ":%04d", uPort);
							url	+= buffer;
						}
						else
						{
							// no port # or illegal port #; should this be an exception or error?
						}

						// don't need to append the servlet-name, since it is done
						// inside the GetStatus() transaction

            std::string sampleChecksumSubmitted;

            i->GetAttribute(AttributeKeys::SampleChecksumSubmitted(), sampleChecksumSubmitted);
						okay	= sampleComm.GetStatus(url, headers, i->GatewayCookie(), sampleChecksumSubmitted, attrs);

						TT_LOOP_INC2

						if (okay)
						{
							bool	dummy;
							i->SetAttribute(AttributeKeys::DateStatus(), DateTime().AsHttpString(dummy));

							if (i->Update(attrs))
							{
								Logger::Log(Logger::LogInfo, Logger::LogIcePackStatusMonitor,
											"Status successfully recieved from gateway");

								changed	= true;
								i->Mark(Sample::sChanged);

								if (SampleStatus::submitted != i->Status())
								{
									submitNewSample = true;	

									if (SampleStatus::available == i->Status())
									{
										newJobPackage = true;
									}
								}
							}
							else
								Logger::Log(Logger::LogDebug, Logger::LogIcePackStatusMonitor,
											"No change to sample analysis status");
						}
						else
            {
              IcePackAgentComm::ErrorCode  errorCode = sampleComm.Error();
              if (IcePackAgentComm::TerminatedByCaller == errorCode)
                return 0;
              else if (IcePackAgentComm::GatewayError == errorCode)
              {
                HandleGatewayError(sampleComm,
                                   attrs,
                                   *i,
                                   errorInfo,
                                   retryWithDelay,
                                   retryWithoutDelay);
              }
              else
              {
                /*
                 * Not a gateway error.
                 */

                if ((IcePackAgentComm::NetworkError         == errorCode) ||
                    (IcePackAgentComm::NetworkCriticalError == errorCode))
                  commError = true;

                /*
                 * Note carefully that retryWithDelay and retryWithoutDelay
                 * may be set by HandleCommError().  In fact, Stop() could
                 * be triggered too...
                 */
                CommErrorHandler::HandleCommError(errorCode,
                                                  sampleComm.SystemExceptionCode(),
                                                  errorInfo,
                                                  retryWithDelay,
                                                  retryWithoutDelay,
                                                  Logger::LogIcePackStatusMonitor,
                                                  "error getting sample status, error = ");

                if ((!GlobalData::stop) && (!retryWithDelay) && (!retryWithoutDelay))
                {
                  switch (errorCode)
                  {
                    case IcePackAgentComm::ProcessTimedOutError :
                    case IcePackAgentComm::ProcessTerminationError :
                    case IcePackAgentComm::ProcessSyncError :
                    case IcePackAgentComm::NetworkCriticalError :
                      i->SetAttribute(AttributeKeys::Attention(),
                                      "Failed to resolve communication error");
                      i->Status(SampleStatus::attention);
                      break;
                    case IcePackAgentComm::UnknownError :
                      i->SetAttribute(AttributeKeys::Attention(),
                                      "Failed to resolve unknown communication error");
                      i->Status(SampleStatus::attention);
                      break;
                    case IcePackAgentComm::InvalidRedirectError :
                      i->SetAttribute(AttributeKeys::Attention(),
                                          "Redirection error at gateway");
                      i->Status(SampleStatus::attention);
                      break;
                    default :
                      break;
                  }
                }
              }

              if (retryWithDelay)
                GlobalData::SleepInSegments(GlobalData::RetryInterval,
                                            Logger::LogIcePackStatusMonitor,
                                            "Pausing because of a problem collecting status information for a sample");
              else if (!retryWithoutDelay && SampleStatus::submitted != i->Status())
                submitNewSample = true;

              changed = true;
            }

            if (commError)
            {
              Attention::Set(Attention::Query);
              commError = false;
            }
            else
              Attention::UnSet(Attention::Query);
          }
          while ((!GlobalData::stop) && (retryWithDelay || retryWithoutDelay));

					if (changed)
					{
						i->Mark(Sample::sChanged);
						i->Commit();
						GlobalData::UpdateMasterSampleList(*i);
						if (SampleStatus::needed == i->Status())
							GlobalData::SetSigToDownload(i->NeededSeqNum(), false);
						if (submitNewSample)
						{
							submitNewSample = false;
							Logger::Log(Logger::LogInfo, Logger::LogIcePackStatusMonitor,
											"Signal ScannerSubmittor to submit new sample(s)");
							GlobalData::submitSample.Set();
						}
						if (newJobPackage && GlobalData::AutoDefinitionDelivery())
						{
							newJobPackage	= false;
							Logger::Log(Logger::LogInfo, Logger::LogIcePackStatusMonitor,
											"Signal JobPackager to deliver signatures to a client machine");
							GlobalData::deliverSignature.Set();	// signal new job
						}
					}

					i->DeActivate();
				}
			}
		}

		catch (...)
		{
			GlobalData::HandleException("StatusMonitor::ThreadHandlerProc()",
										Logger::LogIcePackStatusMonitor);
		}

		try { listCopy.clear(); } catch (...) { };

		TT_LOOP_BOTTOM

		//
		//	This method will wake up when the interval has passed or IcePack is shutting
		//	down.  It also handles the issue of configuration changes while asleep.
		GlobalData::SleepInSegments(GlobalData::StatusCheckInterval,
									Logger::LogIcePackStatusMonitor,
									"Normal pause between checks on sample status");

	} while (!GlobalData::stop);


	return 0;
}

/* ----- */

/*
 * The history of this method, as with others, is opaque.  The alert reader
 * will note that very similar functionality is to be found in three methods
 * both in this module and elsewhere:
 *   ScannerSubmittor::HandleDownloadError()
 *   ScannerSubmittor::HandleSubmissionError()
 *   StatusMonitor::HandleGatewayError()
 * Exactly why this is, I have no idea.  However, in the future it will be
 * nice to merge the logic into one central place.
 */
void StatusMonitor::HandleGatewayError(IcePackAgentComm::SampleComm &sampComm,
                                       std::string                  &attrs,
                                       Sample                       &sample,
                                       ErrorInfo                    &errorInfo,
                                       bool                         &retryWithInterval,
                                       bool                         &retryWithoutInterval)
{
  EntryExit entryExit(Logger::LogIcePackStatusMonitor, "HandleGatewayError");
  string    xError;
  string    why;
  string    msg;

  retryWithInterval    = false;
  retryWithoutInterval = false;

  /*
   * Is there an X-Error: header?
   */
  if ((AttributesParser::ParseMultipuleAttributes(attrs, AttributeKeys::Error(), xError)) &&
      (xError != ""))
  {
    if (XError::Crumbled()  == xError)
    {
      why = "cookie crumbled";
      sample.Status(SampleStatus::error);
    }
    else if (XError::Declined() == xError)
    {
      why = "sample declined";
      sample.Status(SampleStatus::error);
    }
    else if (XError::Internal() == xError)
    {
      why = "internal gateway error";
      sample.Status(SampleStatus::error);
    }
    else if ((XError::Malformed() == xError) ||
             (XError::Missing()   == xError))
    {
      why = "GatewayError, malformed message or missing critical attribute";
      sample.Status(SampleStatus::error);
    }
    else
    {
      /*
       * WTF error.
       */
      why = " Unknown or unexpected GatewayError [" + xError + "]";
      sample.Status(SampleStatus::error);
    }

    /*
     * ????
     */
    if (0 == errorInfo.tries)
      sample.SetAttribute(AttributeKeys::Error(), xError);
  }
  else
  {
    /*
     * There is no X-Error: header.
     */
    why = "GatewayError w/o an X-Error attribute!";
    sample.Status(SampleStatus::error);
  }

  msg = "Error getting sample status, error = " + why;
  Logger::Log(Logger::LogWarning, Logger::LogIcePackStatusMonitor, msg.c_str());

  /*
   * Stamp the changes back into the sample.
   */
  sample.Commit();
}
