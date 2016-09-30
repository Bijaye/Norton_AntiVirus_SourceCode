/////////////////////////////////////////////////////////////////
// SendMail.h - declares functions and data structures for the
// AVISSendMail DLL.
/////////////////////////////////////////////////////////////////

#ifndef __SENDMAIL_H__
#define __SENDMAIL_H__


// SendMail defines /////////////////////////////////////////////

#define SMTP_PORT       25
#define MAX_LINE_SIZE   (1024*4)
#define MAX_NAME_SIZE   (64*4)

// orig.#define MAX_STRING 256
#define MAX_STRING 1024*10   // 10k buffer

//
// profile file keys 
//

// types index values
#define AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX			1
#define AVISSENDMAIL_PROFILE_GATEWAY_TYPE_INDEX				2
#define AVISSENDMAIL_PROFILE_ANALYSIS_COMM_TYPE_INDEX		3
#define AVISSENDMAIL_PROFILE_ANALYSIS_SYSTEM_TYPE_INDEX		4	

// types key
#define AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_KEY			"dataflowtype"
#define AVISSENDMAIL_PROFILE_GATEWAY_TYPE_KEY			"gatewaytype"
#define AVISSENDMAIL_PROFILE_ANALYSIS_COMM_TYPE_KEY		"analysiscommtype"
#define AVISSENDMAIL_PROFILE_ANALYSIS_SYSTEM_TYPE_KEY   "analysissystemtype"

// hostname keys
#define AVISSENDMAIL_PROFILE_HOSTNAME_KEY  "hostname"


// sender user id keys
#define AVISSENDMAIL_PROFILE_SENDERID_KEY  "senderid"


// sender display name keys
#define AVISSENDMAIL_PROFILE_SENDERDISPLAYNAME_KEY "senderdisplayname"


// recipient user id keys
#define AVISSENDMAIL_PROFILE_RECIPIENTID_KEY  "recipientuserid"


// recipient display name keys
#define AVISSENDMAIL_PROFILE_RECIPIENTDISPLAYNAME_KEY "recipientdisplayname"


// reply to user id keys
#define AVISSENDMAIL_PROFILE_REPLYTOID_KEY  "replytouserid"


// reply to display name keys
#define AVISSENDMAIL_PROFILE_REPLYTODISPLAYNAME_KEY "replytodisplayname"


// message id keys
#define AVISSENDMAIL_PROFILE_MESSAGEID_KEY  "messageid"


// Subject keys
#define AVISSENDMAIL_PROFILE_SUBJECT_KEY  "subject"


//
// SendMail data structures /////////////////////////////////////
//

typedef struct SENDMAIL 
{

    LPCTSTR lpszHost;          // host name or dotted IP address
    LPCTSTR lpszSender;        // sender userID (optional)
    LPCTSTR lpszSenderName;    // sender display name (optional)
    LPCTSTR lpszRecipient;     // recipient userID
    LPCTSTR lpszRecipientName; // recipient display name (optional)
    LPCTSTR lpszReplyTo;       // reply to userID (optional)
    LPCTSTR lpszReplyToName;   // reply to display name (optional)
    LPCTSTR lpszMessageID;     // message ID (optional)
    LPCTSTR lpszSubject;       // subject of message
    LPCTSTR lpszMessage;       // message text
    BOOL    bLog;              // if TRUE, log messages to file

} SENDMAIL, *PSENDMAIL;

#endif  //  __SENDMAIL_H__

// End //
