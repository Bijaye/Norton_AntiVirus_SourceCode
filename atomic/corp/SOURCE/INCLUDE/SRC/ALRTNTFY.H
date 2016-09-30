// Copyright 1996 Symantec Corporation
//*****************************************************************************
//
//  $Header:   S:/INCLUDE/VCS/ALRTNTFY.H_v   1.0   24 Apr 1997 11:22:48   JBRENNA  $
//
//  Description:
//
//*****************************************************************************
//  $Log:   S:/INCLUDE/VCS/ALRTNTFY.H_v  $
// 
//    Rev 1.0   24 Apr 1997 11:22:48   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:44   RFULLER
// Initial revision
// 
//    Rev 1.4   13 Feb 1997 20:31:30   TSmith
// Fixed PACKET_SOURCE defines so they're flags instead of numbers.
// 
//    Rev 1.3   21 Jan 1997 13:31:54   TSmith
// Added 'DWORD PacketSource' to packet struct to store flags regarding
// origin of packet. Added declarations for member functions to get and
// set this data. Also defined several flags that can be or'd in a few
// combinations and stored in this struct member.
// 
// 
//    Rev 1.2   22 Dec 1996 15:34:14   TSmith
// Reworked to provide support for both Unicode and Ansi. Broke all string
// related functions down into '...W' and '...A' versions.
//
//    Rev 1.1   04 Dec 1996 18:52:42   DSACKIN
// Changed all TCHAR references to WCHAR for use with non-UNICODE apps.
//
//    Rev 1.0   08 Nov 1996 11:36:16   DKEEFER
// Initial revision
//
//    Rev 1.4   03 Nov 1996 08:52:52   TSmith
// Added member functions to get/set 'ActionSuccess'.
//
//    Rev 1.3   03 Nov 1996 08:36:40   TSmith
// Added 'BOOL ActionSuccess' to packet structure, made appropriate member
// functions 'CONST', cleaned up some file formatting.
//
//    Rev 1.2   02 Nov 1996 14:39:28   TSmith
// Added 'ACTION_xxxxx' flag definitions.
//
//    Rev 1.1   01 Nov 1996 16:43:46   TSmith
// According to MS documentation, the 'UuidCreate' function is not available
// on Windows 95. Odd since the UUIDGEN utility works fine on that platform.
// Be aware if using this module for Win95.
//
//    Rev 1.0   31 Oct 1996 19:13:18   TSmith
// Initial revision.
//*****************************************************************************
#if !defined( __ALRTNTFY_H )
#define __ALRTNTFY_H

#include <rpc.h>                        // For UUID definition

///////////////////////////////////////////////////////////////////////////////
// Packet type definitions.
//
// Only one defined now but different types could be designed for subsequent
// versions to enable different kinds of packets to be sent to the alert
// service. This could facilitate testing, diagnostics, configuration, etc.

#define PACKET_TYPE_NOTIFY 0x00000001


///////////////////////////////////////////////////////////////////////////////
// NAV_NOTIFICATION_PACKET version definitions.
//
// Increment these if significant modification is made to these structures.
// This will provide for the possibility of backward compatibility in
// subsequent versions.

#define PACKET_TYPE_NOTIFY_VERSION  0x0001
#define PACKET_TYPE_NOTIFY_REVISION 0x0000


///////////////////////////////////////////////////////////////////////////////
// NAV_NOTIFICATION_PACKET struct definition.
//
// This structure represents the block of data sent to the NAV Alert Service
// via RPC. The structure is of variable length, can accomodate variable
// length strings which are currently stored only as Unicode. Functionality
// to create and manipulate all struct member variables is provided by the
// 'CAlertNotification' class.
//
// This structure should be treated as private data of the 'CAlertNotification'
// class. A significant amount of pointer arithmetic is used to provide support
// for dynamic length strings. Using the 'CAlertNotification' class only to
// manipulate objects with this structure will keep modifications to the
// structure manageable and aid debugging.
//
// Once the first product has been shipped with this structure any change to it
// will need to be indicated by incrementing one or both of the version constants
// above. The later products will need to perfrom a version check at run time if
// they need to handle older packets.
//

#pragma pack( push, 1 )                 // Ensure structure packing is set to 1

