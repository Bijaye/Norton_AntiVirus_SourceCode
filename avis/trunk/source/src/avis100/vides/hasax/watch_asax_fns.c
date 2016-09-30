#include <stdio.h>
#include <ctype.h>
#include "asax.h"
#include "..\bochsw\watcher\watcher.h"
#include "..\bochsw\watcher\watch_ins_tbl.h"
#include "..\Bochs-96\memory.h"

#ifdef MEMMAPDISKIMAGES
/* for real-time file extraction */
#include "..\bochsw\watcher\fatparser\fatpars.h"
#endif

#pragma warning(disable:4035)	// The 'No Return Value' warning

// Here is an empty string, we use when memory allocation fails in the different 'GetString' functions
// (we can't just return NULL, because it is not a valid asax string)
DWORD emptystr = 0; // Strlen is 0, and hopefully, string is 4-byte aligned
#define EMPTYSTR (&emptystr)

void *bitand(m) /* 2 integer */
char *m;
{
   return((void *) (*(DWORD *)m & *(DWORD *)(m+4)));
}

void *bitor(m) /* 2 integer */
char *m;
{
   return((void *) (*(DWORD *)m | *(DWORD *)(m+4)));
}

void *bitxor(m) /* 2 integer */
char *m;
{
   return((void *) (*(DWORD *)m ^ *(DWORD *)(m+4)));
}




// Reads a null-terminated string from Bochs memory
void *readASCIIZ(m)
char *m;
{
	DWORD addr;
	size_t len;
	char *temp;
	WORD seg = (WORD) *(DWORD *)m;
	DWORD offset = *(DWORD *)(m+4);
	size_t maxlen = (size_t) *(DWORD *)(m+8);

	if(linear_addr(seg, offset, &addr))
		return(EMPTYSTR); /* invalid address */
	else
	{
		if(!(temp = tmalloc(maxlen+4, char))) // Alloc with tmalloc -> will be 'garbage-collected'
			return(EMPTYSTR); /* insufficient memory */

#ifdef USEWIN32VIRTUALMEM
		// We are ready for the access, dismiss memory guard momentarily
		dismiss_mem_guard();
#endif

		bx_access_linear(addr, maxlen, 0, BX_READ, &temp[2]);

#ifdef USEWIN32VIRTUALMEM
		// Restore memory protection
		guard_mem();
#endif

		temp[maxlen+2] = 0; // Just to make sure the string has an end
		len = strlen(&temp[2]);
		if(len >= maxlen)
			len = maxlen; /* cut the string if too long */

		// Write length at the beginning of the string (asax style)
		*(WORD *)temp = len;
		return ((void *) temp);
	}
}

// Reads a dollar-terminated string from Bochs memory
void *readASCIIdollar(m)
char *m;
{
	DWORD addr;
	size_t len;
	char *temp;
	WORD seg = (WORD) *(DWORD *)m;
	DWORD offset = *(DWORD *)(m+4);
	size_t maxlen = (size_t) *(DWORD *)(m+8);
	char *dollar;

	if(linear_addr(seg, offset, &addr))
		return(EMPTYSTR); /* invalid address */
	else
	{
		if(!(temp = tmalloc(maxlen+4, char)))
			return(EMPTYSTR); /* insufficient memory */

#ifdef USEWIN32VIRTUALMEM
		// We are ready for the access, dismiss memory guard momentarily
		dismiss_mem_guard();
#endif

		bx_access_linear(addr, maxlen, 0, BX_READ, &temp[2]);

#ifdef USEWIN32VIRTUALMEM
		// Restore memory protection
		guard_mem();
#endif

		temp[maxlen+2] = 0; // Just to make sure the string has an end
		if (dollar = strchr(&temp[2], '$'))
		*dollar = 0;
		len = strlen(&temp[2]);
		if(len >= maxlen)
			len = maxlen; /* cut the string if too long */

		*(WORD *)temp = len;
		return ((void *) temp);
	}
}



// Reads a word from bochs memory

