// SymCorpUI-newDoc.h : interface of the CSymCorpUIDoc class
//

#pragma once
#define PROTECTIONPROVIDER_HELPERTYPES_WITHNAMESPACE
#import "ProtectionProvider.tlb" raw_interfaces_only exclude("wireHWND", "_RemotableHandle", "__MIDL_IWinTypes_0009")
#include "ProtectionProvider.h"
#include "util.h"


class CSymCorpUIDoc : public CDocument
{
protected: // create from serialization only
	CSymCorpUIDoc();
	DECLARE_DYNCREATE(CSymCorpUIDoc)

    // Loads the protection technology providers.  Frees any existing ones.
    HRESULT LoadProviders( void );
    // Unloads all protection providers.  
    HRESULT UnloadProviders( bool updateViews = true );

    // ** DATA MEMBERS **
	CLSIDList												protectionProvidersIDs;
	ProtectionProviderLib::ProtectionProviderList			protectionProviders;
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CSymCorpUIDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


