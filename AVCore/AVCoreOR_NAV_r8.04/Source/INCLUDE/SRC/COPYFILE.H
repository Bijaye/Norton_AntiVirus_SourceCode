#ifndef _COPY_FILES_
#define _COPY_FILES_

// --------------------------------------------------------------------------
//  CopyFiles() callback function prototype. The application shoud return
//  TRUE to continue with the copying, or FALSE to stop copying.
// --------------------------------------------------------------------------

typedef BOOL ( WINAPI * LPFNDEF ) (     // Callback function format
    UINT    uCopyingFileNumber,         // File number that is being copied
    UINT    uNumFilesToCopy,            // Number of files to copy
    LPTSTR  lpszCopyingFileName,        // Name of file being copied
    UINT    uFileCopiedPercent,         // file copy completed percent
    UINT    uTotalCopiedPercent,        // total percentage completed
    DWORD   dwCookie );                 // user defined DWORD

// --------------------------------------------------------------------------
//  CopyFiles() one and only required parameter.
// --------------------------------------------------------------------------

#define COPYFLAG_OVERWRITEIFEXISTS  0x0001
#define COPYFLAG_NOTUSED_01         0x0002
#define COPYFLAG_NOTUSED_02         0x0004
#define COPYFLAG_NOTUSED_03         0x0005
#define COPYFLAG_NOTUSED_04         0x0010
#define COPYFLAG_NOTUSED_05         0x0020
#define COPYFLAG_NOTUSED_06         0x0040
#define COPYFLAG_NOTUSED_07         0x0080
#define COPYFLAG_NOTUSED_08         0x0100
#define COPYFLAG_NOTUSED_09         0x0200
#define COPYFLAG_NOTUSED_10         0x0400
#define COPYFLAG_NOTUSED_11         0x0800
#define COPYFLAG_NOTUSED_12         0x1000
#define COPYFLAG_NOTUSED_13         0x2000
#define COPYFLAG_NOTUSED_14         0x4000
#define COPYFLAG_NOTUSED_15         0x8000

typedef struct tagCOPYSTRUCT
{
    LPTSTR  lpszFileArray;                  // [in] array of filenames
    TCHAR   szCopyTo    [ SYM_MAX_PATH ];   // [in] copy defs to here
    TCHAR   szCopyFrom  [ SYM_MAX_PATH ];   // [in] copy defs from here
    LPFNDEF lpfnDefCallback;                // [in] function to notify during copy
    DWORD   dwCookie;                       // [in] user defined for callback
    UINT    uFilesCopied;                   // [out] number of files copied.
    WORD    wFlags;                         // [in] different flags.

} COPYSTRUCT, FAR * LPCOPYSTRUCT;

// --------------------------------------------------------------------------
//      Structure used internally. NOT FOR APPLICATION USE!
// --------------------------------------------------------------------------

typedef struct tagCOPYPRIVATE
{
    LPCOPYSTRUCT    lpCopy;             // From the application

    UINT    uNumFilesToCopy;            // Number of files to copy
    DWORD   dwTotalBytes;               // Bytes to copy
    DWORD   dwBytesCopied;

} COPYPRIVATE, FAR * LPCOPYPRIVATE;

// --------------------------------------------------------------------------
//  Possible return codes from CopyFiles()
// --------------------------------------------------------------------------

typedef WORD COPYERR;

#define COPYERR_OK       ((COPYERR)0)   // All the files copied without problem.
#define COPYERR_PARTIAL  ((COPYERR)1)   // Only some files were copied. See
                                        // the uFilesCopied for info on how
                                        // many files copied.
#define COPYERR_EXISTS   ((COPYERR)2)   // File or files with the same name
                                        // already exists on the destination
                                        // drive.
#define COPYERR_HALTED   ((COPYERR)3)   // Copying halted by the application.
#define COPYERR_PARAMS   ((COPYERR)4)   // Invalid parameters
#define COPYERR_RO       ((COPYERR)5)   // Target media is read only.
#define COPYERR_FILEOPEN ((COPYERR)6)   // Couldn't open either source or target
#define COPYERR_WRITE    ((COPYERR)7)   // Error writing to target
#define COPYERR_READ     ((COPYERR)8)   // Error reading from source
#define COPYERR_MEMALLOC ((COPYERR)9)   // Error allocating memory
#define COPYERR_INTERNAL ((COPYERR)10)  // Internal error

// --------------------------------------------------------------------------
//  Function prototypes
// --------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

COPYERR SYM_EXPORT WINAPI CopyFiles (   // Copy files
    LPCOPYSTRUCT    lpCopy );           // [in] pointer to copy structure

#if defined( __cplusplus )
}
#endif

#endif _COPY_FILES_
