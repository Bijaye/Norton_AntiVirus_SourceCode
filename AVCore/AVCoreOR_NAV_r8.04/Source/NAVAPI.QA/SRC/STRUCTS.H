// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI.QA/VCS/Structs.h_v   1.0   06 Aug 1998 12:15:54   NMILLER  $
//
// Description: Header file for APICall RPC structures.  
//
//************************************************************************
// $Log:   S:/NAVAPI.QA/VCS/Structs.h_v  $
// 
//    Rev 1.0   06 Aug 1998 12:15:54   NMILLER
// Initial revision.
// 
//************************************************************************
// Cheesy structs for sending through RPC

#ifndef _STRUCTS_H
#define _STRUCTS_H

// Enumeration that identifies the class to which the rest of the data
// in the packet belongs.

typedef enum
{
	TYPE_INIT = 0,			// NAVEngineInit
	TYPE_CLOSE,				// NAVEngineClose
	TYPE_BR,				// NAVScanBootRecord
	TYPE_MBR,				// NAVScanMasterBootRecord
	TYPE_MEM,				// NAVScanMemory
	TYPE_REPAIR_BR,			// NAVRepairBootRecord
	TYPE_REPAIR_MBR,		// NAVRepairMasterBootRecord
	TYPE_FREE_VH,			// NAVFreeVirusHandle
	TYPE_GET_NUM_VIR,		// NAVGetNumVirusEntries
	TYPE_LOAD_VIR_TABLE,	// NAVLoadVirusEntryTable
	TYPE_FREE_VIR_TABLE,	// NAVFreeVirusEntryTable
	TYPE_GET_VE,			// NAVGetVirusEntry
	TYPE_FREE_VE,			// NAVReleaseVirusEntry
	TYPE_GET_VIR_INFO,		// NAVGetVirusInfo
	TYPE_GET_DEF_INFO,		// NAVGetDefInfo
	TYPE_GET_DEFTAB_INFO,	// NAVGetDefTableInfo
	TYPE_GET_DEFTAB_COUNT,	// NAVGetDefTableCount
	TYPE_SCAN_FILE,			// NAVScanFile
	TYPE_REPAIR_FILE,		// NAVRepairFile
	TYPE_LAST
} APITYPE;


// A simple struct with ample fields to send through RPC the parameters of
// the NAVAPI function calls.

typedef struct RPCSend_tag
{
	APITYPE	dwType;			// Fill in with class type before sending
	DWORD	wWord1;			// Two fields for UINTs and WORDs
	DWORD	wWord2;
	TCHAR *	lpszStr1;		// Two fields for null-terminated strings
	TCHAR *	lpszStr2;
	DWORD	dwContext;		// internal use only--do not send via RPC
	DWORD	dwVirus;
	DWORD	dwVirusDef;
	DWORD	dwVirusDefTable;
} SENDSTRUCT, * LPSENDSTRUCT;

#endif
