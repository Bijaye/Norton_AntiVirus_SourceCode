// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.     */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/disk.h_v   1.43   06 Oct 1998 22:42:24   RANSCHU  $ *
 *                                                                      *
 * Description:                                                         *
 *      Header file for all disk, fat, and low-level directory          *
 *      functions.  Old Symantec Library prototypes can be excluded     *
 *      by defining NOOLDSYMANTEC.                                      *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/INCLUDE/VCS/disk.h_v  $ *
// 
//    Rev 1.43   06 Oct 1998 22:42:24   RANSCHU
// Remove previous definition.
// 
//    Rev 1.42   06 Oct 1998 22:36:08   RANSCHU
// LFNEntryToAscii now returns a value.
// 
//    Rev 1.41   17 Apr 1998 16:12:32   MBROWN
// Updated structures used by Rescue
// 
//    Rev 1.40   30 Mar 1998 13:51:14   MBROWN
// Added flags field to ABSDISKREC
// 
//    Rev 1.39   11 Mar 1998 15:03:26   SKURTZ
// Extended Int13 drive support.
//
//    Rev 1.38   19 Dec 1997 12:28:12   SKURTZ
// SectorsLo and SectorsHi in INT13EXTREC transposed
//
//    Rev 1.37   27 Oct 1997 16:18:36   MARKK
// Added fields to DirScanRec
//
//    Rev 1.36   10 Jul 1997 15:44:08   MBROWN
// Added partition ID's for new partition types. Added prototype for new function
// in dsk_misc.c. Added structure for extended int 13 information. Fixed
// unmatching open and close marker for C comments.
//
//    Rev 1.35   14 May 1997 18:56:02   MBROWN
// Changed FAT mirroring fn's to take a LPCDISKREC instead of a LPDISKREC.
//
//    Rev 1.34   13 May 1997 17:49:16   MBROWN
// Added prototype for FATReadDirect.
//
//    Rev 1.33   08 May 1997 18:11:50   MBROWN
// Added prototypes for FAT32 Mirroring API.
//
//    Rev 1.32   23 Apr 1997 16:15:50   MBROWN
// Moved DiskRead/WriteFSInfo() prototypes outside of #ifdef SYM_DOS block.
//
//    Rev 1.31   19 Mar 1997 21:49:54   BILL
// Modified files to build properly under Alpha platform
//
//    Rev 1.30   28 Dec 1996 17:48:24   RANSCHU
// Made seperate NEC definitions that are not conditioned on NCD definition.
//
//    Rev 1.29.2.1   28 Dec 1996 17:44:12   RANSCHU
// Made seperate NEC definitions that are not conditioned on NCD definition.
//
//    Rev 1.29.2.0   18 Dec 1996 20:11:02   rstanev
// Branch base for version QAKI
//
//    Rev 1.29   22 Nov 1996 12:39:52   MARKK
// Changed calls to DirGet/SetSCN
//
//    Rev 1.28   18 Nov 1996 17:37:08   BMCCORK
// Added DiskGetFreeBytes()
//
//    Rev 1.27   24 Sep 1996 16:05:36   RCHINTA
// Merged Changes from Quake 9 (NEC: Partition Record and
// prototypes for IsValidNECHDNumber() and IsValidNECFloppyNumber()).
//
//    Rev 1.26   22 Aug 1996 13:07:46   DHERTEL
// Merged changes from E branch.
//
//    Rev 1.25   07 Aug 1996 15:11:52   DBUCHES
// Added DSE_NOROOTDIR
//
//    Rev 1.24   31 Jul 1996 12:24:34   MARKK
// Added DSM_ROOT message
//
//    Rev 1.23   06 Jun 1996 13:30:40   MRIVERA
// Added DR_NTFS for working on NTFS partitions.
//
//    Rev 1.22   03 May 1996 11:23:02   MARKK
// Added FATGet/PutRaw
//
//    Rev 1.21   01 May 1996 18:13:18   SKURTZ
// Change 2nd param of ImageSaveWithNotify to a UINT
//
//    Rev 1.20   01 May 1996 17:12:32   SKURTZ
// Change to proto for ImageSaveWithNotify()
//
//    Rev 1.19   25 Apr 1996 13:57:24   SKURTZ
// Added more error codes returned by ImageSave()
//
//    Rev 1.18   16 Apr 1996 15:29:56   PMARTIN
// Added DiskGetBootDriveEx(), which is a cover for DiskGetBootDrive().
//
//    Rev 1.17   15 Apr 1996 12:59:36   SKURTZ
// Changed FRDataRecord.dwrootOffset to dwFatSectors
//
//    Rev 1.16   13 Apr 1996 12:23:12   RANSCHU
// Hadn't gone back far enough in time to restore old FRDataRecordPreFAT32.
//
//    Rev 1.15   13 Apr 1996 11:58:06   RANSCHU
// FRDataRecordPreFAT32 wasn't exactly the earlier version.
//
//    Rev 1.14   12 Apr 1996 22:33:48   RANSCHU
// Added FRDataRecordPreFAT32, the version of FRDataRecord before FAT32 changes.
//
//    Rev 1.13   05 Apr 1996 13:57:10   SKURTZ
// Moved imgstate enum to other header file.
//
//    Rev 1.12   05 Apr 1996 13:10:00   SKURTZ
// Added ImageSaveWithNotify()
//
//    Rev 1.11   03 Apr 1996 10:24:32   SKURTZ
// Protos for FatCompress,FatDecompress & DiskGetFileSCN. Updated FRDataRecord struct
//
//    Rev 1.10   02 Apr 1996 10:01:34   MARKK
// Added DiskMuckWithSwapfile
//
//    Rev 1.9   26 Mar 1996 16:31:24   MARKK
// Padded DeviceParamRec to be larger
//
//    Rev 1.8   14 Mar 1996 22:32:44   THOFF
// Renamed the FSINFO structure to FILESYSINFO because of a name conflict.
//
//    Rev 1.7   28 Feb 1996 08:18:50   MARKK
// Use FAT32BPB as default BPB now
//
//    Rev 1.6   23 Feb 1996 17:08:28   FCHAPA
// Move prototypes for DiskGet/SetRootSCN
//
//    Rev 1.5   23 Feb 1996 12:17:10   MARKK
// Added some FAT 32 protos
//
//    Rev 1.4   23 Feb 1996 10:12:56   MARKK
// Changed DeviceParamRec to have FAT32BPB
//
//    Rev 1.3   16 Feb 1996 16:30:34   MARKK
// Fixed DirectoryIsUnmovableFile function for FAT32
//
//    Rev 1.2   05 Feb 1996 12:57:26   SKURTZ
// Made LPDIRENTRYREC the same as HPDIRENTRYREC. This was an issue from FAT32
// development due to the possibilities of very large directories.
//
//    Rev 1.1   29 Jan 1996 16:10:16   RANSCHU
// DIRSCANPROC accepts a CLUSTER instead of a WORD.
//
//    Rev 1.0   26 Jan 1996 20:15:40   RJACKSO
// Initial revision.
//
//    Rev 1.298   23 Jan 1996 15:35:54   DBUCHES
// Added DSSE_DIRTOOBIG
//
//    Rev 1.297   23 Jan 1996 14:10:16   RANSCHU
// Added IsEndCluster macro.
//
//    Rev 1.296   22 Jan 1996 10:25:18   RANSCHU
// Extend the bits in UNUSED|BAD|END_CLUSTER.
//
//    Rev 1.295   19 Jan 1996 13:39:26   RSTANEV
// Changed the prototype of VolumeLabelGet().
//
//    Rev 1.294   17 Jan 1996 13:00:18   DBUCHES
// Added IDs for FAT32 partitions.
//
//    Rev 1.293   17 Jan 1996 12:57:12   RSTANEV
// Changed a prototype to work with Unicode.
//
//    Rev 1.292   16 Jan 1996 13:39:46   JREARDON
// Moved Fernando's changes to the trunk from an accidental branch
//
//    Rev 1.291.1.0   13 Jan 1996 13:34:18   FCHAPA
// DIRHEADERREC changes: hSibling is now hNextSibling. Added hPrevSibling and dwDirTableIndex.
//
//    Rev 1.291   29 Dec 1995 11:00:48   SKURTZ
// DIRSEARCHREC and DirSearch() handling DWORD size SCNs.
//
//    Rev 1.290   29 Dec 1995 10:16:08   SKURTZ
// Updating FatFindSCN to use DWORD size clusters
//
//    Rev 1.289   27 Dec 1995 11:15:54   SKURTZ
// DirectoryGetNthHandle,DirectoryIndexGetLFNPath & others take/return dword as dir index
//
//    Rev 1.288   20 Dec 1995 14:57:38   DBUCHES
// Changed BitFAT related elements of DIRSCANREC.
//
//    Rev 1.287   18 Dec 1995 13:22:24   DBUCHES
// Added IsRootSCN()
//
//    Rev 1.286   15 Dec 1995 14:12:46   DBUCHES
// More 32 Bit FAT changes.
//
//    Rev 1.285   13 Dec 1995 14:26:00   rowant
// Changed all words in FRDataRecord to DWORDS!
//
//    Rev 1.284   13 Dec 1995 14:06:14   DBUCHES
// More 32 BitFAT changes.
//
//    Rev 1.283   08 Dec 1995 16:05:20   DBUCHES
// Changed FAT.lpData to hpData.
//
//    Rev 1.282   07 Dec 1995 14:50:26   ROWANT
// Removed dead prototypes for SYMFAT
//
//    Rev 1.281   07 Dec 1995 12:52:36   ROWANT
// Removed padding frmo diskrec
//
//    Rev 1.280   07 Dec 1995 12:08:44   ROWANT
// Added FATClearEntireFAT
//
//    Rev 1.279   07 Dec 1995 11:54:44   DBUCHES
// More 32 bit fat changes.
//
//    Rev 1.278   07 Dec 1995 11:08:24   ROWANT
// More FAT32 work
//
//    Rev 1.277   05 Dec 1995 22:36:44   ROWANT
// 32Bit FAT changes
//
//    Rev 1.276   05 Dec 1995 19:31:10   ROWANT
// 32bit FAT changes for fat_fat
//
//    Rev 1.275   05 Dec 1995 18:51:10   DBUCHES
// More 32bit FAT changes.
//
//    Rev 1.274   04 Dec 1995 16:00:02   MARKK
// Work for 32 bit FAT
//
//    Rev 1.273   06 Nov 1995 13:52:18   DBUCHES
// Added DSF_CHECKTIMES.
//
//    Rev 1.272   24 Oct 1995 12:17:52   SKURTZ
// Added wSectorsUsedPerFat to FAT structure
//
//    Rev 1.271   02 Aug 1995 16:36:40   DAVID
// Added IsLongXxxDirEntry2 macros for testing erased dir entries.
//
//    Rev 1.270   28 Jul 1995 12:38:10   BILL
// Merged branch changes to trunk
//
//    Rev 1.269   29 Jun 1995 19:00:26   LindaD
// NEC fixes
//
//    Rev 1.268   20 Jun 1995 18:18:30   RGLENN
// Added define DISK_BLOCK_ALLOC_ERROR
//
//    Rev 1.267   19 Jun 1995 10:49:14   DCHERMA
// Per discussion, changed NDRV_STACKER to 0x0400.
//
//    Rev 1.266   16 Jun 1995 20:10:10   DCHERMA
// Restored NDRV_STACKER fearing a reason to keep out of NDRV_ALL_DRIVES.
//
//    Rev 1.265   16 Jun 1995 19:39:22   DCHERMA
// Changed NDRV_STACKER to 0x0400 to be included in NDRV_ALL_DRIVES.
//
//    Rev 1.264   15 Jun 1995 17:25:20   DBUCHES
// Added DSSE_ILLEGALCHAR.
//
//    Rev 1.263   07 Jun 1995 14:41:10   MARKK
// Added NDRV_STACKER
//
//    Rev 1.262   01 Jun 1995 20:19:00   HENRI
// Changed DiskGetDriveSpaceVersion to use an LPWORD
//
//    Rev 1.261   25 May 1995 14:01:10   HENRI
// Added DiskGetPhysicalInfoEx (to provide translated vs. actual disk geometry)
//
//    Rev 1.260   13 May 1995 14:37:16   DBUCHES
// Added DiskIsDriveSpace3 and DiskGetDriveSpaceVersion.
//
//    Rev 1.259   11 May 1995 11:56:08   MARKK
// Added new function FATGetFATSize to return the size of the FAT
//
//    Rev 1.258   10 Apr 1995 16:28:42   HENRI
// Fixed prototype for DirIsEntry
//
//    Rev 1.257   10 Apr 1995 14:30:56   BRUCE
// Added IMAGE_ERRs that were added in QAK6 but not on trunk yet
//
//    Rev 1.256   10 Apr 1995 13:27:54   BRUCE
// Added FileIsOpen2()
//
//    Rev 1.255   10 Mar 1995 12:39:56   HENRI
// Added DirectoryIsValidVolumeLabel and DirectoryIsLegalName
//
//    Rev 1.254   10 Mar 1995 01:19:18   BRUCE
//
// Changed the lpbSwaped parm to _DiskIsDoubleSpace() to be a pointer to a
// WORD instead of a ptr to a BOOL because BOOL changes size between 32 and 16
// sides (and the thunk compiler doesn't do well with that).
//
//    Rev 1.253   07 Mar 1995 22:37:24   DAVID
// Added DiskGetCurrentLockState()
//
//    Rev 1.252   22 Feb 1995 15:41:48   DBUCHES
// Fixed declaration for DirIsEntry.
//
//    Rev 1.251   19 Feb 1995 17:11:12   HENRI
// From Branch: Added DirectoryHasValidDateTime; DSSE_BADDATETIME replaced
//
//    Rev 1.250   16 Feb 1995 17:39:10   DBUCHES
// Merged fixes from branch.
//
//    Rev 1.249   16 Feb 1995 16:38:58   BILL
// Changed CMOS_288M from 6 to 5 as per investigation.  This may not be
// valid for all Japeneese (NEC?) machines
//
//    Rev 1.248   10 Feb 1995 15:39:40   DBUCHES
// Added DirectoryComputeLFNBasisName
//
//    Rev 1.247   09 Feb 1995 21:18:58   MARKK
// Added params to DiskEnumerateMemoryFiles
//
//    Rev 1.246   02 Feb 1995 17:56:56   BRAD
// Added SYMKRNL VxD changes from QAK6
//
//    Rev 1.245   27 Jan 1995 16:18:20   BRAD
// Made VWIN32.H a system include, rather than local
//
//    Rev 1.244   24 Jan 1995 16:26:30   SKURTZ
// Added IMAGE_ERR_DRIVE_BUSY
//
//    Rev 1.243   06 Jan 1995 15:32:34   RSTANEV
// Added PABSDISKREC and PPARTENTRYREC.
//
//    Rev 1.242   15 Dec 1994 15:02:14   HENRI
// Added DirectoryScanSubTree prototype
//
//    Rev 1.241   09 Dec 1994 13:49:14   HENRI
// Added the DSE_BADDATETIME suberror code for DIRSCAN
//
//    Rev 1.240   08 Dec 1994 16:42:06   BRAD
// Added DiskIsRemovable
//
//    Rev 1.239   06 Dec 1994 12:59:22   HENRI
// Added the DirectoryFormatErrorString prototype
//
//    Rev 1.238   29 Nov 1994 18:50:30   HENRI
// Added lpBitFAT and uBitFATSize to the DIRSCANREC
//
//    Rev 1.237   16 Nov 1994 14:52:40   MARKK
// Added param to  DirectoryFileSpecCompare
//
//    Rev 1.236   11 Nov 1994 11:11:38   HENRI
// Added DirScan fat comparing, more date/time field checking & duplicate LFNs
//
//    Rev 1.235   10 Nov 1994 18:18:28   Basil
// Added dirscan symbols for supporting LFN's in FindFirst scans.
//
//    Rev 1.234   02 Nov 1994 20:28:52   BRUCE
// Changed VolumeSupportsLFN to take an LPCSTR
//
//    Rev 1.233   31 Oct 1994 16:44:00   MARKK
// Added DiskEnumerateMemoryFiles and DiskPollWrites
//
//    Rev 1.232   26 Oct 1994 20:29:50   BILL
// Forced callers of IOctVWin32 to define INCLUDE_VWIN32 inorder to avoid
// breaking all users who currently don't have DDK in path (most of QUAKE
// doesn't have the DDK in their include PATH)
//
//    Rev 1.231   25 Oct 1994 19:29:18   HENRI
// Moved IOCtlVWin32 to an area not excluding SYM_WIN32 (oops)
//
//    Rev 1.230   25 Oct 1994 19:25:16   HENRI
// Added IOCtlVWin32() function (only for SYM_WIN32)
//
//    Rev 1.229   24 Oct 1994 16:56:56   BRAD
// Added DiskGetFirstFixedDrive()
//
//    Rev 1.228   21 Oct 1994 16:25:58   DBUCHES
// Added DSF_SKIPLOSTCLUSTERTEST
//
//    Rev 1.227   17 Oct 1994 16:57:56   bill
// Added DiskIsMDF
//
//    Rev 1.226   12 Oct 1994 15:52:40   BRAD
// Changed from Floppy() to FloppyDrive().  Consistent
//
//    Rev 1.225   12 Oct 1994 15:46:00   BRAD
// Added DiskGetFirst...() rtns
//
//    Rev 1.224   12 Oct 1994 11:34:52   MARKK
// Added some fields to DIRHEADERREC
//
//    Rev 1.223   06 Oct 1994 14:40:36   BRAD
// Added NEC_DRVTYPE.
//
//    Rev 1.222   04 Oct 1994 17:19:08   BRAD
// Added NEC before error codes.  Generated conflicts
//
//    Rev 1.221   04 Oct 1994 17:08:32   BRAD
// Added NEC support
//
//    Rev 1.220   20 Sep 1994 21:04:22   DAVID
// Added padding fields to DiskRec and AbsDiskRec to ensure DWORD fields
// are on DWORD boundaries.  This fixes the exception violations that
// occur when these structures passed to functions implemented with thunks
// in Chicago.  Also made some other adjustments to the thunk script.
//
//    Rev 1.219   24 Aug 1994 14:24:18   MARKK
// Added file enumeration and swap file functions
//
//    Rev 1.218   22 Aug 1994 20:30:28   DAVID
// Fixed DIRHEADERREC size problems in DirTrace(), DirRead(), etc.
//
//    Rev 1.217   22 Aug 1994 14:52:28   MARKK
// New function to determine header size
//
//    Rev 1.216   22 Aug 1994 09:27:22   MARKK
// Added LFN field to DIRHEADERREC
//
//    Rev 1.215   16 Aug 1994 10:47:44   MARKK
// Fixed LFN macro
//
//    Rev 1.214   15 Aug 1994 16:38:36   MARKK
// Added LFN_BITS
//
//    Rev 1.213   10 Aug 1994 16:27:10   MARKK
// Added DSM_DONE message
//
//    Rev 1.212   28 Jul 1994 19:05:28   MFALLEN
// Changed proto to VolumeSupportsLFN()
//
//    Rev 1.211   28 Jul 1994 18:21:44   MFALLEN
// Added VolumeSupportLFN()
//
//    Rev 1.210   15 Jul 1994 12:15:46   BRAD
// Added LFN_* constants
//
//    Rev 1.209   12 Jul 1994 18:40:28   HENRI
// Moved thunked functions to symkrnl's thunk.h
//
//    Rev 1.208   12 Jul 1994 16:13:02   RGLENN
// Moved all Compressed drive info CDISK.H
//
//    Rev 1.207   11 Jul 1994 16:17:12   MARKK
// Added proto for DirctoryIsUnmovableFile
//
//    Rev 1.206   07 Jul 1994 16:05:18   DBUCHES
// Added thunks for DiskIsStacker, DiskIsBlockDevice and DiskIsDriveSpace
//
//    Rev 1.205   06 Jul 1994 17:10:08   DBUCHES
// Added thunks for DiskGetPhysicalType,DiskIsCompressed and DiskIsDoubleSpace.
//
//    Rev 1.204   29 Jun 1994 18:44:18   DBUCHES
// Added prototypes for DiskGetHDParams16/32
//
//    Rev 1.203   29 Jun 1994 18:42:54   HENRI
// Added Windows 4.0 Exclusive Lock Functions
//
//    Rev 1.202   23 Jun 1994 09:33:12   MARKK
// Added new field in dirheader for speedisk
//
//    Rev 1.201   16 Jun 1994 17:33:50   SKURTZ
// Added 7 protos for DirScan
//
//    Rev 1.200   16 Jun 1994 16:32:58   HENRI
// Added thunking prototypes
//
//    Rev 1.199   15 Jun 1994 12:44:30   MARKK
// Added DirectoryFileSpecCompare
//
//    Rev 1.198   15 Jun 1994 12:41:20   HENRI
// Added thunked calls of DiskGetPhysicalType
//
//    Rev 1.197   10 Jun 1994 15:55:00   SKURTZ
// Added protos for DirectoryEntryFromHandle() and DirectoryGetNthDirHeader()
//
//    Rev 1.196   09 Jun 1994 17:19:04   HENRI
// Define directory pointer manipulations for 32 bit
//
//    Rev 1.195   09 Jun 1994 15:04:54   DAVID
// Changed DirTrace cluster type from UINT to WORD.
//
//    Rev 1.194   08 Jun 1994 11:20:48   MARKK
// Changed proto for HandleDiskErrors
//
//    Rev 1.193   07 Jun 1994 14:35:02   HENRI
// Added DSF_USEFINDFIRST to DirScan flags
//
//    Rev 1.192   06 Jun 1994 13:39:30   HENRI
// Changed prototypes for a couple of routines to use WORD instead of UINT
//
//    Rev 1.191   31 May 1994 12:49:58   HENRI
// Change FAT calls to use WORD values rather than UINT values
//
//    Rev 1.190   25 May 1994 17:46:00   DAVID
// Fixed up macros for testing LFN directory entries.
//
//    Rev 1.189   25 May 1994 15:58:16   HENRI
// Changed parameters on DirectoryCheckLFNs
//
//    Rev 1.188   24 May 1994 18:01:16   HENRI
// Pass handle to directory to DirectoryCheckLFNs
//
//    Rev 1.187   24 May 1994 16:05:08   MARKK
// Changed parms to MassageDirEntry
//
//    Rev 1.186   24 May 1994 14:40:02   HENRI
// DirectoryCheckLFN now takes an option to find only the first error in a dir
//
//    Rev 1.185   24 May 1994 12:24:08   MARKK
// Added UNMOVABLEFILE message
//
//    Rev 1.184   20 May 1994 15:15:16   HENRI
// Added hClusterBuffer to DIRSCANREC
//
//    Rev 1.183   20 May 1994 14:04:28   HENRI
// Changed DSSE_BADORD to DSSE_TRUNCATED
//
//    Rev 1.182   19 May 1994 12:53:44   HENRI
// Changed DirectoryReturnLFN to return a BOOL
//
//    Rev 1.181   18 May 1994 17:22:10   HENRI
// Changed some DIRSCAN flags
//
//    Rev 1.180   18 May 1994 13:41:22   HENRI
// Removed lphDirTable from DIRSCANREC
//
//    Rev 1.179   18 May 1994 10:33:30   MARKK
// Two changes:
//     The FILESPECLISTREC structure now has a file spec type of WCHAR and an
//     unwild type of BYTE.
//
//     The directory index in the DIRSCANREC was incorrectly named and typed.
//     It has been renamed hDirIndex and typed to HGLOBAL.
//
//    Rev 1.178   13 May 1994 16:15:20   HENRI
// Added DirectoryCheckLFNs prototype
//
//    Rev 1.177   13 May 1994 11:46:58   HENRI
// Added LFN related stuff
//
//    Rev 1.176   12 May 1994 11:42:22   HENRI
// More changes for DIRSCAN
//
//    Rev 1.175   10 May 1994 16:51:34   DBUCHES
// Added DiskIsDriveSpace()
//
//    Rev 1.174   10 May 1994 16:09:32   MEIJEN
// EISA CMOS error code
//
//    Rev 1.173   06 May 1994 17:05:04   HENRI
// More DIRSCAN changes
//
//    Rev 1.172   06 May 1994 12:00:58   HENRI
// Added DSM_BREAKCHECK messsage
//
//    Rev 1.171   05 May 1994 16:09:44   HENRI
// More DIRSCAN changes
//
//    Rev 1.170   05 May 1994 11:58:44   HENRI
// DirScan now uses HGLOBALS instead of the generic form HANDLE
//
//    Rev 1.169   05 May 1994 11:53:16   SKURTZ
// FATFindSCN now needs a LPFAT parameter
//
//
//    Rev 1.168   04 May 1994 17:45:22   HENRI
// Removed AuxFAT stuff and replaced with FAT calls that require lpFAT param
//
//    Rev 1.167   04 May 1994 14:37:54   MFALLEN
// 32-bit compiler didn't like VDiskLock function prototype.
//
//    Rev 1.166   04 May 1994 13:59:10   HENRI
// Added DIRSCAN stuff.
//
//    Rev 1.165   27 Apr 1994 16:41:08   BRAD
// Added DiskReadMBR() and DiskWriteMBR()
//
//    Rev 1.164   27 Apr 1994 15:25:36   BRAD
// added DiskGetPartitionTableEntry()
//
//    Rev 1.163   27 Apr 1994 13:25:28   BRAD
// Changes for WIN32
//
//    Rev 1.162   27 Apr 1994 12:20:32   BRAD
// Added WRITE_LONG_COMMAND to possible commands for DiskAbsOperation()
//
//    Rev 1.161   26 Apr 1994 16:34:08   BRAD
// Removed PVCS Logs
 ************************************************************************/

