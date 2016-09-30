#pragma once

#include <map>
#include <string>

class CProtocolHelper
{
public:
    CProtocolHelper(void);
    ~CProtocolHelper(void);

    // String description of the port, if it's a commmon one. (http, ftp, etc.)
    //
    // Returns false if it can't find one. 
    //
    bool GetPortName ( std::string& strOut /*out*/, DWORD dwPort /*in*/ );

    // String description of the protocol ( TCP, UDP, ICMP or IP )
    //
    bool GetProtocolName ( std::string& strOut /*out*/, DWORD dwProtocol /*in (winsock)*/ );

protected:
    typedef std::map <DWORD, std::string> mapPorts;
    mapPorts m_mapPorts;
};
