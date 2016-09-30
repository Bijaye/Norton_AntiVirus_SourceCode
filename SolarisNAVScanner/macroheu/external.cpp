//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/EXTERNAL.CPv   1.0   30 Jun 1997 16:15:28   DCHI  $
//
// Description:
//  Macro emulation environment external call emulation functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/EXTERNAL.CPv  $
// 
//    Rev 1.0   30 Jun 1997 16:15:28   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"

//*************************************************************************
//
// BOOL ExternalCallSetup()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//  lpbyMacroName       Module name
//  lpbyFunctionName    Function name
//  lpbySubName         Subroutine name
//
// Description:
//  Sets up for a call to an external routine.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL ExternalCallSetup
(
    LPENV           lpstEnv,
    LPBYTE          lpbyMacroName,
    LPBYTE          lpbyFunctionName,
    LPBYTE          lpbySubName
)
{
    WORD            wIndex;
    DWORD           dwOffset;
    DWORD           dwSize;
    BYTE            byEncryptByte;
    BOOL            bActive;
    BOOL            bResult;
    LPMODULE_ENV    lpstModEnv;

    // Get the macro information for the macro with the given name

    if (WD7GetMacroInfoAtULName(&lpstEnv->stTDTInfo,
                                lpbyMacroName,
                                &wIndex,
                                &dwOffset,
                                &dwSize,
                                &byEncryptByte,
                                &bActive) != WD7_STATUS_OK ||
        bActive == FALSE)
    {
        return(FALSE);
    }

    // Can't allow recursive macros

    lpstModEnv = lpstEnv->lpstModEnv;
    while (lpstModEnv != NULL)
    {
        if (lpstModEnv->wMacroIndex == wIndex)
            return(FALSE);

        lpstModEnv = lpstModEnv->lpstPrev;
    }

    // Create the environment for the module

    lpstModEnv = ModuleEnvCreate(lpstEnv,
                                 lpstEnv->lpstKey,
                                 wIndex,
                                 dwSize,
                                 dwOffset,
                                 byEncryptByte);

    if (lpstModEnv == NULL)
        return(FALSE);

    if (ModuleScan(lpstModEnv) == FALSE)
    {
        ModuleEnvDestroy(lpstEnv,lpstModEnv);
        return(FALSE);
    }

    // Need to do the chaining now because the calls below require it

    lpstModEnv->lpstPrev = lpstEnv->lpstModEnv;
    lpstEnv->lpstModEnv = lpstModEnv;

    // Get the IP

    bResult = TRUE;
    if (lpbyFunctionName == NULL)
    {
        // Sub

        if (lpbySubName == NULL)
            lpstEnv->dwNextIP = lpstModEnv->dwMainIP;
        else
        if (ModuleEnvGetSubIP(lpstEnv,
                              *lpbySubName,
                              lpbySubName + 1,
                              &lpstEnv->dwNextIP) == FALSE)
            bResult = FALSE;
    }
    else
    {
        // Function

        if (ModuleEnvGetFunctionIP(lpstEnv,
                                   *lpbyFunctionName,
                                   lpbyFunctionName + 1,
                                   &lpstEnv->dwNextIP) == FALSE)
            bResult = FALSE;
    }

    // Create the global variable frame for the module

    if (bResult != FALSE &&
        VarPushGlobalFrame(lpstEnv) == FALSE)
    {
        bResult = FALSE;
        ModuleEnvDestroy(lpstEnv,lpstModEnv);
        return(FALSE);
    }

    if (bResult == FALSE)
    {
        lpstEnv->lpstModEnv = lpstModEnv->lpstPrev;
        ModuleEnvDestroy(lpstEnv,lpstModEnv);
        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL ExternalCallCleanup()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//
// Description:
//  Cleans up after a call to an external routine.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL ExternalCallCleanup
(
    LPENV       	lpstEnv
)
{
	LPMODULE_ENV	lpstModEnv;

    if (VarPopGlobalFrame(lpstEnv) == FALSE)
    {
        return(FALSE);
    }

    lpstModEnv = lpstEnv->lpstModEnv->lpstPrev;

    assert(lpstModEnv != NULL);

    ModuleEnvDestroy(lpstEnv,lpstEnv->lpstModEnv);

    lpstEnv->lpstModEnv = lpstModEnv;

    return(TRUE);
}

