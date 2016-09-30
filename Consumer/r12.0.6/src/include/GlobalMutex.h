// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

#ifndef GLOBAL_MUTEX_H
#define GLOBAL_MUTEX_H

// The following mutex is created by NavProxy when Email Protection is enabled
// and running. On Windows XP, the mutex is local to a Terminal Service session.

static const TCHAR SYM_NAVPROXY_MUTEX[] = _T("SYM_NAVPROXY_MUTEX");

// The following mutex is created by instances of NAVW32.exe.
// An application can detect if one or more copies of NAVW32.exe is running by
// trying to open this mutex. The mutex is local to a Terminal Service session.

static const TCHAR SYM_NAVW32_MUTEX[] = _T("SYM_NAVW32_MUTEX");

// The following mutex is created by Def Alert when it is loaded.
// On Windows XP, the mutex is local to a Terminal Service session.

static const TCHAR SYM_DEF_ALERT_MUTEX[] = _T("SYM_DEF_ALERT_MUTEX");

// Mutex for NAVOptRF (NAV Options Refresh) licensing object.

static const char NAVOPTRF_MUTEX_SESSIONWIDE[] = "NAVOPTRF_MUTEX_SESSIONWIDE";

// Mutexes for StatusHP
//
static const char NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_BEGIN [] =  "NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_BEGIN";
static const char NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_RUNNING [] = "NAV_STATUS_HELPER_MUTEX_SESSIONWIDE_RUNNING";
static const char NAV_STATUS_HELPER_MUTEX_MACHINEWIDE_NSC_CLEAN [] = "NAV_STATUS_HELPER_MUTEX_MACHINEWIDE_NSC_CLEAN";

#endif

