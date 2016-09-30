// Platform.cpp: implementation of the Platform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Platform.h"
#include "Internal.h"



bool Platform::AddNew(std::string& id)
{
	bool	rc	= false;

	SQLStmt		sql(Internal::DefaultSession());
	RETCODE		retCode;
	std::string	insert("INSERT INTO Platforms(HWCorrelator) VALUES('");
	SQLStmt::PrepString(id);
	insert += id;
	insert += "')";

	if (sql.ExecDirect(insert, retCode))
	{
		rc = true;
	}
	else if (SQLStmt::CausedDuplicate == sql.GetErrorType())
	{
		rc = false;
	}
	else
	{
		std::string	message;
		sql.GetErrorMessage(message);
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Platform::Platform(std::string& id) : isNull(true), hwCorrelator(""),
							usageToday(0), usageThisWeek(0), usageThisMonth(0), usageThisYear(0)
{
	SQLStmt		sql(Internal::DefaultSession());
	RETCODE		execRetCode, fetchRetCode = 0;
	SQLINTEGER	retLength;
	std::string	select("SELECT UsageToday, UsageThisWeek, UsageThisMonth, UsageThisYear ");
	select	+= "FROM Platforms WHERE HWCorrelator = '";
	select	+= id;
	select	+= "'";

	hwCorrelator	= id;

	if (sql.ExecDirect(select, execRetCode)					&&
		sql.BindColumn(1, &usageToday,		&retLength)		&&
		sql.BindColumn(2, &usageThisWeek,	&retLength)		&&
		sql.BindColumn(3, &usageThisMonth,	&retLength)		&&
		sql.BindColumn(4, &usageThisYear,	&retLength)		&&
		sql.Fetch(fetchRetCode)								)
	{
		isNull		= false;
	}
	else if (100 == fetchRetCode)	// no such record exists!
	{
		isNull	= true;
	}
	else
		Internal::ThrowException(
							AVISDBException::TypeOfException::GetPlatform,
							sql, select);

}

Platform::~Platform()
{

}
