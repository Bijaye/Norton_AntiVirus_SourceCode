/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/DSK_BPB.C_v   1.5   18 Mar 1998 12:19:48   MBROWN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *      BPBTables                                                       *
 *      DiskBPBToDiskRec                                                *
 *      DiskBPBToAbsDiskRec                                             *
 *      DiskGetBPBVersion                                               *
 *      DiskDiskRecToBPB                                                *
 *      DiskBPBTable                                                    *
 *      DiskBPBFromIndex                                                *
 *      DiskGetHDParams                                                 *
 *      DiskFillInBPB                                                   *
 *      DiskIsValidBPB                                                  *
 *      DiskCalcPhysicalSectors                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/DSK_BPB.C_v  $ *
// 
//    Rev 1.5   18 Mar 1998 12:19:48   MBROWN
// Changed ifdef from SYM_WIN to SYM_WIN32 for problem building WDA platform.
// 
//    Rev 1.4   17 Mar 1998 19:12:00   MBROWN
// Updated DiskGetPhysicalType and DiskGetHDParams for extended BIOS support.
// 
//    Rev 1.3   11 Mar 1998 15:05:32   SKURTZ
// Numerous changes for Extended Int13 support
//
//    Rev 1.2   25 Sep 1996 13:37:32   RCHINTA
// Merging changes from QuakeC (NCX Platform: in DiskGetHDParams()
// absDiskPtr->totalTracks calculation)
//
//    Rev 1.1   29 Aug 1996 16:42:36   DHERTEL
// Merged changes from Quake E:
// Fixed bug in DiskGetBPBVersion().  Now will correctly identify DOS7.1
// (Windows95/OSR2) boot sector.
// Use _DiskGetPhysicalInfo as opposed to DiskGetPhysicalInfo to prevent problems with SCSI drives in
//  DOS apps, should not affect windows apps
 ************************************************************************/

#include "platform.h"
#include "disk.h"
#include "xapi.h"

BOOL LOCAL PASCAL FindPartitionTableEntry (LPDWORD	partitionHiddenPtr,
				           LPABSDISKREC	absDiskPtr);

/************************************************************************/
/* BPBTables								*/
/*	BPB (Bios Parameter Block(tables for the different supported	*/
/*	floppy disk types. 						*/
/*									*/
/*	NOTE: Please PRESERVE the order. If you change the order or	*/
/*	add more entries, change the constants BPB_INDEX_xxxx		*/
/************************************************************************/

