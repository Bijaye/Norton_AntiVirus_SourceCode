// Tester.cpp: implementation of the Tester class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DBTester.h"
#include "DatabaseTests.h"
#include "Reporter.h"
#include "DatabaseClassesTest.h"
#include "Signature.h"
#include "AnalysisResults.h"
#include "Submittor.h"
#include "Server.h"
#include "AnalysisStateInfo.h"
#include "Attributes.h"
#include "AnalysisRequest.h"

bool DBTester::RunTests(Reporter& reporter)
{
	bool	rc1 = DatabaseClassesTest::TestDatabaseClasses(reporter);

	DBService	db(std::string("AVISdata"));
	DBSession	connection;

	bool	rc2 = DatabaseTests::AllTests(connection, reporter);

	return rc1 && rc2;
}
