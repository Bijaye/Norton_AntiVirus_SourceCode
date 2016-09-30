// DAPI.cpp : Decomposer SDK API
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 by Symantec Corporation.  All rights reserved.

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "dec_assert.h"

#define DAPI_CPP
#include "DecPlat.h"
#include "DAPI.h"
#include "Dec2Ver.h"
#include "DecEngine.h"

#include "SymSaferStrings.h"

#define MAX_CONFIG_LINE_LENGTH	1024

#if defined(OS400)

#define IO_MODE_READ_TEXT					"r,ccsid=37"
#define IO_MODE_TRUNCATE_READ_WRITE_TEXT	"w+,ccsid=37"

#else

#define IO_MODE_READ_TEXT					"r"
#define IO_MODE_TRUNCATE_READ_WRITE_TEXT	"w+"

#if defined(_WINDOWS)

#define IO_MODE_READ_TEXT_W					L"r"
#define IO_MODE_TRUNCATE_READ_WRITE_TEXT_W	L"w+"

#endif

#endif

const char gszDecSDKVersion[] = VERSION_STRING;

typedef struct tagConfigOption
{
	char	*pszName;
	size_t	nOptionID;
} CONFIGOPTION, *PCONFIGOPTION;

CONFIGOPTION g_Option[] =
{
	"EnforceCRC",					DEC_OPTION_ENFORCE_CRC,
	"ExtractRTFHTML",				DEC_OPTION_EXTRACT_RTFHTML,
	"ExtractOLE10NativeOnly",		DEC_OPTION_EXTRACT_OLE10NATIVE_ONLY,
	"NonMIMEThreshold",				DEC_OPTION_NONMIMETHRESHOLD,
	"EnableMIMEEngine",				DEC_OPTION_ENABLE_MIME_ENGINE,
	"EnableUUEEngine",				DEC_OPTION_ENABLE_UUE_ENGINE,
	"EnableBinHexEngine",			DEC_OPTION_ENABLE_BINHEX_ENGINE,
	"EnableMBOXEngine",				DEC_OPTION_ENABLE_MBOX_ENGINE,
	"LogWarnings",					DEC_OPTION_LOG_WARNINGS,
	"MaxTextScanBytes",				DEC_OPTION_MAX_TEXT_SCAN_BYTES,
	"EventSinkVersion",				DEC_OPTION_EVENTSINK_VERSION,
	"MIMEFuzzyMainHeader",			DEC_OPTION_MIME_FUZZY_MAIN_HEADER,
	"EnhancedTextID",				DEC_OPTION_ENHANCED_TEXT_ID,
	"TranslateNULL",				DEC_OPTION_TRANSLATE_NULL,
	"NonHQXThreshold",				DEC_OPTION_NONHQXTHRESHOLD,
	"NonUUEThresholdBytes",			DEC_OPTION_NONUUETHRESHOLD_BYTES,
	"MinMIMEScanBytes",				DEC_OPTION_MIN_MIME_SCAN_BYTES,
	"MIMEIdentificationStrength",	DEC_OPTION_MIME_IDENTIFICATION_STRENGTH,
	"UseOLE10NativeChildName",		DEC_OPTION_USE_OLE10NATIVE_CHILD_NAME,
	"EventSinkTrace",				DEC_OPTION_EVENTSINK_TRACE,
//	"TypeIdentification",			DEC_OPTION_TYPE_IDENTIFICATION,
	"",								0
};


#ifdef SYM_PLATFORM_HAS_SHAREDOBJS
#include <dlfcn.h>

//
// Function pointers from Dec2 - used when using DecSDK as a shared object
//
typedef DECRESULT (*PFNDECNEWDECOMPOSER)(IDecomposer **ppDecomposer);          // DecNewDecomposer
typedef DECRESULT (*PFNDECDELETEDECOMPOSER)(IDecomposer *pDecomposer);         // DecDeleteDecomposer
typedef DECRESULT (*PFNDECNEWFILEOBJECT)(const char *szName,                   // DecNewFileObject 
										DWORD dwCharset, 
										const char *szDataFile, 
										IDecObject **ppObject);
typedef DECRESULT (*PFNDECDELETEFILEOBJECT)(IDecObject *pObject);              // DecDeleteFileObject
typedef void (*PFNDECGETVERSION)(const char **ppszVersion);                    // DecGetVersion
#endif

class CDAPI :
	public IDAPI
{
public:
	CDAPI();
	~CDAPI();

	// IDAPI methods:
	int	LoadConfig(const char *pszConfigFile, DECCONFIG **ppConfig);
	int	LoadConfigW(const wchar_t *pszConfigFile, DECCONFIG_W **ppConfig);

	int	SaveConfig(const char *pszConfigFile, DECCONFIG *pConfig);
	int	SaveConfigW(const wchar_t *pszConfigFile, DECCONFIG_W *pConfig);

	void FreeConfig(DECCONFIG *pConfig);
	void FreeConfigW(DECCONFIG_W *pConfig);

	int	StartDecomposer(char *pszEnginePath,
						char **ppszEngineFilenames,
						int *piEngineIDs,
						int iNumEngines,
						IDecomposer **ppDecomposer);
	int	StartDecomposerW(wchar_t *pszEnginePath,
						 wchar_t **ppszEngineFilenames,
						 int *piEngineIDs,
						 int iNumEngines,
						 IDecomposer **ppDecomposer);
	void EndDecomposer();
	int	NewObject(const char *szName, unsigned long int dwCharset, const char *szDataFile, IDecObject **ppObject);
	int	DeleteObject(IDecObject *pObject);

	int	GetUnicodeName(IDecObject *pObject, wchar_t *pszName,  DWORD *pdwNameSize, DWORD *pdwCharset);
	int	GetUnicodeDataFile(IDecObject *pObject, wchar_t *pszDataFile, DWORD *pdwNameSize);

private:
	void LoadConfig2(FILE *fp, DECCONFIG *pcfg, char *pszLine);
	void LoadOption(char *pszLine, DECCONFIG *pcfg);
	bool LoadEngines(char *pszEnginePath, char **ppszEngineFilenames, int *piEngineIDs, int iNumEngines);
	bool LoadEnginesW(wchar_t *pszEnginePath, wchar_t **ppszEngineFilenames, int *piEngineIDs, int iNumEngines);
	void UnloadEngines();
	void StripNewline(char *pszLine);
	void StripNewlineW(wchar_t *pszLine);

	IDecomposer		*m_pDecomposer;
	CDecSDKEngine	*m_pEngine[MAX_DEC_ENGINES];
	size_t			m_nOptionIndex;

#ifdef SYM_PLATFORM_HAS_SHAREDOBJS
	//
	// Function pointers to exports from Dec2
	//
	PFNDECNEWDECOMPOSER m_pfnDecNewDecomposer;
	PFNDECDELETEDECOMPOSER m_pfnDecDeleteDecomposer;
	PFNDECNEWFILEOBJECT m_pfnDecNewFileObject;
	PFNDECDELETEFILEOBJECT m_pfnDecDeleteFileObject;
	PFNDECGETVERSION m_pfnDecGetVersion;

	//
	// Pointer to Dec2 shared obj
	//
	void *m_hInst;

	//
	// Loading and cleanup functions for Dec2 functions
	//
	bool InitializeDec2SharedObj(const char *pszDec2Path);
	bool UnloadDec2SharedObj();
#endif
};


