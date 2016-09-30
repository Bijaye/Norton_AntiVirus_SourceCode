//
//	DatabaseTests Class
//
//	This class tests the database to ensure that the database will
//	behave properly when used.  This includes ensureing that triggers
//	get fired, cascading deletes delete properly, and that database
//	consistancy rules are followed.
//
//	This class can also be thought to test the low level C++ interface
//	to the database.  (DBService, DBSession, and SQLStmt).
//
//	Another test class will handle the testing of the higher level
//	C++ classes that provide the main interface to the database.
//
//	WARNING:	THESE TESTS ARE DESTRUCTIVE!!!  DO NOT RUN THESE TESTS ON
//				ANY DATABASE WHOSE'S DATA YOU CARE ABOUT.  These tests are
//				ment to be run on a newly initialized database in the
//				development enviroment to ensure that schema changes did not
//				change the behavior of the database in any undesirable way.
//
//	I've tried to organize each table test into the following order.  Tests
//	that do not apply to a particular table (i.e. no cascading deletions) are
//	not run (dah!).  All categories are run once for each field unless otherwise
//	noted.
//		Insert all valid values (generally this is only one test)
//		Insert with each field being null
//		Insert with duplicte values
//		Insert with invalid values
//		Update with valid values
//		Update with invalid values (only if field is updatable to begin with)
//		Delete a record, try to create dangling references
//		Test cascading deletes
//		Test triggers
//
//	Since most of the tests are very simple I've set up a test method that
//	accepts an array of structures that are used to do database accepts/rejects
//	tests.  More complex tests that require verification of additional actions
//	are done in the body of the test method for each table.
//
//
//	Author:	Andy Klapper
//
//	$Log: $
//

#include "stdafx.h"

#include "DatabaseTests.h"

#include "SQLStmt.h"


using namespace std;

static AVISDBException::TypeOfException	exceptType = AVISDBException::etDatabaseTests;


class SimpleTests
{
public:
	const char*	query;
	const char* description;
	const bool	shouldPass;

	SimpleTests(const char* q, const char* d, bool sp) :
					query(q), description(d), shouldPass(sp)
					{};
};


static bool RunSimpleTests(SQLStmt& sql, Reporter& reporter,
						   SimpleTests* simpleTests)
{
	bool	rc = true;
	bool	sqlRc;
	string	message;

	for (int i = 0; strcmp("", simpleTests[i].query); i++)
	{
		if (i > 200)	// check for non-terminated simpleTests array!
			throw AVISDBException(exceptType, "RunSimpleTests, simpeTests array non-terminated");
	
		sqlRc = SQLStmt::Okay == sql.ExecDirect(string(simpleTests[i].query), true, false);
		if (simpleTests[i].shouldPass != sqlRc)
		{
			message = "Failed Test: ";
			message += simpleTests[i].description;
			reporter.Failure(message);
			sql.GetErrorMessage(message);
			reporter.Failure(message);
			message = "SQL Text: ";
			message += simpleTests[i].query;
			rc = false;
		}
		else
		{
			message = "Passed Test: ";
			message += simpleTests[i].description;
			reporter.Success(message);
		}
	}

	return rc;
}

//
//	AnalysisResults
//
//	Test the AnalysisResults table.  Ensure that following things
//	work correctly (this includes making sure that incorrect things
//	are properly bounced by the database.)
//
//	Insert a record with all fields filled and valid
//	Insert a record with a NULL SignatureSeqNum
//	Insert a record with a NULL AnalysisStateID (should fail)
//	Insert a record with a NULL CheckSum value (should fail)
//	Insert a record with a duplicate CheckSum value (should fail)
//	Insert a record with a duplicate SignatureSeqNum
//	Insert a record with a duplicate AnalysisStateID
//	Insert a record with an invalid AnalysisStateID (should fail)
//	Insert a record with an invalid SignatureSeqNum (should fail)
//	Update CheckSum field (should fail)
//	Update SignatureSeqNum field
//	Update SignatureSeqNum field to an invalid SignatureSeqNum (should fail)
//	Update SignatureSeqNum field to a duplicate SignatureSeqNum
//	Update AnalysisStateID field
//	Update AnalysisStateID to an invalid AnalysisStateID (should fail)
//	Update AnalysisStateID to a duplicate AnalysisStateID
//	Delete a record
//	

//
//	NOTE:	These tests assume that there exists a SignatureSeqNum of 1 and an
//			AnalysisStateID of 1.  Both should be true as the database
//			creation script creates these items by default.
static	SimpleTests	SimpleAnalysisResultsTests[] =
{


	SimpleTests("INSERT INTO Signatures VALUES ( 3, NULL, '1998-8-26-19.11.03.01', '1998-8-26-19.11.03.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 4, NULL, '1998-8-26-19.11.04.01', '1998-8-26-19.11.04.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 5, NULL, '1998-8-26-19.11.05.01', '1998-8-26-19.11.05.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 6, NULL, '1998-8-26-19.11.06.01', '1998-8-26-19.11.06.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 7, NULL, '1998-8-26-19.11.07.01', '1998-8-26-19.11.07.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 8, NULL, '1998-8-26-19.11.08.01', '1998-8-26-19.11.08.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 9, NULL, '1998-8-26-19.11.09.01', '1998-8-26-19.11.09.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 10, NULL, '1998-8-26-19.11.10.01', '1998-8-26-19.11.10.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 11, NULL, '1998-8-26-19.11.11.01', '1998-8-26-19.11.11.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 12, NULL, '1998-8-26-19.11.12.01', '1998-8-26-19.11.12.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 13, NULL, '1998-8-26-19.11.13.01', '1998-8-26-19.11.13.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 14, NULL, '1998-8-26-19.11.14.01', '1998-8-26-19.11.14.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 15, NULL, '1998-8-26-19.11.15.01', '1998-8-26-19.11.15.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 16, NULL, '1998-8-26-19.11.16.01', '1998-8-26-19.11.16.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 17, NULL, '1998-8-26-19.11.17.01', '1998-8-26-19.11.17.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 18, NULL, '1998-8-26-19.11.18.01', '1998-8-26-19.11.18.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 19, NULL, '1998-8-26-19.11.19.01', '1998-8-26-19.11.19.02', '')",
				"Preparing data for AnalysisResults tests", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 20, NULL, '1998-8-26-19.11.20.01', '1998-8-26-19.11.20.02', '')",
				"Preparing data for AnalysisResults tests", true),


	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest1', 1, 1 )",
				"AnalysisResults, insert a record with all fields filled and valid", true),
	SimpleTests("INSERT INTO AnalysisResults (CheckSum, AnalysisStateID) VALUES ('AnalysisResultsTest2', 12)",
				"AnalysisResults, insert a record with NULL SignatureSeqNum", true),
	SimpleTests("INSERT INTO AnalysisResults (CheckSum, SignatureSeqNum) VALUES ('AnalysisResultsTest3', 32)",
				"AnalysisResults, insert a record with NULL AnalysisStateID (should fail)", false),
	SimpleTests("INSERT INTO AnalysisResults (AnalysisStateID, SignatureSeqNum) VALUES( 14, 34 )",
				"AnalysisResults, insert a record with NULL CheckSum (should fail)", false),
	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest1', 5, 15)",
				"AnalysisResults, insert a record with a duplicate CheckSum value (should fail)", false),
	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest6', 1, 16)",
				"AnalysisResults, insert a record with a duplicate SignatureSeqNum", true),
	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest7', 7, 1)",
				"AnalysisResults, insert a record with a duplicate AnalysisStateID", true),
	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest8', 5000, 1800)",
				"AnalysisResults, insert a record with an invalid AnalysisStateID (should fail)", false),
	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest9', 9, 5000)",
				"AnalysisResults, insert a record with an invalid SignatureSeqNum (should fail)", false),

	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest10', 10, 10)",
				"AnalysisResults, prep for test 10", true),
	SimpleTests("UPDATE AnalysisResults SET CheckSum = 'AnalysisResultsTest10b' WHERE CheckSum = 'AnalysisResultsTest10'",
				"AnalysisResults, update CheckSum field (should fail)", false),

	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest11', 11, 11)",
				"AnalysisResults, prep for test 11", true),
	SimpleTests("UPDATE AnalysisResults SET SignatureSeqNum = 20 WHERE CheckSum = 'AnalysisResultsTest11'",
				"AnalysisResults, update SignatureSeqNum field", true),

	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest12', 12, 12)",
				"AnalysisResults, prep for test 12", true),
	SimpleTests("UPDATE AnalysisResults SET SingatureID = 5000 WHERE CheckSum = 'AnalysisResultsTest12'",
				"AnalysisResults, update SignatureSeqNum field to an invalid SignatureSeqNum (should fail)", false),

	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest13', 13, 13)",
				"AnalysisResults, prep for test 13", true),
	SimpleTests("UPDATE AnalysisResults SET SignatureSeqNum = 1 WHERE CheckSum = 'AnalysisResultsTest13'",
				"AnalysisResults, update SignatureSeqNum field to a duplicate SignatureSeqNum", true),

	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest14', 14, 14)",
				"AnalysisResults, prep for test 14", true),
	SimpleTests("UPDATE AnalysisResults SET AnalysisStateID = 30 WHERE CheckSum = 'AnalysisResultsTest14'",
				"AnalysisResults, update AnalysisStateID", true),

	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest15', 15, 15)",
				"AnalysisResults, prep for test 15", true),
	SimpleTests("UPDATE AnalysisResults SET AnalysisStateID = 5000 WHERE CheckSum = 'AnalysisResultsTest15'",
				"AnalysisResults, update AnalysisStateID to an invalid AnalysisStateID (should fail)", false),

	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest16', 16, 16)",
				"AnalysisResults, prep for test 16", true),
	SimpleTests("UPDATE AnalysisResults SET AnalysisStateID = 1 WHERE CheckSum = 'AnalysisResultsTest16'",
				"AnalysisResults, update AnalysisStateID to a duplicate AnalysisStateID", true),

	SimpleTests("INSERT INTO AnalysisResults VALUES( 'AnalysisResultsTest17', 17, 17)",
				"AnalysisResults, prep for test 17", true),
	SimpleTests("DELETE FROM AnalysisResults WHERE CheckSum = 'AnalysisResultsTest17'",
				"AnalysisResults, delete a record", true),

	//
	//	Clean up test records

	SimpleTests("DELETE FROM AnalysisResults WHERE CheckSum LIKE 'AnalysisResultsTest%'",
				"AnalysisResults, Delete test records (AnalysisResults)", true),
	SimpleTests("UPDATE Globals SET NewestSeqNum = 2 WHERE AlwaysZero = 0",
				"AnalysisResults, Delete test records (reset NewestSeqNum)", true),
	SimpleTests("UPDATE Globals SET NewestBlssdSeqNum = 1 WHERE AlwaysZero = 0",
				"AnalysisResults, Delete test records (reset NewestBlssdSeqNum)", true),
	SimpleTests("DELETE FROM Signatures WHERE SignatureSeqNum > 2",
				"AnalysisResults, Delete test records (Signatures)", true),

	SimpleTests("", "", true)		// array terminator
};

bool DatabaseTests::AnalysisResults(DBSession& connection, Reporter& reporter)
{
	SQLStmt	sql(connection, "AnalysisResults, simpleTests", exceptType);

	return RunSimpleTests(sql, reporter, SimpleAnalysisResultsTests);
}


//
//	Submittors
//
//	Test the Submittors table.  Ensure that following things
//	work correctly (this includes making sure that incorrect things
//	are properly bounced by the database.)
//
//	Insert a record with all fields filled and valid
//	Insert a record with a duplicate SubmittorID (should fail)

//	Exceeding usage tests removed because policy dll handles usage monitoring
//	Update SubmittorID (should fail)
//	Delete a record not referenced by an AnalysisRequest record
//	Delete a record referenced by an AnalysisRequest record (should fail)
//	Update UsageToday by one, ensure that all other counts also go up by one
//	Update UsageToday by five, ensure that all other counts also go up by five
//
//

