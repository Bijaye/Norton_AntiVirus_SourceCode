
/*----------------------------------------------------------------------*
 * DSK_STAC.C                                                           *
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/DSK_STAC.C_v   1.2   10 Aug 1997 15:22:20   bsobel  $
 *                                                                      *
 * Contains Thunked calls for support of Stacker Compressed drives      *
 *                                                                      *
 * Copyright 1994 by Symantec Corporation                               *
 *----------------------------------------------------------------------*/

// $Log:   S:/SYMKRNL/VCS/DSK_STAC.C_v  $
// 
//    Rev 1.2   10 Aug 1997 15:22:20   bsobel
// Changed to dynamically load thunk dll on win32 only when accessed for the first time.
// 
//    Rev 1.1   19 Mar 1997 21:47:50   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.0   26 Jan 1996 20:22:08   JREARDON
// Initial revision.
// 
//    Rev 1.3   13 Dec 1995 14:06:54   DBUCHES
// 32 Bit FAT work in progress.
//
//    Rev 1.2   05 Jan 1995 18:17:10   DBUCHES
// Fixed StackerFlushDrive().
//
//    Rev 1.1   04 Jan 1995 16:43:54   RGLENN
// Added StackerFlushDrive()
//
//    Rev 1.0   14 Nov 1994 21:06:08   DBUCHES
// Initial revision.

#include <dos.h>
#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "cdisk.h"
#include "quakever.h"

#if defined(SYM_WIN)                    // Only valid for WIN
#if defined(SYM_DEBUG)
    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut) OutputDebugString(QMODULE_SYMKRNL": "szOut)
    #endif
#else
    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut) 
    #endif
#endif  
#endif


#define REAL_MODE(dw)       HIWORD(dw)
#define PROT_MODE(dw)       LOWORD(dw)

#define asm _asm

#define RotateLeftBYTE(b) (b = (BYTE)((b >= 0x80) ? ((b << 1) | 1) : (b << 1)))

/*----------------10-14-92 04:32pm------------------
** Defines
** ------------------------------------------------*/
#define RESET_DISK                          0x0D
#define FIND_FIRST_FCB                      0x11
#define FIND_NEXT_FCB                       0x12
#define GET_DRIVE_PARAMETER_BLOCK           0x32
#define GET_BOOT_DRIVE_CODE                 0x3305
#define GET_FREE_DISK_SPACE                 0x36
#define SET_DIRECTORY                       0x3B
#define OPEN_FILE                           0x3D
#define CLOSE_FILE                          0x3E
#define READ_FILE                           0x3F
#define DELETE_FILE                         0x41
#define SET_FILE_ATTRIBUTES                 0x4301
#define DRIVER_IOCTL_READ                   0x4404
#define DRIVER_IOCTL_WRITE                  0x4405
#define BLOCK_DEVICE_MEDIA_REMOVEABLE       0x4408
#define BLOCK_DEVICE_LOCAL_OR_REMOTE        0x4409
#define GET_LOGICAL_DRIVE_MAP               0x440E
#define GET_CURRENT_DIRECTORY               0x47
#define FIND_FIRST                          0x4E
#define GET_DISK_LIST                       0x52

#define LFL_TASK_OK                         0x01  // Flag in perunit area
#define CD_LPC                              0x01  // Flag in CA

#define DOS_NO_ERROR                        0x00
#define DOS_INVALID_FUNCTION                0x01 // DOS file sharing error
#define DOS_FILE_NOT_FOUND                  0x02
#define DOS_PATH_NOT_FOUND                  0x03
#define DOS_ACCESS_DENIED                   0x05
#define DOS_INVALID_HANDLE                  0x06

#define IO_SIGNATURE                        0xCD // into signi field as check
#define IO_imageio                          1    // opcode=read/write of img fil

#define MAX_CV_BYTE_XFER_COUNT              0x8000 // max 32K per disk io

#define FAT_SECTORS_PER_XFAT_SEGMENT        3
#define CMAP_SECTORS_PER_XFAT_SEGMENT       6
#define TOTAL_SECTORS_PER_XFAT_SEGMENT      9
#define CONTROL_SECTOR                      0

