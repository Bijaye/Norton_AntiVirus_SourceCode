#ifndef __AVSHARED_H
#define __AVSHARED_H

#include "platform.h"
#include "navapi.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#if defined (SYM_WIN32)

#elif defined (SYM_UNIX)
    #define _MAX_PATH 2048
#elif defined (SYM_NLM)

#endif


///////////////////////////////////////////////////////////////////////////////
// Set this to the value of the last boolean, non-item specific query in 
// NAVVIRINFO.
///////////////////////////////////////////////////////////////////////////////
#define NAV_VI_NON_ITEM_SPECIFIC_LAST NAV_VI_BOOL_REPAIRED-1


// NAVAPI Return values
static char gNAVAPIReturnValues[][50] = {
    {"NAV_OK"},
    {"NAV_ERROR"},
    {"NAV_INSUFFICIENT_BUFFER"},
    {"NAV_INVALID_ARG"},
    {"NAV_MEMORY_ERROR"},
    {"NAV_NO_ACCESS"},
    {"NAV_CANT_REPAIR"},
    {"NAV_CANT_DELETE"},
    {"NAV_ENGINE_IN_USE"},
    {"NAV_VXD_INIT_FAILURE"},
    {"NAV_DEFINITIONS_ERROR"},
    {"UNKNOWN"}
};


///////////////////////////////////////////////////////////////////////////////
// NAVAPI VIRUS INFO STRING structure
///////////////////////////////////////////////////////////////////////////////
typedef struct{
        WORD wItemIndex;
		char szResultStr[40];
}VIRUS_INFO_STR, * LPVIRUS_INFO_STR;


///////////////////////////////////////////////////////////////////////////////
// NAVAPI VIRUS INFO STRING declaration
///////////////////////////////////////////////////////////////////////////////
static VIRUS_INFO_STR gastVirusInfoStr[] = {
	{ NAV_VI_VIRUS_NAME,			"NAV_VI_VIRUS_NAME:             "},
	{ NAV_VI_VIRUS_ID,				"NAV_VI_VIRUS_ID:               "},
	{ NAV_VI_VIRUS_ALIAS,			"NAV_VI_VIRUS_ALIAS:            "},
	{ NAV_VI_VIRUS_INFO,			"NAV_VI_VIRUS_INFO:             "},
	{ NAV_VI_VIRUS_SIZE,			"NAV_VI_VIRUS_SIZE:             "},
	{ NAV_VI_ITEM_NAME,				"NAV_VI_ITEM_NAME:              "},
	{ NAV_VI_PHYSICAL_DRIVE_NUM,	"NAV_VI_PHYSICAL_DRIVE_NUM:     "},
	{ NAV_VI_PARTITION_NUM,			"NAV_VI_PARTITION_NUM:          "},
	{ NAV_VI_PARTITION_TYPE,		"NAV_VI_PARTITION_TYPE:         "},
};


///////////////////////////////////////////////////////////////////////////////
// Calculate number of items in VIRUS INFO STRING structure
///////////////////////////////////////////////////////////////////////////////
#define TOTALENUMSTR sizeof(gastVirusInfoStr) / sizeof(VIRUS_INFO_STR)


///////////////////////////////////////////////////////////////////////////////
// NAVAPI VIRUS INFO BOOLEAN structure
///////////////////////////////////////////////////////////////////////////////
typedef struct{
        WORD wItemIndex;
		char szResultStr[40];
		char szResultValue[2][6];
}VIRUS_INFO_BOOL, * LPVIRUS_INFO_BOOL;


