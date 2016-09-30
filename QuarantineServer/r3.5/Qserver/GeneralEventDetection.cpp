/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// GeneralEventDetection.cpp





#include "stdafx.h"
#include "AvisEventDetection.h"   // in QuarantineServer\Include 
#include "EventObject.h"
#include <time.h>
#include "vpstrutils.h"


// LINK IN THE EVENT RULES DATA IN THIS FILE ONLY
#include "AvisEventRulesData.h"


// ENABLE/DISABLE AMS
#define USE_AMS


// LOCAL
// BOOL FillGeneralAlertMsgData( CAlertEvent * lpstNewEvent );
BOOL LookupHostnameAndAddress( CString& sHostName, CString& sHostAddress );
//TCHAR *LookUpIcePackTokenString(TCHAR *lpszToken, DWORD dwTableID);
BOOL ClearGeneralAttentionTimer( CString sTokens );
AVIS_EVENT_RULES* LookupEventRule( AVIS_EVENT_RULES* lpTable, LPCTSTR lpszToken );




extern void SendQSGeneralAMSEvent(CEvent  *pData , CString& sTitle);
extern void SendQSSampleAMSEvent(CEvent  *pData , CString& sTitle);



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEventDetect::CEventDetect()
{
    m_pServiceModule = NULL;
}
CEventDetect::~CEventDetect()
{
}





/*----------------------------------------------------------------------------
    DetectGeneralErrors()
    Written by: Jim Hill              download,defcast,disk,query,submit,target
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::DetectGeneralErrors(CObList *pEventList, DWORD dwModuleData)
{
    CAlertEvent  stNewEvent;
    HRESULT hr = S_OK;
    TCHAR   szBuff[512];
    TCHAR*  lpszToken=NULL;
    TCHAR*  lpszParentToken=NULL;
    CString sCurrentValue     = _T("");
    CString sPreviousValue    = _T("");
    CString sNewPreviousValue = _T("");
    CString sTemp             = _T("");
    //DWORD   dwEnableGeneralAttention = 0;
    BOOL    bIsGatewayAlert   = FALSE;
    TCHAR*  endptr=NULL;
    time_t  tLastTime = 0;
    time_t  tCurrentTime = 0;
    time_t  tElapsedTime = 0;  

    try
    {
        // SAVE THE MODULE PTR
        m_pServiceModule = (CServiceModule*) dwModuleData;
        //fWidePrintString( "NOTE: CEventDetect  ServiceModule ptr= %x", dwModuleData );

        // ARE WE ENABLED FOR ALERTING?
        GetRegistryValue( REGKEY_QSERVER, _T("TestEnableAlertEngine"), sTemp );
        DWORD dwIsEnable;
        if( sTemp.IsEmpty() )
            dwIsEnable = 1;     // ENABLE AS THE DEFAULT
        else
            dwIsEnable = STRTOUL( (LPCTSTR) sTemp, &endptr, 10 );
        if( dwIsEnable == 0 )
            return(FALSE);                     // NOT ENABLED

        // READ IN CONFIG DATA FOR EACH EVENT
        ReadAllConfigDataFromRegistry();
        
        // INITIALIZE
        ZeroMemory( szBuff, sizeof(szBuff) );

        // CHECK DISK SPACE
        CheckDiskSpace( pEventList );

        // CHECK FOR TEST EVENT
        SendTestEvent( pEventList );

        // DETECT ARRIVAL OF NEW DEFINITIONS    
        DetectNewDefinitionArrival( pEventList );
    
        // IS IT TIME TO CHECK AGAIN?  GET TIME OF LAST ATTENTION ALERT
        if( GetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_ICEPACK_DATE_LAST, sTemp )) 
        {
            sTemp.TrimLeft();
            tLastTime = STRTOUL( (LPCTSTR) sTemp, &endptr, 10 );
            time( &tCurrentTime );    // GET TIME IN SECS SINCE 1979 IN UTC
            tElapsedTime = tCurrentTime - tLastTime;
            
        }
    
        // GET CURRENT AND PREVIOUS ATTENTION TOKENS
        if( !GetRegistryValue( REGKEY_QSERVER_AVIS, REGVALUE_ATTENTION, sCurrentValue )) // REGKEY_QSERVER  4/19/00 changed to use Avis key instead jhill
            return(FALSE);
        if( !GetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_ICEPACK_ATTENTION_LAST, sPreviousValue )) 
            return(FALSE);
        sCurrentValue.Remove(' ');
        sCurrentValue.Remove('\t');
        sPreviousValue.Remove(' ');
        sPreviousValue.Remove('\t');
    
    
        // IF THEY'RE THE SAME OR NO CURRENT ERRORS, THEN WE'RE DONE
        if( sCurrentValue == sPreviousValue || sCurrentValue.IsEmpty() )
        {
            // MAKE SURE WE CLEAR THE LAST SAVE
            // if( sCurrentValue.IsEmpty() )
            SetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_ICEPACK_ATTENTION_LAST, sCurrentValue ); 

            // MAKE SURE THE TIMER IS CLEARED IF NEEDED
            // This detects the case where the timer is running, and all the 
            // General Attention GatewayComm conditions are cleared. This then stops the timer.
            ClearGeneralAttentionTimer( sCurrentValue );

            return(TRUE);
        }
    
        // OTHERWISE PARSE THE TOKENS
        sNewPreviousValue = sCurrentValue;          // 1/21/00
        vpstrncpy( szBuff, (LPCTSTR) sCurrentValue, sizeof(szBuff) );
        lpszToken = _tcstok( szBuff, _T(",") );
        for( int i=0; lpszToken != NULL; i++ )
        {
            lpszParentToken = NULL;

            // DID WE HAVE TWO COMMAS TOGETHER 1/22/00
            if( *lpszToken == ',' )
                continue;

            // IS THIS TOKEN IN THE PREVIOUS LIST? IF NOT, THEN IT'S NEW.
            if( sPreviousValue.Find( lpszToken ) == -1 )
            {
                // IS THIS A GATEWAY COMM EVENT?
                sTemp = GENERAL_ATTENTION_GATEWAY_QUERY_TOKEN_LIST;
                if( sTemp.Find( lpszToken ) >= 0 )
                {
                    lpszParentToken = TOKEN_GATEWAYCOMM;  // _T("gatewayComm") 
                    bIsGatewayAlert = TRUE;
                }
                // LOOKUP EVENT CONFIG AND INITIALIZE
                InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, lpszToken, lpszParentToken );   // 

                // SHOULD WE REPORT IT?
                if( IsEnabledAndTimeInStateOk( &stNewEvent ) )
                {
                    FillGeneralAlertMsgData( &stNewEvent );
                    AddNewEventToEventQueue( &stNewEvent, pEventList );
                }
                else
                {   
                    // fWidePrintString("NOTE General: DetectGeneralErrors  NO alert on [%s]", lpszToken );
                    // NOT YET
                    RemoveTokenFromGeneralAttentionString( &stNewEvent, sNewPreviousValue ); 
                }

            }
            lpszToken = _tcstok( NULL, _T(",") );
        }

        // NOW CLEAR TIME IN STATE, FOR TOKENS THAT HAVE BEEN CLEARED BY ICEPACK
        lpszParentToken = NULL;
        vpstrncpy( szBuff, (LPCTSTR) sPreviousValue, sizeof(szBuff) );
        lpszToken = _tcstok( szBuff, _T(",") );
        for( i=0; lpszToken != NULL; i++ )
        {
            // DID WE HAVE TWO COMMAS TOGETHER 1/22/00
            if( *lpszToken == ',' )
                continue;
            // IS THIS TOKEN IN THE CURRENT LIST? IF NOT, THEN IT'S BEEN REMOVED, SO WE CLEAR THE TIMER.
            if( sNewPreviousValue.Find( lpszToken ) == -1 )
            {
                // IS THIS A GATEWAY COMM EVENT?
                sTemp = GENERAL_ATTENTION_GATEWAY_QUERY_TOKEN_LIST;
                if( sTemp.Find( lpszToken ) >= 0 )
                {
                    lpszParentToken = TOKEN_GATEWAYCOMM;   // _T("gatewayComm"); 
                    //bIsGatewayAlert = TRUE;
                }
                // LOOKUP EVENT CONFIG AND INITIALIZE
                InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, lpszToken, lpszParentToken );   // 
                // CLEAR THE TIMER
                ClearTimeInState( &stNewEvent );
            }
            lpszToken = _tcstok( NULL, _T(",") );
        }
    

        // SAVE THE TIME
        if( i > 0 )
        {
            tCurrentTime = 0;
            time( &tCurrentTime );         // GET TIME IN SECS SINCE 1970 IN UTC
            CString s;
            s.Format(_T("%ld"), tCurrentTime );
            SetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_ICEPACK_DATE_LAST, s ); 
        }
        // SAVE CURRENT TO LAST
        SetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_ICEPACK_ATTENTION_LAST, sNewPreviousValue ); 
    }
    catch(...) 
    {
        //fWidePrintString("FAILURE: CEventDetect::DetectGeneralErrors Exception.");
    }

    return(TRUE);
}


/*----------------------------------------------------------------------------
    DetectNewUnblessedDefArrival()

    DETECT ARRIVAL OF NEW UNBLESSED DEFS    
    Written by: Jim Hill                                     
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::DetectNewUnblessedDefArrival(CObList *pEventList)
{
    CAlertEvent  stNewEvent;
    BOOL    bDefinitionIsActiveBlessed = 0;
    TCHAR*  endptr=NULL;
    CString sUnbessedSeqNum     = _T("");
    CString sUnbessedSeqNumLast = _T("");
    CString s                   = _T("");

    try
    {
        // READ
        if( !GetRegistryValue( REGKEY_QSERVER_AVIS_CURRENT, REGVALUE_DEFS_IS_ACTIVE_BLESSED, s )) 
            return(FALSE);
        bDefinitionIsActiveBlessed = ( STRTOUL( (LPCTSTR)s, &endptr, 10 ) != 0 );
        // IF THE ACTIVE ONES ARE BLESSED, THEN WE CAN'T DETECT UNBLESSED
        if( bDefinitionIsActiveBlessed )
            return(FALSE);

        if( !GetRegistryValue( REGKEY_QSERVER_AVIS_CURRENT, REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM, sUnbessedSeqNum )) 
            return(FALSE);
        if( !GetRegistryValue( REGKEY_QSERVER, REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM_LAST, sUnbessedSeqNumLast )) 
            return(FALSE);

        // VALIDATE
        if( sUnbessedSeqNum.IsEmpty() )
            return(FALSE);
        if( sUnbessedSeqNumLast.IsEmpty() )
        {
            // SAVE CURRENT TO LAST
            SetRegistryValue( REGKEY_QSERVER, REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM_LAST, sUnbessedSeqNum ); 
            // DON'T ALERT WHEN FIRST INSTALLED
            return(FALSE);
        }

        // ARE THERE NEW UN-BLESSED DEFS?
        if( sUnbessedSeqNum == sUnbessedSeqNumLast )
            return(FALSE);

        // INITIALIZE THE EVENT DATA
        InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_NEWUNBLESSEDDEF, NULL );    // _T("newUnblessedDef")
        // SHOULD WE REPORT IT?
        if( IsEnabledAndTimeInStateOk( &stNewEvent ) )
        {
            FillGeneralAlertMsgData( &stNewEvent );
            AddNewEventToEventQueue( &stNewEvent, pEventList );
            // SAVE CURRENT TO LAST
            SetRegistryValue( REGKEY_QSERVER, REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM_LAST, sUnbessedSeqNum ); 
        }
        return(TRUE);
    }
    catch(...) 
    {
        //fWidePrintString("FAILURE: CEventDetect::DetectNewDefinitionArrival Exception.");
    }
    return(FALSE);
}


/*----------------------------------------------------------------------------
    DETECT ARRIVAL OF NEW DEFINITIONS    
    Written by: Jim Hill                                     
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::DetectNewDefinitionArrival(CObList *pEventList)
{
    CAlertEvent  stNewEvent;

    CString sBessedSeqNum     = _T("");
    CString sBessedSeqNumLast = _T("");

    // DETECT ARRIVAL OF NEW UNBLESSED DEFS    
    DetectNewUnblessedDefArrival( pEventList );

    // DETECT ARRIVAL OF NEW BLESSED DEFS    
    try
    {
        // READ
        if( !GetRegistryValue( REGKEY_QSERVER_AVIS_CURRENT, REGVALUE_DEFS_BLESSED_SEQUENCE_NUM, sBessedSeqNum )) 
            return(FALSE);
        if( !GetRegistryValue( REGKEY_QSERVER, REGVALUE_DEFS_BLESSED_SEQUENCE_NUM_LAST, sBessedSeqNumLast )) 
            return(FALSE);

        // VALIDATE
        if( sBessedSeqNum.IsEmpty() )
            return(FALSE);
        if( sBessedSeqNumLast.IsEmpty() )
        {
            // SAVE CURRENT TO LAST
            SetRegistryValue( REGKEY_QSERVER, REGVALUE_DEFS_BLESSED_SEQUENCE_NUM_LAST, sBessedSeqNum ); 
            // DON'T ALERT WHEN FIRST INSTALLED
            return(FALSE);
        }

        // ARE THERE NEW BLESSED DEFS?
        if( sBessedSeqNum == sBessedSeqNumLast )
            return(FALSE);

        // INITIALIZE THE EVENT DATA
        InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_NEWBLESSEDDEF, NULL );     // _T("newBlessedDef")
        // SHOULD WE REPORT IT?
        if( IsEnabledAndTimeInStateOk( &stNewEvent ) )
        {
            FillGeneralAlertMsgData( &stNewEvent );
            AddNewEventToEventQueue( &stNewEvent, pEventList );
            // SAVE CURRENT TO LAST
            SetRegistryValue( REGKEY_QSERVER, REGVALUE_DEFS_BLESSED_SEQUENCE_NUM_LAST, sBessedSeqNum ); 
        }
        return(TRUE);
    }
    catch(...) 
    {
        //fWidePrintString("FAILURE: CEventDetect::DetectNewDefinitionArrival Exception.");
    }

    return(FALSE);
}



/*----------------------------------------------------------------------------
    IsEnabledAndTimeInStateOk()

    Are we enabled for this event? 
    And has it been in state long enough?

    Written by: Jim Hill                                     
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::IsEnabledAndTimeInStateOk( CAlertEvent * lpstNewEvent )
{
    AVIS_EVENT_RULES* lpRule = NULL;
    time_t            tCurrentGmtTime      = 0;

    if( (lpRule = lpstNewEvent->lpEventRuleEntry) == NULL )
        return(FALSE);

    // IS IT ENABLED?
    if( !lpRule->dwEnabled )
        return(FALSE);

    // DOES THIS ONE HAVE A MINIMUM TIME IN STATE? 0 MEANS REPORT IMMEDIATELY
    if( lpRule->dwMinTimeInStateSecs == 0 )
        return(TRUE);

    // GMT CURRENT TIME IN SECS SINCE 1970 IN UTC
    tCurrentGmtTime = TimeToGmtTime( 0 );  

    if( lpstNewEvent->dwEventRulesTableID == EVENT_TOKEN_GENERAL_ATTENTION_TABLE )
    {
        // IS THE TIMER RUNNING?
        if( lpRule->dwInStateTimerExpire == 0 )   
        {
            // NOPE, SO START IT 
            lpRule->dwInStateTimerExpire = tCurrentGmtTime + lpRule->dwMinTimeInStateSecs;
#ifdef _DEBUG
            fWidePrintString("NOTE General: IsEnabledAndTimeInStateOk  Timer started on [%s] Secs remaining %d", lpstNewEvent->sIcePackTokenString, lpRule->dwMinTimeInStateSecs );
#endif
            return(FALSE);
        }
        
        // HAS IT BEEN LONG ENOUGH?
        if( lpRule->dwInStateTimerExpire < (DWORD) tCurrentGmtTime )
        {
            // RESET THE TIMER
            lpRule->dwInStateTimerExpire = 0;
#ifdef _DEBUG
            fWidePrintString("NOTE General: IsEnabledAndTimeInStateOk  Timer stopped on [%s]", lpstNewEvent->sIcePackTokenString );
#endif
            return(TRUE);
        }
        else
        {
// #ifdef  MY_SYSTEM
//             fWidePrintString("NOTE General: IsEnabledAndTimeInStateOk  Timer still running on [%s] Secs remaining %d", lpstNewEvent->sIcePackTokenString, ( lpRule->dwInStateTimerExpire - (DWORD) tCurrentGmtTime ) );
// #endif
            return(FALSE);
        }
    }
    else if( lpstNewEvent->dwEventRulesTableID == EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE )
    {
        return(TRUE);
    }

    return(FALSE);
}


/*----------------------------------------------------------------------------
    ClearGeneralAttentionTimer()
        
    Written by: Jim Hill                                TCHAR * lpszToken     
 ----------------------------------------------------------------------------*/
