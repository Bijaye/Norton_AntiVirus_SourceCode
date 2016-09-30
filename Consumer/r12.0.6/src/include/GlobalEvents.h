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
//
// This file contains names of global events sent by Norton AntiVirus
//

#ifndef __GLOBALEVENTS_H
#define __GLOBALEVENTS_H

// Fired when AutoProtect settings have changed
//
static const TCHAR SYM_REFRESH_AP_STATUS_EVENT[] = _T("SYM_REFRESH_AP_STATUS_EVENT");

// Fired when options have changed

static const TCHAR SYM_OPTIONS_CHANGED_EVENT[] = _T("NAV_OPTIONS_CHANGED_EVENT");

// Event for def status. This is sent by the NAVLUCBK.DLL after processing new defs
//
static const TCHAR SYM_REFRESH_VIRUS_DEF_STATUS_EVENT [] = _T("SYM_REFRESH_VIRUS_DEF_STATUS_EVENT");

// Event that Rescue will signal when it's made a new disk set 
// (see NURSCDLG.CPP).
//
static const TCHAR SYM_NEWRESCUEDISK [] = _T("SYM_NEWRESCUEDISK");

//
// Fired when NAVProxy status has changed
//

static const TCHAR SYM_REFRESH_NAVPROXY_STATUS_EVENT[] = _T("SYM_REFRESH_NAVPROXY_STATUS_EVENT");

//
// Fired when a successful "My Computer" scan has been completed by an Admin
//
static const TCHAR SYM_REFRESH_FULLSYSTEMSCAN_EVENT[] = _T("SYM_REFRESH_FULLSYSTEMSCAN_EVENT");

//
// Fired by NMain to ask DefAlert to check license status.
//
static const TCHAR SYM_CHECK_LICENSE_EVENT[] = _T( "SYM_CHECK_LICENSE_EVENT" );

//
// Fired by NAVLicense on License status change.
//
static const TCHAR SYM_REFRESH_NAV_LICENSE_STATUS_EVENT[] = _T("SYM_REFRESH_NAV_LICENSE_STATUS_EVENT");

//
// Fired by DefAlert on LiveSubscribe status change.
//
static const TCHAR SYM_REFRESH_SUBSCRIPTION_STATUS_EVENT[] = _T("SYM_REFRESH_SUBSCRIPTION_STATUS_EVENT");

//
// Fired by NAVOptions on IMScanner status change.
//
static const TCHAR SYM_REFRESH_IMSCANNER_STATUS_EVENT[] = _T("SYM_REFRESH_IMSCANNER_STATUS_EVENT");
#endif // __GLOBALEVENTS_H
