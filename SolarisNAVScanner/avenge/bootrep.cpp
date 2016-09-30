// Copyright 1996 Symantec Corporation
// ----------------------------------------------------------------------------
//
// $Header:   S:/AVENGE/VCS/BOOTREP.CPv   1.8   27 Feb 1998 13:06:22   MKEATIN  $
//
// Description:
//  This file contains all Boot and MBR Repair routines
//
// Contains:
//  EngRepairBootAux()
//  EngRepairBootPhysical()
//  EngRepairBootSavePhysical()
//  EngRepairBootSaveLogical()
//  EngRepairBootUnique()
//  EngRepairBootEOFD()
//  EngRepairGeneric()
//  EngRepairMBRGeneric()
//  EngRepairBootGeneric()
//  EngGetFloppyType()
//  EngAnalyzeFloppy()
//  EngParseMBRPartition()
//  EngReplaceMBRPartition()
//  EngDecodeSectorCylinder()
//  EngEncodeSectorCylinder()
//  EngValidateMBR()
//  EngValidateMBRPartition()
//  EngValidateBootSector()
//  EngUpdateMBR()
//  EngReadSectorLinear()
//  EngWriteSectorLinear()
//  EngLinearizeSector()
//
// See Also:
//
// ----------------------------------------------------------------------------
// $Log:   S:/AVENGE/VCS/BOOTREP.CPv  $
// 
//    Rev 1.8   27 Feb 1998 13:06:22   MKEATIN
// Fixed a build error for DEC ALPHA.
// 
//    Rev 1.7   07 Aug 1997 12:29:28   Ray
// Some stealthing MBR viruses screw up NT's ability to get drive dimensions.
// Fixed EngValidateMBRPartition to use dwRelativeSectors field in MBR
// partition enrty to locate and validate each Boot Sector.
// 
//    Rev 1.6   08 Jul 1997 17:49:24   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.5   25 Jun 1997 18:27:26   Ray
// Fixed generic boot repair in cases where we cannot determine the
// floppy type.
// 
//    Rev 1.4   03 Jun 1997 13:49:46   MKEATIN
// Initialized eStatus in EngValidateMBRPartition().
// 
//    Rev 1.3   13 May 1997 18:27:04   MKEATIN
// Ported Carey's boot caching code.
// 
//    Rev 1.2   07 May 1997 14:03:08   Ray
// Fixed bug - initialzing lprBPB in EngAnalyzeFloppy().
// 
//    Rev 1.12   19 Jul 1996 18:21:26   RAY
// Added support for 1k and 2k cluster DMF generic boot repair.
// 
//    Rev 1.11   17 Jun 1996 15:08:56   RAY
// Various bug fixes to Boot Repair stuff.
// 
//    Rev 1.10   12 Jun 1996 20:27:42   RAY
// added code to zero out sector buffers before returning to avoid virus
// shadows in memory
// 
//    Rev 1.9   10 Jun 1996 10:46:26   RAY
// Added linear open and close
// 
//    Rev 1.8   05 Jun 1996 14:38:54   CNACHEN
// Fixed code that was passing in an & to a single byte byDecryptSig instead of a
// byDecryptSig array of length 8
// 
//    Rev 1.7   04 Jun 1996 17:41:08   DCHI
// Changes to conform with new header files in global include.
// 
//    Rev 1.6   04 Jun 1996 16:36:06   RAY
// Fixed data file logic in EngRepairBootAux.
// 
//    Rev 1.5   04 Jun 1996 13:44:58   RAY
// Reorganized most of BootRep and added support for SECTORs.
// 
//    Rev 1.4   31 May 1996 16:29:24   RAY
// Removed read request for generic repairs
// 
//    Rev 1.3   21 May 1996 17:09:52   RAY
// Fixed prototype and call to EngRepairBootEOFD
// 
//    Rev 1.2   21 May 1996 16:53:36   RAY
// Changed from Physical to Linear Objects and Callbacks.
// 
//    Rev 1.1   17 May 1996 14:11:16   RAY
// added lpbRepaired handling to EngRepairBootAux
// 
//    Rev 1.0   16 May 1996 18:27:36   RAY
// Initial revision.
// 
// ----------------------------------------------------------------------------

#include "avengel.h"
#include "bootrep.h"
                                        // --------
                                        // MBR Code
                                        // --------
BYTE abyMBRCode [] = MBR_CODE;

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


