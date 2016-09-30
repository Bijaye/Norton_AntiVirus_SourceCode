/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

//////////////////////////////////////////////////////////////////////
// 
// This file contains three tables that can be used to map text 
// tokens used by the IcePack agent into display strings:
// 
//    The IcePackAttentionTable table contains tokens used in the
//    "attention" registry variable.
// 
//    The IcePackErrorTable table contains tokens used in the 
//    X-Error attribute of samples in quarantine.
//
//    The IcePackStateTable table contains tokens used in the
//    X-Analsysis-State attributes of samples in quarantine.
// 
// Each entry in each table has this format:
// 
//    L"token=national language message."
// 
// The "token" values and the "equals" characters are not displayed 
// by Quarantine Console.  They must not be changed when this file
// is translated into other national languages.
//
// The "national language message" values are displayed by Quarantine
// Console.  They should be changed when this file is translated
// into other national languages.
// 
//                             -- Edward Pring <pring@watson.ibm.com>
// 
//////////////////////////////////////////////////////////////////////

#define UNICODE

//--------------------------------------------------------------------
// This include file contains all of the POSIX and Windows include 
// files required by this program.
//--------------------------------------------------------------------

#include "StdAfx.h"

//--------------------------------------------------------------------
// This include file declares the IcePack token tables defined below
// as exported variables for the IcePackTokens.DLL file we are
// building.
//--------------------------------------------------------------------

#define ICEPACKTOKENS_EXPORTS

#include  <tchar.h>    
#include  <wchar.h>      
#include  <string.h>      
#include  "IcePackTokenRc.h"
#include  "IcePackTokens.h"


#undef  _T
#define _T(x)      L ## x



// LOCAL
void LoadStringTable();



#define TOKEN_MAX_WIDTH         30
#define STRING_MAX_WIDTH        250

#define ATTN_TABLE_MAX_ROWS     26
#define ERROR_TABLE_MAX_ROWS    25
#define STATE_TABLE_MAX_ROWS    59
#define STATUS_TABLE_MAX_ROWS   55  // changed to 30 from 25 tmarles 8-4-03
#define RESULT_TABLE_MAX_ROWS   20

//define TABLE_MAX_WIDTH         150
//WCHAR  szAttnTable  [ATTN_TABLE_MAX_ROWS+1][TABLE_MAX_WIDTH]  = {0};
//WCHAR  szErrorTable [ERROR_TABLE_MAX_ROWS+1][TABLE_MAX_WIDTH] = {0};
//WCHAR  szStateTable [STATE_TABLE_MAX_ROWS+1][TABLE_MAX_WIDTH] = {0};
//WCHAR  szStatusTable[STATUS_TABLE_MAX_ROWS+1][TABLE_MAX_WIDTH]= {0};
//WCHAR  szResultTable[RESULT_TABLE_MAX_ROWS+1][TABLE_MAX_WIDTH]= {0};


typedef struct
{
   WCHAR  szToken[TOKEN_MAX_WIDTH];
   WCHAR  szString[STRING_MAX_WIDTH];

}  AVIS_STRING_TABLE;

AVIS_STRING_TABLE  AttnStrTable[ATTN_TABLE_MAX_ROWS+1]     = {0};
AVIS_STRING_TABLE  ErrorStrTable[ERROR_TABLE_MAX_ROWS+1]   = {0};
AVIS_STRING_TABLE  StateStrTable[STATE_TABLE_MAX_ROWS+1]   = {0};
AVIS_STRING_TABLE  StatusStrTable[STATUS_TABLE_MAX_ROWS+1] = {0};
AVIS_STRING_TABLE  ResultStrTable[RESULT_TABLE_MAX_ROWS+1] = {0};

                
typedef struct
{
   DWORD dwGlobalID;
   DWORD dwLocalID;
   WCHAR szString[STRING_MAX_WIDTH];
}  AVIS_RESID_MAP;

