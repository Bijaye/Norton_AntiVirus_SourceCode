#include "stdafx.h"
#include "blockedappui.h"
#include "ResourceHelper.h"     // Class to help us load strings, etc.
#include "SymInterfaceHelpers.h"
#include "tlhelp32.h"   // Toolhelp stuff.
#include "ccVersionInfo.h"
#include "NAVHelpLauncher.h"        // NAVToolbox - for help
#include "ccCriticalSection.h"
#include "ccSymInstalledApps.h"
#include "AVRESBranding.h"

#define BAG_MAX_TEXT 1024   // Max size for wsprintf

extern CSymInterfaceDllHelper g_symHelperCCAlert;

// statics
long CBlockedAppUI::m_lNumActiveUI = 0;
std::string CBlockedAppUI::m_strCCPath;
DWORD CBlockedAppUI::m_dwWaitTimeMinDisplayBlocked = 0;
DWORD CBlockedAppUI::m_dwWaitTimeMaxDisplayBlocked = 0;
DWORD CBlockedAppUI::m_dwForcedAction = 0;

CBlockedAppUI::CBlockedAppUI(SAVRT_ROUS64 hCookie,
                             long lProcessID,
                             std::wstring strPath,
                             DWORD dwWaitTime,
                             CBlockedAppUISink* pSink /*NULL*/)
{
    m_pSink = pSink;
    m_pUI = NULL;
    m_pHelpCallback = NULL;
    m_bDisplaying = false;
    m_bClosing = false;
    m_lProcessID = lProcessID;
    m_hCookie = hCookie;
    m_strPath = strPath;
    m_dwBlockedDelay = dwWaitTime;
    m_bDecremented = false;

    InterlockedIncrement ( &m_lNumActiveUI );
    CCTRACEI ("CBlockedAppUI - active UI = %d", m_lNumActiveUI );
}

// Static member initialization
//
void CBlockedAppUI::init ()
{
    try
    {
        if ( SYM_FAILED (g_symHelperCCAlert.Initialize(g_strCCAlertPath.c_str())))
        {
            CCTRACEE (_T("CBlockedAppUI::CBlockedAppUI - SymHelper failed Initialize"));
            throw false;
        }

		// Get product name from AVRES.DLL
		if(m_csProductName.IsEmpty())
		{
			CBrandingRes BrandRes;
			m_csProductName = BrandRes.ProductName();
		}
    }
    catch(...)
    {
        CCTRACEE (_T("CBlockedAppUI::CBlockedAppUI - Failed making UI object"));
    }
}

CBlockedAppUI::~CBlockedAppUI(void)
{
    // We own this at this point.
    //
    if ( m_pHelpCallback ) 
    {
        delete m_pHelpCallback;
        m_pHelpCallback = NULL;
    }

    if ( !m_bDecremented )
    {
        InterlockedDecrement ( &m_lNumActiveUI );
        CCTRACEI ("~CBlockedAppUI - active UI = %d", m_lNumActiveUI );
    }
}

long CBlockedAppUI::GetNumActiveUI ()
{
    return m_lNumActiveUI;
}

