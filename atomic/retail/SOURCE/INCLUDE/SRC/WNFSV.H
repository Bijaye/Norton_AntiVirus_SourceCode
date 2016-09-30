 /*
 * $Workfile::   wnfsv.h   $
 * $Revision::   1.0     $
 * $Modtime::   30 Jan 1996 15:46:54  $
 *
 * FILE SERVICES -- Global Header file
 *
 * LANGUAGE  : Microsoft C6.0
 * MODEL   : medium
 * ENVIRONMENT : Microsoft Windows 3.0 SDK
 *
 * This module contains global header information required to use the
 * file services system.
 *
 */

#ifndef FSV_H_INCLUDED
#define FSV_H_INCLUDED

#ifndef SYM_WIN
   #define SYM_WIN 1
#endif
#ifndef SYM_WIN32
   #define SYM_WIN32 1
#endif

#ifdef UNICODE
   #define WNetRestoreConnection  WNetRestoreConnectionW
#else
   #define WNetRestoreConnection  WNetRestoreConnectionA
#endif

#include <platform.h>
#include <file.h>
#include <disk.h>
#include <xapi.h>
#include <winsock.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(4)

//===========================================================================
//
//                              DEFINITIONS
//
//===========================================================================

#define FSV_EXPECTED_VERSION     0x00040003L

#define FSV_DLL_FILENAME         "SYMFSV4.DLL"

#define FSV_DATA_NONE            0     // Passed as the datatype for registering

//
// Drive type defines
//
#define FSV_DRIVE_NONE           0
#define FSV_DRIVE_FLOPPY525      1
#define FSV_DRIVE_FLOPPY35       2
#define FSV_DRIVE_FIXED          3
#define FSV_DRIVE_REMOTE         4
#define FSV_DRIVE_RAMDISK        5
#define FSV_DRIVE_CDROM          6
#define FSV_DRIVE_BERNOULLI      7
#define FSV_DRIVE_TAPE           8
#define FSV_DRIVE_STACKER        9
#define FSV_DRIVE_SUPERSTOR      10
#define FSV_DRIVE_COMPATICARD    11
#define FSV_DRIVE_PHANTOMREMOTE  12
#define FSV_DRIVE_UNKNOWN        13

//
// Enumeration function options defines
//
#define FSV_ENUMOPTION_NONE               0
#define FSV_ENUMOPTION_DIRS               0x0001
#define FSV_ENUMOPTION_FILES              0x0002
#define FSV_ENUMOPTION_RECURSE            0x0004
#define FSV_ENUMOPTION_READONLY           0x0010
#define FSV_ENUMOPTION_HIDDEN             0x0020
#define FSV_ENUMOPTION_SYSTEM             0x0040
#define FSV_ENUMOPTION_NETWORK            0x0080
#define FSV_ENUMOPTION_ZIPS               0x0100
#define FSV_ENUMOPTION_DELETED_NORM       0x0200
#define FSV_ENUMOPTION_DELETED_DESTROYED  0x0400
#define FSV_ENUMOPTION_DELETED_POOR       0x0800
#define FSV_ENUMOPTION_DELETED_ONLY       0x1000
#define FSV_ENUMOPTION_POPUPNOTIFY        0x2000
#define FSV_ENUMOPTION_FLATORDER          0x4000

//
// Compound Enumeration function options
//
#define FSV_ENUMOPTION_DELETED_ALL  ( FSV_ENUMOPTION_DELETED_NORM | FSV_ENUMOPTION_DELETED_DESTROYED | FSV_ENUMOPTION_DELETED_POOR )

//
// Enumeration callback function return value defines
//
#define FSV_ENUM_CONTINUE       0           // CONTINUE but DON'T ADD to LIST
#define FSV_ENUM_ABORT_BIT      0x0001
#define FSV_ENUM_DONTRECURSE    0x0002
#define FSV_ENUM_ABORT          0x0003
#define FSV_ENUM_ADD            0x0004
#define FSV_ENUM_ABORTLEVEL     0x0008

//
// wParam of Event notification message
//
#define FSV_EVENT_MESSAGECHECK      0x4a4e

//
// Specifies what happened to generate the current event.
//
#define FSV_EVENT_ACTION_NONE          0x0000
#define FSV_EVENT_ACTION_CREATE        0x0001   // Generated from internal events
#define FSV_EVENT_ACTION_DELETE        0x0002   // Generated from internal events
#define FSV_EVENT_ACTION_RENAME        0x0004   // Generated from internal events
#define FSV_EVENT_ACTION_MODIFY        0x0008   // Generated from internal events
#define FSV_EVENT_ACTION_SHARE         0x0010   // Generated from internal events
#define FSV_EVENT_ACTION_UNDELETE      0x0020   // Generated from internal events
#define FSV_EVENT_ACTION_PURGE         0x0040   // Generated from internal events
#define FSV_EVENT_ACTION_UNKNOWN       0x8000

//
// Specifies on what objects notifications are desired, and also specifies
// what object type the notification was generated on.
//
#define FSV_EVENT_OBJECT_NONE       0x0000
#define FSV_EVENT_OBJECT_DIRS       0x0001
#define FSV_EVENT_OBJECT_FILES      0x0002
#define FSV_EVENT_OBJECT_ARCHIVES   0x0004
#define FSV_EVENT_OBJECT_DRIVES     0x0008
#define FSV_EVENT_OBJECT_ARCCONTENT 0x0010
#define FSV_EVENT_OBJECT_FTPSITE    0x0020
#define FSV_EVENT_OBJECT_ALL        0x001f
#define FSV_EVENT_OBJECT_UNKNOWN    0x8000

//
// Specifies for how far down from a given path notifications are desired.
// Also specifies where from the registered path an event occurred.
//
#define FSV_EVENT_DOMAIN_NONE       0x0000
#define FSV_EVENT_DOMAIN_EXACT      0x0001
#define FSV_EVENT_DOMAIN_BELOWONE   0x0002
#define FSV_EVENT_DOMAIN_BELOWMANY  0x0004
#define FSV_EVENT_DOMAIN_BELOWALL   (FSV_EVENT_DOMAIN_BELOWONE|FSV_EVENT_DOMAIN_BELOWMANY)
#define FSV_EVENT_DOMAIN_ALL        (FSV_EVENT_DOMAIN_EXACT|FSV_EVENT_DOMAIN_BELOWALL)
#define FSV_EVENT_DOMAIN_UNKNOWN    0x8000

//
// Return values from a notification callback or a notification message.
//
#define FSV_EVENT_QUEUE_NO          0x00000000
#define FSV_EVENT_QUEUE_YES         0x00010000

//
// File/Directory attribute defines
//
#define FSV_ATTRIB_NONE             0x00000000
#define FSV_ATTRIB_READONLY         0x00000001
#define FSV_ATTRIB_HIDDEN           0x00000002
#define FSV_ATTRIB_SYSTEM           0x00000004
#define FSV_ATTRIB_DELETED          0x00000008       // *****NUKE*****
#define FSV_ATTRIB_DIRECTORY        0x00000010
#define FSV_ATTRIB_ARCHIVE          0x00000020
#define FSV_ATTRIB_SHAREABLE        0x00000080
#define FSV_ATTRIB_ZIP              0x00000100       // NOT DOS STANDARD
#define FSV_ATTRIB_DELETED_DOS      0x00000200       // NOT DOS STANDARD
#define FSV_ATTRIB_NETSHARED        0x00000400       // NOT DOS STANDARD
#define FSV_ATTRIB_ZIPCONTENTS      0x00000800       // NOT DOS STANDARD
#define FSV_ATTRIB_DELETED_SENTRY   0x00001000       // NOT DOS STANDARD
#define FSV_ATTRIB_DELETED_TRCKR    0x00002000       // NOT DOS STANDARD
#define FSV_ATTRIB_SERVER_ENTRY     0x00004000       // NOT DOS STANDARD
#define FSV_ATTRIB_NETHOOD_ENTRY    0x00008000       // NOT DOS STANDARD
#define FSV_ATTRIB_FTPGROUP_ENTRY   0x00010000       // NOT DOS STANDARD
#define FSV_ATTRIB_ENTIRENET_ENTRY  0x00020000       // NOT DOS STANDARD
#define FSV_ATTRIB_DRIVE_ENTRY      0x00040000       // NOT DOS STANDARD
#define FSV_ATTRIB_MYCOMPUTER_ENTRY 0x00080000       // NOT DOS STANDARD
#define FSV_ATTRIB_FTPSITE_ENTRY    0x00100000       // NOT DOS STANDARD

// Attribute no longer defined by C.
#define _A_VOLID  0x08

// 
// Novell network file attribute defines
//
#define FSV_NETATTRIB_NONE          (DWORD)FSV_ATTRIB_NONE
#define FSV_NETATTRIB_READONLY      (DWORD)FSV_ATTRIB_READONLY
#define FSV_NETATTRIB_HIDDEN        (DWORD)FSV_ATTRIB_HIDDEN
#define FSV_NETATTRIB_SYSTEM        (DWORD)FSV_ATTRIB_SYSTEM
#define FSV_NETATTRIB_EXECUTE       0x00000008L
#define FSV_NETATTRIB_DIRECTORY     (DWORD)FSV_ATTRIB_DIRECTORY
#define FSV_NETATTRIB_ARCHIVE       (DWORD)FSV_ATTRIB_ARCHIVE
#define FSV_NETATTRIB_SHAREABLE     (DWORD)FSV_ATTRIB_SHAREABLE
#define FSV_NETATTRIB_TRANSACTIONAL 0x00001000L
#define FSV_NETATTRIB_INDEXING      0x00002000L
#define FSV_NETATTRIB_AUDITREAD     0x00004000L
#define FSV_NETATTRIB_AUDITWRITE    0x00008000L
#define FSV_NETATTRIB_PURGE         0x00010000L     // Netware 386 only
#define FSV_NETATTRIB_RENAMEINHIBIT 0x00020000L     // Netware 386 only
#define FSV_NETATTRIB_DELETEINHIBIT 0x00040000L     // Netware 386 only
#define FSV_NETATTRIB_COPYINHIBIT   0x00080000L     // Netware 386 only

// 
// Some miscellaneous constants
//
#define FSV_MAXDRIVE             26  // Max # of drives possible
#define FSV_FILEPREFIX_LEN       8   // Max length of name portion of filename
#define FSV_FILEEXTENSION_LEN    3   // Max length of extension of filename
#define FSV_PATHBUF_LEN          269 // Max length of a path
#define FSV_MACHINENAMEBUF_LEN   64  // Max length for a machine name
#define FSV_DRIVELABELBUF_LEN    64  // Max length for a disk/drive label
#define FSV_DIRNAMEBUF_LEN       13  // Max length for a directory name (incl dot & null)
#define FSV_SFILENAMEBUF_LEN     13  // Max length for a short (8.3) file name (incl dot & null)
#define FSV_LFILENAMEBUF_LEN     260 // Max length for a long file name (incl dot & null)
#define FSV_DIRENTRYBUF_LEN      11  // Length of file/dir name as stored by DOS
#define FSV_OBJECTNAMEBUF_LEN    64  // Max length of ...
#define FSV_MAXDIRLEVELS         40  // Maximum possible level of nested subdirs
#define FSV_MAXDIRNAME_LEN       256 // Maximum length of a DOS directory
#define FSV_DIRENTRY_RESVRD_SIZ  10  // Number of reserved bytes in the DOS
                                     //  directory entry structure.

 //FTP return values
 //**************************************************
#define FTP_PRELIM               1
#define FTP_COMPLETE             2
#define FTP_CONTINUE             3
#define FTP_RETRY                4
#define FTP_ERROR                5

// Bitmap identifiers.

#define FSV_BMPSIZE_SMALL           1
#define FSV_BMPSIZE_MEDIUM          2
#define FSV_BMPSIZE_LARGE           3
#define FSV_BMPSIZE_MINI            4

#define FSV_FOLDER_NORMAL           6
#define FSV_FOLDER_OPEN             7
#define FSV_FOLDER_ZIP              10
#define FSV_FOLDER_OPENZIP          11

#define FSV_BMPMISC_PARENT          0
#define FSV_BMPMISC_CHECKMARK       1
#define FSV_BMPMISC_DIRBTN          2
#define FSV_BMPMISC_ONE             3
#define FSV_BMPMISC_TWO             4
#define FSV_BMPMISC_THREE           5
#define FSV_BMPMISC_FOUR            6
#define FSV_BMPMISC_FIVE            7
#define FSV_BMPMISC_STAR            8
#define FSV_BMPMISC_HIDSYS          9
#define FSV_BMPMISC_DELETED         10
#define FSV_BMPMISC_SHARED          11
#define FSV_BMPMISC_SHORTCUT        12
#define FSV_BMPMISC_COMPUTER        13
#define FSV_BMPMISC_FTPGROUP        14
#define FSV_BMPMISC_REMOTESERVER    15
#define FSV_BMPMISC_NETHOOD         16
#define FSV_BMPMISC_ENTIRENET       17
#define FSV_BMPMISC_WORKGROUP       18

#define FSV_BMP_BOXPLUS             1
#define FSV_BMP_BOXMINUS            2
#define FSV_BMP_BOXBLANK            3

#define FSV_DD_MULTIPLE          0x0001  // Drag and Drop cursor flags.
#define FSV_DD_SINGLE            0x0002
#define FSV_DD_MOVE              0x0004
#define FSV_DD_COPY              0x0008

// Constants used with processing the FAT.
#define FSV_CLSTR_AVAIL      0x0000
#define FSV_CLSTR_GOOD_LOW   0x0002
#define FSV_CLSTR_GOOD_HIGH  0xFFEF
#define FSV_CLSTR_RSRVD_LOW  0xFFF0
#define FSV_CLSTR_RSRVD_HIGH 0xFFF6
#define FSV_CLSTR_BAD        0xFFF7
#define FSV_CLSTR_EOF_LOW    0xFFF8
#define FSV_CLSTR_EOF_HIGH   0xFFFF
#define FSV_MAX_CLUSTERS     ( FSV_CLSTR_GOOD_HIGH + 1 )

// Constants that came from WINNET.H
#define WNBD_CONN_UNKNOWN       0x0
#define WNBD_CONN_DISKTREE      0x1
#define WNBD_CONN_PRINTQ        0x3
#define WNBD_MAX_LENGTH         0x80    // path length, includes the NULL

#define WNTYPE_DRIVE            1
#define WNTYPE_FILE             2
#define WNTYPE_PRINTER          3
#define WNTYPE_COMM             4

#define WNPS_FILE               0
#define WNPS_DIR                1
#define WNPS_MULT               2

#define WNNC_NET_TYPE                   0x0002
#define  WNNC_NET_NONE                          0x0000
#define  WNNC_NET_MSNet                         0x0100
#define  WNNC_NET_LanMan                        0x0200
#define  WNNC_NET_NetWare                       0x0300
#define  WNNC_NET_Vines                         0x0400
//#define  WNNC_NET_10NET                         0x0500
#define  WNNC_NET_Locus                         0x0600
#define  WNNC_NET_Sun_PC_NFS                    0x0700
#define  WNNC_NET_LANstep                       0x0800
//#define  WNNC_NET_9TILES                        0x0900
#define  WNNC_NET_LANtastic                     0x0A00
//#define  WNNC_NET_AS400                         0x0B00
//#define  WNNC_NET_FTP_NFS                       0x0C00
//#define  WNNC_NET_PATHWORKS                     0x0D00
#define  WNNC_NET_MultiNet       0x8000
#define   WNNC_SUBNET_NONE          0x0000
#define   WNNC_SUBNET_MSNet            0x0001
#define   WNNC_SUBNET_LanMan           0x0002
#define   WNNC_SUBNET_WinWorkgroups       0x0004
#define   WNNC_SUBNET_NetWare          0x0008
#define   WNNC_SUBNET_Vines            0x0010
#define   WNNC_SUBNET_Other            0x0080

#define WNNC_DIALOG                     0x0008
#define  WNNC_DLG_DeviceMode                    0x0001
#define  WNNC_DLG_BrowseDialog                  0x0002
#define  WNNC_DLG_ConnectDialog                 0x0004
#define  WNNC_DLG_DisconnectDialog              0x0008
#define  WNNC_DLG_ViewQueueDialog               0x0010
#define  WNNC_DLG_PropertyDialog                0x0020
#define  WNNC_DLG_ConnectionDialog              0x0040
#define  WNNC_DLG_PrinterConnectDialog    0x0080
#define  WNNC_DLG_SharesDialog         0x0100
#define  WNNC_DLG_ShareAsDialog        0x0200

#define WNNC_CONNECTION                 0x0006
#define  WNNC_CON_AddConnection                 0x0001
#define  WNNC_CON_CancelConnection              0x0002
#define  WNNC_CON_GetConnections                0x0004
#define  WNNC_CON_AutoConnect                   0x0008
#define  WNNC_CON_BrowseDialog                  0x0010
#define  WNNC_CON_RestoreConnection             0x0020

// Flags returned by MS-DOS 7's Get Volume Information function
#define FSV_FS_CASE_SENSITIVE    0x00000001   // Searches are case sensitive
#define FSV_FS_CASE_IS_PRESERVED 0x00000002   // Preserves case in directory entries
#define FSV_FS_UNICODE_ON_DISK   0x00000004   // Uses Unicode characters in file and directory names
#define FSV_FS_LFN_APIS          0x00004000   // Supports new long filename functions
#define FSV_FS_VOLUME_COMPRESSED 0x00008000   // Volume is compressed

// Defined Windows forgot.
#define FS_LFN_APIS              0x00004000  
#define FS_VOLUME_COMPRESSED     0x00008000  


// Date Time format specifiers for MS-DOS 7's Find First/Find Next routines
#define FSV_FINDEX_DATETIME_WIN32   0x0000
#define FSV_FINDEX_DATETIME_DOS     0x0001

// HIWORD return value flags from MS-DOS 7's Find First/Find Next FSV support routines
#define FSV_FINDEX_OK            0x0000   // All characters in the primary and alternate name fields in the find buffer were successfully converted from UNICODE to OEM/ANSI
#define FSV_FINDEX_CONV_PRIMARY  0x0001   // The primary name returned in the find buffer contains underscore characters where conversion failed
#define FSV_FINDEX_CONV_ALT      0x0002   // The alternate name returned in the find buffer contains underscore characters where conversion failed
#define FSV_FINDEX_ERROR         0xffff   // The call did not succeed (LFN API not supported, I/O error, etc.)


/* The following error codes are returned by functions that employ both */
/*  source and destination files, such as copy and move                     */

#define FSVERR_SOURCE_NOT_FOUND           (-0x02)
#define FSVERR_SOURCE_PATH_NOT_FOUND      (-0x03)
#define FSVERR_SOURCE_ACCESS_DENIED       (-0x05)
#define FSVERR_DESTINATION_NOT_FOUND      (-0x17)
#define FSVERR_DESTINATION_PATH_NOT_FOUND (-0x18)
#define FSVERR_DESTINATION_ACCESS_DENIED  (-0x19)
#define FSVERR_CYCLIC_COPY                (-0x1C)
#define FSVERR_USER_ABORT                 (-0x41)

/* The following error codes are returned by functions that employ only */
/*  one file, such as SetDateAndTime                                                    */

#define FSVERR_FILE_NOT_FOUND             (-0x02)
#define FSVERR_PATH_NOT_FOUND             (-0x03)
#define FSVERR_ACCESS_DENIED              (-0x05)
#define FSVERR_CURRENT_DIRECTORY          (-0x10)
#define FSVERR_NOT_THE_SAME_DEVICE        (-0x11)
#define FSVERR_DOS_MEMORY_NEEDED          (-0x08)
#define FSVERR_INVALID_FAT                (-0x0C)
#define FSVERR_NO_MORE_FILES              (-0x12)
#define FSVERR_NETWORK_REQUEST_REFUSED    (-0x47)
#define FSVERR_FILE_EXISTS                (-0x50)

/* The following error codes, inconsistent and incompatible with the */
/* previous error codes, are returned by the function                */
/* DOSWrap_59_ExtendedError()                                        */
#define FSV_EXTENDED_ERR_INVALID_DRIVE          0x14
#define FSV_EXTENDED_ERR_DRIVE_NOT_READY        0x15
#define FSV_EXTENDED_ERR_DISK_WRITE_PROTECTED   0x13
/* In general it is better to use the function FsvIO_ExtendedErr()   */

/* The following error codes are returned by all file functions that    */
/* return error codes beyond succeed fail                                                 */
#define FSVERR_NO_HANDLES_AVAILABLE       (-0x04)
#define FSVERR_IO_ERROR                   (-0x09)
#define FSVERR_OUT_OF_MEMORY              (-0x0A)
#define FSVERR_DISK_WRITE_PROTECTED       (-0x13)
#define FSVERR_INVALID_DRIVE              (-0x14)
#define FSVERR_DRIVE_NOT_READY            (-0x15)
#define FSVERR_NOTDOSDISK                 (-0x1A)
#define FSVERR_SECTOR_NOT_FOUND           (-0x1B)
#define FSVERR_READ_FAULT                 (-0x1E)
#define FSVERR_CRC_ERROR                  (-0x17)
#define FSVERR_GENERALFAILURE             (-0x1F)
#define FSVERR_SHARING_VIOLATION          (-0x20)
#define FSVERR_DISKFULL                   (-0x27)
#define FSVERR_DIRFULL                    (-0x52)
#define FSVERR_UNSUPPORTED_MEDIA          (-0x5F)

// ZIP Error codes.
typedef LONG ERRCODE;
typedef LONG ZIP_ERRCODE;

#define ZIP_NO_ERROR        FSV_SUCCESS // The ZIP function was successful.
#define ZIP_BAD_ARG          ( -0x100 ) // A parameter passed to one of the ZIP
                                        //  functions was invalid.
#define ZIP_CRC_MISMATCH     ( -0x101 ) // When the compressed file was expanded
                                        // a CRC mismatch was detected.
#define ZIP_DIR_ERROR        ( -0x102 ) // The contents of the archive file
                                        //  are unstable.
#define ZIP_NO_MEMORY        ( -0x103 ) // Insufficient memory for the ZIP
                                        //  operation.
#define ZIP_ABORTED          ( -0x104 ) // The archive file operation (compress,
                                        //  expand, etc.) was aborted by the
                                        //   user.
#define ZIP_ACCESS_DENIED    ( -0x105 ) // The archive file has already been
                                        //  opened with exclusive read or
                                        //   read/write priviledges.
#define ZIP_NO_MORE_HANDLES  ( -0x106 ) // All of the available ZIP handles
                                        //  have been used.
#define ZIP_UNKNOWN_METHOD   ( -0x107 ) // The compressied file has an
                                        //  unsupported compression method.
#define ZIP_EN_FILE_NO_KEY   ( -0x108 ) // The file to be expanded is encrypted
                                        //  and no "key" was specified.
#define ZIP_RUN_OVRFLW       ( -0x109 ) // The buffer allocated by the expansion
                                        //  funcion was not large enough for an
                                        //   encoded sequence to be expanded.
#define ZIP_UNRECNZD_ZIP     ( -0x10A ) // An unrecognized zip file has been
                                        //  encountered.
#define ZIP_FILE_NOT_FOUND  FSVERR_FILE_NOT_FOUND
#define ZIP_BAD_PASSWORD     ( -0x10B ) // The specified password does not
                                        //  "match" what is stored in the
                                        //   encryption header.
#define ZIP_CANT_DO_FOR_MDZF ( -0x10C ) // The operation (i.e. set extra data)
                                        //  is not allowed for a multi-disk
                                        //   archive file.
#define ZIP_NO_MULIT_DISK_CB ( -0x10D ) // A callback function must be passed
                                        //  into ZipOpen() so multi-disk
                                        //   archives files can be handled
                                        //    properly.

/* File Serives error codes. */
#define FSVERR_NOT_NET_DRIVE              (-0x200)
#define FSVERR_PATH_TOO_LONG              (-0x201)
#define FSVERR_CANNOT_EXPAND_PATH         (-0x202)
#define FSVERR_INVALID_PARTITION_TABLE    (-0x203)
#define FSVERR_CANT_USE_INT26             (-0x204)
#define FSVERR_EOF                        (-0x205)
#define FSVERR_TIMEOUT                    (-0x206)

// Undelete error codes.
#define FSVERR_BAD_SENTRY_FILE       (-0x300)
#define FSVERR_BAD_TRCKR_FILE        (-0x301)
#define FSVERR_CORRUPT_DIR_DATA      (-0x302)
#define FSVERR_SOME_CORRUPT_DIR_DATA (-0x303)
#define FSVERR_UDENG_ERROR           (-0x304)
#define FSVERR_NO_NET_PRIVLEGES      (-0x305)
#define FSVERR_CANT_UNDEL_TO_A_DIR   (-0x306)
#define FSVERR_NO_SENTRY_DIR         (-0x307)
#define FSVERR_NO_SENTRY_FILES       (-0x308)


// ftp error codes
#define FSVERR_FTP_CONNECTION_CLOSED     (-0x400)

/* Generic ERROR Codes */
#define FSVERR_GENERIC                      -1
#define FSV_ERROR                           -1
#define FSV_SUCCESS                         0

//
// Special characters in ASCII text files.
//
#define FSVBUFIO_CR  13
#define FSVBUFIO_LF  10
#define FSVBUFIO_EOF 26

//
// Standard size for the file i/o buffer.
//
#define FSVBUFIO_FILE_BUF_SIZ 32768U

/* The following function codes are used for the progress and error callback
   routines. */
typedef enum
{
   FSVFUNC_DELETEFILE, FSVFUNC_RENAMEFILE, FSVFUNC_RENAMEDIR,
   FSVFUNC_GETFILEATTRIBUTES, FSVFUNC_SETFILEATTRIBUTES,
   FSVFUNC_FASTGETDISKFREESPACE, FSVFUNC_GETDRIVELABEL, FSVFUNC_SETDRIVELABEL,
   FSVFUNC_MAKEDIR, FSVFUNC_REMOVEDIR, FSVFUNC_COPYFILE, FSVFUNC_MOVEFILE,
   FSVFUNC_WIPEFILE, FSVFUNC_GETDIRENTRY, FSVFUNC_DIREXISTS,
   FSVFUNC_FILEEXISTS, FSVFUNC_DOSUBDIRSEXIST, FSVFUNC_ISDIREMPTY,
   FSVFUNC_FINDFIRST, FSVFUNC_FINDNEXT, FSVFUNC_OPENFILE, FSVFUNC_CREATEFILE,
   FSVFUNC_READFILE, FSVFUNC_WRITEFILE
} FSVFUNC_ID;

//===========================================================================
//
//                              TYPEDEFS
//
//===========================================================================

// If necessary, define a far pointer to a boolean.
#ifndef LPBOOL
typedef BOOL FAR* LPBOOL;
#endif

// If necessary, define a far pointer to a constant boolean.
#ifndef LPCBOOL
typedef const BOOL FAR* LPCBOOL;
#endif

// If necessary, define a huge pointer to a byte.
#ifndef HPBYTE
typedef BYTE far* HPBYTE;
#endif

// If necessary, define a huge pointer to a constant byte.
#ifndef HPCBYTE
typedef const BYTE far* HPCBYTE;
#endif

// If necessary, define a huge pointer to a word.
#ifndef HPWORD
typedef WORD far* HPWORD;
#endif

// If necessary, define a huge pointer to a constant word.
#ifndef HPCWORD
typedef const WORD far * HPCWORD;
#endif

#ifndef LPCVOID
   typedef const void FAR* LPCVOID;
#endif

//
// Structure used to buffer i/o to and from a file on disk.
//
typedef struct
{
   HFILE  hFile;
   LPBYTE lpBuf;
   WORD   wBufSiz;
   WORD   wBytsUsed;
   WORD   wBufOffset;
   char   cDirty;
   LONG   lStrtBufPos;
   LONG   lEOFPos;
   LONG   lEOFCharPos;
}
FSVBUFIO_FBUF_INFO, FAR* LPFSVBUFIO_FBUF_INFO, FAR* FAR* LPLPFSVBUFIO_FBUF_INFO;
typedef const FSVBUFIO_FBUF_INFO  FAR* LPCFSVBUFIO_FBUF_INFO;

typedef struct _OWNERID
{
   LPSTR lpszOwnerName;                   // Netware use
   DWORD dwObjectID;
} OWNERID, FAR * LPOWNERID;

// This is the structure for an entry in a DOS directory.
typedef struct Tag_DOSDIRENTRY
{
   char  szFileName[ FSV_DIRENTRYBUF_LEN ];
                        // File name.
   BYTE  bAttrib;       // File attributes.
   BYTE  bReserved[ FSV_DIRENTRY_RESVRD_SIZ ];
                        // Reserved for DOS.
   WORD  wTime;         // Last access time.
   WORD  wDate;         // Last access date.
   WORD  wFirstCluster; // First cluster for the file.
   DWORD dwFileSize;    // File size in bytes.
}
FSVIO_DOSDIRENTRY, FAR* LPFSVIO_DOSDIRENTRY;
typedef const FSVIO_DOSDIRENTRY FAR* LPCFSVIO_DOSDIRENTRY;

/*  Structure used by the "FindFirst" and "FindNext" code. */
typedef struct _FSV_FIND   {
   //  First 21 bytes are reserved...
   BYTE              RESERVED[21];
   //  Standard DOS info follows
   BYTE              bAttributes;      //  File attributes
   WORD              wCreateTime;      //  Create time
   WORD              wCreateDate;      //  Create date
   DWORD             dwSize;           //  File size
   char              bFilename[13];    //  Filename
} FSV_FIND, FAR* LPFSV_FIND;

// Structure used in the WIN32_FIND_DATA structure.
// Same as FILETIME defined in MS-DOS 7 extensions.
#define FSV_FILETIME FILETIME
#define LPFSV_FILETIME LPFILETIME

// Structure used by the "FindFirstEx" and "FindNextEx" code.
// Same as WIN32_FIND_DATA defined under MS-DOS 7 extensions.
#define FSV_FINDLD   FINDDATA
#define LPFSV_FINDLD LPFINDDATA

typedef struct _FSVIO_NETSTRUCT {
   DWORD dwOwnerId;
   DWORD dwModifierId;   // Netware 386 only
   WORD  wConnectionId;  // Used in getting Owner and Modifier
   WORD  wCreateDate;
   WORD  wCreateTime;    // Netware 386 only
   WORD  wArchiveDate;
   WORD  wArchiveTime;
   WORD  wAccessedDate;
   DWORD dwAttrib;
}
FSVIO_NETSTRUCT, FAR* LPFSVIO_NETSTRUCT;
typedef const FSVIO_NETSTRUCT FAR* LPCFSVIO_NETSTRUCT;

#if 0
// Structure that contains low level Undelete info about the
// deleted file or directory.    This structure is defined in WNFSV.H
// because the structure FSVIO_DIRENTRYSTRUCT requires it and I (JJS) did
// not want WNFSV.H to have to include the undelete interface file.
typedef struct _FSVIO_UND_INFO
{
   WORD  wFirstCluster; // First cluster for the data in the file.
   WORD  wSelectionNum; // ????.
   WORD  wDelTime;      // Time the file was deleted.
   WORD  wDelDate;      // Date the file was deleted.
   WORD  wDelMethod;    // Method indicating how the file was deleted
                        //  (DOS, sentry, etc.).
   WORD  wRcvrFctr;     // Type used to indicate the "recoverability" of
                        //  the data that pertains to the file.
                        //   of the "clusters" that composed the file.
   DWORD dwDelSector;   // The relative sector that contains the low level
                        //  DOS directory entry for the deleted file.  For
                        //   the "network" method this field contains the
                        //    netware ID for the file/directory in question.
   WORD  wDelOffset;    // The offset within the aforementioned sector of
                        //  the DOS directory entry.
   WORD  wRecordNum;    // This field only applies to the "sentry" and
                        //  "tracker" methods.  It contains the index
                        //   of the affiliate sentry/tracker record.
}
FSVIO_UND_INFO, FAR* LPFSVIO_UND_INFO;
typedef const FSVIO_UND_INFO FAR* LPCFSVIO_UND_INFO;

