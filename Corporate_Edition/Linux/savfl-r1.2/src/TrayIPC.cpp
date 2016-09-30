// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.

// TrayIPC.cpp

#include <new>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <linux/unistd.h>
#include <pwd.h>
#include <sys/time.h>
#include <errno.h>

#include "LOCK.h"
#include "PSCAN.H"

#include "MessageService.h"
#include "MessageConstants.h"
#include "TrayIPC.h"
#include "commonids.h"

#include "defutils.h"

// The global configuration options structure
extern CONFIG_OPTIONS gtConfigOptions;

// NOTE: Since this class is linux-specific, we could be using a pthread mutex
// (or multiple pthread mutexes) directly
INIT_LOCK();


///////////////////////////////////////////////////////////////////////////////
// SocketMessageTransport
// 
// Class provides a MessageTransport over a socket. 
///////////////////////////////////////////////////////////////////////////////
class SocketMessageTransport : public MessageTransport
{
public:
    // CTOR/DTOR
    SocketMessageTransport( int socket );
    virtual ~SocketMessageTransport();

private:
    // Forbid default construction, copying, and assignment
    SocketMessageTransport();
    SocketMessageTransport( const SocketMessageTransport& );
    SocketMessageTransport& operator=( const SocketMessageTransport& );

public:
    // Implementation of MessageTransport:

    // We do nothing on sendComplete and sendFailed callbacks
    void sendComplete( const Message& msg )
        { (void) msg; }
    void sendFailed( const Message& msg, int code )
        { (void) msg; (void) code; }

    ssize_t read( void* buf, size_t count, ErrorCode& err );
    ssize_t write( const void *buf, size_t count, ErrorCode& err );

    void outPending( void );

public:
    // Accessors for socket member and flags indicating transport state
    inline int GetSocket()
        { return m_socket; }

    inline bool HasPendingSend()
        { return m_hasPending; }

    inline bool IsAwaitingClose()
        { return m_needsClose; }

private:
    int  m_socket;     // socket fd for transport
    bool m_hasPending; // flag: is there data waiting for write?
    bool m_needsClose; // flag: should this transport be closed?
};

// SocketMessageTransport CTOR
SocketMessageTransport::SocketMessageTransport( int socket )
    : m_socket( socket ),
      m_hasPending( false ),
      m_needsClose( false )
{
}

// SocketMessageTransport DTOR
SocketMessageTransport::~SocketMessageTransport()
{
    // We could check for errors here, but it's really too late to do anything
    // about it.
    close( m_socket );
}

// SocketMessageTransport::read (implementing MessageTransport)
ssize_t SocketMessageTransport::read( void* buf, size_t count, ErrorCode& err )
{
    return ::recv( m_socket, buf, count, MSG_DONTWAIT );
}

// SocketMessageTransport::write (implementing MessageTransport)
//
// If the write succeeds, the hasPending flag is cleared
ssize_t SocketMessageTransport::write( const void *buf, size_t count, ErrorCode& err )
{
    size_t ret = ::send( m_socket, buf, count, MSG_DONTWAIT | MSG_NOSIGNAL );
    if (static_cast<size_t>(-1) == ret)
    {
        if (EAGAIN == errno)
        {
            dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: write performed on un-ready socket\n" );
        }
        else if (EPIPE == errno)
        {
            dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: write performed on closed socket\n" );
            m_needsClose = true;
        }
        else
        {
            dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: error writing to socket: %d (%s)\n", errno, strerror( errno ) );
        }
        return ret;
    }
    if (ret > 0)
    {
        m_hasPending = false;
    }
    return ret;
}

// SocketMessageTransport::outPending (implementing MessageTransport)
//
// Sets the hasPending flag
void SocketMessageTransport::outPending( void )
{
    m_hasPending = true;
}



///////////////////////////////////////////////////////////////////////////////
// TrayStatusService
// 
// Class provides the status service to tray clients
///////////////////////////////////////////////////////////////////////////////
class TrayStatusService : public MessageService
{
public:
    // CTOR/DTOR
    TrayStatusService( TrayIPCConnection& conn, TrayIPCManager& mgr )
        : m_conn( conn ), m_mgr( mgr )
        {}
    virtual ~TrayStatusService()
        {}

private:
    // Forbid default construction, copying, and assignment
    TrayStatusService();
    TrayStatusService( const TrayStatusService& );
    TrayStatusService& operator=( const TrayStatusService& );

private:
    static const int SERVICE_VERSION = 0;

public:
    // Implements MessageService:
    int  getId()
        { return MESSAGE_SVC_TRAYNOTIFY; }
    int  getVersion()
        { return SERVICE_VERSION; }
    void messageReceived( const Message& msg );

    // we do nothing for messageSent and messageFailed
    void messageSent( const Message& msg )
        { (void) msg; }
    void messageFailed( const Message& msg, int code )
        { (void) msg; (void) code; }

    void transportRejected()
        {}

private:
    // We hold a reference to the connection containing this service
    TrayIPCConnection& m_conn;

    // We hold a reference to the IPC Manager that understands how to generate
    // the messages we need
    TrayIPCManager& m_mgr;
};