void *readmemword(m)
char *m;
{
	DWORD addr;
	WORD seg = (WORD) *(DWORD *)m;
	DWORD offset = *(DWORD *)(m+4);
	WORD data;

	if(linear_addr(seg, offset, &addr))
		return (void *) 0;
	else {
#ifdef USEWIN32VIRTUALMEM
		dismiss_mem_guard();
#endif
		bx_access_linear(addr, 2, 0, BX_READ, &data);
#ifdef USEWIN32VIRTUALMEM
		guard_mem();
#endif
		return (void *) data;
	}
}





// Function returning a goat name/a command line if there is one left

void *getnextgoat(m)
char *m;
{
	static int g=0;
	char *s;
	unsigned short len;
	extern int numcmdlines;
	extern char **cmdlineslist;

	if (g < numcmdlines) {
		len = strlen(cmdlineslist[g]);
		s = (char *) tmalloc(4*((len+3+1+2)/4), char);
		if(s == NULL) error1(0, 24, "tmalloc failure in watch_asax_fns.c - getnextgoat()");
		strcpy(&s[2], cmdlineslist[g++]);
		* (unsigned short *)s = len;
		return (void *) s;
	}
	else
		return (void *) EMPTYSTR;
}

/* All this for watching management feedback */

void *cancel(m) /* no arguments, cancels the completion of the current instruction */
char *m;
{
   cancel_instruction = 1;
}



// These functions are used to watch parts of the memory

void *mem_attach_marker(m) /* 3 integers: addr, size, marker */
char *m;
{
#ifdef USEWIN32VIRTUALMEM
	DWORD addr  = *(DWORD *)  m;
	DWORD len   = *(DWORD *) (m+4);
	BYTE notify = *(BYTE *)  (m+8);

	audit_mem(addr,len,notify);
#else
	set_guard(*(DWORD *)m, (WORD) *(DWORD *)(m+4), (WORD) *(DWORD *)(m+8));
#endif
}


void *mem_detach_marker(m) /* 1 integer: addr */
char *m;
{
#ifdef USEWIN32VIRTUALMEM
	// impossible for the moment
#else
	dismiss_guard(*(DWORD *)m);
#endif
}

void *start_watch_mem(m)
char *m;
{
#ifdef USEWIN32VIRTUALMEM
	guard_mem();
#else
	watching_mem = 1;
#endif
}

void *stop_watch_mem(m)
char *m;
{
#ifdef USEWIN32VIRTUALMEM
	dismiss_mem_guard();
#else
	watching_mem = 0;
#endif
}





// Register audit (does not work)

void *reg_attach_marker(m) /* 2 integers: reg, marker */
char *m;
{
   reg_markers[*(DWORD *)m] = (WORD) *(DWORD *)(m+4);
}

void *reg_detach_marker(m) /* 1 integer: reg */
char *m;
{
   reg_markers[*(DWORD *)m] = 0;
}







void *start_watch_int(m) /* 1 integer: int watch descriptor */
char *m;
{
	BYTE int_no = * (BYTE *) m;
	DWORD notify_type = * (DWORD *) (m+4);

	// Mark the given interrupt as watched for all functions
	notify[int_no] |= WATCHALLFUNCS|notify_type;
}

void *start_watch_func(m)
char *m;
{
	BYTE int_no = * (BYTE *) m;
	int func = * (int *) (m+4);
	DWORD notify_type = * (DWORD *) (m+8);

	// Mark the given interrupt as watched for some functions
	notify[int_no] |= WATCHSOMEFUNCS;

	// Add this functions in the list of watched events
	w_e_insert(int_no, func, -1, notify_type);
}

void *stop_watch_int(m)
char *m;
{
   int int_no = * (BYTE *) m;

   // Mark interrupt as not watched for all functions
   notify[int_no] &= ~(WATCHALLFUNCS|NOTIFY_BEFORE|NOTIFY_AFTER);
}

void *stop_watch_func(m)
char *m;
{
	BYTE int_no = * (BYTE *) m;
	int func = * (int *) (m+4);
	int i;

	if (w_e_lookup(int_no, func, -1, &i)) {
		// Found this particular function
		memmove(&watched_event[i], &watched_event[w_e_top], sizeof(Watched_event));
		w_e_top--;
	}

	// Maybe we should do something here to reset the WATCHSOMEFUNCS flag when there are no functions left
	// but since it will never happen in the real world, and would not cause a problem anyway...
}






// The following functions deal with names, extensions and priorities of execution