#ifndef _INCL_DISK_
#define _INCL_DISK_

#ifdef SYM_WIN32
#ifdef INCLUDE_VWIN32
#ifndef _M_ALPHA        // WES Don't include for Alpha platform
#define Not_VxD
#include <vwin32.h>     // required for IOCtlVWin32(...) call
#endif
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------
// Windows has a few FS_ types defined, but 'forgot' to define these
// (although it does set them).
//----------------------------------------------------------------------
#if !defined(FS_LFN_APIS)
#define FS_LFN_APIS             0x00004000
#endif

#if !defined(FS_VOLUME_COMPRESSED)
#define FS_VOLUME_COMPRESSED    0x00008000
#endif


/*----------------------------------------------------------------------*
 * Symantec TSR control commands                                        *
 *----------------------------------------------------------------------*/

#define TSR_INSTALLED   0               // check for installation
#define TSR_ENABLE      1               // enable
#define TSR_DISABLE     2               // disable
#define TSR_FLUSH       3               // flush
#define TSR_SUSPEND     4               // suspend reads & writes
#define TSR_RESUME      5               // resume  reads & writes

/*----------------------------------------------------------------------*/
/* DiskCache(wCommand) command values                                   */
/*----------------------------------------------------------------------*/

#define CACHE_PRESENT   TSR_INSTALLED   /* Test if cache is present     */
#define CACHE_DISABLE   TSR_DISABLE     /* Turn cache off, if any       */
#define CACHE_ENABLE    TSR_ENABLE      /* Turn cache on, if any        */
#define CACHE_DUMP      TSR_FLUSH       /* Dump cached writes           */

/*----------------------------------------------------------------------*
 * Used with DiskErrorOn()                                              *
 *----------------------------------------------------------------------*/

#ifndef SYM_WIN

#ifndef INCL_STDDOS         // defined in stddos.h
typedef UINT (*ERRORTRAPPROC)(UINT errorInfo, UINT errorCode, BYTE *deviceName);
#endif

#define TRAP_DISK_ERRORS        ((ERRORTRAPPROC) -1)

#endif  // SYM_WIN

/*----------------------------------------------------------------------*/
/* Partition Table System IDs                                           */
/*----------------------------------------------------------------------*/

#define         SYSTEM_NOT_USED         0
#define         SYSTEM_DOS_12           1       /* DOS, 12-bit Fat      */
#define         SYSTEM_XENIX1           2       /* XENIX                */
#define         SYSTEM_XENIX2           3       /* XENIX                */
#define         SYSTEM_DOS_16           4       /* DOS, 16-bit Fat      */
#define         SYSTEM_EXTENDED         5       /* DOS, Extended        */
#define         SYSTEM_BIGDOS           6       /* DOS, 32Meg+ Partition*/
#define         SYSTEM_HPFS             7       /* HPFS partition       */
#define         SYSTEM_OS2_BOOT_MGR     0x0A    /* OS/2 boot manager    */
#define         SYSTEM_DOS_32           0x0B    /* DOS, 32-Bit Fat      */
#define         SYSTEM_EXTENDED_32      0x0C    /* DOS, 32-Bit Fat with */
                                                /* int13 extension      */
#define         SYSTEM_DOS_32_I13EX     0x0C    /* DOS, 32-Bit Fat with */
                                                /* int13 extension (same*/
                                                /* as above, but less   */
                                                /* likely interpreted as*/
                                                /* "extended fat32 part)*/
#define         SYSTEM_BIGDOS_I13EX     0x0E    /* DOS, Extended, with  */
                                                /* int13 extension      */
#define         SYSTEM_EXTENDED_I13EX   0x0F    /* DOS, 32Meg+ Partition*/
                                                /* with int13 extension */
#define         SYSTEM_DM_RO            0x50    /* Disk Manager R/O     */
#define         SYSTEM_DM_RW            0x51    /* Disk Manager R/W     */
#define         SYSTEM_GOLDEN_BOW       0x56    /* Golden Bow partition */
#define         SYSTEM_SS_UNKNOWN1      0x61    /* SpeedStor partition  */
#define         SYSTEM_386IX            0x63    /* Interactive's 386/IX */
#define         SYSTEM_NETWARE_286      0x64    /* Netware 286          */
#define         SYSTEM_NETWARE_386      0x65    /* Netware 386          */
#define         SYSTEM_PCIX             0x75    /* PCIX                 */
#define         SYSTEM_EVEREX_SUPERDOS  0xAA    /* Everex Super DOS     */
                                                /* partitions upto 285M */
#define         SYSTEM_CPM              0xDB    /* CPM                  */
#define         SYSTEM_SS_12BIT         0xE1    /* SpeedStor 12-bit     */
#define         SYSTEM_SS_UNKNOWN2      0xE3    /* SpeedStor partition  */
#define         SYSTEM_SS_16BIT         0xE4    /* SpeedStor 16-bit     */
#define         SYSTEM_SS_EXTENSION     0xF1    /* SpeedStor Extension  */
#define         SYSTEM_SS_LARGE         0xF4    /* SpeedStor LARGE      */
#define         SYSTEM_BBT              0xFF    /* Bad Block Table(Unix)*/

// Retain old name for compatibility
#define SYSTEM_NOVELL SYSTEM_NETWARE_286

/*----------------------------------------------------------------------*/
/* Disk Access Flags                                                    */
/*      Used for allowing/denying access to a DOS disk using DOS 4.0+   */
/*      services.  Has to do with "Invalid Media Type"                  */
/*----------------------------------------------------------------------*/

#define         DISALLOW_ACCESS         0
#define         ALLOW_ACCESS            1

/*----------------------------------------------------------------------*/
/* Internal Disk Accessed Flags                                         */
/*      Used for marking a disk unused so DOS will reload everything    */
/*      at the next access (such as a modified FAT)                     */
/*----------------------------------------------------------------------*/

#define DISK_ACCESSED       (BYTE)0x00
#define DISK_NOT_ACCESSED   (BYTE)0xFF

/*----------------------------------------------------------------------*/
/* DOS 4.0 Extended Boot Record signature                               */
/*----------------------------------------------------------------------*/

#define         EXTENDED_BOOT_RECORD_SIGNATURE          0x29

/*----------------------------------------------------------------------*/
/* First Physical Hard Disk Number.                                     */
/*----------------------------------------------------------------------*/

#define FIRST_HD        0x80            /* 1st Hard Disk on system      */

/*----------------------------------------------------------------------*/
/* Determining Valid Drive codes on NEC machines                        */
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI IsValidNECHDNumber(BYTE byDl );
BOOL SYM_EXPORT WINAPI IsValidNECFloppyNumber(BYTE byDn);

/*----------------------------------------------------------------------*/
/* DISK error or MEMORY error, when performing disk-related task.       */
/*----------------------------------------------------------------------*/

#define DISK_MEMORY_ERROR       1       /* Memory problem               */
#define DISK_PHYSICAL_ERROR     2       /* Error reading/writing        */

/*----------------------------------------------------------------------*/
/* Maximum Values for Physical Characteristics                          */
/*----------------------------------------------------------------------*/

#define MAX_CYLINDERS           1024

/*----------------------------------------------------------------------*
 * Size of buffer allocated by DiskGetDriveType()                       *
 *----------------------------------------------------------------------*/

#define DRIVE_TYPE_BUFFER_SIZE  2048

/*----------------------------------------------------------------------*/
/* MAJOR types of drive types.                                          */
/*      These types are reported by the new DiskGetType().  Do not      */
/*      confuse these with the Windows GetDriveType() return values.    */
/*----------------------------------------------------------------------*/

#define TYPE_UNKNOWN            0
#define TYPE_DOESNT_EXIST       10
#define TYPE_PHANTOM            11
#define TYPE_REMOVEABLE         12
#define TYPE_FIXED              13
#define TYPE_REMOTE             14
#define TYPE_NETWORK            TYPE_REMOTE
#define TYPE_RAMDISK            15
#define TYPE_RAM                TYPE_RAMDISK
#define TYPE_ENCRYPTED          16
#define TYPE_CDROM              17
#define TYPE_SUBST              18
#define TYPE_SUBSTED            TYPE_SUBST
#define TYPE_ASSIGNED           19
#define TYPE_LOGICAL            20
#define TYPE_INTERLINK          21      // DOS 6.0s interlink drives

#define	NEC_DRV_TYPE		4

#define TYPE_HD  TYPE_FIXED     /* Disk Doctor uses this older name     */

#define IsTypeFloppy(d)         (((d) == TYPE_360K) || \
                                 ((d) == TYPE_720K) || \
                                 ((d) == TYPE_12M)  || \
                                 ((d) == TYPE_144M) || \
                                 ((d) == TYPE_288M))

/*----------------------------------------------------------------------*/
/* MINOR types of drive types.                                          */
/*      These types are reported by the new DiskGetType(), and by       */
/*      DiskGetFloppyType().  These numbers are used to index tables,   */
/*      so be careful when changing them.  DiskGetPhysicalType() can    */
/*      return any of these numbers, or TYPE_FIXED.                     */
/*----------------------------------------------------------------------*/

//      TYPE_UNKNOWN            0
#define TYPE_360K               1
#define TYPE_720K               2
#define TYPE_12M                3
#define TYPE_144M               4
#define TYPE_288M               5

/*----------------------------------------------------------------------*/
/* MINOR types for TYPE_ENCRYPTED                                       */
/*      These types are reported by DiskGetType()                       */
/*----------------------------------------------------------------------*/

#define TYPE_CLOSE              1
#define TYPE_OPEN               2

/*----------------------------------------------------------------------*/
/* MINOR types for TYPE_LOGICAL                                         */
/*      These types are reported by DiskGetType()                       */
/*----------------------------------------------------------------------*/

//      TYPE_UNKNOWN            0
#define TYPE_DBLSPACE           1
#define TYPE_STACKER            2
#define TYPE_SUPERSTOR          3

/*----------------------------------------------------------------------*/
/* Old style (Symantec Library for DOS) types of drive types.           */
/* These types are returned by the traditional DiskType function.       */
/*                                                                      */
/* WHEN AT ALL POSSIBLE, USE THE DISKGETTYPE function and the types     */
/* defined above.  DiskType just calls DiskGetType and translates the   */
/* the new style defines to old style defines.                          */
/*----------------------------------------------------------------------*/

#define         DISKTYPE_PHYSICAL       0xFF
#define         DISKTYPE_UNCHECKED      0xFE            // used in DISKDISKTYPE.C
#define         DISKTYPE_UNKNOWN        0
#define         DISKTYPE_360K           1
#define         DISKTYPE_720K           2
#define         DISKTYPE_12M            3
#define         DISKTYPE_144M           4
#define         DISKTYPE_288M           5               // Note: DISKTYPE_288M != CMOS_288M (NOTE THIS COMMENT NOW FALSE WES)
#define         DISKTYPE_HD             10
#define         DISKTYPE_NETWORK        11
#define         DISKTYPE_ASSIGNED       12
#define         DISKTYPE_SUBST          13
#define         DISKTYPE_PHANTOM        14
#define         DISKTYPE_LOGICAL        15
#define         DISKTYPE_ENCRYPTED      16
#define         DISKTYPE_CDROM          17
#define         DISKTYPE_RAM            18
#define         DISKTYPE_REMOVABLE      19 // Removeable drive
#define         DISKTYPE_INTERLINK      20 // Interlink drives

#define DiskTypeIsFloppy(d)      (((d) == DISKTYPE_360K) || \
                                  ((d) == DISKTYPE_720K) || \
                                  ((d) == DISKTYPE_12M)  || \
                                  ((d) == DISKTYPE_144M) || \
                                  ((d) == DISKTYPE_288M))

/*----------------------------------------------------------------------*/
/* Indices used to index into our BPB_TABLES[].                         */
/*----------------------------------------------------------------------*/

#define         BPB_INDEX_160K          0
#define         BPB_INDEX_180K          1
#define         BPB_INDEX_320K          2
#define         BPB_INDEX_360K          3
#define         BPB_INDEX_720K          4
#define         BPB_INDEX_12M           5
#define         BPB_INDEX_144M          6
#define         BPB_INDEX_288M          7
#define		BPB_INDEX_12M8		8	// Added for NEC
#define		BPB_INDEX_640K		9	// PC-9801

/*----------------------------------------------------------------------*/
/* The maximum number of BPB indices supported on a single drive type   */
/* plus 1 (160, 180, 320, 360, 1200).  Use when calling DiskBPBTable(). */
/*----------------------------------------------------------------------*/

#define         MAX_BPB                 8       // For NEC drives

/*----------------------------------------------------------------------*/
/* Different CMOS diskette types                                        */
/*----------------------------------------------------------------------*/

#define         CMOS_360K               1
#define         CMOS_12M                2
#define         CMOS_720K               3
#define         CMOS_144M               4
#define         CMOS_288M               5
/* NOTE: IBM says CMOS type 5 was used on some Japanese machines        */
/* CMOS_288M was defined as 6, I see bios's returning this as 5,        */
/* do to the above comment, this may be invalid on NEC machines.        */

/*----------------------------------------------------------------------*/
/* EISA CMOS Read/Write Error Code                                      */
/*----------------------------------------------------------------------*/

#define   EISA_ERR_SLOT_INVALID         0x80
#define   EISA_ERR_FUNC_INVALID         0x81
#define   EISA_ERR_RAM_CORRUPT          0x82
#define   EISA_ERR_SLOT_EMPTY           0x83
#define   EISA_ERR_WRITE_CMOS           0x84
#define   EISA_ERR_CMOS_FULL            0x85
#define   EISA_ERR_BIOS_INVALID         0x86
#define   EISA_ERR_CFG_INVALID          0x87


/*----------------------------------------------------------------------*
 * Disk block device error code                                         *
 *----------------------------------------------------------------------*/
#define DISK_BLOCK_ALLOC_ERROR           (0x0200)

/*----------------------------------------------------------------------*
 * DiskIOCTL() "byCommand" parameter                                    *
 *----------------------------------------------------------------------*/

#define         IOCTL_CHAR_READ         0x02    /* Read from char dev   */
#define         IOCTL_CHAR_WRITE        0x03    /* Write to char dev    */
#define         IOCTL_BLOCK_READ        0x04    /* Read from block dev  */
#define         IOCTL_BLOCK_WRITE       0x05    /* Write to block dev   */

/*----------------------------------------------------------------------*/
/* "Generic IOCTL" constants                                            */
/*----------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------*/
/* IOCTL drive types                                                    */
/*----------------------------------------------------------------------*/

#define         IOCTL_360K              0
#define         IOCTL_12M               1
#define         IOCTL_720K              2
#define         IOCTL_8_INCH_SINGLE     3
#define         IOCTL_8_INCH_DOUBLE     4
#define         IOCTL_HD                5
#define         IOCTL_TAPE_DRIVE        6
#define         IOCTL_OTHER_DOS4        7 /* Returned by DOS <= 4.0 */
#define         IOCTL_144M              7 /* Returned by DOS >= 5.0 */
#define         IOCTL_288M              9 /* Returned by DOS >= 5.0 */
#define         IOCTL_OTHER_DOS5        8 /* Returned by DOS >= 5.0 */

// NOTE: Even though it looks backwards, it is correct that IOCTL_288M
//       is GREATER than IOCTL_OTHER_DOS5.  Thanks Microsoft!

/*----------------------------------------------------------------------*
 * Misc. IOCTL constants                                                *
 *----------------------------------------------------------------------*/

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

#ifndef SYM_OS2

/*----------------------------------------------------------------------*/
/* INT 25h/INT 26h constants                                            */
/*----------------------------------------------------------------------*/

#define BLOCK_READ      0x25
#define BLOCK_WRITE     0x26

/*----------------------------------------------------------------------*/
/* INT 13h commands                                                     */
/*----------------------------------------------------------------------*/

#define RESET_COMMAND           0       /* Reset disk system            */
#define READ_COMMAND            2       /* Read sectors                 */
#define WRITE_COMMAND           3       /* Write sectors                */
#define VERIFY_COMMAND          4       /* Verify sectors               */
#define FORMAT_COMMAND          5       /* Format track                 */
#define INITIALIZE_COMMAND      0x09    /* Initialize controller        */
#define READ_LONG_COMMAND       0x0A    /* Read long                    */
#define WRITE_LONG_COMMAND      0x0B    /* Write long                   */
#define RECALIBRATE_COMMAND     0x11    /* Recalibrate drive            */

/*----------------------------------------------------------------------*/
// Added for PC9800 machines
/*----------------------------------------------------------------------*/
#define RESET_COMMAND98		0x03
#define	READ_COMMAND98		0xD6	/* 0xF6	 with No retry	*/
#define	WRITE_COMMAND98		0xD5	/* 0xF5  with No retry	*/
#define	VERIFY_COMMAND98	0xD1
#define	FORMAT_COMMAND98	0x5D
#define	READ_COMMAND_RETRY98	0xD6	/* with 8 Times retry	*/
#define	WRITE_COMMAND_RETRY98	0xD5	/* with 8 Times retry	*/
#define	RECALIB_COMMAND98	0x27
#define	SENSE_COMMAND98		0x84
#define	READ_ID_COMMAND98	0x5A
#define	NO_RETRY98		0x20

#define	intrRESET_COMMAND98	3	// Initialize command code
#define	intrREAD_COMMAND98	6	// Read command code
#define	intrWRITE_COMMAND98	5	// Write command code
#define	intrVERIFY_COMMAND98	1	// Verify command code
#define	intrFORMAT_COMMAND98	0x0D	// Format command code
#define	intrRECALIB_COMMAND98	7	// Recalibrate command code
#define	intrSENSE_COMMAND98	4	// Sence command code

/*----------------------------------------------------------------------*/
/* INT 13h errors                                                       */
/*----------------------------------------------------------------------*/

#define         SYM_ERROR_NO_ERROR          ((BYTE) 0x00)
#define         SYM_ERROR_INVALID_FUNCTION  ((BYTE) 0x01)
#define         SYM_ERROR_ADDR_MARK_MISSING ((BYTE) 0x02)
#define         SYM_ERROR_WRITE_PROTECTED   ((BYTE) 0x03)
#define         SYM_ERROR_SECTOR_NOT_FOUND  ((BYTE) 0x04)
#define         SYM_ERROR_LINE_ACTIVE       ((BYTE) 0x06)
#define         SYM_ERROR_DMA_OVERRUN       ((BYTE) 0x08)
#define         SYM_ERROR_64K_DMA           ((BYTE) 0x09)
#define         SYM_ERROR_BAD_SECTOR        ((BYTE) 0x0A)
#define         SYM_ERROR_BAD_CYLINDER      ((BYTE) 0x0B)
#define         SYM_ERROR_INVALID_MEDIA     ((BYTE) 0x0C)
#define         SYM_ERROR_CRC_ERROR         ((BYTE) 0x10)
#define         SYM_ERROR_ECC_ERROR         ((BYTE) 0x11)
#define         SYM_ERROR_CONTROLLER_FAILED ((BYTE) 0x20)
#define         SYM_ERROR_SEEK_FAILED       ((BYTE) 0x40)
#define         SYM_ERROR_TIME_OUT          ((BYTE) 0x80)
#define         UNCORRECTABLE_ERRORS        ((BYTE) 0xA1)