BOOL ClearGeneralAttentionTimer( CString sTokens )
{
    AVIS_EVENT_RULES*  lpTableEventRule = NULL;
    //CAlertEvent stNewEvent;
    CString     sTemp;
    TCHAR *     lpszParentToken = NULL;
    BOOL        bIsValidAlert = FALSE;

    try
    {
        // IS THIS A GATEWAY COMM EVENT?
        // IS ONE OF THE GATEWAY COMM TOKENS IN THE STRING OF TOKENS?
        if( sTokens.Find( TOKEN_DOWNLOAD_GATEWAY ) -1 )
        {
            if( sTokens.Find( TOKEN_QUERY_GATEWAY  ) -1 )
            {
                if( sTokens.Find( TOKEN_SUBMIT_GATEWAY  ) -1 )
                    bIsValidAlert = TRUE;
            }
            else
                bIsValidAlert = TRUE;
        }
        else
            bIsValidAlert = TRUE;


        // IF NONE FOUND, THEN THIS IS NOT A TIMED EVENT
        if( !bIsValidAlert )
            return(FALSE);                      

        // LOOKUP THE RULE
        lpTableEventRule = LookupEventRule( &g_AvisEventRulesGeneralAttention[0], TOKEN_GATEWAYCOMM );
        if( lpTableEventRule == NULL || lpTableEventRule->dwEventId != GENERAL_ATTENTION_GATEWAY_COMM_EVENTID )
            return(FALSE);

        // RESET THE TIMER
#ifdef _DEBUG
        if( lpTableEventRule->dwInStateTimerExpire != 0 )
            fWidePrintString("NOTE General: IsEnabledAndTimeInStateOk  Timer stopped on Gateway Comm problem" );
#endif
        lpTableEventRule->dwInStateTimerExpire = 0;

    }
    catch(...) 
    {
    }
    return(FALSE);
}


