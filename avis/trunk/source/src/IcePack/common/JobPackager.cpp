// JobPackager.cpp: implementation of the JobPackager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JobPackager.h"

#include <algorithm>
#include <sys\stat.h>
#include <list>

#include <AttributeKeys.h>
#include <CMclAutoLock.h>
#include "EventLog.h"

#include "Sample.h"
#include "GlobalData.h"

#include <Logger.h>

#include <Attention.h>

using namespace std;

// These constants specify how long, in milliseconds, the constructor
// for this thread will wait between attempts to initialize the
// DefCast interface, and when it give up and raise the "defcast"
// attention flag.  (EJP 7/24/00)

#define DEFCASTINIT_RETRY_INTERVAL    (5*1000)
#define DEFCASTINIT_RETRY_LIMIT    (5*60*1000)

// These constants specify how long, in milliseconds, this thread will
// wait at the top of its main loop before processing the client at
// the head of the defcast target list.  (Please note that INFINITE is
// defined in WINBASE.H as 0xFFFFFFFF, so these constants must be
// stored in unsigned integers.)  (EJP 7/20/00)

#define TIMEOUT_BRIEF_PAUSE 1000
#define TIMEOUT_EMPTY_DIST_LIST	INFINITE

// These constants specify the filename extensions used for different
// definition package formats.  (EJP 7/20/00)

#define FILENAME_EXTENSION_FATALBERT	"exe"
#define FILENAME_EXTENSION_VDB		"vdb"

typedef std::list<CDefTarget>::iterator DefTargetListIterator ;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

JobPackager::JobPackager() : CMclThreadHandler()
{
	try
		{
			// Initialize DefCast, or die trying.  That is, if
			// initialization fails, pause a moment and re-try repeatedly
			// until it succeeds, or we run out of patience, or a 'halt'
			// signal is received.  (EJP 7/24/00)
			
			for (int patience = DEFCASTINIT_RETRY_LIMIT;
					 patience>0;
					 patience -= DEFCASTINIT_RETRY_INTERVAL)
				{
					// Call the DefCast initialization function.  If it suceeds,
					// break out of the 'for' loop and continue with the
					// constructor.

					if (CDefTarget::Success == CDefTarget::InitApi()) break;

					// If DefCast initialization fails, log the failure and
					// pause for a moment before re-trying. (EJP 7/20/00)

					char msg[300];
					sprintf(msg, 
									"DefCast did not initialize, pausing for %u.%03u seconds...", 
									DEFCASTINIT_RETRY_INTERVAL/1000, 
									DEFCASTINIT_RETRY_INTERVAL%1000);
					Logger::Log(Logger::LogError, Logger::LogIcePackDDS, msg);
					DWORD rcWait = ::WaitForSingleObject(GlobalData::halt.GetHandle(), DEFCASTINIT_RETRY_INTERVAL);

					// Take some action appropriate for the return code from the
					// Windows WaitForSingleObject() function.

					switch(rcWait)
						{
						case WAIT_TIMEOUT:
							// If the timeout has expired, break out of the switch
							// statement and repeat the 'for' loop, re-trying
							// DefCast initialization.
							Logger::Log(Logger::LogError, Logger::LogIcePackDDS, 
													"Re-trying DefCast initialization ...");
							break;
							
						case WAIT_OBJECT_0:
							// If the "halt" object was signalled, terminate IcePack.
							Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, 
													"DefCast initialization abandoned due to 'stop' event");
							throw string("IcePack halted after DefCast initialization failure");
							return; // not reached
														
						default:
							// This should never happen, but if it does, terminate IcePack.
							Logger::Log(Logger::LogError, Logger::LogIcePackDDS, 
													"DefCast initialization abandoned due to unexpected event");
							throw string("IcePack panicking after DefCast initialization failure");
							return; // not reached
						}
				}
			
			// If we have run out of patience, raise the 'defcast' attention
			// flag and terminate IcePack.  This program will not work
			// properly until the DefCast/TransMan problem is fixed, and
			// IcePack is re-started.

			if (patience<=0)
				{
					Attention::Set(Attention::Defcast);
					Logger::Log(Logger::LogError, Logger::LogIcePackDDS, 
											"DefCast initialization failed, attention flag raised");
							throw string("IcePack timing out after DefCast initialization failure");
							return; // not reached
				}

			// If DefCast has been successfully initialized, clear the
			// 'defcast' attention flag, in case it was set earlier. (EJP
			// 7/20/00)

			Attention::UnSet(Attention::Defcast);
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, 
									"DefCast initialization succeeded, attention flag cleared");

			// Push the configured unblessed and unblessed targets on the
			// list, plus samples that are either in the "available" or
			// "distribute" states.  Since new targets are pushed on the
			// front, and targets to distribute are popped off the front, the
			// distribution list acts like a stack for new targets.  Since we
			// want to give priority to the blessed definitions over the
			// unblessed, we should push the unblessed targets on first, then
			// the blessed.
			
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, 
									"Recovering the target distribution list...");
			PushUnblessedTargetsOntoDistributionList(true);
			PushBlessedBroadcastTargetsOntoDistributionList();
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, 
									"Recovered the target distribution list");
		}
	catch(...)
		{
			GlobalData::HandleException("JobPackager constructor", Logger::LogIcePackDDS);
		}
}

