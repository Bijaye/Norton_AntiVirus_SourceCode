/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
//#include <statreg.cpp>
#endif

//#include <atlimpl.cpp>

// INCLUDE FOR CSTRING STUFF jhill
#include <afxdisp.h> 


#if 0
#include "IcePackTokens.h"
/*----------------------------------------------------------------------------
   LookUpIcePackTokenString()

   Using the incoming "token" from IcePack, lookup the coresponding 
   string in IcePackTokens.dll, which is a resource only Dll.

   This is linked with IcePackTokens.Lib

   Written by: Jim Hill
   --------------------------------------------------------------


   IcePackTokens.dll contains three tables that can be used to map  
   text tokens used by the IcePack agent into display strings:
   
      The IcePackAttentionTable table contains tokens used in the
      "attention" registry variable.
   
      The IcePackErrorTable table contains tokens used in the 
      X-Error attribute of samples in quarantine.
   
      The IcePackStateTable table contains tokens used in the
      X-Analsysis-State attributes of samples in quarantine.
   
                               -- Edward Pring <pring@watson.ibm.com>
----------------------------------------------------------------------------*/
TCHAR *LookUpIcePackTokenString(TCHAR *lpszToken, DWORD dwTableID)
{
    wchar_t   szSearchToken[512];
    wchar_t** lppTokenTable   = NULL;
    int       iTokenLength    = 0;
    TCHAR *   lpszString      = NULL;
    TCHAR *   lpszStartString = NULL;
    int       iTokenSeparator = '=';
    
    // VALIDATE
    if( lpszToken == NULL || *lpszToken == 0 )
        return(NULL);
    memset( szSearchToken, 0, sizeof(szSearchToken) );

    // GET THE RIGHT TABLE
    switch( dwTableID )
    {
        case ICEPACK_TOKEN_ATTENTION_TABLE :
            lppTokenTable = (wchar_t**)IcePackAttentionTable;
            break;

        case ICEPACK_TOKEN_ERROR_TABLE :
            lppTokenTable = (wchar_t**)IcePackErrorTable;
            break;

        case ICEPACK_TOKEN_STATE_TABLE :
            lppTokenTable = (wchar_t**)IcePackStateTable;
            break;

        case ICEPACK_TOKEN_STATUS_TABLE:
            lppTokenTable = (wchar_t**)IcePackStatusTable;
            break;

        case ICEPACK_TOKEN_RESULT_TABLE:
            lppTokenTable = (wchar_t**)IcePackResultTable;
            break;

        default:
            return(lpszToken);
    }

    // CONVERT THE TOKEN
    //MultiByteToWideChar(CP_ACP, 0, lpszToken, -1, szSearchToken,sizeof(szSearchToken)/sizeof(wchar_t));
    //wcscat(szSearchToken, L"=");
    //wsprintf( szSearchToken,_T("%s:"), lpszToken );
    wsprintf( szSearchToken,_T("%s%c"), lpszToken, iTokenSeparator );


    // LOOK FOR A MATCH
     iTokenLength = wcslen(szSearchToken);
     for (wchar_t** entry = lppTokenTable; *entry &&
        wcsnicmp(szSearchToken,*entry,iTokenLength)!=0; entry++);


    // IF NO ENTRY FOUND, RETURN NULL
    if( *entry == NULL)
    {
        lpszString = NULL;       // lpszToken;
    }
    else
    {
        // LOOK FOR THE COLON
        //lpszStartString = wcschr( *entry, ':' );
        lpszStartString = wcschr( *entry, iTokenSeparator );    // '='
        if( lpszStartString == NULL )
        {
            lpszStartString = *entry;
        }
        else
        {
            // WALK PAST THE COLON
            ++lpszStartString;

            // WALK PAST THE SPACES
            for(TCHAR c = 0; (c = *lpszStartString)==' ';)
            {
                ++lpszStartString;
            }
        }
        lpszString = lpszStartString;      // *entry;
    }

    return( lpszString );
}

#endif