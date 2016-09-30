/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/quar32.h_v   1.2   30 Jun 1998 17:42:58   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// Quar32.h: public defined values for users of the Quarantine interfaces.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/quar32.h_v  $
// 
//    Rev 1.2   30 Jun 1998 17:42:58   jtaylor
// Added unable to initialize error code.
//
//    Rev 1.1   10 Jun 1998 20:31:16   jtaylor
// Added a new error code for unable to connect to internet.
//
//    Rev 1.0   26 May 1998 14:40:10   DBuches
// Initial revision.
//
//    Rev 1.0   26 May 1998 14:38:46   DBuches
// Moved from AVCORE.
//
//    Rev 1.24   21 May 1998 11:49:26   jtaylor
// Added success code for undeletable virus found
//
//    Rev 1.23   20 May 1998 22:38:16   jtaylor
// Added support for S_BACKEND_ALL_CLEAN
//
//    Rev 1.22   10 May 1998 22:47:22   SEDWARD
// Added E_INVALID_QUARANTINE_SERVER, E_ERROR_READING_OPTIONS, and
// E_ERROR_WRITING_OPTIONS.
//
//    Rev 1.21   30 Apr 1998 20:06:58   jtaylor
// Added E_USER_CANCELLED error code.
//
//    Rev 1.20   26 Apr 1998 21:53:46   jtaylor
// Added error for submission packet being larger then the maximum size.
//
//    Rev 1.19   26 Apr 1998 17:25:58   jtaylor
// Updated the error codes to be unique.
//
//    Rev 1.18   24 Apr 1998 23:53:40   jtaylor
// Add CMaliciousCodeFilter related errors.
//
//    Rev 1.17   23 Apr 1998 19:22:08   jtaylor
// added error/success codes for Socket server.
//
//    Rev 1.16   22 Apr 1998 23:04:14   JTaylor
// Added some error codes for the file stripper class.
//
//    Rev 1.15   20 Apr 1998 22:27:28   SEDWARD
// Added ACKNOWLEDGED bit.
//
//    Rev 1.14   19 Apr 1998 23:51:52   JTaylor
// added error codes for Scan and Deliver.
//
//    Rev 1.13   18 Apr 1998 17:33:10   JTaylor
// Added error for failure to get a GUID.
//
//    Rev 1.12   13 Apr 1998 13:39:20   JTaylor
// Added a new file type QFILE_TYPE_NORMAL.  This should be the primary
// type used to add files to Quarantine.
//
//    Rev 1.11   10 Apr 1998 20:05:34   JTaylor
// Added S_NO_VIRUS_FOUND success code.
//
//    Rev 1.10   10 Apr 1998 19:28:36   JTaylor
// Fixed File Types.  Rev 1.9 was a mistake (old copy of file)
//
//    Rev 1.8   10 Apr 1998 12:37:00   SEDWARD
// Updated some HRESULT codes.
//
//    Rev 1.7   10 Apr 1998 00:06:56   SEDWARD
// Added a couple of new HRESULTs, and a macro for the QuarOpts.dat filename.
//
//    Rev 1.6   07 Apr 1998 21:45:16   SEDWARD
// Removed semicolons from the end of the HRESULT define statements.
//
//    Rev 1.5   06 Apr 1998 19:29:30   JTaylor
// Added a new Quarantine error code for invalid Quarantine Directory.  Added Quarantine Options error codes.
//
//    Rev 1.4   30 Mar 1998 19:53:48   SEDWARD
// Added password stuff.
//
//    Rev 1.3   30 Mar 1998 20:05:46   JTaylor
// Added another error code
//
//    Rev 1.2   30 Mar 1998 10:48:30   JTaylor
// Added a few more error codes for IQuarantineItem support
//
//    Rev 1.1   25 Mar 1998 21:54:32   JTaylor
// Added error codes for QuarantineDll.
//
//    Rev 1.0   16 Mar 1998 19:19:18   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////


