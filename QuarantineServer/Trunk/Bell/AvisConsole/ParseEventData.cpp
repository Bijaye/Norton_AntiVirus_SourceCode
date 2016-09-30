/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


// ParseEventData.cpp



#include "stdafx.h"
#include "resource.h"
#include "AvisConsole.h"
#include "AlertingSetConfig.h"
#include "ParseEventData.h"
//#include "AvisEventDetection.h"   // in QuarantineServer\Include 
#include "EventObject.h"
#include "vpstrutils.h"


// LINK IN THE EVENT RULES DATA IN THIS FILE ONLY
#include "AvisEventRulesData.h"


extern HWND g_hMainWnd;





// LOCAL
BOOL ReadEventConfigData( CServerConfigData* pSConfigData, AVIS_EVENT_RULES* lpTableEntry );
BOOL GetNextNumbersFromCommaString( CString& s, DWORD* pNum1, DWORD* pNum2 );
static AVIS_EVENT_RULES* LookupEventRuleByEventId( AVIS_EVENT_RULES* lpTable, DWORD dwEventId );
BOOL ReadEventConfigData( CServerConfigData* pSConfigData, AVIS_EVENT_RULES* lpTableEntry );
BOOL WriteEventConfigData( CServerConfigData* pSConfigData, AVIS_EVENT_RULES* lpTableEntry );
//void SendTestEvent(CServerConfigData* pSConfigData); 



/*----------------------------------------------------------------------------
    SafeDivide()

    Written by: Jim Hill                             , DWORD dwDefaultIfZero
 ----------------------------------------------------------------------------*/
DWORD SafeDivide( DWORD dwOperand, DWORD dwDivisor )
{
    if( dwDivisor == 0 )
        return(0);
    return( dwOperand / dwDivisor );
}

/*----------------------------------------------------------------------------
    GetEventData()

    Written by: Jim Hill                       
 ----------------------------------------------------------------------------*/
BOOL CAlertingSetConfig::GetEventDataByIndex( DWORD dwEventRulesTableID, CEventObject *pEventObj, DWORD* pdwIndex )
{
    AVIS_EVENT_RULES*  lpTableEventRule = NULL;
    CString s;

    try
    {
        // WHICH TABLE?
        if( dwEventRulesTableID == EVENT_TOKEN_GENERAL_ATTENTION_TABLE )
        {
            lpTableEventRule = &g_AvisEventRulesGeneralAttention[*pdwIndex];
        }
        else if( dwEventRulesTableID == EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE )
        {
            lpTableEventRule = &g_AvisEventRulesSampleIntervention[*pdwIndex];
        }
        else
        {
            *pdwIndex = 0xFFFFFFFF;
            return(FALSE);
        }

        if( lpTableEventRule->lpszIcePackToken == NULL )
        {
            *pdwIndex = 0xFFFFFFFF;
            return(FALSE);
        }

        // CAN THE USER CONFIGURE THIS ONE? IF NOT, DON'T BOTHER TO READ IT
        if( !lpTableEventRule->dwIsConfigurable )
        {
            *pdwIndex += 1;
            return(FALSE);
        }

        // CAN THE USER CONFIGURE TIME IN STATE?
        if( lpTableEventRule->dwIsConfigMinTimeInState )
        {
            // CONVERT SECS TO MINUTES FOR DISPLAY
            // DWORD dwMinTimeInStateMinutes = SafeDivide( lpTableEventRule->dwMinTimeInStateSecs, 60 );
            pEventObj->m_sTime.Format( _T("%d"), lpTableEventRule->dwMinTimeInStateMinutes );                 

        }
        else
        {
            pEventObj->m_sTime.Empty();
        }

        // GET THE EVENT NAME
        if( lpTableEventRule->lpszEventName == NULL )
            pEventObj->m_sEventName = _T(" ");     
        else
		{
			USES_CONVERSION;
			pEventObj->m_sEventName = W2T(lpTableEventRule->lpszEventName);
		}

        pEventObj->m_bEnabled          = (lpTableEventRule->dwEnabled != 0);
        pEventObj->dwEventRulesTableID = dwEventRulesTableID;         
        pEventObj->dwEventID           = lpTableEventRule->dwEventId;                       
            

        // INC THE INDEX
        *pdwIndex += 1;
        return(TRUE);

    }
    catch(...)
    {
        *pdwIndex = 0xFFFFFFFF;
    }
    return(FALSE);
}