///////////////////////////////////////////////////////////////////////////////
// NAVAPI VIRUS INFO BOOL declaration
///////////////////////////////////////////////////////////////////////////////
static VIRUS_INFO_BOOL gastVirusInfoBool[] = {
		{ NAV_VI_BOOL_INFECT_FILES,			"NAV_VI_BOOL_INFECT_FILES:      ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_INFECT_EXE,			"NAV_VI_BOOL_INFECT_EXE:        ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_INFECT_COM,			"NAV_VI_BOOL_INFECT_COM:        ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_INFECT_COMCOM,		"NAV_VI_BOOL_INFECT_COMCOM:     ",	"FALSE", "TRUE"},
        { NAV_VI_BOOL_INFECT_SYS,			"NAV_VI_BOOL_INFECT_SYS:        ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_INFECT_MBR,			"NAV_VI_BOOL_INFECT_MBR:        ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_INFECT_FLOPPY_BOOT,	"NAV_VI_BOOL_INFECT_FLOPPY_BOOT:",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_INFECT_HD_BOOT,		"NAV_VI_BOOL_INFECT_HD_BOOT:    ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_INFECT_WILD,			"NAV_VI_BOOL_INFECT_WILD:       ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_MEMORY_RES,			"NAV_VI_BOOL_MEMORY_RES:        ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_SIZE_STEALTH,			"NAV_VI_BOOL_SIZE_STEALTH:      ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_FULL_STEALTH,			"NAV_VI_BOOL_FULL_STEALTH:      ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_TRIGGER,				"NAV_VI_BOOL_TRIGGER:           ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_ENCRYPTING,			"NAV_VI_BOOL_ENCRYPTING:        ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_POLYMORPHIC,			"NAV_VI_BOOL_POLYMORPHIC:       ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_MULTIPART,			"NAV_VI_BOOL_MULTIPART:         ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_MAC_VIRUS,			"NAV_VI_BOOL_MAC_VIRUS:         ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_MACRO_VIRUS,			"NAV_VI_BOOL_MACRO_VIRUS:       ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_WINDOWS_VIRUS,		"NAV_VI_BOOL_WINDOWS_VIRUS:     ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_AGENT_VIRUS,			"NAV_VI_BOOL_AGENT_VIRUS:       ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_SUBMITABLE,			"NAV_VI_BOOL_SUBMITABLE:        ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_HEURISTIC,			"NAV_VI_BOOL_HEURISTIC:         ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_DELETE_ITEM,          "NAV_VI_BOOL_DELETE_ITEM:       ",  "FALSE", "TRUE"},
        { NAV_VI_BOOL_REPAIRED,				"NAV_VI_BOOL_REPAIRED:          ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_REPAIR_FAILED,		"NAV_VI_BOOL_REPAIR_FAILED:     ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_REPAIRABLE,			"NAV_VI_BOOL_REPAIRABLE:        ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_DELETABLE,			"NAV_VI_BOOL_DELETABLE:         ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_NO_ACCESS,			"NAV_VI_BOOL_NO_ACCESS:         ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_DISEMBODIED,			"NAV_VI_BOOL_DISEMBODIED:       ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_DELETED,				"NAV_VI_BOOL_DELETED:           ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_DELETE_FAILED,		"NAV_VI_BOOL_DELETE_FAILED:     ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_DEF_DELETED,			"NAV_VI_BOOL_DEF_DELETED:       ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_INFO_VALID,			"NAV_VI_BOOL_INFO_VALID:        ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_MEMORY_DETECT,		"NAV_VI_BOOL_MEMORY_DETECT:     ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_FILE_ITEM,			"NAV_VI_BOOL_FILE_ITEM:         ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_BOOT_ITEM,			"NAV_VI_BOOL_BOOT_ITEM:         ",	"FALSE", "TRUE"}, 
        { NAV_VI_BOOL_PART_ITEM,			"NAV_VI_BOOL_PART_ITEM:         ",	"FALSE", "TRUE"}, 
};

///////////////////////////////////////////////////////////////////////////////
// Calculate number of items in VIRUS INFO BOOL structure
///////////////////////////////////////////////////////////////////////////////
#define TOTALENUMBOOL sizeof(gastVirusInfoBool) / sizeof(VIRUS_INFO_BOOL)



#define MAX_GET_VIRUS_INFO_SIZE		40000


///////////////////////////////////////////////////////////////////////////////
//	Engine Status Flags
///////////////////////////////////////////////////////////////////////////////
#define	FUNCTION_NOT_SUPPORTED		0xffffffff


///////////////////////////////////////////////////////////////////////////////
//	Scan Option Flags
///////////////////////////////////////////////////////////////////////////////
#define	SCAN_VIRUS					0
#define	REPAIR_VIRUS				1
#define	DELETE_VIRUS				2
#define PROCESS_SINGLE				4
#define PROCESS_DIR					8
#define	PARTIAL_VIRUS_LOG			16
#define	FULL_VIRUS_LOG				32
#define MULTI_THREAD_SCANNING		64
#define INFO_TEST					128
#define MD5_HASH					256


///////////////////////////////////////////////////////////////////////////////
// Maximum size values
///////////////////////////////////////////////////////////////////////////////
#define MAX_VIRUS_NAME_SIZE			25
#define MAX_VIRUS_ALIAS_SIZE		256
#define MAX_VIRUS_INFO_SIZE			256
#define MAX_VIRUS_SIZE				16
#define MAX_VIRUS_VID_SIZE			16


///////////////////////////////////////////////////////////////////////////////
//	AV Engine config info
///////////////////////////////////////////////////////////////////////////////
typedef struct
{
	HNAVENGINE	hNAVEngine;					// Handle to AV engine
	char		szDefsPath[_MAX_PATH];		// Path to virus definition files
    char		szNavexInfFile[_MAX_PATH];	// Full path to optional .INF file or NULL
	char		szReportLogFile[_MAX_PATH];	// Full path to report log file
	WORD		wHeuristicLevel;			// Heuristic scanning level 0,1,2,3
	DWORD		dwNumOfTimesToRepeatScan;	// Number of times to scan same file, for timing purposes.
	DWORD		dwNumOfTimesToRepeatTotalScan;	// Number of times to repeat total scan, for stability purposes.
    WORD		wFlags;						// Reserved
	BOOL		bTabMode;					// True if we are in tab output mode (file scanning only).
	DWORD		dwOptions;					// Options to pass to functions
} NAVAPI_CONFIG, FAR *LPNAVAPI_CONFIG;


///////////////////////////////////////////////////////////////////////////////
// Data Structure to hold virus table information
///////////////////////////////////////////////////////////////////////////////
typedef struct virtable {
	char	szVirusName[MAX_VIRUS_NAME_SIZE+1];
	char	szVirusAlias[MAX_VIRUS_ALIAS_SIZE+1];
	char	szVirusInfo[MAX_VIRUS_INFO_SIZE+1];
	char	szVirusSize[MAX_VIRUS_SIZE+1];
	BOOL	abVIBoolValues[NAV_VI_NON_ITEM_SPECIFIC_LAST+1];
													// This should always be one
													// more than the last non-item
													// specific query in NAVVIRINFO.
	struct virtable *next;
} VIRTABLE, *LPVIRTABLE;


///////////////////////////////////////////////////////////////////////////////
// Size of Virus hash table. This hash table is hashed to the virus name.
///////////////////////////////////////////////////////////////////////////////
#define VIRTABLE_HASH_SIZE 3097


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL InitializeVirusHashTable ()
//
//	Description:
//		Loads the virus hash table and places it into global g_aVirHashTable
//      variable. Returns TRUE on success, FALSE on error.
//
//	Parameters:
//		none
//
//	Returns:
//		BOOL
//
///////////////////////////////////////////////////////////////////////////////
BOOL InitializeVirusHashTable(NAVAPI_CONFIG stNAVAPI_Config);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		LPVIRTABLE GetVirusHashTableEntry ()
//
//	Description:
//		Return the VIRTABLE entry from the hash table corresponding to a given
//      virus name.
//
//	Parameters:
//		szVirusName
//
//	Returns:
//		LPVIRTABLE
//
///////////////////////////////////////////////////////////////////////////////
LPVIRTABLE GetVirusHashTableEntry(char* szVirusName);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		LPVIRTABLE FreeVirTable()
//
//	Description:
//		Frees memory used by Virus Table entry and all linked entries.
//
//	Parameters:
//		lpVirTable
//
//	Returns:
//		LPVIRTABLE
//
///////////////////////////////////////////////////////////////////////////////
LPVIRTABLE FreeVirTable(LPVIRTABLE lpVirTable);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void CopyHNAVVIRUStoLPVIRTABLE ()
//
//	Description:
//		Fills in lpVirTable with information from a given HNAVVIRUS.
//
//	Parameters:
//		hVirusInfo
//		lpVirTable
//
//	Returns:
//		void
//
///////////////////////////////////////////////////////////////////////////////
void CopyHNAVVIRUStoLPVIRTABLE(HNAVVIRUS hVirusInfo, LPVIRTABLE lpVirTable);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void GetFullVirusInfoLoadSingleVirus()
//
//	Description:
//		Gets all the virus information for a given virus ID using the 
//		NAVLoadVirusDef functions. Fills in lpVirTable with the information.
//
//	Parameters:
//		hVirusInfo
//		dwVirusID
//		lpVirTable
//
//	Returns:
//		void
//
///////////////////////////////////////////////////////////////////////////////
void GetFullVirusInfoLoadSingleVirus(NAVAPI_CONFIG stNAVAPI_Config, DWORD dwVirusID, LPVIRTABLE lpVirTable);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void GetFullVirusInfo ()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void GetFullVirusInfo 
(
	HNAVVIRUS	hVirusInfo,
	LPSTR		lpszInfoBuffer,
	UINT		uInfoBufferSize
);

///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL IsInfected ()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
BOOL IsInfected
(
	HNAVVIRUS	hVirusInfo
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		DWORD GetVirusName ()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
DWORD GetVirusName 
(
	HNAVVIRUS	hVirusInfo,
	LPSTR		lpszVirusName,
	DWORD		dwBufferSize
);

#endif //__AVSHARED_H

