/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*------------------------------------------------------------------------
   UTILPLUS.C
   
   C++ Utility routines 
   
   Written by Jim Hill
------------------------------------------------------------------------*/
//#define USE_RESOURCE_ONLY_DLL   1



#include "stdafx.h"	 
#include "resource.h"	 
#include "UtilPlus.h"	 
#include "qscommon.h"
#include "vpstrutils.h"
// #ifdef USE_RESOURCE_ONLY_DLL
//     #include "AVIS ResourceDll.h"
// #endif

#include   "IcePackTokens.h"



// GLOBAL INSTALL FLAG: WAS QSERVER INSTALLED WITH AVIS IcePack OR WITH OLD S&D?
//DWORD  g_dwIcePackInstalled = TRUE;
DWORD  g_dwQServerVersion   = 0;


// Handle to main console window.
HWND g_hMainWnd = NULL;
DWORD g_dwCurrentThreadID = 0;

// REGISTERED WINDOW MESSAGES
DWORD dwWmsgSConfigWriteData = 0;

static g_ObjectID = 0;

/*----------------------------------------------------------------------------
   GetNextObjectID()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
int GetNextObjectID()
{
    ++g_ObjectID;
    return(g_ObjectID);
}


/*----------------------------------------------------------------------------
   ClearServerVersionNumber

   This will cause the registry to be read on the next call to 
   IsIcePackSupported()

   Written by: Jim Hill      1/4/00
----------------------------------------------------------------------------*/
int ClearServerVersionNumber()
{
    g_dwQServerVersion = 0;
    return(g_dwQServerVersion);
}


/*----------------------------------------------------------------------------
   GetCurrentSelectedQserverVersion

   QsConsole writes the version of the currently selected
   Qserver here just before my ScopeExtData or AvisConExtData 
   objects are created.

   These registry entries are stored on the local console machine.

   Written by: Jim Hill      1/3/00
----------------------------------------------------------------------------*/
DWORD GetCurrentSelectedQserverVersion( int iExtObjectID )
{
    DWORD   dwQserverVersion = 1;
	CRegKey reg;
	LONG    lResult;

    // TEMP 1/1/00   FORCE 
    //g_dwQServerVersion = 2;   // 1
    //return(g_dwQServerVersion);


    //  GO RETRIEVE IT FROM LOCAL REGISTRY ENTRY
	lResult = reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QSCONSOLE);
    if( ERROR_SUCCESS == lResult )
	{
		lResult = reg.QueryDWORDValue(REGVALUE_SELECTED_QSERVER_VERSION, dwQserverVersion);
		if(ERROR_SUCCESS != lResult )
		{
            dwQserverVersion = 1;
		}
	    reg.Close();
	}

   	fWidePrintString("GetCurrentSelectedQserverVersion() called. Version= %d   ObjectID= %d ", dwQserverVersion, iExtObjectID );

    return(dwQserverVersion);
}


/*----------------------------------------------------------------------------
   IsAvisIcePackSupported()
   Written by: Jim Hill    1/1/00
----------------------------------------------------------------------------*/
BOOL IsAvisIcePackSupported( int iExtObjectID )   
{
    if( g_dwQServerVersion == 0 )
        g_dwQServerVersion = GetCurrentSelectedQserverVersion(iExtObjectID);

   	fWidePrintString("IsAvisIcePackSupported() called, Version= %d  ObjectID= %d ", g_dwQServerVersion, iExtObjectID );

    if( g_dwQServerVersion < 2 )
        return(FALSE);
    else
        return(TRUE);
}


/*----------------------------------------------------------------------------
   SaveQServerVersion()
   Written by: Jim Hill    1/1/00
----------------------------------------------------------------------------*/
DWORD SaveQServerVersion(int iExtObjectID )   
{
    //  GO RETRIEVE IT FROM LOCAL REGISTRY ENTRY
    g_dwQServerVersion = GetCurrentSelectedQserverVersion(iExtObjectID);
    return(g_dwQServerVersion);
}


/*----------------------------------------------------------------------------
    StrTableLoadString()

    Using passed global ID, lookup the string from the IcePackTokens.dll RC file.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL StrTableLoadString( DWORD dwGlobalResID, CString& s )
{
    BOOL  rc = FALSE;
    WCHAR szBuff[512];

    s = _T("");
    if( _StrTableLoadString( dwGlobalResID, szBuff, sizeof(szBuff) ) > 0 )
    {
        s  = szBuff;
        rc = TRUE;
    }
    return(rc);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL StrLookUpIcePackTokenString( CString& sToken, CString& s, DWORD dwTableID )
{
    WCHAR* lpszDisplayString = NULL;
    WCHAR* lpszToken         = NULL;
    BOOL   rc = FALSE;
	USES_CONVERSION;

    s = _T("");
    if( sToken.IsEmpty() )
        return(FALSE);

    // LookUpIcePackTokenString
    lpszToken = T2W(sToken.GetBuffer(0));
    lpszDisplayString = LookUpIcePackTokenString( lpszToken, dwTableID );
    sToken.ReleaseBuffer(-1);

    if( lpszDisplayString == NULL )
    {
        s  = sToken;
        rc = FALSE;
    }
    else
    {
        s  = lpszDisplayString;
        rc = TRUE;
    }

    return(rc);
}


/*----------------------------------------------------------------------------
   EncryptDecryptPassword

   Written by: Jim Hill
----------------------------------------------------------------------------*/
int EncryptDecryptPassword(CString& sPassword)   
{
    int    rc = 0;
    TCHAR  szBuff[1024] = {0};  
    int    iLength = 0;
    int    i       = 0;
    

    //return(0); // 5/23/00 jhill Temp Disable until IcePack is renabled to Encrypt/Decrypt
    //  8/28/00 jhill IcePack re-enable to unscramble password in Build 217a 8/30/00

    // VALIDATE
    if( sPassword.IsEmpty() )
        return(-1);
    
    iLength = sPassword.GetLength();
    if( iLength >= sizeof(szBuff) )
        return(-1);
    vpstrncpy( szBuff, sPassword.GetBuffer(0), iLength ); 
    sPassword.ReleaseBuffer(-1);

    for( i = 0; i < iLength && i < sizeof(szBuff); i++  )
    {
        szBuff[i] ^= 0xff;
    }


    //char   *pszWalker = NULL;
    //pszWalker = (char *) &szBuff[0];
    //for( i = 0; *pszWalker; i++, pszWalker++  )
    //{
    //    *pszWalker ^= 0xff;
    //}

    sPassword = szBuff;

    return(rc);
}



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
DWORD SaveCurrentThreadId()
{
    // DEBUG Check current thread 12/29/99
    g_dwCurrentThreadID = GetCurrentThreadId();
    return(g_dwCurrentThreadID);
}