/*----------------------------------------------------------------------------
    PutEventDataByIndex()

    Written by: Jim Hill                       
 ----------------------------------------------------------------------------*/
BOOL CAlertingSetConfig::PutEventDataByIndex( CEventObject *pEventObj, DWORD dwIndex, DWORD* pdwChangedCount )
{
    AVIS_EVENT_RULES*  lpTable = NULL;
    AVIS_EVENT_RULES*  lpTableEventRule = NULL;
    TCHAR*             endptr=NULL;
    DWORD              dwMinTimeInStateSecs = 0;
    DWORD              dwMinTimeInStateMinutes = 0;
    CString            s;

    try
    {
        if( pEventObj == NULL )
            return(FALSE);

        // WHICH TABLE?
        if( pEventObj->dwEventRulesTableID == EVENT_TOKEN_GENERAL_ATTENTION_TABLE )
        {
            lpTable = &g_AvisEventRulesGeneralAttention[0];
        }
        else if( pEventObj->dwEventRulesTableID == EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE )
        {
            lpTable = &g_AvisEventRulesSampleIntervention[0];
        }
        else
        {
            return(FALSE);
        }

        // GET THE RULE
        if( pEventObj->dwEventID == 0 )
            return(FALSE);
        lpTableEventRule = LookupEventRuleByEventId( lpTable, pEventObj->dwEventID );
        if( lpTableEventRule == NULL )
            return(FALSE);

        // HAVE WE CHANGED ANYTHING?
        if( !pEventObj->m_sTime.IsEmpty() )
        {
            dwMinTimeInStateMinutes = STRTOUL( (LPCTSTR)pEventObj->m_sTime, &endptr, 10 );
            dwMinTimeInStateSecs    = dwMinTimeInStateMinutes * 60;
        }
        if( lpTableEventRule->dwEnabled == (DWORD) pEventObj->m_bEnabled && lpTableEventRule->dwMinTimeInStateSecs == dwMinTimeInStateSecs )
            return(TRUE);

        // IS THERE A TIMER RUNNING THAT SHOULD BE CHANGED? 
        // ONLY APPLIES TO GENERAL ATTENTION PROBLEMS WITH THE GATEWAY OR DISKSPACE
        DWORD  dwCurrentGmtTime = (DWORD) TimeToGmtTime( 0 );
        if( lpTableEventRule->dwInStateTimerExpire != 0  && 
            lpTableEventRule->dwEventId == GENERAL_ATTENTION_GATEWAY_COMM_EVENTID &&
            pEventObj->dwEventRulesTableID == EVENT_TOKEN_GENERAL_ATTENTION_TABLE )
        {
            // EXPIRED YET?
            if( dwCurrentGmtTime < lpTableEventRule->dwInStateTimerExpire )
            {
                int iNewTimer = 0;
                int iInStateTimerExpire = (int) lpTableEventRule->dwInStateTimerExpire;
                int iTimeDiference = abs( int (dwMinTimeInStateSecs - lpTableEventRule->dwMinTimeInStateSecs) );
                if( dwMinTimeInStateSecs < lpTableEventRule->dwMinTimeInStateSecs )
                    iNewTimer = iInStateTimerExpire - iTimeDiference;
                else
                    iNewTimer = iInStateTimerExpire + iTimeDiference;
                if( iNewTimer < 0 )
                    iNewTimer = 1;

                // RESTART IT USING THE NEW VALUE
                // lpTableEventRule->dwInStateTimerExpire = dwCurrentGmtTime + dwMinTimeInStateSecs;
                lpTableEventRule->dwInStateTimerExpire = (DWORD) iNewTimer;
            }
        }


        // UPDATE THE TABLE
        lpTableEventRule->dwEnabled               = pEventObj->m_bEnabled;
        lpTableEventRule->dwMinTimeInStateMinutes = dwMinTimeInStateMinutes;
        lpTableEventRule->dwMinTimeInStateSecs    = dwMinTimeInStateSecs;
        lpTableEventRule->bIsDataChanged          = TRUE;

        *pdwChangedCount += 1;

        return(TRUE);
    }
    catch(...)
    {
    }
    return(FALSE);
}

