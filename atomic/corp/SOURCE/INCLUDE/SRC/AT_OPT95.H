/////////////////////////////////////////////////////////////////////
// AT_OPT95.H - Created - 06-13-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/at_opt95.h_v   1.2   13 Jun 1997 11:57:34   DSACKIN  $
//
// Description:
//    This is where the exported functions will be defined for the
//    95 CAlertOptions interface.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/at_opt95.h_v  $
// 
//    Rev 1.2   13 Jun 1997 11:57:34   DSACKIN
// In strings are now marked as constant.
//
//    Rev 1.1   13 Jun 1997 11:50:28   DSACKIN
// Take a pointer to the BOOLs that we are returning instead of using a
// reference method.  (&).
//
//    Rev 1.0   13 Jun 1997 11:47:34   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__AT_OPT95_H)
#define __AT_OPT95_H


/////////////////////////////////////////////////////////////////////
// All of the functions in this header file will return a BOOL value
// that states if the function was successful or not.
//
// All strings that are to be returned to the calling application
// should be of SYM_MAX_PATH (261) in length.


BOOL SYM_EXPORT WINAPI GetNLMTarget(
   LPTSTR   lpszNLMTarget,              // [out] - Buffer to hold the target
   BOOL    *bEnabled);                  // [out] - Is the target enabled?

BOOL SYM_EXPORT WINAPI SetNLMTarget(
   LPCTSTR  lpszNLMTarget,              // [in] - New NLM Target
   BOOL     bEnabled);                  // [in] - Is the target enabled?

BOOL SYM_EXPORT WINAPI GetForwardTarget(
   LPTSTR   lpszForwardTarget,          // [out] - Buffer to hold the target
   BOOL    *bEnabled);                  // [out] - Is the target enabled?

BOOL SYM_EXPORT WINAPI SetForwardTarget(
   LPCTSTR  lpszForwardTarget,          // [in] - NewForward Target
   BOOL     bEnabled);                  // [in] - Is the target enabled?


/////////////////////////////////////////////////////////////////////
// Ok.  Now that we have the TCHAR versions, time to do the specific
// UNICODE/NON-UNICODE versions of both of these functions.

#ifdef UNICODE

BOOL SYM_EXPORT WINAPI GetNLMTarget(
   LPSTR    lpszNLMTarget,              // [out] - Buffer to hold the target
   BOOL    *bEnabled);                  // [out] - Is the target enabled?

BOOL SYM_EXPORT WINAPI SetNLMTarget(
   LPCSTR   lpszNLMTarget,              // [in] - New NLM Target
   BOOL     bEnabled);                  // [in] - Is the target enabled?

BOOL SYM_EXPORT WINAPI GetForwardTarget(
   LPSTR    lpszForwardTarget,          // [out] - Buffer to hold the target
   BOOL    *bEnabled);                  // [out] - Is the target enabled?

BOOL SYM_EXPORT WINAPI SetForwardTarget(
   LPCSTR   lpszForwardTarget,          // [in] - NewForward Target
   BOOL     bEnabled);                  // [in] - Is the target enabled?


/////////////////////////////////////////////////////////////////////
// OK.  Now if UNICODE is NOT defined, we need to allow access to the
// UNICODE functions anyway.


#else // #ifdef UNICODE

BOOL SYM_EXPORT WINAPI GetNLMTarget(
   LPWSTR   lpszNLMTarget,              // [out] - Buffer to hold the target
   BOOL    *bEnabled);                  // [out] - Is the target enabled?

BOOL SYM_EXPORT WINAPI SetNLMTarget(
   LPCWSTR  lpszNLMTarget,              // [in] - New NLM Target
   BOOL     bEnabled);                  // [in] - Is the target enabled?

BOOL SYM_EXPORT WINAPI GetForwardTarget(
   LPWSTR   lpszForwardTarget,          // [out] - Buffer to hold the target
   BOOL    *bEnabled);                  // [out] - Is the target enabled?

BOOL SYM_EXPORT WINAPI SetForwardTarget(
   LPCWSTR  lpszForwardTarget,          // [in] - NewForward Target
   BOOL     bEnabled);                  // [in] - Is the target enabled?

#endif // #ifdef UNICODE





#endif // if !defined(__AT_OPT95_H)
