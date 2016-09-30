/////////////////////////////////////////////////////////////////////
// CVALERT.CPP - Created - 05-15-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CVALERT/VCS/cvalert.cpv   1.3   29 May 1997 10:01:16   DSACKIN  $
//
// Description:
//    This file is where the CVirusAlert class implementation resides.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CVALERT/VCS/cvalert.cpv  $
// 
//    Rev 1.3   29 May 1997 10:01:16   DSACKIN
// Forgot to include AlerType.h
//
//    Rev 1.2   29 May 1997 09:59:38   DSACKIN
// Don't forget to set the PacketType in the constructor.
//
//    Rev 1.1   18 May 1997 05:00:50   DSACKIN
// Fixed a protoype
//
//    Rev 1.0   16 May 1997 13:29:34   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "CValert.h"
#include "AlerType.h"


/////////////////////////////////////////////////////////////////////
// CVirusAlert::CVirusAlert()
//
// Purpose:
//    Generic initialization of the CVirusAlert class
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
CVirusAlert::CVirusAlert()
{
                                        // This Type guy needs to be set before
                                        // NAV will recognize it.
   SetPacketType(ALERT_TYPE_CVIRUSALERT);
}

/////////////////////////////////////////////////////////////////////
// CVirusAlert::CVirusAlert(
//
// Purpose:
//    Creation of a CVirusAlert class and assign an external buffer at
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
CVirusAlert::CVirusAlert(
   LPBYTE lpBuffer) :                   // [in] - Buffer to assign.
   CAlert (lpBuffer)                    // Pass to the base class.
{
                                        // This Type guy needs to be set before
                                        // NAV will recognize it.
   SetPacketType(ALERT_TYPE_CVIRUSALERT);
}


/////////////////////////////////////////////////////////////////////
// CVirusAlert::CVirusAlert(
//
// Purpose:
//    Allows us to construct one object that is a copy of another.
//
// Parameters:
//    CVirusAlert & pAlert - [in] - Object to take a copy of.
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
CVirusAlert::CVirusAlert(
   CVirusAlert & lpcVirusAlert) :       // [in] - Object to take a copy of.
   CAlert (lpcVirusAlert)               // Pass to the base class.
{
                                        // This Type guy needs to be set before
                                        // NAV will recognize it.
   SetPacketType(ALERT_TYPE_CVIRUSALERT);
}





/////////////////////////////////////////////////////////////////////
// CVirusAlert& CVirusAlert::operator = (
//
// Purpose:
//    This allows someone to use the = assignment operator to copy
//    the contents of one object to another.
//
// Parameters:
//    CVirusAlert& pRightHandObject - [in] - Object to copy
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
CVirusAlert& CVirusAlert::operator = (
   CVirusAlert& pRightHandObject)       // [in] - Object to copy
{
                                        // This Type guy needs to be set before
                                        // NAV will recognize it.
   SetPacketType(ALERT_TYPE_CVIRUSALERT);

   return *this;
}


/////////////////////////////////////////////////////////////////////
// CVirusAlert::~CVirusAlert()
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
CVirusAlert::~CVirusAlert()
{
}

