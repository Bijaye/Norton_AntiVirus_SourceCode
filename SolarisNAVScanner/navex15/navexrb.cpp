//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexrb.cpv   1.58   09 Dec 1998 12:54:54   MMAGEE  $
//
// Description:
//      Contains NAVEX EXTRepairBoot code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexrb.cpv  $
// 
//    Rev 1.58   09 Dec 1998 12:54:54   MMAGEE
// removed #ifdef/endif for SARC_SECURECOMPUTING
// 
//    Rev 1.57   25 Nov 1998 19:44:50   relniti
// REVERT BACK TO 1.54
// 
//    Rev 1.54   19 Nov 1998 15:48:10   relniti
// IFDEF out the 144Floppy repair added for Azusa and Flame
// 
//    Rev 1.53   18 Nov 1998 22:51:38   relniti
// blah
// 
//    Rev 1.52   18 Nov 1998 18:02:12   CFORMUL
// Modified repair of Da'Boys for Solomon will not fp on repair.
// I also fixed it so the floppy will do a generic repair
// 
//    Rev 1.51   10 Feb 1998 15:11:26   CFORMUL
// I had the parameters to MEMCPY backwards
// for the wolleh repair
// 
//    Rev 1.50   10 Feb 1998 10:54:16   CFORMUL
// Fixed a possible bug in Wolleh repair.
// 
//    Rev 1.49   06 Feb 1998 12:41:10   DKESSNE
// changed memcpy to MEMCPY in Wolleh repair function
// 
//    Rev 1.48   05 Feb 1998 17:42:14   DKESSNE
// Added repair for Wolleh HBR
// 
//    Rev 1.47   20 Nov 1997 19:42:54   JWILBER
// Modified during Dec97 build.
//
//    Rev DEC97  17 Nov 1997 18:15:34   DCHI
// Added code_seg() pragma for SYM_WIN16.
//
//    Rev 1.46   27 Oct 1997 12:28:00   JWILBER
// Modified during Nov97 build.
//
//    Rev 1.45   23 Oct 1997 18:29:40   AOONWAL
// Modifed "Swiss Boot" repair
//
//    Rev 1.44   20 Oct 1997 20:55:10   AOONWAL
// Added repair for "Swiss Boot"
//
//    Rev 1.43   13 Aug 1997 18:25:32   DANG
// Added INT12 repair.  See comments on what it does to
// 'repair' the virus.
//
//    Rev 1.42   30 May 1997 12:08:52   CNACHEN
// Added LPVOID parameter to repair function to support UI.
//
//    Rev 1.41   29 Apr 1997 10:56:56   AOONWAL
// Added repair for Saigon
//
//    Rev 1.40   26 Dec 1996 15:22:06   AOONWAL
// No change.
//
//    Rev 1.39   13 Dec 1996 12:46:24   FBARAJA
// Remove #ifdef 0 for DMF repair.
//
//    Rev 1.38   02 Dec 1996 14:02:12   AOONWAL
// No change.
//
//    Rev 1.37   31 Oct 1996 11:59:38   AOONWAL
// Added repair for Karnivali boot
//
//    Rev 1.36   29 Oct 1996 12:58:54   AOONWAL
// No change.
//
//    Rev 1.35   03 Oct 1996 15:26:14   JWILBER
// Added a break statement between the cases for the Roma and Edwin
// repairs.
//
//    Rev 1.34   30 Sep 1996 19:42:34   JWILBER
// Changed return type on RomaPG, Cruel, and DMFGeneric repair
// functions from EXTSTATUS to WORD.
//
//    Rev 1.33   16 Sep 1996 19:47:00   FBARAJA
// Passed EDWIN BOOT REPAIR
//
//    Rev 1.32   16 Sep 1996 09:58:36   AOONWAL
// Modified repair for Edwin so it can work on WINDOWS 95
//
//    Rev 1.31   13 Sep 1996 11:10:54   AOONWAL
// Added repair for Edwin boot virus
//
//    Rev 1.30   12 Sep 1996 15:43:28   RAY
// Fixed DMF Generic repair
//
//    Rev 1.29   26 Jul 1996 14:09:04   CRENERT
// Re-instated DMF changes.
//
//    Rev 1.27   25 Jul 1996 18:07:58   RAY
// Added Generic Repair of DMF floppies.
//
//    Rev 1.26   22 Apr 1996 15:27:04   MCANDELA
// uncommented roma
//
//    Rev 1.25   16 Apr 1996 01:04:54   JWILBER
// Commented out the int 3 for the Roma repair, too.
//
//    Rev 1.24   16 Apr 1996 01:03:22   JWILBER
// Commented out Roma repair, since it's untested.
//
//    Rev 1.23   16 Apr 1996 00:59:54   JWILBER
// Minor change to comments in Roma.PG boot repair.
//
//    Rev 1.22   28 Mar 1996 23:33:50   JWILBER
// Commented out case for Roma repair and INT 3 at beginning of file.
//
//    Rev 1.21   28 Mar 1996 23:31:18   JWILBER
// Added repair for Roma.PG virus.
//
//    Rev 1.20   19 Mar 1996 10:33:52   JSULTON
// Uncommented Jackal.
//
//    Rev 1.19   18 Mar 1996 13:39:10   JWILBER
// Added fudge factor into Ether repair due to varying location of
// fix data.
//
//    Rev 1.18   14 Mar 1996 17:36:52   JWILBER
// Added fixes to Ether boot fix to account for new-found polymorphism.
//
//    Rev 1.17   13 Mar 1996 15:01:28   JWILBER
// Fixed check at end of Ether boot repair for valid boot sector -
// Now checks for 0xaa55 instead of 0x55aa.
//
//    Rev 1.16   12 Mar 1996 17:59:46   JWILBER
// Changed Ether boot fix to compute decrypt key from viral boot sector
// instead of from encrypted original boot sector.  Changed some error
// returns from "NO_REPAIR" to an appropriate disk error.
//
//    Rev 1.15   11 Mar 1996 18:41:24   JWILBER
// Replaced occurences of "__far" in Jackal boot repair with "FAR".
//
//    Rev 1.14   06 Mar 1996 15:10:40   JWILBER
// Fixed minor bug w/ Ether repair.
//
//    Rev 1.13   06 Mar 1996 13:51:34   JWILBER
// Removed an unused local variable from Jackal fix.
//
//    Rev 1.12   06 Mar 1996 09:38:16   JWILBER
// Added Jackal boot repair.
//
//    Rev 1.11   01 Mar 1996 19:30:14   JSULTON
// Uncommented Da'Boys, not passed yet.
//
//    Rev 1.10   13 Feb 1996 17:55:54   MKEATIN
// Added DX Support
//
//    Rev 1.9   01 Feb 1996 14:48:06   JALLEE
// Added repair for Cruel boot virus.
//
//    Rev 1.8   01 Feb 1996 13:44:28   JWILBER
// Added boot repair for Ether, 1f3e.
//
//    Rev 1.7   30 Jan 1996 15:43:30   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
//
//    Rev 1.6   12 Dec 1995 18:41:00   JALLEE
// voided unused parameters for NLM
//
//    Rev 1.5   12 Dec 1995 17:58:20   JALLEE
// Added boot repair for Da'Boys, but left disabled pending QA.
//
//    Rev 1.4   01 Nov 1995 10:49:06   DCHI
// Removed include of navexhdr.h due to change in format of navexshr.h.
//
//    Rev 1.3   19 Oct 1995 14:20:36   DCHI
// Added LOADDS to external functions for WIN16.  Modified DOS export
// table structure to support tiny model modules.
//
//    Rev 1.2   18 Oct 1995 11:53:08   CNACHEN
// Voided unused parameters to compile correctly for NLM.
//
//    Rev 1.1   16 Oct 1995 13:03:58   DCHI
// Added include of navexhdr.h.
//
//    Rev 1.0   13 Oct 1995 13:05:18   DCHI
// Initial revision.
//
//************************************************************************

