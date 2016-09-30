// AnalysisRequest.cpp: implementation of the AnalysisRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AnalysisRequest.h"
#include "AnalysisResults.h"
#include "Internal.h"

#include <CMClAutoLock.h>
#include <Logger.h>

#include <fstream>
#include <string>


using namespace std;

const AVISDBException::TypeOfException	AnalysisRequest::exceptType =
AVISDBException::etAnalysisRequest;


//
//	AnalysisRequest::AddNew
//
//	Arguments:
//		[in]	CheckSum	checkSum	checksum of the file submitted.  Used
//										to look for prior solutions in the
//										AnalysisRequest table or a currently
//										active AnalysisRequest.
//		[in]	ushort		priority	Set Priority field in newly created
//										AnalysisRequest field.
//		[in/out] AnalysisStateInfo state On entry contains the default state if
//										no prior result or request can be found.
//										On exit contains the state of this request.
//										Could be a final state, could be an intermediate
//										state (use piggyback to determine).
//		[in]	Submittor	submittor	sets the submittorID in newly created
//										AnalysisRequest record.
//		[out]	bool		piggyBack	Indicates if state was changed because no
//										match AnalysisResults records were found,
//										but a sutiable AnalysisRequest record
//										was found.
//		[out]	bool		localCookie	The cookie used to identify the newly
//										created AnalysisRequest record.
//
//	Returns:
//              true on completion.
//              Throws an AVISDBException if a database error occurs.
//
//	Throws:
//		AVISDBExceptions when state is null, submittor is null, or an sql error
//		occurs.
//
//	Modification History:
//
//      Wed Mar 08 03:13:52 2000 :Michael Wilson: Massive rework... 
//            Please see the fairly exhaustive inline comments for 
//            more details
// 
//	Author:		Andy Klapper
//



