//
//	IBM AntiVirus Immune System
//
//	File Name:	DateTimeException.h
//
//	Author:		Andy Klapper
//
//	Objects of type DateTimeException are thrown by the AVISDB.dll when
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


#if !defined(AFX_DateTimeException_H__4C842327_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_)
#define AFX_DateTimeException_H__4C842327_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISCommon.h"
#include "AVISException.h"
#include <string>
									// WARNING: Warning disabled!

#pragma	warning( disable: 4251 )	// warns that std::string needs to be exported
									// to dll users (which would be true if it was
									// an internal class)

class AVISCOMMON_API DateTimeException : public AVISException
{
public:
	enum TypeOfException	{
		DateTimeStr,
		DateTimeOpAssgnTm,
		DateTimeParts,
		DateTimeMonthIntToStr,
		DateTimeMonthStrToInt
							};

	DateTimeException(TypeOfException t, std::string& di);
	DateTimeException(TypeOfException t, const char* di);

	explicit DateTimeException(TypeOfException t) :
					type(t), detailedInfo(typeAsChar[t])
					{};

	~DateTimeException() {};

	TypeOfException	Type(void)		{ return type; };
	std::string		ClassAsString()	{ return "DateTimeException"; };
	std::string		DetailedInfo()	{ return detailedInfo; };
	std::string		TypeAsString(void) { return std::string(typeAsChar[type]); };

private:
	TypeOfException	type;
	std::string		detailedInfo;

	static char		*typeAsChar[];
};

#pragma warning( default: 4251 )	// try to limit the scope of this a little bit

#endif // !defined(AFX_DateTimeException_H__4C842327_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_)