#ifdef SYM_WIN16
#pragma code_seg("NAVEX_NAVEXRB","NAVEX_BOOT_CODE")
#endif

#ifndef SYM_NLM

#include "platform.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navex.h"

// Declare shared routines

#include "navexshr.h"

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif

// Prototypes for local boot repair functions

WORD      EXTRepairBootDaBoys(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootEther(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootJackal(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootCruel(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootRoma(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootEdwin(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootKarnivali(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootSaigon(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootINT12(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootSwiss(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootWolleh(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBoot144Floppy(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer);

WORD      EXTRepairBootDMFGeneric
(
    LPCALLBACKREV1 lpCallBack,
    LPN30          lpsN30,
    BYTE           byDrive,
    BYTE           byPartitionNum,
    WORD           wSector,
    WORD           wCylinder,
    WORD           wHead,
    LPBYTE         lpbyWorkBuffer
);

// *************************
//
// Native Navex function
//
// *************************


// EXTRepairBoot arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// wVersionNumber       : NAV version number.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk write error
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

#ifdef NAVEX15

EXTSTATUS FAR WINAPI NLOADDS EXTRepairBoot(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           BYTE           byPartitionNum,
                                           WORD           wSector,
                                           WORD           wCylinder,
                                           WORD           wHead,
                                           LPBYTE         lpbyWorkBuffer,
                                           LPVOID         lpvCookie)

#else

EXTSTATUS FAR WINAPI NLOADDS EXTRepairBoot(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           BYTE           byPartitionNum,
                                           WORD           wSector,
                                           WORD           wCylinder,
                                           WORD           wHead,
                                           LPBYTE         lpbyWorkBuffer)

#endif
{

#if defined(SYM_DOS)

    _asm
        {
        mov cs:[wDS], ds
        mov cs:[wES], es
        mov ax, cs
        mov ds, ax
        mov es, ax
        }

#endif

    WORD    wReturn = EXTSTATUS_NO_REPAIR;

    (void)lpCallBack;
    (void)wVersionNumber;
    (void)lpsN30;
    (void)byDrive;
    (void)byPartitionNum;
    (void)wSector;
    (void)wCylinder;
    (void)wHead;
    (void)lpbyWorkBuffer;



                                        // REP_EXEC_CODE defined in repair.inc
                                        // Check if external is called.
    //tagsize = floppytype
    if (lpsN30->wHardType == REP_EXEC_CODE || lpsN30->wTagSize == REP_EXEC_CODE)
    {
                                        // wHostJmp field contains ID of
                                        // repair to perform.  This kludgy
                                        // shared purpose goes way back.

        switch (lpsN30->wHardLoc1)
        {

            case ID_CRUEL   : wReturn = EXTRepairBootCruel(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;

            case ID_DABOYS  : wReturn = EXTRepairBootDaBoys(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;


            case ID_ETHER   : wReturn = EXTRepairBootEther(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;

            case ID_JACKAL  : wReturn = EXTRepairBootJackal(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;
 
            case ID_ROMA    : wReturn = EXTRepairBootRoma(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;

            case ID_EDWIN   : wReturn = EXTRepairBootEdwin(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;

            case ID_KARNIVALI  : wReturn = EXTRepairBootKarnivali(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;

            case ID_SAIGON    : wReturn = EXTRepairBootSaigon(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;

            case ID_INT12     : wReturn =  EXTRepairBootINT12(lpCallBack,
                                                              lpsN30,
                                                              byDrive,
                                                              byPartitionNum,
                                                              wSector,
                                                              wCylinder,
                                                              wHead,
                                                              lpbyWorkBuffer);
                                break;

            case ID_SWISS   : wReturn = EXTRepairBootSwiss(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;

            case ID_WOLLEH   : wReturn = EXTRepairBootWolleh(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;

            case ID_144FLOPPY : wReturn = EXTRepairBoot144Floppy(lpCallBack,
                                                        lpsN30,
                                                        byDrive,
                                                        byPartitionNum,
                                                        wSector,
                                                        wCylinder,
                                                        wHead,
                                                        lpbyWorkBuffer);
                                break;


            // defwriter: your repairs here.

            default         :   break;
        }

    }


    if ( EXTSTATUS_NO_REPAIR == wReturn )
        {
        wReturn = EXTRepairBootDMFGeneric ( lpCallBack,
                                            lpsN30,
                                            byDrive,
                                            byPartitionNum,
                                            wSector,
                                            wCylinder,
                                            wHead,
                                            lpbyWorkBuffer );
        }


#if defined(SYM_DOS)

    _asm
        {
        mov ds, cs:[wDS]
        mov es, cs:[wES]
        }



#endif

    return (wReturn);
}

// ***************************
//
// Local Boot Repair Functions
//
// ***************************


// EXTRepairBootDaBoys arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD      EXTRepairBootDaBoys(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)

{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    BYTE    bySig[] = {0x44, 0x41, 0x27, 0x42, 0x4f, 0x59, 0x53, 0x0b8};
    BYTE    byDecryptor[] = {0x0be, 0x03e, 0x07c, 0x0b9, 0x044, 0x001,
                           0x80, 0x04, 0x01, 0x046, 0x0e2, 0x0fa,
                           0x0e9, 0x09d, 0x0fe };

    WORD    i;

    (void)lpsN30;
    (void)byPartitionNum;

    if (byDrive < 0x80) {
        return (EXTSTATUS_NO_REPAIR);
    }


                                        // Get the boot sector

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,       // buffer
                                        (BYTE)1,           // count
                                        wCylinder,         // cyl
                                        (BYTE)wSector,     // sector
                                        (BYTE)wHead,       // head
                                        (BYTE)byDrive))    // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                        // Double check infection.
    if ((WORD)-1 == ScanString((LPBYTE)(lpbyWorkBuffer + 0x147),
                               10,
                               (LPBYTE)(bySig),
                               8))
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    lpbyWorkBuffer[0x48] = 0xbb;
    lpbyWorkBuffer[0x49] = 0x78;

    for (i=0;i<26;i++)
    {
        lpbyWorkBuffer[0x13a + i] = 0;
    }
    for (i=0;i<0x63;i++)
    {
        lpbyWorkBuffer[0x183 + i] = 0;
    }
    lpbyWorkBuffer[0] = 0xe9;
    lpbyWorkBuffer[1] = 0x8f;
    lpbyWorkBuffer[2] = 0x01;

    for (i=0;i<15;i++)
    {
        lpbyWorkBuffer[0x192 + i] = byDecryptor[i];
    }


    for (i=0;i<0x144;i++)
    {
        lpbyWorkBuffer[0x3e + i] -= 0x1;
    }


    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,  // buffer
                                        (BYTE)1,                // count
                                        wCylinder,              // cyl
                                        (BYTE)wSector,          // sector
                                        (BYTE)wHead,            // head
                                        (BYTE)byDrive))         // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }


    else
    {
        wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!
    }


    return (wReturn);
}


// EXTRepairBootCruel Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD    EXTRepairBootCruel (LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)

{
    WORD        wReturn = EXTSTATUS_NO_REPAIR;
    LPBYTE      lpbyVirusBuffer;

    WORD        wVirusCyl,i;
    BYTE        byVirusSec;

    (void)lpsN30;
    (void)byPartitionNum;


                                        // Get the boot sector

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,       // buffer
                                        (BYTE)1,           // count
                                        wCylinder,         // cyl
                                        (BYTE)wSector,     // sector
                                        (BYTE)wHead,       // head
                                        (BYTE)byDrive))    // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                        // Double check infection.
    if ( lpbyWorkBuffer[0x56] != 0xcd ||
         lpbyWorkBuffer[0x57] != 0x13)
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    lpbyVirusBuffer = lpbyWorkBuffer + 512;

                                        // Get virus body from disk
                                        // located at end of Root dir
                                        // Find by using location bytes in
                                        // the infected boot sector.

    byVirusSec = lpbyWorkBuffer[0x50] & 0x3f;
    wVirusCyl = ((WORD)(lpbyWorkBuffer[0x50]  & 0xc0)) << 2;
    wVirusCyl += lpbyWorkBuffer[0x51];

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyVirusBuffer,
                                      (BYTE)1,
                                      wVirusCyl,
                                      byVirusSec,
                                      lpbyWorkBuffer[0x53],
                                      lpbyWorkBuffer[0x55] ))
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                        // bookmark check for 'CR' near end
                                        // of virus sector.

    if ( lpbyVirusBuffer[0x1f9] != 0x43 ||
         lpbyVirusBuffer[0x1fa] != 0x52)
    {
        return (EXTSTATUS_NO_REPAIR);
    }

                                        // copy repair infomation
    for (i=0;i<29;i++)
    {
        lpbyWorkBuffer[0x3e + i] = lpbyVirusBuffer[0x1db + i];
    }

                                        // Zero reserved byte
    lpbyWorkBuffer[0x25] = 0x00;

                                        // Write back repaired boot sector
    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,  // buffer
                                        (BYTE)1,                // count
                                        wCylinder,              // cyl
                                        (BYTE)wSector,          // sector
                                        (BYTE)wHead,            // head
                                        (BYTE)byDrive))         // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }
    else
    {
        wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!
    }


    return (wReturn);
}

// EXTRepairBootEther arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0 = A,
//                        0x80 = C.
// byPartitionNum       : if != FFh, this is the partition # of the boot
//                        sector
// wSector, wCylinder,
// wHead                : location of boot sector.
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk write error
//                                                          (duh!)

#define DISK_ADD    0x7c00      // Boot sectors loaded here
#define START_SUM   0x81        // Address to begin decrypt adding is here
#define ADD_START   0xbd        // Add this to START_SUM for WRT_VALUE dest.
#define WRT_VALUE   0xbf        // Value to write is located here
#define NUM_ADD     0x147       // Number of bytes to write is here
#define MOVBX       0xbb        // MOV BX, ?? opcode
#define MAXFUDGE    0x0c        // Maximum value for fudge factor

WORD      EXTRepairBootEther(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)
{
    BYTE    bySig[] = {0x8a, 0x16, 0xf6, 0x83, 0x9c, 0xff, 0x01e, 0xfc, 0x83};
    WORD    wLogVolSecs,                // Sectors in logical volume
            wNumHidSecs,                // Number of hidden sectors - lo word
            wSecs_Track,                // Sectors per Track
            wNumHeads;                  // Number of heads (sides)
    WORD    wOrigCyl,                   // Cylinder for Original Boot Sector
            wWrtVal,                    // Value that self-mod code writes
            wFudge;                     // Fudge factor for different BS's.
    BYTE    byOrigSec, byOrigHead;      // Sector, and Head, too
    BYTE    byXOR_Byte;                 // Byte we XOR everything with!

    WORD    i, scrap1, scrap2, scrap3;

    //_asm    int 3                     // Remove when not debugging!
                                        // Get the boot sector
    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,   // buffer
                                        1,                  // count
                                        wCylinder,          // cyl
                                        (BYTE)wSector,      // sector
                                        (BYTE)wHead,        // head
                                        byDrive))           // drive
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }
                                        // Double check infection.
    if (0xffff == ScanString((lpbyWorkBuffer + 0x151), (6 + MAXFUDGE),
                                                                    bySig, 5))
    {                                   // This sigs on the far call to INT 13
        return (EXTSTATUS_NO_REPAIR);
    }                   // Two scans needed because a byte had to be skipped

    wFudge = ScanString((lpbyWorkBuffer + 0x157), (5 + MAXFUDGE),
                                                            (bySig + 5), 4);

    if (0xffff == wFudge)           // Fudge factor is offset of string search
    {
        return (EXTSTATUS_NO_REPAIR);           // Bail if string not found
    }
                                                            //  MOV AX,[7c13]
    wLogVolSecs = *((LPWORD)(lpbyWorkBuffer + OLDLOGVOLSEC_OFF));
    wNumHidSecs = *((LPWORD)(lpbyWorkBuffer + NUMHIDSECS_OFF));
    wSecs_Track = *((LPWORD)(lpbyWorkBuffer + SECSPERTRACK_OFF));
    wNumHeads = *((LPWORD)(lpbyWorkBuffer + NUMHEADS_OFF));
                                                            //  DEC AX
    wOrigCyl = wLogVolSecs - 2; // Mirror virus computations    DEC AX
    wOrigCyl += wNumHidSecs;            // done by the virus    ADD AX,[7c1c]

    scrap1 = wOrigCyl % wSecs_Track;    // scrap1 is DX         DIV [7c18]
    wOrigCyl /= wSecs_Track;            // wOrigCyl is AX       PUSH DX

    scrap2 = wOrigCyl % wNumHeads;      // scrap2 is DX         DIV [7c1a]
    wOrigCyl /= wNumHeads;              // wOrigCyl is AX
        // This is the net when shifting left 6 and right 8.    MOV CL,6
    scrap3 = (wOrigCyl & 0xff00) >> 2;                      //  SHL AH,CL
    scrap3 = ((++scrap1 | scrap3) & 0xff);                  //  POP CX
                            // wOrigCyl is CL   scrap3 is AH    OR  AH,CL
    byOrigSec = (BYTE) scrap3 & 0x3f;       //  scrap3 is CL    MOV CX,AX
    wOrigCyl &= 0xff;           // (Upper 2 bits of CL) | CH == cylnum
    wOrigCyl |= ((scrap3 << 2) & 0x300);                    //  XCHG CH,CL

    byOrigHead = (BYTE) scrap2 & 0xff;                      //  XCHG DH,DL

    wWrtVal = *((LPWORD)(lpbyWorkBuffer + WRT_VALUE + wFudge));
    scrap1 = *((LPWORD)(lpbyWorkBuffer + START_SUM + wFudge));
    scrap1 -= DISK_ADD;                         // Compute where to write it
    i = scrap1;         // Start adding at i, and add offset to destination
    scrap1 += *((LPWORD)(lpbyWorkBuffer + ADD_START + wFudge));
    *((LPWORD)(lpbyWorkBuffer + scrap1)) = wWrtVal;     // Write the value

    byXOR_Byte = 0;                                     // Initialize

    for (scrap2 = *((LPWORD)(lpbyWorkBuffer + NUM_ADD + wFudge));
                                0 != scrap2; scrap2--)  // Find decrypt key
        byXOR_Byte += lpbyWorkBuffer[i++];
                                             // Get the original boot sector
    if (lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,    // buffer
                                        1,                  // count
                                        wOrigCyl,           // cyl
                                        byOrigSec,          // sector
                                        byOrigHead,         // head
                                        byDrive))           // drive
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }

    for (i = 0; i < 0x200; i++)
        lpbyWorkBuffer[i] ^= byXOR_Byte; // Yessss!!! (Pumping motion w/ arm)

    if (0xaa55 != *((LPWORD) (lpbyWorkBuffer + 0x1fe))) // Check the repair
        return (EXTSTATUS_NO_REPAIR);

    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,  // buffer
                                        1,                      // count
                                        wCylinder,              // cyl
                                        (BYTE)wSector,          // sector
                                        (BYTE)wHead,            // head
                                        byDrive))               // drive
    {
        return (EXTSTATUS_DISK_WRITE_ERROR);
    }

    return (EXTSTATUS_OK);              // Whooya, NAV does it again!
}   // End of Ether repair

// EXTRepairBootJackal arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0 = A,
//                        0x80 = C.
// byPartitionNum       : if != FFh, this is the partition # of the boot
//                        sector - currently unused
// wSector, wCylinder,
// wHead                : location of boot sector.
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//
// WARNING: To produce a clean compile, the undocumented keyword "__far" is
// used in this routine.
//
// "__far" replaced with "FAR"  11 Mar 96   jjw
//

#define ORIG_HBR_CYLSEC     0x64        // Offset of CX value for INT 13
#define ORIG_HBR_HEAD       0x67        // Offset of DH value for INT 13

#define COMPUTE_CYLSEC      { wInfCylSec = (wCylinder << 8) |           \
                                            ((wCylinder >> 2) & 0xc0);  \
                              wInfCylSec |= wSector; }

WORD      EXTRepairBootJackal(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)
{
    PART_ENT    FAR *pFixPart;              // Partition table entry struct
    WORD        wHBRCyl, wInfCylSec, wPartnum;
    BYTE        byHBRHead;
    BYTE        bySig[] =
                    { 0x0e, 0x1f, 0x83, 0x2e, 0x13, 0x04, 0x05, 0xcd, 0x12 };

    //_asm    int 3                     // Remove when debugged!    &&J

    if ((byDrive & 0x80) != 0x80)
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                        // Read boot sector to check it
    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,   // buffer
                                        1,                  // count
                                        wCylinder,          // cyl
                                        (BYTE)wSector,      // sector
                                        (BYTE)wHead,        // head
                                        byDrive))           // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                        // Double check infection.
    if (0xffff == ScanString((lpbyWorkBuffer + 0x1c), 10, bySig, 9))
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                            // Get location of original HBR
    wHBRCyl = *((LPWORD) (lpbyWorkBuffer + ORIG_HBR_CYLSEC));
    byHBRHead = lpbyWorkBuffer[ORIG_HBR_HEAD];
                                                    // Read MBR to fix it
    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,   // buffer
                                        1,                  // count
                                        0,                  // cyl
                                        1,                  // sector
                                        0,                  // head
                                        byDrive))           // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                            // Get pointer to partition table
    wPartnum = 0;
    pFixPart = (PART_ENT FAR *) (lpbyWorkBuffer + PART_TAB_OFF);

    COMPUTE_CYLSEC;

    while (((pFixPart->st_head != (BYTE) wHead) ||      // Find entry in
            (pFixPart->st_cylsec != wInfCylSec)) &&     // partition table
            (wPartnum < MAX_PART_ENTS))                 // to change
    {
        wPartnum++;
        pFixPart += PART_ENT_SIZE;      // We loop unless both entries match,
                                        // and we haven't gone past the end
        COMPUTE_CYLSEC;                 // of the partition table
    }

    if (MAX_PART_ENTS < wPartnum)
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    pFixPart->st_head = byHBRHead;          // Change correct entry in PT
    pFixPart->st_cylsec = wHBRCyl;

    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,  // buffer
                                        1,                  // count
                                        0,                  // cyl
                                        1,                  // sector
                                        0,                  // head
                                        byDrive))           // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    return (EXTSTATUS_OK);              // Whooya, NAV does it again!
}   // End of Jackal repair

