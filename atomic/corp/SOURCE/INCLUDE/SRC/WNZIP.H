/* ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» */
/* º Program     :  WnZip.H                        º */
/* º Release     :  2.0                            º */
/* º Description :  Archive management API for     º */
/* º                the PAK Toolkit                º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Author      :  Gus Smedstad                   º */
/* º Last Updated:  09/03/92                       º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Notice     : This source code is NOT intended º */
/* º              for general distribution.  Care  º */
/* º              should be taken to preserve its  º */
/* º              confidentiality.                 º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Copyright (C) 1992 by NoGate Software Corp.   º */
/* º             All rights reserved.              º */
/* ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼ */
#ifndef wnzip_h
#define wnzip_h

#pragma pack(4)

#define USE_ARCPAK 0

// The various options available when opening or creating an archive
// file.
//
#define ZIP_READONLY     1
#define ZIP_READWRITE    2
#define ZIP_EXCLUSIVE    4
#define ZIP_USE_TEMP     8 // Not yet implemented.
#define ZIP_ACCESS_OPTNS ( ZIP_READONLY | ZIP_READWRITE | ZIP_EXCLUSIVE )

// Size of the file name stored in ZIP files.  Maximum number of ZIP
// handles for the outside world.
//
#define ZIP_NAME_SIZ    260
#define ARC_NAME_SIZ    13
#define ZIP_MAX_HANDLES 10

// Size constraints for the encryption/decryption password.
//
#define ZIP_MIN_KEY_LEN  1
#define ZIP_MAX_KEY_LEN 50

// Constants that have to do the special volume label that is stored on
// floppies that are part of a multi-disk archive file.
#define ZIP_BASE_LABEL       "PKBACK# "
#define ZIP_BASE_LABEL_LEN   8
#define ZIP_LABEL_DIGITS_LEN 3
#define ZIP_LABEL_LEN        ( ZIP_BASE_LABEL_LEN + ZIP_LABEL_DIGITS_LEN )

// Special flags that affect the type of compression for the ZIP methods
// IMPLODE and DEFLATE.
#define ZIP_8K_WINDOW     1
#define ZIP_CHARS_ENCODED 2

// Information to retrieve for zip files.
//
typedef enum
{
   ZIP_FILEFORMAT,
   ZIP_FILECOUNT,
   ZIP_ORIGINALSIZE,
   ZIP_COMPRESSEDSIZE,
   ZIP_OPENMODE,
   ZIP_COMMENT,
   ZIP_FREESPACE,
   ZIP_FILENAME,
   ZIP_PATHNAME,
   ZIP_DATE,
   ZIP_COMPRESSTYPE,
   ZIP_ENCRYPTSTATE,
   ZIP_FILEINDEX,
   ZIP_EXTRAINFO,
   ZIP_FILEATTRS,
   ZIP_HEADERDISKNUM,
   ZIP_DATADISKNUM,
   ZIP_COMPDATAOFFSET,
   ZIP_ALLINFO
}
ZIP_INFO;

// Options for compression.
//
typedef enum
{
   ZIP_RECURSE  = 1,  /*****NUKE *****/
   ZIP_SUBPATH  = 2,  /*****NUKE *****/
   ZIP_SPECPATH = 4   /*****NUKE *****/
}
ZIP_ADD_OPTIONS;

// Archive formats.
//
typedef enum
{
   ZIP_FORMAT_NONE,
   ZIP_FORMAT_ARC,
   ZIP_FORMAT_PAK,
   ZIP_FORMAT_ZIP,
   ZIP_FORMAT_LHA
}
ZIP_TYPE;

// Compression types.
//
typedef enum
{
   ZIP_NONE,
   ZIP_BEST,
   ZIP_CRUNCH,
   ZIP_SQUASH,
   ZIP_IMPLODE,
   ZIP_SHRINK,
   ZIP_CRUSH,
   ZIP_DISTILL,
   ZIP_RLE,
   ZIP_SQUEEZED,
   ZIP_REDUCED,
   ZIP_UNKNOWN,
   ZIP_TOKENIZED,
   ZIP_DEFLATN,
   ZIP_ARJ,
   ZIP_LZH,
   ZIP_ZOO
}
ZIP_PACK_TYPE;

// Callback operations.
//
typedef enum
{
   ZIP_ADD_FILE,
   ZIP_COMPRESS_FILE,  //-*****NUKE*****
   ZIP_EXTRACT_FILE,
   ZIP_OPEN_FILE,
   ZIP_PACK_FILE,
   ZIP_REPLACE_FILE,
   ZIP_READ_FILE,
   ZIP_XFER_FILE
}
ZIP_CALLBACK_OP;

// Sub-operations within the callback operations.
//
typedef enum
{
   ZIP_OPEN_INSERT_LAST,
   ZIP_OPEN_INSERT_NTH,
   ZIP_ADD_CREATE_MULTI_QUERY,
   ZIP_ADD_INSERT_NEXT,
   ZIP_ADD_INSERT_NTH,
   ZIP_ADD_REINSERT_ORIGINAL,
   ZIP_EXTRACT_INSERT_NTH,
   ZIP_EXTRACT_PACK_NTH
}
ZIP_SPLIT_DISK_OP;

