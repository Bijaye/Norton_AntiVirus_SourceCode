#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "bootsec.h"
#include "direntry.h"
#include "fatpars.h"
#include "..\..\..\Bochs-96\iodev\iodev.h"

BYTE *FAT = NULL;
BYTE *FAT2 = NULL;
DWORD FATsize = 0; //in bytes
BYTE *root = NULL;
DWORD num_clus = 0;
BYTE FATis12 = 0;
DWORD rootsize = 0; //in bytes
WORD num_root_entries = 0;
WORD sect_per_clus = 0;
WORD bytes_per_sect = 0;
WORD clus_size = 0;
BYTE *partition_start = NULL;


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
int extractitem(char *itemname, char *destpath);


#define HANDLE_ONLY_ACCESS_VIOLATIONS ((GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)


static void dumpentry(DirEntry *e)
{
	__try {
	printf("Name: %.8s   Extension: %.3s\n", e->filename, e->fileext);
	printf("Attributes: %02X\n", e->fileattr);
	if (e->fileattr & ATTR_READONLY)	printf("   READ ONLY\n");
	if (e->fileattr & ATTR_HIDDEN)		printf("   HIDDEN\n");
	if (e->fileattr & ATTR_SYSTEM)		printf("   SYSTEM\n");
	if (e->fileattr & ATTR_VOLLABEL)	printf("   VOLUME LABEL\n");
	if (e->fileattr & ATTR_SUBDIR)		printf("   SUBDIRECTORY\n");
	if (e->fileattr & ATTR_ARCHIVE)		printf("   ARCHIVE\n");
	printf("Unused bits: %02X\n", e->fileattr & ATTR_UNUSED);
	printf("Time: [%04X] %d hour %d min %d sec\n", e->updtime, TIME_SECS(e->updtime), TIME_MINS(e->updtime), TIME_HOURS(e->updtime));
	printf("Date: %d/%d/%d\n", DATE_MONTH(e->upddate), DATE_DAY(e->upddate), DATE_YEAR(e->upddate));
	printf("First Cluster: %04X\n", e->first_clus);
	printf("File length is: %08X (%d bytes)\n", e->filesize, e->filesize);
	}
	__except (HANDLE_ONLY_ACCESS_VIOLATIONS) {
		fprintf(stderr, "Memory access violation trying to dump directory entry!\n");
	}
}

static DirEntry *findFileInRoot(char *fullname)
{
	char name[9];
	char ext[4];
	char ename[9];
	char eext[4];
	char *dot;
	char *space;
	BYTE *res = NULL;
	DirEntry *e;

	name[8] = 0;
	ext[3] = 0;
	ename[8] = 0;
	eext[3] = 0;

	strncpy(name, fullname, 8);
	if (dot=strchr(name, '.')) *dot = 0;
	strncpy(ext, ((dot=strchr(fullname, '.'))==NULL)?"":dot+1, 3);

	__try {
	for (e=(DirEntry *) root ; e<(DirEntry *) (root+num_root_entries*DIRENTRYSIZE) ; e++) {
		strncpy(ename,e->filename,8);
		strncpy(eext,e->fileext,3);
		if (space=(strchr(ename,0x20))) *space = 0;
		if (space=(strchr(eext ,0x20))) *space = 0;
		if (strncmp(ename, name, 8) == 0 &&
			strncmp(eext , ext , 3) == 0)
			return e;
	}
	}
	__except (HANDLE_ONLY_ACCESS_VIOLATIONS) {
		fprintf(stderr, "Memory access violation while looking for file in root!\n");
	}
	return NULL;
}

// Cluster parameter passed using a pointer to the cluster in memory
static DirEntry *findFileInCluster(char *fullname, BYTE *p)
{
	char name[9];
	char ext[4];
	char ename[9];
	char eext[4];
	char *dot;
	char *space;
	//BYTE *res = NULL;
	DirEntry *e;

	name[8] = 0;
	ext[3] = 0;
	ename[8] = 0;
	eext[3] = 0;

	strncpy(name, fullname, 8);
	if (dot=strchr(name, '.')) *dot = 0;
	strncpy(ext, ((dot=strchr(fullname, '.'))==NULL)?"":dot+1, 3);

	__try {
	for (e=(DirEntry *) p ; e<(DirEntry *) (p+clus_size) ; e++) {
		strncpy(ename,e->filename,8);
		strncpy(eext,e->fileext,3);
		if (space=(strchr(ename,0x20))) *space = 0;
		if (space=(strchr(eext ,0x20))) *space = 0;
		if (strncmp(ename, name, 8) == 0 &&
			strncmp(eext , ext , 3) == 0)
			return e;
	}
	}
	__except (HANDLE_ONLY_ACCESS_VIOLATIONS) {
		fprintf(stderr, "Memory access violation while looking for file name in cluster!\n");
	}
	return NULL;
}

