/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/DSK_GENV.C_v   1.0   26 Jan 1996 20:21:06   JREARDON  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/DSK_GENV.C_v  $ *
// 
//    Rev 1.0   26 Jan 1996 20:21:06   JREARDON
// Initial revision.
// 
//    Rev 1.1   03 Oct 1994 14:00:26   DBUCHES
// Added 32 bit version of DiskGenericIOCTL().  For now,
// it only supports the GET_DEVICE_PARAMS function.
//
//    Rev 1.0   18 Sep 1994 16:47:34   GEORGEL
// Initial revision.
//
//    Rev 1.15   09 Jun 1994 15:29:56   MARKK
// Zero regs prior to call
//
//    Rev 1.14   12 May 1994 13:44:10   BRAD
// Use DOSPrintStdAux(), rather than DOSWriteStdAux().
//
//    Rev 1.13   05 May 1994 15:56:04   MARKK
// DOSX work
//
//    Rev 1.12   04 May 1994 05:51:22   MARKK
// DOSX work
//
//    Rev 1.11   15 Mar 1994 12:33:08   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.10   25 Feb 1994 15:02:56   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.8   30 Sep 1993 10:10:08   DWHITE
// Added support for the NEC PC 9800
//
//    Rev 1.7   02 Jul 1993 08:51:54   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.6   15 Feb 1993 21:03:52   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.5   12 Feb 1993 04:27:58   ENRIQUE
// No change.
//
//    Rev 1.4   19 Oct 1992 17:01:08   BILL
// Added changes to handle const typedef
//
//    Rev 1.3   10 Sep 1992 19:05:26   BRUCE
// Deleted commented out includes
//
//    Rev 1.2   09 Sep 1992 18:18:40   BRUCE
// Eliminated dependency on nwinutil.h
//
//    Rev 1.1   08 Sep 1992 12:50:08   HENRI
// Added MODULE_NAMe;.
//
//    Rev 1.0   27 Aug 1992 10:33:42   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>
#include "platform.h"
#include "file.h"
#include "disk.h"
#include "xapi.h"

#if defined(SYM_WIN32)
#define Not_VxD
#include "vwin32.h"
#endif


MODULE_NAME;

UINT LOCAL PASCAL DiskTestSector (BYTE driveLetter, UINT wBytesPerSector);

static  UINT    ioctlErr;

