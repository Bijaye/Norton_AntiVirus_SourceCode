//
// file: AvisSendMail.h
//

// system include files
#include <winsock.h>

// user include files
#include "AVISSendMailDLL.h"

#include "Sendmail.h"

// set namespace
using namespace std;

// This class is exported from the AVISSendMail.dll

//////////////////////////////////////////////////////////////////////////////
//
// Class Name: CAVISSendMail 
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
class AVISSENDMAIL_API CAVISSendMail 
{

public:

	//	
	// constructors
	//

	// use default profile file
	CAVISSendMail( );

	// user specified profile file
	CAVISSendMail( const char *configFileName );

	// destructor
	~CAVISSendMail( );

	// "real" Send Mail method
	bool AVISSendMail( const int iType, const char *Message );

    //  send  		
	int SendMail( PSENDMAIL pMail, int *pResult );

	bool IsNull( void );

	// indicates if profile opened successfully
	BOOL bProfileOpened;

private:

	bool isNull;

	static int SendMailMessage( PSENDMAIL pMail );

	static int Receive( SOCKET s, LPTSTR lpszBuff, int nLenMax, int nFlags, LPCTSTR lpszReplyCode );

	static int Send( SOCKET s, const char *lpszBuff, int nLen, int nFlags );

	static void LogMessage( LPCTSTR lpszMsg );

	static bool GetProfileInformation(const char* cType,
									  const char*  cConvertedNumber,
								   	  string &strHostName,
									  string &strSenderUserID,
									  string &strSenderDisplayname,
									  string &strRecipientID,
									  string &strRecipientDisplayname,
									  string &strReplyToID,
									  string &strReplyToDisplayname,
									  string &strMessageID,
									  string &strSubject );

	
	//
	// log file processing
	//
	static void CheckLogFileSize();

	static void LogData( const char* pString, bool bLogTime, bool bDelimiter );

	void LogEMailData(	const char* Type,          // type
						LPCTSTR lpszHost,          // host name or dotted IP address
						LPCTSTR lpszSender,        // sender userID (optional)
						LPCTSTR lpszSenderName,    // sender display name (optional)
						LPCTSTR lpszRecipient,     // recipient userID
						LPCTSTR lpszRecipientName, // recipient display name (optional)
						LPCTSTR lpszReplyTo,       // reply to userID (optional)
						LPCTSTR lpszReplyToName,   // reply to display name (optional)
						LPCTSTR lpszMessageID,     // message ID (optional)
						LPCTSTR lpszSubject,       // subject of message
						LPCTSTR lpszMessage ) ;     // message text )
   

};

//extern AVISSENDMAIL_API int nAVISSendMail;

//AVISSENDMAIL_API int fnAVISSendMail(void);