/*----------------------------------------------------------------------------
   LookupResourceString()

   Uses a resource only DLL to Lookup strings.
   This should be the same as the new resource only DLL for QsConsole.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL LookupResourceString(CString& sText, DWORD dwID )
{
#ifdef USE_RESOURCE_ONLY_DLL
    DWORD  rc = 0;
    TCHAR szText[MAX_PATH * 2];

    memset(szText,0,sizeof(szText));
    sText.Empty();

    // CALL INTO RESOURCE ONLY DLL
    rc = fnLookupResourceText( szText, dwID );
    if( rc )
    {
        sText = szText;
        return(TRUE);
    }
    else
        return(FALSE);
#else
    sText.LoadString( dwID );
    return(TRUE);
#endif
}


/*----------------------------------------------------------------------------
    IsAttributeDeleted
    Is this attribute deleted?

    Written by: Jim Hill 
 ----------------------------------------------------------------------------*/
BOOL IsAttributeDeleted( VARIANT * pValue )
{
    CString s;

    switch (pValue->vt)
    {
        case VT_UI4 :
            if( pValue->ulVal == 0xFFFFFFFF )
                return( TRUE ); 
            break;
        case VT_DATE :
            if( pValue->date == 0.0 )
                return ( TRUE ); 
            break;
        case VT_BSTR :
            s = pValue->bstrVal;
            if( !s.IsEmpty() && s == _T(" ") )
                return ( TRUE ); 
            break;
        default :
            /* DO NOTHING */
            break;
    }
            
    // Mysterious attribute type, so presumably not deleted.
    return( FALSE );
}




#include <locale.h>    // 7/15


