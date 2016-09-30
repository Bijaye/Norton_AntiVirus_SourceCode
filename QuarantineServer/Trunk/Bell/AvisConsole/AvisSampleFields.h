/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/*
    AvisSampleFields.h
    Defines for the various X- headers.

    DO NOT TRANSLATE ANYTHING IN THIS FILE.
    EVERYTHING TRANSLATABLE IS IN THE RC FILE.

    Written by: Jim Hill
*/


#if !defined(AVISSAMPLEFIELDS_H_INCLUDED)
    #define AVISSAMPLEFIELDS_H_INCLUDED

// GET OUR STRING ID'S
#include "resource.h"
// GET THE LIST OF QSERVER ITEM DEFINES
#include "qsfields.h"




// ALL ATTRIBUTE NAMES
#define AVIS_X_PLATFORM_USER                     _T(QSERVER_ITEM_INFO_USERNAME       )   // "X-Platform-User"          // String
#define AVIS_X_PLATFORM_COMPUTER                 _T(QSERVER_ITEM_INFO_MACHINENAME    )   // "X-Platform-Computer"      // String
#define AVIS_X_PLATFORM_DOMAIN                   _T(QSERVER_ITEM_INFO_DOMAINNAME     )   // "X-Platform-Domain"        // String
#define AVIS_X_PLATFORM_ADDRESS                  _T(QSERVER_ITEM_INFO_MACHINE_ADDRESS)   // "X-Platform-Address"       // String
#define AVIS_X_PLATFORM_PROCESSOR                _T(QSERVER_ITEM_INFO_PROCESSOR      )   // "X-Platform-Processor"     // String
#define AVIS_X_PLATFORM_DISTRIBUTOR              _T(QSERVER_ITEM_INFO_DISTRIBUTOR    )   // "X-Platform-Distributor"   // String
#define AVIS_X_PLATFORM_HOST                     _T(QSERVER_ITEM_INFO_HOST           )   // "X-Platform-Host"          // String
#define AVIS_X_PLATFORM_SYSTEM                   _T(QSERVER_ITEM_INFO_SYSTEM         )   // "X-Platform-System"        // String
#define AVIS_X_PLATFORM_LANGUAGE                 _T(QSERVER_ITEM_INFO_LANGUAGE       )   // "X-Platform-Language"      // String
#define AVIS_X_PLATFORM_OWNER                    _T(QSERVER_ITEM_INFO_OWNER          )   // "X-Platform-Owner"         // String
#define AVIS_X_PLATFORM_SCANNER                  _T(QSERVER_ITEM_INFO_SCANNER        )   // "X-Platform-Scanner"       // String
#define AVIS_X_PLATFORM_CORRELATOR               _T(QSERVER_ITEM_INFO_CORRELATOR     )   // "X-Platform-Correlator"    // String
#define AVIS_X_PLATFORM_GUID                     _T(QSERVER_ITEM_INFO_PLATFORM_GUID  )   // "X-Platform-GUID"          // String
#define AVIS_X_PLATFORM_INFO_DELIVER             _T(QSERVER_ITEM_INFO_DELIVER        )   // String, "X-Platform-Deliver" new in Jedi release  "vdb" or "legacy"
                                                                                           
