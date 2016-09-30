//************************************************************************
//
// $Header:   S:/NAVEX/VCS/BUILTIN.CPv   1.5   09 Dec 1998 17:45:10   DCHI  $
//
// Description:
//  Contains code implementing necessary built-in functions for
//  Word 6.0/95 emulator for heuristic macro virus detection.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/BUILTIN.CPv  $
// 
//    Rev 1.5   09 Dec 1998 17:45:10   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.4   12 Oct 1998 13:39:50   DCHI
// Added NLM relinquish control calls to MacroCopy() and Organizer().
// 
//    Rev 1.3   10 Jun 1998 13:22:10   DCHI
// Added check for NULL param in Val().
// 
//    Rev 1.2   13 Jan 1998 11:16:38   DCHI
// Organizer now checks to be sure .Tab == 3 before logging copy.
// 
//    Rev 1.1   09 Jul 1997 17:12:24   DCHI
// Added #ifdef MACROHEU around entire source.
// 
//    Rev 1.0   09 Jul 1997 16:14:32   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"
#include "wbutil.h"
#include "copy.h"

BOOL Val
(
    LPENV       lpstEnv
)
{
    LPPARAMETER lpstParam;

    // Convert a string to a number

    // The first parameter should be a string

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    // Only take care of strings

    if (lpstParam != NULL && lpstParam->eType == eTYPE_STRING)
    {
        long    lValue;

        lValue = PascalStrToLong(lpstParam->uData.lpbyStr);

        return EvalPushLongOperand(lpstEnv,lValue);
    }
    else
    {
        // Just push a zero

        return EvalPushLongOperand(lpstEnv,0);
    }
}

BOOL Len
(
    LPENV       lpstEnv
)
{
    LPBYTE      lpbyStr;

    if (ParamGetString(lpstEnv,
                       ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam,
                       &lpbyStr) == FALSE)
        return EvalPushLongOperand(lpstEnv,0);

    return EvalPushLongOperand(lpstEnv,lpbyStr[0]);
}

BOOL ChrS
(
    LPENV       lpstEnv
)
{
    long        lValue;
    BYTE        abyChr[2];

    // Convert a character code to a string

    // The first parameter should be numeric

    if (ParamGetLong(lpstEnv,
                     ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam,
                     &lValue) == FALSE || lValue == 0)
        return EvalPushSZOperand(lpstEnv,"");

    abyChr[0] = 1;
    abyChr[1] = (BYTE)lValue;

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyChr);
}

BOOL StrS
(
    LPENV       lpstEnv
)
{
    long        lValue;
    BYTE        abyStr[16];

    // Convert a number to a string

    // The first parameter should be numeric
    // Only take care of longs

    if (ParamGetLong(lpstEnv,
                     ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam,
                     &lValue) == FALSE)
        lValue = 0;

    LongToPascalStr(lValue,abyStr);

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyStr);
}

BOOL LCaseS
(
    LPENV       lpstEnv
)
{
    BYTE        abyStr[256];
    LPBYTE      lpbyStr;

    if (ParamGetString(lpstEnv,
                       ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam,
                       &lpbyStr) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    PascalStrToLower(lpbyStr,abyStr);

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyStr);
}

BOOL UCaseS
(
    LPENV       lpstEnv
)
{
    BYTE        abyStr[256];
    LPBYTE      lpbyStr;

    if (ParamGetString(lpstEnv,
                       ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam,
                       &lpbyStr) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    PascalStrToUpper(lpbyStr,abyStr);

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyStr);
}

BOOL LeftS
(
    LPENV       lpstEnv
)
{
    BYTE        abyStr[256];
    LPBYTE      lpbyStr;
    LPPARAMETER lpstParam;
    long        lCount;

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbyStr) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    lpstParam = lpstParam->lpstNext;

    if (ParamGetLong(lpstEnv,
                     lpstParam,
                     &lCount) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    PascalStrLeft(lpbyStr,lCount,abyStr);

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyStr);
}

BOOL RightS
(
    LPENV       lpstEnv
)
{
    BYTE        abyStr[256];
    LPBYTE      lpbyStr;
    LPPARAMETER lpstParam;
    long        lCount;

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbyStr) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    lpstParam = lpstParam->lpstNext;

    if (ParamGetLong(lpstEnv,
                     lpstParam,
                     &lCount) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    PascalStrRight(lpbyStr,lCount,abyStr);

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyStr);
}