// TrayStatusService::messageReceived (implementing MessageService)
//
// Callback upon receipt of a complete message. The status service
// responds to status requests and ignores incoming messages with
// unknown opcodes
void TrayStatusService::messageReceived( const Message& msg )
{
    if (msg.serviceVer != getVersion())
    {
        dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: received message with incorrect version: got %d, expected %d\n",
                 msg.serviceVer, getVersion() );
        return;
    }

    int payloadLen = msg.getPayloadLen();

    if (static_cast<size_t>(payloadLen) < sizeof(DWORD))
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: received a message too short to contain an opcode\n" );
        return;
    }

    DWORD opCode = 0;
    unsigned char* p = msg.getPayload();
    unsigned char* base = p;
    opCode = UNPACK_U32( p );
    int consumed = p - base;
    int left = payloadLen - consumed;

    switch ( opCode )
    {
    case TRAYNOTIFY_OP_STATUS_REQUEST:
        m_mgr.HandleStatusRequest( m_conn, msg.tag, p, left );
        break;
    case TRAYNOTIFY_OP_VERSION_REQUEST:
        m_mgr.HandleVersionRequest( m_conn, msg.tag );
        break;
    case TRAYNOTIFY_OP_SCAN_STATUS_REQUEST:
        m_mgr.HandleScanStatusRequest( m_conn, msg.tag );
        break;
    case TRAYNOTIFY_OP_LIVE_UPDATE_REQUEST:
        m_mgr.HandleLiveUpdateRequest( m_conn, msg.tag );
        break;
    case TRAYNOTIFY_OP_DEFS_CHECK_REQUEST:
        m_mgr.HandleDefsCheckRequest( m_conn, msg.tag );
        break;
    default:
        // If it's an opcode we don't understand how to deal with, junk it.
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: received message with unexpected opcode: %d\n", opCode );
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// TrayIPCConnection
// 
// Class encapsulates an entire connection with a tray client. Manages the
// transport class, processor class, and service classes for a single client
// connection.
///////////////////////////////////////////////////////////////////////////////
class TrayIPCConnection
{
public:
    // CTOR/DTOR
    TrayIPCConnection( int socket, TrayIPCManager& mgr )
        : m_transport( socket ),
          m_processor( m_transport ),
          m_statusSvc( *this, mgr ),
          m_statusSvcIdx( -1 )
        { m_user[0] = 0; }
    virtual ~TrayIPCConnection()
        { Destroy(); }

private:
    // Forbid default construction, copying, and assignment
    TrayIPCConnection();
    TrayIPCConnection( const TrayIPCConnection& );
    TrayIPCConnection& operator=( const TrayIPCConnection& );

public:
    // Initialization and destruction methods
    bool Initialize( char* workBuf, size_t bufLen );
    void Destroy();

public:
    // useful methods

    // accessor method for socket fd from transport
    inline int GetSocket()
        { return m_transport.GetSocket(); }

    // accessor method for transport's flag to indicate data waiting for send
    inline bool HasPendingSend()
        { return m_transport.HasPendingSend(); }

    // accessor method for transport's flag indicating waiting for close
    inline bool IsAwaitingClose()
        { return m_transport.IsAwaitingClose(); }

    // accessor method for username of connected tray process
    inline const char* GetConnectedUser()
        { return m_user; }

    // entry functions to start a read or write operation over this connection
    // before calling, the connection should be completely ready to begin the
    // operation--the socket should be ready, and in the case of a write, there
    // must be data ready to be sent (see HasPendingSend() above)
    bool PerformRead();
    void PerformWrite();

    // Enumeration of service types available on this connection
    enum ServiceType
    {
        STATUS_SVC,
        REGISTRY_SVC,
    };

    // Asynchronous, non-reentrant method for sending data to a client.
    // The tag value is directly used in the sent message.
    inline bool SendDataNoLock( ServiceType type,
                                int tag,
                                int payloadLen,
                                unsigned char* payload );

private:
    static const size_t READ_BUFSIZE = 4096;

private:
    SocketMessageTransport m_transport; // Transport for the connection
    MessageProcessor m_processor;       // Processor for the connection
    TrayStatusService m_statusSvc;      // Status Service for the connection
    int m_statusSvcIdx;                 // Index for the status service
    char m_user[NAME_SIZE];             // Name of the user on the client end
};

/**
 * Context to handle completion of operations on different threads or from external calls.
 */
class TrayIPCContext
{
public:
    TrayIPCContext( TrayIPCManager& mgr, TrayIPCConnection* conn, int tag, int opcode ) :
        m_mgr( mgr ),
        m_addr( conn ),
        m_socket( conn ? conn->GetSocket() : 0 ),
        m_tag( tag ),
        m_opcode( opcode )
    {
        if ( conn )
        {
            vpstrncpy( m_user, conn->GetConnectedUser(), sizeof(m_user) );
        }
        else
        {
            m_user[0] = 0;
        }
    }
    virtual ~TrayIPCContext()
        {}

    TrayIPCManager& m_mgr;
    TrayIPCConnection* m_addr;
    int m_socket;
    char m_user[NAME_SIZE];             // Name of the user on the client end
    int m_tag;
    int m_opcode;
};

// TrayIPCConnection::Initialize
//
// initializes the connection object. Requires a buffer large enough to fit
// a getpwuid_r call's string buffers into.
//
// Arguments:
//  buf - a work buffer used for the uid lookup upon socket connection. Must
//        be large enough to fit passwd buffers into (from a getpwuid_r call)
//  bufLen - length of the work buffer in bytes
//
// Returns:
//  false on error, true on success
bool TrayIPCConnection::Initialize( char* buf, size_t bufLen )
{
    ucred cred;
    socklen_t credLen = sizeof(cred);

    if (-1 == getsockopt( m_transport.GetSocket(), SOL_SOCKET, SO_PEERCRED, &cred, &credLen ))
    {
        dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: Failed to get peer credentials from socket: %d (%s)\n",
                 errno, strerror( errno ) );
        return false;
    }
    else
    {
        passwd pwd = {0};
        passwd* ret = NULL;
        int retval = getpwuid_r( cred.uid, &pwd, buf, bufLen, &ret );
        if (NULL == ret)
        {
            dprintfTag3( DEBUGCOMMPACKET, "TrayIPC: Failed to get user record for user %d: %d (%s)\n",
                     cred.uid, retval, strerror( retval ) );
            return false;
        }
        else
        {
            if (ret->pw_uid == cred.uid)
            {
                strncpy( m_user, ret->pw_name, NAME_SIZE - 1 );
                m_user[NAME_SIZE-1] = 0;
                dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: Matched userid %d to user %s\n", cred.uid, m_user );
            }
        }
    }

    m_statusSvcIdx = m_processor.addService( &m_statusSvc );
    if (-1 == m_statusSvcIdx)
    {
        Destroy();
        return false;
    }

    return true;
}

// TrayIPCConnection::Destroy
//
void TrayIPCConnection::Destroy()
{
    m_statusSvcIdx = -1;
    m_user[0] = 0;
}

// TrayIPCConnection::PerformRead
//
// Entrypoint for read operation on connection. Peferable to call this method
// when the socket is ready to be read from.
//
// Returns:
//  true if connection is ok, false if connection should be closed
bool TrayIPCConnection::PerformRead()
{
    unsigned char buf[READ_BUFSIZE];
    size_t count;
    MessageTransport::ErrorCode err = MessageTransport::e_ok;

    count = m_transport.read( reinterpret_cast<void*>(buf), 4096, err );
    if (0 == count)
    {
        // That socket must be closed!
        return false;
    }
    else if (static_cast<size_t>(-1) == count)
    {
        if (EAGAIN == errno)
        {
            dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: read performed on un-ready socket\n" );
        }
        else
        {
            dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: failed to read data from transport: %d (%s)\n", errno, strerror( errno ) );
        }
    }
    else
    {
        if (false == m_processor.feed( buf, static_cast<int>( count ) ))
        {
            dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: failed feed data to processor\n" );
        }
    }

    return true;
}

// TrayIPCConnection::PerformWrite
//
// Entrypoint for write operation on connection. Peferable to call this method
// when the socket is ready to be written to, and there is waiting data to be
// written
void TrayIPCConnection::PerformWrite()
{
    if (m_processor.send())
    {
        m_transport.outPending();
    }
}

// TrayIPCConnection::SendDataNoLock
//
// Enqueues a message on the current connection with the correct service
// and tag. Not reentrant.
//
// Arguments:
//  type - The type of service the payload should be sent through
//  tag  - The message tag to be used. Server-originated messages use 0
//  payloadLen - Length of the payload to be sent, in bytes
//  payload    - Payload buffer
//
// Returns:
//  false on error, true on success
inline bool TrayIPCConnection::SendDataNoLock( ServiceType type,
                                               int tag,
                                               int payloadLen,
                                               unsigned char* payload )
{
    int svcIdx;
    switch (type)
    {
    case STATUS_SVC:
        svcIdx = m_statusSvcIdx;
        break;
    default:
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: data send with unknown service type %d\n", type );
        return false;
    }

    if (-1 == svcIdx)
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: attempting to SendDataNoLock on uninitialized connection\n" );
        return false;
    }

    if (0 == m_processor.enque( svcIdx, 0, tag, payloadLen, payload ))
    {
        return true;
    }
    else
    {
        return false;
    }
}


///////////////////////////////////////////////////////////////////////////////
// TrayIPCManager
// 
// Class for managing IPC with savTray(s)
//
// The implementations for TrayIPCManager methods are included here. 
///////////////////////////////////////////////////////////////////////////////

// TrayIPCManager CTOR
TrayIPCManager::TrayIPCManager()
    : m_state( MANAGER_UNINITIALIZED ),
      m_connections( 0 ),
      m_mainSocket( -1 ),
      m_workerCount( 0 ),
      m_getpwBuf( NULL ),
      m_getpwBufLen( static_cast<size_t>(-1) ),
      m_APEnabled( false ),
      m_LUPermitted( true ),
      m_showIcon( true ),
      m_productVersion( 0 ),
      m_productVersionStringLen( 0 ),
      m_engineVersion( 0 ),
      m_engineVersionStringLen( 0 ),
      m_defsYear( 0 ),
      m_defsMonth( 0 ),
      m_defsDay( 0 ),
      m_defsRevision( 0 ),
      m_defsVersionStringLen( 0 ),
      m_scanStatus( S_NEVER_RUN )
{
    pthread_mutex_init(&m_workerMutex, NULL);
    pthread_cond_init(&m_workerCond, NULL);
    
    for (size_t i = 0; i < MAX_CONNECTIONS; ++i)
    {
        m_connTable[i] = NULL;
    }

    m_productVersionString[0] = 0;
    m_engineVersionString[0] = 0;
    m_defsVersionString[0] = 0;
}

