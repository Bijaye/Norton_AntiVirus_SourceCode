/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// SampleEventDetection.cpp



#include "stdafx.h"
#include "AvisEventDetection.h"     // in QuarantineServer\Include
#include "EventObject.h"






// LOCAL
BOOL IsSampleErrorCondition( AVIS_QSERVERITEM* pItem );
BOOL GetSampleAttribute( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, CString &sValue, VARTYPE vt );
BOOL SetSampleAttribute( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, CString &sValue, VARTYPE vt );
BOOL GetSampleAttributeDate( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, time_t* ptTime );
BOOL SetSampleAttributeDate( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, time_t tTime );
void GetStatusToken(CString& sStatusToken, CString& sStatusID );
void GetStatusDisplayText(CString& sStatusToken, CString& sStatusID );
BOOL FillSampleAlertMsgData( IQuarantineServerItem* pItem, CAlertEvent * lpstNewEvent );
BOOL GetSampleAge( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, CString& sAge );
UINT GetFileNameFromPath(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);


#if 0
/*----------------------------------------------------------------------------
    IsSampleErrorCondition()
    Added to QsConsole code

    This provides a method for QsConsole to change the icon of a sample
    if there are any errors. This is to aid the user in visually identifying 
    those samples needing manual attention.

    This requires the following Sample Attributes:
    X-Sample-Status       // DWORD stored as a string
    X-Sample-Result       // string

    The possible error conditions are:
    1. Sample Intervention Needed
       X-Sample-Status attribute is one of the following:
       "held,available,attention,error,notinstalled"

    2. Local Quarantine Intervention Needed
       X-Sample-Status attribute is one of the following: "unneeded,installed"
       And X-Scan-Result does NOT contain "repaired"


    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL IsSampleErrorCondition( AVIS_QSERVERITEM* pItem )
{
    CString sTemp           = _T("");
    CString sSampleStatusID = _T("");
    CString sSampleResult   = _T("");
    CString sSampleAlertStatusTokenIDs = _T("");
    CString sQuarantineAlertStatusTokenIDs = _T("");

    // INITIALIZE
    sSampleAlertStatusTokenIDs.Format(_T("%d,%d,%d,%d,%d"),STATUS_HELD,STATUS_AVAILIABLE,STATUS_ATTENTION,STATUS_ERROR,STATUS_NOTINSTALLED);  
    sQuarantineAlertStatusTokenIDs.Format(_T("%d,%d"),STATUS_UNNEEDED,STATUS_INSTALLED);  


    try
    {
        // TEST
        if( pItem == NULL)
            return(FALSE);
        pItem->lpszSampleStatus = _T("11");;    // "notinstalled"
        pItem->lpszSampleResult = _T("wrong");
        // TEST
        sSampleStatusID         = pItem->lpszSampleStatus;
        sSampleResult           = pItem->lpszSampleResult;
        // TEST


        // IS THIS A SAMPLE INTERVENTION ALERT CONDITION?  X-Sample-Status is a Dword stored as a string.
        if( sSampleAlertStatusTokenIDs.Find(sSampleStatusID) >= 0 )             // is it in the list
            return(TRUE);

        // TEST STRING
        sSampleStatusID         = _T("8");

        // IS THIS A LOCAL QUARANTINE INTERVENTION ALERT CONDITION?
        if( sQuarantineAlertStatusTokenIDs.Find(sSampleStatusID) >= 0 )             // is it in the list
        {
            sTemp = QUARANTINE_ALERT_RESULT_TOKENS;
            if( sTemp.Find(sSampleResult) == -1 )        // condition != "repaired"
                return(TRUE);
        }

    }
    catch(...) 
    {
        //fWidePrintString("FAILURE: IsSampleErrorCondition Exception.");
    }
    return(FALSE);
}
#endif



static    DWORD   dwClearLastAlert = 0;
static    DWORD   dwSetCurrentStatus = 0;

/*----------------------------------------------------------------------------
    DetectSampleError()
    Assume that caller has determined that REGVALUE_ALERT_SAMPLE_INTERVENTION
    is enabled
    Asume that the interval to check for has been read.


To determine the "sample intervention" alert condition, the Quarantine
Service must examine each sample in quarantine like this:

   If X-Sample-Status has one of the values listed below, the sample
   requires manual intervention.

   If "alertSampleIntervention" is enabled, sample alerts should be
   included in alert messages.

   If the X-Alert-Status attribute does not exist or its value does not
   match the X-Sample-Status attribute, the sample alert condition has
   changed.

   If the condition has not changed, the age of the alert can be calculated
   from the X-Date-Alert attribute.

   If the condition has changed, the value of X-Sample-Status should be
   stored in X-Alert-Status and the current date and time should be stored
   in X-Date-Alert for use in the next interval.

   The value of X-Sample-Status should be mapped to a national-language
   message using the new IcePackStatusTable in the IcePackTokens.CPP file,
   and the national-language message should be appended to the message
   body, along with some identification for the sample.

   If the condition has not changed, the age of the alert should be
   included in the message body.

   If the condition has changed, the alert should be flagged.

The values of X-Sample-Status that indicate a "sample intervention" alert
condition are:

   held
   available
   attention
   error
   notinstalled


The values of X-Sample-Status that indicate a sample "too long in state" error
condition are:
    quarantined
    submitted
    released
    needed
    available
    distributed
    distribute




    All times handled in GMT time.

    STATUS_QUARANTINED     0        
    STATUS_SUBMITTED       1
    STATUS_HELD            2
    STATUS_RELEASED        3
    STATUS_UNNEEDED        4
    STATUS_NEEDED          5
    STATUS_AVAILIABLE      6
    STATUS_DISTRIBUTED     7                 
    STATUS_INSTALLED       8
    STATUS_ATTENTION       9
    STATUS_ERROR           10 
    STATUS_NOTINSTALLED    11               // new in Jedi release
    STATUS_RESTART         12               // new in Jedi release
    STATUS_LEGACY          13               // new in Jedi release
    STATUS_DISTRIBUTE      14               // new in Jedi release

d:\Proj\Norton_AntiVirus\QuarantineServer\shared\BinIntelUnicodeWin32Debug\qserver.exe

    Written by: Jim Hill   
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::DetectSampleError( IQuarantineServerItem* pItem, CAlertEvent *pNewEvent, DWORD dwModuleData )
{
    int  rc;
    CString sTemp           = _T("");
    CString sSampleStatus   = _T("");
    CString sSampleStatusID = _T("");
    CString sAlertStatus    = _T("");
    CString sError          = _T("");
    CString sCurrentStatus  = _T("");
    CString sStatusTimer     = _T("");
    time_t  tStatusTimer     = 0;
    TCHAR*  endptr=NULL;
    time_t  tLastGmtTime = 0;
    time_t  tCurrentGmtTime = 0;
    
// #ifdef DISABLE_EVENT_DETECTION
//     // DISABLE 1/22/00 JHILL
//     return(TRUE); 
// #endif

	// Check params 
	if (pNewEvent == NULL || pItem == NULL)
		return FALSE;

    try
    {
        // SAVE THE MODULE PTR
        m_pServiceModule = (CServiceModule*) dwModuleData;

        // INITIALIZE
        pNewEvent->ClearEventData();

        // ARE WE ENABLED FOR ALERTING?
        GetRegistryValue( REGKEY_QSERVER, _T("TestEnableAlertEngine"), sTemp );
        DWORD dwIsEnable;
        if( sTemp.IsEmpty() )
            dwIsEnable = 1;     // ENABLE AS THE DEFAULT
        else
            dwIsEnable = STRTOUL( (LPCTSTR) sTemp, &endptr, 10 );
        if( dwIsEnable == 0 )
            return(FALSE);                     // NOT ENABLED

        // fWidePrintString("NOTE Sample: DetectSampleError Process sample" );

        // WHEN DID WE SEND THE LAST ALERT FOR THIS SAMPLE?
        rc = GetSampleAttributeDate( pItem, QSITEMINFO_X_DATE_ALERT, &tLastGmtTime ); // GMT TIME IN SECS SINCE 1970 IN UTC
        tCurrentGmtTime = TimeToGmtTime( 0 );  // GMT TIME IN SECS SINCE 1970 IN UTC

        // Get X-Sample-Status This is a DWORD stored as a string.
        rc = GetSampleAttribute( pItem, QSITEMINFO_X_SAMPLE_STATUS, sSampleStatusID, VT_BSTR );
        // Pass the Dword as a string. sSampleStatus then contains the token
        GetStatusToken(sSampleStatus, sSampleStatusID );  

        // GET THE STATUS TIMER
        GetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sStatusTimer, VT_BSTR );
        tStatusTimer = STRTOUL( (LPCTSTR) sStatusTimer, &endptr, 10 );

        // Get X-Alert-Status This is a string.
        rc = GetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sAlertStatus, VT_BSTR );

        // IF IT'S "error", THEN LOOK UP THE SPECIFIC ERROR 
        sCurrentStatus = sSampleStatus;

         if( sSampleStatus == TOKEN_ERROR )     //_T("error")
        {
            // Get X-Error value. This is a string.
            rc = GetSampleAttribute( pItem, QSITEMINFO_X_ERROR, sError, VT_BSTR );
            if( !sError.IsEmpty() )
                sCurrentStatus = sError;
        }
        else if( sSampleStatus == TOKEN_ATTENTION )   // _T("attention")
        {
            // Get X-Attention value. This is a string.
            rc = GetSampleAttribute( pItem, QSITEMINFO_X_ATTENTION, sError, VT_BSTR );
        }

        sSampleStatus.TrimLeft(); 
        sSampleStatus.TrimRight();
          

        // DEBUG jhill 2/18/00
        CString sFileName;
        GetSampleAttribute( pItem, QSITEMINFO_X_SAMPLE_FILE, sFileName, VT_BSTR );
        TCHAR szFileName[512];
        szFileName[0] = 0;
        GetFileNameFromPath( sFileName, szFileName, sizeof(szFileName) );
        // fWidePrintString("NOTE Sample: DetectSampleError evaluate  File= %s", szFileName );

        // Get X-Alert-Status-Last This is a string.
        //rc = GetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_LAST, sAlertStatusLast, VT_BSTR );

        // CHECK FOR TIME IN STATE ERRORS. INCLUDES ONLY ONES WITH TIMERS
        sTemp = SAMPLE_ALERT_STATUS_TIMER_TOKEN_LIST;   
        // if( sTemp.Find( sSampleStatus ) >= 0 && pNewEvent!= 0 )
        if( FindStringInCommaDelimitedList( sTemp, sSampleStatus ) >= 0  && pNewEvent!= 0 )  
        {
            // GET EVENT DATA
            InitializeEmptyEventStruct( pNewEvent, EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE, sSampleStatus, NULL );  // sSampleStatus
            AVIS_EVENT_RULES* lpRule = pNewEvent->lpEventRuleEntry;
            // ARE WE ENABLED?
            if( lpRule == NULL || !lpRule->dwEnabled )
            {
                // CLEAR THE TIMER?
                if( lpRule != NULL && !lpRule->dwEnabled )  // ARE WE ENABLED?
                {
                    sTemp = _T("");
                    SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR );
                }
                return(FALSE);                     // NO CHANGE
            }

            // CHECK FOR TIME IN STATE ERRORS
            if( sCurrentStatus != sAlertStatus )                        // if( sCurrentStatus == sAlertStatus )
            {
                // ARE WE RUNNING A TIMER?
                if( lpRule->dwMinTimeInStateSecs > 0 && tStatusTimer == 0 )
                {
                    // START A NEW TIMER
                    sTemp.Format(_T("%d"), (tCurrentGmtTime + lpRule->dwMinTimeInStateSecs) );
                    SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR ); 
#ifdef _DEBUG
                    fWidePrintString("NOTE Sample: DetectSampleError  Timer started on  [%s] Secs remaining = %d  File= %s", pNewEvent->sIcePackTokenString, lpRule->dwMinTimeInStateSecs, szFileName );
#endif
                }
                else  
                {
                    // TIMER EXPIRED?
                    if( lpRule->dwMinTimeInStateSecs == 0 || tStatusTimer <= tCurrentGmtTime )
                    {
#ifdef _DEBUG
                        fWidePrintString("NOTE Sample: DetectSampleError  Timer stopped on  [%s]  File= %s", pNewEvent->sIcePackTokenString, szFileName );
#endif
                        FillSampleAlertMsgData( pItem, pNewEvent );
                        AddNewEventToEventQueue( pNewEvent, NULL );  // CObList *pEventList
                        // SAVE THE CURRENT STATUS AS X-Alert-Status
                        SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sSampleStatus, VT_BSTR );  
                        // SAVE THE CURRENT DATE/TIME AS X-Date-Alert
                        SetSampleAttributeDate( pItem, QSITEMINFO_X_DATE_ALERT, tCurrentGmtTime );
                        // CLEAR THE TIMER
                        sTemp = _T("");
                        SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR );
                        return(TRUE);
                    }

#ifdef  MY_SYSTEM
                    int iTimeRemaining = abs( int (tStatusTimer - tCurrentGmtTime) );
                    fWidePrintString("NOTE Sample: DetectSampleError  Timer still running on [%s] Secs remaining = %d  File= %s", pNewEvent->sIcePackTokenString, iTimeRemaining, szFileName );
#endif
                }
                return(FALSE);              // NO CHANGE 

            }
            else if( tStatusTimer != 0 )    // CHANGE IN STATUS  
            {
                sTemp = _T("");             // CLEAR THE TIMER
                SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR ); 
#ifdef _DEBUG
                fWidePrintString("NOTE Sample: DetectSampleError  Timer cleared for [%s]  File= %s", pNewEvent->sIcePackTokenString, szFileName );
#endif
            }

            // NOW FALL THROUGH TO LOOK FOR SAMPLE OR QUARANTINE ERROR CONDITIONS
        }
        else if( tStatusTimer != 0 )    // CHANGE IN STATUS  
        {
            sTemp = _T("");             // CLEAR THE TIMER
            SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR ); 
#ifdef _DEBUG
            fWidePrintString("NOTE Sample: DetectSampleError  Timer cleared for File= %s", szFileName );
#endif
        }


        // IS THIS NEW? 
        if( sCurrentStatus != sAlertStatus )
        {
            // IS THIS A SAMPLE ERROR CONDITION?
            sTemp = SAMPLE_ALERT_TOKEN_LIST;
            //if( sTemp.Find( sSampleStatus ) >= 0 && pNewEvent!= 0 )
            if( FindStringInCommaDelimitedList( sTemp, sSampleStatus ) >= 0  && pNewEvent!= 0 )  
            {
                // GET EVENT DATA
                rc = InitializeEmptyEventStruct( pNewEvent, EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE, sCurrentStatus, NULL );  
            
                // IS THIS AN ATTENTION ERROR? If so, X-Attention contains the actual string.
                if( sSampleStatus == TOKEN_ATTENTION )   // _T("attention")
                {
					//todo tmarles this is where we want to do the check for "nav" or "norton.once we get permission to check in the code.4-23-02
					sError.Replace(_T(" NAV "), _T(" Symantec AntiVirus "));
//					sError.Replace(_T(" Norton "), _T(" Symantec "));
					sError.Replace(_T("NAV "), _T("Symantec AntiVirus "));
                    pNewEvent->sErrorText = sError;
                }

                // SHOULD WE REPORT IT?
                if( IsEnabledAndTimeInStateOk( pNewEvent ) )    
                {
                    FillSampleAlertMsgData( pItem, pNewEvent );
                    AddNewEventToEventQueue( pNewEvent, NULL );  // CObList *pEventList
            
                    // SAVE THE CURRENT STATUS AS X-Alert-Status
                    rc = SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sCurrentStatus, VT_BSTR );  
                
                    // SAVE THE CURRENT DATE/TIME AS X-Date-Alert
                    rc = SetSampleAttributeDate( pItem, QSITEMINFO_X_DATE_ALERT, tCurrentGmtTime );
                    return(TRUE);
                }
                return(FALSE);
            }
        }
        

        // IS THIS A NEW LOCAL QUARANTINE ERROR? 
        if( sCurrentStatus != sAlertStatus )
        {
#ifdef _DEBUG
            fWidePrintString("NOTE: CEventDetect sCurrentStatus=%s  sAlertStatus=%s", sCurrentStatus, sAlertStatus );
#endif
            // A local quarantine alert is needed in these situations because the user and/or
            // administrator must take some action to remove the sample from local
            // quarantine on the client machine:
            // 
            //    When X-Sample-Status is 'unneeded' and X-Scan-Result is 'uninfected',
            //    definitions are not installed on the client, no repair of the
            //    quarantined file is attempted, and the file will remain in local
            //    quarantine until the administrator and/or user take some action.  The
            //    action should be to restore the file to its original directory.
            // 
            //    When X-Sample-Status is 'installed' and X-Scan-Result is 'unrepairable',
            //    definitions are installed on the client, but the attempted repair of the
            //    quarantined file is unsuccessful, so the file will remain in local
            //    quarantine until the administrator and/or user take some action.  The
            //    action should probably be to delete the file.

            // IS THIS A LOCAL QUARANTINE ERROR CONDITION?
            sTemp = QUARANTINE_ALERT_STATUS_TOKENS;
            // if( sTemp.Find(sSampleStatus) >= 0 )             // is it in the list
            if( FindStringInCommaDelimitedList( sTemp, sSampleStatus ) >= 0  && pNewEvent!= 0 )  
            {
                // Get X-Scan-Result This is a string.
                CString sResult = _T("");
                GetSampleAttribute( pItem, QSITEMINFO_X_SCAN_RESULT, sResult, VT_BSTR );
        
                sTemp = QUARANTINE_ALERT_RESULT_TOKENS;
                // if( sTemp.Find(sResult) == -1 )        // X-Scan-Result != "repaired" 
                if( FindStringInCommaDelimitedList( sTemp, sResult ) == -1 )  
                {
                    // Get X-Alert-Result. This is a string.
                    CString sAlertResult = _T("");
                    GetSampleAttribute( pItem, QSITEMINFO_X_ALERT_RESULT, sAlertResult, VT_BSTR );
                    
                    //fWidePrintString("NOTE: CEventDetect sResult=%s  sAlertResult=%s", sResult, sAlertResult );
        
                    // IS THIS NEW? 
                    if( sResult != sAlertResult )
                    {
                        // GET EVENT DATA
                        InitializeEventByEventId( pNewEvent, QUARANTINE_ALERT_NOT_REPAIRED_EVENTID );
                        // SHOULD WE REPORT IT?
                        if( IsEnabledAndTimeInStateOk( pNewEvent ) )    
                        {
                            FillSampleAlertMsgData( pItem, pNewEvent );
                            AddNewEventToEventQueue( pNewEvent, NULL );
            
                            // SAVE THE CURRENT STATUS AS X-zAlert-Result
                            sTemp = TOKEN_NOTREPAIRED;
                            rc = SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_RESULT, sTemp, VT_BSTR );  
                        
                            // SAVE THE CURRENT STATUS AS X-Alert-Status
                            SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sSampleStatus, VT_BSTR );  
        
                            // SAVE THE CURRENT DATE/TIME AS X-Date-Alert
                            rc = SetSampleAttributeDate( pItem, QSITEMINFO_X_DATE_ALERT, tCurrentGmtTime );
                            return(TRUE);
                        }
                    }
                }
            }
        }


    }
    catch(...) 
    {
#ifdef _DEBUG
        fWidePrintString("FAILURE: CEventDetect::DetectSampleError Exception.");
#endif
    }

    return(FALSE);
}

#if 0
/////////////////////////
        // CHECK FOR TIME IN STATE ERRORS. INCLUDES ONLY ONES WITH TIMERS
        sTemp = SAMPLE_ALERT_STATUS_TIMER_TOKEN_LIST;   
        if( sTemp.Find( sSampleStatus ) >= 0 && pNewEvent!= 0 )
        {
            // GET EVENT DATA
            InitializeEmptyEventStruct( pNewEvent, EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE, sSampleStatus, NULL );  // sSampleStatus
            AVIS_EVENT_RULES* lpRule = pNewEvent->lpEventRuleEntry;
            // ARE WE ENABLED?
            if( lpRule == NULL || !lpRule->dwEnabled )
            {
                // CLEAR THE TIMER?
                if( lpRule != NULL && !lpRule->dwEnabled )  // ARE WE ENABLED?
                {
                    sTemp = _T("");
                    SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR );
                }
                return(FALSE);                     // NO CHANGE
            }

            // CHECK FOR TIME IN STATE ERRORS
            if( sCurrentStatus == sAlertStatus )        
            {
                // ARE WE RUNNING A TIMER?
                if( lpRule->dwMinTimeInStateSecs > 0 && tStatusTimer == 0 )
                {
                    // START A NEW TIMER
                    sTemp.Format(_T("%d"), (tCurrentGmtTime + lpRule->dwMinTimeInStateSecs) );
                    SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR ); 
#ifdef _DEBUG
                    fWidePrintString("NOTE Sample: DetectSampleError  Timer started on  [%s] Secs remaining = %d", pNewEvent->sIcePackTokenString, lpRule->dwMinTimeInStateSecs );
#endif
                }
                else  
                {
                    // TIMER EXPIRED?
                    if( lpRule->dwMinTimeInStateSecs == 0 || tStatusTimer < tCurrentGmtTime )
                    {
#ifdef _DEBUG
                        fWidePrintString("NOTE Sample: DetectSampleError  Timer stopped on  [%s]", pNewEvent->sIcePackTokenString );
#endif
                        FillSampleAlertMsgData( pItem, pNewEvent );
                        AddNewEventToEventQueue( pNewEvent, NULL );  // CObList *pEventList
                        // SAVE THE CURRENT STATUS AS X-Alert-Status
                        SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sSampleStatus, VT_BSTR );  
                        // SAVE THE CURRENT DATE/TIME AS X-Date-Alert
                        SetSampleAttributeDate( pItem, QSITEMINFO_X_DATE_ALERT, tCurrentGmtTime );
                        // CLEAR THE TIMER
                        sTemp = _T("");
                        SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR );
                        return(TRUE);
                    }

                    int iTimeRemaining = abs( int (tStatusTimer - tCurrentGmtTime) );
#ifdef _DEBUG
                    fWidePrintString("NOTE Sample: DetectSampleError  Timer still running on [%s] Secs remaining = %d", pNewEvent->sIcePackTokenString, iTimeRemaining );
#endif
                }
                return(FALSE);                     // NO CHANGE 

            }
            else  // CHANGE IN STATUS
            {
                // START A NEW TIMER?
                if( lpRule->dwMinTimeInStateSecs > 0 ) 
                {
#ifdef _DEBUG
                    fWidePrintString("NOTE Sample: DetectSampleError  Timer started on  [%s] Secs remaining = %d", pNewEvent->sIcePackTokenString, lpRule->dwMinTimeInStateSecs );
#endif
                    sTemp.Format(_T("%d"), (tCurrentGmtTime + lpRule->dwMinTimeInStateSecs) );
                }
                else   
                {
#ifdef _DEBUG
                    fWidePrintString("NOTE Sample: DetectSampleError  Timer stopped for [%s]", pNewEvent->sIcePackTokenString );
#endif
                    sTemp = _T("");       // CLEAR THE TIMER
                }
                SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR ); 
                // SAVE THE CURRENT STATUS AS X-Alert-Status
                SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sSampleStatus, VT_BSTR ); 
                Sleep(50);
            }

            // NOW FALL THROUGH TO LOOK FOR SAMPLE OR QUARANTINE ERROR CONDITIONS
        }
#endif



/*----------------------------------------------------------------------------
    FillSampleAlertMsgData()

    Sample Alert Message Format:
    Message Title
    User
    Machine name
    Machine address
    Platform
    File Name
    Virus Name
    Reason
    Age of Sample
    Error message
    Default/help message


    Written by: Jim Hill                        
 ----------------------------------------------------------------------------*/
