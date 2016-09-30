//
//	IBM AntiVirus Immune System
//
//	File Name:	DBService.h
//	Author:		Andy Klapper
//
//	This class represents the AntiVirus Immune System
//	database.  It is recommended that the users of this
//	class open the database as early as reasonable and
//	close the database as soon as reasonable.  Please keep
//	in mind that it takes a couple of seconds to open and
//	a couple more seconds to close the database.  Do not open
//	and close the database on each request.  On the other hand
//	if the database will not be used for some extended period
//	of time it would be wise to close it.
//
//	$Log:  $
//	 
//

#ifndef DBSERVICE_H
#define DBSERVICE_H


//
//	Unfortunately the ODBC sql header (sqlext.h) needs to know about HWND, go figure.
#include "stdafx.h"

#include <sqlext.h>
#include <string>

#include "AVISDBException.h"

class DBService
{
public:

	//
	//	These functions open and close the database.  They log
	//	the opening and closing of the database and follow the
	//	gateway development standard error loging procedure as well.
	//
	//	Opening a database that is already opened has no effect.
	//	For every call to OpenService a call to CloseService 
	//	must also be made for the database to be closed.
	//
	//	Do not open and close the database frequently unless you 
	//	like to do a whole lot of waiting!
    static	bool	OpenService(const std::string& databaseName);
	static	bool	CloseService(void);
    static  bool    GetErrorMessage(std::string& errorMessage);

	static	SQLHANDLE	DatabaseHandle()
							{	UseCheck("DatabaseHandle");
								return sqlEnvironment; };
	static	bool		Name(std::string& dbName)
							{	UseCheck("Name");
								dbName = databaseName;	return true; };

	explicit DBService(std::string& dbName)		{ OpenService(dbName); };
			 DBService(const DBService& source)	{ OpenService(databaseName); };

	~DBService(void)	{ CloseService(); };

			bool		Valid(void)			{ return useCount > 0; };
	static	bool		ServiceValid(void)	{ return useCount > 0; };

private:
    static unsigned int	useCount;
    static SQLHANDLE	sqlEnvironment;
    static RETCODE		retcode;
	static std::string	databaseName;

	static void	UseCheck(const char* where)
	{
		if (0 == useCount)
		{
			std::string	msg(where);
			msg	+= ", DBService useCount is zero";
			throw AVISDBException(AVISDBException::etDBService, msg);
		}
	}
};


//
//	Note:	In the real version these could be classes in their
//			own right.  For now I need a couple of place holders.

#include "DateTime.h"

typedef	std::string	CustomerID;
typedef	std::string	GatewayID;

#endif