#ifndef QUAR32_PUBLIC_HEADER
#define QUAR32_PUBLIC_HEADER


///////////////////////////////////////////////////////////////////////////////
// QUARANTINE DLL SECTION
///////////////////////////////////////////////////////////////////////////////


// HRESULT error codes for Quarantine Interfaces.

// Object uninitialized (an initialize function was not called)
#define E_UNINITIALIZED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF, 1)

// There was some error while trying to retrieve one of the Quarantine Directories.
#define E_UNABLE_TO_GET_QUARANTINE_PATH MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF, 2)

// There was some error while trying to retrieve one of the Quarantine Directories.
#define E_QUARANTINE_DIRECTORY_INVALID MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF, 3)

// There was an error while trying to get a GUID for a file being added to Quarantine.
#define E_UNABLE_TO_CREATE_GUID MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF, 4)

// There was an error while manipulating a string
#define E_UNABLE_TO_MANIPULATE_STRING MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF, 5)

// The user cancelled the transaction
#define E_USER_CANCELLED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF, 6)

// The user cancelled the transaction
#define E_UNABLE_TO_CREATE_THREAD MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF, 7)

// CDefUtils related errors
#define CDEFUTILS_ERROR_BASE 0x00a

// There was an error while initializing the CDefUtils class
#define E_UNABLE_TO_INITIALIZE_DEFUTILS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,CDEFUTILS_ERROR_BASE + 0)

// There was an error while initializing the CDefUtils class
#define E_DEFUTILS_NOT_CREATED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,CDEFUTILS_ERROR_BASE + 1)

// There was an error while trying to update to the newest defs.
#define E_UNABLE_TO_USE_NEWEST_DEFS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,CDEFUTILS_ERROR_BASE + 2)

// There was an error while trying to get the date of the current defs.
#define E_UNABLE_TO_GET_CURRENT_DEFS_DATE MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,CDEFUTILS_ERROR_BASE + 3)

// There was an error while trying to get the directory of the current defs.
#define E_UNABLE_TO_GET_CURRENT_DEFS_DIR MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,CDEFUTILS_ERROR_BASE + 4)


// AP related errors
#define AP_ERROR_BASE  0x010

// There was an error while trying to load navap32.dll
#define E_UNABLE_TO_LOAD_NAVAP32 MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,AP_ERROR_BASE + 0)

// There was an error while trying to dissable AP
#define E_UNABLE_TO_DISSABLE_AP MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,AP_ERROR_BASE + 1)

// There was an error while trying to dissable AP
#define E_UNABLE_TO_ENABLE_AP MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,AP_ERROR_BASE + 2)

// There was an error while copying a string
#define E_COPY_FAILED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,AP_ERROR_BASE + 3)


// AVAPI related errors
#define AVAPI_ERROR_BASE 0x020

// There was an error initializing the virus scanning engine
#define E_UNABLE_TO_INIT_AVAPI MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,AVAPI_ERROR_BASE + 0)

// There was an error initializing the virus database
#define E_UNABLE_TO_INIT_VIRUS_DB MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,AVAPI_ERROR_BASE + 1)

// There was no virus detected during the scan.
#define S_NO_VIRUS_FOUND MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_ITF,AVAPI_ERROR_BASE + 2)

// There was no virus detected during the scan.
#define S_UNDELETABLE_VIRUS_FOUND MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_ITF,AVAPI_ERROR_BASE + 3)

