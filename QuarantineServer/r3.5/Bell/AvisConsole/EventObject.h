// EventObject.h


#ifndef _AVISEVENTOBJECT_H_INCLUDED
#define	_AVISEVENTOBJECT_H_INCLUDED




// TEST
typedef struct tagTEST_QSERVERITEM
{
   DWORD  dw;
   TCHAR* lpszSampleStatus;
   TCHAR* lpszSampleResult;

}  AVIS_QSERVERITEM;




class CEventDetect  
{
public:
	CEventDetect();
	virtual ~CEventDetect();

    // PUBLIC
    BOOL DetectGeneralErrors();  
    // static BOOL IsSampleErrorCondition( AVIS_QSERVERITEM* pItem );
    BOOL DetectSampleError( AVIS_QSERVERITEM* pItem, CAlertEvent *pNewEvent ); 
    BOOL DetectNewDefinitionArrival();    
    BOOL DetectNewUnblessedDefArrival();                                                       
//  BOOL CheckDiskSpace();


    // LOCAL STUFF private
    static BOOL TestDetectAllEvents();       // test
    static BOOL TestDetectSampleErrors( );
    BOOL TestSampleErrorDection( void * pQServerGetAllAttributes );

    BOOL InitializeEventByEventId( CAlertEvent* lpstNewEvent, DWORD dwEventId );
    BOOL InitializeEmptyEventStruct( CAlertEvent* lpstNewEvent, DWORD dwIcePackTokenTableID, LPCTSTR lpszToken, LPCTSTR lpszParentToken );
    BOOL SetRegistryValue( LPCTSTR pszRegKey, LPCTSTR pszRegValueName, CString &sValue );  
    BOOL GetRegistryValue( LPCTSTR pszRegKey, LPCTSTR pszRegValueName, CString &sValue, LPCTSTR pDefaultValue = NULL ); 
    BOOL AddNewEventToEventQueue( CAlertEvent * lpstNewEvent );               
    //BOOL LogEventOnly( CAlertEvent * lpstNewEvent );                          
    BOOL BuildMessageTitle( CString &sMessageTitle, CAlertEvent * lpstNewEvent );
    BOOL BuildAlertMessage( CAlertEvent * lpstNewEvent );
    BOOL FillGeneralAlertMsgData( CAlertEvent * lpstNewEvent );

    // CONFIG DATA
    BOOL ReadAllConfigDataFromRegistry();
    BOOL ReadEventConfigData( AVIS_EVENT_RULES* lpTableEntry );
    BOOL GetNextNumbersFromCommaString( CString& s, DWORD* pNum1, DWORD* pNum2 );
    BOOL GetNextTokensFromCommaString(CString& s, CString& sToken1, CString& sToken2 );

    BOOL IsEnabledAndTimeInStateOk( CAlertEvent * lpstNewEvent );
    BOOL RemoveTokenFromGeneralAttentionString( CAlertEvent * lpstNewEvent, CString &sNewPreviousValue );

    // TIME IN STATE STUFF
    BOOL ClearTimeInState( CAlertEvent * lpstNewEvent );


    // BOOL  DetectSampleTimeInStateEvents(  AVIS_QSERVERITEM* pItem, CAlertEvent *pNewEvent );
    // DWORD ReadTimerFromTimeInStateString( CString& s, DWORD dwEventID );
    // BOOL  RemoveTimerFromTimeInStateString( CString& s, DWORD dwEventID );
    // BOOL  AddTimerToTimeInStateString( CString& s, DWORD dwEventID, DWORD dwTimer );
    // BOOL  GetNextExpiredTimerFromTimeInStateString( CString& s, CString& sEventTimer, DWORD* pdwEventID, DWORD* pdwTimer );

};


// CLEAR TIME IN STATE
BOOL ClearGeneralAttentionTimer( CString sTokens );
time_t TimeToGmtTime( time_t tTime );



#endif
