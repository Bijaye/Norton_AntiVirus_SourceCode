// QScanner.h: interface for the CScanner class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning (disable: 4786)

#include <time.h>
#include <string>
#include <vector>
#include "navscan.h"

using namespace std;


class CTScanner : public IScanSink, IScanProperties, IScanItems, ISymBaseImpl< CSymRefCount >  
{
public:
	CTScanner();
	virtual ~CTScanner();

	HRESULT Initialize(char* szAppID);

	// Scan routines.
	SCANSTATUS RepairInfection(IScanInfection* pInfection);
	SCANSTATUS Scan();
    void ProccessCmdLine();
    void ProcessSwitch(const char* szSwitch);

	//
	// NavScan interfaces.
	//
	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY( IID_ScanSink, IScanSink )
		SYM_INTERFACE_ENTRY( IID_ScanProperties, IScanProperties )
		SYM_INTERFACE_ENTRY( IID_ScanItems, IScanItems )
	SYM_INTERFACE_MAP_END()

	//
	// IScanSink methods.
	//
    virtual SCANSTATUS OnBusy();
	virtual SCANSTATUS OnError( IScanError* pError );
	virtual SCANSTATUS OnNewItem( const char * pszItem );
    virtual SCANSTATUS OnNewFile( const char * pszLongName, const char * pszShortName );
    virtual SCANSTATUS OnNewDirectory( const char * pszLongName, const char * pszShortName );
    virtual SCANSTATUS OnInfectionFound( IScanInfection* pInfection );
	virtual SCANSTATUS OnRepairFailed( IScanInfection* pInfection );
    virtual SCANSTATUS OnRemoveMimeComponent( IScanInfection* pInfection, char* pszMessage, 
											  int iMessageBufferSize );
	virtual SCANSTATUS LockVolume( const char* szVolume, bool bLock );
    virtual SCANSTATUS OnCleanFile( const char * pszFileName,
									const char * pszTempFileName );

	//
	// IScanProperties methods.
	//
	virtual int GetProperty( const char* szProperty, int iDefault );
	virtual const char * GetProperty( const char* szProperty, const char * szDefault );

	// 
	// IScanItems methods.
	virtual int GetScanItemCount();
	virtual const char * GetScanItemPath( int iIndex );

private:
	// Disallowed
	CTScanner( CTScanner& other );

	// Handle to CCSCAN.DLL
	HMODULE					m_hNavScan;

	// Pointer to real scanner object.	
	IScanner*				m_pScanner;

	// Items to scan.
	vector<string> m_ItemsToScan;

    // Use decomposers?
    bool m_bDecomposers;

    // Use Threat Cat
    bool m_bThreatCat;

    // Scan Memory
    bool m_bScanMem;

    // Scan Boot Records
    bool m_bBootRec;

};