/*----------------------------------------------------------------------*/
/* NEC errors                                                           */
/*----------------------------------------------------------------------*/
#define		NEC_ERROR_WRITE_PROTECTED98 ((BYTE) 0x10)	/* Sense command*/
#define		NEC_ERROR_BAD_CYLINDER98    ((BYTE) 0xD0)
#define		NEC_UNCORRECTABLE_ERRORS98  ((BYTE) 0xE0)
#define		NEC_ERROR_CONTROL_MARK	    ((BYTE) 0x10)	/* Not error	*/
#define		NEC_ERROR_DMA_BOUNDARY	    ((BYTE) 0x20)
#define		NEC_ERROR_END_OF_CYLINDER   ((BYTE) 0x30)
#define		NEC_ERROR_EQUIPMENT_CHECK   ((BYTE) 0x40)
#define		NEC_ERROR_OVER_RUN	    ((BYTE) 0x50)
#define		NEC_ERROR_NOT_READY	    ((BYTE) 0x60)
#define		NEC_ERROR_NOT_WRITABLE	    ((BYTE) 0x70)
#define		NEC_ERROR_FDC_ERROR	    ((BYTE) 0x80)
#define		NEC_ERROR_TIME_OUT98	    ((BYTE) 0x90)
#define		NEC_ERROR_DATA_ERROR_ID	    ((BYTE) 0xA0)
#define		NEC_ERROR_DATA_ERROR_DATA   ((BYTE) 0xB0)
#define		NEC_ERROR_NO_DATA	    ((BYTE) 0xC0)
#define		NEC_ERROR_MISSING_AM_ID	    ((BYTE) 0xE0)
#define		NEC_ERROR_MISSING_AM_DATA   ((BYTE) 0xF0)
					/* Following HD controler status */
#define		NEC_ERROR_CORRECTED_DATA    ((BYTE) 0x08)
#define		NEC_ERROR_ILLEGAL_ADDRESS   ((BYTE) 0x78)
#define		NEC_ERROR_DIRECT_ACCESS	    ((BYTE) 0x88)
#define		NEC_ERROR_DATA_ERROR_HD	    ((BYTE) 0xB8)
#define		NEC_ERROR_SEEK_ERROR	    ((BYTE) 0xC8)
#define		NEC_ERROR_READ_ALTERNATE    ((BYTE) 0xD8)


/*----------------------------------------------------------------------*/
/* INT 25 and 26 error codes                                            */
/*----------------------------------------------------------------------*/

#define         WRITE_PROTECTED         0x0300
#define         TIME_OUT                0x8000

#endif                                  // #ifndef SYM_OS2

/* Used in the NEW DiskGetInfo() routine for reporting return state     */
#define         VALID_DISK              0
#define         MEMORY_ERROR            1
#define         CANT_READ_BOOT_ERROR    2
#define         INVALID_DISK_ERROR      3
/*----------------------------------------------------------------------*/
/* Length of a volume label                                             */
/*----------------------------------------------------------------------*/

#define         VOLUME_LENGTH           11

/*----------------------------------------------------------------------*/
/* Number of bytes in 1 physical sector                                 */
/*----------------------------------------------------------------------*/

#define MAX_PHYSICAL_SECTOR_SIZE_NCD    2048
#define PHYSICAL_SECTOR_SIZE98  1024

#define MAX_PHYSICAL_SECTOR_SIZE    MAX_PHYSICAL_SECTOR_SIZE_NCD
#ifdef NCD //[
#define PHYSICAL_SECTOR_SIZE    PHYSICAL_SECTOR_SIZE98
#else //][
#define PHYSICAL_SECTOR_SIZE    512
#endif //]


/*----------------------------------------------------------------------*
 * Misc. constants related to partition tables                          *
 *----------------------------------------------------------------------*/

#define	MAX_ENTRIES_PER_TABLE_NCD	8	/* 8 entries per table	*/
												/* Start of Partition Table 	*/
#define	PART_TABLE_OFFSET_NCD			0x0000
												/* Size of 1 partition table entry */
#define 	PART_ENTRY_SIZE_NCD			32

												/* Offset of Signature word	*/
#define	SIGNATURE_OFFSET_NCD			0x01FE
												/* Signature in Partition table	*/
												/*	and Boot Record		*/
#define	SIGNATURE_NCD					0xAA55

#define MAX_ENTRIES_PER_TABLE_PC   4       /* 4 entries per table          */

                                        /* Start of Partition Table     */
#define PART_TABLE_OFFSET_PC       0x01BE
                                        /* Size of 1 partition table entry */
#define PART_ENTRY_SIZE_PC         16

                                        /* Offset of Signature UINT     */
#define SIGNATURE_OFFSET_PC        0x01FE

                                        /* Signature in Partition table */
                                        /*      and Boot Record         */
#define SIGNATURE_PC               0xAA55

#ifdef NCD //[
#define MAX_ENTRIES_PER_TABLE MAX_ENTRIES_PER_TABLE_NCD
#define PART_TABLE_OFFSET PART_TABLE_OFFSET_NCD
#define PART_ENTRY_SIZE PART_ENTRY_SIZE_NCD
#define SIGNATURE_OFFSET SIGNATURE_OFFSET_NCD
#define SIGNATURE SIGNATURE_NCD
#else //][
#define MAX_ENTRIES_PER_TABLE MAX_ENTRIES_PER_TABLE_PC
#define PART_TABLE_OFFSET PART_TABLE_OFFSET_PC
#define PART_ENTRY_SIZE PART_ENTRY_SIZE_PC
#define SIGNATURE_OFFSET SIGNATURE_OFFSET_PC
#define SIGNATURE SIGNATURE_PC
#endif //]


/*----------------------------------------------------------------------*/
/* Constants used to return the FAT Type Strings stored in the Boot     */
/* Record.                                                              */
/*----------------------------------------------------------------------*/

#define FAT_12_TYPE             0
#define FAT_16_TYPE             1
#define FAT_32_TYPE				2

/*----------------------------------------------------------------------*
 * Constants used in DiskIsTrueDrive()                                  *
 *----------------------------------------------------------------------*/

#define FILTER_ALL_DRIVES       0       /* All (logical) drives         */
#define FILTER_LOGICAL_ONLY     1       /* No net, assigned, subst-ed   */
#define FILTER_SPECIAL          2       /* LOGICAL_ONLY & filter proc   */
#define FILTER_CUSTOM           3       /* ALL_DRIVES & filter proc     */

/************************************************************************
 * typedef for cluster                                                  *
 ************************************************************************/

typedef DWORD CLUSTER;			// Cluster type now 32 bits
typedef DWORD far *LPCLUSTER;
typedef DWORD huge *HPCLUSTER;
/*----------------------------------------------------------------------*/
/* DASDRecord                                                           */
/*      Given a specific physical type (TYPE_360K, TYPE_720K, etc.)     */
/*              and the number of tracks, gets the DASD type to be      */
/*              used to set the DASD type using INT 13h, function 17h   */
/*                                                                      */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

typedef struct DASDRec
    {
    BYTE        driveType;      /* TYPE_360K, TYPE_720K, etc.   */
    WORD        tracks;         /* Number of tracks             */
    BYTE        DASD_Type;      /* DASD id for this type        */
    } DASDRec, DASDRecord;

typedef struct EXTENDEDBPBFLAGS
    {
    WORD        activeFAT:4;		// Number of the active FAT
    WORD        reserved1:3;
    WORD       	bMirrored:1;		// True if FATs are mirrored
    WORD        reserved2:8;
    } EXTENDEDBPBFLAGS;

/*----------------------------------------------------------------------*/
/* BPBRec                                                               */
/*      Record containing disk BPB information.                         */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

typedef struct OLDBPBREC
    {
    WORD        bytesPerSector;         /* Bytes per sector             */
    BYTE        sectorsPerCluster;      /* Sectors per cluster          */
    WORD        startOfFat;             /* First sector of Fat          */
    BYTE        numberFats;             /* Copies of Fat                */
    WORD        maxRootEntries;         /* Maximum entries in Root dir. */
    WORD        oldTotalSectors;        /* Old total number of sectors  */
    BYTE        mediaDescriptor;        /* Media descriptor BYTE        */
    WORD        sectorsPerFat;          // Sectors per Fat, 0 indicates 32
					// bit FAT
    WORD        sectorsPerTrack;        /* Sectors per Track            */
    WORD        heads;                  /* Number of sides              */
    DWORD       hiddenSectors;          /* Number of hidden sectors     */
    DWORD       totalSectors;           /* Total sectors for BigFoot    */
                                        /*      drives                  */
    BYTE        reserved[6];            /* Reserved                     */
    } OldBPBRec, OldBPBRecord, OLDBPBREC, FAR *LPOLDBPBREC, NEAR *NPOLDBPBREC;

/*----------------------------------------------------------------------*/
/* Record containing Extended BPB information found in the Boot Record  */
/*      of the disk. disk. Use INT 25h to get this information.         */
/* NOTE: This is the EXTENDED version of the BPB.  It is only found in  */
/*      DOS 4.0+                                                        */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

#if 0
typedef struct ExtendedBPBRec
    {
    WORD        bytesPerSector;         /* Bytes per sector             */
    BYTE        sectorsPerCluster;      /* Sectors per cluster          */
    WORD        startOfFat;             /* First sector of Fat          */
    BYTE        numberFats;             /* Copies of Fat                */
    WORD        maxRootEntries;         /* Maximum entries in Root dir. */
    WORD        oldTotalSectors;        /* Old total number of sectors  */
    BYTE        mediaDescriptor;        /* Media descriptor BYTE        */
    WORD        sectorsPerFat;          /* Sectors per Fat              */
    WORD        sectorsPerTrack;        /* Sectors per Track            */
    WORD        heads;                  /* Number of sides              */
    DWORD       hiddenSectors;          /* Number of hidden sectors     */
    DWORD       totalSectors;           /* Total sectors for BigFoot    */
                                        /*      drives                  */
    BYTE        physicalNumber;         /* Physical drive number        */
    BYTE        reserved1[1];           /* Disk head where BPB is found */
    BYTE        extendedSignature;      /* Should be 0x29               */
    DWORD       serialNumber;           /* Date/time of formatting      */
    BYTE        volumeLabel[11];        /* Volume Label                 */
    BYTE        reserved2[8];           /* File Type (HPFS, DOS 12)     */
    } ExtendedBPBRec, ExtendedBPBRecord;
#endif

/************************************************************************
 * FAT 32 BPB record                                                    *
 ************************************************************************/

typedef struct FAT32BPBREC
    {
    WORD        bytesPerSector;         /* Bytes per sector             */
    BYTE        sectorsPerCluster;      /* Sectors per cluster          */
    WORD        startOfFat;             /* First sector of Fat          */
    BYTE        numberFats;             /* Copies of Fat                */
    WORD        maxRootEntries;         /* Maximum entries in Root dir. */
    WORD        oldTotalSectors;        /* Old total number of sectors  */
    BYTE        mediaDescriptor;        /* Media descriptor BYTE        */
    WORD        sectorsPerFat;          // Sectors per Fat, 0 indicates 32
					// bit FAT
    WORD        sectorsPerTrack;        /* Sectors per Track            */
    WORD        heads;                  /* Number of sides              */
    DWORD       hiddenSectors;          /* Number of hidden sectors     */
    DWORD       totalSectors;           /* Total sectors for BigFoot    */
                                        /*      drives                  */
    DWORD       dwBigSectorsPerFAT;	// Number of sectors in a 32 bit FAT
    EXTENDEDBPBFLAGS rExtendedBPBFlags;	// See structure above
    BYTE        byFSMinorVersion;	// Version for file system
    BYTE        byFSMajorVersion;	// Version for file system
    DWORD       dwRootSCN;		// Starting cluster of Root dir
    WORD        wFSInfoSector;		// Sector number of FS Info struct
    WORD        wBackupBootSector;	// If non zero, indicates the sector
					// number of a copy of the boot
					// record.  Usually 6.
    BYTE        reserved1[12];          /* Reserved                     */
    BYTE        byPhysicalDriveNum;	// Int 13 drive number
    BYTE        reserved2[1];
    BYTE        byBootSignature;	// Extended boot signature (29H)
    DWORD       dwVolumeID;		// Volume serial number
    BYTE        abyVolumeLabel[11];	// Volume label
    BYTE        abyFileSystemType[8];	// "FAT32   "
    } FAT32BPBREC, FAR *LPFAT32BPBREC,
      BPBRec, BPBRecord, BPBREC, FAR *LPBPBREC, NEAR *NPBPBREC;

typedef const BPBRec NEAR *NPCBPBREC;
typedef const BPBRec FAR *LPCBPBREC;


/*----------------------------------------------------------------------*/
/* Record containing BPB information found in the Boot Record of the    */
/*      disk. Use INT 25h to get this information.                      */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

typedef struct BPBBootRec
    {
    BYTE        jmp[3];                 /* Jump over BPB                */
    BYTE        oemName[8];             /* OEM Signature                */
    BPBREC      BPB;                    /* BPB                          */
    } BPBBootRec, BPBBootRecord;

#define         FAT32BPBSIG     0xAA550000

typedef struct FAT32BPBBOOTSECTOR
    {
    BYTE        jmp[3];                 /* Jump over BPB                */
    BYTE        oemName[8];             /* OEM Signature                */
    FAT32BPBREC BPB;                    /* BPB                          */
    BYTE        reserved[508-11-sizeof(FAT32BPBREC)];
    DWORD       dwSignature;		// 0xAA550000
    } FAT32BPBBOOTREC;

/************************************************************************
 * FileSysInfo structure                                                *
 ************************************************************************/

#define         FILESYSINFOSIG       0x61417272

typedef struct FILESYSINFO
    {
    DWORD       dwSignature;		// Signature: Aarr;
    DWORD       dwFreeClusterCount;	// Count of free clusters
    DWORD       dwNextFreeCluster;	// Next cluster to be allocated
    DWORD       dwReserved1[3];
    } FILESYSINFO, FAR *LPFILESYSINFO;

typedef struct FAT32EXBOOTSECTOR
    {
    DWORD       dwSignature1;		// 0x41615252
    BYTE        reserved1[0x1e4-sizeof(DWORD)];
    FILESYSINFO FSInfo;
    DWORD       dwSignature2;		// 0xAA550000
    } FAT32EXBOOTREC;

/*----------------------------------------------------------------------*/
/* DiskRec                                                              */
/*      Record containing all information needed to read/write from/to  */
/*      a disk using DOS absolute sectors (INT 25h and INT 26h).        */
/*      Includes support for BigFoot and DOS 3.4                        */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

#define         DR_IO_NORMAL    0
#define         DR_IO_BIGFOOT   1
#define         DR_IO_EXTENDED  2

#define         DR_FAT12        0
#define         DR_FAT16        1
#define         DR_FAT32        2
#define         DR_NTFS         3

#define         IsFAT12(r)      ((r).byFATType == DR_FAT12)
#define         IsFAT16(r)      ((r).byFATType == DR_FAT16)
#define         IsFAT32(r)      ((r).byFATType == DR_FAT32)
#define         IsNTFS(r)       ((r).byFATType == DR_NTFS)

typedef struct DISKREC
    {
    BYTE        dn;                     /* Drive number (0-A:,1-B:,etc) */
    BYTE        driverEntry;            /* Entry within device driver   */
    WORD        bytesPerSector;         /* Bytes per Sector             */
    BYTE        sectorsPerCluster;      /* Sectors per Cluster          */
    BYTE        reserved;               /* Reserved                     */
    WORD        startOfFat;             /* First sector of Fat          */
    BYTE        numberFats;             /* Copies of Fat                */
    WORD        maxRootEntries;         /* Maximum root dir. entries    */
    WORD        startOfRootDir;         /* First sector of Root dir.    */

    BYTE        byIOType;               /* Is drive BigFooted?          */
    BYTE        byFATType;              /* Is 16-bit Fat? (or 12-bit?)  */
    BYTE        dl;                     /* Drive Letter (A:,B:,etc.)    */
    BYTE        mediaDescriptor;        /* Media descriptor BYTE        */
    WORD        sectorsInRoot;          /* Sectors in root directory    */
    BYTE        padding1;               /* Pad to next DWORD boundary   */
    DWORD       totalSectors;           /* Total sectors on disk (not   */
                                        /*    filled in by LoadDiskInfo)*/
    DWORD       sectorsBeforePartition; /* Total sectors before this    */
                                        /*      partition               */
    DWORD       bytesPerCluster;        /* Bytes per cluster: Same as:  */
                                        /* BytesPerSector *             */
                                        /*      SectorsPerCluster       */
    DWORD       sector;                 /* First sector to read/write   */
    DWORD       sectorCount;            /* Number of sectors to r/w     */
    BYTE huge   *buffer;                /* Buffer where reads/writes    */
                                        /*      will occur              */
    WORD        wMajorType;             // Major drive type
    WORD        wMinorType;             // Minor drive type
    WORD        wMask;                  // Mask for this drive i.e. NDRV_xxx
    BYTE        padding2[2];            // Pad to next DWORD boundary
    DWORD       dwStartOfDataEx;        /* First sector of Data area    */
    DWORD       dwMaxClusterEx;         /* Largest Cluster number       */
    DWORD       dwSectorsPerFATEx;      /* Sectors Per Fat              */
    CLUSTER     clBAD;                  // The value of BAD cluster
    CLUSTER     clEOF;                  // The lowest value of EOF

    /********************************************************************
     * Note, the fields below are only valid iff IsFAT32 is TRUE.       *
     ********************************************************************/

    CLUSTER     clRootSCN;              /* First cluster of Root dir.   */
    FILESYSINFO FSInfo;                 // Extended boot info

    BYTE        byReserved[26];         // Future space
    } DiskRec, DISKREC, FAR *LPDISKREC, NEAR *NPDISKREC, DiskRecord;

typedef const DiskRec NEAR *NPCDISKREC;
typedef const DiskRec FAR *LPCDISKREC;

/*----------------------------------------------------------------------*/
/* ShortDiskRec                                                         */
/*      First 19 bytes of the DiskRec defined above.  Used when         */
/*      moving information from internal DOS tables.                    */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/


#if 0
typedef struct ShortDiskRec
    {
    BYTE        driveNumber;    /* Drive number (0-A:,1-B:,etc) */
    BYTE        driverEntry;    /* Entry within device driver   */
    WORD        bytesPerSector; /* Bytes per Sector             */
    BYTE        sectorsPerCluster; /* Sectors per Cluster       */
    BYTE        reserved;       /* Reserved                     */
    WORD        startOfFat;     /* First sector of Fat          */
    BYTE        numberFats;     /* Copies of Fat                */
    WORD        maxRootEntries; /* Maximum root dir. entries    */
    WORD        startOfData;    /* First sector of Data area    */
    WORD        maxCluster;     /* Largest Cluster number       */
    WORD        sectorsPerFat;  /* Sectors Per Fat              */
    WORD        startOfRootDir; /* First sector of Root dir.    */
    } ShortDiskRec, FAR *LPSHORTDISKREC, NEAR *NPSHORTDISKREC, ShortDiskRecord;
#endif
/*------------------------------------------------------------------------
 * DPBRec - DeviceParamaterBlock record.                                 *
 ------------------------------------------------------------------------*/
typedef struct ShortDiskRec
    {
    BYTE        driveNumber;         /* Drive number (0-A:,1-B:,etc) */
    BYTE        driverEntry;         /* Entry within device driver   */
    WORD        bytesPerSector;      /* Bytes per Sector             */
    BYTE        sectorsPerCluster;   /* Sectors per Cluster       */
    BYTE        reserved;            /* Reserved                     */
    WORD        startOfFat;          /* First sector of Fat          */
    BYTE        numberFats;          /* Copies of Fat                */
    WORD        maxRootEntries;      /* Maximum root dir. entries    */
    WORD        startOfData;         /* First sector of Data area    */
    WORD        maxCluster;          /* Largest Cluster number       */
    WORD        sectorsPerFat;       /* Sectors Per Fat              */
    WORD        startOfRootDir;      /* First sector of Root dir.    */
    DWORD       dpb_driver_addr;     /* Pointer to driver            */
    BYTE        mediaDescriptor;     /* Media byte                   */
    BYTE        reserved2;           /* Reserved                     */
    DWORD       dpb_next_dpb;        /* Pointer to next DPB          */
    WORD        dpb_next_free;       /* # of last allocated cluster  */
    WORD        wFreeCntLo;          /* Count of free clusters, -1 if unknown */
    WORD        wFreeCntHi;          /* high word of free_cnt if BigFAT */
    WORD        wFlags;              /* FAT32 flags from BPB */
    WORD        wFSInfoSector;       /* Sec # of file system info sector (-1 if none) */
    WORD        wBkUpBootSector;     /* Sec # of backup boot sector (-1 if none) */
    DWORD       dwStartOfData;       /* First sector of first cluster */
    DWORD       dwMaxCluster;        /* Number of clusters on drive + 1 */
    DWORD       dwSectorsPerFAT;     /* Number of records occupied by FAT */
    DWORD       dwFirstRootCluster;  /* cluster # of first cluster of root dir */
    DWORD       dwNextFreeCluster;   /* # of last allocated cluster */
    } ShortDiskRec, FAR *LPSHORTDISKREC, NEAR *NPSHORTDISKREC, ShortDiskRecord;

/*----------------------------------------------------------------------*/
/* AbsDiskRec                                                           */
/*      Contains all the information you need to manipulate absolute    */
/*      physical sectors of a disk                                      */
/*      Kinda like DiskRec                                              */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

typedef struct ABSDISKREC
    {
    BYTE        dn;                     /* Drive number (0,1,0x80,0x81) */
    BYTE        pad[3];                 /* realignment  to DWORD        */

/* starting coordinates */
    DWORD       dwStartingHead;         /* First head (0-based)         */
    DWORD       dwStartingTrack;        /* First track (0-based)        */
    DWORD       dwStartingSector;         /* First sector (1-based)       */

/* ending coordinates   */
    DWORD       dwEndingHead;           /* Ending head # (0-based)      */
    DWORD       dwEndingTrack;          /* Ending track # (0-based)     */
    DWORD       dwEndingSector;         /* Ending sector # (1-based)    */

/* disk totals          */
    DWORD       dwSectorsPerTrack;      /* Sectors on each track        */
    DWORD       dwTotalTracks;          /* Total tracks on disk         */
    DWORD       dwTotalHeads;           /* Total heads on disk          */

/* current coordinates  */
    DWORD       dwHead;                 /* Current head (0-based)       */
    DWORD       dwTrack;                /* Current track (0-based)      */
    DWORD       dwSector;               /* Current sector (1-based)     */

    DWORD       dwMaxSectorsLo;         /* Quadword holding maximum     */
    DWORD       dwMaxSectorsHi;         /* physical sectors on the drive*/

    BYTE        bIsExtInt13;            /* Is Extend Int13 present?     */
                                        /* See ISINT13X_x, defines      */
    BYTE        numSectors;             /* Number of sectors to use     */

    BYTE        flags;                  /* Misc. info flags, e.g. bit 0 */
                                        /* tells us that the CHS totals */
                                        /* are picked by us to allow    */
                                        /* access to entire disk. These */
                                        /* values don't reflect actual  */
                                        /* disk geometry                */

    BYTE        padding;                /* Put buffer on DWORD boundary */
    BYTE huge   *buffer;                /* Ptr. to buffer to use        */
    BYTE        sectorsProcessed;       /* Number of sectors processed*/
    BYTE        CMOS_Type;              /* CMOS drive type              */
                                        /* (Floppies only)              */
                                        /*   1 - 360K                   */
                                        /*   2 - 1.2M                   */
                                        /*   3 - 720K                   */
                                        /*   4 - 1.44M                  */
    BYTE        numDrives;              /* Num. drives (floppy or hard) */
    BYTE	sectorLenCode;		/* Sector length code		*/
					/* (NEC Floppy or HD only)	*/
					/* code  :   0	  1    2    3	*/
					/* length: 128  256  512 1024	*/
    } AbsDiskRec, ABSDISKREC, *PABSDISKREC, FAR *LPABSDISKREC, NEAR *NPABSDISKREC, AbsDiskRecord;

