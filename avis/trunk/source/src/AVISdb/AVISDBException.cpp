// AVISDBException.cpp: implementation of the AVISDBException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISDBException.h"

#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


char *AVISDBException::typeAsChar[] = {
	"NoDatabase",
	"NoSession",
	"AnalysisRequest",
	"AnalysisResults",
	"AnalysisStateInfo",
	"Attributes",
	"CheckSum",
	"DBServer",
	"DBService",
	"DBSession",
	"Globals",
	"Internal",
	"Server",
	"Signature",
	"SigsToBeExported",
	"SQLStmt",
	"DatabaseClassesTest",
	"DatabaseTests"
};


AVISDBException::AVISDBException(TypeOfException t, std::string& di) :
					type(t), detailedInfo(di)
{
//	std::cerr << "AVISdbException [" << TypeAsString();
//	std::cerr << "] thrown, detailed info to follow." << std::endl;
//	std::cerr << "[ " << di << " ]" << std::endl;
}

AVISDBException::AVISDBException(TypeOfException t, const char* di) :
					type(t), detailedInfo(di)
{
//	std::cerr << "AVISdbException [" << TypeAsString();
//	std::cerr << "] thrown, detailed info to follow." << std::endl;
//	std::cerr << "[ " << di << " ]" << std::endl;
}
