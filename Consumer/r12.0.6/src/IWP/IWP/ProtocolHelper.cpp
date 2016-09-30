#include "StdAfx.h"
#include ".\protocolhelper.h"
#include "winsock.h"


CProtocolHelper::CProtocolHelper(void)
{
    m_mapPorts[80] = "http, www, www-http"; 
    m_mapPorts[8001] = "http-alt";
    m_mapPorts[8008] = "http-alt-1";
    m_mapPorts[8080] = "http-proxy";    // Often used as HTTP proxy
    m_mapPorts[8088] = "http-proxy-1";  // Often used as HTTP proxy
    m_mapPorts[280] = "http-mgmt";      // HTTP management
    m_mapPorts[443] = "https";          // HTTP server
    m_mapPorts[488] = "gss-http";       // HTTP misc
    m_mapPorts[591] = "fmpro-http";
    m_mapPorts[593] = "http-rpc-epmap";
    m_mapPorts[67] = "bootps";          // Bootstrap Protocol Server
    m_mapPorts[68] = "bootpc";          // Bootstrap  Protocol Client
    m_mapPorts[135] = "dcom";           // Microsoft RPC end point to end point mapping
    m_mapPorts[143] = "imap";           // Internet Message Access Protocol
    m_mapPorts[389] = "ldap";           // Lightweight Directory Access Protocol
    m_mapPorts[458] = "video";          // Connectix and Quick Time Streaming protocols
    m_mapPorts[545] = "video-1";        // Connectix and Quick Time Streaming protocols
    m_mapPorts[554] = "rtsp";           // Real Time Stream Protocol
    m_mapPorts[709] = "mountd";         // NFS mount daemon
    m_mapPorts[721] = "pcnfsd";         // PC NFS Deamon
    m_mapPorts[194] = "irc";            // Internet Relay Chat protocol
    m_mapPorts[529] = "irc-serv";
    m_mapPorts[994] = "ircs";
    m_mapPorts[6665] = "ircu";
    m_mapPorts[6666] = "ircu-1";
    m_mapPorts[6667] = "ircu-2";
    m_mapPorts[6668] = "ircu-3";
    m_mapPorts[6669] = "ircu-4";
    m_mapPorts[1080] = "socks";         // Socks
    m_mapPorts[1352] = "lotusnotes";    // Lotus
    m_mapPorts[1433] = "ms-sql-s";      // Microsoft misc
    m_mapPorts[1434] = "ms_sql-m";
    m_mapPorts[1477] = "ms-sna-server";
    m_mapPorts[1478] = "ms-sna-base";
    m_mapPorts[1525] = "orasrv";        // Oracle
    m_mapPorts[1527] = "tdisrv";
    m_mapPorts[1529] = "coauthor";
    m_mapPorts[1537] = "nsvt";            // HP's NSVT native protocol
    m_mapPorts[1570] = "nsvt-stream";    // HP's NSVT TCP stream mode
    m_mapPorts[1745] = "remote-winsock"; // Remote Winsock Proxy
    m_mapPorts[1755] = "netshow";       // Microsoft's NetShow
    m_mapPorts[1863] = "msnp";         // Microsoft's MSN messaging
    m_mapPorts[4000] = "icq";          // ICQ chat program
    m_mapPorts[5190] = "aol";          // America Online
    m_mapPorts[5191] = "aol-1";        // America Online 
    m_mapPorts[5192] = "aol-2";        // America Online
    m_mapPorts[5193] = "aol-3";        // America Online
    m_mapPorts[11523] = "aol-4";       // America Online
    m_mapPorts[31337] = "Back-Orifice";  // Back Orifice
    m_mapPorts[54320] = "Back-Orifice-2000"; // Back Orifice 2000
    m_mapPorts[54321] = "Back-Orifice-2000-1"; // Back Orifice 2000
    m_mapPorts[12345] = "NetBus";             // Netbus
    m_mapPorts[12346] = "NetBus-2";           // Netbus2
    m_mapPorts[20034] = "NetBus-Pro";         // Netbus Pro
    m_mapPorts[1243] = "Backdoor-g-1";       //* Backdoor-g Trojan
    m_mapPorts[6711] = "Backdoor-g-2";       //* Backdoor-g Trojan
    m_mapPorts[6776] = "Backdoor-g-3";       //* Backdoor-g Trojan
    m_mapPorts[5631] = "pc-anywhere-data";   // pcAnywhere data port
    m_mapPorts[5632] = "pc-anywhere-status"; // pcAnywhere status port
    m_mapPorts[6000] = "xserver";            // X Server
    m_mapPorts[7000] = "vdolive";            // VDOLive Player
    m_mapPorts[7007] = "msbd";               // Microsoft MSBD (related to NetShow)
    m_mapPorts[7070] = "realaudio";          // Real Networks Real Audio
    m_mapPorts[26000] = "quake";             // Quake server game
    m_mapPorts[27910] = "quake2";            // Quake2 server game
    m_mapPorts[27911] = "quake2-2";          // Quake2 server game
    m_mapPorts[1900] = "ssdp";               // simple service discovery protocol
    m_mapPorts[137] = "netbios-ns";          // NETBIOS Name Service
    m_mapPorts[138] = "netbios-dgm";         // NETBIOS Datagram
    m_mapPorts[139] = "netbios-ssn";         // NETBIOS Session
    m_mapPorts[0] = "";
}

CProtocolHelper::~CProtocolHelper(void)
{
}

bool CProtocolHelper::GetPortName ( std::string& strOut, DWORD dwPort )
{
    mapPorts::iterator iter = m_mapPorts.find ( dwPort );
    if ( iter != m_mapPorts.end () )
    {
        strOut = iter->second;
        return true;
    }

    return false;
}

bool CProtocolHelper::GetProtocolName ( std::string& strOut, DWORD dwProtocol )
{
	// TCP, UDP, or ICMP? - these don't get translated
    //
	switch(dwProtocol)
	{
		case IPPROTO_TCP:
			strOut = "TCP";
			break;
		case IPPROTO_UDP:
			strOut = "UDP";
			break;
		case IPPROTO_ICMP:
			strOut = "ICMP";
			break;
		case IPPROTO_IP:
			strOut = "IP";
			break;
		default:
			ASSERT(FALSE);
			break;
	}

    return ( !strOut.empty () );
}