// Decomposer API constructor
CDAPI::CDAPI()
{
	m_pDecomposer = NULL;

	for (int index = 0; index < MAX_DEC_ENGINES; index++)
		m_pEngine[index] = NULL;

#ifdef SYM_PLATFORM_HAS_SHAREDOBJS
	m_pfnDecNewDecomposer = NULL;
	m_pfnDecDeleteDecomposer = NULL;
	m_pfnDecNewFileObject = NULL;
	m_pfnDecDeleteFileObject = NULL;
	m_pfnDecGetVersion = NULL;
	m_hInst = NULL;
#endif
}


// Decomposer API destructor
CDAPI::~CDAPI()
{
	EndDecomposer();
}


/////////////////////////////////////////////////////////////////////////
// CDAPI::LoadConfig()
//
// The configuration file is a text file formatted as follows:
// Line #   Description
// ------   -----------
// 1        MBCS path to engine binary files.
// 2        Size of in-memory file system (in bytes).
// 3        Size of in-memory file system data blocks (in bytes).
// 4        Maximum size of an in-memory file (in bytes).
// 5        Integer number of engine descriptions that follow.
// 6 (2n+6) MBCS filename of engine binary file.
// 7 (2n+7) Integer engine type identifier (from the DEC_TYPE_xxx enumeration).
//
// Additional configuration information is specified in blocks of
// name=value pairs.  The configuration blocks begin with a line
// containing a special keyword enclosed in brackets that identifies
// the type of configuration information.
// [Options]
// EnforceCRC={0, 1}                maps to DEC_OPTION_ENFORCE_CRC
// ExtractRTFHTML={0, 1}            maps to DEC_OPTION_EXTRACT_RTFHTML
// ExtractOLE10NativeOnly={0, 1}    maps to DEC_OPTION_EXTRACT_OLE10NATIVE_ONLY
// NonMIMEThreshold={0, n}          maps to DEC_OPTION_NONMIMETHRESHOLD
// NonUUEThreshold={0, n}           maps to DEC_OPTION_NONUUETHRESHOLD
// NonUUEThresholdBytes={0, n}      maps to DEC_OPTION_NONUUETHRESHOLD_BYTES
// EnableMIMEEngine={0, 1}          maps to DEC_OPTION_ENABLE_MIME_ENGINE
// EnableUUEEngine={0, 1}           maps to DEC_OPTION_ENABLE_UUE_ENGINE
// EnableBinHexEngine={0, 1}        maps to DEC_OPTION_ENABLE_BINHEX_ENGINE
// EnableMBOXEngine={0, 1}          maps to DEC_OPTION_ENABLE_MBOX_ENGINE
// LogWarnings={0, 1}               maps to DEC_OPTION_LOG_WARNINGS
// UseOLE10NativeChildName={0, 1}    maps to DEC_OPTION_USE_OLE10NATIVE_CHILD_NAME

//

