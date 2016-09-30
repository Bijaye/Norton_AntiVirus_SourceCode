// Copyright (C) 1996 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/nlm_str.c_v   1.3   24 Jul 1997 14:16:54   DDREW  $
//
// Description: DBCS character and string functions for the NLM platform.
//
//              ***     FOR NLM PLATFORM ONLY     ***
//
//
//***************************************************************************
// $Log:   S:/SYMKRNL/VCS/nlm_str.c_v  $
// 
//    Rev 1.3   24 Jul 1997 14:16:54   DDREW
// Removed extermnal referance to "ExternalPublicList" to get a compile 
// 
//    Rev 1.2   13 Sep 1996 15:53:26   MLELE
// Temporary fix: Disabled DBSC support. NLMImportSymbol crashes on 4.x
// 
//    Rev 1.1   21 Aug 1996 16:43:48   MLELE
// Added NW_StrChr and NW_StrRChr.
// 
//    Rev 1.0   21 Aug 1996 15:06:22   MLELE
// Initial revision.
//***************************************************************************


#ifdef  SYM_NLM                         //  FOR THE NLM PLATFORM ONLY


#include "platform.h"                   //  Platform secific definitions




// --------------------------------------------------------------------------
//  typedef: SYMBOLENTRY
//
//      Singly linked-list node for exported symbols on the NLM platform.
//
//  NOTE:
//      NETWARE UNDOCUMENTED DEFINTION
//---------------------------------------------------------------------------

        //
        //  Incomplete definition. Reqd so that I can typedef LPSYMBOLENTRY
        //
        struct tagSYMBOLENTRY;
typedef struct tagSYMBOLENTRY FAR *LPSYMBOLENTRY;

typedef struct tagSYMBOLENTRY               //  Complete declaration
{
    LPSYMBOLENTRY   lpNextSymbol;           // next symbol in list
    LPVOID          lpNlmEntry;
    DWORD           dwSymValue;
    LPBYTE          lpcpSymName;

} SYMBOLENTRY;


//
//  Type definitions for casting imported functions
//
typedef LPTSTR (*LPFNCHARTYPE)  (TCHAR);
typedef LPTSTR (*LPFNCHARPREV)  (LPTSTR, LPTSTR);
typedef LPTSTR (*LPFNSTRCHR)    (LPTSTR, int);
typedef LPTSTR (*LPFNSTRRCHR)   (LPTSTR, int);




//***************************************************************************
// NLMImportSymbol()
//
//      Our version of ImportSymbol for the NLM platform.
//
//      This function walks through the list of exported symbols,
//      ExternalPublicList, looking for the requested symbol. If it finds
//      a match, it returns a pointer to it, otherwise it returns a NULL.
//
// Parameters:
//      szSymbolName        (IN)    char *
//                          The name of the symbol to import.
//
// Return Value:
//      VOID *              A pointer to the symbol, if found.
//                          NULL if symbol not found.
//
// Uses functions:
//      STRLEN, STRNCPY, STRNCMP
//
// NOTE:
//      ==  FOR NLM PLATFORM ONLY.
//      ==  USES UNDOCUMENTED EXTERNAL SYMBOL (ExternalPublicList)
//
//***************************************************************************
// 08/21/96 MLELE  Function created.
//***************************************************************************

VOID * NLMImportSymbol (char *szSymbolName)

{
//    EXTERN  LPSYMBOLENTRY   ExternalPublicList;
    LPSYMBOLENTRY           ExternalPublicList = NULL;

    LPSYMBOLENTRY           lpSymbol = ExternalPublicList;
    BYTE                    cpSymbol[256];
    BYTE                    bySymLen;
    VOID                    *pSymValue = NULL;


    //
    //  Convert to count-preceded string
    //
    bySymLen = STRLEN (szSymbolName);

    cpSymbol[0] = bySymLen;

    STRNCPY (&cpSymbol[1], szSymbolName, cpSymbol[0]);

    while (lpSymbol != NULL)
        {
        if (bySymLen == lpSymbol->lpcpSymName[0])
            {
            if (!STRNCMP (&cpSymbol[1], &lpSymbol->lpcpSymName[1], bySymLen))
                {
                pSymValue = (VOID *)lpSymbol->dwSymValue;
                break;
                }
            }

        lpSymbol = lpSymbol->lpNextSymbol;
        }

    return  pSymValue;
}




// *************************************************************************
//
//      DBCS CHARACTER AND STRING FUNCTIONS
//
// *************************************************************************


LPTSTR NW_CharNext (LPTSTR pStr)
{
#ifdef  TEMPFIX
    LPFNCHARTYPE    lpfnCharType;


    //
    //  Import NWCharType entry point.
    //
    //  NOTE:
    //      NWNextChar is actually a macro defined as
    //          p + ((NWCharType(*pr)==1) ? 1:2).
    //      So, since I cannot import NWNextChar, I import NWCharType and
    //      compute the next char.  ...I know. I hate this too.
    //
    lpfnCharType = (LPFNCHARTYPE) NLMImportSymbol ("NWCharType");

    if (lpfnCharType)
        return (pStr + ((lpfnCharType(*pStr) == 1) ? 1 : 2));
    else
#endif  // .. TEMPFIX
        return (pStr + 1);
}



LPTSTR NW_CharPrev (LPTSTR szStart, LPSTR pStr)
{
#ifdef TEMPFIX
    LPFNCHARPREV    lpfnCharPrev;

    //
    //  Import NWPrevChar entry point.
    //
    lpfnCharPrev = (LPFNCHARPREV) NLMImportSymbol ("NWPrevChar");

    if (lpfnCharPrev)
        return (lpfnCharPrev (szStart, pStr));
    else
#endif  // .. TEMPFIX
        return ((szStart < pStr) ? (pStr - 1) : szStart);
}



LPTSTR NW_StrChr (LPTSTR pStr, int iFind)
{
#ifdef TEMPFIX
    LPFNSTRCHR  lpfnStrChr;


    //
    //  Import NWLstrchr
    //
    lpfnStrChr = (LPFNSTRCHR) NLMImportSymbol ("NWLstrchr");

    if (lpfnStrChr)
        return (lpfnStrChr (pStr, iFind));
    else
#endif  // .. TEMPFIX
        return (strchr (pStr, iFind));
}



LPTSTR NW_StrRChr (LPTSTR pStr, int iFind)
{
#ifdef TEMPFIX
    LPFNSTRRCHR lpfnStrRChr;

    //
    //  Import NWLstrrchr
    //
    lpfnStrRChr = (LPFNSTRRCHR) NLMImportSymbol ("NWLstrrchr");

    if (lpfnStrRChr)
        return ( lpfnStrRChr (pStr, iFind) );
    else
#endif  // .. TEMPFIX
        return ( strrchr (pStr, iFind) );
}




#endif      // .. SYM_NLM

