//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97CALL.CPv   1.1   12 Oct 1998 13:18:58   DCHI  $
//
// Description:
//  Macro emulation environment call emulation functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97CALL.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:18:58   DCHI
// Modified O97CallSetup() to handle calls to functions.
// 
//    Rev 1.0   15 Aug 1997 13:28:52   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"

//*************************************************************************
//
// BOOL O97CallSetUpParameters()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//  bIsSub              TRUE if call is to a subroutine
//
// Description:
//  Sets up the local variables with the values passed to the function
//  or subroutine.  If the call is not to a subroutine, the function
//  also creates a local variable for the return value.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97CallSetUpParameters
(
    LPO97_ENV       lpstEnv,
    BOOL            bIsSub
)
{
    int             i;
    DWORD           dwParamRecordOffset;
    EO97_TYPE_T     eType;
    BYTE            byNumParams;
    WORD            wID;
    LPVOID          lpvData;
    LPO97_VAR       lpstVar;

    // Get the subroutine information

    if (O97TypeTableGetRoutineInfo(lpstEnv->lpstModEnv->lpstModule,
                                   lpstEnv->dwOpTypeOffset,
                                   NULL,
                                   &wID,
                                   &byNumParams,
                                   &dwParamRecordOffset) == TRUE)
    {
        if (bIsSub == FALSE)
        {
            long        lZero = 0;

            // It is a function, so create the variable for the
            //  return value

            lpstVar = O97VarAlloc(lpstEnv,FALSE,FALSE,wID);
            if (lpstVar == NULL)
                return(FALSE);

            // Initialize the variable to 0

            if (O97VarSet(lpstEnv,
                          lpstVar,
                          eO97_TYPE_LONG,
                          &lZero) == FALSE)
                return(FALSE);
        }

        // Now get the parameters

        i = 0;
        while (i < lpstEnv->nNumParams &&
            dwParamRecordOffset != 0xFFFFFFFF)
        {
            // Get param identifier and next parameter record offset

            if (O97TypeTableGetParamInfo(lpstEnv->lpstModEnv->lpstModule,
                                         dwParamRecordOffset,
                                         &wID,
                                         &dwParamRecordOffset) == FALSE)
                break;

            // Assign the value of the parameter to a local variable

            if (O97ParamGetValue(lpstEnv,i,&eType,&lpvData) == FALSE)
                break;

            lpstVar = O97VarAlloc(lpstEnv,FALSE,FALSE,wID);
            if (lpstVar == NULL)
                return(FALSE);

            if (O97VarSet(lpstEnv,lpstVar,eType,lpvData) == FALSE)
                return(FALSE);

            ++i;
        }

        return(TRUE);
    }

    return(FALSE);
}