// Progress callback responses.
//
typedef enum
{
   ZIP_PROG_CB_ABORT,
   ZIP_PROG_CB_CONTINUE
}
ZIP_PROG_CBACTION;

// Error callback responses.
//
typedef enum
{
   ZIP_ERR_CB_ABORT,
   ZIP_ERR_CB_IGNORE,
   ZIP_ERR_CB_RETRY
}
ZIP_ERR_CBACTION;

// Split Disk callback responses.
//
typedef enum
{
   ZIP_SPLT_CB_ABORT,
   ZIP_SPLT_CB_CONTINUE
}
ZIP_SPLTDSK_CBACTION;

// Variable type for archive handles.
//
typedef int       HZIP;
typedef HZIP FAR* LPHZIP;

#pragma pack(1)
typedef struct
{
   unsigned short day   : 5;  // Days.
   unsigned short month : 4;  // Months.
   unsigned short year  : 7;  // Year.
}
ZIP_DATE_TYPE, FAR* LPZIP_DATE_TYPE;

typedef struct
{
   unsigned short tsec  : 5;  // Two second interval.
   unsigned short min   : 6;  // Minutes.
   unsigned short hour  : 5;  // Hours.
}
ZIP_TIME_TYPE, FAR* LPZIP_TIME_TYPE;

// Expanded file info for the outside world.
//
typedef struct
{
   WORD          wRecSiz;                   // The size of this record.  The
                                            //  path can be variable length.
   char          szName[ ZIP_NAME_SIZ ];    // The 12 byte file name.
   LONG          lOrigSiz;                  // The size of the compressed file
                                            //  before it was compressed.
   ZIP_PACK_TYPE ePckTyp;                   // The compression method.
   int           wPckFlgs;                  // Additional sub-type info (i.e.
                                            //  which version of CRUNCH).
   char          bEncrypted;                // This flag is TRUE if the file
                                            //  was encrypted after it was
                                            //   compressed.
   ZIP_DATE_TYPE diDate;                    // The date and time of associated
   ZIP_TIME_TYPE tiTime;                    //  with the "original" file.
   LONG          lCRC;                      // The CRC that was generated with
                                            //  the file when it was
                                            //   compressed.
   LONG          lComprsdSiz;               // The size of the compressed file.
   WORD          wFileAttrs;                // The file attributes of the
                                            //  original file.
   WORD          wHeadrDiskNum;             // The number of the diskette that
                                            //  the compressed file header
                                            //   resides on.
   LONG          lHeadrOffset;              // The file offset for the start of
                                            //  the compressed file header.
   WORD          wDataDiskNum;              // The number of the diskette that
                                            //  the compressed file data
                                            //   resides on.
   LONG          lCompDataOffset;           // The file offset for the start
                                            //  of the compressed data.
   char          szPath[ FSV_PATHBUF_LEN ]; // The path (if any) that was
                                            //  stored with the compressed file.
}
ZIP_FILE_INFO, FAR* LPZIP_FILE_INFO;

// Progress info passed to the specified callback function.
//
typedef struct
{
   WORD            wPercent; // Percentage of completion.
   ZIP_CALLBACK_OP eZipOp;   // The ZIP function that is in progress (COMPRESS,
                             //  EXTRACT, etc.)
   DWORD           dwParam;  // The affiliate callback data.

}
ZIP_PROGRESSINFO, FAR* LPZIP_PROGRESSINFO;

// Split Disk info passed to the specified callback function.
//
typedef struct
{
   ZIP_SPLIT_DISK_OP eZipOp;    // The ZIP operation that produced the
                                //  callback.
   DWORD             dwParam;   // The affiliate callback data.
   WORD              wDiskNum;  // The number of the disk to insert.
   char              szArchiveName[ FSV_PATHBUF_LEN + 1 ];
                                // Name of the archive file.
   char              szName[ FSV_PATHBUF_LEN + 1 ];
                                // This string is dependent on eZipOp.
                                //   ZIP_ADD_...     = Name of the source file being added.
                                //   ZIP_EXTRACT_... = Name of the compressed file being extracted.
}
ZIP_SPLTDSK_INFO, FAR* LPZIP_SPLTDSK_INFO;
typedef const ZIP_SPLTDSK_INFO FAR* LPCZIP_SPLTDSK_INFO;
#pragma pack()

// Progress call-back function.
//
typedef ZIP_PROG_CBACTION WINAPI ZIP_PROG_CBFUNC( LPZIP_PROGRESSINFO lpProgInfo );
typedef ZIP_PROG_CBFUNC   FAR*   LPZIP_PROG_CBFUNC;

// Error call-back function.
//
typedef ERROR_CALLBACK LPZIP_ERR_CBFUNC;

// Split disk call-back function.
//
typedef ZIP_SPLTDSK_CBACTION ( WINAPI* LPFNZIP_SPLTDSK_CBFUNC )( LPCZIP_SPLTDSK_INFO );

