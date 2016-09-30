// Copyright 1992 Symantec, Peter Norton Group
//**********************************************************************
//                                                                      
// $Header:   S:/NAVSERVE/VCS/sympipe.h_v   1.0   18 Apr 1997 14:01:24   DSACKIN  $
//                                                                      
// Description: Prototypes for pipe.c
//                                                                      
//**********************************************************************
// $Log:   S:/NAVSERVE/VCS/sympipe.h_v  $
// 
//    Rev 1.0   18 Apr 1997 14:01:24   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:13:58   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:53:12   DKEEFER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:54:20   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:39:58   BARRY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1994 11:37:14   MFALLEN
// Initial revision.
// 
//    Rev 1.27   15 Nov 1994 00:29:40   KEITH
// Added OT_NP_PIPE_SERVER_1 to supply a swapped endian version.
// 
//    Rev 1.26   31 Aug 1994 16:19:46   KEITH
// Added an IntSwap Macro for SYM_WIN compiles.
// 
//    Rev 1.25   22 Aug 1994 15:21:06   JMILLARD
// lengthen the canned pipe timeouts for low-level activity (from Quake 2)
// 
//    Rev 1.24   26 Apr 1994 16:39:50   BRAD
// Removed PVCS Logs
/************************************************************************/

#ifndef _SYMPIPE_H
#define _SYMPIPE_H

//  .h files required to include this .h file
                                                
#ifndef INCL_PLATFORM
#error platform.h MUST be included before sympipe.h
#endif



//**********************************************************************
//
//  nested include files
//
//**********************************************************************

#include "symtime.h"                    // TIMEOUT


//**********************************************************************
//
//  defines
//
//**********************************************************************

// pipe server server type

#define OT_NP_PIPE_SERVER       0x0526  // NP pipe server type for advertise
#define OT_NP_PIPE_SERVER_1     0x2605  // 
#define OT_NAV_IPX_SERVER       0x0531  // NAV Ipx server type for advertise

#define NET_PIPE_SPX_SOCKET     0x84bd  // NORTON reserved socket
#define NAV_IPX_SOCKET          0x84be  // NORTON reserved socket

#define MSDOS_60_IPX_SOCKET     0x2682  // Central Point 1.0
#define MSDOS_60_IPX_SOCKET_1   0x8226  // Central Point assigned socket

#define MSDOS_60_IPX_SERVER_PREFIX "MASTER_CPNETAV_"

#define NET_PIPE_NAME           "SymNetPipe"
#define IPX_SERVER_PREFIX       ""
#define PIPE_SERVER_PREFIX      ""
#define ALL_NLMS "*"            // search prototype for pipe servers
#define ALL_OBJECTS "*"         // search prototype for anything
#define MAX_PIPE_NAME           32

// service pipe connection defines

#define SERVICE_PIPE_CONNECTION_RETRY_COUNT 10

// pipe server timer id

#define PIPE_SERVER_TIMER_ID    1
#define ADVERTISE_RATE          ((WORD)60 * (WORD)1000)   // 60 seconds just barely fits

// pipe timeout definitions

#define PIPE_READ_TIMEOUT       10000   // 10 second timeout on reads
#define PIPE_WRITE_TIMEOUT      10000   // 10 second timeout on writes
#define PIPE_OPEN_TIMEOUT       10000   // 10 second timeout on opens
#define PIPE_CLOSE_TIMEOUT      1000    // 1 second timeout on close

// pipe protocol service ids

#define NP_PROTO_BEST           0x0000  // Best available protocol
#define NP_PROTO_SPX            0x0001  // Novell SPX
#define NP_PROTO_IPX            0x0002  // Novell IPX
#define NP_PROTO_NETBIOS        0x0003  // NetBios
#define NP_PROTO_OS2_PIPE       0x0004  // OS/2 named pipe
#define NP_PROTO_TCP            0x0005  // TCP
#define NP_PROTO_UDP            0x0006  // UDP

// user verification returns

#define USER_OK                 0x0000   
#define USER_BAD                0x0001
#define USER_IS_SUPERVISOR      0x0002

// pipe creation mode type bits

#define NP_CREATE_WRITE_THROUGH   0x0001  // all writes sent immediately
#define NP_CREATE_WAIT            0x0002  // read, write and listen block
#define NP_CREATE_STREAM          0x0004  // stream type pipe  - mutex
#define NP_CREATE_MESSAGE         0x0008  // message type pipe - mutex
#define NP_CREATE_SERVICE         0x0010  // service pipe - talks to service
                                          // socket
#define NP_CREATE_NO_USERNAME     0x0020  // forces connect without username
#define NP_CREATE_SUPERVISOR_ONLY 0x0040  // connector must be supervisor
                                          // equivalent
#define NP_CREATE_VALID_USER_ONLY 0x0080  // connector must be valid user
#define NP_CREATE_INVISIBLE       0x0100  // test user password invisible
#define NP_CREATE_VERIFY_ONLY     0x0200  // test user password only

#define NP_CREATE_NOWAIT_OPEN     0x0400  // NetPipeOpen does not block
                                          // ( notified in callback as 
                                          // NP_SEND_CONNECT )
