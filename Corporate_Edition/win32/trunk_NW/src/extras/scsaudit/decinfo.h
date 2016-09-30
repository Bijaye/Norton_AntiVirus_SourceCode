// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _DEC_INFO_H_
#define _DEC_INFO_H_

#if defined WIN32 || defined NLM || defined LINUX

typedef struct {
    DWORD   dwVersion;
    WORD    wDay; 
    WORD    wMonth; 
    WORD    wYear;
    DWORD   dwSequence;
} DEFVERSIONINFO, *PDEFVERSIONINFO;

#include <tchar.h>

#endif //..WIN32

// Container Processing Flags   ( used in DECOMPFILEINFO.dwProcessFlags )
#define DECINFO_STOP_PROCESS                 0x00000001L
#define DECINFO_CONTINUE_PROCESS             0x00000002L
#define DECINFO_STOP_AND_BACKUP              0x00000004L
#define DECINFO_RESTORE_FROM_BACKUP          0x00000008L
#define DECINFO_CREATE_BACKUP_FAILED         0x00000010L

typedef struct _pDECOMPFILEINFO
{
	LPTSTR			lpszOrigArchiveFileName;    // Full path of top-level container (eg. "a:\temp\f0.zip")
												// or the Full path of an Uncompressed file
	LPTSTR			lpszNestingPath;			// Intermediate containers (eg. "f1.zip>>f2.zip>>" )
	LPTSTR		    lpszFileToGet;				// Infected file name (eg. "virus.com")
	LPTSTR		    lpszExtractTempFile;		// Extracted Tempory file name (eg. "tMP123.com")

	DWORD           dwDepthOfFileToGet;         // Depth of infected file (stored in log line)
	DWORD           dwDepthLimit;               // User configurable depth to scan
	
	BOOL			bIsCompressed;				// If the file is compressed
	WORD			wResult;					// Decomposer Result Flag to inform the Dec Engine

	BOOL			bCanModify;					// True for the current file (the top-level
	BOOL			bCanReplace;				// container or the files inside the cotainer)
	BOOL			bCanDelete;					// and is set in OnNew() based on the information from
												// the decomposer.
	
	DWORD			dwInfected;					// Total number of files infected in the container
	DWORD			dwCleaned;					// Total number of files cleaned in the container
	DWORD			dwDeleted;					// Total number of files deleted in the container


	BOOL			bQuarantine;				// If the action results in moving the container												
	BOOL			bDelete;					// to Quarantine, or Deleting the container
    BOOL            bRename;                    // or Renaming the container
	
	DWORD			dwProcessFlags;             // Flag to indicate one of the processing options

    BOOL            bFirstProcess;              // TRUE if it is the first time or FALSE if it is
                                                // a subsequent processing of the container after
                                                // making a backup of the container.

    BOOL            bBackupToQuarantine;        // TRUE if Back up before repair is turned ON in 
                                                // Scan Options

    DWORD           dwVBinID;                   // Non Zero Value if the backup was successfully created.

    WORD            wFilesScanned;              // Number of files scanned within the container.

    WORD            wFilesNeedScan;             // The total number of files in the container that need to
                                                // be scanned.  This will be less than or equal to the total
                                                // number of files in the container depending upon the type 
                                                // of scanning (all files or Selected Extension or Selected Type).

}DECOMPFILEINFO, far * pDECOMPFILEINFO;

#endif // _DEC_INFO_H
