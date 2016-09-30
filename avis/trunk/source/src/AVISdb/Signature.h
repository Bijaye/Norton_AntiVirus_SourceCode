//
//	IBM AntiVirus Immune System
//
//	File Name:	Signature.h
//
//	Author:		Andy Klapper
//
//	This class represents the Signatures table
//	in the AVIS database.
//
//	$Log:  $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//

#if !defined(AFX_SIGNATURE_H__02CF404D_54DA_11D2_88FB_00A0C9DB9E9C__INCLUDED_)
#define AFX_SIGNATURE_H__02CF404D_54DA_11D2_88FB_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "AVIS.h"
#include "AVISDBException.h"
#include "DateTime.h"


class AVISDB_API Signature  
{
public:

	//
	//	Add a new signature record to the database
	//	DateTime fields that are invalid will be inserted as nulls.
	static	bool	AddNew(uint sigSeqNum, DateTime& Blessed,
							DateTime& Produced, DateTime& Published,
							std::string& checkSum);
	
	//
	//	Add a new signature record to the database or replace an
	//	existing signature record in the database.
	//	DateTime fields that are invalid will be inserted as nulls
	static	bool	AddNewOrReplaceExisting(
							uint sigSeqNum, DateTime& Blessed,
							DateTime& Produced, DateTime& Published,
							std::string& checkSum,
							bool& replacedOld);

	//
	//	Get the latest/greatest signature (the most recently produced
	//	signature record)
	static	bool	Latest(Signature& sig);

	//
	//	Get the latest/greatest blessed signature (the most recently
	//	blessed signature record)
	static	bool	LatestBlessed(Signature& sig);

	static	bool	FromSequenceNumber(uint seqNum, Signature& sig)
	{
		sig = Signature(seqNum);
		return !sig.IsNull();
	}

	//
	//	produce a null signature object
	Signature();

	//
	//	Get a Signature object based on signature seq number
	Signature(uint sigSeqNum);

	//
	//	Deconstruct a signature object
	virtual ~Signature();

	bool	IsNull(void) { return null; }	// NOTE: a null signature fires
											//		 an assertion when used
											//		 with any non-static method!
											//		 (and the IsNull method)
	bool	IsBlessed(void)	{ NullCheck("IsBlessed");
								return !blessed.IsNull(); }

	//
	//	Comparison operators
	bool	operator< (Signature& rhs) { NullCheck("operator<"); return seqNum < rhs.seqNum; };
	bool	operator> (Signature& rhs) { NullCheck("operator>"); return seqNum > rhs.seqNum; };
	bool	operator==(Signature& rhs) { NullCheck("operator=="); return seqNum == rhs.seqNum; };
	bool	operator!=(Signature& rhs) { NullCheck("operator!="); return seqNum != rhs.seqNum; };
	bool	operator>=(Signature& rhs) { NullCheck("operator>"); return seqNum >= rhs.seqNum; };
	bool	operator<=(Signature& rhs) { NullCheck("operator<="); return seqNum <= rhs.seqNum; };

	//
	//	Access methods
	DateTime	Blessed(void)		{ NullCheck("Blessed"); return blessed;	};
	DateTime	Produced(void)		{ NullCheck("Produced"); return produced;	};
	DateTime	Published(void)		{ NullCheck("Published"); return published;	};
	uint		SequenceNumber(void){ NullCheck("SequenceNumber"); return seqNum;		};
	std::string	CheckSum(void)		{ NullCheck("CheckSum"); return checkSum;	};

	//
	//	Modify methods
	bool	Blessed(DateTime& when);

	void	Refresh(void)			{ Signature	sig(seqNum); *this = sig; };

	std::string	ToDebugString(void);	// returns a string representation of the
										// object suitable for debugging messages

private:
	bool		null;
	uint		seqNum;
	DateTime	blessed;
	DateTime	produced;
	DateTime	published;
	std::string	checkSum;

	static bool Get(std::string& select, Signature& sig, const char* where);

	void		NullCheck(const char* where)
	{
		if (IsNull())
		{
			std::string	msg(where);
			msg	+= ", Signature is null";

			throw AVISDBException(exceptType, msg);
		}
	}

	static const AVISDBException::TypeOfException	exceptType;
};

#endif // !defined(AFX_SIGNATURE_H__02CF404D_54DA_11D2_88FB_00A0C9DB9E9C__INCLUDED_)
