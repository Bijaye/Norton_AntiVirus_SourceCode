//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexrp.cpv   1.46   09 Jul 1998 15:10:50   AOONWAL  $
//
// Description:
//      Contains NAVEX EXTRepairPart code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexrp.cpv  $
// 
//    Rev 1.46   09 Jul 1998 15:10:50   AOONWAL
// removed ifdef for crazy boot repair
// 
//    Rev 1.45   24 Jun 1998 18:04:22   RELNITI
// ADD #ifdef SARC_CST_2071 to Crazy Boot Repair code
// 
//    Rev 1.44   24 Jun 1998 10:55:18   RELNITI
// FIX Crazy Boot to repair 2nd Physical HD. Crazy Boot infects 2nd Physical
//  HD MBR too.
// 
//    Rev 1.43   23 Jun 1998 22:34:18   RELNITI
// MODIFY Crazy Boot to simply restore the MBR Partition code and
//  to overwrite the first 0xE4 bytes of the MBR with Generic Code.
// 
//    Rev 1.42   25 Mar 1998 20:43:54   FBARAJA
// Removed #ifdef SARC_CST_1582
//
//    Rev 1.41   25 Mar 1998 11:59:16   AOONWAL
// removed declaration int i; since it is not used in the repair of TPVO3464
//
//    Rev 1.40   24 Mar 1998 18:26:54   DKESSNE
// added repair for TPVO.3464 (b) 0x250c (#ifdef SARC_CST_1582)
//
//    Rev 1.39   04 Feb 1998 17:59:26   CFORMUL
// Removed an int 3
//
//    Rev 1.38   04 Feb 1998 12:22:52   CFORMUL
// Stupid parenthesis!
//
//    Rev 1.37   04 Feb 1998 11:28:20   CFORMUL
// Fixed a warning that was causing W32 stuff not to compile
//
//    Rev 1.36   03 Feb 1998 22:04:38   CFORMUL
// Fixed another bug in UN repair dealing with DWORDs
//
//    Rev 1.35   03 Feb 1998 19:41:52   CFORMUL
// Fixed a bug in the UN repair
//
//    Rev 1.34   02 Feb 1998 17:35:00   CFORMUL
// Added UN repair
//
//    Rev 1.33   20 Nov 1997 19:43:38   JWILBER
// Modified during Dec97 build.
//
//    Rev DEC97  17 Nov 1997 18:17:04   DCHI
// Added code_seg() pragma for SYM_WIN16.
//
//    Rev 1.32   11 Aug 1997 18:00:14   CFORMUL
// Fixed Ginger boot repair.
//
//    Rev 1.31   11 Aug 1997 16:41:32   CFORMUL
//
//    Rev 1.29   07 Aug 1997 16:57:38   CFORMUL
// Added Ginger detect/repair
//
//    Rev 1.28   19 Jun 1997 14:42:20   CFORMUL
// Added Repair for Stoned.Empire.Monkey
//
//    Rev 1.27   30 May 1997 12:08:50   CNACHEN
// Added LPVOID parameter to repair function to support UI.
//
//    Rev 1.26   26 Dec 1996 15:22:46   AOONWAL
// No change.
//
//    Rev 1.25   02 Dec 1996 14:00:52   AOONWAL
// No change.
//
//    Rev 1.24   29 Oct 1996 12:58:26   AOONWAL
// No change.
//
//    Rev 1.23   17 Jul 1996 16:55:54   CRENERT
// Fixed #endif bug.
//
//    Rev 1.22   17 Jul 1996 16:47:24   CRENERT
// #if defined out the Hare repair.
//
//    Rev 1.21   15 Jul 1996 12:07:54   MCANDELA
// uncomment frank and badhead
//
//    Rev 1.20   11 Jul 1996 17:09:18   CFORMUL
//
//    Rev 1.19   09 Jul 1996 16:59:28   CFORMUL
// Fixed bug in Hare Repair where I wasn't masking out two
// high bits of sector.
//
//    Rev 1.18   08 Jul 1996 17:38:00   CFORMUL
// Added repair for Hare.7610.A.
//
//    Rev 1.17   03 Jul 1996 02:46:12   CFORMUL
// Added MBR repair for Hare.7610.B
//
//    Rev 1.16   24 Jun 1996 21:22:00   JWILBER
// Added MBR repair for Danish Boot, 1f8f.
//
//    Rev 1.15   04 Jun 1996 12:20:20   CNACHEN
// No change.
//
//    Rev 1.14   04 Jun 1996 12:19:20   CNACHEN
//
//    Rev 1.13   18 Mar 1996 14:37:42   JSULTON
// Removed comments from Crazy Boot.
//
//    Rev 1.12   18 Mar 1996 11:54:48   JALLEE
// Increased bookmark verification for Crazy Boot MBR repair.
//
//    Rev 1.11   08 Mar 1996 17:52:34   CRENERT
// Changed UINT to WORD for Neuroquila
//
//    Rev 1.10   28 Feb 1996 13:07:36   JALLEE
// Added Esto te pasa MBR repair.
//
//    Rev 1.9   13 Feb 1996 17:56:02   MKEATIN
// Added DX Support
//
//    Rev 1.8   30 Jan 1996 15:43:32   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
//
//    Rev 1.7   18 Jan 1996 12:08:40   JALLEE
// Uncommented Urkel MBR repair as QA has passed it.
//
//    Rev 1.6   12 Dec 1995 18:41:26   JALLEE
// Voided unused parameters for NLM compile.
// (Bug fix for below changes.)
//
//    Rev 1.5   12 Dec 1995 17:57:36   JALLEE
// Added MBR repairs, but left them disabled pending QA.
//
//    Rev 1.4   01 Nov 1995 10:49:06   DCHI
// Removed include of navexhdr.h due to change in format of navexshr.h.
//
//    Rev 1.3   19 Oct 1995 14:20:52   DCHI
// Added LOADDS to external functions for WIN16.  Modified DOS export
// table structure to support tiny model modules.
//
//    Rev 1.2   18 Oct 1995 11:53:56   CNACHEN
// Voided unused parameters to compile correctly for NLM.
//
//    Rev 1.1   16 Oct 1995 13:04:34   DCHI
// Added include of navexhdr.h.
//
//    Rev 1.0   13 Oct 1995 13:05:36   DCHI
// Initial revision.
//
//************************************************************************

