// Notification.cpp: implementation of the CNotification class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\navapsvcres\resource.h"
#include "Notification.h"
#include "actions.h"
#include "AllNAVEvents.h"
#include "AVccModuleId.h"
#include "NAVVersion.h"
#include "SAVRTPacketHandlers.h"
#include <wtsapi32.h>
#include "const.h"

// We need to define our interface as a "Version".
#define THREAT_PACKET_VERSION 1
#define SEMISYNC_PACKET_VERSION 1

//////////////////////////////////////////////////////////////////////
// CNotification::CNotification()

// Just copy the data into a buffer and we will deal with it later. The NotificationQueue
// will do the unwrapping in another thread for performance.
//
CNotification::CNotification(const PCSAVRT_EVENTREPORTPACKET pPacket, DWORD dwBufferSize) :
    m_EventType( ThreatEvent )
{
    m_vecData.resize ( dwBufferSize );
    CopyMemory ( &m_vecData[0], pPacket, dwBufferSize);
}

CNotification::CNotification(const PSAVRT_CONTAINERREPORTPACKET pPacket, DWORD dwBufferSize) :
    m_EventType( ContainerReport )
{
    m_vecData.resize ( dwBufferSize );
    CopyMemory ( &m_vecData[0], pPacket, dwBufferSize);
}

CNotification::CNotification(const PSAVRT_SEMISYNCSCANNOTIFICATIONPACKET pPacket, DWORD dwBufferSize) :
    m_EventType( ContainerStatus )
{
    m_vecData.resize ( dwBufferSize );
    CopyMemory ( &m_vecData[0], pPacket, dwBufferSize);
}

CNotification::CNotification(const CEventData& eventData) :
    m_EventType( AlreadyMade )
{
    m_eventData = eventData;
}

CNotification::enumDataType CNotification::GetType ()
{
    return m_EventType;
}

//////////////////////////////////////////////////////////////////////
// CNotification::~CNotification()

CNotification::~CNotification()
{
	// Intentionally empty.
}

// Since we need to use extracted data from the SAVRT packet in both the service
// and the agent we should just "unpack" it now.
//
bool CNotification::MakeEvent ( CEventData& eventData, CAPThreatExclusions* pThreatExclusions /*NULL*/ )
{
    // Did we get data?
    if ( 0 == m_vecData.size () )
        return false;

    switch ( m_EventType )
    {
    case ThreatEvent:
        return makeThreat (eventData, pThreatExclusions);
        break;
    case ContainerReport:
        return makeThreatContainer (eventData, pThreatExclusions);
        break;
    case ContainerStatus:
        return makeContainerStatus (eventData);
        break;
    case AlreadyMade:
        if ( m_eventData.GetSize () )
        {
            eventData = m_eventData;
            return true;
        }
        else
            return false;
        break;
    default:
        return false;
        break;
    };
}

