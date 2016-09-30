/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#include "stdafx.h"	 
#include "resource.h"
#include "resource.hm"
#include "AvisHelp.h"
#include "ACHelp.h"
#include "htmlcall.h"
#include "AvisEventDetection.h"   // in QuarantineServer\Include 


// AVIS TOPIC FILE NAMES. THESE SHOULD NOT BE TRANSLATED    Redone 12/18/99
AVIS_TOPIC_FILENAMES_STRUCT g_AvisTopicFilenamesArray[] =
{
    IDH_TOPIC_FILENAME_WEB_COMMUNICATION_PROPERTIES_HTM   , _T("Web_Communication_Properties.htm")  ,
    IDH_TOPIC_FILENAME_CUSTOMER_INFO_PROPERTIES_HTM       , _T("Customer_Info_Properties.htm")  ,
    IDH_TOPIC_FILENAME_SAMPLE_POLICY_PROPERTIES_HTM       , _T("Sample_Policy_Properties.htm")  ,
    IDH_TOPIC_FILENAME_SAMPLE_ATTRIBUTE_PROPERTIES_HTM    , _T("Sample_Attribute_Properties.htm")  ,
    IDH_TOPIC_FILENAME_SAMPLE_ACTIONS_PROPERTIES_HTM      , _T("Sample_Actions_Properties.htm")  ,
    IDH_TOPIC_FILENAME_CONFIRM_SUBMISSION_HTM             , _T("Confirm_Submission.htm")  ,
    IDH_TOPIC_FILENAME_CONFIRM_DELIVERY_HTM               , _T("Confirm_Delivery.htm")  ,
    IDH_TOPIC_FILENAME_FIREWALL_PROPERTIES_HTM            , _T("Firewall_Properties.htm") ,
    IDH_TOPIC_FILENAME_DEFINITION_POLICY_PROPERTIES_HTM   , _T("Definition_Policy_Properties.htm")  ,
    IDH_TOPIC_FILENAME_INSTALL_DEFINITIONS_PROPERTIES_HTM , _T("Install_Definitions_Properties.htm")  ,
    IDH_TOPIC_FILENAME_ATTENTION_PROPERTIES               , _T("Attention_Properties.htm") , 
    IDH_TOPIC_FILENAME_SAMPLE_ERRORS_PROPERTIES           , _T("Sample_Error_Properties.htm") ,
    IDH_TOPIC_FILENAME_ALERTING_CONFIGURATION_PROPERTIES  , _T("Alerting_Configuration_Properties.htm") , 
    //         , _T("")  ,

    0xFFFF, NULL,
    0, 0 
};

        

// AlertingSetConfig.cpp  HELP MAP "Alerting"    1/1/00
// PROPERTY PAGE HELPIDs FOR HELP BUTTON, F1, WHAT IS, AND RIGHT BUTTON CLICK ON THE PAGE
AVIS_HELP_ID_STRUCT g_AlertingSetConfigPageHelpIdArray[] =
{ 
    IDD_ALERTING_SET_CONFIG1           , IDH_ALERTING_SET_CONFIG1                  , 0 , 
    IDC_CHECK_ENABLE_ALERTS1           , IDH_TOPIC_CHECK_ENABLE_ALERTS1            , 0 , 
    IDC_CHECK_NT_EVENT_LOG             , IDH_TOPIC_CHECK_NT_EVENT_LOG              , 0 , 
    IDC_EDIT_ALERT_CHECK_INTERVAL1     , IDH_TOPIC_EDIT_ALERT_CHECK_INTERVAL1      , 0 , 
    IDC_EDIT_AMS_SERVER_ADDRESS1       , IDH_TOPIC_EDIT_AMS_SERVER_ADDRESS1        , 0 , 
    IDC_TEST_BUTTON2                   , IDH_TOPIC_TEST_BUTTON2                    , 0 , 
    IDC_EVENT_LIST1                    , IDH_TOPIC_EVENT_LIST1                     , 0 , 
    IDC_CONFIG_BUTTON                  , IDH_TOPIC_AMS_CONFIG_BUTTON               , 0 ,

    // SPINNERS
    IDC_SPIN_ALERT_CHECK_INTERVAL1     , IDH_TOPIC_EDIT_ALERT_CHECK_INTERVAL1      , 0 ,

    // STATIC FIELDS THAT POINT TO THE ABOVE
    IDC_STATIC_ALERT_INTERVAL          , IDH_TOPIC_EDIT_ALERT_CHECK_INTERVAL1      , 0 ,
    IDC_STATIC_AMS_SERVER              , IDH_TOPIC_EDIT_AMS_SERVER_ADDRESS1        , 0 ,
    IDC_STATIC_TEST                    , IDH_TOPIC_TEST_BUTTON2                    , 0 ,
    IDC_STATIC12                       , IDH_TOPIC_EDIT_ALERT_CHECK_INTERVAL1      , 0 ,

    // LOOKUP BASED ON HELPID OF STATIC FIELDS                     
    HIDC_STATIC_ALERT_INTERVAL         , IDH_TOPIC_EDIT_ALERT_CHECK_INTERVAL1      , 0 ,
    HIDC_STATIC_AMS_SERVER             , IDH_TOPIC_EDIT_AMS_SERVER_ADDRESS1        , 0 ,
    HIDC_STATIC_TEST                   , IDH_TOPIC_TEST_BUTTON2                    , 0 , 
    HIDC_STATIC12                      , IDH_TOPIC_EDIT_ALERT_CHECK_INTERVAL1      , 0 ,

    // DEFAULT FOR ID == 0
    // 0, IDD_ALERTING_SET_CONFIG1, IDH_ALERTING_SET_CONFIG1 ,
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_ALERTING_CONFIGURATION_PROPERTIES , 0 ,
    0, 0, 0  
};                                                            