#define CONVERT_TO_LOCAL_TIME   0
#define DISPLAY_GMT_TIME        1
/*----------------------------------------------------------------------------
   ConvertVariantToString()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL ConvertVariantToString(CString& sText, VARIANT *pv, DWORD dwAttributeFormat ) 
{
    SYSTEMTIME  SysTime;

    if( pv == NULL)
        return(FALSE);

    sText.Empty();

    switch( pv->vt )
    {
        case VT_BSTR:
            sText = pv->bstrVal;
            break;

        case VT_UI4:
            if( dwAttributeFormat == AVIS_ATTRIBUTE_FORMAT_HEX_STRING )
                sText.Format( _T("0x%08x"), pv->ulVal );
            else
                sText.Format( _T("%d"), pv->ulVal );
            break;

        case VT_DATE:
        {
            // IF IT'S 0 FOR DELETED, JUST DISPLAY A BLANK
            if( pv->date == 0.0 )
            {
                sText = _T(" ");
                return(FALSE);  
            }

            TCHAR szDate[MAX_PATH];
            TCHAR szTime[MAX_PATH];
            TCHAR *lpszLocale = NULL;
            TCHAR szLocale[259];
            //TCHAR szNewLocale[259];
            lpszLocale = _tsetlocale( LC_ALL, NULL );   // English  _T("Japanese")
            szLocale[0] = 0;
            if( lpszLocale != NULL )
                vpstrncpy( szLocale, lpszLocale, sizeof(szLocale) );


// #if DISPLAY_GMT_TIME  == 1
            VariantTimeToSystemTime( pv->date, &SysTime );
            //                                    
            WORD wLangID   = MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US );
            LCID dwocaleID = MAKELCID( wLangID, SORT_DEFAULT );
            GetDateFormat( dwocaleID, 0, &SysTime, _T("ddd, dd MMM yyyy"), szDate, sizeof(szDate) );              // NULL
            GetTimeFormat( dwocaleID, TIME_FORCE24HOURFORMAT, &SysTime, _T("HH:mm:ss"), szTime, sizeof(szTime) ); // NULL

            //GetDateFormat( LOCALE_SYSTEM_DEFAULT, LOCALE_USE_CP_ACP, &SysTime, _T("ddd, dd MMM yyyy"), szDate, sizeof(szDate) );              // NULL
            //GetDateFormat( LOCALE_SYSTEM_DEFAULT, DATE_LONGDATE, &SysTime, NULL, szDate, sizeof(szDate) );                  // NULL
            //GetTimeFormat( LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, &SysTime, _T("HH:mm:ss"), szTime, sizeof(szTime) ); // NULL

            sText.Format( _T("%s %s GMT"), szDate, szTime );  // 
            //sText.Format( IDS_ATTRIBUTE_DATE_DISPLAY_FMT, szDate, szTime );  // _T("%s %s GMT")
            //fWidePrintString( "UtilPlus:Date atrib: %s  Locale= %s ", (LPCTSTR) sText, szLocale );
//#else
#if 0
            // ELSE, DISPLAY LOCAL TIME
            char c;
            char szBuff[100];
            char *lpszTzNameString = NULL;
            TCHAR szWTzName[32];
            TIME_ZONE_INFORMATION TzInfo;
            SYSTEMTIME st;

            VariantTimeToSystemTime( pv->date, &st );
            SystemTimeToTzSpecificLocalTime( NULL, &st, &SysTime );
            GetDateFormat( LOCALE_SYSTEM_DEFAULT, 0, &SysTime, _T("ddd, dd MMM yyyy"), szDate, 32 );               // NULL
            GetTimeFormat( LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, &SysTime, _T("HH:mm:ss"), szTime, 32 );  // NULL

            // GET THE TIME ZONE STRING
            memset( szBuff, 0 ,sizeof(szBuff) );
            memset( szWTzName, 0 ,sizeof(szWTzName) );
            memset( &TzInfo, 0 ,sizeof(TIME_ZONE_INFORMATION) );
            DWORD  dwRet = GetTimeZoneInformation( &TzInfo );

            // if( dwRet == TIME_ZONE_ID_STANDARD)
            //     lpszTzNameString = _tzname[0];     // STRCPY( szWTzName, TzInfo.StandardName );
            // else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
            //     lpszTzNameString = _tzname[1];     //STRCPY( szWTzName, TzInfo.DaylightName );
            if( dwRet == TIME_ZONE_ID_STANDARD)
            {
                lpszTzNameString = _tzname[0]; 
                c = *lpszTzNameString;
                wsprintfA( szBuff, "%cST", c );
                lpszTzNameString = szBuff;
            }
            else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
            {
                lpszTzNameString = _tzname[1];
                c = *lpszTzNameString;
                wsprintfA( szBuff, "%cDT", c );
                lpszTzNameString = szBuff;
            }
 
            // CONVERT IT
            if( lpszTzNameString != NULL )
            {
                BOOL bRet = MultiByteToWideChar(CP_ACP,0,lpszTzNameString,-1,szWTzName,sizeof(szWTzName));   
                if( !bRet )
                    szWTzName[0] = 0;
            }

            // PUT THEM ALL TOGETHER
            sText.Format( _T("%s %s %s"), szDate, szTime, szWTzName );
#endif
        }
        break;

        default:
            return(FALSE);  
            break;

    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
   ConvertVariantToDword()

   If VARIANT* pv is VT_UI4, return as DWORD in lpdwValue.
   If VARIANT* pv is VT_BSTR, convert to DWORD and return in lpdwValue.
   Else return FALSE.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL ConvertVariantToDword( DWORD *lpdwValue, VARIANT *pv)
{
    CString strNum;
    TCHAR *endptr = NULL;

    if( pv == NULL || lpdwValue == NULL )
        return(FALSE);

    *lpdwValue = 0;

    switch( pv->vt )
    {
        case VT_BSTR:
            strNum     = pv->bstrVal;
            *lpdwValue = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
            break;

        case VT_UI4:
            *lpdwValue = pv->ulVal;
            break;

        default:
            return(FALSE);
            break;
    }

    return(TRUE);
}


/*----------------------------------------------------------------------------
   InitializeVariantWithData()

   If VARIANT* pv is VT_UI4,  *lpData points to a DWORD. Store in v.
   If VARIANT* pv is VT_BSTR, *lpData points to a String.
                               Convert to DWORD and store in v.

   Eith lpdwValue or lpBSTR should be NULL

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL InitializeVariantWithData( DWORD *lpdwValue, LPCWSTR lpBSTR, VARIANT *pv)
{
    DWORD   dwTemp = 0;
    CString strNum;
    TCHAR   *endptr = NULL;

    if( pv == NULL || ( lpdwValue == NULL && lpBSTR == NULL ))
        return(FALSE);

    switch( pv->vt )
    {
        case VT_BSTR:
            if( lpdwValue == NULL)
            {
                // IT"S A STRING VALUE
                pv->bstrVal = (LPWSTR)lpBSTR;
            }
            else
            {   // IT'S A DWORD, CONVERT TO STRING
                strNum.Format( _T("%d"), *lpdwValue );
                pv->bstrVal = strNum.AllocSysString();
            }
            break;

                    
        case VT_UI4:
            if( lpdwValue == NULL)
            {
                // IT"S A STRING VALUE, CONVERT TO DWORD
                strNum    = lpBSTR;
                dwTemp    = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                pv->ulVal = dwTemp;

            }
            else
            {   // IT'S A DWORD
                pv->ulVal = *lpdwValue;
            }
            break;

        default:
            return(FALSE);
            break;
    }

    return(TRUE);
}

/*----------------------------------------------------------------------------
   IsVariantDateValid()

   Is this a Date type, and is the date non-zero

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL IsVariantDateValid( VARIANT *pv )
{
    if( pv == NULL )
        return(FALSE);

    if( pv->vt == VT_DATE )
    {
        if( pv->date != 0 )
            return(TRUE);
    }

    return(FALSE);
}


/*----------------------------------------------------------------------------
   IsSuccessfullSampleResult()
   Does the result code represent Success
   Uses SUCCESS() and FAIL() macros and the S_FALSE value.
   Account for GetValue() returning a 2, when the attribute doesn't exist.
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL IsSuccessfullSampleResult( HRESULT hr )
{
    if( hr == 2 )  
        return(FALSE);

    return( IsSuccessfullResult( hr ) );
}


/*----------------------------------------------------------------------------
   IsSuccessfullResult()
   Does the result code represent Success
   Uses SUCCESS() and FAIL() macros and the S_FALSE value
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL IsSuccessfullResult( HRESULT hr )
{
    if( hr == S_OK )
        return(TRUE);

    if( hr = S_FALSE )  
        return(FALSE);

    return( SUCCEEDED( hr ) );
}

/*----------------------------------------------------------------------------
   IsFailedResult()
   Does the result code represent Failure
   Uses SUCCESS() and FAIL() macros and the S_FALSE value
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL IsFailedResult( HRESULT hr )
{
    if( hr = S_FALSE)
        return(TRUE);

    if( hr == S_OK )
        return(FALSE);

    return( FAILED( hr ) );

//    if( FAILED( hr ) ) 
//        return(TRUE);
//    return(FALSE);
}


/*------------------------------------------------------------------------
   CStringFormatErrorCode()

   Format the error message represented by hErrorCode.

   Written By: Jim Hill 
------------------------------------------------------------------------*/
BOOL CStringFormatErrorCode( CString& s, HRESULT hErrorCode )
{
    LPTSTR  lpFormatMessageBuffer = NULL;
    DWORD   dwFormatMessage;
  
    dwFormatMessage= FormatMessage(
                         FORMAT_MESSAGE_ALLOCATE_BUFFER
                         | FORMAT_MESSAGE_FROM_SYSTEM,
                         NULL, hErrorCode, LANG_NEUTRAL,
                         (LPTSTR) &lpFormatMessageBuffer, 0, NULL );

    if( dwFormatMessage && lpFormatMessageBuffer != NULL )
    {
        s = lpFormatMessageBuffer;
    }
    else 
        s.Empty();

    //-- FREE BUFFERS
    if( dwFormatMessage && lpFormatMessageBuffer != NULL )
        LocalFree(lpFormatMessageBuffer);

    return(dwFormatMessage);
}