typedef const AbsDiskRec NEAR *NPCABSDISKREC;
typedef const AbsDiskRec FAR *LPCABSDISKREC;



                                        /* Note that the _YES and the _NO*/
                                        /* values are arbitrary. Make   */
                                        /* sure that the _UNKNOWN always*/
                                        /* remains 0.                   */

#define ISINT13X_UNKNOWN    0           /* Unknown if Int13x supported  */
#define ISINT13X_YES        1           /* Yes, it does                 */
#define ISINT13X_NO         2           /* No it doesnt                 */

// Flags for 'flags' field of ABSDISKREC
#define TRANSLATION_ARBITRARY    0x01   /* Tells us that the CHS totals */
                                        /* were picked by us to allow   */
                                        /* access to entire disk. These */
                                        /* values don't reflect actual  */
                                        /* disk geometry                */

// Macros which use bFlags field of ABSDISKREC
#define IsTranslationArbitrary(pAbsDisk) \
        ((pAbsDisk)->flags & TRANSLATION_ARBITRARY)


/*----------------------------------------------------------------------*/
/* DisketteParamRec                                                     */
/*      Diskette Parameter Table record                                 */
/*      This record is used by the PC to format floppy diskettes        */
/*----------------------------------------------------------------------*/

typedef struct DisketteParamRec
    {
    BYTE        firstSpecifyByte;       /* ???????????                  */
    BYTE        secondSpecifyByte;      /* ???????????                  */
    BYTE        ticksTillTurnMotorOff;  /* Ticks till diskette          */
                                        /*   motor turns off            */
    BYTE        bytesPerSector;         /* Bytes/Sector (coded)         */
                                        /*      0 - 128 bytes/sector    */
                                        /*      1 - 256 bytes/sector    */
                                        /*      2 - 512 bytes/sector    */
                                        /*      3 - 1024 bytes/sector   */
    BYTE        sectorsPerTrack;        /* Sectors Per Track            */
    BYTE        gapLength;              /* Gap length                   */
    BYTE        dataLength;             /* Dtl (Data Length)            */
    BYTE        formatGapLength;        /* Gap Length for Format        */
    BYTE        formatFillByte;         /* Fill BYTE for Format         */
    BYTE        headSettleTime;         /* Head Settle Time (millisec)  */
    BYTE        motorStartupTime;       /* in 1/8 seconds               */
    } DisketteParamRec, DisketteParamRecord, FAR *LPDISKETTEPARAMRECORD;

/*----------------------------------------------------------------------*/
/* DisketteAddressRec                                                   */
/*      Floppy diskette address record.  Each record represents the     */
/*      the address that is layed down for each sector on a floppy disk */
/*----------------------------------------------------------------------*/

typedef struct DisketteAddressRec
    {
    BYTE        track;                  /* Track number (0-based)       */
    BYTE        head;                   /* Head number (0-based)        */
    BYTE        sector;                 /* Sector number (1-based)      */
    BYTE        bytesPerSector;         /* Bytes per sector             */
                                        /*    0 - 128 bytes/sector      */
                                        /*    1 - 256 bytes/sector      */
                                        /*    2 - 512 bytes/sector      */
                                        /*    3 - 1024 bytes/sector     */
    } DisketteAddressRec;

/*----------------------------------------------------------------------*/
/* FormatAddressRec                                                     */
/*      One record in the AT format table.  This table informs the      */
/*      controller of how a track should be formatted.  It includes     */
/*      enough information so the track will be formatted with a        */
/*      specific interleave.                                            */
/*----------------------------------------------------------------------*/

typedef struct FormatAddressRec
    {
    BYTE        mode;                   /* 00h - Good sector; 80h - Bad */
    BYTE        sector;                 /* Sector number; can use to    */
                                        /*   change interleave          */
    } FormatAddressRec;

/*----------------------------------------------------------------------*/
/* Int13Rec                                                             */
/*      Contains many fields that will be filled in if an INT 13h is    */
/*      generated when performing a DOS INT 25h call.  Used to map a    */
/*      logical device to its physical attributes.                      */
/*----------------------------------------------------------------------*/

typedef struct INT13REC
    {
    BYTE        generated;              /* Was it generated?            */
    BYTE        dn;                     /* Physical drive #             */
    DWORD       dwHead;                 /* Starting head # (0-based)    */
    DWORD       dwTrack;                /* Starting track # (0-based)   */
    DWORD       dwSector;               /* Starting sector # (1-based)  */
    BYTE        trigger;                /* Time for handler to act?     */
    BYTE        sectorLenCode;          /* Sector length code (NEC only)*/
    BYTE        ExtendedInt13;          /* was an extended Int13 used? if yes, the following apply*/
    DWORD       block_numLo;            /* used for extended Int13 calls */
    DWORD       block_numHi;            /* used for extended Int13 calls */
    } Int13Rec, INT13REC, Int13Record, FAR *LPINT13REC;


/*----------------------------------------------------------------------*/
/* DISKADDRPACKET                                                       */
/* Packet passed into extended Int13 calls for read/write/seek/etc      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*----------------------------------------------------------------------*/

typedef struct DISKADDRPACKET
    {
    BYTE            packet_size;                    /* size of packet in bytes (16) */
    BYTE            reserved1;                      /* reserved, must be 0          */
    BYTE            block_count;                    /* number of blocks to transfer */
    BYTE            reserved2;                      /* reserved, must be 0          */
    WORD            buffer_Off;                     /* address of transfer buffer   */
    WORD            buffer_Seg;                     /* address of transfer buffer   */
    DWORD           block_numLo;                    /* starting absolute block number*/
    DWORD           block_numHi;                    /* starting absolute block number*/
    } DiskAddrPacket, DISKADDRPACKET, FAR * LPDISKADDRPACKET;

/*----------------------------------------------------------------------*/
/* Int13ExtRec                                                          */
/*      Used for information returned from the extended INT 13h			*/
/*      function to get drive parameters.							    */
/*----------------------------------------------------------------------*/

typedef struct INT13EXTREC
	{
	WORD		info_size;				/* size of structure			*/
	WORD		flags;					/* information flags			*/
	DWORD		cylinders;				/* number of cylinders on disk  */
	DWORD		heads;					/* number of heads on disk		*/
        DWORD           sec_per_track;                          /* number of sectors per track  */
	DWORD		sectorsLo;				/* Lo DWORD for total sectors	*/
        DWORD           sectorsHi;                              /* Hi DWORD for total sectors   */
        WORD            sector_size;                            /* number of bytes per sector   */
	} Int13ExtRec, INT13EXTREC, Int13ExtRecord, FAR *LPINT13EXTREC;


/*----------------------------------------------------------------------*/
/* DeviceParamRec                                                       */
/*      Device information returned by DOS using Generic IOCTL command. */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/


typedef struct DeviceParamRec
    {
    BYTE        specialFunctions;
    BYTE        deviceType;
    WORD        deviceAttributes;
    WORD        numberOfTracks;
    BYTE        mediaType;
    FAT32BPBREC BPB;
    BYTE        abyFiller[85 - sizeof(FAT32BPBREC)];
	WORD        wSectorsPerTrack;
    } DeviceParamRec, DeviceParamRecord;

/*----------------------------------------------------------------------*/
/* READWRITETRACKPACKET                                                 */
/*      Read/write track packet, used to read or write a track of       */
/*      info using IOCTL.                                               */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

typedef struct READWRITETRACKPACKET
    {
    BYTE        bSpecialFunctions;
    WORD        wHead;
    WORD        wCylinder;
    WORD        wFirstSector;
    WORD        wNumberOfSectors;
    BYTE huge   *lpTransferAddress;
    } READWRITETRACKPACKET;

/*----------------------------------------------------------------------*/
/* FORMATPACKET                                                         */
/*      Packet used to format a track using IOCTL.                      */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

typedef struct FORMATPACKET
    {
    BYTE        bSpecialFunctions;
    WORD        wHead;
    WORD        wCylinder;
    } FORMATPACKET;

/*----------------------------------------------------------------------*/
/* SECTORPACKET                                                         */
/*      Packet used to format a track using IOCTL.  This structure      */
/*      helps us set up our track layout.                               */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

typedef struct SECTORPACKET
    {
    WORD        wSectorNumber;
    WORD        wSectorSize;
    } SECTORPACKET;

/*----------------------------------------------------------------------*/
/* PartEntryRec                                                         */
/*      Partition Table entry record.                                   */
/*      Used to be called 'part_struct'                                 */
/*----------------------------------------------------------------------*/

typedef struct PartEntryRec
    {
    BYTE        bootable;
    BYTE        startHead;
    BYTE        startSector;
    BYTE        startCylinder;
    BYTE        system;
    BYTE        endHead;
    BYTE        endSector;
    BYTE        endCylinder;
    DWORD       relativeSectors;        /* Sectors preceding partition  */
    DWORD       numSectors;             /* Sectors in partition         */
    } PartEntryRec, PartitionRec, PartitionRecord, *PPARTENTRYREC, FAR *LPPARTENTRYREC;

typedef const PartEntryRec FAR *LPCPARTENTRYREC;

/*----------------------------------------------------------------------*/
/* PartEntryRec                                                         */
/*      Partition Table entry record for NEC machines                   */
/*----------------------------------------------------------------------*/

typedef struct NECPartEntryRec
    {
    BYTE        bootable;           /* 20h=non-bootable */
    BYTE        condition;          /* Bit7 is active */
    WORD        unknown1;
    WORD        unknown2;
    WORD        unknown3;
    BYTE        unknown4;
    BYTE        unknown5;
    WORD        startCylinder;
    BYTE        unknown6;
    BYTE        unknown7;
    WORD        endCylinder;
    BYTE        systemName[ 16 ];

    } NECPartEntryRec, NECPartitionRec, NECPartitionRecord, *PNECPARTENTRYREC, FAR *LPNECPARTENTRYREC;
typedef const NECPartEntryRec FAR *LPCNECPARTENTRYREC;

/*----------------------------------------------------------------------*/
/* HDParamTable                                                         */
/*      Hard Disk Drive Parameter Table.  This is the table pointed to  */
/*      by Int 41h (1st drive) and Int 46h (2nd drive).                 */
/*                                                                      */
/*      See Pg. 3-16 of PS/2 BIOS Tech. Reference for table definition  */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

typedef struct HDParamTable
    {
    WORD        maxCylinders;           /* Maximum number of cylinders  */
    BYTE        maxHeads;               /* Maximum number of heads      */
    WORD        reducedWriteCylinder;   /* Reduced Write Cylinder (XT)  */
    WORD        writePrecompCylinder;   /* Write Precompensation Cyl.   */
    BYTE        maxECCBurstLength;      /* Max ECC Data Burst Length(XT)*/
    BYTE        controlByte;            /* Control BYTE                 */
    BYTE        standardTimeout;        /* Standard Time-out Value (XT) */
    BYTE        formatTimeout;          /* Time-out for Format Drive(XT)*/
    BYTE        checkTimeout;           /* Time-out for Check Drive (XT)*/
    WORD        landingZone;            /* Landing Zone (AT)            */
    BYTE        sectorsPerTrack;        /* Sectors Per Track (AT)       */
    BYTE        reserved;               /* Unused                       */
    } HDParamTable;

/*--------------------------------------------------------------*
 * This record is used for storing partition IDs and names      *
 *--------------------------------------------------------------*/

typedef struct SystemIDRec
    {
    BYTE id;
    BYTE *string;
    } SystemIDRec;

/*----------------------------------------------------------------------*/
/* BigFootRec                                                           */
/*      Bigfoot packet used to do 32Meg+ direct read/writes.            */
/*      NOTE: Fixed Size                                                */
/*----------------------------------------------------------------------*/

typedef struct BigFootRec
    {
    DWORD       startingSector;         /* First sector to read         */
    WORD        sectorCount;            /* Sector count                 */
    BYTE huge   *buffer;                /* Ptr. to buffer to use        */
    } BigFootRec;

/*----------------------------------------------------------------------*
 * MirrorIndexRec                                                       *
 *                                                                      *
 * This is the complete contents of the PC Tools Mirror index file,     *
 * which has the name MIRORSAV.FIL                                      *
 *                                                                      *
 * Basically, this is an index file, which is used to locate either     *
 * the current MIRROR.FIL, or the previous one, MIRROR.BAK              *
 *      NOTE: Fixed Size                                                *
 *----------------------------------------------------------------------*/

typedef struct MirrorIndexRec
    {
    DWORD       filFirstSector;         // first sector of MIRROR.FIL
    BYTE        signature[21];          // "AMSESLIFVASRORIMESAEP"
    WORD        filYear;                // file date of MIRROR.FIL
    BYTE        filDay;
    BYTE        filMonth;
    BYTE        filMinutes;             // file time of MIRROR.FIL
    BYTE        filHours;
    DWORD       bakFirstSector;         // first sector of MIRROR.BAK
    WORD        bakYear;                // file date of MIRROR.BAK
    BYTE        bakDay;
    BYTE        bakMonth;
    BYTE        bakMinutes;             // file time of MIRROR.BAK
    BYTE        bakHours;
    } MirrorIndexRec;

/*----------------------------------------------------------------------*
 * MirrorRec                                                            *
 *                                                                      *
 * This is the file header for the PC Tools Mirror file MIRROR.FIL and  *
 * its backup copy, MIRROR.BAK.                                         *
 *                                                                      *
 * There are several values that are not known at this time, but enough *
 * is known to make full use of the file.                               *
 *                                                                      *
 * The Mirror file consists of this header, followed by a map record    *
 * that maps sectors in the data area (which make up the MIRROR.FIL) to *
 * sectors in the system area of the disk.                              *
 *      NOTE: Fixed Size                                                *
 *----------------------------------------------------------------------*/

typedef struct MirrorRec
    {
    BYTE        filename[14];           // full pathname of the file
    BYTE        unknown1[8];
    BYTE        physicalDrive;          // ??? (not consistent)
    BYTE        isBackup;               // TRUE if this is a .BAK file
    WORD        unknown2;               // always 0?
    DWORD       startSector;            // first sector of MIRROR.FIL
    DWORD       nextSector;             // next sector of MIRROR.FIL
    WORD        unknown3;               // usually 0 or 512
    WORD        sectorsInSys;           // total system sectors saved in file
    WORD        rootOffset;             // sector offset to the root dir in file
    WORD        rootSectors;            // number of sectors in the root dir
    WORD        startOfRootDir;         // starting sector of root dir on disk
    WORD        unknown8;               // prob. sides on disk, same as unknown9
    BYTE        numberFats;             // number of FATs on the disk
    WORD        startOfFat;             // starting sector of FAT on disk
    BYTE        sectorsPerCluster;      // sectors per cluster
    WORD        sectorsPerTrack;        // sectors per track
    WORD        unknown5;               // usually 0 or sectorsPerTrack
    WORD        unknown10;              // usually a small number
    WORD        bytesPerSector;         // sector size
    BYTE        unknown6[7];
    BYTE        dl;                     // drive letter that this applies to
    BYTE        unknown9;               // prob. sides on disk, same as unknown8
    BYTE        unknown7[15];
    WORD        mapRecordItems;         // total records in the sector map
    } MirrorRec;

/*----------------------------------------------------------------------*
 * MirrorMapRec                                                         *
 *                                                                      *
 * This is the format of the map record, which starts directly after    *
 * the header.                                                          *
 *      NOTE: Fixed Size                                                *
 *----------------------------------------------------------------------*/

typedef struct MirrorMapRec
    {
    DWORD       sysSector;              // DOS sector number
    DWORD       sector;                 // Mirror file sector
    } MirrorMapRec;

/*----------------------------------------------------------------------*
 * FAT constants                                                        *
 *----------------------------------------------------------------------*/

                                        /*------------------------------*/
                                        /* State of FAT being loaded    */
                                        /* These values are set by      */
                                        /* FATREAD and checked by       */
                                        /* FATWRITE                     */
                                        /*------------------------------*/
#define FAT_NONE    ((BYTE) 0)          /* no FAT loaded into memory    */
#define FAT_LOADED  ((BYTE) 1)          /* FAT loaded into the pool area*/
#define FAT_CHANGED ((BYTE) 2)          /* FAT has been modified        */
#define FAT_ERROR   ((BYTE) 3)          /* Error reading or writing FAT */
#define FAT_ALLOCED ((BYTE) 4)          /* FAT allocated, but not read  */

                                        /*------------------------------*/
                                        /* VALUES IN THE FAT            */
                                        /*------------------------------*/
#define UNUSED_CLUSTER  0 /* Unallocated FAT entry        */

// The following definitions are obsolete and should be removed.
// Use the IsEndCluster macro and the IsBadFatEntry function instead.
#define BAD_CLUSTER     ((DWORD) 0xFFFFFF7) /* BAD FAT entry                */
#define END_CLUSTER     ((DWORD) 0xFFFFFFF) /* Last Cluster in chain */
                                        /* Changed from FFF8 to FFFF    */
#define BAD_SECT        BAD_CLUSTER     // (old name)
#define FAT_END         END_CLUSTER     // (old name)

#define FAT32VALUEMASK	0x0FFFFFFF
#define FAT16VALUEMASK	0x0000FFFF
#define FAT12VALUEMASK	0x00000FFF

// The following value is used to mask off the non-value bits of the
// cluster value for FATGet/FATPut
#define FatValueMask(pDisk) \
    (IsFAT32(*(pDisk)) ? FAT32VALUEMASK :\
    (IsFAT16(*(pDisk)) ? FAT16VALUEMASK :\
    (IsFAT12(*(pDisk)) ? FAT12VALUEMASK : ~(DWORD)0)))
#define FatEOFMask(pDisk) \
    (IsFAT32(*(pDisk)) ? FAT32VALUEMASK&~(DWORD)7 :\
    (IsFAT16(*(pDisk)) ? FAT16VALUEMASK&~(DWORD)7 :\
    (IsFAT12(*(pDisk)) ? FAT12VALUEMASK&~(DWORD)7 : ~(DWORD)0)))

#define FatValue(pDisk,clCluster) \
    ((clCluster)&FatValueMask(pDisk))
/* int IsEndCluster ( struct DISKREC * pDisk, CLUSTER clCluster ) */
#define IsEndCluster(pDisk,clCluster) \
    (((clCluster)&FatEOFMask(pDisk))==(pDisk)->clEOF)

#define MAX_FAT_SECTORS         256      /* (65536 * 2 / 512)           */

// The following macro is used to determine the maximum number of sectors,
// that when converted to bytes, will still fit in to a DWORD
#define MaxSectorsNoOverflow(r) (0xFFFFFFFF / (r).bytesPerSector)

/*
 *      Size of VMM blocks that hold FATs in memory
 *
 *      FAT_ENTRIES_PER_VMM_BLOCK must be at least 4096 so that a
 *      12-bit FAT can be read entirely into one block!
 */

#define FAT_ENTRIES_PER_VMM_BLOCK       4096
#define FAT_VMM_BLOCK_SIZE              (FAT_ENTRIES_PER_VMM_BLOCK * 2)
#define MAX_FAT_VMM_BLOCKS              ((WORD)(65536L / FAT_ENTRIES_PER_VMM_BLOCK))

/*
 *      Value in FATInfo.nLockedBlock when no block is locked
 */

#define NO_LOCKED_BLOCK ((UINT) -1)

/*----------------------------------------------------------------------*
 * Data structures                                                      *
 *----------------------------------------------------------------------*/

typedef UINT (WINAPI *LPFNDISKBLOCKDEVICEPROC)(UINT wCommand, LPDISKREC lpDisk);

typedef struct FAT
    {
    DISKREC     Disk;           // I/O info for disk which owns FAT
    LPBYTE      lpbyDirtyBlock;
    UINT        wState;         // State of FAT data buffer contents
    HPVOID      hpData;
    UINT        nFATEntriesPerVMMBlock;
    LPFNDISKBLOCKDEVICEPROC lpfnDiskBlockDeviceProc;
    DWORD       dwSectorsUsedPerFat;
    } FAT, *PFAT, NEAR *NPFAT, FAR *LPFAT;

typedef const FAT *PCFAT;
typedef const FAT NEAR *NPCFAT;
typedef const FAT FAR *LPCFAT;

/*----------------------------------------------------------------------*
 * Used with DiskBlockDevice() redirection                              *
 *----------------------------------------------------------------------*/

// Pointer to a substitute block device handler.  If this pointer is set
// for a given "virtual drive" (0: to 9:) by an application via a call to
// DiskBlockSetProc(), DiskBlockDevice() will call this handler instead
// of _DiskBlockDevice().  This allows applications to "fake" logical
// drives by suppling handlers suitable for a given device.  This is how
// DISKEDIT implements its "virtual mode".
//   The sector and sector count values are passed by reference so that
// the handler can change them if necessary.  The changed values will be
// passed to _DiskBlockDevice if it is subsequently called.
//   The handler should return a reasonable INT 25h/26h error code: zero
// if successful, otherwise as documented.
//   The application should call DiskBlockClearProc() before it exits, or
// when it no longer needs the redirected handler.

// _DiskBlockDevice redirection pointer under Windows.
//
// UINT SYM_EXPORT DiskBlockProc( UINT      wCommand,
//                            LPDISKREC lpDiskRec,
//                            DWORD     FAR *lpdwStartSector,
//                            WORD      FAR *lpwSectorsToRead,
//                            LPVOID    lpvBuffer,
//                            DWORD     dwBufferSegSel,
//                            DWORD     dwBigFootSegSel );
//
//
// lpvBuffer will be NULL under Windows.  The SegSel's will be NULL under DOS
//

