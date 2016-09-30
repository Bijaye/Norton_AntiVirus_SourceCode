// Copyright 1992-1993 Symantec Corporation
/************************************************************************
 *                                                                      *
 * $Header:   I:/INCLUDE/VCS/SYMCFG.H_V   1.3   09 Mar 1993 12:07:00   CRAIG  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   I:/INCLUDE/VCS/SYMCFG.H_V  $ *
 * 
 *    Rev 1.3   09 Mar 1993 12:07:00   CRAIG
 * Change LPCSTR to LPSTR.
 *
 *    Rev 1.2   09 Mar 1993 12:01:10   CRAIG
 * Update from Quake: bCheckCRC, etc.
 ************************************************************************/

#ifndef INCL_NCFG
#define INCL_NCFG

#ifdef  __cplusplus
extern "C" {
#endif

#define IS_WHITE(c)             ((c) == ' ' || (c) == '\t')
#define IS_EOL(c)               ((c) == 0x0D || (c) == 0x0A)


#define MAX_INI_SECTION         30
#define MAX_INI_LINE            80

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
#define CFG_BAD_ID              8               // returned by NLM wrapper
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
#define CFG_FORCE_CREATE	0x0080  // override the CFG_NO_CREATE flag
#define CFG_FULLPATH_FILE       0x0100  // file is fully qualified path
#define CFG_FLOPPY_CREATE       0x0200  // create files on floppies

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

// Return values for ConfigIniQuerySection
// -----------------------------------------------------------------------
#define CFG_SECTION_NOT_FOUND           0
#define CFG_SECTION_IS_EMPTY            1
#define CFG_SECTION_HAS_ENTRIES         2


// Macro to allow strings to be used for block ID's
// -----------------------------------------------------------------------
#define MAKEBLOCKID(s)          (DWORD)(LPBYTE)(s)


typedef struct
    {
    DWORD       dwBlockID;
    WORD        wSize;
    LPVOID      lpData;
    WORD        wFlags;
    WORD        wSource;
    } CFGBLOCKREC;

typedef const CFGBLOCKREC FAR *LPCFGBLOCKREC;

typedef struct
    {
    LPSTR      lpSwitch;
    WORD        wFlags;
    LPSTR      lpSection;
    LPSTR      lpEntry;
    WORD        wSource;
    } CFGTEXTREC;

typedef const CFGTEXTREC FAR *LPCFGTEXTREC;


#define CFG_SW_MINIMUM          0x0000
#define CFG_SW_EXACT            0x0001
#define CFG_SW_PREPEND          0x0002
#define CFG_SW_APPEND           0x0004


// ------------------------------------------------------------------------
// File Record - Each config file has one of these records, which maintain
// information on the file name, stats, and associated memory blocks.
// ------------------------------------------------------------------------

typedef struct
    {
    WORD        wFlags;                         // flags
    LPBYTE      lpBaseName;                     // desired filename for the file
    WORD        bExists;                        // TRUE if file exists
    WORD        wClients;                       // number of clients
    WORD        bDirty;                         // TRUE if file was modified
    HGLOBAL     hMem;                           // memory block for the file
    DWORD       dwSize;                         // size of the file
    BYTE        szPath [MAX_PATH_NAME];         // full pathname of the file
    WORD        wError;                         // initialization error
    } CFGFILE, FAR *LPCFGFILE;

// ------------------------------------------------------------------------
// File Header - This is the file header for the binary config files.  If
// you want to change this structure in any way, you must leave at least
// the first two fields intact.  This will guarantee that any version of
// NCFG can navigate files created by any other version.
// ------------------------------------------------------------------------

typedef struct
    {
    WORD        wHeaderSize;            // size of this structure
    WORD        wTotalBlocks;           // number of blocks in the file
    BYTE        szSignature[8];         // signature
    WORD        wCreatorID;             // creator ID
    WORD        wVersion;               // version number
    WORD        wCheckSum;              // checksum for the file
    WORD        bCheckCRC;              // FALSE to disable CRC check/write
    BYTE        reserved[12];           // unused space
    } CFGFILEHEADER, FAR *LPCFGFILEHEADER;

// ---------------------------------------------------------------------------
// Item Header - This is the header for individual blocks inside the binary
// configuration file.  The first item header directly follows the file
// header (see above).  As with the file header, if this structure is changed,
// the first two fields should not be changed or moved.
//
// Linking to the next item header should always be done using the size
// values in the header, not by using the sizeof operator.
// ---------------------------------------------------------------------------

typedef struct
    {
    WORD        wHeaderSize;            // size of this structure
    WORD        wDataSize;              // size of data following structure
    WORD        wItemID;                // type of item
    WORD        wCreatorID;             // creator ID
    WORD        wVersion;               // version number
    WORD        wFlags;                 // flags
    WORD        wBlockNum;              // the block sequence number
    WORD        wCRC;                   // checksum for the block data
    BYTE        szBlockID[8];           // name or ID value
    WORD        bCheckCRC;              // FALSE to disable CRC check/write
    BYTE        reserved[6];            // unused space
    } CFGITEMHEADER, FAR *LPCFGITEMHEADER;


// Item Type IDs - This field is mainly for future use.  It is intialized
// but not used.
// -----------------------------------------------------------------------
#define NCFG_STD_BLOCK          0x4445  // standard block ID 'ED'


// Creator and version IDs
// -----------------------------------------------------------------------
#define NCFG_DOS                0x0100          // MSDOS
#define NCFG_WIN                0x0200          // Windows 3.x
#define NCFG_NT                 0x0300          // Windows/NT
#define NCFG_OS2                0x0400          // OS/2
#define NCFG_NLM                0x0500          // NLM

#ifdef  WIN16
#define NCFG_CREATOR            NCFG_WIN
#else
#ifdef  NLM
#define NCFG_CREATOR            NCFG_NLM
#else
#define NCFG_CREATOR            NCFG_DOS
#endif  // NLM
#endif  // WIN16

#define NCFG_VERSION            0x0100          // 1.00


// Block Flags
// -----------------------------------------------------------------------
#define CFG_BLK_IS_NAMED        0x0001  // block has a name, not an ID


// Prototypes
// --------------------------------------------------------------------------

//      SYMCFG.C

void     WINAPI ConfigInit (UINT wFlags);
void     WINAPI ConfigTerminate (UINT wFlags);
UINT     WINAPI ConfigFileInit (LPCFGFILE lpConfig, UINT wFlags);
UINT     WINAPI ConfigFileTerminate (LPCFGFILE lpConfig, UINT wFlags);
UINT     WINAPI ConfigSearchSymantecPath (LPSTR lpSearchName, LPSTR lpReturnName);
UINT     WINAPI ConfigSearchVarPath (LPSTR lpFirstVar,
                                LPSTR lpSecondVar, LPSTR lpSearchName,
                                LPSTR lpReturnName, BOOL bForceVariable);
void     WINAPI ConfigNewSymantecPath (LPSTR lpReturnName, LPSTR lpTargetName);
void     WINAPI ConfigNewVarPath (LPSTR lpFirstVar, LPSTR lpSecondVar,
                                LPSTR lpReturnName, LPSTR lpTargetName,
                                LPSTR lpNeighbor);
UINT     WINAPI ConfigSearchPath (LPSTR lpReturnPath, LPSTR lpFileToFind);
UINT     WINAPI ConfigGetSymantecDir (LPSTR lpDirName);
UINT     WINAPI ConfigGetCRC (LPSTR lpData, UINT wLength);
UINT     WINAPI ConfigFileFlush (LPCFGFILE lpConfig, UINT wFlags);
UINT     WINAPI ConfigFlush (UINT wFlags);

// CFG_BLOC.C

UINT           WINAPI ConfigBlockInit (LPCFGFILE lpConfig);
UINT           WINAPI ConfigBlockTerminate (LPCFGFILE lpConfig);
UINT     WINAPI ConfigBlockGet (LPCFGBLOCKREC lpConfig);
UINT     WINAPI ConfigBlockSet (LPCFGBLOCKREC lpConfig);
UINT     WINAPI ConfigBlockFileGet (LPCFGFILE lpFile, LPCFGBLOCKREC lpBlockRec);
UINT     WINAPI ConfigBlockFileSet (LPCFGFILE lpFile, LPCFGBLOCKREC lpBlockRec);
UINT     WINAPI ConfigGetBlockInfo (DWORD dwBlockID,
                                LPCFGFILEHEADER lpHeader,
                                LPCFGITEMHEADER lpBlock);
UINT     WINAPI ConfigGetBlockFileInfo (LPCFGFILE lpFile,
                                DWORD dwBlockID,
                                LPCFGFILEHEADER lpFileHeader,
                                LPCFGITEMHEADER lpBlock);
UINT     WINAPI ConfigBlockFileIdChange (
                                LPCFGFILE       lpFile,
                                DWORD           dwOldID,
                                DWORD           dwNewID);

#ifdef  __cplusplus
}
#endif

#endif