BOOL MidS
(
    LPENV       lpstEnv
)
{
    BYTE        abyStr[256];
    LPBYTE      lpbyStr;
    LPPARAMETER lpstParam;
    long        lStart, lCount;

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbyStr) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    lpstParam = lpstParam->lpstNext;

    if (ParamGetLong(lpstEnv,
                     lpstParam,
                     &lStart) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    lpstParam = lpstParam->lpstNext;

    if (ParamGetLong(lpstEnv,
                     lpstParam,
                     &lCount) == FALSE)
    {
        // Copy up to the end

        lCount = *lpbyStr;
    }

    PascalStrMid(lpbyStr,lStart,lCount,abyStr);

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyStr);
}

BOOL LTrimS
(
    LPENV       lpstEnv
)
{
    BYTE        abyStr[256];
    LPBYTE      lpbyStr;
    LPPARAMETER lpstParam;

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbyStr) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    PascalStrRTrim(lpbyStr,abyStr);

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyStr);
}

BOOL RTrimS
(
    LPENV       lpstEnv
)
{
    BYTE        abyStr[256];
    LPBYTE      lpbyStr;
    LPPARAMETER lpstParam;

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbyStr) == FALSE)
    {
        return EvalPushSZOperand(lpstEnv,"");
    }

    PascalStrRTrim(lpbyStr,abyStr);

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyStr);
}

BOOL InStr
(
    LPENV       lpstEnv
)
{
    LPBYTE      lpbyStr;
    LPBYTE      lpbySearchStr;
    LPPARAMETER lpstParam;
    long        lIndex;

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbyStr) == FALSE)
    {
        if (ParamGetLong(lpstEnv,
                         lpstParam,
                         &lIndex) == FALSE)
            return EvalPushLongOperand(lpstEnv,0);

        lpstParam = lpstParam->lpstNext;

        if (ParamGetString(lpstEnv,
                           lpstParam,
                           &lpbyStr) == FALSE)
            return EvalPushLongOperand(lpstEnv,0);
    }
    else
        lIndex = 1;

    // Get the search string

    lpstParam = lpstParam->lpstNext;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbySearchStr) == FALSE)
        return EvalPushLongOperand(lpstEnv,0);

    // Search

    lIndex = PascalStrInStr(lpbyStr,lIndex,lpbySearchStr);

    return EvalPushLongOperand(lpstEnv,lIndex);
}

BOOL FileNameS
(
    LPENV       lpstEnv
)
{
    return EvalPushSZOperand(lpstEnv,lpstEnv->lpszFN);
}

BOOL FileNameInfoS
(
    LPENV       lpstEnv
)
{
    LPPARAMETER lpstParam;
    LPBYTE      lpbyStr;
    long        lInfoType;

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbyStr) == FALSE)
        return EvalPushSZOperand(lpstEnv,"");

    lpstParam = lpstParam->lpstNext;

    if (ParamGetLong(lpstEnv,
                     lpstParam,
                     &lInfoType) == FALSE)
        return EvalPushOperand(lpstEnv,
                               eTYPE_STRING,
                               lpbyStr);

    if (lInfoType == 4)
    {
        BYTE    abyResult[256];
        int     i, j;

        // Everything except the extension

        // Search for the last slash or colon

        for (i=lpbyStr[0];i>0;i--)
            if (lpbyStr[i] == '/' || lpbyStr[i] == '\\' || lpbyStr[i] == ':')
                break;

        ++i;

        j = 1;
        while (i <= lpbyStr[0])
            abyResult[j++] = lpbyStr[i++];

        abyResult[0] = j - 1;

        return EvalPushOperand(lpstEnv,
                               eTYPE_STRING,
                               lpbyStr);
    }

    // Otherwise just push the full path

    return EvalPushSZOperand(lpstEnv,lpstEnv->lpszFN);
}

BOOL MacroFileNameS
(
    LPENV       lpstEnv
)
{
    if (lpstEnv->bNormal == FALSE)
        return EvalPushSZOperand(lpstEnv,lpstEnv->lpszFN);

    return EvalPushSZOperand(lpstEnv,lpstEnv->lpszNormal);
}

