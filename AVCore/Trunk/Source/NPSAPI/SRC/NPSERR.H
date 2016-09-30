/////////////////////////////////////////////////////////////////////
// NPSSVSTR.H - Created - 09/27/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NPSAPI/VCS/npserr.h_v   1.0   29 May 1997 00:45:10   ksackin  $
//
// Description:
//    Here is where the string IDs are going to go for the Norton
//    Program Scheduler service.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NPSAPI/VCS/npserr.h_v  $
// 
//    Rev 1.0   29 May 1997 00:45:10   ksackin
// Initial revision.
// 
//    Rev 1.0   19 May 1997 00:11:38   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__NPSSVSTR_H)
#define __NPSSVSTR_H

/////////////////////////////////////////////////////////////////////
// Error codes


/////////////////////////////////////////////////////////////////////
// General Error codes
#define IDS_ERR_REGISTERSERVICECTRLR_FAILED       0x0101
#define IDS_ERR_BEGINTHREAD_FAILED                0x0102
#define IDS_ERR_CREATEEVENT_FAILED                0x0103
#define IDS_ERR_MEMORY_ALLOCATION                 0x0104
#define IDS_ERR_STARTLISTEN                       0x0105

/////////////////////////////////////////////////////////////////////
// Norton Program Scheduler Error Codes

#define IDS_ERR_CHECKEVENTS                       0x0201
#define IDS_ERR_LOADEVENTS                        0x0202

#define IDS_ERR_LOADING_SCHEDULE_DAT              0x0210
#define IDS_ERR_BACKUP_SCHEDULE_DAT               0x0211
#define IDS_ERR_OPEN_SCHEDULE_DAT                 0x0212
#define IDS_ERR_VERSION_SCHEDULE_DAT              0x0213
#define IDS_ERR_CHECKSUM_SCHEDULE_DAT             0x0214
#define IDS_ERR_INVALIDENTRY_SCHEDULE_DAT         0x0215
#define IDS_ERR_OPEN_EVENTS_FILE                  0x0216
#define IDS_ERR_WRITING_EVENTS_FILE               0x0217
#define IDS_ERR_WRITING_EVENTS_FILE_HEADER        0x0218

#define IDS_ERR_EXECUTION_FAILURE                 0x0219


/////////////////////////////////////////////////////////////////////
// Error codes for ErrorHandler function.

#define ID_ERR_REGISTERSERVICECTRLR_FAILED        IDS_ERR_REGISTERSERVICECTRLR_FAILED | \
                                                     ID_ERR_ALL
#define ID_ERR_BEGINTHREAD_FAILED                 IDS_ERR_BEGINTHREAD_FAILED | \
                                                     ID_ERR_ALL
#define ID_ERR_CREATEEVENT_FAILED                 IDS_ERR_CREATEEVENT_FAILED | \
                                                     ID_ERR_ALL
#define ID_ERR_MEMORY_ALLOCATION                  IDS_ERR_MEMORY_ALLOCATION | \
                                                     ID_ERR_ALL
#define ID_ERR_STARTLISTEN                        IDS_ERR_STARTLISTEN | \
                                                     ID_ERR_ALL


#define ID_ERR_CHECKEVENTS                        IDS_ERR_CHECKEVENTS | \
                                                     ID_ERR_ALL
#define ID_ERR_LOADEVENTS                         IDS_ERR_LOADEVENTS | \
                                                     ID_ERR_ALL

#define ID_ERR_LOADING_SCHEDULE_DAT               IDS_ERR_LOADING_SCHEDULE_DAT | \
                                                     ID_ERR_ALL
#define ID_ERR_BACKUP_SCHEDULE_DAT                IDS_ERR_BACKUP_SCHEDULE_DAT | \
                                                     ID_ERR_ALL
#define ID_ERR_OPEN_SCHEDULE_DAT                  IDS_ERR_OPEN_SCHEDULE_DAT | \
                                                     ID_ERR_ALL
#define ID_ERR_VERSION_SCHEDULE_DAT               IDS_ERR_VERSION_SCHEDULE_DAT | \
                                                     ID_ERR_ALL
#define ID_ERR_CHECKSUM_SCHEDULE_DAT              IDS_ERR_CHECKSUM_SCHEDULE_DAT | \
                                                     ID_ERR_ALL
#define ID_ERR_INVALIDENTRY_SCHEDULE_DAT          IDS_ERR_INVALIDENTRY_SCHEDULE_DAT | \
                                                     ID_ERR_ALL
#define ID_ERR_OPEN_EVENTS_FILE                   IDS_ERR_OPEN_EVENTS_FILE | \
                                                     ID_ERR_ALL
#define ID_ERR_WRITING_EVENTS_FILE                IDS_ERR_WRITING_EVENTS_FILE | \
                                                     ID_ERR_ALL
#define ID_ERR_WRITING_EVENTS_FILE_HEADER         IDS_ERR_WRITING_EVENTS_FILE_HEADER | \
                                                     ID_ERR_ALL

#define ID_ERR_EXECUTION_FAILURE                  IDS_ERR_EXECUTION_FAILURE | \
                                                     ID_ERR_ALL

#endif // if !defined(__NPSSVSTR_H)
