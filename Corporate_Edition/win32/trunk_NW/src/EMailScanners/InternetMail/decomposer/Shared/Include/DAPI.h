// DAPI.h : Decomposer SDK Interface
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DAPI_H)
#define DAPI_H

#include <stdlib.h>
#include <stdio.h>

#if defined(_WINDOWS)
	#include <io.h>
	#include <errno.h>
	#include <sys/utime.h>
	#include <process.h>
#else
	#include <utime.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


/////////////////////////////////////////////////////////////////////////////
// Standard C++ Library Headers

#include "Dec2.h"

#ifdef __cplusplus
	#define DECSDKLINKAGE extern "C"
#else
	#define DECSDKLINKAGE
#endif

#if defined(_WINDOWS)
	#define STDCALL __stdcall
#else
	#define STDCALL 
#endif

#include "DecIO.h"
#include "DecStrm.h"
#include "DecUnicode.h"

#define DAPI_OK					0
#define DAPI_ERROR				-1
#define DAPI_INCORRECT_USAGE	-2
#define DAPI_INVALID_PARAMS		-3
#define DAPI_OUT_OF_MEMORY		-4

// Maximum number of Decomposer engines that we can load into
// the Decomposer SDK.
#define MAX_DEC_ENGINES			30

// Structure for LoadConfig/SaveConfig
typedef struct tagDecConfig
{
	char	*pszEnginePath;
	char	**ppszEngineFilenames;
	int		*piEngineIDs;
	int		iNumEngines;
	unsigned long int ulFileSystemSize;
	unsigned long int ulFileSystemBlockSize;
	unsigned long int ulFileSizeThreshold;
	size_t	nOptionID[DEC_OPTION_COUNT];
	size_t	nOptionValue[DEC_OPTION_COUNT];
	void	*pvReserved;
} DECCONFIG, *PDECCONFIG;

typedef struct tagDecConfigW
{
	wchar_t *pszEnginePath;
	wchar_t **ppszEngineFilenames;
	int		*piEngineIDs;
	int		iNumEngines;
	unsigned long int ulFileSystemSize;
	unsigned long int ulFileSystemBlockSize;
	unsigned long int ulFileSizeThreshold;
	size_t	nOptionID[DEC_OPTION_COUNT];
	size_t	nOptionValue[DEC_OPTION_COUNT];
	void	*pvReserved;
} DECCONFIG_W, *PDECCONFIG_W;


/////////////////////////////////////////////////////////////////////////////
// Interface IDAPI

class IDAPI
{
public:
	IDAPI() {}
	virtual ~IDAPI() {}

	// IDAPI methods:
	virtual int	LoadConfig(const char *pszConfigFile, DECCONFIG **ppConfig) = 0;
	virtual int	LoadConfigW(const wchar_t *pszConfigFile, DECCONFIG_W **ppConfig) = 0;

	virtual int	SaveConfig(const char *pszConfigFile, DECCONFIG *pConfig) = 0;
	virtual int	SaveConfigW(const wchar_t *pszConfigFile, DECCONFIG_W *pConfig) = 0;

	virtual void FreeConfig(DECCONFIG *pConfig) = 0;
	virtual void FreeConfigW(DECCONFIG_W *pConfig) = 0;

	//  This function loads and links to the specified Decomposer engines.
	//  pszEnginePath[IN] - on-disk location where all of the Decomposer engines can be found.
	//  pszEngineFilenames[IN] - array of on-disk engine filenames that the client wants loaded.
	//      A facility in the SDK will be provided to read the engine filenames from a configuration file of some sort.
	//  piEngineIDs[IN] - array of DEC_TYPE_xxx values corresponding to the array of filenames.
	//  iNumEngines[IN]  - number of engines in the psEngineFilenames array.
	//  pDecIO[IN] - client's I/O callback handler (usually allocated by DAPI_NewIOService).  If NULL, a default handler is provided.
	virtual int	StartDecomposer(char *pszEnginePath,
								char **ppszEngineFilenames,
								int *piEngineIDs,
								int iNumEngines,
								IDecomposer **ppDecomposer) = 0;
	virtual int	StartDecomposerW(wchar_t *pszEnginePath,
								 wchar_t **ppszEngineFilenames,
								 int *piEngineIDs,
								 int iNumEngines,
								 IDecomposer **ppDecomposer) = 0;

	//  This function unloads any Decomposer engines that have been loaded.
	virtual void EndDecomposer() = 0;

