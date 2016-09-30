/////////////////////////////////////////////////////////////////////
// PACKET.CPP - Created - 05-14-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CALERT/VCS/packet.cpv   1.3   12 Jun 1997 10:38:08   DSACKIN  $
//
// Description:
//    These functions are for manipulating the packet as a whole.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CALERT/VCS/packet.cpv  $
// 
//    Rev 1.3   12 Jun 1997 10:38:08   DSACKIN
// Typo - got an = sign instead of a - sign in one of the calculations.
//
//    Rev 1.2   29 May 1997 09:25:22   DSACKIN
// Make sure to initialize the Packet Size variable in the constructor.
//
//    Rev 1.1   15 May 1997 14:09:30   DSACKIN
// Added version information
//
//    Rev 1.0   14 May 1997 17:05:50   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "calert.h"


/////////////////////////////////////////////////////////////////////
// VOID CAlert::CreateBlankPacket(VOID)
//
// Purpose:
//    Allocates memory for our m_pPacket member.  It also initializes
//    the structure if allocation is successful.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - Packet Created successfully.
//    FALSE - Probably a memory allocation error.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::CreateBlankPacket(VOID)
{
BOOL  bReturn = FALSE;
DWORD dwCount;

   m_pPacket = (PCALERT_NOTIFICATION_PACKET) new BYTE [BASE_PACKET_SIZE];

   if (NULL == m_pPacket)
      goto Cleanup;

                                        // Zero it out for our use
   memset(
      m_pPacket,
      0,
      BASE_PACKET_SIZE );

   m_pPacket->wVersionMajor = PACKET_VERSION_MAJOR;
   m_pPacket->wVersionMinor = PACKET_VERSION_MINOR;

   m_pPacket->dwTotalByteLength = BASE_PACKET_SIZE;

                                        // Now initialize all variable length
                                        // buffer offsets
   for (dwCount = 0; dwCount < CUSTOM_BUFFERS; dwCount++)
   {
      m_pPacket->cBufferEntries[dwCount].dwBufferOffset = 0;
      m_pPacket->cBufferEntries[dwCount].dwBufferLength = 0;
   }

   bReturn = TRUE;

Cleanup:
   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// PBYTE CAlert::GetPacket()
//
// Purpose:
//    Returns the internal packet to the caller.
//
// Parameters:
//    none.
//
// Returns:
//    PBYTE - reference to the internal Packet.
//
// Notes:
//    This function locks this class.  You must call ReleasePacket
//    before any of the member functions will work again.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
PBYTE CAlert::GetPacket()
{
                                        // Lock the packet
   m_bPacketLocked = TRUE;

                                        // Give them what they are looking for.
   return ( (PBYTE) m_pPacket );
}

/////////////////////////////////////////////////////////////////////
// VOID CAlert::ReleasePacket()
//
// Purpose:
//    This is the function that an application will call when they
//    are done with the internal Packet of this structure.
//    The only thing this function does is set the flag to enable
//    the member functions again.
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CAlert::ReleasePacket()
{
   m_bPacketLocked = FALSE;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::AssignPacket(
//
// Purpose:
//    Allows us to accept an external Packet and set up our structure
//    to use it.
//
// Parameters:
//   PBYTE pPacket - [in] - Packet to assign
//
// Returns:
//   TRUE  - External Packet was assigned successfully
//   FALSE - External Packet was NOT assigned successfully.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::AssignPacket(
   PBYTE pPacket)                       // [in] - Packet to assign
{
PCALERT_NOTIFICATION_PACKET lpPacket;
DWORD                       dwSize;
BOOL                        bReturn = FALSE;

                                        // Don't allow any changes to the
                                        // packet if someone has it locked!
   if (m_bPacketLocked)
      goto Cleanup;

                                        // First, we must get rid of any
                                        // buffers that we currently have.
   if (m_pPacket)
   {
      delete m_pPacket;
      m_pPacket = NULL;
   }
                                        // Assign the external buffer into
                                        // something we can look at
   lpPacket = (PCALERT_NOTIFICATION_PACKET) pPacket;

                                        // Get the size
   dwSize = lpPacket->dwTotalByteLength;

                                        // Now allocate the memory so that we
                                        // can make our own copy.
   m_pPacket = (PCALERT_NOTIFICATION_PACKET) new BYTE [dwSize];

                                        // Did we get our memory?
   if (NULL == m_pPacket)
      goto Cleanup;

                                        // Copy in the information
   memcpy (
      m_pPacket,
      pPacket,
      dwSize);

                                        // OK.  We have our OWN copy of the
                                        // information.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetPacketLength(
//
// Purpose:
//    Changes the buffer length.  This will be used in adding and
//    removing variable length buffers.
//
// Parameters:
//    DWORD dwNewSize - [in] - new size for the Packet
//
// Returns:
//    TRUE  - Packet was re-sized successfully
//    FALSE - Failed to re-size the Packet.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetPacketLength(
   DWORD dwNewSize)
{
BOOL  bReturn = FALSE;
DWORD dwCopyBytes;
PBYTE pNewPacket;

                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }

                                        // Make sure they aren't trying to
                                        // shrink it below our minimum!
   if (dwNewSize < BASE_PACKET_SIZE)
      goto Cleanup;

                                        // Start by making a new packet
   pNewPacket = new BYTE [dwNewSize];

                                        // Check our memory allocation
   if (NULL == pNewPacket)
      goto Cleanup;


                                        // Now figure out how much needs to be
                                        // copied.
   dwCopyBytes = (m_pPacket->dwTotalByteLength > dwNewSize) ?
                    dwNewSize :
                    m_pPacket->dwTotalByteLength;

                                        // Copy in our current packet
   memcpy(
      pNewPacket,
      m_pPacket,
      dwCopyBytes);

                                        // Get rid of the old packet
   delete m_pPacket;

                                        // Set the new packet.
   m_pPacket = (PCALERT_NOTIFICATION_PACKET) pNewPacket;

                                        // Now update our size DWORD in the
                                        // packet header
   m_pPacket->dwTotalByteLength = dwNewSize;

                                        // Indicate success
   bReturn = TRUE;

Cleanup:

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GrowPacketLength(
//
// Purpose:
//    Calculates the new size and calls SetPacketLength
//
// Parameters:
//    DWORD dwGrowSize - [in] - Bytes to grow by
//
// Returns:
//    TRUE  - Packet has grown successfully
//    FALSE - Some kind of an error occurred
//
// See Also:
//    SetPacketLength
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GrowPacketLength(
   DWORD dwGrowSize)                    // [in] - Bytes to grow by
{
BOOL  bReturn = FALSE;
DWORD dwNewSize;

                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }

   dwNewSize = m_pPacket->dwTotalByteLength + dwGrowSize;

   bReturn = SetPacketLength(
                dwNewSize);

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::ShrinkPacketLength(
//
// Purpose:
//    Calculates the new size and calls SetPacketLength
//
// Parameters:
//    DWORD dwShrinkSize - [in] - Bytes to shrink by
//
// Returns:
//    TRUE  - Packet was shrunk successfully
//    FALSE - Some kind of an error occurred.
//
// See Also:
//    SetPacketLength
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::ShrinkPacketLength(
   DWORD dwShrinkSize)                  // [in] - Bytes to shrink by
{
BOOL  bReturn = FALSE;
DWORD dwNewSize;

                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }

                                        // We must be a little more careful
                                        // here because we don't want them
                                        // trying to shrink the buffer more
                                        // than we have in it.

   if (dwShrinkSize > m_pPacket->dwTotalByteLength)
      goto Cleanup;

                                        // Calculate the new buffer size
   dwNewSize = m_pPacket->dwTotalByteLength - dwShrinkSize;

                                        // Set our packet size!
   bReturn = SetPacketLength(
                dwNewSize);

Cleanup:
   return bReturn;

}


/////////////////////////////////////////////////////////////////////
// DWORD CAlert::GetPacketLength() CONST
//
// Purpose:
//    This returns the buffer length of the internal buffer.
//
// Parameters:
//    none.
//
// Returns:
//    DWORD - Size of our internal buffer.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CAlert::GetPacketLength() CONST
{

                                        // If we have a packet, return the
                                        // size.  Otherwise return 0
   if (NULL !=  m_pPacket)
      return m_pPacket->dwTotalByteLength;
   else
      return 0;
}





