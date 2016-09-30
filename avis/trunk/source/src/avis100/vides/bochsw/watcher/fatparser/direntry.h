#ifndef DIRENTRY_H
#define DIRENTRY_H

#include "basictypes.h"

#define DIRENTRYSIZE 0x20

// Define the attribute bits
#define ATTR_READONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLLABEL	0x08
#define ATTR_SUBDIR		0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_UNUSED		0xC0

// Convenient macros to test the bits
#define IS_READONLY(e)	((e)->fileattr & ATTR_READONLY)
#define IS_HIDDEN(e)	((e)->fileattr & ATTR_HIDDEN)
#define IS_SYSTEM(e)	((e)->fileattr & ATTR_SYSTEM)
#define IS_VOLLABEL(e)	((e)->fileattr & ATTR_VOLLABEL)
#define IS_SUBDIR(e)	((e)->fileattr & ATTR_SUBDIR)
#define IS_ARCHIVE(e)	((e)->fileattr & ATTR_ARCHIVE)

#define IS_ERASED(e)	((e)->filename[0] == 0xE5)
#define IS_DOTDIR(e)	((e)->filename[0] == 0x2E)
#define IS_EMPTY(e)		((e)->filename[0] == 0)

#define SWAP(w) (w)
//#define SWAP(w) ((((w)&0x00FF)<<8) + (((w)&0xFF00)>>8))

// Macros to read the date and time of a directory entry (buggy, needs to be fixed)
#define TIME_SECS(w)	((SWAP(w)&0x001F)*2)
#define TIME_MINS(w)	((SWAP(w)&0x07E0)>>5)
#define TIME_HOURS(w)	((SWAP(w)&0xF800)>>11)

#define DATE_DAY(w)		((w)&0x001F)
#define DATE_MONTH(w)	(((w)&0x01E0)>>5)
#define DATE_YEAR(w)	((((w)&0xFE00)>>9)+1980)

#pragma pack(1)

// The actual directory entry structure
typedef struct {
/* 00 */	BYTE filename[8];
/* 08 */	BYTE fileext[3];
/* 0B */	BYTE fileattr;
/* 0C */	BYTE res[10];
/* 16 */	WORD updtime;
/* 18 */	WORD upddate;
/* 1A */	WORD first_clus;
/* 1C */	DWORD filesize;
} DirEntry;

#pragma pack()

#endif
