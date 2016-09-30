//************************************************************************
// Copyright 1992-1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVSECUR/VCS/SECINIT.CPv   1.2   10 Aug 1997 19:39:06   DBUCHES  $
//
// Description:
//      This file contain functions that are used for password verification
//      in NAVW
//
// Contains (as exports):
//             PasswordCheck()// Ask for password.
//             AskPassword()  // Determines if password should be asked
//             DestroyPasswordWindow() //
//             RemoveProtection()
//
// See Also:
//      NavWMCMD.cpp, all the NAVOPTS .CPP files
//************************************************************************
// $Log:   S:/NAVSECUR/VCS/SECINIT.CPv  $
// 
//    Rev 1.2   10 Aug 1997 19:39:06   DBUCHES
// Now, shared data is alloced in a new segment.  .navsec.  This was neccessary to fix problems with DEC Alpha platforms.
//
//    Rev 1.1   16 Apr 1997 15:11:44   JBRENNA
// Port from Telsa: ksachin: Changed the help button ID for the Verify
// password dialog to be the curent one that is in our help file.
//
//    Rev 1.0   06 Feb 1997 20:59:48   RFULLER
// Initial revision
//
//    Rev 1.0   30 Jan 1996 15:54:12   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:39:50   BARRY
// Initial revision.
//
//    Rev 1.2   21 Sep 1995 16:53:16   YVANG
// Modified the functions, 'PasswordCheck()' and 'AskPassword()' to check
// the 'PASSWORD_VIEWSTARTUPOPT' case.  If it is the case, the protect
// feature value is read from the 'startup' section in the 'NavOptions'
// structure.
//
//
//    Rev 1.1   16 Nov 1994 18:43:08   MFALLEN
// Shared data
//
//    Rev 1.0   19 Apr 1994 19:29:02   MFALLEN
// Initial revision.
//
//    Rev 1.38   06 Apr 1994 18:15:36   DALLEE
// Added File Monitor options to list of control panels with protection.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "ngui.h"
#include "navutil.h"
#include "symcfg.h"
#include "options.h"
#include "security.h"
#include "navw_hlp.h"

#ifdef SYM_WIN32
    #pragma data_seg(".navsec")
#endif

extern   HINSTANCE   hInstance;
static   UINT        CPL_Passed        = FALSE;
static   BOOL        CPL_AskedPassword = FALSE;
static   HWND        hPasswordWnd;

#ifdef SYM_WIN32
    #pragma data_seg()
#endif

//************************************************************************
// PasswordCheck()
//
// This procedure calls the Quake PasswordVerify() function. As the name
// indicates, this functions is used to verify a password.
//
// Parameters:
//      LPCHECKSTRUCT           [in] pointer to CHECHSTRUCT
//      bUseKb                  [in] detect keyboard activity TRUE/FALSE
//      bUseModeless            [in] use modal or modeless dialog boxes
//
// Returns:    If modeless dialog box then one the following is returned
//             in the hModeless CHECKSTRUCT member.
//
//             PASSWORDMODELESS_VALID : passwords match
//             PASSWORDMODELESS_INVALID : passwords do not match
//             PASSWORDMODELESS_DESTROYES : dlg box was destroyed by parent
//
//             If modal dlg box was requested:
//
//             TRUE:    passwords match
//             FALSE:   passwords don't match
//************************************************************************
// 5/20/93 Martin Function Created
//************************************************************************