/*----------------------------------------------------------------------------
    ClearTimeInState()

    Stop the timer for this Sample entry

    Written by: Jim Hill                                     
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::ClearTimeInState( CAlertEvent * lpstNewEvent )
{
    AVIS_EVENT_RULES* lpRule = NULL;

    if( (lpRule = lpstNewEvent->lpEventRuleEntry) == NULL )
        return(FALSE);

    lpRule->dwInStateTimerExpire = 0;

    return(TRUE);
}


/*----------------------------------------------------------------------------
    BuildAlertMessage

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::BuildAlertMessage( CAlertEvent * lpstNewEvent )
{   
    AVIS_EVENT_RULES* lpEventRuleEntry = NULL;
    TCHAR*  lpszDisplayString = NULL;
    TCHAR*  lpszToken         = NULL;
    BOOL    bCrLf             = FALSE;
    CString sMessageTitle     = _T("");
    CString sMessage          = _T("");
    CString sTemp             = _T("");
    CString sLogFileText      = _T("");
    CString sHeading          = _T("");
    CString s                 = _T("");

    try
    {
        // FIND THE TOKEN STRING
        lpszToken = lpstNewEvent->sIcePackTokenString.GetBuffer(0);
        if( lpstNewEvent->dwIcePackTokenTableID > 0 )
        {
            lpszDisplayString = LookUpIcePackTokenString( lpszToken, lpstNewEvent->dwIcePackTokenTableID );
            if( lpszDisplayString != NULL )
            {
                if( !lpstNewEvent->sErrorText.IsEmpty() )
                {
                    lpstNewEvent->sErrorText += _T("\r\n");
                    lpstNewEvent->sErrorText += lpszDisplayString;
                }
                else
                    lpstNewEvent->sErrorText  = lpszDisplayString;
            }
        }
        lpstNewEvent->sIcePackTokenString.ReleaseBuffer(-1);


        // GET THE GROUP TITLE
        BuildMessageTitle( sMessageTitle, lpstNewEvent );

//// jhill 2/28/00
        // GET THE EVENT NAME
        lpEventRuleEntry = lpstNewEvent->lpEventRuleEntry;
        if( lpEventRuleEntry != NULL && lpEventRuleEntry->lpszEventName != NULL )
        {
            lpstNewEvent->sEventName = lpEventRuleEntry->lpszEventName;

            // DON'T REPEAT THE SAME NAME ON SAMPLE PROCESSING ERRORS
            if( lpEventRuleEntry->dwEventNameID != SAMPLE_ALERT_ERROR_EVENTNAME_ID )
            {
                if( sMessageTitle.IsEmpty() )
                    sMessageTitle  = lpstNewEvent->sEventName;
                else
                    sMessageTitle += _T(" - ") + lpstNewEvent->sEventName;
            }
        }
////

        // START
        //sMessage.Format(_T("[%s] "), sMessageTitle );
        sMessage.Empty();
        sLogFileText.Format(_T("[%s] \r\n"), sMessageTitle );

        // USER
        if( !lpstNewEvent->sUser.IsEmpty() )
        {
            StrTableLoadString( IDSTABLE_HEADING_USER, sHeading );               //  _T("User: ")
            sLogFileText += sHeading + lpstNewEvent->sUser + _T("   ");   
            bCrLf = TRUE;
        }

        // MACHINE NAME
        if( !lpstNewEvent->sMachineName.IsEmpty() )
        {
            if( lpstNewEvent->dwEventRulesTableID == EVENT_TOKEN_GENERAL_ATTENTION_TABLE )
            {
                StrTableLoadString( IDSTABLE_HEADING_QSERVER, sHeading );        //  _T("Quarantine Server: ")
                lpstNewEvent->sMachineName = sHeading + lpstNewEvent->sMachineName;
                sLogFileText += lpstNewEvent->sMachineName ;                
            }
            else
            {
                StrTableLoadString( IDSTABLE_HEADING_COMPUTERNAME, sHeading );   //  _T("ComputerName: ")
                sLogFileText += sHeading + lpstNewEvent->sMachineName ;                
            }
            bCrLf = TRUE;
        }

        if( bCrLf )
        {
            sLogFileText += _T("\r\n");
            bCrLf = FALSE;
        }

        // MACHINE ADDRESS
        if( !lpstNewEvent->sMachineAddress.IsEmpty() )
        {
            StrTableLoadString( IDSTABLE_HEADING_ADDRESS, sHeading );           //  _T("Address: ")
            lpstNewEvent->sMachineAddress.TrimLeft();
            sLogFileText += sHeading + lpstNewEvent->sMachineAddress + _T("   ");
            bCrLf = TRUE;
        }

        // DNS NAME
        if( !lpstNewEvent->sDNSname.IsEmpty() )
        {
            StrTableLoadString( IDSTABLE_HEADING_DNSNAME, sHeading );           //  _T("DNS name: ")
            lpstNewEvent->sDNSname.TrimLeft();
            sLogFileText += sHeading + lpstNewEvent->sDNSname;
            bCrLf = TRUE;
        }

        if( bCrLf )
        {
            sLogFileText += _T("\r\n");
            bCrLf = FALSE;
        }

         // VIRUS NAME
        if( !lpstNewEvent->sVirusName.IsEmpty() )
        {
            StrTableLoadString( IDSTABLE_HEADING_VIRUSNAME, sHeading );         // _T("VirusName: ")
            sLogFileText += sHeading + lpstNewEvent->sVirusName;
            bCrLf = TRUE;
        }

        // DEFINITIONS
        if( !lpstNewEvent->sDefinitions.IsEmpty() )
        {
            StrTableLoadString( IDSTABLE_HEADING_DEFINITIONS, sHeading );       // _T("Definitions: ")
            if( lpstNewEvent->dwEventGroupType == EVENT_GROUP_NEW_DEF_ARRIVAL )
                sMessage += _T("  ") + sHeading + lpstNewEvent->sDefinitions;

            StrTableLoadString( IDSTABLE_STAB_HEADING_DEFINITIONS_NEEDED, sHeading ); // _T("Definitions Needed: ")
            
            sLogFileText += _T("\r\n") + sHeading + lpstNewEvent->sDefinitions;  
        }

        if( bCrLf )
        {
            sLogFileText += _T("\r\n");
            bCrLf = FALSE;
        }

         // FILE NAME
        if( !lpstNewEvent->sFileName.IsEmpty() )
        {
            StrTableLoadString( IDSTABLE_HEADING_FILENAME, sHeading );          // _T("FileName: ")
            sLogFileText += sHeading + lpstNewEvent->sFileName + _T("\r\n");
        }

        // ADD THE STATUS
        if( !lpstNewEvent->sSampleStatus.IsEmpty() )
        {
            StrTableLoadString( IDSTABLE_HEADING_STATUS, sHeading );            //  _T("Status: ")
            sLogFileText += sHeading + lpstNewEvent->sSampleStatus + _T("\r\n");
        }

        // ADD THE SAMPLE STATE
        if( !lpstNewEvent->m_sSampleState.IsEmpty() )
        {
            StrTableLoadString( IDSTABLE_HEADING_SAMPLE_STATE, sHeading );      //  _T("Sample State: ")
            sLogFileText += sHeading + lpstNewEvent->m_sSampleState + _T("\r\n");
        }

        // ADD IN THE ERROR TEXT 
        if( !lpstNewEvent->sErrorText.IsEmpty() )
        {
            sMessage     += _T(" ") + lpstNewEvent->sErrorText;
            sLogFileText += lpstNewEvent->sErrorText + _T("\r\n");
        }

        // ADD IN THE DEFAULT MESSAGE
        if( !lpstNewEvent->sDefaultOrHelpMsg.IsEmpty() )
        {
            sLogFileText += lpstNewEvent->sDefaultOrHelpMsg + _T("\r\n");
        }
        else
        {
            // IS THIS A SAMPLE "TIME IN STATE" ALERT?
            sTemp = SAMPLE_ALERT_STATUS_TIMER_TOKEN_LIST;
            // if( lpszToken != NULL && sTemp.Find( lpszToken ) >= 0 )
            if( FindStringInCommaDelimitedList( sTemp, lpszToken ) >= 0 )  
            {
                //sTemp.Format( _T("\r\nThe Sample has shown the [%s] Status for too long."), lpszToken ); 
                StrTableLoadString( IDSTABLE_SAMPLE_INSTATE_TOO_LONG, s );     
                sTemp.Format( s, lpszToken );
                if( lpstNewEvent->sDefaultOrHelpMsg.IsEmpty() )
                    lpstNewEvent->sDefaultOrHelpMsg = sTemp;
                sLogFileText += sTemp + _T("\r\n");
            }
        }

#if 0
        // IS THIS A GENERAL "TIME IN STATE" ALERT?     
        sTemp = GENERAL_ATTENTION_GATEWAY_QUERY_TOKEN_LIST;
        // if( lpszToken != NULL && sTemp.Find( lpszToken ) >= 0 )
        if( FindStringInCommaDelimitedList( sTemp, lpszToken ) >= 0 )  
        {
            sTemp = lpszToken;
            if( sTemp == TOKEN_GATEWAYCOMM )                                  // "gatewayComm"
            {
                StrTableLoadString( IDSTABLE_GENERAL_INSTATE_TOO_LONG1, s );  // "Quarantine Server has been unable to connect to the Gateway for too long."   
            }                                                                 
            else
            {
                StrTableLoadString( IDSTABLE_GENERAL_INSTATE_TOO_LONG2, s );  // "Quarantine Server has shown this Attention state for too long."   
            }
        }
#endif

        // ADD IN THE NOTE TEXT, IF ANY. THIS IS FOR THE "NOT INSTALLED" CASE. DEF_ALERT_NOT_INSTALLED_EVENTID
        if( lpstNewEvent->lpEventRuleEntry != NULL )
        {
            DWORD dwNoteTextID = lpstNewEvent->lpEventRuleEntry->dwNoteTextID;
            if( dwNoteTextID != 0 )
            {
                StrTableLoadString( dwNoteTextID, lpstNewEvent->m_sNote );
                if( !lpstNewEvent->m_sNote.IsEmpty() )
                {
                    sLogFileText += lpstNewEvent->m_sNote + _T("\r\n");

                }
            }
        }


        // RETURN COMPLETED MESSAGE
        sMessage.TrimLeft();
        sLogFileText.TrimLeft();
        lpstNewEvent->sMessage     = sMessage;
        lpstNewEvent->sLogFileText = sLogFileText;

    }
    catch(...) 
    {
#ifdef _DEBUG
        fWidePrintString("FAILURE: CEventDetect::BuildAlertMessage Exception.");
#endif
    }
    return(TRUE);
}


#define AMS_MAX_LEN_COMBINEED_STRINGS   375  
/*----------------------------------------------------------------------------
    TrimAmsParmsIfTooLong()

    If the combined string length is too long, then trim some of the parms.

    Written by: Jim Hill              AMS_MAX_LEN_COMBINEED_STRINGS   400 
 ----------------------------------------------------------------------------*/
BOOL TrimAmsParmsIfTooLong( CAlertEvent * lpstNewEvent )
{
    DWORD   dwLenAllStrings  = 0;        
    DWORD   dwLenAllStringsOriginal  = 0;        
    // TCHAR   szBuffer[512] = {0};
    CString sTitle;
    CEventDetect  EventDetect;

    lpstNewEvent->sMachineAddress.TrimLeft();
    EventDetect.BuildMessageTitle( sTitle, lpstNewEvent );

    // AMS APPEARS TO HAVE A MAX AROUND 340 - 400 FOR THE COMBINED LENGTH OF ALL THE TEXT STRINGS PASSED IN jhill 2/29/00
    dwLenAllStrings +=  sTitle.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sMachineName.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sMachineAddress.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sUser.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sPlatformName.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sFileName.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sVirusName.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sDefinitions.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sSampleStatus.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sAgeOfSample.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sMessage.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->sDefaultOrHelpMsg.GetLength() + 1;
    dwLenAllStrings +=  lpstNewEvent->m_sNote.GetLength() + 1;
    dwLenAllStrings +=  10;
    dwLenAllStringsOriginal = dwLenAllStrings;

#ifdef _DEBUG
    fWidePrintString( "AddNewEventToEventQueue - Filename= %s  AllCstrings= %d ", lpstNewEvent->sFileName, dwLenAllStrings );
#endif

    // START TRIMMING IF THE COMBINED LENGTH IS TOO LONG
    DWORD dwSubtract = 0;
    if( dwLenAllStrings > AMS_MAX_LEN_COMBINEED_STRINGS )
    {
        // FIRST THE AGE OF SAMPLE
        dwLenAllStrings -= (lpstNewEvent->sAgeOfSample.GetLength() + 1);
        lpstNewEvent->sAgeOfSample.Empty();

        // NEXT, MACHINE ADDRESS
        if( dwLenAllStrings > AMS_MAX_LEN_COMBINEED_STRINGS )
        {
            dwLenAllStrings -= (lpstNewEvent->sMachineAddress.GetLength() + 1);
            lpstNewEvent->sMachineAddress.Empty();

            // NEXT, MACHINE NAME
            if( dwLenAllStrings > AMS_MAX_LEN_COMBINEED_STRINGS )
            {
                dwLenAllStrings -= (lpstNewEvent->sMachineName.GetLength() + 1);
                lpstNewEvent->sMachineName.Empty();

                // NEXT, PLATFORM
                if( dwLenAllStrings > AMS_MAX_LEN_COMBINEED_STRINGS )
                {
                    dwLenAllStrings -= (lpstNewEvent->sPlatformName.GetLength() + 1);
                    lpstNewEvent->sPlatformName.Empty();
                }
            }
        }
    }

    // DID WE TRIM ANY OFF?
#ifdef _DEBUG
    if( dwLenAllStringsOriginal != dwLenAllStrings )
        fWidePrintString( "TRIM: AddNewEventToEventQueue - Filename= %s  OriginalAllCstrings= %d  CurrentAllCstrings= %d ", lpstNewEvent->sFileName, dwLenAllStringsOriginal, dwLenAllStrings );
#endif

    return(TRUE);      
}




