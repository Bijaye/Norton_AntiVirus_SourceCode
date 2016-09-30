// ----------------------------------------------------------------------------
// LUISSERV.H - Little UI Server HTTP Server module
// ----------------------------------------------------------------------------
//
// $Header:   S:/INCLUDE/VCS/luisserv.h_v   1.1   03 Sep 1997 18:38:00   Ray  $
//
// Description:
//      Handles all HTTP Requests and Responses
//
// ----------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/luisserv.h_v  $
// 
//    Rev 1.1   03 Sep 1997 18:38:00   Ray
// Added ServerName for error messages by the server.
// 
//    Rev 1.0   21 Aug 1997 14:25:40   Ray
// Initial revision.
// 
//    Rev 1.11   01 Apr 1997 19:53:54   RAY
// Added LUISSERV_CACHEDITEM
// 
//    Rev 1.10   18 Mar 1997 16:51:20   RAY
// Added bTempFile to REQUEST structure
// 
//    Rev 1.9   20 Jan 1997 18:32:36   RAY
// 
//    Rev 1.8   20 Nov 1996 20:10:38   RAY
// Added ServerError and ServerHalt callbacks
// 
//    Rev 1.6   14 Nov 1996 13:33:36   RAY
// Added IP filtering and password support.
// 
//    Rev 1.5   05 Nov 1996 17:25:28   RAY
// Whoops!
// 
//    Rev 1.3   24 Oct 1996 14:25:28   DJesch
// Added parameter to startServer() function.
//
//    Rev 1.2   24 Oct 1996 14:06:12   DJesch
// Added a parameter to the startServer() function.
//
//    Rev 1.1   22 Oct 1996 12:12:02   AOONWAL
// No change.
//
//    Rev 1.0   26 Sep 1996 19:29:14   RAY
// Initial revision.
//
// ----------------------------------------------------------------------------

#if !defined ( LUISSERV_H )
#define LUISSERV_H

#include "windows.h"
#include "ossocket.h"

