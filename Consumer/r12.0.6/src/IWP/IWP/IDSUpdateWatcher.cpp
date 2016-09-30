#include "StdAfx.h"
#include ".\idsupdatewatcher.h"
#include "IWPSettings.h"
#include "TrayNotifyUI.h"
#include "ResourceHelper.h"
#include "resource.h"
#include "OSInfo.h"
#include "AutoUpdateDefines.h"

CIDSUpdateWatcher::CIDSUpdateWatcher(void)
{
}

CIDSUpdateWatcher::~CIDSUpdateWatcher(void)
{
}

int CIDSUpdateWatcher::Run ()
{
    // Set up waiting objects.
    //
    HANDLE aHandles[2];
    makeEvent ();

	aHandles[0] = m_Terminating.GetHandle();
	aHandles[1] = m_event.GetHandle();

    ccLib::CMessageLock msgLock ( TRUE, FALSE );

	// Main thread loop.
	DWORD dwTimeElapsed = 0;
	DWORD dwWaitTime = INFINITE;
	for(;;)
	{
        // Wait here for something to happen
        //
        DWORD dwWait = msgLock.Lock ( 2, aHandles, FALSE, INFINITE, FALSE );

		// Check for exit signal
		if( dwWait - WAIT_OBJECT_0 == 0 )
		{
			// Yes, bail out.
			break;
		}

        // State changed, update
        //
        {
            ccLib::CCriticalSection critNotify;
            ccLib::CSingleLock lock ( &critNotify, INFINITE, FALSE);

            CCTRACEI ("CIDSUpdateWatcher::Run - got update event");
	        // IDS signatures are being updated.
	        // Show the alert only if the event is fired by AutoLiveUpdate. 

	        // Check to see if the event is fired by ALU 
	        // or by LU launched by the user.

	        // Build correct mutex name.
            ccLib::CMutex mutexALU;
	        if(mutexALU.Open (SYNCHRONIZE, FALSE, SYM_AUTO_UPDATE_MUTEX, FALSE))
	        {
		        CCTRACEI ("CIDSUpdateWatcher::Run - ALU running, display");
                // Open the mutex SYM_AUTO_UPDATE_MUTEX successfully means
		        // AutoLiveUpdate created the mutex to update IDS signatures. 

                fireNotification ();

                if ( makeEvent ())
                {
                    aHandles[1] = m_event;
                }
	        }
            else
                CCTRACEI ("CIDSUpdateWatcher::Run - ALU not running");
        }
    }

    return 1;
}

void CIDSUpdateWatcher::fireNotification ()
{
    CTrayNotifyUI trayalert;
    std::string strTrayAlertText;
    CResourceHelper::LoadString ( IDS_IDS_UPDATE, strTrayAlertText, g_hInstance );
    
    cc::INotify2* pAlert = trayalert.GetAlert();
    if ( pAlert )
    {
        if ((!pAlert->SetProperty ( cc::INotify::PROPERTY_OK_BUTTON, true )) ||
            (!pAlert->SetButton ( cc::INotify::BUTTONTYPE_OK, g_hInstance, IDS_OK_BUTTON )))
            CCTRACEE ("CIDSUpdateWatcher::fireNotification - error setting OK button");

        if ( !trayalert.Start (strTrayAlertText.c_str(), false, false))
            CCTRACEE ("CIDSUpdateWatcher::fireNotification - error displaying tray alert");
    }
    else
        CCTRACEE ("CIDSUpdateWatcher::fireNotification - failed creating alert");
}

bool CIDSUpdateWatcher::makeEvent ()
{

    // Create a notification handle for the refresh event.
    //
    COSInfo OSInfo;

    ATL::CDacl nulldacl;
    ATL::CSecurityDesc secdesc;
    ATL::CSecurityAttributes secatt;

    nulldacl.SetNull();
    secdesc.SetDacl (nulldacl);
    secatt.Set (secdesc);   // This throws assertions in debug mode, I know.

    // Reset the owner
    if ( !m_event.Create ( &secatt,
                            TRUE,
                            FALSE,
                            IWP::SYM_REFRESH_IDS_IWP_SIGS_EVENT,
                            TRUE))
        CCTRACEE ("CIDSUpdateWatcher::makeEvent - unable to create event");

    return true;
}