ENGSTATUS EngRepairBootAux
(
    HLENG            hLocalEng,         // [in] Local boot engine context
    LPVIRUS_INFO     lprVirusInfo,      // [in] VID or equivalent to repair
    LPLINEAROBJECT   lprLinear,         // [in] Linear device to repair
    BYTE             byPartNumber,      // [in] HD partition for HARD_BOOT types
    LPBOOL           lpbRepaired        // [out] Repaired?
)
{
    ENGSTATUS           esResult = ENGSTATUS_OK;
    WORD                wDiskType;
    BYTE                byRepairType;
    SECTOR              rInfection;
    BOOTREPAIRDEF       rBootRepairDef;
    BYTE                abyDecryptSig[DECRYPT_MEM_SIG_SIZE];
    LPVOID              lpvDatInfo;
    BOOL                bCloseDataFile = FALSE;
    BOOL                bCloseLinear = FALSE;
    BOOL                bIsPartitionedMedia;

    *lpbRepaired = FALSE;
    rInfection.dwLinearLow = rInfection.dwLinearHigh = 0;

    if ( ENGSTATUS_OK == esResult )
        {
        esResult = lprLinear->lpCallBacks->LinearOpen ( lprLinear->lpvInfo,
                                                        LINEAR_OPEN_READ_WRITE );
        }

                                        // ---------------------------------
                                        // load the repair definition and
                                        // read HD MBR or floppy boot sector
                                        // ---------------------------------
    if ( ENGSTATUS_OK == esResult )
        {
        bCloseLinear = TRUE;
        esResult = GetRepairRecord (hLocalEng,
                                    lprVirusInfo,
                                    (LPVIRUS_REPAIR)&rBootRepairDef,
                                    abyDecryptSig );
        }

                                        // ---------------------------------
                                        // Read HD MBR or Floppy Boot Sector
                                        // ---------------------------------
    if ( ENGSTATUS_OK == esResult )
        {
        esResult = EngReadSectorLinear ( lprLinear, &rInfection );
        }

    if ( ENGSTATUS_OK == esResult )
        {
        if ( CBSTATUS_OK != lprLinear->lpCallBacks->
                LinearIsPartitionedMedia ( lprLinear->lpvInfo,
                                           &bIsPartitionedMedia ))
            {
            // Failure determining whether it was a partitioned media

            esResult = ENGSTATUS_ERROR;
            }
        }

    if (( ENGSTATUS_OK == esResult ) && bIsPartitionedMedia )
        {
                                        // ----------------------
                                        // if this is a hard disk
                                        // ----------------------
        wDiskType = DISK_FIXED;
        byRepairType = LOBYTE ( rBootRepairDef.wHDRepairType );


                                        // --------------------------------
                                        // if infection type is boot sector
                                        // --------------------------------
        if ( byRepairType >= REP_BOOT )
            {
            PARTENTRY  rPartEntry;

                                        // ---------------------------------
                                        // calculate HD boot sector location
                                        // for byPartNumber
                                        // ---------------------------------
            EngParseMBRPartition ( rInfection.abyBuffer,
                                   byPartNumber,
                                   &rPartEntry );

            rInfection.bySide = rPartEntry.byStartSide;
            rInfection.wCylinder = rPartEntry.wStartCylinder;
            rInfection.bySector = rPartEntry.byStartSector;

                                        // ------------------------------------
                                        // read HD boot sector for byPartNumber
                                        // ------------------------------------
            if ( ENGSTATUS_OK != EngLinearizeSector ( lprLinear, &rInfection ) ||
                 ENGSTATUS_OK != EngReadSectorLinear ( lprLinear, &rInfection ))
                {
                esResult = ENGSTATUS_ERROR;
                }
            }
        }

    else if ( ENGSTATUS_OK == esResult )
        {

                                        // ----------------------
                                        // Set floppy repair type
                                        // ----------------------
        byRepairType = LOBYTE ( rBootRepairDef.wFloppyRepairType );

                                        // ------------------------
                                        // analyze floppy disk type
                                        // ------------------------
        esResult = EngGetFloppyType ( lprLinear,
                                      rInfection.abyBuffer,
                                      &wDiskType );
        }

    if(( ENGSTATUS_OK != esResult ) &&
       ( ( DISK_UNKNOWN | DISK_FLOPPY ) == wDiskType ))
        {
        // Couldn't determine the floppy type meaning
        // specific repairs will fail so take a shortcut
        // to generic repair.

        esResult = EngRepairGeneric ( lprLinear,
                                      &rBootRepairDef,
                                      wDiskType,
                                      byRepairType,
                                      &rInfection );
        }

    else if ( ENGSTATUS_OK == esResult )
        {
        switch ( byRepairType )
            {
            case REP_MBR_PHYSICAL:
            case REP_BOOT_PHYSICAL:
                {
                esResult = EngRepairBootPhysical ( lprLinear,
                                                   &rBootRepairDef,
                                                   wDiskType,
                                                   byRepairType,
                                                   &rInfection );
                break;
                }

            case REP_MBR_SAVE_PHYSICAL:
            case REP_BOOT_SAVE_PHYSICAL:
                {
                esResult = EngRepairBootSavePhysical ( lprLinear,
                                                       &rBootRepairDef,
                                                       wDiskType,
                                                       byRepairType,
                                                       &rInfection );

                break;
                }

            case REP_MBR_EOFD:
                {
                esResult = EngRepairBootEOFD ( lprLinear,
                                               &rBootRepairDef,
                                               wDiskType,
                                               byRepairType,
                                               &rInfection );
                break;
                }

            case REP_MBR_UNIQUE:
            case REP_BOOT_UNIQUE:
            case REP_EXEC_CODE:
                {
                                        // -------------------
                                        // Replaced with NAVEX
                                        // -------------------
                esResult = ENGSTATUS_ERROR;
                break;
                }

            case REP_BOOT_SAVE_LOGICAL:
                {
                esResult = EngRepairBootSaveLogical ( lprLinear,
                                                      &rBootRepairDef,
                                                      wDiskType,
                                                      byPartNumber,
                                                      byRepairType,
                                                      &rInfection );
                break;
                }

            case REP_MBR_GENERIC:
            case REP_BOOT_GENERIC:
                {
                esResult = EngRepairGeneric ( lprLinear,
                                              &rBootRepairDef,
                                              wDiskType,
                                              byRepairType,
                                              &rInfection );
                break;
                }
            }

        }

    if ( ENGSTATUS_OK == esResult )
        {
        *lpbRepaired = TRUE;
        }

    if ( bCloseDataFile &&
         DFSTATUS_OK != EngCloseDataFile ( hLocalEng->hGEng->lpDataFileCallBacks,
                                          lpvDatInfo ))
        {
        esResult = ENGSTATUS_ERROR;
        }

    if ( bCloseLinear &&
         ENGSTATUS_OK != lprLinear->lpCallBacks->LinearClose ( lprLinear->lpvInfo ))
        {
        esResult = ENGSTATUS_ERROR;
        }

                                        // Avoid leaving shadow in memory
    MEMSET ( rInfection.abyBuffer, 0, SECTOR_SIZE );

    return ( esResult );
}


