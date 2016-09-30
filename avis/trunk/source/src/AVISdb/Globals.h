//
//	IBM AntiVirus Immune System
//
//	File Name:	Globals.h
//
//	Author:		Andy Klapper
//
//	This class represents the Globals table in the AVIS database.
//
//	$Log:  $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//

#if !defined(AFX_GLOBALS_H__D09DCE33_588A_11D2_BE93_0004ACEC6C96__INCLUDED_)
#define AFX_GLOBALS_H__D09DCE33_588A_11D2_BE93_0004ACEC6C96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Server.h"
#include "Signature.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISDB_API Globals  
{
public:
	enum GatewayType { Unknown = -1, Internet = 0, AnalysisFront = 1, AnalysisBack = 2 , CustomerAdmin = 3 };

	static bool	ThisServer(Server& thisServer);
	static bool	Type(GatewayType& type);
	static bool	NextLocalCookie(uint& nextLocalCookie);
	static bool	NewestSignature(Signature& sig);
	static bool	NewestBlessedSignature(Signature& sig);
	static bool	DatabaseVersion(ushort& version);
	static bool	BackOfficeLock(bool& locked);

	static bool SetThisServer(Server& setThisServer);
	static bool SetGatewayType(GatewayType& setGatewayType);
	static bool SetBackOfficeLock(bool& locked);

	std::string	ToDebugString(void);	// returns a string representation of the
										// object suitable for debugging messages

private:
	Globals() {};
	virtual ~Globals() {};

	static GatewayType	gatewayType;
	static Server		thisServer;
	static ushort		version;

	static const AVISDBException::TypeOfException	exceptType;
};

#endif // !defined(AFX_GLOBALS_H__D09DCE33_588A_11D2_BE93_0004ACEC6C96__INCLUDED_)