BOOL MacroCopy
(
    LPENV       lpstEnv
)
{
    LPPARAMETER lpstParam;
    LPBYTE      lpbySrc, lpbyDst;

    // The first parameter should be a string

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    // Print out the source and destinations

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbySrc) == FALSE)
        return(TRUE);

    lpstParam = lpstParam->lpstNext;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbyDst) == FALSE)
        return(TRUE);

    if (lpstEnv->bNormal == FALSE)
    {
        if (PascalIsGlobalMacroName(lpbyDst) == FALSE)
        {
            // Assume error on copy to same document

            lpstEnv->lErr = 24;
        }
    }
    else
    {
        // In normal.dot

        if (PascalIsGlobalMacroName(lpbyDst) != FALSE)
        {
            // Assume error on copy to same normal.dot

            lpstEnv->lErr = 24;
        }
    }

    if (lpbySrc[0] > 0 && lpbyDst[0] > 0)
    {
#ifdef SYM_NLM
        SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM

        CopyLogCopy((LPCOPY)lpstEnv->lpvContextData,
                    lpbySrc,
                    lpbyDst);

#ifdef SYM_NLM
        SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM
    }

    return(TRUE);
}

BOOL MacroNameS
(
    LPENV       lpstEnv
)
{
    long        lIndex;
    long        lContext;
    LPPARAMETER lpstParam;
    BYTE        abyName[256];

    // The first parameter should be a string

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    if (ParamGetLong(lpstEnv,
                     lpstParam,
                     &lIndex) == FALSE)
        return(FALSE);

    lpstParam = lpstParam->lpstNext;
    if (ParamGetLong(lpstEnv,
                     lpstParam,
                     &lContext) == FALSE)
        lContext = 0;

    if (lpstEnv->bNormal == FALSE && lContext == 0 ||
        lpstEnv->bNormal == TRUE && lContext != 0)
    {
        // Indicate no macros

        return EvalPushSZOperand(lpstEnv,"");
    }

    if (lIndex == 0)
        lIndex = lpstEnv->lpstModEnv->wMacroIndex + 1;

    if (WD7GetULMacroNameAtIndex(&lpstEnv->stTDTInfo,
                                 (WORD)(lIndex - 1),
                                 abyName) != WD7_STATUS_OK)
    {
        EvalPushSZOperand(lpstEnv,"");
    }

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyName);
}

BOOL MacroDescS
(
    LPENV       lpstEnv
)
{
    LPPARAMETER lpstParam;
    LPBYTE      lpbyStr;
    WORD        wIndex;
    BYTE        abyDesc[256];

    // The first parameter should be numeric

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    if (ParamGetString(lpstEnv,
                       lpstParam,
                       &lpbyStr) == FALSE)
        return(FALSE);

    if (WD7GetMacroInfoAtULName(&lpstEnv->stTDTInfo,
                                lpbyStr,
                                &wIndex,
                                NULL,
                                NULL,
                                NULL,
                                NULL) != WD7_STATUS_OK)
    {
        // Just push an empty string

        return EvalPushSZOperand(lpstEnv,"");
    }

    if (WD7GetULMacroDescAtIndex(&lpstEnv->stTDTInfo,
                                 wIndex,
                                 abyDesc) != WD7_STATUS_OK)
    {
        // Just push an empty string

        return EvalPushSZOperand(lpstEnv,"");
    }

    return EvalPushOperand(lpstEnv,
                           eTYPE_STRING,
                           abyDesc);
}

BOOL CountMacros
(
    LPENV       lpstEnv
)
{
    long        lContext;

    if (ParamGetLong(lpstEnv,
                     ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam,
                     &lContext) == FALSE)
        lContext = 0;

    if (lpstEnv->bNormal == FALSE && lContext == 0 ||
        lpstEnv->bNormal == TRUE && lContext != 0)
    {
        // Indicate no macros

        return EvalPushLongOperand(lpstEnv,0);
    }

    return EvalPushLongOperand(lpstEnv,lpstEnv->stTDTInfo.wNumMCDs);
}

