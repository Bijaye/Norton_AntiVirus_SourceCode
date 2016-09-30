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
const TCHAR REGVALUE_QUARANTINE_FOLDER[] = _T( "QuarantineFolder" );
const TCHAR REGVALUE_PORT[] = _T( "IP Socket" );
const TCHAR REGVALUE_SPXSOCKET[] = _T( "SPX Socket" );
const TCHAR REGVALUE_LISTEN_IP[] = _T( "Listen IP" );
const TCHAR REGVALUE_LISTEN_SPX[] = _T( "Listen SPX" );
const TCHAR REGVALUE_MAX_SIZE[] = _T( "MaxSize" );
const TCHAR REGVALUE_MAX_FILES[] = _T( "MaxFiles" );

#endif