#pragma optimize("", off)
/*----------------------------------------------------------------------*/
/* DiskGenericIOCTL                                                     */
/*      Calls the Generic IOCTL request function.  This function can    */
/*      read, write, verify, and format a track on a device.            */
/*      Also it sets and gets device parameters to determine            */
/*      the type of device.                                             */
/*
/*      For WIN32, the only command supported is GET_DEVICE_PARAMS      */
/*                                                                      */
/*      COMMAND_CODE is one of the following:                           */
/*              SET_DEVICE_PARAMS                                       */
/*              GET_DEVICE_PARAMS                                       */
/*              WRITE_TRACK                                             */
/*              READ_TRACK                                              */
/*              FORMAT_TRACK                                            */
/*              VERIFY_TRACK                                            */
/*                                                                      */
/* RETURNS:                                                             */
/*      TRUE            If no error                                     */
/*      FALSE           If error                                        */
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskGenericIOCTL(BYTE command, BYTE driveLetter, LPVOID paramBlockPtr)
{

#if defined(SYM_WIN32)
    DIOC_REGISTERS      r;
    HANDLE              hVWin32 = 0;
    DWORD               dwRet;
    BYTE                nDrive;

                                        // So far, get device params is the
                                        // only IOCTL supported.
    if( command != GET_DEVICE_PARAMS )
        return(FALSE);

    nDrive= (WORD) (driveLetter - 'A') + 1;

    MEMSET(&r, 0, sizeof(DIOC_REGISTERS));


    hVWin32 = CreateFile("\\\\.\\VWIN32", 0,0,0,0,0,0);

    r.reg_EAX = 0x440d;           // IOCTL
    r.reg_EBX = nDrive;           // drive (a=1)
    r.reg_ECX = 0x0800;
    r.reg_ECX |= command;         // GetDeviceParams
    r.reg_EDX = (DWORD) paramBlockPtr;

    DeviceIoControl(hVWin32, VWIN32_DIOC_DOS_IOCTL,
     &r, sizeof(r), &r, sizeof(r), &dwRet, NULL);

    CloseHandle(hVWin32);

    return(TRUE);

#else



    auto        BOOL    bReturn = FALSE;


//TODO - Add Vxd solution here
#if 0


    driveLetter = (BYTE) CharToUpper(driveLetter);

    if (DOSGetVersion() >= DOS_3_20)
        {
                                        /*------------------------------*/
                                        /* Clear the BPB record, since  */
                                        /* some versions of DOS don't   */
                                        /* fill in the full field with  */
                                        /* 0s when we do a GET DEVICE   */
                                        /* PARAMS call.                 */
                                        /*------------------------------*/
        if (command == GET_DEVICE_PARAMS)
            {
            MEMSET (&(((DeviceParamRec FAR *) paramBlockPtr)->BPB),
                                                        0, sizeof(BPBREC));
            }

                                        /*------------------------------*/
                                        /* Use DPMI for Verify track,   */
                                        /* since it's not virtualized   */
                                        /* correctly in Standard mode.  */
                                        /*------------------------------*/

        if (command == IOCTL_FORMAT_TRACK  || command == IOCTL_VERIFY_TRACK)
            {
#ifdef  SYM_PROTMODE
            auto        union REGS      regs;
            auto        struct SREGS    sregs;
            auto        DWORD           dwSegSel;
            auto        LPBYTE          lpBuf;

	    MEMSET(&regs, 0, sizeof(regs));
	    MEMSET(&sregs, 0, sizeof(sregs));
            dwSegSel = GlobalDosAlloc(sizeof(FORMATPACKET));
            if (dwSegSel == NULL)
                return (FALSE);
            lpBuf = (LPBYTE) MAKELP (dwSegSel, 0);

            MEMCPY(lpBuf, paramBlockPtr, sizeof(FORMATPACKET));


            regs.h.cl = command;        // Get the command
            regs.h.ch = 0x08;           // Category code (Major code)
            regs.h.bl = driveLetter;    // Get drive letter
            regs.h.bl -= 'A'-1;         // Get drive number
            sregs.ds = HIWORD(dwSegSel);
            regs.x.dx = 0;
            regs.x.ax = 0x440D;         // Generic IOCTL request
            IntWin(0x21, &regs, &sregs);

            GlobalDosFree(LOWORD(dwSegSel));

            if (regs.x.cflag)
                {
                regs.h.ah = 0x59;       // Get Extended Error
                regs.x.bx = 0;
                IntWin(0x21, &regs, &sregs);
                ioctlErr = regs.x.ax;   // Save it for later
                return (FALSE);
                }
            else
                {
                ioctlErr = 0;
                return (TRUE);
                }
#else
            auto        FORMATPACKET    rPacket;
            auto        LPBYTE          lpBuf = (LPBYTE) &rPacket;
            auto        BOOL            bResult = TRUE;

            MEMCPY(lpBuf, paramBlockPtr, sizeof(FORMATPACKET));

            _asm
                {
                push    ds              // save DS

                lds     dx, lpBuf
                mov     cl, command     // Get the command
                mov     ch, 8           // Category code (Major code)
                mov     bl, driveLetter // Get drive letter
                sub     bl, 'A'-1;      // Get drive number
                mov     ax, 440Dh
                DOS

                pop     ds              // restore DS

                jnc     No_Error

                mov     ah, 59h         // Get extended error
                xor     bx,bx
                DOS
                dec     bReturn         // set return value to FALSE
                jmp     short All_Done
No_Error:
                xor     ax,ax
All_Done:
                mov     ioctlErr, ax    // Save it for later
                }

            return (bReturn);
#endif
            }

#if	!defined(SYM_DOSX)
    _asm
        {
        push    ds                      ; Save registers

        mov     cl, command             ; Get the command
        mov     ch, 08h                 ; Category code (Major code)
        mov     bl, driveLetter         ; Get drive letter
        sub     bl, 'A'-1               ; Get drive number
        lds     dx, paramBlockPtr       ; Get ptr. to param block

        mov     ax, 440Dh               ; Generic IOCTL request
        DOS
        jc      errorLabel              ; If error, return error

        mov     ax, TRUE                ; Signal no error
        jmp     short locRet            ;   and return

errorLabel:
        xor     ax, ax                  ; Signal error

locRet:
        pop     ds                      ; Restore register
        mov     bReturn, ax             ; Save return value
        }
#else
	{
        auto        union REGS      regs;
        auto        struct SREGS    sregs;
        auto        DWORD           dwSegSel;
        auto        LPBYTE          lpBuf;
	auto	WORD		wSize;
	VOID    Beep (VOID);
        VOID    CDECL   DOSPrintStdaux(CONST BYTE *pszFormat, ...);

	switch (command)
	    {
	    case GET_DEVICE_PARAMS: wSize = sizeof(DeviceParamRec); break;
	    default:
		Beep();
		DOSPrintStdaux("Unsupported IOCTL function called: %d\r\n", command);
		return (FALSE);
	    }

	MEMSET(&regs, 0, sizeof(regs));
	MEMSET(&sregs, 0, sizeof(sregs));

        dwSegSel = GlobalDosAlloc(wSize);
        if (dwSegSel == NULL)
            return (FALSE);
        lpBuf = (LPBYTE) MAKELP (dwSegSel, 0);

        MEMCPY(lpBuf, paramBlockPtr, wSize);

        regs.h.cl = command;        // Get the command
        regs.h.ch = 0x08;           // Category code (Major code)
        regs.h.bl = driveLetter;    // Get drive letter
        regs.h.bl -= 'A'-1;         // Get drive number
        sregs.ds = HIWORD(dwSegSel);
        regs.x.dx = 0;
        regs.x.ax = 0x440D;         // Generic IOCTL request
        IntWin(0x21, &regs, &sregs);

        if (regs.x.cflag)
            {
            bReturn = (FALSE);
            }
        else
            {
	    MEMCPY(paramBlockPtr, lpBuf, wSize);
	    ioctlErr = 0;
            bReturn = TRUE;
	    }

	GlobalDosFree(LOWORD(dwSegSel));
	}
#endif
        }



#endif


    return(bReturn);
#endif // SYM_WIN32
}
#pragma optimize("", on)


