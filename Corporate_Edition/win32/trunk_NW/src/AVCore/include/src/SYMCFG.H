// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/symcfg.h_v   1.3   31 Jul 1997 10:12:12   JBELDEN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/symcfg.h_v  $ */
// 
//    Rev 1.3   31 Jul 1997 10:12:12   JBELDEN
// changed MAX_INI_LINE to 512 for SYM_DOS. 256 was just to short.
//
//    Rev 1.2   26 Oct 1996 11:18:32   JBRENNA
// Add ConfigParamToUpperNonDbcs() prototype.
//
//    Rev 1.1   30 Sep 1996 11:22:22   PMARTIN
// Added GetCacheSection() and GetCacheSectionNames() from Nail.
//
//    Rev 1.0   26 Jan 1996 20:13:26   RJACKSO
// Initial revision.
//
//    Rev 1.65   17 Jan 1996 13:49:42   RSTANEV
// Added NTK platform.
//
//    Rev 1.64   01 Aug 1995 22:51:06   AWELCH
// Moved Quake 7 changes to trunk.
//
//    Rev 1.63.1.5   30 May 1995 14:20:54   DLEVITON
// Changed prototype for W32 compiles.
//
//    Rev 1.63.1.4   15 May 1995 20:58:06   MBARNES
// Added prototypes for INI cache functions
//
//    Rev 1.63.1.3   14 May 1995 01:27:18   MBARNES
// Changed Symkrnl**ProfileString() back to Sym**ProfileString()
//
//    Rev 1.63.1.2   11 May 1995 00:19:12   MBARNES
// Changed names of functions from Sym**ProfileString to Symkrnl**ProfileString()
//
//    Rev 1.63.1.1   09 May 1995 22:02:10   TORSTEN
// Added the prototypes for the new functions in CFG_PROF.C.
//
//    Rev 1.63.1.0   24 Apr 1995 19:46:08   SCOTTP
// Branch base for version QAK7
//
//    Rev 1.63   17 Apr 1995 15:33:48   PATRICKM
// Changed return value of ConfigTerminate() from void to UINT.
//
//    Rev 1.62   06 Mar 1995 10:46:14   HENRI
// Added ConfigOpenRegistryKey & ConfigCloseRegistryKey
//
//    Rev 1.61   07 Feb 1995 22:30:50   MFALLEN
// Debugged for W32 and DOS
//
//    Rev 1.59   07 Feb 1995 18:18:46   MFALLEN
// Added ConfigProcessCmdLineEnd() function prototype.
//
//    Rev 1.58   05 Nov 1994 11:14:12   MARKK
// Added ConfigRegEnumValue proto
//
//    Rev 1.57   05 Oct 1994 09:28:32   DLEVITON
// ConfigRegDeleteTree
//
//    Rev 1.56   04 Oct 1994 15:05:10   DLEVITON
// ConfigRegDeleteKey and ConfigRegDeleteValue.
//
//    Rev 1.55   13 Sep 1994 11:44:34   MARKK
// Fixed IFDEF
//
//    Rev 1.54   13 Sep 1994 10:03:42   TONY
// Encompassed OS/2 in conditional definitions for Quake 4 migration
//
//    Rev 1.53   30 Aug 1994 13:40:38   KEITH
// Made comments valid for C again.
//
//    Rev 1.52   30 Aug 1994 12:15:10   BRIANF
// Fixed Geo comments.
//
//    Rev 1.51   25 Aug 1994 14:23:50   DLEVITON
// Use LPVOID for data in Reg calls.
//
//    Rev 1.50   24 Aug 1994 15:27:42   DLEVITON
// ConfigReg calls.
//
//    Rev 1.49   03 Aug 1994 17:14:16   MFALLEN
// Added LFN support.
//
//    Rev 1.48   22 Jul 1994 14:33:00   BRAD
// Added VXD platform
//
//    Rev 1.47   25 May 1994 16:24:26   GVOGEL
// Changed @Library to @Include in comment blocks for Geologist.
//
//    Rev 1.46   20 May 1994 04:17:38   GVOGEL
// Added comment blocks from Geologist project.
//
//    Rev 1.45   26 Apr 1994 16:36:12   BRAD
// Removed PVCS Logs
/************************************************************************/

/*@Library: SYMKRNL*/

#ifndef INCL_NCFG
#define INCL_NCFG

