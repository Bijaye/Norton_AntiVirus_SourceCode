//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexsb.cpv   1.27   26 Dec 1996 15:22:20   AOONWAL  $
//
// Description:
//      Contains NAVEX EXTScanBoot code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexsb.cpv  $
// 
//    Rev 1.27   26 Dec 1996 15:22:20   AOONWAL
// No change.
// 
//    Rev 1.26   02 Dec 1996 14:00:26   AOONWAL
// No change.
// 
//    Rev 1.25   29 Oct 1996 12:59:22   AOONWAL
// No change.
// 
//    Rev 1.24   16 Aug 1996 18:22:52   CFORMUL
// Fixed bug in Hare code under 32-bit.
// 
// 
//    Rev 1.23   29 Jul 1996 12:26:42   SCOURSE
// Removed int 3
// 
//    Rev 1.22   28 Jul 1996 17:53:40   SCOURSE
// Mod to Moloch detection -
// 
//    Rev 1.21   24 Jul 1996 18:29:18   CFORMUL
// Reworked hare Boot detection so it is sure to detect all
// variants in the floppy boot.
// 
// 
//    Rev 1.20   24 Jul 1996 18:21:18   SCOURSE
// Finalized (pre-QA) Moloch
// 
//    Rev 1.19   11 Jul 1996 16:47:52   CFORMUL
// Removed reference to old Hare define
// 
// 
//    Rev 1.18   08 Jul 1996 19:01:08   CRENERT
// Added #if define for Hare
// 
//    Rev 1.17   08 Jul 1996 17:38:20   CFORMUL
// Added Detection for Hare.7610.A
// 
// 
//    Rev 1.16   08 Jul 1996 16:02:56   CFORMUL
// Fixed up floppy detect for Hare
// 
// 
//    Rev 1.15   03 Jul 1996 02:47:44   CFORMUL
// Added Floppy detect for Hare.7610.B
// 
// 
//    Rev 1.14   25 Jun 1996 17:04:50   JWILBER
// Added EXTScanBootDanish, which looks for Danish Boot, VID 1f8f.
// This routine calls EXTScanDanish, which is also called by
// EXTScanPartDanish.
//
//    Rev 1.13   05 Mar 1996 12:19:12   JSULTON
// Removed comments Predator in jump table.
//
//    Rev 1.12   01 Mar 1996 19:42:50   JWILBER
// Added EXTScanBootPredt, so we can pick up Predator on floppy disks,
// too, and not just MBRs.  It is commented out of the jump table.
//
//    Rev 1.11   13 Feb 1996 17:56:02   MKEATIN
// Added DX Support
//
//    Rev 1.10   30 Jan 1996 15:43:34   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
//
//    Rev 1.9   19 Jan 1996 14:50:36   JALLEE
// Rev 1.7
//
//    Rev 1.7   10 Jan 1996 13:20:24   JALLEE
// Changed return value of Neuroquila boot scanning function.
//
//    Rev 1.6   13 Nov 1995 19:26:42   JALLEE
// Bug Fix Cast -1 to WORD
//
//    Rev 1.5   09 Nov 1995 13:32:38   JALLEE
// Added boot detection for Neuroquila.
//
//    Rev 1.4   01 Nov 1995 10:49:06   DCHI
// Removed include of navexhdr.h due to change in format of navexshr.h.
//
//    Rev 1.3   19 Oct 1995 14:20:56   DCHI
// Added LOADDS to external functions for WIN16.  Modified DOS export
// table structure to support tiny model modules.
//
//
//    Rev 1.2   18 Oct 1995 11:49:06   CNACHEN
// Voided unused parameters to build without errors under NLM.
//
//    Rev 1.1   16 Oct 1995 13:02:10   DCHI
// Added include of navexhdr.h.
//
//    Rev 1.0   13 Oct 1995 13:05:44   DCHI
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

// Prototypes for local Boot scanning functions

// Def Writer: Your prototype here.