static	SimpleTests	SimpleSubmittorsTests[] =
{
	SimpleTests("INSERT INTO Submittors VALUES ( 'SubmittorsTest1', 1, 1, 1, 1)",
				"Submittors, insert a record with all fields filled and valid", true),
	SimpleTests("INSERT INTO Submittors VALUES ( 'SubmittorsTest1', 2, 2, 2, 2)",
				"Submittors, insert a record with a duplicate SubmittorID (should fail)", false),

//	SimpleTests("INSERT INTO Platforms VALUES ( 'SubmittorsTest1', 1, 1, 1, 1)",
//				"Submittors, insert a Platform record for use with later tests", true),

	SimpleTests("INSERT INTO Submittors VALUES ( 'SubmittorsTest7', 1, 1, 1, 1)",
				"Submittors, prep for test 7", true),
	SimpleTests("UPDATE Submittors SET SubmittorID = 'SubmittorsTest7b' WHERE SubmittorID = 'SubmittorsTest7'",
				"Submittors, update SubmittorID (should fail)", false),

	SimpleTests("INSERT INTO Submittors VALUES ( 'SubmittorsTest8', 1, 1, 1, 1)",
				"Submittors, prep for test 8", true),
	SimpleTests("DELETE FROM Submittors WHERE SubmittorID = 'SubmittorsTest8'",
				"Submittors, delete a record not referenced by an AnalysisRequest record", true),

	SimpleTests("INSERT INTO Submittors VALUES ( 'SubmittorsTest9', 1, 1, 1, 1)",
				"Submittors, prep for test 9", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID, RemoteCookie) VALUES(9, 'SubmittorsTest9', 1, 'SubmittorsTest9', 0, 9)",
				"Submittors, prep for test 9", true),
	SimpleTests("DELETE FROM Submittors WHERE SubmittorID = 'SubmittorsTest9'",
				"Submittors, delete a record referenced by an AnalysisRequest record (should fail)", false),

	//
	//	Clean up from Submittors Simple tests
	SimpleTests("DELETE FROM AnalysisRequest WHERE RemoteCookie > 1",
				"Clean up after Submittors simple tests, AnalysisRequest clean up", true),
	SimpleTests("DELETE FROM Submittors WHERE SubmittorID LIKE 'SubmittorsTest%'",
				"Clean up after Submittors simple tests, Submittors clean up", true),

	SimpleTests("", "", true)		// array terminator
};


bool DatabaseTests::Submittors(DBSession& connection, Reporter& reporter)
{
	SQLStmt	sql(connection, "Submittors, simple tests", exceptType);

	return RunSimpleTests(sql, reporter, SimpleSubmittorsTests);
}


//
//	Signatures
//
//	Test the Signatures table.  Ensure that following things
//	work correctly (this includes making sure that incorrect things
//	are properly bounced by the database.)
//
//	Insert a record with all fields filled and valid
//	Insert a record with SignatureSeqNum set to null (should fail)
//	Insert a record with Blessed set to null
//	Insert a record with Produced set to null (should fail)
//	Insert a record with Published set to null (should fail)
//	Insert a record with duplicate SignatureSeqNum (should fail)
//	Insert a record with duplicate Blessed
//	Insert a record with duplicate Produced
//	Insert a record with duplicate Published
//	Update Produced
//	Update Published
//	Delete a record not referenced by the Globals or AnalysisResults table
//	Delete a record referenced by the Globals::NewestSeqNum (should fail)
//	Delete a record referenced by the Globals::NewestBlssdSeqNum (should fail)
//	Delete a record referenced by the AnalysisResults table
//
//	Insert a record with all fields filled and valid with SignatureSeqNum >
//			Globals::NewestSeqNum, ensure that Globals::NewestSeqNum is
//			updated.
//	Insert a record with SignatureSeqNum less than Globals::NewestSeqNum,
//			ensure that Globals::NewestSeqNum is not updated.
//	Insert a record with blessed set and SignatureSeqNum >
//			Globals::NewestBlssdSeqNum and ensure that
//			Globals::NewestBlssdSeqNum gets updated.
//	Insert a record with blessed set and SignatureSeqNum <
//			Globals::NewestBlssdSeqNum and ensure that
//			Globals::NewestBlssdSeqNum does not get updated.
//	Update Blessed to not null and SignatureSeqNum > Globals::NewestBlssdSeqNum
//			ensure that Globals::NewestBlssdSeqNum gets updated.
//	Update Blessed to not null and SignatureSeqNum < Globals::NewestBlssdSeqNum
//			ensure that Globals::NewestBlssdSeqNum does not get updated.
//	Update Blessed to null and SignatureSeqNum == Globals::NewestBlssdSeqNum,
//			ensure that Globals::NewestBlssdSeqNum gets updated.
//	Update Blessed to null and SignatureSeqNum != Globals::NewestBlssdSeqNum,
//			ensure that Globals::NewestBlssdSeqNum does not get updated.
//


static	SimpleTests	SimpleSignaturesTests[] =
{
	SimpleTests("INSERT INTO Signatures VALUES ( 101, '1998-8-26-19.11.01.01', '1998-8-26-19.11.01.02', '1998-8-26-19.11.01.03', '' )",
				"Signatures, insert a record with all fields filled and valid", true),

	SimpleTests("INSERT INTO Signatures VALUES ( NULL, '1998-8-26-19.11.03.01', '1998-8-26-19.11.03.02', '1998-8-26-19.11.03.03', '' )",
				"Signatures, insert a record with SignatureSeqNum set to null (should fail)", false),
	SimpleTests("INSERT INTO Signatures VALUES ( 105, NULL, '1998-8-26-19.11.04.02', '1998-8-26-19.11.04.03', '' )",
				"Signatures, insert a record with Blessed set to null", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 105, '1998-8-26-19.11.05.01', NULL, '1998-8-26-19.11.05.03', '' )",
				"Signatures, insert a record with Produced set to null (should fail)", false),
	SimpleTests("INSERT INTO Signatures VALUES ( 106, '1998-8-26-19.11.06.01', '1998-8-26-19.11.06.02', NULL, '' )",
				"Signatures, insert a record with Published set to null (should fail)", false),


	SimpleTests("INSERT INTO Signatures VALUES ( 109, '1998-8-26-19.11.09.01', '1998-8-26-19.11.09.02', '1998-8-26-19.11.09.03', '' )",
				"Signatures, prep for test 9", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 109, '1998-9-26-19.11.09.01', '1998-9-26-19.11.09.02', '1998-9-26-19.11.09.03', '' )",
				"Signatures, insert a record with duplicate SignatureSeqNum (should fail)", false),

	SimpleTests("INSERT INTO Signatures VALUES ( 110, '1998-8-26-19.11.10.01', '1998-8-26-19.11.10.02', '1998-8-26-19.11.10.03', '' )",
				"Signatures, prep for test 10", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 210, '1998-8-26-19.11.10.01', '1998-9-26-19.11.10.02', '1998-9-26-19.11.10.03', '' )",
				"Signatures, insert a record with duplicate Blessed", true),

	SimpleTests("INSERT INTO Signatures VALUES ( 111, '1998-8-26-19.11.11.01', '1998-8-26-19.11.11.02', '1998-8-26-19.11.11.03', '' )",
				"Signatures, prep for test 11", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 211, '1998-9-26-19.11.11.01', '1998-8-26-19.11.11.02', '1998-9-26-19.11.11.03', '' )",
				"Signatures, insert a record with duplicate Produced", true),

	SimpleTests("INSERT INTO Signatures VALUES ( 112, '1998-8-26-19.11.12.01', '1998-8-26-19.11.12.02', '1998-8-26-19.11.12.03', '' )",
				"Signatures, prep for test 12", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 212, '1998-9-26-19.11.12.01', '1998-9-26-19.11.12.02', '1998-8-26-19.11.12.03', '' )",
				"Signatures, insert a record with duplicate Published", true),

	SimpleTests("INSERT INTO Signatures VALUES ( 113, '1998-8-26-19.11.13.01', '1998-8-26-19.11.13.02', '1998-8-26-19.11.13.03', '' )",
				"Signatures, prep for test 13", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 213, '1998-9-26-19.11.13.01', '1998-9-26-19.11.13.02', '1998-9-26-19.11.13.03', '' )",
				"Signatures, insert a record with duplicate Published", true),

	SimpleTests("INSERT INTO Signatures VALUES ( 114, '1998-8-26-19.11.14.01', '1998-8-26-19.11.14.02', '1998-8-26-19.11.14.03', '' )",
				"Signatures, prep for test 14", true),
	SimpleTests("UPDATE Signatures SET Produced = '1998-9-16-19.11.14.02' WHERE SignatureSeqNum = 114",
				"Signatures, Update Produced", true),

	SimpleTests("INSERT INTO Signatures VALUES ( 115, '1998-8-26-19.11.15.01', '1998-8-26-19.11.15.02', '1998-8-26-19.11.15.03', '' )",
				"Signatures, prep for test 15", true),
	SimpleTests("UPDATE Signatures SET Published = '1998-9-16-19.11.15.03' WHERE SignatureSeqNum = 115",
				"Signatures, Update Published", true),

	SimpleTests("INSERT INTO Signatures VALUES ( 116, '1998-8-26-19.11.16.01', '1998-8-26-19.11.16.02', '1998-8-26-19.11.16.03', '' )",
				"Signatures, prep for test 16", true),


	SimpleTests("INSERT INTO Signatures VALUES ( 117, '1998-8-26-19.11.17.01', '1998-8-26-19.11.17.02', '1998-8-26-19.11.17.03', '' )",
				"Signatures, prep for test 17", true),
	SimpleTests("INSERT INTO Signatures VALUES ( 217, '1998-10-26-19.11.17.01', '1998-8-26-19.11.17.02', '1998-8-26-19.11.17.03', '' )",
				"Signatures, prep for test 17", true),
	SimpleTests("DELETE FROM Signatures WHERE SignatureSeqNum = 117",
				"Signatures, Delete a record not referenced by the Globals or AnalysisResults table", true),

	SimpleTests("INSERT INTO Signatures VALUES ( 118, '1998-8-26-19.11.18.01', '1998-8-26-19.11.18.02', '1998-8-26-19.11.18.03', '' )",
				"Signatures, prep for test 18", true),
	SimpleTests("UPDATE Globals SET NewestSeqNum = 118 WHERE AlwaysZero = 0",
				"Signatures, prep for test 18", true),
	SimpleTests("DELETE FROM Signatures WHERE SignatureSeqNum = 118'",
				"Signatures, Delete a record referenced by the Globals::NewestSeqNum (should fail)", false),

	SimpleTests("INSERT INTO Signatures VALUES ( 119, '1998-8-26-19.11.19.01', '1998-8-26-19.11.19.02', '1998-8-26-19.11.19.03', '' )",
				"Signatures, prep for test 19", true),
	SimpleTests("UPDATE Globals SET NewestBlssdSeqNum = 119 WHERE AlwaysZero = 0",
				"Signatures, prep for test 18", true),
	SimpleTests("DELETE FROM Signatures WHERE SignatureSeqNum = 119",
				"Signatures, Delete a record referenced by the Globals::NewestBlssdSeqNum (should fail)", false),

	SimpleTests("INSERT INTO Signatures VALUES ( 120, '1998-8-26-19.11.20.01', '1998-8-26-19.11.20.02', '1998-8-26-19.11.20.03', '' )",
				"Signatures, prep for test 20", true),
	SimpleTests("INSERT INTO AnalysisResults VALUES ('SignatureTest20', 120, 1)",
				"Signatures, prep for test 20", true),
	SimpleTests("DELETE FROM Signatures WHERE SignatureSeqNum = 120",
				"Signatures, Delete a record referenced by the AnalysisResults table (should fail)", false),

	SimpleTests("", "", true)	// Array terminator
};

static	SimpleTests	SignaturesTestsCleanUp[] =
{
	//
	//	Clean up after the simple tests
	SimpleTests("DELETE FROM AnalysisResults WHERE CheckSum LIKE 'SignatureTest%'",
				"Clean up from Signatures table simple tests", true),
	SimpleTests("UPDATE Globals SET NewestSeqNum = 2 WHERE AlwaysZero = 0",
				"Clean up from Signatures table simple tests", true),
	SimpleTests("UPDATE Globals SET NewestBlssdSeqNum = 1 WHERE AlwaysZero = 0",
				"Clean up from Signatures table simple tests", true),
	SimpleTests("DELETE FROM Signatures WHERE SignatureSeqNum > 100",
				"Clean up from Signatures table simple tests", true),


	SimpleTests("", "", true)	// Array terminator
};


class SignaturesException
{
public:
	string	msg;
	SQLStmt&	sql;

	SignaturesException(string message, SQLStmt& sqlStmt) :
					msg(message), sql(sqlStmt)
					{ };
};

static bool	GetNewestAndBlessed(DBSession& connection, Reporter& reporter,
								int& newestSig, int& blessedSig)
{
	SQLStmt	sql(connection, "GetNewestAndBlessed", exceptType);
	SQLINTEGER	lengths[2];

	if (sql.ExecDirect(string("SELECT NewestSeqNum, NewestBlssdSeqNum FROM Globals WHERE AlwaysZero = 0"), true, false) &&
		sql.BindColumn(1, &newestSig, &lengths[0])	&&
		sql.BindColumn(2, &blessedSig,&lengths[1])	&&
		sql.Fetch(true, false))
	{
		return true;
	}
	else
	{
		reporter.Failure(string("Failure: Unable to get Globals::NewestSeqNum & NewestBlssdSeqNum"));
		string	message;
		sql.GetErrorMessage(message);
		reporter.Failure(message);
	}

	return false;
}