void CBlockedAppUI::Close (bool bCloseNow /*false*/)
{
    try 
    {
        if ( m_bClosing )
        {
            CCTRACEE ( _T("CBlockedAppUI::Close already closing"));
            return;
        }

        m_bClosing = true;

        ccLib::CSingleLock lock(&m_critUI, INFINITE, FALSE);

        if ( m_pUI && m_bDisplaying )
        {
            if ( !bCloseNow )
            {
                // Wait two seconds before closing the UI so the user gets a chance to read it.
                //
                DWORD dwElapsedTime = timer.Stop();

                if ( dwElapsedTime >= 0 && dwElapsedTime < m_dwWaitTimeMinDisplayBlocked )
                {
                    CCTRACEI ( _T("CBlockedAppUI::Close - Closing UI - Waiting %d - %d"), m_dwWaitTimeMinDisplayBlocked - timer.GetElapsedTime(), this );
                    m_pCloser = std::auto_ptr<CBlockedAppUICloser> (new CBlockedAppUICloser (m_pUI, m_dwWaitTimeMinDisplayBlocked - timer.GetElapsedTime()));
                    m_pCloser->Create (NULL,0,0);
                }
                else
                {
                    CCTRACEI ( _T("CBlockedAppUI::Close - Closing UI - No waiting - %d"), this);

                    if ( m_pUI )
                    {
                        m_pUI->Hide ();
                        m_bDisplaying = false;
                    }
                }
           }
            else
            {
                CCTRACEI ( _T("CBlockedAppUI::Close - Closing UI NOW! - %d"), this);

                if ( m_pUI )
                {
                    m_pUI->Hide ();
                    m_bDisplaying = false;
                }
            }
        }
        else
        {
            // Send message to running UI to close
            Terminate(INFINITE);
            CCTRACEI ( _T("CBlockedAppUI::Close - Closing, no UI - %d"), this);
        }
    }
    catch (...)
    {
        CCTRACEE ( _T("CBlockedAppUI::Close - CBlockedAppUI::close ()"));
    }
}