// Cluster parameter passed using a cluster number (as indicated in a directory entry, for example)
static DirEntry *findFileInSubDir(char *fullname, WORD c)
{
	DirEntry *e;

	FOREACHCLUSTER(c) {
		if ((e = findFileInCluster(fullname, data(c))) != NULL)
			return e;
		NEXTCLUSTER(c);
	}
	// Test BAD, INVALID and FREE cluster cases here (?)
	return NULL;
}

// Takes an absolute path as argument, starting with a backslash, WITHOUT a drive letter before it
// Backslash is the only allowed delimiter, slash not supported
static DirEntry *findPath(char *fullpath)
{
	char *backslash;
	DirEntry *e;
	char lastPathElem=0;

	if (*fullpath == '\\')
		fullpath++;
	else
		return NULL;
	if (backslash=strchr(fullpath,'\\'))
		*backslash = 0;
	else
		lastPathElem = 1;
	e = findFileInRoot(fullpath);

	while (e && lastPathElem == 0) {
		fullpath = backslash+1;
		if (backslash=strchr(fullpath,'\\'))
			*backslash = 0;
		else
			lastPathElem = 1;
		e = findFileInSubDir(fullpath, e->first_clus);
	}

	return e;
}





static void dumpboot(BYTE *p)
{
	BootSector *s = (BootSector *)p;

	__try {
	printf("OEM: %.8s\n",s->OEM);
	printf("Bytes per sector: %d\n", s->bytes_per_sect);
	printf("Sectors per cluster: %d\n", s->sect_per_clus);
	printf("# of reserved sectors: %d\n", s->reserved_sect_num);
	printf("# of FATs: %d\n", s->num_FAT);
	printf("# of root dir entries: %d\n", s->num_root_dir_entries);
	printf("Total # of sectors: %d\n", s->tot_num_sect);
	printf("Media descriptor: %d\n", s->media_descriptor);
	printf("Sectors per FAT: %d\n", s->sect_per_FAT);
	printf("Sectors per track: %d\n", s->sect_per_track);
	printf("# of heads: %d\n", s->num_heads);
	printf("# of hidden sectors: %d\n", s->num_hidden_sect);
	printf("Total # of sectors (if big partition): %d\n", s->tot_num_sect_bigpart);
	printf("Physical drive number: %d\n", s->phys_drive_num);
	printf("Signature: %d\n", s->sig);
	printf("Volume serial #: %d\n", s->vol_serial_num);
	printf("Volume label: %.11s\n", s->vol_label);
	}
	__except (HANDLE_ONLY_ACCESS_VIOLATIONS) {
		fprintf(stderr, "Memory access violation trying to dump boot sector!\n");
	}
}







// Compute the params, takes ptr to PBS
static void computediskparams(BYTE *p)
{
	BootSector *s = (BootSector *)p;

	__try {
	 	sect_per_clus = s->sect_per_clus;
		if (sect_per_clus == 0) return; /* avoid divide by 0 */
		num_clus = s->tot_num_sect/sect_per_clus;
		FATis12 = (num_clus>0xFF0)?0:1;
		bytes_per_sect = s->bytes_per_sect;
		clus_size = sect_per_clus * bytes_per_sect;
		FAT = partition_start + s->reserved_sect_num * bytes_per_sect;
		FATsize = s->sect_per_FAT * bytes_per_sect;
		FAT2 = (s->num_FAT>1)?(FAT + FATsize):NULL;
		root = FAT + s->num_FAT * FATsize;
		num_root_entries = s->num_root_dir_entries;
		rootsize = num_root_entries * DIRENTRYSIZE;
	}
	__except (HANDLE_ONLY_ACCESS_VIOLATIONS) {
		fprintf(stderr, "Memory access violation while parsing boot sector!\n");
	}
}

// function to chain to next cluster, uses global var FATis12
static WORD nextc(WORD cluster)
{
	DWORD ofs;
	WORD next;

	__try {
	if (FATis12) {
		ofs = cluster + (cluster>>1);
		next = *(WORD *) (FAT+ofs);
		return (cluster&1)?(next>>4):(next&0x0FFF);
	}
	else { // 16-bit FAT
		ofs = cluster<<1;
		return *(WORD *) (FAT+ofs);
	}
	}
	__except (HANDLE_ONLY_ACCESS_VIOLATIONS) {
		fprintf(stderr, "Memory access violation while parsing the FAT!\n");
		return 0; // returning this (free cluster) will cause the FOREACHCLUSTER loops to exit
	}
}

// computes ptr to the data in a cluster given the cluster #
static BYTE *data(WORD cluster)
{
	return root+rootsize+(cluster-2)*sect_per_clus*bytes_per_sect;
}

