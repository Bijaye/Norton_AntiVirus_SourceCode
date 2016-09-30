// Scanner.h: interface for the CScanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANNER_H__D781DCE2_E673_4DF9_803B_40408D00A066__INCLUDED_)
#define AFX_SCANNER_H__D781DCE2_E673_4DF9_803B_40408D00A066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccScanInterface.h"
#include "ccScanLoader.h"
#include <string>

class CScanner : public IScanSink, IScanProperties, IScanItems, ISymBaseImpl< CSymRefCount >  
{
public:
	CScanner();
	virtual ~CScanner();

	// Using szTestDefsPath turns off def authentication.
    //
    SCANSTATUS Initialize( const char* szAppID, int iBloodhoundLevel,
                           bool bAttemptRepair, const char* szTestDefs = NULL);
    bool GetDefsDate ( WORD& wYear, WORD& wMonth, WORD& wDay, unsigned long& ulRevision );
    bool GetDefinitionsDirectory ( char*& szDefsDir );

	// Scan routines.
	void ScanBootRecord( char dl, IScanResults** ppResults );
	void ScanMasterBootRecord( BYTE byID, IScanResults** ppResults );
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
    virtual SCANSTATUS OnCleanFile(const char* pszFileName,
                                   const char* pszTempFileName);

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
	CScanner( CScanner& other );

	// scanner loader
	cc::ccScan_IScanner m_ccScanLoader;

	// Pointer to real scanner object.	
    IScanner* m_pTheScanner;

	// Drive to scan (x:\)
    std::string					m_sDrive;

	// MBR scanning flag
	bool					m_bScanMBR;

    bool                    m_bAttemptRepair;
};

#endif // !defined(AFX_SCANNER_H__D781DCE2_E673_4DF9_803B_40408D00A066__INCLUDED_)
