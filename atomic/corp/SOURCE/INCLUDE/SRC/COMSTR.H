// Copyright 1992, 1993 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:/include/VCS/comstr.h_v   1.0   06 Feb 1997 20:52:04   RFULLER  $
//
// Description: String ids for scan(?).
//
// See Also:
//
//**********************************************************************
//
// $Log:   S:/include/VCS/comstr.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:52:04   RFULLER
// Initial revision
// 
//    Rev 1.0   16 Apr 1996 19:26:26   TSmith
// Initial revision.
// 
//    Rev 1.10   06 Jul 1995 20:22:42   JMILLARD
// add NDS_RELOGIN
// 
//    Rev 1.9   02 Jul 1995 13:21:14   JMILLARD
// add COM_NLM_SERVER_DOWN_UNLOAD
// 
//    Rev 1.8   09 Jun 1995 16:27:36   GDZIECI
// Added COM_NO_MEMORY_SCAN_RING3.
// 
//    Rev 1.7   12 Apr 1995 14:53:36   JMILLARD
// split last message into 2 messages with new FILE_HOOK_TRY_PROPRIETARY
// 
//    Rev 1.6   12 Apr 1995 14:40:48   JMILLARD
// add FILE_HOOK_CANT_INSTALL message
// 
//    Rev 1.5   17 Feb 1995 13:28:04   GDZIECI
// Added NDS_LOGIN.
// 
//    Rev 1.4   02 Feb 1995 19:43:50   GDZIECI
// Added NDS_LOGOUT.
// 
//    Rev 1.3   25 Jan 1995 20:11:34   GDZIECI
// Added NDS_INSTALL_SUCCESS.
// 
//    Rev 1.2   29 Dec 1994 09:30:42   GDZIECI
// Added NDS_ERR_INSTALL.
// 
// 
//    Rev 1.1   31 Oct 1994 15:52:08   GDZIECI
// Added MON_SAM_NOACTION; Renamed MON_NO_ACTION to MON_NAV_NOACTION.
// 
// 
//    Rev 1.0   15 Aug 1994 16:14:32   GDZIECI
// Baseline Mario from Vishnu with Win32 awk changes applied
// 
//    Rev 1.28   15 Jul 1994 13:13:50   JMILLARD
// add MAC_CACHED_FILE
// 
//    Rev 1.27   12 Jul 1994 07:39:54   JMILLARD
// add COM_NO_EVAL
// 
//    Rev 1.26   11 May 1994 08:56:42   JMILLARD
// add file hook messages
// 
//    Rev 1.25   23 Dec 1993 11:20:36   JMILLARD
// add BSCAN_NO_LOAD_NLM
// 
//    Rev 1.24   17 Dec 1993 10:47:08   DDREW
// One more
// 
//    Rev 1.23   05 Dec 1993 16:48:00   JMILLARD
// add MON_NO_ACTION
// 
//    Rev 1.22   05 Dec 1993 12:20:32   JMILLARD
// add COM_NO_MEMORY_SCAN_CLOSE
// 
//    Rev 1.21   03 Dec 1993 15:59:14   JMILLARD
// add COM_NO_MEMORY_BACKSCAN
// 
//    Rev 1.20   01 Dec 1993 17:54:58   JMILLARD
// add COM_NO_MEMORY_MEMSCAN
// 
//    Rev 1.19   01 Dec 1993 13:15:40   JMILLARD
// add COM_NO_MEMORY_ENABLE and COM_NO_MEMORY_DISABLE
// 
//    Rev 1.18   16 Nov 1993 17:50:18   JMILLARD
// add COM_NO_MEMORY
// 
//    Rev 1.17   16 Nov 1993 13:17:24   JMILLARD
// add COM_NO_MEMORY
// 
//    Rev 1.16   02 Sep 1993 01:40:58   EHU
// Added some action strings.
// 
//    Rev 1.15   01 Sep 1993 09:55:00   JMILLARD
// add no memory errors
// 
//    Rev 1.14   01 Sep 1993 08:12:52   JMILLARD
// adding scheduling error strings
// 
//    Rev 1.13   30 Aug 1993 10:38:16   DDREW
// BSCAN_INVALID_PATH
// 
//    Rev 1.12   25 Aug 1993 15:26:54   DDREW
// 
//    Rev 1.11   24 Aug 1993 16:08:04   DDREW
// 
//    Rev 1.10   24 Aug 1993 08:45:06   JMILLARD
// add strings
// 
//    Rev 1.9   21 Aug 1993 15:39:50   EHU
// Added COM_SYSTEM_CONSOLE.
// 
//    Rev 1.8   21 Aug 1993 15:28:16   DDREW
// 
//    Rev 1.7   21 Aug 1993 15:10:18   DDREW
// 
//    Rev 1.6   21 Aug 1993 13:35:30   DDREW
// More strings
// 
//    Rev 1.5   20 Aug 1993 18:17:24   DDREW
// 
//    Rev 1.4   20 Aug 1993 18:01:12   DDREW
// Lots of strings
// 
//    Rev 1.3   20 Aug 1993 17:25:08   EHU
// Added SCAN_UNKNOWN_VIRUS.
// 
//    Rev 1.2   20 Aug 1993 12:53:40   DDREW
// 
//    Rev 1.1   20 Aug 1993 12:36:10   DDREW
// Added OSLIB strings
// 
//    Rev 1.0   18 Aug 1993 10:52:42   DDREW
// Initial revision.
//
//**********************************************************************