// Here is an error for each of the AVAPI error codes
#define VSTATUS_ERROR_BASE 0x030
#define E_AVAPI_VS_MEMALLOC         MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_MEMALLOC)
#define E_AVAPI_VS_FILE_ERROR       MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_FILE_ERROR)
#define E_AVAPI_VS_CRIT_ERROR       MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_CRIT_ERROR)
#define E_AVAPI_VS_CANT_REPAIR      MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_CANT_REPAIR)
#define E_AVAPI_VS_DRIVE_ERROR      MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_DRIVE_ERROR)
#define E_AVAPI_VS_INVALID_ARG      MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_INVALID_ARG)
#define E_AVAPI_VS_ENTRY_UNAVAIL    MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_ENTRY_UNAVAIL)
#define E_AVAPI_VS_NO_EXIST         MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_NO_EXIST)
#define E_AVAPI_VS_WRONG_VERSION    MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_WRONG_VERSION)
#define E_AVAPI_VS_CANT_DELETE      MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_CANT_DELETE)
#define E_AVAPI_VS_NO_ACCESS        MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_NO_ACCESS)
#define E_AVAPI_VS_SORTERROR        MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_SORTERROR)
#define E_AVAPI_VS_INSCAN_CREATION  MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,VSTATUS_ERROR_BASE +VS_INSCAN_CREATION)



// File Manipulation related errors
#define FILE_MANIPULATION_ERROR_BASE 0x020

// There was an error creating a file
#define E_FILE_CREATE_FAILED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,FILE_MANIPULATION_ERROR_BASE + 0)

// There was an error writing to a file
#define E_FILE_WRITE_FAILED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,FILE_MANIPULATION_ERROR_BASE + 1)

// There was an error reading from a file
#define E_FILE_READ_FAILED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,FILE_MANIPULATION_ERROR_BASE + 2)

// There was an error closing a file
#define E_FILE_CLOSE_FAILED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,FILE_MANIPULATION_ERROR_BASE + 3)

// There was an error deleting a file
#define E_FILE_DELETE_FAILED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,FILE_MANIPULATION_ERROR_BASE + 4)

// There was an error manipulating the name of a file
#define E_FILE_FILENAME_MANIPULATION_FAILED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,FILE_MANIPULATION_ERROR_BASE + 5)

// There was an error moving the location of a file pointer
#define E_FILE_SET_POSITON_FAILED MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,FILE_MANIPULATION_ERROR_BASE + 6)



// IQuarantineItem related errors
#define IQUARANTINE_ITEM_ERROR_BASE 0x020

// There is an invalid file loaded into Quarantine
#define E_INVALID_QUARANTINE_FILE MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_ITEM_ERROR_BASE + 0)

#define E_UNABLE_TO_CREATE_QUARANTINE_ITEM MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_ITEM_ERROR_BASE + 1)

#define E_UNABLE_TO_SELECT_QUARANTINE_ITEM_PROTOCOL MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_ITEM_ERROR_BASE + 2)

#define E_UNABLE_TO_FORWARD_QUARANTINE_ITEM MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_ITEM_ERROR_BASE + 3)


// IQuarantineOptions related errors
#define IQUARANTINE_OPTIONS_ERROR_BASE 0x030

// There was a problem reading in the options file
#define E_UNABLE_TO_READ_OPTIONS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_OPTIONS_ERROR_BASE + 0)

// There was a problem writing the options file
#define E_UNABLE_TO_WRITE_OPTIONS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_OPTIONS_ERROR_BASE + 1)

// There was a problem creating a default options file
#define E_UNABLE_TO_CREATE_DEFAULT_OPTIONS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_OPTIONS_ERROR_BASE + 2)

// The options file does not exist
#define E_UNABLE_TO_FIND_OPTIONS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_OPTIONS_ERROR_BASE + 3)

// Unable to create an instance of the options object
#define E_UNABLE_TO_CREATE_OPTIONS_OBJECT MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_OPTIONS_ERROR_BASE + 4)

// Unable to create an instance of the options object
#define E_INVALID_QUARANTINE_SERVER MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_OPTIONS_ERROR_BASE + 5)

// An error occured while reading the options
#define E_ERROR_READING_OPTIONS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_OPTIONS_ERROR_BASE + 6)

// An error occured while writing the options
#define E_ERROR_WRITING_OPTIONS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,IQUARANTINE_OPTIONS_ERROR_BASE + 7)


// IScanDeliverDLL related errors
#define ISCANDELIVERDLL_ERROR_BASE 0x040