// debug function - NOT SECURED
static void dumpline(BYTE *p)
{
printf("%02X %02X %02X %02X ",*(p+0x00),*(p+0x01),*(p+0x02),*(p+0x03));
printf("%02X %02X %02X %02X-",*(p+0x04),*(p+0x05),*(p+0x06),*(p+0x07));
printf("%02X %02X %02X %02X ",*(p+0x08),*(p+0x09),*(p+0x0A),*(p+0x0B));
printf("%02X %02X %02X %02X ",*(p+0x0C),*(p+0x0D),*(p+0x0E),*(p+0x0F));
printf("  ");
printf("%c%c%c%c",PRINTABLE(*(p+0x00)),PRINTABLE(*(p+0x01)),PRINTABLE(*(p+0x02)),PRINTABLE(*(p+0x03)));
printf("%c%c%c%c",PRINTABLE(*(p+0x04)),PRINTABLE(*(p+0x05)),PRINTABLE(*(p+0x06)),PRINTABLE(*(p+0x07)));
printf("%c%c%c%c",PRINTABLE(*(p+0x08)),PRINTABLE(*(p+0x09)),PRINTABLE(*(p+0x0A)),PRINTABLE(*(p+0x0B)));
printf("%c%c%c%c",PRINTABLE(*(p+0x0C)),PRINTABLE(*(p+0x0D)),PRINTABLE(*(p+0x0E)),PRINTABLE(*(p+0x0F)));
printf("\n");
}

// debug function - NOT SECURED
static void dumpsector(BYTE *p)
{
	int i;
	for (i=0;i<0x20;i++,p+=0x10) dumpline(p);
}

// Given a pointer to a directory entry in memory, writes the file pointed to by the entry to the given handle
static void writeEntryToHandle(DirEntry *e, HANDLE h)
{
    __try {
	WORD curclus = e->first_clus;
	DWORD towrite = e->filesize;
	DWORD written;
	
	if (IS_SUBDIR(e)) towrite = 0xFFFFFFFF; // because this parameter is zero for directories

	FOREACHCLUSTER(curclus) {
		if (h) {
			WriteFile(h,data(curclus),clus_size>towrite?towrite:clus_size,&written,NULL);
			towrite -= written;
		}
		NEXTCLUSTER(curclus);
	}
	if (BADCLUS(curclus)) printf("Found bad cluster, # %04X\n", curclus);
	if (INVCLUS(curclus)) printf("Found invalid cluster, # %04X\n", curclus);
	if (FREECLUS(curclus)) printf("Found free cluster, # %04X\n", curclus);
	}
	__except (HANDLE_ONLY_ACCESS_VIOLATIONS) {
		fprintf(stderr, "Memory access violation while reading a FileSystem object!\n");
	}
}








extern char *ow_bootsec[]; // boot sectors to use instead of the ones on the images

/*
   Be careful, this function is case-sensitive
 */

