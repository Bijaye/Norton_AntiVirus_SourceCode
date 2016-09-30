/////////////////////////////////////////////////////////////////////
// CALERT.CPP - Created - 05-08-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CALERT/VCS/calert.cpv   1.3   03 Jun 1997 17:16:24   DSACKIN  $
//
// Description:
//    This is where the functionality of the CAlert class goes.
//    This code was originally copied from the ALRTNTFY project.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CALERT/VCS/calert.cpv  $
// 
//    Rev 1.3   03 Jun 1997 17:16:24   DSACKIN
// Make sure that m_pPacket is NULL before we call AssignPacket so that
// we don't delete a variable that hasn't actually been allocated.
//
//    Rev 1.2   03 Jun 1997 15:29:24   DSACKIN
// The Packet must be marked as unlocked before assignbuffer is called in the
// constructors/assignment operators.
//
//    Rev 1.1   14 May 1997 17:06:30   DSACKIN
// Updated use of Packet functions from Buffer functions.
//
//    Rev 1.0   14 May 1997 13:39:40   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "calert.h"



/////////////////////////////////////////////////////////////////////
// CAlert::CAlert()
//
// Purpose:
//    Generic initialization of the CAlert class
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
// Notes:
//    If the memory allocation fails, the object will not function,
//    but it will fail gracefully.
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlert::CAlert()
{
                                        // Use our member function to create
                                        // the new buffer.
   CreateBlankPacket();

                                        // We are only locked when someone
                                        // calls the GetBuffer Function.
   m_bPacketLocked = FALSE;
}

/////////////////////////////////////////////////////////////////////
// CAlert::CAlert(
//
// Purpose:
//    Creation of a CAlert class and assign an external buffer at
//    the same time.
//
// Parameters:
//   LPBYTE lpBuffer - [in] - Buffer to assign.
//
// Returns:
//    none.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlert::CAlert(
   LPBYTE lpBuffer)                     // [in] - Buffer to assign.
{
                                        // Initialize us to NULL so that when
                                        // we call to AssignPacket we will not
                                        // delete a buffer that doesn't exist.
   m_pPacket = NULL;

                                        // We are only locked when someone
                                        // calls the GetBuffer Function.
   m_bPacketLocked = FALSE;

   if (NULL == lpBuffer)
   {
                                        // Use our member function to create
                                        // the new buffer.
      CreateBlankPacket();
   }
   else
   {
                                        // Use our buffer assignment function
                                        // to put in the new buffer.
      AssignPacket(
         lpBuffer);
   }
}


/////////////////////////////////////////////////////////////////////
// CAlert::CAlert(
//
// Purpose:
//    Allows us to construct one object that is a copy of another.
//
// Parameters:
//    CAlert & pAlert - [in] - Object to take a copy of.
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlert::CAlert(
   CAlert & pAlert)                     // [in] - Object to take a copy of.
{
LPBYTE lpBuffer;

                                        // This locks the target buffer
   lpBuffer = pAlert.GetPacket();

                                        // Initialize us to NULL so that when
                                        // we call to AssignPacket we will not
                                        // delete a buffer that doesn't exist.
   m_pPacket = NULL;

                                        // We are only locked when someone
                                        // calls the GetBuffer Function.
   m_bPacketLocked = FALSE;

   if (NULL == lpBuffer)
   {
                                        // Use our member function to create
                                        // the new buffer.
      CreateBlankPacket();
   }
   else
   {
                                        // Use our buffer assignment function
                                        // to put in the new buffer.
      AssignPacket(
         lpBuffer);
   }

                                        // DON'T Leave the alert locked!
   pAlert.ReleasePacket();
}





/////////////////////////////////////////////////////////////////////
// CAlert& CAlert::operator = (
//
// Purpose:
//    This allows someone to use the = assignment operator to copy
//    the contents of one object to another.
//
// Parameters:
//    CAlert& pRightHandObject - [in] - Object to copy
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlert& CAlert::operator = (
   CAlert& pRightHandObject)            // [in] - Object to copy
{
LPBYTE lpBuffer;

                                        // This locks the target buffer
   lpBuffer = pRightHandObject.GetPacket();

                                        // Initialize us to NULL so that when
                                        // we call to AssignPacket we will not
                                        // delete a buffer that doesn't exist.
   m_pPacket = NULL;

                                        // We are only locked when someone
                                        // calls the GetBuffer Function.
   m_bPacketLocked = FALSE;

   if (NULL == lpBuffer)
   {
                                        // Use our member function to create
                                        // the new buffer.
      CreateBlankPacket();
   }
   else
   {
                                        // Use our buffer assignment function
                                        // to put in the new buffer.
      AssignPacket(
         lpBuffer);
   }

                                        // DON'T Leave the alert locked!
   pRightHandObject.ReleasePacket();
                                        // Send ourselves back to the left hand
                                        // side.
   return *this;
}


/////////////////////////////////////////////////////////////////////
// CAlert::~CAlert()
//
// Purpose:
//    Default destructor
//
// Parameters:
//    none
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 05-08-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlert::~CAlert()
{

                                        // We aren't going to check the locked
                                        // flag because we are about to go away
                                        // anyway.  We can't leave our buffer
                                        // around no-matter who has it locked.

                                        // Basically go through and clean up
                                        // our memory.

   if (NULL != m_pPacket)
   {
      delete m_pPacket;
      m_pPacket = NULL;
   }
}

