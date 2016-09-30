// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*----------------------------------------------------------------------------
   IcePackTokens.h

   #IPE DO NOT TRANSLATE THIS FILE
 ----------------------------------------------------------------------------*/

#ifndef _AVIS_ICEPACKTOKENS_H_INCLUDED
#define	_AVIS_ICEPACKTOKENS_H_INCLUDED




// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ICEPACKTOKENS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ICEPACKTOKENS_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef ICEPACKTOKENS_EXPORTS
#define ICEPACKTOKENS_API __declspec(dllexport)
#else
#define ICEPACKTOKENS_API __declspec(dllimport)
#endif

//--------------------------------------------------------------------
// Declare the tables of IcePack tokens as either exported or imported
// variables for the IcePackTokens.DLL file, depending upon whether
// we are building the IcePackTokens.DLL file itself, or another
// program that is linked with it.           
//--------------------------------------------------------------------

extern ICEPACKTOKENS_API WCHAR *StrTableLookUpMiscString( DWORD dwGlobalResID );
extern ICEPACKTOKENS_API WCHAR *LookUpIcePackTokenString(WCHAR *lpszToken, DWORD dwTableID);
extern ICEPACKTOKENS_API int    _StrTableLoadString( DWORD dwGlobalResID, LPWSTR lpBuffer, int nBufferMax );




extern ICEPACKTOKENS_API const wchar_t* const IcePackAttentionTable[];
extern ICEPACKTOKENS_API const wchar_t* const IcePackErrorTable[];
extern ICEPACKTOKENS_API const wchar_t* const IcePackStateTable[];
extern ICEPACKTOKENS_API const wchar_t* const IcePackStatusTable[];
extern ICEPACKTOKENS_API const wchar_t* const IcePackResultTable[];



// LOOKUP ICEPACK STRINGS FROM TOKEN
#define ICEPACK_TOKEN_ATTENTION_TABLE    1
#define ICEPACK_TOKEN_ERROR_TABLE        2
#define ICEPACK_TOKEN_STATE_TABLE        3
#define ICEPACK_TOKEN_STATUS_TABLE       4
#define ICEPACK_TOKEN_RESULT_TABLE       5



// STRING TABLE DEFINES FOR USE BY QSERVER, QCONSOLE, AVISCONSOLE
#define IDSTABLE_QS_SAGE_FORMAT                        112
#define IDSTABLE_QUARANTINED_STRING                    113

#define IDSTABLE_DEFCAST_EVENTNAME                    1001
#define IDSTABLE_DISK_EVENTNAME                       1002
#define IDSTABLE_SCANEXPLICIT_EVENTNAME               1003
#define IDSTABLE_TARGET_EVENTNAME                     1004
#define IDSTABLE_GATEWAY_COMM_EVENTNAME               1005
#define IDSTABLE_BLESSED_DEFS_EVENTNAME               1006
#define IDSTABLE_UNBLESSED_DEFS_EVENTNAME             1007
#define IDSTABLE_SAMPLE_ATTENTION_EVENTNAME           1008
#define IDSTABLE_SAMPLE_HELD_EVENTNAME                1009
#define IDSTABLE_SAMPLE_ERROR_EVENTNAME               1010
#define IDSTABLE_SAMPLE_NOTINSTALLED_EVENTNAME        1011
#define IDSTABLE_SAMPLE_AVAILABLE_EVENTNAME           1012
#define IDSTABLE_SAMPLE_QUARANTINED_EVENTNAME         1013
#define IDSTABLE_SAMPLE_SUBMITTED_EVENTNAME           1014
#define IDSTABLE_SAMPLE_RELEASED_EVENTNAME            1015
#define IDSTABLE_DEF_NEEDED_EVENTNAME                 1016
#define IDSTABLE_DEF_DISTRIBUTED_EVENTNAME            1017
#define IDSTABLE_DEF_DISTRIBUTE_EVENTNAME             1018
#define IDSTABLE_Q_NOT_REPAIRED_EVENTNAME             1019
#define IDSTABLE_SEND_TEST_EVENTNAME                  1020
#define IDSTABLE_GENERAL_INFO_EVENTNAME               1021
#define IDSTABLE_TEXT_SEND_TESTEVENT                  1022
#define IDSTABLE_TEXT_UNABLE_TO_DISTRIBUTE_DEFS       1023
#define IDSTABLE_MSGTITLE_GENERAL_ATTENTION           1024
#define IDSTABLE_MSGTITLE_GATEWAY_ERROR               1025
#define IDSTABLE_MSGTITLE_SAMPLE_ERROR                1026
#define IDSTABLE_MSGTITLE_DEF_ERROR1                  1027
#define IDSTABLE_MSGTITLE_DEF_ERROR2                  1028
#define IDSTABLE_MSGTITLE_INFORMATIONAL               1029
#define IDSTABLE_TEXT_SEQUENCE_                       1030
#define IDSTABLE_HEADING_USER                         1031
#define IDSTABLE_HEADING_QSERVER                      1032
#define IDSTABLE_HEADING_COMPUTERNAME                 1033
#define IDSTABLE_HEADING_ADDRESS                      1034
#define IDSTABLE_HEADING_DNSNAME                      1035
#define IDSTABLE_HEADING_VIRUSNAME                    1036
#define IDSTABLE_HEADING_DEFINITIONS                  1037
#define IDSTABLE_HEADING_FILENAME                     1038
#define IDSTABLE_HEADING_UNKNOWN                      1039
#define IDSTABLE_STATUS_QUARANTINED                   1040
#define IDSTABLE_STATUS_SUBMITTED                     1041
#define IDSTABLE_STATUS_HELD                          1042
#define IDSTABLE_STATUS_RELEASED                      1043
#define IDSTABLE_STATUS_UNNEEDED                      1044
#define IDSTABLE_STATUS_NEEDED                        1045
#define IDSTABLE_STATUS_AVAILABLE                     1046
#define IDSTABLE_STATUS_DISTRIBUTE                    1047
#define IDSTABLE_STATUS_DISTRIBUTED                   1048
#define IDSTABLE_STATUS_INSTALLED                     1049
#define IDSTABLE_STATUS_ATTENTION                     1050
#define IDSTABLE_STATUS_ERROR                         1051
#define IDSTABLE_STATUS_NOTINSTALLED                  1052
#define IDSTABLE_STATUS_RESTART                       1053
#define IDSTABLE_STATUS_LEGACY                        1054
#define IDSTABLE_STATUS_UNKNOWN                       1055
#define IDSTABLE_HEADING_STATUS                       1056
#define IDSTABLE_SAMPLE_INSTATE_TOO_LONG              1057
#define IDSTABLE_GENERAL_INSTATE_TOO_LONG1            1058
#define IDSTABLE_GENERAL_INSTATE_TOO_LONG2            1059
#define IDSTABLE_DISKQUOTA_LOW_EVENTNAME_ID           1060
#define IDSTABLE_DISKSPACE_LOW_EVENTNAME_ID           1061
#define IDSTABLE_DISKQUOTA_QUARANTINE_FULL            1062
#define IDSTABLE_MSGTITLE_DISK_WARNING                1064
#define IDSTABLE_DISKSPACE_LOW_TEXTID                 1065
#define IDSTABLE_DISKQUOTA_LOW_TEXTID                 1066
#define IDSTABLE_HELPTEXT_ATTN_SCANEXPLICIT           1067
#define IDSTABLE_HELPTEXT_ATTN_DISK                   1068
#define IDSTABLE_HELPTEXT_ATTN_DEFCAST                1069
#define IDSTABLE_HELPTEXT_ATTN_TARGET                 1070
#define IDSTABLE_HELPTEXT_ATTN_GATEWAY                1071
#define IDSTABLE_HEADING_SAMPLE_STATE                 1072
#define IDSTABLE_NEW_BLESSED_DEFS_TEXTID              1073
#define IDSTABLE_NEW_UNBLESSED_DEFS_TEXTID            1074

#define IDSTABLE_HELPTEXT_SAMPLE_NOTINSTALLED         1075
#define IDSTABLE_HELPTEXT_SAMPLE_AVAILABLE            1076
#define IDSTABLE_HELPTEXT_SAMPLE_QUARANTINED          1077
#define IDSTABLE_HELPTEXT_SAMPLE_SUBMITTED            1078
#define IDSTABLE_HELPTEXT_SAMPLE_RELEASED             1079
#define IDSTABLE_HELPTEXT_DEF_NEEDED                  1080
#define IDSTABLE_HELPTEXT_DEF_DISTRIBUTE              1081
#define IDSTABLE_HELPTEXT_SAMPLE_HELD                 1082
#define IDSTABLE_HELPTEXT_SAMPLE_DISTRIBUTED          1083
#define IDSTABLE_STAB_HEADING_DEFINITIONS_NEEDED      1084
#define IDSTABLE_STAB_TEXT_VERSION_NUMBER             1085
#define IDSTABLE_HELPTEXT_DEF_FTP_ADDRESS             1086
#define IDSTABLE_HELPTEXT_SAMPLE_NOTINSTALLED2        1087

#define IDSTABLE_SHUTDOWN_EVENTNAME                   1088
#define IDSTABLE_HELPTEXT_ATTN_SHUTDOWN               1089
#define IDSTABLE_HELPTEXT_SAMPLE_NOTREPAIRED          1090

#define IDSTABLE_UNAVAILABLE_EVENTNAME                1091
#define IDSTABLE_HELPTEXT_ATTN_UNAVAILABLE            1092
#define IDSTABLE_HELPTEXT_SAMPLE_ENCRYPTED			  1093
#define IDSTABLE_HELPTEXT_SAMPLE_RESTORE			  1094
#define IDSTABLE_HELPTEXT_SAMPLE_DELETE 			  1095

// added 8-4-03 tmarles
#define IDSTABLE_STATUS_RISK						  1096
#define IDSTABLE_STATUS_HACKTOOL					  1097
#define IDSTABLE_STATUS_SPYWARE						  1098
#define IDSTABLE_STATUS_TRACKWARE					  1099
#define IDSTABLE_STATUS_DIALER						  1100
#define IDSTABLE_STATUS_REMOTE						  1101
#define IDSTABLE_STATUS_ADWARE						  1102
#define IDSTABLE_STATUS_PRANK						  1103



// #IPE BEGIN - DO NOT TRANSLATE!


// DEFINES FOR ICEPACK TOKENS
// ATTENTION TABLE   
#define TOKEN_DEFCAST              _T("defcast")        
#define TOKEN_DISK                 _T("disk")           
#define TOKEN_QUARANTINE           _T("quarantine")         
#define TOKEN_SCANEXPLICIT         _T("scanexplicit")   
#define TOKEN_TARGET               _T("target")         
#define TOKEN_GATEWAYCOMM          _T("gatewayComm")    
#define TOKEN_SHUTDOWN             _T("shutdown")
#define TOKEN_UNAVAILABLE          _T("unavailable")
// NEW DEFINITIONS ARRIVAL         
#define TOKEN_NEWBLESSEDDEF        _T("newBlessedDef")  
#define TOKEN_NEWUNBLESSEDDEF      _T("newUnblessedDef")
// FOR A SEARCH FUNCTION in ClearGeneralAttentionTimer()
#define TOKEN_DOWNLOAD_GATEWAY     _T("download")
#define TOKEN_QUERY_GATEWAY        _T("query")
#define TOKEN_SUBMIT_GATEWAY       _T("submit")
// SEND TEST EVENT
#define TOKEN_SEND_TEST_EVENT      _T("sendTestEvent")
// DISK SPACE WARNING EVENTS            
#define TOKEN_DISKQUOTA_LOW_WATER  _T("diskQuotaLowWater")
#define TOKEN_DISKSPACE_LOW_WATER  _T("diskSpaceLowWater")
#define TOKEN_QUARANTINE_FULL	   _T("diskQuotaCQFull")



// SAMPLE TABLE
// ICEPACK STATUS TOKENS
#define TOKEN_HELD              _T("held")        
#define TOKEN_ATTENTION         _T("attention")   
#define TOKEN_ERROR             _T("error")       
#define TOKEN_NOTINSTALLED      _T("notinstalled")
//   
#define TOKEN_QUARANTINED       _T("quarantined")
#define TOKEN_SUBMITTED         _T("submitted")
#define TOKEN_RELEASED          _T("released")
#define TOKEN_UNNEEDED          _T("unneeded")
#define TOKEN_NEEDED            _T("needed")
#define TOKEN_AVAILABLE         _T("available")   
#define TOKEN_DISTRIBUTED       _T("distributed")
#define TOKEN_INSTALLED         _T("installed")
#define TOKEN_RESTART           _T("restart")
#define TOKEN_DISTRIBUTE        _T("distribute")  
#define TOKEN_LEGACY            _T("legacy")
#define TOKEN_UNKNOWN           _T("unknown")
// added 8-4-03 tmarles
#define TOKEN_RISK		        _T("risk")
#define TOKEN_HACKTOOL	        _T("hacktool")
#define TOKEN_SPYWARE	        _T("spyware")
#define TOKEN_TRACKWARE	        _T("trackware")
#define TOKEN_DIALER	        _T("dialer")
#define TOKEN_REMOTE	        _T("remote")
#define TOKEN_ADWARE	        _T("adware")
#define TOKEN_PRANK		        _T("prank")


// ICEPACK ERROR TOKENS
#define TOKEN_ABANDONED         _T("abandoned")   
#define TOKEN_CONTENT           _T("content")     
#define TOKEN_CRUMBLED          _T("crumbled")    
#define TOKEN_DECLINED          _T("declined")    
#define TOKEN_INTERNAL          _T("internal")    
#define TOKEN_LOST              _T("lost")        
#define TOKEN_MALFORMED         _T("malformed")   
#define TOKEN_MISSING           _T("missing")     
#define TOKEN_OVERRUN           _T("overrun")     
#define TOKEN_SAMPLE            _T("sample")      
#define TOKEN_SUPERCEDED        _T("superceded")  
#define TOKEN_TYPE              _T("type")        
#define TOKEN_UNAVAILABLE       _T("unavailable") 
#define TOKEN_UNDERRUN          _T("underrun")    
#define TOKEN_UNPACKAGE         _T("unpackage")   
#define TOKEN_UNPUBLISHED       _T("unpublished") 

// Local Quarantine
#define TOKEN_NOTREPAIRED       _T("norepair") 


// ICEPACK STATE TABLE
#define TOKEN_CAPTURED                   _T("captured")
#define TOKEN_SUBMITTING                 _T("submitting")
#define TOKEN_RECEIVING                  _T("receiving")
#define TOKEN_ACCEPTED                   _T("accepted")
#define TOKEN_IMPORTING                  _T("importing")
#define TOKEN_IMPORTED                   _T("imported")
#define TOKEN_CLASSIFYING                _T("classifying")
#define TOKEN_RESCAN                     _T("rescan")
#define TOKEN_BINARY                     _T("binary")
#define TOKEN_BINARYCONTROLLING          _T("binaryControlling")
#define TOKEN_BINARYWAIT                 _T("binaryWait")
#define TOKEN_BINARYREPLICATING          _T("binaryReplicating")
#define TOKEN_BINARYSCORING              _T("binaryScoring")
#define TOKEN_MACRO                      _T("macro")
#define TOKEN_MACROCONTROLLING           _T("macroControlling")
#define TOKEN_MACROWAIT                  _T("macroWait")
#define TOKEN_MACROREPLICATING           _T("macroReplicating")
#define TOKEN_MACROSCORING               _T("macroScoring")
#define TOKEN_SIGNATURES                 _T("signatures")
#define TOKEN_LOCKING                    _T("locking")
#define TOKEN_INCRBUILDING               _T("incrBuilding")
#define TOKEN_INCRUNITTESTING            _T("incrUnitTesting")
#define TOKEN_FULLBUILDING               _T("fullBuilding")
#define TOKEN_FULLUNITTESTING            _T("fullUnitTesting")
#define TOKEN_UNLOCKING                  _T("unlocking")
#define TOKEN_DEFERRING                  _T("deferring")
#define TOKEN_DEFERRED                   _T("deferred")
#define TOKEN_DEFER                      _T("defer")
#define TOKEN_ARCHIVE                    _T("archive")
#define TOKEN_ARCHIVING                  _T("archiving")
#define TOKEN_ATTENTION                  _T("attention")
#define TOKEN_EMPTY                      _T("empty")
#define TOKEN_ERROR                      _T("error")
#define TOKEN_UNINFECTABLE               _T("uninfectable")
#define TOKEN_UNSUBMITABLE               _T("unsubmitable")
#define TOKEN_NODETECT                   _T("nodetect")
#define TOKEN_INFECTED                   _T("infected")
#define TOKEN_UNINFECTED                 _T("uninfected")
#define TOKEN_MISFIRED                   _T("misfired")
// ADDED 6/28/00
#define TOKEN_WIN32                      _T("win32")
#define TOKEN_WIN32CONTROLLING           _T("win32Controlling")
#define TOKEN_WIN32WAIT                  _T("win32Wait")
#define TOKEN_WIN32REPLICATING           _T("win32Replicating")
#define TOKEN_WIN32SCORING               _T("win32Scoring")
// ADDED 6/30/00
#define TOKEN_CONTROLLING                _T("controlling")
#define TOKEN_WAIT                       _T("wait")
#define TOKEN_REPLICATING                _T("replicating")
#define TOKEN_SCORING                    _T("scoring")

// ADDED 3-11-02 tmarles
#define TOKEN_ENCRYPTED                   _T("encrypted")
#define TOKEN_RESTORE                     _T("restore")
#define TOKEN_DELETE                      _T("delete")



// RESULT TABLE
#define TOKEN_BADREPAIR                  _T("badrepair") 
#define TOKEN_BADSCAN                    _T("badscan")
#define TOKEN_HEURISTIC                  _T("heuristic")
#define TOKEN_NODETECT                   _T("nodetect")
#define TOKEN_NOREPAIR                   _T("norepair")
#define TOKEN_OVERRUN                    _T("overrun")
#define TOKEN_REPAIRED                   _T("repaired")
#define TOKEN_UNDERRUN                   _T("underrun")
#define TOKEN_UNREPAIRABLE               _T("unrepairable")
#define TOKEN_UNINFECTABLE               _T("uninfectable")
#define TOKEN_UNSUBMITTABLE              _T("unsubmittable")

// #IPE END





#endif