#if defined ( __cplusplus )
extern "C" {
#endif

typedef struct tagLUISSERVERCONFIG
{
    LPSTR   lpstrServerAddress;         // Server Address in string form
    WORD    wServerPort;                // Port as host ordered word

    LPSTR   lpstrViewIPAddrs;           // Comma separated IP range strings
    LPSTR   lpstrModifyIPAddrs;

    LPSTR   lpstrViewPassword;
    LPSTR   lpstrModifyPassword;

    LPSTR   lpstrRootDir;
    LPSTR   lpstrTemplateDir;

    LPSTR   lpstrDefaultHTML;

    BOOL    ( *lpfnServerHaltCallback )( DWORD dwError, LPSTR lpstrMessage );
    BOOL    ( *lpfnServerErrorCallback )( DWORD dwError, LPSTR lpstrMessage );

    LPSTR   lpstrServerName;            // Readable program name for messages
                                        // like: "Norton Anti-Whatchamajiggy"

} LUISSERVERCONFIG, FAR * LPLUISSERVERCONFIG;
#define sLUISSERVERCONFIG ( sizeof( LUISSERVERCONFIG ))


typedef struct tagLUISSERVER
{
    LPLUISTABLE         lprRootTable;   // Server's RootTable

    HSOCKET             hListenSocket;  // Server's listen() socket
    LPSOCKET_ADDRESS    lprSocketAddr;  // Server side connection info

    LPSTR lpstrRootDir;                 // Directory containing raw files
    LPSTR lpstrTemplateDir;             // Directory containing files to be
                                        // preprocessed
    LPSTR lpstrDefaultHTML;             // default HTML file name

    BOOL bStopSign;                     // Requests server to stop
    BOOL bSuspend;                      // Requests server to suspend
    HANDLE hResumeEvent;                // If suspended, wait on this to resume

    HANDLE hThread;                     // Handle to serverThread
    UINT   uThreadID;                   // ID returned by _beginthreadex()

    HANDLE hRequestSemaphore;           // Controls number of requestThreads

    HANDLE hIPFilter;                   // This IPFilter controls view/modift
                                        // access to the server.  Cookies are:
                                        //      LUIS_NO_ACCESS
                                        //      LUIS_VIEW_ACCESS
                                        //      LUIS_MODIFY_ACCESS

    LPSTR lpstrViewPassword;
    LPSTR lpstrModifyPassword;

    BOOL    ( *lpfnServerHaltCallback )( DWORD dwError, LPSTR lpstrMessage );
    BOOL    ( *lpfnServerErrorCallback )( DWORD dwError, LPSTR lpstrMessage );

    LPSTR   lpstrServerName;            // Readable program name for messages
                                        // like: "Norton Anti-Whatchamajiggy"

} LUISSERVER, FAR * LPLUISSERVER;
#define sLUISSERVER ( sizeof( LUISSERVER ))


typedef struct tagLUISREQUEST
{
    LPLUISSERVER        lprServer;      // Server to which this request was made
    HSOCKET             hAcceptSocket;  // Server's accept() socket
    LPSOCKET_ADDRESS    lprAcceptAddr;  // Client side connection info
    HANDLE              hThread;        // Handle to requestThread
    UINT                uThreadID;      // ID returned by _beginthreadex()
    LPLUISTABLE         lprCache;       // Used throughout this request if set
    BOOL                bTempFile;      // If true, file is deleted after use
} LUISREQUEST, FAR * LPLUISREQUEST;
#define sLUISREQUEST ( sizeof( LUISREQUEST ))


typedef enum tagMETHOD
{
    HTTP_UNKNOWN = 0,
    HTTP_GET,
    HTTP_POST,
    HTTP_HEAD,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_LINK,
    HTTP_UNLINK
} METHOD;


#define LUISSERV_BUFFER_SIZE        16384
#define LUISSERV_CACHE_SIZE         1021
#define LUISSERV_MAXREQUESTTHREADS  32
#define LUISSERV_SELECTTIMEOUT      500
#define LUISSERV_POSTERRORMESSAGE_PREFIX    "This standard message goes before all error messages!<br><br>\n"
#define LUISSERV_POSTERRORMESSAGE_SUFFIX    "This standard message goes after all error messages!<br>\n"

#define LUISSERV_NO_ACCESS                  0x00000000
#define LUISSERV_VIEW_ACCESS                0x00000001
#define LUISSERV_MODIFY_ACCESS              0x00000002

#define LUISSERV_NOVIEWIPACCESS             0x004A
#define LUISSERV_NOVIEWPASSWORDACCESS       0x004B
#define LUISSERV_NOMODIFYIPACCESS           0x004C
#define LUISSERV_NOMODIFYPASSWORDACCESS     0x004D

#define LUISSERV_CACHEDITEM                 0xFFFFFFFE



typedef struct tagREQUESTHEADER
{
    METHOD              rMethod;
    BYTE                abyPath[128];
    BYTE                abyAuthorization[128];
    BYTE                abyContentType[128];
    DWORD               dwContentLength;
    WORD                wVersionMajor;
    WORD                wVersionMinor;
} REQUESTHEADER, FAR * LPREQUESTHEADER;
#define sREQUESTHEADER ( sizeof( REQUESTHEADER ))


LUISSTATUS WINAPI startServer
(
    LPLUISTABLE         lprRootTable,
    LPLUISSERVERCONFIG  lprConfig,
    LPLUISSERVER FAR *  lplprServer
);


LUISSTATUS WINAPI stopServer
(
    LPLUISSERVER lprServer
);


LUISSTATUS WINAPI suspendServer
(
    LPLUISSERVER lprServer
);


LUISSTATUS WINAPI resumeServer
(
    LPLUISSERVER lprServer
);


UINT WINAPI serverThread
(
    LPVOID lpv
);


UINT WINAPI requestThread
(
    LPVOID lpv
);


BOOL WINAPI parseRequestHeader
(
    LPSTR   lpstrRequestHeader,
    LPREQUESTHEADER lprRequestHeader
);


BOOL WINAPI readRequestHeader
(
    LPLUISREQUEST      lprRequest,
    LPREQUESTHEADER    lprRequestHeader,
    LPBYTE             lpbyRequestBuffer,
    LPWORD             lpwRequestBufferSize
);


BOOL WINAPI readRequestBody
(
    LPLUISREQUEST      lprRequest,
    LPREQUESTHEADER    lprRequestHeader,
    LPBYTE             lpbyRequestBuffer,
    WORD               wBodySize,
    LPWORD             lpwRequestBufferSize
);


void replaceChar
(
    LPSTR lpstr,
    char  c1,
    char  c2
);


BOOL sendErrorResponse
(
    DWORD           dwError,
    LPLUISREQUEST   lprRequest,
    LPREQUESTHEADER lprRequestHeader,
    LPSTR           lpstrBody
);


BOOL sendResponse
(
    LPLUISREQUEST   lprRequest,
    LPSTR           lpstrMIMEType,
    LPBYTE          lpbyBuffer,
    DWORD           dwBufferSize
);


BOOL sendResponseHeader
(
    LPLUISREQUEST   lprRequest,
    LPSTR           lpstrMIMEType,
    LPBYTE          lpbyBuffer,
    DWORD           dwBufferSize
);


LUISSTATUS readServerFile
(
    LPSTR           lpstrPath,
    LPSTR           lpstrDir,
    LPBYTE FAR *    lplpbyBuffer,
    LPDWORD         lpdwBufferSize
);


LUISSTATUS handleGet
(
    LPLUISREQUEST   lprRequest,
    LPREQUESTHEADER lprRequestHeader
);


LUISSTATUS handleHead
(
    LPLUISREQUEST   lprRequest,
    LPREQUESTHEADER lprRequestHeader
);


LUISSTATUS handlePost
(
    LPLUISREQUEST   lprRequest,
    LPREQUESTHEADER lprRequestHeader,
    LPSTR           lpstrRequestBody
);


LPSTR getMIMEType
(
    LPSTR lpstrFileName
);


void decodeURL
(
    LPSTR lpstrURL
);

void encodeURL
(
    LPSTR lpstrURL,
    LPSTR lpstrEncodedURL
);

void decodeBase64
(
    LPSTR lpstrIn,
    LPSTR lpstrOut
);

BOOL checkViewIPList
(
    LPLUISREQUEST lprRequest
);

BOOL checkModifyIPList
(
    LPLUISREQUEST lprRequest
);

BOOL checkViewPassword
(
    LPLUISREQUEST   lprRequest,
    LPREQUESTHEADER lprRequestHeader
);

BOOL checkModifyPassword
(
    LPLUISREQUEST   lprRequest,
    LPREQUESTHEADER lprRequestHeader
);

#define startThread(a,b,c)   _beginthreadex ( NULL, 0, a, b, 0, c )

#if defined ( __cplusplus )
}                                       // extern "C" {
#endif

#endif                                  // !LUISSERV_H
