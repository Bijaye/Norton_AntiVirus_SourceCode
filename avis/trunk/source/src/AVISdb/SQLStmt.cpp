//
//	IBM AntiVirus Immune System
//
//	File Name:	SQLStmt.cpp
//	Author:		Andy Klapper
//
//	This class represents an SQL statment.
//
//	Methods:
//	SQLStmt			Constructor
//	~SQLStmt		Destructor
//	ExecDirect		Execute a given SQL statement immediately
//	Prepare			Prepare an SQL statement, but wait on executing the statement
//	BindParameter	Bind a paramenter to a user variable.  Different versions
//					of this method exist for each type of variable that can be bound.
//	Execute			Execute a prepared SQL statement
//
//	$Log: /AVISdb/AVISdb/SQLStmt.cpp $
//
//1     8/19/98 9:10p Andytk
//Initial check in.
//
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <time.h>


#include <Logger.h>

#include "SQLStmt.h"
#include "Internal.h"

using namespace std;

const AVISDBException::TypeOfException	SQLStmt::sqlExceptType =
													AVISDBException::etSQLStmt;

//RETCODE* SQLStmt::retCode	= NULL;
/*
static DWORD tlsIndex = TLS_OUT_OF_INDEXES;

bool SQLStmt::ProcessInit()
{
	tlsIndex = TlsAlloc();

	return TLS_OUT_OF_INDEXES != tlsIndex;
}

bool SQLStmt::ProcessExit()
{
	if (TLS_OUT_OF_INDEXES != tlsIndex)
	{
		TlsFree(tlsIndex);
		tlsIndex = TLS_OUT_OF_INDEXES;
	}

	return true;
}

bool SQLStmt::ThreadInit()
{
	return true;
}

bool SQLStmt::ThreadExit()
{
	RETCODE	*retCode = (RETCODE *) TlsGetValue(tlsIndex);

	if (0 != retCode)
		delete retCode;

	return true;
}

RETCODE* SQLStmt::RetCode()
{
	if (TLS_OUT_OF_INDEXES == tlsIndex)
		throw AVISDBException(AVISDBException::etSQLStmt,
									"RetCode, TLS_OUT_OF_INDEXES == tlsIndex");

	RETCODE*	retCode = (RETCODE*) TlsGetValue(tlsIndex);

	if (NULL == retCode)
	{
		DWORD	error = GetLastError();

		if (NO_ERROR == error)
		{
			retCode = new RETCODE;
			TlsSetValue(tlsIndex, retCode);
		}
		else
		{
			DWORD	error	= GetLastError();
			char	msg[512];
			char	err[600];

			if (0 < FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
									error, 0, msg, 512, NULL))
			{
				sprintf(err, "RetCode, Unable to set retcode, system error message\n[%s]", msg);
			}
			else
				strcpy(err, "RetCode, Unable to set retcode, FormatMessage failed to return last error message");

			throw AVISDBException(AVISDBException::etSQLStmt, err);
		}
	}

	return retCode;
}
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef unsigned char uchar;


SQLStmt::SQLStmt(DBSession& session, const char* w, AVISDBException::TypeOfException eType) : 
					validHandle(false), validPrep(false), lastRetCode(0), stmtAsStr(""), where(w), exceptType(eType)
{
	if (!session.Valid())
		throw AVISDBException(AVISDBException::etSQLStmt, "Constructor, session not valid");


	lastRetCode = SQLAllocHandle(SQL_HANDLE_STMT, session.ConnectionHandle(),
								&sqlStatement);

	validHandle = (SQL_SUCCESS == lastRetCode || SQL_SUCCESS_WITH_INFO == lastRetCode);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
						PackageMsg("SQL Warning SQLStmt Constructor").c_str());

	if (!validHandle)
		throw AVISDBException(exceptType,
						PackageMsg("SQL Error SQLStmt Constructor").c_str());
}





SQLStmt::~SQLStmt()
{
	if (validHandle)
	{
		lastRetCode = SQLFreeHandle(SQL_HANDLE_STMT, sqlStatement);

		//
		//	throwing an exception from within a destructor will cause the application to
		//	exit without clean up, so all we can really do is log the error and hope for
		//	the best.

		if (SQL_SUCCESS_WITH_INFO == lastRetCode)
			Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
						PackageMsg("SQL Warning SQLStmt Destructor").c_str());
		else if (SQL_SUCCESS != lastRetCode)
			Logger::Log(Logger::LogCriticalError, Logger::LogAvisDB,
						PackageMsg("SQL Error SQLStmt Destructor").c_str());

	}

	sqlStatement = NULL;
}


SQLStmt::RetType SQLStmt::ExecDirect(std::string& stmt, bool logWarnings, bool throwExceptions)
{
	HandleCheck("ExecDirect");

	validPrep	= true;
	stmtAsStr	= stmt;
	std::string packageMsgString;
	int retryCount = 0;
	// We couldn't get the proper "reason code" on any SQL errors.
	//This string allows us to check if a deadlock occurred.
	char *deadlockString = "SQL0911N  The current transaction has been rolled back because of a deadlock or timeout.  Reason code \"2\".  SQLSTATE=40001";

	//Get a random seed.	
	srand( (unsigned)time( NULL ) );

	Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
		stmtAsStr.c_str());

	//Stay in a loop for 1000 times until we execute the SQL successfully.
	while (++retryCount < 1000)
	{
		lastRetCode = SQLExecDirect(sqlStatement, (uchar *) stmt.c_str(),
									SQL_NTS);
		if (SQL_SUCCESS != lastRetCode)
		{
			packageMsgString = PackageMsg("");
			//If the SQL Detailed Information string doesn't contain, the deadlock related
			//information, exit this loop.
			if (!strstr(packageMsgString.c_str(), deadlockString))
				break;
		}
		else
			break;

		//Wait between 1 to 5 seconds randomly.
		Sleep ((rand() % 4000)+1000);
	}


	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
		std::string fullMsg ("SQL Warning ExecDirect");
		fullMsg += packageMsgString;
		if (logWarnings)
			Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
						fullMsg.c_str());
	}
	else if (throwExceptions && SQL_SUCCESS != lastRetCode)
	{
		std::string fullMsg ("SQL Error ExecDirect");
		fullMsg += packageMsgString;
		throw AVISDBException(exceptType, fullMsg);
	}

	return RetCodeToRetType();
}


SQLStmt::RetType SQLStmt::Prepare(std::string& stmt, bool logWarnings, bool throwExceptions)
{
	HandleCheck("Prepare");

	stmtAsStr	= stmt;

	lastRetCode = SQLPrepare(sqlStatement, (uchar *) stmt.c_str(),
									SQL_NTS);

	validPrep = (SQL_SUCCESS == lastRetCode || SQL_SUCCESS_WITH_INFO == lastRetCode);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
		if (logWarnings)
			Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
						PackageMsg("SQL Warning Prepare").c_str());
	}
	else if (throwExceptions && SQL_SUCCESS != lastRetCode)
	{
		throw AVISDBException(exceptType, PackageMsg("SQL Error Prepare"));
	}

	return RetCodeToRetType();
}



SQLStmt::RetType SQLStmt::BindParameter(uint paramIndex, IOType ioType, short* arg)
{
	HandleCheck("BindParameter short");
	PrepCheck("BindParameter short");

	lastRetCode = SQLBindParameter(sqlStatement, paramIndex, ioType,
										SQL_C_SSHORT, SQL_SMALLINT, 0, 0,
										arg, sizeof(short), NULL);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
		char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindParameter short, parameter %d", paramIndex);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
		char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindParameter short, parameter %d", paramIndex);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}


SQLStmt::RetType SQLStmt::BindParameter(uint paramIndex, IOType ioType, ushort* arg)
{
	HandleCheck("BindParameter ushort");
	PrepCheck( "BindParameter ushort");

	lastRetCode = SQLBindParameter(sqlStatement, paramIndex, ioType,
										SQL_C_USHORT, SQL_SMALLINT, 0, 0,
										arg, sizeof(ushort), NULL);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
		char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindParameter ushort, parameter %d", paramIndex);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
		char	prefix[1024];
                memset(prefix,0,1024);

		sprintf(prefix, "SQL Error BindParameter ushort, parameter %d", paramIndex);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}

SQLStmt::RetType SQLStmt::BindParameter(uint paramIndex, IOType ioType, int* arg)
{
	HandleCheck("BindParameter int");
	PrepCheck( "BindParameter int");

	lastRetCode = SQLBindParameter(sqlStatement, paramIndex, ioType,
										SQL_C_SLONG, SQL_INTEGER, 0, 0,
										arg, sizeof(int), NULL);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
		char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindParameter int, parameter %d", paramIndex);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
          char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindParameter int, parameter %d", paramIndex);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}


SQLStmt::RetType SQLStmt::BindParameter(uint paramIndex, IOType ioType, uint* arg)
{
	HandleCheck("BindParameter uint");
	PrepCheck( "BindParameter uint");

	lastRetCode = SQLBindParameter(sqlStatement, paramIndex, ioType,
										SQL_C_ULONG, SQL_INTEGER, 0, 0,
										arg, sizeof(uint), NULL);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindParameter uint, parameter %d", paramIndex);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindParameter uint, parameter %d", paramIndex);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}


SQLStmt::RetType SQLStmt::BindParameter(uint paramIndex, IOType ioType, char* arg, uint byteLength)
{
	HandleCheck("BindParameter string");
	PrepCheck( "BindParameter string");

	lastRetCode = SQLBindParameter(sqlStatement, paramIndex, ioType,
										SQL_C_CHAR, SQL_CHAR,
										byteLength, 0, (PTR) arg,
										byteLength, NULL);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindParameter string (maxLen = %d), parameter %d",
						byteLength, paramIndex);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindParameter string (maxLen = %d), parameter %d",
							byteLength, paramIndex);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}


SQLStmt::RetType SQLStmt::BindParameter(uint paramIndex, IOType ioType, char* arg)
{
	HandleCheck("BindParameter bit");
	PrepCheck( "BindParameter bit");

	lastRetCode = SQLBindParameter(sqlStatement, paramIndex, ioType,

//										SQL_C_UTINYINT,	SQL_TINYINT, 1, 3,
//										SQL_C_STINYINT, SQL_TINYINT, 1, 3,

//										SQL_C_STINYINT,	SQL_BINARY, 1, 3,
//										SQL_C_CHAR,		SQL_BINARY, 1, 3,
//										SQL_C_BIT,		SQL_BINARY, 1, 3,
//										SQL_C_UTINYINT, SQL_BINARY, 1, 3,
//										SQL_C_BINARY,	SQL_BINARY, 1, 3,

//										SQL_C_BINARY, SQL_CHAR, 1, 3,
//										SQL_C_UTINYINT, SQL_CHAR, 1, 3,
										SQL_C_BIT, SQL_CHAR, 1, 3,
										arg, sizeof(char), NULL);


	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindParameter char, parameter %d", paramIndex);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindParameter char, parameter %d", paramIndex);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}


SQLStmt::RetType SQLStmt::BindParameter(uint paramIndex, IOType ioType, DateTime& dateTime)
{
	HandleCheck("BindParameter DateTime");
	PrepCheck( "BindParameter DateTime");

	lastRetCode = SQLBindParameter(sqlStatement, paramIndex, ioType,
										SQL_C_TIMESTAMP, SQL_TIMESTAMP,
										0, 0, (PTR) &dateTime.timeStamp,
										sizeof(TIMESTAMP_STRUCT), NULL);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindParameter DateTime, parameter %d", paramIndex);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindParameter DateTime, parameter %d", paramIndex);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}

	//bool	BindParameter(uint paramIndex, IOType ioType, std::string& string);

SQLStmt::RetType SQLStmt::BindColumn(uint column, short* arg, SQLINTEGER* retLength)
{
	HandleCheck("BindColumn short");

	lastRetCode = SQLBindCol(sqlStatement, column, SQL_C_SHORT, arg,
										sizeof(short), retLength);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindColumn short, column %d", column);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindColumn short, column %d", column);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}



SQLStmt::RetType SQLStmt::BindColumn(uint column, ushort* arg, SQLINTEGER* retLength)
{
	HandleCheck("BindColumn ushort");

	lastRetCode = SQLBindCol(sqlStatement, column, SQL_C_USHORT, arg,
										sizeof(ushort), retLength);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindColumn ushort, column %d", column);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindColumn ushort, column %d", column);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}




SQLStmt::RetType SQLStmt::BindColumn(uint column, int* arg, SQLINTEGER* retLength)
{
	HandleCheck("BindColumn int");

	lastRetCode = SQLBindCol(sqlStatement, column, SQL_C_SLONG, arg,
										sizeof(int), retLength);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindColumn int, column %d", column);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindColumn int, column %d", column);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}



SQLStmt::RetType SQLStmt::BindColumn(uint column, uint* arg, SQLINTEGER* retLength)
{
	HandleCheck("BindColumn uint");

	lastRetCode = SQLBindCol(sqlStatement, column, SQL_C_ULONG, arg,
										sizeof(uint), retLength);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindColumn uint, column %d", column);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindColumn uint, column %d", column);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}


	return RetCodeToRetType();
}



SQLStmt::RetType SQLStmt::BindColumn(uint column, char* arg, uint byteLength, SQLINTEGER* retLength)
{
	HandleCheck("BindColumn string");

//	retcode = SQLBindCol(sqlStatement, column, SQL_C_CHAR, (PTR) arg,
	lastRetCode = SQLBindCol(sqlStatement, column, SQL_C_BINARY, (PTR) arg,
										byteLength, retLength);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindColumn string (maxLength = %d), column %d", byteLength, column);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindColumn string (maxLength = %d), column %d", byteLength, column);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}



SQLStmt::RetType SQLStmt::BindColumn(uint column, char* arg, SQLINTEGER* retLength)
{
	HandleCheck("BindColumn bit");

	lastRetCode = SQLBindCol(sqlStatement, column, SQL_C_BINARY, arg,
									sizeof(char), retLength);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindColumn char, column %d", column);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindColumn char, column %d", column);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}


	return RetCodeToRetType();
}


SQLStmt::RetType SQLStmt::BindColumn(uint column, DateTime&	arg, SQLINTEGER* retLength)
{
	HandleCheck("BindColumn DateTime");

	lastRetCode	= SQLBindCol(sqlStatement, column, SQL_C_TIMESTAMP, &arg.timeStamp,
									sizeof(TIMESTAMP_STRUCT), retLength);


	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Warning BindColumn DateTime, column %d", column);

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
					PackageMsg(prefix).c_str());
	}
	else if (SQL_SUCCESS != lastRetCode)
	{
                char	prefix[1024];
                memset(prefix,0,1024);
		sprintf(prefix, "SQL Error BindColumn DateTime, column %d", column);

		throw AVISDBException(exceptType, PackageMsg(prefix));
	}

	return RetCodeToRetType();
}




SQLStmt::RetType SQLStmt::Execute(bool logWarnings, bool throwExceptions)
{
	HandleCheck("Execute");
	PrepCheck( "Execute");

	std::string packageMsgString;
	int retryCount = 0;
	// We couldn't get the proper "reason code" on any SQL errors.
	//This string allows us to check if a deadlock occurred.
	char *deadlockString = "SQL0911N  The current transaction has been rolled back because of a deadlock or timeout.  Reason code \"2\".  SQLSTATE=40001";

	//Get a random seed.
	srand( (unsigned)time( NULL ) );

	Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
		stmtAsStr.c_str());


	//Stay in a loop for 1000 times until we execute the SQL successfully.
	while (++retryCount < 1000)
	{
		lastRetCode = SQLExecute(sqlStatement);
		if (SQL_SUCCESS != lastRetCode)
		{
			packageMsgString = PackageMsg("");
			//If the SQL Detailed Information string doesn't contain, the deadlock related
			//information, exit this loop.
			if (!strstr(packageMsgString.c_str(), deadlockString))
				break;
		}
		else
			break;

		//Wait between 1 to 5 seconds randomly.
		Sleep ((rand() % 4000)+1000);
	}


	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
		std::string fullMsg ("SQL Warning Execute");
		fullMsg += packageMsgString;
		if (logWarnings)
		{
			Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
						fullMsg.c_str());
		}
	}
	else if (throwExceptions && SQL_SUCCESS != lastRetCode)
	{
		std::string fullMsg ("SQL Error Execute");
		fullMsg += packageMsgString;
		throw AVISDBException(exceptType, fullMsg);
	}

	return RetCodeToRetType();
}

SQLStmt::RetType SQLStmt::Fetch(bool logWarnings, bool throwExceptions)
{
	HandleCheck("Fetch");

	lastRetCode = SQLFetch(sqlStatement);

	if (SQL_SUCCESS_WITH_INFO == lastRetCode)
	{
		if (logWarnings)
		{
			Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
						PackageMsg("SQL Warning Fetch").c_str());
		}
	}
	else if (throwExceptions && SQL_SUCCESS != lastRetCode)
	{
		throw AVISDBException(exceptType, PackageMsg("SQL Error Fetch"));
	}

	return RetCodeToRetType();
}


SQLStmt::RetType SQLStmt::FreeCursor(bool logWarnings, bool throwExceptions)
{
	if (validHandle)
	{
		lastRetCode = SQLFreeStmt(sqlStatement, SQL_CLOSE);

		if (SQL_SUCCESS_WITH_INFO == lastRetCode)
		{
			if (logWarnings)
			{
				Logger::Log(Logger::LogWarning, Logger::LogAvisDB,
							PackageMsg("SQL Warning FreeCursor").c_str());
			}
		}
		else if (throwExceptions && SQL_SUCCESS != lastRetCode)
		{
			throw AVISDBException(exceptType, PackageMsg("SQL Error FreeCursor"));
		}

		return RetCodeToRetType();
	}
	 else
		return Error;
}


bool SQLStmt::GetErrorMessage(std::string& errorMessage)
{
	SQLCHAR		sqlState[6];
	SQLINTEGER	nativeError = 0;
	SQLCHAR		messageText[2048];
	SQLSMALLINT	textLengthPtr = 0;
	RETCODE		rc;
        memset((char*)messageText,0,2048);

	errorMessage	= "";
	messageText[0]	= '\0';

	for (int i = 1;
		 SQL_SUCCESS == (rc = SQLGetDiagRec( SQL_HANDLE_STMT, sqlStatement, i,
										sqlState, &nativeError,
										messageText, 1024, &textLengthPtr)) ||
		SQL_SUCCESS_WITH_INFO == rc;
		 i++)
	{
		if (0 < errorMessage.size())
			errorMessage += "\n";

		errorMessage += (char *) messageText;
		errorMessage += "nativeError = ";
		sprintf((char*)messageText, "%d (%x)", nativeError, nativeError);
		errorMessage += (char *)messageText;

		messageText[0] = '\0';
	}

	return 0 < errorMessage.size();
}


SQLStmt::ErrorType SQLStmt::GetErrorType()
{
	SQLSMALLINT			returnLength = 0;
	SQLINTEGER			nativeError = 0;
	SQLStmt::ErrorType	errorType;

	SQLGetDiagField(SQL_HANDLE_STMT, sqlStatement, 1, SQL_DIAG_NATIVE,
					&nativeError, SQL_IS_INTEGER, &returnLength);

	switch (nativeError)
	{
	case 0:
		errorType	= SQLStmt::NoError;
		break;

	case -803:
		errorType	= SQLStmt::CausedDuplicate;
		break;

	default:
		errorType	= UnknownDBError;
	}

	return errorType;
}

//
//	Currently we only search for the single quote character ('),
//	if found we turn it into two single quote characters (the
//	database recoginizes this as a single quote vs. the end of 
//	the string.
//
//	NOTE:	This method should NOT be called on character strings
//	that are bound to parsed statements!

bool SQLStmt::PrepString(std::string& str)
{
	for (int pos = 0; pos < str.length() && std::string::npos != (pos = str.find('\'', pos)); pos+=2)
		str.insert(pos, "'");

	return true;
}