/*----------------------------------------------------------------------*/
/* DiskGenericIOCTLErr                                                  */
/*      Returns the critical error number, if any, generated by the     */
/*      last format or verify track generic ioctl call.                 */
/*                                                                      */
/*      This routine is needed because the Windows DOSGetExtendedError  */
/*      is unreliable, esp. when paging takes place.  It would be       */
/*      better if DiskGenericIOCTL returned the critical error number   */
/*      in all cases, but as usual we're in a hurry....                 */
/*----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI DiskGenericIOCTLError (void)
{
    UINT temp;

    temp = ioctlErr;
    ioctlErr = 0;
    return (temp);
}

// Nuke 'til the end of the file


#if 0

/*----------------------------------------------------------------------*/
/* DiskTestFormat                                                       */
/*      Tests if a diskette has been formatted by using Generic IOCTL   */
/*      to read the first sector and check if it contains a valid BPB.  */
/*                                                                      */
/*      This was written for wformat.exe, which blows up in standard    */
/*      mode when it tries to use int 21h function 32h to get drive     */
/*      info on an unformatted disk.                                    */
/*                                                                      */
/*      This routine requires DOS 3.2 or later.                         */
/*                                                                      */
/*      This will be a lengthy operation if the diskette is not         */
/*      formatted (20 seconds or so).                                   */
/*                                                                      */
/* Returns:                                                             */
/*      NOERR if all went well                                          */
/*      ERR   if unable to test format on this drive                    */
/*      Else an extended error code:                                    */
/*          08h (insufficent mem)   if not enough Win or DOS memory     */
/*          15h (drive not ready)   if diskette is missing from drive   */
/*          1Fh (general failure)   if diskette is not formatted        */
/*          others                                                      */
/*----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI DiskTestFormat (BYTE byDrive, LPCBPBREC lpBPB)
{
    auto        DeviceParamRec          savedDeviceParam;
    auto        GLOBALHANDLE            hBuffer;
    auto        LPSTR                   lpBuffer;
    auto        DeviceParamRec          tempInfoPacket;
    auto        SECTORPACKET FAR*       lpSectorPacketPtr;
    auto        UINT                    wNumberOfCylinders;
    auto        UINT                    wRetValue;
    auto        UINT                    i;

#ifdef NEC
	GetDosDriverTablePtr ( byDrive );
    tempInfoPacket.specialFunctions = IOCTL_SF_GET_CURRENT;
    if (!DiskGenericIOCTL(IOCTL_GET_PARAMS, byDrive, &tempInfoPacket))
	{
	_asm
	{
	mov		ah, 59h		; Get Extended Error
	mov		bx, 0
        DOS
	mov		wRetValue, ax
	}
	return ( wRetValue );
	}

	if ( lpBPB->bytesPerSector == 0 )
		return ( ERR );
#endif // NEC



                                        /*------------------------------*/
                                        /* Get the default drevice      */
                                        /* param block.                 */
                                        /*------------------------------*/

    savedDeviceParam.specialFunctions = IOCTL_SF_GET_DEFAULT;
    if (!DiskGenericIOCTL(IOCTL_GET_PARAMS, byDrive, &savedDeviceParam))
        return ((UINT)ERR);

                                        /*------------------------------*/
                                        /* Alloc space for new device   */
                                        /* param block.                 */
                                        /*------------------------------*/

    hBuffer = MemAlloc(GMEM_MOVEABLE,
                          sizeof(DeviceParamRec) +
                          (lpBPB->sectorsPerTrack * sizeof(DWORD)));

    if (hBuffer == NULL)
        return (0x08);

    if ((lpBuffer = MemLock(hBuffer)) == NULL)
        {
        MemFree(hBuffer);
        return (0x08);
        }

                                        /*------------------------------*/
                                        /* Fill in first portion of new */
                                        /* device param block.          */
                                        /*------------------------------*/