// EXTRepairBootRoma arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0 = A,
//                        0x80 = C.
// byPartitionNum       : if != FFh, this is the partition # of the boot
//                        sector
// wSector, wCylinder,
// wHead                : location of boot sector.
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk write error
//                                                          (duh!)

#define ROMA_SIG_OFF    0x57            // Offset into boot sector of sig
#define ROMA_SIG_LEN    14              // Length of sig

WORD    EXTRepairBootRoma(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)
{
    BYTE        bySig[] = {0xb1, 0x06, 0x50, 0xff, 0x0e, 0x13, 0x04,
                        0xa1, 0x13, 0x04, 0xd3, 0xe0, 0xa3, 0x75 };
    DWORD       dwLogVolSecs;           // Sectors in logical volume
    BPB_struct  FAR *bs_parms;          // Easy way to get to boot parameters

    //_asm    int 3                     // Remove when not debugging!
                                        // Get the boot sector
    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,   // buffer
                                        1,                  // count
                                        wCylinder,          // cyl
                                        (BYTE)wSector,      // sector
                                        (BYTE)wHead,        // head
                                        byDrive))           // drive
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }
                                        // Double check infection.
    if (0xffff == ScanString((lpbyWorkBuffer + ROMA_SIG_OFF),
                                (ROMA_SIG_LEN + 1), bySig, ROMA_SIG_LEN))
    {                                   // This sigs on the far call to INT 13
        return (EXTSTATUS_NO_REPAIR);
    }

    bs_parms = (BPB_struct FAR *) (lpbyWorkBuffer + BPB_OFFSET);

    dwLogVolSecs = (0 == bs_parms->Sectors) ? bs_parms->HugeSectors :
                                                (DWORD) (bs_parms->Sectors);
    dwLogVolSecs += ((bs_parms->HiddenSecs) & 0xffff);  // Get disk sectors
    dwLogVolSecs /= bs_parms->SecPerTrack;              // Find total tracks
    dwLogVolSecs &= 0xffff;
    dwLogVolSecs /= bs_parms->Heads;                    // Find tracks / side

    dwLogVolSecs--;
                                             // Get the original boot sector
    if (lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,    // buffer
                                        1,                      // count
                                        (WORD) dwLogVolSecs,    // cyl
                                        (BYTE) bs_parms->SecPerTrack, // sect
                                        (BYTE) (bs_parms->Heads - 1), // head
                                        byDrive))               // drive
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }

    if (0xaa55 != *((LPWORD) (lpbyWorkBuffer + 0x1fe))) // Check the repair
        return (EXTSTATUS_NO_REPAIR);

    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,  // buffer
                                        1,                      // count
                                        wCylinder,              // cyl
                                        (BYTE)wSector,          // sector
                                        (BYTE)wHead,            // head
                                        byDrive))               // drive
    {
        return (EXTSTATUS_DISK_WRITE_ERROR);
    }

    return (EXTSTATUS_OK);              // Whooya, NAV does it again!
}   // End of Roma repair