#ifdef SYM_WIN16
#pragma code_seg("NAVEX_NAVEXRP","NAVEX_PART_CODE")
#endif

#ifndef SYM_NLM

#include "platform.h"
#include "endutils.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navex.h"

// Declare shared routines

#include "navexshr.h"

#ifdef SARCBOT        //for Customer changes
#include "config.h"
#endif

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif

// *****************************************
//
// Prototypes for local MBR repair functions
//
// *****************************************

WORD EXTRepairPartBH(LPCALLBACKREV1 lpCallBack,
                   LPN30          lpsN30,
                   BYTE           byDrive,
                   LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartNeuro(LPCALLBACKREV1 lpCallBack,
                   LPN30          lpsN30,
                   BYTE           byDrive,
                   LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartFrank(LPCALLBACKREV1 lpCallBack,
                   LPN30          lpsN30,
                   BYTE           byDrive,
                   LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartCB(LPCALLBACKREV1 lpCallBack,
                   LPN30          lpsN30,
                   BYTE           byDrive,
                   LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartUrkel(LPCALLBACKREV1 lpCallBack,
                   LPN30          lpsN30,
                   BYTE           byDrive,
                   LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartETP(LPCALLBACKREV1 lpCallBack,
                   LPN30          lpsN30,
                   BYTE           byDrive,
                   LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartOneHalf(LPCALLBACKREV1 lpCallBack,
                   LPN30          lpsN30,
                   BYTE           byDrive,
                   LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartDanish(LPCALLBACKREV1 lpCallBack,
                   LPN30          lpsN30,
                   BYTE           byDrive,
                   LPBYTE         lpbyWorkBuffer);


#if defined(SYM_DOS) || defined(SYM_DOSX)
WORD EXTRepairPartHare(LPCALLBACKREV1 lpCallBack,
                   LPN30          lpsN30,
                   BYTE           byDrive,
                   LPBYTE         lpbyWorkBuffer);
#endif

WORD EXTRepairPartMonkey(LPCALLBACKREV1 lpCallBack,
                         LPN30          lpsN30,
                         BYTE           byDrive,
                         LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartGinger(LPCALLBACKREV1 lpCallBack,
                         LPN30          lpsN30,
                         BYTE           byDrive,
                         LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartUN(LPCALLBACKREV1 lpCallBack,
                     LPN30          lpsN30,
                     BYTE           byDrive,
                     LPBYTE         lpbyWorkBuffer);

WORD EXTRepairPartTPVO3464(LPCALLBACKREV1 lpCallBack,
                         LPN30          lpsN30,
                         BYTE           byDrive,
                         LPBYTE         lpbyWorkBuffer);



// *********************
//
// Navex native function
//
// *********************


// EXTRepairPart arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// wVersionNumber       : NAV version number.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
// lpbyWorkBuffer       : NULL pointer in version 1
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

#ifdef NAVEX15

EXTSTATUS FAR WINAPI NLOADDS EXTRepairPart(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           LPBYTE         lpbyWorkBuffer,
                                           LPVOID         lpvCookie)

#else

EXTSTATUS FAR WINAPI NLOADDS EXTRepairPart(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
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
    (void)lpbyWorkBuffer;

                                        // REP_EXEC_CODE defined in repair.inc
                                        // Check if external is called.


    if (lpsN30->wHardType == REP_EXEC_CODE)
    {
                                        // wHostJmp field contains ID of
                                        // repair to perform.  This kludgy
                                        // shared purpose goes way back.


        switch (lpsN30->wHardLoc1)
        {


            case ID_BH      : wReturn = EXTRepairPartBH (lpCallBack,
                                                         lpsN30,
                                                         byDrive,
                                                         lpbyWorkBuffer);
                              break;

            case ID_NEURO   : wReturn = EXTRepairPartNeuro (lpCallBack,
                                                            lpsN30,
                                                            byDrive,
                                                            lpbyWorkBuffer);
                              break;

            case ID_URKEL   : wReturn = EXTRepairPartUrkel (lpCallBack,
                                                            lpsN30,
                                                            byDrive,
                                                            lpbyWorkBuffer);
                              break;

            case ID_ETP     : wReturn = EXTRepairPartETP (lpCallBack,
                                                          lpsN30,
                                                          byDrive,
                                                          lpbyWorkBuffer);
                              break;

            case ID_CB      : wReturn = EXTRepairPartCB (lpCallBack,
                                                         lpsN30,
                                                         byDrive,
                                                         lpbyWorkBuffer);
                              break;

            case ID_ONEHALF : wReturn = EXTRepairPartOneHalf (lpCallBack,
                                                              lpsN30,
                                                              byDrive,
                                                              lpbyWorkBuffer);
                              break;

            case ID_DANEBOOT : wReturn = EXTRepairPartDanish (lpCallBack,
                                                              lpsN30,
                                                              byDrive,
                                                              lpbyWorkBuffer);
                              break;

#if defined(SYM_DOS) || defined(SYM_DOSX)
            case ID_HARE 	: wReturn = EXTRepairPartHare (lpCallBack,
                                                              lpsN30,
                                                              byDrive,
                                                              lpbyWorkBuffer);
                              break;
#endif


            case ID_FRANK   : wReturn = EXTRepairPartFrank (lpCallBack,
                                                            lpsN30,
                                                            byDrive,
                                                            lpbyWorkBuffer);
                              break;

            case ID_MONKEY   : wReturn = EXTRepairPartMonkey (lpCallBack,
                                                              lpsN30,
                                                              byDrive,
                                                              lpbyWorkBuffer);
                              break;

            case ID_GINGER   : wReturn = EXTRepairPartGinger (lpCallBack,
                                                              lpsN30,
                                                              byDrive,
                                                              lpbyWorkBuffer);
                              break;

            case ID_UN   : wReturn = EXTRepairPartUN (lpCallBack,
                                                      lpsN30,
                                                      byDrive,
                                                      lpbyWorkBuffer);
                              break;


            case ID_TPVO3464   : wReturn = EXTRepairPartTPVO3464 (lpCallBack,
                                                      lpsN30,
                                                      byDrive,
                                                      lpbyWorkBuffer);
                              break;


            default         : break;
        }

    }


#if defined(SYM_DOS)

    _asm
        {
        mov ds, cs:[wDS]
        mov es, cs:[wES]
        }

#endif


    return ( wReturn );
}

// ******************************
//
// Local functions for MBR repair
//
// ******************************


// EXTRepairPartBH
// Repair for Bad_Head
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartBH (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)

{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    UINT    i,k;
    (void)lpsN30;

                                        // Get the encrypted original MBR
                                        // at drv 80, head 0, cyl 0, sec 12.

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,     // buffer
                                        (BYTE)1,            // count
                                        (WORD)0,            // cyl
                                        (BYTE)12,           // sector
                                        (BYTE)0,            // head
                                        (BYTE)byDrive))     // drive
    {
        return (EXTSTATUS_NO_REPAIR);   // Return error if read fails.
    }
    k = 512;
    for (i=0;i<512;i++)                 // Do simple decrypt of stored MBR
    {
        lpbyWorkBuffer[i] = lpbyWorkBuffer[i] ^ (BYTE)k--;
    }
                                        // Quick integrity check decrypted MBR
                                        // Check that there is only one active
                                        // partition.
    k = lpbyWorkBuffer[0x1be] +
        lpbyWorkBuffer[0x1ce] +
        lpbyWorkBuffer[0x1de] +
        lpbyWorkBuffer[0x1ee];

    if (k != 0x80)
        return (EXTSTATUS_NO_REPAIR);

    if (0x55 == lpbyWorkBuffer[510] &&
        0xAA == lpbyWorkBuffer[511])
    {                                   // Write the decrypted MBR back
        if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyWorkBuffer,    // buffer
                                           (BYTE)1,            // count
                                           (WORD)0,            // cyl
                                           (BYTE)1,            // sec
                                           (BYTE)0,            // head
                                           (BYTE)byDrive))     // drive
        {
            wReturn = EXTSTATUS_NO_REPAIR;
        }
        else
            wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!

    }
    else
        wReturn = EXTSTATUS_NO_REPAIR;  // Hold up, no signature byte




    return (wReturn);
}

// EXTRepairPartCB
// Repair for Crazy Boot
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

#define MBR_CODE_SIZE 0xE3
WORD EXTRepairPartCB (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)
{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    LPBYTE  lpbyMBR, lpbyVirus;
    BYTE    byGenericMBR[] = {0xFA, 0x33, 0xC0, 0x8E, 0xD0, 0xBC, 0x00, 0x7C,
                              0x8B, 0xF4, 0x8E, 0xC0, 0x8E, 0xD8, 0xFB, 0xFC,
                              0xBF, 0x00, 0x06, 0xB9, 0x00, 0x01, 0xF2, 0xA5,
                              0xEA, 0x1D, 0x06, 0x00, 0x00, 0xBE, 0xBE, 0x07,
                              0xB3, 0x04, 0x80, 0x3C, 0x80, 0x74, 0x0E, 0x80,
                              0x3C, 0x00, 0x75, 0x1C, 0x83, 0xC6, 0x10, 0xFE,
                              0xCB, 0x75, 0xEF, 0xCD, 0x18, 0x8B, 0xEE, 0x8B,
                              0x14, 0x8B, 0x4C, 0x02, 0x83, 0xC6, 0x10, 0xFE,
                              0xCB, 0x74, 0x0A, 0x80, 0x3C, 0x00, 0x74, 0xF4,
                              0xBE, 0xC7, 0x06, 0xEB, 0x30, 0xBF, 0x04, 0x00,
                              0xB8, 0x01, 0x02, 0xBB, 0x00, 0x7C, 0x57, 0xCD,
                              0x13, 0x5F, 0x73, 0x0C, 0x33, 0xC0, 0xCD, 0x13,
                              0x4F, 0x75, 0xED, 0xBE, 0xA8, 0x06, 0xEB, 0x15,
                              0xBF, 0xFE, 0x7D, 0x81, 0x3D, 0x55, 0xAA, 0x74,
                              0x05, 0xBE, 0x8F, 0x06, 0xEB, 0x07, 0x8B, 0xF5,
                              0xEA, 0x00, 0x7C, 0x00, 0x00, 0xAC, 0x0A, 0xC0,
                              0x74, 0x0B, 0x56, 0xB4, 0x0E, 0xBB, 0x07, 0x00,
                              0xCD, 0x10, 0x5E, 0xEB, 0xF0, 0xEB, 0xFE, 0x4D,
                              0x69, 0x73, 0x73, 0x69, 0x6E, 0x67, 0x20, 0x6F,
                              0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6E, 0x67,
                              0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x00,
                              0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x6C, 0x6F,
                              0x61, 0x64, 0x69, 0x6E, 0x67, 0x20, 0x6F, 0x70,
                              0x65, 0x72, 0x61, 0x74, 0x69, 0x6E, 0x67, 0x20,
                              0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x00, 0x49,
                              0x6E, 0x76, 0x61, 0x6C, 0x69, 0x64, 0x20, 0x70,
                              0x61, 0x72, 0x74, 0x69, 0x74, 0x69, 0x6F, 0x6E,
                              0x20, 0x74, 0x61, 0x62, 0x6C, 0x65, 0x00, 0x42,
                              0x56, 0x41, 0x4E };

    WORD    i,k;

    (void)lpsN30;

    lpbyMBR = lpbyWorkBuffer;
    lpbyVirus = lpbyWorkBuffer + 512;

                                            // Get the MBR
        if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyMBR,       // buffer
                                            (BYTE)1,           // count
                                            (WORD)0,           // cyl
                                            (BYTE)1,           // sector
                                            (BYTE)0,           // head
                                            (BYTE)byDrive))    // drive
        {
            return(EXTSTATUS_NO_REPAIR);
        }
                                            // Double check infection.
        if (!(lpbyMBR[0] == 0xeb &&
              lpbyMBR[1] == 0x3e &&
              lpbyMBR[0x46] == 0xbc &&
              lpbyMBR[0x47] == 0x00))
        {
            return(EXTSTATUS_NO_REPAIR);
        }

                                            // Get virus
        if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyVirus,      // buffer
                                            (BYTE)1,            // count
                                            (WORD)0,            // cyl
                                            (BYTE)4,            // sector
                                            (BYTE)0,            // head
                                            (BYTE)byDrive))     // drive
        {
            return(EXTSTATUS_NO_REPAIR);
        }

                                            // Triple check infection.
        //if (*((LPDWORD)(lpbyVirus + 0x52)) != 0xcd0000ba)
        //{
        //    return(EXTSTATUS_NO_REPAIR);
        //}
        //restoring MBR code from Offset 64
        //for (i=0;i<32;i++)
        //{
        //    lpbyMBR[i + 64] = lpbyVirus[i];
        //}

        //restoring MBR Partition Information which is stored in the
        // infected MBR from offset 0x100
        for (i=0;i<64;i++)
        {
            lpbyMBR[0x1be + i] = lpbyMBR[0x100 + i];
            lpbyMBR[0x100 + i] = 0;
        }

        //restoring the first two bytes of MBR
        //lpbyMBR[0] = lpbyVirus[32];
        //lpbyMBR[1] = lpbyVirus[33];

        //Generic MBR Repair is better since CrazyBoot overwrites bytes
        // from offset 256-320, which may be part of MBR Code, i.e. OSR2
        for (i=0;i<MBR_CODE_SIZE;i++)
        {
            lpbyMBR[i] = byGenericMBR[i];
        }
                                            // Quick integrity check decrypted M
                                            // Check that there is only one acti
                                            // partition.
        k = lpbyMBR[0x1be] +
            lpbyMBR[0x1ce] +
            lpbyMBR[0x1de] +
            lpbyMBR[0x1ee];

        if (k != 0x80)
            return(EXTSTATUS_NO_REPAIR);

        if (0x55 == lpbyMBR[510] &&
            0xAA == lpbyMBR[511])
        {                                   // Write the decrypted MBR back
            if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyMBR,    // buffer
                                               (BYTE)1,            // count
                                               (WORD)0,            // cyl
                                               (BYTE)1,            // sec
                                               (BYTE)0,            // head
                                               (BYTE)byDrive))     // drive
            {
                return(EXTSTATUS_NO_REPAIR);;
            }
            else
                wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!
        }
        else
            return(EXTSTATUS_NO_REPAIR);

    return (wReturn);
}