typedef struct _NAV_NOTIFICATION_PACKET
    {
    DWORD       TotalByteLength;        // Size of entire alert packet. This
                                        // must be the first struct member
                                        // to facilitate RPC transmission of
                                        // this structure.

    WORD        Version;                // Major version of these structures.
    WORD        Revision;               // Minor version of these structures.
    
    DWORD       PacketType;             // Unique identifier of this "kind"
                                        // of packet.

    DWORD       PacketSource;           // Flags indicating originating location
                                        // and application of packet.

    UUID        UniqueID;               // Identifier for this packet instance.
    SYSTEMTIME  SystemTime;             // Time packet constructed/sent.
    DWORD       AlertType;              // Type of event.
    DWORD       Action;                 // Action taken on event.
    BOOL        ActionSuccess;          // Success or failure of action.
    BOOL        UnicodeStrings;         // Are strings in Unicode?
    UINT        CodePage;               // Code page for ANSI strings.

    DWORD       SystemNameOffset;       // Offset from start of this structure to
                                        // name of system sending packet.

    DWORD       UserNameOffset;         // Offset from start of this structure to
                                        // user name on system sending packet.

    DWORD       ObjectNameOffset;       // Offset from start of this structure to
                                        // object on system sending packet.

    DWORD       VirusNameOffset;        // Offset from start of this structure to
                                        // name of virus on system sending packet.

    DWORD       DomainNameOffset;       // Offset from start of this structure to
                                        // domain name of system sending packet
    } NAV_NOTIFICATION_PACKET, *PNAV_NOTIFICATION_PACKET;


///////////////////////////////////////////////////////////////////////////////
// NAV_ALERT_PACKET_STRING struct definition.
//
// Variable length structure used to store strings for the NAV_NOTIFICATION_PACKET
// struct. When a string is added to the packet, it is in the form of this
// structure.

typedef struct _NAV_ALERT_PACKET_STRING
    {
    DWORD       ByteLength;             // Size of this structure
    WCHAR       String[ 1 ];            // NULL terminated string
    } NAV_ALERT_PACKET_STRING, *PNAV_ALERT_PACKET_STRING;


#pragma pack( pop )                     // Return packing to previous value


///////////////////////////////////////////////////////////////////////////////
// Map function names to Unicode or ANSI versions of functions.

#if defined( UNICODE )
    #define InsertSystemName    InsertSystemNameW
    #define InsertUserName      InsertUserNameW
    #define InsertObjectName    InsertObjectNameW
    #define InsertVirusName     InsertVirusNameW
    #define InsertDomainName    InsertDomainNameW
    #define GetSystemName       GetSystemNameW
    #define GetUserName         GetUserNameW
    #define GetObjectName       GetObjectNameW
    #define GetVirusName        GetVirusNameW
    #define GetDomainName       GetDomainNameW
#else
    #define InsertSystemName    InsertSystemNameA
    #define InsertUserName      InsertUserNameA
    #define InsertObjectName    InsertObjectNameA
    #define InsertVirusName     InsertVirusNameA
    #define InsertDomainName    InsertDomainNameA
    #define GetSystemName       GetSystemNameA
    #define GetUserName         GetUserNameA
    #define GetObjectName       GetObjectNameA
    #define GetVirusName        GetVirusNameA
    #define GetDomainName       GetDomainNameA
#endif


///////////////////////////////////////////////////////////////////////////////
// 'AlertType' definitions

#define FILE_KNOWN_VIRUS          0x00000001
#define FILE_UNKNOWN_VIRUS        0x00000002
#define BOOT_RECORD_KNOWN_VIRUS   0x00000003
#define BOOT_RECORD_UNKNOWN_VIRUS 0x00000004
#define MBR_KNOWN_VIRUS           0x00000005
#define MBR_UNKNOWN_VIRUS         0x00000006


///////////////////////////////////////////////////////////////////////////////
// 'Action' flags

#define ACTION_NONE               0x00000001
#define ACTION_MOVE               0x00000002
#define ACTION_RENAME             0x00000004
#define ACTION_REPAIR             0x00000008
#define ACTION_DELETE             0x00000010
#define ACTION_EXCLUDE            0x00000020
#define ACTION_SHUTDOWN           0x00000040
#define ACTION_INOCULATE          0x00000080
#define ACTION_DENY_ACCESS        0x00000100
#define ACTION_UNDEFINED          0x00000200

///////////////////////////////////////////////////////////////////////////////
// 'PacketSource' flags

// High word indicates originating location
#define PACKET_SOURCE_LOCAL       0x00010000 // Another process on local system
#define PACKET_SOURCE_FORWARD     0x00020000 // Forwarded from remote Alert Service
#define PACKET_SOURCE_REMOTE      0x00040000 // Process on remote system

