/* Copyright 1991 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/VDISKREC.H_V   1.3   16 Dec 1991 18:11:20   TORSTEN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 *	Defines the VDISKREC structure used to virtualize the disk	*
 * 	operations.							*
 * 									*
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************ 
 * $Log:   S:/INCLUDE/VCS/VDISKREC.H_V  $ *
 * 
 *    Rev 1.3   16 Dec 1991 18:11:20   TORSTEN
 * Added the NetBIOS VDISKREC enum.
 * 
 *    Rev 1.2   18 Nov 1991 12:45:02   MARKK
 * Added diskisencrypted
 * 
 *    Rev 1.1   17 Oct 1991 10:05:28   MARKK
 * Changed VDISK stuff
 * 
 *    Rev 1.0   07 Oct 1991 13:10:04   PETERD
 * Initial revision.
 ************************************************************************/

#ifndef	INCL_VDISKREC
#define	INCL_VDISKREC

#ifndef INCL_TYPES
#include <types.h>
#endif

#ifndef	INCL_VMM
#include <vmm.h>
#endif

enum __VDISKTYPE
    {
    VTYPE_DOS = 0,
    VTYPE_SPX,
    VTYPE_FASTLYNX,
    VTYPE_ZIP,
    VTYPE_NETBIOS
    };

/************************************************************************
 * Define the VDiskRec structure					*
 ************************************************************************/

typedef struct VDISKREC
    {
    int	       (CDECL *FileFindFirst)	(struct VDISKREC *pVDisk,
					 Byte *fileName,
					 Word attr,
					 void *infoPtr);
    int	       (CDECL *FileFindNext)	(struct VDISKREC *pVDisk,
					 void *infoPtr);
    int	       (CDECL *FileOpen)	(struct VDISKREC *pVDisk,
					 Byte *fileName,
					 Word openMode);
    int	       (CDECL *FileClose)	(struct VDISKREC *pVDisk,
					 Word handle);
    int	       (CDECL *FileRead)	(struct VDISKREC *pVDisk,
					 Word handle,
					 void *buffer,
					 Word size);
    int	       (CDECL *FileWrite)	(struct VDISKREC *pVDisk,
					 Word handle,
					 void *buffer,
					 Word size);
    int	       (CDECL *FileGetDateTime) (struct VDISKREC *pVDisk,
					 Word handle,
					 Word *date,
					 Word *time);
    int	       (CDECL *FileSetDateTime) (struct VDISKREC *pVDisk,
					 Word handle,
					 Word date,
					 Word time);
    Ulong      (CDECL *FileSize)	(struct VDISKREC *pVDisk,
					 Word handle);
    Ulong      (CDECL *FileSeek)	(struct VDISKREC *pVDisk,
					 Word handle,
					 Ulong offset,
					 Word method);
    int	       (CDECL *FileCreate)	(struct VDISKREC *pVDisk,
					 Byte *fileName,
					 Word attributes);
    int	       (CDECL *FileGetAttr)	(struct VDISKREC *pVDisk,
					 Byte *fileName);
    int	       (CDECL *FileSetAttr)	(struct VDISKREC *pVDisk,
					 Byte *fileName,
					 Word attributes);
    int	       (CDECL *FileDelete)	(struct VDISKREC *pVDisk,
					 Byte *fileName);
    int	       (CDECL *FileRename)	(struct VDISKREC *pVDisk,
					 Byte *oldName,
					 Byte *newName);
    int	       (PASCAL *FileCopy)	(struct VDISKREC *pVDisk,
					 Byte *fromFile,
					 Byte *toFile,
					 Boolean (*breakCheck)(void));
    int	       (CDECL *DirRemove)	(struct VDISKREC *pVDisk,
					 Byte *path);
    int	       (CDECL *DirChange)	(struct VDISKREC *pVDisk,
					 Byte *path);
    int	       (CDECL *DirMake)         (struct VDISKREC *pVDisk,
					 Byte *path);
    int	       (PASCAL *DirRemoveTree)	(struct VDISKREC *pVDisk,
					 Byte *path,
					 void (*displayFunc)(Byte *),
					 int (*breakFunc)(void));
    int	       (CDECL *DirCopyTree)	(struct VDISKREC *pVDisk,
					 Byte *source,
					 Byte *dest,
					 void (*displayFunc)(Byte *),
					 Boolean (*breakFunc)(void));
    int	       (CDECL *DirMoveTree)	(struct VDISKREC *pVDisk,
					 Byte *source,
					 Byte *dest,
					 void (*displayFunc)(Byte *),
					 Boolean (*breakFunc)(void));
    void FAR * (PASCAL * GetObjectInfo) (struct VDISKREC *pVDisk,
					 void FAR *lpVoid);
    int	       (CDECL *DirGetName)      (struct VDISKREC *pVDisk,
					 Byte driveletter,
                                         Byte *dest);
    Word       (FAR PASCAL *DOSGetExtendedError) (struct VDISKREC *pVDisk);
					 

    void       (FAR PASCAL *DTASet)     (struct VDISKREC *pVDisk,
					 void FAR *lpDTA);
    void FAR * (FAR PASCAL *DTAGet)     (struct VDISKREC *pVDisk);
    Byte       (CDECL *DiskGet)         (struct VDISKREC *pVDisk);
    Word       (PASCAL *DiskSet)        (struct VDISKREC *pVDisk,
					 Byte driveLetter);
    Word       (PASCAL *DiskType)       (struct VDISKREC *pVDisk,
					 Byte byDriveLetter);
    Boolean    (PASCAL *DiskGetVolumeLabel) (struct VDISKREC *pVDisk,
					 Byte byDriveLetter, Byte *pszVolumeLabel);
    Boolean    (PASCAL *DiskSetVolumeLabel) (struct VDISKREC *pVDisk,
					 Byte byDriveLetter, Byte *pszVolumeLabel);
    Boolean    (PASCAL *DiskGetAllocInfo)(struct VDISKREC *pVDisk,
					  Byte dl,
					  Byte *mediaDescriptor, 
					  Byte *SectorsPerCluster,
					  Word *totalClusters, 
					  Word *bytesPerSector);
    Word       (PASCAL *DiskGetClusterSize)(struct VDISKREC *pVDisk,
					 Byte dl);
    Word       (PASCAL *DiskGetFreeSpaceInfo)(struct VDISKREC *pVDisk,
					      Byte driveLetter,
					      Word *bytesSectorPtr, 
					      Word *sectorsClusterPtr, 
					      Word *clustersPtr);
    Byte	byType;			// The type of the vdisk
    Handle	hInstanceData;		// The instance data for this vdisk
    Ulong	(PASCAL	*DiskIsEncrypted)(struct VDISKREC *pVDisk, Byte dl);
    } VDISKREC;
    
typedef VDISKREC NEAR *NPVDISKREC;
typedef VDISKREC FAR  *LPVDISKREC;
typedef VDISKREC HUGE *HPVDISKREC;

#endif
