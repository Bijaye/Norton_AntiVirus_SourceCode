//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: EV_EVENT_IS_ALERT
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_IS_ALERT                ((DWORD)0x80FF0001L)

//
// MessageId: EV_EVENT_SCAN_STOP
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_SCAN_STOP               ((DWORD)0x40FF0002L)

//
// MessageId: EV_EVENT_SCAN_START
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_SCAN_START              ((DWORD)0x40FF0003L)

//
// MessageId: EV_EVENT_PATTERN_UPDATE
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_PATTERN_UPDATE          ((DWORD)0x40FF0004L)

//
// MessageId: EV_EVENT_INFECTION
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_INFECTION               ((DWORD)0xC0FF0005L)

//
// MessageId: EV_EVENT_FILE_NOT_OPEN
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_FILE_NOT_OPEN           ((DWORD)0x80FF0006L)

//
// MessageId: EV_EVENT_LOAD_PATTERN
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_LOAD_PATTERN            ((DWORD)0x40FF0007L)

//
// MessageId: EV_EVENT_CHECKSUM
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_CHECKSUM                ((DWORD)0xC0FF000AL)

//
// MessageId: EV_EVENT_TRAP
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_TRAP                    ((DWORD)0xC0FF000BL)

//
// MessageId: EV_EVENT_CONFIG_CHANGE
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_CONFIG_CHANGE           ((DWORD)0x40FF000CL)

//
// MessageId: EV_EVENT_SHUTDOWN
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_SHUTDOWN                ((DWORD)0x40FF000DL)

//
// MessageId: EV_EVENT_STARTUP
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_STARTUP                 ((DWORD)0x40FF000EL)

//
// MessageId: EV_EVENT_PATTERN_DOWNLOAD
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_PATTERN_DOWNLOAD        ((DWORD)0x40FF0010L)

//
// MessageId: EV_EVENT_TOO_MANY_VIRUSES
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_TOO_MANY_VIRUSES        ((DWORD)0x40FF0011L)

//
// MessageId: EV_EVENT_FWD_TO_QSERVER
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_FWD_TO_QSERVER          ((DWORD)0x40FF0012L)

//
// MessageId: EV_EVENT_SCANDLVR
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_SCANDLVR                ((DWORD)0x40FF0013L)

//
// MessageId: EV_EVENT_BACKUP
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_BACKUP                  ((DWORD)0xC0FF0014L)

//
// MessageId: EV_EVENT_SCAN_ABORT
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_SCAN_ABORT              ((DWORD)0xC0FF0015L)

//
// MessageId: EV_EVENT_RTS_LOAD_ERROR
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_RTS_LOAD_ERROR          ((DWORD)0xC0FF0016L)

//
// MessageId: EV_EVENT_RTS_LOAD
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_RTS_LOAD                ((DWORD)0xC0FF0017L)

//
// MessageId: EV_EVENT_RTS_UNLOAD
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_RTS_UNLOAD              ((DWORD)0xC0FF0018L)

//
// MessageId: EV_EVENT_REMOVE_CLIENT
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_REMOVE_CLIENT           ((DWORD)0x40FF0019L)

//
// MessageId: EV_EVENT_SCAN_DELAYED
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_SCAN_DELAYED            ((DWORD)0x40FF001AL)

//
// MessageId: EV_EVENT_SCAN_RESTART
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_SCAN_RESTART            ((DWORD)0x40FF001BL)

//
// MessageId: EV_EVENT_ADD_SAVROAMCLIENT_TOSERVER
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_ADD_SAVROAMCLIENT_TOSERVER ((DWORD)0x40FF001CL)

//
// MessageId: EV_EVENT_REMOVE_SAVROAMCLIENT_FROMSERVER
//
// MessageText:
//
//  
//  %1
//
#define EV_EVENT_REMOVE_SAVROAMCLIENT_FROMSERVER ((DWORD)0x40FF001DL)

