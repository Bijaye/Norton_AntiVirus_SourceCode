// AVISFileException.h: interface for the AVISFileException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISFILEEXCEPTION_H__A0537115_E5EC_11D2_AD9E_00A0C9C71BBC__INCLUDED_)
#define AFX_AVISFILEEXCEPTION_H__A0537115_E5EC_11D2_AD9E_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVISException.h>

#include <string>

class AVISFileException  : public AVISException
{
public:
	enum TypeOfException	{ 
								CompoundDataManip,
								NullDataManip,
								ToolFactory
	};

	AVISFileException(TypeOfException t, std::string& di);
	AVISFileException(TypeOfException t, const char* di);

	virtual ~AVISFileException();

	std::string		ClassAsString()	{ return "AVISFileException"; }
	std::string		DetailedInfo()	{ return detailedInfo; };
	std::string		TypeAsString(void) { return std::string(typeAsChar[type]); };

private:
	TypeOfException	type;
	std::string		detailedInfo;

	static char		*typeAsChar[];
};

#endif // !defined(AFX_AVISFILEEXCEPTION_H__A0537115_E5EC_11D2_AD9E_00A0C9C71BBC__INCLUDED_)