// returns the name, without the extension, of a given file
void *getnameoffile(m)
char *m;
{
	WORD fulllen = **(WORD **) m;
	char *p = (*(char **) m)+2;
	char *res;
	char *q;
	WORD i;

	// Alloc space for fulllen+2 and align on DWORD
	res = tmalloc(((fulllen+5)/4)*4,char); // Some bytes may be unused at the end of the allocated space
	// Return an empty string if the alloc failed
	if (res == NULL)
		return EMPTYSTR;
	// Copy all chars until we find a dot or the end of the input name
	q = res+2;	// leaving the room for the string size
	for (i=0 ; i<fulllen && *p != '.' ; i++)
		*q++ = *p++;
	// now write the string length at the beginning
	*(WORD *) res = i;

	return (void *) res;
}

// returns the extension of a given file
void *getextoffile(m)
char *m;
{
	WORD fulllen = **(WORD **) m;
	char *p = (*(char **) m)+2;
	char *res;
	char *q;
	WORD i,j;

	res = tmalloc(((fulllen+5)/4)*4,char);
	if (res == NULL)
		return EMPTYSTR;
	q = res+2;
	for (i=0 ; i++<fulllen ;)
		if (*p++ == '.') break;
	for (j=0 ; i++<fulllen ; j++)
		*q++ = *p++;
	*(WORD *) res = j;

	return (void *) res;
}

void *priority(m)
char *m;
{
	char ext[4];
	WORD fulllen = **(WORD **) m;
	char *p = (*(char **) m)+2;
	int pr;
	int i;

	memset(ext, 0, 4);
	for (i=0 ; i<fulllen && i<4 ; i++)
		ext[i] = *p++;

	pr = 0;
	if (stricmp(ext, "EXE") == 0)
		pr = 1;
	else if (stricmp(ext, "COM") == 0)
		pr = 2;
	else if (stricmp(ext, "BAT") == 0)
		pr = 3;

	return (void *) pr;
}








void *start_watch_irq(m) /* 1 integer: int watch descriptor */
char *m;
{
   if(!(*(DWORD *)m & 0xfffffff0))
      irq_watch[*(DWORD *)m] = 1;
}

void *stop_watch_irq(m) /* 1 integer: int watch descriptor */
char *m;
{
   if(!(*(DWORD *)m & 0xfffffff0))
      irq_watch[*(DWORD *)m] = 0;
}









// These functions manipulate the audit breakpoints

void *start_watch_brkpt(m)
char *m;
{
	checkAuditBrkPt = 1;
}

void *stop_watch_brkpt(m)
char *m;
{
	checkAuditBrkPt = 0;
}

void *setbrkpt(m)
char *m;
{
	WORD bpCS = *(WORD *)m;
	WORD bpIP = *(WORD *)(m+4);
	int tag = *(int *)(m+8);

	if (brkPtStackTop < BRKPTSTACKSIZE) {
		brkPtStack[brkPtStackTop].brkPtCS = bpCS;
		brkPtStack[brkPtStackTop].brkPtIP = bpIP;
		brkPtStack[brkPtStackTop].linAddr = (((DWORD) bpCS) << 4) + bpIP;
		brkPtStack[brkPtStackTop].tag = tag;
		brkPtStackTop++;
	}
}

// Just a small utility function to choose unique names for the extracted files
static void pick_unique_fname(char *dest) {
	char id;
    FILE *tmp;
    char tmpdest[256];
	int l;

    tmpdest[255] = '\0';
    if ((tmp = fopen(dest, "r")) == NULL) {
		return; // good, the file does not exist, so we can create it
	}

	fclose(tmp);
	l = strlen(dest);
	if (l > 250) {
		*dest = '\0'; // not good, the file exists and there's no place for suffixes, don't OW the file
		return;
	}
	// try other candidates based on the same name, by appending suffixes
	for (id = 2 ; id < 100 ; id++) {
		strcpy(tmpdest,dest);
		strcat(tmpdest,".");
		ltoa(id,&tmpdest[l+1],10);
		if (tmp = fopen(tmpdest, "r")) {
			fclose(tmp); // not good, a file with this suffix exists, keep on going
		}
		else {
			strcat(dest, "."); // good, we found an unused suffix, file can be saved
			ltoa(id,&dest[l+1],10);
			return;
		}
	} // end for each id
	*dest = '\0'; // give up with this file, all suffixes are taken
}

