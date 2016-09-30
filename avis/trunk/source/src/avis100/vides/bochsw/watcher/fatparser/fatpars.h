#ifndef SMALLFATPARSER_H
#define SMALLFATPARSER_H

#include "basictypes.h"
#include "direntry.h"

// This macro is used in the dumps
#define PRINTABLE(c) ((c)<0x20?'.':(c))

// Here are the values describing all sorts of cluster types
#define FREECLUS(c)		((c)==0)
#define BADCLUS(c)		(FATis12?((c)>0xFF0 && (c)<0xFF8):((c)>0xFFF0 && (c)<0xFFF8))
#define ENDCLUS(c)		(FATis12?((c)>=0xFF8 && (c)<=0xFFF):((c)>=0xFFF8 && (c)<=0xFFFF))
#define USUALBADCLUS(c)	(FATis12?((c)==0xFF7):((c)==0xFFF7))
#define USUALENDCLUS(c)	(FATis12?((c)==0xFFF):((c)==0xFFFF))
#define INVCLUS(c)		((c)==1)

// Macros to walk the cluster chain
#define FOREACHCLUSTER(c)	while(!BADCLUS(c)&&!INVCLUS(c)&&!FREECLUS(c)&&!ENDCLUS(c))
#define NEXTCLUSTER(c)		c=nextc(c)

/*

// These are parameters read from the Partition Boot Sector or computed using params in the PBS
extern BYTE *image;
extern BYTE *boot;
extern BYTE *FAT;
extern BYTE *FAT2;
extern DWORD FATsize;
extern BYTE *root;
extern DWORD num_clus;
extern BYTE FATis12;
extern DWORD rootsize;
extern WORD num_root_entries;
extern WORD sect_per_clus;
extern WORD bytes_per_sect;
extern WORD clus_size;

*/

/*

// Sets up all the variables above, takes a pointer to the PBS in mem
void computediskparams(BYTE *p);
// function to chain to the next cluster
WORD nextc(WORD cluster);
// function to compute a pointer to the cluster data in mem from the cluster #
BYTE *data(WORD cluster);
// dump functions
void dumpline(BYTE *p);
void dumpsector(BYTE *p);



// a function to dump an entry
void dumpentry(DirEntry *p);
// finds an entry corresponding to the given name in the root directory, NULL if not found
DirEntry *findFileInRoot(char *fullname);
// finds an entry corresponding to a file/dir described by a full absolute path
DirEntry *findPath(char *fullpath);



// to dump a boot sector
void dumpboot(BYTE *);

*/

int extractitem(char *itemname, char *destpath);
int extractsector(int disk, int head, int track, int sector, char *destpath);

#endif
