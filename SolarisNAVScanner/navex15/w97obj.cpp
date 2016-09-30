//************************************************************************
//
// $Header:   S:/NAVEX/VCS/W97OBJ.CPv   1.3   09 Dec 1998 17:45:10   DCHI  $
//
// Description:
//  Word 97 object handling functions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/W97OBJ.CPv  $
// 
//    Rev 1.3   09 Dec 1998 17:45:10   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.2   12 Oct 1998 13:43:42   DCHI
// Added Import/Export, VBE, VBComponent, VBProject, etc. support.
// 
//    Rev 1.1   13 Jan 1998 11:16:58   DCHI
// Organizer now checks to be sure .Tab == 3 before logging copy.
// 
//    Rev 1.0   15 Oct 1997 13:20:38   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "o97api.h"
#include "o97env.h"

#include "w97str.h"

#include "wbutil.h"
#include "copy.h"

LPSTR glpszO97Normal = "\\NORMAL.DOT";
LPSTR glpszO97FN = "FN.DOC";

// Custom object IDs

#define W97_GLOBAL_VBPROJECT        ((NUM_W97_STR) + 1)
#define W97_LOCAL_VBPROJECT         ((NUM_W97_STR) + 2)
#define W97_GLOBAL_VBCOMPONENTS     ((NUM_W97_STR) + 3)
#define W97_LOCAL_VBCOMPONENTS      ((NUM_W97_STR) + 4)

#define W97_SPECIAL_OBJ_MASK        0x00FFFFFF
#define W97_SPECIAL_CAT_MASK        0xFF000000

#define W97_LOCAL_MODULE_INDEX_CAT  0x01000000
#define W97_GLOBAL_MODULE_INDEX_CAT 0x02000000


//////////////////////////////////////////////////////////////////////
// Word 97 dialog info

typedef struct tagO97_DLG_FILESAVEAS
{
    DWORD       dwSize;
    DWORD       dwCmd;

    BYTE        byFormat;
} O97_DLG_FILESAVEAS_T, FAR *LPO97_DLG_FILESAVEAS;

typedef struct tagO97_DLG_FILENEW
{
    DWORD       dwSize;
    DWORD       dwCmd;

    BYTE        byNewTemplate;
} O97_DLG_FILENEW_T, FAR *LPO97_DLG_FILENEW;



BOOL Word97CountMacros
(
    LPO97_ENV       lpstEnv
)
{
    long            lContext;

    if (O97ParamGetLong(lpstEnv,0,&lContext) == FALSE)
        lContext = 0;

    if (lpstEnv->bNormal == FALSE && lContext == 0 ||
        lpstEnv->bNormal == TRUE && lContext != 0)
    {
        // Indicate no macros

        return O97EvalPushLongOperand(lpstEnv,0);
    }

    return O97EvalPushLongOperand(lpstEnv,(long)lpstEnv->dwNumModules);
}

BOOL Word97FileNameInfoS
(
    LPO97_ENV       lpstEnv
)
{
    LPBYTE          lpbyFileName;
    long            lInfoType;

    // Get the filename

    if (O97ParamGetString(lpstEnv,0,&lpbyFileName) != FALSE &&
        O97ParamGetLong(lpstEnv,1,&lInfoType) != FALSE)
    {
        BYTE    abyResult[256];
        int     iFileNameIdx;
        int     iBeginIdx, iEndIdx;
        int     i, j;

        // Search for the last slash or colon

        for (i=lpbyFileName[0];i>0;i--)
            if (lpbyFileName[i] == '/' ||
                lpbyFileName[i] == '\\' ||
                lpbyFileName[i] == ':')
                break;

        iFileNameIdx = ++i;

        iBeginIdx = 0;
        if (lInfoType == 3)
        {
            // The filename including the extension

            iBeginIdx = iFileNameIdx;
            iEndIdx = lpbyFileName[0] + 1;
        }
        else
        if (lInfoType == 4)
        {
            // The filename without the extension

            while (i <= lpbyFileName[0])
            {
                if (lpbyFileName[i] == '.')
                    break;

                ++i;
            }

            iBeginIdx = iFileNameIdx;
            iEndIdx = i;
        }
        else
        if (lInfoType == 5)
        {
            iBeginIdx = 1;
            iEndIdx = iFileNameIdx;
        }

        if (iBeginIdx != 0)
        {
            abyResult[0] = iEndIdx - iBeginIdx;
            for (i=iBeginIdx,j=1;i<iEndIdx;i++,j++)
                abyResult[j] = lpbyFileName[i];

            return O97EvalPushOperand(lpstEnv,
                                      eO97_TYPE_STRING,
                                      abyResult);
        }
    }

    // Otherwise just push the full path

    return O97EvalPushOperand(lpstEnv,
                              eO97_TYPE_STRING,
                              glpszO97FN);
}

BOOL Word97IsExecuteOnly
(
    LPO97_ENV       lpstEnv
)
{
    BOOL        bGlobal;
    LPBYTE      lpbyName;
    long        lResult;

    if (O97ParamGetString(lpstEnv,0,&lpbyName) == FALSE)
        bGlobal = FALSE;
    else
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

    return O97EvalPushLongOperand(lpstEnv,lResult);
}

