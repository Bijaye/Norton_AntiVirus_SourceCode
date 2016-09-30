//
//	IBM AntiVirus Immune System
//
//	File Name:	AVISDBException.h
//
//	Author:		Andy Klapper
//
//	Objects of type AVISDBException are thrown by the AVISDB.dll when
//	unthinkable problems occur within the dll that could not possibly be
//	anticipated.
//
//	$Log:  $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//


#if !defined(AFX_AVISDBEXCEPTION_H__4C842327_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_)
#define AFX_AVISDBEXCEPTION_H__4C842327_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
									// WARNING: Warning disabled!

#pragma	warning( disable: 4251 )	// warns that std::string needs to be exported
									// to dll users (which would be true if it was
									// an internal class)

#include <AVISException.h>

class AVISDB_API AVISDBException : public AVISException
{
public:
	enum TypeOfException	{ NoDatabase,
							  NoSession,

							  etAnalysisRequest,
							  etAnalysisResults,
							  etAnalysisStateInfo,
							  etAttributes,
							  etCheckSum,
							  etDBServer,
							  etDBService,
							  etDBSession,
							  etGlobals,
							  etInternal,
							  etServer,
							  etSignature,
							  etSigsToBeExported,
							  etSQLStmt,
							  etSubmittor,
							  etDatabaseClassesTest,
							  etDatabaseTests,
							  etStatusToBeReported

							};

	AVISDBException(TypeOfException t, std::string& di);
	AVISDBException(TypeOfException t, const char* di);

	explicit AVISDBException(TypeOfException t) :
					type(t), detailedInfo(typeAsChar[t])
					{};

	~AVISDBException() {};

	TypeOfException	Type(void)			{ return type; };
	std::string		DetailedInfo(void)	{ return detailedInfo; };
	std::string		TypeAsString(void)	{ return std::string(typeAsChar[type]); };

	std::string		ClassAsString(void)	{ return std::string("AVISDBException"); };

private:
	TypeOfException	type;
	std::string		detailedInfo;

	static char		*typeAsChar[];
};

#pragma warning( default: 4251 )	// try to limit the scope of this a little bit

#endif // !defined(AFX_AVISDBEXCEPTION_H__4C842327_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_)
