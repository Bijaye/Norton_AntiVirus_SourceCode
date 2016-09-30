// DecEngine.cpp: Implementation of the CDecSDKEngine class.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#define DAPI_CPP
#include "DecPlat.h"
#include "DAPI.h"
#include "DecEngine.h"

#if defined(SYM_PLATFORM_HAS_SHAREDOBJS)
#include <dlfcn.h>
#endif

// The following engine header files are included here only for those platforms
// that do NOT support dynamic linking.
#if !defined(SYM_PLATFORM_HAS_DLLS) || !defined(SYM_PLATFORM_HAS_SHAREDOBJS)
#include "Dec2ID.h"
//#include "Dec29660.h"
#include "Dec2AMG.h"
#include "Dec2ARC.h"
#include "Dec2ARJ.h"
#include "Dec2AS.h"
#include "Dec2CAB.h"
#include "Dec2GZIP.h"
#include "Dec2LHA.h"
#include "Dec2LZ.h"
#include "Dec2MB3.h"
#include "Dec2OLE1.h"
#include "Dec2PDF.h"
#include "Dec2RAR.h"
#include "Dec2RTF.h"
#include "Dec2SS.h"
#include "Dec2TAR.h"
#include "Dec2Text.h"
#include "Dec2TNEF.h"
#include "Dec2ZIP.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDecSDKEngine::CDecSDKEngine() :
	m_pEngine( NULL ),
	m_hInst( NULL ),
	m_pfnNew( NULL ),
	m_pfnDelete( NULL )
{
}


