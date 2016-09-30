// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.

// TrayIPC.h

#ifndef TRAYIPC_H_INCLUDED
#define TRAYIPC_H_INCLUDED

#include <pthread.h>

// Forward declaration of connection class
class TrayIPCConnection;

// Forward declaration of context class
class TrayIPCContext;

/**
 * Class for managing IPC with savTray(s)
 * This class is responsible for accepting and servicing client IPC
 * connections. It also synchronizes access from mutiple threads to
 * coordinate updates and broadcasts.
 */
class TrayIPCManager
{
public:
    /** Constructor */
    TrayIPCManager();

    /** Destructor */
    virtual ~TrayIPCManager();

private:
    // Forbid default copying and assignment
    TrayIPCManager( const TrayIPCManager& );
    TrayIPCManager& operator=( const TrayIPCManager& );

public:
    // Initialization and destruction methods.

    /**
     * Prepares an instance for use.
     * An instance of TrayIPCManager must be initialized with Initialize
     * before use.
     * @return true on success, false on error.
     */
    bool Initialize();

    /**
     * Cleans up instance.
     * An instance of TrayIPCManager may optionally be explicitly
     * deinitialized after use with Destroy, or simply deinitialized
     * through its destructor.
     */
    void Destroy();

public:
    // Update methods to be called when certain conditions change that tray IPC
    // clients care about.

    /**
     * Updates AP status
     *
     * @param APEnabled true if AP is enabled, false when disabled
     */
    void UpdateAPStatus( bool APEnabled );

    /**
     * Updates LiveUpdate Permission
     *
     * @param LUPermitted true if LU is permitted, false when locked
     */
    void UpdateLUPermission( bool LUPermitted );

    /**
     * Updates Desktop icon visibility
     *
     * @param showIcon true if the tray icon is to be shown, false otherwise.
     */
    void UpdateShowDesktopIcon( bool showIcon );

    /**
     * Updates product version
     *
     * @param productVersion Version of product, packed into DWORD
     */
    void UpdateProductVersion( DWORD productVersion );

    /**
     * Updates engine version
     *
     * @param engineVersion Version of engines, packed into DWORD
     */
    void UpdateEngineVersion( DWORD engineVersion );

    /**
     * Updates virus definitions version
     *
     * @param defsVersion Version of virus definitions, packed into DWORD
     */
    void UpdateDefsVersion( DWORD defsVersion );

    /**
     * Updates the status of the scanner
     *
     * @param scanStatus encoded value of the scanner's status
     */
    void UpdateScanStatus( DWORD scanStatus );

    /**
     * Sends a scan notification to any clients that are qualified to receive
     * it (based on the uid of the client end of the connection). Unlike the
     * above methods, which represent updates in status and may not generate
     * messages to clients, this method explicitly sends notifications, and
     * does not update internal state of the IPC manager.
     *
     * @param dateSeconds   scan date in seconds since the Epoch
     * @param scanType      type of scan (manual, AP, ...)
     * @param eventType     type of event (infection found)
     * @param threatName    name of detected threat
     * @param fileName      name of scanned file
     * @param location      current location of scanned file
     * @param computer      name of computer
     * @param user          name of user initiating scan
     * @param actionTaken   actions taken on scan
     * @param actionDesc    disposition of actions taken
     * @param format        format string for notification display
     *                      (aka Custom Message)
     */
    void SendScanNotification( DWORD dateSeconds,       // time/date of event
                               const char* scanType,    // type of scan (manual, AP, ...)
                               const char* eventType,   // type of event (infection found)
                               const char* threatName,  // name of detected threat
                               const char* fileName,    // name of scanned file
                               const char* location,    // current location of scanned file
                               const char* computer,    // name of computer
                               const char* user,        // name of user initiating scan
                               const char* actionTaken, // actions taken on scan
                               const char* actionDesc,  // disposition of actions taken
                               const char* format       // format string
                               );

    /**
     * Callback routine for external functions to return results.
     *
     * @param context Callback context initiall created by a TrayIPCManager instance.
     * @param percent Value to signal for progress
     */
    void OperationUpdate( TrayIPCContext* context, double percent );

    /**
     * Callback routine for external functions to return results.
     *
     * @param context Callback context initiall created by a TrayIPCManager instance.
     * @param rc      Result of external operation.
     */
    void OperationComplete( TrayIPCContext* context, DWORD rc );

public:
    // Handlers for internal processing of operation completions.
    void HandleDefsCheckCompletion( TrayIPCConnection* conn, int index, int tag, DWORD rc );
    void HandleMDefsUpdateCompletion( TrayIPCConnection* conn, int index, int tag, DWORD rc );

    // Reply methods made available to connection objects for handling requests
    // for certain message types.
    
    /**
     * Handler method for requests for status messages
     *
     * @param conn   Reference to the connection originating the request
     * @param tag    Tag of the request message
     * @param buf    Buffer containing remainder of the request message
     * @param buflen Size in bytes of the buffer pointed to by buf
     */
    void HandleStatusRequest( TrayIPCConnection& conn, int tag, const unsigned char* buf, size_t buflen );

    /**
     * Handler method for requests for version messages
     *
     * @param conn   Reference to the connection originating the request
     * @param tag    Tag of the request message
     */
    void HandleVersionRequest( TrayIPCConnection& conn, int tag );

    /**
     * Handler method for requests for scan status messages
     *
     * @param conn   Reference to the connection originating the request
     * @param tag    Tag of the request message
     */
    void HandleScanStatusRequest( TrayIPCConnection& conn, int tag );