extern char extractdir[]; // Global variable created during the parsing of the options

// This function extracts the given file from the emulated file system to the real disk
// return code is: 0 for no error, non 0 for error
void *extractfile(m)
char *m;
{
  // get the name and put it into an ASCIIZ string, then work with the C-style ASCIIZ string
  WORD namelen = ** (WORD **) m;
  char *asaxname = (* (char **) m) + 2;
  char *name = (char *) malloc(namelen+1);
  int retcode;
  char dest[256];
  char *copyfrom, *p, *q;

  // abort if we couldn't allocate enough for the asciiz string
  if (name == NULL) return (void *) 1;

  // Copy name to uppercase asciiz
  name[namelen] = '\0';
  for (p = name, q = asaxname ; namelen-- ; p++, q++)
	  *p = islower(*q) ? toupper(*q) : *q;

  // Copy the destination directory for extracted files to the destination path, make sure NUL is here
  strncpy(dest, extractdir, 255);
  dest[255] = '\0';
  
  // Find the last backslash in the file name (of the virtual file system) (if it exists)
  if ((copyfrom = strrchr(name, '\\')) == NULL) {
	  strcat(dest, "\\");
	  if ((copyfrom = strchr(name, ':')) == NULL) {
		  copyfrom = name;
	  }
	  else {
		  copyfrom++;
	  }
  }

  // Append the filename to the destination directory
  strncat(dest, copyfrom, 255-strlen(dest));
  pick_unique_fname(dest); // maybe append a suffix to the filename in case a file with the same name was already extracted

#ifdef MEMMAPDISKIMAGES
  if (dest && *dest)
	  retcode = extractitem(name, dest);
  else
#endif
	  retcode = 1;
      /* If we're not using win32 memory-mapped files for the disks, extraction of files is not available */

  free(name);
  return (void *) retcode;
}

void *extractHDMBR(m)
char *m;
{
	int retcode;
    char dest[256];
	strncpy(dest,extractdir,255);
	dest[255] = '\0';
    strncat(dest,"\\HD.MBR",255-strlen(dest));
    pick_unique_fname(dest);
#ifdef MEMMAPDISKIMAGES
    if (dest && *dest)
		retcode = extractsector(0x80,0,0,1,dest);
	else
#endif
		retcode = 1;
	return (void *) retcode;
}

void *extractHDPBS(m)
char *m;
{
	int retcode;
    char dest[256];
	strncpy(dest,extractdir,255);
	dest[255] = '\0';
    strncat(dest,"\\HD.PBS",255-strlen(dest));
    pick_unique_fname(dest);
#ifdef MEMMAPDISKIMAGES
    if (dest && *dest)
		retcode = extractsector(0x80,1,0,1,dest); // check this, I'm not sure the PBS is really there
	else
#endif
		retcode = 1;

	return (void *) retcode;
}

void *extractFDBS(m)
char *m;
{
	int retcode;
    char dest[256];
	strncpy(dest,extractdir,255);
	dest[255] = '\0';
    strncat(dest,"\\FD.BS",255-strlen(dest));
    pick_unique_fname(dest);
#ifdef MEMMAPDISKIMAGES
    if (dest && *dest)
		retcode = extractsector(0,0,0,1,dest);
	else
#endif
		retcode = 1;

	return (void *) retcode;
}

void *extractsec(m)
char *m;
{
    int disk      = (int) *((int *) (m));
	int head      = (int) *((int *) (m+4));
	int track     = (int) *((int *) (m+8));
	int sector    = (int) *((int *) (m+12));
	int maxseccnt = (int) *((int *) (m+16));
	int switchcnt = (int) *((int *) (m+20)); // STefan, counts diskchanges
    
	extern int iPassNumber;
    int seccnt;
#define dMAX_EXT_CNT 200 // maximal count of sectors to extract
					     // some images like blackwor.img flood the drive st
						 // return 1 if limit is reached
	static int iDiskCnt; // helps to save the current switchcnt and to reset extcnt
	static int extcnt;   // extension counter
	int retcode;
    char dest[256];

	if (iDiskCnt != switchcnt) { // STefan
		iDiskCnt = switchcnt;    // save the current switch
		extcnt   = 0;		     // reset the minor extcnt
	}
	for ( seccnt=0; seccnt<maxseccnt; seccnt++ ) // write maxseccnt of sectors to disk
	{
		strncpy(dest,extractdir,255-25); /* switch from 20 to 25 */
		dest[255-25] = '\0';
		sprintf(&dest[strlen(dest)],"\\%02X-%02X-%04X-%02X.%03d_%03d", disk,head,track,sector + seccnt,iPassNumber + switchcnt,extcnt++  );
		pick_unique_fname(dest);
#ifdef MEMMAPDISKIMAGES
		if (dest && *dest && (extcnt <= dMAX_EXT_CNT))
			retcode = extractsector(disk,head,track,sector + seccnt,dest);
		else
#endif
			retcode = 1;
	}
	return (void *) retcode;
}