AVIS_RESID_MAP  StrTableResIdMap[] = 
{
    IDSTABLE_DEFCAST_EVENTNAME              , IDS_STAB_DEFCAST_EVENTNAME              , L"" , 
    IDSTABLE_DISK_EVENTNAME                 , IDS_STAB_DISK_EVENTNAME                 , L"" ,
    IDSTABLE_SCANEXPLICIT_EVENTNAME         , IDS_STAB_SCANEXPLICIT_EVENTNAME         , L"" ,
    IDSTABLE_TARGET_EVENTNAME               , IDS_STAB_TARGET_EVENTNAME               , L"" ,
    IDSTABLE_GATEWAY_COMM_EVENTNAME         , IDS_STAB_GATEWAY_COMM_EVENTNAME         , L"" ,
    IDSTABLE_BLESSED_DEFS_EVENTNAME         , IDS_STAB_BLESSED_DEFS_EVENTNAME         , L"" ,
    IDSTABLE_UNBLESSED_DEFS_EVENTNAME       , IDS_STAB_UNBLESSED_DEFS_EVENTNAME       , L"" ,
    IDSTABLE_SAMPLE_ATTENTION_EVENTNAME     , IDS_STAB_SAMPLE_ATTENTION_EVENTNAME     , L"" ,
    IDSTABLE_SAMPLE_HELD_EVENTNAME          , IDS_STAB_SAMPLE_HELD_EVENTNAME          , L"" ,
    IDSTABLE_SAMPLE_ERROR_EVENTNAME         , IDS_STAB_SAMPLE_ERROR_EVENTNAME         , L"" ,
    IDSTABLE_SAMPLE_NOTINSTALLED_EVENTNAME  , IDS_STAB_SAMPLE_NOTINSTALLED_EVENTNAME  , L"" ,
    IDSTABLE_SAMPLE_AVAILABLE_EVENTNAME     , IDS_STAB_SAMPLE_AVAILABLE_EVENTNAME     , L"" ,
    IDSTABLE_SAMPLE_QUARANTINED_EVENTNAME   , IDS_STAB_SAMPLE_QUARANTINED_EVENTNAME   , L"" ,
    IDSTABLE_SAMPLE_SUBMITTED_EVENTNAME     , IDS_STAB_SAMPLE_SUBMITTED_EVENTNAME     , L"" ,
    IDSTABLE_SAMPLE_RELEASED_EVENTNAME      , IDS_STAB_SAMPLE_RELEASED_EVENTNAME      , L"" ,
    IDSTABLE_DEF_NEEDED_EVENTNAME           , IDS_STAB_DEF_NEEDED_EVENTNAME           , L"" ,
    IDSTABLE_DEF_DISTRIBUTED_EVENTNAME      , IDS_STAB_DEF_DISTRIBUTED_EVENTNAME      , L"" ,
    IDSTABLE_DEF_DISTRIBUTE_EVENTNAME       , IDS_STAB_DEF_DISTRIBUTE_EVENTNAME       , L"" ,
    IDSTABLE_Q_NOT_REPAIRED_EVENTNAME       , IDS_STAB_Q_NOT_REPAIRED_EVENTNAME       , L"" ,
    IDSTABLE_SEND_TEST_EVENTNAME            , IDS_STAB_SEND_TEST_EVENTNAME            , L"" ,
    IDSTABLE_GENERAL_INFO_EVENTNAME         , IDS_STAB_GENERAL_INFO_EVENTNAME         , L"" ,
    IDSTABLE_SHUTDOWN_EVENTNAME             , IDS_STAB_SHUTDOWN_EVENTNAME             , L"" ,
    IDSTABLE_UNAVAILABLE_EVENTNAME          , IDS_STAB_UNAVAILABLE_EVENTNAME          , L"" ,


    // MISC STRING IDs 
    IDSTABLE_TEXT_SEND_TESTEVENT            , IDS_STAB_TEXT_SEND_TESTEVENT            , L"" ,
    IDSTABLE_TEXT_UNABLE_TO_DISTRIBUTE_DEFS , IDS_STAB_TEXT_UNABLE_TO_DISTRIBUTE_DEFS , L"" ,
    IDSTABLE_MSGTITLE_GENERAL_ATTENTION     , IDS_STAB_MSGTITLE_GENERAL_ATTENTION     , L"" ,
    IDSTABLE_MSGTITLE_GATEWAY_ERROR         , IDS_STAB_MSGTITLE_GATEWAY_ERROR         , L"" ,
    IDSTABLE_MSGTITLE_SAMPLE_ERROR          , IDS_STAB_MSGTITLE_SAMPLE_ERROR          , L"" ,
    IDSTABLE_MSGTITLE_DEF_ERROR1            , IDS_STAB_MSGTITLE_DEF_ERROR1            , L"" ,
    IDSTABLE_MSGTITLE_DEF_ERROR2            , IDS_STAB_MSGTITLE_DEF_ERROR2            , L"" ,
    IDSTABLE_MSGTITLE_INFORMATIONAL         , IDS_STAB_MSGTITLE_INFORMATIONAL         , L"" ,
    IDSTABLE_TEXT_SEQUENCE_                 , IDS_STAB_TEXT_SEQUENCE_                 , L"" ,
    IDSTABLE_HEADING_USER                   , IDS_STAB_HEADING_USER                   , L"" ,
    IDSTABLE_HEADING_QSERVER                , IDS_STAB_HEADING_QSERVER                , L"" ,
    IDSTABLE_HEADING_COMPUTERNAME           , IDS_STAB_HEADING_COMPUTERNAME           , L"" ,
    IDSTABLE_HEADING_ADDRESS                , IDS_STAB_HEADING_ADDRESS                , L"" ,
    IDSTABLE_HEADING_DNSNAME                , IDS_STAB_HEADING_DNSNAME                , L"" ,
    IDSTABLE_HEADING_VIRUSNAME              , IDS_STAB_HEADING_VIRUSNAME              , L"" ,
    IDSTABLE_HEADING_DEFINITIONS            , IDS_STAB_HEADING_DEFINITIONS            , L"" ,
    IDSTABLE_HEADING_FILENAME               , IDS_STAB_HEADING_FILENAME               , L"" ,
    IDSTABLE_HEADING_UNKNOWN                , IDS_STAB_HEADING_UNKNOWN                , L"" ,
    IDSTABLE_STATUS_QUARANTINED             , IDS_STAB_STATUS_QUARANTINED             , L"" ,
    IDSTABLE_STATUS_SUBMITTED               , IDS_STAB_STATUS_SUBMITTED               , L"" ,
    IDSTABLE_STATUS_HELD                    , IDS_STAB_STATUS_HELD                    , L"" ,
    IDSTABLE_STATUS_RELEASED                , IDS_STAB_STATUS_RELEASED                , L"" ,
    IDSTABLE_STATUS_UNNEEDED                , IDS_STAB_STATUS_UNNEEDED                , L"" ,
    IDSTABLE_STATUS_NEEDED                  , IDS_STAB_STATUS_NEEDED                  , L"" ,
    IDSTABLE_STATUS_AVAILABLE               , IDS_STAB_STATUS_AVAILABLE               , L"" ,
    IDSTABLE_STATUS_DISTRIBUTE              , IDS_STAB_STATUS_DISTRIBUTE              , L"" ,
    IDSTABLE_STATUS_DISTRIBUTED             , IDS_STAB_STATUS_DISTRIBUTED             , L"" ,
    IDSTABLE_STATUS_INSTALLED               , IDS_STAB_STATUS_INSTALLED               , L"" ,
    IDSTABLE_STATUS_ATTENTION               , IDS_STAB_STATUS_ATTENTION               , L"" ,
    IDSTABLE_STATUS_ERROR                   , IDS_STAB_STATUS_ERROR                   , L"" ,
    IDSTABLE_STATUS_NOTINSTALLED            , IDS_STAB_STATUS_NOTINSTALLED            , L"" ,
    IDSTABLE_STATUS_RESTART                 , IDS_STAB_STATUS_RESTART                 , L"" ,
    IDSTABLE_STATUS_LEGACY                  , IDS_STAB_STATUS_LEGACY                  , L"" ,
    IDSTABLE_STATUS_UNKNOWN                 , IDS_STAB_STATUS_UNKNOWN                 , L"" ,
    // added 8-4-03 tmarles
    IDSTABLE_STATUS_RISK	                , IDS_STAB_STATUS_RISK		              , L"" ,
    IDSTABLE_STATUS_HACKTOOL                , IDS_STAB_STATUS_HACKTOOL	              , L"" ,
    IDSTABLE_STATUS_SPYWARE	                , IDS_STAB_STATUS_SPYWARE	              , L"" ,
    IDSTABLE_STATUS_TRACKWARE               , IDS_STAB_STATUS_TRACKWARE	              , L"" ,
    IDSTABLE_STATUS_DIALER	                , IDS_STAB_STATUS_DIALER	              , L"" ,
    IDSTABLE_STATUS_REMOTE	                , IDS_STAB_STATUS_REMOTE	              , L"" ,
    IDSTABLE_STATUS_ADWARE	                , IDS_STAB_STATUS_ADWARE	              , L"" ,
    IDSTABLE_STATUS_PRANK	                , IDS_STAB_STATUS_PRANK		              , L"" ,
    // end change 8-4-03 tmarles
    IDSTABLE_HEADING_STATUS                 , IDS_STAB_HEADING_STATUS                 , L"" ,
    IDSTABLE_SAMPLE_INSTATE_TOO_LONG        , IDS_STAB_SAMPLE_INSTATE_TOO_LONG        , L"" ,
    IDSTABLE_GENERAL_INSTATE_TOO_LONG1      , IDS_STAB_GENERAL_INSTATE_TOO_LONG1      , L"" ,
    IDSTABLE_GENERAL_INSTATE_TOO_LONG2      , IDS_STAB_GENERAL_INSTATE_TOO_LONG2      , L"" ,
    IDSTABLE_DISKQUOTA_LOW_EVENTNAME_ID     , IDS_DISKQUOTA_LOW_EVENTID               , L"" ,
    IDSTABLE_DISKQUOTA_QUARANTINE_FULL      , IDS_DISKQUOTA_CQ_FULL_EVENTID           , L"" ,
    IDSTABLE_DISKSPACE_LOW_EVENTNAME_ID     , IDS_DISKSPACE_LOW_EVENTID               , L"" ,
    IDSTABLE_DISKSPACE_LOW_TEXTID           , IDS_DISKSPACE_LOW_TEXTID                , L"" ,
    IDSTABLE_DISKQUOTA_LOW_TEXTID           , IDS_DISKQUOTA_LOW_TEXTID                , L"" ,
    IDSTABLE_MSGTITLE_DISK_WARNING          , IDS_STAB_MSGTITLE_DISK_WARNING          , L"" ,
    IDSTABLE_HELPTEXT_ATTN_SCANEXPLICIT     , IDS_HELPTEXT_ATTN_SCANEXPLICIT          , L"" ,
    IDSTABLE_HELPTEXT_ATTN_DISK             , IDS_HELPTEXT_ATTN_DISK                  , L"" ,
    IDSTABLE_HELPTEXT_ATTN_DEFCAST          , IDS_HELPTEXT_ATTN_DEFCAST               , L"" ,
    IDSTABLE_HELPTEXT_ATTN_TARGET           , IDS_HELPTEXT_ATTN_TARGET                , L"" ,
    IDSTABLE_HELPTEXT_ATTN_GATEWAY          , IDS_HELPTEXT_ATTN_GATEWAY               , L"" ,
    IDSTABLE_QS_SAGE_FORMAT                 , IDS_QS_SAGE_FORMAT                      , L"" ,
    IDSTABLE_QUARANTINED_STRING             , IDS_QUARANTINED_STRING                  , L"" ,
    IDSTABLE_HEADING_SAMPLE_STATE           , IDS_STAB_HEADING_SAMPLE_STATE           , L"" ,
    IDSTABLE_NEW_BLESSED_DEFS_TEXTID        , IDS_NEW_BLESSED_DEFS_TEXTID             , L"" ,
    IDSTABLE_NEW_UNBLESSED_DEFS_TEXTID      , IDS_NEW_UNBLESSED_DEFS_TEXTID           , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_NOTINSTALLED   , IDS_HELPTEXT_SAMPLE_NOTINSTALLED        , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_AVAILABLE      , IDS_HELPTEXT_SAMPLE_AVAILABLE           , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_QUARANTINED    , IDS_HELPTEXT_SAMPLE_QUARANTINED         , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_SUBMITTED      , IDS_HELPTEXT_SAMPLE_SUBMITTED           , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_RELEASED       , IDS_HELPTEXT_SAMPLE_RELEASED            , L"" ,
    IDSTABLE_HELPTEXT_DEF_NEEDED            , IDS_HELPTEXT_DEF_NEEDED                 , L"" ,
    IDSTABLE_HELPTEXT_DEF_DISTRIBUTE        , IDS_HELPTEXT_DEF_DISTRIBUTE             , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_HELD           , IDS_HELPTEXT_SAMPLE_HELD                , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_DISTRIBUTED    , IDS_HELPTEXT_SAMPLE_DISTRIBUTED         , L"" ,
    IDSTABLE_STAB_HEADING_DEFINITIONS_NEEDED, IDS_STAB_HEADING_DEFINITIONS_NEEDED     , L"" ,
    IDSTABLE_STAB_TEXT_VERSION_NUMBER       , IDS_STAB_TEXT_VERSION_NUMBER            , L"" ,
    IDSTABLE_HELPTEXT_DEF_FTP_ADDRESS       , IDS_HELPTEXT_DEF_FTP_ADDRESS            , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_NOTINSTALLED2  , IDS_HELPTEXT_SAMPLE_NOTINSTALLED2       , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_NOTREPAIRED    , IDS_HELPTEXT_SAMPLE_NOTREPAIRED         , L"" ,

    // ADD NEW STRING IDs HERE   
    IDSTABLE_HELPTEXT_ATTN_SHUTDOWN         , IDS_HELPTEXT_ATTN_SHUTDOWN              , L"" ,
    IDSTABLE_HELPTEXT_ATTN_UNAVAILABLE      , IDS_HELPTEXT_ATTN_UNAVAILABLE           , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_ENCRYPTED      , IDS_HELPTEXT_SAMPLE_ENCRYPTED           , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_RESTORE        , IDS_HELPTEXT_SAMPLE_RESTORE             , L"" ,
    IDSTABLE_HELPTEXT_SAMPLE_DELETE         , IDS_HELPTEXT_SAMPLE_DELETE              , L"" ,




    0,0,L"" ,
    0,0,L"" ,
    0,0,L""
};