// AvisServerConfig2.cpp  HELP MAP "Web Communication"    Redone 12/18/99
// PROPERTY PAGE HELPIDs FOR HELP BUTTON, F1, WHAT IS, AND RIGHT BUTTON CLICK ON THE PAGE
//AVIS_HELP_ID_STRUCT g_AvisServerConfig2HelpIdArray[] = 
AVIS_HELP_ID_STRUCT g_WebCommunicationHelpIdArray[] = 
{
    // THESE ARE THE ONLY ENTRIES WITH ASSOCATED TEXT
    IDD_AVIS_WEB_COMM_PAGE             , IDH_TOPIC_AVIS_WEB_COMM_PAGE              , 0 ,   //, IDS_TOPIC_AVIS_WEB_COMM_PAGE,
    IDC_COMBO_GATEWAY_ADDRESS          , IDH_TOPIC_GATEWAY_ADDRESS                 , 0 ,   //, IDS_TOPIC_GATEWAY_ADDRESS   ,
    IDC_EDIT_GATEWAY_PORT              , IDH_TOPIC_GATEWAY_PORT                    , 0 ,   //, IDS_TOPIC_GATEWAY_PORT      ,
    IDC_EDIT_SSL_PORT                  , IDH_TOPIC_EDIT_SSL_PORT                   , 0 ,
    IDC_CHECK_SECURE_SAMPLE_SUBMISSION , IDH_TOPIC_CHECK_SECURE_SAMPLE_SUBMISSION  , 0 ,
    IDC_CHECK_SECURE_DEF_DOWNLOAD      , IDH_TOPIC_CHECK_SECURE_DEF_DOWNLOAD       , 0 ,
    IDC_CHECK_SECURE_STATUS_QUERY      , IDH_TOPIC_CHECK_SECURE_STATUS_QUERY       , 0 ,
    IDC_CHECK_SECURE_IGNORE_HOSTNAME   , IDH_TOPIC_CHECK_SECURE_IGNORE_HOSTNAME    , 0 ,
    IDC_EDIT_TRANSACTION_TIMEOUT       , IDH_TOPIC_EDIT_TRANSACTION_TIMEOUT        , 0 ,
    IDC_EDIT_RETRY_INTERVAL            , IDH_TOPIC_EDIT_RETRY_INTERVAL             , 0 ,
    IDC_EDIT_RETRY_LIMIT               , IDH_TOPIC_EDIT_RETRY_LIMIT                , 0 ,

    // SPINNERS
    IDC_SPIN_GATEWAY_PORT              , IDH_TOPIC_GATEWAY_PORT                    , 0 ,   //, IDS_TOPIC_GATEWAY_PORT      ,
    IDC_SPIN_SSL_PORT                  , IDH_TOPIC_EDIT_SSL_PORT                   , 0 ,
    IDC_SPIN_TRANSACTION_TIMEOUT       , IDH_TOPIC_EDIT_TRANSACTION_TIMEOUT        , 0 ,
    IDC_SPIN_RETRY_INTERVAL            , IDH_TOPIC_EDIT_RETRY_INTERVAL             , 0 ,
    IDC_SPIN_RETRY_LIMIT               , IDH_TOPIC_EDIT_RETRY_LIMIT                , 0 ,

    // STATIC FIELDS THAT POINT TO THE ABOVE
    IDC_STATIC_SECURITY                , IDH_TOPIC_CHECK_SECURE_SAMPLE_SUBMISSION  , 0 ,
    IDC_STATIC_GATEWAY                 , IDH_TOPIC_GATEWAY_ADDRESS                 , 0 , 
    IDC_STATIC_GATEWAY_ADDRESS         , IDH_TOPIC_GATEWAY_ADDRESS                 , 0 ,   //, IDS_TOPIC_GATEWAY_ADDRESS ,  
    IDC_STATIC_GATEWAY_PORT            , IDH_TOPIC_GATEWAY_PORT                    , 0 ,   //, IDS_TOPIC_GATEWAY_PORT    , 
    IDC_STATIC_SSL_PORT                , IDH_TOPIC_EDIT_SSL_PORT                   , 0 ,
    IDC_STATIC_TRANSACTION_TIMEOUT     , IDH_TOPIC_EDIT_TRANSACTION_TIMEOUT        , 0 ,
    IDC_STATIC_RETRY_INTERVAL          , IDH_TOPIC_EDIT_RETRY_INTERVAL             , 0 ,
    IDC_STATIC_RETRY_LIMIT             , IDH_TOPIC_EDIT_RETRY_LIMIT                , 0 ,
                                                                   
    // LOOKUP BASED ON HELPID OF STATIC FIELDS                     
    HIDC_STATIC_GATEWAY_ADDRESS        , IDH_TOPIC_GATEWAY_ADDRESS                 , 0 ,   //, IDS_TOPIC_GATEWAY_ADDRESS ,  
    // HIDC_STATIC_GATEWAY_PORT           , IDH_TOPIC_GATEWAY_PORT                    , 0 ,   //, IDS_TOPIC_GATEWAY_PORT    ,  
    // HIDC_STATIC_SSL_PORT               , IDH_TOPIC_EDIT_SSL_PORT                   , 0 ,
    // HIDC_STATIC_TRANSACTION_TIMEOUT    , IDH_TOPIC_EDIT_TRANSACTION_TIMEOUT        , 0 ,
    // HIDC_STATIC_RETRY_INTERVAL         , IDH_TOPIC_EDIT_RETRY_INTERVAL             , 0 ,
    // HIDC_STATIC_RETRY_LIMIT            , IDH_TOPIC_EDIT_RETRY_LIMIT                , 0 ,
                                  
    // DEFAULT FOR ID == 0
    0, IDH_TOPIC_AVIS_WEB_COMM_PAGE, IDS_TOPIC_AVIS_WEB_COMM_PAGE ,
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_WEB_COMMUNICATION_PROPERTIES_HTM , 0 ,
    0, 0, 0  
};                                                           

// CAServerFirewallConfig2   "Firewall"
AVIS_HELP_ID_STRUCT g_FirewallConfigHelpIdArray[] =
{
    IDD_AVIS_FIREWALL_COMM_PAGE          ,  IDH_TOPIC_FIREWALL_COMM_PAGE    , 0 ,
    IDC_EDIT_PROXY_FIREWALL              ,  IDH_TOPIC_PROXY_FIREWALL        , 0 ,
    IDC_EDIT_FIREWALL_PORT               ,  IDH_TOPIC_FIREWALL_PORT         , 0 ,
    IDC_EDIT_FIREWALL_USERNAME           ,  IDH_TOPIC_FIREWALL_USERNAME     , 0 ,
    IDC_EDIT_FIREWALL_PASSWORD           ,  IDH_TOPIC_FIREWALL_PASSWORD     , 0 ,

    // SPINNERS
    IDC_SPIN_FIREWALL_PORT               ,  IDH_TOPIC_FIREWALL_PORT         , 0 ,

    // STATIC FIELDS THAT POINT TO THE ABOVE
    IDC_STATIC_FIREWALL_ADDRESS          ,  IDH_TOPIC_PROXY_FIREWALL        , 0 ,
    IDC_STATIC_FIREWALL_PORT             ,  IDH_TOPIC_FIREWALL_PORT         , 0 ,
    IDC_STATIC_FIREWALL_USERNAME         ,  IDH_TOPIC_FIREWALL_USERNAME     , 0 ,
    IDC_STATIC_FIREWALL_PASSWORD         ,  IDH_TOPIC_FIREWALL_PASSWORD     , 0 ,


    // LOOKUP BASED ON HELPID OF STATIC FIELDS
    HIDC_EDIT_PROXY_FIREWALL             ,  IDH_TOPIC_PROXY_FIREWALL        , 0 ,
    HIDC_STATIC_FIREWALL_ADDRESS         ,  IDH_TOPIC_PROXY_FIREWALL        , 0 ,
    HIDC_STATIC_FIREWALL_PASSWORD        ,  IDH_TOPIC_FIREWALL_PORT         , 0 ,
    HIDC_STATIC_FIREWALL_PORT            ,  IDH_TOPIC_FIREWALL_USERNAME     , 0 ,
    HIDC_STATIC_FIREWALL_USERNAME        ,  IDH_TOPIC_FIREWALL_PASSWORD     , 0 ,

    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_FIREWALL_PROPERTIES_HTM , 0 ,
    0, 0, 0  
};