#ifndef LPHVOID
typedef VOID far* LPHVOID;
#endif

#ifndef ULONG
typedef unsigned long ULONG;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//==========================================================================
//
// ZIP_ERRCODE Zip_Create( lpszArchvFile, eArchiveType, lphArchvFile,
//                         lpProgCBFunc, dwProgCBData, lpErrCBFunc,
//                         dwErrCBData, lpfnSpltDskCB, dwSpltDskData )
//
//    lpszArchvFile - Pointer to the name of the archive file to open.
//    eArchiveType  - The type of archive file to create (ZIP, ARC, or PAK).
//    lphArchvFile  - Pointer to the variable that will contain the handle to
//                    the successfully created archive file.
//    lpProgCBFunc  - Pointer to the "progress" callback function.  This
//                    value can be NULL.
//    dwProgCBData  - Data that is to be passed the "progress" callback
//                    function. This value can be NULL.
//    lpErrCBFunc   - Pointer to the "critical error" callback function.  This
//                    value can be NULL.
//    dwErrCBData   - Data that is to be passed to the "critical error"
//                    callback function.  This value can be NULL.
//    lpfnSpltDskCB - Pointer to the "split disk" callback function.  This
//                    value can be NULL.
//    dwSpltDskData - Data that is to be passed to the "split disk" callback
//                    function.  This value can be NULL.
//
// This function will try to create an archive file of the specified type
// (ZIP, ARC, or PAK).  If lpgArchvFile is NULL and the archive file is
// successfully created then the appropriate trailer info will be written
// and the archive file will be "closed".
//
// Returns:
//    ZIP_NO_ERROR        - The operation was successful.
//    ZIP_BAD_ARG         - One of the input parameter(s) is invalid.
//    ZIP_NO_MEMORY       - Not enough  memory available for the "internal
//                          directory".
//    ZIP_NO_MORE_HANDLES - No more entires in the table of archive file
//                          handles.
//    ZIP_ACCESS_DENIED   - An active read/write "connection" has been made to
//                          the archive file in question (I only allow one such
//                          connection).
//    FSVERR...           - The appropriate error code returned by File
//                          Services ( i.e. file not found, disk i/o error,
//                          etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipCreate(
   LPCSTR                 lpszArchvFile,
   ZIP_TYPE               eArchiveType,
   LPHZIP                 lphArchvFile,
   LPZIP_PROG_CBFUNC      lpProgCBFunc,
   DWORD                  dwProgCBData,
   LPZIP_ERR_CBFUNC       lpErrCBFunc,
   DWORD                  dwErrCBData,
   LPFNZIP_SPLTDSK_CBFUNC lpfnSpltDskCB,
   DWORD                  dwSpltDskData
);
typedef ZIP_ERRCODE ( WINAPI* LPFNZIPCREATE )( LPCSTR, ZIP_TYPE, LPHZIP, LPZIP_PROG_CBFUNC, DWORD, LPZIP_ERR_CBFUNC, DWORD, LPFNZIP_SPLTDSK_CBFUNC, DWORD );


//==========================================================================
//
// ZIP_ERRCODE Zip_Open( lpszArchvFile, lphArchvFile, wOpenOptns, lpProgCBFunc,
//                       dwProgCBData, lpErrCBFunc, dwErrCBData, lpfnSpltDskCB,
//                       dwSpltDskData )
//
//    lpszArchvFile - Pointer to the name of the archive file to open.
//    lphArchvFile  - Pointer to the variable that will contain the "archive
//                    handle" if a successful file connection is made.
//    wOpenOptns    - A series of flags which are used to indicate the access
//                    privileges (read only or read/write) of the archive file
//                    and the request to use a temporary file to contain files
//                    that are "replaced" in the archive file.
//    lpProgCBFunc  - Pointer to the "progress" callback function.  This value
//                    can be NULL.
//    dwProgCBData  - Data that is to be passed the "progress" callback
//                    function.  This value can be NULL.
//    lpErrCBFunc   - Pointer to the "critical error" callback function.  This
//                    value can be NULL.
//    dwErrCBData   - Data that is to be passed to the "critical error"
//                    callback function.  This value can be NULL.
//    lpfnSpltDskCB - Pointer to the "split disk" callback function.  This
//                    value can be NULL.
//    dwSpltDskData - Data that is to be passed to the "split disk" callback
//                    function.  This value can be NULL.
//
// This function will try to "connect" to the specified archive file.  It is
// possible for multiple connections to be made to the same archive file.
// If a previous connection has not been made to the archive file then this
// function will try to open the file in question, and if successful, it
// will try to parse archive file so that it can build the "internal
// directory".
//
// Returns:
//    ZIP_NO_ERROR        - The operation was successful.
//    ZIP_UNRECNZD_ZIP    - The file is not a recognized zip file.
//    ZIP_BAD_ARG         - One of the input parameter(s) is invalid.
//    ZIP_NO_MEMORY       - Not enough  memory available for the "internal
//                          directory".
//    ZIP_NO_MORE_HANDLES - No more entires in the table of archive file
//                          handles.
//    ZIP_ACCESS_DENIED   - A connection has already been made to the archive
//                          file with read/write privileges (I only allow one
//                          read/write connection).
//    ZIP_DIR_ERROR       - The end of file was prematurely detected, the data
//                          stored in the compressed file headers, the central
//                          directory, or the special ARC/PAK records is
//                          invalid.
//    FSVERR...           - The appropriate error code returned by File
//                          Services ( i.e. file not found, disk i/o error,
//                          etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipOpen(
   LPCSTR                 lpszArchvFile,
   LPHZIP                 lphArchvFile,
   WORD                   wOpenOptns,
   LPZIP_PROG_CBFUNC      lpProgCBFunc,
   DWORD                  dwProgCBData,
   LPZIP_ERR_CBFUNC       lpErrCBFunc,
   DWORD                  dwErrCBData,
   LPFNZIP_SPLTDSK_CBFUNC lpfnSpltDskCB,
   DWORD                  dwSpltDskData
);
typedef ZIP_ERRCODE (WINAPI * LPFNZIPOPEN)( LPCSTR, LPHZIP, WORD, LPZIP_PROG_CBFUNC, DWORD, LPZIP_ERR_CBFUNC, DWORD, LPFNZIP_SPLTDSK_CBFUNC, DWORD );