#define NP_CREATE_SERVICE_DATAGRAM 0x0800 // service pipe communications
                                          // use datagrams
#define NP_PIPE_CREATOR           0x8000  // this end created the pipe

// wPipeState - pipe state definitions

#define NP_PIPE_NOT_CONNECTED   0       // pipe not connected yet
#define NP_PIPE_LISTENING       1       // pipe listening for connect
#define NP_PIPE_PARTIAL_CONNECT 2       // initial stage of connection
#define NP_PIPE_CONNECTED       3       // pipe connected for user use
#define NP_PIPE_DISCONNECTED    4       // pipe disconnected at other end
#define NP_PIPE_CLOSING         5       // close in progress
#define NP_PIPE_CANCELLING      6       // cancel in progress
#define NP_PIPE_SWAPPED_OUT     7       // connection swapped out (disconnected)
#define NP_PIPE_RECONNECTING    8       // connection reconnecting

// Internal pipe message definitions

#define NP_INTERNAL_PIPE_MSG_BASE     0xff00  // base id for internal messages
#define NP_INTERNAL_PIPE_QUERY        0xff00  // query for pipe
#define NP_INTERNAL_PIPE_QUERY_RESP   0xff01  // query for pipe
#define NP_INTERNAL_PIPE_CONNECT      0xff02  // request for pipe connection
#define NP_INTERNAL_PIPE_CONNECT_RESP 0xff03  // return pipe connection request

// response status defines

#define NP_QUERY_RESP_NO_SUCH_PIPE      0
#define NP_QUERY_RESP_PIPE_AVAILABLE    1
#define NP_QUERY_RESP_VERSION_OUT_OF_BAND 2

#define NP_CONNECT_RESP_NO_SUCH_PIPE    0
#define NP_CONNECT_RESP_PIPE_LISTENING  1
#define NP_CONNECT_RESP_PIPE_FAILED     2
#define NP_CONNECT_RESP_INVALID_USER    3
#define NP_CONNECT_RESP_VERSION_OUT_OF_BAND 4


// basic internal pipe communications packet

#define MAX_INTERNAL_COMMAND_SIZE   (sizeof(PIPECONNECT))

// this number is 534 - max SPX packet length less the header size
 
#define MAX_NET_COMMAND_LENGTH  (534 - USER_PACKET_HEADER_SIZE)

// WARNING - this must match the size of the structure that follows (less
//           the byData part

#define USER_PACKET_HEADER_SIZE   (sizeof(DWORD)*6+sizeof(WORD))


//**********************************************************************
//
//  Mapped error definitions
//
//**********************************************************************

// pipe error 

#define NP_ERR_INIT_NO_SERVERS_FOUND    1
#define NP_ERR_INIT_PIPE_NOT_LISTENING  2
#define NP_ERR_INIT_PROTOCOL_DRIVER_TOO_OLD  3
#define NP_ERR_INIT_BAD_SERVER          4
#define NP_ERR_INIT_SPX_NOT_INST        5
#define NP_ERR_INIT_IPXSPX_NOT_INIT     6
#define NP_ERR_INIT_NO_DOS_MEMORY       7
#define NP_ERR_INIT_NO_FREE_ECB         8
#define NP_ERR_INIT_LOCK_FAILED         9
#define NP_ERR_INIT_OVER_MAX_LIMIT      10
#define NP_ERR_INIT_IPXSPX_PREV_INIT    11
#define NP_ERR_INIT_UNDOC               12
#define NP_ERR_INIT_ALREADY_INIT        13
#define NP_ERR_INIT_NO_SPX              14
#define NP_ERR_INIT_NO_MEM              15
#define NP_ERR_INIT_NO_LOCK             16
#define NP_ERR_INIT_NO_IPX              17
#define NP_ERR_INIT_SOCKET_ALREADY_OPEN 18  
#define NP_ERR_INIT_SOCKET_TABLE_FULL   19
#define NP_ERR_INIT_SOCKET_UNDOC        20
#define NP_ERR_INIT_DLL_NOT_FOUND       21
#define NP_ERR_INIT_DLL_LOAD_ERROR      22
#define NP_ERR_INIT_DLL_API_NOT_PRESENT 23
#define NP_ERR_INIT_APP_EXITING         24

#define NP_ERR_CREATE_NO_NAME       26
#define NP_ERR_CREATE_NO_MEM        27
#define NP_ERR_CREATE_NO_LOCK       28

#define NP_ERR_ECB_NOT_AVAILABLE    30
#define NP_ERR_ECB_NOT_IN_USE       31
#define NP_ERR_ECB_BAD_USAGE_CODE   32
#define NP_ERR_ECB_CORRUPT          33

#define NP_ERR_SPX_CONNECTION_FAILED 40
#define NP_ERR_SPX_SOCKET_NOT_OPENED 41
#define NP_ERR_SPX_ECB_CANCELLED     42
#define NP_ERR_SPX_PACKET_OVERFLOW   43
#define NP_ERR_SPX_MALFORMED_PACKET  44
#define NP_ERR_SPX_SOCKET_CLOSED     45
#define NP_ERR_SPX_CONNECTION_TABLE_FULL 46
#define NP_ERR_SPX_LISTEN_UNDOC      47
#define NP_ERR_SPX_COMMAND_CANCELED  48

