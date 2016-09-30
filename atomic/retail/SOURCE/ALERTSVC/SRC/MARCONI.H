/////////////////////////////////////////////////////////////////////
// MARCONI.H - Created - 05-20-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/marconi.h_v   1.0   21 May 1997 13:42:16   DSACKIN  $
//
// Description:
//    Here is where I'll put the definition of the old AlertNotification
//    structure for use in the Alert service.  I don't want to have
//    the class loaded up, but this struct will give me access to
//    all of the information that I need without the class.
//
//    For ease of use, and to make it a little more clear to the people
//    who look at this later, I'm changing the name of the struct
//    from NAV_NOTIFICATION_PACKET to MARCONI_PACKET.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/marconi.h_v  $
// 
//    Rev 1.0   21 May 1997 13:42:16   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__MARCONI_H)
#define __MARCONI_H

#pragma pack( push, 1 )                 // Ensure structure packing is set to 1

typedef struct tagMARCONI_PACKET
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
    } MARCONI_PACKET, *PMARCONI_PACKET;


///////////////////////////////////////////////////////////////////////////////
// NAV_ALERT_PACKET_STRING struct definition.
//
// Variable length structure used to store strings for the NAV_NOTIFICATION_PACKET
// struct. When a string is added to the packet, it is in the form of this
// structure.

typedef struct tagMARCONI_PACKET_STRING
    {
    DWORD       ByteLength;             // Size of this structure
    WCHAR       String[ 1 ];            // NULL terminated string
    } MARCONI_PACKET_STRING, *PMARCONI_PACKET_STRING;


#pragma pack( pop )                     // Return packing to previous value





#endif // if !defined(__MARCONI_H)
