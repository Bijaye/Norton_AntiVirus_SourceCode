/////////////////////////////////////////////////////////////////////
// BUFFERS.CPP - Created - 05-08-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CALERT/VCS/buffers.cpv   1.6   12 Jun 1997 10:40:38   DSACKIN  $
//
// Description:
//    This contains the buffering functions for the CAlert class
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CALERT/VCS/buffers.cpv  $
// 
//    Rev 1.6   12 Jun 1997 10:40:38   DSACKIN
// Compact Buffers should be updating the dwPacketLength of the new packet.
//
//    Rev 1.5   09 Jun 1997 16:42:22   DSACKIN
// We need to allocate MORE than just the BASE size for the new packet
// during CompactBuffers.
//
//    Rev 1.4   05 Jun 1997 19:34:00   DSACKIN
// Had a memcpy from and to reversed.
//
//    Rev 1.3   05 Jun 1997 18:58:34   DSACKIN
// DeleteBuffer should check to see if the size is already zero before
// trying to delete the buffer.  We were checking to see if it was Not zero
// and exiting (falsly assuming it was empty already.)
//
//    Rev 1.2   05 Jun 1997 18:35:26   DSACKIN
// Pointer arithmetic should be done on a byte level and not a structure level.
//
//    Rev 1.1   14 May 1997 17:06:02   DSACKIN
// Changed the name of Packet functions to Packet instead of Buffers and then
// moved the functions into Packet.cpp
//
//    Rev 1.0   14 May 1997 13:39:30   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "calert.h"



