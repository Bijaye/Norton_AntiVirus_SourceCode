#include <stdio.h>

//************************************************************************
//
// $Header:   S:/NAVEX/VCS/NAVEXSP.CPv   1.52   20 Nov 1998 16:08:32   rpulint  $
//
// Description:
//      Contains NAVEX EXTScanPart code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/NAVEXSP.CPv  $
// 
//    Rev 1.52   20 Nov 1998 16:08:32   rpulint
// WATCOM Changes: moved extern declarations to global section from with 
// -in functions
// 
//    Rev 1.51   05 Aug 1998 18:38:42   hjaland
// Carey added code to disable the IBM scanner if he find an excluded boot rec.
// 
//    Rev 1.50   26 Jun 1998 17:37:28   relniti
// FIX ExtScanPartCE bysearch which was replaced accidentally on Rev 1.31
// 
//    Rev 1.49   10 Jun 1998 13:42:20   AOONWAL
// Removed printf statement
// 
//    Rev 1.48   09 Jun 1998 16:55:58   CNACHEN
// updated to call naveng...
// 
//    Rev 1.47   10 Mar 1998 17:08:52   CFORMUL
// Malloc'd 2K for lpbyWorkBuffer.  Commented out temp buffers and
// used lpbyWorkBuffer instead in individual scan functions.
// 
//    Rev 1.46   11 Feb 1998 18:14:46   JWILBER
// Fixed Win32 crashing bug in MBR scanning.
//
//    Rev 1.45   10 Feb 1998 20:26:26   CFORMUL
// Added Detection for Invisible
//
//    Rev 1.44   29 Dec 1997 17:56:06   JWILBER
// Modified during Jan98 build.
//
//    Rev JAN98  19 Dec 1997 16:18:36   ECHIEN
// Added 0xcd 0x13 to the valid byte set
//
//    Rev JAN98  19 Dec 1997 12:47:44   ECHIEN
// Added 0xb9 to the litlith valid set
//
//    Rev JAN98  17 Dec 1997 17:38:38   ECHIEN
// Added Lilith
//
//    Rev 1.43   08 Dec 1997 10:15:46   CNACHEN
// Disabled bloodhound-boot/mbr for all but picasso and NAVEX1.5 NAV products.
//
//    Rev 1.42   02 Dec 1997 11:49:56   CNACHEN
// Added creation of NAVBOOFP.DAT file in C:\ if the file C:\MAKEBOO.DAT is
// found, when a bloodhound detection occurs.
//
//    Rev 1.41   20 Nov 1997 19:45:32   JWILBER
// Modified during Dec97 build.
//
//    Rev DEC97  17 Nov 1997 18:17:08   DCHI
// Added code_seg() pragma for SYM_WIN16.
//
//    Rev 1.40   10 Nov 1997 16:47:58   CNACHEN
// Removed bloodhound ifdefs
//
//    Rev 1.39   27 Oct 1997 12:27:46   JWILBER
// Modified during NOV 97 build.
//
//    Rev NOV97  20 Oct 1997 13:12:04   CFORMUL
// Made change to scan string function for cHaN detection.
//
//    Rev NOV97  20 Oct 1997 12:22:58   CFORMUL
// Made a change to cHaN detection.
//
//    Rev 1.38   16 Oct 1997 17:16:16   CFORMUL
// Made a change to the detection for cHaN
//
//    Rev 1.38   16 Oct 1997 13:00:50   CFORMUL
// Changed scan string for cHaN def.
//
//    Rev 1.37   15 Sep 1997 11:48:00   CNACHEN
// Added check for USE_BLOOD_BOOT ifdef
//
//    Rev 1.36   05 Sep 1997 20:26:32   CNACHEN
// Added bloodhound boot.
//
//    Rev 1.35   05 Sep 1997 20:25:34   CNACHEN
// No change.
//
//    Rev 1.34   05 Sep 1997 20:25:12   CNACHEN
//
//    Rev 1.33   25 Aug 1997 17:38:18   JWILBER
// Modified during Sep 97 build.
//
//    Rev 1.33   21 Aug 1997 14:12:34   RELNITI
// ADD EXTScanPartChan (0x1d69).
//
//    Rev 1.32   07 Aug 1997 16:57:24   CFORMUL
// Added Ginger detect/repair.
//
//    Rev 1.31   06 Aug 1997 18:41:32   CFORMUL
// Added detection for Midsummer
//
//    Rev 1.30   28 Jul 1997 12:53:00   AOONWAL
// Modified during AUG97 build
//
//    Rev 1.29   17 Jul 1997 18:23:32   JWILBER
// Added EXTScanPartOneHalf, to detect OneHalf and Shin variants in MBRs.
// This routine calls EXTScanOneHalf in DANISH.CPP.
//
//    Rev 1.28   25 Mar 1997 21:49:20   RELNITI
// ADD EXTScanPartCE to let NAV fix the MBR of CE infected system
//
//    Rev 1.27   26 Dec 1996 15:22:58   AOONWAL
// No change.
//
//    Rev 1.26   02 Dec 1996 14:01:40   AOONWAL
// No change.
//
//    Rev 1.25   29 Oct 1996 12:58:34   AOONWAL
// No change.
//
//    Rev 1.24   16 Aug 1996 18:22:30   CFORMUL
// Fixed bug in Hare code under 32-bit.
//
//    Rev 1.23   24 Jul 1996 18:21:06   SCOURSE
// Finalized (pre-QA) Moloch
//
//    Rev 1.22   15 Jul 1996 19:32:42   CRENERT
// Removed int 3
//
//    Rev 1.21   11 Jul 1996 17:07:18   CFORMUL
// Had to make a few changes to Hare detection to make it stronger
//
//    Rev 1.20   11 Jul 1996 16:09:28   CFORMUL
// Modified Hare detection so it no longer calls int 13
//
//    Rev 1.19   09 Jul 1996 16:58:40   CFORMUL
// Fixed bug in Hare detection where I wasn't masking out two
// high bits of segment.
//
//    Rev 1.18   08 Jul 1996 17:38:42   CFORMUL
// Added Detection for Hare.7610.A
//
//    Rev 1.17   08 Jul 1996 16:02:12   CFORMUL
// Added more comments to Hare detection
//
//    Rev 1.16   03 Jul 1996 12:19:02   CRENERT
// Changed boot signature verification to absolute bytes.
//
//    Rev 1.15   03 Jul 1996 02:47:00   CFORMUL
// Added MBR detect for Hare.7610.B
//
//    Rev 1.14   25 Jun 1996 17:03:26   JWILBER
// Added EXTScanPartDanish, which looks for Danish Boot, VID 1f8f.
// This routine calls EXTScanDanish, which is also called by
// EXTScanBootDanish.
//
//    Rev 1.13   13 Feb 1996 17:56:22   MKEATIN
// Added DX Support
//
//    Rev 1.12   01 Feb 1996 14:47:50   JWILBER
// Added detection for Predator.2448, VID 1f58 on MBRs.
//
//    Rev 1.11   30 Jan 1996 15:43:38   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
//
//    Rev 1.10   19 Jan 1996 14:51:36   JALLEE
// Rev 1.8
//
//    Rev 1.8   10 Jan 1996 13:18:42   JALLEE
// Changed lpwVID return value for Neuroquila MBR scanning function.
//
//    Rev 1.7   13 Nov 1995 19:30:48   JALLEE
// Cast -1 to WORD, bug fix.
//
//    Rev 1.6   09 Nov 1995 12:07:28   JALLEE
// Reworked below changes to compile under NLM.
// Resolved unused function parameters.
//
//    Rev 1.5   09 Nov 1995 11:18:02   JALLEE
// Added Neuroquila MBR detection
//
//    Rev 1.4   01 Nov 1995 10:49:08   DCHI
// Removed include of navexhdr.h due to change in format of navexshr.h.
//
//    Rev 1.3   19 Oct 1995 14:20:58   DCHI
// Added LOADDS to external functions for WIN16.  Modified DOS export
// table structure to support tiny model modules.
//
//    Rev 1.2   18 Oct 1995 11:50:12   CNACHEN
// Voided parameters to compile correctly for NLM.
//
//    Rev 1.1   16 Oct 1995 13:02:42   DCHI
// Added include of navexhdr.h.
//
//    Rev 1.0   13 Oct 1995 13:06:18   DCHI
// Initial revision.
//
//************************************************************************

