//
//	IBM AntiVirus Immune System
//
//	File Name:	DatabaseClassesTest.h
//
//	Author:		Andy Klapper
//
//	This class tests the database classes that hide the use of ODBC
//	or DB/2's CLI interface.
//
//	$Log:  $
//	
//
#include "stdafx.h"

#include "DatabaseClassesTest.h"
#include "DBService.h"
#include "DBSession.h"
#include "SQLStmt.h"


using namespace std;

static const AVISDBException::TypeOfException	exceptType = AVISDBException::etDatabaseClassesTest;

//
//	DBServiceTest1
//
//	Create a couple of DBService objects, release those objects, make sure the
//		counter is zero and the database is closed.
//

bool DatabaseClassesTest::DBServiceTest1(Reporter& reporter)
{
	bool	rc = false;

	std::string	dbName("AVISdata");

	DBService	*db1	= new DBService(dbName);
	
	if (db1->Valid())
	{
		DBService	*db2	= new DBService(dbName);
		if (db2->Valid() && DBService::ServiceValid())
		{
			delete db1;
			if (db2->Valid() && DBService::ServiceValid())
			{
				delete db2;
				if (!DBService::ServiceValid())
				{
					rc = true;
				}
				else
					reporter.Failure(std::string("DBService: Database still valid when count should be 0"));
			}
			else
				reporter.Failure(std::string("DBService: Database invalid when count should be > 1"));
		}
		else
			reporter.Failure(std::string("DBService: Unable to open database again"));
	}
	else
		reporter.Failure(std::string("DBService: Unable to open database"));

	return rc;
}


static bool DBServiceFooBar(DBService	db, Reporter& reporter)
{
	bool	rc	= true;

	if (!db.Valid())
	{
		reporter.Failure(std::string("DBService: invalid DBService object copyed to DBServiceFooBar function."));
		rc = false;
	}

	return rc;
}

//
//	DBServiceTest2
//
//	Create a copy of an DBService object through a pass by value function call,
//		make sure the count does not get messed up.
//

bool DatabaseClassesTest::DBServiceTest2(Reporter& reporter)
{
	bool	rc = false;

	std::string	dbName("AVISdata");

	DBService	*db	= new DBService(dbName);
	
	
	if (db->Valid())
	{
		if (DBServiceFooBar(*db, reporter))
		{
			if (db->Valid())
			{
				delete db;

				if (!DBService::ServiceValid())
				{
					rc = true;
				}
				else
					reporter.Failure(std::string("DBService: Pass DBService object by value test failed(1)"));
			}
			else
				reporter.Failure(std::string("DBService: Pass DBService object by value test failed(2)"));
		}
		else
			reporter.Failure(std::string("DBService: Pass DBService object by value test failed(3)"));
	}
	else
		reporter.Failure(std::string("DBService: Pass DBService object by value test failed(unable to create valid DBService object)"));

	return rc;
}

//	Make sure the Open/Close database calls handle the count properly
//
bool DatabaseClassesTest::DBServiceTest3(Reporter& reporter)
{
	bool	rc = false;

	if (DBService::OpenService("AVISdata"))
	{
		if (DBService::ServiceValid())
		{
			if (DBService::OpenService("AVISdata"))
			{
				if (DBService::ServiceValid())
				{
					if (DBService::CloseService())
					{
						if (DBService::ServiceValid())
						{
							if (DBService::CloseService())
							{
								if (!DBService::ServiceValid())
								{
									rc = true;
								}
								else
									reporter.Failure(std::string("DBService: CloseService did not close the database when it should have"));
							}
							else
								reporter.Failure(std::string("DBService: second call to CloseService returned false"));
						}
						else
							reporter.Failure(std::string("DBService: One call to CloseService closed database when two calls should have been necessary"));
					}
					else
						reporter.Failure(std::string("DBService: CloseService call returned false"));
				}
				else
					reporter.Failure(std::string("DBService: ServiceValid and return code from second call to OpenService in conflict"));
			}
			else
				reporter.Failure(std::string("DBService: Unable to call OpenService twice"));
		}
		else
			reporter.Failure(std::string("DBService: ServiceValid and return code from OpenService in conflict"));
	}
	else
		reporter.Failure(std::string("DBService: Unable to open the database with OpenService"));

	return rc;
}

//	Make sure DBService only works with one database name
//	(This assumes that a second valid database called "AVISdata2" exists)