///previous repair /////WORD EXTRepairPartCB (LPCALLBACKREV1 lpCallBack,
///previous repair /////                      LPN30          lpsN30,
///previous repair /////                      BYTE           byDrive,
///previous repair /////                      LPBYTE         lpbyWorkBuffer)
///previous repair /////{
///previous repair /////    WORD    wReturn = EXTSTATUS_NO_REPAIR;
///previous repair /////    LPBYTE  lpbyMBR, lpbyVirus;
///previous repair /////    WORD    i,k;
///previous repair /////
///previous repair /////    (void)lpsN30;
///previous repair /////
///previous repair /////    lpbyMBR = lpbyWorkBuffer;
///previous repair /////    lpbyVirus = lpbyWorkBuffer + 512;
///previous repair /////
///previous repair /////                                            // Get the MBR
///previous repair /////
///previous repair /////        if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyMBR,       // buffer
///previous repair /////                                            (BYTE)1,           // count
///previous repair /////                                            (WORD)0,           // cyl
///previous repair /////                                            (BYTE)1,           // sector
///previous repair /////                                            (BYTE)0,           // head
///previous repair /////                                            (BYTE)byDrive))    // drive
///previous repair /////        {
///previous repair /////            return(EXTSTATUS_NO_REPAIR);
///previous repair /////        }
///previous repair /////                                            // Double check infection.
///previous repair /////        if (!(lpbyMBR[0] == 0xeb &&
///previous repair /////              lpbyMBR[1] == 0x3e &&
///previous repair /////              lpbyMBR[0x46] == 0xbc &&
///previous repair /////              lpbyMBR[0x47] == 0x00))
///previous repair /////        {
///previous repair /////            return(EXTSTATUS_NO_REPAIR);
///previous repair /////        }
///previous repair /////
///previous repair /////                                            // Get virus
///previous repair /////        if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyVirus,      // buffer
///previous repair /////                                            (BYTE)1,            // count
///previous repair /////                                            (WORD)0,            // cyl
///previous repair /////                                            (BYTE)4,            // sector
///previous repair /////                                            (BYTE)0,            // head
///previous repair /////                                            (BYTE)byDrive))     // drive
///previous repair /////        {
///previous repair /////            return(EXTSTATUS_NO_REPAIR);
///previous repair /////        }
///previous repair /////
///previous repair /////                                            // Triple check infection.
///previous repair /////        if (*((LPDWORD)(lpbyVirus + 0x52)) != 0xcd0080ba)
///previous repair /////        {
///previous repair /////            return(EXTSTATUS_NO_REPAIR);
///previous repair /////        }
///previous repair /////        for (i=0;i<32;i++)
///previous repair /////        {
///previous repair /////            lpbyMBR[i + 64] = lpbyVirus[i];
///previous repair /////        }
///previous repair /////        for (i=0;i<64;i++)
///previous repair /////        {
///previous repair /////            lpbyMBR[0x1be + i] = lpbyMBR[0x100 + i];
///previous repair /////            lpbyMBR[0x100 + i] = 0;
///previous repair /////        }
///previous repair /////        lpbyMBR[0] = lpbyVirus[32];
///previous repair /////        lpbyMBR[1] = lpbyVirus[33];
///previous repair /////                                            // Quick integrity check decrypted M
///previous repair /////                                            // Check that there is only one acti
///previous repair /////                                            // partition.
///previous repair /////        k = lpbyMBR[0x1be] +
///previous repair /////            lpbyMBR[0x1ce] +
///previous repair /////            lpbyMBR[0x1de] +
///previous repair /////            lpbyMBR[0x1ee];
///previous repair /////
///previous repair /////        if (k != 0x80)
///previous repair /////            return(EXTSTATUS_NO_REPAIR);
///previous repair /////
///previous repair /////        if (0x55 == lpbyMBR[510] &&
///previous repair /////            0xAA == lpbyMBR[511])
///previous repair /////        {                                   // Write the decrypted MBR back
///previous repair /////            if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyMBR,    // buffer
///previous repair /////                                               (BYTE)1,            // count
///previous repair /////                                               (WORD)0,            // cyl
///previous repair /////                                               (BYTE)1,            // sec
///previous repair /////                                               (BYTE)0,            // head
///previous repair /////                                               (BYTE)byDrive))     // drive
///previous repair /////            {
///previous repair /////                return(EXTSTATUS_NO_REPAIR);;
///previous repair /////            }
///previous repair /////            else
///previous repair /////                wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!
///previous repair /////        }
///previous repair /////        else
///previous repair /////            return(EXTSTATUS_NO_REPAIR);
///previous repair /////
///previous repair /////    return (wReturn);
///previous repair /////}