BOOL Word97MacroDescS
(
    LPO97_ENV       lpstEnv
)
{
    LPBYTE          lpbyMacroName;
    BYTE            abyMain[] = { 4, 'm', 'a', 'i', 'n' };
    BYTE            abyDesc[256];
    LPSS_STREAM     lpstStream;
    BOOL            bResult = TRUE;

    if (O97ParamGetString(lpstEnv,0,&lpbyMacroName) == FALSE)
        return(FALSE);

    if (SSAllocStreamStruct(lpstEnv->lpstRoot,
                            &lpstStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
        return(FALSE);

    // Get the macro by the given name

    if (O97OpenModuleStreamByName(lpstEnv->lpstRoot,
                                  lpstStream,
                                  lpstEnv->dwVBAChildEntry,
                                  lpbyMacroName) != FALSE)
    {
        if (O97ModuleGetDesc(lpstStream,
                             NULL,
                             abyMain,
                             abyDesc,
                             sizeof(abyDesc)) != FALSE)
            bResult = TRUE;
        else
            bResult = FALSE;
    }
    else
        bResult = FALSE;

    if (bResult == FALSE)
    {
        // Push an empty string

        abyDesc[0] = 0;
    }

    bResult = O97EvalPushOperand(lpstEnv,
                                 eO97_TYPE_STRING,
                                 abyDesc);

    if (SSFreeStreamStruct(lpstStream) != SS_STATUS_OK)
        bResult = FALSE;

    return(bResult);
}

BOOL Word97MacroNameS
(
    LPO97_ENV       lpstEnv
)
{
    long            lIndex;
    long            lContext;
    BYTE            abyName[256];

    // The first parameter should be a long

    if (O97ParamGetLong(lpstEnv,0,&lIndex) == FALSE)
        return(FALSE);

    // The second parameter if any should be a long

    if (O97ParamGetLong(lpstEnv,1,&lContext) == FALSE)
        lContext = 0;

    if (lpstEnv->bNormal == FALSE && lContext == 0 ||
        lpstEnv->bNormal == TRUE && lContext != 0)
    {
        // Indicate no macros

        return O97EvalPushSZOperand(lpstEnv,NULL);
    }

    if (lIndex == 0)
        lIndex = lpstEnv->lpstModEnv->dwModuleIndex + 1;

    if (O97GetModuleNameAtIndex(lpstEnv->lpstRoot,
                                lpstEnv->dwVBAChildEntry,
                                (DWORD)(lIndex - 1),
                                abyName) == FALSE)
    {
        O97EvalPushSZOperand(lpstEnv,NULL);
    }

    return O97EvalPushSZOperand(lpstEnv,(LPSTR)abyName);
}

BOOL Word97MacroCopy
(
    LPO97_ENV       lpstEnv
)
{
    LPBYTE          lpbySrc, lpbyDst;

    if (O97ParamGetString(lpstEnv,0,&lpbySrc) == FALSE ||
        O97ParamGetString(lpstEnv,1,&lpbyDst) == FALSE)
    {
        // Just ignore it

        return(TRUE);
    }

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

/*
    {
        int i;

        printf("macrocopy ");
        for (i=1;i<=lpbySrc[0];i++)
            putchar(lpbySrc[i]);
        printf(",");
        for (i=1;i<=lpbyDst[0];i++)
            putchar(lpbyDst[i]);
        printf("\n");
    }
*/

#ifdef SYM_NLM
    SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM

    CopyLogCopy((LPCOPY)lpstEnv->lpvContextData,
                lpbySrc,
                lpbyDst);

#ifdef SYM_NLM
    SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM

    return(TRUE);
}

BOOL Word97Organizer
(
    LPO97_ENV       lpstEnv,
    BOOL            bCopy,
    BOOL            bRename
)
{
    LPBYTE      lpbySrc;
    LPBYTE      lpbyDst;
    LPBYTE      lpbyName;
    LPBYTE      lpbyNewName;
    BYTE        abySrc[256];
    BYTE        abyDst[256];
    int         i, j, nLen;
    WORD        wNamedParamID;
    long        lTab;

    // Get rename, copy, source, destination, name, and new name,
    //  and Tab type

    lpbySrc = NULL;
    lpbyDst = NULL;
    lpbyName = NULL;
    lpbyNewName = NULL;
    lTab = 3; // Assume macros
    for (i=0;i<lpstEnv->nNumParams;i++)
    {
        if (O97ParamGetNamedParamID(lpstEnv,
                                    i,
                                    &wNamedParamID) == FALSE)
            return(FALSE);

        if (wNamedParamID != O97_UNNAMED_PARAM_ID)
        {
            if (wNamedParamID == (WORD)eO97_FIXED_ID_NAME)
            {
                if (O97ParamGetString(lpstEnv,i,&lpbyName) == FALSE)
                    return(TRUE);
            }
            else
            if (wNamedParamID == (WORD)eO97_FIXED_ID_TAB)
            {
                if (O97ParamGetLong(lpstEnv,i,&lTab) == FALSE)
                    return(TRUE);
            }
            else
            {
                EW97_STR_T  eParam;

                eParam = (EW97_STR_T)O97GetKnownIdentEnum(lpstEnv,
                                                          wNamedParamID);

                switch (eParam)
                {
                    case eW97_STR_copy:
                        bCopy = TRUE;
                        break;

                    case eW97_STR_rename:
                        bRename = TRUE;
                        break;

                    case eW97_STR_source:
                        if (O97ParamGetString(lpstEnv,i,&lpbySrc) == FALSE)
                            return(TRUE);
                        break;

                    case eW97_STR_destination:
                        if (O97ParamGetString(lpstEnv,i,&lpbyDst) == FALSE)
                            return(TRUE);
                        break;

                    case eW97_STR_newname:
                        if (O97ParamGetString(lpstEnv,i,&lpbyNewName) == FALSE)
                            return(TRUE);
                        break;

                    default:
                        break;
                }
            }
        }
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

            for (i=0;glpszO97Normal[i];i++)
                abySrc[++nLen] = glpszO97Normal[i];
        }
        else
        {
            // Make the source local

            // Copy the local filename

            for (i=0;glpszO97FN[i];i++)
                abySrc[++nLen] = glpszO97FN[i];
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

    if (lpstEnv->bNormal == TRUE)
    {
        // In normal.dot

        if (PascalIsGlobalMacroName(abyDst) != FALSE)
        {
            // Assume error on copy to global

            lpstEnv->lErr = 5940;
        }
    }

/*
    {
        int i;

        printf("Organizer ");
        for (i=1;i<=abySrc[0];i++)
            putchar(abySrc[i]);
        printf(",");
        for (i=1;i<=abyDst[0];i++)
            putchar(abyDst[i]);
        printf("\n");
    }
*/

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

BOOL Word97MacroFileNameS
(
    LPO97_ENV       lpstEnv
)
{
    if (lpstEnv->bNormal == FALSE)
        return O97EvalPushSZOperand(lpstEnv,glpszO97FN);

    return O97EvalPushSZOperand(lpstEnv,glpszO97Normal);
}

BOOL Word97LCaseS
(
    LPO97_ENV       lpstEnv
)
{
    BYTE            abyStr[256];
    LPBYTE          lpbyStr;

    if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    // The first parameter should be a string

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) == FALSE)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    PascalStrToLower(lpbyStr,abyStr);

    return O97EvalPushOperand(lpstEnv,eO97_TYPE_STRING,abyStr);
}


BOOL Word97UCaseS
(
    LPO97_ENV       lpstEnv
)
{
    BYTE            abyStr[256];
    LPBYTE          lpbyStr;
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    // Determine whether the parameter is an object

    if (O97ParamGetValue(lpstEnv,0,&eType,&lpvData) == FALSE)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    if (eType == eO97_TYPE_OBJECT_ID)
    {
        switch (*(LPDWORD)lpvData)
        {
            case eW97_STR_normaltemplate:
                return O97EvalPushSZOperand(lpstEnv,glpszO97Normal+1);

            default:
                break;
        }
    }

    // The first parameter should be a string

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) == FALSE)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    PascalStrToUpper(lpbyStr,abyStr);

    return O97EvalPushOperand(lpstEnv,eO97_TYPE_STRING,abyStr);
}


