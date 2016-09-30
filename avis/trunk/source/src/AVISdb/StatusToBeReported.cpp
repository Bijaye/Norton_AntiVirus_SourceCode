// StatusToBeReported.cpp: implementation of the StatusToBeReported class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusToBeReported.h"
#include "Internal.h"

#include <string>

using namespace std;

const AVISDBException::TypeOfException	StatusToBeReported::exceptType =
												AVISDBException::etStatusToBeReported;

StatusToBeReported::~StatusToBeReported()
{

}

bool StatusToBeReported::GetOldestByServer(StatusToBeReported& stbr, Server& server)
{
	string	select("SELECT ServerID, Checksum FROM StatusToBeReported ");
	select += "WHERE ServerID = ";
	char		asChar[1024];
        memset(asChar,0,1024);
	sprintf(asChar, "%hu", server.ID());
	select	+= asChar;

	return Get(select, stbr, "GetOldestByServer");
}

bool StatusToBeReported::RemoveFromDatabase()
{
	NullCheck("RemoveFromDatabase");

	string	remove("DELETE FROM StatusToBeReported WHERE ServerID = ");
	char	asChar[1024];
        memset(asChar,0,1024);
	std::string strCheckSum = checksum;
	sprintf(asChar, "%hu AND Checksum = '%s'", serverID, strCheckSum.c_str());
	remove	+= asChar;

	SQLStmt		sql(Internal::DefaultSession(), "RemoveFromDatabase", exceptType);
	sql.ExecDirect(remove);

	return true;
}


bool StatusToBeReported::Get(string& select, StatusToBeReported& stbr,
							const char* where)
{
	stbr.Clear();

	SQLINTEGER	retLength, checkSumRetLength;

	SQLStmt		sql(Internal::DefaultSession(), where, exceptType);
    char		checkSumAsChar[CheckSum::MaxLength];
    string		warningWhere("Get, called from ");
    warningWhere	+= where;

	if (sql.ExecDirect(select)							&&
		sql.BindColumn(1, &stbr.serverID, &retLength)	&&
		sql.BindColumn(2, checkSumAsChar, CheckSum::CheckSumLength(),
                 &checkSumRetLength)	&&
		sql.Fetch(true, false)							)
	{
		stbr.isNull		= false;
        if (checkSumRetLength <= 0)
          throw AVISDBException(exceptType,
                              (warningWhere += " checkSumRetLength <= 0"));
        if (checkSumRetLength >= CheckSum::CheckSumLength())
          throw AVISDBException(exceptType,
                              (warningWhere += " checkSumRetLength >= CheckSum::CheckSumLength()"));
        checkSumAsChar[checkSumRetLength] = '\0';
        stbr.checksum = checkSumAsChar;
	}
	else if (100 != sql.LastRetCode())		// did not fail because the record doesn't exist
	{
		sql.ThrowException("");
	}

	return !stbr.IsNull();
}

/*
string StatusToBeReported::ToDebugString()// returns a string representation of the
{										// object suitable for debugging messages
	string	dbStr("StatusToBeReported{\n");

	if (isNull)
	{
		dbStr	+= "Is Null";
	}
	else
	{
		char	asChar[64];
		std::string strCheckSum = checksum;

		sprintf(asChar, "checksum = %s\nserverID = \n", strCheckSum.c_str(), serverID);
	}

	dbStr	+= "}\n";


	return dbStr;
}

bool StatusToBeReported::AddNew(Server& server, CheckSum& checksum)
{
	bool	rc = false;

	string	insert("INSERT INTO StatusToBeReported(ServerID, Checksum) VALUES (");
	char	asChar[64];
	std::string strCheckSum = checksum;
	sprintf(asChar, " %hu, '%s' )", server.ID(), strCheckSum.c_str());
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


bool StatusToBeReported::AddForAllActiveServers(CheckSum& checksum)
{
	bool	rc = false;

	string	insert("INSERT INTO StatusToBeReported(ServerID, Checksum) ");
	char	asChar[64];
	std::string strCheckSum = checksum;
	sprintf(asChar, "SELECT ServerID, '%s' ", strCheckSum.c_str());
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

StatusToBeReported::StatusToBeReported(Server& server, CheckSum& checksum) :
										isNull(true), checksum(string("")), serverID(0)
{
	string	select("SELECT ServerID, Checksum FROM StatusToBeReported ");
	select	+= "WHERE ServerID = ";
	char		asChar[64];
	std::string strCheckSum = checksum;
	sprintf(asChar, "%hu", server.ID());
	select	+= asChar;
	select	+= " AND Checksum = ";
	sprintf(asChar, "'%s'", strCheckSum.c_str());
	select	+= asChar;

	Get(select, *this, "StatusToBeReported");
}

bool StatusToBeReported::RemoveFromDatabase(Server& server, CheckSum& checksum)
{
	NullCheck("RemoveFromDatabase");

	string	remove("DELETE FROM StatusToBeReported WHERE ServerID = ");
	char	asChar[64];
	std::string strCheckSum = checksum;
	sprintf(asChar, "%hu AND Checksum = '%s'", server.ID(), strCheckSum.c_str());
	remove	+= asChar;

	SQLStmt		sql(Internal::DefaultSession(), "RemoveFromDatabase", exceptType);
	sql.ExecDirect(remove);

	return true;
}

*/