BOOL FillSampleAlertMsgData( IQuarantineServerItem* pItem, CAlertEvent * lpstNewEvent )
{
    CString  sSeq, sVer, sSampleStatusID;
    WCHAR*   lpszString = NULL;

    try
    {
        GetSampleAttribute( pItem, QSITEMINFO_X_PLATFORM_USER    , lpstNewEvent->sUser          , VT_BSTR );
        GetSampleAttribute( pItem, QSITEMINFO_X_PLATFORM_COMPUTER, lpstNewEvent->sMachineName   , VT_BSTR );
        GetSampleAttribute( pItem, QSITEMINFO_X_PLATFORM_HOSTNAME, lpstNewEvent->sDNSname       , VT_BSTR );
        GetSampleAttribute( pItem, QSITEMINFO_X_PLATFORM_ADDRESS , lpstNewEvent->sMachineAddress, VT_BSTR );
        GetSampleAttribute( pItem, QSITEMINFO_X_PLATFORM_SYSTEM  , lpstNewEvent->sPlatformName  , VT_BSTR );
        GetSampleAttribute( pItem, QSITEMINFO_X_SAMPLE_FILE      , lpstNewEvent->sFileName      , VT_BSTR );
        GetSampleAttribute( pItem, QSITEMINFO_X_SAMPLE_REASON    , lpstNewEvent->sReason        , VT_BSTR );


        // GET VIRUS NAME, FIRST FROM THE ANALYSIS CENTER
        GetSampleAttribute( pItem, QSITEMINFO_X_ANALYSIS_VIRUS_NAME, lpstNewEvent->sVirusName   , VT_BSTR );
        if( lpstNewEvent->sVirusName.IsEmpty() )
        {
            // IF THAT FAILS, USE THE ONE FROM BLOODHOUND
            GetSampleAttribute( pItem, QSITEMINFO_X_SCAN_VIRUS_NAME,lpstNewEvent->sVirusName    , VT_BSTR );
            if( lpstNewEvent->sVirusName.IsEmpty() )
                StrTableLoadString( IDSTABLE_HEADING_UNKNOWN, lpstNewEvent->sVirusName );     //  _T("Unknown")
        }

        // Get X-Sample-Status This is a DWORD stored as a string.
        GetSampleAttribute( pItem, QSITEMINFO_X_SAMPLE_STATUS, sSampleStatusID, VT_BSTR );
        // Pass the Dword. sSampleStatus then contains the token
        GetStatusDisplayText(lpstNewEvent->sSampleStatus, sSampleStatusID );  
        
        // GET THE SEQ AND VER OF THE DEFINITIONS NEEDED
        //CString  sHeading1, sHeading2;
        //GetSampleAttribute( pItem, _T(QSERVER_ITEM_INFO_DEF_VERSION),  sVer, VT_BSTR );
        //StrTableLoadString( IDSTABLE_STAB_TEXT_VERSION_NUMBER, sHeading2 );                   // _T("Ver: ")
        CString  sHeading1;
        GetSampleAttribute( pItem, QSITEMINFO_X_SIGNATURES_SEQUENCE, sSeq, VT_BSTR );
        StrTableLoadString( IDSTABLE_TEXT_SEQUENCE_, sHeading1 );                             // _T("Sequence: ")

        // FIXUP DEFAULT TEXT IF IT'S A "NOT INSTALLED" EVENT 
        if( lpstNewEvent->dwEventId == DEF_ALERT_NOT_INSTALLED_EVENTID && !sSeq.IsEmpty() )
        {
            //GetSampleAttribute( pItem, QSITEMINFO_X_SIGNATURES_SEQUENCE, sSeq , VT_BSTR );
            sSeq.TrimLeft( _T(" 0") );  // TRIM LEADING ZEROS AND SPACES
            if( !sSeq.IsEmpty() )
                lpstNewEvent->sDefaultOrHelpMsg += sSeq;
        }

        // BUILD THE DEFINITIONS NEEDED STRING 
        if( !sSeq.IsEmpty() )
            lpstNewEvent->sDefinitions = sHeading1 + sSeq; 


//         // BUILD THE DEFINITIONS NEEDED STRING 
//         if( !sVer.IsEmpty() )
//             sVer = sHeading2 + sVer;
//         if( !sSeq.IsEmpty() )
//             lpstNewEvent->sDefinitions = sHeading1 + sSeq + _T("   ");
//         else if( !sVer.IsEmpty() )
//             lpstNewEvent->sDefinitions = sVer;
//         lpstNewEvent->sDefinitions.TrimLeft();

        

        // CALCULATE THE AGE    Qconn Itemdate.cpp
        GetSampleAge( pItem, _T(QSERVER_ITEM_INFO_QUARANTINE_QDATE), lpstNewEvent->sAgeOfSample );

        // GET THE SAMPLE STATE  "X-Analysis-State"
        GetSampleAttribute( pItem, _T(QSERVER_ITEM_INFO_SAMPLE_STATE), lpstNewEvent->m_sSampleState, VT_BSTR ); 

        // TRIM PATH FROM FILE NAME
        if( !lpstNewEvent->sFileName.IsEmpty() )
        {
            TCHAR  szFilename[512] = {0}; 
            GetFileNameFromPath(lpstNewEvent->sFileName, szFilename, sizeof( szFilename ) );
            if( szFilename[0] != 0 )
                lpstNewEvent->sFileName = szFilename;
        }

        // TRIM
        lpstNewEvent->sMachineAddress.TrimRight();
        lpstNewEvent->sDNSname.TrimLeft();       

    }
    catch(...) 
    {
#ifdef _DEBUG
        fWidePrintString("FAILURE: FillSampleAlertMsgData Exception.");
#endif
    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
    GetSampleAttribute()
    Lookup single attribute

    Written by: Jim Hill                                   NTE_NOT_FOUND
 ----------------------------------------------------------------------------*/
BOOL GetSampleAttribute( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, CString &sValue, VARTYPE vt )
{

    HRESULT hr     = S_OK;
    VARIANT v;
    VariantInit( &v );

    try
    {
        hr = pItem->GetValue( psSampleAttribute, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                if( v.vt == VT_UI4 )
                    sValue.Format(_T("%d"), v.ulVal );
                else if( v.vt == VT_BSTR )
                    sValue = v.bstrVal;
				VariantClear(&v);
                return(TRUE);   
            }
        }
        else
            sValue.Empty();
    }
    catch(...) 
    {
    }
    return(FALSE);
}


/*----------------------------------------------------------------------------
    SetSampleAttribute()
    Write single attribute

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL SetSampleAttribute( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, CString &sValue, VARTYPE vt )
{
    HRESULT hr     = S_OK;
    HRESULT hr2    = S_OK;
    VARIANT v;
    VariantInit( &v );
    TCHAR*  endptr=NULL;

    try
    {
        // INITIALIZE
        if( sValue.IsEmpty() )
            sValue = _T(" ");     // NEEDS AT LEAST 1 CHR TO WRITE            hr = S_OK; 

        v.vt = vt;
        switch( vt )
        {
            case VT_BSTR:
                v.bstrVal = sValue.AllocSysString();
                break;

            case VT_UI4:
                v.ulVal = STRTOUL( (LPCTSTR) sValue, &endptr, 10 );
                break;

            //case VT_DATE:
            //    time_t  tTimeSecs = STRTOUL( (LPCTSTR) sValue, &endptr, 10 );
            //    struct tm* GmtTime = gmtime( &tTimeSecs );
            //    break;

            default:
                return(FALSE);  
                break;
        }

        hr = pItem->SetValue( psSampleAttribute, v );
        if( SUCCEEDED( hr ) )
        {
            // COMMIT CHANGES TO DISK
            hr2 = pItem->Commit();
            if( SUCCEEDED( hr2 ) )
                return(TRUE);   
        }
		VariantClear (&v); //tam 5-25-00 free up bstr in v
        if( !SUCCEEDED( hr ) || !SUCCEEDED( hr2 ) )
        {
#ifdef _DEBUG
            fWidePrintString("FAILURE: SetSampleAttribute failed with %s  hr=0x%x  hr2=0x%x", psSampleAttribute, hr, hr2);
#endif
        }
    }
    catch(...)
    {
    }
    return(FALSE);
}


/*----------------------------------------------------------------------------
    GetSampleAttributeDate()
    Look up a date.
    Convert to time_t

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL GetSampleAttributeDate( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, time_t* ptTime )
{
    HRESULT hr     = S_OK;
    BOOL    rc = 0;
    VARIANT v;
    VariantInit( &v );
    SYSTEMTIME SysTime;

    try
    {
        hr = pItem->GetValue( psSampleAttribute, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt == VT_DATE )
            {
                VariantTimeToSystemTime( v.date, &SysTime );
                CTime ctTime( SysTime );
                *ptTime = ctTime.GetTime( ); 
            }
			VariantClear(&v);
        }
    }

    catch(...)
    {
    }
    return(FALSE);
}


/*----------------------------------------------------------------------------
    SetSampleAttributeDate()
    Write single attribute

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL SetSampleAttributeDate( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, time_t tTime )
{
    HRESULT hr     = S_OK;
    BOOL    rc = 0;
    VARIANT v;
    VariantInit( &v );
    SYSTEMTIME timeDest;

    try
    {
        if( tTime > 0 )
        {
            CTime ctTime( tTime );
            rc = ctTime.GetAsSystemTime( timeDest ); 

            v.vt = VT_DATE;
            SystemTimeToVariantTime( &timeDest, &v.date );


            hr = pItem->SetValue( psSampleAttribute, v );
            if( SUCCEEDED( hr ) )
            {
                // COMMIT CHANGES TO DISK
                hr = pItem->Commit();
                if( SUCCEEDED( hr ) )
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
    GetStatusToken
    Looks up the text for X-Sample-Status 
 ----------------------------------------------------------------------------*/
void GetStatusToken(CString& sStatusToken, CString& sStatusID )
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
        }

    return;
}