#ifdef SYM_WIN16
#pragma code_seg("NAVEX_NAVEXSP","NAVEX_PART_CODE")
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

// prototype for bloodhound boot

#include "booscan.h"

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif


#ifdef NAVEX15
    #ifdef __WATCOMC__
            extern PEXPORT15_TABLE_TYPE     glpstEngineExportTable;
    #endif
#endif


// Prototypes for local MBR scanning functions

// Def Writer: Your prototype here.

WORD EXTScanPartNeuro (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID);

WORD EXTScanPartPredt (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID);

WORD EXTScanPartDanish (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID);

WORD EXTScanPartHare (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID);

WORD EXTScanPartMidsummer (LPCALLBACKREV1 lpCallBack,
                           BYTE           byDrive,
                           LPBYTE         lpbyPartBuffer,
                           LPBYTE         lpbyWorkBuffer,
                           WORD           wVersionNumber,
                           LPWORD         lpwVID);

WORD EXTScanPartGinger (LPCALLBACKREV1 lpCallBack,
                        BYTE           byDrive,
                        LPBYTE         lpbyPartBuffer,
                        LPBYTE         lpbyWorkBuffer,
                        WORD           wVersionNumber,
                        LPWORD         lpwVID);

WORD EXTScanPartMoloch (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID);

WORD EXTScanPartInvisible (LPCALLBACKREV1 lpCallBack,
                           BYTE           byDrive,
                           LPBYTE         lpbyPartBuffer,
                           LPBYTE         lpbyWorkBuffer,
                           WORD           wVersionNumber,
                           LPWORD         lpwVID);

