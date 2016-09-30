/////////////////////////////////////////////////////////////////////
// CUSTOM.CPP - Created - 05-08-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CALERT/VCS/custom.cpv   1.1   05 Jun 1997 13:36:12   DSACKIN  $
//
// Description:
//    Here is where the CAlert custom data access functions are
//    implemented.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CALERT/VCS/custom.cpv  $
// 
//    Rev 1.1   05 Jun 1997 13:36:12   DSACKIN
// The GetBinaryData guy needs to treat the m_pPacket as a LPBYTE when
// adding a number to itself.
//
//    Rev 1.0   14 May 1997 13:40:04   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include <assert.h>
#include "calert.h"


/////////////////////////////////////////////////////////////////////
// Now for the custom data manipulation functions

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetBool(
//
// Purpose:
//    Sets one of the custom boolean values in our structure.
//
// Parameters:
//   DWORD dwBoolNumber - [in] - number of the bool to set
//   BOOL  bValue       - [in] - value to assign to the bool.
//
// Returns:
//   TRUE  - Boolean value set successfully
//   FALSE - Either there was no packet, the packet was locked or
//           the subscript was out of range.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetBool(
   DWORD dwBoolNumber,                  // [in] - number of the bool to set
   BOOL  bValue)                        // [in] - value to assign to the bool.
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

                                        // Check the subscript.
   if (dwBoolNumber >= CUSTOM_BOOL_VALUES)
   {
                                        // Out of our range!
      goto Cleanup;
   }

                                        // Set our value
   m_pPacket->bBoolValues[dwBoolNumber] = bValue;

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetBool(
//
// Purpose:
//    Retrieves one of the boolean values from the packet.
//
// Parameters:
//    DWORD dwBoolNumber -  [in] - number of the bool to get
//    BOOL & bValue      -  [out] - returned boolean value
//
//
// Returns:
//    TRUE  - Successful retrieval
//    FALSE - Retrieval was not successful.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetBool(
   DWORD dwBoolNumber,                  // [in] - number of the bool to get
   BOOL & bValue)                       // [out] - returned boolean value
{
BOOL bReturn = FALSE;

                                        // Make sure there is a packet.
   if (NULL == m_pPacket)
      goto Cleanup;

                                        // Check the subscript.
   if (dwBoolNumber >= CUSTOM_BOOL_VALUES)
   {
                                        // Out of our range!
      goto Cleanup;
   }

                                        // Otherwise, set our value.
   bValue = m_pPacket->bBoolValues[dwBoolNumber];

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetDWord(
//
// Purpose:
//    Sets one of the custom DWORD values in our structure.
//
// Parameters:
//   DWORD dwDWordNumber - [in] - number of the DWORD to set
//   DWORD dwValue       - [in] - value to assign to the DWORD.
//
// Returns:
//   TRUE  - DWORD value set successfully
//   FALSE - Either there was no packet, the packet was locked or
//           the subscript was out of range.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetDWord(
   DWORD dwDWordNumber,                 // [in] - number of the DWORD to set
   DWORD dwValue)                       // [in] - value to assign to the DWORD.
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

                                        // Check the subscript.
   if (dwDWordNumber >= CUSTOM_DWORD_VALUES)
   {
                                        // Out of our range!
      goto Cleanup;
   }

                                        // Set our value
   m_pPacket->dwDWordValues[dwDWordNumber] = dwValue;

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetDWord(
//
// Purpose:
//    Retrieves one of the DWORD values from the packet.
//
// Parameters:
//    DWORD dwDWordNumber -  [in]  - number of the DWORD to get
//    DWORD & dwValue     -  [out] - returned DWORD value
//
// Returns:
//    TRUE  - Successful retrieval
//    FALSE - Retrieval was not successful.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetDWord(
   DWORD dwDWordNumber,                 // [in] - number of the DWORD to get
   DWORD & dwValue)                        // [out] - returned boolean value
{
BOOL bReturn = FALSE;

                                        // Make sure there is a packet.
   if (NULL == m_pPacket)
      goto Cleanup;

                                        // Check the subscript.
   if (dwDWordNumber >= CUSTOM_DWORD_VALUES)
   {
                                        // Out of our range!
      goto Cleanup;
   }

                                        // Otherwise, get our value.
   dwValue = m_pPacket->dwDWordValues[dwDWordNumber];

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetBinaryData(
//
// Purpose:
//    This Function will put information into one of the buffers in
//    our packet.  Functions such as setString should use this to
//    store their information.
//
// Parameters:
//   DWORD  dwBufferNumber - [in] - Buffer to store data in
//   DWORD  dwDataLength   - [in] - Number of bytes to store
//   LPBYTE lpData         - [in] - Data to Set
//
// Returns:
//   TRUE  - Data was successfully placed in the buffer.
//   FALSE - Probably a buffer creation problem.
//
//
/////////////////////////////////////////////////////////////////////
// 05-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetBinaryData(
   DWORD  dwBufferNumber,               // [in] - Buffer to store data in
   DWORD  dwDataLength,                 // [in] - Number of bytes to store
   LPBYTE lpData)                       // [in] - Data to Set
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

                                        // Check the subscript.
   if (dwBufferNumber >= CUSTOM_BUFFERS)
   {
                                        // Out of our range!
      goto Cleanup;
   }

                                        // Check to see if the buffer is
                                        // already in use, and if so, delete
                                        // the current info out of the buffer.
   if (m_pPacket->cBufferEntries[dwBufferNumber].dwBufferOffset > 0)
   {
                                        // We must delete this buffer.
      DeleteBuffer(dwBufferNumber);
   }

                                        // Try to create a new buffer.
   if (!CreateBuffer(
           dwBufferNumber,
           dwDataLength))
   {
      goto Cleanup;
   }

                                        // Now put the info into the buffer.
   if (!SetBufferData(
           dwBufferNumber,
           dwDataLength,
           lpData))
   {
      goto Cleanup;
   }

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetBinaryData(
//
// Purpose:
//
//
// Parameters:
//   DWORD  dwBufferNumber - [in]  - Buffer to get data from
//   DWORD  dwBufferLength - [in]  - Length of the receive buffer
//   LPBYTE lpData         - [out] - Data retrieved
//
//
// Returns:
//   TRUE  - Data retrieved successfully.
//   FALSE - Something wasn't set up correctly.
//
/////////////////////////////////////////////////////////////////////
// 05-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetBinaryData(
   DWORD  dwBufferNumber,               // [in] - Buffer to get data from
   DWORD  dwBufferLength,               // [in] - Length of the receive buffer
   LPBYTE lpData)                       // [out] - Data retrieved
{
BOOL  bReturn = FALSE;
DWORD dwBufferOffset;

                                        // Make sure there is a packet.
   if (NULL == m_pPacket)
      goto Cleanup;

                                        // Check the subscript.
   if (dwBufferNumber >= CUSTOM_BUFFERS)
   {
                                        // Out of our range!
      goto Cleanup;
   }

                                        // Check to see if the buffer is being
                                        // used
   dwBufferOffset = m_pPacket->cBufferEntries[dwBufferNumber].dwBufferOffset;
   if (0 == dwBufferOffset)
   {
      goto Cleanup;
   }

                                        // Now check for enough space in the
                                        // buffer
   if (dwBufferLength < m_pPacket->cBufferEntries[dwBufferNumber].dwBufferLength)
   {
      goto Cleanup;
   }

                                        // Check for a NULL pointer being
                                        // passed to us.
   if (NULL == lpData)
   {
      goto Cleanup;
   }

                                        // All clear! - Copy the data.
   memcpy(
      lpData,
      (LPBYTE) ( ((LPBYTE)m_pPacket) + dwBufferOffset),
      m_pPacket->cBufferEntries[dwBufferNumber].dwBufferLength);

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}