// EXTRepairBootEdwin arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD      EXTRepairBootEdwin(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)

{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    LPBYTE  lpbyBOOT, lpbyVirus;
    WORD    i,k;

    (void)lpsN30;
    (void)byPartitionNum;


    lpbyBOOT = lpbyWorkBuffer;
    lpbyVirus = lpbyWorkBuffer + 512;

    //_asm    int 3                     // Remove when not debugging!
                                        // Get the boot sector

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyBOOT,         // buffer
                                        (BYTE)1,                // count
                                        (WORD)0,                // cyl
                                        (BYTE)1,                // sector
                                        (BYTE)1,                // head
                                        (BYTE)byDrive))         // drive

    {
        return (EXTSTATUS_NO_REPAIR);
    }

    k = lpbyBOOT[1];
                                         // Double check infection.
    if (!(lpbyBOOT[k+10] == 0xfb &&
              lpbyBOOT[k+11] == 0x68 &&
              lpbyBOOT[k+29] == 0x80 &&
              lpbyBOOT[0] == 0xeb))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

                                         // Get virus

    if (lpCallBack->SDiskReadPhysical(  (LPSTR)lpbyVirus,      // buffer
                                         (BYTE)1,            // count
                                         (WORD)0,            // cyl
                                         (BYTE)4,            // sector
                                         (BYTE)0,            // head
                                         (BYTE)byDrive))     // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    for (i=0;i<80;i++)
    {
        lpbyBOOT[i + k + 2] = lpbyVirus[i+11];
    }

    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyBOOT,         // buffer
                                        (BYTE)1,                // count
                                        (WORD)0,                // cyl
                                        (BYTE)1,                // sector
                                        (BYTE)1,                // head
                                        (BYTE)byDrive))         // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    else
    {
        wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!
    }

    return (wReturn);
}

