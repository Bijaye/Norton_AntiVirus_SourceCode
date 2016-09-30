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
#pragma once

#include <string>

#include "ccScanLoader.h"

#define AV_HURISTIC_LEVEL	3		//Enable Heuristic file scanning

class CTestScanner
{
public:
	CTestScanner() throw();
	virtual ~CTestScanner() throw();

public:
	// These methods all will return a ref counted object back to the caller
	// Note, the handle to the ccScan DLL is held in this class
	// The scanner object must be destroyed from before this calss is destroyed
	// class is destryed
	bool GetScanner(IScanner*& pScanner) throw();
	bool GetScanSink(IScanSink*& pSink) throw();
	bool GetScanProperties(IScanProperties*& pScanProps) throw();

	BOOL m_bDecompose;

	// Contained classes
private:
	class CSink : public IScanSink,
		public ISymBaseImpl< CSymThreadSafeRefCount >   
	{
	public:
		CSink() throw();
		virtual ~CSink() throw();

	private:
		// Disallowed
		CSink(const CSink&) throw();
		CSink& operator=(const CSink&) throw();

	public:
		//
		// NavScan interfaces.
		//
		SYM_INTERFACE_MAP_BEGIN()
			SYM_INTERFACE_ENTRY( IID_ScanSink, IScanSink )
		SYM_INTERFACE_MAP_END()

		//
		// IScanSink methods.
		//
		virtual SCANSTATUS OnBusy() throw();
		virtual SCANSTATUS OnError( IScanError* pError ) throw();
		virtual SCANSTATUS OnNewItem( const char * pszItem ) throw();
		virtual SCANSTATUS OnNewFile( const char * pszLongName, const char * pszShortName ) throw();
		virtual SCANSTATUS OnNewDirectory( const char * pszLongName, const char * pszShortName ) throw();
		virtual SCANSTATUS OnInfectionFound( IScanInfection* pInfection ) throw();
		virtual SCANSTATUS OnRepairFailed( IScanInfection* pInfection ) throw();
		virtual SCANSTATUS OnRemoveMimeComponent( IScanInfection* pInfection, char* pszMessage, 
			int iMessageBufferSize ) throw();
		virtual SCANSTATUS LockVolume( const char* szVolume, bool bLock ) throw();
		virtual SCANSTATUS OnCleanFile( const char * pszFileName,
			const char * pszTempFileName ) throw();

	};

	class CScanProperties : public IScanProperties, 
		public ISymBaseImpl< CSymThreadSafeRefCount >   
	{
	public:
		CScanProperties(BOOL bDecompose) throw();
		virtual ~CScanProperties() throw();

	private:
		// Disallowed
		CScanProperties(const CScanProperties&) throw();
		CScanProperties& operator=(const CScanProperties&) throw();

		BOOL m_bDecompose;

	public:
		//
		// NavScan interfaces.
		//
		SYM_INTERFACE_MAP_BEGIN()
			SYM_INTERFACE_ENTRY( IID_ScanProperties, IScanProperties )
		SYM_INTERFACE_MAP_END()

		//
		// IScanProperties methods.
		//
		virtual int GetProperty( const char* szProperty, int iDefault ) throw();
		virtual const char * GetProperty( const char* szProperty, const char * szDefault ) throw();
	};

	class CScanner : public IScanner, 
		public ISymBaseImpl< CSymThreadSafeRefCount >   
	{
	public:
		CScanner();
		virtual ~CScanner() throw();

	private:
		// Disallowed
		CScanner(const CScanner &) throw();
		CScanner& operator=(const CScanner&) throw();

	protected:
		cc::ccScan_IScanner m_ccScanLoader;
		IScannerPtr m_pScanner;
		DWORD m_dwTotalScanTick;

	public:
		//
		// NavScan interfaces.
		//
		SYM_INTERFACE_MAP_BEGIN()
			SYM_INTERFACE_ENTRY( IID_Scanner, IScanner )
		SYM_INTERFACE_MAP_END()

		//
		// IScanner methods.
		//
		virtual SCANSTATUS Initialize( const char* pszProductID,
			const char* pszNavexInfFile,
			const char* pszTempDirectory,
			int iBloodhoundLevel,
			long lFlags );

		virtual SCANSTATUS SetBloodhoundLevel( int iBloodhoundLevel );
		virtual SCANSTATUS SetMaxDecomposerExtractSize( unsigned long ulMaxSize );
		virtual SCANSTATUS SetDecomposerMemoryFSSize( unsigned long ulSize );

		//
		// Scan operations.
		//
		virtual SCANSTATUS Scan( IScanItems* pScanItems,
			IScanProperties* pProps,
			IScanSink* pSink,
			IScanResults** ppResults );

		//
		// Repair operations.
		//
		virtual SCANSTATUS RepairInfection( IScanInfection* pInfection, IScanResults** ppResults = NULL );
		virtual SCANSTATUS DeleteInfection( IScanInfection* pInfection, bool bDeepDelete = false );

		//
		// Infection helpers.
		//
		virtual SCANSTATUS ExtractInfectedFile( IScanCompressedFileInfection* pInfection,
			char *pszExtractedFile,
			int iSize );
		virtual SCANSTATUS DeleteInfectedFile( const char * szFileName );

		//
		// Virus information operations.
		//
		virtual SCANSTATUS GetVirusInfo( unsigned long ulVirusID, IScanVirusInfo** ppInfo );

		//
		// Virus definition management operations.
		//

		//
		// ReloadDefinitions() should only be called if all external threads are paused (or 
		// there is only one external thread).
		//
		virtual SCANSTATUS ReloadDefinitions();
		virtual SCANSTATUS NewDefinitionsAvailaible();
		virtual SCANSTATUS GetDefinitionsInfo( IScanDefinitionsInfo** pDefInfo );
		virtual SCANSTATUS PreDefinitionsUpdate( char* pszNewDefsDirectory, int iSize );
		virtual SCANSTATUS PostDefinitionsUpdate();
		virtual SCANSTATUS IsAuthenticDefs();
	};

};