//==========================================================================
//
// ZIP_ERRCODE ZipReOpen( hArchvFile )
//
//    hArchvFile - The handle (one-based) of the archive file.
//    wOpenOptns - A series of flags which are used to indicate the access
//                 privileges (read only or read/write) of the archive file
//                 and the request to use a temporary file to contain files
//                 that are "replaced" in the archive file.
//
// This function re-establishes a "connection" to the archive file.  Calling
// ZipReOpen() is much faster than ZipOpen() since the "internal directory"
// information does have to be constructed (unless some outside task has
// modified the archive file in the interim).
//
// Returns:
//    ZIP_NO_ERROR      - The operation was successful.
//    ZIP_UNRECNZD_ZIP  - The file is not a recognized zip file.
//    ZIP_BAD_ARG       - The specified handle is invalid.
//    ZIP_ACCESS_DENIED - The archive file associated with the handle has
//                        been opened with read/write privileges.
//    FSVERR_...        - The appropriate File Services error code.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipReOpen(
   HZIP hArchvFile,
   WORD wOpenOptns
);


//==========================================================================
//
// ZIP_ERRCODE ZipClose( hArchvFile )
//
//    hArchvFile - The handle (one based) of the archive file.
//
// This function closes the archive file affiliated with the handle and
// "frees" the specified handle.  If this is the last "connection" to the
// archive file then the "internal directory" will be "freed".
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    ZIP_BAD_ARG  - The specified handle is invalid.
//    FSVERR_...   - The appropriate File Services error code.
//
// Note:
//    1) If the archive file has been modified then you MUST call ZipPack()
//       before ZipClose() is called.  If you don't then the archive file
//       will probably have been left in an "unstable" state.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipClose(
   HZIP hArchvFile
);
typedef ZIP_ERRCODE (WINAPI * LPFNZIPCLOSE)( HZIP );


//==========================================================================
//
// ZIP_ERRCODE ZipTempClose( hArchvFile )
//
//    hArchvFile - The handle of the archive file.
//
// This function closes the archive file affiliated with the handle.  The
// handle to the archive file will remain active and a "connection" to the
// archive can be re-established by calling ZipReOpen().  Calling ZipReOpen()
// is much faster than ZipOpen() since the "internal directory" information
// does have to be constructed (unless some outside task has modified the
// archive file in the interim).
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    ZIP_BAD_ARG  - The specified handle is invalid.
//    FSVERR_...   - The appropriate File Services error code.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipTempClose(
   HZIP hArchvFile
);

//==========================================================================
//
// ZIP_ERRCODE ZipPack( hArchvFile )
//
//    hArchvFile - The handle (one-based) of the archive file.
//
// This function elimates all of the "unused space" in the archive file.  This
// function must be called whenever an archive file is "modified".
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_BAD_ARG   - One of the input parameter(s) is invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipPack(
   HZIP hArchvFile
);


//==========================================================================
//
// ZIP_ERRCODE ZipGetInfo( hArchvFile, eInfoType, lpBuf, wBufSiz, lpwInfoByts )
//
//    hArchvFile  - The handle of the archive file.
//    eInfoType   - The type of archive information to return.
//    lpBuf       - Pointer to the buffer that will contain the info.
//    wBufSiz     - The size of the recipient info buffer.
//    lpwInfoByts - Pointer to a variable that will contain the number of bytes
//                  of info that was returned.  This pointer can be NULL.
//
// This function tries to return requested information about the archive
// file (number of files, archive type, etc.).
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    ZIP_BAD_ARG  - The specified handle is invalid or it has been
//                   temporarily closed, or one of the input parameter(s)
//                   is invalid.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipGetInfo(
   HZIP     hArchvFile,
   ZIP_INFO eInfoType,
   LPVOID   lpBuf,
   WORD     wBufSize,
   LPWORD   lpwInfoByts
);


