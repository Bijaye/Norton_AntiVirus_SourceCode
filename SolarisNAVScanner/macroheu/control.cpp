//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/CONTROL.CPv   1.2   18 May 1998 11:15:32   DCHI  $
//
// Description:
//  Main Word 6.0/7.0/95 emulation environment control functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/CONTROL.CPv  $
// 
//    Rev 1.2   18 May 1998 11:15:32   DCHI
// Fixed problem in ParamGetString().
// 
//    Rev 1.1   05 Aug 1997 18:16:22   DCHI
// Fixed CreateSelectControl(), was not copying last char of case string.
// 
//    Rev 1.0   30 Jun 1997 16:15:00   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"

//*************************************************************************
//
// BOOL ControlCreate()
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

BOOL ControlCreate
(
    LPENV       lpstEnv
)
{
    // Allocate control stack

    lpstEnv->nMaxControls = 64;

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      sizeof(CONTROL_T) * lpstEnv->nMaxControls,
                      (LPLPVOID)&lpstEnv->lpastControlStack) != SS_STATUS_OK)
        return(FALSE);

    lpstEnv->lpstTopControl = lpstEnv->lpastControlStack;

    // Allocate control data store

    lpstEnv->dwGSMControlSize = 4096;
    lpstEnv->lpstGSMControlData =
        GSMCreate(lpstEnv->lpvRootCookie,
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
// BOOL ControlDestroy()
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

BOOL ControlDestroy
(
    LPENV       lpstEnv
)
{
    BOOL        bResult = TRUE;

    if (GSMDestroy(lpstEnv->lpvRootCookie,
                   lpstEnv->lpstGSMControlData) == FALSE)
        bResult = FALSE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastControlStack) != SS_STATUS_OK)
        bResult = FALSE;

    return(bResult);
}


//*************************************************************************
//
// BOOL ControlInit()
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

BOOL ControlInit
(
    LPENV       lpstEnv
)
{
    lpstEnv->nNumControls = 1;
    lpstEnv->lpstTopControl = lpstEnv->lpastControlStack;
    lpstEnv->lpstTopControl->eState = esINIT;
    lpstEnv->lpstTopControl->eSubState = essDO;
    lpstEnv->lpstTopControl->lpvstControl = NULL;

    GSMInit(lpstEnv->lpstGSMControlData,
            lpstEnv->dwGSMControlSize);

    return(TRUE);
}


