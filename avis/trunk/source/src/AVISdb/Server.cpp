// Server.cpp: implementation of the Server class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>


#include "Server.h"
#include "Internal.h"


using namespace std;

const ushort							Server::URLLength	= 128;
const AVISDBException::TypeOfException	Server::exceptType	= AVISDBException::etServer;

bool Server::AddNew(string& url)
{
	if (url.length() >= URLLength)
		throw AVISDBException(exceptType, "AddNew, url.length() >= URLLength");

	_strlwr((char*) url.c_str());			// convert to lower case
	SQLStmt::PrepString(url);				// deal with any problem characters

	bool		rc = false;
	string	insert("INSERT INTO Servers(ServerID, URL) VALUES( ");
	insert	+= "(SELECT MAX(ServerID) + 1 FROM Servers) , '";
	insert	+= url;
	insert	+= "')";

	SQLStmt		insertSQL(Internal::DefaultSession(), "AddNew", exceptType);
	if (insertSQL.ExecDirect(insert, true, false))
		rc = true;
	else if (SQLStmt::CausedDuplicate == insertSQL.GetErrorType())
		rc = false;
	else
		insertSQL.ThrowException("");

	return rc;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Server::Server(string& URL)	: isNull(true), id(0), url(URL)
{
	if (url.length() >= URLLength)
		throw AVISDBException(exceptType,
									"Construct from URL, url.length() >= URLLength");

	_strlwr((char*) url.c_str());			// convert to lower case

	ushort		ac;
	SQLINTEGER	idRetLength, acRetLength;
	string	select("SELECT ServerID, Active FROM Servers WHERE URL = '");
	select	+= url;
	select	+= "'";


	SQLStmt		sql(Internal::DefaultSession(), "Construct from URL", exceptType);
	if (sql.ExecDirect(select)					&&
		sql.BindColumn(1, &id, &idRetLength)	&&
		sql.BindColumn(2, &ac, &acRetLength)	&&
		sql.Fetch(true, false)					)
	{
		isNull	= false;
		active	= (ac != 0);
	}
	else if (100 == sql.LastRetCode())
	{
		isNull	= true;
	}
	else
		sql.ThrowException("");
}

Server::Server(ushort serverID)	: isNull(true), id(0), url("")
{
	ushort		ac;
	SQLINTEGER	urlRetLength, acRetLength;
	char		asChar[20];
	string	select("SELECT URL, Active FROM Servers WHERE ServerID = ");
	sprintf(asChar, "%d", serverID);
	select	+= asChar;

	char		storedURL[URLLength];

	SQLStmt		sql(Internal::DefaultSession(), "Construct from id", exceptType);
	if (sql.ExecDirect(select)									&&
		sql.BindColumn(1, storedURL, URLLength, &urlRetLength)	&&
		sql.BindColumn(2, &ac, &acRetLength)					&&
		sql.Fetch(true, false)									)
	{
		if (urlRetLength < 1)
			throw AVISDBException(exceptType, "Construct from id, urlRetLength < 1");
		if (urlRetLength >= URLLength)
			throw AVISDBException(exceptType, "Construct from id, urlRetLength >= URLLength");

		storedURL[urlRetLength] = '\0';

		url		= storedURL;
		id		= serverID;
		active	= (ac != 0);
		isNull	= false;
	}
	else if (100 == sql.LastRetCode())
	{
		isNull	= true;
	}
	else
		sql.ThrowException("");
}

Server::~Server()
{

}


bool Server::URL(string& newURL)
{
	NullCheck("URL");
	if (newURL.length() >= URLLength)
		throw AVISDBException(exceptType, "URL, newURL.length() >= URLLength");

	_strlwr((char*) newURL.c_str());			// convert to lower case

	bool		rc = false;
	string	update("UPDATE Servers SET URL = '");
	update	+= newURL;
	update	+= "' WHERE ServerID = ";
	char		asChar[20];
	sprintf(asChar, "%d", id);
	update	+= asChar;

	SQLStmt		sql(Internal::DefaultSession(), "URL", exceptType);
	if (sql.ExecDirect(update))
	{
		url	= newURL;
		rc	= true;
	}

	return rc;
}

bool Server::ClearAllActive()
{
	string clear("UPDATE Servers SET Active = 0");

	SQLStmt		sql(Internal::DefaultSession(), "ClearAllActive", exceptType);
	sql.ExecDirect(clear);

	return true;
}

bool Server::SetActive(bool newState)
{
	string	update("UPDATE Servers SET Active = ");
	if (newState)
		update	+= "1 ";
	else
		update	+= "0 ";
	update	+= "WHERE ServerID = ";
	char		asChar[32];
	sprintf(asChar, "%hu", id);
	update	+= asChar;

	SQLStmt		sql(Internal::DefaultSession(), "SetActive", exceptType);
	if (sql.ExecDirect(update))
	{
		active = newState;
	}

	return true;
}


string Server::ToDebugString()	// returns a string representation of the
{										// object suitable for debugging messages
	string	dbStr("Server{\n");

	if (isNull)
	{
		dbStr	+= "Is Null";
	}
	else
	{
		char	asChar[64];

		sprintf(asChar, "id = %d\nurl = ", id);
		dbStr	+= asChar;
		dbStr	+= url;
		sprintf(asChar, "active = %s\n", active ? "True" : "False");
		dbStr	+= asChar;
	}

	dbStr	+= "}\n";


	return dbStr;
}