/*----------------------------------------------------------------------------
   LeftTrimNumericString()
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL LeftTrimHexString(CString& sText ) 
{
    CString s;
    TCHAR szBuffer[259];
    TCHAR *endptr  =NULL;
    DWORD dwLength = 0;
    DWORD dwValue = 0;

    // DISABLE 5/20/99 
    return(TRUE);


    dwLength = sText.GetLength();
    if( dwLength <= MAX_LENGTH_HEX_DISPLAY_STTRING )      // dwMaxLength
        return(TRUE);

    memset( szBuffer, 0, sizeof(szBuffer) );
    vpstrncpy( szBuffer, (LPCTSTR)sText , sizeof(szBuffer));

    //if( szBuffer[0] == (unsigned short) _T("0") && (szBuffer[1] == (unsigned short)_T("x") ||  szBuffer[1] == (unsigned short)_T("X")) )
    //if( a == _T(0) && (b == _T("x") ||  b == _T("X")) )
    if( szBuffer[0] == 0 && (szBuffer[1] == 'x' ||  szBuffer[1] == 'X') )
    {
        dwLength= STRLEN(&szBuffer[2]);    
        if( dwLength <= MAX_LENGTH_HEX_DISPLAY_STTRING )      //dwMaxLength
            return(TRUE);

        dwValue = STRTOUL( (LPCTSTR)sText, &endptr, 10 );
        s.Format(_T("0x%0d"), MAX_LENGTH_HEX_DISPLAY_STTRING);
        s += _T("x");
        sText.Format( s, dwValue);
    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
   StripXHeader()

   Remove the category header from the X-Header string.
   Return all text after the partial header in sDisplayName.

   Example: "X-Platform-User" becomes just "User"

   Error: If an error occurs, it copies all of sSrc to sDisplayName.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL StripXHeader( CString& sCategoryName, CString& sSrc, CString& sDisplayName )
{
    int   iLength = 0;

    try
    {
        if( sSrc.IsEmpty() )
            throw;

        // STRIP OFF THE "X-Platform-" PART
        iLength  = sCategoryName.GetLength();
        if( iLength < sSrc.GetLength() )
            sDisplayName = ((LPCTSTR)sSrc) + iLength;
        else
            sDisplayName = sCategoryName;

        return(TRUE);

    }
    catch(...)
    {
        sDisplayName = sCategoryName;
    }

    return(FALSE);
}



/*------------------------------------------------------------------------
   IsAddressValid()
   Test if the ptr will generate an access violation or not, by
   dereferencing it inside a try-except exception handler.

   Written By: Jim Hill 
------------------------------------------------------------------------*/
BOOL IsAddressValid(void *lpPtr, DWORD dwBytes)
{
   DWORD  dwValue,rc;

   // DO THE EASY ONE
   if(lpPtr==NULL)
      return(FALSE);

   // ASSUME TRUE
   rc=TRUE;

    // TRY
    try
    {
        dwValue=(BYTE) *((BYTE *)lpPtr);
        if(IsBadReadPtr(lpPtr, dwBytes))
            rc=FALSE;
        if(IsBadWritePtr(lpPtr, dwBytes))
            rc=FALSE;

    }  // END TRY BLOCK

    // CATCH
    catch(...) 
    {
       rc=FALSE;
    } // END CATCH BLOCK

   return(rc);
}