BPBREC BPBTables[] =
    {
        {				/* 160K format: 40 tracks	*/
	512,				/*    Bytes Per Sector		*/
	1,				/*    Sectors Per Cluster	*/
	1,				/*    Start Of FAT		*/
	2,				/*    Number of FATs		*/
	64,				/*    Max. Root entries		*/
	320,				/*    Total sectors		*/
	0xFE,				/*    Media Descriptor Byte	*/
	1,				/*    Size of 1 FAT		*/
	8,				/*    Sectors Per Track		*/
	1,				/*    Number of Heads		*/
	0L,				/*    Number of Hidden Sectors  */
	0L,				/*    Large Total Sectors	*/
	0,0,0,0,0,0			/*    Reserved sectors		*/
	},

        {				/* 180K format: 40 tracks	*/
	512,				/*    Bytes Per Sector		*/
	1,				/*    Sectors Per Cluster	*/
	1,				/*    Start Of FAT		*/
	2,				/*    Number of FATs		*/
	64,				/*    Max. Root entries		*/
	360,				/*    Total sectors		*/
	0xFC,				/*    Media Descriptor Byte	*/
	2,				/*    Size of 1 FAT		*/
	9,				/*    Sectors Per Track		*/
	1,				/*    Number of Heads		*/
	0L,				/*    Number of Hidden Sectors  */
	0L,				/*    Large Total Sectors	*/
	0,0,0,0,0,0			/*    Reserved sectors		*/
	},

        {				/* 320K format: 40 tracks	*/
	512,				/*    Bytes Per Sector		*/
	2,				/*    Sectors Per Cluster	*/
	1,				/*    Start Of FAT		*/
	2,				/*    Number of FATs		*/
	112,				/*    Max. Root entries		*/
	640,				/*    Total sectors		*/
	0xFF,				/*    Media Descriptor Byte	*/
	1,				/*    Size of 1 FAT		*/
	8,				/*    Sectors Per Track		*/
	2,				/*    Number of Heads		*/
	0L,				/*    Number of Hidden Sectors  */
	0L,				/*    Large Total Sectors	*/
	0,0,0,0,0,0			/*    Reserved sectors		*/
	},

        {				/* 360K format: 40 tracks	*/
	512,				/*    Bytes Per Sector		*/
	2,				/*    Sectors Per Cluster	*/
	1,				/*    Start Of FAT		*/
	2,				/*    Number of FATs		*/
	112,				/*    Max. Root entries		*/
	720,				/*    Total sectors		*/
	0xFD,				/*    Media Descriptor Byte	*/
	2,				/*    Size of 1 FAT		*/
	9,				/*    Sectors Per Track		*/
	2,				/*    Number of Heads		*/
	0L,				/*    Number of Hidden Sectors  */
	0L,				/*    Large Total Sectors	*/
	0,0,0,0,0,0			/*    Reserved sectors		*/
	},

        {				/* 720K format: 80 tracks	*/
	512,				/*    Bytes Per Sector		*/
	2,				/*    Sectors Per Cluster	*/
	1,				/*    Start Of FAT		*/
	2,				/*    Number of FATs		*/
	112,				/*    Max. Root entries		*/
	1440,				/*    Total sectors		*/
	0xF9,				/*    Media Descriptor Byte	*/
	3,				/*    Size of 1 FAT		*/
	9,				/*    Sectors Per Track		*/
	2,				/*    Number of Heads		*/
	0L,				/*    Number of Hidden Sectors  */
	0L,				/*    Large Total Sectors	*/
	0,0,0,0,0,0			/*    Reserved sectors		*/
	},

        {				/* 1.2M format: 80 tracks	*/
	512,				/*    Bytes Per Sector		*/
	1,				/*    Sectors Per Cluster	*/
	1,				/*    Start Of FAT		*/
	2,				/*    Number of FATs		*/
	224,				/*    Max. Root entries		*/
	2400,				/*    Total sectors		*/
	0xF9,				/*    Media Descriptor Byte	*/
	7,				/*    Size of 1 FAT		*/
	15,				/*    Sectors Per Track		*/
	2,				/*    Number of Heads		*/
	0L,				/*    Number of Hidden Sectors  */
	0L,				/*    Large Total Sectors	*/
	0,0,0,0,0,0			/*    Reserved sectors		*/
	},

        {				/* 1.4M format: 80 tracks	*/
	512,				/*    Bytes Per Sector		*/
	1,				/*    Sectors Per Cluster	*/
	1,				/*    Start Of FAT		*/
	2,				/*    Number of FATs		*/
	224,				/*    Max. Root entries		*/
	2880,				/*    Total sectors		*/
	0xF0,				/*    Media Descriptor Byte	*/
	9,				/*    Size of 1 FAT		*/
	18,				/*    Sectors Per Track		*/
	2,				/*    Number of Heads		*/
	0L,				/*    Number of Hidden Sectors  */
	0L,				/*    Large Total Sectors	*/
	0,0,0,0,0,0			/*    Reserved sectors		*/
	},

        {				/* 2.8M format: 80 tracks	*/
	512,				/*    Bytes Per Sector		*/
	2,				/*    Sectors Per Cluster	*/
	1,				/*    Start Of FAT		*/
	2,				/*    Number of FATs		*/
	240,				/*    Max. Root entries		*/
	5760,				/*    Total sectors		*/
	0xF0,				/*    Media Descriptor Byte	*/
	9,				/*    Size of 1 FAT		*/
	36,				/*    Sectors Per Track		*/
	2,				/*    Number of Heads		*/
	0L,				/*    Number of Hidden Sectors  */
	0L,				/*    Large Total Sectors	*/
	0,0,0,0,0,0			/*    Reserved sectors		*/
	},

	{				/* 1.2M format: 8 Sector/tracks */
	1024,				/*	  Bytes Per Sector			*/
	1,				/*	  Sectors Per Cluster		*/
	1,				/*	  Start Of FAT				*/
	2,				/*	  Number of FATs			*/
	192,				/*	  Max. Root entries 		*/
	1232,				/*	  Total sectors 			*/
	0xFE,				/*	  Media Descriptor Byte 	*/
	2,				/*	  Size of 1 FAT 			*/
	8,				/*	  Sectors Per Track 		*/
	2,				/*	  Number of Heads			*/
	0L, 				/*	  Number of Hidden Sectors	*/
	0L, 				/*	  Large Total Sectors		*/
	0,0,0,0,0,0 			/*	  Reserved sectors			*/
	},

	{				/* 640K format: 8 Sector/tracks */
	512,				/*	  Bytes Per Sector			*/
	2,				/*	  Sectors Per Cluster		*/
	1,				/*	  Start Of FAT				*/
	2,				/*	  Number of FATs			*/
	112,				/*	  Max. Root entries 		*/
	1280,				/*	  Total sectors 			*/
	0xFB,				/*	  Media Descriptor Byte 	*/
	2,				/*	  Size of 1 FAT 			*/
	8,				/*	  Sectors Per Track 		*/
	2,				/*	  Number of Heads			*/
	0L, 				/*	  Number of Hidden Sectors	*/
	0L, 				/*	  Large Total Sectors		*/
	0,0,0,0,0,0 			/*	  Reserved sectors			*/
	},

        {				/* End of Table marker  	*/
	0,				/*    Bytes Per Sector		*/
	0,				/*    Sectors Per Cluster	*/
	0,				/*    Start Of FAT		*/
	0,				/*    Number of FATs		*/
	0,				/*    Max. Root entries		*/
	0,				/*    Total sectors		*/
	0,				/*    Media Descriptor Byte	*/
	0,				/*    Size of 1 FAT		*/
	0,				/*    Sectors Per Track		*/
	0,				/*    Number of Heads		*/
	0,				/*    Number of Hidden Sectors  */
	0,				/*    Large Total Sectors	*/
	0,0,0,0,0,0			/*    Reserved sectors		*/
	},
    };

