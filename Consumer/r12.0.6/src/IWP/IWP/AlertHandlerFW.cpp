#include "StdAfx.h"
#include ".\alerthandlerfw.h"

#include "resourcehelper.h"         // For LoadString
#include "Resource.h"
#include "ccVersionInfo.h"        // CC
#include "NAVInfo.h"            // For launching scans
#include "IWPThreatLevel.h"

using namespace ccEvtMgr;
using namespace SymNeti;
using namespace cc;
using namespace ccFirewallSettings;

// static
CProtocolHelper CAlertHandlerFW::m_protocols;

CAlertHandlerFW::CAlertHandlerFW(void)
{
    m_bAppIsKnown = false;
    m_pSNEvent = NULL;
    m_eAlertType = (IAlertData::ALERT_TYPE) 0;
}

CAlertHandlerFW::~CAlertHandlerFW(void)
{
}

void CAlertHandlerFW::commonInit (void)
{
    if ( m_pSNEvent )
    {
        // Get app info data
        //
        char szFilePath[MAX_PATH+1] = {0};
        m_pSNEvent->GetAlertInfo(CSNAlertEventEx::eFilePath, &szFilePath[0], MAX_PATH );

        // string assignment might throw
        try 
        {
            m_strFilePath = szFilePath;

            // Get the app name, if we don't have it already
            //
            if ( m_strAppName.empty () )
            {
		        // Check for a version description in the EXE
		        m_bAppIsKnown = getAppRealName (szFilePath, m_strAppName);
            }
        }
        catch(std::exception& err)
        {
            CCTRACEE ("CAlertHandlerFW::commonInit - exception setting file path - %s", err.what());
        }
    }
}