ENGSTATUS EngRepairBootPhysical
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPBOOTREPAIRDEF lprBootRepairDef,   // [in] Boot view of Repair Def
    WORD            wDiskType,          // [in] Disk type
    BYTE            byRepairType,       // [in] Repair type
    LPSECTOR        lprInfection        // [in] Infected sector
)
{
    ENGSTATUS esResult = ENGSTATUS_OK;
    WORD      wCylinderSector;
    SECTOR    rRepair;

    if ( DISK_FIXED == wDiskType )
        {
        rRepair.wCylinder = lprBootRepairDef->wHDLocation1;
        rRepair.bySide = HIBYTE ( lprBootRepairDef->wHDLocation2 );
        rRepair.bySector = LOBYTE ( lprBootRepairDef->wHDLocation2 );
        }

    else
        {
        switch ( wDiskType )
            {
            case DISK_FLOPPY_360K:
                {
                wCylinderSector = lprBootRepairDef->wFloppy360;
                break;
                }

            case DISK_FLOPPY_720K:
                {
                wCylinderSector = lprBootRepairDef->wFloppy720;
                break;
                }

            case DISK_FLOPPY_12M:
                {
                wCylinderSector = lprBootRepairDef->wFloppy12;
                break;
                }

            case DISK_FLOPPY_144M:
                {
                wCylinderSector = lprBootRepairDef->wFloppy144;
                break;
                }

            case DISK_FLOPPY_288M:
                {
                wCylinderSector = lprBootRepairDef->wFloppy288;
                break;
                }

            default:
                {
                esResult = ENGSTATUS_ERROR;
                break;
                }
            }

        if ( ENGSTATUS_OK == esResult )
            {
            rRepair.bySide = HIBYTE ( lprBootRepairDef->wFloppyAll );
            rRepair.wCylinder = (WORD) HIBYTE ( wCylinderSector );
            rRepair.bySector = LOBYTE ( wCylinderSector );
            }
        }

    if ( ENGSTATUS_OK != esResult ||
         ENGSTATUS_OK != EngLinearizeSector ( lprLinear, &rRepair ) ||
         ENGSTATUS_OK != EngReadSectorLinear ( lprLinear, &rRepair ))
        {
        esResult = ENGSTATUS_ERROR;
        }

    else
        {
        if ( byRepairType < REP_BOOT )
            {
                                        // -------------------------
                                        // Update MBR before writing
                                        // -------------------------
            if ( ENGSTATUS_OK == EngValidateMBR ( lprLinear,
                                                  lprInfection->abyBuffer ))
                {
                EngUpdateMBR ( lprLinear,
                               lprInfection->abyBuffer,
                               rRepair.abyBuffer );
                }

            esResult = EngValidateMBR ( lprLinear, rRepair.abyBuffer );
            }
        else
            {
            esResult = EngValidateBootSector ( lprLinear, rRepair.abyBuffer );
            }

        }

    if ( ENGSTATUS_OK == esResult )
        {
        rRepair.dwLinearLow = lprInfection->dwLinearLow;
        rRepair.dwLinearHigh = lprInfection->dwLinearHigh;
        esResult = EngWriteSectorLinear ( lprLinear, &rRepair );
        }

    if ( ENGSTATUS_OK != esResult )
        {
        esResult = EngRepairGeneric ( lprLinear,
                                      lprBootRepairDef,
                                      wDiskType,
                                      byRepairType,
                                      lprInfection );
        }

    return ( esResult );
}


ENGSTATUS EngRepairBootSavePhysical
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPBOOTREPAIRDEF lprBootRepairDef,   // [in] Boot view of Repair Def
    WORD            wDiskType,          // [in] Disk type
    BYTE            byRepairType,       // [in] Repair type
    LPSECTOR        lprInfection        // [in] Infected Sector
)
{
    ENGSTATUS   esResult = ENGSTATUS_OK;
    WORD        wCylinderSector;
    WORD        wCylinderOffset;
    WORD        wSideOffset;
    WORD        wSectorOffset;
    BYTE        byAdd;
    SECTOR      rRepair;

    if ( DISK_FIXED == wDiskType )
        {
        wCylinderOffset = HIBYTE ( lprBootRepairDef->wHDLocation1 );
        wSectorOffset = LOBYTE ( lprBootRepairDef->wHDLocation1 );
        wSideOffset = HIBYTE ( lprBootRepairDef->wHDLocation2 );
        byAdd = LOBYTE ( lprBootRepairDef->wHDLocation2 );
        }

    else
        {
        switch ( wDiskType )
            {
            case DISK_FLOPPY_360K:
                {
                wCylinderSector = lprBootRepairDef->wFloppy360;
                break;
                }

            case DISK_FLOPPY_720K:
                {
                wCylinderSector = lprBootRepairDef->wFloppy720;
                break;
                }

            case DISK_FLOPPY_12M:
                {
                wCylinderSector = lprBootRepairDef->wFloppy12;
                break;
                }

            case DISK_FLOPPY_144M:
                {
                wCylinderSector = lprBootRepairDef->wFloppy144;
                break;
                }
            case DISK_FLOPPY_288M:
                {
                wCylinderSector = lprBootRepairDef->wFloppy288;
                break;
                }
            default:
                {
                esResult = ENGSTATUS_ERROR;
                break;
                }
            }

        if ( ENGSTATUS_OK == esResult )
            {
            wCylinderOffset = HIBYTE ( wCylinderSector );
            wSectorOffset = LOBYTE ( wCylinderSector );
            wSideOffset = HIBYTE ( lprBootRepairDef->wFloppyAll );
            byAdd = LOBYTE ( lprBootRepairDef->wFloppyAll );
            }
        }

    if ( ENGSTATUS_OK == esResult )
        {
        if ( lprBootRepairDef->wRestoreFlags & BIG_SECTOR_OFFSET )
            {
            wSectorOffset += 0x0100;
            }

        if ( lprBootRepairDef->wRestoreFlags & BIG_SIDE_OFFSET )
            {
            wSideOffset += 0x0100;
            }

        if ( lprBootRepairDef->wRestoreFlags & BIG_CYLINDER_OFFSET )
            {
            wCylinderOffset += 0x0100;
            }

        if ( lprBootRepairDef->wSpecialFlags & WORD_CYLINDER_OFFSET )
            {
                                        // ---------------------------
                                        // Read cylinder value as WORD
                                        // ---------------------------
            rRepair.wCylinder = lprInfection->abyBuffer [ wCylinderOffset ] +
                                ((WORD) lprInfection->abyBuffer [ wCylinderOffset + 1 ] << 8 );
            }
        else
            {
                                        // ---------------------------
                                        // Read cylinder value as BYTE
                                        // ---------------------------
            rRepair.wCylinder = lprInfection->abyBuffer [ wCylinderOffset ];
            }

        rRepair.bySide = lprInfection->abyBuffer [ wSideOffset ];
        rRepair.bySector = lprInfection->abyBuffer [ wSectorOffset ] + byAdd;

        if (( rRepair.bySector & 0xC0 ) != 0 )
            {
            rRepair.wCylinder |= ((WORD) (rRepair.bySector & 0xC0) << 2);
            rRepair.bySector &= ~(rRepair.bySector & 0xC0);
            }

        if ( ENGSTATUS_OK != EngLinearizeSector ( lprLinear, &rRepair ) ||
             ENGSTATUS_OK != EngReadSectorLinear ( lprLinear, &rRepair ))
            {
            esResult = ENGSTATUS_ERROR;
            }
        }

    if ( ENGSTATUS_OK == esResult )
        {
        if ( byRepairType < REP_BOOT )
            {
                                        // -------------------------
                                        // Update MBR before writing
                                        // -------------------------
            if ( ENGSTATUS_OK == EngValidateMBR ( lprLinear,
                                                  lprInfection->abyBuffer ))
                {
                EngUpdateMBR ( lprLinear,
                               lprInfection->abyBuffer,
                               rRepair.abyBuffer );

                }

            esResult = EngValidateMBR ( lprLinear, rRepair.abyBuffer );
            }
        else
            {
            esResult = EngValidateBootSector ( lprLinear, rRepair.abyBuffer );
            }
        }

    if ( ENGSTATUS_OK == esResult )
        {
        rRepair.dwLinearLow = lprInfection->dwLinearLow;
        rRepair.dwLinearHigh = lprInfection->dwLinearHigh;
        esResult = EngWriteSectorLinear ( lprLinear, &rRepair );
        }

    if ( ENGSTATUS_OK != esResult )
        {
        esResult = EngRepairGeneric ( lprLinear,
                                      lprBootRepairDef,
                                      wDiskType,
                                      byRepairType,
                                      lprInfection );
        }

    return ( esResult );
}


