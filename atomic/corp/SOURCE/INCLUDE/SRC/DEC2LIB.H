// Dec2Lib.h : Decomposer 2 Library (for Decomposer use only)
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2LIB_H)
#define DEC2LIB_H


/////////////////////////////////////////////////////////////////////////////
// Headers

// C++ Library headers
#pragma warning(disable:4786)
#include <list>
#include <string>
#if defined(_WINDOWS)
#ifdef UNICODE
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif
#endif // _WINDOWS

#if defined(UNIX)
#ifdef UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
#endif // UNIX

// Special value used internally to indicate that the size
// (compressed or uncompressed) of a container item has not
// been determined.
#define DEC_CHILD_SIZE_UNKNOWN  (DWORDLONG)-1

/////////////////////////////////////////////////////////////////////////////
// Standard Property Names

#define DEC_PROPERTY_GUESSTYPE _T("_GUESSTYPE")
#define DEC_PROPERTY_EXESIZE _T("_EXESIZE")


/////////////////////////////////////////////////////////////////////////////
// Support function for computing the size of an EXE

HRESULT GetExeSize(FILE* fp, DWORD* pdwSize, BOOL* pbIsZipExtractor);


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
   HRESULT MakeValidLowASCIIWindowsFilename (LPTSTR pszFilename);

private:
	CRITICAL_SECTION m_critical;  // Protects all data members.
	DWORD m_dwTempFile;
	TCHAR m_szTempDir[MAX_PATH];
	TCHAR m_szProcessString[3];
	HRESULT CreateTempFile(LPCTSTR szFile, BOOL* pbFileExists);
   void SeedTempFileName();
	HRESULT GetNextTempFileName(LPTSTR pszFile, int nRetries);
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
   virtual HRESULT GetNameCharacterSet(DWORD *pdwCharset);
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
   virtual HRESULT GetNormalizedName(LPTSTR pszInput, LPTSTR pszOutput, DWORD dwOutputSize, BOOL bANSI);

	// IDecObjectEx methods:
	virtual HRESULT SetName(LPCTSTR szName);
   virtual HRESULT SetNameCharacterSet(DWORD dwCharset);
	virtual HRESULT SetParent(IDecContainerObject* pParentObject);
	virtual HRESULT SetDataFile(LPCTSTR szDataFile);
	virtual HRESULT SetCanDelete(BOOL bCanDelete);
	virtual HRESULT SetCanModify(BOOL bCanModify);
	virtual HRESULT SetCanReplace(BOOL bCanReplace);

	// IDecContainerObject methods:
	virtual HRESULT GetType(DWORD* pdwType);
   virtual HRESULT GetUncompressedSize(DWORDLONG* pdwlSize);
   virtual HRESULT GetCompressedSize(DWORDLONG* pdwlSize);

	// IDecContainerObjectEx methods:
	virtual HRESULT SetType(DWORD dwType);
   virtual HRESULT SetUncompressedSize(DWORDLONG dwlSize);
   virtual HRESULT SetCompressedSize(DWORDLONG dwlSize);

private:
	TCHAR   m_szName[MAX_PATH];
	TCHAR   m_szDataFile[MAX_PATH];
	IDecContainerObject* m_pParent;
   DWORD   m_dwNameCharset;
	BOOL    m_bCanDelete;
	BOOL    m_bCanModify;
	BOOL    m_bCanReplace;
	DWORD   m_dwType;
   DWORDLONG m_dwlUncompressedSize;
   DWORDLONG m_dwlCompressedSize;
#if defined(_WINDOWS)
   std::list<std::pair<tstring, void*> > m_Properties;
#else
   list<pair<tstring, void*> > m_Properties;
#endif
};


#endif  // DEC2LIB_H
