// SigsToBeExported.h: interface for the SigsToBeExported class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIGSTOBEEXPORTED_H__53BDD705_E6AF_11D2_AD9F_00A0C9C71BBC__INCLUDED_)
#define AFX_SIGSTOBEEXPORTED_H__53BDD705_E6AF_11D2_AD9F_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "AVIS.h"
#include "AVISDBException.h"

#include "Server.h"
#include "Signature.h"

class AVISDB_API SigsToBeExported  
{
public:
	static bool	AddNew(Server& server, Signature& sig);
	static bool AddForAllActiveServers(Signature& sig);

	explicit SigsToBeExported(Server& server, Signature& sig);
	         SigsToBeExported() : isNull(true), sigID(0), serverID(0) {};
	virtual ~SigsToBeExported();

	static bool GetOldest(SigsToBeExported& stbe);
	static bool GetOldestByServer(SigsToBeExported& stbe, Server& server);

	bool	IsNull(void)		{ return isNull; }
	uint	ServerID(void)		{ NullCheck("ServerID"); return serverID; }
	uint	SignatureID(void)	{ NullCheck("SignatureID"); return sigID; }

	bool	RemoveFromDatabase(void);

	void	Clear(void)			{ isNull = true; sigID = 0; serverID = 0; }

	std::string	ToDebugString(void);	// returns a string representation of the
										// object suitable for debugging messages

private:
	bool		isNull;
	uint		sigID;
	uint		serverID;

	static bool Get(std::string& select, SigsToBeExported& stbe,
							const char* where);

	void		NullCheck(const char* where)
	{
		if (isNull)
		{
			std::string	msg(where);
			msg	+= ", SigsToBeExported is null";

			throw AVISDBException(exceptType, msg);
		}
	}

	static const AVISDBException::TypeOfException	exceptType;
};

#endif // !defined(AFX_SIGSTOBEEXPORTED_H__53BDD705_E6AF_11D2_AD9F_00A0C9C71BBC__INCLUDED_)
