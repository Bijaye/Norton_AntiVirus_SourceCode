// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include <windows.h>
#include "DAPI.h"

//for dec2text integration for I-EMAIL 
#include "dec2ex.h"
#include "dec2lib.h"
#include "textfile.h"
#include "decinterface.h"


typedef struct tagDecOption
{
	DWORD	 dwID;
	DWORD	 dwValue;
} DECOPTION, *PDECOPTION;


/////////////////////////////////////////////////////////////////////////////
// Class CDecomposer
//
// The main Decomposer class.
//
// Applications use DecNewDecomposer() and DecDeleteDecomposer() to create
// and release objects of this class.

class CDecomposer:
	public IDecomposerEx
{
public:
	CDecomposer();
	virtual ~CDecomposer();

	// IDecomposer methods:
	virtual DECRESULT AddEngine(IDecEngine *pEngine);
	virtual DECRESULT RemoveEngines(void);
	virtual DECRESULT SetTempDir(const char *szDir);
	virtual DECRESULT SetMaxExtractSize(DWORD dwMaxSize);
	virtual DECRESULT AbortProcess(bool bAbort);
	virtual DECRESULT SetOption(DWORD dwOptionID, DWORD dwValue);
	virtual DECRESULT GetOption(DWORD dwOptionID, DWORD *pdwValue);
	virtual DECRESULT DecProcess(IDecObject *pObject,
								IDecEventSink *pSink,
								IDecIOCB *pIOCallback,
								WORD *pwResult,
								char *pszNewDataFile);
	virtual DECRESULT Process(IDecObject *pObject,
							 IDecEventSink *pSink,
							 IDecIOCB *pIOCallback,
							 WORD *pwResult,
							 char *pszNewDataFile);

	// IDecomposerEx methods:
	virtual DECRESULT DecProcess(IDecContainerObjectEx *pObject, IDecEventSink *pSink, WORD *pwResult, char *pszNewDataFile);

	//
	// Used by any engine that is not thread-safe.  For instance,
	// this is only used by the AMG engine at the moment.  In the 
	// static lib implementation for Netware, there is no way to
	// initialize a lock object in the engine itself.  We need to
	// ask the Decomposer itself for a lock object that is already
	// created and ready to go.  This frees the engine from worrying
	// about creating/destroying this object.  All platforms and
	// implementations (static lib, dll, shared obj, nlm) will now
	// use this method in order to be consistent.
	//
	virtual CCriticalSection & GetCriticalSection() { return m_CriticalSection; }
	virtual DWORD *GetUniquePtr() { return &m_dwUnique; }
	virtual DWORD GetUniqueValue();

private:
	// This value is used to generate temporary filenames.
	DWORD					m_dwUnique;
	CCriticalSection		m_UniqueCriticalSection;
	CCriticalSection		m_CriticalSection;
	IDecEngineEx			**m_Engines;
	int						m_EnginesCount;
	int						m_EnginesCapacity;
	DWORD					m_dwEventSinkInterfaceVersion;
	DECOPTION				m_Options[DEC_OPTION_COUNT];
};


////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


// static members
IDecFiles* CDecInterface::s_pIDecFiles = NULL;
IDecIO* CDecInterface::s_pIDecIO = NULL;
IDecomposerEx* CDecInterface::s_pDecomposer = NULL;
CTextEngine CDecInterface::s_cTextEngine;

CDecInterface::CDecInterface(void)
{
}

CDecInterface::~CDecInterface(void)
{
}

///////////////////////////////////////////////////////////
//
// DECRESULT CDecomposerSDK::NewObject(const char* szName, const char* szDataFile)
//
// parameters:
//          szName
//          szDataFile
//          szTempDir
//          **ppIDecObject
//
// this is used for Internet E-mail while we are integrating the ccDec
// this is a new interface that doesn't use dec2.dll to create a new decomposer object
//
DECRESULT CDecInterface::NewObject(const char *szName, 
								   const char *szDataFile, 
								   const char* szTempDir,
								   IDecContainerObjectEx **ppIDecObject)
{
	ULARGE_INTEGER liUnused, liFree;
	
	// Set the maximum extraction size to 90% of disk space or 1GB.
	if (!GetDiskFreeSpaceEx(szTempDir, &liUnused, &liUnused, &liFree))
		liFree.QuadPart = 1000000000;
	else
		liFree.QuadPart = ULONGLONG(LONGLONG(liFree.QuadPart) * .9);
 
    if( s_pDecomposer == NULL )
        return DECERR_GENERIC;

	int rv = s_pDecomposer->SetMaxExtractSize(min(1000000000, ULONG(liFree.QuadPart)));

	if (DEC_OK != rv)
		return rv;
	
	return decNewFileObject(szName, DEC_CHARSET_CP850, szDataFile, ppIDecObject);
}

