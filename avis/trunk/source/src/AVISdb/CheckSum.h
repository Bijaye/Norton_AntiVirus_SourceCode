//
//	IBM AntiVirus Immune System
//
//	File Name:	CheckSum.h
//
//	Author:		Andy Klapper
//
//	This class represents the CheckSum value stored in the database.
//
//	$Log:  $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//


#if !defined(AFX_CHECKSUM_H__4342B4EC_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
#define AFX_CHECKSUM_H__4342B4EC_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>

#include "AVISdbDLL.h"
#include "AVIS.h"

#include "AVISDBException.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISDB_API CheckSum  
{
public:
	CheckSum(std::string& str) : value(str)
	{
		if (str.length() >= CheckSumLength())
			throw AVISDBException(AVISDBException::etCheckSum,
										"str.length() >= CheckSumLength()");
		ToLower(value);
	}

	CheckSum(uchar* str) : value((char*)str)
	{
	}

	virtual ~CheckSum();

	operator std::string()	{ return value; };
	CheckSum& operator=(std::string& newValue)
	{
		if (newValue.length() >= CheckSumLength())
			throw AVISDBException(AVISDBException::etCheckSum,
										"newValue.length() >= CheckSumLength()");

		value = newValue;
		ToLower(value);

		return *this;
	}

	CheckSum& operator=(char *newValue)
	{
		if (NULL == newValue)
			throw AVISDBException(AVISDBException::etCheckSum,
										"NULL == newValue");
		if (strlen(newValue) >= CheckSumLength())
			throw AVISDBException(AVISDBException::etCheckSum,
										"strlen(newValue) >= CheckSumLength()");

		value = newValue;
		ToLower(value);

		return *this;
	}

	bool operator==(CheckSum& rhs)	{ return value == rhs.value; }
	bool operator!=(CheckSum& rhs)	{ return value != rhs.value; };
	bool operator> (CheckSum& rhs)	{ return value >  rhs.value; };
	bool operator< (CheckSum& rhs)	{ return value <  rhs.value; };
	bool operator>=(CheckSum& rhs)	{ return value >= rhs.value; };
	bool operator<=(CheckSum& rhs)	{ return value <= rhs.value; };

	bool operator==(std::string& rhs)	{ return value == rhs; }
	bool operator!=(std::string& rhs)	{ return value != rhs; };
	bool operator> (std::string& rhs)	{ return value >  rhs; };
	bool operator< (std::string& rhs)	{ return value <  rhs; };
	bool operator>=(std::string& rhs)	{ return value >= rhs; };
	bool operator<=(std::string& rhs)	{ return value <= rhs; };

	static ushort CheckSumLength(void)	{ return checkSumLength; };

	enum { MaxLength = 34 };

	std::string	ToDebugString(void);	// returns a string representation of the
										// object suitable for debugging messages

private:
	std::string	value;
	static const ushort	checkSumLength;

public:
	static void ToLower(std::string& str)
	{
		for (int i = 0; &str[i] < str.end(); i++)
		{
			if (isupper(str[i]))
				str[i] = tolower(str[i]);
		}
	}
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_CHECKSUM_H__4342B4EC_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