// Convert from SAVRT_EVENTREPORTPACKET to Event_ID_Threat
// Clients need to set the defs version since it's feature specific.
//
bool CNotification::makeThreat ( CEventData& eventData, CAPThreatExclusions* pThreatExclusions )
{
    CEventPacketHandler packetHandler;
    PSAVRT_EVENTREPORTPACKET pPacket = (PSAVRT_EVENTREPORTPACKET) &m_vecData[0];
    packetHandler.InitializeEventResults ( pPacket );

    // Initialize the packet handlers
    packetHandler.FirstEntry();

    // ************************************************
    // BASE DATA
    //
    eventData.SetData ( AV::Event_Base_propType, AV::Event_ID_Threat );

    // Threat event type. Figure out if this is not virus-like activity.
    //
    bool bVirusLike = true;

    switch ( pPacket->dwAVContext )
    {
    case AVCONTEXT_SCANFILE:
    case AVCONTEXT_SCANBOOT:
    case AVCONTEXT_SCANMBR:
    case AVCONTEXT_SCANMEMSTARTUP:
        bVirusLike = false;
        eventData.SetData ( AV::Event_Threat_propEventSubType, AV::Event_Threat );
        break;
    default:
        eventData.SetData ( AV::Event_Threat_propEventSubType, AV::Event_VirusLike );
        break;
    }

    // Gather data from packet.
    PSAVRT_USERINFOSUBPACKET pUserInfo = packetHandler.GetUserInfoSubPacket();

    DWORD dwFlags1 = packetHandler.GetFlags1();
    bool bUseActiveSession = false;
    if( (dwFlags1 & SAVRT_EPRP_FILE_ORIGIN_REMOTE) && 
        pUserInfo && pUserInfo->dwTerminalServerSessionID == 0 )
    {
        CCTRACEI("CNotification::MakeThreat() - This was initiated from a remote source. Using the active session ID to display the alert.");
        bUseActiveSession = true;
    }

    DecodeUserInfo( packetHandler.GetUserInfoSubPacket(), eventData, bUseActiveSession );

    // What version of our packet are we sending?
    eventData.SetData ( AV::Event_Base_propVersion, THREAT_PACKET_VERSION );

    // The event is from us
    eventData.SetData ( AV::Event_Threat_propFeature, SAVRT_MODULE_ID_NAVAPSVC );

    CNAVVersion Version;
    eventData.SetData ( AV::Event_Threat_propProductVersion, Version.GetPublicRevision ());

    // ************************************************
    // SUBFILE DATA
    //
    CEventData SubFile;

	switch( pPacket->dwAVContext )
	{
	case AVCONTEXT_SCANFILE:
        eventData.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );
        SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, packetHandler.GetFileInTrouble () );
		break;

	case AVCONTEXT_SCANBOOT:
        eventData.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_BootRecord );
        SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, packetHandler.GetDeviceInTrouble () );
    	break;

	case AVCONTEXT_SCANMBR:
		{
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, packetHandler.GetDeviceInTrouble () );
            eventData.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_MasterBootRecord );
		}
		break;

    // Virus-like
    case AVCONTEXT_HDFORMAT:
    case AVCONTEXT_HDMBRWRITE:
    case AVCONTEXT_HDBOOTWRITE:
    case AVCONTEXT_FDBOOTWRITE:
        eventData.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );
        SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, packetHandler.GetDeviceInTrouble () );
        SubFile.SetData ( AV::Event_ThreatSubFile_propApplicationPath, packetHandler.GetFileCausingTrouble());
        break;

    //case AVCONTEXT_WRITETOFILE: // Unsupported
    //case AVCONTEXT_FILEROATTRCHANGE: // Unsupported
	default:
        CCTRACEE ( "CNotification::makeThreat - unknown context" );
		_ASSERT( FALSE );
        return false;
		break;
	}

    // Convert actions
    //
    std::vector <BYTE> vecActions;

    if ( AVACTION_FIRST < pPacket->Action1.dwAction && AVACTION_LAST > pPacket->Action1.dwAction )
        vecActions.push_back( (BYTE)decodeNavAPAction (pPacket->Action1.dwAction, pPacket->Action1.dwSuccess, bVirusLike ));

    if ( AVACTION_FIRST < pPacket->Action2.dwAction && AVACTION_LAST > pPacket->Action2.dwAction )
        vecActions.push_back( (BYTE)decodeNavAPAction (pPacket->Action2.dwAction, pPacket->Action2.dwSuccess, bVirusLike ));

    if ( AVACTION_FIRST < pPacket->Action3.dwAction && AVACTION_LAST > pPacket->Action3.dwAction )
        vecActions.push_back( (BYTE)decodeNavAPAction (pPacket->Action3.dwAction, pPacket->Action3.dwSuccess, bVirusLike ));

    // ************************************************
    // THREAT DATA
    //
    CEventData AllThreats;

    if ( bVirusLike )
    {
        // ************************************************
        // Virus-like special handling. Virus-like packets
        // from SAVRT have no threat entries
        //

        // Make a temp threat
        CEventData threat;

        threat.SetData ( AV::Event_ThreatEntry_propActionCount, vecActions.size() );
        threat.SetData ( AV::Event_ThreatEntry_propActionData, &vecActions[0], vecActions.size() );

        // Virus-like Attempted actions
        switch (pPacket->dwAVContext)
        {
        case AVCONTEXT_HDFORMAT:
            threat.SetData ( AV::Event_ThreatEntry_propAttempted_Action, AV::Event_VirusLike_Attempted_Action_HD_Format );
            break;
        case AVCONTEXT_HDMBRWRITE:
            threat.SetData ( AV::Event_ThreatEntry_propAttempted_Action, AV::Event_VirusLike_Attempted_Action_HD_MBR_Write);
            break;
        case AVCONTEXT_HDBOOTWRITE:
            threat.SetData ( AV::Event_ThreatEntry_propAttempted_Action, AV::Event_VirusLike_Attempted_Action_HD_Boot_Write );
            break;
        case AVCONTEXT_FDBOOTWRITE:
            threat.SetData ( AV::Event_ThreatEntry_propAttempted_Action, AV::Event_VirusLike_Attempted_Action_FD_Boot_Write );
            break;
        }

        AllThreats.SetNode ( 0, threat );
    }
    else
    {
        //*******************************************
        // Virus events
        //

        // We might exclude entries so keep track of the final result
        DWORD dwNumFinalEntries = 0;

        for ( DWORD dwEntry = 0; dwEntry < pPacket->dwEntryCount; dwEntry++ )
        {
            if ( dwEntry != 0 )
                if ( -1 == packetHandler.IncrementEntry())
                    break;

            // Make a temp threat
            CEventData threat;

            // Virus name and ID of the first entry
            //
            WCHAR szVirusName [1] = {0};
            LPWSTR lpszVirusName = szVirusName;
            DWORD dwVirusID = 0;
            packetHandler.GetEntryVirusInfo ( &lpszVirusName, &dwVirusID );

            threat.SetData ( AV::Event_ThreatEntry_propVirusID, dwVirusID );
            threat.SetData ( AV::Event_ThreatEntry_propVirusName, lpszVirusName );

            // Set threat categories
            DWORD dwCatCount = 0;
            LPDWORD pdwThreatCats = 0;
            packetHandler.GetEntryCategories( &dwCatCount, &pdwThreatCats);

            // Set threat flags
            DWORD dwThreatFlags = 0;
            threat.SetData ( AV::Event_ThreatEntry_propThreatFlags, packetHandler.GetEntryThreatFlags (&dwThreatFlags));
#ifdef AP_SPYWARE_SUPPORT
            // Threat cats 0-4 are all viruses
            //
            if ( dwCatCount > 0 && *pdwThreatCats > 4)
            {
                // Check category exclusions
                if(pThreatExclusions && 
                    !pThreatExclusions->IsExcluded(dwVirusID, 
                        packetHandler.GetFileInTrouble(), dwCatCount, pdwThreatCats))
                {
/*                if ( pThreatExclusions && !pThreatExclusions->IsThreatCatExcluded (dwCatCount, pdwThreatCats ))
                {
                    // Get the SFN for exclusion checking
                    CAtlStringW wstrSFN;
                    DWORD dwBufLen = (wcslen(packetHandler.GetFileInTrouble())*2) +1;
                    DWORD dwRet = GetShortPathNameW(packetHandler.GetFileInTrouble(), wstrSFN.GetBuffer(dwBufLen), dwBufLen);
                    wstrSFN.ReleaseBuffer();

                    if( dwRet == 0 || dwRet > dwBufLen )
                    {
                        CCTRACEI ("CNotification::makeThreat() - SFN path could not be obtained for %ls", packetHandler.GetFileInTrouble());
                        wstrSFN = packetHandler.GetFileInTrouble();
                    }

                    // Setup a temporary buffer for the OEM conversion
                    std::vector<char> vOemBuff;
                    int iBuffSize = (wcslen(wstrSFN)*2) + 1;
	                vOemBuff.reserve( iBuffSize );

	                // Convert the wstring to an OEM string
                    WideCharToMultiByte( CP_OEMCP, 0,
                                        wstrSFN, -1,
                                        &vOemBuff[0], iBuffSize, NULL, NULL );

                    // Check file exclusions (only check if there are non-virals)
                    if ( pThreatExclusions && !pThreatExclusions->IsExcluded (&vOemBuff[0]))
                    { */

                    // Convert the DWORD array of threat cats to a space delimited string
                    std::string strThreatCats;
                    char szBuff[10] = {0};

                    for ( DWORD dwLoop = 0; dwLoop < dwCatCount; dwLoop ++)
                    {
                        ultoa ( pdwThreatCats[dwLoop], szBuff, 10);
                        strThreatCats += szBuff;
                        if (dwLoop != dwCatCount-1 )
                            strThreatCats += " ";
                    }

                    threat.SetData ( AV::Event_ThreatEntry_propThreatCatCount, dwCatCount );
                    threat.SetData ( AV::Event_ThreatEntry_propThreatCategories, strThreatCats.c_str() );

                    // This is spyware so the action is always just "Detected"
                    BYTE byAction = AV::Event_Action_NoActionTaken;
                    threat.SetData ( AV::Event_ThreatEntry_propActionCount, 1);
                    threat.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1);

                    // Add this threat to the list (1,2,3,etc.)
                    AllThreats.SetNode ( dwNumFinalEntries, threat );

                    dwNumFinalEntries++;
                }
                else
                    CCTRACEI ("CNotification::makeThreat - item was excluded");

            }
            else
#endif //AP_SPYWARE_SUPPORT
            {
                // If this is a virus, copy the file actions to each virus.
                // In reality, we can have 2 viruses, one repaired, one not. SAVRT doesn't
                // report the data this way. We only get a summary of the file.

                threat.SetData ( AV::Event_ThreatEntry_propActionCount, vecActions.size ());
                threat.SetData ( AV::Event_ThreatEntry_propActionData, &vecActions[0], vecActions.size ());

                // Add this threat to the list (1,2,3,etc.)
                AllThreats.SetNode ( dwNumFinalEntries, threat );

                dwNumFinalEntries++;
            }
        }

        // Were all threats excluded??
        if (0 == dwNumFinalEntries)
            return false;
    }

    SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);
    CEventData AllFiles;
    AllFiles.SetNode ( 0, SubFile );
    eventData.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

    return true;
}