WORD EXTScanPartChan (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID);

WORD EXTScanPartCE   (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID);

WORD EXTScanPartOneHalf (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID);

WORD EXTScanPartLilith (LPCALLBACKREV1 lpCallBack,
                        BYTE           byDrive,
                        LPBYTE         lpbyPartBuffer,
                        LPBYTE         lpbyWorkBuffer,
                        WORD           wVersionNumber,
                        LPWORD         lpwVID);

WORD EXTScanPartBloodhound (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID);


// Jump Table used by EXTScanPart

WORD    (*SPJumpTable[]) (LPCALLBACKREV1 lpCallBack,
              BYTE           byDrive,
              LPBYTE         lpbyPartBuffer,
              LPBYTE         lpbyWorkBuffer,
              WORD           wVersionNumber,
              LPWORD         lpwVID) = {

    // Def Writer: Insert your function name here.


    EXTScanPartNeuro,           // boot detect for neuroquila
    EXTScanPartPredt,           // boot detect for predator
    EXTScanPartDanish,          // boot detect for danish boot
    EXTScanPartHare,            // boot detect for Hare.7610
    EXTScanPartMidsummer,        // boot detect for Midsummer
    EXTScanPartGinger,          // boot detect for Ginger
    EXTScanPartMoloch,          // boot detect for Moloch
    EXTScanPartInvisible,          // boot detect for Invisible
    EXTScanPartChan,            // MBR detect for Chan
    EXTScanPartCE,              // boot detect for CE
    EXTScanPartOneHalf,         // boot detect for OneHalf and Shin variants
    EXTScanPartLilith,          // boot detect for Lilith
    EXTScanPartBloodhound,      // THIS MUST BE LAST!
    NULL
};                              // wID = 0 => end of table.

#define WORK_BUFFER_SIZE 0x800

// EXTScanPart arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_READ_ERROR          if disk error occured
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory alloc. error
//


EXTSTATUS FAR WINAPI NLOADDS EXTScanPart(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         LPBYTE         lpbyPartBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
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

    WORD        cwI=0;
    WORD        wReturn = EXTSTATUS_OK;

    (void)lpCallBack;
    (void)byDrive;
    (void)lpbyPartBuffer;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;
    (void)lpwVID;

#ifndef SYM_WIN32
    lpbyWorkBuffer = (LPBYTE) lpCallBack->PermMemoryAlloc (WORK_BUFFER_SIZE);
#else
    lpbyWorkBuffer = (LPBYTE) GlobalAlloc (GMEM_FIXED, WORK_BUFFER_SIZE);
#endif

    while (SPJumpTable[cwI] != NULL && wReturn != EXTSTATUS_VIRUS_FOUND)
    {
        wReturn = SPJumpTable[ cwI++ ] (lpCallBack,
                                        byDrive,
                                        lpbyPartBuffer,
                                        lpbyWorkBuffer,
                                        wVersionNumber,
                                        lpwVID);
    }

#if defined(SYM_DOS)

    _asm
        {
        mov ds, cs:[wDS]
        mov es, cs:[wES]
        }

#endif




    //////////////////////////////////////////////////////////////////////////
    // Now see if we have an engine virus if we havent' found one so far
    //////////////////////////////////////////////////////////////////////////

#ifdef NAVEX15

//    printf("About to scan part!\n");

    if (wReturn != EXTSTATUS_VIRUS_FOUND)
    {
#ifndef __WATCOMC__
        extern PEXPORT15_TABLE_TYPE     glpstEngineExportTable;
#endif


        if (glpstEngineExportTable != NULL)
        {
            wReturn = glpstEngineExportTable->
                        EXTScanPart(lpCallBack,
                                    byDrive,
                                    lpbyPartBuffer,
                                    lpbyWorkBuffer,
                                    wVersionNumber,
                                    lpwVID);
        }
    }

#endif // #ifdef NAVEX15

    //////////////////////////////////////////////////////////////////////////////
    // Done with engine work
    //////////////////////////////////////////////////////////////////////////////

#ifndef SYM_WIN32
    lpCallBack->PermMemoryFree ((LPVOID) lpbyWorkBuffer);
#else
    GlobalFree ((LPVOID) lpbyWorkBuffer);
#endif

    return ( wReturn );
}


// Local MBR scanning functions
// Def Writer: Insert your function here

// EXTScanPartNeuro     MBR detection for Neuroquila
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanPartNeuro (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID)
{

    BYTE    bSearch[] = {0xcd, 0x13, 0x72, 0xfe, 0xcb};
    WORD    cwI, wReturn = EXTSTATUS_OK;

    (void)lpCallBack;
    (void)byDrive;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;

    if ((WORD)-1 != (cwI = ScanString(lpbyPartBuffer + 25,
                       485,             // sizeof lpbyPartBuffer - 25 - 2
                       bSearch,         // that for which we search
                       sizeof(bSearch))))
    {
        // Look for these bytes in front of the string we found for
        // a double check.

        if (lpbyPartBuffer[25 + cwI - 3] == 0xba &&
            lpbyPartBuffer[25 + cwI - 7] == 0x02 &&
            lpbyPartBuffer[25 + cwI - 10] == 0x93)
        {
            wReturn = EXTSTATUS_VIRUS_FOUND;
            *lpwVID = VID_NEURO_BOOT;
        }
    }
    return (wReturn);
}