// Low word indicates originating application
#define PACKET_SOURCE_NAVAP       0x00000001 // NAV Auto Protect
#define PACKET_SOURCE_NAVW        0x00000002 // NAV Scanner                
#define PACKET_SOURCE_SAM         0x00000004 // Symantec AV for Macintosh  
#define PACKET_SOURCE_NAVNLM      0x00000008 // NAV for Netware
#define PACKET_SOURCE_NAVFW       0x00000010 // Internet firewall
#define PACKET_SOURCE_NAVMAIL     0x00000020 // Email scanning service
#define PACKET_SOURCE_NAVCLSCAN   0x00000040 // NAV console mode scanner


///////////////////////////////////////////////////////////////////////////////
// Miscellaneous defines

#define DEF_BUF_SIZE sizeof( NAV_NOTIFICATION_PACKET )


///////////////////////////////////////////////////////////////////////////////
// CAlertNotification class
//
// This purpose of this class is to build, manipulate and read a
// NAV_NOTIFICATION_PACKET structure and should be used by any program that
// needs to send a packet to NAV.
//
// The class performs pointer arithmetic and handles memory management for
// inserting and reading dynamic length strings from the packet. The packet
// is "one way", i.e. the strings can be inserted but cannot be modified or
// replaced. The object must be destroyed and reinstantiated with new strings.
//
// An attempt has been made to keep the class as generic and portable as
// possible. There are no calls to Quake or MFC and very few to Win32. Almost
// all code is generic C/C++ and RTL.
//
// The class currently only supports storing the dynamic strings as Unicode
// though both Unicode and Ansi functions are provided. This allows the class
// to be built in or called from either Unicode or Ansi programs. The class
// will perform all necessary conversions to either accept or supply both
// Unicode and Ansi strings.
//


