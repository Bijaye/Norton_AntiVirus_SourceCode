//
//	IBM AntiVirus Immune System
//
//	File Name:	Internal.h
//
//	Author:		Andy Klapper
//
//	This class deals with issues internal to the AVISDB.dll
//
//	$Log:  $
//	
//

#if !defined(AFX_INTERNAL_H__4C842326_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_)
#define AFX_INTERNAL_H__4C842326_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DBSession.h"
#include "SQLStmt.h"
#include "AVISDBException.h"


class Internal  
{
public:
	static DBSession& DefaultSession(void);

	static void OnThreadInit(void);
	static void	OnThreadExit(void);
	static void OnProcessInit(void);
	static void	OnProcessExit(void);
	static void	DatabaseName(std::string& dbName) { databaseName = dbName; }

/*
	static void ThrowException(AVISDBException::TypeOfException typeOfException)
	{
		throw AVISDBException(typeOfException, std::string("no sql call related to failure"));
	}
*/

	static void ThrowException(AVISDBException::TypeOfException typeOfException, SQLStmt& sql)
	{
		std::string	detailedInfo;

		sql.GetErrorMessage(detailedInfo);
		throw AVISDBException(typeOfException, detailedInfo);
	}

	static void ThrowException(AVISDBException::TypeOfException typeOfException, SQLStmt& sql,
								std::string& sqlStr)
	{
		std::string	detailedInfo;

		sql.GetErrorMessage(detailedInfo);
		detailedInfo	+= "(";
		detailedInfo	+= sqlStr;
		detailedInfo	+= ")";

		throw AVISDBException(typeOfException, detailedInfo);
	}

	static void ThrowException(AVISDBException::TypeOfException typeOfException, SQLStmt& sql,
								std::string& sqlStr, const char* where)
	{
		std::string	msg(where);
		std::string	detailedInfo;

		sql.GetErrorMessage(detailedInfo);
		msg	+= " (";
		msg	+= sqlStr;
		msg	+= ")";

		throw AVISDBException(typeOfException, msg);
	}

//	static bool	LogRC(SQLStmt::RetType retType, SQLStmt& sql, const char * where);

private:
	static std::string	databaseName;

	Internal();
	virtual ~Internal();

	static const AVISDBException::TypeOfException	exceptType;
};

#endif // !defined(AFX_INTERNAL_H__4C842326_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_)
