//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexrp.cpv   1.26   26 Dec 1996 15:22:46   AOONWAL  $
//
// Description:
//      Contains NAVEX EXTRepairPart code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexrp.cpv  $
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
// 
//    Rev 1.22   17 Jul 1996 16:47:24   CRENERT
// #if defined out the Hare repair.
// 
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
// 
//    Rev 1.18   08 Jul 1996 17:38:00   CFORMUL
// Added repair for Hare.7610.A
// 
// 
//    Rev 1.17   03 Jul 1996 02:46:12   CFORMUL
// Added MBR repair for Hare.7610.B
// 
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

EXTSTATUS FAR WINAPI NLOADDS EXTRepairPart(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           LPBYTE         lpbyWorkBuffer)
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

WORD EXTRepairPartCB (LPCALLBACKREV1 lpCallBack,
                      LPN30          lpsN30,
                      BYTE           byDrive,
                      LPBYTE         lpbyWorkBuffer)
{
    WORD    wReturn = EXTSTATUS_NO_REPAIR;
    LPBYTE  lpbyMBR, lpbyVirus;
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
        if (*((LPDWORD)(lpbyVirus + 0x52)) != 0xcd0080ba)
        {
            return(EXTSTATUS_NO_REPAIR);
        }
        for (i=0;i<32;i++)
        {
            lpbyMBR[i + 64] = lpbyVirus[i];
        }
        for (i=0;i<64;i++)
        {
            lpbyMBR[0x1be + i] = lpbyMBR[0x100 + i];
            lpbyMBR[0x100 + i] = 0;
        }
        lpbyMBR[0] = lpbyVirus[32];
        lpbyMBR[1] = lpbyVirus[33];
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