//*************************************************************************
//
// BOOL O97CallSetup()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//  wFunctionID         ID if function or O97_ID_INVALID if not
//  wSubID              ID if subroutine or O97_ID_INVALID if not
//
// Description:
//  Sets up for a call to either a function or a subroutine.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97CallSetup
(
    LPO97_ENV           lpstEnv,
    WORD                wFunctionID,
    WORD                wSubID
)
{
    LPO97_CALL          lpstCall;
    WORD                wRoutineLine;
    WORD                wRoutineOffset;

    // Is it a function or a subroutine?

    if (wFunctionID != O97_ID_INVALID)
    {
        // Get the location of the function

        if (O97ModEnvGetFunctionIP(lpstEnv,
                                   wFunctionID,
                                   &wRoutineLine,
                                   &wRoutineOffset) == FALSE)
        {
            if (wSubID == O97_ID_INVALID)
                return(FALSE);

            // Get the location of the subroutine

            if (O97ModEnvGetSubIP(lpstEnv,
                                  wSubID,
                                  &wRoutineLine,
                                  &wRoutineOffset) == FALSE)
                return(FALSE);

        }

        // Need to prepare for a call

        if (wSubID == O97_ID_INVALID)
            O97StatePush(lpstEnv,esO97_CALL_FUNCTION);
        else
            O97StatePush(lpstEnv,esO97_CALL_SUB);
    }
    else
    {
        // Get the location of the subroutine

        if (O97ModEnvGetSubIP(lpstEnv,
                              wSubID,
                              &wRoutineLine,
                              &wRoutineOffset) == FALSE)
            return(FALSE);

        // Need to prepare for a call

        O97StatePush(lpstEnv,esO97_CALL_SUB);
    }

    if (O97ControlCreateControl(lpstEnv,ectO97_CALL) == FALSE)
        return(FALSE);

    lpstCall = (LPO97_CALL)O97ControlTop(lpstEnv);

    lpstCall->stReturnIP.wLine = (WORD)lpstEnv->dwIPLine;
    lpstCall->stReturnIP.wOffset = (WORD)lpstEnv->dwNextIPOffset;;

    // Swap the routine start IP

    lpstCall->stRoutineStartIP = lpstEnv->stRoutineStartIP;

    lpstEnv->stRoutineStartIP.wLine = wRoutineLine;
    lpstEnv->stRoutineStartIP.wOffset = wRoutineOffset;

    lpstEnv->bNewIP = TRUE;
    lpstEnv->dwIPLine = (DWORD)wRoutineLine;
    lpstEnv->dwIPOffset = (DWORD)wRoutineOffset;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ExternalCallSetup()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//  dwModuleIndex       Module index
//  dwModuleEntry       Module stream number
//
// Description:
//  Sets up for a call to an external routine.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ExternalCallSetup
(
    LPO97_ENV           lpstEnv,
    DWORD               dwModuleIndex,
    DWORD               dwModuleEntry
)
{
    LPO97_MODULE_ENV    lpstModEnv;

    // Maximum module depth of three

    if (lpstEnv->wModuleDepth >= 3)
        return(FALSE);

    // Can't allow recursive modules

    lpstModEnv = lpstEnv->lpstModEnv;
    while (lpstModEnv != NULL)
    {
        if (lpstModEnv->dwModuleIndex == dwModuleIndex)
            return(FALSE);

        lpstModEnv = lpstModEnv->lpstPrev;
    }

    // Create the environment for the module

    lpstModEnv = O97ModuleEnvCreate(lpstEnv);

    if (lpstModEnv == NULL)
        return(FALSE);

    // Open the stream

    if (SSOpenStreamAtIndex(lpstModEnv->lpstStream,
                            dwModuleEntry) != SS_STATUS_OK)
    {
        O97ModuleEnvDestroy(lpstEnv,lpstModEnv);
        return(FALSE);
    }

    // Initialize access to the module

    if (O97ModuleEnvInit(lpstModEnv,
                         dwModuleIndex,
                         dwModuleEntry) == FALSE)
    {
        O97ModuleEnvDestroy(lpstEnv,lpstModEnv);
        return(FALSE);
    }

    // Need to do the chaining now because the calls below require it

    lpstModEnv->lpstPrev = lpstEnv->lpstModEnv;
    lpstEnv->lpstModEnv = lpstModEnv;

    if (O97VarPushGlobalFrame(lpstEnv) == FALSE)
    {
        lpstEnv->lpstModEnv = lpstModEnv->lpstPrev;
        O97ModuleEnvDestroy(lpstEnv,lpstModEnv);
        return(FALSE);
    }

    lpstEnv->wModuleDepth++;

    O97StatePush(lpstEnv,esO97_EXTERNAL_CLEANUP);

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ExternalCallCleanup()
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

BOOL O97ExternalCallCleanup
(
    LPO97_ENV           lpstEnv
)
{
    BOOL                bResult = TRUE;
    LPO97_MODULE_ENV    lpstModEnv;

    // Pop esO97_EXTERNAL_CLEANUP

    O97StatePop(lpstEnv);

    lpstEnv->wModuleDepth--;
    if (O97VarPopGlobalFrame(lpstEnv) == FALSE)
        bResult = FALSE;

    lpstModEnv = lpstEnv->lpstModEnv->lpstPrev;

    assert(lpstModEnv != NULL);

    if (O97ModuleEnvDestroy(lpstEnv,lpstEnv->lpstModEnv) == FALSE)
        bResult = FALSE;

    lpstEnv->lpstModEnv = lpstModEnv;

    return(bResult);
}


//*************************************************************************
//
// BOOL O97SetupCallUsingString()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//
// Description:
//  Prepares for a call using a string with the format of either
//  "module.routine" or "routine".  This string should be the
//  first parameter.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97SetupCallUsingString
(
    LPO97_ENV           lpstEnv
)
{
    LPBYTE              lpbyRoutine;
    LPBYTE              lpbyModule;
    int                 i, nStrLen;

    // Get the string

    if (O97ParamGetString(lpstEnv,0,&lpbyRoutine) == FALSE)
        return(FALSE);

    // Empty string?

    if (lpbyRoutine[0] == 0)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Determine whether it is "module.routine" or
    //  just "routine" format

    // Get the module name, or routine name if appropriate

    nStrLen = lpbyRoutine[0];
    lpbyModule = lpbyRoutine;
    for (i=1;i<=nStrLen;i++)
    {
        // If we see the '.' then it means we got the module name

        if (lpbyRoutine[i] == '.')
        {
            // Empty module name?

            lpbyRoutine += i;
            break;
        }
    }

    if (lpbyModule != lpbyRoutine)
    {
        lpbyModule[0] = (BYTE)((lpbyRoutine - lpbyModule) - 1);
        if (lpbyModule[0] == 0)
        {
            lpstEnv->bIsStringCall = FALSE;
        }
        else
        {
            lpbyRoutine[0] = nStrLen - i;

            // Look up the module and routine IDs

            if (O97IdentLookup(lpstEnv,
                               lpbyModule,
                               lpbyRoutine,
                               &lpstEnv->wCallModuleID,
                               &lpstEnv->wCallRoutineID) == FALSE ||
                lpstEnv->wCallModuleID == O97_ID_INVALID ||
                lpstEnv->wCallRoutineID == O97_ID_INVALID)
                lpstEnv->bIsStringCall = FALSE;

            lpbyModule[0] = nStrLen;
            lpbyRoutine[0] = '.';
        }
    }
    else
    {
        // Just look up the routine ID

        if (O97IdentLookup(lpstEnv,
                           NULL,
                           lpbyRoutine,
                           NULL,
                           &lpstEnv->wCallRoutineID) == FALSE ||
            lpstEnv->wCallRoutineID == O97_ID_INVALID)
            lpstEnv->bIsStringCall = FALSE;

        lpstEnv->wCallModuleID = O97_ID_INVALID;
    }

    if (O97ParamInvalidateFirst(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}

