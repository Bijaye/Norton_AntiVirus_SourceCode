// SigsToBeExported.cpp: implementation of the SigsToBeExported class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SigsToBeExported.h"
#include "Internal.h"

#include <string>

using namespace std;

const AVISDBException::TypeOfException	SigsToBeExported::exceptType =
												AVISDBException::etSigsToBeExported;


bool SigsToBeExported::AddNew(Server& server, Signature& sig)
{
	bool	rc = false;

	string	insert("INSERT INTO SigsToBeExported(ServerID, SignatureSeqNum) VALUES (");
	char	asChar[1024];
        memset(asChar,0,1024);
	sprintf(asChar, " %hu, %hu )", server.ID(), sig.SequenceNumber());
	insert	+= asChar;

	SQLStmt	sql(Internal::DefaultSession(), "AddNew", exceptType);
	if (sql.ExecDirect(insert, true, false))
		rc	= true;
	else if (SQLStmt::CausedDuplicate == sql.GetErrorType())
		rc	= false;
	else
		sql.ThrowException("");
	return rc;
}


bool SigsToBeExported::AddForAllActiveServers(Signature& sig)
{
	bool	rc = false;

	string	insert("INSERT INTO SigsToBeExported(ServerID, SignatureSeqNum) ");
	char	asChar[1024];
        memset(asChar,0,1024);
	sprintf(asChar, "SELECT ServerID, %hu ", sig.SequenceNumber());
	insert	+= asChar;
	insert	+= "FROM Servers WHERE Active = 1";

	SQLStmt	sql(Internal::DefaultSession(), "AddForAllActiveServers", exceptType);
	if (sql.ExecDirect(insert, true, false))
		rc	= true;
	else if (SQLStmt::CausedDuplicate == sql.GetErrorType())
		rc	= false;
	else
		sql.ThrowException("");

	return rc;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SigsToBeExported::SigsToBeExported(Server& server, Signature& sig) :
										isNull(true), sigID(0), serverID(0)
{
	string	select("SELECT ServerID, SignatureSeqNum FROM SigsToBeExported ");
	select	+= "WHERE ServerID = ";
	char		asChar[1024];
        memset(asChar,0,1024);
	sprintf(asChar, "%hu", server.ID());
	select	+= asChar;
	select	+= " AND SignatureSeqNum = ";
	sprintf(asChar, "%u", sig.SequenceNumber());
	select	+= asChar;

	Get(select, *this, "SigsToBeExported");
}

SigsToBeExported::~SigsToBeExported()
{

}


bool SigsToBeExported::GetOldest(SigsToBeExported& stbe)
{
	string	select("SELECT ServerID, SignatureSeqNum FROM SigsToBeExported ");
	select += "ORDER BY SignatureSeqNum ASC";

	return Get(select, stbe, "GetOldest");
}

bool SigsToBeExported::GetOldestByServer(SigsToBeExported& stbe, Server& server)
{
	string	select("SELECT ServerID, SignatureSeqNum FROM SigsToBeExported ");
	select += "WHERE ServerID = ";
	char		asChar[1024];
        memset(asChar,0,1024);
	sprintf(asChar, "%hu", server.ID());
	select	+= asChar;
	select	+= " ORDER BY SignatureSeqNum ASC";

	return Get(select, stbe, "GetOldestByServer");
}

bool SigsToBeExported::RemoveFromDatabase()
{
	NullCheck("RemoveFromDatabase");

	string	remove("DELETE FROM SigsToBeExported WHERE ServerID = ");
	char	asChar[1024];
        memset(asChar,0,1024);
	sprintf(asChar, "%hu AND SignatureSeqNum = %hu", serverID, sigID);
	remove	+= asChar;

	SQLStmt		sql(Internal::DefaultSession(), "RemoveFromDatabase", exceptType);
	sql.ExecDirect(remove);

	return true;
}

bool SigsToBeExported::Get(string& select, SigsToBeExported& stbe,
							const char* where)
{
	stbe.Clear();

	SQLINTEGER	retLength;

	SQLStmt		sql(Internal::DefaultSession(), where, exceptType);
	if (sql.ExecDirect(select)							&&
		sql.BindColumn(1, &stbe.serverID, &retLength)	&&
		sql.BindColumn(2, &stbe.sigID,    &retLength)	&&
		sql.Fetch(true, false)							)
	{
		stbe.isNull		= false;
	}
	else if (100 != sql.LastRetCode())		// did not fail because the record doesn't exist
	{
		sql.ThrowException("");
	}

	return !stbe.IsNull();
}

string SigsToBeExported::ToDebugString()// returns a string representation of the
{										// object suitable for debugging messages
	string	dbStr("SigsToBeExported{\n");

	if (isNull)
	{
		dbStr	+= "Is Null";
	}
	else
	{
		char	asChar[1024];
                memset(asChar,0,1024);
		sprintf(asChar, "sigID = %d\nserverID = \n", sigID, serverID);
	}

	dbStr	+= "}\n";


	return dbStr;
}
