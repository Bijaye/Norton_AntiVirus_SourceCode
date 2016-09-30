// AnalysisResults.cpp: implementation of the AnalysisResults class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>

#include <Logger.h>

#include "AnalysisResults.h"
#include "Internal.h"

#include <stdlib.h>

using namespace std;

const AVISDBException::TypeOfException	AnalysisResults::exceptType =
AVISDBException::etAnalysisResults;

//
//	AnalysisResults::AddNew
//
//	Purpose:
//			Adds a new record to the AnalysisResults table.
//			Searchs for a prior result or currently active request with a
//			matching checksum to provide a new state value that can used
//			to move the sample more quickly through the analysis process.
//
//	Arguments:
//		[in]	CheckSum	checkSum	checksum of the sample that has reached a non-error
//										final result.
//		[in]	AnalysisStateInfo state Contains the final state to associate with checksum.
//
//	Returns:
//		True on success, false if an AnalysisResults record with the same checksum already
//		exists in the database.
//
//	Throws:
//		AVISDBExceptions when state is null or an sql error occurs.
//
//	Algorithm:
//		Create an SQL string and execute it.
//
//	Modification History:
//
//      09/13/99 19:21:16 MPW, removed all references to Signature objects
//
//	Author:		Andy Klapper
//


bool AnalysisResults::AddNew(CheckSum& checkSum, uint sig,
                             AnalysisStateInfo& state)
{
  if (Logger::WillLog(Logger::LogDebug, Logger::LogAvisDB))
    {
      string	msg("AnalysisResults::AddNew(");
      msg	+= checkSum.ToDebugString();
      msg	+= " , ";
      // Mikemod.  Now appends the sequence number
      //		msg	+= sig.ToDebugString();
      char _seqnum[10]; 
      ltoa(sig,_seqnum,10);
      msg += _seqnum;
      //
      msg	+= " , ";
      msg	+= state.ToDebugString();
      msg	+= " )";

      Logger::Log(Logger::LogDebug, Logger::LogAvisDB, msg.c_str());
    }

  if (state.IsNull())
    return false;

  bool	rc = false;
  string	insert("INSERT INTO AnalysisResults VALUES( '");
  insert	+= checkSum;
  char	asChar[1024];
  memset(asChar,0,1024);
  // Changed from this...
  //  	if (sig.IsNull())
  //  		strcpy(asChar, "', NULL, ");
  //  	else
  //  		sprintf(asChar, "', %d , ", sig.SequenceNumber());
  // To this...
  if (sig == 0)
    strcpy(asChar, "', NULL, ");
  else
    sprintf(asChar, "', %d , ", sig);
        
        
        
  insert	+= asChar;
  sprintf(asChar, "%d", state.ID());
  insert	+= asChar;
  insert	+= " )";

  SQLStmt	sql(Internal::DefaultSession(), "AddNew", exceptType);
  if (sql.ExecDirect(insert, true, false))
    {
      rc = true;
    }
  else if (SQLStmt::CausedDuplicate == sql.GetErrorType())
    {
      rc = false;
    }
  else
    sql.ThrowException("");

  return rc;
}

//
//	AnalysisResults::AnalysisResults
//
//	Purpose:
//			Constructs an AnalysisResults record based on the checksum
//
//	Arguments:
//		[in]	CheckSum	checkSum	searchs for an AnalysiResults record with this
//										checksum.
//
//	Returns:
//		None.
//
//	Throws:
//		AVISDBExceptions when something screwy happens.
//
//	Algorithm:
//		Create an SQL string and execute it.
//
//	Modification History:
//
//	Author:		Andy Klapper
//


AnalysisResults::AnalysisResults(CheckSum& cs) : isNull(true), 
  checkSum(cs), sigSeqNum(0), state(0)
{
  SQLINTEGER	sigRetLength, stateRetLength;
  string	select("SELECT SignatureSeqNum, AnalysisStateID ");
  select	+= "FROM AnalysisResults WHERE CheckSum = '";
  select	+= checkSum;
  select	+= "'";


  SQLStmt		sql(Internal::DefaultSession(), "Constructor", exceptType);
  if (sql.ExecDirect(select)							&&
      sql.BindColumn(1, &sigSeqNum, &sigRetLength)	&&
      sql.BindColumn(2, &state, &stateRetLength)		&&
      sql.Fetch(true, false)							)
    {
      isNull	= false;
    }
  else if (100 == sql.LastRetCode())		// nothing matched this checksum
    {
      isNull	= true;
    }
  else
    sql.ThrowException("");
}

AnalysisResults::~AnalysisResults()
{

}


//
//	AnalysisResults::ToDebugString
//
//	Purpose:
//			Creates a string that represents the values of all of the member variables.
//
//	Arguments:
//		None.
//
//	Returns:
//		A string that represents the values of all of the member variables.
//
//	Throws:
//		Nothing.
//
//	Modification History:
//
//	Author:		Andy Klapper
//

string AnalysisResults::ToDebugString()	// returns a string representation of the
{										// object suitable for debugging messages
  string	dbStr("AnalysisResults{\n");

  if (isNull)
    {
      dbStr	+= "Is Null";
    }
  else
    {
      char	asChar[1024];
      memset(asChar,0,1024);
      dbStr	+= "checkSum = ";
      dbStr	+= checkSum;
      sprintf(asChar, "\nsigSeqNum = %d\nstate = ", sigSeqNum);
      dbStr	+= asChar;
      AnalysisStateInfo	asi(state);
      dbStr	+= asi;
      dbStr	+= "\n";
    }

  dbStr	+= "}\n";

  return dbStr;
}
