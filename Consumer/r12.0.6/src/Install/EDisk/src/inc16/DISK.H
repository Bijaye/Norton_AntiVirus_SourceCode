/* $Header:   V:/INCLUDE/VCS/DISK.H_V   1.16   05 Aug 1993 18:55:16   JOHN  $ */

/*----------------------------------------------------------------------*
 * DOS disk data structures					 DISK.H	*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   V:/INCLUDE/VCS/DISK.H_V  $
 * 
 *    Rev 1.16   05 Aug 1993 18:55:16   JOHN
 * Fixed values for IOCTL_288M and IOCTL_OTHER_DOS5.  They were reversed.
 * With any luck, this will finally fix all the problems we've had with
 * 2.88M drive type detection.
 * 
 *    Rev 1.15   01 Feb 1993 21:28:50   JOHN
 * Added IOCTL_xxx constants for DiskIOCTL()
 * 
 *    Rev 1.14   27 Oct 1992 15:22:36   JOHN
 * Added SYSTEM_OS2_BOOT_MGR partition type
 * 
 *    Rev 1.13   01 Sep 1992 19:08:40   JOHN
 * Removed constants related to DMA buffer
 * 
 *    Rev 1.12   30 Aug 1992 20:50:40   JOHN
 * Increased MAX_SECTORS_PER_TACK to 64
 * 
 *    Rev 1.11   13 Aug 1992 18:21:40   JOHN
 * Added constants related to DMA buffer (DiskGetDMABuffer() function).
 * 
 *    Rev 1.10   14 Feb 1992 10:28:14   PETERD
 * Added TYPE_RAM and TYPE_REMOVABLE.
 * 
 *    Rev 1.9   27 Jan 1992 22:36:08   TORSTEN
 * Changed the #include statements to use angle brackets.
 * 
 *    Rev 1.8   17 Dec 1991 11:56:50   ED
 * Added TYPE_UNCHECKED
 * 
 *    Rev 1.7   02 Dec 1991 17:05:46   JOHN
 * Added DRIVE_TYPE_BUFFER_SIZE which is used internally by some library
 * code and isn't of much concern to applications.
 * 
 *    Rev 1.6   26 Nov 1991 09:37:44   TORSTEN
 * Added TYPE_CDROM.
 * 
 *    Rev 1.5   13 Nov 1991 07:41:40   ED
 * Added TRAP_DISK_ERRORS
 * 
 *    Rev 1.4   31 Oct 1991 13:06:04   JOHN
 * Fixed 2.88M floppy support.  We had the wrong CMOS drive type,
 * and lacked the new drive types returned by IOCTL in DOS 5.0.
 * 
 *    Rev 1.3   28 Aug 1991 20:32:06   ED
 * Added the IsTypeFloppy() macro
 * 
 *    Rev 1.2   24 Jul 1991 15:47:22   ED
 * Repeated the last change because my changes were uselessly
 * destroyed in a library move.  [another perspective is that
 * your original changes were uselessly made in the wrong place]
 * 
 *    Rev 1.2   23 Jul 1991 10:36:52   ED
 * Added lots of IOCTL stuff and typedefs to allow Othello code
 * to port much more easily.
 * 
 *    Rev 1.1   15 Mar 1991 14:06:58   ED
 * 
 *    Rev 1.0   29 Aug 1990 11:24:30   DAVID
 * Initial revision.
 */

#ifndef INCL_DISK
#define INCL_DISK

#ifndef INCL_TYPES
#include <types.h>
#endif

/*----------------------------------------------------------------------*
 * Used with DiskErrorOn()						*
 *----------------------------------------------------------------------*/

#define	TRAP_DISK_ERRORS	((void (*)())-1)

/*----------------------------------------------------------------------*
 * Partition Table System IDs						*
 *----------------------------------------------------------------------*/