/*----------------------------------------------------------------------------
    TimeToGmtTime()
    Convert passed time value to GMT time.
    If tTime == 0, return Current GMT time

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
time_t TimeToGmtTime( time_t tTime )
{
    if( tTime == 0 )
        tTime = time(NULL);

    // CONVERT TO GMT
    struct tm* pGmtTime = gmtime( &tTime );
    tTime = mktime( pGmtTime );
    return( tTime );
}


/*----------------------------------------------------------------------------
    LookupEventRuleByEventId
    Written by: Jim Hill                                                  
 ----------------------------------------------------------------------------*/
static AVIS_EVENT_RULES* LookupEventRuleByEventId( AVIS_EVENT_RULES* lpTable, DWORD dwEventId )
{
    AVIS_EVENT_RULES*  lpTableEventRule = NULL;

    for( int i = 0; lpTable->lpszIcePackToken != NULL; i++ ) 
    {
        if( lpTable->dwEventId == dwEventId )  
        {
            lpTableEventRule = lpTable;
            break;
        }
        lpTable++;
    }

    return( lpTableEventRule );
}


static bStringsReadFromStrTable = FALSE;
/*----------------------------------------------------------------------------
    ReadAllEventConfigData()
    Called from ServerConfigData.cpp

    Written by: Jim Hill                       
 ----------------------------------------------------------------------------*/