#define AVIS_X_DATE_ACCESSED                     _T(QSERVER_ITEM_INFO_FILE_ACCESSED_TIME)  // "X-Date-Accessed"        // QSDATE (Binary) / OLE DATE   
#define AVIS_X_DATE_ANALYZED                     _T("X-Date-Analyzed")
#define AVIS_X_DATE_BLESSED                      _T("X-Date-Blessed")                              
#define AVIS_X_DATE_CAPTURED                     _T("X-Date-Captured")
#define AVIS_X_DATE_CREATED                      _T(QSERVER_ITEM_INFO_FILE_CREATED_TIME)   // "X-Date-Created"         // QSDATE (Binary) / OLE DATE 
#define AVIS_X_DATE_DISTRIBUTED                  _T("X-Date-Distributed")
#define AVIS_X_DATE_FINISHED                     _T("X-Date-Finished")
#define AVIS_X_DATE_FORWARDED                    _T("X-Date-Forwarded")
#define AVIS_X_DATE_INSTALLED                    _T("X-Date-Installed")
#define AVIS_X_DATE_MODIFIED                     _T(QSERVER_ITEM_INFO_FILE_MODIFIED_TIME)  // "X-Date-Modified"        // QSDATE (Binary) / OLE DATE 
#define AVIS_X_DATE_PRODUCED                     _T("X-Date-Produced")
#define AVIS_X_DATE_PUBLISHED                    _T("X-Date-Published")
#define AVIS_X_DATE_SUBMITTED                    _T(QSERVER_ITEM_INFO_SUBMIT_QDATE)        // "X-Date-Submitted"       // QSDATE (Binary) / OLE DATE 
#define AVIS_X_DATE_ANALYSIS_FINISHED            _T("X-Date-Analysis-Finished")
#define AVIS_X_DATE_SAMPLE_FINISHED              _T("X-Date-Sample-Finished")
#define AVIS_X_DATE_QUARANTINEDATE               _T("X-Date-QuarantineDate")               // "X-Date-QuarantineDate"  // QSDATE (Binary) / OLE DATE 
#define AVIS_X_DATE_QUARANTINE                   _T(QSERVER_ITEM_INFO_QUARANTINE_QDATE)    // "X-Date-Quarantined"
#define AVIS_X_DATE_COMPLETED                    _T(QSERVER_ITEM_INFO_COMPLETED_TIME)      // "X-Date-Completed"
                                                                               
#define AVIS_X_SCAN_VIRUS_IDENTIFIER             _T(QSERVER_ITEM_INFO_VIRUSID     )        // "X-Scan-Virus-Identifier"     // DWORD
#define AVIS_X_SCAN_SIGNATURE_SEQUENCE           _T(QSERVER_ITEM_INFO_DEF_SEQUENCE)        // "X-Scan-Signature-Sequence"   // DWORD
#define AVIS_X_SCAN_SIGNATURE_VERSION            _T(QSERVER_ITEM_INFO_DEF_VERSION )        // "X-Scan-Signature-Version"    // DWORD
#define AVIS_X_SCAN_SIGNATURES_SEQUENCE          _T("X-Scan-Signatures-Sequence")                                           // DWORD 
#define AVIS_X_SCAN_SIGNATURES_VERSION           _T("X-Scan-Signatures-Version")                                            // DWORD 
#define AVIS_X_SCAN_VIRUS_NAME                   _T(QSERVER_ITEM_INFO_VIRUSNAME  )         // "X-Scan-Virus-Name"           // String
#define AVIS_X_SCAN_RESULT                       _T(QSERVER_ITEM_INFO_SCAN_RESULT)         // "X-Scan-Result"               // DWORD  
                                                                              