// TrayIPCManager DTOR
TrayIPCManager::~TrayIPCManager()
{
    Destroy();
    pthread_cond_destroy(&m_workerCond);
    pthread_mutex_destroy(&m_workerMutex);
}

// TrayIPCManager::Initialize()
//
// Sets up the IPC manager for use.
// Returns:
// false on failure, true on success
bool TrayIPCManager::Initialize()
{
    // First get the system-defined maximum buffer size for holding getpw*_r
    // results. 
    m_getpwBufLen = sysconf( _SC_GETPW_R_SIZE_MAX );
    if (static_cast<size_t>(-1) == m_getpwBufLen)
    {
        dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: failed to get buffer size for getpw functions: %d (%s)\n",
                 errno, strerror( errno ) );
        return false;
    }

    // Now allocate a buffer of the appropriate size
    m_getpwBuf = new(std::nothrow) char[m_getpwBufLen];

    if (NULL == m_getpwBuf)
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: failed to allocate buffer for pw operations\n" );
        Destroy();
        return false;
    }

    // Create the main listening socket
    m_mainSocket = socket( PF_UNIX, SOCK_STREAM, 0 );
    if (-1 == m_mainSocket)
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: failed to initialize main socket\n" );
        Destroy();
        return false;
    }

    // Create the socket address
    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    if (isSocketAddressAbstract( SOCKNAME_TRAY ))
    {
        // For abstract sockets, the socket name is not null-terminated, so
        // we have to wipe the entire buffer
        memset( addr.sun_path, 0, sizeof(addr.sun_path) );
        // socket address is abstract, so skip past the initial NULL
        char* s = SOCKNAME_TRAY;
        ++s;
        strncpy( addr.sun_path + 1, s, sizeof(addr.sun_path) - 1 );
    }
    else
    {
        strncpy( addr.sun_path, SOCKNAME_TRAY, sizeof(addr.sun_path) );
    }

    // Bind the socket to the address
    if (-1 == bind( m_mainSocket, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_un) ))
    {
        dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: failed to bind main socket %d (%s)\n", errno, strerror(errno) );
        Destroy();
        return false;
    }

    if (!isSocketAddressAbstract( SOCKNAME_TRAY ))
    {
        if (-1 == chmod( SOCKNAME_TRAY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH ))
        {
            dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: failed to set permissions on socket: %d (%s)\n",
                     errno, strerror( errno ) );
        }
    }

    // Set up the socket to received connections
    if (-1 == listen( m_mainSocket, MAX_CONNECTIONS ))
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: failed to listen on main socket\n" );
        Destroy();
        return false;
    }

    m_state = MANAGER_INITIALIZED;

    // Spawn our two worker threads
    MyBeginThread( SocketAcceptThread, reinterpret_cast<void*>(this), "Tray Socket Accept Thread" );
    MyBeginThread( IPCWorkerThread, reinterpret_cast<void*>(this), "Tray IPC Worker Thread" );

    return true;
}

// TrayIPCManager::Destroy()
//
// Shuts down the IPC manager. 
void TrayIPCManager::Destroy()
{
    LOCK();
    State originalState = m_state;
    if (MANAGER_INITIALIZED == originalState)
    {
        m_state = MANAGER_SHUTTING_DOWN;
    }
    UNLOCK();

    if (MANAGER_INITIALIZED == originalState)
    {
        // Wait for the worker threads to die.  The worker threads will destroy
        // the main socket and the connection table.
        struct timeval now;
        struct timespec timeout;
        int waitrc;

        pthread_mutex_lock(&m_workerMutex);
        gettimeofday(&now, NULL);
        timeout.tv_sec = now.tv_sec + (TEARDOWN_THREAD_TIMEOUT_MSECS / 1000);
        timeout.tv_nsec = now.tv_usec * 1000;
        waitrc = 0;
        while(m_workerCount > 0 && waitrc != ETIMEDOUT)
        {
            waitrc = pthread_cond_wait(&m_workerCond, &m_workerMutex);
        }
        pthread_mutex_unlock(&m_workerMutex);
    }
    else if (MANAGER_UNINITIALIZED == originalState)
    {
        // We have no threads to do it, so we must destroy the main socket
        // and connection table here.

        // Close the main socket. This also signals the worker threads to quit
        DestroyMainSocket();

        // Clear out the connection table
        DestroyConnectionTable();
    }
    else // if MANAGER_SHUTTING_DOWN == originalState
    {
        // Don't do anything if we're already shutting down
        return;
    }

    // Delete the work buffer--this gets done here regardless of whether
    // the threads got started or not.
    delete[] m_getpwBuf;
    m_getpwBuf = NULL;
    m_getpwBufLen = static_cast<size_t>(-1);
}

// TrayIPCManager::UpdateAPStatus()
//
// Method to be called when AP's status changes.
//
// Arguments:
//   APEnabled - true if AP is active
void TrayIPCManager::UpdateAPStatus( bool APEnabled )
{
    LOCK();
    if (APEnabled != m_APEnabled)
    {
        m_APEnabled = APEnabled;
        BroadcastStatus();
    }
    UNLOCK();
}

// TrayIPCManager::UpdateLUPermission()
//
// Method to be called when LiveUpdate's permission status changes.
//
// Arguments:
//   LUPermitted - true if LiveUpdate is permitted
void TrayIPCManager::UpdateLUPermission( bool LUPermitted )
{
    LOCK();
    if (LUPermitted != m_LUPermitted)
    {
        m_LUPermitted = LUPermitted;
        BroadcastStatus();
    }
    UNLOCK();
}

// TrayIPCManager::UpdateShowDesktopIcon()
//
// Method to be called when desktop icon visibility status changes.
//
// Arguments:
//   LUPermitted - true if LiveUpdate is permitted
void TrayIPCManager::UpdateShowDesktopIcon( bool showIcon )
{
    LOCK();
    if (showIcon != m_showIcon)
    {
        m_showIcon = showIcon;
        BroadcastStatus();
    }
    UNLOCK();
}

// TrayIPCManager::UpdateProductVersion()
//
// Method to be called when the product version changes.
//
// Arguments:
//   productVersion - product version packed into a DWORD, as stored in the
//                    registry
void TrayIPCManager::UpdateProductVersion( DWORD productVersion )
{
    LOCK();
    if (productVersion != m_productVersion || 0 == m_productVersionStringLen)
    {
        m_productVersion = productVersion;
        m_productVersionStringLen = vpsnprintf( m_productVersionString,
                                                sizeof(m_productVersionString),
                                                "%d.%d.%d.%d",
                                                LOWORD(m_productVersion) / 100,
                                                (LOWORD(m_productVersion) % 100) / 10,
                                                LOWORD(m_productVersion) % 10,
                                                HIWORD(m_productVersion) );
        if (m_productVersionStringLen < sizeof(m_productVersionString))
        {
            ++m_productVersionStringLen; // snprintf returns string length not
                                         // including trailing null, so add it
        }
        else
        {
            // There was a truncation!
            dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: truncated product version string\n" );
            m_productVersionStringLen = sizeof(m_productVersionString);
        }

        BroadcastVersion();
    }
    UNLOCK();
}