/*----------------------------------------------------------------------------
    GetStatusDisplayText
    Looks up the text for X-Sample-Status 
 ----------------------------------------------------------------------------*/
void GetStatusDisplayText(CString& sStatusToken, CString& sStatusID )
{
    DWORD dwStatus = 0;
    TCHAR*  endptr=NULL;
    
    dwStatus = STRTOUL( (LPCTSTR) sStatusID, &endptr, 10 );

    switch( dwStatus )
        {
        case STATUS_QUARANTINED:
            StrTableLoadString( IDSTABLE_STATUS_QUARANTINED, sStatusToken );    //  _T("quarantined")
            break;

        case STATUS_SUBMITTED:
            StrTableLoadString( IDSTABLE_STATUS_SUBMITTED, sStatusToken );      //  _T("submitted")
            break;

        case STATUS_HELD:
            StrTableLoadString( IDSTABLE_STATUS_HELD, sStatusToken );           //  _T("held")
            break;

        case STATUS_RELEASED:
            StrTableLoadString( IDSTABLE_STATUS_RELEASED, sStatusToken );       //  _T("released")
            break;

        case STATUS_UNNEEDED:
            StrTableLoadString( IDSTABLE_STATUS_UNNEEDED, sStatusToken );       //  _T("unneeded")
            break;

        case STATUS_NEEDED:
            StrTableLoadString( IDSTABLE_STATUS_NEEDED, sStatusToken );         //  _T("needed")
            break;

        case STATUS_AVAILIABLE:
            StrTableLoadString( IDSTABLE_STATUS_AVAILABLE, sStatusToken );      //  _T("available")
            break;

        case STATUS_DISTRIBUTE:                  
            StrTableLoadString( IDSTABLE_STATUS_DISTRIBUTE, sStatusToken );     //  _T("Distribute")
            break;

        case STATUS_DISTRIBUTED:
            StrTableLoadString( IDSTABLE_STATUS_DISTRIBUTED, sStatusToken );    //  _T("Distributed")
            break;

        case STATUS_INSTALLED:                 
            StrTableLoadString( IDSTABLE_STATUS_INSTALLED, sStatusToken );      //  _T("Installed")
            break;

        case STATUS_ATTENTION:                 
            StrTableLoadString( IDSTABLE_STATUS_ATTENTION, sStatusToken );      //  _T("Attention")
            break;

        case STATUS_ERROR:                     
            StrTableLoadString( IDSTABLE_STATUS_ERROR, sStatusToken );          //  _T("Error")
            break;

        case STATUS_NOTINSTALLED:                
            StrTableLoadString( IDSTABLE_STATUS_NOTINSTALLED, sStatusToken );   //  _T("Not installed")
            break;

        case STATUS_RESTART:                     
            StrTableLoadString( IDSTABLE_STATUS_RESTART, sStatusToken );        //  _T("Restart")
            break;

        case STATUS_LEGACY:                      
            StrTableLoadString( IDSTABLE_STATUS_LEGACY, sStatusToken );         //  _T("legacy")

        default:
            StrTableLoadString( IDSTABLE_STATUS_UNKNOWN, sStatusToken );        //  _T("unknown")
        }

    return;
}