// EXTRepairBootKarnivali arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD      EXTRepairBootKarnivali(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)

{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    BYTE    bySig[] = {0x1e, 0x06, 0xb8, 0x00, 0x70, 0x8e, 0xc0, 0x0b4};

    LPBYTE  lpbyBOOT;
    WORD    i;

    lpbyBOOT = lpbyWorkBuffer;

                                        // Get the boot sector

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,       // buffer
                                        (BYTE)1,           // count
                                        wCylinder,         // cyl
                                        (BYTE)wSector,     // sector
                                        (BYTE)wHead,       // head
                                        (BYTE)byDrive))    // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    if (!(lpbyBOOT[0] == 0xe9 &&
              lpbyBOOT[1] == 0x9D &&
              lpbyBOOT[2] == 0x01))
    {
        return(EXTSTATUS_NO_REPAIR);
    }
                                        // check infection.

    if ((WORD)-1 == ScanString((LPBYTE)(lpbyWorkBuffer + 0x1A0),
                               10,
                               (LPBYTE)(bySig),
                               8))
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    for (i=0;i<31;i++)
    {
        lpbyWorkBuffer[0x1A0 + i] = 0x90;
    }


    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,  // buffer
                                        (BYTE)1,                // count
                                        wCylinder,              // cyl
                                        (BYTE)wSector,          // sector
                                        (BYTE)wHead,            // head
                                        (BYTE)byDrive))         // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }


    else
    {
        wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!
    }


    return (wReturn);
}