// AvisServerConfig.cpp  HELP MAP "Sample Policy"    Redone 12/18/99  
// PROPERTY PAGE HELPIDs FOR HELP BUTTON, F1, WHAT IS, AND RIGHT BUTTON CLICK ON THE PAGE
// AVIS_HELP_ID_STRUCT g_AvisServerConfigHelpIdArray[] = 
AVIS_HELP_ID_STRUCT g_SamplePolicyHelpIdArray[] = 
{
    // LOOKUP BASED ON RESOURCEID                 STRING ID'S    
    // THESE ARE THE ONLY ENTRIES WITH ASSOCATED TEXT
    IDD_AVIS_POLICY_PAGE                 , IDH_TOPIC_AVIS_POLICY_PAGE           , IDS_TOPIC_AVIS_POLICY_PAGE    , 
    IDC_CHECK_AUTO_SUBMIT                , IDH_TOPIC_CHECK_AUTO_SUBMIT          , 0                             ,
    IDC_EDIT_SUBMISSION_PRIORITY         , IDH_TOPIC_SUBMISSION_PRIORITY        , IDS_TOPIC_SUBMISSION_PRIORITY , 
    IDC_EDIT_MAX_PENDING_SAMPLES         , IDH_TOPIC_MAX_PENDING_SAMPLES        , IDS_TOPIC_MAX_PENDING_SAMPLES , 
    IDC_EDIT_QUEUE_CHECK_INTERVAL        , IDH_TOPIC_QUEUE_CHECK_INTERVAL       , IDS_TOPIC_QUEUE_CHECK_INTERVAL, 
    IDC_CHECK_STRIP_USER_DATA            , IDH_TOPIC_STRIP_USER_DATA            , IDS_TOPIC_STRIP_USER_DATA     , 
    IDC_CHECK_SCRAMBLE_USER_DATA         , IDH_TOPIC_SCRAMBLE_USER_DATA         , 0 ,
    IDC_CHECK_COMPRESS_USER_DATA         , IDH_TOPIC_COMPRESS_USER_DATA         , 0 ,
    IDC_EDIT_STATUS_INTERVAL2            , IDH_TOPIC_STATUS_INTERVAL            , 0 ,
    //IDC_CHECK_AUTO_DELIVERY              , IDH_TOPIC_AUTO_DELIVERY              , IDS_TOPIC_AUTO_DELIVERY       , 
    //IDC_DEFINITION_LIBRARY_PATH          , IDH_TOPIC_DEF_LIB_PATH               , IDS_TOPIC_DEF_LIB_PATH        , 
    //IDC_FILEDIALOG_BUTTON1               , IDH_TOPIC_BROWSE_FOR_DEFLIB          , IDS_TOPIC_BROWSE_FOR_DEFLIB   , 

    // SPINNERS
    IDC_SPIN2                            , IDH_TOPIC_SUBMISSION_PRIORITY        , IDS_TOPIC_SUBMISSION_PRIORITY , 
    IDC_SPIN3                            , IDH_TOPIC_MAX_PENDING_SAMPLES        , IDS_TOPIC_MAX_PENDING_SAMPLES , 
    IDC_SPIN1                            , IDH_TOPIC_QUEUE_CHECK_INTERVAL       , IDS_TOPIC_QUEUE_CHECK_INTERVAL, 
    IDC_SPIN_STATUS_INTERVAL5            , IDH_TOPIC_STATUS_INTERVAL            , 0 ,

    // STATIC FIELDS THAT POINT TO THE ABOVE
    IDC_STATIC_SUBMISSION_PRIORITY       , IDH_TOPIC_SUBMISSION_PRIORITY        , IDS_TOPIC_SUBMISSION_PRIORITY  ,
    IDC_STATIC_MAX_PENDING_SAMPLES       , IDH_TOPIC_MAX_PENDING_SAMPLES        , IDS_TOPIC_MAX_PENDING_SAMPLES  ,
    IDC_STATIC_QUEUE_CHECK_INTERVAL      , IDH_TOPIC_QUEUE_CHECK_INTERVAL       , IDS_TOPIC_QUEUE_CHECK_INTERVAL ,
    IDC_STATIC_STRIP_USER_DATA           , IDH_TOPIC_STRIP_USER_DATA            , IDS_TOPIC_STRIP_USER_DATA      ,
    IDC_STATIC_STATUS_INTERVAL2          , IDH_TOPIC_STATUS_INTERVAL            , 0 ,
    //IDC_STATIC_AUTO_DELIVERY             , IDH_TOPIC_AUTO_DELIVERY              , IDS_TOPIC_AUTO_DELIVERY        ,
    //IDC_STATIC_DEF_LIB_PATH              , IDH_TOPIC_DEF_LIB_PATH               , IDS_TOPIC_DEF_LIB_PATH         ,
    //IDC_STATIC_DEFINITION_CHECK_INTERVAL , IDH_TOPIC_DEFINITION_CHECK_INTERVAL  , IDS_TOPIC_DEFINITION_CHECK_INTERVAL,  
                                            
    // LOOKUP BASED ON HELPID OF STATIC FIELDS
    //HIDC_STATIC_SUBMISSION_PRIORITY      , IDH_TOPIC_SUBMISSION_PRIORITY        , IDS_TOPIC_SUBMISSION_PRIORITY  ,
    //HIDC_STATIC_MAX_PENDING_SAMPLES      , IDH_TOPIC_MAX_PENDING_SAMPLES        , IDS_TOPIC_MAX_PENDING_SAMPLES  ,
    HIDC_STATIC_QUEUE_CHECK_INTERVAL     , IDH_TOPIC_QUEUE_CHECK_INTERVAL       , IDS_TOPIC_QUEUE_CHECK_INTERVAL ,
    HIDC_CHECK_STRIP_USER_DATA           , IDH_TOPIC_STRIP_USER_DATA            , IDS_TOPIC_STRIP_USER_DATA      ,
    //HIDC_CHECK_SCRAMBLE_USER_DATA        , IDH_TOPIC_SCRAMBLE_USER_DATA         , 0 , 
    //HIDC_CHECK_COMPRESS_USER_DATA        , IDH_TOPIC_COMPRESS_USER_DATA         , 0 , 
    HIDC_STATIC_STATUS_INTERVAL2         , IDH_TOPIC_STATUS_INTERVAL            , 0 ,
//    HIDC_CHECK_AUTO_DELIVERY             , IDH_TOPIC_AUTO_DELIVERY              , IDS_TOPIC_AUTO_DELIVERY        ,
//    HIDC_STATIC_DEF_LIB_PATH             , IDH_TOPIC_DEF_LIB_PATH               , IDS_TOPIC_DEF_LIB_PATH         ,
                                            
    // DEFAULT FOR ID == 0
    0, IDH_TOPIC_AVIS_POLICY_PAGE, IDS_TOPIC_AVIS_POLICY_PAGE ,
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_SAMPLE_POLICY_PROPERTIES_HTM , 0 ,
    0, 0, 0 
};

    
// CAvisServerDefPolicyConfig     "Definition Policy"
AVIS_HELP_ID_STRUCT g_DefinitionPolicyHelpIdArray[] =
{
    IDD_AVIS_DEFINITION_POLICY_PAGE      ,  IDH_TOPIC_DEFINITION_POLICY_PAGE         , 0 ,
    IDC_ACTIVE_DEFS_SEQ_NUM              ,  IDH_TOPIC_ACTIVE_DEFS_SEQ_NUM            , 0 ,
    IDC_BLESSED_DEFS_SEQ_NUM             ,  IDH_BLESSED_DEFS_SEQ_NUM                 , 0 ,
    IDC_EDIT_DEF_HEURISTIC_LEVE2         ,  IDH_TOPIC_EDIT_DEF_HEURISTIC_LEVE2       , 0 ,
    IDC_EDIT_DEFS_BLESSED_INTERVAL       ,  IDH_TOPIC_EDIT_DEFS_BLESSED_INTERVAL     , 0 ,
    IDC_EDIT_DEFS_NEEDED_INTERVAL2       ,  IDH_TOPIC_EDIT_DEFS_NEEDED_INTERVAL2     , 0 ,
    IDC_EDIT_DEF_UNPACK_TIMEOUT          ,  IDH_TOPIC_EDIT_DEF_UNPACK_TIMEOUT        , 0 ,
    IDC_CHECK_PRUNE_DEFS                 ,  IDH_TOPIC_CHECK_PRUNE_DEFS               , 0 ,
    IDC_DEFINITION_LIBRARY_PATH          ,  IDH_TOPIC_DEF_LIB_PATH                   , 0 ,
    IDC_FILEDIALOG_BUTTON1               ,  IDH_TOPIC_BROWSE_FOR_DEFLIB              , 0 ,
                                          
    // SPINNERS
    IDC_SPIN_DEF_HEURISTIC_LEVEL         ,  IDH_TOPIC_EDIT_DEF_HEURISTIC_LEVE2       , 0 , 
    IDC_SPIN_DEFS_BLESSED_INTERVAL       ,  IDH_TOPIC_EDIT_DEFS_BLESSED_INTERVAL     , 0 , 
    IDC_SPIN_DEFS_NEEDED_INTERVAL2       ,  IDH_TOPIC_EDIT_DEFS_NEEDED_INTERVAL2     , 0 , 
    IDC_SPIN_DEF_UNPACK_TIMEOUT          ,  IDH_TOPIC_EDIT_DEF_UNPACK_TIMEOUT        , 0 , 

    // STATIC FIELDS THAT POINT TO THE ABOVE
    IDC_STATIC_DEF_DLOAD                 ,  IDH_TOPIC_EDIT_DEFS_BLESSED_INTERVAL     , 0 , 
    IDC_STATIC_DEF_ACTIVE                ,  IDH_TOPIC_ACTIVE_DEFS_SEQ_NUM            , 0 ,
    IDC_STATIC_DEFS_ACTIVE               ,  IDH_TOPIC_ACTIVE_DEFS_SEQ_NUM            , 0 ,
    IDC_STATIC_DEFS_BLESSED              ,  IDH_BLESSED_DEFS_SEQ_NUM                 , 0 ,
    IDC_STATIC_DEF_HEURISTIC_LEVEL       ,  IDH_TOPIC_EDIT_DEF_HEURISTIC_LEVE2       , 0 , 
    IDC_STATIC_DEF_BLESSED_INTERVAL      ,  IDH_TOPIC_EDIT_DEFS_BLESSED_INTERVAL     , 0 , 
    IDC_STATIC_DEF_NEEDED_INTERVAL2      ,  IDH_TOPIC_EDIT_DEFS_NEEDED_INTERVAL2     , 0 , 
    IDC_STATIC_DEF_UNPACK_TIMEOUT        ,  IDH_TOPIC_EDIT_DEF_UNPACK_TIMEOUT        , 0 , 
    IDC_STATIC_DEF_LIB_PATH              ,  IDH_TOPIC_DEF_LIB_PATH                   , 0 ,


    // LOOKUP BASED ON HELPID 
    HIDC_STATIC_DEFS_ACTIVE              ,  IDH_TOPIC_ACTIVE_DEFS_SEQ_NUM            , 0 ,
    HIDC_STATIC_DEFS_BLESSED             ,  IDH_BLESSED_DEFS_SEQ_NUM                 , 0 ,
    HIDC_EDIT_DEF_HEURISTIC_LEVE2        ,  IDH_TOPIC_EDIT_DEF_HEURISTIC_LEVE2       , 0 ,
    HIDC_STATIC_DEF_HEURISTIC_LEVEL      ,  IDH_TOPIC_EDIT_DEF_HEURISTIC_LEVE2       , 0 ,
    HIDC_STATIC_DEF_BLESSED_INTERVAL     ,  IDH_TOPIC_EDIT_DEFS_BLESSED_INTERVAL     , 0 ,
    HIDC_STATIC_DEF_NEEDED_INTERVAL2     ,  IDH_TOPIC_EDIT_DEFS_NEEDED_INTERVAL2     , 0 ,
    HIDC_EDIT_DEF_UNPACK_TIMEOUT         ,  IDH_TOPIC_EDIT_DEF_UNPACK_TIMEOUT        , 0 ,
    HIDC_STATIC_DEF_UNPACK_TIMEOUT       ,  IDH_TOPIC_EDIT_DEF_UNPACK_TIMEOUT        , 0 ,
    HIDC_DEFINITION_LIBRARY_PATH         ,  IDH_TOPIC_DEF_LIB_PATH                   , 0 ,
    HIDC_STATIC_DEF_LIB_PATH             ,  IDH_TOPIC_DEF_LIB_PATH                   , 0 ,
    HIDC_FILEDIALOG_BUTTON1              ,  IDH_TOPIC_BROWSE_FOR_DEFLIB              , 0 ,

    
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_DEFINITION_POLICY_PROPERTIES_HTM , 0 ,
    0, 0, 0  
};