// TrayIPCManager::UpdateEngineVersion()
//
// Method to be called when the engine version changes.
//
// Arguments:
//   engineVersion - engine version packed into a DWORD, as stored in the
//                   registry
void TrayIPCManager::UpdateEngineVersion( DWORD engineVersion )
{
    LOCK();
    if (engineVersion != m_engineVersion || 0 == m_engineVersionStringLen)
    {
        m_engineVersion = engineVersion;
        m_engineVersionStringLen = vpsnprintf( m_engineVersionString,
                                               sizeof(m_engineVersionString),
                                               "%d.%d.%d.%d",
                                               (int)(0xff & (m_engineVersion >> 24)),
                                               (int)(0xff & (m_engineVersion >> 16)),
                                               (int)(0xff & (m_engineVersion >>  8)),
                                               (int)(0xff & (m_engineVersion)) );
        if (m_engineVersionStringLen < sizeof(m_engineVersionString))
        {
            ++m_engineVersionStringLen; // snprintf returns string length not
                                        // including trailing null, so add it.
        }
        else
        {
            // There was a truncation!
            dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: truncated engine version string\n" );
            m_engineVersionStringLen = sizeof(m_engineVersionString);
        }

        BroadcastVersion();
    }
    UNLOCK();
}

// TrayIPCManager::UpdateDefsVersion()
//
// Method to be called when the defs version changes.
//
// Arguments:
//   defsVersion - definitions version, packed into a single DWORD, as
//                 packed by VDBVersion
void TrayIPCManager::UpdateDefsVersion( DWORD defsVersion )
{
    // format of defs version information:
    // bits 18-24: year since 1998
    // bits 14-17: month of the year, 1-indexed
    // bits  9-13: day of the month, 1-indexed
    // bits  0- 8: revision number
    WORD  defsYear = (1998 + (0x07F & (defsVersion >> 18)));
    WORD  defsMonth =        (0x00F & (defsVersion >> 14));
    WORD  defsDay =          (0x01F & (defsVersion >>  9));
    DWORD defsRevision =     (0x1FF &  defsVersion);

    LOCK();
    if (   defsYear     != m_defsYear
        || defsMonth    != m_defsMonth
        || defsDay      != m_defsDay
        || defsRevision != defsRevision
        || 0 == m_defsVersionStringLen)
    {
        m_defsYear = defsYear;
        m_defsMonth = defsMonth;
        m_defsDay = defsDay;
        m_defsRevision = defsRevision;

        m_defsVersionStringLen = vpsnprintf( m_defsVersionString,
                                             sizeof(m_defsVersionString),
                                             "%d/%d/%d rev. %lu",
                                             m_defsMonth,
                                             m_defsDay,
                                             m_defsYear,
                                             m_defsRevision );
        if (m_defsVersionStringLen < sizeof(m_defsVersionString))
        {
            ++m_defsVersionStringLen; // snprintf returns string length not
                                      // including trailing null, so add it.
        }
        else
        {
            // There was a truncation!
            dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: truncated defs version string\n" );
            m_defsVersionStringLen = sizeof(m_defsVersionString);
        }

        BroadcastVersion();
    }
    UNLOCK();
}

// TrayIPCManager::UpdateScanStatus()
//
// Method to be called when scan status changes.
//
// Arguments:
//   scanStatus - encoded scan status
void TrayIPCManager::UpdateScanStatus( DWORD scanStatus )
{
    LOCK();
    if (scanStatus != m_scanStatus)
    {
        m_scanStatus = scanStatus;
        BroadcastScanStatus();
    }
    UNLOCK();
}

// TrayIPCManager::SendScanNotification()
//
// Sends a scan notification to any clients that are qualified to receive
// it (based on the uid of the client end of the connection). 
// 
// Arguments:
//   scanType    // type of scan (manual, AP, ...)
//   eventType   // type of event (infection found)
//   threatName  // name of detected threat
//   fileName    // name of scanned file
//   location    // current location of scanned file
//   computer    // name of computer
//   user        // name of user initiating scan
//   actionTaken // actions taken on scan
//   actionDesc  // disposition of actions taken
//   format      // the notification format (custom message)
void TrayIPCManager::SendScanNotification( DWORD dateSeconds,
                                           const char* scanType,
                                           const char* eventType,
                                           const char* threatName,
                                           const char* fileName,
                                           const char* location,
                                           const char* computer,
                                           const char* user,
                                           const char* actionTaken,
                                           const char* actionDesc,
                                           const char* format )
{
    // base buffer length includes the following:
    // * 4-byte opcode
    // * 40-bytes for 10x4-byte string length fields
    // * 4-bytes for 1x4-byte numeric value
   size_t buflen = 48;

    // Get string lengths for each of the strings we will be using to construct
    // the payload buffer, including room for the null terminator for each
    DWORD scanTypeLen = strlen( scanType ) + 1;
    DWORD eventTypeLen = strlen( eventType ) + 1;
    DWORD threatNameLen = strlen( threatName ) + 1;
    DWORD fileNameLen = strlen( fileName ) + 1;
    DWORD locationLen = strlen( location ) + 1;
    DWORD computerLen = strlen( computer ) + 1;
    DWORD userLen = strlen( user ) + 1;
    DWORD actionTakenLen = strlen( actionTaken ) + 1;
    DWORD actionDescLen = strlen( actionDesc ) + 1;
    DWORD formatLen = strlen( format ) + 1;

    // Get a total length for the payload buffer
    buflen += scanTypeLen + eventTypeLen + threatNameLen + fileNameLen +
              locationLen + computerLen + userLen + actionTakenLen +
              actionDescLen + formatLen;

    // Allocate the payload buffer
    unsigned char* buf = new(std::nothrow) unsigned char[buflen];
    if (NULL == buf)
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: Failed to allocate payload buffer for scan notification\n" );
        return;
    }

    // working pointer into the payload buffer
    unsigned char* p = buf;

    // Write in opcode
    PACK_U32( p, TRAYNOTIFY_OP_SCAN_NOTIFY );

    // Write the event time.
    PACK_U32( p, dateSeconds );

    // Write scan type string, prefixed by length
    PACK_STR( p, scanType, scanTypeLen );

    // Write event type string, prefixed by length
    PACK_STR( p, eventType, eventTypeLen );

    // Write threat name string, prefixed by length
    PACK_STR( p, threatName, threatNameLen );

    // Write file name string, prefixed by length
    PACK_STR( p, fileName, fileNameLen );

    // Write location string, prefixed by length
    PACK_STR( p, location, locationLen );

    // Write computer name string, prefixed by length
    PACK_STR( p, computer, computerLen );

    // Write user name string, prefixed by length
    PACK_STR( p, user, userLen );

    // Write action string, prefixed by length
    PACK_STR( p, actionTaken, actionTakenLen );

    // Write action description type string, prefixed by length
    PACK_STR( p, actionDesc, actionDescLen );

    // Write format string, prefixed by length
    PACK_STR( p, format, formatLen );

#ifdef DEBUG
    assert( buflen == static_cast<size_t>(p - buf));
#endif

    LOCK();
    // Walk the connection table
    for (size_t i = 0; i < m_connections; ++i)
    {
        // If the connected user matches the user in the notification,
        // then send the data
        if (0 == strcmp( m_connTable[i]->GetConnectedUser(), user ))
        {
            if (false == m_connTable[i]->SendDataNoLock( TrayIPCConnection::STATUS_SVC, 0, buflen, buf ))
            {
                dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: failed to send scan notification to client %d (%s)\n",
                             i, m_connTable[i]->GetConnectedUser() );
            }
        }
    }
    UNLOCK();

    // Free the payload buffer
    delete[] buf;
}