//==========================================================================
//
// ZIP_ERRCODE ZipSetInfo( hArchvFile, eInfoType, lpBuf, wBufSiz )
//
//    hArchvFile - The handle of the archive file.
//    eInfoType  - The type of archive information to store.
//    lpBuf      - Pointer to the input buffer.
//    wBufSiz    - The size of the input buffer.
//
// This function tries to store the specified data in the archive file
// (extra data or remark data).
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    ZIP_BAD_ARG  - The specified handle is invalid or it has been
//                   temporarily closed, or one of the input parameter(s)
//                   is invalid.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipSetInfo(
   HZIP     hArchvFile,
   ZIP_INFO eInfoType,
   LPVOID   lpBuf,
   WORD     wBufSize
);


// *************************NOT IMPLEMENTED***************************
// Moves a file index.  Does not change the physical file at all.
#ifdef IfImplemented
ZIP_ERRCODE WINAPI ZipReorderIndex(
   HZIP hArchvFile,
   WORD wOldIndex,
   WORD wNewIndex
);
#endif
// *************************NOT IMPLEMENTED***************************


//==========================================================================
//
// ZIP_ERRCODE ZipAddFile( hArchvFile, lpszFullName, lpszPathInfo, lpszNewName,
//                         ePackType, lpszKey, lpExtra, wExtraSiz, eAddOptns )
//
//    hArchvFile   - The handle of the archive file.
//    lpszFullName - The pointer to the fully qualified name of the file to
//                   "compress".
//    lpszPathInfo - Pointer to the path info (partial or full) that will be
//                   stored with the compressed file.  If this string contains
//                   a drive ID then the drive ID will not be stored.  If this
//                   pointer is NULL or the string is "empty" then no path
//                   info will be stored.
//    lpszNewName  - Pointer to the new name to store with the compressed file
//                   that has just been added.  If this pointer is NULL then
//                   the name that was part of lpszFullName will be stored.
//    ePackType    - The type of compression algorithm to use.  If you don't
//                   what to use pass in ZIP_BEST.
//    lpszKey      - Pointer to the key that will be used to "encrypt" the
//                   data after it has been compressed.  If this pointer is
//                   NULL then the data will NOT be encrypted.
//    lpExtra      - Pointer to any "extra data" that is to be stored with
//                   when the compressed file.  The "extra" data does not
//                   get compressed.  This pointer can be NULL.
//    wExtraSiz    - The number of bytes of "extra data" to store.
//    eAddOptns    - Special option flags.  At this time none are avaiable.
//
// This function tries to add the specified file to the archive file.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_BAD_ARG   - The specified handle is invalid or it has been
//                    temporarily closed, or one of the input parameter(s)
//                    is invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services.
//
// Notes:
//    1) If a "full path" is stored then the drive info and leading backslash
//       will be removed.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipAddFile(
   HZIP            hArchvFile,
   LPCSTR          lpszFullName,
   LPCSTR          lpszPathInfo,
   LPCSTR          lpszNewName,
   ZIP_PACK_TYPE   ePackType,
   LPCSTR          lpszKey,
   LPVOID          lpExtra,
   WORD            wExtraSiz,
   ZIP_ADD_OPTIONS eAddOPtns
);


//==========================================================================
//
// ZIP_ERRCODE ZipReplaceFile( hArchvFile, lpszName, wIndex, lpszSrcFile,
//                             lpszNewPath, lpszNewName, lpszKey )
//
//    hArchvFile  - The handle of the archive file.
//    lpszName    - The pointe to the name of the compressed file that
//                  is stored in the archive file.  This string must contain
//                  the same "path info" that is stored with the compressed
//                  file for a match to occur.  If this pointer is NULL then
//                  wIndex will be used.
//    wIndex      - The "internal directory" index (zero based) of the
//                  compressed file to "replace".
//    lpszSrcFile - The pointer to the fully qualified name of the file to
//                  "compress".
//    lpszNewPath - Pointer to the new path info (partial, full, or "empty")
//                  to store with the compressed file that has been replaced.
//                  If this string contains a drive ID then the drive ID will
//                  not be stored.  If this pointer is NULL then the original
//                  path info will be preserved.
//    lpszNewName - Pointer to the new name to store with the compressed file
//                  that has been replaced.  If this pointer is NULL then the
//                  original name will be preserved.
//    lpszKey     - Pointer to the key that will be used to "encrypt" the
//                  data after it has been compressed.  If this pointer is
//                  NULL then the data will NOT be encrypted.
//
// This function replaces the compressed file that is already stored in the
// archive file.  Since the amount of compressed data that will be produced
// is not known until the file has been compressed, the new data for the
// compressed file will always be "outputed" at the end of the archive file,
// and a "hole" will be left where the old copy resided.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_BAD_ARG   - The specified handle is invalid or it has been
//                    temporarily closed, or one of the input parameter(s)
//                    is invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipReplaceFile(
   HZIP   hArchvFile,
   LPCSTR lpszName,
   WORD   wIndex,
   LPCSTR lpszSrcFile,
   LPCSTR lpszNewPath,
   LPCSTR lpszNewName,
   LPCSTR lpszKey
);