/*****/
JobPackager::~JobPackager()
{
	// terminate use of the DefCast Api
	// *** Do we even care what the result code is for this call? ***
	CDefTarget::TermApi();
}
/*****/
bool JobPackager::IsDistributionListEmpty()
{
	return m_listDist.empty();
}
/*****/
bool JobPackager::PushTargetToFrontOfDistributionList(CDefTarget& dt)
{
	DefTargetListIterator i;

	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Pushing target to front of distribution list; checking for duplicates...");

	// only need to add targets that are not PointCast targets
	if (!dt.IsPointCastTarget())
	{
		// don't push duplicates on the list
		for (i = m_listDist.begin(); i != m_listDist.end(); i++)
		{
			// is the new element a duplicate of an existing target element
			// and is a PointCast target?
			if (i->IsDuplicate(dt) && !i->IsPointCastTarget())
			{
				Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "New target is a duplicate; it will not be added to the distribution list.");

				// found a duplicate; no need to push this target onto the list
				// because the duplicate target does not need to have any updates to
				// a sample state in quarantine (as opposed to targets that DO have an
				// associated sample).
				return false;
			}
		}
	}

	// iterated through the list and didn't find any duplicates.
	// push the target onto the front of the list
	m_listDist.push_front(dt);

	char szMsg[200];
	::sprintf(szMsg, "Pushed new target onto front of distribution list (total = %u).", m_listDist.size());
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

	return true;
}
/*****/
bool JobPackager::PushTargetToBackOfDistributionList(CDefTarget& dt)
{
	dt.ResetStatusChanged();
	m_listDist.push_back(dt);

	char szMsg[200];
	::sprintf(szMsg, "Pushed target onto back of distribution list (total targets = %u).", m_listDist.size());
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

	return true;
}
/*****/
CDefTarget JobPackager::GetTargetAtBackOfDistributionList()
{
	return m_listDist.back();
}
/*****/
CDefTarget JobPackager::GetTargetFromFrontOfDistributionList()
{
	return m_listDist.front();
}
/*****/
CDefTarget JobPackager::PopFirstTargetOffDistributionList()
{
	// copy the first element on the list to dt
	CDefTarget dtTemp = m_listDist.front();

	// remove the first element from the list
	m_listDist.pop_front();

	dtTemp.LogContents("Contents of target to process");

	char szMsg[200];
	::sprintf(szMsg, "Removed first target from front of distribution list (%u targets remain).", m_listDist.size());
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

	return dtTemp;
}
/*****/
void JobPackager::PushBroadcastTargetsOntoDistributionList(
	const string& strListOfTargets,
	CDefTarget::TargetType targetType)
{
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "About to push BroadCast targets onto distribution list...");

	// valid separators are <space>, <comma>, <newline>, and implicitly, end-of-line
	char szSeparators[] = " ,\n";

	// make a copy, since strtok() modified the original string
	char* pszCopyListOfTargets = ::_tcsdup(strListOfTargets.c_str());	

	// iterate through individual items in "strListOfTargets".
	// add each to the end of the distribution list
	char* pszTargetMachine = ::strtok(pszCopyListOfTargets, szSeparators);

	char szMsg[100];

	while (pszTargetMachine != NULL)
	{
		::sprintf(szMsg, "Found new BroadCast target, %s; creating target...", pszTargetMachine);
		Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

		CDefTarget dtListElement(targetType, pszTargetMachine);

		// add the target to the front of the distribution list
		PushTargetToFrontOfDistributionList(dtListElement);

		/* Get next token: */
		pszTargetMachine = ::strtok(NULL, szSeparators);
	}

	// no longer need the copy
	::free(pszCopyListOfTargets);

	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Finished pushing BroadCast targets onto distribution list.");
}
/*****/
void JobPackager::PushBlessedBroadcastTargetsOntoDistributionList()
{
	if (GlobalData::DefBlessedBroadcast())
	{
		Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "About to push Blessed BroadCast targets onto distribution list...");

		// append the list of blessed targets to the distribution list
		PushBroadcastTargetsOntoDistributionList(
			GlobalData::DefBlessedTargets(),
			CDefTarget::blessedBroadcast);				// blessed defs only

		Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Finished pushing Blessed BroadCast targets onto distribution list.");
	}
}
/*****/
void JobPackager::PushUnblessedBroadcastTargetsOntoDistributionList()
{
	// add an "unblessed broadcast" target?
	if (GlobalData::DefUnblessedBroadcast())
	{
		Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "About to push Unblessed BroadCast targets onto distribution list...");

		PushBroadcastTargetsOntoDistributionList(
			GlobalData::DefUnblessedTargets(),
			CDefTarget::unblessedBroadcast);			// don't require blessed defs only

		Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Finished pushing Unblessed BroadCast targets onto distribution list.");
	}
}
/*****/
void JobPackager::PushUnblessedTargetsOntoDistributionList(bool boInitializing)
{
	char szMsg[1000];

	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "About to push unblessed targets onto distribution list.");

	// The targets should be obtained from the samples. Each machine that 
	// submitted a sample that can be disinfected by the currently downloaded
	// defs (or its parent server) will be added to the list of targets.

	{
		// lock the sample list during the block that accesses the sample list
		CMclAutoLock lock(GlobalData::critSamplesList);

		Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Iterating through master sample list to build list of definition targets...");

		// iterate through the list of samples.
		for (SampleListIterator iterSample = GlobalData::samplesList.begin();
			 iterSample != GlobalData::samplesList.end();
			 iterSample++)
		{
			// check if sample can be disinfected by current defs, or
			// if the "boInitializing" argument is true,
			// check if a target from the sample was already enqueued for distribution
			// this prevents enqueuing duplicates of the samples in the "available" state.

			SampleStatus::Status st = iterSample->Status();

			// undelivered defs
			if (( st == SampleStatus::available  ) ||
				((st == SampleStatus::distribute ) && boInitializing))
			{
				Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Found a candidate sample from which to create a distribution target.");

				// flag indicating whether or not we've added an unblessed target to the list
				bool boAddedTargetToDistributionList = false;

				// get the value of the "signatures priority" attribute
				uint uiSignaturesPriority = iterSample->SigPriority();

				// add a "pointcast" target?
				if (uiSignaturesPriority > 0)
				{
					::sprintf(szMsg, "Sample's signature priority is %u; creating a PointCast target.", uiSignaturesPriority);
					Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

					// define an undistributed pointcast target, based on sample attributes
					CDefTarget dtListElement(CDefTarget::pointCast, iterSample);

					// put the target on the front of the list, giving it priority for
					// distribution.
					PushTargetToFrontOfDistributionList(dtListElement);

					boAddedTargetToDistributionList = true;
				}

				// add an "unblessed narrowcast" target?
				if (GlobalData::DefUnblessedNarrowcast())
				{
					Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "NarrowCasting is enabled; creating a NarrowCast target...");

					CDefTarget dtListElement(CDefTarget::narrowCast, iterSample);

					// put the target at the front of the list
					PushTargetToFrontOfDistributionList(dtListElement);

					// This flag should be set only when definitions have been
					// delivered directly to a client that submitted an infected
					// sample (the "pointcast" case, immediately above).  This
					// flag changes the status of the sample (the "distributed"
					// case, immediately below) so that IcePack will query the
					// client until the defintions are installed.  (EJP 8/3/00)

					//boAddedTargetToDistributionList = true;
				}

				// if we added any targets to the distribution list for this sample,
				// we must change the status of the sample to "distribute"
				if (boAddedTargetToDistributionList)
				{
					// only need to change the status of the sample if it doesn't have
					// that status already.
					if (st != SampleStatus::distribute)
					{
						Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Updating sample status to 'distribute'...");

						// update the status to "distribute"
						iterSample->Status(SampleStatus::distribute);

						// mark the sample as "changed"
						iterSample->Mark(Sample::sChanged);

						// update the list
						GlobalData::UpdateMasterSampleList(*iterSample);
					}
				}

			} 
			// delivered, but not installed defs
			else if ((st == SampleStatus::distributed) && boInitializing)
			{
				Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Recovering a PointCast target for which definitions were already distributed...");

				// we can't be sure whether the distributed state was set for the sample
				// because we did a pointcast, a narrowcast, or both. Since the only
				// reason we are adding these targets is to be able to update the sample
				// status to "installed", we only need to use a pointcast target, because
				// pointcasted or narrowcasted defs will always reach the original 
				// machine on which the sample was captured.
				CDefTarget dtListElement(CDefTarget::pointCast, iterSample, true);

				// in order to give higher priority to the delivery of defintions
				// over the querying for the installed state of definitions,
				// put the target on the back of the list
				PushTargetToBackOfDistributionList(dtListElement);

				// don't need to change the status of the sample -- it should remain in
				// the distributed state.
			}

			// Samples need to be disconnected from COM or users will be
			// unable to delete them from Jim Hill's extension - atk
			iterSample->DeActivate();

		} // end for()
	}

	// append the list of unblessed targets to the distribution list
	PushUnblessedBroadcastTargetsOntoDistributionList();
}
/*****/
void JobPackager::ProcessDuplicates(CDefTarget& dtOriginal, bool boMoveToBackOfList)
{
	// only need to process duplicates for PointCast targets
	if (!dtOriginal.IsPointCastTarget())
		return;

	CDefTarget dtListElement;
	unsigned int uiSize = m_listDist.size();
	DefTargetListIterator i = m_listDist.begin();

	// get the status of the original sample
	SampleStatus::Status statusOriginal = dtOriginal.GetSampleStatus();

	// sequence number of definitions delivered to original target
	ulong uiOriginalSeqNumDelivered = dtOriginal.GetSeqNumDelivered();

	// determine if we need to update the status of the duplicates
	bool boUpdateStatusOfDuplicate = (dtOriginal.SampleStatusHasChanged() &&
									  (statusOriginal != SampleStatus::unknown));

	char szMsg[200];

	::sprintf(szMsg, "Searching for duplicate targets on distribution list (total of %u targets)...", uiSize);
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

	for (unsigned int uiCount = 0; uiCount < uiSize; uiCount++)
	{
		// is the new element a duplicate of an existing target element
		// and is a PointCast target?
		if (i->IsDuplicate(dtOriginal) && i->IsPointCastTarget())
		{
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Found a duplicate target on the distribution list.");

			// need to update the sample status for the duplicate on the 
			// distribution list
			if (boUpdateStatusOfDuplicate)
			{
				if (statusOriginal == SampleStatus::distributed)
					i->MarkAsDistributed(uiOriginalSeqNumDelivered);
				else if (statusOriginal == SampleStatus::installed)
					i->MarkAsInstalled();
				else if (statusOriginal == SampleStatus::notInstalled)
					i->MarkAsNotInstalled();
				else
					i->UpdateSampleStatus(statusOriginal);
			}

			if (boMoveToBackOfList)
			{
				// get a copy of element i
				dtListElement = *i;
			}

			// remove the element
			i = m_listDist.erase(i);

			// need to move the element to the back of the list?
			if (boMoveToBackOfList)
			{
				PushTargetToBackOfDistributionList(dtListElement);
				Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Moved the duplicate target to the back of the distribution list.");
			}
			else
			{
				char szMsg[200];
				::sprintf(szMsg, "Removed the duplicate target from the distribution list (%u targets remain).", m_listDist.size());
				Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);
			}

			// no need to autoincrement, since it is done for us in the erase() method.
		}
		else
		{
			// need to autoincrement
			i++;
		}
	}
}
/*****/
void JobPackager::RemoveAllDuplicatesFromList(CDefTarget& dtListElement)
{
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Removing all duplicate targets from distribution list...");
	ProcessDuplicates(dtListElement, false);
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Removed all duplicate targets from distribution list.");
}
/*****/
void JobPackager::MoveAllDuplicatesToBackOfList(CDefTarget& dtListElement)
{
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Moving all duplicate targets to back of distribution list...");
	ProcessDuplicates(dtListElement, true);
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Moved all duplicate targets to back of distribution list.");
}
/*****/
unsigned JobPackager::ThreadHandlerProc()
{
#if 0
/* The following block contains test cases for exercising the RemoveAllDuplicatesFromList()
   and MoveAllDuplicatesToBackOfList().*/

	CDefTarget dt1(
		"ComputerName1",
		"HostName1",
		"Address1",
		"Parent1",
		"Guid1",
		false,				// no blessed
		1,					// sample id = 1
		false,				// not delivered
		0);					// no sequence number sent
	PushTargetToBackOfDistributionList(dt1);

	CDefTarget dtx(
		"ComputerNameX",
		"HostNameX",
		"AddressX",
		"ParentX",
		"GuidX",
		false,				// no blessed
		0,					// sample id = 1
		false,				// not delivered
		0);					// no sequence number sent
	PushTargetToBackOfDistributionList(dtx);

	CDefTarget dt2(
		"ComputerName1",
		"HostName1",
		"Address1",
		"Parent1",
		"Guid1",
		false,				// no blessed
		2,					// sample id = 1
		false,				// not delivered
		0);					// no sequence number sent
	PushTargetToBackOfDistributionList(dt2);

	CDefTarget dt3(
		"ComputerName1",
		"HostName1",
		"Address1",
		"Parent1",
		"Guid1",
		false,				// no blessed
		3,					// sample id = 1
		false,				// not delivered
		0);					// no sequence number sent
	PushTargetToBackOfDistributionList(dt3);

	RemoveAllDuplicatesFromList(dt1);
//	MoveAllDuplicatesToBackOfList(dt1);
	return 0;

	CDefTarget dt1(
		"ComputerName1",
		"HostName1",
		"Address1",
		"Parent1",
		"Guid1",
		false,				// no blessed
		1000,				// sample id = 1
		false,				// not delivered
		0,					// no sequence number sent
		"filename1.txt");
	PushTargetToBackOfDistributionList(dt1);

	CDefTarget dt2(
		"ComputerName1",
		"HostName1",
		"Address1",
		"Parent1",
		"Guid1",
		false,				// no blessed
		1001,				// sample id = 1
		false,				// not delivered
		0,					// no sequence number sent
		"filename2.txt");
	PushTargetToBackOfDistributionList(dt2);
#endif

	EntryExit		enterExit(Logger::LogIcePackDDS, "ThreadHandlerProc");

	char szMsg[1000];

	uint uiWaitTimeout;
	
	if (IsDistributionListEmpty())
		uiWaitTimeout = TIMEOUT_EMPTY_DIST_LIST;
	else
		uiWaitTimeout = TIMEOUT_BRIEF_PAUSE;

	DWORD rcWait;
	CDefTarget firstTargetWithTransientError;
	HANDLE hArrEvents[3] = 
	{
		GlobalData::newBlessed.GetHandle(),
		GlobalData::deliverSignature.GetHandle(),
		GlobalData::halt.GetHandle()
	};

	// loop forever (or until an explicit "break" is executed within this loop)
	while (true)
	{
		try
		{
			// We must wait until a "definitions are available" event to
			// become signaled. If so, we must build a list of targets for the definition 
			// set. We only need a non-infinite timeout so that we can periodically check 
			// for the value of the "stop" flag.

			// Must wait a non-zero timeout for "defs available" events when currently 
			// iterating through a non-empty listDist.  Can't wait 0, because we might 
			// always get a WAIT_TIMEOUT return even if an event is signaled.

			// Log the timeout value as an unsigned number of milliseconds,
			// remembering that it may have a value of 0xFFFFFFFF.  (EJP 7/18/00)

			char msg[300];
			sprintf(msg, "Waiting for %u.%03u seconds...", uiWaitTimeout/1000, uiWaitTimeout%1000);
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, msg);
			rcWait = ::WaitForMultipleObjects(3, hArrEvents, FALSE,	uiWaitTimeout);
			switch(rcWait)
			{
				case WAIT_TIMEOUT:
					Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Timeout occurred while waiting; proceeding to process the next target on the distribution list...");
					// no events were signaled; timeout can only happen when we are
					// iterating through a non-empty list
					// just go ahead and process the list, if it is not empty
					break;

				// the first event: a new blessed def is availabe
				case WAIT_OBJECT_0:
					Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Detected the download of a blessed definition; adding Blessed BroadCast targets to distribution list...");
					PushBlessedBroadcastTargetsOntoDistributionList();
					break;

				// the second event: a new unblessed def is availabe
				case WAIT_OBJECT_0 + 1:
					// only push targets for samples in the "available" state,
					// NOT samples in the "distribute" state. This logic prevents us
					// from pushing duplicates of samples in the "available" state.
					Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Detected the download of an unblessed definition; adding Unblessed targets to distribution list...");
					PushUnblessedTargetsOntoDistributionList();
					break;

				// the third event: a "stop" request was made
				case WAIT_OBJECT_0 + 2:
					Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Detected a 'stop' event.; ending the definition distribution thread.");
					// end this thread function
					return 0;

				default:
					// should never get here, since there are only two events
					break;
			}

			// the list may be empty because we didn't actually append any targets to the
			// list, and it was already empty.
			if (IsDistributionListEmpty())
			{
				// update the "wait" timeout interval
				uiWaitTimeout = TIMEOUT_EMPTY_DIST_LIST;

				// nothing to do, just go wait again
				continue;
			}

			// the list is non-empty; must process the next element in the list
			// before waiting again.

			// just set the "wait" timeout to zero (when checking if the events are 
			// signaled, return immediately to allow prompt processing of the next element
			// in the list
			uiWaitTimeout = TIMEOUT_BRIEF_PAUSE;

			// flag indicating that we need to query the target machine for installed
			// definitions
			bool boAttemptToQuery;

			// flag indicating that we need to deliver and install the definitions at 
			// the target
			bool boAttemptToDeliver;

			// flag indicating that processing of the target is not complete; we must
			// move the target to the back of the list
			bool boMoveToBackOfList;

			// flag indicating that a transient error occurred while communicating
			// with the target
			bool boTransientError = false;

			// unconditionally remove the element from the front (the current element)
			// of the list.  We may push it back on to the end of the list if there is a
			// transient error associated with delivering a definition to the target.
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Processing first target on distribution list...");
			CDefTarget dtListElement = PopFirstTargetOffDistributionList();

			// is the target a PointCast target?
			if (dtListElement.IsPointCastTarget())
			{
				Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The target is a PointCast target.");

				// does the sample still exist in quarantine?
				if (!dtListElement.SampleExistsInQuarantine())
				{
					Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The sample associated with the target has been removed from quarantine; the target will be removed from the distribution list.");

					// don't query target
					boAttemptToQuery = false;

					// don't deliver to target
					boAttemptToDeliver = false;

					// lose this target
					boMoveToBackOfList = false;
				}
				else
					boAttemptToQuery = true;
			}
			else
				boAttemptToQuery = true;

			// get the "needed" definition sequence number for the current target.
			ulong ulSeqNumNeeded;
			CDefTarget::ResultCode dtRC;
			int iDeliveryTimeout;

			if (boAttemptToQuery)
			{
				// only need to calculate a sequence number needed if we are going to
				// deliver definitions
				if (!dtListElement.DefsWereDelivered())
				{
					Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Querying target to determine if it needs new definitions.");

					if (dtListElement.NeedsBlessedDefsOnly())
					{
						// since the latest blessed is always equal to or later than the needed
						// blessed, just use the latest blessed.
						ulSeqNumNeeded = GlobalData::GetNewestBlessedSigAvailable();

						::sprintf(szMsg, "This target requires blessed definitions (seq num = %u).", ulSeqNumNeeded);
						Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);
					}
					else
					{
						// don't care if the latest defs are blessed, since we also have a 
						// latest blessed sequence number as well
						bool boDummy;

						// since the latest defs are always equal to or greater than the latest
						// needed (based on sequence number), just use the latest needed.
						ulSeqNumNeeded = GlobalData::GetNewestSigAvailable(boDummy);

						::sprintf(szMsg, "This target requires definitions that DO NOT need to be blessed (seq num = %u).", ulSeqNumNeeded);
						Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);
					}
				}

				// obtain the timeout interval for delivery functions
				iDeliveryTimeout = GlobalData::DefDeliveryTimeout();

				// get the sequence number installed on the target
				ulong ulSeqNumInstalled;

				dtRC = dtListElement.QueryInstalledDefs(
					iDeliveryTimeout, 
					&ulSeqNumInstalled);

				switch(dtRC)
				{
					case CDefTarget::Success:
						::sprintf(szMsg, "Successfully queried target; its installed definitions have sequence number %u.", ulSeqNumInstalled);
						Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

						// did we query in order to see if definitions we already delivered
						// have been installed, or did we query to see if the target
						// machine needs thd definitions that we want to send to it?
						if (dtListElement.DefsWereDelivered())
						{
							// did the target machine install the definitions we sent it
							// (or later)?
							if (ulSeqNumInstalled >= dtListElement.GetSeqNumDelivered())
							{
								// yes, we are done
								Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The target machine has installed the definitions we sent to it.");

								// mark the sample as being installed
								dtListElement.MarkAsInstalled();

								// we are finished with this target, so remove it from
								// the list
								boMoveToBackOfList = false;
							}
							else
							{
								Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The target machine has not yet installed the definitions that were delivered to it; will check again later...");

								// Although we have already sent definitions to the target,
								// the target machine does not have those definitions,
								// or later, installed; thus, we must query again later.
								// Do this by moving the target to the back of the list.
								boMoveToBackOfList = true;
							}

							// Since we already delivered the definition, no need to 
							// deliver again.
							boAttemptToDeliver = false;
						}
						else // defs have not yet been delivered
						{
							// check if the target already has a sufficient definition installed
							if (ulSeqNumInstalled >= ulSeqNumNeeded)
							{
								Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The target machine already has the required definitions; no need to deliver definitions to it.");

								// nothing to do; the target already has a suffcient definition 
								// level installed; we will remove the target from the 
								// distribution list.
								boAttemptToDeliver = false;

								// mark the sample as being installed
								dtListElement.MarkAsInstalled();

								// we will eventually remove the target from the list
								boMoveToBackOfList = false;
							}
							else 
							{
								Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The target machine needs to be sent definitions.");

								// the target machine needs a definition package newer than the
								// one that is currently installed on it; we need to deliver 
								// these defs
								boAttemptToDeliver = true;
							}

						}

						// clear the attention flag in the registry for "target"
						Attention::UnSet(Attention::Target);

						break;

					// for transient errors, move element to back of list
					case CDefTarget::TransientError:
						Logger::Log(Logger::LogWarning, Logger::LogIcePackDDS, "A temporary error occurred while querying the target; the definitions will be re-sent later.");

						boTransientError = true;

						// error querying the target: do not attempt to deliver the definition
						// to it
						boAttemptToDeliver = false;

						// try again later...
						boMoveToBackOfList = true;

						break;

					// for permanent errors, remove element from list
					case CDefTarget::PermanentError:
					case CDefTarget::ExceptionOccurred:
						Logger::Log(Logger::LogError, Logger::LogIcePackDDS, "A permanent error occurred while querying the target; the target will be removed from the distribution list.");

						// mark the sample status as "not installed"; the method will do
						// nothing if the target has no associated sample
						dtListElement.MarkAsNotInstalled();

						// error querying the target: do not attempt to deliver the definition
						// to it
						boAttemptToDeliver = false;

						// remove the target from the list, since this error is permanent
						boMoveToBackOfList = false;
						break;

					// any cases we don't know about
					default:
						Logger::Log(Logger::LogCriticalError, Logger::LogIcePackDDS, "Unknown result from querying target.");

						// mark the sample status as "not installed"; the method will do
						// nothing if the target has no associated sample
						dtListElement.MarkAsNotInstalled();

						// just get rid of the target
						boAttemptToDeliver = false;
						boMoveToBackOfList = false;

						Attention::Set(Attention::Target);
						break;
				}
			}

			// check for the "stop" flag
			if (GlobalData::stop)
			{
				// the "stop" flag was set; must terminate this thread.
				// just break out of this loop.
				// this flag might have been set while we were querying the target
				// machine.
				break;
			}

			// was query successful?
			if (boAttemptToDeliver)
			{
				Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Attempting to deliver definition to target...");

				// must construct a path to the definition file
				char szPackagePath[1024];
				struct _stat bufStat;

				// first try package with .vdb extension
				::sprintf(
					szPackagePath, 
					"%s\\%08u.%s", 
					GlobalData::DefLibraryDirectory().c_str(), 
					ulSeqNumNeeded,
					FILENAME_EXTENSION_VDB);

				// must determine whether the package is FatAlbert (.exe) or VDB (.vdb)
				// check if the file has a FatAlbert-type filename
				if (-1 == ::_stat(szPackagePath, &bufStat))
				{
					// assume that GetStatus failed because the file does not exist.
					// construct VDB-format signature filename
					// first try package with .exe extension
					::sprintf(
						szPackagePath, 
						"%s\\%08u.%s", 
						GlobalData::DefLibraryDirectory(), 
						ulSeqNumNeeded,
						FILENAME_EXTENSION_FATALBERT);

					// check if the file has a VDB-type filename
					if (-1 == ::_stat(szPackagePath, &bufStat))
					{
						::sprintf(szMsg, "Unable to find defintion package file in \"%s\" with sequence number %u.", GlobalData::DefLibraryDirectory(), ulSeqNumNeeded);
						Logger::Log(Logger::LogError, Logger::LogIcePackDDS, szMsg);

						// can't deliver the definition package
						boAttemptToDeliver = false;

						// can't do anything else with this target
						boMoveToBackOfList = false;
					}
				}

				if (boAttemptToDeliver)
				{
					::sprintf(szMsg, "Definition package path = \"%s\".", szPackagePath);
					Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

					// attempt to deliver the package to the target
					dtRC = dtListElement.DeliverDefs(iDeliveryTimeout, szPackagePath);

					switch(dtRC)
					{
						case CDefTarget::Success:	// function was successful

							Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Successfully delivered definition package to target.");

							if (dtListElement.IsPointCastTarget())
							{
								Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The target will be moved to the back of the distribution list, since we will need to query it to determine when the definitions have been installed.");

								// mark the sample status as "distributed"
								dtListElement.MarkAsDistributed(ulSeqNumNeeded);

								// Now that the definitions are delivered, move the target to
								// the back of the list. Later we will query the target to 
								// check if the definitions have been installed.
								boMoveToBackOfList = true;
							}
							else // not a PointCast target
							{
								Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The target will now be removed, since it is not a PointCast target.");

								// don't need to keep this target around, since there is
								// no sample associated with it; therefore, we don't need
								// to query the target to verify that the definitions were
								// installed there... only needed for displaying the
								// "distributed" or "installed sample states in quarantine 
								// console.
								boMoveToBackOfList = false;
							}

							// since we successfully delivered the definition to the target,
							// we can reset the first "transient error target" object.
							firstTargetWithTransientError.Clear();

							// clear the attention flag in the registry for "target"
							Attention::UnSet(Attention::Target);

							break;

						// transient errors
						case CDefTarget::TransientError:
							Logger::Log(Logger::LogWarning, Logger::LogIcePackDDS, "A temporary error occurred while delivering definitions to the target; the definitions will be re-sent later.");
							boTransientError = true;

							// error deilvering/installing the definition on the target: 
							// must move the element to the back of the list to try again
							boMoveToBackOfList = true;
							break;

						// permanent errors
						case CDefTarget::PermanentError:
						case CDefTarget::ExceptionOccurred:
							Logger::Log(Logger::LogError, Logger::LogIcePackDDS, "A permanent error occurred while delivering definitions to the target; the target will be removed from the distribution list.");

							// mark the sample status as "not installed"; the method will do
							// nothing if the target has no associated sample
							dtListElement.MarkAsNotInstalled();

							// remove the target from the list, since this error is permanent
							boMoveToBackOfList = false;
							break;

						// any cases we don't know about
						default:
							Logger::Log(Logger::LogCriticalError, Logger::LogIcePackDDS, "Unknown result from delivering definitions to target.");

							// mark the sample status as "not installed"; the method will do
							// nothing if the target has no associated sample
							dtListElement.MarkAsNotInstalled();

							// just get rid of the target
							boMoveToBackOfList = false;

							Attention::Set(Attention::Target);
							break;

					} // end switch

				} // end "inner" if (boAttemptToDeliver)

			} // end "outer" if (boAttemptToDeliver)

			// should we add the element to the back of the list (transient error or
			// need to query for installed state later)?
			if (boMoveToBackOfList)
			{
				// Any duplicate targets (that are associated with a sample) must be 
				// moved to the back of the list. We can't remove them, because their
				// associated samples must have their states updated along with the
				// original.
				MoveAllDuplicatesToBackOfList(dtListElement);

				// move the element from the front to the back of the list
				PushTargetToBackOfDistributionList(dtListElement);

				if (boTransientError)
				{
					// we know there was a transient error, so alert the administrator
					Attention::Set(Attention::Target);

					// have we started counting?
					if (firstTargetWithTransientError.IsClear())	// no
					{
						Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The current target will be remembered as the first one with a temporary error.");

						// remember the current element as the first target that had a
						// transient error
						firstTargetWithTransientError = GetTargetAtBackOfDistributionList();
					}
					// see if the two objects (the one at the start of the list and the one
					// we remembered) are the same.
					else if (GetTargetFromFrontOfDistributionList().IsDuplicate(firstTargetWithTransientError))
					{
						::sprintf(szMsg, "All targets on the distribution list had temporary errors; waiting for %u seconds to elapse before continuing...", GlobalData::DefDeliveryInterval() / 1000);
						Logger::Log(Logger::LogWarning, Logger::LogIcePackDDS, szMsg);
						uiWaitTimeout = GlobalData::DefDeliveryInterval();

						// all the targets on the distribution list are associated with
						// transient errors, and we have returned to the first one; thus,
						// we have completely iterated the list once, with all transient
						// errors.  We must go into a wait state before attempting to 
						// deliver definitions to this target.
/*
						rcWait = WaitForSingleObject(
							GlobalData::halt.GetHandle(),
							GlobalData::DefDeliveryInterval());	// timeout in milliseconds

						// detected a "stop" event
						if (rcWait == WAIT_OBJECT_0)
							return 0;

						if (rcWait == WAIT_TIMEOUT)
						{
							// just continue processing the distribution list
						}
						else
						{
							// some error occurred
						}
*/
					}
					else
					{
						// else the last n targets to which we attempted to deliver definitions
						// all had transient errors, but we haven't come around to the first
						// one yet, so just continue iterating...
					}
				}
				else
				{
					// the reason we moved the target to the back of the list was 
					// because we successfully delivered a definition, and need to
					// query the target later, in order to verify that the definition
					// was actually installed there.
				}
			}
			else // don't move the target to the back of the list
			{
				Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "The target will be discarded.");

				// since the original target is being removed from the list, we must
				// also remove all duplicates of this target.
				RemoveAllDuplicatesFromList(dtListElement);

				// clear the attention flag in the registry for "target"
				Attention::UnSet(Attention::Target);
			}

			// did we just remove the last element on the list?
			if (IsDistributionListEmpty())
			{
				// use the wait timeout interval for an empty list
				uiWaitTimeout = TIMEOUT_EMPTY_DIST_LIST;
			}
		}
		catch(...)
		{
			GlobalData::HandleException("JobPackager::ThreadHandlerProc()",
										Logger::LogIcePackDDS);
		}
	}

	return 0;
}