void TrayIPCManager::OperationComplete( TrayIPCContext* context, DWORD rc )
{
    if ( context )
    {
        // Only process contexts created by this manager instance
        if ( &(context->m_mgr) == this )
        {
            LOCK();

            TrayIPCConnection* conn = 0;
            bool processMore = true;
            int connIndex = -1;
            if ( context->m_addr )
            {
                // Walk the connection table
                for ( size_t i = 0; i < context->m_mgr.m_connections; i++ )
                {
                    if ( context->m_mgr.m_connTable[i] == context->m_addr )
                    {
                        // Found the matching pointer. However, check to see
                        // if it's the original connection, or a new one that just took the same location
                        if ( (0 == strcmp( context->m_mgr.m_connTable[i]->GetConnectedUser(), context->m_user ))
                             && (context->m_mgr.m_connTable[i]->GetSocket() == context->m_socket)
                            )
                        {
                            connIndex = i;
                            conn = m_connTable[i];
                        }
                        break;
                    }
                }
                processMore = (conn != 0);
            }

            if ( processMore )
            {
                switch ( context->m_opcode )
                {
                    case TRAYNOTIFY_OP_DEFS_CHECK_REQUEST:
                    {
                        HandleDefsCheckCompletion( conn, connIndex, context->m_tag, rc );
                    }
                    break;
                }
            }

            UNLOCK();
        }
        // All done. Cleanup.
        delete context;
    }
}


/**
 * Handler method for completion of defs update requests
 *
 * @param conn      Pointer to the connection originating the request, or NULL.
 * @param index     index in the table of the originating connection
 * @param tag       Tag of the request message
 * @param rc        Result of the operation.
 *
 * @note IPC lock must be acquired prior to calling
 */
void TrayIPCManager::HandleDefsCheckCompletion( TrayIPCConnection* conn, int index, int tag, DWORD rc )
{
    if ( conn )
    {
        unsigned char rspbuf[TRAYNOTIFY_MSGSIZE_DEFS_CHECK_RESPONSE];
        size_t rspbuflen = TRAYNOTIFY_MSGSIZE_DEFS_CHECK_RESPONSE;

        // Produce a message payload of the needed type
        if ( false == FillDefsCheckResponsePayload( rspbuf, rspbuflen, ERROR_SUCCESS ) )
        {
#ifdef DEBUG
            // If there was an error, it shouldn't be because we didn't provide
            // enough room for the payload
            assert( TRAYNOTIFY_MSGSIZE_DEFS_CHECK_RESPONSE >= rspbuflen );
#endif
            dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: Defs check response payload required an unexpected size: %d\n", rspbuflen );
        }
        else
        {
            if ( false == conn->SendDataNoLock( TrayIPCConnection::STATUS_SVC, tag, rspbuflen, rspbuf ) )
            {
                dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: failed to send defs check response to client %d (%s)\n",
                             index, conn->GetConnectedUser() );
            }
        }
    }
}

// TrayIPCManager::HandleStatusRequest()
//
// Reply handler for requests for the status message.
// 
// NOTE: IPC lock must be acquired prior to calling
//
// Arguments:
//   conn   - the TrayIPCConnection object originating the request
//   tag    - the tag value of the request message
//   buf    - pointer to remaining payload bytes from the request
//   buflen - number of payload bytes provided
void TrayIPCManager::HandleStatusRequest( TrayIPCConnection& conn, int tag, const unsigned char* buf, size_t buflen )
{
    (void)buf;
    (void)buflen;

    unsigned char rspbuf[TRAYNOTIFY_MSGSIZE_STATUS_UPDATE];
    size_t rspbuflen = TRAYNOTIFY_MSGSIZE_STATUS_UPDATE;

    // Produce a message payload of the needed type
    if (false == FillStatusPayload( rspbuf, rspbuflen ))
    {
#ifdef DEBUG
        // If there was an error, it shouldn't be because we didn't provide
        // enough room for the payload
        assert( TRAYNOTIFY_MSGSIZE_STATUS_UPDATE >= rspbuflen );
#endif
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: Status payload required an unexpected size: %d\n", rspbuflen );
        return;
    }

    // Send the message back to the originator
    if (false == conn.SendDataNoLock( TrayIPCConnection::STATUS_SVC, tag, rspbuflen, rspbuf ))
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: failed to send status response to client\n" );
    }
}

// TrayIPCManager::HandleVersionRequest()
//
// Reply handler for requests for the version message
// 
// NOTE: IPC lock must be acquired prior to calling
//
// Arguments:
//   conn   - the TrayIPCConnection object originating the request
//   tag    - the tag value of the request message
void TrayIPCManager::HandleVersionRequest( TrayIPCConnection& conn, int tag )
{
    size_t rspbuflen = 0;

    // How much room do we need to accommodate the version information?
    FillVersionPayload( NULL, rspbuflen );

    // We expect that call to fail and provide the needed size
    if (0 >= rspbuflen)
    {
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: Unexpected error getting version payload size: returned %d\n", rspbuflen );
        return;
    }

    // Attempt to allocate the space we need
    unsigned char* rspbuf = new(std::nothrow) unsigned char[rspbuflen];
    if (NULL == rspbuf)
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: Failed to allocate payload rspbuffer for version response: %s\n" );
        return;
    }

    // OK, now try to obtain a message payload
    if (FillVersionPayload( rspbuf, rspbuflen ))
    {
        // And try to send that message back to the requestor
        if (false == conn.SendDataNoLock( TrayIPCConnection::STATUS_SVC, tag, rspbuflen, rspbuf ))
        {
            dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: failed to send version response to client\n" );
        }
    }
    else
    {
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: Unexpected failure to fill version payload: returned size %d\n", rspbuflen );
        // No message payload!
    }

    delete[] rspbuf;
}

// TrayIPCManager::HandleScanStatusRequest()
//
// Reply handler for requests for the scan status message
// 
// NOTE: IPC lock must be acquired prior to calling
//
// Arguments:
//   conn   - the TrayIPCConnection object originating the request
//   tag    - the tag value of the request message
void TrayIPCManager::HandleScanStatusRequest( TrayIPCConnection& conn, int tag )
{
    unsigned char rspbuf[TRAYNOTIFY_MSGSIZE_SCAN_UPDATE];
    size_t rspbuflen = TRAYNOTIFY_MSGSIZE_SCAN_UPDATE;

    // Produce a message payload of the needed type
    if (false == FillScanStatusPayload( rspbuf, rspbuflen ))
    {
#ifdef DEBUG
        // If there was an error, it shouldn't be because we didn't provide
        // enough room for the payload
        assert( TRAYNOTIFY_MSGSIZE_SCAN_UPDATE >= rspbuflen );
#endif
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: Scan status payload required an unexpected size: %d\n", rspbuflen );
        return;
    }

    if (false == conn.SendDataNoLock( TrayIPCConnection::STATUS_SVC, tag, rspbuflen, rspbuf ))
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: failed to send scan status response to client\n" );
    }
}

