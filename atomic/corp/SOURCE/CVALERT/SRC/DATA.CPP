/////////////////////////////////////////////////////////////////////
// DATA.CPP - Created - 05-15-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CVALERT/VCS/data.cpv   1.1   16 May 1997 15:21:08   DSACKIN  $
//
// Description:
//    This file is where we will write the functions for the
//    CVirusAlert class to access the data in CAlert.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CVALERT/VCS/data.cpv  $
// 
//    Rev 1.1   16 May 1997 15:21:08   DSACKIN
// Added the AlertType functions.
//
//    Rev 1.0   16 May 1997 13:29:56   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include <assert.h>
#include "CValert.h"


/////////////////////////////////////////////////////////////////////
// DWORD value functions

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetAction(
//
// Purpose:
//
//
// Parameters:
//    DWORD dwAction - [in] - Action taken
//
// Returns:
//    TRUE  - Actions set successfully
//    FALSE - Packet locked or subscript out of range.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetAction(
   DWORD dwAction)                      // [in] - Action taken
{
BOOL bReturn;

   bReturn = SetDWord(
                DWORD_ACTION,
                dwAction);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetAction(
//
// Purpose:
//
//
// Parameters:
//    DWORD & dwAction - [out] - Action taken
//
// Returns:
//    TRUE  - Action retrieved successfully
//    FALSE - Possibly an invalid packet?
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetAction(
   DWORD & dwAction)                    // [out] - Action taken
{
BOOL bReturn;

   bReturn = GetDWord(
                DWORD_ACTION,
                dwAction);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetAlertType(
//
// Purpose:
//
//
// Parameters:
//    DWORD dwAlertType - [in] - Alert type to set
//
// Returns:
//    TRUE  - AlertType set successfully
//    FALSE - Packet locked or subscript out of range.
//
/////////////////////////////////////////////////////////////////////
// 05-16-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetAlertType(
   DWORD dwAlertType)                   // [in] - Alert type to set
{
BOOL bReturn;

   bReturn = SetDWord(
                DWORD_ALERT_TYPE,
                dwAlertType);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetAlertType(
//
// Purpose:
//
//
// Parameters:
//    DWORD & dwAlertType - [out] - Alert type from packet
//
// Returns:
//    TRUE  - AlertType retrieved successfully
//    FALSE - Possibly an invalid packet?
//
/////////////////////////////////////////////////////////////////////
// 05-16-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetAlertType(
   DWORD & dwAlertType)                 // [out] - Alert type from packet
{
BOOL bReturn;

   bReturn = GetDWord(
                DWORD_ALERT_TYPE,
                dwAlertType);

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// BOOL Value functions

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetActionSuccess(
//
// Purpose:
//
//
// Parameters:
//    BOOL bActionSuccess - [in] - Was the action successful
//
// Returns:
//    TRUE  - ActionSuccess was set successfully.
//    FALSE - Probably the packet was locked.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetActionSuccess(
   BOOL bActionSuccess)                 // [in] - Was the action successful
{
BOOL bReturn;

   bReturn = SetBool(
                BOOL_ACTION_SUCCESS,
                bActionSuccess);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetActionSuccess(
//
// Purpose:
//
//
// Parameters:
//    BOOL & bActionSuccess - [out] - Was the action successful
//
// Returns:
//    TRUE  - Retrieved the ActionSuccess flag successfully.
//    FALSE - Couldn't retrieve the flag.  Possibly bas subscript?
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetActionSuccess(
   BOOL & bActionSuccess)               // [out] - Was the action successful
{
BOOL bReturn;

   bReturn = GetBool(
                BOOL_ACTION_SUCCESS,
                bActionSuccess);

   return bReturn;
}



