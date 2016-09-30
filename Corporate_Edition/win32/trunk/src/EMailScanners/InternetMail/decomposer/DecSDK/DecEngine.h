// DecEngine.h: General-purpose Decomposer engine loader/unloader.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 by Symantec Corporation.  All rights reserved.

//////////////////////////////////////////////////////////////////////
//
// CDecSDKEngine - represents a single decomposer engine.  Handles
// loading the library and allocating the engine.
//
//////////////////////////////////////////////////////////////////////

//
// Typedefs for engine creation and destruction
//
#if defined(__cplusplus)
extern "C" {
#endif

typedef DECRESULT (*PFNDECCREATEENGINE)(IDecEngine** ppEngine);
typedef DECRESULT (*PFNDECDESTROYENGINE)(IDecEngine* ppEngine);
typedef DECRESULT (*PFNDECGETENGINEVERSION)(const char *pszVersion);        // Not currently implemented

#if defined(__cplusplus)
}
#endif

class CDecSDKEngine
{
public:
	CDecSDKEngine();
	virtual ~CDecSDKEngine();

	bool LoadEngine(const char	*pszPath,
					const char	*pszFilename,
					int			iEngineID,
					const char	*pszNewFunc,
					const char	*pszDeleteFunc);
	bool LoadEngineW(const wchar_t	*pszPath,
					const wchar_t	*pszFilename,
					int			iEngineID,
					const char		*pszNewFunc,
					const char		*pszDeleteFunc);
	void UnloadEngine();
	bool AddEngine(IDecomposer *pDecomposer);

	operator IDecEngine* () { return m_pEngine; }
//    operator == ( CDecSDKEngine& other ) { return m_pfnNew == other.m_pfnNew; }

private:
	bool LinkEngine(int			iEngineID,
					const char	*pszNewFunc,
					const char	*pszDeleteFunc);

	// The engine object itself
	IDecEngine* m_pEngine;

	// Function pointers to exports from this decomposer engine.
	PFNDECCREATEENGINE		m_pfnNew;
	PFNDECDESTROYENGINE		m_pfnDelete;
	PFNDECGETENGINEVERSION	m_pfnGetVersion;

#if defined(SYM_PLATFORM_HAS_DLLS)
	// Instance handle for Windows DLLs.
	HINSTANCE m_hInst;
#else
	// Instance handle for UNIX shared objects.  Declared for all UNIX platforms,
	// but only used where applicable.
	void *m_hInst;
#endif
};