int extractitem(char *itemname, char *destpath)
{
  DirEntry *entry;
  BYTE *boot = NULL;
  HANDLE h;
  char absolute[256];

  if ((h = CreateFile(destpath,
		GENERIC_READ|GENERIC_WRITE,
		0,NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE)
	  return 1;
  absolute[255] = '\0';

  if (strnicmp(itemname, "A:", 2) == 0) {
	  strncpy(&absolute[1], itemname+2, 254);
	  partition_start = getfloppyimg(0);
	  if (ow_bootsec[0])
		  boot = ow_bootsec[0];
	  else
		  boot = partition_start;
  }
  else if (strnicmp(itemname, "B:", 2) == 0) {
	  strncpy(&absolute[1], itemname+2, 254);
	  partition_start = getfloppyimg(1);
	  if (ow_bootsec[1])
		  boot = ow_bootsec[1];
	  else
		  boot = partition_start;
  }
  else if (strnicmp(itemname, "C:", 2) == 0) {
	  BYTE *image = gethdimg();
	  strncpy(&absolute[1], itemname+2, 254);
	  // In the case of a HD, take the field of the MBR indicating how many sectors
	  // there are before the first partition to compute the address of the PBS
	  // It's kind of a hack, as we should check that the partition is valid, that it's
	  // a DOS file system, and take care of viruses that mess with the partition table
	  // and change the active partition... Fred
	  partition_start = image + (* (WORD *) (image + 0x1BE + 8)) * 512; // BTW, it's really a DWORD, but WORD is OK...
	  if (ow_bootsec[2])
		  boot = ow_bootsec[2];
	  else
		  boot = partition_start;
  }
  else {
	  strncpy(&absolute[1], itemname, 254);
	  partition_start = getfloppyimg(0);
 	  if (ow_bootsec[0])
		  boot = ow_bootsec[0];
	  else
		  boot = partition_start;
  }

  if (absolute[1] == '\\') {
	  strcpy(absolute, &absolute[1]); // Is it safe to do that ?
  }
  else {
	  absolute[0] = '\\';
  }

  computediskparams(boot);

  if ((entry = findPath(absolute)) == NULL) return 1;

  // Save virtual file to real file
  writeEntryToHandle(entry, h);
  CloseHandle(h);
  printf("EXTRACTING FILE %s TO FILE %s\n",itemname,destpath); // shows filemapping, STefan

  return 0;
}


int extractsector(int disk, int head, int track, int sector, char *destpath)
{
  HANDLE h;
  BYTE *boot;
  BYTE *image;
  DWORD written;
  int retcode = 0;
  int secpertrack;
  int numheads;
  BYTE *sector_addr;
  static char localbuf[512]; // for debug purposes

  if ((h = CreateFile(destpath,
		GENERIC_READ|GENERIC_WRITE,
		0,NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE)
	  return 1;

  switch (disk) {
  case 0:
	  image = partition_start = getfloppyimg(disk);
  	  if (ow_bootsec[disk])
		  boot = ow_bootsec[disk];
	  else
		  boot = partition_start;
	  secpertrack = 18;
	  numheads = 2;
	  break;
  
  case 1:                        /* added by STefan, currently this stuff is hard coded */
	  image = partition_start = getfloppyimg(disk);
  	  if (ow_bootsec[disk])
		  boot = ow_bootsec[disk];
	  else
		  boot = partition_start;
	  secpertrack = 15;
	  numheads = 2;
	  break;


  case 0x80:
	  image = gethdimg();
	  partition_start = image + (* (WORD *) (image + 0x1BE + 8)) * 512;
	  if (ow_bootsec[2])
		  boot = ow_bootsec[2];
	  else
		  boot = partition_start;
	  secpertrack = 17;
	  numheads = 4;
	  break;

  default:
	  return 1;
  }
  computediskparams(boot);
  
  sector_addr = image + 512 * (sector-1 + secpertrack * (track * numheads + head));

  __try {
	  memcpy(localbuf,sector_addr,512);
  }
  __except (HANDLE_ONLY_ACCESS_VIOLATIONS) {
	  fprintf(stderr, "Memory access violation trying to extract sector (%d,%d,%d,%d)!\n",
		disk, head, track, sector);
	  retcode = 1;
  }
  WriteFile(h,localbuf,512,&written,NULL);
  if (written != 512) retcode = 1;
  CloseHandle(h);
  printf("EXTRACTING SECTOR %s\n",destpath); // shows filemapping, STefan
  return retcode;
}



/* This is the main procedure of a test program for the functions above

int main(int argc, char *argv[])
{
	HANDLE in;
	HANDLE inmap;
	DirEntry *entry;
	HANDLE h = NULL;
	char *target;

	// Parsing the parameters

	// Requires at least an image name
	if (argc < 2) {
		exit(1);
	}

	// If an output file name is given, open it
	if (argc > 2) {
		h = CreateFile(argv[2],GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	}

	// If a target name is given, reseach it, otherwise use default target
	if (argc > 3) {
		target = argv[3];
	}
	else {
		target = "\\DOS\\ATTRIB.EXE";
	}

	// Map the image file in memory
	in = CreateFile(argv[1],GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	inmap = CreateFileMapping(in,NULL,PAGE_READWRITE,0,0,NULL);
	image = (PBYTE) MapViewOfFile(inmap,FILE_MAP_WRITE,0,0,0);


	// Start by dumping a few things...
	//dumpsector(image);
	boot = image;
	//dumpboot(boot);
	
	// Initialized important parameters read from the boot sector
	computediskparams(boot);
	
	// Dump a small part of the FAT...
	//printf("FAT:\n");
	//dumpsector(FAT);
	//if (FAT2) {
	//	printf("Copy of FAT:\n");
	//	dumpsector(FAT2);
	//}

	// ...and of the Root directory
	//printf("Root directory:\n");
	//dumpsector(root);
	//dumpentry((DirEntry *) root);
	//dumpentry((DirEntry *) root+1);
	//dumpentry((DirEntry *) root+2);
	//dumpentry((DirEntry *) root+3);
	//dumpentry((DirEntry *) root+4);
	

	// Now, find the target. Exit if failure
	//if (entry = findFileInRoot(target)) {
	if (entry = findPath(target)) {
		printf("\nFound %s [at %08X]\n", target, entry);
		dumpentry(entry);
	}
	else
		exit(1);

	// Small debug dump
	//printf("First %s sector is:\n", target);
	//dumpsector(data(entry->first_clus));
	//printf("Next cluster is # %04X\n", nextc(entry->first_clus));

	// Save virtual file to real file
	writeEntryToHandle(entry, h);


	// That's it
	if (h) {
		CloseHandle(h);
	}

	return 0;
}

*/