#define SDEFRAG_ERROR_CODE_START 147    // Don't enable quite yet...

enum ERROR_CODES
        {
        ERROR_NO_ERROR,                 // 0 no error -- CHKDSK is clean
        SD_DONE,                        // 1 no error, but break out of loop
        ERR_INSUFFICIENT_DISK,          // 2 not enough disk space
        ERR_INTERNAL,                   // 3 internal consistency error
        ERR_TOO_MANY_DIR,               // 4 too many directories

        ERR_DISK_WRITE,                 // 5 unable to write
        ERR_DISK_READ,                  // 6 unable to read
        WT_DIRTY,                               // 7 WT_FLUSH_DIR flush dirty dir buffer
        WT_OVERFLOW_ERR,                // 8 directory stack overflow (very rare)
        WT_DISK_ERR,                    // 9 disk error during tree walk

        WT_BAD_ENTRY,                   // 10 invalid cluster?
        CHKDSK_BAD_DISK,                // 11 disk problem, couldn't even start
        CHKDSK_XLINK,                   // 12 cross linked files
        CHKDSK_LOST_CLUSTER,            // 13 lost clusters
        CHKDSK_SIZE_MISMATCH,           // 14 file size mismatch

        CHKDSK_BAD_FILE_ENTRY,          // 15 invalid cluster entry in a file
        CHKDSK_BAD_ENTRY,               // 16 invalid cluster entry (e.g., 1)
        CHKDSK_BAD_DOT,                 // 17 wrong cluster # in '.'
        CHKDSK_BAD_DOTDOT,              // 18 wrong cluster # in '..'
        UNABLE_TO_READ_BOOT_SECTOR,     // 19

        INVALID_DRIVE_SPECIFICATION,    // 20
        CURRENT_DIRECTORY_ERROR,        // 21
        HOST_BOOT_SECTOR_ERROR,         // 22
        SWAP_MAP_CREATION_ERROR,        // 23
        FILE_OPEN_ERROR,                // 24

        CORRUPT_VERSION_NUMBER,         // 25
        IOCTL_INVALID_FUNCTION,         // 26
        IOCTL_ACCESS_DENIED,            // 27
        IOCTL_INVALID_HANDLE,           // 28
        NON_STANDARD_ERROR,             // 29 Special for custom error messages

        SD_ABORT,                       // 30 Code for Ctrl-C, Ctrl-Break, Escape
        WX_INCLUDE,                     // 31
        WX_DONT_INCLUDE,                // 32
        WRITE_PROTECT_ERR,              // 33
        UNKNOWN_UNIT_ERR,               // 34

        DRIVE_NOT_READY_ERR,            // 35
        UNKNOWN_COMMAND_ERR,            // 36
        DATA_ERR,                       // 37
        BAD_STRUCT_LENGTH_ERR,          // 38
        SEEK_ERROR_ERR,                 // 39

        UNKNOWN_MEDIA_TYPE_ERR,         // 40
        SECTOR_NOT_FOUND_ERR,           // 41
        PRINTER_OUT_OF_PAPER_ERR,       // 42
        WRITE_FAULT_ERR,                // 43
        READ_FAULT_ERR,                 // 44

        GENERAL_FAILURE_ERR     ,       // 45
        UNABLE_TO_FIND_DPB_ERR,         // 46
        INCOMPATIBLE_STACKER_VERSION,   // 47 Driver is incompatible with stacvol ver.
        STACKER_NOT_ACTIVE,             // 48 Driver is not active.
        INVALID_FAT_COUNT,              // 49 Invalid number of FATs.
        FAT_SIZE_MISMATCH,              // 50 FAT size is incorrect.
        OUT_OF_MEMORY,                  // 51 Out of memory.
        };

#define asm _asm

/*----------------10-14-92 04:32pm------------------
** Structures
** ------------------------------------------------*/
typedef struct IO_CTL_struct
    {
    BYTE        IOsig;      /* signature = IO_SIGNATURE  */
    BYTE        IOopcode;   /* what operation to perform */
    UINT        IOscnt;     /* sector count */
    DWORD       IOsnum;     /* sector number */
    LPVOID      IObuffer;   /* buffer address */
    } IO_CTL;




