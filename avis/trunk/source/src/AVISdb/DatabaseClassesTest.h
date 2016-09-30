//
//	IBM AntiVirus Immune System
//
//	File Name:	DatabaseClassesTest.h
//
//	Author:		Andy Klapper
//
//	This class tests the database access classes that hide the use
//	of ODBC or DB2's CLI APIs from the higher level table abstraction
//	classes.
//
//	$Log:  $
//	
//

#if !defined(AFX_DATABASECLASSESTEST_H__B7AEEB33_4D86_11D2_BE8E_0004ACEC6C96__INCLUDED_)
#define AFX_DATABASECLASSESTEST_H__B7AEEB33_4D86_11D2_BE8E_0004ACEC6C96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Reporter.h"

class DatabaseClassesTest  
{
public:
	static	bool	DBServiceTest(Reporter& reporter);
	static	bool	DBSessionTest(Reporter& reporter);
	static	bool	SQLStmtTest(Reporter& reporter);

	static	bool	TestDatabaseClasses(Reporter& reporter);

private:
	static	bool	DBServiceTest1(Reporter& reporter);
	static	bool	DBServiceTest2(Reporter& reporter);
	static	bool	DBServiceTest3(Reporter& reporter);
	static	bool	DBServiceTest4(Reporter& reporter);
	static	bool	DBServiceTest5(Reporter& reporter);

	DatabaseClassesTest();
	virtual ~DatabaseClassesTest();

};

#endif // !defined(AFX_DATABASECLASSESTEST_H__B7AEEB33_4D86_11D2_BE8E_0004ACEC6C96__INCLUDED_)