BOOL CServerConfigData::ReadAllEventConfigData() 
{
    AVIS_EVENT_RULES* lpTable = NULL;
    int i = 0;

    try
    {
        // READ FOR GENERAL EVENTS  
        lpTable = g_AvisEventRulesGeneralAttention;
        for( i = 0; lpTable->lpszIcePackToken != NULL ; i++, lpTable++ )
        {
            ReadEventConfigData( this, lpTable );
        }
        
        // READ FOR SAMPLE EVENTS  
        lpTable = g_AvisEventRulesSampleIntervention;
        for( i = 0; lpTable->lpszIcePackToken != NULL ; i++, lpTable++ )
        {
            ReadEventConfigData( this, lpTable );
        }

        // ONLY NEED TO LOAD THE STRINGS ONCE FROM THE STRING TABLE
        bStringsReadFromStrTable = TRUE;
    }
    catch(...)
    {
    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
    WriteAllEventConfigData()
    Called from ServerConfigData.cpp

    Written by: Jim Hill                       
 ----------------------------------------------------------------------------*/
BOOL CServerConfigData::WriteAllEventConfigData()  
{
    AVIS_EVENT_RULES* lpTable = NULL;
    int i = 0;

    try
    {
        // ARE WE SENDING A TEST ALERT?
        SendTestEvent();    // this

        // WRITE FOR GENERAL EVENTS  
        lpTable = g_AvisEventRulesGeneralAttention;
        for( i = 0; lpTable->lpszIcePackToken != NULL ; i++, lpTable++ )
        {
            WriteEventConfigData( this, lpTable );
        }
        
        // WRITE SAMPLE EVENTS  
        lpTable = g_AvisEventRulesSampleIntervention;
        for( i = 0; lpTable->lpszIcePackToken != NULL ; i++, lpTable++ )
        {
            WriteEventConfigData( this, lpTable );
        }

    }
    catch(...)
    {
    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
    WriteEventConfigData()

    Written by: Jim Hill                       
 ----------------------------------------------------------------------------*/
BOOL WriteEventConfigData( CServerConfigData* pSConfigData, AVIS_EVENT_RULES* lpTableEntry )
{
    HRESULT  hr = 0;
    CString  sValue;
	USES_CONVERSION;
//    CString  s;
    BSTR  bstrKey = SysAllocString( T2COLE(REGKEY_QSERVER));
	BSTR  bstrValue;
    VARIANT  v;
    LPCTSTR  lpszRegVal = NULL;

    // VALIDATE
    if( lpTableEntry == NULL )
        return(FALSE);

    try
    {
        if( lpTableEntry->lpszIcePackToken == NULL || (lpszRegVal = lpTableEntry->lpszRegValue) == NULL )
            return(FALSE);

        // CAN THE USER CONFIGURE THIS ONE? IF NOT, DON'T BOTHER TO WRITE IT
        if( !lpTableEntry->dwIsConfigurable )
            return(FALSE);

        // HAS THIS ONE BEEN CHANGED? IF NOT, DON'T BOTHER TO WRITE IT
        if( !lpTableEntry->bIsDataChanged )
            return(FALSE);

        // CONSTRUCT THE DATA. ENTRIES STORED as Enabled, Duration Time i.e. 0,15
        sValue.Format( _T("%d,%d"), lpTableEntry->dwEnabled, lpTableEntry->dwMinTimeInStateMinutes );  

        // WRITE IT OUT
        VariantInit( &v );
        v.vt = VT_BSTR;
        v.bstrVal = sValue.AllocSysString();	// tam 5-25-00 bstr update
        bstrValue = SysAllocString(T2COLE(lpszRegVal));
        hr = pSConfigData->SetValue( bstrValue, &v, bstrKey );// tam 5-25-00 bstr change
		VariantClear (&v);

    }
    catch(...)
    {
    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
    SendTestEvent()

    Send a test event to the AMS server to validate the system.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CServerConfigData::SendTestEvent() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT  hr = 0;
    CString  sValue;
	USES_CONVERSION;

    BSTR  bstrS = SysAllocString (T2COLE(REGVALUE_SEND_TEST_EVENT));
    BSTR  bstrKey = SysAllocString (T2COLE(REGKEY_QSERVER));
    VARIANT  v;
    
    try
    {
            // ARE WE SET TO SEND IT?
            if( !m_bSendTestEvent )
                return;
            // CLEAR IT
            m_bSendTestEvent = FALSE;

            VariantInit( &v );
            sValue = _T("1");
            v.vt = VT_BSTR;
            v.bstrVal = sValue.AllocSysString();// tam 5-25-00 bstr change
//            s  = REGVALUE_SEND_TEST_EVENT;
            hr = SetValue( bstrS, &v, bstrKey );
			VariantClear (&v);
			
			SysFreeString(bstrS);
			SysFreeString(bstrKey);
    }
    catch(...) 
    {
    }
    return;
}



/*----------------------------------------------------------------------------
    ReadEventConfigData()
    Read in single event config data from Registry.
    Written by: Jim Hill                        
 ----------------------------------------------------------------------------*/
BOOL ReadEventConfigData( CServerConfigData* pSConfigData, AVIS_EVENT_RULES* lpTableEntry )
{
    CString  sValue = _T("");
    TCHAR    szBuff[512];
    TCHAR*   lpszToken=NULL;
    TCHAR*   endptr=NULL;
    LPCTSTR  lpszRegVal = NULL;
    DWORD	 dwEnabled  = 0;           
    DWORD    dwMinTimeInStateMinutes = 0;
	USES_CONVERSION;

    HRESULT  hr = 0;
    BSTR bstrS;
    BSTR  bstrKey = SysAllocString(T2COLE(REGKEY_QSERVER));
    VARIANT  v;

    // VALIDATE
    if( lpTableEntry == NULL )
        return(FALSE);
    // INITIALIZE
    ZeroMemory( szBuff, sizeof(szBuff) );

    try
    {
        if( lpTableEntry->lpszIcePackToken == NULL || (lpszRegVal = lpTableEntry->lpszRegValue) == NULL )
		{
			SysFreeString(bstrKey);
            return(FALSE);
		}

        // LOAD THE STRINGS?
        if( !bStringsReadFromStrTable )
        {
            lpTableEntry->lpszEventName = StrTableLookUpMiscString( lpTableEntry->dwEventNameID );
            lpTableEntry->lpszDefaultMessage = StrTableLookUpMiscString( lpTableEntry->dwDefaultMessageID );
        }


        // CAN THE USER CONFIGURE THIS ONE? IF NOT, DON'T BOTHER TO READ IT
        if( !lpTableEntry->dwIsConfigurable )  // dwEventId != GENERAL_ATTENTION_SEND_TEST_EVENT
		{
			SysFreeString(bstrKey);
            return(FALSE);
		}

        // ENTRIES STORED as Enabled, Duration Time i.e. 0,15
        VariantInit( &v );
        bstrS = SysAllocString(T2COLE(lpszRegVal));
        hr = pSConfigData->GetValue( bstrS, &v, bstrKey );
		SysFreeString(bstrS);// tam 5-25-00 bstr change
		SysFreeString(bstrKey);
        if( SUCCEEDED( hr ) && v.vt != VT_EMPTY )   // GetRegistryValue( REGKEY_QSERVER, lpszRegVal , sValue )   
        {
            sValue = v.bstrVal;
            VariantClear( &v );

            // DO WE HAVE A SETTING? IF NOT, USE THE DEFAULT
            if( sValue.IsEmpty() || hr == 2 )
            {
                // sValue.Format(_T("%d,%d"), lpTableEntry->dwEnabled, lpTableEntry->dwMinTimeInStateSecs );
                // SetRegistryValue( REGKEY_QSERVER, lpszRegVal, sValue );
                lpTableEntry->dwMinTimeInStateSecs = lpTableEntry->dwMinTimeInStateMinutes * 60;  // THE STATIC DEFAULT TABLE IS IN MINUTES
                return(FALSE);
            }

            if( !GetNextNumbersFromCommaString( sValue, &dwEnabled, &dwMinTimeInStateMinutes ) )
                return(FALSE);

            // UPDATE THE EVENT FULES
            lpTableEntry->dwEnabled               = dwEnabled;
            lpTableEntry->dwMinTimeInStateMinutes = dwMinTimeInStateMinutes;
            lpTableEntry->dwMinTimeInStateSecs    = (dwMinTimeInStateMinutes * 60);

            return(TRUE);
        }

    }
    catch(...)
    {
    }
    return(FALSE);
}


/*----------------------------------------------------------------------------
    GetNextNumbersFromCommaString()

    Given a string of comma delimited numbers, parse next 2, and
    return the values in the passed parameters.

    Return the remainder of the string in passed CString

    Destroys content of passed string.

    Written by: Jim Hill                      TCHAR* pszString
 ----------------------------------------------------------------------------*/
BOOL GetNextNumbersFromCommaString( CString& s, DWORD* pNum1, DWORD* pNum2 )
{
    TCHAR    szBuff[512];
    TCHAR*   lpszToken=NULL;
    TCHAR*   endptr=NULL;
    
    // INITIALIZE
    ZeroMemory( szBuff, sizeof(szBuff) );
    if( pNum1 != NULL )
        *pNum1 = 0;
    if( pNum2 != NULL )
        *pNum2 = 0;

    if( s.IsEmpty() )
        return(FALSE);

    try
    {
        s.Remove(' ');         // Trash the spaces
        vpstrncpy( szBuff, (LPCTSTR) s , sizeof(szBuff));
        lpszToken = _tcstok( szBuff, _T(",") );
        if( lpszToken == NULL )
            return(FALSE);
        // WALK PAST ANY EXTRA COMMAS
        for( ; *lpszToken == ','; lpszToken++ )
        {
            if( *lpszToken == 0 )   // BAIL
                return(FALSE);
        }
        if( pNum1 != NULL )
            *pNum1 = STRTOUL( lpszToken, &endptr, 10 );

        lpszToken = _tcstok( NULL, _T(",") );
        if( lpszToken == NULL )
            return(FALSE);
        // WALK PAST ANY EXTRA COMMAS
        for( ; *lpszToken == ','; lpszToken++ )
        {
            if( *lpszToken == 0 )   // BAIL
                return(FALSE);
        }
        if( pNum2 != NULL )
            *pNum2 = STRTOUL( lpszToken, &endptr, 10 );

        // FIND THE END OF THE CURRENT PAIR
        s.Empty();
        DWORD  dwNullByte = 0;
        for( int i = 0; i < 512 && dwNullByte < 2 ; i++ )
        {
             if( szBuff[i] == 0 )
                 dwNullByte++;
        }
        if( dwNullByte == 2 )
        {
            //s = &szBuff[i];
            lpszToken = &szBuff[i];
            // WALK PAST ANY COMMAS ON THE END
            for( ; *lpszToken == ','; lpszToken++ )
            {
                if( *lpszToken == 0 )   // BAIL
                    break;
            }
            s = lpszToken;
            return(TRUE);
        }

    }
    catch(...)
    {
    }
    return(FALSE);
}



/*----------------------------------------------------------------------------
    GetEventNameAndDefaultMsg()
    Returns 0 on fail. Return the EventID on success,
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD GetEventNameAndDefaultMsg( CString& sToken, CString& sText, CString& sEventName, CString& sNote, DWORD dwEventRulesTableID )
{
    CAlertEvent stNewEvent;
    AVIS_EVENT_RULES*  lpTable = NULL;
    AVIS_EVENT_RULES*  lpTableEventRule = NULL;
    LPCTSTR            lpszLookupToken  = NULL;
    DWORD              dwEventID = 0;

    try
    {
        sText = _T("");
        if( sToken.IsEmpty() )
            return(0);
        
        // WHICH TABLE?
        if( dwEventRulesTableID == EVENT_TOKEN_GENERAL_ATTENTION_TABLE )
        {
            lpTable = &g_AvisEventRulesGeneralAttention[0];
        }
        else if( dwEventRulesTableID == EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE )
        {
            lpTable = &g_AvisEventRulesSampleIntervention[0];
        }
        
        if( lpTable != NULL )
        {
            lpszLookupToken = sToken.GetBuffer(0);
            lpTableEventRule = LookupEventRule( lpTable, lpszLookupToken );   
            // if( lpTableEventRule == NULL && lpTableEventRule->lpszIcePackToken )  // AND DON'T USE THE DEFAULT
            if( lpTableEventRule == NULL || lpTableEventRule->lpszIcePackToken == NULL )  // AND DON'T USE THE DEFAULT
                return(0);
        
            if( lpTableEventRule->lpszEventName == NULL ) 
            {
                lpTableEventRule->lpszEventName      = StrTableLookUpMiscString( lpTableEventRule->dwEventNameID );
                lpTableEventRule->lpszDefaultMessage = StrTableLookUpMiscString( lpTableEventRule->dwDefaultMessageID );
            }

            if( lpTableEventRule->lpszEventName != NULL )
                sEventName = lpTableEventRule->lpszEventName;
        
            if( lpTableEventRule->lpszDefaultMessage != NULL )
                sText = lpTableEventRule->lpszDefaultMessage;
        
            dwEventID = lpTableEventRule->dwEventId;

            if( lpTableEventRule->dwNoteTextID != 0 )
                StrTableLoadString( lpTableEventRule->dwNoteTextID, sNote );

            sToken.ReleaseBuffer(-1);
        }
    }
    catch(...) 
    {
    }
    return(dwEventID);
}


/*----------------------------------------------------------------------------
    LookupEventRule
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
AVIS_EVENT_RULES* LookupEventRule( AVIS_EVENT_RULES* lpTable, LPCTSTR lpszToken )
{
    AVIS_EVENT_RULES*  lpTableEventRule = NULL;
    TCHAR*             lpszTableToken = NULL;

    for( int i = 0; (lpszTableToken = lpTable->lpszIcePackToken) != NULL; i++ ) 
    {
        if( _tcscmp( lpszToken, lpszTableToken ) == 0 )  
        {
            lpTableEventRule = lpTable;
            break;
        }
        lpTable++;
    }

    // IF WE COULDN'T FIND IT USE THE DEFAULT
    if( lpszTableToken == NULL )
    {
        // fPrintString( "Unknown event recieved: %s", lpszToken); 
        lpTableEventRule = lpTable;
    }

    return( lpTableEventRule );
}

/*----------------------------------------------------------------------------
    GetStatusToken
    Looks up the text for X-Sample-Status 
 ----------------------------------------------------------------------------*/
BOOL GetStatusToken(CString& sStatusToken, CString& sStatusID )
{
    DWORD dwStatus = 0;
    TCHAR*  endptr=NULL;
    
    dwStatus = STRTOUL( (LPCTSTR) sStatusID, &endptr, 10 );

    switch( dwStatus )
        {
        case STATUS_QUARANTINED:
            sStatusToken = TOKEN_QUARANTINED;       // _T("quarantined");
            break;

        case STATUS_SUBMITTED:
            sStatusToken = TOKEN_SUBMITTED;         // _T("submitted");
            break;

        case STATUS_HELD:
            sStatusToken = TOKEN_HELD;              // _T("held");
            break;

        case STATUS_RELEASED:
            sStatusToken = TOKEN_RELEASED;          // _T("released");
            break;

        case STATUS_UNNEEDED:
            sStatusToken = TOKEN_UNNEEDED;          // _T("unneeded");
            break;

        case STATUS_NEEDED:
            sStatusToken = TOKEN_NEEDED;            // _T("needed");
            break;

        case STATUS_AVAILIABLE:
            sStatusToken = TOKEN_AVAILABLE;         // _T("available");
            break;

        case STATUS_DISTRIBUTE:                  
            sStatusToken = TOKEN_DISTRIBUTE;        // _T("distribute");   
            break;

        case STATUS_DISTRIBUTED:
            sStatusToken = TOKEN_DISTRIBUTED;       // _T("distributed");  
            break;

        case STATUS_INSTALLED:                 
            sStatusToken = TOKEN_INSTALLED;         // _T("installed");          
            break;

        case STATUS_ATTENTION:                 
            sStatusToken = TOKEN_ATTENTION;         // _T("attention");            
            break;

        case STATUS_ERROR:                     
            sStatusToken = TOKEN_ERROR;             // _T("error");                    
            break;

        case STATUS_NOTINSTALLED:               
            sStatusToken = TOKEN_NOTINSTALLED;      // _T("notinstalled"); 
            break;

        case STATUS_RESTART:                     
            sStatusToken = TOKEN_RESTART;           // _T("restart");      
            break;

        case STATUS_LEGACY:                      
            sStatusToken = TOKEN_LEGACY;            // _T("legacy");

        default:
            sStatusToken = TOKEN_UNKNOWN;           // _T("unknown");
            return(FALSE);
        }

    return(TRUE);
}


/*----------------------------------------------------------------------------
    FindStringInCommaDelimitedList()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
int FindStringInCommaDelimitedList( CString& sStringList, LPCTSTR pszSubString )
{
    int  index =  0;
    CString sList         = _T("");
    CString sSearchString = _T("");
    CString s             = _T("");
    CString sSubString    = _T("");

    // VALIDATE
    if( pszSubString == NULL )
        return( -1 );

    // INITIALIZE
    s = sStringList;
    s.TrimLeft();
    s.TrimRight();
    sSubString = pszSubString;
    sSubString.TrimLeft();
    sSubString.TrimRight();

    if( s.IsEmpty() || sSubString.IsEmpty() )
        return( -1 );

    // FORMAT
    sList.Format( _T(",%s,"), s  );
    sSearchString.Format( _T(",%s,"), sSubString );

    // SEARCH
    index = sList.Find( sSearchString );

    // MAKE THE INDEX 0 BASED AGAIN
    // if( index > 0 )
    //     --index;

    return( index );
}

/*----------------------------------------------------------------------------
    IsLocalQuarantineSampleAlert()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL IsLocalQuarantineSampleAlert( CString& sSampleStatus, int* piStatusID, CString& sResultToken, CString& sAnalysisState )
{
    CString s         = _T("");
    CString sTemp     = QUARANTINE_ALERT_STATUS_TOKENS;
    CString sTemp1    = QUARANTINE_ALERT_RESULT_TOKENS;
    CString sTemp2    = QUARANTINE_SAMPLE_STATE_TOKENS;
    CString sStatusID = _T("");


    try
    {
        // VALIDATE
        if( sSampleStatus.IsEmpty() && (piStatusID == NULL || *piStatusID == -1) && sResultToken.IsEmpty() )
            return(FALSE);

        if( piStatusID != NULL && *piStatusID >= 0 )
        {
            sStatusID.Format( _T("%d"), *piStatusID );
            if( GetStatusToken( s, sStatusID ) )
                sSampleStatus = s;
        }

        if( sSampleStatus.IsEmpty() )
            return(FALSE);

        if( FindStringInCommaDelimitedList( sTemp, sSampleStatus ) >= 0 )        // is it in the list
        {
            if( (FindStringInCommaDelimitedList( sTemp1, sResultToken ) == -1 ) &&  // condition != "repaired"  &&
				!(FindStringInCommaDelimitedList( sTemp2, sAnalysisState ) >= 0 &&         // !((State == "infected) && (Alert Status == uneeded))
					FindStringInCommaDelimitedList( sTemp, sSampleStatus) >= 0	))      
            {
                return(TRUE);
            }
        }
    }
    catch(...) 
    {
    }
    return(FALSE);
}



/*----------------------------------------------------------------------------
    IsSampleTimeInStateAlert()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL IsSampleTimeInStateAlert( CString& sSampleStatus, CString& sLastAlertStatus )
{
    CString            sTemp;

    try
    {
        if( sSampleStatus.IsEmpty() ) 
            return(FALSE);

        // CHECK FOR TIME IN STATE ERRORS. INCLUDES ONLY ONES WITH TIMERS
        sTemp = SAMPLE_ALERT_STATUS_TIMER_TOKEN_LIST;   
        if( FindStringInCommaDelimitedList( sTemp, sSampleStatus ) >= 0 )        // is it in the list
        {
            if( sSampleStatus == sLastAlertStatus )
                return(TRUE);
        }
    }
    catch(...) 
    {
    }
    return(FALSE);
}















