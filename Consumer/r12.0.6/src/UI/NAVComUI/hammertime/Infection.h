// Infection.h: interface for the CInfection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFECTION_H__DA66F0D9_B7AE_46E5_9F64_576508186DFA__INCLUDED_)
#define AFX_INFECTION_H__DA66F0D9_B7AE_46E5_9F64_576508186DFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInfection : 
    public IScanFileInfection2,
	public ISymBaseImpl< CSymThreadSafeRefCount >    
{
public:
	CInfection( int iFileNum, unsigned long* plCats = NULL, long lCatCount = 0, bool bCanDelete = true, bool bCanSERepair = true);
	virtual ~CInfection();

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY( IID_ScanInfection, IScanInfection )
        SYM_INTERFACE_ENTRY( IID_ScanFileInfection, IScanFileInfection )
		SYM_INTERFACE_ENTRY( IID_ScanFileInfection2, IScanFileInfection2 )
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
	// IScanFileInfection methods.
	//
	virtual const char * GetLongFileName();
	virtual const char * GetShortFileName();
    virtual bool CanDelete();

    //
	// IScanFileInfection2 methods.
	//
    virtual bool CanRepair();
	virtual bool CanSideEffectRepair();

	// File name
	string		m_sFileName;
	
	// User data
	long		m_lUserData;

    // Number of threat categories
    long        m_lNumCategories;

    // Categories
    unsigned long*       m_plCategories;

    // Can Delete?
    bool        m_bCanDelete;

    // Can SideEffectRepair
    bool        m_bCanSERepair;
	
	// Status
	int         m_iStatus;

};

class CSideEffectInfection : 
    public ISideEffectFileInfection,
    public CInfection
{
public:
    CSideEffectInfection( int iFileNum, unsigned long* plCats = NULL, long lCatCount = 0, bool bCanDelete = true, bool bCanSERepair = true, int nSECount = 0);
	virtual ~CSideEffectInfection();

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( IID_SideEffectFileInfection, ISideEffectFileInfection )
        SYM_INTERFACE_ENTRY( IID_ScanInfection, CInfection )
        SYM_INTERFACE_ENTRY( IID_ScanFileInfection, CInfection )
        SYM_INTERFACE_ENTRY( IID_ScanFileInfection2, CInfection )
	SYM_INTERFACE_MAP_END()

    // ISideEffectFileInfection methods
    virtual int GetSideEffectsCount();
    virtual SCANSTATUS GetSideEffect(int iSideEffect, ISideEffect** ppSideEffect);

    // Side effects count
    int m_nSECount;
};

#endif // !defined(AFX_INFECTION_H__DA66F0D9_B7AE_46E5_9F64_576508186DFA__INCLUDED_)