bool AnalysisRequest::AddNew(CheckSum& checkSum, ushort priority,
                             AnalysisStateInfo& state,
                             Submittor& submittor, //Platform& platform,
                             bool& piggyBacked, uint& localCookie)
{
  if (state.IsNull())
    throw AVISDBException(exceptType, "AddNew, State is null");
  if (submittor.IsNull())
    throw AVISDBException(exceptType, "AddNew, Submittor is null");
  //	if (platform.IsNull())
  //		throw AVISDBException(AVISDBException::AnalysisRequestAddNew,
  //									"Platform is null");
  
  //--------------------------------------------------------------
  // Initialize values of response parameters
  //--------------------------------------------------------------
  // Confirm this value AGAIN with Ed & Co just to be sure.
  state       = AnalysisStateInfo(AnalysisStateInfo::Receiving());
  piggyBacked = false;
  localCookie	= 0;
  //--------------------------------------------------------------
  
  bool	   rc = true;
  DateTime finished;
  uint	   sigSeqNum	 = 0;
  //--------------------------------------------------------------
  // Find out whether or not a sample with this checksum has a
  // corresponding row in the AnalysisResults table.  This will be the 
  // case if either:
  // 1) A duplicate of this sample has been processed to completion by 
  //    the Anti-Virus Immune System, and valid results exist in this
  //    database.
  // 2) The AnalysisResults table has been "primed" or "pre-loaded"
  //    with a database of files with known results (i.e. from a
  //    corpus of known-clean files)
  //--------------------------------------------------------------
  // Note that in Case #2 above, no "cookie" or incident number is
  // required or assigned.  
  // On a Gateway: "Final status" is returned directly in response to
  //               the submission transaction.
  // In the AnalysisCenter:  "Final Status" is immediately posted to
  //                         the appropriate gateway via checksum.
  //--------------------------------------------------------------
  // An added "wrinkle" of doing this in this manner is that if the
  // caller receives a cookie number of zero from this method (back up 
  // through the filter) then it will be responsible for retrieving
  // the appropriate information from the AnalysisResults table.
  //--------------------------------------------------------------
  std::string _debug_msg;
  char _other_debug_string[1024];
  memset (_other_debug_string,0,1024);
  
  AnalysisResults	results(checkSum);
  if (!results.IsNull())
    {
      _debug_msg = "DUPLICATE AnalysisResults[";
      _debug_msg.append(checkSum);
      _debug_msg.append("]");

      Logger::Log(Logger::LogDebug,
                  Logger::LogAvisDB,
                  _debug_msg.c_str());
      state		= results.SampleState();
      sigSeqNum	= results.GetSignatureSequenceNum();
    }
  else
    {

      //--------------------------------------------------------------
      // If there was no AnalysisResults record, look for a record in the
      // AnalysisRequest table with this checksum.  This will occur only
      // if a duplicate sample has already been submitted, but processing
      // on that sample by the Anti-Virus Immune System is NOT yet
      // complete. (otherwise it would have appeared as a result in the
      // query above.)
      //--------------------------------------------------------------
     
      //--------------------------------------------------------------
      // Clear the local 'finished timestamp'
      // (this is of questionable importance)
      //--------------------------------------------------------------
      finished.Null(true);
      
      //--------------------------------------------------------------
      // Build the SQL Select statement.
      //--------------------------------------------------------------
	  // Changed on Mar/27/2000. No need to check for the 'nodetect' state
	  // specifically. Because, a new row insertion will never set the analysis state
	  // to 'nodetect'.
	  /*
      string	findDuplicate("SELECT * FROM AnalysisRequest ar, ");
      findDuplicate	+= "AnalysisStateInfo asi ";
      findDuplicate	+= "WHERE ar.CheckSum = '";
      findDuplicate	+= checkSum;
      findDuplicate	+= "' AND asi.AnalysisStateID = ar.AnalysisStateID ";
      findDuplicate	+= "AND asi.Name <> 'nodetect' ";
      //       findDuplicate	+= "AND Imported IS NOT NULL ";
      //       findDuplicate	+= "AND Finished IS NULL";
	  findDuplicate	+= " fetch first 1 row only for read only optimize for 1 row";
	  */

	  //Replaced the previous block of code on Mar/27/2000 with the following.
      string	findDuplicate("SELECT * FROM AnalysisRequest ");
      findDuplicate	+= "WHERE CheckSum = '";
      findDuplicate	+= checkSum;
      findDuplicate	+= "'";
	  findDuplicate	+= " fetch first 1 row only for read only optimize for 1 row";
       
      //--------------------------------------------------------------
      // Create an AnalysisRequest object to receive the result (if
      // any) of the SQL select composed above.
      //--------------------------------------------------------------
      AnalysisRequest	request;
      
      //--------------------------------------------------------------
      // Execute The SQL select and populate the "request" object with 
      // the results of the FIRST ROW THAT SATISFIES THE SELECT.  That 
      // is to say AnalysisRequest::Get will perform the SQL select,
      // and fetch only the first row and populate the internal member 
      // variables of the request object from that row.
      //--------------------------------------------------------------
      Get(findDuplicate, request, "RequestByCheckSum");
      
      //--------------------------------------------------------------
      // If the request succeeded. (i.e. a row was found in the
      // AnalysisRequest table with the same checksum as the sample we 
      // are attempting to submit)
      //--------------------------------------------------------------
      if (!request.IsNull())
        {
                      
          //--------------------------------------------------------------
          // Return the 'cookie' and state of the retrieved row.
          // Since the checksums are the same, all results will be
          // duplicated.  Therefore, we do not even need to create a
          // row in the AnalysisRequest table, as we previously had
          // done.  This should save a tremendous amount of overhead.
          // Both in database storage and the process overhead for each
          // "sample submission" transaction (both on the AVIS Gateway 
          // and in the AnalysisCenter)
          //--------------------------------------------------------------
 
          state = request.State();
          localCookie = request.LocalCookie();
          
          // Use piggybacked flag in the meantime to indicate duplicate
          // in-process sample.

          piggyBacked = true;
          memset(_other_debug_string,0,1024);
          sprintf(_other_debug_string,"DUPLICATE REQUEST: cookie[%d],state:[%s]",
                  localCookie,
                  ((std::string)state).c_str()
                  );
          
          Logger::Log(Logger::LogDebug,
                      Logger::LogAvisDB,
                      _other_debug_string);
          
        }
      else // There is no duplicate in the AnalysisRequest table
        {
          _debug_msg = "NEW REQUEST checksum [";
          _debug_msg.append(checkSum);
          _debug_msg.append("]");

          Logger::Log(Logger::LogDebug,
                      Logger::LogAvisDB,
                      _debug_msg.c_str());
          //--------------------------------------------------------------
          // In this case, we have not found a similar sample anywhere 
          // in this database.  This means we actually have to insert
          // the record and begin processing on this sample.
          //--------------------------------------------------------------

          //--------------------------------------------------------------
          // asChar is a simple temporary holding space for converted
          // data.
          //--------------------------------------------------------------
          char	asChar[1024];
          memset(asChar,0,1024);

          //--------------------------------------------------------------
          // A std::string object used to convert DateTime
          // objects. (the DateTime class is defined in
          // "src/aviscommon/datetime.h"
          //--------------------------------------------------------------
          string	dbDateStr;

          //--------------------------------------------------------------
          // Build the SQL statement.
          //--------------------------------------------------------------
          string	insert_request("INSERT INTO AnalysisRequest(LocalCookie, CheckSum, ");
          insert_request	+= "Priority, Finished, AnalysisStateID, SubmittorID) VALUES( ";
          insert_request	+= "( SELECT NextLocalCookie FROM Globals WHERE AlwaysZero = 0 ), ";
          insert_request	+= "'";
          insert_request	+= checkSum;
          insert_request	+= "', ";
          sprintf(asChar, "%d", priority);
          insert_request	+= asChar;
          insert_request	+= " , ";
          if (finished.IsNull())
            insert_request	+= " NULL , ";
          else
            {
              bool	dummy;
              insert_request    += "'";
              insert_request	+= finished.AsDBString(dummy);
              insert_request	+= "' , ";
            }
          sprintf(asChar, "%d", state.ID());
          insert_request	+= asChar;
          insert_request	+= " , '";
          //--------------------------------------------------------------
          // "Prepare" the submittorID string. Presumably this method
          // converts escape characters and the like.
          //--------------------------------------------------------------
          string	submittorID	= submittor.ID();
          SQLStmt::PrepString(submittorID);

          insert_request	+= submittorID;
          //	insert	+= "', '";
          //	string	hwCorrelator = platform.ID();
          //	SQLStmt::PrepString(hwCorrelator);
          //	insert	+= hwCorrelator;
          insert_request	+= "')";

          //--------------------------------------------------------------
          // Create the SQLStmt object (this creates a "statement"
          // handle through the ODBC interface, using the connection handle
          // specified in "Internal::DefaultSession()."  There is one
          // database connection per thread by default.  This
          // uncertain scheme will most certainly change in the future.
          //--------------------------------------------------------------
          SQLStmt	insertSQL(Internal::DefaultSession(), "AddNew", exceptType);

          //--------------------------------------------------------------
          // Execute the SQL insert and stuff the row into the
          // database.  
          //--------------------------------------------------------------
          bool success = (SQLStmt::Error != insertSQL.ExecDirect(insert_request,true,false));
          //--------------------------------------------------------------
          // There's some explaining to be done here about how and why
          // this next block works. 
          //--------------------------------------------------------------
          // At this point in time, we've inserted a new row into the
          // AnalysisRequest table.  Internal to that insert operation 
          // there are two extremely important blocks of code.  One
          // you can see, and one that resides in the database schema.
          // 1) The "LocalCookie" number (which is the primary key on
          //    the AnalysisRequest table) is retrieved from the
          //    "Globals" table which maintains a column called
          //    "NextLocalCookie" which contains (as you might expect) 
          //    the NEXT sequentially-available LocalCookie number.  
          // 2) There is a database trigger, defined in
          //    "src/avisdb/scripts/createtriggers.sql" that says
          //    (essentially) the following:
          //     Immediately after an Insert on the AnalysisRequest
          //     table, go to the Globals table and, as an atomic
          //     operation, increment the NextLocalCookie value.
          //
          // The combination of these two pieces of code mean that:
          //
          // a) An AnalysisRequest is insured to have a unique cookie
          //    number assigned to it. (the insert operation would
          //    fail otherwise)
          // b) The "LocalCookie" assigned to the new AnalysisRequest
          //    record is (as an unfortunate side-effect) never
          //    actually returned to the application code (i.e. above
          //    or below this comment.)
          //
          // Therefore (bored yet ;)
          //
          //    We have to re-select the row from the database based
          //    on checksum (since we have now ensured that checksums
          //    in the AnalysisRequest table will be unique) in order
          //    to obtain the LocalCookie for this sample.
          //
          // See.  That was simple, right?... right?... Hello?
          //
          // Snoring, great.
          //--------------------------------------------------------------
          if (success)
            {
              //--------------------------------------------------------------
              // Now re-select the record from the database as discussed
              // above.
              // Use the same "request" object declared above as a
              // receive
              // Create the SQL select statement.
              //--------------------------------------------------------------
              std::string _reselect("select * from analysisrequest where");
              _reselect.append(" checksum = '");
              _reselect.append(checkSum);
              _reselect.append("'");
			  _reselect.append(" fetch first 1 row only for read only optimize for 1 row");

              //--------------------------------------------------------------
              // Issue the select and populate the object.
              //--------------------------------------------------------------
              Get(_reselect,request,"Reselecting inserted row");
              //--------------------------------------------------------------
              // Did it work (no GOOD reason why it shouldn't have,
              // but the world being what it is...)
              //--------------------------------------------------------------
              if (!request.IsNull())
                {
                  // Populate any response vars I may need
                  //  The ones I 'care' about are:
                  //--------------------------------------------------------------
                  //  AnalysisStateInfo& state, (the more difficult of 
                  //  the bunch)
                  //--------------------------------------------------------------
                  state = request.State();
                  //--------------------------------------------------------------
                  //  bool& piggyBacked, (this one's easy)
                  //--------------------------------------------------------------
                  piggyBacked = false;
                  //--------------------------------------------------------------
                  //  uint& localCookie (this is why we just
                  //  reselected this record)
                  //--------------------------------------------------------------
                  localCookie = request.LocalCookie();
                }
              else 
                // Unable to reselect the record we just inserted (very odd indeed)
                {
                  throw
                    AVISDBException(AVISDBException::etAnalysisRequest,
                                    "AnalysisRequest select failed in AnalysisRequest::AddNew()");
                }
              //--------------------------------------------------------------
            } // if the new AnalysisRequest insert succeeded.
          else
            {
              throw
                AVISDBException(AVISDBException::etAnalysisRequest,"AnalysisRequest insert failed in AnalysisRequest::AddNew()");
              // The AnalysisRequest insert failed
              // Throw a (gasp) AVISDBException
            } // Insert failed
        } // close the "else we must insert one" case
    } // Close the "we found no AnalysisResults record" case
  
  //--------------------------------------------------------------
  // Revisiting the parameters passed into us to be fulfilled:
  //--------------------------------------------------------------
  // AnalysisStateInfo& state 
  //     This was defaulted to "receiving" and only changed if we
  //     found a record in the AnalysisResults or AnalysisRequest
  //     tables.
  //--------------------------------------------------------------
  // bool& piggyBacked
  //     This was set to false and not touched.
  //--------------------------------------------------------------
  // uint& localCookie
  //     This was initialized to zero and modified only if no 
  //     record was found in the AnalysisResults table. (i.e. we
  //     either found an AnalysisRequest with the same checksum or 
  //     we had to create one.  Either way, we have set the cookie)
  //--------------------------------------------------------------
  return rc;
}