BOOL Insert
(
    LPENV       lpstEnv
)
{
/*
    LPPARAMETER lpstParam;

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    while (lpstParam != NULL)
    {
        switch (lpstParam->eType)
        {
            case eTYPE_LONG:
                printf("%ld ",lpstParam->uData.lValue);
                break;

            case eTYPE_STRING:
            {
                int i;
                LPBYTE lpby;

                lpby = lpstParam->uData.lpbyStr;
                i = *lpby++;

                while (i-- > 0)
                    putchar(*lpby++);

                break;
            }

            case eTYPE_VAR:
            {
                LPVAR   lpstVar = (LPVAR)lpstParam->uData.lpvstVar;

                switch (lpstVar->eType)
                {
                    case eTYPE_LONG:
                        printf("%ld ",lpstParam->uData.lValue);
                        break;

                    case eTYPE_STRING:
                    {
                        int i;
                        LPBYTE lpby;

                        lpby = lpstParam->uData.lpbyStr;
                        i = *lpby++;

                        while (i-- > 0)
                            putchar(*lpby++);

                        break;
                    }

                    default:
                        printf("unknown");
                        break;
                }
                break;
            }

            default:
                printf("unknown");
                break;
        }

        lpstParam = lpstParam->lpstNext;
    }

    putchar('\n');
*/
    return(TRUE);
}

BOOL IsExecuteOnly
(
    LPENV       lpstEnv
)
{
    BOOL        bGlobal;
    LPBYTE      lpbyName;
    long        lResult;

    if (ParamGetString(lpstEnv,
                       ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam,
                       &lpbyName) == FALSE)
        return(FALSE);

    bGlobal = PascalIsGlobalMacroName(lpbyName);
    if (lpstEnv->bNormal == FALSE)
    {
        // Emulating document

        if (bGlobal == FALSE)
            lResult = -1;
        else
            lResult = 0;
    }
    else
    {
        // Emulating normal

        if (bGlobal == FALSE)
            lResult = 0;
        else
            lResult = -1;
    }

    return EvalPushLongOperand(lpstEnv,lResult);
}