#define AVIS_X_SAMPLE_CHECKSUM                   _T(QSERVER_ITEM_INFO_CHECKSUM        )  // "X-Sample-Checksum"             // String 
#define AVIS_X_CHECKSUM_METHOD                   _T(QSERVER_ITEM_CHECKSUM_METHOD      )  // "X-Checksum-Method"             // String (md5)
#define AVIS_X_SAMPLE_EXTENSION                  _T(QSERVER_ITEM_INFO_INFO_FILE_EXT   )  // "X-Sample-Extension"            // String
#define AVIS_X_SAMPLE_FILE                       _T(QSERVER_ITEM_INFO_FILENAME        )  // "X-Sample-File"                 // String
#define AVIS_X_SAMPLE_SIZE                       _T(QSERVER_ITEM_INFO_FILESIZE        )  // "X-Sample-Size"                 // DWORD 
#define AVIS_X_SAMPLE_TYPE                       _T(QSERVER_ITEM_INFO_SAMPLE_TYPE     )  // "X-Sample-Type"                 // String (file)
#define AVIS_X_SAMPLE_REASON                     _T(QSERVER_ITEM_INFO_SAMPLE_REASON   )  // "X-Sample-Reason"               // String
#define AVIS_X_SAMPLE_GEOMETRY                   _T(QSERVER_ITEM_INFO_DISK_GEOMETRY   )  // "X-Sample-Geometry"             // Binary Geometry
#define AVIS_X_SAMPLE_STATUS                     _T(QSERVER_ITEM_INFO_STATUS          )  // "X-Sample-Status"               // DWORD  
#define AVIS_X_SAMPLE_SUBMISSION_ROUTE           _T(QSERVER_ITEM_INFO_SUBMISSION_ROUTE)  // "X-Sample-Submission-Route"     // DWORD 
#define AVIS_X_SAMPLE_UUID                       _T(QSERVER_ITEM_INFO_UUID            )  // "X-Sample-UUID"                 // String ( GUID format {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx})
#define AVIS_X_SAMPLE_FILEID                     _T(QSERVER_ITEM_INFO_FILEID          )  // "X-Sample-FileID"               // DWORD
#define AVIS_X_SAMPLE_CHANGES                    _T(QSERVER_ITEM_INFO_CHANGES         )  // "X-Sample-Changes"              // DWORD 
#define AVIS_X_SAMPLE_PRIORITY                   _T(QSERVER_ITEM_INFO_PRIORITY        )  // "X-Sample-Priority"             // DWORD 
#define AVIS_X_SAMPLE_SIGNATURES_PRIORITY        _T("X-Sample-Signatures-Priority")
#define AVIS_X_SAMPLE_SECTOR                     _T("X-Sample-Sector")
#define AVIS_X_SAMPLE_SERVICE                    _T("X-Sample-Service")
#define AVIS_X_SAMPLE_STRIP                      _T("X-Sample-Strip")
#define AVIS_X_SAMPLE_CATEGORY                   _T("X-Sample-Category")
#define AVIS_X_SAMPLE_ERROR_ATTENTION            _T("X-Attention")
#define AVIS_X_SAMPLE_ERROR                      _T("X-Error")               
                                                                          
#define AVIS_X_ANALYSIS_STATE                    _T("X-Analysis-State")
#define AVIS_X_ANALYSIS_COOKIE                   _T("X-Analysis-Cookie")
#define AVIS_X_ANALYSIS_ISSUE                    _T("X-Analysis-Issue")
#define AVIS_X_ANALYSIS_VIRUS_NAME               _T("X-Analysis-Virus_Name")
#define AVIS_X_ANALYSIS_SERVICE                  _T("X-Analysis-Service")
#define AVIS_X_ANALYSIS_VIRUS_IDENTIFIER         _T("X-Analysis-Virus-Identifier")
                                                                                           
#define AVIS_X_SIGNATURES_SEQUENCE               _T("X-Signatures-Sequence")
#define AVIS_X_SIGNATURES_SEQUENCE_OLD           _T("X-Signature-Sequence")
#define AVIS_X_SIGNATURES_PRIORITY               _T("X-Signatures-Priority")
#define AVIS_X_SIGNATURES_NAME                   _T("X-Signatures-Name")
#define AVIS_X_SIGNATURES_SIZE                   _T("X-Signatures-Size")
#define AVIS_X_SIGNATURES_VERSION                _T("X-Signatures-Version")
#define AVIS_X_SIGNATURES_VERSION_OLD            _T("X-Signature-Version")
#define AVIS_X_SIGNATURES_NAME_X_SAMPLE_CHECKSUM _T("X-Signatures-Name:X-Sample-Checksum")
                                                                                           
#define AVIS_X_CONTENT_CHECKSUM                  _T("X-Content-Checksum")
#define AVIS_X_CONTENT_COMPRESSION               _T("X-Content-Compression")
#define AVIS_X_CONTENT_ENCODING                  _T("X-Content-Encoding")
#define AVIS_X_CONTENT_SCRAMBLING                _T("X-Content-Scrambling")
                                                                                 