static bool InsertSigReturnNewestAndBlessed(DBSession& connection, Reporter& reporter,
											int newSigNum, bool blessed,
											int& newestSig, int& blessedSig)
{
	SQLStmt		sql(connection, "InsertSigReturnNeewestAndBlessed", exceptType);
	char		buffer[256];

	sprintf(buffer, "INSERT INTO Signatures VALUES (%d, %s, '1998-8-26-19.11.20.2', '1998-8-26-19.11.20.3', '' )",
					newSigNum, blessed ? "'1998-8-26-19.11.20.1'": "NULL");

	if (sql.ExecDirect(string(buffer), true, false)			&&
		GetNewestAndBlessed(connection, reporter, newestSig, blessedSig))
	{
		return true;
	}
	else
	{
		reporter.Failure(string("Failure: Unable to insert into Signatures table"));
		string	message;
		sql.GetErrorMessage(message);
		reporter.Failure(message);
	}

	return false;
}


static bool UpdateSigReturnNewestAndBlessed(DBSession& connection, Reporter& reporter,
											int sigNum, bool blessed,
											int& newestSig, int& blessedSig)
{
	SQLStmt		sql(connection, "UpdateSigReturnNEwestAndBlessed", exceptType);
	char		buffer[120];

	sprintf(buffer, "UPDATE Signatures SET Blessed = %s WHERE SignatureSeqNum = %d",
					blessed ? "'1998-8-26-19.11.20.1'" : "NULL", sigNum);
	if (sql.ExecDirect(string(buffer), true, false))
	{
		return GetNewestAndBlessed(connection, reporter, newestSig, blessedSig);
	}
	else
	{
		reporter.Failure(string("Failure: Unable to change Blessed state of a Signature record"));
		string	message;
		sql.GetErrorMessage(message);
		reporter.Failure(message);
	}

	return false;
}


bool DatabaseTests::Signatures(DBSession& connection, Reporter& reporter)
{
	SQLStmt	sql(connection, "Signatures", exceptType);

	bool rc = RunSimpleTests(sql, reporter, SimpleSignaturesTests);

	int			newest;
	int			blessed;
	int			addSignature, addSignature2, addSignature3;
	string	message, message2;

	try
	{
		//
		//	Insert a record with all fields filled and valid with SignatureSeqNum >
		//	Globals::NewestSeqNum, ensure that Globals::NewestSeqNum is updated.

		bool	sqlRC = SQLStmt::Okay == sql.ExecDirect(string("UPDATE Globals SET NewestSeqNum = 2, NewestBlssdSeqNum = 1 WHERE AlwaysZero = 0"), true, false);

		if (sqlRC)
		{
			addSignature	= 501;
			if (InsertSigReturnNewestAndBlessed(connection, reporter,
												addSignature, false, newest, blessed))
			{
				if (newest != addSignature)
				{
					reporter.Failure(string("Failure: Globals::NewestSeqNum not updated on Signatures insertion when it should"));
					rc = false;
				}
				else if (blessed == addSignature)
				{
					reporter.Failure(string("Failure: Globals::NewestBlssdSeqNum was updated on Signatures insertion when it shouldn't"));
					rc = false;
				}
				else
				{
					reporter.Success(string("Passed Test: Inserting a record with SignatureSeqNum > Globals::NewestSeqNum (not blessed),"));
					reporter.Success(string("\tensure that Globals::NewestSeqNum is updated and Globals::NewestBlssd is not."));
				}
			}
		}
		else
		{
			string	errorMsg;

			reporter.Failure(string("Failure: Signatures advanced tests"));
			reporter.Failure(string("Unable to reset Globals::NewestSeqNum & NewestBlssdSeqNum prior to test"));
			sql.GetErrorMessage(errorMsg);
			reporter.Failure(errorMsg);
		}

		//
		//	Insert a record with SignatureSeqNum less than Globals::NewestSeqNum.
		//	ensure that Globals::NewestSeqNum is NOT updated.
		sqlRC = SQLStmt::Okay == sql.ExecDirect(string("UPDATE Globals SET NewestSeqNum = 501, NewestBlssdSeqNum = 1 WHERE AlwaysZero = 0"), true, false);

		if (sqlRC)
		{
			addSignature = 90;
			if (InsertSigReturnNewestAndBlessed(connection, reporter,
												addSignature, false, newest, blessed))
			{
				if (newest == addSignature)
				{
					reporter.Failure(string("Failure: Globals::NewestSeqNum updated on Signatures insertion when it shouldn't"));
					rc = false;
				}
				else if (blessed == addSignature)
				{
					reporter.Failure(string("Failure: Globals::NewestBlssdSeqNum was updated on Signatures insertion when it shouldn't"));
					rc = false;
				}
				else
				{
					reporter.Success(string("Passed Test: Inserting a record with SignatureSeqNum < Globals::NewestSeqNum (not blessed),"));
					reporter.Success(string("\tensure that neither Globals::NewestSeqNum or Globals::NewestBlssd is updated."));
				}
			}
		}
		else
		{
			string	errorMsg;

			reporter.Failure(string("Failure: Signatures advanced tests"));
			reporter.Failure(string("Unable to reset Globals::NewestSeqNum & NewestBlssdSeqNum prior to test"));
			sql.GetErrorMessage(errorMsg);
			reporter.Failure(errorMsg);
		}


		//
		//	Insert a record with blessed set and SignatureSeqNum >
		//	Globals::NewestBlssdSeqNum and ensure that
		//	Globals::NewestBlssdSeqNum gets updated.
		sqlRC = SQLStmt::Okay == sql.ExecDirect(string("UPDATE Globals SET NewestSeqNum = 101, NewestBlssdSeqNum = 1 WHERE AlwaysZero = 0"), true, false);

		if (sqlRC)
		{
			addSignature = 92;
			if (InsertSigReturnNewestAndBlessed(connection, reporter,
												addSignature, true, newest, blessed))
			{
				if (newest == addSignature)
				{
					reporter.Failure(string("Failure: Globals::NewestSeqNum updated on Signatures insertion when it shouldn't"));
					rc = false;
				}
				else if (blessed != addSignature)
				{
					reporter.Failure(string("Failure: Globals::NewestBlssdSeqNum not updated on Signatures insertion when it should"));
					rc = false;
				}
				else
				{
					reporter.Success(string("Passed Test: Inserting a blessed record with SignatureSeqNum < Globals::NewestSeqNum & > NewestBlssdSeqNum,"));
					reporter.Success(string("\tensure that Globals::NewestSeqNum does not get updated and Globals::NewestBlssd is updated."));
				}
			}
		}
		else
		{
			string	errorMsg;

			reporter.Failure(string("Failure: Signatures advanced tests"));
			reporter.Failure(string("Unable to reset Globals::NewestSeqNum & NewestBlssdSeqNum prior to test"));
			sql.GetErrorMessage(errorMsg);
			reporter.Failure(errorMsg);
		}


		//
		//	Insert a record with blessed set and SignatureSeqNum <
		//	Globals::NewestBlssdSeqNum and ensure that
		//	Globals::NewestBlssdSeqNum does NOT get updated.
		sqlRC = SQLStmt::Okay == sql.ExecDirect(string("UPDATE Globals SET NewestSeqNum = 101, NewestBlssdSeqNum = 92 WHERE AlwaysZero = 0"), true, false);

		if (sqlRC)
		{
			addSignature = 89;
			if (InsertSigReturnNewestAndBlessed(connection, reporter,
												addSignature, true, newest, blessed))
			{
				if (newest == addSignature)
				{
					reporter.Failure(string("Failure: Globals::NewestSeqNum was updated on Signatures insertion when it shouldn't"));
					rc = false;
				}
				else if (blessed == addSignature)
				{
					reporter.Failure(string("Failure: Globals::NewestBlssdSeqNum was updated on Signatures insertion when it shouldn't"));
					rc = false;
				}
				else
				{
					reporter.Success(string("Passed Test: Inserting a record with SignatureSeqNum < Globals::NewestSeqNum & NewestBlssdSeqNum(blessed),"));
					reporter.Success(string("\tensure that both Globals::NewestSeqNum and Globals::NewestBlssd are not updated."));
				}
			}
		}
		else
		{
			string	errorMsg;

			reporter.Failure(string("Failure: Signatures advanced tests"));
			reporter.Failure(string("Unable to reset Globals::NewestSeqNum & NewestBlssdSeqNum prior to test"));
			sql.GetErrorMessage(errorMsg);
			reporter.Failure(errorMsg);
		}

		//
		//	Update Blessed to not null and SignatureSeqNum > Globals::NewestBlssdSeqNum
		//	ensure that Globals::NewestBlssdSeqNum gets updated.

		addSignature = 200;
		if (InsertSigReturnNewestAndBlessed(connection, reporter,
											addSignature, false,
											newest, blessed)			&&
			newest == addSignature && blessed != addSignature			&&
			UpdateSigReturnNewestAndBlessed(connection, reporter, addSignature,
											true, newest, blessed)		)
		{
			if (blessed != addSignature)
			{
				reporter.Failure(string("Failure: Globals::NewestBlssdSeqNum wasn't updated on Signatures blessed update to true when it should"));
				rc = false;
			}
			else if (newest != addSignature)
			{
				reporter.Failure(string("Failure: Signatures advanced test #5, newest not set properly"));
			}
			else
			{
				reporter.Success(string("Passed Test: Updating a signature to Blessed = true updates Globals::NewestBlssdSigNum"));
			}
		}
		else
		{
			reporter.Failure(string("Failure: Updating a signature to Blessed = true did not update Globals::NewestBlssdSigNum"));
		}


		//
		//	Update Blessed to not null and SignatureSeqNum < Globals::NewestBlssdSeqNum
		//	ensure that Globals::NewestBlssdSeqNum does not get updated.

		addSignature = 190;
		if (InsertSigReturnNewestAndBlessed(connection, reporter,
											addSignature, false,
											newest, blessed)			&&
			newest != addSignature && blessed != addSignature			&&
			UpdateSigReturnNewestAndBlessed(connection, reporter, addSignature,
											true, newest, blessed)		)
		{
			if (blessed == addSignature)
			{
				reporter.Failure(string("Failure: Globals::NewestBlssdSeqNum was updated on Signatures blessed update to true when it shouldn't"));
				rc = false;
			}
			else
			{
				reporter.Success(string("Passed Test: Updating a signature to Blessed = true doesn't updates Globals::NewestBlssdSigNum when less than existing NewestBlssdSigNum"));
			}
		}
		else
		{
			reporter.Failure(string("Failure: Updating a signature to Blessed = true did not update Globals::NewestBlssdSigNum"));
		}



		//
		//	Update Blessed to null and SignatureSeqNum == Globals::NewestBlssdSeqNum,
		//	ensure that Globals::NewestBlssdSeqNum gets updated.

		addSignature = 600;
		addSignature2= 602;
		addSignature3= 603;
		if (InsertSigReturnNewestAndBlessed(connection, reporter,
											addSignature, true,
											newest, blessed)		&&
			newest == addSignature && blessed == addSignature		&&
			InsertSigReturnNewestAndBlessed(connection, reporter,
											addSignature2, false,
											newest, blessed)		&&
			newest == addSignature2 && blessed != addSignature2		&&
			InsertSigReturnNewestAndBlessed(connection, reporter,
											addSignature3, true,
											newest, blessed)		&&
			newest == addSignature3 && blessed == addSignature3		&&
			UpdateSigReturnNewestAndBlessed(connection, reporter,
											addSignature3, false,
											newest, blessed)		)
		{
			if (blessed != addSignature)
			{
				reporter.Failure(string("Failure: Update Blessed to null and ensure that correct prior signature is selected"));
				rc = false;
			}
			else
			{
				reporter.Success(string("Passed Test: Update Blessed to null and ensure that correct prior signature is selected"));
			}
		}
		else
		{
			reporter.Failure(string("Failure: Preparing for update Blessed to null and ensure that correct prior signature is selected test"));
		}


		//
		//	Update Blessed to null and SignatureSeqNum != Globals::NewestBlssdSeqNum,
		//	ensure that Globals::NewestBlssdSeqNum does not get updated.

		addSignature = 620;
		addSignature2= 610;
		if (InsertSigReturnNewestAndBlessed(connection, reporter,
											addSignature, true,
											newest, blessed)		&&
			newest == addSignature && blessed == addSignature		&&
			InsertSigReturnNewestAndBlessed(connection, reporter,
											addSignature2, true,
											newest, blessed)		&&
			newest != addSignature2 && blessed != addSignature2		&&
			UpdateSigReturnNewestAndBlessed(connection, reporter,
											addSignature2, false,
											newest, blessed)		)
		{
			if (blessed != addSignature)
			{
				reporter.Failure(string("Failure: Update Blessed to null and ensure that current (higher) NewestBlssdSeqNum remains"));
				rc = false;
			}
			else
			{
				reporter.Success(string("Passed Test: Update Blessed to null and ensure that current (higher) NewestBlssdSeqNum remains"));
			}
		}
		else
		{
			reporter.Failure(string("Failure: Preparing for update Blessed to null and ensure that current (higher) NewestBlssdSeqNum remains"));
		}

	}


	catch (SignaturesException&	sigExcp)
	{
		string	errorMsg;

		reporter.Failure(string("Failure: Signatures advanced tests"));
		reporter.Failure(sigExcp.msg);
		sigExcp.sql.GetErrorMessage(errorMsg);
		reporter.Failure(errorMsg);
	}


	RunSimpleTests(sql, reporter, SignaturesTestsCleanUp);


	return rc;
}