//  CInstallDefinitions         "Install Definitions"
AVIS_HELP_ID_STRUCT g_InstallDefinitionsHelpIdArray[] =
{
    IDD_INSTALL_DEFS_PAGE               ,  IDH_TOPIC_INSTALL_DEFS_PAGE               ,  0  , 
    IDC_CHECK_UNBLESSED_BROADCAST       ,  IDH_TOPIC_CHECK_UNBLESSED_BROADCAST       ,  0  ,  
    IDC_CHECK_UNBLESSED_NARROWCAST      ,  IDH_TOPIC_CHECK_UNBLESSED_NARROWCAST      ,  0  ,  
    IDC_CHECK_UNBLESSED_POINTCAST       ,  IDH_TOPIC_CHECK_UNBLESSED_POINTCAST       ,  0  ,  
    IDC_CHECK_BLESSED_BROADCAST         ,  IDH_TOPIC_CHECK_BLESSED_BROADCAST         ,  0  ,  
    IDC_EDIT_DEF_DELIVERY_INTERVAL      ,  IDH_TOPIC_EDIT_DEF_DELIVERY_INTERVAL      ,  0  ,  
    IDC_EDIT_DEF_DELIVERY_TIMEOUT       ,  IDH_TOPIC_EDIT_DEF_DELIVERY_TIMEOUT       ,  0  ,  
    IDC_BUTTON_UNBLESED_SELECT_TARGETS  ,  IDH_TOPIC_BUTTON_UNBLESED_SELECT_TARGETS  ,  0  ,  
    IDC_BUTTON_BLESED_SELECT_TARGETS    ,  IDH_TOPIC_BUTTON_BLESED_SELECT_TARGETS    ,  0  ,  

    // SPINNERS
    IDC_SPIN_DEFS_DELIVERY_INTERVAL     ,  IDH_TOPIC_EDIT_DEF_DELIVERY_INTERVAL      ,  0  ,  
    IDC_SPIN_DEFS_DELIVERY_TIMEOUT      ,  IDH_TOPIC_EDIT_DEF_DELIVERY_TIMEOUT       ,  0  ,  

    // STATIC FIELDS THAT POINT TO THE ABOVE
    IDC_STATIC_DEFS_DELIVERY_INTERVAL3  ,  IDH_TOPIC_EDIT_DEF_DELIVERY_INTERVAL      ,  0  , 
    IDC_STATIC_DEFS_DELIVERY_TIMEOUT2   ,  IDH_TOPIC_EDIT_DEF_DELIVERY_TIMEOUT       ,  0  , 
    IDC_STATIC_INSTALL2                 ,  IDH_TOPIC_CHECK_BLESSED_BROADCAST         ,  0  ,
    IDC_STATIC_INSTALL1                 ,  IDH_TOPIC_CHECK_UNBLESSED_POINTCAST       ,  0  ,
    IDC_STATIC_INSTALL3                 ,  IDH_TOPIC_EDIT_DEF_DELIVERY_INTERVAL      ,  0  ,

    // LOOKUP BASED ON HELPID OF STATIC FIELDS
    HIDC_STATIC_DEFS_DELIVERY_INTERVAL3  ,  IDH_TOPIC_EDIT_DEF_DELIVERY_INTERVAL      ,  0  , 
    //HIDC_STATIC_DEFS_DELIVERY_TIMEOUT2   ,  IDH_TOPIC_EDIT_DEF_DELIVERY_TIMEOUT       ,  0  , 
    HIDC_EDIT_DEF_DELIVERY_INTERVAL     ,  IDH_TOPIC_EDIT_DEF_DELIVERY_INTERVAL      ,  0  , 
    //HIDC_EDIT_DEF_DELIVERY_TIMEOUT      ,  IDH_TOPIC_EDIT_DEF_DELIVERY_TIMEOUT       ,  0  , 


    // DEFAULT FOR ID == 0
    0, IDH_TOPIC_INSTALL_DEFS_PAGE, 0 ,

    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_INSTALL_DEFINITIONS_PROPERTIES_HTM , 0 ,
    0, 0, 0  
};

// CServerGeneralError   "Attention"
AVIS_HELP_ID_STRUCT g_AtttentionPageHelpIdArray[] =
{
    IDD_GENERAL_ERROR                    ,  IDH_TOPIC_GENERAL_ERROR                  ,  0  ,
    IDC_GENERAL_ERROR_EDIT               ,  IDH_TOPIC_GENERAL_ERROR_EDIT             ,  0  ,
    HIDC_GENERAL_ERROR_EDIT              ,  IDH_TOPIC_GENERAL_ERROR_EDIT             ,  0  ,
//  HIDC_STATIC_TEST1                    ,  IDH_TOPIC_GENERAL_ERROR_EDIT             ,  0  ,

    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_ATTENTION_PROPERTIES , 0 ,
    0, 0, 0  
};


// CSampleErrorPage   "Sample Errors"   
AVIS_HELP_ID_STRUCT g_SampleErrorPageHelpIdArray[] =  
{
    IDD_SAMPLE_ERROR                      , IDH_TOPIC_SAMPLE_ERROR                  ,  0  ,
    IDC_SAMPLE_ERROR_EDIT                 , IDH_TOPIC_SAMPLE_ERROR                  ,  0  ,
    HIDC_SAMPLE_ERROR_EDIT                , IDH_TOPIC_SAMPLE_ERROR                  ,  0  ,

    IDC_ICON_BUTTON                       , IDH_TOPIC_FILE_NAME_AND_ICON         , IDS_TOPIC_SAMPLE_ACTIONS_ICON         , 
    IDC_NAME_EDIT                         , IDH_TOPIC_FILE_NAME_AND_ICON         , IDS_TOPIC_SAMPLE_ACTIONS_ICON         , 
    HIDC_ICON_BUTTON                      , IDH_TOPIC_FILE_NAME_AND_ICON         , IDS_TOPIC_SAMPLE_ACTIONS_ICON         , 
    HIDC_NAME_EDIT                        , IDH_TOPIC_FILE_NAME_AND_ICON         , IDS_TOPIC_SAMPLE_ACTIONS_ICON         , 

    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_SAMPLE_ERRORS_PROPERTIES , 0 ,
    0, 0, 0  
};