/*----------------------------------------------------------------------*/
/* DiskBPBtoDiskRec							*/
/*	Given a BPB, fills in an equivalent DiskRec.			*/
/*----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI DiskBPBToDiskRec (BYTE dl, LPCBPBREC BPBPtr, LPDISKREC diskPtr)
{
    auto	UINT	dirEntriesPerSector;

					/*------------------------------*/
					/* Set drive letter and number	*/
					/*------------------------------*/
    diskPtr->dl = dl;
    diskPtr->dn = dl - (BYTE)'A';
					/*------------------------------*/
    					/* Set Bytes Per Sector		*/
					/*------------------------------*/
    diskPtr->bytesPerSector = BPBPtr->bytesPerSector;

					/*------------------------------*/
					/* Set Sectors Per Cluster	*/
					/*------------------------------*/
    diskPtr->sectorsPerCluster = BPBPtr->sectorsPerCluster;

					/*------------------------------*/
					/* Set Bytes Per Cluster	*/
					/*------------------------------*/
    diskPtr->bytesPerCluster = diskPtr->bytesPerSector *
    			       diskPtr->sectorsPerCluster;

					/*------------------------------*/
    					/* Set Start of FAT		*/
					/*------------------------------*/
    diskPtr->startOfFat = BPBPtr->startOfFat;

					/*------------------------------*/
					/* Set Number of FATs		*/
					/*------------------------------*/
    diskPtr->numberFats = BPBPtr->numberFats;

					/*------------------------------*/
					/* Set Max Root Entries		*/
					/*------------------------------*/
    diskPtr->maxRootEntries = BPBPtr->maxRootEntries;

					/*------------------------------*/
					/* Set Media Descriptor Byte	*/
					/*------------------------------*/
    diskPtr->mediaDescriptor = BPBPtr->mediaDescriptor;

					/*------------------------------*/
					/* Set Sectors Per FAT		*/
					/*------------------------------*/
    diskPtr->dwSectorsPerFATEx = BPBPtr->sectorsPerFat;

					/*------------------------------*/
					/* Compute Sectors in Root	*/
					/* Round up			*/
					/*------------------------------*/
    dirEntriesPerSector = BPBPtr->bytesPerSector / sizeof(DIRENTRYREC);
    diskPtr->sectorsInRoot  = BPBPtr->maxRootEntries / dirEntriesPerSector;
    if (BPBPtr->maxRootEntries % dirEntriesPerSector)
        diskPtr->sectorsInRoot++;

					/*------------------------------*/
    					/* Compute Starting Root Sector	*/
					/*------------------------------*/
    diskPtr->startOfRootDir = BPBPtr->startOfFat +
    			       (BPBPtr->numberFats * BPBPtr->sectorsPerFat);

					/*------------------------------*/
					/* Compute Starting Data Sector	*/
					/*------------------------------*/
    diskPtr->dwStartOfDataEx = diskPtr->startOfRootDir + diskPtr->sectorsInRoot;

					/*------------------------------*/
    					/* Set Sectors Before Partition */
					/*------------------------------*/
    diskPtr->sectorsBeforePartition = BPBPtr->hiddenSectors;

					/*------------------------------*/
					/* Set Total Sectors		*/
					/*------------------------------*/
    diskPtr->totalSectors = (BPBPtr->oldTotalSectors == 0) ?
    			BPBPtr->totalSectors : BPBPtr->oldTotalSectors;

					/*------------------------------*/
					/* Compute Number of Clusters	*/
					/*------------------------------*/
    diskPtr->dwMaxClusterEx = ((diskPtr->totalSectors -
                       diskPtr->dwStartOfDataEx) /
    		       		   BPBPtr->sectorsPerCluster);

					/*------------------------------*/
    					/* Set to Maximum Cluster	*/
					/*------------------------------*/
    diskPtr->dwMaxClusterEx++;

                    /*------------------------------*/
					/* Set 16-bit FAT flag		*/
					/*------------------------------*/
    if (diskPtr->dwMaxClusterEx < (UINT)0x0FF6)
        {
        diskPtr->clEOF = FAT12VALUEMASK&~(DWORD)7;
        diskPtr->byFATType = DR_FAT12;
        }
    else if (diskPtr->dwMaxClusterEx < 0xFFFF)
        {
        diskPtr->clEOF = FAT16VALUEMASK&~(DWORD)7 ;
        diskPtr->byFATType = DR_FAT16;
        }
    else
        {
        diskPtr->clEOF = FAT32VALUEMASK&~(DWORD)7;
        diskPtr->byFATType = DR_FAT32;
        }
    diskPtr->clBAD = diskPtr->clEOF-1;

					/*------------------------------*/
                    /* Set BIGFOOT state            */
					/*------------------------------*/
    if( CanSupportExtendedIO() )
        {
        diskPtr->byIOType = DR_IO_EXTENDED;
        }
    else
        {
        if (diskPtr->totalSectors <= 0x0000FFFFL)
            diskPtr->byIOType = DR_IO_NORMAL;
        else
            diskPtr->byIOType = DR_IO_BIGFOOT;
        }




}

/*----------------------------------------------------------------------*/
/* DiskBPBtoAbsDiskRec							*/
/*	Given a BPB, fills in an equivalent AbsDiskRec.			*/
/*	The drive number, numDrives, and current location fields are	*/
/*	not modified.							*/
/*----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI DiskBPBToAbsDiskRec (LPCBPBREC BPBPtr, LPABSDISKREC absDiskPtr)
{
					/*------------------------------*/
    					/* Set starting head, track,	*/
					/*	and sector		*/
					/*------------------------------*/
    absDiskPtr->dwStartingTrack = 0;
    absDiskPtr->dwStartingHead = 0;
    absDiskPtr->dwStartingSector = 1;

					/*------------------------------*/
    					/* Get heads			*/
					/*------------------------------*/
    absDiskPtr->dwTotalHeads = BPBPtr->heads;
    absDiskPtr->dwEndingHead = (BPBPtr->heads - 1);

					/*------------------------------*/
    					/* Get tracks			*/
					/*------------------------------*/
    absDiskPtr->dwTotalTracks = BPBPtr->oldTotalSectors /
    				(BPBPtr->sectorsPerTrack * BPBPtr->heads);
    absDiskPtr->dwEndingTrack = absDiskPtr->dwTotalTracks - 1;

					/*------------------------------*/
    					/* Get sectors per track	*/
					/*------------------------------*/
    absDiskPtr->dwSectorsPerTrack =
                absDiskPtr->dwEndingSector = (BYTE)BPBPtr->sectorsPerTrack;

    if (HWIsNEC())
        {
        // 640K || 720K ?
        if (BPBPtr->oldTotalSectors == 1280 || BPBPtr->oldTotalSectors == 1440)
            {
            absDiskPtr->dn &= 0x0f;
            absDiskPtr->dn |= 0x10;
            }
            // 1.21M || 1.25M ?
        else if( BPBPtr->oldTotalSectors == 2400
                 || BPBPtr->oldTotalSectors == 1232 )
            {
            absDiskPtr->dn &= 0x0f;
            absDiskPtr->dn |= 0x90;
            }
            // 1.44M ?
        else if( BPBPtr->oldTotalSectors == 2880 )
            {
            absDiskPtr->dn &= 0x0f;
            absDiskPtr->dn |= 0x30;
            }
        }
}

