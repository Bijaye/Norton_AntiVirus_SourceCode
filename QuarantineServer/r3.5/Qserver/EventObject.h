// EventObject.h

#ifndef _AVISEVENTOBJECT_H_INCLUDED
#define	_AVISEVENTOBJECT_H_INCLUDED


#include  "util.h"
#include "Qserver.h"
#include "IcePackTokens.h"


// TESTING 1/28/00
//#define  DISABLE_EVENT_DETECTION  1
// TEST
typedef struct tagTEST_QSERVERITEM
{
   DWORD  dw;
   TCHAR* lpszSampleStatus;
   TCHAR* lpszSampleResult;

}  AVIS_QSERVERITEM;





class CEvent : public CObject
{
public:
	CEvent () 
		{
			memset (&m_AvisAlertEvent, 0,sizeof (m_AvisAlertEvent));
			m_pQSItem = NULL;
		};
	CEvent (CAlertEvent *pEvent)
		{
			m_AvisAlertEvent.sMessage = pEvent->sMessage;
			m_AvisAlertEvent.sIcePackTokenString = pEvent->sIcePackTokenString;
			m_AvisAlertEvent.dwEventGroupType = pEvent->dwEventGroupType ;
			m_AvisAlertEvent.dwEventId = pEvent->dwEventId ;
			m_AvisAlertEvent.dwAlertSeverityLevel = pEvent->dwAlertSeverityLevel;
			m_AvisAlertEvent.dwOperationalStatusLevel = pEvent->dwOperationalStatusLevel;
			memcpy (&m_AvisAlertEvent.EventTime, &pEvent->EventTime,sizeof (m_AvisAlertEvent.EventTime));
			m_AvisAlertEvent.dwIcePackTokenTableID = pEvent->dwIcePackTokenTableID;
			m_pQSItem = NULL;

            // JHILL added 2/12/00
            m_AvisAlertEvent.sEventName        = pEvent->sEventName;
            m_AvisAlertEvent.sMachineAddress   = pEvent->sMachineAddress + _T("  ");
            m_AvisAlertEvent.sMachineAddress  += pEvent->sDNSname;
            m_AvisAlertEvent.sMachineName      = pEvent->sMachineName;
            m_AvisAlertEvent.sVirusName        = pEvent->sVirusName;
            m_AvisAlertEvent.sFileName         = pEvent->sFileName;
            m_AvisAlertEvent.sUser             = pEvent->sUser;
            m_AvisAlertEvent.sDefaultOrHelpMsg = pEvent->sDefaultOrHelpMsg;
            m_AvisAlertEvent.sPlatformName     = pEvent->sPlatformName;
            m_AvisAlertEvent.lpEventRuleEntry  = pEvent->lpEventRuleEntry;
            m_AvisAlertEvent.sSampleStatus     = pEvent->sSampleStatus;
            m_AvisAlertEvent.sAgeOfSample      = pEvent->sAgeOfSample;
            m_AvisAlertEvent.m_sSampleState    = pEvent->m_sSampleState;
            m_AvisAlertEvent.m_sNote           = pEvent->m_sNote;
            m_AvisAlertEvent.sDefinitions	   = pEvent->sDefinitions;

		}
	~CEvent () 
		{
			if(m_pQSItem)
			{
				m_pQSItem->Release();
				m_pQSItem=NULL;
			}
		};
	IQuarantineServerItem*		m_pQSItem;
	CAlertEvent	m_AvisAlertEvent;
private:
};


class CEventDetect  
{
public:
	CEventDetect();
	virtual ~CEventDetect();

    // DATA
    CServiceModule* m_pServiceModule;


    // PUBLIC 
    BOOL DetectGeneralErrors(CObList *pEventList, DWORD dwModuleData);
//    BOOL IsSampleErrorCondition( AVIS_QSERVERITEM* pItem );
	BOOL TestGeneralErrors(CObList *pEventList, DWORD dwModuleData);
    BOOL DetectSampleError( IQuarantineServerItem* pItem ,CAlertEvent *pNewEvent, DWORD dwModuleData);
	BOOL TestSampleError( IQuarantineServerItem* pItem, CAlertEvent *pNewEvent, DWORD dwModuleData );

    BOOL DetectNewDefinitionArrival(CObList *pEventList);
	BOOL TestNewDefinitionArrival(CObList *pEventList);
    BOOL DetectNewUnblessedDefArrival(CObList *pEventList);
    BOOL TestNewUnblessedDefArrival(CObList *pEventList);
	HRESULT ProccesEventQueue(CObList *pEventList);

    // LOCAL STUFF private
    BOOL InitializeEventByEventId( CAlertEvent* lpstNewEvent, DWORD dwEventId );
    BOOL InitializeEmptyEventStruct( CAlertEvent* lpstNewEvent, DWORD dwIcePackTokenTableID, LPCTSTR lpszToken, LPCTSTR lpszParentToken );
    BOOL SetRegistryValue( LPCTSTR pszRegKey, LPCTSTR pszRegValueName, CString &sValue ); 
    BOOL GetRegistryValue( LPCTSTR pszRegKey, LPCTSTR pszRegValueName, CString &sValue, LPCTSTR pDefaultValue = NULL  ); 
    BOOL AddNewEventToEventQueue( CAlertEvent * lpstNewEvent, CObList *pEventList);
	BOOL AddNewTestEventToEventQueue( CAlertEvent * lpstNewEvent, CObList *pEventList );

    BOOL LogEventToNTEventLog( CAlertEvent * lpstNewEvent );
    BOOL BuildMessageTitle( CString &sMessageTitle, CAlertEvent * lpstNewEvent );
    BOOL BuildAlertMessage( CAlertEvent * lpstNewEvent );
    BOOL FillGeneralAlertMsgData( CAlertEvent * lpstNewEvent );
    BOOL SendTestEvent( CObList *pEventList );
    BOOL CheckDiskSpace( CObList *pEventList );
	BOOL TestCheckDiskSpace( CObList *pEventList );

    // CONFIG DATA
    BOOL ReadAllConfigDataFromRegistry();
    BOOL ReadEventConfigData( AVIS_EVENT_RULES* lpTableEntry );
    BOOL GetNextNumbersFromCommaString( CString& s, DWORD* pNum1, DWORD* pNum2 );
    BOOL GetNextTokensFromCommaString(CString& s, CString& sToken1, CString& sToken2 );

    BOOL IsEnabledAndTimeInStateOk( CAlertEvent * lpstNewEvent );
    BOOL RemoveTokenFromGeneralAttentionString( CAlertEvent * lpstNewEvent, CString &sNewPreviousValue );

    // TIME IN STATE STUFF
    BOOL  ClearTimeInState( CAlertEvent * lpstNewEvent );

};


// CLEAR TIME IN STATE
BOOL ClearGeneralAttentionTimer( CString sTokens );
time_t TimeToGmtTime( time_t tTime );
BOOL StrTableLoadString( DWORD dwGlobalResID, CString& s );
//int FindStringInCommaDelimitedList( CString& sStringList, CString& sSubString );
int FindStringInCommaDelimitedList( CString& sStringList, LPCTSTR pszSubString );





#endif