// CustomerInfo.cpp  HELP MAP
// PROPERTY PAGE HELPIDs FOR HELP BUTTON, F1, WHAT IS, AND RIGHT BUTTON CLICK ON THE PAGE
AVIS_HELP_ID_STRUCT g_CustomerInfoHelpIdArray[] = 
{
    // LOOKUP BASED ON RESOURCEID              STRING ID'S    
    // THESE ARE THE ONLY ENTRIES WITH ASSOCATED TEXT
    IDD_AVIS_CUSTOMER_PAGE        , IDH_TOPIC_CUSTOMER_PAGE  , IDS_TOPIC_CUSTOMER_PAGE , 
    IDC_EDIT_COMPANY_NAME         , IDH_TOPIC_COMPANY_NAME   , IDS_TOPIC_COMPANY_NAME  , 
    IDC_EDIT_CONTACT_NAME         , IDH_TOPIC_CONTACT_NAME   , IDS_TOPIC_CONTACT_NAME  , 
    IDC_EDIT_CONTACT_PHONE        , IDH_TOPIC_CONTACT_PHONE  , IDS_TOPIC_CONTACT_PHONE , 
    IDC_EDIT_CONTACT_EMAIL        , IDH_TOPIC_CONTACT_EMAIL  , IDS_TOPIC_CONTACT_EMAIL , 
    IDC_EDIT_CUST_ACCT            , IDH_TOPIC_CUST_ACCT      , IDS_TOPIC_CUST_ACCT     , 

    // STATIC FIELDS THAT POINT TO THE ABOVE
    IDC_STATIC_EDIT_COMPANY_NAME  , IDH_TOPIC_COMPANY_NAME   , IDS_TOPIC_COMPANY_NAME  , 
    IDC_STATIC_EDIT_CONTACT_NAME  , IDH_TOPIC_CONTACT_NAME   , IDS_TOPIC_CONTACT_NAME  , 
    IDC_STATIC_EDIT_CONTACT_PHONE , IDH_TOPIC_CONTACT_PHONE  , IDS_TOPIC_CONTACT_PHONE , 
    IDC_STATIC_EDIT_CONTACT_EMAIL , IDH_TOPIC_CONTACT_EMAIL  , IDS_TOPIC_CONTACT_EMAIL , 
    IDC_STATIC_EDIT_CUST_ACCT     , IDH_TOPIC_CUST_ACCT      , IDS_TOPIC_CUST_ACCT     , 
                                    
    // LOOKUP BASED ON HELPID OF STATIC FIELDS
    HIDC_STATIC_EDIT_COMPANY_NAME , IDH_TOPIC_COMPANY_NAME  , IDS_TOPIC_COMPANY_NAME  , 
    HIDC_STATIC_EDIT_CONTACT_NAME , IDH_TOPIC_CONTACT_NAME  , IDS_TOPIC_CONTACT_NAME  , 
    HIDC_STATIC_EDIT_CONTACT_PHONE, IDH_TOPIC_CONTACT_PHONE , IDS_TOPIC_CONTACT_PHONE , 
    HIDC_STATIC_EDIT_CONTACT_EMAIL, IDH_TOPIC_CONTACT_EMAIL , IDS_TOPIC_CONTACT_EMAIL , 
    HIDC_STATIC_EDIT_CUST_ACCT    , IDH_TOPIC_CUST_ACCT     , IDS_TOPIC_CUST_ACCT     , 
                                    

    // DEFAULT FOR ID == 0
    0, IDH_TOPIC_CUSTOMER_PAGE, IDS_TOPIC_CUSTOMER_PAGE ,
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_CUSTOMER_INFO_PROPERTIES_HTM , 0 ,
    0, 0, 0  
};                                                           


// ConfirmationDialog.cpp  HELP MAP
// PROPERTY PAGE HELPIDs FOR HELP BUTTON, F1, WHAT IS, AND RIGHT BUTTON CLICK ON THE PAGE
AVIS_HELP_ID_STRUCT g_ConfirmationDialogSubmitHelpIdArray[] = 
{
    // LOOKUP BASED ON RESOURCEID              STRING ID'S    
    // THESE ARE THE ONLY ENTRIES WITH ASSOCATED TEXT
    // HELP FOR THE LISTBOX CONTROL, BASED ON WHERE IT WAS CALLED FROM
    IDH_SUBMIT_CONFIRM_LISTBOX       , IDH_TOPIC_SUBMIT_CONFIRM_LISTBOX          , IDS_TOPIC_SUBMIT_CONFIRM_LISTBOX           ,  
    IDH_DELIVER_CONFIRM_LISTBOX      , IDH_TOPIC_DELIVER_CONFIRM_LISTBOX         , IDS_TOPIC_DELIVER_CONFIRM_LISTBOX          ,  
    // HELP PAGE GIVING AN OVERVIEW OF THE ENTIRE DIALOG, BASED ON WHERE IT WAS CALLED FROM
    IDH_OVERVIEW_SUBMIT_FILES_DIALOG , IDH_TOPIC_OVERVIEW_SUBMIT_CONFIRM_DIALOG  , IDS_TOPIC_OVERVIEW_SUBMIT_CONFIRM_DIALOG   ,  
    IDH_OVERVIEW_DELIVER_DEFS_DIALOG , IDH_TOPIC_OVERVIEW_DELIVER_CONFIRM_DIALOG , IDS_TOPIC_OVERVIEW_DELIVER_CONFIRM_DIALOG  ,  
    IDC_HELP_BUTTON_DIALOG2          , IDH_TOPIC_HELP_BUTTON                     , IDS_TOPIC_HELP_BUTTON                      ,  
    IDOK                             , IDH_TOPIC_OK_BUTTON                       , IDS_TOPIC_OK_BUTTON                        ,  
    IDCANCEL                         , IDH_TOPIC_CANCEL_BUTTON                   , IDS_TOPIC_CANCEL_BUTTON                    ,  
    IDC_LIST1                        , IDH_TOPIC_SUBMIT_CONFIRM_LISTBOX          , IDS_TOPIC_SUBMIT_CONFIRM_LISTBOX           ,

    // DISABLE
    IDD_CONFIRMDIALOG , 0 , 0,
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_CONFIRM_SUBMISSION_HTM , 0 ,
    0, 0, 0  
};                                                           

// ConfirmationDialog.cpp  HELP MAP
// PROPERTY PAGE HELPIDs FOR HELP BUTTON, F1, WHAT IS, AND RIGHT BUTTON CLICK ON THE PAGE
AVIS_HELP_ID_STRUCT g_ConfirmationDialogDeliveryHelpIdArray[] = 
{
    // LOOKUP BASED ON RESOURCEID              STRING ID'S    
    // THESE ARE THE ONLY ENTRIES WITH ASSOCATED TEXT
    // HELP FOR THE LISTBOX CONTROL, BASED ON WHERE IT WAS CALLED FROM
    IDH_SUBMIT_CONFIRM_LISTBOX       , IDH_TOPIC_SUBMIT_CONFIRM_LISTBOX          , IDS_TOPIC_SUBMIT_CONFIRM_LISTBOX           ,  
    IDH_DELIVER_CONFIRM_LISTBOX      , IDH_TOPIC_DELIVER_CONFIRM_LISTBOX         , IDS_TOPIC_DELIVER_CONFIRM_LISTBOX          ,  
    // HELP PAGE GIVING AN OVERVIEW OF THE ENTIRE DIALOG, BASED ON WHERE IT WAS CALLED FROM
    IDH_OVERVIEW_SUBMIT_FILES_DIALOG , IDH_TOPIC_OVERVIEW_SUBMIT_CONFIRM_DIALOG  , IDS_TOPIC_OVERVIEW_SUBMIT_CONFIRM_DIALOG   ,  
    IDH_OVERVIEW_DELIVER_DEFS_DIALOG , IDH_TOPIC_OVERVIEW_DELIVER_CONFIRM_DIALOG , IDS_TOPIC_OVERVIEW_DELIVER_CONFIRM_DIALOG  ,  
    IDC_HELP_BUTTON_DIALOG2          , IDH_TOPIC_HELP_BUTTON                     , IDS_TOPIC_HELP_BUTTON                      ,  
    IDOK                             , IDH_TOPIC_OK_BUTTON                       , IDS_TOPIC_OK_BUTTON                        ,  
    IDCANCEL                         , IDH_TOPIC_CANCEL_BUTTON                   , IDS_TOPIC_CANCEL_BUTTON                    ,  
    IDC_LIST1                        , IDH_TOPIC_DELIVER_CONFIRM_LISTBOX         , IDS_TOPIC_DELIVER_CONFIRM_LISTBOX          ,

    // DISABLE
    IDD_CONFIRMDIALOG , 0 , 0,
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_CONFIRM_DELIVERY_HTM , 0 ,
    0, 0, 0  
};                                                           