/*----------------------------------------------------------------------------
   ShowErrorMessage()


   Written by: Jim Hill
----------------------------------------------------------------------------*/
int ShowErrorMessage(LPCTSTR lpsErrorMsg, DWORD dwCaptionID, DWORD dwFlags, LONG lErrorCode,
                      LPCTSTR lpsServerName, LPCTSTR lpsHeader, LPCTSTR lpsTailer)
{
    CString s1,s2,sCaption,sServerName, sErrorMsg;   // ,sHeader,sTailer;
    TCHAR szText[5];
    BOOL bRet = 0;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    szText[0] = 0;
    if( lpsServerName == NULL)
        sServerName    = g_ServerDisplayName;   //_T(""); // lpsServerName
    else
        sServerName    = lpsServerName;
    if( lpsHeader     == NULL)
        lpsHeader      = _T(""); 
    if( lpsTailer     == NULL)
        lpsTailer      = _T(""); 
    if( lpsErrorMsg   == NULL )
        lpsErrorMsg    = _T("");           //   lpsErrorMsg = szText;   MB_ICONERROR | MB_OKCANCEL
    if( !dwCaptionID )
        dwCaptionID = IDS_ERROR;
    if( !s2.LoadString(dwCaptionID) )
        s2 = _T("");
    if( dwFlags == 0 )
        dwFlags = MB_ICONERROR | MB_OK;
    else if( !(dwFlags & MB_OKCANCEL) && !( dwFlags | MB_YESNO ))
        dwFlags = dwFlags | MB_OK;

    sCaption.Format(_T("%s  %s")  , sServerName, s2);  
    s1.Format(      _T("%s %s %s"), lpsHeader, lpsErrorMsg, lpsTailer);
    s1.TrimLeft();
    s1.TrimRight();
    s2.Format(      _T("%s: %s")  , sCaption, s1);

    // REMOVE THE CRLFs
    s2.Replace('\r', ' ');
    s2.Replace('\n', ' ');

    sErrorMsg.Empty();
    if( IsFailedResult( lErrorCode ) )
    {
        bRet = CStringFormatErrorCode( sErrorMsg, lErrorCode);
    }
    
    if( sErrorMsg.IsEmpty() )
        fWidePrintString("%s  Error= 0x%x", s2, lErrorCode);   
    else
        fWidePrintString("%s  ERROR=%s  0x%x", s2, sErrorMsg, lErrorCode);   

    int iRet  = 0;
    HWND hWnd;
    hWnd = GetActiveWindow();
    if( hWnd == NULL ) 
        hWnd = g_hMainWnd;
    // if( g_hMainWnd == NULL )
    //     hWnd = GetActiveWindow();       //   MB_ICONSTOP
    // else
    //     hWnd = g_hMainWnd;
    //if(hWnd == NULL)
    //    GetMainWindow( &hWnd );     // ((CSnapper1Data*)m_pSnapin)->GetComponentData()->m_spConsole->GetMainWindow( &hWnd );

    //dwFlags |=  WS_EX_TOPMOST;        // TRY PUTTING IT ON TOP But causes function to fail

    iRet=MessageBox( hWnd, s1, sCaption, dwFlags ); 

    // TRANSLATE
    if( iRet == IDYES )
        iRet  = IDOK;
    if( iRet == IDNO )
        iRet  = IDCANCEL;

    return(iRet);
}



/*----------------------------------------------------------------------------
   GetLastErrorText()

   Looks up error text from system.
   Adds it to the end of the existing string.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
DWORD GetLastErrorText(CString& sGetLastErrorText, DWORD dwClearString = FALSE)
{
    DWORD   dwFormatMessage = 0;
    DWORD   dwLastError = 0;
    LPTSTR  lpFormatMessageBuffer = NULL;
    CString s;

    if( dwClearString )
        s.Empty();

    dwLastError=GetLastError();
    if( dwLastError != 0 )
    {
        dwFormatMessage= FormatMessage(
                       FORMAT_MESSAGE_ALLOCATE_BUFFER
                       | FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL, dwLastError, LANG_NEUTRAL,
                       (LPTSTR) &lpFormatMessageBuffer, 0, NULL );
        if( dwFormatMessage && lpFormatMessageBuffer != NULL)
           s.Format(_T("%s \r\n"), (LPCTSTR) lpFormatMessageBuffer);
        else
           s.Format(_T("Error: 0x%x\r\n"),dwLastError);

        if( !sGetLastErrorText.IsEmpty() )
            sGetLastErrorText += _T(" ");
        sGetLastErrorText += s;

        //-- FREE BUFFERS
        if(dwFormatMessage && lpFormatMessageBuffer != NULL)
            LocalFree((HLOCAL)lpFormatMessageBuffer);

        // CLEAR THE ERROR
        SetLastError(0);
    }

    return(dwLastError);
}







///////////////////////////////////////////////////////////////////////////////
// 4/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
static BOOL CALLBACK SetDefaultFontEnumProc( HWND hWnd, LPARAM lParam )
{
    //CCommonPropPage* pThis = (CCommonPropPage*) lParam;
    
    // 
    // Set the font for this control.
    // 
    CWnd* pWnd = CWnd::FromHandle( hWnd );
    if( pWnd )
        {
        pWnd->SetFont( CFont::FromHandle( (HFONT) GetStockObject( DEFAULT_GUI_FONT ) ) );
        }

    return TRUE;
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill                                          
----------------------------------------------------------------------------*/
BOOL  mySetDefaultFont( HWND hParentWin )
{
    try
    {
        if( hParentWin == NULL )
            return TRUE;
        
        // Set all controls to the correct font.
        // EnumChildWindows( GetSafeHwnd(), SetDefaultFontEnumProc, (LPARAM) this );
        EnumChildWindows( hParentWin, SetDefaultFontEnumProc, (LPARAM) NULL );
    }       
    catch(...)
    {
    }
    return TRUE;
}





    // EXTRACT THE PORT IF IT'S THERE
    //.......

