// AnalysisStateInfo.cpp: implementation of the AnalysisStateInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>

#include "AnalysisStateInfo.h"
#include "Internal.h"


using namespace std;


const ushort AnalysisStateInfo::NameLength	= 32;
const AVISDBException::TypeOfException	AnalysisStateInfo::exceptType =
																AVISDBException::etAnalysisStateInfo;


bool AnalysisStateInfo::AddNew(string& newName)
{
	if (newName.length() >= NameLength)
		throw AVISDBException(exceptType, "AddNew, newName.length() >= NameLength");

	bool		rc	= false;
	string		modNewName = newName;
	SQLStmt::PrepString(modNewName);	
	string	insert("INSERT INTO AnalysisStateInfo VALUES( ");
	insert	+= "(SELECT MAX(AnalysisStateID) + 1 FROM AnalysisStateInfo ) , '";
	insert	+= modNewName;
	insert	+= "')";

	SQLStmt		sql(Internal::DefaultSession(), "AddNew", exceptType);
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AnalysisStateInfo::AnalysisStateInfo(string& findName) :
							isNull(true), id(0), name("")
{
	if (findName.length() >= NameLength)
		throw AVISDBException(exceptType, "Construct by name, findName.length() >= NameLength");

	operator=(findName);
}


AnalysisStateInfo::AnalysisStateInfo(uint findID) :
							isNull(true), id(0), name("")
{
	SQLINTEGER	nameRetLength;
	char		asChar[1024];
	char		nameAsChar[1024];
        memset(asChar,0,1024);
        memset(nameAsChar,0,1024);
	sprintf(asChar, "%d", findID);
	string	select("SELECT Name FROM AnalysisStateInfo ");
	select	+= "WHERE AnalysisStateID = ";
	select	+= asChar;

	SQLStmt		sql(Internal::DefaultSession(), "Construct from id",
						exceptType);
	if (sql.ExecDirect(select)										&&
		sql.BindColumn(1, nameAsChar, NameLength, &nameRetLength)	&&
		sql.Fetch(true, false)										)
	{
		if (nameRetLength <= 0)
			throw AVISDBException(exceptType, "Construct from id, nameRetLength <= 0");
		if (nameRetLength >= NameLength)
			throw AVISDBException(exceptType, "Construct from id, nameRetLength >= NameLength");

		nameAsChar[nameRetLength]	= '\0';
		name	= nameAsChar;
		id		= findID;
		isNull	= false;
	}
	else if (100 == sql.LastRetCode())
	{
		isNull	= true;
	}
	else
		sql.ThrowException("");
}


AnalysisStateInfo::~AnalysisStateInfo()
{

}

//
//	ATK NOTE:	This would be a GREAT place to add id cacheing.  (You could 
//				reduce the number of calls to the database by cacheing id/name
//				pairs and checking the cache for a match before going to the
//				database.

AnalysisStateInfo& AnalysisStateInfo::operator=(string& value)
{
	if (value.length() >= NameLength)
		throw AVISDBException(exceptType, "operator=, value.length() >= NameLength");

	name	= "";
	isNull	= true;

	SQLINTEGER	idRetLength;
	string	modFindName(value);
	SQLStmt::PrepString(modFindName);
	string	select("SELECT AnalysisStateID FROM AnalysisStateInfo ");
	select	+= "WHERE Name = '";
	select	+= modFindName;
	select	+= "'";

	SQLStmt		sql(Internal::DefaultSession(), "operator=", exceptType);
	if (sql.ExecDirect(select)					&&
		sql.BindColumn(1, &id, &idRetLength)	&&
		sql.Fetch(true, false)					)
	{
		name	= value;
		isNull	= false;
	}
	else if (100 == sql.LastRetCode())	// if it does not exist add it
	{
		AddNew(value);
		sql.FreeCursor();

		SQLStmt		sql2(Internal::DefaultSession(), "operator=", exceptType);
		if (sql2.ExecDirect(select)					&&
			sql2.BindColumn(1, &id, &idRetLength)	&&
			sql2.Fetch()							)
		{
			name	= value;
			isNull	= false;
		}
	}
	else
		sql.ThrowException("");

	return *this;
}

#pragma warning ( disable : 4101 )

bool AnalysisStateInfo::Test()
{
	try
	{
		string	newStateName("Class Test 1");
		AddNew(newStateName);

		AnalysisStateInfo	asi(newStateName);
		string			nameBack = asi;

		bool	rc = asi.IsNull();
		AnalysisStateInfo	asi2(string("peek-a-boo"));

		rc = asi2.IsNull();
	}

	catch (AVISDBException& exception)
	{
	}

	return true;
}

#pragma warning ( default : 4101 )

uint AnalysisStateInfo::NoDetectID()
{
static uint noDetectID	= 0;

	if (0 == noDetectID)
	{
		AnalysisStateInfo	asi(AnalysisStateInfo::NoDetect());

		noDetectID	= asi.ID();
	}

	return noDetectID;
}

string AnalysisStateInfo::ToDebugString()	// returns a string representation of the
{											// object suitable for debugging messages
	string	dbStr("AnalysisStateInfo{\n");

	if (isNull)
	{
		dbStr	+= "Is Null";
	}
	else
	{
		char	asChar[1024];
                memset(asChar,0,1024);

		sprintf(asChar, "id = %d\nname = ", id);
		dbStr	+= asChar;
		dbStr	+= name;
		dbStr	+= "\n";
	}

	dbStr	+= "}\n";

	return dbStr;
}