BOOL Word97Len
(
    LPO97_ENV       lpstEnv
)
{
    long            lLen;
    LPBYTE          lpbyStr;

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) == FALSE)
        lLen = 0;
    else
        lLen = lpbyStr[0];

    return O97EvalPushLongOperand(lpstEnv,lLen);
}


BOOL Word97Val
(
    LPO97_ENV       lpstEnv
)
{
    LPBYTE          lpbyStr;
    long            lValue;

    if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        return O97EvalPushLongOperand(lpstEnv,0);

    // Convert a string to a number

    // The first parameter should be a string

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) == FALSE)
        lValue = 0;
    else
        lValue = PascalStrToLong(lpbyStr);

    return O97EvalPushLongOperand(lpstEnv,lValue);
}


BOOL Word97StrS
(
    LPO97_ENV       lpstEnv
)
{
    long            lValue;
    BYTE        abyStr[16];

    if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    // The first parameter should be numeric
    // Only take care of longs

    if (O97ParamGetLong(lpstEnv,0,&lValue) == FALSE)
        lValue = 0;

    // Convert a number to a string

    LongToPascalStr(lValue,abyStr);

    return O97EvalPushOperand(lpstEnv,
                              eO97_TYPE_STRING,
                              abyStr);
}


BOOL Word97ChrS
(
    LPO97_ENV       lpstEnv
)
{
    long            lValue;
    BYTE            abyChr[2];

    if (O97ParamGetLong(lpstEnv,0,&lValue) == FALSE)
        abyChr[0] = 0;
    else
    {
        abyChr[0] = 1;
        abyChr[1] = (BYTE)lValue;
    }

    return O97EvalPushOperand(lpstEnv,
                              eO97_TYPE_STRING,
                              abyChr);
}