// Local function prototypes & shared data.
#if !defined(SYM_WIN32)
static VOID FindStackerCA(VOID);
static LPPERUNIT StackerGetUnitDataPtr(BYTE drive);
static WORD  wStackerCASeg;      // Real mode segment of CA
static EXE_DRIVER far *lpStackerCA;
static WORD  wCommAreaSelector;  // Used to store a CommArea Selector
static void PASCAL StackerFreeSelector(VOID);
static void PASCAL StackerAllocateSelector(VOID);
#endif



BOOL SYM_EXPORT WINAPI StackerGetUnitData(BYTE drive, LPPERUNIT lpPerUnit)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC  lpfn_StackerGetUnitData;
    extern HINSTANCE    hInstThunkDll;

    if (lpfn_StackerGetUnitData == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
	        lpfn_StackerGetUnitData = GetProcAddress(hInstThunkDll,"StackerGetUnitData32" );
            if (lpfn_StackerGetUnitData == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfn_StackerGetUnitData( drive, lpPerUnit) );
#endif // _M_ALPHA
#else

    LPPERUNIT lpPerUnit16=NULL;

    StackerAllocateSelector();

    if((lpPerUnit16 = StackerGetUnitDataPtr(drive)) == NULL)
        {
        StackerFreeSelector();
        return(FALSE);
        }

    *lpPerUnit = *lpPerUnit16;

    StackerFreeSelector();

    return(TRUE);
#endif
}

BOOL SYM_EXPORT WINAPI StackerPutUnitData(BYTE drive, LPPERUNIT lpPerUnit)