/**********************************************************************************/
void LogLiveUpdateRequest( const char* connName, bool permitted )
{
    EVENTBLOCK log;
    const char* safeConnName = connName ? connName : "";
    int additionalRoomForConnName = strlen( safeConnName );
    char line[64 + additionalRoomForConnName];

    memset(&log,0,sizeof(log));
    vpsnprintf(line, sizeof(line), permitted?(LS(IDS_LIVE_UPDATE_REQUEST_PERMITTED)):(LS(IDS_LIVE_UPDATE_REQUEST_BLOCKED)), connName);

    log.Description = line;
    log.logger = LOGGER_System;
    log.hKey[0] = 0;
    log.Category = GL_CAT_PATTERN;
    log.Event =  GL_EVENT_PATTERN_DOWNLOAD;
    GlobalLog(&log);
}
/**********************************************************************************/
// TrayIPCManager::HandleLiveUpdateRequest()
//
// Reply handler for requests for a live update session
// 
// NOTE: IPC lock must be acquired prior to calling
//
// Arguments:
//   conn   - the TrayIPCConnection object originating the request
//   tag    - the tag value of the request message
void TrayIPCManager::HandleLiveUpdateRequest( TrayIPCConnection& conn, int tag )
{

    (void)conn;
    (void)tag;

    // very simple implementation. In order to keep the I/O worker thread (from
    // which this handler will be called) moving forward, we'll use a separate
    // thread to monitor the launched LU process and wait for a result. Locking
    // to prevent simultaneous updates is included in the threadproc.

    if (m_LUPermitted)
    {
      // Log *before* starting the actual request.
        LogLiveUpdateRequest( conn.GetConnectedUser(), true );
        MyBeginThread( (THREAD)StartLiveUpdateDownload, NULL, "Tray-initiated LiveUpdate Monitor" );
    }
    else
    {
        LogLiveUpdateRequest( conn.GetConnectedUser(), false );
    }

    // no response to client

}

void TrayIPCManager::DefsCheckThread( void* context )
{
    // reconstitute our TrayIPCManager reference
    TrayIPCContext* ipcContext = reinterpret_cast<TrayIPCContext*>(context);
    dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: defs check thread starting\n" );

    DWORD cc = ERROR_SUCCESS;

    cc = CheckPattern( FALSE );

    ipcContext->m_mgr.OperationComplete( ipcContext, cc );

    dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: defs check thread finishing\n" );
}

/**
 * Handler method for requests for virus defs check messages
 *
 * @param conn   Reference to the connection originating the request
 * @param tag    Tag of the request message
 *
 * @note IPC lock must be acquired prior to calling
 */
void TrayIPCManager::HandleDefsCheckRequest( TrayIPCConnection& conn, int tag )
{
    // very simple implementation. In order to keep the I/O worker thread (from
    // which this handler will be called) moving forward, we'll use a separate
    // thread to monitor the launched LU process and wait for a result. Locking
    // to prevent simultaneous updates is included in the threadproc.

    TrayIPCContext* context = new(std::nothrow) TrayIPCContext( *this, &conn, tag, TRAYNOTIFY_OP_DEFS_CHECK_REQUEST );
    if ( context )
    {
        MyBeginThread( (THREAD)DefsCheckThread, reinterpret_cast<void*>(context), "Tray-initiated Defs check" );
    }
    else
    {
        // Send 'nak' to client
        HandleDefsCheckCompletion( &conn, -1, tag, ERROR_FUNCTION_FAILED );
    }
}

// TrayIPCManager::FillStatusPayload()
//
// Fills in a payload buffer (space permitting) with a status message
// 
// NOTE: IPC lock must be acquired prior to calling
//
// Arguments:
//   buf    - destination buffer for message payload
//   buflen - [in]  size of destination buffer
//            [out] number of bytes used on success, number of bytes needed if
//                  input buffer was too small, 0 on other error
//
// Returns:
//   true on success, false on failure
bool TrayIPCManager::FillStatusPayload( unsigned char* buf, size_t& buflen )
{
    // If the buffer is not large enough, then toss it out.
    if (NULL == buf || TRAYNOTIFY_MSGSIZE_STATUS_UPDATE > buflen)
    {
        buflen = TRAYNOTIFY_MSGSIZE_STATUS_UPDATE;
        return false;
    }

    unsigned char* p = buf;

    // Pack the opcode
    PACK_U32( p, TRAYNOTIFY_OP_STATUS_UPDATE );

    // Pack the status of AP
    PACK_U32( p, m_APEnabled ? 1 : 0 );

    // Pack the status of rtvscan (always running)
    PACK_U32( p, 1 );

    // Pack the status of LU
    PACK_U32( p, m_LUPermitted ? 1 : 0 );

    // Pack the status of the icon display
    PACK_U32( p, m_showIcon ? 1 : 0 );

    buflen = p - buf;

#ifdef DEBUG
    assert( buflen == TRAYNOTIFY_MSGSIZE_STATUS_UPDATE );
#endif

    return true;
}

// TrayIPCManager::FillVersionPayload()
//
// Fills in a payload buffer (space permitting) with a version message
// 
// NOTE: IPC lock must be acquired prior to calling
//
// Arguments:
//   buf    - destination buffer for message payload
//   buflen - [in]  size of destination buffer
//            [out] number of bytes used on success, number of bytes needed if
//                  input buffer was too small, 0 on other error
//
// Returns:
//   true on success, false on failure
bool TrayIPCManager::FillVersionPayload( unsigned char* buf, size_t& buflen )
{

    // base buffer length includes the following:
    // * 4-byte opcode
    // * 12-bytes for 3 x 4-byte string length fields
    // * 16-bytes for 4 x 4-byte ints for defs version values
    size_t buflenNeeded = 32;

    // Now add in the lengths of the strings to be included, including NULL
    buflenNeeded += m_productVersionStringLen;
    buflenNeeded += m_engineVersionStringLen;
    buflenNeeded += m_defsVersionStringLen;

    // If we're not given enough space, return right here
    if (NULL == buf || buflenNeeded > buflen)
    {
        buflen = buflenNeeded;
        return false;
    }

    unsigned char* p = buf;

    // Write opcode
    PACK_U32( p, TRAYNOTIFY_OP_VERSION_UPDATE );

    // Write product version string, prefixed by length
    PACK_STR( p, m_productVersionString, m_productVersionStringLen );

    // Write engine version string, prefixed by length
    PACK_STR( p, m_engineVersionString, m_engineVersionStringLen );

    // Write defs version string, prefixed by length
    PACK_STR( p, m_defsVersionString, m_defsVersionStringLen );

    PACK_U32( p, m_defsDay );
    PACK_U32( p, m_defsMonth );
    PACK_U32( p, m_defsYear );
    PACK_U32( p, m_defsRevision );

    buflen = p - buf;

#ifdef DEBUG
    assert( buflen == buflenNeeded );
#endif

    return true;
}

// TrayIPCManager::FillScanStatusPayload()
//
// Fills in a payload buffer (space permitting) with a scan status message
// 
// NOTE: IPC lock must be acquired prior to calling
//
// Arguments:
//   buf    - destination buffer for message payload
//   buflen - [in]  size of destination buffer
//            [out] number of bytes used on success, number of bytes needed if
//                  input buffer was too small, 0 on other error
//
// Returns:
//   true on success, false on failure
bool TrayIPCManager::FillScanStatusPayload( unsigned char* buf, size_t& buflen )
{
    // If the buffer is not large enough, then toss it out.
    if (NULL == buf || TRAYNOTIFY_MSGSIZE_SCAN_UPDATE > buflen)
    {
        buflen = TRAYNOTIFY_MSGSIZE_SCAN_UPDATE;
        return false;
    }

    unsigned char* p = buf;
    
    // Pack the opcode
    PACK_U32( p, TRAYNOTIFY_OP_SCAN_UPDATE );

    // Pack the scan status code
    PACK_U32( p, m_scanStatus );

    buflen = p - buf;

#ifdef DEBUG
    assert( buflen == TRAYNOTIFY_MSGSIZE_SCAN_UPDATE );
#endif

    return true;
}

