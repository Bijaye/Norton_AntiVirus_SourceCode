////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CompressedInfection.h: interface for the CCompressedInfection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(CCOMPRESSEDINFECTION_H)
#define CCOMPRESSEDINFECTION_H

#pragma warning (disable: 4786)

class CCompressedInfection : 
    public IScanCompressedFileInfection,
	public ISymBaseImpl< CSymThreadSafeRefCount >    
{
public:
	CCompressedInfection(int iFileNum, int iCompressionDepth);
	virtual ~CCompressedInfection();

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY( IID_ScanInfection, IScanInfection )
		SYM_INTERFACE_ENTRY( IID_ScanCompressedFileInfection, IScanCompressedFileInfection )
	SYM_INTERFACE_MAP_END()
		
	//
	// IScanInfection methods.
	//
	virtual const char* GetVirusName();
    virtual unsigned long GetVirusID();
	virtual int GetStatus();
	virtual void SetUserData( long lData );
	virtual long GetUserData();

    virtual unsigned long GetCategoryCount();
	virtual const unsigned long* GetCategories();
	virtual bool IsCategoryType( unsigned long ulCategory );

	//
	// IScanCompressedFileInfection methods.
	//
	virtual int GetComponentCount();
    virtual const char* GetComponent( int iComponent ) ;
	virtual const char * GetTempFileName();
	virtual unsigned long GetDecCharSet();

    virtual unsigned long GetCanDeleteAncestorDepth();
	virtual unsigned long GetComponentCharSet( int iComponent );
    virtual bool IsPackedExecutable();

	// File name
	string		m_sFileName;
	
	// Levels of compression
	int m_iCompressionDepth;

	// Vector of COmponent Names at each level
	vector<string>	m_vComponents;

	// User data
	long		m_lUserData;

    // Number of threat categories
    long        m_lNumCategories;

    // Categories
    unsigned long*       m_plCategories;
	
	// Status
	int m_iStatus;
};

#endif 
