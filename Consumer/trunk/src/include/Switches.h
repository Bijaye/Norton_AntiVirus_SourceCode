////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SWITCHES_H
#define SWITCHES_H

// Published switches
const TCHAR SWITCH_ALL_DRIVES[]				  = _T("/A");
const TCHAR SWITCH_LOCAL_DRIVES[]			  = _T("/L");
const TCHAR SWITCH_ENABLE_SUBFOLDERS[]		  = _T("/S+");
const TCHAR SWITCH_DISABLE_SUBFOLDERS[]		  = _T("/S-");
const TCHAR SWITCH_ENABLE_BOOTREC[]			  = _T("/B+");
const TCHAR SWITCH_DISABLE_BOOTREC[]		  = _T("/B-");
const TCHAR SWITCH_BOOTREC_ONLY[]		  	  = _T("/BOOT");
const TCHAR SWITCH_SILENT_ERASER_SCAN[]		  = _T("/SESCAN");
const TCHAR SWITCH_ENABLE_ERASER_SCAN[]		  = _T("/SE+");
const TCHAR SWITCH_DISABLE_ERASER_SCAN[]	  = _T("/SE-");
const TCHAR SWITCH_QUICK_SCAN[]	              = _T("/QUICK");

// Internal/Non-published switches
const TCHAR SWITCH_AUTOMATION[]				  = _T("/AUTOMATION");
const TCHAR SWITCH_OFFICEAV[]				  = _T("/OFFICEAV");
const TCHAR SWITCH_IMSCAN[]			    	  = _T("/IMSCAN");
const TCHAR SWITCH_THREADS[]                  = _T("/THREADS:");     // - /THREADS:NNNN      (def=0)
const TCHAR SWITCH_PROGINT[]                  = _T("/PROGINT:");     // - /PROGINT:NNNN      (in mS, def=1000)
const TCHAR SWITCH_DECDEPTH[]                 = _T("/DECDEPTH:");    // - /DECDEPTH:NNNN     (def=10)
const TCHAR SWITCH_NOAUTOREMOVE[]             = _T("/NOAUTOREMOVE"); // Default settings comes from NAVOpts
const TCHAR SWITCH_COH[]                      = _T("/COH");          // Force Eraser scan with Confidence Online Heavy Engine
const TCHAR SWITCH_COHONLY[]                  = _T("/COHONLY");      // Disabled all eraser detections, except COH
const TCHAR SWITCH_ENABLE_STEALTH_SCAN[]      = _T("/ST+");          // Force stealth file scanning on
const TCHAR SWITCH_DISABLE_STEALTH_SCAN[]     = _T("/ST-");          // Force stealth file scanning off
const TCHAR SWITCH_DISABLE_COOKIE_SCAN[]      = _T("/CK-");          // Force cookie scanning off


  // Use a task file as input to the list of items to scan
  // (task file path should follow the ':')
const TCHAR SWITCH_TASKFILE[]				= _T("/TASK:");
  // Same as the above only the task file is assumed to
  // be a temporary file to be deleted at the end of processing
const TCHAR SWITCH_TTASKFILE[]				= _T("/TTASK:");
const TCHAR SWITCH_HELP[]					= _T("/?");

#endif