WORD EXTScanBootNeuro (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootPredt (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootDanish (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootHare (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootMoloch (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);


// Jump Table used by EXTScanBoot

WORD    (*SBJumpTable[]) (LPCALLBACKREV1 lpCallBack,
                          BYTE           byDrive,
                          BYTE           byPartitionNum,
                          WORD           wSector,
                          WORD           wCylinder,
                          WORD           wHead,
                          LPBYTE         lpbyBootBuffer,
                          LPBYTE         lpbyWorkBuffer,
                          WORD           wVersionNumber,
                          LPWORD         lpwVID) = {

    // Def Writer: Insert your function name here.

    EXTScanBootNeuro,           // boot detect for neuroquila
    EXTScanBootPredt,           // boot detect for predator
    EXTScanBootDanish,          // boot detect for danish boot
    EXTScanBootMoloch,          // boot detect for Moloch
    EXTScanBootHare,            // boot detect for Hare
    NULL
};                              // wID = 0 => end of table.

// EXTScanBoot arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_READ_ERROR          if disk error occured
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory alloc. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTScanBoot(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
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
    (void)byPartitionNum;
    (void)wSector;
    (void)wCylinder;
    (void)wHead;
    (void)lpbyBootBuffer;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;
    (void)lpwVID;

    while (SBJumpTable[cwI] != NULL && wReturn != EXTSTATUS_VIRUS_FOUND)
    {
        wReturn = SBJumpTable[ cwI++ ] ( lpCallBack,
                                         byDrive,
                                         byPartitionNum,
                                         wSector,
                                         wCylinder,
                                         wHead,
                                         lpbyBootBuffer,
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


    return ( wReturn );
}

// *************************************
// Local Boot scanning functions
// Def Writer: Insert your function here
// *************************************

// EXTScanBootNeuro     boot detection for Neuroquila
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot sectors
//                        on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanBootNeuro (LPCALLBACKREV1 lpCallBack,
                       BYTE           byDrive,
                       BYTE           byPartitionNum,
                       WORD           wSector,
                       WORD           wCylinder,
                       WORD           wHead,
                       LPBYTE         lpbyBootBuffer,
                       LPBYTE         lpbyWorkBuffer,
                       WORD           wVersionNumber,
                       LPWORD         lpwVID)

{

    BYTE    bSearch[] = {0xcd, 0x13, 0x72, 0xfe, 0xcb};
    WORD    cwI,wReturn = EXTSTATUS_OK;

    (void)lpCallBack;
    (void)byDrive;
    (void)byPartitionNum;
    (void)wSector;
    (void)wCylinder;
    (void)wHead;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;


    if ((WORD)-1 != (cwI = ScanString(lpbyBootBuffer + 25,
                       485,             // sizeof lpbyBootBuffer - 25 - 2
                       bSearch,         // that for which we search
                       sizeof(bSearch))))
    {
        // Look for these bytes in front of the string we found for
        // a double check.

        if (lpbyBootBuffer[25 + cwI - 3] == 0xba &&
            lpbyBootBuffer[25 + cwI - 7] == 0x02 &&
            lpbyBootBuffer[25 + cwI - 10] == 0x93)
        {
            wReturn = EXTSTATUS_VIRUS_FOUND;
            *lpwVID = VID_NEURO_BOOT;
        }
    }
    return (wReturn);
}

// EXTScanBootPredt     boot detection for Predator
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanBootPredt (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{

    BYTE    bSearch[] = { 0x2e, 0xd3, 0x0d, 0xd1, 0xc1,
                          0x47, 0x47, 0x48, 0x75, 0xf6 };
    WORD    wReturn = EXTSTATUS_OK;

    (void)lpCallBack;
    (void)byDrive;
    (void)byPartitionNum;
    (void)wSector;
    (void)wCylinder;
    (void)wHead;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;
    (void)lpwVID;

    // _asm    int 3                    // remove when not debugging &&&

    // 0x0a is offset from beginning of MBR to start searching
 
    if ( 0xffff != ScanString(lpbyBootBuffer + 0x0a,
                       500,             // sizeof lpbyPartBuffer - 0x0a - 2
                       bSearch,         // that for which we search
                       sizeof(bSearch)))
    {
        wReturn = EXTSTATUS_VIRUS_FOUND;
        *lpwVID = VID_PREDT_BOOT;
    }
 
    return (wReturn);
}   // End of EXTScanBootPredt

// EXTScanBootDanish        boot detection for Danish Boot
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanBootDanish (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{
    return(EXTScanDanish(lpCallBack, lpbyBootBuffer, lpwVID));
}   // End of EXTScanBootPredt


// EXTScanBootMoloch
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//

WORD EXTScanBootMoloch (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)


{
    WORD    wReturn = EXTSTATUS_OK;
    BYTE    i;
    BYTE    byEP;                   // offset to boot-program EP
    WORD    wCyl = 0x4F;            // should be a static value
    BYTE    byHead = 0x01,          // value always 1
            bySec = 0x00;           // value variable based on diskette type
    BYTE    buf[0x200];
    BYTE    bSearch1[] = {0x4F};
    BYTE    bSearch2[] = {0xFB, 0xFC, 0x51, 0xB9, 0x6D, 0x04, 0x2E, 0x80, 0x34};

    lpbyWorkBuffer = buf;

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if (lpbyBootBuffer[0x00] != 0xEB)
        return(wReturn);

    byEP = lpbyBootBuffer[0x01];

    //_asm        int 3

    for (i = 0; i < 30; i++)
        if (lpbyBootBuffer[0x00 + byEP + i] == 0x4F)
        {
            bySec = lpbyBootBuffer[0x18] - 3;   // assign bySec value based on BPB info
                                                // necessary to detect virus on 720K
                                                // or 1.4 Mb diskettes

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
                               bSearch2,         // that for which we search
                               sizeof(bSearch2)))
            {
               wReturn = EXTSTATUS_VIRUS_FOUND;
               *lpwVID = VID_MOLOCH;
            }
        }

return(wReturn);
}



// EXTScanBootHare    MBR detection for Hare.7610 Hare.7750 Hare.7786
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//

WORD EXTScanBootHare (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)


{
    WORD 	wInfect = 0xCCFF;
    WORD 	wReturn = EXTSTATUS_OK;
    WORD    i,j,k;
	BYTE	byVOffset;
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

    if(wInfect != (WORD) (*((LPWORD) (lpbyBootBuffer + 0x102)) - 
                          *((LPWORD) (lpbyBootBuffer + 0x100))))
	{
		return(wReturn);
	}


	// Get the offset of the starting jump to the decryptor

	byVOffset = lpbyBootBuffer[1] + 2;


    for (i = byVOffset + 8; i < byVOffset + 15 ; i++)
    {

        // Check for index initialization.

        if (lpbyBootBuffer[i] == 0xbb ||
            lpbyBootBuffer[i] == 0xbe ||
            lpbyBootBuffer[i] == 0xbf)

		{
			if (*(LPWORD) (lpbyBootBuffer + i + 1) > (0x7c14 + byVOffset) &&
				*(LPWORD) (lpbyBootBuffer + i + 1) < (0x7c1c + byVOffset) )
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


	for (i = byVOffset + 8 ; i < byVOffset + 15 ; i++)
	{
		// Check for key initialization

		if ((lpbyBootBuffer[i] & 0xf8) == 0xb0 )
		{
			bVirusKey = TRUE;
			break;
		}
	}

	
	if (!bVirusKey)
		return (wReturn);



	for (j = i ; i < j + 10; i ++)
    {
		if (lpbyBootBuffer[i] == 0x00 ||
			lpbyBootBuffer[i] == 0x28 ||
			lpbyBootBuffer[i] == 0x30  )
		{
			for (k = 0; k < 22 ; k++)
			{
				if (lpbyBootBuffer[i+1] == byRegEncode[k])
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


// DANISH.CPP contains EXTScanDanish(), which is also called for MBR
// detection.  The prototype for this is in NAVEXSHR.H.

#if defined(SYM_DOS)    // DANISH.CPP is also included in NAVEXSP.CPP and
                        // only needs to be included again in DOS because
#include "danish.cpp"   // partition and boot scanning are in separate
                        // files in DOS NAVEX.




MODULE_EXPORT_TABLE_TYPE module_export_table =
{
  EXPORT_STRING,
  (void (*)())EXTScanBoot
};

void main(void)
{
}

#endif

#endif  // #ifndef SYM_NLM

