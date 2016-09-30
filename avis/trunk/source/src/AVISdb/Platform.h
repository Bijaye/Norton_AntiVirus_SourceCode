//
//	IBM AntiVirus Immune System
//
//	File Name:	Platform.h
//
//	Author:		Andy Klapper
//
//	This class represents the Platforms table in the AVIS database.
//
//	$Log: $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//

#if !defined(AFX_PLATFORM_H__4342B4E8_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
#define AFX_PLATFORM_H__4342B4E8_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "AVIS.h"
#include "AVISDBException.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISDB_API Platform  
{
public:
	static bool	AddNew(std::string& id);

	explicit Platform(std::string& id);
	virtual ~Platform();

	bool	IsNull(void)		{ return isNull; };
	std::string	ID(void)		{ NullCheck(AVISDBException::PlatformID); return hwCorrelator; };
	operator std::string()		{ NullCheck(AVISDBException::PlatformOpStr); return hwCorrelator; };
	uint	UsageToday(void)	{ NullCheck(AVISDBException::PlatformUsageToday); return usageToday; };
	uint	UsageThisWeek(void)	{ NullCheck(AVISDBException::PlatformUsageThisWeek); return usageThisWeek; };
	uint	UsageThisMonth(void){ NullCheck(AVISDBException::PlatformUsageThisMonth); return usageThisMonth; };
	uint	UsageThisYear(void)	{ NullCheck(AVISDBException::PlatformUsageThisYear); return usageThisYear; };

	bool	operator==(Platform& rhs) { NullCheck(AVISDBException::PlatformOpEq);
										if (rhs.isNull)
											throw new AVISDBException(AVISDBException::PlatformOpEq,
																		"rhs.isNull");
										 return hwCorrelator == rhs.hwCorrelator; };
	bool	operator!=(Platform& rhs)	{ return !(*this == rhs); };

	void	Refresh(void)		{ Platform	sub(hwCorrelator); *this = sub; };

	enum	{ MaxLength = 34 };

private:
	bool		isNull;
	std::string	hwCorrelator;
	uint		usageToday;
	uint		usageThisWeek;
	uint		usageThisMonth;
	uint		usageThisYear;


	void		NullCheck(AVISDBException::TypeOfException type)
	{
		if (IsNull())
			throw new AVISDBException(type, "Platform is null");
	}
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_PLATFORM_H__4342B4E8_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