#define		SYSTEM_NOT_USED		0
#define		SYSTEM_DOS_12		1	/* DOS, 12-bit Fat	*/
#define		SYSTEM_XENIX1		2	/* XENIX		*/
#define		SYSTEM_XENIX2		3	/* XENIX		*/
#define		SYSTEM_DOS_16		4	/* DOS, 16-bit Fat	*/
#define		SYSTEM_EXTENDED		5	/* DOS, Extended	*/
#define		SYSTEM_BIGDOS		6	/* DOS, 32Meg+ Partition*/
#define		SYSTEM_HPFS		7	/* HPFS partition	*/
#define		SYSTEM_OS2_BOOT_MGR	0x0A	/* OS/2 boot manager	*/
#define		SYSTEM_DM_RO		0x50	/* Disk Manager R/O	*/
#define		SYSTEM_DM_RW		0x51	/* Disk Manager R/W	*/
#define		SYSTEM_GOLDEN_BOW	0x56	/* Golden Bow partition	*/
#define		SYSTEM_SS_UNKNOWN1	0x61	/* SpeedStor partition	*/
#define		SYSTEM_386IX		0x63	/* Interactive's 386/IX	*/
#define		SYSTEM_NETWARE_286	0x64	/* Netware 286		*/
#define		SYSTEM_NETWARE_386	0x65	/* Netware 386		*/
#define		SYSTEM_PCIX		0x75	/* PCIX			*/
#define		SYSTEM_EVEREX_SUPERDOS	0xAA	/* Everex Super DOS	*/
						/* partitions upto 285M	*/
#define		SYSTEM_CPM		0xDB	/* CPM			*/
#define		SYSTEM_SS_12BIT		0xE1	/* SpeedStor 12-bit	*/
#define		SYSTEM_SS_UNKNOWN2	0xE3	/* SpeedStor partition	*/
#define		SYSTEM_SS_16BIT		0xE4	/* SpeedStor 16-bit	*/
#define		SYSTEM_SS_EXTENSION	0xF1	/* SpeedStor Extension	*/
#define		SYSTEM_SS_LARGE		0xF4	/* SpeedStor LARGE 	*/
#define		SYSTEM_BBT		0xFF	/* Bad Block Table(Unix)*/

/* For compatibility with code using old name */
#define		SYSTEM_NOVELL		SYSTEM_NETWARE_286

/*----------------------------------------------------------------------*
 * Disk Access Flags							*
 *	Used for allowing/denying access to a DOS disk using DOS 4.0+	*
 *	services.  Has to do with "Invalid Media Type"			*
 *----------------------------------------------------------------------*/

#define		DISALLOW_ACCESS		0
#define		ALLOW_ACCESS		1

/*----------------------------------------------------------------------*
 * DOS 4.0 Extended Boot Record signature				*
 *----------------------------------------------------------------------*/

#define		EXTENDED_BOOT_RECORD_SIGNATURE		0x29

/*----------------------------------------------------------------------*
 * First Physical Hard Disk Number.					*
 *----------------------------------------------------------------------*/

#define	FIRST_HD	0x80		/* 1st Hard Disk on system	*/

/*----------------------------------------------------------------------*
 * Maximum Values for Physical Characteristics				*
 *----------------------------------------------------------------------*/

#define	MAX_CYLINDERS	1024

/*----------------------------------------------------------------------*
 * Size of buffer allocated by DiskGetDriveType()			*
 *----------------------------------------------------------------------*/

#define DRIVE_TYPE_BUFFER_SIZE	2048

/*----------------------------------------------------------------------*
 * Different types of known disk drives					*
 *----------------------------------------------------------------------*/

#define		TYPE_PHYSICAL	0xFF
#define         TYPE_UNCHECKED  0xFE            // used in DISKTYPE.C
#define 	TYPE_UNKNOWN	0
#define 	TYPE_360K	1
#define		TYPE_720K	2
#define 	TYPE_12M	3
#define 	TYPE_144M	4
#define		TYPE_288M	5  // Note: TYPE_288M != CMOS_288M
#define		TYPE_HD		10
#define		TYPE_NETWORK	11
#define		TYPE_ASSIGNED	12
#define		TYPE_SUBST	13
#define		TYPE_PHANTOM	14
#define		TYPE_LOGICAL	15
#define		TYPE_ENCRYPTED	16
#define         TYPE_CDROM      17
#define         TYPE_RAM        18
#define         TYPE_REMOVABLE  19 // Removeable drive

