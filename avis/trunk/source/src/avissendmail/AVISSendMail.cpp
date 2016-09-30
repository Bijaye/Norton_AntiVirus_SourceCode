//
// file: AVISSendMail.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#pragma warning (disable:4786 )

#define SIZE_TEMP_BUFFER	80

//
// system include files
//
#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <new>

//
// user include files
//

#include "utilexception.h"
#include "AVISSendMail.h"
#include "SendMail.h"
#include "AvisSendMailProfile.h"
#include "Sendmailexception.h"

// set namespace
using namespace std;

//
// Static variables /////////////////////////////////////////////

static BOOL   gbLog  = FALSE;
static HANDLE ghFile = INVALID_HANDLE_VALUE;

//
// Constant Variables
//
const char gszMailerID[] = "X-Mailer: AVIS SendMail DLL V1.0\r\n";

const char gszLogFile[]  = "SendMail.Log";

const char *ConfigFile   = "\\Avissendmail.prf";

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: DllMain
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{

    switch ( ul_reason_for_call )
	{

		case DLL_PROCESS_ATTACH:

		case DLL_THREAD_ATTACH:

		case DLL_THREAD_DETACH:

		case DLL_PROCESS_DETACH:

		break;

    } // end switch 

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//
// Function Name: CAVISSendMail
//
// Author: C. Falterer.
//
// Purpose: 
//
// This is the constructor of a class that has been exported.
// see AVISSendMail.h for the class definition
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
CAVISSendMail::CAVISSendMail()
{ 

	//
	// local stuff
	//
	DWORD dwCurrentDirectorySize;

	// Load the configuration file information from default file, avissendmail.prf
	CAVISSendMailProfile ProfileObject;

	char CurrentDirectory[MAX_STRING];

	char DebugString[MAX_STRING];

	//
	// get the current directory
	//
	dwCurrentDirectorySize = GetCurrentDirectory( MAX_STRING, CurrentDirectory );

	strcat( CurrentDirectory, ConfigFile );

	bProfileOpened = ProfileObject.OpenProfile( CurrentDirectory );

	sprintf( DebugString, "Open profile file: %s", CurrentDirectory );
	LogData( DebugString, true, true );

	return; 

}


//////////////////////////////////////////////////////////////////////////////
//
// Function Name: CAVISSendMail
//
// Author: C. Falterer.
//
// Purpose: 
//
// This is the constructor of a class that has been exported.
// see AVISSendMail.h for the class definition
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
CAVISSendMail::CAVISSendMail(const char *configFileName)
{

	//
	// local stuff
	//

	char DebugString[MAX_STRING];

	// Load the configuration file information from the user supplied file
	CAVISSendMailProfile ProfileObject;

	bProfileOpened = ProfileObject.OpenProfile( configFileName );

	// log data to log file
	sprintf( DebugString,"Open Profile file: %s", configFileName );
	LogData( DebugString, true, true );

	return;

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: ~CAVISSendMail
//
// Author: C. Falterer.
//
// Purpose: 
//
// This is the desstructor of a class that has been exported.
// see AVISSendMail.h for the class definition
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
CAVISSendMail::~CAVISSendMail()
{

	char DebugString[MAX_STRING];

	// Close the profile file
	CAVISSendMailProfile::CloseProfile();

	//
	// log data to log file
	sprintf( DebugString,"Close Profile file" );
	LogData( DebugString, true, false );

}


//
// Methods 
//

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: AVISSendMail
//
// Author: C. Falterer.
//
// Purpose: 
//
// This is the Send mail method of a class that has been exported.
// see AVISSendMail.h for the class definition
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
bool CAVISSendMail::AVISSendMail( const int iType, const char *Message )
{

	string strType;

	// SMTP information
	string strHostName;
	string strSenderUserID;
	string strSenderDisplayname;
	string strRecipientID;
	string strRecipientDisplayname;
	string strReplyToID;
	string strReplyToDisplayname;
	string strMessageID;
	string strSubject;

	bool bResult = true;
	bool bProfileResult;

	int iNumberofEmails;
	int iNumberofEmailLoop;

	int  iResult;
	int* pResult = &iResult;

	char cConvertedNumber[20];
	char cType[80];
	char cNumberEMail[80];

	char DebugString[MAX_STRING];

	LPCSTR pszKey ="";

	// check if profile is OK
	if ( bProfileOpened == FALSE )
		return false;

	switch ( iType )
	{ // switch on type

		//
		// DATAFLOW MAIL
		//
		case AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX:

			//
			// log data to log file
			//
			sprintf( DebugString,"E-mail type is DATAFLOW" );
			LogData( DebugString, true, false );

			pszKey = AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_KEY ;

			// make sure this type is in the 

			strType = CAVISSendMailProfile::GetStringValue( pszKey );

			if ( strType != "")
			{ // found in the profile file

				// set the type string
				strcpy( cType, "dataflow" );

				ZeroMemory( cNumberEMail, 80 );

				sprintf( cNumberEMail,"%snumberofrecipients", cType );

				// set key value
				pszKey = cNumberEMail;

				// get the number of email messages to be sent
				iNumberofEmails = CAVISSendMailProfile::GetIntValue( pszKey );
				
				//
				// log data to log file
				//
				sprintf( DebugString,"Number of DATAFLOW E-mails in profile: %d", iNumberofEmails );
				LogData( DebugString, true, false );

				// check for valid number of e-mails to be sent
				if ( iNumberofEmails > 0 )
				{ // if

					// Create a Send Mail Data object
					PSENDMAIL pSendMail = (PSENDMAIL)malloc( sizeof(SENDMAIL) );

					// check for allocation error
					if ( pSendMail == NULL)
						return false;
				
					//
					// log data to log file
					//
					sprintf( DebugString,"**Read Profile info for each E-Mail**" );
					LogData( DebugString, true, false );

					//
					// loop thru the STL map to pull out the information
					//
					for ( iNumberofEmailLoop = 0; iNumberofEmailLoop < iNumberofEmails; iNumberofEmailLoop ++ )
					{ // for number of e-mails

				
						//
						// log data to log file
						//
						sprintf( DebugString,"DATAFLOW E-mails Number: #%d", iNumberofEmailLoop+1 );
						LogData( DebugString, true, false );

						//
						// Need to build the keys for searching the profile info.
						//

						// convert loop index to string
						_itoa( iNumberofEmailLoop + 1, cConvertedNumber, 10 );

						//
						// Get the data flow profile information
						//
						bProfileResult = GetProfileInformation(	cType,
																cConvertedNumber,
									   							strHostName,
																strSenderUserID,
																strSenderDisplayname,
																strRecipientID,
																strRecipientDisplayname,
																strReplyToID,
																strReplyToDisplayname,
																strMessageID,
																strSubject );

						// check if there is a problem in the profile file
						if ( bProfileResult == false )
						{ // if, yes problem, bail

							// bail out
							return bProfileResult;

						} // end if
												
						//
						// set up the SendMail info.
						//
						pSendMail->lpszHost          = strHostName.c_str();
						pSendMail->lpszSender        = strSenderUserID.c_str();
						pSendMail->lpszSenderName    = strSenderDisplayname.c_str();
						pSendMail->lpszRecipient     = strRecipientID.c_str();
						pSendMail->lpszRecipientName = strRecipientDisplayname.c_str(); 
						pSendMail->lpszReplyTo       = strReplyToID.c_str();
						pSendMail->lpszReplyToName	 = strReplyToDisplayname.c_str();
						pSendMail->lpszMessageID     = strMessageID.c_str();
						pSendMail->lpszSubject       = strSubject.c_str();
						pSendMail->lpszMessage       = Message;
				
						//
						// log data to log file
						//
						LogEMailData(	"DATAFLOW",
										pSendMail->lpszHost,          
										pSendMail->lpszSender,
										pSendMail->lpszSenderName,
										pSendMail->lpszRecipient,     
										pSendMail->lpszRecipientName,  
										pSendMail->lpszReplyTo,       
										pSendMail->lpszReplyToName,	 
										pSendMail->lpszMessageID,     
										pSendMail->lpszSubject,       
										pSendMail->lpszMessage );      

						//
						// Call the Send-email method
						//
						SendMail( pSendMail, pResult );

					} // end for of e-mails

					
				} // end if
				else
				{ // else

					return false;

				} // end else
					
			} // end if
			else
			{ // uh, oh!! not found

				return false;

			} // end else

		break;
		
		//
		// GATEWAY MESSAGES
		//
		case AVISSENDMAIL_PROFILE_GATEWAY_TYPE_INDEX:

			
			//
			// log data to log file
			//
			sprintf( DebugString,"E-mail type is GATEWAY" );
			LogData( DebugString, true, false );

			pszKey = AVISSENDMAIL_PROFILE_GATEWAY_TYPE_KEY ;

			// make sure this type is in the 
			strType = CAVISSendMailProfile::GetStringValue( pszKey );

			if ( strType != "" )
			{ // found in the profile file
				
				// set the type string
				strcpy( cType, "gateway" );

				ZeroMemory( cNumberEMail, 80 );

				sprintf( cNumberEMail,"%snumberofrecipients", cType );

				// set key value
				pszKey = cNumberEMail;

				// get the number of email messages to be sent
				iNumberofEmails = CAVISSendMailProfile::GetIntValue( pszKey );
				
				//
				// log data to log file
				//
				sprintf( DebugString,"Number of GATEWAY E-mails in profile: %d", iNumberofEmails );
				LogData( DebugString, true, false );

				// check for valid number of e-mails to be sent
				if ( iNumberofEmails > 0 )
				{ // if

					// Create a Send Mail Data object
					PSENDMAIL pSendMail = (PSENDMAIL)malloc( sizeof(SENDMAIL) );

					// check for allocation error
					if ( pSendMail == NULL)
						return false;
									
					//
					// log data to log file
					//
					sprintf( DebugString,"**Read Profile info for each E-Mail**" );
					LogData( DebugString, true, false );

					//
					// loop thru the STL map to pull out the information
					//
					for ( iNumberofEmailLoop = 0; iNumberofEmailLoop < iNumberofEmails; iNumberofEmailLoop ++ )
					{ // for number of e-mails
				
						//
						// log data to log file
						//
						sprintf( DebugString,"GATEWAY E-mails Number: #%d", iNumberofEmailLoop+1 );
						LogData( DebugString, true, false );

						//
						// Need to build the keys for searching the profile info.
						//

						// convert loop index to string
						_itoa( iNumberofEmailLoop + 1, cConvertedNumber, 10 );

						//
						// Get the data flow profile information
						//
						bProfileResult = GetProfileInformation(	cType,
																cConvertedNumber,
									   							strHostName,
																strSenderUserID,
																strSenderDisplayname,
																strRecipientID,
																strRecipientDisplayname,
																strReplyToID,
																strReplyToDisplayname,
																strMessageID,
																strSubject );

						// check if there is a problem in the profile file
						if ( bProfileResult == false )
						{ // if, yes problem, bail

							// bail out
							return bProfileResult;

						} // end if
						
						//
						// set up the SendMail info.
						//
						pSendMail->lpszHost          = strHostName.c_str();
						pSendMail->lpszSender        = strSenderUserID.c_str();
						pSendMail->lpszSenderName    = strSenderDisplayname.c_str();
						pSendMail->lpszRecipient     = strRecipientID.c_str();
						pSendMail->lpszRecipientName = strRecipientDisplayname.c_str(); 
						pSendMail->lpszReplyTo       = strReplyToID.c_str();
						pSendMail->lpszReplyToName	 = strReplyToDisplayname.c_str();
						pSendMail->lpszMessageID     = strMessageID.c_str();
						pSendMail->lpszSubject       = strSubject.c_str();
						pSendMail->lpszMessage       = Message;
				
						//
						// log data to log file
						//
						LogEMailData(	"GATEWAY",
										pSendMail->lpszHost,          
										pSendMail->lpszSender,
										pSendMail->lpszSenderName,
										pSendMail->lpszRecipient,     
										pSendMail->lpszRecipientName,  
										pSendMail->lpszReplyTo,       
										pSendMail->lpszReplyToName,	 
										pSendMail->lpszMessageID,     
										pSendMail->lpszSubject,       
										pSendMail->lpszMessage );      

						//
						// Call the Send-email method
						//
						SendMail( pSendMail, pResult );

					} // end for of e-mails

					
				} // end if
				else
				{ // else

					return false;

				} // end else
					
			} // end if
			else
			{ // uh, oh!! not found

				return false;

			} // end else 

		break;

		//
		// ANALYSIS CENTER COMMUNICATIONS MESSAGES
		//
		case AVISSENDMAIL_PROFILE_ANALYSIS_COMM_TYPE_INDEX:
			
			//
			// log data to log file
			//
			sprintf( DebugString,"E-mail type is ANALYSIS COMMUNICATIONS" );
			LogData( DebugString, true, false );

			pszKey = AVISSENDMAIL_PROFILE_ANALYSIS_COMM_TYPE_KEY ;

			// make sure this type is in the 
			strType = CAVISSendMailProfile::GetStringValue( pszKey );

			if ( strType != "")
			{ // found in the profile file
		
				// set the type string
				strcpy( cType, "analysiscomm" );

				ZeroMemory( cNumberEMail, 80 );

				sprintf( cNumberEMail,"%snumberofrecipients", cType );

				// set key value
				pszKey = cNumberEMail;
	
				// get the number of email messages to be sent
				iNumberofEmails = CAVISSendMailProfile::GetIntValue( pszKey );
				
				//
				// log data to log file
				//
				sprintf( DebugString,"Number of ANALYSIS COMM. E-mails in profile: %d", iNumberofEmails );
				LogData( DebugString, true, false );

				// check for valid number of e-mails to be sent
				if ( iNumberofEmails > 0 )
				{ // if

					// Create a Send Mail Data object
					PSENDMAIL pSendMail = (PSENDMAIL)malloc( sizeof(SENDMAIL) );

					// check for allocation error
					if ( pSendMail == NULL)
						return false;
				
					//
					// log data to log file
					//
					sprintf( DebugString,"**Read Profile info for each E-Mail**" );
					LogData( DebugString, true, false );

					//
					// loop thru the STL map to pull out the information
					//
					for ( iNumberofEmailLoop = 0; iNumberofEmailLoop < iNumberofEmails; iNumberofEmailLoop ++ )
					{ // for number of e-mails
				
						//
						// log data to log file
						//
						sprintf( DebugString,"ANALYSIS COMM. E-mails Number: #%d", iNumberofEmailLoop+1 );
						LogData( DebugString, true, false );

						//
						// Need to build the keys for searching the profile info.
						//

						// convert loop index to string
						_itoa( iNumberofEmailLoop + 1, cConvertedNumber, 10 );

						//
						// Get the data flow profile information
						//
						bProfileResult = GetProfileInformation(	cType,
																cConvertedNumber,
									   							strHostName,
																strSenderUserID,
																strSenderDisplayname,
																strRecipientID,
																strRecipientDisplayname,
																strReplyToID,
																strReplyToDisplayname,
																strMessageID,
																strSubject );
				
						// check if there is a problem in the profile file
						if ( bProfileResult == false )
						{ // if, yes problem, bail

							// bail out
							return bProfileResult;

						} // end if

						//
						// set up the SendMail info.
						//
						pSendMail->lpszHost          = strHostName.c_str();
						pSendMail->lpszSender        = strSenderUserID.c_str();
						pSendMail->lpszSenderName    = strSenderDisplayname.c_str();
						pSendMail->lpszRecipient     = strRecipientID.c_str();
						pSendMail->lpszRecipientName = strRecipientDisplayname.c_str(); 
						pSendMail->lpszReplyTo       = strReplyToID.c_str();
						pSendMail->lpszReplyToName	 = strReplyToDisplayname.c_str();
						pSendMail->lpszMessageID     = strMessageID.c_str();
						pSendMail->lpszSubject       = strSubject.c_str();
						pSendMail->lpszMessage       = Message;

						//
						// log data to log file
						//
						LogEMailData(	"ANALYSIS COMM.",
										pSendMail->lpszHost,          
										pSendMail->lpszSender,
										pSendMail->lpszSenderName,
										pSendMail->lpszRecipient,     
										pSendMail->lpszRecipientName,  
										pSendMail->lpszReplyTo,       
										pSendMail->lpszReplyToName,	 
										pSendMail->lpszMessageID,     
										pSendMail->lpszSubject,       
										pSendMail->lpszMessage );      


						//
						// Call the Send-email method
						//
						SendMail( pSendMail, pResult );

					} // end for of e-mails

					
				} // end if
				else
				{ // else

					return false;

				} // end else
					
			} // end if
			else
			{ // uh, oh!! not found

				return false;

			} // end else

		break;

		//
		// ANALYSIS SYSTEM MESSAGES
		//
		case AVISSENDMAIL_PROFILE_ANALYSIS_SYSTEM_TYPE_INDEX:

			//
			// log data to log file
			//
			sprintf( DebugString,"E-mail type is ANALYSIS SYSTEM" );
			LogData( DebugString, true, false );
			
			pszKey = AVISSENDMAIL_PROFILE_ANALYSIS_SYSTEM_TYPE_KEY ;

			// make sure this type is in the 
			strType = CAVISSendMailProfile::GetStringValue( pszKey );

			if ( strType != "")
			{ // found in the profile file

				// set the type string
				strcpy( cType, "analysissystem" );

				ZeroMemory( cNumberEMail, 80 );

				sprintf( cNumberEMail,"%snumberofrecipients", cType );

				// set key value
				pszKey = cNumberEMail;

				// get the number of email messages to be sent
				iNumberofEmails = CAVISSendMailProfile::GetIntValue( pszKey );
				
				//
				// log data to log file
				//
				sprintf( DebugString,"Number of ANALYSIS SYSTEM E-mails in profile: %d", iNumberofEmails );
				LogData( DebugString, true, false );

				// check for valid number of e-mails to be sent
				if ( iNumberofEmails > 0 )
				{ // if

					// Create a Send Mail Data object
					PSENDMAIL pSendMail = (PSENDMAIL)malloc( sizeof(SENDMAIL) );

					// check for allocation error
					if ( pSendMail == NULL)
						return false;
				
					//
					// log data to log file
					//
					sprintf( DebugString,"**Read Profile info for each E-Mail**" );
					LogData( DebugString, true, false );

					//
					// loop thru the STL map to pull out the information
					//
					for ( iNumberofEmailLoop = 0; iNumberofEmailLoop < iNumberofEmails; iNumberofEmailLoop ++ )
					{ // for number of e-mails
				
						//
						// log data to log file
						//
						sprintf( DebugString,"ANALYSIS SYSTEM E-mails Number: #%d", iNumberofEmailLoop=1 );
						LogData( DebugString, true, false );

						//
						// Need to build the keys for searching the profile info.
						//

						// convert loop index to string
						_itoa( iNumberofEmailLoop + 1, cConvertedNumber, 10 );

						//
						// Get the data flow profile information
						//
						bProfileResult = GetProfileInformation(	cType,
																cConvertedNumber,
									   							strHostName,
																strSenderUserID,
																strSenderDisplayname,
																strRecipientID,
																strRecipientDisplayname,
																strReplyToID,
																strReplyToDisplayname,
																strMessageID,
																strSubject );
						
						// check if there is a problem in the profile file
						if ( bProfileResult == false )
						{ // if, yes problem, bail

							// bail out
							return bProfileResult;

						} // end if

						//
						// set up the SendMail info.
						//
						pSendMail->lpszHost          = strHostName.c_str();
						pSendMail->lpszSender        = strSenderUserID.c_str();
						pSendMail->lpszSenderName    = strSenderDisplayname.c_str();
						pSendMail->lpszRecipient     = strRecipientID.c_str();
						pSendMail->lpszRecipientName = strRecipientDisplayname.c_str(); 
						pSendMail->lpszReplyTo       = strReplyToID.c_str();
						pSendMail->lpszReplyToName	 = strReplyToDisplayname.c_str();
						pSendMail->lpszMessageID     = strMessageID.c_str();
						pSendMail->lpszSubject       = strSubject.c_str();
						pSendMail->lpszMessage       = Message;

						//
						// log data to log file
						//
						LogEMailData(	"ANALYSIS SYSTEM",
										pSendMail->lpszHost,          
										pSendMail->lpszSender,
										pSendMail->lpszSenderName,
										pSendMail->lpszRecipient,     
										pSendMail->lpszRecipientName,  
										pSendMail->lpszReplyTo,       
										pSendMail->lpszReplyToName,	 
										pSendMail->lpszMessageID,     
										pSendMail->lpszSubject,       
										pSendMail->lpszMessage );      

						//
						// Call the Send-email method
						//
						SendMail( pSendMail, pResult );

					} // end for of e-mails

					
				} // end if
				else
				{ // else

					return false;

				} // end else
					
			} // end if
			else
			{ // uh, oh!! not found

				return false;

			} // end else 

		break;

		default:
			
			//
			// log data to log file
			//
			sprintf( DebugString,"E-mail type is NOT VALID... BAD" );
			LogData( DebugString, true, false );

		break;


	}// end switch

	return bResult;

}


//////////////////////////////////////////////////////////////////////////////
//
// Function Name: SendMail
//
// Author: C. Falterer.
//
// Purpose: 
//
// SendMail - sends an SMTP mail message to specified host. This
// is the only Public function from this DLL.
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
int CAVISSendMail::SendMail(PSENDMAIL pMail, int *pResult)
{
	
	WORD    wVersion = MAKEWORD( 1, 1 );

	WSADATA wsaData;

	int  nRet;

	//
	// check for required parameters
	//
	if ( pMail == NULL || 
	 	 pResult == NULL ||	
		 pMail->lpszHost == NULL ||
		 pMail->lpszRecipient == NULL ||
		 pMail->lpszSubject == NULL  ||
		 pMail->lpszMessage == NULL )
	{ // if

		if( pResult )
			*pResult = WSAEINVAL;

	    return FALSE;

	} // end if

	if ( (*pResult = WSAStartup(wVersion, &wsaData)) )
		return FALSE;

	// try to send the message
	nRet = SendMailMessage( pMail );

	if ( nRet != 0 )
	{ // if

		if ( gbLog )
		{ // if

			char szMsg[ MAX_LINE_SIZE ];

			if ( nRet < 0 )
			{ // if

				sprintf( szMsg, "SMTP error %d in SendMail.\n", -nRet );
							//
				// log data to log file
				//
				LogData( szMsg, true, false );

			} // end if
			else
			{ // else

				sprintf( szMsg, "Socket error %d in SendMail.\n", nRet );
											//
				// log data to log file
				//
				LogData( szMsg, true, false );

			} // end else

			LogMessage( szMsg );

		} // end if

	} // end if

	// cleanup socket lib
	WSACleanup( );

	*pResult = nRet;

	return ( *pResult == 0 );

}


//////////////////////////////////////////////////////////////////////////////
//
// Function Name: SendMailMessage
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
int CAVISSendMail::SendMailMessage( PSENDMAIL pMail)
{ 
	
	char   szBuff[ MAX_LINE_SIZE + 1 ]; // transmit/receive buffer
	char   szUser[ MAX_NAME_SIZE + 1 ]; // user name buffer
	char   szName[ MAX_NAME_SIZE + 1 ]; // host name buffer

	DWORD  dwSize = MAX_NAME_SIZE;

	SOCKET s;

	struct hostent    *ph;
	struct sockaddr_in addr;

	char   szTime[ MAX_NAME_SIZE + 1 ]; // time related vars
	time_t tTime;

	struct tm   *ptm;
	struct timeb tbTime;

	char DebugString[MAX_STRING];

	LogData( "Start SMTP transactions...", true, false );

	//
	// connect to the SMTP port on remote host
	//

	// create the socket
	if ( (s = socket( AF_INET, SOCK_STREAM, 0) ) == INVALID_SOCKET )
	    return WSAGetLastError( );

	//
	// check if domain name or IP address
	//
	if ( isdigit(*pMail->lpszHost) && strchr(pMail->lpszHost, '.') )
	{ // if, IP Address
	
		sprintf( DebugString, "Get INET address..." );
		LogData( DebugString, false, false );

		unsigned long iaddr = inet_addr( pMail->lpszHost );
	
		sprintf( DebugString, "Get HOST by address..." );
		LogData( DebugString, false, false );

		ph = gethostbyaddr( (const char *)&iaddr, 4, PF_INET ); // orig.
				
	} // end if
	else
	{ // else, domain name
					
		sprintf( DebugString, "Get HOST by Name..." );
		LogData( DebugString, false, false );

		ph = gethostbyname( pMail->lpszHost );

	} // end else

	if ( ph == NULL )
		return WSAGetLastError( );
					
	sprintf( DebugString, "HOST: %s", ph->h_name );
	LogData( DebugString, false, false );

	addr.sin_family = AF_INET;
	addr.sin_port   = htons( SMTP_PORT );

	LogData( "Do memcpy ...", false, false );

	// 
	memcpy( &addr.sin_addr, ph->h_addr_list[0],	sizeof(struct in_addr) ); 

	//
	// log data to log file
	//
	sprintf( DebugString, "Connect to port 25." );
	LogData( DebugString, false, false );

	// connect to port 25
	if ( connect(s, (struct sockaddr *)&addr, sizeof(struct sockaddr) ) )
		return WSAGetLastError();

	//
	// log data to log file
	//
	sprintf( DebugString, "Connection to port 25 successfull." );
	LogData( DebugString, false, false );

	//
	// receive signon message
	//
	Receive( s, szBuff, MAX_LINE_SIZE, 0, "220" );

	sprintf( DebugString, "Received: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// get user name and local host name
	//
	GetUserName( szUser, &dwSize );

	sprintf( DebugString, "User name is: %s", szUser );
	LogData( DebugString, false, false );

	gethostname( szName, MAX_NAME_SIZE );
	
	sprintf( DebugString, "Local Host name: %s", szName );
	LogData( DebugString, false, false );

	//
	// send HELO message
	//
	sprintf( szBuff, "HELO %s\r\n", szName );

	Send( s, szBuff, strlen(szBuff), 0 ); 

	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	Receive( s, szBuff, MAX_LINE_SIZE, 0, "250" ); 

	// log to file
	sprintf( DebugString, "Receive: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// send MAIL message
	//
	if ( pMail->lpszSender )
	{ // if

		sprintf( szBuff, "MAIL FROM: <%s", pMail->lpszSender );

		if( strchr( pMail->lpszSender, '@' ) )
			strcat( szBuff, ">\r\n" );
		else
			sprintf( szBuff + strlen( szBuff ), "@%s>\r\n", szName );

	} // end if
	else
		sprintf( szBuff, "MAIL FROM:<%s@%s>\r\n", szUser, szName );

	Send( s, szBuff, strlen(szBuff), 0 ); 
	
	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	Receive( s, szBuff, MAX_LINE_SIZE, 0, "250" ); 

	// log to file
	sprintf( DebugString, "Receive: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// send RCPT message
	//
	sprintf( szBuff, "RCPT TO: <%s>\r\n", pMail->lpszRecipient );

	Send( s, szBuff, strlen(szBuff), 0 ); 
	
	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	Receive( s, szBuff, MAX_LINE_SIZE, 0, "25" ); 

	// log to file
	sprintf( DebugString, "Receive: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// send DATA message
	//
	sprintf( szBuff, "DATA\r\n" );

	Send( s, szBuff, strlen(szBuff), 0 ); 

	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	Receive( s, szBuff, MAX_LINE_SIZE, 0, "354" );

	// log to file
	sprintf( DebugString, "Receive: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// construct date string
	//
	tTime = time( NULL );

	ptm = localtime( &tTime );

	strftime( szTime, MAX_NAME_SIZE, "%a, %d %b %Y %H:%M:%S %Z", ptm );

	//
	// find time zone offset and correct for DST
	//
	ftime( &tbTime );

	if ( tbTime.dstflag )
		tbTime.timezone -= 60;

	sprintf( szTime + strlen(szTime), " %2.2d%2.2d",
		     -tbTime.timezone / 60, tbTime.timezone % 60 );

	//
	// send mail headers
	// Date:
	//
	sprintf( szBuff, "Date: %s\r\n", szTime );

	Send( s, szBuff, strlen(szBuff), 0 ); 

	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// X-Mailer:
	//
	Send( s, gszMailerID, strlen(gszMailerID), 0 );

	// log to file
	LogData( gszMailerID, false, false );

	//
	// Message-ID:
	//
	if ( pMail->lpszMessageID )
	{ // if

		sprintf( szBuff, "Message-ID: %s\r\n", pMail->lpszMessageID );

		Send( s, szBuff, strlen(szBuff), 0 ); 
		
		// log to file
		sprintf( DebugString, "Send: %s", szBuff );
		LogData( DebugString, false, false );

	} // end if

	//
	// To:
	//
	sprintf( szBuff, "To: %s", pMail->lpszRecipient );

	if ( pMail->lpszRecipientName )
		sprintf( szBuff + strlen(szBuff), " (%s)\r\n", pMail->lpszRecipientName );
	else
		strcat( szBuff, "\r\n" );

	Send( s, szBuff, strlen(szBuff), 0 ); 

	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// From:
	//
	if ( pMail->lpszSender )
	{ // if

		sprintf( szBuff, "From: %s", pMail->lpszSender );

	    if ( pMail->lpszSenderName )
		    sprintf( szBuff + strlen(szBuff), " (%s)\r\n", pMail->lpszSenderName );
		else
			strcat( szBuff, "\r\n" );

	} // end if
	else
		sprintf( szBuff, "From: %s@%s\r\n", szUser, szName );

	Send( s, szBuff, strlen(szBuff), 0 ); 

	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// Reply-To:
	//
	if ( pMail->lpszReplyTo )
	{ // if

		sprintf( szBuff, "Reply-To: %s", pMail->lpszReplyTo );

		if( pMail->lpszReplyToName )
			sprintf( szBuff + strlen(szBuff), " (%s)\r\n", pMail->lpszReplyToName );
		else
			strcat( szBuff, "\r\n" );

		Send( s, szBuff, strlen(szBuff), 0 ); 
		
		// log to file
		sprintf( DebugString, "Send: %s", szBuff );
		LogData( DebugString, false, false );

	} // end if

	//
	// Subject:
	//
	sprintf( szBuff, "Subject: %s\r\n", pMail->lpszSubject );

	Send( s, szBuff, strlen(szBuff), 0 ); 

	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// empty line needed after headers, RFC822
	//
	strcpy(szBuff, "\r\n");

	Send( s, szBuff, strlen(szBuff), 0 ); 

	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	/////////////////////////////////////////////////////////
	//
	// send message text
	//
	Send( s, pMail->lpszMessage,  strlen(pMail->lpszMessage), 0 ); 

	// log to file
	sprintf( DebugString, "Send: Message Text, number of bytes = %d :", strlen(pMail->lpszMessage) );
	LogData( DebugString, false, false );

	//
	// send message terminator and receive reply
	//
	Send( s, "\r\n.\r\n", 5, 0 ); 
	
	// log to file
	LogData( "Message terminator sent", false, false );

	Receive( s, szBuff, MAX_LINE_SIZE, 0, "250" ); 
	
	// log to file
	sprintf( DebugString, "Receive: %s", szBuff );
	LogData( DebugString, false, false );

	//
	// send QUIT message
	//
	sprintf( szBuff, "QUIT\r\n" );

	Send( s, szBuff, strlen(szBuff), 0 ); 
	
	// log to file
	sprintf( DebugString, "Send: %s", szBuff );
	LogData( DebugString, false, false );

	Receive( s, szBuff, MAX_LINE_SIZE, 0, "221" ); 

	// log to file
	sprintf( DebugString, "Receive: %s", szBuff );
	LogData( DebugString, false, false );

	// close the connection
	closesocket( s );

	// log to file
	LogData( "Close Socket", true, false );

	return 0;

}


//////////////////////////////////////////////////////////////////////////////
//
// Function Name: LogMessage
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CAVISSendMail::LogMessage(LPCTSTR lpszMsg)
{

	DWORD dwRet;

	// 
	if ( ghFile != INVALID_HANDLE_VALUE )
	{ // if

		WriteFile(	ghFile, 
					lpszMsg, 
					strlen(lpszMsg), 
					&dwRet, 
					NULL );

	} // end if 

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: Send
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
int CAVISSendMail::Send(SOCKET s, 
						const char *lpszBuff, 
						int nLen, 
						int nFlags)
{

	int nCnt = 0;

	while ( nCnt < nLen )
	{ // while

		int nRes = send( s, lpszBuff + nCnt, nLen - nCnt, nFlags );

		if ( nRes == SOCKET_ERROR )
	        return WSAGetLastError();
		else
			nCnt += nRes;

	} // end while

	if ( gbLog )
		LogMessage( lpszBuff );

	return 0;

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: Receive
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
int CAVISSendMail::Receive(SOCKET s, 
						   LPTSTR lpszBuff, 
						   int nLenMax, 
						   int nFlags, 
						   LPCTSTR lpszReplyCode)
{

	LPTSTR p;

	int    nRes = recv( s, lpszBuff, nLenMax, nFlags );

	if ( nRes == SOCKET_ERROR )
		return WSAGetLastError();
	else
	    *( lpszBuff + nRes ) = '\0';

	if ( gbLog )
		LogMessage( lpszBuff );

	// check reply code for success/failure
	p = strtok( lpszBuff, "\n" );

	while ( p )
	{ // while

		if ( *(p + 3) == ' ' )
		{ // if

			if ( !strncmp(p, lpszReplyCode, strlen(lpszReplyCode) ) )
				return 0;
			else
			{ // else

				int nErr = 1;

	            sscanf( p, "%d", &nErr );

		        return -nErr;

			} // end else

		} // end if
		else
			p = strtok( NULL, "\n" );

	} // end while

	return -1;

}


//////////////////////////////////////////////////////////////////////////////
//
// Function Name: IsNull
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
bool CAVISSendMail::IsNull()
{
 
	return isNull;

}


//////////////////////////////////////////////////////////////////////////////
//
// Function Name: GetProfileInformation
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
bool CAVISSendMail::GetProfileInformation( const char* cType,
										   const char* cConvertedNumber,
									   	   string &strHostName,
										   string &strSenderUserID,
										   string &strSenderDisplayname,
										   string &strRecipientID,
 										   string &strRecipientDisplayname,
										   string &strReplyToID,
										   string &strReplyToDisplayname,
										   string &strMessageID,
										   string &strSubject )
{

	bool bStatus = true;

	char cTempBuffer[SIZE_TEMP_BUFFER];

	//
	// HOST NAME
	//

	// clean out temp buffer
	memset( (void*)cTempBuffer, 0, SIZE_TEMP_BUFFER );

	// build the host name key
	strcpy( cTempBuffer, cType );
	strcat( cTempBuffer, AVISSENDMAIL_PROFILE_HOSTNAME_KEY );
	strcat( cTempBuffer, cConvertedNumber );
							
	// get the entry from the hash table 
	strHostName = CAVISSendMailProfile::GetStringValue( cTempBuffer );			

	//
	// make sure we have a valid entry in the profile file
	//
	if ( strHostName == "")
	{ // NOT found in the profile file

		// return error condition
		return false;

	} // end if

	//
	// SENDER USER ID
	//

	// clean out temp buffer
	memset( (void*)cTempBuffer, 0, SIZE_TEMP_BUFFER );

	// build the Sender ID key
	strcpy( cTempBuffer, cType );
	strcat( cTempBuffer, AVISSENDMAIL_PROFILE_SENDERID_KEY );
	strcat( cTempBuffer, cConvertedNumber );
							
	// get the entry from the hash table 
	strSenderUserID = CAVISSendMailProfile::GetStringValue( cTempBuffer );			

	//
	// make sure we have a valid entry in the profile file
	//
	if ( strSenderUserID == "")
	{ // NOT found in the profile file

		// return error condition
		return false;

	} // end if
						
	//
	// SENDER DISPLAY NAME 
	//

	// clean out temp buffer
	memset( (void*)cTempBuffer, 0, SIZE_TEMP_BUFFER );

	// build the sender display name key
	strcpy( cTempBuffer, cType );
	strcat( cTempBuffer, AVISSENDMAIL_PROFILE_SENDERDISPLAYNAME_KEY );
	strcat( cTempBuffer, cConvertedNumber );
							
	// get the entry from the hash table 
	strSenderDisplayname = CAVISSendMailProfile::GetStringValue( cTempBuffer );			

	//
	// RECIPIENT ID 
	//
						
	// clean out temp buffer
	memset( (void*)cTempBuffer, 0, SIZE_TEMP_BUFFER );

	// build the recipient ID key
	strcpy( cTempBuffer, cType );
	strcat( cTempBuffer, AVISSENDMAIL_PROFILE_RECIPIENTID_KEY );
	strcat( cTempBuffer, cConvertedNumber );
							
	// get the entry from the hash table 
	strRecipientID = CAVISSendMailProfile::GetStringValue( cTempBuffer );			

	//
	// make sure we have a valid entry in the profile file
	//
	if ( strRecipientID == "")
	{ // NOT found in the profile file

		// return error condition
		return false;

	} // end if
	
	//
	// RECIPIENT DISPLAY NAME
	//
				
	// clean out temp buffer
	memset( (void*)cTempBuffer, 0, SIZE_TEMP_BUFFER );

	// build the recipient display key
	strcpy( cTempBuffer, cType );
	strcat( cTempBuffer, AVISSENDMAIL_PROFILE_RECIPIENTDISPLAYNAME_KEY );
	strcat( cTempBuffer, cConvertedNumber );
							
	// get the entry from the hash table 
	strRecipientDisplayname = CAVISSendMailProfile::GetStringValue( cTempBuffer );			
						
	//
	// REPLY TO USER ID
	//
						
	// clean out temp buffer
	memset( (void*)cTempBuffer, 0, SIZE_TEMP_BUFFER );

	// build the reply to user ID key
	strcpy( cTempBuffer, cType );
	strcat( cTempBuffer, AVISSENDMAIL_PROFILE_REPLYTOID_KEY );
	strcat( cTempBuffer, cConvertedNumber );
							
	// get the entry from the hash table 
	strReplyToID = CAVISSendMailProfile::GetStringValue( cTempBuffer );			

	//
	// REPLY TO DISPLAY NAME
	//
														
	// clean out temp buffer
	memset( (void*)cTempBuffer, 0, SIZE_TEMP_BUFFER );

	// build the reply to display name key
	strcpy( cTempBuffer, cType );
	strcat( cTempBuffer, AVISSENDMAIL_PROFILE_REPLYTOID_KEY );
	strcat( cTempBuffer, cConvertedNumber );
							
	// get the entry from the hash table 
	strReplyToDisplayname = CAVISSendMailProfile::GetStringValue( cTempBuffer );			
										
	//
	// MESSAGE ID
	//
														
	// clean out temp buffer
	memset( (void*)cTempBuffer, 0, SIZE_TEMP_BUFFER );

	// build the message key
	strcpy( cTempBuffer, cType );
	strcat( cTempBuffer, AVISSENDMAIL_PROFILE_MESSAGEID_KEY );
	strcat( cTempBuffer, cConvertedNumber );
							
	// get the entry from the hash table 
	strMessageID = CAVISSendMailProfile::GetStringValue( cTempBuffer );			

	//
	// SUBJECT OF MESSAGE
	//
														
	// clean out temp buffer
	memset( (void*)cTempBuffer, 0, SIZE_TEMP_BUFFER );

	// build the subject key
	strcpy( cTempBuffer, cType );
	strcat( cTempBuffer, AVISSENDMAIL_PROFILE_SUBJECT_KEY );
	strcat( cTempBuffer, cConvertedNumber );
							
	// get the entry from the hash table 
	strSubject = CAVISSendMailProfile::GetStringValue( cTempBuffer );			
	
	return bStatus;

}


//////////////////////////////////////////////////////////////////////////////
//
// Function Name: LogData
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CAVISSendMail::LogData(const char *pString, bool bLogTime, bool bDelimiter)
{
	// file pointer for log file
	FILE* fpLogFile;

	DWORD dwCurrentDirectorySize;

	struct _timeb TimeBuffer;

	char* timeline;

	char CurrentDirectory[MAX_STRING];

	// check the size of the log file
	CheckLogFileSize( );	

	//
	// open the log file
	//
	
	//
	// get the current directory
	//
	dwCurrentDirectorySize = GetCurrentDirectory( MAX_STRING, CurrentDirectory );

	strcat( CurrentDirectory, "\\AVISSENDMAIL.LOG" );

	// open the log file
	fpLogFile = fopen( CurrentDirectory, "a+" );

	//
	// get the current system time
	//
	_ftime( &TimeBuffer );

	timeline = ctime( &(TimeBuffer.time));

	//
	// save off to log file
	//
	fprintf( fpLogFile, "\n" );

	if ( bDelimiter == true )
		fprintf( fpLogFile, "---------------------------------------------------------- \n");

	// string to file 
	fprintf( fpLogFile, pString );

	// log the time?
	if ( bLogTime == true )
	{ // if

		fprintf( fpLogFile," at time: ");
		fprintf( fpLogFile, timeline );

	} // end if

	// 
	fprintf( fpLogFile, "\n" );

	// close the log file
	if ( fpLogFile != NULL )
		fclose( fpLogFile );

}

//////////////////////////////////////////////////////////////////////////////
//
// Function Name: CheckLogFileSize
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
// Input Parameters: 
//
// Return Value: 
//
//
/////////////////////////////////////////////////////////////////////////////
void CAVISSendMail::CheckLogFileSize()
{

	//
	// local stuff
	//

	// file pointer for log file
	FILE* fpLogFile;

	DWORD dwCurrentDirectorySize;

	DWORD dwLogFileSize;

	fpos_t FilePosition;

	char CurrentDirectory[MAX_STRING];

	//
	// get the current directory
	//
	dwCurrentDirectorySize = GetCurrentDirectory( MAX_STRING, CurrentDirectory );

	strcat( CurrentDirectory, "\\AVISSENDMAIL.LOG" );

	// open the log file
	fpLogFile = fopen( CurrentDirectory, "a+" );

	// get the  file size
	fseek( fpLogFile, 0, SEEK_END );

	fgetpos( fpLogFile, &FilePosition );

	dwLogFileSize = (DWORD)FilePosition;

	// close the file
	if ( fpLogFile != NULL )
		fclose( fpLogFile );

	//
	// check if file is grown > 600k
	//
	if ( ( dwLogFileSize > (600 * 1024) ) && ( dwLogFileSize != 0xFFFFFFFF))
	{ // if

		//
		// copy the log file to the backup one, then delete the current one
		//
		system( "copy avissendmail.log avissendmail.log.bak" );
		system( "del avissendmail.log" );

	} // end if


}

void CAVISSendMail::LogEMailData(	const char* Type,          // type
									LPCTSTR lpszHost,          // host name or dotted IP address
									LPCTSTR lpszSender,        // sender userID (optional)
									LPCTSTR lpszSenderName,    // sender display name (optional)
									LPCTSTR lpszRecipient,     // recipient userID
									LPCTSTR lpszRecipientName, // recipient display name (optional)
									LPCTSTR lpszReplyTo,       // reply to userID (optional)
								    LPCTSTR lpszReplyToName,   // reply to display name (optional)
									LPCTSTR lpszMessageID,     // message ID (optional)
								    LPCTSTR lpszSubject,       // subject of message
									LPCTSTR lpszMessage )      // message text )
    
{

	char DebugString[MAX_STRING];

	
	//
	// log data to log file
	//

	// HOST
	sprintf( DebugString,"%s E-mails in HOST in profile: %s", Type, lpszHost );
	LogData( DebugString, true, false );

	// SENDER ID
	sprintf( DebugString,"%s E-mails in SENDER ID in profile: %s", Type, lpszSender );
	LogData( DebugString, true, false );

	// SENDER NAME
	sprintf( DebugString,"%s E-mails in SENDER NAME in profile: %s", Type, lpszSenderName );
	LogData( DebugString, true, false );

	// RECIPIENT ID
	sprintf( DebugString,"%s E-mails in RECIPIENT ID in profile: %s", Type, lpszRecipient );
	LogData( DebugString, true, false );

	// RECIPIENT NAME
	sprintf( DebugString,"%s E-mails in RECIPIENT NAME in profile: %s", Type, lpszRecipientName );
	LogData( DebugString, true, false );

	// REPLY TO ID
	sprintf( DebugString, "%s E-mails in REPLY TO ID in profile: %s", Type, lpszReplyTo );
	LogData( DebugString, true, false );

	// REPLY TO NAME
	sprintf( DebugString,"%s E-mails in REPLY TO NAME in profile: %s", Type, lpszReplyToName );
	LogData( DebugString, true, false );

	// MESSAGE ID
	sprintf( DebugString,"%s E-mails in MESSAGE ID in profile: %s", Type, lpszMessageID );
	LogData( DebugString, true, false );

	// SUBJECT
	sprintf( DebugString,"%s E-mails in SUBJECT in profile: %s", Type, lpszSubject );
	LogData( DebugString, true, false );

}