//#include "resource.h"
/*----------------------------------------------------------------------------
    GetSampleAge()
    Lookup single date attribute, and calc the age

    Written by: Jim Hill           QSERVER_ITEM_INFO_QUARANTINE_QDATE                        
 ----------------------------------------------------------------------------*/
BOOL GetSampleAge( IQuarantineServerItem* pItem, TCHAR* psSampleAttribute, CString& sAge )
{
    //AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    HRESULT  hr     = S_OK;
    CString  s, sDate;
    VARIANT  v;
    VariantInit( &v );
    SYSTEMTIME st, stRecieved;
    TCHAR szBuffer[ 32 ];
    WCHAR*   lpszFormat;

    try
    {
        sAge.Empty();
        hr = pItem->GetValue( psSampleAttribute, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt == VT_DATE )
            {
				VariantTimeToSystemTime( v.date, &st );
                SystemTimeToTzSpecificLocalTime( NULL,
                                                 &st,
                                                 &stRecieved );

                CTime ctQTime(stRecieved), ctCurrentTime;
				ctCurrentTime = CTime::GetCurrentTime();
				if (ctQTime > ctCurrentTime)  // 7-20-00 terrym added to prevent neg numbers in alerting log
				{
					sAge = "--";
				}
				else
				{
					CTimeSpan  cstAge = ctCurrentTime - ctQTime;
					//sAge = cstAge.Format( "%D days, %H hours, %M mins"  );            // ("%D days, %H hours, %M mins" ); IDS_QS_TIME_FORMAT
					lpszFormat = StrTableLookUpMiscString( IDSTABLE_QS_SAGE_FORMAT );
					sAge = cstAge.Format( lpszFormat );                                 // "%D days, %H hours, %M mins" 

				}
                // FORMAT THE TIME
                GetDateFormat( LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, &stRecieved, NULL, szBuffer, 32 );
                sDate  = StrTableLookUpMiscString( IDSTABLE_QUARANTINED_STRING );    // _T("Quarantined:");
                sDate += " ";
                sDate += szBuffer;
                sDate += " ";
                GetTimeFormat( LOCALE_SYSTEM_DEFAULT, 0, &stRecieved, NULL, szBuffer, 32 );
                sDate += szBuffer;

				TIME_ZONE_INFORMATION TzInfo;
				WCHAR szWTzName[MAX_PATH];
				char *lpszTzNameString;
				//GET THE TIME ZONE STRING
                 memset( szWTzName, 0 ,sizeof(szWTzName) );
                 memset( &TzInfo, 0 ,sizeof(TIME_ZONE_INFORMATION) );
                 DWORD  dwRet = GetTimeZoneInformation( &TzInfo );
                 if( dwRet == TIME_ZONE_ID_STANDARD)
                     lpszTzNameString = _tzname[0];            // STANDARD TIME
                 else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
                     lpszTzNameString = _tzname[1];            // DAYLIGHT SAVINGS TIME
                 
                 // CONVERT IT
                 if( lpszTzNameString != NULL )
                 {
                     BOOL bRet = MultiByteToWideChar(CP_ACP,0,lpszTzNameString,-1,szWTzName,sizeof(szWTzName));   
                     if( !bRet )
                         szWTzName[0] = 0;
                 }
                 sDate += " ";
                 sDate += szWTzName;

                sAge += _T("   ") + sDate;
                return(TRUE);   
            }
        }
    }
    catch(...) 
    {
#ifdef _DEBUG
        fWidePrintString("FAILURE: GetSampleAge Exception.");
#endif
    }
    return(FALSE);
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CFileNameParse::GetFileName
//
// Description   : This routine will return the file name for a fully qualified path
//
// Return type   : UINT 
//
// Argument      : LPCTSTR lpszPathName - full path to parse
// Argument      : LPTSTR lpszTitle     - destination buffer
// Argument      : UINT nMax            - destination buffer size.
//
///////////////////////////////////////////////////////////////////////////////
// 2/1/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
UINT GetFileNameFromPath(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax)
{
	//ASSERT(lpszTitle == NULL ||
	//	AfxIsValidAddress(lpszTitle, _MAX_FNAME));
	//ASSERT(AfxIsValidString(lpszPathName));

    if( lpszPathName == NULL )
        return( 0 );
	if (lpszTitle != NULL)
        *lpszTitle = 0;

	// always capture the complete file name including extension (if present)
	LPTSTR lpszTemp = (LPTSTR)lpszPathName;
	for (LPCTSTR lpsz = lpszPathName; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// remember last directory/drive separator
		if (*lpsz == '\\' || *lpsz == '/' || *lpsz == ':')
			lpszTemp = (LPTSTR)_tcsinc(lpsz);
	}

	// lpszTitle can be NULL which just returns the number of bytes
	if (lpszTitle == NULL)
		return lstrlen(lpszTemp)+1;

	// otherwise copy it into the buffer provided
	lstrcpyn(lpszTitle, lpszTemp, nMax);
	return 0;
}