#define AVIS_X_CUSTOMER_CONTACT_EMAIL            _T("X-Customer-Contact-Email")
#define AVIS_X_CUSTOMER_CONTACT_NAME             _T("X-Customer-Contact-Name")
#define AVIS_X_CUSTOMER_CONTACT_TELEPHONE        _T("X-Customer-Contact-Telephone")
#define AVIS_X_CUSTOMER_CREDENTIALS              _T("X-Customer-Credentials")
#define AVIS_X_CUSTOMER_IDENTIFIER               _T("X-Customer-Identifier")
#define AVIS_X_CUSTOMER_NAME                     _T("X-Customer-Name")


// Added 12/31/99 jhill
#define AVIS_X_ICEPACK_INITIALIZED               _T("X-IcePack-Initialized")
#define AVIS_X_REAL_SCAN_RESULT                  _T("X-Real-Scan-Result")
#define AVIS_X_SAMPLE_STATUS_READABLE            _T("X-Sample-Status-Readable") 
#define AVIS_X_CONTENT_STRIP                     _T("X-Content-Strip")  
#define AVIS_X_ERROR                             _T( QSERVER_ITEM_INFO_ERROR          )
#define AVIS_X_ATTENTION                         _T( QSERVER_ITEM_INFO_X_ATTENTION    )
#define AVIS_X_ALERT_STATUS                      _T( QSERVER_ITEM_INFO_X_ALERT_STATUS )
#define AVIS_X_DATE_ALERT                        _T( QSERVER_ITEM_INFO_X_DATE_ALERT   )
#define AVIS_X_ALERT_RESULT                      _T( "X-Alert-Result" )  
#define AVIS_X_ALERT_STATUS_TIMER                _T( "X-Alert-Status-Timer" )  
#define AVIS_X_DATE_SAMPLE_STATUS                _T( "X_Date_Sample_Status" )
#define AVIS_X_SUBMISSION_COUNT                  _T( "X-Submission-Count" )



// ADD ON TO THE LIST OF QSERVER ITEM DEFINES
#define AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY     AVIS_X_SAMPLE_PRIORITY              // _T("X-Sample-Priority")            // DWORD
#define AVIS_SAMPLE_ACTIONS_ANALYSIS_STATE          AVIS_X_ANALYSIS_STATE               // _T("X-Analysis-State")             // STRING
#define AVIS_SAMPLE_ACTIONS_GATEWAY_COOKIE          AVIS_X_ANALYSIS_COOKIE              // _T("X-Analysis-Cookie")            // STRING
#define AVIS_SAMPLE_ACTIONS_ANALYSIS_ISSUE          AVIS_X_ANALYSIS_ISSUE               // _T("X-Analysis-Issue")             // STRING 
#define AVIS_SAMPLE_ACTIONS_VIRUS_NAME              AVIS_X_ANALYSIS_VIRUS_NAME          // _T("X-Analysis-Virus_Name")        // STRING 
#define AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED          AVIS_X_SIGNATURES_SEQUENCE          // _T("X-Signatures-Sequence")        // STRING
#define AVIS_SAMPLE_ACTIONS_DATE_ANALYSIS_FINISHED  AVIS_X_DATE_ANALYSIS_FINISHED       // _T("X-Date-Analysis-Finished")
#define AVIS_SAMPLE_ACTIONS_AUTO_DELIVERY_DEFS      AVIS_X_SAMPLE_SIGNATURES_PRIORITY   // _T("X-Sample-Signatures-Priority") // 
#define AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY     AVIS_X_SIGNATURES_PRIORITY          // _T("X-Signatures-Priority")
#define AVIS_SAMPLE_ACTIONS_SAMPLE_FINISHED         AVIS_X_DATE_SAMPLE_FINISHED         // _T("X-Date-Sample-Finished")
//#define AVIS_QSERVER_ITEM_INFO_GUID               AVIS_X_PLATFORM_GUID                // _T("X-Platform-GUID") 

 