//*************************************************************************
//
// BOOL ControlCallPushParameter()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eType           Data type
//  lpvData         Ptr to data
//
// Description:
//  This function adds a parameter to the end of the current parameter
//  list.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL ControlCallPushParameter
(
    LPENV       lpstEnv,
    ETYPE_T     eType,
    LPVOID      lpvData
)
{
    LPCONTROL       lpstControl;
    LPPARAMETER     lpstParam, lpstNewParam;
    LPCONTROL_HDR   lpstControlHdr;
    int             i;

    assert(lpstEnv->lpstTopControl != NULL);

    lpstControl = lpstEnv->lpstTopControl;

    // Do preprocessing

    lpstControlHdr = (LPCONTROL_HDR)lpstControl->lpvstControl;
    assert(lpstControlHdr);
    assert(lpstControlHdr->ecType == ectIDENTIFIER ||
           lpstControlHdr->ecType == ectBUILTIN ||
           lpstControlHdr->ecType == ectDIALOG_CALL);

    // Allocate parameter storage and store the data

    switch (eType)
    {
        case eTYPE_VAR:
            lpstNewParam =
                (LPPARAMETER)GSMAlloc(lpstEnv->lpstGSMControlData,
                                      sizeof(PARAMETER_T));

            if (lpstNewParam == NULL)
                return(FALSE);

            // Store the pointer to the variable

            lpstNewParam->eType = eType;
            lpstNewParam->uData.lpvstVar = lpvData;
            break;

        case eTYPE_DIALOG_FIELD:
            lpstNewParam =
                (LPPARAMETER)GSMAlloc(lpstEnv->lpstGSMControlData,
                                      sizeof(PARAMETER_T));

            if (lpstNewParam == NULL)
                return(FALSE);

            // Store the dialog field value

            lpstNewParam->eType = eType;
            lpstNewParam->uData.wDlgField = *(LPWORD)lpvData;
            break;

        case eTYPE_LONG:
            lpstNewParam =
                (LPPARAMETER)GSMAlloc(lpstEnv->lpstGSMControlData,
                                      sizeof(PARAMETER_T));

            if (lpstNewParam == NULL)
                return(FALSE);

            // Store the long value

            lpstNewParam->eType = eType;
            lpstNewParam->uData.lValue = *(LPLONG)lpvData;
            break;

        case eTYPE_STRING:
            lpstNewParam =
                (LPPARAMETER)GSMAlloc(lpstEnv->lpstGSMControlData,
                                      sizeof(PARAMETER_T) +
                                      *(LPBYTE)lpvData + 1);

            if (lpstNewParam == NULL)
                return(FALSE);

            // Store the string data

            lpstNewParam->eType = eType;
            lpstNewParam->uData.lpbyStr = (LPBYTE)(lpstNewParam + 1);
            for (i=0;i<=*(LPBYTE)lpvData;i++)
                lpstNewParam->uData.lpbyStr[i] = ((LPBYTE)lpvData)[i];
            break;

        default:
            // Unknown type

            return(FALSE);
    }

    lpstNewParam->lpstNext = NULL;

    // Add the parameter to the end of the list

    switch (lpstControlHdr->ecType)
    {
        case ectIDENTIFIER:
        {
            LPIDENTIFIER    lpstIdentifier;

            lpstIdentifier = (LPIDENTIFIER)lpstControlHdr;

            if (lpstIdentifier->u.lpstFirstParam == NULL)
            {
                // This is the first parameter

                lpstIdentifier->u.lpstFirstParam = lpstNewParam;
                return(TRUE);
            }

            lpstParam = lpstIdentifier->u.lpstFirstParam;
            break;
        }

        case ectBUILTIN:
        {
            LPBUILTIN       lpstBuiltIn;

            lpstBuiltIn = (LPBUILTIN)lpstControlHdr;

            if (lpstBuiltIn->lpstFirstParam == NULL)
            {
                // This is the first parameter

                lpstBuiltIn->lpstFirstParam = lpstNewParam;
                return(TRUE);
            }

            lpstParam = lpstBuiltIn->lpstFirstParam;
            break;
        }

        default: // ectDIALOG_CALL
        {
            LPDIALOG_CALL       lpstDialogCall;

            lpstDialogCall = (LPDIALOG_CALL)lpstControlHdr;

            if (lpstDialogCall->lpstFirstParam == NULL)
            {
                // This is the first parameter

                lpstDialogCall->lpstFirstParam = lpstNewParam;
                return(TRUE);
            }

            lpstParam = lpstDialogCall->lpstFirstParam;
            break;
        }
    }

    // Search for last parameter

    while (lpstParam->lpstNext != NULL)
        lpstParam = lpstParam->lpstNext;

    // Attach to last parameter

    lpstParam->lpstNext = lpstNewParam;
    return(TRUE);
}