#define	IsTypeFloppy(d)		(((d) == TYPE_360K) || \
				 ((d) == TYPE_720K) || \
				 ((d) == TYPE_12M)  || \
				 ((d) == TYPE_144M) || \
				 ((d) == TYPE_288M))

/* Indices used to index into BPBTables	*/
#define		BPB_INDEX_160K		(Byte) 1
#define		BPB_INDEX_180K		(Byte) 2
#define		BPB_INDEX_320K		(Byte) 3
#define		BPB_INDEX_360K		(Byte) 4
#define		BPB_INDEX_720K		(Byte) 5
#define		BPB_INDEX_12M		(Byte) 6
#define		BPB_INDEX_144M		(Byte) 7
#define		BPB_INDEX_288M		(Byte) 8

/* Different CMOS diskette types	*/
#define		CMOS_360K	1
#define		CMOS_12M	2
#define		CMOS_720K	3
#define		CMOS_144M	4
#define		CMOS_288M	6
/* NOTE: IBM says CMOS type 5 was used on some Japanese machines */

/*----------------------------------------------------------------------*
 * DiskIOCTL() "byCommand" parameter					*
 *----------------------------------------------------------------------*/

#define		IOCTL_CHAR_READ		0x02	/* Read from char dev	*/
#define		IOCTL_CHAR_WRITE	0x03	/* Write to char dev	*/
#define		IOCTL_BLOCK_READ	0x04	/* Read from block dev	*/
#define		IOCTL_BLOCK_WRITE	0x05	/* Write to block dev	*/

/*----------------------------------------------------------------------*
 * Generic IOCTL constants						*
 *----------------------------------------------------------------------*/
                                        /* IOCTL commands               */
#define         IOCTL_SET_PARAMS        0x40    /* DOS 3.2+             */
#define         IOCTL_GET_PARAMS        0x60    /* DOS 3.2+             */
#define         IOCTL_WRITE_TRACK       0x41    /* DOS 3.2+             */
#define         IOCTL_READ_TRACK        0x61    /* DOS 3.2+             */
#define         IOCTL_FORMAT_TRACK      0x42    /* DOS 3.2+             */
#define         IOCTL_VERIFY_TRACK      0x62    /* DOS 3.2+             */
#define         IOCTL_SET_MEDIA_ID      0x46    /* DOS 4.x+             */
#define         IOCTL_GET_MEDIA_ID      0x66    /* DOS 4.x+             */
#define         IOCTL_SET_ACCESS_FLAG   0x47    /* DOS 4.x+             */
#define         IOCTL_GET_ACCESS_FLAG   0x67    /* DOS 4.x+             */

                                        /* Old names                    */
#define         SET_DEVICE_PARAMS       IOCTL_SET_PARAMS
#define         GET_DEVICE_PARAMS       IOCTL_GET_PARAMS
#define         WRITE_TRACK             IOCTL_WRITE_TRACK
#define         READ_TRACK              IOCTL_READ_TRACK
#define         FORMAT_TRACK            IOCTL_FORMAT_TRACK
#define         VERIFY_TRACK            IOCTL_VERIFY_TRACK

/*----------------------------------------------------------------------*
 * IOCTL drive types                                                    *
 *----------------------------------------------------------------------*/

#define         IOCTL_360K              0
#define         IOCTL_12M               1
#define         IOCTL_720K              2
#define         IOCTL_8_INCH_SINGLE     3
#define         IOCTL_8_INCH_DOUBLE     4
#define         IOCTL_HD                5
#define         IOCTL_TAPE_DRIVE        6
#define		IOCTL_OTHER_DOS4	7 /* Returned by DOS <= 4.0 */
#define		IOCTL_144M		7 /* Returned by DOS >= 5.0 */
#define		IOCTL_288M		9 /* Returned by DOS >= 5.0 */
#define		IOCTL_OTHER_DOS5	8 /* Returned by DOS >= 5.0 */

// NOTE: I know it looks backwards, but IOCTL_288M really is
//	 greater than IOCTL_OTHER_DOS5.  Thanks Microsoft!

                                        /* Special Function commands    */
                                        /* for GET DEVICE PARAMS        */