/////////////////////////////////////////////////////////////////////////////
//
// DECRESULT CDecInterface::DecNewFileObject()
//
// Creates a new decomposer file object
//
// parameters:
//			szName
//			dwCharset
//			szDataFile
//			ppObject
//
// returns: DECRESULT
DECRESULT CDecInterface::decNewFileObject(const char *szName, 
										  DWORD dwCharset, 
										  const char *szDataFile, 
										  IDecContainerObjectEx **ppObject)
{
	// Validate parameters.
	if (szName == NULL || szDataFile == NULL || ppObject == NULL)
		return DECERR_INVALID_PARAMS;

	try
	{
		// Create a standard Decomposer data object to represent the data file.
		// We can always create a replacement data file, even if the Decomposer
		// user can't do anything with it.
		CDecObj *pObject = new CDecObj();
		if (pObject == NULL)
			return DECERR_OUT_OF_MEMORY;
		pObject->SetName(szName);
		pObject->SetNameCharacterSet(dwCharset);
		pObject->SetDataFile(szDataFile);
		pObject->SetCanDelete(true);
		pObject->SetCanModify(true);
		pObject->SetCanReplace(true);
		pObject->SetCanRename(true);

		// Upcast from the internal (Extended) object interface
		// using IDecObjectEx's IDecObject (vs IDecContainerObject's).
		*ppObject = pObject;

		return (*ppObject != NULL) ? DEC_OK : DECERR_OUT_OF_MEMORY;
	}
	catch (...)
	{
		*ppObject = NULL;
		return DECERR_OUT_OF_MEMORY;
	}
}


/////////////////////////////////////////////////////////////////////////////
// DECRESULT CDecInterface::DeleteFileObject()
//
// delete decomposer file object
// pulled from decomposer object
//
// parameters: IDecObject
//
// returns: Decomputer result
DECRESULT CDecInterface::DeleteObject(IDecContainerObjectEx *pObject)
{
	// Downcast from the internal (Extended) object interface
	// using IDecObjectEx's IDecObject (vs IDecContainerObject's).

	CDecObj *pCDecObj = static_cast<CDecObj *>(	static_cast<IDecObjectEx *>(pObject));

	delete pCDecObj;

	return DEC_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
// DECRESULT CDecInterface::NewDecomposer()
//
// Creates a new decomposer object
// 
// Parameters:
//			IDecomposer
//
// Returns:
//			DECRESULT
DECRESULT CDecInterface::NewDecomposer()
{
	try
	{
		s_pDecomposer = new CDecomposer;

        //set decomposer options
		if( s_pDecomposer != NULL ){
			s_pDecomposer->SetOption(DEC_OPTION_EXTRACT_OLE10NATIVE_ONLY, 1);
            s_pDecomposer->SetOption(DEC_OPTION_EXTRACT_RTFHTML, 1);
			s_pDecomposer->SetOption(DEC_OPTION_ENABLE_MIME_ENGINE, 1);
			s_pDecomposer->SetOption(DEC_OPTION_ENABLE_MBOX_ENGINE, 1);
			s_pDecomposer->SetOption(DEC_OPTION_ENABLE_UUE_ENGINE, 1);
			s_pDecomposer->SetOption(DEC_OPTION_ENHANCED_TEXT_ID, 8);
			s_pDecomposer->SetOption(DEC_OPTION_MIME_FUZZY_MAIN_HEADER, 100000);
			s_pDecomposer->AddEngine(&s_cTextEngine);
            s_pDecomposer->SetTempDir("mem://");
		}

		return (s_pDecomposer != NULL) ? DEC_OK : DECERR_OUT_OF_MEMORY;
	}
	catch (...)
	{
		s_pDecomposer = NULL;
		return DECERR_OUT_OF_MEMORY;
	}
}


/////////////////////////////////////////////////////////////////////////////
//
// DECRESULT CDecInterface::DeleteDecomposer()
//
// deletes a decomposer object
//
// parameters: 
//			IDecomposer
//
// Returns:
//		DECRESULT
DECRESULT CDecInterface::DeleteDecomposer()
{
	s_pDecomposer->RemoveEngines();
	delete static_cast<CDecomposer *>(s_pDecomposer);
	return DEC_OK;
}


///////////////////////////////////////////
//
// IDecomposerEx* CDecInterface::GetDecomposer() const
//
// parameters:
//          None
//
// returns:
//          Decomposer object
IDecomposerEx* CDecInterface::GetDecomposer() const
{
	return s_pDecomposer;
}

//////////////////////////////////////////////////////////////////////////
//
// bool CDecInterface::CheckFileSystem() const
//
// returns:
//		TRUE if filesystem is initialized
//		FALSE if filesystem isn't
bool CDecInterface::CheckFileSystem() const
{
	if( s_pIDecFiles == NULL ){
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////
//
// int CDecInterface::NewIOService()
//
// parameters:
//          None
//
// return: 
//          DECRESULT
int CDecInterface::NewIOService()
{
    //create a new decomposer file system
    if( s_pIDecFiles == NULL ){
		try
		{
			s_pIDecFiles = new CDecFiles;
		}
		catch (...)
		{
			s_pIDecFiles = NULL;
			return DAPI_ERROR;
		}
	}

    //create decomposer IO and file system
	try
	{
		s_pIDecIO = new CDecSDKIO;
		s_pIDecIO->SetFileSystem(s_pIDecFiles);
		return DAPI_OK;
	}
	catch (...)
	{
		s_pIDecIO = NULL;
		return DAPI_ERROR;
	}
}

/////////////////////////////////////////////////////
//
// void CDecInterface::DeleteIOService()
//
// parameters:
//          None
void CDecInterface::DeleteIOService()
{
	if (s_pIDecIO)
	{
		delete s_pIDecIO;
	}

    if( s_pIDecFiles )
    {
        delete s_pIDecFiles;
    }
}

//////////////////////////////////////////////////
//
// void CDecInterface::GetDecIO()
//
// parameters:
//          None
IDecIO* CDecInterface::GetDecIO() const
{
	return s_pIDecIO;
}