//
//	Globals
//
//	Test the Globals table.  Ensure that following things
//	work correctly (this includes making sure that incorrect things
//	are properly bounced by the database.)
//
//	Insert a record with all fields filled and valid (should fail)
//	Update NextLocalCookie field
//	Update TypeOfServer field
//	Update NewestSeqNum field
//	Update NewestBlssdSeqNum
//	Update DatabaseVersion
//	Update ThisServerID
//	Update NextLocalCookie field to null (should fail)
//	Update TypeOfServer field to null (should fail)
//	Update NewestSeqNum field to null (should fail)
//	Update NewestBlssdSeqNum to null (should fail)
//	Update DatabaseVersion to null (should fail)
//	Update ThisServerID to null (should fail)
//	Update ThisServerID to an invalid value (should fail)
//	Delete a record (should fail)
//

static	SimpleTests	SimpleGlobalsTests[] =
{
	SimpleTests("INSERT INTO Globals VALUES ( 0, 1, X'00', 1, 1, 1, 1 )",
				"Globals, insert a record with all fields filled and valid (should fail)", false),

	SimpleTests("UPDATE Globals SET NextLocalCookie = 1 WHERE AlwaysZero = 0",
				"Globals, update NextLocalCookie", true),
	SimpleTests("UPDATE Globals SET TypeOfServer = X'01' WHERE AlwaysZero = 0",
				"Globals, update TypeOfServer", true),
	SimpleTests("UPDATE Globals SET NewestSeqNum = 1 WHERE AlwaysZero = 0",
				"Globals, update NewestSeqNum", true),
	SimpleTests("UPDATE Globals SET NewestBlssdSeqNum = 1 WHERE AlwaysZero = 0",
				"Globals, update NewestBlssdSeqNum", true),
	SimpleTests("UPDATE Globals SET ThisServerID = 0 WHERE AlwaysZero = 0",
				"Globals, update ThisServerID", true),

	SimpleTests("UPDATE Globals SET NextLocalCookie = NULL WHERE AlwaysZero = 0",
				"Globals, update NextLocalCookie to null (should fail)", false),
	SimpleTests("UPDATE Globals SET TypeOfServer = NULL WHERE AlwaysZero = 0",
				"Globals, update TypeOfServer to null (should fail)", false),
	SimpleTests("UPDATE Globals SET NewestSeqNum = NULL WHERE AlwaysZero = 0",
				"Globals, update NewestSeqNum to null (should fail)", false),
	SimpleTests("UPDATE Globals SET NewestBlssdSeqNum = NULL WHERE AlwaysZero = 0",
				"Globals, update NewestBlssdSeqNum to null (should fail)", false),
	SimpleTests("UPDATE Globals SET ThisServerID = NULL WHERE AlwaysZero = 0",
				"Globals, update ThisServerID to null (should fail)", false),

	SimpleTests("DELETE FROM Globals WHERE AlwaysZero = 0",
				"Globals, delete a record (should fail)", false),


	SimpleTests("", "", true)	// Array terminator
};

bool DatabaseTests::Globals(DBSession& connection, Reporter& reporter)
{
	SQLStmt	sql(connection, "Globals", exceptType);

	return RunSimpleTests(sql, reporter, SimpleGlobalsTests);
}


//
//	Servers
//
//	Test the Servers table.  Ensure that following things
//	work correctly (this includes making sure that incorrect things
//	are properly bounced by the database.)
//
//	Insert a record with all fields filled and valid
//	Insert a record with ServerID set to null (should fail)
//	Insert a record with NickName set to null (should fail)
//	Insert a record with URL set to null (should fail)
//	Insert a record with a duplicate ServerID (should fail)
//	Insert a record with a duplicate NickName (should fail)
//	Insert a record with a duplicate URL
//	Update ServerID (should fail)
//	Update NickName
//	Update NickName to duplicate (should fail)
//	Update URL to duplicate
//	Delete a record (should fail)
//

static	SimpleTests	SimpleServersTests[] =
{
	SimpleTests("INSERT INTO Servers VALUES( 101, 'http://www.ServersTest1', 1)",
				"Servers, insert a record with all fields filled and valid", true),
	SimpleTests("INSERT INTO Servers VALUES( NULL, 'http://www.ServersTest2', 1)",
				"Servers, insert a record with ServerID set to null (should fail)", false),
	SimpleTests("INSERT INTO Servers VALUES( 104, NULL, 1)",
				"Servers, insert a record with URL set to null (should fail)", false),

	SimpleTests("INSERT INTO Servers VALUES( 105, 'http://www.ServersTest5', 1)",
				"Servers, prep for test 5", true),
	SimpleTests("INSERT INTO Servers VALUES( 105, 'http://www.ServersTest5b', 1)",
				"Servers, insert a record with a duplicate ServerID (should fail)", false),

	SimpleTests("INSERT INTO Servers VALUES( 117, 'http://www.ServersTest7', 1)",
				"Servers, prep for test 7", true),
	SimpleTests("INSERT INTO Servers VALUES( 107, 'http://www.ServersTest7', 1)",
				"Servers, insert a record with a duplicate URL (should fail)", false),

	SimpleTests("INSERT INTO Servers VALUES( 108, 'http://www.ServersTest8', 1)",
				"Servers, prep for test 8", true),
	SimpleTests("UPDATE Servers SET ServerID = 118 WHERE ServerID = 108",
				"Servers, update ServerID (should fail)", false),


	SimpleTests("INSERT INTO Servers VALUES( 111, 'http://www.ServersTest11', 1)",
				"Servers, prep for test 11", true),
	SimpleTests("UPDATE Servers SET URL = 'http://www.ServersTest21' WHERE ServerID = 111",
				"Servers, update URL", true),

	SimpleTests("INSERT INTO Servers VALUES( 112, 'http://www.ServersTest12', 1)",
				"Servers, prep for test 12", true),
	SimpleTests("INSERT INTO Servers VALUES( 132, 'http://www.ServersTest12b', 1)",
				"Servers, prep for test 12", true),
	SimpleTests("UPDATE Servers SET URL = 'http://www.ServersTest12b' WHERE ServerID = 112",
				"Servers, update URL to a duplicate (should fail", false),

	SimpleTests("INSERT INTO Servers VALUES( 113, 'http://www.ServersTest13', 1)",
				"Servers, prep for test 13", true),
	SimpleTests("DELETE FROM Servers WHERE ServerID = 113",
				"Servers, delete a record (should fail)", false),

	SimpleTests("", "", true)	// Array terminator
};


bool DatabaseTests::Servers(DBSession& connection, Reporter& reporter)
{
	SQLStmt	sql(connection, "Servers", exceptType);

	return RunSimpleTests(sql, reporter, SimpleServersTests);
}



//
//	 
//
//	Test the Attributes table.  Ensure that following things
//	work correctly (this includes making sure that incorrect things
//	are properly bounced by the database.)
//
//	Insert a record with all fields filled and valid
//	Insert a record with a null LocalCookie (should fail)
//	Insert a record with a null AttributeKey (should fail)
//	Insert a record with a null Value (should fail)
//	Insert a record with a duplicate LocalCookie
//	Insert a record with a duplicate AttributeKey
//	Insert a record with a duplicate Value
//	Insert a record with a duplicate LocalCookie & AttributeKey,
//			pair (should fail) 
//	Insert a record with an invalid LocalCookie (should fail)
//	Insert a record with an invalid AttributeKey (should fail)
//	Update LocalCookie (should fail)
//	Update AttributeKey (should fail)
//	Update Value
//	Update Value to null (should fail)
//	Delete a record
//	

static	SimpleTests	SimpleAttributesTests[] =
{
	SimpleTests("INSERT INTO Submittors(SubmittorID) VALUES('AttributesTests')",
				"Attributes, prep for Tests (Submittors)", true),

//	SimpleTests("INSERT INTO Platforms(HWCorrelator) VALUES('AttributesTests')",
//				"Attributes, prep for Tests (Platforms)", true),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (1, 1, 'AttributeTest1', 1, 'AttributesTests', 0)",
				"Attributes, prep for Test 1", true),
	SimpleTests("INSERT INTO Attributes VALUES( 1, 1, 'AttributeTest1')",
				"Attributes, insert a record with all fields filled and valid", true),

	SimpleTests("INSERT INTO Attributes VALUES( NULL, 2, 'AttributeTest2')",
				"Attributes, insert a record with a null LocalCookie (should fail)", false),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (4, 4, 'AttributeTest4', 14, 'AttributesTests', 0)",
				"Attributes, prep for Test 4", true),
	SimpleTests("INSERT INTO Attributes VALUES( 4, NULL, 'AttributeTest4')",
				"Attributes, insert a record with a null AttributeKey (should fail)", false),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (5, 5, 'AttributeTest5', 15, 'AttributesTests', 0)",
				"Attributes, prep for Test 5", true),
	SimpleTests("INSERT INTO Attributes VALUES( 5, 5, NULL)",
				"Attributes, insert a record with a null Value (should fail)", false),


	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (6, 6, 'AttributeTest6', 16, 'AttributesTests', 0)",
				"Attributes, prep for Test 6", true),
	SimpleTests("INSERT INTO Attributes VALUES( 6, 66, 'AttributeTest6')",
				"Attributes, prep for Test 6", true),
	SimpleTests("INSERT INTO Attributes VALUES( 6, 67, 'AttributeTest6b')",
				"Attributes, insert a record with a duplicate LocalCookie", true),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (8, 8, 'AttributeTest8', 18, 'AttributesTests', 0)",
				"Attributes, prep for Test 8", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (38, 38, 'AttributeTest38', 18, 'AttributesTests', 0)",
				"Attributes, prep for Test 8", true),
	SimpleTests("INSERT INTO Attributes VALUES( 8, 8, 'AttributeTest8')",
				"Attributes, prep for Test 8", true),
	SimpleTests("INSERT INTO Attributes VALUES( 38,  8, 'AttributeTest8b')",
				"Attributes, insert a record with a duplicate AttributeKey", true),


	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (9, 9, 'AttributeTest9', 69, 'AttributesTests', 0)",
				"Attributes, prep for Test 9", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (39, 39, 'AttributeTest9b', 69, 'AttributesTests', 0)",
				"Attributes, prep for Test 9", true),
	SimpleTests("INSERT INTO Attributes VALUES( 9, 69, 'AttributeTest9')",
				"Attributes, prep for Test 9", true),
	SimpleTests("INSERT INTO Attributes VALUES( 39, 69, 'AttributeTest9')",
				"Attributes, insert a record with a duplicate Value", true),


	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (10, 10, 'AttributeTest10', 10, 'AttributesTests', 0)",
				"Attributes, prep for Test 10", true),
	SimpleTests("INSERT INTO Attributes VALUES( 10, 10, 'AttributeTest10')",
				"Attributes, prep for Test 10", true),
	SimpleTests("INSERT INTO Attributes VALUES( 10, 10, 'AttributeTest10b')",
				"Attributes, insert a record with a duplicate LocalCookie & AttributeKey pair (should fail)", false),

	SimpleTests("INSERT INTO Attributes VALUES( 11, 11, 'AttributeTest11')",
				"Attributes, insert a record with an invalid LocalCookie (should fail)", false),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (13, 13, 'AttributeTest13', 13, 'AttributesTests', 0)",
				"Attributes, prep for Test 13", true),
	SimpleTests("INSERT INTO Attributes VALUES( 13, 2000, 'AttributeTest13')",
				"Attributes, insert a record with an invalid AttributeKey (should fail)", false),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (14, 14, 'AttributeTest14', 14, 'AttributesTests', 0)",
				"Attributes, prep for Test 14", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (214, 214, 'AttributeTest214', 14, 'AttributesTests', 0)",
				"Attributes, prep for Test 14", true),
	SimpleTests("INSERT INTO Attributes VALUES( 14, 24, 'AttributeTest14')",
				"Attributes, prep for Test 14", true),
	SimpleTests("UPDATE Attributes SET LocalCookie = 214 WHERE LocalCookie = 14",
				"Attributes, update LocalCookie (should fail)", false),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (16, 16, 'AttributeTest16', 16, 'AttributesTests', 0)",
				"Attributes, prep for Test 16 - a", true),
	SimpleTests("INSERT INTO Attributes VALUES( 16, 66, 'AttributeTest16')",
				"Attributes, prep for Test 16 - b", true),
	SimpleTests("UPDATE Attributes SET AttributeKey = 56 WHERE LocalCookie = 16",
				"Attributes, update AttributeKey (should fail)", false),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (17, 17, 'AttributeTest17', 17, 'AttributesTests', 0)",
				"Attributes, prep for Test 17 - a", true),
	SimpleTests("INSERT INTO Attributes VALUES( 17, 67, 'AttributeTest17')",
				"Attributes, prep for Test 17 - b", true),
	SimpleTests("UPDATE Attributes SET Value = 'AttributeTest17b' WHERE LocalCookie = 17",
				"Attributes, update Value", true),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (18, 18, 'AttributeTest18', 18, 'AttributesTests', 0)",
				"Attributes, prep for Test 18", true),
	SimpleTests("INSERT INTO Attributes VALUES( 18, 68, 'AttributeTest18')",
				"Attributes, prep for Test 18", true),
	SimpleTests("UPDATE Attributes SET Value = NULL WHERE LocalCookie = 18",
				"Attributes, update Value to null (should fail)", false),

	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, SubmittorID, RemoteServerID) VALUES (19, 19, 'AttributeTest19', 59, 'AttributesTests', 0)",
				"Attributes, prep for Test 19", true),
	SimpleTests("INSERT INTO Attributes VALUES( 19, 69, 'AttributeTest19')",
				"Attributes, prep for Test 19", true),
	SimpleTests("DELETE FROM Attributes WHERE LocalCookie = 19",
				"Attributes, delete a record", true),


	//
	//	Clean up some of the records created by this set of tests

	SimpleTests("DELETE FROM Attributes WHERE Value LIKE 'AttributeTest%'",
				"Attributes, remove Attributes test records", true),
	SimpleTests("DELETE FROM AnalysisRequest WHERE CheckSum LIKE 'AttributeTest%'",
				"Attributes, remove AnalysisRequest test records", true),

	SimpleTests("", "", true)	// Array terminator
};