#define NP_ERR_SEMAPHOR_ALLOC_FAIL   50     // OpenLocalSemaphor failed
#define NP_ERR_PROC_INSTANCE_FAIL    51

#define NP_ERR_DEINIT_NO_INIT        60

#define NP_ERR_FILE_OPEN_FAILED      62
#define NP_ERR_FILE_ALREADY_OPEN     63

#define NP_ERR_FILE_CLOSE_NOT_OPEN   64
#define NP_ERR_FILE_CLOSE_FAILED     65

#define NP_ERR_FILE_READ_NOT_OPEN    66
#define NP_ERR_FILE_READ_ERROR       67
#define NP_ERR_FILE_READ_TOO_BIG     68

#define NP_ERR_FILE_WRITE_NOT_OPEN   69
#define NP_ERR_FILE_WRITE_ERROR      70
#define NP_ERR_FILE_WRITE_TOO_BIG    71

#define NP_ERR_SPX_CLOSE_UNDOC       73
#define NP_ERR_CLOSE_NO_INIT         74

#define NP_ERR_OPEN_GET_FAILED                  75
#define NP_ERR_OPEN_NOT_HERE                    76
#define NP_ERR_OPEN_UNDOC                       77
#define NP_ERR_OPEN_NO_ANSWER_FROM_TARGET       78
#define NP_ERR_OPEN_SOCKET_NOT_OPENED           79
#define NP_ERR_OPEN_CONNECTION_TABLE_FULL       80
#define NP_ERR_OPEN_NO_LOCAL_TARGET_IDENTIFIED  81
#define NP_ERR_OPEN_COMMAND_CANCELLED           82
#define NP_ERR_OPEN_MALFORMED_PACKET            83
#define NP_ERR_OPEN_PIPE_SERVER_NOT_FOUND       84 

// these errors are mapped from response to pipe open request

#define NP_ERR_OPEN_INVALID_CONNECT_ERRORS      85
#define NP_ERR_OPEN_CONNECT_NO_SUCH_PIPE        85
#define NP_ERR_OPEN_CONNECT_PIPE_FAILED         86
#define NP_ERR_OPEN_CONNECT_INVALID_USER        87

#define NP_ERR_RECONNECT_NONE_CONNECTED     88

#define NP_ERR_BIND_SERVER_OUT_OF_MEMORY    90
#define NP_ERR_BIND_INVALID_NAME            91
#define NP_ERR_BIND_NO_SUCH_OJBECT          92
#define NP_ERR_BIND_SERVER_BINDERY_LOCKED   93
#define NP_ERR_BIND_BINDERY_FAILURE         94
#define NP_ERR_BIND_UNDOC                   95
#define NP_ERR_BIND_INVALID_FILE_HANDLE     96 
#define NP_ERR_BIND_NO_READ_PRIVILEGES      97 
#define NP_ERR_BIND_NO_SUCH_SEGMENT         98 
#define NP_ERR_BIND_WILD_CARD_NOT_ALLOWED   99 
#define NP_ERR_BIND_INVALID_BINDERY_SECURITY    100
#define NP_ERR_BIND_NO_PROPERTY_READ_PRIVILEGE  101
#define NP_ERR_BIND_NO_SUCH_PROPERTY            102
#define NP_ERR_BIND_NO_SUCH_OBJECT              103

#define NP_ERR_SEMAPHORE_UNDOC               110

#define NP_ERR_SEND_TERMINATED_BY_REMOTE_PARTNER 120
#define NP_ERR_SEND_SPX_CONNECTION_FAILED        121
#define NP_ERR_SEND_SPX_INVALID_CONNECTION       122
#define NP_ERR_SEND_SPX_MALFORMED_PACKET         123
#define NP_ERR_SEND_UNDOC                        124

#define NP_ERR_RECEIVE_TERMINATED_BY_REMOTE_PARTNER 130
#define NP_ERR_RECEIVE_SPX_CONNECTION_FAILED        131
#define NP_ERR_RECEIVE_SPX_INVALID_CONNECTION       132
#define NP_ERR_RECEIVE_SPX_MALFORMED_PACKET         133
#define NP_ERR_RECEIVE_UNDOC                        134

#define NP_ERR_TERMINATE_TERMINATED_BY_REMOTE_PARTNER 135
#define NP_ERR_TERMINATE_TERMINATED_POORLY            136
#define NP_ERR_TERMINATE_INVALID_CONNECTION           137
#define NP_ERR_TERMINATE_MALFORMED_PACKET             138
#define NP_ERR_TERMINATE_UNDOC                        139

#define NP_ERR_WIN_CLASS_REGISTER_FAILED            140
#define NP_ERR_WIN_CREATE_FAILED                    141
#define NP_ERR_WIN_CREATE_TIMER_FAILED              142

#define NP_ERR_WIN_CLASS_DEREGISTER_FAILED          145
#define NP_ERR_WIN_DESTROY_FAILED                   146
#define NP_ERR_WIN_TIMER_DESTROY_FAILED             147

#define NP_ERR_ADVERTISE_FAILED                     150
#define NP_ERR_UNADVERTISE_FAILED                   151

