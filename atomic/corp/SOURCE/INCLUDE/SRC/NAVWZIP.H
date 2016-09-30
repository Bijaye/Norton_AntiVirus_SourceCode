//-----------------------------------------------------------------
// include file for zip scanning
//-----------------------------------------------------------------
// $Header:   S:/include/VCS/navwzip.h_v   1.0   06 Feb 1997 20:51:22   RFULLER  $
//-----------------------------------------------------------------
// $Log:   S:/include/VCS/navwzip.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:22   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:38   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:12:02   BARRY
// Initial revision.
// 
//    Rev 1.1   28 May 1995 19:40:34   BARRY
// Added IsCompressedExtension
// 
//    Rev 1.0   28 Feb 1995 16:54:10   SZIADEH
// Initial revision.
//-----------------------------------------------------------------

#ifndef INCL_SYMZIP
#define INCL_SYMZIP

#define MAX_ZIPPASSWORD_LEN   64        // Zip password length.


                                        // Actions send to the callback
                                        // function from the engine
#define ZIP_FILESPEC           1        // Returns a file spec
                                        // entry from the zip catalog
#define ZIP_EXTRACTED          2        // File extracted
#define ZIP_ERROR              3        // Error encountered
#define ZIP_WARNING            4        // Warning encountered
#define ZIP_PASSWORD           5        // We need a password for this ZIP file
#define ZIP_PROGRESS           6        // Zip progress information it is
                                        // returned as a percentage
#define ZIP_PROGRESS_INIT      7        // Initialize the progress bar
#define ZIP_DONE               8        // The process of the zip file has been
                                        // completed.
                                        // Returns from the callback
                                        // to the ZIP Engine
#define ZIP_EXTRACT             1       // Extract the file
#define ZIP_SKIP                0       // Don't extract the specified file
#define ZIP_ABORT             (-1)      // Abort the enumeration

#define  SIZE_BUFFER 1024

typedef enum
  {
  ZIP_MEMORYALLOC_ERROR,
  ZIP_FILEOPEN_ERROR,
  ZIP_FILEREAD_ERROR,
  ZIP_FILECLOSE_ERROR,
  ZIP_OK
  }ZIP_STATUS;



typedef struct ZIPRECORD
  { 
    WORD        wPathTag;
    DWORD       dwSig;
    WORD        wCreat_Ver;
    WORD        wVer_2_ext;
    WORD        wflag_word;
    WORD        wComp_Type;             // Compression type
    DWORD       dwDate;                 // Date
    DWORD       dwCrc;                  // File CRC
    DWORD       dwComp_Size;            // Compressed Size
    DWORD       dwUncomp_Size;          // Uncompressed Size
    WORD        wFnlen;
    WORD        wExtra_len;
    WORD        wComment_len;
    WORD        wStart_disk;
    WORD        wI_attr;                
    DWORD       dwE_attr;               // File Attribute DOS in LOBYTE of
                                        // LOWORD.
    DWORD       dwLocal_off;
    LPCSTR      lpszBuffer;             // Also buffer for password (64+1 buffer)
    char        szPassword[MAX_ZIPPASSWORD_LEN + 1];
    } ZIPRECORD, FAR *LPZIPRECORD;

typedef int (CALLBACK* LPZIPCALLBACK)(
        UINT   uAction,
        UINT   uData,                   // Error state (if uAction = ZIP_ERROR)
                                        // Percentage  (if uAction = ZIP_PROGRESS)
        LPZIPRECORD lpZipRecord,        // Zip record
        DWORD  dwUserData);
                                        // Extract to alternate name
                                        // If clear extracts to the original
#define UNZIPFLAG_ALTERNATENAME 0x01    // name
#define UNZIPFLAG_TEMPNAME      0x02
#define UNZIPFLAG_DELETE_AFTER  0x04    // Delete file, after extracting
#define UNZIPFLAG_FREE_BUFFERS  0x08    // Free buffers, after extracting file
                                        
typedef struct {
    UINT            uFlags;             // UNZIPFLAG_XXXXX
    LPCSTR          lpszZipFileName;    // Zip file name to process
    LPCSTR          lpszMatchSpec;      // Spec to match  NOT IMPLEMENTED
    LPCSTR          lpszDestDir;        // Destination Directory
    LPZIPCALLBACK   lpCallback;
    DWORD           dwUserData;         // User specific data
    } UNZIP_STRUCT;
typedef UNZIP_STRUCT FAR *LPUNZIP_STRUCT;

#ifdef __cplusplus
extern "C" {
#endif
                                        // Must be called to initialize
ZIP_STATUS SYM_EXPORT WINAPI ZipInit (VOID);  // the Zip Engine

ZIP_STATUS SYM_EXPORT WINAPI ZipCleanup(VOID);// Cleanup the zip engine

ZIP_STATUS SYM_EXPORT WINAPI ZipProcessFile (LPUNZIP_STRUCT lpUnZipStruct);
BOOL SYM_EXPORT WINAPI IsCompressedExtension (LPSTR lpszFilename);


#ifdef __cplusplus
}
#endif

