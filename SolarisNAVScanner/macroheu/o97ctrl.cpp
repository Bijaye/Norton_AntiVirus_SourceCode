//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97CTRL.CPv   1.1   12 Oct 1998 13:20:50   DCHI  $
//
// Description:
//  Control state handling functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97CTRL.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:20:50   DCHI
// Added control count verification to O97StatePop().
// 
//    Rev 1.0   15 Aug 1997 13:28:54   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"

//*************************************************************************
//
// BOOL O97ControlCreate()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function allocates memory for state control.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ControlCreate
(
    LPO97_ENV       lpstEnv
)
{
    // Allocate control stack

    lpstEnv->nMaxControls = 64;

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      sizeof(O97_CONTROL_T) * lpstEnv->nMaxControls,
                      (LPLPVOID)&lpstEnv->lpastControlStack) != SS_STATUS_OK)
        return(FALSE);

    lpstEnv->lpstTopControl = lpstEnv->lpastControlStack;

    // Allocate control data store

    lpstEnv->dwGSMControlSize = 4096;
    lpstEnv->lpstGSMControlData = GSMCreate(lpstEnv->lpvRootCookie,
                                            lpstEnv->dwGSMControlSize);

    if (lpstEnv->lpstGSMControlData == NULL)
    {
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastControlStack);
        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ControlDestroy()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function frees memory allocated for state control.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ControlDestroy
(
    LPO97_ENV       lpstEnv
)
{
    GSMDestroy(lpstEnv->lpvRootCookie,
               lpstEnv->lpstGSMControlData);

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastControlStack) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ControlInit()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function initializes memory allocated for state control.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ControlInit
(
    LPO97_ENV       lpstEnv
)
{
    lpstEnv->nNumControls = 1;
    lpstEnv->lpstTopControl = lpstEnv->lpastControlStack;
    lpstEnv->lpstTopControl->eState = esO97_INIT;
    lpstEnv->lpstTopControl->eSubState = essO97_DO;
    lpstEnv->lpstTopControl->lpvstControl = NULL;

    GSMInit(lpstEnv->lpstGSMControlData,
            lpstEnv->dwGSMControlSize);

    return(TRUE);
}


//*************************************************************************
//
// void O97StateNew()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eState          New state
//
// Description:
//  Changes the top state to the given new state.
//
// Returns:
//  Nothing
//
//*************************************************************************

#ifndef NDEBUG
void O97StateNew
(
    LPO97_ENV       lpstEnv,
    EO97_STATE_T    eState
)
{
    assert(lpstEnv->nNumControls > 0);

    lpstEnv->lpstTopControl->eState = eState;
}
#endif


//*************************************************************************
//
// void O97StatePush()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eState          New state
//
// Description:
//  Pushes a new state.
//
// Returns:
//  Nothing
//
//*************************************************************************

void O97StatePush
(
    LPO97_ENV       lpstEnv,
    EO97_STATE_T    eState
)
{
    if (lpstEnv->nNumControls++ >= lpstEnv->nMaxControls)
    {
        lpstEnv->eError = eO97_ERROR_STATE_PUSH;
        lpstEnv->nNumControls--;
    }
    else
    {
        LPO97_CONTROL   lpstCurControl;
        LPO97_CONTROL   lpstNewControl;

        // Get current control and update top control

        lpstCurControl = lpstEnv->lpstTopControl++;

        // Update # of controls and ptr to top control

        lpstNewControl = lpstEnv->lpstTopControl;
        lpstNewControl->eState = eState;
        lpstNewControl->eSubState = lpstCurControl->eSubState;
        lpstNewControl->lpvstControl = NULL;
    }
}


//*************************************************************************
//
// EO97_STATE_T O97StatePop()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops the top state.
//
// Returns:
//  Nothing
//
//*************************************************************************

