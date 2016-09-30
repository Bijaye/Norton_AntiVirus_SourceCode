// ParseEventData.h         PARSEEVENTDATA

#if !defined(AFX_PARSEEVENTDATA_H__941A50E5_F75D_11D2_9795_00C04F788464__INCLUDED_)
#define AFX_PARSEEVENTDATA_H__941A50E5_F75D_11D2_9795_00C04F788464__INCLUDED_


#include "AvisEventDetection.h"  // in QuarantineServer\Include


AVIS_EVENT_RULES* LookupEventRule( AVIS_EVENT_RULES* lpTable, LPCTSTR lpszToken );
//BOOL GetEventDefaultMsg( CString& sToken, CString& sText, DWORD dwEventRulesTableID );
DWORD GetEventNameAndDefaultMsg( CString& sToken, CString& sText, CString& sEventName, CString& sNote, DWORD dwEventRulesTableID );
BOOL IsLocalQuarantineSampleAlert( CString& sStatusToken, int* piStatusID, CString& sResultToken, CString& sAnalysisState );
BOOL GetStatusToken(CString& sStatusToken, CString& sStatusID );
//int FindStringInCommaDelimitedList( CString& sStringList, CString& sSubString );
int FindStringInCommaDelimitedList( CString& sStringList, LPCTSTR pszSubString );
BOOL IsSampleTimeInStateAlert( CString& sSampleStatus, CString& sLastAlertStatus );












#endif