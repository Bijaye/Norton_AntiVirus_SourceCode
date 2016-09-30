// Submittor.cpp: implementation of the Submittor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Submittor.h"
#include "Internal.h"

using namespace std;

const AVISDBException::TypeOfException	Submittor::exceptType =
													AVISDBException::etSubmittor;


bool Submittor::AddNew(string& id)
{
	bool	rc	= false;

	string	insert("INSERT INTO Submittors(SubmittorID) VALUES('");
	SQLStmt::PrepString(id);
	insert += id;
	insert += "')";

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
	{
		sql.ThrowException("");
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Submittor::Submittor(string& id) : isNull(true), submittorID(""),
							usageToday(0), usageThisWeek(0), usageThisMonth(0), usageThisYear(0)
{
	SQLINTEGER	retLength;
	string	select("SELECT UsageToday, UsageThisWeek, UsageThisMonth, UsageThisYear ");
	select	+= "FROM Submittors WHERE submittorID = '";
	select	+= id;
	select	+= "'";

	submittorID	= id;

	SQLStmt		sql(Internal::DefaultSession(), "Construct from id", exceptType);
	if (sql.ExecDirect(select)							&&
		sql.BindColumn(1, &usageToday,		&retLength)	&&
		sql.BindColumn(2, &usageThisWeek,	&retLength)	&&
		sql.BindColumn(3, &usageThisMonth,	&retLength)	&&
		sql.BindColumn(4, &usageThisYear,	&retLength)	&&
		sql.Fetch(true, false)							)
	{
		isNull		= false;
	}
	else if (100 == sql.LastRetCode())	// no such record exists!
	{
		isNull	= true;
	}
	else
		sql.ThrowException("");
}

Submittor::~Submittor()
{

}


string Submittor::ToDebugString()	// returns a string representation of the
{									// object suitable for debugging messages
	string	dbStr("Submittor{\n");

	if (isNull)
	{
		dbStr	+= "Is Null";
	}
	else
	{
		char	asChar[1024];
                memset(asChar,0,1024);
		dbStr	+= "submittorID = ";
		dbStr	+= submittorID;
		sprintf(asChar, "\nusageToday = %d\nusageThisWeek = %d\n", usageToday, usageThisWeek);
		dbStr	+= asChar;
		sprintf(asChar, "usageThisMonth = %d\nusageThisYear = %d\n", usageThisMonth, usageThisYear);
		dbStr	+= asChar;
	}

	dbStr	+= "}\n";


	return dbStr;
}
