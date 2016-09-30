#include "stdafx.h"
#include ".\notifyapspywaredetections.h"
#include "ccModule.h"
#include "Savrt32.h"
#include "APOptNames.h"
#include "NAVInfo.h"
#include "OSInfo.h"
#include "AutoProtectWrapper.h"

// Headers for sending non-viral detection data to AP
#import "navapsvc.tlb" 					// For COM interface to AP service.
#include "navapcommands.h"				// Commands for service.
#include "AllNAVEvents.h"

CNotifyAPSpywareDetections::CNotifyAPSpywareDetections(void) : m_bShouldSend(true),
                                                               m_lCookie(0)
{
}

CNotifyAPSpywareDetections::~CNotifyAPSpywareDetections(void)
{
}

void CNotifyAPSpywareDetections::Initialize(bool bEmail)
{
    // Only need to notify on NT systems
    if (!COSInfo::IsWinNT())
    {
        CCTRACEI(_T("CNotifyAPSpywareDetections::Initialize() - This is not an NT platform no notifications will be sent to AP."));
        m_bShouldSend = false;
        return;
    }

    // Never send notification for email scans
    if( bEmail )
    {
        CCTRACEI(_T("CNotifyAPSpywareDetections::Initialize() - This is an email scan no notifications will be sent to AP."));
        m_bShouldSend = false;
        return;
    }

    //
    // Check to see if AP spyware scanning is enabled
    //
    CAutoProtectOptions savrtOptions;

    if( SAVRT_OPTS_OK == savrtOptions.Load() )
    {
        DWORD dwSpywareEnabled = SAVRT_THREAT_MODE_NONVIRAL;
        savrtOptions.GetDwordValue(AP_szNAVAPCFGdwRespondToThreats, &dwSpywareEnabled, SAVRT_THREAT_MODE_NONVIRAL);
        m_bShouldSend = (dwSpywareEnabled & SAVRT_THREAT_MODE_NONVIRAL) ? true : false;
        CCTRACEI(_T("CNotifyAPSpywareDetections::Initialize() - m_bShouldSend value is %s based on AP's non-viral threat scanning mode."), m_bShouldSend ? "ON" : "OFF");
    }
    else
    {
        CCTRACEE(_T("CNotifyAPSpywareDetections::Initialize() - Failed to load savrt32 options."));
    }
}

void CNotifyAPSpywareDetections::ScanComplete()
{
    // Make sure we've actually sent some data
    if( m_bShouldSend && m_lCookie != 0 )
    {
        if( !IsAPServiceUp() )
            return;

        // Send the completion
        CEventData eventData;
        eventData.SetData ( AV::Event_Base_propType, AV::Event_ID_ManualScan_Spyware_Detection );
        eventData.SetData ( AV::Event_ManualScan_Spyware_propCookie, m_lCookie);
        eventData.SetData ( AV::Event_ManualScan_Spyware_propEnd, static_cast<long>(1));

        CCTRACEI(_T("CNotifyAPSpywareDetections::ScanComplete() - Sending completion event for cookie %d."), m_lCookie);
        SendEvent(&eventData);

        // Clear our file set and reset our cookie in case another scan is started with this
        // same scan manager object
        setAPFilesAlreadyNotified.clear();
        m_lCookie = 0;
    }
}

