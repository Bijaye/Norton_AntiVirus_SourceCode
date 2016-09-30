/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/





#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ENABLE/DISABLE LINKING WITH HTMLHELP.LIB
#define  CALL_HTML_HELP     

#define  AVIS_HELPFILE_NAME      _T("AvisConsole.chm")   //  _T("QSCon.chm")    // _T("ACHelp.chm")
#define  AVIS_CONTEXT_FILENAME   _T("ACHelp.txt")        //  _T("QSCon.txt")    // _T("ACHelp.txt")

typedef struct tagavis_topic_filenames{
    DWORD    dwTopicNumber;          
    LPTSTR   lpszTopicFilename;
} AVIS_TOPIC_FILENAMES_STRUCT;

// AVIS TOPIC FILE NAMES. THESE SHOULD NOT BE TRANSLATED
AVIS_TOPIC_FILENAMES_STRUCT g_AvisTopicFilenamesArray[];
AVIS_TOPIC_FILENAMES_STRUCT g_AvisSampleAttributeNamesArray[];


#define  IDH_TOPIC_FILENAME_WEB_COMMUNICATION_PROPERTIES_HTM     9351
#define  IDH_TOPIC_FILENAME_CUSTOMER_INFO_PROPERTIES_HTM         9352
#define  IDH_TOPIC_FILENAME_SAMPLE_POLICY_PROPERTIES_HTM         9353
#define  IDH_TOPIC_FILENAME_SAMPLE_ATTRIBUTE_PROPERTIES_HTM      9354
#define  IDH_TOPIC_FILENAME_SAMPLE_ACTIONS_PROPERTIES_HTM        9355
#define  IDH_TOPIC_FILENAME_CONFIRM_SUBMISSION_HTM               9356
#define  IDH_TOPIC_FILENAME_CONFIRM_DELIVERY_HTM                 9357
#define  IDH_TOPIC_FILENAME_FIREWALL_PROPERTIES_HTM              9358
#define  IDH_TOPIC_FILENAME_DEFINITION_POLICY_PROPERTIES_HTM     9359
#define  IDH_TOPIC_FILENAME_INSTALL_DEFINITIONS_PROPERTIES_HTM   9360
#define  IDH_TOPIC_FILENAME_ATTENTION_PROPERTIES                 9361
#define  IDH_TOPIC_FILENAME_SAMPLE_ERRORS_PROPERTIES             9362
#define  IDH_TOPIC_FILENAME_ALERTING_CONFIGURATION_PROPERTIES    9363

//#define  TOPIC_FILENAME_






BOOL CallHtmlHelpTopic( HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD dwData );
BOOL CallHtmlHelpContext( HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD dwData );
int ShowHelpMessageText(LPCTSTR lpszMsgText);












#ifdef __cplusplus
}
#endif // __cplusplus