typedef UINT (*DISKBLOCKPROC)( UINT, LPDISKREC, DWORD FAR *, WORD FAR *, LPVOID, DWORD, DWORD );


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/* Overlays the Header for Format Recover records */
#define ID_CODE_LEN 8                   /* Also change FR.C if value changes */

typedef struct FRrecord
    {
    unsigned char IDcode [ID_CODE_LEN];
    unsigned int  dateWord;
    unsigned int  timeWord;
    unsigned int  sequenceNumber;
    unsigned int  versionNumber;
    } FRRecord;

/* Image type codes for ImageSelect() */

#define IMAGE_NORMAL    0               /* NU IMAGE or FRECOVER file    */
#define IMAGE_MIRROR    1               /* PC-TOOLS or DOS5 MIRROR file */

/* flags used in uFlags parameter of ImageSaveWithNotify() */

#define IMAGE_FLAG_SINGLECOPY 0x01
#define IMAGE_FLAG_VERIFY     0x02


/* Error codes returned from IMAGE.EXE */

#define IMAGE_ERR_NONE                 0
#define IMAGE_ERR_GENERAL_ERROR        1
#define IMAGE_ERR_WRITE_PROTECTED      2
#define IMAGE_ERR_NO_SPACE             3
#define IMAGE_ERR_SUBST_DRIVE          4
#define IMAGE_ERR_INVALID_DRIVE        5
#define IMAGE_ERR_BAD_DISK             6
#define IMAGE_ERR_NETWORK_ERROR        7
#define IMAGE_ERR_NO_IDX_FILE_SPACE    8
#define IMAGE_ERR_MEMORY_ALLOCATION    9
#define IMAGE_ERR_IMAGE_EXE_NOT_FOUND 10
#define IMAGE_ERR_DOS_EXEC_FAILURE    11
#define IMAGE_ERR_LANTASTIC           12
#define IMAGE_ERR_EMPTY_DIR           13
#define IMAGE_ERR_DRIVE_BUSY          14  /* diskPollWrites repeatedly failed */
#define IMAGE_ERR_FILE_LOCKED         15  /* idx or dat file repeatedly R/O   */
#define IMAGE_ERR_LOCK_FAIL           16  /* DiskLockLogicalVolume failed     */
#define IMAGE_ERR_SHARE_VIOLATION     17  /* share violation on dat/idx files */
#define IMAGE_ERR_TOO_FRAGMENTED      18  /* fat too fragmented to compress */
#define IMAGE_ERR_COMPRESS_VALIDATION 19  /* failure when compressed fat was validated */


#define HEADER_SIZE                  16 /* OLD FR had 16 BYTE header    */

typedef struct FRMapRecord
    {
    DWORD       sector;                 /* Sector where allocated       */
    WORD        sysSector;              /* System sector                */
    } FRMapRecord;

typedef struct FRDataIndex
    {
    DWORD               firstSector;    /* Starting sector of file      */
    BYTE                month;          /* Month (1-12)                 */
    BYTE                day;            /* Day (1-31)                   */
    WORD                year;           /* Year (1980-2099)             */
    BYTE                hour;           /* Hour (0-23)                  */
    BYTE                minute;         /* Minute (0-59)                */
    } FRDataIndex;

typedef struct FRFoundRecord
    {
    UINT                found;          /* Is it found or not           */
    FRDataIndex         file;           /* Info about file              */
    } FRFoundRecord;

typedef struct  FRIndexRecord
    {
    BYTE                signature [9];  /* "BKPNCIHI"                   */
    FRDataIndex         file [2];       /* Data file info               */
    } FRIndexRecord;

/*----------------------------------------------------------------------*/
/* Make sure this record is NEVER larger than 128 BYTEs                 */
/* 128 BYTEs is the smallest size for a DOS logical sector              */
/*----------------------------------------------------------------------*/


typedef struct FRDataRecord
    {
    BYTE                signature [9];  /* Signature                    */
    BYTE                dl;             /* Drive letter                 */
    BYTE                isBackup;       /* Is Backup copy?              */
    WORD                bytesPerSector; /* BYTEs per sector             */
    DWORD               sectorsInSys;   /* Sectors In system            */
    DWORD               startOfFat;     /* Start of FAT                 */
    DWORD               startOfRootDir; /* Start of Root dir.           */
    BYTE                numberFats;     /* Number of FATs               */
    DWORD               sectorsPerFat;  /* Sectors per fat              */
    DWORD               bootOffset;     /* BYTE offset of boot index    */
    DWORD               fatOffset;      /* BYTE offset of Fat index     */
    DWORD               sectorsInIndex; /* Sectors used by the index    */
    FRDataIndex         file;           /* Same info found in INDEX file*/
    DWORD               indexSectors[10]; /* Sectors used for index     */
    DWORD               dwFatSectors;   /* sectors in compressed fat    */
    WORD                fatType;        /* these are the DR_FATxx       */
    BYTE                sectorsPerCluster;/* Sectors per Cluster        */
    BYTE                reserved [14];  /* Reserve lots of extra space  */
    } FRDataRecord;

typedef struct FRDataRecordPreFAT32
    {
    BYTE                signature [9];  /* Signature                    */
    BYTE                dl;             /* Drive letter                 */
    BYTE                isBackup;       /* Is Backup copy?              */
    WORD                bytesPerSector; /* BYTEs per sector             */
    WORD                sectorsInSys;   /* Sectors In system            */
    WORD                startOfFat;     /* Start of FAT                 */
    WORD                startOfRootDir; /* Start of Root dir.           */
    BYTE                numberFats;     /* Number of FATs               */
    WORD                sectorsPerFat;  /* Sectors per fat              */
    WORD                bootOffset;     /* BYTE offset of boot index    */
    WORD                fatOffset;      /* BYTE offset of Fat index     */
    WORD                sectorsInIndex; /* Sectors used by the index    */
    FRDataIndex         file;           /* Same info found in INDEX file*/
    DWORD               indexSectors[10]; /* Sectors used for index     */
    BYTE                reserved [20];  /* Reserve lots of extra space  */
    } FRDataRecordPreFAT32;


/*----------------------------------------------------------------------*/
/* The following records are used to save the partition table info.     */
/*----------------------------------------------------------------------*/

typedef struct PhysicalDriveInfo
    {
    BYTE        driveNum;               /* Drive number                 */
    DWORD       dwTotalHeads;           /* Total heads                  */
    DWORD       dwTotalTracks;          /* Total tracks                 */
    DWORD       dwSectorsPerTrack;      /* Sectors Per Track            */
    WORD        sectorSize;             /* Physical sector size         */
    BYTE        numPartitions;          /* Number of partition tables   */
    } PhysicalDriveInfo;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#define PART_HEADER_VERSION     3
#define BOOT_HEADER_VERSION     3
#define CMOS_HEADER_VERSION     2
#define CMOS_SAVE_LENGTH        64

#define RESCUE_PART             0x0001
#define RESCUE_BOOT             0x0002
#define RESCUE_CMOS             0x0004

#define SAMPLE_BYTE_LENGTH            5 // Number of bytes in sample buffer

#define SAMPLE_BYTE_CHECKSUM_COUNT_1 100 // Number of checksumed bytes Ver 1
#define SAMPLE_BYTE_CHECKSUM_COUNT_2  48 // Number of checksumed bytes Ver 2

#define SAMPLE_BASE_OFFSET_1    0xD000  // Sample Offset from Seg f000 Ver 1
#define SAMPLE_BASE_OFFSET_2    0xFFD0  // Sample Offset from Seg f000 Ver 2

typedef struct MainHeaderRec
    {
    char                signature [40]; /* Signature recognizing valid  */
                                        /* partition table info         */
    BYTE                version;        /* Version ID #                 */

    WORD                checksum;       /* Checksum of data file        */
    BYTE                numDrives;      /* Number of physical drives    */
    DWORD               fileSize;       /* Size of file                 */

    BYTE                month;          /* Month (1-12)                 */
    BYTE                day;            /* Day (1-31)                   */
    WORD                year;           /* Year (1980-2099)             */
    BYTE                hour;           /* Hour (0-23)                  */
    BYTE                minute;         /* Minute (0-59)                */
    WORD                dosVersion;     /* DOS version                  */
    PhysicalDriveInfo   driveInfo[4];   /* Allow 4 physical drives to   */
                                        /* be saved.                    */
    DWORD               computerCheckSum;/* machine signature checksum  */
                                        /* Sample BYTEs from machine    */
    BYTE                sampleBytes[SAMPLE_BYTE_LENGTH + 1];
    } MainHeaderRec;

/*----------------------------------------------------------------------*/
/* The DriveHeaderRec is used by Rescue. Each physical drive uses one   */
/* DriveHeaderRec for each partition.                                   */
/*                                                                      */
/* NOTE: The structure shouldn't be more than 16 bytes or Rescue breaks */
/*----------------------------------------------------------------------*/

typedef struct DriveHeaderRec
    {
    BYTE                driveNum;
    DWORD               dwHead;
    DWORD               dwTrack;
    DWORD               dwSector;
    BYTE                reserved [3];   /* In case we need more info    */
    } DriveHeaderRec;

/*----------------------------------------------------------------------*/
/* The following records are used to save the Boot Record info.         */
/*----------------------------------------------------------------------*/

typedef struct BootRecInfo
    {
    DISKREC             disk;           /* Disk Rec for this partition  */
    BYTE                filler1[20];    /* In case 'disk' grows         */
    ABSDISKREC          absDisk;        /* Abs. Disk Rec for partition  */
    BYTE                filler2[20];    /* In case 'absDisk' grows      */
    BPBREC              BPB;            /* BPB of partition             */
    BYTE                filler3[20];    /* In case 'BPB' grows          */
    } BootRecInfo;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

typedef struct BootHeaderRec
    {
    char                signature [40]; /* Signature recognizing valid  */
                                        /* partition table info         */
    BYTE                version;        /* Version ID #                 */

    BYTE                numDrives;      /* Number of Boot Records       */
    BYTE                month;          /* Month (1-12)                 */
    BYTE                day;            /* Day (1-31)                   */
    WORD                year;           /* Year (1980-2099)             */
    BYTE                hour;           /* Hour (0-23)                  */
    BYTE                minute;         /* Minute (0-59)                */
    WORD                dosVersion;     /* DOS version                  */
    DWORD               computerCheckSum;/* machine signature checksum  */
                                        /* Sample BYTEs from machine    */
    BYTE                sampleBytes[SAMPLE_BYTE_LENGTH + 1];
    } BootHeaderRec;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

typedef struct CMOSHeaderRec
    {
    char                signature [40]; /* Signature recognizing valid  */
                                        /* partition table info         */
    BYTE                version;        /* Version ID #                 */

    BYTE                month;          /* Month (1-12)                 */
    BYTE                day;            /* Day (1-31)                   */
    WORD                year;           /* Year (1980-2099)             */
    BYTE                hour;           /* Hour (0-23)                  */
    BYTE                minute;         /* Minute (0-59)                */
    WORD                dosVersion;     /* DOS version                  */
    DWORD               computerCheckSum;/* machine signature checksum  */
                                        /* Sample BYTEs from machine    */
    BYTE                sampleBytes[SAMPLE_BYTE_LENGTH + 1];
    } CMOSHeaderRec;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#define  ENCRYPTEDOPEN_MASK     0x0001

/*
**  Defines for the Drive Masks:
**  Each Mask represents a different bitmap.
*/
#define  NDRV_AVAILABLE         0x0000  // Returns available drives

#define  NDRV_ANY_DRIVE         0xFFFF  // Includes DRV_PHANTOM
#define  NDRV_ALL_DRIVES        0x0FFF  // Excludes DRV_PHANTOM
#define  NDRV_ALL_FLOPPIES      (NDRV_525 | NDRV_350 | NDRV_CDROM)
#define  NDRV_FIXED             (NDRV_HARD | NDRV_RAM | NDRV_SUBSTED | \
                                 NDRV_ENCRYPTED | NDRV_COMPRESSED | NDRV_STACKER)

/*
**  Floppies:
*/
#define  NDRV_525               0x0002
#define  NDRV_350               0x0004
#define  NDRV_CDROM             0x0008

/*
**  Fixed:
*/
#define  NDRV_HARD              0x0010
#define  NDRV_RAM               0x0020
#define  NDRV_SUBSTED           0x0040
#define  NDRV_ASSIGNED          NDRV_SUBSTED
#define  NDRV_ENCRYPTED         0x0100
#define  NDRV_LOCKED            (NDRV_ENCRYPTED)
#define  NDRV_UNLOCKED          (NDRV_ENCRYPTED | ENCRYPTEDOPEN_MASK)
#define  NDRV_COMPRESSED        0x0200
#define  NDRV_STACKER           0x0400
#define  NDRV_NETWORK           0x0800
#define  NDRV_PHANTOM           0x1000

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#define FULL_STROKE_SEEKS       58
#define AVERAGE_SEEKS           232
#define STATUS_BAR_LENGTH       28

/*----------------------------------------------------------------------*/
/* This is the recovery info stored in the diagnostic cylinder          */
/*  algorithm to recover data:                                          */
/*          for all tracks in diagnostic cylinder                       */
/*              {                                                       */
/*              if sector 1 contains signature and sector 1 == sector 2 */
/*                  {                                                   */
/*                  read data track in diag cylinder and restore data   */
/*                  }                                                   */
/*              }                                                       */
/*----------------------------------------------------------------------*/

extern  BYTE    FAR CALIBRATE_SIGNATURE[];

typedef struct RecoveryRec
    {
    BYTE        signature[40];          /*                              */
    WORD        dataTrack;              /* data track number            */
    BYTE        dataHead;               /* data head number             */
    BYTE        diagHead;               /* head number for data         */
    BYTE        dl;                     /* DOS disk letter              */
    } RecoveryRec;

/*----------------------------------------------------------------------*/
/* IsDir() return error states                                          */
/*----------------------------------------------------------------------*/

#define ILLEGAL_DIR_ENTRY       0
#define LEGAL_DIR_ENTRY         1
#define NOT_USED_DIR_ENTRY      2

/*----------------------------------------------------------------------*/
/* Maximum size of a DOS filename (8 + 3)                               */
/*----------------------------------------------------------------------*/

#define DOS_FILENAME_LENGTH     11

/*----------------------------------------------------------------------*/
/* Types of directory entries                                           */
/*----------------------------------------------------------------------*/

#define DIR_EOD         ((BYTE) 0x00)   /* End-Of-Directory entry       */
#define DIR_ERASED      ((BYTE) 0xE5)   /* Erased entry                 */

/*----------------------------------------------------------------------*/
/* DIRSEARCHREC                                                 */
/*      This structure allows us to perform FAST searches by reading    */
/*      directly from the disk.                                         */
/*----------------------------------------------------------------------*/

#define DIR_SEARCH_FILENAME     1       /* Search by filename           */
#define DIR_SEARCH_SCN          2       /* Search by SCN                */

typedef struct DIRSEARCHREC
    {
    BYTE filename[DOS_FILENAME_LENGTH + 1];     /* Filename             */
    CLUSTER scn;                                /* Scn                  */
    BYTE type;                                  /* Type of file         */
    } DIRSEARCHREC;





/*----------------------------------------------------------------------*
 * DIRSCAN								*
 *									*
 * The following section defines constants, structures and prototypes	*
 * used with the DIRSCAN module.  Refer to the DIRSCAN.DOC Word		*
 * document which contains a detailed description of DIRSCAN.		*
 *----------------------------------------------------------------------*/


#define	LAST_LONG		0x40	// Bit set for last long file entry

/************************************************************************
 * Message values sent to the callback function				*
 ************************************************************************/


#define	DSM_BASE	0x1000
#define	DSM_INIT	DSM_BASE	// Called prior to starting the dir
					// scan
#define	DSM_NEWDIR	DSM_BASE + 1	// Called when a new directory,
					// lParam points to the name of the
					// directory in UNICODE (may be LFN)
#define	DSM_NEWFILE	DSM_BASE + 2	// Called when processing a new file,
					// lParam points to the name of the
					// file in UNICODE.
#define	DSM_MATCH	DSM_BASE + 3	// While performing a DirectoryWalk a
					// directory matching the specified
					// criteria has been found.  The
					// matching LPDIRHEADERREC is passed
					// in lParam.  The criteria is passed
					// in wParam.
#define	DSM_DONEDIR	DSM_BASE + 4	// Called after each directory is
					// completed scanning and analyzing.
#define	DSM_ERROR	DSM_BASE + 5	// An error has been detected, the
					// error code will be passed in
					// wParam.
#define DSM_TRAVERSEDIR	DSM_BASE + 6    // Called before DirScan is about to
					// traverse a directory.  Returing
					// DSR_OK will allow the traversal,
					// while DSR_SKIP will skip it.
#define DSM_BREAKCHECK	DSM_BASE + 7	// This is called when the dirscan
					// is in the middle of CPU or DISK
					// intensive usage and wants to allow
					// the caller an opportunity to
					// update the screen, check for user
					// input, etc.  A return value of
					// DSR_ABORT will cause DIRSCAN to
					// ABORT.
#define	DSM_UNMOVABLEFILE DSM_BASE + 8	// Called when processing an
					// unmovable file.
#define	DSM_DONE	DSM_BASE + 9	// Done processing
#define DSM_ROOT        DSM_BASE + 10	// Done reading root directory

/************************************************************************
 * Error values in wParam of a DSM_ERROR message.			*
 ************************************************************************/

#define	DSE_LOSTCLUSTER		0x0100	// Found a lost cluster.  lParam
					// points to DSError structure.
#define	DSE_CROSSLINK		0x0200	// Cross-link found.  lParam points
					// to DSError structure.
#define	DSE_ALLOCATIONERROR	0x0300	// File's size does not match the
					// allocated clusters.  lParam points
					// to DSError structure.
#define	DSE_BADENTRY		0x0400	// Illegal directory entry.  Entry
					// pointed to by lParam.
#define	DSE_ERRORREADING	0x0500	// Error reading cluster.  lParam
					// points to DSError structure.
#define	DSE_ERRORWRITING	0x0600	// Error writing a directory.  lParam
					// points to DSError structure.
#define	DSE_BADCLUSTER		0x0700	// Starting cluster is invalid.
					// lParam points to DSError
					// structure.
#define DSE_BADCLUSTERCHAIN     0x0800	// Files's cluster chain is invalid.
					// lParam points to DSError
					// structure.
#define	DSE_ENTRYAFTEREOD	0x0900	// The entry occurs after the EOD
					// character.  lParam points to
					// DSError structure.
#define	DSE_ILLEGALDIR		0x0A00	// Illegal directory structure (too
					// many or too few . & .. entries).
					// lParam points to DSError
					// structure.
   /*************************************
    * Error sub codes for DSE_ILLGALDIR *
    *************************************/

    #define DSSE_NODOT		    0x0A01 // No . entry.
    #define DSSE_NODOTDOT	    0x0A02 // No .. entry.
    #define DSSE_EXTRADOT	    0x0A03 // Extra . entry
    #define DSSE_EXTRADOTDOT	    0x0A04 // Extra .. entry
    #define DSSE_BADMSEC	    0x0A05 // The d_CreateMSec field is not
				    	   // valid.
    #define DSSE_BADATTR	    0x0A06 // Bad attribute combination
    #define DSSE_HASSPACES	    0x0A07 // Spaces between chars.
    #define DSSE_BADCURRENTSCN	    0x0A08 // . entry has invalid scn
    #define DSSE_BADPARENTSCN       0x0A09 // .. entry has invalid scn
    #define DSSE_DUPLICATEENTRIES   0x0A0A // There are duplicate director
				    	   // entries.  lParam points to DSError
					   // structure.
    #define DSSE_BADMODIFYDATE      0x0A0B // Invalid modify date
    #define DSSE_BADMODIFYTIME      0x0A0C // Invalid modify time
    #define DSSE_BADCREATEDATE      0x0A0D // Invalid create date
    #define DSSE_BADCREATETIME      0x0A0E // Invalid create time
    #define DSSE_BADACCESSDATE      0x0A0F // Invalid access date
    #define DSSE_BADACCESSDATERANGE 0x0A10 // Access Date is out of valid range
    #define DSSE_DIRTOOBIG          0x0A11 // Dir has < 64k entries



#define	DSE_BADLFN		0x0B00	// The entry appears to be a LFN but
					// there are problems with the entry.
					// lParam points to the DSError
					// structure.
   /**********************************
    * Error sub codes for DSE_BADLFN *
    **********************************/

    #define DSSE_TRUNCATED	0x0B01	// Truncated LFN entry.
    #define DSSE_BADLFNATTR	0x0B02	// Attribute field != 0x0F
    #define DSSE_BADTYPE	0x0B03	// Type field != 0
    #define DSSE_BADSCN		0x0B04	// Starting cluster != 0
    #define DSSE_BADNAME	0x0B05	// Bad characters in file name.
    #define DSSE_BADCRC		0x0B06  // The CRC an LFN does not match
    #define DSSE_ORPHAN		0x0B07	// An orphaned LFN has been found.
    #define DSSE_ILLEGALCHAR 0x0B08 // Illegal LFN chars
					// lParam points to the DSError
					// structure.
					// the first LFN's CRC.

#define DSE_FATMISMATCH         0x0C00  // 1st & 2nd Copy of FAT don't match

#define DSE_MEMORY		0xF100	// Memory management error

   /**********************************
    * Error sub codes for DSE_MEMORY *
    **********************************/

    #define DSSE_ALLOC_FAILED	0xF101	// MemAlloc failed
    #define DSSE_LOCK_FAILED	0xF102	// MemLock failed
    #define DSSE_REALLOC_FAILED 0xF103  // MemReAlloc failed


#define DSE_NOROOTDIR           0x0D00  // FAT32 only: Root chain contains
                                        // no valid entries.  NDD will create
                                        // a new root directory chain.

/************************************************************************
 * Return codes from the call back and directory scan functions.	*
 ************************************************************************/

#define	DSR_OK			0x0000	// Continue as normal
#define	DSR_ABORT		0x0001	// Stop processing, free all memory,
					// and exit.
#define	DSR_STOP		0x0002	// Stop processing and exit,
					// preserving any work done up to
					// this time.
#define	DSR_RESTART		0x0003	// Stop processing, free all memory,
					// then start over.
#define DSR_PHYSICAL_ERROR	0x0004	// Physical disk error
#define DSR_SKIP		0x0005  // Skip to next directory (returned
					// by callback with DSM_TRAVERSEDIR
					// message.