UINT SYM_EXPORT WINAPI PasswordCheck (LPCHECKSTRUCT lpPwCheck, BOOL bUseKb,
                                  BOOL bUseModeless)
{
    auto     LPPASSWORDSTRUCT lpPass;
    auto     UINT           uPassed = FALSE;
    auto     BOOL           bAsk    = FALSE;
    auto     MSG            msg;

    if( CPL_Passed )
        return TRUE;

    lpPwCheck->wVirtKey   = 0;
    lpPwCheck->dwKeyFlags = 0;

    if (lpPwCheck->lpNavOptions->password.bProtect &&
       (lpPass = (LPPASSWORDSTRUCT)MemAllocPtr(GHND, sizeof(PASSWORDSTRUCT))))
       {
       if (lpPwCheck->uID == PASSWORD_VIEWSTARTUPOPT)
          bAsk = (lpPwCheck->uID == PASSWORD_ALWAYS) ? TRUE
                           : lpPwCheck->lpNavOptions->startup.bProtectFeature;
       else
          bAsk = (lpPwCheck->uID == PASSWORD_ALWAYS) ? TRUE
                           : lpPwCheck->lpNavOptions->password.bProtectFeature[lpPwCheck->uID];

       if (bAsk == TRUE || lpPwCheck->lpNavOptions->password.bMaxProtection)
          {
          lpPass->dwSize            = sizeof(PASSWORDSTRUCT);
          lpPass->hParent           = lpPwCheck->hWnd;
          lpPass->dwPassword        = lpPwCheck->lpNavOptions->password.dwPassword;
          lpPass->uMaxCharsToAccept = 16;
          lpPass->bCaseSensitive    = FALSE;
          lpPass->bOemConvert       = TRUE;
          lpPass->uHelpID           = IDH_NAVW_VERIFY_PASSWORD_DLG_HELP_BTN;

          if (bUseKb)
             lpPass->wVirtKey = TRUE;

          if (bUseModeless == TRUE)
             {
             lpPass->hModeless = (HWND)0xFFFF;

             PasswordVerify (lpPass);
             hPasswordWnd = lpPass->hModeless;

             while (GetMessage (&msg, NULL, 0, 0))
                {
                if (IsWindow(lpPass->hModeless))
                   {
                   if (!IsDialogMessage(lpPass->hModeless, &msg))
                      {
                      TranslateMessage (&msg);
                      DispatchMessage  (&msg);
                      }
                   }
                else
                   break;
                }

             if (lpPass->hModeless == (HWND)PASSWORDMODELESS_VALID)
                {
                uPassed = (UINT)PASSWORDMODELESS_VALID;
                CPL_Passed = TRUE;
                CPL_AskedPassword = TRUE;
                }
             else if (lpPass->hModeless == (HWND)PASSWORDMODELESS_INVALID)
                {
                uPassed = (UINT)PASSWORDMODELESS_INVALID;
                CPL_Passed = FALSE;
                }
             }
          else
             {
             lpPass->hModeless = NULL;
             uPassed = CPL_Passed = (UINT)PasswordVerify (lpPass);
             }

          lpPwCheck->wVirtKey   = lpPass->wVirtKey;
          lpPwCheck->dwKeyFlags = lpPass->dwKeyFlags;
          }
       else
          uPassed = TRUE;

       MemFreePtr(lpPass);
       }
    else
       uPassed = TRUE;


    return (uPassed);
}

//************************************************************************
// ResetPassword()
//
// This procedure resets the password state.
//
// Parameters:
//      None
//
// Returns:    Nothing
//***********************************************************************
//************************************************************************

VOID SYM_EXPORT WINAPI ResetPassword (VOID)
{
    CPL_AskedPassword = CPL_Passed = FALSE;
}


VOID SYM_EXPORT WINAPI RemoveProtection(VOID)
{
    CPL_AskedPassword = CPL_Passed = TRUE;
}


//************************************************************************
// AskPassword()
//
// This procedure calculates if password should be asked or not. It returns
// statuses for if password should be asked, if password was entered but
// verification failed, and if password was entered and verification was OK
//
// Parameters:
//      LPCHECKSTRUCT           [in] pointer to CHECHSTRUCT
//
// Returns:    If modeless dialog box then one the following is returned
//             PASSWORD_SHOWCONTROL : password entered and verified OK
//             PASSWORD_ASK : password was not successfully verified
//             0 : password was probably not even entered.
//***********************************************************************
// 5/20/93 Martin Function Created
//************************************************************************

WORD SYM_EXPORT WINAPI AskPassword (LPCHECKSTRUCT lpPwCheck)
{
    auto     BOOL     bProtected = FALSE;
    auto     WORD     wStatus = 0;

    if( lpPwCheck->lpNavOptions->password.bProtect &&
        lpPwCheck->lpNavOptions->password.dwPassword )
        {
        if ( lpPwCheck->lpNavOptions->password.bMaxProtection ||
             lpPwCheck->uID == PASSWORD_ALWAYS )
            {
            bProtected = TRUE;
            }
        else
            {
            bProtected = lpPwCheck->lpNavOptions->password.bProtectFeature[lpPwCheck->uID];
            }
        wStatus = bProtected ? PASSWORD_ASK : PASSWORD_SHOWCONTROL;
        }
    else
        wStatus = PASSWORD_SHOWCONTROL;

    return (wStatus);
}

//************************************************************************
// DestroyPasswordWindow()
//
// This procedure destroys the password verification dialog box.
//
// Parameters: none
//
// Returns:    none
//
//***********************************************************************
// 5/20/93 Martin Function Created
//************************************************************************

VOID SYM_EXPORT WINAPI DestroyPasswordWindow(VOID)
{
   if (IsWindow(hPasswordWnd))
      DestroyWindow(hPasswordWnd);
}

