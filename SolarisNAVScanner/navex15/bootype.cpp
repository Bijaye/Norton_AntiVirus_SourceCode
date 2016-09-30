// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/bootype.cpv   1.6   12 Nov 1997 20:39:08   CNACHEN  $
//
// Description:
//
// Contains: Bloodhound boot record type determination (FBR, PBR, MBR)
//
// See Also:
//************************************************************************
// $Log:   S:/NAVEX/VCS/bootype.cpv  $
// 
//    Rev 1.6   12 Nov 1997 20:39:08   CNACHEN
// Fixed problem with 55 AA checking in partition table.
// 
//    Rev 1.5   12 Nov 1997 20:37:58   JWILBER
// No change.
// 
//    Rev 1.4   10 Nov 1997 16:47:54   CNACHEN
// Removed bloodhound ifdefs
// 
//    Rev 1.3   13 Oct 1997 16:22:20   CNACHEN
// Fixed fbr/pbr/mbr type determination.  Before, we'd check for at least 1
// active partition 0x80 in the partition table to identify a MBR. Now we check
// for 0 or 1, since this could be a second hard drive with no active partitions.
// 
//    Rev 1.2   15 Sep 1997 11:47:56   CNACHEN
// Added check for USE_BLOOD_BOOT ifdef
// 
// 
//    Rev 1.1   08 Sep 1997 12:10:26   CNACHEN
// Changed prototype of DetermineBootType function.
// 
//    Rev 1.0   05 Sep 1997 20:24:54   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef SYM_NLM

#ifdef VC
#include <windows.h>
#else
#include "platform.h"
#endif

#include "bootype.h"

// err on the side of PBR
// only report FBR if absolutely sure
// report MBR if we find valid partition entries

int DetermineBootType
(
    LPBYTE                      lpbyBooBuffer
)
{
    LPMBR                       lpstMBR;
    LPBOOT_RECORD               lpstBR;
    int                         i, nActive;

    lpstBR = (LPBOOT_RECORD)lpbyBooBuffer;

    if (lpstBR->wBytesPerSector == BYTES_PER_SECTOR)
    {
        // probably dealing with a boot record...

        if (lpstBR->wNumHeads == FLOPPY_HEADS)
        {
            // probably a floppy disk, but could be a ZIP drive or something

            if (lpstBR->wFatSize <= MAX_FLOPPY_SECTORS_PER_FAT &&
                lpstBR->wFatSize != 0)
            {
                // definitely a floppy diskette...

                return(BOO_TYPE_FLOPPY);
            }

            // probably a ZIP drive PBR...

            if (lpstBR->byMediaDescriptor == FIXED_DISK_MEDIA_DESCRIPTOR)
                return(BOO_TYPE_PBR);
        }
    }

    // now we're not sure...

    lpstMBR = (LPMBR)lpbyBooBuffer;

    nActive = 0;

    for (i=0;i<MAX_PARTITION_ENTRIES;i++)
    {
        if (lpstMBR->stEntry[i].byBoot == 0x55 &&
            lpstMBR->stEntry[i].byStartHead == 0xAA)
        {
            break;
        }

        else if (lpstMBR->stEntry[i].byBoot == ACTIVE_PARTITION)
            nActive++;
        else if (lpstMBR->stEntry[i].byBoot != INACTIVE_PARTITION)
        {
            // not sure what this is... assume its a PBR

            return(BOO_TYPE_PBR);
        }
    }

    if (nActive == 1 || nActive == 0)
        return(BOO_TYPE_MBR);

    return(BOO_TYPE_PBR);
}

// #endif

#endif // SYM_NLM
