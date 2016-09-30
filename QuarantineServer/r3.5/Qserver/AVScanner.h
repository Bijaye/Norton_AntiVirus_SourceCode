/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// AVScanner.h: interface for the CAVScanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVSCANNER_H__0E68FB20_D72A_11D2_ADDE_00104BF3F36A__INCLUDED_)
#define AFX_AVSCANNER_H__0E68FB20_D72A_11D2_ADDE_00104BF3F36A__INCLUDED_

//#include "navapi.h"
#include "defutils.h"
#include "navapicallbacks.h"
#include <ecomtype.h>
#include <eicore.h>
#include <eilegtyp.h>
#include <eitclist.h>
#include <eilegacy.h>
#include <ecomlodr.h>
#include <thretcat.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAVScanner  
{
public:
    CAVScanner();
    virtual ~CAVScanner();

    HRESULT Initialize();
    HRESULT ScanFile( LPCTSTR pszFileName, LPVOID* pVirus );
    HRESULT DeleteFile( LPCTSTR pszFileName );
    HRESULT RepairFile( LPCTSTR pszFileName );

    HRESULT GetVirusName( LPVOID pVirus, LPTSTR szVirusName, int nBufferLen );
    HRESULT GetVirusID( LPVOID pVirus, DWORD& dwVirusID );
    HRESULT FreeVirus( LPVOID pVirus );

    HRESULT LoadVirusDef( DWORD dwVirusID, LPVOID* pVirusDef );
    DWORD GetVirusDefInfo( LPVOID pVirusDef, 
                             LEGVIRINFO enumVirInfo,
                             LPSTR       lpszResultBuffer,
                             LPDWORD      lpdwBufferSize );
    HRESULT FreeVirusDef( LPVOID pVirusDef );
    LPCTSTR GetVirusDefsDir() { return m_sVirusDefs; }

    HRESULT GetVirusDefsDate( SYSTEMTIME* pst,DWORD *dwRev );

	HRESULT GetVirusDBSequenceNumber( DWORD *pdwSequence);  // added by tm 6-26-01
	HRESULT GetVirusDBVersionString( VARIANT *pV);  // added by tm 6-26-01
	HRESULT GetVirusDefVersionNumber( VARIANT *pV);  // added by tm 6-26-01
	HRESULT LoadVirusTable( void );  // added by tm 6-27-01
	HRESULT GetVirusInfobyTableIndex( DWORD dwVIndex, VARIANT *pVariant );
	DWORD	GetNoDefInTable(void){return m_dwNoVDefInTable;}
    operator HLEGENGINE() {return m_hEngine; }

private:
	HRESULT UnloadECOM(void);
    // 
    // Handle to NAVAPI32
    // 
    static HLEGENGINE  m_hEngine;

    // 
    // Hawkings object.
    // 
//    static CDefUtils   defUtils;

    // 
    // Virus defs path.
    // 
    static CString       m_sVirusDefs;


    // 
    // Memory based callbacks
    // 
    CNavAPIMemCallbacks m_MemCallbacks;


    // 
    // Memory scan flag.
    // 
    BOOL        m_bMemoryScan;

    
	// added by tm 6-26-01
	// 
    // status flag for retriving virus db info.
    // 
    static BOOL        m_bVirusDBInfoLoaded;

	// storage for def db information.
	static LEGVIRUSDBINFOEX m_NavVirusDBInfo;

	static HLEGVIRUSDEFTABLE m_hNavVDefTable;
	
	static DWORD			m_dwNoVDefInTable;

	HRESULT GetVirusDBInfo( void);

	static int				m_iNoInstances;
	
	void BuildInfectsString(DWORD  dwVirusDefIndex, CString& sBuffer );
	static TCHAR m_szPathToDefs [MAX_PATH];
	static TCHAR m_szDef[MAX_PATH];

	// additions for ECOM support, tmarles, 11/08/2004
	static	EILegacy*		m_LegacyIntfPtr;		// pointer to EILegacy interface
	static	EITCList*		m_ThreatListIntfPtr;	// pointer to EITCList interface
	static	EICore*			m_CoreIntfPtr;		// pointer to Core interface
	static	EIFoundation*	m_FoundationPtr;		// pointer to foundation
	static    DWORD		m_dwDataSets;				// data sets to use (passed to EngineInit)


};

#endif // !defined(AFX_AVSCANNER_H__0E68FB20_D72A_11D2_ADDE_00104BF3F36A__INCLUDED_)