bool DatabaseClassesTest::DBServiceTest4(Reporter& reporter)
{
	bool	rc = false;

	std::string	dbName("AVISdata");
	std::string	dbName2("AVISdata2");

	if (DBService::OpenService(dbName) && DBService::ServiceValid())
	{
		if (DBService::CloseService())
		{
			if (DBService::OpenService(dbName2) && DBService::ServiceValid())
			{
				if (!DBService::OpenService(dbName))
				{
					rc = true;
				}
				else
					reporter.Failure(std::string("DBService: Claimed another database was opened when it was not"));
			}
			else
				reporter.Failure(std::string("DBService: Unable to open AVISdata2 database"));
		}
		else
			reporter.Failure(std::string("DBService: Unable to close AVISdata database"));
	}
	else
		reporter.Failure(std::string("DBService: Unable to open AVISdata database"));


	if (DBService::ServiceValid())
	{
		DBService::CloseService();
		if (DBService::ServiceValid())
			DBService::CloseService();
	}


	return rc;
}

//	Make sure DatabaseName returns the correct name
//

bool DatabaseClassesTest::DBServiceTest5(Reporter& reporter)
{
	bool	rc = false;

	std::string	dbName("AVISdata");
	DBService	db(dbName);

	if (db.Valid())
	{
		std::string	copyOfName;
		if (DBService::Name(copyOfName))
		{
			if (copyOfName == dbName)
			{
				rc = true;
			}
			else
				reporter.Failure(std::string("DBService: database name returned is not the same as the one given"));
		}
		else
			reporter.Failure(std::string("DBService: Unable to get the name of the database"));
	}
	else
		reporter.Failure(std::string("DBService: Unable to open AVISdata database"));

	return rc;
}


//
//	DBService
//
//	Test the DBService class
//
//	Create a couple of DBService objects, release those objects, make sure the
//		counter is zero and the database is closed.
//
//	Create a copy of an DBService object through a pass by value function call,
//		make sure the count does not get messed up.
//
//	Make sure the Open/Close database calls handle the count properly
//
//	Make sure DBService only works with one database name
//
//	Make sure DatabaseName returns the correct name
//
//	Make sure Valid/ServiceValid work properly (this gets tested by all the other tests)

bool DatabaseClassesTest::DBServiceTest(Reporter& reporter)
{
	bool	rc1, rc2, rc3, rc4, rc5;
	rc1 = rc2 = rc3 = rc4 = rc5 = false;


	rc1 = DBServiceTest1(reporter);
	rc2 = DBServiceTest2(reporter);
	rc3 = DBServiceTest3(reporter);
	rc4 = DBServiceTest4(reporter);
	rc5 = DBServiceTest5(reporter);


	return rc1 && rc2 && rc3 && rc4 && rc5;
}


//
//	DBSessionTest
//
//	Test the DBSession class
//
//	Connect to a valid database
//	(ensure that Valid is properly set)
//
//	Connect to an invalid database (should fail)
//	(ensure that Valid is properly set, GetErrorMessage works)
//
//

#pragma warning ( disable : 4101 )

bool DatabaseClassesTest::DBSessionTest(Reporter& reporter)
{
	bool	rc1 = false;
	bool	rc2 = false;

	DBService	*dbGood	= new DBService(std::string("AVISdata"));

	if (dbGood->Valid())
	{
		DBSession	connection;

		if (connection.Valid())
		{
			if (connection.ConnectionHandle())
				rc1 = true;
			else
				reporter.Failure(std::string("DBSession: Connection claims to be valid, but has null conneciton handle"));
		}
		else
			reporter.Failure(std::string("DBSession: Unable to create a valid connection"));
	}
	else
		reporter.Failure(std::string("DBSession: Unable to open DBService database"));

	delete dbGood;
	

	DBService	*dbBad = new DBService(std::string("BogusDatabase"));

	if (dbBad->Valid())		// even though database is bogus this is okay, ODBC is running
	{
		try
		{
			DBSession	badConnection;
			if (!badConnection.Valid())
			{
				rc2	= true;
			}
			else
				reporter.Failure(std::string("DBSession: badConnection should be invalid, but Valid is returning true"));
		}
		catch (AVISDBException& e)
		{
			rc2 = true;
		}
	}
	else
		reporter.Failure(std::string("DBSession: Unable to connection to database service"));


	delete dbBad;

	return rc1 && rc2;
}
#pragma warning ( default : 4101 )

