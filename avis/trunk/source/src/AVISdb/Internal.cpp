// Internal.cpp: implementation of the Internal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <iostream>
#include <string>

#include <CMclCritSec.h>
#include <CMclAutoLock.h>

#include "Internal.h"

#include "SQLStmt.h"

#include <Logger.h>

using namespace std;

const AVISDBException::TypeOfException	Internal::exceptType =
													AVISDBException::etInternal;


//EXPIMP_TEMPLATE template class AVISDB_API std::string;

//__declspec(thread) DBSession*	session	= NULL;

static	DWORD	tlsIndex	= TLS_OUT_OF_INDEXES;

std::string	Internal::databaseName	= "AVISdata";

void Internal::OnThreadInit()
{
//	TlsSetValue(tlsIndex, NULL);	// unnecessary, system defaults value to 0

//	SQLStmt::ThreadInit();
}

//
//	If this thread has a default session still open
//	close the session.

void Internal::OnThreadExit()
{
//	SQLStmt::ThreadExit();


	DBSession	*session = (DBSession *) TlsGetValue(tlsIndex);

	if (NULL != session)
	{
		delete session;
		TlsSetValue(tlsIndex, NULL);	// I don't check if set to null, doesn't matter
		if (DBService::ServiceValid())
			DBService::CloseService();
	}
}


//
//	Try to close the service.  Since this may not be possible
//	and we don't want to go into an infinate loop only try
//	32 times.  (Nothing special about that number)

void Internal::OnProcessExit()
{
//	SQLStmt::ProcessExit();

	for (int i = 0; i < 32 && DBService::ServiceValid(); i++)
		DBService::CloseService();

	if (TLS_OUT_OF_INDEXES != tlsIndex)
	{
		TlsFree(tlsIndex);
		tlsIndex	= TLS_OUT_OF_INDEXES;
	}
}

void Internal::OnProcessInit()
{
	tlsIndex = TlsAlloc();

	if (TLS_OUT_OF_INDEXES == tlsIndex)
		throw AVISDBException(exceptType,
									"OnProcessInit, TLS_OUT_OF_INDEXES == tlsIndex");

//	SQLStmt::ProcessInit();
}

DBSession& Internal::DefaultSession()
{
	DBSession	*session = (DBSession *) TlsGetValue(tlsIndex);

	if (NULL == session)
	{
		static CMclCritSec	initializing;

		CMclAutoLock	lock(initializing);

		session	= (DBSession *) TlsGetValue(tlsIndex);	// just in case we were waiting
														// while another thread did
														// the initialization.

		if (NULL == session)
		{
			DBService::OpenService(databaseName);

			if (DBService::ServiceValid())
			{
//std::cerr << "before creating a new DBSession\n";
				session = new DBSession();

				if (NULL != session)
				{
					if (0 == TlsSetValue(tlsIndex, session))
					{
						DWORD	error	= GetLastError();
						char	msg[2048];
						char	err[2048];
                                                memset(msg,0,2048);
                                                memset(err,0,2048);

						if (0 < FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
												error, 0, msg, 2047, NULL))
						{
							sprintf(err, "DefaultSession, Unable to set session value, system error message\n[%s]", msg);
						}
						else
							strcpy(err, "DefaultSession, Unable to set session value, FormatMessage failed to return last error message");

						throw AVISDBException(exceptType, err);
					}
				}
			}
		}
	}

	return *session;
}
/*
bool Internal::LogRC(SQLStmt::RetType retType, SQLStmt& sql, const char * where)
{
	if (SQLStmt::Warning == retType)
	{
		string	warningDetails, logMsg("SQL Warning (");
		logMsg	+= where;
		logMsg	+= ") [";

		sql.GetErrorMessage(warningDetails);

		logMsg	+= "]";

		Logger::Log(Logger::LogWarning, Logger::LogAvisDB, logMsg.c_str()); 

		return true;
	}
	else
		return false;
}
*/
