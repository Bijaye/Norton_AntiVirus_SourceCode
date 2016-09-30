
#ifndef SWITCHES_H
#define SWITCHES_H

const TCHAR SWITCH_ALL_DRIVES[]				  = _T("/A");
const TCHAR SWITCH_LOCAL_DRIVES[]			  = _T("/L");
const TCHAR SWITCH_ENABLE_SUBFOLDERS[]		  = _T("/S+");
const TCHAR SWITCH_DISABLE_SUBFOLDERS[]		  = _T("/S-");
const TCHAR SWITCH_ENABLE_MEMORY[]			  = _T("/M+");
const TCHAR SWITCH_DISABLE_MEMORY[]			  = _T("/M-");
const TCHAR SWITCH_MEMORY_ONLY[]			  = _T("/MEM");
const TCHAR SWITCH_ENABLE_BOOTREC[]			  = _T("/B+");
const TCHAR SWITCH_DISABLE_BOOTREC[]		  = _T("/B-");
const TCHAR SWITCH_BOOTREC_ONLY[]		  	  = _T("/BOOT");
const TCHAR SWITCH_NORESULTS[]			  	  = _T("/NORESULTS");
const TCHAR SWITCH_HEURISTICS[]				  = _T("/HEUR:");
const TCHAR SWITCH_AUTOMATION[]				  = _T("/AUTOMATION");
const TCHAR SWITCH_OFFICEAV[]				  = _T("/OFFICEAV");
const TCHAR SWITCH_SILENT_ERASER_SCAN[]		  = _T("/SESCAN");
const TCHAR SWITCH_ENABLE_ERASER_SCAN[]		  = _T("/SE+");
const TCHAR SWITCH_DISABLE_ERASER_SCAN[]	  = _T("/SE-");
const TCHAR SWITCH_QUICK_SCAN[]	              = _T("/QUICK");
const TCHAR SWITCH_SILENT[]	                  = _T("/SILENT");
const TCHAR SWITCH_ENABLE_NONVIRAL[]          = _T("/NV+");
const TCHAR SWITCH_REMEDIATE_AFTER_REBOOT[]   = _T("/REMEDIATE");
const TCHAR SWITCH_DISABLE_RELEVANCY[]        = _T("/SR-");

  // Use a task file as input to the list of items to scan
  // (task file path should follow the ':')
const TCHAR SWITCH_TASKFILE[]				= _T("/TASK:");
  // Same as the above only the task file is assumed to
  // be a temporary file to be deleted at the end of processing
const TCHAR SWITCH_TTASKFILE[]				= _T("/TTASK:");
const TCHAR SWITCH_HELP[]					= _T("/?");

#endif