// This error is returned if there is a problem creating the CVirusFilter class
#define E_UNABLE_TO_CREATE_VIRUS_FILTER MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 0)

// This error is returned if there is a problem initializing the CVirusFilter class
#define E_UNABLE_TO_START_VIRUS_FILTER MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 1)

// There was an error while calling the reset method of the SARC packaging class
#define E_UNABLE_TO_START_PACKAGING_CLASS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 2)

// There was an error while setting an info field in the SARC packaging class
#define E_UNABLE_TO_SET_INFO_FIELD MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 3)

// There was an error when trying to submit a sample to the SARC packaging class
#define E_UNABLE_TO_ADD_FILE_TO_PACKAGE MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 4)

// There was an error when trying to submit a sample to the SARC packaging class
#define E_UNABLE_TO_FINISH_PACKAGE MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 5)

// This error is returned if there is a problem creating the CVirusFilter class
#define E_UNABLE_TO_CREATE_FILE_STRIPPER MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 6)

// This error is returned if there is a problem initializing the CVirusFilter class
#define E_UNABLE_TO_STRIP_FILE MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 7)

// This error is returned if there is a problem creating the CSocketClient class
#define E_UNABLE_TO_CREATE_SOCKETCLIENT MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 8)

// This error is returned if there is a problem connecting to the socket server
#define E_UNABLE_TO_GET_BACKEND_STATUS MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 9)

// The backend was contacted and said to send the package
#define S_BACKEND_SEND_PACKAGE MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 10)

// The backend was contacted and said package contained false positives
#define S_BACKEND_FALSE_POSITIVE MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 11)

// The backend was contacted and said the server was busy
#define S_BACKEND_BUSY MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 12)

// This error is returned if there is a problem creating the CMaliciousCodeFilter class
#define E_UNABLE_TO_CREATE_FILE_MALICIOUSCODEFILTER  MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 13)

// This error is returned if there is a problem filtering a file.
#define E_UNABLE_TO_FILTER_FILE MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 14)

// This error is returned if the package created for submission is larger than its mazimimum size.
#define E_PACKAGE_OVER_MAX_SUBMISSION_SIZE MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 15)

// The backend was contacted and said the package contained only clean files.
#define S_BACKEND_ALL_CLEAN MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 16)

// The backend was contacted and said the package contained only clean files.
#define E_BACKEND_UNABLE_TO_CONNECT_TO_INTERNET MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 17)

// Scan and Deliver was unable to initialize due to a problem with the DLLs that it uses.
#define E_UNABLE_TO_INITIALIZE MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,ISCANDELIVERDLL_ERROR_BASE  + 18)



// Quarantine file status
#define QFILE_STATUS_HEADER_INCOMPLETE                      0x00000000
#define QFILE_STATUS_HEADER_COMPLETE                        0x00000001
#define QFILE_STATUS_QUARANTINED                            0x00000002
#define QFILE_STATUS_SUBMITTED                              0x00000004
#define QFILE_STATUS_BACKUP_FILE                            0x00000008
#define QFILE_STATUS_ACKNOWLEDGED                           0x00000010

// file types
#define QFILE_TYPE_NORMAL                                   0x00000000
#define QFILE_TYPE_REPAIRABLE                               0x00000001
#define QFILE_TYPE_COMPRESSED                               0x00000002




///////////////////////////////////////////////////////////////////////////////
// QUARANTINE OPTIONS SECTION
///////////////////////////////////////////////////////////////////////////////


#define QUARANTINE_OPTIONS_FILENAME                     "QuarOpts.dat"

// values used as indexes into the allowable operations on specific file status items
#define QFILEOPS_INDEX_QUARANTINED                      0
#define QFILEOPS_INDEX_SUBMITTED                        1
#define QFILEOPS_INDEX_BACKUP_FILE                      2