BOOL Word97LeftS
(
    LPO97_ENV       lpstEnv
)
{
    BYTE            abyStr[256];
    LPBYTE          lpbyStr;
    long            lCount;

    if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    // 1. The first parameter should be a string
    // 2. The second parameter should be the count of the number
    //    of leftmost characters to get.

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) == FALSE ||
        O97ParamGetLong(lpstEnv,1,&lCount) == FALSE)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    PascalStrLeft(lpbyStr,lCount,abyStr);

    return O97EvalPushOperand(lpstEnv,eO97_TYPE_STRING,abyStr);
}


BOOL Word97MidS
(
    LPO97_ENV       lpstEnv
)
{
    BYTE            abyStr[256];
    LPBYTE          lpbyStr;
    long            lStart, lCount;

    if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    // 1. The first parameter should be a string
    // 2. The second parameter should be the count of the number
    //    of rightmost characters to get.

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) == FALSE ||
        O97ParamGetLong(lpstEnv,1,&lStart) == FALSE)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    if (O97ParamGetLong(lpstEnv,2,&lCount) == FALSE)
        lCount = *lpbyStr;

    PascalStrMid(lpbyStr,lStart,lCount,abyStr);

    return O97EvalPushOperand(lpstEnv,eO97_TYPE_STRING,abyStr);
}


BOOL Word97RightS
(
    LPO97_ENV       lpstEnv
)
{
    BYTE            abyStr[256];
    LPBYTE          lpbyStr;
    long            lCount;

    if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    // 1. The first parameter should be a string
    // 2. The second parameter should be the count of the number
    //    of rightmost characters to get.

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) == FALSE ||
        O97ParamGetLong(lpstEnv,1,&lCount) == FALSE)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    PascalStrRight(lpbyStr,lCount,abyStr);

    return O97EvalPushOperand(lpstEnv,eO97_TYPE_STRING,abyStr);
}


BOOL Word97LTrimS
(
    LPO97_ENV       lpstEnv
)
{
    BYTE            abyStr[256];
    LPBYTE          lpbyStr;

    if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    // The parameter should be a string

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) == FALSE)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    PascalStrLTrim(lpbyStr,abyStr);

    return O97EvalPushOperand(lpstEnv,eO97_TYPE_STRING,abyStr);
}


BOOL Word97RTrimS
(
    LPO97_ENV       lpstEnv
)
{
    BYTE            abyStr[256];
    LPBYTE          lpbyStr;

    if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    // The parameter should be a string

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) == FALSE)
        return O97EvalPushSZOperand(lpstEnv,NULL);

    PascalStrRTrim(lpbyStr,abyStr);

    return O97EvalPushOperand(lpstEnv,eO97_TYPE_STRING,abyStr);
}


BOOL Word97VBComponents
(
    LPO97_ENV       lpstEnv,
    DWORD           dwVBProjectID
)
{
    LPBYTE          lpbyStr;
    DWORD           dwObjectID;

    // The parameter should be a string

    if (O97ParamGetString(lpstEnv,0,&lpbyStr) != FALSE)
    {
        // Now get the index of the module

        if (SSGetStreamIndexCB(lpstEnv->lpstRoot,
                               O97OpenVBA5ModuleULCB,
                               lpstEnv->dwVBAChildEntry,
                               lpbyStr,
                               &dwObjectID) == FALSE)
            dwObjectID = 0;
    }
    else
    if (O97ParamGetLong(lpstEnv,0,(LPLONG)&dwObjectID) != FALSE)
        --dwObjectID;
    else
        dwObjectID = 0;

    dwObjectID &= W97_SPECIAL_OBJ_MASK;
    if (dwVBProjectID == W97_LOCAL_VBPROJECT)
        dwObjectID |= W97_LOCAL_MODULE_INDEX_CAT;
    else
        dwObjectID |= W97_GLOBAL_MODULE_INDEX_CAT;

    return O97EvalPushOperand(lpstEnv,
                              eO97_TYPE_OBJECT_ID,
                              &dwObjectID);
}


BOOL Word97Import
(
    LPO97_ENV       lpstEnv,
    DWORD           dwObjectID
)
{
    LPBYTE          lpabypsSrcFileName;
    WORD            wFlags;

    if (O97ParamGetString(lpstEnv,0,&lpabypsSrcFileName) == FALSE)
        return(TRUE);

    if (dwObjectID == W97_LOCAL_VBCOMPONENTS)
    {
        // Import to local

        wFlags = COPY_FLAG_IMPORT_TO_LOCAL;
    }
    else
    {
        // Import to global

        wFlags = COPY_FLAG_IMPORT_TO_GLOBAL;
    }

#ifdef SYM_NLM
    SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM

    CopyLogImport((LPCOPY)lpstEnv->lpvContextData,
                  wFlags,
                  lpabypsSrcFileName);

#ifdef SYM_NLM
    SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM

    return(TRUE);
}