// We don't bother with virus-like there will never be one in a container, and container
// scans aren't implemented on 9x.
// Clients need to set the virus def revision since it's feature specific.
//
bool CNotification::makeThreatContainer ( CEventData& eventData, CAPThreatExclusions* pThreatExclusions )
{
#ifdef AP_COMPRESSED_SUPPORT
    CContainerPacketHandler packetHandler;
    PSAVRT_CONTAINERREPORTPACKET pPacket = (PSAVRT_CONTAINERREPORTPACKET) &m_vecData[0];
    packetHandler.InitializeContainerResults ( pPacket );

    // ************************************************
    // BASE DATA
    //
    eventData.SetData ( AV::Event_Base_propType, AV::Event_ID_Threat );

    // threat event type
    eventData.SetData ( AV::Event_Threat_propEventSubType, AV::Event_Threat_Container );

    // Gather data from packet.
    DecodeUserInfo( packetHandler.GetUserInfoSubPacket(), eventData, false );

    // What version of our packet are we sending?
    eventData.SetData ( AV::Event_Base_propVersion, THREAT_PACKET_VERSION );

    // The event is from us
    eventData.SetData ( AV::Event_Threat_propFeature, SAVRT_MODULE_ID_NAVAPSVC );

    // always a file (not BR, MBR)
    eventData.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );

    CNAVVersion Version;
    eventData.SetData ( AV::Event_Threat_propProductVersion, Version.GetPublicRevision ());

    // ************************************************
    // SUBFILE DATA - first sub file is always the base container!!
    //
    CEventData AllFiles;

    // 0 = uninitialized
    // 1 = excluded
    // 2 = not excluded
    //
    DWORD dwParentContainerExcluded = 0;

    // Do the parent file action (whole container was deleted, etc.)
    {
        CEventData parentfile;

        // File name - we only care about the parent (above) and the final file name here.
        //
        parentfile.SetData ( AV::Event_ThreatSubFile_propFileName, packetHandler.GetContainerName());

        /* // we don't use the action for the parent so I'm not builing it.
        CEventData eventThreat;
        eventThreat.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );

        std::vector <BYTE> vecParentAction;
        vecParentAction.push_back ( decodeNavAPAction(packetHandler.GetContainerAction(), packetHandler.GetContainerActionResultFlags()));
        eventThreat.SetData ( AV::Event_ThreatEntry_propActionData, &vecParentAction[0], vecParentAction.size () );

        // Save
        CEventData AllThreats;
        AllThreats.SetNode (0, eventThreat);
        parentfile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);
        */

        AllFiles.SetNode ( 0, parentfile );
    } // end parent file

    // We might exclude entries so keep track of the final result
    // Start at #1 since we already added the parent
    DWORD dwNumFinalEntries = 1;

    for ( DWORD dwFileIter = 1; dwFileIter < packetHandler.GetEntriesCount()+1; dwFileIter++)
    {
        // Initialize the packet handlers
        int iHandlerResult = -1;

        if ( 1 == dwFileIter )
            iHandlerResult = packetHandler.FirstEntry();
        else
            iHandlerResult = packetHandler.IncrementEntry();

        if ( -1 == iHandlerResult )
            break;

        CEventData SubFile;

        // file name
        LPWSTR lpwFileName = NULL;
        WCHAR szFileName [1] = {0}; // packet handler should really just return a const...
        LPWSTR lpszFileName = szFileName;
        packetHandler.GetEntryFileNameComponent(0, &lpszFileName);

        // No filename? Not a valid entry.
        if ( !lpszFileName )
            break;

        SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, lpszFileName );

        // Convert action (only one for containers)
        //
        // Actions for compressed files are reported differently than for regular files.
        // There is only one action which is the final action taken on the file. The
        // success value is always true. See defect # 1-3G7O8D.
        //
        std::vector <BYTE> vecActions;
        DWORD dwAction = 0;
        DWORD dwActionResult = 0;
        packetHandler.GetEntryActionFlags (&dwAction, &dwActionResult);
        vecActions.push_back( (BYTE) decodeNavAPAction (dwAction, true ));

        // ************************************************
        // THREAT DATA
        //
        CEventData AllThreats;

        // Containers only have one threat per file, as described by SAVRT
        //
        // Make a temp threat
        CEventData threat;

        // Virus name and ID of the first entry
        //
        WCHAR szVirusName [1] = {0};
        LPWSTR lpszVirusName = szVirusName;
        DWORD dwVirusID = 0;
        packetHandler.GetEntryVirusInfo ( &lpszVirusName, &dwVirusID );

        threat.SetData ( AV::Event_ThreatEntry_propVirusID, dwVirusID );
        threat.SetData ( AV::Event_ThreatEntry_propVirusName, lpszVirusName );

        DWORD dwThreatFlags = 0;
        threat.SetData ( AV::Event_ThreatEntry_propThreatFlags, packetHandler.GetEntryMappedThreatFlags (&dwThreatFlags));