//
//	AnalysisRequest::FromRemoteCookie
//
//	Purpose:
//		Get AnalysisRequest record associated with a particular remote cookie/server
//		pair.
//
//	Arguments:
//		[in]	uint		remoteCookie	which remoteCookie to look for
//		[in]	Server		remoteServer	which remoteServer to look for
//		[out]	AnalysisRequest&	request	gets filled with a matching request or
//											gets cleared out and set to null depending
//											on whether or not a matching record can be found.
//
//	Returns:
//		true if a matching record is found, false if a matching record cannot be found.
//
//	Throws:
//		AVISDBException when remoteServer is null or something very screwy happens.
//
//	Algorithm:
//		Builds a sql select statement and fills request with the results.
//
//	Modification History:
//
//	Author:	Andy Klapper


bool AnalysisRequest::FromRemoteCookie(uint remoteCookie, Server& remoteServer,
                                       AnalysisRequest& request)
{
  string	select("SELECT * FROM AnalysisRequest WHERE RemoteCookie = ");
  char		asChar[1024];
  memset(asChar,0,1024);
  sprintf(asChar, "%d", remoteCookie);
  select	+= asChar;
  select	+= " AND RemoteServerID = ";
  sprintf(asChar, "%d", remoteServer.ID());
  select	+= asChar;

  return Get(select, request, "FromRemoteCookie");
}


//
//	AnalysisRequest::HighestPriority
//
//	Purpose:
//		Get AnalysisRequest record with the highest priority.
//
//	Arguments:
//		[out]	AnalysisRequest&	request	gets filled with a matching request or
//											gets cleared out and set to null depending
//											on whether or not a matching record can be found.
//
//	Returns:
//		true if a matching record is found, false if a matching record cannot be found.
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Builds a sql select statement and fills request with the results.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::HighestPriority(AnalysisRequest& request)
{
  string	select("SELECT * FROM AnalysisRequest ORDER BY Priority DESC");

  return Get(select, request, "HighestPriority");
}


//
//	AnalysisRequest::HighestPriorityInState
//
//	Purpose:
//		Get AnalysisRequest record with the highest priority in a given state.
//
//	Arguments:
//		[in]	AnalysisStateInfo&	state	state that request must be in to qualify
//		[out]	AnalysisRequest&	request	gets filled with a matching request or
//											gets cleared out and set to null depending
//											on whether or not a matching record can be found.
//
//	Returns:
//		true if a matching record is found, false if a matching record cannot be found.
//
//	Throws:
//		AVISDBException whenAnalysisStateInfo is null or something very screwy happens.
//
//	Algorithm:
//		Builds a sql select statement and fills request with the results.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::HighestPriorityInState(AnalysisStateInfo& state,
                                             AnalysisRequest& request)
{
  char	asChar[1024];
  memset(asChar,0,1024);
  sprintf(asChar, "%d", state.ID());
	
  string	select("SELECT * FROM AnalysisRequest WHERE AnalysisStateID = ");
  select	+= asChar;
//Commented the following line on Mar/24/2000.
//  select	+= " AND IMPORTED IS NOT NULL ";
  select        += " AND priority = ";
  select	+= "( SELECT MAX(Priority) FROM AnalysisRequest WHERE AnalysisStateID = ";
  select	+= asChar;
  select        += " ) ";
//Commented the following line on Mar/24/2000.
//  select        += " and IMPORTED is NOT NULL ) ";
  //  select	+= " ) ORDER BY Imported ASC";
    //Commented the following line on APR/12/2000.
  //select	+= " ORDER BY localcookie ASC";
  select	+= " fetch first 1 row only for read only optimize for 1 row";

  return Get(select, request, "HighestPriorityInState");

//Commented the following code block on Mar/24/2000.
/*  
  if (false == Get(select, request, "HighestPriorityInState"))
    return false;

  string    select1("SELECT * FROM AnalysisRequest WHERE CheckSum = \'");
  select1   += request.GetCheckSum();
  //  select1   += "\' ORDER BY Imported ASC";
  select1   += "\' AND IMPORTED IS NOT NULL ORDER BY localcookie ASC";
  select1   += " fetch first 1 row only for read only optimize for 1 row";

  return Get(select1, request, "HighestPriorityInState");
*/
}


//
//	AnalysisRequest::OldestNofityClient
//
//	Purpose:
//		Get the AnalysisRequest record with the oldest LastStateChange date and its
//		NotifyClient flag set to 1 (true).
//
//	Arguments:
//		[out]	AnalysisRequest	request	gets filled with a matching request or
//											gets cleared out and set to null depending
//											on whether or not a matching record can be found.
//
//	Returns:
//		true if a matching record is found, false if a matching record cannot be found.
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Builds a sql select statement and fills request with the results.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::OldestNotifyClient(AnalysisRequest& request)
{
  string	select("SELECT * FROM AnalysisRequest WHERE ");
  select	+= "NotifyClient = X'01' ORDER BY LastStateChange ASC";

  return Get(select, request, "OldestNotifyClient");
}


//
//	Get the AnalysisRequest record that has been waiting the longest
//	time to notify Symantec of a state change

//
//	AnalysisRequest::OldestNofitySymantec
//
//	Purpose:
//		Get the AnalysisRequest record with the oldest LastStateChange date and its
//		NotifySymantec flag set to 1 (true).
//
//	Arguments:
//		[out]	AnalsysisRequest	request	gets filled with a matching request or
//											gets cleared out and set to null depending
//											on whether or not a matching record can be found.
//
//	Returns:
//		true if a matching record is found, false if a matching record cannot be found.
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Builds a sql select statement and fills request with the results.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::OldestNotifySymantec(AnalysisRequest& request)
{
  string	select("SELECT * FROM AnalysisRequest WHERE ");
  select	+= "NotifySymantec = X'01' ORDER BY LastStateChange ASC";

  return Get(select, request, "OldestNotifySymantec");
}