bool TrayIPCManager::FillDefsCheckResponsePayload( unsigned char* buf, size_t& buflen, int rc )
{
    int worked = false;
    // If the buffer is not large enough, then toss it out.
    if (NULL == buf || TRAYNOTIFY_MSGSIZE_DEFS_CHECK_RESPONSE > buflen)
    {
        buflen = TRAYNOTIFY_MSGSIZE_DEFS_CHECK_RESPONSE;
    }
    else
    {
        unsigned char* p = buf;

        // Pack the opcode
        PACK_U32( p, TRAYNOTIFY_OP_DEFS_CHECK_REQUEST );

        // Pack the result code
        PACK_U32( p, rc );

        buflen = p - buf;

#ifdef DEBUG
        assert( buflen == TRAYNOTIFY_MSGSIZE_DEFS_CHECK_RESPONSE );
#endif
        worked = true;
    }
    return worked;
}

// TrayIPCManager::BroadcastStatus()
// 
// Uses TrayIPCManager internal state to produce a status broadcast message
// and send it to all connected clients
// 
// NOTE: IPC lock must be acquired prior to calling
void TrayIPCManager::BroadcastStatus()
{
    unsigned char buf[TRAYNOTIFY_MSGSIZE_STATUS_UPDATE];
    size_t buflen = TRAYNOTIFY_MSGSIZE_STATUS_UPDATE;

    // Fill in the payload for our message
    if (false == FillStatusPayload( buf, buflen ))
    {
#ifdef DEBUG
        // If there was an error, it shouldn't be because we didn't provide
        // enough room for the payload
        assert( TRAYNOTIFY_MSGSIZE_STATUS_UPDATE >= buflen );
#endif
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: (bcast) status payload required an unexpected size: %d\n", buflen );
        return;
    }

    // Now send this notification to each connection
    for (size_t i = 0; i < m_connections; ++i)
    {
        if (false == m_connTable[i]->SendDataNoLock( TrayIPCConnection::STATUS_SVC, 0, buflen, buf ))
        {
            dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: failed to send status broadcast to client %d\n", i );
        }
    }
}

// TrayIPCManager::BroadcastVersion()
//
// Uses TrayIPCManager internal state to produce a version broadcast message
// and send it to all connected clients
// 
// NOTE: IPC lock must be acquired prior to calling
void TrayIPCManager::BroadcastVersion()
{
    size_t buflen = 0;

    // How much room do we need to accommodate the version information?
    FillVersionPayload( NULL, buflen );

    if (0 >= buflen)
    {
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: (bcast) Unexpected error getting version payload size: returned %d\n", buflen );
        return;
    }

    // Attempt to allocate the space we need
    unsigned char* buf = new(std::nothrow) unsigned char[buflen];
    if (NULL == buf)
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: Failed to allocate payload buffer for version broadcast\n" );
        return;
    }

    // Fill in the payload for our message
    if (true == FillVersionPayload( buf, buflen ))
    {
        // Now send this notification to each connection
        for (size_t i = 0; i < m_connections; ++i)
        {
            if (false == m_connTable[i]->SendDataNoLock( TrayIPCConnection::STATUS_SVC, 0, buflen, buf ))
            {
                dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: failed to send version broadcast to client %d\n", i );
            }
        }
    }
    else
    {
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: (bcast) Unexpected failure to fill version payload: returned size %d\n", buflen );
    }

    delete[] buf;
}

// TrayIPCManager::BroadcastScanStatus()
//
// Uses TrayIPCManager internal state to produce a scan status broadcast message
// and send it to all connected clients
// 
// NOTE: IPC lock must be acquired prior to calling
void TrayIPCManager::BroadcastScanStatus()
{
    unsigned char buf[TRAYNOTIFY_MSGSIZE_SCAN_UPDATE];
    size_t buflen = TRAYNOTIFY_MSGSIZE_SCAN_UPDATE;

    // Fill in the payload for our message
    if (false == FillScanStatusPayload( buf, buflen ))
    {
#ifdef DEBUG
        // If there was an error, it shouldn't be because we didn't provide
        // enough room for the payload
        assert( TRAYNOTIFY_MSGSIZE_SCAN_UPDATE >= buflen );
#endif
        dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: (bcast) Scan status payload required an unexpected size: %d\n", buflen );
        return;
    }

    // Now send this notification to each connection
    for (size_t i = 0; i < m_connections; ++i)
    {
        if (false == m_connTable[i]->SendDataNoLock( TrayIPCConnection::STATUS_SVC, 0, buflen, buf ))
        {
            dprintfTag1( DEBUGCOMMPACKET, "TrayIPC: failed to send scan status broadcast to client %d\n", i );
        }
    }
}

// static TrayIPCManager::SocketAcceptThread
//
// Thread procedure for the worker thread that loops on the main listening
// socket, accepts incoming connections, and creates the associated connection
// objects and adds them to the connection table.
//
// The context parameter is a pointer to the TrayIPCManager object associated
// with the thread
void TrayIPCManager::SocketAcceptThread( void* context )
{
    // reconstitute our TrayIPCManager reference
    TrayIPCManager* ipcMgr = reinterpret_cast<TrayIPCManager*>(context);

    // Sanity check arguments and bail on error
    if (NULL == ipcMgr)
    {
        return;
    }

    pthread_mutex_lock(&(ipcMgr->m_workerMutex));
    ++ipcMgr->m_workerCount;
    pthread_mutex_unlock(&(ipcMgr->m_workerMutex));

    // Obtain the main socket fd from the IPC manager. If it is invalid,
    // break out of the main loop.
    int mainSocket = ipcMgr->m_mainSocket;
    if (-1 == mainSocket)
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: Invalid main socket provided\n" );
        goto threadEnd;
    }

    // We don't care about address data we are getting from accept, but we still
    // need a place to hold it.
    socklen_t dummy;

    // fd set for doing the select, and wait time structure for the same
    fd_set sockets;
    timeval waitTime;

    // Main thread loop. We select on the main socket and accept new connections
    // as they arrive. When the main socket closes, we exit.
    for (;;)
    {
        // Obtain the state of the manager to determine if we're shutting down
        LOCK();
        State mgrState = ipcMgr->m_state;
        UNLOCK();

        // If we're shutting down, break out of the loop
        if (MANAGER_SHUTTING_DOWN == mgrState)
        {
            break;
        }

        // Clear out the fd_set and add in the main socket alone.
        FD_ZERO( &sockets );
        FD_SET( mainSocket, &sockets );

        // Set up a timeout structure for our select
        waitTime.tv_sec = ACCEPT_THREAD_SELECT_TIMEOUT_SECS;
        waitTime.tv_usec = 0;

        // Select on the main socket
        int res = select( mainSocket + 1, &sockets, NULL, NULL, &waitTime );
        if (-1 == res)
        {
            dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: error in select: %d (%s)\n", errno, strerror( errno ) );
            break;
        }
        else if (0 == res)
        {
            // No new connections, so just go back into the loop
        }
        else
        {
            // If there was a waiting connection on the socket, accept it and
            // add a new connection to the connection table.
            int newSocket = accept( mainSocket, NULL, &dummy );
            if (-1 == newSocket)
            {
                dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: socket accept failed\n" );
            }
            else if (-1 == ipcMgr->AddConnection( newSocket ))
            {
                dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: add connection failed\n" );
                close( newSocket );
            }
        }
    }

 threadEnd:
    // This thread is responsible for closing the main socket when it exits
    ipcMgr->DestroyMainSocket();

    // Always upon exit, we must check in with the IPC manager so it knows the
    // thread is gone.
    ipcMgr->WorkerDeath();
}