/*----------------------------------------------------------------------*/
/*	Returns the DOS version that a disk was formatted under, by	*/
/*	looking at the OEM signature in the Boot Record.  Also returns	*/
/*	the size of the BPB, based upon the DOS version determined.	*/
/*----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI DiskGetBPBVersion (LPSTR oemSignature, UINT FAR * BPBSizePtr)
{
    auto	UINT		dosVersion;
    auto	UINT		version;
    auto	UINT		bpbSize;


    dosVersion = DOSGetVersion();

    if ((oemSignature [6] == '.') &&
        CharIsNumeric(oemSignature [7]) &&
        CharIsNumeric(oemSignature [5]))
        {
        if ( 0 == STRNCMP(oemSignature, "MSWIN", 5) )
            {
            if(oemSignature [7] == '1')
                version = DOS_7_10;         // Windows 95 OSR2
            else
                version = DOS_7_00;         // Windows 95
            }
        else
            version =  (256 * (oemSignature [5] - '0')) +
                       (10 * (oemSignature [7] - '0'));

                    /* If invalid, use DOS version  */
        if (version < 0x0200 || version > 0x0A00)
            version = dosVersion;
        else if ((version == DOS_3_30) && (dosVersion == DOS_3_31))
            version = DOS_3_31;
        }
    else
        {
        version = dosVersion;
        }

					/*------------------------------*/
					/* Slap a new boot program on	*/
					/* the disk.			*/
					/* Use the version number	*/
					/* computed to determine which	*/
					/* version of boot record to	*/
					/* slap.			*/
					/*------------------------------*/
    if (version < DOS_3_00)
        {				/* Use DOS 2.0 boot record	*/
        bpbSize = 19;
        }
    else if (version < DOS_3_20)
        {				/* Use DOS 3.1 boot record	*/
        bpbSize = 19;
        }
    else if (version <= DOS_3_30)
    	{				/* use DOS 3.2, 3.3 boot record	*/
        bpbSize = 21;
        }
    else
    	{				/* Use DOS 4.0 boot record	*/
        bpbSize = 25;
        }

    *BPBSizePtr = bpbSize;
    return (version);
}

/*----------------------------------------------------------------------*/
/* DiskDiskRecToBPB							*/
/*	Given a Disk Rec, fills in an equivalent BPB.			*/
/*----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI DiskDiskRecToBPB (LPCDISKREC diskPtr, LPBPBREC BPBPtr)
{

					/*------------------------------*/
    					/* Set Bytes Per Sector		*/
					/*------------------------------*/
    BPBPtr->bytesPerSector = diskPtr->bytesPerSector;

					/*------------------------------*/
					/* Set Sectors Per Cluster	*/
					/*------------------------------*/
    BPBPtr->sectorsPerCluster = diskPtr->sectorsPerCluster;

					/*------------------------------*/
    					/* Set Start of FAT		*/
					/*------------------------------*/
    BPBPtr->startOfFat = diskPtr->startOfFat;

					/*------------------------------*/
					/* Set Number of FATs		*/
					/*------------------------------*/
    BPBPtr->numberFats = diskPtr->numberFats;

					/*------------------------------*/
					/* Set Max Root Entries		*/
					/*------------------------------*/
    BPBPtr->maxRootEntries = diskPtr->maxRootEntries;

					/*------------------------------*/
					/* Set Media Descriptor Byte	*/
					/*------------------------------*/
    BPBPtr->mediaDescriptor = diskPtr->mediaDescriptor;

					/*------------------------------*/
					/* Set Sectors Per FAT		*/
					/*------------------------------*/
    BPBPtr->sectorsPerFat = (UINT)diskPtr->dwSectorsPerFATEx;

					/*------------------------------*/
    					/* Set Sectors Before Partition */
					/*------------------------------*/
    BPBPtr->hiddenSectors = diskPtr->sectorsBeforePartition;

					/*------------------------------*/
					/* Set Total Sectors		*/
					/*------------------------------*/
    if (diskPtr->totalSectors > 0x0000FFFFL)
	{
	BPBPtr->oldTotalSectors = 0;
	BPBPtr->totalSectors = diskPtr->totalSectors;
    	}
    else
    	{
	BPBPtr->oldTotalSectors = (UINT)diskPtr->totalSectors;
	BPBPtr->totalSectors = 0;
	}
}

/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This is routine creates a table of BPB index numbers for the 	*/
/*	specified drive type.  The index numbers can then be used to	*/
/*	obtain the possible BPB records for the drive. 			*/
/*                                                                      */
/* Arguments                                                            */
/*      UINT	wDiskMinorType	The disk's minor type (e.g., TYPE_360K(*/
/*									*/
/*	LPBYTE	lpbyBPBTable	A long pointer to an array of bytes     */
/*                              at least MAX_BPB bytes long.            */
/*                              The routine will fill the table with    */
/*                              the BPB_INDEX values for the drive      */
/*                              type.					*/
/*                                                                      */
/*	LPBYTE	lpwCount	Set to the number of index values	*/
/*                              copied into the table.                  */
/*                                                                      */
/* Return value:                                                        */
/*      NOERR                                                           */
/*                                                                      */
/************************************************************************/
/* 4/18/91 DMD Function created.                                        */
/************************************************************************/

UINT SYM_EXPORT WINAPI DiskBPBTable (UINT wDiskMinorType,
                                 LPBYTE lpbyBPBTable, UINT FAR * lpwCount)
{
    UINT count = 0;

    if (!HWIsNEC())
        {
        switch (wDiskMinorType)
            {
                                                /* 5 1/4 inch drives	*/
            case TYPE_12M:
                count = 5;
                lpbyBPBTable[4] = BPB_INDEX_12M;
	        /* FALL THROUGH */

            case TYPE_360K:
                if (count == 0)
                    count = 4;
                lpbyBPBTable[3] = BPB_INDEX_360K;
                lpbyBPBTable[2] = BPB_INDEX_320K;
                lpbyBPBTable[1] = BPB_INDEX_180K;
                lpbyBPBTable[0] = BPB_INDEX_160K;
                break;

                                                /* 3 1/2 inch drives	*/
            case TYPE_288M:
                count = 3;
                lpbyBPBTable[2] = BPB_INDEX_288M;
	        /* FALL THROUGH */

            case TYPE_144M:
                if (count == 0)
                    count = 2;
                lpbyBPBTable[1] = BPB_INDEX_144M;
	        /* FALL THROUGH */

            case TYPE_720K:
                if (count == 0)
                    count = 1;
                lpbyBPBTable[0] = BPB_INDEX_720K;
                break;
            }
        }
    else
        {
        switch (wDiskMinorType)
            {
            case TYPE_144M:
                count = 5;
                lpbyBPBTable[4] = BPB_INDEX_144M;
		/* FALL THROUGH */

            case TYPE_12M:
                if( count == 0 )
	            count = 4;
                lpbyBPBTable[3] = BPB_INDEX_12M8;
                lpbyBPBTable[2] = BPB_INDEX_12M;
		/* FALL THROUGH */

            case TYPE_720K:
                if (count == 0)
                    count = 2;
                lpbyBPBTable[1] = BPB_INDEX_720K;
                lpbyBPBTable[0] = BPB_INDEX_640K;
                break;

            case TYPE_360K:
                if (count == 0)
                    count = 2;
                lpbyBPBTable[1] = BPB_INDEX_360K;
                lpbyBPBTable[0] = BPB_INDEX_320K;
                break;
            }
        }

    *lpwCount = count;
    return (NOERR);
}

