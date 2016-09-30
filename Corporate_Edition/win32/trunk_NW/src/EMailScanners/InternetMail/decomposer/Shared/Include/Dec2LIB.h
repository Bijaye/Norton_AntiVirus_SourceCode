// Dec2LIB.h : Decomposer 2 Library (for Decomposer use only)
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2LIB_H)
#define DEC2LIB_H

#include "fixibmps.h"

// define MAX_PATH if not already defined
#if !defined( MAX_PATH )
  #if defined(_MAX_PATH)
	#define MAX_PATH _MAX_PATH
  
  #elif defined(_POSIX_PATH_MAX)
	#define MAX_PATH _POSIX_PATH_MAX

  #elif defined( PATH_MAX )
	#define MAX_PATH PATH_MAX
  
  #else
	#error MAX_PATH not declared correctly!
  #endif
#endif

#include "Support.h"
#include "FastArray.h"

// Special value used internally to indicate that the size
// (compressed or uncompressed) of a container item has not
// been determined.
#define DEC_CHILD_SIZE_UNKNOWN	(DWORDLONG)-1
#define	DEFAULT_ATTRIBUTE_COLLECTION_SIZE 15

/////////////////////////////////////////////////////////////////////////////
// Standard Property Names - Reserved for internal decomposer use

#define DEC_PROPERTY_GUESSTYPE	"_GUESSTYPE"        // File format guess (ZIP, ARJ, etc...)
#define DEC_PROPERTY_EXESIZE	"_EXESIZE"          // Size of EXE portion (if applicable)

/////////////////////////////////////////////////////////////////////////////
// Support function for computing the size of an EXE

DECRESULT GetExeSize(FILE *fp, DWORD *pdwSize, bool *pbIsZipExtractor);

//
// Size of the buffer we use in the ICachedDecObject interface
//
#define SIZE_INITIAL_BUFFER_READ	(1 * 1024)

/////////////////////////////////////////////////////////////////////////////
// CDecEngine
//
// Standard base class of a Decomposer engine.

class CDecEngine :
	public IDecEngineEx
{
public:
	CDecEngine();
	~CDecEngine();

	// IDecEngine methods:
	DECRESULT SetTempDir(const char *szDir);
	DECRESULT SetMaxExtractSize(DWORD dwMaxSize);
	DECRESULT AbortProcess(bool bAbort);
	DECRESULT SetIOCallback(IDecIOCB *pIOCallback);
	DECRESULT CreateTempFile(const char *szExtension, char *pszFile, IDecomposerEx *pDecomposer);

protected:
	DECRESULT GetTempDir(char *pszDir);
	DECRESULT FilterObject(IDecContainerObject *pObject, DWORD types[], bool *pbContinue);
	DECRESULT FilterObjectByType(IDecContainerObject *pObject, DWORD types[], bool *pbContinue);
	DECRESULT FindExtension(const char *szName, const char **ppszExtension);
	DECRESULT FindNativeExtension(const char *szName, DWORD dwNameType, char *pszDst);
	DECRESULT MakeValidLowASCIIWindowsFilename (char *pszFilename);

private:
	DECRESULT CreateTempFile(const char *szFile, bool *pbFileExists);
	DECRESULT GetNextTempFileName(char *pszFile, IDecomposerEx *pDecomposer);

	char	m_szTempDir[MAX_PATH];
};


/////////////////////////////////////////////////////////////////////////////
// CDecObj
//
// Standard implementation of a Decomposer data object