#define NP_ERR_ECB_STAT_CONNECTION_TERMINATED       160 
#define NP_ERR_ECB_STAT_TERMINATED_POORLY           161 
#define NP_ERR_ECB_STAT_INVALID_CONNECTION          162 
#define NP_ERR_ECB_STAT_CONNECTION_TABLE_FULL       163 
#define NP_ERR_ECB_STAT_SOCKET_NOT_OPEN             164 
#define NP_ERR_ECB_STAT_SOCKET_ALREADY_OPEN         165 
#define NP_ERR_ECB_STAT_ECB_CANNOT_BE_CANCELLED     166 
#define NP_ERR_ECB_STAT_NO_KNOWN_ROUTE_TO_DESTINATION 167
#define NP_ERR_ECB_STAT_EVENT_CANCELLED             168
#define NP_ERR_ECB_STAT_PACKET_OVERFLOW             169 
#define NP_ERR_ECB_STAT_MALFORMED_PACKET            170 
#define NP_ERR_ECB_STAT_TRANSMIT_FAILURE            171 
#define NP_ERR_ECB_STAT_UNDOC                       172

#define NP_ERR_ESTCONN_ECB_STAT_NO_TARGET_ANSWER      180
#define NP_ERR_ESTCONN_ECB_STAT_CONNECTION_TABLE_FULL 181
#define NP_ERR_ESTCONN_ECB_STAT_LISTEN_CANCELED       182
#define NP_ERR_ESTCONN_ECB_STAT_MALFORMED_PACKET      183
#define NP_ERR_ESTCONN_ECB_STAT_SOCKET_NOT_OPENED     184
#define NP_ERR_ESTCONN_ECB_STAT_UNDOC                 185

#define NP_ERR_LISTCONN_ECB_STAT_CONNECTION_TABLE_FULL 190
#define NP_ERR_LISTCONN_ECB_STAT_LISTEN_CANCELED       191
#define NP_ERR_LISTCONN_ECB_STAT_MALFORMED_PACKET      192
#define NP_ERR_LISTCONN_ECB_STAT_SOCKET_NOT_OPENED     193
#define NP_ERR_LISTCONN_ECB_STAT_UNDOC                 194

#define NP_ERR_SEND_ECB_STAT_CONNECTION_TERMINATED    200
#define NP_ERR_SEND_ECB_STAT_TERMINATED_POORLY        201
#define NP_ERR_SEND_ECB_STAT_INVALID_CONNECTION       202
#define NP_ERR_SEND_ECB_STAT_LISTEN_CANCELED          203
#define NP_ERR_SEND_ECB_STAT_MALFORMED_PACKET         204
#define NP_ERR_SEND_ECB_STAT_UNDOC                    205

#define NP_ERR_LISTEN_ECB_STAT_CONNECTION_TABLE_FULL  210
#define NP_ERR_LISTEN_ECB_STAT_LISTEN_CANCELED        211
#define NP_ERR_LISTEN_ECB_STAT_MALFORMED_PACKET       212
#define NP_ERR_LISTEN_ECB_STAT_UNDOC                  213

#define NP_ERR_TERMINATE_ECB_STAT_TERMINATED_POORLY        220
#define NP_ERR_TERMINATE_ECB_STAT_CONNECTION_TERMINATED    221
#define NP_ERR_TERMINATE_ECB_STAT_INVALID_CONNECTION       222
#define NP_ERR_TERMINATE_ECB_STAT_MALFORMED_PACKET         223
#define NP_ERR_TERMINATE_ECB_STAT_UNDOC                    224

#define NP_ERR_BAD_PROTOCOL         230       // bad protocol to NetPipeRegister
#define NP_ERR_BAD_SERVICE          231       // bad service from anywhere
#define NP_ERR_BAD_PIPE             232       // bad pipe from anywhere

#define NP_ERR_CANCEL_ECB_CANNOT_BE_CANCELLED      240
#define NP_ERR_CANCEL_ECB_EVENT_CANCELLED          241
#define NP_ERR_CANCEL_ECB_NOT_IN_USE               242
#define NP_ERR_CANCEL_ECB_UNDOC                    243

#define NP_ERR_ID_SUBCLASS_TABLE_FULL              245
#define NP_ERR_ID_ALREADY_SUBCLASSED               246
#define NP_ERR_ID_NOT_SUBCLASSED                   247

#define NP_ERR_OPERATION_TIMED_OUT                 248 

#define NP_ERR_MULTI_IN_PROCESS                    249
#define NP_ERR_MULTI_NOT_IN_PROCESS                250
#define NP_ERR_MULTI_NO_MEM                        251
#define NP_ERR_MULTI_SEQ_ERR                       252

#define NP_MAX_STR      125


//**********************************************************************
//
//  Typedefs
//
//**********************************************************************

// basic pipe communications packet

typedef struct tagUSERPACKET         // complete declaration
    {

    DWORD       dwStructSize;       // size of basic structure
    DWORD       dwDataSize;         // size of variable data
    DWORD       dwParam1Send;       // callback for responding to this message
    DWORD       dwParam2Send;       // dwParam to go with callback
    DWORD       dwParam1Resp;       // callback for this response
    DWORD       dwParam2Resp;       // dwParam to go with callback
    WORD        wCommand;           // command request
                                    // accompanying data
    BYTE        byData[MAX_NET_COMMAND_LENGTH]; 

    } USERPACKET;