// EXTScanPartPredt     MBR detection for Predator
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanPartPredt (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID)
{

    BYTE    bSearch[] = { 0x2e, 0xd3, 0x0d, 0xd1, 0xc1,
                          0x47, 0x47, 0x48, 0x75, 0xf6 };
    WORD    wReturn = EXTSTATUS_OK;

    (void)lpCallBack;
    (void)byDrive;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;

    // _asm    int 3                    // remove when not debugging &&&

    // 0x0a is offset from beginning of MBR to start searching

    if ( 0xffff != ScanString(lpbyPartBuffer + 0x0a,
                       500,             // sizeof lpbyPartBuffer - 0x0a - 2
                       bSearch,         // that for which we search
                       sizeof(bSearch)))
    {
        wReturn = EXTSTATUS_VIRUS_FOUND;
        *lpwVID = VID_PREDT_BOOT;
    }

    return (wReturn);
}

// EXTScanPartDanish    MBR detection for Danish Boot
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanPartDanish (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID)
{
    return(EXTScanDanish(lpCallBack, lpbyPartBuffer, lpwVID));
}

// DANISH.CPP contains EXTScanDanish() and EXTScanOneHalf(), which are also
// called for floppy boot detection.  The prototypes for these are in
// NAVEXSHR.H.

#include "danish.cpp"


// EXTScanPartHare    MBR detection for Hare.7610.A & Hare.7610.B
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanPartHare (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID)
{
    WORD 	wInfect = 0xCCFF;
    WORD 	wReturn = EXTSTATUS_OK;
    WORD    i,j,k;
    BOOL    bVirusIndex = FALSE,
			bVirusKey   = FALSE,
			bDecrypt	= FALSE;

	// Table of Register encodings for encryption

	BYTE	byRegEncode[] = { 0x07, 0x27, 0x0f, 0x2f, 0x17,
                              0x37, 0x04, 0x24, 0x0c, 0x2c,
                              0x14, 0x34, 0x05, 0x25, 0x0d,
                              0x2d, 0x15, 0x3d, 0x1d, 0x3c,
                              0x1c, 0x35 };




    //_asm    int 3


    // This is the viruses own self check.  If the word at offset 0x100
    // into the MBR/BS minus the word at offset 0x102 is equal to 0xccff
    // then it will not infect.  It needs to do this because the virus
    // is polymorphic in the MBR/BS and it cannot easily decrypt and
    // check that way.

    if(wInfect != (WORD) (*((LPWORD) (lpbyPartBuffer + 0x102)) -
                          *((LPWORD) (lpbyPartBuffer + 0x100))))
	{
		return(wReturn);
	}


    for (i = 8; i < 15 ; i++)
    {

        // Check for index initialization.

        if (lpbyPartBuffer[i] == 0xbb ||
            lpbyPartBuffer[i] == 0xbe ||
            lpbyPartBuffer[i] == 0xbf)

		{
			if (*(LPWORD) (lpbyPartBuffer + i + 1) > 0x7c14 &&
				*(LPWORD) (lpbyPartBuffer + i + 1) < 0x7c1c   )
			{
	            bVirusIndex = TRUE;
				break;
			}
        }
    }


	// If we didn't find a mov into an index register then there's
    // no virus here.

	if (!bVirusIndex)
		return (wReturn);


	for (i = 8 ; i < 15 ; i++)
	{
		// Check for key initialization

		if ((lpbyPartBuffer[i] & 0xf8) == 0xb0 )
		{
			bVirusKey = TRUE;
			break;
		}
	}

	
	if (!bVirusKey)
		return (wReturn);



	for (j = i ; i < j + 10; i ++)
    {
		if (lpbyPartBuffer[i] == 0x00 ||
			lpbyPartBuffer[i] == 0x28 ||
			lpbyPartBuffer[i] == 0x30  )
		{
			for (k = 0; k < 22 ; k++)
			{
				if (lpbyPartBuffer[i+1] == byRegEncode[k])
				{
					bDecrypt = TRUE;
					break;
				}
			}
		}

		if (bDecrypt)
			break;
	}


	if (!bDecrypt)
		return (wReturn);


	
	wReturn = EXTSTATUS_VIRUS_FOUND;
	*lpwVID = VID_HARE_BOOT;


    return(wReturn);
}