#define         IOCTL_SF_GET_DEFAULT    0x00
#define         IOCTL_SF_GET_CURRENT    0x01
                                        /* Special Function commands    */
                                        /* for SET DEVICE PARAMS        */
#define         IOCTL_SF_SET_DEFAULT    0x00
#define         IOCTL_SF_SET_CURRENT    0x01

#define         IOCTL_SF_USE_ALL_PARAMS 0x00
#define         IOCTL_SF_USE_TRACK_ONLY 0x02

#define         IOCTL_SF_DIFFERENT_SIZE 0x00
#define         IOCTL_SF_SAME_SIZE      0x04
                                        /* Special Function commands    */
                                        /* for FORMATPACKET             */
#define         IOCTL_SF_FORMAT         0x00
#define         IOCTL_SF_IS_FORMAT_OK   0x01
                                        /* On return from call          */
#define         IOCTL_SF_FORMAT_OK      0x00
#define         IOCTL_SF_FORMAT_NOT_OK  0x01

#define         IOCTL_SF_COMBO_OK       0x00
#define         IOCTL_SF_COMBO_NOT_OK   0x02

/*----------------------------------------------------------------------*
 * INT 25h/INT 26h constants						*
 *----------------------------------------------------------------------*/

#define	BLOCK_READ	0x25
#define	BLOCK_WRITE	0x26

/*----------------------------------------------------------------------*
 * INT 13h commands							*
 *----------------------------------------------------------------------*/

#define RESET_COMMAND		0	/* Reset disk system		*/
#define	READ_COMMAND		2	/* Read sectors			*/
#define	WRITE_COMMAND		3	/* Write sectors		*/
#define	VERIFY_COMMAND		4	/* Verify sectors		*/
#define	FORMAT_COMMAND		5	/* Format track			*/
#define INITIALIZE_COMMAND	0x09	/* Initialize controller	*/
#define READ_LONG_COMMAND	0x0A	/* Read long			*/
#define RECALIBRATE_COMMAND	0x11	/* Recalibrate drive		*/

/*----------------------------------------------------------------------*
 * INT 13h errors							*
 *----------------------------------------------------------------------*/

#define		ERROR_NO_ERROR		((Byte) 0x00)
#define		ERROR_INVALID_FUNCTION	((Byte) 0x01)
#define		ERROR_ADDR_MARK_MISSING	((Byte) 0x02)	
#define		ERROR_WRITE_PROTECTED	((Byte) 0x03)
#define		ERROR_SECTOR_NOT_FOUND	((Byte) 0x04)
#define		ERROR_LINE_ACTIVE	((Byte) 0x06)
#define		ERROR_DMA_OVERRUN	((Byte) 0x08)
#define		ERROR_64K_DMA		((Byte) 0x09)
#define		ERROR_BAD_SECTOR	((Byte) 0x0A)
#define		ERROR_BAD_CYLINDER	((Byte) 0x0B)
#define		ERROR_INVALID_MEDIA	((Byte) 0x0C)
#define		ERROR_CRC_ERROR		((Byte) 0x10)
#define 	ERROR_ECC_ERROR		((Byte) 0x11)
#define		ERROR_CONTROLLER_FAILED ((Byte) 0x20)
#define		ERROR_SEEK_FAILED	((Byte) 0x40)
#define		ERROR_TIME_OUT		((Byte) 0x80)
#define		UNCORRECTABLE_ERRORS	((Byte) 0xA1)

/* INT 25 and 26 error codes ( DosSectorRead & DosSectorWrite ) */
#define		WRITE_PROTECTED		0x0300
#define	  	TIME_OUT		0x8000

/* Used in the NEW DiskGetInfo() routine for reporting return state */
#define		VALID_DISK		0
#define		MEMORY_ERROR		1
#define		CANT_READ_BOOT_ERROR	2
#define		INVALID_DISK_ERROR	3

#define	MAX_DOS_DRIVES		26	/* Maximum number of DOS drives	*/

#define MAX_SECTORS_PER_TRACK	64	/* Max phys sectors per track	*/

#define	PHYSICAL_SECTOR_SIZE	512	/* Bytes per physical sector 	*/