//************************************************************************
// Zip structure and functions   
//************************************************************************

#define ZIPFLAG_DELETE              0x0001
#define ZIPFLAG_STORE_PATH          0x0002
#define ZIPFLAG_SETCURRENTTIME      0x0004
#define ZIPFLAG_SETTIMEMOSTRECENT   0x0008
#define ZIPFLAG_TIMEDONOTCHANGE     0x0010

#define METHOD_SUPERFAST            0x0001
#define METHOD_FAST                 0x0002
#define METHOD_NORMAL               0x0004
#define METHOD_MAXIMUM              0x0008
#define METHOD_NONE                 0x0010

typedef struct {
    UINT   uFlags;                      // ZIPFLAG_XXXXX
    LPCSTR lpszZipFileName;             // Zip file name to create/update
    LPSTR lpszFileSpec;                 // Files to add to the zip
    LPZIPCALLBACK lpCallback;
    DWORD  dwUserData;                  // User specific data
    WORD   uCompressionMethod;          // METHOD_XXXX
    LPCSTR lpszPassWord;                // Password (NULL if none)
    char   szRelativeStart[SYM_MAX_PATH + 1];   // Only store paths relative to
                                                // this path
    char   szTempDir[SYM_MAX_PATH + 1]; // Temp path for engine to use for
                                        // temp files
    } ZIP_STRUCT;
typedef ZIP_STRUCT FAR *LPZIP_STRUCT;

VOID SYM_EXPORT WINAPI ZipEngine(LPZIP_STRUCT lpZipStruct);


//************************************************************************
// Typedefs for dynamic linking  
//************************************************************************

//typedef ZIP_STATUS (WINAPI *LPZIPINIT)(VOID);
//typedef ZIP_STATUS (WINAPI *LPZIPCLEANUP)(VOID);
//typedef ZIP_STATUS (WINAPI *LPZIPPROCESSFILE)(LPUNZIP_STRUCT lpUnZipStruct);
//typedef VOID (WINAPI *LPZIPENGINE)(LPZIP_STRUCT lpZipStruct);


//************************************************************************
// Error codes from UNZIP Engine
//************************************************************************

#define WBAD_TABLE_NBITS      1
#define WBAD_TABLE_LDBITS     2
#define WBAD_TABLE_REDUCE     3
#define WBAD_ZIP              4         // Bad Zip File
#define WNO_MEM_CPA           5         // Out of Memory error
#define WNO_MEM_C             6         // Out of Memory error
#define WNO_MEM_B             7         // Out of Memory error
#define WCANT_OPEN_Z          8         // Can't open zip
#define WCANT_OPEN_PRN        9         // Not Used
#define WCANT_CREAT          10         // Can't create zip
#define W_UNKNOWN            11         // Unknown error
#define WSKIP_ENCRYPT        12         // File encrypted skip it
#define W_INCON              13
#define WBAD_PWD             14         // Bad Password
#define WBAD_CRC             15         // Bad CRC
#define WBAD_AV_COUNT        16         // Password Errors
#define WBAD_AV_NUM          17         // Password Errors
#define W_OVERWRITE          18         // Warning overwrite
#define WINVALID_SHRUNK      19
#define WNO_MEM_MAKETABLE    20
#define WBAD_TABLE_IPP       21
#define WNO_MEM_FAST         22
#define WBAD_BLOCK_TYPE      23
#define W_NEW_VER            24         // New version
#define WNO_MEM_ZC           25
#define WHAS_ERRORS          26         // Zip file has errors
#define WZIP_HAS_SPAN        27


//************************************************************************
// Error codes from ZIP Engine
//************************************************************************

#define EBAD_FN	      1
#define EBAD_ZIP_FMT1 2
#define EBAD_ZIP_FMT2 3
#define ENO_MEM_CE    4                 // central end
#define ENO_MEM_C     5                 // central record
#define ENO_MEM_CA    6                 // central pointer array
#define ENO_MEM_H     7                 // hash table
#define ENO_MEM_P     8                 // path hash
#define ENO_MEM_B     9                 // buffers
#define ENO_MEM_T    10                 // new comment/recurse too deep/password
                                        // too long not enough memory for
                                        // authenticity text spec list
#define ENO_FILES    12
#define ENO_FIND     13
#define EDISK_FULL   14
#define ERD_ONLY     15
#define EBAD_PARM    16
#define EMAX_FILES   17
#define ECANT_OPEN   18
#define ENO_MEM_TB   19
#define ENO_MEM_NB   21
#define ENO_MEM_CB   22
#define ENO_MEM_DL   23
#define EWRONG_DOS   26
#define ENON_REMOVE  27
#define ESRC_IS_DEST 28
#define ENO_MEM_PL   29

#endif  // !INCL_SYMZIP

