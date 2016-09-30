// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

/*	File system interface */

#ifndef FILE_DEFS
#define FILE_DEFS

#ifdef __cplusplus
extern "C" {
#endif


/*	File type flags (used with NSFSearch directory searching). */


#define	FILE_ANY		0		/* Any file type */
#define	FILE_DBREPL		1		/* Starting in V3, any DB that is a candidate for replication */
#define FILE_DBDESIGN   2       /* Databases that can be templates */
								/*	3 unused */
#define	FILE_DBANY		4		/* NS?, any NSF version */
#define	FILE_FTANY		5		/* NT?, any NTF version */
#define FILE_MDMTYPE	6		/* MDM - modem command file */
#define FILE_DIRSONLY	7		/* directories only */
#define FILE_VPCTYPE	8		/* VPC - virtual port command file */
#define FILE_SCRTYPE	9		/* SCR - comm port script files */

#define	FILE_TYPEMASK	0x00ff	/* File type mask (for FILE_xxx codes above) */
#define	FILE_DIRS		0x8000	/* List subdirectories as well as normal files */
#define	FILE_NOUPDIRS	0x4000	/* Do NOT return ..'s */
#define	FILE_RECURSE	0x2000	/* Recurse into subdirectories */
#define	FILE_ALL_LINKS	0x1000	/* All directories, linked files & directories */


/*	Function declarations */

STATUS LNPUBLIC		OSPathNetConstruct(char far *PortName,
										char far *ServerName,
										char far *FileName,
										char far *retPathName);
STATUS LNPUBLIC		OSPathNetParse(char far *PathName,
										char far *retPortName,
										char far *retServerName,
										char far *retFileName);
WORD 	LNPUBLIC OSGetDataDirectory(char far *retPathName);

#ifdef __cplusplus
}
#endif

#endif

