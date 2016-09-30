//
//	IBM AntiVirus Immune System
//
//	File Name:	Tester.h
//
//	Author:		Andy Klapper
//
//	This class provides an interface between the world outside of 
//	the AVISDB.dll and the internal tests that can be run on the dll.
//
//	NOTE: These tests are destructive on the database and are ment to
//	test the database triggers, constraints and structure as well as the
//	code that is used to access the database.  This is not a validity
//	check of the database.
//
//	NOTE: This is a singleton class.
//
//	$Log:  $
//	
//

#if !defined(AFX_DBTESTER_H__B67B75D3_4D96_11D2_BE8F_0004ACEC6C96__INCLUDED_)
#define AFX_DBTESTER_H__B67B75D3_4D96_11D2_BE8F_0004ACEC6C96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "Reporter.h"

class AVISDB_API DBTester  
{
public:
	static bool RunTests(Reporter& reporter);

private:
	DBTester()			{};
	virtual ~DBTester()	{};

};

#endif // !defined(AFX_DBTESTER_H__B67B75D3_4D96_11D2_BE8F_0004ACEC6C96__INCLUDED_)