// Definitions used to work with a list of pointers to low level
// undelete information records.
typedef LPFSVIO_UND_INFO  FAR* LPFSVIO_UNDRECLST;
typedef LPFSVIO_UNDRECLST FAR* LPLPFSVIO_UNDRECLST;
#endif

typedef struct _FSVIO_DIRENTRYSTRUCT
{
   BYTE              szName[FSV_DIRENTRYBUF_LEN];           // 8.3 filename
   BYTE              bLevel;                                // read-only
   DWORD             dwFsvAttrib;                           // FSV_ATTRIB_??? Flags (File Services specific)
   WORD              wTime;                                 // Last modified time (DOS format)
   WORD              wDate;                                 // Last modified date (DOS format)
   DWORD             dwSize;                                // File size, read-only
   DWORD             dwTag;                                 // for FM file tagging.
   LPFSVIO_NETSTRUCT lpFsvNet;                              // Novell NetWare struct.
   LPNETRESOURCE     lpNetInfo;                             // Network info
   LPVOID            lpUndInfo;        // non-NULL if item is deleted item.
   LONG              lDelLoc;          // If lpUndInfo non-NULL, this tells where the deleted item is located. (DelTech)
   LPSTR             lpszLongDesc;     // Long description (Windows 95)
   DWORD             dwDosAttrib;      // File attributes (Windows 95)
   DWORD             dwSizeHigh;       // File size High DWORD (Windows 95)
   FSV_FILETIME      ftCreationTime;   // Creation time (Windows 95)
   FSV_FILETIME      ftLastAccessTime; // Last access time (Windows 95)
   FSV_FILETIME      ftLastWriteTime;  // Last modified time (Windows 95)
}
FSVIO_DIRENTRYSTRUCT, FAR* LPFSVIO_DIRENTRYSTRUCT, FAR* FAR* LPLPFSVIO_DIRENTRYSTRUCT;
typedef const FSVIO_DIRENTRYSTRUCT FAR* LPCFSVIO_DIRENTRYSTRUCT;

// Definitions used to work with a list of pointers to File Services
// directory entry records.
typedef LPFSVIO_DIRENTRYSTRUCT FAR* LPFSVIO_DIRENTRYLST;
typedef LPFSVIO_DIRENTRYLST FAR*    LPLPFSVIO_DIRENTRYLST;

// This slimly little structure is used when sending the viewer message
// VM_SETDELFILE.  Since this type is really used by the deleted file
// viewer access method and any application that needs to send that message,
// it turns out this is a reasonable home for the little bugger.
typedef struct _FSV_VCDATA
{
    char                   szName[ FSV_PATHBUF_LEN ];
    LPFSVIO_DIRENTRYSTRUCT lpdes;
} FSV_VCDATA, FAR* LPFSV_VCDATA, FAR* FAR* LPLPUDAM_VCDATA;
typedef const FSV_VCDATA FAR* LPCFSV_VCDATA;

typedef struct tagFSV_DIRENTRYINFO {
   struct  tagFSV_DIRENTRYINFO far * lpdeiParent;
   struct  tagFSV_DIRENTRYINFO far * lpdeiChild;
   struct  tagFSV_DIRENTRYINFO far * lpdeiNext;
   DWORD       dwUserInfo;
   struct  _FSVIO_DIRENTRYSTRUCT fi;
   BYTE     bLevel;
   BYTE        flags;
} FSV_DIRENTRYINFO, FAR* LPFSV_DIRENTRYINFO;

typedef struct tagFSV_DEITREE {
   struct   tagFSV_DEITREE far * lpnext;
   LPFSV_DIRENTRYINFO lpdeiFreeList;
   WORD (WINAPI far * ExpandDirEnt)(struct tagFSV_DEITREE far *, LPFSV_DIRENTRYINFO, WORD );
   WORD (WINAPI far * ExpandAllEnt)(struct tagFSV_DEITREE far *, LPFSV_DIRENTRYINFO, WORD );
   BOOL (WINAPI far * DirectoryHasSubdir)(struct tagFSV_DEITREE far *, LPFSV_DIRENTRYINFO, WORD );
   DWORD dwBlockSize;
   LPVOID   hUE;
   FSV_DIRENTRYINFO deiRoot;
} FSVDEITREE, FAR* LPFSV_DEITREE;

/* The following calls are direct C wraps of some dos interfaces in all their
   their naked ugliness. Structures are censored somewhat because C
   compilers are apt to word align structures*/

typedef struct tagDEVICEPARAMS{        /*dp*/         
   BYTE  bSpecFunc;
   BYTE  bDevType;               
   WORD  wDevAttr;               
   WORD  wCylinders;
   BYTE  bMediaType;
   /* Start of BPB at offset 7   */
   WORD  wBytesPerSec;
   BYTE  bSectorsPerCluster;
   WORD  wResSectors;
   BYTE  bFats;
   WORD  wRootDirEnt;
   WORD  wSectors;
   BYTE  bMedia;
   WORD  wFATsecs;               
   WORD  wSecPerTrack;  /* Sectors per track */    
   WORD  wHeads;        /* Tracks per cylinder */
   DWORD dwHiddenSecs;              
   DWORD dwHugeSects;
   BYTE  bDriveNumber;  /* Garbage value */
   BYTE  bPad1;
   BYTE  bBootSignature;
   BYTE  dwVolumeId[4];
   }  DEVICEPARAMS;              
                        
typedef struct tagRWBLOCK{       /*rw*/         
   BYTE  bPadding;               
   BYTE  bSpecFunc;  /* MUST be zero    */      
   WORD  wHead;      /* usually 0 or 1  */      
   WORD  wCylinder;  /* usually 0 to 79 */      
   WORD  wFirstSector;  /* Usually zero      */    
   WORD  wSectors;     /* Usually same as wSecPerTrack*/
   char far * abBuffer;
   }  RWBLOCK;

typedef struct tagFSVIO_FORMATSTRUCT
{   /*Format*/
   DWORD          dwDiskCapacity;
   WORD           uCylinders;
   WORD           uHeads;
   WORD           uSecPerTrack;
   WORD           uBytesPerSec;
   BYTE           bDriveNumber;
   __int16        fIs12BitFat;
   WORD           wMaxFATClstr;     // The value of the maximum index into the FAT.
   WORD           wUsableClusters;  // The number of usable clusters (excluding
                                    //  those used by DOS) on the disk.
   WORD           wClusterBytes;    // Number of bytes in a cluster.
   WORD           wRootDirSectors;  // Number of sectors in the root directory.
   DEVICEPARAMS   dp;
}
FSVIO_FORMATSTRUCT, far * LPFSVIO_FORMATSTRUCT;
typedef const FSVIO_FORMATSTRUCT FAR* LPCFSVIO_FORMATSTRUCT;

typedef struct tagFSVIO_FORMATPROG{  /*frmprog*/
   FSVIO_FORMATSTRUCT DiskFormat;
   FSVIO_FORMATSTRUCT DriveFormat;
   LPSTR              lpFat;
}  FSVIO_FORMATPROG, far * LPFSVIO_FORMATPROG;

typedef struct tagFSVEVNT_STRUCT {
   LPSTR lpszName1;
   LPSTR lpszName2;
   WORD  wAction;
   WORD  wObject;
   WORD  wDomain;
   DWORD dwEventId;
   DWORD dwUser;
} FSVEVNT_STRUCT, FAR * LPFSVEVNT_STRUCT;

typedef struct tagFSVCRITERROR_STRUCT {
   FSVFUNC_ID  FunctionID;         // FSVFUNC_???
   long        lError;              // FSVERR_???
   LPCSTR      lpstr1;
   LPCSTR      lpstr2;
   DWORD       dwParam;
} FSVCRITERROR_STRUCT, FAR * LPFSVCRITERROR_STRUCT;

typedef struct tagFSVPROGRESS_STRUCT {
   FSVFUNC_ID  FunctionID;         // FSVFUNC_???
   WORD        wPercent;
   LPSTR       lpstr1;
   LPSTR       lpstr2;
   DWORD       dwParam;
} FSVPROGRESS_STRUCT, FAR * LPFSVPROGRESS_STRUCT;

// The following is used so that warnings don't appear on Level 4
typedef struct tm   TM;

//===========================================================================
//
//                              PROTOTYPES
//
//===========================================================================

// Dir Entry Enumeration Callback Prototype
typedef WORD (WINAPI * LPFSV_ENUMDIRENTRYCALLBACK)(LPFSVIO_DIRENTRYSTRUCT, LPSTR, DWORD);

// DEI Enumeration Callback Prototype
typedef WORD (WINAPI * LPFSVDEI_CALLBACK)( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, DWORD dwUser );

// Event Callback prototype
typedef LONG (CALLBACK * LPFSVEVNT_CALLBACK)( LPFSVEVNT_STRUCT lpFsvEvnt );

// Process completion call back
typedef enum { FS_CONTINUE, FS_ABORT } PROCESS_CALLBACK_VALUE;
typedef PROCESS_CALLBACK_VALUE (WINAPI * PROCESS_CALLBACK)(FSVPROGRESS_STRUCT far * lpProgress);

// Critical Error Handler callback
typedef enum {  FSVCRIT_ABORT, FSVCRIT_RETRY, FSVCRIT_IGNORE } ERROR_CALLBACK_VALUE;
typedef ERROR_CALLBACK_VALUE (WINAPI * ERROR_CALLBACK)(FSVCRITERROR_STRUCT far * lpCritError);

//===========================================================================
//
// Fsv_Register( hInst, dwVersion, wDataType ) : HINSTANCE;
//
//   hInst              Application's instance handle
//   dwVersion          Expected version number of DLL.
//   wDataType          Specifies what type of custom data should be
//                      attached to File Service Objects.
//
// This function will add the application to File Services internal App
// registration list. If the version number is not compatible, then the
// function fails.
//
// Returns: HINSTANCE of WNFSV if the function is successful, 0 if it is not.
//
// Typical usage
//
// if( !Fsv_Register(hInst, FSV_EXPECTED_VERSION, 0) )
//
HINSTANCE WINAPI Fsv_Register(HINSTANCE  hInst, DWORD dwVersion, WORD wDataType);
typedef HINSTANCE (WINAPI * LPFNFSV_REGISTER)(HINSTANCE,DWORD,WORD);

//===========================================================================
//
// Fsv_Unregister( hInst ) : BOOL;
//
//   hInst              Application's instance handle
//
// This function will remove the application from File Services registration
// list. Unregistering will remove all notification callbacks and will destroy
// all instance specific data associated with File Services.
//
// Returns: TRUE if the function is successful, FALSE if it is not.
//
BOOL WINAPI Fsv_Unregister(HINSTANCE hInst);
typedef BOOL (WINAPI * LPFNFSV_UNREGISTER)(HINSTANCE);

//===========================================================================
//
// Fsv_GetRegisteredCount() : LONG;
//
// This function will return the number of registered instances.
//

LONG WINAPI Fsv_GetRegisteredCount( void );
typedef LONG (WINAPI * LPFNFSV_GETREGISTEREDCOUNT)( void );

//===========================================================================
//
// Fsv_SetWndInst( hWnd ) : LONG;
//
// This function registers the window handle and instance handle of the
// FSV background task.
//
// Returns: TRUE if successful, FALSE otherwise.
//

BOOL WINAPI Fsv_SetWndInst( HWND hWnd );
typedef BOOL (WINAPI * LPFNFSV_SETWNDINST)( HWND );

//============================================================================
// FsvEvnt_AttachBkTask: void
//
// This function is called by the background task (and should only be called
// by it) to tell a particular instance of File Services that it should call
// the task back for all valid file system events generated by SymEvent.  To
// detach the task from File Services, this function is called with a NULL
// window handle.
//
void WINAPI FsvEvnt_AttachBkTask(
   HWND  hwndTask
);
typedef void (WINAPI * LPFNFSVEVNT_ATTACHBKTASK)( HWND );

/**********/

//============================================================================
// FsvEvnt_RegisterForEvent( lpszName, wObject, wDomain, hWnd, msg, 
//                           dwUser ) : DWORD;
//
// lpszName       Name of event source (may be NULL to specify ALL sources)
// wObject        Type of objects event notifications are desired
// wDomain        Domain, based on lpszName, event notifications are desired
// hWnd           Window to receive notification (must not be NULL if 
//                lpfnFsvEvnt is NULL).
// msg            Message to send to window (must be a valid USER message
//                value if hWnd is not NULL).
// dwUser         A long value passed into the callback (may be NULL).
//
// Return Value: The return value is a unique event registration ID value
//               which must be passed in to the FsvEvnt_UnregisterEvent
//               function when removing a registration.  The return value is
//               0 if registration fails.
//
DWORD WINAPI FsvEvnt_RegisterForEvent( LPSTR lpszName, WORD wObject, WORD wDomain, HWND hWnd, UINT msg, DWORD dwUser );
typedef DWORD (WINAPI * LPFNFSVEVNT_REGISTERFOREVENT)(LPSTR,WORD,WORD,HWND,UINT,DWORD);

//============================================================================
// FsvEvnt_UnregisterEvent( dwEventId ) : BOOL;
//
//    dwEventId      ID of event registration to remove
//
// Return Value:  Is TRUE if registration successfully removed.  The return
//                value is FALSE if the event ID is invalid or some other
//                error occurs and the registration is not removed.
//
BOOL  WINAPI FsvEvnt_UnregisterEvent( DWORD dwEventId );
typedef BOOL (WINAPI * LPFNFSVEVNT_UNREGISTEREVENT)(DWORD);

//============================================================================
// FsvEvnt_TriggerAnEvent( lpszName1, lpszName2, wAction, wObject, lpv ) : BOOL;
//
// lpszName1         Specifies name of source object to trigger event on (may be NULL)
// lpszName2         Specifies a new name for the source (may be NULL)
// wAction           Specifies the event action
// wObject           Specifies the object type
// lpv               Used internally -- must be NULL.
//
// Return value:  Is TRUE if event successfuly triggered.  The return value
//                is FALSE if any of the parameters are invalid, an error
//                occurs and the event cannot be triggered, or events are
//                disabled.
//
BOOL WINAPI FsvEvnt_TriggerAnEvent(LPCSTR lpszName1, LPCSTR lpszName2, WORD wAction, WORD wObject, LPVOID lpv );
typedef BOOL (WINAPI * LPFNFSVEVNT_TRIGGERANEVENT)(LPCSTR,LPCSTR,WORD,WORD,LPVOID);

//============================================================================
// FsvEvnt_EnableRegistration( dwEventId, fEnbale ) : int;
//
// This function enables or disables event notifications going to the
// specified event registration.
//
//    dwEventId      Event ID (returned by FsvEvnt_RegisterForEvent)
//    fEnbale        TRUE = enable, FALSE = disable
//
// Return value:  Is the current count of disables.  If this
//                value is < 0, then an error has occurred (mismatch of
//                enable/disable).
//
int   WINAPI FsvEvnt_EnableRegistration( DWORD dwEventId, BOOL fEnable );
typedef int (WINAPI * LPFNFSVEVNT_ENABLEREGISTRATION)(DWORD,BOOL);

//============================================================================
// FsvEvnt_EnableEvents( fEnable ) : BOOL;
//
// This function enables or disables event notifications for all 
// registrations.
//
//    fEnable        TRUE = enable; FALSE = disable
//
// Return value:  Is the current count of disables.  If this
//                value is < 0, then an error has occurred (mismatch of
//                enable/disable).
//
int   WINAPI FsvEvnt_EnableEvents( BOOL fEnable );
typedef int (WINAPI * LPFNFSVEVNT_ENABLEEVENTS)(BOOL);

//============================================================================
// FsvEvnt_EnableFileCDR( fEnable ) : BOOL;
//
// This function enables or disables File CDR notifications for all
// registrations.
//
//    fEnable        TRUE = enable; FALSE = disable
//
// Return value:  Is the current count of disables.  If this
//                value is < 0, then an error has occurred (mismatch of
//                enable/disable).
//
int   WINAPI FsvEvnt_EnableFileCDR( BOOL fEnable );
typedef int (WINAPI * LPFNFSVEVNT_ENABLEFILECDR)(BOOL);

//===========================================================================
// FsvEvnt_DoDriveEventCheck( void ): void;
//
// This function will check to see if any drives have been added, removed, or
// modified since the last time it was called. If they have been changed, then
// the proper events will be triggered.
//
void  WINAPI FsvEvnt_DoDriveEventCheck(void);
typedef void (WINAPI * LPFNFSVEVNT_DODRIVEEVENTCHECK)(void);

//============================================================================
// FsvEvnt_ClearQueue( dwEventId ) : BOOL;
//
//    dwEventId      ID of event registration from which to clear queue
//
// Return Value:  Is TRUE if queue removed. Is FALSE if ID invalid.
//
BOOL  WINAPI FsvEvnt_ClearQueue( DWORD dwEventId );
typedef BOOL (WINAPI * LPFNFSVEVNT_CLEARQUEUE)(DWORD);

//============================================================================
// FsvEvnt_GetQueue( dwEventId ) : BOOL;
//
//    dwEventId      ID of event registration whose queue to return
//
// Return Value:  Is TRUE if notifications sent.  FALSE if no notifications
//                or an error occurs.
//
BOOL  WINAPI FsvEvnt_GetQueue( DWORD dwEventId );
typedef BOOL (WINAPI * LPFNFSVEVNT_GETQUEUE)(DWORD);

/**********/

/*============================================================================
 *
 * WINAPI FsvDei_CreateDirTreeFromPath( LPCSTR lpPath ): LPFSV_DEITREE
 *
 *  lpPath     Path to create the tree for.
 *
 * Create the directory tree.
 *
 */
LPFSV_DEITREE WINAPI FsvDei_CreateDirTreeFromPath( LPCSTR lpPath );
typedef LPFSV_DEITREE (WINAPI * LPFNFSVDEI_CREATEDIRTREEFROMPATH)(LPSTR);

/*============================================================================
 *
 * WINAPI FsvDei_CreateDirTree( DWORD dw ): LPFSV_DEITREE
 *
 *  dw   bit mask of drive letters to be masked out, bit 0 invalid
 *       bit 1 is drive A, bit 2 is drive B, etc.
 *
 *       If a bit is set, the relevant drive is not shown,
 *       if a bit is clear, the relevant drive is shown. 
 *       This back to front approach is used so that a
 *       default of zero will yield sensible results.
 *
 * Create the directory tree.
 *
 */
LPFSV_DEITREE WINAPI FsvDei_CreateDirTree( DWORD dw );
typedef LPFSV_DEITREE (WINAPI * LPFNFSVDEI_CREATEDIRTREE)(DWORD);

/*============================================================================
 *
 * WINAPI FsvDei_CreateDirTreeFromPathNoCache( LPCSTR lpPath ): LPFSV_DEITREE
 *
 *  lpPath     Path to create the tree for.
 *
 * Create the directory tree direct from physical media.
 *
 */
LPFSV_DEITREE WINAPI FsvDei_CreateDirTreeNoCache( LPCSTR lpPath );
typedef LPFSV_DEITREE (WINAPI * LPFNFSVDEI_CREATEDIRTREENOCACHE)(LPSTR);

/*============================================================================
 *
 * WINAPI FsvDei_DestroyDirTree( LPFSV_DEITREE lptdei ) : void
 *
 *  lptdei     Directory tree to be destroyed.
 *
 */
VOID WINAPI FsvDei_DestroyDirTree( LPFSV_DEITREE lptdei );
typedef VOID (WINAPI * LPFNFSVDEI_DESTROYDIRTREE)(LPFSV_DEITREE);

/*============================================================================
 *
 * WINAPI FsvDei_PathOfEntry( lptdei, lpdei, lpBuffer, fLFN, fUNC ): void
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpdei    Directory entry for which the pathname is to be given
 *    lpBuffer Buffer in which pathmame will be stored.
 *    fLFN     TRUE if path to be returned should use LFN
 *    fUNC     FALSE if OK to return a non-UNC name ("MyComputer\SYMUSA-BV-DEV-CPT_DEV\...")
 *
 */
VOID WINAPI FsvDei_PathOfEntry( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, LPSTR lpBuffer, BOOL fLFN, BOOL fUNC );
typedef VOID (WINAPI * LPFNFSVDEI_PATHOFENTRY)(LPFSV_DEITREE,LPFSV_DIRENTRYINFO,LPSTR,BOOL,BOOL);

/*============================================================================
 *
 * WINAPI FsvDei_CollapseDir( lptdei, lpdei ) : void
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpdei    Directory entry to be collapsed.
 *
 * The contents of the directory are thrown away (from our cache, not from
 * the disk)
 */
VOID WINAPI FsvDei_CollapseDir( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei );
typedef VOID (WINAPI * LPFNFSVDEI_COLLAPSEDIR)(LPFSV_DEITREE,LPFSV_DIRENTRYINFO);

/*============================================================================
 *
 * WINAPI FsvDei_DoSubdirsExist( lptdei, lpdei, wEnumOptions ) : BOOL
 *
 *    lptdei.  Directory tree
 *    lpdei    Directory
 *    wEnumOptions
 *
 * Returns true if a subdirectory of lpdei, exists, false otherwise
 *
 */
BOOL WINAPI FsvDei_DoSubdirsExist( LPFSV_DEITREE, LPFSV_DIRENTRYINFO, WORD );
typedef BOOL (WINAPI * LPFNFSVDEI_DOSUBDIRSEXIST)(LPFSV_DEITREE,LPFSV_DIRENTRYINFO,WORD);

/*============================================================================
 *
 * WINAPI FsvDei_EntryOfPath( lptdei, lpPath ) : LPFSV_DIRENTRYINFO
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpPath   Path of entry to be found.
 *
 * Returns the entry corresponding to that path. Expands as many directories
 * as needed to get there.
 */
LPFSV_DIRENTRYINFO WINAPI FsvDei_EntryOfPath( LPFSV_DEITREE, LPCTSTR, WORD );
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_ENTRYOFPATH)(LPFSV_DEITREE, LPCTSTR, WORD );

/*============================================================================
 *
 * WINAPI FsvDei_EntryOfPathNet( lptdei, lpPath, wFlags, lpNetResource ) : LPFSV_DIRENTRYINFO
 *
 *    lptdei         Tree to which lpdei belongs.
 *    lpPath         Path of entry to be found.
 *    wFlags
 *    lpNetResource  Pointer to NETRESOURCE structure to match.
 *
 * Returns the entry corresponding to that path and NETRESOURCE. Expands as many
 * directories as needed to get there.
 */
LPFSV_DIRENTRYINFO WINAPI FsvDei_EntryOfPathNet( LPFSV_DEITREE, LPCTSTR, WORD, LPNETRESOURCE );
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_ENTRYOFPATHNET)(LPFSV_DEITREE, LPCTSTR, WORD, LPNETRESOURCE );

/*============================================================================
 *
 * WINAPI FsvDei_Compare( lpdei1, lpdei2 ) : int
 *
 *    lpdei1.
 *    lpdei2
 *
 * Compares lpdei1 and lpdei2. Returns lexical order of their paths.
 * as needed to get there.
 */
int WINAPI FsvDei_Compare( LPFSV_DIRENTRYINFO lpdei1, LPFSV_DIRENTRYINFO lpdei2);
typedef int (WINAPI * LPFNFSVDEI_COMPARE)(LPFSV_DIRENTRYINFO,LPFSV_DIRENTRYINFO);

/*============================================================================
 *
 * WINAPI FsvDei_FirstSubdir( ): LPFSV_DIRENTRYINFO
 *
 *    lptdei.  Directory tree
 *    lpdei    Directory
 *    wEnumOptions
 *
 * Returns the first subdirectory of lpdei, or null if there are no
 * subdirectories. Errors are returned as no subdirectories. Higher
 * levels of the code see a hardware problem as a suddenly emptied
 * directory.
 *
 */
LPFSV_DIRENTRYINFO WINAPI FsvDei_FirstSubdir( LPFSV_DEITREE, LPFSV_DIRENTRYINFO, WORD );
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_FIRSTSUBDIR)(LPFSV_DEITREE, LPFSV_DIRENTRYINFO, WORD );

/*============================================================================
 *
 * WINAPI FsvDei_NextDir( lptdei, lpdei ): LPFSV_DIRENTRYINFO
 *
 *    lptdei.  Directory tree
 *    lpdei    Directory
 *
 * Returns the subdirectory following lpdei, or null if lpdei is the last
 * subdirectory in this directory. Can never have an error, but can get out
 * of sync with the disk and not know.
 */
LPFSV_DIRENTRYINFO WINAPI FsvDei_NextDir( LPFSV_DEITREE, LPFSV_DIRENTRYINFO );
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_NEXTDIR)( LPFSV_DEITREE, LPFSV_DIRENTRYINFO );

/*============================================================================
 *
 * WINAPI FsvDei_NextDirOfTree( lptdei, lpdei ): LPFSV_DIRENTRYINFO
 *
 *    lptdei.  Directory tree
 *    lpdei    Directory
 *
 * Returns the subdirectory following lpdei in tree order, usually a
 * subdirectory of lpdei, but could be the following directory on the same
 * level, or a following directory on a higher level. Returns a null if we
 * reach the end of the tree, we have enumerated all the directories.
 *
 * If an I/O error occurs when expanding a directory, that directory will
 * appear to be empty.
 */
LPFSV_DIRENTRYINFO WINAPI FsvDei_NextDirOfTree( LPFSV_DEITREE, LPFSV_DIRENTRYINFO, WORD );
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_NEXTDIROFTREE)( LPFSV_DEITREE, LPFSV_DIRENTRYINFO, WORD );

/*============================================================================
 *
 * WINAPI FsvDei_NextEntryOfTree( lptdei, lpdei ): LPFSV_DIRENTRYINFO
 *
 *    lptdei.  Directory tree
 *    lpdei    Directory
 *
 * Returns the file or subdirectory following lpdei in tree order, usually a
 * file of lpdei, but could be the following directory on the same
 * level, or a following directory on a higher level. Returns a null if we
 * reach the end of the tree, we have enumerated all the directories.
 *
 * If an I/O error occurs when expanding a directory, that directory will
 * appear to be empty.
 */
LPFSV_DIRENTRYINFO WINAPI FsvDei_NextEntryOfTree( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, WORD wEnumOptions );
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_NEXTENTRYOFTREE)(LPFSV_DEITREE, LPFSV_DIRENTRYINFO, WORD );

/*============================================================================
 *
 * WINAPI FsvDei_NextEntry( lptdei, lpdei ): LPFSV_DIRENTRYINFO
 *
 *    lptdei.  Directory tree
 *    lpdei    Directory
 *
 * Returns the subdirectory following lpdei, or null if lpdei is the last
 * element in this directory. Can never have an error, but can get out
 * of sync with the disk and not know.
 */
LPFSV_DIRENTRYINFO WINAPI FsvDei_NextEntry(LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei );
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_NEXTENTRY)(LPFSV_DEITREE, LPFSV_DIRENTRYINFO );

/*============================================================================
 *
 * WINAPI FsvDei_EraseDirEntry( lptdei, lpdei ) : void
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpdei    entry to be cleared
 *
 * This entry, and its descendents are added to the free list.
 */
void WINAPI FsvDei_EraseDirEntry( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei );
typedef void (WINAPI * LPFNFSVDEI_ERASEDIRENTRY)(LPFSV_DEITREE,LPFSV_DIRENTRYINFO);

/*============================================================================
 *
 * WINAPI FsvDei_ReAllocLongDesc( lpdei, lpszNew ) : LONG
 *
 *    lpdei    dir entry whose long description needs to be reallocated
 *    lpszNew  New long description
 *
 */
LONG WINAPI FsvDei_ReAllocLongDesc( LPFSV_DIRENTRYINFO, LPCSTR );
typedef LONG (WINAPI * LPFSVDEI_REALLOCLONGDESC)(LPFSV_DIRENTRYINFO, LPCSTR );


/*============================================================================
 *
 * WINAPI FsvDei_EventDeletePath( ) : LPFSV_DIRENTRYINFO
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpPath   Path of entry to be found.
 * 
 * Returns the entry corresponding to that path if it is part of the cache,
 * otherwise returns null. Delete it from any existing data structures.
 *
 * This in intended to be used in response to notification events. If you
 * are displaying a directory tree, and receive an event notification,
 * check the event to see if it is a directory event (look for the flag
 * bit FSV_EVENT_DIR). If is not, ignore the message. If it is a delete
 * message, call FsvDei_EventDeletePath. If FsvDei_EventDeletePath
 * returns null, you can forget this message. If it returns a non null
 * value you may have to delete an entry from your display.
 *
 * After you have done what processing is needed, delete the entry from
 * the cache with FsvDei_EraseDirEntry.
 * 
 * Referencing an item after erasing it will most likely cause a UAE.
 * 
 */
LPFSV_DIRENTRYINFO  WINAPI FsvDei_EventDeletePath( LPFSV_DEITREE lptdei, LPCSTR lpPath);
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_EVENTDELETEPATH)(LPFSV_DEITREE,LPCSTR);

/*============================================================================
 *
 * WINAPI FsvDei_EventAddPath(lptdei, lpPath, fZip ) : LPFSV_DIRENTRYINFO
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpPath   Path of entry to be added.
 *    fZip     TRUE signifies a Zip-type file, FALSE signifies normal dir.
 * 
 * If the parent directory to which the new directory has been added is part
 * of the cache, and its offspring are also part of the cache, this adds this
 * new directory to the cache and returns a pointer to the newly added
 * directory. Otherwise it returns a null.
 *
 * This in intended to be used in response to notification events. If you
 * are displaying a directory tree, and receive an event notification,
 * check the event to see if it is a directory event (look for the flag
 * bit FSV_EVENT_DIR). If is not, ignore the message. If it is a create
 * message(FSV_EVENT_CREATED), call FsvDei_EventAddPath. If
 * FsvDei_EventAddPath returns null, you can forget this message. If it
 * returns a non null value you may have to delete an entry from your
 * display.
 */
LPFSV_DIRENTRYINFO  WINAPI FsvDei_EventAddPath( LPFSV_DEITREE lptdei, LPSTR lpPath, BOOL fZip);
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_EVENTADDPATH)(LPFSV_DEITREE, LPSTR, BOOL);

/*============================================================================
 *
 * WINAPI FsvDei_EventAddDrive( lptdei, lpPath ) : LPFSV_DIRENTRYINFO
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpPath   Path of entry to be added.
 * 
 * This function adds the specified drive to the tree structure.  The drive
 * should be in the format "C:"
 *
 */
LPFSV_DIRENTRYINFO WINAPI FsvDei_EventAddDrive( LPFSV_DEITREE lptdei, LPCSTR lpPath );
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_EVENTADDDRIVE)(LPFSV_DEITREE,LPCSTR );


/*============================================================================
 *
 * WINAPI FsvDei_EventAddFTPSite(lptdei, lpPath  ) : LPFSV_DIRENTRYINFO
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpPath   Path of FTPSite to be added e.g. ///NewSite
 * 
 * This in intended to be used in response to notification events. If you
 * are displaying a directory tree, and receive an event notification,
 * check the event to see if it is a directory event (look for the flag
 * bit FSV_EVENT_DIR). If is not, ignore the message. If it is a create
 * message(FSV_EVENT_CREATED)and the objecttype is FSV_EVENT_FTPSITE,
 * call FsvDei_EventAddFTPSite. This should ass the site to Site list.
 */
LPFSV_DIRENTRYINFO  WINAPI FsvDei_EventAddFTPSite( LPFSV_DEITREE lptdei, LPSTR lpPath );
typedef LPFSV_DIRENTRYINFO (WINAPI * LPFNFSVDEI_EVENTADDFTPSITE)(LPFSV_DEITREE, LPSTR);


/*============================================================================
 *
 * WINAPI FsvDei_DirectoryCount( lptdei, lpdei, lpCallBack, dwUser ): long
 *
 *    lptdei.     Directory tree
 *    lpdei       Directory
 *    lpCallBack  Callback procedure for selective inclusion
 *    dwUser      User parameter for callback function;
 *
 * This gives a count of the sub directories of lpdei. If lpdei is a machine,
 * it gives a count of how many drives. If lpdei is a drive, it gives a
 * count of how many top level directories in that drive, if lpdei is a
 * directory, it gives a count of how many sub directories that directory has,
 * but it never goes all the way down the tree enumerating sub sub
 * directories of sub directories.
 */