	virtual int	NewObject(const char *szName, unsigned long int dwCharset, const char *szDataFile, IDecObject **ppObject) = 0;
	virtual int	DeleteObject(IDecObject *pObject) = 0;

	// Unicode support functions for the IDecObject interface.
	virtual int	GetUnicodeName(IDecObject *pObject, wchar_t *pszName, DWORD *pdwNameSize, DWORD *pdwCharset) = 0;
	virtual int	GetUnicodeDataFile(IDecObject *pObject, wchar_t *pszDataFile, DWORD *pdwNameSize) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Global Functions

DECSDKLINKAGE int STDCALL DAPI_NewDecomposer(IDAPI **ppDecomposer);
DECSDKLINKAGE int STDCALL DAPI_DeleteDecomposer(IDAPI *pDecomposer);
DECSDKLINKAGE int STDCALL DAPI_NewIOService(IDecIO **ppDecIO);
DECSDKLINKAGE int STDCALL DAPI_DeleteIOService(IDecIO *pDecIO);
DECSDKLINKAGE int STDCALL DAPI_NewFileSystem(IDecFiles **ppDecFS);
DECSDKLINKAGE int STDCALL DAPI_DeleteFileSystem(IDecFiles *pDecFS);
DECSDKLINKAGE int STDCALL DAPI_NewNetService(INetData **ppDecNet);
DECSDKLINKAGE int STDCALL DAPI_DeleteNetService(INetData *pDecNet);
DECSDKLINKAGE void STDCALL DAPI_GetVersion(const char **ppszVersion);
DECSDKLINKAGE void STDCALL DeleteUnicodeMappingObject(CUNIMO *pObjectToDelete);
DECSDKLINKAGE void STDCALL EndUnicodeMapping(void);
DECSDKLINKAGE CUNIMO * STDCALL GetUnicodeMappingObject(const char *pszMBCSName);
DECSDKLINKAGE CUNIMO * STDCALL GetUnicodeMappingObjectW(const wchar_t *pszUnicodeName);
DECSDKLINKAGE void STDCALL InitializeUnicodeMapping(void);
DECSDKLINKAGE CUNIMO * STDCALL NewUnicodeMappingObject(const wchar_t *pUnicodeName);

// 
// Typedefs for DecSDK functions
// 
typedef int (STDCALL *PFNDELETEDECOMPOSER)(IDAPI *pDecomposer);     // DAPI_DeleteDecomposer
typedef int (STDCALL *PFNDELETEFILESYSTEM)(IDecFiles *pDecFS);      // DAPI_DeleteFileSystem
typedef int (STDCALL *PFNDELETEIOSERVICE)(IDecIO *pDecIO);          // DAPI_DeleteIOService
typedef int (STDCALL *PFNDELETENETSERVICE)(INetData *pDecNet);      // DAPI_DeleteNetService
typedef void (STDCALL *PFNGETVERSION)(const char **ppszVersion);    // DAPI_GetVersion
typedef int (STDCALL *PFNNEWDECOMPOSER)(IDAPI **ppDecomposer);      // DAPI_NewDecomposer
typedef int (STDCALL *PFNNEWFILESYSTEM)(IDecFiles **ppDecFS);       // DAPI_NewFileSystem
typedef int (STDCALL *PFNNEWIOSERVICE)(IDecIO **ppDecIO);           // DAPI_NewIOService
typedef int (STDCALL *PFNNEWNETSERVICE)(INetData **ppDecNet);       // DAPI_NewNetService
typedef void (STDCALL *PFNDELETEUNICODEMAPPINGOBJECT)(CUNIMO *pObjectToDelete);         // DeleteUnicodeMappingObject
typedef void (STDCALL *PFNENDUNICODEMAPPING)(void);                                     // EndUnicodeMapping
typedef CUNIMO *(STDCALL *PFNGETUNICODEMAPPINGOBJECT)(const char *pszMBCSName);         // GetUnicodeMappingObject
typedef CUNIMO *(STDCALL *PFNGETUNICODEMAPPINGOBJECTW)(const wchar_t *pszUnicodeName);  // GetUnicodeMappingObjectW
typedef void (STDCALL *PFNINITUNICODEMAPPING)(void);                                    // InitializeUnicodeMapping                
typedef CUNIMO *(STDCALL *PFNNEWUNICODEMAPPINGOBJECT)(const wchar_t *pUnicodeName);     // NewUnicodeMappingObject

#endif	// DAPI_H