ENGSTATUS EngRepairBootSaveLogical
(
    LPLINEAROBJECT      lprLinear,
    LPBOOTREPAIRDEF     lprBootRepairDef,
    WORD                wDiskType,
    BYTE                byPartNumber,
    BYTE                byRepairType,
    LPSECTOR            lprInfection
)
{
                                        // --------------------
                                        // Try a generic repair
                                        // --------------------
    return ( EngRepairGeneric ( lprLinear,
                                lprBootRepairDef,
                                wDiskType,
                                byRepairType,
                                lprInfection ));
}


ENGSTATUS EngRepairBootEOFD
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPBOOTREPAIRDEF lprBootRepairDef,   // [in] Boot view of Repair def
    WORD            wDiskType,          // [in] Disk type
    BYTE            byRepairType,       // [in] Repair type
    LPSECTOR        lprInfection        // [in] Sector with partition data
)
{
    ENGSTATUS   esResult = ENGSTATUS_OK;
    PARTENTRY   rPartEntry;
    LPHARDBPB   lprBPB;
    BYTE        byActivePartition;
    SECTOR      rRepair;

                                        // ---------------------
                                        // Find active partition
                                        // ---------------------
    for ( byActivePartition = 0;
          byActivePartition < MAX_PARTITIONS;
          ++byActivePartition )
        {
        EngParseMBRPartition ( lprInfection->abyBuffer,
                               byActivePartition,
                               &rPartEntry );
        if ( rPartEntry.bActive )
            {
            break;
            }
        }
                                        // ------------------------------------
                                        // If none of the partitions are active
                                        // just try a generic repair
                                        // ------------------------------------
    if ( MAX_PARTITIONS >= byActivePartition )
        {
        esResult = ENGSTATUS_ERROR;
        }
                                        // -------------------
                                        // Certlibn style EOFD
                                        // -------------------
     if ( ENGSTATUS_OK == esResult && !lprBootRepairDef->wSpecialFlags )
        {
                                        // -------------------------------
                                        // Calculate location of clean MBR
                                        // -------------------------------
        rRepair.wCylinder = rPartEntry.wEndCylinder;
        rRepair.bySide = rPartEntry.byEndSide;
        rRepair.bySector = rPartEntry.byEndSector +
                           LOBYTE ( lprBootRepairDef->wHDLocation1 );

        if ( ENGSTATUS_OK != EngLinearizeSector ( lprLinear, &rRepair ) ||
             ENGSTATUS_OK != EngValidateMBR ( lprLinear, rRepair.abyBuffer ) ||
             ENGSTATUS_OK != EngReadSectorLinear ( lprLinear, &rRepair ))
            {
            esResult = ENGSTATUS_ERROR;
            }
        else
            {
            rRepair.dwLinearLow = lprInfection->dwLinearLow;
            rRepair.dwLinearHigh = lprInfection->dwLinearHigh;
            esResult = EngWriteSectorLinear ( lprLinear, &rRepair );
            }
        }
                                        // ---------------------
                                        // New AVENGE style EOFD
                                        // ---------------------
    else if ( ENGSTATUS_OK == esResult )
        {

        rRepair.wCylinder = rPartEntry.wEndCylinder;
        rRepair.bySide = rPartEntry.byEndSide;

        if ( lprBootRepairDef->wSpecialFlags & AFTER_ENDING_SECTOR )
            {
            rRepair.bySector = rPartEntry.byEndSector +
                               HIBYTE ( lprBootRepairDef->wHDLocation1 );
            }
        else if ( lprBootRepairDef->wSpecialFlags & BEFORE_ENDING_SECTOR )
            {
            rRepair.bySector = rPartEntry.byEndSector -
                               HIBYTE ( lprBootRepairDef->wHDLocation1 );
            }
        else
            {
            esResult = ENGSTATUS_ERROR;
            }
                                        // ---------------------
                                        // Read the original MBR
                                        // ---------------------
        if ( ENGSTATUS_OK != esResult ||
             ENGSTATUS_OK != EngLinearizeSector ( lprLinear, &rRepair ) ||
             ENGSTATUS_OK != EngReadSectorLinear ( lprLinear, &rRepair ))
            {
            esResult = ENGSTATUS_ERROR;
            }
                                        // ------------------------------------
                                        // Next, check if any Fixups are needed
                                        // ------------------------------------
        if ( ENGSTATUS_OK == esResult  &&
            ( lprBootRepairDef->wSpecialFlags &
             ( FIXUP_MBR_ENDINGSECTOR |
               FIXUP_MBR_TOTALSECTORS |
               FIXUP_BOOTSECTOR )))
            {
                                        // ----------------------------------
                                        // Parse the original MBR's partition
                                        // ----------------------------------
            EngParseMBRPartition ( rRepair.abyBuffer,
                                   byActivePartition,
                                   &rPartEntry );

            if ( lprBootRepairDef->wSpecialFlags & FIXUP_MBR_ENDINGSECTOR )
                {
                rPartEntry.byEndSector +=
                            HIBYTE ( lprBootRepairDef->wRestoreFlags );
                }

            if ( lprBootRepairDef->wSpecialFlags & FIXUP_MBR_TOTALSECTORS )
                {
                rPartEntry.dwTotalSectors +=
                            HIBYTE ( lprBootRepairDef->wRestoreFlags );
                }

            EngReplaceMBRPartition ( rRepair.abyBuffer,
                                     byActivePartition,
                                     rPartEntry );

                                        // ---------------------
                                        // Fixup the Boot Sector
                                        // ---------------------
            if ( lprBootRepairDef->wSpecialFlags & FIXUP_BOOTSECTOR )
                {
                SECTOR rBootSector;

                rBootSector.bySector = rPartEntry.byStartSector;
                rBootSector.wCylinder = rPartEntry.wStartCylinder;
                rBootSector.bySector = rPartEntry.byStartSector;

                                        // --------------------
                                        // Read the Boot Sector
                                        // --------------------

                if ( ENGSTATUS_OK != EngLinearizeSector ( lprLinear, &rBootSector ) ||
                     ENGSTATUS_OK != EngReadSectorLinear ( lprLinear, &rBootSector ))
                    {
                    lprBPB = (LPHARDBPB) ( rBootSector.abyBuffer + BPB_OFFSET );

                                        // -------------------
                                        // For BigFoot volumes
                                        // -------------------
                    if ( rPartEntry.dwTotalSectors > 0x0000FFFFL )
                        {
                        lprBPB->dwBigTotalSectors += LOBYTE ( lprBootRepairDef->wRestoreFlags );
                        }
                    else
                        {
                        lprBPB->wTotalSectors += LOBYTE ( lprBootRepairDef->wRestoreFlags );
                        }

                                        // -----------------------------------
                                        // Validate Boot Sector before writing
                                        // -----------------------------------
                    if ( ENGSTATUS_OK != esResult ||
                         ENGSTATUS_OK != EngValidateBootSector ( lprLinear, rBootSector.abyBuffer ) ||
                         ENGSTATUS_OK != EngWriteSectorLinear ( lprLinear, &rBootSector ))
                        {
                        esResult = ENGSTATUS_ERROR;
                        }
                    }
                }

            if ( ENGSTATUS_OK == esResult )
                {
                rRepair.dwLinearLow = lprInfection->dwLinearLow;
                rRepair.dwLinearHigh = lprInfection->dwLinearHigh;
                esResult = EngWriteSectorLinear ( lprLinear, &rRepair );
                }
            }
        }

    if ( ENGSTATUS_OK != esResult )
        {
        esResult = EngRepairGeneric ( lprLinear,
                                      lprBootRepairDef,
                                      wDiskType,
                                      byRepairType,
                                      lprInfection );
        }

    return ( esResult );
}