#define DSR_RETRY		0x0006	        // Retry the last operation

/************************************************************************
 * Dir scan input flags							*
 ************************************************************************/

                                	// Analyze the directory structure.
					// This involves verifying that
					// directory entries are valid and
					// performing cross link checking iff
					// a backfat and/or bit fat have been
					// allocated.
#define	DSF_ANALYZE	         0x00000001
                                	// Send the DSM_NEWFILE message for
				        // each file encountered.
#define	DSF_GIVENAMES	         0x00000002
                                	// Insert this directory as the child
				        // of hInsertionPoint.
					// If hInsertionPoint already has a
				        // child then this will moved down
				        // and hDir will acquire the
				        // firstInDir flag.
#define	DSF_INSERTCHILD	         0x00000004
                                	// Insert this directory as a sibling
				        // of hInsertionPoint placing it
				        // before hInsertionPoint.  If
				        // hInsertionPoint has the firstInDir
				        // flag this will pass to hDir.
#define	DSF_INSERTBEFORE         0x00000008
                                	// Insert this directory as a sibling
				        // of hInsertionPoint placing it
#define	DSF_INSERTAFTER	         0x00000010
					// Fix spaces in file names which are
					// legal in OS/2.
#define DSF_FIXSPACESINFILENAMES 0x00000020
                                	// Doesn't update the back FAT.
					// This is used by NDD when it is
					// converting files to dirs.  It
					// doesn't want back fat checking
					// or updates.  With this set, cross-
					// link files and lost clusters will
					// not be found.
#define DSF_NOBACKFAT	         0x00000040
                                        // Use FindFirst/FindNext instead of
					// sector level I/O to scan the tree.
#define DSF_USEFINDFIRST         0x00000100
                                	// Include LFNs in the DIRHEADERREC.
#define	DSF_INCLUDELFN	         0x00000200
                                        // For NDD.  Skips lost cluster testing.
#define DSF_SKIPLOSTCLUSTERTEST  0x00000400
                                        // when DSF_USEFINDFIRST is set, only
                                        // short names are stored, specifying
                                        // this flag will force dirscan to
                                        // allocate and store LFN's
                                        // NOTE: the handle to LFN is stored
                                        // in d_CreateTime and d_CreateDate
#define  DSF_FINDFIRST_LFN       0x00000800
                                        // Read in the 2nd FAT and compare
                                        // it with the first to see if
                                        // there are ANY differences.
#define  DSF_FATCOMPARE          0x00001000

                                        // For Speedisk: Clear 2 hi bits of
										// attibute for DIRECTORY entries, also
										// zeros out the size field.
#define DSF_CLEARDIRSIZE		 0x00002000

                                        // For NDD: Check file dates & times.
#define DSF_CHECKTIMES           0x00004000


/************************************************************************
 * Dir Scan call back function						*
 ************************************************************************/

typedef LRESULT (CALLBACK* DSPROC)(struct DIRSCANREC far *lpDS,
				   UINT   uMessage,
                   DWORD  wParam,
				   LPARAM lParam );

/************************************************************************
 * File specification structure (for Speedisk Unmovable Files		*
 ************************************************************************/

#define MAX_IN_FILESPEC_LIST	 10
#define	LFN_MAX			256

typedef struct FILESPECLISTREC
    {
    WCHAR 	fileSpec[MAX_IN_FILESPEC_LIST][LFN_MAX];
    BYTE 	unwild[MAX_IN_FILESPEC_LIST][LFN_MAX];
    BYTE 	byCount;
    } FILESPECLISTREC, _huge *HPFILESPECLISTREC, FAR *LPFILESPECLISTREC, NEAR *NPFILESPECLISTREC;

/************************************************************************
 * Dir Scan header 							*
 * 	Template for calling into the Dir Scan engine.			*
 ************************************************************************/

typedef struct DIRSCANREC
    {
    UINT		uVersion;	// Version of this header
    struct DISKREC far *lpDisk;		// Diskrec for this scan
    HGLOBAL		hDirHead;	// The head of the dir tree
    DWORD		dwFlags;	// Flags for controlling the scan
    struct FAT far     *lpFAT;		// FAT pointer
    struct FAT far     *lpBackFAT;	// Back FAT pointer, NULL if none
    DSPROC		lpCallBack;	// The callback function
    BYTE       huge     *hpBitFAT;       // The traversal bit FAT.
    DWORD                dwBitFATSize;    // Size of the bit FAT.
    LPFNDISKBLOCKDEVICEPROC lpDiskBlockDevice;
					// Use this to hook the disk block
					// device calls.  If this is NULL
					// the default disk block device call
					// is used.
    LPFILESPECLISTREC	lpUnmovable;	// List of unmovable files
    HGLOBAL		hLastDir;	// Last directory scanned
    HGLOBAL		hDirTable;	// Handle to table of directory handles
    DWORD		dwNumDirs;	// Number of directories stored in
					// hDirTable.
    DWORD		dirTableSize;	// Size of dirTable in bytes
    DWORD		dwBadClusters;	// Number of Bad clusters
    DWORD		dwFileClusters;	// Number of File clusters
    DWORD		dwDirClusters;	// Number of Directory clusters
    DWORD		dwFreeClusters;	// Number of Free clusters
    DWORD		dwHiddenClusters;// Number of Hidden clusters
    DWORD		dwUnmovableClusters;
					// Number of Unmovable clusters
    DWORD		dwFragmentCount;// Number of fragments
    DWORD		dwUserData;	// For user's use
    DWORD		dwFragments;	// The total number of file
					// fragments.
    DWORD		dwFiles;        // The total number of files
					// (excluding hidden files).
    DWORD		dwUnmovableFiles;// The total number of unmovable
					// files.
    DWORD		dwHiddenFiles;  // The total number of hidden files.
    UINT		uFragmentation;	// The fragmentation of the disk.
					// Computed
					// by the formula (uFiles -
					// uUnmovableFiles)
					// * 100L / uFragments.
    HGLOBAL		hOpenFileInfo;  // Open file information table
    UINT		uOpenFiles;	// Number of open files on disk.
    HGLOBAL		hClusterBuffer; // Buffer used to write out
					// directories whose fields
					// need to be cleaned up before they
					// can be written (i.e. stripping the
					// bits off attribute field).
					// This buffer is optional, if it is
					// set to NULL, the directory will
					// be written out directly.
    UINT                uUnmovableAttrs;// Attributes for unmovable files
    CLUSTER             clAppLog[2];	// Clusters defining the app log area
    } DIRSCANREC, _huge *HPDIRSCANREC, FAR *LPDIRSCANREC, NEAR *NPDIRSCANREC;


/*----------------------------------------------------------------------*
 * These flags are set within the attribute field of DIRENTRYREC, they	*
 * should be masked off before the directory is written to disk.	*
 *----------------------------------------------------------------------*/
#define	CHANGED_ENTRY  ((BYTE) 0x80)
#define OPEN_ENTRY     ((BYTE) 0x40)
#define SYSTEM_BITS    ((BYTE) 0x3F)
#define	LFN_BITS	((BYTE) 0x0F)
#define LFN_MASK    ((BYTE) 0x3F)


/*----------------------------------------------------------------------*/
/* DirEntryRec								*/
/*	Template of a directory entry, as it appears on the disk.	*/
/*----------------------------------------------------------------------*/

typedef struct DIRENTRYREC
    {
    BYTE	d_name [8];		// Name, space padded on right
    BYTE  	d_ext  [3];		// Ext, space padded on right
    BYTE  	d_attr;			// Bit-encoded fields
    BYTE	d_NT;			// Used by NT
    BYTE	d_CreateMSec;		// Creation ms - # of 10 ms intervals
					// in 2 sec create time.
    WORD	d_CreateTime;		// Creation time
    WORD	d_CreateDate;		// Creation date
    WORD	d_LastAccess;		// Last access date
    WORD	d_scnHi;		// Hi word of SCN
    WORD  	d_time;			// Encoded
    WORD  	d_date;			// Encoded
    WORD  	d_scnLo;  		// Lo word of starting cluster number
    DWORD 	d_size;			// Size in bytes
    } DIRENTRYREC, DirEntryRec, _huge *HPDIRENTRYREC, _huge *LPDIRENTRYREC,
      NEAR *NPDIRENTRYREC, DirectoryEntryRecord;

typedef const DIRENTRYREC NEAR *NPCDIRENTRYREC;
typedef const DIRENTRYREC FAR *LPCDIRENTRYREC;
typedef const DIRENTRYREC HUGE *HPCDIRENTRYREC;

/************************************************************************
 * LFNEntryRec								*
 * 	Template of a LFN entry, as it appears on the disk.		*
 ************************************************************************/

typedef struct LFNENTRYREC
    {
    BYTE	byOrd;			// The order of this entry in the
					// sequence of long dir entries
					// associated with the short dir
					// entry at the end of the loing dir
					// set.
    WORD	wName1_5[5];		// Characters 1-5 of the long name
					// sub-component in this dir entry.
    BYTE	byAttr;			// Must be FA_LONG.
    BYTE	byType;			// If zero, indicates a directory
					// entry which is a sub-component of
					// a long name.  NOTE: Other values
					// reserved for future exentsions.
					// Non-zero implies other dir entry
					// types.
    BYTE	byCheckSum;		// Checksum of name in the short dir
					// entry at the end of the long dir
					// set.
    WORD	wName6_11[6];		// Characters 6-11 of the long name
					// sub component in the dir entry.
    WORD	wFirstCluster;		// Must be ZERO.  This is an artifact
					// of the FAT "first cluster" and
					// must be zero for compatibility
					// with existing disk utilities.
					// It's meaningless in the context of
					// a LFN.  (Subject to change.)
    WORD	wName12_13[2];		// Characters 12-13 of the long name
					// sub component in the dir entry.
    } LFNENTRYREC, _huge *HPLFNENTRYREC, FAR *LPLFNENTRYREC,
      NEAR *NPLFNENTRYREC;


/*----------------------------------------------------------------------*/
/* DIRHEADERREC								*/
/* 	Structure used as a header for each directory.			*/
/* 	Filler is used to get it to be same size as the structure 	*/
/* 	DirEntryRec. We HAVE to do this, since we put ourselves at the	*/
/*	beginning of the directory and we MUST be aligned so a HUGE	*/
/*	ptr. won't straddle the 64K segment boundary.			*/
/*									*/
/*	Changed 'nextDirSeg' and 'parentsDirSeg' to HGLOBAL, since	*/
/*	in Windows they will have the memory handle, rather than the	*/
/*	segment, as in DOS.						*/
/*----------------------------------------------------------------------*/

typedef struct  DIRHEADERREC
    {
    DWORD           count;          // Number of directory entries
    CLUSTER         scn;            // Starting Cluster Number
    BYTE            byFlags;        // Are we dirty?
    HGLOBAL         hNextDir;       // Next directory in list
    HGLOBAL         hParentDir;     // Backwards pointer to parent
    BYTE            name[13];       // ASCIIZ name of this directory
    DWORD           chainLength;    // Original length of dir.
    BYTE            depth;          // NCD depth
    BYTE            firstInDir;     // NCD flag
    HGLOBAL         hNextSibling;   // Next Sibling handle
    HGLOBAL         hPrevSibling;   // Previous Sibling handle
    HGLOBAL         hPrevDir;       // Double link the list
    DWORD           dwDirTableIndex;// Index into lpDirScan->hDirTable for this dir
    WORD            wFirstUnopt;    // First unoptimized entry (SDW)
    WORD            wLFNFlag;       // Flag controlling LFN structure
    WORD            wLastUnopt;     // Last unoptimized entry (SDW)
    DWORD           dwDirSize;      // Size of this directory (in bytes)
    DWORD           dwSubTreeSize;  // Size of children dirs (in bytes)
    LPDIRENTRYREC   lpFirstDir;     // First dir entry
    BYTE        szLFN[1];           // Variable length LFN
    } DIRHEADERREC, DirHeaderRec, FAR *LPDIRHEADERREC, _huge *HPDIRHEADERREC,
      NEAR *NPDIRHEADERREC;

#define DIRHEADERMINSIZE (((sizeof(DIRHEADERREC) + 15) / 16) * 16)

/*----------------------------------------------------------------------*/
/* Values for byFlags field in DIRHEADERREC                             */
/*----------------------------------------------------------------------*/

#define DIR_DIRTY               ((BYTE) 0x01)
#define DIR_EXACT_MATCH         ((BYTE) 0x02)
#define DIR_OPEN_FILES          ((BYTE) 0x04)
#define DIR_OPEN_FILE_MOVED     ((BYTE) 0x08)

/*----------------------------------------------------------------------*
 * VDirEntryRec (same as NDD's VDirPtr								*
 *									*
 * Structure used to point to a directory entry allocated in virtual	*
 * memory.  hDir is a handle to the VM block, and wEntry is an entry	*
 * number within that block.  wEntry is an array index, not a byte	*
 * offset!  This allows the VM block for a single directory to be	*
 * more than 64K (it is rather stupid to have over 4000 files in a	*
 * single directory, but users do stupid things all the time!).		*
 *									*
 * Note: The structure LOGICALSEGREC was used in a similar way in the	*
 *	 Othello version of Windows NDD.				*
 *----------------------------------------------------------------------*/

typedef struct
    {
    HGLOBAL	hDir;		/* Handle to directory VM block */
    DWORD   dwEntry;     /* Entry number within VM block */
    } VDIRENTRYREC, _huge *HPVDIRENTRYREC, FAR *LPVDIRENTRYREC, NEAR *NPVDIRENTRYREC;

/************************************************************************
 * DSError								*
 * 	Template for the error messages.				*
 ************************************************************************/

typedef struct DSERRORREC
    {
    DWORD       uErrorCode;     // Error code for this error
    DWORD       uSubCode;       // Error sub code for this error
    VDIRENTRYREC    vDirEntry;	   	// VDirEntry information - this is
					// contains information about the
					// location of the sub-dir ENTRY.
    HGLOBAL	    hDir;		// Handle to actual sub-directory
    LPDIRENTRYREC   lpDirEntry;		// Pointer to dir entry (same as
					// what vDirEntry points to).
    LPDIRHEADERREC  lpDirHeader;	// Pointer to header record for
					// directory with this error.
    CLUSTER	    clCluster;		// For errors involving clusters, the
					// cluster causing the problem.
    DWORD	    dwExtra;		// Additional error info.
   } DSERRORREC, _huge *HPDSERRORREC, FAR *LPDSERRORREC, NEAR *NPDSERRORREC;

/*----------------------------------------------------------------------*/
/* Macros								*/
/*	These macros make it easy to move from one DirEntryRec 		*/
/*	to another.							*/
/*									*/
/*	IncDirPtr()	  Moves 'inc' DirEntryRec entries forward	*/
/*	DecDirPtr()	  Moves 'inc' DirEntryRec entries backward	*/
/*----------------------------------------------------------------------*/

#if defined(SYM_PROTMODE) || defined(SYM_32BIT)

#define	IncDirPtr(dirPtr,inc)		((HPDIRENTRYREC) dirPtr) += inc
#define	DecDirPtr(dirPtr,inc)		((HPDIRENTRYREC) dirPtr) -= inc

#else

#define IncDirPtr(dirPtr, inc)          FP_SEG(dirPtr) += (inc) * (sizeof(DIRENTRYREC) / 16)
#define DecDirPtr(dirPtr, inc)          FP_SEG(dirPtr) -= (inc) * (sizeof(DIRENTRYREC) / 16)

#endif

#define FirstDirEntry(dirTablePtr) \
	  ( ((LPDIRHEADERREC)dirTablePtr)->lpFirstDir \
		? ((HPDIRENTRYREC)((LPDIRHEADERREC)dirTablePtr)->lpFirstDir) \
	        : ((HPDIRENTRYREC)((HPBYTE)dirTablePtr + DIRHEADERMINSIZE)) )


/*----------------------------------------------------------------------*
 * Macros for testing Chicago (Windows 4) long filename directory 	*
 * entries.								*
 * 									*
 * These macros are based on the 0.91 FAT: Long Name On-Media Format	*
 * Specification from Microsoft.					*
 *									*
 * For historical reasons many of our programs simply skip entries with	*
 * the volume bit set instead of consistently using these macros.	*
 *----------------------------------------------------------------------*/

					// Test if a dir entry is an
                                        // LFN entry of any kind.
#define IsLongDirEntry(dirPtr) \
    ( ((dirPtr)->d_attr & LFN_MASK) == LFN_BITS && \
       (dirPtr)->d_name[0] != 0xE5 )

                                        // Test if a dir entry is an
                                        // LFN entry dedicated to storing
                                        // a long file name subcomponent.
					// Currently this is the only kind
					// of LFN entry that can occur.
					// Use an LFNENTRYREC pointer to
					// access the entry's fields.
#define IsLongNameDirEntry(dirPtr) \
    ( IsLongDirEntry(dirPtr) && \
      ((LPLFNENTRYREC)(dirPtr))->byType == 0 )

                                        // Test if a dir entry is an
                                        // LFN entry of a type reserved
                                        // for future use by Microsoft.
					// These LFN entries should be
					// left alone.
#define IsLongReservedDirEntry(dirPtr) \
    ( IsLongDirEntry(dirPtr) && \
      ((LPLFNENTRYREC)(dirPtr))->byType != 0 )

                                        // The following macros are for
                                        // Unerase, and are identical to
                                        // the ones above, save that they
                                        // don't check for erased names.
#define IsLongDirEntry2(dirPtr) \
    (((dirPtr)->d_attr & LFN_MASK) == LFN_BITS)

#define IsLongNameDirEntry2(dirPtr) \
    ( IsLongDirEntry2(dirPtr) && \
      ((LPLFNENTRYREC)(dirPtr))->byType == 0 )

#define IsLongReservedDirEntry2(dirPtr) \
    ( IsLongDirEntry2(dirPtr) && \
      ((LPLFNENTRYREC)(dirPtr))->byType != 0 )


                                        // This macro returns the handle
                                        // to the LFN of direntry.
                                        // NOTE: this only works with scans
                                        // using DSF_USEFINDFIRST | DSF_FINDFIRST_LFN
#define GET_FINDFIRST_HLFN(lpDir) (*((HGLOBAL *)(&lpDir->d_CreateTime)))

#define LFNENTRY_ASCII_SIZE    13
#define LFNENTRY_UNICODE_SIZE  (LFNENTRY_ASCII_SIZE * sizeof(WORD))


BYTE SYM_EXPORT WINAPI LFNComputeCheckSum(LPDIRENTRYREC lpDirEntry);
void SYM_EXPORT WINAPI LFNBuildName	 (LPLFNENTRYREC lpEntryRec, LPSTR lpszLFN );
UINT SYM_EXPORT WINAPI LFNEntryToAscii	 (LPLFNENTRYREC lpEntryRec, LPSTR lpszLFN);
void SYM_EXPORT WINAPI LFNAsciiToEntry	 (LPLFNENTRYREC lpDirEntry, LPSTR lpszLFN);



/************************************************************************
 * Prototypes								*
 ************************************************************************/

LRESULT SYM_EXPORT WINAPI DirectoryScan(LPDIRSCANREC lpDirScan, VDIRENTRYREC parentDir);
LRESULT SYM_EXPORT WINAPI DirectoryFree(LPDIRSCANREC lpDirScan);
BOOL    SYM_EXPORT WINAPI DirectorySearch(LPDIRSCANREC lpDirScan, HGLOBAL hStartDir, CLUSTER clSCN, CLUSTER clNewSCN, LPDIRENTRYREC lpDirMatch, LPHGLOBAL lphParent);
BOOL    SYM_EXPORT WINAPI DirectoryReturnLFN(VDIRENTRYREC vDir, LPBYTE lpbyLFN);
UINT    SYM_EXPORT WINAPI _DirectoryReturnLFN(LPDIRENTRYREC lpDirEntry, DWORD offset, LPBYTE lpbyLFN);
LRESULT SYM_EXPORT WINAPI DirectoryRead(LPDIRSCANREC lpDirScan,
                      CLUSTER clScn,
				      LPHGLOBAL lphDir);
LRESULT SYM_EXPORT WINAPI DirectoryReadEx(LPDIRSCANREC lpDirScan,
                      CLUSTER clScn,
					  LPHGLOBAL lphDir,
					  LPBYTE lpLFN,
                      VDIRENTRYREC parentDir);
LRESULT SYM_EXPORT WINAPI DirectoryWrite(LPDIRSCANREC lpDirScan,
					 HGLOBAL hDir);
LRESULT SYM_EXPORT WINAPI DirectoryScanSubTree(LPDIRSCANREC lpDirScan, VDIRENTRYREC parentDir, UINT uDepth, BOOL bFirstBorn, LPHGLOBAL lphNewDir, LPDWORD lpdwSubTreeSize);
BOOL SYM_EXPORT WINAPI DirectoryLinkIntoChain(LPDIRSCANREC lpDirScan,
					HGLOBAL hPrevDir,
					HGLOBAL hDir);
BOOL SYM_EXPORT WINAPI DirectoryUnlinkFromChain(LPDIRSCANREC lpDirScan,
					HGLOBAL hPrevDir,
					HGLOBAL hDir);
LRESULT SYM_EXPORT WINAPI DirectoryMassageEntry(LPDIRSCANREC lpDirScan,
					VDIRENTRYREC dir,
					DIRENTRYREC far *dirPtr,
					BOOL bIsRoot);
LRESULT SYM_EXPORT WINAPI DirectoryCheckLFNs(LPDIRSCANREC lpDirScan, LPDSERRORREC lpDSError, BOOL bCheckAll);
BOOL SYM_EXPORT WINAPI NameHasSpaces(LPDIRSCANREC lpDirScan,
				     DIRENTRYREC far *dirPtr);
BOOL SYM_EXPORT WINAPI           DirectoryIsLegal                 (LPDIRSCANREC lpDirScan, DIRENTRYREC far *dirPtr, CLUSTER scn, CLUSTER parentsScn, LPDWORD lpuSubCode);
WORD SYM_EXPORT WINAPI EntriesValidAfterEOD(LPDIRSCANREC lpDirScan,
			  DIRHEADERREC far *dirTablePtr,
			  BOOL bMakeRepairs, WORD activeFiles);