/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*	Get a pointer to a BPB record from a BPB index value.		*/
/*                                                                      */
/*	The index value should come from a table build by		*/
/*	DiskBPBTable().							*/
/*                                                                      */
/* Arguments                                                            */
/*      UINT	 wBPBIndex	A BPB_INDEX_ constant			*/
/*									*/
/*	LPBPBREC lpBPBRec	Far ptr to a BPB record which will be	*/
/*				filled with values determined by the	*/
/*                              index.                                  */
/*                                                                      */
/* Return value:                                                        */
/*      NOERR                                                           */
/*                                                                      */
/************************************************************************/

UINT SYM_EXPORT WINAPI DiskBPBFromIndex (UINT wBPBIndex, LPBPBREC lpBPBRec)
{
    *lpBPBRec = BPBTables[wBPBIndex];
    return (NOERR);
}


/*----------------------------------------------------------------------*/
/* DiskGetHDParams							*/
/*	Gets the logical and physical attributes about a hard disk.	*/
/*									*/
/* Returns TRUE if successful, else FALSE				*/
/*----------------------------------------------------------------------*/
// Note: Thie function used to set the total tracks value for absDiskPtr to
// the number of tracks used by the partition only. Now it will set this to
// the total number for the entire disk. This is because there are now actual
// starting and ending locations stored in the same structure (as opposed to
// translated values) and this will cause problems if ConvertPhySectorToLong
// is called later.
BOOL SYM_EXPORT WINAPI        DiskGetHDParams (LPDISKREC	diskPtr,
				 LPABSDISKREC	absDiskPtr,
				 LPBPBREC	BPBPtr)
{

    DWORD		        part1;
    DWORD		        BPBhiddenSectors;
    INT13REC		    int13Package;	    /* Map logical -> physical	*/
    DeviceParamRecord 	deviceParamTable;   /* Generic IOCTL record	*/
    BOOL		        validGenericIO;
    ABSDISKREC          tempAbsDisk;
    ABSDISKREC          tempAbsDiskBasic;
    DWORD               dwStartingSector, dwEndingSector;

    MEMSET(&tempAbsDisk, 0, sizeof(ABSDISKREC));
    MEMSET(&tempAbsDiskBasic, 0, sizeof(ABSDISKREC));

					/* Get disk parameters		*/
					/*     from driver info.	*/
    if (!DiskGetDriverTable (diskPtr->dl, diskPtr))
        return (FALSE);

					/* Set to 0 so we get the	*/
					/*	default BPB		*/
					/* and clear entire buffer	*/
    MEMSET(&deviceParamTable, 0, sizeof(DeviceParamRecord));
    if (validGenericIO = (BOOL)DiskGenericIOCTL (GET_DEVICE_PARAMS, diskPtr->dl,
					       &deviceParamTable))
        {				/* Use values from GENERIC IO	*/

					/* Get total tracks		*/
	    if (HWIsNEC())
            {
            absDiskPtr->dwTotalTracks = deviceParamTable.numberOfTracks *
                                      ( deviceParamTable.BPB.bytesPerSector /
                                                ( 128 << absDiskPtr->sectorLenCode ) );
            }
        else
            {
            absDiskPtr->dwTotalTracks = deviceParamTable.numberOfTracks;
            }

					/* Get total heads		*/
        absDiskPtr->dwTotalHeads = (BYTE)deviceParamTable.BPB.heads;

					/* Get total sectors per track	*/
        absDiskPtr->dwSectorsPerTrack = (BYTE)deviceParamTable.BPB.sectorsPerTrack;

					/* Get total sectors		*/
	    diskPtr->totalSectors = deviceParamTable.BPB.oldTotalSectors;


	    if (DOSGetPCMOSVersion())
	        {
	        diskPtr->totalSectors = diskPtr->totalSectors +
	    		    ((deviceParamTable.BPB.sectorsPerFat >> 8) * 0x10000L);
	        }
	    else if ((diskPtr->totalSectors == 0) && deviceParamTable.BPB.totalSectors)
	        {
	        diskPtr->totalSectors = deviceParamTable.BPB.totalSectors;
	        }

					    /* Get the BPB hidden sectors	*/
	    BPBhiddenSectors = deviceParamTable.BPB.hiddenSectors;

					    /* Get media descriptor		*/
	    diskPtr->mediaDescriptor = deviceParamTable.BPB.mediaDescriptor;
        // The info we have now is based on plain int13 geometry. However,
        // other fields in the absDiskPtr are base on extended int13 (actual)
        // geometry, so get that geometry now.
        tempAbsDisk.dn = tempAbsDiskBasic.dn = absDiskPtr->dn;
#if defined (SYM_WIN)
        if (DiskGetPhysicalInfo (&tempAbsDisk))
#else
        if (_DiskGetPhysicalInfo (&tempAbsDisk))
#endif
            return (FALSE);

        if(tempAbsDisk.bIsExtInt13 == ISINT13X_YES)
#ifndef SYM_WIN32 // DOS/WIN16
            DiskGetPhysicalInfoBasic(&tempAbsDiskBasic);
#else           // WIN32
            DiskGetPhysicalInfoEx(&tempAbsDiskBasic, &tempAbsDisk);
#endif
        else
            tempAbsDiskBasic = tempAbsDisk;

	    }
    else
    	{
					/* Use BIOS function 08h to	*/
					/*	get disk parameters	*/
#if defined (SYM_WIN)
    if (DiskGetPhysicalInfo (absDiskPtr))
#else
    if (_DiskGetPhysicalInfo (absDiskPtr))
#endif
        return (FALSE);

        if (!HWIsNEC())
            {
    					/* Get hidden sectors from	*/
					/*	partition table		*/
					/* If not in partition table,	*/
					/* then just use value returned	*/
					/* by function 32h call		*/
	    if (!FindPartitionTableEntry (&BPBhiddenSectors, absDiskPtr))
	        {
	    				/* Couldn't find partition, just */
					/* use 0 value 			 */
	        BPBhiddenSectors = 0;
	        }
            }
        else
            {
            //*
	    //* following is for 'NEC-DOS 3.10 PS129' or previous
	    //*
	    if ( ! ( diskPtr->mediaDescriptor & 0xF0 ) )
	        {
	        if ( ! ( absDiskPtr->dn & 0x50 ) )
	    	    diskPtr->mediaDescriptor = 0xF8;	// Type HD
	        else if ( ! ( absDiskPtr->dn & 0x60 ) )	// 2HD FD
	            {
                if ( absDiskPtr->dwSectorsPerTrack > 8 )
	    		diskPtr->mediaDescriptor = 0xF9;	// Type 1.2M 15sec
	    	    else
	    		diskPtr->mediaDescriptor = 0xFE;	// Type 8 Inch double
                    }
	        else								// 2DD FD
                if ( absDiskPtr->dwSectorsPerTrack > 8 )
	    	        diskPtr->mediaDescriptor = 0xF9;	// Type 720K
	            else
	    	        diskPtr->mediaDescriptor = 0xFB;	// Type 640K
	            }
                }
					/* Compute total sectors, based	*/
					/* on the max. clusters		*/
        diskPtr->totalSectors = ((DWORD)(diskPtr->dwMaxClusterEx - 1) *
              diskPtr->sectorsPerCluster) + diskPtr->dwStartOfDataEx;
	}
    
    if(tempAbsDisk.bIsExtInt13 == ISINT13X_YES)
        {
        // Use the actual starting location, and actual totals in our temp struct
        // to find the linear starting sector
        dwStartingSector = ConvertPhySectorToLong(&tempAbsDisk, 
                                                  absDiskPtr->dwStartingHead,
                                                  absDiskPtr->dwStartingTrack,
                                                  absDiskPtr->dwStartingSector);

        absDiskPtr->bIsExtInt13 = tempAbsDisk.bIsExtInt13;

        // Now convert the starting location to translated values
        ConvertLongToPhySector(&tempAbsDiskBasic, 
                               dwStartingSector, 
                               &tempAbsDiskBasic.dwStartingHead,
                               &tempAbsDiskBasic.dwStartingTrack,
                               &tempAbsDiskBasic.dwStartingSector);

        // Stuff the actual CHS totals into the absDiskPtr
        absDiskPtr->dwTotalHeads = tempAbsDisk.dwTotalHeads;
        absDiskPtr->dwTotalTracks = tempAbsDisk.dwTotalTracks;
        absDiskPtr->dwSectorsPerTrack = tempAbsDisk.dwSectorsPerTrack;
        }



					/* Compute number of sectors	*/
					/*	before this partition	*/
    part1 = (absDiskPtr->dwStartingHead *
             absDiskPtr->dwSectorsPerTrack) +
             (absDiskPtr->dwStartingSector - 1);

    diskPtr->sectorsBeforePartition = (
       (absDiskPtr->dwTotalHeads * absDiskPtr->dwSectorsPerTrack) *
             (DWORD)absDiskPtr->dwStartingTrack) + part1;

    if (!HWIsNEC())
        {
        if (!BPBhiddenSectors)
	    BPBhiddenSectors = diskPtr->sectorsBeforePartition;
        }

					/* Find last physical location 	*/
					/*	of disk			*/
    DiskMapLogToPhyParams ((DWORD)(diskPtr->totalSectors - 1),
    				 diskPtr->dl,
				 &int13Package);

    absDiskPtr->dwEndingHead   = int13Package.dwHead;
    absDiskPtr->dwEndingTrack  = int13Package.dwTrack;
    absDiskPtr->dwEndingSector = int13Package.dwSector;

					/* If abs. disk info from BIOS,	*/
					/*   must adjust total tracks,	*/
					/*   since BIOS gives tracks	*/
					/*   on entire disk, but we just*/
					/*   want the tracks in the 	*/
					/*   partition.			*/
    if (!validGenericIO)
        absDiskPtr->dwTotalTracks = absDiskPtr->dwEndingTrack - absDiskPtr->dwStartingTrack + 1;

    if(tempAbsDisk.bIsExtInt13 == ISINT13X_YES)
        {
        // Use the actual ending location, and actual totals in our temp struct
        // to find the linear ending sector
        dwEndingSector = ConvertPhySectorToLong(&tempAbsDisk, 
                                                absDiskPtr->dwEndingHead,
                                                absDiskPtr->dwEndingTrack,
                                                absDiskPtr->dwEndingSector);

        // Now convert the ending location to translated values
        ConvertLongToPhySector(&tempAbsDiskBasic, 
                               dwEndingSector, 
                               &tempAbsDiskBasic.dwEndingHead,
                               &tempAbsDiskBasic.dwEndingTrack,
                               &tempAbsDiskBasic.dwEndingSector);

        }
					/* Set 16-bit FAT flag		*/

    if (diskPtr->dwMaxClusterEx < (UINT)0x0FF6)
        {
        diskPtr->clEOF = FAT12VALUEMASK&~(DWORD)7;
        diskPtr->byFATType = DR_FAT12;
        }
    else if (diskPtr->dwMaxClusterEx < 0xFFFF)
        {
        diskPtr->clEOF = FAT16VALUEMASK&~(DWORD)7 ;
        diskPtr->byFATType = DR_FAT16;
        }
    else
        {
        diskPtr->clEOF = FAT32VALUEMASK&~(DWORD)7;
        diskPtr->byFATType = DR_FAT32;
        }
    diskPtr->clBAD = diskPtr->clEOF-1;

                    /*------------------------------*/
					/* We now need to set the 	*/
					/* bigFoot flag.  Base it on	*/
					/* the total sectors in the 	*/
					/* partition.  If there are 	*/
					/* more than 64K, then use 	*/
					/* the BigFoot interface.	*/
                    /*------------------------------*/

    if( CanSupportExtendedIO() )
        {
        diskPtr->byIOType = DR_IO_EXTENDED;
        }
    else
        {
        if (diskPtr->totalSectors <= 0x0000FFFFL)
            diskPtr->byIOType = DR_IO_NORMAL;
        else
            diskPtr->byIOType = DR_IO_BIGFOOT;
        }

    diskPtr->sectorsInRoot = (UINT)(diskPtr->dwStartOfDataEx - diskPtr->startOfRootDir);

    					/* Get bytes per cluster	*/
    diskPtr->bytesPerCluster = diskPtr->bytesPerSector * diskPtr->sectorsPerCluster;

					/* Fill in BPB from drive values */
    // The BPB values should be based on plain int13 info, so if our absDisk
    // contains actual values, then pass in our temp struct which has the
    // translated starting and ending locations
    if(tempAbsDisk.bIsExtInt13 == ISINT13X_YES)
        DiskFillInBPB (BPBhiddenSectors, BPBPtr, diskPtr, &tempAbsDiskBasic);
    else
        DiskFillInBPB (BPBhiddenSectors, BPBPtr, diskPtr, absDiskPtr);

    return (TRUE);
}