ENGSTATUS EngRepairGeneric
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPBOOTREPAIRDEF lprBootRepairDef,   // [in] Boot view of repair def
    WORD            wDiskType,          // [in] Disk type
    BYTE            byRepairType,       // [in] Repair type
    LPSECTOR        lprInfection        // [in] Infected sector
)
{
    ENGSTATUS esResult;

    if ( lprBootRepairDef->dwRepairFlags & CTRL2_NO_GENERIC )
        {
                                        // --------------------------------
                                        // No generic repair for this virus
                                        // --------------------------------
        esResult = ENGSTATUS_ERROR;
        }

    else if ( wDiskType & DISK_FLOPPY )
        {
                                        // --------------------------
                                        // Generic Floppy Boot Sector
                                        // --------------------------
        esResult = EngRepairBootGeneric ( lprLinear, lprInfection, wDiskType );
        }

    else if ( byRepairType < REP_BOOT )
        {
                                        // ------------------
                                        // Generic MBR Repair
                                        // ------------------
        esResult = EngRepairMBRGeneric ( lprLinear, lprInfection, wDiskType );
        }

    else
        {
                                        // -----------------------------
                                        // No generic Boot repair for HD
                                        // -----------------------------
        esResult = ENGSTATUS_ERROR;
        }

    return ( esResult );
}


ENGSTATUS EngRepairMBRGeneric
(
    LPLINEAROBJECT  lprLinear,          // [in] Sector to fill
    LPSECTOR        lprInfection,       // [in] Sector with partition data
    WORD            wDiskType           // [in] Disk type
)
{
    ENGSTATUS   esResult = ENGSTATUS_OK;
    SECTOR      rRepair;

    esResult = EngValidateMBR ( lprLinear, lprInfection->abyBuffer );

    if ( ENGSTATUS_OK == esResult )
        {

                                        // -------------------------
                                        // Initialize sector with 0s
                                        // -------------------------
        MEMSET ( rRepair.abyBuffer, 0, SECTOR_SIZE );

                                        // -------------------------------
                                        // Put MBR Code at start of sector
                                        // -------------------------------
        MEMCPY ( rRepair.abyBuffer, &abyMBRCode, sMBR_CODE );

                                        // ------------------------------------
                                        // Put original partition data at 0x1be
                                        // ------------------------------------
        MEMCPY ( rRepair.abyBuffer + PARTITION_OFFSET,
                 lprInfection->abyBuffer + PARTITION_OFFSET,
                 PARTENTRY_SIZE * 4 );

                                        // ---------------------------
                                        // Put 0xAA55 signature at end
                                        // ---------------------------
        rRepair.abyBuffer [ SECTOR_SIZE - 2 ] = 0x55;
        rRepair.abyBuffer [ SECTOR_SIZE - 1 ] = 0xAA;

        rRepair.dwLinearLow = rRepair.dwLinearHigh = 0;

        esResult = EngWriteSectorLinear ( lprLinear, &rRepair );

        }

    return ( esResult );
}