#ifndef NEC
    tempInfoPacket = savedDeviceParam;
#endif
    tempInfoPacket.BPB = *lpBPB;
    tempInfoPacket.specialFunctions = IOCTL_SF_SET_CURRENT +
                                      IOCTL_SF_USE_ALL_PARAMS +
                                      IOCTL_SF_SAME_SIZE;

    wNumberOfCylinders = tempInfoPacket.BPB.oldTotalSectors /
      (tempInfoPacket.BPB.heads * tempInfoPacket.BPB.sectorsPerTrack);

    if (wNumberOfCylinders != tempInfoPacket.numberOfTracks)
        {
        tempInfoPacket.numberOfTracks = wNumberOfCylinders;
        tempInfoPacket.mediaType = 1;   /* Not the default media        */
        }

    tempInfoPacket.wSectorsPerTrack = tempInfoPacket.BPB.sectorsPerTrack;

    *((DeviceParamRec FAR *) lpBuffer) = tempInfoPacket;

                                        /*------------------------------*/
                                        /* Fill in sector table portion */
                                        /* of new device param block.   */
                                        /*------------------------------*/

    lpSectorPacketPtr = (SECTORPACKET FAR*) (lpBuffer + sizeof(DeviceParamRec));
    for (i = 1; i <= tempInfoPacket.BPB.sectorsPerTrack;
                                                i++, lpSectorPacketPtr++)
        {
        lpSectorPacketPtr->wSectorNumber = i;
        lpSectorPacketPtr->wSectorSize   = tempInfoPacket.BPB.bytesPerSector;
        }

                                        /*------------------------------*/
                                        /* Set new default device param */
                                        /* block, the test the first    */
                                        /* sector on the disk.          */
                                        /*------------------------------*/

    if (!DiskGenericIOCTL(IOCTL_SET_PARAMS, byDrive, lpBuffer))
        wRetValue = (UINT)ERR;
    else
        wRetValue = DiskTestSector(byDrive, tempInfoPacket.BPB.bytesPerSector);

                                        /*------------------------------*/
                                        /* Reset default device param   */
                                        /* block back to original value */
                                        /*------------------------------*/

    savedDeviceParam.specialFunctions = IOCTL_SF_SET_DEFAULT +
                                        IOCTL_SF_USE_ALL_PARAMS +
                                        IOCTL_SF_SAME_SIZE;
    savedDeviceParam.wSectorsPerTrack = 0;   /* Zero out the track data */
    DiskGenericIOCTL(IOCTL_SET_PARAMS, byDrive, &savedDeviceParam);

                                        /*------------------------------*/
                                        /* Free track buffer            */
                                        /*------------------------------*/

    MemUnlock(hBuffer, lpBuffer);
    MemFree(hBuffer);
    return (wRetValue);
}