{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC  lpfn_StackerPutUnitData;
    extern HINSTANCE    hInstThunkDll;

    if (lpfn_StackerPutUnitData == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
	        lpfn_StackerPutUnitData = GetProcAddress(hInstThunkDll,"StackerPutUnitData32" );
            if (lpfn_StackerPutUnitData == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfn_StackerPutUnitData( drive, lpPerUnit) );
#endif // _M_ALPHA
#else

    LPPERUNIT   lpPerUnit16;

    StackerAllocateSelector();

    if((lpPerUnit16 = StackerGetUnitDataPtr(drive)) == NULL)
        {
        StackerFreeSelector();
        return(FALSE);
        }

    *lpPerUnit16 = *lpPerUnit;

    StackerFreeSelector();

    return(TRUE);
#endif
}




/*=======================================================================
==
==  STK20DiskBlockDeviceThunk()
==
  =======================================================================
**
** DESCRIPTION:
**      This routine will do an absolute sector read or write on a
** Stacker compressed volume file (CVF).  The command var should
** be either BLOCK_READ or BLOCK_WRITE as defiened in disk.h
**
** RETURNS:
**      0 = Success.
**      1 = Error.
**
 *=====================================================================*/

#pragma optimize("leg", off)

UINT SYM_EXPORT WINAPI STK20DiskBlockDeviceThunk(LPCFATINFOREC lpCFAT, UINT command, LPDISKREC diskPtr)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC  lpfn_STK20DiskBlockDeviceThunk;
    extern HINSTANCE    hInstThunkDll;

    if (lpfn_STK20DiskBlockDeviceThunk == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
            lpfn_STK20DiskBlockDeviceThunk = GetProcAddress(hInstThunkDll,"STK20DiskBlockDeviceThunk32" );
            if (lpfn_STK20DiskBlockDeviceThunk == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfn_STK20DiskBlockDeviceThunk( command, diskPtr ) );
#endif // _M_ALPHA
#else

    WORD    mode;
    WORD    byteCount;
    WORD    numSectorsThisPass;
    WORD    wReturnValue;
    DWORD   sectorCount             = diskPtr->sectorCount;
    BYTE    driveLetter             = diskPtr->dl;
    DWORD   startingSectorNumber    = diskPtr->sector;
    BYTE    huge *writeDataBuffer   = (BYTE huge *)diskPtr->buffer;
    WORD    ioctlSize               = sizeof(IO_CTL);
    WORD    ioctlError              = DOS_NO_ERROR;
    BYTE    driveNumber             = (driveLetter - (BYTE)'A') + (BYTE)1;
    WORD    maxSectorsPerXfer       = MAX_CV_BYTE_XFER_COUNT / diskPtr->bytesPerSector;
#ifdef SYM_PROTMODE
    LPBYTE  lpbyNormilizedPtr       = NULL;
    DWORD   dwDosBuffer             = 0;
    LPBYTE  lpDosBufferREAL;
    LPBYTE  lpDosBufferPROT;
    DWORD   dwIoctl;
    IO_CTL  far *lpIoctlREAL;
    IO_CTL  far *lpIoctlPROT;
    union   REGS regs;
    struct  SREGS sregs;
#else
    IO_CTL  IoctlPacket;
    IO_CTL  far *lpIoctlPROT = &IoctlPacket;    // I know the variable name
                                                // ends in PROT, but when
                                                // compiled for the DOS
                                                // platform the variable
                                                // holds a segment:offset.
                                                // This seemed easier than
                                                // adding even more #ifdef's.
#endif

    if(command == BLOCK_WRITE)
        mode = DRIVER_IOCTL_WRITE;
    else if(command == BLOCK_READ)
        mode = DRIVER_IOCTL_READ;
    else
        return(SYM_ERROR_INVALID_FUNCTION);         // Unsupported mode

#ifdef SYM_PROTMODE
    // Allocate the largest possible real mode buffer
    maxSectorsPerXfer = (WORD)min(sectorCount, maxSectorsPerXfer);

    while (maxSectorsPerXfer > 0)
        {
        dwDosBuffer = GlobalDosAlloc(maxSectorsPerXfer * diskPtr->bytesPerSector);

        if (dwDosBuffer == 0)
            maxSectorsPerXfer /= 2;
        else
            break;
        }

    if (dwDosBuffer == 0)
        return(SYM_ERROR_CRC_ERROR); // ?

    lpDosBufferREAL = MAKELP(REAL_MODE(dwDosBuffer), 0);
    lpDosBufferPROT = MAKELP(PROT_MODE(dwDosBuffer), 0);

    // Allocate a real mode IOCTL packet
    if ((dwIoctl = GlobalDosAlloc(sizeof(IO_CTL))) == 0)
        {
        GlobalDosFree(PROT_MODE(dwDosBuffer));
        return(SYM_ERROR_CRC_ERROR);
        }
    lpIoctlREAL = (IO_CTL far *) MAKELP(REAL_MODE(dwIoctl), 0);
    lpIoctlPROT = (IO_CTL far *) MAKELP(PROT_MODE(dwIoctl), 0);
#endif

    lpIoctlPROT->IOsig = IO_SIGNATURE;       /* signature */
    lpIoctlPROT->IOopcode = IO_imageio;      /* operation to perform */

    wReturnValue = SYM_ERROR_NO_ERROR;

    while(sectorCount != 0)
        {
        numSectorsThisPass = (WORD)min(sectorCount, maxSectorsPerXfer);
        byteCount = diskPtr->bytesPerSector * numSectorsThisPass;

        lpIoctlPROT->IOscnt = numSectorsThisPass;
        lpIoctlPROT->IOsnum = startingSectorNumber;    /* sector number */

#ifdef SYM_PROTMODE
        lpbyNormilizedPtr = MemoryNormalizeAddress(writeDataBuffer);

        if(lpbyNormilizedPtr==NULL)
            {
            wReturnValue = SYM_ERROR_CRC_ERROR; // ?
            break;
            }

        // If writing, copy data from protected to real mode buffer
        if (command == BLOCK_WRITE)
            MEMCPY(lpDosBufferPROT, lpbyNormilizedPtr, byteCount);

        lpIoctlPROT->IObuffer = lpDosBufferREAL;

        MEMSET(&regs, 0, sizeof(regs));
        MEMSET(&sregs, 0, sizeof(sregs));

        regs.x.ax = mode;
        regs.h.bl = driveNumber;
        regs.x.cx = byteCount;
        sregs.ds = REAL_MODE(dwIoctl);
        regs.x.dx = 0x0000;

        IntWin((BYTE)0x21, &regs, &sregs);

        if (regs.x.cflag & 0x01)
            ioctlError = regs.x.ax;
        else
            ioctlError = 0;
#else
        lpIoctlPROT->IObuffer = writeDataBuffer;
        ioctlError = 0;

        asm     push    ds;
        asm     push    bp;

        asm     mov     ax,[mode];
        asm     mov     bl,[driveNumber];
        asm     mov     cx,[byteCount];
        asm     lds     dx,[lpIoctlPROT]
        asm     int     0x21;

        asm     pop     bp;
        asm     pop     ds;
        asm     jnc     WCVS_OK;                    /* carry set == error */
        asm     mov     [ioctlError],ax
WCVS_OK:
#endif

        if (ioctlError == DOS_INVALID_FUNCTION)
            {
            // errorCode = IOCTL_INVALID_FUNCTION;
            wReturnValue = SYM_ERROR_INVALID_FUNCTION;
            break;
            }
        else if (ioctlError == DOS_ACCESS_DENIED)
            {
            // errorCode = IOCTL_ACCESS_DENIED;
            wReturnValue = SYM_ERROR_WRITE_PROTECTED;
            break;
            }
        else if (ioctlError != 0)
            {
            // errorCode = IOCTL_INVALID_HANDLE;
            wReturnValue = SYM_ERROR_CRC_ERROR; // ?
            break;
            }

#ifdef SYM_PROTMODE
        // If reading, copy data from real to protected mode buffer
        if (command == BLOCK_READ)
            MEMCPY(lpbyNormilizedPtr, lpDosBufferPROT, byteCount);
#endif

        sectorCount -= numSectorsThisPass;
        startingSectorNumber += numSectorsThisPass;
        writeDataBuffer += byteCount;

#ifdef SYM_PROTMODE
        FreeNormalizedAddress(lpbyNormilizedPtr);
        lpbyNormilizedPtr=NULL;
#endif
        } // End of while loop

#ifdef SYM_PROTMODE
    // Free real mode buffers
    GlobalDosFree(PROT_MODE(dwDosBuffer));
    GlobalDosFree(PROT_MODE(dwIoctl));

    if(lpbyNormilizedPtr)
        FreeNormalizedAddress(lpbyNormilizedPtr);
#endif

    return(wReturnValue);

#endif //SYM_WIN32
}


#pragma optimize("", on)





/****************************************************************************
*
* Function Name:    StackerFlushDrive
*
* Function:         Flush a Stacker Drive.
*
-****************************************************************************/

#pragma optimize("leg", off)

BOOL SYM_EXPORT WINAPI StackerFlushDrive(BYTE drive)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC  lpfn_StackerFlushDrive;
    extern HINSTANCE    hInstThunkDll;

    if (lpfn_StackerFlushDrive == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
	        lpfn_StackerFlushDrive = GetProcAddress(hInstThunkDll,"StackerFlushDrive32" );
            if (lpfn_StackerFlushDrive == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfn_StackerFlushDrive( drive ) );
#endif // _M_ALPHA    
#else

    auto int    temp[8];

    if(drive >= 'A')
        drive = (drive-'A') & 0x1f;

    asm mov     ax,0x4404
    asm mov     cx,4
    asm xor     bh,bh
    asm mov     bl,drive
    asm inc     bx
    asm lea     dx,temp
    asm push    ds
    asm push    ss
    asm pop     ds
    asm int     0x21                    // IOCTL flush
    asm pop     ds

    return(TRUE);
#endif
}

#pragma optimize("", on)


#if !defined (SYM_WIN32)

/****************************************************************************
*
* Function Name:    StackerGetUnitData
*
* Function:         See if a drive is Stacker and return perunit if so
*
-****************************************************************************/

#pragma optimize("leg", off)

static LPPERUNIT StackerGetUnitDataPtr(BYTE drive)
{
    auto        int             temp[256];
    auto        int             j;
    auto        BYTE            unitNum[4];
    auto        int             axReg=0x4408;
    auto        unsigned far   *units;
    auto        EXE_DRIVER far *lpTStackerCA;
    auto        WORD            wTStackerCASeg;

    if(drive >= 'A')
        drive = (drive-'A') & 0x1f;

    FindStackerCA();

    if (lpStackerCA == NULL)
        return(NULL);

    units = (unsigned far *) MAKELP(FP_SEG(lpStackerCA), lpStackerCA->CAunits);

    if (DOSGetVersion() == DOS_3_31)
        axReg = 0x440E; // Work around DRDOS bug

    for (j = 0; j < 4; j++)
        {
        lpStackerCA->CA_boot_ID.ID_lastUnit = 0xFF;

        asm     mov     ax,axReg
        asm     xor     bh,bh
        asm     mov     bl,drive
        asm     inc     bx
        asm     int     0x21

        unitNum[j] = lpStackerCA->CA_boot_ID.ID_lastUnit;
        if ((unitNum[j] == 0xFF) || ((j>0) && (unitNum[j]!=unitNum[j-1])))
            return NULL;
        }

    if (units[unitNum[0]] == 0)         // dummy unit?
        return NULL;

    MEMSET(temp,0,sizeof(temp));

    lpTStackerCA = lpStackerCA;
    wTStackerCASeg = wStackerCASeg;

    asm mov     ax,0x4404               // try ioctl flush as a check...
    asm mov     cx,4
    asm xor     bh,bh
    asm mov     bl,drive
    asm inc     bx
    asm lea     dx,temp
    asm push    ds
    asm push    ss
    asm pop     ds
    asm int     0x21
    asm pop     ds
    asm mov     ax,[temp]
    asm cmp     ax,word ptr [lpTStackerCA]
    asm jne     NotFound
    asm mov     ax,[temp+2]
#ifdef SYM_PROTMODE
    // Compare real mode segment, not protected mode selector
    asm cmp     ax,[wTStackerCASeg]
#else
    asm cmp     ax,word ptr [lpTStackerCA+2]
#endif
    asm jne     NotFound

    return  (LPPERUNIT) MAKELP(FP_SEG(lpStackerCA), units[unitNum[0]]);
NotFound:
    return(NULL);
}

#pragma optimize("", on)

/****************************************************************************
*
* Function Name:    FindStackerCA
*
* Function:         Get address of Stacker communcations area.
*
-****************************************************************************/

#ifdef SYM_PROTMODE

static VOID FindStackerCA(VOID)
{
    struct int25rec
        {
        unsigned signature;     // must be STACKER_INT25_SIG on return
        unsigned cnt;           // must be same as value passed in CX to INT 25
        EXE_DRIVER far *captr;
        char pad[1024];         // temp buffer (just in case INT 25 tries to read!
        } far *lpIdRecREAL, far *lpIdRecPROT;
    EXE_DRIVER far *lpCommAreaPROT;
    DWORD dwDosBuffer;
    DPMIRec DPMI;
    BOOL bDpmiError;

    lpStackerCA = NULL;

    if ((dwDosBuffer = GlobalDosAlloc(sizeof(struct int25rec))) == 0)
        return;

    lpIdRecREAL = MAKELP(REAL_MODE(dwDosBuffer), 0);
    lpIdRecPROT = MAKELP(PROT_MODE(dwDosBuffer), 0);

    lpIdRecPROT->signature = 0; // clear fields before call to see if it works
    lpIdRecPROT->cnt       = 0xFFFF;
    lpIdRecPROT->captr     = NULL;

    MEMSET(&DPMI, 0, sizeof(DPMI));   // Must start with zero'd structure
    DPMI.eax = 0xCDCD;                  // Special drive # that Stacker uses
    DPMI.ecx = 1;                       // Read one sector
    DPMI.edx = 0;                       // Sector 0
    DPMI.ds =  FP_SEG(lpIdRecREAL);     // Buffer segment
    DPMI.ebx = FP_OFF(lpIdRecREAL);     // Buffer offset

    // Work around bug in MSDOS 3.X where the word at ES:[BP+1EH] gets
    // 0xFFFF written to it during an INT 25H.  We point ES:BP at the
    // int25rec, so the only thing trashed are some "pad" bytes that
    // we don't care about anyway.
    DPMI.es = DPMI.ds;
    DPMI.ebp = DPMI.ebx;

    DPMI.simulatedInt = 0x25;
    bDpmiError = DPMISimulateInt(&DPMI);

    if (!bDpmiError &&
        lpIdRecPROT->signature == 0xCDCD &&
        lpIdRecPROT->cnt == 1)
        {
        SetSelectorBase(wCommAreaSelector, (DWORD)FP_SEG(lpIdRecPROT->captr) * 16L);
        lpCommAreaPROT = MAKELP(wCommAreaSelector, FP_OFF(lpIdRecPROT->captr));

        if (lpCommAreaPROT->CAsignature == CA_SIGNATURE &&
            lpCommAreaPROT->CA_boot_ID.ID_ca_ptr == lpIdRecPROT->captr)
            {
            // Found the communications area!
            lpStackerCA = lpCommAreaPROT;
            wStackerCASeg = FP_SEG(lpIdRecPROT->captr);
            }
        }

    GlobalDosFree(PROT_MODE(dwDosBuffer));
    return;
}

#else  /////  DOS VERSION  /////////////////////////////////////////////////

#if 0
#pragma optimize("leg", off)

LOCAL EXE_DRIVER far *FindStackerCA(LPCFATINFOREC lpCFAT)
{
    struct int25rec
        {
        unsigned signature;     // must be STACKER_INT25_SIG on return
        unsigned cnt;           // must be same as value passed in CX to INT 25
        EXE_DRIVER far *captr;
        char pad[1024];         // temp buffer (just in case INT 25 tries to read!
        } idrec;


    // Just return address of CA if we've already found it once
    if (lpCFAT->lpStackerCA != NULL)
        return(lpCFAT->lpStackerCA);

    idrec.signature     =0;             // clear fields before call to see if it works
    idrec.cnt           =0xFFFF;
    idrec.captr         =NULL;

    asm push ds
    asm mov  ax,0xCDCD          // fictitious drive
    asm lea  bx,idrec           // read to here
    asm mov  cx,1               // one sector only
    asm xor  dx,dx              // read boot sector
    asm push bp                 // save BP
    asm push si
    asm push di
    asm push ss
    asm pop  ds
    asm push ss
    asm pop  es
    asm mov  bp,bx              // fix bug in DOS 3.X
    asm int  0x25               // read
    asm pop  cx                 // restore flags
    asm pop  di
    asm pop  si
    asm pop  bp                 // restore BP
    asm pop  ds

    if ((idrec.signature != 0xCDCD) || (idrec.cnt!=1) ||
        (idrec.captr->CAsignature != CA_SIGNATURE) ||
        (idrec.captr->CA_boot_ID.ID_ca_ptr != idrec.captr))
        {
        lpCFAT->lpStackerCA = NULL;
        }
    else
        {
        lpCFAT->lpStackerCA = idrec.captr;
        }

    return(lpCFAT->lpStackerCA);
}

#pragma optimize("", on)

#endif // SYM_PROTMODE

#endif

static void PASCAL StackerAllocateSelector(VOID)
{
    // NOTE: The linear memory base will be reset by FindStackerCA()
    SelectorAlloc(0x0000, 0xFFFF, &wCommAreaSelector);
}

/*----------------------------------------------------------------------*
 * StackerFreeSelector()                                               *
 *                                                                      *
 * Free the selectors allocated by StackerAllocateSelector().          *
 *----------------------------------------------------------------------*/

static void PASCAL StackerFreeSelector(VOID)
{
    if(wCommAreaSelector)
        SelectorFree(wCommAreaSelector);
    wCommAreaSelector = NULL;
}

#endif // !SYM_WIN32

