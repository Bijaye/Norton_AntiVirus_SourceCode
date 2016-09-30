// AVISACSampleComm.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <stdarg.h>
#include <SystemException.h>
#include "AVISACSampleCommDlg.h"
#include "AVISTransactions.h"
#include "AVISTransaction.h"
#include "AVISGatewayManagerSampleComm.h"
#include "AVISProfile.h"
#include "AppInstance.h"
#include "UtilException.h"
#include "AVISTransactionException.h"
#include "FilterException.h"
#include "AVISTransactionException.h"
#include "AVISSendMail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// declare statics
MyTraceClient CAVISACSampleCommApp::s_tc;
int CAVISACSampleCommApp::s_iClientDelayAfterNetworkErrorSecs = 0;
BOOL CAVISACSampleCommApp::s_boAllowUserToRespondToCriticalError = FALSE;
BOOL CAVISACSampleCommApp::s_boSoundAlarmOnCriticalError = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CAVISACSampleCommApp

BEGIN_MESSAGE_MAP(CAVISACSampleCommApp, CWinApp)
	//{{AFX_MSG_MAP(CAVISACSampleCommApp)
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAVISACSampleCommApp construction

CAVISACSampleCommApp::CAVISACSampleCommApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAVISACSampleCommApp object

CAVISACSampleCommApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAVISACSampleCommApp initialization