LONG WINAPI FsvDei_DirectoryCount( LPFSV_DEITREE, LPFSV_DIRENTRYINFO, WORD, LPFSVDEI_CALLBACK, DWORD);
typedef LONG (WINAPI * LPFNFSVDEI_DIRECTORYCOUNT)( LPFSV_DEITREE, LPFSV_DIRENTRYINFO, WORD, LPFSVDEI_CALLBACK, DWORD);

/*======================================================================
 *
 * WINAPI FsvDei_ListDirectories( lptdei, lpdei, lpBuffer ): Void
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpdei entry for which the number of sub directories was counted.
 *    lpBuffer Place in which the directories will be put
 *
 * This fills in the buffer with the directories counted in
 * FsvDei_DirectoryCount, or rather with far pointers to data structures
 * describing those directories.
 *
 * It should be called immediately after FsvDei_DirectoryCount. It is
 * guaranteed to always return the number of directories promised by
 * FsvDei_DirectoryCount, regardless of whether that number is true or false.
 * This call cannot fail. If however you call some other FsvDei functions
 * after FsvDei_DirectoryCount and before FsvDei_ListDirectories, then
 * other outcomes are possible.
 *
 */
VOID WINAPI FsvDei_ListDirectories( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, LPFSV_DIRENTRYINFO far * lpBuffer);
typedef VOID (WINAPI * LPFNFSVDEI_LISTDIRECTORIES)( LPFSV_DEITREE , LPFSV_DIRENTRYINFO, LPFSV_DIRENTRYINFO far *);

/*============================================================================
 *
 * WINAPI FsvDei_FileAndDirCount( lptdei, lpdei lpCallBack, dwUser): long
 *
 *    lptdei.     Directory tree
 *    lpdei       Directory
 *    lpCallBack  Callback procedure for selective inclusion
 *    dwUser      Arbitrary long which will be passed to the call back
 *                   function
 *
 * This gives a count of the files and directories of lpdei. If lpdei is a
 * machine, it gives a count of how many drives. If lpdei is a drive, it
 * gives a count of how many top level files and directories in that drive, and
 * similarly for directories. It never goes all the way down the tree
 * enumerating sub sub directories of sub directories.
 *
 * This never returns an error. If something goes wrong, the count will be low,
 * usually zero. The user may be informed, but you will not.
 *
 * The CallBack function will be called once for each file and directory found.
 * If the call back function returns a zero, FALSE, then that file or directory
 * will not be included in the count. Furthermore, once ignored, the file or
 * directory is likely to remain invisible on subsequent counts, even with a
 * different call back function, unless you first collapse lpdei with a call to
 * FsvDei_CollapseDir.
 */
LONG WINAPI FsvDei_FileAndDirCount( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, WORD wEnumOptions, LPFSVDEI_CALLBACK lpCallBack, DWORD dwUser);
typedef LONG (WINAPI * LPFNFSVDEI_FILEANDDIRCOUNT)( LPFSV_DEITREE , LPFSV_DIRENTRYINFO, WORD, LPFSVDEI_CALLBACK, DWORD);

/*======================================================================
 *
 * WINAPI FsvDei_ListFileAndDir( lptdei, lpdei, lpBuffer ): Void
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpdei entry for which the number of sub directories was counted.
 *    lpBuffer Place in which the directories will be put
 *
 * This fills in the buffer with the files and directories counted in the
 * previous FsvDei_FileAndDirCount, or rather with far pointers to data
 * structures describing those directories. The usual rules about calling it
 * immediately afterwards apply.
 *
 */
VOID WINAPI FsvDei_ListFileAndDir( LPFSV_DEITREE  lptdei, LPFSV_DIRENTRYINFO lpdei, LPFSV_DIRENTRYINFO far * lpBuffer);
typedef VOID (WINAPI * LPFNFSVDEI_LISTFILEANDDIR)( LPFSV_DEITREE , LPFSV_DIRENTRYINFO , LPFSV_DIRENTRYINFO far * );

/*============================================================================
 *
 * WINAPI FsvDei_SubDirectoryCount( lptdei, lpdei lpCallBack, dwUser): Long
 *
 *    lptdei.     Directory tree
 *    lpdei       Directory
 *    lpCallBack  Callback procedure for selective inclusion
 *    dwUser      User parameter for callback function;
 *
 * This gives a count of the sub directories and sub sub directories of lpdei,
 * all the way down the tree. If lpdei is a machine, the drives are treated like
 * directories. This never returns an error. If something goes wrong, the count
 * will be low, usually zero. The user may be informed, but you will not.
 *
 * The CallBack function will be called once for each directory found. If the
 * call back function returns a zero, FALSE, then that directory and all its sub
 * directories will not be included in the count. Furthermore, once ignored, the
 * directory is likely to remain invisible on subsequent counts, even with a
 * different call back function, unless you first collapse lpdei with a call to
 * FsvDei_CollapseDir.
 *
 * If the callback function is null, this is the same as a callback function
 * that always returns true (but executes faster).
 *
 */
LONG WINAPI FsvDei_SubDirectoryCount( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, WORD wEnumOptions, LPFSVDEI_CALLBACK lpCallBack, DWORD dwUser);
typedef LONG (WINAPI * LPFNFSVDEI_SUBDIRECTORYCOUNT)( LPFSV_DEITREE , LPFSV_DIRENTRYINFO , WORD , LPFSVDEI_CALLBACK , DWORD );

/*======================================================================
 *
 * WINAPI FsvDei_ListSubDirectories( lptdei, lpdei, lpBuffer ): Void
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpdei entry for which the number of sub directories was counted.
 *    lpBuffer Place in which the directories will be put
 *
 * This fills in the buffer with the directories counted in
 * FsvDei_SubDirectoryCount, or rather with far pointers to data structures
 * describing those directories.
 *
 * It should be called immediately after FsvDei_SubDirectoryCount. It is
 * guaranteed to always return the number of directories promised by
 * FsvDei_SubDirectoryCount, regardless of whether that number is true or
 * false. This call cannot fail.
 *
 * If however you call some other FsvDei functions after
 * FsvDei_SubDirectoryCount and before FsvDei_ListSubDirectories, then
 * other outcomes are possible.
 *
 */
VOID WINAPI FsvDei_ListSubDirectories( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, LPFSV_DIRENTRYINFO far * lpBuffer);
typedef VOID (WINAPI * LPFNFSVDEI_LISTSUBDIRECTORIES)( LPFSV_DEITREE , LPFSV_DIRENTRYINFO , LPFSV_DIRENTRYINFO far * );

/*============================================================================
 *
 * WINAPI FsvDei_AllFilesCount( lptdei, lpdei, lpCallBack, dwUser): Long
 *
 *    lptdei.     Directory tree
 *    lpdei       Directory
 *    lpCallBack  Callback procedure for selective inclusion
 *    dwUser      User parameter for callback function;
 *
 * This gives a count of all the files and only the files in all the
 * directories, and sub directories of lpdei all the way down the tree.
 * This never returns an error. If something goes wrong, the count will
 * be low, usually zero. The user may be informed, but you will not. If
 * you give it an invalid pointer, it will UAE.
 *
 * The CallBack function will be called once for each file and each
 * directory found. If the call back function returns a zero, FALSE, then
 * the contents of that directory and all its sub directories will not be
 * included in the count. Furthermore, once ignored, the directory is
 * likely to remain invisible on subsequent counts, even with a different
 * call back function, unless you first collapse lpdei with a call to
 * FsvDei_CollapseDir.
 *
 */
LONG WINAPI FsvDei_AllFilesCount( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, WORD wEnumOptions, LPFSVDEI_CALLBACK lpCallBack, DWORD dwUser);
typedef LONG (WINAPI * LPFNFSVDEI_ALLFILESCOUNT)( LPFSV_DEITREE , LPFSV_DIRENTRYINFO, WORD, LPFSVDEI_CALLBACK, DWORD);

/*======================================================================
 *
 * WINAPI FsvDei_ListAllFiles( lptdei, lpdei, lpBuffer ): Void
 *
 *    lptdei   Tree to which lpdei belongs.
 *    lpdei entry for which the files were counted
 *    lpBuffer Place in which the files will be put
 *
 * This fills in the buffer with the files counted in FsvDei_AllFilesCount,
 * or rather with far pointers to data structures describing those files.
 *
 * It should be called immediately after FsvDei_AllFilesCount. It is
 * guaranteed to always return the number of directories promised by
 * FsvDei_AllFilesCount, regardless of whether that number is true or false.
 * This call cannot fail.
 *
 * If however you call some other FsvDei functions after
 * FsvDei_AllFilesCount and before FsvDei_ListAllFiles, then other
 * outcomes are possible.
 *
 */
VOID WINAPI FsvDei_ListAllFiles( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, LPFSV_DIRENTRYINFO far * lpBuffer);
typedef VOID (WINAPI * LPFNFSVDEI_LISTALLFILES)( LPFSV_DEITREE , LPFSV_DIRENTRYINFO, LPFSV_DIRENTRYINFO far *);

/*===========================================================================
 *
 * WINAPI FsvDei_EnumerateDirEntries : long;
 *
 *    lptdei      Tree to which lpdei belongs.
 *    lpdei       entry for which the files were counted
 *    wEnumOptions
 *                Any combination of
 *                   FSV_ENUMOPTION_RECURSE  recurse into subdirectories
 *                   FSV_ENUMOPTION_FILES    return files
 *                   FSV_ENUMOPTION_DIRS     return dirs
 *    lphplpdei   Address of pointer which will point to freshly allocated
 *                table of entries, possibly huge. If NULL nothing will be
 *                returned. Typical usage is
 *                   &hplpdei, where hplpdei is declared
 *                   FSV_DIRENTRYINFO far * far * hplpdei;
 *                hplpdei is the address of a huge array of far pointers
 *
 *
 *    lpCallBack  Callback procedure for selective inclusion
 *    lpUserDword Application supplied parameter to be passed to the
 *                   callback function.
 *
 * If you are going to be using the table entries returned one at a time,
 * and are then going to immediately destroy the storage allocated, it is
 * more efficient to call FsvDei_NextDir or FsvDei_NextEntryOfTree, etc
 * directly. This "get the whole block" function is merely a wrapper for
 * the "Next" functions which get them one at a time. The "Next"
 * functions are themselves wrappers for calls that get a whole directory
 * of subdirectories, or a whole directory of files and subdirectories,
 * at a time.
 *
 * The reason for this double wrapping is that we not only need to get the
 * strings, we need to create a data structure that protects the user from
 * having to deal with strings. The bottom most layer deals with blocks of
 * strings. This layer is private. The "Next" layer deals with pointers to
 * data structures containing strings and pointers to other data structures.
 * It is public, and its use is recommended.
 *
 */
LONG WINAPI FsvDei_EnumerateDirEntries( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei, WORD wEnumOption, FSV_DIRENTRYINFO far * far * far * lphplpdei, LPFSVDEI_CALLBACK lpCallBack, DWORD);
typedef LONG (WINAPI * LPFNFSVDEI_ENUMERATEDIRENTRIES)( LPFSV_DEITREE, LPFSV_DIRENTRYINFO, WORD, FSV_DIRENTRYINFO far * far * far *, LPFSVDEI_CALLBACK, DWORD);




/*

 *============================================================================
 *
 * WINAPI FsvDei_SetDefaultDirTree( lptdei );
 *
 *  FSVDEITREE far * lptdei;
 *
 * This tells the directory tree manipulation code that information stored
 * in this tree can be trusted.
 *
 */

void WINAPI FsvDei_SetDefaultDirTree( FSVDEITREE far * lptdei );


/*

 *============================================================================
 *
 * WINAPI FsvDei_UnSetDefaultDirTree( lptdei );
 *
 *  FSVDEITREE far * lptdei;
 *
 * This tells the directory tree manipulation code that information stored
 * in this tree can no longer be trusted.
 *
 */

void WINAPI FsvDei_UnSetDefaultDirTree( FSVDEITREE far * lptdei );




//===========================================================================
//
// FsvDei_StripDummyPad( lpPath ) : LPTSTR;
//
// LPTSTR lpPath;
//
// This function will check if the path starts with a dummy pad like "Entire Netowrk"
// and if it does it returns a pointer to the start of the valid path
//
// Returns: A pointer to the path after the pad or else NULL
//
LPTSTR WINAPI FsvDei_StripDummyPad( LPTSTR lpPath );
typedef LPTSTR (WINAPI * LPFNFSVDEI_STRIPDUMMYPAD)(LPTSTR);


/**********/

//===========================================================================
//
// FsvBmp_GetFileBmp( szExtension, wAttrib, lpdwLoc, lpdwSize, lpdwLoc2 ) : HDC;
//
//  szExtension      File extension (excluding the dot).
//  wAttrib          Attribute word.
//  lpdwLoc          far pointer to (x,y) offset into bitmap (Normal/OR bmp)
//  lpdwSize         far pointer to width and height
//  lpdwLoc2         far pointer to (x,y) offset into bitmap (AND bmp)
//
// This function will return a bitmap given a file extension. It uses
// the application registration database to determine whether the file is an
// executable or document.
//
// Returns: the HDC handle, or NULL on failure.
//
HDC WINAPI FsvBmp_GetFileBmp(LPSTR szExtension,WORD wAttrib,LPDWORD lpdwLoc,LPDWORD lpdwSize,LPDWORD lpdwLoc2);
typedef HDC (WINAPI * LPFNFSVBMP_GETFILEBMP)(LPSTR,WORD,LPDWORD,LPDWORD,LPDWORD);

//===========================================================================
//
// FsvBmp_GetBoxBmp( wBoxType, lpdwLoc, lpdwSize, lpdwLoc2 ) : HDC;
//
//  wBoxType         Type of box bitmap.
//  lpdwLoc          far pointer to (x,y) offset into bitmap (Normal/OR bmp)
//  lpdwSize         far pointer to width and height
//  lpdwLoc2         far pointer to (x,y) offset into bitmap (AND bmp)
//
// This function will return a bitmap given a box type.
//
// Returns: the HDC handle, or NULL on failure.
//

HDC WINAPI FsvBmp_GetBoxBmp(WORD wBoxType,LPDWORD lpdwLoc,LPDWORD lpdwSize,LPDWORD lpdwLoc2);
typedef HDC (WINAPI * LPFNFSVBMP_GETBOXBMP)(WORD,LPDWORD,LPDWORD,LPDWORD);

//===========================================================================
//
// FsvBmp_GetDirBmp( wFolderId, lpdwLoc, lpdwSize, lpdwLoc2 ) : HDC;
//
//  wFolderId        Folder Id Type - FSV_FOLDER_???.
//  lpdwLoc          far pointer to (x,y) offset into bitmap (Normal/OR bmp)
//  lpdwSize         far pointer to width and height
//  lpdwLoc2         far pointer to (x,y) offset into bitmap (AND bmp)
//
// This function will return a folder bitmap.
//
// Returns: the HDC handle, or NULL on failure.
//
HDC WINAPI FsvBmp_GetDirBmp(WORD wFolderId, LPDWORD lpdwLoc, LPDWORD lpdwSize, LPDWORD lpdwLoc2);
typedef HDC (WINAPI * LPFNFSVBMP_GETDIRBMP)(WORD, LPDWORD, LPDWORD, LPDWORD);

//===========================================================================
//
// FsvBmp_GetDriveBmp( wDriveType, wBmpSize, lpdwLoc, lpdwSize, lpdwLoc2 ) : HDC;
//
//  wDriveType       Drive Type (FSV_DRIVE_???)
//  wBmpSize         Bitmap Size (FSV_BMPSIZE_MINI,SMALL,MEDIUM,LARGE)
//  lpdwLoc          far pointer to (x,y) offset into bitmap (Normal/OR bmp)
//  lpdwSize         far pointer to width and height
//  lpdwLoc2         far pointer to (x,y) offset into bitmap (AND bmp)
//
// This function will return a bitmap given a File Services drive type.
//
// Returns: the HDC handle, or NULL on failure.
//
HDC WINAPI FsvBmp_GetDriveBmp(WORD wDriveType, WORD wBmpSize, LPDWORD lpdwLoc, LPDWORD lpdwSize, LPDWORD lpdwLoc2);
typedef HDC (WINAPI * LPFNFSVBMP_GETDRIVEBMP)(WORD, WORD, LPDWORD, LPDWORD, LPDWORD);

//===========================================================================
// FsvBmp_GetMiscBmp( wBitmapId, lpdwSize, lpdwLoc2 ) : HDC;
//
//  wBitmapId        Bitmap Id Type - FSV_BMPMISC_???.
//  lpdwLoc          far pointer to (x,y) offset into bitmap (Normal/OR bmp)
//  lpdwSize         far pointer to width and height
//  lpdwLoc2         far pointer to (x,y) offset into bitmap (AND bmp)
//
// This function will return a misc bitmap; such as the one used to paint the face
// of the file list icon column header.
//
// Returns: the HDC handle, or NULL on failure.
//
HDC WINAPI FsvBmp_GetMiscBmp( WORD wBitmapId, LPDWORD lpdwLoc, LPDWORD lpdwSize, LPDWORD lpdwLoc2 );
typedef HDC (WINAPI * LPFNFSVBMP_GETMISCBMP)( WORD, LPDWORD, LPDWORD, LPDWORD );

//===========================================================================
//
// FsvBmp_GetDriveCursor( wDriveType ) : HCURSOR;
//
//  wDriveType       Drive Type (FSV_DRIVE_???)
//
// This function will return a drive cursor given a drive type. This cursor is NOT
// to be deleted by the caller.
//
// Returns: the cursor handle, or NULL on failure.
//
HCURSOR WINAPI FsvBmp_GetDriveCursor(WORD wDriveType);
typedef HCURSOR (WINAPI * LPFNFSVBMP_GETDRIVECURSOR)(WORD);

//===========================================================================
//
// FsvBmp_GetFileCursor( wDDFlags ) : HCURSOR;
//
//  wDDFlags        FSV_DD_??? flags.
//
// This function will return a file cursor given a drag and drop flags.
// This cursor is NOT to be deleted by the caller.
//
// Returns: the cursor handle, or NULL on failure.
//
HCURSOR WINAPI FsvBmp_GetFileCursor(WORD wDDFlags);
typedef HCURSOR (WINAPI * LPFNFSVBMP_GETFILECURSOR)(WORD);

//===========================================================================
//
// FsvBmp_GetDirCursor( wDDFlags ) : HCURSOR;
//
//  wDDFlags        FSV_DD_??? flags.
//
// This function will return a dir cursor given a drag and drop flags.
// This cursor is NOT to be deleted by the caller.
//
// Returns: the cursor handle, or NULL on failure.
//
HCURSOR WINAPI FsvBmp_GetDirCursor(WORD wDDFlags);
typedef HCURSOR (WINAPI * LPFNFSVBMP_GETDIRCURSOR)(WORD);

//===========================================================================
//
// FsvBmp_GetNoDropCursor() : HCURSOR;
//
// This function will return the nodrop cursor.
// This cursor is NOT to be deleted by the caller.
//
// Returns: the cursor handle, or NULL on failure.
//
HCURSOR WINAPI FsvBmp_GetNoDropCursor(void);
typedef HCURSOR (WINAPI * LPFNFSVBMP_GETNODROPCURSOR)(void);

/**********/

//===========================================================================
//
// FsvIO_GetDriveList( lpawDrives ) : long;
//
//      lpawDrives      Buffer large enough to hold 27 words.
//
// This function will enumerate all of the drives and place them in the
// given drive list buffer.
//
// Returns: FSV_SUCCESS
//
// Warning. Because of an ad hoc fix placed in LocalDrive, to avoid the
// effect of repeated redundant calls, this function does not necessarily
// return the current state of the drives, but rather the state they were in
// up to a second ago. If you are calling this because you have some reason
// to suspect the drive mapping has just been changed, or if you are
// confident the call you are making will not be made too often, call
// FsvDOSWrap_Flush_Is_Local first. Sorry about this. Unpleasant side effect
// of an efficiency tweak, made necessary by the architecture.
//
//
long WINAPI FsvIO_GetDriveList(LPWORD lpawDrives);
typedef long (WINAPI * LPFNFSVIO_GETDRIVELIST)(LPWORD);

//=====================================================================
// FsvDOSWrap_GetDateAndTime (int iFileHandle) : long;
//
//    iFileHandle    Yes, a file handle, what did you think it was.
//                   And by the way, unnecessary and redundant
//                   documentation obscures, it does not clarify.
//
// This function gets the date and time, given the file handle. It is faster
// than get directory entry if you already have the file handle, because it
// does not generate a disk hit.
//
// Date is high order word, standard DOS packing, time is low order word,
// standard DOS packing
//

LONG WINAPI FsvDOSWrap_GetDateAndTime (int iFileHandle);

//=====================================================================
// FsvDOSWrap_SetDateAndTime () : long;
//
//    iFileHandle    Yes, a file handle, what did you think it was.
//                   And by the way, unnecessary and redundant
//                   documentation obscures, it does not clarify.
//    dwDateTime     Date and time, standard DOS packing.
//
// This function sets the date and time, given the file handle.
//
// Date is high order word, standard DOS packing, time is low order word,
// standard DOS packing
//

LONG WINAPI FsvDOSWrap_SetDateAndTime (int iFileHandle, DWORD dwDateTime);

//===========================================================================
//
// FsvIO_GetDriveType( bDriveId ) : long;
//
//    bDriveId     1 based drive id.
//
// This function will return drive type.
//
// If successful returns:  FSV_DRIVE_NONE
//                         FSV_DRIVE_FLOPPY525
//                         FSV_DRIVE_FLOPPY35
//                         FSV_DRIVE_FIXED
//                         FSV_DRIVE_REMOTE
//                         FSV_DRIVE_RAMDISK
//                         FSV_DRIVE_CDROM
//                         FSV_DRIVE_BERNOULLI
//                         FSV_DRIVE_TAPE
//                         FSV_DRIVE_COMPATICARD
// FSVERR_??? if an error occurs.
//
long WINAPI FsvIO_GetDriveType(BYTE bDriveId);
typedef long (WINAPI * LPFNFSVIO_GETDRIVETYPE)(BYTE);

//===========================================================================
//
// FsvIO_IsDriveRemovable( bDriveId ) : long
//
//    bDriveId     1 based drive id.  Eg, Drive A is 1, 'A' or 'a'
//
// Returns a positive value if removable, zero if unremovable, negative if
// there is an error.
//
long WINAPI FsvIO_IsDriveRemovable ( BYTE bDriveId );

//========================================================================
//
//       void WINAPI FsvDOSWrap_Flush_Is_Local( void )
//
// The function DOSWrap_4409_LocalDrive gets repeated redundant calls.
//
// To minimize the effect of repeated redundant calls, I have put in a
// timer delay. If the most recent call was less than a second ago, it
// does not do a repeat call.
//
// This leads to a bug. Suppose we are doing a check because we know
// something just happened. Then a false (out of date) value will be
// returned. To avoid this I provide a flush function,
// FsvDOSWrap_Flush_Is_Local, which throws out the old values for
// DOSWrap_4409_LocalDrive
//
// Does this add up to a convoluted mess of nasty code. Yes. Why are we
// doing repeated redundant calls to check if a drive is local. Because
// I do not control the architecture. Functions got thrown in higgledy
// piggledy, in a rush project, with the messy irregularities of DOS
// underneath. This is bad code, but I am only doing it because other
// parts of the project have a bad architecture, and it is much too
// late to do the architecture right. So it is not my fault.

void WINAPI FsvDOSWrap_Flush_Is_Local( void );
typedef void (WINAPI * LPFNFSVDOSWRAP_FLUSH_IS_LOCAL)( void );
typedef struct  tagDPMIREGS{  /*dpregs*/
   union
   {
      DWORD dw;
      WORD  w;
   }
   EDI;
   union
   {
      DWORD dw;
      WORD  w;
   }
   ESI;
   union
   {
      DWORD dwEBP;
      WORD  w;
   }
   EBP;
   long dwReserved;
   union
   {
      DWORD dw;
      WORD  w;
   }
   EBX;
   union
   {
      DWORD dw;
      WORD  w;
   }
   EDX;
   union
   {
      DWORD dw;
      WORD  w;
   }
   ECX;
   union
   {
      DWORD dw;
      WORD  w;
   }
   EAX;
   WORD  wFL;
   WORD  wES;
   WORD  wDS;
   WORD  wFS;
   WORD  wGS;
   WORD  wIP;
   WORD  wCS;
   WORD  wSP;
   WORD  wSS;
}  DPMIREGS;

//===========================================================================
//
// FsvIO_DeleteFile( lpFilename, fRecycle, lpCrit, dwCrit ) : long;
//
// Deletes the specified file.
//
// Input:
//    lpFilename     specifies the file to be deleted
//    fRecycle       TRUE = move to Recycle Bin, FALSE = normal delete.
//    lpCrit         Critical error handler.
//    dwCrit         dwparam to pass to critical error handler.
//
// Returns: FSV_SUCCESS if successful, FSV_ERROR if not
//
long WINAPI FsvIO_DeleteFile(LPCSTR lpFilename, BOOL fRecycle, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_DELETEFILE)( LPCSTR, BOOL, ERROR_CALLBACK, DWORD );

//===========================================================================
//
// FsvIO_RenameFile : long;
//
// Renames the specified file to the specified filename.  If the file is an
// archive file and the new name is is *.exe, a new self-extracting archive
// file is created.
//
// Returns FSV_SUCCESS if successful
// 
// If it fails, returns FSVERR_FILE_NOT_FOUND            
//                      FSVERR_PATH_NOT_FOUND     
//                      FSVERR_ACCESS_DENIED      
//                      FSVERR_NOT_THE_SAME_DEVICE
//                         and others possible.
//
long WINAPI FsvIO_RenameFile (
   LPCSTR lpOldFilename,      // Specifies the file to be renames (FULL NAME)
   LPCSTR lpNewFilename,      // Specifies the new filename (FULL NAME)
   ERROR_CALLBACK lpCrit,     // Critical error handler
   DWORD dwCrit,              // Data to pass to critical error handler
   FARPROC  lpProgCBFunc,     // ZIP progress callback function
   DWORD    dwProgCBData,     // ZIP progress callback data
   FARPROC  lpErrCBFunc,      // ZIP error callback function
   DWORD    dwErrCBData       // ZIP error callback data
);
typedef long (WINAPI * LPFNFSVIO_RENAMEFILE )(LPCSTR,LPCSTR,ERROR_CALLBACK,DWORD,FARPROC,DWORD,FARPROC,DWORD);

//===========================================================================
//
// FsvIO_RenameDir( lpOldPathname, lpNewPathname, lpCrit, dwCrit ) : long;
//
// Renames the specified file to the specified filename.
//
// Input:
//      lpOldPathname  specifies the file to be renamed (FULL NAME)
//      lpNewPathname  specifies the new filename (FULL NAME)
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Returns FSV_SUCCESS if successful
// 
// If it fails, returns FSVERR_FILE_NOT_FOUND            
//                      FSVERR_PATH_NOT_FOUND     
//                      FSVERR_ACCESS_DENIED      
//                      FSVERR_NOT_THE_SAME_DEVICE
//
long WINAPI FsvIO_RenameDir ( LPCSTR lpOldPathname, LPCSTR lpNewPathname, ERROR_CALLBACK lpCrit, DWORD dwCrit);
typedef long (WINAPI * LPFNFSVIO_RENAMEDIR )( LPCSTR, LPCSTR, ERROR_CALLBACK, DWORD);

//===========================================================================
//
// FsvIO_GetFileAttributes( lpFilename, lpdwAttributes, lpCrit, dwCrit) : long;
//
// Gets the attributes WORD for the specified file or directory.  This
// function will work correctly if a directory name has a trailing backslash.
//
// Input:
//      lpFilename     far ptr to path\filename
//      lpdwAttributes  far ptr to where the attributes will be placed
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Returns: FSV_SUCCESS if successful, FSVERR_??? if not
//

long WINAPI FsvIO_GetFileAttributes (LPSTR lpFilename,LPDWORD lpdwAttributes, ERROR_CALLBACK lpCrit, DWORD dwCrit);
typedef long (WINAPI * LPFNFSVIO_GETFILEATTRIBUTES )(LPSTR ,LPDWORD, ERROR_CALLBACK, DWORD );

//===========================================================================
//
// FsvIO_SetFileAttributes( lpFilename, dwAttributes, lpCrit, dwCrit ) : long;
//
// Sets the attributes WORD for the specified file.
//
// Input:
//      lpFilename     far ptr to path\filename
//      dwAttributes   new attributes DWORD
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Returns: FSV_SUCCESS if successful, FSVERR_??? if not
//

long WINAPI FsvIO_SetFileAttributes (LPCSTR lpFilename,DWORD dwAttributes, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_SETFILEATTRIBUTES )(LPCSTR,DWORD, ERROR_CALLBACK, DWORD );

//===========================================================================
//
// FsvIO_GetCurrentDrive( void ) : BYTE;
//
// Input:
//     none
//
// Returns: the current drive (1=A, 2=B, etc)
//
BYTE WINAPI FsvIO_GetCurrentDrive (void);
typedef BYTE (WINAPI * LPFNFSVIO_GETCURRENTDRIVE )(void);

//===========================================================================
//
// FsvIO_FastGetDiskFreeSpace( bDrive, lpdwFreeSpace, lpdwTotalSpace, lpCrit, dwCrit)
//
//      bDrive         Drive in question (1 based)
//      lpdwFreeSpace  Address where free space will be put
//      lpdwTotalSpace Address where total space will be put
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
//
// Returns: the disk free space in bytes. Negative values are error numbers,
// usually:
//    FSVERR_INVALID_DRIVE;
//    FSVERR_DRIVE_NOT_READY    
//    FSVERR_NOTDOSDISK         
//    FSVERR_GENERALFAILURE
//    FSVERR_IO_ERROR              
//
// Finds the free space on disk by looking at DOS internals, rather than
// the slow painfull way of rereading the entire FAT every time. Dos
// adjusts its internal free space count whenever it allocates or releases
// clusters, thus it rarely has to re-read the FAT. DOS interrupt 21 36
// re-reads the entire FAT every time, and is therefore excruciatingly
// slow.
// 
// Due to a bizarre unpleasantness in Novell Netware, (They are still
// supporting DOS 2.0!) this routine cannot use the fast access with
// Novell Netware drives, so for ALL networked drives, it still uses the
// slow INT 21 36.
//
// However on network drives (CD ROM drives are network drives to DOS)
// INT 21 36 is redirected to INT 2F 110C, which most networks implement
// considerably faster than DOS, because they had the luxury of designing
// for large fast drives from the beginning.
//
long WINAPI FsvIO_FastGetDiskFreeSpace(BYTE bDrive, LPDWORD lpdwFreeSpace, LPDWORD lpdwTotalSpace, ERROR_CALLBACK lpCrit, DWORD dwCrit);
typedef long (WINAPI * LPFNFSVIO_FASTGETDISKFREESPACE)(BYTE, LPDWORD, LPDWORD, ERROR_CALLBACK, DWORD);

//========================================================================
//
//  long WINAPI FsvIO_GetDiskFreeSpace (
//      BYTE    bDrive,
//      LPWORD  lpwAvailableClusters,
//      LPWORD  lpwClustersPerDrive,
//      LPWORD  lpwSectorsPerCluster,
//      LPWORD  lpwBytesPerSector
//  );
//
//  Returns -1 if no such drive, 0 if success
//
long WINAPI FsvIO_GetDiskFreeSpace(BYTE bDrive,LPDWORD lpdwAvailableClusters,LPDWORD lpdwClustersPerDrive,LPDWORD lpdwSectorsPerCluster,LPDWORD lpdwBytesPerSector, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_GETDISKFREESPACE)(BYTE,LPDWORD,LPDWORD,LPDWORD,LPDWORD, ERROR_CALLBACK, DWORD );
// This definition removed from the API, because people should be using the
// fast one.

