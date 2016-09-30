#ifndef _BOOTYPE_H

#define _BOOTYPE_H

#define BYTES_PER_SECTOR                512

#define FLOPPY_HEADS                    2

#define MAX_FLOPPY_SECTORS_PER_FAT      24              // or less

#define PARTITION_CODE_SIZE             0x01be

#define BOOT_RECORD_CODE_SIZE           0x1e1

#define MAX_PARTITION_ENTRIES           4

#define FIXED_DISK_MEDIA_DESCRIPTOR     0xf8

#define ACTIVE_PARTITION                0x80
#define INACTIVE_PARTITION              0

#define BOO_TYPE_FLOPPY                 0
#define BOO_TYPE_MBR                    1
#define BOO_TYPE_PBR                    2
#define BOO_TYPE_NOT_SET                3

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

typedef struct
{
    BYTE                        byBoot;
    BYTE                        byStartHead;
    WORD                        wStartSecCyl;
    BYTE                        bySys;
    BYTE                        byEndHead;
    WORD                        wEndSecCyl;
    DWORD                       dwStartRel;
    DWORD                       dwTotalSec;
} PARTITION_ENTRY_T;

typedef struct
{
    BYTE                    byBootCode[PARTITION_CODE_SIZE];
    PARTITION_ENTRY_T       stEntry[4];
    BYTE                    byEndSig[2];
} MBR_T, FAR *LPMBR, *PMBR;

typedef struct
{
    BYTE                    byJump[3];
    BYTE                    byOEM[8];
    WORD                    wBytesPerSector;
    BYTE                    byClusterSize;
    WORD                    wReservedSectors;
    BYTE                    byFatCount;
    WORD                    wRootEntries;
    WORD                    wTotalSectors;
    BYTE                    byMediaDescriptor;
    WORD                    wFatSize;
    WORD                    wSectorsPerTrack;
    WORD                    wNumHeads;
    WORD                    wHiddenSectors;
    BYTE                    byCode[BOOT_RECORD_CODE_SIZE];
    BYTE                    byEndSig[2];
} BOOT_RECORD_T, FAR *LPBOOT_RECORD, *PBOOT_RECORD;

#if defined(SYM_WIN32) || defined(SYM_NTK)
#pragma pack(pop)
#else
#pragma pack()
#endif

int DetermineBootType
(
    LPBYTE                  lpbyBooBuffer
);

#endif // #ifndef _BOOTYPE_H