BOOL Word97Export
(
    LPO97_ENV       lpstEnv,
    DWORD           dwModuleIndex
)
{
    LPBYTE          lpabypsDstFileName;
    BYTE            abypsModuleName[256];
    int             i, nLen;
    WORD            wFlags;

    if (O97ParamGetString(lpstEnv,0,&lpabypsDstFileName) == FALSE)
        return(TRUE);

    if (O97GetModuleNameAtIndex(lpstEnv->lpstRoot,
                                lpstEnv->dwVBAChildEntry,
                                (DWORD)(dwModuleIndex &
                                        W97_SPECIAL_OBJ_MASK),
                                abypsModuleName) == FALSE)
        return(TRUE);

    // Convert the module name to a Pascal string

    for (i=0;abypsModuleName[i];i++);

    nLen = i;
    while (i-- > 0)
        abypsModuleName[i + 1] = abypsModuleName[i];

    abypsModuleName[0] = (BYTE)nLen;

    if ((dwModuleIndex & W97_SPECIAL_CAT_MASK) ==
        W97_LOCAL_MODULE_INDEX_CAT)
    {
        // Export from local

        wFlags = COPY_FLAG_EXPORT_FROM_LOCAL;
    }
    else
    {
        // Export from global

        wFlags = COPY_FLAG_EXPORT_FROM_GLOBAL;
    }

#ifdef SYM_NLM
    SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM

    CopyLogExport((LPCOPY)lpstEnv->lpvContextData,
                  wFlags,
                  lpabypsDstFileName,
                  abypsModuleName);

#ifdef SYM_NLM
    SSProgress(lpstEnv->lpvRootCookie);
#endif // #ifdef SYM_NLM

    return(TRUE);
}


BOOL Word97CountOfLines
(
    LPO97_ENV       lpstEnv,
    DWORD           dwModuleIndex
)
{
    long            lCount;

    if ((dwModuleIndex & W97_SPECIAL_CAT_MASK) ==
        W97_LOCAL_MODULE_INDEX_CAT)
    {
        // Get the line count of a local module

        if (lpstEnv->bNormal == FALSE)
        {
            // Not executing as normal, so return non-zero

            lCount = 10;
        }
        else
        {
            // Indicate no lines in the normal module

            lCount = 0;
        }
    }
    else
    {
        // Get the line count of a global module

        if (lpstEnv->bNormal != FALSE)
        {
            // Executing as normal, so return non-zero

            lCount = 10;
        }
        else
        {
            // Indicate no lines in the normal module

            lCount = 0;
        }
    }

    return O97EvalPushLongOperand(lpstEnv,lCount);
}


BOOL Word97ObjectEvaluate
(
    LPO97_ENV       lpstEnv,
    WORD            wNumParams,
    LPBOOL          lpbKnown
)
{
    DWORD           dwObjectID;
    EW97_STR_T      eObject;

    (void)wNumParams;

    eObject = (EW97_STR_T)O97GetKnownIdentEnum(lpstEnv,lpstEnv->wOpID);

    if ((WORD)eObject == (WORD)0xFFFF)
    {
        // Unknown

        *lpbKnown = FALSE;
        return(TRUE);
    }

    // Assume known

    *lpbKnown = TRUE;

    // Evaluate the top level object

    switch (eObject)
    {
        case eW97_STR_wordbasic:
        case eW97_STR_activedocument:
        case eW97_STR_normaltemplate:
            dwObjectID = eObject;

            // Push the object

            O97EvalPushOperand(lpstEnv,
                               eO97_TYPE_OBJECT_ID,
                               &dwObjectID);
            return(TRUE);

        case eW97_STR_macrocontainer:
            if (lpstEnv->bNormal == FALSE)
                dwObjectID = eW97_STR_activedocument;
            else
                dwObjectID = eW97_STR_normaltemplate;

            // Push the object

            O97EvalPushOperand(lpstEnv,
                               eO97_TYPE_OBJECT_ID,
                               &dwObjectID);
            return(TRUE);

        case eW97_STR_chr:
            return Word97ChrS(lpstEnv);

        case eW97_STR_left:
            return Word97LeftS(lpstEnv);

        case eW97_STR_lcase:
            return Word97LCaseS(lpstEnv);

        case eW97_STR_ltrim:
            return Word97LTrimS(lpstEnv);

        case eW97_STR_mid:
            return Word97MidS(lpstEnv);

        case eW97_STR_right:
            return Word97RightS(lpstEnv);

        case eW97_STR_rtrim:
            return Word97RTrimS(lpstEnv);

        case eW97_STR_str:
            return Word97StrS(lpstEnv);

        case eW97_STR_ucase:
            return Word97UCaseS(lpstEnv);

        default:
            *lpbKnown = FALSE;
            return(TRUE);
    }
}

