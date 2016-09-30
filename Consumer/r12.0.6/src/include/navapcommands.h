//////////////////////////////////////////////////////////////////////
//
// navapcommands.h - This file contains the constants that define 
// the commands that can be sent to the NAVAP service.  On Win9x systems,
// this functionality is implemented in the NAVAPW32 agent module.
//

#if !defined( _NAVAPCOMMANDS_H_ )
#define _NAVAPCOMMANDS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Enable/Disable commands.
const WCHAR NAVAPCMD_ENABLE[]				= L"9416FB5B-436B-46b3-AE45-DA6787F79E2A";
const WCHAR NAVAPCMD_DISABLE[]				= L"9416FB5C-436B-46b3-AE45-DA6787F79E2A";

// This command completly unloads all of the drivers and settings
// and restarts AP.  Use this command if you want the AP driver to
// reload it's options.
const WCHAR NAVAPCMD_RESTART[]				= L"9416FB5D-436B-46b3-AE45-DA6787F79E2A";

// This command forces the NAVAP service to reload it's options.  Cheaper than
// NAVAPCMD_RESTART, but not as effective.
const WCHAR NAVAPCMD_RELOADSETTINGS[]		= L"9416FB5E-436B-46b3-AE45-DA6787F79E2A";

// This command does not return until the service has fully started.
// This is a cheap and easy way for the Agent to verify that the service
// is ready to accept commands.  It will wait for a maximum of 60 seconds.
// If a timeout occurs E_FAIL is returned.
const WCHAR NAVAPCMD_WAITFORSTARTUP[]		= L"9416FB5F-436B-46b3-AE45-DA6787F79E2A";

// This command is issued for canceling a asyncronous AP scan. The
// SAVRT32_ROUS64 cookie must be passed in the VARIANT data.
//
const WCHAR NAVAPCMD_CANCELSCAN[]           = L"2AF5126D-8B47-4784-BD02-3B33E5679D6D";

// This command is issued when the manual scan makes a non-viral threat detection so AP
// will not alert for the item a second time. It is also sent after a scan completes
// so AP knows to start alerting on those items again if new events come in.
//
const WCHAR NAVAPCMD_MANUALSCANSPYWAREDETECTION[] = L"2AF5126D-8B47-4784-BD02-3B33E5679666";

// In order to keep clients from starting the services via COM calls,
// clients should call OpenMutex() on this mutex to detect the presence of 
// the service.  If the service is not running, no COM calls should be made.
// Note that on Win2k and greater systems, you must prepend the string "Global\"
// to ensure that you are using the global mutex.
const TCHAR NAVAPSVC_RUNNING_MUTEX[]				= _T("NAVAPSVC_MUTEX");

#endif