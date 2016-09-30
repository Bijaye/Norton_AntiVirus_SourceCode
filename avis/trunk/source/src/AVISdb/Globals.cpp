// Globals.cpp: implementation of the Globals class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>

#include "Globals.h"
#include "Internal.h"

using namespace std;


Globals::GatewayType	Globals::gatewayType	= Globals::Unknown;
Server					Globals::thisServer;
ushort					Globals::version		= 0;
const AVISDBException::TypeOfException	Globals::exceptType = AVISDBException::etGlobals;


bool Globals::ThisServer(Server& server)
{
	bool	rc = false;

	if (thisServer.IsNull())
	{
		SQLINTEGER	retLength;
		string		select("SELECT ThisServerID FROM Globals WHERE AlwaysZero = 0");
		ushort		serverID;

		SQLStmt		sql(Internal::DefaultSession(), "ThisServer", exceptType);
		if (sql.ExecDirect(select)						&&
			sql.BindColumn(1, &serverID, &retLength)	&&
			sql.Fetch(true, false)						)
		{
			thisServer = Server(serverID);
			rc = true;
		}
	}
	else
		rc = true;

	server	= thisServer;

	return rc;
}

bool Globals::Type(GatewayType& type)
{
	bool	rc = false;

	if (Unknown == gatewayType)
	{
		SQLINTEGER	retLength;
		string	select("SELECT ServerType FROM Globals WHERE AlwaysZero = 0");

		SQLStmt		sql(Internal::DefaultSession(), "Type", exceptType);
		if (sql.ExecDirect(select)									&&
			sql.BindColumn(1, (ushort *) &gatewayType, &retLength)	&&
			sql.Fetch(true, false)									)
		{
			rc = true;;
		}
	}
	else
		rc = true;

	type = gatewayType;

	return rc;
}

bool Globals::NextLocalCookie(uint& nextLocalCookie)
{
	bool	rc = false;

	SQLINTEGER	retLength;
	string	select("SELECT NextLocalCookie FROM Globals WHERE AlwaysZero = 0");

	SQLStmt		getSQL(Internal::DefaultSession(), "NextLocalCookie", exceptType);
	if (getSQL.ExecDirect(select)							&&
		getSQL.BindColumn(1, &nextLocalCookie, &retLength)	&&
		getSQL.Fetch()										&&
		getSQL.FreeCursor())
	{
		string	update("UPDATE Globals SET NextLocalCookie = NextLocalCookie + 1");
		update	+= " WHERE AlwaysZero = 0";

		SQLStmt		incrementSQL(Internal::DefaultSession(), "NextLocalCookie", exceptType);
		if (incrementSQL.ExecDirect(update))
		{
			rc = true;
		}
	}

	return rc;
}

bool Globals::NewestSignature(Signature& sig)
{
	bool	rc = false;

	SQLINTEGER	retLength;
	string	select("SELECT NewestSeqNum FROM Globals WHERE AlwaysZero = 0");
	uint		seqNum;

	SQLStmt		sql(Internal::DefaultSession(), "NewestSignature", exceptType);
	if (sql.ExecDirect(select)					&&
		sql.BindColumn(1, &seqNum, &retLength)	&&
		sql.Fetch(true, false)					)
	{
		sig = Signature(seqNum);
		rc  = true;
	}

	return rc;
}

bool Globals::NewestBlessedSignature(Signature& sig)
{
	bool	rc = false;

	SQLINTEGER	retLength;
	string	select("SELECT NewestBlssdSeqNum FROM Globals WHERE AlwaysZero = 0");
	uint		seqNum;

	SQLStmt		sql(Internal::DefaultSession(), "NewestBlessedSignature", exceptType);
	if (sql.ExecDirect(select)					&&
		sql.BindColumn(1, &seqNum, &retLength)	&&
		sql.Fetch(true, false)					)
	{
		sig = Signature(seqNum);
		rc = true;
	}

	return rc;
}

