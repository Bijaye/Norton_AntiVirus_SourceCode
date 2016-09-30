#ifdef _X86_
/* ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» */
/* º Program     :  WnZip.C                        º */
/* º Release     :  2.0                            º */
/* º Description :  Archive management API for     º */
/* º                the PAK Toolkit                º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Author      :  Gus Smedstad                   º */
/* º Rewritten By:  Jon J. Sorensen                º */
/* º Last Updated:  10/27/92                       º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Notice     : This source code is NOT intended º */
/* º              for general distribution.  Care  º */
/* º              should be taken to preserve its  º */
/* º              confidentiality.                 º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Copyright (C) 1992 by NoGate Software Corp.   º */
/* º             All rights reserved.              º */
/* ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼ */

/******************************************************
 *
 * $Header:   S:/BVZIP/VCS/wnzip.c_v   1.3   11 Aug 1997 00:56:48   KSACKIN  $
 *
 * Date:
 * Log:
 *
 *****************************************************/


/*---------------------------------------------------------*/
/*   I N C L U D E   F I L E S                             */
/*---------------------------------------------------------*/

// Common includes for Pre-compiled Header
// This should go before any other includes or definitions
#ifdef SYM_BUILD_SM
#include "fsv_pch.h"
#else
#include "..\inc\fsv_pch.h"
#endif
#pragma hdrstop
// Put all other includes after this

/* File Services include files (not in PCH). */
#ifdef SYM_BUILD_SM
#include "fsv.p"
#else
#include "..\inc\fsv.p"
#endif

/* Archive file compression routines include files. */
#include "compress.h"
#include "unarc.h"
#include "zip2x.h"
#include "pzip2x.h"
#include "lzh.h"
#include "wnzip.h"
//#include "tfile.h"

/*****NUKE*****/
#define Mem_SubMalloc(size) Mem_SubMallocHeap(size,NULL)
#define Mem_SubFree(lp) Mem_SubFreeHeap(lp,NULL)
/*****NUKE*****/

/*****NUKE*****/
//#define MYDEBUG
/*****NUKE*****/

/*---------------------------------------------------------*/
/*   C O N S T A N T S                                     */
/*---------------------------------------------------------*/

// Some of the "filter" routines are constrained by a byte boundary
// condition.
#define ZIP_BOUNDARY_BYTS 8

// The maximum amount of output to produce.
#define ZIP_MAXLONG ( 0x7fffffffL - ZIP_BOUNDARY_BYTS )

// Constants that signify the type of an archive file (ZIP, ARC, or PAK).
const BYTE ZIP_ARC_MARKER = 26;
const BYTE ZIP_PAK_MARKER = 0xFE;
const BYTE ZIP_ZIP_MARKER = 0x50;

// Four byte signature values used to mark the various header records that
// constitute a ZIP archive file.
#define ZIP_FILE_SIGNATURE 0x04034b50
#define ZIP_DIR_SIGNATURE  0x02014b50
#define ZIP_END_SIGNATURE  0x06054b50

// Constants for the different files a data block can be contained
// in (archive or temporary).
#define ZIP_ARCHV_HANDLE 0
#define ZIP_TEMP_HANDLE  1

// ID's of the various special info records that are stored at the end
// of ARC and PAK files.  PAK only recognizes the values 1-19.
#define ZIP_EOF              0
#define ZIP_PAK_REMARK_DATA  1
#define ZIP_PAK_PATH_DATA    2
#define ZIP_PAK_EXTRA_DATA   5
//#define ZIP_PAK_FILE_ATTRS  19

// Buffer sizes used for compressing/expanding a file.
#define ZIP_INPUT_BUFFER_SIZE   (4096 + sizeof (int))
#define ZIP_INPUT_CHARS         ( ZIP_INPUT_BUFFER_SIZE - sizeof(int) )
#define ZIP_OUTPUT_BUFFER_SIZE  4096 // Must be divisible by ZIP_BOUNDARY_BYTS.

// Size of the buffer used when copying a portion of an archive file.
#define ZIP_COPY_BUFFER_SIZE    32768

// The amount of additional space to allocate when "growing"
// the archive file.
#define ZIP_GROWTH_CHUNK        ( 256L * 1024L )

// The various OS types.
#define ZIP_MSDOS      0
#define ZIP_AMIGA      1
#define ZIP_VMS        2
#define ZIP_UNIX       3
#define ZIP_VM_CMS     4
#define ZIP_ATARI_ST   5
#define ZIP_OS2_HPFS   6
#define ZIP_MATINTOSH  7
#define ZIP_SYSTEM     8
#define ZIP_CPM        9

// Size of the static portion of LHA compressed file header.
#define ZIP_LHA_BASE_HEADR_BYTS 22

// The maximum number of unidentified bytes that can be stored in a
// level 0 and a level 1 LZH compressed file header.
#define ZIP_MAX_LHA_LEV0_UNKNOWNBYTS 6
#define ZIP_MAX_LHA_LEV1_UNKNOWNBYTS 0
#define ZIP_MAX_LHA_UNKNOWNBYTS      6 // This must be the max.

// Percentage of free space required on the "next" diskette for
// a multi-disk archive file.  Multiple of 10 (i.e. 6 is 60%).
#define ZIP_PERCENT_FREE 6

/*---------------------------------------------------------*/
/*   T Y P E   D E F I N I T I O N S                       */
/*---------------------------------------------------------*/

/*****REPLACE WITH LPWORD*****/
typedef UINT FAR* LPUINT;
/*****REPLACE WITH LPWORD*****/

#ifndef LPBOOL
   typedef BOOL FAR* LPBOOL ;
#endif

#ifndef ULONG
   typedef unsigned long ULONG;
#endif

// Definition of a fixed size file name string.
typedef char  ARC_FLENMESTR[ ARC_NAME_SIZ ];
typedef TCHAR ZIP_FLENMESTR[ ZIP_NAME_SIZ ];
typedef TCHAR ZIP_FULLNAME[ FSV_PATHBUF_LEN ];

// Structure used to process and modify a file on disk.
typedef struct
{
   ZIP_FULLNAME         szFullName;  // The fully qualified name of the file
                                     //  that has been opened or created.
   LPFSVBUFIO_FBUF_INFO lpFBuf;      // Structure used to buffer file i/o.
}
ZIP_BFILE_INFO, FAR* LPZIP_BFILE_INFO;

// Structure used to process a block of data within a disk file or
// a memory buffer.
typedef struct
{
   LONG lStart;    // Start of the data block within the file.
   LONG lSize;     // Size of the data block in bytes.
   WORD wDiskNum;  // Disk number (0 relative) the block begins.
   BYTE hbFleIndx; // Handle to the file that the data block is
                   //  stored in (archive or temporary).
}
ZIP_DATA_BLK, FAR* LPZIP_DATA_BLK;

// Generic structure used to process a compressed file for the
// various archive file types (ZIP, PAK, or ARC).
typedef struct
{
   BYTE          bDirty;         // This flag is TRUE if any data in this
                                 //  structure has been modified since it
                                 //   was read in from disk.
   BYTE          bHeadrVrfd;     // This flag only pertains to compressed
                                 //  files contained in a ZIP archive file.
                                 //   If this flag is FALSE then the data in
                                 //    this structure will be compared with
                                 //     the data stored in the compressed
                                 //      file header when it is being
                                 //       "extracted".

   ZIP_FLENMESTR szName;         // Local copy of compressd file name.
   LPTSTR         lpszPath;       // Pointer to path data stored with the
                                 //  compressed file.
   LPTSTR         lpszRemark;     // Pointer to the "remark" stored with
                                 //  compressed file.

   LONG          lOrigSiz;       // The sie of the file in it's expanded
                                 //  form.
   ZIP_PACK_TYPE ePckTyp;        // The technique used to compress the data
                                 //  in the file.
   WORD          wPckFlgs;       // Additional info about the type of
                                 //  compression used.
   BYTE          bEncrypted;     // This flag is TRUE if the file had been
                                 //  encrypted.
   ZIP_DATE_TYPE diDate;         // The date and time the file was last
   ZIP_TIME_TYPE tiTime;         //  modified when it resided on disk.
   WORD          wAttrs;         // The DOS file attibutes (read only, hidden,
                                 //  system, etc.).
   LONG          lCRC;           // The 32 bit CRC computed from the compressed
                                 //  data.
   LONG          lCompHeadrStrt; // File position of the header of the
                                 //  compressed file.
   WORD          wDiskNum;       // Disk number (0 relative) where this file
                                 //  begins.

   ZIP_DATA_BLK  dbComprssdFile; // Size and file position of the compressed
                                 //  file.
// ZIP_DATA_BLK  dbRemark;       // Size and file position of the remark text
//                               //  associated with the compressed file.
   ZIP_DATA_BLK  dbExtra1;       // Size and file position of the 1st copy
                                 //  of the extra data associated with the
                                 //   compressed file.  For ARC/PAK files this
                                 //    is the only copy.  For ZIP files this
                                 //     is the copy stored with the file
                                 //      header.

   WORD          wSpclRecByts;   // The number of bytes that are stored in the
                                 //  extended header records.  This field is
                                 //   only used for LHA archive files.
   BYTE          bLevel;         // Type of header format in an LZH compressed
                                 //  file.  Only values of 0 and 1 are
                                 //   supported.  A value of 0 implies that
                                 //    the header cannot have extended header
                                 //     records.
   WORD          wUnknownByts;   // The number of unidentified bytes that can
                                 //  be stored in the header of an LZH file
                                 //   (real bozo format).
   BYTE          abUnknwnByts[ ZIP_MAX_LHA_UNKNOWNBYTS ];
                                 // The unidentified header bytes that are
                                 //  preserved in case the compressed file
                                 //   gets relocated in the archive file.
}
ZIP_COMP_INFO, FAR* LPZIP_COMP_INFO, FAR* FAR* LPLPZIP_COMP_INFO;
typedef const ZIP_COMP_INFO FAR* LPCZIP_COMP_INFO;

// Structure used to optimize the sequential retrieval of data
// when calling ZipReadFile().
typedef struct
{
   HZIP          hArchvFile;   // Handle of the archive file last accessed.
   WORD          wIndex;       // Directory index of the last file accessed.
   LONG          lLastByt;     // The index of the last byte of output that was
                               //  that was returned.
   LONG          lComprsdStrt; // File position of the compressed file data.
   LPVOID        lpvVars;      // Pointer to the state variables used by the
                               //  various de-compression algorithms.
   ZIP_PACK_TYPE ePckTyp;      // The compression method used to pack the
                               //  file in question.
   WORD          wKeyIndx;     // ????
   LPCTSTR        lpszKey;      // ????
}
ZIP_RDFILE_INFO, FAR* LPZIP_RDFILE_INFO;

// Structure used to keep track of the state of the archive file.
typedef struct
{
   BOOL              bDirty;        // This flag is set to "TRUE" when the
                                    //  archive file is modified.
   BOOL              bBadState;     // This flag is set to TRUE if ZipPack()
                                    //  was called and it failed.

   LONG              lExtraRemByts; // The number of bytes for all of the
                                    //  "extra" and "remark" data.
   LONG              lFreedByts;    // The number of bytes for all of the
                                    //  "holes" that are in the file because
                                    //   file(s) were relocated.
   LONG              lEOFByts;      // The number of bytes required for the
                                    //  end of file data (ARC/PAK data records
                                    //   or central directory records).
   LONG              lAvailPostn;   // The file position of the location
                                    //  available for the next compressed
                                    //   file to store.

   WORD              wDate;         // The date, time, and size of the archive
   WORD              wTime;         //  file when it was last "closed".  This
   DWORD             dwFileSiz;     //   is used to detect if the archive file
                                    //    has been modified while it was
                                    //     closed.

   // Fields that pertain to multi-disk archive files.
   //
   BOOL              fCanBeSplit;   // This flag is TRUE if the archive file
                                    //  CAN be split across multiple floppies.
   BOOL              fIsSplit;      // This flag is TRUE if the archive file
                                    //  IS SPLIT across multiple floppies.
   WORD              wCurDisk;      // The current disk (0 based) that has been
                                    //  inserted for the multi-disk ZIP file.
   WORD              wStrtDisk;     // The disk (0 based) where the central
                                    //  directory starts.
   WORD              wLastDisk;     // The disk (0 based) where the ZIP end
                                    //  of file record has been stored.
   LONG              lCntrlDirOff;  // The relative file position of the
                                    //  Central Directory.
}
ZIP_DIR_STATE, FAR* LPZIP_DIRSTATE;

// Structure used to keep track of all of pertinent information and all of
// the compressed files that constitute an archive file.
typedef struct
{
   ZIP_TYPE          eArchvTyp;     // The type of archive file (ZIP, PAK, or
                                    //  ARC) that the directory info pertains
                                    //   to.
   BOOL              bArchvVrfd;    // If this flag is TRUE then all of the
                                    //  file header records in the archive
                                    //   file have been verified.  This flag
                                    //    only pertains to ZIP files.
   WORD              wUseCount;     // The number of connections made to the
                                    //  archive file (usually one).
   WORD              wActiveCount;  // The number of active connections made
                                    //  to the archive file (gets decremented
                                    //   the caller does a temporary close).

   ZIP_BFILE_INFO    fiArchvFile;   // Structure used to buffer data to and
                                    //  from the archive file being processed.
// ZIP_BFILE_INFO    fiTmpFile;     // Temporary file for end-dir information: remarks,
                                    //  and 'extra' information.

   LPLPZIP_COMP_INFO lplpCompFiles; // Pointer to the list of pointers to the
                                    //  info structures for the compressed
                                    //   files.
   WORD              wFileCount;    // The number of compressed files contained
                                    //  in the archive file.

   ZIP_DATA_BLK      dbExeHeadr;    // Size and file position (always 0) of
                                    //  the self-extraction header.
// ZIP_DATA_BLK      dbRemark;      // Size and file position of affiliate
//                                  //  "remark text".
   LPTSTR             lpszRemark;    // Pointer to remark data stored with the
                                    //  archive file.
   ZIP_DATA_BLK      dbExtra;       // Size and file position of affiliate
                                    //  "extra data".

   ZIP_DIR_STATE     SI;            // Info pertaining to the state of the
                                    //  archive file.

   LPBYTE            lpTmpBuf;      // Pointer to the buffer and the size
   WORD              wTmpBufSiz;    //  allocated.  This info will be used
                                    //   by Zip_CopyDataBlk().

   ZIP_RDFILE_INFO   RFI;           // Info used to optimize sequential ex-
                                    //  pansion of data read via ZipReadFile().

}
ZIP_ARCHIVE_DIR, FAR* LPZIP_ARCHIVE_DIR, FAR* FAR* LPLPZIP_ARCHIVE_DIR;

#pragma pack(1)

// Header record
// 2 bytes at the start of each ARC/PAK header.
typedef struct
{
   BYTE bMarker; // Marker type used to indicate an ARC file or a PAK file.
   BYTE bType;   // The pack type for a compressed file header or the
                 //  record type for a special data record.  A value of
                 //   0 implies the end of file.
}
ZIP_ARC_HEADR_STRT, FAR* LPZIP_ARC_HEADR_STRT;

// The header record definition of a compressed file stored in
// an ARC/PAK archive file.
typedef struct
{
   BYTE          bMarker;       // Always ZIP_ARC_MARKER.
   BYTE          bPckTyp;       // The algorithm used to compress the data.
   ARC_FLENMESTR szName;        // The name of the compressed file.
   LONG          lComprsdSiz;   // The size of the compressed data.
   ZIP_DATE_TYPE diDate;        // The date and time the file was last
   ZIP_TIME_TYPE tiTime;        //  modified when it resided on disk.
   WORD          wCRC;          // The 16 bit CRC computed from the compressed
                                //  data.
   LONG          lOrigSiz;      // The size of the file before it was
                                //  "compressed".
}
ZIP_ARC_HEADR, FAR* LPZIP_ARC_HEADR;

// Structure used to describe any special data that pertains to a
// compressed file stored in a PAK/ARC archive file.
typedef struct
{
   BYTE bMarker;  // A one byte "marker".  Should always be ZIP_PAK_MARKER.
   BYTE bRecType; // The type of the special data record (remark, extra,
                  //  path, etc.).
   WORD wFileNum; // The one-based number of the compressed file that special
                  //  data is affiliated with.  A value of zero implies that
                  //   the data is affiliated with the archive file.
   LONG lDataSiz; // The number of bytes of special data.
}
ZIP_PAK_HEADR, FAR* LPZIP_PAK_HEADR;

// Structure stored both in the compressed file header and in the central
// directory header in a ZIP archive file.
typedef struct
{
   BYTE          bExtractVersn;      // program version to extract file
   BYTE          bExtractOSTyp;      // computer type that made file
   WORD          fEncrypted    : 1;  // 1 if file is encrypted
   WORD          fEightKWindow : 1;  // 1 if 8k window used, else 4k
   WORD          fCharsEncoded : 1;  // 1 if characters were encoded
   WORD          fUnused       : 13; // unused bits
   WORD          wPackTyp;           // The algorithm used to compress the
                                     //  data.
   ZIP_TIME_TYPE tiTime;             // The date and time the file was last
   ZIP_DATE_TYPE diDate;             //  modified when it resided on disk.
   LONG          lCRC;               // The 32 bit CRC computed from the
                                     //  compressed data.
   LONG          lComprsdSiz;        // The size of the compressed data.
   LONG          lOrigSiz;           // The size of the file before it was
                                     //  "compressed".
   WORD          wNameSiz;           // The size of the file name (could
                                     //  include the path) that follows a
                                     //   compressed file header record or
                                     //    a central directory header record.
   WORD          wExtraSiz;          // The size of extra information.
}
ZIP_SHARED_HEADR, FAR* LPZIP_SHARED_HEADR;

/* Header definition for a ZIP compressed file. */
typedef struct
{
   LONG             lFileSignature; // Always ZIP_FILE_SIGNATURE.
   ZIP_SHARED_HEADR shData;         // Complete info about the compressed file.
}
ZIP_ZIP_HEADR, FAR* LPZIP_ZIP_HEADR;

// Structure definition for a ZIP central directory record.
typedef struct
{
   LONG             lDirSignature;  // Always ZIP_DIR_SIGNATURE.
   BYTE             bProgMadeVersn; // Program version that created the file.
   BYTE             bOSTyp;         // Computer/operating system type that
                                    //  the file ws created on.
   ZIP_SHARED_HEADR shData;         // Complete info about the compressed file.
   WORD             wRmrkSiz;       // Size of the comment associated with
                                    //  central directory record.
   WORD             wDiskNum;       // Disk number (0 based) where this file
                                    //  begins.
   WORD             wIntAttr;       // Internal attribute flag.  If the low
                                    //  bit is set then the file is an ASCII
                                    //   or text file.  The rest of the bits
                                    //    are unused.
   LONG             lExtAttr;       // External attribute flag.  The MS-DOS
                                    //  file attributes.
   LONG             lCompHeadrStrt; // File position of the compressed file
                                    //  header record.
}
ZIP_CDIR_REC, FAR* LPZIP_CDIR_REC;

// Structure defintion of the ZIP file trailer record.
typedef struct
{
   LONG lEndSignature;   // Should always be ZIP_END_SIGNATURE.
   WORD wDiskNum;        // ID of the disk (0 based) for this portion of
                         //  the file.
   WORD wStartDisk;      // ID of the disk where the central directory starts.
   WORD wEntries;        // Number of central directory entries on this disk.
   WORD wTotalEntries;   // Number of entries in central directory */
   LONG lCntrlDirSize;   // Size of the central directory.
   LONG lCntrlDirStart;  // File position of the start of central directory.
   WORD wRmrkSiz;        // Length of comment associated with the archive
                         //  file.
}
ZIP_ZIP_END_REC;

// Structure definition for an LHA file header.
typedef struct tagZIP_LHA_FILE_HEADR
{
   BYTE          bHeadrSiz;       // Header size.
   BYTE          bHeadrChkSum;    // Checksum of header contents.
   TCHAR          szCompMethod[5]; // Compression method.
   LONG          lComprsdSiz;     // Compressed file size.
   LONG          lOrigSiz;        // Uncompressed file size.
   ZIP_TIME_TYPE tiTime;          // MS-DOS file time.
   ZIP_DATE_TYPE diDate;          // MS-DOS file date.
   BYTE          bFileAttr;       // MS-DOS file attribute (always 0x20 for
                                  //  level 1 files).
   BYTE          bLevel;          // Level 0 and level 1 files are supported.
   BYTE          bLen;            // Filename length.
   TCHAR          szFileName[ FSV_PATHBUF_LEN + 4 + ZIP_MAX_LHA_UNKNOWNBYTS ];
                                  // Filename string (can contain a path for
                                  //  level 0 files).
// --------Level 0 and level 1 fields that follow the file name.
// WORD          wCRC;            // CRC for the compressed data.
// --------Level 1 fields that follow the file name.
// BYTE          bPlatform;       // Character used to signify the platform.
//                                //  M = MS DOS
// WORD          wExtHeadLen;     // Length of the first extended header (a
//                                //  value of zero implies none).
}
ZIP_LHA_FILE_HEADR, FAR* LPZIP_LHA_FILE_HEADR;

// Encryption data structure for ZIP files.
//
typedef struct tagZIP_ENCRYPT_INFO
{
   BYTE bRandom[10]; /* random data (progam name) */
   BYTE bCrcOld;
   BYTE bCrcHigh;    /* top 8 bits of file CRC */
}
ZIP_ENCRYPT_INFO, FAR* LPZIP_ENCRYPT_INFO;

// Posible state of an archive directory handle.
//
typedef enum
{
   ZIP_DIR_OPENED,      // The connection to the archive file is active.
   ZIP_DIR_TMP_CLOSED,  // The connection to the archive file has been
                        //  temporarily suspended.
   ZIP_HANDLE_AVAIL,    // The handle is available for use.
   ZIP_DIR_INIT_STATE   // The handle is being "initialized".
}
ZIP_HANDLE_STATE, FAR* LPZIP_HANDLE_STATE;

// Structure for an entry in the table of archive file handles.
typedef struct
{
   LPZIP_ARCHIVE_DIR      lpDir;         // Pointer to the directory info for the
                                         //  archive file in question.

   WORD                   wOpenOptns;    // Read only, exlusive read only, or
                                         //  exclusive read/write.
   ZIP_HANDLE_STATE       hsDirHandleSt; // The connection to the archive file is
                                         //  "active", it has been temporarily
                                         //   suspended, or this handle entry
                                         //    is not being used.

   LPZIP_PROG_CBFUNC      lpProgCBFunc;  // Pointer to the progress callback and
   DWORD                  dwProgCBData;  //  affiliate callback data that
                                         //   corresponds to the archive handle.
   LPZIP_ERR_CBFUNC       lpCritCBFunc;  // Pointer to the critical error callback
   DWORD                  dwCritCBData;  //  and affiliate callback data that
                                         //   corresponds to the archive handle.
   LPFNZIP_SPLTDSK_CBFUNC lpfnSpltDskCB; // Pointer to the callback function used
                                         //  to handle ZIP files that are "split"
                                         //   across multiple floppies.
   DWORD                  dwSpltDskData; // Data for the split disk callback.

   LONG                   lPercntMax;    // This value will be 100 for the one pass
                                         //  compression algorithms and 50 for
                                         //   the two pass compression algorithms.
   WORD                   wPercntOff;    // This value will usually be 0 and it
                                         //  will be 50 for the 2nd pass of the two
                                         //   pass compression algorithms.
}
ZIP_HANDLE_INFO, FAR* LPZIP_HANDLE_INFO, FAR* FAR* LPLPZIP_HANDLE_INFO;

// Description of the table used to process the archive file handles.
typedef struct
{
   WORD            wHandlesInUse;
   WORD            wLastHandleUsed;
   ZIP_HANDLE_INFO hlHandleLst[ ZIP_MAX_HANDLES ];
}
ZIP_HANDLE_TABLE, FAR* LPZIP_HANDLE_TABLE;

// The "intent" for accessing the archive file.
typedef enum
{
   ZIP_DIR_READ,      // The caller wants to read data from the archive file.
   ZIP_DIR_WRITE,     // The caller wants to modify the archive file.
   ZIP_DIR_CLOSE,     // The caller wants to close the archive file.
   ZIP_DIR_REOPEN,    // The caller wants to "reopen" the archive file.
   ZIP_DIR_QUERY_INFO // The caller wants to obtain info about the archive file.
}
ZIP_DIR_ACCESS_OP, FAR* LPZIP_DIR_ACCESS_OP;

// Structure used to copy expanded output to a memory buffer.
typedef struct
{
   LPBYTE lpBuf;     // Pointer to the output buffer.
   WORD   wBufSiz;   // Size of the recipient buffer.
   LONG   lVirtOff;  // The number of bytes expanded prior to...
   LONG   lStart;    // The start of the output stream that corresponds to
                     //  the beginning of the recipient buffer.
}
ZIP_MBUF_INFO, FAR* LPZIP_MBUF_INFO;

// Define the generic read/write function pointer.  The read/write functions
// get passed to the functions that expand and compress data.
typedef LONG ( WINAPI *LPZIP_FILEIO_FUNC )( LPFSVBUFIO_FBUF_INFO lpFBuf, LPVOID lpBuf, WORD wNumByts, LPZIP_ERR_CBFUNC lpfnCrit, DWORD dwCrit );

// Define the generic seek function pointer.  The seek function gets passed
// the functions that compresses a file or a memory buffer.
typedef LONG ( WINAPI *LPZIP_SEEK_FUNC ) ( LPFSVBUFIO_FBUF_INFO lpFBuf, LONG lOffset, int nSeekOp, LPZIP_ERR_CBFUNC lpfnCrit, DWORD dwCrit );

// Define the generic filtering function.  All of the data
// "compression/expansion" must use this protocol.
typedef int ( WINAPI *LPZIP_FILTER_FUNC ) ( LPTSTR lpInBuf, LPWORD lpwInputSiz, LPTSTR lpOutBuf, LPWORD lpwOutptSiz, LPVOID lpvVariables, TCHAR cEOF );

// Structure used to contain the encryption keys.
//
typedef struct tagZIP_KEYS
{
   ULONG ulKey1; // First key to Zip Data Encryption.
   ULONG ulKey2; // Second key to Zip Data Encryption.
   ULONG ulKey3; // Third key to Zip Data Encryption.
}
ZIP_KEYS, FAR* LPZIP_KEYS;

// Stucture used to contain all of the parameters required by the generic
// "filter" function.
//
typedef struct
{
   LPVOID            lpVars;
   WORD              wStructSiz;

   BOOL              bCompress;
   WORD              wKeyIndx;
   LPCTSTR            lpszKey;
   BOOL              bComputeCRC;
   BOOL              bUseCRC32;
   LONG              lCRC;
   WORD              wBndryByts;

   LONG              lNumInputByts; // The number of input bytes to read.
   LONG              lMaxOutptByts; // The ...
   LONG              lOutptByts;    // The number of bytes written to the
                                    //  output file.

   ZIP_KEYS          sKeys;         // Structure containing the ZIP
                                    //  "encryption keys".

   LPZIP_FILTER_FUNC lpFiltrFunc;   // Pointer to the function to call to
                                    //  compress or expand a "block" of data.
   LPZIP_FILEIO_FUNC lpReadFunc;    // Pointer to the function to call when
                                    //  reading a "block" of data.
   LPZIP_FILEIO_FUNC lpWriteFunc;   // Pointer to the function to call when
                                    //  writing a "block" of data.
   ZIP_CALLBACK_OP   eZipFunc;
}
ZIP_FILTER_INFO, FAR* LPZIP_FILTER_INFO;
#pragma pack()


/*---------------------------------------------------------*/
/*   G L O B A L   V A R I A B L E S                       */
/*---------------------------------------------------------*/

// Table used to process the archive handles.
static ZIP_HANDLE_TABLE gHandleTbl = { 0, 0 };

// A blank path name.  Compressed file info records that don't have an
// affiliate path will point to this string.  This is to prevent dynamic
// allocations of a zero byte null terminated string.  Of course, logic has
// to be added to check to see if the "path pointer" points to this string
// when the path is to be "freed".
static TCHAR gszNullPath[ 1 ] = { '\0' };


/*---------------------------------------------------------*/
/*   M A C R O S                                           */
/*---------------------------------------------------------*/

#define Zip_FreePathChk(lpszPath) if ( ( lpszPath ) != NULL && ( lpszPath ) != &gszNullPath[ 0 ] )\
                                     Mem_SubFree( ( lpszPath ) )
#define Zip_FreeRemarkChk(lpszRemark) if ( ( lpszRemark ) != NULL && ( lpszRemark ) != &gszNullPath[ 0 ] )\
                                         Mem_SubFree( ( lpszRemark ) )
#define Zip_GetRemarkLen(lpszRemark) (WORD)FsvStr_GetByteCount( lpszRemark )

#define Zip_GetFBufInfo(lpDir, hbFleIndx) (lpDir->fiArchvFile.lpFBuf)

#define Zip_FileVrfyChk(lpH, lpFile) ( ( ZIP_ERRCODE ) ( lpFile->bHeadrVrfd     ) ? ZIP_NO_ERROR : Zip_VerifyFileHeadr( lpH, lpFile ) )
#define Zip_ArchvVrfyChk(lpH)        ( ( ZIP_ERRCODE ) ( lpH->lpDir->bArchvVrfd ) ? ZIP_NO_ERROR : Zip_VerifyArchv( lpH ) )

#define Zip_GetHPtr(hArchvFile) &gHandleTbl.hlHandleLst[ ( hArchvFile ) - 1 ]

#define Zip_TypSupportsDirEntries(eArchvTyp) ( ( eArchvTyp ) == ZIP_FORMAT_ZIP )

#define Zip_GetFileInfoByts(lpFile, lpFileInfo) ( FsvStr_GetByteCount( ( *lpFile ).lpszPath ) + 1 + ( WORD ) ( ( LPBYTE ) ( *lpFileInfo ).szPath - ( LPBYTE ) lpFileInfo ) )

//   ( lpDir->fiArchvFile.lpFBuf == NULL && lpDir->wActiveCount > 0 ) ||
#define Zip_BadDir(lpDir)\
   ( lpDir->SI.lExtraRemByts    < 0 ||\
     lpDir->SI.lFreedByts       < 0 ||\
     lpDir->SI.lEOFByts         < 0 ||\
     lpDir->SI.lAvailPostn      < 0 ||\
     /*lpDir->dbRemark.lSize      > UINT_MAX ||*/\
     lpDir->dbExtra.lSize       > UINT_MAX ||\
     lpDir->wActiveCount        > lpDir->wUseCount ||\
     ( lpDir->wFileCount > 0 && lpDir->lplpCompFiles == NULL ) ||\
     ( lpDir->wTmpBufSiz > 0 && lpDir->lpTmpBuf == NULL ) )

// Return number of unknown bytes for a given level of LZH file.
#define Zip_GetUnknownByts(bLevel) ( ( WORD ) ( ( ( bLevel ) == 0 ) ? ZIP_MAX_LHA_LEV0_UNKNOWNBYTS : ZIP_MAX_LHA_LEV1_UNKNOWNBYTS ) )

/*---------------------------------------------------------*/
/*   P R O T O T Y P E S                                   */
/*---------------------------------------------------------*/

VOID          WINAPI Zip_CopyPrivateCompInto2PublicStruct( LPCZIP_COMP_INFO, LPZIP_FILE_INFO );
WORD          WINAPI Zip_IsFileInArchive( LPZIP_ARCHIVE_DIR, LPCTSTR );
ZIP_ERRCODE   WINAPI Zip_CopyDataBlk( LPZIP_HANDLE_INFO, LPZIP_DATA_BLK, LPFSVBUFIO_FBUF_INFO, BOOL );
ZIP_ERRCODE   WINAPI Zip_MemWrite( LPZIP_MBUF_INFO, LPVOID, WORD, LPZIP_ERR_CBFUNC, DWORD );
int           _cdecl Zip_PListCompare( const VOID*, const VOID* );
int           WINAPI Zip_TransOpenOptns( WORD );
ZIP_ERRCODE   WINAPI Zip_CloseChk( LPZIP_HANDLE_INFO, BOOL );
ZIP_ERRCODE   WINAPI Zip_ReOpenChk( LPZIP_HANDLE_INFO, WORD );
ZIP_ERRCODE   WINAPI Zip_GetDirInfo( HZIP, LPLPZIP_HANDLE_INFO, ZIP_DIR_ACCESS_OP );
VOID          WINAPI Zip_InitArchiveDir( LPZIP_ARCHIVE_DIR, LPFSVBUFIO_FBUF_INFO, LPCTSTR, ZIP_TYPE, BOOL, BOOL, BOOL );
ZIP_ERRCODE   WINAPI Zip_ScanForFileInDirTable( LPCTSTR, WORD, LPLPZIP_ARCHIVE_DIR );
VOID          WINAPI Zip_InitHandleRec( LPZIP_HANDLE_INFO, LPZIP_ARCHIVE_DIR, WORD, LPZIP_PROG_CBFUNC, DWORD, LPZIP_ERR_CBFUNC, DWORD, LPFNZIP_SPLTDSK_CBFUNC, DWORD );
HZIP          WINAPI Zip_GetNewHandle( VOID );
VOID          WINAPI Zip_FreeHandle( HZIP );
VOID          WINAPI Zip_InitCompFileRec( LPZIP_COMP_INFO, BOOL, BOOL, LPCTSTR );
ZIP_ERRCODE   WINAPI Zip_InsCompFileRec( LPZIP_ARCHIVE_DIR, WORD, LPZIP_COMP_INFO );
ZIP_ERRCODE   WINAPI Zip_DelCompFileRec( LPZIP_ARCHIVE_DIR, WORD );
VOID          WINAPI Zip_FreeComprssdRec( LPZIP_COMP_INFO );
VOID          WINAPI Zip_FreeComprssdInfoRecs( LPZIP_ARCHIVE_DIR );
VOID          WINAPI Zip_FreeDirInfo( LPZIP_ARCHIVE_DIR );
BOOL          WINAPI Zip_ConvertOtherPlatformFileName( LPTSTR, WORD, LPBYTE );
BOOL          WINAPI Zip_DoesDiskHaveSpecialLabel( LPZIP_HANDLE_INFO, LPTSTR, LPWORD );
ZIP_ERRCODE   WINAPI Zip_WriteSpecialLabel( LPZIP_HANDLE_INFO, LPTSTR, WORD );
ZIP_ERRCODE   WINAPI Zip_CreateNextArchiveFile( LPZIP_HANDLE_INFO, WORD );
ZIP_ERRCODE   WINAPI Zip_PutInNthDiskChk( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, WORD, ZIP_SPLIT_DISK_OP );
ZIP_ERRCODE   WINAPI Zip_fReadByts( LPZIP_HANDLE_INFO, LPFSVBUFIO_FBUF_INFO, LPVOID, WORD );
ZIP_ERRCODE   WINAPI Zip_fArchiveRead( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPVOID, WORD );
ZIP_ERRCODE   WINAPI Zip_fArchiveReadByts( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPVOID, WORD );
ZIP_ERRCODE   WINAPI Zip_fArchiveWrt( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPVOID, WORD );
ZIP_ERRCODE   WINAPI Zip_fFlushBuf( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO );
ZIP_ERRCODE   WINAPI Zip_fArchiveSeek( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, WORD, LONG, int );
ZIP_ERRCODE   WINAPI Zip_fArchiveAdvance( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LONG );
ZIP_ERRCODE   WINAPI Zip_fArchiveSetEOF( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, WORD, LONG );
ZIP_ERRCODE   WINAPI Zip_ReadZipHeader( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPZIP_COMP_INFO );
ZIP_ERRCODE   WINAPI Zip_ReadZipDir( LPZIP_HANDLE_INFO );
BYTE          WINAPI Zip_CalcLHAByteChkSum( LPVOID, WORD, BYTE );
WORD          WINAPI Zip_CalcLZHHeadrSiz( BYTE, BYTE );
ZIP_ERRCODE   WINAPI Zip_ReadLHAFileHeader( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO, LPBOOL );
ZIP_ERRCODE   WINAPI Zip_ReadLHAFile( LPZIP_HANDLE_INFO );
ZIP_ERRCODE   WINAPI Zip_BuildInternalDir( LPZIP_HANDLE_INFO );
VOID          WINAPI Zip_SetSharedData( LPZIP_COMP_INFO, LPZIP_SHARED_HEADR, WORD );
ZIP_ERRCODE   WINAPI Zip_WriteZipPath( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPTSTR );
ZIP_ERRCODE   WINAPI Zip_WriteZipHeadr( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPZIP_COMP_INFO, BOOL );
ZIP_ERRCODE   WINAPI Zip_WriteCntrlHeadr( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPZIP_COMP_INFO, LPWORD );
ZIP_ERRCODE   WINAPI Zip_WriteZipEnd( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPLPZIP_COMP_INFO );
ZIP_ERRCODE   WINAPI Zip_WriteLHAHeadr( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO );
ZIP_ERRCODE   WINAPI Zip_WriteFileHeadr( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPZIP_COMP_INFO, BOOL );
ZIP_ERRCODE   WINAPI Zip_WriteArchiveEnd( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPLPZIP_COMP_INFO );
ZIP_ERRCODE   WINAPI Zip_MoveSpecialDataToEnd( LPZIP_HANDLE_INFO );
LONG          WINAPI Zip_CalcOldComprssdFileByts( LPZIP_COMP_INFO );
WORD          WINAPI Zip_CalcEndInfoByts( LPZIP_ARCHIVE_DIR, LPZIP_COMP_INFO );
WORD          WINAPI Zip_CalcTrailrByts( LPZIP_ARCHIVE_DIR );
ZIP_PACK_TYPE WINAPI Zip_SelectPckTyp( ZIP_TYPE, LPZIP_COMP_INFO );
BOOL          WINAPI Zip_IsCompMethComptbl( ZIP_TYPE, ZIP_TYPE, ZIP_PACK_TYPE );
ZIP_ERRCODE   WINAPI Zip_VerifyPckTyp( ZIP_TYPE, ZIP_PACK_TYPE );
ZIP_ERRCODE   WINAPI Zip_CreateTempFile( LPZIP_HANDLE_INFO, LPTSTR, LPZIP_BFILE_INFO, LONG );
ZIP_ERRCODE   WINAPI Zip_CloseTempFile( LPZIP_HANDLE_INFO, LPZIP_BFILE_INFO );
BOOL          WINAPI Zip_Unfiltered( LPTSTR, LPWORD, LPTSTR, LPWORD, LPVOID, TCHAR );
BOOL          WINAPI Zip_ExpRLEWrppr( LPTSTR, LPWORD, LPTSTR, LPWORD, LPVOID, TCHAR );
ZIP_ERRCODE   WINAPI Zip_FilterFile( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPLPFSVBUFIO_FBUF_INFO, LPZIP_FILTER_INFO );
ZIP_ERRCODE   WINAPI Zip_Compress2Pass( LPZIP_HANDLE_INFO, LPFSVBUFIO_FBUF_INFO, LPLPFSVBUFIO_FBUF_INFO, LPZIP_FILTER_FUNC, LPZIP_FILTER_FUNC, LPZIP_FILTER_INFO );
VOID          WINAPI Zip_DeInitCompressExpandChk( LPVOID FAR*, ZIP_PACK_TYPE, BOOL );
ZIP_ERRCODE   WINAPI Zip_InitExpand( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO, LPZIP_FILEIO_FUNC, LONG, LPCTSTR, WORD, BOOL, LPVOID, LONG, WORD, ZIP_CALLBACK_OP, LPZIP_FILTER_INFO );
ZIP_ERRCODE   WINAPI Zip_CompressFile( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO, LPCTSTR, LPCTSTR, LPVOID, WORD, BOOL );
ZIP_ERRCODE   WINAPI Zip_GrowthCheck( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LONG, LONG, LONG );
ZIP_ERRCODE   WINAPI Zip_MoveFileToEnd( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO, LPZIP_COMP_INFO, BOOL );
ZIP_ERRCODE   WINAPI Zip_XFerCompFile( LPZIP_HANDLE_INFO, LPZIP_HANDLE_INFO, LPZIP_COMP_INFO, LPZIP_COMP_INFO, BOOL );
ZIP_ERRCODE   WINAPI Zip_RenameCompFile( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO, LPCTSTR, LPCTSTR );
ZIP_ERRCODE   WINAPI Zip_CreateDataBlk( LPZIP_HANDLE_INFO, LPZIP_DATA_BLK, LPVOID, WORD, BOOL );
ZIP_ERRCODE   WINAPI Zip_SetFileInfo( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO, ZIP_INFO, LPVOID, WORD );
ZIP_ERRCODE   WINAPI Zip_VerifyFileHeadr( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO );
ZIP_ERRCODE   WINAPI Zip_VerifyArchv( LPZIP_HANDLE_INFO );
ULONG         INLINE Zip_Mul32( ULONG, ULONG );
VOID          WINAPI Zip_UpdateKeys( LPZIP_KEYS, BYTE );
VOID          WINAPI Zip_EnDeCryptByts( LPZIP_KEYS, LPBYTE, WORD, BOOL );
ZIP_ERRCODE   WINAPI Zip_DecryptSetup( LPZIP_HANDLE_INFO, LPZIP_KEYS, LPCTSTR, LONG );
ZIP_ERRCODE   WINAPI Zip_EncryptSetup( LPZIP_HANDLE_INFO, LPZIP_KEYS, LPCTSTR, LONG );
ZIP_ERRCODE   WINAPI Zip_EncryptZipFile( LPZIP_HANDLE_INFO, LPLPFSVBUFIO_FBUF_INFO, LPZIP_COMP_INFO, LPCTSTR, BOOL );

ZIP_ERRCODE   WINAPI Zip_ReadArcPakHeader( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO, BYTE );
ZIP_ERRCODE   WINAPI Zip_ReadArcPakRecord( LPZIP_HANDLE_INFO, WORD );
ZIP_ERRCODE   WINAPI Zip_ReadArcPakDir( LPZIP_HANDLE_INFO );
ZIP_ERRCODE   WINAPI Zip_WriteArcPakHeadr( LPZIP_HANDLE_INFO, LPZIP_COMP_INFO );
ZIP_ERRCODE   WINAPI Zip_WriteArcPakDataBlk( LPZIP_HANDLE_INFO, LPZIP_DATA_BLK, WORD, BYTE );
ZIP_ERRCODE   WINAPI Zip_WriteArcPakDataStr( LPZIP_HANDLE_INFO, LPCTSTR, WORD, BYTE );
ZIP_ERRCODE   WINAPI Zip_WriteArcPakPath( LPZIP_HANDLE_INFO, LPCTSTR, WORD );
ZIP_ERRCODE   WINAPI Zip_WriteArcPakEnd( LPZIP_HANDLE_INFO, LPLPZIP_COMP_INFO );

#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

/*---------------------------------------------------------*/
/*   G L O B A L   F U N C T I O N S                       */
/*---------------------------------------------------------*/

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

#ifndef NAV95
ZIP_ERRCODE WINAPI ZipCreate(
   LPCTSTR                 lpszArchvFile,
   ZIP_TYPE               eArchiveType,
   LPHZIP                 lphArchvFile,
   LPZIP_PROG_CBFUNC      lpProgCBFunc,
   DWORD                  dwProgCBData,
   LPZIP_ERR_CBFUNC       lpErrCBFunc,
   DWORD                  dwErrCBData,
   LPFNZIP_SPLTDSK_CBFUNC lpfnSpltDskCB,
   DWORD                  dwSpltDskData
)

{
   LPZIP_HANDLE_INFO    lpH;
   LPZIP_ARCHIVE_DIR    lpDir;
   ZIP_ERRCODE          eErr;
   LPFSVBUFIO_FBUF_INFO lpfiFBuf;
   BOOL                 bUsedOld;
   BOOL                 bNormlCreat = ( lphArchvFile != NULL );

   if ( eArchiveType > ZIP_FORMAT_LHA )
   /* Bad news.  Invalid input parameter(s). */
   {
      return( ZIP_BAD_ARG );
   }

   if ( ( eErr = Zip_ScanForFileInDirTable( lpszArchvFile, (WORD)(ZIP_READWRITE | ZIP_EXCLUSIVE),
                                            &lpDir ) ) != ZIP_NO_ERROR )
   /* Bad news.  The archive file in question has an "active connection". */
   {
      return( eErr );
   }

   if ( bNormlCreat && ( *lphArchvFile = Zip_GetNewHandle() ) == 0 )
   /* Bad news.  No more archive handles are available. */
   {
      return( ZIP_NO_MORE_HANDLES );
   }

   /* Is the "internal directory" from a previous connection being used. */
   bUsedOld = ( lpDir != NULL );

   /* Try to create the archive file. */
   if ( ( eErr = FsvBufIO_fCreat( lpszArchvFile, 0, (WORD)(( bNormlCreat ) ? FSVBUFIO_FILE_BUF_SIZ : 0),
                                  &lpfiFBuf, lpErrCBFunc, dwErrCBData ) ) == ZIP_NO_ERROR )
   {
      if ( !bUsedOld &&
           ( lpDir = Mem_SubMalloc( sizeof( ZIP_ARCHIVE_DIR ) ) ) == NULL )
      /* Bad news, unable to allocate the directory info structure. */
      {
         eErr = ZIP_NO_MEMORY;
      }
      else
      {
         ZIP_HANDLE_INFO HandleRec;

         if ( bNormlCreat )
         /* Get pointer to the newly allocated haned info record. */
         {
            lpH = Zip_GetHPtr( *lphArchvFile );
         }
         else
         /* Special case. Use a temporary local handle record. */
         {
            lpH = &HandleRec;
         }

         /* Store appropriate info in the handle record. */
         Zip_InitHandleRec( lpH, lpDir, ZIP_READWRITE | ZIP_EXCLUSIVE, lpProgCBFunc, dwProgCBData,
                            lpErrCBFunc, dwErrCBData, lpfnSpltDskCB, dwSpltDskData );

         /* Initialize the "internal directory" as being empty. */
         Zip_InitArchiveDir( lpDir, lpfiFBuf, lpszArchvFile, eArchiveType,
                             TRUE, bUsedOld, TRUE );
         if ( eArchiveType != ZIP_FORMAT_ZIP )
            lpDir->SI.fCanBeSplit = FALSE;

         /*****NUKE*****/
         #ifdef MYDEBUG
         if ( bNormlCreat && eErr == ZIP_NO_ERROR )
         {
            TCHAR szTmp[ 200 ];

            lpH->hsDirHandleSt = ZIP_DIR_OPENED;

            if ( eArchiveType == ZIP_FORMAT_ARC || eArchiveType == ZIP_FORMAT_PAK )
            {
               lstrcpy( szTmp, "This is archive extra data for " );
               lstrcat( szTmp, lpszArchvFile );
               lstrcat( szTmp, ".  Hi mom." );
               eErr = ZipSetInfo( *lphArchvFile, ZIP_EXTRAINFO, szTmp, FsvStr_GetByteCount( szTmp ) );
            }

            if ( eErr == ZIP_NO_ERROR )
            {
               lstrcpy( szTmp, "This is archive remark for " );
               lstrcat( szTmp, lpszArchvFile );
               lstrcat( szTmp, "." );
               eErr = ZipSetInfo( *lphArchvFile, ZIP_COMMENT, szTmp, FsvStr_GetByteCount( szTmp ) );
            }

            lpH->hsDirHandleSt = ZIP_DIR_INIT_STATE;
         }
         #endif
         /*****NUKE*****/

         /* Write trailer record for an empty file. */
         eErr = Zip_WriteArchiveEnd( lpH, &lpfiFBuf, lpDir->lplpCompFiles );

         if ( eErr == ZIP_NO_ERROR )
         /* Flush the data to disk. */
         {
            eErr = FsvBufIO_fFlush( lpfiFBuf, lpErrCBFunc, dwErrCBData );
         }

         if ( !FsvIO_IsCritErr( eErr ) &&
              ( !bNormlCreat || eErr != ZIP_NO_ERROR ) )
         /* Close the file for the "quick create" or because an */
         /* error was detected.                                 */
         {
            FsvBufIO_fClose( lpfiFBuf, FALSE, lpErrCBFunc, dwErrCBData );

            if ( eErr != ZIP_NO_ERROR )
            /* An error was detected so delete the file. */
            {
               FsvIO_DeleteFile( lpszArchvFile, FALSE, NULL, 0 );
            }
         }

         if ( bNormlCreat && eErr == ZIP_NO_ERROR )
         {
            /* Save final state info about the archive file. */
            lpDir->SI.dwFileSiz   = FsvBufIO_fTell( lpfiFBuf );
            lpDir->SI.lEOFByts    = lpDir->SI.dwFileSiz;
            lpDir->SI.lAvailPostn = 0;

            if ( eErr == ZIP_NO_ERROR )
            /* The archive file has one more "active" connection. */
            {
               lpDir->wUseCount++;
               lpDir->wActiveCount++;
               lpH->hsDirHandleSt = ZIP_DIR_OPENED;
            }
         }

         if ( !bUsedOld && ( !bNormlCreat || eErr != ZIP_NO_ERROR ) )
         /* Free all dynamically allocated memory for the "quick create" */
         /* or because an error was detected.                            */
         {
            Zip_FreeDirInfo( lpDir );
         }
      }
   }

   if ( bNormlCreat && eErr != ZIP_NO_ERROR )
   /* An error occurred after a handle was allocated so */
   /* free that handle.                                 */
   {
      Zip_FreeHandle( *lphArchvFile );
      *lphArchvFile = 0;
   }

   return( eErr );
} /* ZipCreate */
#endif  // NOT NAV95



//==========================================================================
//
// ZIP_ERRCODE ZipOpen( lpszArchvFile, lphArchvFile, wOpenOptns, lpProgCBFunc,
//                      dwProgCBData, lpErrCBFunc, dwErrCBData, lpfnSpltDskCB,
//                      dwSpltDskData )
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
   LPCTSTR                 lpszArchvFile,
   LPHZIP                 lphArchvFile,
   WORD                   wOpenOptns,
   LPZIP_PROG_CBFUNC      lpProgCBFunc,
   DWORD                  dwProgCBData,
   LPZIP_ERR_CBFUNC       lpErrCBFunc,
   DWORD                  dwErrCBData,
   LPFNZIP_SPLTDSK_CBFUNC lpfnSpltDskCB,
   DWORD                  dwSpltDskData
)

{
   LPZIP_HANDLE_INFO lpH;
   LPZIP_ARCHIVE_DIR lpDir;
   ZIP_ERRCODE       eErr;

   if ( lpszArchvFile == NULL || lphArchvFile == NULL )
   /* Bad news, the user supplied parameters are invalid. */
   {
      return( ZIP_BAD_ARG );
   }

   if ( ( eErr = Zip_ScanForFileInDirTable( lpszArchvFile, (WORD)( wOpenOptns & ZIP_ACCESS_OPTNS ),
                                            &lpDir ) ) != ZIP_NO_ERROR )
   /* Bad news.  A connection has already been made to the archive */
   /* and a conflict in access privileges has been detected.       */
   {
      return( eErr );
   }

   if ( ( *lphArchvFile = Zip_GetNewHandle( ) ) == 0 )
   /* Bad news.  No more archive handles are available. */
   {
      return( ZIP_NO_MORE_HANDLES );
   }

   /* Get pointer to affiliate handle info. */
   lpH = Zip_GetHPtr( *lphArchvFile );

   if ( lpDir == NULL )
   /* Try to make the initial "connection" with the specified */
   /* archive file.                                           */
   {
      LPFSVBUFIO_FBUF_INFO lpfiFBuf;

      /* Try to open archive file in question. */
      eErr = FsvBufIO_fOpen( lpszArchvFile, Zip_TransOpenOptns( wOpenOptns ),
                             FSVBUFIO_FILE_BUF_SIZ, &lpfiFBuf, lpErrCBFunc, dwErrCBData );

      if ( eErr == ZIP_NO_ERROR )
      /* Try to allocate the directory info record and then parse the */
      /* archive file to construct the "internal directory".          */
      {
         if ( ( lpDir = Mem_SubMalloc( sizeof( ZIP_ARCHIVE_DIR ) ) ) == NULL )
         /* Bad news, unable to allocate the directory info structure. */
         {
            eErr = ZIP_NO_MEMORY;
         }
         else
         {
            /* Init handle info. */
            Zip_InitHandleRec( lpH, lpDir, wOpenOptns, lpProgCBFunc, dwProgCBData,
                               lpErrCBFunc, dwErrCBData, lpfnSpltDskCB, dwSpltDskData );

            /* Initialize the "internal directory" as being empty. */
            Zip_InitArchiveDir( lpDir, lpfiFBuf, lpszArchvFile, ZIP_FORMAT_NONE,
                                FALSE, FALSE, FALSE );

            /* Try to parse the archive file to construct the */
            /* "internal directory".                          */
            if ( ( eErr = Zip_BuildInternalDir( lpH ) ) != ZIP_NO_ERROR )
            /* Bad news.  The file in question is not a valid archive */
            /* file or an error was detected.  Free all dynamic data  */
            /* allocated.                                             */
            {
			   if ( lpDir->fiArchvFile.lpFBuf != NULL )
				   {
				   FsvBufIO_fClose( lpDir->fiArchvFile.lpFBuf, FALSE, lpErrCBFunc, dwErrCBData );
				   lpDir->fiArchvFile.lpFBuf = NULL;
				   }

               Zip_FreeDirInfo( lpDir );
			   lpDir = NULL;
            }
         }

         if ( eErr != ZIP_NO_ERROR && lpDir && lpDir->fiArchvFile.lpFBuf != NULL )
         // The allocation or archive file parse failed so close the
         // file in question.
         {
            FsvBufIO_fClose( lpDir->fiArchvFile.lpFBuf, FALSE, lpErrCBFunc, dwErrCBData );
         }
      }
   }
   else
   {
      /* Init handle info. */
      Zip_InitHandleRec( lpH, lpDir, wOpenOptns, lpProgCBFunc, dwProgCBData,
                         lpErrCBFunc, dwErrCBData, lpfnSpltDskCB, dwSpltDskData );

      /* If necessary, re-open the archive file. */
      eErr = Zip_ReOpenChk( lpH, wOpenOptns );
   }

   if ( eErr != ZIP_NO_ERROR )
   /* An error occurred after a handle was allocated so */
   /* free that handle.                                 */
   {
      Zip_FreeHandle( *lphArchvFile );
      *lphArchvFile = 0;
   }
   else
   {
      /* The archive file has one more "active" connection. */
      lpDir->wUseCount++;
      lpDir->wActiveCount++;
      lpH->hsDirHandleSt = ZIP_DIR_OPENED;
   }

   return( eErr );
} /* ZipOpen */



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
//    ZIP_BAD_ARG       - The specified handle is invalid.
//    ZIP_ACCESS_DENIED - The archive file associated with the handle has
//                        been opened with read/write privileges.
//    FSVERR_...        - The appropriate File Services error code.
//
//==========================================================================

ZIP_ERRCODE WINAPI ZipReOpen(
   HZIP hArchvFile,
   WORD wOpenOptns
)

{
   ZIP_ERRCODE       eErr;
   LPZIP_HANDLE_INFO lpH;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_REOPEN ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle or access conflict. */
   {
      return( eErr );
   }

   if ( lpH->hsDirHandleSt != ZIP_DIR_TMP_CLOSED )
   /* Bad news.  This connection has not been temporarily closed. */
   {
      return( ZIP_BAD_ARG );
   }

   if ( ( wOpenOptns & ZIP_READWRITE ) && lpH->lpDir->wActiveCount > 0 )
   /* We're trying to open the file with read/write privileges and  */
   /* someone else has it opened.                                   */
   {
      return( ZIP_ACCESS_DENIED );
   }

   /* If necessary, re-open the archive file. */
   eErr = Zip_ReOpenChk( lpH, wOpenOptns );

   if ( eErr == ZIP_NO_ERROR )
   /* The connection has been successfully re-established so   */
   /* update the appropriate states in the handle info record. */
   {
       lpH->hsDirHandleSt = ZIP_DIR_OPENED;
       lpH->lpDir->wActiveCount++;
   }

   return( eErr );
} /* ZipReOpen */



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
//==========================================================================

ZIP_ERRCODE WINAPI ZipClose(
   HZIP hArchvFile
)

{
   LPZIP_HANDLE_INFO lpH;
   ZIP_ERRCODE       eErr;
   LPZIP_ARCHIVE_DIR lpDir;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_CLOSE ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle. */
   {
      return( eErr );
   }

   /* Make local copy. */
   lpDir = lpH->lpDir;

   if ( lpH->hsDirHandleSt == ZIP_DIR_OPENED )
   /* If relevant, close the "open" archive file. */
   {
      eErr = Zip_CloseChk( lpH, lpDir->wUseCount > 1 );
   }

   /* Decrement the number of connections made to the archive file. */
   lpDir->wUseCount--;

   /* Free the archive handle. */
   Zip_FreeHandle( hArchvFile );

   if ( lpDir->wUseCount == 0 )
   /* The last "connection" to the archive file has been severed */
   /* so free the "internal directory".                          */
   {
      Zip_FreeDirInfo( lpDir );
   }

   return( eErr );
} /* ZipClose */



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
)

{
   LPZIP_HANDLE_INFO lpH;
   ZIP_ERRCODE       eErr;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_CLOSE ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle. */
   {
      return( eErr );
   }

   if ( lpH->hsDirHandleSt == ZIP_DIR_OPENED )
   /* If relevant, close the "open" archive file. */
   {
      eErr = Zip_CloseChk( lpH, TRUE );
      lpH->hsDirHandleSt = ZIP_DIR_TMP_CLOSED;
   }

   return( eErr );
} /* ZipTempClose */



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
// Note:  I will only display the hour glass cursor at the appropriate
//        phases of the "pack" iff the "progress callback pointer" is
//        not NULL.  If this pointer is NULL then the caller has assummed
//        responsibility for indicating that a time comsuming operation
//        is underway.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI ZipPack(
   HZIP hArchvFile
)

{
   LPZIP_HANDLE_INFO    lpH;
   LPZIP_ARCHIVE_DIR    lpDir;
   ZIP_ERRCODE          eErr;
   int                  nRatio;
   LPFSVBUFIO_FBUF_INFO lpFBuf;
   HCURSOR              hOrigCurs;
   BOOL                 bShowProg;
   HCURSOR              hWaitCurs;
   LPLPZIP_COMP_INFO    lplpSrtdFiles = NULL;
   LONG                 lSizCount     = 0;
   LONG                 lTotalSiz     = 0;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_READ ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle or access conflict. */
   {
      return( eErr );
   }

   /* Get pointer to directory info. */
   lpDir = lpH->lpDir;

   if ( !lpDir->SI.bDirty )
   /* The archive file has not been modified so don't do the pack. */
   {
      return( ZIP_NO_ERROR );
   }

   /* Init pointer dependent locals. */
   lpFBuf    = lpDir->fiArchvFile.lpFBuf;
   bShowProg = ( lpH->lpProgCBFunc != NULL );

   if ( bShowProg )
   /* Display the hour glass cursor while "verifying zip headers" */
   /* and moving extra/remark data.                               */
   {
      hWaitCurs = LoadCursor( NULL, MAKEINTRESOURCE( IDC_WAIT ) );
      hOrigCurs = SetCursor( hWaitCurs );
   }

   if ( !lpDir->SI.fIsSplit )
   {
      // If necessary, get the size of the extra data for all of the
      // compressed files contained in the archive file.  If working
      // with a file on a floppy flush the i/o buffer.
      if ( ((eErr = Zip_ArchvVrfyChk( lpH )) != ZIP_NO_ERROR) ||
           (lpDir->SI.fCanBeSplit && ((eErr = Zip_fFlushBuf( lpH, &lpFBuf )) != ZIP_NO_ERROR)) )
      {
         // IF ERROR (?)
         // DOESN'T DO ANYTHING.
         //
      }
      else
      if ( ( lpDir->wFileCount > 0 &&
             ( lplpSrtdFiles = ( LPLPZIP_COMP_INFO )
                               Mem_SubMalloc( lpDir->wFileCount * sizeof( LPZIP_COMP_INFO ) ) ) == NULL ) ||
           ( lpDir->lpTmpBuf = ( LPBYTE ) Mem_SubMalloc( ZIP_COPY_BUFFER_SIZE ) ) == NULL )
      {
         eErr = ZIP_NO_MEMORY;
      }
      else
      {
         /* Store size of buffer allocated. */
         lpDir->wTmpBufSiz = ZIP_COPY_BUFFER_SIZE;

         if ( lpDir->wFileCount > 0 )
         /* Make a copy of the pointer list and sort it. */
         {
            _fmemcpy( lplpSrtdFiles, lpDir->lplpCompFiles,
                      lpDir->wFileCount * sizeof( LPZIP_COMP_INFO ) );
            qsort( lplpSrtdFiles, lpDir->wFileCount, sizeof( LPZIP_COMP_INFO ),
                   Zip_PListCompare );
         }

         if ( bShowProg )
         {
            LPLPZIP_COMP_INFO lplpFiles;
            WORD              wIndx;

            /* Sum all of the compressed files. */
            for ( wIndx = lpDir->wFileCount, lplpFiles = lpDir->lplpCompFiles;
                  wIndx > 0; wIndx--, lplpFiles++ )
            {
               lTotalSiz += lplpFiles[ 0 ]->dbComprssdFile.lSize;
            }

            /* Determine if multiplying lTotalSiz by 100 will produce */
            /* overflow.  It's this or use floating-point.            */
            nRatio = ( int )( lTotalSiz / ( ZIP_MAXLONG / 100 ) );
            nRatio = ( nRatio < 1 ) ? 1 : nRatio;

            /* ???? */
            lTotalSiz /= nRatio;
         }

         /* Move the files in the archive "forward" to eliminate "gaps".  This */
         /* is a two pass process.  The first pass is to move the file(s)      */
         /* stored in the archive file forward, and the 2nd pass is to copy    */
         /* the file(s) stored in the temp file to the end of the file(s)      */
         /* that were just moved.                                              */

         /* Copy all "remark" and "extra" data to the end of the current file */
         /* or to the temp file (if one is being used) so as to keep it from  */
         /* getting overwritten during the "pack".                            */
         if ( ( eErr = Zip_MoveSpecialDataToEnd( lpH ) ) == ZIP_NO_ERROR &&
              ( eErr = FsvBufIO_fSeek( lpFBuf, lpDir->dbExeHeadr.lSize, SEEK_SET,
                                       lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == lpDir->dbExeHeadr.lSize )
         {
            WORD              wIndx;
            LPLPZIP_COMP_INFO lplpFiles;

            if ( bShowProg )
            /* Restore the original cursor. */
            {
               SetCursor( hOrigCurs );
            }

            /* Loop to "pack" the files. */
            for ( eErr = ZIP_NO_ERROR, wIndx = lpDir->wFileCount, lplpFiles = lplpSrtdFiles;
                  wIndx > 0 && eErr == ZIP_NO_ERROR;
                  wIndx--, lplpFiles++ )
            {
               if ( bShowProg )
               /* Inform the user of the "progress". */
               {
                  ZIP_PROGRESSINFO piProgInfo = { ( WORD ) ( lSizCount * 100 / max( 1, lTotalSiz ) ),
                                                  ZIP_PACK_FILE, lpH->dwProgCBData };

                  /* Increment counter. */
                  lSizCount += ( lplpFiles[ 0 ]->dbComprssdFile.lSize / nRatio );

                  if ( wIndx == 0 )
                  /* Fudge ending to account for rounding errors. */
                  {
                     lSizCount = lTotalSiz;
                  }

                  if ( lpH->lpProgCBFunc( &piProgInfo ) == ZIP_PROG_CB_ABORT )
                  /* Not very wise.  The calling code requests termination */
                  /* of the "pack".                                        */
                  {
                     eErr = ZIP_ABORTED;
                     break;
                  }
               }

               /* If necessary, write out the compressed file header. */
               if ( ( eErr = Zip_WriteFileHeadr( lpH, &lpFBuf, lplpFiles[ 0 ], TRUE ) ) == ZIP_NO_ERROR )
               /* If necessary, "move" the compressed file data. */
               {
                  eErr = Zip_CopyDataBlk( lpH, &lplpFiles[ 0 ]->dbComprssdFile, lpFBuf, FALSE );
               }
            }

            if ( bShowProg )
            /* Inform user that the file has been packed. */
            {
               ZIP_PROGRESSINFO piProgInfo = { 100, ZIP_PACK_FILE, lpH->dwProgCBData };

               if ( lpH->lpProgCBFunc( &piProgInfo ) == ZIP_PROG_CB_ABORT )
               /* Not very wise.  The calling code requests termination */
               /* of the "pack".                                        */
               {
                  eErr = ZIP_ABORTED;
               }
            }
         }
      }
   }
   else
   // Multi-disk ZIP files are not "packed".  It is too much trouble
   // to collapse the holes.  Instead we simply write out the latest
   // central directory and EOF record.
   {
      // Position file pointer at the beginning of the Central Directory.
      if ( ( eErr = Zip_fArchiveSeek( lpH, &lpFBuf, lpDir->SI.wStrtDisk, lpDir->SI.lAvailPostn, SEEK_SET ) == lpDir->SI.lAvailPostn ) )
      {
         eErr = ZIP_NO_ERROR;
      }
   }

   if ( eErr == ZIP_NO_ERROR )
   /* Try to write the end of file info. */
   {
      if ( bShowProg )
      /* Display the hour glass cursor while writing the end */
      /* of file info.                                       */
      {
         SetCursor( hWaitCurs );
      }

      // Try to write the end of file info.
      if ( ( eErr = Zip_WriteArchiveEnd( lpH, &lpFBuf, ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP ) ? lpDir->lplpCompFiles : lplpSrtdFiles ) ) == ZIP_NO_ERROR )
      {
         // Try to "truncate" the end of the file.
         eErr = FsvBufIO_fSetEOF( lpFBuf, FsvBufIO_fTell( lpFBuf ), lpH->lpCritCBFunc,
                                  lpH->dwCritCBData );
      }
   }

   if ( lplpSrtdFiles != NULL )
   /* Release the sorted list of the pointers. */
   {
      Mem_SubFree( lplpSrtdFiles );
   }

   if ( lpDir->lpTmpBuf != NULL )
   /* Release the buffer used by Zip_CopyDataBlk(). */
   {
      Mem_SubFree( lpDir->lpTmpBuf );
      lpDir->lpTmpBuf   = NULL;
      lpDir->wTmpBufSiz = 0;
   }

   if ( eErr != ZIP_NO_ERROR )
   /* Restore the original contents of the archive file. */
   {
      lpDir->SI.bBadState = TRUE;
      FsvBufIO_fSetEOF( lpFBuf, lpDir->SI.dwFileSiz, lpH->lpCritCBFunc, lpH->dwCritCBData );
   }

   // Update the archive file state variables.
   lpDir->SI.dwFileSiz   = FsvBufIO_fTell( lpDir->fiArchvFile.lpFBuf );
   lpDir->SI.lAvailPostn = ( !lpDir->SI.fIsSplit ) ? lpDir->SI.dwFileSiz : lpDir->SI.lCntrlDirOff;
   lpDir->SI.lFreedByts  = 0;
   lpDir->SI.lEOFByts    = 0;

   if ( bShowProg )
   /* Restore the original cursor. */
   {
      SetCursor( hOrigCurs );
   }

   return( eErr );
} /* ZipPack */
#endif          // NOT NAV95



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
)

{
   WORD              wInfoSiz;
   LPVOID            lpInfo;
   LPZIP_HANDLE_INFO lpH;
   LPZIP_ARCHIVE_DIR lpDir;
   ZIP_ERRCODE       eErr;
   LONG              lTotal = 0;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_READ ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle or access conflict. */
   {
      return( eErr );
   }

   /* Make local copy. */
   lpDir = lpH->lpDir;

   /* Handle the various information queries. */
   switch ( eInfoType )
   {
      case ZIP_FILEFORMAT:
         wInfoSiz = sizeof( lpDir->eArchvTyp );
         lpInfo   = &lpDir->eArchvTyp;
      break;

      case ZIP_FILECOUNT:
         wInfoSiz = sizeof( lpDir->wFileCount );
         lpInfo   = &lpDir->wFileCount;
      break;

      case ZIP_ORIGINALSIZE:
      {
         LPLPZIP_COMP_INFO lplpCompFiles;
         WORD              wIndx;

         wInfoSiz = sizeof( lTotal );
         lpInfo   = &lTotal;

         for ( wIndx = lpDir->wFileCount, lplpCompFiles = lpDir->lplpCompFiles;
               wIndx > 0;
               wIndx--, lplpCompFiles++ )
         {
            lTotal += lplpCompFiles[ 0 ]->lOrigSiz;
         }
      }
      break;

      case ZIP_COMPRESSEDSIZE:
      {
         LPLPZIP_COMP_INFO lplpCompFiles;
         WORD              wIndx;

         wInfoSiz = sizeof( lTotal );
         lpInfo   = &lTotal;

         for ( wIndx = lpDir->wFileCount, lplpCompFiles = lpDir->lplpCompFiles;
               wIndx > 0;
               wIndx--, lplpCompFiles++ )
         {
            lTotal += lplpCompFiles[ 0 ]->dbComprssdFile.lSize;
         }
      }
      break;

      case ZIP_FREESPACE:
         wInfoSiz = sizeof( lTotal );
         lpInfo   = &lTotal;
      break;

      case ZIP_COMMENT:
//       wInfoSiz = ( WORD ) lpDir->dbRemark.lSize;
         wInfoSiz = ( WORD ) Zip_GetRemarkLen( lpDir->lpszRemark );

         if ( lpBuf != NULL && wBufSize > 0 )
         {
            WORD wNumByts = min( wBufSize - 1, wInfoSiz );

            if ( wNumByts >= 0 )
            {
//             if ( ( eErr = FsvBufIO_fSeek( Zip_GetFBufInfo( lpDir, lpDir->dbRemark.hbFleIndx ),
//                                           lpDir->dbRemark.lStart, SEEK_SET, lpH->lpCritCBFunc,
//                                           lpH->dwCritCBData ) ) == lpDir->dbRemark.lStart )
//             {
//                eErr = Zip_fReadByts( lpH, Zip_GetFBufInfo( lpDir, lpDir->dbRemark.hbFleIndx ),
//                                      lpBuf, wNumByts );
//             }

               _fmemcpy( lpBuf, lpDir->lpszRemark, wNumByts );
            }

            // Null terminate the "remark".
            ( ( LPBYTE ) lpBuf )[ wNumByts ] = '\0';
         }

         /* The data has been transferred. */
         lpBuf = NULL;
      break;

      case ZIP_EXTRAINFO:
         wInfoSiz = ( WORD ) lpDir->dbExtra.lSize;

         if ( lpBuf != NULL && wBufSize > 0 && wInfoSiz > 0 )
         {
            if ( ( eErr = FsvBufIO_fSeek( Zip_GetFBufInfo( lpDir, lpDir->dbExtra.hbFleIndx ), lpDir->dbExtra.lStart,
                                          SEEK_SET, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == lpDir->dbExtra.lStart )
            {
               eErr = Zip_fReadByts( lpH, Zip_GetFBufInfo( lpDir, lpDir->dbExtra.hbFleIndx ),
                                     lpBuf, (WORD)min( wBufSize, wInfoSiz ) );
            }
         }

         /* The data has been transferred. */
         lpBuf = NULL;
      break;

      default :
         eErr = ZIP_BAD_ARG;
      break;
   }

   if ( eErr == ZIP_NO_ERROR )
   {
      if ( lpwInfoByts != NULL )
      /* Return the number of bytes of information that is being returned. */
      {
         *lpwInfoByts = wInfoSiz;
      }

      if ( lpBuf != NULL )
      /* Copy as much of the requested info as possible. */
      {
         _fmemcpy( lpBuf, lpInfo, min( wBufSize, wInfoSiz ) );
      }
   }

   return( eErr );
} /* ZipGetInfo */



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
// Globals:
//    gszNullPath - The zero byte null terminated string used to prevent
//                  excessive memory allocations.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    ZIP_BAD_ARG  - The specified handle is invalid or it has been
//                   temporarily closed, or one of the input parameter(s)
//                   is invalid.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI ZipSetInfo(
   HZIP     hArchvFile,
   ZIP_INFO eInfoType,
   LPVOID   lpBuf,
   WORD     wBufSize
)

{
   LPZIP_HANDLE_INFO lpH;
   LPZIP_ARCHIVE_DIR lpDir;
   ZIP_ERRCODE       eErr;

   if ( lpBuf == NULL || wBufSize == 0 )
   /* Bad news, the user supplied parameters are invalid. */
   {
      return( ZIP_BAD_ARG );
   }

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_READ ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle or access conflict. */
   {
      return( eErr );
   }

   /* Make local copy. */
   lpDir = lpH->lpDir;

   switch ( eInfoType )
   {
      case ZIP_COMMENT:
      {
         LPTSTR lpszTmp;

//       // Try to store the comment at the end of the file.
//       eErr = Zip_CreateDataBlk( lpH, &lpDir->dbRemark, lpBuf, wBufSize,
//                                 TRUE );

         if ( ( ( LPBYTE ) lpBuf )[ 0 ] == '\0' )
         // Nuke the existing "remark".
         {
            Zip_FreeRemarkChk( lpDir->lpszRemark );
            lpDir->lpszRemark = &gszNullPath[ 0 ];
         }
         else
         if ( ( lpszTmp = ( LPTSTR ) Mem_SubMalloc( wBufSize + 1 ) ) == NULL )
         {
            eErr = ZIP_NO_MEMORY;
         }
         else
         {
            Zip_FreeRemarkChk( lpDir->lpszRemark );
            lpDir->lpszRemark = lpszTmp;
            _fmemcpy( lpszTmp, lpBuf, wBufSize );
            lpszTmp[ wBufSize ] = '\0';
         }
      }
      break;

      case ZIP_EXTRAINFO:
         /* Try to store the extra data at the end of the file. */
         eErr = Zip_CreateDataBlk( lpH, &lpDir->dbExtra, lpBuf, wBufSize,
                                   TRUE );
      break;

      default:
         eErr = ZIP_BAD_ARG;
      break;
   }

   return( eErr );
} /* ZipSetInfo */

#endif          // NOT NAV95



// *************************NOT IMPLEMENTED***************************
// Moves a file index.  Does not change the physical file at all.
#ifdef IfImplemented
ZIP_ERRCODE WINAPI ZipReorderIndex(
   HZIP hArchvFile,
   WORD wOldIndex,
   WORD wNewIndex
)

{
   return( ZIP_BAD_ARG );
}
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

#ifndef NAV95
ZIP_ERRCODE WINAPI ZipAddFile(
   HZIP            hArchvFile,
   LPCTSTR         lpszFullName,
   LPCTSTR         lpszPathInfo,
   LPCTSTR         lpszNewName,
   ZIP_PACK_TYPE   ePackType,
   LPCTSTR         lpszKey,
   LPVOID          lpExtra,
   WORD            wExtraSiz,
   ZIP_ADD_OPTIONS eAddOPtns
)

{
   TCHAR             szPath[ 2 ] = { '\0' };
   TCHAR             szName[ FSV_LFILENAMEBUF_LEN ];
   LPZIP_COMP_INFO   lpFile;
   LPZIP_HANDLE_INFO lpH;
   LPZIP_ARCHIVE_DIR lpDir;
   ZIP_ERRCODE       eErr;
   LPCTSTR           lpszPath = ( lpszPathInfo != NULL ) ? lpszPathInfo : szPath;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_WRITE ) ) != ZIP_NO_ERROR )
   // Bad news.  Invalid handle or access conflict.
   {
      return( eErr );
   }

   // Make local copy.
   lpDir = lpH->lpDir;

   if ( lpszFullName == NULL ||
        ( eErr = Zip_VerifyPckTyp( lpDir->eArchvTyp, ePackType ) ) != ZIP_NO_ERROR ||
        ( lpszPathInfo != NULL && lstrlen( lpszPathInfo ) > 0 && !FsvUtil_ValidateFullFileName( lpszPathInfo, FALSE ) ) ||
        ( lpszNewName != NULL && !FsvUtil_ValidateFileName( lpszNewName, TRUE ) ) )
   // Bad news.  The input parameter(s) are invalid.
   {
      return( ZIP_BAD_ARG );
   }

   if ( ( lpFile = ( LPZIP_COMP_INFO ) Mem_SubMalloc( sizeof( ZIP_COMP_INFO ) ) ) == NULL )
   // Bad news.  Not enough memory for the compressed file info record.
   {
      return( ZIP_NO_MEMORY );
   }

   if ( lpszNewName == NULL )
   // Use the name that is part of the file to compress.
   {
      if (lpDir->eArchvTyp == ZIP_FORMAT_ARC)
      {
         TCHAR       szTempName[FSV_LFILENAMEBUF_LEN];

         // LFN's can't exist in ARC files, so convert to short format first
         GetShortPathName( lpszFullName, szTempName, sizeof(szTempName) );

         // get just the name
         FsvUtil_GetPathMask( szTempName, NULL, szName, FALSE );
         lpszNewName = szName;
      }
      else
      {
         FsvUtil_GetPathMask( lpszFullName, NULL, szName, FALSE );
         lpszNewName = szName;
      }
   }

   if ( lpszPath[ 0 ] == '\\' )
   // Ignore the leading backslash.
   {
      lpszPath = CharNext(lpszPath);
   }

   // Initialize all info fields.  Mark the record as being "dirty" and that
   // it has been "verified".
   Zip_InitCompFileRec( lpFile, TRUE, TRUE, lpszNewName );
   lpFile->ePckTyp = ePackType;

   if ( lstrlen( lpszPath ) > 0 )
   // Try to dynamically allocate the path.
   {
      if ( ( lpFile->lpszPath = ( LPTSTR ) Mem_SubMalloc( FsvStr_GetByteCount( lpszPath ) + 1 ) ) == NULL )
      // Bad news, unable to allocate the path.
      {
         eErr = ZIP_NO_MEMORY;
      }
      else
      {
         lstrcpy( lpFile->lpszPath, lpszPath );
      }
   }

   // Try to add the compressed info record to the end of the "list".
   if ( eErr == ZIP_NO_ERROR &&
        ( eErr =  Zip_InsCompFileRec( lpDir, ( WORD ) -1, lpFile ) ) == ZIP_NO_ERROR )
   // Try to "compress" the file.
   {
      /*****NUKE*****/
      #ifdef MYDEBUG
      TCHAR szExtra[ FSV_PATHBUF_LEN + FSV_PATHBUF_LEN ];

      if ( lpExtra == NULL )
      {
         lstrcpy( szExtra, "The extra data is for " );
         lstrcat( szExtra, lpszFullName );
         lstrcat( szExtra, ".  Hi mom." );

         lpExtra   = szExtra;
         wExtraSiz = FsvStr_GetByteCount( szExtra );
      }
      #endif
      /*****NUKE*****/

      eErr = Zip_CompressFile( lpH, lpFile, lpszFullName, /*FsvBufIO_fRead, FsvBufIO_fSeek,*/
                               lpszKey, lpExtra, wExtraSiz, TRUE );

      /*****NUKE*****/
      #ifdef MYDEBUG
      if ( eErr == ZIP_NO_ERROR )
      {
         TCHAR szRemark[ FSV_PATHBUF_LEN + FSV_PATHBUF_LEN ];

         lstrcpy( szRemark, "This is a remark for " );
         lstrcat( szRemark, lpszFullName );
         lstrcat( szRemark, "." );

         eErr = ZipSetFileInfo( hArchvFile, NULL, lpDir->wFileCount - 1,
                                ZIP_COMMENT, ( LPVOID ) szRemark,
                                FsvStr_GetByteCount( szRemark ) );
      }
      #endif
      /*****NUKE*****/

      if ( eErr != ZIP_NO_ERROR )
      // Free the allocated compressed info record and remove it's
      // pointer from the pointer list.
      {
         Zip_DelCompFileRec( lpDir, (WORD)(lpDir->wFileCount - 1) );
      }
   }
   else
   // Free dynamically allocated path and compressed info record.
   {
      Zip_FreeComprssdRec( lpFile );
   }

   return( eErr );
} // ZipAddFile

#endif          // NOT NAV95



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

#ifndef NAV95
ZIP_ERRCODE WINAPI ZipReplaceFile(
   HZIP   hArchvFile,
   LPCTSTR lpszName,
   WORD   wIndex,
   LPCTSTR lpszSrcFile,
   LPCTSTR lpszNewPath,
   LPCTSTR lpszNewName,
   LPCTSTR lpszKey
)

{
   ZIP_COMP_INFO     fiNewFile;
   LPZIP_COMP_INFO   lpFile;
   LPZIP_HANDLE_INFO lpH;
   LPZIP_ARCHIVE_DIR lpDir;
   ZIP_ERRCODE       eErr;
   BOOL              fFreeOldPath = FALSE;

   if ( lpszSrcFile == NULL ||
        ( lpszNewPath != NULL && lstrlen(lpszNewPath) > 0 && !FsvUtil_ValidateFullFileName( lpszNewPath, FALSE ) ) ||
        ( lpszNewName != NULL && !FsvUtil_ValidateFileName( lpszNewName, TRUE ) ) )
   // Bad news, the user supplied parameters are invalid.
   {
      return( ZIP_BAD_ARG );
   }

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_WRITE ) ) != ZIP_NO_ERROR )
   // Bad news.  Invalid handle or access conflict.
   {
      return( eErr );
   }

   // Make local copy.
   lpDir = lpH->lpDir;

   if ( lpszName != NULL )
   // Try to find the specified file in the "internal directory".
   {
      wIndex = Zip_IsFileInArchive( lpDir, lpszName );
   }

   if ( wIndex >= lpDir->wFileCount )
   // Bad news.  The specified index is invalid.
   {
      return( ZIP_BAD_ARG );
   }

   // If necessary, get the size of the extra data for all of the
   // compressed files contained in the archive file.
   if ( ( eErr = Zip_ArchvVrfyChk( lpH ) ) != ZIP_NO_ERROR )
   {
      return( eErr );
   }

   // Make local copy of the file info record in case an error is
   // detected and the old states need to be restored.
   lpFile    = lpDir->lplpCompFiles[ wIndex ];
   fiNewFile = *lpFile;

   if ( lpszNewPath != NULL )
   // Try to dynamically allocate the path.
   {
      LPCTSTR  lpszWork;

      if ( (lpszWork = CharNext(lpszNewPath)) && *lpszWork == ':' && lstrlen(lpszNewPath) > 0 )
      // Ignore the drive letter.
      {
         lpszNewPath = CharNext(lpszWork);
      }

      if ( lpszNewPath[ 0 ] == '\\' )
      // Ignore the leading backslash.
      {
         lpszNewPath = CharNext(lpszNewPath);
      }

      if ( lstrlen(lpszNewPath) == 0 )
      // There is no new path.
      {
         fiNewFile.lpszPath = &gszNullPath[ 0 ];
      }
      else
      {
         if ( ( fiNewFile.lpszPath = ( LPTSTR ) Mem_SubMalloc( FsvStr_GetByteCount( lpszNewPath ) + 1 ) ) == NULL )
         // Bad news, unable to allocate the new path.
         {
            return( ZIP_NO_MEMORY );
         }
         else
         // Store copy of the new path.
         {
            lstrcpy( fiNewFile.lpszPath, lpszNewPath );
         }
      }
   }

   if ( lpszNewName != NULL )
   // Store the new name for the compressed file.
   {
      lstrcpy( fiNewFile.szName, lpszNewName );
   }

   // Try to "compress" the file.
   eErr = Zip_CompressFile( lpH, &fiNewFile, lpszSrcFile, /*FsvBufIO_fRead, FsvBufIO_fSeek,*/
                            lpszKey, NULL, 0, FALSE );

   if ( eErr == ZIP_NO_ERROR )
   // The "replace" was successful so update the "file info" record.
   {
      if ( lpFile->lpszPath != NULL && lpFile->lpszPath != fiNewFile.lpszPath )
      // Free the old path that has been replaced.
      {
         Mem_SubFree( lpFile->lpszPath );
      }

      // Use the new file info record.
      *lpFile = fiNewFile;
   }
   else
   if ( lpszNewName != NULL && fiNewFile.lpszPath != NULL )
   // Free the new path that had been allocated.
   {
      Mem_SubFree( fiNewFile.lpszPath );
   }

   return( eErr );
} // ZipReplaceFile

#endif          // NOT NAV95



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

#ifndef NAV95
ZIP_ERRCODE WINAPI ZipDeleteFile(
   HZIP   hArchvFile,
   LPCTSTR lpszName,
   WORD   wIndex
)

{
   LPZIP_HANDLE_INFO lpH;
   LPZIP_ARCHIVE_DIR lpDir;
   ZIP_ERRCODE       eErr;
   LPZIP_COMP_INFO   lpFile;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_WRITE ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle or access conflict. */
   {
      return( eErr );
   }

   /* Make local copy. */
   lpDir = lpH->lpDir;

   if ( lpszName != NULL )
   /* Try to find the specified file in the "internal directory". */
   {
      wIndex = Zip_IsFileInArchive( lpDir, lpszName );
   }

   if ( wIndex >= lpDir->wFileCount )
   /* Bad news.  The specified index is invalid. */
   {
      eErr = ZIP_BAD_ARG;
   }

   /* Account for the space that was used by the compressed file. */
   lpFile                   = lpDir->lplpCompFiles[ wIndex ];
   lpDir->SI.lFreedByts    += Zip_CalcOldComprssdFileByts( lpFile ) +
                              Zip_CalcEndInfoByts( lpDir, lpFile );
// lpDir->SI.lExtraRemByts -= ( lpFile->dbExtra1.lSize + lpFile->dbRemark.lSize );
   lpDir->SI.lExtraRemByts -= lpFile->dbExtra1.lSize;
   lpDir->SI.bDirty         = TRUE;

   /* Free the allocated compressed info record and remove it's */
   /* pointer from the pointer list.                            */
   Zip_DelCompFileRec( lpDir, wIndex );

   return( ZIP_NO_ERROR );
} /* ZipDeleteFile */
#endif          // NOT NAV95


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
   LPCTSTR lpszName,
   WORD   wIndex,
   LPCTSTR lpszDestPath,
   LPCTSTR lpszDestName,
   BOOL   bUseStoredPath,
   LPCTSTR lpszKey
)

{
   LPZIP_HANDLE_INFO    lpH;
   LPZIP_ARCHIVE_DIR    lpDir;
   LPZIP_COMP_INFO      lpFile;
   TCHAR                szFullName[ FSV_PATHBUF_LEN ];
   LPFSVBUFIO_FBUF_INFO lpOutpt;
   ZIP_ERRCODE          eErr;
   ZIP_ERRCODE          eTmpErr;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_READ ) ) != ZIP_NO_ERROR )
   // Bad news.  Invalid handle or access conflict.
   {
      return( eErr );
   }

   // Make local copy.
   lpDir = lpH->lpDir;

   if ( lpszName != NULL )
   // Try to find the specified file in the "internal directory".
   {
      wIndex = Zip_IsFileInArchive( lpDir, lpszName );
   }

   if ( wIndex >= lpDir->wFileCount )
   // Bad news.  The specified index is invalid.
   {
      return( ZIP_BAD_ARG );
   }

   // Make local pointer to compressed file info.
   lpFile = lpDir->lplpCompFiles[ wIndex ];

   // If specified, use the "destination" path.
   if ( lpszDestPath == NULL )
   {
      szFullName[ 0 ] = '\0';
   }
   else
   {
      lstrcpy( szFullName, lpszDestPath );
      FsvUtil_AddBackSlashChk( szFullName );
   }

   if ( bUseStoredPath )
   // Use the path stored with the compressed file.
   {
      LPTSTR  lpszWork;

      if ( lpFile->lpszPath[ 0 ] == '\\' && szFullName[ 0 ] != '\0' &&
           (lpszWork = CharNext(szFullName)) && *lpszWork == ':' )
      // The path stored with the compressed file is a "full" path
      // so only use the drive specified in the "destination path".
      {
         lpszWork = CharNext(lpszWork);
         *lpszWork = 0;
      }

      lstrcat( szFullName, lpFile->lpszPath );
      FsvUtil_AddBackSlashChk( szFullName );
   }

   if ( lpszDestName != NULL )
   // Add the file name supplied by the caller.
   {
      lstrcat( szFullName, lpszDestName );
   }
   else
   // Add the file name stored in the compressed file.
   {
      lstrcat( szFullName, lpFile->szName );
   }

   if ( lpFile->wAttrs & FSV_ATTRIB_DIRECTORY )
   // Create the specified directory entry.
   {
      eErr = FsvUtil_CreateMultipleDirs( szFullName, lpH->lpCritCBFunc,
                                         lpH->dwCritCBData );
   }
   else
   if ( ( eErr = FsvBufIO_fCreat( szFullName, FSV_ATTRIB_NONE, FSVBUFIO_FILE_BUF_SIZ, &lpOutpt,
                                  lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == ZIP_NO_ERROR )
   // The output file was created.
   {
      ZIP_FILTER_INFO fvFltrVars;

      // Init feedback control info for the one pass expansion algorithms.
      lpH->wPercntOff = 0;
      lpH->lPercntMax = 100;

      if ( lpFile->dbComprssdFile.lSize > 0 )
      // Initialize the generic "filter" parameter structure.
      {
         if ( ( eErr = Zip_InitExpand( lpH, lpFile, FsvBufIO_fWrite, ZIP_MAXLONG,
                                       lpszKey, 0, TRUE, NULL,
                                       /*lpFile->dbComprssdFile.lStart*/0, ZIP_BOUNDARY_BYTS,
                                       ZIP_EXTRACT_FILE, &fvFltrVars ) ) == ZIP_NO_ERROR )
         {
            // Call the function that will expand the compressed data
            // out to the specified file.
            eErr = Zip_FilterFile( lpH, &Zip_GetFBufInfo( lpDir, lpFile->dbComprssdFile.hbFleIndx ),
                                   &lpOutpt, &fvFltrVars );

            // If allocated, free the state information structure.
            Zip_DeInitCompressExpandChk( &fvFltrVars.lpVars, lpFile->ePckTyp, FALSE );
         }
      }

      if ( eErr == ZIP_NO_ERROR )
      // Validate the CRC generated.
      {
         if ( fvFltrVars.lCRC != lpFile->lCRC && lpFile->dbComprssdFile.lSize != 0 )
         {
            eErr = ZIP_CRC_MISMATCH;
         }
         else
         // Write out buffered data in the output file.  This is done so
         // DOS won't reset the date/time when calling FsvBufIO_fClose() and
         // flushing the buffer.
         if ( ( eErr = FsvBufIO_fFlush( lpOutpt, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == ZIP_NO_ERROR )
         {
            // Set the time and date of the expanded file to be the same
            // as when it was initially compressed.
            eTmpErr = MAKELONG( *( ( LPWORD ) &lpFile->tiTime ), *( ( LPWORD ) &lpFile->diDate ) );
            DOSWrap_5701_SetDateAndTime( lpOutpt->hFile, eTmpErr );
         }
      }

      // Close output file (buffered data has already been flushed).
      eTmpErr = FsvBufIO_fClose( lpOutpt, FALSE, lpH->lpCritCBFunc, lpH->dwCritCBData );

      if ( lpFile->wAttrs != 0 && eErr == ZIP_NO_ERROR && eTmpErr == ZIP_NO_ERROR )
      // Restore special file attributes.
      {
         eErr = FsvIO_SetFileAttributes( szFullName, lpFile->wAttrs,
                                         lpH->lpCritCBFunc, lpH->dwCritCBData );
      }

      if ( eErr != ZIP_NO_ERROR || eTmpErr != ZIP_NO_ERROR )
      // The "expansion" was unsuccessful so delete the destination file.
      {
         FsvIO_DeleteFile( szFullName, FALSE, lpH->lpCritCBFunc, lpH->dwCritCBData );
         eErr = ( eErr != ZIP_NO_ERROR ) ? eErr : eTmpErr;
      }
   }

   return( eErr );
} // ZipExtractFile



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
   LPCTSTR   lpszName,
   WORD     wIndex,
   ZIP_INFO eInfoType,
   LPVOID   lpBuf,
   WORD     wBufSize,
   LPWORD   lpwInfoByts
)

{
   LPZIP_HANDLE_INFO lpH;
   LPZIP_ARCHIVE_DIR lpDir;
   LPZIP_COMP_INFO   lpFile;
   LPVOID            lpInfo;
   ZIP_ERRCODE       eErr;
   WORD              wInfoSiz    = 0;
   BOOL              fStrngData  = FALSE;
   BOOL              fDataCopied = FALSE;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_READ ) ) != ZIP_NO_ERROR )
   // Bad news.  Invalid handle or access conflict.
   {
      return( eErr );
   }

   if ( lpBuf == NULL )
   // Bogus input paramter.
   {
      return( ZIP_BAD_ARG );
   }

   // Make local copy.
   lpDir = lpH->lpDir;

   if ( lpszName != NULL )
   // Try to find the specified file in the "internal directory".
   {
      wIndex = Zip_IsFileInArchive( lpDir, lpszName );
   }

   if ( wIndex >= lpDir->wFileCount )
   // Bad news.  The specified index is invalid.
   {
      return( ZIP_BAD_ARG );
   }

   // Get pointer to compressed file info.
   lpFile = lpDir->lplpCompFiles[ wIndex ];

   // Handle the various information queries.
   switch ( eInfoType )
   {
      case ZIP_FILEINDEX:
         wInfoSiz = sizeof( wIndex );
         lpInfo   = &wIndex;
      break;

      case ZIP_FILENAME:
         wInfoSiz   = FsvStr_GetByteCount( lpFile->szName ) + 1;
         lpInfo     = lpFile->szName;
         fStrngData = TRUE;
      break;

      case ZIP_PATHNAME:
         wInfoSiz   = FsvStr_GetByteCount( lpFile->lpszPath ) + 1;
         lpInfo     = lpFile->lpszPath;
         fStrngData = TRUE;
      break;

      case ZIP_DATE:
         wInfoSiz = sizeof( lpFile->tiTime ) + sizeof( lpFile->diDate );

         if (lpBuf != NULL)
         {
            _fmemcpy( lpBuf, &lpFile->tiTime, min( wBufSize, sizeof( lpFile->tiTime ) ) );

            if ( wBufSize > sizeof( lpFile->tiTime ) )
            {
               _fmemcpy( ( LPBYTE ) lpBuf + sizeof( lpFile->tiTime ), &lpFile->diDate,
                         min( wBufSize - sizeof( lpFile->tiTime), sizeof( lpFile->diDate ) ) );
            }

            // The data has been transferred.
            fDataCopied = TRUE;
         }
      break;

      case ZIP_ORIGINALSIZE:
         wInfoSiz = sizeof( lpFile->lOrigSiz );
         lpInfo   = &lpFile->lOrigSiz;
      break;

      case ZIP_COMPRESSEDSIZE:
         wInfoSiz = sizeof( lpFile->dbComprssdFile.lSize );
         lpInfo   = &lpFile->dbComprssdFile.lSize;
      break;

      case ZIP_COMPRESSTYPE:
         wInfoSiz = sizeof( lpFile->ePckTyp );
         lpInfo   = &lpFile->ePckTyp;
      break;

      case ZIP_ENCRYPTSTATE:
         wInfoSiz = sizeof( lpFile->bEncrypted );
         lpInfo   = &lpFile->bEncrypted;
      break;

      case ZIP_HEADERDISKNUM:
         wInfoSiz = sizeof( lpFile->wDiskNum );
         lpInfo   = &lpFile->wDiskNum;
      break;

      case ZIP_DATADISKNUM:
         wInfoSiz = sizeof( lpFile->dbComprssdFile.wDiskNum );
         lpInfo   = &lpFile->dbComprssdFile.wDiskNum;
      break;

      case ZIP_COMPDATAOFFSET:
         wInfoSiz = sizeof( lpFile->dbComprssdFile.lStart );
         lpInfo   = &lpFile->dbComprssdFile.lStart;
      break;

      case ZIP_COMMENT:
//       wInfoSiz = ( WORD ) lpFile->dbRemark.lSize + 1;
         wInfoSiz = Zip_GetRemarkLen( lpFile->lpszRemark ) + 1;

         if ( wBufSize > 0 )
         {
            WORD wNumByts = min( wBufSize - 1, wInfoSiz );

//          if ( wNumByts == 0 &&
//               ( eErr = FsvBufIO_fSeek( Zip_GetFBufInfo( lpDir, lpFile->dbRemark.hbFleIndx ),
//                                        lpFile->dbRemark.lStart, SEEK_SET, lpH->lpCritCBFunc,
//                                        lpH->dwCritCBData  ) ) == lpFile->dbRemark.lStart )
//          {
//             eErr = Zip_fReadByts( lpH, Zip_GetFBufInfo( lpDir, lpFile->dbRemark.hbFleIndx),
//                                   lpBuf, wNumByts );
//          }

            if ( wNumByts != 0 && lpFile->lpszRemark != NULL )
            // Copy as much of the remark string as possible.
            {
               _fmemcpy( lpBuf, lpFile->lpszRemark, wNumByts );
            }

            // Null terminate the "remark".
            ( ( LPBYTE ) lpBuf )[ wNumByts ] = '\0';
         }

         // The data has been transferred.
         fDataCopied = TRUE;
      break;

      case ZIP_EXTRAINFO:
      {
         // If necessary, get the size of the extra data for the
         // compressed file.
         if ( ( eErr = Zip_FileVrfyChk( lpH, lpFile ) ) != ZIP_NO_ERROR )
         {
            return( eErr );
         }

         wInfoSiz = ( WORD ) lpFile->dbExtra1.lSize;

         if ( wBufSize > 0 && wInfoSiz > 0 )
         {
            if ( ( eErr = FsvBufIO_fSeek( Zip_GetFBufInfo( lpDir, lpFile->dbExtra1.hbFleIndx ), lpFile->dbExtra1.lStart,
                                          SEEK_SET, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == lpFile->dbExtra1.lStart )
            {
               eErr = Zip_fReadByts( lpH, Zip_GetFBufInfo( lpDir, lpFile->dbExtra1.hbFleIndx ),
                                     lpBuf, (WORD)min( wBufSize, wInfoSiz ) );
            }
         }

         // The data has been transferred.
         fDataCopied = TRUE;
      }
      break;

      case ZIP_ALLINFO:
      {
         WORD wRecByts = Zip_GetFileInfoByts( lpFile, ( LPZIP_FILE_INFO ) lpBuf );

         if ( wRecByts > wBufSize )
         // Buffer not big enough.
         {
            eErr = ZIP_BAD_ARG;
         }
         else
         // Copy data from private structure to public structure.
         {
            Zip_CopyPrivateCompInto2PublicStruct( lpFile, ( LPZIP_FILE_INFO ) lpBuf );
            fDataCopied = TRUE;
         }
      }
      break;

      default:
         eErr = ZIP_BAD_ARG;
      break;
   }

   if ( lpwInfoByts != NULL )
   {
      *lpwInfoByts = wInfoSiz;
   }

   if ( !fDataCopied )
   {
      if ( wBufSize > wInfoSiz )
      {
         eErr = ZIP_BAD_ARG;
      }
      else
      {
         _fmemcpy( lpBuf, lpInfo, wInfoSiz );
      }
   }

   return( eErr );
} /* ZipGetFileInfo */



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

#ifndef NAV95
ZIP_ERRCODE WINAPI ZipSetFileInfo(
   HZIP     hArchvFile,
   LPCTSTR   lpszName,
   WORD     wIndex,
   ZIP_INFO eInfoType,
   LPVOID   lpBuf,
   WORD     wBufSize
)

{
   LPZIP_HANDLE_INFO lpH;
   ZIP_ERRCODE       eErr;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_READ ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle or access conflict. */
   {
      return( eErr );
   }

   if ( lpszName != NULL )
   /* Try to find the specified file in the "internal directory". */
   {
      wIndex = Zip_IsFileInArchive( lpH->lpDir, lpszName );
   }

   if ( wIndex >= lpH->lpDir->wFileCount || lpBuf == NULL )
   // Bad news, bogus input parameters.
   {
      return( ZIP_BAD_ARG );
   }

   /* Try to set the appropriate info. */
   eErr = Zip_SetFileInfo( lpH, lpH->lpDir->lplpCompFiles[ wIndex ],
                           eInfoType, lpBuf, wBufSize );

   return( eErr );
} /* ZipSetFileInfo */
#endif  // NOT NAV95


// *************************NOT IMPLEMENTED***************************
// Encrypt a file with a new key.
#ifdef IfImplemented
ZIP_ERRCODE WINAPI ZipReEncryptFile(
   HZIP   hArchvFile,
   LPCTSTR lpszName,
   WORD   wIndex,
   LPCTSTR lpszOldKey,
   LPCTSTR lpszNewKey
)

{
   return( ZIP_BAD_ARG );
} /* ZipReEncryptFile */
#endif
// *************************NOT IMPLEMENTED***************************



// *************************NOT IMPLEMENTED***************************
// Re-compress an existing file in a new format.
#ifdef IfImplemented
ZIP_ERRCODE WINAPI ZipRecompressFile(
   HZIP          hArchvFile,
   LPCTSTR        lpszName,
   WORD          wIndex,
   ZIP_PACK_TYPE ptPackType
)

{
   return( ZIP_BAD_ARG );
} /* ZipRecompressFile */
#endif
// *************************NOT IMPLEMENTED***************************



// *************************NOT IMPLEMENTED***************************
// Compress a file contained in a memory block.
#ifdef IfImplemented
ZIP_ERRCODE WINAPI ZipAddFileFromMemory(
   HZIP          hArchvFile,
   LPHVOID       lphBuf,
   ULONG         ulBufSize,
   LPCTSTR        lpszName,
   WORD          wIndex,
   ZIP_PACK_TYPE ePackType,
   LPCTSTR        lpszKey,
   LPVOID        lpExtraData,
   WORD          wExtraSize
)

{
   return( ZIP_BAD_ARG );
} /* ZipAddFileFromMemory */
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
   HZIP   hArchvFile,
   LPCTSTR lpszKey,
   LPCTSTR lpszName,
   WORD   wIndex,
   ULONG  ulStartPostn,
   LPVOID lpBuf,
   WORD   wBufSize,
   LPWORD lpwBytesRead
)

{
   LPZIP_HANDLE_INFO    lpH;
   LPZIP_ARCHIVE_DIR    lpDir;
   LPZIP_COMP_INFO      lpCompFile;
   ZIP_ERRCODE          eErr;
   ZIP_MBUF_INFO        miMemBuf;
   ZIP_FILTER_INFO      fvFltrVars;
   WORD                 wBytesRead;
   LPFSVBUFIO_FBUF_INFO lpInpt;
   WORD                 wBndryByts;
   LPZIP_MBUF_INFO      lpMemBuf = &miMemBuf;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_READ ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle or access conflict. */
   {
      return( eErr );
   }

   /* Make local copy. */
   lpDir = lpH->lpDir;

   if ( lpszName != NULL )
   /* Try to find the specified file in the "internal directory". */
   {
      wIndex = Zip_IsFileInArchive( lpDir, lpszName );
   }

   if ( lpBuf == NULL || wBufSize == 0 || wIndex >= lpDir->wFileCount ||
        ( lpCompFile = lpDir->lplpCompFiles[ wIndex ] ) == NULL ||
        ulStartPostn > ( ULONG ) lpCompFile->lOrigSiz )
   /* Bogus buffer pointer, buffer size, directory index, or */
   /* starting position.                                     */
   {
      return( ZIP_BAD_ARG );
   }

   /* Get pointer to file buffer info for the compressd file. */
   lpInpt = Zip_GetFBufInfo( lpDir, lpCompFile->dbComprssdFile.hbFleIndx );

   if ( TRUE /*****FIX*****/ /*
        lpDir->RFI.hArchvFile != hArchvFile ||
        lpDir->RFI.lLastByt    > ( LONG ) ulStartPostn ||
        lpDir->RFI.wIndex     != wIndex ||
        lpDir->RFI.lpvVars    == NULL */ )
   /* A sequential expansion was NOT detected so start expanding */
   /* from the beginning of the compressed data.                 */
   {
      // If allocated, free the de-compression state info.
      Zip_DeInitCompressExpandChk( &lpDir->RFI.lpvVars, lpCompFile->ePckTyp, FALSE );

      lpDir->RFI.lComprsdStrt = lpCompFile->dbComprssdFile.lStart;
      lpDir->RFI.lLastByt     = 0;
   }
   /*****NUKE*****/
   #ifdef MYDEBUG
   else
   {
      int i=10;
   }
   #endif
   /*****NUKE*****/

   /* Initialize the structure used to contain the expanded output. */
   miMemBuf.lpBuf    = lpBuf;
   miMemBuf.wBufSiz  = wBufSize;
   miMemBuf.lVirtOff = lpDir->RFI.lLastByt;
   miMemBuf.lStart   = ulStartPostn;

   /* For some bizarre reason the de-compress functions require extra */
   /* buffer bytes when the end of the input stream is detected.      */
   /*****FIX*****/
   wBndryByts = ( miMemBuf.lStart + ( LONG ) wBufSize < lpCompFile->lOrigSiz ) ? 0 : ZIP_BOUNDARY_BYTS;
   /*****FIX*****/
   wBndryByts = ZIP_BOUNDARY_BYTS;

   /* Init info required to de-compress the specified block of data. */
   if ( ( eErr = Zip_InitExpand( lpH, lpCompFile, ( LPZIP_FILEIO_FUNC ) Zip_MemWrite,
                                 ( ulStartPostn + wBufSize ) - lpDir->RFI.lLastByt,
                                 NULL, 0, FALSE, lpDir->RFI.lpvVars, lpDir->RFI.lComprsdStrt/*****FIX*****/,
                                 wBndryByts, ZIP_READ_FILE, &fvFltrVars ) ) == ZIP_NO_ERROR )
   {
      /* Init feedback control info for the one pass expansion algorithms. */
      lpH->wPercntOff = 0;
      lpH->lPercntMax = 100;

      /* Expand the compressed data out to a buffer or a file. */
      eErr = Zip_FilterFile( lpH, &lpInpt, ( LPLPFSVBUFIO_FBUF_INFO ) &lpMemBuf,
                             &fvFltrVars );
   }

   /* Get the number of bytes read. */
   if ( miMemBuf.lVirtOff > miMemBuf.lStart )
   {
      wBytesRead = ( WORD ) ( miMemBuf.lVirtOff - miMemBuf.lStart );
   }
   else
   {
      wBytesRead = 0;
   }

   /*****IMPLEMENT*****/
   #ifdef HiMom
   if ( eErr == ZIP_NO_ERROR )
   /* Save state info regarding the last block of data read. */
   {
      lpDir->RFI.hArchvFile   = hArchvFile;
      lpDir->RFI.wIndex       = wIndex;
      lpDir->RFI.lLastByt     = ulStartPostn + wBytesRead;
      lpDir->RFI.lpvVars      = fvFltrVars.lpVars;
      lpDir->RFI.ePckTyp      = lpCompFile->ePckTyp;
   }
   else
   /* Store a bogus handle value to force a "restart". */
   {
      // If allocated, free the de-compression state info.
      Zip_DeInitCompressExpandChk( &lpDir->RFI.lpvVars, lpCompFile->ePckTyp, FALSE );

      lpDir->RFI.hArchvFile = ZIP_MAX_HANDLES + 1;
   }
   #endif
   /*****IMPLEMENT*****/

   /******NUKE*****/
   // If allocated, free the de-compression state info.
   Zip_DeInitCompressExpandChk( &fvFltrVars.lpVars, lpCompFile->ePckTyp, FALSE );
   /******NUKE*****/

   if ( lpwBytesRead != NULL )
   /* Return the number of bytes read. */
   {
      *lpwBytesRead = wBytesRead;
   }

   return( eErr );
} /* ZipReadFile */



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
   HZIP   hArchvFile,
   WORD   wStartIndex,
   WORD   wMaxCount,
   LPBYTE lpBuf,
   WORD   wBufSize,
   LPWORD lpwNumEntries
)

{
   WORD              wByts = 0;
   WORD              wDirs = 0;
   WORD              wIndx = wStartIndex;
   LPZIP_HANDLE_INFO lpH;
   LPZIP_ARCHIVE_DIR lpDir;
   LPLPZIP_COMP_INFO lplpCompFile;
   ZIP_ERRCODE       eErr;

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_READ ) ) != ZIP_NO_ERROR )
   // Bad news.  Invalid handle or access conflict.
   {
      return( eErr );
   }

   // Make local copy.
   lpDir = lpH->lpDir;

   if ( lpBuf == NULL || wStartIndex >= lpDir->wFileCount )
   // Bogus buffer pointer or directory index.
   {
      return( ZIP_BAD_ARG );
   }

   // Point to the first directory entry to return.
   lplpCompFile = &lpDir->lplpCompFiles[ wStartIndex ];

   // Copy as many directory entries as possible.
   for ( ;
         wIndx < lpDir->wFileCount && wMaxCount > 0;
         wIndx++, lplpCompFile++, wMaxCount-- )
   {
      LPZIP_COMP_INFO lpCompFile = lplpCompFile[ 0 ];
//      WORD            wPathLen   = lstrlen( lpCompFile->lpszPath ) + 1;
      WORD            wDirByts   = Zip_GetFileInfoByts( lpCompFile, ( LPZIP_FILE_INFO ) lpBuf );

      // Accummulate the number of bytes for the directory info.
      wByts += wDirByts;

      if ( wByts > wBufSize )
      // The buffer has been filled.
      {
         break;
      }

      // Copy the local data structure into public data directory.
      Zip_CopyPrivateCompInto2PublicStruct( lpCompFile, ( LPZIP_FILE_INFO ) lpBuf );

      // Advance the buffer pointer.
      lpBuf += wDirByts;
   }

   if ( lpwNumEntries != NULL )
   // Return the number of directory info records copied.
   {
      *lpwNumEntries = wIndx - wStartIndex;
   }

   return( ZIP_NO_ERROR );
} // ZipGetAllFileInfo



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

#ifndef NAV95
ZIP_ERRCODE WINAPI ZipXferFile(
   HZIP   hArchvSrc,
   LPCTSTR lpszSrcName,
   WORD   wSrcIndx,
   HZIP   hArchvDst,
   WORD   wDstIndx,
   LPCTSTR lpszKey
)

{
   LPZIP_HANDLE_INFO lpSrcH;
   LPZIP_HANDLE_INFO lpDstH;
   LPZIP_ARCHIVE_DIR lpSrcDir;
   LPZIP_ARCHIVE_DIR lpDstDir;
   LPZIP_COMP_INFO   lpSrcInfo;
   ZIP_COMP_INFO     fiDstInfo;
   ZIP_ERRCODE       eErr;
   BOOL              bAddDstFile;
   BOOL              bCopyCompFile;
   BOOL              bCopyExtra;
   BOOL              bCopyRmrk;
   WORD              wBufSiz;
   WORD              wRmrkLen;
   LONG              lOrigEOF;
   ZIP_DIR_STATE     siOrigState;
   LPVOID            lpBuf = NULL;

   if ( ( eErr = Zip_GetDirInfo( hArchvSrc, &lpSrcH, ZIP_DIR_READ  ) ) != ZIP_NO_ERROR ||
        ( eErr = Zip_GetDirInfo( hArchvDst, &lpDstH, ZIP_DIR_WRITE ) ) != ZIP_NO_ERROR )
   /* Bad news.  Invalid handle or access conflict. */
   {
      return( eErr );
   }

   /* Make local copies. */
   lpSrcDir = lpSrcH->lpDir;
   lpDstDir = lpDstH->lpDir;

   if ( hArchvSrc == hArchvDst || lpSrcDir == lpDstDir )
   /* Bad news.  Source and destination files are the same. */
   {
      return( ZIP_BAD_ARG );
   }

   if ( lpSrcDir->SI.fIsSplit || lpDstDir->SI.fIsSplit )
   // The user cannot "xfer" from a multi-disk archive file or to a multi-disk
   // archive file.
   {
      return( ZIP_CANT_DO_FOR_MDZF );
   }

   if ( lpszSrcName != NULL )
   /* Search for the source file name. */
   {
      wSrcIndx = Zip_IsFileInArchive( lpSrcDir, lpszSrcName );
   }

   if ( wSrcIndx >= lpSrcDir->wFileCount )
   /* The specified file name doesn't exist or the specified */
   /* index is invalid.                                      */
   {
      return( ZIP_BAD_ARG );
   }

   if ( lpszSrcName != NULL )
   /* Search for the destination file name. */
   {
      wDstIndx = Zip_IsFileInArchive( lpDstDir, lpszSrcName );
   }

   /* Get pointer to info for the source compressed file. */
   lpSrcInfo = lpSrcDir->lplpCompFiles[ wSrcIndx ];

   /* If necessary, get the size of the extra data for the source */
   /* compressed file.                                            */
   if ( ( eErr = Zip_FileVrfyChk( lpSrcH, lpSrcInfo ) ) != ZIP_NO_ERROR )
   {
      return( eErr );
   }

   /* If necessary, get the size of the extra data for all of the */
   /* compressed files contained in the destination archive file. */
   if ( ( eErr = Zip_ArchvVrfyChk( lpDstH ) ) != ZIP_NO_ERROR )
   {
      return( eErr );
   }

   /* Save archive state info in case an error is detected. */
   lOrigEOF    = FsvBufIO_fRtrnEOFPos( lpDstDir->fiArchvFile.lpFBuf );
   siOrigState = lpDstDir->SI;

   /* Can a simple copy be performed? */
   bCopyCompFile = Zip_IsCompMethComptbl( lpSrcDir->eArchvTyp, lpDstDir->eArchvTyp,
                                          lpSrcInfo->ePckTyp );

   /* Does the extra data and/or remark data have to be manually copied. */
   bCopyExtra = ( lpSrcInfo->dbExtra1.lSize > 0 && lpDstDir->eArchvTyp != ZIP_FORMAT_LHA &&
                  ( !bCopyCompFile || lpSrcDir->eArchvTyp != ZIP_FORMAT_ZIP || lpDstDir->eArchvTyp != ZIP_FORMAT_ZIP ) );
// bCopyRmrk  = ( lpSrcInfo->dbRemark.lSize > 0 && lpDstDir->eArchvTyp != ZIP_FORMAT_LHA );
   wRmrkLen   = Zip_GetRemarkLen( lpSrcInfo->lpszRemark );
   bCopyRmrk  = ( wRmrkLen > 0 && lpDstDir->eArchvTyp != ZIP_FORMAT_LHA );

   // Compute size of buffer needed to contain the extra and/or remark data.
   wBufSiz = ( bCopyRmrk  ) ? wRmrkLen + 1 : 0;
   wBufSiz = ( bCopyExtra ) ? max( ( WORD ) lpSrcInfo->dbExtra1.lSize, wBufSiz ) : wBufSiz;

   if ( wBufSiz > 0 && ( lpBuf = Mem_SubMalloc( wBufSiz ) ) == NULL )
   /* Bad news, can't allocate buffer for the extra/remark data. */
   {
      return( ZIP_NO_MEMORY );
   }

   if ( wDstIndx < lpDstDir->wFileCount )
   /* The file already exists in the destination archive. */
   {
      bAddDstFile = FALSE;
      fiDstInfo   = *lpDstDir->lplpCompFiles[ wDstIndx ];
   }
   else
   /* Add the file to the destination archive. */
   {
      LPZIP_COMP_INFO lpFile;

      bAddDstFile = TRUE;
      wDstIndx    = lpDstDir->wFileCount;
      Zip_InitCompFileRec( &fiDstInfo, TRUE, TRUE, lpSrcInfo->szName );

      if ( lstrlen(lpSrcInfo->lpszPath) > 0 )
      {
         if ( ( fiDstInfo.lpszPath = ( LPTSTR ) Mem_SubMalloc( FsvStr_GetByteCount( lpSrcInfo->lpszPath ) + 1 ) ) == NULL )
         /* Bad news, unable to allocate the path. */
         {
            return( ZIP_NO_MEMORY );
         }
         else
         {
            lstrcpy( fiDstInfo.lpszPath, lpSrcInfo->lpszPath );
         }
      }

      if ( ( lpFile = ( LPZIP_COMP_INFO ) Mem_SubMalloc( sizeof( ZIP_COMP_INFO ) ) ) == NULL )
      /* Bad news.  Not enough memory for the compressed file info record. */
      {
         Zip_FreePathChk( fiDstInfo.lpszPath );
         return( ZIP_NO_MEMORY );
      }

      if ( ( eErr =  Zip_InsCompFileRec( lpDstDir, ( WORD ) -1, lpFile ) ) != ZIP_NO_ERROR )
      /* Free dynamically allocated path and compressed info record. */
      {
         Zip_FreePathChk( fiDstInfo.lpszPath );
         Mem_SubFree( lpFile );
         return( eErr );
      }

      /* Update pointer to path in case an error is detected. */
      lpFile->lpszPath = fiDstInfo.lpszPath;
   }

   // The destination file must use the following info from the
   // source file.
   fiDstInfo.lOrigSiz     = lpSrcInfo->lOrigSiz;
   fiDstInfo.ePckTyp      = lpSrcInfo->ePckTyp;
   fiDstInfo.wPckFlgs     = lpSrcInfo->wPckFlgs;
   fiDstInfo.bEncrypted   = lpSrcInfo->bEncrypted;
   fiDstInfo.diDate       = lpSrcInfo->diDate;
   fiDstInfo.tiTime       = lpSrcInfo->tiTime;
   fiDstInfo.wAttrs       = lpSrcInfo->wAttrs;
   fiDstInfo.lCRC         = lpSrcInfo->lCRC;
   fiDstInfo.wSpclRecByts = lpSrcInfo->wSpclRecByts;
   fiDstInfo.bLevel       = lpSrcInfo->bLevel;
   fiDstInfo.wUnknownByts = lpSrcInfo->wUnknownByts;
   _fmemcpy( fiDstInfo.abUnknwnByts, lpSrcInfo->abUnknwnByts, sizeof( fiDstInfo.abUnknwnByts ) );

   if ( bCopyCompFile )
   /* The source and destination archive files are of the same type */
   /* or they both support the compression method used by the       */
   /* compressed file, so simply copy the compressed file to the    */
   /* destination archive file.                                     */
   {
      eErr = Zip_XFerCompFile( lpSrcH, lpDstH, lpSrcInfo, &fiDstInfo,
                               bAddDstFile );
   }
   else
   /* Bummer, the compression method is imcompatible between archive */
   /* files so extract the compression file to a temporary file and  */
   /* then compress that temporary file into the destination archive */
   /* file.                                                          */
   {
      TCHAR chJunk = 0; // This value is ignored in GetTempDrive().
      TCHAR szTmpFullName[ FSV_PATHBUF_LEN ];
      TCHAR szTmpPath[ FSV_MAXDIRNAME_LEN + 1 ];
      TCHAR szTmpName[ FSV_SFILENAMEBUF_LEN ];

      if ( ( eErr = FsvUtil_GetTempFileName( 3, "TMP", 0, szTmpFullName, fiDstInfo.lOrigSiz ) ) == ZIP_NO_ERROR )
      {
         // Extract path and name.
         FsvUtil_GetPathMask( szTmpFullName, szTmpPath, szTmpName, TRUE );

         if ( ( eErr = ZipExtractFile( hArchvSrc, NULL, wSrcIndx, szTmpPath, szTmpName,
                                       FALSE, lpszKey ) ) == ZIP_NO_ERROR )
         /* Try to "compress" the temporary file into the destination archive. */
         {
            LPVOID lpExtra   = NULL;
            WORD   wExtraSiz = 0;

            /* Use best compression method for destination. */
            fiDstInfo.ePckTyp = ZIP_BEST;

            if ( bCopyExtra )
            /* Try to get the extra data with the source compressed file. */
            {
               lpExtra   = lpBuf;
               wExtraSiz = ( WORD ) lpSrcInfo->dbExtra1.lSize;
               eErr      = ZipGetFileInfo( hArchvSrc, NULL, wSrcIndx, ZIP_EXTRAINFO,
                                           lpExtra, wExtraSiz, NULL );
            }

            if ( eErr == ZIP_NO_ERROR )
            /* Try to compress source file info destination archive. */
            {
               eErr = Zip_CompressFile( lpDstH, &fiDstInfo, szTmpFullName, /*FsvBufIO_fRead,
                                        FsvBufIO_fSeek,*/ lpszKey, lpExtra, wExtraSiz,
                                        bAddDstFile );
            }

            /* Delete the temporary file. */
            FsvIO_DeleteFile( szTmpFullName, FALSE, NULL, 0 );
         }
      }
   }

   if ( eErr == ZIP_NO_ERROR && bCopyRmrk )
   // Transfer the remark from the source file to the destination
   // file.
   {
      if ( ( eErr = ZipGetFileInfo( hArchvSrc, NULL, wSrcIndx, ZIP_COMMENT,
                                    lpBuf, wBufSiz, NULL ) ) == ZIP_NO_ERROR )
      {
         eErr = Zip_SetFileInfo( lpDstH, &fiDstInfo, ZIP_COMMENT, lpBuf,
//                               ( WORD ) lpSrcInfo->dbRemark.lSize );
                                 Zip_GetRemarkLen( lpSrcInfo->lpszRemark ) );
      }
   }

   if ( eErr == ZIP_NO_ERROR )
   /* Operation successful, update compressed file info. */
   {
      *lpDstDir->lplpCompFiles[ wDstIndx ] = fiDstInfo;
   }
   else
   /* Do clean up since the operation was unsuccessful. */
   {
      if ( bAddDstFile )
      /* Free the allocated compressed info record and remove it's */
      /* pointer from the pointer list.                            */
      {
         Zip_DelCompFileRec( lpDstDir, (WORD)(lpDstDir->wFileCount - 1) );
      }

      /* Restore archive state info and logical end of file. */
      lpDstDir->SI = siOrigState;
      FsvBufIO_fSetEOF( lpDstDir->fiArchvFile.lpFBuf, lOrigEOF, lpDstH->lpCritCBFunc,
                        lpDstH->dwCritCBData );
   }

   if ( lpBuf != NULL )
   /* Free buffer used to manually copy the extra/remark data. */
   {
      Mem_SubFree( lpBuf );
   }

   return( eErr );
} /* ZipXferFile */
#endif  // NOT NAV95


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

#ifndef NAV95
ZIP_ERRCODE WINAPI ZipAddDirEntry(
   HZIP   hArchvFile,
   LPCTSTR lpszSrcDir,
   LPCTSTR lpszPathInfo,
   LPCTSTR lpszNewName
)

{
   LPZIP_HANDLE_INFO lpH;
   LPZIP_ARCHIVE_DIR lpDir;
   LPZIP_COMP_INFO   lpFile;
   TCHAR             szPath[ FSV_PATHBUF_LEN ] = { '\0' };
   LPTSTR             lpszPath    = szPath;
   ZIP_ERRCODE       eErr        = ZIP_NO_ERROR;

   if ( lpszSrcDir == NULL || lstrlen(lpszSrcDir) == 0 ||
        ( lpszPathInfo != NULL && lstrlen(lpszPathInfo) > 0 && !FsvUtil_ValidateFullFileName( lpszPathInfo, FALSE ) ) ||
        ( lpszNewName != NULL && !FsvUtil_ValidateFileName( lpszNewName, TRUE ) ) )
   // Bad news, the caller supplied parameters are invalid.
   {
      return( ZIP_BAD_ARG );
   }

   if ( ( eErr = Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_WRITE ) ) != ZIP_NO_ERROR )
   // Bad news.  Invalid handle or access conflict.
   {
      return( eErr );
   }

   // Make local copy.
   lpDir = lpH->lpDir;

   if ( Zip_TypSupportsDirEntries( lpDir->eArchvTyp ) )
   {
      LPCTSTR  lpszWork;
      FSVIO_DIRENTRYSTRUCT diDirInfo;

      // Get info about the directory entry.
      if ( ( eErr = FsvIO_GetDirEntry( lpszSrcDir, &diDirInfo, NULL, lpH->lpCritCBFunc,
                                       lpH->dwCritCBData ) ) != FSV_SUCCESS )
      {
         return( eErr );
      }

      if ( ( diDirInfo.dwFsvAttrib & FSV_ATTRIB_DIRECTORY ) == 0 )
      // Bad news, we aren't dealing with a directory.
      {
         return( ZIP_BAD_ARG );
      }

      if ( ( lpFile = ( LPZIP_COMP_INFO ) Mem_SubMalloc( sizeof( ZIP_COMP_INFO ) ) ) == NULL )
      // Bad news.  Not enough memory for the compressed file info record.
      {
         return( ZIP_NO_MEMORY );
      }

      // Initialize all info fields.  Mark the record as being
      // "dirty" and that it has been "verified".
      Zip_InitCompFileRec( lpFile, TRUE, TRUE, "" );

      if ( lpszPathInfo != NULL )
      // Make a local copy.
      {
         lstrcpy( szPath, lpszPathInfo );
      }

      if ( (lpszWork = CharNext(lpszPath)) && *lpszWork == ':' && lstrlen(lpszPath) > 0 )
      // Ignore the drive letter.
      {
         lpszPath = CharNext(lpszWork);
      }

      if ( lpszPath[ 0 ] == '\\' )
      // Ignore the leading backslash.
      {
         lpszPath = CharNext(lpszPath);
      }

      if ( lstrlen(lpszPath) > 0 )
      // Make sure the path has a trailing backslash.
      {
         FsvUtil_AddBackSlashChk( lpszPath );
      }

      // Store the "node" of the directory name with the "path info".
      if ( lpszNewName != NULL )
      // Store the new name for the directory.
      {
         lstrcat( lpszPath, lpszNewName );
      }
      else
      // Use the original directory name.
      {
         TCHAR szName[ FSV_LFILENAMEBUF_LEN ];

         FsvUtil_GetPathMask( lpszSrcDir, NULL, szName, FALSE );
         lstrcat( lpszPath, szName );
      }

      // Make sure the path has a trailing backslash.
      FsvUtil_AddBackSlashChk( lpszPath );

      if ( lstrlen(lpszPath) > 0 )
      {
         // Try to dynamically allocate the path.
         if ( ( lpFile->lpszPath = ( LPTSTR ) Mem_SubMalloc( FsvStr_GetByteCount( lpszPath ) + 1 ) ) == NULL )
         // Bad news, unable to allocate the path.
         {
            eErr = ZIP_NO_MEMORY;
         }
         else
         // Make the directory node separators compatible with ZIP.
         {
            lstrcpy( lpFile->lpszPath, lpszPath );
            for ( lpszPath = lpFile->lpszPath; lstrlen(lpszPath) > 0; lpszPath = CharNext(lpszPath) )
            {
               if ( *lpszPath == '\\' )
               {
                  *lpszPath = '/';
               }
            }
         }
      }

      // Try to add the compressed info record to the end of the "list".
      if ( eErr == ZIP_NO_ERROR &&
           ( eErr =  Zip_InsCompFileRec( lpDir, ( WORD ) -1, lpFile ) ) == ZIP_NO_ERROR )
      // Try to "compress" the file.
      {
         LPFSVBUFIO_FBUF_INFO lpOutpt  = lpDir->fiArchvFile.lpFBuf;
         LONG                 lOrigEOF = FsvBufIO_fRtrnEOFPos( lpOutpt );

         // Point to available space to store the "file header".
         if ( ( eErr = FsvBufIO_fSeek( lpOutpt, lpDir->SI.lAvailPostn, SEEK_SET, lpH->lpCritCBFunc,
                                       lpH->dwCritCBData ) ) == lpDir->SI.lAvailPostn &&
              ( eErr = Zip_WriteFileHeadr( lpH, &lpOutpt, lpFile, FALSE ) ) == ZIP_NO_ERROR )
         // Try to grow the archive file to acount for the space
         // required for all accumulated EOF bytes.  After growing
         // the reset file pointer to the logical end of file.
         {
            eErr = Zip_GrowthCheck( lpH, &lpDir->fiArchvFile.lpFBuf, 0, Zip_CalcEndInfoByts( lpDir, lpFile ), 0 );
         }

         if ( eErr != ZIP_NO_ERROR )
         // Free the allocated compressed info record and remove it's
         // pointer from the pointer list.
         {
            Zip_DelCompFileRec( lpDir, (WORD)(lpDir->wFileCount - 1) );
         }
         else
         // Set appropriate fields pertaining to the directory info.
         {
            lpFile->ePckTyp                 = ZIP_NONE;
            lpFile->wAttrs                  = FSV_ATTRIB_DIRECTORY;
            *( ( LPWORD ) &lpFile->diDate ) = diDirInfo.wDate;
            *( ( LPWORD ) &lpFile->tiTime ) = diDirInfo.wTime;
         }
      }

      if ( eErr != ZIP_NO_ERROR )
      // Free dynamically allocated path and compressed info record.
      {
         Zip_FreeComprssdRec( lpFile );
      }
   }

   return( eErr );
}  // ZipAddDirEntry
#endif // NOT NAV95


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
)

{
   LPZIP_HANDLE_INFO lpH;
   BOOL              fRtn = FALSE;

   return( ( Zip_GetDirInfo( hArchvFile, &lpH, ZIP_DIR_READ ) == ZIP_NO_ERROR &&
             lpH->lpDir->SI.fIsSplit ) );
} // ZipIsMultiDiskZipFile



/*---------------------------------------------------------*/
/*   L O C A L   F U N C T I O N S                         */
/*---------------------------------------------------------*/


//==========================================================================
//
// VOID Zip_CopyPrivateCompInto2PublicStruct(
//
//    lpFileInfo   - Pointer to the compressed file info structure.
//    lpPublicInfo - Pointer to the public compressed file info structure.
//
// This function copies the compressed file info that is stored in private
// data structure to a structure that is public.
//
//==========================================================================

VOID WINAPI Zip_CopyPrivateCompInto2PublicStruct(
   LPCZIP_COMP_INFO lpFile,
   LPZIP_FILE_INFO  lpPublicInfo
)

{
   WORD wRecByts = Zip_GetFileInfoByts( lpFile, lpPublicInfo );

   // Copy the local data structure into public data directory.
   lpPublicInfo->wRecSiz         = wRecByts;
   lpPublicInfo->lOrigSiz        = lpFile->lOrigSiz;
   lpPublicInfo->ePckTyp         = lpFile->ePckTyp;
   lpPublicInfo->wPckFlgs        = lpFile->wPckFlgs;
   lpPublicInfo->bEncrypted      = lpFile->bEncrypted;
   lpPublicInfo->diDate          = lpFile->diDate;
   lpPublicInfo->tiTime          = lpFile->tiTime;
   lpPublicInfo->lCRC            = lpFile->lCRC;
   lpPublicInfo->lComprsdSiz     = lpFile->dbComprssdFile.lSize;
   lpPublicInfo->wFileAttrs      = lpFile->wAttrs;
   lpPublicInfo->wHeadrDiskNum   = lpFile->wDiskNum;
   lpPublicInfo->lHeadrOffset    = lpFile->lCompHeadrStrt;
   lpPublicInfo->wDataDiskNum    = lpFile->dbComprssdFile.wDiskNum;
   lpPublicInfo->lCompDataOffset = lpFile->dbComprssdFile.lStart;
   lstrcpy( lpPublicInfo->szName, lpFile->szName );
   lstrcpy( lpPublicInfo->szPath, lpFile->lpszPath );
}



//==========================================================================
//
// WORD Zip_IsFileInArchive( lpDir, lpFileName )
//
//    lpDir      - Pointer to the information structure used to process
//                 an archive file.
//    lpFileName - Pointer to the name of the file to search for in the
//                 archive file.
//
// This function searches the "internal directory", constructed from the
// archive file, for the specified file.
//
// This function returns the zero-based index position within the "internal
// directory" for the file in question or a value of -1 if the file wasn't
// found.
//
//==========================================================================

WORD WINAPI Zip_IsFileInArchive(
   LPZIP_ARCHIVE_DIR lpDir,
   LPCTSTR            lpszFileName
)

{
   TCHAR              szName[ FSV_PATHBUF_LEN ];
   WORD              wIndx;
   LPLPZIP_COMP_INFO lplpCompFiles;

   for ( wIndx = lpDir->wFileCount - 1, lplpCompFiles = &lpDir->lplpCompFiles[ wIndx ];
         wIndx != ( WORD ) -1;
         wIndx--, lplpCompFiles-- )
   {
      lstrcpy( szName, lplpCompFiles[ 0 ]->lpszPath );
      lstrcat( szName, lplpCompFiles[ 0 ]->szName );

      if ( lstrcmpi( szName, lpszFileName ) == 0 )
      /* The file name in question has been found. */
      {
         break;
      }
   }

   return( wIndx );
} /* Zip_IsFileInArchive */



//==========================================================================
//
// ZIP_ERRCODE Zip_MemWrite( lpFBuf, lpBuf, wBytsToWrt, lpfnCrit, dwCrit )
//
//    lpFBuf     - Pointer to the structure used to buffer file i/o.
//    lpBuf      - Pointer to the buffer that contains the data to copy to
//                 the i/o buffer and/or disk file.
//    wBytsToWrt - The number of bytes to write.
//    lpfnCrit   - Not Used.
//    dwCrit     - Not Used.
//
// This function will is to use write "expanded data" to a memory buffer.
// It only copies the bytes when the ....
//
// This function will return the number of bytes to write.  It does no error
// checking and should never generate an error.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_MemWrite(
   LPZIP_MBUF_INFO  lpMemBuf,
   LPVOID           lpBuf,
   WORD             wBytsToWrt,
   LPZIP_ERR_CBFUNC lpfnCrit,
   DWORD            dwCrit
)

{
   WORD wNumByts = wBytsToWrt;

   if ( lpMemBuf->lStart > lpMemBuf->lVirtOff )
   /* Still expanding data to reach the start of the "output stream". */
   {
      LONG lCount;

      lCount              = min( ( LONG ) wNumByts, lpMemBuf->lStart - lpMemBuf->lVirtOff );
      lpMemBuf->lVirtOff += lCount;
      wNumByts           -= ( WORD ) lCount;
      ( LPBYTE ) lpBuf   += lCount;
   }

   if ( wNumByts > 0 )
   /* Copy the appropriate number of bytes. */
   {
      /* Test for buffer overflow. */
      wNumByts = min( wNumByts, lpMemBuf->wBufSiz - ( WORD ) ( lpMemBuf->lVirtOff - lpMemBuf->lStart ) );

      if ( wNumByts > 0 )
      {
         _fmemcpy( lpMemBuf->lpBuf, lpBuf, wNumByts );
         lpMemBuf->lpBuf    += wNumByts;
         lpMemBuf->lVirtOff += ( LONG ) wNumByts;
      }
   }

   return( wBytsToWrt );
} /* Zip_MemWrite */



//==========================================================================
//
// int Zip_PListCompare( lpArg1, lpArg2 );
//
//    lpArg1 - The 1st pointer to compare.
//    lpArg2 - The 2nd pointer to compare.
//
// This function will is called by the C RunTime routine qsort().  It
// is passed two compressed info pointers.  The pointers are de-refenced
// and the "file position for the compressed file header" is compared.
//
// Returns:
//    -1 - Arg1 is less than Arg2.
//    0  - Arg1 is equal to Arg2.
//    1  - Arg1 is greater than Arg2.
//
//==========================================================================

int _cdecl Zip_PListCompare(
   const VOID* lpArg1,
   const VOID* lpArg2
)

{
  LONG lDif = ( *( LPLPZIP_COMP_INFO ) lpArg1 )->lCompHeadrStrt -
              ( *( LPLPZIP_COMP_INFO ) lpArg2 )->lCompHeadrStrt;

   if ( lDif < 0 )
   {
      return( -1 );
   }

   if ( lDif > 0 )
   {
      return( 1 );
   }

   return( 0 );
} /* Zip_PListCompare */



//==========================================================================
//
// ZIP_ERRCODE Zip_CopyDataBlk( lpH, lpSrcBlock, lpDstFile, bDsplyProg )
//
//    lpH         - Pointer to the information structure used to process a
//                  connection to an archive file.  This structure contains
//                  the pointer to the buffer that will be used transfer the
//                  data within the same file or across different files.  If
//                  the pointer to this structure is NULL then a temporary
//                  buffer will allocated upon entry and freed upon exit.
//    lpSrcBlock  - Pointer to the structure that describes what area in
//                  the archive file is to be copied.
//    lpDstFile   - Pointer to the info for the file that will receive the
//                  specified block.  The data will be copied to the current
//                  file position.
//    bDsplyProg  - If this flag is TRUE then call the progress callback
//                  function (providing one exists) as the data block is
//                  being copied.
//
// This function copies a block of data within the same file or it copies
// a data block from one file to another file.  If the destination file is
// the same as the source file and the areas "overlap" then it is assummed
// that starting file position of the destination is "before" the starting
// file position of the source.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_NO_MEMORY - No memory available for the temporary buffer.
//    FSVERR...     - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_CopyDataBlk(
   LPZIP_HANDLE_INFO    lpH,
   LPZIP_DATA_BLK       lpSrcBlock,
   LPFSVBUFIO_FBUF_INFO lpDstFile,
   BOOL                 bDsplyProg
)

{
   LPZIP_ARCHIVE_DIR    lpDir       = lpH->lpDir;
   BOOL                 bAllocBuf   = ( lpDir->lpTmpBuf == NULL );
   LPFSVBUFIO_FBUF_INFO lpSrcFile   = Zip_GetFBufInfo( lpDir, lpSrcBlock->hbFleIndx );
   LONG                 lCurPostn   = FsvBufIO_fTell( lpDstFile );
   LONG                 lBytsToCopy = lpSrcBlock->lSize;
   LONG                 lBytsCopied;
   ZIP_ERRCODE          eErr;
   LPBYTE               lpBuf;
   WORD                 wBufSiz;
   LONG                 lReadPostn;
   LONG                 lWrtPostn;

   if ( lBytsToCopy == 0 || lpSrcBlock->lStart == lCurPostn &&
        lpSrcFile == lpDstFile )
   /* There is no data to copy or the source file position is the */
   /* same as the destination file postiion so try to skip over   */
   /* the "duplicate" data block.                                 */
   {
      if ( lBytsToCopy == 0 ||
           ( eErr = FsvBufIO_fSeek( lpDstFile, lpSrcBlock->lSize, SEEK_CUR,
                                    lpH->lpCritCBFunc, lpH->dwCritCBData ) ) > 0 )
      {
         eErr = ZIP_NO_ERROR;
      }

      return( eErr );
   }

   /* Either use the buffer provided or allocate a temporary one. */
   if ( bAllocBuf )
   {
      wBufSiz = ( WORD ) min( ( LONG ) ZIP_COPY_BUFFER_SIZE, lBytsToCopy );

      if ( ( lpBuf = ( LPBYTE ) Mem_SubMalloc( wBufSiz ) ) == NULL )
      /* Bad news.  The temporary buffer could not be allocated. */
      {
         return( ZIP_NO_MEMORY );
      }
   }
   else
   {
      lpBuf   = lpDir->lpTmpBuf;
      wBufSiz = lpDir->wTmpBufSiz;
   }

   /* Init copy variables. */
   lReadPostn  = lpSrcBlock->lStart;
   lWrtPostn   = lCurPostn;
   lBytsCopied = 0;

   /* This is a bit of a side-effect.  Update of the input block info */
   /* to reference the new file position of the data block.           */
   lpSrcBlock->lStart = lWrtPostn;

   /* Loop to copy one area of a file to another. */
   do
   {
      WORD wByts = ( WORD ) min( ( LONG ) wBufSiz, lBytsToCopy );

      /* Set file pointer to start of the current read area. */
      if ( ( eErr = FsvBufIO_fSeek( lpSrcFile, lReadPostn, SEEK_SET, lpH->lpCritCBFunc,
                                    lpH->dwCritCBData ) ) != lReadPostn )
      /* Bad news.  Unable to access the disk. */
      {
         break;
      }

      /* Read as many bytes from the source as possible. */
      if ( ( eErr = FsvBufIO_fRead( lpSrcFile, lpBuf, wByts, lpH->lpCritCBFunc,
                                    lpH->dwCritCBData ) ) < 0 )
      /* Bad news, a disk error was detected. */
      {
         break;
      }
      else
      if ( ( WORD ) eErr < wByts )
      /* Bad news.  The end of the source file was detected. */
      /* This should never happen.                           */
      {
         eErr = FSVERR_ACCESS_DENIED;
         break;
      }

      /* Write the appropriate portion of the source area to the */
      /* destination area.                                       */
      if ( ( eErr = FsvBufIO_fSeek( lpDstFile, lWrtPostn, SEEK_SET, lpH->lpCritCBFunc,
                                    lpH->dwCritCBData ) ) != lWrtPostn ||
           ( eErr = FsvBufIO_fWrite( lpDstFile, lpBuf, wByts, lpH->lpCritCBFunc,
                                     lpH->dwCritCBData ) ) != ( LONG ) wByts )
      /* Bad news.  Unable to write to disk. */
      {
         break;
      }

      /* Update count and file position varaibles. */
      lBytsCopied += wByts;
      lBytsToCopy -= wByts;
      lReadPostn  += wByts;
      lWrtPostn   += wByts;
      eErr         = ZIP_NO_ERROR;

      if ( bDsplyProg && lpH->lpProgCBFunc != NULL && lBytsToCopy > 0 )
      /* Inform the user of the progress of the compression/expansion. */
      {
         ZIP_PROGRESSINFO piProgInfo = { ( WORD ) ( lBytsCopied * 100 / lpSrcBlock->lSize ),
                                         ZIP_XFER_FILE, lpH->dwProgCBData };

         if ( lpH->lpProgCBFunc( &piProgInfo ) == ZIP_PROG_CB_ABORT )
         /* The user wants to abort compressing/expanding the file. */
         {
            eErr  = ZIP_ABORTED;
            break;
         }
      }
   }
   while ( lBytsToCopy > 0 );

   if ( bAllocBuf )
   /* Free the temporary transfer buffer. */
   {
      Mem_SubFree( lpBuf );
   }

   /* We be kool. */
   return( eErr );
} /* Zip_CopyDataBlk */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_TransOpenOptns( wOpenOptns )
//
//    wOpenOptns - The access mode (read or read/write) and the share
//                 privileges.
//
// This function converts the ZIP DLL open flags to the flags used by
// the WINDOWS function _lopen().
//
//==========================================================================

int WINAPI Zip_TransOpenOptns(
   WORD wOpenOptns
)

{
   int fsFlags      = OF_READ;
   int fsShareFlags = OF_SHARE_COMPAT;

   if ( wOpenOptns & ZIP_READWRITE )
   /* When an archive file is being modified the affiliate task */
   /* must have exclusive access.                               */
   {
      fsFlags      = OF_READWRITE;
      fsShareFlags = OF_SHARE_EXCLUSIVE;
   }

   if ( wOpenOptns & ZIP_EXCLUSIVE )
   /* The caller requires exclusive access to the file. */
   {
      fsShareFlags = OF_SHARE_EXCLUSIVE;
   }

   return( fsFlags | fsShareFlags );
} /* Zip_TransOpenOptns */



//==========================================================================
//
// ZIP_ERRCODE Zip_CloseChk( lpH, bGetFileInfo )
//
//    lpH          - Pointer to info for the handle in question.
//    bGetFileInfo - If this flag is TRUE and the handle pertains to the
//                   last "active" connection to the file then get the
//                   new file date/time/size after the file is closed.
//
// This function will close the archive file in question if there is only
// one "connection" that has the file open.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR_...   - The appropriate File Services error code.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_CloseChk(
   LPZIP_HANDLE_INFO lpH,
   BOOL              bGetFileInfo
)

{
   ZIP_ERRCODE       eErr  = ZIP_NO_ERROR;
   LPZIP_ARCHIVE_DIR lpDir = lpH->lpDir;

   /* Decrement the number of active connections made to the file. */
   lpDir->wActiveCount--;

   if ( lpDir->wActiveCount == 0 )
   /* This was the last "active" connection to the archive file */
   /* so close it.                                              */
   {
      /* Close the file and "free" the i/o buffer. */
      eErr = FsvBufIO_fClose( lpDir->fiArchvFile.lpFBuf, TRUE, lpH->lpCritCBFunc,
                              lpH->dwCritCBData );
      lpDir->fiArchvFile.lpFBuf = NULL;

      if ( bGetFileInfo )
      /* Get statistical info about the archive file. */
      {
         FSVIO_DIRENTRYSTRUCT diDirInfo;

         if ( eErr == FSV_SUCCESS &&
              FsvIO_GetDirEntry( lpDir->fiArchvFile.szFullName, &diDirInfo, NULL,
                                 lpH->lpCritCBFunc, lpH->dwCritCBData ) == FSV_SUCCESS )
         /* Save size of file and the date and time it was last modified. */
         {
            lpDir->SI.wDate     = diDirInfo.wDate;
            lpDir->SI.wTime     = diDirInfo.wTime;
            lpDir->SI.dwFileSiz = diDirInfo.dwSize;
         }
         else
         /* Store bogus info to force a "re-parse". */
         {
            lpDir->SI.wDate     = 0;
            lpDir->SI.wTime     = 0;
            lpDir->SI.dwFileSiz = 0;
         }
      }
   }

   return( eErr );
} /* Zip_CloseChk */



//==========================================================================
//
// ZIP_ERRCODE Zip_ReOpenChk( lpH, wOpenOptns )
//
//    lpH        - Pointer to info for the handle in question.
//    wOpenOptns - A series of flags which are used to indicate the access
//                 privileges (read only or read/write) of the archive file
//                 and the request to use a temporary file to contain files
//                 that are "replaced" in the archive file.
//
// This function checks to see if the archive file associated with a
// "connection" needs to be opened.  It is does then the file will be
// "opened", and if it is detected that it has been modified, then the
// "internal directory" will be freed and re-constructed.
//
// *****FIX***** ==> Notify all registered callers that the "internal
//                   directory" has been re-constructed.
//
// Returns:
//    ZIP_NO_ERROR     - The operation was successful.
//    ZIP_UNRECNZD_ZIP - The file is not a recognized zip file.
//    FSVERR_...       - The appropriate File Services error code.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_ReOpenChk(
   LPZIP_HANDLE_INFO lpH,
   WORD              wOpenOptns
)

{
   ZIP_ERRCODE       eErr  = ZIP_NO_ERROR;
   LPZIP_ARCHIVE_DIR lpDir = lpH->lpDir;

   /* If relevant, try to open archive file in question. */
   if ( ( lpDir->SI.bBadState && lpDir->wActiveCount > 0 ) ||
        ( lpDir->wActiveCount == 0 &&
          ( eErr = FsvBufIO_fOpen( lpDir->fiArchvFile.szFullName,
                                   Zip_TransOpenOptns( wOpenOptns ), FSVBUFIO_FILE_BUF_SIZ,
                                   &lpDir->fiArchvFile.lpFBuf, lpH->lpCritCBFunc,
                                   lpH->dwCritCBData ) ) == ZIP_NO_ERROR ) )
   /* The archive file has been re-opened. */
   {
      FSVIO_DIRENTRYSTRUCT diDirInfo;

      /* Get statistical info about the archive file. */
      if ( FsvIO_GetDirEntry( lpDir->fiArchvFile.szFullName, &diDirInfo, NULL,
                              lpH->lpCritCBFunc, lpH->dwCritCBData ) != FSV_SUCCESS )
      /* Store bogus info to force a "re-parse". */
      {
         lpDir->SI.wDate     = ( WORD  ) -1;
         lpDir->SI.wTime     = ( WORD  ) -1;
         lpDir->SI.dwFileSiz = ( DWORD ) -1;
      }

      if ( lpDir->SI.bBadState                    ||
           lpDir->SI.wDate     != diDirInfo.wDate ||
           lpDir->SI.wTime     != diDirInfo.wTime ||
           lpDir->SI.dwFileSiz != diDirInfo.dwSize )
      /* The archive file was left in an unstable state when ZipPack() */
      /* failed or it has changed since it was "temporarily" closed.   */
      /* Free the "internal directory" and re-construct it.            */
      {
         Zip_FreeComprssdInfoRecs( lpDir );
         eErr = Zip_BuildInternalDir( lpH );
      }
   }

   return( eErr );
} /* Zip_ReOpenChk */



//==========================================================================
//
// ZIP_ERRCODE Zip_GetDirInfo( hArchvFile, lplpH, eAccessOp )
//
//    hArchvFile    - Handle (one-based) to the archive file to get the
//                    directory info for.
//    lplpH         - A pointer to the pointer which will point to the handle
//                    info for the handle in question.
//    eAccessOp     - The reason the caller is accessing the archive file
//                    (read, write, open, reopen, close).
//
// This function returns the "internal directory" information that is
// associated with the specified handle.
//
// Globals:
//    gHandleTbl    - Table used to process the archive handles.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_BAD_ARG   - The specified handle is invalid or it has been
//                    temporarily closed.
//    ZIP_DIR_ERROR - The ZipPack() function failed and the archive file has
//                    been left in a bad state.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_GetDirInfo(
   HZIP                hArchvFile,
   LPLPZIP_HANDLE_INFO lplpH,
   ZIP_DIR_ACCESS_OP   eAccessOp
)

{
   LPZIP_HANDLE_INFO lpHandleRec;
   LPZIP_ARCHIVE_DIR lpDir;

   /* Assume an error. */
   *lplpH = NULL;

   if ( ( WORD ) --hArchvFile >= gHandleTbl.wLastHandleUsed ||
        ( lpHandleRec = &gHandleTbl.hlHandleLst[ hArchvFile ] ) == NULL ||
        ( lpDir = lpHandleRec->lpDir ) == NULL ||
        lpHandleRec->hsDirHandleSt == ZIP_HANDLE_AVAIL ||
        lpHandleRec->hsDirHandleSt == ZIP_DIR_INIT_STATE ||
        ( ( eAccessOp == ZIP_DIR_READ || eAccessOp == ZIP_DIR_WRITE ) &&
          lpHandleRec->hsDirHandleSt != ZIP_DIR_OPENED ) ||
        ( eAccessOp == ZIP_DIR_REOPEN && lpHandleRec->hsDirHandleSt != ZIP_DIR_TMP_CLOSED ) )
   /* Bad news.  The specified handle is invalid. */
   {
      return( ZIP_BAD_ARG );
   }

   #ifdef _DEBUG
   if ( Zip_BadDir( lpDir ) )
   {
      return( ZIP_BAD_ARG );
   }
   #endif

   if ( lpDir->SI.bBadState && eAccessOp != ZIP_DIR_CLOSE && eAccessOp != ZIP_DIR_REOPEN )
   /* Archive file is in a bad state.  Allow processing for close */
   /* and reopen.  The file will be reparsed for reopen.          */
   {
      return( ZIP_DIR_ERROR );
   }

   *lplpH = lpHandleRec;
   return( ZIP_NO_ERROR );
} /* Zip_GetDirInfo */



//==========================================================================
//
// VOID Zip_InitArchiveDir( lpDir, lpFBuf, lpszFullName, eArchvTyp,
//                          bArchvVrfd, bPrsrvCntInfo, bDirty )
//
//    lpDir         - Pointer to the information structure used to process
//                    an archive file.
//    lpFBuf        - Pointer to the structure used to buffer file i/o.
//    lpszFullName  - The full name of the archive file.
//    eArchvTyp     - The type of the archive file (zip, pak, or arc).
//    bArchvVrfd    - If this flag is TRUE then all of the file header
//                    records in the archive file have been verified.  This
//                    flag only pertains to ZIP files.
//    bPrsrvCntInfo - If this flag is TRUE then the "count" information
//                    (used to keep track of multiple connections) will be
//                    preserved.
//    bDirty        - This flag is TRUE if the archive directory has
//                    been "dirtied".
//
// Globals:
//    gszNullPath - The zero byte null terminated string used to prevent
//                  excessive memory allocations.
//
// This function initializes all of the state variables in the archive
// directory structure.
//
//==========================================================================

VOID WINAPI Zip_InitArchiveDir(
   LPZIP_ARCHIVE_DIR    lpDir,
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LPCTSTR               lpszFullName,
   ZIP_TYPE             eArchvTyp,
   BOOL                 bArchvVrfd,
   BOOL                 bPrsrvCntInfo,
   BOOL                 bDirty
)

{
   WORD wUseCount;
   WORD wActiveCount;
#ifndef NAV95                           // Not supporting multivolume ZIP
   WORD wDrvTyp = ( WORD ) FsvIO_GetDriveType( FsvUtil_ConvrtDrvLtr2ID( lpszFullName[ 0 ] ) );
#endif

   if ( bPrsrvCntInfo )
   {
      wUseCount    = lpDir->wUseCount;
      wActiveCount = lpDir->wActiveCount;
   }

   /* Most of the fields need to be initialized to zero. */
   _fmemset( lpDir, 0, sizeof( ZIP_ARCHIVE_DIR ) );

   // Set appropriate state info.
   lpDir->bArchvVrfd = bArchvVrfd;
   lpDir->eArchvTyp  = eArchvTyp;
   lpDir->SI.bDirty  = bDirty;
   lpDir->lpszRemark = &gszNullPath[ 0 ];

   // Init fields for ZIP files that span multiple disks.
// lpDir->SI.wCurDisk  = 0;
// lpDir->SI.wStrtDisk = 0;
// lpDir->SI.wLastDisk = 0;

   /* Save the file info. */
   lpDir->fiArchvFile.lpFBuf = lpFBuf;
   lstrcpy( lpDir->fiArchvFile.szFullName, lpszFullName );

   if ( bPrsrvCntInfo )
   {
      lpDir->wUseCount    = wUseCount;
      lpDir->wActiveCount = wActiveCount;
   }

#ifndef NAV95                           // Not supporting multivolume ZIP
   //-*****FIX***** For ZipOpen() don't know the archive file type yet.
   if ( wDrvTyp == FSV_DRIVE_FLOPPY525 || wDrvTyp == FSV_DRIVE_FLOPPY35 )
   // The "split archive file" feature only works on floppies for ZIP files.
   {
      lpDir->SI.fCanBeSplit = TRUE;
   }
#endif

} /* Zip_InitArchiveDir */



//==========================================================================
//
// ZIP_ERRCODE Zip_ScanForFileInDirTable( lpszFile, wAccessPriv, lplpDir )
//
//    lpszArchvFile - Pointer to the name of the archive file to scan for.
//    wAccessPriv   - The access privileges for the file in question (read
//                    only or read/write).
//    lplpDir       - A pointer to the pointer which points to the directory
//                    information structure for the "connection" to the file
//                    in question.  If a connection has not been made or an
//                    access privilege conflict has been detected then a
//                    value of NULL will be returned.
//
// Globals:
//    gHandleTbl    - Table used to process the archive handles.
//
// This function scans the archive handle table for any "connections" to the
// archive file in question.
//
// This function actually returns two control values.  The first is the
// function result which indicates whether an error was detected.  The
// second is the pointer to the affiliate directory structure.  It will
// only be a valid pointer if a connection has already been made and no
// sharing violations were detected.
//
// Returns:
//    ZIP_NO_ERROR      - A connection has not previously been made with the
//                        archive file or a connection has been made and there
//                        were no sharing conflicts.
//    ZIP_ACCESS_DENIED - A "connection" has already been made to the file
//                        in question with read/write access (I only allow
//                        one such connection).
//    FSVERR_...        - The appropriate File Services error code.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_ScanForFileInDirTable(
   LPCTSTR              lpszArchvFile,
   WORD                wAccessPriv,
   LPLPZIP_ARCHIVE_DIR lplpDir
)

{
   LPZIP_ARCHIVE_DIR lpTmpDir = NULL;
   LPZIP_HANDLE_INFO lpHandleInfo;
   WORD              wIndx;

   /* Scan the handle table for the specified archive table. */
   for ( wIndx = 0, lpHandleInfo = &gHandleTbl.hlHandleLst[ 0 ];
         wIndx < gHandleTbl.wLastHandleUsed;
         wIndx++, lpHandleInfo++ )
   {
      LPZIP_ARCHIVE_DIR lpDir = lpHandleInfo->lpDir;

      if ( lpHandleInfo->hsDirHandleSt != ZIP_HANDLE_AVAIL &&
           lpHandleInfo->hsDirHandleSt != ZIP_DIR_INIT_STATE &&
           lstrcmpi( lpszArchvFile, lpDir->fiArchvFile.szFullName ) == 0 )
      /* A duplicate entry has been detected. */
      {
         if ( lpHandleInfo->hsDirHandleSt == ZIP_DIR_OPENED &&
              ( ( lpHandleInfo->wOpenOptns & ( ZIP_READWRITE | ZIP_EXCLUSIVE ) ) ||
                ( wAccessPriv & ( ZIP_READWRITE | ZIP_EXCLUSIVE ) ) ) )
         /* Bad news. The archive file has an "active" connection with */
         /* read/write or exclusive privileges or the new connection   */
         /* requires read/write or exclusive privileges.               */
         {
            *lplpDir = NULL;
            return( ZIP_ACCESS_DENIED );
         }

         /* A potential candidate has been found. */
         lpTmpDir = lpDir;
      }
   }

   #ifdef _DEBUG
   if ( lpTmpDir != NULL && Zip_BadDir( lpTmpDir ) )
   {
      return( ZIP_BAD_ARG );
   }
   #endif

   /* Return NULL or an active connection. */
   *lplpDir = lpTmpDir;

   return( ZIP_NO_ERROR );
} /* Zip_ScanForFileInDirTable */



//==========================================================================
//
// VOID Zip_InitHandleRec( lpH, lpDir, wOpenOptns, lpProgCBFunc, dwProgCBData,
//                         lpErrCBFunc, dwErrCBData, lpfnSpltDskCB,
//                         dwSpltDskData )
//
//    lpH           - Pointer to info for the handle in question.
//    lpDir         - Pointer to the information structure used to process an
//                    archive file.
//    wOpenOptns    - The flags used to indicate the access privileges (read
//                    only or read/write) of the archive file.
//    lpProgCBFunc  - Pointer to the "progress" callback function.
//    dwProgCBData  - Data that is to be passed the "progress" callback
//                    function.
//    lpErrCBFunc   - Pointer to the "critical error" callback function.
//    dwErrCBData   - Data that is to be passed to the "critical error"
//                    callback function.
//    lpfnSpltDskCB - Pointer to the "split disk" callback function.  This
//                    value can be NULL.
//    dwSpltDskData - Data that is to be passed to the "split disk" callback
//                    function.  This value can be NULL.
//
// This function initializes the specified handle record with the new
// "connection".
//
//==========================================================================

VOID WINAPI Zip_InitHandleRec(
   LPZIP_HANDLE_INFO      lpH,
   LPZIP_ARCHIVE_DIR      lpDir,
   WORD                   wOpenOptns,
   LPZIP_PROG_CBFUNC      lpProgCBFunc,
   DWORD                  dwProgCBData,
   LPZIP_ERR_CBFUNC       lpErrCBFunc,
   DWORD                  dwErrCBData,
   LPFNZIP_SPLTDSK_CBFUNC lpfnSpltDskCB,
   DWORD                  dwSpltDskData
)

{
   // Init the handle record.
   lpH->lpDir         = lpDir;
   lpH->wOpenOptns    = wOpenOptns;
   lpH->hsDirHandleSt = ZIP_DIR_INIT_STATE;
   lpH->lpProgCBFunc  = lpProgCBFunc;
   lpH->dwProgCBData  = dwProgCBData;
   lpH->lpCritCBFunc  = lpErrCBFunc;
   lpH->dwCritCBData  = dwErrCBData;
   lpH->lpfnSpltDskCB = lpfnSpltDskCB;
   lpH->dwSpltDskData = dwSpltDskData;
   lpH->lPercntMax    = 100;
   lpH->wPercntOff    = 0;

} /* Zip_InitHandleRec */



//==========================================================================
//
// HZIP Zip_GetNewHandle( )
//
//
// Globals:
//    gHandleTbl - Table used to process the archive handles.
//
// This function tries to find an available archive handle.  If one is found
// then the affiliate directory info will be initialized and it will the
// responsibility of the caller to fill in the appropriate information.
//
// This function returns the number of the available archive handle (one
// based - I'm being compatible with the original definition of the ZIP
// API) if it was successful or it returns NULL if all of the directory
// handles are in use.
//
//==========================================================================

HZIP WINAPI Zip_GetNewHandle( VOID )

{
   HZIP hArchvFile = 0;

   if ( gHandleTbl.wHandlesInUse < ZIP_MAX_HANDLES )
   {
      if ( gHandleTbl.wHandlesInUse == gHandleTbl.wLastHandleUsed )
      /* The handles allocated so far are contiguous so simply */
      /* allocate the handle that follows the last one used.   */
      {
         gHandleTbl.hlHandleLst[ gHandleTbl.wLastHandleUsed ].hsDirHandleSt = ZIP_DIR_INIT_STATE;
         gHandleTbl.wLastHandleUsed++;
         gHandleTbl.wHandlesInUse++;
         hArchvFile = gHandleTbl.wHandlesInUse;
      }
      else
      /* Scan the table of handles for the first available handle. */
      {
         LPZIP_HANDLE_INFO lpHandleLst;
         WORD              wIndx;

         for ( wIndx = 1, lpHandleLst = &gHandleTbl.hlHandleLst[ 0 ];
               wIndx <= ZIP_MAX_HANDLES;
               wIndx++, lpHandleLst++ )
         {
            if ( lpHandleLst->hsDirHandleSt == ZIP_HANDLE_AVAIL )
            /* An available handle has been detected. */
            {
               hArchvFile = wIndx;
               gHandleTbl.wHandlesInUse++;
               lpHandleLst->hsDirHandleSt = ZIP_DIR_INIT_STATE;
               break;
            }
         }
      }
   }

   return( hArchvFile );
} /* Zip_GetNewHandle */



//==========================================================================
//
// VOID Zip_FreeHandle( hArchvFile )
//
//    hArchvFile - The one-based entry in the table of archive handles to
//                 "free".
//
// Globals:
//    gHandleTbl - Table used to process the archive handles.
//
// This function "frees" the specified entry in the table of archive
// handles.
//
//==========================================================================

VOID WINAPI Zip_FreeHandle(
   HZIP hArchvFile
)

{
   LPZIP_HANDLE_INFO lpHandleInfo = &gHandleTbl.hlHandleLst[ hArchvFile - 1 ];

   /* Clean out the entry. */
   lpHandleInfo->lpDir         = NULL;
   lpHandleInfo->hsDirHandleSt = ZIP_HANDLE_AVAIL;

   gHandleTbl.wHandlesInUse--;

   if ( ( WORD ) hArchvFile == gHandleTbl.wLastHandleUsed )
   /* Find the last handle used. */
   {
      for ( ;
            gHandleTbl.wLastHandleUsed > 0 && lpHandleInfo->hsDirHandleSt == ZIP_HANDLE_AVAIL;
            gHandleTbl.wLastHandleUsed--, lpHandleInfo-- );
   }

} /* Zip_FreeHandle */



//==========================================================================
//
// VOID Zip_InitCompFileRec( lpFileInfo, bDirty, bHeadrVrfyd, lpszName )
//
//    lpFileInfo  - Pointer to the compressed file info structure.
//    bDirty      - This flag will be TRUE if any data in this structure
//                  has been modified since it was read in from disk.
//    bHeadrVrfd  - This flag only pertains to compressed files contained
//                  in a ZIP archive file.  If this flag is FALSE then the
//                  data in this structure will be compared with the data
//                  stored in the compressed file header when it is being
//                  "extracted".
//    lpszName    - Pointer to the compressed file name to store.  This
//                  pointer can be NULL.
//
// Globals:
//    gszNullPath - The zero byte null terminated string used to prevent
//                  excessive memory allocations.
//
// This function initializes a compressed file info record.
//
// Notes:
//    1) This function sets the "path pointer" to an "empty" string that
//       is contained in global data segment.  This is done to avoid making
//       lots of redundant one byte allocations.
//
//==========================================================================

VOID WINAPI Zip_InitCompFileRec(
   LPZIP_COMP_INFO lpFile,
   BOOL            bDirty,
   BOOL            bHeadrVrfd,
   LPCTSTR          lpszName
)

{
   /* Most of the fields need to be initialized to zero. */
   _fmemset( lpFile, 0, sizeof( ZIP_COMP_INFO ) );

   lpFile->bDirty     = bDirty;
   lpFile->bHeadrVrfd = bHeadrVrfd;
   lpFile->lpszPath   = &gszNullPath[ 0 ];
   lpFile->lpszRemark = &gszNullPath[ 0 ];
   lpFile->ePckTyp    = ZIP_NONE;
   lpFile->bLevel     = 1;

   if ( lpszName != NULL )
   /* Store specified file name (check for overflow). */
   {
      _fmemcpy( lpFile->szName, lpszName, ZIP_NAME_SIZ );
      lpFile->szName[ ZIP_NAME_SIZ - 1 ] = '\0';
   }

} /* Zip_InitCompFileRec */



//==========================================================================
//
// ZIP_ERROCDE Zip_InsCompFileRec( lpDir, uIndx, lpFileInfo )
//
//    lpDir      - Pointer to the information structure used to process
//                 an archive file.
//    wIndx      - The position in the list to insert the pointer ot the
//                 archive file info structure.
//    lpFileInfo - Pointer to the compressed file info structure.
//
// This function inserts a pointer to a compressed file info structure in a
// list of pointers at the specified index position.  If the index is
// greater than the size of the list then the pointer will be added to the
// end of the list.  The list that contains the compressed file info pointers
// will grow dynamically.
//
// Returns:
//  ZIP_NO_ERROR  - The operation was successful.
//  ZIP_NO_MEMORY - There is not enough memory to dynamically grow the
//                  list.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_InsCompFileRec(
   LPZIP_ARCHIVE_DIR lpDir,
   WORD              wIndx,
   LPZIP_COMP_INFO   lpFileInfo
)

{
   #define ZIP_INITIAL_FILE_LST_SIZ 100

   if ( lpDir->lplpCompFiles == NULL )
   {
      lpDir->lplpCompFiles = ( LPLPZIP_COMP_INFO )
                             ListMalloc( ZIP_INITIAL_FILE_LST_SIZ,
                                         sizeof( LPZIP_COMP_INFO ),
                                         LIST_ALLOC_SHARED );

      if ( lpDir->lplpCompFiles == NULL )
      {
         return( ZIP_NO_MEMORY );
      }
   }

  /* Make sure the index of the new element is valid. */
  if ( wIndx >= lpDir->wFileCount )
  {
    wIndx = lpDir->wFileCount;
  }

  if ( ListInsert( ( LPVOID FAR* ) &lpDir->lplpCompFiles, wIndx,
                   ( LPVOID ) &lpFileInfo ) == LIST_INDEXNONE )
  {
     return( ZIP_NO_MEMORY );
  }

  /* Account for the new file information record. */
  lpDir->wFileCount++;

  return( ZIP_NO_ERROR );
} /* Zip_InsCompFileRec */



//==========================================================================
//
// ZIP_ERROCDE Zip_DelCompFileRec( lpDir, uIndx )
//
//    lpDir  - Pointer to the information structure used to process
//             an archive file.
//    wIndx  - The position in the list of the pointer to delete.
//
// This function frees the compressed file info record that corresponds to
// the index and then delete corresponding pointer from the pointer list.
//
// Returns:
//  ZIP_NO_ERROR - The operation was successful.
//  ZIP_BAD_ARG  - The specified index is invalid.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_DelCompFileRec(
   LPZIP_ARCHIVE_DIR lpDir,
   WORD              wIndx
)

{
   if ( wIndx >= lpDir->wFileCount )
   // The pointer list is "empty".
   {
      return( ZIP_BAD_ARG );
   }

   // Free all dynamically allocated memory.
   Zip_FreeComprssdRec( lpDir->lplpCompFiles[ wIndx ] );

   // Delete pointer from the list and decrement file count.
   ListDelete( ( LPVOID ) &lpDir->lplpCompFiles, wIndx );
   lpDir->wFileCount--;

   if ( lpDir->wFileCount == 0 )
   // The file info pointer list is empty so free it.
   {
      Zip_FreeComprssdInfoRecs( lpDir );
   }

   return( ZIP_NO_ERROR );
} // Zip_DelCompFileRec



//==========================================================================
//
// VOID Zip_FreeComprssdRec( lpCompFile )
//
//    lpCompFile  - Pointer to the structure used to describe a compressed
//                  file.
//
// Globals:
//    gszNullPath - The zero byte null terminated string used to prevent
//                  excessive memory allocations.
//
// This function "frees" all the dynamic data allocated in a compressed info
// record and then frees the compressed info record.
//
//==========================================================================

VOID WINAPI Zip_FreeComprssdRec(
   LPZIP_COMP_INFO lpCompFile
)

{
   Zip_FreeRemarkChk( lpCompFile->lpszRemark );
   Zip_FreePathChk( lpCompFile->lpszPath );
   Mem_SubFree( lpCompFile );
} /* Zip_FreeComprssdRec */



//==========================================================================
//
// VOID Zip_FreeComprssdInfoRecs( lpDir )
//
//    lpDir - Pointer to the information structure used to process
//            an archive file.
//
// This function, if necessary, frees all of the compressed file info records
// referenced in the list of compressed info records, and then frees the
// the affiliate list.
//
//==========================================================================

VOID WINAPI Zip_FreeComprssdInfoRecs(
   LPZIP_ARCHIVE_DIR lpDir
)

{
   LPLPZIP_COMP_INFO lpFileInfoList = lpDir->lplpCompFiles;

   if ( lpFileInfoList != NULL )
   {
      WORD wIndx = lpDir->wFileCount;

      /* Free all of the compressed file info records. */
      for ( ; wIndx > 0; wIndx--, lpFileInfoList++ )
      {
         Zip_FreeComprssdRec( lpFileInfoList[ 0 ] );
      }

      /* Free the array containing the pointers. */
      ListFree( lpDir->lplpCompFiles );

      /* Reset state variables. */
      lpDir->lplpCompFiles = NULL;
      lpDir->wFileCount    = 0;
   }

} /* Zip_FreeComprssdInfoRecs */



//==========================================================================
//
// VOID Zip_FreeDirInfo( lpDir )
//
//    lpDir - Pointer to the information structure used to process
//            an archive file.
//
// This function, frees all of the compressed file info records referenced
// in the list of compressed info records, frees that affiliate list, and
// then frees the directory info structure.
//
//==========================================================================

VOID WINAPI Zip_FreeDirInfo(
   LPZIP_ARCHIVE_DIR lpDir
)

{
   if ( lpDir != NULL )
   {
      // If allocated, free the de-compression state info.
      Zip_DeInitCompressExpandChk( lpDir->RFI.lpvVars, lpDir->RFI.ePckTyp, FALSE );

      Zip_FreeComprssdInfoRecs( lpDir );
      Zip_FreeRemarkChk( lpDir->lpszRemark );
      Mem_SubFree( lpDir );
   }

} /* Zip_FreeDirInfo */



//==========================================================================
//
// BOOL Zip_ConvertOtherPlatformFileName( lpszName, wOSID, lpbDirtied )
//
//    lpszName   - Pointer to the file name contained in the archive file
//                 that might be invalid because the archive file was created
//                 on another platform.  This file name can contain a path.
//    wOSID      - The ID of the OS (platform) that the archive file was
//                 created on.
//    lpbDirtied - This flag will be to TRUE if the file name passed in is
//                 modified (it's length changes).  This flag can be NULL.
//
// This function converts file names created on other platforms to the
// MSDOS file name format.
//
// Returns:
//    FALSE - The file name for the specified platform was invalid.
//    TRUE  - The file name for the specified platform was valid and has been
//            successfully converted to a valid DOS file name.
//
// Notes: ****
//    1) Currently this function only supports file names that were created
//       on DOS and the Amiga.
//
//==========================================================================

BOOL WINAPI Zip_ConvertOtherPlatformFileName(
   LPTSTR  lpszName,
   WORD   wOSID,
   LPBYTE lpbDirtied
)

{
   LPTSTR lpszEnd;
   LPTSTR lpszTmp;

   if ( lstrlen(lpszName) == 0 )
   // Bogus dude.
   {
      return( FALSE );
   }

   // Compute pointer to the NULL terminator.
   lpszEnd = FsvStr_RevStr( lpszName, 0 );
   lpszTmp = lpszEnd;

   // Convert file names from other platforms to the DOS format.
   switch ( wOSID )
   {
      // These file names can have multiple extension characters and a
      // component can have more than eight characters.  Simply use the
      // file name with the rightmost extension character.
      case ZIP_AMIGA:
      {
      }
      break;
   }

   if ( lpbDirtied != NULL )
   // Was the file name modified.
   {
      *lpbDirtied = ( lpszTmp != lpszEnd );
   }

   // Is this file name valid?
   return( FsvUtil_ValidateFullFileName( lpszName, FALSE ) );
} // Zip_ConvertOtherPlatformFileName



//==========================================================================
//
// BOOL Zip_DoesDiskHaveSpecialLabel( lpH, lpszLabel, lpwDskNum )
//
//    lpH       - Pointer to info for the handle in question.
//    lpszLabel - Pointer to the string that will contain the volume label.
//                This pointer can be NULL.
//    lpwDskNum - Pointer to the variable that will contain (providing a valid
//                label is obtained) the number of the disk (0-based) indicated
//                in the label.  This pointer can be NULL.
//
// This function checks to see if the drive that the archive file resides
// on contains the special "signature" volume label.
//
// Returns:
//    FALSE - The diskette is NOT part of multi-diskette archive file.
//    TRUE  - The diskette IS part of multi-diskette archive file.
//
//==========================================================================

BOOL WINAPI Zip_DoesDiskHaveSpecialLabel(
   LPZIP_HANDLE_INFO lpH,
   LPTSTR             lpszLabel,
   LPWORD            lpwDskNum
)

{
   TCHAR szLabel[ FSV_DRIVELABELBUF_LEN + 1 ];
   BOOL fRtn = FALSE;

   if ( FsvIO_GetDriveLabel( FsvUtil_ConvrtDrvLtr2ID( lpH->lpDir->fiArchvFile.szFullName[ 0 ] ),
                             szLabel, FSV_DRIVELABELBUF_LEN, lpH->lpCritCBFunc,
                             lpH->dwCritCBData ) == FSV_SUCCESS )
   {
      WORD wDskNum;

      if ( lstrlen( szLabel ) == ZIP_LABEL_LEN &&
           memcmp( szLabel, ZIP_BASE_LABEL, ZIP_BASE_LABEL_LEN ) == 0 &&
           isdigit( szLabel[ ZIP_BASE_LABEL_LEN ] ) &&
           isdigit( szLabel[ ZIP_BASE_LABEL_LEN + 1 ] ) &&
           isdigit( szLabel[ ZIP_BASE_LABEL_LEN + 2 ] ) &&
           ( wDskNum = atoi( &szLabel[ ZIP_BASE_LABEL_LEN ] ) ) != 0 )
      // The label does contain the special base portion.
      {
         fRtn = TRUE;

         if ( lpwDskNum != NULL )
         // Return the disk number embedded in the label.
         {
            *lpwDskNum = wDskNum - 1;
         }
      }
   }

   if ( lpszLabel != NULL )
   // Return the volume label.
   {
      lstrcpy( lpszLabel, szLabel );
   }

   return( fRtn );
}



//==========================================================================
//
// ZIP_ERRCODE Zip_WriteSpecialLabel( lpH, lpszLabel, wDskNum )
//
//    lpH       - Pointer to info for the handle in question.
//    lpszLabel - Pointer to the string that will contain the volume label
//                that was written to the disk.  This pointer can be NULL.
//    wDskNum   - The number of the disk (0-based) that will be indicated
//                in the label.
//
// This function tries to write out the special "signature" volume label for
// the specified disk number.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services
//                   ( i.e. file not found, disk i/o error, etc.)
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteSpecialLabel(
   LPZIP_HANDLE_INFO lpH,
   LPTSTR             lpszLabel,
   WORD              wDskNum
)

{
   TCHAR        szLabel[ FSV_DRIVELABELBUF_LEN + 1 ];
   ZIP_ERRCODE eErr;

   // Generate the label to write.
   lstrcpy( szLabel, ZIP_BASE_LABEL );
   wsprintf( ( LPTSTR ) &szLabel[ ZIP_BASE_LABEL_LEN ], ( LPCTSTR ) "%03u", wDskNum + 1 );

   // Try to write out the volume label.
   eErr = FsvIO_SetDriveLabel( FsvUtil_ConvrtDrvLtr2ID( lpH->lpDir->fiArchvFile.szFullName[ 0 ] ),
                               szLabel, lpH->lpCritCBFunc, lpH->dwCritCBData );

   if ( lpszLabel != NULL )
   // Return the volume label.
   {
      lstrcpy( lpszLabel, szLabel );
   }

   return( eErr );
}
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_CreateNextArchiveFile( lpH, wDiskNum )
//
//    lpH      - Pointer to info for the handle in question.
//    wDiskNum - The number of disk (0 based) that needs to be inserted.
//
// This function will check to see if the disk that was inserted is empty.
// If it then it will try to write out the special volume label and
// create another archive file.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services
//
// Notes:
//    1) This function can create a new archive file on the current diskette.
//       If that happens lpH->lpDir->fiArchvFile.lpFBuf will be modified.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_CreateNextArchiveFile(
   LPZIP_HANDLE_INFO lpH,
   WORD              wDiskNum
)

{
   ZIP_ERRCODE eErr;
   DWORD       dwFreeSpace;
   DWORD       dwTotalSpace;
   DWORD       dwAvailableClusters;
   DWORD       dwClustersPerDrive;
   DWORD       dwSectorsPerCluster;
   DWORD       dwBytesPerSector;
   BYTE        bDrvID = FsvUtil_ConvrtDrvLtr2ID( lpH->lpDir->fiArchvFile.szFullName[ 0 ] );

   // Try to get the total and available disk space.
   if ( ( eErr = FsvIO_GetDiskFreeSpace( bDrvID, &dwAvailableClusters, &dwClustersPerDrive,
                                         &dwSectorsPerCluster, &dwBytesPerSector, lpH->lpCritCBFunc,
                                         lpH->dwCritCBData ) ) == ZIP_NO_ERROR )
   {
      DWORD dwBytsPerCluster = dwSectorsPerCluster * dwBytesPerSector;

      dwFreeSpace  = dwBytsPerCluster * dwAvailableClusters;
      dwTotalSpace = dwBytsPerCluster * dwClustersPerDrive;

      if ( dwFreeSpace < ( dwTotalSpace * ZIP_PERCENT_FREE / 10 ) )
      // Error, the floppy does not have enough free space.
      {
         eErr = FSVERR_DISKFULL;
      }
      else
      if ( ( eErr = Zip_WriteSpecialLabel( lpH, NULL, wDiskNum ) ) == ZIP_NO_ERROR )
      {
         eErr = FsvBufIO_fCreat( lpH->lpDir->fiArchvFile.szFullName, 0, FSVBUFIO_FILE_BUF_SIZ,
                                 &lpH->lpDir->fiArchvFile.lpFBuf, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData );
      }
   }

   return( eErr );
}
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_PutInNthDiskChk( lpH, lplpFBuf, wDiskNum, eZipOp )
//
//    lpH      - Pointer to info for the handle in question.
//    lplpFBuf - Pointer to the pointer to the structure used to buffer
//               file i/o for an archive file.  If the archive file is
//               closed and re-opened because another diskette had to be
//               inserted then *lplpFBuf will be updated.
//    wDiskNum - The number of disk (0 based) that needs to be inserted.
//    eZipOp   - The ZIP operation (parse zip file, extract compressed
//               file, etc.) that requires a disk to be inserted.
//
// This function, if necessary, will prompt the user to insert the appropriate
// diskette for the specified compressed file that resides in a multi-disk
// archive file.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services
//                   ( i.e. file not found, disk i/o error, etc.)
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and re-open it on another diskette.  If this happens *lplpFBuf (and
//       lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_PutInNthDiskChk(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   WORD                   wDiskNum,
   ZIP_SPLIT_DISK_OP      eZipOp
)

{
#ifdef NAV95
    return ( ZIP_NO_ERROR );    // Lie: NAV95 doesn't support multi-disk
#else
   LPZIP_ARCHIVE_DIR lpDir = lpH->lpDir;
   ZIP_ERRCODE       eErr  = ZIP_NO_ERROR;

   if ( eZipOp == ZIP_OPEN_INSERT_LAST || eZipOp == ZIP_ADD_INSERT_NEXT ||
        ( lpDir->SI.fIsSplit && lpDir->SI.wCurDisk != wDiskNum ) )
   {
      ZIP_SPLTDSK_CBACTION eAction;
      ZIP_SPLTDSK_INFO     sSpltDskInfo = { eZipOp, lpH->dwSpltDskData, wDiskNum + 1 };
      BOOL                 fDone        = FALSE;

      if ( lpH->lpfnSpltDskCB == NULL )
      // This connect to the archive file didn't provide a multi-disk
      // error handler.
      {
         return( ZIP_NO_MULIT_DISK_CB );
      }

      // Close file since diskette it resides on might change.
      FsvBufIO_fClose( lpDir->fiArchvFile.lpFBuf, FALSE, lpH->lpCritCBFunc, lpH->dwCritCBData );
      lpDir->fiArchvFile.lpFBuf = *lplpFBuf = NULL;

      // Loop until new disk inserted that contains archive file
      // or user aborts.
      while ( !fDone )
      {
         // Assume one iteration.
         fDone = TRUE;

         // Let caller put up the dialog to inform the user that the
         // "nth disk" for the archive file needs to be inserted.
         lstrcpy( sSpltDskInfo.szArchiveName, lpDir->fiArchvFile.szFullName );
         eAction = lpH->lpfnSpltDskCB( &sSpltDskInfo );

         // Abort or retry.
         switch ( eAction )
         {
            case ZIP_SPLT_CB_ABORT:
               // Abort (a little slimey).
               eErr = FSVERR_DRIVE_NOT_READY;
            break;

//          case ZIP_SPLT_CB_CONTINUE:
            default:
            {
               WORD wLblDskNum;

               if ( eZipOp == ZIP_ADD_INSERT_NEXT )
               // Try to create the next archive file.
               {
                  fDone = ( Zip_CreateNextArchiveFile( lpH, wDiskNum ) == ZIP_NO_ERROR );
               }
               else
               // Try to re-open the multi-disk archive file.
               {
                  fDone = ( Zip_DoesDiskHaveSpecialLabel( lpH, NULL, &wLblDskNum ) &&
                            ( wLblDskNum == wDiskNum || eZipOp == ZIP_OPEN_INSERT_LAST ) &&
                            FsvBufIO_fOpen( lpDir->fiArchvFile.szFullName, Zip_TransOpenOptns( lpH->wOpenOptns ),
                                            FSVBUFIO_FILE_BUF_SIZ, &lpDir->fiArchvFile.lpFBuf,
                                            lpH->lpCritCBFunc, lpH->dwCritCBData ) == FSV_SUCCESS );
               }

               if ( fDone )
               // Success.  Keep track of the current diskette.
               {
                  //-****Don't do if ZIP_ADD_INSERT_NEXT and zero length zip file.
                  lpDir->SI.wCurDisk = wDiskNum;
               }

               // Keep this pointer current.
               *lplpFBuf = lpDir->fiArchvFile.lpFBuf;
            }
            break;
         }
      }
   }

   return( eErr );
#endif          // NOT NAV95
}


/*---------------------------------------------------------*/
/*   A R C H I V E   F I L E   R E A D   R O U T I N E S   */
/*---------------------------------------------------------*/


//==========================================================================
//
// ZIP_ERRCODE Zip_fReadByts( lpH, lpFBuf, lpBuf, wBytsToRead )
//
//    lpH         - Pointer to the information structure used to process a
//                  connection to an archive file.
//    lpFBuf      - Pointer to the structure used to buffer file i/o.
//    lpBuf       - Pointer to the buffer that will contain the data copied
//                  from the i/o buffer and/or disk file.
//    wBytsToRead - The number of bytes to read.
//
// This function calls FsvBufIO_fRead() to read the data in question.  This
// function will return an error if not all of the requested bytes were
// read.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - The end of file was prematurely detected which means
//                    the archive file is in an unstable state.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_fReadByts(
   LPZIP_HANDLE_INFO    lpH,
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LPVOID               lpBuf,
   WORD                 wBytsToRead
)

{
   ZIP_ERRCODE eErr;

   if ( ( eErr = FsvBufIO_fRead( lpFBuf, lpBuf, wBytsToRead, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) != ( LONG ) wBytsToRead )
   /* Bads news, unable to read the file or all of the data. */
   {
      if ( eErr >= 0 )
      /* The end of file was detected. */
      {
         eErr = ZIP_DIR_ERROR;
      }
   }
   else
   /* Clear the error return. */
   {
      eErr = ZIP_NO_ERROR;
   }

   return( eErr );
} /* Zip_fReadByts */



//==========================================================================
//
// ZIP_ERRCODE Zip_fArchiveRead( lpH, lplpFBuf, lpBuf, wBytsToRead )
//
//    lpH         - Pointer to the information structure used to process a
//                  connection to an archive file.
//    lplpFBuf    - Pointer to the pointer to the structure used to buffer
//                  file i/o for an archive file.  If the archive file is
//                  closed and re-opened because another diskette had to be
//                  inserted then *lplpFBuf will be updated.
//    lpBuf       - Pointer to the buffer that will contain the data copied
//                  from the archive file.
//    wBytsToRead - The number of bytes to read.
//
// This function calls FsvBufIO_fRead() to read the data in question.  If not
// all of the bytes were read and a multi-disk archive file is being read (and
// we are not reading from the last disk in the sequence) then the user will
// be prompted to insert another diskette.
//
// Returns:
//    <bytes read> - The number of bytes that were read.  Can be less than
//                   wBytsToRead if the end of file was detected.
//    FSVERR...    - The appropriate error code returned by File Services
//                   ( i.e. file not found, disk i/o error, etc.)
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and re-open it on another diskette.  If this happens *lplpFBuf (and
//       lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_fArchiveRead(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   LPVOID                 lpBuf,
   WORD                   wBytsToRead
)

{
   ZIP_ERRCODE eErr;
   ZIP_ERRCODE lOtherByts = 0;

ReadData:
   if ( ( eErr = FsvBufIO_fRead( *lplpFBuf, lpBuf, wBytsToRead, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) == ( LONG ) wBytsToRead )
   // Account for bytes read from the previous floppy.
   {
      eErr += lOtherByts;
   }
   else
   if ( eErr >= 0 )
   // Unable to read all of the data.
   {
      LPZIP_ARCHIVE_DIR lpDir = lpH->lpDir;

      if ( lpDir->SI.fIsSplit && lpDir->SI.wCurDisk != lpDir->SI.wLastDisk )
      // The user needs to insert the next diskette.
      {
         WORD wByts = ( WORD ) eErr;

         if ( ( eErr = Zip_PutInNthDiskChk( lpH, lplpFBuf, (WORD)(lpDir->SI.wCurDisk + 1), ZIP_EXTRACT_PACK_NTH ) ) == ZIP_NO_ERROR )
         {
            ( LPBYTE ) lpBuf += wByts;
            wBytsToRead      -= wByts;
            lOtherByts       += wByts;
            goto ReadData;
         }
      }
   }

   return( eErr );
} // Zip_fArchiveRead



//==========================================================================
//
// ZIP_ERRCODE Zip_fArchiveReadByts( lpH, lpFBuf, lpBuf, wBytsToRead )
//
//    lpH         - Pointer to the information structure used to process a
//                  connection to an archive file.
//    lplpFBuf    - Pointer to the pointer to the structure used to buffer
//                  file i/o for an archive file.  If the archive file is
//                  closed and re-opened because another diskette had to be
//                  inserted then *lplpFBuf will be updated.
//    lpBuf       - Pointer to the buffer that will contain the data copied
//                  from the archive file.
//    wBytsToRead - The number of bytes to read.
//
// This function calls FsvBufIO_fRead() to read the data in question.  If not
// all of the bytes were read and a multi-disk archive file is being read (and
// we are not reading from the last disk in the sequence) then the user will
// be prompted to insert another diskette.
//
// This function will return an error if not all of the requested bytes were
// read.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - The end of file was prematurely detected which means
//                    the archive file is in an unstable state.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and re-open it on another diskette.  If this happens *lplpFBuf (and
//       lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_fArchiveReadByts(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   LPVOID                 lpBuf,
   WORD                   wBytsToRead
)

{
   ZIP_ERRCODE eErr;

ReadData:
   if ( ( eErr = FsvBufIO_fRead( *lplpFBuf, lpBuf, wBytsToRead, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) == ( LONG ) wBytsToRead )
   // Account for bytes read from the previous floppy.
   {
      eErr = ZIP_NO_ERROR;
   }
   else
   if ( eErr >= 0 )
   // Unable to read all of the data.
   {
      LPZIP_ARCHIVE_DIR lpDir = lpH->lpDir;

      if ( lpDir->SI.fIsSplit && lpDir->SI.wCurDisk != lpDir->SI.wLastDisk )
      // The user needs to insert the next diskette.
      {
         WORD wByts = ( WORD ) eErr;

         if ( ( eErr = Zip_PutInNthDiskChk( lpH, lplpFBuf, (WORD)(lpDir->SI.wCurDisk + 1), ZIP_EXTRACT_PACK_NTH ) ) == ZIP_NO_ERROR )
         {
            ( LPBYTE ) lpBuf += wByts;
            wBytsToRead      -= wByts;
            goto ReadData;
         }
      }
      else
      // The end of file was detected.
      {
         eErr = ZIP_DIR_ERROR;
      }
   }

   return( eErr );
} // Zip_fArchiveReadByts



//==========================================================================
//
// ZIP_ERRCODE Zip_fArchiveWrt( lpH, lplpFBuf, lpBuf, wBytsToWrt )
//
//    lpH        - Pointer to the information structure used to process a
//                 connection to an archive file.
//    lplpFBuf   - Pointer to the pointer to the structure used to buffer
//                 file i/o for an archive file.  If the archive file is
//                 closed and re-opened because another diskette had to be
//                 inserted then *lplpFBuf will be updated.
//    lpBuf      - Pointer to the buffer that contains the data to copy
//                 to the archive file.
//    wBytsToWrt - The number of bytes to write.
//
// This function calls FsvBufIO_fWrite() to write the data in question.  If
// not all of the bytes were written and a multi-disk archive file is being
// "processed" then the user will be prompted to insert another diskette.
//
// Returns:
//    <bytes written> - The number of bytes that were written.  Can be less
//                      than wBytsToWrt if there is no more disk space.
//    FSVERR...       - The appropriate error code returned by File Services
//                      ( i.e. file not found, disk i/o error, etc.)
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and create a new one on another diskette.  If this happens *lplpFBuf
//       (and lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//    2) If the archive file only resides on one diskette the user will be
//       given the option to create a multi-disk archive file.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_fArchiveWrt(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   LPVOID                 lpBuf,
   WORD                   wBytsToWrt
)

{
   ZIP_ERRCODE eErr       = 0;
   ZIP_ERRCODE lTotalByts = 0;

   if ( lpH->lpDir->SI.fCanBeSplit )
   {
      // Loop to write out the specified data.
      while ( wBytsToWrt != 0 )
      {
         WORD wByts;
         WORD wBytsInBuf = 0;

         // Determine maximum bytes that can be stored in the buffer.
         wBytsInBuf = FsvBufIO_BufBytsAvail( *lplpFBuf );
         wByts      = min( wBytsToWrt, wBytsInBuf );

         //********
         // NOTE: If we don't do this, we will end up in an infinite loop.
         //********
         if (wByts == 0)
            wByts = wBytsToWrt;

         // Write bytes to the buffer only.
         FsvBufIO_fWrite( *lplpFBuf, lpBuf, wByts, NULL, 0 );

         // Keep track of how many bytes have been written.
         ( LPBYTE ) lpBuf += wByts;
         wBytsToWrt       -= wByts;
         lTotalByts       += wByts;

         // Manually flush the i/o buffer.
         if ( wBytsToWrt > 0 && ( eErr = Zip_fFlushBuf( lpH, lplpFBuf ) ) != ZIP_NO_ERROR )
         {
            return( eErr );
         }

         // Keep track of total bytes written.
         eErr = lTotalByts;
      }
   }
   else
   // Try to write the data out to disk.
   {
      eErr = FsvBufIO_fWrite( *lplpFBuf, lpBuf, wBytsToWrt, lpH->lpCritCBFunc,
                              lpH->dwCritCBData );
   }

   return( eErr );
} // Zip_fArchiveWrt
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_fFlushBuf( lpH, lplpFBuf )
//
//    lpH      - Pointer to the information structure used to process a
//               connection to an archive file.
//    lplpFBuf - Pointer to the pointer to the structure used to buffer
//               file i/o for an archive file.  If the archive file is
//               closed and re-opened because another diskette had to be
//               inserted then *lplpFBuf will be updated.
//
// This function manually flushes the i/o buffer.  This is done in case a
// multi-disk archive file is being constructed and the user needs to insert
// a new diskette because the buffer cannot fit on the current disk.
//
// Returns:
//    FSV_SUCCESS - The low level i/o buffer was successfully written out
//                  to one or more diskettes.
//    FSVERR...   - The appropriate error code returned by File Services
//                  ( i.e. file not found, disk i/o error, etc.)
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and create a new one on another diskette.  If this happens *lplpFBuf
//       (and lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//    2) If the archive file only resides on one diskette the user will be
//       given the option to create a multi-disk archive file.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_fFlushBuf(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf
)

{
#ifdef NAV95
    return ZIP_NO_ERROR;                // Kludge: Since we don't support
                                        // writing ZIP's, there should be
                                        // nothing to flush. This is still
                                        // called by Zip_fArchiveSeek
#else
   ZIP_ERRCODE       eErr      = ZIP_NO_ERROR;
   LPBYTE            lpTmpBuf  = NULL;
   LPBYTE            lpIOBuf   = FsvBufIO_GetIOBufPtr( *lplpFBuf );
   WORD              wIOBufSiz = FsvBufIO_GetIOBufBytsUsed( *lplpFBuf );
   LPZIP_ARCHIVE_DIR lpDir     = lpH->lpDir;

   if ( *lplpFBuf == NULL )
   // Not good, can happen when user aborts the creation of a multi-disk
   // archive file.
   {
      return( FSV_ERROR );
   }

   if ( !FsvBufIO_GetDirtyFlag( *lplpFBuf ) )
   // I/O buffer not dirty.
   {
      // Evil.  Manually update the file position and reset the
      // appropriate fields to indicate that the i/o buffer is empty.
      ( *lplpFBuf )->lStrtBufPos += ( *lplpFBuf )->wBytsUsed;
      ( *lplpFBuf )->wBytsUsed    = 0;
      ( *lplpFBuf )->wBufOffset   = 0;

      return( ZIP_NO_ERROR );
   }

#define DEBUG_ZIP 0
#if DEBUG_ZIP
   {
      WORD  cw;
      for (cw = 0; cw < wIOBufSiz; cw++)
      {
         if (_fmemicmp( &(lpIOBuf[cw]), "wnvf3", 5 ) == 0)
         {
            DebugBreak();
            break;
         }
      }
   }
#endif

   // Loop to flush the i/o buffer to multiple diskettes.
   while ( wIOBufSiz > 0 )
   {
      WORD wByts;

      if ( ( eErr = FsvIO_Seek( FsvBufIO_GetFileHandle( *lplpFBuf ), ( *lplpFBuf )->lStrtBufPos, SEEK_SET,
                                lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == ( *lplpFBuf )->lStrtBufPos )
      // Try to write the buffer data to disk.
      {
         //************************
         // KLUGE ALERT!!!!
         //************************
         // UNKNOWN BUG:
         // This do~while necessary because with WFW3.11, 32BFA, DOS 6.22, the FsvIO_Write function
         // doesn't write any bytes if the request to write is greater than space left on floppy.
         //
         // FIX:
         // Halve the size of each write until even a single byte cannot be written.
         //************************
         WORD  wToWrite = wIOBufSiz;
         WORD  cwWritten = 0;
         do
         {
            eErr  = FsvIO_Write( FsvBufIO_GetFileHandle( *lplpFBuf ), &(lpIOBuf[cwWritten]), wToWrite,
                                 lpH->lpCritCBFunc, lpH->dwCritCBData );
            if (eErr < 0)
               break;
            cwWritten += (WORD)eErr;
            if (eErr == 0)
               wToWrite /= 2;
         } while ( ((eErr == 0) || (wToWrite != wIOBufSiz)) && (wToWrite > 0) );
         wByts = cwWritten;
      }

      if ( eErr < 0 )
      // Bad news.  Low level disk error.
      {
         break;
      }

      if ( lpTmpBuf != NULL )
      // Evil.  Manually update the EOF position.
      {
         ( *lplpFBuf )->lEOFPos += wByts;
      }

      // Evil.  Manually update the file position and reset the
      // appropriate fields to indicate that the i/o buffer is empty.
      ( *lplpFBuf )->wBytsUsed    = 0;
      ( *lplpFBuf )->wBufOffset   = 0;
      ( *lplpFBuf )->lStrtBufPos += wByts;

      // Reset the i/o buffer "dirty" flag.
      FsvBufIO_SetDirtyFlag( *lplpFBuf, FALSE );

      if ( wByts == wIOBufSiz )
      // Good.  Buffer successfully written.
      {
         eErr = FSV_SUCCESS;
         break;
      }

      // JJS. 05/22/94. Make sure this is the physical end of file.  This is
      // necessary becuase of a DOS bug.
      FsvBufIO_fSetEOF( *lplpFBuf, ( *lplpFBuf )->lStrtBufPos, lpH->lpCritCBFunc, lpH->dwCritCBData );

      // Keep track of how much of the data buffer is left to write.
      lpIOBuf   += wByts;
      wIOBufSiz -= wByts;

      if ( !lpDir->SI.fIsSplit )
      {
         TCHAR                 szLabel[ FSV_DRIVELABELBUF_LEN + 1 ];
         TCHAR                 szCurLabel[ FSV_DRIVELABELBUF_LEN + 1 ];
         TCHAR                 szDiskID[ FSV_DRIVELABELBUF_LEN + 2 ];
         ZIP_SPLTDSK_CBACTION eAction;
         LPTSTR                lpszTmp;
         ZIP_SPLTDSK_INFO     sSpltDskInfo = { ZIP_ADD_CREATE_MULTI_QUERY, lpH->dwSpltDskData,  1 };
         BYTE                 bDrv         = FsvUtil_ConvrtDrvLtr2ID( lpDir->fiArchvFile.szFullName[ 0 ] );
         BOOL                 fDone        = FALSE;

         // Get the current label.
         if ( ( eErr = FsvIO_GetDriveLabel( bDrv, szLabel, sizeof( szLabel ), lpH->lpCritCBFunc, lpH->dwCritCBData ) ) != FSV_SUCCESS )
         // Can't get the current label so return disk full error.
         {
            lpDir->SI.fCanBeSplit = FALSE;
            eErr                  = ( FsvIO_IsCritErr( eErr ) ) ? eErr : FSVERR_DISKFULL;
            break;
         }

         // Store file name as label to uniquely identify the disk.
         {
            TCHAR szShortName[FSV_PATHBUF_LEN];
            GetShortPathName( lpDir->fiArchvFile.szFullName, szShortName, sizeof(szShortName) );
            FsvUtil_GetPathMask( szShortName, NULL, szDiskID, TRUE );
            szDiskID[FSV_DIRENTRYBUF_LEN] = 0;
         }

         if ( ( lpszTmp = _fstrchr( szDiskID, '.' ) ) != NULL )
         // The function FsvIO_SetDriveLabel() does not like '.' so
         // replace it with the "#" character.
         {
            lpszTmp[ 0 ] = '#';
         }

         // Write out the label used to identify the diskette.
         if ( ( eErr = FsvIO_SetDriveLabel( bDrv, szDiskID, lpH->lpCritCBFunc,
                                            lpH->dwCritCBData ) ) != FSV_SUCCESS )
         // Can't write special ID label so return disk full error.
         {
            lpDir->SI.fCanBeSplit = FALSE;
            eErr                  = ( FsvIO_IsCritErr( eErr ) ) ? eErr : FSVERR_DISKFULL;
            break;
         }

         // Prompt user if they want to create a multi-disk archive file.
         lstrcpy( sSpltDskInfo.szArchiveName, lpDir->fiArchvFile.szFullName );
         eAction = lpH->lpfnSpltDskCB( &sSpltDskInfo );

         // Loop until disk error or original diskette is detected.
         while ( !fDone )
         {
            // JJS.  It seems that I need to call this so that
            // FsvIO_GetDriveLabel() will work correctly.  James!  Your
            // bogus function doesn't work!
//          FsvIO_ResetFloppyDrive( bDrv );

            // I check for an error return of -34 because FsvIO_GetDriveLabel()
            // fails if the diskette has been switche (DOS sucks!!)
            if ( ( eErr = FsvIO_GetDriveLabel( bDrv, szCurLabel, sizeof( szCurLabel ), lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == -34L ||
                 ( eErr == FSV_SUCCESS && lstrcmpi( szCurLabel, szDiskID ) != 0 ) )
            // Inform user to insert the "original" diskette.
            {
               sSpltDskInfo.eZipOp = ZIP_ADD_REINSERT_ORIGINAL;
               eAction = lpH->lpfnSpltDskCB( &sSpltDskInfo );
               fDone   = ( eAction == ZIP_SPLT_CB_ABORT );
            }
            else
            // Disk error encountered or proper diskette is in the drive.
            {
               fDone = TRUE;
            }
         }

         if ( eErr != FSV_SUCCESS || eAction == ZIP_SPLT_CB_ABORT )
         // Can't get current label or user want's to abort so return
         // the disk full error.
         {
            lpDir->SI.fCanBeSplit = FALSE;
            eErr                  = ( FsvIO_IsCritErr( eErr ) ) ? eErr : FSVERR_DISKFULL;
            break;
         }

         if ( eAction == ZIP_SPLT_CB_ABORT )
         // User does not want to create an archive file so continue as
         // normal (restore the original label).
         {
            FsvIO_SetDriveLabel( bDrv, szLabel, lpH->lpCritCBFunc, lpH->dwCritCBData );
            lpDir->SI.fCanBeSplit = FALSE;
            eErr                  = FSVERR_DISKFULL;
            break;
         }
         else
         // Try to write out the volume label that will indicate that
         // this is the 1st diskette in the multi-disk archive file.
         if ( ( eErr = Zip_WriteSpecialLabel( lpH, NULL, 0 ) ) != ZIP_NO_ERROR )
         {
            lpDir->SI.fCanBeSplit = FALSE;
            break;
         }
      }

      if ( lpTmpBuf == NULL )
      // Allocate a buffer to hold the "remains" of the i/o buffer since the
      // i/o buffer will be freed after Zip_PutInNthDiskChk() is called.
      {
         if ( ( lpTmpBuf = ( LPBYTE ) Mem_SubMalloc( wIOBufSiz ) ) == NULL )
         // Bad news.  No more memory.
         {
            eErr = FSVERR_OUT_OF_MEMORY;
            break;
         }

         // Store contents of low level i/o buffer.
         _fmemcpy( lpTmpBuf, lpIOBuf, wIOBufSiz );
         lpIOBuf = lpTmpBuf;
      }

      // Prompt user to insert the next diskette.
      if ( ( eErr = Zip_PutInNthDiskChk( lpH, lplpFBuf, (WORD)(lpDir->SI.wCurDisk + 1), ( lpDir->SI.wCurDisk == lpDir->SI.wLastDisk ) ? ZIP_ADD_INSERT_NEXT : ZIP_ADD_INSERT_NTH ) ) != ZIP_NO_ERROR )
      {
         FsvBufIO_SetDirtyFlag( *lplpFBuf, TRUE );
         break;
      }

      // Keep track of state info for the current disk.
      lpDir->SI.fIsSplit  = TRUE;
      lpDir->SI.wStrtDisk = lpDir->SI.wCurDisk;
      lpDir->SI.wLastDisk = lpDir->SI.wCurDisk;
   }

   if ( lpTmpBuf != NULL )
   // Free buffer used to write the i/o buffer to multiple disks.
   {
      Mem_SubFree( lpTmpBuf );
   }

   return( eErr );
#endif          // NOT NAV95
} // Zip_fFlushBuf


//==========================================================================
//
// ZIP_ERRCODE Zip_fArchiveSeek( lpH, lplpFBuf, wDiskNum, lOffset, nSeekOp )
//
//    lpH      - Pointer to the information structure used to process a
//               connection to an archive file.
//    lplpFBuf - Pointer to the pointer to the structure used to buffer
//               file i/o for an archive file.  If the archive file is
//               closed and re-opened because another diskette had to be
//               inserted then *lplpFBuf will be updated.
//    wDiskNum - The number of the diskette that contains the desired
//               portion of the file.
//    lOffset  - The offset to add to the beggining of file, current
//               file position, or the end of the file.
//    nSeekOp  - The type of seek operation to perform (seek from
//               beginning of file, seek from current file position,
//               seek to the end of the file).
//
// This function will 1st check to see if the appropriate diskette is
// inserted in the drive (only applies if we are dealing with a multi-disk
// archive file), and if it isn't, will prompt the user accordingly.  After
// that it will call FsvBufIO_fSeek().
//
// Returns:
//    ZIP_NO_ERROR - All is well.
//    FSVERR...    - The appropriate error code returned by File Services
//                   ( i.e. file not found, disk i/o error, etc.)
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and open a new one on another diskette.  If this happens *lplpFBuf
//       (and lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_fArchiveSeek(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   WORD                   wDiskNum,
   LONG                   lOffset,
   int                    nSeekOp
)

{
   ZIP_ERRCODE eErr = ZIP_NO_ERROR;

   if ( lpH->lpDir->SI.fCanBeSplit )
   {
      if ( wDiskNum != lpH->lpDir->SI.wCurDisk || FsvBufIO_fSeekCausesFlush( *lplpFBuf, lOffset, nSeekOp ) )
      // The seek will cause the i/o buffer to be flushed.
      {
         eErr = Zip_fFlushBuf( lpH, lplpFBuf );
      }
   }

   // Ensure that the proper diskette is inserted.
   if ( eErr == ZIP_NO_ERROR &&
        ( eErr = Zip_PutInNthDiskChk( lpH, lplpFBuf, wDiskNum, ZIP_ADD_INSERT_NTH ) ) == ZIP_NO_ERROR )
   {
      eErr = FsvBufIO_fSeek( *lplpFBuf, lOffset, nSeekOp, lpH->lpCritCBFunc, lpH->dwCritCBData );
   }

   return( eErr );
} // Zip_fArchiveSeek



//==========================================================================
//
// ZIP_ERRCODE Zip_fArchiveAdvance( lpH, lplpFBuf, lOffset )
//
//    lpH      - Pointer to the information structure used to process a
//               connection to an archive file.
//    lplpFBuf - Pointer to the pointer to the structure used to buffer
//               file i/o for an archive file.  If the archive file is
//               closed and re-opened because another diskette had to be
//               inserted then *lplpFBuf will be updated.
//    lOffset  - The offset to add to the current file position.
//
// This function will try to advance the file pointer the specified number
// of bytes.  If the archive file spans multiple diskettes and the offset
// extends beyond the end of the file then the user will be prompted to insert
// the next diskette.
//
// Returns:
//    ZIP_NO_ERROR - All is well.
//    FSVERR...    - The appropriate error code returned by File Services
//                   ( i.e. file not found, disk i/o error, etc.)
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and open a new one on another diskette.  If this happens *lplpFBuf
//       (and lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_fArchiveAdvance(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   LONG                   lOffset
)

{
   ZIP_ERRCODE eErr = ZIP_NO_ERROR;

   if ( lpH->lpDir->SI.fCanBeSplit )
   {
      LONG lBytsBeforeEOF = FsvBufIO_fRtrnEOFPos( *lplpFBuf ) - FsvBufIO_fTell( *lplpFBuf );

      // Loop in case multiple diskettes have to be inserted.
      while ( lOffset >= lBytsBeforeEOF )
      {
         // The seek spans beyond the current end of file so prompt the
         // user to insert the next diskette.
         if ( ( eErr = Zip_PutInNthDiskChk( lpH, lplpFBuf, lpH->lpDir->SI.wCurDisk, ZIP_EXTRACT_INSERT_NTH ) ) != ZIP_NO_ERROR )
         {
            return( eErr );
         }

         lOffset        -= lBytsBeforeEOF;
         lBytsBeforeEOF  = FsvBufIO_fRtrnEOFPos( *lplpFBuf ) - FsvBufIO_fTell( *lplpFBuf );
      }
   }

   //  Now advance the file pointer within the current file.
   eErr = FsvBufIO_fSeek( *lplpFBuf, lOffset, SEEK_CUR, lpH->lpCritCBFunc, lpH->dwCritCBData );

   return( eErr );
} // Zip_fArchiveAdvance



//==========================================================================
//
// ZIP_ERRCODE Zip_fArchiveSetEOF( lpH, lplpFBuf, wDiskNum, lEOFPostn )
//
//    lpH       - Pointer to the information structure used to process a
//                connection to an archive file.
//    lplpFBuf  - Pointer to the pointer to the structure used to buffer
//                file i/o for an archive file.  If the archive file is
//                closed and re-opened because another diskette had to be
//                inserted then *lplpFBuf will be updated.
//    wDiskNum  - The number of the diskette that contains the end of the
//                file.
//    lEOFPostn - The zero-based file position to make the End of File.
//
// This function will 1st check to see if the appropriate diskette is
// inserted in the drive (only applies if we are dealing with a multi-disk
// archive file), and if it isn't, will prompt the user accordingly.  After
// that it will call FsvBufIO_fSetEOF().
//
// Returns:
//    ZIP_NO_ERROR - All is well.
//    FSVERR...    - The appropriate error code returned by File Services
//                   ( i.e. file not found, disk i/o error, etc.)
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and open a new one on another diskette.  If this happens *lplpFBuf
//       (and lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_fArchiveSetEOF(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   WORD                   wDiskNum,
   LONG                   lEOFPostn
)

{
   ZIP_ERRCODE eErr;

   // Ensure that the proper diskette is inserted.
   if ( ( eErr = Zip_PutInNthDiskChk( lpH, lplpFBuf, wDiskNum, ZIP_ADD_INSERT_NTH ) ) == ZIP_NO_ERROR )
   {
      eErr = FsvBufIO_fSetEOF( *lplpFBuf, lEOFPostn, lpH->lpCritCBFunc, lpH->dwCritCBData );
   }

   return( eErr );
} // Zip_fArchiveSetEOF



//==========================================================================
//
// VOID Zip_ReadZipHeader( lpH, lplpInpt, lpFile )
//
//    lpH      - Pointer to the information structure used to process a
//               connection to an archive file.
//    lplpInpt - Pointer to the pointer to the structure used to buffer
//               file i/o for an archive file.  If the archive file is
//               closed and re-opened because another diskette had to be
//               inserted then *lplpInpt will be updated.
//    lpFile   - Pointer to the temporary memory resident structure that will
//               contain the info for the compressed file.
//
// This function extracts the appropriate file info from the header record
// in the central directory, and copies it into a temporary structure that
// resides in memory.
//
// Returns:
//    ZIP_NO_ERROR  - The memory copy of the ZIP file info was valid and was
//                    successfully copied from disk.
//    ZIP_DIR_ERROR - The data stored in the central directory for the file
//                    in question is invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and re-open it on another diskette.  If this happens *lplpInpt (and
//       lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_ReadZipHeader(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpInpt,
   LPZIP_COMP_INFO        lpFile
)

{
   ZIP_CDIR_REC      ZipCDirRec;
   TCHAR              szFullName[ FSV_PATHBUF_LEN ];
   TCHAR              szName[ FSV_LFILENAMEBUF_LEN ];
   TCHAR              szPath[ FSV_PATHBUF_LEN ];
   LPTSTR             lpszTmp;
   ZIP_ERRCODE       eErr;
   LPZIP_ARCHIVE_DIR lpDir = lpH->lpDir;

   if ( ( eErr = Zip_fArchiveReadByts( lpH, lplpInpt, &ZipCDirRec.bProgMadeVersn,
                                       sizeof( ZipCDirRec ) - sizeof( LONG ) ) ) != ZIP_NO_ERROR )
   /* Bads news, unable to read the directory header. */
   {
      return( eErr );
   }

   // Copy key data from disk into the memory copy.
   lpFile->bEncrypted     = ( BYTE ) ZipCDirRec.shData.fEncrypted;
   lpFile->lCompHeadrStrt = ZipCDirRec.lCompHeadrStrt;
   lpFile->lOrigSiz       = ZipCDirRec.shData.lOrigSiz;
   lpFile->tiTime         = ZipCDirRec.shData.tiTime;
   lpFile->diDate         = ZipCDirRec.shData.diDate;
   lpFile->wAttrs         = ( ZipCDirRec.bOSTyp ) != ZIP_MSDOS ?
                               0 :
                               ( WORD ) ( ZipCDirRec.lExtAttr & ( FSV_ATTRIB_READONLY | FSV_ATTRIB_HIDDEN |
                                                                  FSV_ATTRIB_SYSTEM | FSV_ATTRIB_ARCHIVE |
                                                                  FSV_ATTRIB_DIRECTORY ) );
   lpFile->lCRC           = ZipCDirRec.shData.lCRC;
   lpFile->wDiskNum       = ZipCDirRec.wDiskNum;

   // Keep track of the size of the compressed data (location will be
   // determined by processing the local header).
   lpFile->dbComprssdFile.lSize     = ZipCDirRec.shData.lComprsdSiz;
   lpFile->dbComprssdFile.hbFleIndx = ZIP_ARCHV_HANDLE;

   // Translate the compression type.
   switch ( ZipCDirRec.shData.wPackTyp )
   {
      case 0:
         lpFile->ePckTyp = ZIP_NONE;
      break;

      case 1:
         lpFile->ePckTyp = ZIP_SHRINK;
      break;

      case 2:
      case 3:
      case 4:
      case 5:
         lpFile->ePckTyp  = ZIP_REDUCED;
         lpFile->wPckFlgs = ZipCDirRec.shData.wPackTyp - 2;
      break;

      case 6:
         lpFile->ePckTyp = ZIP_IMPLODE;

         if ( ZipCDirRec.shData.fEightKWindow )
         {
            lpFile->wPckFlgs |= ZIP_8K_WINDOW;
         }

         if ( ZipCDirRec.shData.fCharsEncoded )
         {
            lpFile->wPckFlgs |= ZIP_CHARS_ENCODED;
         }
      break;

      case 7:
         lpFile->ePckTyp = ZIP_TOKENIZED;
      break;

      case 8:
         lpFile->ePckTyp = ZIP_DEFLATN;

         if ( ZipCDirRec.shData.fEightKWindow )
         {
            lpFile->wPckFlgs |= ZIP_8K_WINDOW;
         }

         if ( ZipCDirRec.shData.fCharsEncoded )
         {
            lpFile->wPckFlgs |= ZIP_CHARS_ENCODED;
         }
      break;

      /* This is allowed, but an error will be generated if a */
      /* call is made to "extract" this file.                 */
      default:
         lpFile->ePckTyp = ZIP_UNKNOWN;
      break;
   }

   if ( ZipCDirRec.shData.wNameSiz >= FSV_PATHBUF_LEN )
   /* Bad news.  The path/file name is too long. */
   {
      return( ZIP_DIR_ERROR );
   }

   /* Null terminate the recipient file name string. */
   szFullName[ZipCDirRec.shData.wNameSiz] = 0;

   /* Read the file name (can include the path). */
   if ( ( eErr = Zip_fArchiveReadByts( lpH, lplpInpt, szFullName, ZipCDirRec.shData.wNameSiz ) ) != ZIP_NO_ERROR )
   {
      return( eErr );
   }

   /* Make the directory node separators compatible with DOS. */
   for ( lpszTmp = szFullName; *lpszTmp != '\0'; lpszTmp = CharNext(lpszTmp) )
   {
      if ( *lpszTmp == '/' )
      {
         *lpszTmp = '\\';
      }
   }

   if ( !Zip_ConvertOtherPlatformFileName( szFullName, ZipCDirRec.shData.bExtractOSTyp, &lpFile->bDirty ) )
   // Bad news.  The path and/or the file name is bogus.
   {
      return( ZIP_DIR_ERROR );
   }

   /* Separate the file name and the path. */
   FsvUtil_GetPathMask( szFullName, szPath, szName, FALSE );

   /* Make local copy of the file name. */
   lstrcpy( lpFile->szName, szName );

   if ( lstrlen(szPath) > 0 )
   /* Try to dynamically allocate the path. */
   {
      if ( ( lpFile->lpszPath = ( LPTSTR ) Mem_SubMalloc( FsvStr_GetByteCount( szPath ) + 1 ) ) == NULL )
      /* Bad news, unable to allocate the path. */
      {
         return( ZIP_NO_MEMORY );
      }
      else
      {
         lstrcpy( lpFile->lpszPath, szPath );
      }
   }

   // Keep track of the size and the location of the "extra data".
   // ** Actually I use the info stored with the compressed file.  The reason
   // ** for this is ...  Send cash or money order to JJS to find out why.
// lpFile->dbExtra1.lSize     = ZipCDirRec.shData.wExtraSiz;
// lpFile->dbExtra1.lStart    = FsvBufIO_fTell( lpInpt ) + ZipCDirRec.shData.wNameSiz;
// lpFile->dbExtra1.wDiskNum  = lpDir->SI.wCurDisk;
// lpFile->dbExtra1.hbFleIndx = ZIP_ARCHV_HANDLE;

   // I have to do a seek because the archive file can span multiple
   // floppies.
   if ( ( eErr = Zip_fArchiveAdvance( lpH, lplpInpt, ZipCDirRec.shData.wExtraSiz ) ) < 0 )
   // Bad news.  File i/o error detected
   {
      return( eErr );
   }

// // Keep track of the size and the location of the "remark data"
// // for the compressed file.
// lpFile->dbRemark.lSize     = ZipCDirRec.wRmrkSiz;
// lpFile->dbRemark.lStart    = eErr;
// lpFile->dbRemark.wDiskNum  = lpDir->SI.wCurDisk;
// lpFile->dbRemark.hbFleIndx = ZIP_ARCHV_HANDLE;

   if ( ZipCDirRec.wRmrkSiz != 0 )
   // Try to store the "remark" in memory.
   {
      if ( ( lpFile->lpszRemark = ( LPTSTR ) Mem_SubMalloc( ZipCDirRec.wRmrkSiz + 1 ) ) == NULL )
      // Bad news, unable to allocate the remark.
      {
         return( ZIP_NO_MEMORY );
      }

      // Null terminate the "remark".
      (lpFile->lpszRemark)[ZipCDirRec.wRmrkSiz] = 0;

      if ( ( eErr = Zip_fArchiveReadByts( lpH, lplpInpt, lpFile->lpszRemark,
                                          ZipCDirRec.wRmrkSiz ) ) != ZIP_NO_ERROR )
      // Bads news, unable to read the "remark".
      {
         return( eErr );
      }
   }

// // Keep track of the number of bytes required for the "remark data".
// lpDir->SI.lExtraRemByts += ( /*ZipCDirRec.shData.wExtraSiz +*/ ZipCDirRec.wRmrkSiz );

   return( ZIP_NO_ERROR );
} // Zip_ReadZipHeader



//==========================================================================
//
// ZIP_ERRCODE Zip_ReadZipDir( lpH )
//
//    lpH - Pointer to the information structure used to process a
//          connection to an archive file.
//
// This function tries to parse the "central directory" in the ZIP archive
// file.  This function will first try to read the trailer record (works
// great if a comment for the ZIP file hasn't been stored) from the end
// of the ZIP file to determine the start of the central directory.  If
// that doesn't work then it start parsing from the beginning of the ZIP
// file and skip over the compressed file until it reaches the central
// directory.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - The data stored in the central directory or in compressed
//                    file headers is invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_ReadZipDir(
   LPZIP_HANDLE_INFO lpH
)

{
   #define ZIP_MAXBACKSCAN_BYTS 600

   ZIP_ZIP_END_REC      ZipEOFRec;
   WORD                 wPassCount;
   ZIP_ERRCODE          eErr;
   BOOL                 bDirParsed  = FALSE;
   LPZIP_ARCHIVE_DIR    lpDir       = lpH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpFBuf      = lpDir->fiArchvFile.lpFBuf;
   LONG                 lStartPos   = FsvBufIO_fTell( lpFBuf );

   /* The archive file appears to be a ZIP file. */
   lpDir->eArchvTyp = ZIP_FORMAT_ZIP;

   /* At most try two different parsing methods. */
   for ( wPassCount = 1; wPassCount <= 2 && !bDirParsed; wPassCount++ )
   {
      LONG            lSignature;
      LPZIP_COMP_INFO lpCmpFle;

      /* Assume the pass will be successful. */
      eErr = ZIP_NO_ERROR;

      /* Do the "optimized" or the "slow" parse. */
      switch ( wPassCount )
      {
InitialParseAttempt:
         /* Try to do the fast parse.  Get the start of the central */
         /* directory from the zip "end of file" record.            */
         case 1:
         {
            LONG lFileSiz = FsvBufIO_fRtrnEOFPos( lpFBuf );

            if ( lFileSiz < sizeof( ZipEOFRec ) )
            /* The ZIP file must contain this record. */
            {
               return( ZIP_DIR_ERROR );
            }

            /* Try to point to the start of end of file record and */
            /* read the zip "end of file" record,                  */
            if ( ( eErr = FsvBufIO_fSeek( lpFBuf, 0L - ( LONG ) sizeof( ZipEOFRec ),
                                          SEEK_END, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) < 0 ||
                 ( eErr = Zip_fReadByts( lpH, lpFBuf, &ZipEOFRec, sizeof( ZipEOFRec ) ) ) != ZIP_NO_ERROR )
            /* Bad news, unable to access the file. */
            {
               return( eErr );
            }

            if ( ZipEOFRec.lEndSignature != ZIP_END_SIGNATURE )
            /* The trailer record was probably followed by a "comment"    */
            /* record so scan backwards a maximum of ZIP_MAXBACKSCAN_BYTS */
            /* for the end of file record signature.                      */
            {
               WORD   wByts2Read;
               LPBYTE lpbTmp;
               BYTE   bBuf[ ZIP_MAXBACKSCAN_BYTS ];

               /* Try to point to the start of the bend of file record and */
               /* read the zip "end of file" record,                  */
               if ( ( eErr = FsvBufIO_fSeek( lpFBuf, 0L - ( LONG ) ( ZIP_MAXBACKSCAN_BYTS ),
                                             SEEK_END, lpH->lpCritCBFunc, lpH->dwCritCBData  ) ) < 0 )
               {
                  return( eErr );
               }

               /* Determine amount of data to read. */
               wByts2Read = min( ( WORD ) ( lFileSiz - eErr ), ZIP_MAXBACKSCAN_BYTS );

               if ( ( eErr = Zip_fReadByts( lpH, lpFBuf, bBuf, wByts2Read ) ) != ZIP_NO_ERROR )
               /* Bad news, unable to access the file. */
               {
                  return( eErr );
               }

               /* Scan for the end of file record signature. */
               for( lpbTmp = &bBuf[ wByts2Read - sizeof( LONG ) ];
                    lpbTmp >= &bBuf[ 0 ];
                    lpbTmp-- )
               {
                  if ( *( ( LPLONG ) lpbTmp ) == ZIP_END_SIGNATURE )
                  /* The end of file signature was encountered. */
                  {
                     _fmemcpy( &ZipEOFRec, lpbTmp, sizeof( ZipEOFRec ) );
                     break;
                  }
               }
            }

            if ( ZipEOFRec.lEndSignature != ZIP_END_SIGNATURE )
            /* The signature was not found so scan from the beginning of */
            /* the file.                                                 */
            {
               eErr = ZIP_DIR_ERROR;
               ZipEOFRec.lCntrlDirStart = 0;
            }
            else
            /* Move file pointer to the start of the "central directory". */
            if ( ( eErr = FsvBufIO_fSeek( lpFBuf, ZipEOFRec.lCntrlDirStart, SEEK_SET,
                                          lpH->lpCritCBFunc, lpH->dwCritCBData  ) ) != ZipEOFRec.lCntrlDirStart ||
                 ( eErr = Zip_fReadByts( lpH, lpFBuf, &lSignature, sizeof( lSignature ) ) ) != ZIP_NO_ERROR ||
                 ( lSignature != ZIP_DIR_SIGNATURE &&
                   lSignature != ZIP_END_SIGNATURE ) )
            /* Unable to move to the start of the central directory so */
            /* parse the entire file.                                  */
            {
               eErr = ZIP_DIR_ERROR;
            }
            else
            // If the Central Directory is on another diskette prompt the
            // user accordingly.
            {
               lpDir->SI.wCurDisk = ZipEOFRec.wDiskNum;
               eErr               = Zip_PutInNthDiskChk( lpH, &lpFBuf, ZipEOFRec.wStartDisk, ZIP_OPEN_INSERT_NTH );
            }

            if ( eErr != ZIP_NO_ERROR && lpDir->SI.fCanBeSplit && !FsvIO_IsCritErr( eErr ) )
            {
               if ( lpH->lpfnSpltDskCB == NULL || !Zip_DoesDiskHaveSpecialLabel( lpH, NULL, NULL ) )
               // No callback function or the floppy does NOT contain the
               // magic volume label.
               {
                  lpDir->SI.fCanBeSplit = FALSE;
               }
               else
               // The floppy contains the magic volume label so prompt the
               // user to insert the last disk of the sequence.
               {
                  // Prompt user to insert the last disk of the sequence.
                  eErr = Zip_PutInNthDiskChk( lpH, &lpFBuf, 0xFFFF, ZIP_OPEN_INSERT_LAST );

                  if ( eErr != ZIP_NO_ERROR )
                  // Abort (a little slimey).
                  {
                     wPassCount = 2;
                  }
                  else
                  // Once again, look for the Central Directory.
                  {
                     goto InitialParseAttempt;
                  }
               }
            }
         }
         break;

         /* Parse the entire file. */
         case 2:
         {
            ZIP_SHARED_HEADR fhZipFileHeadr;

            /* Reset file position to the first file record. */
            if ( ( eErr = FsvBufIO_fSeek( lpFBuf, lStartPos, SEEK_SET, lpH->lpCritCBFunc,
                                          lpH->dwCritCBData  ) ) != lStartPos ||
                 ( eErr = Zip_fReadByts( lpH, lpFBuf, &lSignature, sizeof( lSignature ) ) ) != ZIP_NO_ERROR )
            /* Bad news, unable to access the file. */
            {
               return( eErr );
            }

            /* Scan the entire file for the central directory. */
            while ( lSignature == ZIP_FILE_SIGNATURE )
            {
               if ( ( eErr = Zip_fReadByts( lpH, lpFBuf, &fhZipFileHeadr, sizeof( fhZipFileHeadr ) ) ) != ZIP_NO_ERROR ||
                    ( eErr = FsvBufIO_fSeek( lpFBuf, fhZipFileHeadr.wExtraSiz + fhZipFileHeadr.wNameSiz +
                                             fhZipFileHeadr.lComprsdSiz, SEEK_CUR,
                                             lpH->lpCritCBFunc, lpH->dwCritCBData ) ) < 0 ||
                    ( eErr = Zip_fReadByts( lpH, lpFBuf, &lSignature, sizeof( lSignature ) ) ) != ZIP_NO_ERROR )
               /* Bad news, unable to access the file. */
               {
                  return( eErr );
               }
            }

            if ( ( lSignature != ZIP_DIR_SIGNATURE &&
                   lSignature != ZIP_END_SIGNATURE ) ||
                 FsvBufIO_fTell( lpFBuf ) == ZipEOFRec.lCntrlDirStart )
            /* Invalid ZIP signature or the file poistion for the central */
            /* directory that was extracted in the 1st pass was valid and */
            /* the central directory was found to be invalid.             */
            {
               return( ZIP_DIR_ERROR );
            }
         }
         break;
      }

      /* Now build the memory resident copy of the central directory. */
      while ( lSignature == ZIP_DIR_SIGNATURE && eErr == ZIP_NO_ERROR )
      {
         /* Try to allocate a compressed file info record. */
         if ( ( lpCmpFle = ( LPZIP_COMP_INFO ) Mem_SubMalloc( sizeof( ZIP_COMP_INFO ) ) ) == NULL )
         {
            eErr = ZIP_NO_MEMORY;
         }
         else
         {
            /* Initialize the compressed file info record. */
            Zip_InitCompFileRec( lpCmpFle, FALSE, FALSE, NULL );

            if ( ( eErr = Zip_ReadZipHeader( lpH, &lpFBuf, lpCmpFle ) ) != ZIP_NO_ERROR )
            /* Unable to read data for the compressed file info record */
            /* so free it since it hasn't yet been added to the list.  */
            {
               Zip_FreeComprssdRec( lpCmpFle );
            }
            else
            /* Try to add the compressed info pointer to the list and */
            /* read the next ZIP signature.                           */
            if ( ( eErr = Zip_InsCompFileRec( lpDir, ( WORD ) -1, lpCmpFle ) ) == ZIP_NO_ERROR )

            {
               eErr = Zip_fArchiveReadByts( lpH, &lpFBuf, &lSignature, sizeof( lSignature ) );
            }
         }
      }

      if ( eErr == ZIP_NO_ERROR )
      /* Look for the end of file record. */
      {
         if ( lSignature != ZIP_END_SIGNATURE )
         {
            eErr = ZIP_DIR_ERROR;
         }
         else
         if ( ( eErr == Zip_fReadByts( lpH, lpFBuf, &ZipEOFRec.wDiskNum, sizeof( ZipEOFRec ) - sizeof( LONG ) ) ) != ZIP_NO_ERROR )
         /* The file was successfully parsed. */
         {
            if ( ZipEOFRec.wRmrkSiz > 0 )
            {
//             /* Keep track of the remark for the archive file. */
//             lpDir->dbRemark.lSize     = ZipEOFRec.wRmrkSiz;
//             lpDir->dbRemark.lStart    = FsvBufIO_fTell( lpFBuf );
//             lpDir->dbRemark.hbFleIndx = ZIP_ARCHV_HANDLE;

//             /* Keep track of the number of bytes required for the */
//             /* "remark data".                                     */
//             lpDir->SI.lExtraRemByts += ( ZipEOFRec.wRmrkSiz );

//             /* Skip over the remark data. */
//             if ( ( eErr = FsvBufIO_fSeek( lpFBuf, ZipEOFRec.wRmrkSiz, SEEK_CUR,
//                                           lpH->lpCritCBFunc, lpH->dwCritCBData ) ) > 0 )
//             {
//                eErr = ZIP_NO_ERROR;
//             }

               if ( ( lpDir->lpszRemark = ( LPTSTR ) Mem_SubMalloc( ZipEOFRec.wRmrkSiz + 1 ) ) == NULL )
               // Bad news, unable to allocate the remark.
               {
                  eErr = ZIP_NO_MEMORY;
               }
               else
               {
                  // Null terminate the "remark".
                  lpDir->lpszRemark[ ZipEOFRec.wRmrkSiz ] = '\0';

                  // Try to read the "remark".
                  eErr = Zip_fArchiveReadByts( lpH, &lpFBuf, lpDir->lpszRemark, ZipEOFRec.wRmrkSiz );
               }
            }

            // If on a floppy, the ZIP file does not span the floppy.
            if ( lpDir->SI.fCanBeSplit )
            {
               // Does the archive file span multiple floppies?
               lpDir->SI.fIsSplit = ( ZipEOFRec.wDiskNum != 0 );

               // Set fields that only pertain to multi-disk archive files.
               lpDir->SI.wStrtDisk    = ZipEOFRec.wStartDisk;
               lpDir->SI.wLastDisk    = ZipEOFRec.wDiskNum;
               lpDir->SI.wCurDisk     = ZipEOFRec.wDiskNum;
               lpDir->SI.lCntrlDirOff = ZipEOFRec.lCntrlDirStart;
            }

            // The archive file was successfully parsed.
            bDirParsed = TRUE;
         }
      }

      if ( eErr != ZIP_NO_ERROR )
      /* Free the list of dynamically allocated compressed file info */
      /* records and the list used to contain all of the pointers.   */
      {
         Zip_FreeComprssdInfoRecs( lpDir );
      }
   }

   return( eErr );
} /* Zip_ReadZipDir */



//==========================================================================
//
// ZIP_ERRCODE Zip_ReadArcPakHeader( lpH, lpFile, bPckTyp )
//
//    lpH     - Pointer to the information structure used to process a
//              connection to an archive file.
//    lpFile  - Pointer to the temporary memory resident structure that will
//              contain the info for the compressed file.
//    bPckTyp - The manner in which the data was "compressed".
//
// This function extracts the appropriate file info from the ARC/PAK
// compressed file header record and copies it into a temporary structure
// that resides in memory.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - The data stored in the compressed file header is
//                    invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_ReadArcPakHeader(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_COMP_INFO   lpFile,
   BYTE              bPckTyp
)

{
   LPFSVBUFIO_FBUF_INFO lpInpt = lpH->lpDir->fiArchvFile.lpFBuf;
   ZIP_ARC_HEADR        hHeadr;
   ZIP_ERRCODE          eErr;

   if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &hHeadr.szName, sizeof( hHeadr ) - sizeof( ZIP_ARC_HEADR_STRT ) ) ) != ZIP_NO_ERROR )
   /* Bad news.  Unable to read the disk file or the end of file was detected. */
   {
      return( eErr );
   }

   /* Update the compressed file info structure with the data */
   /* retrieived from the compressed file header record.      */
   lpFile->lOrigSiz                 = hHeadr.lOrigSiz;
   lpFile->dbComprssdFile.lSize     = hHeadr.lComprsdSiz;
   lpFile->dbComprssdFile.lStart    = FsvBufIO_fTell( lpInpt );
   lpFile->dbComprssdFile.hbFleIndx = ZIP_ARCHV_HANDLE;
   lpFile->lCompHeadrStrt           = lpFile->dbComprssdFile.lStart - sizeof( hHeadr );
   lpFile->lCRC                     = hHeadr.wCRC;
   lpFile->tiTime                   = hHeadr.tiTime;
   lpFile->diDate                   = hHeadr.diDate;

   /* Translate the compression type. */
   switch ( bPckTyp )
   {
      case 2:
         lpFile->ePckTyp = ZIP_NONE;
      break;

      case 3:
         lpFile->ePckTyp = ZIP_RLE;
      break;

      case 4:
         lpFile->ePckTyp = ZIP_SQUEEZED;
      break;

      case 5:
      case 6:
      case 7:
      case 8:
         lpFile->wPckFlgs = bPckTyp - 5;
         lpFile->ePckTyp  = ZIP_CRUNCH;
      break;

      case 9:
         lpFile->ePckTyp = ZIP_SQUASH;
      break;

      case 10:
         lpFile->ePckTyp = ZIP_CRUSH;
      break;

#if USE_ARCPAK
      case 11:
         lpFile->ePckTyp = ZIP_DISTILL;
      break;
#endif

      /* This is allowed, but an error will be generated if a */
      /* call is made to "extract" this file.                 */
      default:
         lpFile->ePckTyp = ZIP_UNKNOWN;
      break;
   }

   /* Get the file name string. */
   _fmemcpy( lpFile->szName, hHeadr.szName, ZIP_NAME_SIZ );
   lpFile->szName[ ZIP_NAME_SIZ - 1 ] = '\0';

   if ( !Zip_ConvertOtherPlatformFileName( lpFile->szName, ZIP_MSDOS, &lpFile->bDirty ) )
   // Bad news.  The path and/or the file name is bogus.
   {
      eErr = ZIP_DIR_ERROR;
   }
   else
   /* Try to advance to the next compressed file header. */
   if ( ( eErr = FsvBufIO_fSeek( lpInpt, hHeadr.lComprsdSiz, SEEK_CUR, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) > 0 )
   {
      eErr = ZIP_NO_ERROR;
   }

   return( eErr );
} /* Zip_ReadArcPakHeader */



//==========================================================================
//
// VOID Zip_ReadArcPakRecord( lpH, wRecTyp )
//
//    lpH          - Pointer to the information structure used to process a
//                   connection to an archive file.
//    wRecTyp      - The type of data contained in the special record (remark,
//                   path, or extra).
//
// Globals:
//    gszNullPath  - The zero byte null terminated string used to prevent
//                   excessive memory allocations.
//
// This function tries to extract the data from the special data record
// for a PAK or an ARC file.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - The data stored in the special data record is invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_ReadArcPakRecord(
  LPZIP_HANDLE_INFO lpH,
  WORD              wRecTyp
)

{
   LPZIP_ARCHIVE_DIR    lpDir  = lpH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpInpt = lpDir->fiArchvFile.lpFBuf;
   ZIP_PAK_HEADR        drDataRec;
   LPZIP_DATA_BLK       lpBlock;
   LPZIP_COMP_INFO      lpCompFile;
   LONG                 lNextRec;
   ZIP_ERRCODE          eErr;

   if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &drDataRec.wFileNum,
                                sizeof( drDataRec ) - sizeof( ZIP_ARC_HEADR_STRT ) ) ) != ZIP_NO_ERROR )
   /* Bad news.  Unable to read the disk. */
   {
      return( eErr );
   }

   if ( drDataRec.wFileNum > lpDir->wFileCount )
   /* Bad news.  The one-based compressed file info index is invalid. */
   {
      return( ZIP_DIR_ERROR );
   }

   /* Compute file position of the next data record. */
   lNextRec = FsvBufIO_fTell( lpInpt ) + drDataRec.lDataSiz;

   if ( drDataRec.wFileNum > 0 )
   /* Compute pointer to the compressed info record. */
   {
      lpCompFile = lpDir->lplpCompFiles[ drDataRec.wFileNum - 1 ];
   }

   /* Process the various record types supported. */
   switch ( wRecTyp )
   {
      #ifdef DoThis
      case ZIP_PAK_ARCHV_TYP:
      {
         ZIP_TYPE eArchvTyp;

         if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &eArchvTyp, sizeof( lpDir->eArchvTyp ) ) ) != ZIP_NO_ERROR )
         /* Bad news.  Unable to read the disk. */
         {
            return( eErr );
         }

         if ( drDataRec.lDataSiz == sizeof( lpDir->eArchvTyp ) &&
              drDataRec.wFileNum == 0                          &&
              ( eArchvTyp == ZIP_FORMAT_ARC || eArchvTyp == ZIP_FORMAT_PAK ) )
         /* Pick up actual type of the archive file.  This is the only way */
         /* to truely distinguish an "extended" ARC file from a PAK file.  */
         {
            lpDir->eArchvTyp = eArchvTyp;
         }
      }
      break;
      #endif

      case ZIP_PAK_REMARK_DATA:
      {
         LPTSTR lpszRemark;

         if ( drDataRec.lDataSiz == 0 )
         // Special case.  There is no new remark.
         {
            lpszRemark = &gszNullPath[ 0 ];
         }
         else
         // Try to dynamically allocate memory for the "remark".
         {
            if ( ( lpszRemark = ( LPTSTR ) Mem_SubMalloc( ( WORD ) drDataRec.lDataSiz + 1 ) ) == NULL )
            {
               return( ZIP_NO_MEMORY );
            }

            if ( ( eErr = Zip_fReadByts( lpH, lpInpt, lpszRemark, ( WORD ) drDataRec.lDataSiz ) ) != ZIP_NO_ERROR )
            // Bad news.  Unable to read the disk.
            {
               Mem_SubFree( lpszRemark );
               return( eErr );
            }

            // Null terminate the remark.
            lpszRemark[ drDataRec.lDataSiz ] = '\0';
         }

         if ( drDataRec.wFileNum > 0 )
         // The "remark" pertains to a compressed file.
         {
            // If relevant, free the "old" remark.
            Zip_FreeRemarkChk( lpCompFile->lpszRemark );

            // Store new remark pointer.
            lpCompFile->lpszRemark = lpszRemark;
         }
         else
         // The "remark" pertains to the archive file.
         {
            // If relevant, free the "old" remark.
            Zip_FreeRemarkChk( lpDir->lpszRemark );

            // Store new remark pointer.
            lpDir->lpszRemark = lpszRemark;
         }

//       // Keep track of the number of bytes required for the "remark data".
//       lpDir->SI.lExtraRemByts += drDataRec.lDataSiz;
      }
      break;

//    case ZIP_PAK_REMARK_DATA:
      case ZIP_PAK_EXTRA_DATA:
         if ( drDataRec.wFileNum > 0 )
         /* The "remark" or "extra data" pertains to a compressed file. */
         {
//          if ( wRecTyp == ZIP_PAK_REMARK_DATA )
//          {
//             lpBlock = &lpCompFile->dbRemark;
//          }
//          else
            {
               lpBlock = &lpCompFile->dbExtra1;
            }
         }
         /* The "remark" or "extra data" pertains to the entire */
         /* archive file.                                       */
         else
         {
//          if ( wRecTyp == ZIP_PAK_REMARK_DATA )
//          {
//             lpBlock = &lpDir->dbRemark;
//          }
//          else
            {
               lpBlock = &lpDir->dbExtra;
            }
         }

         /* Describe size and position of the "data block". */
         lpBlock->lSize     = drDataRec.lDataSiz;
         lpBlock->lStart    = FsvBufIO_fTell( lpInpt );
         lpBlock->hbFleIndx = ZIP_ARCHV_HANDLE;

         /* Keep track of the number of bytes required for the */
         /* "extra data" and the "remark data".                */
         lpDir->SI.lExtraRemByts += drDataRec.lDataSiz;
      break;

      case ZIP_PAK_PATH_DATA:
      {
         LPTSTR lpszPath;

         if ( drDataRec.wFileNum == 0 || drDataRec.lDataSiz > FSV_MAXDIRNAME_LEN )
         /* Bad news.  Invalid compressed file info record index or */
         /* the path data is too "long".                            */
         {
            return( ZIP_DIR_ERROR );
         }

         if ( drDataRec.lDataSiz == 0 )
         /* Special case.  Point to the global "blank" path. */
         {
            lpszPath = &gszNullPath[ 0 ];
         }
         else
         /* Try to dynamically allocate memory for the "path". */
         {
            lpszPath = ( LPTSTR ) Mem_SubMalloc( ( WORD ) drDataRec.lDataSiz + 1 );

            if ( lpszPath == NULL )
            {
               return( ZIP_NO_MEMORY );
            }

            if ( ( eErr = Zip_fReadByts( lpH, lpInpt, lpszPath, ( WORD ) drDataRec.lDataSiz ) ) != ZIP_NO_ERROR )
            /* Bad news.  Unable to read the disk. */
            {
               return( eErr );
            }

            /* Null terminate the path. */
            lpszPath[ drDataRec.lDataSiz ] = '\0';
         }

         /* If relevant, free the "old" path. */
         Zip_FreePathChk( lpCompFile->lpszPath );

         /* Store new path pointer. */
         lpCompFile->lpszPath = lpszPath;

         if ( drDataRec.lDataSiz > 0 && !FsvUtil_ValidateFullFileName( lpszPath, FALSE ) )
         // Bad news, the path is bogus.
         {
            return( ZIP_DIR_ERROR );
         }
      }
      break;
   }

   /* Try to move the file position to the next special data record. */
   if ( ( eErr = FsvBufIO_fSeek( lpInpt, lNextRec, SEEK_SET, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) == lNextRec )
   {
      eErr = ZIP_NO_ERROR;
   }

   return( eErr );
} /* Zip_ReadArcPakRecord */



//==========================================================================
//
// ZIP_ERRCODE Zip_ReadArcPakDir( lpH )
//
//    lpH - Pointer to the information structure used to process a
//          connection to an archive file.
//
// This function tries to parse the specified ARC or PAK file.  It starts
// from the beginning of the file and evaluates each compressed file header.
// After that it processes any special info records that can be stored at
// the end of the file.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - The data stored in the compressed file headers is
//                    invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_ReadArcPakDir(
  LPZIP_HANDLE_INFO lpH
)

{
   BOOL                 bOnlyArcFiles = TRUE;
   LPZIP_ARCHIVE_DIR    lpDir         = lpH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpInpt        = lpDir->fiArchvFile.lpFBuf;
   ZIP_ARC_HEADR_STRT   hsHeader;
   LPZIP_COMP_INFO      lpCmpFle;
   ZIP_ERRCODE          eErr;

   /* Assume the file is a PAK file. */
   lpDir->eArchvTyp = ZIP_FORMAT_PAK;

   /* Try to read the initial "marker" record. */
   if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &hsHeader, sizeof( hsHeader ) ) ) != ZIP_NO_ERROR )
   /* Bad news.  Unable to read the disk file. */
   {
      return( eErr );
   }

   /* Parse all compressed file headrers. */
   while ( hsHeader.bMarker == ZIP_ARC_MARKER && hsHeader.bType != ZIP_EOF )
   {
      /* Try to allocate a compressed file info record. */
      if ( ( lpCmpFle = ( LPZIP_COMP_INFO ) Mem_SubMalloc( sizeof( ZIP_COMP_INFO ) ) ) == NULL )
      {
         eErr = ZIP_NO_MEMORY;
         break;
      }
      else
      {
         /* Initialize the compressed file info record. */
         Zip_InitCompFileRec( lpCmpFle, FALSE, TRUE, NULL );

         if ( ( eErr = Zip_ReadArcPakHeader( lpH, lpCmpFle, hsHeader.bType ) ) != ZIP_NO_ERROR )
         /* Unable to read data for the compressed file info record */
         /* so free it since it hasn't yet been added to the list.  */
         {
            Zip_FreeComprssdRec( lpCmpFle );
            break;
         }
         else
         /* Try to add the compressed info pointer to the list and */
         /* read the next "marker" record.                         */
         if ( ( eErr = Zip_InsCompFileRec( lpDir, ( WORD ) -1, lpCmpFle ) ) != ZIP_NO_ERROR ||
              ( eErr = Zip_fReadByts( lpH, lpInpt, &hsHeader, sizeof( hsHeader ) ) ) != ZIP_NO_ERROR )
         /* Bad news.  Unable to insert pointer or read the disk file. */
         {
            break;
         }
         else
         if ( bOnlyArcFiles )
         /* See if the compressed files only utilize ARC */
         /* compression methods.                         */
         {
            bOnlyArcFiles = ( hsHeader.bType == ZIP_NONE     ||
                              hsHeader.bType == ZIP_RLE      ||
                              hsHeader.bType == ZIP_SQUEEZED ||
                              hsHeader.bType == ZIP_CRUNCH   ||
                              hsHeader.bType == ZIP_SQUASH );
         }
      }
   }

   if ( eErr == ZIP_NO_ERROR )
   {
      if ( hsHeader.bMarker != ZIP_ARC_MARKER )
      /* Bad news.  An invalid "marker" was detcted. */
      {
         eErr = ZIP_DIR_ERROR;
      }
      else
      /* Try to read a subsequent "marker" record. */
      if ( ( eErr = FsvBufIO_fRead( lpInpt, &hsHeader, sizeof( hsHeader ), lpH->lpCritCBFunc, lpH->dwCritCBData ) ) >= 0 )
      {
         if ( eErr != sizeof( hsHeader ) || hsHeader.bMarker != ZIP_PAK_MARKER )
         /* JJS - 09/22/92 - Account for random garbage stored */
         /* at the end of an "ARC" file.                       */
         {
            eErr             = ZIP_NO_ERROR;
            lpDir->eArchvTyp = ZIP_FORMAT_ARC;
         }
         else
         /* Process all of the "special" data records that can be */
         /* stored at the end of an ARC or PAK file.  If the file */
         /* is really an "extended ARC" file then an identifier   */
         /* record will have been stored in the "special" data    */
         /* records.                                              */
         {
            WORD wZipTyp;

            /* Not an error, just the number of bytes read. */
            eErr = ZIP_NO_ERROR;

            while ( hsHeader.bMarker == ZIP_PAK_MARKER && hsHeader.bType != ZIP_EOF )
            {
               if ( ( eErr = Zip_ReadArcPakRecord( lpH, hsHeader.bType ) ) != ZIP_NO_ERROR ||
                    ( eErr = Zip_fReadByts( lpH, lpInpt, &hsHeader, sizeof( hsHeader ) ) ) != ZIP_NO_ERROR )
               {
                  break;
               }
            }

            if ( hsHeader.bMarker != ZIP_PAK_MARKER && eErr == ZIP_NO_ERROR )
            /* Bad news.  The ARC/PAK file was not concluded with a */
            /* special end of file record.                          */
            {
               eErr = ZIP_DIR_ERROR;
            }
            else
            if ( bOnlyArcFiles && FsvUtil_IsZipFile( lpDir->fiArchvFile.szFullName, &wZipTyp ) &&
                 wZipTyp == ZIP_FORMAT_ARC )
            {
               lpDir->eArchvTyp = ZIP_FORMAT_ARC;
            }
         }
      }
   }

   if ( eErr != ZIP_NO_ERROR )
   /* Free the list of dynamically allocated compressed file info */
   /* records and the list used to contain all of the pointers.   */
   {
      Zip_FreeComprssdInfoRecs( lpDir );
   }

   return( eErr );

} /* Zip_ReadArcPakDir */



//==========================================================================
//
// BYTE Zip_CalcLHAByteChkSum( lpvData, wNumByts, bChkSum );
//
//    lpvData  - Pointer to the data to compute the byte checkcum with.
//    wNumByts - The number of bytes to use when computing the checksum.
//    bChkSum  - The current checksum value (pass in zero if this is the
//               initial stream of data).
//
// This function computes and returns a byte checksum.
//
//==========================================================================

BYTE WINAPI Zip_CalcLHAByteChkSum(
   LPVOID lpvData,
   WORD   wNumByts,
   BYTE   bChkSum
)

{
   LPBYTE lpbData = ( LPBYTE ) lpvData;

   // Calculate the checksum;
   for ( ; wNumByts != 0; wNumByts--, lpbData++ )
   {
      bChkSum += *lpbData;
   }

   return( bChkSum );
} // Zip_CalcLHAByteChkSum



//==========================================================================
//
// BYTE Zip_CalcLZHHeadrSiz( bLevel, bFileLen,
//
//    bLevel   - The level of the LZH file (0 or 1).
//    bFileLen - The length of the dynamic file name that can be stored
//               in the header.
//
// This function computes the "expected" header size of a level 0 or level 1
// compressed file.
//
//==========================================================================

WORD WINAPI Zip_CalcLZHHeadrSiz(
   BYTE bLevel,
   BYTE bFileLen
)

{
   WORD wExpctdSiz = 0;

   // Only level 0 and level are supported.
   switch ( bLevel )
   {
      case 0:
         wExpctdSiz = ZIP_LHA_BASE_HEADR_BYTS + bFileLen;
      break;

      case 1:
         wExpctdSiz = ZIP_LHA_BASE_HEADR_BYTS + bFileLen + 3;
      break;
   }

   return( wExpctdSiz );
} // Zip_CalcLZHHeadrSiz



//==========================================================================
//
// ZIP_ERRCODE Zip_ReadLHAFileHeader( lpH, lpFile, lpfEOF )
//
//    lpH    - Pointer to the information structure used to process a
//             connection to an archive file.
//    lpFile - Pointer to the temporary memory resident structure that will
//             contain the info for the compressed file.
//    lpfEOF - Pointer to a boolean.  This flag will be set to true if the
//             end of file is prematurely detected or a bad header check-sum
//             is detected.  This flag is returned because this is how I
//             detect when the end of an LHA file is encounter (the bozo
//             designer didn't include an end of file record).  I also
//             interpret a bad check-sum as the end of file.
//
// This function extracts the appropriate file info from the LHA compressed
// file header record and copies it into a temporary structure that resides
// in memory.
//
// This function supports level 0 and level 1 LZH files.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - The data stored in the compressed file header is
//                    invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_ReadLHAFileHeader(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_COMP_INFO   lpFile,
   LPBOOL            lpfEOL
)

{
   ZIP_LHA_FILE_HEADR   hHeadr;
   TCHAR                 szName[ FSV_LFILENAMEBUF_LEN ];
   TCHAR                 szPath[ FSV_PATHBUF_LEN ];
   ZIP_ERRCODE          eErr;
   WORD                 wRecLen;
   WORD                 wExpctdSiz;
   LPFSVBUFIO_FBUF_INFO lpInpt = lpH->lpDir->fiArchvFile.lpFBuf;

   // Mark the start of the header data.
   lpFile->lCompHeadrStrt = FsvBufIO_fTell( lpInpt );

   if ( ( eErr = FsvBufIO_fRead( lpInpt, &hHeadr, ZIP_LHA_BASE_HEADR_BYTS, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) < 0 )
   // Bad news.  Unable to read the disk file.
   {
      return( eErr );
   }

   // Calculate the expected header size.
   wExpctdSiz = Zip_CalcLZHHeadrSiz( hHeadr.bLevel, hHeadr.bLen );

   if ( eErr < ZIP_LHA_BASE_HEADR_BYTS || ( WORD ) hHeadr.bHeadrSiz < wExpctdSiz ||
        _fmemicmp( hHeadr.szCompMethod, &"-lh", 3 ) != 0 || hHeadr.szCompMethod[ 4 ] != '-' ||
        hHeadr.szCompMethod[ 3 ] < '0' || hHeadr.szCompMethod[ 3 ] > '9' ||
        hHeadr.bLen > FSV_PATHBUF_LEN - 3 || hHeadr.bLevel > 1 )
   // I assume the end of file has been detected.
   {
      *lpfEOL = TRUE;
      return( ZIP_NO_ERROR );
   }

   // Calculate the number of unidentified header bytes.
   lpFile->wUnknownByts = hHeadr.bHeadrSiz - wExpctdSiz;

   if ( lpFile->wUnknownByts > Zip_GetUnknownByts( hHeadr.bLevel ) )
   // Bad news.  The header contains too much garbage.
   {
      return( ZIP_DIR_ERROR );
   }

   if ( lpFile->wUnknownByts > 0 )
   // Store any unidentified header bytes that were encountered.
   {
      _fmemcpy( lpFile->abUnknwnByts, &( ( LPBYTE ) hHeadr.szCompMethod )[ wExpctdSiz ],
                lpFile->wUnknownByts );
   }

   // Read the file name (can include the path) plus a few static bytes
   // (don't ask me why these static bytes are stored after a dynamic
   // string).
   if ( ( eErr = Zip_fReadByts( lpH, lpInpt, hHeadr.szFileName, (WORD)(( hHeadr.bHeadrSiz + 2 ) - ZIP_LHA_BASE_HEADR_BYTS) ) ) != ZIP_NO_ERROR )
   // Bad news.  Unable to read the disk file.
   {
      return( eErr );
   }

   // Calculate the checksum for the header info.
   if ( Zip_CalcLHAByteChkSum( &hHeadr.szCompMethod, hHeadr.bHeadrSiz, 0 ) != hHeadr.bHeadrChkSum )
   // I assume the end of file has been detected.
   {
      *lpfEOL = TRUE;
      return( ZIP_NO_ERROR );
   }

   // The CRC is stored after the file name (don't ask me why).
   lpFile->lCRC  = ( LONG ) *( ( LPWORD ) &hHeadr.szFileName[ hHeadr.bLen ] );

   // Null terminate the file name.
   hHeadr.szFileName[ hHeadr.bLen ] = '\0';

   if ( !Zip_ConvertOtherPlatformFileName( hHeadr.szFileName, ZIP_AMIGA, &lpFile->bDirty ) )
   // Bad news.  The file name is bogus.
   {
      return( ZIP_DIR_ERROR );
   }

   // Separate the file name and the path (the path will only be stored
   // here if this is a self-extracting LHZ file).
   FsvUtil_GetPathMask( hHeadr.szFileName, szPath, szName, FALSE );

   if ( hHeadr.bLevel == 1 )
   // The header size indicates that this is a version of an LHA compression
   // file that can have extended header records.
   {
      // Get the length of the 1st extended header record that might be stored
      // in the compressed area (sad but true).
      wRecLen = *( ( LPWORD ) ( ( LPBYTE ) &hHeadr + hHeadr.bHeadrSiz ) );

      // Skip over all extended header records and total the bytes used
      // for the extended header records.
      while ( wRecLen != 0 )
      {
         BYTE bRecID;
         WORD wLen = wRecLen - 3;

         // Keep track of the extended header record bytes.
         lpFile->wSpclRecByts += wRecLen;

         // Subtract the length of extended header record.
         hHeadr.lComprsdSiz -= wRecLen;

         if ( wRecLen <= 3 )
         // Bad news, the record length is bogus.
         {
            return( ZIP_DIR_ERROR );
         }

         if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &bRecID, sizeof( bRecID ) ) ) != ZIP_NO_ERROR )
         // Bad news.  Unable to read the disk file.
         {
            return( eErr );
         }

         // Process some of the extended header records.
         switch ( bRecID )
         {
            // Record that contains the path.
            case 0x02:
            {
               LPTSTR lpszTmp;

               if ( wLen >= FSV_MAXDIRNAME_LEN )
               // Bad news, the path length is bogus.
               {
                  return( ZIP_DIR_ERROR );
               }

               if ( ( eErr = Zip_fReadByts( lpH, lpInpt, szPath, wLen ) ) != ZIP_NO_ERROR )
               // Bad news.  Unable to read the disk file.
               {
                  return( eErr );
               }

               // Null terminate the path.
               szPath[ wLen ] = '\0';

               // Convert 0xFF to a backslash.
               for ( lpszTmp = szPath; lstrlen(lpszTmp) > 0; lpszTmp = CharNext(lpszTmp) )
               {
                  if ( ( BYTE ) lpszTmp[ 0 ] == ( BYTE ) 0xFF )
                  {
                     lpszTmp[ 0 ] = '\\';
                  }
               }

               if ( !FsvUtil_ValidatePathFileMask( szPath, FALSE, FALSE ) )
               // Bad news.  The path is bogus.
               {
                  return( ZIP_DIR_ERROR );
               }
            }
            break;

            // Get the "extended" file attribute.  Don't ask me why numb-nuts
            // makes use of this record.  I guess because he stored garbage
            // in the hHeadr.wFileAttr field.
            case 0x40:
            {
               WORD wAttr;

               if ( wLen != 2 )
               // Bad news, the file attribute length is bogus.
               {
                  return( ZIP_DIR_ERROR );
               }

               if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &wAttr, wLen ) ) != ZIP_NO_ERROR )
               // Bad news.  Unable to read the disk file.
               {
                  return( eErr );
               }

               // Store the extended file attribute.
               hHeadr.bFileAttr = LOBYTE( wAttr );
            }
            break;

            // The rest of the records are simply skipped.
            default:
               // Try to skip over the data contained in the record.
               if ( ( eErr = FsvBufIO_fSeek( lpInpt, wLen, SEEK_CUR, lpH->lpCritCBFunc,
                                             lpH->dwCritCBData ) ) > 0 )
               {
                  eErr = ZIP_NO_ERROR;
               }
               else
               {
                  return( eErr );
               }
            break;
         }

         // Try to read the length of the next extended header record.
         if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &wRecLen, sizeof( wRecLen ) ) ) != ZIP_NO_ERROR )
         // Bad news.  Unable to read the disk file.
         {
            return( eErr );
         }
      }
   }

   // Update the compressed file info structure with the data
   // retrieived from the compressed file header record.
   lpFile->lOrigSiz                 = hHeadr.lOrigSiz;
   lpFile->dbComprssdFile.lStart    = FsvBufIO_fTell( lpInpt );
   lpFile->dbComprssdFile.hbFleIndx = ZIP_ARCHV_HANDLE;
   lpFile->tiTime                   = hHeadr.tiTime;
   lpFile->diDate                   = hHeadr.diDate;
// lpFile->wAttrs                   = hHeadr.wFileAttr & 0xFF;
   lpFile->wAttrs                   = hHeadr.bFileAttr;
   lpFile->dbComprssdFile.lSize     = hHeadr.lComprsdSiz;
   lpFile->wPckFlgs                 = hHeadr.szCompMethod[ 3 ] - '0';
   lpFile->bLevel                   = hHeadr.bLevel;

   if ( szPath[ 0 ] != '\0' )
   // Try to dynamically allocate the path.
   {
      if ( ( lpFile->lpszPath = ( LPTSTR ) Mem_SubMalloc( FsvStr_GetByteCount( szPath ) + 1 ) ) == NULL )
      // Bad news, unable to allocate the path.
      {
         return( ZIP_NO_MEMORY );
      }
      else
      {
         lstrcpy( lpFile->lpszPath, szPath );
      }
   }

   // Make local copy of the file name.
   lstrcpy( lpFile->szName, szName );

   // Translate the compression type.
   switch ( hHeadr.szCompMethod[ 3 ] )
   {
      case '0':
         lpFile->ePckTyp = ZIP_NONE;
      break;

      default:
         lpFile->ePckTyp  = ZIP_LZH;
      break;
   }

   // Try to advance to the next compressed file header (the header
   // size doesn't account for the 1st two bytes).
   if ( ( eErr = FsvBufIO_fSeek( lpInpt, hHeadr.lComprsdSiz, SEEK_CUR, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) > 0 )
   {
      eErr = ZIP_NO_ERROR;
   }

   return( eErr );
} // Zip_ReadLHAFileHeader



//==========================================================================
//
// ZIP_ERRCODE Zip_ReadLHAFile( lpH )
//
//    lpH - Pointer to the information structure used to process a
//          connection to an archive file.
//
// This function tries to parse the specified LZH file.  It starts
// from the beginning of the file and evaluates each compressed file header.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - The data stored in the compressed file headers is
//                    invalid.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_ReadLHAFile(
  LPZIP_HANDLE_INFO lpH
)

{
   LONG                 lFilePos;
   ZIP_ERRCODE          eErr   = ZIP_NO_ERROR;
   LPZIP_ARCHIVE_DIR    lpDir  = lpH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpInpt = lpDir->fiArchvFile.lpFBuf;
   BOOL                 fEOF   = FALSE;

   // Assume the file is an LHA file.
   lpDir->eArchvTyp = ZIP_FORMAT_LHA;

   // A file of zero length is assummed to be an empty LZH file.
   if ( FsvBufIO_fRtrnEOFPos( lpInpt ) != 0 )
   {
      // Parse all compressed file headrers.
      do
      {
         LPZIP_COMP_INFO lpCmpFle;

         // Try to allocate a compressed file info record.
         if ( ( lpCmpFle = ( LPZIP_COMP_INFO ) Mem_SubMalloc( sizeof( ZIP_COMP_INFO ) ) ) == NULL )
         {
            eErr = ZIP_NO_MEMORY;
         }
         else
         {
            // Keep track of the last valid file position.
            lFilePos = FsvBufIO_fTell( lpInpt );

            // Initialize the compressed file info record.
            Zip_InitCompFileRec( lpCmpFle, FALSE, TRUE, NULL );

            // Try to read the data for the compressed file and then
            // add the compressed info pointer to the list.
            if ( ( eErr = Zip_ReadLHAFileHeader( lpH, lpCmpFle, &fEOF ) ) != ZIP_NO_ERROR ||
                 fEOF ||
                 ( eErr = Zip_InsCompFileRec( lpDir, ( WORD ) -1, lpCmpFle ) ) != ZIP_NO_ERROR )
            {
               Zip_FreeComprssdRec( lpCmpFle );
            }
         }
      }
      while ( eErr == ZIP_NO_ERROR && !fEOF );
   }

   if ( fEOF )
   {
      if ( lpDir->lplpCompFiles == NULL )
      // This is not an LHA file.
      {
         eErr = ZIP_UNRECNZD_ZIP;
      }
      else
      // Restore the last valid file position.
      {
         if ( ( eErr = FsvBufIO_fSeek( lpInpt, lFilePos, SEEK_SET, lpH->lpCritCBFunc,
                                       lpH->dwCritCBData ) ) == lFilePos )
         {
            eErr = ZIP_NO_ERROR;
         }
      }
   }

   if ( eErr != ZIP_NO_ERROR )
   // Free the list of dynamically allocated compressed file info
   // records and the list used to contain all of the pointers.
   {
      Zip_FreeComprssdInfoRecs( lpDir );
   }

   return( eErr );
} // Zip_ReadLHAFile



//==========================================================================
//
// ZIP_ERRCODE Zip_BuildInternalDir( lpH )
//
//    lpH - Pointer to the information structure used to process a
//          connection to an archive file.
//
// This function deterimines if the file is an archive file, and if so,
// dispatches the appropriate parsing function to construct the "internal
// directory".
//
// Returns:
//    ZIP_NO_ERROR     - The operation was successful.
//    ZIP_UNRECNZD_ZIP - The file is not a recognized zip file.
//    ZIP_DIR_ERROR    - The end of file was prematurely detected, the data
//                       stored in the compressed file headers, the central
//                       directory, or the special ARC/PAK records is invalid.
//    ZIP_NO_MEMORY    - Not enough memory available for dynamic allocations.
//    FSVERR...        - The appropriate error code returned by File Services
//                       ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_BuildInternalDir(
  LPZIP_HANDLE_INFO lpH )

{
   ZIP_ERRCODE          eErr;
   FSVIO_DIRENTRYSTRUCT diDirInfo;
   BYTE                 bSignature = 0;
   WORD                 wSignature = 0;
   LPZIP_ARCHIVE_DIR    lpDir      = lpH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpInpt     = lpDir->fiArchvFile.lpFBuf;

   if ( FsvBufIO_fRtrnEOFPos( lpInpt ) != 0 )
   {
      /* Read the first two bytes of the file to determine if it */
      /* is an "executable" program.                             */
      if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &wSignature, sizeof( wSignature ) ) ) != ZIP_NO_ERROR )
      {
         return( eErr );
      }

      //-*****FIX*****Zip File spanning diskettes
      if ( wSignature == 0x5a4d )
      /* The archive file appears to be a self extracting executable. */
      {
         WORD wRemainder;
         WORD wNumSctrs;

         /* Try to read the number of 512 byte sectors (rounded up) and */
         /* the remainder.                                              */
         if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &wRemainder, sizeof( wRemainder ) ) ) != ZIP_NO_ERROR ||
              ( eErr = Zip_fReadByts( lpH, lpInpt, &wNumSctrs,  sizeof( wNumSctrs ) ) ) != ZIP_NO_ERROR )
         {
            return( eErr );
         }

         if ( wRemainder != 0 )
         /* There is a remainder so reduce the "rounded up" sector count. */
         {
            wNumSctrs--;
         }

         /* Multiply the number of sectors by 512 and add the remainder. */
         lpDir->dbExeHeadr.lSize = ( ( LONG ) wNumSctrs << 9 ) | wRemainder;

         /* Try to read the ARC/PAK ID byte. */
         if ( ( eErr = Zip_fReadByts( lpH, lpInpt, &bSignature, sizeof( bSignature ) ) ) != ZIP_NO_ERROR )
         {
            return( ZIP_DIR_ERROR );
         }

         if ( bSignature != ZIP_ARC_MARKER )
         /* It's not an ARC/PAK file so assume it may be a ZIP file. */
         {
            if ( ( eErr = FsvBufIO_fSeek( lpInpt, 29L, SEEK_SET, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) != 29L ||
                 ( eErr = Zip_fReadByts( lpH, lpInpt, &bSignature, sizeof( bSignature ) ) ) != ZIP_NO_ERROR )
            /* Try to find the "extra byte" in the header.  The ZIP */
            /* self-extract header is somewhat "screwy".            */
            {
               return( ZIP_DIR_ERROR );
            }

            /* Add the "extra byte" to the header size. */
            if (bSignature == 1)
               lpDir->dbExeHeadr.lSize += bSignature;
         }
      }

      /* If necessary, skip over the "executable" portion of the archive */
      /* file. Read the "marker byte" then reset the file position.      */
      if ( ( eErr = FsvBufIO_fSeek( lpInpt, lpDir->dbExeHeadr.lSize, SEEK_SET,
                                    lpH->lpCritCBFunc, lpH->dwCritCBData ) ) != lpDir->dbExeHeadr.lSize ||
           ( eErr = Zip_fReadByts( lpH, lpInpt, &bSignature, sizeof( bSignature ) ) ) != ZIP_NO_ERROR ||
           ( eErr = FsvBufIO_fSeek( lpInpt, lpDir->dbExeHeadr.lSize, SEEK_SET,
                                    lpH->lpCritCBFunc, lpH->dwCritCBData ) ) != lpDir->dbExeHeadr.lSize )
      {
         return( eErr );
      }
   }

   if ( ( eErr = FsvIO_GetDirEntry( lpDir->fiArchvFile.szFullName, &diDirInfo, NULL,
                                    lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == FSV_SUCCESS )
   {
      BOOL fParseLZH = FALSE;

      /* Save size of file and the date and time it was last modified. */
      lpDir->SI.wDate     = diDirInfo.wDate;
      lpDir->SI.wTime     = diDirInfo.wTime;
      lpDir->SI.dwFileSiz = diDirInfo.dwSize;

      //-*****FIX*****Zip File spanning diskettes
      if ( bSignature == ZIP_ARC_MARKER )
      /* Try to parse the ARC/PAK archive file. */
      {
         eErr = Zip_ReadArcPakDir( lpH );
         if (eErr == ZIP_NO_ERROR)
            lpDir->SI.fCanBeSplit = FALSE;
      }
      else
      if ( bSignature == ZIP_ZIP_MARKER ||
           ( lpDir->SI.fCanBeSplit && Zip_DoesDiskHaveSpecialLabel( lpH, NULL, NULL ) ) )
      // Try to parse the ZIP archive file.
      {
         //-*****If split then verify signature of 1st header record and
         //-*****possibly the ZIP extension.
         eErr = Zip_ReadZipDir( lpH );
      }
      else
      // See if the file is an LZH file.
      {
         fParseLZH = TRUE;
      }

      if ( fParseLZH || ( eErr != ZIP_NO_ERROR && !FsvIO_IsCritErr( eErr ) ) )
      // See if we are dealing with an LHA file (I handle the case
      // where the 1st byte was random data but it happened to be the
      // ARC/PAK file ID or the ZIP file ID).
      {
         if ( !fParseLZH )
         // Another archive type was unsuccessfully parsed so reset the
         // starting file position.
         {
            if ( ( eErr = FsvBufIO_fSeek( lpInpt, lpDir->dbExeHeadr.lSize, SEEK_SET,
                                          lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == lpDir->dbExeHeadr.lSize )
            // We be kool.
            {
               eErr = ZIP_NO_ERROR;
            }
         }

         if ( eErr == ZIP_NO_ERROR )
         {
            eErr = Zip_ReadLHAFile( lpH );
            if (eErr == ZIP_NO_ERROR)
               lpDir->SI.fCanBeSplit = FALSE;
         }
      }

      if ( eErr == ZIP_NO_ERROR )
      /* Save final state info about the archive file. */
      {
         lpDir->SI.lAvailPostn = ( !lpDir->SI.fIsSplit ) ? FsvBufIO_fTell( lpInpt ) : lpDir->SI.lCntrlDirOff;
         lpDir->SI.bBadState   = FALSE;
      }
   }

   return( eErr );
} /* Zip_BuildInternalDir */



/*---------------------------------------------------------*/
/*   A R C H I V E   F I L E   W R I T E   R O U T I N E S */
/*---------------------------------------------------------*/


//==========================================================================
//
// ZIP_ERRCODE Zip_SetSharedData( lpFile, lpShardHeadr, wExtraSiz )
//
//    lpFile       - Pointer to the structure that contains the compressed
//                   file info.
//    lpShardHeadr - Pointer to the "shared data" structure.
//    wExtraSiz    - The amount of extra data associated with the file
//                   header or the central directory.
//
// This function copies and translates the "generic" compressed file info
// to the "shared" ZIP record that is stored in a compressed file header
// and a central directory header.
//
//==========================================================================

VOID WINAPI Zip_SetSharedData(
   LPZIP_COMP_INFO    lpFile,
   LPZIP_SHARED_HEADR lpShardHeadr,
   WORD               wExtraSiz
)

{
   /* Initialize fields intrinsic to ZIP. */
   lpShardHeadr->fEightKWindow  = 0;
   lpShardHeadr->fCharsEncoded  = 0;
   lpShardHeadr->fUnused        = 0;
   lpShardHeadr->bExtractVersn  = 9;
   lpShardHeadr->bExtractOSTyp  = ZIP_MSDOS;

   /* Copy all fields stored in the generic compressed info. */
   lpShardHeadr->fEncrypted     = lpFile->bEncrypted;
   lpShardHeadr->tiTime         = lpFile->tiTime;
   lpShardHeadr->diDate         = lpFile->diDate;
   lpShardHeadr->lCRC           = lpFile->lCRC;
   lpShardHeadr->lOrigSiz       = lpFile->lOrigSiz;
   lpShardHeadr->lComprsdSiz    = lpFile->dbComprssdFile.lSize;
   lpShardHeadr->wNameSiz       = (WORD)(FsvStr_GetByteCount( lpFile->szName ) + FsvStr_GetByteCount( lpFile->lpszPath ));
   lpShardHeadr->wExtraSiz      = wExtraSiz;

   /* Decode the generic pack type. */
   switch ( lpFile->ePckTyp )
   {
      case ZIP_NONE:
         lpShardHeadr->wPackTyp = 0;
      break;

      case ZIP_SHRINK:
         lpShardHeadr->wPackTyp = 1;
      break;

      case ZIP_REDUCED:
         lpShardHeadr->wPackTyp = 2 + lpFile->wPckFlgs;
      break;

      case ZIP_IMPLODE:
         lpShardHeadr->wPackTyp      = 6;
         lpShardHeadr->bExtractVersn = 10;
         lpShardHeadr->fEightKWindow = ( lpFile->wPckFlgs & ZIP_8K_WINDOW ) > 0;
         lpShardHeadr->fCharsEncoded = ( lpFile->wPckFlgs & ZIP_CHARS_ENCODED ) > 0;
      break;

      case ZIP_TOKENIZED:
         lpShardHeadr->wPackTyp = 7;
      break;

      case ZIP_DEFLATN:
         lpShardHeadr->wPackTyp      = 8;
         lpShardHeadr->fEightKWindow = ( lpFile->wPckFlgs & ZIP_8K_WINDOW ) > 0;
         lpShardHeadr->fCharsEncoded = ( lpFile->wPckFlgs & ZIP_CHARS_ENCODED ) > 0;
      break;

      // Should never happen.
      default:
         lpShardHeadr->wPackTyp = 255;
      break;
   }

} /* Zip_SetSharedData */



//==========================================================================
//
// ZIP_ERRCODE WriteZipPath( lpH, lplpOutpt, lpszPath )
//
//    lpH       - Pointer to the information structure used to process a
//                connection to an archive file.
//    lplpOutpt - Pointer to the pointer to the structure used to buffer
//                i/o to a file or memory buffer.  When compressing a file
//                into an archive file if the archive file is closed and
//                re-opened because another diskette had to be inserted
//                then *lplpOutpt will be updated.
//    lpszPath  - Pointer to the path to write.
//
// This function writes the path (if one exists) to the output file.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteZipPath(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpOutpt,
   LPTSTR                  lpszPath
)

{
   LPTSTR       lpszTmp;
   ZIP_ERRCODE eErr;
   WORD        wLen;
   TCHAR        szTmp[ FSV_PATHBUF_LEN ];

   // Make the directory node separators compatible with ZIP.
   lstrcpy( szTmp, lpszPath );
   for ( wLen = 0, lpszTmp = szTmp; lstrlen(lpszTmp) > 0; lpszTmp = CharNext(lpszTmp), wLen++ )
   {
      if (IsDBCSLeadByte(*lpszTmp))
         wLen++;

      if ( *lpszTmp == '\\' )
      {
         *lpszTmp = '/';
      }
   }

   if ( wLen == 0 || ( eErr = Zip_fArchiveWrt( lpH, lplpOutpt, szTmp, wLen ) ) == ( LONG ) wLen )
   {
      eErr = ZIP_NO_ERROR;
   }

   return( eErr );
} // Zip_WriteZipPath
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteZipHeadr( lpH, lplpOutpt, lpFile, bWrtExtraData )
//
//    lpH           - Pointer to the information structure used to process a
//                    connection to an archive file.
//    lplpOutpt     - Pointer to the pointer to the structure used to buffer
//                    i/o to a file or memory buffer.  When compressing a file
//                    into an archive file if the archive file is closed and
//                    re-opened because another diskette had to be inserted
//                    then *lplpOutpt will be updated.
//    lpFile        - Pointer to the structure that contains the compressed
//                    file info.
//    bWrtExtraData - If this flag is TRUE then write out the "extra data"
//                    referenced by lpFile->dbExtra1.
//
// This function writes out the ZIP compressed file header, the path and
// file name, and the "extra data" (if requested).
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteZipHeadr(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpOutpt,
   LPZIP_COMP_INFO        lpFile,
   BOOL                   bWrtExtraData
)

{
   ZIP_ZIP_HEADR fhHeadr;
   LONG          eErr;
   WORD          wLen    = (WORD)FsvStr_GetByteCount( lpFile->szName );

   // Save new file position of the compressed file header.
   lpFile->lCompHeadrStrt = FsvBufIO_fTell( *lplpOutpt );
   lpFile->bDirty         = FALSE;
   lpFile->wDiskNum       = lpH->lpDir->SI.wCurDisk;

   // Construct file header data.
   fhHeadr.lFileSignature = ZIP_FILE_SIGNATURE;
   Zip_SetSharedData( lpFile, &fhHeadr.shData, ( WORD ) lpFile->dbExtra1.lSize );

   if ( ( eErr = Zip_fArchiveWrt( lpH, lplpOutpt, &fhHeadr, sizeof( fhHeadr ) ) ) == sizeof( fhHeadr ) &&
        ( eErr = Zip_WriteZipPath( lpH, lplpOutpt, lpFile->lpszPath ) ) == ZIP_NO_ERROR &&
        ( eErr = Zip_fArchiveWrt( lpH, lplpOutpt, lpFile->szName, wLen ) ) == ( LONG ) wLen )
   {
      if ( bWrtExtraData )
      {
         eErr = Zip_CopyDataBlk( lpH, &lpFile->dbExtra1, *lplpOutpt, FALSE );
      }
      else
      {
         eErr = ZIP_NO_ERROR;
      }
   }

   return( eErr );
} // Zip_WriteZipHeadr
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteCntrlHeadr( lpH, lplpOutpt, lpFile, lpwByts )
//
//    lpH       - Pointer to the information structure used to process a
//                connection to an archive file.
//    lplpOutpt - Pointer to the pointer to the structure used to buffer
//                i/o to a file or memory buffer.  When compressing a file
//                into an archive file if the archive file is closed and
//                re-opened because another diskette had to be inserted
//                then *lplpOutpt will be updated.
//    lpFile    - Pointer to the structure that contains the compressed
//                file info.
//    lpwByts   - Pointer to the variable that will contain the size (in bytes)
//                of the Central Directory entry that was created.
//
// This function writes out the ZIP central directory header, the path and
// file name, the "extra" data, and the "remark" data.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteCntrlHeadr(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpOutpt,
   LPZIP_COMP_INFO        lpFile,
   LPWORD                 lpwByts
)

{
   ZIP_CDIR_REC cdHeadr;
   ZIP_ERRCODE  eErr;
   WORD         wLen      = (WORD)FsvStr_GetByteCount( lpFile->szName );
   WORD         wExtraSiz = ( lpH->lpDir->SI.fIsSplit ) ? 0 : ( WORD ) lpFile->dbExtra1.lSize;

   // Construct central directory header data.
   Zip_SetSharedData( lpFile, &cdHeadr.shData, wExtraSiz );
   cdHeadr.lDirSignature  = ZIP_DIR_SIGNATURE;
   cdHeadr.bProgMadeVersn = 20; // JJS. 12/01/93. Use 2.0 ID.
   cdHeadr.bOSTyp         = 0;
// cdHeadr.wRmrkSiz       = ( WORD ) lpFile->dbRemark.lSize;
   cdHeadr.wRmrkSiz       = Zip_GetRemarkLen( lpFile->lpszRemark );
   cdHeadr.wDiskNum       = lpFile->wDiskNum;
   cdHeadr.wIntAttr       = 0;
   cdHeadr.lExtAttr       = ( LONG ) lpFile->wAttrs; // Without the caste the compiler (C7)
                                                     //  was assigning a value of zero.
   cdHeadr.lCompHeadrStrt = lpFile->lCompHeadrStrt;

   // Try to write the central directory header, the full name, the
   // extra data, and the remark data.
   if ( ( eErr = Zip_fArchiveWrt( lpH, lplpOutpt, &cdHeadr, sizeof( cdHeadr ) ) ) == sizeof( cdHeadr ) &&
        ( eErr = Zip_WriteZipPath( lpH, lplpOutpt, lpFile->lpszPath ) ) == ZIP_NO_ERROR &&
        ( eErr = Zip_fArchiveWrt( lpH, lplpOutpt, lpFile->szName, wLen ) ) == ( LONG ) wLen &&
        ( lpH->lpDir->SI.fIsSplit || ( eErr = Zip_CopyDataBlk( lpH, &lpFile->dbExtra1, *lplpOutpt, FALSE ) ) == ZIP_NO_ERROR ) )
   {
//    eErr = Zip_CopyDataBlk( lpH, &lpFile->dbRemark, lpOutpt, FALSE );
      if ( ( eErr = Zip_fArchiveWrt( lpH, lplpOutpt, lpFile->lpszRemark, cdHeadr.wRmrkSiz ) ) == ( LONG ) cdHeadr.wRmrkSiz )
      {
         eErr = ZIP_NO_ERROR;
      }
   }

   if ( lpwByts != NULL )
   // Return size of Central Directory entry.
   {
      *lpwByts = sizeof( cdHeadr ) + FsvStr_GetByteCount( lpFile->lpszPath ) + wLen + wExtraSiz + cdHeadr.wRmrkSiz;
   }

   return( eErr );
} // Zip_WriteCntrlHeadr
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteZipEnd( lpH, lplpOutpt, lplpFiles )
//
//    lpH       - Pointer to the information structure used to process a
//                connection to an archive file.
//    lplpOutpt - Pointer to the pointer to the structure used to buffer
//                i/o to a file or memory buffer.  When compressing a file
//                into an archive file if the archive file is closed and
//                re-opened because another diskette had to be inserted
//                then *lplpOutpt will be updated.
//    lplpFiles - Pointer to the list of file info pointers.
//
// This function writes out the central directory and initializes and writes
// the trailer ZIP record and the final remark text (if it exists).
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteZipEnd(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpOutpt,
   LPLPZIP_COMP_INFO      lplpFiles
)

{
   ZIP_ZIP_END_REC   trHeadr;
   WORD              wIndx;
   ZIP_ERRCODE       eErr;
   WORD              wByts;
   LPZIP_ARCHIVE_DIR lpDir = lpH->lpDir;

   // Save info used to keep track of the Central directory.
   trHeadr.wStartDisk     = lpDir->SI.wStrtDisk;
   trHeadr.lCntrlDirStart = FsvBufIO_fTell( *lplpOutpt );

   // Loop to write out the central directory.
   trHeadr.lCntrlDirSize = 0;
   for ( wIndx = lpDir->wFileCount; wIndx > 0; wIndx--, lplpFiles++ )
   {
      if ( ( eErr = Zip_WriteCntrlHeadr( lpH, lplpOutpt, lplpFiles[ 0 ], &wByts ) ) != ZIP_NO_ERROR )
      {
         return( eErr );
      }

      trHeadr.lCntrlDirSize += ( LONG ) wByts;
   }

   // Construct ZIP trailer record.
   trHeadr.lEndSignature  = ZIP_END_SIGNATURE;
   trHeadr.wDiskNum       = lpDir->SI.wCurDisk;
   trHeadr.wEntries       = lpDir->wFileCount;
   trHeadr.wTotalEntries  = lpDir->wFileCount;
// trHeadr.wRmrkSiz       = ( WORD ) lpDir->dbRemark.lSize;
   trHeadr.wRmrkSiz       = Zip_GetRemarkLen( lpDir->lpszRemark );

   // Write out the trailer and the optional "remark" data.
   if ( ( eErr = Zip_fArchiveWrt( lpH, lplpOutpt, &trHeadr, sizeof( trHeadr) ) ) == sizeof( trHeadr ) )
   {
//    eErr = Zip_CopyDataBlk( lpH, &lpDir->dbRemark, lpOutpt, FALSE );
      if ( ( eErr = Zip_fArchiveWrt( lpH, lplpOutpt, lpDir->lpszRemark, trHeadr.wRmrkSiz ) ) == ( LONG ) trHeadr.wRmrkSiz )
      {
         eErr = ZIP_NO_ERROR;
      }
   }

   // Zip_fFlush() could have incorrectly modified lpDir->SI.wStrtDisk.
   lpDir->SI.wStrtDisk    = trHeadr.wStartDisk;
   lpDir->SI.lCntrlDirOff = trHeadr.lCntrlDirStart;

   return( eErr );
} // Zip_WriteZipEnd
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteArcPakHeadr( lpH, lpFile )
//
//    lpH    - Pointer to the information structure used to process a
//             connection to an archive file.
//    lpFile - Pointer to the structure that contains the compressed
//             file info.
//
// This function writes out the compressed info and file name for an ARC
// or PAK file.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteArcPakHeadr(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_COMP_INFO   lpFile
)

{
   ZIP_ARC_HEADR        hFileHeadr;
   ZIP_ERRCODE          eErr;
   LPFSVBUFIO_FBUF_INFO lpOutpt = lpH->lpDir->fiArchvFile.lpFBuf;

   /* Store the new file position for the start of the compressed */
   /* info header.                                                */
   lpFile->lCompHeadrStrt = FsvBufIO_fTell( lpOutpt );
   lpFile->bDirty         = FALSE;

   /* Set the header fields. */
   hFileHeadr.bMarker     = (TCHAR) ZIP_ARC_MARKER;
   hFileHeadr.lComprsdSiz = lpFile->dbComprssdFile.lSize;
   hFileHeadr.diDate      = lpFile->diDate;
   hFileHeadr.tiTime      = lpFile->tiTime;
   hFileHeadr.wCRC        = ( WORD ) lpFile->lCRC;
   hFileHeadr.lOrigSiz    = lpFile->lOrigSiz;
   if (FsvUtil_IsLFN( lpFile->szName ))
   {
      LPTSTR      lpszExt;
      LPTSTR      lpszWork;
      int         iChar;

      // find the "extension" part after the last dot
      lpszExt = FsvStr_RevStr( lpFile->szName, 1 );
      while (lpszExt > lpFile->szName && *lpszExt != '.')
         lpszExt = CharPrev( lpFile->szName, lpszExt );

      // if this is the beginning or not a dot, move extension ptr to end
      if (lpszExt <= lpFile->szName || *lpszExt != '.')
         lpszExt = FsvStr_RevStr( lpFile->szName, 0 );

      // start copying from start of the string
      lpszWork = lpFile->szName;
      iChar = 0;

      // move forward until we hit the end
      while (*lpszWork)
      {
         TCHAR       tc;
         BOOL        fValidChar;

         // if we've already copied 8 chars, move to the extension part
         if (iChar == 8)
            lpszWork = lpszExt;

         // get current character
         tc = *lpszWork;

         // quit if we accidentally reached the end
         if (tc == 0 || iChar > 11)
            break;

         // convert spaces to underscores
         if (tc == ' ')
            tc = '_';

         // determine if we can use this character
         switch (tc)
         {
            case '\'':
            case '!':
            case '@':
            case '#':
            case '$':
            case '%':
            case '^':
            case '&':
            case '(':
            case ')':
            case '-':
            case '{':
            case '}':
            case '`':
            case '_':
            case '~':
               fValidChar = TRUE;
            break;

            default:
               fValidChar = IsCharAlphaNumeric(tc);
            break;
         }

         // special case for the dot at the extension
         if (lpszWork == lpszExt && tc == '.')
         {
            fValidChar = TRUE;
            iChar = 8;
         }

         // do we copy this character?
         if (fValidChar)
         {
            // store character
            hFileHeadr.szName[iChar++] = tc;
         }

         // advance to next source character
         lpszWork = CharNext( lpszWork );
      }

      // null-terminate the string (for good measure)
      hFileHeadr.szName[iChar] = 0;
   }
   else
   {
      lstrcpy( hFileHeadr.szName, lpFile->szName);
   }

   // convert file name to uppercase
//   CharUpper( hFileHeadr.szName );

   /* Convert the generic compresion type. */
   switch ( lpFile->ePckTyp )
   {
      case ZIP_NONE:
         hFileHeadr.bPckTyp = 2;
      break;

      case ZIP_RLE:
         hFileHeadr.bPckTyp = 3;
      break;

      case ZIP_SQUEEZED:
         hFileHeadr.bPckTyp = 4;
      break;

      case ZIP_CRUNCH:
         hFileHeadr.bPckTyp = ( BYTE ) ( lpFile->wPckFlgs + 5 );
      break;

      case ZIP_SQUASH:
         hFileHeadr.bPckTyp = 9;
      break;

      case ZIP_CRUSH:
         hFileHeadr.bPckTyp = 10;
      break;

#if USE_ARCPAK
      case ZIP_DISTILL:
         hFileHeadr.bPckTyp = 11;
      break;
#endif

      /* Should never happen. */
      default:
         hFileHeadr.bPckTyp = 255;
      break;
   }

   if ( ( eErr = FsvBufIO_fWrite( lpOutpt, &hFileHeadr, sizeof( hFileHeadr), lpH->lpCritCBFunc,
                                  lpH->dwCritCBData ) ) == sizeof( hFileHeadr ) )
   {
      eErr = ZIP_NO_ERROR;
   }

   return( eErr );
} /* Zip_WriteArcPakHeadr */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteArcPakDataBlk( lpH, lpDataBlk, wFileNum, bRecTyp )
//
//    lpH       - Pointer to the information structure used to process a
//                connection to an archive file.
//    lpDataBlk - Pointer to the block of data that needs to be copied.
//    wFileNum  - The one-based index of the compressed file that the
//                information pertains to.
//    wRecTyp   - The type of data to be stored in the special record (remark
//                or extra).
//
// This function, if necessary, writes out a special information header and
// copies the specified block of data from one place in the file to another.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteArcPakDataBlk(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_DATA_BLK    lpDataBlk,
   WORD              wFileNum,
   BYTE              bRecTyp
)

{
   ZIP_ERRCODE          eErr    = ZIP_NO_ERROR;
   LPFSVBUFIO_FBUF_INFO lpOutpt = lpH->lpDir->fiArchvFile.lpFBuf;

   if ( lpDataBlk->lSize > 0 )
   {
      ZIP_PAK_HEADR hPakHeadr;

      /* Init the special record header. */
      hPakHeadr.bMarker  = ZIP_PAK_MARKER;
      hPakHeadr.bRecType = bRecTyp;
      hPakHeadr.wFileNum = wFileNum;
      hPakHeadr.lDataSiz = lpDataBlk->lSize;

      /* Try to write the header and copy the block of data. */
      if ( ( eErr = FsvBufIO_fWrite( lpOutpt, &hPakHeadr, sizeof( hPakHeadr ),
                                     lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == sizeof( hPakHeadr ) )
      {
         eErr = Zip_CopyDataBlk( lpH, lpDataBlk, lpOutpt, FALSE );
      }
   }

   return( eErr );
} /* Zip_WriteArcPakDataBlk */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteArcPakDataStr( lpH, lpDataBlk, wFileNum, bRecTyp )
//
//    lpH       - Pointer to the information structure used to process a
//                connection to an archive file.
//    lpszData  - Pointer to the "string" that needs to be copied.
//    wFileNum  - The one-based index of the compressed file that the
//                information pertains to.
//    wRecTyp   - The type of data to be stored in the special record (remark
//                or extra).
//
// This function, if necessary, writes out a special information header
// followed by a string (excluding NULL terminator).
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteArcPakDataStr(
   LPZIP_HANDLE_INFO lpH,
   LPCTSTR            lpszData,
   WORD              wFileNum,
   BYTE              bRecTyp
)

{
   ZIP_ERRCODE          eErr    = ZIP_NO_ERROR;
   LPFSVBUFIO_FBUF_INFO lpOutpt = lpH->lpDir->fiArchvFile.lpFBuf;
   WORD                 wLen    = Zip_GetRemarkLen( lpszData );

   if ( wLen > 0 )
   {
      ZIP_PAK_HEADR hPakHeadr;

      // Init the special record header.
      hPakHeadr.bMarker  = ZIP_PAK_MARKER;
      hPakHeadr.bRecType = bRecTyp;
      hPakHeadr.wFileNum = wFileNum;
      hPakHeadr.lDataSiz = wLen;

      // Try to write the header and copy the string of data.
      if ( ( eErr = FsvBufIO_fWriteByts( lpOutpt, &hPakHeadr, sizeof( hPakHeadr ),
                                         lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == ZIP_NO_ERROR )
      {
         eErr = FsvBufIO_fWriteByts( lpOutpt, lpszData, wLen, lpH->lpCritCBFunc,
                                     lpH->dwCritCBData );
      }
   }

   return( eErr );
}
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteArcPakPath( lpH, lpszPath, wFileNum )
//
//    lpH      - Pointer to the information structure used to process a
//               connection to an archive file.
//    lpszPath - Pointer to the path to write out.
//    wFileNum - The one-based index of the compressed file that the
//               information pertains to.
//
// This function, if necessary, writes out a special information header and
// writes out the specified path.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteArcPakPath(
   LPZIP_HANDLE_INFO lpH,
   LPCTSTR            lpszPath,
   WORD              wFileNum
)

{
   ZIP_ERRCODE          eErr    = ZIP_NO_ERROR;
   WORD                 wLen    = (WORD)FsvStr_GetByteCount( lpszPath );
   LPFSVBUFIO_FBUF_INFO lpOutpt = lpH->lpDir->fiArchvFile.lpFBuf;

   if ( wLen > 0 )
   {
      ZIP_PAK_HEADR hPakHeadr;

      /* Init the special record header. */
      hPakHeadr.bMarker  = ZIP_PAK_MARKER;
      hPakHeadr.bRecType = ZIP_PAK_PATH_DATA;
      hPakHeadr.wFileNum = wFileNum;
      hPakHeadr.lDataSiz = wLen;

      /* Try to write the header and the path. */
      if ( ( eErr = FsvBufIO_fWrite( lpOutpt, &hPakHeadr, sizeof( hPakHeadr ), lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == sizeof( hPakHeadr ) &&
           ( eErr = FsvBufIO_fWrite( lpOutpt, lpszPath, wLen, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == ( LONG ) wLen )
      {
         eErr = ZIP_NO_ERROR;
      }
   }

   return( eErr );
} /* Zip_WriteArcPakPath */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteArcPakEnd( lpDir, lplpFiles )
//
//    lpH       - Pointer to the information structure used to process a
//                connection to an archive file.
//    lplpFiles - Pointer to the list of file info pointers.
//
// This function writes out the ARC end of file record, and if necessary,
// special data records for the remark and the extra data affiliated with
// the archive file.  Also, if necessary, it writes out special data records
// for the remark, the extra data, and the path affiliated with all of the
// compressed files.  Finally, it writes a PAK end of file record.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteArcPakEnd(
   LPZIP_HANDLE_INFO lpH,
   LPLPZIP_COMP_INFO lplpFiles
)

{
   ZIP_ERRCODE          eErr;
   ZIP_ARC_HEADR_STRT   hEndHeadr;
   WORD                 wIndx;
   LPZIP_ARCHIVE_DIR    lpDir   = lpH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpOutpt = lpDir->fiArchvFile.lpFBuf;

   /* Construct the ARC end of file record.  The following */
   /* special info records are optional for ARC/PAK files. */
   hEndHeadr.bMarker = ZIP_ARC_MARKER;
   hEndHeadr.bType   = ZIP_EOF;

   /* Try to write the end of file record and (if necessary) the */
   /* "extra data" and the "remark" for the archive file.        */
   if ( ( eErr = FsvBufIO_fWrite( lpOutpt, &hEndHeadr, sizeof( hEndHeadr ), lpH->lpCritCBFunc, lpH->dwCritCBData ) ) != sizeof( hEndHeadr ) ||
//      ( eErr = Zip_WriteArcPakDataBlk( lpH, &lpDir->dbRemark, 0, ZIP_PAK_REMARK_DATA ) ) != ZIP_NO_ERROR ||
        ( eErr = Zip_WriteArcPakDataStr( lpH, lpDir->lpszRemark, 0, ZIP_PAK_REMARK_DATA ) ) != ZIP_NO_ERROR ||
        ( eErr = Zip_WriteArcPakDataBlk( lpH, &lpDir->dbExtra,  0, ZIP_PAK_EXTRA_DATA ) ) != ZIP_NO_ERROR )
   {
      return( eErr );
   }

   /* Loop to optionally write out the path, the remark, and/or extra */
   /* data for all of the compressed files store in the archive.      */
   for ( wIndx = 1; wIndx <= lpDir->wFileCount; wIndx++, lplpFiles++ )
   {
      if ( ( eErr = Zip_WriteArcPakPath( lpH, lplpFiles[ 0 ]->lpszPath, wIndx ) ) != ZIP_NO_ERROR ||
//         ( eErr = Zip_WriteArcPakDataBlk( lpH, &lplpFiles[ 0 ]->dbRemark, wIndx, ZIP_PAK_REMARK_DATA ) ) != ZIP_NO_ERROR ||
           ( eErr = Zip_WriteArcPakDataStr( lpH, lplpFiles[ 0 ]->lpszRemark, wIndx, ZIP_PAK_REMARK_DATA ) ) != ZIP_NO_ERROR ||
           ( eErr = Zip_WriteArcPakDataBlk( lpH, &lplpFiles[ 0 ]->dbExtra1, wIndx, ZIP_PAK_EXTRA_DATA ) ) != ZIP_NO_ERROR )
      {
         return( eErr );
      }
   }

   /* Construct the PAK end of file record. */
   hEndHeadr.bMarker = ZIP_PAK_MARKER;

   /* Try to write the PAK end of file record. */
   if ( ( eErr = FsvBufIO_fWrite( lpOutpt, &hEndHeadr, sizeof( hEndHeadr ), lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == sizeof( hEndHeadr ) )
   {
      eErr = ZIP_NO_ERROR;
   }

   return( eErr );
} /* Zip_WriteArcPakEnd */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteLHAHeadr( lpH, lpFile )
//
//    lpH    - Pointer to the information structure used to process a
//             connection to an archive file.
//    lpFile - Pointer to the structure that contains the compressed
//             file info.
//
// This function writes out the compressed info and file name for an LZH
// compressed file.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteLHAHeadr(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_COMP_INFO   lpFile
)

{
   ZIP_LHA_FILE_HEADR   hHeadr;
   ZIP_ERRCODE          eErr;
   LPWORD               lpwTmp;
   WORD                 wLevel0Byts;
   WORD                 wCRC         = 0;
   WORD                 wPathLen     = 0;
   LPFSVBUFIO_FBUF_INFO lpOutpt      = lpH->lpDir->fiArchvFile.lpFBuf;
   BOOL                 fWrteAttrRec = ( lpFile->wAttrs != FSV_ATTRIB_ARCHIVE && lpFile->bLevel == 1 );
   WORD                 wRecLen      = 0;

   // Store the new file position for the start of the compressed
   // info header.
   lpFile->lCompHeadrStrt = FsvBufIO_fTell( lpOutpt );
   lpFile->bDirty         = FALSE;
   lpFile->wSpclRecByts   = 0;

   // Set the header fields.
   hHeadr.szCompMethod[ 0 ] = '-';
   hHeadr.szCompMethod[ 1 ] = 'l';
   hHeadr.szCompMethod[ 2 ] = 'h';
   hHeadr.szCompMethod[ 3 ] = '0' + ( TCHAR ) lpFile->wPckFlgs;
   hHeadr.szCompMethod[ 4 ] = '-';
   hHeadr.lComprsdSiz       = lpFile->dbComprssdFile.lSize;
   hHeadr.lOrigSiz          = lpFile->lOrigSiz;
   hHeadr.tiTime            = lpFile->tiTime;
   hHeadr.diDate            = lpFile->diDate;
   hHeadr.bLevel            = lpFile->bLevel;

   // For level 0 compressed files store the DOS attribute.  For level 1
   // compressed files always store 0x20 (don't ask me why).
   hHeadr.bFileAttr = ( hHeadr.bLevel == 0 ) ? ( BYTE ) lpFile->wAttrs : 0x20;

   if ( hHeadr.bLevel == 0 )
   // Store the path with the file name.
   {
      hHeadr.bLen = (BYTE)FsvStr_GetByteCount( lpFile->lpszPath );
      _fmemcpy( hHeadr.szFileName, lpFile->lpszPath, hHeadr.bLen );
      lstrcpy( &hHeadr.szFileName[ hHeadr.bLen ], lpFile->szName );
      hHeadr.bLen += (BYTE)FsvStr_GetByteCount( lpFile->szName );
   }
   else
   // Just store the file name.
   {
      hHeadr.bLen = (BYTE)FsvStr_GetByteCount( lpFile->szName );
      lstrcpy( hHeadr.szFileName, lpFile->szName );
   }

   // Compute and store the header size.
   if ( hHeadr.bLevel == 0 )
   {
      hHeadr.bHeadrSiz = ( ZIP_LHA_BASE_HEADR_BYTS - 2 ) + hHeadr.bLen + 2;
      wLevel0Byts      = 2;
   }
   else
   {
      hHeadr.bHeadrSiz = ( ZIP_LHA_BASE_HEADR_BYTS - 2 ) + hHeadr.bLen + 5;
      wLevel0Byts      = 0;
   }

   // Store the CRC after the file name.
   lpwTmp  = ( LPWORD ) ( hHeadr.szFileName + hHeadr.bLen );
   *lpwTmp = ( WORD ) lpFile->lCRC;

   // Set the platform ID to MS-DOS (this doesn't affect level 0 files).
   lpwTmp++;
   *( ( LPBYTE ) lpwTmp ) = 'M';

   if ( hHeadr.bLevel == 1 && lpFile->lpszPath != NULL && ( wPathLen = (WORD)FsvStr_GetByteCount( lpFile->lpszPath ) ) != 0 )
   // Account for the length of the extended header record that will
   // be used to contain the path.
   {
      wRecLen              = 3 + wPathLen;
      lpFile->wSpclRecByts = wRecLen;
   }

   if ( fWrteAttrRec )
   // Account for the length of the extended header record that will
   // be used to contain the special attribute record.
   {
      wRecLen               = ( wRecLen == 0 ) ? 3 + sizeof( WORD ) : wRecLen;
      lpFile->wSpclRecByts += ( 3 + sizeof( WORD ) );
   }

   if ( lpFile->wSpclRecByts > 0 )
   // Add the CRC record that will be used to verify the normal header
   // and the extended header records.
   {
      lpFile->wSpclRecByts += 5;
   }

   // Store the length of the 1st extended header record so that the CheckSum
   // will be computed correctly (this doesn't affect level 0 files).
   ( ( LPBYTE ) lpwTmp )++;
   *lpwTmp = wRecLen;

   // All of the extended header record bytes are considered to be part of
   // the compressed data.
   hHeadr.lComprsdSiz += lpFile->wSpclRecByts;

   if ( lpFile->wUnknownByts > 0 )
   // Handle this stupid case.
   {
      lpwTmp++;
      _fmemcpy( lpwTmp, lpFile->abUnknwnByts, lpFile->wUnknownByts );
      hHeadr.bHeadrSiz += ( BYTE ) lpFile->wUnknownByts;
   }

   // Compute the check-sum for the initial header info.
   hHeadr.bHeadrChkSum = Zip_CalcLHAByteChkSum( &hHeadr.szCompMethod, hHeadr.bHeadrSiz, 0 );

   // Start computing the CRC for the entire level-1 header.
   wCRC = crc( ( LPTSTR ) &hHeadr, hHeadr.bHeadrSiz, 0 );

   // Try to write out the initial file header.
   if ( ( eErr = FsvBufIO_fWriteByts( lpOutpt, &hHeadr, (WORD)(hHeadr.bHeadrSiz + wLevel0Byts),
                                      lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == ZIP_NO_ERROR &&
        wPathLen != 0 )
   // Store the extended header record for the path.
   {
      LPBYTE lpbTmp = ( ( LPBYTE ) &hHeadr ) + 2;
      WORD   wIndx  = wPathLen;

      // Store the length of the path record.
      ( ( LPWORD ) &hHeadr )[ 0 ] = wPathLen + 3;

      // Store the ID and the path.
      lpbTmp[ 0 ] = 0x02;
      _fmemcpy( &lpbTmp[ 1 ], lpFile->lpszPath, wPathLen );

      // Convert a backslash TO 0xFF.
      for ( lpbTmp++; wIndx > 0; lpbTmp = CharNext(lpbTmp), wIndx-- )
      {
         if (IsDBCSLeadByte(*lpbTmp))
            wIndx--;

         if ( lpbTmp[ 0 ] == '\\' )
         {
            lpbTmp[ 0 ] = 0xFF;
         }
      }

      // Continue computing the CRC for the entire level-1 header.
      wCRC = crc( ( LPTSTR ) &hHeadr, (WORD)(wPathLen + 3), wCRC );

      // Try to write out path record.
      eErr = FsvBufIO_fWriteByts( lpOutpt, &hHeadr, (WORD)(wPathLen + 3), lpH->lpCritCBFunc, lpH->dwCritCBData );
   }

   if ( fWrteAttrRec && eErr == ZIP_NO_ERROR )
   // Try to write out the extended header attribute record.
   {

      // Store the length of the attribute record.
      ( ( LPWORD ) &hHeadr )[ 0 ] = ( 3 + sizeof( WORD ) );

      // Store record ID and the attribute.
      ( ( LPBYTE ) &hHeadr + 2 )[ 0 ]                = 0x40;
      ( ( LPWORD ) ( ( LPBYTE ) &hHeadr + 3 ) )[ 0 ] = lpFile->wAttrs;

      // Continue computing the CRC for the entire level-1 header.
      wCRC = crc( ( LPTSTR ) &hHeadr, 5, wCRC );

      // Try to write out attribute record.
      eErr = FsvBufIO_fWriteByts( lpOutpt, &hHeadr, 5, lpH->lpCritCBFunc, lpH->dwCritCBData );
   }

   if ( lpFile->wSpclRecByts > 0 && eErr == ZIP_NO_ERROR )
   // Try to write out the extended header CRC record.
   {
      // Store the length of the CRC record.
      ( ( LPWORD ) &hHeadr )[ 0 ] = ( 3 + sizeof( WORD ) );

      // Store record ID and the attribute (always use a value of 0 for the
      // CRC that has yet to be computed).
      ( ( LPBYTE ) &hHeadr + 2 )[ 0 ]                = 0x00;
      ( ( LPWORD ) ( ( LPBYTE ) &hHeadr + 3 ) )[ 0 ] = 0;

      // Continue computing the CRC for the entire level-1 header (always
      // use a value of 0 for the CRC that has yet to be computed).
      wCRC = crc( ( LPTSTR ) &hHeadr, 5, wCRC );

      // Continue computing the CRC for the entire level-1 header.  Account
      // for the NULL extended header record.
      wRecLen = 0;
      wCRC = crc( ( LPTSTR ) &wRecLen, 2, wCRC );

      // Store the CRC for the entire level-1 header.
      ( ( LPWORD ) ( ( LPBYTE ) &hHeadr + 3 ) )[ 0 ] = wCRC;

      // Try to write out CRC record.
      eErr = FsvBufIO_fWriteByts( lpOutpt, &hHeadr, 5, lpH->lpCritCBFunc, lpH->dwCritCBData );
   }

   if ( lpFile->bLevel == 1 && eErr == ZIP_NO_ERROR )
   // Try to write the out a NULL extended header record (end of list).
   {
      ( ( LPWORD ) &hHeadr )[ 0 ] = 0;
      eErr = FsvBufIO_fWriteByts( lpOutpt, &hHeadr, 2, lpH->lpCritCBFunc, lpH->dwCritCBData );
   }

   return( eErr );
} // Zip_WriteLHAHeadr
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteFileHeadr( lpH, lplpOutpt, lpFile, bWrtExtra )
//
//    lpH       - Pointer to the information structure used to process a
//                connection to an archive file.
//    lplpOutpt - Pointer to the pointer to the structure used to buffer
//                i/o to a file or memory buffer.  When compressing a file
//                into an archive file if the archive file is closed and
//                re-opened because another diskette had to be inserted
//                then *lplpOutpt will be updated.
//    lpFile    - Pointer to the info for the compressed file in question.
//    bWrtExtra - If this flag is TRUE then write out the extra data after
//                the header data.  This flag only applies to ZIP files.
//
// This function writes out a compressed file header for a ZIP or an ARC/PAK
// archive file.  If this function detects that the file header information
// in the disk file has not been modified and that the file has not been
// "moved" then it will simply advance the file pointer by the appropriate
// amount.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteFileHeadr(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpOutpt,
   LPZIP_COMP_INFO        lpFile,
   BOOL                   bWrtExtra
)

{
   ZIP_ERRCODE       eErr;
   LPZIP_ARCHIVE_DIR lpDir     = lpH->lpDir;
   LONG              lCurPostn = FsvBufIO_fTell( Zip_GetFBufInfo( lpDir, lpFile->dbComprssdFile.hbFleIndx ) );

   if ( lpFile->bDirty || lCurPostn != lpFile->lCompHeadrStrt
        /* || Zip_GetFBufInfo(lpDir, lpFile->dbComprssdFile.hbFleIndx ) != lpDir->fiArchvFile.lpFBuf */ )
   // Write out the compressed file header.
   {
      if ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP )
      {
         eErr = Zip_WriteZipHeadr( lpH, lplpOutpt, lpFile, bWrtExtra );
      }
      else
      if ( lpDir->eArchvTyp == ZIP_FORMAT_LHA )
      {
         eErr = Zip_WriteLHAHeadr( lpH, lpFile );
      }
      else
      {
         eErr = Zip_WriteArcPakHeadr( lpH, lpFile );
      }

      lpFile->bDirty = FALSE;
   }
   else
   // Simply skip over the "intact" compressed file header.
   {
      if ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP )
      {
         lCurPostn += sizeof( ZIP_ZIP_HEADR ) + FsvStr_GetByteCount( lpFile->lpszPath ) +
                      FsvStr_GetByteCount( lpFile->szName ) + lpFile->dbExtra1.lSize;
      }
      else
      if ( lpDir->eArchvTyp == ZIP_FORMAT_LHA )
      {
         lCurPostn += Zip_CalcLZHHeadrSiz( lpFile->bLevel, ( BYTE ) FsvStr_GetByteCount( lpFile->szName ) ) +
                      2 + lpFile->wUnknownByts;

         if ( lpFile->bLevel == 0 )
         {
            lCurPostn += FsvStr_GetByteCount( lpFile->lpszPath );
         }
         else
         {
            lCurPostn += lpFile->wSpclRecByts;
         }
      }
      else
      {
         lCurPostn += sizeof( ZIP_ARC_HEADR );
      }

      if ( ( eErr = FsvBufIO_fSeek( lpDir->fiArchvFile.lpFBuf, lCurPostn, SEEK_SET,
                                    lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == lCurPostn )
      {
         eErr = ZIP_NO_ERROR;
      }
   }

   return( eErr );
} // Zip_WriteFileHeadr
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_WriteArchiveEnd( lpH, lplpOutpt, lplpFiles )
//
//    lpH       - Pointer to the information structure used to process a
//                connection to an archive file.
//    lplpOutpt - Pointer to the pointer to the structure used to buffer
//                i/o to a file or memory buffer.  When compressing a file
//                into an archive file if the archive file is closed and
//                re-opened because another diskette had to be inserted
//                then *lplpOutpt will be updated.
//    lplpFiles - Pointer to the list of file info pointers.
//
// This function dispatches the appropriate function to write out special
// "conclusion" info for the various archive file types.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_WriteArchiveEnd(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpOutpt,
   LPLPZIP_COMP_INFO      lplpFiles
)

{
   ZIP_ERRCODE eErr;

   if ( lpH->lpDir->eArchvTyp == ZIP_FORMAT_ZIP )
   {
      eErr = Zip_WriteZipEnd( lpH, lplpOutpt, lplpFiles );
   }
   else
   if ( lpH->lpDir->eArchvTyp == ZIP_FORMAT_ARC || lpH->lpDir->eArchvTyp == ZIP_FORMAT_PAK )
   {
      eErr = Zip_WriteArcPakEnd( lpH, lplpFiles );
   }
   else
   if ( lpH->lpDir->eArchvTyp == ZIP_FORMAT_LHA )
   {
      if ( lpH->lpDir->wFileCount > 0 )
      // For non-empty LZH files write out a header size of zero to indicate
      // that this is the end of the file.
      {
         BYTE bHeadrSiz = 0;
         eErr = FsvBufIO_fWriteByts( lpH->lpDir->fiArchvFile.lpFBuf, &bHeadrSiz, 1, lpH->lpCritCBFunc, lpH->dwCritCBData );
      }
      else
      {
         eErr = ZIP_NO_ERROR;
      }
   }
   else
   {
      eErr = ZIP_BAD_ARG;
   }

   return( eErr );
} // Zip_WriteArchiveEnd
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_MoveSpecialDataToEnd( lpH )
//
//    lpH - Pointer to the information structure used to process a
//          connection to an archive file.
//
// This function copies all of the "extra data" and "remark text" for the
// archives file and the compressed files to the end of the archive file.
// This is done to keep that data from getting overwritten while the files
// are "packed".
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_MoveSpecialDataToEnd(
   LPZIP_HANDLE_INFO lpH
)

{
   LPLPZIP_COMP_INFO lplpFiles;
   ZIP_ERRCODE       eErr;
   WORD              wIndx;
   LONG              lStart;
   LPZIP_ARCHIVE_DIR lpDir         = lpH->lpDir;
   LONG              lExtraRemByts = lpDir->SI.lExtraRemByts;

   if ( lExtraRemByts == 0 || lpDir->SI.fIsSplit ) //-*****FIX for multi-disk archive files.
   // No data to move or data is not moved for multi-disk archive files.
   {
      return( ZIP_NO_ERROR );
   }

   /* Compute starting location for temporary storage of the */
   /* extra/remark data.                                     */
   lStart = lpDir->SI.lAvailPostn + max( 0, lpDir->SI.lEOFByts - lpDir->SI.lFreedByts );

   /* Try to copy "extra data" and "remark text" for archive file. */
   if ( ( eErr = FsvBufIO_fSeek( lpDir->fiArchvFile.lpFBuf, lStart, SEEK_SET, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) != lStart ||
        ( eErr = Zip_CopyDataBlk( lpH, &lpDir->dbExtra, lpDir->fiArchvFile.lpFBuf, FALSE ) ) != ZIP_NO_ERROR /*||
        ( eErr = Zip_CopyDataBlk( lpH, &lpDir->dbRemark, lpDir->fiArchvFile.lpFBuf, FALSE ) ) != ZIP_NO_ERROR */ )
   {
      return( eErr );
   }

   /* Account for archive extra data and comment. */
// lExtraRemByts -= ( lpDir->dbExtra.lSize + lpDir->dbRemark.lSize );
   lExtraRemByts -= lpDir->dbExtra.lSize;

   /* Loop to move all "extra data" and "remark text" to */
   /* the end of the file.                               */
   for ( wIndx = lpDir->wFileCount, lplpFiles = lpDir->lplpCompFiles;
         wIndx > 0 /*&& lExtraRemByts > 0*/ && eErr == ZIP_NO_ERROR;
         wIndx--, lplpFiles++ )
   {
      /* Try to copy "extra data" and "remark text" for archive file. */
      if ( ( eErr = Zip_CopyDataBlk( lpH, &lplpFiles[0]->dbExtra1, lpDir->fiArchvFile.lpFBuf, FALSE ) ) != ZIP_NO_ERROR /*||
           ( eErr = Zip_CopyDataBlk( lpH, &lplpFiles[0]->dbRemark, lpDir->fiArchvFile.lpFBuf, FALSE ) ) != ZIP_NO_ERROR*/ )
      {
         break;
      }

      // Account for compressed file extra data and comment.
//    lExtraRemByts -= ( lplpFiles[0]->dbExtra1.lSize + lplpFiles[0]->dbRemark.lSize );
      lExtraRemByts -= lplpFiles[0]->dbExtra1.lSize;
   }

   return( eErr );
} /* Zip_MoveSpecialDataToEnd */
#endif          // NOT NAV95


/*---------------------------------------------------------*/
/*   A R C H I V E   E X P A N S I O N   R O U T I N E S   */
/*---------------------------------------------------------*/


//==========================================================================
//
// WORD Zip_CalcOldComprssdFileByts( lpFile )
//
//    lpFile - Pointer to the compressed file info.
//
// This function returns the number of bytes for the compressed file header
// and the compressed data for the various archive file types (ZIP, ZRC, PAK).
// This function only works for and a compressed file that already resides
// in the archive file and none of it's affiliate dynamic data (file name
// and extra data) has been modified.
//
//==========================================================================

LONG WINAPI Zip_CalcOldComprssdFileByts(
   LPZIP_COMP_INFO lpFile
)

{
   return( // Bogus for multi-disk archive files.
           /*lpFile->dbComprssdFile.lStart - lpFile->lCompHeadrStrt +*/
           lpFile->dbComprssdFile.lSize );
} /* Zip_CalcOldComprssdFileByts */



//==========================================================================
//
// WORD Zip_CalcEndInfoByts( lpDir, lpFile )
//
//    lpDir  - Pointer to the information structure used to process
//             an archive file.
//    lpFile - Pointer to the compressed file info.
//
// This function returns the number of bytes that will be written at
// the end of the archive file for the specified compressed file.  The
// number of bytes required is a function of the archive file type (ZIP
// verses ARC/PAK) and the amount of dynamic info associated with the
// file (extra data, remark data, path, etc.)
//
//==========================================================================

#ifndef NAV95
WORD WINAPI Zip_CalcEndInfoByts(
   LPZIP_ARCHIVE_DIR lpDir,
   LPZIP_COMP_INFO   lpFile
)

{
   WORD wByts;
   WORD wLen     = (WORD)FsvStr_GetByteCount( lpFile->lpszPath );
   WORD wRmrkLen = Zip_GetRemarkLen( lpFile->lpszRemark );

   if ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP )
   {
      wByts = sizeof( ZIP_CDIR_REC ) + FsvStr_GetByteCount( lpFile->szName ) + wLen +
//            ( WORD ) lpFile->dbExtra1.lSize + ( WORD ) lpFile->dbRemark.lSize;
              ( WORD ) lpFile->dbExtra1.lSize + wRmrkLen;
   }
   else
   if ( lpDir->eArchvTyp == ZIP_FORMAT_ARC || lpDir->eArchvTyp == ZIP_FORMAT_PAK )
   {
      wByts  = ( wLen == 0 )                   ? 0 : sizeof( ZIP_PAK_HEADR ) + wLen;
      wByts += ( lpFile->dbExtra1.lSize == 0 ) ? 0 : sizeof( ZIP_PAK_HEADR ) + ( WORD ) lpFile->dbExtra1.lSize;
//    wByts += ( lpFile->dbRemark.lSize == 0 ) ? 0 : sizeof( ZIP_PAK_HEADR ) + ( WORD ) lpFile->dbRemark.lSize;
      wByts += ( wRmrkLen == 0 )               ? 0 : sizeof( ZIP_PAK_HEADR ) + wRmrkLen;
   }
   else
   // LZH files do not have end of file info.
   {
      wByts = 0;
   }

   return( wByts );
} /* Zip_CalcEndInfoByts */
#endif          // NOT NAV95


//==========================================================================
//
// WORD Zip_CalcTrailrByts( lpDir )
//
//    lpDir - Pointer to the information structure used to process
//            an archive file.
//
// This function returns the number of bytes that will be written for the
// trailer record for the various archive file types.
//
//==========================================================================

#ifndef NAV95
WORD WINAPI Zip_CalcTrailrByts(
   LPZIP_ARCHIVE_DIR lpDir
)

{
   WORD wByts;
   WORD wRmrkLen = Zip_GetRemarkLen( lpDir->lpszRemark );

   if ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP )
   {
//    wByts = sizeof( ZIP_ZIP_END_REC ) + ( WORD ) lpDir->dbRemark.lSize;
      wByts = sizeof( ZIP_ZIP_END_REC ) + wRmrkLen;
   }
   else
   if ( lpDir->eArchvTyp == ZIP_FORMAT_ARC || lpDir->eArchvTyp == ZIP_FORMAT_PAK )
   {
      wByts  = sizeof( ZIP_ARC_HEADR_STRT ) * 2;
      wByts += ( lpDir->dbExtra.lSize  == 0 ) ? 0 : sizeof( ZIP_PAK_HEADR ) + ( WORD ) lpDir->dbExtra.lSize;
//    wByts += ( lpDir->dbRemark.lSize == 0 ) ? 0 : sizeof( ZIP_PAK_HEADR ) + ( WORD ) lpDir->dbRemark.lSize;
      wByts += ( wRmrkLen == 0 ) ? 0 : sizeof( ZIP_PAK_HEADR ) + wRmrkLen;
   }
   else
   // LZH files do not have any trailer info.
   {
      wByts = 0;
   }

   return( wByts );
} /* Zip_CalcTrailrByts */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_PACK_TYPE Zip_SelectPckTyp( eArchvTyp, lpFile )
//
//    eArchvTyp - The type of the archive file (ZIP, PAK, ARC).
//    lpFile    - Pointer to the compressed file info.
//
// This function returns the best compression algorithm to use.  It makes
// it decision based on the archive type and the size of the input file.
//
//==========================================================================

#ifndef NAV95
ZIP_PACK_TYPE WINAPI Zip_SelectPckTyp(
   ZIP_TYPE        eArchvTyp,
   LPZIP_COMP_INFO lpFile
)

{
   ZIP_PACK_TYPE ePckTyp;

   switch ( eArchvTyp )
   {
      case ZIP_FORMAT_ZIP:
//       if ( lpFile->lOrigSiz < 500 )
//       {
//          ePckTyp = ZIP_SHRINK;
//       }
//       else
         {
            ePckTyp = ZIP_DEFLATN;
         }
      break;

      case ZIP_FORMAT_PAK:
         if ( lpFile->lOrigSiz < 1500 )
         {
            ePckTyp = ZIP_CRUSH;
         }
#if USE_ARCPAK
         else
         {
            ePckTyp = ZIP_DISTILL;
         }
#else
         else
         {
            ePckTyp = ZIP_CRUSH;
         }
#endif
      break;

      case ZIP_FORMAT_ARC:
         ePckTyp = ZIP_SQUASH;
      break;

      case ZIP_FORMAT_LHA:
         ePckTyp = ZIP_LZH;
      break;
   }

   return( ePckTyp );
} /* Zip_SelectPckTyp */
#endif          // NOT NAV95


//==========================================================================
//
// BOOL Zip_IsCompMethComptbl( eSrcTyp, eDstTyp, eSrcPckTyp )
//
//    eSrcTyp    - The type of the source archive file.
//    eDstTyp    - The type of the destination archive file.
//    eSrcPckTyp - The compression method of the source compression file.
//
// This function returns TRUE if the compression method of a compressed file
// contained in a source archive file is supported by the destination archive
// file.
//
// Returns:
//    FALSE - Compression method is NOT supported.
//    TRUE  - Compression method IS supported.
//
//==========================================================================

#ifndef NAV95
BOOL WINAPI Zip_IsCompMethComptbl(
   ZIP_TYPE      eSrcTyp,
   ZIP_TYPE      eDstTyp,
   ZIP_PACK_TYPE eSrcPckTyp )

{
   return( ( eSrcTyp == eDstTyp ||
             ( eSrcPckTyp == ZIP_NONE && eSrcTyp != ZIP_FORMAT_ZIP && eDstTyp != ZIP_FORMAT_ZIP )
                  ||
             ( eSrcTyp == ZIP_FORMAT_ARC && eDstTyp == ZIP_FORMAT_PAK &&
               ( eSrcPckTyp == ZIP_CRUNCH   || eSrcPckTyp == ZIP_SQUASH ||
                 eSrcPckTyp == ZIP_SQUEEZED || eSrcPckTyp == ZIP_RLE ) )
                 ) );
} /* Zip_IsCompMethComptbl */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_VerifyPckTyp( eArchvTyp, ePckTyp )
//
//    eArchvTyp - The type of the archive file (ZIP, PAK, ARC).
//    ePackType - The type of compression algorithm to use.
//
// This function evaluate the specified compression method based on
// the specified archive type.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_BAD_ARG   - The specified
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_VerifyPckTyp(
   ZIP_TYPE      eArchvTyp,
   ZIP_PACK_TYPE ePckTyp
)

{
   ZIP_ERRCODE eErr = ZIP_BAD_ARG;

   // Evaluate compression method based on archive type.
   switch ( eArchvTyp )
   {
      case ZIP_FORMAT_ARC:
         if ( ePckTyp == ZIP_BEST   ||
              ePckTyp == ZIP_CRUNCH ||
              ePckTyp == ZIP_SQUASH )
         {
            eErr = ZIP_NO_ERROR;
         }
      break;

      case ZIP_FORMAT_PAK:
         if ( ePckTyp == ZIP_BEST    ||
              ePckTyp == ZIP_CRUNCH  ||
              ePckTyp == ZIP_SQUASH  ||
              ePckTyp == ZIP_CRUSH
#if USE_ARCPAK
              ||
              ePckTyp == ZIP_DISTILL
#endif
              )
         {
            eErr = ZIP_NO_ERROR;
         }
      break;

      case ZIP_FORMAT_LHA:
         if ( ePckTyp == ZIP_BEST ||
              ePckTyp == ZIP_LZH )
         {
            eErr = ZIP_NO_ERROR;
         }
      break;

      case ZIP_FORMAT_ZIP:
         if ( ePckTyp == ZIP_BEST    ||
              ePckTyp == ZIP_DEFLATN ||
              ePckTyp == ZIP_SHRINK  ||
              ePckTyp == ZIP_IMPLODE )
         {
            eErr = ZIP_NO_ERROR;
         }
      break;
   }

   return( eErr );
} // Zip_VerifyPckTyp



//==========================================================================
//
// ZIP_ERRCODE Zip_CreateTempFile( lpszPrefix, lpFileInfo, lOrigSiz,
//                                 lpErrCBFunc, dwErrCBData )
//
//    lpH         - Pointer to the information structure used to process a
//                  connection to an archive file.
//    lpszPrefix  - Prefix portion of the unique temporary file name that
//                  will be created.
//    lpFileInfo  - Pointer to info used to buffer i/o to the file that
//                  will be created.
//    lOrigSiz    - The original size of the file that is being compressed.
//                  This is passed in so that the temporary file will be
//                  created on a drive that will have enough room.
//
// This function will use the specified prefix to generate a unique file
// name.  The file name generated and created will reside on a drive that
//  has at least "lOrigSize" bytes available.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_CreateTempFile(
   LPZIP_HANDLE_INFO lpH,
   LPTSTR             lpszPrefix,
   LPZIP_BFILE_INFO  lpFileInfo,
   LONG              lOrigSiz
)

{
   long  lError;
   TCHAR  chJunk = 0; // This value is ignored in GetTempDrive().

   /* Construct a temporary file useing the specified prefix. */
   lError = FsvUtil_GetTempFileName( 3, lpszPrefix, 0, lpFileInfo->szFullName, lOrigSiz );

   if (lError == FSV_SUCCESS)
   {
      lError = FsvBufIO_fCreat( lpFileInfo->szFullName, 0, FSVBUFIO_FILE_BUF_SIZ, &lpFileInfo->lpFBuf,
                                lpH->lpCritCBFunc, lpH->dwCritCBData );

      if ( lError == FSVERR_SOURCE_PATH_NOT_FOUND )
      /* Within the confusion the song remains, you have no choice   */
      /* but to throw laughing Sam's dice, and pretend you are sane. */
      {
         TCHAR szDrvDir[ FSV_PATHBUF_LEN ];

         FsvUtil_GetPathMask( lpFileInfo->szFullName, szDrvDir, NULL, FALSE );

         if ( ( lError = FsvUtil_CreateMultipleDirs( szDrvDir, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == ZIP_NO_ERROR )
         /* The path has been recreated so try to create the file. */
         {
            lError = FsvBufIO_fCreat( lpFileInfo->szFullName, 0, FSVBUFIO_FILE_BUF_SIZ,
                                      &lpFileInfo->lpFBuf, lpH->lpCritCBFunc,
                                      lpH->dwCritCBData );
         }
      }
   }

   return lError;
} /* Zip_CreateTempFile */
#endif                                  // Not NAV95

//==========================================================================
//
// ZIP_ERRCODE Zip_ClosTempFile( lpH, lpFileInfo );
//
//    lpH        - Pointer to the information structure used to process a
//                 connection to an archive file.
//    lpFileInfo - Pointer to the...
//
// This function closes the temporary file (no flush) and deletes it.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_CloseTempFile(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_BFILE_INFO  lpFileInfo
)

{
   ZIP_ERRCODE eErr;
   ZIP_ERRCODE eTmpErr;

   eErr    = FsvBufIO_fClose( lpFileInfo->lpFBuf, FALSE, lpH->lpCritCBFunc, lpH->dwCritCBData );
   eTmpErr = FsvIO_DeleteFile( lpFileInfo->szFullName, FALSE, NULL, 0 );
   return( ( eErr != ZIP_NO_ERROR ) ? eErr : eTmpErr );
} /* Zip_CloseTempFile */
#endif                                  // Not NAV95


//==========================================================================
//
// BOOL Zip_Unfiltered( lpInBuf, lpwInBufSiz, lpOutBuf, lpwOutBufSiz,
//                      lpvVars, bEndOfInpt )
//
//    lpInBuf      -
//    lpwInBufSiz  -
//    lpOutBuf     -
//    lpwOutBufSiz -
//    lpvVars      -
//    bEndOfInpt   -
//
// This function is for files that are stored/retrieved in/from their "raw"
// state.  It simply copies the appropriate number of bytes from the
// input buffer to the output buffer.
//
// Returns:
//    FALSE - Keep on copying.
//    TRUE  - All input data has been copied.
//
//==========================================================================

BOOL WINAPI Zip_Unfiltered(
   LPTSTR  lpInBuf,
   LPWORD lpwInBufSiz,
   LPTSTR  lpOutBuf,
   LPWORD lpwOutBufSiz,
   LPVOID lpvVars,
   TCHAR   bEndOfInpt
)

{
   WORD wBytsToCopy = min( *lpwInBufSiz, *lpwOutBufSiz );
   BOOL bResult     = ( bEndOfInpt && wBytsToCopy == *lpwInBufSiz );

   lpwInBufSiz  -= wBytsToCopy;
   lpwOutBufSiz -= wBytsToCopy;

   _fmemcpy( lpOutBuf, lpInBuf, wBytsToCopy );

   return( bResult );
} /* Zip_Unfiltered */



//==========================================================================
//
// BOOL Zip_ExpRLEWrppr( lpInBuf, lpwInBufSiz, lpOutBuf, lpwOutBufSiz,
//                       lpvVars, bEndOfInpt )
//
//    lpInBuf      -
//    lpwInBufSiz  -
//    lpOutBuf     -
//    lpwOutBufSiz -
//    lpvVars      -
//    bEndOfInpt   -
//
// This function is a "wrapper" for the expandRLE() function so that it can
// be called via a function pointer and a generic parameter list.
//
// Returns:
//    FALSE - Keep on expanding.
//    TRUE  - All input data has been expanded.
//
//==========================================================================

BOOL WINAPI Zip_ExpRLEWrppr(
   LPTSTR  lpInBuf,
   LPWORD lpwInBufSiz,
   LPTSTR  lpOutBuf,
   LPWORD lpwOutBufSiz,
   LPVOID lpvVars,
   TCHAR   bEndOfInpt
)

{
   return( expandRLE( ( LPTSTR ) lpInBuf, lpwInBufSiz, ( LPTSTR ) lpOutBuf,
                      lpwOutBufSiz, bEndOfInpt ) );
} /* Zip_ExpRLEWrppr */



//==========================================================================
//
// ZIP_ERRCODE Zip_FilterFile( lpH, lplpInput, lplpOutpt, lpFltrVars )
//
//    lpH        - Pointer to the information structure used to process a
//                 connection to an archive file.
//    lplpInput  - Pointer to the pointer to the structure used to buffer
//                 i/o from a file or memory buffer.  When expanding an archive
//                 file if the archive file is closed and re-opened because
//                 another diskette had to be inserted then *lplpInput will
//                 be updated.
//    lplpOutpt  - Pointer to the pointer to the structure used to buffer
//                 i/o to a file or memory buffer.  When compressing a file
//                 into an archive file if the archive file is closed and
//                 re-opened because another diskette had to be inserted
//                 then *lplpOutpt will be updated.
//    lpFltrVars - Pointer to the generic "filter parameter" structure.
//
// This function is called to either read raw data and produce compressed
// output or to read compressed data and produce expanded output.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_NO_MEMORY - No memory available for the temporary buffers.
//    FSVERR...     - The appropriate error code returned by File Services
//                    ( i.e. file not found, disk i/o error, etc.)
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and create a new one on another diskette.  If this happens *lplpFBuf
//       (and lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_FilterFile(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpInput,
   LPLPFSVBUFIO_FBUF_INFO lplpOutpt,
   LPZIP_FILTER_INFO      lpFltrVars
)

{
   LPBYTE      lpInptBuf;
   LPBYTE      lpOutptBuf;
   WORD        wBytsRead;
   ZIP_ERRCODE eErr;
   BOOL        bDone;
   WORD        wInptByts;
   WORD        wOutptByts;
   BOOL        fNoGateBug = FALSE;
   WORD        wCurDisk;
   LONG        lCurPos;
   BOOL        fFrstRead     = TRUE;
   LONG        lNumInputByts = lpFltrVars->lNumInputByts;
   LONG        lMaxOutptByts = lpFltrVars->lMaxOutptByts;
   LONG        lOrigSiz      = max( 1L, lNumInputByts );
   LONG        lBytsPrcssd   = 0;
   WORD        wCurByts      = 0;

// /*****IMPLEMENT*****/
// /* Keep track of last input byte for ZipReadFile(). */
// lpDir->RFI.lComprsdStrt = FsvBufIO_fTell( lpInput );
// /*****IMPLEMENT*****/

   // JJS - 10/04/93.
   // There is a bug in the expandLZW() function that will cause it to
   // blow if the input buffer is not exhausted each time.
   fNoGateBug = ( lpFltrVars->lpFiltrFunc == expandLZW );

   if ( ( lpInptBuf = ( LPBYTE ) Mem_SubMalloc( ZIP_INPUT_BUFFER_SIZE ) ) == NULL )
   /* Bad news.  Unable to allocate the input buffer. */
   {
      return( ZIP_NO_MEMORY );
   }

   if ( ( lpOutptBuf = ( LPBYTE ) Mem_SubMalloc( ZIP_OUTPUT_BUFFER_SIZE ) ) == NULL )
   /* Bad news.  Unable to allocate the output buffer. */
   {
      Mem_SubFree( lpInptBuf );
      return( ZIP_NO_MEMORY );
   }

   if ( lpFltrVars->bComputeCRC )
   /* Init the CRC value for 16 and 32 bit. */
   {
      if ( lpFltrVars->bUseCRC32 )
      {
         lpFltrVars->lCRC = -1;
      }
      else
      {
         lpFltrVars->lCRC = 0;
      }
   }

   /* Outer loop used to read the input file. */
   do
   {
      BOOL   bEOF;
      LPBYTE lpTmpBuf;

      // Compute the maximum number of bytes to read (allow for 2 unused
      // bytes in the input buffer since some of the compression routines
      // access beyond the end of the buffer - don't ask me why).
      wBytsRead = ( WORD ) min( ( LONG ) ZIP_INPUT_CHARS - wCurByts, lNumInputByts );

      // Read next block from the input file.
      if ( lpFltrVars->bCompress )
      {
         eErr = lpFltrVars->lpReadFunc( *lplpInput, lpInptBuf + wCurByts, wBytsRead, lpH->lpCritCBFunc, lpH->dwCritCBData );
      }
      else
      {
         if ( !fFrstRead )
         // When expanding a "compressed file" it is possible that another
         // instance of File Manager is using lpH->lpDir->fiArchvFile.lpFBuf
         // and when Zip_FileFilter() yielded the file position was modified,
         // so I must reset the file position.
         {
            if ( ( eErr = Zip_fArchiveSeek( lpH, lplpInput, wCurDisk, lCurPos, SEEK_SET ) ) != lCurPos )
            {
               break;
            }
         }

         // Very slimey.  When epanding a compressed file I have to handle the
         // multi-disk archive files.  The expansion logic always uses
         // FsvBufIO_fRead() so I don't need to use lpFltrVars->lpReadFunc().
         eErr = Zip_fArchiveRead( lpH, lplpInput, lpInptBuf + wCurByts, wBytsRead );

         // When expanding a "compressed file" it is possible that another
         // instance of File Manager is using lpH->lpDir->fiArchvFile.lpFBuf
         // and when Zip_FileFilter() yielded the file position was modified,
         // so I must reset store the current file position.
         wCurDisk  = lpH->lpDir->SI.wCurDisk;
         lCurPos   = FsvBufIO_fTell( *lplpInput );
         fFrstRead = FALSE;
      }

      // Evaluate the result.
      if ( eErr < 0 )
      // Bad news.  Unable to read the file.
      {
         break;
      }
      else
      if ( eErr == 0 && wBytsRead > 0 )
      // Not good.  Premature end of file detected.
      {
         eErr = ZIP_CRC_MISMATCH;
         break;
      }

      // Keep track of the amount of the input file read and determine
      // if the end of the input file has been detected.
      wBytsRead      = ( WORD ) eErr;
      lBytsPrcssd   += wBytsRead;
      lNumInputByts -= wBytsRead;
      bEOF           = ( lNumInputByts == 0 );

      if ( lpFltrVars->bCompress )
      /* The input data is being "compressed" so, if necessary, */
      /* calculate the CRC value.                               */
      {
         if ( lpFltrVars->bComputeCRC )
         {
            if ( lpFltrVars->bUseCRC32 )
            {
               lpFltrVars->lCRC = crc32( lpInptBuf + wCurByts, wBytsRead, lpFltrVars->lCRC );
            }
            else
            {
               lpFltrVars->lCRC = crc( lpInptBuf + wCurByts, wBytsRead, ( unsigned short) lpFltrVars->lCRC );
            }
         }
      }
      else
      /* The input data is being "expanded" so, if necessary, decrypt */
      /* the compressed data stored in the input buffer.              */
      {
         if ( lpFltrVars->lpszKey != NULL )
         {
//          xorBlock( lpInptBuf + wCurByts, wBytsRead, (LPTSTR) lpFltrVars->lpszKey, ( LPINT ) &lpFltrVars->wKeyIndx );
            Zip_EnDeCryptByts( &lpFltrVars->sKeys, lpInptBuf + wCurByts, wBytsRead, FALSE );
         }
      }

      // Account for bytes that were already contained in the input buffer.
      wBytsRead += wCurByts;
      wCurByts   = 0;

      // Point to start of input buffer.
      lpTmpBuf = lpInptBuf;

      /* Filter the block of input. */
      do
      {
         /* Initialize control parameters for the "filter" routine.  When */
         /* computing the size of the output buffer allow for "boundary   */
         /* bytes".                                                       */
         wInptByts  = wBytsRead;
         wOutptByts = ( WORD ) ZIP_OUTPUT_BUFFER_SIZE;

         /* Compress or expand, a portion or all of the input buffer. */
         __try
         {
            bDone = lpFltrVars->lpFiltrFunc( lpTmpBuf, &wInptByts, lpOutptBuf, &wOutptByts,
                                          lpFltrVars->lpVars, ( TCHAR ) bEOF );
         }
         __except (EXCEPTION_EXECUTE_HANDLER)
         {
            // If unexpected exception occurred in filter function, assume bad data.
            eErr = ZIP_CRC_MISMATCH;
            bDone = TRUE;
            break;
         }

         if ( wInptByts == 0 && wOutptByts == 0 &&
              ( bEOF || wBytsRead == ZIP_INPUT_CHARS ) )
         // Bad news, the compress/expansion routine could not produce any
         // output and the input buffer contains the maximum amount of input.
         {
            eErr  = ZIP_RUN_OVRFLW;
            bDone = TRUE;
            break;
         }

         if ( ( LONG ) wOutptByts > lMaxOutptByts )
         /* Do not generate more output than was requested by the caller. */
         {
            wOutptByts = (WORD)lMaxOutptByts;
            bDone      = TRUE;
         }

//       /*****IMPLEMENT*****/
//       /* Keep track of last input byte for ZipReadFile(). */
//       lpDir->RFI.lComprsdStrt += wInptByts;
//       /*****IMPLEMENT*****/

         /* Keep track of the maximum amount of output to produce. */
         lMaxOutptByts -= wOutptByts;

                                        // Something really bad happened...
         if ( wInptByts > wBytsRead )
            {
                                        // Cleanup and get out of here...
            eErr  = ZIP_ABORTED;
            bDone = TRUE;
                                        // Return from this function.
            goto cleanup;
            }

         /* Decrement amount of input and advance input buffer pointer. */
         wBytsRead -= wInptByts;
         lpTmpBuf  += wInptByts;

         if ( lpFltrVars->bCompress )
         /* The input data is being compressed so, if necessary, encrypt */
         /* the compressed data stored in the output buffer.             */
         {
//          if ( lpFltrVars->lpszKey != NULL )
//          {
//             xorBlock( lpOutptBuf, wOutptByts, (LPTSTR) lpFltrVars->lpszKey,
//                       ( LPINT ) &lpFltrVars->wKeyIndx );
//             Zip_EnDeCryptByts( &lpFltrVars->sKeys, lpOutptBuf, wOutptByts, TRUE );
//          }
         }
         else
         /* The input data is being expanded so, if necessary, decrypt */
         /* the expanded data stored in the output buffer.             */
         {
            if ( lpFltrVars->bComputeCRC )
            {
               if ( lpFltrVars->bUseCRC32 )
               {
                  lpFltrVars->lCRC = crc32( lpOutptBuf, wOutptByts, lpFltrVars->lCRC );
               }
               else
               {
                  lpFltrVars->lCRC = crc( lpOutptBuf,  wOutptByts, ( unsigned short) lpFltrVars->lCRC );
               }
            }
         }

         if ( !lpFltrVars->bCompress )
         // Try to write the "expanded" data to the output file.
         {
            eErr = lpFltrVars->lpWriteFunc( *lplpOutpt, lpOutptBuf, wOutptByts, lpH->lpCritCBFunc, lpH->dwCritCBData );
         }
#ifndef NAV95                           // Not supporting write to ZIP
                                        // and Zip_fArchiveWrt is harder
                                        // to include than to just drop
                                        // this conditional (Kludge)
         else
         // Very slimey.  When writing a compressed file I have to handle
         // multi-disk archive files.  The compression logic always uses
         // FsvBufIO_fWrite() so I don't need to use lpFltrVars->lpWriteFunc().
         {
            eErr = Zip_fArchiveWrt( lpH, lplpOutpt, lpOutptBuf, wOutptByts );
         }
#endif

         if ( eErr == ( LONG ) wOutptByts )
         // Bytes successfully written.
         {
            lpFltrVars->lOutptByts += eErr;
            eErr                    = ZIP_NO_ERROR;
         }
         else
         // Bad news.  Uabale to write to the output file.
         {
            bDone = TRUE;
         }
      }
      while ( ( !fNoGateBug && bEOF && !bDone ) ||
              ( fNoGateBug && wBytsRead > 0 && wOutptByts > 0 && !bDone ) );
//    while ( wBytsRead > 0 && wOutptByts > 0 && !bDone );

      if ( wBytsRead > 0 && !bDone )
      // The input buffer wasn't completely read.
      {
//       if ( bEOF || wBytsRead == ZIP_INPUT_CHARS && wOutptByts == 0 )
//       // Bad news, the compress/expansion routine could not process
//       // the input buffer.
//       {
//          eErr  = ZIP_RUN_OVRFLW;
//          bDone = TRUE;
//       }
//       else
         // Feed the compression/expansion routine as large an input
         // buffer as possible.
         {
            wCurByts = wBytsRead;
            _fmemcpy( lpInptBuf, lpTmpBuf, wCurByts );
         }
      }

      if ( !bEOF && !bDone && lpH->lpProgCBFunc != NULL )
      /* Inform the user of the progress of the compression/expansion. */
      {
         ZIP_PROGRESSINFO piProgInfo = { ( WORD ) ( lBytsPrcssd * lpH->lPercntMax / lOrigSiz ) + lpH->wPercntOff,
                                         lpFltrVars->eZipFunc, lpH->dwProgCBData };

         if ( lpH->lpProgCBFunc( &piProgInfo ) == ZIP_PROG_CB_ABORT )
         /* The user wants to abort compressing/expanding the file. */
         {
            eErr  = ZIP_ABORTED;
            bDone = TRUE;
         }
      }
   }
   /* The test for wOutptByts is included because the filter   */
   /* functions can return, indicating that all of the input   */
   /* has been processed but still have more output to produce */
   /* (kind of of bizarre but that's the way it works).        */
   while ( ( lNumInputByts > 0 || wOutptByts > 0 ) && !bDone );

cleanup:

   /* Free the temporary buffers. */
   Mem_SubFree( lpInptBuf );
   Mem_SubFree( lpOutptBuf );

   if ( lpFltrVars->bComputeCRC && lpFltrVars->bUseCRC32 )
   /* ??? */
   {
      lpFltrVars->lCRC = -1 - lpFltrVars->lCRC;
   }

   return( eErr );
} // Zip_FilterFile



//==========================================================================
//
// ZIP_ERRCODE Zip_Compress2Pass( lpH, lpInput, lplpOutpt, lpFiltrFunc1,
//                                lpFiltrFunc2, lpFltrVars )
//
//    lpH          - Pointer to the information structure used to process a
//                   connection to an archive file.
//    lpInput      - Pointer ti the structure used to buffer the input file.
//    lplpOutpt    - Pointer to the pointer to the structure used to buffer
//                   i/o to a file or memory buffer.  When compressing a file
//                   into an archive file if the archive file is closed and
//                   re-opened because another diskette had to be inserted
//                   then *lplpOutpt will be updated.
//    lpFiltrFunc1 - Pointer to the function to do the 1st pass of
//                   compression.
//    lpFiltrFunc2 - Pointer to the function to do the 2nd pass of
//                   compression.
//    lpFltrVars   - Pointer to the generic "filter parameter" structure.
//
// This function is called for 2 pass compression methods.  It first
// compresses the raw data to a temporary file, and compresses the data
// stored in that temporary file to the archive file.
//
// Returns:
//    ZIP_NO_ERROR       - The specified file was successfully compressed and
//                         added to the archive file.
//    ZIP_NO_MEMORY      - Not enough memory for dynamic allocations.
//    ZIP_UNKNOWN_METHOD - The specified file contains a compression method
//                         that is not supported.
//    FSVERR_...         - The appropriate error code returned by File.
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and create a new one on another diskette.  If this happens *lplpFBuf
//       (and lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_Compress2Pass(
   LPZIP_HANDLE_INFO      lpH,
   LPFSVBUFIO_FBUF_INFO   lpInput,
   LPLPFSVBUFIO_FBUF_INFO lplpOutpt,
   LPZIP_FILTER_FUNC      lpFiltrFunc1,
   LPZIP_FILTER_FUNC      lpFiltrFunc2,
   LPZIP_FILTER_INFO      lpFltrVars
)

{
   ZIP_ERRCODE    eErr;
   ZIP_BFILE_INFO fiTmpFile;
   LPCTSTR         lpszKey = lpFltrVars->lpszKey;

   if ( ( eErr = Zip_CreateTempFile( lpH, "PASS2", &fiTmpFile,
                                     FsvBufIO_fRtrnEOFPos( lpInput ) ) ) == ZIP_NO_ERROR )
   {
      ZIP_ERRCODE       eTmpErr;
      LPZIP_ARCHIVE_DIR lpDir          = lpH->lpDir;
      BOOL              fTmpCanBeSplit = lpDir->SI.fCanBeSplit;
      BOOL              fTmpIsSplit    = lpDir->SI.fIsSplit;

      // Init feedback control info for the 1st pass.
      lpH->wPercntOff = 0;
      lpH->lPercntMax = 50;

      // Use the 1st pass filter function.
      lpFltrVars->lpFiltrFunc = lpFiltrFunc1;
      lpFltrVars->lpszKey     = NULL;

      // De-activate multi-disk flags for the 1st pass file.
      lpDir->SI.fCanBeSplit = FALSE;
      lpDir->SI.fIsSplit    = FALSE;

      if ( ( eErr = Zip_FilterFile( lpH, &lpInput, &fiTmpFile.lpFBuf, lpFltrVars ) ) == ZIP_NO_ERROR )
      {
         eErr = FsvBufIO_fSeek( fiTmpFile.lpFBuf, 0L, SEEK_SET, lpH->lpCritCBFunc, lpH->dwCritCBData );
      }

      // Evil!  If relevant, turn multi-disk flags back on for the 2nd pass.
      lpDir->SI.fCanBeSplit = fTmpCanBeSplit;
      lpDir->SI.fIsSplit    = fTmpIsSplit;

      if ( eErr == ZIP_NO_ERROR )
      {
         // Init feedback control info for the 2nd pass.
         lpH->wPercntOff = 50;

         // Use the 2nd pass filter function.
         lpFltrVars->lpFiltrFunc   = lpFiltrFunc2;
         lpFltrVars->lpszKey       = lpszKey;
         lpFltrVars->lNumInputByts = FsvBufIO_fRtrnEOFPos( fiTmpFile.lpFBuf );
         lpFltrVars->bComputeCRC   = FALSE;
         lpFltrVars->lOutptByts    = 0;

         eErr = Zip_FilterFile( lpH, &fiTmpFile.lpFBuf, lplpOutpt, lpFltrVars );
      }

      if ( ( eTmpErr = Zip_CloseTempFile( lpH, &fiTmpFile ) ) != ZIP_NO_ERROR &&
           eErr == ZIP_NO_ERROR )
      {
         eErr = eTmpErr;
      }
   }

   return( eErr );
} // Zip_Compress2Pass
#endif          // NOT NAV95


//==========================================================================
//
// VOID Zip_DeInitCompressExpandChk( lplpVars, ePckTyp, fCompress )
//
//    lplpVars  - Pointer to the pointer of the structure that was used
//                by the compression/expansion routine.  The structure
//                pointer will be set to NULL after it has been freed.
//    ePckTyp   - The type of compression/expansion technique.
//    fCompress - If this flag is TRUE then we are dealing with compression.
//
// This function is used to free all memory that was utilized by the
// compresssion/expansion routine.
//
//==========================================================================

VOID WINAPI Zip_DeInitCompressExpandChk(
   LPVOID FAR*   lplpVars,
   ZIP_PACK_TYPE ePckTyp,
   BOOL          fCompress
)

{
   if ( lplpVars != NULL && *lplpVars != NULL )
   {
      switch ( ePckTyp )
      {
         case ZIP_DEFLATN:
            if ( fCompress )
            {
               Zip2x_CompressFree( *lplpVars );
            }
            else
            {
               Zip2x_ExpandFree( *lplpVars );
            }
         break;

         case ZIP_LZH:
            if ( fCompress )
            {
               LZH_EncodeLZH45Free( *lplpVars );
            }
         break;
      }

      Mem_SubFree( *lplpVars );
      *lplpVars = NULL;
   }

} // Zip_DeInitCompressExpandChk



//==========================================================================
//
// ZIP_ERRCODE Zip_InitExpand( lpH, lpFile, lpWriteFunc, lMaxOutput, lpszKey,
//                             wKeyIndx, bComputeCRC, lpVariables, lStart,
//                             wBndryByts, eZipFunc, lpFltrVars )
//
//    lpH         - Pointer to the information structure used to process a
//                  connection to an archive file.
//    lpFile      - Pointer to the structure used to process the compressed
//                  file contained in the archive file that is to be
//                  expanded.
//    lpWriteFunc - The write function that is to be for output.
//    lMaxOutput  - The amount of "compressed data" to expand.
//    lpszKey     - Pointer to the key used for "decryption".
//    wKeyIndx    - The starting index position within the decryption key.
//    bComputeCRC - If this flag is TRUE then the CRC will be computed when
//                  the data is "expanded".
//    lpVariables - If this pointer is NULL and the "expansion routine"
//                  requires "state info" then the appropriate structure will
//                  be allocated and initialized.  If this pointer isn't NULL
//                  then it is assummed that a file is sequentually expanded
//                  via ZipReadFile().
//    lStart      - The position within the file to start reading the data
//                  to expand.
//    wBndryByts  - ....
//    eZipFunc    - The type of zip operation (ZipReadFile or ZipExtractFile)
//                  that will be performed from ZipFilterFile().
//    lpFltrVars  - Pointer to the structure that will be initialized with
//                  the appropriate "filter parameters".
//
// This function is used to initialize the "filter parameters" structure
// for the specified file and the specified compression method.
//
// Returns:
//    ZIP_NO_ERROR       - The operation was successful.
//    ZIP_DIR_ERROR      - File header data didn't match the central directory
//                         data.
//    ZIP_UNKNOWN_METHOD - The specified file employed a compression method
//                         that is not supported.
//    FSVERR...          - The appropriate error code returned by File
//                         Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_InitExpand(
   LPZIP_HANDLE_INFO   lpH,
   LPZIP_COMP_INFO     lpFile,
   LPZIP_FILEIO_FUNC   lpWriteFunc,
   LONG                lMaxOutput,
   LPCTSTR              lpszKey,
   WORD                wKeyIndx,
   BOOL                bComputeCRC,
   LPVOID              lpVariables,
   LONG                lStart,
   WORD                wBndryByts,
   ZIP_CALLBACK_OP     eZipFunc,
   LPZIP_FILTER_INFO   lpFltrVars
)

{
   LPZIP_FILTER_FUNC    lpExpandFunc;
   TCHAR                 cBits;
   ZIP_ERRCODE          eErr;
   BOOL                 bUseCRC32  = FALSE;
   WORD                 wStructSiz = 0;

   /* Initialize pointer to NULL in case an error is detected prior */
   /* to allocation/initialization.                                 */
   lpFltrVars->lpVars = NULL;

   if ( lpFile->bEncrypted && lpszKey == NULL )
   /* Cannot expand an encrypted file without a key. */
   {
      return( ZIP_EN_FILE_NO_KEY );
   }

   if ( !lpFile->bEncrypted || lpH->lpDir->eArchvTyp != ZIP_FORMAT_ZIP )
   // The file has not been encrypted or the archive type doesn't support
   // encryption so deactivate the "decryption" key.
   {
      lpszKey = NULL;
   }

   // If necessary, get the size of the extra data for the
   // compressed file.
   if ( ( eErr = Zip_FileVrfyChk( lpH, lpFile ) ) != ZIP_NO_ERROR )
   {
      return( eErr );
   }

   /*****FIX for ZipReadFile() optimization*****/
   lStart = lpFile->dbComprssdFile.lStart;
   /*****FIX for ZipReadFile() optimization*****/

   if ( lMaxOutput > lpFile->lOrigSiz )
   {
      lMaxOutput = lpFile->lOrigSiz;
   }

   // Move file pointer to the start of the data for the compressed file.
   if ( ( eErr = Zip_fArchiveSeek( lpH, &lpH->lpDir->fiArchvFile.lpFBuf, lpFile->dbComprssdFile.wDiskNum,
                                   lStart, SEEK_SET ) ) != lStart )
   // Bad news.  User want's to abort or file i/o error detected.
   {
      return( eErr );
   }

   // Reset the error code.
   eErr = ZIP_NO_ERROR;

   /* Zero out the "filter" structure. */
   _fmemset( lpFltrVars, 0, sizeof( ZIP_FILTER_INFO ) );

   /* Get the size of the state information structure (if any) */
   /* used in the various de-compression algorithms.           */
   switch ( lpFile->ePckTyp )
   {
      case ZIP_NONE:
         lpExpandFunc = Zip_Unfiltered;
         bUseCRC32    = ( lpH->lpDir->eArchvTyp == ZIP_FORMAT_ZIP );
      break;

      case ZIP_RLE:
         lpExpandFunc = Zip_ExpRLEWrppr;
      break;

      case ZIP_SQUEEZED:
         wStructSiz   = expand_huffman_size();
         lpExpandFunc = expand_huffman;
      break;

      case ZIP_CRUNCH:
         if ( lpFile->wPckFlgs < 3 )
         {
            wStructSiz   = uncrunch_size();
            lpExpandFunc = uncrunch;
         }
         else
         {
            if ( ( eErr = Zip_fReadByts( lpH, lpH->lpDir->fiArchvFile.lpFBuf, &cBits, sizeof( cBits) ) ) != ZIP_NO_ERROR )
            /* Bad news.  File i/o error detected. */
            {
               return( eErr );
            }

            if ( lpszKey != NULL )
            /* Decrypt the compressed data with special "key". */
            {
               xorBlock( &cBits, 1, (LPTSTR) lpszKey, ( __int16 far * ) &wKeyIndx );
            }

            if ( cBits != 12 )
            /* Bad news. The special signature information didn't match. */
            {
               return( ZIP_DIR_ERROR );
            }

            wStructSiz   = expandLZWsize();
            lpExpandFunc = expandLZW;
         }
      break;

      case ZIP_SQUASH:
         wStructSiz   = expandLZWsize();
         lpExpandFunc = expandLZW;
      break;

      case ZIP_REDUCED:
         wStructSiz   = fatten_size();
         lpExpandFunc = fatten;
         bUseCRC32    = TRUE;
      break;

      case ZIP_IMPLODE:
         wStructSiz   = explodeWorkSize();
         lpExpandFunc = explode;
         bUseCRC32    = TRUE;
      break;

      case ZIP_DEFLATN:
         wStructSiz   = Zip2x_ExpandWorkSize();
         lpExpandFunc = /*( LPZIP_FILTER_FUNC )*/ Zip2x_Expand; //*****FIX*****
         bUseCRC32    = TRUE;
      break;

      case ZIP_SHRINK:
         wStructSiz   = unshrinkSize();
         lpExpandFunc = unshrink;
         bUseCRC32    = TRUE;
      break;

      case ZIP_CRUSH:
         wStructSiz   = expandCrushSize();
         lpExpandFunc = expandCrush;
      break;

#if USE_ARCPAK
      case ZIP_DISTILL:
         wStructSiz   = diluteWorkSize();
         lpExpandFunc = dilute;
      break;
#endif

      case ZIP_LZH:
         if ( lpFile->wPckFlgs < 4 || lpFile->wPckFlgs > 5 )
         // Only methods "4" and "5" are supported.
         {
            return( ZIP_UNKNOWN_METHOD );
         }

         wStructSiz   = LZH_DecodeLZH45WorkSize();
         lpExpandFunc = /*( LPZIP_FILTER_FUNC )*/  LZH_DecodeLZH45; //*****FIX*****
      break;

      // The compressed file uses an unsupported compression method.
      default:
         return( ZIP_UNKNOWN_METHOD );
      break;
   }

   if ( lpVariables == NULL )
   /* If necessary, allocate the state info structure required */
   /* for some of the de-compression algorithms.               */
   {
      if ( wStructSiz > 0 && ( lpVariables = Mem_SubMalloc( wStructSiz ) ) == NULL )
      /* Bad news.  Unable to allocate the data structure used to     */
      /* contain state information during the de-compression process. */
      {
         return( ZIP_NO_MEMORY );
      }

      /* Get pointer to the de-compression function, and, if necessary,    */
      /* initialize the state info structure for the different compression */
      /* formats.                                                          */
      switch ( lpFile->ePckTyp )
      {
         case ZIP_RLE:
            initExpandRLE( );
         break;

         case ZIP_SQUEEZED:
            init_expand_huffman( lpVariables );
         break;

         case ZIP_CRUNCH:
            if (lpFile->wPckFlgs < 3)
            {
               init_uncrunch( lpVariables, ( TCHAR ) ( lpFile->wPckFlgs > 1 ),
                              ( TCHAR ) ( lpFile->wPckFlgs > 0 ) );
            }
            else
            {
               initExpandLZW( lpVariables, cBits, 1 );
            }
         break;

         case ZIP_SQUASH:
            initExpandLZW( lpVariables, 13, 0 );
         break;

         case ZIP_REDUCED:
            init_fatten( lpVariables, 7 - lpFile->wPckFlgs );
         break;

         case ZIP_IMPLODE:
            initExplode( lpVariables, ( TCHAR ) ( lpFile->wPckFlgs & ZIP_CHARS_ENCODED ),
                         ( TCHAR ) ( lpFile->wPckFlgs & ZIP_8K_WINDOW ) );
         break;

         case ZIP_DEFLATN:
            if ( Zip2x_ExpandInit( lpVariables ) )
            {
               return( ZIP_NO_MEMORY );
            }
         break;

         case ZIP_SHRINK:
            initUnshrink( lpVariables );
         break;

         case ZIP_CRUSH:
            initExpandCrush( lpVariables );
         break;

#if USE_ARCPAK
         case ZIP_DISTILL:
            initDilute( lpVariables );
         break;
#endif

         case ZIP_LZH:
            LZH_DecodeLZH45Init( lpVariables, lpFile->wPckFlgs );
         break;
      }
   }

   // Init the parameters for the generic "filter" function.
   lpFltrVars->lpVars        = lpVariables;
   lpFltrVars->wStructSiz    = wStructSiz;
   lpFltrVars->wBndryByts    = wBndryByts;
   lpFltrVars->wKeyIndx      = wKeyIndx;
   lpFltrVars->lpszKey       = lpszKey;
   lpFltrVars->bComputeCRC   = bComputeCRC;
   lpFltrVars->bUseCRC32     = bUseCRC32;
   lpFltrVars->lNumInputByts = lpFile->dbComprssdFile.lSize - ( lStart - lpFile->dbComprssdFile.lStart );
   lpFltrVars->lMaxOutptByts = lMaxOutput;
   lpFltrVars->lpFiltrFunc   = lpExpandFunc;
   lpFltrVars->lpReadFunc    = FsvBufIO_fRead;
   lpFltrVars->lpWriteFunc   = lpWriteFunc;
   lpFltrVars->eZipFunc      = eZipFunc;

   if ( lpszKey != NULL && lpH->lpDir->eArchvTyp == ZIP_FORMAT_ZIP )
   // Try to decode the ZIP file encryption header.
   {
      eErr                       = Zip_DecryptSetup( lpH, &lpFltrVars->sKeys, lpFltrVars->lpszKey, lpFile->lCRC );
      lpFltrVars->lNumInputByts -= sizeof( ZIP_ENCRYPT_INFO );
   }

   return( eErr );
} /* Zip_InitExpand */



//==========================================================================
//
// ZIP_ERRCODE Zip_CompressFile( lpH, lpFile, lpszFullName, lpReadFunc,
//                               lpSeekFunc, lpszKey, lpExtra, wExtraSiz,
//                               bAddFile )
//
//    lpH          - Pointer to the information structure used to process a
//                   connection to an archive file.
//    lpFile       - Pointer to the structure that will be used to keep track
//                   of the file after it has been compressed.
//    lpszFullName - The full name of the file to compress.
//    lpReadFunc   -
//    lpSeekFunc   -
//    lpszKey      - Pointer to the key that will be used to "encrypt" the
//                   data after it has been compressed.  If this pointer is
//                   NULL then the data will NOT be encrypted.
//    lpExtra      - Pointer to any "extra data" that is to be stored with
//                   the compressed file.  The "extra" data does not get
//                   compressed.  This pointer can be NULL.
//    wExtraSiz    - The number of bytes of "extra data" to store.
//    bAddFile     - This flag is TRUE if the file is being "added" to the
//                   archive file.
//
// This function "compresses" the specified file and to stores that
// compressed data at the end of the archive file.  This function always sets
// the file pointer beyond the end of file to ensure that there will be enough
// space for the end of file data (ARC/PAK record or the ZIP central
// directory) and all of the "extra data" and "remark text" that will get
// re-located beyond the end of the file for the "pack".
//
// This function will always leave the "file pointer" pointer to the area
// immediately following the last successfully compressed file.
//
// Returns:
//    ZIP_NO_ERROR       - The specified file was successfully compressed and
//                         added to the archive file.
//    ZIP_NO_MEMORY      - Not enough memory for dynamic allocations.
//    ZIP_UNKNOWN_METHOD - The specified file contains a compression method
//                         that is not supported.
//    FSVERR_...         - The appropriate error code returned by File.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_CompressFile(
   LPZIP_HANDLE_INFO  lpH,
   LPZIP_COMP_INFO    lpFile,
   LPCTSTR             lpszFullName,
   LPCTSTR             lpszKey,
   LPVOID             lpExtra,
   WORD               wExtraSiz,
   BOOL               bAddFile
)

{
   LPFSVBUFIO_FBUF_INFO   lpInpt;
   TCHAR                   cBits;
   ZIP_ERRCODE            eErr;
   ZIP_FILTER_INFO        fvFltrVars;
   LPZIP_FILTER_FUNC      lpCompressFunc2 = NULL;
   LONG                   lBytsToFree;
   FSVIO_DIRENTRYSTRUCT   diDirInfo;
   LPZIP_ARCHIVE_DIR      lpDir           = lpH->lpDir;
   LONG                   lExtraRemByts   = 0;
   LPFSVBUFIO_FBUF_INFO   lpOutpt         = lpDir->fiArchvFile.lpFBuf;
   LONG                   lOrigEOF        = FsvBufIO_fRtrnEOFPos( lpOutpt );
   WORD                   wOrigDisk       = lpDir->SI.wCurDisk;
   WORD                   wEncryptByts    = 0;

   if ( lpDir->eArchvTyp != ZIP_FORMAT_ZIP )
   // Only ZIP files can be encrypted.
   {
      lpszKey = NULL;
   }

   // Zero out the "filter" structure.
   _fmemset( &fvFltrVars, 0, sizeof( ZIP_FILTER_INFO ) );
   fvFltrVars.lpszKey = lpszKey;

   if ( lstrcmpi( lpDir->fiArchvFile.szFullName, lpszFullName ) == 0 )
   /* The specified file is the "archive file", so do nothing. */
   {
      return( ZIP_NO_ERROR );
   }

   if ( !lpDir->SI.fIsSplit )
   {
      // Get the size of the extra data for all of the compressed files
      // contained in the archive file.  Doesn't pertain to multi-disk
      // archive files.
      if ( ( eErr = Zip_ArchvVrfyChk( lpH ) ) != ZIP_NO_ERROR )
      {
         return( eErr );
      }
   }
   else
   // Make sure the diskette containing the "starting position" for a
   // compressed file is inserted in the disk drive.
   if ( ( eErr = Zip_PutInNthDiskChk( lpH, &lpOutpt, lpDir->SI.wCurDisk, ZIP_ADD_INSERT_NTH ) ) != ZIP_NO_ERROR )
   {
      return( eErr );
   }

   if ( ( eErr = FsvBufIO_fOpen( lpszFullName, OF_READ | OF_SHARE_DENY_WRITE,
                                 0, &lpInpt, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) != ZIP_NO_ERROR ||
        ( eErr = FsvIO_GetDirEntry( lpszFullName, &diDirInfo, NULL, lpH->lpCritCBFunc,
                                    lpH->dwCritCBData ) ) != ZIP_NO_ERROR )
   /* Bad news.  Unable to open the input file or unable to get the */
   /* file attribute info.                                          */
   {
      return( eErr );
   }

   /* Store date, time, and attributes of the file to compress. */
// lDateTime = FsvDOSWrap_GetDateAndTime( lpInpt->hFile );
// *( ( LPWORD ) &lpFile->diDate ) = HIWORD( lDateTime );
// *( ( LPWORD ) &lpFile->tiTime ) = LOWORD( lDateTime );
   *( ( LPWORD ) &lpFile->diDate ) = diDirInfo.wDate;
   *( ( LPWORD ) &lpFile->tiTime ) = diDirInfo.wTime;
   lpFile->wAttrs                  = (WORD)diDirInfo.dwFsvAttrib;

   /* Determine size of input file. */
   lpFile->lOrigSiz = FsvBufIO_fRtrnEOFPos( lpInpt );

   // Is the file to be encrypted?
   lpFile->bEncrypted = ( lpszKey != NULL );

   // Keep track of the disk the header starts on.
   lpFile->wDiskNum = lpDir->SI.wCurDisk;

   if ( lpFile->lOrigSiz == 0 )
   // JJS.  09/24/93.  No data to compress so use ZIP_NONE.  This
   // is to keep Zip2x_CompressInit() from failing.
   {
      lpFile->ePckTyp = ZIP_NONE;
   }
   else
   if ( lpFile->ePckTyp == ZIP_BEST )
   /* Determine the best compression algorithm to use. */
   {
      lpFile->ePckTyp = Zip_SelectPckTyp( lpDir->eArchvTyp, lpFile );
   }

   if ( wExtraSiz > 0 )
   /* Account for any additional extra bytes. */
   {
      lExtraRemByts = ( LONG ) wExtraSiz - lpFile->dbExtra1.lSize;
   }

   /* Based on whether the is being "added" or "replaced" determine */
   /* the number of bytes in the file that will be freed.           */
   if ( bAddFile )
   {
      lBytsToFree = 0;
   }
   else
   {
      lBytsToFree = Zip_CalcOldComprssdFileByts( lpFile ) +
                    Zip_CalcEndInfoByts( lpDir, lpFile );
   }

   // Point to available space for the file to compress.
   if ( ( eErr = FsvBufIO_fSeek( lpOutpt, lpDir->SI.lAvailPostn, SEEK_SET, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) == lpDir->SI.lAvailPostn )
   {
      BOOL bNewExtraData = ( lpExtra != NULL && wExtraSiz > 0 );

      /* Write out the header data. */
      eErr = Zip_WriteFileHeadr( lpH, &lpOutpt, lpFile, !bNewExtraData );

      if ( eErr == ZIP_NO_ERROR && bNewExtraData )
      // Store the provided extra information just after the file header.
      {
         eErr = Zip_CreateDataBlk( lpH, &lpFile->dbExtra1, lpExtra, wExtraSiz, FALSE );
      }

      // Store starting position of the compressed file data.
      lpFile->dbComprssdFile.lStart    = FsvBufIO_fTell( lpOutpt );
      lpFile->dbComprssdFile.wDiskNum  = lpDir->SI.wCurDisk;
      lpFile->dbComprssdFile.hbFleIndx = ZIP_ARCHV_HANDLE;

      if ( eErr == ZIP_NO_ERROR && lpFile->bEncrypted )
      // Write out the encryption header.  After the data is compressed the
      // encryption header will be rewritten since it has to initialized
      // with the CRC for the compressed data.
      {
         eErr         = Zip_EncryptSetup( lpH, &fvFltrVars.sKeys, fvFltrVars.lpszKey, 0L );
         wEncryptByts = sizeof( ZIP_ENCRYPT_INFO );
      }
   }

   if ( eErr == ZIP_NO_ERROR )
   {
      // Get the size of the state information structure (if any)
      // used in the various compression algorithms.
      switch ( lpFile->ePckTyp )
      {
         case ZIP_NONE:
            fvFltrVars.lpFiltrFunc = Zip_Unfiltered;
            fvFltrVars.bUseCRC32   = ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP );
         break;

         case ZIP_CRUNCH :
            lpFile->wPckFlgs       = 3;
            cBits                  = 12;
            fvFltrVars.wStructSiz  = LZWworkSize();
            fvFltrVars.lpFiltrFunc = LZW;

            if ( lpszKey )
            /* Encrypt the special signature bits. */
            {
               xorBlock( &cBits, 1, (LPTSTR) lpszKey, &fvFltrVars.wKeyIndx );
            }

            /* Write out special signature bites. */
            if ( ( eErr = FsvBufIO_fWrite( lpOutpt, &cBits, 1, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == 1 )
            {
               eErr = ZIP_NO_ERROR;
            }
         break;

         case ZIP_SQUASH:
            fvFltrVars.wStructSiz  = LZWworkSize();
            fvFltrVars.lpFiltrFunc = LZW;
         break;

         case ZIP_SHRINK:
            fvFltrVars.wStructSiz  = shrinkSize();
            fvFltrVars.lpFiltrFunc = shrink;
            fvFltrVars.bUseCRC32   = TRUE;
         break;

         case ZIP_CRUSH:
            fvFltrVars.wStructSiz  = CrushWorkSize();
            fvFltrVars.lpFiltrFunc = Crush;
         break;

#pragma message( "FIX: IMPLODE/EXPLODE\n\r" )
#ifdef FIX
         case ZIP_IMPLODE:
            lpFile->wPckFlgs       = ZIP_8K_WINDOW | ZIP_CHARS_ENCODED;
            fvFltrVars.wStructSiz  = implodeWorkSize();
            fvFltrVars.bUseCRC32   = TRUE;
            fvFltrVars.lpFiltrFunc = preImplode;
            lpCompressFunc2        = implode;
         break;
#endif

         case ZIP_DEFLATN:
//          lpFile->wPckFlgs       = ZIP_8K_WINDOW; // Maximal set to TRUE when
//                                                  //  calling Zip2x_CompressInit().
            fvFltrVars.wStructSiz  = Zip2x_CompressWorkSize();
            fvFltrVars.bUseCRC32   = TRUE;
            fvFltrVars.lpFiltrFunc = Zip2x_Compress;
         break;

#if USE_ARCPAK
         case ZIP_DISTILL:
            fvFltrVars.wStructSiz  = distillWorkSize();
            fvFltrVars.lpFiltrFunc = preDistill;
            lpCompressFunc2 = distill;
         break;
#endif

         case ZIP_LZH: //*****FIX***** -- Handle 4&5 only.
            lpFile->wPckFlgs       = 5;
            lpFile->bLevel         = 1;
            fvFltrVars.wStructSiz  = LZH_EncodeLZH45WorkSize();
            fvFltrVars.lpFiltrFunc = LZH_EncodeLZH45;
         break;

         default:
            eErr = ZIP_UNKNOWN_METHOD;
         break;
      }

      if ( eErr == ZIP_NO_ERROR )
      {
         if ( fvFltrVars.wStructSiz > 0 &&
              ( fvFltrVars.lpVars = Mem_SubMalloc( fvFltrVars.wStructSiz ) ) == NULL )
         /* Bad news.  Unable to allocate the data structure used to  */
         /* contain state information during the compression process. */
         {
            eErr = ZIP_NO_MEMORY;
         }
         else
         {
            /* Initialize any special state variables. */
            switch ( lpFile->ePckTyp )
            {
               case ZIP_CRUNCH:
                  initLZW( fvFltrVars.lpVars, cBits, 1);
               break;

               case ZIP_SQUASH:
                  initLZW( fvFltrVars.lpVars, 13, 0 );
               break;

               case ZIP_SHRINK:
                  initShrink( fvFltrVars.lpVars );
               break;

               case ZIP_CRUSH:
                  initCrush( fvFltrVars.lpVars );
               break;

#pragma message( "FIX: IMPLODE/EXPLODE\n\r" )
#ifdef FIX
               case ZIP_IMPLODE:
                  initImplode( fvFltrVars.lpVars, 0 );
               break;
#endif

               case ZIP_DEFLATN:
                  if ( Zip2x_CompressInit( fvFltrVars.lpVars, lpFile->lOrigSiz, FALSE ) )
                  // Bad news, an allocation error was encountered.
                  {
                     eErr = ZIP_NO_MEMORY;
                  }
               break;

#if USE_ARCPAK
               case ZIP_DISTILL:
                  initDistill( fvFltrVars.lpVars );
               break;
#endif

               case ZIP_LZH: //*****FIX***** -- Handle 4&5 only.
                  eErr = LZH_EncodeLZH45Init( fvFltrVars.lpVars, lpFile->wPckFlgs );
               break;
            }

            if ( eErr == ZIP_NO_ERROR )
            {
               // Init the parameters for the generic "filter" function.
               fvFltrVars.wBndryByts    = ZIP_BOUNDARY_BYTS;
               fvFltrVars.bCompress     = TRUE;
               fvFltrVars.bComputeCRC   = TRUE;
               fvFltrVars.lCRC          = 0;
               fvFltrVars.lNumInputByts = lpFile->lOrigSiz;
               fvFltrVars.lMaxOutptByts = ZIP_MAXLONG;
               fvFltrVars.lpReadFunc    = FsvBufIO_fRead;
               fvFltrVars.lpWriteFunc   = FsvBufIO_fWrite;
               fvFltrVars.eZipFunc      = ( bAddFile ) ? ZIP_ADD_FILE : ZIP_REPLACE_FILE;

               if ( lpFile->ePckTyp == ZIP_IMPLODE
#if USE_ARCPAK
                    || lpFile->ePckTyp == ZIP_DISTILL
#endif
                    )
               // Handle the 2 pass compression algorithms.
               {
                  eErr = Zip_Compress2Pass( lpH, lpInpt, &lpOutpt, fvFltrVars.lpFiltrFunc,
                                            lpCompressFunc2, &fvFltrVars );
               }
               else
               /* Handle the 1 pass compression algorithms. */
               {
                  /* Init feedback control info for the one pass */
                  /* compression algorithms.                     */
                  lpH->wPercntOff = 0;
                  lpH->lPercntMax = 100;

                  eErr = Zip_FilterFile( lpH, &lpInpt, &lpOutpt, &fvFltrVars );
               }

               // If allocated, free the structure of special state variables.
               Zip_DeInitCompressExpandChk( &fvFltrVars.lpVars, lpFile->ePckTyp, TRUE );

               if ( eErr == ZIP_NO_ERROR )
               {
                  LONG lCurPostn;
                  WORD wCurDisk;

                  // Store size of the compressed file.
                  lpFile->dbComprssdFile.lSize = fvFltrVars.lOutptByts + wEncryptByts;
                  lpFile->bDirty               = TRUE;

                  if (!lpDir->SI.fIsSplit)
                  //*****************
                  // KLUGE ALERT!!!!!
                  //*****************
                  // BUG:
                  //    For some unknown reason, if rewinding and storing is performed
                  //    on a spanning archive, we lose data in the file that spans the
                  //    disks.
                  //
                  // FIX:
                  //    We don't allow spanning archives to go back and store in
                  //    uncompressed format.
                  //*****************
                  {
                     if ( lpFile->dbComprssdFile.lSize - wEncryptByts >= lpFile->lOrigSiz )
                     // Not good.  The compressed file is larger than the
                     // original so simply store it in it's raw state.
                     {
                        lpFile->ePckTyp              = ZIP_NONE;
                        lpFile->wPckFlgs             = 0;
                        lpFile->dbComprssdFile.lSize = lpFile->lOrigSiz + wEncryptByts;

                        // Init feedback control info for this one pass copy.
                        lpH->wPercntOff = 0;
                        lpH->lPercntMax = 100;

                        if ( ( eErr = FsvBufIO_fSeek( lpInpt, 0L, SEEK_SET, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == 0 &&
                             ( eErr = Zip_fArchiveSeek( lpH, &lpOutpt, lpFile->dbComprssdFile.wDiskNum, lpFile->dbComprssdFile.lStart + wEncryptByts, SEEK_SET ) ) == lpFile->dbComprssdFile.lStart + wEncryptByts )
                        {
                           /* Set fields for "unfiltered" data and reset fields */
                           /* that were clobbered by Zip_Compress2Pass().       */
                           fvFltrVars.wKeyIndx      = 0;
                           fvFltrVars.lpFiltrFunc   = Zip_Unfiltered;
                           fvFltrVars.bUseCRC32     = ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP );
                           fvFltrVars.lNumInputByts = lpFile->lOrigSiz;
                           fvFltrVars.lMaxOutptByts = lpFile->lOrigSiz;
                           fvFltrVars.lOutptByts    = 0;
                           fvFltrVars.bComputeCRC   = TRUE;

                           eErr = Zip_FilterFile( lpH, &lpInpt, &lpOutpt, &fvFltrVars );
                        }
                     }
                  }

                  /* Make permanent copy of CRC. */
                  lpFile->lCRC = fvFltrVars.lCRC;

                  // Keep track of the logical EOF.
                  lCurPostn = FsvBufIO_fTell( lpOutpt );
                  wCurDisk  = lpDir->SI.wCurDisk;

                  // Rewrite the compressed file header record since the size
                  // is now known (also the compression type might have
                  // changed to ZIP_NONE).  After that move the current
                  // file pointer back to the logical EOF.
                  if ( eErr == ZIP_NO_ERROR &&
                       ( eErr = Zip_fArchiveSeek( lpH, &lpOutpt, lpFile->wDiskNum, lpFile->lCompHeadrStrt, SEEK_SET ) ) == lpFile->lCompHeadrStrt &&
                       ( eErr = Zip_WriteFileHeadr( lpH, &lpOutpt, lpFile, FALSE ) ) == ZIP_NO_ERROR )
                  {
                     if ( lpFile->bEncrypted )
                     // Try to encrypt the ZIP file.
                     {
                        eErr = Zip_EncryptZipFile( lpH, &lpOutpt, lpFile, lpszKey, TRUE );
                     }

                     // Move the current file pointer back to the logical EOF.
                     if ( eErr == ZIP_NO_ERROR &&
                          ( eErr = Zip_fArchiveSeek( lpH, &lpOutpt, wCurDisk, lCurPostn, SEEK_SET ) ) == lCurPostn )
                     // Try to grow the archive file to acount for the space
                     // required for all accumulated EOF bytes and all of the
                     // "extra" and "remark" data that will be re-located
                     // for the "pack".  After growing the file, reset the file
                     // pointer to the logical end of file.
                     {
                        eErr = Zip_GrowthCheck( lpH, &lpOutpt, lExtraRemByts, Zip_CalcEndInfoByts( lpDir, lpFile ), lBytsToFree );
                     }
                  }
               }
            }
         }
      }
   }

   if ( eErr != ZIP_NO_ERROR )
   // Restore logical end of file.
   {
      Zip_fArchiveSetEOF( lpH, &lpOutpt, wOrigDisk, lOrigEOF );
   }

   /* Close the input file. */
   FsvBufIO_fClose( lpInpt, FALSE, lpH->lpCritCBFunc, lpH->dwCritCBData );

   return( eErr );
} /* Zip_CompressFile */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_GrowthCheck( lpH, lplpOutpt, lNewExtraRemByts, lNewEOFByts,
//                              lBytsToFree )
//
//    lpH           - Pointer to the information structure used to process a
//                    connection to an archive file.
//    lplpOutpt     - Pointer to the pointer to the structure used to buffer
//                    i/o to a file or memory buffer.  When compressing a file
//                    into an archive file if the archive file is closed and
//                    re-opened because another diskette had to be inserted
//                    then *lplpOutpt will be updated.
//    lExtraRemByts - The number of additional extra/remark bytes to store in
//                    the archive file.
//    lNewEOFByts   - The number of additional end of file bytes to store in
//                    the archive file.
//    lBytsToFree   - The number of bytes of newly released free space in the
//                    the archive file.
//
// This function tries to claim the specified amount of space beyond the
// physical end of file so that the PACK operation will be successful.
//
// To optimize performance for floppy drives this function will grow the file
// in large chunks (instead of just the amount of space needed).  This will
// keep down the number of "writes" to the disk drive (floppies are often
// uncached).  If the large chunk can not be allocated that just the amount
// needed will be allocated.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    FSVERR...     - The appropriate error code returned by File Services.
//
// Notes:
//    1) This function could potentially close the archive file on one diskette
//       and create a new one on another diskette.  If this happens *lplpFBuf
//       (and lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_GrowthCheck(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpOutpt,
   LONG                   lNewExtraRemByts,
   LONG                   lNewEOFByts,
   LONG                   lBytsToFree
)

{
   LONG              lBytsToReserve;
   ZIP_ERRCODE       eErr      = ZIP_NO_ERROR;
   LPZIP_ARCHIVE_DIR lpDir     = lpH->lpDir;
   LONG              lCurPostn = FsvBufIO_fTell( *lplpOutpt );

   if ( lpDir->SI.fIsSplit )
   // This logic does not apply to multi-disk archive files.
   {
      lpDir->SI.bDirty      = TRUE;
      lpDir->SI.wStrtDisk   = lpDir->SI.wCurDisk;
      lpDir->SI.lAvailPostn = lCurPostn;
      return( ZIP_NO_ERROR );
   }

   /* Accumulate and determine the number of EOF data bytes that will    */
   /* be stored for this file. Also determine amount of space to reserve */
   /* beyond the logical end of file.                                    */
   lNewEOFByts      += lpDir->SI.lEOFByts;
   lBytsToFree      += lpDir->SI.lFreedByts;
   lNewExtraRemByts += lpDir->SI.lExtraRemByts;
   lBytsToReserve    = max( 0, lNewEOFByts - lBytsToFree ) + lNewExtraRemByts;

   /* Does the current position plus available data exceed or equal (need */
   /* to see if the buffer can be flushed) the end of the file?           */
   if ( lCurPostn + lBytsToReserve >= FsvBufIO_fRtrnEOFPos( *lplpOutpt ) )
   /* Try to allocate a "chunk" of free space (account for */
   /* the amount of free space available).                 */
   {
      if ( ( eErr = FsvBufIO_fSetEOF( *lplpOutpt, lCurPostn + lBytsToReserve + ZIP_GROWTH_CHUNK,
                                      lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == FSVERR_DISKFULL )
      {
         /* Flush out the i/o buffer before getting the amount */
         /* of free space.                                     */
         if ( ( eErr = FsvBufIO_fSetEOF( *lplpOutpt, lCurPostn + lBytsToReserve,
                                         lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == ZIP_NO_ERROR )
         /* Determine the amount of free space. */
         {
            #define ZIP_MIN_SECTOR_SIZ 512 // I use the minimum known sector
                                           // size since there isn't a function
                                           // currently avaiable that returns
                                           // this info.
                                           // ** Also need to know the cluster
                                           //    size.

            DWORD dwFreeSpace;
            DWORD dwTotalSpace;
            LONG  lMaxPostn = lCurPostn;
            LONG  wRmndr    = ( WORD ) ( ( lCurPostn + 1 ) % ZIP_MIN_SECTOR_SIZ );

            if ( wRmndr > 0 && lCurPostn > 0 )
            /* Round up to the next sector. */
            {
               lMaxPostn += ( ZIP_MIN_SECTOR_SIZ - wRmndr + 1 );
            }

            if ( ( eErr = FsvIO_FastGetDiskFreeSpace( FsvUtil_ConvrtDrvLtr2ID( lpDir->fiArchvFile.szFullName[ 0 ] ),
                                                      &dwFreeSpace, &dwTotalSpace,
                                                      lpH->lpCritCBFunc,
                                                      lpH->dwCritCBData ) ) == ZIP_NO_ERROR &&
                 dwFreeSpace != 0 )
            /* Allocate all available free space (could add logic to round */
            /* up to the sector boundary).                                 */
            {
               eErr = FsvBufIO_fSetEOF( *lplpOutpt, lMaxPostn + ( LONG ) dwFreeSpace,
                                        lpH->lpCritCBFunc, lpH->dwCritCBData );
            }
         }
      }
   }

   if ( eErr == ZIP_NO_ERROR &&
        ( eErr = FsvBufIO_fSeek( *lplpOutpt, lCurPostn, SEEK_SET, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData  ) ) == lCurPostn )
   /* Grow the archive file to acount for the space required */
   /* for all accumulated EOF bytes and all of the "extra"   */
   /* and "remark" data that will be re-located for the      */
   /* "pack".                                                */
   {
      eErr                    = ZIP_NO_ERROR;
      lpDir->SI.bDirty        = TRUE;
      lpDir->SI.lAvailPostn   = lCurPostn;
      lpDir->SI.lFreedByts    = lBytsToFree;
      lpDir->SI.lEOFByts      = lNewEOFByts;
      lpDir->SI.lExtraRemByts = lNewExtraRemByts;
   }
   else
   if ( lpDir->SI.fCanBeSplit && !FsvIO_IsCritErr( eErr ) /*eErr == FSVERR_DISKFULL*/ )
   // Ask user if they want to create a multi-disk archive file.
   {
      BOOL  fIsSplitSav = lpDir->SI.fIsSplit;

//    // Should be able to restore the current end of file.
//    if ( ( eErr = FsvBufIO_fSetEOF( lpOutpt, lCurPostn, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) != ZIP_NO_ERROR )
//    {
//       return( eErr );
//    }

      if ( ( eErr = Zip_fFlushBuf( lpH, lplpOutpt ) ) != ZIP_NO_ERROR )
      // Disk error detected or the user does not want to create a
      // multi-disk archive file so continue as normal.
      {
         lpDir->SI.fCanBeSplit = FALSE;
         eErr                  = ( FsvIO_IsCritErr( eErr ) ) ? eErr : FSVERR_DISKFULL;
      }
      else
      // Reset the error status and continue as normal.
      {
         lpDir->SI.lAvailPostn = FsvBufIO_fTell( *lplpOutpt );
         lpDir->SI.bDirty      = TRUE;
         lpDir->SI.wStrtDisk   = lpDir->SI.wCurDisk;
         lpDir->SI.fIsSplit    = fIsSplitSav;
         eErr                  = ZIP_NO_ERROR;
      }
   }

  return( eErr );
} // Zip_GrowthCheck
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_MoveFileToEnd( lpH, lpOldInfo, lpNewInfo, bAddFile )
//
//    lpH          - Pointer to the information structure used to process a
//                   connection to an archive file.
//    lpOldInfo    - Pointer to the compressed info before the file header
//                   was modified.
//    lpNewInfo    - Pointer to the compressed info that will be updated to
//                   contain the new position of the compressed file.
//    bAddFile     - This flag is TRUE if the file is that is being "moved"
//                   is being "added" to the destination file.
//
// This function tries to copy the compressed file header and data from
// it's current location to the logical end of file.  This function is a
// little sneaky because it can be used to copy the aforementioned data
// from one file to another.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_MoveFileToEnd(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_COMP_INFO   lpOldInfo,
   LPZIP_COMP_INFO   lpNewInfo,
   BOOL              bAddFile
)

{
   LPZIP_ARCHIVE_DIR    lpDir    = lpH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpOutpt  = lpDir->fiArchvFile.lpFBuf;
   LONG                 lOrigEOF = FsvBufIO_fRtrnEOFPos( lpOutpt );
   ZIP_ERRCODE          eErr;

   /* Point to available space for the compressed file to move. */
   if ( ( eErr = FsvBufIO_fSeek( lpOutpt, lpDir->SI.lAvailPostn, SEEK_SET,
                                 lpH->lpCritCBFunc, lpH->dwCritCBData ) ) != lpDir->SI.lAvailPostn )
   {
      return( eErr );
   }

   /* The header affiliated with this baby needs to be moved. */
   lpNewInfo->bDirty = TRUE;

   /* Try to write out the new file header and then move the  */
   /* compressed data.  Also, make sure there is enough space */
   /* for additional EOF info.                                */
   if ( ( eErr = Zip_WriteFileHeadr( lpH, &lpOutpt, lpNewInfo, TRUE ) ) == ZIP_NO_ERROR &&
        ( eErr = Zip_CopyDataBlk( lpH, &lpNewInfo->dbComprssdFile, lpOutpt, FALSE ) ) == ZIP_NO_ERROR )
   {
      LONG lBytsToFree   = 0;
//    LONG lExtraRemByts = lpNewInfo->dbExtra1.lSize + lpNewInfo->dbRemark.lSize;
      LONG lExtraRemByts = lpNewInfo->dbExtra1.lSize;
      LONG lEOFByts      = ( LONG ) Zip_CalcEndInfoByts( lpDir, lpNewInfo );

      if ( !bAddFile )
      {
//       lExtraRemByts -= ( lpOldInfo->dbExtra1.lSize + lpOldInfo->dbRemark.lSize );
         lExtraRemByts -= lpOldInfo->dbExtra1.lSize;
         lBytsToFree    = Zip_CalcOldComprssdFileByts( lpOldInfo ) + ( LONG ) Zip_CalcEndInfoByts( lpDir, lpOldInfo );
      }

      eErr = Zip_GrowthCheck( lpH, &lpOutpt, lExtraRemByts, lEOFByts, lBytsToFree );
   }

   if ( eErr != ZIP_NO_ERROR )
   /* Restore logical end of file. */
   {
      FsvBufIO_fSetEOF( lpOutpt, lOrigEOF, lpH->lpCritCBFunc, lpH->dwCritCBData );
   }

   return( eErr );
} /* Zip_MoveFileToEnd */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_XFerCompFile( lpSrcH, lpDstH, lpOldInfo, lpNewInfo )
//
//    lpSrcH    - Pointer to the information structure used to process a
//                connection to the source archive file.
//    lpDstH    - Pointer to the information structure used to process a
//                connection to the destination archive file.
//    lpSrcInfo - Pointer to the compressed info for the file in the source
//                archive file.
//    lpDstInfo - Pointer to the compressed info for the file in the
//                destination archive file.
//    bAddFile  - This flag is TRUE if the file is that is being "moved"
//                is being "added" to the destination file.
//
// This function tries to copy the compressed file header and data from
// it's current location in one archive file to the logical end of another
// archive file.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_XFerCompFile(
   LPZIP_HANDLE_INFO lpSrcH,
   LPZIP_HANDLE_INFO lpDstH,
   LPZIP_COMP_INFO   lpSrcInfo,
   LPZIP_COMP_INFO   lpDstInfo,
   BOOL              bAddFile
)

{
   LPZIP_ARCHIVE_DIR    lpDstDir      = lpDstH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpOutpt       = lpDstDir->fiArchvFile.lpFBuf;
   LONG                 lOrigEOF      = FsvBufIO_fRtrnEOFPos( lpOutpt );
   LONG                 lBytsToFree   = 0;
   LONG                 lExtraRemByts = 0;
   ZIP_ERRCODE          eErr;
   LONG                 lEOFByts;

   /* Point to available space for the compressed file to move. */
   if ( ( eErr = FsvBufIO_fSeek( lpOutpt, lpDstDir->SI.lAvailPostn, SEEK_SET,
                                 lpDstH->lpCritCBFunc, lpDstH->dwCritCBData ) ) != lpDstDir->SI.lAvailPostn )
   {
      return( eErr );
   }

   if ( !bAddFile )
   {
//    lExtraRemByts = 0 - ( lpDstInfo->dbExtra1.lSize + lpDstInfo->dbRemark.lSize );
      lExtraRemByts = 0 - ( lpDstInfo->dbExtra1.lSize );
      lBytsToFree   = Zip_CalcOldComprssdFileByts( lpDstInfo ) + ( LONG ) Zip_CalcEndInfoByts( lpDstDir, lpDstInfo );
   }

   /* Remark data has to be manually extracted and added. */
// lpDstInfo->dbRemark.lSize = 0;
//-*****FIX*****
// lpDstInfo->dbRemark.lSize = 0;
//-*****FIX*****

   /* Reference the extra data in the source file (sneaky). */
   lpDstInfo->dbExtra1 = lpSrcInfo->dbExtra1;

// if ( lpSrcDir->eArchvTyp != ZIP_FORMAT_ZIP || lpDstDir->eArchvTyp != ZIP_FORMAT_ZIP )
// /* Extra data for ARC/PAK files has to be manually extracted and added. */
// {
//    lpDstInfo->dbExtra1.lSize = 0;
// }
// else
// /* Transfer the extra data from the source archive to the */
// /* destination archive.                                   */
// {
//    lpDstInfo->dbExtra1 = lpSrcInfo->dbExtra1;
// }

   /* Compute area required by the new compressed file. */
   lExtraRemByts += lpDstInfo->dbExtra1.lSize;
   lEOFByts       = ( LONG ) Zip_CalcEndInfoByts( lpDstDir, lpDstInfo );

   /* The header affiliated with this baby needs to be moved. */
   lpDstInfo->bDirty = TRUE;

   /* Reference the compressed data in the source file (sneaky). */
   lpDstInfo->dbComprssdFile = lpSrcInfo->dbComprssdFile;

   /* For ARC/PAK files write the extra data before the header. */
   if ( lpDstDir->eArchvTyp == ZIP_FORMAT_ZIP || lpDstDir->eArchvTyp == ZIP_FORMAT_LHA ||
        ( eErr = Zip_CopyDataBlk( lpSrcH, &lpDstInfo->dbExtra1, lpOutpt, FALSE ) ) == ZIP_NO_ERROR )
   {
      /* Try to write out the destination file header. */
      if ( ( eErr = Zip_WriteFileHeadr( lpDstH, &lpOutpt, lpDstInfo, FALSE ) ) == ZIP_NO_ERROR )
      {
         /* For ZIP files write the source extra data after the header. */
         if ( lpDstDir->eArchvTyp != ZIP_FORMAT_ZIP ||
              ( eErr = Zip_CopyDataBlk( lpSrcH, &lpDstInfo->dbExtra1, lpOutpt, FALSE ) ) == ZIP_NO_ERROR )
         {
            /* Copy the compressed data. */
            if ( ( eErr = Zip_CopyDataBlk( lpSrcH, &lpDstInfo->dbComprssdFile, lpOutpt, TRUE ) ) == ZIP_NO_ERROR )
            /* Make sure there is enough space for  the end of */
            /* file data.                                     */
            {
               eErr = Zip_GrowthCheck( lpDstH, &lpOutpt, lExtraRemByts, lEOFByts,
                                       lBytsToFree );
            }
         }
      }
   }

   if ( eErr != ZIP_NO_ERROR )
   /* Restore file pointer and logical end of file. */
   {
      FsvBufIO_fSetEOF( lpOutpt, lOrigEOF, lpDstH->lpCritCBFunc, lpDstH->dwCritCBData );
   }

   return( eErr );
} /* Zip_XFerCompFile */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_RenameCompFile( lpH, lpFile, lpszNewPath, lpszNewName )
//
//    lpH         - Pointer to the information structure used to process a
//                  connection to an archive file.
//    lpFile      - Pointer to the temporary memory resident structure that
//                  will contain the info for the compressed file.
//    lpszNewPath - Pointer to the potential new file name.
//    lpszNewName - Pointer to the potential new path.
//
// This function tries to change the file name or the path of a compressed
// file.  For a ZIP file, the compressed header and file data will be moved
// to the "logical" end of file if the new name or path cause the header
// to "overflow".
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_NO_MEMORY - Not enough memory available for dynamic allocations.
//    FSVERR...     - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_RenameCompFile(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_COMP_INFO   lpFile,
   LPCTSTR            lpszNewPath,
   LPCTSTR            lpszNewName
)

{
   LPZIP_ARCHIVE_DIR lpDir       = lpH->lpDir;
   ZIP_ERRCODE       eErr        = ZIP_NO_ERROR;
   WORD              wNewPathLen = (WORD)FsvStr_GetByteCount( lpszNewPath );
   WORD              wOldLen     = (WORD)(FsvStr_GetByteCount( lpFile->szName ) + FsvStr_GetByteCount( lpFile->lpszPath ));
   WORD              wNewLen     = (WORD)FsvStr_GetByteCount( lpszNewName ) + wNewPathLen;
   ZIP_COMP_INFO     fiCompFile  = *lpFile;

   if ( lstrcmpi( lpszNewPath, fiCompFile.lpszPath ) != 0 )
   /* A new path has been specified. */
   {
      if ( ( fiCompFile.lpszPath = ( LPTSTR ) Mem_SubMalloc( wNewPathLen + 1 ) ) == NULL )
      {
         return( ZIP_NO_MEMORY );
      }
   }

   /* Update the new copy of the compressed info. */
   fiCompFile.bDirty = TRUE;
   lstrcpy( fiCompFile.szName, lpszNewName );
   lstrcpy( fiCompFile.lpszPath, lpszNewPath );

   if ( wNewLen > wOldLen &&
        ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP || lpDir->eArchvTyp == ZIP_FORMAT_LHA ) )
   /* Bummer, the compressed header info has overflowed, */
   /* so move the compressed header and data need to the */
   /* logical end of file.                               */
   {
      eErr = Zip_MoveFileToEnd( lpH, lpFile, &fiCompFile, FALSE );
   }
   else
   /* Make sure the new file name or path won't cause overflow. */
   if ( ( eErr = FsvBufIO_fSeek( lpDir->fiArchvFile.lpFBuf, lpDir->SI.lAvailPostn,
                                 SEEK_SET, lpH->lpCritCBFunc,
                                 lpH->dwCritCBData ) ) == lpDir->SI.lAvailPostn )
   {
      eErr = Zip_GrowthCheck( lpH, &lpDir->fiArchvFile.lpFBuf, 0, wNewLen - min( wOldLen, wNewLen ),
                              wOldLen - min( wNewLen, wOldLen ) );
   }

   if ( eErr == ZIP_NO_ERROR )
   /* The operation was successful. */
   {
      if ( fiCompFile.lpszPath != lpFile->lpszPath )
      /* Free the old copy of the path. */
      {
         Zip_FreePathChk( lpFile->lpszPath );
      }

      *lpFile = fiCompFile;
   }
   else
   {
      if ( fiCompFile.lpszPath != lpFile->lpszPath )
      /* Free the copy of the new path that was allocated. */
      {
         Mem_SubFree( fiCompFile.lpszPath );
      }
   }

   return( eErr );
} /* Zip_RenameCompFile */
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_CreateDataBlk( lpH, lpDataBlk, lpBuf, wBufSiz,
//                                bDoGrwthChk )
//
//    lpH         - Pointer to the information structure used to process a
//                  connection to an archive file.
//    lpDataBlk   - Pointer to the data block structure that will be
//                  modified if the operation is successful.
//    lpBuf       - Pointer to the buffer containing data for the new
//                  data block.
//    wBufSiz     - Size of the new data block.
//    bDoGrwthChk - If this flag is TRUE and the new data block is successfully
//                  copied then a check will be done to make sure that there
//                  is still enough space for the end of file data.
//
// This function will fill out the supplied "data block" structure and try
// to copy the specified remark/extra-data to the end of the archive file.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    FSVERR...     - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_CreateDataBlk(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_DATA_BLK    lpDataBlk,
   LPVOID            lpBuf,
   WORD              wBufSiz,
   BOOL              bDoGrwthChk
)

{
   LPZIP_ARCHIVE_DIR    lpDir    = lpH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpOutpt  = lpDir->fiArchvFile.lpFBuf;
   LONG                 lOrigEOF = FsvBufIO_fRtrnEOFPos( lpOutpt );
   LONG                 lStart   = ( bDoGrwthChk ) ? lpDir->SI.lAvailPostn : FsvBufIO_fTell( lpOutpt );
   WORD                 wCurDisk = lpDir->SI.wCurDisk;
   ZIP_ERRCODE          eErr;

   // Point to available space for new data block.
   if ( ( eErr = FsvBufIO_fSeek( lpOutpt, lStart, SEEK_SET, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) != lStart )
   {
      return( eErr );
   }

   // Try to write out the new data block.
   if ( ( eErr = Zip_fArchiveWrt( lpH, &lpOutpt, lpBuf, wBufSiz ) ) == ( LONG ) wBufSiz )
   {
      if ( bDoGrwthChk )
      // Ensure that there is enough space available for end of file info.
      {
         WORD wHeadrByts = ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP ) ? 0 : sizeof( ZIP_PAK_HEADR );

         eErr = Zip_GrowthCheck( lpH, &lpOutpt, ( LONG ) wBufSiz - lpDataBlk->lSize,
                                 wBufSiz + wHeadrByts,
                                 lpDataBlk->lSize + ( ( lpDataBlk->lSize == 0 ) ? 0 : wHeadrByts ) );
      }
      else
      // We be kool.
      {
         eErr = ZIP_NO_ERROR;
      }
   }

   if ( eErr == ZIP_NO_ERROR )
   // Update the data block info.
   {
      lpDataBlk->lStart    = lStart;
      lpDataBlk->lSize     = wBufSiz;
      lpDataBlk->wDiskNum  = wCurDisk;
      lpDataBlk->hbFleIndx = ZIP_ARCHV_HANDLE;
   }
   else
   if ( !FsvIO_IsCritErr( eErr ) )
   // Restore logical end of file.
   {
      Zip_fArchiveSetEOF( lpH, &lpOutpt, wCurDisk, lStart );
   }

   return( eErr );
} // Zip_CreateDataBlk
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_SetFileInfo( lpH, lpFile, eInfoType, lpBuf, wBufSiz )
//
//    lpH        - Pointer to the information structure used to process a
//                 connection to an archive file.
//    lpFile     - Pointer to the info for the compressed file to modify.
//    eInfoType  - The type of information to return.
//    lpBuf      - Pointer to the buffer that will contain the info.  Can be
//                 NULL.
//    wBufSiz    - The size of the info buffer.
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

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_SetFileInfo(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_COMP_INFO   lpFile,
   ZIP_INFO          eInfoType,
   LPVOID            lpBuf,
   WORD              wBufSiz
)

{
   ZIP_ERRCODE       eErr  = ZIP_NO_ERROR;
   LPZIP_ARCHIVE_DIR lpDir = lpH->lpDir;

   if ( wBufSiz == 0 )
   // Nothing to do.
   {
      return( ZIP_NO_ERROR );
   }

   if ( lpDir->SI.fIsSplit &&
        ( eInfoType == ZIP_FILENAME ||
          eInfoType == ZIP_PATHNAME ||
          eInfoType == ZIP_EXTRAINFO ) )
   // These operations are not allowed for ZIP files that span multiple
   // disks.
   {
      return( ZIP_CANT_DO_FOR_MDZF );
   }

   // Handle the various data fields.
   switch ( eInfoType )
   {
      case ZIP_FILENAME:
         eErr = Zip_RenameCompFile( lpH, lpFile, lpFile->lpszPath,
                                    ( LPCTSTR ) lpBuf );
      break;

      case ZIP_PATHNAME:
         eErr = Zip_RenameCompFile( lpH, lpFile, ( LPCTSTR ) lpBuf,
                                    lpFile->szName );
      break;

      case ZIP_DATE:
        if ( wBufSiz >= sizeof( lpFile->tiTime ) )
        {
           // Store the new time.
           _fmemcpy( &lpFile->tiTime, lpBuf, sizeof( lpFile->tiTime ) );

           if ( wBufSiz >= sizeof( lpFile->tiTime ) + sizeof( lpFile->diDate ) )
           // Store the new date.
           {
             _fmemcpy( &lpFile->diDate, ( LPBYTE ) lpBuf + sizeof( lpFile->tiTime ),
                       sizeof( lpFile->diDate ) );
           }
        }
      break;

      case ZIP_COMMENT:
         if ( lpDir->eArchvTyp != ZIP_FORMAT_LHA )
         // Try to store the comment at the end of the file.
         {
            LPTSTR lpszTmp;

//          eErr = Zip_CreateDataBlk( lpH, &lpFile->dbRemark, lpBuf, wBufSiz,
//                                    TRUE );

            if ( ( lpszTmp = ( LPTSTR ) Mem_SubMalloc( wBufSiz + 1 ) ) == NULL )
            {
               eErr = ZIP_NO_MEMORY;
            }
            else
            {
               Zip_FreeRemarkChk( lpFile->lpszRemark );
               lpFile->lpszRemark = lpszTmp;
               _fmemcpy( lpszTmp, lpBuf, wBufSiz );
               lpszTmp[ wBufSiz ] = '\0';
            }
         }
      break;

      case ZIP_EXTRAINFO:
         if ( lpDir->eArchvTyp != ZIP_FORMAT_LHA )
         {
            // Get the size of the extra data for all of the compressed files
            // contained in the ZIP file.
            if ( ( eErr = Zip_ArchvVrfyChk( lpH ) ) != ZIP_NO_ERROR )
            {
               return( eErr );
            }

            if ( ( LONG ) wBufSiz <= lpFile->dbExtra1.lSize )
            // Simply overwrite the existing extra data.
            {
               if ( ( eErr = FsvBufIO_fWrite( lpDir->fiArchvFile.lpFBuf, lpBuf, wBufSiz, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == ( LONG ) wBufSiz )
               {
                  LONG lDeltaByts = lpFile->dbExtra1.lSize - ( LONG ) wBufSiz;

                  if ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP )
                  // The file header will have to be rewritten.
                  {
                     lpFile->bDirty = TRUE;
                  }

                  // Update state info.
                  lpFile->dbExtra1.lSize   = wBufSiz;
                  lpDir->SI.lExtraRemByts -= lDeltaByts;
                  lpDir->SI.lFreedByts    += lDeltaByts;
                  lpDir->SI.lEOFByts      -= lDeltaByts;
               }
            }
            else
            // Store extra data at the end of the file.
            {
               ZIP_COMP_INFO fiFileInfo = *lpFile;
               BOOL          bIsZipFile = ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP );

               if ( ( !bIsZipFile ||
                      ( eErr = FsvBufIO_fSeek( lpDir->fiArchvFile.lpFBuf, lpDir->SI.lAvailPostn, SEEK_SET, lpH->lpCritCBFunc, lpH->dwCritCBData ) ) == lpDir->SI.lAvailPostn ) &&
                    ( eErr = Zip_CreateDataBlk( lpH, &fiFileInfo.dbExtra1, lpBuf,
                                                wBufSiz, !bIsZipFile ) ) == ZIP_NO_ERROR )
               {
                  if ( bIsZipFile )
                  // Because of overflow, try to move the file header, special
                  // data, and the compressed file to the end of the file.
                  {
                     eErr = Zip_MoveFileToEnd( lpH, lpFile, &fiFileInfo, FALSE );
                  }

                  if ( eErr == ZIP_NO_ERROR )
                  // Operation successful, update the file info record.
                  {
                     *lpFile = fiFileInfo;
                  }
               }
            }
         }
      break;

      default:
         eErr = ZIP_BAD_ARG;
      break;
   }

   return( eErr );
} // Zip_SetFileInfo
#endif          // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_VerifyFileHeadr( lpH, lpFile )
//
//    lpH    - Pointer to the information structure used to process a
//             connection to an archive file.
//    lpFile - Pointer to the compressed file structure to evaluate.
//
// This function only applies to ZIP files.  This function ensures that the
// critical data stored in the header for the compressed file is the same as
// the data that was stored in the central directory.  This function will
// also update the "extra data block" for the compressed file.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - File header data didn't match the central directory
//                    data.
//    FSVERR...     - The appropriate error code returned by File Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_VerifyFileHeadr(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_COMP_INFO   lpFile
)

{
   ZIP_ERRCODE          eErr;
   LPZIP_ARCHIVE_DIR    lpDir   = lpH->lpDir;
   LPFSVBUFIO_FBUF_INFO lpOutpt = lpDir->fiArchvFile.lpFBuf;

   if ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP )
   {
      ZIP_ZIP_HEADR zfhFileHeadr;

      if ( ( eErr = Zip_fArchiveSeek( lpH, &lpOutpt, lpFile->wDiskNum, lpFile->lCompHeadrStrt, SEEK_SET ) ) != lpFile->lCompHeadrStrt ||
           ( eErr = Zip_fArchiveReadByts( lpH, &lpOutpt, &zfhFileHeadr, sizeof( zfhFileHeadr ) ) ) != ZIP_NO_ERROR )
      // Bad news.  File i/o error detected
      {
         return( eErr );
      }

      if ( zfhFileHeadr.lFileSignature     != ZIP_FILE_SIGNATURE           ||
           zfhFileHeadr.shData.lComprsdSiz != lpFile->dbComprssdFile.lSize ||
           zfhFileHeadr.shData.lOrigSiz    != lpFile->lOrigSiz             ||
           zfhFileHeadr.shData.lCRC        != lpFile->lCRC )
      // Bad news.  The file data stored in the central directory is
      // different than the file data stored compressed file header.
      {
         return( ZIP_DIR_ERROR );
      }

      // I have to do a seek because the archive file can span multiple
      // floppies.
      if ( ( eErr = Zip_fArchiveAdvance( lpH, &lpOutpt, zfhFileHeadr.shData.wNameSiz ) ) < 0 )
      // Bad news.  File i/o error detected.
      {
         return( eErr );
      }

      // Construct record pertaining to the 1st copy of the extra data.
      lpFile->dbExtra1.lSize     = zfhFileHeadr.shData.wExtraSiz;
      lpFile->dbExtra1.lStart    = eErr;
      lpFile->dbExtra1.wDiskNum  = lpDir->SI.wCurDisk;
      lpFile->dbExtra1.hbFleIndx = ZIP_ARCHV_HANDLE;

      // I have to do a seek because the archive file can span multiple
      // floppies.
      if ( ( eErr = Zip_fArchiveAdvance( lpH, &lpDir->fiArchvFile.lpFBuf, zfhFileHeadr.shData.wExtraSiz ) ) < 0 )
      // Bad news.  File i/o error detected
      {
         return( eErr );
      }

      // Compute the start of the compressed file based on the
      // local file header.
      lpFile->dbComprssdFile.lStart   = eErr;
      lpFile->dbComprssdFile.wDiskNum = lpDir->SI.wCurDisk;

      // Account for the extra data stored with the file header.
      lpDir->SI.lExtraRemByts += zfhFileHeadr.shData.wExtraSiz;
   }

   // Only need to do this once.
   lpFile->bHeadrVrfd = TRUE;

   return( ZIP_NO_ERROR );
} // Zip_VerifyFileHeadr


//==========================================================================
//
// ZIP_ERRCODE Zip_VerifyArchv( lpH )
//
//    lpH - Pointer to the information structure used to process a
//          connection to an archive file.
//
// This function only applies to ZIP files.  This function ensures that the
// critical data stored in the header for all of the compressed files is the
// same as the data that was stored in the central directory.  This function
// will also update the "extra data block" for all of the compressed files.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_DIR_ERROR - File header data didn't match the central directory
//                    data.
//    FSVERR...     - The appropriate error code returned by File Services.
//
// Note:  I will only display the hour glass cursor if the "progress
//        callback pointer" is not NULL.  If this pointer is NULL then the
//        caller has assummed responsibility for indicating that a time
//        comsuming operation is underway.
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_VerifyArchv(
   LPZIP_HANDLE_INFO lpH
)

{
   ZIP_ERRCODE       eErr      = ZIP_NO_ERROR;
   LPZIP_ARCHIVE_DIR lpDir     = lpH->lpDir;
   BOOL              bShowProg = ( lpH->lpProgCBFunc != NULL );

   if ( lpDir->eArchvTyp == ZIP_FORMAT_ZIP && !lpDir->bArchvVrfd && !lpDir->SI.fIsSplit )
   {
      LPLPZIP_COMP_INFO lplpFiles = lpDir->lplpCompFiles;
      WORD              wIndx;
      HCURSOR           hOrigCurs;
      HCURSOR           hWaitCurs;

      if ( bShowProg )
      /* Display the hour glass cursor while "verifying zip headers". */
      {
         hWaitCurs = LoadCursor( NULL, MAKEINTRESOURCE( IDC_WAIT ) );
         hOrigCurs = SetCursor( hWaitCurs );
      }

      for ( wIndx = lpDir->wFileCount; wIndx > 0; wIndx--, lplpFiles++ )
      {
         if ( ( eErr = Zip_FileVrfyChk( lpH, lplpFiles[ 0 ] ) ) != ZIP_NO_ERROR )
         {
            break;
         }
      }

      if ( bShowProg )
      /* Restore the original cursor. */
      {
         SetCursor( hOrigCurs );
      }
   }

   /* Only need to do this once. */
   lpDir->bArchvVrfd = ( eErr == ZIP_NO_ERROR );

   return( eErr );
} /* Zip_VerifyArchv */
#endif          // NOT NAV95



/*
                         Excerpt from AppNote.Txt
                         ========================


    Each encrypted file has an extra 12 bytes stored at the start of
    the data area defining the encryption header for that file.  The
    encryption header is originally set to random values, and then
    itself encrypted, using 3, 32-bit keys.  The key values are
    initialized using the supplied encryption password.  After each byte
    is encrypted, the keys are then updated using psuedo-random number
    generation techniques in combination with the same CRC-32 algorithm
    used in PKZIP and described elsewhere in this document.

    The following is the basic steps required to decrypt a file:

    1) Initialize the three 32-bit keys with the password.
    2) Read and decrypt the 12-byte encryption header, further
       initializing the encryption keys.
    3) Read and decrypt the compressed data stream using the
       encryption keys.


    Step 1 - Initializing the encryption keys
    -----------------------------------------

    Key(0) <- 305419896
    Key(1) <- 591751049
    Key(2) <- 878082192

    loop for i <- 0 to length(password)-1
   update_keys(password(i))
    end loop


    Where update_keys() is defined as:


    update_keys(char):
      Key(0) <- crc32(key(0),char)
      Key(1) <- Key(1) + (Key(0) & 000000ffH)
      Key(1) <- Key(1) * 134775813 + 1
      Key(2) <- crc32(key(2),key(1) >> 24)
    end update_keys


    Where crc32(old_crc,char) is a routine that given a CRC value and a
    character, returns an updated CRC value after applying the CRC-32
    algorithm described elsewhere in this document.


    Step 2 - Decrypting the encryption header
    -----------------------------------------

    The purpose of this step is to further initialize the encryption
    keys, based on random data, to render a plaintext attack on the
    data ineffective.


    Read the 12-byte encryption header into Buffer, in locations
    Buffer(0) thru Buffer(11).

    loop for i <- 0 to 11
   C <- buffer(i) ^ decrypt_byte()
   update_keys(C)
   buffer(i) <- C
    end loop


    Where decrypt_byte() is defined as:


    unsigned TCHAR decrypt_byte()
   local unsigned short temp
   temp <- Key(2) | 2
   decrypt_byte <- (temp * (temp ^ 1)) >> 8
    end decrypt_byte


    After the header is decrypted, the last two bytes in Buffer
    should be the high-order word of the CRC for the file being
    decrypted, stored in Intel low-byte/high-byte order.  This can
    be used to test if the password supplied is correct or not.


    Step 3 - Decrypting the compressed data stream
    ----------------------------------------------

    The compressed data stream can be decrypted as follows:


    loop until done
   read a charcter into C
   Temp <- C ^ decrypt_byte()
   update_keys(temp)
   output Temp
    end loop
*/



//==========================================================================
//
// ULONG Zip_Mul32( ulArg1, ulArg2 )
//
//    ulArg1 -
//    ulArg2 -
//
// This function returns ...
//
//==========================================================================

ULONG INLINE Zip_Mul32(
   ULONG ulArg1,
   ULONG ulArg2
)

{
   USHORT u1, u0, v1, v0;

   u1 = ( USHORT ) ( ulArg1 >> 16 );
   u0 = ( USHORT ) ulArg1;
   v1 = ( USHORT ) ( ulArg2 >> 16 );
   v0 = ( USHORT ) ulArg2;

   return( ( ( ULONG ) ( ( u1 * v0 ) + ( u0 * v1 ) ) << 16 ) + ( ( ULONG ) u0 * v0 ) );
}


//==========================================================================
//
// VOID Zip_UpdateKeys( lpKeys, bCurByt )
//
//    lpKeys  - Pointer to structure containing the ZIP encryption keys.
//    bCurByt - The byte to update the "keys" with.
//
// This function updates the "encryption keys" with a byte from the password.
//
//==========================================================================

VOID WINAPI Zip_UpdateKeys(
   LPZIP_KEYS lpKeys,
   BYTE       bCurByt
)

{
   BYTE bByt;

   lpKeys->ulKey1 = crc32( &bCurByt, 1, lpKeys->ulKey1 );
   lpKeys->ulKey2 = Zip_Mul32( lpKeys->ulKey2 + ( BYTE ) lpKeys->ulKey1, 0x8088405 ) + 1;
   bByt           = ( BYTE ) ( lpKeys->ulKey2 >> 24 );
   lpKeys->ulKey3 = crc32( &bByt, 1, lpKeys->ulKey3 );
}


//==========================================================================
//
// VOID Zip_EnDeCryptByts( lpKeys, lpBuf, wByts, fEncrypt )
//
//    lpKeys   - Pointer to structure containing the ZIP encryption keys.
//    lpBuf    - Pointer to the buffer to encrypt or decrypt.
//    wByts    - The number of bytes to encrypt or decrypt.
//    fEncrypt - If this flag is TRUE the bytes will be "encrypted" otherwise
//               they will be "decrypted".
//
// This function encrypts or decrypts a series of data bytes.
//
//==========================================================================

VOID WINAPI Zip_EnDeCryptByts(
   LPZIP_KEYS lpKeys,
   LPBYTE     lpBuf,
   WORD       wByts,
   BOOL       fEncrypt
)

{
   BYTE bCurByt;

   // Loop until all bytes have been "processed".
   for ( ; wByts > 0; wByts--, lpBuf++ )
   {
      // Encrypt/decrypt byte but use decrypted data to update keys.
      bCurByt = *lpBuf;
      *lpBuf ^= ( BYTE ) ( ( ( USHORT ) ( lpKeys->ulKey3 | 2 ) * ( USHORT ) ( ( lpKeys->ulKey3 | 2 ) ^ 1 ) ) >> 8 );

      if ( !fEncrypt )
      // ?????.
      {
         bCurByt = *lpBuf;
      }

      // Update the encryption keys.
      Zip_UpdateKeys( lpKeys, bCurByt );
   }

}



//==========================================================================
//
// ZIP_ERRCODE Zip_DecryptSetup( lpH, lpKeys, lpszKey, lCRC )
//
//    lpH     - Pointer to the information structure used to process a
//              connection to an archive file.
//    lpKeys  - Pointer to structure containing the ZIP encryption keys.
//    lpszKey - Pointer to the "key" to decrypt the file with.
//    lCRC    - The CRC for the compressed file data.
//
// This function initializes the encryption keys, decrypts the encryption
// header and checks to see if the password entered by the user is valid.
//
// Returns:
//    ZIP_NO_ERROR     - The operation was successful.
//    ZIP_BAD_PASSWORD - The specified password does not "match" the
//                       encryption header.
//    FSVERR...        - The appropriate error code returned by File Services.
//
//==========================================================================

ZIP_ERRCODE WINAPI Zip_DecryptSetup(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_KEYS        lpKeys,
   LPCTSTR            lpszKey,
   LONG              lCRC
)

{
   LPCTSTR           lpszTmp;
   ZIP_ENCRYPT_INFO sEncryptInfo;
   ZIP_ERRCODE      eErr;

   // Initialize the keys.
   lpKeys->ulKey1 = 0x12345678;
   lpKeys->ulKey2 = 0x23456789;
   lpKeys->ulKey3 = 0x34567890;

   // Update the encryption keys with the password.
   for ( lpszTmp = lpszKey; lpszTmp[ 0 ] != '\0'; lpszTmp++ )
   {
      Zip_UpdateKeys( lpKeys, lpszTmp[ 0 ] );
   }

   // Read the encryption header from zip file.
   if ( ( eErr = Zip_fArchiveReadByts( lpH, &lpH->lpDir->fiArchvFile.lpFBuf, &sEncryptInfo, sizeof( sEncryptInfo ) ) ) == ZIP_NO_ERROR )
   {
      // Decrypt the encryption header.
      Zip_EnDeCryptByts( lpKeys, ( LPBYTE ) &sEncryptInfo, sizeof( sEncryptInfo ), FALSE );

      if ( sEncryptInfo.bCrcHigh != ( BYTE ) ( lCRC >> 24 ) )
      // Bad news, the specified password doesn't "match".
      {
         eErr = ZIP_BAD_PASSWORD;
      }
   }

   return( eErr );
}



//==========================================================================
//
// ZIP_ERRCODE Zip_EncryptSetup( lpH, lpKeys, lpszKey, lCRC )
//
//    lpH     - Pointer to the information structure used to process a
//              connection to an archive file.
//    lpKeys  - Pointer to structure containing the ZIP encryption keys.
//    lpszKey - Pointer to the "key" to encrypt the file with.
//    lCRC    - The CRC for the compressed data.
//
// This function initializes the encryption keys and encrypts the encryption
// header.
//
// Returns:
//    ZIP_NO_ERROR - The operation was successful.
//    FSVERR...    - The appropriate error code returned by File Services.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_EncryptSetup(
   LPZIP_HANDLE_INFO lpH,
   LPZIP_KEYS        lpKeys,
   LPCTSTR            lpszKey,
   LONG              lCRC
)

{
   ZIP_ERRCODE      eErr;
   LPCTSTR           lpszTmp;
   ZIP_ENCRYPT_INFO sEncryptInfo = { "CPS FM v2x", 2, 5 };

   // Initialize the keys.
   lpKeys->ulKey1 = 0x12345678;
   lpKeys->ulKey2 = 0x23456789;
   lpKeys->ulKey3 = 0x34567890;

   // Update the encryption keys with the password.
   for ( lpszTmp = lpszKey; lpszTmp[ 0 ] != '\0'; lpszTmp++ )
   {
      Zip_UpdateKeys( lpKeys, lpszTmp[ 0 ] );
   }

   // Used to check for password validity.
   sEncryptInfo.bCrcHigh = ( BYTE )( lCRC >> 24 );

   // Encrypt the encryption header.
   Zip_EnDeCryptByts( lpKeys, ( LPBYTE ) &sEncryptInfo, sizeof( sEncryptInfo ), TRUE );


   // Write out the encryption header.
   if ( ( eErr = Zip_fArchiveWrt( lpH, &lpH->lpDir->fiArchvFile.lpFBuf, &sEncryptInfo,
                                  sizeof( sEncryptInfo ) ) ) == sizeof( sEncryptInfo ) )
   {
      eErr = ZIP_NO_ERROR;
   }

   return( eErr );
}
#endif  // NOT NAV95


//==========================================================================
//
// ZIP_ERRCODE Zip_EncryptZipFile( lpH, lplpFBuf, lpFile, lpszKey, bDsplyProg )
//
//    lpH        - Pointer to the information structure used to process a
//                 connection to an archive file.  This structure contains
//                 the pointer to the buffer that will be used transfer the
//                 data within the same file or across different files.  If
//                 the pointer to this structure is NULL then a temporary
//                 buffer will allocated upon entry and freed upon exit.
//    lplpFBuf   - Pointer to the pointer to the structure used to buffer
//                 file i/o for an archive file.  If the archive file is
//                 closed and re-opened because another diskette had to be
//                 inserted then *lplpFBuf will be updated.
//    lpFile     - Pointer to the structure used to describe the compressed
//                 file.
//    lpszKey    - Pointer to the "key" to encrypt the file with.
//    bDsplyProg - If this flag is TRUE then call the progress callback
//                 function (providing one exists) as the data block is
//                 being encrypted.
//
// This function encrypts the contents of a ZIP file.  The file has to be
// encrypted after it has been "compressed" because a portion of the CRC
// for the ZIP file is stored in the encryption header.
//
// Returns:
//    ZIP_NO_ERROR  - The operation was successful.
//    ZIP_NO_MEMORY - No memory available for the temporary buffer.
//    FSVERR...     - The appropriate error code returned by File Services.
//
// Notes:
//    1) The file pointer will be one byte after the compressed data that
//       was encrypted.
//    2) This function could potentially close the archive file on one diskette
//       and create a new one on another diskette.  If this happens *lplpFBuf
//       (and lpH->lpDir->fiArchvFile.lpFBuf) will be modified.
//
//==========================================================================

#ifndef NAV95
ZIP_ERRCODE WINAPI Zip_EncryptZipFile(
   LPZIP_HANDLE_INFO      lpH,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   LPZIP_COMP_INFO        lpFile,
   LPCTSTR                 lpszKey,
   BOOL                   bDsplyProg
)

{
   ZIP_ERRCODE          eErr;
   LPBYTE               lpBuf;
   WORD                 wBufSiz;
   ZIP_KEYS             sKeys;
   WORD                 wCurDisk;
   LONG                 lStart;
   LPZIP_ARCHIVE_DIR    lpDir       = lpH->lpDir;
   BOOL                 bAllocBuf   = ( lpDir->lpTmpBuf == NULL );
   LONG                 lNumByts    = lpFile->dbComprssdFile.lSize - sizeof( ZIP_ENCRYPT_INFO );
   LONG                 lTotlByts   = lNumByts;
   LONG                 lBytsCopied = 0;

   // Init the encryption header and write it out to the compressed
   // file data area.
   if ( ( eErr = Zip_fArchiveSeek( lpH, lplpFBuf, lpFile->dbComprssdFile.wDiskNum, lpFile->dbComprssdFile.lStart, SEEK_SET ) != lpFile->dbComprssdFile.lStart ) ||
        ( eErr = Zip_EncryptSetup( lpH, &sKeys, lpszKey, lpFile->lCRC ) ) != ZIP_NO_ERROR )
   // Bad news.  Unable to write to access the disk.
   {
      return( eErr );
   }

   // Either use the buffer provided or allocate a temporary one.
   if ( bAllocBuf )
   {
      wBufSiz = ( WORD ) min( ( LONG ) FSVBUFIO_FILE_BUF_SIZ, lNumByts );

      if ( ( lpBuf = ( LPBYTE ) Mem_SubMalloc( wBufSiz ) ) == NULL )
      // Bad news.  The temporary buffer could not be allocated.
      {
         return( ZIP_NO_MEMORY );
      }
   }
   else
   {
      lpBuf   = lpDir->lpTmpBuf;
      wBufSiz = lpDir->wTmpBufSiz;
   }

   // Store current file pointer position.
   lStart   = FsvBufIO_fTell( *lplpFBuf );
   wCurDisk = lpDir->SI.wCurDisk;

   // Loop to encrypt the ZIP file.
   do
   {
      WORD wByts = ( WORD ) min( ( LONG ) wBufSiz, lNumByts );

      // Read as many bytes from the source as possible.
      if ( ( eErr = Zip_fArchiveReadByts( lpH, lplpFBuf, lpBuf, wByts ) ) != FSV_SUCCESS )
      // Bad news, a disk error was detected.
      {
         break;
      }

      // Encrypt the data.
      Zip_EnDeCryptByts( &sKeys, lpBuf, wByts, TRUE );

      // Write the encrypted bytes back to the ZIP file.
      if ( ( eErr = Zip_fArchiveSeek( lpH, lplpFBuf, wCurDisk, lStart, SEEK_SET ) ) != lStart ||
           ( eErr = Zip_fArchiveWrt( lpH, lplpFBuf, lpBuf, wByts ) ) != ( LONG ) wByts ||
           ( lpDir->SI.fCanBeSplit && ( eErr = Zip_fFlushBuf( lpH, lplpFBuf ) ) != ZIP_NO_ERROR ) )
      // Bad news.  Unable to write to disk.
      {
         break;
      }

      // Update count and file position varaibles.
      lBytsCopied += wByts;
      lNumByts    -= wByts;
      lStart       = FsvBufIO_fTell( *lplpFBuf );
      wCurDisk     = lpDir->SI.wCurDisk;
      eErr         = ZIP_NO_ERROR;

      if ( bDsplyProg && lpH->lpProgCBFunc != NULL && lNumByts > 0 )
      // Inform the user of the progress of the encryption.
      {
         ZIP_PROGRESSINFO piProgInfo = { ( WORD ) ( lBytsCopied * 100 / lTotlByts ),
                                         ZIP_XFER_FILE, lpH->dwProgCBData };

         if ( lpH->lpProgCBFunc( &piProgInfo ) == ZIP_PROG_CB_ABORT )
         // The user wants to abort encrypting the  file.
         {
            eErr  = ZIP_ABORTED;
            break;
         }
      }
   }
   while ( lNumByts > 0 );

   if ( bAllocBuf )
   // Free the temporary transfer buffer.
   {
      Mem_SubFree( lpBuf );
   }

   return( eErr );
} // Zip_EncryptZipFile
#endif  // NOT NAV95

#else
// Stubs for Alpha

#include "wnfsv.h"
#include "wnzip.h"

ZIP_ERRCODE WINAPI ZipOpen(
   LPCTSTR                 lpszArchvFile,
   LPHZIP                 lphArchvFile,
   WORD                   wOpenOptns,
   LPZIP_PROG_CBFUNC      lpProgCBFunc,
   DWORD                  dwProgCBData,
   LPZIP_ERR_CBFUNC       lpErrCBFunc,
   DWORD                  dwErrCBData,
   LPFNZIP_SPLTDSK_CBFUNC lpfnSpltDskCB,
   DWORD                  dwSpltDskData
)
{
	return ZIP_BAD_ARG;
}

ZIP_ERRCODE WINAPI ZipGetInfo(
   HZIP     hArchvFile,
   ZIP_INFO eInfoType,
   LPVOID   lpBuf,
   WORD     wBufSize,
   LPWORD   lpwInfoByts
)
{
	return ZIP_BAD_ARG;
}

ZIP_ERRCODE WINAPI ZipGetFileInfo(
   HZIP     hArchvFile,
   LPCTSTR   lpszName,
   WORD     wIndex,
   ZIP_INFO eInfoType,
   LPVOID   lpBuf,
   WORD     wBufSize,
   LPWORD   lpwInfoByts
)
{
	return ZIP_BAD_ARG;
}

ZIP_ERRCODE WINAPI ZipExtractFile(
   HZIP   hArchvFile,
   LPCTSTR lpszName,
   WORD   wIndex,
   LPCTSTR lpszDestPath,
   LPCTSTR lpszDestName,
   BOOL   bUseStoredPath,
   LPCTSTR lpszKey
)
{
	return ZIP_BAD_ARG;
}

ZIP_ERRCODE WINAPI ZipClose(
   HZIP hArchvFile
)
{
	return ZIP_BAD_ARG;
}

BOOL WINAPI FsvUtil_IsZipFile(LPCSTR lpszName, LPWORD lpwType)
{
	return FALSE;
}

#endif