//==========================================================================
//
// ZIP_ERRCODE ZipDeleteFile( hArchvFile, lpszName, wIndex )
//
//    hArchvFile - The handle of the archive file.
//    lpszName   - The pointer to the name of the compressed file to delete.
//                 If this pointer is NULL then wIndex will be used.
//    wIndex     - The "internal directory" index (zero based) of the
//                 compressed file to "delete".
//
// This function removes the compressed file from the "internal directory".
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_BAD_ARG   - The specified handle is invalid or it has been
//                    temporarily closed, or one of the input parameter(s)
//                    is invalid.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipDeleteFile(
   HZIP   hArchvFile,
   LPCSTR lpszFileName,
   WORD   wIndex
);


//==========================================================================
//
// ZIP_ERRCODE ZipExtractFile( hArchvFile, lpszName, wIndex, lpszDestPath,
//                             lpszDestName, bUseStoredPath, lpszKey )
//
//    hArchvFile     - The handle of the archive file.
//    lpszName       - The name of the compressed file to extract.  If this
//                     pointer is NULL then "wIndex" will be used.
//    wIndex         - The "internal directory" index (zero based) of the
//                     compressed file to extract.
//    lpszDestPath   - Pointer to the drive/directory that the file should
//                     be extracted to.  If this pointer is NULL and
//                     bUseStoredPath is FALSE the file will be extracted to
//                     the current directory.  If this pointer is NULL and
//                     bUseStoredPath is TRUE the file will be extracted to
//                     the current directory combined with the "path" that is
//                     stored with the compressed file.
//    lpDestName     - Pointer to the name to use for the destination.  If this
//                     pointer is NULL then the name stored with the compression
//                     file will be used.
//    bUseStoredPath - If this flag is "TRUE" then the "path" that was stored
//                     with the compressed file will be appended to the end
//                     of lpszDest.
//    lpszKey        - Pointer to the key that will be used to "decrypt" the
//                     compressed file.  If this pointer is NULL then the data
//                     will NOT be decrypted.
//
// This function will try to extract the specified compressed file from the
// archive file and copy it to the destination created from lpszDestPath and
// lpszDestName (or the name stored with the compressed file).  If
// "bUseStoredPath" is TRUE then the "path" (could be none, partial, or full)
// that was stored with the compressed file will be appended to the end of
// lpszDestPath.
//
// If the file to extract is actually a "directory entry" then the appropriate
// directory will be created.  The directory created will use the same rules
// regarding lpszDestPath and lpszDestName that were used when extracting a
// file.
//
// Returns:
//    ZIP_NO_ERROR       - The operation was successful.
//    ZIP_BAD_ARG        - The specified handle is invalid or it has been
//                         temporarily closed, or one of the input parameter(s)
//                         is invalid.
//    ZIP_CRC_MISMATCH   - The CRC stored in the header info didn't match
//                         the expanded data.
//    ZIP_DIR_ERROR      - File header data didn't match the central
//                         directory data.
//    ZIP_UNKNOWN_METHOD - The specified file employed a compression method
//                         that is not supported.
//    FSVERR...          - The appropriate error code returned by File
//                         Services.
//
// Notes:
//    1) If the path stored with the compressed file is a "full path" and
//       lpszDestPath is not NULL then only the drive specified in
//       lpszDestPath will be used.
//    2) If the file to extract is actually a "directory entry" then the
//       appropriate directory will be created.  An error will NOT be returned
//       if the directory to create already exists.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipExtractFile(
   HZIP   hArchvFile,
   LPCSTR lpszName,
   WORD   wIndex,
   LPCSTR lpszDestPath,
   LPCSTR lpszDestName,
   BOOL   bUseStoredPath,
   LPCSTR lpszKey
);
typedef ZIP_ERRCODE (WINAPI * LPFNZIPEXTRACTFILE)( HZIP, LPCSTR, WORD, LPCSTR, LPCSTR, BOOL, LPCSTR );