/*----------------------------------------------------------------------------
    AddNewEventToEventQueue()
    ADD TO EVENT QUEUE
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::AddNewEventToEventQueue( CAlertEvent * lpstNewEvent, CObList *pEventList )
{
    CString sTemp  = _T("");
    TCHAR*  endptr = NULL;

    try
    {
#ifdef MY_SYSTEM
        MessageBeep(MB_ICONEXCLAMATION);
#endif

        BuildAlertMessage( lpstNewEvent );
        if( !lpstNewEvent->sLogFileText.IsEmpty() )
        {
#ifdef _DEBUG
            fWidePrintString("ALERT: %s\r\n",(LPCTSTR) lpstNewEvent->sLogFileText);
#endif
            // WRITE TO NT EVENT LOG
            LogEventToNTEventLog( lpstNewEvent );
        }

        // SAVE THE LAST ATTENTION ALERT?
        if( lpstNewEvent->dwEventRulesTableID == EVENT_TOKEN_GENERAL_ATTENTION_TABLE )
        {
            SetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_ICEPACK_ATTENTION_LAST_ALERT, lpstNewEvent->sIcePackTokenString ); 
            time_t  tCurrentTime = 0;
            time( &tCurrentTime );         // GET TIME IN SECS SINCE 1970 IN UTC
            CString s;
            s.Format(_T("%ld"), tCurrentTime );
            SetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_ICEPACK_ATTENTION_LAST_ALERT_DATE, s ); 
        }

        // ARE WE ENABLED FOR ALERTING TO AMS?
        GetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERTING_ENABLED, sTemp );
        DWORD dwIsEnable = STRTOUL( (LPCTSTR) sTemp, &endptr, 10 );
        if( dwIsEnable == 0 )
        {
            // CLEANUP
            ClearTimeInState( lpstNewEvent );   // CLEAR TIMER IF RUNNING
            return(FALSE);                      // NOT ENABLED
        }

        TrimAmsParmsIfTooLong( lpstNewEvent );


        // ADD TO THE QUEUE
        if( pEventList != NULL )
        {
	    	//create event object.
			CEvent *pEvent = new CEvent (lpstNewEvent);

            if( pEvent )
            {
			    // add it to the event queue
		    	pEventList->AddTail(pEvent);
            }
        }


        // CLEANUP
        ClearTimeInState( lpstNewEvent );   // CLEAR TIMER IF RUNNING

    }
    catch(...) 
    {
        //fWidePrintString("FAILURE: CEventDetect::AddNewEventToEventQueue Exception.");
    }

    return(TRUE);
}


/*----------------------------------------------------------------------------
    FillGeneralAlertMsgData()

    Sample Alert Message Format:
    Message Title
    Machine name
    Machine address
    Error message
    Default/help message

      //if( !GetRegistryValue( _T("System\\CurrentControlSet\\Control\\ComputerName\\ComputerName"), _T("ComputerName") , sValue ) )     

    Written by: Jim Hill                               
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::FillGeneralAlertMsgData( CAlertEvent * lpstNewEvent )
{
    TCHAR  szBuff[512];
    DWORD  dwSize = sizeof(szBuff);
    TCHAR*   endptr = NULL;
    CString  sSeq, sVer, s, sBlessedSeqNum, sUnblessedSeqNum;
    CString  sRegKeyAvis, sRegKeyAvisCurrent, sRegKeyQServer;
    BOOL     bDefinitionIsActiveBlessed = 0;

    // REG KEYS
    sRegKeyQServer      = REGKEY_QSERVER;
    sRegKeyAvis         = sRegKeyQServer + REGKEY_ASERVER_SUBKEY;
    sRegKeyAvisCurrent  = sRegKeyAvis + REGKEY_ICEPACK_CURRENT_SUBKEY; 

    // GET OUR MACHINE NAME
    ZeroMemory( szBuff, sizeof(szBuff) );
    GetComputerName( szBuff, &dwSize );
    lpstNewEvent->sMachineName = szBuff;

    // GET THE DNS NAME AND ADDRESS
    LookupHostnameAndAddress( lpstNewEvent->sDNSname, lpstNewEvent->sMachineAddress );

    // DO WE NEED THE DEF SEQ AND VERSION
    if( lpstNewEvent->dwEventId == DEF_ALERT_NEW_BLESSED_DEFS_EVENTID || lpstNewEvent->dwEventId == DEF_ALERT_NEW_UNBLESSED_DEFS_EVENTID )
    {
        sSeq == _T("");
        if( GetRegistryValue( REGKEY_QSERVER_AVIS_CURRENT, REGVALUE_DEFS_IS_ACTIVE_BLESSED, s ) ) 
            bDefinitionIsActiveBlessed = ( STRTOUL( (LPCTSTR)s, &endptr, 10 ) != 0 );

        if( lpstNewEvent->dwEventId == DEF_ALERT_NEW_BLESSED_DEFS_EVENTID )
        {
            if( GetRegistryValue( REGKEY_QSERVER_AVIS_CURRENT, REGVALUE_DEFS_BLESSED_SEQUENCE_NUM, sBlessedSeqNum )) 
            {
                StrTableLoadString( IDSTABLE_TEXT_SEQUENCE_, s );   //  _T("Sequence ")
                sSeq = s + sBlessedSeqNum;
                // if( bDefinitionIsActiveBlessed )    // sSeq.Format( _T(""),  =
                //     sSeq += _T(" [blessed]");
                // else
                //     sSeq += _T(" [unblessed]");
            }
        }
        else if( lpstNewEvent->dwEventId == DEF_ALERT_NEW_UNBLESSED_DEFS_EVENTID )
        {
            // IF THE ACTIVE ONES ARE BLESSED, THEN WE CAN'T DETECT UNBLESSED
            if( !bDefinitionIsActiveBlessed )
            {
                if( GetRegistryValue( REGKEY_QSERVER_AVIS_CURRENT, REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM, sUnblessedSeqNum )) 
                {
                    StrTableLoadString( IDSTABLE_TEXT_SEQUENCE_, s );   //  _T("Sequence ")
                    sSeq = s + sUnblessedSeqNum;   // + _T(" [unblessed]");
                }
            }
        }

        // DO WE HAVE THEM?
        if( !sSeq.IsEmpty() )
            lpstNewEvent->sDefinitions = _T("  ") + sSeq + _T("  ");
    }



    return(TRUE);
}


/*----------------------------------------------------------------------------
    BuildMessageTitle
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::BuildMessageTitle( CString &sMessageTitle, CAlertEvent * lpstNewEvent )
{
    AVIS_EVENT_RULES* lpEventRuleEntry = NULL;


    try
    {
        sMessageTitle = _T("");

        switch( lpstNewEvent->dwEventGroupType )
        {
            case EVENT_GROUP_GENERAL_ERROR:
                StrTableLoadString( IDSTABLE_MSGTITLE_GENERAL_ATTENTION, sMessageTitle );   //  _T("General Attention")
                break;
    
            case EVENT_GROUP_GATEWAY_COMM_ERROR:
                StrTableLoadString( IDSTABLE_MSGTITLE_GATEWAY_ERROR, sMessageTitle );       //  _T("Error Connecting to Gateway")
                break;
    
            case EVENT_GROUP_SAMPLE_ERROR:
                StrTableLoadString( IDSTABLE_MSGTITLE_SAMPLE_ERROR, sMessageTitle );       //  _T("Sample Processing Error")
                break;
    
            case EVENT_GROUP_DEFINITION_ERROR:
                if( lpstNewEvent->dwEventId == DEF_ALERT_INSTATE_DISTRIBUTED_EVENTID )
                    StrTableLoadString( IDSTABLE_MSGTITLE_DEF_ERROR1, sMessageTitle );     //  _T("Definition Processing")
                else
                    StrTableLoadString( IDSTABLE_MSGTITLE_DEF_ERROR2, sMessageTitle );     //  _T("Definition Processing Error")
                break;
    
            case EVENT_GROUP_NEW_DEF_ARRIVAL:
                //if( lpstNewEvent->dwEventId == DEF_ALERT_NEW_BLESSED_DEFS_EVENTID )
                //    sMessageTitle = _T("New Blessed Definitions Arrived");   
                //else if( lpstNewEvent->dwEventId == DEF_ALERT_NEW_UNBLESSED_DEFS_EVENTID )
                //    sMessageTitle = _T("New Unblessed Definitions Arrived");  
                //else
                //    sMessageTitle = _T("New Definitions Arrived");
                //
                //
                // removed 2/12/00
                //if( lpstNewEvent->dwEventId == DEF_ALERT_NEW_BLESSED_DEFS_EVENTID || 
                //    lpstNewEvent->dwEventId == DEF_ALERT_NEW_UNBLESSED_DEFS_EVENTID)
                //{
                //    sMessageTitle = _T("New Definitions Arrived");
                //}
                break;
    
            case EVENT_GROUP_GENERAL_INFORMATIONAL:
                StrTableLoadString( IDSTABLE_MSGTITLE_INFORMATIONAL, sMessageTitle );     //  _T("Informational")
                break;
    
            case EVENT_GROUP_DISK_WARNING:
                StrTableLoadString( IDSTABLE_MSGTITLE_DISK_WARNING, sMessageTitle );      //  _T("Disk Space Warning")
                break;

            default:
                sMessageTitle = _T("");
                break;
        }

//         // GET THE EVENT NAME
//         lpEventRuleEntry = lpstNewEvent->lpEventRuleEntry;
//         if( lpEventRuleEntry != NULL && lpEventRuleEntry->lpszEventName != NULL )
//         {
//             lpstNewEvent->sEventName = lpEventRuleEntry->lpszEventName;
// 
//             // DON'T REPEAT THE SAME NAME ON SAMPLE PROCESSING ERRORS
//             if( lpEventRuleEntry->dwEventNameID != SAMPLE_ALERT_ERROR_EVENTNAME_ID )
//             {
//                 if( sMessageTitle.IsEmpty() )
//                     sMessageTitle  = lpstNewEvent->sEventName;
//                 else
//                     sMessageTitle += _T(" - ") + lpstNewEvent->sEventName;
//             }
//         }

    }
    catch(...) 
    {
    }

    return(TRUE);
}


/*----------------------------------------------------------------------------
    LogEventToNTEventLog

    CServiceModule* m_pServiceModule;

    void CServiceModule::LogEvent( DWORD dwMsgID,
			            		   WORD wType,         // = EVENTLOG_ERROR_TYPE  
					               LPSTR lpszParam1,   // = NULL
					               LPSTR lpszParam2,   // = NULL
					               LPSTR lpszParam3,   // = NULL 
					               LPSTR lpszParam4 )  // = NULL 
    // WinNT.h
    EVENTLOG_ERROR_TYPE      
    EVENTLOG_WARNING_TYPE    
    EVENTLOG_INFORMATION_TYPE

    // qservermsg.h
    IDM_SAMPLE_ALERTABLE_EVENT
    IDM_GENERAL_ALERTABLE_EVENT


    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::LogEventToNTEventLog( CAlertEvent * lpstNewEvent )
{
    LPCTSTR pSourceName = NULL;
	LPCTSTR	aParams[ 2 ];
	HANDLE	hEventSource;
    CString sTemp;
    WORD    wType       = EVENTLOG_ERROR_TYPE;
    DWORD   dwMsgID     = 0;


    // ARE WE ENABLED FOR WRITING TO NT EVENT LOG?
    GetRegistryValue( REGKEY_QSERVER, REGVALUE_WRITE_TO_NT_EVENT_LOG, sTemp );     // _T("QsWriteAlertsToNTeventLog")
    DWORD  dwIsEnable;
    TCHAR* endptr = NULL;
    if( sTemp.IsEmpty() )
        dwIsEnable = 1;     // ENABLE AS THE DEFAULT
    else
        dwIsEnable = STRTOUL( (LPCTSTR) sTemp, &endptr, 10 );
    if( dwIsEnable == 0 )
        return(FALSE);                     // NOT ENABLED


    // DO WE HAVE THE LOG MSG?
    if( lpstNewEvent->sLogFileText.IsEmpty() )
        sTemp = _T(" ");
    else
        sTemp = _T("\r\n") + lpstNewEvent->sLogFileText;

    if( lpstNewEvent->dwEventRulesTableID == EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE )
        dwMsgID = IDM_SAMPLE_ALERTABLE_EVENT;
    else
        dwMsgID = IDM_GENERAL_ALERTABLE_EVENT;

    if( lpstNewEvent->dwEventGroupType == EVENT_GROUP_GENERAL_INFORMATIONAL || 
        lpstNewEvent->dwEventGroupType == EVENT_GROUP_NEW_DEF_ARRIVAL )
        dwMsgID = IDM_GENERAL_INFO;

    // GET THE SOURCE NAME
    if( m_pServiceModule != NULL )
        pSourceName = m_pServiceModule->m_szEventLogServiceName;
    else
        pSourceName = _T(" ");  // Central Quarantine

    // SETUP PARAMS
    aParams[0] = sTemp;               
    aParams[1] = NULL;

    // REGISTER
	hEventSource = RegisterEventSource( NULL, pSourceName );    // m_szEventLogServiceName
	if( hEventSource == NULL )
        return(FALSE);

	if( !ReportEvent (
		  hEventSource,             // Handle to Event Source
		  wType,					// Event type to log
		  0,                        // Event category
		  dwMsgID,					// Event ID (from the MC file)
		  NULL,                     // Security ID
		  1,                        // Number of strings
		  0,
		  aParams,                  // Array of strings to merge with MSG
		  NULL) )                   // Address of RAW data (not used)
				 
	{
		ATLTRACE( "Error reporting event." );
	}

    // Cleanup  
	DeregisterEventSource( hEventSource ); 

    return(TRUE);
}


/*----------------------------------------------------------------------------
    SendTestEvent()

    Send a test event to the AMS server to validate the system.

                                              TOKEN_SEND_TEST_EVENT   
    Written by: Jim Hill                      GENERAL_ATTENTION_SEND_TEST_EVENT  
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::SendTestEvent( CObList *pEventList )
{
    CAlertEvent  stNewEvent;
    CString sTemp     = _T("");
    TCHAR*  lpszToken = TOKEN_SEND_TEST_EVENT;

    // IS THERE A TEST EVENT TO SEND?
    GetRegistryValue( REGKEY_QSERVER, REGVALUE_SEND_TEST_EVENT, sTemp );   // _T("QsWriteAlertsToNTeventLog")
    DWORD  dwIsTestEvent;
    TCHAR* endptr = NULL;

    if( sTemp.IsEmpty() )
        dwIsTestEvent = 0;     
    else
        dwIsTestEvent = STRTOUL( (LPCTSTR) sTemp, &endptr, 10 );
    if( dwIsTestEvent == 0 )
        return(FALSE);                    

    // CLEAR IT
    sTemp = _T("0");
    SetRegistryValue( REGKEY_QSERVER, REGVALUE_SEND_TEST_EVENT, sTemp ); 

    // LOOKUP EVENT CONFIG, INITIALIZE, AND SEND
    InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, lpszToken, NULL );   
    FillGeneralAlertMsgData( &stNewEvent );
    AddNewEventToEventQueue( &stNewEvent, pEventList );


    return(TRUE);
}




/*----------------------------------------------------------------------------
    SetRegistryValue()
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::SetRegistryValue( LPCTSTR pszRegKey, LPCTSTR pszRegValueName, CString &sValue ) 
{
    HRESULT hr = S_OK;
    BOOL    rc = 0;
    DWORD   dwSize = 0;
    CRegKey reg;

    try
    {
        hr = reg.Open( HKEY_LOCAL_MACHINE, pszRegKey, KEY_ALL_ACCESS );
        if( hr != ERROR_SUCCESS )
        {
            // IF IT DOESN'T EXIST, CREATE IT
            hr = reg.Create( HKEY_LOCAL_MACHINE, pszRegKey, REG_NONE, 
                             REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, NULL ); 
            if( hr == ERROR_SUCCESS )        
                rc = TRUE;
        }
        
        if( hr == ERROR_SUCCESS )        
        {
            hr = reg.SetStringValue(  pszRegValueName, (LPCTSTR) sValue); //pszRegKey,
            if( hr == ERROR_SUCCESS )
                rc = TRUE;
        }
    }
    catch(...) 
    {
        rc = FALSE;
    }

    reg.Close();
    return(rc);
}


/*----------------------------------------------------------------------------
    GetRegistryValue
    Written by: Jim Hill                                   REGKEY_QSERVER
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::GetRegistryValue( LPCTSTR pszRegKey, LPCTSTR pszRegValueName, CString &sValue, LPCTSTR pDefaultValue ) 
{
    HRESULT hr = S_OK;
    DWORD   dwSize = 0;
    CRegKey reg;
    TCHAR   szValueBuff[512];
    BOOL    bCreateDefault = FALSE;

    try
    {
        // CLEAR RET VAR
        sValue = _T("");

        if( pDefaultValue == NULL )
            bCreateDefault = TRUE;

        hr = reg.Open( HKEY_LOCAL_MACHINE, pszRegKey, KEY_READ );
        if( hr != ERROR_SUCCESS )
        {
            // IF IT DOESN'T EXIST, CREATE IT
            hr = reg.Create( HKEY_LOCAL_MACHINE, pszRegKey, REG_NONE, 
                             REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, NULL ); 
            if( hr != ERROR_SUCCESS )        
                return FALSE;
        }
        
        // GET STRING VALUE.
        dwSize  = sizeof(szValueBuff);
        ZeroMemory( szValueBuff, dwSize );
        hr = reg.QueryStringValue( pszRegValueName, szValueBuff, &dwSize );
        if( hr == ERROR_SUCCESS )
        {
            sValue = szValueBuff;
        }
        else 
        {
            // VALUE DOES NOT EXIST
            if( pDefaultValue == NULL )
                sValue = _T("");
            else
                sValue = pDefaultValue;
            if( bCreateDefault )
            {
                SetRegistryValue( pszRegKey, pszRegValueName, sValue );
            }
        }
    }
    catch(...) 
    {
        return FALSE;
    }

    reg.Close();
    return(TRUE);
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
    LookupEventRuleByEventId
    Written by: Jim Hill                                                  
 ----------------------------------------------------------------------------*/