void CNotifyAPSpywareDetections::Notify(IScanInfection* pInfection)
{
    if( !m_bShouldSend )
        return;

    if( !IsAPServiceUp() )
        return;

    // Need to get the file name to send to AP
    ATL::CAtlStringW wStrFile;
    IScanFileInfection2QIPtr pFile = pInfection;
    IScanCompressedFileInfection2QIPtr pCompressed;
    UINT nCodePage = CP_THREAD_ACP;
    if( pFile )
    {
        // For regular files it's just the file name
        if( pFile->AreOEMFileApis() )
            nCodePage = CP_OEMCP;

        if( !ConvertToWideStr(pFile->GetLongFileName(), nCodePage, wStrFile) )
            return;
    }
    else if( pCompressed = pInfection )
    {
        // For compressed files it's the container name
        if( pCompressed->AreOEMFileApis() )
            nCodePage = CP_OEMCP;

        if( !ConvertToWideStr(pCompressed->GetComponent(pCompressed->GetComponentCount()-1), nCodePage, wStrFile) )
            return;
    }
    else
    {
        CCTRACEI(_T("CNotifyAPSpywareDetections::Notify() : This is not a file or compressed infection, no need to send to AP."));
        return;
    }

    // Make sure we haven't already notified for this file
    if( setAPFilesAlreadyNotified.find(wStrFile) != setAPFilesAlreadyNotified.end() )
    {
        CCTRACEI(_T("CNotifyAPSpywareDetections::Notify() - Already notified AP about file %ls."), wStrFile.GetString());
        return;
    }

    if( m_lCookie == 0 )
    {
        // Generate a pseudo-random number to use as the cookie
        srand( (unsigned)time( NULL ) );
        m_lCookie = rand();
        CCTRACEI(_T("CNotifyAPSpywareDetections::Notify() - Generated cookie %d."), m_lCookie);
    }

    // Notify AP of the detection
    CEventData eventData;
    eventData.SetData ( AV::Event_Base_propType, AV::Event_ID_ManualScan_Spyware_Detection );
    eventData.SetData ( AV::Event_ManualScan_Spyware_propCookie, m_lCookie);
    eventData.SetData ( AV::Event_ManualScan_Spyware_propVirusID, static_cast<long>(pInfection->GetVirusID()) );
    eventData.SetData ( AV::Event_ManualScan_Spyware_propFileName, wStrFile.GetString() );
    eventData.SetData ( AV::Event_ManualScan_Spyware_propEnd, static_cast<long>(0));

    CCTRACEI(_T("CNotifyAPSpywareDetections::Notify() - Sending file %ls. VID = %lu"), wStrFile.GetString(), pInfection->GetVirusID());
    if( SendEvent(&eventData) )
    {
        // Save the file names we already notified AP of so we don't send duplicates
        setAPFilesAlreadyNotified.insert(wStrFile);
    }
    CCTRACEI(_T("CNotifyAPSpywareDetections::Notify() - Sent to navapsvc."));
}

bool CNotifyAPSpywareDetections::SendEvent(CEventData* pEvent)
{
    // Create interface pointer to navapsvc
    NAVAPSVCLib::INAVAPServicePtr pNAVAPSVC = NULL;

    try
    {
        if( FAILED(pNAVAPSVC.CreateInstance(__uuidof( NAVAPSVCLib::NAVAPService))) || pNAVAPSVC == NULL )
        {
            CCTRACEE(_T("CNotifyAPSpywareDetections::SendEvent() - Failed to create NAVAPService"));
            return false;
        }
    }
    catch(_com_error& err)
    {
        CCTRACEE(_T("CNotifyAPSpywareDetections::SendEvent() - Caught COM exception attempting to create NAVAPService. hr = 0x%X"), err.Error());
        return false;
    }

    _bstr_t sCommand( NAVAPCMD_MANUALSCANSPYWAREDETECTION );
    VARIANT v;

    // Initialize the variant data.
    v.vt = VT_ARRAY | VT_UI1;
    v.parray = pEvent->Serialize ();
    if( v.parray == NULL )
    {
        CCTRACEE(_T("CNotifyAPSpywareDetections::SendEvent() - Failed to serialize the data to a Variant."));
        return false;
    }

    try
    {
        pNAVAPSVC->SendCommand( sCommand, v );
    }
    catch(_com_error& err)
    {
        CCTRACEE(_T("CNotifyAPSpywareDetections::SendEvent() - Caught COM exception attempting to SendCommand. hr = 0x%X"), err.Error());
        return false;
    }

    return true;
}

bool CNotifyAPSpywareDetections::IsAPServiceUp()
{
    // Make sure the AP service is up and running by checking
    // for it's running mutex
    ccLib::CMutex APServiceMutex;
    if (APServiceMutex.Open(SYNCHRONIZE,
        FALSE,
        NAVAPSVC_RUNNING_MUTEX,
        COSInfo::IsTerminalServicesInstalled()) == FALSE)
    {
        // Mutex does not exist thus navapsvc is not running
        CCTRACEW(_T("CNotifyAPSpywareDetections::IsAPServiceUp() : The AP service is not running, will not send event. GetLastError() = 0x%08X"), GetLastError());
        return false;
    }

    return true;
}

bool CNotifyAPSpywareDetections::ConvertToWideStr ( LPCSTR lpOrig /*in*/, UINT nCodePage /*in*/, ATL::CAtlStringW& strWide /*out*/ )
{
    // Setup a temporary buffer
    std::vector<wchar_t> vBuff;
    int iBuffSize = (strlen(lpOrig) * 2) + 1;
    vBuff.reserve( iBuffSize );

    if ( MultiByteToWideChar( nCodePage, 0,
        lpOrig, -1,
        &vBuff[0], iBuffSize))
    {
        strWide = &vBuff[0];
        strWide.MakeLower();
        return true;
    }

    CCTRACEE (_T("CNotifyAPSpywareDetections::ConvertToWideStr() - Error converting to wide %d"), ::GetLastError());

    return false;
}