#define	MAX_ENTRIES_PER_TABLE	4	/* 4 entries per table		*/

#define	PART_TABLE_OFFSET	0x01BE	/* Start of Partition Table 	*/

#define PART_ENTRY_SIZE		16	/* Size of 1 partition table entry */

#define	SIGNATURE_OFFSET	0x01FE	/* Offset of Signature word	*/

					/* Signature in Partition table	*/
					/*	and Boot Record		*/
#define	SIGNATURE		0xAA55

/*----------------------------------------------------------------------*/
/* DASDRecord								*/
/*	Given a specific physical type (TYPE_360K, TYPE_720K, etc.)	*/
/*		and the number of tracks, gets the DASD type to be	*/
/*		used to set the DASD type using INT 13h, function 17h	*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte	driveType;	/* TYPE_360K, TYPE_720K, etc.	*/
    Word	tracks;		/* Number of tracks		*/
    Byte	DASD_Type;	/* DASD id for this type	*/
    } DASDRec;
typedef DASDRec	DASDRecord;

/*----------------------------------------------------------------------*/
/* BPBRec								*/
/* 	Record containing disk BPB information.				*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Word	bytesPerSector; 	/* Bytes per sector		*/
    Byte	sectorsPerCluster; 	/* Sectors per cluster		*/
    Word	startOfFat;		/* First sector of Fat		*/
    Byte	numberFats;		/* Copies of Fat		*/
    Word	maxRootEntries;		/* Maximum entries in Root dir.	*/
    Word	oldTotalSectors;	/* Old total number of sectors  */
    Byte	mediaDescriptor;	/* Media descriptor byte	*/
    Word	sectorsPerFat;  	/* Sectors per Fat		*/
    Word	sectorsPerTrack;	/* Sectors per Track		*/
    Word	heads;			/* Number of sides		*/
    Ulong	hiddenSectors;  	/* Number of hidden sectors	*/
    Ulong	totalSectors;		/* Total sectors for BigFoot	*/
	 				/*	drives			*/
    Byte	reserved [6];		/* Reserved			*/
    } BPBRec;

typedef BPBRec	BPBRecord;

typedef	BPBRec	     BPBREC;
typedef	BPBRec FAR   *LPBPBREC;
typedef	BPBRec NEAR  *NPBPBREC;

/*----------------------------------------------------------------------*/
/* Record containing Extended BPB information found in the Boot Record 	*/
/*	of the disk. disk. Use INT 25h to get this information.		*/
/* NOTE: This is the EXTENDED version of the BPB.  It is only found in	*/
/*	DOS 4.0+							*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Word	bytesPerSector; 	/* Bytes per sector		*/
    Byte	sectorsPerCluster; 	/* Sectors per cluster		*/
    Word	startOfFat;		/* First sector of Fat		*/
    Byte	numberFats;		/* Copies of Fat		*/
    Word	maxRootEntries;		/* Maximum entries in Root dir.	*/
    Word	oldTotalSectors;	/* Old total number of sectors  */
    Byte	mediaDescriptor;	/* Media descriptor byte	*/
    Word	sectorsPerFat;  	/* Sectors per Fat		*/
    Word	sectorsPerTrack;	/* Sectors per Track		*/
    Word	heads;			/* Number of sides		*/
    Ulong	hiddenSectors;  	/* Number of hidden sectors	*/
    Ulong	totalSectors;		/* Total sectors for BigFoot	*/
					/*	drives			*/
    Byte	physicalNumber;		/* Physical drive number	*/
    Byte	reserved1 [1];		/* Disk head where BPB is found */
    Byte	extendedSignature;	/* Should be 0x29		*/
    Ulong	serialNumber;		/* Date/time of formatting	*/
    Byte	volumeLabel [11];	/* Volume Label			*/
    Byte	reserved2 [8];		/* File Type (HPFS, DOS 12)	*/
    } ExtendedBPBRec;
typedef ExtendedBPBRec	ExtendedBPBRecord;

/*----------------------------------------------------------------------*/
/* Record containing BPB information found in the Boot Record of the	*/
/*	disk. Use INT 25h to get this information.			*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte	jmp [3];		/* Jump over BPB		*/
    Byte	oemName [8];		/* OEM Signature		*/
    BPBRec	BPB;			/* BPB				*/
    } BPBBootRec;