#include  <winsock.h>
/*----------------------------------------------------------------------------
   ValidateHostnameorIpAddress()

   Written by: Jim Hill                                          
----------------------------------------------------------------------------*/
int ValidateHostnameOrIpAddress( LPTSTR lpsAddrString )
{
    struct sockaddr_in saDestAddr;
    char   szAddr[259];
    int    iRet = TRUE;
    CString  s;

    if( lpsAddrString == NULL )
        return(FALSE);


#ifdef _UNICODE
     iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)lpsAddrString,-1,
                                (LPSTR)szAddr,sizeof(szAddr),NULL,NULL);
     if( iRet == FALSE )
        return(FALSE);
#else
    vpstrncpy( szAddr, lpsAddrString, sizeof(szAddr) );
#endif

    // INITIALIZE
    memset((void *)&saDestAddr,0,sizeof(struct sockaddr));
    saDestAddr.sin_addr.s_addr = inet_addr(szAddr);
    if( saDestAddr.sin_addr.s_addr == INADDR_NONE )
    {

        return(FALSE);
    }
    return(iRet);
}


/*----------------------------------------------------------------------------
   myMemsetW()

   Clear only 1st byte of wide chars. memset sets both bytes
                                           Does this func work under _MBCS
   Written by: Jim Hill                    _isleadbyte() 
----------------------------------------------------------------------------*/
void myMemsetW(LPTSTR lpsString, TCHAR c, int iLen)
{
    LPTSTR  p = lpsString;
    for( int i = 0; i < iLen && *p ; i++ )
    {
        *p = c;
        p=CharNext(p);   
    }
    return;
}
