//
//	SQLStmtTest
//
//	Test the SQLStmt class
//
//	Create using an invalid DBSession object (should fail)
//
//	Use ExecDirect to create the following table:
//	SQLStmtTest
//		short	tShort
//		ushort	tUShort
//		int		tInt
//		uint	tUInt
//		char*	tString
//		char	tByte
//		TIMESTAMP	tTimeStamp
//
//	Use Prepare to create the following insert statement
//	INSERT INTO SQLStmtTest VALUES (?, ?, ?, ?, ?, ?, ?, ? )
//
//	Bind each parameter using BindParameter
//
//	Add 10 rows of data to the table using the Execute method
//
//	Free the cursor using FreeCusor method
//
//	Use Prepare to create the following select statement
//	SELECT * FROM SQLStmtTest
//
//	Bind each of the seven result columns using BindColumn
//
//	Use the Fecth method to retrieve each row.
//
//	Free the cursor using FreeCusor method
//
//	Try to ExecDirect a bogus SQL statement, ensure that error ensues and that
//		GetErrorMessage returns a non-null error message
//

static const int	STRLEN	= 20;
static const int	TESTLEN	= 10;
class SQLStmtTestTable
{
public:
	short		tShort;
	ushort		tUShort;
	int			tInt;
	uint		tUInt;
	char		tString[STRLEN];
	char		tByte;
	DateTime	tTimeStamp;

	SQLStmtTestTable(short s, ushort us, int i, uint ui, char* str, char b, DateTime& t) :
			tShort(s), tUShort(us), tInt(i), tUInt(ui), tByte(b), tTimeStamp(t)
			{
				strncpy(tString, str, STRLEN);
			};
};

static SQLStmtTestTable	testData[TESTLEN] = {
	SQLStmtTestTable(1, 1, 1, 1, "Test Row One",   (char) 1, DateTime("1998-09-18-09.00.01")),
	SQLStmtTestTable(2, 2, 2, 2, "Test Row Two",   (char) 2, DateTime("1998-09-18-09.00.02")),
	SQLStmtTestTable(3, 3, 3, 3, "Test Row Three", (char) 3, DateTime("1998-09-18-09.00.03")),
	SQLStmtTestTable(4, 4, 4, 4, "Test Row Four",  (char) 4, DateTime("1998-09-18-09.00.04")),
	SQLStmtTestTable(5, 5, 5, 5, "Test Row Five",  (char) 5, DateTime("1998-09-18-09.00.05")),
	SQLStmtTestTable(6, 6, 6, 6, "Test Row Six",   (char) 6, DateTime("1998-09-18-09.00.06")),
	SQLStmtTestTable(7, 7, 7, 7, "Test Row Seven", (char) 7, DateTime("1998-09-18-09.00.07")),
	SQLStmtTestTable(8, 8, 8, 8, "Test Row Eight", (char) 8, DateTime("1998-09-18-09.00.08")),
	SQLStmtTestTable(9, 9, 9, 9, "Test Row Nine",  (char) 9, DateTime("1998-09-18-09.00.09")),
	SQLStmtTestTable(10,10,10,10,"Test Row Ten",   (char) 10, DateTime("1998-09-18-09.00.10")),

};

static void ReportError(Reporter& reporter, SQLStmt& sql, std::string& title)
{
	reporter.Failure(title);

	std::string message;
	sql.GetErrorMessage(message);
	reporter.Failure(message);
}


