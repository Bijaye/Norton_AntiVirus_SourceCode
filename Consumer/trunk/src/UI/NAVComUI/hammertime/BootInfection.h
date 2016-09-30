////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// BootInfection.h: interface for the CBootInfection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOOTINFECTION_H__150BF2CD_CD37_4038_98C7_1E2180C031D3__INCLUDED_)
#define AFX_BOOTINFECTION_H__150BF2CD_CD37_4038_98C7_1E2180C031D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBootInfection : 
	public ISymBaseImpl< CSymThreadSafeRefCount >, 
	public IScanBootInfection  
{
public:
	CBootInfection(char letter);
	virtual ~CBootInfection();


	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY( IID_ScanInfection, IScanInfection )
		SYM_INTERFACE_ENTRY( IID_ScanBootInfection, IScanBootInfection )
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
	// IScanBootInfection methods.
	//
	virtual char GetDriveLetter();

	// File name
	char		m_cLetter;

	// User data
	long		m_lUserData;

    // Number of threat categories
    long        m_lNumCategories;

    // Categories
    unsigned long*       m_plCategories;
	
	// Status
	int m_iStatus;

};

#endif // !defined(AFX_BOOTINFECTION_H__150BF2CD_CD37_4038_98C7_1E2180C031D3__INCLUDED_)