BOOL Word97SubObjectEvaluate
(
    LPO97_ENV       lpstEnv,
    WORD            wNumParams,
    EO97_TYPE_T     eParentType,
    LPVOID          lpvParentData,
    LPBOOL          lpbKnown
)
{
    DWORD           dwObjectID;
    EW97_STR_T      eObject;

    eObject = (EW97_STR_T)O97GetKnownIdentEnum(lpstEnv,lpstEnv->wOpID);

    // Assume known

    *lpbKnown = TRUE;

    if (eParentType == eO97_TYPE_VAR)
    {
        LPO97_VAR       lpstVar;

        lpstVar = (LPO97_VAR)lpvParentData;

        if (lpstVar->eType == eO97_TYPE_DIALOG)
        {
            LPO97_DIALOG    lpstDlg;

            // First see if it is a specific dialog type

            lpstDlg = (LPO97_DIALOG)lpstVar->uData.lpvstDlg;
            switch (lpstDlg->dwDlgID)
            {
                case eW97_STR_filesaveas:
                    if (lpstEnv->wOpID == eO97_FIXED_ID_FORMAT)
                        return O97EvalPushLongOperand(lpstEnv,
                                                      ((LPO97_DLG_FILESAVEAS)lpstDlg)->
                                                          byFormat);
                    break;

                case eW97_STR_filenew:
                    if (eObject == eW97_STR_newtemplate)
                        return O97EvalPushLongOperand(lpstEnv,
                                                      ((LPO97_DLG_FILENEW)lpstDlg)->
                                                          byNewTemplate);
                    break;

                case eW97_STR_filesummaryinfo:
                    if (eObject == eW97_STR_template)
                        return O97EvalPushSZOperand(lpstEnv,glpszO97Normal);
                    break;

                case eW97_STR_filetemplates:
                    if (eObject == eW97_STR_template)
                        return O97EvalPushSZOperand(lpstEnv,glpszO97FN);
                    break;

                default:
                    break;
            }

            // Else use generic handling

            if (lpstEnv->wOpID == eO97_FIXED_ID_NAME)
                return O97EvalPushSZOperand(lpstEnv,glpszO97FN);

            switch (eObject)
            {
                case eW97_STR_directory:
                    return O97EvalPushSZOperand(lpstEnv,NULL);

                case eW97_STR_filename:
                    return O97EvalPushSZOperand(lpstEnv,glpszO97FN);

                case eW97_STR_template:
                    return O97EvalPushSZOperand(lpstEnv,glpszO97Normal);

                default:
                    break;
            }

            *lpbKnown = FALSE;
            return(TRUE);
        }
        else
        if (lpstVar->eType == eO97_TYPE_OBJECT_ID)
        {
            eParentType = eO97_TYPE_OBJECT_ID;
            dwObjectID = lpstVar->uData.dwObjectID;
        }
        else
        {
            *lpbKnown = FALSE;
            return(TRUE);
        }
    }
    else
    {
        dwObjectID = *(LPDWORD)lpvParentData;
    }

    if (eParentType != eO97_TYPE_OBJECT_ID &&
        eParentType != eO97_TYPE_O97_OBJECT_ID)
    {
        // Don't know what it is

        *lpbKnown = FALSE;
        return(TRUE);
    }

    if (eParentType == eO97_TYPE_O97_OBJECT_ID)
    {
        if (dwObjectID == eO97_STR_application)
        {
            switch (eObject)
            {
                case eW97_STR_organizercopy:
                    return Word97Organizer(lpstEnv,TRUE,FALSE);

                case eW97_STR_organizerrename:
                    return Word97Organizer(lpstEnv,FALSE,TRUE);

                case eW97_STR_vbe:
                {
                    dwObjectID = eObject;
                    return O97EvalPushOperand(lpstEnv,
                                              eO97_TYPE_OBJECT_ID,
                                              &dwObjectID);
                }
                default:
                    *lpbKnown = FALSE;
                    return(TRUE);
            }
        }
    }
    else
    {
        switch (dwObjectID)
        {
            case eW97_STR_wordbasic:
                if (lpstEnv->wOpID == eO97_FIXED_ID_CALL)
                {
                    lpstEnv->bIsStringCall = TRUE;
                    *lpbKnown = FALSE;
                    return(TRUE);
                }

                switch (eObject)
                {
                    case eW97_STR_chr:
                    case eW97_STR_chrS:
                        return Word97ChrS(lpstEnv);

                    case eW97_STR_countmacros:
                        return Word97CountMacros(lpstEnv);

                    case eW97_STR_countwindows:
                        return O97EvalPushLongOperand(lpstEnv,1);

                    case eW97_STR_defaultdirS:
                        return O97EvalPushSZOperand(lpstEnv,NULL);

                    case eW97_STR_dialogrecord:
                        dwObjectID = eW97_STR_dialogrecord;

                        // Push the object

                        return O97EvalPushOperand(lpstEnv,
                                                 eO97_TYPE_OBJECT_ID,
                                                 &dwObjectID);

                    case eW97_STR_filename:
                    case eW97_STR_filenameS:
                    case eW97_STR_windownameS:
                    case eW97_STR_filenamefromwindowS:
                    case eW97_STR_selectionfilename:
                        return O97EvalPushSZOperand(lpstEnv,glpszO97FN);

                    case eW97_STR_filenameinfoS:
                        return Word97FileNameInfoS(lpstEnv);

                    case eW97_STR_isexecuteonly:
                        return Word97IsExecuteOnly(lpstEnv);

                    case eW97_STR_lcase:
                    case eW97_STR_lcaseS:
                        return Word97LCaseS(lpstEnv);

                    case eW97_STR_len:
                        return Word97Len(lpstEnv);

                    case eW97_STR_left:
                    case eW97_STR_leftS:
                        return Word97LeftS(lpstEnv);

                    case eW97_STR_ltrim:
                    case eW97_STR_ltrimS:
                        return Word97LTrimS(lpstEnv);

                    case eW97_STR_macrocopy:
                        return Word97MacroCopy(lpstEnv);

                    case eW97_STR_macrodescS:
                        return Word97MacroDescS(lpstEnv);

                    case eW97_STR_macrofilenameS:
                        if (O97SubStateTop(lpstEnv) == essO97_SKIP)
                            return O97EvalPushSZOperand(lpstEnv,NULL);

                        return Word97MacroFileNameS(lpstEnv);

                    case eW97_STR_macronameS:
                        if (O97SubStateTop(lpstEnv) == essO97_SKIP)
                            return O97EvalPushSZOperand(lpstEnv,NULL);

                        return Word97MacroNameS(lpstEnv);

                    case eW97_STR_mid:
                    case eW97_STR_midS:
                        return Word97MidS(lpstEnv);

                    case eW97_STR_organizer:
                        return Word97Organizer(lpstEnv,FALSE,FALSE);

                    case eW97_STR_right:
                    case eW97_STR_rightS:
                        return Word97RightS(lpstEnv);

                    case eW97_STR_rtrim:
                    case eW97_STR_rtrimS:
                        return Word97RTrimS(lpstEnv);

                    case eW97_STR_str:
                    case eW97_STR_strS:
                        return Word97StrS(lpstEnv);

                    case eW97_STR_ucase:
                    case eW97_STR_ucaseS:
                        return Word97UCaseS(lpstEnv);

                    case eW97_STR_val:
                        return Word97Val(lpstEnv);

                    default:
                        *lpbKnown = FALSE;
                        return(TRUE);
                }
                //break;

            case eW97_STR_dialogrecord:
                *lpbKnown = TRUE;
                dwObjectID = eObject;

                // Push the dialog record value

                return O97EvalPushOperand(lpstEnv,
                                          eO97_TYPE_DIALOG_ID,
                                          &dwObjectID);

            case eW97_STR_activedocument:
                if (lpstEnv->wOpID == (WORD)eO97_FIXED_ID_NAME ||
                    eObject == eW97_STR_fullname)
                    return O97EvalPushSZOperand(lpstEnv,glpszO97FN);
                else
                if (eObject == eW97_STR_vbproject)
                {
                    dwObjectID = W97_LOCAL_VBPROJECT;

                    return O97EvalPushOperand(lpstEnv,
                                              eO97_TYPE_OBJECT_ID,
                                              &dwObjectID);
                }
                break;

            case eW97_STR_normaltemplate:
                if (lpstEnv->wOpID == (WORD)eO97_FIXED_ID_NAME ||
                    eObject == eW97_STR_fullname)
                    return O97EvalPushSZOperand(lpstEnv,glpszO97Normal);
                else
                if (eObject == eW97_STR_vbproject)
                {
                    dwObjectID = W97_GLOBAL_VBPROJECT;

                    return O97EvalPushOperand(lpstEnv,
                                              eO97_TYPE_OBJECT_ID,
                                              &dwObjectID);
                }
                break;

            case eW97_STR_vbe:
                if (eObject == eW97_STR_activevbproject)
                {
                    if (lpstEnv->bNormal == FALSE)
                        dwObjectID = W97_LOCAL_VBPROJECT;
                    else
                        dwObjectID = W97_GLOBAL_VBPROJECT;

                    return O97EvalPushOperand(lpstEnv,
                                              eO97_TYPE_OBJECT_ID,
                                              &dwObjectID);
                }
                break;

            case W97_LOCAL_VBPROJECT:
            case W97_GLOBAL_VBPROJECT:
                if (eObject == eW97_STR_vbcomponents)
                {
                    if (wNumParams == 0)
                    {
                        if (dwObjectID == W97_LOCAL_VBPROJECT)
                            dwObjectID = W97_LOCAL_VBCOMPONENTS;
                        else
                            dwObjectID = W97_GLOBAL_VBCOMPONENTS;

                        return O97EvalPushOperand(lpstEnv,
                                                  eO97_TYPE_OBJECT_ID,
                                                  &dwObjectID);
                    }

                    // The number of parameters is non-zero

                    return Word97VBComponents(lpstEnv,dwObjectID);
                }
                break;

            case W97_LOCAL_VBCOMPONENTS:
            case W97_GLOBAL_VBCOMPONENTS:
                if (eObject == eW97_STR_count)
                {
                    if (lpstEnv->bNormal == TRUE &&
                        dwObjectID == W97_LOCAL_VBCOMPONENTS ||
                        lpstEnv->bNormal == FALSE &&
                        dwObjectID == W97_GLOBAL_VBCOMPONENTS)
                    {
                        // Indicate no macros

                        return O97EvalPushLongOperand(lpstEnv,0);
                    }

                    // Push module count

                    return O97EvalPushLongOperand(lpstEnv,
                                                  (long)lpstEnv->dwNumModules);

                }
                else
                if (eObject == eW97_STR_item)
                {
                    if (dwObjectID == W97_LOCAL_VBCOMPONENTS)
                    {
                        return Word97VBComponents(lpstEnv,
                                                  W97_LOCAL_VBPROJECT);
                    }
                    else
                    {
                        return Word97VBComponents(lpstEnv,
                                                  W97_GLOBAL_VBPROJECT);
                    }
                }
                else
                if (eObject == eW97_STR_import)
                {
                    // Make sure we have the source filename

                    if (wNumParams == 1)
                        return Word97Import(lpstEnv,dwObjectID);
                }
                break;

            default:
                break;
        }

        // See if it is a special object

        switch (dwObjectID & W97_SPECIAL_CAT_MASK)
        {
            case W97_LOCAL_MODULE_INDEX_CAT:
            case W97_GLOBAL_MODULE_INDEX_CAT:
            {
                switch (eObject)
                {
                    case eW97_STR_export:
                        return Word97Export(lpstEnv,dwObjectID);

                    case eW97_STR_codemodule:
                        return O97EvalPushOperand(lpstEnv,
                                                  eO97_TYPE_OBJECT_ID,
                                                  &dwObjectID);

                    case eW97_STR_countoflines:
                        return Word97CountOfLines(lpstEnv,dwObjectID);

                    case eW97_STR_addfromfile:
                        if ((dwObjectID & W97_SPECIAL_CAT_MASK) ==
                            W97_LOCAL_MODULE_INDEX_CAT)
                            return Word97Import(lpstEnv,
                                                W97_LOCAL_VBCOMPONENTS);

                        return Word97Import(lpstEnv,
                                            W97_GLOBAL_VBCOMPONENTS);

                    default:
                        break;
                }
                break;
            }
            default:
                break;
        }
    }

    *lpbKnown = FALSE;
    return(TRUE);
}

