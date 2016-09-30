//
//	$Header$
//
//	IBM AntiVirus Immune System
//
//	File Name:	AnalysisRequest.h
//
//	Author:		Andy Klapper
//
//	This class represents the AnalysisRequest table in the AVIS database.
//
//	$Revision$
//
//	$Log$
//	
//
//	IBM/Symantec Confidential
//

#if !defined(AFX_ANALYSISREQUEST_H__4342B4EB_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
#define AFX_ANALYSISREQUEST_H__4342B4EB_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "AVIS.h"
#include "AVISDBException.h"
#include "CheckSum.h"
#include "DateTime.h"
#include "Server.h"
#include "AnalysisStateInfo.h"
#include "Submittor.h"
//#include "Platform.h"
//#include "Attributes.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISDB_API AnalysisRequest  
{
public:
	//
	//	Create a new Analysis Request record.
	//	piggyBacked is an output variable and it indicates if this
	//	request has been piggy backed onto an earlier request
	static bool AddNew(CheckSum& checkSum, ushort priority,
						AnalysisStateInfo& state,
						Submittor& submittor, //Platform& hwCorrelator, 
						bool& piggyBacked, uint& localCookie);

	//
	//	Get an AnalysisRequest record with the given remote cookie
	static bool FromRemoteCookie(uint remoteCookie, Server& remoteServer,
									AnalysisRequest& request);

	//
	//	Get the AnalysisRequest record with the highest priority
	static bool	HighestPriority(AnalysisRequest& request);

	//
	//	Get the AnalysisRequest record with the highest priority that
	//	is in a given state
	static bool	HighestPriorityInState(AnalysisStateInfo& state,
										AnalysisRequest& request);

	//
	//	Get the AnalysisRequest record that has been waiting the longest
	//	time to notify it's client of a state change
	static bool	OldestNotifyClient(AnalysisRequest& request);

	//
	//	Get the first AnalysisRequest record in the following sort order;
	//		Finished records
	//			Priority
	//			LocalCookie (used to indicate relative age of the request)
	//		Intermediate records (not finished)
	//			Priority
	//			LocalCookie (used to indicate relative age of the request)
	//	For a given gateway that need to be informed of a change in request status.
	static bool OldestNotifyClientForAGateway(Server& server, AnalysisRequest& request);


	//
	//	Get the AnalysisRequest record that has been waiting the longest
	//	time to notify Symantec of a state change
	static bool	OldestNotifySymantec(AnalysisRequest& request);

	// Delete the samples that are left in the receiving state.
	static void DeleteSamplesInReceivingState(Server gateway);


	//
	//	Create a null AnalysisRequest, presumably so it can be used
	//	with one of the above static methods
	AnalysisRequest();

	//
	//	Get the AnalysisRequest record with a given local cookie
	explicit AnalysisRequest(uint localCookie);

	//
	//	Get the AnalysisRequest record with a given check sum value
	explicit AnalysisRequest(CheckSum& checkSum);


	virtual ~AnalysisRequest();

	//
	//	Access methods
	bool		IsNull(void)		{ return isNull; };
	uint		LocalCookie(void)	{ NullCheck("LocalCookie"); return localCookie; };
	CheckSum	GetCheckSum(void)	{ NullCheck("GetCheckSum"); return checkSum; };
	ushort		Priority(void)		{ NullCheck("Priority"); return priority; };
	DateTime	Imported(void)		{ NullCheck("Imported"); return imported; };
	DateTime	Informed(void)		{ NullCheck("Informed"); return informed; };
	DateTime	Finished(void)		{ NullCheck("Finished"); return finished; };
	DateTime	LastStateChange(void){NullCheck("LastStateChange"); return lastStateChange; };
	bool		NotifyClient(void)	{ NullCheck("NotifyClient"); return notifyClient; };
	bool		NotifySymantec(void){ NullCheck("NotifySymantec"); return notifySymantec; };
	Submittor	GetSubmittor(void)	{ NullCheck("GetSubmittor");
									  Submittor	submittor(submittorID);
									  return submittor; };
//	Platform	GetPlatform(void)	{ NullCheck(AVISDBException::AnalysisRequestGetPlatform); 
//									  Platform	platform(hwCorrelator);
//									  return platform; };
	Server		RemoteServer(void)	{ NullCheck("RemoteServer"); 
									  Server remoteServer(remoteServerID);
									  return remoteServer; };
	uint		RemoteCookie(void)	{ NullCheck("RemoteCookie"); return remoteCookie; };
  //	Attributes	GetAttributes(void)	{ NullCheck("GetAttributes"); Attributes att(localCookie); return att;};
	AnalysisStateInfo State(void)	{ NullCheck("State"); return AnalysisStateInfo(state); };

	//
	//	Modification methods
	bool		Priority(ushort newPriority);
	bool		Informed(DateTime& newInformed);
	bool		Imported(DateTime& newImported);
	bool		State(AnalysisStateInfo& newState);
	bool		FinalState(AnalysisStateInfo& newState, uint sigSeqNum);
// 	bool		FinalErrorState(AnalysisStateInfo& newState, std::string& error);
	bool		NotifyClient(bool notified);
	bool		NotifySymantec(bool notified);
	bool		RemoteServerAndCookie(Server& newServer, uint newCookie);
	bool		SignatureSequence(uint sigSeqNum);

	//
	//	NOTE:  This method is only intended to be used by the gateway.
	//			The Analysis Center/Data flow should use the FinalState
	//			methods instead.
	//			(The gateway needs this method to be able to sync the
	//			date field to it's time).

	bool		AnalysisFinished(DateTime& finishedDate);


	//
	//	Sets all AnalysisRequest records in one state to another state.
	//	returns true if any records had their state changed,
	//	returns false if no records were in the from state.
	static bool	ChangeAllInStateToAnotherState(AnalysisStateInfo& from,
													AnalysisStateInfo& to);

	void		Refresh(void)	{ NullCheck("Refresh"); 
								  AnalysisRequest	req(localCookie);
								  *this = req; };

	static void	RemoveAll(void);	// note this is only for testing
									// purposes only!

	std::string	ToDebugString(void);	// returns a string representation of the
										// object suitable for debugging messages

private:
	bool		isNull;
	uint		localCookie;
	CheckSum	checkSum;
	ushort		priority;
	DateTime	imported;
	DateTime	informed;
	DateTime	finished;
	DateTime	lastStateChange;
	ushort		state;
	bool		notifyClient;
	bool		notifySymantec;
	std::string	submittorID;
//	std::string	hwCorrelator;
	ushort		remoteServerID;
	bool		nullRemoteServer;
	uint		remoteCookie;
	bool		nullRemoteCookie;

	//
	//	Clear an analysis request object to it's initial(null) state
	void		Clear(void);
	//
	//	Get the first record that matches this select statement
	static bool Get(std::string& select, AnalysisRequest& request,
					const char * where);

	void		NullCheck(const char* where)
	{
		if (IsNull())
		{
			std::string	msg(where);
			msg	+= ", AnalysisRequest is null";
			throw AVISDBException(exceptType, msg);
		}
	}

	bool		FinalState_(AnalysisStateInfo& newState);

	static const AVISDBException::TypeOfException	exceptType;
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_ANALYSISREQUEST_H__4342B4EB_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