typedef BPBBootRec   BPBBootRecord;

/*----------------------------------------------------------------------*/
/* DiskRec								*/
/* 	Record containing all information needed to read/write from/to 	*/
/*	a disk using DOS absolute sectors (INT 25h and INT 26h).	*/
/*	Includes support for BigFoot and DOS 3.4			*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte 	dn;			/* Drive number	(0-A:,1-B:,etc)	*/
    Byte 	driverEntry;		/* Entry within device driver	*/
    Word	bytesPerSector; 	/* Bytes per Sector		*/
    Byte	sectorsPerCluster; 	/* Sectors per Cluster		*/
    Byte	reserved;		/* Reserved			*/
    Word	startOfFat;		/* First sector of Fat		*/
    Byte	numberFats;		/* Copies of Fat		*/
    Word	maxRootEntries; 	/* Maximum root dir. entries	*/
    Word	startOfData;		/* First sector of Data area	*/
    Word	maxCluster;		/* Largest Cluster number	*/
    Word	sectorsPerFat; 		/* Sectors Per Fat		*/
    Word	startOfRootDir;		/* First sector of Root dir.	*/

    Boolean	isBigFooted;		/* Is drive BigFooted?		*/
    Boolean	is16BitFat;		/* Is 16-bit Fat? (or 12-bit?)	*/
    Byte	dl;			/* Drive Letter (A:,B:,etc.)	*/
    Byte	mediaDescriptor;	/* Media descriptor byte	*/
    Word	sectorsInRoot;		/* Sector in root directory	*/
    Ulong	totalSectors;		/* Total sectors on disk (not	*/
    					/*    filled in by LoadDiskInfo)*/
    Ulong	sectorsBeforePartition;	/* Total sectors before this 	*/
    					/*	partition		*/
    Word	bytesPerCluster; 	/* Bytes per cluster: Same as:	*/
    					/* BytesPerSector * 		*/
					/*	SectorsPerCluster 	*/
    Ulong	sector;			/* First sector to read/write	*/
    Word	sectorCount;		/* Number of sectors to r/w	*/
    void far	*buffer;		/* Buffer where reads/writes	*/
					/* 	will occur		*/
    } DiskRec;

typedef DiskRec	DiskRecord;

typedef	DiskRec	     DISKREC;
typedef	DiskRec	FAR  *LPDISKREC;
typedef	DiskRec	NEAR *NPDISKREC;

/*----------------------------------------------------------------------*/
/* ShortDiskRec								*/
/*	First 19 bytes of the DiskRecord defined above.  Used when 	*/
/*	moving information from internal DOS tables.			*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte 	driveNumber;	/* Drive number	(0-A:,1-B:,etc)	*/
    Byte 	driverEntry;	/* Entry within device driver	*/
    Word	bytesPerSector; /* Bytes per Sector		*/
    Byte	sectorsPerCluster; /* Sectors per Cluster	*/
    Byte	reserved;	/* Reserved			*/
    Word	startOfFat;	/* First sector of Fat		*/
    Byte	numberFats;	/* Copies of Fat		*/
    Word	maxRootEntries; /* Maximum root dir. entries	*/
    Word	startOfData;	/* First sector of Data area	*/
    Word	maxCluster;	/* Largest Cluster number	*/
    Word	sectorsPerFat; 	/* Sectors Per Fat		*/
    Word	startOfRootDir;	/* First sector of Root dir.	*/
    } ShortDiskRec;

typedef ShortDiskRec ShortDiskRecord;

typedef	ShortDiskRec FAR  *LPSHORTDISKREC;
typedef	ShortDiskRec NEAR *NPSHORTDISKREC;

/*----------------------------------------------------------------------*/
/* AbsDiskRec								*/
/*	Contains all the information you need to manipulate absolute	*/
/*	physical sectors of a disk					*/
/*	Kinda like DiskRec						*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte	dn;			/* Drive number (0,1,0x80,0x81)	*/