//===========================================================================
//
// FsvIO_ChangeCurrentDrive( bDrive ) : long;
//
// Changes the current drive to the specified drive.
//
// Input:
//     WORD wDrive
//         the new drive number (1=A, 2=B, etc)
//
// Returns: FSV_SUCCESS if successful, FSV_ERROR if not
//
long WINAPI FsvIO_ChangeCurrentDrive (BYTE bDrive);
typedef long (WINAPI * LPFNFSVIO_CHANGECURRENTDRIVE )(BYTE);

//===========================================================================
//
// FsvIO_GetDriveLabel( bDrive, lpszLabel, wBytes, lpCrit, dwCrit ) : long;
//
//    bDrive      the drive number  (1=A, 2=B, etc)
//    lpszLabel   buffer to place the label in
//    wBytes      size of the buffer (including the null terminator).
//      lpCrit      Critical error handler.
//      dwCrit      dwparam to pass to critical error handler.
//
// Retrieves the label for the specified drive.
//
// Returns: FSV_ERROR if an error occurs, FSV_SUCCESS if a label returned;
//

long WINAPI FsvIO_GetDriveLabel (BYTE bDrive,LPSTR lpszLabel,WORD wBytes, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_GETDRIVELABEL )(BYTE ,LPSTR ,WORD, ERROR_CALLBACK, DWORD );

//===========================================================================
//
// FsvIO_SetDriveLabel( bDrive, lpszLabel, lpCrit, dwCrit ) : long;
//
//    bDrive      the drive number  (1=A, 2=B, etc)
//    lpszLabel   new drive label
//      lpCrit      Critical error handler.
//      dwCrit      dwparam to pass to critical error handler.
//
// Sets the label for the specified drive.
//
// Returns: 0 if successful, FSVERR_??? if unsuccessful
//

long WINAPI FsvIO_SetDriveLabel (BYTE bDrive,LPSTR lpszLabel, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_SETDRIVELABEL )(BYTE ,LPSTR, ERROR_CALLBACK, DWORD );

//===========================================================================
//
// FsvIO_GetCurrentDir( wDrive, lpBuffer, wCount ) : WORD;
//
//     wDrive       drive to get current directory for (1 based)
//     lpBuffer     far ptr to buffer into which the current directory
//                  string will be copied
//     wCount       length of buffer
//
// Gets the current directory for the specified drive (1 based). If wdrive
// is zero, then the current drive is used. This function returns the full path.
//
// Returns: number of bytes copied
//
WORD WINAPI FsvIO_GetCurrentDir (BYTE bDrive,LPSTR lpBuffer,WORD wCount);
typedef WORD (WINAPI * LPFNFSVIO_GETCURRENTDIR )(BYTE,LPSTR,WORD);

//===========================================================================
//
// FsvIO_ChangeCurrentDir( lpPath ) : long;
//
// Changes the current directory to the specified directory.
//
// Input:
//     LPSTR lpPath
//         far ptr to new directory
//
// Returns: FSV_SUCCESS if successful,
//          FSV_ERROR or FSVERR_PATH_NOT_FOUND if and error occured.
//
long WINAPI FsvIO_ChangeCurrentDir (LPCSTR lpPath);
typedef long (WINAPI * LPFNFSVIO_CHANGECURRENTDIR )(LPCSTR);

//===========================================================================
//
// FsvIO_MakeDir( lpPath, lpCrit, dwCrit ) : long;
//
// Creates the specified directory.
//
// Input:
//      lpPath         far ptr to new directory
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Returns: FSV_SUCCESS if successful. Usually returns a meaninfull error
// number. if unsuccessful.
//
// However the error number FSVERR_ACCESS_DENIED may mean that a file or
// directory already exists with that name, or it may mean you are out of
// disk space, two totally different errors.
//
long WINAPI FsvIO_MakeDir ( LPSTR lpPath, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_MAKEDIR )( LPSTR, ERROR_CALLBACK, DWORD );

//===========================================================================
//
// FsvIO_RemoveDir( lpPath, lpCrit, dwCrit ) : long;
//
// Removes the specified directory.
//
// Input:
//      lpPath         far ptr to directory which is to be removed
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Returns: FSV_SUCCESS if successful
//
// Returns  FSVERR_FILE_NOT_FOUND if no directory to remove.
// Returns  FSVERR_PATH_NOT_FOUND if could not find the path.
// Returns  FSVERR_ACCESS_DENIED  for directory not empty,
//    or you are trying to delete the root directory.
//
// DOES NOT RETURN FSVERR_CURRENT_DIRECTORY, instead bumps the path up one.
//
// Returns FSVERR_IO_ERROR            
//         FSVERR_INVALID_DRIVE       
//         FSVERR_DRIVE_NOT_READY     
//         FSVERR_DISK_WRITE_PROTECTED
//         FSVERR_NOTDOSDISK          
//         FSVERR_GENERALFAILURE
//       as appropriate.
//
long WINAPI FsvIO_RemoveDir ( LPSTR lpPath, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_REMOVEDIR )( LPSTR, ERROR_CALLBACK, DWORD );

//============================================================================
//
// FsvIO_CopyFile( lpszfnSource, lpszfnDest, wFsvFunc, lpfn, dwUserDword, lpCrit, dwCrit ) : long;
//
//      lpszfnSource   Source file path to copy
//      lpszfnDest     Destination file path to copy to
//      wFsvFunc       Should always be FSVFUNC_COPYFILE
//      lpfn           Callback, usually null
//      dwUserDword    Callback parameter, usualy zero
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Copy a file. Assumes a fully qualified path
//
// Normally returns FSV_SUCCESS. If an error occurs, it returns one of the
// following errors:
//    FSVERR_SOURCE_NOT_FOUND
//    FSVERR_SOURCE_PATH_NOT_FOUND
//    FSVERR_DISKFULL
//    FSVERR_SOURCE_ACCESS_DENIED
//    FSVERR_DESTINATION_NOT_FOUND
//    FSVERR_DESTINATION_PATH_NOT_FOUND
//    FSVERR_DESTINATION_ACCESS_DENIED
//    FSVERR_CYCLIC_COPY
//    FSVERR_NO_HANDLES_AVAILABLE
//    FSV_ERROR
//
//
// The callback function can be used to keep windows alive and provide user
// update during long copies. For files larger than 64K, it gets called every
// 64K. For smaller files it never gets called at all, to keep overheads low.
//
long WINAPI FsvIO_CopyFile(LPCSTR lpszfnSource,LPCSTR lpszfnDest, FSVFUNC_ID, PROCESS_CALLBACK lpfn, DWORD dwUserDword, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_COPYFILE)(LPCSTR,LPCSTR, FSVFUNC_ID, PROCESS_CALLBACK, DWORD, ERROR_CALLBACK, DWORD );

//============================================================================
//
// FsvIO_MoveFile( lpszfnSource, lpszfnDest, lpfn, dwUserDword, lpCrit, dwCrit ) : long;
//
//      lpszfnSource    Source file path to copy
//      lpszfnDest      Destination file path to copy to
//      lpfn                Callback, usually null
//      dwUserDword     Callback parameter, usualy zero
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Move a file
//
// Returns FSV_SUCCESS if successful
// If unsuccessful, it returns  FSVERR_SOURCE_NOT_FOUND
//                                          FSVERR_SOURCE_PATH_NOT_FOUND
//                                          FSVERR_SOURCE_ACCESS_DENIED
//                                          FSVERR_NO_HANDLES_AVAILABLE
//
// The callback function can be used to keep windows alive and provide user
// update during long moves. For files larger than 64K, it gets called every
// 64K. For smaller files it never gets called at all, to keep overheads low.
//

long WINAPI FsvIO_MoveFile(LPCSTR lpszfnSource, LPCSTR lpszfnDest, PROCESS_CALLBACK lpfn, DWORD dwUserDword, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_MOVEFILE)(LPCSTR, LPCSTR, PROCESS_CALLBACK, DWORD, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// FsvIO_WipeFile( lpFilename, fDOD, lpfn, dwUserDword, lpCrit, dwCrit ) : long
//
//      lpFilename     specifies the file to be deleted
//      fDOD           Do we waste time doing it the DOD way.
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Wipes then deletes the specified file.
//
// Returns FSV_SUCCESS if successful, FSVERR_IO_ERROR if not
//
//

long WINAPI FsvIO_WipeFile (LPSTR lpszFilename,BOOL fDOD, PROCESS_CALLBACK lpfn, DWORD dwUserDword, ERROR_CALLBACK lpCrit, DWORD dwCrit);
typedef long (WINAPI * LPFNFSVIO_WIPEFILE )(LPSTR, BOOL, PROCESS_CALLBACK, DWORD, ERROR_CALLBACK, DWORD );

//============================================================================
//
// FsvIO_GetDirEntry( lpFullName, lpDirEntry, lpLFN, lpCrit, dwCrit ) : long
//
//      lpFullName     Full Name of the file or directory
//      lpDirEntry     Far pointer to an FSVIO_DIRENTRYSTRUCT
//      lpLFN          Far pointer to buffer to be filled with LFN (may be NULL)
//                     This pointer will be assigned to lpDirEntry->lpszLongDesc.
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Fills in a DirEntry structure for the given file/directory. If the name ends
// in a trailing backslash, it will assume a directory. If it does not end in a
// backslash, it will try to find a matching file, then a matching directory.
//
// Returns:    FSV_SUCCESS if successful, FSV_ERROR if unsuccessful
//
long WINAPI FsvIO_GetDirEntry (
   LPCTSTR                 lpFullName,
   LPFSVIO_DIRENTRYSTRUCT  lpDirEntry,
   LPTSTR                  lpLFN,
   ERROR_CALLBACK          lpCrit,
   DWORD                   dwCrit
);
typedef long ( WINAPI * LPFNFSVIO_GETDIRENTRY)( LPCTSTR, LPFSVIO_DIRENTRYSTRUCT, LPTSTR, ERROR_CALLBACK, DWORD );

//============================================================================
//
// FsvIO_GetNetStruct( lpFullName, lpNet, lpCrit, dwCrit ) : long
//
//      lpFullName     Full Name of the file or directory
//      lpNet          Far pointer to an FSVIO_NETSTRUCT
//      lpCrit         Critical error handler.
//      dwCrit         dwparam to pass to critical error handler.
//
// Fills in a NetEntry structure for the given file/directory. If the name ends
// in a trailing backslash, it will assume a directory. If it does not end in a
// backslash, it will try to find a matching file, then a matching directory.
//
// Returns:    FSV_SUCCESS if successful, FSV_ERROR if unsuccessful
//
long WINAPI FsvIO_GetNetStruct( LPSTR lpFullName, LPFSVIO_NETSTRUCT lpNet, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long ( WINAPI * LPFNFSVIO_GETNETSTRUCT)(LPSTR, LPFSVIO_NETSTRUCT, ERROR_CALLBACK, DWORD );

//============================================================================
//
// FsvIO_IsCritErr( lErr )
//
//    lErr - The File Services error code to evaluate.
//
// Returns TRUE if the specified error code pertains to a critical error
// (i.e. drive not ready, write protected, etc.)
//

#define FsvIO_IsCritErr(lErr) ( ( lErr ) == FSVERR_DISK_WRITE_PROTECTED ||\
                                ( lErr ) == FSVERR_DRIVE_NOT_READY ||\
                                ( lErr ) == FSVERR_GENERALFAILURE ||\
                                ( lErr ) == FSVERR_IO_ERROR ||\
                                ( lErr ) == FSVERR_NOTDOSDISK )

//============================================================================
//
// FsvIO_IsDirTheRoot( lpDir )
//
//       lpDir - Pointer to the directory name to evaluate.
//
// Returns TRUE if the specified directory is the "root" (i.e. "C:\").
//

#define FsvIO_IsDirTheRoot(lpDir) ( ( BOOL ) ( ( lpDir[ 3 ] == '\0' && lpDir[ 0 ] != '\0' && lpDir[ 1 ] == ':' && lpDir[ 2 ] == '\\' ) ||\
                                             ( lpDir[ 0 ] == '\\' && lpDir[ 1 ] == '\0' ) ) )

//============================================================================
//
// FsvUtil_IsAFullName( lpszName )
//
//       lpszName - The file name to evaluate.
//
// Returns TRUE if the specified file name is fully qualified.  That is,
// it contains a drive and a path or UNC server and share name.
//
BOOL WINAPI FsvUtil_IsAFullName( LPCTSTR lpszName );
typedef BOOL (WINAPI * LPFNFSVUTIL_ISAFULLNAME)(LPCTSTR);

//============================================================================
//
// FsvUtil_DoesFileHavePath( lpszName )
//
//       lpszName - The file name to evaluate.
//
// Returns TRUE if the specified file name contains a path.
//

#define FsvUtil_DoesFileHavePath(lpszName) ( ( BOOL ) ( _fstrchr( lpszName, '\\' ) != NULL ) )

//============================================================================
//
// FsvIO_DirExists( lpDirName, lpCrit, dwCrit ) : long
//
//       lpDirName      Directory Name
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
//

long WINAPI FsvIO_DirExists( LPCSTR lpDirName, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_DIREXISTS2)( LPCSTR, ERROR_CALLBACK, DWORD );

//============================================================================
//
// FsvIO_FileExists( lpFileName, lpCrit, dwCrit ) : long
//
//       lpFileName     File Name
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
// Returns TRUE if the file exists, FALSE if it does not
//

long WINAPI FsvIO_FileExists( LPCSTR lpFileName, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_FILEEXISTS2)( LPCSTR, ERROR_CALLBACK, DWORD );

//===========================================================================
//
// FsvIO_IsDriveSubstituted( BYTE bDrive ) : BOOL;
//
//    bDrive   Logical drive
//
// A trivial wrap of int 21.44.09
//
// Returns: TRUE if logical drive is substituted, FALSE if not.
//
BOOL WINAPI FsvIO_IsDriveSubstituted( BYTE bLogicalDrive );
typedef BOOL (WINAPI * LPFNFSVIO_ISDRIVESUBSTITUTED)( BYTE );

//===========================================================================
//
// FsvIO_DoSubDirsExist( lpszDir, lpCrit, dwCrit ) : BOOL;
//
//    lpszDir        Directory to search for subdirs in
//    lpCrit         Critical error handler.
//    dwCrit         dwparam to pass to critical error handler.
//
// Checks to see if the given directory contains subdirectories.
//
// Returns: TRUE if subdirs exist, FALSE if otherwise
//

