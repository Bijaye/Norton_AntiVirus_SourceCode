/* IAVVXD1C.H - include file for VxD IAVVXD1 and its customers */

#include "common.h"

#pragma pack(1)

/* How to make a Control Code; taken from WinIoctl.H */
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    (((DWORD)(DeviceType)) << 16) | (((DWORD)(Access)) << 14) | (((DWORD)(Function)) << 2) | (Method) \
    )

/* Device type           -- in the "User Defined" range from 0x8000 - 0xFFFF */
#define IAVVXD1_TYPE 0x911C

/* The IOCTL function codes from 0x800 to 0xFFF are for customer use. */
#define IOCTL_IAVVXD1_BASE (0x9A0)

/* IOCTL_IAVVXD1_AYT - Are You There, and version information */
#define IOCTL_IAVVXD1_AYT \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+0, 0, 0 )

/* IOCTL_IAVVXD1_MEMCPY - A memcpy that can see everywhere */
#define IOCTL_IAVVXD1_MEMCPY \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+1, 0, 0 )

/* IOCTL_IAVVXD1_FC - Turn check-on-open on and off */
#define IOCTL_IAVVXD1_FC \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+2, 0, 0 )

/* IOCTL_IAVVXD1_DT - Turn diskette-boot check on and off */
#define IOCTL_IAVVXD1_DT \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+3, 0, 0 )

/* IOCTL_IAVVXD1_TEST - Test IOCTL for debugging */
#define IOCTL_IAVVXD1_TEST \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+4, 0, 0 )

/* IOCTL_IAVVXD1_QUERY_STATUS - Query drive params */
#define IOCTL_IAVVXD1_QUERY_PARAMS \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+5, 0, 0 )

typedef struct _DRIVE_PARAMS {
        byte fError ;
        byte bStatus ;
        byte bType ;
        byte nDrives ;
        word nHeads ;
        word nCylinders ;
        word nSectorsPerTrack ;
        } DRIVE_PARAMS, *PDRIVE_PARAMS ;


/* IOCTL_IAVVXD1_READ_SECTORS - read sectors */
#define IOCTL_IAVVXD1_READ_SECTORS \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+6, 0, 0 )

typedef struct _DRIVE_IO {
        byte bDrive ;
        byte fError ;
        byte bStatus ;
        byte bHead ;
        word wCylinder ;
        byte bSector ;
        byte nSectors ;
        dword sBuffer ;
        byte buffer[1024] ; /* To support NEC 1.2MB diskettes */
        } DRIVE_IO, *PDRIVE_IO ;

/* IOCTL_IAVVXD1_WRITE_SECTORS - read sectors */
#define IOCTL_IAVVXD1_WRITE_SECTORS \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+7, 0, 0 )

/*  IOCTL_IAVVXD1_LOCK_PHYSICAL - lock physical unit */
#define IOCTL_IAVVXD1_LOCK_PHYSICAL \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+8, 0, 0 )

typedef struct _DRIVE_LOCK
        {
        byte bDrive ;
        byte bLevel ;
        word wPermissions ;
#define IAVVXD1_PERMISSION_NOWRITE              0x0000
#define IAVVXD1_PERMISSION_WRITE                0x0001
#define IAVVXD1_PERMISSION_NOFILEMAP            0x0002
#define IAVVXD1_PERMISSION_FILEMAP              0x0000
#define IAVVXD1_PERMISSION_NOFORMAT             0x0000
#define IAVVXD1_PERMISSION_FORMAT               0x0004
        } DRIVE_LOCK, *PDRIVE_LOCK ;

/*  IOCTL_IAVVXD1_UNLOCK_PHYSICAL - lock physical unit */
#define IOCTL_IAVVXD1_UNLOCK_PHYSICAL \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+9, 0, 0 )

/*  IOCTL_IAVVXD1_VSTAYT - do INT21 to DOS TSR */
#define IOCTL_IAVVXD1_VSTAYT \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+10, 0, 0 )

/*  IOCTL_IAVVXD1_REPROF - re-read AUTORUN.PRF for shield on/offs */
#define IOCTL_IAVVXD1_REPROF \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+11, 0, 0 )

/* IOCTL_IAVVXD1_SET_DISKETTE_SHIELD - Turn on/off diskette shield */
#define IOCTL_IAVVXD1_SET_DISKETTE_SHIELD \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+12, 0, 0 )

/* IOCTL_IAVVXD1_SET_CHECK_ON_OPEN - Turn on/off check on open */
#define IOCTL_IAVVXD1_SET_CHECK_ON_OPEN \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+14, 0, 0 )

/*  IOCTL_IAVVXD1_RESIG - re-read signature files */
#define IOCTL_IAVVXD1_RESIG \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+15, 0, 0 )

/*  IOCTL_IAVVXD1_WIN32EVT_HDL - Get a win32 event handle for Tray Icon Program */
#define IOCTL_IAVVXD1_WIN32EVT_HDL \
        CTL_CODE( IAVVXD1_TYPE, IOCTL_IAVVXD1_BASE+16, 0, 0 )

#pragma pack()


// ----------------
// File I/O stuff
// ----------------


// ---------------------------------
// Data structures for buffered I/O
// --------------------------------

// 16k file buffers
#ifdef T_VXD

 #ifdef WIN40
  #define FILEBUFFER_BUFFER 0x4000
 #else
  #define FILEBUFFER_BUFFER 0x1000
 #endif

#else

 #if defined(T_DOS)
  #define FILEBUFFER_BUFFER 0x1000
 #else
  #define FILEBUFFER_BUFFER 0x4000
 #endif

#endif

// Maximum number of buffers
#define MAX_FILE_BUFFERS 4


typedef struct _FileBufferData
{
  unsigned int  buffer_size;         /* file size                   */
  unsigned char *pfilebuffer;        /* pointer to file buffer      */
  dword         bufferstartposition; /* current file start position */
  dword         bufferendposition;   /* current file end position   */
} FILEBUFFERDATA;

typedef struct _FileData
{

  int file_handle;          /* file handle               */
  long file_size;           /* file size                 */
  int buffer_to_flush;      /* current buffer to flush   */
  int file_written_to;      /* file written to indicator */
  int NumBuffers;           /* Number of buffers         */

  FILEBUFFERDATA FileBufferArray[MAX_FILE_BUFFERS];  /* array of buffer info. structs */

} FILEDATA, *PFILEDATA ;


typedef struct _wdmac_handles
{

#ifdef T_VXD
 #ifdef WIN40
   pFileIOTable  pFIOT;
 #endif
#endif

  int           handle;
  PFILEDATA     pFileData;

} wdmac_handles, *pwdmac_handles;

/* END IAVVXD1C.H - include file for VxD IAVVXD1 and its customers */