    /**
     * Handler method for requests for live update messages
     *
     * @param conn   Reference to the connection originating the request
     * @param tag    Tag of the request message
     */
    void HandleLiveUpdateRequest( TrayIPCConnection& conn, int tag );

    /**
     * Handler method for requests for virus defs check messages
     *
     * @param conn   Reference to the connection originating the request
     * @param tag    Tag of the request message
     *
     * @note IPC lock must be acquired prior to calling
     */
    void HandleDefsCheckRequest( TrayIPCConnection& conn, int tag );

    /**
     * Handler method for requests for MicroDef Updates
     *
     * @param conn   Reference to the connection originating the request
     * @param tag    Tag of the request message
     * @param buf    Buffer containing remainder of the request message
     * @param buflen Size in bytes of the buffer pointed to by buf
     */
    void HandleMDefsUpdateRequest( TrayIPCConnection& conn, int tag, const unsigned char* buf, size_t buflen );    

private:
    // Methods used to fill in the payloads for various message types sent out
    // by this module. Each one takes a buffer and its size in bytes as input.
    // Return value will be true on success and false on failure. On success,
    // buflen outparam is set to the number of bytes used. On failure, buflen
    // outparam is set to the number of bytes needed if the buffer was too
    // small, or zero if the method failed for some other reason.
    bool FillStatusPayload( unsigned char* buf, size_t& buflen );
    bool FillVersionPayload( unsigned char* buf, size_t& buflen );
    bool FillScanStatusPayload( unsigned char* buf, size_t& buflen );
    bool FillDefsCheckResponsePayload( unsigned char* buf, size_t& buflen, int rc );
    bool FillMDefsUpdateResponsePayload( unsigned char* buf, size_t& buflen, int rc );    

private:
    // Methods used to effect the broadcast of update messages with common data
    // backed by the manager object. These methods handle payload buffers and
    // perform the actual sends to connected clients.
    void BroadcastStatus();
    void BroadcastVersion();
    void BroadcastScanStatus();

private:
    // Thread procedures for two worker threads used for tray IPC: one thread
    // for accepting new connections on the main listening thread, and one for
    // doing reading and writing that needs to be done for connected clients
    static void SocketAcceptThread( void* context );
    static void IPCWorkerThread( void* context );

    /** Thread procedure to handle defs update checks. */
    static void DefsCheckThread( void* context );

    /** Thread procedure to handle MicroDdef updates. */
    static void MDefsUpdateThread( void* context );

private:
    // Private utility functions
    bool AddConnection( int socket );
    void WorkerDeath();

    // Cleanup methods
    void DestroyMainSocket();
    void DestroyConnectionTable();

public:
    // maximum allowable number of simultaneous connections
    static const size_t MAX_CONNECTIONS = 64;

private:
    // private constants

    // Number of seconds the socket accept thread should wait per iteration
    static const long ACCEPT_THREAD_SELECT_TIMEOUT_SECS = 1;

    // Number of seconds the IPC worker thread should wait per iteration
    static const long WORK_THREAD_SELECT_TIMEOUT_SECS = 1;

    // Number of milliseconds to wait on teardown for each worker thread
    static const unsigned long TEARDOWN_THREAD_TIMEOUT_MSECS = 10000;

    // Size of buffers for holding version strings
    static const size_t VERSION_STRING_BUF_SIZE = 128;

private:
    // State of the Tray IPC Manager
    enum State
    {
        MANAGER_UNINITIALIZED,
        MANAGER_INITIALIZED,
        MANAGER_SHUTTING_DOWN
    };
    State m_state;

    // Connection table: array of connection objects and number of valid
    // connections
    TrayIPCConnection*  m_connTable[MAX_CONNECTIONS];
    size_t              m_connections;

    // Main listening socket: its fd
    int                 m_mainSocket;

    // For waiting for worker threads to die
    pthread_mutex_t     m_workerMutex;
    pthread_cond_t      m_workerCond;
    int                 m_workerCount;

    // Work buffer used for getpwuid_r calls in connection class
    char*               m_getpwBuf;
    size_t              m_getpwBufLen;

    ////////////////////////////////////////////////////////////////////////////
    // Data stored in the manager for fulfulling requests from clients
    ////////////////////////////////////////////////////////////////////////////

    // Is AP enabled?
    bool m_APEnabled;

    // Is LU Permitted?
    bool m_LUPermitted;

    // Is the icon to be shown on the desktop?
    bool m_showIcon;

    // Product version packed into a DWORD, also string form and length
    // including terminating NULL
    DWORD  m_productVersion;
    char   m_productVersionString[VERSION_STRING_BUF_SIZE];
    size_t m_productVersionStringLen;

    // Engine version packed into a DWORD, also string form and length
    // including terminating NULL
    DWORD  m_engineVersion;
    char   m_engineVersionString[VERSION_STRING_BUF_SIZE];
    size_t m_engineVersionStringLen;

    // Definitions version packed into a DWORD, also string form and length
    // including terminating NULL
    WORD   m_defsYear;
    WORD   m_defsMonth;
    WORD   m_defsDay;
    DWORD  m_defsRevision;
    char   m_defsVersionString[VERSION_STRING_BUF_SIZE];
    size_t m_defsVersionStringLen;

    // Scan Status
    DWORD  m_scanStatus;
};



#endif // TRAYIPC_H_INCLUDED