bool Globals::DatabaseVersion(ushort& ver)
{
	bool	rc = false;

	if (0 == version)
	{
		SQLINTEGER	retLength;
		string	select("SELECT NewestSeqNum FROM Globals WHERE AlwaysZero = 0");

		SQLStmt		sql(Internal::DefaultSession(), "DatabaseVersion", exceptType);
		if (sql.ExecDirect(select)					&&
			sql.BindColumn(1, &version, &retLength)	&&
			sql.Fetch(true, false)					)
		{
			rc = true;
		}
	}
	else
		rc = true;

	ver	= version;

	return rc;
}

bool Globals::BackOfficeLock(bool& locked)
{
	bool		rc = false;
	SQLINTEGER	retLength;
	string	select("SELECT BackOfficeLock FROM Globals WHERE AlwaysZero = 0");

	SQLStmt		sql(Internal::DefaultSession(), "BackOfficeLock", exceptType);
	if (sql.ExecDirect(select)								&&
		sql.BindColumn(1, (ushort *) &locked, &retLength)	&&
		sql.Fetch(true, false)								)
	{
		rc = true;
	}

	return rc;
}

bool Globals::SetThisServer(Server& setThisServer)
{
	bool	rc = false;

	if (thisServer.ID() != setThisServer.ID())
	{
		string update("UPDATE Globals SET ThisServerID = ");
		char		asChar[1024];
                memset(asChar,0,1024);
		sprintf(asChar, "%d", setThisServer.ID());
		update	+= " WHERE AlwaysZero = 0";

		SQLStmt		sql(Internal::DefaultSession(), "SetThisServer", exceptType);
		if (sql.ExecDirect(update, true, false))
		{
			rc = true;
		}
	}
	else
		rc = true;

	return rc;
}

bool Globals::SetGatewayType(GatewayType& setGatewayType)
{
	bool	rc = false;

	if (gatewayType != setGatewayType)
	{
		string update("UPDATE Globals SET ServerType = ");
		char		asChar[1024];
                memset(asChar,0,1024);
		sprintf(asChar, "X'%02d'", setGatewayType);
		update	+= " WHERE AlwaysZero = 0";

		SQLStmt		sql(Internal::DefaultSession(), "GetGatewayType", exceptType);
		if (sql.ExecDirect(update, true, false))
		{
			rc = true;
		}
	}
	else
		rc = true;

	return rc;
}

bool Globals::SetBackOfficeLock(bool& locked)
{
	bool	rc = false;
	string update("UPDATE Globals SET BackOfficeLock = ");
	char		asChar[1024];
        memset(asChar,0,1024);
	sprintf(asChar, "X'%02d'", locked);
	update	+= " WHERE AlwaysZero = 0";

	SQLStmt		sql(Internal::DefaultSession(), "SetBackOfficeLock", exceptType);
	if (sql.ExecDirect(update, true, false))
	{
		rc = true;
	}

	return rc;
}


string Globals::ToDebugString()	// returns a string representation of the
{								// object suitable for debugging messages
	string	dbStr("Globals{\n");

	dbStr	+= "gatewayType = ";
	switch (gatewayType)
	{
	case Unknown:
		dbStr	+= "Unknown";	break;
	case Internet:
		dbStr	+= "Internet";	break;
	case AnalysisFront:
		dbStr	+= "AnalysisFront";	break;
	case AnalysisBack:
		dbStr	+= "AnalysisBack";	break;
	case CustomerAdmin:
		dbStr	+= "CustomerAdmin";	break;
	default:
		dbStr	+= "Really unknown type";
	}

	dbStr	+= "\nthisServer = ";
	dbStr	+= thisServer.ToDebugString();
	char	asChar[1024];
        memset(asChar,0,1024);
	sprintf(asChar, "\nversion = %d\n", version);
	dbStr	+= asChar;

	dbStr	+= "}\n";

	return dbStr;
}