// ADD IN WILDCARDS TO GET ALL ENTRIES CATEGORY  i.e. "X-Platform"
#define AVIS_WILDCARD_ALL_CATEGORY                 _T("X-") 
#define AVIS_WILDCARD_PLATFORM_CATEGORY            _T("X-Platform-") 
#define AVIS_WILDCARD_SAMPLE_CATEGORY              _T("X-Sample-")
#define AVIS_WILDCARD_SCAN_CATEGORY                _T("X-Scan-")
#define AVIS_WILDCARD_DATE_CATEGORY                _T("X-Date-")
#define AVIS_WILDCARD_SIGNATURES_CATEGORY          _T("X-Signatures-")
#define AVIS_WILDCARD_X_ERROR                      _T("X-Error-")                                 
#define AVIS_WILDCARD_X_BACKUP                     _T("X-Backup")                                 



// Values for QSERVER_ITEM_INFO_STATUS
#define AVIS_SAMPLE_STATUS_QUARANTINED   STATUS_QUARANTINED     // 0     
#define AVIS_SAMPLE_STATUS_SUBMITTED     STATUS_SUBMITTED       // 1
#define AVIS_SAMPLE_STATUS_HELD          STATUS_HELD            // 2
#define AVIS_SAMPLE_STATUS_RELEASED      STATUS_RELEASED        // 3
#define AVIS_SAMPLE_STATUS_UNNEEDED      STATUS_UNNEEDED        // 4
#define AVIS_SAMPLE_STATUS_NEEDED        STATUS_NEEDED          // 5
#define AVIS_SAMPLE_STATUS_AVAILABLE     STATUS_AVAILIABLE      // 6
#define AVIS_SAMPLE_STATUS_DISTRIBUTED   STATUS_DISTRIBUTED     // 7
#define AVIS_SAMPLE_STATUS_INSTALLED     STATUS_INSTALLED       // 8
#define AVIS_SAMPLE_STATUS_ATTENTION     STATUS_ATTENTION       // 9  
#define AVIS_SAMPLE_STATUS_ERROR         STATUS_ERROR           // 10 
#define AVIS_SAMPLE_STATUS_NOTINSTALLED  STATUS_NOTINSTALLED    // 11   // new in Jedi release
#define AVIS_SAMPLE_STATUS_RESTART       STATUS_RESTART         // 12   // new in Jedi release
#define AVIS_SAMPLE_STATUS_LEGACY        STATUS_LEGACY          // 13   // new in Jedi release
#define AVIS_SAMPLE_STATUS_DISTRIBUTE    STATUS_DISTRIBUTE      // 14    // renamed in Jedi release
#define AVIS_SAMPLE_STATUS_RISK		     STATUS_RISK		    // 15	// new in Kepler release tmarles 8-04-03
#define AVIS_SAMPLE_STATUS_HACKTOOL	     STATUS_HACKTOOL	    // 16	// new in Kepler release tmarles 8-04-03
#define AVIS_SAMPLE_STATUS_SPYWARE	     STATUS_SPYWARE		    // 17	// new in Kepler release tmarles 8-04-03
#define AVIS_SAMPLE_STATUS_TRACKWARE     STATUS_TRACKWARE	    // 18	// new in Kepler release tmarles 8-04-03
#define AVIS_SAMPLE_STATUS_DIALER	     STATUS_DIALER		    // 19	// new in Kepler release tmarles 8-04-03
#define AVIS_SAMPLE_STATUS_REMOTE	     STATUS_REMOTE		    // 20	// new in Kepler release tmarles 8-04-03
#define AVIS_SAMPLE_STATUS_ADWARE	     STATUS_ADWARE		    // 21	// new in Kepler release tmarles 8-04-03
#define AVIS_SAMPLE_STATUS_PRANK	     STATUS_PRANK		    // 22	// new in Kepler release tmarles 8-04-03





// X-ATTRIBBUTE LIST LOOKUP TABLE INCLUDING EXCLUDE LIST
#define AVIS_ID_X_PLATFORM_USER                          1 
#define AVIS_ID_X_PLATFORM_COMPUTER                      2 
#define AVIS_ID_X_PLATFORM_DOMAIN                        3 
#define AVIS_ID_X_PLATFORM_ADDRESS                       4 
#define AVIS_ID_X_PLATFORM_PROCESSOR                     5 
#define AVIS_ID_X_PLATFORM_DISTRIBUTOR                   6 
#define AVIS_ID_X_PLATFORM_HOST                          7 
#define AVIS_ID_X_PLATFORM_SYSTEM                        8 
#define AVIS_ID_X_PLATFORM_LANGUAGE                      9 
#define AVIS_ID_X_PLATFORM_OWNER                         10
#define AVIS_ID_X_PLATFORM_SCANNER                       11
#define AVIS_ID_X_PLATFORM_CORRELATOR                    12
#define AVIS_ID_X_PLATFORM_GUID                          13
                                                         
#define AVIS_ID_X_DATE_ACCESSED                          14
#define AVIS_ID_X_DATE_ANALYZED                          15
#define AVIS_ID_X_DATE_BLESSED                           16
#define AVIS_ID_X_DATE_CAPTURED                          17
#define AVIS_ID_X_DATE_CREATED                           18
#define AVIS_ID_X_DATE_DISTRIBUTED                       19
#define AVIS_ID_X_DATE_FINISHED                          20
#define AVIS_ID_X_DATE_FORWARDED                         21
#define AVIS_ID_X_DATE_INSTALLED                         22
#define AVIS_ID_X_DATE_MODIFIED                          23
#define AVIS_ID_X_DATE_PRODUCED                          24
#define AVIS_ID_X_DATE_PUBLISHED                         25
#define AVIS_ID_X_DATE_SUBMITTED                         26
#define AVIS_ID_X_DATE_ANALYSIS_FINISHED                 27
#define AVIS_ID_X_DATE_SAMPLE_FINISHED                   28
#define AVIS_ID_X_DATE_QUARANTINEDATE                    29
#define AVIS_ID_X_DATE_QUARANTINE                        30
                                                         
#define AVIS_ID_X_SCAN_VIRUS_IDENTIFIER                  31
#define AVIS_ID_X_SCAN_SIGNATURE_SEQUENCE                32
#define AVIS_ID_X_SCAN_SIGNATURE_VERSION                 33
#define AVIS_ID_X_SCAN_SIGNATURES_SEQUENCE               34
#define AVIS_ID_X_SCAN_SIGNATURES_VERSION                35
#define AVIS_ID_X_SCAN_VIRUS_NAME                        36
#define AVIS_ID_X_SCAN_RESULT                            37
                                        
#define AVIS_ID_X_SAMPLE_CHECKSUM                        38
#define AVIS_ID_X_CHECKSUM_METHOD                        39
#define AVIS_ID_X_SAMPLE_EXTENSION                       40
#define AVIS_ID_X_SAMPLE_FILE                            41
#define AVIS_ID_X_SAMPLE_SIZE                            42
#define AVIS_ID_X_SAMPLE_TYPE                            43
#define AVIS_ID_X_SAMPLE_REASON                          44
#define AVIS_ID_X_SAMPLE_GEOMETRY                        45
#define AVIS_ID_X_SAMPLE_STATUS                          46
#define AVIS_ID_X_SAMPLE_SUBMISSION_ROUTE                47
#define AVIS_ID_X_SAMPLE_UUID                            48
#define AVIS_ID_X_SAMPLE_FILEID                          49
#define AVIS_ID_X_SAMPLE_CHANGES                         50
#define AVIS_ID_X_SAMPLE_PRIORITY                        51
#define AVIS_ID_X_SAMPLE_SIGNATURES_PRIORITY             52
#define AVIS_ID_X_SAMPLE_SECTOR                          53
#define AVIS_ID_X_SAMPLE_SERVICE                         54
#define AVIS_ID_X_SAMPLE_STRIP                           55
#define AVIS_ID_X_SAMPLE_CATEGORY                        56
                                                          