#ifdef AP_SPYWARE_SUPPORT
        // Set threat categories
        DWORD dwCatCount = 0;
        LPDWORD pdwThreatCats = 0;
        packetHandler.GetEntryCategories( &dwCatCount, &pdwThreatCats);
        if ( dwCatCount > 0 && *pdwThreatCats > 4)
        {
            // Check exclusions on the parent container
            if (  0 == dwParentContainerExcluded )
            {
                if(pThreatExclusions && 
                    pThreatExclusions->IsExcluded(0, packetHandler.GetContainerName(), 0, NULL))
                {
                    // Excluded
                    dwParentContainerExcluded = 1;
                    CCTRACEI ("CNotification::makeThreatContainer - parent file excluded from threat cat");
                }
                else
                    dwParentContainerExcluded = 2;

                /*                
                int iParentBuffSize = (wcslen (packetHandler.GetContainerName())*2) + 1;
	            std::vector<char> vParentOemBuff;
                vParentOemBuff.reserve( iParentBuffSize );

	            // Convert the wstring to an OEM string
                WideCharToMultiByte( CP_OEMCP, 0,
                                    packetHandler.GetContainerName(), -1,
                                    &vParentOemBuff[0], iParentBuffSize, NULL, NULL );
                */
            }

            // Is the parent container not excluded?
            if ( 2 == dwParentContainerExcluded )
            {
                // Check all exclusion criteria
                if( pThreatExclusions && 
                    !pThreatExclusions->IsExcluded(dwVirusID, szFileName, dwCatCount, pdwThreatCats) )
                {
/*                if ( pThreatExclusions && !pThreatExclusions->IsThreatCatExcluded (dwCatCount, pdwThreatCats))
                {
                    // Setup a temporary buffer
                    std::vector<char> vOemBuff;
                    int iBuffSize = (wcslen (szFileName)*2) + 1;
	                vOemBuff.reserve( iBuffSize );

	                // Convert the wstring to an OEM string
                    WideCharToMultiByte( CP_OEMCP, 0,
                                        szFileName, -1,
                                        &vOemBuff[0], iBuffSize, NULL, NULL );

                    // Check file exclusions (only check if there are non-virals)
                    if ( pThreatExclusions && !pThreatExclusions->IsExcluded (&vOemBuff[0]))
                    { */

                    // Convert the DWORD array of threat cats to a space delimited string
                    std::string strThreatCats;
                    char szBuff[10] = {0};

                    for ( DWORD dwLoop = 0; dwLoop < dwCatCount; dwLoop ++)
                    {
                        ultoa ( pdwThreatCats[dwLoop], szBuff, 10);
                        strThreatCats += szBuff;
                        if (dwLoop != dwCatCount-1 )
                            strThreatCats += " ";
                    }

                    threat.SetData ( AV::Event_ThreatEntry_propThreatCatCount, dwCatCount );
                    threat.SetData ( AV::Event_ThreatEntry_propThreatCategories, strThreatCats.c_str());

                    // This is spyware so the action is always just "Detected"
                    BYTE byAction = AV::Event_Action_NoActionTaken;
                    threat.SetData ( AV::Event_ThreatEntry_propActionCount, 1);
                    threat.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1);

                    // Add this threat to the list
                    AllThreats.SetNode ( 0, threat );

                    // If there were no entries don't add this to the list
                    //
                    SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);
                    AllFiles.SetNode ( dwNumFinalEntries, SubFile );
                    dwNumFinalEntries++;
                }
            }
            else
                CCTRACEI ("CNotification::makeThreatContainer - threat was excluded");
        }
        else