BOOL Organizer
(
    LPENV       lpstEnv
)
{
    LPPARAMETER lpstParam;
    BOOL        bCopy;
    BOOL        bRename;
    LPBYTE      lpbySrc;
    LPBYTE      lpbyDst;
    LPBYTE      lpbyName;
    LPBYTE      lpbyNewName;
    BYTE        abySrc[256];
    BYTE        abyDst[256];
    int         i, j, nLen;
    long        lTab;

    lpstParam = ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam;

    // Get rename, copy, source, destination, name, and new name,
    //  and Tab type

    bRename = FALSE;
    bCopy = FALSE;
    lpbySrc = NULL;
    lpbyDst = NULL;
    lpbyName = NULL;
    lpbyNewName = NULL;
    lTab = 3; // Assume macros
    while (lpstParam != NULL)
    {
        if (lpstParam->eType == eTYPE_DIALOG_FIELD)
        {
            switch (lpstParam->uData.wDlgField)
            {
                case eWDDLG_Copy:
                    bCopy = TRUE;
                    break;

                case eWDDLG_Rename:
                    bRename = TRUE;
                    break;

                case eWDDLG_Source:
                    if (ParamGetString(lpstEnv,
                                       lpstParam->lpstNext,
                                       &lpbySrc) == FALSE)
                        return(TRUE);

                    break;

                case eWDDLG_Destination:
                    if (ParamGetString(lpstEnv,
                                       lpstParam->lpstNext,
                                       &lpbyDst) == FALSE)
                        return(TRUE);

                    break;

                case eWDDLG_Name:
                    if (ParamGetString(lpstEnv,
                                       lpstParam->lpstNext,
                                       &lpbyName) == FALSE)
                        return(TRUE);

                    break;

                case eWDDLG_NewName:
                    if (ParamGetString(lpstEnv,
                                       lpstParam->lpstNext,
                                       &lpbyNewName) == FALSE)
                        return(TRUE);

                    break;

                case eWDDLG_Tab:
                    if (ParamGetLong(lpstEnv,
                                     lpstParam->lpstNext,
                                     &lTab) == FALSE)
                        return(TRUE);

                    break;

                default:
                    break;
            }
        }

        lpstParam = lpstParam->lpstNext;
    }

    if (lTab != 3)
    {
        // Not a macro copy, ignore it

        return(TRUE);
    }

    if (bRename == TRUE)
    {
        if (lpbySrc == NULL || lpbyName == NULL || lpbyNewName == NULL)
        {
            // Not enough parameters, so ignore it

            return(TRUE);
        }

        // Pretend the source of the rename is actually the destination

        if (lpbySrc[0] + 1 + lpbyNewName[0] > 255)
            return(TRUE);

        // Copy it to the destination

        for (i=1;i<=lpbySrc[0];i++)
            abyDst[i] = lpbySrc[i];

        // Now copy the new name

        abyDst[i] = ':';

        for (j=1;j<=lpbyNewName[0];j++)
            abyDst[++i] = lpbyNewName[j];

        abyDst[0] = i;

        // See if it is a global or local rename

        nLen = 0;
        if (PascalIsGlobalMacroName(abyDst) == FALSE)
        {
            // Make the source global

            // Copy the global filename

            for (i=0;lpstEnv->lpszNormal[i];i++)
                abySrc[++nLen] = lpstEnv->lpszNormal[i];
        }
        else
        {
            // Make the source local

            // Copy the local filename

            for (i=0;lpstEnv->lpszFN[i];i++)
                abySrc[++nLen] = lpstEnv->lpszFN[i];
        }

        abySrc[++nLen] = ':';

        if (nLen + lpbyName[0] > 255)
            return(TRUE);

        // Copy the source name

        for (i=1;i<=lpbyName[0];i++)
            abySrc[++nLen] = lpbyName[i];

        abySrc[0] = nLen;
    }
    else
    {
        if (bCopy == FALSE ||
            lpbySrc == NULL ||
            lpbyDst == NULL ||
            lpbyName == NULL)
        {
            // Not a copy or not enough parameters, so ignore it

            return(TRUE);
        }

        // Copy the source name

        for (i=1;i<=lpbySrc[0];i++)
            abySrc[i] = lpbySrc[i];

        nLen = lpbySrc[0];
        if (nLen > 253)
            return(TRUE);

        abySrc[++nLen] = ':';

        for (i=1;i<=lpbyName[0] && nLen < 255;i++)
            abySrc[++nLen] = lpbyName[i];

        abySrc[0] = nLen;

        // Copy the destination name

        for (i=1;i<=lpbyDst[0];i++)
            abyDst[i] = lpbyDst[i];

        nLen = lpbyDst[0];
        if (nLen > 253)
            return(TRUE);

        abyDst[++nLen] = ':';

        for (i=1;i<=lpbyName[0] && nLen < 255;i++)
            abyDst[++nLen] = lpbyName[i];

        abyDst[0] = nLen;
    }

#ifdef SYM_NLM
    SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM

    CopyLogCopy((LPCOPY)lpstEnv->lpvContextData,
                abySrc,
                abyDst);

#ifdef SYM_NLM
    SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM

    return(TRUE);
}

BOOL GetDocumentVarS
(
    LPENV       lpstEnv
)
{
    // Assume the virus uses it to check to see whether it is already
    //  installed

    if (lpstEnv->bNormal == FALSE)
    {
        // We are in the document, so we should assume that it is
        //  installed in the document

        return EvalPushSZOperand(lpstEnv,"Yes");
    }

    // We must be in NORMAL.DOT, so we should assume that it is
    //  not installed in the document

    return EvalPushSZOperand(lpstEnv,"");
}

BOOL GetProfileStringS
(
    LPENV       lpstEnv
)
{
    // Assume the virus uses it to check to see whether it is already
    //  installed

    if (lpstEnv->bNormal == FALSE)
    {
        // We are in the document, so we should assume that it is
        //  installed in the document

        return EvalPushSZOperand(lpstEnv,"0");
    }

    // We must be in NORMAL.DOT, so we should assume that it is
    //  not installed in the document

    return EvalPushSZOperand(lpstEnv,"");
}

BOOL AppInfoS
(
    LPENV       lpstEnv
)
{
    long        lType;

    if (ParamGetLong(lpstEnv,
                     ((LPBUILTIN)ControlTop(lpstEnv))->lpstFirstParam,
                     &lType) == FALSE)
        return EvalPushSZOperand(lpstEnv,"");

    return EvalPushSZOperand(lpstEnv,"Italiano");
}