HINSTANCE g_hInstance = 0;   



/*----------------------------------------------------------------------------
    MiscStrTableLoadStrings()

    Load the misc purpose string table. AVIS_RESID_MAP  StrTableResIdMap[]

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL MiscStrTableLoadStrings()
{
    AVIS_RESID_MAP*  pStrTableRow = NULL;
    int    rc      = 0;
	TCHAR sztTemp[STRING_MAX_WIDTH];
    try
    {
        pStrTableRow = &StrTableResIdMap[0];

        // MAP FROM GLOBAL STRING ID TO ACTUAL STRING
        for( int i = 0; ; i++ )
        {
            // ARE WE AT THE END OF THE TABLE?
            if( pStrTableRow->dwGlobalID == 0 )
                return(TRUE);

            // LOAD THE STRING
            rc = LoadString( g_hInstance, pStrTableRow->dwLocalID, sztTemp, STRING_MAX_WIDTH ); 
			MultiByteToWideChar(CP_ACP,0,sztTemp,sizeof(sztTemp),pStrTableRow->szString,sizeof(pStrTableRow->szString));

            // NEXT
            pStrTableRow++;
        }

    }
    catch(...)
    {
    }
    return(rc);
}


/*----------------------------------------------------------------------------
    StrTableLookUpMiscString()

    Returns ptr to the staic string in the resource dll

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
ICEPACKTOKENS_API WCHAR *StrTableLookUpMiscString( DWORD dwGlobalResID )
{
    WCHAR* pszString = NULL;  
    DWORD  dwStrID   = 0;

    try
    {
        if( dwGlobalResID == 0 )
            return(NULL);

        // MAP FROM GLOBAL STRING ID TO STRING
        for( int i = 0; ; i++ )
        {
            // ARE WE AT THE END OF THE TABLE?
            if( (dwStrID = StrTableResIdMap[i].dwGlobalID) == 0 )
                return(NULL);

            // DID WE FIND IT?
            if( dwStrID == dwGlobalResID )
            {
                pszString = StrTableResIdMap[i].szString;
                break;
            }
        }
    }
    catch(...)
    {
    }
    return( pszString );
}


/*----------------------------------------------------------------------------
    StrTableLoadString()

    Using passed global ID, lookup the string from the local RC file.
    Called from Qserver, Qconsole, AvisConsole

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
ICEPACKTOKENS_API int _StrTableLoadString( DWORD dwGlobalResID, LPWSTR lpBuffer, int nBufferMax )
{
    int    rc      = 0;
    DWORD  dwStrID = 0;
    UINT   uID     = 0;

    try
    {
        if( lpBuffer == NULL )
            return(0);
        *lpBuffer = 0;
        
        // MAP FROM GLOBAL STRING ID TO LOCAL ID
        for( int i = 0; ; i++ )
        {
            // ARE WE AT THE END OF THE TABLE?
            if( (dwStrID = StrTableResIdMap[i].dwGlobalID) == 0 )
                return(0);
            // DID WE FIND IT?
            if( dwStrID == dwGlobalResID )
            {
                uID = StrTableResIdMap[i].dwLocalID;
                break;
            }
        }
        
        rc = LoadStringW( g_hInstance, uID, lpBuffer, nBufferMax ); 
    }
    catch(...)
    {
    }
    return(rc);
}


/*----------------------------------------------------------------------------
    DllMain()
 ----------------------------------------------------------------------------*/
