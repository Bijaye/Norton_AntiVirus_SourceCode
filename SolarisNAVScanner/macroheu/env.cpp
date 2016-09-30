//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/ENV.CPv   1.0   30 Jun 1997 16:15:20   DCHI  $
//
// Description:
//  Main Word 6.0/7.0/95 emulation environment functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/ENV.CPv  $
// 
//    Rev 1.0   30 Jun 1997 16:15:20   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"

//*************************************************************************
//
// LPENV WD7EnvCreate()
//
// Parameters:
//  None
//
// Description:
//  Allocates memory for an emulation environment.
//
// Returns:
//  LPENV           On success, ptr to environment structure
//  NULL            On failure
//
//*************************************************************************

LPSTR glpszNormal = "\\NORMAL.DOT";
LPSTR glpszFN = "FN.DOC";

LPENV WD7EnvCreate
(
    LPVOID          lpvRootCookie
)
{
    LPENV           lpstEnv;

    // Allocate environment structure

    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(ENV_T),
                      (LPLPVOID)&lpstEnv) != SS_STATUS_OK)
        return(NULL);

    lpstEnv->lpvRootCookie = lpvRootCookie;

    // Allocate global variable storage

    if (VarCreate(lpstEnv) == FALSE)
    {
        SSMemoryFree(lpvRootCookie,lpstEnv);
        return(NULL);
    }

    // Allocate expression evaluation stack

    if (EvalStackCreate(lpstEnv) == FALSE)
    {
        VarDestroy(lpstEnv);
        SSMemoryFree(lpvRootCookie,lpstEnv);
        return(NULL);
    }

    // Allocate control stack

    if (ControlCreate(lpstEnv) == FALSE)
    {
        EvalStackDestroy(lpstEnv);
        VarDestroy(lpstEnv);
        SSMemoryFree(lpvRootCookie,lpstEnv);
        return(NULL);
    }

    lpstEnv->nNormalLen = sizeof(glpszNormal) - 1;
    lpstEnv->lpszNormal = glpszNormal;
    lpstEnv->nFNLen = sizeof(glpszFN) - 1;
    lpstEnv->lpszFN = glpszFN;

    return(lpstEnv);
}


//*************************************************************************
//
// LPENV WD7EnvDestroy()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Destroys memory allocated for an emulation environment.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL WD7EnvDestroy
(
    LPENV           lpstEnv
)
{
    if (ControlDestroy(lpstEnv) == FALSE)
        return(FALSE);

    if (EvalStackDestroy(lpstEnv) == FALSE)
        return(FALSE);

    if (VarDestroy(lpstEnv) == FALSE)
        return(FALSE);

    SSMemoryFree(lpstEnv->lpvRootCookie,lpstEnv);
    return(TRUE);
}


//*************************************************************************
//
// LPENV WD7EnvInit()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpstStream      Ptr to WordDocument stream
//  lpstKey         Key
//
// Description:
//  Initializes environment for emulation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL WD7EnvInit
(
    LPENV           lpstEnv,
    LPSS_STREAM     lpstStream,
    LPWD7ENCKEY     lpstKey
)
{
    if (ControlInit(lpstEnv) == FALSE)
        return(FALSE);

    if (EvalStackInit(lpstEnv) == FALSE)
        return(FALSE);

    if (VarInit(lpstEnv) == FALSE)
        return(FALSE);

    lpstEnv->lpstStream = lpstStream;
    lpstEnv->lpstKey = lpstKey;

    lpstEnv->lpstModEnv = NULL;

    lpstEnv->wModuleDepth = 0;

    return(TRUE);
}