class CAlertNotification
    {
private:
    ///////////////////////////////////////////////////////////////////////////
    // Private types

    enum STR_TYPE
        {
        STR_SYSTEM_NAME,
        STR_USER_NAME,
        STR_OBJECT_NAME,
        STR_VIRUS_NAME,
        STR_DOMAIN_NAME
        };

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class management & manipulation functions

    CAlertNotification(                 // Default constructor
        IN DWORD BufferSize = DEF_BUF_SIZE
        );

    CAlertNotification(                 // Copy constructor
        IN CAlertNotification&
        );

    CAlertNotification& operator=(      // Assignment operator
        CAlertNotification&
        );

    ~CAlertNotification( );             // Default destructor


    ///////////////////////////////////////////////////////////////////////////
    // Member variable access functions

    PBYTE GetBuffer( );                 // Pointer to data block

    BOOL GrowBuffer( DWORD NewSize );   // Expand data block

    DWORD GetSize( ) CONST;             // Current size of data block

    BOOL AssignExternalBuffer( PBYTE pBuffer );
    VOID ReleaseExternalBuffer( );

    VOID SetUniqueID( IN CONST UUID& UniqueID );
    VOID GetUniqueID( OUT UUID& UniqueID ) CONST;

    VOID SetTime( IN CONST SYSTEMTIME& Time );
    VOID GetTime( OUT SYSTEMTIME& TIme ) CONST;

    ///////////////////////////////////////////////////////////////////////////
    // String insertion & retrieval functions ( Unicode and Ansi versions )

    BOOL InsertSystemNameW( IN LPCWSTR SystemName );
    BOOL InsertSystemNameA( IN LPCSTR  SystemName );

    BOOL InsertUserNameW( IN LPCWSTR UserName );
    BOOL InsertUserNameA( IN LPCSTR  UserName );

    BOOL InsertObjectNameW( IN LPCWSTR ObjectName );
    BOOL InsertObjectNameA( IN LPCSTR  ObjectName );

    BOOL InsertVirusNameW( IN LPCWSTR VirusName );
    BOOL InsertVirusNameA( IN LPCSTR  VirusName );

    BOOL InsertDomainNameW( IN LPCWSTR DomainName );
    BOOL InsertDomainNameA( IN LPCSTR  DomainName );

    UINT GetSystemNameW(
        IN  UINT   BufferSize,          // Size of buffer
        OUT LPWSTR SystemName           // Pointer to buffer
        );

    UINT GetSystemNameA(
        IN  UINT  BufferSize,           // Size of buffer
        OUT LPSTR SystemName            // Pointer to buffer
        );

    UINT GetUserNameW(
        IN  UINT  BufferSize,           // Size of buffer
        OUT LPWSTR UserName             // Pointer to buffer
        );

    UINT GetUserNameA(
        IN  UINT BufferSize,            // Size of buffer
        OUT LPSTR UserName              // Pointer to buffer
        );

    UINT GetObjectNameW(
        IN  UINT  BufferSize,           // Size of buffer
        OUT LPWSTR ObjectName           // Pointer to buffer
        );

    UINT GetObjectNameA(
        IN  UINT BufferSize,            // Size of buffer
        OUT LPSTR ObjectName            // Pointer to buffer
        );

    UINT GetVirusNameW(
        IN  UINT  BufferSize,           // Size of buffer
        OUT LPWSTR VirusName            // Pointer to buffer
        );

    UINT GetVirusNameA(
        IN  UINT BufferSize,            // Size of buffer
        OUT LPSTR VirusName             // Pointer to buffer
        );

    UINT GetDomainNameW(
        IN  UINT  BufferSize,           // Size of buffer
        OUT LPWSTR DomainName           // Pointer to buffer
        );

    UINT GetDomainNameA(
        IN  UINT BufferSize,            // Size of buffer
        OUT LPSTR DomainName            // Pointer to buffer
        );


    ///////////////////////////////////////////////////////////////////////////
    // Inline functions

    UINT SystemNameSize( ) CONST {
        return( ( GetStringCharCount( STR_SYSTEM_NAME ) + 1 ) * sizeof( WCHAR ) );
        }

    UINT UserNameSize( ) CONST {
        return( ( GetStringCharCount( STR_USER_NAME ) + 1 ) * sizeof( WCHAR ) );
        }

    UINT ObjectNameSize( ) CONST {
        return( ( GetStringCharCount( STR_OBJECT_NAME ) + 1 ) * sizeof( WCHAR ) );
        }

    UINT VirusNameSize( ) CONST {
        return( ( GetStringCharCount( STR_VIRUS_NAME ) + 1 ) * sizeof( WCHAR ) );
        }

    UINT DomainNameSize( ) CONST {
        return( ( GetStringCharCount( STR_DOMAIN_NAME ) + 1 ) * sizeof( WCHAR ) );
        }

    DWORD GetPacketSource( ) CONST {
        return( m_pPacket->PacketSource );
        }

    VOID SetPacketSource( IN CONST DWORD Source ) {
        m_pPacket->PacketSource = Source;
        }

    VOID SetAlertType( IN CONST DWORD AlertType ) {
        m_pPacket->AlertType = AlertType;
        }

    DWORD GetAlertType( ) CONST {
        return( m_pPacket->AlertType );
        }

    VOID SetAction( IN CONST DWORD Action ) {
        m_pPacket->Action = Action;
        }

    DWORD GetAction( ) CONST {
        return( m_pPacket->Action );
        }

    VOID SetActionSuccess( IN CONST BOOL Status ) {
        m_pPacket->ActionSuccess = Status;
        }

    BOOL ActionSuccessful( ) CONST {
        return( m_pPacket->ActionSuccess );
        }

private:
    ///////////////////////////////////////////////////////////////////////////
    // Private data & functions

    PNAV_NOTIFICATION_PACKET m_pPacket; // Pointer to formatted buffer
    BOOL m_ExternalBuffer;              // TRUE if memory block provided and
                                        // managed by user of class.

    BOOL AppendVarLengthStringW(
        IN LPCWSTR  Source,             // Wide string to append
        IN STR_TYPE StrType             // Which type being added
        );

    BOOL AppendVarLengthStringA(
        IN LPCSTR   Source,             // String to append
        IN STR_TYPE StrType             // Which type being added
        );

    UINT RetrieveVarLengthStringW(
        IN  UINT     BufferSize,        // Size of buffer pointed to by param 3
        IN  STR_TYPE StrType,           // Which packet string to get
        OUT LPWSTR   Target             // Pointer to character buffer
        );

    UINT RetrieveVarLengthStringA(
        IN  UINT     BufferSize,        // Size of buffer pointed to by param 3
        IN  STR_TYPE StrType,           // Which packet string to get
        OUT LPSTR    Target             // Pointer to character buffer
        );

    DWORD GetStringBufferLengthW(
        IN LPCWSTR StringBuffer         // String to calculate byte length
        );

    DWORD GetStringCharCount(
        IN STR_TYPE StrType             // Which packet string to get value for
        ) CONST;

    DWORD GetStringOffset(
        IN STR_TYPE StrType             // Which packet string to get value for
        ) CONST;
    };

#endif  // _ALRTNTFY_H
