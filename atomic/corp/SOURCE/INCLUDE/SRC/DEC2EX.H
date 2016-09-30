// Dec2Ex.h : Decomposer 2 Extended Interface (for Decomposer use only)
// Copyright 1999 by Symantec Corporation.  All rights reserved.


#if !defined(DEC2EX_H)
#define DEC2EX_H

#ifdef UNIX
#include "Dec2.h"
#endif

#if !defined(DEC2_H)
#error Include Dec2.h before Dec2EX.h.
#endif


/////////////////////////////////////////////////////////////////////////////
// Forward Declarations

class IDecomposerEx;
class IDecEngineEx;
class IDecObjectEx;
class IDecContainerObjectEx;


/////////////////////////////////////////////////////////////////////////////
// Interface IDecomposerEx

class IDecomposerEx :
	public IDecomposer
{
public:
	// IDecomposer methods:
	virtual HRESULT AddEngine(IDecEngine* pEngine) = 0;
	virtual HRESULT RemoveEngines() = 0;
	virtual HRESULT SetTempDir(LPCTSTR szDir) = 0;
	virtual HRESULT Process(IDecObject* pObject, IDecEventSink* pSink, WORD* pwResult, LPTSTR pszNewDataFile) = 0;

	// IDecomposerEx methods:
	virtual HRESULT Process(IDecContainerObjectEx* pObject, IDecEventSink* pSink, WORD* pwResult, LPTSTR pszNewDataFile) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecEngineEx

class IDecEngineEx :
	public IDecEngine
{
public:
	// IDecEngine methods:
	virtual HRESULT SetTempDir(LPCTSTR szDir) = 0;

	// IDecEngineEx methods:
	virtual HRESULT Process(
		IDecomposerEx* pDecomposer, 
		IDecContainerObjectEx* pObject, 
		IDecEventSink* pSink, 
		WORD* pwResult, 
		LPTSTR pszNewDataFile, 
		BOOL* pbIdentified, 
		BOOL* pbContinue) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecObjectEx

class IDecObjectEx :
	public IDecObject
{
public:
	// IDecObject methods:
	virtual HRESULT GetName(LPTSTR pszName) = 0;
    virtual HRESULT GetNameCharacterSet(DWORD *pdwCharset) = 0;
	virtual HRESULT GetParent(IDecContainerObject** ppParentObject) = 0;
	virtual HRESULT GetDepth(DWORD* pdwDepth) = 0;
	virtual HRESULT GetDataFile(LPTSTR pszDataFile) = 0;
	virtual HRESULT GetCanDelete(BOOL* pbCanDelete) = 0;
	virtual HRESULT GetCanDeleteAncestor(BOOL* pbCanDelete, DWORD* pdwDepth) = 0;
	virtual HRESULT GetCanModify(BOOL* pbCanModify) = 0;
	virtual HRESULT GetCanReplace(BOOL* pbCanReplace) = 0;
    virtual HRESULT GetProperty(LPCTSTR szName, void** ppValue) = 0;
    virtual HRESULT SetProperty(LPCTSTR szName, void* pValue) = 0;
    virtual HRESULT RemoveProperty(LPCTSTR szName) = 0;
    virtual HRESULT GetNormalizedName(LPTSTR pszInput, LPTSTR pszOutput, DWORD dwOutputSize, BOOL bANSI) = 0;

	// IDecObjectEx methods:
	virtual HRESULT SetName(LPCTSTR szName) = 0;
    virtual HRESULT SetNameCharacterSet(DWORD dwCharset) = 0;
	virtual HRESULT SetParent(IDecContainerObject* pParentObject) = 0;
	virtual HRESULT SetDataFile(LPCTSTR szDataFile) = 0;
	virtual HRESULT SetCanDelete(BOOL bCanDelete) = 0;
	virtual HRESULT SetCanModify(BOOL bCanModify) = 0;
	virtual HRESULT SetCanReplace(BOOL bCanReplace) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecContainerObjectEx

class IDecContainerObjectEx :
	public IDecObjectEx,
	public IDecContainerObject
{
public:
	// IDecObject methods:
	virtual HRESULT GetName(LPTSTR pszName) = 0;
    virtual HRESULT GetNameCharacterSet(DWORD *pdwCharset) = 0;
	virtual HRESULT GetParent(IDecContainerObject** ppParentObject) = 0;
	virtual HRESULT GetDepth(DWORD* pdwDepth) = 0;
	virtual HRESULT GetDataFile(LPTSTR pszDataFile) = 0;
	virtual HRESULT GetCanDelete(BOOL* pbCanDelete) = 0;
	virtual HRESULT GetCanDeleteAncestor(BOOL* pbCanDelete, DWORD* pdwDepth) = 0;
	virtual HRESULT GetCanModify(BOOL* pbCanModify) = 0;
	virtual HRESULT GetCanReplace(BOOL* pbCanReplace) = 0;
    virtual HRESULT GetProperty(LPCTSTR szName, void** ppValue) = 0;
    virtual HRESULT SetProperty(LPCTSTR szName, void* pValue) = 0;
    virtual HRESULT RemoveProperty(LPCTSTR szName) = 0;
    virtual HRESULT GetNormalizedName(LPTSTR pszInput, LPTSTR pszOutput, DWORD dwOutputSize, BOOL bANSI) = 0;

	// IDecObjectEx methods:
	virtual HRESULT SetName(LPCTSTR szName) = 0;
    virtual HRESULT SetNameCharacterSet(DWORD dwCharset) = 0;
	virtual HRESULT SetParent(IDecContainerObject* pParentObject) = 0;
	virtual HRESULT SetDataFile(LPCTSTR szDataFile) = 0;
	virtual HRESULT SetCanDelete(BOOL bCanDelete) = 0;
	virtual HRESULT SetCanModify(BOOL bCanModify) = 0;
	virtual HRESULT SetCanReplace(BOOL bCanReplace) = 0;

	// IDecContainerObject methods:
	virtual HRESULT GetType(DWORD* pdwType) = 0;
    virtual HRESULT GetUncompressedSize(DWORDLONG* pdwlSize) = 0;
    virtual HRESULT GetCompressedSize(DWORDLONG* pdwlSize) = 0;

	// IDecContainerObjectEx methods:
	virtual HRESULT SetType(DWORD dwType) = 0;
    virtual HRESULT SetUncompressedSize(DWORDLONG dwlSize) = 0;
    virtual HRESULT SetCompressedSize(DWORDLONG dwlSize) = 0;
};


#endif  // DEC2EX_H
