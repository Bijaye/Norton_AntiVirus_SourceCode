//////////////////////////////////////////////////////////////////////////
// Copyright 1999 Symantec Corporation
//////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/N32userL/VCS/N32userL.CPv   1.1   06 Aug 1997 21:19:28   bsobel  $
//
// Description:
//
//     Library for determining N32userL codes stored in the registry (or elsewhere)
//     as a mechanism for an ad-hok copy protection
// Contains:
//
// See Also:
//
//////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32userL/VCS/N32userL.CPv  $
//

#ifndef N32USERL_H
#define N32USERL_H

#include <msi.h>
#include <msiquery.h>
#include "passkey.h"

// Note: For the trial version of this DLL be sure you define "TRIAL_BUILD"!

#if defined(__cplusplus)
extern "C" {
#endif

#define	N32User_Time_Delay_Constant		3600
#define	N32User_Time_Delay_Key			"Software\\Symantec\\Norton AntiVirus"
#define	N32User_Time_Delay_ValueName	"Last Prompt"

    enum N32UserState
    {
        N32UserState_TRIAL,
        N32UserState_EXPIRED,
        N32UserState_PURCHASED
    };  


    // installs the DLL onto the machine (from source)
    UINT __stdcall N32User_Install(MSIHANDLE hInstall);


    // uninstalls the DLL onto the machine (from source)
    UINT __stdcall N32User_UnInstall(MSIHANDLE hInstall);


    // returns the current state of the product
    BOOL WINAPI N32User_GetUserState(PASSKEYCOOKIE * pCookie,UINT * pState);

    // Same as N32User_GetUserState, but shows the VBox UI only if the trial
    // period has expired.
    BOOL WINAPI N32User_GetUserStateSilent(PASSKEYCOOKIE * pCookie,UINT * pState);

    // Same as N32User_GetUserStateSilent, but never shows the VBox UI.  Useful
    // for services.
    BOOL WINAPI N32User_GetUserStateSilent2(PASSKEYCOOKIE * pCookie,UINT * pState);

    // returns the number of days the product has been in trial mode
    BOOL WINAPI N32User_GetTrialDaysUsed(DWORD* pdwDaysUsed);

    // returns the number of days the trial is allowed
    BOOL WINAPI N32User_GetTrialDaysAllowed(DWORD* pdwDaysAllowed);

    // returns the user's registration number
    // the buffer should be at least 17 TCHARs long
    BOOL WINAPI N32User_GetRegNum(LPTSTR szBuffer, DWORD dwBufferSize);

#if defined(__cplusplus)
}
#endif


#endif

