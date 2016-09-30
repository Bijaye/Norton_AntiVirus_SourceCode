// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
//
// QSREGVAL.H - contains registry keys and valus for Quarantine server 
//              configuration values.
//
///////////////////////////////////////////////////////////////////////////////
// 12/19/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////

#ifndef _QSREGVAL_H_
#define	_QSREGVAL_H_

#pragma once

const TCHAR REGKEY_QSERVER[] = _T( "Software\\Symantec\\Quarantine\\Server" );
const TCHAR REGKEY_AVISSERVER[] = _T("Software\\Symantec\\Quarantine\\Server\\Avis\\current");

const TCHAR REGVALUE_QUARANTINE_FOLDER[] = _T( "QuarantineFolder" );
const TCHAR REGVALUE_PORT[]       = _T( "IP Socket" );
const TCHAR REGVALUE_SPXSOCKET[]  = _T( "SPX Socket" );
const TCHAR REGVALUE_LISTEN_IP[]  = _T( "Listen IP" );
const TCHAR REGVALUE_LISTEN_SPX[] = _T( "Listen SPX" );
const TCHAR REGVALUE_MAX_SIZE[]   = _T( "MaxSize" );
const TCHAR REGVALUE_MAX_FILES[]  = _T( "MaxFiles" );
const TCHAR REGVALUE_PURGE[]  = _T( "PurgeSamples" );//TM 5/8/00
const TCHAR REGVALUE_ATTENTION[]  = _T("attention"); 
const TCHAR REGVALUE_ALERTINTERVAL[]  = _T("QsAlertInterval"); 
//tm 1/14/99
const TCHAR REGVALUE_VERSION[]  = _T("Version"); 
// jhill 3/3/00
const TCHAR REGVALUE_QFOLDER_WARNING_SIZE[] = _T( "QFolderWarningSize" );
const TCHAR REGVALUE_QFOLDER_FREE_SPACE[]   = _T( "QFreeSpace" );
//tm 3/14/00
const TCHAR CONFIG_I_TIMEOUT[]  = _T("Config Interface Timeout"); 

//tm 4/17/00
const TCHAR WINSOCK_TRACE[]  = _T("WinSock Trace"); 

//tm 5/19/00
const TCHAR QSERVERITEM_TRACE[]  = _T("QSItem Interface Trace"); 

//tm 6-20-01
const TCHAR REGVALUE_MOVE_SAMPLE[]  = _T("Move QSamples"); 

//tm 10-15-01
const TCHAR REGVALUE_SESA_AGENT_PORT[]  = _T("SESA Agent Listen Port"); 

#define MOVE_SAMPLES_ON_QFOLDER_CHANGE		1
#define DELETE_SAMPLES_ON_QFOLDER_CHANGE	0

//tm 11-8-01 (used only to signal a test of all events
const TCHAR REGVALUE_TEST_EVENTS[]  = _T("TestEvents"); 
#define QS_TEST_GENERAL_EVENTS				1
#define QS_TEST_SAMPLE_EVENTS				2

//tm 11-11-03 (needed to control generation of SESA events)
const TCHAR REGVALUE_GENERATE_SESA_EVENTS[]		  = _T("GenerateSESAEvents"); 

// AVIS ALERTING
const TCHAR REGVALUE_ALERTING_ENABLED[]           = _T( "QsAlertingEnabled" );
const TCHAR REGVALUE_AMS_SERVER_ADDRESS[]         = _T( "QsAmsServerAddress" );
const TCHAR REGVALUE_ALERT_DISKQUOTA_LOW_WATER[]  = _T( "QsAlertDiskQuotaLowWater" );
const TCHAR REGVALUE_ALERT_DISKSPACE_LOW_WATER[]  = _T( "QsAlertDiskspaceLowWater" );

// CLIENT QUARANTINE FORWARDING 4/28/00 JHILL
const TCHAR REGKEY_LANDESK_QFORWARDING[]           = _T( "SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\Quarantine" );
const TCHAR REGVALUE_LANDESK_FORWARDING_ENABLED[]  = _T( "ForwardingEnabled" );
const TCHAR REGVALUE_LANDESK_FORWARDING_PORT[]     = _T( "ForwardingPort" );
const TCHAR REGVALUE_LANDESK_FORWARDING_PROTOCOL[] = _T( "ForwardingProtocol" );
#define  REGVALUE_LANDESK_FORWARDING_PROTOCOL_IP  0
#define  REGVALUE_LANDESK_FORWARDING_PROTOCOL_IPX 1



#endif