// Handles IP, Listen and Trojan Horse events
//
int CAlertHandlerFW::displayCommonAlert()
{
	if ( !m_pSNEvent )
        return IAlertData::NO_ACTION;

	int nOptionIndex = 0;
    cc::IAlert::ALERTTHREATLEVEL threatlevel;

    // Get threat level - do this first so we can return quickly for
    // signed or virus infected threats.
    //
    CIWPThreatLevel::IWP_THREAT_LEVEL iThreatLevel = CIWPThreatLevel::IWP_THREAT_MEDIUM;
    
    // Don't bother using TLevel if there's no file name.
    //
    if ( !m_strFilePath.empty () && 0 != stricmp (m_strFilePath.c_str(), "n/a"))
    {
        // Minty fresh scoping of ThreatLevel since it has the scanner open
        //
        CIWPThreatLevel ThreatLevel;
        CCTRACEI ("CAlertHandlerFW::displayCommonAlert - loading IWPThreatLevel - %s", m_strFilePath.c_str());
        if (ThreatLevel.Initialize ())
        {
            // If this fails it means we can't get accurate threat info. 
            // We default to HIGH so we don't check the return code.
            //
            CCTRACEI ("CAlertHandlerFW::displayCommonAlert - GetThreatLevel");
            ThreatLevel.GetThreatLevel(m_strFilePath.c_str(),
                                       iThreatLevel); 
        }
        else
            CCTRACEE ("CAlertHandlerFW::displayCommonAlert - failed loading IWPThreatLevel");

		switch (iThreatLevel)
		{
        case CIWPThreatLevel::IWP_THREAT_LOW: // "Low Risk", Permit - Has ALE or a Symantec signature
            return IAlertData::PERMIT_ALL;
			break;
    		
		case CIWPThreatLevel::IWP_THREAT_HIGH:	// "High Risk", no options, will Block - virus, etc.
            {
                launchScan ( m_strFilePath.c_str() );
                return IAlertData::BLOCK_ONCE;      // Don't create a rule because we might be able to repair this.
            }
			break;
    		
		default:
		case CIWPThreatLevel::IWP_THREAT_MEDIUM:    // "Medium Risk", Permit, Bold, (Recommended) - unknown app, etc.
			threatlevel = cc::IAlert::THREAT_MED;
			break;
		}

        CCTRACEI ("CAlertHandlerFW::displayCommonAlert - unloading IWPThreatLevel - %d", iThreatLevel);
    }

    int nInitSelectOption = 0;

	// Get the direction
    //
	CSNAlertEventEx::eDirection iDirection;
	m_pSNEvent->GetDirection(&iDirection);
    
    if(iDirection == CSNAlertEventEx::eDirectionOutbound)
	{
        return IAlertData::NO_ACTION;   // This should never happen, just ignore the traffic.
	}

    // Start setting up the alert. If we get here then we have filtered out all the 
    // bad traffic or silent-response traffic.
    //

    std::string strDetails;
    std::string strTitle;
    std::string strDirection;

    cc::IAlert* pAlert = m_alert.GetAlert ();

	char szDate[40];
	char szTime[40];
    std::string strDateKey;
    std::string strTimeKey;

    // Get time/date and add those rows
    //
    {
	    FILETIME ftTime = {0};
    	FILETIME	localTime;
	    SYSTEMTIME	sysTime;

	    m_pSNEvent->GetAlertTime(&ftTime);

	    //
	    // Show what time the alert happened.
	    //
	    FileTimeToLocalFileTime(&ftTime, &localTime);
	    FileTimeToSystemTime(&localTime, &sysTime);

		GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, szDate, sizeof(szDate));
		GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &sysTime, NULL, szTime, sizeof(szTime));

        CResourceHelper::LoadString ( IDS_DATE, strDateKey, g_hInstance );
        CResourceHelper::LoadString ( IDS_TIME, strTimeKey, g_hInstance );

        pAlert->AddTableRow ( strDateKey.c_str(), szDate );
        pAlert->AddTableRow ( strTimeKey.c_str(), szTime );
	}

    // File Path
    std::string strPathKey;
    CResourceHelper::LoadString ( IDS_PATH, strPathKey, g_hInstance );
    pAlert->AddTableRowPath ( strPathKey.c_str(), m_strFilePath.c_str() );

    // File name
    std::string strFileNameKey;
    CResourceHelper::LoadString ( IDS_FILENAME, strFileNameKey, g_hInstance);
    pAlert->AddTableRowPath ( strFileNameKey.c_str(), m_strAppName.c_str() );

    // Direction
    if ( iDirection == CSNAlertEventEx::eDirectionEither )
    {
        pAlert->AddTableRow ( g_hInstance, IDS_DIRECTION, IDS_EITHER_DESCR );
    }
    else
    {
        pAlert->AddTableRow ( g_hInstance, IDS_DIRECTION, IDS_INBOUND_DESCR );
    }

    // Get the IP/port information
    //
    std::string strLocalIPKey;
    std::string strRemoteIPKey;
    std::string strLocalPortKey;
    std::string strRemotePortKey;
    std::string strLocalPortName;
    std::string strRemotePortName;
    std::string strLocalIP;
    std::string strRemoteIP;
    DWORD dwProtocol = 0;

	m_pSNEvent->GetProtocol((CSNAlertEventEx::eProtocol*)&dwProtocol);
    CResourceHelper::LoadString ( IDS_LOCAL_PORT, strLocalPortKey, g_hInstance );
    CResourceHelper::LoadString ( IDS_REMOTE_PORT, strRemotePortKey, g_hInstance );

    // Local IP/Port formatting
    //
    {
        char szLocalIPAddress [255] = {0};
        DWORD dwLocalServicePort = 0;
        m_pSNEvent->GetLocalIP(szLocalIPAddress, 255);
	    m_pSNEvent->GetLocalPort(&dwLocalServicePort);
        
        // If there is no local IP, call it "All Local"
        //
        if (0 == strcmp(szLocalIPAddress, "0.0.0.0"))
            CResourceHelper::LoadString ( IDS_ALL_LOCAL_ADAPTERS, strLocalIP, g_hInstance );
        else
            strLocalIP = szLocalIPAddress;

        // IP
        CResourceHelper::LoadString ( IDS_LOCAL_ADDRESS, strLocalIPKey, g_hInstance );
        pAlert->AddTableRow ( strLocalIPKey.c_str(), strLocalIP.c_str() );            

        // Port
        formatPort ( strLocalPortName, dwLocalServicePort, dwProtocol );
        pAlert->AddTableRow ( strLocalPortKey.c_str(), strLocalPortName.c_str());
    }

    // Remote IP/port formatting
    //
	// If this is a listen alert DON'T get the remote adress, because there is none
	if( m_eAlertType != IAlertData::ISALERTTYPE_LISTEN )
    {
        // IP
        //
        char szRemoteIPAddress [255] = {0};

	    m_pSNEvent->GetRemoteIP(szRemoteIPAddress, 255);

        // Is there a remote IP? If not, just don't show anything
        //
        if (0 != strcmp(szRemoteIPAddress, "0.0.0.0"))
        {
            CResourceHelper::LoadString ( IDS_REMOTE_ADDRESS, strRemoteIPKey, g_hInstance );
            if ( '\0' == szRemoteIPAddress[0] )
                CResourceHelper::LoadString ( IDS_UNKNOWN, strRemoteIP, g_hInstance );
            else
                strRemoteIP = szRemoteIPAddress;

            pAlert->AddTableRow ( strRemoteIPKey.c_str(), strRemoteIP.c_str() );            

            // Port
            //
            DWORD dwRemoteServicePort = 0;
	        m_pSNEvent->GetRemotePort(&dwRemoteServicePort);
            formatPort ( strRemotePortName, dwRemoteServicePort, dwProtocol );
            pAlert->AddTableRow ( strRemotePortKey.c_str(), strRemotePortName.c_str());
        }
    }

    std::string strProtocolKey;
    std::string strProtocol;

    if ( m_protocols.GetProtocolName ( strProtocol, dwProtocol ))
    {
        CResourceHelper::LoadString ( IDS_PROTOCOL, strProtocolKey, g_hInstance );
        pAlert->AddTableRow ( strProtocolKey.c_str(), strProtocol.c_str() );
    }
    
    //
    // ****************** End of Table *************************

    // Read to process ID that generated the traffic
    DWORD dwTrafficProcID = 0;
    m_pSNEvent->GetAlertInfo(CSNAlertEventEx::eProcessId, &dwTrafficProcID, sizeof( dwTrafficProcID ) );
    if ( dwTrafficProcID )
        pAlert->SetProperity(cc::IAlert::PROCESS_ID_EXCLUSION, dwTrafficProcID);
    
    // Description
    //
    m_alert.GetAlert()->SetBriefDesc ( m_strDescription.c_str() );

    // Return the UI return action.
    //
    return m_alert.DisplayAlert(NULL, HWND_TOPMOST);
}