void CBlockedAppUI::display ()
{
	try 
    {
        if ( IsTerminating() )
        {
            // Exit has been set, we need to bail
            return;
        }

        init ();    // For statics

        std::wstring strButtonOK;
        std::wstring strButtonAbort;
        std::wstring strButtonMoreInfo;

        // <app name(s1)> is waiting for a virus scan of <filename(s2)>
        //
        WCHAR szAlertText [BAG_MAX_TEXT] = {0}; // 1024 is max size for wsprintf
        WCHAR szToolTipText [BAG_MAX_TEXT] = {0}; // 1024 is max size for wsprintf
        std::wstring strAppName;
        std::wstring strAlertTextFormat;
        std::wstring strFileName;
        std::wstring strAppPath;

        // Lock the UI so we don't close it.
        //
        {
            ccLib::CSingleLock lock(&m_critUI, INFINITE, FALSE);

            // Do we have a UI object?
            //
            if ( !m_pUI )
            {
                SYMRESULT symRes = g_symHelperCCAlert.CreateObject(cc::IID_Notify, cc::IID_Notify, (void**) &m_pUI);
                if ( SYM_FAILED (symRes ) || m_pUI == NULL )
                {
                    CCTRACEE (_T("CBlockedAppUI::display - SymHelper failed CreateObject"));
                    throw false;
                }
            }

            // Set values for the alert
            //
            // Set control properties - there are defaults but I don't trust them.
            //                                                                 
            bool bResult = false;
            bResult = m_pUI->SetProperty (cc::INotify::PROPERTY_TASKBAR_ENTRY, true);  // Taskbar so you can keyboard to it
            bResult = m_pUI->SetProperty (cc::INotify::PROPERTY_TOPMOST, false);       // No taking focus
	        bResult = m_pUI->SetProperty (cc::INotify::PROPERTY_SHOW_PROGRESS, false); // No progress bar
            bResult = m_pUI->SetProperty (cc::INotify::PROPERTY_OK_BUTTON, true);      // OK button
	        bResult = m_pUI->SetProperty (cc::INotify::PROPERTY_CANCEL_BUTTON, true ); // Cancel button
	        bResult = m_pUI->SetProperty (cc::INotify::PROPERTY_MOREINFO_BUTTON, true);// More Info/Help link

            if ( !CResourceHelper::LoadString ( IDS_BLOCKED_APP_ALERT_TEXT, strAlertTextFormat, _Module.GetResourceInstance() ))
            {
                CCTRACEE (_T("CBlockedAppUI::display - No alert text"));
            }

            if ( m_lProcessID )
            {
                // Figure out the real name for the app.
                //
                if ( getAppPath ( (DWORD) m_lProcessID, strAppPath ))
                {
                    if ( !getAppRealName ( strAppPath, strAppName ))
                    {
                        // If we can't figure it out, use the EXE name.
                        //
                        WCHAR pcFileName [_MAX_FNAME] = {0};
                        WCHAR pcExtension [_MAX_EXT] = {0};            
                        _wsplitpath ( strAppPath.c_str(), NULL, NULL, pcFileName, pcExtension );
                        strAppName = pcFileName;
                        strAppName += pcExtension; // e.g. ".exe"
                    }
                }
            }
            else
                CCTRACEE (_T("CBlockedAppUI::display - No process ID"));

            // If we don't know the name, say "unknown".
            //
            if (strAppName.empty())
            {
                CResourceHelper::LoadString ( IDS_BLOCKED_APP_UNKNOWN_APP, strAppName, _Module.GetResourceInstance() );
            }

            if ( m_strPath.empty() )
            {
                CCTRACEE (_T("CBlockedAppUI::display - No file name"));
            }
            else
            {
                // Get the filename from a whole path
                //
                WCHAR pcFileName [_MAX_FNAME] = {0};
                WCHAR pcExtension [_MAX_EXT] = {0};
                _wsplitpath ( m_strPath.c_str(), NULL, NULL, pcFileName, pcExtension );
                strFileName = pcFileName;
                strFileName += pcExtension; // e.g. ".zip"
            }

            // Before we write into our buffer make sure it will fit!
            //
            long lTestSize = strAlertTextFormat.length() + strAppName.length () + strFileName.length () + 1;
            if ( lTestSize > BAG_MAX_TEXT)
            {
                CCTRACEE (_T("CBlockedAppUI::display - Alert text too large"));
                throw false;
            }
            
            // WCHAR version of wsprintf
            //
            wsprintfW ( szAlertText, strAlertTextFormat.c_str(), strAppName.c_str(), strFileName.c_str() );

            // Tool tip is App Name and Full Path
            //
            lTestSize = strAlertTextFormat.length() + strAppName.length () + m_strPath.length () + 1;
            if ( lTestSize <= BAG_MAX_TEXT)
            {
                wsprintfW ( szToolTipText, strAlertTextFormat.c_str(), strAppName.c_str(), m_strPath.c_str() );
                m_pUI->SetTooltip (szToolTipText);
            }
            
            m_pUI->SetText (szAlertText);

            bResult = m_pUI->SetTimeout (m_dwWaitTimeMaxDisplayBlocked); // run away scan timer

            // Buttons
            //
            CResourceHelper::LoadString ( IDS_BLOCKED_APP_BUTTON_OK, strButtonOK, _Module.GetResourceInstance() );
            CResourceHelper::LoadString ( IDS_BLOCKED_APP_BUTTON_ABORT, strButtonAbort, _Module.GetResourceInstance() );
            CResourceHelper::LoadString ( IDS_BLOCKED_APP_BUTTON_MOREINFO, strButtonMoreInfo, _Module.GetResourceInstance() );

            bResult = m_pUI->SetButton ( cc::INotify::BUTTONTYPE_OK, strButtonOK.c_str());
		    bResult = m_pUI->SetButton ( cc::INotify::BUTTONTYPE_CANCEL, strButtonAbort.c_str());

            // Set help button callback
            m_pHelpCallback = new CBlockedAppUIHelp (IDH_NAVW_VIRUS_SCAN_DOWNLOAD_ALERT);
            bResult = m_pUI->SetButton ( cc::INotify::BUTTONTYPE_MOREINFO, strButtonMoreInfo.c_str(), m_pHelpCallback);

            // Icon
            bResult = m_pUI->SetIcon ( g_hInstance, IDI_NAV_ICON, IDI_NAV_ICON );

            // Set the title and title bitmap
            bResult = m_pUI->SetTitle ( m_csProductName );

			CString csTaskBarFormat;
			CString csTaskBar;
			csTaskBarFormat.LoadString(_Module.GetResourceInstance(), IDS_BLOCKED_APP_TASKBAR);
			csTaskBar.Format(csTaskBarFormat, m_csProductName);
            bResult = m_pUI->SetTaskBarText ( csTaskBar );

            // Check for exit one last time
            //
            if ( IsTerminating ())
            {
                // Exit has been set, we need to bail
                return;
            }

            // Set the time - do this *just* before displaying
            //
            timer.Start();

            m_bDisplaying = true;

            // Show it. This call is modal.
            //
            CCTRACEI (_T("CBlockedAppUI::display - Displaying BlockedApp UI - %d"), this);
        }
        cc::INotify::RESULT result;

        if ( m_dwForcedAction )
            result = (cc::INotify::RESULT) m_dwForcedAction;   // For stress testing
        else
            result = m_pUI->Display (NULL);  // Display the UI

        // We don't need this anymore and we don't need to close it.
        //
        m_pUI.Release ();

        m_bDisplaying = false;

        switch ( result )    // User wants to abort the scan
        {
            case cc::INotify::RESULT_CANCEL:
                // Call back into the service with an Abort!
                // Don't cancel a scan that has already finished.
                if ( !m_bClosing )
                {
                    CCTRACEI (_T("CBlockedAppUI::display - Aborting scan"));
                    m_pSink->OnBlockedAppUICancel();
                }
                break;

            case cc::INotify::RESULT_TIMEOUT:
            case cc::INotify::RESULT_OK:
                // Tell the owner that we are closing.
                //
                if ( !m_bClosing && m_pSink )
                {
                    CCTRACEI (_T("CBlockedAppUI::display - calling sink"));
                    InterlockedDecrement ( &m_lNumActiveUI );
                    CCTRACEI ("CBlockedAppUI - closing - active UI = %d", m_lNumActiveUI );
                    m_bDecremented = true;
                    m_pSink->OnBlockedAppUIClosed ();
                }

                if ( result == cc::INotify::RESULT_TIMEOUT)
                    CCTRACEI (_T("CBlockedAppUI::display - UI timed out"));
                else
                    CCTRACEI (_T("CBlockedAppUI::display - User closed UI"));

                break;

            case cc::INotify::RESULT_HIDE:
                CCTRACEI (_T("CBlockedAppUI::display - UI closed by us"));
                break;
            
            default:
                CCTRACEE (_T("CBlockedAppUI::display - Failure displaying UI"));
                break;
        }
    }
    catch (...)
    {
        CCTRACEE (_T("CBlockedAppUI::display"));
    }
}


