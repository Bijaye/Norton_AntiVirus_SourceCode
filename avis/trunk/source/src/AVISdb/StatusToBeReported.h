// StatusToBeReported.h: interface for the StatusToBeReported class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSTOBEREPORTED_H__53BDD705_E6AF_11D2_AD9F_00A0C9C71BBC__INCLUDED_)
#define AFX_STATUSTOBEREPORTED_H__53BDD705_E6AF_11D2_AD9F_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "AVIS.h"
#include "AVISDBException.h"

#include "Server.h"
#include "Checksum.h"

class AVISDB_API StatusToBeReported  
{
public:
/*
	static bool	AddNew(Server& server, CheckSum& checksum);
	static bool AddForAllActiveServers(CheckSum& checksum);

	explicit StatusToBeReported(Server& server, CheckSum& checksum);
	bool	RemoveFromDatabase(Server& server, CheckSum& checksum);
	std::string	ToDebugString(void);	// returns a string representation of the
										// object suitable for debugging messages
*/

	StatusToBeReported() : isNull(true), checksum(std::string("")), serverID(0) {};
	virtual ~StatusToBeReported();

	static bool GetOldestByServer(StatusToBeReported& stbr, Server& server);

	bool	IsNull(void)		{ return isNull; }
	uint	ServerID(void)		{ NullCheck("ServerID"); return serverID; }
	CheckSum	Checksum(void)	{ NullCheck("Checksum"); return checksum; }

	bool	RemoveFromDatabase(void);

	void	Clear(void)			{ isNull = true; checksum = std::string(""); serverID = 0; }


private:
	bool		isNull;
	CheckSum		checksum;
	uint		serverID;

	static bool Get(std::string& select, StatusToBeReported& stbr,
							const char* where);

	void		NullCheck(const char* where)
	{
		if (isNull)
		{
			std::string	msg(where);
			msg	+= ", StatusToBeReported is null";

			throw AVISDBException(exceptType, msg);
		}
	}

	static const AVISDBException::TypeOfException	exceptType;
};

#endif // !defined(AFX_STATUSTOBEREPORTED_H__53BDD705_E6AF_11D2_AD9F_00A0C9C71BBC__INCLUDED_)
