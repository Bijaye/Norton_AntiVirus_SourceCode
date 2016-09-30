// Dec2Lib.h : Decomposer 2 Library (for Decomposer use only)
// Copyright 1998 by Symantec Corporation.  All rights reserved.
// $Header:   S:/INCLUDE/VCS/DEC2LIB.H_v   1.0   16 Mar 1998 19:04:02   MKEATIN  $


#if !defined(DEC2LIB_H)
#define DEC2LIB_H


/////////////////////////////////////////////////////////////////////////////
// Headers

// C++ Library headers
#pragma warning(disable:4786)
#include <list>
#include <string>
#ifdef UNICODE
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif


/////////////////////////////////////////////////////////////////////////////
// Standard Property Names

#define DEC_PROPERTY_GUESSTYPE _T("_GUESSTYPE")


/////////////////////////////////////////////////////////////////////////////
// CDecEngine
//
// Standard base class of a Decomposer engine.

class CDecEngine :
	public IDecEngineEx
{
public:
	CDecEngine();
	virtual ~CDecEngine();

	// IDecEngine methods:
	virtual HRESULT SetTempDir(LPCTSTR szDir);
    virtual HRESULT SetTrustFileExtensions(BOOL bEnabled);

protected:
	HRESULT GetTempDir(LPTSTR pszDir);
    HRESULT GetTrustFileExtensions(BOOL* pbEnabled);
	HRESULT CreateTempFile(LPCTSTR szExtension, LPTSTR pszFile);
    HRESULT FilterObject(IDecContainerObject* pObject, LPCTSTR extensions[], DWORD types[], BOOL* pbContinue);
    HRESULT FilterObjectByFileExtension(IDecContainerObject* pObject, LPCTSTR extensions[], BOOL* pbContinue);
    HRESULT FilterObjectByType(IDecContainerObject* pObject, DWORD types[], BOOL* pbContinue);
    HRESULT FindExtension(LPCTSTR szName, LPCTSTR* ppszExtension);

private:
	CRITICAL_SECTION m_critical;  // Protects all data members.
	DWORD m_dwTempFile;
	TCHAR m_szTempDir[MAX_PATH];
	HRESULT CreateTempFile(LPCTSTR szFile, BOOL* pbFileExists);
	HRESULT GetNextTempFileName(LPTSTR pszFile);
    BOOL m_bTrustFileExtensions;
};


/////////////////////////////////////////////////////////////////////////////
// CDecObj
//
// Standard implementation of a Decomposer data object

class CDecObj : 
	public IDecContainerObjectEx
{
public:
	CDecObj();

	// IDecObject methods:
	virtual HRESULT GetName(LPTSTR szName);
	virtual HRESULT GetParent(IDecContainerObject** ppParentObject);
	virtual HRESULT GetDepth(DWORD* pdwDepth);
	virtual HRESULT GetDataFile(LPTSTR pszDataFile);
	virtual HRESULT GetCanDelete(BOOL* pbCanDelete);
	virtual HRESULT GetCanDeleteAncestor(BOOL* pbCanDelete, DWORD* pdwDepth);
	virtual HRESULT GetCanModify(BOOL* pbCanModify);
	virtual HRESULT GetCanReplace(BOOL* pbCanReplace);
    virtual HRESULT GetProperty(LPCTSTR szName, void** ppValue);
    virtual HRESULT SetProperty(LPCTSTR szName, void* pValue);
    virtual HRESULT RemoveProperty(LPCTSTR szName);

	// IDecObjectEx methods:
	virtual HRESULT SetName(LPCTSTR szName);
	virtual HRESULT SetParent(IDecContainerObject* pParentObject);
	virtual HRESULT SetDataFile(LPCTSTR szDataFile);
	virtual HRESULT SetCanDelete(BOOL bCanDelete);
	virtual HRESULT SetCanModify(BOOL bCanModify);
	virtual HRESULT SetCanReplace(BOOL bCanReplace);

	// IDecContainerObject methods:
	virtual HRESULT GetType(DWORD* pdwType);

	// IDecContainerObjectEx methods:
	virtual HRESULT SetType(DWORD dwType);

private:
	TCHAR m_szName[MAX_PATH];
	TCHAR m_szDataFile[MAX_PATH];
	IDecContainerObject* m_pParent;
	BOOL m_bCanDelete;
	BOOL m_bCanModify;
	BOOL m_bCanReplace;
	DWORD m_dwType;
    std::list<std::pair<tstring, void*> > m_Properties;

};


#endif  // DEC2LIB_H