void StateBUILTIN_SUB_CALL
(
    LPENV       lpstEnv
)
{
    LPBUILTIN       lpstBuiltIn;
    BOOL            bResult;

    assert(StateTop(lpstEnv) == esBUILTIN_SUB_CALL);

    RedoToken(lpstEnv);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esBUILTIN_SUB_CALL)

        StatePop(lpstEnv);
        return;
    }

    lpstBuiltIn = (LPBUILTIN)ControlTop(lpstEnv);

    assert(lpstBuiltIn->ecType == ectBUILTIN);

    // Call off

    switch (lpstBuiltIn->eWDCMD)
    {
        case eWDCMD_MacroCopy:
            bResult = MacroCopy(lpstEnv);
            break;

        case eWDCMD_Insert:
            bResult = Insert(lpstEnv);
            break;

        case eWDCMD_Organizer:
            bResult = Organizer(lpstEnv);
            break;

        default:
            bResult = TRUE;
            break;
    }

    if (bResult == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Delete parameters

    if (ControlFreeParameterList(lpstEnv,
                                 lpstBuiltIn->lpstFirstParam) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstBuiltIn->lpstFirstParam = NULL;

    // Free control top

    ControlDestroyTopControl(lpstEnv);

    // pop (esBUILTIN_SUB_CALL)

    StatePop(lpstEnv);
}

void StateBUILTIN_FUNCTION_CALL
(
    LPENV           lpstEnv
)
{
    LPBUILTIN       lpstBuiltIn;
    BOOL            bResult;

    assert(StateTop(lpstEnv) == esBUILTIN_FUNCTION_CALL);

    RedoToken(lpstEnv);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esBUILTIN_FUNCTION_CALL)

        StatePop(lpstEnv);
        return;
    }

    lpstBuiltIn = (LPBUILTIN)ControlTop(lpstEnv);

    assert(lpstBuiltIn->ecType == ectBUILTIN);

    // Call off

    switch (lpstBuiltIn->eWDCMD)
    {
        case eWDCMD_CountWindows:
        {
            bResult = EvalPushLongOperand(lpstEnv,1);
            break;
        }

        case eWDCMD_CountMacros:
        {
            bResult = CountMacros(lpstEnv);
            break;
        }

        case eWDCMD_MacroNameS:
        {
            bResult = MacroNameS(lpstEnv);
            break;
        }

        case eWDCMD_Val:
            bResult = Val(lpstEnv);
            break;

        case eWDCMD_FileNameS:
        case eWDCMD_WindowNameS:
        case eWDCMD_FileNameFromWindowS:
            bResult = FileNameS(lpstEnv);
            break;

        case eWDCMD_FileNameInfoS:
            bResult = FileNameInfoS(lpstEnv);
            break;

        case eWDCMD_MacroFileNameS:
            bResult = MacroFileNameS(lpstEnv);
            break;

        case eWDCMD_MacroDescS:
            bResult = MacroDescS(lpstEnv);
            break;

        case eWDCMD_Len:
            bResult = Len(lpstEnv);
            break;

        case eWDCMD_ChrS:
            bResult = ChrS(lpstEnv);
            break;

        case eWDCMD_StrS:
            bResult = StrS(lpstEnv);
            break;

        case eWDCMD_LeftS:
            bResult = LeftS(lpstEnv);
            break;

        case eWDCMD_RightS:
            bResult = RightS(lpstEnv);
            break;

        case eWDCMD_MidS:
            bResult = MidS(lpstEnv);
            break;

        case eWDCMD_LCaseS:
            bResult = LCaseS(lpstEnv);
            break;

        case eWDCMD_UCaseS:
            bResult = UCaseS(lpstEnv);
            break;

        case eWDCMD_LTrimS:
            bResult = LTrimS(lpstEnv);
            break;

        case eWDCMD_RTrimS:
            bResult = RTrimS(lpstEnv);
            break;

        case eWDCMD_InStr:
            bResult = InStr(lpstEnv);
            break;

        case eWDCMD_IsExecuteOnly:
            bResult = IsExecuteOnly(lpstEnv);
            break;

        case eWDCMD_GetDocumentVarS:
            bResult = GetDocumentVarS(lpstEnv);
            break;

        case eWDCMD_GetProfileStringS:
            bResult = GetProfileStringS(lpstEnv);
            break;

        case eWDCMD_AppInfoS:
            bResult = AppInfoS(lpstEnv);
            break;

        case eWDCMD_Day:
            // For twno.h

            bResult = EvalPushLongOperand(lpstEnv,1);
            break;

        case eWDCMD_StringS:
        case eWDCMD_TimeS:
        case eWDCMD_SelectionS:
        case eWDCMD_GetBookmarkS:
        case eWDCMD_BookmarkNameS:
        case eWDCMD_MergeFieldNameS:
        case eWDCMD_StyleNameS:
        case eWDCMD_GetAutoTextS:
        case eWDCMD_AutoTextNameS:
        case eWDCMD_TabLeaderS:
        case eWDCMD_DDERequestS:
        case eWDCMD_FontS:
        case eWDCMD_LanguageS:
        case eWDCMD_FilesS:
        case eWDCMD_InputS:
        case eWDCMD_InputBoxS:
        case eWDCMD_DateS:
        case eWDCMD_StyleDescS:
        case eWDCMD_FoundFileNameS:
        case eWDCMD_WW2_MenuMacroS:
        case eWDCMD_WW2_MenuTextS:
        case eWDCMD_KeyMacroS:
        case eWDCMD_WW2_GetToolMacroS:
        case eWDCMD_GetDirectoryS:
        case eWDCMD_EnvironS:
        case eWDCMD_DOSToWinS:
        case eWDCMD_WinToDOSS:
        case eWDCMD_GetSystemInfoS:
        case eWDCMD_GetPrivateProfileStringS:
        case eWDCMD_MailMergeDataSourceS:
        case eWDCMD_DlgTextS:
        case eWDCMD_DlgFocusS:
        case eWDCMD_WW2_FilesS:
        case eWDCMD_DlgFilePreviewS:
        case eWDCMD_MenuItemTextS:
        case eWDCMD_MenuItemMacroS:
        case eWDCMD_MenuTextS:
        case eWDCMD_AnnotationRefFromSelS:
        case eWDCMD_GetFormResultS:
        case eWDCMD_MacroNameFromWindowS:
        case eWDCMD_GetFieldDataS:
        case eWDCMD_MacScriptS:
        case eWDCMD_MacIDS:
        case eWDCMD_GetTextS:
        case eWDCMD_GetAddInNameS:
        case eWDCMD_ToolsRevisionAuthorS:
        case eWDCMD_ToolsRevisionDateS:
        case eWDCMD_DefaultDirS:
        case eWDCMD_ToolbarNameS:
        case eWDCMD_GetAutoCorrectS:
        case eWDCMD_GetMergeFieldS:
        case eWDCMD_ConverterS:
        case eWDCMD_GetDocumentVarNameS:
        case eWDCMD_CleanStringS:
        case eWDCMD_PathFromWinPathS:
        case eWDCMD_PathFromMacPathS:
        case eWDCMD_SelectionFileNameS:
        case eWDCMD_ToolbarButtonMacroS:
        case eWDCMD_FieldSeparatorS:
        case eWDCMD_AOCEGetRecipientS:
        case eWDCMD_AOCEGetSenderS:
        case eWDCMD_AOCEGetSubjectS:
        case eWDCMD_FileTypeS:
        case eWDCMD_FileCreatorS:
        case eWDCMD_GetAddressS:
        case eWDCMD_GetDocumentPropertyS:
        case eWDCMD_DocumentPropertyNameS:
        case eWDCMD_GetAutoCorrectExceptionS:
        case eWDCMD_HanCharSetS:
        case eWDCMD_InputBS:
        case eWDCMD_LeftBS:
        case eWDCMD_MidBS:
        case eWDCMD_RightBS:
        case eWDCMD_RomanCharSetS:
        case eWDCMD_StrConvS:
            bResult = EvalPushSZOperand(lpstEnv,"");
            break;

        default:
            bResult = EvalPushLongOperand(lpstEnv,0);
            break;
    }

    // Ignore the error

//    if (bResult == FALSE)
//    {
//        StateNew(lpstEnv,esERROR);
//        return;
//    }

    // Delete parameters

    if (ControlFreeParameterList(lpstEnv,
                                 lpstBuiltIn->lpstFirstParam) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    lpstBuiltIn->lpstFirstParam = NULL;

    // Free control top

    ControlDestroyTopControl(lpstEnv);

    // pop (esBUILTIN_FUNCTION_CALL)

    StatePop(lpstEnv);
}

#endif // #ifdef MACROHEU