/*----------------------------------------------------------------------------
    DetectSampleError()
    Assume that caller has determined that REGVALUE_ALERT_SAMPLE_INTERVENTION
    is enabled
    Asume that the interval to check for has been read.


To determine the "sample intervention" alert condition, the Quarantine
Service must examine each sample in quarantine like this:

   If X-Sample-Status has one of the values listed below, the sample
   requires manual intervention.

   If "alertSampleIntervention" is enabled, sample alerts should be
   included in alert messages.

   If the X-Alert-Status attribute does not exist or its value does not
   match the X-Sample-Status attribute, the sample alert condition has
   changed.

   If the condition has not changed, the age of the alert can be calculated
   from the X-Date-Alert attribute.

   If the condition has changed, the value of X-Sample-Status should be
   stored in X-Alert-Status and the current date and time should be stored
   in X-Date-Alert for use in the next interval.

   The value of X-Sample-Status should be mapped to a national-language
   message using the new IcePackStatusTable in the IcePackTokens.CPP file,
   and the national-language message should be appended to the message
   body, along with some identification for the sample.

   If the condition has not changed, the age of the alert should be
   included in the message body.

   If the condition has changed, the alert should be flagged.

The values of X-Sample-Status that indicate a "sample intervention" alert
condition are:

   held
   available
   attention
   error
   notinstalled


The values of X-Sample-Status that indicate a sample "too long in state" error
condition are:
    quarantined
    submitted
    released
    needed
    available
    distributed
    distribute




    All times handled in GMT time.

    STATUS_QUARANTINED     0        
    STATUS_SUBMITTED       1
    STATUS_HELD            2
    STATUS_RELEASED        3
    STATUS_UNNEEDED        4
    STATUS_NEEDED          5
    STATUS_AVAILIABLE      6
    STATUS_DISTRIBUTED     7                 
    STATUS_INSTALLED       8
    STATUS_ATTENTION       9
    STATUS_ERROR           10 
    STATUS_NOTINSTALLED    11               // new in Jedi release
    STATUS_RESTART         12               // new in Jedi release
    STATUS_LEGACY          13               // new in Jedi release
    STATUS_DISTRIBUTE      14               // new in Jedi release

d:\Proj\Norton_AntiVirus\QuarantineServer\shared\BinIntelUnicodeWin32Debug\qserver.exe

    Written by: tmarles   
 ----------------------------------------------------------------------------*/