CDecSDKEngine::~CDecSDKEngine()
{
	UnloadEngine();
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CDecSDKEngine::LoadEngine
//
// Description  : Attempts to load/link the specified engine
//
// Argument     : const char    *pszPath - location of the Decomposer engine file
// Argument     : const char    *pszFilename - name of the Decomposer engine file
// Argument     : int           iEngineID - DEC_TYPE_xxx engine identifier
// Argument     : const char    *pszNewFunc - name of the engine's creation function
// Argument     : const char    *pszDeleteFunc - name of the engine's destruction function
//
///////////////////////////////////////////////////////////////////////////////
bool CDecSDKEngine::LoadEngine(const char	*pszPath,
							  const char	*pszFilename,
							  int			iEngineID,
							  const char	*pszNewFunc,
							  const char	*pszDeleteFunc)
{

#if defined(SYM_PLATFORM_HAS_DLLS) || defined (SYM_PLATFORM_HAS_SHAREDOBJS)
	char	szEngineFilename[MAX_PATH];
	int		iLen1, iLen2;
	char	chLast;

	// Construct the fully-qualified filename of the engine file.
	// Add a path separator if necessary.
	iLen1 = strlen(pszPath);
	iLen2 = strlen(pszFilename);

	if (iLen1 + iLen2 + 2 >= sizeof(szEngineFilename))
		return(false);	// Constructed path would be too long...

	strcpy(szEngineFilename, pszPath);

	if (iLen1)
		iLen1--;
	
	chLast = szEngineFilename[iLen1];

	if (chLast != '/' &&
		chLast != '\\')
	{
	#if defined(_WINDOWS)
		strcat(szEngineFilename, "\\");
	#else
		strcat(szEngineFilename, "/");
	#endif
	}

	strcat(szEngineFilename, pszFilename);

	#if defined(SYM_PLATFORM_HAS_DLLS)
		// Platform-specific logic for loading a Windows DLL.

		// Try loading the library.
		m_hInst = LoadLibraryEx(szEngineFilename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	#endif

	#if defined(SYM_PLATFORM_HAS_SHAREDOBJS)
		// Platform-specific logic for loading a Unix shared object.

		// Try loading the library.
		m_hInst = dlopen(szEngineFilename, RTLD_NOW);
	#endif

	if (m_hInst == NULL)
		return(false);

#endif

	return (LinkEngine(iEngineID, pszNewFunc, pszDeleteFunc));
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CDecSDKEngine::LoadEngineW
//
// Description  : Attempts to load/link the specified engine
//
// Argument     : const char    *pszPath - location of the Decomposer engine file
// Argument     : const char    *pszFilename - name of the Decomposer engine file
// Argument     : int           iEngineID - DEC_TYPE_xxx engine identifier
// Argument     : const char    *pszNewFunc - name of the engine's creation function
// Argument     : const char    *pszDeleteFunc - name of the engine's destruction function
//
///////////////////////////////////////////////////////////////////////////////
bool CDecSDKEngine::LoadEngineW(const wchar_t	*pszPath,
								const wchar_t	*pszFilename,
								int				iEngineID,
								const char		*pszNewFunc,
								const char		*pszDeleteFunc)
{
#if defined(SYM_PLATFORM_HAS_DLLS)
	wchar_t szEngineFilename[MAX_PATH];
	int		iLen1, iLen2;
	wchar_t chLast;

	// Construct the fully-qualified filename of the engine file.
	iLen1 = wcslen(pszPath);
	iLen2 = wcslen(pszFilename);

	// Determine if the generated path will be greater than szEngineFilename[MAX_PATH]
	// String length is doubled due to being a UNICODE string (two bytes per wchar_t)
	if (((iLen1 + iLen2 + 1) * 2)  >= sizeof(szEngineFilename))
		return(false);	// Constructed path would be too long...

	wcscpy(szEngineFilename, pszPath);

	if (iLen1)
		iLen1--;

	// Retrieve char before the null
	chLast = szEngineFilename[iLen1];

	// Add a path separator if necessary.
	if (chLast != L'/' && chLast != L'\\')
	{
		/*
			Determine if the generated path will be greater than szEngineFilename[MAX_PATH]
			Increment of 2 for path separator and terminating null
			String length is doubled due to being a UNICODE string (two bytes per wchar_t)
		*/
		if (((iLen1 + iLen2 + 2) * 2) >= sizeof(szEngineFilename))
			return(false);	// Constructed path would be too long...

	#if defined(_WINDOWS)
		wcscat(szEngineFilename, L"\\");
	#else
		wcscat(szEngineFilename, L"/");
	#endif

	}

	// Generate full path to module
	wcscat(szEngineFilename, pszFilename);

	// Platform-specific logic for loading a Windows DLL.

	// Try loading the library.
	m_hInst = LoadLibraryExW(szEngineFilename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (m_hInst == NULL)
		return(false); 
	
	return (LinkEngine(iEngineID, pszNewFunc, pszDeleteFunc));
#else
	// 
	// No wide char support for these functions under any flavors of UNIX
	//
	m_pfnNew = NULL;
	m_pfnDelete = NULL;

	return(false);
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CDecSDKEngine::LinkEngine
//
// Description  : Attempts to link the specified engine
//
///////////////////////////////////////////////////////////////////////////////
bool CDecSDKEngine::LinkEngine(int			iEngineID,
							  const char	*pszNewFunc,
							  const char	*pszDeleteFunc)
{

#if defined(SYM_PLATFORM_HAS_DLLS)
	// Get pointers to exported functions.  Need to handle formatting of mangled function names.
	char szCreate[64];
	char szDestroy[64];
	strcpy(szCreate, "?");
	strcat(szCreate, pszNewFunc);
	strcat(szCreate, "@@YAJPAPAVIDecEngine@@@Z");
	m_pfnNew = (PFNDECCREATEENGINE)GetProcAddress(m_hInst, szCreate);

	strcpy(szDestroy, "?");
	strcat(szDestroy, pszDeleteFunc);
	strcat(szDestroy, "@@YAJPAVIDecEngine@@@Z");
	m_pfnDelete = (PFNDECDESTROYENGINE)GetProcAddress(m_hInst, szDestroy);

#elif defined(SYM_PLATFORM_HAS_SHAREDOBJS)
	// Get pointers to exported functions for platforms with shared objects.
	m_pfnNew = (PFNDECCREATEENGINE)dlsym(m_hInst, pszNewFunc);
	m_pfnDelete = (PFNDECDESTROYENGINE)dlsym(m_hInst, pszDeleteFunc);

#else
	// For those platforms that do not support dynamic linking, we must call the
	// engine entry points directly by name.
	switch (iEngineID)
	{
		case DEC_TYPE_AMG:
			m_pfnNew = &DecNewAMGEngine;
			m_pfnDelete = &DecDeleteAMGEngine;
			break;
//        case DEC_TYPE_ARC:
//            m_pfnNew = &DecNewARCEngine;
//            m_pfnDelete = &DecDeleteARCEngine;
//            break;
		case DEC_TYPE_ARJ:
			m_pfnNew = &DecNewARJEngine;
			m_pfnDelete = &DecDeleteARJEngine;
			break;
		case DEC_TYPE_AS:
			m_pfnNew = &DecNewASEngine;
			m_pfnDelete = &DecDeleteASEngine;
			break;
//        case DEC_TYPE_CAB:
//            m_pfnNew = &DecNewCabEngine;
//            m_pfnDelete = &DecDeleteCabEngine;
//            break;
		case DEC_TYPE_GZIP:
			m_pfnNew = &DecNewGZIPEngine;
			m_pfnDelete = &DecDeleteGZIPEngine;
			break;
		case DEC_TYPE_ID:
			m_pfnNew = &DecNewIDEngine;
			m_pfnDelete = &DecDeleteIDEngine;
			break;
//        case DEC_TYPE_ISO9660:
//            m_pfnNew = &DecNew9660Engine;
//            m_pfnDelete = &DecDelete9660Engine;
//            break;
//        case DEC_TYPE_JAR:
//            m_pfnNew = &DecNewJAREngine;
//            m_pfnDelete = &DecDeleteJAREngine;
//            break;
		case DEC_TYPE_LHA:
			m_pfnNew = &DecNewLHAEngine;
			m_pfnDelete = &DecDeleteLHAEngine;
			break;
//        case DEC_TYPE_LN:
//            m_pfnNew = &DecNewLNEngine;
//            m_pfnDelete = &DecDeleteLNEngine;
//            break;
		case DEC_TYPE_LZ:
			m_pfnNew = &DecNewLZEngine;
			m_pfnDelete = &DecDeleteLZEngine;
			break;
		case DEC_TYPE_MB3:
			m_pfnNew = &DecNewMB3Engine;
			m_pfnDelete = &DecDeleteMB3Engine;
			break;
		case DEC_TYPE_OLE1:
			m_pfnNew = &DecNewOLE1Engine;
			m_pfnDelete = &DecDeleteOLE1Engine;
			break;
		case DEC_TYPE_OLESS:
			m_pfnNew = &DecNewSSEngine;
			m_pfnDelete = &DecDeleteSSEngine;
			break;
//        case DEC_TYPE_PDF:
//            m_pfnNew = &DecNewPDFEngine;
//            m_pfnDelete = &DecDeletePDFEngine;
//            break;
		case DEC_TYPE_RAR:
			m_pfnNew = &DecNewRAREngine;
			m_pfnDelete = &DecDeleteRAREngine;
			break;
		case DEC_TYPE_RTF:
			m_pfnNew = &DecNewRTFEngine;
			m_pfnDelete = &DecDeleteRTFEngine;
			break;
		case DEC_TYPE_TAR:
			m_pfnNew = &DecNewTAREngine;
			m_pfnDelete = &DecDeleteTAREngine;
			break;
		case DEC_TYPE_TEXT:
			m_pfnNew = &DecNewTextEngine;
			m_pfnDelete = &DecDeleteTextEngine;
			break;
		case DEC_TYPE_TNEF:
			m_pfnNew = &DecNewTNEFEngine;
			m_pfnDelete = &DecDeleteTNEFEngine;
			break;
		case DEC_TYPE_ZIP:
			m_pfnNew = &DecNewZipEngine;
			m_pfnDelete = &DecDeleteZipEngine;
			break;
//        case DEC_TYPE_ZOO:
//            m_pfnNew = &DecNewZOOEngine;
//            m_pfnDelete = &DecDeleteZOOEngine;
//            break;
		default:
			m_pfnNew = NULL;
			m_pfnDelete = NULL;
			break;
	}
#endif

	// Make sure all is well.
	if (m_pfnNew == NULL || m_pfnDelete == NULL)
		return(false);

	// Finally, create the decomposer engine.
	DECRESULT hr = (*m_pfnNew)(&m_pEngine);
	if (DEC_OK != hr )
		return(false);

	return(true);
}


void CDecSDKEngine::UnloadEngine()
{
	// Destroy this engine.
	if (m_pEngine != NULL)
	{
		(*m_pfnDelete)(m_pEngine);
		m_pEngine = NULL;
	}

	// Free the library.
	if (m_hInst)
	{
#if defined(SYM_PLATFORM_HAS_DLLS)
		FreeLibrary(m_hInst);
#elif defined(SYM_PLATFORM_HAS_SHAREDOBJS)
		dlclose(m_hInst);
#endif
	}
}


bool CDecSDKEngine::AddEngine(IDecomposer *pDecomposer)
{
	pDecomposer->AddEngine(m_pEngine);
	return(true);
}
