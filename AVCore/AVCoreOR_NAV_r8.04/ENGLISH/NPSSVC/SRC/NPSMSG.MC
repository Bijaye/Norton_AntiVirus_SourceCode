;;///////////////////////////////////////////////////////////////////
;; NPSMSG.MC - Created - 09/27/96
;;
;; Copyright 1992-1996 Symantec, Peter Norton Product Group
;;///////////////////////////////////////////////////////////////////
;; $Header:   N:/NPSSVC/VCS/npsmsg.mcv   1.0   19 May 1997 00:13:14   KSACKIN  $
;;
;; Description:
;;    NT Event Log Messages for the Norton Program Scheduler Service.
;;
;;///////////////////////////////////////////////////////////////////
;;
;; $Log:   N:/NPSSVC/VCS/npsmsg.mcv  $
;  
;     Rev 1.0   19 May 1997 00:13:14   KSACKIN
;  Initial revision.
;;
;;///////////////////////////////////////////////////////////////////

MessageIdTypedef=WORD

MessageID=0x8101
SymbolicName=IDM_ERR_REGISTERSERVICECTRLR_FAILED
Language=English
Unable to Initialize Service Control Manager
.

MessageID=0x8102
SymbolicName=IDM_ERR_BEGINTHREAD_FAILED
Language=English
Unable to create a new thread (_beginthread)
.

MessageID=0x8103
SymbolicName=IDM_ERR_CREATEEVENT_FAILED
Language=English
Unable to create event (CreateEvent)
.

MessageID=0x8104
SymbolicName=IDM_ERR_MEMORY_ALLOCATION
Language=English
Memory allocation error
.

MessageID=0x8105
SymbolicName=IDM_ERR_STARTLISTEN
Language=English
Unable to start RPC server
.


MessageID=0x8201
SymbolicName=IDM_ERR_CHECKEVENTSTEXT
Language=English
Error Checking Events
.

MessageID=0x8202
SymbolicName=IDM_ERR_LOADEVENTS
Language=English
Error Loading Events
.


MessageID=0x8210
SymbolicName=IDM_ERR_LOADING_SCHEDULE_DAT
Language=English
Error Loading Schedule.dat
.

MessageID=0x8211
SymbolicName=IDM_ERR_BACKUP_SCHEDULE_DAT
Language=English
Error backing up Schedule.dat
.

MessageID=0x8212
SymbolicName=IDM_ERR_OPEN_SCHEDULE_DAT
Language=English
Unable to open Schedule.dat
.

MessageID=0x8213
SymbolicName=IDM_ERR_VERSION_SCHEDULE_DAT
Language=English
Incompatible Version number in Schedule.dat
.

MessageID=0x8214
SymbolicName=IDM_ERR_CHECKSUM_SCHEDULE_DAT
Language=English
Schedule.dat contains and invalid checksum value
.

MessageID=0x8215
SymbolicName=IDM_ERR_INVALIDENTRY_SCHEDULE_DAT
Language=English
Invalid entry in Schedule.dat
.

MessageID=0x8216
SymbolicName=IDM_ERR_OPEN_EVENTS_FILE
Language=English
Error opening events file
.

MessageID=0x8217
SymbolicName=IDM_ERR_WRITING_EVENTS_FILE
Language=English
Error writing events file
.

MessageID=0x8218
SymbolicName=IDM_ERR_WRITING_EVENTS_FILE_HEADER
Language=English
Error writing events file header
.


MessageID=0x219
SymbolicName=IDM_ERR_EXECUTION_FAILURE
Language=English
Unable to execute program [%1]
.