//==========================================================================
//
// ZIP_ERRCODE ZipGetFileInfo( hArchvFile, lpszName, wIndex, eInfoType, lpBuf,
//                             wBufSize, lpwInfoByts )
//
//    hArchvFile  - The handle of the archive file.
//    lpszName    - The name of the compressed file to get the information
//                  for.  If this pointer is NULL then wIndex will be used.
//    wIndex      - The "internal directory" index (zero based) of the
//                  compressed file.
//    eInfoType   - The type of information to return.
//    lpBuf       - Pointer to the buffer that will contain the info.  Can be
//                  NULL.
//    wBufSize    - The size of the recipient info buffer.
//    lpwInfoByts - Pointer to a variable that will contain the number of
//                  bytes of info for the item in question.  This pointer can
//                  be NULL.
//
// This function tries to return requested information about the compressed
// file (size of compressed file, original size, affiliate path, etc.).
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    ZIP_BAD_ARG  - The specified handle is invalid or it has been
//                   temporarily closed, or one of the input parameter(s)
//                   is invalid.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipGetFileInfo(
   HZIP     hArchvFile,
   LPCSTR   lpszName,
   WORD     wIndex,
   ZIP_INFO eInfoType,
   LPVOID   lpBuf,
   WORD     wBufSize,
   LPWORD   lpwInfoByts
);


//==========================================================================
//
// ZIP_ERRCODE ZipSetFileInfo( hArchvFile, lpszName, wIndex, eInfoType, lpBuf,
//                             wBufSize )
//
//    hArchvFile - The handle of the archive file.
//    lpszName   - The name of the compressed file to get the information
//                 for.  If this pointer is NULL then wIndex will be used.
//    wIndex     - The "internal directory" index (zero based) of the
//                 compressed file.
//    eInfoType  - The type of information to return.
//    lpBuf      - Pointer to the buffer that will contain the info.  Can be
//                 NULL.
//    wBufSize   - The size of the recipient info buffer.
//
// This function tries to set internal information (date, time, path, etc.)
// for the specified compressed file.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    ZIP_BAD_ARG  - The specified handle is invalid or it has been
//                   temporarily closed, or one of the input parameter(s)
//                   is invalid.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipSetFileInfo(
   HZIP     hArchvFile,
   LPCSTR   lpszName,
   WORD     wIndex,
   ZIP_INFO eInfoType,
   LPVOID   lpBuf,
   WORD     wBufSize
);


// *************************NOT IMPLEMENTED***************************
// Encrypt a file with a new key.
#ifdef IfImplemented
ZIP_ERRCODE WINAPI ZipReEncryptFile(
   HZIP   hArchvFile,
   LPCSTR lpszName,
   WORD   wIndex,
   LPCSTR lpszOldKey,
   LPCSTR lpszNewKey
);
#endif
// *************************NOT IMPLEMENTED***************************


// *************************NOT IMPLEMENTED***************************
// Re-compress an existing file in a new format.
#ifdef IfImplemented
ZIP_ERRCODE WINAPI ZipRecompressFile(
   HZIP          hArchvFile,
   LPCSTR        lpszName,
   WORD          wIndex,
   ZIP_PACK_TYPE ptPackType
);
#endif
// *************************NOT IMPLEMENTED***************************


// *************************NOT IMPLEMENTED***************************
// Compress a file contained in a memory block.
#ifdef IfImplemented
ZIP_ERRCODE WINAPI ZipAddFileFromMemory(
   HZIP          hArchvFile,
   LPHVOID       lphBuf,
   ULONG         ulBufSize,
   LPCSTR        lpszName,
   WORD          wIndex,
   ZIP_PACK_TYPE ePackType,
   LPCSTR        lpszKey,
   LPVOID        lpExtraData,
   WORD          wExtraSize
);
#endif
// *************************NOT IMPLEMENTED***************************


//==========================================================================
//
// ZIP_ERRCODE ZipReadFile( hArchvFile, lpszKey, lpszName, wIndex,
//                          ulStartPostn, lpBuf, wBufSize, wBytesRead )
//
//    hArchvFile   - The handle of the archive file.
//    lpszKey      - Pointer to the decryption key.  A value of NULL implies
//                   that the data was not encrypted.
//    lpszName     - The name of the compressed file to get the information
//                   for.  If this pointer is NULL then wIndex will be used.
//    wIndex       - The "internal directory" index (zero based) of the
//                   compressed file.
//    ulStartPostn - The number of bytes of expanded output to be "skipped"
//                   before the expanded bytes are store in the specified
//                   buffer.
//    lpBuf        - Pointer to the buffer that will contain the expanded
//                   data.
//    wBufSize     - The size of the recipient buffer.
//    lpBytesRead  - Pointer to a variable that will contain the number of
//                   bytes of data returned.  This pointer can be NULL.
//
// This function tries to expand and copy a portion of a compressed file
// to a memory buffer.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    ZIP_BAD_ARG  - The specified handle is invalid or it has been
//                   temporarily closed, or one of the input parameter(s)
//                   is invalid.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipReadFile(
   HZIP    hArchvFile,
   LPCSTR  lpszKey,
   LPCSTR  lpszName,
   WORD    wIndex,
   ULONG   ulStartPostn,
   LPVOID  lpBuf,
   WORD    wBufSize,
   LPWORD  wBytesRead
);