// EXTRepairPartFrank
// Repair for Frankenstein
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartFrank (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)
{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    WORD    wCyl, i ,k;
    BYTE    bySec, byHead;

    (void)lpsN30;

                                        // Get MBR
    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,     // buffer
                                        (BYTE)1,            // count
                                        (WORD)0,            // cyl
                                        (BYTE)1,            // sector
                                        (BYTE)0,            // head
                                        (BYTE)byDrive))     // drive
    {
        return (EXTSTATUS_NO_REPAIR);   // Return error if read fails.
    }
                                        // Locate original MBR
    wCyl = lpbyWorkBuffer[0x4a];              //    ; cyl
    bySec = lpbyWorkBuffer[0x4c];             //    ; sec
    byHead = lpbyWorkBuffer[0x4b];            //    ; head

                                        // Read original MBR
    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,     // buffer
                                        1,                  // count
                                        wCyl,               // cyl
                                        bySec,              // sector
                                        byHead,             // head
                                        byDrive))           // drive
    {
        return (EXTSTATUS_NO_REPAIR);   // Return error if read fails.
    }



                                        // Decrypt
    k = 512;
    for (i=0;i<512;i++)
    {
        lpbyWorkBuffer[i] ^= (BYTE)k--;
        lpbyWorkBuffer[i] ^= (BYTE)0x4a;
    }

                                        // Quick integrity check decrypted MBR
    k = lpbyWorkBuffer[0x1be] +
        lpbyWorkBuffer[0x1ce] +
        lpbyWorkBuffer[0x1de] +
        lpbyWorkBuffer[0x1ee];

    if (k != 0x80)
        return (EXTSTATUS_NO_REPAIR);


    if (0x55 == lpbyWorkBuffer[510] &&
        0xAA == lpbyWorkBuffer[511])
    {                                   // Write the decrypted MBR back
        if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyWorkBuffer,    // buffer
                                           (BYTE)1,            // count
                                           (WORD)0,            // cyl
                                           (BYTE)1,            // sec
                                           (BYTE)0,            // head
                                           (BYTE)byDrive))     // drive
        {
            wReturn = EXTSTATUS_NO_REPAIR;
        }
        else
            wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!

    }
    else
        wReturn = EXTSTATUS_NO_REPAIR;  // Hold up, no signature byte

    return (EXTSTATUS_OK);
}

