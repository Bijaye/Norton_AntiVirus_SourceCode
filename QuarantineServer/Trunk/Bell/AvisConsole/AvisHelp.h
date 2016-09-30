#if !defined(_AVISHELP_H_INCLUDED)
#define  _AVISHELP_H_INCLUDED

////#include "AvisHelpTopicID.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AvisHelp.h : header file
//





// typedef struct taghelp_ID_struct{
//     DWORD    dwContrlID;
//     DWORD    dwTopicNumber;          // dwTopicID
//     LPCTSTR  lpszTopicName;
//     LPCTSTR  lpszShortHelpText;
// } HELP_ID_STRUCT;

// STRUCT FOR STATIC ARRAYS OF HELP IDS
typedef struct tagavis_help_IDs{
    DWORD    dwContrlID;
    DWORD    dwTopicID;
    DWORD    dwTopicNameStringID;
} AVIS_HELP_ID_STRUCT;




// RETURNS FULL PATH AND NAME OF THE HELPFILE
TCHAR *GetHelpfileName();

// Handles the help button on the property sheets       this
BOOL DoHelpPropSheetHelpButton( DWORD dwControlID, AVIS_HELP_ID_STRUCT *lpHelpIdArray,
                                NMHDR* pNMHDR );

// Handles both F1 and "WHAT IS" on the property sheets.    
BOOL DoHelpPropSheetF1( DWORD dwDefaultID, AVIS_HELP_ID_STRUCT *lpHelpIdArray, 
                        HELPINFO* pHelpInfo );

// Handles Prop Sheet Right Mouse Click. Displays a "What's This?" menu
BOOL DoHelpPropSheetContextHelp( DWORD dwControlID, AVIS_HELP_ID_STRUCT *lpHelpIdArray, CWnd* pWnd, HWND hTargetWnd = NULL );
BOOL DoHelpDisplayContextMenuWhatsThis( DWORD dwDefaultID, DWORD *lpdwResID, 
                                        AVIS_HELP_ID_STRUCT *lpHelpIdArray, HWND hPropPageWnd, 
                                        CWnd* pWnd, CPoint point, DWORD dwForcedDefault = 0, DWORD* pdwTargetWnd = NULL );

// GET CONTEXT HELP FROM THE ATTRIBUTE NAME
BOOL DoHelpAttributeNameContextHelp( DWORD dwControlID, TCHAR* lpszAttributeName, CWnd* pWnd, HWND hTargetWnd ); 




//AVIS_HELP_ID_STRUCT g_AvisServerConfigHelpIdArray[]; 
//AVIS_HELP_ID_STRUCT g_AvisServerConfig2HelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_WebCommunicationHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_SamplePolicyHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_CustomerInfoHelpIdArray[];
AVIS_HELP_ID_STRUCT g_SampleAttributesHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_SampleActionsHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_ConfirmationDialogSubmitHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_ConfirmationDialogDeliveryHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_FirewallConfigHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_DefinitionPolicyHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_InstallDefinitionsHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_AtttentionPageHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_SelectTargetDialogHelpIdArray[];
AVIS_HELP_ID_STRUCT g_SampleErrorPageHelpIdArray[]; 
AVIS_HELP_ID_STRUCT g_AlertingSetConfigPageHelpIdArray[]; 










#endif // !defined(_AVISHELP_H_INCLUDED)