/*----------------------------------------------------------------------*/
/* DiskFillInBPB ()							*/
/*	Fill in the BPB from the values in the 'disk' record and the	*/
/*	values in the 'absDisk' record.					*/
/*									*/
/*	We only use 'absDisk' to get the 'sectorsPerTrack' and the	*/
/*	'heads'.							*/
/*----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI        DiskFillInBPB (DWORD		BPBhiddenSectors,
		    	       LPBPBREC		BPBPtr,
		    	       LPDISKREC	diskPtr,
			       LPABSDISKREC	absDiskPtr)
{
					/* Get Bytes per sector		*/
    BPBPtr->bytesPerSector = diskPtr->bytesPerSector;

    					/* Get sectors per cluster	*/
    BPBPtr->sectorsPerCluster = diskPtr->sectorsPerCluster;

    					/* Get first sector of Fat	*/
    BPBPtr->startOfFat = diskPtr->startOfFat;

    					/* Get number of Fats		*/
    BPBPtr->numberFats = diskPtr->numberFats;

    					/* Get maximum root entries	*/
    BPBPtr->maxRootEntries = diskPtr->maxRootEntries;

    					/* Get media descriptor		*/
    BPBPtr->mediaDescriptor = diskPtr->mediaDescriptor;

    					/* Get sectors per Fat		*/
    BPBPtr->sectorsPerFat = (UINT)diskPtr->dwSectorsPerFATEx;

    					/* Get sectors per track	*/
    BPBPtr->sectorsPerTrack = (WORD)absDiskPtr->dwSectorsPerTrack;

    					/* Get total heads		*/
    BPBPtr->heads = (WORD)absDiskPtr->dwTotalHeads;

    					/* Get hidden sectors		*/
    BPBPtr->hiddenSectors = BPBhiddenSectors;

    					/* Set total sectors		*/
    if (diskPtr->totalSectors > 0xFFFF)
        {
					/* MOS uses strange BPB format	*/
					/* if the disk is > 32M		*/
	if (DOSGetPCMOSVersion())
	    {
	    BPBPtr->oldTotalSectors = (UINT)(
	                        diskPtr->totalSectors & 0x0000FFFFL);
        BPBPtr->sectorsPerFat = (UINT)diskPtr->dwSectorsPerFATEx +
	    		(UINT)((diskPtr->totalSectors / 0x10000) << 8);
	    BPBPtr->totalSectors = 0L;
	    }
	else
	    {				/* BigFooted partition		*/
	    BPBPtr->totalSectors = diskPtr->totalSectors;
    	    BPBPtr->oldTotalSectors = 0;
	    }
	}
    else
    	{				/* Normal partition		*/
	BPBPtr->totalSectors = 0L;
	BPBPtr->oldTotalSectors = (UINT)(diskPtr->totalSectors);
	}
}