//*************************************************************************
//
// BOOL ControlFreeParameter()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpstParam       Ptr to parameter
//
// Description:
//  This function frees memory allocated for a parameter.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL ControlFreeParameter
(
    LPENV       lpstEnv,
    LPPARAMETER lpstParam
)
{
    if (GSMFree(lpstEnv->lpstGSMControlData,
                lpstParam) == FALSE)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// BOOL ControlFreeParameterList()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpstParam       Ptr to parameter list
//
// Description:
//  This function frees memory allocated for a chained set of parameters.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL ControlFreeParameterList
(
    LPENV       lpstEnv,
    LPPARAMETER lpstParam
)
{
    LPPARAMETER lpstNext;

    while (lpstParam != NULL)
    {
        lpstNext = lpstParam->lpstNext;
        if (ControlFreeParameter(lpstEnv,
                                 lpstParam) == FALSE)
            return(FALSE);

        lpstParam = lpstNext;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL ParamGetLong()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpstParam       Ptr to parameter list
//  lplValue        Ptr to variable to store long
//
// Description:
//  This function attempts to get the value of a parameter as a long
//  value.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL ParamGetLong
(
    LPENV       lpstEnv,
    LPPARAMETER lpstParam,
    LPLONG      lplValue
)
{
    if (lpstParam == NULL)
        return(FALSE);

    if (lpstParam->eType == eTYPE_VAR)
    {
        LPVAR   lpstVar;

        lpstVar = (LPVAR)lpstParam->uData.lpvstVar;
        if (lpstVar->eType != eTYPE_LONG)
            return(FALSE);

        *lplValue = lpstVar->uData.lValue;
    }
    else
    if (lpstParam->eType != eTYPE_LONG)
        return(FALSE);
    else
        *lplValue = lpstParam->uData.lValue;

    return(TRUE);
}


//*************************************************************************
//
// BOOL ParamGetString()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpstParam       Ptr to parameter list
//  lplpbyStr       Ptr to variable to store ptr to string
//
// Description:
//  This function attempts to get the value of a parameter as a string
//  value.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL ParamGetString
(
    LPENV       lpstEnv,
    LPPARAMETER lpstParam,
    LPLPBYTE    lplpbyStr
)
{
    if (lpstParam == NULL)
        return(FALSE);

    if (lpstParam->eType == eTYPE_VAR)
    {
        LPVAR   lpstVar;

        lpstVar = (LPVAR)lpstParam->uData.lpvstVar;
        if (lpstVar->eType == eTYPE_VAR)
            lpstVar = (LPVAR)lpstVar->uData.lpvstVar;

        if (lpstVar->eType != eTYPE_STRING)
            return(FALSE);

        *lplpbyStr = lpstVar->uData.lpbyStr;
    }
    else
    if (lpstParam->eType != eTYPE_STRING)
        return(FALSE);
    else
        *lplpbyStr = lpstParam->uData.lpbyStr;

    return(TRUE);
}


//*************************************************************************
//
// void StateNew()
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
void StateNew
(
    LPENV       lpstEnv,
    ESTATE_T    eState
)
{
    assert(lpstEnv->nNumControls > 0);

    lpstEnv->lpstTopControl->eState = eState;
}
#endif


//*************************************************************************
//
// void StatePush()
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

void StatePush
(
    LPENV       lpstEnv,
    ESTATE_T    eState
)
{
    if (lpstEnv->nNumControls++ >= lpstEnv->nMaxControls)
    {
        lpstEnv->eError = eERROR_STATE_PUSH;
        lpstEnv->nNumControls--;
    }
    else
    {
        LPCONTROL   lpstCurControl;
        LPCONTROL   lpstNewControl;

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
// void StatePop()
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

ESTATE_T StatePop
(
    LPENV       lpstEnv
)
{
    ESTATE_T   eState;

    assert(lpstEnv->nNumControls > 1);
#if 0
    if (lpstEnv->lpstTopControl->lpvstControl != NULL)
    {
        printf("Non-empty control popped %d\n",
            ((LPCONTROL_HDR)lpstEnv->lpstTopControl->lpvstControl)->ecType);

        printf("dwIP=%lu\n",lpstEnv->dwIP);

        if (((LPCONTROL_HDR)lpstEnv->lpstTopControl->lpvstControl)->ecType ==
            ectIDENTIFIER)
        {
            int i;
            LPBYTE lpbyName =
                ((LPIDENTIFIER)lpstEnv->lpstTopControl->lpvstControl)->lpbyName;

            i = *lpbyName++;
            while (i-- > 0)
                putchar(*lpbyName++);

            putchar('\n');

        }

    }
#endif
    // Remember the top state

    eState = lpstEnv->lpstTopControl->eState;

    // Update # of controls and ptr to top control

    lpstEnv->nNumControls--;
    lpstEnv->lpstTopControl--;

    return(eState);
}


//*************************************************************************
//
// ESTATE_T StateTop()
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
ESTATE_T StateTop
(
    LPENV       lpstEnv
)
{
    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl != NULL);

    return(lpstEnv->lpstTopControl->eState);
}
#endif


//*************************************************************************
//
// void SubStateNew()
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
void SubStateNew
(
    LPENV       lpstEnv,
    ESUBSTATE_T eSubState
)
{
    assert(lpstEnv->nNumControls > 0);

    lpstEnv->lpstTopControl->eSubState = eSubState;
}
#endif


//*************************************************************************
//
// ESUBSTATE_T SubStateTop()
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
ESUBSTATE_T SubStateTop
(
    LPENV       lpstEnv
)
{
    assert(lpstEnv->nNumControls > 0);

    return(lpstEnv->lpstTopControl->eSubState);
}
#endif


//*************************************************************************
//
// void ControlSetTopControl()
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
void ControlSetTopControl
(
    LPENV       lpstEnv,
    LPVOID      lpvstControl
)
{
    assert(lpstEnv->nNumControls > 0);

    lpstEnv->lpstTopControl->lpvstControl = lpvstControl;
}
#endif


//*************************************************************************
//
// void ControlDestroyTopControl()
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

void ControlDestroyTopControl
(
    LPENV           lpstEnv
)
{
    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl->lpvstControl != NULL);

/*
    printf("ControlDestroy %d\n",
        ((LPCONTROL_HDR)lpstEnv->lpstTopControl->lpvstControl)->ecType);

    if (((LPCONTROL_HDR)lpstEnv->lpstTopControl->lpvstControl)->ecType > 8)
        printf("dwIP=%lu\n",lpstEnv->dwIP);
*/
    if (GSMFree(lpstEnv->lpstGSMControlData,
                lpstEnv->lpstTopControl->lpvstControl) == FALSE)
    {
        lpstEnv->eError = eERROR_CONTROL_ALLOC;
        return;
    }

    lpstEnv->lpstTopControl->lpvstControl = NULL;
}


//*************************************************************************
//
// BOOL ControlCreateTopControl()
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

BOOL ControlCreateControl
(
    LPENV           lpstEnv,
    ECONTROL_TYPE_T ecType
)
{
    LPCONTROL_HDR   lpstControl;
    DWORD           dwControlSize;

    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl != NULL);

//    printf("ControlCreate %d\n",ecType);

    switch (ecType)
    {
        case ectIDENTIFIER:
            dwControlSize = sizeof(IDENTIFIER_T) +
                lpstEnv->uTokenData.Identifier.byLen + 1;
            break;

        case ectBUILTIN:
            dwControlSize = sizeof(BUILTIN_T);
            break;

        case ectCALL:
            dwControlSize = sizeof(CALL_T);
            break;

        case ectCALL_GET_ARGS:
            dwControlSize = sizeof(CALL_GET_ARGS_T);
            break;

        case ectEXPR:
            dwControlSize = sizeof(EXPR_T);
            break;

        case ectFOR:
            dwControlSize = sizeof(FOR_T);
            break;

        case ectWHILE:
            dwControlSize = sizeof(WHILE_T);
            break;

        case ectDIALOG_CALL:
            dwControlSize = sizeof(DIALOG_CALL_T);
            break;

        case ectLABEL_IDENTIFIER:
            dwControlSize = sizeof(LABEL_IDENTIFIER_T) +
                lpstEnv->uTokenData.Identifier.byLen + 1;
            break;

        case ectLABEL_WORD:
            dwControlSize = sizeof(LABEL_WORD_T);
            break;

        default:
            assert(0);
            break;
    }

    // Allocate memory for the control structure

    lpstControl =
        (LPCONTROL_HDR)GSMAlloc(lpstEnv->lpstGSMControlData,
                                dwControlSize);

    if (lpstControl == NULL)
    {
        lpstEnv->eError = eERROR_CONTROL_ALLOC;
        return(FALSE);
    }

    lpstControl->ecType = ecType;

    // Do postprocessing

    switch (ecType)
    {
        case ectIDENTIFIER:
        {
            LPIDENTIFIER    lpstIdentifier;
            LPBYTE          lpbyIdentifier;
            int             i;

            lpstIdentifier = (LPIDENTIFIER)lpstControl;
            lpbyIdentifier = (LPBYTE)(lpstIdentifier + 1);
            lpstIdentifier->lpbyName = lpbyIdentifier;

            // Copy the length and the identifier

            *lpbyIdentifier++ = lpstEnv->uTokenData.Identifier.byLen;
            for (i=0;i<lpstEnv->uTokenData.Identifier.byLen;i++)
                lpbyIdentifier[i] = lpstEnv->uTokenData.Identifier.lpby[i];

            lpstIdentifier->u.lpstFirstParam = NULL;
            break;
        }

        case ectBUILTIN:
        {
            LPBUILTIN       lpstBuiltIn;

            lpstBuiltIn = (LPBUILTIN)lpstControl;

            lpstBuiltIn->eWDCMD = lpstEnv->uTokenData.eWDCMD;
            lpstBuiltIn->lpstFirstParam = NULL;
            break;
        }

        case ectEXPR:
        {
            ((LPEXPR)lpstControl)->wParenDepth = 0;
            break;
        }

        case ectFOR:
        {
            ((LPFOR)lpstControl)->lStep = 1;
            break;
        }

        case ectWHILE:
        {
            ((LPWHILE)lpstControl)->dwCondIP = lpstEnv->dwIP;
            break;
        }

        case ectDIALOG_CALL:
        {
            ((LPDIALOG_CALL)lpstControl)->lpstFirstParam = NULL;
            break;
        }

        case ectLABEL_IDENTIFIER:
        {
            LPLABEL_IDENTIFIER  lpstIdentifier;
            LPBYTE              lpbyIdentifier;
            int                 i;

            lpstIdentifier = (LPLABEL_IDENTIFIER)lpstControl;
            lpbyIdentifier = (LPBYTE)(lpstIdentifier + 1);
            lpstIdentifier->lpbyLabel = lpbyIdentifier;

            // Copy the length and the identifier

            *lpbyIdentifier++ = lpstEnv->uTokenData.Identifier.byLen;
            for (i=0;i<lpstEnv->uTokenData.Identifier.byLen;i++)
                lpbyIdentifier[i] = lpstEnv->uTokenData.Identifier.lpby[i];

            lpstIdentifier->dwStartIP = lpstEnv->dwIP;
            break;
        }

        case ectLABEL_WORD:
        {
            LPLABEL_WORD        lpstLabelWord;

            lpstLabelWord = (LPLABEL_WORD)lpstControl;
            lpstLabelWord->wLabel = lpstEnv->uTokenData.wValue;
            lpstLabelWord->dwStartIP = lpstEnv->dwIP;
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
// BOOL ControlCreateSelectControl()
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

BOOL ControlCreateSelectControl
(
    LPENV           lpstEnv,
    ETYPE_T         eType,
    LPVOID          lpvData
)
{
    LPSELECT        lpstSelect;
    DWORD           dwControlSize;

    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl != NULL);

    switch (eType)
    {
        case eTYPE_STRING:
            dwControlSize = sizeof(SELECT_T) +
                *(LPBYTE)lpvData + 1;
            break;

        case eTYPE_LONG:
            dwControlSize = sizeof(SELECT_T);
            break;

        default:
            return(FALSE);
    }

    // Allocate memory for the control structure

    lpstSelect =
        (LPSELECT)GSMAlloc(lpstEnv->lpstGSMControlData,dwControlSize);

    if (lpstSelect == NULL)
    {
        lpstEnv->eError = eERROR_CONTROL_ALLOC;
        return(FALSE);
    }

    lpstSelect->ecType = ectSELECT;

    // Do postprocessing

    lpstSelect->eType = eType;
    switch (eType)
    {
        case eTYPE_STRING:
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

        case eTYPE_LONG:
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
// LPVOID ControlTop()
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
LPVOID ControlTop
(
    LPENV           lpstEnv
)
{
    assert(lpstEnv->nNumControls > 0);
    assert(lpstEnv->lpstTopControl);

    return(lpstEnv->lpstTopControl->lpvstControl);
}
#endif