void *bootTriage(m)
char *m;
{
    extern int boot_triage;
	return (void *) boot_triage;
}

void *replaceFloppyA(m)
char *m;
{
	void bx_replace_disk(int,char*);
	extern char cleanfloppy[];
	bx_replace_disk(0, cleanfloppy);
	return (void *) 0;
}

void *start_watch_io(m) /* 2 integers: addr, size */
char *m;
{
   unsigned int addr, size, shift, tbl_addr;
   addr = *(DWORD *)m;
   size = *(DWORD *)(m+4);

   tbl_addr = addr >> 5; /* we have an array of DWORDS, one bit for each byte in memory */
   if((shift = addr & 0x1f) + size <= 0x20)
      iowatch[tbl_addr] |= (0xffffffff >> (32 - size)) << shift;
   else
   {
      int remain = (shift - size) & 0x1f; /* remaining bytes at the end after the alignment */
      int tbl_size = (size - shift) >> 5;
      iowatch[tbl_addr] |= 0xffffffff << shift;
      iowatch[tbl_addr + 1 + tbl_size] |= 0xffffffff >> remain;
      memset(&iowatch[tbl_addr + 1], 0xff, tbl_size);
   }
}

void *stop_watch_io(m) /* 2 integers: addr, size */
char *m;
{
   unsigned int addr, size, shift, tbl_addr;
   addr = *(DWORD *)m;
   size = *(DWORD *)(m+4);

   tbl_addr = addr >> 5; /* we have an array of DWORDS, one bit for each byte in memory */
   if((shift = addr & 0x1f) + size <= 0x20)
      iowatch[tbl_addr] &= _rotl((0xffffffff << size), shift);
   else
   {
      int remain = (size - shift) & 0x1f; /* remaining bytes at the end after the alignement */
      int tbl_size = (size - shift) >> 5;
      iowatch[tbl_addr] &= 0xffffffff >> (32 - shift);
      iowatch[tbl_addr + 1 + tbl_size] &= 0xffffffff << remain;
      memset(&iowatch[tbl_addr + 1], 0xff, tbl_size);
   }
}

void *stop_program(m) /* 1 string: message */
char *m;
{
	*(char *)(*(DWORD *)m + 2 + *(WORD *)(*(DWORD *)m)) = 0; /* get rid of the trash ASAX leaves at the end of strings. */
   PANIC("Program terminated by ASAX: %s\n", (char *) (*(DWORD *)m + 2));
}

void *toggle_watch_ins(m) /* 2 integers: escape, opcode */
char *m;
{
	unsigned char escape = (BYTE) *(DWORD *)m;
	unsigned char opcode = (BYTE) *(DWORD *)(m+4);
	Instr temp;
	Instr *table;
	Instr *watchtable;

   if(escape)
   {
#if 0
	  temp = instructions_escape[opcode];
      instructions_escape[opcode] = watching_instructions_escape[opcode];
      watching_instructions_escape[opcode] = temp;
      //printf("instruction watch toggled for 0F %X\n",opcode);
#endif
	  switch(escape) {
		case 0x0F:	table = instructions_escape;
					watchtable = watching_instructions_escape;
					break;
		case 0xFF:	table = instructions_group5;
					watchtable = watching_instructions_group5;
					break;
		default:
					return (void *) NULL;
	  }
	  temp = table [opcode];
	  table [opcode] = watchtable [opcode];
	  watchtable [opcode] = temp;
   }
   else
   {
      temp = instructions[opcode];
      instructions[opcode] = watching_instructions[opcode];
      watching_instructions[opcode] = temp;
      //printf("instruction watch toggled for %X\n",opcode);
   }
}





