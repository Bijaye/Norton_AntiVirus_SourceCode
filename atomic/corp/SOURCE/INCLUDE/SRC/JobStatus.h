// Copyright (C) 1997 Symantec Corporation
//*****************************************************************************
// $Header:   S:/INCLUDE/VCS/JobStatus.h_v   1.12   06 Aug 1997 20:56:36   DBUCHES  $
//
// Description: 
//
//*****************************************************************************
// $Log:   S:/INCLUDE/VCS/JobStatus.h_v  $
// 
//    Rev 1.12   06 Aug 1997 20:56:36   DBUCHES
// Added stopped ID.
// 
//    Rev 1.11   06 Aug 1997 20:10:14   DBUCHES
// Added JDF_STATUS_STOPPED.
// 
//    Rev 1.10   15 Jul 1997 15:45:48   TSmith
// Added defines for cancelled job status.
// 
//    Rev 1.9   25 Jun 1997 11:57:38   TSmith
// Added additional status flag.
// 
//    Rev 1.8   18 Jun 1997 12:44:16   TSmith
// Added 'szJobName' to status struct.
// 
//    Rev 1.6   16 Jun 1997 06:44:12   TSmith
// Moved JDF_STATUS_... strings defines here from Descriptor.h.
// 
//    Rev 1.5   11 Jun 1997 15:42:44   TSmith
// Using TCHARS instead of WCHARS. Build as either Unicode or Ansi. No more
// dual support headaches.
// 
//    Rev 1.4   11 Jun 1997 14:25:48   TSmith
// Removed MAX_PATH & replaced with numeric constant for compatibility
// with IDL inclusion of this file.
// 
//    Rev 1.3   10 Jun 1997 16:01:46   TSmith
// Added struct member 'dwSize'.
// 
//    Rev 1.2   08 Jun 1997 21:13:54   TSmith
// Changed char buffer member size from MAX_PATH to MAX_COMPUTERNAME_LENGTH + 1.
// 
//    Rev 1.1   08 Jun 1997 18:44:16   TSmith
// Renamed a couple of #define's due to nameing conflict with preexisting macro.
// 
//    Rev 1.0   08 Jun 1997 17:25:14   TSmith
// Initial revision.
//*****************************************************************************
#if !defined( __JobStatus_h__ )
#define       __JobStatus_h__

#define MIF_RESULT_FAILURE                  0x00000000
#define MIF_RESULT_SUCCESS                  0x00000001

#define JOB_STATUS_PENDING                  0x00000001
#define JOB_STATUS_PROCESSING               0x00000002
#define JOB_STATUS_FAILURE                  0x00000004
#define JOB_STATUS_SUCCESS                  0x00000008
#define JOB_STATUS_CANCELLED                0x00000010
#define JOB_STATUS_STOPPED					0x00000020
#define JOB_STATUS_MIXED                    0x0000000C
#define JOB_STATUS_UNKNOWN                  0xffffffff

#define JOB_STATUS_ALREADY_IN_PROGRESS      0x10000001
#define JOB_STATUS_NOT_IN_PROGRESS          0x10000002
#define JOB_STATUS_BAD_JOB_NAME             0x10000003
#define JOB_STATUS_DISK_FULL                0x10000004
#define JOB_STATUS_DIR_CREATE_FAILED        0x10000005
#define JOB_STATUS_DIR_REMOVE_FAILED        0x10000006
#define JOB_STATUS_FILE_CLEANUP_FAILED      0x10000007
#define JOB_STATUS_RESULT_FILE_NOT_FOUND    0x10000008
#define JOB_STATUS_RESOURCE_NOT_FOUND       0x10000009
#define JOB_STATUS_NO_MEMORY                0x1000000A
#define JOB_STATUS_MAX_REMOTE_PROCESS       0x1000000B
#define JOB_STATUS_CREATE_PROCESS_FAIL      0x1000000C
#define JOB_STATUS_REMOTE_PROCESS_BEGIN     0x1000000D
#define JOB_STATUS_REMOTE_PROCESS_COMPLETE  0x1000000E
#define JOB_STATUS_PRECOMPLETE_AGENT_STOP   0x1000000F

//
// The following strings will be written to the
// Job Description File and used to display and
// interpret job status.
//
// *** DO NOT TRANSLATE ***
//
#define JDF_STATUS_SUCCESS                  TEXT( "SUCCESS"     )
#define JDF_STATUS_FAILURE                  TEXT( "FAILURE"     )
#define JDF_STATUS_PENDING                  TEXT( "PENDING"     )
#define JDF_STATUS_PROCESSING               TEXT( "PROCESSING"  )
#define JDF_STATUS_CANCELLED                TEXT( "CANCELLED"   )
#define JDF_STATUS_STOPPED					TEXT( "STOPPED" 	)

#define JOB_STATUS_MAJOR_VERSION            0x0001
#define JOB_STATUS_MINOR_VERSION            0X0000

typedef struct _JOB_STATUS_PACKET
    {
    WORD    wMajorVer;
    WORD    wMinorVer;
    DWORD   dwStatus;
    TCHAR   szTargetName[ 15 + 1 ];
    TCHAR   szJobName[ MAX_PATH ];
    } JOB_STATUS_PACKET, *PJOB_STATUS_PACKET;

#endif     // __JobStatus_h__
