//************************************************************************
//
// $Header:   S:/NAVEX/VCS/NVXBTSHR.CPv   1.4   06 Jan 1997 19:13:44   RSTANEV  $
//
// Description:
//      Contains NAVEX code shared among boot and partition modules.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/NVXBTSHR.CPv  $
// 
//    Rev 1.4   06 Jan 1997 19:13:44   RSTANEV
// Removed memset and memcpy.
//
//    Rev 1.3   26 Dec 1996 15:22:32   AOONWAL
// No change.
//
//    Rev 1.2   02 Dec 1996 14:00:22   AOONWAL
// No change.
//
//    Rev 1.1   29 Oct 1996 12:59:18   AOONWAL
// No change.
//
//    Rev 1.0   28 Aug 1996 15:54:18   DCHI
// Initial revision.
//
//************************************************************************

#include "platform.h"

#include "endutils.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navexshr.h"

                                        // ----------------
                                        // Boot sector code
                                        // ----------------
BYTE abyJMPAndNOP [] = JMP_AND_NOP;
BYTE abyOEMID [] = OEM_ID;
BYTE abyVolumeLabel [] = VOLUME_LABEL;
BYTE abyNonBootableCode [] = NON_BOOTABLE_CODE;
BYTE abyNonBootableText [] = NON_BOOTABLE_TEXT;

BYTE abyBPBTable [] [ 16 ] = { BPB_360K, BPB_720K, BPB_12M, BPB_144M, BPB_288M,
                               BPB_160K, BPB_180K, BPB_320K, BPB_640K, BPB_12M_2,
                               BPB_1KDMF, BPB_2KDMF };



BOOL NavexGetFloppyType
(
    LPBYTE              lpbySector,     // [in] Boot sector to anaylze
    LPWORD              lpwDiskType     // [out] Disk type
)
{
    BOOL        bResult = FALSE;
    LPFLOPPYBPB lprBPB;
    LPFLOPPYBPB lprTargetBPB;
    BYTE        byBPBIndex;
    BYTE        byMatches;
    BYTE        byMatchedBPB;
    BYTE        byMaxMatches = 0;

    *lpwDiskType = DISK_FLOPPY | DISK_UNKNOWN;
    lprBPB = (LPFLOPPYBPB) ( lpbySector + BYTESPERSEC_OFF );

    for ( byBPBIndex = 0; byBPBIndex < NUM_BPBs; ++byBPBIndex )
        {
        lprTargetBPB = (LPFLOPPYBPB) &abyBPBTable [ byBPBIndex ] [ 0 ];
        byMatches = 0;

        if ( lprTargetBPB->wBytesPerSector == lprBPB->wBytesPerSector )
            ++byMatches;

        if ( lprTargetBPB->bySectorsPerCluster == lprBPB->bySectorsPerCluster )
            ++byMatches;

        if ( lprTargetBPB->wReservedSectors == lprBPB->wReservedSectors )
            ++byMatches;

        if ( lprTargetBPB->byNumberOfFats == lprBPB->byNumberOfFats )
            ++byMatches;

        if ( lprTargetBPB->wMaxRootEntries == lprBPB->wMaxRootEntries )
            ++byMatches;

        if ( lprTargetBPB->wTotalSectors == lprBPB->wTotalSectors )
            ++byMatches;

        if ( lprTargetBPB->byMediaDescriptor == lprBPB->byMediaDescriptor )
            ++byMatches;

        if ( lprTargetBPB->wSectorsPerFat == lprBPB->wSectorsPerFat )
            ++byMatches;

        if ( lprTargetBPB->wSectorsPerTrack == lprBPB->wSectorsPerTrack )
            ++byMatches;

        if ( lprTargetBPB->bySides == lprBPB->bySides )
            ++byMatches;

        if ( byMatches > byMaxMatches )
            {
            byMaxMatches = byMatches;
            byMatchedBPB = byBPBIndex;
            }
        }

    if ( byMaxMatches >= BPB_MATCH_THRESHOLD )
        {
        *lpwDiskType = byMatchedBPB + DISK_FLOPPY + 1;
        bResult = TRUE;
        }

    return ( bResult );
}