void *seteax(m) /* 1 integer: register content */
char *m;
{
   bx_cpu.eax = *(DWORD *)m;
}

void *setebx(m) /* 1 integer: register content */
char *m;
{
   bx_cpu.ebx = *(DWORD *)m;
}

void *setecx(m) /* 1 integer: register content */
char *m;
{
   bx_cpu.ecx = *(DWORD *)m;
}

void *setedx(m) /* 1 integer: register content */
char *m;
{
   bx_cpu.edx = *(DWORD *)m;
}

void *setesi(m) /* 1 integer: register content */
char *m;
{
   bx_cpu.esi = *(DWORD *)m;
}

void *setedi(m) /* 1 integer: register content */
char *m;
{
   bx_cpu.edi = *(DWORD *)m;
}

void *setebp(m) /* 1 integer: register content */
char *m;
{
   bx_cpu.ebp = *(DWORD *)m;
}

void *setesp(m) /* 1 integer: register content */
char *m;
{
   bx_cpu.esp = *(DWORD *)m;
}

void *seteip(m) /* 1 integer: register content */
char *m;
{
   bx_cpu.eip = *(DWORD *)m;
}

void *setcs(m) /* 1 integer: register content */
char *m;
{
   bx_load_seg_reg(&bx_cpu.cs, (WORD) *(DWORD *)m);
}

void *setds(m) /* 1 integer: register content */
char *m;
{
   bx_load_seg_reg(&bx_cpu.ds, (WORD) *(DWORD *)m);
}

void *setes(m) /* 1 integer: register content */
char *m;
{
   bx_load_seg_reg(&bx_cpu.es, (WORD) *(DWORD *)m);
}

void *setss(m) /* 1 integer: register content */
char *m;
{
   bx_load_seg_reg(&bx_cpu.ss, (WORD) *(DWORD *)m);
}

void *setfs(m) /* 1 integer: register content */
char *m;
{
   bx_load_seg_reg(&bx_cpu.fs, (WORD) *(DWORD *)m);
}

void *setgs(m) /* 1 integer: register content */
char *m;
{
   bx_load_seg_reg(&bx_cpu.gs, (WORD) *(DWORD *)m);
}

// change all this later when I modify eflags
         
void *setcf(m)
char *m;
{
   bx_cpu.eflags.cf = (WORD) *(DWORD *)m;
}

void *setpf(m)
char *m;
{
   // bochs doesn't have a pf yet
//   bx_cpu.eflags.pf = (WORD) *(DWORD *)m;
}

void *setaf(m)
char *m;
{
   bx_cpu.eflags.af = (WORD) *(DWORD *)m;
}

void *setzf(m)
char *m;
{
   bx_cpu.eflags.zf = (WORD) *(DWORD *)m;
}

void *setsf(m)
char *m;
{
   bx_cpu.eflags.sf = (WORD) *(DWORD *)m;
}

void *settf(m)
char *m;
{
   bx_cpu.eflags.tf = (WORD) *(DWORD *)m;
}

void *setif(m)
char *m;
{
   bx_cpu.eflags.if_ = (WORD) *(DWORD *)m;
}

void *setdf(m)
char *m;
{
   bx_cpu.eflags.df = (WORD) *(DWORD *)m;
}

void *setof(m)
char *m;
{
   bx_cpu.eflags.of = (WORD) *(DWORD *)m;
}

void *setiopl(m)
char *m;
{
   bx_cpu.eflags.iopl = (BYTE) *(DWORD *)m;
}

void *setnt(m)
char *m;
{
   bx_cpu.eflags.nt = (WORD) *(DWORD *)m;
}

void *setrf(m)
char *m;
{
   bx_cpu.eflags.rf = (WORD) *(DWORD *)m;
}

void *setvm(m)
char *m;
{
   bx_cpu.eflags.vm = (WORD) *(DWORD *)m;
}

void *setac(m)
char *m;
{
   bx_cpu.eflags.ac = (WORD) *(DWORD *)m;
}

#pragma warning(default:4035)