int CBlockedAppUI::Run ()
{
    CCTRACEI (_T("CBlockedAppUI::Run - Waiting to display blocked UI %d - %d"), this, m_hCookie);

    // Don't display right away. Make sure this doesn't get cancelled immediately.
    //
    ccLib::CMessageLock msgLock (TRUE, TRUE);
    if ( WAIT_TIMEOUT == msgLock.Lock ( m_Terminating.GetHandle(), m_dwBlockedDelay))
    {
        CCTRACEI (_T("CBlockedAppUI::Run - displaying UI"));

        // Display the UI.
        //
        display ();
    }

    CCTRACEI (_T("CBlockedAppUI::Run - Closing UI thread - %d"), this);

    // We were told to exit. Bail.
    return 0; // SUCCESS
}

bool CBlockedAppUI::getAppPath ( DWORD dwProcessID, std::wstring& strAppFullPath )
{
    bool bReturn = false;
    HANDLE         hModuleSnap = NULL;
    MODULEENTRY32W me32      = {0}; 
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID); 
    
    if (hModuleSnap == INVALID_HANDLE_VALUE) 
        return false; 

    //  Fill in the size of the structure before using it. 
    //
    me32.dwSize = sizeof(MODULEENTRY32W); 
 
    if ( Module32FirstW(hModuleSnap, &me32))
    {
        strAppFullPath = me32.szExePath;
        bReturn = true;
    }

    // Do not forget to clean up the snapshot object. 
    //
    CloseHandle (hModuleSnap);
    return bReturn;
}