void Word97GetDialogInfo
(
    LPO97_DIALOG    lpstDlg,
    DWORD           dwDlgID
)
{
    lpstDlg->dwDlgID = dwDlgID;
    switch (dwDlgID)
    {
        case eW97_STR_filesaveas:
            lpstDlg->dwSize = sizeof(O97_DLG_FILESAVEAS_T);
            break;

        case eW97_STR_filenew:
            lpstDlg->dwSize = sizeof(O97_DLG_FILENEW_T);
            break;

        default:
            lpstDlg->dwSize = sizeof(O97_DIALOG_T);
            break;
    }
}

void Word97InitDialogInfo
(
    LPO97_ENV       lpstEnv,
    LPO97_DIALOG    lpstDlg
)
{
    (void)lpstEnv;

    switch (lpstDlg->dwDlgID)
    {
        case eW97_STR_filesaveas:
            ((LPO97_DLG_FILESAVEAS)lpstDlg)->byFormat = 0;
            break;

        case eW97_STR_filenew:
            ((LPO97_DLG_FILENEW)lpstDlg)->byNewTemplate = 0;
            break;

        default:
            break;
    }
}

BOOL Word97AssignDialogField
(
    LPO97_ENV       lpstEnv,
    LPO97_DIALOG    lpstDlg
)
{
    EW97_STR_T      eObject;
    long            lValue;

    eObject = (EW97_STR_T)O97GetKnownIdentEnum(lpstEnv,lpstEnv->wOpID);

    switch (lpstDlg->dwDlgID)
    {
        case eW97_STR_filesaveas:
            if (lpstEnv->wOpID == eO97_FIXED_ID_FORMAT)
            {
                if (O97EvalGetAndPopTopAsLong(lpstEnv,
                                              &lValue) == FALSE)
                    return(FALSE);

                ((LPO97_DLG_FILESAVEAS)lpstDlg)->byFormat = (BYTE)lValue;
                return(TRUE);
            }
            break;

        case eW97_STR_filenew:
            if (eObject == eW97_STR_newtemplate)
            {
                if (O97EvalGetAndPopTopAsLong(lpstEnv,
                                              &lValue) == FALSE)
                    return(FALSE);

                ((LPO97_DLG_FILENEW)lpstDlg)->byNewTemplate = (BYTE)lValue;
                return(TRUE);
            }
            break;

        default:
            break;
    }

    // Just pop the operand and return

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
    {
        // Failed to pop

        return(FALSE);
    }

    return(TRUE);
}

O97_OBJECT_FUNC_T gstWord97ObjectFunc =
{
    Word97ObjectEvaluate,
    Word97SubObjectEvaluate,
    Word97GetDialogInfo,
    Word97InitDialogInfo,
    Word97AssignDialogField
};

#endif // #ifdef MACROHEU