BOOL NavexRepairBootGeneric
(
    LPBYTE  lpbyNewBootSector,          // [in/out] The generic boot sector
    WORD    wDiskType                   // [in] Disk type
)
{
    BOOL        bResult = TRUE;
    UINT        nOffset = 0;
    LPBYTE      lpbyBPB;

                                        // ----------------------
                                        // Fill buffer with zeros
                                        // ----------------------
    MEMSET ( lpbyNewBootSector, 0, SECTOR_SIZE );

                                        // --------------------------------
                                        // Put JMP + NOP at start of sector
                                        // --------------------------------
    MEMCPY ( lpbyNewBootSector, abyJMPAndNOP, sJMP_AND_NOP );
    nOffset += sJMP_AND_NOP;

                                        // -----------------------------
                                        // Put OEM ID next (offset 0x03)
                                        // -----------------------------
    MEMCPY ( lpbyNewBootSector + nOffset, abyOEMID, sOEM_ID );
    nOffset += sOEM_ID;

                                        // -------------------
                                        // Get appropriate BPB
                                        // -------------------
    switch ( wDiskType )
        {

        case DISK_FLOPPY_360K:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_360K ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_720K:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_720K ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_12M:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_12M ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_144M:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_144M ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_288M:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_288M ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_160K:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_160K ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_180K:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_180K ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_320K:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_320K ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_640K:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_640K ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_12M_2:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_12M_2 ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_1KDMF:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_1KDMF ] [ 0 ];
            break;
            }

        case DISK_FLOPPY_2KDMF:
            {
            lpbyBPB = &abyBPBTable [ BPB_INDEX_2KDMF ] [ 0 ];
            break;
            }

        default:
            {
            bResult = FALSE;
            break;
            }
        }

    if ( TRUE == bResult )
        {
                                        // --------------------------
                                        // Put BPB next (offset 0x0B)
                                        // --------------------------
        MEMCPY ( lpbyNewBootSector + nOffset, lpbyBPB, sBPB );
        nOffset += sBPB;

                                        // -------------------------------
                                        // Put Volume Label at offset 0x2B
                                        // -------------------------------
        nOffset = 0x2B;
        MEMCPY ( lpbyNewBootSector + nOffset, abyVolumeLabel, sVOLUME_LABEL );

                                        // ------------------------------------
                                        // Put Non-bootable code at offset 0x40
                                        // This is where the latest SYS command
                                        // from Win95 puts it to account for
                                        // extra fields in BPB.
                                        //
                                        // *** WARNING ***
                                        //
                                        // This offset must match the JMP
                                        // instruction used above.  Also, if
                                        // the code grows larger than 0x40
                                        // bytes, the non-bootable text below
                                        // must be relocated and the reference
                                        // to it in this code must be changed.
                                        // ------------------------------------
        nOffset = 0x40;
        MEMCPY ( lpbyNewBootSector + nOffset, abyNonBootableCode, sNON_BOOTABLE_CODE );

                                        // ------------------------------------
                                        // Put non-bootable text at offset 0x80
                                        //
                                        // *** WARNING ***
                                        //
                                        // This offset must match the location
                                        // used by the non-bootable code above.
                                        // ------------------------------------
        nOffset = 0x80;
        MEMCPY ( lpbyNewBootSector + nOffset, abyNonBootableText, sNON_BOOTABLE_TEXT );

                                        // ---------------------------
                                        // Put 0xAA55 signature at end
                                        // ---------------------------
        lpbyNewBootSector [ SECTOR_SIZE - 2 ] = 0x55;
        lpbyNewBootSector [ SECTOR_SIZE - 1 ] = 0xAA;

        }

    return ( bResult );

}