VDIRENTRYREC SYM_EXPORT WINAPI FindDirEntryGivenCluster(LPDIRSCANREC lpDirScan, CLUSTER scn);
BOOL SYM_EXPORT WINAPI IsStartOfDirChain(LPDIRSCANREC lpDirScan, CLUSTER cluster, CLUSTER parentsScn);
BOOL SYM_EXPORT WINAPI DirectoryIsUnmovableFile(LPDIRSCANREC lpDirScan, DIRENTRYREC far *dirPtr, DWORD dwEntryInDir, BOOL bIsRoot);
BOOL SYM_EXPORT WINAPI DirectoryIsValidTime( WORD wEncodedTime, BOOL bZeroIsValid );
BOOL SYM_EXPORT WINAPI DirectoryIsValidDate( WORD wEncodedDate, BOOL bZeroIsValid );
BOOL SYM_EXPORT WINAPI DirectoryIsValidLastAccess(HPCDIRENTRYREC lpDirEntry, BOOL bZeroIsValid);
BOOL SYM_EXPORT WINAPI DirectoryHasValidDateTime( LPDIRSCANREC lpDirScan, HPCDIRENTRYREC lpDirEntry, LPUINT lpuSubCode);
BOOL SYM_EXPORT WINAPI DirectoryIsValidVolumeLabel( HPCDIRENTRYREC lpDirEntry );
BOOL SYM_EXPORT WINAPI DirectoryIsLegalName( HPCDIRENTRYREC lpDirEntry );
void SYM_EXPORT WINAPI DirectoryComputeLFNBasisName( LPSTR lpszLFN, LPSTR lpszShortName );
BOOL SYM_EXPORT WINAPI DirectoryCreateUniqueLFNBasisName( LPDIRSCANREC lpDirScan, LPDSERRORREC lpDSError, HPDIRENTRYREC   lpShortEntry, UINT iShortEntryIndex );

LPDIRENTRYREC SYM_EXPORT WINAPI VDirLock(VDIRENTRYREC dir);
void SYM_EXPORT WINAPI VDirUnlock(VDIRENTRYREC dir);

void SYM_EXPORT WINAPI DirectoryFormatErrorString( LPDIRSCANREC lpDirScan, LPDSERRORREC lpDSError, LPSTR lpszBuffer, UINT uBufSize );

LRESULT SYM_EXPORT WINAPI DirectoryEntryFromHandle(LPDISKREC lpDisk, HGLOBAL hHandle,LPDIRENTRYREC lpDirEntry);
void SYM_EXPORT WINAPI DirectoryGetNthHandle(LPDIRSCANREC lpDirScan,
                                             DWORD dwCount,
                                             HGLOBAL * lpHandle);


LPDIRHEADERREC SYM_EXPORT WINAPI DirectoryGetNthDirHeader(LPDIRSCANREC lpDirScan,
                                                          DWORD dwCount,
                                                          HGLOBAL * lpHandle);

DWORD  SYM_EXPORT WINAPI DirectoryGetDirIndex(LPDIRSCANREC lpDirScan,HGLOBAL hDir);
VDIRENTRYREC SYM_EXPORT WINAPI DirectoryHandleToVDir(LPDISKREC lpDisk, HGLOBAL hDir);
BOOL SYM_EXPORT WINAPI  DirectoryGetLFNPath(LPDIRSCANREC lpDirScan,
                                            HGLOBAL hDir,
                                            LPSTR lpszPath,
                                            BOOL bUseLFNs);

BOOL SYM_EXPORT WINAPI  DirectoryIndexGetLFNPath(LPDIRSCANREC lpDirScan,
                                                 DWORD dwIndex,
                                                 LPSTR lpszPath,
                                                 BOOL bUseLFNs);

DWORD SYM_EXPORT WINAPI DirectoryHeaderSize(HANDLE hDir);
WORD  SYM_EXPORT WINAPI DirectoryClearBitFATChain(LPDIRSCANREC lpDirScan, CLUSTER clSCN);

BOOL SYM_EXPORT WINAPI DirectoryFileSpecCompare(HPDIRENTRYREC dirptr, LPSTR fileSpec1, LPSTR fileSpec2 );

CLUSTER SYM_EXPORT WINAPI DirGetSCN(LPDISKREC lpDisk, LPDIRENTRYREC lpDirEntry);
void SYM_EXPORT WINAPI DirSetSCN(LPDISKREC lpDisk, LPDIRENTRYREC lpDirEntry, CLUSTER clCluster);

BOOL SYM_EXPORT WINAPI  IsRootSCN(LPDISKREC lpDisk, CLUSTER scn);
BOOL SYM_EXPORT WINAPI IsBadFatEntry(LPDIRSCANREC  lpDirScan, CLUSTER clCluster );


/*----------------------------------------------------------------------*
 * Routines to handle open files (usually through DOS's SFT's)		*
 *----------------------------------------------------------------------*/
                                        // FileIsOpen2() is in symkrnl, dsk_accs.c
BOOL SYM_EXPORT WINAPI FileIsOpen2(LPCSTR lpszFullPathName);
                                        // FileIsOpen() is in symfat, fat_sft.c
BOOL SYM_EXPORT WINAPI FileIsOpen     (LPDIRSCANREC lpDirScan, LPBYTE szPackedName, CLUSTER clScn);
BOOL SYM_EXPORT WINAPI IsFileOpenInDir(LPDIRSCANREC lpDirScan, HGLOBAL hDir);

/*----------------------------------------------------------------------*/
/* DIRSCAN: End of dirscan stuff					*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#define CHKDSK_NO_ERR                   0
#define CHKDSK_READ_ERR                 110
#define CHKDSK_ROOT_DIR_READ_ERR        111
#define CHKDSK_DIR_READ_ERR             112
#define CHKDSK_WRITE_ERR                120
#define CHKDSK_NO_MEM_ERR               130
#define CHKDSK_LOST_CLUSTERS            -11
#define CHKDSK_FATS_NOT_EQUAL           -10
#define CHKDSK_INVALID_FAT_SIGNATURE    -9
#define CHKDSK_CANNOT_INIT_DRIVE        -8
#define CHKDSK_FILE_INVALID_SCN         -7
#define CHKDSK_DIR_INVALID_SCN          -6
#define CHKDSK_CROSSLINKED_FILE         -5
#define CHKDSK_CROSSLINKED_DIR          -4
#define CHKDSK_ALLOCATION_MISMATCH      -3
#define CHKDSK_CYCLIC_DIRECTORY         -2
#define CHKDSK_ALLOCATION_ERR           -1

#define	ENUM_NORMAL_FILE		0
#define	ENUM_MEMORY_FILE		1
#define	ENUM_UNMOVABLE_FILE		2

typedef struct CheckFATRec
    {
    DISKREC             disk;
    LPSTR               fat;
    LPSTR               bitFat;
    LPDIRENTRYREC       tree;
    UINT                clustersChecked;
    UINT                allocatedClusters;
    int                 errorCode;
    } CheckFATRec;

/*----------------------------------------------------------------------*
 * This record must be a multiple of 16 bytes                           *
 *----------------------------------------------------------------------*/

typedef struct ChkDskDirTreeRec
    {
    BYTE name[8];                       /* name of directory, 8 bytes */
    BYTE extension[3];                  /* extension,         3 bytes */
    BYTE reserved;                      /* flags,             1 byte  */
    WORD scn;                           /* startin cluster,   2 bytes */
    WORD reserved2;                     /* pointer to child   2 bytes */
    } ChkDskDirTreeRec;                 /* TOTAL:            16 bytes */

/*----------------------------------------------------------------------*/
/* Prototypes                                                           */
/*----------------------------------------------------------------------*/
BOOL  SYM_EXPORT WINAPI   DiskGetFileSCN(LPSTR lpFilename,LPCLUSTER lpSCN,LPDISKREC lpdisk,DWORD dwCharSet);

VOID SYM_EXPORT WINAPI DiskGetDriveList (LPSTR lpUnformatted, LPSTR lpFormatList,
                                     UINT uFormatListSize);
BYTE SYM_EXPORT WINAPI  DiskAbsOperation (BYTE command, LPABSDISKREC lpAbsDisk);
#ifdef SYM_WIN32
DWORD SYM_EXPORT WINAPI DiskGetLastIOSAbsOpError();
#endif
BYTE SYM_EXPORT WINAPI  DiskReadMBR (LPABSDISKREC lpAbsDisk);
BYTE SYM_EXPORT WINAPI  DiskWriteMBR (LPABSDISKREC lpAbsDisk);
#if defined(SYM_DOS)
    extern  LPVOID _glpAbsBuffer;
    #define DiskAbsBufferRegister(h,lpR,lpP)    (_glpAbsBuffer = lpR)
    #define DiskAbsBufferUnregister(h)          (_glpAbsBuffer = NULL)
    #define DiskAbsBufferGet(lplpR,lplpP)       (*lplpP = _glpAbsBuffer)
#else
    BOOL SYM_EXPORT WINAPI  DiskAbsBufferRegister (HINSTANCE hInstance, LPVOID lpRealBuffer, LPVOID lpProtBuffer);
    BOOL SYM_EXPORT WINAPI  DiskAbsBufferUnregister (HINSTANCE hInstance);
    BOOL SYM_EXPORT WINAPI  DiskAbsBufferGet (LPVOID FAR *lplpRealBuffer, LPVOID FAR *lplpProtBuffer);
#endif

DWORD SYM_EXPORT WINAPI  ConvertPhySectorToLong(LPCABSDISKREC lpAbsDisk,
                                 DWORD head, DWORD cylinder, DWORD sector);
VOID SYM_EXPORT WINAPI   ConvertLongToPhySector(LPCABSDISKREC lpAbsDisk, DWORD offset,
                                 DWORD FAR * headPtr, DWORD FAR * cylinderPtr, DWORD FAR * sectorPtr);
DWORD SYM_EXPORT WINAPI  DiskGetSectorStartOffset (LPCABSDISKREC lpAbsDisk,
                                LPCPARTENTRYREC partPtr);
DWORD SYM_EXPORT WINAPI  DiskGetSectorEndOffset (LPCABSDISKREC lpAbsDisk,
                                 LPCPARTENTRYREC partPtr);
BOOL SYM_EXPORT WINAPI   DiskGetPartitionTableEntry(BYTE dn,
                                UINT uEntry,
                                LPPARTENTRYREC lpPartEntry,
                                LPABSDISKREC lpAbsDisk);
BOOL SYM_EXPORT WINAPI   DiskRecToFloppyAbsDiskRec (LPCDISKREC diskPtr, LPABSDISKREC lpAbsDisk);
BYTE SYM_EXPORT WINAPI   DiskGetPhysicalInfo (LPABSDISKREC lpAbsDisk);
#ifdef SYM_WIN32
BYTE SYM_EXPORT WINAPI   DiskGetPhysicalInfoEx (LPABSDISKREC lpAbsDiskTranslated, LPABSDISKREC lpAbsDiskActual);
#endif

#if defined(SYM_PROTMODE) || defined(SYM_DOS)
BYTE SYM_EXPORT WINAPI DiskGetPhysicalInfoBasic(LPABSDISKREC lpAbsDisk);
#endif


BYTE SYM_EXPORT WINAPI   _DiskGetPhysicalInfo (LPABSDISKREC lpAbsDisk);
BYTE SYM_EXPORT WINAPI   DiskGetHDParamTable(BYTE dn, HDParamTable FAR *driveParamTable);
VOID SYM_EXPORT WINAPI   DiskUnscrambleBIOSCoding (WORD encodedValue, DWORD FAR *  cylinderPtr, DWORD FAR *  sectorPtr);
DWORD SYM_EXPORT WINAPI  DiskScrambleBIOSCoding (DWORD track, DWORD sector);
BOOL SYM_EXPORT WINAPI   DiskResetAccessFlag (BYTE dl);
BOOL SYM_EXPORT WINAPI   DiskAccessFlagAddr (BYTE dl, LPBYTE FAR* lplpFlag);
void SYM_EXPORT WINAPI   DiskFreeAccessFlagAddr (LPBYTE FAR* lplpFlag);
UINT SYM_EXPORT WINAPI   DiskBlockDevice98 (UINT wCommand, LPDISKREC lpDisk);
UINT SYM_EXPORT WINAPI   DiskLockLogicalVolume(BYTE dl, BYTE byLockLevel, BYTE byPermissions);
UINT SYM_EXPORT WINAPI   DiskLockPhysicalVolume(BYTE byDriveNum, BYTE byLockLevel, BYTE byPermissions);
UINT SYM_EXPORT WINAPI   DiskUnlockLogicalVolume(BYTE dl);
UINT SYM_EXPORT WINAPI   DiskUnlockPhysicalVolume(BYTE byDriveNum);
UINT SYM_EXPORT WINAPI	 DiskEnumerateOpenFiles(BYTE byDriveNum, LPBYTE lpszFileName, UINT uFileIndex);
UINT SYM_EXPORT WINAPI	 DiskEnumerateMemoryFiles(BYTE byDriveNum, LPBYTE lpszFileName, UINT uFileIndex, LPWORD lpwType, LPWORD lpwOpenMode);
UINT SYM_EXPORT WINAPI	 DiskGetWindowsSwapFile(LPBYTE lpszFileName);
BOOL SYM_EXPORT WINAPI	 DiskPollWrites(BYTE byDriveNum);
UINT SYM_EXPORT WINAPI   DiskGetCurrentLockState(BYTE byDriveNum, LPWORD lpwLockLevel, LPWORD lpwLockPermissions);
UINT SYM_EXPORT WINAPI   DiskMuckWithSwapFile(BOOL bStart);
void SYM_EXPORT WINAPI   ShortDiskRecToLongDiskRec(LPSHORTDISKREC lpsDr, LPDISKREC lpDiskPtr);
BOOL SYM_EXPORT WINAPI   CanSupportExtendedIO();
BYTE SYM_EXPORT WINAPI   DiskIsExtendedInt13Drive(LPCABSDISKREC lpAbsDisk);
UINT SYM_EXPORT WINAPI   DiskBlockDevice (UINT wCommand, LPDISKREC lpDisk);
UINT SYM_EXPORT WINAPI   _DiskBlockDevice(UINT wCommand, BYTE byIsBigFooted,
                                        BYTE byDrive, DWORD dwStartSector,
                                        UINT wSectorsThisCall, DWORD dwBigFootSegSel,
                                        LPBYTE lpDosBuffer, Int13Rec FAR *lpInt13Package);
UINT SYM_EXPORT WINAPI   DiskBlockAllocateBuffer(void);
UINT SYM_EXPORT WINAPI   DiskBlockDeAllocateBuffer(void);
BOOL SYM_EXPORT WINAPI   DiskBlockUseOldWinCode(BOOL bForceReinit);
UINT SYM_EXPORT WINAPI   DiskGetBlockDeviceError(VOID);
BYTE SYM_EXPORT WINAPI   DiskBlockSetProc( DISKBLOCKPROC lpfnDiskBlockProc );
BOOL SYM_EXPORT WINAPI   DiskBlockClearProc( BYTE byDriveLetter, DISKBLOCKPROC lpfnDiskBlockProc );
VOID SYM_EXPORT WINAPI   DiskBPBToDiskRec (BYTE dl, LPCBPBREC BPBPtr, LPDISKREC diskPtr);
VOID SYM_EXPORT WINAPI   DiskBPBToAbsDiskRec (LPCBPBREC BPBPtr, LPABSDISKREC absDiskPtr);
UINT SYM_EXPORT WINAPI   DiskGetBPBVersion (LPSTR oemSignature, UINT FAR * BPBSizePtr);
VOID SYM_EXPORT WINAPI   DiskDiskRecToBPB (LPCDISKREC diskPtr, LPBPBREC BPBPtr);
UINT SYM_EXPORT WINAPI   DiskBPBTable (UINT wDiskMinorType, LPBYTE lpbyBPBTable, UINT FAR * lpwCount);
UINT SYM_EXPORT WINAPI   DiskBPBFromIndex (UINT wBPBIndex, LPBPBREC lpBPBRec);
BOOL SYM_EXPORT WINAPI   DiskGetHDParams (LPDISKREC diskPtr,
                                          LPABSDISKREC absDiskPtr,
                                          LPBPBREC BPBPtr);
VOID SYM_EXPORT WINAPI   DiskFillInBPB (DWORD BPBhiddenSectors, LPBPBREC BPBPtr,
                                      LPDISKREC diskPtr, LPABSDISKREC absDiskPtr);
BOOL SYM_EXPORT WINAPI   DiskIsValidBPB (LPCBPBREC BPBPtr);
DWORD SYM_EXPORT WINAPI   DiskCalcPhysicalSectors(LPCBPBREC BPBPtr);
BOOL SYM_EXPORT WINAPI   DiskGetDriverTable (BYTE dl, LPDISKREC lpDiskPtr);
BOOL SYM_EXPORT WINAPI   DiskGetDriverTablePtr(BYTE drive, LPSHORTDISKREC FAR *lpDrivePtr);
BOOL SYM_EXPORT WINAPI   DiskFindDPB(BYTE dl, LPDISKREC FAR *lplpDPB);
BOOL SYM_EXPORT WINAPI   DiskIsBigTable (LPCDISKREC diskPtr, DWORD FAR * startOfRootPtr,
                                      DWORD FAR * sectorsPerFatPtr, UINT FAR * okPtr);
BOOL SYM_EXPORT WINAPI   DiskGenericIOCTL(BYTE command, BYTE driveLetter, LPVOID paramBlockPtr);
UINT SYM_EXPORT WINAPI   DiskGenericIOCTLError (void);
UINT SYM_EXPORT WINAPI   DiskTestFormat (BYTE byDrive, LPCBPBREC lpBPB);
UINT SYM_EXPORT WINAPI   DiskIOCTL(BYTE byCommand, WORD wDriveOrHandle, LPVOID lpBuffer, UINT uBufferSize);
UINT SYM_EXPORT WINAPI   DiskIOCTLError(void);
UINT           WINAPI   BIOSInstallInt13Handler (LPINT13REC lpInt13Package);
UINT           WINAPI   BIOSUninstallInt13Handler (void);
VOID SYM_EXPORT WINAPI   DiskMapLogToPhyParams (DWORD sector,
                                                BYTE dl,
                                                LPINT13REC int13PackagePtr);
void SYM_EXPORT WINAPI   DiskLogSectorToPhySector (DWORD sector,
                                                   LPDISKREC diskPtr,
                                                   LPABSDISKREC absDiskPtr);
VOID SYM_EXPORT WINAPI   DiskResetDOS (BYTE dl); /* Letter of drive to reset     */
DWORD SYM_EXPORT WINAPI   DiskComputeDriveSize (LPCDISKREC diskPtr);
DWORD SYM_EXPORT WINAPI   DiskComputeDriveSizeInSectors (LPCDISKREC lpDisk);
BYTE SYM_EXPORT WINAPI   _DiskGetBootDrive(VOID);
BYTE SYM_EXPORT WINAPI   DiskGetBootDrive(VOID);
DWORD SYM_EXPORT WINAPI  DiskGetBootDriveEx(LPBYTE lpbyDrive);
BYTE SYM_EXPORT WINAPI   DiskGetMaxDrive(VOID);
VOID SYM_EXPORT WINAPI   DiskReset(VOID);
DWORD SYM_EXPORT WINAPI   DiskGetSerialNumber (VOID);
BYTE SYM_EXPORT WINAPI   DiskGetAccessFlag(BYTE driveLetter);
BOOL SYM_EXPORT WINAPI   DiskSetAccessFlag(BYTE driveLetter, BYTE accessType);
VOID SYM_EXPORT WINAPI   DiskGetLockedTable(LPBYTE lockedTablePtr);
VOID SYM_EXPORT WINAPI   DiskSetLockedTable(LPBYTE lockedTablePtr);
DWORD SYM_EXPORT WINAPI   ConvertClusterToSector (LPCDISKREC diskPtr, CLUSTER cln);
BOOL SYM_EXPORT WINAPI   DiskIsValidDiskRec (LPCDISKREC diskPtr);
BOOL SYM_EXPORT WINAPI   DiskGetInfo (BYTE dl, LPDISKREC diskRecPtr);
DWORD SYM_EXPORT WINAPI   ConvertSectorToCluster (LPCDISKREC diskPtr, DWORD dwSector);
DWORD SYM_EXPORT WINAPI   ConvertSizeToCluster (LPCDISKREC diskPtr, DWORD size);
UINT SYM_EXPORT WINAPI   DiskSetFormatType (LPCABSDISKREC absDiskRecPtr,
                                      LPDISKETTEPARAMRECORD disketteParamPtr);