#endif //AP_SPYWARE_SUPPORT
        {
            // If this is a virus, copy the file actions to each virus.
            // In reality, we can have 2 viruses, one repaired, one not. SAVRT doesn't
            // report the data this way. We only get a summary of the file.

            threat.SetData ( AV::Event_ThreatEntry_propActionCount, vecActions.size ());
            threat.SetData ( AV::Event_ThreatEntry_propActionData, &vecActions[0], vecActions.size ());

            // Add this threat to the list
            AllThreats.SetNode ( 0, threat );

            // If there were no entries don't add this to the list
            //
            SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);
            AllFiles.SetNode ( dwNumFinalEntries, SubFile );
            dwNumFinalEntries++;
        }
    } // End subfiles

    if ( 0 == dwNumFinalEntries )
        return false;

    eventData.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );
#endif //AP_COMPRESSED_SUPPORT

    return true;
}


bool CNotification::makeContainerStatus ( CEventData& eventData )
{
    PSAVRT_SEMISYNCSCANNOTIFICATIONPACKET pPacket = (PSAVRT_SEMISYNCSCANNOTIFICATIONPACKET) &m_vecData[0];

    // Gather data from packet.
	DecodeUserInfo( &pPacket->rUser, eventData, false );

    eventData.SetData ( AV::Event_Base_propType, AV::Event_ID_Container_ScanStatus );
    eventData.SetData ( AV::Event_Container_Status, convertSAVRTStatustoNAVStatus ( pPacket->eStatus ) );
    eventData.SetData ( AV::Event_Base_propVersion, SEMISYNC_PACKET_VERSION );

    // Store the cookie.
    //
    long lSize = sizeof (SAVRT_ROUS64);
    eventData.SetData ( AV::Event_Container_Cookie, (BYTE*) &pPacket->hCookie, lSize );

    eventData.SetData ( AV::Event_Container_ObjectName, &pPacket->szFile[0] );

    eventData.SetData( AV::Event_Container_ProcessID, (long) pPacket->ullProcessId );

    return true;
}

