// Copyright (C) 1997 Symantec Corporation
//*****************************************************************************
// $Header:   S:/INCLUDE/VCS/DistribCommand.h_v   1.5   08 Jul 1997 09:21:20   TSmith  $
//
// Description: Type definition file for the DISTRIB_COMMAND_PACKET struct.
//              This packet is transmitted to the SymAdminSvc to instruct it
//              to perfrom processing on a distribution job file.
//
//*****************************************************************************
// $Log:   S:/INCLUDE/VCS/DistribCommand.h_v  $
// 
//    Rev 1.5   08 Jul 1997 09:21:20   TSmith
// Added defines for three additional commands for remove, resubmit and resume.
// 
//    Rev 1.4   16 Jun 1997 17:00:18   TSmith
// Defined ERROR_UNKNOWN_PACKET.
// 
//    Rev 1.3   11 Jun 1997 15:43:42   TSmith
// Using TCHAR instead of WCHAR. Build as either Unicode or Ansi, one or the
// other. No more dual support headaches.
// 
//    Rev 1.2   11 Jun 1997 14:26:58   TSmith
// Removed MAX_PATH & replaced with numeric constant for compatibility
// with IDL inclusion of this file.
// 
//    Rev 1.1   04 Jun 1997 17:51:54   TSmith
// Took away bad TCHAR struct member & replaced with WCHAR. The TCHARS caused
// "undefined" behavior since the distribution services are built as Unicode,
// the apps as Ansi and both include this file.
// 
//    Rev 1.0   30 May 1997 11:06:02   TSmith
// Initial revision.
//*****************************************************************************
#if !defined( __DistribCommand_h__ )
#define       __DistribCommand_h__

//
// Commands supported by NAdminSvc
//
#define COMMAND_START_JOB           0x00000001
#define COMMAND_STOP_JOB            0x00000002
#define COMMAND_REMOVE_JOB          0x00000003
#define COMMAND_RESUBMIT_JOB        0x00000004
#define COMMAND_RESUME_JOB          0x00000005

//
// Major and minor version of this structure. These
// numbers should be incremented if any significant
// changes are made to this structure.
//
#define PACKET_MAJOR_VERSION    0x0001
#define PACKET_MINOR_VERSION    0x0000

#define ERROR_UNKNOWN_PACKET    0xffffffff

typedef struct _DISTRIB_COMMAND_PACKET
    {
    WORD    wMajorVer;
    WORD    wMinorVer;
    DWORD   dwCommand;
    TCHAR   szJobFileName[ 260 ];
    } DISTRIB_COMMAND_PACKET, *PDISTRIB_COMMAND_PACKET;

#endif // __DistribCommand_h__