//
//	AnalysisRequest::OldestNotifyClientForAGateway
//
//	Purpose:
//		Get the AnalysisRequest record with the following conditions;
//			NotifyClient flag is set to 1 (true)
//			RemoteServerID matches the one given as an argument.
//		Get the first AnalysisRequest record found given the above conditions
//		and the following ordering;
//			the highest priority and the lowest localCookie when informed is also not null
//			the highest priority and the lowest localCookie when informed is null and
//				imported is not null.
//
//	Arguments:
//		[in]	Server		remoteServer	which remoteServer to look for
//		[out]	AnalsysisRequest	request	gets filled with a matching request or
//											gets cleared out and set to null depending
//											on whether or not a matching record can be found.
//
//	Returns:
//		true if a matching record is found, false if a matching record cannot be found.
//
//	Throws:
//		AVISDBException when remoteServer is null or something very screwy happens.
//
//	Algorithm:
//		Builds a sql select statement with the first ordering and fills request with the results.
//		If the first ordering returns no matches then it tries the second ordering and fills
//		request with the results.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::OldestNotifyClientForAGateway(Server& server,
                                                    AnalysisRequest& request)
{
  char	buffer[1024];
  memset(buffer,0,1024);
  sprintf(buffer, "%d", server.ID());
  string	select("SELECT * FROM AnalysisRequest WHERE ");
  select	+= "NotifyClient = X'01' AND RemoteServerID = ";
  select	+= buffer;
  select	+= " AND Informed IS NOT NULL";
  select	+= " ORDER BY Priority DESC, LocalCookie ASC";

  if (!Get(select, request, "OldestNofityClientForAGateway"))
    {
      select	 = " SELECT * FROM AnalysisRequest WHERE ";
      select	+= " NotifyClient = X'01' AND RemoteServerID = ";
      select	+= buffer;
      select	+= " AND Informed IS NULL AND Imported IS NOT NULL";
      select	+= " ORDER BY Priority DESC, LocalCookie ASC";

      return Get(select, request, "OldestNofityClientForAGateway");
    }
  else 
    return true;
}

//
//	AnalysisRequest::AnalysisRequest()
//
//	Purpose:
//		Creates a null AnalysisRequest object, presumably so it can be used
//		with one of the above static methods.
//
//	Arguments:
//		None.
//
//	Returns:
//		None.
//
//	Throws:
//		None.
//
//	Algorithm:
//		Initialize the object to default values, set isNull to true.
//
//	Modification History:
//
//	Author:	Andy Klapper

AnalysisRequest::AnalysisRequest() : isNull(true), localCookie(0),
                                     checkSum(string("")), priority(0), state(0),
                                     notifyClient(false), notifySymantec(false),
                                     submittorID(""), remoteServerID(0),
                                     nullRemoteServer(true), remoteCookie(0), nullRemoteCookie(true)
{
  imported.Null(true);
  informed.Null(true);
  lastStateChange.Null(true);
}



//
//	AnalysisRequest::AnalysisRequest
//
//	Purpose:
//		Get AnalysisRequest record associated with a particular localCookie.
//
//	Arguments:
//		[in]	uint		localCookie		which localCookie to look for
//
//	Returns:
//		None
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Builds a sql select statement and fills request with the results.
//
//	Modification History:
//
//	Author:	Andy Klapper

AnalysisRequest::AnalysisRequest(uint lCookie) : isNull(true), localCookie(0),
                                                 checkSum(string("")), priority(0), state(0),
                                                 notifyClient(false), notifySymantec(false),
                                                 submittorID(""), remoteServerID(0),
                                                 nullRemoteServer(true), remoteCookie(0), nullRemoteCookie(true)
{
  imported.Null(true);
  informed.Null(true);
  lastStateChange.Null(true);

  string	select("SELECT * FROM AnalysisRequest WHERE LocalCookie = ");
  char		asChar[1024];
  memset(asChar,0,1024);
  sprintf(asChar, "%d", lCookie);
  select	+= asChar;
  select	+= " fetch first 1 row only for read only optimize for 1 row";

  Get(select, *this, "Get with local cookie");
}


//
//	AnalysisRequest::AnalysisRequest
//
//	Purpose:
//		Get AnalysisRequest record associated with a particular checkSum
//
//	Arguments:
//		[in]	CheckSum	cSum			which checkSum to look for (note more than one
//											record can have the same checksum).
//		[out]	AnalsysisRequest	request	gets filled with a matching request or
//											gets cleared out and set to null depending
//											on whether or not a matching record can be found.
//
//	Returns:
//		true if a matching record is found, false if a matching record cannot be found.
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Builds a sql select statement and fills request with the results.
//
//	Modification History:
//
//	Author:	Andy Klapper

AnalysisRequest::AnalysisRequest(CheckSum& cSum)  : isNull(true), localCookie(0),
                                                    checkSum(string("")), priority(0), state(0),
                                                    notifyClient(false), notifySymantec(false),
                                                    submittorID(""), remoteServerID(0),
                                                    nullRemoteServer(true), remoteCookie(0), nullRemoteCookie(true)
{
  imported.Null(true);
  informed.Null(true);
  lastStateChange.Null(true);

  string	select("SELECT * FROM AnalysisRequest ar WHERE ar.CheckSum = '");
  select	+= cSum;
  select	+= "'";
  select	+= " fetch first 1 row only for read only optimize for 1 row";

  Get(select, *this, "Get with checksum");
}



AnalysisRequest::~AnalysisRequest()
{

}


//
//	Modification methods
bool AnalysisRequest::Priority(ushort newPriority)
{
  if (IsNull())
    throw AVISDBException(exceptType, "Priority, AnalysisRequest is null");

  string	update("UPDATE AnalysisRequest SET Priority = ");
  char		asChar[1024];
  memset(asChar,0,1024);
  sprintf(asChar, "%d", newPriority);
  update	+= asChar;
  update	+= " WHERE LocalCookie = ";
  sprintf(asChar, "%d", localCookie);
  update	+= asChar;

  SQLStmt		sql(Internal::DefaultSession(), "Priority", exceptType);
  sql.ExecDirect(update);
  priority= newPriority;

  return true;
}


bool AnalysisRequest::Informed(DateTime& newInformed)
{
  if (IsNull())
    throw AVISDBException(exceptType, "Informed, AnalysisRequest is null");
  if (newInformed.IsNull())
    throw AVISDBException(exceptType, "Informed, newInformed is null");

  char		asChar[1024];
  memset(asChar,0,1024);
  string	dbDateStr;
  string	update("UPDATE AnalysisRequest SET Informed = '");
  newInformed.AsDBString(dbDateStr);
  update	+= dbDateStr;
  update	+= "'";
  update	+= " WHERE LocalCookie = ";
  sprintf(asChar, "%d", localCookie);
  update	+= asChar;

  SQLStmt		sql(Internal::DefaultSession(), "Informed",
                            exceptType);
  sql.ExecDirect(update);
  informed= newInformed;

  return true;
}