//////////////////////////////////////////////////////////////////////
// CNotification::decodeUserInfo()

void CNotification::DecodeUserInfo( PSAVRT_USERINFOSUBPACKET pUserInfo, CEventData& eventData, bool bUseActiveSession )
{
    if( pUserInfo == NULL )
    {
        CCTRACEI ( "CNotification::DecodeUserInfo - User info is null, using active session and default user name." );
        eventData.SetData ( AV::Event_Base_propSessionID, getActiveSessionID() );
        eventData.SetData (AV::Event_Base_propUserName, DEFAULT_USER_NAME );
        return;
    }

    // Save off terminal server session ID.
    DWORD dwSession = pUserInfo->dwTerminalServerSessionID;
    if( bUseActiveSession )
    {
        dwSession = getActiveSessionID();
    }

    eventData.SetData ( AV::Event_Base_propSessionID, dwSession );
    CCTRACEI ( _T("NAVAPSVC - Session ID = %d"), dwSession );
    std::wstring sUserName;

	if( pUserInfo->bUseAsSIDInsteadOfString )
	{
		SID_NAME_USE use;
		DWORD dwUserSize = 0;
		DWORD dwDomainSize = 0;
        std::vector<WCHAR> vDomainBuffer;

		// First call to this routine will fail, but we need to
		// get the sizes of return buffers.
		LookupAccountSidW( NULL, &(pUserInfo->rUserInfo.Sid),
						  NULL, &dwUserSize,
						  NULL, &dwDomainSize,
						  &use );

		// Resize vectors.
		sUserName.reserve( dwUserSize );
		vDomainBuffer.reserve( dwDomainSize );

		if( LookupAccountSidW( NULL, &(pUserInfo->rUserInfo.Sid),
						  &sUserName[0], &dwUserSize,
						  &vDomainBuffer[0], &dwDomainSize,
						  &use ) == FALSE)
		{
            // Couldn't figure out who this is.
			WCHAR szBuffer[255];
			LoadStringW( _Module.GetResourceInstance(), IDS_UNKNOWN_USER, szBuffer, 255 );
			sUserName = szBuffer;
		}

        CCTRACEI ( _T("NAVAPSVC - UserInfo as SID User = %ls"), sUserName.c_str() );
	}
	else
	{
		// UNC style string is in the form "\\domain\user" or "user"
		std::wstring sTemp = pUserInfo->rUserInfo.szUserID;

		// Extract username.
		std::wstring::size_type pos = sTemp.find_last_of( _T('\\') );
        sUserName = ( pos == std::string::npos ) ? sTemp : sTemp.substr( pos + 1 );

        CCTRACEI ( _T("NAVAPSVC - UserInfo as String User = %ls"), sUserName.c_str() );
	}

    eventData.SetData ( AV::Event_Base_propUserName, sUserName.c_str() );
}