WORD EXTRepairBootDMFGeneric
(
    LPCALLBACKREV1 lpCallBack,
    LPN30          lpsN30,
    BYTE           byDrive,
    BYTE           byPartitionNum,
    WORD           wSector,
    WORD           wCylinder,
    WORD           wHead,
    LPBYTE         lpbyWorkBuffer
)
{
    WORD    wFloppyType;

    if ( 0x80 & byDrive )
        {
        return EXTSTATUS_NO_REPAIR;     // No repair for Hard Drives
        }

                                        // Read the boot sector
    if ( lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,
                                        1,
                                        wCylinder,
                                       (BYTE)wSector,
                                       (BYTE)wHead,
                                        byDrive))
        {
        return EXTSTATUS_DISK_READ_ERROR;
        }

                                        // Use it to determine floppy type
    if ( !NavexGetFloppyType ( lpbyWorkBuffer, &wFloppyType ))
        {
        return EXTSTATUS_NO_REPAIR;
        }

                                        // If it's a DMF floppy
    if ( DISK_FLOPPY_1KDMF == wFloppyType || DISK_FLOPPY_2KDMF == wFloppyType )
        {

                                        // Try a generic repair
        if ( !NavexRepairBootGeneric ( lpbyWorkBuffer, wFloppyType ))
            {
            return EXTSTATUS_NO_REPAIR;
            }
                                        // Write out the new sector
        if ( lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,
                                             1,
                                             wCylinder,
                                             (BYTE)wSector,
                                             (BYTE)wHead,
                                             byDrive))
            {
            return EXTSTATUS_DISK_WRITE_ERROR;
            }

        return EXTSTATUS_OK;
        }

    return EXTSTATUS_NO_REPAIR;
}