typedef USERPACKET FAR *LPUSERPACKET;



// pipe connect

typedef struct tagPIPECONNECT      // complete declaration
    {

    DWORD   dwPipeMode;                     // pipe mode bits
    WORD    wSocket;                        // socket for service connection
    BYTE    szPipeName[MAX_PIPE_NAME];
    BYTE    byUserName[NETMAXUSERNAME];     // encrypted
    BYTE    byPassword[NETMAXUSERNAME];     // encrypted

    } PIPECONNECT;

typedef PIPECONNECT FAR *LPPIPECONNECT;


typedef struct tagPIPEPACKET        // complete declaration
    {

    DWORD       dwStructSize;       // size of basic structure
    DWORD       dwDataSize;         // size of variable data
    WORD        wCommand;           // command request
    WORD        wStatus;            // status returned
                                    // accompanying data
    BYTE        byData[MAX_INTERNAL_COMMAND_SIZE]; 

    } PIPEPACKET;

typedef PIPEPACKET FAR *LPPIPEPACKET;

#define PIPE_PACKET_SIZE    (sizeof(PIPEPACKET) - MAX_INTERNAL_COMMAND_SIZE)

// pipe packet message definitions

// pipe query

typedef struct tagPIPEQUERY         // complete declaration
    {

    DWORD   dwPipeMode;
    BYTE    szPipeName[MAX_PIPE_NAME];

    } PIPEQUERY;

typedef PIPEQUERY FAR *LPPIPEQUERY;
// pipe connect return

typedef struct tagPIPECONNECTRESPONSE   // complete declaration
    {

    WORD wSocket;                       // new socket to connect to

    } PIPECONNECTRESPONSE;

typedef PIPECONNECTRESPONSE FAR *LPPIPECONNECTRESPONSE;


// struture for send/receive of packets

typedef struct SENDRECEIVE
        {

        USERPACKET  UserPacket;
        BOOL        bCallbackReceived;      // callback complete flag

        } SENDRECEIVE;

typedef SENDRECEIVE FAR *LPSENDRECEIVE;


// packet reassembly structure

typedef struct PACKETREASSEMBLY
    {

    DWORD       dwDataSize;             // buffer size
    DWORD       dwBytesSoFar;           // bytes reassembled so far
    BOOL        bDone;                  // packet reassembly complete flag
    BYTE        byBuffer[1];            // variable size

    } PACKETREASSEMBLY;

typedef PACKETREASSEMBLY FAR *LPPACKETREASSEMBLY;


// windows definitions

#ifndef SPRINTF
#ifdef SYM_WIN                // select the appropriate sprintf
#define SPRINTF wsprintf
#else
#define SPRINTF sprintf
#endif
#endif


#define NP_WND_CLASS    "NortonPipeClassAct"
#define NP_WND_NAME     "NortonPipeWindow"

#define NP_WINDOW_EXTRA_LPSERVICE   0   // pointer to lpService
#define NP_WINDOW_EXTRA_BYTES       4   // size of FAR PTR to service 

// callback message notify types for NOTIFYPROC (and messages for internal
// processing window

#ifndef WM_USER
#define WM_USER		    0x0400
#endif

#define NP_LISTEN_CONNECT       (WM_USER+ 1)    // 1025 - listen connect complete
#define NP_SEND_CONNECT         (WM_USER+ 2)    // 1026 - send connect complete
#define NP_LISTEN               (WM_USER+ 3)    // 1027 - listen complete
#define NP_SEND                 (WM_USER+ 4)    // 1028 - send complete
#define NP_TIMED_OUT            (WM_USER+ 5)    // 1029 - operation timed out

typedef WORD (WINAPI *GENERICNOTIFYPROC)(
            WORD    wParam,
            DWORD   dwParam );      // standard notify procedure

typedef      (WINAPI *FAREXPORTPROC)(  );
typedef      (WINAPI *ENUMPROC)(
                                LPSTR lpszServerName, DWORD dwParam );
typedef void (WINAPI *FARESRPROC)( VOID );

#ifdef SYM_NLM

typedef LONG    HADVERTISE;         // SAP handle

#else

typedef UINT    HADVERTISE;         // timer handle for advertising

#endif

        struct tagSERVICE;                  // incomplete SERVICE so I can
typedef struct tagSERVICE FAR *LPSERVICE;   // typedef LPSERVICE

        struct tagPIPE;             // incomplete PIPE so I can typedef
typedef struct tagPIPE FAR *LPPIPE; // LPPIPE