EO97_STATE_T O97StatePop
(
    LPO97_ENV       lpstEnv
)
{
    EO97_STATE_T   eState;

    if (lpstEnv->nNumControls <= 1)
    {
        lpstEnv->eError = eO97_ERROR_STATE_POP;
        return(esO97_ERROR);
    }

    assert(lpstEnv->nNumControls > 1);

#if 0
    if (lpstEnv->lpstTopControl->lpvstControl != NULL)
    {
        printf("Non-empty control popped %d\n",
            ((LPO97_CONTROL_HDR)lpstEnv->lpstTopControl->lpvstControl)->ecType);

        printf("dwIP=(%u,%u)\n",lpstEnv->dwIPLine,lpstEnv->dwIPOffset);
    }
#endif

    // Remember the top state

    eState = lpstEnv->lpstTopControl->eState;

    // Update # of controls and ptr to top control

    lpstEnv->nNumControls--;
    lpstEnv->lpstTopControl--;

    // See if we need to keep the substate as essO97_SKIP

    if (lpstEnv->eStateExit != eO97_STATE_EXIT_NONE)
        lpstEnv->lpstTopControl->eSubState = essO97_SKIP;

    return(eState);
}


//*************************************************************************
//
// EO97_STATE_T O97StateTop()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Gets the top state.
//
// Returns:
//  ESTATE_T        The top state
//
//*************************************************************************

#ifndef NDEBUG
EO97_STATE_T O97StateTop
(
    LPO97_ENV       lpstEnv
)
{
    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl != NULL);

    return(lpstEnv->lpstTopControl->eState);
}
#endif


//*************************************************************************
//
// void O97SubStateNew()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eSubState       New substate
//
// Description:
//  Sets the current substate.
//
// Returns:
//  Nothing
//
//*************************************************************************

#ifndef NDEBUG
void O97SubStateNew
(
    LPO97_ENV       lpstEnv,
    EO97_SUBSTATE_T eSubState
)
{
    assert(lpstEnv->nNumControls > 0);

    lpstEnv->lpstTopControl->eSubState = eSubState;
}
#endif


//*************************************************************************
//
// EO97_SUBSTATE_T O97SubStateTop()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Gets the current substate.
//
// Returns:
//  ESUBSTATE_T     Substate
//
//*************************************************************************

#ifndef NDEBUG
EO97_SUBSTATE_T O97SubStateTop
(
    LPO97_ENV       lpstEnv
)
{
    assert(lpstEnv->nNumControls > 0);

    return(lpstEnv->lpstTopControl->eSubState);
}
#endif


//*************************************************************************
//
// void O97ControlSetTopControl()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Sets the top control.
//
// Returns:
//  Nothing
//
//*************************************************************************

#ifndef NDEBUG
void O97ControlSetTopControl
(
    LPO97_ENV       lpstEnv,
    LPVOID      lpvstControl
)
{
    assert(lpstEnv->nNumControls > 0);

    lpstEnv->lpstTopControl->lpvstControl = lpvstControl;
}
#endif


//*************************************************************************
//
// void O97ControlDestroyTopControl()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Frees memory allocated for the top control.
//
// Returns:
//  Nothing
//
//*************************************************************************

void O97ControlDestroyTopControl
(
    LPO97_ENV           lpstEnv
)
{
    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl->lpvstControl != NULL);

    if (GSMFree(lpstEnv->lpstGSMControlData,
                lpstEnv->lpstTopControl->lpvstControl) == FALSE)
    {
        lpstEnv->eError = eO97_ERROR_CONTROL_ALLOC;
        return;
    }

    lpstEnv->lpstTopControl->lpvstControl = NULL;
}