#ifndef _COMSTR_H
#define _COMSTR_H


// string ids
#define INTERNAL_ERROR          1

#define SCAN_NO_DOS_DATA        2
#define SCAN_NO_MAC_DATA        3
#define SCAN_NO_CACHE_INIT      4
#define SCAN_DOS_NO_DATABASE    5
#define SCAN_UNKNOWN_VIRUS      6

#define OSLIB_BAD_VERS          7
#define OSLIB_NO_SFT3           8 
#define OSLIB_BAD_CLIB          9
#define OSLIB_NO_SUPER          10
#define OSLIB_LOAD_FAIL         11

#define BSCAN_SCHED             12
#define BSCAN_SCHED_START       13
#define BSCAN_SCHED_FINISH      14
#define BSCAN_SCHED_TERM        15
#define BSCAN_IMMED             16
#define BSCAN_IMMED_START       17
#define BSCAN_IMMED_FINISH      18
#define BSCAN_IMMED_TERM        19
#define BSCAN_INOC              20
#define BSCAN_INOC_START        21
#define BSCAN_INOC_FINISH       22
#define BSCAN_INOC_TERM         23
#define BSCAN_UNINOC            24
#define BSCAN_UNINOC_START      25
#define BSCAN_UNINOC_FINISH     26
#define BSCAN_UNINOC_TERM       27
#define BSCAN_INVALID_PATH      28

#define MON_AND                 29
#define MON_ENABLE              30
#define MON_DISABLE             31
#define MON_EMPTY               32
#define MON_FAIL                33
#define MON_MOVE                34
#define MON_RENAME              35
#define MON_ERASE               36
#define MON_SCHED               37
#define MON_INCOMING            38
#define MON_OUTGOING            39
#define MON_INOUTGOING          40
#define MON_SCANALLDOS          41
#define MON_SCANDOSEXE          42
#define MON_SCANMACEXE          43
#define MON_KNOWN               44
#define MON_UNKNOWN             45
#define MON_KNOWNUNKNOWN        46
#define MON_VIRUSES             47

#define COM_SYSTEM_CONSOLE      48

#define SCHED_TOO_MANY_SCANS_RESCHED 49
#define SCHED_TOO_MANY_SCANS_CANCEL  50
#define SCHED_NO_MEM_RESCHED    51 
#define SCHED_NO_MEM_CANCEL     52

#define MON_NLM_LAUNCH          53
#define MON_WS_LOGOUT           54

#define COM_CONSOLE_FMT         55
#define COM_NO_MEMORY           56
#define COM_NO_MEMORY_ENABLE    57
#define COM_NO_MEMORY_DISABLE   58
#define COM_NO_MEMORY_MEMSCAN   59
#define COM_NO_MEMORY_BACKSCAN  60
#define COM_NO_MEMORY_SCAN_CLOSE 61
#define COM_NO_MEMORY_SCAN_SFT3 62

#define MON_NAV_NOACTION        63
#define BSCAN_NO_LOAD_NLM       64

#define FILE_HOOK_OVERWRITE     65
#define FILE_HOOK_HOOKED        66

#define COM_NO_EVAL             67      // unable to install eval information

#define MAC_CACHED_FILE         68      // used to flag cached Mac files to user

#define MON_SAM_NOACTION        69

#define NDS_ERR_INSTALL         70      // NDS installation error
#define NDS_INSTALL_SUCCESS     71      // NDS installation successful
#define NDS_LOGOUT              72     
#define NDS_LOGIN               73     

#define FILE_HOOK_CANT_INSTALL  74
#define FILE_HOOK_TRY_PROPRIETARY  75

#define COM_NO_MEMORY_SCAN_RING3    76

#define COM_NLM_SERVER_DOWN_UNLOAD  77

#define NDS_RELOGIN             78

#define COM_MAX_STR             79      // always one more than last id

#endif  // _COMSTR_H