//==========================================================================
//
// ZIP_ERRCODE ZipGetAllFileInfo( hArchvFile, wStartIndex, wMaxCount, lpBuf,
//                                wBufSize, lpwNumEntries )
//
//    hArchvFile    - Handle to the zip file to extract the directory info
//                    from.
//    wStartIndex   - The first directory entry to get data from.
//    wMaxCount     - The maximum number of records to try to retrieve
//                    (a value of -1 means as many as possible).
//    lpBuf         - Pointer to the buffer to contain the directory info
//                    records.
//    wBufSize      - The maximum size of the buffer used to contain the
//                    directory info records.
//    lpwNumEntries - The number of directory records returned.  This pointer
//                    can be NULL.
//
// This function returns as many compressed file info records specified or
// as many that will fit in the specified buffer.  The first compressed file
// info record to return is specified by wStartIndex.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    ZIP_BAD_ARG  - The specified handle is invalid or it has been
//                   temporarily closed, or one of the input parameter(s)
//                   is invalid.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipGetAllFileInfo(
   HZIP    hArchvFile,
   WORD    wStartIndex,
   WORD    wMaxCount,
   LPBYTE  lpBuf,
   WORD    wBufSize,
   LPWORD  lpwNumEntries
);


//==========================================================================
//
// ZIP_ERRCODE ZipXferFile( hArchvSrc, lpszSrcName, wSrcIndx, hArchvDst,
//                          wDstIndx, lpszKey )
//
//    hArchvSrc   - Handle to the archive file to extract a compressed
//                  file from.
//    lpszSrcName - The name of the compression file to transfer from the
//                  source archive file.  The file name cannot contain wild
//                  card characters.  If this pointer is NULL then wSrcIndx
//                  and wDstIndx will be used.
//    wSrcIndx    - Index of the compressed file to extract from the source.
//    hArchvDst   - Handle to the archive file that will contain the compressed
//                  file that was extracted from the source.
//    wDstIndx    - Index of the compressed file (only aplies if it exists) in
//                  the destination archive file.
//    lpszKey     - Pointer to the key that will be used to decrypt/encrypt
//                  the compressed file if it has to be transferred between
//                  archive files that are different types.  If this pointer
//                  is NULL then the data will NOT be decrypted/encrypted.
//
// This function extracts a compressed file from the source archive file
// and transfers it to the destination archive file.  If the archive files
// are the same type then the data is simply copied from one archive file
// to the other.  Otherwsie, the compressed file will be extracted to a
// temporary file and then that temporary file will be compressed into the
// destination archive file.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_NO_MEMORY - No memory available for dynamic allocations.
//    ZIP_BAD_ARG   - Invalid input parameter (bad zip handle, etc.)
//    FSVERR...     - The appropriate error code returned by File Services.
//
// Notes:
//    1) It is assummed that both source and destination files have been
//       opened with the read/write attribute.  It would be disasterous
//       if the contents of the source file were altered doing the
//       transfer process.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipXferFile(
   HZIP   hArchvSrc,
   LPCSTR lpszSrcName,
   WORD   wSrcIndx,
   HZIP   hArchvDst,
   WORD   wDstIndx,
   LPCSTR lpszKey
);


//==========================================================================
//
// ZIP_ERRCODE ZipAddDirEntry( hArchvFile, lpszSrcDir, lpszPathInfo,
//                             lpszNewName )
//
//    hArchvFile   - Handle to the archive file to modify.
//    lpszSrcDir   - The fully qualified name of the directory to store.
//    lpszPathInfo - Pointer to the path info (partial or full) that will be
//                   stored with the directory name.  If this pointer is NULL
//                   or the string is "empty" then no path info will be stored.
//    lpszNewName  - Pointer to the directory name to store.  If this pointer
//                   is NULL then the name that was part of lpszSrcDir will
//                   be stored.
//
// This function tries to add the specified path as a directory entry in the
// archive file.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_NO_MEMORY - No memory available for dynamic allocations.
//    ZIP_BAD_ARG   - Invalid input parameter (bad zip handle, etc.)
//    FSVERR...     - The appropriate error code returned by File Services.
//
// Notes:
//    1) It is assummed that the specified path refers to a unique entry.
//       This function does not check for duplicates.
//    2) This function only works for ZIP files.
//    3) If a "full path" is stored then the drive info and leading backslash
//       will be removed.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipAddDirEntry(
   HZIP   hArchvFile,
   LPCSTR lpszSrcDir,
   LPCSTR lpszPathInfo,
   LPCSTR lpszNewName
);


//==========================================================================
//
// BOOL ZipIsMultiDiskZipFile( hArchvFile )
//
//    hArchvFile - Handle to the archive file to assess.
//
// This function returns TRUE if the specified archive file is a ZIP file that
// spans multiple disks.
//
// Returns:
//    FALSE - The arcvhive file is NOT a ZIP file that spans multiple disks or
//            the specified handle is invalid.
//    TRUE  - The arcvhive file IS a ZIP file that spans multiple disks.
//
//==========================================================================

BOOL WINAPI ZipIsMultiDiskZipFile(
   HZIP   hArchvFile
);
typedef BOOL ( WINAPI* LPFNZIPISMULTIDISKZIPFILE )( HZIP );

#ifdef __cplusplus
}
#endif


#pragma pack()

#endif
