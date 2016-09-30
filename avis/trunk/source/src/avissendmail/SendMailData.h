// SendMailData.h: interface for the SendMailData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDMAILDATA_H__A004DADB_E6E1_11D2_9134_0004ACEC70EC__INCLUDED_)
#define AFX_SENDMAILDATA_H__A004DADB_E6E1_11D2_9134_0004ACEC70EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4251)

#include "AVISSendMailDLL.h"

// This class is exported from the AVISSendMail.dll

//////////////////////////////////////////////////////////////////////////////
//
// Class Name: SendMailData  
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
class AVISSENDMAIL_API SendMailData  
{

public:

	// constructor/destructor
	SendMailData( );

	virtual ~SendMailData( );

	//
	// modification methods
	//
	void SetLogging( bool bLogging );

	void SetHost( std::string &strHost );

	void SetSenderUserID( std::string &strSenderUserID );

	void SetSenderDisplayName( std::string &strSenderDisplayName );

	void SetRecipientUserID( std::string &strRecipientUserID );

	void SetRecipientDisplayName( std::string &strRecipientDisplayName );

	void SetReplyToUserID( std::string &strReplyToUserID );

	void SetReplyToName( std::string &strReplyToName );

	void SetMessageID( std::string &strMessageID );

	void SetMessageText( std::string &strMessageText );

	void SetSubject( std::string &strSubjectText );

	//
	// access methds
	//
	std::string GetHost( ) { return m_strHost; }

	std::string GetMessageID( ) { return m_strMessageID; }

	std::string GetMessageText( ) { return m_strMessageText; }

	std::string GetRecipientID( ) { return m_strRecipientID; }

	std::string GetRecipientDisplayName( ) { return m_strRecipientName; }

	std::string GetReplyToUserID( ) { return m_strReplyToID; }

	std::string GetReplyToName( ) { return m_strReplyToName; }

	std::string GetSenderUserID( ) { return m_strSenderID; }

	std::string GetSenderDisplayName( ) { return m_strSenderName; }

	std::string GetSubject() { return m_strSubject; }

	bool GetLogging( ) { return m_bLog; }

	bool IsNull( void );

private:

    std::string m_strHost;          // host name or dotted IP address
    std::string m_strSenderID;      // sender userID 
    std::string m_strSenderName;    // sender display name (optional)
    std::string m_strRecipientID;   // recipient userID
    std::string m_strRecipientName; // recipient display name (optional)
    std::string m_strReplyToID;     // reply to userID (optional)
    std::string m_strReplyToName;   // reply to display name (optional)
    std::string m_strMessageID;     // message ID (optional)
    std::string m_strSubject;       // subject of message
    std::string m_strMessageText;   // message text

	bool isNull;					// null 
    bool m_bLog;					// if TRUE, log messages to file

};

#endif // !defined(AFX_SENDMAILDATA_H__A004DADB_E6E1_11D2_9134_0004ACEC70EC__INCLUDED_)