// SelectTargetDialog.cpp HELP MAP
// PROPERTY PAGE HELPIDs FOR HELP BUTTON, F1, WHAT IS, AND RIGHT BUTTON CLICK ON THE PAGE
AVIS_HELP_ID_STRUCT g_SelectTargetDialogHelpIdArray[] =
{
    //IDD_SELECTTARGETDIALOG
    IDC_SELECT_TARGET_LIST      ,  IDH_TOPIC_SELECT_TARGET_LIST               , 0,
    IDOK                        ,  IDH_TOPIC_SELECT_IDOK                      , 0,
    IDCANCEL                    ,  IDH_TOPIC_SELECT_IDCANCEL                  , 0,
    IDC_HELP_BUTTON_DIALOG2     ,  IDH_TOPIC_SELECT_HELP_BUTTON_DIALOG2       , 0,
    IDADD                       ,  IDH_TOPIC_SELECT_IDADD                     , 0,
    IDDELETE                    ,  IDH_TOPIC_SELECT_IDDELETE                  , 0,

    // DISABLE
    IDC_EDIT1              , 0 , 0,
    IDD_SELECTTARGETDIALOG , 0 , 0,
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_INSTALL_DEFINITIONS_PROPERTIES_HTM , 0 ,
    0, 0, 0  
};                                                           



// The title for this page is now "AVIS Sample Attributes". Jhill 5/29/99
// SampleAttributesPage.cpp  HELP MAP
// PROPERTY PAGE HELPIDs FOR HELP BUTTON, F1, WHAT IS, AND RIGHT BUTTON CLICK ON THE PAGE
AVIS_HELP_ID_STRUCT g_SampleAttributesHelpIdArray[] = 
{
    // LOOKUP BASED ON RESOURCEID              STRING ID'S    
    // THESE ARE THE ONLY ENTRIES WITH ASSOCATED TEXT
    // IDD_SAMPLE_PLATFORM              , IDH_TOPIC_OVERVIEW_SAMPLE_ATTRIBUTES , IDS_TOPIC_OVERVIEW_SAMPLE_ATTRIBUTES ,     
    IDD_SAMPLE_ATTRIBUTES            , IDH_TOPIC_OVERVIEW_SAMPLE_ATTRIBUTES , IDS_TOPIC_OVERVIEW_SAMPLE_ATTRIBUTES ,     
    // HELP FOR THE LISTBOX CONTROL, BASED ON WHERE IT WAS CALLED FROM
    IDH_PLATFORM_ATTRIBS_LISTBOX     , IDH_TOPIC_SAMPLE_ATTRIBS_LISTBOX     , IDS_TOPIC_SAMPLE_ATTRIBS_LISTBOX   ,                 // IDS_TOPIC_PLATFORM_ATTRIBS_LISTBOX
    // HELP PAGE GIVING AN OVERVIEW OF THE ENTIRE DIALOG, BASED ON WHERE IT WAS CALLED FROM
    IDH_OVERVIEW_PLATFORM_ATTRIB_LIST, IDH_TOPIC_OVERVIEW_SAMPLE_ATTRIBUTES , IDS_TOPIC_OVERVIEW_SAMPLE_ATTRIBUTES   ,             // IDS_TOPIC_OVERVIEW_SAMPLE_PLATFORM
    // myListCtrl
    IDC_LIST1                        , IDH_TOPIC_SAMPLE_ATTRIBS_LISTBOX     , IDS_TOPIC_SAMPLE_ATTRIBS_LISTBOX ,

    IDC_ICON_BUTTON                  , IDH_TOPIC_FILE_NAME_AND_ICON         , IDS_TOPIC_SAMPLE_ACTIONS_ICON         ,
    IDC_NAME_EDIT                    , IDH_TOPIC_FILE_NAME_AND_ICON         , IDS_TOPIC_SAMPLE_ACTIONS_ICON         ,
    HIDC_ICON_BUTTON                 , IDH_TOPIC_FILE_NAME_AND_ICON         , IDS_TOPIC_SAMPLE_ACTIONS_ICON         ,
    HIDC_NAME_EDIT                   , IDH_TOPIC_FILE_NAME_AND_ICON         , IDS_TOPIC_SAMPLE_ACTIONS_ICON         ,

    // DEFAULT FOR ID == 0
    0, IDH_TOPIC_OVERVIEW_SAMPLE_ATTRIBUTES, IDS_TOPIC_OVERVIEW_SAMPLE_ATTRIBUTES ,
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_SAMPLE_ATTRIBUTE_PROPERTIES_HTM , 0 ,
    0, 0, 0  
};                                                           



// SampleActionsPage.cpp  HELP MAP
// PROPERTY PAGE HELPIDs FOR HELP BUTTON, F1, WHAT IS, AND RIGHT BUTTON CLICK ON THE PAGE
AVIS_HELP_ID_STRUCT g_SampleActionsHelpIdArray[] = 
{
    // LOOKUP BASED ON RESOURCEID              STRING ID'S    
    // THESE ARE THE ONLY ENTRIES WITH ASSOCATED TEXT
    IDD_SAMPLE_ACTIONS                      , IDH_TOPIC_SAMPLE_ACTIONS                , IDS_TOPIC_SAMPLE_ACTIONS              ,  
    IDC_SAMPLE_SUBMISSION_PRIORITY          , IDH_TOPIC_SAMPLE_SUBMISSION_PRIORITY    , IDS_TOPIC_SAMPLE_SUBMISSION_PRIORITY  ,  
    IDC_ANALYSIS_STATE                      , IDH_TOPIC_ANALYSIS_STATE                , IDS_TOPIC_ANALYSIS_STATE              ,  
    IDC_GATEWAY_COOKIE                      , IDH_TOPIC_GATEWAY_COOKIE                , IDS_TOPIC_GATEWAY_COOKIE              ,  
    IDC_ISSUE_NUMBER                        , IDH_TOPIC_ISSUE_NUMBER                  , IDS_TOPIC_ISSUE_NUMBER                ,  
    IDC_VIRUS_NAME                          , IDH_TOPIC_VIRUS_NAME                    , IDS_TOPIC_VIRUS_NAME                  ,  
    IDC_CHECK_SAMPLE_AUTO_DELIVERY          , IDH_TOPIC_DEFINITION_AUTOMATIC_DELIVERY , IDS_TOPIC_SAMPLE_AUTOMATIC_DELIVERY   ,  
    IDC_DEF_SEQUENCE_NEEDED                 , IDH_TOPIC_DEF_SEQ_NEEDED                , IDS_TOPIC_DEF_SEQ_NEEDED              ,  
    IDC_SPIN1                               , IDH_TOPIC_SAMPLE_SUBMISSION_PRIORITY    , IDS_TOPIC_SAMPLE_SUBMISSION_PRIORITY  ,
    IDC_ACTIONS_SUBMIT_BUTTON               , IDH_TOPIC_DEFINITION_SUBMIT_BUTTON      , IDS_TOPIC_ACTION_SUBMIT_BUTTON        ,
    IDC_ACTIONS_DELIVER_BUTTON              , IDH_TOPIC_DEFINITION_DELIVERY_BUTTON    , IDS_TOPIC_ACTION_DELIVER_BUTTON       ,

    IDC_ICON_BUTTON                         , IDH_TOPIC_FILE_NAME_AND_ICON            , IDS_TOPIC_SAMPLE_ACTIONS_ICON         ,
    IDC_FILENAME_EDIT                       , IDH_TOPIC_FILE_NAME_AND_ICON            , IDS_TOPIC_SAMPLE_ACTIONS_ICON         ,
    HIDC_FILENAME_EDIT                      , IDH_TOPIC_FILE_NAME_AND_ICON            , IDS_TOPIC_SAMPLE_ACTIONS_ICON         , 
    HIDC_ICON_BUTTON                        , IDH_TOPIC_FILE_NAME_AND_ICON            , IDS_TOPIC_SAMPLE_ACTIONS_ICON         , 

    IDC_STATIC_SAMPLE_SUBMISSION_PRIORITY   , IDH_TOPIC_SAMPLE_SUBMISSION_PRIORITY    , IDS_TOPIC_SAMPLE_SUBMISSION_PRIORITY  ,  
    IDC_STATIC_ANALYSIS_STATE               , IDH_TOPIC_ANALYSIS_STATE                , IDS_TOPIC_ANALYSIS_STATE              ,  
    IDC_STATIC_GATEWAY_COOKIE               , IDH_TOPIC_GATEWAY_COOKIE                , IDS_TOPIC_GATEWAY_COOKIE              ,  
    IDC_STATIC_ISSUE_NUMBER                 , IDH_TOPIC_ISSUE_NUMBER                  , IDS_TOPIC_ISSUE_NUMBER                ,  
    IDC_STATIC_VIRUS_NAME                   , IDH_TOPIC_VIRUS_NAME                    , IDS_TOPIC_VIRUS_NAME                  ,  
    IDC_STATIC_SAMPLE_AUTOMATIC_DELIVERY    , IDH_TOPIC_DEFINITION_AUTOMATIC_DELIVERY , IDS_TOPIC_SAMPLE_AUTOMATIC_DELIVERY   ,  
    IDC_STATIC_DEF_SEQ_NEEDED               , IDH_TOPIC_DEF_SEQ_NEEDED                , IDS_TOPIC_DEF_SEQ_NEEDED              ,  
                                               
    HIDC_STATIC_SAMPLE_SUBMISSION_PRIORITY  , IDH_TOPIC_SAMPLE_SUBMISSION_PRIORITY    , IDS_TOPIC_SAMPLE_SUBMISSION_PRIORITY  ,  
    HIDC_STATIC_ANALYSIS_STATE              , IDH_TOPIC_ANALYSIS_STATE                , IDS_TOPIC_ANALYSIS_STATE              ,  
    HIDC_STATIC_GATEWAY_COOKIE              , IDH_TOPIC_GATEWAY_COOKIE                , IDS_TOPIC_GATEWAY_COOKIE              ,  
    HIDC_STATIC_ISSUE_NUMBER                , IDH_TOPIC_ISSUE_NUMBER                  , IDS_TOPIC_ISSUE_NUMBER                ,  
    HIDC_STATIC_VIRUS_NAME                  , IDH_TOPIC_VIRUS_NAME                    , IDS_TOPIC_VIRUS_NAME                  ,  
    HIDC_CHECK_SAMPLE_AUTO_DELIVERY         , IDH_TOPIC_DEFINITION_AUTOMATIC_DELIVERY , IDS_TOPIC_SAMPLE_AUTOMATIC_DELIVERY   ,  
    HIDC_STATIC_DEF_SEQ_NEEDED              , IDH_TOPIC_DEF_SEQ_NEEDED                , IDS_TOPIC_DEF_SEQ_NEEDED              ,  


    // DEFAULT FOR ID == 0
    0, IDH_TOPIC_SAMPLE_ACTIONS, IDS_TOPIC_SAMPLE_ACTIONS ,
    0xFFFF, 0xFFFF, 0xFFFF,
    // TOPIC FILENAME
    0, IDH_TOPIC_FILENAME_SAMPLE_ACTIONS_PROPERTIES_HTM , 0 ,
    0, 0, 0  
};                                                           