/////////////////////////////////////////////////////////////////////
// BOOL CAlert::CreateBuffer(
//
// Purpose:
//    Creates a buffer appended to the end of our structure to
//    hold some custom data.
//
// Parameters:
//   DWORD dwBufferNumber -  [in] - Buffer slot to use
//   DWORD dwBytesNeeded  -  [in] - Bytes needed in the buffer
//
//
// Returns:
//   TRUE  - Success
//   FALSE - Failure
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::CreateBuffer(
   DWORD dwBufferNumber,             // [in] - Buffer slot to use
   DWORD dwBytesNeeded)              // [in] - Bytes needed in the buffer
{
DWORD dwNewOffset;
BOOL  bReturn = FALSE;
                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }

                                        // First check the subscript
   if (dwBufferNumber >= CUSTOM_BUFFERS)
   {
      goto Cleanup;
   }

                                        // Now check to see if the buffer is in
                                        // use
   if (m_pPacket->cBufferEntries[dwBufferNumber].dwBufferOffset > 0)
   {
      goto Cleanup;
   }

                                        // All new buffers go at the end of the
                                        // structure.
   dwNewOffset = m_pPacket->dwTotalByteLength;

                                        // Get the new space
   if (!GrowPacketLength(
           dwBytesNeeded))
   {
      goto Cleanup;
   }

                                        // Update our structure (This should
                                        // only happen when we know that we
                                        // were successful in the growing of
                                        // the buffer.
   m_pPacket->cBufferEntries[dwBufferNumber].dwBufferOffset = dwNewOffset;
   m_pPacket->cBufferEntries[dwBufferNumber].dwBufferLength = dwBytesNeeded;

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:
   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::DeleteBuffer(
//
// Purpose:
//    Deletes one of the variable length buffers.
//
// Parameters:
//    DWORD dwBufferNumber - [in] - Buffer to delete
//
// Returns:
//    TRUE  - Buffer deleted successfully
//    FALSE - Failed to delete the buffer.
//
/////////////////////////////////////////////////////////////////////
// 05-09-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::DeleteBuffer(
   DWORD dwBufferNumber)                // [in] - Buffer to delete
{
BOOL  bReturn = FALSE;
                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }

                                        // First check the subscript
   if (dwBufferNumber >= CUSTOM_BUFFERS)
   {
      goto Cleanup;
   }

                                        // Check to see if the buffer we are
                                        // trying to delete actually has
                                        // anything in it.
   if (m_pPacket->cBufferEntries[dwBufferNumber].dwBufferOffset == 0)
   {
      goto Cleanup;
   }

                                        // OK.  Now we know that we need to be
                                        // deleting the buffer.

                                        // Zero out the buffer
   m_pPacket->cBufferEntries[dwBufferNumber].dwBufferOffset = 0;
   m_pPacket->cBufferEntries[dwBufferNumber].dwBufferLength = 0;

                                        // Now take up the slack space.
   if (!CompactBuffers())
   {
      goto Cleanup;
   }

   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetBufferData(
//
// Purpose:
//    Use this function to put data into the allocated buffer.
//
// Parameters:
//   DWORD  dwBufferNumber - [in] - Buffer to receive data
//   DWORD  dwBytesToSet   - [in] - Number of bytes to put in
//   LPBYTE lpData         - [in] - Data to be put in buffer
//
// Returns:
//    TRUE  - Data copied into the buffer successfully.
//    FALSE - Probably too much data to copy into the buffer.
//          - or the packet is locked.
//
//
/////////////////////////////////////////////////////////////////////
// 05-09-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetBufferData(
      DWORD  dwBufferNumber,            // [in] - Buffer to receive data
      DWORD  dwBytesToSet,              // [in] - Number of bytes to put in
      LPBYTE lpData)                    // [in] - Data to be put in buffer
{
BOOL   bReturn = FALSE;
LPBYTE lpBuffer;
                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }

                                        // First check the subscript
   if (dwBufferNumber >= CUSTOM_BUFFERS)
   {
      goto Cleanup;
   }

                                        // Check to see if we have enough space
                                        // to copy in the new data.
   if (m_pPacket->cBufferEntries[dwBufferNumber].dwBufferLength < dwBytesToSet)
   {
      goto Cleanup;
   }

                                        // Calculate where to put the data
   lpBuffer = ((LPBYTE) m_pPacket) +
               m_pPacket->cBufferEntries[dwBufferNumber].dwBufferOffset;

                                        // Copy in the information
   memcpy(
      lpBuffer,
      lpData,
      dwBytesToSet);

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetBufferData(
//
// Purpose:
//    Gets the information out of the specified buffer
//
// Parameters:
//    DWORD  dwBufferNumber - [in] - Buffer to get data from
//    DWORD  dwDataSize     - [in] - Size of buffer to receive data
//    LPBYTE lpData         - [in] - Buffer to put data into
//
// Returns:
//    TRUE  - Successful copy of data made from the buffer
//    FALSE - Either an invalid buffer was specified or there
//          - wasn't enough space in the buffer provided for the copy.
//
/////////////////////////////////////////////////////////////////////
// 05-09-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetBufferData(
   DWORD  dwBufferNumber,               // [in] - Buffer to get data from
   DWORD  dwDataSize,                   // [in] - Size of buffer to receive data
   LPBYTE lpData)                       // [in] - Buffer to put data into
{
BOOL   bReturn = FALSE;
LPBYTE lpBuffer;
                                        // If there isn't a packet don't allow
                                        // this to happen.
   if (NULL == m_pPacket)
   {
      goto Cleanup;
   }

                                        // First check the subscript
   if (dwBufferNumber >= CUSTOM_BUFFERS)
   {
      goto Cleanup;
   }

                                        // Check to see if this buffer is being
                                        // used
   if (0 == m_pPacket->cBufferEntries[dwBufferNumber].dwBufferOffset)
   {
      goto Cleanup;
   }

                                        // Check for buffer space on the way
                                        // out.
   if (dwDataSize < m_pPacket->cBufferEntries[dwBufferNumber].dwBufferLength)
   {
      goto Cleanup;
   }

                                        // Calculate where to get the data from
   lpBuffer = ((LPBYTE) m_pPacket) +
               m_pPacket->cBufferEntries[dwBufferNumber].dwBufferOffset;

                                        // Copy the information
   memcpy(
      lpData,
      lpBuffer,
      m_pPacket->cBufferEntries[dwBufferNumber].dwBufferLength);

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;

}

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::CompactBuffers();
//
// Purpose:
//    This function takes out any dead space in our structure.
//    All variable length buffers are spaced end to end in this function.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - Successful packing.
//    FALSE - Packing was unsuccessful.
//
// Notes:
//    The following steps will be performed in this function in order:
//
//    1.) Basic packet validation
//    2.) Calculate the new packet size
//    3.) Check if we need to do any work
//    4.) Check for overlapped buffers.
//    5.) Allocate a new buffer
//    6.) Copy the basic structure over
//    7.) Copy over the variable length buffers.
//    8.) Re-Assign the m_pPacket to our NEW buffer.
//
/////////////////////////////////////////////////////////////////////
// 05-09-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::CompactBuffers()
{
PCALERT_NOTIFICATION_PACKET lpTemp;
BOOL   bReturn = FALSE;
DWORD  dwNewPacketSize;
DWORD  dwCurrentOffset;
LPBYTE lpFromPointer, lpToPointer;
DWORD  dwCount;
DWORD  dwFromOffset;
DWORD  dwBufferSize;


/////////////////////////////////////////////////////////////////////
// 1.) Basic Validation Check

                                        // If there isn't a packet or the
                                        // packet is locked, don't allow this
                                        // to happen.
   if ( (NULL == m_pPacket) ||
        (m_bPacketLocked) )
   {
      goto Cleanup;
   }


/////////////////////////////////////////////////////////////////////
// 2.) Calculate the new packet size
                                        // Start by calculating the new packet
                                        // size.
   dwNewPacketSize = BASE_PACKET_SIZE;

   for (dwCount = 0; dwCount < CUSTOM_BUFFERS; dwCount++)
   {
                                        // If the buffer is in use, add up the
                                        // packet size.
      if (0 != m_pPacket->cBufferEntries[dwCount].dwBufferOffset)
         dwNewPacketSize += m_pPacket->cBufferEntries[dwCount].dwBufferLength;
   }

/////////////////////////////////////////////////////////////////////
// 3.) Check to see if we have anything to compact
                                        // Is the packet already compacted?
   if (dwNewPacketSize == m_pPacket->dwTotalByteLength)
   {
                                        // Yep!  We don't have to do any more
                                        // work.
      goto Cleanup;
   }

/////////////////////////////////////////////////////////////////////
// 4.) Check for overlapped buffers
                                        // Check for packet validity
   if (dwNewPacketSize > m_pPacket->dwTotalByteLength)
   {
                                        // We have REAL problems if we get
                                        // here.  This means that some of the
                                        // packets have overlapped data.
      assert(FALSE);
      goto Cleanup;
   }


/////////////////////////////////////////////////////////////////////
// 5.) Allocate a new buffer.

                                        // Allocate a place for us to store the
                                        // packet while restructuring.
   lpTemp = (PCALERT_NOTIFICATION_PACKET) new BYTE [dwNewPacketSize];

                                        // Check the allocation
   if (NULL == lpTemp)
   {
                                        // Memory allocation failure
      goto Cleanup;
   }

/////////////////////////////////////////////////////////////////////
// 6.) Copy the basic structure over

   memcpy(
      (LPBYTE) lpTemp,
      (LPBYTE) m_pPacket,
      BASE_PACKET_SIZE);


/////////////////////////////////////////////////////////////////////
// 7.) Copy over the variable length buffers.

   dwCurrentOffset = BASE_PACKET_SIZE;

   for (dwCount = 0; dwCount < CUSTOM_BUFFERS; dwCount++)
   {
                                        // Check to see if we need to move it

      dwFromOffset = m_pPacket->cBufferEntries[dwCount].dwBufferOffset;
      if (dwFromOffset > 0)
      {
                                        // Calculate the move
         lpFromPointer = (LPBYTE) (((LPBYTE)m_pPacket) + dwFromOffset);
         lpToPointer   = (LPBYTE) (((LPBYTE)lpTemp) + dwCurrentOffset);

                                        // How much to move?
         dwBufferSize = m_pPacket->cBufferEntries[dwCount].dwBufferLength;

                                        // Copy the data
         memcpy(
            lpToPointer,
            lpFromPointer,
            dwBufferSize);

                                        // Update our structure and variables
         lpTemp->cBufferEntries[dwCount].dwBufferOffset = dwCurrentOffset;
         lpTemp->cBufferEntries[dwCount].dwBufferLength = dwBufferSize;

         dwCurrentOffset += dwBufferSize;
      }
   }

                                        // We must update the size becuase it
                                        // is different from the old packet.
                                        // The memcpy takes a copy of the old
                                        // one, but we need the 'shrunk' size.
   lpTemp->dwTotalByteLength = dwNewPacketSize;

/////////////////////////////////////////////////////////////////////
// 8.) Re-Assign the m_pPacket to our NEW buffer.

   delete m_pPacket;
   m_pPacket = lpTemp;

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}









