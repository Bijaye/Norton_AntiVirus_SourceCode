// Dec2EX.h : Decomposer 2 Extended Interface (for Decomposer use only)
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2EX_H)
#define DEC2EX_H

#ifdef UNIX
#include "Dec2.h"
#endif

#if !defined(DEC2_H)
#error Include Dec2.h before Dec2EX.h.
#endif

#include <stdio.h>		// For FILE *

/////////////////////////////////////////////////////////////////////////////
// Forward Declarations

class IDecomposerEx;
class IDecEngineEx;
class IDecObjectEx;
class IDecContainerObjectEx;
class CCriticalSection;
class IDecAttributeCollection;

/////////////////////////////////////////////////////////////////////////////
// Interface IDecomposerEx

class IDecomposerEx :
	public IDecomposer
{
public:
	IDecomposerEx() {}
	virtual ~IDecomposerEx() {}

	// IDecomposerEx methods:
	virtual DECRESULT DecProcess(IDecContainerObjectEx *pObject,
								IDecEventSink *pSink,
								WORD *pwResult,
								char *pszNewDataFile) = 0;

	virtual CCriticalSection & GetCriticalSection() = 0;	// This is assigned to the ArcManager engine.
	virtual DWORD *GetUniquePtr() = 0;
	virtual DWORD GetUniqueValue() = 0;

};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecEngineEx

class IDecEngineEx :
	public IDecEngine
{
public:
	IDecEngineEx() {}
	virtual ~IDecEngineEx() {}

	// IDecEngineEx methods:
	virtual DECRESULT Process(
		IDecomposerEx *pDecomposer,
		IDecContainerObjectEx *pObject,
		IDecEventSink *pSink,
		WORD *pwResult,
		char *pszNewDataFile,
		bool *pbIdentified,
		bool *pbContinue) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecObjectEx

class IDecObjectEx :
	public IDecContainerObject
{
public:
	IDecObjectEx() {}
	virtual ~IDecObjectEx() {}

	// IDecObjectEx methods:
	virtual DECRESULT	SetName(const char *szName) = 0;
	virtual DECRESULT	SetSecondaryName(const char *szName) = 0;
	virtual DECRESULT	SetNameType(DWORD dwNameType) = 0;
	virtual DECRESULT	SetNameCharacterSet(DWORD dwCharset) = 0;
	virtual DECRESULT	SetParent(IDecContainerObject* pParentObject) = 0;
	virtual DECRESULT	SetDataFile(const char *szDataFile) = 0;
	virtual DECRESULT	SetCanDelete(bool bCanDelete) = 0;
	virtual DECRESULT	SetCanModify(bool bCanModify) = 0;
	virtual DECRESULT	SetCanReplace(bool bCanReplace) = 0;
	virtual DECRESULT	SetCanRename(bool bCanRename) = 0;

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	virtual void		AddAttributeCollection(IDecAttributeCollection* pAttrCollection) = 0;
	virtual void		FinalizeAttributes() = 0;
	virtual	void		SetAttributesSupported() = 0;
#endif //*** End AS400 Attribute API Removal *** 

};


/////////////////////////////////////////////////////////////////////////////
// Interface ICachedDecObject

class ICachedDecObject
{
public:
	ICachedDecObject() {}
	virtual ~ICachedDecObject() {}
	
	virtual FILE *OpenDataFile(const char *pszFilename, 
							  const char *pszMode, 
							  BYTE **ppbyBuffer, 
							  size_t *pSizeOfBuffer, 
							  long *plFileSize) = 0;
	virtual int CloseDataFile() = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecContainerObjectEx

class IDecContainerObjectEx :
	public IDecObjectEx,
	public ICachedDecObject
{
public:
	IDecContainerObjectEx() {}
	virtual ~IDecContainerObjectEx() {}

	// IDecObject methods:
	virtual DECRESULT GetName(char *pszName, DWORD *pdwNameSize, DWORD *pdwCharset) = 0;
	virtual DECRESULT GetSecondaryName(char *pszName, DWORD *pdwNameSize, DWORD *pdwCharset) = 0;
	virtual DECRESULT GetNameType(DWORD *pdwNameType) = 0;
	virtual DECRESULT GetParent(IDecContainerObject **ppParentObject) = 0;
	virtual DECRESULT GetDepth(DWORD *pdwDepth) = 0;
	virtual DECRESULT GetDataFile(char *pszDataFile, DWORD *pdwNameSize) = 0;
	virtual DECRESULT GetCanDelete(bool *pbCanDelete) = 0;
	virtual DECRESULT GetCanDeleteAncestor(bool *pbCanDelete, DWORD *pdwDepth) = 0;
	virtual DECRESULT GetCanModify(bool *pbCanModify) = 0;
	virtual DECRESULT GetCanReplace(bool *pbCanReplace) = 0;
	virtual DECRESULT GetCanRename(bool *pbCanRename) = 0;
	virtual DECRESULT GetProperty(const char *szName, DWORD &value) = 0;
	virtual DECRESULT GetProperty(const char *szName, DWORD *pValue) = 0;
	virtual DECRESULT SetProperty(const char *szName, DWORD value) = 0;
	virtual DECRESULT GetProperty(const char *szName, const char *&value) = 0;
	virtual DECRESULT SetProperty(const char *szName, const char *value) = 0;
	virtual DECRESULT RemoveProperty(const char *szName) = 0;
	virtual DECRESULT GetNormalizedName(char *pszInput, char *pszOutput, DWORD dwOutputSize, bool bANSI) = 0;
	virtual DECRESULT GetRenameItemName(char **ppszName) = 0;
	virtual DECRESULT SetRenameItemName(const char *pszName, bool bSetParent) = 0;
	virtual DECRESULT GetMIMEItemContentType(char **ppszType) = 0;
	virtual DECRESULT SetMIMEItemContentType(const char *pszType, bool bSetParent) = 0;
	virtual DECRESULT GetAttributes(DWORD *pdwAttrs, DWORD *pdwAttrType) = 0;
	virtual DECRESULT SetAttributes(DWORD dwAttrs, DWORD dwAttrType) = 0;
	virtual DECRESULT GetTime(DWORD *pdwHour, DWORD *pdwMinute, DWORD *pdwSecond, DWORD *pdwMillisecond) = 0;
	virtual DECRESULT SetTime(DWORD dwHour, DWORD dwMinute, DWORD dwSecond, DWORD dwMillisecond) = 0;
	virtual DECRESULT GetDate(DWORD *pdwYear, DWORD *pdwMonth, DWORD *pdwDay) = 0;
	virtual DECRESULT SetDate(DWORD dwYear, DWORD dwMonth, DWORD dwDay) = 0;
	virtual DECRESULT GetItemType(DWORD *pdwType) = 0;
	virtual DECRESULT SetItemType(DWORD dwType) = 0;
	virtual DECRESULT GetMalformed(DWORD *pdwType) = 0;
	virtual DECRESULT SetMalformed(DWORD dwType) = 0;
	virtual DECRESULT GetEngineInterface(void **ppInterface) = 0;
	virtual DECRESULT SetEngineInterface(void *pInterface) = 0;

	// IDecObjectEx methods:
	virtual DECRESULT SetName(const char *szName) = 0;
	virtual DECRESULT SetSecondaryName(const char *szName) = 0;
	virtual DECRESULT SetNameType(DWORD dwNameType) = 0;
	virtual DECRESULT SetNameCharacterSet(DWORD dwCharset) = 0;
	virtual DECRESULT SetParent(IDecContainerObject* pParentObject) = 0;
	virtual DECRESULT SetDataFile(const char *szDataFile) = 0;
	virtual DECRESULT SetCanDelete(bool bCanDelete) = 0;
	virtual DECRESULT SetCanModify(bool bCanModify) = 0;
	virtual DECRESULT SetCanReplace(bool bCanReplace) = 0;
	virtual DECRESULT SetCanRename(bool bCanRename) = 0;

	// IDecContainerObject methods:
	virtual DECRESULT GetContainerType(DWORD *pdwType) = 0;
	virtual DECRESULT GetUncompressedSize(DWORDLONG *pdwlSize) = 0;
	virtual DECRESULT GetCompressedSize(DWORDLONG *pdwlSize) = 0;

	// IDecContainerObjectEx methods:
	virtual DECRESULT SetContainerType(DWORD dwType) = 0;
	virtual DECRESULT SetUncompressedSize(DWORDLONG dwlSize) = 0;
	virtual DECRESULT SetCompressedSize(DWORDLONG dwlSize) = 0;
	
	// ICachedDecObject methods:
	virtual FILE *OpenDataFile(const char *pszFilename, 
							  const char *pszMode, 
							  BYTE **ppbyBuffer, 
							  size_t *pSizeOfBuffer, 
							  long *plFileSize) = 0;
	virtual int CloseDataFile() = 0;
};

#endif	// DEC2EX_H