#include "AvisSampleFields.h"

// AVIS ATTRIBUTE NAMES. THESE SHOULD NOT BE TRANSLATED
AVIS_TOPIC_FILENAMES_STRUCT g_AvisSampleAttributeNamesArray[] =
{
    { IDH_X_PLATFORM_USER                      , AVIS_X_PLATFORM_USER                       },  // X-Platform-User                       
    { IDH_X_PLATFORM_COMPUTER                  , AVIS_X_PLATFORM_COMPUTER                   },  // X-Platform-Computer                   
    { IDH_X_PLATFORM_DOMAIN                    , AVIS_X_PLATFORM_DOMAIN                     },  // X-Platform-Domain                     
    { IDH_X_PLATFORM_ADDRESS                   , AVIS_X_PLATFORM_ADDRESS                    },  // X-Platform-Address                    
    { IDH_X_PLATFORM_PROCESSOR                 , AVIS_X_PLATFORM_PROCESSOR                  },  // X-Platform-Processor                  
    { IDH_X_PLATFORM_DISTRIBUTOR               , AVIS_X_PLATFORM_DISTRIBUTOR                },  // X-Platform-Distributor                
    { IDH_X_PLATFORM_HOST                      , AVIS_X_PLATFORM_HOST                       },  // X-Platform-Host                       
    { IDH_X_PLATFORM_SYSTEM                    , AVIS_X_PLATFORM_SYSTEM                     },  // X-Platform-System                     
    { IDH_X_PLATFORM_LANGUAGE                  , AVIS_X_PLATFORM_LANGUAGE                   },  // X-Platform-Language                   
    { IDH_X_PLATFORM_OWNER                     , AVIS_X_PLATFORM_OWNER                      },  // X-Platform-Owner                      
    { IDH_X_PLATFORM_SCANNER                   , AVIS_X_PLATFORM_SCANNER                    },  // X-Platform-Scanner                    
    { IDH_X_PLATFORM_CORRELATOR                , AVIS_X_PLATFORM_CORRELATOR                 },  // X-Platform-Correlator                 
    { IDH_X_PLATFORM_GUID                      , AVIS_X_PLATFORM_GUID                       },  // X-Platform-GUID                       
    { IDH_X_PLATFORM_INFO_DELIVER              , AVIS_X_PLATFORM_INFO_DELIVER               },  // X-Platform-Deliver                       
                                                                                                                                               
    { IDH_X_DATE_ACCESSED                      , AVIS_X_DATE_ACCESSED                       },  // X-Date-Accessed                       
    { IDH_X_DATE_ANALYZED                      , AVIS_X_DATE_ANALYZED                       },  // X-Date-Analyzed                       
    { IDH_X_DATE_BLESSED                       , AVIS_X_DATE_BLESSED                        },  // X-Date-Blessed                        
    { IDH_X_DATE_CAPTURED                      , AVIS_X_DATE_CAPTURED                       },  // X-Date-Captured                       
    { IDH_X_DATE_CREATED                       , AVIS_X_DATE_CREATED                        },  // X-Date-Created                        
    { IDH_X_DATE_DISTRIBUTED                   , AVIS_X_DATE_DISTRIBUTED                    },  // X-Date-Distributed                    
    { IDH_X_DATE_FINISHED                      , AVIS_X_DATE_FINISHED                       },  // X-Date-Finished                       
    { IDH_X_DATE_FORWARDED                     , AVIS_X_DATE_FORWARDED                      },  // X-Date-Forwarded                      
    { IDH_X_DATE_INSTALLED                     , AVIS_X_DATE_INSTALLED                      },  // X-Date-Installed                      
    { IDH_X_DATE_MODIFIED                      , AVIS_X_DATE_MODIFIED                       },  // X-Date-Modified                       
    { IDH_X_DATE_PRODUCED                      , AVIS_X_DATE_PRODUCED                       },  // X-Date-Produced                       
    { IDH_X_DATE_PUBLISHED                     , AVIS_X_DATE_PUBLISHED                      },  // X-Date-Published                      
    { IDH_X_DATE_SUBMITTED                     , AVIS_X_DATE_SUBMITTED                      },  // X-Date-Submitted                      
    { IDH_X_DATE_ANALYSIS_FINISHED             , AVIS_X_DATE_ANALYSIS_FINISHED              },  // X-Date-Analysis-Finished              
    { IDH_X_DATE_SAMPLE_FINISHED               , AVIS_X_DATE_SAMPLE_FINISHED                },  // X-Date-Sample-Finished                
    { IDH_X_DATE_QUARANTINEDATE                , AVIS_X_DATE_QUARANTINEDATE                 },  // X-Date-QuarantineDate                 
    { IDH_X_DATE_QUARANTINE                    , AVIS_X_DATE_QUARANTINE                     },  // X-Date-Quarantine                     
    { IDH_X_DATE_COMPLETED                     , AVIS_X_DATE_COMPLETED                      },  // X-Date-Completed                     
                                                                                                                                         
    { IDH_X_SCAN_VIRUS_IDENTIFIER              , AVIS_X_SCAN_VIRUS_IDENTIFIER               },  // X-Scan-Virus-Identifier               
    { IDH_X_SCAN_SIGNATURE_SEQUENCE            , AVIS_X_SCAN_SIGNATURE_SEQUENCE             },  // X-Scan-Signature-Sequence             
    { IDH_X_SCAN_SIGNATURE_VERSION             , AVIS_X_SCAN_SIGNATURE_VERSION              },  // X-Scan-Signature-Version              
    { IDH_X_SCAN_SIGNATURES_SEQUENCE           , AVIS_X_SCAN_SIGNATURES_SEQUENCE            },  // X-Scan-Signatures-Sequence            
    { IDH_X_SCAN_SIGNATURES_VERSION            , AVIS_X_SCAN_SIGNATURES_VERSION             },  // X-Scan-Signatures-Version             
    { IDH_X_SCAN_VIRUS_NAME                    , AVIS_X_SCAN_VIRUS_NAME                     },  // X-Scan-Virus-Name                     
    { IDH_X_SCAN_RESULT                        , AVIS_X_SCAN_RESULT                         },  // X-Scan-Result                         
                                                                                                                                         
    { IDH_X_SAMPLE_CHECKSUM                    , AVIS_X_SAMPLE_CHECKSUM                     },  // X-Sample-Checksum                     
    { IDH_X_CHECKSUM_METHOD                    , AVIS_X_CHECKSUM_METHOD                     },  // X-Checksum-Method                     
    { IDH_X_SAMPLE_EXTENSION                   , AVIS_X_SAMPLE_EXTENSION                    },  // X-Sample-Extension                    
    { IDH_X_SAMPLE_FILE                        , AVIS_X_SAMPLE_FILE                         },  // X-Sample-File                         
    { IDH_X_SAMPLE_SIZE                        , AVIS_X_SAMPLE_SIZE                         },  // X-Sample-Size                         
    { IDH_X_SAMPLE_TYPE                        , AVIS_X_SAMPLE_TYPE                         },  // X-Sample-Type                         
    { IDH_X_SAMPLE_REASON                      , AVIS_X_SAMPLE_REASON                       },  // X-Sample-Reason                       
    { IDH_X_SAMPLE_GEOMETRY                    , AVIS_X_SAMPLE_GEOMETRY                     },  // X-Sample-Geometry                     
    { IDH_X_SAMPLE_STATUS                      , AVIS_X_SAMPLE_STATUS                       },  // X-Sample-Status                       
    { IDH_X_SAMPLE_SUBMISSION_ROUTE            , AVIS_X_SAMPLE_SUBMISSION_ROUTE             },  // X-Sample-Submission-Route             
    { IDH_X_SAMPLE_UUID                        , AVIS_X_SAMPLE_UUID                         },  // X-Sample-UUID                         
    { IDH_X_SAMPLE_FILEID                      , AVIS_X_SAMPLE_FILEID                       },  // X-Sample-FileID                       
    { IDH_X_SAMPLE_CHANGES                     , AVIS_X_SAMPLE_CHANGES                      },  // X-Sample-Changes                      
    { IDH_X_SAMPLE_PRIORITY                    , AVIS_X_SAMPLE_PRIORITY                     },  // X-Sample-Priority                     
    { IDH_X_SAMPLE_SIGNATURES_PRIORITY         , AVIS_X_SAMPLE_SIGNATURES_PRIORITY          },  // X-Sample-Signatures-Priority          
    { IDH_X_SAMPLE_SECTOR                      , AVIS_X_SAMPLE_SECTOR                       },  // X-Sample-Sector                       
    { IDH_X_SAMPLE_SERVICE                     , AVIS_X_SAMPLE_SERVICE                      },  // X-Sample-Service                      
    { IDH_X_SAMPLE_STRIP                       , AVIS_X_SAMPLE_STRIP                        },  // X-Sample-Strip                        
    { IDH_X_SAMPLE_CATEGORY                    , AVIS_X_SAMPLE_CATEGORY                     },  // X-Sample-Category                     
    { IDH_X_SAMPLE_ERROR_ATTENTION             , AVIS_X_SAMPLE_ERROR_ATTENTION              },  // X-Attention                     
                                                                                                                                         
    { IDH_X_ANALYSIS_STATE                     , AVIS_X_ANALYSIS_STATE                      },  // X-Analysis-State                      
    { IDH_X_ANALYSIS_COOKIE                    , AVIS_X_ANALYSIS_COOKIE                     },  // X-Analysis-Cookie                     
    { IDH_X_ANALYSIS_ISSUE                     , AVIS_X_ANALYSIS_ISSUE                      },  // X-Analysis-Issue                      
    { IDH_X_ANALYSIS_VIRUS_NAME                , AVIS_X_ANALYSIS_VIRUS_NAME                 },  // X-Analysis-Virus_Name                 
    { IDH_X_ANALYSIS_SERVICE                   , AVIS_X_ANALYSIS_SERVICE                    },  // X-Analysis-Service                    
    { IDH_X_ANALYSIS_VIRUS_IDENTIFIER          , AVIS_X_ANALYSIS_VIRUS_IDENTIFIER           },  // X-Analysis-Virus-Identifier           
                                                                                                                                         
    { IDH_X_SIGNATURES_SEQUENCE                , AVIS_X_SIGNATURES_SEQUENCE                 },  // X-Signatures-Sequence                 
    { IDH_X_SIGNATURES_SEQUENCE                , AVIS_X_SIGNATURES_SEQUENCE_OLD             },  // X-Signature-Sequence                 
    { IDH_X_SIGNATURES_PRIORITY                , AVIS_X_SIGNATURES_PRIORITY                 },  // X-Signatures-Priority                 
    { IDH_X_SIGNATURES_NAME                    , AVIS_X_SIGNATURES_NAME                     },  // X-Signatures-Name                     
    { IDH_X_SIGNATURES_SIZE                    , AVIS_X_SIGNATURES_SIZE                     },  // X-Signatures-Size                     
    { IDH_X_SIGNATURES_VERSION                 , AVIS_X_SIGNATURES_VERSION                  },  // X-Signatures-Version                  
    { IDH_X_SIGNATURES_VERSION                 , AVIS_X_SIGNATURES_VERSION_OLD              },  // X-Signature-Version                  
//  { IDH_X_SIGNATURES_NAME_X_SAMPLE_CHECKSUM  , AVIS_X_SIGNATURES_NAME_X_SAMPLE_CHECKSUM   },  // X-Signatures-Name:X-Sample-Checksum   
                                                                                                                                         
    { IDH_X_CONTENT_CHECKSUM                   , AVIS_X_CONTENT_CHECKSUM                    },  // X-Content-Checksum                    
    { IDH_X_CONTENT_COMPRESSION                , AVIS_X_CONTENT_COMPRESSION                 },  // X-Content-Compression                 
    { IDH_X_CONTENT_ENCODING                   , AVIS_X_CONTENT_ENCODING                    },  // X-Content-Encoding                    
    { IDH_X_CONTENT_SCRAMBLING                 , AVIS_X_CONTENT_SCRAMBLING                  },  // X-Content-Scrambling                  
                                                                                                                                         
    { IDH_X_CUSTOMER_CONTACT_EMAIL             , AVIS_X_CUSTOMER_CONTACT_EMAIL              },  // X-Customer-Contact-Email              
    { IDH_X_CUSTOMER_CONTACT_NAME              , AVIS_X_CUSTOMER_CONTACT_NAME               },  // X-Customer-Contact-Name               
    { IDH_X_CUSTOMER_CONTACT_TELEPHONE         , AVIS_X_CUSTOMER_CONTACT_TELEPHONE          },  // X-Customer-Contact-Telephone          
    { IDH_X_CUSTOMER_CREDENTIALS               , AVIS_X_CUSTOMER_CREDENTIALS                },  // X-Customer-Credentials                
    { IDH_X_CUSTOMER_IDENTIFIER                , AVIS_X_CUSTOMER_IDENTIFIER                 },  // X-Customer-Identifier                 
    { IDH_X_CUSTOMER_NAME                      , AVIS_X_CUSTOMER_NAME                       },  // X-Customer-Name                       

    { IDH_X_ICEPACK_INITIALIZED                , AVIS_X_ICEPACK_INITIALIZED                 },  // X-Icepack-Initialized                       
    { IDH_X_REAL_SCAN_RESULT                   , AVIS_X_REAL_SCAN_RESULT                    },  // X-Real-Scan-Result       
    { IDH_X_SAMPLE_STATUS_READABLE             , AVIS_X_SAMPLE_STATUS_READABLE              },  // X-Sample-Status-Readable 
    { IDH_X_CONTENT_STRIP                      , AVIS_X_CONTENT_STRIP                       },  // X-Content-Strip
    { IDH_X_ERROR                              , AVIS_X_SAMPLE_ERROR                        },  // X-Error
    { IDH_X_ALERT_STATUS                       , QSITEMINFO_X_ALERT_STATUS                  },  // X-Alert-Status
    { IDH_X_DATE_ALERT                         , QSITEMINFO_X_DATE_ALERT                    },  // X-Date-Alert
    { IDH_X_ALERT_RESULT                       , QSITEMINFO_X_ALERT_RESULT                  },  // X-zAlert-Result
    { IDH_X_ALERT_STATUS_TIMER                 , QSITEMINFO_X_ALERT_STATUS_TIMER            },  // X-zAlert-Status-Timer
    { IDH_X_DATE_SAMPLE_STATUS                 , AVIS_X_DATE_SAMPLE_STATUS                  },  // X_Date_Sample_Status
    { IDH_X_SUBMISSION_COUNT                   , AVIS_X_SUBMISSION_COUNT                    },  // X-Submission-Count 


    { 0xFFFF, NULL },
    { 0, 0 }
};