BOOL WINAPI FsvIO_DoSubDirsExist( LPSTR lpszDir, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef BOOL (WINAPI * LPFNFSVIO_DOSUBDIRSEXIST)( LPSTR, ERROR_CALLBACK, DWORD );

//===========================================================================
//
// FsvIO_IsDirEmpty( lpszDir, lpCrit, dwCrit ) : BOOL;
//
//    lpszDir        Directory to search for dir entries in
//    lpCrit         Critical error handler.
//    dwCrit         dwparam to pass to critical error handler.
//
// Checks to see if the given directory contains anything.
//
// Returns: TRUE if empty, FALSE otherwise
//

BOOL WINAPI FsvIO_IsDirEmpty( LPSTR lpszDir, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef BOOL (WINAPI * LPFNFSVIO_ISDIREMPTY)( LPSTR, ERROR_CALLBACK, DWORD );

/*============================================================================
 *
 * WINAPI FsvIO_IsMediaRemovable( drive ) : long
 *
 *    Checks if a drive employs removable media. Returns FALSE if the drive
 *    does not seem to be removable or if an error occurs. Otherwise returns
 *    true. Does not hit the disk.
 *
 *
 */
BOOL WINAPI FsvIO_IsMediaRemovable( BYTE bDrive );
typedef BOOL (WINAPI * LPFNFSVIO_ISMEDIAREMOVABLE)( BYTE );

/*============================================================================
 *
 * WINAPI FsvIO_IsDriveReady( drive ) : long
 *
 *    Checks if a drive employs removable media. If it does, hits the drive
 *    If a drive is readable, returns zero. If a drive is not readable
 *    returns
 *       FSVERR_INVALID_DRIVE      
 *       FSVERR_DRIVE_NOT_READY
 *    or some similar error.
 *
 *    Anything other than zero or one of the above errors is usually an I/O
 *    error. Drive not ready usually means no disk in the drive, or the
 *    drive door open.
 *
 *    This function can also be used to determine if a drive exists or not.
 *    It will not attempt to hit on a nonexistant drive
 *
 *    This is a more sophisticated version of "IsDiskLoaded"
 *
 *
 *    In response to popular demand that I support bad code I have just
 *    created a DOS hack so twisted that I want to show everyone.
 *
 *    Several people have asked for a fast method of detecting whether there
 *    is a disk in the drive. Of course any code that needs to check in
 *    advance whether there is a disk in the drive probably has unacceptable
 *    error handling.
 *
 *    All FsvIO routines have (or are supposed to have) error numbers that
 *    correspond to no such drive, and drive not ready (for removable
 *    drives, drive not ready means no disk in the drive).
 *
 *    What good code should do is bubble these error numbers up and up,
 *    cleaning up as they retreat, then at the top present an error message
 *    to the user, then return to windows.
 *
 *    But sometimes, due to the accidents of history, we wind up being
 *    forced to write really bad code, for example my change volume label
 *    code (modified from John Teddy, who modified it from WNABSIO).
 *    Therefore I have created a routine "FsvIO_IsDriveReady", which takes no
 *    perceptible time if the root directory of a removable drive is already
 *    in a buffer, and if it is not in a buffer, causes it to be in a
 *    buffer. This does take perceptible time, but since the next operation
 *    is usually to read the root directory, this does not cost anything.
 *
 *    How do I fool DOS into doing this:
 *
 *    I look for a file in the root directory called NUL. Dos always finds
 *    this file immediately, before it actually reads the root directory.

 *
 *    Usual errors returned by FsvIO_IsDriveReady are:
 *       FSVERR_DRIVE_NOT_READY
 *       FSVERR_INVALID_DRIVE
 *       FSVERR_NOTDOSDISK
 *       FSVERR_GENERALFAILURE
 *
 *    I repeat: If you need to use this routine, you should carefully
 *    examine the design of your code. Of course, sometimes, because you need
 *    to support existing code which was written in a subtly different
 *    context, or which is simply obsolete, you have no choice but to go
 *    with a bad design.
 *
 */
LONG WINAPI FsvIO_IsDriveReady( BYTE bDrive );
typedef LONG (WINAPI * LPFNFSVIO_ISDRIVEREADY)( BYTE );

/*============================================================================
 *
 * FsvIO_HugeMemCopy( hpDest, hpSource, dwSize ) : void;
 *
 *    hpDest         
 *    hpSource       
 *    dwSize         size of block to be copied
 *
 * This copies a huge block fast. In the inner loop it only uses far pointers
 * and a word index.
 *
 * No huge pointers in the inner loop!
 *
 */
void WINAPI FsvIO_HugeMemCopy( char far *, char far *, DWORD );
typedef void (WINAPI * LPFNFSVIO_HUGEMEMCOPY)( char far *, char far *, DWORD );

//===========================================================================
//
// FsvIO_GetDriveFormat( ) : long;
//
//
//    lpFormat,   Address of format data structure
//    bDriveId    Drive containing disk whose format is sought
//
// Comments: Gets the drive default format. If it successfully gets the
// format it returns zero and stores information describing the format in
// the FSVIO_FORMATSTRUCT.
//
// Return value: Normally zero. Only likely error is
// FSVERR_INVALID_DRIVE
//
long WINAPI FsvIO_GetDriveFormat( LPFSVIO_FORMATSTRUCT lpFormat, BYTE bDriveId);
typedef long (WINAPI * LPFNFSVIO_GETDRIVEFORMAT)( LPFSVIO_FORMATSTRUCT , BYTE );

//===========================================================================
//
// FsvIO_GetDiskFormat( ) : long;
//
//    lpFormat,   Address of format data structure
//    bDriveId    Drive containing disk whose format is sought
//
//
// Comments: Gets the format that the disk actually uses. If the disk is
// unformatted it returns an error code. If it successfully gets the
// format it returns zero and stores information describing the format in
// the FSVIO_FORMATSTRUCT. This replaces "IsDiskFormatted".
//
// Return value: The error number will distinguish between an unreadable
// disk in the drive and a blank disk in the drive. An empty drive is
// FSVERR_DRIVE_NOT_READY, a nonexistent drive is
// FSVERR_INVALID_DRIVE, any other error is either an
// unreadable disk, or one with an alien format. 
//
long WINAPI FsvIO_GetDiskFormat( LPFSVIO_FORMATSTRUCT lpFormat, BYTE bDriveId );
typedef long (WINAPI * LPFNFSVIO_GETDISKFORMAT)( LPFSVIO_FORMATSTRUCT , BYTE );

//===========================================================================
//
// FsvIO_GetCapacityFormat( ) : long;
//
//
//    lpFormat,   Address of format data structure
//    uCapacity   Desired capacity. Must be one of 320, 360, 640, 1200,
//       720, 1440, 2880
//
//
long WINAPI FsvIO_GetCapacityFormat( LPFSVIO_FORMATSTRUCT lpFormat, WORD   wCapacity );
typedef long (WINAPI * LPFNFSVIO_GETCAPACITYFORMAT)( LPFSVIO_FORMATSTRUCT , WORD );

//===========================================================================
//
// FsvIO_InitFormatProgress( ) : long
//    lpformprog, Format progress structure.
//    lpFat,      Fat buffer
//    lpFormat,   Format that you intend the target disk to be formatted to
//    bDriveId    Drive number
//
// Call this to initialize your format progress structure after you have
// obtained the format that will be used on the disk and after you have
// allocated the FAT that will be later placed on the disk. You do not need
// to obtain the drive format, InitFormatProgress gets that for you.
//
// Typically used as:
// FSVIO_FORMATPROG formprog;
// FSVIO_FORMATSTRUCT   format;
// FsvIO_GetCapacityFormat( &format, 1200);
// FsvIO_InitFormatProgress(
//    &formprog,
//    lpFat,
//    &format,
//    bDriveId
// );
//

long WINAPI FsvIO_InitFormatProgress( LPFSVIO_FORMATPROG lpformprog, LPBYTE   lpFat, LPFSVIO_FORMATSTRUCT   lpFormat, BYTE bDriveId );

//===========================================================================
//
// FsvIO_CleanupFormatProgress() : long
//    lpformprog, Format progress structure.
//
// The Fat is not discarded. You have to do that yourself. In fact
// at present this routine does nothing at all, it is placed in here for
// symmetry, and because it probably will have to do something by and by,
// and I hope to avoid yet another API change.

long WINAPI FsvIO_CleanupFormatProgress( LPFSVIO_FORMATPROG formprog );

//===========================================================================
//
// FsvIO_SetDriveFormat( ) : long;
//
//    lpFormat,      Address of format data structure
//    bDriveId,
//
// Flushes buffers, prepares the disk drive for format
//
// 
// You must call this to prepare the diskdrive for formatting a disk. You
// set the format to the desired format.
//
// After you have issued this command some DOS functions will behave
// strangely with the the drive. Get all the information that you need to get
// before issuing this call.
//
// Write Track and Read Track are not affected. FormatTrack is affected.
// This call puts the data structures maintained by DOS into a fit state for
// formatting the disk. You will need to issue this call even if you are
// formatting a disk to the same format as the default format of the drive.
//
// Before doing this get the original format associated with a drive, so
// that you can restore it later with: FsvIO_RestoreDriveFormat
//

long WINAPI FsvIO_SetDriveFormat( LPFSVIO_FORMATSTRUCT lpFormat, BYTE bDriveId);
typedef long (WINAPI * LPFNFSVIO_SETDRIVEFORMAT)( LPFSVIO_FORMATSTRUCT, BYTE);

//===========================================================================
//
// FsvIO_RestoreDriveFormat( ) : long;
//
//    lpFormat,      Address of format data structure
//    bDriveId,
//
// Calling SetDriveFormat( &NewFormat), RestoreDriveFormat(&OriginalFormat)
// also has the beneficial and sometimes essential effect of forcing the disk
// change flag. If you write to the FAT tables on the disk, and fail to force
// the disk change flag then DOS will keep its cached copy of what it thinks
// the FAT is, which can lead to undesirable results. The disk change flag
// invalidates DOS's cached FAT.
//
// You shoul Restore the drive format after writing the FAT and before doing
// any normal file operations to the disk. MakeSystem disk does normal file
// operations. If you use this before you do the restore strange and obscure
// errors are possible and likely.

long WINAPI FsvIO_RestoreDriveFormat( LPFSVIO_FORMATSTRUCT lpFormat, BYTE bDriveId );
typedef long (WINAPI * LPFNFSVIO_RESTOREDRIVEFORMAT)( LPFSVIO_FORMATSTRUCT , BYTE);

//===========================================================================
//
// FsvIO_FormatTrack( ) : long;
//
//    lpformprog     Address of format progress structure
//    uCylinder,
//    uHead
//
// This is used in a DOS format, or where the disk has a different format
// from the one desired. Formats one side of one cylinder, not a bunch of
// cylinders as was proposed. If we were to format a bunch of cylinders in
// one go windows would shut down for an intolerable period. Doing it this
// way allows you to keep windows alive and maintain an abort box.
//
// If an error occurs the track is marked as bad in the FAT and the error
// number is returned.
//
// Return Value: Normally zero.
//
// FSVERR_ACCESS_DENIED bad track, If there is no disk in the drive, or the
// disk is write protected you will never get this error. Instead you will
// get: FSVERR_INVALID_FAT. A track necessary for the one of the FATS or the
// directory has failed. Such a track makes the disk unusable. Usually
// caused by no disk in drive, a very bad disk, or a write protected disk.
// Use FsvDOSWrap_59_ExtendedError() if you wish for a more informative error
// number, FSV_EXTENDED_ERR_INVALID_DRIVE Invalid drive.
//
// If you get a bad track while preparing a blank disk you should inform the
// user that some of the tracks on the disk will be bad and keep right on
// formatting unless the user aborts the format. (As he should). This is
// consistent with what Microsoft does. If you get a bad track while copying
// a disk you should Abort Retry Ignore, as a bad track on the target will
// lead to data on the target disk being unreadable.

long WINAPI FsvIO_FormatTrack( LPFSVIO_FORMATPROG  lpformprog , UINT uCylinder, UINT uHead);
typedef long (WINAPI * LPFNFSVIO_FORMATTRACK)( LPFSVIO_FORMATPROG, UINT, UINT );

//============================================================================
// FsvIO_ExtendedErr : long
//
//
// Returns: Zero if success. Wide variety of errors.
//    Amongst the many possible errors are 
//    FSVERR_DISKFULL
//    FSVERR_INVALID_DRIVE       
//    FSVERR_DRIVE_NOT_READY     (usually means no disk in drive)
//    FSVERR_DISK_WRITE_PROTECTED
//
//
long WINAPI FsvIO_ExtendedErr( void );
typedef long (WINAPI * LPFNFSVIO_EXTENDEDERR)( void );

//========================================================================
//
//       WINAPI FsvIO_BeginCriticalSection (void)
//
//       Locks out any dos boxes from accessing the disk whilst you are doing
//       something indivisible.
//
//       If a dos box was copying data to a disk and at the same time the user
//       attempted to copy the entire disk, the resulting disk might wind up
//       in a bad state. On the other hand if a user is crazy enough to do
//       that, he ought to expect a strange outcome.
//       
//       Guarantees that DOS internals will be in a good, and unchanging
//       state.
//       
void WINAPI FsvIO_BeginCriticalSection (void);
typedef void (WINAPI * LPFNFSVIO_BEGINCRITICALSECTION )(void);

//========================================================================
//
//       WINAPI FsvIO_FlushDiskBuffers : void
//       
//               Forces the disk buffers into alignment with the disk.
//               Useless unless you have first called
//               FsvIO_BeginCriticalSection, because they can swiftly get out
//               of alignment again.
//
void WINAPI FsvIO_FlushDiskBuffers(void);
typedef void (WINAPI * LPFNFSVIO_FLUSHDISKBUFFERS)(void);

//========================================================================
//
//       WINAPI FsvIO_EndCriticalSection (void)
//               
//       Call this as soon as possible after FsvIO_BeginCritical section.
//
void WINAPI FsvIO_EndCriticalSection (void);
typedef void (WINAPI * LPFNFSVIO_ENDCRITICALSECTION )(void);

//===========================================================================
//
// FsvIO_EnumerateDirEntries( lpPath, lpFileSpec, lphUE, wOptions,
//                            lpEnumDirEntryCallback, dwParam) : long;
//
//    lpPath            Full path to be enumerated (no filespec).
//    lpFileSpec        File spec to match. (NULL is equivalent to *.*)
//    lphUE             Pointer to UnErase object to allocate or use.  If
//                      content of pointer is NULL, new object will be
//                      allocated.  If not NULL, then it must point to a
//                      previously allocated object.  If the pointer itself
//                      is NULL, this function is unable to enumerate
//                      deleted objects.
//    wOptions          Option Flags (FSV_ENUMOPTION_???)
//    lpEnumDirEntryCallback  Callback function to be called with each enumeration.
//    dwParam           Application supplied parameter to be passed to the
//                      callback function.
//
// This function will enumerate the specified dir entries within a given
// path. If the callback function returns FSV_ENUM_ABORT, then the enumeration
// process is aborted. If the FSV_ENUMOPTION_POPUPNOTIFY flag is set, then the
// callback function is called with a NULL DIRENTRYSTRUCT once for each popup.
// If FSV_ENUMOPTION_ZIPS is specified, then a ZIP/ARC/PAK file does not appear
// as a file - only as a directory with the FSV_ATTRIB_ZIP flag set.
//
// Returns: Number of Files "Added" to the count, or FSVERR_??? if an error occurs.
//
long WINAPI FsvIO_EnumerateDirEntries(LPSTR lpPath,LPSTR lpFileSpec,LPVOID * lphUE, WORD wOptions,LPFSV_ENUMDIRENTRYCALLBACK lpCallback,DWORD dwParam);
typedef long (WINAPI * LPFNFSVIO_ENUMERATEDIRENTRIES)(LPSTR,LPSTR,LPVOID *,WORD,LPFSV_ENUMDIRENTRYCALLBACK,DWORD );

/**********/


//============================================================================
//
// FsvIO_OpenFile( lpszfn, lpofs, fuMode, lpCrit, dwCrit ) : long;
//
//       lpszfn         file path
//       lpofs          pointer to OFSTRUCT (filled in by function)
//       fuMode         Window sharing attibutes, for example
//                         READ | OF_SHARE_DENY_WRITE
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
//
// Normally returns file handle in the low order word (a non-negative number
// of type HFILE).  Note that zero is a valid file handle.  If an error
// occurs, it returns the appropriate error number, always a negative number.

long WINAPI FsvIO_OpenFile( LPCTSTR lpszfn, LPOFSTRUCT lpofs, UINT fuMode, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_OPENFILE)( LPCTSTR, LPOFSTRUCT, UINT, ERROR_CALLBACK, DWORD );


//============================================================================
//
// FsvIO_CreateFile( lpszfn, uAttrib, lpCrit, dwCrit ) : long;
//
//       lpszfn         file path
//       uAttrib        File attributes, for example
//                         _A_RDONLY | _A_HIDDEN | _A_SYSTEM
//                         possible attributes are _A_NORMAL, _A_RDONLY,
//                         _A_HIDDEN, _A_SYSTEM, _A_ARCH    
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
//
// Normally returns file handle in the low order word (a non-negative number
// of type HFILE).  Note that zero is a valid file handle.  If an error
// occurs, it returns the appropriate error number, always a negative number.

long WINAPI FsvIO_CreateFile( LPCTSTR lpszfn, UINT uAttrib, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef long (WINAPI * LPFNFSVIO_CREATEFILE)( LPCTSTR, UINT, ERROR_CALLBACK, DWORD );

//============================================================================
//
// FsvIO_Read( hf, lpBuff, wSize, lpCrit, dwCrit ) : long;
//
//       hf             file Handle
//       lpBuff         Buffer to read into
//       wSize          Must be 0xFFFE or less
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
//       Normally returns a positive number, the number of bytes read. If
//       this is less than wSize we reached the end of file. A negative
//       number indicates an error.
//
//    This function is simply a wrapper for the windows function, which is
//    identical to the dos interrupt, so it is of rather marginal utility.
//

long WINAPI FsvIO_Read( HFILE hf, LPSTR lpBuff, WORD wSize, ERROR_CALLBACK lpCrit, DWORD dwCrit );

//============================================================================
//
// FsvIO_ReadByts( hf, lpBuff, wSize, lpCrit, dwCrit ) : long;
//
//       hf             file Handle
//       lpBuff         Buffer to read into
//       wSize          Must be 0xFFFE or less
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
//       This function will try to read the number of bytes specified.  A
//       negative number indicates an error.  If less bytes were read than
//       requested then FSVERR_EOF will be returned.
//

long WINAPI FsvIO_ReadByts(
   HFILE hf,
   LPSTR lpBuff,
   WORD  wSize,
   ERROR_CALLBACK lpCrit, DWORD dwCrit
);

/**/

//============================================================================
//
// FsvIO_Write( hf, lpBuff, wSize, lpCrit, dwCrit ) : long;
//
//
//       hf             file Handle
//       lpBuff         Buffer to write from into
//       wSize          Must be 0xFFFE or less
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
//       Normally returns number of bytes written, a positive value. If
//       something goes wrong, it returns an error, a negative value. If
//       we run out of space then the number of bytes actually written will
//       be returned.


long WINAPI FsvIO_Write(
   HFILE hf,
   LPSTR lpBuff,
   WORD  wSize,
   ERROR_CALLBACK lpCrit, DWORD dwCrit
);
typedef long ( WINAPI* LPFNFSVIO_WRITE )( HFILE, LPSTR, WORD, ERROR_CALLBACK );

//============================================================================
//
// FsvIO_WriteBytes( hf, lpBuff, wSize, lpCrit, dwCrit ) : long;
//
//
//       hf             file Handle
//       lpBuff         Buffer to write from into
//       wSize          Must be 0xFFFE or less
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
//       Normally returns number of bytes written, a positive value. If
//       something goes wrong, it returns an error, a negative value. Unlike
//       the windows function, if we run out of space it returns
//       FSVERR_DISKFULL;

long WINAPI FsvIO_WriteBytes(
   HFILE hf,
   LPSTR lpBuff,
   WORD  wSize,
   ERROR_CALLBACK lpCrit, DWORD dwCrit
);
typedef long ( WINAPI* LPFNFSVIO_WRITEBYTES )( HFILE, LPSTR, WORD, ERROR_CALLBACK );

//============================================================================
//
// FsvIO_Seek( hf, lOffset, bOrigin, lpCrit, dwCrit ) : long;
//
//
//       hf             file Handle
//       lOffset        Distance to move
//       bOrigin        0 means move relative to start
//                      1 means move relative to current position
//                      2 means move relative to end.
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
//       Normally returns the file position, a positive value. Negative
//       value is an error, (Or else you have one hell of a hard disk)


long WINAPI FsvIO_Seek( HFILE hf, long lOffset, BYTE bOrigin, ERROR_CALLBACK lpCrit, DWORD dwCrit );

//============================================================================
//
// FsvIO_Tell( hf, lpCrit, dwCrit ) : long;
//
//
//       hf             file Handle
//       lpCrit         Critical error handler.
//       dwCrit         dwparam to pass to critical error handler.
//
//       Normally returns the file position, a positive value. Negative
//       value is an error, (Or else you have one hell of a hard disk)


long WINAPI FsvIO_Tell( HFILE hf, ERROR_CALLBACK lpCrit, DWORD dwCrit );


//============================================================================
// FsvUtil_IsPathLenValid( uLen ) : BOOL;
//
//    uLen           Number specifying length to check
//
// This macro determines if a specified length is a valid length for the
// path portion of a fully qualified path + filename.
//
// Returns:  TRUE if valid, FALSE if too long
//
//
#define FsvUtil_IsPathLenValid(uLen) ( BOOL ) ( ( uLen ) < FSV_PATHBUF_LEN - ( FSV_FILEPREFIX_LEN + 1 + FSV_FILEEXTENSION_LEN ) )

//========================================================================
//
//  WORD WINAPI FsvUtil_Build11Char (
//      LPSTR lpDst,
//      LPCSTR lpSrc);
//
//  Make an 11-char directory entry filename from an ASCIIZ name.
//
WORD    WINAPI FsvUtil_Build11Char (LPSTR lpDst, LPCSTR lpSrc);
typedef WORD (WINAPI * LPFNFSVUTIL_BUILD11CHAR )(LPSTR,LPCSTR);

//========================================================================
//
//  WORD WINAPI FsvUtil_Build13Char (
//      LPSTR lpDst,
//      LPSTR lpSrc);
//
//  Make an ASCIIZ filename from an 11-char directory entry name.
//
WORD    WINAPI FsvUtil_Build13Char (LPSTR lpDst, LPCSTR lpSrc);
typedef WORD (WINAPI * LPFNFSVUTIL_BUILD13CHAR )(LPSTR, LPCSTR);

//===========================================================================
//
// FsvUtil_GetNetworkDriverHandle( void ) : HINSTANCE;
//
// This routine determines if a network driver is installed, and if so, then
// returns the handle to the library.  If the network has not been loaded,
// the driver will be loaded.
//
// Returns: Network driver handle if network installed, LoadLibrary error code if not.
//
HINSTANCE WINAPI FsvUtil_GetNetworkDriverHandle(void);
typedef HINSTANCE (WINAPI * LPFNFSVUTIL_GETNETWORKDRIVERHANDLE)(void);

//============================================================================
// FsvUtil_IsWorkgroup() : BOOL;
//
//
// This function returns whether a workgroup network driver is loaded or
// not.
//
//

BOOL WINAPI FsvUtil_IsWorkgroup();
typedef BOOL (WINAPI * LPFNFSVUTIL_ISWORKGROUP)(void);

//============================================================================
// FsvUtil_NetSetDefaultDrive( wDriveId ) : WORD;
//
//    wDriveId       Zero-based index specifying new default drive
//
// Sets the default drive for the WNetDisconnectDialog function.  Only
// available under WFW.
//
// Return value is WN_NOT_SUPPORTED or WN_SUCCESS.
//
//

WORD WINAPI FsvUtil_NetSetDefaultDrive( WORD wDriveId );
typedef WORD (WINAPI * LPFNFSVUTIL_NETSETDEFAULTDRIVE)( WORD );

//============================================================================
// FsvUtil_GetDirectoryType( lpName, lpType ) : WORD;
//
//    lpName         String specifying a full directory name.  Must not have
//                   a trailing backslash.
//    lpType         A 16-bit value to receive the directory type ID.
//                      0 = Normal
//                      1 = Network
//                      2 = Shared
//                      (WNDT_* defines in WINNET.H)
//
// This function determines the directory type of the specified fully-
// qualified directory name.  The return value indicates an error value from
// the network driver.
//
//

WORD WINAPI FsvUtil_GetDirectoryType( LPSTR lpName, LPINT lpType );
typedef WORD (WINAPI * LPFNFSVUTIL_GETDIRECTORYTYPE)( LPSTR, LPINT );

//============================================================================
// FsvUtil_GetShareName( lpPath, lpName, wCount ) : WORD;
//
//    lpPath         String specifying a full directory name.  Must not have
//                   a trailing backslash.
//    lpName         Buffer to receive share name for a given path.
//    wCount         Buffer length of buffer specified by lpName.
//
// This function retrieves the share name of a specified fully-qualified
// directory name.  The return value is a network driver error code.
//

WORD WINAPI FsvUtil_GetShareName( LPSTR lpPath, LPSTR lpName, WORD wCount );
typedef WORD (WINAPI * LPFNFSVUTIL_GETSHARENAME)( LPSTR, LPSTR, WORD );

//============================================================================
// FsvUtil_RestoreConnection( hwndParent, lpszDevice ) : LONG;
//
//    hwndParent     Window handle of parent of any dialog
//    lpszDevice     Name of device to connect
//
// This function calls the WNETRestoreConnection function of the current
// network driver.  See DDK Reference.
//

LONG WINAPI FsvUtil_RestoreConnection( HWND hwndParent, LPSTR lpszDevice );
typedef LONG (WINAPI * LPFNFSVUTIL_RESTORECONNECTION)( HWND, LPSTR );

//===========================================================================
//
// FsvUtil_IsNovellNetwork( void ) : BOOL;
//
// This routine determines if the installed network is a Novell network.
//
// Returns: TRUE if Novell Network is installed. FALSE otherwise.
//
BOOL WINAPI FsvUtil_IsNovellNetwork(void);
typedef BOOL (WINAPI * LPFNFSVUTIL_ISNOVELLNETWORK)(void);

//===========================================================================
//
// FsvUtil_GetNetworkUserName( wConnId, dwUserId, lpszBuffer, wLen ) : long
//
//
// This routine will check for existance and load the Netware Support Dlls.
//
// Returns: FSV_SUCCESS if found, FSV_ERROR if an error occurs.
//
long WINAPI FsvUtil_GetNetworkUserName( WORD wConnId, DWORD dwUserId, LPSTR lpszBuffer, WORD wLen);
typedef long (WINAPI * LPFNFSVUTIL_GETNETWORKUSERNAME)( WORD, DWORD , LPSTR , WORD );

//===========================================================================
//
// FsvUtil_CompressTime( lpwTime, lptm): VOID
//
//  lpwTime         Destination (compressed time).
//  lptm            Source (tm struct)
//
// Converts a tm structure into a compressed directory entry time format.
//
// Returns: none.
//
VOID  WINAPI FsvUtil_CompressTime (LPWORD lpwTime,TM far * lptm);
typedef VOID (WINAPI * LPFNFSVUTIL_COMPRESSTIME )(LPWORD ,TM far * );

//===========================================================================
//
// FsvUtil_CompressDate( lpwDate, lptm): VOID
//
//  lpwDate         Destination (compressed date).
//  lptm            Source (tm struct)
//
// Converts a tm structure into a compressed directory entry date format.
//
// Returns: none.
//
VOID  WINAPI FsvUtil_CompressDate (LPWORD lpwDate,TM far * lptm);
typedef VOID (WINAPI * LPFNFSVUTIL_COMPRESSDATE )(LPWORD ,TM far * );

//===========================================================================
//
// FsvUtil_ExpandTime( lptm, wTime): VOID
//
//  lptm            Destinateion (tm struct)
//  wTime           Source (compressed time).
//
// Converts a compressed directory entry time format to a tm structure.
// NOTE: this only fills the tm_sec, tm_min, and tm_hour fields.
//
// Returns: none.
//
VOID  WINAPI FsvUtil_ExpandTime (TM far * lptm,WORD wTime);
typedef VOID (WINAPI * LPFNFSVUTIL_EXPANDTIME )(TM far * ,WORD );

//===========================================================================
//
// FsvUtil_ExpandDate( lptm, wDate): VOID
//
//  lptm            Destinateion (tm struct)
//  wDate           Source (compressed date).
//
// Converts a compressed directory entry date format to a tm structure.
// NOTE: this only fills the tm_mday, tm_mon, and tm_year fields.
//
// Returns: none.
//
VOID  WINAPI FsvUtil_ExpandDate (TM far * lptm,WORD wDate);
typedef VOID (WINAPI * LPFNFSVUTIL_EXPANDDATE )(TM far * ,WORD );

//==========================================================================
//
// FsvUtil_CondensePath( lpszPath, lpszCondsdPath, wMaxChs ) : LPSTR
//
//    lpszPath       - Far pointer to the path to evaluate.
//    lpszCondsdPath - Far pointer to the path that has been "condensed".
//    wMaxChs        - The maximum size allowed for the path.  This
//                     excludes the null terminator.
//
// This function, "condenses" the path name if it is contains more characters
// than the maximum allowed.  The path is "condensed" by inserting an
// ellipsis between the first directory and the last.  For example
// D:\JON\FILES\APPS\WORD would be converted to D:\JON\...\WORD.  If the
// condensed path still exceeds the maximum size, it will be truncated.
//
// This function will return the pointer to the "path" that was condensed.
//
LPSTR WINAPI FsvUtil_CondensePath(LPSTR lpszPath,LPSTR lpszCondsdPath,WORD wMaxChs);
typedef LPSTR (WINAPI * LPFNFSVUTIL_CONDENSEPATH)(LPSTR ,LPSTR ,WORD );

//===========================================================================
// FsvUtil_LoadExtTable( void ): void
//
// Loads the [Extensions] section from the win.ini file.
//
// Returns: none
//
void  WINAPI FsvUtil_LoadExtTable( void );
typedef void (WINAPI * LPFNFSVUTIL_LOADEXTTABLE)( void );

//===========================================================================
// FsvUtil_FreeExtTable( void ): void
//
// Frees the extension table.
//
// Returns: none
//
void  WINAPI FsvUtil_FreeExtTable( void );
typedef void (WINAPI * LPFNFSVUTIL_FREEEXTTABLE)( void );

//===========================================================================
// FsvUtil_GetDocumentString( lpszDocs, wLen ): WORD
//
//  lpszDocs         Buffer to contain document wildcards
//  wLen             length of buffer
//
// Fills a buffer with document wildcards. (i.e. *.DOC *.WK1 ...)
//
// Returns: number of characters placed in the buffer.
//
WORD WINAPI FsvUtil_GetDocumentString(LPSTR lpszDocs, WORD wLen);
typedef WORD (WINAPI * LPFNFSVUTIL_GETDOCUMENTSTRING)(LPSTR, WORD);

//===========================================================================
// FsvUtil_GetProgramString( lpszProgs, wLen ): WORD
//
//  lpszProgs        Buffer to contain program wildcards
//  wLen             length of buffer
//
// Fills a buffer with program wildcards. (i.e. *.DOC *.WK1 ...)
//
// Returns: number of characters placed in the buffer.
//
WORD WINAPI FsvUtil_GetProgramString(LPSTR lpszProgs, WORD wLen);
typedef WORD (WINAPI * LPFNFSVUTIL_GETPROGRAMSTRING)(LPSTR, WORD);

//===========================================================================
// FsvUtil_GetArchiveString( lpszArch, wLen ): WORD
//
//  lpszProgs        Buffer to contain archive wildcards
//  wLen             length of buffer
//
// Fills a buffer with archive wildcards. (i.e. *.DOC *.WK1 ...)
//
// Returns: number of characters placed in the buffer.
//
WORD WINAPI FsvUtil_GetArchiveString(LPSTR lpszArch, WORD wLen);
typedef WORD (WINAPI * LPFNFSVUTIL_GETARCHIVESTRING)(LPSTR, WORD );

//===========================================================================
// FsvUtil_IsExtDocument( lpszExt ): BOOL
//
//  lpszExt         Extension to check
//
// Checks to see if the extension signifies a document. The incoming extension
// must be lowercase and NULL terminated.
//
// Returns: TRUE if a document, FALSE if not.
//
BOOL    WINAPI FsvUtil_IsExtDocument(LPCSTR lpszExt);
typedef BOOL (WINAPI * LPFNFSVUTIL_ISEXTDOCUMENT)(LPCSTR );

//===========================================================================
// FsvUtil_IsExtExecutable( lpszExt ): BOOL
//
//  lpszExt         Extension to check
//
// Checks to see if the extension signifies an executable. The incoming extension
// must be lowercase and NULL terminated.
//
// Returns: TRUE if a document, FALSE if not.
//
BOOL    WINAPI FsvUtil_IsExtExecutable(LPCSTR lpszExt);
typedef BOOL (WINAPI * LPFNFSVUTIL_ISEXTEXECUTABLE)(LPCSTR);

//===========================================================================
//
// FsvUtil_IsZipFile( lpszName, lpwType ) : BOOL
//
//    lpszName     Incoming file name (may include path)
//    lpwType      Will be filled with the archive file type.
//                 ZIP_??? - defined in wnzip.h
//
// This routine will check to see if the file name matches that of
// a known archive file.
//
// Returns: return TRUE if an archive file (zip/arc/pak), FALSE if not.
//
BOOL WINAPI FsvUtil_IsZipFile(LPCSTR lpszName, LPWORD lpwType);
typedef BOOL (WINAPI * LPFNFSVUTIL_ISZIPFILE)(LPCSTR,LPWORD);

//==========================================================================
//
// BOOL FsvUtil_ValidateFileName( lpszFileMask, f83 )
//
//    lpszFileName - Pointer to the "file name" to validate.
//    f83 - Validate as 8.3 filename
//
// This function validates the "file name" (cannot have a path) entered by
// the user.  It checks for multiple extension separators, bad characters,
// too many characters, and wild card characters.
//
// This function returns "TRUE" if the file name is valid and "FALSE" if it
// is bogus.
//
//
BOOL WINAPI FsvUtil_ValidateFileName( LPCTSTR lpszFileName, BOOL f83 );
typedef BOOL (WINAPI * LPFNFSVUTIL_VALIDATEFILENAME)( LPCTSTR, BOOL );

//==========================================================================
//
// BOOL FsvUtil_ValidateFullFileName( lpszFullMask, f83 )
//
//    lpszFullName - Pointer to the "file name" to validate.
//    f83 - Validate as a 8.3 filename
//
// This function validates the "file name" (it can have a path) entered by
// the user.  It checks for multiple extension separators, bad characters,
// too many characters, wild card characters, and anything else that is
// unacceptable.
//
// This function returns "TRUE" if the file name is valid and "FALSE" if it
// is bogus.
//
//
BOOL WINAPI FsvUtil_ValidateFullFileName( LPCTSTR lpszFullName, BOOL f83 );
typedef BOOL (WINAPI * LPFNFSVUTIL_VALIDATEFULLFILENAME)( LPCTSTR );

//==========================================================================
//
// BOOL FsvUtil_ValidateFileMask( lpszFileMask, f83 )
//
//    lpszFileMask - Pointer to the "file mask" to validate.
//    f83 - Validate as 8.3 file mask
//
// This function validates the "file mask" (cannot have a path) entered by
// the user.  It checks for multiple extension separators, bad characters,
// too many characters, and illegal use of wild card characters.
//
// This function returns "TRUE" if the mask is valid and "FALSE" if it
// is bogus.
//
// Note: When this function encounters an asterisk it removes any unnecessary
//       characters that follow it (i.e. "**.*NC*" ==> "*.*").  This means
//       that the input string could possibly be MODIFIED.
//

BOOL WINAPI FsvUtil_ValidateFileMask( LPTSTR lpszFileMask, BOOL f83 );
typedef BOOL (WINAPI * LPFNFSVUTIL_VALIDATEFILEMASK)( LPTSTR );

//==========================================================================
//
// BOOL FsvUtil_ValidatePathFileMask( lpszFileMask, bAllwSubstMask, f83  )
//
//    lpszFileMask    - Pointer to the "file mask" to validate.
//    bAllowSubstMask - If this flag is TRUE then allow the path to contain
//                      the "substition mask" (i.e. "*\").
//    f83 - Validate as 8.3 name
//
// This function validates the "file mask" entered by the user.  It checks
// for multiple extension separators, bad characters and other stuff like
// that.  If the mask contains a path then the path will also be validated.
//
// This function returns "TRUE" if the mask is valid and "FALSE" if it
// is bogus.
//
// Note: When this function encounters an asterisk it removes any unnecessary
//       characters that follow it (i.e. "**.*NC*" ==> "*.*").  This means
//       that the input string could possibly be MODIFIED.
//

BOOL WINAPI FsvUtil_ValidatePathFileMask( LPTSTR lpszFileMask, BOOL bAllowSubstMask, BOOL f83 );
typedef BOOL (WINAPI * LPFNFSVUTIL_VALIDATEPATHFILEMASK)( LPTSTR, BOOL, BOOL );

//==========================================================================
//
// VOID FsvUtil_CreateDestName( lpszSrcName, lpszDstMask, lpszDstName, f83 )
//
//    lpszSrcName - Pointer to a file name that was generated by the source
//                  mask.
//    lpszDstMask - Pointer to the desination mask.
//    lpszDstName - Pointer to the string that will contain the file name
//                  that is generated when the destination mask is "applied"
//                  to the source file name.
//    f83 - create using only 8.3 name
//
// This function generates a destination file name by applying the
// "destination mask" to the source file name.
//
// Notes:
//    1) The following "masks" imply that the destination name is to be the
//       same as the source name ( "*.*", "*", "", "*." ).
//    2) This function assumes that the destination mask does not contain
//       a "bogus mask" (i.e. "jon..c").
//
VOID WINAPI FsvUtil_CreateDestName( LPCTSTR lpszSrcName,LPCTSTR lpszDstMask,LPTSTR lpszDstName, BOOL f83 );
typedef VOID (WINAPI * LPFNFSVUTIL_CREATEDESTNAME)( LPCTSTR ,LPCTSTR ,LPTSTR, BOOL );

//==========================================================================
//
// BOOL FsvUtil_ContainsWildCardChs( lpszFileName )
//
//    lpszFileName - Pointer to the file name to evaluate.
//
// This function scans the specified file name for any Dos wild card
// characters.
//
// This function returns "TRUE" if any wild card characters (*, ?, etc.)
// are detected.
//
BOOL WINAPI FsvUtil_ContainsWildCardChs( LPCSTR lpszFileName );
typedef BOOL (WINAPI * LPFNFSVUTIL_CONTAINSWILDCARDCHS)( LPCSTR );

//==========================================================================
//
// BOOL FsvUtil_MaskIsAllFiles( lpszFileName )
//
//    lpszFileName - Pointer to the file name to evaluate.
//
// This function scans the specified file name to see if it contains the
// mask for "all files".
//
// This function returns "TRUE" if it contains the mask ("*.*") that
// pertains to all files.
//
BOOL WINAPI FsvUtil_MaskIsAllFiles( LPCSTR lpszFileName );
typedef BOOL (WINAPI * LPFNFSVUTIL_MASKISALLFILES)( LPCSTR );

//==========================================================================
//
// BOOL FsvUtil_DoesFileMatchMask( lpszSrcMaskIn, lpszSrcNameIn, f83 )
//
//    lpszSrcMask - The source mask that the source file name needs to
//                  "match".
//    lpszSrcName - Pointer to a file name that was generated by the source
//                  mask.
//    f83 - Treat names as 8.3 names
//
// This function checks to see if the source file name matches the source
// file mask.
//
// Notes:
//    1) This function assumes that the source mask is valid.
//    2) This function assumes that the source mask and source file name do
//       not include the path.
//    3) This function assumes that no characters (except the extension)
//       follow the asterisk.
//
BOOL WINAPI FsvUtil_DoesFileMatchMask( LPCSTR lpszSrcMask,LPCSTR lpszSrcName,BOOL f83 );
typedef BOOL (WINAPI * LPFNFSVUTIL_DOESFILEMATCHMASK)( LPCSTR ,LPCSTR, BOOL );

//==========================================================================
//
// BOOL FsvUtil_DoesFileMatchFullMask( lpszPathMask, lpszFullName, f83 )
//
//    lpszPathMask - The source mask that the source file name needs to
//                   "match".  This mask can optionally contain a partial
//                   path (i.e. JON\JON.C) or a full path (i.e. C:\JON\JON.C).
//    lpszFullName - Pointer to a file name that was generated by the source
//                   mask.
//    f83 - Treat names as 8.3 names
//
// This function checks to see if the potentially fully qualified file name
// matches the specified file mask.  If the mask contains a partial path
// (i.e. "JON\JON.C") or a full path (i.e. "C:\JON\JON.C") then the path
// portion of the specified file name must match that path.  The file name
// portion of the fully qualified file name must match the "file mask" portion
// of the specified mask (i.e. "JON.C" would match "\JON\*.C").
//
// Notes:
//    1) This function assumes that the mask is "valid".
//    2) This function assumes that no characters (except the extension)
//       follow the asterisk.
//
//==========================================================================
BOOL WINAPI FsvUtil_DoesFileMatchFullMask( LPCSTR lpszFullMask, LPCSTR lpszFullName, BOOL f83 );
typedef BOOL (WINAPI * LPFNFSVUTIL_DOESFILEMATCHFULLMASK)( LPCSTR , LPCSTR, BOOL );

//==========================================================================
//
// BYTE FsvUtil_ConvrtDrvLtr2ID( cDrvLtr )
//
//  cDrvLtr - The "drive letter" for a disk drive.
//
// This functions converts the specified character to a
// one-based "disk drive ID" (i.e. 1 = A:).
//
// This function returns a one-based disk drive ID.
//
BYTE  WINAPI FsvUtil_ConvrtDrvLtr2ID(char cDrvLtr );
typedef BYTE (WINAPI * LPFNFSVUTIL_CONVRTDRVLTR2ID)(char );

//==========================================================================
//
// char FsvUtil_ConvrtID2DrvLtr( bDrvID )
//
//  bDrvID - The one-based disk drive ID.
//
// This functions converts the specified drive ID to an upper-case drive
// letter. (i.e. 1 = A:).
//
// This function returns the upper case drive letter for the disk
// drive.
//
char  WINAPI FsvUtil_ConvrtID2DrvLtr(BYTE bDrvID );
typedef char (WINAPI * LPFNFSVUTIL_CONVRTID2DRVLTR)(BYTE );

//==========================================================================
//
// VOID FsvUtil_GetPathMask( lpszFullName, lpszPath, lpszMask, f83 )
//
//  lpszName - Pointer to a fully qualified drive/path with an optional
//             file mask.
//  lpszPath - Pointer to the string that will contain the drive and the
//             path.  This pointer can be NULL.
//  lpszMask - Pointer to the string that will contain the optional mask.
//             This pointer can be NULL.
//  f83      - Treat names as 8.3 filenames?
//
// This function extracts the drive/path and the optional path from the
// fully qualified path name.
//
//==========================================================================
VOID WINAPI FsvUtil_GetPathMask( LPCSTR lpszFullName,LPSTR lpszPath,LPSTR lpszMask, BOOL f83 );
typedef VOID (WINAPI * LPFNFSVUTIL_GETPATHMASK)( LPCSTR ,LPSTR ,LPSTR, BOOL f83 );

//==========================================================================
//
// VOID FsvUtil_AddBackSlashChk( lpszPath )
//
//  lpszPath - Pointer to the path name.
//
// This function adds a trailing "\" to the path name if one doesn't
// exist.
//
//==========================================================================
VOID WINAPI FsvUtil_AddBackSlashChk( LPSTR lpszPath );
typedef VOID (WINAPI * LPFNFSVUTIL_ADDBACKSLASHCHK)( LPSTR );

//==========================================================================
//
// FsvUtil_ExpandFullPath( lpszInPath, lpszOutPath, wMaxLen, lpCrit,
//                         dwCrit ) : long
//
//    lpszInPath  - Pointer to the path to evaluate.
//    lpszOutPath - Pointer to the path to contain the input path after
//                  it has been "expanded".
//    wMaxLen     - The maximum size of the output path.
//    lpCrit      - Pointer to the critical error handler.  This pointer can
//                  be NULL.
//    dwCrit      - Parameter to pass to the critical error handler.
//
// This function will evaluate the specified path, and if necessary, it will
// supply the default drive/or and path.  It will also process any occurrences
// of "." and "..".
//
// Returns:
//    FSV_SUCCESS               - The path was successfully expanded.
//    FSVERR_CANNOT_EXPAND_PATH - An error was encountered when trying to
//                                obtain the current drive or the current
//                                directory.  Disk error or drive not ready.
//    FSVERR_PATH_TOO_LONG      - Expanding the path caused string overflow.
//    FSVERR_INVALID_DRIVE      - The drive letter refers to a device that
//                                does not exist.
//
// Note:  Because of limititations of the support routines, the parameters
//        lpCrit and dwCrit are not used.  If a critical error is detected
//        FSVERR_DRIVE_NOT_READY will always be returned.
//
//==========================================================================

long WINAPI FsvUtil_ExpandFullPath( LPCSTR lpszInPath, LPSTR lpszOutPath, WORD wMaxLen, ERROR_CALLBACK lpfnCrit, DWORD dwCrit );
typedef long ( WINAPI* LPFSVUTIL_EXPANDFULLPATH )( LPCSTR, LPSTR, WORD, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// VOID FsvUtil_RemoveBackSlashChk( lpszPath )
//
//  lpszPath - Pointer to the path name.
//
// This function removes the trailing "\" from the path name if one exists.
// However it will not remove a trailing backslash from the "root".
//
//==========================================================================
VOID WINAPI FsvUtil_RemoveBackSlashChk( LPSTR lpszPath );
typedef VOID (WINAPI * LPFNFSVUTIL_REMOVEBACKSLASHCHK)( LPSTR );

//==========================================================================
//
// WORD FsvUtil_GetExt( lpszFileName, lpszExt, wMaxChs )
//
//    lpszFileName - Pointer to the file name to extract the extension from.
//    lpszExt      - Pointer to string that will contain the extension.
//    wMaxChs      - The maximum size (including the NULL terminator) of
//                   *lpszExt.
//
// This function will extract the extension contained in *lpszFileName.  If
// there is no extension then an empty string will be returned.
//
// This function returns the length of the extension extracted.  A value
// of zero implies that there is no extension or an error was encountered.
//
//==========================================================================

WORD WINAPI FsvUtil_GetExt(
   LPCSTR lpszFileName,
   LPSTR  lpszExt,
   WORD   wMaxChs
);
typedef WORD ( WINAPI* LPFNFSVUTIL_GETEXT )( LPCSTR, LPSTR, WORD );

//==========================================================================
//
// LPSTR FsvUtil_AddExtChk( lpszFileName, lpszExt, wMaxChs )
//
//    lpszFileName - Pointer to the file name to evaluate and possibly modify.
//    lpszExt      - Pointer to the default file name extension.
//    wMaxChs      - The maximum size (including the NULL terminator) of
//                   *lpszFileName.
//
// This function will add the default file name extension (*lpszExt) to
// the specified file (*lpszFileName) if it does not have an extension.
//
// This function returns the pointer to lpszFileName.
//
//==========================================================================

LPSTR WINAPI FsvUtil_AddExtChk(
   LPSTR  lpszFileName,
   LPCSTR lpszExt,
   WORD   wMaxChs
);
typedef LPSTR ( WINAPI* LPFNFSVUTIL_ADDEXTCHK )( LPSTR, LPCSTR, WORD );

//==========================================================================
//
// WORD FsvUtil_RemoveLeadingTrailingChs( lpszStr, cCh )
//
//    lpszStr - Pointer to the string to process.
//    cCh     - The character to remove from the beginning and/or end of
//              the string in question.
//
// This function removes occurrences of the specified character from
// beginning and/or end of the specified string.
//
// This function returns the length of the string after any leading/trailing
// characters have been removed.
//
//==========================================================================

WORD WINAPI FsvUtil_RemoveLeadingTrailingChs(
   LPSTR lpszStr,
   char  cCh
);
typedef WORD ( WINAPI* LPFNFSVUTIL_REMOVELEADINGTRAILINGCHS )( LPSTR, char );

//==========================================================================
//
// WORD FsvUtil_ChckBadChar( lpsz )
//
//  lpsz - Pointer to a string to check for illegal characters.
//
// This function checks the given string.  It must be upper-case and OEM.
//
// Return value is 0 if no bad characters are found.
//
//==========================================================================
WORD WINAPI FsvUtil_ChckBadChar( LPSTR lpsz, BOOL f83 );
typedef WORD (WINAPI * LPFNFSVUTIL_CHCKBADCHAR)( LPSTR, BOOL );

//==========================================================================
//
// WORD FsvUtil_ChckBadLabelChar( lpsz )
//
//  lpsz - Pointer to a string to check for illegal characters.
//
// This function checks the given string.  It must be upper-case and OEM.
//
// Return value is 0 if no bad characters are found.
//
//==========================================================================
WORD WINAPI FsvUtil_ChckBadLabelChar( LPSTR lpsz );
typedef WORD (WINAPI * LPFNFSVUTIL_CHCKBADLABELCHAR)( LPSTR );

//==========================================================================
//
// FsvUtil_CreateMultipleDirs( lpszDirName, lpCrit, dwCrit ) : LONG
//
//    lpszDirName - Pointer to the fully qualified source name for the file
//                  operation.
//    lpCrit      - Callback pointer to the critical error handler.  Can be
//                  NULL.
//    dwCrit      - Pointer to the critical error structure.  Can be NULL.
//
// This function is called when it has been determined that a path
// specified by the operator doesn't exist.  This function will determine
// which portion of the path does exist and then create all of the subsequent
// sub-directories that do not exist.
//
// Notes:  1) It is assumed that the directory contains a drive and a
//            "path".
//
LONG WINAPI FsvUtil_CreateMultipleDirs( LPSTR lpszDirName, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef LONG (WINAPI * LPFNFSVUTIL_CREATEMULTIPLEDIRS)( LPSTR, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// FsvUtil_SetTimer( hwnd, idTimer, uTimeOut, tmprc ) : UINT
//
//    hwnd        - handle of window for timer messages
//    idTimer     - timer identifier (non-Zero)
//    uTimeOut    - time-out duration in milliseconds
//    tmprc       - procedure instance address of the callback function
//
// This function is a replacement for the Windows SetTimer function. The
// parameters are identical. The only difference is that this function will
// share a common timer amongst all users - thus saving on timer resources.
//
// Return:  Id of timer if hwnd is NULL and function is successful. The return
//          value is idTimer if hwnd is non-NULL and the function is successful.
//          The return value is zero if the fails.
//
UINT WINAPI FsvUtil_SetTimer(HWND hwnd, UINT idTimer,UINT uTimeOut,TIMERPROC tmprc);
typedef UINT (WINAPI * LPFNFSVUTIL_SETTIMER)(HWND , UINT ,UINT ,TIMERPROC );

//==========================================================================
//
// FsvUtil_KillTimer( hwnd, idTimer ) : BOOL
//
//    hwnd        - handle of window for timer messages
//    idTimer     - timer identifier (non-Zero)
//
// This function is a replacement for the Windows KillTimer function. The
// parameters are identical. This function should be called to "kill" timers
// created with the FsvUtil_SetTime function.
//
// Return:  non-NULL if successful, 0 if unsuccessful.
//
BOOL WINAPI FsvUtil_KillTimer(HWND hwnd, UINT idTimer);
typedef BOOL (WINAPI * LPFNFSVUTIL_KILLTIMER)(HWND , UINT );

//============================================================================
// FsvUtil_GetTempFileName( bDrive, lpszPrefix, uUnique, lpszBuffer, dwEstSize) : long
//
//    bDrive         suggested drive ('A','B', etc)
//    lpszPrefix     prefix to use for filename (may be NULL)
//    uUnique        number to use as prefix
//    lpszBuffer     output buffer for filename
//    dwEstSize      estimated size of the file
//
// This function will return a temp file name. It is identical to the Windows
// API call GetTempFileName, with the exception of the estimated size field.
//
long WINAPI FsvUtil_GetTempFileName(BYTE bDrive,LPCSTR lpszPrefix,UINT uUnique,LPSTR lpszBuffer,DWORD dwEstSize);
typedef long (WINAPI * LPFNFSVUTIL_GETTEMPFILENAME)(BYTE,LPCSTR,UINT,LPSTR,DWORD);

//============================================================================
// FsvUtil_GetBkTaskHWND() : HWND;
//
//
// This function returns the window handle of the FileServices background
// Task, if it exists.  If it does not, then the return value is NULL.
//
//
HWND WINAPI FsvUtil_GetBkTaskHWND();
typedef HWND (WINAPI * LPFNFSVUTIL_GETBKTASKHWND)();

/**********/

//===========================================================================
//
// FsvCache_InvalidatePath( LPSTR lpszPath, BOOL fRecurse ) : long;
//
//    lpszPath    Fully qualified path to invalidate
//    fRecurse    TRUE = invalidate recursively, FALSE = invalidate one level
//
// Invalidates a path in the cache system. fRecurse determines whether the
// flush contains all subdirectories.
//
// Returns: FSV_SUCCESS if successful, FSV_ERROR if not
//
long WINAPI FsvCache_InvalidatePath(LPSTR lpszPath, BOOL fRecurse);
typedef long (WINAPI * LPFNFSVCACHE_INVALIDATEPATH)(LPSTR , BOOL );

//===========================================================================
//
// FsvCache_SetCaching( BYTE bDriveId, BOOL fEnable ) : long;
//
//    bDriveId    1-based drive id
//    fEnable     TRUE = turn caching on, FALSE = turn caching off
//
// This function will enable/disable caching for a particular drive.
//
// Returns: FSV_SUCCESS if successful, FSV_ERROR if not
//
long WINAPI FsvCache_SetCaching(BYTE bDriveId,BOOL fEnable);
typedef long (WINAPI * LPFNFSVCACHE_SETCACHING)(BYTE ,BOOL );

//===========================================================================
//
// FsvCache_SetCachingAll( dwCacheBits ): long;
//
//    dwCacheBits    The binary positions determine which drives get cached.
//
// This function will blast out the cache settings.
//
// Returns: FSV_SUCCESS if successful, FSV_ERROR if not
//
long WINAPI FsvCache_SetCachingAll( DWORD dwCacheBits );
typedef long (WINAPI * LPFNFSVCACHE_SETCACHINGALL)( DWORD );

//===========================================================================
//
// FsvCache_EnumerateDirEntries( lpPath, lpFileSpec, wOptions,
//                               lpEnumDirEntryCallback, dwParam) : long;
//
//    lpPath            Full path to be enumerated.
//    lpFileSpec        File spec to be enumerated (NULL indicates *.*).
//    lphUE             Pointer to UnErase object to allocate or use.  If
//                      content of pointer is NULL, new object will be
//                      allocated.  If not NULL, then it must point to a
//                      previously allocated object.  If the pointer itself
//                      is NULL, this function is unable to enumerate
//                      deleted objects.
//    wOptions          Option Flags (FSV_ENUMOPTION_???)
//    lpEnumDirEntryCallback  Callback function to be called with each enumeration.
//    dwParam           Application supplied parameter to be passed to the
//                      callback function.
//
// This function will enumerate the specified dir entries within a given
// path. If the callback function returns FSV_ENUM_ABORT, then the enumeration
// process is aborted. If the FSV_ENUMOPTION_POPUPNOTIFY flag is set, then the
// callback function is called with a NULL DIRENTRYSTRUCT once for each popup.
// If FSV_ENUMOPTION_ZIPS is specified, then a ZIP/ARC/PAK file does not appear
// as a file - only as a directory with the FSV_ATTRIB_ZIP flag set.
//
// Returns: Number of Files "Added" to the count, or FSVERR_??? if an error occurs.
//
long WINAPI FsvCache_EnumerateDirEntries(LPSTR lpPath,LPSTR lpFileSpec,LPVOID *,WORD wOptions, LPFSV_ENUMDIRENTRYCALLBACK lpCallback, DWORD dwParam);
typedef long (WINAPI * LPFNFSVCACHE_ENUMERATEDIRENTRIES)(LPSTR ,LPSTR ,LPVOID *, WORD , LPFSV_ENUMDIRENTRYCALLBACK , DWORD );

/**********/

//========================================================================
//
//       WORD WINAPI FsvDOSWrap_440D61_ReadTrack(
//               WORD                    wLogicalDrive,
//               RWBLOCK far * lpRw
//               );
//
//       Returns 0 if no problem, usual dos error flags if a problem,
//
//       typedef struct tagRWBLOCK{            /*rw*/
//               BYTE    bPadding;
//               BYTE    bSpecFunc;      /* MUST be zero    */
//               WORD    wHead;          /* usually 0 or 1  */
//               WORD    wCylinder;      /* usually 0 to 79 */
//               WORD    wFirstSector;   /* Usually zero    */
//               WORD    wSectors;     /* Usually same as wSecPerTrack*/
//               LPSTR   abBuffer;
//               }       RWBLOCK;
//
//       This returns an error code. Zero is success, non zero is on failure.
//       Error code is uninformative
//
//       Use FsvDOSWrap_59_ExtendedError() for a more meaningfull error code.
//
//       This function, unlike the WNABSIO function, does not flush the
//       buffers or lock out access to DOS before executing. Depending on your
//       application you may wish to call FsvIO_BeginCriticalSection followed
//       by FsvIO_FlushBuffers before you start directly accessing the disk.
//       If you do you MUST call FsvIO_EndCriticalSection after you have
//       finished directly accessing the disk.
//
//       I do not recommend this for most tasks, but other believe in it
//       always. Obviously the people who wrote WNABSIO do. My feeling is
//       that the only way this will screw up is if somebody does an XCOPY c:
//       a: in a background DOS box, and then commands a disk copy while the
//       XCOPY is still running. If somebody does this, then more fool he.
//
//       Furthermore none of this begin critical section stuff can protect
//       against the common case of somebody running two disk apps at once in
//       windows. BeginCriticalSection will only protect against other DOS
//       boxes, not other windows apps. Still, some people swear by this
//       stuff. I say windows just is not a real multi tasking system, and it
//       is not a big problem because you only have one user at a time. 
//
WORD WINAPI FsvDOSWrap_440D61_ReadTrack(  WORD wLogicalDrive,  RWBLOCK far * lpRw );
typedef WORD (WINAPI * LPFNFSVDOSWRAP_440D61_READTRACK)(  WORD ,  RWBLOCK far * );

//========================================================================
//
//       WORD WINAPI FsvDOSWrap_440D41_WriteTrack(                      
//               WORD                    wLogicalDrive,                  
//               RWBLOCK far * lpRw                                      
//               );                                                      
//                                                                       
//       Returns 0 if no problem, usual dos error flags if a problem,    
//                                                                       
//       typedef struct tagRWBLOCK{            /*rw*/                    
//               BYTE    bPadding;                                       
//               BYTE    bSpecFunc;      /* MUST be zero    */           
//               WORD    wHead;          /* usually 0 or 1  */           
//               WORD    wCylinder;      /* usually 0 to 79 */           
//               WORD    wFirstSector;   /* Usually zero    */           
//               WORD    wSectors;       /* Usually same as wSecPerTrack */
//               LPSTR   abBuffer;                                       
//               }       RWBLOCK;                                        
//
WORD WINAPI FsvDOSWrap_440D41_WriteTrack( WORD wLogicalDrive, RWBLOCK   far * lpRw );
typedef WORD (WINAPI * LPFNFSVDOSWRAP_440D41_WRITETRACK)( WORD , RWBLOCK   far * );

//========================================================================
//
//       WORD WINAPI FsvDOSWrap_3305_StartupDrive ( void )
//
WORD WINAPI FsvDOSWrap_3305_StartupDrive ( void );
typedef WORD (WINAPI * LPFNFSVDOSWRAP_3305_STARTUPDRIVE )( void );

//==========================================================================
//
// LONG FsvIO_GetUsableClusters( lpFrmtInfo )
//
//    lpFrmtInfo - Pointer to a File Services structure that contains
//                 low level drive information.
//
// This function returns the number of clusters on the drive that are
// can be used to contain data.
//
//==========================================================================

LONG WINAPI FsvIO_GetUsableClusters( LPCFSVIO_FORMATSTRUCT lpFrmtInfo );
typedef LONG ( WINAPI* LPFSVIO_GETUSABLECLUSTERS )( LPCFSVIO_FORMATSTRUCT lpFrmtInfo );


//==========================================================================
//
// BOOL FsvIO_Is12BitFat( lpFrmtInfo )
//
//    lpFrmtInfo - Pointer to a File Services structure that contains
//                 low level drive information.
//
// This function determines if the FAT affiliated with the specified
// drive is 12 bit or 16 bit.
//
// Returns:
//    FALSE - The FAT is 16 bit.
//    TRUE  - The FAT is 12 bit.
//
//==========================================================================

BOOL WINAPI FsvIO_Is12BitFat( LPCFSVIO_FORMATSTRUCT lpFrmtInfo );
typedef BOOL ( WINAPI* LPFSVIO_IS12BITFAT )( LPCFSVIO_FORMATSTRUCT );


//==========================================================================
//
// WORD FsvIO_MaxFATEntry( lpDskInfo )
//
//   lpDskInfo - Pointer to a File Services structure that contains
//               low level drive information.
//
// This function returns the index of the maximum valid entry in the FAT.
//
//==========================================================================

WORD WINAPI FsvIO_MaxFATEntry( LPCFSVIO_FORMATSTRUCT lpDskInfo );
typedef WORD ( WINAPI* LPFSVIO_MAXFATENTRY )( LPCFSVIO_FORMATSTRUCT );


//==========================================================================
//
//  BOOL FsvIO_IsValidCluster( lpDskInfo, wClustrNum )
//
//    lpDskInfo - Pointer to a File Services structure that contains
//                 low level drive information.
//    wClustrNum - The cluster number to evaluate.
//
// This function validates the cluster number.
//
// Returns:
//    FALSE - Bad news.
//    TRUE  - The cluster number is valid.
//
//==========================================================================

BOOL WINAPI FsvIO_IsValidCluster( LPCFSVIO_FORMATSTRUCT lpDskInfo, WORD wClustrNum );
typedef BOOL ( WINAPI* LPFSVIO_ISVALIDCLUSTER )( LPCFSVIO_FORMATSTRUCT, WORD );


//==========================================================================
//
// LONG FsvIO_GetFATEntry( lpDskInfo, hpbFAT, wClustrNum )
//
//    lpDskInfo  - Pointer to a File Services structure that contains
//                 low level drive information.
//    lphFAT     - Pointer to the buffer containing the FAT.
//    wClustrNum - The cluster number to return FAT information for.
//
// This function returns the FAT information that pertains to the specified
// cluster number.
//
// This function will return the affiliate FAT info if it is successful or
// FSVERR_ERROR if the cluster number is invalid.
//
// Notes:
//    1) If the FAT for the specified drive is a 12-bit FAT and the high
//       byte is "F" then... This is ...
//
//==========================================================================

LONG WINAPI FsvIO_GetFATEntry( LPCFSVIO_FORMATSTRUCT lpDskInfo, HPCBYTE hpbFAT, WORD wClustrNum );
typedef LONG ( WINAPI* LPFSVIO_GETFATENTRY )( LPCFSVIO_FORMATSTRUCT, HPCBYTE, WORD );


//==========================================================================
//
// LONG FsvIO_SetFATEntry( lpDskInfo, hpbFAT, wClustrNum, wFATEntry )
//
//    lpDskInfo  - Pointer to a File Services structure that contains
//                 low level drive information.
//    lphFAT     - Pointer to the buffer containing the FAT.
//    wClustrNum - The cluster number in the FAT to set.
//    wFATEntry  - The data to put in the FAT for wClustrNum.
//
// This function sets the FAT entry that corresponds to wClustrNum with the
// specified value.  This function works for a 12 or 16 bit FAT.
//
// This function will return FSV_SUCCESS if it is successful or FSVERR_ERROR
// if the cluster number is invalid.
//
//==========================================================================

LONG WINAPI FsvIO_SetFATEntry( LPCFSVIO_FORMATSTRUCT lpDskInfo, HPBYTE hpbFAT, WORD wClustrNum, WORD wFATEntry );
typedef LONG ( WINAPI* LPFSVIO_SETFATENTRY )( LPCFSVIO_FORMATSTRUCT, HPBYTE, WORD, WORD );


//==========================================================================
//
// LONG FsvUtil_DosVersnComp( bMajor, bMinor )
//
//    bMajor - The major part of the version number.
//    bMinor - The minor part of the version number.
//
// Globals:
//    gFsv   - Global data for the File Services DLL.
//
// This macro compares the specified DOS version with the actual DOS
// version.
//
// Returns:
//    -1 - Specified version less than actual Dos version.
//     0 - Specified version equal to actual Dos version.
//     1 - Specified version greater than actual Dos version.
//

#define FsvUtil_DosVersnComp(bMajor, bMinor)\
   ( int ) ( gFsv.osversion.b[ 0 ]  > ( BYTE ) ( bMajor ) || ( gFsv.osversion.b[ 0 ] == ( BYTE ) ( bMajor ) && gFsv.osversion.b[ 1 ] > ( BYTE ) ( bMinor ) ) ) ? - 1 :\
           ( gFsv.osversion.b[ 0 ] == ( BYTE ) ( bMajor ) && gFsv.osversion.b[ 1 ] == ( BYTE ) ( bMinor ) ) ? 0 : 1

//==========================================================================
//
// BYTE FsvIO_FSVDrvID2NovellDrvID( bDrv )
//
//    bDrv - The File Srvices (one-based) drive ID.
//
// This macro converts a one-based File Services drive ID to a Novell
// (zero-based) drive ID.
//

#define FsvIO_FSVDrvID2NovellDrvID(bDrv) ( BYTE ) ( bDrv - 1 )

//==========================================================================
//
// BYTE FsvIO_FSVDrvID2NovellDrvID( bDrv )
//
//    bDrv - The File Srvices (one-based) drive ID.
//
// This macro checks to see if the cluster value is a EOF cluster.
//
// Returns:
//    FALSE - The cluster number does not indicate the end of file.
//    TRUE  - The cluster number is the EOF value.
//

#define FsvIO_IsEOFCluster(wClstr) ( BOOL ) ( ( WORD ) ( wClstr ) >= FSV_CLSTR_EOF_LOW && ( WORD ) ( wClstr ) <= FSV_CLSTR_EOF_HIGH )

//===========================================================================
//
// FsvUtil_GetOwnerNames( void ) : LPOWNERID;
//
// This routine determines the names and IDs of the owners on the network
//
// Returns: Pointer to a list of OWNERID sturctures
//
LPOWNERID WINAPI FsvUtil_GetOwnerNames(void);
typedef LPOWNERID (WINAPI * LPFNFSVUTIL_GETOWNERNAMES)(void);

//===========================================================================
//
// FsvUtil_GetOwnerNameFromID( dwUserID, lpszName ) : LONG;
//
// This routine determines the names  of the owner on the network from the ID
//
// Returns: FSV_SUCCESS / FSV_ERROR
//
LONG WINAPI FsvUtil_GetOwnerNameFromID(DWORD dwUserID, LPSTR lpszName);
typedef LONG (WINAPI * LPFNFSVUTIL_GETOWNERNAMEFROMID)(DWORD, LPSTR);

//==========================================================================
//
// LPFSVIO_DOSDIRENTRY FsvUtil_AllocCopyDirEntryStruct( lpDirEntryStruct )
//
//    lpDirEntryStruct - The pointer to the File Services directory record
//                       to copy.
//
// This function will allocate the appropriate amount of memory required by
// the specified directory structure (the structure can have pointers to
// additional structures) and will copy all pertinent info and fix up the
// additional structure pointers.
//
// This function returns a valid pointer if it was successful or NULL if
// a memory allocation error was encountered.
//
//==========================================================================

LPFSVIO_DIRENTRYSTRUCT WINAPI FsvUtil_AllocCopyDirEntryStruct(
   LPCFSVIO_DIRENTRYSTRUCT lpDirEntryStruct
);
typedef LPFSVIO_DIRENTRYSTRUCT ( WINAPI* LPFNFSVUTIL_ALLOCCOPYDIRENTRYSTRUCT )( LPCFSVIO_DIRENTRYSTRUCT );

//==========================================================================
//
// VOID FsvUtil_FreeDirEntryStruct( lpDirEntryStruct )
//
//    lpDirEntryStruct - The pointer to the File Services directory record
//                       to free.
//
// This function will free any sub-structures that are contained in the
// directory structure then it will free the directory structure itself.
//
// Note: You can only call this function if lpDirEntryStruct was created
//       by calling FsvUtil_AllocCopyDirEntryStruct().  If that wasn't the
//       case then evil things will happen.
//
//==========================================================================

VOID WINAPI FsvUtil_FreeDirEntryStruct(
   LPFSVIO_DIRENTRYSTRUCT lpDirEntryStruct
);
typedef VOID ( WINAPI* LPFNFSVUTIL_FREEDIRENTRYSTRUCT )( LPFSVIO_DIRENTRYSTRUCT );

//
// DIFF structure
//

typedef struct tag_DIFF
{
   long lErrorReturn;
   int width, height;
   char pad[1];
} DIFF, far * LPDIFF;

//============================================================================
// FsvIO_DiffTextFiles: LPDIFF
// Given a pair of files, creates a data object describing the differences
// 
LPDIFF WINAPI FsvIO_DiffTextFiles(LPCSTR, LPCSTR);

//============================================================================
// FsvIO_DestroyDiffTextFiles: LPDIFF
//    Destroys the data object describing the differences
//    between two text files.
//
// Returns an error value, normally zero.
// 
long WINAPI FsvIO_DestroyDiffTextFiles(LPDIFF);

//==========================================================================
//
// LONG FsvBufIO_fOpen( lpszName, nMode, wBufSiz, lplpFBuf, lpfnCritErrFunc,
//                      dwCritErrData )
//
//    lpszName        - The name of the file to open.
//    nMode           - The access privledges to be to given to the opened
//                      file (allow others to read, deny other write access,
//                      etc.).  The values passed in are the OF_... and the
//                      OF_SHARE_... constants that are used with the
//                      Windows _lopen() function.
//    wBufSiz         - The size of the buffer to allocate for buffered file
//                      i/o.  A value of zero implies that the i/o will NOT
//                      be buffered.
//    lplpFBuf        - Pointer to the pointer that will point to the file
//                      buffer info structure in the "open" is successful.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function tries to open the file in question and then allocate the
// file buffer info structure and an affiliate i/o buffer.
//
// Returns:
//    FSV_SUCCESS          - The operation was successful.
//    FSVERR_OUT_OF_MEMORY - Not enough memory for the dynamic data structures.
//    FSVERR_...           - The appropriate error code returned by File
//                           Services.
//
//==========================================================================

LONG WINAPI FsvBufIO_fOpen(
   LPCSTR                 lpszName,
   int                    nMode,
   WORD                   wBufSiz,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   ERROR_CALLBACK         lpfnCritErrFunc,
   DWORD                  dwCritErrData
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FOPEN )( LPCSTR, int, WORD, LPLPFSVBUFIO_FBUF_INFO, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// LONG FsvBufIO_fCreat( lpszName, nAttr, wBufSiz, lplpFBuf, lpfnCritErrFunc,
//                       dwCritErrData )
//
//    lpszName        - The name of the file to open.
//    nAttr           - The file attribute(s) for the file to create (hidden,
//                      system, etc.).
//    wBufSiz         - The size of the buffer to allocate for buffered file
//                      i/o.  A value of zero implies that the i/o will NOT
//                      be buffered.
//    lplpFBuf        - Pointer to the pointer that will point to the file
//                      buffer info structure in the "open" is successful.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function tries to create the file in question and then allocate the
// file buffer info structure and an affiliate i/o buffer.
//
// Returns:
//    FSV_SUCCESS          - The operation was successful.
//    FSVERR_OUT_OF_MEMORY - Not enough memory for the dynamic data structures.
//    FSVERR_...           - The appropriate error code returned by File
//                           Services.
//
//==========================================================================

LONG WINAPI FsvBufIO_fCreat(
   LPCSTR                 lpszName,
   int                    nAttr,
   WORD                   wBufSiz,
   LPLPFSVBUFIO_FBUF_INFO lplpFBuf,
   ERROR_CALLBACK         lpfnCritErrFunc,
   DWORD                  dwCritErrData
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FCREAT )( LPCSTR, int, WORD, LPLPFSVBUFIO_FBUF_INFO, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// VOID FsvBufIO_fClose( lpFBuf, bFlushChk, lpfnCritErrFunc, dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    bFlushChk       - If this flag is TRUE then the buffer affiliated with
//                      the file will be written if it has been modified.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function tries to write out any "dirty" data in the i/o buffer
// and close the file in question.  It will also free the i/o buffer and
// the file buffer info structure.
//
// Returns:
//    FSV_SUCCESS - The close was successful.
//    FSVERR_...  - The appropriate error code returned by File Services
//                  (not good).
//
//==========================================================================

LONG WINAPI FsvBufIO_fClose(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   BOOL                 bFlushChk,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FCLOSE )( LPFSVBUFIO_FBUF_INFO, BOOL, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// LONG FsvBufIO_fFlush( lpFBuf, lpfnCritErrFunc, dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//                      This pointer will be NULL if the ...
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will write out the i/o buffer if it has been "dirtied" and
// it will move the DOS file position to the 1st byte after the end of the
// current buffer.
//
// Returns:
//    FSV_SUCCESS - The operation was successful.
//    FSVERR_...  - The appropriate error code returned by File Services.
//
//==========================================================================

LONG WINAPI FsvBufIO_fFlush(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData
);
typedef LONG ( WINAPI* LPDNFSVBUFIO_FFLUSH )( LPFSVBUFIO_FBUF_INFO, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// LONG FsvBufIO_fSetEOF( lpFBuf, lEOFPostn, lpfnCritErrFunc, dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lEOFPostn       - The zero-based file position to make the End of File.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will try to set the end of file to the position specified
// (an index position of 5 will make the file 5 bytes long).  It will only
// update the current file position to the new end of file position, if the
// new end of file position is less the old current file position.
//
// Returns:
//    FSV_SUCCESS - The operation was successful.
//    FSVERR_...  - The appropriate error code returned by File Services.
//
//==========================================================================

LONG WINAPI FsvBufIO_fSetEOF(
   LPFSVBUFIO_FBUF_INFO  lpFBuf,
   LONG                  lEOFPostn,
   ERROR_CALLBACK        lpfnCritErrFunc,
   DWORD                 dwCritErrData
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FSETEOF )( LPFSVBUFIO_FBUF_INFO, LONG, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// LONG FsvBufIO_fRead( lpFBuf, lpBuf, wBytsToRead, lpfnCritErrFunc,
//                      dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lpBuf           - Pointer to the buffer that will contain the data
//                      copied from the i/o buffer and/or disk file.
//    wBytsToRead     - The number of bytes to read.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will copy the number of bytes requested from the i/o buffer
// and/or the disk file.
//
// This function will return the number of bytes read from the i/o buffer
// and/or the disk file.  If the number returned is not equal to the number
// requested but greater than or equal to zero then the end of file was
// detected.  If the value returned is less than zero, it contains the
// File Services error code of the error that was encountered.
//
//==========================================================================

LONG WINAPI FsvBufIO_fRead(
   LPFSVBUFIO_FBUF_INFO  lpFBuf,
   LPVOID                lpBuf,
   WORD                  wBytsToRead,
   ERROR_CALLBACK        lpfnCritErrFunc,
   DWORD                 dwCritErrData
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FREAD )( LPFSVBUFIO_FBUF_INFO, LPVOID, WORD, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// LONG FsvBufIO_fReadByts( lpFBuf, lpBuf, wBytsToRead, lpfnCritErrFunc,
//                          dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lpBuf           - Pointer to the buffer that will contain the data
//                      copied from the i/o buffer and/or disk file.
//    wBytsToRead     - The number of bytes to read.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function calls FsvBufIO_fRead() to read the data in question.  This
// function will return an error if not all of the requested bytes were
// read.
//
// Returns:
//    FSV_SUCCESS - The operation was successful.
//    FSV_ERROR   - The end of file was prematurely detected which means
//                  the archive file is in an unstable state.
//    FSVERR...   - The appropriate error code returned by File Services
//                  ( i.e. file not found, disk i/o error, etc.)
//
//==========================================================================

LONG WINAPI FsvBufIO_fReadByts(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LPVOID               lpBuf,
   WORD                 wBytsToRead,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FREADBYTS )( LPFSVBUFIO_FBUF_INFO, LPVOID, WORD, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// LONG FsvBufIO_fWrite( lpFBuf, lpBuf, wBytsToWrt, lpfnCritErrFunc,
//                       dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lpBuf           - Pointer to the buffer that contains the data to copy
//                      to the i/o buffer and/or disk file.
//    wBytsToWrt      - The number of bytes to write.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will write the specified bytes to the i/o buffer and/or the
// disk file.
//
// This function will return the number of bytes written to the i/o buffer
// and/or the disk file.  If the value returned is less than zero, it contains
// the File Services error code of the error that was encountered.
//
//==========================================================================

LONG WINAPI FsvBufIO_fWrite(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LPCVOID              lpBuf,
   WORD                 wBytsToWrt,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FWRITE )( LPFSVBUFIO_FBUF_INFO, LPCVOID, WORD, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// LONG FsvBufIO_fWriteByts( lpFBuf, lpBuf, wBytsToWrt, lpfnCritErrFunc,
//                           dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lpBuf           - Pointer to the buffer that contains the data to copy
//                      to the i/o buffer and/or disk file.
//    wBytsToWrt      - The number of bytes to write.
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function calls FsvBufIO_fWrite() to write the data in question.  This
// function will return an error if not all of the requested bytes were
// written.
//
//==========================================================================

LONG WINAPI FsvBufIO_fWriteByts(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LPCVOID              lpBuf,
   WORD                 wBytsToWrt,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FWRITEBYTS )( LPFSVBUFIO_FBUF_INFO, LPCVOID, WORD, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// LONG FsvBufIO_fTell( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns the logical file position.
//
//==========================================================================

LONG WINAPI FsvBufIO_fTell(
   LPCFSVBUFIO_FBUF_INFO lpFBuf
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FTELL )( LPCFSVBUFIO_FBUF_INFO );

//==========================================================================
//
// LONG FsvBufIO_fSeek( lpFBuf, lOffset, nSeekOp, lpfnCritErrFunc,
//                      dwCritErrData )
//
//    lpFBuf          - Pointer to the structure used to buffer file i/o.
//    lOffset         - The offset to add to the beggining of file, current
//                      file position, or the end of the file.
//    nSeekOp         - The type of seek operation to perform (seek from
//                      beginning of file, seek from current file position,
//                      seek to the end of the file).
//    lpfnCritErrFunc - Pointer to the function to call if a critical error
//                      is detected.  This pointer can be NULL if the caller
//                      wants to ignore critical errors.
//    dwCritErrData   - The data that is to be passed to the critical error
//                      routine (usually a structure pointer).
//
// This function will update the logical file position.
//
// This function will return the new logical file position if successful,
// or the appropriate File Services error code if it failed.
//
//==========================================================================

LONG WINAPI FsvBufIO_fSeek(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LONG                 lOffset,
   int                  nSeekOp,
   ERROR_CALLBACK       lpfnCritErrFunc,
   DWORD                dwCritErrData
);
typedef LONG ( WINAPI* LPFNFSVBUFIO_FSEEK )( LPFSVBUFIO_FBUF_INFO, LONG, int, ERROR_CALLBACK, DWORD );

//==========================================================================
//
// BOOL FsvBufIO_fSeekCausesFlush( lpFBuf, lOffset, nSeekOp )
//
//    lpFBuf  - Pointer to the structure used to buffer file i/o.
//    lOffset - The offset to add to the beggining of file, current file
//              position, or the end of the file.
//    nSeekOp - The type of seek operation to perform (seek from beginning
//              of file, seek from current file position, seek to the end
//              of the file).
//
// This function checks to see if the "seek" will cause the i/o buffer to
// be flushed.
//
// Returns:
//    FALSE - I/O buffer will NOT be flushed.
//    TRUE  - I/O buffer WILL be flushed.
//
//==========================================================================

BOOL WINAPI FsvBufIO_fSeekCausesFlush(
   LPFSVBUFIO_FBUF_INFO lpFBuf,
   LONG                 lOffset,
   int                  nSeekOp
);
typedef BOOL ( WINAPI* LPFNFSVBUFIO_FSEEKCAUSESFLUSH )( LPFSVBUFIO_FBUF_INFO, LONG, int );

//==========================================================================
//
// LONG FsvBufIO_fRtrnEOFPos( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns the file position one byte beyond the current
// end of file.
//
//==========================================================================

#define FsvBufIO_fRtrnEOFPos(lpFBuf) ( ( ( lpFBuf ) == NULL ) ? 0 : ( ( *lpFBuf ).lEOFPos ) )

//==========================================================================
//
// BOOL FsvBufIO_fAtEOF( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns TRUE if the specified file position is at or beyond
// the current end of file.  Otherwise it returns FALSE.
//
// Notes:
//    1) You must use FsvBufIO_fAtEOF() if you are using FsvBufIO_fReadLine().
//
//==========================================================================

// Is pointer file at end of file?
#define FsvBufIO_fAtEOF(lpFBuf) ( BOOL ) ( ( *lpFBuf ).lStrtBufPos + ( *lpFBuf ).wBufOffset >= ( *lpFBuf ).lEOFPos )

//==========================================================================
//
// BOOL FsvBufIO_fAtAsciiEOF( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns TRUE if the specified file position is at or beyond
// the current end of the ASCII file.  Otherwise it returns FALSE.
//
// Notes:
//    1) You must call this function if you are using FsvBufIO_fReadLine().
//
//==========================================================================

#define FsvBufIO_fAtAsciiEOF(lpFBuf) ( BOOL ) ( ( *lpFBuf ).lStrtBufPos + ( *lpFBuf ).wBufOffset >= min( ( *lpFBuf ).lEOFPos, ( *lpFBuf ).lEOFCharPos ) )

//==========================================================================
//
// WORD FsvBufIO_BufBytsAvail( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns the number of bytes that can be added to the i/o
// buffer before it has to be written to disk.
//
//==========================================================================

#define FsvBufIO_BufBytsAvail(lpFBuf) ( ( ( lpFBuf ) == NULL ) ? 0 : ( ( *lpFBuf ).wBufSiz - ( *lpFBuf ).wBufOffset ) )

//==========================================================================
//
// LPBYTE FsvBufIO_GetIOBufPtr( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns the pointer to the i/o buffer.
//
//==========================================================================

#define FsvBufIO_GetIOBufPtr(lpFBuf) ( ( ( lpFBuf ) == NULL ) ? NULL : ( *lpFBuf ).lpBuf )

//==========================================================================
//
// WORD FsvBufIO_GetIOBufSiz( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns the size of the i/o buffer.
//
//==========================================================================

#define FsvBufIO_GetIOBufSiz(lpFBuf) ( ( ( lpFBuf ) == NULL ) ? 0 : ( *lpFBuf ).wBufSiz )

//==========================================================================
//
// WORD FsvBufIO_GetIOBufBytsUsed( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns the number of bytes that are "used" in the
// i/o buffer.
//
//==========================================================================

#define FsvBufIO_GetIOBufBytsUsed(lpFBuf) ( ( ( lpFBuf ) == NULL ) ? 0 : ( *lpFBuf ).wBytsUsed )

//==========================================================================
//
// VOID FsvBufIO_GetDirtyFlag( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns the "dirty flag" for the i/o buffer.
//
// Returns:
//    FALSE - The i/o buffer has NOT been modified.
//    TRUE  - The i/o buffer has BEEN modified.
//
//==========================================================================

#define FsvBufIO_GetDirtyFlag(lpFBuf) ( BOOL ) ( ( ( lpFBuf ) == NULL ) ? FALSE : ( *lpFBuf ).cDirty )

//==========================================================================
//
// VOID FsvBufIO_SetDirtyFlag( lpFBuf, fState )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//    fState - A value of TRUE or FALSE.
//
// This function sets the i/o buffer "dirty flag" using fState.
//
//==========================================================================

#define FsvBufIO_SetDirtyFlag(lpFBuf,fState) if ( ( lpFBuf ) != NULL ) ( *lpFBuf ).cDirty = ( char ) fState;

//==========================================================================
//
// VOID FsvBufIO_GetFileHandle( lpFBuf )
//
//    lpFBuf - Pointer to the structure used to buffer file i/o.
//
// This function returns the DOS file handle for the file that corresponds
// to *lpFBuf.
//
//==========================================================================

#define FsvBufIO_GetFileHandle(lpFBuf) ( ( ( lpFBuf ) == NULL ) ? 0 : ( *lpFBuf ).hFile )

//============================================================================
//
// FsvIO_ResetFloppyDrive( bDrvID ) : LONG
//
//    This function is used to "reset" the floppy in case the floppy has been
//    opened and a new diskette has been inserted.  DOS has some bugs so there
//    are times when this has to be done manually.
//
// Returns an error value, normally zero.
// 
//============================================================================

long WINAPI FsvIO_ResetFloppyDrive(
   BYTE bDrvID
);

//============================================================================
// FsvUtil_Zip2Exe(): LONG
//
// Takes a ZIP file (must be ZIP and cannot be any other archive file type)
// and turns it into a self-extracting archive file of the specified name.
// This function copies the data to the new self-extracting file and does not
// modify the original ZIP file.
//
// Returns: Error value (FSVERR_<x>), normally zero.
//
LONG WINAPI FsvUtil_Zip2Exe(
   LPCSTR   lpszZIPFile,      // Source ZIP file.  (Must contain full path.)
   LPCSTR   lpszSFXFile,      // Destination .EXE file. (Must have .EXE extension and must contain full path.)
   FARPROC  lpProgCBFunc,     // ZIP progress callback function
   DWORD    dwProgCBData,     // ZIP progress callback data
   FARPROC  lpErrCBFunc,      // ZIP error callback function
   DWORD    dwErrCBData,      // ZIP error callback data
   ERROR_CALLBACK lpCrit,     // Critical error callback function
   DWORD    dwCrit            // Critical error callback data
);
typedef LONG (WINAPI * LPFNFSVUTIL_ZIP2EXE)( LPCSTR, LPCSTR, FARPROC, DWORD, FARPROC, DWORD, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvUtil_MakeSFXZip: LONG
//
// Creates an empty self-extracting ZIP archive file.
//
// Returns: Error value (FSVERR_<x>), normally zero.
//
LONG WINAPI FsvUtil_MakeSFXZip(
   LPCSTR   lpszSFXFile,      // Destination .EXE file. (Must have .EXE extension and must contain full path.)
   FARPROC  lpErrCBFunc,      // ZIP error callback function
   DWORD    dwErrCBData,      // ZIP error callback data
   ERROR_CALLBACK lpCrit,     // Critical error callback function
   DWORD    dwCrit            // Critical error callback data
);
typedef LONG (WINAPI * LPFNFSVUTIL_MAKESFXZIP)( LPCSTR, FARPROC, DWORD, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvUtil_Exe2Zip(): LONG
//
// Takes a self-extracting archive file and transfers the contents to the
// specified destination archive file.
//
// Returns: Error value (FSVERR_<x>), normally zero.
//
LONG WINAPI FsvUtil_Exe2Zip(
   LPCSTR   lpszSFXFile,      // Source .EXE file. (Must have .EXE extension and must contain full path.)
   LPCSTR   lpszZIPFile,      // Destination archive file.  (Must contain full path.)
   FARPROC  lpProgCBFunc,     // ZIP progress callback function
   DWORD    dwProgCBData,     // ZIP progress callback data
   FARPROC  lpErrCBFunc,      // ZIP error callback function
   DWORD    dwErrCBData,      // ZIP error callback data
   ERROR_CALLBACK lpCrit,     // Critical error callback function
   DWORD    dwCrit            // Critical error callback data
);
typedef LONG (WINAPI * LPFNFSVUTIL_EXE2ZIP)( LPCSTR, LPCSTR, FARPROC, DWORD, FARPROC, DWORD, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvIO_FindFirstExWrap: WORD
//
// A wrapper around FsvIO_FindFirstEx and FsvIO_FindFirst.  Calls the
// extended Find First function first, then if that fails, the basic function.
// This function also translates the contents of the FSV_FINDLD structure to
// the FSV_FIND structure.
//
// The contents of lpdwHandle should be used for subsequent FindNextExWrap and
// FindCloseExWrap calls.  If this value is invalid on subsequent calls, then
// the contents of lpFind are assumed to be valid.  The FsvIO_FindCloseExWrap()
// function must be called at the end of the search sequence.
//
// Returns: 0 if successful (either call).  Non-zero if neither are successful.
//
WORD WINAPI FsvIO_FindFirstExWrap(
   WORD           wSearchAttribute,
   LPBYTE         lpbPath,
   LPFSV_FIND     lpFind,
   LPFSV_FINDLD   lpFindLD,
   LPDWORD        lpdwHandle,
   ERROR_CALLBACK lpCrit,
   DWORD          dwCrit,
   BOOL           fNetAuthenticate
);

//============================================================================
// FsvIO_FindNextExWrap: WORD
//
// A wrapper around FsvIO_FindNextEx and FsvIO_FindNext.  Calls the
// extended Find Next function if lpdwHandle is a valid file search handle.
// If not then lpFind is assumed to be valid and the basic Find Next function
// is called.
//
// This function also translates the contents of the FSV_FINDLD structure to
// the FSV_FIND structure.
//
// This function assumes that dwHandle and lpFind were filled in by
// FsvIO_FindFirstExWrap().  If not, then behaviour and result of calling
// this function is unpredictable.
//
// Returns: 0 if successful, non-zero if not successful.
//
WORD WINAPI FsvIO_FindNextExWrap(
   LPFSV_FIND     lpFind,
   LPFSV_FINDLD   lpFindLD,
   DWORD          dwHandle,
   ERROR_CALLBACK lpCrit,
   DWORD          dwCrit
);

//============================================================================
// FsvIO_FindCloseExWrap: void
//
// A wrapper around FsvIO_FindCloseEx function.  If lpdwHandle is valid then
// FsvIO_FindCloseEx is called to end the file search.  The lpFind and
// lpFindLD parameters are not used but provided for consistency's sake.
//
void WINAPI FsvIO_FindCloseExWrap(
   LPFSV_FIND     lpFind,
   LPFSV_FINDLD   lpFindLD,
   DWORD          dwHandle
);

//=============================================================================
// DWORD WINAPI FsvUtil_FileTimeToDOSTime(
//     LPFSV_FILETIME lpft,
//     LPBYTE         lpbms )
//
// MS-DOS 7's File Time To DOS Time function.
//
// Returns: DOS Date in HIWORD, DOS Time in LOWORD.
//          0xffffffff if result is an error.
//
DWORD __declspec(dllexport) WINAPI FsvUtil_FileTimeToDOSTime(
   LPFSV_FILETIME lpft,
   LPBYTE         lpbms );
typedef DWORD (WINAPI * LPFNFSVUTIL_FILETIMETODOSTIME)( LPFSV_FILETIME, LPBYTE );

//=============================================================================
// WORD WINAPI FsvUtil_DOSTimeToFileTime(
//     WORD     wDOSDate,
//     WORD     wDOSTime,
//     BYTE     bms,
//     LPFSV_FILETIME lpft )
//
// MS-DOS 7's DOS Time to File Time function.
//
// Returns: 0 on success, 0xffff on error.
//
WORD __declspec(dllexport) WINAPI FsvUtil_DOSTimeToFileTime(
   WORD  wDOSDate,
   WORD  wDOSTime,
   BYTE  bms,
   LPFSV_FILETIME lpft );
typedef WORD (WINAPI * LPFNFSVUTIL_DOSTIMETOFILETIME)( WORD, WORD, BYTE, LPFSV_FILETIME );

//============================================================================
// FsvIO_GetLFNFromName: LONG
//
// This function fills in the supplied buffer with the long filename for the
// given short (or long) filename.  Both lpszName and lpszLFN may point to the
// same buffer.
//
LONG WINAPI FsvIO_GetLFNFromName(
   LPCTSTR  lpszName,      // Fully qualified path\filename
   LPTSTR   lpszLFN,       // Buffer to fill (FSV_PATHBUF_LEN in size)
   ERROR_CALLBACK lpCrit,
   DWORD          dwCrit
);
typedef LONG (WINAPI * LPFNFSVIO_GETLFNFROMNAME)( LPCTSTR, LPTSTR, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvUtil_IsLFN: BOOL
//
// Returns indication as to whether specified name is a long [path]\filename.
// This function does not validate the correctness of the name.
//
BOOL WINAPI FsvUtil_IsLFN(
   LPTSTR   lpszName
);
typedef BOOL (WINAPI * LPFNFSVUTIL_ISLFN)( LPTSTR );

//============================================================================
// FsvUtil_IsUNC: BOOL
//
// Returns indication as to whether specified name is a UNC name.
// This function does not check the validity of any of the name subpieces.
//
BOOL WINAPI FsvUtil_IsUNC(
   LPCTSTR   lpszName
);
typedef BOOL (WINAPI * LPFNFSVUTIL_ISUNC)(LPCTSTR);

//============================================================================
// FsvUtil_IsFTP: BOOL
//
// Returns indication as to whether specified name is a FTP server name.
// This function does not check the validity of any of the name subpieces.
//
BOOL WINAPI FsvUtil_IsFTP(
   LPCTSTR   lpszName
);
typedef BOOL (WINAPI * LPFNFSVUTIL_ISFTP)(LPCTSTR);

//============================================================================
// FsvUtil_IsMyComputer: BOOL
//
// Returns indication as to whether specified name is a FTP server name.
// This function does not check the validity of any of the name subpieces.
//
BOOL WINAPI FsvUtil_IsMyComputer(
   LPCTSTR   lpszName
);
typedef BOOL (WINAPI * LPFNFSVUTIL_ISMYCOMPUTER)(LPCTSTR);

//============================================================================
// FsvIO_GetShortNameFromName: LONG
//
// This function fills in a buffer with the short name (8.3) of the specified
// fully qualified directory or file name.  Both pointer may point to the
// same buffer.
//
// Returns: Count of characters copied to buffer.
//
LONG WINAPI FsvIO_GetShortNameFromName(
   LPCTSTR  lpszName,      // Fully qualified path\filename
   LPTSTR   lpszShort,     // Buffer to fill (FSV_PATHBUF_LEN in size)
   ERROR_CALLBACK lpCrit,
   DWORD          dwCrit
);
typedef LONG (WINAPI * LPFNFSVIO_GETSHORTNAMEFROMNAME)( LPCTSTR, LPTSTR, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvUtil_GetPtrToRoot: LPTSTR
//
// This function returns a pointer to the start of the root of the
// specified path.  It returns NULL if an error occurs, or if no root is
// found.
//
LPTSTR WINAPI FsvUtil_GetPtrToRoot(
   LPCTSTR  lpszPath );
typedef LPTSTR (WINAPI * LPFNFSVUTIL_GETPTRTOROOT)(LPCTSTR);

//============================================================================
// FsvUtil_DoesVolumeSupportLFN: BOOL
//
// Returns whether the specified volume supports LFNs or not.  The volume may
// be specified as any valid DOS pathname or a UNC name.
//
BOOL WINAPI FsvUtil_DoesVolumeSupportLFN(
   LPCTSTR  lpszVolume );
typedef BOOL (WINAPI * LPFNFSVUTIL_DOESVOLUMESUPPORTLFN)( LPCTSTR );

//============================================================================
// FsvIO_GetDiskRec: LONG
//
// Fills in DISKREC.
//
// Returns: error code.
//
LONG WINAPI FsvIO_GetDiskRec(
   TCHAR       chDrive,    // Drive letter.
   LPDISKREC   lpDiskRec,
   ERROR_CALLBACK lpCrit,
   DWORD          dwCrit );
typedef LONG (WINAPI * LPFNFSVIO_GETDISKREC)(BYTE,LPDISKREC,ERROR_CALLBACK,DWORD);

//============================================================================
// FsvIO_DiskReadBlock: LONG
//
// Reads in a block specified in the DISKREC structure.
//
// Returns: error code.
//
LONG WINAPI FsvIO_DiskReadBlock(
   LPDISKREC   lpDiskRec,
   ERROR_CALLBACK lpCrit,
   DWORD          dwCrit );
typedef LONG (WINAPI * LPFNFSVIO_DISKREADBLOCK)(LPDISKREC,ERROR_CALLBACK,DWORD);

//============================================================================
// FsvIO_DiskWriteBlock: LONG
//
// Writes out a block specified in the DISKREC structure.
//
// Returns: error code.
//
LONG WINAPI FsvIO_DiskWriteBlock(
   LPDISKREC   lpDiskRec,
   ERROR_CALLBACK lpCrit,
   DWORD          dwCrit );
typedef LONG (WINAPI * LPFNFSVIO_DISKWRITEBLOCK)(LPDISKREC,ERROR_CALLBACK,DWORD);

//============================================================================
// FsvIO_DiskGetPhysicalInfo: LONG
//
// Fills in a ABSDISKREC structure.
//
// Returns: error code.
//
LONG WINAPI FsvIO_DiskGetPhysicalInfo(
   LPABSDISKREC   lpAbsDisk
);
typedef LONG (WINAPI * LPFNFSVIO_DISKGETPHYSICALINFO)(LPABSDISKREC);

//
//
// UNDO STUFF
//
//

// Possible UNDO operation types
typedef enum
{
   FSV_UNDO_DELETE, FSV_UNDO_COPY, FSV_UNDO_MOVE, FSV_UNDO_RENAME,
   FSV_UNDO_CREATEDIR, FSV_UNDO_REMOVEDIR,
   FSV_UNDO_SE_CREATEFILE, FSV_UNDO_SE_CREATEDIR,
   FSV_UNDO_SE_DELETE,
   FSV_UNDO_SE_RENAMEFILE, FSV_UNDO_SE_RENAMEDIR,
   FSV_UNDO_SE_MOVEFILE, FSV_UNDO_SE_MOVEDIR,
   FSV_UNDO_NONE, FSV_UNDO_OPCOUNT
} FSV_UNDO_TYPES;

// Undo info record flags.
#define FSV_UNDO_FLAG_MARKER     0x00000001  // Marks the beginning of a new, named UNDO section, may be combined with FSV_UNDO_FLAG_OPEND
#define FSV_UNDO_FLAG_EXTERNAL   0x00000002  // Signifies this operation was performed outside of File Services
#define FSV_UNDO_FLAG_PREPPED    0x00000004  // File Services internal use only
#define FSV_UNDO_FLAG_SUCCESS    0x00000008  // File Services internal use only
#define FSV_UNDO_FLAG_REMOVED    0x00000010  // File Services internal use only
#define FSV_UNDO_FLAG_OPEND      0x00000020  // Marks the end of a named UNDO section, combined with FSV_UNDO_FLAG_MARKER

// Undo info struct
typedef struct _FSV_UNDO_INFOSTRUCT {
   DWORD             dwOffset;      // File Services internal use.
   DWORD             dwPrev;        // File Services internal use.
   DWORD             dwProcessId;   // Process id of process that generated this undo object.
                                    // Not valid if FSV_UNDO_FLAG_EXTERNAL is set.
   FSV_UNDO_TYPES    undoType;      // Operation to be reversed.
   DWORD             dwFlags;       // FSV_UNDO_FLAG_<x> flags.
   SYSTEMTIME        stPosted;      // System time when event was posted.
   TCHAR             szNameSrc[FSV_PATHBUF_LEN];   // The "From" path/filename.
                                                   // If FSV_UNDO_FLAG_MARKER is set, this contains
                                                   // the string that was passed to FsvUndo_SetMarker().
   TCHAR             szNameDst[FSV_PATHBUF_LEN];   // The "To" path/filename
} FSV_UNDO_INFOSTRUCT, FAR * LPFSV_UNDO_INFOSTRUCT, * PFSV_UNDO_INFOSTRUCT;


//============================================================================
// FsvUndo_SetMarker: LONG
//
// Places a marker in the Undo stack.
//
// Returns: FSV_SUCCESS, FSV_ERROR, FSVERR_TIMEOUT
//
LONG WINAPI FsvUndo_SetMarker(
   FSV_UNDO_TYPES undoType,   // Undo type (IN).
   BOOL     fEnd,             // Mark the end? (IN).
   LPCTSTR  lpszMarkerName,   // Name to give the marker (IN).
   LPCTSTR  lpszMarkerName2,  // A second string that can be given to the marker (IN).
   BOOL     fWait,            // If initial try is blocked, try until success? (IN)
   DWORD    dwTimeOut         // If fWait == TRUE, try for this many milliseconds before returning FSVERR_TIMEOUT.
                              // If dwTimeOut == 0, try forever until successful.
);
typedef LONG (WINAPI * LPFNFSVUNDO_SETMARKER)( LPCTSTR, BOOL, DWORD );

//============================================================================
// FsvUndo_CanUndo(): LONG
//
// Queries whether File Services has Undo capability.
//
// Returns: -1 if undo could not be initialized, 0 if undo has not been
//          initialized, and 1 if undo is available.
//
LONG WINAPI FsvUndo_CanUndo();
typedef LONG (WINAPI * LPFNFSVUNDO_CANUNDO)();

//============================================================================
// FsvUndo_PurgeUndoStack: LONG
//
// Purges all existing Undo records.
//
// Returns: File Services error code.
//
LONG WINAPI FsvUndo_PurgeUndoStack(
   BOOL  fWait,            // TRUE = wait for all blocks to clear or until timeout reached, FALSE = return immediately if op blocked.
   DWORD dwTimeOut         // maximum amount of time (ms) to wait before returning.
);
typedef LONG (WINAPI * LPFNFSVUNDO_PURGEUNDOSTACK)( BOOL, DWORD );

//============================================================================
// FsvUndo_FileLock: LONG
//
// Requests a complete lock on the undo stack file.  All entries in the
// in-memory queue are flushed out to disk before the lock is acquired.
//
// Returns: 1 if successful, 0 if lock cannot be acquired (e.g., someone else
//          has the lock), FSV_ERROR if error occurs, or FSVERR_TIMEOUT on wait
//          timeout.
//
LONG WINAPI FsvUndo_FileLock(
   BOOL  fWait,         // If lock cannot be imeediately acquired, wait until it is free or until timeout.
   DWORD dwTimeOut      // Specifies timeout value (ms) in case of wait. 0 indicates indefinite wait.
);
typedef LONG (WINAPI * LPFNFSVUNDO_FILELOCK)(BOOL,DWORD);

//============================================================================
// FsvUndo_FileUnlock: LONG
//
// Unlocks the stack file.  Only the process that locked the file can unlock
// it.
//
// Returns: 1 if unlocked, 0 if not correct process or never locked, FSV_ERROR
//          if error, FSVERR_TIMEOUT on timeout.
//
LONG WINAPI FsvUndo_FileUnlock(
   BOOL  fWait,         // If lock cannot be imeediately freed, wait until it is free or until timeout.
   DWORD dwTimeOut      // Specifies timeout value (ms) in case of wait. 0 indicates indefinite wait.
);
typedef LONG (WINAPI * LPFNFSVUNDO_FILEUNLOCK)( BOOL, DWORD );

//============================================================================
// FsvUndo_GetStackSizeLimit: DWORD
//
// Returns the maximum stack size (in number of entries).  Returns 0xffffffff
// if error occurs.  This function should not be called if there is a
// possbility that FsvUndo_SetStackSizeLimit() may be called while inside this
// function.
//
DWORD WINAPI FsvUndo_GetStackSizeLimit();
typedef DWORD (WINAPI * LPFNFSVUNDO_GETSTACKSIZELIMIT)();

//============================================================================
// FsvUndo_SetStackSizeLimit: DWORD
//
// Sets the maximum number of undo records that may be placed into the stack
// file.  If current count of entries is less than the limit, the limit is
// simply increased.  If the current count of entries is greater than the
// new limit, old entries are purged until the new limit is reached.
//
// This function will fail if a stack file lock has not been acquired via
// FsvUndo_FileLock().
//
// Returns: Old limit, or 0xffffffff on error.
//
DWORD WINAPI FsvUndo_SetStackSizeLimit(
   DWORD dwNewLimit,    // New stack size (in number of items)
   BOOL  fWait,         // If operation blocked, wait until free?
   DWORD dwTimeOut      // Maximum amount of time (ms) to wait for block to free.
);
typedef DWORD (WINAPI * LPFNFSVUNDO_SETSTACKSIZELIMIT)(DWORD,BOOL,DWORD);

//============================================================================
// FsvUndo_StackFindTop: LONG
//
// Fills in the undo structure with information about the top of the stack.
// This function will fail if a lock is not acquired prior to the call.
//
// Returns: FSV_SUCCESS,
//          FSV_ERROR,
//          FSVERR_TIMEOUT,
//          FSVERR_EOF
//
LONG WINAPI FsvUndo_StackFindTop(
   LPFSV_UNDO_INFOSTRUCT   lpUndoInfo, // UndoInfo structure to fill.
   BOOL  fWait,         // If operation blocked, wait until free?
   DWORD dwTimeOut      // Maximum amount of time (ms) to wait for block to free.
);
typedef LONG (WINAPI * LPFNFSVUNDO_STACKFINDTOP)( LPFSV_UNDO_INFOSTRUCT, BOOL, DWORD );

//============================================================================
// FsvUndo_StackFindNext: LONG
//
// Fills in the undo strcuture with the information for the undo object after
// the specified one.  The initial object must have been obtained via call
// to FsvUndo_StackFindTop().  In subsequent calls to this function, the
// object filled in by the previous call to this function should be used for
// the first parameter.
//
// This function will fail if a lock is not acquired prior to the call.
//
// Returns: FSV_SUCCESS,
//          FSV_ERROR,
//          FSVERR_TIMEOUT,
//          FSVERR_EOF
//
LONG WINAPI FsvUndo_StackFindNext(
   LPFSV_UNDO_INFOSTRUCT   lpUndoInfo, // UndoInfo from prior call to FsvUndo_StackFindTop() or FsvUndo_StackFindNext()
   LPFSV_UNDO_INFOSTRUCT   lpUndoNext, // UndoInfo structure to fill.
   BOOL  fWait,         // If operation blocked, wait until free?
   DWORD dwTimeOut      // Maximum amount of time (ms) to wait for block to free.
);
typedef LONG (WINAPI * LPFNFSVUNDO_STACKFINDNEXT)( LPFSV_UNDO_INFOSTRUCT, LPFSV_UNDO_INFOSTRUCT, BOOL, DWORD );

//============================================================================
// FsvUndo_PrepUndo: LONG
//
// Prepares an undo object for an actual undo operation.  This must be called
// before FsvUndo_ExecuteUndo() is called.  This function will fail if a lock
// is not acquired prior to the call.  Once this function is called, the
// specific undo information is removed from the undo stack.
//
// Returns: FSV_SUCCESS,
//          FSV_ERROR,
//          FSV_TIMEOUT
//
LONG WINAPI FsvUndo_PrepUndo(
   LPFSV_UNDO_INFOSTRUCT   lpUndoInfo, // UndoInfo structure to prep.
   BOOL  fWait,         // If operation blocked, wait until free?
   DWORD dwTimeOut      // Maximum amount of time (ms) to wait for block to free.
);
typedef LONG (WINAPI * LPFNFSVUNDO_PREPUNDO)( LPFSV_UNDO_INFOSTRUCT, BOOL, DWORD );

//============================================================================
// FsvUndo_ExecuteUndo: LONG
//
// Attempts to undo the operation specified in the lpUndoInfo parameter.
//
// Operation and Undo action:
//    Delete:     Undeletes target
//    Copy:       Deletes source or moves source to target
//    Move:       Moves source to target
//    Rename:     Renames source to target
//
// Returns: File Services error code.
//
LONG WINAPI FsvUndo_ExecuteUndo(
   LPFSV_UNDO_INFOSTRUCT   lpUndoInfo,    // Operation to undo
   BOOL                    fOverwrite,    // If TRUE, overwrites target if it exists, if FALSE returns FSVERR_FILE_EXISTS if target exists
   PROCESS_CALLBACK        lpfn,          // Progress callback
   DWORD                   dwUserDword,   // User defined data to be passed back to progress callback
   ERROR_CALLBACK          lpCrit,        // Critical error handler callback function pointer
   DWORD                   dwCrit         // Optional user-defined data to be passed to the crititcal error callback
);
typedef LONG (WINAPI * LPFNFSVUNDO_EXECUTEUNDO)( LPFSV_UNDO_INFOSTRUCT, BOOL, PROCESS_CALLBACK, DWORD, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvUndo_FinishUndo: LONG
//
// In all conceivable cases, this function should be called after a
// successful call to FsvUndo_ExecuteUndo().  The only time it wouldn't be
// called is if there is some need to keep the Undo record in the file.  This
// function removes the Undo record from the undo stack file.
//
// Returns: FSV_SUCCESS,
//          FSV_ERROR,
//          FSV_TIMEOUT
//
LONG WINAPI FsvUndo_FinishUndo(
   LPFSV_UNDO_INFOSTRUCT   lpUndoInfo, // UndoInfo structure to prep.
   BOOL  fWait,         // If operation blocked, wait until free?
   DWORD dwTimeOut      // Maximum amount of time (ms) to wait for block to free.
);
typedef LONG (WINAPI * LPFNFSVUNDO_FINISHUNDO)( LPFSV_UNDO_INFOSTRUCT, BOOL, DWORD );

//============================================================================
// FsvUndo_BlowAwayRecord: LONG
//
// This function is used to remove a record from the Undo stack without
// having to actually preform an Undo operation.  This function is analogous
// to calling FsvUndo_PrepUndo() followed by FsvUndo_ExecuteUndo() and
// finally FsvUndo_FinishUndo().
//
// Returns: FSV_SUCCESS,
//          FSV_ERROR,
//          FSV_TIMEOUT
//
LONG WINAPI FsvUndo_BlowAwayRecord(
   LPFSV_UNDO_INFOSTRUCT   lpUndoInfo, // UndoInfo structure containing undo record information to remove.
   BOOL  fWait,                        // If operation blocked, wait until free?
   DWORD dwTimeOut                     // Maximum amount of time (ms) to wait for block to free.
);
typedef LONG (WINAPI * LPFNFSVUNDO_BLOWAWAYRECORD)( LPFSV_UNDO_INFOSTRUCT, BOOL, DWORD );

//============================================================================
// FsvUndo_DisablePost: DWORD
//
// Enables/disables posting of undo information to the undo stack.
// Internally uses a counter to keep track of actual counts of enable vs.
// disable.  There MUST be one enable call for each disable call.
//
// Returns: New disable count.  0 means posts are enabled.
//
DWORD WINAPI FsvUndo_DisablePost(
   BOOL  fDisable    // TRUE == increment disable count.  FALSE == decrement disable count.
);
typedef DWORD (WINAPI * LPFNFSVUNDO_DISABLEPOST)( BOOL );

//============================================================================
// FsvUndo_LoadExceptionList: BOOL
//
// Allocates and loads Undo exception list.
//
// Returns: TRUE if successful.
//
BOOL WINAPI FsvUndo_LoadExceptionList(
   LPTSTR **   palpszExceptions, // Pointer to exception list to be allocated and filled.  Pointer value must be NULL on entry.
   LPDWORD     lpdwCount         // Count of items in exception list.  Filled by function, may be 0 on exit.
);
typedef BOOL (WINAPI * LPFNFSVUNDO_LOADEXCEPTIONLIST)(LPTSTR**,LPDWORD);

//============================================================================
// FsvUndo_FreeExceptionList: BOOL
//
// Deallocates memory used by the Undo exception list.
//
// Returns: TRUE if successful.
//
BOOL WINAPI FsvUndo_FreeExceptionList(
   LPTSTR **   palpszExceptions,    // Exception list to free, allocated by FsvUndo_LoadExceptionList().  Pointer value set to NULL on return.
   LPDWORD     lpdwCount            // Count of items in list.  Count set to 0 on return.
);
typedef BOOL (WINAPI * LPFNFSVUNDO_FREEEXCEPTIONLIST)(LPTSTR**,LPDWORD);

//============================================================================
// FsvUndo_SetSaveExternalEvents: BOOL
//
// Enables/disables the ability to save Undo information for file system
// events that occur outside of the File Services API.
//
// Returns: Previous state.
//
BOOL WINAPI FsvUndo_SetSaveExternalEvents(
   BOOL  fEnable     // TRUE = save external events, FALSE = don't save external events.
);
typedef BOOL (WINAPI * LPFNFSVUNDO_SETSAVEEXTERNALEVENTS)(BOOL);

//============================================================================
// FsvUndo_GetSaveExternalEvents: BOOL
//
// Returns the current state of the save external events flag.
//
BOOL WINAPI FsvUndo_GetSaveExternalEvents(void);
typedef BOOL (WINAPI * LPFNFSVUNDO_GETSAVEEXTERNALEVENTS)(void);

//============================================================================
// FsvStr_RevStr: LPTSTR
//
// Returns a pointer to the n-th character from the end of the string.  If
// n is 0, the pointer is to the terminating NULL of the string.  If n is
// greater than the length of the string, the return value is NULL.
//
LPTSTR WINAPI FsvStr_RevStr(
   LPCTSTR  lpszIn,
   int      nthChar
);
typedef LPTSTR (WINAPI * LPFNFSVSTR_REVSTR)(LPCTSTR,int);

//============================================================================
// FsvStr_FwdStr: LPTSTR
//
// Returns a pointer to the n-th character from the start of the string.  The
// first character of the string is a 0-th element.  If n is greater than the
// length of the string, the return value is NULL.  If n is the length of the
// string, the returned pointer points to the terminating NULL character.
//
LPTSTR WINAPI FsvStr_FwdStr(
   LPCTSTR  lpszIn,
   int      nthChar
);
typedef LPTSTR (WINAPI * LPFNFSVSTR_FWDSTR)(LPCTSTR,int);

//============================================================================
// FsvStr_GetByteCount: LONG
//
// Returns the total count of bytes used by the string (as opposed to the
// string length which is normally a count of characters used by the string).
// The count does NOT include the terminating NULL character.
//
LONG WINAPI FsvStr_GetByteCount(
   LPCTSTR  lpszIn
);
typedef LONG (WINAPI * LPFNFSVSTR_GETBYTECOUNT)(LPCTSTR);

//============================================================================
// FsvStr_GetCharCount: LONG
//
// Returns the total count of characters used by the string (length of the
// string rather than the count of bytes used by the string).  The count does
// NOT include the terminating NULL character.
//
LONG WINAPI FsvStr_GetCharCount(
   LPCTSTR  lpszIn );
typedef LONG (WINAPI * LPFNFSVSTR_GETCHARCOUNT)(LPCTSTR);


//
//
// FTP STUFF
//
//
//**********************************************************************
// ftp connection...........
#define FSV_MAXFTPDESC        255   // max sizeof FTP site description 
#define FSV_MAXFTPADDR        255   // max sizeof FTP site address 
#define FSV_MAXFTPDEFLOGDIR   255   // max sizeof FTP site default login dir
#define FSV_MAXFTPLOGNAME     255   // max sizeof FTP site login name  
#define FSV_MAXFTPPW          255   // max sizeof FTP site password 
#define FSV_MAXFTPACCT        255   // max sizeof FTP site account 
#define FSV_MAXFWHOST         255   // max sizeof FTP FW host name
#define FSV_MAXFWUSERID       255   // max sizeof FTP FW user ID
#define FSV_MAXFWPASSWORD     255   // max sizeof FTP FW password
#define FSV_MAXFTPFIELDS      20    // max number of FTP fields in a custom definition
#define FSV_FTPPREFIX                  TEXT("\\\\\\")

// ftp initialization.......
#define FSV_INI_FTPSITESDEF_SECTION    TEXT("SYMFSV4\\FTP")
#define FSV_INI_FTPEMAILADDRESS_KEY    TEXT("E-mail Address")
#define FSV_INI_FTPDISPWINDOW_KEY      TEXT("FTP Status Window")
#define FSV_INI_FTPSITENAME_KEY        TEXT("Site Name")
#define FSV_INI_FTPSITEADDR_KEY        TEXT("Site Address")
#define FSV_INI_FTPSITEDEFDIR_KEY      TEXT("Default Dir")
#define FSV_INI_FTPROOTPATH_KEY        TEXT("Root Path")
#define FSV_INI_FTPSITELOGNAME_KEY     TEXT("Login Name")
#define FSV_INI_FTPSITEPASSWORD_KEY    TEXT("Password")
#define FSV_INI_FTPSITEACCT_KEY        TEXT("Account")
#define FSV_INI_FTPSITEANONYMOUS_KEY   TEXT("Anonymous")
#define FSV_INI_FTPSITEHOSTTYPE_KEY    TEXT("Host Type")
#define FSV_INI_FTPSITERETRYCNT_KEY    TEXT("Retry Count")
#define FSV_INI_FTPSITETIMEOUT_KEY     TEXT("Timeout")
#define FSV_INI_FTPSITEPORT_KEY        TEXT("Port")
#define FSV_INI_FTPSITESETTINGS_KEY    TEXT("Settings")
#define FSV_INI_FTPSITEFWUSE_KEY       TEXT("Firewall")
#define FSV_INI_FTPSITEFWTYPE_KEY      TEXT("Firewall Type")
#define FSV_INI_FTPSITEFWHOSTNAME_KEY  TEXT("Firewall HostName")
#define FSV_INI_FTPSITEFWUSERID_KEY    TEXT("Firewall User ID")
#define FSV_INI_FTPSITEFWPASSWORD_KEY  TEXT("Firewall Password")
#define FSV_INI_FTPSITEFWPORT_KEY      TEXT("Firewall Port")
#define FSV_INI_FTPSITETKDIR_KEY       TEXT("Dir Token List")
#define FSV_INI_FTPSITETKFILE_KEY      TEXT("File Token List")
#define FSV_INI_FTPSITEDISPLINE_KEY    TEXT("Display Line")
#define FSV_INI_FTPSITECACHELIST_KEY   TEXT("Cache List")

// FTP error codes
#define FSVCBM_NO_FTPEMAILADDRESS      (-0x02)
#define FSVCBM_NO_FTPUSERNAME          (-0x03)
#define FSVCBM_NO_FTPPASSWORD          (-0x04)
#define FSVCBM_FTPMESSAGEBOX           (-0x05)

// FTP Error MessageBox Types
#define FSVFTP_MSG_OK                  0
#define FSVFTP_MSG_OKCANCEL            1
#define FSVFTP_MSG_YESNO               2

// FTP Token Types
#define   FSV_NUMFTPTOKENS 13    // number of FTP Tokens available
#define   FTPUNIXDATETIME  1
#define   FTPUNIXATTRIB    2
#define   FTPVMSNAME       3
//#define FTPVMSSIZE       4
#define   FTPVMSDATE       4
#define   FTPVMSATTRIB     5
#define   FTPTIME          6
#define   FTPSIZE          7
#define   FTPNAME          8
#define   FTPATTRIB        9
#define   FTPTYPE          10
#define   FTPIGNORE        11
#define   FTPNWATTRIB      12
#define   FTPDATE          13

//callback for filemanager to call.......
typedef struct _FSVFTP_CALLBACKSTRUCT {
   HWND     hwndParent;    // Handle to parent window (usually Filemgr Frame)
   LPTSTR   lpszUsername;  // ptr to username string
   LPTSTR   lpszPassword;  // ptr to password string
   LPTSTR   lpszDispText;  // ptr to text to display in FTP Output window
   LPTSTR   lpszErrorMsg;  // ptr to text to display in an error messagebox
   WORD     wButtons;      // buttons to display (see FTP Error MessageBox Types above)
   LPVOID   lpData;        // ptr to data
   long     lCBM;          // Callback message ID
} FSVFTP_CALLBACKSTRUCT, FAR * LPFSVFTP_CALLBACKSTRUCT, * PFSVFTP_CALLBACKSTRUCT;


typedef struct _FSV_FTP_ENUMSTRUCT {
   DWORD             dwIndex;       // File Services internal use.
   HKEY              hKey;          // File Services internal use.
   TCHAR             szFTPSiteName[FSV_MAXFTPDESC]; // The site name
} FSV_FTP_ENUMSTRUCT, FAR * LPFSV_FTP_ENUMSTRUCT, * PFSV_FTP_ENUMSTRUCT;


//***************************************************************************
// information about the ftp sites........
typedef struct _FSV_FTP_SITESTRUCT
{
   TCHAR    szRootPath[FSV_MAXFTPDEFLOGDIR + 1];
   TCHAR    szDescription[FSV_MAXFTPDESC + 1];
   TCHAR    szAddress[FSV_MAXFTPADDR + 1];
   TCHAR    szDefLogDir[FSV_MAXFTPDEFLOGDIR + 1];
   TCHAR    szLoginName[FSV_MAXFTPLOGNAME + 1];
   TCHAR    szPassword[FSV_MAXFTPPW + 1];
   TCHAR    szAcct[FSV_MAXFTPACCT + 1];
   BOOL     fUseAnonymous;                      // anonymous ftp flag......
   UINT     uHostType;
   UINT     uRetryCount;
   UINT     uTimeout;
   UINT     uConnectPort;
   BOOL     fUseFirewall;                       // use gateway or not
   UINT     uFWType;                            // gateway type.....
   TCHAR    szFWHostName[FSV_MAXFWHOST];        // hostname for gateway use
   TCHAR    szFWUserID[FSV_MAXFWUSERID];        // user name for gateway...
   TCHAR    szFWPassword[FSV_MAXFWPASSWORD];    // password for gateway..
   UINT     uFWPort;                            // gateway port....
   int      anDirFields[FSV_MAXFTPFIELDS];      // Custom dir fields
   int      anFileFields[FSV_MAXFTPFIELDS];     // Custom file fields
   BOOL     fDisplayLine;                       //whether to display line in msg window
   LPTSTR FAR * lplpList;                        //list of temp files and cached dirs
} FSV_FTP_SITESTRUCT, FAR * LPFSV_FTP_SITESTRUCT, * PFSV_FTP_SITESTRUCT;


// FTP Callback prototype
typedef LONG (CALLBACK * LPFNFDLG_FTPCALLBACK)( LPFSVFTP_CALLBACKSTRUCT lpCallbackInfo );

//============================================================================
// FsvUtil_FTPParseFields: LONG
//
//    lpszLine       line of text from a directory listing
//    lpDirEntry     ptr to dir entry struct to fill out
//    lpnTokenList   array of token types (ints) indicating order and types of tokens
//
// Returns: FSV_SUCCESS if line was successfully parsed, or other error codes.
//
LONG WINAPI FsvUtil_FTPParseFields(
   LPTSTR                  lpszLine,
   LPFSVIO_DIRENTRYSTRUCT  lpDirEntry,
   LPINT                   lpnTokenList
);
typedef LONG (WINAPI * LPFNFSVUTIL_FTPPARSEFIELDS)( LPTSTR, LPFSVIO_DIRENTRYSTRUCT, LPINT );

//============================================================================
//
// FsvUtil_FTPCacheAdd( lpPath, lpTemp ): BOOL
//
//       lpPath - The path of the cached dir
//       lpTemp -  The temp file with the dir listing
//       lpSite - Site Info for the site (optional)
//
BOOL WINAPI FsvUtil_FTPCacheAdd( 
   LPTSTR               lpPath, 
   LPTSTR               lpTemp,
   LPFSV_FTP_SITESTRUCT lpSite
);

typedef BOOL (WINAPI *LPFNFSVUTIL_FTPCACHEADD)(LPTSTR, LPTSTR, LPFSV_FTP_SITESTRUCT );

//============================================================================
//
// FsvUtil_FTPCacheRemove( lpPath, lpSite ): BOOL
//
//       lpPath - The path of the cached dir to remove if found
//       lpSite - SiteInfo (optional)
// Removes the cached dir and the temp file for it
//
BOOL WINAPI FsvUtil_FTPCacheRemove( 
   LPTSTR               lpPath, 
   LPFSV_FTP_SITESTRUCT lpSite);

typedef BOOL (WINAPI *LPFNFSVUTIL_FTPCACHEREMOVE)(LPTSTR, LPFSV_FTP_SITESTRUCT );


//============================================================================
//
// FsvUtil_FTPCacheQuery( lpPath, lpBuff, lpSite ): BOOL
//
//       lpPath - The path of the cached dir to remove if found
//       lpBuff - Buffer for temp file name 
//       lpSite - SiteInfo (optional)
// Returns temp file name for the dir if it was cached
//
BOOL WINAPI FsvUtil_FTPCacheQuery( 
   LPTSTR               lpPath,
   LPTSTR               lpBuff,
   LPFSV_FTP_SITESTRUCT lpSite);

typedef BOOL (WINAPI *LPFNFSVUTIL_FTPCACHEQUERY)(LPTSTR, LPTSTR, LPFSV_FTP_SITESTRUCT );

//============================================================================
// FsvFTP_Initialize: LONG
//
// Performs all neccessary FTP initialization.
//
// Returns: FSV_SUCCESS if successfully initialized, or other error codes.
//
LONG WINAPI FsvFTP_Initialize();
typedef LONG (WINAPI * LPFNFSVFTP_INITIALIZE)();

//============================================================================
// FsvFTP_Uninitialize: LONG
//
// Performs all neccessary FTP uninitialization.
//
// Returns: FSV_SUCCESS if successfully uninitialized, or other error codes.
//
LONG WINAPI FsvFTP_Uninitialize();
typedef LONG (WINAPI * LPFNFSVFTP_UNINITIALIZE)();

//=============================================================================================
// 
// FsvDei_EnumFTPSites: LONG
//
// Reads FTP entries from registry and enumerates them

LONG WINAPI FsvDei_EnumFTPSites( LPFSV_DEITREE lptdei, LPFSV_DIRENTRYINFO lpdei );
typedef LONG (WINAPI * LPFNFSVDEI_ENUMFTPSITES)(LPFSV_DEITREE, LPFSV_DIRENTRYINFO);

//============================================================================
// FsvFTP_IsFTPSite: BOOL
//
// Determines whether the specified path resides on an FTP server.  This 
// function should be called to determine which IO functions to use, normal
// or FTP.
//
// Returns TRUE if the path is that of an FTP Site.
//
BOOL WINAPI FsvFTP_IsFTPSite(
   LPCTSTR  lpszPathName );
typedef BOOL (WINAPI * LPFNFSVFTP_ISFTPSITE)(LPCTSTR);

//============================================================================
// FsvFTP_GetSocket: SOCKET
//
// Attempts to find the socket corresponding to the site lpszSiteName
//
// Returns 0 if no socket found. Else return socket id
//
SOCKET WINAPI FsvFTP_GetSocket( LPCTSTR lpszSiteName );
typedef SOCKET (WINAPI * LPFNFSVFTP_GETSOCKET)(LPCTSTR);

//============================================================================
// FsvFTP_ShutDown() : void
// This function gets called when filemanager closes , it does all necessary
// cleanup for sockets and frees up the memory allocated to hold sitenames..
void WINAPI FsvFTP_ShutDown();
typedef VOID (WINAPI *LPFNFSVFTP_SHUTDOWN)();
//============================================================================

//============================================================================
// FsvFTP_ConnectSite: LONG
//
// Attempts to attach to the site specified.  This function first checks to
// see if the site definition exists in the registry.  If it does, it then
// checks to see if a password is defined.  If not, it will query the user
// for the password for the site.  Note: this function also brings up the
// status window if the proper setting is set and it is not already up.  
//
// Returns FSV_SUCCESS or other error codes.
//
LONG WINAPI FsvFTP_ConnectSite(
   LPCTSTR  lpszSiteName );
typedef LONG (WINAPI * LPFNFSVFTP_CONNECTSITE)(LPCTSTR);

//============================================================================
// FsvFTP_DisconnectSite: LONG
//
// Attempts to disconnect the site specified, then closes the status window
// if it was up and this is the last site open.
//
// Returns FSV_SUCCESS or other error codes.
//
LONG WINAPI FsvFTP_DisconnectSite(
   LPCTSTR  lpszSiteName );
typedef LONG (WINAPI * LPFNFSVFTP_DISCONNECTSITE)(LPCTSTR);

//============================================================================
// FsvFTP_CopyFile: LONG
//
// Copies the source file to the destination file.
//
//    lpszSourceFile    Source file path        (one of these better be
//    lpszDestFile      Destination file path    an FTP Site!!!)
//    lpCrit            Critical error handler
//    dwCrit            dwParam to pass to critical error handler
//
// Returns FSV_SUCCESS or other error codes.
//
LONG WINAPI FsvFTP_CopyFile(
   LPCTSTR  lpszSourceFile, LPCTSTR lpszDestFile, PROCESS_CALLBACK lpfn, DWORD dwUserDword, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef LONG (WINAPI * LPFNFSVFTP_COPYFILE)( LPCTSTR, LPCTSTR, PROCESS_CALLBACK, DWORD, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvFTP_DeleteFile: LONG
//
// Deletes the specified file.
//
// Input:
//    lpszFileName      specifies the file to be deleted (must begin with
//                                                       \\\FTP Site Name\)
//    lpCrit            Critical error handler
//    dwCrit            dwParam to pass to critical error handler
//
// Returns FSV_SUCCESS or other error codes.
//
LONG WINAPI FsvFTP_DeleteFile(
   LPCTSTR  lpszFilename, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef LONG (WINAPI * LPFNFSVFTP_DELETEFILE)( LPCTSTR, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvFTP_RenameFile: LONG
//
// Renames the source file to the destination file on a remote FTP site.
//
//    lpszSourceFile    Source file path        (must begin with \\\FTP site\)
//    lpszDestFile      Destination file name   
//    lpCrit            Critical error handler
//    dwCrit            dwParam to pass to critical error handler
//
// Returns FSV_SUCCESS or other error codes.
//
LONG WINAPI FsvFTP_RenameFile(
   LPCTSTR  lpszSourceFile, LPCTSTR lpszDestFile, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef LONG (WINAPI * LPFNFSVFTP_RENAMEFILE)( LPCTSTR, LPCTSTR, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvFTP_MakeDir: LONG
//
// Creates the specified directory on a remote FTP site.
//
//    lpszPath          New directory     (must begin with \\\FTP site\)
//    lpCrit            Critical error handler
//    dwCrit            dwParam to pass to critical error handler
//
// Returns FSV_SUCCESS or other error codes.
//
LONG WINAPI FsvFTP_MakeDir(
   LPCTSTR  lpszPath, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef LONG (WINAPI * LPFNFSVFTP_MAKEDIR)( LPCTSTR, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvFTP_RemoveDir: LONG
//
// Removes the specified directory on a remote FTP site.
//
//    lpszPath          Directory to remove (must begin with \\\FTP site\)
//    lpCrit            Critical error handler
//    dwCrit            dwParam to pass to critical error handler
//
// Returns FSV_SUCCESS or other error codes.
//
LONG WINAPI FsvFTP_RemoveDir(
   LPCTSTR  lpszPath, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef LONG (WINAPI * LPFNFSVFTP_REMOVEDIR)( LPCTSTR, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvFTP_EnumDir: LONG
//
// Enumerates the files and directories on a remote FTP site directory.
//
//    lpszPath          Directory to enumerate (must begin with \\\FTP site\)
//    lpCrit            Critical error handler
//    dwCrit            dwParam to pass to critical error handler
//
// Returns FSV_SUCCESS or other error codes.
//
LONG WINAPI FsvFTP_EnumDir(
   LPCTSTR  lpszPath, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef LONG (WINAPI * LPFNFSVFTP_ENUMDIR)( LPCTSTR, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvFTP_EnumDirs: LONG
//
// Enumerates the directories on a remote FTP site.
//
//    lpszPath          Directory to enum from (must begin with \\\FTP site\)
//    lpCrit            Critical error handler
//    dwCrit            dwParam to pass to critical error handler
//
// Returns FSV_SUCCESS or other error codes.
//
LONG WINAPI FsvFTP_EnumDirs(
   LPCTSTR  lpszPath, ERROR_CALLBACK lpCrit, DWORD dwCrit );
typedef LONG (WINAPI * LPFNFSVFTP_ENUMDIRS)( LPCTSTR, ERROR_CALLBACK, DWORD );

//============================================================================
// FsvFTP_HasSubDir: BOOL
//
// Determines whether the specified FTP site directory contains sub-dirs.
//
//    lpszPath          Directory to remove (must begin with \\\FTP site\)
//
// Returns TRUE if it does contain sub-dirs.
//
BOOL WINAPI FsvFTP_HasSubDir(
   LPCTSTR  lpszPath );
typedef BOOL (WINAPI * LPFNFSVFTP_HASSUBDIR)( LPCTSTR, ERROR_CALLBACK, DWORD );

//===========================================================================
//
// FsvFTP_EnumerateDirEntries( lpPath, lpFileSpec, wOptions,
//                             lpEnumDirEntryCallback, dwParam) : long;
//
//    lpPath            Full path to be enumerated (no filespec).
//    lpFileSpec        File spec to match. (NULL is equivalent to *.*)
//    wOptions          Option Flags (FSV_ENUMOPTION_???)
//    lpEnumDirEntryCallback  Callback function to be called with each enumeration.
//    dwParam           Application supplied parameter to be passed to the
//                      callback function.
//
// This function will enumerate the specified dir entries within a given FTP
// path. If the callback function returns FSV_ENUM_ABORT, then the enumeration
// process is aborted. If the FSV_ENUMOPTION_POPUPNOTIFY flag is set, then the
// callback function is called with a NULL DIRENTRYSTRUCT once for each popup.
// If FSV_ENUMOPTION_ZIPS is specified, then a ZIP/ARC/PAK file does not appear
// as a file - only as a directory with the FSV_ATTRIB_ZIP flag set.
//
// Returns: Number of Files "Added" to the count, or FSVERR_??? if an error occurs.
//
long WINAPI FsvFTP_EnumerateDirEntries(LPSTR lpPath,LPSTR lpFileSpec,WORD wOptions,LPFSV_ENUMDIRENTRYCALLBACK lpCallback,DWORD dwParam);
typedef long (WINAPI * LPFNFSVFTP_ENUMERATEDIRENTRIES)(LPSTR,LPSTR,WORD,LPFSV_ENUMDIRENTRYCALLBACK,DWORD );

//============================================================================
// FsvFTP_FindFirstSite: LPTSTR
//
// Initializes the enum site structure and returns the first site name in
// the structure.
//
//    lpFTPSiteEnum     Enumeration structure
//
// Returns FSV_SUCCESS on success or other error codes.
//
LONG WINAPI FsvFTP_FindFirstSite(
   LPFSV_FTP_ENUMSTRUCT lpSiteEnum );
typedef LONG (WINAPI * LPFNFSVFTP_FINDFIRSTSITE)( LPFSV_FTP_ENUMSTRUCT );

//============================================================================
// FsvFTP_FindNextSite: LONG
//
// Returns the next site name in the enum site structure.
//
//    lpFTPSiteEnum     Enumeration structure
//
// Returns FSV_SUCCESS on success or other error codes.
//
LONG WINAPI FsvFTP_FindNextSite(
   LPFSV_FTP_ENUMSTRUCT lpSiteEnum );
typedef LONG (WINAPI * LPFNFSVFTP_FINDNEXTSITE)( LPFSV_FTP_ENUMSTRUCT );

////////////////////////////////////////////////////////////////////////////
//  

// Fsv_FTPWriteSiteDef( hwndParent, lpFTPSite ) : BOOL
//
//    hwndParent           The handle to the parent window
//    lpFTPSite            Ptr to the FTP site definition to write to the registry
//
// This function writes the ftp site definition to the registry.
//
// Returns: TRUE if no errors occurred, FALSE otherwise
//
BOOL WINAPI FsvFTP_WriteSiteDef(
   HWND     hwndParent,
   LPFSV_FTP_SITESTRUCT   lpFTPSite );
typedef BOOL (WINAPI * LPFNFSVFTP_WRITESITEDEF)( HWND, LPFSV_FTP_SITESTRUCT);

////////////////////////////////////////////////////////////////////////////
//
// FsvFTP_GetSiteInfo: LONG
//
//    lpszSiteName         Site name to look up in the registry
//    lpFTPSite            Struct to fill with site specific info
//
// This function reads the ftp site definition from the registry and stuffs
// them into the clist that is passed in.
//
// Returns FSV_SUCCESS and valid struct on success or other error codes.
//
LONG WINAPI FsvFTP_GetSiteInfo(
   LPCTSTR lpszSiteName,
   LPFSV_FTP_SITESTRUCT lpFTPSite );
typedef LONG (WINAPI * LPFNFSVFTP_GETSITEINFO)( LPCTSTR, LPFSV_FTP_SITESTRUCT );

//*****************************************************************************
// FsvFTP_SetLogWindow( HWND hwndLogWindow )
//
// Sets the log window for the FTP output. Typically called by File Manager.

void WINAPI FsvFTP_SetLogWindow(HWND  hwndLogWindow );
typedef void (WINAPI * LPFNFSVFTP_SETLOGWINDOW)(HWND);

 // ////////////////////////////////////////////////////////////////////////////

// FsvFTP_DoCWD : int
//
//    sock     Socket number
//    lpDir    Dir to change to
//
// This function changes current dir to the specified dir on the FTP site
//
// Returns: FTP_COMPLETE on success, or other error codes
//

int FsvFTP_DoCWD(SOCKET sock,LPSTR lpDir, LPFSV_FTP_SITESTRUCT);
typedef int (WINAPI * LPFNFSVFTP_DOCWD)( SOCKET, LPSTR, LPFSV_FTP_SITESTRUCT);

// ////////////////////////////////////////////////////////////////////////////

// FsvFTP_GetEmailAddress( ) : LONG
//
//    lpszEmailAddress     The current user's email address
//    cbMax                Max bytes to copy into lpszEmailAddress
//    fQueryUser           if TRUE, will query the user for the address
//
// This function stuffs the string passed into it with the current user's
// e-mail address.  If the address is not defined, it will query the user
// for their e-mail address (if desired)
//
// Returns: FSV_SUCCESS on success, or other error codes
//
LONG WINAPI FsvFTP_GetEmailAddress(
   LPTSTR   lpszEmailAddress,
   WORD     cbMax,    
   BOOL     fQueryUser );
typedef LONG (WINAPI * LPFNFSVFTP_GETEMAILADDRESS)( LPTSTR, WORD, BOOL );

////////////////////////////////////////////////////////////////////////////
//
// FsvFTP_DisplayMessage( ) : LONG
//
//    lpszDispText         The text to display in the MessageBox
//    wButtons             The buttons to display in the MessageBox
//                         (FSVFTP_MSG_OK, FSVFTP_MSG_OKCANCEL, or
//                         FSVFTP_MSG_YESNO)
//
// This function uses a callback to call into file dialogs and display a
// messagebox with the specified text and specified buttons.
//
// Returns: FSV_SUCCESS if the first button was pressed, FSV_ERROR if the
// second button was pressed or the messagebox was closed.
//
LONG WINAPI FsvFTP_DisplayMessage( LPTSTR lpszDispText, WORD wButtons );
typedef LONG (WINAPI * LPFNFSVFTP_DISPLAYMESSAGE)( LPTSTR, WORD );

//============================================================================
// FsvUtil_ExtractFTPSite: LPSTR
//
// Takes a FTP path (lpPath) and extracts the site name in the lpSite buffer
//
// Returns: Pointer to first character after the site name
//
LPSTR WINAPI FsvUtil_ExtractFTPSite( LPTSTR lpPath, LPTSTR lpSite );
typedef LPSTR (WINAPI *LPFNFSVUTIL_EXTRACTFTPSITE)( LPTSTR, LPTSTR );

//============================================================================
// FsvUtil_GetLongPathName: LONG
//
// Takes an arbitrary fully qualified pathname that exists and returns an
// equivalent fully qualified pathname using all long filenames and pathnames.
//
// Returns: FSV_ERROR on error, FSV_SUCCESS on success.
//
LONG WINAPI FsvUtil_GetLongPathName(
   LPCTSTR  lpszSpec,      // Pathname to convert (IN)
   LPTSTR   lpszLongSpec,  // Converted pathname (OUT)
   BOOL     bFmtForDisp    // TRUE to format for display (???) (IN)
);
typedef LONG (WINAPI * LPFNFSVUTIL_GETLONGPATHNAME)( LPCTSTR, LPTSTR, BOOL );

//============================================================================
// Fsv_SetCallbackInfo( LPVOID, LPVOID ) : VOID;
//
//    LPVOID         Ptr to FTP callback in File Dialogs
//    LPVOID         Ptr to parent window
//
// This function sets the global callback and hwndParent for the 
// callback used by the FTP code.  This was done to give the FTP code a
// way to get back to file dialogs to handle errors and querying for
// passwords and such.
//
VOID WINAPI Fsv_SetCallbackInfo(
   LPVOID lpFDlgCallback,
   LPVOID lpFDlgCallbackData );
typedef VOID (WINAPI * LPFNFSV_SETCALLBACK)( LPVOID, LPVOID );

//============================================================================
// FsvUtil_IsPathShared: BOOL
//
// This function returns a TRUE/FALSE value indicating whether the specified
// path is shared or not.
//
BOOL WINAPI FsvUtil_IsPathShared(
   LPCTSTR  lpszPath );
typedef BOOL (WINAPI * LPFNFSVUTIL_ISPATHSHARED)( LPCTSTR );

//============================================================================
// FsvIO_CreateLink: long
//
// Creates a link to the source object in the specified target path, using the
// specified description.
//
// Returns: OLE result codes.
//
LONG WINAPI FsvIO_CreateLink(
   LPCTSTR  lpszPathObj,
   LPCTSTR  lpszCmdLine,      // may be NULL
   LPTSTR   lpszPathLink, 
   LPTSTR   lpszDesc,
   LPTSTR   lpszWorkDir
);
typedef LONG (WINAPI * LPFNFSVIO_CREATELINK)( LPCTSTR, LPCTSTR, LPTSTR, LPTSTR, LPTSTR );

//============================================================================
// FsvIO_GetLinkInfo: LONG
//
// Retrieves information about the specified link object.
//
// Returns: OLE result codes.
//
LONG WINAPI FsvIO_GetLinkInfo(
   LPCTSTR  lpszLinkName,     // IN:  Path\filename of link object whose information is desired.
   LPTSTR   lpszTarget,       // OUT: Path\filename of target object (may be NULL).
   LPFSV_FINDLD  lpfsvFindLD, // OUT: Pointer to WIN32_FIND_DATA (may be NULL).
   LPBOOL   lpfFileSys,       // OUT: Is target a file system object? (may be NULL but if not, return value is meaningless if lpszTarget is NULL).
   int      cbTarget,         // IN:  Size of lpszTarget.
   LPTSTR   lpszCmdLine,      // OUT: Command line arguments (may be NULL).
   int      cbCmdLine,        // IN:  Size of lpszCmdLine.
   LPTSTR   lpszDesc,         // OUT: Description string (may be NULL).
   int      cbDesc,           // IN:  Size of lpszDesc.
   LPTSTR   lpszWorkDir,      // OUT: Working directory (may be NULL).
   int      cbWorkDir         // IN:  Size of lpszWorkDir.
);
typedef LONG (WINAPI * LPFNFSVIO_GETLINKINFO)( LPCTSTR, LPTSTR, LPFSV_FINDLD, int, LPTSTR, int, LPTSTR, int LPTSTR, int );

//============================================================================
// FsvUtil_ShareDialog: LONG
//
// Displays sharing dialog for the specified folder.
//
LONG WINAPI FsvUtil_ShareDialog(
   HWND     hwndParent,
   LPCTSTR  lpszFolder );
typedef LONG (WINAPI * LPFNFSVUTIL_SHAREDIALOG)( HWND, LPCTSTR );

//============================================================================
// FsvUtil_AddConnection: LONG
//
// Attempts to connect to the specified network resource.
//
// Returns: Positive value if connected, or connect successful.
//          Negative value if connect unsuccessful.
//          Zero if not a network resource.
//
LONG WINAPI FsvUtil_AddConnection(
   LPTSTR   lpszRemoteName,   // in:  UNC path to connect to.
   LPTSTR   lpszUserName,     // in:  User name to connect with (may be NULL).
   LPTSTR   lpszPassword,     // in:  Password to use (may be NULL).
   LPTSTR   lpszLocalName,    // in:  Local device to redirect connection to (may be NULL).
   LONG     lFsvErr,          // in:  GetLastError() error value from attempting to access unconnected network resource.
   BOOL     fRoot             // in:  Connect to root of UNC path?
);
typedef LONG (WINAPI * LPFNFSVUTIL_ADDCONNECTION)( LPTSTR, LPTSTR, LPTSTR, LPTSTR, LONG, BOOL );

//=============================================================================
// FsvUtil_IsFileOpen: BOOL
//
// Determines whether or not the specified file is in an opened state.
//
BOOL WINAPI FsvUtil_IsFileOpen(
   LPCTSTR  lpszFullName
);
typedef BOOL (WINAPI * LPFNFSVUTIL_ISFILEOPEN)( LPCTSTR );

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif
