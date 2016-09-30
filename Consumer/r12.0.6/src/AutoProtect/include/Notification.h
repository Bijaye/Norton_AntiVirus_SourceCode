// Notification.h: interface for the CNotification class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTIFICATION_H__6726257A_AAF4_4DBB_8322_6F9CBC3E4A66__INCLUDED_)
#define AFX_NOTIFICATION_H__6726257A_AAF4_4DBB_8322_6F9CBC3E4A66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef AP_SPYWARE_SUPPORT
    #include "CAPThreatExclusions.h"
#else
    class CAPThreatExclusions;
#endif

#include "AvEvents.h"
#include "savrt32.h"
#include <string>
#include <vector>

// Base class for the notification packet handling.
//
class CNotification  
{
public:
    CNotification( const PCSAVRT_EVENTREPORTPACKET pPacket, DWORD dwBufferSize );    // For single files
    CNotification( const PSAVRT_CONTAINERREPORTPACKET pPacket, DWORD dwBufferSize );
    CNotification( const PSAVRT_SEMISYNCSCANNOTIFICATIONPACKET pPacket, DWORD dwBufferSize );
    CNotification( const CEventData& eventData ); // already processed
	virtual ~CNotification();

enum enumDataType
{
    ThreatEvent = 0,
    ContainerReport,
    ContainerStatus,
    AlreadyMade      // From the NAVAPSVC scanner, already "made" into an event
};

    enumDataType GetType ();

    // pThreatExclusions : Optional, if the client uses threat exclusions. This sink *must*
    //                     be made thread-safe by the client.
    //
    // This can throw exceptions via STL. I've only seen this if there is a driver mismatch
    // without a version change in the header. This happens during development, but *shouldn't*
    // after shipping.
    //
    bool MakeEvent ( CEventData& eventData, CAPThreatExclusions* pThreatExclusions = NULL );

    // public so the scanner can use them to pre-process
	static void DecodeUserInfo( PSAVRT_USERINFOSUBPACKET pUserInfo, CEventData& eventData, bool bUseActiveSession );  // Defined in SAVRT_PACKETHEADER

    static DWORD getActiveSessionID();

protected:
	CNotification(void);

	void decodeNavAPTime( const PSAVRT_SYSTEMTIME pAPTime, CEventData*& pEvent );     // Defined in SAVRT_PACKETHEADER
    long decodeNavAPAction( DWORD dwAPAction, DWORD dwSucceeded, bool bVirusLike = false /*9x*/ );
    DWORD convertSAVRTStatustoNAVStatus ( SAVRT_CONTAINERSTATUS dwSAVRTStatus );

    // Hold the raw data here for now
    std::vector <BYTE> m_vecData;
    CEventData m_eventData; // for pre-processed data

    enumDataType m_EventType; // enumDataTypes

    bool makeThreat ( CEventData& eventData, CAPThreatExclusions* pThreatExclusions );
    bool makeThreatContainer ( CEventData& eventData, CAPThreatExclusions* pThreatExclusions );
    bool makeContainerStatus ( CEventData& eventData );
};

#endif // !defined(AFX_NOTIFICATION_H__6726257A_AAF4_4DBB_8322_6F9CBC3E4A66__INCLUDED_)