#define AVIS_ID_X_ANALYSIS_STATE                         57
#define AVIS_ID_X_ANALYSIS_COOKIE                        58
#define AVIS_ID_X_ANALYSIS_ISSUE                         59
#define AVIS_ID_X_ANALYSIS_VIRUS_NAME                    60
#define AVIS_ID_X_ANALYSIS_SERVICE                       61
#define AVIS_ID_X_ANALYSIS_VIRUS_IDENTIFIER              62
                                        
#define AVIS_ID_X_SIGNATURES_SEQUENCE                    63
#define AVIS_ID_X_SIGNATURES_PRIORITY                    64
#define AVIS_ID_X_SIGNATURES_NAME                        65
#define AVIS_ID_X_SIGNATURES_SIZE                        67
#define AVIS_ID_X_SIGNATURES_VERSION                     68
#define AVIS_ID_X_SIGNATURES_NAME_X_SAMPLE_CHECKSUM      69
                                        
#define AVIS_ID_X_CONTENT_CHECKSUM                       70
#define AVIS_ID_X_CONTENT_COMPRESSION                    71
#define AVIS_ID_X_CONTENT_ENCODING                       72
#define AVIS_ID_X_CONTENT_SCRAMBLING                     73
                                        
#define AVIS_ID_X_CUSTOMER_CONTACT_EMAIL                 74
#define AVIS_ID_X_CUSTOMER_CONTACT_NAME                  75
#define AVIS_ID_X_CUSTOMER_CONTACT_TELEPHONE             76
#define AVIS_ID_X_CUSTOMER_CREDENTIALS                   77
#define AVIS_ID_X_CUSTOMER_IDENTIFIER                    78
#define AVIS_ID_X_CUSTOMER_NAME                          79

#define AVIS_ID_PLATFORM_INFO_DELIVER                    80
#define AVIS_ID_X_ERROR                                  81
#define AVIS_ID_X_ATTENTION                              82
#define AVIS_ID_X_ALERT_STATUS                           83
#define AVIS_ID_X_DATE_ALERT                             84
#define AVIS_ID_X_ALERT_RESULT                           85
#define AVIS_ID_X_ALERT_STATUS_TIMER                     86
//#define AVIS_ID_WILDCARD_X_BACKUP                        87


#define AVISID_SAMPLE_ACTIONS_SUBMISSION_PRIORITY        AVIS_ID_X_SAMPLE_PRIORITY
#define AVISID_SAMPLE_ACTIONS_AUTO_DELIVERY_DEFS         AVIS_ID_X_SAMPLE_SIGNATURES_PRIORITY
#define AVISID_SAMPLE_ACTIONS_ANALYSIS_STATE             AVIS_ID_X_ANALYSIS_STATE
#define AVISID_SAMPLE_ACTIONS_GATEWAY_COOKIE             AVIS_ID_X_ANALYSIS_COOKIE
#define AVISID_SAMPLE_ACTIONS_ANALYSIS_ISSUE             AVIS_ID_X_ANALYSIS_ISSUE
#define AVISID_SAMPLE_ACTIONS_VIRUS_NAME                 AVIS_ID_X_ANALYSIS_VIRUS_NAME
#define AVISID_SAMPLE_ACTIONS_DEF_SEQ_NEEDED             AVIS_ID_X_SIGNATURES_SEQUENCE



#endif