typedef struct tagSERVICE           // complete declaration
    {

    DWORD      dwStructSize;        // size of basic structure

    LPSERVICE  lpNextService;       // next service in list
    LPSERVICE  lpPrevService;       // previous service in list

    HADVERTISE hAdvertise;          // handle for advertising mechanism

    HINSTANCE  hLibInst;            // network library DLL instance
    HGLOBAL    hServiceMem;         // memory handle for service entry
    LPPIPE     lpPipe;              // pointer to list of open pipes

    DWORD      dwConnectionId;      // connection id
    DWORD      dwUseCount;          // number of current users

    FAREXPORTPROC    lpfnPipeCreate;
    FAREXPORTPROC    lpfnPipeDestroy;
    FAREXPORTPROC    lpfnPipeListen;
    FAREXPORTPROC    lpfnPipeOpen;
    FAREXPORTPROC    lpfnPipeClose;
    FAREXPORTPROC    lpfnPipeCancelIO;
    FAREXPORTPROC    lpfnPipeRead;
    FAREXPORTPROC    lpfnPipeReadAlloc;
    FAREXPORTPROC    lpfnPipeWrite;
    FAREXPORTPROC    lpfnPipeEnum;
    FAREXPORTPROC    lpfnPipeSysDeinit;
    FAREXPORTPROC    lpfnGetNodeId;
    FAREXPORTPROC    lpfnListenConnectArrived;
    FAREXPORTPROC    lpfnSendConnectArrived;
    FAREXPORTPROC    lpfnListenArrived;
    FAREXPORTPROC    lpfnSendArrived;

#ifdef SYM_WIN
    HINSTANCE  hInstance;           // instance handle of parent
    HWND       hNPWindow;           // window used for everything
#endif

    BOOL       bAdvertise;          // TRUE is service advertises
    BOOL       bServiceClosing;     // TRUE is service is shutting down
    BOOL       bConnectionsRoveable;    // TRUE if it makes sense to
                                        // swap connections in and out
#ifdef SYM_WIN
    BOOL       bTimerRequired;      // TRUE is WM_TIMER messages required
#endif

    WORD       wProtocolId;
    WORD       wServiceSocket;
    WORD       wServiceListenSocket;    // socket for incoming service replies, 
                                        // if one per service
    WORD       wServerTypeId;           // advertising server type
    WORD       wServicePipeProtocol;    // protocol for service pipe

    BYTE       szServiceName[48];   // service name for advertising

    } SERVICE;

    // note the treatment of these as "handles". They are really 
    // pointers all the time

typedef LPSERVICE       HSERVICE;     // handle for service protocol
typedef HSERVICE FAR *LPHSERVICE;


typedef LPPIPE       HPIPE;           // handle for pipe
typedef HPIPE FAR *LPHPIPE;


// callback for NetPipeOpen upon arrival of packets from the outside world.
// wParam contains NP_LISTEN for arriving packets to be read,
//                 NP_SEND_CONNECT when a connect actually takes place
//                 you get no notify on NP_SEND or NP_LISTEN_CONNECT.
// dwParam is the value you passed to NetPipeOpen to give to the callback

typedef WORD (WINAPI *NOTIFYPROC)(
            HPIPE   hPipe,          // pipe handle
            WORD    wParam,         // notification type (NP_LISTEN, etc.)
            DWORD   dwParam );      // standard notify procedure


typedef struct tagPIPE              // complete declaration
    {

    DWORD       dwStructSize;       // size of basic structure
    DWORD       dwDataSize;         // size of variable data part

    LPPIPE      lpNextPipe;         // next service in list
    LPPIPE      lpPrevPipe;         // previous service in list
                                
    LPSERVICE   lpService;          // service handling this pipe
    LPPACKETREASSEMBLY lpReassembly; // area used for packet reassembly

    DWORD       dwPipeMode;         // pipe mode bits
    DWORD       dwConnectionId;     // connection id
    DWORD       dwMaxPacketSize;    // max packet size that will cross net

    VOID FAR    *lpQueuedInput;     // list of queued packets
    NOTIFYPROC  lpfnNotifyProc;     // procedure to notify user
    DWORD       dwParam;            // parameter to pass back to notify

    HPIPE       hServicePipe;       // handle for service pipe during connect
    HPIPE       hParentPipe;        // parent of service pipe

    DWORD       dwListenConnectTimeout;     // timeouts for various operations
    DWORD       dwSendConnectTimeout;
    DWORD       dwListenTimeout;
    DWORD       dwSendTimeout;
    DWORD       dwTerminateTimeout;

    DWORD       dwMessageTransmitDelay;     // time in ms to send message
    DWORD       dwLastTimeUsed;      
    TIMEOUT     MessageTimer;

#ifdef SYM_NLM
    HSEMAPHORE  hListenConnectSemaphore;
    HSEMAPHORE  hSendConnectSemaphore;
    HSEMAPHORE  hListenSemaphore;
    HSEMAPHORE  hSendSemaphore;
    HSEMAPHORE  hTerminateSemaphore;
#endif

    HFILE       hFile;               // handle for file being transferred
    HGLOBAL     hPipeMem;            // memory handle for service entry

    WORD        wSocket;             // socket for pipe connection
    WORD        wDynamicSocket;      // UNUSED
    WORD        wPermanentSocket;    // UNUSED
    WORD        wPipeState;          // Pipe state flag
    WORD        wPipeErr;            // errors from connect and asynch events
    WORD        wRetryCount;         // retry count for current operation

    BOOL        bExpectingResponse;  // for remote response management
    BOOL        bPipeActive;         // Pipe active flag
    BOOL        bTimeOutActive;      // TRUE = time out in progress

    BYTE        byImmediateAddress[6];  // IPX immediate address
    BYTE        byPipeDestAddress[10];  // address of other end of pipe
    WORD        wDestinationSocket;     // socket finishes address MUST FOLLOW
                                        // byPipeDestAddress

    BYTE        szServerName[NETMAXSERVERNAME]; // name of pipe server
    BYTE        byUserName[NETMAXUSERNAME];     // encrypted
    BYTE        byPassword[NETMAXUSERNAME];     // encrypted
    BYTE        szPipeName[1];       // variable size pipe name

    } PIPE;