bool DatabaseTests::Attributes(DBSession& connection, Reporter& reporter)
{
	SQLStmt	sql(connection, "Attributes", exceptType);

	return RunSimpleTests(sql, reporter, SimpleAttributesTests);
}


//
//	AttributeKeys
//
//	Test the AttributeKeys table.  Ensure that following things
//	work correctly (this includes making sure that incorrect things
//	are properly bounced by the database.)
//
//	Insert a record with all fields filled and valid
//	Insert a record with a null AttributeKey (should fail)
//	Insert a record with a null Text value (should fail)
//	Insert a record with duplicate AttributeKey value (should fail)
//	Insert a record with duplicate Text value (should fail)
//	Update AttributeKey with a valid value (should fail)
//	Update Text with a valid value (should fail)
//	Delete a record (should fail)

static	SimpleTests	SimpleAttributeKeysTests[] =
{
	SimpleTests("INSERT INTO AttributeKeys VALUES( 501, 'AttKeysTest1')",
				"AttributeKeys, insert a record with all fields filled and valid", true),
	SimpleTests("INSERT INTO AttributeKeys VALUES( NULL, 'AttKeysTest2')",
				"AttributeKeys, insert a record with a null AttributeKey (should fail)", false),
	SimpleTests("INSERT INTO AttributeKeys VALUES( 503, NULL)",
				"AttributeKeys, insert a record with a null Text (should fail)", false),

	SimpleTests("INSERT INTO AttributeKeys VALUES( 504, 'AttKeysTest4')",
				"AttributeKeys, prep for test 4", true),
	SimpleTests("INSERT INTO AttributeKeys VALUES( 504, 'AttKeysTest4b')",
				"AttributeKeys, insert a record with a duplicate AttributeKey (should fail)", false),

	SimpleTests("INSERT INTO AttributeKeys VALUES( 505, 'AttKeysTest5')",
				"AttributeKeys, prep for test 5", true),
	SimpleTests("INSERT INTO AttributeKeys VALUES( 515, 'AttKeysTest5')",
				"AttributeKeys, insert a record with a duplicate Text (should fail)", false),

	SimpleTests("INSERT INTO AttributeKeys VALUES( 506, 'AttKeysTest6')",
				"AttributeKeys, prep for test 6", true),
	SimpleTests("UPDATE AttributeKeys SET AttributeKey = 516 WHERE AttributeKey = 506",
				"AttributeKeys, update AttributeKey (should fail)", false),

	SimpleTests("INSERT INTO AttributeKeys VALUES( 507, 'AttKeysTest7')",
				"AttributeKeys, prep for test 7", true),
	SimpleTests("UPDATE AttributeKeys SET Text = 'AttKeysTest7b' WHERE AttributeKey = 507",
				"AttributeKeys, update Text (should fail)", false),

	SimpleTests("INSERT INTO AttributeKeys VALUES( 508, 'AttKeysTest8')",
				"AttributeKeys, prep for test 8", true),
	SimpleTests("DELETE AttributeKeys WHERE AttributeKey = 508",
				"AttributeKeys, delete a record (should fail)", false),

	SimpleTests("", "", true)	// Array terminator
};

bool DatabaseTests::AttributeKeys(DBSession& connection, Reporter& reporter)
{
	SQLStmt	sql(connection, "AttributeKeys", exceptType);

	return RunSimpleTests(sql, reporter, SimpleAttributeKeysTests);
}


//
//	AnalysisStateInfo
//
//	Test the AnalysisStateInfo table.  Ensure that following things
//	work correctly (this includes making sure that incorrect things
//	are properly bounced by the database.)
//
//	Insert a record with all fields filled and valid
//	Insert a record with a null AnalysisStateID (should fail)
//	Insert a record with a null name (should fail)
//	Insert a record duplicate AnalysisStateID (should fail)
//	Insert a record duplicate Name (should fail)
//	Update AnalysisStateID with a valid value (should fail)
//	Update Name with a valid value (should fail)
//	Delete a record (should fail)

static	SimpleTests	SimpleAnalysisStateInfoTests[] =
{
	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( 101, 'AnalysisStateInfoTest1')",
				"AnalysisStateInfo, insert a record with all fields filled and valid", true),

	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( NULL, 'AnalysisStateInfoTest2')",
				"AnalysisStateInfo, insert a record with a null AnalysisStateID (should fail)", false),
	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( 103, NULL)",
				"AnalysisStateInfo, insert a record with a null name (should fail)", false),

	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( 104, 'AnalysisStateInfoTest4')",
				"AnalysisStateInfo, prep for test 4", true),
	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( 104, 'AnalysisStateInfoTest4b')",
				"AnalysisStateInfo, insert a record with a duplicte AnalysisStateID (should fail)", false),

	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( 105, 'AnalysisStateInfoTest5')",
				"AnalysisStateInfo, prep for test 5", true),
	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( 205, 'AnalysisStateInfoTest5')",
				"AnalysisStateInfo, insert a record with a duplicte Name (should fail)", false),

	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( 106, 'AnalysisStateInfoTest6')",
				"AnalysisStateInfo, prep for test 6", true),
	SimpleTests("UPDATE AnalysisStateInfo SET AnalysisStateID = 206 WHERE AnalysisStateID = 106)",
				"AnalysisStateInfo, update AnalysisStateID with a valid value (should fail)", false),

	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( 107, 'AnalysisStateInfoTest7')",
				"AnalysisStateInfo, prep for test 7", true),
	SimpleTests("UPDATE AnalysisStateInfo SET Name = 'AnalysisStateInfoTest7a' WHERE AnalysisStateID = 107)",
				"AnalysisStateInfo, update name with a valid value (should fail)", false),

	SimpleTests("INSERT INTO AnalysisStateInfo VALUES( 108, 'AnalysisStateInfoTest8')",
				"AnalysisStateInfo, prep for test 8", true),
	SimpleTests("DELETE FROM AnalysisStateInfo WHERE AnalysisStateID = 108",
				"AnalysisStateInfo, delete a record (should fail)", false),

	SimpleTests("", "", true)	// Array terminator
};

bool DatabaseTests::AnalysisStateInfo(DBSession& connection, Reporter& reporter)
{
	SQLStmt	sql(connection, "AnalysisStateInfo", exceptType);

	return RunSimpleTests(sql, reporter, SimpleAnalysisStateInfoTests);
}


//
//	AnalysisRequest
//
//	Test the AnalysisRequest table.  Ensure that following things
//	work correctly (this includes making sure that incorrect things
//	are properly bounced by the database.)
//
//	Insert a record with all fields filled and valid
//	Insert a record with a null LocalCookie (should fail)
//	Insert a record with a null RemoteCookie
//	Insert a record with a null Informed
//	Insert a record with a null CheckSum (should fail)
//	Insert a record with a null AnalysisStateID (should fail)
//	Insert a record with a null SubmittorID (should fail)
//	Insert a record with a null RemoteServerID
//
//	The above tests also test the following insert a record with a null field tests
//	Insert a record with a null Priority
//	Insert a record with a null Imported
//	Insert a record with a null LastStateChange
//	Insert a record with a null NotifyClient
//	Insert a record with a null NotifySymantec
//
//	Insert a duplicate LocalCookie (should fail)
//	Insert a duplicate Imported
//	Insert a duplicate Informed
//	Insert a duplicate LastStateChange
//	Insert a duplicate CheckSum
//	Insert a duplicate AnalysisStateID
//	Insert a duplicate SubmittorID
//	Insert a duplicate RemoteCookie/RemoteServerID pair (should fail)
//	Insert a duplicate RemoteCookie (RemoteServerID different)
//	Insert a duplicate RemoteServerID (RemoteCookie different)
//
//	The above tests also test the following insert a duplicate record tests
//	Insert a duplicate Priority
//	Insert a duplicate NotifyClient
//	Insert a duplicate NotifySymantec
//
//	Insert an invalid AnalysisStateID (should fail)
//	Insert an invalid SubmittorID (should fail)
//	Insert an invalid RemoteServerID (should fail)
//	Insert an invalid NotifyClient (should fail)
//	Insert an invalid NotifySymantec (should fail)
//	Update LocalCookie to a valid value (should fail)
//	Update Priority to a valid value
//	Update Imported to a valid value
//	Update Informed to a valid value 
//	Update LastStateChange to a valid value
//	Update CheckSum to a valid value (should fail)
//	Update NotifyClient to a valid value
//	Update NotifySymantec to a valid value
//	Update AnalysisStateID to a valid value
//	Update AnalysisStateID to an invalid value
//	Update SubmittorID to a valid value (should fail)
//	Update RemoteServerID to a valid value (should fail)
//	Update RemoteCookie to a valid value (should fail)
//	Insert a duplicate SubmittorID, ensure that Submittor usage goes up
//	Insert a duplicate SubmittorID, ensure that if any Submittor limit is
//			exceeded that the Insert will fail.
//	Delete AnalysisRequest, ensure that associated attributes get removed,
//			and other attributes remain.
//
//	Insert a record with Priority, Imported, Informed, LastStateChange, NotifyClient,
//			and NotifySymantec not defined and ensure that defaults are used
//	Update AnalysisStateID and ensure that LastStateChange, NotifyClient, &
//			NotifySymantec are properly updated.
//	