/*----------------------------------------------------------------------*/
/* FindPartitionTableEntry ()						*/
/*	Reads in the partition table and trys to find the partition 	*/
/*	that we will format.						*/
/*									*/
/* Returns TRUE if entry found, else FALSE				*/
/*----------------------------------------------------------------------*/

BOOL LOCAL PASCAL FindPartitionTableEntry (LPDWORD	partitionHiddenPtr,
				           LPABSDISKREC	absDiskPtr)
{
    register int	i;
    PartEntryRec far 	*partitionEntryPtr;
    PartEntryRec	partEntry;
    BOOL		returnValue = FALSE;
    BYTE		err;
    BYTE		systemIndicator;
    DWORD                startCylinder;
    DWORD                startSector;
    LPBYTE              lpBuffer;


					/* Read partition table in	*/
    if ((lpBuffer = MemAllocPtr(GHND, PHYSICAL_SECTOR_SIZE)) == NULL)
        return(returnValue);

    DiskGetPhysicalInfo(absDiskPtr);
    absDiskPtr->buffer = lpBuffer;
    absDiskPtr->dwHead = (DWORD)(absDiskPtr->dwTrack = 0);
    absDiskPtr->dwSector = 1;
    absDiskPtr->numSectors = 1;
    err = DiskAbsOperation (READ_COMMAND, absDiskPtr);
    if (err)
        {
    	DiskAbsOperation (RESET_COMMAND, absDiskPtr);
        goto done;
	}

    					/* Search the 4 entries in the	*/
					/*   table for our partition	*/
    partitionEntryPtr = (PartEntryRec far *)(lpBuffer + 0x01BE);
    for (i = 0; i < 4; i++, partitionEntryPtr++)
        {
					/* Get system indicator		*/
	systemIndicator = partitionEntryPtr->system;

					/* Skip over non-used entries	*/
					/*    and EXTENDED entries.	*/
					/* EXTENDED entries are picked	*/
					/*    up by the generic IO	*/
					/*    call, therefore, they are */
					/*    never searched for in the	*/
					/*    Partition Table.		*/
	if ((systemIndicator == SYSTEM_NOT_USED) ||
	     (systemIndicator == SYSTEM_EXTENDED))
	    continue;

					/* Convert from encoded to 	*/
					/* to unencoded			*/
	partEntry = *partitionEntryPtr;
    	DiskUnscrambleBIOSCoding (partEntry.startSector,
                                  &startCylinder, &startSector);


					/* Is this the partition?	*/
    if ((partitionEntryPtr->startHead == absDiskPtr->dwStartingHead) &&
         (startCylinder == absDiskPtr->dwStartingTrack) &&
         (startSector   == absDiskPtr->dwStartingSector))
	    {
	    				/* Return its hidden sectors	*/
	    *partitionHiddenPtr = partitionEntryPtr->relativeSectors;
	    returnValue = TRUE;
	    break;
	    }
	}

done:
    MemFreePtr(lpBuffer);
    return (returnValue);
}