/*----------------------------------------------------------------------*/
/* DiskTestSector()                                                     */
/*      Utility routine for DiskTestFormat().  Reads the first sector   */
/*      and makes sure it has a valid BIOS Parameter Block.             */
/*      Note that the read track IOCTL call is not virtualized, so      */
/*      we must use DPMI.                                               */
/*                                                                      */
/*      This test replaces a verify track test, which "half" formatted  */
/*      diskettes would unfortunately pass.  Also, the old test         */
/*      returned "sector not found" if there was a size mismatch (e.g.  */
/*      testing a formatted 360K disk in a 1.2M drive configured as     */
/*      1.2M).   This new test returns zero if the mismatch is a small  */
/*      diskette in a big drive, or "general failure" if it's a big     */
/*      diskette in a small drive, which works out much better.         */
/*                                                                      */
/* Returns:                                                             */
/*      0 or extended error code.                                       */
/*----------------------------------------------------------------------*/

#ifdef  SYM_PROTMODE

UINT LOCAL PASCAL DiskTestSector (BYTE driveLetter, UINT wBytesPerSector)
{
#ifndef NEC
    auto        union REGS      regs;
    auto        struct SREGS    sregs;
    auto        DWORD           dwSegSelPacket;
    auto        DWORD           dwSegSelBuffer;
    auto        READWRITETRACKPACKET FAR* lpReadPacket;
    auto        BPBBootRec FAR* lpBootBuffer;
    auto        LPBYTE          lpReadBuffer;
    auto        UINT            wRet = NOERR;

    dwSegSelPacket = GlobalDosAlloc(sizeof(READWRITETRACKPACKET));
    if (dwSegSelPacket == NULL)
        return (0x08);                  // Insufficient memory

    FP_SEG(lpReadPacket) = LOWORD(dwSegSelPacket);
    FP_OFF(lpReadPacket) = 0;

    dwSegSelBuffer = GlobalDosAlloc(wBytesPerSector);
    if (dwSegSelBuffer == NULL)
        {
        GlobalDosFree(LOWORD(dwSegSelPacket));
        return (0x08);                   // Insufficient memory
        }

    lpReadBuffer = (LPBYTE) MAKELP (dwSegSelBuffer, 0);

    lpReadPacket->bSpecialFunctions = 0;
    lpReadPacket->wHead = 0;
    lpReadPacket->wCylinder = 0;
    lpReadPacket->wFirstSector = 0;
    lpReadPacket->wNumberOfSectors = 1;

    lpReadPacket->lpTransferAddress = MAKELP(HIWORD(dwSegSelBuffer),0);

    regs.h.cl = 0x61;                   // Read track
    regs.h.ch = 0x08;                   // Category code (Major code)
    regs.h.bl = driveLetter;            // Get drive letter
    regs.h.bl -= 'A'-1;                 // Get drive number
    sregs.ds = HIWORD(dwSegSelPacket);
    regs.x.dx = 0;
    regs.x.ax = 0x440D;                 // Generic IOCTL request
    IntWin(0x21, &regs, &sregs);

    if (regs.x.cflag)
        {
        regs.h.ah = 0x59;               // Get Extended Error
        regs.x.bx = 0;
        IntWin(0x21, &regs, &sregs);
        wRet = regs.x.ax;
        }
    else
        {
        lpBootBuffer = (BPBBootRec FAR *) lpReadBuffer;
        if (!DiskIsValidBPB(&(lpBootBuffer->BPB)))
            wRet = 0x1F;                // General failure, as if no format
        else
            wRet = 0;
        }

    GlobalDosFree(LOWORD(dwSegSelBuffer));
    GlobalDosFree(LOWORD(dwSegSelPacket));
    return (wRet);
#else // NEC
    auto        DeviceParamRec          tempInfoPacket;
    auto        WORD            wRet = NOERR;

	GetDosDriverTablePtr ( driveLetter );
    tempInfoPacket.specialFunctions = IOCTL_SF_GET_CURRENT;
    if (!DiskGenericIOCTL(IOCTL_GET_PARAMS, driveLetter, &tempInfoPacket))
	{
	_asm
	    {
	    mov		ah, 59h		; Get Extended Error
	    mov		bx, 0
            DOS
	    mov		wRet, ax
	    }
	return ( wRet );
	}

	if ( ! DiskIsValidBPB ( &tempInfoPacket.BPB ) )
		wRet = 0x1F;                // General failure, as if no format
	else
		wRet = 0;

	return ( wRet );

#endif // NEC

}

