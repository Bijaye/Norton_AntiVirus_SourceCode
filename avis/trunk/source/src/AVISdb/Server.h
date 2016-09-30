//
//	IBM AntiVirus Immune System
//
//	File Name:	Server.h
//
//	Author:		Andy Klapper
//
//	This class represents the Servers table in the AVIS database.
//
//	$Log:  $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//


#if !defined(AFX_SERVER_H__4342B4E9_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
#define AFX_SERVER_H__4342B4E9_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "AVIS.h"
#include "AVISDBException.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISDB_API Server  
{
public:

	static bool AddNew(std::string& url);

	explicit Server(ushort serverID);
	explicit Server(std::string& url);
	Server(void) : isNull(true), id(0), url("") {};
	virtual ~Server();

	bool		IsNull(void)	{ return isNull; };

	ushort		ID(void)		{ NullCheck("ID");		return id;		}
	std::string	URL(void)		{ NullCheck("URL");		return url;		};
	bool		Active(void)	{ NullCheck("Active");	return active;	};

	bool		URL(std::string& newURL);

	bool		operator==(Server& rhs) {	NullCheck("operator==");
											if (rhs.isNull)
												throw AVISDBException(exceptType,
																		"operator==, rhs.isNull");
											return id == rhs.id; };

	void		Refresh(void)	{	NullCheck("Refresh"); 
									Server	server(id);
									*this = server;
								};

	static bool	ClearAllActive(void);
	bool		SetActive(bool newState);

	std::string	ToDebugString(void);	// returns a string representation of the
										// object suitable for debugging messages

private:
	bool		isNull;
	ushort		id;
	std::string	url;
	bool		active;

	static const ushort	URLLength;
	static const AVISDBException::TypeOfException	exceptType;

	void		NullCheck(const char* where)
	{
		if (IsNull())
		{
			std::string	msg(where);
			msg	+= ", Server is null";
			throw AVISDBException(exceptType, msg);
		}
	}
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_SERVER_H__4342B4E9_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