// EXTRepairBootSaigon Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD    EXTRepairBootSaigon (LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)

{
    WORD       wReturn = EXTSTATUS_NO_REPAIR;
    WORD       wTotHeads, wTotSectors;
    DWORD      dwGoodSector, dwGoodHead, dwGoodCylinder, dwValue;

    (void)lpsN30;
    (void)byPartitionNum;


                                        // Get the boot sector

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,       // buffer
                                        (BYTE)1,           // count
                                        wCylinder,         // cyl
                                        (BYTE)wSector,     // sector
                                        (BYTE)wHead,       // head
                                        (BYTE)byDrive))    // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                        // Double check infection.
    if ( lpbyWorkBuffer[0x4d] != 0xcd ||
         lpbyWorkBuffer[0x4e] != 0x12)
    {
        return (EXTSTATUS_NO_REPAIR);
    }

                                        // Find Orignal boot
                                        // by using location bytes in
                                        // the infected boot sector.

    dwValue = *((LPDWORD)(lpbyWorkBuffer + 0x67));
    wTotSectors = *((LPWORD)(lpbyWorkBuffer + 0x18));
    wTotHeads = *((LPWORD)(lpbyWorkBuffer + 0x1a));

    dwGoodSector = dwValue % wTotSectors;
    dwGoodSector = dwGoodSector + 1;
    dwValue = dwValue / wTotSectors;
    dwGoodHead = dwValue % wTotHeads;
    dwGoodCylinder = dwValue / wTotHeads;

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,
                                      (BYTE)1,
                                      (BYTE)dwGoodCylinder,
                                      (BYTE)dwGoodSector,
                                      (BYTE)dwGoodHead,
                                      (BYTE)byDrive))
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                        // Check boot signature 0x55AA

    if ( lpbyWorkBuffer[0x1fe] != 0x55 ||
         lpbyWorkBuffer[0x1ff] != 0xAA)
    {
        return (EXTSTATUS_NO_REPAIR);
    }

                                        // Write back Orignal boot sector
    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,  // buffer
                                        (BYTE)1,                // count
                                        wCylinder,              // cyl
                                        (BYTE)wSector,          // sector
                                        (BYTE)wHead,            // head
                                        (BYTE)byDrive))         // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }
    else
    {
        wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!
    }


    return (wReturn);
}


// EXTRepairBootINT12 arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD      EXTRepairBootINT12(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)

{

    BYTE    bySig[] = {0x33, 0xC0, 0xB3, 0x13, 0x8E, 0xD8, 0xA3, 0x48,
                       0x00, 0xB7, 0x04, 0xB1, 0x06, 0xFF, 0x0F, 0x8B};

    LPBYTE  lpbyBOOT;
    WORD    i;

    lpbyBOOT = lpbyWorkBuffer;

    // Get the boot sector
    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,       // buffer
                                       (BYTE)1,           // count
                                       (WORD) wCylinder,  // cyl
                                       (BYTE)wSector,     // sector
                                       (BYTE)wHead,       // head
                                       (BYTE)byDrive))    // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }


    // Check if the JMP sig is valid.  Int12 doesn't use anything else.
    if (!(lpbyBOOT[0] == 0xe9 &&
              lpbyBOOT[1] == 0xAF &&
              lpbyBOOT[2] == 0x01))
    {
        return(EXTSTATUS_NO_REPAIR);
    }


    // Double check for infection by looking for the Int 12 hooking routine
    // of the virus within the Boot Sector code.  Never changes for infections
    if ((WORD)-1 == ScanString((LPBYTE)(lpbyWorkBuffer+0x1B2),
                               20,
                               (LPBYTE)(bySig),
                               16))
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    // NOP the viral instructions that do the INT12 hook
    for (i=0;i<37;i++)
    {
        lpbyWorkBuffer[0x1B2 + i] = 0x90;
    }

    // Give back the sector the virus took away.
    // It will be overwritten when it get used to hold information.
    // Only possibility I see this failing is if the total number of
    // sectors available happens to have this byte value at 00
    // Can't find any reference to beyond DOS4.0 for this field.
    lpbyWorkBuffer[32]++;

    // Write out the newly "neutered" boot sector to the hard drive.
    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,  // buffer
                                        (BYTE)1,                // count
                                        wCylinder,              // cyl
                                        (BYTE)wSector,          // sector
                                        (BYTE)wHead,            // head
                                        (BYTE)byDrive))         // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    return EXTSTATUS_OK;
}

// EXTRepairBootSwiss arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD      EXTRepairBootSwiss(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)