// EXTRepairPartUrkel
// Repair for Urkel
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartUrkel (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)

{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    UINT    i,k;

    (void)lpsN30;


                                        // Get the encrypted original MBR
                                        // at drv 80, head 0, cyl 0, sec 12.

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,     // buffer
                                        (BYTE)1,            // count
                                        (WORD)0,            // cyl
                                        (BYTE)5,           // sector
                                        (BYTE)0,            // head
                                        (BYTE)byDrive))     // drive
    {
        return (EXTSTATUS_NO_REPAIR);   // Return error if read fails.
    }
    for (i=0;i<512;i++)                 // Do simple decrypt of stored MBR
    {
        lpbyWorkBuffer[i] = lpbyWorkBuffer[i] ^ (BYTE)i;
    }
                                        // Quick integrity check decrypted MBR
    k = lpbyWorkBuffer[0x1be] +
        lpbyWorkBuffer[0x1ce] +
        lpbyWorkBuffer[0x1de] +
        lpbyWorkBuffer[0x1ee];

    if (k != 0x80)
        return (EXTSTATUS_NO_REPAIR);


    if (0x55 == lpbyWorkBuffer[510] &&
        0xAA == lpbyWorkBuffer[511])
    {                                   // Write the decrypted MBR back
        if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyWorkBuffer,    // buffer
                                           (BYTE)1,            // count
                                           (WORD)0,            // cyl
                                           (BYTE)1,            // sec
                                           (BYTE)0,            // head
                                           (BYTE)byDrive))     // drive
        {
            wReturn = EXTSTATUS_NO_REPAIR;
        }
        else
            wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!

    }
    else
        wReturn = EXTSTATUS_NO_REPAIR;  // Hold up, no signature byte




    return (wReturn);
}

