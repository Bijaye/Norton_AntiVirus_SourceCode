//
//	IBM AntiVirus Immune System
//
//	File Name:	Submittor.h
//
//	Author:		Andy Klapper
//
//	This class represents the Submittors table in the AVIS database.
//
//	$Log: $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//

#if !defined(AFX_SUBMITTOR_H__4342B4E8_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
#define AFX_SUBMITTOR_H__4342B4E8_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "AVIS.h"
#include "AVISDBException.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISDB_API Submittor  
{
public:
	static bool	AddNew(std::string& id);

	explicit Submittor(std::string& id);
	virtual ~Submittor();

	bool	IsNull(void)		{ return isNull; };
	std::string	ID(void)		{ NullCheck("ID"); return submittorID; };
	operator std::string()		{ NullCheck("operator std::string()"); return submittorID; };
	uint	UsageToday(void)	{ NullCheck("UsageToday"); return usageToday; };
	uint	UsageThisWeek(void)	{ NullCheck("UsageThisWeek"); return usageThisWeek; };
	uint	UsageThisMonth(void){ NullCheck("UsageThisMonth"); return usageThisMonth; };
	uint	UsageThisYear(void)	{ NullCheck("UsageThisYear"); return usageThisYear; };

	bool	operator==(Submittor& rhs) { NullCheck("operator==(Submittor& rhs)");
										 if (rhs.isNull)
											 throw AVISDBException(exceptType,
																		"operator==(Submittor& rhs), rhs.isNull");
										 return submittorID == rhs.submittorID; };
	bool	operator!=(Submittor& rhs)	{ return !(*this == rhs); };

	void	Refresh(void)		{ Submittor	sub(submittorID); *this = sub; };

	enum { MaxLength = 64 };

	std::string	ToDebugString(void);	// returns a string representation of the
										// object suitable for debugging messages

private:
	bool		isNull;
	std::string	submittorID;
	uint		usageToday;
	uint		usageThisWeek;
	uint		usageThisMonth;
	uint		usageThisYear;

	void		NullCheck(const char* where)
	{
		if (IsNull())
		{
			std::string	msg(where);
			msg	+= ", Submittor is null";
			throw AVISDBException(exceptType, msg);
		}
	}

	static const AVISDBException::TypeOfException	exceptType;
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_SUBMITTOR_H__4342B4E8_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