// static TrayIPCManager::IPCWorkerThread
//
// Thread procedure for the worker thread that handles doing the I/O over the
// open connections. Selects over all connected sockets, then performs work on
// all ready connections.
//
// The context parameter is a pointer to the TrayIPCManager object associated
// with the thread
void TrayIPCManager::IPCWorkerThread( void* context )
{
    // Reconstitute our TrayIPCManager reference
    TrayIPCManager* ipcMgr = reinterpret_cast<TrayIPCManager*>(context);

    // Sanity check arguments and bail on error
    if (NULL == ipcMgr)
    {
        return;
    }

    pthread_mutex_lock(&(ipcMgr->m_workerMutex));
    ++ipcMgr->m_workerCount;
    pthread_mutex_unlock(&(ipcMgr->m_workerMutex));

    fd_set readSockets;
    fd_set writeSockets;
    timeval waitTime;

    // Main thread loop. We select on all open sockets, do work for any that are
    // ready, cleanup for those that close, and quit when the main socket is
    // closed.
    for (;;)
    {
        // Obtain the state of the manager to determine if we're shutting down
        LOCK();
        State mgrState = ipcMgr->m_state;
        UNLOCK();

        // If we're shutting down, break out of the loop
        if (MANAGER_SHUTTING_DOWN == mgrState)
        {
            break;
        }

        // Clear our fd_sets and our maximum socket value
        FD_ZERO( &readSockets );
        FD_ZERO( &writeSockets );
        int maxSocket = 0;

        // Loop through all open connections and add them to the appropriate
        // fd sets. All open connections get added to the read set, but only
        // those connections marked as having data to send get added to the
        // write set.
        LOCK();
        for (size_t i = 0; i < ipcMgr->m_connections; ++i)
        {
            // Get the socket descriptor for this connection
            int sock = ipcMgr->m_connTable[i]->GetSocket();

            // Add it to the read set
            FD_SET( sock, &readSockets );

            // If the connection has data to send, add it to the write set
            if (ipcMgr->m_connTable[i]->HasPendingSend())
            {
                FD_SET( sock, &writeSockets );
            }

            // If this socket is larger that the biggest one we've seen, save
            // its value off here.
            if (sock > maxSocket)
            {
                maxSocket = sock;
            }
        }

        UNLOCK();

        // Set up wait time for select operation
        waitTime.tv_sec = WORK_THREAD_SELECT_TIMEOUT_SECS;
        waitTime.tv_usec = 0;

        // the parameter to select must be one greater than the maximum socket
        ++maxSocket;

        // Perform the select operation
        int retval = select( maxSocket, &readSockets, &writeSockets, NULL, &waitTime );
        if (-1 == retval)
        {
            dprintfTag2( DEBUGCOMMPACKET, "TrayIPC: select failed: %d (%s)\n", errno , strerror( errno ) );
        }
        else if (0 == retval)
        {
            // No sockets ready, so just re-enter the loop
        }
        else
        {
            // Some sockets are ready for work!

            LOCK();
            
            // Loop through the connection table and check for ready sockets.
            for (size_t i = 0; i < ipcMgr->m_connections; ++i)
            {
                // Is this socket ready to be read from?
                if (FD_ISSET( ipcMgr->m_connTable[i]->GetSocket(), &readSockets ))
                {
                    // Then perform the read
                    if (false == ipcMgr->m_connTable[i]->PerformRead())
                    {
                        // PerformRead returns false to indicate the connection
                        // should be closed, so delete and continue. NULL the
                        // entry out now, and later on we'll compress the table
                        delete ipcMgr->m_connTable[i];
                        ipcMgr->m_connTable[i] = NULL;
                        continue;
                    }
                }

                // Is the socket ready for a write?
                if (FD_ISSET( ipcMgr->m_connTable[i]->GetSocket(), &writeSockets ))
                {
                    // Then perform the write
                    ipcMgr->m_connTable[i]->PerformWrite();
                }

                // Is the socket waiting to be closed?
                if (ipcMgr->m_connTable[i]->IsAwaitingClose())
                {
                    delete ipcMgr->m_connTable[i];
                    ipcMgr->m_connTable[i] = NULL;
                }
            }
            
            // Purge any NULLS in the set. If a connection was closed, it is
            // now in the table as a NULL. We need to squeeze all of the open
            // connections together into a contiguous block for easy iteration
            for (size_t i = 0; i < ipcMgr->m_connections;)
            {
                // Found a NULL
                if (ipcMgr->m_connTable[i] == NULL)
                {
                    if (i != ipcMgr->m_connections - 1)
                    {
                        // If the NULL is not the last connection, move the
                        // last connection in
                        ipcMgr->m_connTable[i] = ipcMgr->m_connTable[ipcMgr->m_connections - 1];
                    
                        // NULL out the previous last connection
                        ipcMgr->m_connTable[ipcMgr->m_connections - 1] = NULL;
                    }

                    // Now decrement the number of connections.
                    --ipcMgr->m_connections;

                    // Now re-run the loop with the same index value being
                    // checked to make sure we didn't just swap a NULL for
                    // a NULL. If the last connection in the table turned out
                    // to be the NULL, the loop will terminate because
                    // m_connections has decreased.
                }
                else
                {
                    // No NULL, so advance our index
                    ++i;
                }
            }
            UNLOCK();
        }
    }

    // This thread clears out the connection table on exit
    LOCK();
    ipcMgr->DestroyConnectionTable();
    UNLOCK();

    // Always upon exit, we must check in with the IPC manager so it knows the
    // thread is gone.
    ipcMgr->WorkerDeath();
}

// TrayIPCManager::AddConnection
//
// Function for adding a connection to the connection table safely. Acquires the
// lock, creates the connection object and initializes it, then fits the
// connection into the connection table.
//
// Arguments:
//  socket - the connected socket to create the connection object around
//
// Returns:
//  true on success, false on failure
bool TrayIPCManager::AddConnection( int socket )
{
    int retval = true;

    LOCK();

    // Sanity check that the IPCManager is initialized
    if (MANAGER_SHUTTING_DOWN == m_state)
    {
        retval = false;
    }
    // Make sure we have room for the connection
    else if (m_connections >= MAX_CONNECTIONS)
    {
        retval = false;
    }
    else
    {
        // Construct the connection object
        TrayIPCConnection* conn = new(std::nothrow) TrayIPCConnection( socket, *this );
        if (NULL == conn)
        {
            dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: Failed to create TrayIPCConnection\n" );
            retval = false;
        }
        else
        {
            // Initialize the connection object
            if (false == conn->Initialize( m_getpwBuf, m_getpwBufLen ))
            {
                dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: Failed to initialize TrayIPCConnection\n" );
                delete conn;
                retval = false;
            }
            else
            {
                m_connTable[m_connections++] = conn;
            }
        }
    }

    UNLOCK();

    return retval;
}

// TrayIPCManager::WorkerDeath
//
// Utility function must be called once at the end of each thread's lifetime.
// Peforms cleanup and notifies the main IPCManager object, which right now
// consists of decrementing the worker thread count and signaling the
// associated condition variable.
void TrayIPCManager::WorkerDeath()
{
    pthread_mutex_lock(&m_workerMutex);
    --m_workerCount;
    if(m_workerCount <= 0)
        pthread_cond_signal(&m_workerCond);
    pthread_mutex_unlock(&m_workerMutex);
}

// TrayIPCManager::DestroyMainSocket
//
// Cleanup function that closes down the main socket used by the IPC Manager
void TrayIPCManager::DestroyMainSocket()
{
    if (-1 != m_mainSocket)
    {
        dprintfTag0( DEBUGCOMMPACKET, "TrayIPC: Main socket closing\n" );
        close( m_mainSocket );
        if (!isSocketAddressAbstract( SOCKNAME_TRAY ))
        {
            unlink( SOCKNAME_TRAY );
        }
        m_mainSocket = -1;
    }
}

// TrayIPCManager::DestroyConnectionTable
//
// Cleanup function that closes down all open connections
void TrayIPCManager::DestroyConnectionTable()
{
    m_connections = 0;
    for (size_t i = 0; i < MAX_CONNECTIONS; ++i)
    {
        delete m_connTable[i];
        m_connTable[i] = NULL;
    }
}