ENGSTATUS EngRepairBootGeneric
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object to repair
    LPSECTOR        lprInfection,       // [in] Infected Sector
    WORD            wDiskType           // [in] Disk type
)
{
    ENGSTATUS   esResult = ENGSTATUS_OK;
    SECTOR      rRepair;
    UINT        nOffset = 0;
    LPBYTE      lpbyBPB;

    if ( ENGSTATUS_OK == esResult )
        {
                                        // ----------------------
                                        // Fill buffer with zeros
                                        // ----------------------
        MEMSET ( rRepair.abyBuffer, 0, SECTOR_SIZE );

                                        // --------------------------------
                                        // Put JMP + NOP at start of sector
                                        // --------------------------------
        MEMCPY ( rRepair.abyBuffer, abyJMPAndNOP, sJMP_AND_NOP );
        nOffset += sJMP_AND_NOP;

                                        // -----------------------------
                                        // Put OEM ID next (offset 0x03)
                                        // -----------------------------
        MEMCPY ( rRepair.abyBuffer + nOffset, abyOEMID, sOEM_ID );
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
                // if we can't tell what kind of floppy this is
                // use the BPB from the infected boot record.

                lpbyBPB = &lprInfection->abyBuffer[ BPB_OFFSET ];
                break;
                }
            }
        }

    if ( ENGSTATUS_OK == esResult )
        {
                                        // --------------------------
                                        // Put BPB next (offset 0x0B)
                                        // --------------------------
        MEMCPY ( rRepair.abyBuffer + nOffset, lpbyBPB, sBPB );
        nOffset += sBPB;

                                        // -------------------------------
                                        // Put Volume Label at offset 0x2B
                                        // -------------------------------
        nOffset = 0x2B;
        MEMCPY ( rRepair.abyBuffer + nOffset, abyVolumeLabel, sVOLUME_LABEL );

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
        MEMCPY ( rRepair.abyBuffer + nOffset, abyNonBootableCode, sNON_BOOTABLE_CODE );

                                        // ------------------------------------
                                        // Put non-bootable text at offset 0x80
                                        //
                                        // *** WARNING ***
                                        //
                                        // This offset must match the location
                                        // used by the non-bootable code above.
                                        // ------------------------------------
        nOffset = 0x80;
        MEMCPY ( rRepair.abyBuffer + nOffset, abyNonBootableText, sNON_BOOTABLE_TEXT );

                                        // ---------------------------
                                        // Put 0xAA55 signature at end
                                        // ---------------------------
        rRepair.abyBuffer [ SECTOR_SIZE - 2 ] = 0x55;
        rRepair.abyBuffer [ SECTOR_SIZE - 1 ] = 0xAA;

        esResult = EngValidateBootSector ( lprLinear, rRepair.abyBuffer );

        }

    if ( ENGSTATUS_OK == esResult )
        {
        rRepair.dwLinearLow = lprInfection->dwLinearLow;
        rRepair.dwLinearHigh = lprInfection->dwLinearHigh;

        esResult = EngWriteSectorLinear ( lprLinear, &rRepair );
        }

    return ( esResult );

}


ENGSTATUS EngGetFloppyType
(
    LPLINEAROBJECT      lprLinear,      // [in] Linear object to analyze
    LPBYTE              lpbySector,     // [in] Boot sector to anaylze
    LPWORD              lpwDiskType     // [out] Disk type
)
{
    ENGSTATUS   esResult = ENGSTATUS_OK;
    LPFLOPPYBPB lprBPB;
    LPFLOPPYBPB lprTargetBPB;
    BYTE        byBPBIndex;
    BYTE        byMatches;
    BYTE        byMatchedBPB;
    BYTE        byMaxMatches = 0;

    *lpwDiskType = DISK_FLOPPY | DISK_UNKNOWN;
    lprBPB = (LPFLOPPYBPB) ( lpbySector + BPB_OFFSET );

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
        }

    else
        {
                                        // --------------
                                        // try FAT method
                                        // --------------
        esResult = EngAnalyzeFloppy ( lprLinear, lpbySector, lpwDiskType );
        }

    return ( esResult );
}


