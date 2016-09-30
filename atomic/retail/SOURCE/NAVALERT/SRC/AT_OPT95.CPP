/////////////////////////////////////////////////////////////////////
// AT_OPTS9.CPP - Created - 06-13-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/AT_OPT95.CPv   1.1   01 Jul 1997 13:59:28   DBUCHES  $
//
// Description:
//    This file will contain a couple of functions that the 95
//    product can use to interface our CAlertOptions without having
//    to use a CAlertOptions class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/AT_OPT95.CPv  $
// 
//    Rev 1.1   01 Jul 1997 13:59:28   DBUCHES
// Fixed SetForwardTarget().  We now set the default protocol to
//  "ncacn_ip_tcp".
// 
// 
//
//    Rev 1.0   13 Jun 1997 22:08:54   JBRENNA
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "Platform.h"

#include "AT_Opts.h"                    // Main header file for options.

#include "at_opt95.h"                   // For this file.


/////////////////////////////////////////////////////////////////////
// This set of functions will compile either UNICODE or NON-UNICODE
// depending on the definition.


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI GetNLMTarget(
//
// Purpose:
//    This function gets only the top NLM Target from the NavAlert.Dat file.
//
// Parameters:
//    LPTSTR   lpszNLMTarget - [out] - Buffer to hold the target
//    BOOL    *bEnabled      - [out] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was retrieved successfully.
//    FALSE - Probably unable to read the NAVALERT.DAT file or
//          - something like that.
//
// Notes:
//    If TRUE is returned and there is no content in the lpszNLMTarget,
//    there was none set in the NAVALERT.DAT file.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI GetNLMTarget(
   LPTSTR   lpszNLMTarget,              // [out] - Buffer to hold the target
   BOOL    *bEnabled)                   // [out] - Is the target enabled?
{
CAlertOptions  cAlertOptions;
CNLMTarget    *lpcNLMTarget;
BOOL           bReturn = FALSE;

                                        // Start by trying to load the alert
                                        // options.
   if (!cAlertOptions.LoadOptions())
   {
      goto Cleanup;
   }

                                        // Get the next FORWARD target.
   lpcNLMTarget = (CNLMTarget *) cAlertOptions.GetNextTarget(
                                 TARGET_NLM);

   if (NULL != lpcNLMTarget)
   {
                                        // Get the information out of the
                                        // Target.
      *bEnabled = lpcNLMTarget->GetActive();

                                        // Now copy in the address from the
                                        // class.
      STRCPY (
         lpszNLMTarget,
         lpcNLMTarget->GetAddress());
   }
   else
   {
                                        // At this point, we know that we don't
                                        // have a target specified.  We now
                                        // know that we need to return valid
                                        // but BLANK information
      *lpszNLMTarget = NULL;
      *bEnabled = FALSE;
   }

                                        // Indicate our success status.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI SetNLMTarget(
//
// Purpose:
//    This function sets only the top NLM Target from the NavAlert.Dat file.
//
// Parameters:
//    LPCTSTR  lpszNLMTarget - [in] - New NLM Target
//    BOOL     bEnabled      - [in] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was set successfully.
//    FALSE - Probably unable to write to the NAVALERT.DAT file or
//          - something like that.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI SetNLMTarget(
   LPCTSTR  lpszNLMTarget,              // [in] - New NLM Target
   BOOL     bEnabled)                   // [in] - Is the target enabled?
{
CAlertOptions  cAlertOptions;
CNLMTarget    *lpcNLMTarget = NULL;
BOOL           bReturn = FALSE;

                                        // Start by trying to load the alert
                                        // options.
   if (!cAlertOptions.LoadOptions())
   {
      goto Cleanup;
   }

                                        // Get the next FORWARD target.
   lpcNLMTarget = (CNLMTarget *) cAlertOptions.RemoveNextTarget(
                                    TARGET_NLM);


                                        // If we didn't get a target, we should
                                        // create one.
   if (NULL == lpcNLMTarget)
   {
      lpcNLMTarget = new CNLMTarget;

                                        // Were we able to create our new
                                        // target?
      if (NULL == lpcNLMTarget)
      {
         goto Cleanup;
      }

   }


                                        // Now set up our new target

   lpcNLMTarget->SetActive(
      bEnabled);

                                        // If we can't set the Address, this is
                                        // a failure state.
   if (!lpcNLMTarget->SetAddress(
         lpszNLMTarget))
   {
      goto Cleanup;
   }

                                        // Once the information has been put
                                        // into our Target, it is time to put
                                        // it back in the list.
   cAlertOptions.AddTarget(
      lpcNLMTarget);

   cAlertOptions.SaveOptions();

                                        // Indicate our success status.
   bReturn = TRUE;

Cleanup:

                                        // If we weren't successful, we need to
                                        // make sure we delete our object when
                                        // we are done.
   if (!bReturn && (NULL != lpcNLMTarget))
   {
      delete lpcNLMTarget;
      lpcNLMTarget = NULL;
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI GetForwardTarget(
//
// Purpose:
//    This function gets only the top Forward Target from the NavAlert.Dat.
//
// Parameters:
//    LPTSTR   lpszForwardTarget - [out] - Buffer to hold the target
//    BOOL    *bEnabled          - [out] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was retrieved successfully.
//    FALSE - Probably unable to read the NAVALERT.DAT file or
//          - something like that.
//
// Notes:
//    If TRUE is returned and there is no content in the lpszForwardTarget,
//    there was none set in the NAVALERT.DAT file.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI GetForwardTarget(
   LPTSTR   lpszForwardTarget,          // [out] - Buffer to hold the target
   BOOL    *bEnabled)                   // [out] - Is the target enabled?
{
CAlertOptions      cAlertOptions;
CForwardTarget    *lpcForwardTarget;
BOOL               bReturn = FALSE;

                                        // Start by trying to load the alert
                                        // options.
   if (!cAlertOptions.LoadOptions())
   {
      goto Cleanup;
   }

                                        // Get the next FORWARD target.
   lpcForwardTarget = (CForwardTarget *) cAlertOptions.GetNextTarget(
                                     TARGET_FORWARD);

   if (NULL != lpcForwardTarget)
   {
                                        // Get the information out of the
                                        // Target.
      *bEnabled = lpcForwardTarget->GetActive();

                                        // Now copy in the address from the
                                        // class.
      STRCPY (
         lpszForwardTarget,
         lpcForwardTarget->GetAddress());

                                        // Note: we are ignoring the Protocol
                                        // field in the CForwardTarget.  This
                                        // will always default to any.

   }
   else
   {
                                        // At this point, we know that we don't
                                        // have a target specified.  We now
                                        // know that we need to return valid
                                        // but BLANK information
      *lpszForwardTarget = NULL;
      *bEnabled = FALSE;
   }

                                        // Indicate our success status.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI SetForwardTarget(
//
// Purpose:
//    This function sets only the top Forward Target from the NavAlert.Dat
//    file.  This is the ANSI Version of this function.
//
// Parameters:
//    LPCTSTR  lpszForwardTarget [in] - NewForward Target
//    BOOL     bEnabled          [in] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was set successfully.
//    FALSE - Probably unable to write to the NAVALERT.DAT file or
//          - something like that.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI SetForwardTarget(
   LPCTSTR  lpszForwardTarget,          // [in] - NewForward Target
   BOOL     bEnabled)                   // [in] - Is the target enabled?
{
CAlertOptions   cAlertOptions;
CForwardTarget *lpcForwardTarget = NULL;
BOOL            bReturn = FALSE;

                                        // Start by trying to load the alert
                                        // options.
   if (!cAlertOptions.LoadOptions())
   {
      goto Cleanup;
   }

                                        // Get the next FORWARD target.
   lpcForwardTarget = (CForwardTarget *) cAlertOptions.RemoveNextTarget(
                                            TARGET_FORWARD);


                                        // If we didn't get a target, we should
                                        // create one.
   if (NULL == lpcForwardTarget)
   {
      lpcForwardTarget = new CForwardTarget;

                                        // Were we able to create our new
                                        // target?
      if (NULL == lpcForwardTarget)
      {
         goto Cleanup;
      }

   }


                                        // Now set up our new target

   lpcForwardTarget->SetActive(
      bEnabled);

                                        // Set up default protocol.
                                        // IPE - do not translate!
   lpcForwardTarget->SetProtocol( _T("ncacn_ip_tcp") );

                                        // If we can't set the Address, this is
                                        // a failure state.
   if (!lpcForwardTarget->SetAddress(
         lpszForwardTarget))
   {
      goto Cleanup;
   }

                                        // Once the information has been put
                                        // into our Target, it is time to put
                                        // it back in the list.
   cAlertOptions.AddTarget(
      lpcForwardTarget);

   cAlertOptions.SaveOptions();

                                        // Indicate our success status.
   bReturn = TRUE;

Cleanup:

                                        // If we weren't successful, we need to
                                        // make sure we delete our object when
                                        // we are done.
   if (!bReturn && (NULL != lpcForwardTarget))
   {
      delete lpcForwardTarget;
      lpcForwardTarget = NULL;
   }

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// The following set of functions are ANSI version that will be
// provided if this lib is compiled in UNICODE.

#ifdef UNICODE


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI GetNLMTarget(
//
// Purpose:
//    This function gets only the top NLM Target from the NavAlert.Dat file
//    and returns it as an ANSI string.
//
// Parameters:
//    LPSTR    lpszNLMTarget - [out] - Buffer to hold the target
//    BOOL    *bEnabled      - [out] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was retrieved successfully.
//    FALSE - Probably unable to read the NAVALERT.DAT file or
//          - something like that.
//
// Notes:
//    If TRUE is returned and there is no content in the lpszNLMTarget,
//    there was none set in the NAVALERT.DAT file.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI GetNLMTarget(
   LPSTR    lpszNLMTarget,              // [out] - Buffer to hold the target
   BOOL    *bEnabled)                   // [out] - Is the target enabled?
{
WCHAR  szStringW[SYM_MAX_PATH];
BOOL   bReturn = FALSE;

                                        // Start by initializing the variable
                                        // that was sent to us
   *lpszNLMTarget = NULL;

                                        // Get the UNICODE version
   bReturn = GetNLMTarget(
                szStringW,
                bEnabled);

                                        // If successful, convert it and return
                                        // it.
   if (bReturn)
   {

                                        // Convert the string.  Don't check the
                                        // return value because the string may
                                        // be an EMPTY string.
      WideCharToMultiByte (
         CP_ACP,
         WC_COMPOSITECHECK,
         szStringW,
         -1,
         lpszNLMTarget,
         sizeof (char) * SYM_MAX_PATH,
         NULL,
         NULL );

   }
                                        // Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI SetNLMTarget(
//
// Purpose:
//    This function sets only the top NLM Target from the NavAlert.Dat file.
//    This is the ANSI Version of this function.
//
// Parameters:
//    LPCSTR   lpszNLMTarget - [in] - New NLM Target
//    BOOL     bEnabled      - [in] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was set successfully.
//    FALSE - Probably unable to write to the NAVALERT.DAT file or
//          - something like that.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI SetNLMTarget(
   LPCSTR   lpszNLMTarget,              // [in] - New NLM Target
   BOOL     bEnabled)                   // [in] - Is the target enabled?
{
LPWSTR lpszStringW = NULL;
DWORD  dwStringLength;
BOOL   bReturn = FALSE;

                                        // Don't forget to add one for the NULL
                                        // terminator
   dwStringLength = strlen (lpszNLMTarget) + 1;

                                        // Allocate a new string
   lpszStringW = new WCHAR[dwStringLength];

                                        // Convert the string to UNICODE
   MultiByteToWideChar (
      CP_ACP,
      MB_PRECOMPOSED,
      lpszNLMTarget,
      -1,
      lpszStringW,
      dwStringLength );

   bReturn = SetNLMTarget(
                lpszStringW,
                bEnabled);

                                        // Cleanup:
   if (NULL != lpszStringW)
   {
      delete lpszStringW;
      lpszStringW = NULL;
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI GetForwardTarget(
//
// Purpose:
//    This function gets only the top Forward Target from the NavAlert.Dat
//    file and returns it as an ANSI string.
//
// Parameters:
//    LPSTR    lpszForwardTarget - [out] - Buffer to hold the target
//    BOOL    *bEnabled          - [out] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was retrieved successfully.
//    FALSE - Probably unable to read the NAVALERT.DAT file or
//          - something like that.
//
// Notes:
//    If TRUE is returned and there is no content in the lpszForwardTarget,
//    there was none set in the NAVALERT.DAT file.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI GetForwardTarget(
   LPSTR    lpszForwardTarget,          // [out] - Buffer to hold the target
   BOOL    *bEnabled)                   // [out] - Is the target enabled?
{
WCHAR  szStringW[SYM_MAX_PATH];
BOOL   bReturn = FALSE;

                                        // Start by initializing the variable
                                        // that was sent to us
   *lpszForwardTarget = NULL;

                                        // Get the UNICODE version
   bReturn = GetForwardTarget(
                szStringW,
                bEnabled);

                                        // If successful, convert it and return
                                        // it.
   if (bReturn)
   {

                                        // Convert the string.  Don't check the
                                        // return value because the string may
                                        // be an EMPTY string.
      WideCharToMultiByte (
         CP_ACP,
         WC_COMPOSITECHECK,
         szStringW,
         -1,
         lpszForwardTarget,
         sizeof (char) * SYM_MAX_PATH,
         NULL,
         NULL );

   }


                                        // Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI SetForwardTarget(
//
// Purpose:
//    This function sets only the top Forward Target from the NavAlert.Dat
//    file.  This is the ANSI Version of this function.
//
// Parameters:
//    LPCSTR   lpszForwardTarget - [in] - NewForward Target
//    BOOL     bEnabled          - [in] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was set successfully.
//    FALSE - Probably unable to write to the NAVALERT.DAT file or
//          - something like that.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI SetForwardTarget(
   LPCSTR   lpszForwardTarget,          // [in] - NewForward Target
   BOOL     bEnabled)                   // [in] - Is the target enabled?
{
LPWSTR lpszStringW = NULL;
DWORD  dwStringLength;
BOOL   bReturn = FALSE;

                                        // Don't forget to add one for the NULL
                                        // terminator
   dwStringLength = strlen (lpszForwardTarget) + 1;

                                        // Allocate a new string
   lpszStringW = new WCHAR[dwStringLength];

                                        // Convert the string to UNICODE
   MultiByteToWideChar (
      CP_ACP,
      MB_PRECOMPOSED,
      lpszForwardTarget,
      -1,
      lpszStringW,
      dwStringLength );

   bReturn = SetForwardTarget(
                lpszStringW,
                bEnabled);

                                        // Cleanup:

   if (NULL != lpszStringW)
   {
      delete lpszStringW;
      lpszStringW = NULL;
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// The following functions are provided to allow people to access
// UNICODE versions of these functions when they don't have UNICODE
// defined.

#else // #ifdef UNICODE

/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI GetNLMTarget(
//
// Purpose:
//    This function gets only the top NLM Target from the NavAlert.Dat file
//    and returns it as a UNICODE string.
//
// Parameters:
//    LPWSTR   lpszNLMTarget - [out] - Buffer to hold the target
//    BOOL    *bEnabled      - [out] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was retrieved successfully.
//    FALSE - Probably unable to read the NAVALERT.DAT file or
//          - something like that.
//
// Notes:
//    If TRUE is returned and there is no content in the lpszNLMTarget,
//    there was none set in the NAVALERT.DAT file.
//
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI GetNLMTarget(
   LPWSTR   lpszNLMTarget,              // [out] - Buffer to hold the target
   BOOL    *bEnabled)                   // [out] - Is the target enabled?
{
char   szString[SYM_MAX_PATH];
BOOL   bReturn = FALSE;

                                        // Start by initializing the variable
                                        // that was sent to us
   *lpszNLMTarget = NULL;

                                        // Get the UNICODE version
   bReturn = GetNLMTarget(
                szString,
                bEnabled);

                                        // If successful, convert it and return
                                        // it.
   if (bReturn)
   {

                                        // Convert the string to UNICODE
      MultiByteToWideChar (
         CP_ACP,
         MB_PRECOMPOSED,
         szString,
         -1,
         lpszNLMTarget,
         SYM_MAX_PATH );

   }

                                        // Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI SetNLMTarget(
//
// Purpose:
//    This function sets only the top NLM Target from the NavAlert.Dat file.
//    This is the UNICODE Version of this function.
//
// Parameters:
//    LPCWSTR  lpszNLMTarget - [in] - New NLM Target
//    BOOL     bEnabled      - [in] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was set successfully.
//    FALSE - Probably unable to write to the NAVALERT.DAT file or
//          - something like that.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI SetNLMTarget(
   LPCWSTR  lpszNLMTarget,              // [in] - New NLM Target
   BOOL     bEnabled)                   // [in] - Is the target enabled?
{
LPSTR  lpszString = NULL;
DWORD  dwStringLength;
BOOL   bReturn = FALSE;

                                        // Don't forget to add one for the NULL
                                        // terminator
   dwStringLength = wcslen (lpszNLMTarget) + 1;

                                        // Allocate a new string
   lpszString = new char[dwStringLength];

                                        // Convert to ANSI
   WideCharToMultiByte (
      CP_ACP,
      WC_COMPOSITECHECK,
      lpszNLMTarget,
      -1,
      lpszString,
      sizeof (char) * dwStringLength,
      NULL,
      NULL );

   bReturn = SetNLMTarget(
                lpszString,
                bEnabled);

                                        // Cleanup:

   if (NULL != lpszString)
   {
      delete lpszString;
      lpszString = NULL;
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI GetForwardTarget(
//
// Purpose:
//    This function gets only the top Forward Target from the NavAlert.Dat
//    file and returns it as a UNICODE string.
//
// Parameters:
//    LPWSTR   lpszForwardTarget - [out] - Buffer to hold the target
//    BOOL    *bEnabled          - [out] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was retrieved successfully.
//    FALSE - Probably unable to read the NAVALERT.DAT file or
//          - something like that.
//
// Notes:
//    If TRUE is returned and there is no content in the lpszForwardTarget,
//    there was none set in the NAVALERT.DAT file.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI GetForwardTarget(
   LPWSTR   lpszForwardTarget,          // [out] - Buffer to hold the target
   BOOL    *bEnabled)                   // [out] - Is the target enabled?
{
char   szString[SYM_MAX_PATH];
BOOL   bReturn = FALSE;

                                        // Start by initializing the variable
                                        // that was sent to us
   *lpszForwardTarget = NULL;

                                        // Get the UNICODE version
   bReturn = GetForwardTarget(
                szString,
                bEnabled);

                                        // If successful, convert it and return
                                        // it.
   if (bReturn)
   {

                                        // Convert the string to UNICODE
      MultiByteToWideChar (
         CP_ACP,
         MB_PRECOMPOSED,
         szString,
         -1,
         lpszForwardTarget,
         SYM_MAX_PATH );

   }

                                        // Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI SetForwardTarget(
//
// Purpose:
//    This function sets only the top Forward Target from the NavAlert.Dat
//    file.  This is the UNICODE Version of this function.
//
// Parameters:
//    LPCWSTR  lpszForwardTarget - [in] - NewForward Target
//    BOOL     bEnabled          - [in] - Is the target enabled?
//
// Returns:
//    TRUE  - Information was set successfully.
//    FALSE - Probably unable to write to the NAVALERT.DAT file or
//          - something like that.
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI SetForwardTarget(
   LPCWSTR  lpszForwardTarget,          // [in] - NewForward Target
   BOOL     bEnabled)                   // [in] - Is the target enabled?
{
LPSTR  lpszString = NULL;
DWORD  dwStringLength;
BOOL   bReturn = FALSE;

                                        // Don't forget to add one for the NULL
                                        // terminator
   dwStringLength = wcslen (lpszForwardTarget) + 1;

                                        // Allocate a new string
   lpszString = new char[dwStringLength];

                                        // Convert to ANSI
   WideCharToMultiByte (
      CP_ACP,
      WC_COMPOSITECHECK,
      lpszForwardTarget,
      -1,
      lpszString,
      sizeof (char) * dwStringLength,
      NULL,
      NULL );

   bReturn = SetForwardTarget(
                lpszString,
                bEnabled);

                                        // Cleanup:

   if (NULL != lpszString)
   {
      delete lpszString;
      lpszString = NULL;
   }

   return bReturn;
}

#endif // #ifdef UNICODE


