// QScanner.h: interface for the CScanner class.
//
//////////////////////////////////////////////////////////////////////

#ifndef QSCANNER_H
#define QSCANNER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning (disable: 4786)

#include <time.h>
#include <string>
#include <vector>
#include "ccScanInterface.h"
#include "ccScanLoader.h"

class CQScanner : public IScanSink, IScanProperties, IScanItems, ISymBaseImpl< CSymRefCount >  
{
public:
	CQScanner();
	virtual ~CQScanner();

	HRESULT Initialize(char* szAppID);
    HRESULT InitializeWithExternalScanner(IScanner* pScanner);

	// Scan routines.
	SCANSTATUS RepairInfection(IScanInfection* pInfection);
	SCANSTATUS Scan(char* szFileName, IScanInfection** ppInfection);

    // Helper function
    HRESULT GetCurrentDefinitionsDate( SYSTEMTIME *lptTime );
	HRESULT GetCurrentDefinitionsDirectory(LPTSTR szDefinitionsDirectory);
    HRESULT GetVirusInformation( unsigned long ulVirusID, IScanVirusInfo** ppInfo );

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
	CQScanner( CQScanner& other );

	// scanner loader
	cc::ccScan_IScanner m_ccScanLoader;

	// Pointer to real scanner object.	
	IScanner*				m_pScanner;

	// Items to scan.
	::std::vector<::std::string> m_ItemsToScan;
};

#endif 