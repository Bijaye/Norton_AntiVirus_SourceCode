/////////////////////////////////////////////////////////////////////
// CALERT.H - Created - 05-07-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/calert.h_v   1.4   21 Aug 1997 02:20:04   DSACKIN  $
//
// Description:
//    This is the base CAlert class declaration.  This code was
//    originally taken from the ALRTNTFY project.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/calert.h_v  $
// 
//    Rev 1.4   21 Aug 1997 02:20:04   DSACKIN
// Added functions to handle machine type.
//
//    Rev 1.3   18 May 1997 00:17:26   DSACKIN
// Added a string length function.
//
//    Rev 1.2   15 May 1997 14:09:48   DSACKIN
// Added Versioning information.
//
//    Rev 1.1   14 May 1997 17:06:52   DSACKIN
// Changed Packet functions from Buffer functions.
//
//    Rev 1.0   14 May 1997 13:39:12   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__CALERT_H)
#define __CALERT_H

#include <rpc.h>                        // For UUID definition

///////////////////////////////////////////////////////////////////////////////
// NAV_NOTIFICATION_PACKET version definitions.
//
// Increment these if significant modification is made to these structures.
// This will provide for the possibility of backward compatibility in
// subsequent versions.

#define PACKET_VERSION_MAJOR     0x0001
#define PACKET_VERSION_MINOR     0x0005


///////////////////////////////////////////////////////////////////////////////
// CALERT_NOTIFICATION_PACKET struct definition.
//
// This structure represents the block of data sent to the Alert Service
// via RPC. The structure is of variable length, can accomodate variable
// length strings which are currently stored only as Unicode. Functionality
// to create and manipulate all struct member variables is provided by the
// 'CAlert' class.
//
// This structure should be treated as private data of the 'CAlert'
// class. A significant amount of pointer arithmetic is used to provide support
// for dynamic length strings. Using the 'CAlert' class only to
// manipulate objects with this structure will keep modifications to the
// structure manageable and aid debugging.
//
// Once the first product has been shipped with this structure any change to it
// will need to be indicated by incrementing one or both of the version constants
// above. The later products will need to perfrom a version check at run time if
// they need to handle older packets.
//

#pragma pack( push, 1 )                 // Ensure structure packing is set to 1

                                        // Fixed length custom entries
#define CUSTOM_BOOL_VALUES     5
#define CUSTOM_DWORD_VALUES    5

                                        // Variable length custom entries
#define CUSTOM_STRING_ENTRIES  5
#define CUSTOM_BINARY_ENTRIES  1

#define CUSTOM_BUFFERS (CUSTOM_STRING_ENTRIES + CUSTOM_BINARY_ENTRIES)

#define INVALID_BUFFER_ENTRY 0xFFFFFFFF


/////////////////////////////////////////////////////////////////////
// This is the header at the beginning of all variable length
// buffers.  This will keep track of the ID of the entry and also the
// length of the entry.

typedef struct _CALERT_BUFFER_ENTRY
{
   DWORD dwBufferOffset;
   DWORD dwBufferLength;
} CALERT_BUFFER_ENTRY, *PCALERT_BUFFER_ENTRY;


/////////////////////////////////////////////////////////////////////
// Here is the actual packet definition

typedef struct _CALERT_NOTIFICATION_PACKET
{
   DWORD       dwTotalByteLength;       // Size of entire alert packet. This
                                        // must be the first struct member
                                        // to facilitate RPC transmission of
                                        // this structure.

   WORD        wVersionMajor;           // Major version of these structures.
   WORD        wVersionMinor;           // Minor version of these structures.

   DWORD       dwPacketType;            // Unique identifier of this "kind"
                                        // of packet.

   DWORD       dwPacketSource;          // Flags indicating originating location
                                        // and application of packet.

   UUID        uuidUniqueID;            // Identifier for this packet instance.
   SYSTEMTIME  sSystemTime;             // Time packet constructed/sent.

                                        // The following values are to be used
                                        // by the derived classes to store the
                                        // specific information they need to
                                        // transfer in the packet.
   BOOL        bBoolValues[CUSTOM_BOOL_VALUES];

   DWORD       dwDWordValues[CUSTOM_DWORD_VALUES];

                                        // This is for the variable length
                                        // entries in our packet
   CALERT_BUFFER_ENTRY cBufferEntries[CUSTOM_BUFFERS];

} CALERT_NOTIFICATION_PACKET, *PCALERT_NOTIFICATION_PACKET;