AVIS_EVENT_RULES* LookupEventRuleByEventId( AVIS_EVENT_RULES* lpTable, DWORD dwEventId )
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


/*----------------------------------------------------------------------------
    InitializeEventByEventId()
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::InitializeEventByEventId( CAlertEvent* lpstNewEvent, DWORD dwEventId )
{
    AVIS_EVENT_RULES*  lpTableEventRule = NULL;
    DWORD              dwEventRulesTableID = 0;

    try
    {
        // CLEAR
        lpstNewEvent->ClearEventData();

        // TRY THE GENERAL TABLE
        dwEventRulesTableID = EVENT_TOKEN_GENERAL_ATTENTION_TABLE;
        lpTableEventRule    = LookupEventRuleByEventId( &g_AvisEventRulesGeneralAttention[0], dwEventId );  
        if( lpTableEventRule == NULL )
        {
            // TRY THE SAMPLE TABLE
            dwEventRulesTableID = EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE;
            lpTableEventRule    = LookupEventRuleByEventId( &g_AvisEventRulesSampleIntervention[0], dwEventId );  
        }
        if( lpTableEventRule == NULL )
            return(FALSE);
        
        // SAVE THE EVENT TABLE ENTRY
        lpstNewEvent->lpEventRuleEntry         = lpTableEventRule;
        lpstNewEvent->dwEventRulesTableID      = dwEventRulesTableID;
        //lpszLookupToken                        = lpTableEventRule->lpszIcePackToken;

        lpstNewEvent->dwEventGroupType         = lpTableEventRule->dwEventGroupType        ;        
        lpstNewEvent->dwEventId                = lpTableEventRule->dwEventId               ;               
        lpstNewEvent->dwAlertSeverityLevel     = lpTableEventRule->dwAlertSeverityLevel    ;    
        lpstNewEvent->dwOperationalStatusLevel = lpTableEventRule->dwOperationalStatusLevel;
        lpstNewEvent->dwIcePackTokenTableID    = lpTableEventRule->dwIcePackTokenTableID;
        lpstNewEvent->sIcePackTokenString      = lpTableEventRule->lpszIcePackToken;

        if( lpTableEventRule->lpszDefaultMessage != NULL )
            lpstNewEvent->sDefaultOrHelpMsg = lpTableEventRule->lpszDefaultMessage;

        return(TRUE);
    }
    catch(...) 
    {
    }
    return(FALSE);
}


/*----------------------------------------------------------------------------
    InitializeEmptyEventStruct()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::InitializeEmptyEventStruct( CAlertEvent* lpstNewEvent, DWORD dwEventRulesTableID, LPCTSTR lpszToken, LPCTSTR lpszParentToken )
{
    AVIS_EVENT_RULES*  lpTable = NULL;
    AVIS_EVENT_RULES*  lpTableEventRule = NULL;
    LPCTSTR            lpszLookupToken =NULL;

    if( lpszToken == NULL )
        return(FALSE);

    // CLEAR
    lpstNewEvent->ClearEventData();

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
        if( lpszParentToken != NULL )
        {
           lpstNewEvent->sIcePackParentTokenString = lpszParentToken;
           lpstNewEvent->sIcePackParentTokenString.TrimLeft(); 
           lpstNewEvent->sIcePackParentTokenString.TrimRight();
           lpszLookupToken                         = lpstNewEvent->sIcePackParentTokenString;
        }
        else
        {
           lpszLookupToken = lpszToken;

        }

        lpTableEventRule = LookupEventRule( lpTable, lpszLookupToken );   //lpszToken
        if( lpTableEventRule == NULL )
            return(TRUE);
        lpstNewEvent->lpEventRuleEntry         = lpTableEventRule;
        lpstNewEvent->dwEventRulesTableID      = dwEventRulesTableID;

        lpstNewEvent->dwEventGroupType         = lpTableEventRule->dwEventGroupType        ;        
        lpstNewEvent->dwEventId                = lpTableEventRule->dwEventId               ;               
        lpstNewEvent->dwAlertSeverityLevel     = lpTableEventRule->dwAlertSeverityLevel    ;    
        lpstNewEvent->dwOperationalStatusLevel = lpTableEventRule->dwOperationalStatusLevel;
        lpstNewEvent->dwIcePackTokenTableID    = lpTableEventRule->dwIcePackTokenTableID;
        if( lpszToken != NULL )
        {
            lpstNewEvent->sIcePackTokenString  = lpszToken;
            lpstNewEvent->sIcePackTokenString.TrimLeft();
            lpstNewEvent->sIcePackTokenString.TrimRight();
        }
        if( lpTableEventRule->lpszDefaultMessage != NULL )
            lpstNewEvent->sDefaultOrHelpMsg = lpTableEventRule->lpszDefaultMessage;

        // SAVE THE EVENT TABLE ENTRY
        lpstNewEvent->lpEventRuleEntry      = lpTableEventRule;

    }
    return(TRUE);
}


static bStringsReadFromStrTable = FALSE;
/*----------------------------------------------------------------------------
    ReadAllConfigDataFromRegistry()
    Read in all config data from Registry.
    Written by: Jim Hill                       
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::ReadAllConfigDataFromRegistry()
{
    AVIS_EVENT_RULES* lpTable = NULL;
    int i = 0;

    try
    {
        // READ FOR GENERAL EVENTS  
        lpTable = g_AvisEventRulesGeneralAttention;
        for( i = 0; lpTable->lpszIcePackToken != NULL ; i++, lpTable++ )
        {
            ReadEventConfigData( lpTable );
        }
        
        // READ FOR SAMPLE EVENTS  
        lpTable = g_AvisEventRulesSampleIntervention;
        for( i = 0; lpTable->lpszIcePackToken != NULL ; i++, lpTable++ )
        {
            ReadEventConfigData( lpTable );
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
    ReadEventConfigData()
    Read in single event config data from Registry.
    Written by: Jim Hill                       
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::ReadEventConfigData( AVIS_EVENT_RULES* lpTableEntry )
{
    CString  sValue = _T("");
    TCHAR    szBuff[512];
    TCHAR*   lpszToken=NULL;
    TCHAR*   endptr=NULL;
    LPCTSTR  lpszRegVal = NULL;
    DWORD	 dwEnabled  = 0;           
    DWORD    dwMinTimeInStateMinutes = 0;

    // VALIDATE
    if( lpTableEntry == NULL )
        return(FALSE);
    // INITIALIZE
    ZeroMemory( szBuff, sizeof(szBuff) );

    try
    {
        if( lpTableEntry->lpszIcePackToken == NULL || (lpszRegVal = lpTableEntry->lpszRegValue) == NULL )
            return(FALSE);

        // LOAD THE STRINGS?
        if( !bStringsReadFromStrTable )
        {
            lpTableEntry->lpszEventName = StrTableLookUpMiscString( lpTableEntry->dwEventNameID );
            lpTableEntry->lpszDefaultMessage = StrTableLookUpMiscString( lpTableEntry->dwDefaultMessageID );
        }


        // CAN THE USER CONFIGURE THIS ONE? IF NOT, DON'T BOTHER TO READ IT
        if( !lpTableEntry->dwIsConfigurable )
            return(FALSE);

        // ENTRIES STORED as Enabled, MinTime i.e. 0,15
        if( GetRegistryValue( REGKEY_QSERVER, lpszRegVal , sValue ) )     
        {
            // DO WE HAVE A SETTING? IF NOT, WRITE DEFAULT
            if( sValue.IsEmpty() )
            {
                lpTableEntry->dwMinTimeInStateSecs = lpTableEntry->dwMinTimeInStateMinutes * 60;  // THE STATIC DEFAULT TABLE IS IN MINUTES
                sValue.Format(_T("%d,%d"), lpTableEntry->dwEnabled, lpTableEntry->dwMinTimeInStateMinutes ); // dwMinTimeInStateSecs
                SetRegistryValue( REGKEY_QSERVER, lpszRegVal, sValue );
                return(FALSE);
            }

            if( !GetNextNumbersFromCommaString( sValue, &dwEnabled, &dwMinTimeInStateMinutes ) )
                return(FALSE);


            // // // ONLY APPLIES TO GENERAL ATTENTION PROBLEMS WITH THE GATEWAY OR DISKSPACE
            // if( lpTableEventRule->dwInStateTimerExpire != 0  && 
            //     lpTableEventRule->dwEventId == GENERAL_ATTENTION_GATEWAY_COMM_EVENTID &&
            //     pEventObj->dwEventRulesTableID == EVENT_TOKEN_GENERAL_ATTENTION_TABLE )

            // IS THERE A TIMER RUNNING THAT SHOULD BE CHANGED?    2/18/00
            // ONLY APPLIES TO GENERAL ATTENTION PROBLEMS WITH THE GATEWAY OR DISKSPACE.
            // THE SAMPLE EVENT TIMERS ARE STORED IN ATTRIBUTES, NOT IN THE GLOBAL TABLE.
            DWORD  dwNewTimeInStateSecs = (dwMinTimeInStateMinutes * 60);
            DWORD  dwCurrentGmtTime     = (DWORD) TimeToGmtTime( 0 );
            // IS THERE A TIMER AND HAS THE CONFIG SETTING CHANGED?
            if( lpTableEntry->dwInStateTimerExpire != 0 && dwNewTimeInStateSecs != lpTableEntry->dwMinTimeInStateSecs ) 
            {
                // EXPIRED YET?
                if( dwCurrentGmtTime < lpTableEntry->dwInStateTimerExpire )
                {
                    int iNewTimer = 0;
                    int iInStateTimerExpire = (int) lpTableEntry->dwInStateTimerExpire;
                    int iTimeDiference = abs( int (dwNewTimeInStateSecs - lpTableEntry->dwMinTimeInStateSecs) );
                    if( dwNewTimeInStateSecs < lpTableEntry->dwMinTimeInStateSecs )
                        iNewTimer = iInStateTimerExpire - iTimeDiference;
                    else
                        iNewTimer = iInStateTimerExpire + iTimeDiference;
                    if( iNewTimer < 0 )
                        iNewTimer = 1;
        
                    // RESTART IT USING THE NEW VALUE
                    lpTableEntry->dwInStateTimerExpire = (DWORD) iNewTimer;
                    DWORD  dwTimeRemaining = lpTableEntry->dwInStateTimerExpire - (DWORD) dwCurrentGmtTime;
                    if( lpTableEntry->dwInStateTimerExpire < (DWORD) dwCurrentGmtTime )
                        dwTimeRemaining = 0;
#ifdef _DEBUG
                    fWidePrintString("NOTE ReadEventConfigData: Timer reset with new value [%s] Secs remaining %d", lpTableEntry->lpszIcePackToken, dwTimeRemaining );
#endif
                }
            }


            // UPDATE THE EVENT FULES  existing 2/18/00
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
BOOL CEventDetect::GetNextNumbersFromCommaString( CString& s, DWORD* pNum1, DWORD* pNum2 )
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
        vpstrncpy( szBuff, (LPCTSTR) s, sizeof (szBuff) );
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
    GetNextTokensFromCommaString()

    Given a string of comma delimited strings, parse next 2, and
    return the strings in the passed parameters.

    Return the remainder of the string in passed CString

    Destroys content of passed string.

    Written by: Jim Hill                      TCHAR* pszString
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::GetNextTokensFromCommaString(CString& s, CString& sToken1, CString& sToken2 )
{
    TCHAR    szBuff[512];
    TCHAR*   lpszToken=NULL;
    TCHAR*   endptr=NULL;
    
    // INITIALIZE
    ZeroMemory( szBuff, sizeof(szBuff) );

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
        sToken1 = lpszToken;

        lpszToken = _tcstok( NULL, _T(",") );
        if( lpszToken == NULL )
            return(FALSE);
        // WALK PAST ANY EXTRA COMMAS
        for( ; *lpszToken == ','; lpszToken++ )
        {
            if( *lpszToken == 0 )   // BAIL
                return(FALSE);
        }
        sToken2 = lpszToken;

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


static bDiskQuotaAlertSent = FALSE;
 
/*----------------------------------------------------------------------------
    CheckDiskSpace()

    Ckeck free disk space, and size of QFolder
    
    Written by: Jim Hill                      
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::CheckDiskSpace( CObList *pEventList )
{
    CAlertEvent  stNewEvent;
    CString      sTemp;
    CString      sQuarantineDir;
    TCHAR*       endptr=NULL;
    //DWORD        dwLowWaterPercent = 0;
    //DWORD        dwQsWarningSize = 0;
    DWORD        dwSpaceRemaining = 0;
    DWORD        dwDiskQuota      = 0;
    DWORD        dwWarningSize = 0;


    try
    {
        // GET DISK WARNING LEVEL
        GetRegistryValue( REGKEY_QSERVER, REGVALUE_QFOLDER_WARNING_SIZE, sTemp );
        dwWarningSize = STRTOUL( (LPCTSTR) sTemp, &endptr, 10 );

        //fWidePrintString("NOTE General: CheckDiskSpace " );
        
        // CHECK DISK QUOTA
        if( m_pServiceModule != NULL )
        {
            // LOOKUP EVENT CONFIG, INITIALIZE
            InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_QUARANTINE_FULL, NULL );

			// first check number of samples that are in CQ to the limit.
			if (_Module.m_ulItemCount >= _Module.GetFileQuota())
			{
                if( IsEnabledAndTimeInStateOk( &stNewEvent ) )
                {
                    FillGeneralAlertMsgData( &stNewEvent );
                    AddNewEventToEventQueue( &stNewEvent, pEventList );
                    bDiskQuotaAlertSent = TRUE;
                }
			}
            else   
            {
                // CLEAR THE TIMER
                if( stNewEvent.lpEventRuleEntry != NULL && stNewEvent.lpEventRuleEntry->dwInStateTimerExpire != 0 )
                {
                    stNewEvent.lpEventRuleEntry->dwInStateTimerExpire = 0;
#ifdef _DEBUG
                    fWidePrintString("NOTE General: CheckDiskSpace  Timer stopped on [%s]", stNewEvent.sIcePackTokenString );
#endif
                }
                bDiskQuotaAlertSent = FALSE;
            }    
            dwDiskQuota = m_pServiceModule->GetDiskQuota();
            if( dwDiskQuota != 0 )
            {
                if( dwWarningSize == 0  )
                {
                    // DEFAULT IS 90%
                    if( dwDiskQuota == 0)
                        dwDiskQuota = 1;
                    dwWarningSize = (dwDiskQuota * 9)/10;    
                    if( dwWarningSize == 0)
                        dwWarningSize = 1;
                    sTemp.Format( _T("%d"), dwWarningSize );
                    SetRegistryValue( REGKEY_QSERVER, REGVALUE_QFOLDER_WARNING_SIZE, sTemp );
                }

                // CONVERT FROM MEG TO BYTES
                dwDiskQuota   *= 0x100000;       
                dwWarningSize *= 0x100000;

                // LOOKUP EVENT CONFIG, INITIALIZE
                InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_DISKQUOTA_LOW_WATER, NULL );
    
                if( m_pServiceModule->m_uQserverByteCount > dwWarningSize )
                { 
                    if( dwDiskQuota > m_pServiceModule->m_uQserverByteCount )   // test
                        dwSpaceRemaining = dwDiskQuota - m_pServiceModule->m_uQserverByteCount;
                    else
                        dwSpaceRemaining = 0;
    
                    // SHOULD WE SEND IT YET? BUT ONLY ONCE UNTIL RESET. 4/29/00
                    // if( !bDiskQuotaAlertSent && IsEnabledAndTimeInStateOk( &stNewEvent ) )
                    if( IsEnabledAndTimeInStateOk( &stNewEvent ) )
                    {
                        FillGeneralAlertMsgData( &stNewEvent );
                        AddNewEventToEventQueue( &stNewEvent, pEventList );
                        bDiskQuotaAlertSent = TRUE;
                    }
                }
                else   
                {
                    // CLEAR THE TIMER
                    if( stNewEvent.lpEventRuleEntry != NULL && stNewEvent.lpEventRuleEntry->dwInStateTimerExpire != 0 )
                    {
                        stNewEvent.lpEventRuleEntry->dwInStateTimerExpire = 0;
#ifdef _DEBUG
                        fWidePrintString("NOTE General: CheckDiskSpace  Timer stopped on [%s]", stNewEvent.sIcePackTokenString );
#endif
                    }
                    bDiskQuotaAlertSent = FALSE;
                }    
    

#if 0
        // GET DISK QUOTA LOW WATER LEVEL
        GetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_DISKQUOTA_LOW_WATER, sTemp );
        dwLowWaterPercent = STRTOUL( (LPCTSTR) sTemp, &endptr, 10 );
        if( dwLowWaterPercent == 0 || dwLowWaterPercent > 100 )
        {
            // DEFAULT IS 10%
            dwLowWaterPercent = 10;    
            sTemp.Format( _T("%d"), dwLowWaterPercent );
            SetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_DISKQUOTA_LOW_WATER, sTemp );
        }

        if( m_pServiceModule != NULL )
        {
            dwDiskQuota = m_pServiceModule->GetDiskQuota() * 0x100000;
            if( dwDiskQuota != 0 )
            {
                dwQsWarningSize = ( dwDiskQuota / 100 )  * dwLowWaterPercent;
            
                // LOOKUP EVENT CONFIG, INITIALIZE
                InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_DISKQUOTA_LOW_WATER, NULL );
    
                if( (m_pServiceModule->m_uQserverByteCount + dwQsWarningSize) > dwDiskQuota )
                { 
                    if( dwDiskQuota > m_pServiceModule->m_uQserverByteCount )
                        dwSpaceRemaining = dwDiskQuota - m_pServiceModule->m_uQserverByteCount;
                    else
                        dwSpaceRemaining = 0;
    
                    // SHOULD WE SEND IT YET?
                    if( IsEnabledAndTimeInStateOk( &stNewEvent ) )
                    {
                        FillGeneralAlertMsgData( &stNewEvent );
                        AddNewEventToEventQueue( &stNewEvent, pEventList );
                    }
                }
                else   
                {
                    // CLEAR THE TIMER
                    if( stNewEvent.lpEventRuleEntry != NULL && stNewEvent.lpEventRuleEntry->dwInStateTimerExpire != 0 )
                    {
                        stNewEvent.lpEventRuleEntry->dwInStateTimerExpire = 0;
                        fWidePrintString("NOTE General: CheckDiskSpace  Timer stopped on [%s]", stNewEvent.sIcePackTokenString );
                    }
                }    
#endif

    
                // CHECK DISK FREE SPACE
                GetRegistryValue( REGKEY_QSERVER, REGVALUE_QUARANTINE_FOLDER, sQuarantineDir );
                LPCTSTR lpDirectoryName = sQuarantineDir;
                if( sQuarantineDir.IsEmpty() )
                    lpDirectoryName = NULL;
    
                ULARGE_INTEGER iFreeBytesAvailableToCaller ;
                ULARGE_INTEGER iTotalNumberOfBytes     ;
                ULARGE_INTEGER iTotalNumberOfFreeBytes ;
                GetDiskFreeSpaceEx( lpDirectoryName, &iFreeBytesAvailableToCaller, &iTotalNumberOfBytes, &iTotalNumberOfFreeBytes );
                DWORD64  dwDiskFreeSpace64       = iTotalNumberOfFreeBytes.QuadPart;       
                //DWORD64  dwDiskFreeWarningSize64 = (DWORD64)( ( dwDiskQuota / 10 ) * 11 );   // 110%
                DWORD64  dwDiskFreeWarningSize64 = (DWORD64)( dwDiskQuota );   // 100%
                
                // LOOKUP EVENT CONFIG, INITIALIZE
                InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_DISKSPACE_LOW_WATER, NULL );
                
                // ARE WE AT LOW WATER?
                if( dwDiskFreeSpace64 < dwDiskFreeWarningSize64 )
                { 
                    // SHOULD WE SEND IT YET?
                    if( IsEnabledAndTimeInStateOk( &stNewEvent ) )
                    {
                        FillGeneralAlertMsgData( &stNewEvent );
                        AddNewEventToEventQueue( &stNewEvent, pEventList );    // 
                    }                                                  
                }
                else   
                {
                    // CLEAR THE TIMER
                    if( stNewEvent.lpEventRuleEntry != NULL && stNewEvent.lpEventRuleEntry->dwInStateTimerExpire != 0 )
                    {
                        stNewEvent.lpEventRuleEntry->dwInStateTimerExpire = 0;
#ifdef _DEBUG
                        fWidePrintString("NOTE General: CheckDiskSpace  Timer stopped on [%s]", stNewEvent.sIcePackTokenString );
#endif
                    }
                } 

                // SAVE OUT THE FREE SPACE 4/29/00
                dwSpaceRemaining = (DWORD) ( dwDiskFreeSpace64 / 0x100000 );
                if( dwSpaceRemaining == 0 )
                    dwSpaceRemaining = 1;
                sTemp.Format( _T("%d"), dwSpaceRemaining );
                SetRegistryValue( REGKEY_QSERVER, REGVALUE_QFOLDER_FREE_SPACE, sTemp );

            }
        }
    }
    catch(...) 
    {
#ifdef _DEBUG
        fWidePrintString("FAILURE: CEventDetect::CheckDiskSpace Exception.");
#endif
    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
    TimeToGmtTime()
    Convert passed time value to GMT time.
    If tTime == 0, return Current GMT time

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
//time_t CEventDetect::TimeToGmtTime( time_t tTime )
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
    RemoveTokenFromGeneralAttentionString()

    Remove the passed token from the General attention string

    Written by: Jim Hill                                     
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::RemoveTokenFromGeneralAttentionString( CAlertEvent * lpstNewEvent, CString &sNewPreviousValue )
{
    CString sToken       = _T("");
    int     pos          = 0;

    try
    {
        // IF IT'S NOT TIME YET, THEN REMOVE IT FROM THE STRING WE WRITE OUT
        sToken = lpstNewEvent->sIcePackTokenString + ",";
        pos    = sNewPreviousValue.Find( sToken );
        if( pos == -1 )
        {
            sToken = "," + lpstNewEvent->sIcePackTokenString;
            pos    = sNewPreviousValue.Find( sToken );
            if( pos == -1 )
            {
                sToken = lpstNewEvent->sIcePackTokenString;
                pos    = sNewPreviousValue.Find( sToken );
            }
        }
        if( pos >= 0 )
        {
            sNewPreviousValue.Delete( pos, sToken.GetLength() );
        }

        return(TRUE);

    }
    catch(...)
    {
    }
    return(FALSE);
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



#include "winsock.h"
/*----------------------------------------------------------------------------
    Written by: Jim Hill                      
 ----------------------------------------------------------------------------*/
