/////////////////////////////////////////////////////////////////////
// DATAACCS.CPP - Created - 05-08-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CALERT/VCS/DATAACCS.CPv   1.2   21 Aug 1997 02:20:16   DSACKIN  $
//
// Description:
//    This file contains the data access functions for the CAlert class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CALERT/VCS/DATAACCS.CPv  $
// 
//    Rev 1.2   21 Aug 1997 02:20:16   DSACKIN
// Added functions to handle machine type.
//
//    Rev 1.1   15 May 1997 14:10:12   DSACKIN
// Added versioning information.
//
//    Rev 1.0   14 May 1997 13:40:10   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "calert.h"



                                        // Version and purpose related functions
/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetPacketType(
//
// Purpose:
//    Sets the packet type.
//
// Parameters:
//    DWORD dwPacketType - [in] - Type to set for packet
//
// Returns:
//    TRUE  - Packet type set successfully.
//    FALSE - Probably locked or invalid packet.
//
/////////////////////////////////////////////////////////////////////
// 05-14-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetPacketType(
   DWORD dwPacketType)                  // [in] - Type to set for packet
{
BOOL bReturn = FALSE;

                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }

   m_pPacket->dwPacketType = dwPacketType;

   bReturn = TRUE;

Cleanup:

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetPacketType(
//
// Purpose:
//    Retrieves the current packet type.
//
// Parameters:
//    DWORD & dwPacketType - [out] - Type of the current packet
//
// Returns:
//    TRUE  - Retrieved the packet type successfully.
//    FALSE - Unable to get the current packet type.
//
/////////////////////////////////////////////////////////////////////
// 05-14-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetPacketType(
   DWORD & dwPacketType)                // [out] - Type of the current packet
{
BOOL bReturn = FALSE;
                                        // Make sure there is a packet.
   if (NULL == m_pPacket)
   {
      goto Cleanup;
   }

   dwPacketType = m_pPacket->dwPacketType;

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetPacketSource(
//
// Purpose:
//    Sets the packet Source.
//
// Parameters:
//    DWORD dwPacketSource - [in] - Source to set for packet
//
// Returns:
//    TRUE  - Packet Source set successfully.
//    FALSE - Probably locked or invalid packet.
//
/////////////////////////////////////////////////////////////////////
// 05-14-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetPacketSource(
   DWORD dwPacketSource)                // [in] - Type to set for packet
{
BOOL bReturn = FALSE;

                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }

                                        // Preserve the high byte for machine
                                        // type.
   m_pPacket->dwPacketSource &= 0xFF000000;

   m_pPacket->dwPacketSource |= (dwPacketSource & 0x00FFFFFF);

   bReturn = TRUE;

Cleanup:

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetPacketSource(
//
// Purpose:
//    Retrieves the current packet Source.
//
// Parameters:
//    DWORD & dwPacketSource - [out] - Source of the current packet
//
// Returns:
//    TRUE  - Retrieved the packet Source successfully.
//    FALSE - Unable to get the current packet Source.
//
/////////////////////////////////////////////////////////////////////
// 05-14-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetPacketSource(
   DWORD & dwPacketSource)              // [out] - Source of the current packet
{
BOOL bReturn = FALSE;
                                        // Make sure there is a packet.
   if (NULL == m_pPacket)
   {
      goto Cleanup;
   }

   dwPacketSource = m_pPacket->dwPacketSource;

                                        // Only return the low 3 bytes of this
                                        // field because the high byte is the
                                        // machine type.
   dwPacketSource &= 0x00FFFFFF;

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetPacketMachineType(
//
// Purpose:
//
//
// Parameters:
//   DWORD dwMachineType - [in] - Machine type to set
//
// Returns:
//   TRUE  - Machine type was set correctly.
//   FALSE - The packet was probably locked.
//
// Notes:
//
//
/////////////////////////////////////////////////////////////////////
// 08/21/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetPacketMachineType(
   DWORD dwMachineType)                 // [in] - Machine type to set
{
BOOL bReturn = FALSE;

                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }

                                        // Zero out the current machine type.
   m_pPacket->dwPacketSource &= 0x00FFFFFF;

                                        // Now OR in the new machine type
   m_pPacket->dwPacketSource |= dwMachineType;

   bReturn = TRUE;

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetPacketMachineType(
//
// Purpose:
//
//
// Parameters:
//   DWORD & dwMachineType - [out] - Machine type of the packet
//
// Returns:
//   TRUE  - machine type retrieved successfully.
//   FALSE - Unable to get machine type.
//
// Notes:
//
//
/////////////////////////////////////////////////////////////////////
// 08/21/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetPacketMachineType(
   DWORD & dwMachineType)               // [out] - Machine type of the packet
{
BOOL  bReturn = FALSE;
                                        // Make sure there is a packet.
   if (NULL == m_pPacket)
   {
      goto Cleanup;
   }

   dwMachineType = m_pPacket->dwPacketSource;

                                        // Only return the high byte for the
                                        // machine type.
   dwMachineType &= 0xFF000000;

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::CreatePacketMachineType(VOID);
//
// Purpose:
//    This function will call the Win32 function to get the
//    windows type.  We will only support Win32 classifications
//    of the alert packet here.
//
// Parameters:
//    none.
//
// Returns:
//    none.   - The packet data contains what we found.
//
// Notes:
//
//
/////////////////////////////////////////////////////////////////////
// 08/21/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::CreatePacketMachineType(VOID)
{
BOOL  bReturn = FALSE;
DWORD dwWindowsVersion = 0;
DWORD dwMachineType = 0;

   dwWindowsVersion = ::GetVersion();

                                        // Windows NT has the high bit of a
                                        // DWORD cleared.
   if (dwWindowsVersion < 0x80000000)
   {
      dwMachineType = PACKET_SOURCE_WINNT;
   }
   else
   {
      dwMachineType = PACKET_SOURCE_WIN95;
   }

   bReturn = SetPacketMachineType (
                dwMachineType);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetVersion(
//
// Purpose:
//    This function gets the version from the current packet.  There
//    will be no function to take care of setting the version because
//    that functionality will be taken care of in the constructor
//    of the function.
//
// Parameters:
//    WORD & wVersionMajor - [out] - Major version
//    WORD & wVersionMinor - [out] - Minor version
//
// Returns:
//    TRUE  - Versions retrieved successfully.
//    FALSE - Unable to retrieve versions (probably invalid packet).
//
/////////////////////////////////////////////////////////////////////
// 05-14-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetVersion(
   WORD & wVersionMajor,                // [out] - Major version
   WORD & wVersionMinor)                // [out] - Minor version
{
BOOL bReturn = FALSE;
                                        // Make sure there is a packet.
   if (NULL == m_pPacket)
   {
      goto Cleanup;
   }

   wVersionMajor = m_pPacket->wVersionMajor;
   wVersionMinor = m_pPacket->wVersionMinor;

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}




/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetUniqueID(
//
// Purpose:
//    Inserts a UUID into our structure.
//
// Parameters:
//   CONST UUID& uuidUniqueID - [in] - UUID to assign to the packet
//
// Returns:
//   TRUE  - UUID Set successfully.
//   FALSE - Packet is invalid or locked.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetUniqueID(
   CONST UUID& uuidUniqueID)            // [in] - UUID to assign to the packet
{
BOOL bReturn = FALSE;

                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }
                                        // Copy in the new UUID
   memcpy(
      &m_pPacket->uuidUniqueID,
      &uuidUniqueID,
      sizeof(UUID) );

   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetUniqueID(
//
// Purpose:
//    Retrieves the UUID from our structure
//
// Parameters:
//   UUID& uuidUniqueID - [out] - UUID from packet
//
// Returns:
//   TRUE  - UUID retrieved successfully.
//   FALSE - Packet is invalid
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetUniqueID(
   UUID& uuidUniqueID)                  // [out] - UUID from packet
{
BOOL bReturn = FALSE;
                                        // Make sure there is a packet.
   if (NULL == m_pPacket)
   {
      goto Cleanup;
   }

   memcpy(
      &uuidUniqueID,
      &m_pPacket->uuidUniqueID,
      sizeof(UUID) );

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::CreateUniqueID();
//
// Purpose:
//    Generates a UUID and inserts it into our packet.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - UUID created and inserted correctly.
//    FALSE - Unable to create or set the UUID.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::CreateUniqueID(VOID)
{
BOOL bReturn = FALSE;
UUID uuidNewUniqueID;

                                        // Generate our UUID
   if ( RPC_S_OK != UuidCreate( &uuidNewUniqueID ) )
   {
      goto Cleanup;
   }

                                        // Insert it into our packet.
   bReturn = SetUniqueID(
                uuidNewUniqueID);

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetTime(
//
// Purpose:
//    Copies the specified time into the structure.
//
// Parameters:
//    CONST SYSTEMTIME& sTime - [in] - Time to set to this packet
//
// Returns:
//    TRUE  - Time set successfully.
//    FALSE - Invalid packet or the packet was locked.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetTime(
   CONST SYSTEMTIME& sTime)             // [in] - Time to set to this packet
{
BOOL bReturn = FALSE;
                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }
                                        // Copy in the new TIME
   memcpy(
      &m_pPacket->sSystemTime,
      &sTime,
      sizeof(SYSTEMTIME) );

   bReturn = TRUE;

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetTime(
//
// Purpose:
//    Gets the time out of our packet.
//
// Parameters:
//   SYSTEMTIME& sTime - [out] - Time from the packet.
//
// Returns:
//    TRUE  - Time was successfully retrieved.
//    FALSE - Probably an invalid packet.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetTime(
   SYSTEMTIME& sTime )                  // [out] - Time from the packet.
{
BOOL bReturn = FALSE;
                                        // Make sure there is a packet.
   if (NULL == m_pPacket)
   {
      goto Cleanup;
   }

                                        // Copy the TIME back into the struct
                                        // provided.
   memcpy(
      &sTime,
      &m_pPacket->sSystemTime,
      sizeof(SYSTEMTIME) );

   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL StampTime()
//
// Purpose:
//    Gets the current time and stamps it into the packet.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - Time stamped successfully.
//    FALSE - Unable to set the time in the packet.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::StampTime(VOID)
{
BOOL bReturn = FALSE;
SYSTEMTIME sSystemTime;

                                        // Lets start by getting the current
                                        // system time (Functions doesn't fail)
   GetSystemTime(
      &sSystemTime);

                                        // Now stamp it into our structure.
   bReturn = SetTime(
                sSystemTime);

   return bReturn;
}