// QuarOpts INI strings
#define QOPTS_SECTION_FORWARDING_ENABLED                "FORWARDING_ENABLED"
#define QOPTS_SECTION_SCAN_AT_STARTUP                   "SCAN_AT_STARTUP"
#define QOPTS_SECTION_SCAN_REPAIRED_FILES               "SCAN_REPAIRED_FILES"
#define QOPTS_SECTION_SARC_SUBMISSION_ENABLED           "SARC_SUBMISSION_ENABLED"
#define QOPTS_SECTION_QUARANTINE_SERVER                 "QUARANTINE_SERVER"
#define QOPTS_SECTION_QUARANTINE_PASSWORD               "PASSWORD"
#define QOPTS_SECTION_VALID_FORWARD_TYPES               "VALID_FORWARD_TYPES"
#define QOPTS_SECTION_VALID_OPERATIONS_QUARANTINED      "VALID_OPERATIONS_QUARANTINED"
#define QOPTS_SECTION_VALID_OPERATIONS_SUBMITTED        "VALID_OPERATIONS_SUBMITTED"
#define QOPTS_SECTION_VALID_OPERATIONS_REPAIRED         "VALID_OPERATIONS_REPAIRED"
#define QOPTS_KEY_ENABLED                               "Enabled"
#define QOPTS_KEY_QSERVER                               "QServer"
#define QOPTS_KEY_PASSWORD                              "Password"
#define QOPTS_KEY_SUBMITTED                             "Submitted"
#define QOPTS_KEY_REPAIRED                              "Repaired"
#define QOPTS_KEY_QUARANTINED                           "Quarantined"
#define QOPTS_KEY_DELETE                                "Delete"
#define QOPTS_KEY_SCAN                                  "Scan"
#define QOPTS_KEY_REPAIR                                "Repair"
#define QOPTS_KEY_PUTBACK                               "PutBack"
#define QOPTS_KEY_SUBMIT_TO_SARC                        "SubmitToSarc"

// QuarOpts file operations
#define QUAR_OPTS_FILEOP_DELETE                             0x00000001
#define QUAR_OPTS_FILEOP_SCAN                               0x00000002
#define QUAR_OPTS_FILEOP_REPAIR                             0x00000004
#define QUAR_OPTS_FILEOP_PUT_BACK                           0x00000008
#define QUAR_OPTS_FILEOP_SUBMIT_TO_SARC                     0x00000010

// QuarOpts error values
#define QUAR_OPTS_ERROR_DAT_FILE_NOT_FOUND                  0x00000001
#define QUAR_OPTS_ERROR_READING_SCAN_AT_STARTUP             0x00000002
#define QUAR_OPTS_ERROR_READING_FORWARDING_ENABLED          0x00000004
#define QUAR_OPTS_ERROR_READING_SCAN_REPAIRED_FILES         0x00000008
#define QUAR_OPTS_ERROR_READING_SARC_SUBMISSION_ENABLED     0x00000010
#define QUAR_OPTS_ERROR_READING_QUARANTINE_SERVER           0x00000020
#define QUAR_OPTS_ERROR_READING_VALID_FORWARD_TYPES         0x00000040
#define QUAR_OPTS_ERROR_READING_QFILE_ATTRIBUTES            0x00000080
#define QUAR_OPTS_ERROR_WRITING_SCAN_AT_STARTUP             0x00000100
#define QUAR_OPTS_ERROR_WRITING_FORWARDING_ENABLED          0x00000200
#define QUAR_OPTS_ERROR_WRITING_SCAN_REPAIRED_FILES         0x00000400
#define QUAR_OPTS_ERROR_WRITING_SARC_SUBMISSION_ENABLED     0x00000800
#define QUAR_OPTS_ERROR_WRITING_QUARANTINE_SERVER           0x00001000
#define QUAR_OPTS_ERROR_WRITING_VALID_FORWARD_TYPES         0x00002000
#define QUAR_OPTS_ERROR_WRITING_QFILE_ATTRIBUTES            0x00004000

#endif  // QUAR32_PUBLIC_HEADER