BOOL CAVISACSampleCommApp::InitInstance()
{
	// force all system exceptions to be thrown as typed C++ exceptions
	SystemException::Init();

	CAVISACSampleCommDlg* pDlg = NULL;

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	try
	{
		DWORD dwErr;

		// make sure there is only one instance of this process
		if (!::AreWeFirstInstance(SINGLE_INSTANCE_MUTEX_NAME, dwErr))
		{
			// check if an error occurred while creating/opening the mutex
			if (ERROR_ALREADY_EXISTS != dwErr)
			{
				char szMsg[256];

				::wsprintf(szMsg, "Error: Unable to create/open app instance mutex (err = %d); exiting...", dwErr);
				::AfxMessageBox(szMsg);

			} else
			{
				::AfxMessageBox("Cannot start multiple instances of AVISACSampleCommApp.exe; exiting...");
			}

			// exit this app (either an instance is already running, or an error occurred
			// while checking).
			return FALSE;
		}

		// register this thread to be able to output trace/log messages.
		s_tc.RegisterThread(MAIN_LOGFILE_NAME, MAIN_TRACEFILE_NAME, MAIN_AGENT_NAME);

		// allow the AVISTransactions DLL to use the trace client declared in this exe.
		CAVISTransactionsApp::SetTraceClient(&s_tc);

		// allow the AVISTransactions DLL to use the AutoTerminate function in this process
		CAVISTransactionsApp::SetAutoTerminateFunction(AutoTerminate);

		// read the app title and remember it

		// free the "default" app name
		if (m_pszAppName != NULL)
			::free((void*)m_pszAppName);

		// make sure we find the resource for the title
		CString strAppName;
		if (!strAppName.LoadString(IDS_APP_TITLE))
		{
			// we didn't find the resource, so hard-code the app name
			m_pszAppName = "AVIS Analysis Center Sample Communications";
		}
		else
			// allocate storage for the new app name and copy the resource string into it
			m_pszAppName = ::_tcsdup(strAppName);

		// free the old INI file name associated with this app
		if (m_pszProfileName != NULL)
			::free((void*)m_pszProfileName);

		// make sure we store the INI file in the current directory
		char szCurDir[1024];
		::GetCurrentDirectory(sizeof(szCurDir), szCurDir);
		char szProfilePath[1024];
		::wsprintf(szProfilePath, "%s\\AVISACSampleComm.ini", szCurDir);
		m_pszProfileName = ::_tcsdup(szProfilePath);

		// create the configuration profile object
		if (!CAVISProfile::OpenProfile(CONFIG_PROFILE_NAME))
		{
			::AfxMessageBox("Unable to open configuration profile; exiting...");
			return FALSE;
		}

		s_boAllowUserToRespondToCriticalError = CAVISProfile::IsValueEnabled(
			ALLOW_USER_TO_RESPOND_TO_CRITICAL_ERROR_KEYNAME, 
			FALSE);

		s_boSoundAlarmOnCriticalError = CAVISProfile::IsValueEnabled(
			SOUND_ALARM_ON_CRITICAL_ERROR_KEYNAME, 
			FALSE);

		// check if we should enable trace output to be written to the trace file(s).
		if (CAVISProfile::IsValueEnabled(ENABLE_TRACING_KEYNAME, FALSE))
		{
			s_tc.EnableTracing();
		}

		s_tc.info("*****************************************************");
		s_tc.msg("Starting AVIS Analysis Center Sample Communications...");

		s_tc.debug("Loading configuration values...");

		// get and store the delay value shared by all clients
		s_iClientDelayAfterNetworkErrorSecs = CAVISProfile::GetIntValue(
			CLIENT_DELAY_AFTER_NETWORK_ERROR_KEYNAME,
			CLIENT_DELAY_AFTER_NETWORK_ERROR_DEFAULT);

		// must map the firewall type into an enumerated type
		std::string strFirewallType = CAVISProfile::GetStringValue(FIREWALL_TYPE_KEYNAME);
		CAVISTransaction::FirewallType fwType = CAVISTransaction::FW_NONE;

		if (strFirewallType == FIREWALL_TYPE_SOCKS)
			fwType = CAVISTransaction::FW_SOCKS;
		else
		{
			if (strFirewallType == FIREWALL_TYPE_PROXY)
				fwType = CAVISTransaction::FW_PROXY;
			else
			{
				if (strFirewallType == FIREWALL_TYPE_DEFINED_IN_REGISTRY)
					fwType = CAVISTransaction::FW_DEFINED_IN_REGISTRY;
			}
		}

		// initialize AVIS internet classes with default values
		CAVISTransaction::InitInternetCommunicationsData(
			fwType,																											// firewall type
			CAVISProfile::GetStringValue(SOCKS_ADDRESS_KEYNAME).c_str(),														// socks address
			CAVISProfile::GetStringValue(PROXY_ADDRESS_KEYNAME).c_str(),														// proxy address
			CAVISProfile::GetStringValue(PROXY_USERNAME_KEYNAME).c_str(),														// proxy username
			CAVISProfile::GetStringValue(PROXY_PASSWORD_KEYNAME).c_str(),														// proxy password
			CAVISProfile::GetIntValue(CONNECTION_TIMEOUT_KEYNAME, CONNECTION_TIMEOUT_DEFAULT) * 1000,						// connect timeout
			CAVISProfile::GetIntValue(CONNECTION_RETRIES_KEYNAME, CONNECTION_RETRIES_DEFAULT),								// connect retries
			CAVISProfile::GetIntValue(SEND_REQUEST_TIMEOUT_KEYNAME, SEND_REQUEST_TIMEOUT_DEFAULT) * 1000,					// send request timeout
			CAVISProfile::GetIntValue(SEND_REQUEST_DATA_TIMEOUT_KEYNAME, SEND_REQUEST_DATA_TIMEOUT_DEFAULT) * 1000,			// send request data timeout
			CAVISProfile::GetIntValue(RECEIVE_RESPONSE_TIMEOUT_KEYNAME, RECEIVE_RESPONSE_TIMEOUT_DEFAULT) * 1000,			// receive response timeout		
			CAVISProfile::GetIntValue(RECEIVE_RESPONSE_DATA_TIMEOUT_KEYNAME, RECEIVE_RESPONSE_DATA_TIMEOUT_DEFAULT) * 1000,	// receive response data timeout
			CAVISProfile::GetIntValue(IGNORE_INVALID_CERTIFICATE_SERVER_NAME_KEYNAME, IGNORE_INVALID_CERTIFICATE_SERVER_NAME_DEFAULT));	// ignore server name mismatch in SSL certificate

		// enable WinInet Logging if specified in profile
		if (CAVISProfile::IsValueEnabled(WININET_DEBUGGING_KEYNAME, FALSE))
		{
			CAVISTransaction::EnableWinInetLogging();
		}

		// wait for user indication to stop program
		s_tc.msg("Waiting for 'termination' request...");

		pDlg = new CAVISACSampleCommDlg;
		m_pMainWnd = pDlg;
		pDlg->DoModal();

		s_tc.msg("Termination request received; shutting down gateway clients...");
	}
	catch(UtilityException* pExcept)
	{
		CriticalError("Critical error occurred using profile or log during application initialization; %s", pExcept->what());

		delete pExcept;
	}
	catch(AVISDBException* pExcept)
	{
		CriticalError("A database exception occured; type = %s, desc = %s", pExcept->TypeAsString().c_str(), pExcept->DetailedInfo().c_str());
		delete pExcept;
	}
	catch(AVISDBException except)
	{
		CriticalError("A database exception occured; type = %s, desc = %s", except.TypeAsString().c_str(), except.DetailedInfo().c_str());
	}
	// AVISException* includes FilterException*, DateTimeException*, AVISFileException*
	catch(AVISException* pExcept)
	{
		CriticalError(
			"A \"%s\" exception occured; type = %s, desc = %s", 
			pExcept->ClassAsString().c_str(),
			pExcept->TypeAsString().c_str(), 
			pExcept->DetailedInfo().c_str());
		delete pExcept;
	}
	catch(CInternetException* pExcept)
	{
		char szMsg[500];
		pExcept->GetErrorMessage(szMsg, sizeof(szMsg), NULL);

		CriticalError("An Internet exception occured; err = %d, desc = %s", pExcept->m_dwError, szMsg);

		pExcept->Delete();
	}
	catch(CException* pExcept)
	{
		char szMsg[500];
		pExcept->GetErrorMessage(szMsg, sizeof(szMsg), NULL);

		CriticalError("An MFC exception occured; desc = %s", szMsg);

		pExcept->Delete();
	}
	catch(CAVISTransactionException* pExcept)
	{
		CriticalError("An AVISTransaction exception occured; desc = %s", pExcept->GetReasonText());
		delete pExcept;
	}
	catch(exception* pExcept)
	{
		CriticalError("An exception from the Standard C++ library was caught; desc = %s", pExcept->what());
		delete pExcept;
	}
	catch (exception& except)
	{
		CriticalError("An exception from the Standard C++ library was caught; desc = %s", except.what());
	}
	catch (SystemException& except)
	{
		CriticalError("A SystemException exception occurred; desc = %s", except.TypeAsString().c_str());
	}
	catch(...)
	{
		CriticalError("An exception of unknown type occurred.");
	}

	if (pDlg != NULL)
		delete pDlg;

	s_tc.msg("Gateway clients have terminated.");
	s_tc.msg("Exiting AVIS Analysis Center Sample Communications...");

	// finished using the configuration profile
	CAVISProfile::CloseProfile();

	// quit the application
	return FALSE;
}
/*****/
void CAVISACSampleCommApp::AutoTerminate(LPCSTR pszMessage)
{
	if (pszMessage != NULL)
		s_tc.info(pszMessage);

	CAVISACSampleCommDlg* pDlg = (CAVISACSampleCommDlg* )AfxGetMainWnd();

	if (pDlg != NULL)
		pDlg->ShutDown();
}
/*****/
void CAVISACSampleCommApp::CriticalError(LPCSTR pszFormat, ...) 
{
	char szFormatted[1024];
	va_list valist;

	va_start( valist, pszFormat );
	::wvsprintf( szFormatted, pszFormat, valist );
	va_end( valist );

	// generate alert via SMTP
	CAVISSendMail sendMail;
	CString strSMTPMessage;
	strSMTPMessage.Format("A critical error has occurred in AVISACSampleComm.exe. Description follows...\n\n%s\n\nSee log and trace files on EXECUTIVE machine for more details.", szFormatted);
	if (!sendMail.AVISSendMail(AVISSENDMAIL_PROFILE_ANALYSIS_COMM_TYPE_INDEX, (LPCSTR )strSMTPMessage))
		s_tc.warning("Unable to send alert via SMTP!");

	s_tc.critical(szFormatted);

	if (s_boSoundAlarmOnCriticalError)
	{
		for (int iLoop = 0; iLoop < 2; iLoop++)
		{
			for (int iFreq = 200; iFreq < 1200; iFreq*= 1.2)
				Beep(iFreq, 10);
			for (; iFreq > 500; iFreq/= 1.1)
				Beep(iFreq, 10);
		}
	}

	if (s_boAllowUserToRespondToCriticalError)
	{
		CString strUserMessage = strSMTPMessage + "\n\nThis stability of this program has been compromised.\n\nPress \"OK\" to terminate.";

		AfxMessageBox(strUserMessage, MB_OK | MB_APPLMODAL | MB_TOPMOST);
	}

	AutoTerminate();
}