// EXTRepairPartNeuro
// Repair for Neuroquila
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartNeuro (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)

{
    LPWORD  lpwOldMBR, lpwHBR;
    WORD    wCyl;
    LPBYTE  lpbyOldMBR;
    WORD    i,k;
    BYTE    bySec, byHead, byActive=0;

    (void)lpsN30;


    lpwOldMBR = (LPWORD) lpbyWorkBuffer;
    lpwHBR = (LPWORD) (lpbyWorkBuffer + 512);
                                        // First get the encrypted original
                                        // MBR at drv 80, head 0, cyl 0, sec 7
    lpbyOldMBR = (LPBYTE)lpwOldMBR;
    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpwOldMBR,     // buffer
                                        (BYTE)1,            // count
                                        (WORD)0,            // cyl
                                        (BYTE)7,            // sector
                                        (BYTE)0,            // head
                                        (BYTE)byDrive))     // drive
    {
        return (EXTSTATUS_NO_REPAIR);   // Return error if read fails.
    }

                                        // Decrypt
    k = 0xdead;
    for (i=0;i<255;i++)
    {
        lpwOldMBR[i] ^= k;
        k += 0x7f;
    }
                                        // Find the HBR
    k = 0x1be;
    for(i=0;i<4;i++)
    {
        if (0x80 == lpbyOldMBR[k])
        {

            if(byActive)
            {
                                        // There is more than one active
                                        // partition.  Our decrypted MBR
                                        // therefore fails integrity check.
                return (EXTSTATUS_NO_REPAIR);
            }
            byActive = 1;
                                        // Head is stored in byte following
                                        // the 80h which stands both for
                                        // drive 80 and for active partition.
            byHead = lpbyOldMBR[k + 1];
                                        // The sec is stored in the 6 lo
                                        // order bits of the word following
                                        // Drive / Head word.
            bySec = lpbyOldMBR[k + 2] & 0x3f;
            wCyl = (WORD) lpbyOldMBR[k + 3] +
                   ((WORD) (lpbyOldMBR[k + 2] & 0xc0)) << 8;

        }
        k += 16;
    }

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpwHBR,     // buffer
                                        1,                  // count
                                        wCyl,               // cyl
                                        bySec,              // sector
                                        byHead,             // head
                                        byDrive))           // drive
    {
        return (EXTSTATUS_NO_REPAIR);   // Return error if read fails.
    }

                                        // Decrypt HBR
    k = 0xdead << byHead;

    for (i=0;i<255;i++)
    {
        lpwHBR[i] ^= k;
        k += 0x7f;
    }

                                        // Integrity Check HBR
    if (0xe0 != ((BYTE) lpwHBR[0] & 0xf0))
    {
        return (EXTSTATUS_NO_REPAIR);   // The HBR should start with a jump.
    }
                                        // Write it all out!
                                        // MBR
    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpwOldMBR,     // buffer
                                        (BYTE)1,            // count
                                        (WORD)0,            // cyl
                                        (BYTE)1,            // sector
                                        (BYTE)0,            // head
                                        (BYTE)byDrive))     // drive
    {
        return (EXTSTATUS_NO_REPAIR);   // Return error if read fails.
    }
                                        // HBR
    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpwHBR,     // buffer
                                        1,                  // count
                                        wCyl,               // cyl
                                        bySec,              // sector
                                        byHead,             // head
                                        byDrive))           // drive
    {
        return (EXTSTATUS_NO_REPAIR);   // Return error if read fails.
    }
 
    return (EXTSTATUS_OK);
}

// EXTRepairPartETP
// Repair for Esto te Pasa
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartETP (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)

{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    UINT    i,k;
    (void)lpsN30;

                                        // Get the encrypted original MBR
                                        // at drv 80, head 0, cyl 0, sec 12.

    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,     // buffer
                                        (BYTE)1,            // count
                                        (WORD)0,            // cyl
                                        (BYTE)7,           // sector
                                        (BYTE)0,            // head
                                        (BYTE)byDrive))     // drive
    {
        return (EXTSTATUS_NO_REPAIR);   // Return error if read fails.
    }
    k = 512;
    for (i=0;i<512;i++)                 // Do simple decrypt of stored MBR
    {
        lpbyWorkBuffer[i] = ~lpbyWorkBuffer[i];
    }
                                        // Quick integrity check decrypted MBR
                                        // Check that there is only one active
                                        // partition.
    k = lpbyWorkBuffer[0x1be] +
        lpbyWorkBuffer[0x1ce] +
        lpbyWorkBuffer[0x1de] +
        lpbyWorkBuffer[0x1ee];

    if (k != 0x80)
        return (EXTSTATUS_NO_REPAIR);

    if (0x55 == lpbyWorkBuffer[510] &&
        0xAA == lpbyWorkBuffer[511])
    {                                   // Write the decrypted MBR back
        if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyWorkBuffer,    // buffer
                                           (BYTE)1,            // count
                                           (WORD)0,            // cyl
                                           (BYTE)1,            // sec
                                           (BYTE)0,            // head
                                           (BYTE)byDrive))     // drive
        {
            wReturn = EXTSTATUS_NO_REPAIR;
        }
        else
            wReturn = EXTSTATUS_OK;     // Whooya, NAV does it again!

    }
    else
        wReturn = EXTSTATUS_NO_REPAIR;  // Hold up, no signature byte




    return (wReturn);
}

// EXTRepairPartDanish
// Repair for Danish Boot
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
// lpbyWorkBuffer       : 2 - 4k buffer for temporary operations
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk writy error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//
//  This virus only changes part of the original MBR by overwriting it.  The
// bytes that are overwritten are saved on parts of sector 7, where other
// virus code resides.  This repair reads both sectors, and puts the correct
// parts together to recover the original MBR.