#else

#pragma optimize("", off)
UINT LOCAL PASCAL DiskTestSector (BYTE driveLetter, UINT wBytesPerSector)
{
    auto        READWRITETRACKPACKET            rPacket;
    auto        READWRITETRACKPACKET FAR*       lpReadPacket;
    auto        BPBBootRec FAR*                 lpBootBuffer;
    auto        HANDLE                          hMem;
    auto        LPBYTE                          lpReadBuffer;
    auto        UINT                            wRet = NOERR;


    lpReadPacket = &rPacket;

    if ((hMem = MemAlloc(GHND, wBytesPerSector)) == NULL)
        return (0x08);                  // Insufficient memory

    if ((lpReadBuffer = MemLock (hMem)) == NULL)
        {
        MemFree (hMem);
        return (0x08);                  // Insufficient memory
        }

    lpReadPacket->bSpecialFunctions = 0;
    lpReadPacket->wHead = 0;
    lpReadPacket->wCylinder = 0;
    lpReadPacket->wFirstSector = 0;
    lpReadPacket->wNumberOfSectors = 1;
    lpReadPacket->lpTransferAddress = lpReadBuffer;

    _asm
        {
        push    ds              // save DS

        lds     dx, lpReadPacket
        mov     cl, 61h         // read track
        mov     ch, 8           // Category code (Major code)
        mov     bl, driveLetter // Get drive letter
        sub     bl, 'A'-1;      // Get drive number
        mov     ax, 440Dh
        DOS

        pop     ds              // restore DS

        jnc     No_Error

        mov     ah, 59h         // Get extended error
        xor     bx,bx
        DOS
        mov     wRet, ax
        jmp     short Finish_Up
        }

No_Error:
    lpBootBuffer = (BPBBootRec FAR *) lpReadBuffer;

    if (!DiskIsValidBPB(&(lpBootBuffer->BPB)))
        wRet = 0x1F;                // General failure, as if no format
    else
        wRet = 0;

Finish_Up:
    return (wRet);
}
#pragma optimize("", on)

#endif

#endif