static	SimpleTests	SimpleAnalysisRequestTests[] =
{
//	SimpleTests("INSERT INTO Platforms (HWCorrelator) VALUES('AnalysisRequestTest1')",
//				"AnalysisRequest, prep for tests", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest1')",
				"AnalysisRequest, prep for test 1", true),
	SimpleTests("INSERT INTO Servers VALUES( 601, 'AnalysisRqst1', 1)",
				"AnalysisRequest, prep for test 1", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID) VALUES( 601, 601, 'AnalysisRequestTest1', 1, '1998-8-28-08.00.01.01', 'AnalysisRequestTest1', 601)",
				"AnalysisRequest, insert a record with all fields filled and valid", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest2b')",
				"AnalysisRequest, prep for test 2b", true),
	SimpleTests("INSERT INTO Servers VALUES( 702, 'AnalysisRqst2b', 1)",
				"AnalysisRequest, prep for test 2b", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( NULL, 702, 'AnalysisRequestTest2b', 702, '1999-8-28-08.00.02.01', 'AnalysisRequestTest2b', 702)",
				"AnalysisRequest, insert a record with a null LocalCookie (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest2')",
				"AnalysisRequest, prep for test 2", true),
	SimpleTests("INSERT INTO Servers VALUES( 602, 'AnalysisRqst2', 1)",
				"AnalysisRequest, prep for test 2", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 602, NULL, 'AnalysisRequestTest2', 12, '1998-8-28-08.00.02.01', 'AnalysisRequestTest2', NULL)",
				"AnalysisRequest, insert a record with a null RemoteCookie", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest3')",
				"AnalysisRequest, prep for test 3", true),
	SimpleTests("INSERT INTO Servers VALUES( 603, 'AnalysisRqst3', 1)",
				"AnalysisRequest, prep for test 3", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 603, 603, NULL, 13, '1998-8-28-08.00.03.01', 'AnalysisRequestTest3', 603)",
				"AnalysisRequest, insert a record with a null CheckSum (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest4')",
				"AnalysisRequest, prep for test 4", true),
	SimpleTests("INSERT INTO Servers VALUES( 604, 'AnalysisRqst4', 1)",
				"AnalysisRequest, prep for test 4", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 604, 604, 'AnalysisRequestTest4', NULL, '1998-8-28-08.00.04.01', 'AnalysisRequestTest4', 604)",
				"AnalysisRequest, insert a record with a null AnalysisStateID (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest5')",
				"AnalysisRequest, prep for test 5", true),
	SimpleTests("INSERT INTO Servers VALUES( 605, 'AnalysisRqst5', 1)",
				"AnalysisRequest, prep for test 5", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 605, 605, 'AnalysisRequestTest5', 15, NULL, 'AnalysisRequestTest5', 605)",
				"AnalysisRequest, insert a record with a null Informed", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest6')",
				"AnalysisRequest, prep for test 6", true),
	SimpleTests("INSERT INTO Servers VALUES( 606, 'AnalysisRqst6', 1)",
				"AnalysisRequest, prep for test 6", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 606, 606, 'AnalysisRequestTest6', 16, '1998-8-28-08.00.06.01', NULL, 606)",
				"AnalysisRequest, insert a record with a null Submittor (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest7')",
				"AnalysisRequest, prep for test 7", true),
	SimpleTests("INSERT INTO Servers VALUES( 607, 'AnalysisRqst7', 1)",
				"AnalysisRequest, prep for test 7", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 607, NULL, 'AnalysisRequestTest7', 17, '1998-8-28-08.00.07.01', 'AnalysisRequestTest7', NULL)",
				"AnalysisRequest, insert a record with a null RemoteServerID", true),


	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest8x')",
				"AnalysisRequest, prep for test 8b", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest8y')",
				"AnalysisRequest, prep for test 8b", true),
	SimpleTests("INSERT INTO Servers VALUES( 708, 'AnalysisRqst8x', 1)",
				"AnalysisRequest, prep for test 8b", true),
	SimpleTests("INSERT INTO Servers VALUES( 908, 'AnalysisRqst8y', 1)",
				"AnalysisRequest, prep for test 8b", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 708, 808, 'AnalysisRequestTest8x', 56, '1999-8-28-08.00.08.01', 'AnalysisRequestTest8x', 708)",
				"AnalysisRequest, prep for test 8b", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 708, 908, 'AnalysisRequestTest8y', 57, '1999-8-28-08.00.08.02', 'AnalysisRequestTest8y', 908)",
				"AnalysisRequest, insert a record with a duplicate LocalCookie (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest8')",
				"AnalysisRequest, prep for test 8", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest8b')",
				"AnalysisRequest, prep for test 8", true),
	SimpleTests("INSERT INTO Servers VALUES( 608, 'AnalysisRqst8', 1)",
				"AnalysisRequest, prep for test 8", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 608, 608, 'AnalysisRequestTest8', 58, '1998-8-28-08.00.08.01', 'AnalysisRequestTest8', 602)",
				"AnalysisRequest, prep for test 8", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 708, 608, 'AnalysisRequestTest8b', 18, '1998-8-28-08.00.08.02', 'AnalysisRequestTest8b', 602)",
				"AnalysisRequest, insert a record with a duplicate RemoteCookie/ServerID pair (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest9')",
				"AnalysisRequest, prep for test 9", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest9b')",
				"AnalysisRequest, prep for test 9", true),
	SimpleTests("INSERT INTO Servers VALUES( 609, 'AnalysisRqst9', 1)",
				"AnalysisRequest, prep for test 9", true),
	SimpleTests("INSERT INTO Servers VALUES( 709, 'AnalysisRqst9b', 1)",
				"AnalysisRequest, prep for test 9", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 609, 609, 'AnalysisRequestTest9', 59, '1998-8-28-08.00.09.01', 'AnalysisRequestTest9', 609)",
				"AnalysisRequest, prep for test 9", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 709, 609, 'AnalysisRequestTest9b', 59, '1998-8-28-08.00.09.02', 'AnalysisRequestTest9b', 709)",
				"AnalysisRequest, insert a record with a duplicate RemoteCookie (ServerID different)", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest10')",
				"AnalysisRequest, prep for test 10", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest10b')",
				"AnalysisRequest, prep for test 10", true),
	SimpleTests("INSERT INTO Servers VALUES( 610, 'AnalysisRqst10', 1)",
				"AnalysisRequest, prep for test 10", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 610, 610, 'AnalysisRequestTest10', 10, '1998-8-28-08.00.10.01', 'AnalysisRequestTest10', 609)",
				"AnalysisRequest, prep for test 10", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 710, 710, 'AnalysisRequestTest10b', 30, '1998-8-28-08.00.10.02', 'AnalysisRequestTest10b', 609)",
				"AnalysisRequest, insert a record with a duplicate ServerID (RemoteCookie different)", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest11.1')",
				"AnalysisRequest, prep for test 11.1", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest11.1b')",
				"AnalysisRequest, prep for test 11.1", true),
	SimpleTests("INSERT INTO Servers VALUES( 1611, 'AnlyssRqst11.1', 1)",
				"AnalysisRequest, prep for test 11.1", true),
	SimpleTests("INSERT INTO Servers VALUES( 1711, 'AnlyssRqst11.1b', 1)",
				"AnalysisRequest, prep for test 11.1", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID, Imported)  VALUES( 1611, 1611, 'AnalysisRequestTest11.1', 11, '1998-8-28-08.00.11.01', 'AnalysisRequestTest11.1', 1611, '1998-8-28-08.00.11.01')",
				"AnalysisRequest, prep for test 11.1", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID, Imported)  VALUES( 1711, 1711, 'AnalysisRequestTest11.1', 31, '1998-8-28-08.00.11.02', 'AnalysisRequestTest11.1b', 1711, '1998-8-28-08.00.11.01')",
				"AnalysisRequest, insert a record with a duplicate Imported", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest11.2')",
				"AnalysisRequest, prep for test 11.2.1", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest11.2b')",
				"AnalysisRequest, prep for test 11.2.2", true),
	SimpleTests("INSERT INTO Servers VALUES( 1612, 'AnlyssRqst11.2', 1)",
				"AnalysisRequest, prep for test 11.2.3", true),
	SimpleTests("INSERT INTO Servers VALUES( 1712, 'AnlyssRqst11.2b', 1)",
				"AnalysisRequest, prep for test 11.2.4", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID, LastStateChange)  VALUES( 1612, 1612, 'AnalysisRequestTest11.2', 11, '1998-8-28-08.00.11.01', 'AnalysisRequestTest11.2', 1612, '1998-8-28-08.00.11.01')",
				"AnalysisRequest, prep for test 11.2.5", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID, LastStateChange)  VALUES( 1712, 1712, 'AnalysisRequestTest11.2', 31, '1998-8-28-08.00.11.02', 'AnalysisRequestTest11.2b', 1712, '1998-8-28-08.00.11.01')",
				"AnalysisRequest, insert a record with a duplicate LastStateChange", true),


	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest11')",
				"AnalysisRequest, prep for test 11", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest11b')",
				"AnalysisRequest, prep for test 11", true),
	SimpleTests("INSERT INTO Servers VALUES( 611, 'AnalysisRqst11', 1)",
				"AnalysisRequest, prep for test 11", true),
	SimpleTests("INSERT INTO Servers VALUES( 711, 'AnalysisRqst11b', 1)",
				"AnalysisRequest, prep for test 11", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 611, 611, 'AnalysisRequestTest11', 11, '1998-8-28-08.00.11.01', 'AnalysisRequestTest11', 611)",
				"AnalysisRequest, prep for test 11", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 711, 711, 'AnalysisRequestTest11', 31, '1998-8-28-08.00.11.02', 'AnalysisRequestTest11b', 711)",
				"AnalysisRequest, insert a record with a duplicate CheckSum", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest12')",
				"AnalysisRequest, prep for test 12", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest12b')",
				"AnalysisRequest, prep for test 12", true),
	SimpleTests("INSERT INTO Servers VALUES( 612, 'AnalysisRqst12', 1)",
				"AnalysisRequest, prep for test 12", true),
	SimpleTests("INSERT INTO Servers VALUES( 712, 'AnalysisRqst12b', 1)",
				"AnalysisRequest, prep for test 12", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 612, 612, 'AnalysisRequestTest12',  12, '1998-8-28-08.00.12.01', 'AnalysisRequestTest12', 612)",
				"AnalysisRequest, prep for test 12", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 712, 712, 'AnalysisRequestTest12b', 12, '1998-8-28-08.00.12.02', 'AnalysisRequestTest12b', 712)",
				"AnalysisRequest, insert a record with a duplicate AnalysisStateID", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest13')",
				"AnalysisRequest, prep for test 13", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest13b')",
				"AnalysisRequest, prep for test 13", true),
	SimpleTests("INSERT INTO Servers VALUES( 613, 'AnalysisRqst13', 1)",
				"AnalysisRequest, prep for test 13", true),
	SimpleTests("INSERT INTO Servers VALUES( 713, 'AnalysisRqst13b', 1)",
				"AnalysisRequest, prep for test 13", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 613, 613, 'AnalysisRequestTest13',  13, '1998-8-28-08.00.13.01', 'AnalysisRequestTest13', 613)",
				"AnalysisRequest, prep for test 13", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 713, 713, 'AnalysisRequestTest13b', 53, '1998-8-28-08.00.13.01', 'AnalysisRequestTest13b', 713)",
				"AnalysisRequest, insert a record with a duplicate Informed", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest14')",
				"AnalysisRequest, prep for test 14", true),
	SimpleTests("INSERT INTO Servers VALUES( 614, 'AnalysisRqst14', 1)",
				"AnalysisRequest, prep for test 14", true),
	SimpleTests("INSERT INTO Servers VALUES( 714, 'AnalysisRqst14b', 1)",
				"AnalysisRequest, prep for test 14", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 614, 614, 'AnalysisRequestTest14', 14, '1998-8-28-08.00.14.01', 'AnalysisRequestTest14', 611)",
				"AnalysisRequest, prep for test 14", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 714, 714, 'AnalysisRequestTest14', 54, '1998-8-28-08.00.14.02', 'AnalysisRequestTest14', 714)",
				"AnalysisRequest, insert a record with a duplicate CheckSum", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest15')",
				"AnalysisRequest, prep for test 15", true),
	SimpleTests("INSERT INTO Servers VALUES( 615, 'AnalysisRqst15', 1)",
				"AnalysisRequest, prep for test 15", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 615, 615, 'AnalysisRequestTest15', 295, '1998-8-28-08.00.15.01', 'AnalysisRequestTest15', 615)",
				"AnalysisRequest, insert an invalid AnalysisStateID", false),

	SimpleTests("INSERT INTO Servers VALUES( 616, 'AnalysisRqst16', 1)",
				"AnalysisRequest, prep for test 16", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 616, 616, 'AnalysisRequestTest16', 16, '1998-8-28-08.00.16.01', 'AnalysisRequestTest16', 616)",
				"AnalysisRequest, insert an invalid SubmittorID", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest17')",
				"AnalysisRequest, prep for test 17", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 617, 617, 'AnalysisRequestTest17', 17, '1998-8-28-08.00.17.01', 'AnalysisRequestTest17', 617)",
				"AnalysisRequest, insert an invalid RemoteServerID", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest17b')",
				"AnalysisRequest, prep for test 17b", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID, NotifyClient)  VALUES( 617, 617, 'AnalysisRequestTest17b', 17, '1998-8-28-08.01.17.01', 'AnalysisRequestTest17b', 617, X'04')",
				"AnalysisRequest, insert an invalid NotifyClient", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest17c')",
				"AnalysisRequest, prep for test 17c", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID, NotifySymantec)  VALUES( 617, 617, 'AnalysisRequestTest17c', 17, '1998-8-28-08.02.17.01', 'AnalysisRequestTest17c', 617, X'04')",
				"AnalysisRequest, insert an invalid NotifySymantec", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest18b')",
				"AnalysisRequest, prep for test 18b", true),
	SimpleTests("INSERT INTO Servers VALUES( 718, 'AnalysisRqst18b', 1)",
				"AnalysisRequest, prep for test 18b", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 918, 918, 'AnalysisRequestTest18b', 57, '1999-8-28-08.00.18.01', 'AnalysisRequestTest18b', 718)",
				"AnalysisRequest, prep for test 18b", true),
	SimpleTests("UPDATE AnalysisRequest SET LocalCookie = 1018 WHERE LocalCookie = 918",
				"AnalysisRequest, update LocalCookie to a valid value (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest18')",
				"AnalysisRequest, prep for test 18", true),
	SimpleTests("INSERT INTO Servers VALUES( 618, 'AnalysisRqst18', 1)",
				"AnalysisRequest, prep for test 18", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 618, 618, 'AnalysisRequestTest18', 18, '1998-8-28-08.00.18.01', 'AnalysisRequestTest18', 618)",
				"AnalysisRequest, prep for test 18", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest19')",
				"AnalysisRequest, prep for test 19", true),
	SimpleTests("INSERT INTO Servers VALUES( 619, 'AnalysisRqst19', 1)",
				"AnalysisRequest, prep for test 19", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 619, 619, 'AnalysisRequestTest19', 59, '1998-8-28-08.00.19.01', 'AnalysisRequestTest19', 619)",
				"AnalysisRequest, prep for test 19", true),
	SimpleTests("UPDATE AnalysisRequest SET CheckSum = 'AnalysisRequestTest19b' WHERE RemoteCookie = 619",
				"AnalysisRequest, update CheckSum to a valid value (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest20')",
				"AnalysisRequest, prep for test 20", true),
	SimpleTests("INSERT INTO Servers VALUES( 620, 'AnalysisRqst20', 1)",
				"AnalysisRequest, prep for test 20", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 620, 620, 'AnalysisRequestTest20', 30, '1998-8-28-08.00.20.01', 'AnalysisRequestTest20', 620)",
				"AnalysisRequest, prep for test 20", true),
	SimpleTests("UPDATE AnalysisRequest SET AnalysisStateID = 57 WHERE RemoteCookie = 620",
				"AnalysisRequest, update AnalysisStateID to a valid value", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest20b')",
				"AnalysisRequest, prep for test 20b.1", true),
	SimpleTests("INSERT INTO Servers VALUES( 1620, 'AnalysisRqst20b', 1)",
				"AnalysisRequest, prep for test 20b.2", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 1620, 1620, 'AnalysisRequestTest20b', 30, '1998-8-28-08.00.20.01', 'AnalysisRequestTest20b', 1620)",
				"AnalysisRequest, prep for test 20b.3", true),
	SimpleTests("UPDATE AnalysisRequest SET Priority = 27 WHERE RemoteCookie = 1620",
				"AnalysisRequest, update Priority to a valid value", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest20c')",
				"AnalysisRequest, prep for test 20c", true),
	SimpleTests("INSERT INTO Servers VALUES( 1621, 'AnalysisRqst20c', 1)",
				"AnalysisRequest, prep for test 20c", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 1621, 1621, 'AnalysisRequestTest20c', 30, '1998-8-28-08.00.20.01', 'AnalysisRequestTest20c', 1621)",
				"AnalysisRequest, prep for test 20c", true),
	SimpleTests("UPDATE AnalysisRequest SET Imported = '1988-8-28-08.00.20.01' WHERE RemoteCookie = 1621",
				"AnalysisRequest, update Imported to a valid value", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest20d')",
				"AnalysisRequest, prep for test 20d", true),
	SimpleTests("INSERT INTO Servers VALUES( 1622, 'AnalysisRqst20d', 1)",
				"AnalysisRequest, prep for test 20d", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 1622, 1622, 'AnalysisRequestTest20d', 30, '1998-8-28-08.00.20.01', 'AnalysisRequestTest20d', 1622)",
				"AnalysisRequest, prep for test 20d", true),
	SimpleTests("UPDATE AnalysisRequest SET LastStateChange = '1988-8-28-08.00.20.01' WHERE RemoteCookie = 1622",
				"AnalysisRequest, update LastStateChange to a valid value", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest20e')",
				"AnalysisRequest, prep for test 20e", true),
	SimpleTests("INSERT INTO Servers VALUES( 1623, 'AnalysisRqst20e', 1)",
				"AnalysisRequest, prep for test 20e", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 1623, 1623, 'AnalysisRequestTest20e', 30, '1998-8-28-08.00.20.01', 'AnalysisRequestTest20e', 1623)",
				"AnalysisRequest, prep for test 20e", true),
	SimpleTests("UPDATE AnalysisRequest SET NotifyClient = X'00' WHERE RemoteCookie = 1623",
				"AnalysisRequest, update NotifyClient to a valid value", true),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest20f')",
				"AnalysisRequest, prep for test 20f", true),
	SimpleTests("INSERT INTO Servers VALUES( 1624, 'AnalysisRqst20f', 1)",
				"AnalysisRequest, prep for test 20f", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 1624, 1624, 'AnalysisRequestTest20f', 30, '1998-8-28-08.00.20.01', 'AnalysisRequestTest20f', 1624)",
				"AnalysisRequest, prep for test 20f", true),
	SimpleTests("UPDATE AnalysisRequest SET NotifySymantec = X'01' WHERE RemoteCookie = 1624",
				"AnalysisRequest, update NotifySymantec to a valid value", true),


	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest21')",
				"AnalysisRequest, prep for test 21", true),
	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest21b')",
				"AnalysisRequest, prep for test 21", true),
	SimpleTests("INSERT INTO Servers VALUES( 621, 'AnalysisRqst21', 1)",
				"AnalysisRequest, prep for test 21", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 621, 621, 'AnalysisRequestTest21', 31, '1998-8-28-08.00.21.01', 'AnalysisRequestTest21', 621)",
				"AnalysisRequest, prep for test 21", true),
	SimpleTests("UPDATE AnalysisRequest SET SubmittorID = 'AnalysisRequestTest21b WHERE RemoteCookie = 621",
				"AnalysisRequest, update SubmittorID to a valid value (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest22')",
				"AnalysisRequest, prep for test 22", true),
	SimpleTests("INSERT INTO Servers VALUES( 622, 'AnalysisRqst22', 1)",
				"AnalysisRequest, prep for test 22", true),
	SimpleTests("INSERT INTO Servers VALUES( 722, 'AnalysisRqst22b', 1)",
				"AnalysisRequest, prep for test 22", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 622, 622, 'AnalysisRequestTest20', 30, '1998-8-28-08.00.20.01', 'AnalysisRequestTest20', 620)",
				"AnalysisRequest, prep for test 22", true),
	SimpleTests("UPDATE AnalysisRequest SET ServerID = 722 WHERE RemoteCookie = 622",
				"AnalysisRequest, update ServerID to a valid value (should fail)", false),

	SimpleTests("INSERT INTO Submittors (SubmittorID) VALUES('AnalysisRequestTest23')",
				"AnalysisRequest, prep for test 23", true),
	SimpleTests("INSERT INTO Servers VALUES( 623, 'AnalysisRqst23', 1)",
				"AnalysisRequest, prep for test 23", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 623, 623, 'AnalysisRequestTest23', 53, '1998-8-28-08.00.23.01', 'AnalysisRequestTest23', 623)",
				"AnalysisRequest, prep for test 23", true),
	SimpleTests("UPDATE AnalysisRequest SET AnalysisStateID = 9347 WHERE RemoteCookie = 623",
				"AnalysisRequest, update AnalysisStateID to an invalid value (should fail)", false),
/*
	SimpleTests("INSERT INTO Submittors VALUES('AnalysisRequestTest30', 99, 1, 1, 1)",
				"AnalysisRequest, prep for test 30", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, HWCorrelator, RemoteServerID)  VALUES( 630, 630, 'AnalysisRequestTest30', 1, NULL, 'AnalysisRequestTest30', 'AnalysisRequestTest1', 623)",
				"AnalysisRequest, Add new request, exceed daily usage maximum (should fail)", false),

	SimpleTests("INSERT INTO Submittors VALUES('AnalysisRequestTest31', 1, 499, 1, 1)",
				"AnalysisRequest, prep for test 31", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, HWCorrelator, RemoteServerID)  VALUES( 631, 631, 'AnalysisRequestTest31', 1, NULL, 'AnalysisRequestTest31', 'AnalysisRequestTest1', 623)",
				"AnalysisRequest, Add new request, exceed weekly usage maximum (should fail)", false),

	SimpleTests("INSERT INTO Submittors VALUES('AnalysisRequestTest32', 1, 1, 1499, 1)",
				"AnalysisRequest, prep for test 32", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, HWCorrelator, RemoteServerID)  VALUES( 632, 632, 'AnalysisRequestTest32', 1, NULL, 'AnalysisRequestTest32', 'AnalysisRequestTest1', 623)",
				"AnalysisRequest, Add new request, exceed monthly usage maximum (should fail)", false),

	SimpleTests("INSERT INTO Submittors VALUES('AnalysisRequestTest33', 1, 1, 1, 14999)",
				"AnalysisRequest, prep for test 33", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, HWCorrelator, RemoteServerID)  VALUES( 630, 630, 'AnalysisRequestTest33', 1, NULL, 'AnalysisRequestTest33', 'AnalysisRequestTest1', 623)",
				"AnalysisRequest, Add new request, exceed daily usage maximum (should fail)", false),
*/


	SimpleTests("INSERT INTO Submittors VALUES('AnalysisRequestTest40', 1, 1, 1, 1)",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID)  VALUES( 640, 640, 'AnalysisRequestTest40', 1, NULL, 'AnalysisRequestTest40', 623)",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 640, 1, 'AnalysisRequestTest40-1')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 640, 2, 'AnalysisRequestTest40-2')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 640, 3, 'AnalysisRequestTest40-3')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 640, 4, 'AnalysisRequestTest40-4')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 640, 5, 'AnalysisRequestTest40-5')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 640, 6, 'AnalysisRequestTest40-6')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 640, 7, 'AnalysisRequestTest40-7')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 640, 8, 'AnalysisRequestTest40-8')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 640, 9, 'AnalysisRequestTest40-9')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO AnalysisRequest (LocalCookie, RemoteCookie, CheckSum, AnalysisStateID, Informed, SubmittorID, RemoteServerID) VALUES( 740, 740, 'AnalysisRequestTest40b', 1, NULL, 'AnalysisRequestTest40', 623)",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("INSERT INTO Attributes VALUES( 740, 1, 'AnalysisRequestTest40-1')",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("DELETE FROM AnalysisRequest WHERE LocalCookie = 640",
				"AnalysisRequest, prep for test 40", true),
	SimpleTests("Update Attributes SET Value = 'Should be gone' WHERE LocalCookie = 640 AND AttributeKey = 1",
				"AnalysisRequest, update an attribute that should have been deleted (should fail)", false),
	SimpleTests("Update Attributes SET Value = 'Should be gone' WHERE LocalCookie = 640 AND AttributeKey = 7",
				"AnalysisRequest, update an attribute that should have been deleted, (should fail)", false),
	SimpleTests("Update Attributes SET Value = 'Should still be there' WHERE LocalCookie = 740 AND AttributeKey = 1",
				"AnalysisRequest, update an attribute that should still be there", true),

	SimpleTests("", "", true)	// Array terminator
};

static SimpleTests SimpleAnalysisRequestCleanUp[] =
{
	SimpleTests("DELETE FROM AnalysisRequest WHERE LocalCookie > 100",
				"Clean up from AnalysisRequest Simple Tests", true),
	SimpleTests("DELETE FROM Submittors WHERE SubmittorID LIKE 'AnalysisRequestTest%'",
				"Clean up from AnalysisRequest Simple Tests", true),

	SimpleTests("", "", true)	// Array terminator
};

static void ReportError(Reporter& reporter, SQLStmt& sql, string& title)
{
	reporter.Failure(title);

	string message;
	sql.GetErrorMessage(message);
	reporter.Failure(message);
}