#define SECSIZE     0x200               // Size of sector
#define DANESRC1    0x3b                // Offset of 1st set of bytes to move
#define DANEDES1    0                   // Where to move first set
#define DANEMOVE1   3                   // Number of bytes to move first
#define DANESRC2    0x3e                // Offset of 2nd set of bytes to move
#define DANEDES2    0x3e                // Offset of 2nd set of bytes to move
#define DANEMOVE2   0x0f                // Number of bytes to move next

WORD EXTRepairPartDanish (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)

{
    WORD    i;
    (void)lpsN30;
                                        // Get current MBR
    if (lpCallBack->SDiskReadPhysical((LPSTR) lpbyWorkBuffer,   // buffer
                                        (BYTE) 1,           // count
                                        (WORD) 0,           // cyl
                                        (BYTE) 1,           // sector
                                        (BYTE) 0,           // head
                                        byDrive))           // drive
        return(EXTSTATUS_DISK_READ_ERROR);  // Return error if read fails.
                                        // Get Viral MBR
    if (lpCallBack->SDiskReadPhysical((LPSTR) lpbyWorkBuffer + SECSIZE,
                                        (BYTE) 1,           // count
                                        (WORD) 0,           // cyl
                                        (BYTE) 7,           // sector
                                        (BYTE) 0,           // head
                                        byDrive))           // drive
        return(EXTSTATUS_DISK_READ_ERROR);  // Return error if read fails.

    for (i = 0; i < DANEMOVE1; i++)
        *(lpbyWorkBuffer + DANEDES1 + i) =
                                *(lpbyWorkBuffer + SECSIZE + DANESRC1 + i);

    for (i = 0; i < DANEMOVE2; i++)
        *(lpbyWorkBuffer + DANEDES2 + i) =
                                *(lpbyWorkBuffer + SECSIZE + DANESRC2 + i);

    if (GOODSEC != *((LPWORD) (lpbyWorkBuffer + CHECK_VAL_OFF)))
        return(EXTSTATUS_NO_REPAIR);

    if (lpCallBack->SDiskWritePhysical((LPSTR) lpbyWorkBuffer,  // buffer
                                        (BYTE) 1,               // count
                                        (WORD) 0,               // cyl
                                        (BYTE) 1,               // sector
                                        (BYTE) 0,               // head
                                        byDrive))               // drive
        return(EXTSTATUS_DISK_WRITE_ERROR);

    return(EXTSTATUS_OK);
}


#if defined(SYM_DOS) || defined(SYM_DOSX)
// EXTRepairPartHare
// Repair for Hare.7610.A and Hare.7610.B
//
// This code needs to use int 13 funtion 8 in order to determine
// where the original MBR is stored.
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
// lpbyWorkBuffer       : 2 - 4k buffer for temporary operations
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk writy error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//


WORD EXTRepairPartHare (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)
{
    WORD	wCyl;
    BYTE	byHead, bySec;


	// Both varients of Hare store the MBR at the some location

	_asm {
			mov     ah, 08h
			mov		dl, 80h
			int		13h
			inc		ch
			dec		dh
			mov		byHead, dh
			mov		bySec, cl
			and		bySec, 03fh
			mov		ax, cx
			xchg	ah,al
			mov		cl, 6
			shr		ah, cl
			mov		wCyl, ax
		 }


    if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,     // buffer
                                        (BYTE)1,            // count
                                        wCyl,               // cyl
                                        bySec,              // sector
                                        byHead,             // head
                                        (BYTE)0x80))        // drive
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }

    if (lpCallBack->SDiskWritePhysical( (LPSTR)lpbyWorkBuffer,     // buffer
                                        (BYTE)1,                   // count
                                        (WORD)0,                   // cyl
                                        (BYTE)1,                   // sector
                                        (BYTE)0,                   // head
                                        byDrive))                  // drive
    {
        return (EXTSTATUS_DISK_WRITE_ERROR);
    }


	return (EXTSTATUS_OK);

}
#endif


// EXTRepairPartMonkey
// Repair for Stoned.Empire.Monkey.A/B
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
// lpbyWorkBuffer       : 2 - 4k buffer for temporary operations
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk writy error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartMonkey (LPCALLBACKREV1 lpCallBack,
                          LPN30          lpsN30,
                          BYTE           byDrive,
                          LPBYTE         lpbyWorkBuffer)
{
    WORD	wCylinder;
    BYTE	byHead;
    BYTE    bySector;
    BYTE    byCount;
    BYTE    byKey;
    int     i;

    // Read in Encrypted MBR

    wCylinder = 0;
    byHead = 0;
    bySector = 3;
    byCount = 1;
    if (lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,
                                      byCount,
                                      wCylinder,
                                      bySector,
                                      byHead,
                                      byDrive))
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }


    // Decrypt Original MBR

    byKey = 0x2e;
    for (i = 0; i < 512; i++)
        lpbyWorkBuffer[i] ^= byKey;


    // Write it back out
 
    bySector = 1;
    if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyWorkBuffer,
                                       byCount,
                                       wCylinder,
                                       bySector,
                                       byHead,
                                       byDrive))
    {
        return (EXTSTATUS_DISK_WRITE_ERROR);
    }


	return (EXTSTATUS_OK);

}