// EXTScanPartMidsummer
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanPartMidsummer (LPCALLBACKREV1 lpCallBack,
                           BYTE           byDrive,
                           LPBYTE         lpbyPartBuffer,
                           LPBYTE         lpbyWorkBuffer,
                           WORD           wVersionNumber,
                           LPWORD         lpwVID)
{
    WORD    wReturn = EXTSTATUS_OK;
    WORD    wCylinder;
    BYTE    byCount;
    BYTE    bySector;
    BYTE    byHead;
//    BYTE    buf[0x200];
    BYTE    bySearch[] = {0xa1, 0x13, 0x04, 0xa3, 0x70, 0x7d, 0x2d,
                          0x03, 0x00, 0xa3, 0x13, 0x04, 0xb1, 0x06};
 

//    lpbyWorkBuffer = buf;

    // Get the bootable Partition

    byCount = 1;
    byDrive = 0x80;
    byHead = lpbyPartBuffer[0x1bf];
    bySector = lpbyPartBuffer[0x1c0] & 0x3f;
    wCylinder = (((WORD) lpbyPartBuffer[0x1c0] & 0xc0) << 0x02) |
                (WORD) lpbyPartBuffer[0x1c1];

    if (lpCallBack->SDiskReadPhysical ((LPSTR) lpbyWorkBuffer,
                                       byCount,
                                       wCylinder,
                                       bySector,
                                       byHead,
                                       byDrive))
    {
        return (wReturn);
    }


    // Scan for our virus

    if ( 0xffff != ScanString(lpbyWorkBuffer,
                              0x200,
                              bySearch,
                              sizeof(bySearch)))
    {
        wReturn = EXTSTATUS_VIRUS_FOUND;
        *lpwVID = VID_MIDSUMMER;
    }
	

    return(wReturn);
}

// EXTScanPartGinger
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanPartGinger (LPCALLBACKREV1 lpCallBack,
                        BYTE           byDrive,
                        LPBYTE         lpbyPartBuffer,
                        LPBYTE         lpbyWorkBuffer,
                        WORD           wVersionNumber,
                        LPWORD         lpwVID)
{
    WORD    wReturn = EXTSTATUS_OK;
    WORD    wCylinder;
    BYTE    byCount;
    BYTE    bySector;
    BYTE    byHead;
    int     nPartEnt;
//    BYTE    buf[0x200];
    BYTE    bySearch[] = {0xb8, 0xe7, 0xee, 0xcd, 0x21, 0x3d, 0x03,
                          0xd7, 0x74, 0x1c, 0x26, 0x8e, 0x06, 0x2c};
 

//    lpbyWorkBuffer = buf;

    // Get the bootable Partition

    for (nPartEnt = 0; nPartEnt < 4; nPartEnt++)
    {
        if (lpbyPartBuffer[0x1be + (nPartEnt * 0x10)] == 0x80)
        {
            // We found the bootable partition

            byHead = lpbyPartBuffer[0x1bf + (nPartEnt * 0x10)];
            bySector = lpbyPartBuffer[0x1c0 + (nPartEnt * 0x10)] & 0x3f;
            wCylinder = ((lpbyPartBuffer[0x1c0 + (nPartEnt * 0x10)] & 0xc0) << 0x02) |
                          lpbyPartBuffer[0x1c1 + (nPartEnt * 0x10)];
            break;
        }
    }

    if (nPartEnt >= 4)
    {
        // There's no bootable partition

        return (wReturn);
    }



    byCount = 1;
    byDrive = 0x80;
    if (lpCallBack->SDiskReadPhysical ((LPSTR) lpbyWorkBuffer,
                                       byCount,
                                       wCylinder,
                                       bySector,
                                       byHead,
                                       byDrive))
    {
        return (wReturn);
    }


    // Scan for our virus

    if ( 0xffff != ScanString(lpbyWorkBuffer,
                              0x200,
                              bySearch,
                              sizeof(bySearch)))
    {
        wReturn = EXTSTATUS_VIRUS_FOUND;
        *lpwVID = VID_GINGER;
    }
	

    return(wReturn);
}


// EXTScanPartMoloch      MBR detection for Moloch
// arguments:
//
// lpCallBack			: pointer to callback structure for FileOpen, etc.
// byDrive				: drive # containing the boot sector (00=A, 0x80=first
//						  physical HD
// lpbyPartBuffer		: contains image of appropriate part sector
//						  to scan (ignore byDrive)
// lpbyWorkBuffer		: 2-4K buffer for temporary operations.
// wVersionNumber		: version number of NAV calling external code
// lpwVID				: virus ID # of the virus if found
//
// Returns:
//
//	   EXTSTATUS		: EXTSTATUS_OK					if no virus found
//						  EXTSTATUS_VIRUS_FOUND 		if virus was found

