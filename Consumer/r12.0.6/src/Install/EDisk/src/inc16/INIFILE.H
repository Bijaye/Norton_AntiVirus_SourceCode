/* $Header:   S:/INCLUDE/VCS/INIFILE.H_V   1.5   31 Mar 1992 00:34:04   JOHN  $ */

/*----------------------------------------------------------------------*
 * INIFILE.H								*
 *									*
 * Norton master INI file structures					*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * PVCS revision history:
 *
 * $Log:   S:/INCLUDE/VCS/INIFILE.H_V  $
 * 
 *    Rev 1.5   31 Mar 1992 00:34:04   JOHN
 * Changed to support loading entire INI file into memory.
 * 
 *    Rev 1.4   27 Jan 1992 22:36:18   TORSTEN
 * Changed the #include statements to use angle brackets.
 * 
 *    Rev 1.3   20 Jan 1992 18:31:30   JOHN
 * Added CFG_FLOPPY error code
 * 
 *    Rev 1.2   18 Nov 1991 12:22:06   PETERD
 * Added ConfigGetVersion() prototype.
 * 
 *    Rev 1.1   29 Oct 1991 16:34:46   PETERD
 * Added product variable name to ConfigOpen/CreateFile.
 * 
 *    Rev 1.0   23 Mar 1991 20:54:50   JOHN
 * Initial revision.
 */

#ifndef INCL_FILE
#include <file.h>
#endif

#ifndef INCL_INIFILE
#define INCL_INIFILE

/*----------------------------------------------------------------------*
 * Maximum number of products in master INI file			*
 *----------------------------------------------------------------------*/

#define MAX_INI_INDEX	20

/*----------------------------------------------------------------------*
 * Signature and version						*
 *----------------------------------------------------------------------*/

#define NORTON_INI_SIG	"\x1ASY"	/* EOF marker, 'S', 'Y'		*/
#define NORTON_INI_VER	0x0001		/* NORTON.INI file version	*/

/*----------------------------------------------------------------------*
 * Return codes from Config...() functions				*
 *----------------------------------------------------------------------*/

#define CFG_OK		0x0000		/* No error			*/
#define CFG_NO_FILE	0x0001		/* ERROR - File not found	*/
#define CFG_NO_SIG	0x0002		/* ERROR - Signature not found	*/
#define CFG_DISK_ERR	0x0003		/* ERROR - Disk problem		*/
#define CFG_WRONG_VER	0x0004		/* ERROR - Wrong version	*/
#define CFG_WRONG_LEN	0x0005		/* ERROR - Wrong length		*/
#define CFG_FULL	0x0006		/* ERROR - All indexes used	*/
#define CFG_FLOPPY	0x0007		/* ERROR - Attempted to create	*/
					/*         INI file on floppy	*/
#define CFG_NO_MEM	0x0008		/* ERROR - Out of memory	*/

/*----------------------------------------------------------------------*
 * Flags and constants used in header records				*
 *----------------------------------------------------------------------*/

#define CFG_INDEX_CHANGED       0x8000  /* FLAG - Index was changed     */

#define CFG_CLEAR_VERSION       0xFFFF  /* No verison code              */

#define CFG_IN_MEMORY		0x80000000L

/*----------------------------------------------------------------------*
 * Data structures							*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte	signature[4];		/* Product signature		*/
    Ulong	offset;			/* Offset in NORTON.INI file	*/
    Word	length;			/* Length of configuration data	*/
    Word	version;		/* Configuration data version	*/
    } CfgIndexRec;

typedef struct
    {
    Byte	signature[4];		/* Signature of NORTON.INI file	*/
    Word	version;		/* NORTON.INI file version	*/
    CfgIndexRec	index[MAX_INI_INDEX];	/* Index to initialization data	*/
    Word        flags;			/* Flags			*/

    // The following are used only when in memory:

    Byte	fileName[MAX_PATH_NAME]; /* Name of INI file		*/
    Word	handle;			 /* Handle of open INI file	*/
    Byte	far *lpBuffer;		 /* Buffer to read INI file	*/
    } CfgHeaderRec;

// This is the format of the header when stored on disk.
// It must exactly match the first four fields of CfgHeaderRec.
// The filler at the end is for NU 6.0 compatibility.

typedef struct
    {
    Byte	signature[4];
    Word	version;
    CfgIndexRec	index[MAX_INI_INDEX];
    Word        flags;
    Byte	filler[MAX_PATH_NAME + sizeof(Word)];
    } CfgDiskHeaderRec;

/*----------------------------------------------------------------------*
 * Function prototypes							*
 *----------------------------------------------------------------------*/

/*
 * Open an existing .INI file and read header
 *
 * Returns error code (listed above)
 */

int PASCAL ConfigOpenFile(Byte *pszProductVar, Byte *fileName, CfgHeaderRec *pHeader);

/*
 * Create a new .INI file
 *
 * Returns error code (listed above)
 */

int PASCAL ConfigCreateFile(Byte *pszProductVar, Byte *pszSearchName,
			    Byte *fileName, CfgHeaderRec *pHeader);

/*
 * Read configuration data for a particular product
 *
 * Returns error code (listed above)
 *
 * Example:
 *   ConfigReadData(&cfgHeader, NU_SIG, NU_VER, sizeof(NUConfigRec), &config);
 */

int PASCAL ConfigReadData(CfgHeaderRec *pHeader,
			  Byte *signature, Word version,
			  Word length, void *pBuffer);

/*
 * Read configuration version and length
 *
 * Returns error code (listed above)
 *
 * Example:
 *   ConfigGetVersion(&cfgHeader, NU_SIG, &version, &length);
 */

int PASCAL ConfigGetVersion(CfgHeaderRec *pHeader,
			    Byte *signature, Word *version,
			    Word *length);

/*
 * Write configuration data for a particular product
 *
 * Replaces the existing product data or adds product to end of INI file
 * file if it didn't previously exist.  If new length is larger than old
 * length, the function will rearrange the INI file to make room.
 *
 * Returns error code (listed above)
 *
 * Example:
 *   ConfigWriteData(&cfgHeader, NU_SIG, NU_VER, sizeof(NUConfigRec), &config);
 */

int PASCAL ConfigWriteData( CfgHeaderRec *pHeader,
			    Byte *signature, Word version,
			    Word length, void *pBuffer );

/*
 * Close an .INI file
 */

void PASCAL ConfigCloseFile(CfgHeaderRec *pHeader);

#endif