int	CDAPI::LoadConfig(const char *pszConfigFile, DECCONFIG **ppConfig)
{
	DECCONFIG	*pcfg = NULL;
	FILE		*fp = NULL;
	char		*pszLine = NULL;
	int			iCount;
	int			index;
	unsigned long int ulValue;
	bool		bError = false;

	m_nOptionIndex = 0;

	if (!pszConfigFile || !ppConfig)
		return DAPI_INVALID_PARAMS;

	pcfg = new DECCONFIG;
	if (!pcfg)
		return DAPI_OUT_OF_MEMORY;

	// Zero out all of the DECCONFIG fields.
	memset(pcfg, 0, sizeof(DECCONFIG));

	pszLine = new char[MAX_CONFIG_LINE_LENGTH + 1];
	if (!pszLine)
		goto loadconfig_error;

	fp = fopen(pszConfigFile, IO_MODE_READ_TEXT);
	if (!fp)
		goto loadconfig_error;

	pcfg->pszEnginePath = new char[MAX_CONFIG_LINE_LENGTH + 1];
	if (!pcfg->pszEnginePath)
		goto loadconfig_error;

	// Get the first line - the engine path.
	memset(pcfg->pszEnginePath, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgets(pcfg->pszEnginePath, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfig_error;

	StripNewline(pcfg->pszEnginePath);

	// Get the second line - the size of the in-memory file system (in bytes).
	memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgets(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfig_error;

	ulValue = strtoul(pszLine, NULL, 10);

	// Limit file system size to range from 4K to 2GB.
	if (ulValue > 0 && ulValue < 4 * 1024)
		ulValue = 4 * 1024;
	if (ulValue > (unsigned long int)2 * 1024 * 1024 * 1024)
		ulValue = (unsigned long int)2 * 1024 * 1024 * 1024;
	pcfg->ulFileSystemSize = ulValue;

	// Get the third line - the size of the in-memory file system's data blocks (in bytes).
	memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgets(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfig_error;

	// File system block size is an obsolete parameter.
	pcfg->ulFileSystemBlockSize = 0;

	// Get the fourth line - the file size threshold (in bytes).
	memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgets(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfig_error;

	ulValue = strtoul(pszLine, NULL, 10);

	// The file size threshold can not be larger than the total size of
	// the in-memory file system.
	if (ulValue > pcfg->ulFileSystemSize)
		ulValue = pcfg->ulFileSystemSize;
	pcfg->ulFileSizeThreshold = ulValue;

	// Get the fifth line - the engine count.
	memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgets(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfig_error;

	iCount = atoi(pszLine);

	// Make sure the engine count is in a reasonable range.
	if (iCount <= 0 || iCount > 50)
		goto loadconfig_error;

	// Allocate the two arrays (pszEngineFilenames and piEngineIDs).
	pcfg->ppszEngineFilenames = new char *[iCount];
	if (!pcfg->ppszEngineFilenames)
		goto loadconfig_error;

	pcfg->piEngineIDs = new int[iCount];
	if (!pcfg->piEngineIDs)
		goto loadconfig_error;

	// Loop through the engine filename and ID pairs.
	index = 0;
	while (iCount)
	{
		// Get the engine filename.
		memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
		if (!fgets(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		{
			bError = true;
			break;
		}

		StripNewline(pszLine);

		// Save off the filename.
		*(pcfg->ppszEngineFilenames + index) = pszLine;

		// Create a new line.
		pszLine = new char[MAX_CONFIG_LINE_LENGTH + 1];
		if (!pszLine)
		{
			bError = true;
			break;
		}

		// Get the engine ID.
		memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
		if (!fgets(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		{
			bError = true;
			break;
		}

		pcfg->piEngineIDs[index] = atoi(pszLine);

		iCount--;
		index++;
		pcfg->iNumEngines++;
	}

	// Read any configuration options that may be present.
	LoadConfig2(fp, pcfg, pszLine);

	if (bError)
		goto loadconfig_error;

	if (pszLine)
	{
		delete [] pszLine;
		pszLine = NULL;
	}
	
	if (EOF == fclose(fp))
	{
		fp = NULL;
		goto loadconfig_error;
	}

	// Return the DECCONFIG that we just created.
	// The caller is now responsible for destroying the memory
	// allocated for this structure.
	*ppConfig = pcfg;
	return DAPI_OK;

loadconfig_error:
	if (pszLine)
		delete [] pszLine;
	
	// Don't need to check this fclose since we're already in an
	// error state here.
	if (fp)
		fclose(fp);

	FreeConfig(pcfg);

	return DAPI_ERROR;
}


#define STATE_GET_KEYWORD	  0
#define STATE_GET_OPTIONS	  1

void CDAPI::LoadConfig2(FILE *fp, DECCONFIG *pcfg, char *pszLine)
{
	int		nState = STATE_GET_KEYWORD;

	// Read the rest of the configuration file looking for special
	// keywords that start configuration blocks.
	while (!feof(fp))
	{
		// Get the next line.
		memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
		if (!fgets(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
			return;

		if (pszLine[0] == '\r' || pszLine[0] == '\n')
			nState = STATE_GET_KEYWORD;

		// See if this is the beginning of the Options block.
		if (pszLine[0] == '[' &&
			(pszLine[1] == 'O' || pszLine[1] == 'o') &&
			(pszLine[2] == 'P' || pszLine[2] == 'p') &&
			(pszLine[3] == 'T' || pszLine[3] == 't') &&
			(pszLine[4] == 'I' || pszLine[4] == 'i') &&
			(pszLine[5] == 'O' || pszLine[5] == 'o') &&
			(pszLine[6] == 'N' || pszLine[6] == 'n') &&
			(pszLine[7] == 'S' || pszLine[7] == 's') &&
			pszLine[8] == ']')
		{
			nState = STATE_GET_OPTIONS;
			continue;
		}

		switch (nState)
		{
			case STATE_GET_OPTIONS:
				LoadOption(pszLine, pcfg);
				break;

			default:
				break;
		}
	}
}


void CDAPI::LoadOption(char *pszLine, DECCONFIG *pcfg)
{
	char	*ptr;
	size_t	nLength;
	int		index = 0;
	long	lValue;

	if (m_nOptionIndex >= DEC_OPTION_COUNT)
		return;

	ptr = g_Option[index].pszName;
	nLength = strlen(ptr);
	while (nLength)
	{
		if (!strncmp(pszLine, ptr, nLength))
		{
			// Scan for the equal sign.
			ptr = pszLine;
			while (*ptr)
			{
				if (*ptr == '=')
					break;

				ptr++;
			}

			if (*ptr == '=')
			{
				ptr++;

				// ptr is now pointing to the value string.
				lValue = atol(ptr);
				pcfg->nOptionID[m_nOptionIndex] = g_Option[index].nOptionID;
				pcfg->nOptionValue[m_nOptionIndex] = (unsigned long)lValue;
				m_nOptionIndex++;
				break;
			}
		}

		// Try the next string.
		index++;
		ptr = g_Option[index].pszName;
		nLength = strlen(ptr);
	}
}


/////////////////////////////////////////////////////////////////////////
// CDAPI::LoadConfigW()
//
// The configuration file is a text file formatted as follows:
// Line #   Description
// ------   -----------
// 1        Unicode path to engine binary files.
// 2        Size of in-memory file system (in bytes).
// 3        Size of in-memory file system data blocks (in bytes).
// 4        Maximum size of an in-memory file (in bytes).
// 5        Integer number of engine descriptions that follow.
// 6 (2n+6) Unicode filename of engine binary file.
// 7 (2n+7) Integer engine type identifier (from the DEC_TYPE_xxx enumeration).
// last     Signature, checksum, or some such???  For now, always 0.
//

int	CDAPI::LoadConfigW(const wchar_t *pszConfigFile, DECCONFIG_W **ppConfig)
{
#if defined(_WINDOWS)
	FILE		*fp = NULL;
	DECCONFIG_W *pcfg = NULL;
	wchar_t		*pszLine = NULL;
	int			iCount;
	int			index;
	unsigned long int ulValue;
	bool		bError = false;

	if (!pszConfigFile || !ppConfig)
		return DAPI_INVALID_PARAMS;

	pcfg = new DECCONFIG_W;
	if (!pcfg)
		return DAPI_OUT_OF_MEMORY;

	// Zero out all of the DECCONFIG_W fields.
	memset(pcfg, 0, sizeof(DECCONFIG_W));

	pszLine = new wchar_t[MAX_CONFIG_LINE_LENGTH + 1];
	if (!pszLine)
		goto loadconfigw_error;

	fp = _wfopen(pszConfigFile, IO_MODE_READ_TEXT_W);
	if (!fp)
		goto loadconfigw_error;

	pcfg->pszEnginePath = new wchar_t[MAX_CONFIG_LINE_LENGTH + 1];
	if (!pcfg->pszEnginePath)
		goto loadconfigw_error;

	// Get the first line - the engine path.
	memset(pcfg->pszEnginePath, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgetws(pcfg->pszEnginePath, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfigw_error;

	StripNewlineW(pcfg->pszEnginePath);

	// Get the second line - the size of the in-memory file system (in bytes).
	memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgetws(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfigw_error;

	ulValue = wcstoul(pszLine, NULL, 10);

	// Limit file system size to range from 4K to 2GB.
	if (ulValue > 0 && ulValue < 4 * 1024)
		ulValue = 4 * 1024;
	if (ulValue > (unsigned long int)2 * 1024 * 1024 * 1024)
		ulValue = (unsigned long int)2 * 1024 * 1024 * 1024;
	pcfg->ulFileSystemSize = ulValue;

	// Get the third line - the size of the in-memory file system's data blocks (in bytes).
	memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgetws(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfigw_error;

	// File system block size is an obsolete parameter.
	pcfg->ulFileSystemBlockSize = 0;

	// Get the fourth line - the file size threshold (in bytes).
	memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgetws(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfigw_error;

	ulValue = wcstoul(pszLine, NULL, 10);

	// The file size threshold can not be larger than the total size of
	// the in-memory file system.
	if (ulValue > pcfg->ulFileSystemSize)
		ulValue = pcfg->ulFileSystemSize;
	pcfg->ulFileSizeThreshold = ulValue;

	// Get the fifth line - the engine count.
	memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
	if (!fgetws(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		goto loadconfigw_error;

	iCount = _wtoi(pszLine);

	// Make sure the engine count is in a reasonable range.
	if (iCount <= 0 || iCount > 50)
		goto loadconfigw_error;

	// Allocate the two arrays (pszEngineFilenames and piEngineIDs).
	pcfg->ppszEngineFilenames = new wchar_t *[iCount];
	if (!pcfg->ppszEngineFilenames)
		goto loadconfigw_error;

	pcfg->piEngineIDs = new int[iCount];
	if (!pcfg->piEngineIDs)
		goto loadconfigw_error;

	// Loop through the engine filename and ID pairs.
	index = 0;
	while (iCount)
	{
		// Get the engine filename.
		memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
		if (!fgetws(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		{
			bError = true;
			break;
		}

		StripNewlineW(pszLine);

		// Save off the filename.
		*(pcfg->ppszEngineFilenames + index) = pszLine;

		// Create a new line.
		pszLine = new wchar_t[MAX_CONFIG_LINE_LENGTH + 1];
		if (!pszLine)
		{
			bError = true;
			break;
		}

		// Get the engine ID.
		memset(pszLine, 0, MAX_CONFIG_LINE_LENGTH + 1);
		if (!fgetws(pszLine, MAX_CONFIG_LINE_LENGTH, fp))
		{
			bError = true;
			break;
		}

		pcfg->piEngineIDs[index] = _wtoi(pszLine);

		iCount--;
		index++;
		pcfg->iNumEngines++;
	}

	if (bError)
		goto loadconfigw_error;

	if(pszLine)
	{
		delete [] pszLine;
		pszLine = NULL;
	}

	if(EOF == fclose(fp))
	{
		fp = NULL;
		goto loadconfigw_error;
	}

	// Return the DECCONFIG that we just created.
	// The caller is now responsible for destroying the memory
	// allocated for this structure.
	*ppConfig = pcfg;
	return DAPI_OK;

loadconfigw_error:
	if (pszLine)
		delete [] pszLine;
   
	// Don't need to check this fclose since we're already in an
	// error state here.
	if (fp)
		fclose(fp);

	FreeConfigW(pcfg);

	return DAPI_ERROR;
#else
	return DAPI_ERROR;
#endif
}


/////////////////////////////////////////////////////////////////////////
// CDAPI::SaveConfig()
//
// See LoadConfig above for the configuration file's format.
//

int	CDAPI::SaveConfig(const char *pszConfigFile, DECCONFIG *pConfig)
{
	FILE		*fp = NULL;
	char		*pszLine = NULL;
	char		*ptr;
	int			iCount;
	int			index;
	bool		bError = false;

	if (!pszConfigFile || !pConfig)
		return DAPI_INVALID_PARAMS;

	pszLine = new char[MAX_CONFIG_LINE_LENGTH + 1];
	if (!pszLine)
		goto saveconfig_error;

	fp = fopen(pszConfigFile, IO_MODE_TRUNCATE_READ_WRITE_TEXT);
	if (!fp)
		goto saveconfig_error;

	// Write the first line - the engine path.
	if (fputs(pConfig->pszEnginePath, fp) == EOF ||
		fputs("\n", fp) == EOF)
		goto saveconfig_error;

	// Write the second line - the size of the in-memory file system (in bytes).
	sssnprintf(pszLine, MAX_CONFIG_LINE_LENGTH + 1, "%u", pConfig->ulFileSystemSize);
	if (fputs(pszLine, fp) == EOF ||
		fputs("\n", fp) == EOF)
		goto saveconfig_error;

	// Write the third line - the size of the in-memory file system's data blocks (in bytes).
	sssnprintf(pszLine, MAX_CONFIG_LINE_LENGTH + 1, "%u", pConfig->ulFileSystemBlockSize);
	if (fputs(pszLine, fp) == EOF ||
		fputs("\n", fp) == EOF)
		goto saveconfig_error;

	// Write the fourth line - the file size threshold (in bytes).
	sssnprintf(pszLine, MAX_CONFIG_LINE_LENGTH + 1, "%u", pConfig->ulFileSizeThreshold);
	if (fputs(pszLine, fp) == EOF ||
		fputs("\n", fp) == EOF)
		goto saveconfig_error;

	// Write the fifth line - the engine count.
	sssnprintf(pszLine, MAX_CONFIG_LINE_LENGTH + 1, "%i", pConfig->iNumEngines);
	if (fputs(pszLine, fp) == EOF ||
		fputs("\n", fp) == EOF)
		goto saveconfig_error;

	iCount = pConfig->iNumEngines;

	// Loop through the engine filename and ID pairs, writing them as we go...
	index = 0;
	while (iCount)
	{
		// Write the engine filename.
		strcpy(pszLine, pConfig->ppszEngineFilenames[index]);
		if (fputs(pszLine, fp) == EOF ||
			fputs("\n", fp) == EOF)
		{
			bError = true;
			break;
		}

		// Write the engine ID.
		sssnprintf(pszLine, MAX_CONFIG_LINE_LENGTH + 1, "%i", pConfig->piEngineIDs[index]);
		if (fputs(pszLine, fp) == EOF ||
			fputs("\n", fp) == EOF)
		{
			bError = true;
			break;
		}

		iCount--;
		index++;
	}

	// Now write all the options.
	// Write the engine ID.
	if (fputs("\n", fp) == EOF ||
		fputs("[Options]\n", fp) == EOF)
	{
		bError = true;
	}
	else
	{
		for (index = 0; index < DEC_OPTION_COUNT; index++)
		{
			int		i;
			size_t	nOptionID;

			// Get the option ID from the nOptionID array.
			nOptionID = pConfig->nOptionID[index];
			if (nOptionID)
			{
				// Locate a match in our list.
				ptr = NULL;
				i = 0;
				while (true)
				{
					if (g_Option[i].nOptionID == nOptionID)
					{
						ptr = g_Option[i].pszName;
						break;
					}

					if (strlen(g_Option[i].pszName) == 0)
						break;

					i++;
				}

				if (ptr)
				{
					sssnprintf(pszLine, MAX_CONFIG_LINE_LENGTH + 1, "%s=%d", ptr, pConfig->nOptionValue[index]);
					if (fputs(pszLine, fp) == EOF ||
						fputs("\n", fp) == EOF)
					{
						bError = true;
						break;
					}
				}
			}
		}
	}

	if (bError)
		goto saveconfig_error;

	if (pszLine)
	{
		delete [] pszLine;
		pszLine = NULL;
	}

	if(EOF == fclose(fp))
	{
		fp = NULL;
		goto saveconfig_error;
	}

	return DAPI_OK;

saveconfig_error:
	if (pszLine)
		delete [] pszLine;
	
	// Don't need to check this fclose since we're already in an
	// error state here.
	if (fp)
		fclose(fp);

	return DAPI_ERROR;
}


/////////////////////////////////////////////////////////////////////////
// CDAPI::SaveConfigW()
//
// The configuration file is a text file formatted as follows:
// Line #   Description
// ------   -----------
// 1        Unicode path to engine binary files.
// 2        Size of in-memory file system (in bytes).
// 3        Size of in-memory file system data blocks (in bytes).
// 4        Maximum size of an in-memory file (in bytes).
// 5        Integer number of engine descriptions that follow.
// 6 (2n+6) Unicode filename of engine binary file.
// 7 (2n+7) Integer engine type identifier (from the DEC_TYPE_xxx enumeration).
// last     Signature, checksum, or some such???  For now, always 0.
//

int	CDAPI::SaveConfigW(const wchar_t *pszConfigFile, DECCONFIG_W *pConfig)
{
#if defined(_WINDOWS)
	FILE		*fp = NULL;
	wchar_t		*pszLine = NULL;
	int			iCount;
	int			index;
	bool		bError = false;

	if (!pszConfigFile || !pConfig)
		return DAPI_INVALID_PARAMS;

	pszLine = new wchar_t[MAX_CONFIG_LINE_LENGTH + 1];
	if (!pszLine)
		goto saveconfigw_error;

	fp = _wfopen(pszConfigFile, IO_MODE_TRUNCATE_READ_WRITE_TEXT_W);
	if (!fp)
		goto saveconfigw_error;

	// Write the first line - the engine path.
	if (fputws(pConfig->pszEnginePath, fp) == WEOF ||
		fputws(L"\n", fp) == WEOF)
		goto saveconfigw_error;

	// Write the second line - the size of the in-memory file system (in bytes).
	_ltow(pConfig->ulFileSystemSize, pszLine, 10);
	if (fputws(pszLine, fp) == WEOF ||
		fputws(L"\n", fp) == WEOF)
		goto saveconfigw_error;

	// Write the third line - the size of the in-memory file system's data blocks (in bytes).
	_ltow(pConfig->ulFileSystemBlockSize, pszLine, 10);
	if (fputws(pszLine, fp) == WEOF ||
		fputws(L"\n", fp) == WEOF)
		goto saveconfigw_error;

	// Write the fourth line - the file size threshold (in bytes).
	_ltow(pConfig->ulFileSizeThreshold, pszLine, 10);
	if (fputws(pszLine, fp) == WEOF ||
		fputws(L"\n", fp) == WEOF)
		goto saveconfigw_error;

	// Write the fifth line - the engine count.
	_itow(pConfig->iNumEngines, pszLine, 10);
	if (fputws(pszLine, fp) == WEOF ||
		fputws(L"\n", fp) == WEOF)
		goto saveconfigw_error;

	iCount = pConfig->iNumEngines;

	// Loop through the engine filename and ID pairs, writing them as we go...
	index = 0;
	while (iCount)
	{
		// Write the engine filename.
		wcscpy(pszLine, pConfig->ppszEngineFilenames[index]);
		if (fputws(pszLine, fp) == WEOF ||
			fputws(L"\n", fp) == WEOF)
		{
			bError = true;
			break;
		}

		// Write the engine ID.
		_itow(pConfig->piEngineIDs[index], pszLine, 10);
		if (fputws(pszLine, fp) == WEOF ||
			fputws(L"\n", fp) == WEOF)
		{
			bError = true;
			break;
		}

		iCount--;
		index++;
	}

	if (bError)
		goto saveconfigw_error;

	if(pszLine)
	{
		delete [] pszLine;
		pszLine = NULL;
	}
	
	if(EOF == fclose(fp))
	{
		fp = NULL;
		goto saveconfigw_error;
	}

	return DAPI_OK;

saveconfigw_error:
	if (pszLine)
		delete [] pszLine;
	
	// Don't need to check this fclose since we're already in an
	// error state here.
	if (fp)
		fclose(fp);

	return DAPI_ERROR;
#else
	return DAPI_ERROR;
#endif
}


void CDAPI::FreeConfig(DECCONFIG *pConfig)
{
	if (!pConfig)
		return;

	if (pConfig->pszEnginePath)
		delete [] pConfig->pszEnginePath;

	if (pConfig->ppszEngineFilenames)
	{
		int		iCount;
		char	*ptr;

		iCount = pConfig->iNumEngines;
		while (iCount)
		{
			ptr = pConfig->ppszEngineFilenames[iCount - 1];
			if (ptr)
				delete [] ptr;

			iCount--;
		}

		delete [] pConfig->ppszEngineFilenames;
	}

	if (pConfig->piEngineIDs)
		delete [] pConfig->piEngineIDs;

	delete pConfig;
}


void CDAPI::FreeConfigW(DECCONFIG_W *pConfig)
{
	if (!pConfig)
		return;

	if (pConfig->pszEnginePath)
		delete [] pConfig->pszEnginePath;

	if (pConfig->ppszEngineFilenames)
	{
		int		iCount;
		wchar_t *ptr;

		iCount = pConfig->iNumEngines;
		while (iCount)
		{
			ptr = pConfig->ppszEngineFilenames[iCount - 1];
			if (ptr)
				delete [] ptr;

			iCount--;
		}

		delete [] pConfig->ppszEngineFilenames;
	}

	if (pConfig->piEngineIDs)
		delete [] pConfig->piEngineIDs;

	delete pConfig;
}


/////////////////////////////////////////////////////////////////////////
// CDAPI::StartDecomposer()

int	CDAPI::StartDecomposer(char *pszEnginePath,
							char **ppszEngineFilenames,
							int *piEngineIDs,
							int iNumEngines,
							IDecomposer **ppDecomposer)
{
	int		index;

	if (iNumEngines >= MAX_DEC_ENGINES)
		return DAPI_INVALID_PARAMS;

	m_pDecomposer = NULL;

	// Clear the engine pointer array.
	for (index = 0; index < MAX_DEC_ENGINES; index++)
		m_pEngine[index] = NULL;

#ifdef SYM_PLATFORM_HAS_SHAREDOBJS
	//
	// Try and load libdec2.so and get the needed function pointers
	//
	if(!InitializeDec2SharedObj(pszEnginePath))
	{
		return DAPI_ERROR;
	}

	//
	// Try and create a new decomposer instance
	//
	if((*m_pfnDecNewDecomposer)(&m_pDecomposer) != DEC_OK)
	{
		return DAPI_ERROR;
	}
#else
	if (DecNewDecomposer(&m_pDecomposer) != DEC_OK)
	{
		return DAPI_ERROR;
	}
#endif

	*ppDecomposer = m_pDecomposer;

	if (!LoadEngines(pszEnginePath, ppszEngineFilenames, piEngineIDs, iNumEngines))
	{
		// Failed to load all engines successfully.
		return DAPI_ERROR;
	}

	// OK, loaded all of the engines successfully.
	return(DAPI_OK);
}


/////////////////////////////////////////////////////////////////////////
// CDAPI::StartDecomposerW()

int	CDAPI::StartDecomposerW(wchar_t *pszEnginePath,
							wchar_t **ppszEngineFilenames,
							int *piEngineIDs,
							int iNumEngines,
							IDecomposer **ppDecomposer)
{
#ifdef SYM_PLATFORM_HAS_SHAREDOBJS
	//
	// No wide char support under UNIX
	//
	return DAPI_ERROR;
#else
	int		index;

	if (iNumEngines >= MAX_DEC_ENGINES)
		return DAPI_INVALID_PARAMS;

	m_pDecomposer = NULL;

	// Clear the engine pointer array.
	for (index = 0; index < MAX_DEC_ENGINES; index++)
		m_pEngine[index] = NULL;

	if (DecNewDecomposer(&m_pDecomposer) != DEC_OK)
	{
		return DAPI_ERROR;
	}

	*ppDecomposer = m_pDecomposer;

	if (!LoadEnginesW(pszEnginePath, ppszEngineFilenames, piEngineIDs, iNumEngines))
	{
		// Failed to load all engines successfully.
		return DAPI_ERROR;
	}

	// OK, loaded all of the engines successfully.
	return(DAPI_OK);
#endif
}


/////////////////////////////////////////////////////////////////////////
// CDAPI::EndDecomposer()

void CDAPI::EndDecomposer()
{
	if (m_pDecomposer)
	{
		m_pDecomposer->RemoveEngines();

		UnloadEngines();

#ifdef SYM_PLATFORM_HAS_SHAREDOBJS
		//
		// Validate our pointers...
		//
		dec_assert(m_pfnDecDeleteDecomposer);

		(*m_pfnDecDeleteDecomposer)(m_pDecomposer);

		//
		// Release libdec2.so
		//
		UnloadDec2SharedObj();
#else
		DecDeleteDecomposer(m_pDecomposer);
#endif
	}

	m_pDecomposer = NULL;
}


int	CDAPI::NewObject(const char *szName, unsigned long int dwCharset, const char *szDataFile, IDecObject **ppObject)
{
#ifdef SYM_PLATFORM_HAS_SHAREDOBJS
	//
	// Validate our pointers...
	//
	dec_assert(m_pfnDecNewFileObject);

	if ((*m_pfnDecNewFileObject)(szName, dwCharset, szDataFile, ppObject) == DEC_OK)
		return DAPI_OK;
#else
	if (DecNewFileObject(szName, dwCharset, szDataFile, ppObject) == DEC_OK)
		return DAPI_OK;
#endif

	return DAPI_ERROR;
}


int	CDAPI::DeleteObject(IDecObject *pObject)
{
#ifdef SYM_PLATFORM_HAS_SHAREDOBJS
	//
	// Validate our pointers...
	//
	dec_assert(m_pfnDecDeleteFileObject);
	
	if ((*m_pfnDecDeleteFileObject)(pObject) == DEC_OK)
		return DAPI_OK;
#else
	if (DecDeleteFileObject(pObject) == DEC_OK)
		return DAPI_OK;
#endif

	return DAPI_ERROR;
}


int CDAPI::GetUnicodeName(IDecObject *pObject, wchar_t *pszName, DWORD *pdwNameSize, DWORD *pdwCharset)
{
#ifdef _WINDOWS
	CUNIMO	*pMapObject = NULL;
	char	*pszMBCSName = NULL;
	DWORD	dwMBCSBufferSize = 0;
	DWORD	dwUniBufferSize = 0;
	DWORD	dwUniNameSize = 0;

	if (!pObject)
		return DAPI_ERROR;

	// Get the size of the caller's buffer.
	if (pdwNameSize)
	{
		dwMBCSBufferSize = *pdwNameSize;
		dwUniBufferSize = *pdwNameSize;
	}
	else
	{
		dwMBCSBufferSize = MAX_PATH;
		dwUniBufferSize = MAX_PATH * sizeof(wchar_t);
	}

	// Allocate buffer for MBCS name
	pszMBCSName = (char *)malloc(dwMBCSBufferSize);
	if(!pszMBCSName)
		return DAPI_ERROR;

	// Get the MBCS name from the input IDecObject.
	pObject->GetName(pszMBCSName, &dwMBCSBufferSize, pdwCharset);
	pMapObject = GetUnicodeMappingObject(pszMBCSName);

	// Store the returned name (if found)
	if (pMapObject)
	{
		// Save required size of buffer (in bytes, not characters)
		dwUniNameSize = (wcslen(pMapObject->pszUniName) + 1) * sizeof(wchar_t);

		if(dwUniBufferSize >= dwUniNameSize)
		{
			if (pszName)
				wcscpy(pszName, pMapObject->pszUniName);
		}
		else
		{
			// The buffer isn't large enough to hold the string.
			// Return a NULL-string for the name.
			if (pszName)
				*pszName = L'\0';
		}

		// Delete mapping object once we're done with it
		DeleteUnicodeMappingObject(pMapObject);
	}
	else
	{
		// Convert the MBCS name string to Unicode in the caller's input buffer.
		if(pszMBCSName)
		{
			if (pszName)
				mbstowcs(pszName, pszMBCSName, dwMBCSBufferSize);
		}
		else
		{
			// We couldn't retrieve the name for some reason
			if (pszName)
				*pszName = L'\0';
		}
	}

	// Save out params
	if(pdwNameSize)
		*pdwNameSize = dwUniNameSize;

	// Cleanup
	if(pszMBCSName)
	{
		free(pszMBCSName);
		pszMBCSName = NULL;
	}

	return DAPI_OK;
#else
	//
	// No Unicode support under UNIX
	//
	return DAPI_ERROR;
#endif
}


int CDAPI::GetUnicodeDataFile(IDecObject *pObject, wchar_t *pszDataFile, DWORD *pdwNameSize)
{
#ifdef _WINDOWS
	CUNIMO	*pMapObject = NULL;
	char	*pszMBCSDataFile = NULL;
	DWORD	dwMBCSBufferSize = 0;
	DWORD	dwUniBufferSize = 0;
	DWORD	dwUniNameSize = 0;

	if (!pObject)
		return DAPI_ERROR;

	// Get the size of the caller's buffer.
	if (pdwNameSize)
	{
		dwMBCSBufferSize = *pdwNameSize;
		dwUniBufferSize = *pdwNameSize;
	}
	else
	{
		dwMBCSBufferSize = MAX_PATH;
		dwUniBufferSize = MAX_PATH * sizeof(wchar_t);
	}

	// Allocate buffer for MBCS name
	pszMBCSDataFile = (char *)malloc(dwMBCSBufferSize);
	if(!pszMBCSDataFile)
		return DAPI_ERROR;

	// Get the MBCS name from the input IDecObject.
	pObject->GetDataFile(pszMBCSDataFile, &dwMBCSBufferSize);
	pMapObject = GetUnicodeMappingObject(pszMBCSDataFile);

	// Store the returned name (if found)
	if (pMapObject)
	{
		// Save required size of buffer (in bytes, not characters)
		dwUniNameSize = (wcslen(pMapObject->pszUniName) + 1) * sizeof(wchar_t);

		if(dwUniBufferSize >= dwUniNameSize)
		{
			if (pszDataFile)
				wcscpy(pszDataFile, pMapObject->pszUniName);
		}
		else
		{
			// The buffer isn't large enough to hold the string.
			// Return a NULL-string for the name.
			if (pszDataFile)
				*pszDataFile = L'\0';
		}

		// Delete mapping object once we're done with it
		DeleteUnicodeMappingObject(pMapObject);
	}
	else
	{
		// Convert the MBCS name string to Unicode in the caller's input buffer.
		if(pszMBCSDataFile)
		{
			if (pszDataFile)
				mbstowcs(pszDataFile, pszMBCSDataFile, dwMBCSBufferSize);
		}
		else
		{
			// We couldn't retrieve the name for some reason
			if (pszDataFile)
				*pszDataFile = L'\0';
		}
	}

	// Save out params
	if(pdwNameSize)
		*pdwNameSize = dwUniNameSize;

	// Cleanup
	if(pszMBCSDataFile)
	{
		free(pszMBCSDataFile);
		pszMBCSDataFile = NULL;
	}

	return DAPI_OK;

#else
	//
	// No Unicode support under UNIX
	//
	return DAPI_ERROR;
#endif
}


bool CDAPI::LoadEngines(char *pszEnginePath, char **ppszEngineFilenames, int *piEngineIDs, int iNumEngines)
{
	int				index;
	CDecSDKEngine	*pEngine;
	char			szNewFunc[48];
	char			szDeleteFunc[48];
	bool			rc = true;

	index = 0;

	while (index < iNumEngines)
	{
		pEngine = new CDecSDKEngine;
		if (!pEngine)
			return(false);

#if defined(SYM_PLATFORM_HAS_DLLS) || defined(SYM_PLATFORM_HAS_SHAREDOBJS)
		// For those platforms that support dynamic linking, the entry point names
		// are the same for all Decomposer engines.
		strcpy(szNewFunc, "DecNewEngine");
		strcpy(szDeleteFunc, "DecDeleteEngine");
#else
		// For those platforms that do NOT support dynamic linking, the Decomposer SDK
		// must be statically linked to the engines.  Thus, the links must be known at
		// compile-time in the CDecSDKEngine class (see DecEngine.cpp).
		strcpy(szNewFunc, "");
		strcpy(szDeleteFunc, "");
#endif

		if (pEngine->LoadEngine(pszEnginePath, ppszEngineFilenames[index], piEngineIDs[index], szNewFunc, szDeleteFunc))
		{
			// Add the loaded engine to the Decomposer.
			pEngine->AddEngine(m_pDecomposer);
			m_pEngine[index] = pEngine;
			index++;
		}
		else
		{
			delete pEngine;
			rc = false;
			break;
		}
	}

	return(rc);
}


bool CDAPI::LoadEnginesW(wchar_t *pszEnginePath, wchar_t **ppszEngineFilenames, int *piEngineIDs, int iNumEngines)
{
	int				index;
	CDecSDKEngine	*pEngine;
	char			szNewFunc[48];
	char			szDeleteFunc[48];
	bool			rc = true;

	index = 0;

	while (index < iNumEngines)
	{
		pEngine = new CDecSDKEngine;
		if (!pEngine)
			return(false);

#if defined(_WINDOWS)
		// For those platforms that support dynamic linking, the entry point names
		// are the same for all Decomposer engines.
		strcpy(szNewFunc, "DecNewEngine");
		strcpy(szDeleteFunc, "DecDeleteEngine");
#else
		// 
		// Only Windows supports the wide char version of LoadEngines...
		//
		return false;
#endif

		if (pEngine->LoadEngineW(pszEnginePath, ppszEngineFilenames[index], piEngineIDs[index], szNewFunc, szDeleteFunc))
		{
			// Add the loaded engine to the Decomposer.
			pEngine->AddEngine(m_pDecomposer);
			m_pEngine[index] = pEngine;
			index++;
		}
		else
		{
			delete pEngine;
			rc = false;
			break;
		}
	}

	return(rc);
}


void CDAPI::UnloadEngines()
{
	for (int index = 0; index < MAX_DEC_ENGINES; index++)
	{
		if (m_pEngine[index])
			delete m_pEngine[index];
	}
}


void CDAPI::StripNewline(char *pszLine)
{
	size_t	len;
	char	*ptr;

	dec_assert(pszLine);

	len = strlen(pszLine);
	if (len)
	{
		ptr = pszLine + len - 1;
		while ( len > 0 && (*ptr == '\n' || *ptr == '\r') )
		{
			*ptr = 0;
			ptr--;
			len--;
		}
	}
}


void CDAPI::StripNewlineW(wchar_t *pszLine)
{
#if defined(_WINDOWS)

	size_t	len;
	wchar_t *ptr;

	dec_assert(pszLine);

	len = wcslen(pszLine);
	if (len)
	{
		ptr = pszLine + len - 1;
		while ( len > 0 && (*ptr == L'\n' || *ptr == L'\r') )
		{
			*ptr = 0;
			ptr--;
			len--;
		}		
	}
#else
	//
	// No Unicode support except under Windows.
	//
	dec_assert(0);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// DAPI_NewDecomposer()

DECSDKLINKAGE int STDCALL DAPI_NewDecomposer(IDAPI **ppDecomposer)
{
	try
	{
		*ppDecomposer = new CDAPI;
		return DAPI_OK;
	}
	catch (...)
	{
		*ppDecomposer = NULL;
		return DAPI_ERROR;
	}
}


/////////////////////////////////////////////////////////////////////////////
// DAPI_DeleteDecomposer()

DECSDKLINKAGE int STDCALL DAPI_DeleteDecomposer(IDAPI *pDecomposer)
{
	if (pDecomposer)
	{
		delete pDecomposer;
	}

	return DAPI_OK;
}


DECSDKLINKAGE void STDCALL DAPI_GetVersion(const char **ppszVersion)
{
	*ppszVersion = &gszDecSDKVersion[0];
}


#ifdef SYM_PLATFORM_HAS_SHAREDOBJS

/////////////////////////////////////////////////////////////////////////
// CDAPI::InitializeDec2SharedObj()

bool CDAPI::InitializeDec2SharedObj(const char *pszDec2Path)
{
	char		szFullPathToDec2[MAX_PATH];
	const char	szDec2Filename[] = "libdec2.so";
	int			iLen1, iLen2;
	char		chLast;

	//
	// Validate input
	//
	if(!pszDec2Path)
	{
		return false;
	}

	// Construct the fully-qualified filename of the Dec2 shared object.
	// Add a path separator if necessary.
	iLen1 = strlen(pszDec2Path);
	iLen2 = strlen(szDec2Filename);
	if (iLen1 + iLen2 + 2 >= sizeof(szFullPathToDec2))
		return false;  // Constructed path would be too long...

	strcpy(szFullPathToDec2, pszDec2Path);
	if (iLen1)
		iLen1--;
	chLast = szFullPathToDec2[iLen1];
	if (chLast != '/')
		strcat(szFullPathToDec2, "/");

	strcat(szFullPathToDec2, szDec2Filename);

	//
	// Load the DecSDK
	//
	m_hInst = dlopen(szFullPathToDec2, RTLD_NOW);

	if(!m_hInst)
	{
//      printf("Failed to load shared object %s.  Error message: %s\n", szFullPathToDec2, dlerror());
		return false;
	}

	//
	// Get the functions pointers we need
	//
	m_pfnDecNewDecomposer = (PFNDECNEWDECOMPOSER)dlsym(m_hInst, "DecNewDecomposer");
	m_pfnDecDeleteDecomposer = (PFNDECDELETEDECOMPOSER)dlsym(m_hInst, "DecDeleteDecomposer");
	m_pfnDecNewFileObject = (PFNDECNEWFILEOBJECT)dlsym(m_hInst, "DecNewFileObject");
	m_pfnDecDeleteFileObject = (PFNDECDELETEFILEOBJECT)dlsym(m_hInst, "DecDeleteFileObject");
	m_pfnDecGetVersion = (PFNDECGETVERSION)dlsym(m_hInst, "DecGetVersion");

	//
	// Make sure we succeeded
	//
	if(!m_pfnDecNewDecomposer		|| 
	  !m_pfnDecDeleteDecomposer	||
	  !m_pfnDecNewFileObject		||
	  !m_pfnDecDeleteFileObject	||
	  !m_pfnDecGetVersion)
	{
		return false;
	}

	return true;
}
 
/////////////////////////////////////////////////////////////////////////
// CDAPI::UnloadDec2SharedObj()

bool CDAPI::UnloadDec2SharedObj()
{
	if(m_hInst)
	{
		//
		// Free the shared object
		//
		dlclose(m_hInst);
		m_hInst = NULL;
	}

	m_pfnDecNewDecomposer = NULL;
	m_pfnDecDeleteDecomposer = NULL;
	m_pfnDecNewFileObject = NULL;
	m_pfnDecDeleteFileObject = NULL;
	m_pfnDecGetVersion = NULL;

	return true;
}

#endif