// callback procedure for subclassed ids

typedef WORD (WINAPI *IDSUBCLASSPROC)(
        LPUSERPACKET lpPacket,      // arriving packet
        DWORD       dwPacketLen,    // size of packet
        HPIPE       hPipe );        // pipe generating message

// callback procedure for send message callbacks

typedef WORD (WINAPI *PACKETPROC)(
            HPIPE       hPipe,
            LPUSERPACKET lpPacket,
            DWORD       dwPacketLen,
            DWORD       dwParam );          // standard baggage dword


//**********************************************************************
//
//  Macros
//
//**********************************************************************

#ifdef SYM_WIN 
#define IntSwap(a) (((WORD)(a) >> 8) | ((WORD)(a) << 8))
#endif

//**********************************************************************
//
//  Prototypes
//
//**********************************************************************
         
#ifdef __cplusplus
extern "C" {
#endif

WORD SYM_EXPORT WINAPI NetPipeInitialize( // initialize the pipe system
    VOID );

WORD SYM_EXPORT WINAPI NetPipeRegister( // register with the pipe system
    LPHSERVICE  lphService,    // service handle (out)
    HINSTANCE   hInstance,      // instance of calling program
    WORD        wProtocolId);  // protocol identifier (in) NP_PROTO_????

WORD SYM_EXPORT WINAPI NetPipeDeregister( // deregister with the pipe system
    HSERVICE    hService);     // service handle (in) 

WORD SYM_EXPORT WINAPI NetPipeCreate( // create a pipe
     HSERVICE   hService,      // service handle from NetPipeRegister (in)
     LPBYTE     lpszPipeName,  // pipe name (in)
     LPHPIPE    lphPipe,       // pipe handle (out)
     DWORD      dwPipeMode);   // pipe mode bits (in) NP_CREATE_?????

WORD SYM_EXPORT WINAPI NetPipeDestroy( // destroy a pipe
    HPIPE   hPipe);            // pipe handle    (in)

WORD SYM_EXPORT WINAPI NetPipeListen(  // create a pipe
     HPIPE       hPipe,        // pipe handle from create (in) 
     LPHPIPE     lphPipe,      // pipe handle (out)
     NOTIFYPROC  lpfnCallBack, // callback for no-wait listen (in)
     DWORD       dwParam);     // passed back by callback

WORD SYM_EXPORT WINAPI NetPipeOpen(     // open a pipe
     HSERVICE   hService,           // service handle from NetPipeRegister (in)
     LPBYTE     lpszServer,         // name of pipe's creator
     LPBYTE     lpszPipeName,       // name of pipe
     LPHPIPE    lphPipe,            // pipe handle (out)
     DWORD      dwPipeMode,         // pipe mode bits (in) NP_CREATE_?????
     CONST NOTIFYPROC lpfnNotify,   // callback for no-wait listen (in)
     CONST DWORD      dwParam,      // passed back by callback
     LPSTR      szUserName,         // user name for connect
     LPSTR      szPassword );       // password for user

WORD SYM_EXPORT WINAPI NetPipeFind(     // locate an open pipe to a server
     LPBYTE     lpszServer,         // name of pipe's creator
     LPHPIPE    lphPipe );          // pipe handle (out)

WORD SYM_EXPORT WINAPI NetPipeClose( // create a pipe
    HPIPE hPipe);                // pipe handle (in) 

WORD SYM_EXPORT WINAPI NetPipeCancelIO(  // cancel pending operations
    HPIPE   hPipe);             // pipe handle    (in)

WORD SYM_EXPORT WINAPI NetPipeRead(     // read from a pipe
    CONST HPIPE   hPipe,            // pipe handle (in)
    LPVOID        lpBuffer,         // buffer to read to (out)
    CONST DWORD   dwByteCount,      // size of buffer (in)
    LPDWORD       lpdwActualBytes,  // actual bytes read (out)
    CONST NOTIFYPROC lpfnCallBack,     // callback for no-wait listen (in)
    CONST DWORD   dwParam);         // passed back by callback (in)

WORD SYM_EXPORT WINAPI NetPipeReadAlloc(     // read from a pipe, alloc buffer
    CONST HPIPE   hPipe,            // pipe handle (in)
    LPBYTE FAR *  lplpBuffer,       // buffer to read to (out)
    LPDWORD       lpdwActualBytes,  // actual bytes read (out)
    CONST NOTIFYPROC lpfnCallBack,     // callback for no-wait listen (in)
    CONST DWORD   dwParam);         // passed back by callback (in)

WORD SYM_EXPORT WINAPI NetPipeWrite(       // write to a pipe
    CONST HPIPE   hPipe,            // pipe handle (in)
    LPVOID        lpBuffer,         // buffer to write from (in)
    CONST DWORD   dwByteCount,      // bytes to write (in)
    LPDWORD       lpdwActualBytes,  // actual bytes written (out)
    CONST NOTIFYPROC lpfnCallBack,     // callback for no-wait listen (in)
    CONST DWORD   dwParam);         // passed back by callback (in)

WORD SYM_EXPORT PASCAL NetPipeEnum(        // enumerate pipes
     HSERVICE hService,         // service handle from NetPipeRegister (in)
     LPSTR    lpszPipeName,     // pipe name   (in)  
     ENUMPROC lpfnEnumCallback, // callback for enumeration (in)
     DWORD    dwParam);         // passed back by callback (in)

WORD SYM_EXPORT WINAPI NetPipeReconnect(  // reconnect a shutdown pipe
    HPIPE hPipe);                     // pipe handle (in)

WORD SYM_EXPORT WINAPI NetPipeDisconnect( // shutdown a pipe
    HPIPE hPipe);                     // pipe handle (in)

WORD SYM_EXPORT WINAPI NetPipeSetReadTimeout(   // set operation timeout
    HPIPE hPipe,                            // pipe handle (in)
    DWORD dwTimeout );                      // timeout for this type of operation

WORD SYM_EXPORT WINAPI NetPipeSetListenTimeout( // set operation timeout
    HPIPE hPipe,                            // pipe handle (in)
    DWORD dwTimeout );                      // timeout for this type of operation

WORD SYM_EXPORT WINAPI NetPipeSetOpenTimeout(   // set operation timeout
    HPIPE hPipe,                            // pipe handle (in)
    DWORD dwTimeout );                      // timeout for this type of operation

WORD SYM_EXPORT WINAPI NetPipeSetCloseTimeout(  // set operation timeout
    HPIPE hPipe,                            // pipe handle (in)
    DWORD dwTimeout );                      // timeout for this type of operation

WORD SYM_EXPORT WINAPI NetPipeSetWriteTimeout(  // set operation timeout
    HPIPE hPipe,                            // pipe handle (in)
    DWORD dwTimeout );                      // timeout for this type of operation

WORD SYM_EXPORT WINAPI WaitForSomething(    // wait for something to happen
     LPPIPE      lpPipe,                // pipe (contains window if windows)
     GENERICNOTIFYPROC  lpfnNotify,     // callback for no-wait listen (in)
     WORD        wParam,                // passed back to lpfnNotify
     DWORD       dwParam,               // passed back to lpfnNotify
     DWORD       dwTimeout );           // timeout for wait

#ifdef SYM_WIN

VOID SYM_EXPORT WINAPI RelinquishControl(   // wait for something to happen
    VOID );

LRESULT SYM_EXPORT CALLBACK SendCallback(   // send callback
    LPPIPE      lpPipe,                 // pipe to notify
    WPARAM      wParam);                // wParam to relay to user

#endif

VOID SYM_EXPORT WINAPI WindowsPrint(    // print to debug window
    HWND        hWnd,
    LPBYTE      szText );

WORD SYM_EXPORT WINAPI SendUserPipeCommand(            // sends command
        HPIPE       hPipe,          // pipe to send to
        LPUSERPACKET lpPacket,      // departing packet
        WORD        wCommand,       // command
        DWORD       dwDataSize );   // variable part size

WORD SYM_EXPORT WINAPI SendUserPipeCommandCallback(    // sends command with callback
        HPIPE       hPipe,          // pipe to send to
        LPUSERPACKET lpPacket,      // departing packet
        WORD        wCommand,       // command
        DWORD       dwDataSize,     // variable part size
        PACKETPROC  lpfnNotify,     // callback to notify of response
        DWORD       dwNotifyParam );    // dwParam for callback

WORD SYM_EXPORT WINAPI SendUserPipeCommandWaitResponse( // sends command, waits
        HPIPE         hPipe,            // pipe to send to
        LPSENDRECEIVE lpSendReceive,    // departing packet
        WORD          wCommand,         // command
        DWORD         dwDataSize,       // variable part size
        DWORD         dwTimeOut );      // timeout for wait

WORD SYM_EXPORT WINAPI RespondUserPipeCommandCallback( // responds to command with callback
        HPIPE       hPipe,          // pipe to send to
        LPUSERPACKET lpPacket,      // departing packet
        WORD        wCommand,       // command
        DWORD       dwDataSize,     // variable part size
        PACKETPROC  lpfnNotify,     // callback to notify of response
        DWORD       dwNotifyParam,  // dwParam for callback
        LPUSERPACKET lpSentPacket );  // packet you are responding to

WORD RegisterUserPipeIdSubclass(
        WORD            wId,            // id to subclass
        IDSUBCLASSPROC  lpfnSubClassProc ); // function that handles id

WORD DeRegisterUserPipeIdSubclass(
        WORD            wId );          // id to deregister subclass

VOID SYM_EXPORT WINAPI GetNetPipeDLLHInst(      // returns DLL instance
            LPHINSTANCE lphInstance );

#ifdef __cplusplus
}
#endif
         
#endif  // _SYMPIPE_H
