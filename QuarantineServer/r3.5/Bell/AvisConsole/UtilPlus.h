/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*------------------------------------------------------------------------
   UTILPLUS.H
   
   C++ Utility routines 
   
   Written by Jim Hill
------------------------------------------------------------------------*/



#ifndef _UTILPLUS_H_DEFINED
   #define  _UTILPLUS_H_DEFINED

    // #include  "InputRange.h"

    int GetNextObjectID();
    BOOL LookupResourceString(CString& sText, DWORD dwID );

    // DEBUG 12/29/99
    DWORD SaveCurrentThreadId();
    extern DWORD g_dwCurrentThreadID;

    // ACCESS STRING TABLE IN ICEPACKTOKEN.DLL
    BOOL StrTableLoadString( DWORD dwGlobalResID, CString& s );

    // WRAPPER
    BOOL StrLookUpIcePackTokenString( CString& sToken, CString& s, DWORD dwTableID );

    // XOR THE FIREWALL PASSWORD
    int EncryptDecryptPassword(CString& sPassword);


    // GLOBAL INSTALL FLAG: WAS QSERVER INSTALLED WITH AVIS IcePack OR WITH OLD S&D?
    BOOL  IsAvisIcePackSupported( int iExtObjectID );   
    DWORD SaveQServerVersion(int iExtObjectID);   
    DWORD GetCurrentSelectedQserverVersion();
    //int ClearServerVersionNumber();



    // REGISTERED WINDOW MESSAGES
    extern   DWORD dwWmsgSConfigWriteData;
    #define  REGISTERED_WMSG_SCONFIG_WRITE_DATA      _T("AvisSConfigWriteData")

    BOOL ConvertVariantToString(CString& sText, VARIANT *pv, DWORD dwAttributeFormat = 0 ); 
    BOOL ConvertVariantToDword( DWORD *lpdwValue, VARIANT *pv);
    BOOL IsVariantDateValid( VARIANT *pv );
    BOOL InitializeVariantWithData( DWORD *lpdwValue, LPCTSTR lpBSTR, VARIANT *pv);
    BOOL IsAttributeDeleted( VARIANT * pValue );


    BOOL IsSuccessfullSampleResult( HRESULT hr );
    BOOL IsSuccessfullResult( HRESULT hr );
    BOOL IsFailedResult( HRESULT hr );
    BOOL CStringFormatErrorCode( CString& s, HRESULT hErrorCode );


    BOOL LeftTrimHexString(CString& sText ); 
    BOOL StripXHeader( CString& sCategoryName, CString& sSrc, CString& sDisplayName );

    BOOL IsAddressValid(void *lpPtr, DWORD dwBytes);

    int ShowErrorMessage(LPCTSTR lpsErrorMsg, DWORD dwCaptionID, DWORD dwFlags = 0, LONG lErrorCode = 0, 
                      LPCTSTR lpsServerName=NULL, LPCTSTR lpsHeader=NULL, LPCTSTR lpsTailer=NULL);

    DWORD GetLastErrorText(CString& lpsGetLastErrorText);

    int ValidateHttpAddress(LPTSTR lpsAddrString, DWORD *lpdwPort, LPTSTR lpsErrorString );

    int ValidateHostnameOrIpAddress( LPTSTR lpsAddrString );

    BOOL  mySetDefaultFont( HWND hParentWin );




// FORMAT DEFINES FOR X_ATTRIBUTES_STRUCT in GetAllAttributes.h
#define AVIS_ATTRIBUTE_FORMAT_USE_DEFAULT                   0
#define AVIS_ATTRIBUTE_FORMAT_HEX_STRING                    1
#define AVIS_ATTRIBUTE_FORMAT_DWORD                         2
#define AVIS_ATTRIBUTE_FORMAT_STRING                        3
#define AVIS_ATTRIBUTE_FORMAT_DATE                          4
#define AVIS_ATTRIBUTE_FORMAT_EXCLUDE                       5
#define AVIS_ATTRIBUTE_FORMAT_LOOKUP_SAMPLE_STATUS_TEXT     7




#endif