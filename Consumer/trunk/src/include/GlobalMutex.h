////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GLOBAL_MUTEX_H
#define GLOBAL_MUTEX_H

// The following mutex is created by NavProxy when Email Protection is enabled
// and running. On Windows XP, the mutex is local to a Terminal Service session.

static const WCHAR SYM_NAVPROXY_MUTEX[] = L"SYM_NAVPROXY_MUTEX";

// The following mutex is created by instances of NAVW32.exe.
// An application can detect if one or more copies of NAVW32.exe is running by
// trying to open this mutex. The mutex is local to a Terminal Service session.

static const WCHAR SYM_NAVW32_MUTEX[] = L"SYM_NAVW32_MUTEX";

// The following mutex is created by Def Alert when it is loaded.
// On Windows XP, the mutex is local to a Terminal Service session.

static const WCHAR SYM_DEF_ALERT_MUTEX[] = L"SYM_DEF_ALERT_MUTEX";

// Mutexes for StatusHP
//
static const WCHAR NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_BEGIN [] =  L"NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_BEGIN";
static const WCHAR NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_RUNNING [] = L"NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_RUNNING";
static const WCHAR NAV_STATUS_HELPER_MUTEX_MACHINEWIDE_NSC_CLEAN [] = L"NAV_STATUS_HELPER_MUTEX_MACHINEWIDE_NSC_CLEAN";

#endif

