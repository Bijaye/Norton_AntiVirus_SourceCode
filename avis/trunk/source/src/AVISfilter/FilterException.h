// FilterException.h: interface for the FilterException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTEREXCEPTION_H__82ED1F1C_E5DA_11D2_AD9E_00A0C9C71BBC__INCLUDED_)
#define AFX_FILTEREXCEPTION_H__82ED1F1C_E5DA_11D2_AD9E_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISFilter.h"

#include "AVISException.h"
#include <string>

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISFILTER_API FilterException : AVISException  
{
public:
	enum TypeOfException	{ 
		Constructor,
		AddToResults,
		AppendData,
		CreateRequest,
		Done,
		ParseOutValues,
		SampleName,
		ContentName
	};

	FilterException(TypeOfException t, std::string& di);
	FilterException(TypeOfException t, const char* di);
	virtual ~FilterException();

	std::string		ClassAsString()	{ return "FilterException"; }
	std::string		DetailedInfo()	{ return detailedInfo; };
	std::string		TypeAsString(void) { return std::string(typeAsChar[type]); };

private:
	TypeOfException	type;
	std::string		detailedInfo;

	static char		*typeAsChar[];
};

#endif // !defined(AFX_FILTEREXCEPTION_H__82ED1F1C_E5DA_11D2_AD9E_00A0C9C71BBC__INCLUDED_)