ENGSTATUS EngAnalyzeFloppy
(
    LPLINEAROBJECT      lprLinear,      // [in] Linear object to analyze
    LPBYTE              lpbySector,     // [in] Sector to analyze
    LPWORD              lpwDiskType     // [out] Disk type
)
{
    ENGSTATUS   esResult = ENGSTATUS_OK;
    SECTOR      rTempSector;
    BYTE        byMediaDescriptor1;
    BYTE        byFAT2Sector;
    LPFLOPPYBPB lprBPB  = (LPFLOPPYBPB) ( lpbySector + BPB_OFFSET );

                                        // ----------------------------------
                                        // Read media descriptor from 1st FAT
                                        // ----------------------------------
    rTempSector.dwLinearLow = 1;
    rTempSector.dwLinearHigh = 0;
    esResult = EngReadSectorLinear ( lprLinear, &rTempSector );

    if ( ENGSTATUS_OK == esResult )
        {
        byMediaDescriptor1 = rTempSector.abyBuffer [ 0 ];
        switch ( byMediaDescriptor1 )
            {

            case 0xFD:
                {
                                        // -------------
                                        // 360k diskette
                                        // -------------
                byFAT2Sector = 4;
                *lpwDiskType = DISK_FLOPPY_360K;
                break;
                }

            case 0xF9:
                {
                                        // --------------------------------
                                        // 720k or 1.2M diskette
                                        // --------------------------------
                                        // Both types share the same media
                                        // descriptor byte. The actual disk
                                        // type is determined later when
                                        // checking the 2nd FAT.
                                        // --------------------------------
                byFAT2Sector = 5;
                *lpwDiskType = DISK_FLOPPY_720K;
                break;
                }

            case 0xF0:
                {
                                        // -----------------------------------
                                        // 1.44M or 2.88M diskette
                                        // -----------------------------------
                                        // Both types share the same media
                                        // descriptor byte and FAT locations.
                                        // The actual type is determined later
                                        // by checking the BPB in lpbySector
                                        // -----------------------------------
                byFAT2Sector = 11;
                *lpwDiskType = DISK_FLOPPY_144M;
                break;
                }

            case 0xFE:
                {
                                        // ---------------------------------
                                        // 160K or 1.2M (1k/sector) diskette
                                        // ---------------------------------
                                        // Both types share the same media
                                        // descriptor byte.  The actual type
                                        // is determined later when checking
                                        // the 2nd FAT.
                                        // ---------------------------------
                byFAT2Sector = 3;
                *lpwDiskType = DISK_FLOPPY_160K;
                break;
                }

            case 0xFC:
                {
                                        // -------------
                                        // 180K diskette
                                        // -------------
                byFAT2Sector = 4;
                *lpwDiskType = DISK_FLOPPY_180K;
                break;
                }

            case 0xFF:
                {
                                        // -------------
                                        // 320K diskette
                                        // -------------
                byFAT2Sector = 3;
                *lpwDiskType = DISK_FLOPPY_320K;
                break;
                }

            case 0xFB:
                {
                                        // -------------
                                        // 640K diskette
                                        // -------------
                byFAT2Sector = 4;
                *lpwDiskType = DISK_FLOPPY_640K;
                break;
                }

            default:
                {
                *lpwDiskType = DISK_FLOPPY | DISK_UNKNOWN;
                esResult = ENGSTATUS_ERROR;
                }
            }
        }

    if ( ENGSTATUS_OK == esResult )
        {
                                        // ----------------------------------
                                        // Read media descriptor from 2nd FAT
                                        // ----------------------------------

        rTempSector.dwLinearLow = byFAT2Sector - 1;
        esResult = EngReadSectorLinear ( lprLinear, &rTempSector );
        }
                                        // -------------------------
                                        // Compare media descriptors
                                        // -------------------------
    if ( ENGSTATUS_OK == esResult &&
         byMediaDescriptor1 != rTempSector.abyBuffer [ 0 ])
        {
                                        // -----------------------------------
                                        // Don't give up yet!  Some disk types
                                        // share media descriptors but differ
                                        // in FAT size so 2nd media descriptor
                                        // won't match.
                                        // -----------------------------------

                                        // -----------------------------------
                                        // 1.2M diskettes have the same Media
                                        // Descriptor (0xF9) as 720k's, but the
                                        // second FAT starts at sector 9 instead
                                        // of sector 5.
                                        // ------------------------------------
        if ( *lpwDiskType == DISK_FLOPPY_720K )
            {
            rTempSector.dwLinearLow = 8;

                                        // -------------------------------
                                        // Read 2nd media descriptor again
                                        // -------------------------------
            esResult = EngReadSectorLinear ( lprLinear, &rTempSector );

                                        // -------------------------
                                        // Compare media descriptors
                                        // -------------------------
            if ( ENGSTATUS_OK == esResult &&
                 byMediaDescriptor1 == rTempSector.abyBuffer [ 0 ])
                {
                *lpwDiskType = DISK_FLOPPY_12M;
                }

            else esResult = ENGSTATUS_ERROR;
            }

                                        // -----------------------------------
                                        // 1.2M diskettes with 1k/sector have
                                        // the same Media Descriptor (0xFE) as
                                        // 160Ks, but the second FAT starts at
                                        // sector 4 (is that physical sector
                                        // 8) instead of sector 3.
                                        // ------------------------------------
        else if ( *lpwDiskType == DISK_FLOPPY_160K )
            {
            rTempSector.dwLinearLow = 7;
                                        // -------------------------------
                                        // Read 2nd media descriptor again
                                        // -------------------------------
            esResult = EngReadSectorLinear ( lprLinear, &rTempSector );

                                        // -------------------------
                                        // Compare media descriptors
                                        // -------------------------
            if ( ENGSTATUS_OK == esResult &&
                 byMediaDescriptor1 == rTempSector.abyBuffer [ 0 ])
                {
                *lpwDiskType = DISK_FLOPPY_12M_2;
                }

            else esResult = ENGSTATUS_ERROR;
            }


        else if ( *lpwDiskType == DISK_FLOPPY_144M )
            {
                                        // ------------------------------------
                                        // DMF floppies look like 1.44M,
                                        // except they have 16 max root
                                        // entries (224 for 1.44M), 3360 total
                                        // sectors (2880 for 1.44M), and 21
                                        // sectors per track (18 for 1.44M).
                                        // If we find ANY of these DMF values
                                        // try reading second FAT at sector 4.
                                        // ------------------------------------
            if ( lprBPB->wMaxRootEntries == 16  ||
                 lprBPB->wTotalSectors == 3360 ||
                 lprBPB->wSectorsPerTrack == 21 )
                {
                                        // ------------------------------------
                                        // There are 2 types of DMF floppies:
                                        // 1K Clusters (2 sectors per cluster
                                        // and 5 sectors per FAT) and 2K
                                        // Clusters (4 sectors per cluster and
                                        // 3 sectors per FAT.)
                                        // ------------------------------------
                if ( lprBPB->wSectorsPerFat == 5 ||
                     lprBPB->bySectorsPerCluster == 2 )
                    {
                    *lpwDiskType = DISK_FLOPPY_1KDMF;
                    rTempSector.dwLinearLow = 6;
                    }
                else if ( lprBPB->wSectorsPerFat == 3 ||
                          lprBPB->bySectorsPerCluster == 4 )
                    {
                    *lpwDiskType = DISK_FLOPPY_2KDMF;
                    rTempSector.dwLinearLow = 4;
                    }
                else esResult = ENGSTATUS_ERROR;

                if ( esResult != ENGSTATUS_ERROR )
                    {
                                        // -------------------------------
                                        // Read 2nd media descriptor again
                                        // -------------------------------
                    esResult = EngReadSectorLinear ( lprLinear, &rTempSector );
                    }

                                        // -------------------------
                                        // Compare media descriptors
                                        // -------------------------
                if ( ENGSTATUS_OK != esResult ||
                     byMediaDescriptor1 != rTempSector.abyBuffer [ 0 ])
                    {
                    esResult = ENGSTATUS_ERROR;
                    }
                }
            else esResult = ENGSTATUS_ERROR;
            }
        else esResult = ENGSTATUS_ERROR;
        }

    else if ( ENGSTATUS_OK == esResult && DISK_FLOPPY_144M == *lpwDiskType )
        {
                                        // ------------------------------------
                                        // 2.88M floppies look like 1.44M,
                                        // except they have 2 sectors per
                                        // cluster (1 for 1.44M), 240 max root
                                        // entries (224 for 1.44M), 5760 total
                                        // sectors (2880 for 1.44M), and 36
                                        // sectors per track (18 for 1.44M).
                                        // Finding ANY of these 2.88M values
                                        // here changes 1.44M floppies to 2.88M
                                        // ------------------------------------
        if ( lprBPB->bySectorsPerCluster == 2 ||
             lprBPB->wMaxRootEntries == 240 ||
             lprBPB->wTotalSectors == 5760 ||
             lprBPB->wSectorsPerTrack == 36 )
            {
            *lpwDiskType = DISK_FLOPPY_288M;
            }
        }

    return ( esResult );
}


VOID EngReplaceMBRPartition
(
    LPBYTE      lpbyMBR,                // [in/out] Entire MBR
    BYTE        byPartNumber,           // [in] Partition to replace (0 - 3)
    PARTENTRY   rPartEntry              // [in] filled out PARTENTRY structure
)
{
    WORD wEncodedSectorCylinder;

    lpbyMBR += PARTITION_OFFSET + ( byPartNumber * PARTENTRY_SIZE );

    if ( TRUE == rPartEntry.bActive )
        {
        *lpbyMBR++ = PARTITION_ACTIVE;
        }
    else
        {
        *lpbyMBR++ = 0;
        }

    *lpbyMBR++ = rPartEntry.byStartSide;

    EngEncodeSectorCylinder ( rPartEntry.byStartSector,
                              rPartEntry.wStartCylinder,
                              &wEncodedSectorCylinder );

    *((LPWORD) lpbyMBR ) = wEncodedSectorCylinder;
    lpbyMBR += 2;

    *lpbyMBR++ = rPartEntry.byFileSystem;

    *lpbyMBR++ = rPartEntry.byEndSide;

    EngEncodeSectorCylinder ( rPartEntry.byEndSector,
                              rPartEntry.wEndCylinder,
                              &wEncodedSectorCylinder );

    *((LPWORD) lpbyMBR ) = wEncodedSectorCylinder;
    lpbyMBR += 2;

    *((LPDWORD) lpbyMBR ) = rPartEntry.dwRelativeSectors;
    lpbyMBR += 4;

    *((LPDWORD) lpbyMBR ) = rPartEntry.dwTotalSectors;
}


