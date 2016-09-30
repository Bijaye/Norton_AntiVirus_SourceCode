////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// MBRInfection.h: interface for the CMBRInfection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MBRINFECTION_H__257C765A_2BBC_44A6_A6C9_357916C0457F__INCLUDED_)
#define AFX_MBRINFECTION_H__257C765A_2BBC_44A6_A6C9_357916C0457F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMBRInfection : 
	public ISymBaseImpl< CSymThreadSafeRefCount >, 
	public IScanMBRInfection  
{
public:
	CMBRInfection(char cNumber, long lOffset);
	virtual ~CMBRInfection();


	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY( IID_ScanInfection, IScanInfection )
		SYM_INTERFACE_ENTRY( IID_ScanMBRInfection, IScanMBRInfection )
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
	// IScanMBRInfection methods.

	virtual unsigned char GetDriveNumber();
	virtual unsigned long GetMBROffset();

	char m_cNumber;
	long m_lOffset;

	// User data
	long		m_lUserData;

    // Number of threat categories
    long        m_lNumCategories;

    // Categories
    unsigned long*       m_plCategories;
	
	// Status
	int m_iStatus;

};

#endif // !defined(AFX_MBRINFECTION_H__257C765A_2BBC_44A6_A6C9_357916C0457F__INCLUDED_)