#ifdef  __cplusplus
extern "C" {
#endif

#define IS_WHITE(c)             ((c) == ' ' || (c) == '\t')
#define IS_EOL(c)               ((c) == 0x0D || (c) == 0x0A)


#define MAX_INI_SECTION         256
#ifdef SYM_DOS
#define MAX_INI_LINE            512
#else
#define MAX_INI_LINE            256
#endif

// Error Values
// ------------------------------------------------------------------------
#define CFG_SUCCESS             0
#define CFG_FILE_ERROR          1
#define CFG_MEMORY_ERROR        2
#define CFG_BLOCK_NOT_FOUND     3
#define CFG_CHECKSUM_FAILED     4
#define CFG_BLOCK_IS_SMALLER    5
#define CFG_BLOCK_IS_LARGER     6
#define CFG_FILE_NOT_FOUND      7
#define CFG_BAD_ID              8               // returned by SYM_NLM wrapper
                                                // ConstructConfigId
#define CFG_FLUSH_ERROR         9               // returned by ConfigFlush()
#define CFG_ERROR_FIRST         CFG_FILE_ERROR
#define CFG_ERROR_LAST          CFG_BAD_ID


// Switch Return Values
// -----------------------------------------------------------------------
#define CFG_SWITCH_NOT_SET      0               // switch wasn't set
#define CFG_SWITCH_SET          1               // switch was set
#define CFG_SWITCH_PLUS         2               // switch was set and '+' added
#define CFG_SWITCH_MINUS        3               // switch was set and '-' added


// ConfigSetState() values
// -----------------------------------------------------------------------
#define CFG_SET_NORMAL          0               // clear '+' or '-' from entry
#define CFG_SET_PLUS            1               // set entry to '+'
#define CFG_SET_MINUS           2               // set entry to '-'

// Flags for use with ConfigInit-ConfigTerminate
// -----------------------------------------------------------------------

#define CFG_NO_CREATE           0x0040  // don't create if file doesn't exist
#define CFG_FORCE_CREATE        0x0080  // override the CFG_NO_CREATE flag
#define CFG_FULLPATH_FILE       0x0100  // file is fully qualified path
#define CFG_FLOPPY_CREATE       0x0200  // create files on floppies
#define CFG_NO_CRC_CHECKS       0x0400  // disable CRC checking

// Internal Flags - Not to be used for ConfigInit-ConfigTerminate
// -----------------------------------------------------------------------

#define CFG_LOCAL_FILE          0x0001  // file is local
#define CFG_GLOBAL_FILE         0x0002  // file is global
#define CFG_TEXT_FILE           0x0004  // file is a text file
#define CFG_BINARY_FILE         0x0008  // file is a binary file
#define CFG_GLOBAL_FIRST        0x0010  // search global file first
#define CFG_LOCAL_FIRST         0x0020  // search local file first

#define CFG_LOCAL_TEXT          (CFG_LOCAL_FILE | CFG_TEXT_FILE)
#define CFG_LOCAL_BINARY        (CFG_LOCAL_FILE | CFG_BINARY_FILE)
#define CFG_GLOBAL_TEXT         (CFG_GLOBAL_FILE | CFG_TEXT_FILE)
#define CFG_GLOBAL_BINARY       (CFG_GLOBAL_FILE | CFG_BINARY_FILE)
#define CFG_FULLPATH_TEXT       (CFG_FULLPATH_FILE | CFG_TEXT_FILE)
#define CFG_FULLPATH_BINARY     (CFG_FULLPATH_FILE | CFG_BINARY_FILE)


// Command Line Parsing
// -----------------------------------------------------------------------

#define CFG_MAX_ARGS            32

#define CFG_SWITCH_ARG          0
#define CFG_PARAM_ARG           1

//------------------------------------------------------------------------
//  Defines that should be used with the CFGCMDLINEDATA.wFlags field.
//
//  IMPORTANT: if you enable long file name processing it will only work
//             if the the long file names are fully qualified. Otherwise
//             it is not possible to determine where the separation between
//             arguments is.
//------------------------------------------------------------------------

#define CFG_FLAG_ENABLE_LFN     0x0001  // Enable LFN command line processing
#define CFG_FLAG_RESERVED_01    0x0002  // Not used at present time.
#define CFG_FLAG_RESERVED_02    0x0004  // Not used at present time.
#define CFG_FLAG_RESERVED_03    0x0008  // Not used at present time.
#define CFG_FLAG_RESERVED_04    0x0010  // Not used at present time.
#define CFG_FLAG_RESERVED_05    0x0020  // Not used at present time.
#define CFG_FLAG_RESERVED_06    0x0040  // Not used at present time.
#define CFG_FLAG_RESERVED_07    0x0080  // Not used at present time.
#define CFG_FLAG_RESERVED_08    0x0100  // Not used at present time.
#define CFG_FLAG_RESERVED_09    0x0200  // Not used at present time.
#define CFG_FLAG_RESERVED_10    0x0400  // Not used at present time.
#define CFG_FLAG_RESERVED_11    0x0800  // Not used at present time.
#define CFG_FLAG_RESERVED_12    0x1000  // Not used at present time.
#define CFG_FLAG_RESERVED_13    0x2000  // Not used at present time.
#define CFG_FLAG_RESERVED_14    0x4000  // Not used at present time.
#define CFG_FLAG_RESERVED_15    0x8000  // Not used at present time.

//------------------------------------------------------------------------
// 08/02/94 MARTIN, Changed the hard coded szLineBuffer[128] to
//                  szLineBuffer[SYM_MAX_PATH].
//                  Added the wFlags field.
//------------------------------------------------------------------------
typedef struct CFGCMDLINEDATA
    {
    UINT    wSwitches;                  // switch count
    UINT    wParameters;                // parameter count
    LPSTR   lpLineBuffer;               // local copy of the command line
    UINT    argc;                       // argument count
    LPSTR   argv [CFG_MAX_ARGS];        // argument list
    BYTE    argt [CFG_MAX_ARGS];        // argument types
    WORD    wFlags;                     // How to process command line params
    } CFGCMDLINEDATA;

typedef const CFGCMDLINEDATA FAR *LPCFGCMDLINEDATA;

// Size definitions

#define BLOCK_ID_SIZE       8   // 8-letter limit on block id names
#define BLOCK_ID_STRING_SIZE (BLOCK_ID_SIZE+1)

// Macro to allow strings to be used for block ID's
// -----------------------------------------------------------------------
#define MAKEBLOCKID(s)          (DWORD)(LPBYTE)(s)

/*@STRUCTAPI:*************************************************************/
typedef struct CFGBLOCKREC
    {
    DWORD       dwBlockID;
    WORD        wSize;
    LPVOID      lpData;
    WORD        wFlags;
    WORD        wSource;
    } CFGBLOCKREC;
/*****
@Name: CFGBLOCKREC

@Description:
This structure is used to perform block (or binary) operations on a
configuration file.

@Members:
$dwBlockID$ Specifies the ID of the block to retrieve or store data from or to.

$wSize$ Specifies the size of the data block.

$lpData$ Specifies a buffer to receive a data block on retrieval operations
and the block to be written otherwise.

$wFlags$ Specifies any necessary flags. [[What values can this take and what
do they mean?]]

$wSource$ [[What's this?]]

@Include: symcfg.h
******************************************************************************/

typedef const CFGBLOCKREC FAR *LPCFGBLOCKREC;

/*@STRUCTAPI:*************************************************************/
typedef struct CFGTEXTREC
    {
    LPSTR       lpSwitch;
    WORD        wFlags;
    LPSTR       lpSection;
    LPSTR       lpEntry;
    WORD        wSource;
    } CFGTEXTREC;
/*****
@Name: CFGTEXTREC

@Description:
This structure is used to perform text (ini) operations on a
configuration file.

@Members:
$lpSwitch$ Pointer to NULL-terminated string that specifies the minimum
text of the command line switch, not including the switch character. May be NULL
for no command line switch search.

$wFlags$ Bit flags which control the interpretation of the structure.
    ~CFG_SW_MINIMUM~    lpSwitch designates minimum text, but the command line
                        may contain more. This is the preferred setting.
    ~CFG_SW_EXACT~      lpSwitch must exactly match the text on the command line.
    ~CFG_FL_PREPEND~    ?
    ~CFG_FL_APPEND~     ?

$lpSection$ Corresponds to lpSection in Windows WritePrivateProfile.

$lpEntry$ Corresponds to lpEntry in Windows WritePrivateProfile.

$wSource$ [[What's this?]]

@Include: symcfg.h
******************************************************************************/

typedef const CFGTEXTREC FAR *LPCFGTEXTREC;

// Flags used in CFGTEXTRECs

#define CFG_SW_MINIMUM          0x0000  // switch text matches prefix
#define CFG_SW_EXACT            0x0001  // switch text matches exactly
#define CFG_SW_PREPEND          0x0002  //
#define CFG_SW_APPEND           0x0004  //
#define CFG_INI_MULTIPLE        0x0040  // allow multiple keyword entry creates
#define CFG_INI_FINDMINIMUM     0x0008  // keyname text matches prefix
#define CFG_INI_FINDEXACT       0x0010  // keyname text matches exactly
#define CFG_INI_FINDFILE        0x0020  // keyname text contains filename

// ------------------------------------------------------------------------
// File Record -
// NOTE: This structure is NOT written to disk, and therefore doesn't need
//       to have a fixed size.
// ------------------------------------------------------------------------
/*@STRUCTAPI:**************************************************************/
typedef struct CFGFILE
    {
    WORD        wFlags;                 // flags
    LPBYTE      lpBaseName;             // desired filename for the file
    BOOL        bExists;                // TRUE if file exists
    WORD        wClients;               // number of clients
    BOOL        bDirty;                 // TRUE if file was modified
    HGLOBAL     hMem;                   // memory block for the file
    DWORD       dwSize;                 // size of the file
    WORD        wError;                 // initialization error
    char        szPath [SYM_MAX_PATH+1];// full pathname of the file
    } CFGFILE, FAR *LPCFGFILE;
/*****
@Name: CFGFILE

@Description:
This structure exists for each config file. It maintains
information on the filename, statistics, and associated memory blocks.

@Members:
$wFlags$ Specifies any necessary flags. [[What values can this take and what
do they mean?]]

$lpBaseName$ Specifies the desired filename for the file.

$bExists$ This is set to TRUE if the file exists.

$wClients$ Specifies the number of clients.

$bDirty$ This is set to TRUE if the file was modified.

$hMem$ Specifies the memory block to use for the file.

$dwSize$ Specifies the size of the file.

$wError$ Specifies whether was an initialization error.

$szPath$ Specifies the full pathname of the configuration file.

@Include: symcfg.h
*****************************************************************************/

// ------------------------------------------------------------------------
// File Header -
// If you want to change the following structure in any way, you must leave at
// least the first two fields intact.  This will guarantee that any version of
// NCFG can navigate files created by any other version.
// ------------------------------------------------------------------------

/*@STRUCTAPI:****************************************************************/
typedef struct CFGFILEHEADER
    {
    WORD        wHeaderSize;            // size of this structure
    WORD        wTotalBlocks;           // number of blocks in the file
    char        szSignature[8];         // signature
    WORD        wCreatorID;             // creator ID
    WORD        wVersion;               // version number
    WORD        wCheckSum;              // checksum for the file
    SHORTBOOL   bCheckCRC;              // FALSE to disable CRC check/write
    char        reserved[12];           // unused space
    } CFGFILEHEADER, FAR *LPCFGFILEHEADER;
/*****
@Name: CFGFILEHEADER

@Description:
This structure is the file header for binary configuration files. It used to
store and retrieve data to and from configuration files.

@Members:
$wHeaderSize$ Specifies the size of this structure.

$wTotalBlocks$ Specifies the number of blocks in the file.

$szSignature$ Specifies the file's signature.

$wCreatorID$ Specifies the ID of the configuration file's creator.

$wVersion$ Specifies the version number of the file.

$wCheckSum$ Specifies the checksum for the file.

$bCheckCRC$ Set to FALSE to disable the CRC check. For write operations, FALSE
disables the calculation and storage of a CRC value.

$reserved$ This is currently unused space.

@Include: symcfg.h
******************************************************************************/

// ---------------------------------------------------------------------------
// Item Header -
// As with the file header, if this structure is changed,
// the first two fields should not be changed or moved.
// ---------------------------------------------------------------------------
/*@STRUCTAPI:*****************************************************************/
typedef struct CFGITEMHEADER
    {
    WORD        wHeaderSize;            // size of this structure
    WORD        wDataSize;              // size of data following structure
    WORD        wItemID;                // type of item
    WORD        wCreatorID;             // creator ID
    WORD        wVersion;               // version number
    WORD        wFlags;                 // flags
    WORD        wBlockNum;              // the block sequence number
    WORD        wCRC;                   // checksum for the block data
                                        // name or ID value
    char        szBlockID[BLOCK_ID_SIZE];
    SHORTBOOL   bCheckCRC;              // FALSE to disable CRC check/write
    char        reserved[6];            // unused space
    } CFGITEMHEADER, FAR *LPCFGITEMHEADER;
/*****
@Name: CFGITEMHEADER

@Description:
This structure is the header for individual blocks inside the binary
configuration file.  The first item header directly follows the file
header. \n

Linking to the next item header should always be done using the size
values in the header, not by using the sizeof operator.

@Members:
$wHeaderSize$ Specifies the size of this structure.

$wDataSize$ Indicates the size of data following structure.

$wItemID$ Indicates what type of item.

$wCreatorID$ Specifies the ID of the creator.

$wVersion$ Specifies the version number.

$wFlags$ Specifies any necessary flags. [[What values can this take and what
do they mean?]]

$wBlockNum$ Specifies the block sequence number.

$wCRC$ Specifies the checksum for the block data.

$szBlockID$ Specifies the name or ID of the block to write or retrieve.

$bCheckCRC$ Set to FALSE to disable the CRC check. For write operations, FALSE
disables the calculation and storage of a CRC value.

$reserved$ This is currently unused space.

@See: CFGFILEHEADER
@Include: symcfg.h
******************************************************************************/

// Item Type IDs - This field is mainly for future use.  It is intialized
// but not used.
// -----------------------------------------------------------------------
#define NCFG_STD_BLOCK          0x4445  // standard block ID 'ED'


// Creator and version IDs
// -----------------------------------------------------------------------
#define NCFG_DOS                0x0100          // MSDOS
#define NCFG_WIN                0x0200          // Windows 3.x
#define NCFG_WIN32              0x0300          // Windows/NT, Chicago
#define NCFG_OS2                0x0400          // OS/2
#define NCFG_NLM                0x0500          // NLM
#define NCFG_VXD                0x0600          // VXD
#define NCFG_NTK                0x0700          // NT Kernel mode

#if defined(SYM_WIN16)
    #define NCFG_CREATOR            NCFG_WIN
#elif defined(SYM_WIN32)
    #define NCFG_CREATOR            NCFG_WIN32
#elif defined(SYM_NLM)
    #define NCFG_CREATOR            NCFG_NLM
#elif defined (SYM_DOS)
    #define NCFG_CREATOR            NCFG_DOS
#elif defined (SYM_OS2)
    #define NCFG_CREATOR            NCFG_OS2
#elif defined (SYM_VXD)
    #define NCFG_CREATOR            NCFG_VXD
#elif defined (SYM_NTK)
    #define NCFG_CREATOR            NCFG_NTK
#else
    #error No recognized SYM_ platform defined
#endif

#define NCFG_VERSION            0x0200          // 2.00


// Block Flags
// -----------------------------------------------------------------------
#define CFG_BLK_IS_NAMED        0x0001  // block has a name, not an ID

// Windows (32-bit) Registry Support
// --------------------------------------------------------------------------
/*@STRUCTAPI:*****************************************************************/
#if !defined(SYM_WIN)
#if !defined(HKEY)
	#define HKEY	DWORD
#endif
#if !defined(ERROR_SUCCESS)
	#define ERROR_SUCCESS	0
#endif
#if !defined(REG_SZ)
	#define REG_SZ			1
#endif
#endif	// SYM_DOS
typedef struct CFGREGKEY
    {
    HKEY        hkey;
    LPCTSTR     lpszProduct;
    LPCTSTR     lpszVersion;
    LPCTSTR     lpszProject;
    LPCTSTR     lpszSection;
    LPCTSTR     lpszEntry;
    LPCTSTR     lpszName;               // WIN32 only
    LPCTSTR     lpszSwitch;
    } CFGREGKEY, FAR * LPCFGREGKEY;
/*****
@Name: CFGREGKEY

@Description:
This structure collects the components of a Symantec standard registry
key. We view the registry as a straightforward extension of the data hierarchy
of configuration (INI) files. This makes it easier to comply with the
convention and reduces the number of parameters required by the registry
functions. \n

When Symkrnl constructs a key from a CFGREGKEY, it prepends "Software\Symantec"
if hkey is HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE. Then it concatenates lpszProduct,
lpszVer3ion, lpszProject, lpszSection, and lpszEntry, separated by backslashes.
Any of these may be NULL or empty, and such entries and their corresponding
backslashes are omitted.

@Members:
$hkey$ Identifies a currently open key or any of the following predefined
    reserved handle values:

~HKEY_CLASSES_ROOT~
~HKEY_CURRENT_USER~
~HKEY_LOCAL_MACHINE~
~HKEY_USERS~

$lpszProduct$ Should be filled in by a static string which has been initialized
    with a global #define for the entire product, containing the product name.

$lpszVersion$ Should be filled in by a static string which has been initialized
    with a global #define for the entire product, containing the product version.

$lpszProject$ Should be filled in by a static string which has been initialized
    with a project global #define for the entire project or program, containing
    the project or program name.

$lpszSection$ Should be filled in by a static string containing the section of
    the data.

$lpszEntry$ Should be filled in by a static string containing the name of
    the data entry.

$lpszName$ Should be filled in by a static string containing the name of
    the value of the data entry.

$lpszSwitch$ If not NULL, a command line switch which overrides the registry in the
    ConfigRegQueryValue call.

@See: ConfigRegQueryValue
@Include: symcfg.h
******************************************************************************/

// Prototypes
// --------------------------------------------------------------------------

//      SYMCFG.C

void SYM_EXPORT WINAPI ConfigInit (UINT wFlags);
UINT SYM_EXPORT WINAPI ConfigTerminate (UINT wFlags);
UINT SYM_EXPORT WINAPI ConfigFileInit (LPCFGFILE lpConfig, UINT wFlags);
UINT SYM_EXPORT WINAPI ConfigFileTerminate (LPCFGFILE lpConfig, UINT wFlags);
UINT SYM_EXPORT WINAPI ConfigSearchSymantecPath (LPCSTR lpSearchName, LPSTR lpReturnName);
UINT SYM_EXPORT WINAPI ConfigSearchVarPath (LPCSTR lpFirstVar,
                                LPCSTR lpSecondVar, LPCSTR lpSearchName,
                                LPSTR lpReturnName, BOOL bForceVariable);
void SYM_EXPORT WINAPI ConfigNewSymantecPath (LPSTR lpReturnName, LPCSTR lpTargetName);
void SYM_EXPORT WINAPI ConfigNewVarPath (LPCSTR lpFirstVar, LPCSTR lpSecondVar,
                                LPSTR lpReturnName, LPCSTR lpTargetName,
                                LPCSTR lpNeighbor);
UINT SYM_EXPORT WINAPI ConfigSearchPath (LPSTR lpReturnPath, LPCSTR lpFileToFind);
UINT SYM_EXPORT WINAPI ConfigGetSymantecDir (LPSTR lpDirName);
WORD SYM_EXPORT WINAPI ConfigGetCRC (const void FAR *lpData, WORD wLength);
UINT SYM_EXPORT WINAPI ConfigFileFlush (LPCFGFILE lpConfig, UINT wFlags);
UINT SYM_EXPORT WINAPI ConfigFlush (UINT wFlags);
void SYM_EXPORT WINAPI ConfigDisableCRC(void);
void SYM_EXPORT WINAPI ConfigEnableCRC(void);

// CFG_BLOC.C

UINT           WINAPI ConfigBlockInit (LPCFGFILE lpConfig);
UINT           WINAPI ConfigBlockTerminate (LPCFGFILE lpConfig);
UINT SYM_EXPORT WINAPI ConfigBlockGet (LPCFGBLOCKREC lpConfig);
UINT SYM_EXPORT WINAPI ConfigBlockSet (LPCFGBLOCKREC lpConfig);
UINT SYM_EXPORT WINAPI ConfigBlockFileGet (LPCFGFILE lpFile, LPCFGBLOCKREC lpBlockRec);
UINT SYM_EXPORT WINAPI ConfigBlockFileSet (LPCFGFILE lpFile, LPCFGBLOCKREC lpBlockRec);
UINT SYM_EXPORT WINAPI ConfigGetBlockInfo (DWORD dwBlockID,
                                LPCFGFILEHEADER lpHeader,
                                LPCFGITEMHEADER lpBlock);
UINT SYM_EXPORT WINAPI ConfigGetBlockFileInfo (LPCFGFILE lpFile,
                                DWORD dwBlockID,
                                LPCFGFILEHEADER lpFileHeader,
                                LPCFGITEMHEADER lpBlock);
UINT SYM_EXPORT WINAPI ConfigBlockFileIdChange (
                                LPCFGFILE       lpFile,
                                DWORD           dwOldID,
                                DWORD           dwNewID);

        // diagnostic functions for binary config files

        // return TRUE to continue, FALSE to stop
typedef BOOL (CALLBACK *CFGENUMPROC)(LPCFGFILEHEADER, LPCFGITEMHEADER);

UINT SYM_EXPORT WINAPI ConfigLocalEnumBlocks (CFGENUMPROC lpProc);
UINT SYM_EXPORT WINAPI ConfigGlobalEnumBlocks (CFGENUMPROC lpProc);
UINT SYM_EXPORT WINAPI ConfigFileEnumBlocks (LPCFGFILE lpFile, CFGENUMPROC lpProc);


// CFG_SWIT.C

void SYM_EXPORT WINAPI ConfigProcessCmdLine (LPSTR lpLineBuffer);
UINT SYM_EXPORT WINAPI ConfigParamCount (void);
UINT SYM_EXPORT WINAPI ConfigSwitchCount (void);
UINT SYM_EXPORT WINAPI ConfigParamGet (UINT wParam, LPSTR lpBuffer, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigParamToUpper (void);
BOOL SYM_EXPORT WINAPI ConfigSwitchSet (LPCFGTEXTREC lpConfig, UINT FAR *lpwResult);
BOOL SYM_EXPORT WINAPI ConfigSwitchGetValue (LPCFGTEXTREC lpConfig, UINT FAR *lpwValue);
BOOL SYM_EXPORT WINAPI ConfigSwitchGetLongValue (LPCFGTEXTREC lpConfig, DWORD FAR *lpdwValue);
BOOL SYM_EXPORT WINAPI ConfigSwitchGetText (LPCFGTEXTREC lpConfig, LPSTR lpBuffer, UINT wSize);

void SYM_EXPORT WINAPI ConfigProcessCmdLineEx (CFGCMDLINEDATA FAR *lpLineData, LPCSTR lpLineBuffer);
UINT SYM_EXPORT WINAPI ConfigParamCountEx (LPCFGCMDLINEDATA lpLineData);
UINT SYM_EXPORT WINAPI ConfigSwitchCountEx (LPCFGCMDLINEDATA lpLineData);
UINT SYM_EXPORT WINAPI ConfigParamGetEx (LPCFGCMDLINEDATA lpLineData, UINT wParam, LPSTR lpBuffer, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigParamToUpperEx (LPCFGCMDLINEDATA lpLineData);
BOOL SYM_EXPORT WINAPI ConfigSwitchSetEx (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig, UINT FAR *lpwResult);
BOOL SYM_EXPORT WINAPI ConfigSwitchGetValueEx (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig, UINT FAR *lpwValue);
BOOL SYM_EXPORT WINAPI ConfigSwitchGetLongValueEx (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig, DWORD FAR *lpdwValue);
BOOL SYM_EXPORT WINAPI ConfigSwitchGetTextEx (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig, LPSTR lpBuffer, UINT wSize);

// Convert all non-DBCS characters on the command line parameters to upper case.
UINT SYM_EXPORT WINAPI ConfigParamToUpperNonDbcs (void);

VOID SYM_EXPORT WINAPI ConfigProcessCmdLineEnd (
    CFGCMDLINEDATA FAR *lpLineData      // [in] pointer to use for cleanup
    );

// CFG_INI.C

UINT SYM_EXPORT WINAPI ConfigTextInit (LPCFGFILE lpConfig);
UINT SYM_EXPORT WINAPI ConfigIniGet (LPCFGTEXTREC lpConfig, LPSTR lpData, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigIniSet (LPCFGTEXTREC lpConfig, LPSTR lpData);
UINT SYM_EXPORT WINAPI ConfigIniFileGet (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig,
                                LPSTR lpData,
                                UINT wSize);
UINT SYM_EXPORT WINAPI ConfigIniFileSet (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig,
                                LPSTR lpData);
UINT SYM_EXPORT WINAPI ConfigMinimizeText (LPSTR lpText);

UINT SYM_EXPORT WINAPI ConfigIniQuerySection (LPCFGTEXTREC lpConfig);
UINT SYM_EXPORT WINAPI ConfigIniFileQuerySection (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig);

UINT SYM_EXPORT WINAPI ConfigIniGetEx (LPCFGTEXTREC lpConfig, LPSTR lpMatch,
                                        LPSTR lpData, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigIniFileGetEx (LPCFGFILE lpFile, LPCFGTEXTREC lpConfig,
                                LPSTR lpMatch, LPSTR lpData, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigIniSetEx (LPCFGTEXTREC lpConfig,
                                LPSTR lpMatch, LPSTR lpData);
UINT SYM_EXPORT WINAPI ConfigIniFileSetEx (LPCFGFILE lpFile, LPCFGTEXTREC lpConfig,
                                LPSTR lpMatch, LPSTR lpData);


// CFG_TEXT.C

UINT SYM_EXPORT WINAPI ConfigReadValue (LPCFGTEXTREC lpConfig,
                                UINT FAR *lpwValue, UINT FAR *lpwState);
UINT SYM_EXPORT WINAPI ConfigFileReadValue (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig,
                                UINT FAR *lpwValue, UINT FAR *lpwState);
UINT SYM_EXPORT WINAPI ConfigReadValueArray (LPCFGTEXTREC lpConfig,
                                UINT FAR *lpwValue, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigFileReadValueArray (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig,
                                UINT FAR *lpwValue, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigReadString (LPCFGTEXTREC lpConfig,
                                LPSTR lpBuffer, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigFileReadString (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig,
                                LPSTR lpBuffer, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigWriteString (LPCFGTEXTREC lpConfig, LPSTR lpBuffer);
UINT SYM_EXPORT WINAPI ConfigFileWriteString (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig, LPSTR lpBuffer);
UINT SYM_EXPORT WINAPI ConfigWriteValue (LPCFGTEXTREC lpConfig, UINT wValue);
UINT SYM_EXPORT WINAPI ConfigFileWriteValue (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig, UINT wValue);
UINT SYM_EXPORT WINAPI ConfigWriteValueArray (LPCFGTEXTREC lpConfig,
                                UINT FAR * lpwValue, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigFileWriteValueArray (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig,
                                UINT FAR * lpwValue, UINT wSize);
UINT SYM_EXPORT WINAPI ConfigSetState (LPCFGTEXTREC lpConfig, UINT wState);
UINT SYM_EXPORT WINAPI ConfigFileSetState (LPCFGFILE lpFile,
                                LPCFGTEXTREC lpConfig, UINT wState);

// CFG_REG.C
LONG SYM_EXPORT WINAPI ConfigRegQueryValue( LPCFGREGKEY lpCfgRegKey,
    LPDWORD lpdwReserved, LPDWORD lpdwType, LPVOID lpbData, LPDWORD lpcbData);
LONG SYM_EXPORT WINAPI ConfigRegEnumValue (
    LPCFGREGKEY lpCfgRegKey,            // specifies data entry to query
    LPDWORD lpdwReserved,               // reserved
    DWORD   dwIndex,			// Index
    LPDWORD lpdwType,                   // address of buffer for value type
    LPVOID  lpbData,                    // address of data buffer
    LPDWORD  lpcbData );                // address of data buffer size
LONG SYM_EXPORT WINAPI ConfigRegSetValue ( LPCFGREGKEY lpCfgRegKey, DWORD dwReserved,
    DWORD fdwType, CONST LPVOID lpbData, DWORD cbData );
LONG SYM_EXPORT WINAPI ConfigRegDeleteValue ( LPCFGREGKEY lpCfgRegKey );
LONG SYM_EXPORT WINAPI ConfigRegDeleteKey ( LPCFGREGKEY lpCfgRegKey );
LONG SYM_EXPORT WINAPI ConfigRegDeleteTree ( LPCFGREGKEY lpCfgRegKey );
HKEY SYM_EXPORT WINAPI ConfigOpenRegistryKey( LPCFGREGKEY lpCfgRegKey, BOOL fWithEntry );
VOID SYM_EXPORT WINAPI ConfigCloseRegistryKey( HKEY hKey );

// CFG_PROF.C
int SYM_EXPORT WINAPI SymGetPrivateProfileString(
    LPCSTR      lpszSection,
    LPCSTR      lpszEntry,
    LPCSTR      lpszDefault,
    LPSTR       lpszReturnBuffer,
    int         cbReturnBuffer,
    LPCSTR      lpszFileName
);

BOOL SYM_EXPORT WINAPI SymWritePrivateProfileString(
    LPCSTR      lpszSection,
    LPCSTR      lpszEntry,
    LPCSTR      lpszString,
    LPCSTR      lpszFileName
);

UINT SYM_EXPORT WINAPI SymGetPrivateProfileInt(
    LPCSTR      lpszSection,
    LPCSTR      lpszEntry,
    int         nDefault,
    LPCSTR      lpszFileName
);

LPVOID SYM_EXPORT WINAPI CreateINICache(char far *pszFileName);

int SYM_EXPORT WINAPI GetCacheEntry( LPSTR lpszSection, LPSTR lpszEntry,
                                     LPSTR lpszDefault, LPSTR lpszReturnBuffer,
                                     int cbReturnBuffer, void far *pCacheHandle );

void SYM_EXPORT WINAPI ReleaseINICache(void far *pCacheHandle);

DWORD SYM_EXPORT WINAPI GetCacheSection(
    LPCSTR   lpszSection,
    LPSTR    lpReturnString,
    DWORD    dwSize,
    void far *pCacheHandle
);

DWORD SYM_EXPORT WINAPI GetCacheSectionNames(
    LPSTR    lpReturnString,
    DWORD    dwSize,
    void far *pCacheHandle
);


#ifdef  __cplusplus
}
#endif

#endif