static bool GetAnalysisRequestValues(SQLStmt& sql, Reporter& reporter, short& priority,
									 DateTime& imported, DateTime& informed,
									 DateTime& lastStateChange, char& notifyClient,
									 char& notifySymantec)
{
	SQLINTEGER	priorityRetLength;
	SQLINTEGER	importedRetLength;
	SQLINTEGER	informedRetLength;
	SQLINTEGER	lastStateChangeRetLength;
	SQLINTEGER	notifyClientRetLength;
	SQLINTEGER	notifySymantecRetLength;

	bool		rc1, rc2, rc3, rc4, rc5, rc6, rc7, rc8;

	if ((rc1 = SQLStmt::Okay == sql.ExecDirect(string("SELECT Priority, Imported, Informed, LastStateChange, NotifyClient, NotifySymantec FROM AnalysisRequest WHERE LocalCookie = 3001"), true, false)) &&
		(rc2 = SQLStmt::Okay == sql.BindColumn(1, &priority, &priorityRetLength)) &&
		(rc3 = SQLStmt::Okay == sql.BindColumn(2, imported, &importedRetLength)) &&
		(rc4 = SQLStmt::Okay == sql.BindColumn(3, informed, &informedRetLength)) &&
		(rc5 = SQLStmt::Okay == sql.BindColumn(4, lastStateChange, &lastStateChangeRetLength)) &&
		(rc6 = SQLStmt::Okay == sql.BindColumn(5, &notifyClient, &notifyClientRetLength)) &&
		(rc7 = SQLStmt::Okay == sql.BindColumn(6, &notifySymantec, &notifySymantecRetLength))	&&
		(rc8 = SQLStmt::Okay == sql.Fetch(true, false)))
	{
		imported.Null(-1 == importedRetLength);
		informed.Null(-1 == informedRetLength);
		lastStateChange.Null(-1 == lastStateChangeRetLength);

		return true;
	}
	else
		return false;
}


bool DatabaseTests::AnalysisRequest(DBSession& connection, Reporter& reporter)
{
	SQLStmt	sql(connection, "AnalysisRequest", exceptType);
	bool	rc = RunSimpleTests(sql, reporter, SimpleAnalysisRequestTests);

	//
	//	Insert a record with Priority, Imported, Informed, LastStateChange, NotifyClient &
	//	NotifySymantec not defined and ensure that defaults are used.

	if (sql.ExecDirect(string("INSERT INTO AnalysisRequest (LocalCookie, CheckSum, AnalysisStateID, SubmittorID) VALUES (3001, 'AnalysisRequestAdvancedTest1', 1, 'AnalysisRequestTest1')"), true, false))
	{
		short		priority;
		DateTime	imported;
		DateTime	informed;
		DateTime	lastStateChange;
		char		notifyClient;
		char		notifySymantec;

		if (GetAnalysisRequestValues(sql, reporter, priority,
									 imported, informed,
									 lastStateChange, notifyClient,
 									 notifySymantec))
		{
			DateTime	currentTime(DateTime::Now());


			if (0 != priority)
			{
				rc = false;
				reporter.Failure(string("Failure: AnalysisRequest advanced test 1, priority not set to proper default"));
			}
			if (currentTime - imported > 2)
			{
				rc = false;
				reporter.Failure(string("Failure: AnalysisRequest advanced test 1, imported not set to proper default"));
			}
			if (!informed.IsNull())
			{
				rc = false;
				reporter.Failure(string("Failure: AnalysisRequest advanced test 1, informed not set to proper default"));
			}
			if (currentTime - lastStateChange > 2)
			{
				rc = false;
				reporter.Failure(string("Failure: AnalysisRequest advanced test 1, lastStateChange not set to proper default"));
			}
			if (notifyClient != 0x00)
			{
				rc = false;
				reporter.Failure(string("Failure: AnalysisRequest advanced test 1, notifyClient not set to proper default"));
			}
			if (notifySymantec != 0x01)
			{
				rc = false;
				reporter.Failure(string("Failure: AnalysisRequest advanced test 1, notifySymantec not set to proper default"));
			}
		}
		else
		{
			rc = false;

			ReportError(reporter, sql, string("Failure: Unable to get values from database as part of AnalysisRequest advance test 1"));
		}
	}
	else
	{
		rc = false;

		ReportError(reporter, sql, string("Failure: Unable to insert AnalysisRequest record as part of AnalysisRequest advanced test 1"));
	}

	sql.FreeCursor();

	//
	//	Update AnalysisStateID and ensure that LastStateChange, NotifyClient, &
	//	NotifySymantec are properly updated.

	if (sql.ExecDirect(string("UPDATE AnalysisRequest SET NotifyClient = X'00', NotifySymantec = X'01' WHERE LocalCookie = 3001"), true, false) &&
		sql.ExecDirect(string("UPDATE AnalysisRequest SET AnalysisStateID = 32 WHERE LocalCookie = 3001"), true, false))
	{
		short		priority;
		DateTime	imported;
		DateTime	informed;
		DateTime	lastStateChange;
		char		notifyClient;
		char		notifySymantec;

		if (GetAnalysisRequestValues(sql, reporter, priority,
									 imported, informed,
									 lastStateChange, notifyClient,
 									 notifySymantec))
		{
			if (notifyClient != 0x00)
			{
				rc = false;
				reporter.Failure(string("Failure: AnalysisRequest advanced test 1, notifyClient not set to proper default"));
			}
			if (notifySymantec != 0x01)
			{
				rc = false;
				reporter.Failure(string("Failure: AnalysisRequest advanced test 1, notifySymantec not set to proper default"));
			}			
		}
		else
		{
			rc = false;

			ReportError(reporter, sql, string("Failure: Unable to get values from database as part of AnalysisRequest advance test 2"));
		}
	}
	else
	{
		rc = false;

		ReportError(reporter, sql, string("Failure: Unable to update AnalysisRequest record as part of AnalysisRequest advanced test 2"));
	}

	sql.FreeCursor();

	RunSimpleTests(sql, reporter, SimpleAnalysisRequestCleanUp);

	return rc;
}

//
//	AdditionalTests
//
//	This is where I envisioned putting cascading and other tests.  However,
//	I ended up putting these testing in with each of the tables.  I'm
//	keeping this method however just in case I change my mind later.


bool DatabaseTests::AdditionalTests(DBSession& connection, Reporter& reporter)
{
	bool	rc = true;

	return rc;
}



//
//	1	Get the current time
//		Create a DateTime object
//		Get the HTTP and DB string versions
//		Create DateTime objects with each string
//		Get the number of UNIX seconds from each
//		Make sure the number of seconds match

static bool DateTimeTest1(Reporter& reporter)
{
	bool		rc = false;
	time_t		now	= time(NULL);
	DateTime	Now(now);
	string	http1, db1;
	
	if (Now.AsHttpString(http1) && Now.AsDBString(db1))
	{
		DateTime	fromHttp(http1), fromDB(db1);

		if (now == Now.UnixSeconds() && now == fromHttp.UnixSeconds()	&&
			now == fromDB.UnixSeconds())
			rc = true;
	}

	if (!rc)
		reporter.Failure(string("Failure: DateTimeTest1"));

	return rc;
}

//	2	Create DateTime objects using HTTP & DB strings
//		Ask for the HTTP and DB strings back from each.
//		Make sure you get what you expect.

static bool DateTimeTest2(Reporter& reporter)
{
	bool		rc	= false;

	string	httpStr("Mon, 14 Sep 1998 18:15:58 GMT");
	if (_daylight)
		httpStr = "Mon, 14 Sep 1998 17:15:58 GMT";

	string	dbStr("1998-09-14-13.15.58");
	DateTime	http(httpStr);
	DateTime	db(dbStr);

	string	httpStrBack, dbStrBack;

	if (http.AsHttpString(httpStrBack) && db.AsDBString(dbStrBack))
	{
		if (httpStr == httpStrBack && dbStr == dbStrBack)
			rc = true;
	}

	if (!rc)
		reporter.Failure(string("Failure: DateTimeTest2"));

	return rc;
}

//	3	Create DateTime object using a fixed date.
//		Get the HTTP and DB strings back.
//		Ensure you get what you expect.
//

static bool DateTimeTest3(Reporter& reporter)
{
	bool	rc	= false;

	string	httpStr("Mon, 14 Sep 1998 18:15:58 GMT");
	if (_daylight)
		httpStr = "Mon, 14 Sep 1998 17:15:58 GMT";

	string	dbStr("1998-09-14-13.15.58");	// time shift of 5 hours
	DateTime	dtHttp(httpStr);
	DateTime	dtDB(dbStr);
	string	httpBack, dbBack;

	if (dtHttp.AsDBString(dbBack) && dtDB.AsHttpString(httpBack))
	{
		if (dbBack == dbStr && httpBack == httpStr)
			rc = true;
	}

	if (!rc)
		reporter.Failure(string("Failure: DateTimeTest3"));

	return rc;
}

//
//	Test the compare functions
static bool DateTimeTest4(Reporter& reporter)
{
	DateTime	time0(1998, 9, 15,  9, 55, 00);
	DateTime	time1(1998, 9, 15, 10, 00, 00);
	DateTime	time2(1998, 9, 15, 10, 05, 00);
	DateTime	time3(1998, 9, 15, 10, 05, 00);
	bool		rc, rc1, rc2, rc3, rc4, rc5, rc6, rc7, rc8, rc9, rc10, rc11;

	rc1 = time0 < time1;
	rc2 = !(time0 > time1);
	rc3 = !(time0 == time1);
	rc4 = time0 <= time1;
	rc5 = !(time0 >= time1);
	rc6 = time2 == time3;

	rc7 = !(time2 < time1);
	rc8 = time2 > time1;
	rc9 = !(time2 == time1);
	rc10= !(time2 <= time1);
	rc11= time2 >= time1;

	rc = rc1 && rc2 && rc3 && rc4 && rc5 && rc6 && rc7 && rc8 && rc9 &&
		   rc10 && rc11;

	if (!rc)
		reporter.Failure(string("Failure: DateTimeTest4"));

	return rc;
}


//
//	Test the arithmetic functions
static bool DateTimeTest5(Reporter& reporter)
{
	int			fiveMinutes	= 300;	// 300 seconds
	bool		rc, rc1, rc2, rc3, rc4, rc5, rc6;

	DateTime	time0(1998, 9, 15,  9, 55, 00);
	DateTime	time1(1998, 9, 15, 10, 00, 00);
	DateTime	time2(1998, 9, 15, 10, 05, 00);

	rc1 = (time1 - fiveMinutes) == time0;
	rc2 = (time0 + 2*fiveMinutes) == time2;


	DateTime	time3(1998, 9, 30, 23, 59, 59);
	DateTime	time4(1998, 10, 1, 00, 00, 01);
	DateTime	time5(1998, 9, 30, 23, 59, 59);

	rc3 = (time3 + 2) == time4;
	rc4 = (time3 - 2) == time5;

	DateTime	time6(1998, 12, 31, 23, 59, 59);
	DateTime	time7(1999,  1,  1, 00, 00, 01);
	DateTime	time8(1998, 12, 31, 23, 59, 59);

	rc5 = (time6 + 2) == time7;
	rc6 = (time6 - 2) == time8;

	rc = rc1 && rc2 && rc3 && rc4 && rc5 && rc6;

	if (!rc)
		reporter.Failure(string("Failure: DateTimeTest5"));

	return rc;
}

//
//	Add tests of the DateTime class +,-, == operators

//
//	Test the DateTime class
bool DatabaseTests::DateTimeClassTests(Reporter& reporter)
{
	return DateTimeTest1(reporter) && DateTimeTest2(reporter) &&
		   DateTimeTest3(reporter) && DateTimeTest4(reporter) && DateTimeTest5(reporter);
}

bool DatabaseTests::AllTests(DBSession& connection, Reporter& reporter)
{
	bool	rcDateTime	= DateTimeClassTests(reporter);
	bool	rc0 = false;
	bool	rc1 = false, rc2 = false, rc3 = false, rc4 = false, rc5 = false;
	bool	rc6 = false, rc7 = false, rc8 = false, rc9 = false, rc10 = false;

	DBService	database(string("AVISdata"));

	if (database.Valid())
	{
		if (connection.Valid())
		{
			rc0	= true;

			rc1 =	AnalysisResults(connection, reporter);
			rc2 =	Submittors(connection, reporter);
			rc3 =	Signatures(connection, reporter);
			rc4 =	Globals(connection, reporter);
			rc5 =	Servers(connection, reporter);
			rc6 =	Attributes(connection, reporter);
			rc7 =	AttributeKeys(connection, reporter);
			rc8 =	AnalysisStateInfo(connection, reporter);
			rc9 =	AnalysisRequest(connection, reporter);
			rc10=	AdditionalTests(connection, reporter);
		}
	}

	return rcDateTime && rc0 && rc1 && rc2 && rc3 && rc4 && rc5 && rc6 && rc7 && rc8 && rc9 && rc10;
}


