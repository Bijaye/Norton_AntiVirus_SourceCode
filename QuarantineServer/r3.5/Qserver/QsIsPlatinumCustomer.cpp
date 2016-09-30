/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/*----------------------------------------------------------------------------
    QsIsPlatinumCustomer

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/



#include <windows.h>
#include "stdafx.h"
#include <TCHAR.h>
#include "SymSaferRegistry.h"
#include "vpstrutils.h"



#define PLATINUM_PRIORITY                   0
#define REGKEY_HKLM_SW_NAV_QUARANTINE       _T("SOFTWARE\\Symantec\\Symantec AntiVirus\\Quarantine")
#define REGVAL_EMAIL_SERVER_PRIORITY        _T("EmailServerPriority")

// If REGKEY_HKLM_SW_NAV_QUARANTINE contains a ValueName of "EmailServerPriority"
//    0 means Platinum
//    1 means Gold

LPTSTR striinc(LPTSTR str1,LPTSTR str2);



/*----------------------------------------------------------------------------
    IsPlatinumCustomer()

    Determine if this is a Platinum customer:
    If "SOFTWARE\\Symantec\\Norton AntiVirus\\Quarantine" contains the ValueName
    "EmailServerPriority", and it has a value of 0, then this is a Platinum customer.

    Value defines for "EmailServerPriority"
    0 means Platinum
    1 means Gold
    2 means Retail

    All DIS customers are by definition either Platinum or Gold.

    If this is a Symantec Platinum customer, we want to insure that the string "Platinum"
    appears in the value for X-Customer-Identifier. If the sample is deferred to SND, 
    the word "Platinum" will cause SND to place it in the Platinum queue. 
    Otherwise all DIS defferred samples end up in the Gold queue.

    Written by: Jim Hill 7/20/01
 ----------------------------------------------------------------------------*/
BOOL IsPlatinumCustomer()
{
    BOOL            bResult = FALSE;
    DWORD           dwRegValueType = 0;
    DWORD           dwDataBuf = 0;
    DWORD           dwSize = 0;
    HKEY            hKey = NULL;
    long            lResult = 0;

    // OPEN THE REGISTRY 
    lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE              // handle of open key
                           ,REGKEY_HKLM_SW_NAV_QUARANTINE   // subkey to open
                           , 0                              // 
                           , (KEY_READ | KEY_WRITE)         // security access mask
                           , &hKey);                        // address of handle of open key
    if (lResult != ERROR_SUCCESS)
    {
        if( hKey != NULL)
            RegCloseKey(hKey);
        return(FALSE);
    }

    // GET THE VALUE OF "EmailServerPriority" 
    dwSize  = sizeof(DWORD);

	lResult = SymSaferRegQueryValueEx(hKey                          // handle of key to query
                            , REGVAL_EMAIL_SERVER_PRIORITY  // name of value to query
                            , 0                             // 
                            , &dwRegValueType               // buffer for value type
                            , (LPBYTE)&dwDataBuf            // data buffer
                            , &dwSize);                     // data buffer size

    if ((lResult != ERROR_SUCCESS)  ||  (dwRegValueType != REG_DWORD) )
    {
        if( hKey != NULL)
            RegCloseKey(hKey);
        return(FALSE);
    }

    if( dwDataBuf == PLATINUM_PRIORITY )
        bResult = TRUE;

    if( hKey != NULL)
        RegCloseKey(hKey);

    return( bResult );
}



#define REGKEY_QSERVER_AVIS                       _T("SOFTWARE\\Symantec\\Quarantine\\Server\\Avis")
#define REGVAL_AVIS_CUSTOMER_IDENTIFIER           _T("customerIdentifier")
#define REGVAL_AVIS_CONFIGURATION_CHANGE_COUNTER  _T("configurationChangeCounter")
#define PLATINUM_STRING                           _T("Platinum")

/*----------------------------------------------------------------------------
    AddPlatinumCustomerType()

    If this is a Symantec Platinum customer, insure that the string "Platinum"
    appears in the value for "customerIdentifier". If the sample is deferred to SND, 
    the word "Platinum" will cause SND to place it in the Platinum queue. 
    Otherwise all DIS defferred samples end up in the Gold queue.

    Value name "customerIdentifier" at SOFTWARE/Symantec/Quarantine/Server/Avis to 
    include this string. Any change to the values requires the ValueName 
    "configurationChangeCounter" to be incremented by 1.

    Written by: Jim Hill 7/20/01                         
 ----------------------------------------------------------------------------*/