#pragma pack( pop )                     // Return packing to previous value


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

// Highest byte indicates originating OS
#define PACKET_SOURCE_DOS         0x01000000 // Packet is from a DOS machine
#define PACKET_SOURCE_WIN16       0x02000000 // Packet is from a Win 3.1 machine
#define PACKET_SOURCE_WIN95       0x03000000 // Packet is from a Windows 95 machine
#define PACKET_SOURCE_WINNT       0x04000000 // Packet is from a WIndows NT machine


///////////////////////////////////////////////////////////////////////////////
// Miscellaneous defines

#define BASE_PACKET_SIZE sizeof(CALERT_NOTIFICATION_PACKET)


///////////////////////////////////////////////////////////////////////////////
// CAlert class
//
// This purpose of this class is to build, manipulate and read a
// CALERT_NOTIFICATION_PACKET structure and should be used by any
// program thatneeds to send a packet to the Alert Service.
//
// An attempt has been made to keep the class as generic and portable as
// possible. There are no calls to Quake or MFC and very few to Win32. Almost
// all code is generic C/C++ and RTL.
//


class CAlert
{

private:
                                        // Private data goes here.

                                        // Internal pointer to our packet.
PCALERT_NOTIFICATION_PACKET m_pPacket;

                                        // This variable signifies if the
                                        // packet is locked for transport
BOOL                     m_bPacketLocked;

public:

/////////////////////////////////////////////////////////////////////
// Class management & minipulation functions

                                        // Default constructor
   CAlert();

                                        // Construction with an external buffer
   CAlert(
      LPBYTE lpBuffer);                 // [in] - Buffer to assign

                                        // Copy constructor
   CAlert(
      CAlert&);

                                        // Assignment operator
   CAlert& operator =(
      CAlert&);

                                        // Default destructor
   ~CAlert();


/////////////////////////////////////////////////////////////////////
// Buffer manipulation functions

private:
                                        // Private utility used in creating
                                        // blank packets.
   BOOL CreateBlankPacket(VOID);

public:

                                        // Returns pointer to the Data block
   PBYTE GetPacket();
                                        // Removes the lock on the buffer
   VOID ReleasePacket();

                                        // Buffer functions
   BOOL AssignPacket(
      PBYTE pBuffer);                   // [in] - Buffer to assign

                                        // Used to set the size of the data
                                        // block
   BOOL SetPacketLength(
      DWORD dwNewSize);                 // [in] - New buffer size

   BOOL GrowPacketLength(
      DWORD dwGrowSize);                // [in] - Bytes to grow by

   BOOL ShrinkPacketLength(
      DWORD dwShrinkSize);              // [in] - Bytes to shrink by

                                        // Current size of the data block
   DWORD GetPacketLength() CONST;

private:

   BOOL CreateBuffer(
      DWORD dwBufferNumber,             // [in] - Buffer slot to use
      DWORD dwBytesNeeded);             // [in] - Bytes needed in the buffer

   BOOL DeleteBuffer(
      DWORD dwBufferNumber);            // [in] - Buffer to delete

   BOOL SetBufferData(
      DWORD  dwBufferNumber,            // [in] - Buffer to receive data
      DWORD  dwBytesToSet,              // [in] - Number of bytes to put in
      LPBYTE lpData);                   // [in] - Data to be put in buffer

   BOOL GetBufferData(
      DWORD  dwBufferNumber,            // [in] - Buffer to get data from
      DWORD  dwDataSize,                // [in] - Size of buffer to receive data
      LPBYTE lpData);                   // [in] - Buffer to put data into

                                        // This function packs all of the
                                        // variable length buffers together and
                                        // updates the offsets.
   BOOL CompactBuffers();


/////////////////////////////////////////////////////////////////////
// Member variable access functions

public:
                                        // Version and purpose related functions
   BOOL SetPacketType(
      DWORD dwPacketType);              // [in] - Type to set for packet

   BOOL GetPacketType(
      DWORD & dwPacketType);            // [out] - Type of the current packet

   BOOL SetPacketSource(
      DWORD dwPacketSource);            // [in] - Type to set for packet

   BOOL GetPacketSource(
      DWORD & dwPacketSource);          // [out] - Type of the current packet

   BOOL SetPacketMachineType(
      DWORD dwMachineType);             // [in] - Machine type to set