BOOL SYM_EXPORT WINAPI   DiskGetDASDType(BYTE drive, LPBYTE DASDTypePtr);
BYTE SYM_EXPORT PASCAL   DiskSetDASDType (BYTE disk,BYTE type);
BOOL SYM_EXPORT WINAPI   DiskExists (BYTE byDl);
BYTE SYM_EXPORT WINAPI   DiskIsPhantom(BYTE dl);
BYTE SYM_EXPORT WINAPI   _DiskIsPhantom (BYTE driveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsBlockDevice (BYTE dl);
BOOL SYM_EXPORT WINAPI   DiskIsAssigned (BYTE dl);
BOOL SYM_EXPORT WINAPI   _DiskIsAssigned (LPCSTR lpSource, LPSTR lpDest);
BOOL SYM_EXPORT WINAPI   DiskIsSubst(BYTE driveLetter);
DWORD SYM_EXPORT WINAPI   DiskIsEncrypted(BYTE driveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsNetwork(BYTE driveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsFixed(BYTE driveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsRemovable(BYTE driveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsHPFS(BYTE dl);
BOOL SYM_EXPORT WINAPI   DiskIsCDRom(BYTE dl);
BOOL SYM_EXPORT WINAPI   DiskIsRAM(BYTE byDriveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsVFATClient(BYTE byDriveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsCompressed(BYTE byDriveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsStacker(BYTE byDriveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsStacker2(BYTE byDriveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsSuperStor(BYTE byDriveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsDoubleSpace(BYTE drive);
BOOL SYM_EXPORT WINAPI   DiskIsDriveSpace( BYTE drive );
BOOL SYM_EXPORT WINAPI   DiskIsDriveSpace3(BYTE dl);
int  SYM_EXPORT WINAPI   DiskGetDriveSpaceVersion(BYTE dl, LPWORD lpVer);
DWORD SYM_EXPORT WINAPI  DiskIsDMF(BYTE dl);

BOOL SYM_EXPORT WINAPI   _DiskIsDoubleSpace(BYTE drive, LPBYTE lpbyHostDriveLetter,
                                   LPWORD lpbSwapped, LPBYTE lpbySequence);
BYTE SYM_EXPORT WINAPI   DiskGetSuperStorSwapDrive(BYTE bySuperStorDrive);
BYTE SYM_EXPORT WINAPI   DiskGetSuperStorHostDrive(BYTE bySuperStorDrive);
BOOL SYM_EXPORT WINAPI   DiskIsInterLink(BYTE drive);
UINT SYM_EXPORT WINAPI   DiskGetType(BYTE dl, LPUINT lpuMajorType, LPUINT lpuMinorType);
UINT SYM_EXPORT WINAPI   DiskGetFloppyType(BYTE dl, UINT FAR * lpwType);
int SYM_EXPORT WINAPI    DiskGetPhysicalType(LPDISKREC diskPtr, LPABSDISKREC absDiskPtr);
UINT SYM_EXPORT WINAPI   DiskGetMask(UINT wMajorType, UINT wMinorType);
BYTE SYM_EXPORT WINAPI   DiskGetFirstRemovableDrive(VOID);
BYTE SYM_EXPORT WINAPI   DiskGetFirstFixedDrive(VOID);
BYTE SYM_EXPORT WINAPI   DiskGetFirstFloppyDrive(VOID);
UINT SYM_EXPORT WINAPI   NetworkInstalled(void);
BOOL SYM_EXPORT WINAPI   NetDirectIOOK(BYTE dl);
BOOL SYM_EXPORT WINAPI   DiskHasBootRecord(BYTE dl);
UINT SYM_EXPORT WINAPI   DiskCheckLBAProblem(BYTE byDriveLetter, LPBOOL lpbHasProblem, LPBYTE lpbyRealDriveLetter);

BOOL SYM_EXPORT WINAPI   DiskCheckForVFAT (void);
BOOL SYM_EXPORT WINAPI   DiskIsVFAT(BYTE dl);

UINT SYM_EXPORT WINAPI   VolumeLabelGet(TCHAR byDriveLetter, LPTSTR lpName);
UINT SYM_EXPORT WINAPI   VolumeLabelSet(BYTE byDriveLetter, BOOL bRemoveOldLabel, LPCSTR lpLabel);
BOOL SYM_EXPORT WINAPI   VolumeSupportsLFN ( LPCTSTR lpVolumePath );

BOOL SYM_EXPORT WINAPI	 DiskReadFSInfo(LPDISKREC lpDisk, LPFILESYSINFO lpFSInfo);
BOOL SYM_EXPORT WINAPI	 DiskWriteFSInfo(LPDISKREC lpDisk, LPFILESYSINFO lpFSInfo);

BOOL SYM_EXPORT WINAPI   DiskGetActiveFAT(LPCDISKREC lpDisk, UINT FAR * lpuActiveFAT);
BOOL SYM_EXPORT WINAPI   DiskNoFATMirrAndSetActiveFAT(LPCDISKREC lpDisk, UINT uActiveFAT);
BOOL SYM_EXPORT WINAPI   DiskGetFATMirrStatus(LPCDISKREC lpDisk, LPBOOL lpbIsFATMirrored);
BOOL SYM_EXPORT WINAPI   DiskEnableFATMirr(LPCDISKREC lpDisk);
BOOL SYM_EXPORT WINAPI   DiskGetSetActFATandMirr(LPCDISKREC lpDisk, DWORD dwValue1, DWORD FAR * lpdwValue2);


					// Main FAT object implicitly used by
					// FATxxx(), Dirxxx(), _ImageSave()
					// functions.
					// Function used to do I/O

CLUSTER SYM_EXPORT WINAPI DiskGetRootSCN(LPDISKREC lpDisk);
BOOL SYM_EXPORT WINAPI DiskSetRootSCN(LPDISKREC lpDisk, CLUSTER clRootSCN);

DWORD SYM_EXPORT WINAPI   FATSize (LPCDISKREC lpDisk);
LPFNDISKBLOCKDEVICEPROC SYM_EXPORT WINAPI FATGetCallBack(LPFAT lpFAT);
void SYM_EXPORT WINAPI   FATSetCallBack(LPFAT lpFAT, LPFNDISKBLOCKDEVICEPROC lpFunc);

UINT SYM_EXPORT WINAPI      FATAlloc (LPCDISKREC lpDisk, LPFAT lpFAT);
UINT SYM_EXPORT WINAPI      FATFree (LPFAT lpFAT);
LPVOID SYM_EXPORT WINAPI    FATLock(LPFAT lpFAT, CLUSTER clCluster, CLUSTER FAR *lpclOffset, BOOL bDirty);
void SYM_EXPORT WINAPI      FATMarkEntireFATDirty (LPFAT lpFAT);
CLUSTER SYM_EXPORT WINAPI   FATGet (LPFAT lpFAT, CLUSTER clCluster);
CLUSTER SYM_EXPORT WINAPI   FATGetRaw(LPFAT lpFAT, CLUSTER clCluster);
CLUSTER SYM_EXPORT WINAPI   FATGetPrev (LPFAT lpFAT, CLUSTER clCluster);
void SYM_EXPORT WINAPI      FATPut (LPFAT lpFAT, CLUSTER clCluster, CLUSTER clNewValue);
void SYM_EXPORT WINAPI      FATPutRaw(LPFAT lpFAT, CLUSTER clCluster, CLUSTER clNewValue);
CLUSTER SYM_EXPORT WINAPI   FATGetNthLink (LPFAT lpFAT, CLUSTER clCluster, CLUSTER clN);
DWORD SYM_EXPORT WINAPI     FATLengthOfChain (LPFAT lpFAT, CLUSTER clStartCluster);
void SYM_EXPORT WINAPI      FATFreeChain (LPFAT lpFAT, CLUSTER clCluster);
DWORD SYM_EXPORT WINAPI     FATFragmentCount (LPFAT lpFAT, CLUSTER clCluster);
UINT SYM_EXPORT WINAPI      FATRead (LPFAT lpFAT, LPCDISKREC lpDisk, UINT uFatCopy);
UINT SYM_EXPORT WINAPI      FATReadSize (LPFAT lpFAT, LPCDISKREC lpDisk, UINT uFatCopy);
UINT SYM_EXPORT WINAPI		FATReadDirect (LPFAT lpFAT, LPCDISKREC lpDisk, UINT uFatCopy);
UINT SYM_EXPORT WINAPI      FATWrite (LPFAT lpFAT);
UINT SYM_EXPORT WINAPI      FATReadImage (LPFAT lpFAT, LPSTR lpszFileName, UINT wImageType);
DWORD SYM_EXPORT WINAPI     FATGetFATSize(LPFAT lpFAT);
void SYM_EXPORT WINAPI      FATClearEntireFAT (LPFAT lpFAT);

HPVOID SYM_EXPORT WINAPI FATCompress(LPDISKREC lpDisk, LPFAT lpFAT,LPDWORD lpNewSize);
BOOL SYM_EXPORT WINAPI FATDeCompress(LPDISKREC lpDisk, HPBYTE lpData, LPFAT lpFAT);


LPVOID PASCAL near _FATLock (LPFAT lpFAT, CLUSTER clCluster, CLUSTER FAR *lpclOffset, BOOL bDirty);

void SYM_EXPORT WINAPI   DiskGetFATStr(BYTE byFATType, LPSTR lpBuffer);
UINT SYM_EXPORT WINAPI   DiskGetDefaultVolume(LPSTR lpBuffer);

UINT SYM_EXPORT WINAPI   ImageSelect (BYTE byDriveLetter, LPSTR lpszFileName, WORD FAR * lpwImageType);
BYTE SYM_EXPORT WINAPI   ImageSave(BYTE  byDrive, BOOL bSingleCopy);
BYTE SYM_EXPORT WINAPI   _ImageSave(LPDISKREC lpDiskRec);
BYTE SYM_EXPORT WINAPI   ImageSaveFmt (BYTE   byDrive,
                                      BOOL   bSingleCopy,
                                      FRDataRecord far *lpFRD,
                                      LPDWORD lpdwIndexSize);



typedef UINT (WINAPI *LPFNIMAGENOTIFYPROC)(BYTE byDriveLetter,UINT uStage,LPVOID lpUserData);

BYTE SYM_EXPORT WINAPI  ImageSaveWithNotify(BYTE  byDrive, UINT uFlags,LPFNIMAGENOTIFYPROC lpNotifyProc,LPVOID lpUserData);



UINT SYM_EXPORT WINAPI DirTrace (LPDISKREC lpDisk, LPFAT lpFAT, LPSTR lpFullPath, CLUSTER FAR* lpDirSCN);
int SYM_EXPORT WINAPI DirRead (LPDISKREC lpDisk, LPFAT lpFAT, CLUSTER scn, HGLOBAL FAR* lphDir);
int SYM_EXPORT WINAPI DirWrite (LPDISKREC lpDisk, LPFAT lpFAT, HGLOBAL hDirHeader);
/*----------------------------------------------------------------------*/
/* Callback function type for DirScan()                                 */
/*----------------------------------------------------------------------*/

typedef UINT (WINAPI *DIRSCANPROC)(LPCDISKREC, LPSTR, CLUSTER, LPDIRHEADERREC,
                                   UINT, VOID FAR*);
UINT SYM_EXPORT WINAPI DirScan (BYTE byDriveLetter, DIRSCANPROC lpFunc,
                            UINT wData, VOID FAR* lpData, BOOL bFreeFat);
BOOL SYM_EXPORT WINAPI DirSearch (LPDISKREC lpDisk, LPFAT lpFAT, CLUSTER clSCN, const DIRSEARCHREC *lpSearchRec, LPDIRENTRYREC lpDir);
BOOL SYM_EXPORT WINAPI DirInDirectory (HGLOBAL hDir, LPSTR lpFileName);
BOOL SYM_EXPORT WINAPI DirIsSystemFile (LPCSTR lpFileName);

int SYM_EXPORT WINAPI IsDir (LPDIRSCANREC lpDirScan, HPCDIRENTRYREC lpDirEntry);
int SYM_EXPORT WINAPI IsDirBrief (LPDIRSCANREC lpDirScan, HPCDIRENTRYREC lpDirEntry);
int SYM_EXPORT WINAPI DirIsEntry (LPDIRSCANREC lpDirScan, HPCDIRENTRYREC dirPtr);

                                        // The following prototypes are ALSO
                                        // in file.h.
BYTE SYM_EXPORT WINAPI     DiskGet(VOID);
UINT SYM_EXPORT WINAPI     DiskSet(BYTE driveLetter);
BOOL SYM_EXPORT WINAPI     DiskGetAllocInfo(BYTE driveLetter,
                                        LPBYTE mediaDescriptorPtr,
                                        LPBYTE sectorsClusterPtr,
                                        UINT FAR *totalClustersPtr,
                                        UINT FAR *bytesPerSectorPtr);
UINT SYM_EXPORT WINAPI     DiskGetEquipmentDrives(VOID);
VOID SYM_EXPORT WINAPI     DiskSetToSingleDisk (BYTE dl);
BOOL SYM_EXPORT WINAPI     DiskSetLogical(BYTE driveLetter);
UINT SYM_EXPORT WINAPI     DiskGetFreeSpace(BYTE bDriveLetter,
                                        UINT FAR * lpwBytesSector,
                                        UINT FAR * lpwSectorsCluster,
                                        UINT FAR * lpwClusters);

#ifdef SYM_WIN32
UINT SYM_EXPORT WINAPI DiskGetFreeBytes(// Returns NOERR or ERR
    TCHAR byDriveLetter,
    PULARGE_INTEGER pulFreeBytes);      // Returns number of free bytes available to caller on this drive
#endif

UINT SYM_EXPORT WINAPI     DiskFreeAvailGet (BYTE dl,LPDWORD lpdwAvail,LPDWORD lpdwUsed);
DWORD SYM_EXPORT WINAPI    DiskGetClusterSize (BYTE driveLetter);
                                        // End of prototypes duplicated in
                                        // file.h.



/****************************************************************************/
/*
**  Functions in DSK_CACH.C:
*/
// Defines and prototype for DiskCacheUpdate():
#define DISK_CACHE_INIT         '\0'    // initialize all drives
#define DISK_CACHE_REINIT       '\1'    // re-initialize all drives, forced
#define DiskCacheInit()         DiskCacheUpdate( DISK_CACHE_INIT )
WORD SYM_EXPORT WINAPI DiskCacheUpdate(char cDrive);
// Macros and prototype for DiskCacheGetInfo():
#define DiskCacheGetMask( cDrive, lpwMask )\
        DiskCacheGetInfo(cDrive, NULL, NULL, lpwMask, NULL )
#define DiskCacheGetVolLabel( cDrive, lpszVolLabel )\
        DiskCacheGetInfo(cDrive, NULL, NULL, NULL, lpszVolLabel )
WORD SYM_EXPORT WINAPI DiskCacheGetInfo(char   cDrive,
                                    LPWORD lpwMajor,
                                    LPWORD lpwMinor,
                                    LPWORD lpwMask,
                                    LPSTR  lpszVolLabel);
// Prototypes for available drive functions:
int SYM_EXPORT WINAPI DiskCacheGetDrivesByMask(LPSTR lpDrvNames, WORD wMask);
typedef VOID (WINAPI *RECONCILECALLBACK)(BYTE,BOOL);
VOID SYM_EXPORT WINAPI DiskCacheReconcileChanges( LPSTR   lpszOldDrvList,
                                              RECONCILECALLBACK fpCallBack );
// Should only call this from the WEP function
VOID SYM_EXPORT WINAPI DiskCacheTerminate(VOID);

/****************************************************************************/

#ifdef SYM_WIN32
#ifdef INCLUDE_VWIN32
#ifndef _M_ALPHA        // WES Don't include for Alpha platform
LRESULT SYM_EXPORT WINAPI IOCtlVWin32     (DIOC_REGISTERS *lpRegsIn, DIOC_REGISTERS *lpRegsOut, DWORD  dwIoControlCode);
#endif
#endif
BOOL SYM_EXPORT WINAPI DiskVCacheFlush(BYTE driveLetter);
BOOL SYM_EXPORT WINAPI DiskVCacheInvalidate( BYTE driveLetter );
#endif


#if defined(SYM_WIN16) || defined(SYM_DOS)
UINT SYM_EXPORT WINAPI      DiskParseFilename (LPSTR commandLineToParse, LPSTR unopenedFCB);
#endif

#ifdef SYM_WIN                            // Windows only
UINT SYM_EXPORT WINAPI     DrvSelectCB(HWND hDlg,int nCtlId,LPARAM lParam);
#else                                   // DOS only
typedef int (WINAPI *DRIVEFILTERPROC)(BYTE);

BOOL SYM_EXPORT WINAPI      DiskIsLogical (BYTE dl);
BOOL SYM_EXPORT WINAPI      DiskIsTrueDrive (BYTE dl, BYTE byFilter, DRIVEFILTERPROC lpfnFilter);




// THE FOLLOWING PROTOTYPES ARE NOT IN THE QUAKE LIBRARIES


#if defined(SYM_DOS)

void            CheckRecoveryInfo(void);

UINT    PASCAL  DiskType( BYTE dl );
void    CDECL   DOSInstallInt1CHandler(void);
void    CDECL   DOSUnInstallInt1CHandler(void);
DWORD           DiskComputeSeekOverhead(ABSDISKREC *absDiskPtr);
DWORD           DiskComputeTrackToTrack(ABSDISKREC *absDiskPtr, int seeks);
DWORD           DiskComputeFullStroke(ABSDISKREC *absDiskPtr);
DWORD           DiskComputeAverageSeeks(ABSDISKREC *absDiskPtr);

/*----------------------------------------------------------------------*
 * Rescue Library public function prototypes                            *
 *----------------------------------------------------------------------*/

BYTE    PASCAL RescueSavePartInfo(const BYTE *pszRescuePath);
BYTE    PASCAL RescueSaveCMOSInfo(const BYTE *pszRescuePath);
BYTE    PASCAL RescueSaveBootInfo(const BYTE *pszRescuePath);
int     PASCAL RescueRestorePartInfo(const BYTE *pszRescuePath);
int     PASCAL RescueRestoreCMOSInfo(const BYTE *pszRescuePath);
int     PASCAL RescueRestoreBootInfo(const BYTE *pszRescuePath);

BYTE    PASCAL RescueSavePartInfoToFile(const BYTE *szFileName);
BYTE    PASCAL RescueSaveBootInfoToFile(const BYTE *szFileName);
BYTE    PASCAL RescueSaveCMOSInfoToFile(const BYTE *szFileName);

UINT    PASCAL  RescueSaveData(const BYTE *pszRescuePath);
UINT    PASCAL  RescueRestoreData(const BYTE *pszRescuePath, UINT wRestoreFlags);
BYTE    PASCAL  StdDlgSaveRescueData(const BYTE *pszRescuePath);
BYTE    PASCAL  StdDlgRestoreRescueData(const BYTE *pszRescuePath);


BYTE    PASCAL  DiskRecalcPartEntry (PartEntryRec *partPtr, ABSDISKREC *absDiskPtr);
UINT    CDECL   DiskIsNCACHEInstalled(void);
BYTE    CDECL   DiskIsSmartdriveInstalled(void);
void    CDECL   DiskFlushSmartdrive(void);
void    CDECL   DiskSuspendSmartdrive(void);
void    CDECL   DiskResumeSmartdrive(void);
void    CDECL   DiskFastOpenFlush(BYTE driveLetter);
BYTE    CDECL   DiskFastOpenInstalled(BYTE dl);
int     PASCAL  NUDiskGetType(BYTE dl);
BYTE    PASCAL  DiskIsReady(BYTE dl);

LPBYTE  PASCAL  DiskGetDMABuffer(void);
WORD    PASCAL  DiskGetDMABufferSize(void);
BOOL    PASCAL  DiskAllocDMABuffer  (BYTE far **plpBuffer);
void    PASCAL  DiskFreeDMABuffer(void);
BOOL    PASCAL  _DiskAllocDMABuffer (BYTE far **plpBuffer, WORD nSectors);
void    PASCAL  _DiskFreeDMABuffer  (BYTE far *lpBuffer);

BYTE    PASCAL  DiskSetFATPtr (BYTE dl, UINT cluster);
BYTE            DiskSingleFloppy (void);
#ifdef NCD
BYTE    CDECL   DiskSeek (BYTE driveNum, WORD track, BYTE sectorLenCode);
#else
BYTE    CDECL   DiskSeek (BYTE driveNum, UINT track);
#endif
BYTE    CDECL   DiskReady(BYTE driveNum);
#ifndef	SYM_DOSX
void            HandleDiskErrors (void);        /* Pass addr to DiskErrorOn()   */
#else
void interrupt HandleDiskErrors(REGS16 rRegs);
#endif
UINT    PASCAL  DiskGetDriveType(DISKREC *diskPtr, ABSDISKREC *absDiskPtr);
int     PASCAL  DiskIsBig(BYTE dl, DISKREC *diskPtr);
UINT            _DiskIsBig(BYTE drv_num,UINT buff_seg,UINT buff_off);
BYTE    PASCAL  DiskIsLantasticServer(void);
UINT    PASCAL  DiskSetVerifyState (UINT newState);
BYTE    CDECL   DiskGetVerifyState (void);
BYTE  * PASCAL  DiskGetPartitionSystemString (BYTE systemID);
void    CDECL   DiskGetLowLevelInt13(void);
BYTE    CDECL   DiskLowLevelInt13 (UINT command, ABSDISKREC *absDiskRecPtr);
BYTE    PASCAL  DiskCheckFatIds (DISKREC *diskPtr, BYTE far *buffer);
int     PASCAL  DiskFATCheck(DISKREC *diskPtr);
BYTE    CDECL   DiskCheckController(BYTE driveNumber);
BYTE    CDECL   DiskCheckControllerRAM(BYTE driveNumber);
int     CDECL   DiskCheckBPB(BYTE dl);
void    PASCAL  DiskSetBPBVolumeLabel(BYTE byDrive, BYTE *pszVolumeLabel);
void    PASCAL  DiskKeepRunning(BYTE byClockTicks);
BYTE    PASCAL  DiskMotorOffGet(void);
BYTE    PASCAL  DiskMotorOffSet(BYTE byClockTicks);
BYTE    PASCAL  DiskSettleTimeGet(void);
BYTE    PASCAL  DiskSettleTimeSet(BYTE byMilliseconds);
BYTE    PASCAL  DiskStartupGet(void);
BYTE    PASCAL  DiskStartupSet(BYTE byEightsOfSec);

#ifdef	NCD
BYTE	PASCAL	isExtendHD(BYTE dn);
int	PASCAL	diskParm (BYTE dn, WORD *drives, WORD *cylinders,
					WORD *sectors, WORD *sectorSize);
#endif

DWORD   PASCAL  VDiskGetDriveList (struct VDISKREC *pVDisk, BYTE *pszDriveList,
                                        UINT *wListIndex);
DWORD   PASCAL  ConvertPhySectorToLogSector(ABSDISKREC *absDisk, DISKREC *disk,
                                 DWORD head, DWORD track, DWORD sector);

void    PASCAL  DirSwap (LPDIRENTRYREC a, LPDIRENTRYREC b);
int     PASCAL  DirPruneTree (const BYTE *source, BYTE *dest);
int     PASCAL  DirPrepareList (DIRENTRYREC huge **p, int cnt, int *countPtr, UINT scn);
void    PASCAL  DirClear (HGLOBAL hDirHeader);

#define RMVTREE_COMPLETE                0
#define RMVTREE_ERR_INVALID_SOURCE      1
#define RMVTREE_ERR_INVALID_DEST        2
#define RMVTREE_ERR_ENTRY_NOT_FOUND     3
#define RMVTREE_ERR_NO_ROOM             4
#define RMVTREE_ERR_GENERAL             5
#define RMVTREE_ERR_DUP_NAME            6

CLUSTER PASCAL  FATFindSCN(LPFAT lpFat, DISKREC *diskPtr, CLUSTER cluster);
BYTE    PASCAL  FATBitGet (UINT cluster, HGLOBAL hMem);
void    PASCAL  FATBitPutOn (UINT cluster, HGLOBAL hMem);
void    PASCAL  FATBitPutOff (UINT cluster, HGLOBAL hMem);
HGLOBAL PASCAL  FATBitAlloc (DISKREC *diskPtr);

#if 0
void    PASCAL  FATReset (void);
int     PASCAL  RawDirReadPartial (DISKREC *diskPtr, UINT scn, DIRENTRYREC huge**ptr);
int     PASCAL  RawDirWritePartial (DISKREC *diskPtr, UINT scn, DIRENTRYREC huge  *ptr);
void    PASCAL  RawDirClearPartial (void far *ptr);
int     PASCAL  RawDirReadRoot (DISKREC *diskPtr, DIRENTRYREC huge **ptr);
int     PASCAL  RawDirWriteRoot (DISKREC *diskPtr, DIRENTRYREC huge *ptr);
#endif  // #if 0

#endif  // NOOLDSYMANTEC

#endif  // SYM_WIN

#ifdef __cplusplus
}
#endif

#endif  // _INCL_DISK_