BOOL CEventDetect::TestSampleError( IQuarantineServerItem* pItem, CAlertEvent *pNewEvent, DWORD dwModuleData )
{
    int  rc;
    CString sTemp           = _T("");
    CString sSampleStatus   = _T("");
    CString sSampleStatusID = _T("");
    CString sAlertStatus    = _T("");
    CString sError          = _T("");
    CString sCurrentStatus  = _T("");
    CString sStatusTimer     = _T("");
    time_t  tStatusTimer     = 0;
    TCHAR*  endptr=NULL;
    time_t  tLastGmtTime = 0;
    time_t  tCurrentGmtTime = 0;
//    TCHAR   szBuff[512];


	// Check params 
	if (pNewEvent == NULL || pItem == NULL)
		return FALSE;

    try
    {
        // SAVE THE MODULE PTR
        m_pServiceModule = (CServiceModule*) dwModuleData;

        // INITIALIZE
        pNewEvent->ClearEventData();


        // Get X-Sample-Status This is a DWORD stored as a string.
        rc = GetSampleAttribute( pItem, QSITEMINFO_X_SAMPLE_STATUS, sSampleStatusID, VT_BSTR );
        // Pass the Dword as a string. sSampleStatus then contains the token
        GetStatusToken(sSampleStatus, sSampleStatusID );  

        // GET THE STATUS TIMER
        GetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sStatusTimer, VT_BSTR );
        tStatusTimer = STRTOUL( (LPCTSTR) sStatusTimer, &endptr, 10 );

        // Get X-Alert-Status This is a string.
        rc = GetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sAlertStatus, VT_BSTR );

        // IF IT'S "error", THEN LOOK UP THE SPECIFIC ERROR 
        sCurrentStatus = sSampleStatus;
	
         if( sSampleStatus == TOKEN_ERROR )     //_T("error")
        {
            // Get X-Error value. This is a string.
            rc = GetSampleAttribute( pItem, QSITEMINFO_X_ERROR, sError, VT_BSTR );
            if( !sError.IsEmpty() )
                sCurrentStatus = sError;
        }
        else if( sSampleStatus == TOKEN_ATTENTION )   // _T("attention")
        {
            // Get X-Attention value. This is a string.
            rc = GetSampleAttribute( pItem, QSITEMINFO_X_ATTENTION, sError, VT_BSTR );
        }

        sSampleStatus.TrimLeft(); 
        sSampleStatus.TrimRight();
          

        // DEBUG jhill 2/18/00
        CString sFileName;
        GetSampleAttribute( pItem, QSITEMINFO_X_SAMPLE_FILE, sFileName, VT_BSTR );
        TCHAR szFileName[512];
        szFileName[0] = 0;
        GetFileNameFromPath( sFileName, szFileName, sizeof(szFileName) );
        // fWidePrintString("NOTE Sample: DetectSampleError evaluate  File= %s", szFileName );

        // Get X-Alert-Status-Last This is a string.
        //rc = GetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_LAST, sAlertStatusLast, VT_BSTR );

        // CHECK FOR TIME IN STATE ERRORS. INCLUDES ONLY ONES WITH TIMERS
        sTemp = SAMPLE_ALERT_STATUS_TIMER_TOKEN_LIST;   
        // if( sTemp.Find( sSampleStatus ) >= 0 && pNewEvent!= 0 )
        if( FindStringInCommaDelimitedList( sTemp, sSampleStatus ) >= 0  && pNewEvent!= 0 )  
        {
            // GET EVENT DATA
            InitializeEmptyEventStruct( pNewEvent, EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE, sSampleStatus, NULL );  // sSampleStatus
            AVIS_EVENT_RULES* lpRule = pNewEvent->lpEventRuleEntry;
            // ARE WE ENABLED?
            if( lpRule == NULL || !lpRule->dwEnabled )
            {
                // CLEAR THE TIMER?
                if( lpRule != NULL && !lpRule->dwEnabled )  // ARE WE ENABLED?
                {
                    sTemp = _T("");
                    SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR );
                }
                return(FALSE);                     // NO CHANGE
            }

            // CHECK FOR TIME IN STATE ERRORS
            if( sCurrentStatus != sAlertStatus )                        // if( sCurrentStatus == sAlertStatus )
            {
                // ARE WE RUNNING A TIMER?
                if( lpRule->dwMinTimeInStateSecs > 0 && tStatusTimer == 0 )
                {
                    // START A NEW TIMER
                    sTemp.Format(_T("%d"), (tCurrentGmtTime + lpRule->dwMinTimeInStateSecs) );
                    SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR ); 
#ifdef _DEBUG
                    fWidePrintString("NOTE Sample: DetectSampleError  Timer started on  [%s] Secs remaining = %d  File= %s", pNewEvent->sIcePackTokenString, lpRule->dwMinTimeInStateSecs, szFileName );
#endif
                }
                else  
                {
                    // TIMER EXPIRED?
                    if( lpRule->dwMinTimeInStateSecs == 0 || tStatusTimer <= tCurrentGmtTime )
                    {
#ifdef _DEBUG
                        fWidePrintString("NOTE Sample: DetectSampleError  Timer stopped on  [%s]  File= %s", pNewEvent->sIcePackTokenString, szFileName );
#endif
                        FillSampleAlertMsgData( pItem, pNewEvent );
                        AddNewEventToEventQueue( pNewEvent, NULL );  // CObList *pEventList
                        // SAVE THE CURRENT STATUS AS X-Alert-Status
                        SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sSampleStatus, VT_BSTR );  
                        // SAVE THE CURRENT DATE/TIME AS X-Date-Alert
                        SetSampleAttributeDate( pItem, QSITEMINFO_X_DATE_ALERT, tCurrentGmtTime );
                        // CLEAR THE TIMER
                        sTemp = _T("");
                        SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR );
                        return(TRUE);
                    }