   BOOL GetPacketMachineType(
      DWORD & dwMachineType);           // [out] - Machine type of the packet

   BOOL CreatePacketMachineType(VOID);

                                        // There is no set version for the
                                        // packet.  This will only be updated
                                        // when new code is written.  This
                                        // value is assigned when a packet is
                                        // created.
   BOOL GetVersion(
      WORD & wVersionMajor,             // [out] - Major version
      WORD & wVersionMinor);            // [out] - Minor version


                                        // UUID Functions
                                        // This function will create a UUID and
                                        // insert it into our structure.
   BOOL CreateUniqueID(VOID);

   BOOL SetUniqueID(
      CONST UUID& uuidUniqueID);        // [in] - UUID to assign to the packet

   BOOL GetUniqueID(
      UUID& uuidUniqueID );             // [out] - UUID from packet

                                        // Buffer Time Functions

                                        // Use this function to time stamp the
                                        // packet.
   BOOL StampTime(VOID);

   BOOL SetTime(
      CONST SYSTEMTIME& sTime);         // [in] - Time to set to this packet

   BOOL GetTime(
      SYSTEMTIME& sTime );              // [out] - Time from the packet.


/////////////////////////////////////////////////////////////////////
// Custom value assignment/retrieval section

                                        // BOOLEAN values
   BOOL SetBool(
      DWORD dwBoolNumber,               // [in] - number of the bool to set
      BOOL  bValue);                    // [in] - value to assign to the bool.

   BOOL GetBool(
      DWORD dwBoolNumber,               // [in] - number of the bool to get
      BOOL & bBool);                    // [out] - returned boolean value

                                        // DWORD values
   BOOL SetDWord(
      DWORD dwDWordNumber,              // [in] - number of the DWORD to set
      DWORD dwValue);                   // [in] - value to assign to the dword

   BOOL GetDWord(
      DWORD dwDWordNumber,              // [in] - number of the DWORD to get
      DWORD & dwValue);                 // [out] - returned DWORD value

                                        // String values
   BOOL SetString(
      DWORD  dwStringNumber,            // [in] - String to set
      LPTSTR lpszString);               // [in] - Value to assign to the string

#ifdef UNICODE
                                        // non-UNICODE version for UNICODE
   BOOL SetString(
      DWORD dwStringNumber,             // [in] - String to set
      LPSTR lpszString);                // [in] - Value to assign to the string

#else // ifdef UNICODE
                                        // non-UNICODE version for UNICODE
   BOOL SetString(
      DWORD  dwStringNumber,            // [in] - String to set
      LPWSTR lpszString);               // [in] - Value to assign to the string

#endif // ifdef UNICODE

   BOOL GetString(
      DWORD  dwStringNumber,            // [in]  - String to get
      DWORD  dwBufferSize,              // [in]  - Maximum size (BYTES) of string to get
      LPTSTR lpszString);               // [out] - The String

#ifdef UNICODE

   BOOL GetString(
      DWORD  dwStringNumber,            // [in]  - String to get
      DWORD  dwBufferSize,              // [in]  - Maximum size (BYTES) of string to get
      LPSTR lpszString);                // [out] - The String

#else // #ifdef UNICODE

   BOOL GetString(
      DWORD  dwStringNumber,            // [in]  - String to get
      DWORD  dwBufferSize,              // [in]  - Maximum size (BYTES) of string to get
      LPWSTR lpszString);               // [out] - The String

#endif // #ifdef UNICODE

   BOOL GetStringLength(
      DWORD dwStringNumber,             // [in]  - Number for string length
      DWORD & dwStringLength);          // [out] - String length in characters.

   BOOL SetBinaryData(
      DWORD  dwBufferNumber,            // [in] - Buffer to store data in
      DWORD  dwDataLength,              // [in] - Number of bytes to store
      LPBYTE lpData);                   // [in] - Data to Set

   BOOL GetBinaryData(
      DWORD  dwBufferNumber,            // [in] - Buffer to get data from
      DWORD  dwBufferLength,            // [in] - Length of the receive buffer
      LPBYTE lpData);                   // [out] - Data retrieved

/////////////////////////////////////////////////////////////////////
// Utility functions

   DWORD StringLength(
      DWORD dwStringNumber);            // [in] - Number of string to get size

   DWORD StringOffset(
      DWORD dwStringNumber);            // [in] - String to find.

};

#endif // if !defined(__CALERT_H)