/* starting coordinates	*/
    Byte	startingHead;		/* First head (0-based)		*/
    Word	startingTrack;		/* First track (0-based)	*/
    Byte	startingSector; 	/* First sector (1-based)	*/

/* ending coordinates	*/    
    Byte	endingHead;		/* Ending head # (0-based)	*/
    Word	endingTrack;		/* Ending track # (0-based)	*/
    Byte	endingSector;		/* Ending sector # (1-based)	*/
    
    Byte	sectorsPerTrack; 	/* Sectors on each track	*/
    Word	totalTracks;	 	/* Total tracks on disk		*/
    Byte	totalHeads;	 	/* Total heads on disk		*/
    
/* current coordinates	*/    
    Byte	head;			/* Current head (0-based)	*/
    Word	track;			/* Current track (0-based)	*/
    Byte	sector;			/* Current sector (1-based)	*/

    Byte	numSectors;		/* Number of sectors to use	*/
    void far	*buffer;		/* Ptr. to buffer to use	*/
    Byte	sectorsProcessed;	/* Number of sectors processed*/
    Byte	CMOS_Type;		/* CMOS drive type		*/
    					/* (Floppies only)		*/
    					/*   1 - 360K			*/
					/*   2 - 1.2M			*/
					/*   3 - 720K			*/
					/*   4 - 1.44M			*/
    Byte	numDrives;		/* Num. drives (floppy or hard) */
    } AbsDiskRec;

typedef AbsDiskRec AbsDiskRecord;

typedef	AbsDiskRec FAR 	*LPABSDISKREC;
typedef	AbsDiskRec NEAR *NPABSDISKREC;

/*----------------------------------------------------------------------*/
/* DisketteParamRec							*/
/*	Diskette Parameter Table record					*/
/*	This record is used by the PC to format floppy diskettes	*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte	firstSpecifyByte;	/* ???????????		*/
    Byte	secondSpecifyByte;	/* ???????????		*/
    Byte	ticksTillTurnMotorOff;	/* Ticks till diskette	*/
    						/*   motor turns off	*/
    Byte	bytesPerSector;		/* Bytes/Sector (coded)	*/
    						/* 0 - 128 bytes/sector */
						/* 1 - 256 bytes/sector */
						/* 2 - 512 bytes/sector */
						/* 3 - 1024 bytes/sector*/
    Byte	sectorsPerTrack;	/* Sectors Per Track	*/
    Byte	gapLength;		/* Gap length		*/
    Byte	dataLength;		/* Dtl (Data Length)	*/
    Byte	formatGapLength;	/* Gap Length for Format*/
    Byte	formatFillByte;		/* Fill Byte for Format	*/
    Byte	headSettleTime;		/* Head Settle Time (millisec)*/
    Byte	motorStartupTime;	/* in 1/8 seconds	*/
    } DisketteParamRec;
typedef DisketteParamRec DisketteParamRecord;

/*----------------------------------------------------------------------*/
/* DisketteAddressRec							*/
/*	Floppy diskette address record.  Each record represents the 	*/
/*      the address that is layed down for each sector on a floppy disk	*/
/*----------------------------------------------------------------------*/
typedef struct
    {
    Byte	track;			/* Track number (0-based)	*/
    Byte	head;			/* Head number (0-based)	*/
    Byte	sector;			/* Sector number (1-based)	*/
    Byte	bytesPerSector;		/* Bytes per sector		*/
    					/*    0 - 128 bytes/sector	*/
					/*    1 - 256 bytes/sector	*/
					/*    2 - 512 bytes/sector	*/
					/*    3 - 1024 bytes/sector	*/
    } DisketteAddressRec;

/*----------------------------------------------------------------------*/
/* FormatAddressRec							*/
/*	One record in the AT format table.  This table informs the	*/
/*	controller of how a track should be formatted.  It includes	*/
/*	enough information so the track will be formatted with a	*/
/*	specific interleave.						*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte	mode;			/* 00h - Good sector; 80h - Bad	*/
    Byte	sector;			/* Sector number; can use to	*/
    					/*   change interleave		*/
    } FormatAddressRec;