bool AnalysisRequest::Imported(DateTime& newImported)
{
  if (IsNull())
    throw AVISDBException(exceptType, "Imported, AnalysisRequest is null");
  if (newImported.IsNull())
    throw AVISDBException(exceptType, "Imported, newImported is null");

  char		asChar[1024];
  memset(asChar,0,1024);
  string	dbDateStr;
  string	update("UPDATE AnalysisRequest SET Imported = '");
  newImported.AsDBString(dbDateStr);
  update	+= dbDateStr;
  update	+= "'";
  update	+= " WHERE LocalCookie = ";
  sprintf(asChar, "%d", localCookie);
  update	+= asChar;

  SQLStmt		sql(Internal::DefaultSession(), "Imported", exceptType);
  sql.ExecDirect(update);
  imported= newImported;

  return true;
}


//
//	NOTE:  This method is only intended to be used by the gateway.
//			The Analysis Center/Data flow should use the FinalState
//			methods instead.
//			(The gateway needs this method to be able to sync the
//			date field to it's time).

bool AnalysisRequest::AnalysisFinished(DateTime& dateFinished)
{
  if (IsNull())
    throw AVISDBException(exceptType, "AnalysisFinished, AnalysisRequest is null");
  if (dateFinished.IsNull())
    throw AVISDBException(exceptType, "AnalysisFinished, dateFinished is null");

  char asChar[1024];
  memset(asChar,0,1024);
  string	dbDateStr;
  string	update("UPDATE AnalysisRequest SET Finished = '");
  dateFinished.AsDBString(dbDateStr);
  update	+= dbDateStr;
  update	+= "'";
  update	+= " WHERE LocalCookie = ";
  sprintf(asChar, "%d", localCookie);
  update	+= asChar;

  SQLStmt		sql(Internal::DefaultSession(), "AnalysisFinished", exceptType);
  sql.ExecDirect(update);
  finished= dateFinished;

  return true;
}

//
//	AnalysisRequest::State
//
//	Purpose:
//		Change the state of this record and any other records that have been imported,
//		but not finished, to this state, unless they are in the 'nodetect' state.
//
//	Arguments:
//		[in]	AnalysisStateInfo&	newState
//											Change this AnalysisRequest record and any
//											piggy backed records to this state.
//
//	Returns:
//		true if a matching record is found, false if a matching record cannot be found.
//
//	Throws:
//		AVISDBException when AnalysisRequest or newState is null or something
//		very screwy happens.
//
//	Algorithm:
//		Builds and excecute a sql statement.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::State(AnalysisStateInfo& newState)
{
  if (IsNull())
    throw AVISDBException(exceptType, "State, AnalysisRequest is null");
  if (newState.IsNull())
    throw AVISDBException(exceptType, "State, newState is null");

  char	asChar[1024];
  memset(asChar,0,1024);
  string	update("UPDATE AnalysisRequest SET AnalysisStateID = ");
  sprintf(asChar, "%d", newState.ID());
  update	+= asChar;
  update	+= " WHERE ( CheckSum = '";
  update	+= checkSum;
  update	+= "')";

//Commented the following block on Mar/24/2000.
/*
  update	+= "' AND AnalysisStateID <> ";
  sprintf(asChar, "%d ) ", AnalysisStateInfo::NoDetectID());
  update	+= asChar;

  update	+= "AND ( LocalCookie = ";
  sprintf(asChar, "%d", localCookie);
  update	+= asChar;
  update	+= " OR Imported IS NOT NULL) AND Finished IS NULL";
*/

  SQLStmt		sql(Internal::DefaultSession(), "State",
                            exceptType);
  sql.ExecDirect(update);
  state	= newState.ID();

  return true;
}


//
//	AnalysisRequest::FinalState
//
//	Purpose:
//		Do everything necessary to set an AnalysisRequest to it's final state.
//			Create an AnalysisResult record.
//			For all AnalysisResult records that have been imported, but not finished, or
//					in the 'nodetect' state, that have a checksum that matches this AnalysisRequest
//					record;
//				Set the Finished timestamp to the current date/time
//				Set the state to match the state given as an argument.
//
//	Arguments:
//		[in]	AnalysisStateInfo	newState
//											The state to set this and other pending requests to.
//		[in]	uint				sigSeqNum
//											The sequence number required to resolve the request.
//											Can be zero if the file is not infected.
//
//	Returns:
//		true.
//
//	Warning:
//		This method should only be called for final SUCCESSFUL states.  Use the FinalErrorState
//		method if the final state is an error state.
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Creates the AnalysisResults record.
//		Updates the state and finished columns with new values
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::FinalState(AnalysisStateInfo& newState, uint sigSeqNum)
{
  if (IsNull())
    throw AVISDBException(exceptType, "FinalState, AnalysisRequest is null");
  if (newState.IsNull())
    throw AVISDBException(exceptType, "FinalState, newState is null");

  if (Logger::WillLog(Logger::LogDebug, Logger::LogAvisDB))
    {
      char	buffer[1024];
      memset(buffer,0,1024);
      sprintf(buffer, "AnalysisRequest::FinalState(%s, %d)",
              newState.ToDebugString().c_str(), sigSeqNum);
      Logger::Log(Logger::LogDebug, Logger::LogAvisDB, buffer);
    }

  //    // I don't understand why this code is here.
  //    // Clearly, SignatureSequence sets the signature sequence number for
  //    // the current AnalysisRequest record, but what does that have to do
  //    // with a concrete Signature object?  It's not used at all.
  //    Signature	sig;
  if (0 != sigSeqNum)
    {
      //        sig	= Signature(sigSeqNum);
      SignatureSequence(sigSeqNum);
    }
  //-------------------------------------------------------------------
  // This needs to be performed conditionally.  It is distinctly
  // possible that this record already exists in the database (in the
  // case of a duplicate sample with final status)
  // I'll use the "established mechanism" to check (i.e. create an
  // AnalysisResults record with the given checksum.  If it's not
  // null, it already exists)
  //-------------------------------------------------------------------
  
  AnalysisResults _results(checkSum);
  if (_results.IsNull())
    {
      Logger::Log(Logger::LogDebug,Logger::LogAvisDB,"Adding AnalysisResults record.");
      AnalysisResults::AddNew(checkSum, sigSeqNum, newState);
    }
  else
    {
      std::string _logmsg("AnalysisResults record already existed with this checksum: ");
      _logmsg += checkSum;
      Logger::Log(Logger::LogDebug,Logger::LogAvisDB,_logmsg.c_str());
    }
  //-------------------------------------------------------------------
  
  char		asChar[1024];
  memset(asChar,0,1024);  
  string	update("UPDATE AnalysisRequest SET AnalysisStateID = ");
  sprintf(asChar, "%d", newState.ID());
  update	+= asChar;
  update	+= ", Finished = CURRENT TIMESTAMP";
  update	+= " WHERE ( CheckSum = '";
  update	+= checkSum;
  update	+= "')";

//Commented the following code block on Mar/24/2000.
/*
  update	+= "' AND AnalysisStateID <> ";
  sprintf(asChar, "%d ) ", AnalysisStateInfo::NoDetectID());
  update	+= asChar;
  update	+= "AND ( LocalCookie = ";
  sprintf(asChar, "%d", localCookie);
  update	+= asChar;
  update	+= " OR Imported IS NOT NULL)";
*/

  SQLStmt		sql(Internal::DefaultSession(), "FinalState_", exceptType);
  try
    {
      sql.ExecDirect(update);
    }
  catch(AVISDBException& e_)
    {
      if (sql.LastRetCode() != SQL_NO_DATA)
        throw e_;
    }
  AnalysisRequest	tmp(localCookie);
  finished= tmp.finished;
  
  state	= newState.ID();
  return true;
}