/*----------------------------------------------------------------------*/
/* DiskIsValidBPB()							*/
/*	Takes a look at the BPB and returns TRUE if all fields within	*/
/*	it are valid, else FALSE.					*/
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskIsValidBPB (LPCBPBREC BPBPtr)
{
    UINT	bytesPerSector;
    UINT	maxRootEntries;
    UINT	dirEntriesPerSector;
    UINT	rootSectors;
    DWORD	totalSectors;
    DWORD   minimumSectors;
    BOOL    bIsFAT32;
    DWORD   dwSectorsPerFAT;

					/*------------------------------*/
    					/* Check bytes Per sector	*/
					/*------------------------------*/
    bytesPerSector = BPBPtr->bytesPerSector;
    if (bytesPerSector == 0)
	{
	return (FALSE);
        }
    else if (bytesPerSector < 128)
        {
	if (bytesPerSector & 63)
	    return (FALSE);
	}
    else if (bytesPerSector < 512)
        {
	if (bytesPerSector & 127)
	    return (FALSE);
	}
    else
    	{
	if (bytesPerSector & 511)
	    return (FALSE);
	}

					/*------------------------------*/
					/* SECTORS PER CLUSTER must	*/
					/* be power of 2		*/
					/*------------------------------*/
    if (!MathIsPower2 (BPBPtr->sectorsPerCluster))
        return (FALSE);

					/*------------------------------*/
					/* Check NUMBER OF FATS		*/
					/*------------------------------*/
    if (BPBPtr->numberFats > 4)
        return (FALSE);

					/*------------------------------*/
					/* Check START OF FAT		*/
					/*------------------------------*/
    if (BPBPtr->startOfFat > 32)
        return (FALSE);

					/*------------------------------*/
                    /* Check MAX ROOT ENTRIES       */
                    /*                              */
                    /* only check on non-FAT32 BPBs */
                    /* A FAT32 BPB is signaled by 0 */
                    /* in the SectorsPerFAT field . */
                    /*------------------------------*/


    bIsFAT32 = (BPBPtr->sectorsPerFat == 0) ? TRUE : FALSE;

    if ( !bIsFAT32 )
        {
        maxRootEntries = BPBPtr->maxRootEntries;
        dirEntriesPerSector = BPBPtr->bytesPerSector / sizeof(DIRENTRYREC);
        rootSectors = maxRootEntries / dirEntriesPerSector;
        if (maxRootEntries % dirEntriesPerSector)
            rootSectors++;

        if (maxRootEntries & 3)
            return (FALSE);

                    /*------------------------------*/
                    /* root entries must be evenly  */
                    /* divisible by bytesPerSector  */
                    /*------------------------------*/
        if ((maxRootEntries * 32L) % bytesPerSector)
            return (FALSE);

					/*------------------------------*/
					/* Check SECTORS PER FAT	*/
					/* Maximum values is 256	*/
					/*------------------------------*/
        if (BPBPtr->sectorsPerFat > 300)
            return (FALSE);

        dwSectorsPerFAT = BPBPtr->sectorsPerFat;
        }
    else
        {
                    // Assume at least 1 cluster
                    // for root directory
        rootSectors = BPBPtr->sectorsPerCluster;
        }


					/*------------------------------*/
					/* Make sure the MEDIA		*/
					/* DESCRIPTOR BYTE is 0xF0 or	*/
					/* greater.			*/
					/*------------------------------*/
    if ((BPBPtr->mediaDescriptor & 0xF0) != 0xF0)
        return (FALSE);

					/*------------------------------*/
					/* Make sure all of these fields*/
					/* have values.			*/
					/*------------------------------*/

    if(bIsFAT32)
        {
        auto    LPFAT32BPBREC lpFAT32;

        lpFAT32 = (LPFAT32BPBREC) BPBPtr;

        if( !lpFAT32->sectorsPerCluster ||
            !lpFAT32->numberFats        ||
            !lpFAT32->sectorsPerTrack   ||
            !lpFAT32->heads             ||
            !lpFAT32->dwBigSectorsPerFAT||
            !lpFAT32->dwRootSCN )
            return (FALSE);

        dwSectorsPerFAT = lpFAT32->dwBigSectorsPerFAT;
        }
    else
        {
        if (!maxRootEntries            ||
            !BPBPtr->sectorsPerCluster ||
            !BPBPtr->numberFats        ||
            !BPBPtr->sectorsPerTrack   ||
            !BPBPtr->heads             ||
            !BPBPtr->sectorsPerFat    )
            return (FALSE);
        }
                    /*------------------------------*/
					/* Make sure total sectors is	*/
					/* greater than system area plus*/
					/* 5 clusters.			*/
					/*------------------------------*/
    minimumSectors = BPBPtr->startOfFat +
             (dwSectorsPerFAT * BPBPtr->numberFats) +
             rootSectors + (BPBPtr->sectorsPerCluster * 5);
    totalSectors = ((BPBPtr->oldTotalSectors) ?
            (DWORD) BPBPtr->oldTotalSectors : BPBPtr->totalSectors);
    if (((DWORD) minimumSectors) > totalSectors)
        return(FALSE);

    return (TRUE);			/* Passed all tests. Valid	*/
}

/*----------------------------------------------------------------------*/
/*	Calculates the physical sectors represented by a BPB.		*/
/*----------------------------------------------------------------------*/

DWORD SYM_EXPORT WINAPI DiskCalcPhysicalSectors(LPCBPBREC BPBPtr)
{
    auto	DWORD		totalSectors;

    totalSectors = ((BPBPtr->oldTotalSectors) ?
			(DWORD)BPBPtr->oldTotalSectors : BPBPtr->totalSectors);
    totalSectors *= (BPBPtr->bytesPerSector / PHYSICAL_SECTOR_SIZE);

    return(totalSectors);
}