/*----------------------------------------------------------------------*/
/* Int13Rec								*/
/*	Contains many fields that will be filled in if an INT 13h is	*/
/*	generated when performing a DOS INT 25h call.  Used to map a	*/
/*	logical device to its physical attributes.			*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Boolean	generated;	/* Was it generated?		*/
    Byte	dn;		/* Physical drive #		*/
    Byte	head;		/* Starting head # (0-based)	*/
    Word	track;		/* Starting track # (0-based)	*/
    Byte	sector;		/* Starting sector # (1-based)	*/
    } Int13Rec;
typedef Int13Rec Int13Record;

/*----------------------------------------------------------------------*/
/* DeviceParamRec							*/
/*	Device information returned by DOS using Generic IOCTL command.	*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte	specialFunctions;
    Byte	deviceType;
    Word	deviceAttributes;
    Word	numberOfTracks;
    Byte	mediaType;
    BPBRec	BPB;
    Word	sectorsPerTrack;
    } DeviceParamRec;
typedef DeviceParamRec DeviceParamRecord;

/*----------------------------------------------------------------------*/
/* READWRITETRACKPACKET                                                 */
/*      Read/write track packet, used to read or write a track of       */
/*      info using IOCTL.                                               */
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte        bSpecialFunctions;
    Word        wHead;
    Word        wCylinder;
    Word        wFirstSector;
    Word        wNumberOfSectors;
    Byte huge   *lpTransferAddress;
    } READWRITETRACKPACKET;

/*----------------------------------------------------------------------*/
/* FORMATPACKET                                                         */
/*      Packet used to format a track using IOCTL.                      */
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte        bSpecialFunctions;
    Word        wHead;
    Word        wCylinder;
    } FORMATPACKET;

/*----------------------------------------------------------------------*/
/* SECTORPACKET                                                         */
/*      Packet used to format a track using IOCTL.  This structure      */
/*      helps us set up our track layout.                               */
/*----------------------------------------------------------------------*/

typedef struct
    {
    Word        wSectorNumber;
    Word        wSectorSize;
    } SECTORPACKET;

/*----------------------------------------------------------------------*/
/* PartEntryRec								*/
/*	Partition Table entry record.					*/
/*	Used to be called 'part_struct'					*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Byte 	bootable;
    Byte 	startHead;
    Byte 	startSector;
    Byte 	startCylinder;
    Byte 	system;
    Byte 	endHead;
    Byte 	endSector;
    Byte 	endCylinder;
    Ulong	relativeSectors;	/* Sectors preceding partition 	*/
    Ulong	numSectors;	    	/* Sectors in partition 	*/
    } PartEntryRec;
typedef PartEntryRec PartitionRec;
typedef PartEntryRec PartitionRecord;

/*----------------------------------------------------------------------*/
/* HDParamTable								*/
/*	Hard Disk Drive Parameter Table.  This is the table pointed to	*/
/*	by Int 41h (1st drive) and Int 46h (2nd drive).			*/
/*									*/
/*	See Pg. 3-16 of PS/2 BIOS Tech. Reference for table definition	*/
/*----------------------------------------------------------------------*/

typedef struct
    {
    Word	maxCylinders;		/* Maximum number of cylinders	*/
    Byte	maxHeads;		/* Maximum number of heads	*/
    Word	reducedWriteCylinder;	/* Reduced Write Cylinder (XT)	*/
    Word	writePrecompCylinder;	/* Write Precompensation Cyl.	*/
    Byte	maxECCBurstLength;	/* Max ECC Data Burst Length(XT)*/
    Byte	controlByte;		/* Control Byte			*/
    Byte	standardTimeout;	/* Standard Time-out Value (XT)	*/
    Byte	formatTimeout;		/* Time-out for Format Drive(XT)*/
    Byte	checkTimeout;		/* Time-out for Check Drive (XT)*/
    Word	landingZone;		/* Landing Zone	(AT)		*/
    Byte	sectorsPerTrack;	/* Sectors Per Track (AT)	*/
    Byte	reserved;		/* Unused			*/
    } HDParamTable;

/*--------------------------------------------------------------*
 * This record is used for storing partition IDs and names 	*
 *--------------------------------------------------------------*/

typedef struct
    {
    Byte id;
    Byte *string;
    } SystemIDRec;

#endif