#if 0
/*----------------------------------------------------------------------------
   ValidateHttpAddressScheme()

   InternetCrackUrlA() can't handle the case of some of the 
   letters in "http://" at the start of the address, to be missing.

   Don't call InternetCrackUrlA if "http://" is missing.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
int ValidateHttpAddressScheme(LPTSTR lpsHttpAddrString, DWORD *lpdwStringModified, CString& sErrorString)
{
    LPTSTR   lpszGetBuffer  = NULL;
    CString  sAddr, s1, sHttpSchemeName;
    TCHAR    *p = NULL;
    int      iLen   = 0;
    int      iRet   = IDCANCEL;
    BOOL     bModifyString = FALSE;
    BOOL     bAddHttpString = FALSE;

    // VALIDATE
    if(lpsHttpAddrString==NULL)
        return(IDCANCEL);

    // INITIALIZE
    sHttpSchemeName = _T("http://");
    s1 = lpsHttpAddrString;
    s1.TrimLeft();

    // CONVERT ANY BACKWARD SLASHES
    p = lpsHttpAddrString;
    for( int i = 0; *p && i <= 7 ; i++ )
    {
         TCHAR c = *p;
         if( c == '\\')
         {
             c = '/';
             bModifyString = TRUE;
             bAddHttpString = TRUE;
         }

    }
    // LOOK FOR THE HTTP
    if( s1.Left(7) != sHttpSchemeName )
    {
        {
            // DOES IT HAVE A SINGLE SLASH OR A : NEAR THE BEGINNING
            lpszGetBuffer = s1.GetBuffer(0);
            iLen = s1.GetLength();
            p = lpszGetBuffer;
            for(int i = 0; *p && i < iLen; i++)
            {
                TCHAR  c = *p;
                if( c == '/' || c == ':' )
                {
                    myMemsetW(lpszGetBuffer, ' ', i + 1 );
                    iRet = IDOK;
                    bModifyString  = TRUE;
                    bAddHttpString = TRUE;
                    break;
                }
                else if( c == '.' )     // WENT TOO FAR OR THE ADDRESS IS REALLY MESSED UP
                {
                    iRet          = IDABORT;
                    bModifyString = TRUE;
                    bAddHttpString = TRUE;
                    break;
                }
                p=CharNext(p);   
            }
            s1.ReleaseBuffer(-1);
            s1.TrimLeft();
            s1.TrimLeft(_T(":/"));      // REMOVE IF ON THE FRONT OF THE STRING

        }

        // DID WE TRIM IT WITH SPACES?
        // if( iRet == IDOK || bModifyString )
        if( bAddHttpString )
        {
            s1.TrimLeft();
            sAddr.Format(_T("%s%s"),sHttpSchemeName,s1);            // ADD IT BACK ON
            STRCPY(lpsHttpAddrString, (LPCTSTR) sAddr );
        }

    }
    else
        iRet = IDOK;

    if (lpdwStringModified != NULL)
        *lpdwStringModified = bModifyString;

    return(iRet);
}
#endif



#if 0

#include "wininet.h"	 
//    2 is ERROR_FILE_NOT_FOUND 
//  120 is ERROR_CALL_NOT_IMPLEMENTED        
// http://avis.symantec.com:80            
/*----------------------------------------------------------------------------
   ValidateHttpAddress()

   Validates that an http address has the proper form and a port number
   Expects both parms to be 512 bytes long.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
int ValidateHttpAddress(LPTSTR lpsAddrString, DWORD *lpdwPort, LPTSTR lpsErrorString )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

    URL_COMPONENTSA  Url;
    char     szScheme[INTERNET_MAX_SCHEME_LENGTH+5], szHostName[INTERNET_MAX_HOST_NAME_LENGTH+1];
    char     szUrlPath[MAX_PATH], szExtraInfo[MAX_PATH];
    //
    char     szAddrString[MAX_PATH + MAX_PATH], szConvertedAddrString[MAX_PATH + MAX_PATH];
    TCHAR    szWideAddrString[MAX_PATH];
    DWORD    dwFlags = 0;
    BOOL     bRet = 0;
    int      iRet = IDOK;
    int      iLen = 0;
    DWORD    cError = 0;
    DWORD    dwFatalError = 0;
    DWORD    dwLastError = 0;
    DWORD    dwBufferLength = sizeof(szWideAddrString);
    DWORD    dwStringModified = 0;
    CString  sErrorString, sGetLastErrorText, s, s2;

    // INITIALIZE
    memset(szScheme,0,sizeof(szScheme));
    memset(szHostName,0,sizeof(szHostName));
    memset(szUrlPath,0,sizeof(szUrlPath));
    memset(szExtraInfo,0,sizeof(szExtraInfo));
    memset(&Url,0,sizeof(Url));
    memset(szAddrString,0,sizeof(szAddrString));
    memset(szConvertedAddrString,0,sizeof(szConvertedAddrString));
    memset(szWideAddrString,0,sizeof(szWideAddrString));
    if(lpdwPort != NULL)
        *lpdwPort = 0;
    sErrorString.Empty();

//_ASSERTE(0 == "InternetCrackUrl");

    try
    {
        iRet = ValidateHttpAddressScheme(lpsAddrString, &dwStringModified, sErrorString);
        if( iRet != IDOK )
        {
            dwLastError = GetLastErrorText(sGetLastErrorText, TRUE);    
            s.LoadString(IDS_ERROR_HTTPSTRING_NOT_FOUND);      //  'http://' is not recognizable at the beginning of the address.\r\n
            sErrorString += s;                       

            throw dwFatalError;
        }
        if( dwStringModified )
        {
            s.LoadString(IDS_ERROR_HTTPSTRING_CORRECTED);          // 'http://' string was corrected at the beginning of the address.\r\n
            sErrorString += s;                       
            ++cError;
        }
    
        // MAKE A CANONICAL VERSION
		// dwFlags = ICU_NO_ENCODE | ICU_DECODE | ICU_NO_META | ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE;
        // dwLastError=GetLastError();
        // bRet = InternetCanonicalizeUrl( lpsAddrString, szWideAddrString, &dwBufferLength, dwFlags);
        // if( bRet == FALSE )
        // {
        //     dwLastError=GetLastError();
        //     STRCPY(szWideAddrString, lpsAddrString);
        // }
        vpstrncpy(szWideAddrString, lpsAddrString, sizeof(szWideAddrString));
    
    
        // CONVERT TO ASCII       strlen
        // iLen = STRLEN(szWideAddrString);
        SetLastError(0);
        bRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)szWideAddrString,-1,
                                  (LPSTR)szAddrString,sizeof(szAddrString),NULL,NULL);
        if( bRet == FALSE )
        {
            s.LoadString(IDS_ERROR_WIDETOMBCS);          //  "Conversion error: WideCharToMultiByte.  "
            sErrorString += s;
            dwLastError = GetLastErrorText(s, TRUE);
            if( dwLastError )
                sErrorString += s;
            else
                sErrorString += _T("\r\n");
            throw dwFatalError;
        }
    
    
        // PARSE URL. WE'RE IN MULTIBYTE ASCII ONLY. WIDE CHAR VERSION OF InternetCrackUrl DOES NOT WORK
        Url.dwStructSize      = sizeof(Url);
        Url.lpszScheme        = (LPSTR)szScheme;
        Url.dwSchemeLength    = sizeof(szScheme);
        Url.nScheme           = INTERNET_SCHEME_HTTP;
        Url.lpszHostName      = (LPSTR)szHostName;
        Url.dwHostNameLength  = sizeof(szHostName);
        Url.lpszUrlPath       = (LPSTR)szUrlPath;
        Url.dwUrlPathLength   = sizeof(szUrlPath);
        Url.lpszExtraInfo     = (LPSTR)szExtraInfo;
        Url.dwExtraInfoLength = sizeof(szExtraInfo);
        iLen = strlen((LPSTR)szAddrString);    // wcslen(szAddrString);
        SetLastError(0);
        bRet = InternetCrackUrlA((LPSTR)szAddrString, iLen, 0, &Url);
        if( bRet == FALSE )
        {
            dwLastError = GetLastErrorText(sGetLastErrorText, FALSE);
            s.LoadString( IDS_ERROR_UNABLE_TO_PARSEURL );       // "Unable to parse URL: \r\n"
            sErrorString += s;
            throw dwFatalError;
        }
    
        if( Url.nScheme != INTERNET_SCHEME_HTTP || Url.lpszScheme == NULL || *Url.lpszScheme == '\0')
        {
            Url.nScheme = INTERNET_SCHEME_HTTP;
            strcpy( (LPSTR )szScheme , "http");
            s.LoadString( IDS_ERROR_MISSING_HTTP_SCHEME );
            sErrorString += s;
            throw dwFatalError;
        }

        // REMOVE SPACES IN HOST NAME
        s = (LPSTR)szHostName;
        if( s.Remove(' ') )
            WideCharToMultiByte(CP_ACP,0,s.GetBuffer(0),-1,
                               (LPSTR)szHostName,sizeof(szHostName),NULL,NULL);
        s.ReleaseBuffer(-1);
        
        //ValidateHostnameOrIpAddressA( szHostName );
        

//         // CHECK THAT THE PORT NUMBER APPEARS IN THE ORIGINAL STRING
//         DWORD nPort = Url.nPort;
//         if( nPort != 0 )
//         {
//             // DID THE ORIGINAL STRING SHOW THIS PORT NUMBER
//             s.Format(_T(":%d"), nPort);
//             s2 = lpsAddrString;
//             if( s2.Find(s) == -1 )
//                 nPort = 0;           // SET TO 0 TO FORCE NEXT SECTION
//         }
//         if( nPort == 0 )      
//         {
//             s.LoadString(IDS_ERROR_MISING_PORT);
//             sErrorString += s;
//             ++cError;
//         }

        dwBufferLength = sizeof(szConvertedAddrString);
        SetLastError(0);
        bRet = InternetCreateUrlA(&Url, ICU_ESCAPE, szConvertedAddrString, &dwBufferLength );
        if( bRet == FALSE )
        {
            dwLastError = GetLastErrorText(sGetLastErrorText, FALSE);
            strcpy(szConvertedAddrString, szAddrString);
        }


        SetLastError(0);
        bRet= MultiByteToWideChar(CP_ACP,0,szConvertedAddrString,-1,szWideAddrString,sizeof(szWideAddrString));   
        if( bRet == FALSE )
        {
            s.LoadString(IDS_ERROR_MBCSTOWIDE);        // "Conversion error: MultiByteToWideChar."
            sErrorString += s;
            dwLastError = GetLastErrorText(s, TRUE);
            if( dwLastError )
                sErrorString += s;
            else
                sErrorString += _T("\r\n");
            throw dwFatalError;
        }


    }
    catch(...)
    {
         ++cError; 
         ++dwFatalError; 
    }

    if( dwFatalError )
        ++cError; 

    if( cError || dwFatalError)
    {
        DWORD   dwFlags     = MB_ICONERROR | MB_OKCANCEL;
        DWORD   dwCaptionID = IDS_ERROR_PARSINGURL;               
        if( dwFatalError )
        {
             dwFlags     = 0;
             dwCaptionID = IDS_ERROR_PARSINGURL_FATAL;
        }

        if( !sGetLastErrorText.IsEmpty() )
        {
             s.LoadString(IDS_ERROR_SYSTEM_ERROR);      // "System errors: "
             sErrorString += s;
             sErrorString += sGetLastErrorText;
        }

        if( dwFatalError )
        {                             
             s.LoadString(IDS_ERROR_PLEASE_REENTER);    //  "\r\nErrors were detected, please re-enter the address\r\n"
             sErrorString += s;
        }
        else
        {
             dwFlags = MB_ICONERROR | MB_YESNO;          
             s.LoadString(IDS_ERROR_SAVE_ANYWAY);       // "\r\nErrors were detected. Do you want to save it anyway?\r\n"
             sErrorString += s;
        }

        iRet = ShowErrorMessage( (LPCTSTR) sErrorString, dwCaptionID, dwFlags, 0, NULL, NULL, NULL);

        // IF FATAL, DON'T RETURN THE MESS
        if( dwFatalError )
            return(IDABORT);

        // DO THEY WANT TO PROCEED
        // if( iRet == IDCANCEL)             // IDOK  
        //     return(IDCANCEL);

    }
    else
    {
        if(lpdwPort != NULL)
            *lpdwPort = Url.nPort;
    }

    // TRIM THE TRAILING SLASH AND ANYWHITE SPACE ON THE RIGHT
    s = szWideAddrString;
    s.TrimRight(_T(" /"));

    // RETURN THE STRING
    STRCPY( lpsAddrString, (LPCTSTR) s );   // szWideAddrString

    return(iRet);
}
#endif


#if 0
/*----------------------------------------------------------------------------
   ParseHttpAddress()


   Modes of operation:
   If sHttpAddrStringAndPort is not empty, then parse it into components.
   Save the Http address without the port in sHttpAddrStringOnly.
   Save the port number to lpdwPort.
   
   If sHttpAddrStringAndPort is empty, then build it from the two components:
   sHttpAddrStringOnly and lpdwPort

   Written by: Jim Hill                       , LPTSTR lpsErrorString
----------------------------------------------------------------------------*/
int ParseHttpAddress( CString& sHttpAddrStringAndPort, CString& sHttpAddrStringOnly, DWORD *lpdwPort, URL_COMPONENTSA *lpUrl = NULL)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

    URL_COMPONENTSA  Url;
    char     szScheme[INTERNET_MAX_SCHEME_LENGTH+5], szHostName[INTERNET_MAX_HOST_NAME_LENGTH+1];
    char     szUrlPath[MAX_PATH], szExtraInfo[MAX_PATH];
    char     szAddrString[MAX_PATH + MAX_PATH], szConvertedAddrString[MAX_PATH + MAX_PATH];
    TCHAR    szWideAddrString[MAX_PATH];
    BOOL     bRet = 0;
    int      iRet = IDOK;
    int      iLen = 0;
    DWORD    dwModeSplit  = 0;
    DWORD    dwFatalError = 0;
    DWORD    dwLastError = 0;
    DWORD    dwBufferLength = sizeof(szWideAddrString);

    // INITIALIZE
    if(lpdwPort != NULL)
        *lpdwPort = 0;
    if( lpUrl == NULL)
        lpUrl = &Url;
    memset(szScheme,0,sizeof(szScheme));
    memset(szHostName,0,sizeof(szHostName));
    memset(szUrlPath,0,sizeof(szUrlPath));
    memset(szExtraInfo,0,sizeof(szExtraInfo));
    memset(lpUrl,0,sizeof(Url));
    memset(szAddrString,0,sizeof(szAddrString));
    memset(szConvertedAddrString,0,sizeof(szConvertedAddrString));
    memset(szWideAddrString,0,sizeof(szWideAddrString));

    try
    {
        if(sHttpAddrStringAndPort.IsEmpty())
           dwModeSplit = FALSE;
        else
           dwModeSplit = TRUE;




    }
    catch(...)
    {
         ++dwFatalError; 
    }




    return(iRet);
}
#endif



#if 0

    CString  strServer, strObject; 
    DWORD dwServiceType = 0;
    INTERNET_PORT nPort = 0;
    BOOL     bRet = 0;
    DWORD    dwLastError = 0;

    bRet = AfxParseURL(lpsAddrString, dwServiceType, strServer, strObject, nPort );  

    dwLastError=GetLastError();
#endif