BOOL WINAPI DllMain(   HINSTANCE hInstance, 
                       DWORD     ul_reason_for_call,
                       LPVOID    lpReserved
					 )
{
    g_hInstance   = hInstance;

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
            OutputDebugStringW( L"IcePackTokens dll loaded\r\n" );  
            LoadStringTable();
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


/*----------------------------------------------------------------------------
    LoadTokenAndString()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void LoadTokenAndString( WCHAR *pszToken, DWORD dwResID, AVIS_STRING_TABLE* pTableRow )
{
    wcscpy( pTableRow->szToken, pszToken );
    LoadStringW( g_hInstance, dwResID, pTableRow->szString, STRING_MAX_WIDTH ); 

    return;
}


/*----------------------------------------------------------------------------
    LoadStringTable()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void LoadStringTable()
{
    int i = 0;

    // LOAD ATTENTION TABLE
    i = 0;
    LoadTokenAndString( TOKEN_DEFCAST            , IDS_ATTN_DEFCAST            , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_DISK               , IDS_ATTN_DISK               , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_DOWNLOAD_GATEWAY   , IDS_ATTN_DOWNLOAD           , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_QUARANTINE         , IDS_ATTN_QUARANTINE         , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_QUERY_GATEWAY      , IDS_ATTN_QUERY              , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_SCANEXPLICIT       , IDS_ATTN_SCANEXPLICIT       , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_SUBMIT_GATEWAY     , IDS_ATTN_SUBMIT             , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_TARGET             , IDS_ATTN_TARGET             , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_DISKQUOTA_LOW_WATER, IDS_DISKQUOTA_LOW_ERROR     , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_DISKSPACE_LOW_WATER, IDS_DISKSPACE_LOW_ERROR     , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_QUARANTINE_FULL    , IDS_DISKQUOTA_CQ_FULL_ERROR , &AttnStrTable[i++] );
    LoadTokenAndString( TOKEN_SHUTDOWN           , IDS_ATTN_SHUTDOWN           , &AttnStrTable[i++] );
    // ADDED 6/19/00
    LoadTokenAndString( TOKEN_UNAVAILABLE        , IDS_ATTN_UNAVAILABLE        , &AttnStrTable[i++] );


    // LOAD ERROR TABLE
    i = 0;
    LoadTokenAndString( TOKEN_ABANDONED         , IDS_ERROR_ABANDONED         , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_CONTENT           , IDS_ERROR_CONTENT           , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_CRUMBLED          , IDS_ERROR_CRUMBLED          , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_DECLINED          , IDS_ERROR_DECLINED          , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_INTERNAL          , IDS_ERROR_INTERNAL          , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_LOST              , IDS_ERROR_LOST              , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_MALFORMED         , IDS_ERROR_MALFORMED         , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_MISSING           , IDS_ERROR_MISSING           , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_OVERRUN           , IDS_ERROR_OVERRUN           , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_SAMPLE            , IDS_ERROR_SAMPLE            , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_SUPERCEDED        , IDS_ERROR_SUPERCEEDED       , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_TYPE              , IDS_ERROR_TYPE              , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_UNAVAILABLE       , IDS_ERROR_UNAVAILABLE       , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_UNDERRUN          , IDS_ERROR_UNDERRUN          , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_UNPACKAGE         , IDS_ERROR_UNPACKAGE         , &ErrorStrTable[i++] );
    LoadTokenAndString( TOKEN_UNPUBLISHED       , IDS_ERROR_UNPUBLISHED       , &ErrorStrTable[i++] );
                                                                                   
    // LOAD STATUS TABLE                                                           
    i = 0;                                                                         
    LoadTokenAndString( TOKEN_QUARANTINED       , IDS_STATUS_QUARANTINED      , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_SUBMITTED         , IDS_STATUS_SUBMITTED        , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_HELD              , IDS_STATUS_HELD             , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_RELEASED          , IDS_STATUS_RELEASED         , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_UNNEEDED          , IDS_STATUS_UNNEEDED         , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_NEEDED            , IDS_STATUS_NEEDED           , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_AVAILABLE         , IDS_STATUS_AVAILABLE        , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_DISTRIBUTED       , IDS_STATUS_DISTRIBUTED      , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_INSTALLED         , IDS_STATUS_INSTALLED        , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_ATTENTION         , IDS_STATUS_ATTENTION        , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_ERROR             , IDS_STATUS_ERROR            , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_NOTINSTALLED      , IDS_STATUS_NOTINSTALLED     , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_RESTART           , IDS_STATUS_RESTART          , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_DISTRIBUTE        , IDS_STATUS_DISTRIBUTE       , &StatusStrTable[i++] );
    // added 8-4-03 tmarles
	LoadTokenAndString( TOKEN_RISK		        , IDS_STATUS_RISK		      , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_HACKTOOL	        , IDS_STATUS_HACKTOOL	      , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_SPYWARE	        , IDS_STATUS_SPYWARE	      , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_TRACKWARE	        , IDS_STATUS_TRACKWARE	      , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_DIALER	        , IDS_STATUS_DIALER		      , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_REMOTE	        , IDS_STATUS_REMOTE		      , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_ADWARE	        , IDS_STATUS_ADWARE		      , &StatusStrTable[i++] );
    LoadTokenAndString( TOKEN_PRANK		        , IDS_STATUS_PRANK		      , &StatusStrTable[i++] );
                                                                                   
    // LOAD RESULT TABLE                                                           
    i = 0;                                                                         
    LoadTokenAndString( TOKEN_BADREPAIR         , IDS_RESULT_BADREPAIR        , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_BADSCAN           , IDS_RESULT_BADSCAN          , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_HEURISTIC         , IDS_RESULT_HEURISTIC        , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_NODETECT          , IDS_RESULT_NODETECT         , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_NOREPAIR          , IDS_RESULT_NOREPAIR         , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_OVERRUN           , IDS_RESULT_OVERRUN          , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_REPAIRED          , IDS_RESULT_REPAIRED         , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_UNDERRUN          , IDS_RESULT_UNDERRUN         , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_UNREPAIRABLE      , IDS_RESULT_UNREPAIRABLE     , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_UNINFECTABLE      , IDS_RESULT_UNINFECTABLE     , &ResultStrTable[i++] );
    LoadTokenAndString( TOKEN_UNSUBMITTABLE     , IDS_RESULT_UNSUBMITTABLE    , &ResultStrTable[i++] );

    // LOAD STATE TABLE
    i = 0;
    LoadTokenAndString( TOKEN_CAPTURED          , IDS_STATE_CAPTURED          , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_SUBMITTING        , IDS_STATE_SUBMITTING        , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_RECEIVING         , IDS_STATE_RECEIVING         , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_ACCEPTED          , IDS_STATE_ACCEPTED          , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_IMPORTING         , IDS_STATE_IMPORTING         , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_IMPORTED          , IDS_STATE_IMPORTED          , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_CLASSIFYING       , IDS_STATE_CLASSIFYING       , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_RESCAN            , IDS_STATE_RESCAN            , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_BINARY            , IDS_STATE_BINARY            , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_BINARYCONTROLLING , IDS_STATE_BINARYCONTROLLING , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_BINARYWAIT        , IDS_STATE_BINARYWAIT        , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_BINARYREPLICATING , IDS_STATE_BINARYREPLICATING , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_BINARYSCORING     , IDS_STATE_BINARYSCORING     , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_MACRO             , IDS_STATE_MACRO             , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_MACROCONTROLLING  , IDS_STATE_MACROCONTROLLING  , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_MACROWAIT         , IDS_STATE_MACROWAIT         , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_MACROREPLICATING  , IDS_STATE_MACROREPLICATING  , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_MACROSCORING      , IDS_STATE_MACROSCORING      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_SIGNATURES        , IDS_STATE_SIGNATURES        , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_LOCKING           , IDS_STATE_LOCKING           , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_INCRBUILDING      , IDS_STATE_INCRBUILDING      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_INCRUNITTESTING   , IDS_STATE_INCRUNITTESTING   , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_FULLBUILDING      , IDS_STATE_FULLBUILDING      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_FULLUNITTESTING   , IDS_STATE_FULLUNITTESTING   , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_UNLOCKING         , IDS_STATE_UNLOCKING         , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_DEFERRING         , IDS_STATE_DEFERRING         , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_DEFERRED          , IDS_STATE_DEFERRED          , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_DEFER             , IDS_STATE_DEFER             , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_ARCHIVE           , IDS_STATE_ARCHIVE           , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_ARCHIVING         , IDS_STATE_ARCHIVING         , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_ATTENTION         , IDS_STATE_ATTENTION         , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_EMPTY             , IDS_STATE_EMPTY             , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_ERROR             , IDS_STATE_ERROR             , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_UNINFECTABLE      , IDS_STATE_UNINFECTABLE      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_UNSUBMITABLE      , IDS_STATE_UNSUBMITABLE      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_NODETECT          , IDS_STATE_NODETECT          , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_INFECTED          , IDS_STATE_INFECTED          , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_UNINFECTED        , IDS_STATE_UNINFECTED        , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_MISFIRED          , IDS_STATE_MISFIRED          , &StateStrTable[i++] );
    // ADDED 4/27/00
    LoadTokenAndString( TOKEN_LOST              , IDS_ERROR_LOST              , &StateStrTable[i++] );
    // ADDED 6/19/00
    LoadTokenAndString( TOKEN_NOREPAIR          , IDS_STATE_NOREPAIR          , &StateStrTable[i++] );
    // ADDED 6/28/00
    LoadTokenAndString( TOKEN_WIN32             , IDS_STATE_WIN32             , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_WIN32CONTROLLING  , IDS_STATE_WIN32CONTROLLING  , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_WIN32WAIT         , IDS_STATE_WIN32WAIT         , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_WIN32REPLICATING  , IDS_STATE_WIN32REPLICATING  , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_WIN32SCORING      , IDS_STATE_WIN32SCORING      , &StateStrTable[i++] );
    // ADDED 6/30/00
    LoadTokenAndString( TOKEN_CONTROLLING       , IDS_STATE_CONTROLLING       , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_WAIT              , IDS_STATE_WAIT              , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_REPLICATING       , IDS_STATE_REPLICATING       , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_SCORING           , IDS_STATE_SCORING           , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_ENCRYPTED         , IDS_STATE_ENCRYPTED         , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_RESTORE           , IDS_STATE_RESTORE           , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_DELETE            , IDS_STATE_DELETE            , &StateStrTable[i++] );
         
	LoadTokenAndString( TOKEN_RISK		        , IDS_STATUS_RISK		      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_HACKTOOL	        , IDS_STATUS_HACKTOOL	      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_SPYWARE	        , IDS_STATUS_SPYWARE	      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_TRACKWARE	        , IDS_STATUS_TRACKWARE	      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_DIALER	        , IDS_STATUS_DIALER		      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_REMOTE	        , IDS_STATUS_REMOTE		      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_ADWARE	        , IDS_STATUS_ADWARE		      , &StateStrTable[i++] );
    LoadTokenAndString( TOKEN_PRANK		        , IDS_STATUS_PRANK		      , &StateStrTable[i++] );


    // LOAD MISC STRING TABLE
    MiscStrTableLoadStrings();

    return;
}


/*----------------------------------------------------------------------------
   LookUpIcePackTokenString()

   Using the incoming "token" from IcePack, lookup the coresponding 
   string in IcePackTokens.dll, which is a resource only Dll.

   


   IcePackTokens.dll contains three tables that can be used to map  
   text tokens used by the IcePack agent into display strings:
   
      The IcePackAttentionTable table contains tokens used in the
      "attention" registry variable.
   
      The IcePackErrorTable table contains tokens used in the 
      X-Error attribute of samples in quarantine.
   
      The IcePackStateTable table contains tokens used in the
      X-Analsysis-State attributes of samples in quarantine.
   
      The IcePackStatusTable



   Written by: Jim Hill                         size_t 
----------------------------------------------------------------------------*/
ICEPACKTOKENS_API WCHAR *LookUpIcePackTokenString(WCHAR *lpszToken, DWORD dwTableID)
{
    AVIS_STRING_TABLE*  pStrTable = NULL;
    DWORD               dwTokenLength   = 0;
    WCHAR*              lpszString      = NULL;
    DWORD               dwMaxRow = 0;

    try
    {
        // VALIDATE
        if( lpszToken == NULL || *lpszToken == 0 )
            return(NULL);
    
        // GET THE RIGHT TABLE
        switch( dwTableID )
        {
            case ICEPACK_TOKEN_ATTENTION_TABLE :
                pStrTable   = &AttnStrTable[0];
                dwMaxRow    = ATTN_TABLE_MAX_ROWS;
                break;
    
            case ICEPACK_TOKEN_ERROR_TABLE :
                pStrTable   = &ErrorStrTable[0];
                dwMaxRow    = ERROR_TABLE_MAX_ROWS;
                break;
    
            case ICEPACK_TOKEN_STATE_TABLE :
                pStrTable   = &StateStrTable[0];
                dwMaxRow    = STATE_TABLE_MAX_ROWS;
                break;
    
            case ICEPACK_TOKEN_STATUS_TABLE:
                pStrTable   = &StatusStrTable[0];
                dwMaxRow    = STATUS_TABLE_MAX_ROWS;
                break;
    
            case ICEPACK_TOKEN_RESULT_TABLE:
                pStrTable   = &ResultStrTable[0];
                dwMaxRow    = RESULT_TABLE_MAX_ROWS;
                break;
    
            default:
                return(lpszToken);  
        }
    
        // LOOK FOR A MATCH
         dwTokenLength = wcslen(lpszToken);
         for( int i = 0; i < (int) dwMaxRow && pStrTable->szToken[0] != 0; i++ )
         {
             if( wcsnicmp( lpszToken, pStrTable->szToken, dwTokenLength ) == 0 )
             {
                 if( wcslen(pStrTable->szToken) == dwTokenLength )
                    break;
             }
    
             pStrTable++;
         }
    
        // IF NO ENTRY FOUND, RETURN NULL
        if( pStrTable->szToken[0] == 0 )
            lpszString = NULL;       // lpszToken;
        else
            lpszString = pStrTable->szString;

    }
    catch(...)
    {
        lpszString = NULL;
    }
    return( lpszString );
}


















//--------------------------------------------------------------------
// This table contains all of the tokens that may appear in the
// "attention" registry variable.
//--------------------------------------------------------------------

extern ICEPACKTOKENS_API const wchar_t* const IcePackAttentionTable[] = 
{
  L"defcast=defcast: IcePack cannot connect to the DefCast component.",
  L"disk=disk: IcePack cannot access the definition or temporary directory.",
  L"download=download: IcePack cannot connect to the gateway to download definitions.",
  L"quarantine=quarantine: IcePack cannot connect to the Quarantine Service.",
  L"query=query: IcePack cannot connect to the gateway to query sample status.",
  L"scanexplicit=scanexplicit: IcePack cannot connect to the ScanExplicit Service.",
  L"submit=submit: IcePack cannot connect to the gateway to submit samples.",
  L"target=target: IcePack cannot install definitions on one or more machines.",
  NULL   
};

extern ICEPACKTOKENS_API const wchar_t* const IcePackErrorTable[] = 
{
  L"abandoned=abandoned: This signature sequence number has been abandoned." ,
  L"content=content: This sample's content checksum does not match its content.",
  L"crumbled=crumbled: This sample's cookie has not been assigned by the gateway.",
  L"declined=declined:  This sample has been declined by the gateway.",
  L"internal=internal: An internal failure occurred while processing this sample.",
  L"lost=lost: This sample was not completely received due to a network failure.",
  L"malformed=malformed: An essential attribute of this sample was malformed.",
  L"missing=missing: An essential attribute of this sample was missing.",
  L"overrun=overrun: The content of this sample exceeds its length.",
  L"sample=sample: This sample's sample checksum does not match its content.",
  L"superceded=superceded: This signature sequence number has been superceded.",
  L"type=type: This sample's type is not supported.",
  L"unavailable=unavailable: This signature sequence number has not been published.",
  L"underrun=underrun: The length of this sample exceeds its content.",
  L"unpackage=unpackage: The sample or signature could not be unpacked.",
  L"unpublished=unpublished: The signature set could not be published.",
  NULL
};

extern ICEPACKTOKENS_API const wchar_t* const IcePackStateTable[] = 
{
  // These are tranport states:
  L"captured=captured: This sample has been captured and stored in quarantine.",
  L"submitting=submitting: This sample is being submitted to the gateway.",
  L"receiving=receiving: This sample is being received by the gateway.",
  L"accepted=accepted: This sample has been accepted by the gateway.",
  L"importing=importing: This sample is being imported into the analysis center.",
  L"imported=imported: This sample has been imported into the analysis center.",

  // These are dataflow states:
  L"classifying=classifying: This sample is being classified to determine its datatype.",
  L"rescan=rescan: This sample will be re-scanned with newer virus definition files.",

  // These are binary analysis states:
  L"binary=binary: This sample has been classified as a binary program.",
  L"binaryControlling=binaryControlling: This sample's replication environments are being set.",
  L"binaryWait=binaryWait: This sample is waiting for replication engines.",
  L"binaryReplicating=binaryReplicating: This sample is being executed by a replication engine.",
  L"binaryScoring=binaryScoring: Signatures for a new virus are being scored.",

  // These are macro analysis states:
  L"macro=macro: This sample has been classified as a binary program.",
  L"macroControlling=macroControlling: This sample's replication environments are being set.",
  L"macroWait=macroWait: This sample is waiting for replication engines.",
  L"macroReplicating=macroReplicating: This sample is being executed by a replication engine.",
  L"macroScoring=macroScoring: Signatures for a new virus are being scored.",

  // These are build states:
  L"signatures=signatures: Signatures for a new virus have been selected.",
  L"locking=locking: The definition generation service is being locked.",
  L"incrBuilding=incrBuilding:  Virus definition files are being built.",
  L"incrUnitTesting=incrUnitTesting: Virus definition files are being tested.",
  L"fullBuilding=fullBuilding: Virus definition files are being built.",
  L"fullUnitTesting=fullUnitTesting: Virus definition files are being tested.",
  L"unlocking=unlocking: The definition generation service is being unlocked.",

  // These are deferral states:
  L"defer=defer: This sample will be deferred to experts for manual analysis.",
  L"deferred=deferred: This sample has been deferred to experts for manual analysis.",
  L"deferring=deferring: This sample is being deferred to experts for manual analysis.",

  // These are archive states:
  L"archive=archive: This sample's automated analysis files will be archived.",
  L"archiving=archiving: This sample's automated analysis files are being archived.",

  // These are final states:
  L"attention=attention: User intervention is required.",
  L"empty=empty: This sample is empty.",
  L"error=error: An error occurred while processing the sample.",
  L"uninfectable=uninfectable: This sample contains no executable code.",
  L"unsubmitable=unsubmitable: This sample cannot be submitted for analysis.",
  L"nodetect=nodetect: No suspicious code was detected in this sample.",
  L"infected=infected: This sample is infected with a virus that can be repaired.",
  L"uninfected=uninfected: This sample is not infected with a virus.",
  L"misfired=misfired: This sample is not infected, but did cause a false positive.",


  // added by tmarles 3-11-02
  L"encrypted=encrypted:This sample is encrypted or password-protected.",
  L"restore=restore:This file has been altered. You need to restore it from a clean backup or to delete it.",
  L"delete=delete:This file contains malicious code which cannot be removed or has been created by malicious software. You should delete this file.",
  L"risk=risk:  This file is not viral, but it can be used by an intruder to perform attacks and represents a security risk. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"hacktool=hacktool: This file can act as malware or help to create malware such as viruses or Trojan horses. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"spyware=spyware: This file monitors system activity such as keystokes and reports collected information to another system. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"trackware=trackware: This file tracks users' internet activity and reports collected information to another system. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"dialer=dialer: This file uses a modem to call fee-paying phone numbers or create rogue connections. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"remote=remote: This file allows remote users to gain access to systems on which it runs. It should be uninstalled or deleted if there is no legitimate need for it..",
  L"adware=adware: This file downloads and displays advertising content.  It should be uninstalled or deleted if there is no legitimate need for it.",
  L"prank=prank: This file is a joke program; it can alter or interrupt the operation of a system in a humorous way. It should be uninstalled or deleted if there is no legitimate need for it.",

  NULL
};


extern ICEPACKTOKENS_API const wchar_t* const IcePackStatusTable[] =
{
  L"quarantined=quarantined: This sample has been received by Central Quarantine.",
  L"submitted=submitted: This sample has been submitted for analysis.",
  L"held=held: This sample is held for submission to the analysis center.",
  L"released=released: This sample will be submitted to the analysis center.",
  L"unneeded=unneeded: New definitions are not needed on this sample's originator.",
  L"needed=needed: New definitions are needed for this sample.",
  L"available=available: New definitions are held for delivery to this sample's originator.",
  L"distributed=distributed: New definitions have been delivered to this sample's originator.",
  L"installed=installed: New definitions have been installed on this sample's originator.",
  L"attention=attention: Contact Symantec technical support for assistance with this sample.",
  L"error=error: An error occurred while processing this sample.",
  L"notinstalled=notinstalled: Definitions could not be delivered to this sample's originator.",
  L"restart=restart: This sample will be restarted.",
//L"legacy=legacy: This sample must be submitted manually via Scan&Deliver.",
  L"distribute=distribute: New definitions will be delivered to this sample's originator",
  // added 8-4-03 tmarles
  L"risk=risk:  This file is not viral, but it can be used by an intruder to perform attacks and represents a security risk. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"hacktool=hacktool: This file can act as malware or help to create malware such as viruses or Trojan horses. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"spyware=spyware: This file monitors system activity such as keystokes and reports collected information to another system. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"trackware=trackware: This file tracks users' internet activity and reports collected information to another system. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"dialer=dialer: This file uses a modem to call fee-paying phone numbers or create rogue connections. It should be uninstalled or deleted if there is no legitimate need for it.",
  L"remote=remote: This file allows remote users to gain access to systems on which it runs. It should be uninstalled or deleted if there is no legitimate need for it..",
  L"adware=adware: This file downloads and displays advertising content.  It should be uninstalled or deleted if there is no legitimate need for it.",
  L"prank=prank: This file is a joke program; it can alter or interrupt the operation of a system in a humorous way. It should be uninstalled or deleted if there is no legitimate need for it.",


  NULL
};


extern ICEPACKTOKENS_API const wchar_t* const IcePackResultTable[] =
{
  L"badrepair=badrepair: Repair engine failed while disinfecting this sample.",
  L"badscan=badscan: Scan engine failed while disinfecting this sample.",
  L"heuristic=heuristic=heuristic: This sample may be infected with an unknown virus.",
  L"nodetect=nodetect: No known viruses or suspicious code were found in this sample.",
  L"norepair=norepair: There are no repair instructions for the virus infecting this sample.",
  L"overrun=overrun: Repair engine outgrew buffer space for this sample.",
  L"repaired=repaired: This sample has been successfully disinfected.",
  L"underrun=underrun: Repair engine stepped outside buffer space for this sample.",
  L"unrepairable=unrepairable: This sample was not successfully disinfected.",
  L"uninfectable=uninfectable: This sample contains no code.",
  L"unsubmittable=unsubmittable: This sample may not be submitted for analysis.",
  NULL
};

