bool CAlertHandlerFW::isNetBIOS ( DWORD dwLocalPort, DWORD dwRemotePort )
{
	return ((dwLocalPort == 137 || dwLocalPort == 138 || dwLocalPort == 139 ||
		    dwRemotePort == 137 || dwRemotePort== 138 || dwRemotePort== 139));
}

bool CAlertHandlerFW::getAppRealName ( const std::string& strAppFullPath /*in*/, std::string& strAppRealName /*out*/ )
{
	// Is the path already known to be wrong?
    //
    if ( 0 == strAppFullPath.compare ( "N/A" ))
    {
        strAppRealName = strAppFullPath;
        return false;
    }
    
    // Check for a version description in the EXE
    char szTempAppName[MAX_PATH] = {0};
	ccLib::CVersionInfo vi;

	if(vi.Load(strAppFullPath.c_str()))
    {
		strAppRealName = vi.GetFileDescription();
        return true;
    }

	// There's no good file name
    //
    if(strcmpi(strAppFullPath.c_str(), "System") == 0) // does this translate??
	{
        CResourceHelper::LoadString ( IDS_WINDOWSSUBSYSTEM, strAppRealName, g_hInstance );
        return false;
	}
	else
    {
        // If we can't figure it out, use the EXE name.
        //
        char pcFileName [_MAX_FNAME] = {0};
        char pcExtension [_MAX_EXT] = {0};            
        _splitpath ( strAppFullPath.c_str(), NULL, NULL, pcFileName, pcExtension );
        strAppRealName = pcFileName;
        strAppRealName += pcExtension; // e.g. ".exe"
        return false;
    }
}