//
//	AnalysisRequest::FinalErrorState
//
//	Purpose:
//		Do everything necessary to set an AnalysisRequest to it's final error state.
//			For all AnalysisResult records that have been imported, but not finished, or
//					in the 'nodetect' state, that have a checksum that matches this AnalysisRequest
//					record;
//				Set the X-Error attribute to indicate the reason for failure.
//				Set the state to the state given as an argument
//				Set finished to the current date/time
//			Unless this request has not been fully imported, in which case do the above for
//					only this record.
//
//	Arguments:
//		[in]	AnalysisStateInfo	newState
//											The state to set this and other pending requests to.
//		[in]	string				error	The X-Error value for this and any piggybacked request.
//		[out]	AnalsysisRequest	request	gets filled with a matching request or
//											gets cleared out and set to null depending
//											on whether or not a matching record can be found.
//
//	Returns:
//		true if a matching record is found, false if a matching record cannot be found.
//
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Update the X-Error attribute
//		Update all piggy backed request records if past the import stage,
//			if not past the import stage then only modify this record.
//
//	Modification History:
//
//	Author:	Andy Klapper

// bool AnalysisRequest::FinalErrorState(AnalysisStateInfo& newState, string& error)
// {
// #if 0
//   if (IsNull())
//     throw AVISDBException(exceptType, "FinalState_, AnalysisRequest is null");
//   if (newState.IsNull())
//     throw AVISDBException(exceptType, "FinalState_, newState is null");

//   if (Logger::WillLog(Logger::LogDebug, Logger::LogAvisDB))
//     {
//       string	msg("AnalysisRequest::FinalErrorState(");
//       msg	+= newState.ToDebugString();
//       msg	+= " , \"";
//       msg	+= error.c_str();
//       msg	+= "\" )";

//       Logger::Log(Logger::LogDebug, Logger::LogAvisDB, msg.c_str());
//     }

//   Refresh();	// make sure I have an up to date record to base this on.

//   //  Attributes	attr(localCookie);

//   //  attr[AttributeKeys::Error()]	= error;

//   char		asChar[1024];
//   memset(asChar,0,1024);
//   string	update("UPDATE AnalysisRequest SET AnalysisStateID = ");
//   sprintf(asChar, "%d", newState.ID());
//   update	+= asChar;
//   update	+= ", Finished = CURRENT TIMESTAMP";

//   char		localCookieAsChar[1024];
//   memset(asChar,0,1024);
//   sprintf(localCookieAsChar, "%d", localCookie);

//   if (imported.IsNull())
//     {
//       update	+= " WHERE LocalCookie = ";
//       update	+= localCookieAsChar;
//     }
//   else
//     {
//       update	+= " WHERE ( CheckSum = '";
//       update	+= checkSum;
//       update	+= "')";

// //Commented the following code block on Mar/24/2000.
// /*
//       update	+= "' AND AnalysisStateID <> ";
//       sprintf(asChar, "%d ) ", AnalysisStateInfo::NoDetectID());
//       update	+= asChar;
//       update	+= "AND ( LocalCookie = ";
//       update	+= localCookieAsChar;
//       update	+= " OR Imported IS NOT NULL)";
// */
//     }

//   SQLStmt		sql(Internal::DefaultSession(), "FinalState_", exceptType);
//   sql.ExecDirect(update);


//   Refresh();
// #endif
//   return true;
// }


//
//	AnalysisRequest::NotifyClient
//
//	Purpose:
//		Mark a particular AnalysisRequest record as needing/not needing the client to be
//		notified of a state change.
//
//	Arguments:
//		[in]	bool			notified	mark notification needed true/false
//
//	Returns:
//		true.
//
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Build and execute sql string.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::NotifyClient(bool notified)
{
  if (IsNull())
    throw AVISDBException(exceptType, "NotifyClient, AnalysisRequest is null");

  char	asChar[1024];
  memset(asChar,0,1024);
  string	update("UPDATE AnalysisRequest SET NotifyClient = ");
  sprintf(asChar, "X'0%c'", (notified ? '1' : '0'));
  update	+= asChar;
  update	+= " WHERE LocalCookie = ";
  sprintf(asChar, "%d", localCookie);
  update	+= asChar;

  SQLStmt		sql(Internal::DefaultSession(), "NotifyClient", exceptType);
  sql.ExecDirect(update);
  notifyClient= notified;

  return true;
}

//
//	AnalysisRequest::NotifySymantec
//
//	Purpose:
//		Mark a particular AnalysisRequest record as needing/not needing Symantec to be
//		notified of a state change.
//
//	Arguments:
//		[in]	bool			notified	mark notification needed true/false
//
//	Returns:
//		true.
//
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Build and execute sql string.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::NotifySymantec(bool notified)
{
  if (IsNull())
    throw AVISDBException(exceptType, "NotifySymantec, AnalysisRequest is null");

  char		asChar[1024];
  memset(asChar,0,1024);
  string	update("UPDATE AnalysisRequest SET NotifySymantec = ");
  sprintf(asChar, "X'0%c'", (notified ? '1' : '0'));
  update	+= asChar;
  update	+= " WHERE LocalCookie = ";
  sprintf(asChar, "%d", localCookie);
  update	+= asChar;

  SQLStmt		sql(Internal::DefaultSession(), "NotifySymantec", exceptType);
  sql.ExecDirect(update);
  notifySymantec	= notified;

  return true;
}

//
//	AnalysisRequest::RemoteServerAndCookie
//
//	Purpose:
//		Update a particular AnalysisRequest record's remote server and cookie values.
//
//	Arguments:
//		[in]	Server			newServer	The server this request was sent to
//		[in]	uint			newCookie	The cookie returned by the remote server
//
//	Returns:
//		true.
//
//
//	Throws:
//		AVISDBException when AnalysisRequest is null, newServer is null, or
//		something very screwy happens.
//
//	Algorithm:
//		Build and execute sql string.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::RemoteServerAndCookie(Server& newServer, uint newCookie)
{
  if (IsNull())
    throw AVISDBException(exceptType, "RemoteServerAndCookie, AnalysisRequest is null");
  if (newServer.IsNull())
    throw AVISDBException(exceptType, "RemoteServerAndCookie, newServer is null");

  char	asChar[1024];
  memset(asChar,0,1024);
  string	update("UPDATE AnalysisRequest SET RemoteServerID = ");
  sprintf(asChar, "%d", newServer.ID());
  update	+= asChar;
  update	+= ", RemoteCookie = ";
  sprintf(asChar, "%d", newCookie);
  update	+= asChar;
  update	+= " WHERE LocalCookie = ";
  sprintf(asChar, "%d", localCookie);
  update	+= asChar;

  SQLStmt		sql(Internal::DefaultSession(), "RemoteServerAndCookie", exceptType);
  sql.ExecDirect(update);
  remoteServerID	= newServer.ID();
  remoteCookie	= newCookie;
  nullRemoteServer= false;
  nullRemoteCookie= false;

  return true;
}