//////////////////////////////////////////////////////////////////////
// CNotification::getActiveSessionID()

DWORD CNotification::getActiveSessionID()
{
    // Need to dynamic link to wtsapi32.dll for WTSEnumerateSessions() 
    // function.
    HMODULE hMod = LoadLibrary( _T("wtsapi32.dll") );
    if( hMod == NULL )
        return 0;

    // get entry points.
    typedef BOOL (WINAPI *pfnWTSEnumerateSessions )( HANDLE, DWORD, DWORD, PWTS_SESSION_INFO*, DWORD* );
    typedef void (WINAPI *pfnWTSFreeMemory)(PVOID);
    pfnWTSEnumerateSessions pEnum = (pfnWTSEnumerateSessions) GetProcAddress( hMod, "WTSEnumerateSessionsW" );
    pfnWTSFreeMemory pFree = (pfnWTSFreeMemory) GetProcAddress( hMod, "WTSFreeMemory" );
    if( pEnum == NULL || pFree == NULL )
        return 0;

    // Call the function.
    DWORD dwCount = 0;
    PWTS_SESSION_INFO pSessionInfo = NULL;
    if( FALSE == pEnum( WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount ) )
        return 0;

    // Figure out which session is active.
    DWORD dwRet = 0;
    for( DWORD i = 0; i < dwCount; ++i)
    {
        if( pSessionInfo[i].State == WTSActive )
        {
            dwRet = pSessionInfo[i].SessionId;
            break;
        }
    }

    // Clean up.
    if( pSessionInfo )
        pFree( pSessionInfo );

    FreeLibrary( hMod );

    return dwRet;
}

