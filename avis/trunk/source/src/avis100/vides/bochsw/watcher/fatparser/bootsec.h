#ifndef BOOTSEC_H
#define BOOTSEC_H

#include "basictypes.h"

#pragma pack(1)

// Structure of a partition boot sector
typedef struct {
/* 00 */	BYTE jump[3];
/* 03 */	BYTE OEM[8];
/* 0B */	WORD bytes_per_sect;
/* 0D */	BYTE sect_per_clus;
/* 0E */	WORD reserved_sect_num;
/* 10 */	BYTE num_FAT;
/* 11 */	WORD num_root_dir_entries;
/* 13 */	WORD tot_num_sect;
/* 15 */	BYTE media_descriptor;
/* 16 */	WORD sect_per_FAT;
/* 18 */	WORD sect_per_track;
/* 1A */	WORD num_heads;
/* 1C */	WORD num_hidden_sect;
			WORD gap1;
/* 20 */	DWORD tot_num_sect_bigpart;
/* 24 */	BYTE phys_drive_num;
			BYTE res1;
/* 26 */	BYTE sig;
/* 27 */	DWORD vol_serial_num;
/* 2B */	BYTE vol_label[11];
			BYTE res2[8];
} BootSector;

#pragma pack()

#endif
