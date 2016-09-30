//
//	IBM AntiVirus Immune System
//
//	File Name:	DatabaseTests.h
//
//	Author:		Andy Klapper
//
//
//	This class tests the database itself.  Another class will be created
//	to test the classes that most users will use to access the database.
//
//	$Log:  $
//	
//

#if !defined(AFX_DATABASETESTS_H__CCC826E7_3C2C_11D2_88B2_00A0C9DB9E9C__INCLUDED_)
#define AFX_DATABASETESTS_H__CCC826E7_3C2C_11D2_88B2_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "DBSession.h"
#include "Reporter.h"


class DatabaseTests  
{
public:
	static	bool	AnalysisResults(DBSession& connection, Reporter& rep);
	static	bool	Submittors(DBSession& connection, Reporter& rep);
	static	bool	Signatures(DBSession& connection, Reporter& rep);
	static	bool	Globals(DBSession& connection, Reporter& rep);
	static	bool	Servers(DBSession& connection, Reporter& rep);
	static	bool	Attributes(DBSession& connection, Reporter& rep);
	static	bool	AttributeKeys(DBSession& connection, Reporter& rep);
	static	bool	AnalysisStateInfo(DBSession& connection, Reporter& rep);
	static	bool	AnalysisRequest(DBSession& connection, Reporter& rep);

	static	bool	AdditionalTests(DBSession& connection, Reporter& rep);

	static	bool	DateTimeClassTests(Reporter& rep);

	static	bool	AllTests(DBSession& connection, Reporter& rep);

private:
	DatabaseTests();
	virtual ~DatabaseTests();

};


#endif // !defined(AFX_DATABASETESTS_H__CCC826E7_3C2C_11D2_88B2_00A0C9DB9E9C__INCLUDED_)