BOOL LookupHostnameAndAddress( CString& sHostName, CString& sHostAddress )
{
    WSADATA  WinsockData;
    WORD     wVersion = 0x0101;     // Version 1.1 
    struct   sockaddr_in  saAddr;
    struct   hostent*     lpHostEntry = NULL;
    char     szDnsHostname[512];
    char     szHostname[255];
    char*    lpNetAddress = NULL;
    TCHAR    szBuff[512];

    // INITIALIZE 
    sHostName.Empty();
    sHostAddress.Empty();
    ZeroMemory( &WinsockData, sizeof(WSADATA) );
    ZeroMemory( szDnsHostname, sizeof(szDnsHostname) );
    ZeroMemory( szHostname, sizeof(szHostname) );
    ZeroMemory( szBuff, sizeof(szBuff) );
    ZeroMemory( &saAddr, sizeof(struct sockaddr) );

    if( WSAStartup(wVersion,&WinsockData) != 0 )  
        return(FALSE);

    // LOOK IT UP
    gethostname( szHostname, sizeof(szHostname) );
    if(!(lpHostEntry = gethostbyname(szHostname))) 
    {
        int lastError=WSAGetLastError();
        WSACleanup();
        return(FALSE);
    }

    // GET THE ADDRESS
    memcpy((void *)&saAddr.sin_addr,(void *)lpHostEntry->h_addr,lpHostEntry->h_length);
    lpNetAddress = inet_ntoa(saAddr.sin_addr);           // convert to dotted notation
    if( lpNetAddress != NULL )
    {
        // CONVERT TO WIDE
        if( MultiByteToWideChar(CP_ACP,0,lpNetAddress,-1,szBuff,sizeof(szBuff)/ sizeof(TCHAR)) )   
            sHostAddress = szBuff;
    }

    // DO THE DNS NAME
    strcpy(szDnsHostname,lpHostEntry->h_name);
    // CONVERT TO WIDE
    if( MultiByteToWideChar(CP_ACP,0,szDnsHostname,-1,szBuff,sizeof(szBuff)/ sizeof(TCHAR)) )   
       sHostName = szBuff;

    // SHUT IT DOWN
    WSACleanup();
    return(TRUE);
}