{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    LPBYTE  lpbyBOOT, lpbyGood;
    BYTE    SectorNum, HeadNum;
    WORD    CylinderNum;

    (void)lpsN30;
    (void)byPartitionNum;


    lpbyBOOT = lpbyWorkBuffer;
    lpbyGood = lpbyWorkBuffer + 512;

    //_asm    int 3                     // Remove when not debugging!
                                        // Get the boot sector

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyBOOT,         // buffer
                                        (BYTE)1,                // count
                                        (WORD)0,                // cyl
                                        (BYTE)1,                // sector
                                        (BYTE)1,                // head
                                        (BYTE)byDrive))         // drive

    {
        return (EXTSTATUS_NO_REPAIR);
    }

                                         // Double check infection.
    if (!(lpbyBOOT[0] == 0xe9 &&
              lpbyBOOT[1] == 0xAE &&
              lpbyBOOT[2] == 0x00))
    {
        return(EXTSTATUS_NO_REPAIR);
    }

                                         // Get Orignal Boot
    SectorNum = lpbyBOOT[82];
    CylinderNum = (BYTE)lpbyBOOT[83];
    HeadNum = lpbyBOOT[84];

    if (lpCallBack->SDiskReadPhysical(  (LPSTR)lpbyGood,      // buffer
                                         (BYTE)1,            // count
                                         (WORD)CylinderNum,  // cyl
                                         (BYTE)SectorNum,    // sector
                                         (BYTE)HeadNum,      // head
                                         (BYTE)byDrive))     // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }

                                        // Check boot signature 0x55AA

    if ( lpbyGood[0x1fe] != 0x55 ||
         lpbyGood[0x1ff] != 0xAA)
    {
        return (EXTSTATUS_NO_REPAIR);
    }
                                        // Write back Orignal boot sector

    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyGood,         // buffer
                                        (BYTE)1,                // count
                                        (WORD)0,                // cyl
                                        (BYTE)1,                // sector
                                        (BYTE)1,                // head
                                        (BYTE)byDrive))         // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    else
    {
        wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!
    }

    return (wReturn);
}

// EXTRepairBootWolleh arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD      EXTRepairBootWolleh(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)

{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    LPBYTE  lpbyBOOT, lpbyGood;
    BYTE    SectorNum, HeadNum;
    WORD    CylinderNum;
    BYTE    byEP;


    lpbyBOOT = lpbyWorkBuffer;
    lpbyGood = lpbyWorkBuffer + 512;

    //_asm    int 3                     // Remove when not debugging!
                                        // Get the boot sector

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyBOOT,         // buffer
                                        (BYTE)1,                // count
                                        (WORD)0,                // cyl
                                        (BYTE)1,                // sector
                                        (BYTE)1,                // head
                                        (BYTE)byDrive))         // drive

    {
        return (EXTSTATUS_NO_REPAIR);
    }

    byEP= lpbyBOOT[1]+2;

    // Get the first sector of the virus which contains the original
    // boot sector code that was overwritten

    SectorNum = lpbyBOOT[byEP+15];
    CylinderNum = (SectorNum & 0xc0) << 2 | lpbyBOOT[byEP+13];
    SectorNum &= 0x3f;
    HeadNum = lpbyBOOT[byEP+17];


    if (lpCallBack->SDiskReadPhysical(  (LPSTR)lpbyGood,      // buffer
                                         (BYTE)1,            // count
                                         (WORD)CylinderNum,  // cyl
                                         (BYTE)SectorNum,    // sector
                                         (BYTE)HeadNum,      // head
                                         (BYTE)byDrive))     // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }


    // copy the original code over the virus code.

    MEMCPY (lpbyBOOT+byEP, lpbyGood+0x17, 0x01e);



    // Write back Orignal boot sector

    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyBOOT,         // buffer
                                        (BYTE)1,                // count
                                        (WORD)0,                // cyl
                                        (BYTE)1,                // sector
                                        (BYTE)1,                // head
                                        (BYTE)byDrive))         // drive
    {
        return (EXTSTATUS_NO_REPAIR);
    }

    else
    {
        wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!
    }

    return (wReturn);
}


// EXTRepairBoot144Floppy arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD      EXTRepairBoot144Floppy(LPCALLBACKREV1 lpCallBack,
                             LPN30          lpsN30,
                             BYTE           byDrive,
                             BYTE           byPartitionNum,
                             WORD           wSector,
                             WORD           wCylinder,
                             WORD           wHead,
                             LPBYTE         lpbyWorkBuffer)

{
    WORD    wFloppyType;

    if ( 0x80 & byDrive )
        {
        return EXTSTATUS_NO_REPAIR;     // No repair for Hard Drives
        }

                                        // Read the boot sector
    if ( lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,
                                        1,
                                        wCylinder,
                                       (BYTE)wSector,
                                       (BYTE)wHead,
                                        byDrive))
        {
        return EXTSTATUS_DISK_READ_ERROR;
        }

                                        // Use it to determine floppy type
    if ( !NavexGetFloppyType ( lpbyWorkBuffer, &wFloppyType ))
        {
        wFloppyType = DISK_FLOPPY_144M;
//        return EXTSTATUS_NO_REPAIR;
        }

                                        // If it's a 144/288M floppy
    if ( DISK_FLOPPY_288M == wFloppyType )
    {
        wFloppyType = DISK_FLOPPY_144M; //force to 144M type
    }
                                        // Try a generic repair
    if ( !NavexRepairBootGeneric ( lpbyWorkBuffer, wFloppyType ))
    {
            return EXTSTATUS_NO_REPAIR;
    }
                                        // Write out the new sector
    if ( lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,
                                             1,
                                             wCylinder,
                                             (BYTE)wSector,
                                             (BYTE)wHead,
                                             byDrive))
    {
            return EXTSTATUS_DISK_WRITE_ERROR;
    }

    return EXTSTATUS_OK;

    //return EXTSTATUS_NO_REPAIR;

}



#if defined(SYM_DOS)

MODULE_EXPORT_TABLE_TYPE module_export_table =
{
  EXPORT_STRING,
  (void (*)())EXTRepairBoot
};

void main(void)
{
}

#endif


#endif  // #ifndef SYM_NLM