// EXTRepairPartGinger
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
// lpbyWorkBuffer       : 2 - 4k buffer for temporary operations
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk writy error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartGinger (LPCALLBACKREV1 lpCallBack,
                          LPN30          lpsN30,
                          BYTE           byDrive,
                          LPBYTE         lpbyWorkBuffer)
{
    WORD    wCylinder;
    WORD    wPartLoc;
    BYTE    byHead;
    BYTE    bySector;
    BYTE    byCount;

    int     nPartEnt;


    // Read in MBR

    wCylinder = 0;
    byHead = 0;
    bySector = 1;
    byCount = 1;
    if (lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,
                                      byCount,
                                      wCylinder,
                                      bySector,
                                      byHead,
                                      byDrive))
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }


    // find the bootable partition

    for (nPartEnt = 0; nPartEnt < 4; nPartEnt++)
    {
        if (lpbyWorkBuffer[0x1be + (nPartEnt * 0x10)] == 0x80)
        {
            // We found the bootable partition

            wPartLoc = AVDEREF_WORD (lpbyWorkBuffer + 0x1bf + (nPartEnt * 0x10));
 
            if (wPartLoc == 0x200)
            {
                AVASSIGN_WORD (lpbyWorkBuffer + 0x1bf + (nPartEnt * 0x10), 0x101);
                break;
            }

        }
    }


    if (nPartEnt >= 4)
    {
        // There's no bootable partition

        return (EXTSTATUS_NO_REPAIR);
    }


    // Write it back out
 
    bySector = 1;
    if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyWorkBuffer,
                                       byCount,
                                       wCylinder,
                                       bySector,
                                       byHead,
                                       byDrive))
    {
        return (EXTSTATUS_DISK_WRITE_ERROR);
    }


    return (EXTSTATUS_OK);

}


// EXTRepairPartUN
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
// lpbyWorkBuffer       : 2 - 4k buffer for temporary operations
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk writy error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartUN (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)
{
    WORD    wCylinder;
    BYTE    byHead;
    BYTE    bySector;
    BYTE    byCount;

    WORD    wWordReg; 
    DWORD   dwDWordReg;

    // Read in MBR

    wCylinder = 0;
    byHead = 0;
    bySector = 1;
    byCount = 1;
    if (lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,
                                      byCount,
                                      wCylinder,
                                      bySector,
                                      byHead,
                                      byDrive))
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }


    // Calculate where the original MBR is located.

    wWordReg   = AVDEREF_WORD (lpbyWorkBuffer + 0x0e) +
                 AVDEREF_WORD (lpbyWorkBuffer + 0x16) * 2 +
                 AVDEREF_WORD (lpbyWorkBuffer + 0x11) / 0x10;
    dwDWordReg = (DWORD) (AVDEREF_WORD (lpbyWorkBuffer + 0x03) - 2) * 
                 (WORD) lpbyWorkBuffer[0x0d] +
                 wWordReg + AVDEREF_WORD (lpbyWorkBuffer + 0x1c) + 1;
    wWordReg   = (WORD) (dwDWordReg % AVDEREF_WORD (lpbyWorkBuffer + 0x18));
    dwDWordReg = (DWORD) dwDWordReg / AVDEREF_WORD (lpbyWorkBuffer + 0x18);

    byHead = (BYTE) (dwDWordReg % AVDEREF_WORD (lpbyWorkBuffer + 0x1a));
    bySector = (BYTE) (wWordReg & 0x003F) + 3;
    wCylinder = ((wWordReg & 0x00C0) << 2) | 
                (WORD) (dwDWordReg / AVDEREF_WORD (lpbyWorkBuffer + 0x1a));

    // Read in the original MBR
    if (lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,
                                      byCount,
                                      wCylinder,
                                      bySector,
                                      byHead,
                                      byDrive))
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }
    
    // Is this an MBR?

    if (AVDEREF_WORD (lpbyWorkBuffer + 0x1fe) != 0xAA55)
        return (EXTSTATUS_NO_REPAIR);
    

    // Write it back out
 
    byHead = 0;
    wCylinder = 0;
    bySector = 1;
    if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyWorkBuffer,
                                       byCount,
                                       wCylinder,
                                       bySector,
                                       byHead,
                                       byDrive))
    {
        return (EXTSTATUS_DISK_WRITE_ERROR);
    }


    return (EXTSTATUS_OK);

}


// EXTRepairPartTPVO3464
// Repair for TPVO.3464 (b)  VID 0x250c -- Darren K. 3/23/98
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
// lpbyWorkBuffer       : 2 - 4k buffer for temporary operations
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk writy error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartTPVO3464 (LPCALLBACKREV1 lpCallBack,
                          LPN30          lpsN30,
                          BYTE           byDrive,
                          LPBYTE         lpbyWorkBuffer)
{
    WORD	wCylinder;
    BYTE	byHead;
    BYTE    bySector;
    BYTE    byCount;
    BYTE    byCH, byCL;

    //read in infected MBR

    wCylinder = 0;
    byHead    = 0;
    bySector  = 1;
    byCount   = 1;
    if (lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,
                                      byCount,
                                      wCylinder,
                                      bySector,
                                      byHead,
                                      byDrive))
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }


    //calculate location of clean MBR

    byCL   = lpbyWorkBuffer[0x4c];
    byCH   = lpbyWorkBuffer[0x4d];
    byHead = lpbyWorkBuffer[0x50];

    wCylinder = byCH + ( (byCL & 0300) << 2 );   //this is the usual int 13
    bySector = byCL & 0077;                      //coding of cylinder/sector
                                                 //in the CX register
    bySector += 7;


    //read in clean MBR
    if (lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,
                                      byCount,
                                      wCylinder,
                                      bySector,
                                      byHead,
                                      byDrive))
    {
        return (EXTSTATUS_DISK_READ_ERROR);
    }


    //write out clean MBR
    wCylinder = 0;
    byHead    = 0;
    bySector  = 1;
    if (lpCallBack->SDiskWritePhysical((LPSTR)lpbyWorkBuffer,
                                       byCount,
                                       wCylinder,
                                       bySector,
                                       byHead,
                                       byDrive))
    {
        return (EXTSTATUS_DISK_WRITE_ERROR);
    }


	return (EXTSTATUS_OK);

}//end EXTReapairPartTPVO3464





#if defined(SYM_DOS)

MODULE_EXPORT_TABLE_TYPE module_export_table =
{
  EXPORT_STRING,
  (void (*)())EXTRepairPart
};

void main(void)
{
}

#endif

#endif  // #ifndef SYM_NLM