/*----------------------------------------------------------------------------
    LoadEventStringTableAndConfigData()
    Called from qserver.cpp on startup.
    Written by: Jim Hill                      
 ----------------------------------------------------------------------------*/
int LoadEventStringTableAndConfigData()
{
    CEventDetect  EventDetect;

    EventDetect.ReadAllConfigDataFromRegistry();

    return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CEventDetect::ProccesEventQueue
//
// Description: Proccess the Queue of events
//
// Return type: HRESULT
//
//
///////////////////////////////////////////////////////////////////////////////
// 01-18-00 - TMARLES	: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CEventDetect::ProccesEventQueue(CObList *pEventList)
{
	HRESULT hr = S_OK;
	CEvent  *pData= NULL; 
	CString sEventTitle;

	if (pEventList == NULL)
		return E_INVALIDARG;
	
	POSITION pos = pEventList->GetHeadPosition();
	while (pos)
	{
		pData = static_cast <CEvent *>(pEventList->GetAt(pos));
		// do work here
		if (pData)
		{
		    BuildMessageTitle (sEventTitle, &pData->m_AvisAlertEvent);
			if (pData->m_pQSItem == NULL)
			{
				// do general event work here	
#ifdef USE_AMS
				SendQSGeneralAMSEvent(pData, sEventTitle);
#endif
				hr = S_OK;
			}
			else
			{
				// do sample event work here	
#ifdef USE_AMS
				SendQSSampleAMSEvent(pData, sEventTitle);
#endif
				hr = S_OK;
			}

		}
		pEventList->GetNext(pos);
	}

    return(hr);
}


/*----------------------------------------------------------------------------
    TestGeneralErrors()
    Written by: tmarles              download,defcast,disk,query,submit,target
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::TestGeneralErrors(CObList *pEventList, DWORD dwModuleData)
{
    CAlertEvent  stNewEvent;
    HRESULT hr = S_OK;
    TCHAR   szBuff[512];
    TCHAR*  lpszToken=NULL;
    TCHAR*  lpszParentToken=NULL;
    CString sCurrentValue     = _T("");
    CString sPreviousValue    = _T("");
    CString sNewPreviousValue = _T("");
    CString sTemp             = _T("");
    //DWORD   dwEnableGeneralAttention = 0;
    BOOL    bIsGatewayAlert   = FALSE;
    TCHAR*  endptr=NULL;
    time_t  tLastTime = 0;
    time_t  tCurrentTime = 0;
    time_t  tElapsedTime = 0;  
    try
    {
        // SAVE THE MODULE PTR
        m_pServiceModule = (CServiceModule*) dwModuleData;
        //fWidePrintString( "NOTE: CEventDetect  ServiceModule ptr= %x", dwModuleData );

        
        // INITIALIZE
        ZeroMemory( szBuff, sizeof(szBuff) );

        // CHECK DISK SPACE
        TestCheckDiskSpace( pEventList );


        // DETECT ARRIVAL OF NEW DEFINITIONS    
        TestNewDefinitionArrival( pEventList );
    
		sCurrentValue = GENERAL_ATTENTION_GATEWAY_QUERY_TOKEN_LIST;
        // GET CURRENT AND PREVIOUS ATTENTION TOKENS
//        if( !GetRegistryValue( REGKEY_QSERVER_AVIS, REGVALUE_ATTENTION, sCurrentValue )) // REGKEY_QSERVER  4/19/00 changed to use Avis key instead jhill
//            return(FALSE);
//        if( !GetRegistryValue( REGKEY_QSERVER, REGVALUE_ALERT_ICEPACK_ATTENTION_LAST, sPreviousValue )) 
//            return(FALSE);
//        sCurrentValue.Remove(' ');
//        sCurrentValue.Remove('\t');
//        sPreviousValue.Remove(' ');
//        sPreviousValue.Remove('\t');
    
    
        // OTHERWISE PARSE THE TOKENS

        vpstrncpy( szBuff, (LPCTSTR) sCurrentValue ,sizeof (szBuff));
        lpszToken = _tcstok( szBuff, _T(",") );
        for( int i=0; lpszToken != NULL; i++ )
        {
            lpszParentToken = NULL;

            // DID WE HAVE TWO COMMAS TOGETHER 1/22/00
            if( *lpszToken == ',' )
                continue;

            // IS THIS A GATEWAY COMM EVENT?
            sTemp = GENERAL_ATTENTION_GATEWAY_QUERY_TOKEN_LIST;
            if( sTemp.Find( lpszToken ) >= 0 )
            {
                lpszParentToken = TOKEN_GATEWAYCOMM;  // _T("gatewayComm") 
                bIsGatewayAlert = TRUE;
            }
            // LOOKUP EVENT CONFIG AND INITIALIZE
            InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, lpszToken, lpszParentToken );   // 

            FillGeneralAlertMsgData( &stNewEvent );
            AddNewEventToEventQueue( &stNewEvent, pEventList );


            lpszToken = _tcstok( NULL, _T(",") );
        }


    }
    catch(...) 
    {
        //fWidePrintString("FAILURE: CEventDetect::DetectGeneralErrors Exception.");
    }

    return(TRUE);
}
/*----------------------------------------------------------------------------
    CheckDiskSpace()

    Ckeck free disk space, and size of QFolder
    
    Written by: tmarles
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::TestCheckDiskSpace( CObList *pEventList )
{
    CAlertEvent  stNewEvent;
    CString      sTemp;
    CString      sQuarantineDir;
    TCHAR*       endptr=NULL;


    try
    {
		        
        // CHECK DISK QUOTA
        if( m_pServiceModule != NULL )
        {

            // LOOKUP EVENT CONFIG, INITIALIZE
            InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_DISKQUOTA_LOW_WATER, NULL );

            FillGeneralAlertMsgData( &stNewEvent );
            AddNewTestEventToEventQueue( &stNewEvent, pEventList );

    
                
            // LOOKUP EVENT CONFIG, INITIALIZE
            InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_DISKSPACE_LOW_WATER, NULL );
            
            FillGeneralAlertMsgData( &stNewEvent );
            AddNewTestEventToEventQueue( &stNewEvent, pEventList );    // 

        }
    }
    catch(...) 
    {
#ifdef _DEBUG
        fWidePrintString("FAILURE: CEventDetect::CheckDiskSpace Exception.");
#endif
    }
    return(TRUE);
}

/*----------------------------------------------------------------------------
    AddNewEventToEventQueue()
    ADD TO EVENT QUEUE
    Written by: tmarles
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::AddNewTestEventToEventQueue( CAlertEvent * lpstNewEvent, CObList *pEventList )
{
    CString sTemp  = _T("");
    TCHAR*  endptr = NULL;

    try
    {
#ifdef MY_SYSTEM
        MessageBeep(MB_ICONEXCLAMATION);
#endif

        BuildAlertMessage( lpstNewEvent );
        if( !lpstNewEvent->sLogFileText.IsEmpty() )
        {
#ifdef _DEBUG
            fWidePrintString("ALERT: %s\r\n",(LPCTSTR) lpstNewEvent->sLogFileText);
#endif
            // WRITE TO NT EVENT LOG
            LogEventToNTEventLog( lpstNewEvent );
        }



        TrimAmsParmsIfTooLong( lpstNewEvent );


        // ADD TO THE QUEUE
        if( pEventList != NULL )
        {
	    	//create event object.
			CEvent *pEvent = new CEvent (lpstNewEvent);

            if( pEvent )
            {
			    // add it to the event queue
		    	pEventList->AddTail(pEvent);
            }
        }



    }
    catch(...) 
    {
        //fWidePrintString("FAILURE: CEventDetect::AddNewEventToEventQueue Exception.");
    }

    return(TRUE);
}

/*----------------------------------------------------------------------------
    DETECT ARRIVAL OF NEW DEFINITIONS    
    Written by: Jim Hill                                     
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::TestNewDefinitionArrival(CObList *pEventList)
{
    CAlertEvent  stNewEvent;

    CString sBessedSeqNum     = _T("");
    CString sBessedSeqNumLast = _T("");

    // DETECT ARRIVAL OF NEW UNBLESSED DEFS    
    TestNewUnblessedDefArrival( pEventList );

    // DETECT ARRIVAL OF NEW BLESSED DEFS    
    try
    {
        // READ
        if( !GetRegistryValue( REGKEY_QSERVER_AVIS_CURRENT, REGVALUE_DEFS_BLESSED_SEQUENCE_NUM, sBessedSeqNum )) 
            return(FALSE);
        if( !GetRegistryValue( REGKEY_QSERVER, REGVALUE_DEFS_BLESSED_SEQUENCE_NUM_LAST, sBessedSeqNumLast )) 
            return(FALSE);

        // VALIDATE
        if( sBessedSeqNum.IsEmpty() )
            return(FALSE);


        // INITIALIZE THE EVENT DATA
        InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_NEWBLESSEDDEF, NULL );     // _T("newBlessedDef")
        // SHOULD WE REPORT IT?
        FillGeneralAlertMsgData( &stNewEvent );
        AddNewEventToEventQueue( &stNewEvent, pEventList );
        return(TRUE);
    }
    catch(...) 
    {
        //fWidePrintString("FAILURE: CEventDetect::DetectNewDefinitionArrival Exception.");
    }

    return(FALSE);
}

/*----------------------------------------------------------------------------
    DetectNewUnblessedDefArrival()

    DETECT ARRIVAL OF NEW UNBLESSED DEFS    
    Written by: Jim Hill                                     
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::TestNewUnblessedDefArrival(CObList *pEventList)
{
    CAlertEvent  stNewEvent;
    BOOL    bDefinitionIsActiveBlessed = 0;
    TCHAR*  endptr=NULL;
    CString sUnbessedSeqNum     = _T("");
    CString sUnbessedSeqNumLast = _T("");
    CString s                   = _T("");

    try
    {
        // READ
        if( !GetRegistryValue( REGKEY_QSERVER_AVIS_CURRENT, REGVALUE_DEFS_IS_ACTIVE_BLESSED, s )) 
            return(FALSE);
        bDefinitionIsActiveBlessed = ( STRTOUL( (LPCTSTR)s, &endptr, 10 ) != 0 );
        // IF THE ACTIVE ONES ARE BLESSED, THEN WE CAN'T DETECT UNBLESSED

        GetRegistryValue( REGKEY_QSERVER_AVIS_CURRENT, REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM, sUnbessedSeqNum );
        GetRegistryValue( REGKEY_QSERVER, REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM_LAST, sUnbessedSeqNumLast );
        


        // INITIALIZE THE EVENT DATA
        InitializeEmptyEventStruct( &stNewEvent, EVENT_TOKEN_GENERAL_ATTENTION_TABLE, TOKEN_NEWUNBLESSEDDEF, NULL );    // _T("newUnblessedDef")
        // SHOULD WE REPORT IT?
        FillGeneralAlertMsgData( &stNewEvent );
        AddNewEventToEventQueue( &stNewEvent, pEventList );
        return(TRUE);
    }
    catch(...) 
    {
        //fWidePrintString("FAILURE: CEventDetect::DetectNewDefinitionArrival Exception.");
    }
    return(FALSE);
}