VOID EngDecodeSectorCylinder
(
    WORD    wEncoded,                   // [in] Encoded Sect/Cyl as in MBR
    LPBYTE  lpbySector,                 // [out] Decoded Sector
    LPWORD  lpwCylinder                 // [out] Decoded Cylinder
)
{
    *lpbySector = LOBYTE ( wEncoded & 0x003F );
    *lpwCylinder = ((WORD) ( LOBYTE ( wEncoded ) & 0xC0 ) << 2 ) +
                   HIBYTE ( wEncoded );
}


VOID EngEncodeSectorCylinder
(
    BYTE    bySector,                   // [in] Decoded Sector
    WORD    wCylinder,                  // [in] Decoded Cylinder
    LPWORD  lpwEncoded                  // [out] Encoded Sect/Cyl as in MBR
)
{
    *lpwEncoded = (WORD) ( bySector & 0x3F ) +
                  (( HIBYTE ( wCylinder ) & 0x03 ) << 6) +
                  (((WORD) LOBYTE ( wCylinder )) << 8 );
}


ENGSTATUS EngValidateMBR
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPBYTE          lpbyMBR             // [in] Raw MBR to validate
)
{
    ENGSTATUS   esResult = ENGSTATUS_OK;
    BYTE        byPartNumber;

    if ( 0x55 != lpbyMBR [ SECTOR_SIZE - 2 ] ||
         0xAA != lpbyMBR [ SECTOR_SIZE - 1 ])
        {
        esResult = ENGSTATUS_ERROR;
        }

                                        // -----------------------------
                                        // Validate each partition entry
                                        // -----------------------------
    else for ( byPartNumber = 0; byPartNumber < MAX_PARTITIONS; ++byPartNumber )
        {

        if ( ENGSTATUS_OK != EngValidateMBRPartition ( lprLinear,
                                                       lpbyMBR,
                                                       byPartNumber ))
            {
            esResult = ENGSTATUS_ERROR;
            break;
            }
        }

    return ( esResult );
}


ENGSTATUS EngValidateMBRPartition
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPBYTE          lpbyMBR,            // [in] Raw MBR
    BYTE            byPartNumber        // [in] Partition to validate (0 - 3)
)
{
    ENGSTATUS   esResult = ENGSTATUS_OK;
    SECTOR      rBootSector;
    PARTENTRY   rPartEntry;

    EngParseMBRPartition ( lpbyMBR,
                           byPartNumber,
                           &rPartEntry );

    if ( PARTFS_NOT_USED != rPartEntry.byFileSystem )
        {
        rBootSector.dwLinearHigh = 0;
        rBootSector.dwLinearLow = rPartEntry.dwRelativeSectors;

                                        // ------------------------------
                                        // Read and validate boot sector.
                                        // ------------------------------
        if ( ENGSTATUS_OK != EngReadSectorLinear ( lprLinear, &rBootSector ) ||
             ENGSTATUS_OK != EngValidateBootSector ( lprLinear, rBootSector.abyBuffer ))
            {
            esResult = ENGSTATUS_ERROR;
            }
        }
    return ( esResult );
}


ENGSTATUS EngValidateBootSector
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPBYTE          lpbyBootSector      // [in] Raw boot sector to validate
)
{
    ENGSTATUS esResult = ENGSTATUS_OK;

    if ( 0x55 != lpbyBootSector [ SECTOR_SIZE - 2 ] ||
         0xAA != lpbyBootSector [ SECTOR_SIZE - 1 ])
        {
        esResult = ENGSTATUS_ERROR;
        }

    return ( esResult );
}


VOID EngUpdateMBR
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPBYTE          lpbyInfectedMBR,    // [in] Original MBR
    LPBYTE          lpbyCleanMBR        // [in/out] New MBR
)
{
    PARTENTRY   rInfectedPart;
    BYTE        byPartNumber;

    for ( byPartNumber = 0; byPartNumber < MAX_PARTITIONS; ++byPartNumber )
        {

        EngParseMBRPartition ( lpbyInfectedMBR,
                               byPartNumber,
                               &rInfectedPart );

        EngReplaceMBRPartition ( lpbyCleanMBR,
                                 byPartNumber,
                                 rInfectedPart );

        }
}


ENGSTATUS EngReadSectorLinear
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPSECTOR        lprSector           // [in] Sector ro read
)
{
    ENGSTATUS esResult;
    DWORD     dwActual;

    esResult = lprLinear->lpCallBacks->
                    LinearRead ( lprLinear->lpvInfo,
                                 lprSector->dwLinearLow,
                                 lprSector->dwLinearHigh,
                                 lprSector->abyBuffer,
                                 1,
                                 &dwActual );

    if ( ENGSTATUS_OK == esResult && 1 != dwActual )
        {
        esResult = ENGSTATUS_ERROR;
        }

    return ( esResult ) ;
}


ENGSTATUS EngWriteSectorLinear
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPSECTOR        lprSector           // [in] Sector to write
)
{
    ENGSTATUS esResult;
    DWORD     dwActual;

    esResult = lprLinear->lpCallBacks->
                    LinearWrite ( lprLinear->lpvInfo,
                                  lprSector->dwLinearLow,
                                  lprSector->dwLinearHigh,
                                  lprSector->abyBuffer,
                                  1,
                                  &dwActual );

    if ( ENGSTATUS_OK == esResult && 1 != dwActual )
        {
        esResult = ENGSTATUS_ERROR;
        }

    return ( esResult ) ;
}


ENGSTATUS EngLinearizeSector
(
    LPLINEAROBJECT  lprLinear,          // [in] Linear object
    LPSECTOR        lprSector           // [in/out] Sector to linearize
)
{
    lprSector->dwLinearHigh = 0;

    return ( lprLinear->lpCallBacks->
                LinearConvertFromPhysical ( lprLinear->lpvInfo,
                                            lprSector->bySide,
                                            lprSector->wCylinder,
                                            lprSector->bySector,
                                            &lprSector->dwLinearLow ));

}