//////////////////////////////////////////////////////////////////////
// CNotification::decodeNavAPAction
// Virus-like is for 9x
//
long CNotification::decodeNavAPAction( DWORD dwAPAction, DWORD dwSucceeded, bool bVirusLike /*false*/ )
{
    switch( dwAPAction )
	{
    case AVACTION_STOP:
        if ( bVirusLike )
        {
            if ( dwSucceeded )
                return AV::Event_Action_Blocked;
            else
                return AV::Event_Action_BlockFailed;
        }

        if ( dwSucceeded )
            return AV::Event_Action_Access_Denied;
        else
		    return AV::Event_Action_Allowed; // Unrepairable container items
        break;

    case AVACTION_REPAIR:
		if ( dwSucceeded )
            return AV::Event_Action_Repaired;
        else
            return AV::Event_Action_RepairFailed;
		break;

    case AVACTION_DELETE:
        if ( dwSucceeded )
            return AV::Event_Action_Deleted;
        else
		    return AV::Event_Action_DeleteFailed;
		break;

    case AVACTION_QUARANTINE:
        if ( dwSucceeded )
		    return AV::Event_Action_Quarantined;
        else
            return AV::Event_Action_QuarantinedFailed;
		break;

    case AVACTION_DESTROY:
        if ( dwSucceeded )
            return AV::Event_Action_Auto_Deleted;
        else
            return AV::Event_Action_DeleteFailed;

    case AVACTION_SHUTDOWN:
        return AV::Event_Action_Shutdown;
        break;

    case AVACTION_EXCLUDE:
        if ( dwSucceeded )
            return AV::Event_Action_Excluded;
        else
            return AV::Event_Action_Exclude_Failed;
        break;

    // Detected from SAVRT spyware and virus-like
    case AVACTION_CONTINUE:
        if ( bVirusLike )
            return AV::Event_Action_Allowed;
        else
            return AV::Event_Action_NoActionTaken;
        break;

    default:
        assert (false);
        return AV::Event_Action_Unknown;
		break;
	}
}



DWORD CNotification::convertSAVRTStatustoNAVStatus ( SAVRT_CONTAINERSTATUS dwSAVRTStatus )
{
    switch ( dwSAVRTStatus )
    {
        case Scanning:
            {
                CCTRACEI (_T("NAVAPSVC : Received container scan notification : START"));
                return AV::Event_Container_Status_STARTED;
            }
            break;
        case BlockingAccess:
            {
                CCTRACEI (_T("NAVAPSVC : Received container scan notification : BLOCKED"));
                return AV::Event_Container_Status_BLOCKED;
            }
            break;
        case ScanCompleted:
            {
                CCTRACEI (_T("NAVAPSVC : Received container scan notification : COMPLETED"));
                return AV::Event_Container_Status_COMPLETED;
            }
            break;
        case ScanCancelled:
            {
                CCTRACEI (_T("NAVAPSVC : Received container scan notification : CANCELLED"));
                return AV::Event_Container_Status_CANCELLED;
            }
            break;
        case ScanAborted:
            {
                CCTRACEI (_T("NAVAPSVC : Received container scan notification : ABORTED"));
                return AV::Event_Container_Status_ABORTED;
            }
            break;
        default:
            return -1;
            break;
    }
}