bool CBlockedAppUI::getAppRealName ( std::wstring& strAppFullPath /*in*/, std::wstring& strAppRealName /*out*/ )
{
	// Kludgey here because we aren't compiled UNICODE.
    //
    std::string strAppname;
    TCHAR szTempAppPath [_MAX_PATH] = {0};
    wcstombs ( szTempAppPath, strAppFullPath.c_str(), _MAX_PATH );

	// Check for a version description in the EXE
	LPCTSTR szTempString;
	ccLib::CVersionInfo vi;
	vi.Load(szTempAppPath);

	bool goodName = false;

	if(vi.Load(szTempAppPath))
    {
		szTempString = vi.GetFileDescription();
        WCHAR szTempAppRealName [MAX_PATH] = {0};
        mbstowcs ( szTempAppRealName, szTempString, MAX_PATH );

        // strAppRealName = szTempAppRealName;

		// Trim the description.
		CStringW name = CStringW(szTempAppRealName).Trim();

		if (!name.IsEmpty())
		{
			strAppRealName = name;
			goodName = true;
		}
    }

    if (goodName)
	{
		// There's no good file name
        //
        if(strcmpi(szTempString, _T("System")) == 0) // does this translate??
		{
            CResourceHelper::LoadString ( IDS_WINDOWSSUBSYSTEM, strAppRealName, _Module.GetResourceInstance() );
		}
		else
            return false;
	}

    // If the name is "N/A", erase it so the client knows it's not found. Don't translate!
    //
	if( 0 == lstrcmpiW(strAppRealName.c_str(), L"N/A"))
    {
        strAppRealName = L"";
        return false;
    }

    return true;
}

// Show the UI for at least this much time (no flickers!)
void CBlockedAppUI::SetWaitTimeMinDisplayBlocked ( DWORD dwWaitTimeMinDisplayBlocked )
{
    m_dwWaitTimeMinDisplayBlocked = dwWaitTimeMinDisplayBlocked;
}

// Close the UI if an app is blocked for this long. Doesn't cancel.
void CBlockedAppUI::SetWaitTimeMaxDisplayBlocked ( DWORD dwWaitTimeMaxDisplayBlocked )
{
    m_dwWaitTimeMaxDisplayBlocked = dwWaitTimeMaxDisplayBlocked;
}

void CBlockedAppUI::SetForcedAction ( DWORD dwForcedAction )
{
    m_dwForcedAction = dwForcedAction;
}

CBlockedAppUIHelp::CBlockedAppUIHelp(DWORD dwHelpID)
{
    m_dwHelpID = dwHelpID;
}

bool CBlockedAppUIHelp::Run(HWND hWndParent, unsigned long nID, cc::INotify* pAlert)
{
    NAVToolbox::CNAVHelpLauncher Help;
    Help.LaunchHelp ( m_dwHelpID );
    return true;
}

CBlockedAppUICloser::CBlockedAppUICloser ( cc::INotifyPtr& pUI, DWORD dwWaitTime )
{
    m_pUI = pUI;
    m_dwWaitTime = dwWaitTime;
}

CBlockedAppUICloser::~CBlockedAppUICloser ()
{
}

int CBlockedAppUICloser::Run ()
{
    CCTRACEI ( _T("CBlockedAppUICloser::Run - starting"));

    ccLib::CMessageLock msgLock (TRUE, TRUE);
    if ( WAIT_TIMEOUT == msgLock.Lock ( m_Terminating.GetHandle(), m_dwWaitTime ))
    {
        if ( m_pUI )
        {
            CCTRACEW ( _T("CBlockedAppUICloser::Run - hiding UI"));
            m_pUI->Hide ();
        }
        else
            CCTRACEW ( _T("CBlockedAppUICloser::Run - UI gone before we could close it"));
    }

    CCTRACEI ( _T("CBlockedAppUICloser::Run - closing"));

    return 0; // No errors
}