WORD EXTScanPartMoloch (LPCALLBACKREV1 lpCallBack,
						BYTE		   byDrive,
						LPBYTE		   lpbyPartBuffer,
						LPBYTE		   lpbyWorkBuffer,
						WORD		   wVersionNumber,
						LPWORD		   lpwVID)
{
    WORD    wReturn = EXTSTATUS_OK;
    WORD    wCyl = 0x00;            // value always 0
    BYTE    byHead = 0x00,          // value always 1
            bySec = 0x03;           // value always 3
    BYTE    bSearch1[] = {0xFA, 0x33, 0xC0, 0x8E, 0xD0, 0xBC, 0x00, 0x7C, 0x16, 0x07};
    BYTE    bSearch2[] = {0xFB, 0xFC, 0x51, 0xB9, 0x6D, 0x04, 0x2E, 0x80, 0x34};
//    BYTE    buf[0x200];


//    lpbyWorkBuffer = buf;

    if (lpbyPartBuffer[0x00] != 0xEB)
        return(wReturn);

    if ( 0xffff != ScanString(lpbyPartBuffer + 0x0a,
                       500,             // sizeof lpbyPartBuffer - 0x0a - 2
                       bSearch1,        // that for which we search
                       sizeof(bSearch1)))
    {

        if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,   // buffer
                                            (BYTE)1,                // count
                                            wCyl,                   // cyl
                                            bySec,                  // sector
                                            byHead,                 // head
                                            byDrive))               // drive
        {
            return (wReturn);
        }

        if ( 0xffff != ScanString(lpbyWorkBuffer + 0x09,
                           501,             // sizeof lpbyPartBuffer - 0x09 - 2
                           bSearch2,        // that for which we search
                           sizeof(bSearch2)))
        {
           wReturn = EXTSTATUS_VIRUS_FOUND;
           *lpwVID = VID_MOLOCH;
        }
    }

return(wReturn);
}


// EXTScanPartInvisible   MBR detection for Invisible 2926 and 3223
// arguments:
//
// lpCallBack			: pointer to callback structure for FileOpen, etc.
// byDrive				: drive # containing the boot sector (00=A, 0x80=first
//						  physical HD
// lpbyPartBuffer		: contains image of appropriate part sector
//						  to scan (ignore byDrive)
// lpbyWorkBuffer		: 2-4K buffer for temporary operations.
// wVersionNumber		: version number of NAV calling external code
// lpwVID				: virus ID # of the virus if found
//
// Returns:
//
//	   EXTSTATUS		: EXTSTATUS_OK					if no virus found
//						  EXTSTATUS_VIRUS_FOUND 		if virus was found

WORD EXTScanPartInvisible (LPCALLBACKREV1 lpCallBack,
                           BYTE           byDrive,
                           LPBYTE         lpbyPartBuffer,
                           LPBYTE         lpbyWorkBuffer,
                           WORD           wVersionNumber,
                           LPWORD         lpwVID)
{
    WORD    wReturn = EXTSTATUS_OK;
    WORD    wOffset, wOffset2;
    WORD    wVID;
    int     i;
    BYTE    bSearch1[] = {0xb9, 0x27, 0x00};
    BYTE    bSearch2[] = {0xb9, 0x2d, 0x00};
    BYTE    bSearch3[] = {0x36, 0x80, 0x74, 0x39};



    if ((wOffset = ScanString(lpbyPartBuffer,
                              0x39,
                              bSearch1,
                              sizeof(bSearch1))) == WERROR)
    {
        if ((wOffset = ScanString(lpbyPartBuffer,
                                  0x39,
                                  bSearch2,
                                  sizeof(bSearch2))) == WERROR)
        {
            return (wReturn);
        }
        else
        {
            wVID = VID_INVIS3223;
        }
    }
    else
    {
        wVID = VID_INVIS2926;
    }

    if ((wOffset2 = ScanString(lpbyPartBuffer + wOffset + 3,
                              0x39 - wOffset - 3,
                              bSearch3,
                              sizeof(bSearch3))) == WERROR)
    {
        return (wReturn);
    }

    for (i = (int) wOffset + (int) wOffset2 + 4; i < 0x39; i++)
    {
        if ((lpbyPartBuffer[i]) == 0x46)
            break;
    }

    if (i == 0x39)
        return (wReturn);

    for (i++; i < 0x39; i++)
    {
        if ((lpbyPartBuffer[i]) == 0xe2)
            break;
    }

    if (i == 0x39)
        return (wReturn);
 
    for (i += 2; i < 0x39; i++)
    {
        if ((lpbyPartBuffer[i]) == 0xeb &&
            (lpbyPartBuffer[i+1]) == 0x39 - i - 2)
        {
            *lpwVID = wVID;
            wReturn = EXTSTATUS_VIRUS_FOUND;
            break;
        }
    }

    return(wReturn);
}