//*************************************************************************
//
// BOOL O97ControlCreateTopControl()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  ecType          Type of control to create
//
// Description:
//  Creates a control for the top control.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ControlCreateControl
(
    LPO97_ENV           lpstEnv,
    EO97_CONTROL_TYPE_T ecType
)
{
    LPO97_CONTROL_HDR   lpstControl;
    DWORD               dwControlSize;

    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl != NULL);

    switch (ecType)
    {
        case ectO97_CALL:
            dwControlSize = sizeof(O97_CALL_T);
            break;

        case ectO97_CALL_GET_ARGS:
            dwControlSize = sizeof(O97_CALL_GET_ARGS_T);
            break;

        case ectO97_EXPR:
            dwControlSize = sizeof(O97_EXPR_T);
            break;

        case ectO97_FOR:
            dwControlSize = sizeof(O97_FOR_T);
            break;

        case ectO97_DO_LOOP:
            dwControlSize = sizeof(O97_DO_LOOP_T);
            break;

        case ectO97_WHILE:
            dwControlSize = sizeof(O97_WHILE_T);
            break;

        case ectO97_DIALOG_CALL:
            dwControlSize = sizeof(O97_DIALOG_CALL_T);
            break;

        default:
            assert(0);
            break;
    }

    // Allocate memory for the control structure

    lpstControl =
        (LPO97_CONTROL_HDR)GSMAlloc(lpstEnv->lpstGSMControlData,
                                    dwControlSize);

    if (lpstControl == NULL)
    {
        lpstEnv->eError = eO97_ERROR_CONTROL_ALLOC;
        return(FALSE);
    }

    lpstControl->ecType = ecType;

    // Do postprocessing

    switch (ecType)
    {
        case ectO97_EXPR:
        {
            ((LPO97_EXPR)lpstControl)->wParenDepth = 0;
            break;
        }

        case ectO97_FOR:
        {
            ((LPO97_FOR)lpstControl)->lStep = 1;
            break;
        }

        case ectO97_DO_LOOP:
        {
            // Assume that it is always at the beginning of the line

            ((LPO97_DO_LOOP)lpstControl)->stCondIP.wLine =
                (WORD)lpstEnv->dwIPLine;
            ((LPO97_DO_LOOP)lpstControl)->stCondIP.wOffset = 0;
            break;
        }

        case ectO97_WHILE:
        {
            // Assume that it is always at the beginning of the line

            ((LPO97_WHILE)lpstControl)->stCondIP.wLine =
                (WORD)lpstEnv->dwIPLine;
            ((LPO97_WHILE)lpstControl)->stCondIP.wOffset = 0;
            break;
        }

        case ectO97_DIALOG_CALL:
        {
            ((LPO97_DIALOG_CALL)lpstControl)->lpstFirstParam = NULL;
            break;
        }

        default:
            break;
    }

    lpstEnv->lpstTopControl->lpvstControl = lpstControl;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ControlCreateSelectControl()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eType           Data type
//  lpvData         Ptr to data
//
// Description:
//  Creates a select control for the top control.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ControlCreateSelectControl
(
    LPO97_ENV           lpstEnv,
    EO97_TYPE_T         eType,
    LPVOID              lpvData
)
{
    LPO97_SELECT        lpstSelect;
    DWORD               dwControlSize;

    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl != NULL);

    switch (eType)
    {
        case eO97_TYPE_STRING:
            dwControlSize = sizeof(O97_SELECT_T) +
                *(LPBYTE)lpvData + 1;
            break;

        case eO97_TYPE_LONG:
            dwControlSize = sizeof(O97_SELECT_T);
            break;

        default:
            return(FALSE);
    }

    // Allocate memory for the control structure

    lpstSelect =
        (LPO97_SELECT)GSMAlloc(lpstEnv->lpstGSMControlData,dwControlSize);

    if (lpstSelect == NULL)
    {
        lpstEnv->eError = eO97_ERROR_CONTROL_ALLOC;
        return(FALSE);
    }

    lpstSelect->ecType = ectO97_SELECT;

    // Do postprocessing

    lpstSelect->eType = eType;
    switch (eType)
    {
        case eO97_TYPE_STRING:
        {
            LPBYTE      lpbySrc;
            LPBYTE      lpbyDst;
            BYTE        byLen;

            lpstSelect->uData.lpbyStr = (LPBYTE)(lpstSelect + 1);

            // Copy the length and the identifier

            lpbyDst = lpstSelect->uData.lpbyStr;
            lpbySrc = (LPBYTE)lpvData;
            byLen = *lpbySrc;
            do
            {
                *lpbyDst++ = *lpbySrc++;
            }
            while (byLen-- != 0);
            break;
        }

        case eO97_TYPE_LONG:
            lpstSelect->uData.lValue = *(LPLONG)lpvData;
            break;

        default:
            // This should never happen
            assert(0);
            return(FALSE);
    }

    lpstEnv->lpstTopControl->lpvstControl = lpstSelect;

    return(TRUE);
}

//*************************************************************************
//
// LPVOID O97ControlTop()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Returns a ptr to the top control.
//
// Returns:
//  LPVOID          Ptr to top control
//
//*************************************************************************

#ifndef NDEBUG
LPVOID O97ControlTop
(
    LPO97_ENV           lpstEnv
)
{
    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl);

    return(lpstEnv->lpstTopControl->lpvstControl);
}
#endif