bool DatabaseClassesTest::SQLStmtTest(Reporter& reporter)
{
	bool	rcPrep	= false;
	bool	rcInsert= true;
	bool	rcFetch	= true;

	std::string	createTable("CREATE TABLE SQLStmtTest ( ");
	createTable	+= "tShort		SMALLINT NOT NULL PRIMARY KEY, ";
	createTable += "tUShort		SMALLINT, ";	// db2 does not understand signed/unsigned
	createTable += "tInt		INTEGER, ";
	createTable += "tUInt		INTEGER, ";
	createTable += "tString		VARCHAR(19), ";
//	createTable += "tString		CHAR(19), ";
	createTable += "tByte		CHAR FOR BIT DATA, ";
	createTable += "tTimeStamp	TIMESTAMP ";
	createTable += ")";

	DBService	db(std::string("AVISdata"));
	DBSession	connection;

	if (db.Valid() && connection.Valid())
	{
		SQLStmt	sqlStmt(connection, "SQLStmtTest", exceptType);

		if (sqlStmt.Valid())
		{
			if (sqlStmt.ExecDirect(createTable, true, false))
			{
				if (sqlStmt.Prepare(string("INSERT INTO SQLStmtTest VALUES (?, ?, ?, ?, ?, ?, ?)"), true, false))
				{
					short	tShort;
					ushort	tUShort;
					int		tInt;
					uint	tUInt;
					char	tString[STRLEN];
					char	tByte;
					DateTime	timeStamp;

					sqlStmt.BindParameter(1, SQLStmt::Input, &tShort);
					sqlStmt.BindParameter(2, SQLStmt::Input, &tUShort);
					sqlStmt.BindParameter(3, SQLStmt::Input, &tInt);
					sqlStmt.BindParameter(4, SQLStmt::Input, &tUInt);
					sqlStmt.BindParameter(5, SQLStmt::Input, tString, STRLEN);
					sqlStmt.BindParameter(6, SQLStmt::Input, &tByte);
					sqlStmt.BindParameter(7, SQLStmt::Input, timeStamp);

					rcPrep	= true;

					for (int i = 0; i < TESTLEN; i++)
					{
						tShort		= testData[i].tShort;
						tUShort		= testData[i].tUShort;
						tInt		= testData[i].tInt;
						tUInt		= testData[i].tUInt;
						strncpy(tString, testData[i].tString, STRLEN);
						tByte		= testData[i].tByte;
						timeStamp	= testData[i].tTimeStamp;

						if (!sqlStmt.Execute(true, false))
						{
							ReportError(reporter, sqlStmt, string("SQLStmtTest: Unable to add records to SQLStmtTest table"));
							rcInsert	= false;
						}
					}

					sqlStmt.FreeCursor();

					if (rcInsert)
					{
						if (sqlStmt.ExecDirect(string("SELECT * FROM SQLStmtTest"), true, false))
						{
							SQLINTEGER	tShortRet, tUShortRet, tIntRet, tUIntRet;
							SQLINTEGER	tStringRet, tByteRet, tTimeStampRet;

							sqlStmt.BindColumn(1, &tShort,	&tShortRet);
							sqlStmt.BindColumn(2, &tUShort, &tUShortRet);
							sqlStmt.BindColumn(3, &tInt,	&tIntRet);
							sqlStmt.BindColumn(4, &tUInt,	&tUIntRet);
							sqlStmt.BindColumn(5, tString,	STRLEN, &tStringRet);
							sqlStmt.BindColumn(6, &tByte,	&tByteRet);
							sqlStmt.BindColumn(7, timeStamp, &tTimeStampRet);

							for (int j = 0; j < TESTLEN; j++)
							{
								if (sqlStmt.Fetch(true, false))
								{
									tString[tStringRet] = '\0';

									if (testData[j].tShort	== tShort	&&
										testData[j].tUShort	== tUShort	&&
										testData[j].tInt	== tInt		&&
										testData[j].tUInt	== tUInt	&&
										!strncmp(testData[j].tString, tString, STRLEN) &&
										testData[j].tByte	== tByte	&&
										testData[j].tTimeStamp	== timeStamp )
									{
										;
									}
									else
									{
										char	buffer[128];
										std::string	message("SQLStmtTest: Value(s) fetched from database not the same as the values stored!\n\t");
										sprintf(buffer, "%d: (%d, %d), (%u, %u), (%d, %d), (%u, %u)", j,
															testData[j].tShort, tShort,
															testData[j].tUShort, tUShort,
															testData[j].tInt, tInt,
															testData[j].tUInt, tUInt);
										message	+= buffer;
										message	+= "\n\t(";
										message	+= testData[j].tString;
										message	+= ",";
										message	+= tString;
										message	+= ")\n\t";
										sprintf(buffer, "(%c, %c)\n\t(", testData[j].tByte, tByte);
										std::string	tmp;
										testData[j].tTimeStamp.AsHttpString(tmp);
										message	+= tmp;
										message	+= ",";
										timeStamp.AsHttpString(tmp);
										message	+= tmp;
										message	+= ")\n";

										ReportError(reporter, sqlStmt, message);
										rcFetch = false;
									}
								}
								else
								{
									ReportError(reporter, sqlStmt, std::string("SQLStmtTest: Problem fetching rows from SQLStmtTest table"));
									rcFetch = false;
								}
							}
						}
						else
							ReportError(reporter, sqlStmt, std::string("SQLStmtTest: Unable to select all columns from the SQLStmtTest table"));
					}
				}
				else
					ReportError(reporter, sqlStmt, std::string("SQLStmtTest: Unable to create INSERT INTO SQLStmtTest statement"));
			}
			else
				ReportError(reporter, sqlStmt, std::string("SQLStmtTest: Unable to create SQLStmtTest table"));
		}
		else
			reporter.Failure(std::string("SQLStmtTest: Unable to create a valid SQLStmt object"));
	}
	else
		reporter.Failure(std::string("SQLStmtTest: Unable to open AVISdata or get a valid connection"));

	return rcPrep && rcInsert && rcFetch;
}


bool DatabaseClassesTest::TestDatabaseClasses(Reporter& reporter)
{
	bool	rc, rc1, rc2, rc3;

	rc1 = DBServiceTest(reporter);
	rc2 = DBSessionTest(reporter);
	rc3 = SQLStmtTest(reporter);

	rc = rc1 && rc2 && rc3;

	return rc;
}