#if 0
// REMAP QSERVER ITEM INFO X HEADERS TO OURS
#define AVIS_QSERVER_ITEM_INFO_USERNAME            AVIS_X_PLATFORM_USER       
#define AVIS_QSERVER_ITEM_INFO_MACHINENAME         AVIS_X_PLATFORM_COMPUTER   
#define AVIS_QSERVER_ITEM_INFO_DOMAINNAME          AVIS_X_PLATFORM_DOMAIN     
#define AVIS_QSERVER_ITEM_INFO_MACHINE_ADDRESS     AVIS_X_PLATFORM_ADDRESS    
#define AVIS_QSERVER_ITEM_INFO_PROCESSOR           AVIS_X_PLATFORM_PROCESSOR  
#define AVIS_QSERVER_ITEM_INFO_DISTRIBUTOR         AVIS_X_PLATFORM_DISTRIBUTOR
#define AVIS_QSERVER_ITEM_INFO_HOST                AVIS_X_PLATFORM_HOST       
#define AVIS_QSERVER_ITEM_INFO_SYSTEM              AVIS_X_PLATFORM_SYSTEM     
#define AVIS_QSERVER_ITEM_INFO_LANGUAGE            AVIS_X_PLATFORM_LANGUAGE   
#define AVIS_QSERVER_ITEM_INFO_OWNER               AVIS_X_PLATFORM_OWNER      
#define AVIS_QSERVER_ITEM_INFO_SCANNER             AVIS_X_PLATFORM_SCANNER    
#define AVIS_QSERVER_ITEM_INFO_CORRELATOR          AVIS_X_PLATFORM_CORRELATOR 
#define AVIS_QSERVER_ITEM_INFO_FILE_CREATED_TIME   AVIS_X_DATE_CREATED 
#define AVIS_QSERVER_ITEM_INFO_FILE_MODIFIED_TIME  AVIS_X_DATE_MODIFIED 
#define AVIS_QSERVER_ITEM_INFO_FILE_ACCESSED_TIME  AVIS_X_DATE_ACCESSED 
#define AVIS_QSERVER_ITEM_INFO_QUARANTINE_QDATE    AVIS_X_DATE_QUARANTINEDATE 
#define AVIS_QSERVER_ITEM_INFO_SUBMIT_QDATE        AVIS_X_DATE_SUBMITTED 
#define AVIS_QSERVER_ITEM_INFO_VIRUSID             AVIS_X_SCAN_VIRUS_IDENTIFIER   
#define AVIS_QSERVER_ITEM_INFO_DEF_SEQUENCE        AVIS_X_SCAN_SIGNATURE_SEQUENCE
#define AVIS_QSERVER_ITEM_INFO_DEF_VERSION         AVIS_X_SCAN_SIGNATURE_VERSION 
#define AVIS_QSERVER_ITEM_INFO_VIRUSNAME           AVIS_X_SCAN_VIRUS_NAME 
#define AVIS_QSERVER_ITEM_INFO_SCAN_RESULT         AVIS_X_SCAN_RESULT     
#define AVIS_QSERVER_ITEM_INFO_CHECKSUM            AVIS_X_SAMPLE_CHECKSUM 
#define AVIS_QSERVER_ITEM_INFO_INFO_FILE_EXT       AVIS_X_SAMPLE_EXTENSION            
#define AVIS_QSERVER_ITEM_INFO_FILENAME            AVIS_X_SAMPLE_FILE                 
#define AVIS_QSERVER_ITEM_INFO_FILESIZE            AVIS_X_SAMPLE_SIZE                 
#define AVIS_QSERVER_ITEM_INFO_SAMPLE_TYPE         AVIS_X_SAMPLE_TYPE                 
#define AVIS_QSERVER_ITEM_INFO_SAMPLE_REASON       AVIS_X_SAMPLE_REASON               
#define AVIS_QSERVER_ITEM_INFO_DISK_GEOMETRY       AVIS_X_SAMPLE_GEOMETRY             
#define AVIS_QSERVER_ITEM_INFO_STATUS              AVIS_X_SAMPLE_STATUS               
#define AVIS_QSERVER_ITEM_INFO_SUBMISSION_ROUTE    AVIS_X_SAMPLE_SUBMISSION_ROUTE     
#define AVIS_QSERVER_ITEM_CHECKSUM_METHOD          AVIS_X_CHECKSUM_METHOD              
#define AVIS_QSERVER_ITEM_INFO_UUID  	           AVIS_X_SAMPLE_UUID                  
#define AVIS_QSERVER_ITEM_INFO_FILEID	           AVIS_X_SAMPLE_FILEID                
#define AVIS_QSERVER_ITEM_INFO_CHANGES             AVIS_X_SAMPLE_CHANGES  // _T("X-Sample-Changes")                                                                                
#endif                                                                                           




            