// EXTScanPartChan      MBR detection for Chan
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanPartChan (LPCALLBACKREV1 lpCallBack,
                      BYTE           byDrive,
                      LPBYTE         lpbyPartBuffer,
                      LPBYTE         lpbyWorkBuffer,
                      WORD           wVersionNumber,
                      LPWORD         lpwVID)
{
    WORD    wReturn = EXTSTATUS_OK;
    //The infection looks Like this:
    // 0x16, 0x07, 0xBB, 0x78, 0x06, 0x8D, 0x87, 0x8A, 0xFB, 0xBA
    // HD SD 0xB9 SS CL where HD is Drive Number (0x80 usually)
    //                        SD is Side
    //                        SS is Sector
    //                        CL is Cylinder
    //                  of location of the 2 viral sectors in that HD.
    // 0x16, 0xCD, 0x13, 0x72, 0xE4, 0x53, 0xCB, 0xAF, 0x29
    BYTE    bSearch1[] = {0x16, 0x07, 0xBB, 0x78, 0x06, 0x8D, 0x87, 0x8A, 0xFB, 0xBA};
    BYTE    bSearch2[] = {0x16, 0xCD, 0x13, 0x72, 0xE4, 0x53, 0xCB};


    //Search for the first string at offset 0x08 of the MBR
    if ( 0xffff != ScanString(lpbyPartBuffer + 0x08,
                       0x1f8,
                       bSearch1,
                       sizeof(bSearch1)))
    {
        //Found the first string, skip the Location Info (Skip 6 basically),
        //search for the 2nd one at offset 0x17 of the MBR
        if ( 0xffff != ScanString(lpbyPartBuffer + 0x17,
                           0x1ea,
                           bSearch2,        //
                           sizeof(bSearch2)))
        {
           wReturn = EXTSTATUS_VIRUS_FOUND;
           *lpwVID = VID_CHAN;
        }
    }

return(wReturn);
}


// EXTScanPartCE        MBR detection for CE
//                       the virus doesn't really infect MBR.
//                       It's messing around w/ the Partition Table Structure
//                        so the system boots from a copy of viral BR.
// arguments:
//
// lpCallBack			: pointer to callback structure for FileOpen, etc.
// byDrive				: drive # containing the boot sector (00=A, 0x80=first
//						  physical HD
// lpbyPartBuffer		: contains image of appropriate part sector
//						  to scan (ignore byDrive)
// lpbyWorkBuffer		: 2-4K buffer for temporary operations.
// wVersionNumber		: version number of NAV calling external code
// lpwVID				: virus ID # of the virus if found
//
// Returns:
//
//	   EXTSTATUS		: EXTSTATUS_OK					if no virus found
//						  EXTSTATUS_VIRUS_FOUND 		if virus was found

WORD EXTScanPartCE     (LPCALLBACKREV1 lpCallBack,
						BYTE		   byDrive,
						LPBYTE		   lpbyPartBuffer,
						LPBYTE		   lpbyWorkBuffer,
						WORD		   wVersionNumber,
						LPWORD		   lpwVID)
{
    WORD    wReturn = EXTSTATUS_OK;
    WORD    wbootCyl,
            bypartStruc;
    BYTE    bybootHead,
            bybootSec,
            bypartNum,
            temp;
    BYTE    bSearch[] = {0xbe, 0x23, 0x7d, 0xbf, 0x00, 0x03, 0xb9,
                         0x38, 0x00, 0xf3, 0xa4, 0xbe, 0x4c, 0x00};
//    BYTE    buf[0x200];


//    lpbyWorkBuffer = buf;

    //get the location of boot sector listed in MBR
    for(bypartNum = 0, bypartStruc = 0x1be; bypartNum < 4; bypartNum++, bypartStruc += 0x10)
     {
      if (lpbyPartBuffer[bypartStruc] == 0x80)
        {
         bybootHead = lpbyPartBuffer[bypartStruc + 1];
         bybootSec  = lpbyPartBuffer[bypartStruc + 2];
         temp       = lpbyPartBuffer[bypartStruc + 3];
         wbootCyl   = (WORD) temp;
         break;
        }
     }

    if(bypartNum == 4) // no bootable partition found
      return (wReturn);
    else
      {
      //get the boot sector
      if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,   // buffer
                                          (BYTE)1,                // count
                                          wbootCyl,               // cyl
                                          bybootSec,              // sector
                                          bybootHead,             // head
                                          byDrive))               // drive
        {
          return (wReturn);
        }

      //see if the boot sector pointed by MBR is the virus' boot sector
      if ( 0xffff != ScanString(lpbyWorkBuffer,
                           512,
                           bSearch,        // that for which we search
                           sizeof(bSearch)))
        {
           wReturn = EXTSTATUS_VIRUS_FOUND;
           *lpwVID = VID_CE;
        }
      }

return(wReturn);
}

// EXTScanPartOneHalf   MBR detection for OneHalf and Shin Variants
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//
// EXTScanOneHalf is in DANISH.CPP, because Danish was the first virus to
// share code for boot and MBR detection.

WORD EXTScanPartOneHalf (LPCALLBACKREV1 lpCallBack,
                            BYTE        byDrive,
                            LPBYTE      lpbyPartBuffer,
                            LPBYTE      lpbyWorkBuffer,
                            WORD        wVersionNumber,
                            LPWORD      lpwVID)
{
    return(EXTScanOneHalf(lpCallBack, lpbyPartBuffer, lpwVID));
}   // EXTScanOneHalf is in DANISH.CPP!!!


