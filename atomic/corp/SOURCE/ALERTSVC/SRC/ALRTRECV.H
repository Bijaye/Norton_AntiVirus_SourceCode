// Copyright 1997 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/ALERTSVC/VCS/alrtrecv.h_v   1.4   14 May 1997 16:44:02   GDZIECI  $ 
// 
// Description:
//   AlertReceiver class declaration
// 
// See Also:                                                            
// 
//*************************************************************************** 
// $Log:   S:/ALERTSVC/VCS/alrtrecv.h_v  $ 
// 
//    Rev 1.4   14 May 1997 16:44:02   GDZIECI
// Defined WINSOCKVERSION here to make sure that all modules use the same
// WINSOCK version number.
// 
//    Rev 1.3   09 May 1997 11:13:22   GDZIECI
// Bug fixes.
// 
//    Rev 1.2   08 May 1997 18:20:54   GDZIECI
// Added bThreadExiting.
// 
//    Rev 1.1   07 May 1997 15:28:12   GDZIECI
// Defined __ALRTRECV_H to avoid including the file more than once.
// 
//    Rev 1.0   06 May 1997 15:56:06   GDZIECI
// Initial revision.
//*************************************************************************** 

//***************************************************************************
// Defines
//***************************************************************************

#if !defined(__ALRTRECV_H)
#define __ALRTRECV_H

#define WINSOCKVERSION	        0x0101

// event data packet defines
#define EVENT_WORKSTATION_VIRUS 0x94
#define EVENT_ID_SIZE           3
#define EVENT_SAM_ID            "SAM"
#define EVENT_NAV_ID            "NAV"


//***************************************************************************
// Typedefs
//***************************************************************************

typedef struct tagEVENT_DATA_PACKET // our data packet
{
    BYTE    byCommand;              // command
    char    chID[EVENT_ID_SIZE];    // origin ID
    char    data[1];                // data
} EVENT_DATA_PACKET;


//***************************************************************************
// Class definitions
//***************************************************************************

class AlertReceiver
{
public:

    AlertReceiver();
    ~AlertReceiver();

    int GetLastError() const { return iLastError; }

    virtual BOOL init() = 0;
    virtual BOOL run();
    virtual void kill();

protected:
    BOOL    bInitialized;
    BOOL    bThreadRunning;
    BOOL    bThreadExiting;
    SOCKET  s;
    HANDLE  hThread;
    int     iLastError;
    LPTHREAD_START_ROUTINE lpThreadProc;

    void sendVirusNotificationPacket ( LPSTR lpszVirusName,
        LPSTR lpszUserName, LPSTR lpszPathName, DWORD dwEventSource );

    virtual void processIncomingPacket ( EVENT_DATA_PACKET *pPacket ) = 0;

    virtual void deregisterService();

    static DWORD WINAPI AlertReceiverThreadProc ( LPVOID p ) {return 0;}
};


class ATPReceiver : public AlertReceiver
{
public:
    ATPReceiver();
    ~ATPReceiver() { kill(); }

    BOOL init();

private:
    virtual void processIncomingPacket ( EVENT_DATA_PACKET *pPacket );

    static DWORD WINAPI AlertReceiverThreadProc ( LPVOID p );

    void convertMacToPC ( LPBYTE lpbyBuffer );
};


class IPXReceiver : public AlertReceiver
{
public:
    IPXReceiver();
    ~IPXReceiver() { kill(); }

    BOOL init();

private:
    virtual void deregisterService();

    virtual void processIncomingPacket ( EVENT_DATA_PACKET *pPacket );

    static DWORD WINAPI AlertReceiverThreadProc ( LPVOID p );
};



class IPReceiver : public AlertReceiver
{
public:
    IPReceiver();
    ~IPReceiver() { kill(); }

    BOOL init();

private:
    virtual void processIncomingPacket ( EVENT_DATA_PACKET *pPacket );

    static DWORD WINAPI AlertReceiverThreadProc ( LPVOID p );
};

#endif      // __ALRTRECV_H