BOOL AddPlatinumCustomerType(void)
{
    BOOL            bResult = FALSE;
    DWORD           dwRegValueType = 0;
    TCHAR            szDataBuff[255] = {0};
    TCHAR            szCurrentCustomerID[255] = {0};
//    char            szNewCustomerID[255] = {0};
    DWORD           dwSize = 0;
    HKEY            hKey = NULL;
    long            lResult = 0;

    // IS THE WORD PLATINUM ALREADY IN THE STRING? 
    // Platinum customers have been told to manually add it at QConsole.


    // OPEN THE REGISTRY 
    lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE              // handle of open key
                           ,REGKEY_QSERVER_AVIS             // subkey to open
                           , 0                              // 
                           , (KEY_READ | KEY_WRITE)         // security access mask
                           , &hKey);                        // address of handle of open key
    if(lResult != ERROR_SUCCESS)
    {
        if( hKey != NULL)
            RegCloseKey(hKey);
        return(FALSE);
    }

    // GET THE VALUE OF "customerIdentifier" 
    dwSize  = sizeof(szCurrentCustomerID);
    lResult = SymSaferRegQueryValueEx(hKey                             // handle of key to query
                            , REGVAL_AVIS_CUSTOMER_IDENTIFIER  // name of value to query
                            , 0                                // 
                            , &dwRegValueType                  // buffer for value type
                            , (LPBYTE)szCurrentCustomerID      // data buffer
                            , &dwSize);                        // data buffer size

    if((lResult != ERROR_SUCCESS)  ||  (dwRegValueType != REG_SZ) )
    {
        if( hKey != NULL)
            RegCloseKey(hKey);
        return(FALSE);
    }

    // IS THE WORD PLATINUM ALREADY THERE?
    if( striinc( PLATINUM_STRING, szCurrentCustomerID ) != NULL )
    {
        if( hKey != NULL)
            RegCloseKey(hKey);
        return(TRUE);
    }

    // IF NOT, THEN LOOK FOR THE REG KEY "EmailServerPriority"
    if( IsPlatinumCustomer() )
    {
        // IF TRUE, THEN ADD THE STRING "Platinum"
        vpstrncpy( szDataBuff, PLATINUM_STRING, sizeof(szDataBuff) );
        vpstrnappend( szDataBuff, szCurrentCustomerID , sizeof(szDataBuff));

        lResult = RegSetValueEx(hKey                             // handle of key 
                              , REGVAL_AVIS_CUSTOMER_IDENTIFIER  // name of value to set
                              , 0                                 
                              , REG_SZ
                              , (LPBYTE)szDataBuff               // string to write
                              , _tcslen(szDataBuff) );

        if( lResult != ERROR_SUCCESS )
        {
            if( hKey != NULL)
                RegCloseKey(hKey);
            return(FALSE);
        }

        // NOW WE HAVE TO INCREMENT THE CHANGE COUNTER
        dwSize  = sizeof(szDataBuff);
        lResult = SymSaferRegQueryValueEx(hKey                                      // handle of key to query
                                , REGVAL_AVIS_CONFIGURATION_CHANGE_COUNTER  // name of value to query
                                , 0                                
                                , &dwRegValueType                           // buffer for value type
                                , (LPBYTE)szDataBuff                        // data buffer
                                , &dwSize);                                 // data buffer size
        
        if((lResult != ERROR_SUCCESS)  ||  (dwRegValueType != REG_SZ) || _tcslen(szDataBuff) == 0 )
        {
            if( hKey != NULL)
                RegCloseKey(hKey);
            return(FALSE);
        }
USES_CONVERSION;
        // INCREMENT IT
        int iChangeCounter = atoi(T2A( szDataBuff) );
        ++iChangeCounter;
        wsprintf( szDataBuff, _T("%d"), iChangeCounter );
        lResult = RegSetValueEx(hKey                                      // handle of key 
                              , REGVAL_AVIS_CONFIGURATION_CHANGE_COUNTER  // name of value to set
                              , 0                                 
                              , REG_SZ
                              , (LPBYTE)szDataBuff                        // string to write
                              , _tcslen(szDataBuff) );


        if( lResult == ERROR_SUCCESS )
        {
            if( hKey != NULL)
                RegCloseKey(hKey);
            return(TRUE);
        }

    }

    if( hKey != NULL)
        RegCloseKey(hKey);
    return( FALSE );
}



/*----------------------------------------------------------------------------
   STRIINC()
      
   determines if string1 is included in string2
   no regard for case

   Returns:   The address where string1 is included in string2.
              Else NULL if not included.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
LPTSTR striinc(LPTSTR str1,LPTSTR str2)
{
    register int  max;
    LPTSTR        p;

    if( str1 == NULL || str2 == NULL )
        return(NULL);

    max=_tcslen( str1 );

    for( p = str2; *p; ) {
       if( _tcsncicmp( str1, p, max ) == 0 )
         return(p);
       p = CharNext(p);   
    }
    return(NULL);                       /* string1 not found in string2 */
}