bool AnalysisRequest::SignatureSequence(uint sigSeqNum)
{
#if 0
  char	localCookieAsChar[1024];
  char	sigSeqIDAsChar[1024];
  char	sigSeqAsChar[1024];
  char	noDetectIDAsChar[1024];
  memset(localCookieAsChar,0,1024);
  memset(sigSeqIDAsChar,0,1024);
  memset(sigSeqAsChar,0,1024);
  memset(noDetectIDAsChar,0,1024); 

  sprintf(localCookieAsChar, "%d", localCookie);
  sprintf(sigSeqIDAsChar, "%d", Attributes::SignatureSequenceID());
  sprintf(sigSeqAsChar, "%08d", sigSeqNum);
  sprintf(noDetectIDAsChar, "%d", AnalysisStateInfo::NoDetectID());

  string	insert("INSERT INTO Attributes ");
  insert	+= " SELECT DISTINCT LocalCookie , ";
  insert	+= sigSeqIDAsChar;
  insert	+= " , '";
  insert	+= sigSeqAsChar;
  insert	+= "' ";
  insert	+= " FROM AnalysisRequest";
  insert	+= " WHERE CheckSum = '";
  insert	+= checkSum;
  insert	+= "'";

//Commented the following code block on Mar/24/2000.
/*
  insert	+= "' AND AnalysisStateID <> ";
  insert	+= noDetectIDAsChar;
  insert	+= " AND ( LocalCookie = ";
  insert	+= localCookieAsChar;
  insert	+= " OR Imported IS NOT NULL ) AND Finished IS NULL";
*/

  // prevent duplicates in the insert statement
  insert	+= " AND LocalCookie NOT IN (SELECT LocalCookie FROM Attributes WHERE AttributeKey = ";
  insert	+= sigSeqIDAsChar;
  insert	+= " )";

  SQLStmt	sql(Internal::DefaultSession(), "SignatureSequence", exceptType);
  if (!sql.ExecDirect(insert, true, false) && 100 != sql.LastRetCode())
    sql.ThrowException("");
	
#endif
  return true;
}

//
//	AnalysisRequest::ChangeAllInStateToAntoherState
//
//	Purpose:
//		Changes all AnalysisRequest records that are currently in one state to
//		a new state.  (records that have already been finished are excluded from
//		the list of records that get changed).
//
//	Arguments:
//		[in]	AnalysisStateInfo	from	old state used to pick AnalysisRequests
//		[in]	AnalysisStateInfo	to		new state to set requests to
//
//	Returns:
//		true if anything was changed, false other wise?
//
//
//	Throws:
//		AVISDBException when AnalysisRequest is null, from or to state are null, or
//		something very screwy happens.
//
//	Algorithm:
//		Build and execute sql string.
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::ChangeAllInStateToAnotherState(AnalysisStateInfo& from,
                                                     AnalysisStateInfo& to)
{
  if (from.IsNull())
    throw AVISDBException(exceptType, "ChangeAllInStateToAnotherState, from state is null");
  if (to.IsNull())
    throw AVISDBException(exceptType, "ChangeAllInStateToAnotherState, to state is null");


  string		update("UPDATE AnalysisRequest SET AnalysisStateID = ");
  char		buffer[1024];
  memset(buffer,0,1024);
  sprintf(buffer, "%d WHERE AnalysisStateID = %d", to.ID(), from.ID());
  update	+= buffer;
  update	+= " AND FINISHED IS NULL";

  SQLStmt		sql(Internal::DefaultSession(), "ChangeAllInStateToAnotherState",
                            exceptType);
  if (!sql.ExecDirect(update, true, false) && 100 != sql.LastRetCode())
    {
      sql.ThrowException("");
    }

  return 100 != sql.LastRetCode();
}

//
//	AnalysisRequest::RemoveAll
//
//	Purpose:
//		Removes all AnalysisRequest records from the database.
//		THIS SHOULD ONLY BE CALLED FROM DEBBUGING CODE AND SHOULD NEVER BE CALLED FROM
//		PRODUCTION CODE!!!
//
//	Arguments:
//		None
//
//	Returns:
//		None
//
//	Throws:
//		AVISDBException when something very screwy happens.
//
//	Algorithm:
//		Build and execute sql string.
//
//	Modification History:
//
//	Author:	Andy Klapper

void AnalysisRequest::RemoveAll(void)
{
  string		deleteAll("DELETE FROM AnalysisRequest");

  SQLStmt		sql(Internal::DefaultSession(), "RemoveAll",
                            exceptType);
  sql.ExecDirect(deleteAll);
}


//	AnalysisRequest::Clear
//
//	Purpose:
//		Clears an AnalysisRequest record by returning it to the default, unassociated state.
//
//	Arguments:
//		None.
//
//	Returns:
//		None.
//
//	Throws:
//		Nothing.
//
//	Algorithm:
//		Assign every member to it's default state.
//
//	Modification History:
//
//	Author:	Andy Klapper

void AnalysisRequest::Clear(void)
{
  isNull			= true;
  localCookie		= 0;
  checkSum		= string("");
  priority		= 0;
  imported.Null(true);
  informed.Null(true);
  finished.Null(true);
  lastStateChange.Null(true);
  state			= 0;
  notifyClient	= false;
  notifySymantec	= false;
  submittorID		= "";
  remoteServerID	= 0;
  nullRemoteServer= true;
  remoteCookie	= 0;
  nullRemoteCookie= true;
}

//
//	AnalysisRequest::Get
//
//	Purpose:
//		Get the first record that matches this select statement
//
//	Arguments:
//		[in]	string				select	The select statement to use to pick
//											which AnalysisRequest object to return
//		[out]	AnalysisRequest		request	The AnalysisRequest object to fill with
//											the values of the selected record.
//		[in]	const char*			where	Where this method was called from.  Used
//											for warning logging and exception throwing
//											purposes to give a better idea of where
//											the problem was caused.
//
//	Returns:
//		true if a matching record was found, false if no matching record was found.
//
//
//	Throws:
//		AVISDBException when there is a problem with one of the columns in the selected
//		record or something very screwy happens.
//
//	Algorithm:
//		Clear the request object passed in, build and execute sql string based on
//
//	Modification History:
//
//	Author:	Andy Klapper