// EXTScanPartLilith
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanPartLilith (LPCALLBACKREV1 lpCallBack,
                        BYTE           byDrive,
                        LPBYTE         lpbyPartBuffer,
                        LPBYTE         lpbyWorkBuffer,
                        WORD           wVersionNumber,
                        LPWORD         lpwVID)
{
    WORD    wReturn = EXTSTATUS_OK;
    WORD    wCylinder;
    BYTE    byCount;
    BYTE    bySector;
    BYTE    byHead;
//    BYTE    buf[0x200];
    BYTE    bySearch[] = {0xcd, 0x13, 0x5a, 0x80, 0x3e, 0xef, 0x04,
                          0x4C, 0x75, 0x03, 0xe9, 0x82, 0x00, 0xc6,
                          0x06, 0xef, 0x04, 0x4c};
    BYTE    byValidSet[33] = {0x00, 0x02, 0x03, 0x07, 0x0d, 0x13, 0x16, 0x2b, 0x2e,
                              0x33, 0x7c, 0x7e, 0x80, 0x8e, 0x9c, 0xb0, 0xb1,
                              0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9,
                              0xba, 0xbb, 0xbc, 0xcd, 0xc0, 0xd0, 0xfa, 0xff };
    BYTE    by;
    BYTE    byValidSetIndex;
    BYTE    byValidSetStatus;

    //////////////////////////////////////////////////////////////////
    // This virus is polymorphic in the MBR.  It sets up the registers in
    // the MBR and calls INT13 read of sector 0x0D.  The code below
    // checks a length of bytes and verifies the bytes only equal the
    // bytes in the validset of bytes
 
    if ((!(lpbyPartBuffer[0] == 0xEB ||
           lpbyPartBuffer[0] == 0xE9)) ||
        lpbyPartBuffer[1] != 0x4C ||
        lpbyPartBuffer[2] != 0x00)
    {
        return(wReturn);
    }


    for(by=0;by<24;by++)
    {
        byValidSetStatus = 0;
        for (byValidSetIndex=0;byValidSetIndex<33;byValidSetIndex++)
        {
            if ((lpbyPartBuffer[79 + by]) == byValidSet[byValidSetIndex])
            {
                byValidSetStatus = 1;
                break;
            }
        }
        if (byValidSetStatus == 0)
        {
            return(wReturn);
        }
    }

    //////////////////////////////////////////////////////////////////
    // The virus appears to be in the MBR so, we now load the viral
    // sector and check for our scan string

//    lpbyWorkBuffer = buf;
 
    byCount = 1;
    byDrive = 0x80;
    byHead = 0x00;
    bySector = 0x0D;
    wCylinder = 0x00;

    if (lpCallBack->SDiskReadPhysical ((LPSTR) lpbyWorkBuffer,
                                       byCount,
                                       wCylinder,
                                       bySector,
                                       byHead,
                                       byDrive))
    {
        return (wReturn);
    }


    // Scan for our virus

    if ( 0xffff != ScanString(lpbyWorkBuffer,
                              0x200,
                              bySearch,
                              sizeof(bySearch)))
    {
        wReturn = EXTSTATUS_VIRUS_FOUND;
        *lpwVID = VID_LILITH;
    }
	

    return(wReturn);
} // EXTScanPartLilith


// EXTScanPartBloodhound   MBR detection for Bloodhound
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found


WORD EXTScanPartBloodhound (LPCALLBACKREV1 lpCallBack,
                            BYTE        byDrive,
                            LPBYTE      lpbyPartBuffer,
                            LPBYTE      lpbyWorkBuffer,
                            WORD        wVersionNumber,
                            LPWORD      lpwVID)
{
    WORD                                wReturn;
    BOOL                                bExcluded = FALSE;

#if !defined(NAVEX15)
    if (wVersionNumber < 3)
    {
        *lpwVID = 0;
        return(EXTSTATUS_OK);
    }
#endif

    if (BloodhoundFindBootVirus(lpCallBack,lpbyPartBuffer,&bExcluded) == TRUE)
    {
        *lpwVID = VID_BLOODHOUND_PART;
        wReturn = EXTSTATUS_VIRUS_FOUND;
    }
    else
    {
        *lpwVID = 0;
        wReturn = EXTSTATUS_OK;
    }

    // since the bloodhound scanner is always called last in NAVEX just
    // before NAVENG is called, we can fill the work buffer with a request
    // to disable the IBM scanner if we find an excluded boot record...

    if (bExcluded == TRUE)
    {
        lpbyWorkBuffer[0] = 'I' ^ 'B' ^ 'M';
    }
    else
    {
        lpbyWorkBuffer[0] = 0;
    }

    return(wReturn);

}


#if defined(SYM_DOS)

MODULE_EXPORT_TABLE_TYPE module_export_table =
{
  EXPORT_STRING,
  (void (*)())EXTScanPart
};

void main(void)
{
}

#endif

#endif  // #ifndef SYM_NLM