class CDecObj : 
	public IDecContainerObjectEx
{
public:
	CDecObj(IDecContainerObject* pParentObject = NULL);
	~CDecObj();

	// IDecObject methods:
	
#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	AttributeState GetAttributeState() const;
	IDecAttributeCollection* GetAttributeCollectionAt(size_t index) const;
	size_t GetNumberOfAttributeCollections() const;
	AttributeSupport GetAttributeSupport() const;
#endif //*** End AS400 Attribute API Removal *** 

	DECRESULT GetName(char *pszName, DWORD *pdwNameSize, DWORD *pdwCharset);
	DECRESULT GetSecondaryName(char *pszName, DWORD *pdwNameSize, DWORD *pdwCharset);
	DECRESULT GetNameType(DWORD *pdwNameType);
	DECRESULT GetParent(IDecContainerObject** ppParentObject);
	DECRESULT GetDepth(DWORD* pdwDepth);
	DECRESULT GetDataFile(char *pszDataFile, DWORD *pdwNameSize);
	DECRESULT GetCanDelete(bool* pbCanDelete);
	DECRESULT GetCanDeleteAncestor(bool* pbCanDelete, DWORD* pdwDepth);
	DECRESULT GetCanModify(bool* pbCanModify);
	DECRESULT GetCanReplace(bool* pbCanReplace);
	DECRESULT GetCanRename(bool* pbCanRename);
	DECRESULT GetProperty(const char *szName, const char *&szValue);
	DECRESULT SetProperty(const char *szName, const char *szValue);
	DECRESULT GetProperty(const char *szName, DWORD *pValue);
	DECRESULT GetProperty(const char *szName, DWORD &value);
	DECRESULT SetProperty(const char *szName, DWORD value);
	DECRESULT RemoveProperty(const char *szName);
	DECRESULT GetNormalizedName(char *pszInput, char *pszOutput, DWORD dwOutputSize, bool bANSI);
	DECRESULT GetRenameItemName(char **ppszName);
	DECRESULT SetRenameItemName(const char *pszName, bool bSetParent);
	DECRESULT GetMIMEItemContentType(char **ppszType);
	DECRESULT SetMIMEItemContentType(const char *pszType, bool bSetParent);
	DECRESULT GetAttributes(DWORD *pdwAttrs, DWORD *pdwAttrType);
	DECRESULT SetAttributes(DWORD dwAttrs, DWORD dwAttrType);
	DECRESULT GetTime(DWORD *pdwHour, DWORD *pdwMinute, DWORD *pdwSecond, DWORD *pdwMillisecond);
	DECRESULT SetTime(DWORD dwHour, DWORD dwMinute, DWORD dwSecond, DWORD dwMillisecond);
	DECRESULT GetDate(DWORD *pdwYear, DWORD *pdwMonth, DWORD *pdwDay);
	DECRESULT SetDate(DWORD dwYear, DWORD dwMonth, DWORD dwDay);
	DECRESULT GetItemType(DWORD *pdwType);
	DECRESULT SetItemType(DWORD dwType);
	DECRESULT GetMalformed(DWORD *pdwType);
	DECRESULT SetMalformed(DWORD dwType);
	DECRESULT GetEngineInterface(void **ppInterface);
	DECRESULT SetEngineInterface(void *pInterface);
	virtual const char* ToString(unsigned int dwLevel);


	// IDecObjectEx methods:
	DECRESULT SetName(const char *szName);
	DECRESULT SetSecondaryName(const char *szName);
	DECRESULT SetNameType(DWORD dwNameType);
	DECRESULT SetNameCharacterSet(DWORD dwCharset);
	DECRESULT SetParent(IDecContainerObject* pParentObject);
	DECRESULT SetDataFile(const char *szDataFile);
	DECRESULT SetCanDelete(bool bCanDelete);
	DECRESULT SetCanModify(bool bCanModify);
	DECRESULT SetCanReplace(bool bCanReplace);
	DECRESULT SetCanRename(bool bCanRename);

	// IDecContainerObject methods:
	DECRESULT GetContainerType(DWORD *pdwType);
	DECRESULT GetUncompressedSize(DWORDLONG *pdwlSize);
	DECRESULT GetCompressedSize(DWORDLONG *pdwlSize);

	// IDecContainerObjectEx methods:
	DECRESULT SetContainerType(DWORD dwType);
	DECRESULT SetUncompressedSize(DWORDLONG dwlSize);
	DECRESULT SetCompressedSize(DWORDLONG dwlSize);	
	
#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	void	  AddAttributeCollection(IDecAttributeCollection* pAttrCollection);
	void	  FinalizeAttributes();
	void	  SetAttributesSupported();
#endif //*** End AS400 Attribute API Removal *** 

	// ICachedDecObject methods
	FILE *OpenDataFile(const char *pszFilename, 
					  const char *pszMode, 
					  BYTE **ppbyBuffer, 
					  size_t *pSizeOfBuffer, 
					  long *plFileSize);
	int CloseDataFile();

private:
	int FindProperty(const char *szName);
	int FindAvailableProperty(const char *szName);

#if !defined(OS400)  //*** Start AS400 Attribute API Removal *** 
	const char* GetAttrStateEnum (DWORD type);
	const char* GetAttrSupportEnum (DWORD type);

	// Begin data members associated with attributes...
	FastArray< IDecAttributeCollection* , DEFAULT_ATTRIBUTE_COLLECTION_SIZE> m_AttributeCollection;
	AttributeState		m_eAttributeState;
	AttributeSupport	m_eAttributeSupport;
	// End data members associated with attributes...

#endif //*** End AS400 Attribute API Removal *** 


	char	*m_pszName;
	char	*m_pszName2;
	char	m_szDataFile[MAX_PATH];
	char	m_szRenameName[MAX_PATH];
	char	m_szMIMEContentType[64];
	IDecContainerObject *m_pParent;
	void	*m_pEngineInterface;
	DWORD	m_dwNameCharset;
	DWORD	m_dwNameType;
	bool	m_bCanDelete;
	bool	m_bCanModify;
	bool	m_bCanReplace;
	bool	m_bCanRename;
	DWORD	m_dwMalformedType;
	DWORD	m_dwContainerType;
	DWORD	m_dwItemType;
	DWORD	m_dwAttrs;
	DWORD	m_dwAttrType;
	DWORD	m_dwYear;
	DWORD	m_dwMonth;
	DWORD	m_dwDay;
	DWORD	m_dwHour;
	DWORD	m_dwMinute;
	DWORD	m_dwSecond;
	DWORD	m_dwMillisecond;
	DWORDLONG m_dwlUncompressedSize;
	DWORDLONG m_dwlCompressedSize;
	
	FILE	*m_pCachedFile;
	BYTE	*m_pbyCachedBuffer;
	size_t	m_sizeCachedBufferSize;
	long	m_lCachedFileSize;
	char*	m_pDecObjValString;
	
	// Implementation of properties
#define MAX_PROP_NAME_LEN 32
	struct _DEC_OBJ_PROP 
	{
		char name[MAX_PROP_NAME_LEN + 1];
		DWORD value;
		char *strValue;
	};

#define MAX_PROPS 20
	struct _DEC_OBJ_PROP m_Properties[MAX_PROPS];
};


#endif	// DEC2LIB_H