bool AnalysisRequest::Get(string& select, AnalysisRequest& request,
                          const char* where)
{
  bool		rc = false;

  request.Clear();

  SQLINTEGER	informedRetLength, remoteServerRetLength, remoteCookieRetLength;
  SQLINTEGER	dummyRetLength, checkSumRetLength, importedRetLength;
  SQLINTEGER	submittorRetLength;
  SQLINTEGER	lscRetLength, finishedRetLength;
  ushort		stateAsUShort;
  char		checkSumAsChar[CheckSum::MaxLength];
  char		submittorIDAsChar[Submittor::MaxLength];
  //	char		hwCorrelatorAsChar[Platform::MaxLength];
  char		notifyClientAsChar, notifySymantecAsChar;
  string		warningWhere("Get, called from ");
  warningWhere	+= where;

  SQLStmt		sql(Internal::DefaultSession(), warningWhere.c_str(), exceptType);
  sql.ExecDirect(select);
  sql.BindColumn(1, &request.localCookie, &dummyRetLength);
  sql.BindColumn(2, checkSumAsChar, CheckSum::CheckSumLength(),
                 &checkSumRetLength);
  sql.BindColumn(3, &request.priority, &dummyRetLength);
  sql.BindColumn(4, request.imported, &importedRetLength);
  sql.BindColumn(5, request.informed, &informedRetLength);
  sql.BindColumn(6, request.finished, &finishedRetLength);
  sql.BindColumn(7, request.lastStateChange, &lscRetLength);
  sql.BindColumn(8, &stateAsUShort, &dummyRetLength);
  sql.BindColumn(9, &notifyClientAsChar, &dummyRetLength);
  sql.BindColumn(10, &notifySymantecAsChar, &dummyRetLength);
  sql.BindColumn(11,submittorIDAsChar, Submittor::MaxLength, &submittorRetLength);
  //	sql.BindColumn(12,hwCorrelatorAsChar, Platform::MaxLength, &hwCorrelatorRetLength);
  sql.BindColumn(12,&request.remoteServerID, &remoteServerRetLength);
  sql.BindColumn(13,&request.remoteCookie, &remoteCookieRetLength);
  if (sql.Fetch(true, false))
    {
      if (checkSumRetLength <= 0)
        throw AVISDBException(exceptType,
                              (warningWhere += " checkSumRetLength <= 0"));
      if (checkSumRetLength >= CheckSum::CheckSumLength())
        throw AVISDBException(exceptType,
                              (warningWhere += " checkSumRetLength >= CheckSum::CheckSumLength()"));
      if (submittorRetLength <= 0)
        throw AVISDBException(exceptType,
                              (warningWhere += " submittorRetLength <= 0"));
      if (submittorRetLength >= Submittor::MaxLength)
        throw AVISDBException(exceptType,
                              (warningWhere += " submittorRetLength >= Submittor::MaxLength"));
      //		if (hwCorrelatorRetLength >= Platform::MaxLength)
      //			throw AVISDBException(type, "hwCorrelatorRetLength >= Platform::MaxLength");

      checkSumAsChar[checkSumRetLength] = '\0';
      request.checkSum = checkSumAsChar;

      if (importedRetLength > 0)
        request.imported.Null(false);
      if (informedRetLength > 0)
        request.informed.Null(false);
      if (finishedRetLength > 0)
        request.finished.Null(false);
      if (lscRetLength > 0)
        request.lastStateChange.Null(false);

      request.state	= stateAsUShort;
      request.notifyClient	= (0x01 == notifyClientAsChar);
      request.notifySymantec	= (0x01 == notifySymantecAsChar);

      submittorIDAsChar[submittorRetLength] = '\0';
      request.submittorID	= submittorIDAsChar;

      //		hwCorrelatorAsChar[hwCorrelatorRetLength] = '\0';
      //		request.hwCorrelator = hwCorrelatorAsChar;
      if (remoteServerRetLength > 0)
        request.nullRemoteServer	= false;
      if (remoteCookieRetLength > 0)
        request.nullRemoteCookie	= false;

      request.isNull	= false;
      rc		= true;
    }
  else if (100 == sql.LastRetCode())		// no matching records!
    {
      rc = false;
    }
  else
    sql.ThrowException("");

  return rc;
}


//
//	AnalysisRequest::ToDebugString
//
//	Purpose:
//		Creates a string that shows the values of each of the member variables suitable
//		for debugging/logging purposes.
//
//	Arguments:
//		None.
//
//	Returns:
//		The above mentioned string.
//
//	Throws:
//		Nothing.
//
//	Modification History:
//
//	Author:	Andy Klapper

string AnalysisRequest::ToDebugString()	// returns a string representation of the
{										// object suitable for debugging messages
  string	dbStr("AnalysisRequest{\n");

  if (isNull)
    {
      dbStr	+= "Is Null";
    }
  else
    {
      bool	okay;
      char	asChar[1024];
      memset(asChar,0,1024);
      sprintf(asChar, "localCookie = %d\n", localCookie);
      dbStr	+= asChar;
      dbStr	+= "CheckSum = ";
      dbStr	+= checkSum.ToDebugString();
      sprintf(asChar, "priority = %d\n", priority);
      dbStr	+= asChar;
      dbStr	+= "imported = ";
      if (imported.IsNull())
        dbStr	+= "Null";
      else
        dbStr	+= imported.AsHttpString(okay);
      dbStr	+= "\ninformed = ";
      if (informed.IsNull())
        dbStr	+= "Null";
      else
        dbStr	+= informed.AsHttpString(okay);
      dbStr	+= "\nfinished = ";
      if (finished.IsNull())
        dbStr	+= "Null";
      else
        dbStr	+= finished.AsHttpString(okay);
      dbStr	+= "\nlastStateChange = ";
      if (lastStateChange.IsNull())
        dbStr	+= "Null";
      else
        dbStr	+= lastStateChange.AsHttpString(okay);
      dbStr	+= "\nstate = ";
      AnalysisStateInfo	asi(state);
      if (asi.IsNull())
        dbStr	+= "Null";
      else
        dbStr	+= asi;
      sprintf(asChar, "\nnotifyClient = %s\nnotifySymantec = %s\n",
              notifyClient ? "True" : "False",
              notifySymantec ? "True" : "False");
      dbStr	+= asChar;
      dbStr	+= "submittorID = ";
      dbStr	+= submittorID;
      sprintf(asChar, "\nremoteServerID = %d\n", remoteServerID);
      dbStr	+= remoteServerID;
      sprintf(asChar, "\nnullRemoteServer = %s\nremoteCookie = %d\n",
              nullRemoteServer ? "True" : "False", remoteCookie);
      dbStr	+= asChar;
      sprintf(asChar, "remoteCookie = %d, nullRemoteCookie = %s\n",
              remoteCookie, nullRemoteCookie ? "True" : "False");
      dbStr	+= asChar;
    }

  dbStr	+= "}\n";

  return dbStr;
}

/*
This method was added on May/03/2000. This will be called
from the sample importer (AVISACSampleComm.exe) at the time of program
startup and shutdown. This method deletes all the samples from the ANALYSISREQUEST
table that are in the 'receiving' state.
*/
void AnalysisRequest::DeleteSamplesInReceivingState(Server gateway)
{
  char	asChar[1024];
  memset(asChar,0,1024);
  AnalysisStateInfo asi = AnalysisStateInfo(AnalysisStateInfo::Receiving());
  string	deleteStr("DELETE FROM AnalysisRequest Where AnalysisStateId = ");
  sprintf(asChar, "%d", asi.ID());
  deleteStr	+= asChar;
  deleteStr	+= " AND RemoteServerID = ";
  sprintf(asChar, "%d", gateway.ID());
  deleteStr	+= asChar;

  SQLStmt		sql(Internal::DefaultSession(), "DeleteSamplesInReceivingState", exceptType);
  sql.ExecDirect(deleteStr, true, false);
}