#ifdef  MY_SYSTEM
                    int iTimeRemaining = abs( int (tStatusTimer - tCurrentGmtTime) );
                    fWidePrintString("NOTE Sample: DetectSampleError  Timer still running on [%s] Secs remaining = %d  File= %s", pNewEvent->sIcePackTokenString, iTimeRemaining, szFileName );
#endif
                }
                return(FALSE);              // NO CHANGE 

            }
            else if( tStatusTimer != 0 )    // CHANGE IN STATUS  
            {
                sTemp = _T("");             // CLEAR THE TIMER
                SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR ); 
#ifdef _DEBUG
                fWidePrintString("NOTE Sample: DetectSampleError  Timer cleared for [%s]  File= %s", pNewEvent->sIcePackTokenString, szFileName );
#endif
            }

            // NOW FALL THROUGH TO LOOK FOR SAMPLE OR QUARANTINE ERROR CONDITIONS
        }
        else if( tStatusTimer != 0 )    // CHANGE IN STATUS  
        {
            sTemp = _T("");             // CLEAR THE TIMER
            SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS_TIMER, sTemp, VT_BSTR ); 
#ifdef _DEBUG
            fWidePrintString("NOTE Sample: DetectSampleError  Timer cleared for File= %s", szFileName );
#endif
        }


        // IS THIS NEW? 
        if( sCurrentStatus != sAlertStatus )
        {
            // IS THIS A SAMPLE ERROR CONDITION?
            sTemp = SAMPLE_ALERT_TOKEN_LIST;
            //if( sTemp.Find( sSampleStatus ) >= 0 && pNewEvent!= 0 )
            if( FindStringInCommaDelimitedList( sTemp, sSampleStatus ) >= 0  && pNewEvent!= 0 )  
            {
                // GET EVENT DATA
                rc = InitializeEmptyEventStruct( pNewEvent, EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE, sCurrentStatus, NULL );  
            
                // IS THIS AN ATTENTION ERROR? If so, X-Attention contains the actual string.
                if( sSampleStatus == TOKEN_ATTENTION )   // _T("attention")
                {
                    pNewEvent->sErrorText = sError;
                }

                // SHOULD WE REPORT IT?
                if( IsEnabledAndTimeInStateOk( pNewEvent ) )    
                {
                    FillSampleAlertMsgData( pItem, pNewEvent );
                    AddNewEventToEventQueue( pNewEvent, NULL );  // CObList *pEventList
            
                    // SAVE THE CURRENT STATUS AS X-Alert-Status
                    rc = SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sCurrentStatus, VT_BSTR );  
                
                    // SAVE THE CURRENT DATE/TIME AS X-Date-Alert
                    rc = SetSampleAttributeDate( pItem, QSITEMINFO_X_DATE_ALERT, tCurrentGmtTime );
                    return(TRUE);
                }
                return(FALSE);
            }
        }
        

        // IS THIS A NEW LOCAL QUARANTINE ERROR? 
        if( sCurrentStatus != sAlertStatus )
        {
#ifdef _DEBUG
            fWidePrintString("NOTE: CEventDetect sCurrentStatus=%s  sAlertStatus=%s", sCurrentStatus, sAlertStatus );
#endif
            // A local quarantine alert is needed in these situations because the user and/or
            // administrator must take some action to remove the sample from local
            // quarantine on the client machine:
            // 
            //    When X-Sample-Status is 'unneeded' and X-Scan-Result is 'uninfected',
            //    definitions are not installed on the client, no repair of the
            //    quarantined file is attempted, and the file will remain in local
            //    quarantine until the administrator and/or user take some action.  The
            //    action should be to restore the file to its original directory.
            // 
            //    When X-Sample-Status is 'installed' and X-Scan-Result is 'unrepairable',
            //    definitions are installed on the client, but the attempted repair of the
            //    quarantined file is unsuccessful, so the file will remain in local
            //    quarantine until the administrator and/or user take some action.  The
            //    action should probably be to delete the file.

            // IS THIS A LOCAL QUARANTINE ERROR CONDITION?
            sTemp = QUARANTINE_ALERT_STATUS_TOKENS;
            // if( sTemp.Find(sSampleStatus) >= 0 )             // is it in the list
            if( FindStringInCommaDelimitedList( sTemp, sSampleStatus ) >= 0  && pNewEvent!= 0 )  
            {
                // Get X-Scan-Result This is a string.
                CString sResult = _T("");
                GetSampleAttribute( pItem, QSITEMINFO_X_SCAN_RESULT, sResult, VT_BSTR );
        
                sTemp = QUARANTINE_ALERT_RESULT_TOKENS;
                // if( sTemp.Find(sResult) == -1 )        // X-Scan-Result != "repaired" 
                if( FindStringInCommaDelimitedList( sTemp, sResult ) == -1 )  
                {
                    // Get X-Alert-Result. This is a string.
                    CString sAlertResult = _T("");
                    GetSampleAttribute( pItem, QSITEMINFO_X_ALERT_RESULT, sAlertResult, VT_BSTR );
                    
                    //fWidePrintString("NOTE: CEventDetect sResult=%s  sAlertResult=%s", sResult, sAlertResult );
        
                    // IS THIS NEW? 
                    if( sResult != sAlertResult )
                    {
                        // GET EVENT DATA
                        InitializeEventByEventId( pNewEvent, QUARANTINE_ALERT_NOT_REPAIRED_EVENTID );
                        // SHOULD WE REPORT IT?
                        if( IsEnabledAndTimeInStateOk( pNewEvent ) )    
                        {
                            FillSampleAlertMsgData( pItem, pNewEvent );
                            AddNewEventToEventQueue( pNewEvent, NULL );
            
                            // SAVE THE CURRENT STATUS AS X-zAlert-Result
                            sTemp = TOKEN_NOTREPAIRED;
                            rc = SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_RESULT, sTemp, VT_BSTR );  
                        
                            // SAVE THE CURRENT STATUS AS X-Alert-Status
                            SetSampleAttribute( pItem, QSITEMINFO_X_ALERT_STATUS, sSampleStatus, VT_BSTR );  
        
                            // SAVE THE CURRENT DATE/TIME AS X-Date-Alert
                            rc = SetSampleAttributeDate( pItem, QSITEMINFO_X_DATE_ALERT, tCurrentGmtTime );
                            return(TRUE);
                        }
                    }
                }
            }
        }


    }
    catch(...) 
    {
#ifdef _DEBUG
        fWidePrintString("FAILURE: CEventDetect::DetectSampleError Exception.");
#endif
    }

    return(FALSE);
}