void CAlertHandlerFW::formatPort ( std::string& strPortOut /*in/out*/, DWORD dwPortIn, int iProtocol )
{
    // Do we care about the protocol? (TCP,UDP,ICMP,IP)
    //
    if(iProtocol == IPPROTO_ICMP)
        getICMPCommandName ( strPortOut, dwPortIn );
    else
    {
        if ( !m_protocols.GetPortName ( strPortOut, dwPortIn ))
        {
            // No common port name
            char szTemp[100] = {0};
            _snprintf ( szTemp, 99, "%d", dwPortIn);
            strPortOut = szTemp;
            return;
        }
    }

	// If we have a string protocol put port number in perens
    //
    char szTemp[100] = {0};
    _snprintf ( szTemp, 99, "%s (%d)", strPortOut.c_str(), dwPortIn );
    strPortOut = szTemp;
}

void CAlertHandlerFW::getICMPCommandName ( std::string& strCommandOut/*in/out*/, DWORD dwCommandIn )
{
    int iResID = 0;

    switch ( dwCommandIn )
    {
    case 0:
        iResID = IDS_ICMP_ECHO_REPLY;
        break;
    case 3:
	    iResID = IDS_ICMP_DEST_UNREACHABLE;
        break;
    case 4:
	    iResID = IDS_ICMP_SOURCE_QUENCH;
        break;
    case 5:
	    iResID = IDS_ICMP_REDIRECT;
        break;
    case 8:
	    iResID = IDS_ICMP_ECHO_REQUEST;
        break;
    case 9:
	    iResID = IDS_ICMP_ROUTER_ADVERTISEMENT;
        break;
    case 10:
	    iResID = IDS_ICMP_ROUTER_SOLICITATION;
        break;
    case 11:
	    iResID = IDS_ICMP_TIME_EXCEEDED;
        break;
    case 12:
	    iResID = IDS_ICMP_PARAMETER_PROBLEM;
        break;
    case 13:
	    iResID = IDS_ICMP_TIMESTAMP_REQUEST;
        break;
    case 14:
	    iResID = IDS_ICMP_TIMESTAMP_REPLY;
        break;
    case 15:
	    iResID = IDS_ICMP_INFORMATION_REQUEST;
        break;
    case 16:
	    iResID = IDS_ICMP_INFORMATION_REPLAY;
        break;
    case 17:
	    iResID = IDS_ICMP_ADDRESSMASK_REQUEST;
        break;
    case 18:
	    iResID = IDS_ICMP_ADDRESSMASK_REPLY;
        break;
    default:
        iResID = IDS_UNKNOWN;
        break;
    }

    if ( iResID )
    {
        CResourceHelper::LoadString ( iResID, strCommandOut, g_hInstance );
    }
}

void CAlertHandlerFW::launchScan ( LPCSTR lpszFilePath )
{
    // Launch a scan
    // Command line is c:\progra~1\norton~1\navw32.exe <filepath>"
    //
    std::string strExePath;

    CNAVInfo NAVInfo;

    char szShortNAVDir [MAX_PATH] = {0};
    char szShortAppPath [MAX_PATH] = {0};

    ::GetShortPathName ( NAVInfo.GetNAVDir (), szShortNAVDir, MAX_PATH );
    ::GetShortPathName ( lpszFilePath, szShortAppPath, MAX_PATH );

    // Check to make sure the file exists first.
    //
    if ( INVALID_FILE_ATTRIBUTES == ::GetFileAttributes ( szShortAppPath ))
    {
        CCTRACEW ("CAlertHandlerFW::launchScan - file doesn't exist %s", szShortAppPath);
        return;
    }
    
    // Don't do GSER scans.
    strcat ( szShortAppPath, " /SE-");

    strExePath = szShortNAVDir;
    strExePath += _T("\\navw32.exe");

    HINSTANCE hResult = ::ShellExecute( ::GetDesktopWindow(),
                                        "open",         // Verb
	                                    strExePath.c_str(),     // File
                                        szShortAppPath,  // Parameters
                                        NULL,                   // Directory
	                                    SW_SHOWNORMAL );               // ShowCmd

	if (HINSTANCE (32) >= hResult )
    {
		CCTRACEE ("CAlertHandlerFW::launchScan - failed to launch scan %s %s", strExePath.c_str(),szShortAppPath );
    }
}
