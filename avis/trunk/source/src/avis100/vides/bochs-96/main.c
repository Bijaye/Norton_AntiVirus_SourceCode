/*
Copyright Notice
================
BOCHS is Copyright 1994,1995,1996 by Kevin P. Lawton.

BOCHS is commercial software.

For more information, read the file 'LICENSE' included in the bochs
distribution.  If you don't have access to this file, or have questions
regarding the licensing policy, the author may be contacted via:

    US Mail:  Kevin Lawton
              528 Lexington St.
              Waltham, MA 02154

    EMail:    bochs@world.std.com
*/






#define BX_MAIN_C 1

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
   #include <time.h>
#else
   #include <unistd.h>
   #include <sys/time.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>


#include "bochs.h"
#include "iodev/iodev.h"

#include "..\bochsw\watcher\watcher.h"

void hga_update();


/* prototypes */
static void bx_usage(char *progname);
void bx_init_cpu(void);
void bx_init_debug(void);
void bx_cpu_loop(void);
void bx_emulate_hga_dumps_timer(void);

/* typedefs */


/* the device id and stepping id are loaded into DH & DL upon processor
   startup.  for device id: 3 = 80386, 4 = 80486.  just make up a
   number for the stepping (revision) id. */
#define BX_DEVICE_ID     2
#define BX_STEPPING_ID   0


#if BX_EXTERNAL_ENVIRONMENT==0
bx_cpu_t bx_cpu;
#endif

#if 1 /* bill */
unsigned long revert=0L, max_count=0L;
int switchC=0, lock_floppy=0;
time_t time_ofs=0; 
int iconic=0, novideo=0;
#endif

#if 1 /* STefan */
int iPassNumber;  /* counter the runs of the emulator */
#endif

int g_hurry_up = 0; /* riad */
int g_do_boo   = 0; /* riad */
int boot_triage = 0;
char cleanfloppy[256];
int sectimeout = 0; // Timeout in seconds. Fred
char extractdir[256] = ".\\extract"; // Where to put the extracted replicants
int numcmdlines = 0;
char **cmdlineslist = NULL; // The list of command lines to send to the keyboard
char *ow_bootsec[3]; // The boot sectors to be used instead of the ones on the disks
#ifdef FOR_BOO
char boo_sections[256] = ".\\sections"; // The file where the record the code sections for boot sectors
#endif

FILE *bx_logfd; /* for logging bx_printf() messages */



void bx_sanity_checks(void);




Bit8u bx_io_handler_id[0x10000];  /* 64K */
bx_io_handler_t bx_io_handler[BX_MAX_IO_DEVICES];

/* more for informative purposes, the names of the devices which
 * are use each of the IRQ 0..15 lines are stored here
 */
char *bx_irq_handler_name[BX_MAX_IRQS];

/* number of currently registered io handlers (devices) */
Bit8u bx_num_io_handles = 0;




/* for efficient repetitive checking of multiple ansynchronous
 * events, the bx_async_event flag is set if any async event occur.
 * That is, for timer events, file descriptor events, and by the PIC.
 */
volatile Boolean bx_async_event = 0;
   
/* after an asynchronous event is determined, the following flags
 * determine which type of event(s) occured */

volatile Boolean bx_fd_event    = 0; /* a file descriptor event occured */




/* for now, if an INTR occurs and is not masked, a C function is called
 * to handle the interrupt.  Functions register themselves with and
 * associated interrupt number 0..255 by means of bx_register_int_vector()
 * NOTE: IRQ's 0..7  = interrupt 08h..0Fh
 *       IRQ's 8..15 = interrupt 70h..77h
 */
void (*bx_interrupt_table[256])(int);


/* To allow IO devices to register functions to be called when events
 * occur on associated file descriptors, we need to define a set of
 * file descriptors to select() on, and a couple arrays to hold the
 * file descriptors, and the function pointers
 */
int max_fd = 0; /* maximum fd (to pass to select() ) */


int registered_fds[BX_MAX_REGISTERED_FDS]; /* values of fd's registered */
void (* registered_fd_handler[BX_MAX_REGISTERED_FDS])(void); /* function ptrs*/
int num_registered_fds = 0; /* number of currently registered fd's */



bx_debug_t bx_dbg;


/* local prototypes */
void bx_iodev_init(void);




bx_options_t bx_options = {
  { "", 0 },
  { "", 0 },
  { "" },
  { NULL },
  "a",
  300000
  };

static char bochsrc_path[512];
static char logfilename[512] = "-";

static void parse_line(int num_params, char *params[]);
static void parse_bochsrc(void);


void bx_iodev_init_hardware(void);

  int
#if BX_EXTERNAL_ENVIRONMENT==0
main
#else
bx_bochs_init
#endif
  (int argc, char *argv[])
{
  int n;

  /* read the .bochsrc file */
  parse_bochsrc();

  n = 2;
  while (n <= argc) {
    if (!strcmp(argv[n-1], "-log")) {
      strcpy(logfilename, argv[n]);
      n += 2;
      }
#if BX_EXTERNAL_ENVIRONMENT==0
    else if (!strcmp(argv[n-1], "-sanity-check")) {
      bx_sanity_checks();
      n += 1;
      exit(0);
      }
#endif
    else if (!strcmp(argv[n-1], "-bootA")) {
      strcpy(bx_options.bootdrive, "a");
      n += 1;
      }
    else if (!strcmp(argv[n-1], "-bootC")) {
      strcpy(bx_options.bootdrive, "c");
      n += 1;
      }
#if 1 /* bill */
    else if (strstr(argv[n-1], "-timeout=")) {
      revert = 1L; 
      max_count = atol(&argv[n-1][9]);
      n += 1;
    }
    else if (strstr(argv[n-1], "-sectimeout=")) { //Fred
      sectimeout = atol(&argv[n-1][12]);
      n += 1;
    }
    else if (!strcmp(argv[n-1], "-switch")) {
      switchC = 1;
      n += 1;
    }
    else if (!strcmp(argv[n-1], "-lock")) {
      lock_floppy = 1;
      n += 1;
    }
    else if (strstr(argv[n-1], "-imgA=")) {
      strcpy(bx_options.floppya.path, &argv[n-1][6]);
      bx_options.floppya.type = BX_FLOPPY_AUTO;
      n++;
    }
    else if (strstr(argv[n-1], "-imgB=")) {
      strcpy(bx_options.floppyb.path, &argv[n-1][6]);
      n++;
    }
    else if (strstr(argv[n-1], "-imgC=")) {   
      strcpy(bx_options.diskc.path, &argv[n-1][6]);
      n++;
    }
    else if (strstr(argv[n-1], "-time=")) {
      time_ofs = ((time_t)atol(&argv[n-1][6]))-time(NULL);
      n++;
    }
    else if (!strcmp(argv[n-1], "-iconic")) {
      iconic = 1;
      n++;
    }
    else if (!strcmp(argv[n-1], "-novideo")) {
      novideo = 1;
      n++;
    }
	else if (!strcmp(argv[n-1], "-hurry")) { /* riad */
	  g_hurry_up = 1;
	  n++;
	}
   else if (strstr(argv[n-1], "-module=") == argv[n-1]) {
     strcpy(asaxmodule, &argv[n-1][8]);
     n++;
   }
   else if (strstr(argv[n-1], "-sample=") == argv[n-1]) {
     strcpy(samplename, &argv[n-1][8]);
     n++;
   }
   else if (strstr(argv[n-1], "-descrfile=") == argv[n-1]) {
     strcpy(asaxdescrfile, &argv[n-1][11]);   
     n++;
   }
   else if (strstr(argv[n-1], "-virusreport=") == argv[n-1]) {
     strcpy(virusreport, &argv[n-1][13]);   
     n++;
   }
   else if (strstr(argv[n-1], "-pass=")) {  /* STefan */
      iPassNumber = (atoi(&argv[n-1][6]));  /* get the new run number */
      n++;
    }

   else if (strstr(argv[n-1], "-extract=") == argv[n-1]) { // Fred
     strncpy(extractdir, &argv[n-1][9], 255);
	 extractdir[255] = '\0';
     n++;
   }

   else if (strstr(argv[n-1], "-cmdlines=") == argv[n-1]) { // Fred
	 char cmdlinesfile[256];
	 char cmdline[256];
	 FILE *cmds;
     strncpy(cmdlinesfile, &argv[n-1][10], 255);
	 cmdlinesfile[255] = '\0';
     if ((cmds = fopen(cmdlinesfile, "r")) == NULL) {
		 fprintf(stderr, "Can't open %s for reading\n", cmdlinesfile);
		 exit(1);
	 }
	 cmdline[255] = '\0';
     while (fgets(cmdline, 255, cmds)) {
		 if (cmdline[strlen(cmdline)-1] == '\n')
			 cmdline[strlen(cmdline)-1] = '\0';
		 if ((cmdlineslist = (char **) realloc(cmdlineslist, ++numcmdlines * sizeof(char *))) == NULL) {
			 fprintf(stderr, "realloc failed\n");
			 exit(1);
		 }
		 if ((cmdlineslist[numcmdlines-1] = (char *) malloc(strlen(cmdline)+1)) == NULL) {
			 fprintf(stderr, "malloc failed\n");
			 exit(1);
		 }
		 strcpy(cmdlineslist[numcmdlines-1], cmdline);
	 }
	 fclose(cmds);
     n++;
   }

   else if (strstr(argv[n-1], "-usePBS=") == argv[n-1]) { // Fred
	   char *pbslist;
	   char pbspath[256];
	   FILE *pbshnd;
	   pbspath [255] = '\0';
	   pbslist = &argv[n-1][7]; // points to the equal sign

	   while (pbslist && *pbslist) {
		   pbslist++;
		   /* Check it starts with a valid drive letter and a coma */
		   if (*pbslist >= 'A' && *pbslist <= 'C' && *(pbslist+1) == ',') {
			   char *nextpbs;
			   int disknum = *pbslist - 'A';

			   /* Alloc the memory for the boot sector */
			   if (ow_bootsec[disknum]) {
				   fprintf(stderr, "2 PBS's for the same disk is 1 PBS too much!\n");
				   exit(1);
			   }
			   else {
				   if ((ow_bootsec[disknum] = malloc (512)) == NULL) {
					   fprintf(stderr, "malloc failed!\n");
					   exit(1);
				   }
			   }

			   /* Extract the path to the PBS file */
			   nextpbs = strchr(pbslist, ';'); // points to the first ';' or NULL
			   if (nextpbs) *nextpbs = '\0';
			   strncpy(pbspath, pbslist+2, 255); // copy the path to the file containing the PBS

			   /* Read the file */
			   if ((pbshnd = fopen (pbspath, "rb")) == NULL) {
				   fprintf(stderr, "Can't open %s!\n", pbspath);
				   exit(1);
			   }
			   if (fread(ow_bootsec[disknum],512,1,pbshnd) != 1) {
				   fprintf(stderr, "Can't read %s!\n", pbspath);
				   exit(1);
			   }
			   fclose(pbshnd);

			   if (pbslist = nextpbs) *pbslist = '='; // do next pbs
		   }
		   else
			   pbslist = NULL;
	   }
	   n++;
   }

#ifdef FOR_BOO
    else if (strstr(argv[n-1], "-boottriage=") == argv[n-1]) {
      boot_triage = 1;
	  strncpy(cleanfloppy, &argv[n-1][12], 255);
	  n++;
    }
    else if (strstr(argv[n-1], "-boo") == argv[n-1]) { /* moved riads code, stefan */
      g_do_boo = 1;
	  if (argv[n-1][4] == '=') {
		  strncpy(boo_sections, &argv[n-1][5], 255);
		  n++;
	  }
	  /* n++; */                                       /* moved stefan */
    }
#endif
//#ifdef AUDIT_ENABLED
//    else if (strstr(argv[n-1], "-audit=")) {
//      strcpy(audit_fname, &argv[n-1][7]);   
//      n++;
//    }
//    else if (!strcmp(argv[n-1], "-noaudit")) {
//      noaudit=1;
//      n++;
//    }
//#endif
#endif /* if 1 */

    else if ((n<argc) && !strcmp(argv[n-1], "-hga_update_interval")) {
      bx_options.hga_update_interval = atol(argv[n]);
      if (bx_options.hga_update_interval < 50000) {
        fprintf(stderr, "hga_update_interval not big enough!\n");
        bx_usage(argv[0]);
        }
      n += 2;
      }
    else {
      bx_usage(argv[0]);
      }
    }

  if (!strcmp(logfilename, "-")) {
    bx_logfd = stderr;
    }
  else {
    bx_logfd = fopen(logfilename, "w");
    if (!bx_logfd) {
      fprintf(stderr, "could not open log file '%s'\n", logfilename);
      exit(1);
      }
    }

#if BX_EXTERNAL_ENVIRONMENT == 0
  bx_init_cpu();
  bx_init_memory(BX_PHY_MEM_SIZE);
#endif
  bx_init_debug();
  bx_iodev_init();
  bx_iodev_init_hardware();




#if BX_EXTERNAL_ENVIRONMENT==0

#if BX_EMULATE_HGA_DUMPS>0
  bx_register_timer( bx_emulate_hga_dumps_timer, BX_EMULATE_HGA_DUMPS, 1, 1);
#endif

  if (bx_options.rom.path == NULL) {
    /* give bios POST to do minimal setup of the post routine */
    bx_post_setup();
    }
  else {
    struct stat stat_buf;
    int fd, ret;
    unsigned long size, offset;

    /* read in ROM BIOS image file */
    if (bx_options.rom.path == NULL)
      bx_panic("main.c: ROM image file not specified.\n");
#ifdef WIN32
    fd = open(bx_options.rom.path, O_BINARY | O_RDONLY);
#else
    fd = open(bx_options.rom.path, O_RDONLY);
#endif
    if (fd < 0)
      bx_panic("main.c: couldn't open ROM image file '%s'.\n",
        bx_options.rom.path);
    ret = fstat(fd, &stat_buf);
    if (ret)
      bx_panic("main.c: couldn't stat ROM image file '%s'.\n",
        bx_options.rom.path);
    if (stat_buf.st_size != 65536)
      bx_panic("main.c: ROM image file '%s' not 64K bytes in size\n",
        bx_options.rom.path);

    size = 65536;
    offset = 0;
    while (size > 0) {
      ret = read(fd, &bx_phy_memory[0xf0000 + offset], size);
      if (ret <= 0) {
        bx_panic("main.c: read failed on ROM BIOS image\n");
        }
      size   -= ret;
      offset += ret;
      }
    }
#endif /* BX_EXTERNAL_ENVIRONMENT==0 */

  bx_start_timers();

//#ifdef AUDIT_ENABLED
//  init_audit(audit_fname);
//  if (noaudit) audit_select = 0;
//#endif

#if WATCH_ENABLED /* Hooman */
printf("asaxinit\n");
  if(watcher_init())
     bx_panic("watcher initialization failed\n");
#endif

#if BX_EXTERNAL_ENVIRONMENT==0
  bx_cpu_loop();
#endif

  return(0);
}


static void
bx_usage(char *progname)
{
   fprintf(stderr, "Usage: %s <-log logfilename> <-sanity-check> <-bootA/-bootC>\n"
      "          <-timeout=number> <-switch> <-lock> <-imgA/-imgB/-imgC=filename>\n"
      "          <-time=number> <-iconic> <-novideo> <-sample=filename>\n"// <-audit=filename> <-noaudit>\n"
      "          <-module=filename> <-descrfile=filename> <-virusreport=filename>\n"
	  "          <-hga_update_interval>\n\n"
      "    Filenames are a maximum of 255 characters long.\n\n", progname);
  exit(-1);
}


  void
bx_init_debug(void)
{
  bx_dbg.floppy = 0;
  bx_dbg.keyboard = 0;
  bx_dbg.video = 0;
  bx_dbg.disk = 0;
  bx_dbg.pit = 0;
  bx_dbg.pic = 0;
  bx_dbg.bios = 0;
  bx_dbg.cmos = 0;
  bx_dbg.protected = 0;
  bx_dbg.a20 = 0;
  bx_dbg.interrupts = 0;
  bx_dbg.exceptions = 0;
  bx_dbg.unsupported = 0;
  bx_dbg.temp = 0;
  bx_dbg.reset = 0;
  bx_dbg.mouse = 0;
  bx_dbg.io = 0;
  bx_dbg.debugger = 1;
  bx_dbg.xms = 0;
  bx_dbg.v8086 = 0;
  bx_dbg.paging = 0;
  bx_dbg.creg = 0;
  bx_dbg.dreg = 1;
}

#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_init_cpu(void)
{
  /* general registers */
  bx_cpu.eax = 0; /* processor passed test :-) */
  bx_cpu.ebx = 0; /* undefined */
  bx_cpu.ecx = 0; /* undefined */
  bx_cpu.edx = (BX_DEVICE_ID << 8) | BX_STEPPING_ID; /* ??? */
  bx_cpu.ebp = 0; /* undefined */
  bx_cpu.esi = 0; /* undefined */
  bx_cpu.edi = 0; /* undefined */
  bx_cpu.esp = 0; /* undefined */

  /* status and control flags register set */
  bx_set_CF(0);
  bx_cpu.eflags.bit1 = 1;
  bx_set_PF(0);
  bx_cpu.eflags.bit3 = 0;
  bx_set_AF(0);
  bx_cpu.eflags.bit5 = 0;
  bx_set_ZF(0);
  bx_set_SF(0);
  bx_cpu.eflags.tf = 0;
  bx_cpu.eflags.if_ = 0;
  bx_cpu.eflags.df = 0;
  bx_set_OF(0);
#if BX_CPU >= 2
  bx_cpu.eflags.iopl = 0;
  bx_cpu.eflags.nt = 0;
#endif
  bx_cpu.eflags.bit15 = 0;
#if BX_CPU >= 3
  bx_cpu.eflags.rf = 0;
  bx_cpu.eflags.vm = 0;
#endif
#if BX_CPU >= 4
  bx_cpu.eflags.ac = 0;
#endif

  bx_cpu.inhibit_interrupts = 0;
  bx_single_step_event = 0;

  /* instruction pointer */
#if BX_CPU < 2
  bx_cpu.eip = 0x00000000;
#else /* from 286 up */
  bx_cpu.eip = 0x0000FFF0;
#endif


  /* CS (Code Segment) and descriptor cache */
  /* Note: on a real cpu, CS initially points to upper memory.  After
   * the 1st jump, the descriptor base is zero'd out.  Since I'm just
   * going to jump to my BIOS, I don't need to do this.
   * For future reference:
   *   processor  cs.selector   cs.base    cs.limit    EIP
   *        8086    FFFF          FFFF0        FFFF   0000
   *        286     F000         FF0000        FFFF   FFF0
   *        386+    F000       FFFF0000        FFFF   FFF0
   */
  bx_cpu.cs.selector.value =     0xf000;
#if BX_CPU >= 2
  bx_cpu.cs.selector.index =     0x0000;
  bx_cpu.cs.selector.ti = 0;
  bx_cpu.cs.selector.rpl = 0;

  bx_cpu.cs.cache.valid =     1;
  bx_cpu.cs.cache.p = 1;
  bx_cpu.cs.cache.dpl = 0;
  bx_cpu.cs.cache.segment = 1; /* data/code segment */
  bx_cpu.cs.cache.type = 0; /* not used for code seg, 0=invalid */

  bx_cpu.cs.cache.u.segment.executable   = 0; /* data/stack segment */
  bx_cpu.cs.cache.u.segment.c_ed         = 0; /* normal expand up */
  bx_cpu.cs.cache.u.segment.r_w          = 1; /* writeable */
  bx_cpu.cs.cache.u.segment.a            = 1; /* accessed */
  bx_cpu.cs.cache.u.segment.base         = 0x000F0000;
  bx_cpu.cs.cache.u.segment.limit        =     0xFFFF;
  bx_cpu.cs.cache.u.segment.limit_scaled =     0xFFFF;
#endif
#if BX_CPU >= 3
  bx_cpu.cs.cache.u.segment.g   = 0; /* byte granular */
  bx_cpu.cs.cache.u.segment.d_b = 0; /* 16bit default size */
  bx_cpu.cs.cache.u.segment.avl = 0;
#endif


  /* SS (Stack Segment) and descriptor cache */
  bx_cpu.ss.selector.value =     0x0000;
#if BX_CPU >= 2
  bx_cpu.ss.selector.index =     0x0000;
  bx_cpu.ss.selector.ti = 0;
  bx_cpu.ss.selector.rpl = 0;

  bx_cpu.ss.cache.valid =     1;
  bx_cpu.ss.cache.p = 1;
  bx_cpu.ss.cache.dpl = 0;
  bx_cpu.ss.cache.segment = 1; /* data/code segment */
  bx_cpu.ss.cache.type = 0; /* not used for code seg, 0=invalid */

  bx_cpu.ss.cache.u.segment.executable   = 0; /* data/stack segment */
  bx_cpu.ss.cache.u.segment.c_ed         = 0; /* normal expand up */
  bx_cpu.ss.cache.u.segment.r_w          = 1; /* writeable */
  bx_cpu.ss.cache.u.segment.a            = 1; /* accessed */
  bx_cpu.ss.cache.u.segment.base         = 0x00000000;
  bx_cpu.ss.cache.u.segment.limit        =     0xFFFF;
  bx_cpu.ss.cache.u.segment.limit_scaled =     0xFFFF;
#endif
#if BX_CPU >= 3
  bx_cpu.ss.cache.u.segment.g   = 0; /* byte granular */
  bx_cpu.ss.cache.u.segment.d_b = 0; /* 16bit default size */
  bx_cpu.ss.cache.u.segment.avl = 0;
#endif


  /* DS (Data Segment) and descriptor cache */
  bx_cpu.ds.selector.value =     0x0000;
#if BX_CPU >= 2
  bx_cpu.ds.selector.index =     0x0000;
  bx_cpu.ds.selector.ti = 0;
  bx_cpu.ds.selector.rpl = 0;

  bx_cpu.ds.cache.valid =     1;
  bx_cpu.ds.cache.p = 1;
  bx_cpu.ds.cache.dpl = 0;
  bx_cpu.ds.cache.segment = 1; /* data/code segment */
  bx_cpu.ds.cache.type = 0; /* not used for code seg, 0=invalid */

  bx_cpu.ds.cache.u.segment.executable   = 0; /* data/stack segment */
  bx_cpu.ds.cache.u.segment.c_ed         = 0; /* normal expand up */
  bx_cpu.ds.cache.u.segment.r_w          = 1; /* writeable */
  bx_cpu.ds.cache.u.segment.a            = 1; /* accessed */
  bx_cpu.ds.cache.u.segment.base         = 0x00000000;
  bx_cpu.ds.cache.u.segment.limit        =     0xFFFF;
  bx_cpu.ds.cache.u.segment.limit_scaled =     0xFFFF;
#endif
#if BX_CPU >= 3
  bx_cpu.ds.cache.u.segment.g   = 0; /* byte granular */
  bx_cpu.ds.cache.u.segment.d_b = 0; /* 16bit default size */
  bx_cpu.ds.cache.u.segment.avl = 0;
#endif


  /* ES (Extra Segment) and descriptor cache */
  bx_cpu.es.selector.value =     0x0000;
#if BX_CPU >= 2
  bx_cpu.es.selector.index =     0x0000;
  bx_cpu.es.selector.ti = 0;
  bx_cpu.es.selector.rpl = 0;

  bx_cpu.es.cache.valid =     1;
  bx_cpu.es.cache.p = 1;
  bx_cpu.es.cache.dpl = 0;
  bx_cpu.es.cache.segment = 1; /* data/code segment */
  bx_cpu.es.cache.type = 0; /* not used for code seg, 0=invalid */

  bx_cpu.es.cache.u.segment.executable   = 0; /* data/stack segment */
  bx_cpu.es.cache.u.segment.c_ed         = 0; /* normal expand up */
  bx_cpu.es.cache.u.segment.r_w          = 1; /* writeable */
  bx_cpu.es.cache.u.segment.a            = 1; /* accessed */
  bx_cpu.es.cache.u.segment.base         = 0x00000000;
  bx_cpu.es.cache.u.segment.limit        =     0xFFFF;
  bx_cpu.es.cache.u.segment.limit_scaled =     0xFFFF;
#endif
#if BX_CPU >= 3
  bx_cpu.es.cache.u.segment.g   = 0; /* byte granular */
  bx_cpu.es.cache.u.segment.d_b = 0; /* 16bit default size */
  bx_cpu.es.cache.u.segment.avl = 0;
#endif


  /* FS and descriptor cache */
#if BX_CPU >= 3
  bx_cpu.fs.selector.value =     0x0000;
  bx_cpu.fs.selector.index =     0x0000;
  bx_cpu.fs.selector.ti = 0;
  bx_cpu.fs.selector.rpl = 0;

  bx_cpu.fs.cache.valid =     1;
  bx_cpu.fs.cache.p = 1;
  bx_cpu.fs.cache.dpl = 0;
  bx_cpu.fs.cache.segment = 1; /* data/code segment */
  bx_cpu.fs.cache.type = 0; /* not used for code seg, 0=invalid */

  bx_cpu.fs.cache.u.segment.executable   = 0; /* data/stack segment */
  bx_cpu.fs.cache.u.segment.c_ed         = 0; /* normal expand up */
  bx_cpu.fs.cache.u.segment.r_w          = 1; /* writeable */
  bx_cpu.fs.cache.u.segment.a            = 1; /* accessed */
  bx_cpu.fs.cache.u.segment.base         = 0x00000000;
  bx_cpu.fs.cache.u.segment.limit        =     0xFFFF;
  bx_cpu.fs.cache.u.segment.limit_scaled =     0xFFFF;
  bx_cpu.fs.cache.u.segment.g   = 0; /* byte granular */
  bx_cpu.fs.cache.u.segment.d_b = 0; /* 16bit default size */
  bx_cpu.fs.cache.u.segment.avl = 0;
#endif


  /* GS and descriptor cache */
#if BX_CPU >= 3
  bx_cpu.gs.selector.value =     0x0000;
  bx_cpu.gs.selector.index =     0x0000;
  bx_cpu.gs.selector.ti = 0;
  bx_cpu.gs.selector.rpl = 0;

  bx_cpu.gs.cache.valid =     1;
  bx_cpu.gs.cache.p = 1;
  bx_cpu.gs.cache.dpl = 0;
  bx_cpu.gs.cache.segment = 1; /* data/code segment */
  bx_cpu.gs.cache.type = 0; /* not used for code seg, 0=invalid */

  bx_cpu.gs.cache.u.segment.executable   = 0; /* data/stack segment */
  bx_cpu.gs.cache.u.segment.c_ed         = 0; /* normal expand up */
  bx_cpu.gs.cache.u.segment.r_w          = 1; /* writeable */
  bx_cpu.gs.cache.u.segment.a            = 1; /* accessed */
  bx_cpu.gs.cache.u.segment.base         = 0x00000000;
  bx_cpu.gs.cache.u.segment.limit        =     0xFFFF;
  bx_cpu.gs.cache.u.segment.limit_scaled =     0xFFFF;
  bx_cpu.gs.cache.u.segment.g   = 0; /* byte granular */
  bx_cpu.gs.cache.u.segment.d_b = 0; /* 16bit default size */
  bx_cpu.gs.cache.u.segment.avl = 0;
#endif


  /* GDTR (Global Descriptor Table Register) */
#if BX_CPU >= 2
  bx_cpu.gdtr.base         = 0x00000000;  /* undefined */
  bx_cpu.gdtr.limit        =     0x0000;  /* undefined */
  /* ??? AR=Present, Read/Write */
#endif

  /* IDTR (Interrupt Descriptor Table Register) */
#if BX_CPU >= 2
  bx_cpu.idtr.base         = 0x00000000;
  bx_cpu.idtr.limit        =     0x03FF; /* always byte granular */ /* ??? */
  /* ??? AR=Present, Read/Write */
#endif

  /* LDTR (Local Descriptor Table Register) */
#if BX_CPU >= 2
  bx_cpu.ldtr.selector.value =     0x0000;
  bx_cpu.ldtr.selector.index =     0x0000;
  bx_cpu.ldtr.selector.ti = 0;
  bx_cpu.ldtr.selector.rpl = 0;

  bx_cpu.ldtr.cache.valid   = 0; /* not valid */
  bx_cpu.ldtr.cache.p       = 0; /* not present */
  bx_cpu.ldtr.cache.dpl     = 0; /* field not used */
  bx_cpu.ldtr.cache.segment = 0; /* system segment */
  bx_cpu.ldtr.cache.type    = 2; /* LDT descriptor */

  bx_cpu.ldtr.cache.u.ldt.base      = 0x00000000;
  bx_cpu.ldtr.cache.u.ldt.limit     =     0xFFFF;
#endif

  /* TR (Task Register) */
#if BX_CPU >= 2
  /* ??? I don't know what state the TR comes up in */
  bx_cpu.tr.selector.value =     0x0000;
  bx_cpu.tr.selector.index =     0x0000; /* undefined */
  bx_cpu.tr.selector.ti    =     0;
  bx_cpu.tr.selector.rpl   =     0;

  bx_cpu.tr.cache.valid    = 0;
  bx_cpu.tr.cache.p        = 0;
  bx_cpu.tr.cache.dpl      = 0; /* field not used */
  bx_cpu.tr.cache.segment  = 0;
  bx_cpu.tr.cache.type     = 0; /* invalid */
  bx_cpu.tr.cache.u.tss286.base             = 0x00000000; /* undefined */
  bx_cpu.tr.cache.u.tss286.limit            =     0x0000; /* undefined */
#endif

  /* DR0 - DR7 (Debug Register 0 to 7), unimplemented */
#if BX_CPU >= 3
  bx_cpu.dr0 = 0;   /* undefined */
  bx_cpu.dr1 = 0;   /* undefined */
  bx_cpu.dr2 = 0;   /* undefined */
  bx_cpu.dr3 = 0;   /* undefined */
#endif
#if BX_CPU >= 3
  bx_cpu.dr4 = 0;   /* undefined */
  bx_cpu.dr5 = 0;   /* undefined */
#endif
#if   BX_CPU == 3
  bx_cpu.dr6 = 0xFFFF0FF0;   /* ??? unimplemented */
  bx_cpu.dr7 = 0x00000000;   /* unimplemented */
#elif BX_CPU == 4
  bx_cpu.dr6 = 0xFFFF0FF0;   /* unimplemented */
  bx_cpu.dr7 = 0x00000000;   /* unimplemented */
#elif BX_CPU == 5
  bx_cpu.dr6 = 0xFFFF0FF0;   /* unimplemented */
  bx_cpu.dr7 = 0x00000400;   /* unimplemented */
#endif

#if 0
  /* test registers 3-7 (unimplemented) */
  bx_cpu.tr3 = 0;   /* undefined */
  bx_cpu.tr4 = 0;   /* undefined */
  bx_cpu.tr5 = 0;   /* undefined */
  bx_cpu.tr6 = 0;   /* undefined */
  bx_cpu.tr7 = 0;   /* undefined */
#endif

  /* MSW (Machine Status Word), so called on 286 */
  /* CR0 (Control Register 0), so called on 386+ */
#if BX_CPU >= 3
  bx_cpu.cr0.pg = 0; /* paging disabled */
#endif
#if BX_CPU >= 4
  bx_cpu.cr0.cd = 1; /* caching disabled */
  bx_cpu.cr0.nw = 1; /* not write-through */
  bx_cpu.cr0.am = 0; /* disable alignment check */
  bx_cpu.cr0.wp = 0; /* disable write-protect */
  bx_cpu.cr0.ne = 0; /* ndp exceptions through int 13H, DOS compat */
#endif
#if BX_CPU >= 2
  bx_cpu.cr0.ts = 0; /* no task switch */
  bx_cpu.cr0.em = 0; /* emulate math coprocessor */
  bx_cpu.cr0.mp = 0; /* wait instructions not trapped */
  bx_cpu.cr0.pe = 0; /* real mode */
#endif

#if BX_CPU >= 3
  bx_cpu.cr3 = 0;
#endif


  bx_cpu.EXT = 0;
  bx_cpu.INTR = 0;
}
#endif


#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_sanity_checks(void)
{
  Bit8u al, cl, dl, bl, ah, ch, dh, bh;
  Bit16u ax, cx, dx, bx, sp, bp, si, di;
  Bit32u eax, ecx, edx, ebx, esp, ebp, esi, edi;

  bx_cpu.eax = 0xFFEEDDCC;
  bx_cpu.ecx = 0xBBAA9988;
  bx_cpu.edx = 0x77665544;
  bx_cpu.ebx = 0x332211FF;
  bx_cpu.esp = 0xEEDDCCBB;
  bx_cpu.ebp = 0xAA998877;
  bx_cpu.esi = 0x66554433;
  bx_cpu.edi = 0x2211FFEE;

  al = AL;
  cl = CL;
  dl = DL;
  bl = BL;
  ah = AH;
  ch = CH;
  dh = DH;
  bh = BH;

  if ( al != (bx_cpu.eax & 0xFF) ||
       cl != (bx_cpu.ecx & 0xFF) ||
       dl != (bx_cpu.edx & 0xFF) ||
       bl != (bx_cpu.ebx & 0xFF) ||
       ah != ((bx_cpu.eax >> 8) & 0xFF) ||
       ch != ((bx_cpu.ecx >> 8) & 0xFF) ||
       dh != ((bx_cpu.edx >> 8) & 0xFF) ||
       bh != ((bx_cpu.ebx >> 8) & 0xFF) ) {
    bx_panic("problems using BX_READ_8BIT_REG()!\n");
    }

  ax = AX;
  cx = CX;
  dx = DX;
  bx = BX;
  sp = SP;
  bp = BP;
  si = SI;
  di = DI;

  if ( ax != (bx_cpu.eax & 0xFFFF) ||
       cx != (bx_cpu.ecx & 0xFFFF) ||
       dx != (bx_cpu.edx & 0xFFFF) ||
       bx != (bx_cpu.ebx & 0xFFFF) ||
       sp != (bx_cpu.esp & 0xFFFF) ||
       bp != (bx_cpu.ebp & 0xFFFF) ||
       si != (bx_cpu.esi & 0xFFFF) ||
       di != (bx_cpu.edi & 0xFFFF) ) {
    bx_panic("problems using BX_READ_16BIT_REG()!\n");
    }


  eax = EAX;
  ecx = ECX;
  edx = EDX;
  ebx = EBX;
  esp = ESP;
  ebp = EBP;
  esi = ESI;
  edi = EDI;
  

  if ( eax != bx_cpu.eax ||
       ecx != bx_cpu.ecx ||
       edx != bx_cpu.edx ||
       ebx != bx_cpu.ebx ||
       esp != bx_cpu.esp ||
       ebp != bx_cpu.ebp ||
       esi != bx_cpu.esi ||
       edi != bx_cpu.edi ) {
    bx_panic("problems using BX_READ_32BIT_REG()!\n");
    }


  if (sizeof(Bit8u)  != 1  ||  sizeof(Bit8s)  != 1)
    bx_panic("data type Bit8u or Bit8s is not of length 1 byte!\n");
  if (sizeof(Bit16u) != 2  ||  sizeof(Bit16s) != 2)
    bx_panic("data type Bit16u or Bit16s is not of length 2 bytes!\n");
  if (sizeof(Bit32u) != 4  ||  sizeof(Bit32s) != 4)
    bx_panic("data type Bit32u or Bit32s is not of length 4 bytes!\n");
  
  fprintf(stderr, "all sanity checks passed!\n");
}
#endif


/* bx_printf()
 *
 * To print warnings, errors, etc, use the variable argument list
 * bx_printf() function.
 *
 * bomb_out       0 = continue, 1 = exit
 * ...            the format and parameters typically passed to printf()
 */

#ifdef MYSTUFF /* riad */
#define MAXPRINTTRASH  4096
static unsigned trashcounter = 0;
#endif
  void
bx_printf(char *fmt, ...)
{
  va_list ap;
#ifdef MYSTUFF /* riad */
  /* to avoid havingma huge debug file */
  if(++trashcounter > MAXPRINTTRASH) 
  {
      /* but print from time to time :) */
	  if(trashcounter%MAXPRINTTRASH != 0) return;
  }
#endif
  va_start(ap, fmt);
  vfprintf(bx_logfd, fmt, ap);
  va_end(ap);

  /*fflush(bx_logfd); ??? */
}

  void
bx_panic(char *fmt, ...)
{
  va_list ap;
  static Boolean been_here = 0;
#ifdef MYSTUFF /* riad */
  trashcounter = 0;
#endif

  fprintf(bx_logfd, "bochs: panic at %04X:%08X, ", bx_cpu.cs.selector.value, bx_cpu.eip);

  va_start(ap, fmt);
#ifdef MYSTUFF
  fprintf(stderr, "bochs: panic at %04X:%08X, ", bx_cpu.cs.selector.value, bx_cpu.eip);
  vfprintf(stderr, fmt, ap);
#endif
  vfprintf(bx_logfd, fmt, ap);
  va_end(ap);

  bx_close_harddrive();
#if BX_EXTERNAL_ENVIRONMENT==0
  if (been_here == 0) {
#if BX_CPU >= 2
    been_here = 1;
    if (bx_protected_mode()) bx_printf("protected mode\n");
    else if (bx_v8086_mode()) bx_printf("v8086 mode\n");
    else bx_printf("real mode\n");
    if (bx_cpu.cs.cache.u.segment.d_b)
      bx_printf("CS.d_b = 32 bit\n");
    else
      bx_printf("CS.d_b = 16 bit\n");
    bx_printf("  current operand size = %s\n",
      bx_cpu.is_32bit_opsize? "32" : "16");
    bx_printf("  current address size = %s\n",
      bx_cpu.is_32bit_addrsize? "32" : "16");
    bx_printf("SS.d_b = %s bit\n",
      bx_cpu.ss.cache.u.segment.d_b? "32" : "16");
#endif
    bx_debug(&bx_cpu.cs, bx_cpu.prev_eip);
    }
#endif /* BX_EXTERNAL_ENVIRONMENT==0 */

  fflush(bx_logfd);
  fclose(bx_logfd);

  exit(1);
}

  int
bx_register_io_handler(bx_iodev_t device)
{
  int handle, i;

  if (device.irq != BX_NO_IRQ  &&  device.irq >= BX_MAX_IRQS) {
    bx_panic("IO device %s registered with IRQ=%d above %u\n",
      device.handler_name, device.irq, (unsigned) BX_MAX_IRQS-1);
    }

  /* first find existing handle for function or create new one */
  for (handle=0; handle < (int) bx_num_io_handles; handle++) {
    if (bx_io_handler[handle].read_funct == device.read_funct) break;
    }
  if (handle >= (int) bx_num_io_handles) {
    /* no existing handle found, create new one */
    if (bx_num_io_handles >= BX_MAX_IO_DEVICES) {
      bx_printf("too many IO devices installed.\n");
      bx_panic("  try increasing BX_MAX_IO_DEVICES\n");
      }
    if (device.irq != BX_NO_IRQ  &&  bx_irq_handler_name[device.irq]) {
      bx_panic("IRQ %d conflict.  IO devices %s & %s\n",
        device.irq, bx_irq_handler_name[device.irq], device.handler_name);
      }
    if (device.irq != BX_NO_IRQ)
      bx_irq_handler_name[device.irq] = device.handler_name;
    bx_num_io_handles++;
    bx_io_handler[handle].read_funct     = device.read_funct;
    bx_io_handler[handle].write_funct    = device.write_funct;
    bx_io_handler[handle].handler_name   = device.handler_name;
    bx_io_handler[handle].irq            = device.irq;
    }

  /* change table to reflect new handler id for that address */
  for (i=device.start_addr; i <= device.end_addr; i++) {
    if (bx_io_handler_id[i] != 0) {
      bx_printf("IO device address conflict at IO address %Xh\n",
        (unsigned) i);
      bx_panic("  conflicting devices: %s & %s\n",
        bx_io_handler[handle].handler_name, bx_io_handler[bx_io_handler_id[i]].handler_name);
      }
    bx_io_handler_id[i] = handle;
    }
  return(1);
}



#if BX_EXTERNAL_ENVIRONMENT==0
/* bx_register_int_vector()
 *
 * register a C function to be called when there is an pseudo-interrupt
 * designated by vector.
 */

  void
bx_register_int_vector(Bit8u vector,
    Bit8u *code, int code_size,
    void (* funct)(int vector)
#ifdef WIN32DEBUG
      ,int debug_code
#endif    
    )
{
  int i;
  static Bit32u next_bios_addr = 0xE0000;

  if (bx_options.rom.path != NULL)
    bx_panic("register_int_vector: called when using alternate ROM image.\n");

  bx_interrupt_table[vector] = funct;

#ifdef WIN32DEBUG2
  bx_printf("VECTOR:::vector=%02X [add:%08lX] [code=%d]\n", vector, next_bios_addr,
	  debug_code);
#endif

  /* if code addr not given, only register function & return. */
  if (!code) return;

  bx_set_interrupt_vector(vector, next_bios_addr);

  for (i=0; i < code_size; i++) {
    bx_access_physical(next_bios_addr + i, 1, BX_WRITE, &code[i]);
    }
  next_bios_addr += code_size;
}
#endif


 
/* bx_iodev_init()
 *
 * intialize local constructs used to all the IO device handling features
 */

  void
bx_iodev_init(void)
{
  unsigned i;

  /* handle 0 maps to the unmapped IO device handler.  Basically any
     ports which don't map to any other device get mapped to this
     handler which does absolutely nothing.
   */
  bx_io_handler[0].read_funct     = bx_unmapped_io_read_handler;
  bx_io_handler[0].write_funct    = bx_unmapped_io_write_handler;
  bx_io_handler[0].handler_name   = "unmapped io";
  bx_io_handler[0].irq            = BX_NO_IRQ;
  bx_num_io_handles = 1;

  /* set unused elements to appropriate values */
  for (i=1; i < BX_MAX_IO_DEVICES; i++) {
    bx_io_handler[i].read_funct          = NULL;
    bx_io_handler[i].write_funct         = NULL;
    }

  for (i=0; i < 0x10000; i++)
    bx_io_handler_id[i] = 0;  /* unmapped IO handle */

  for (i=0; i < BX_MAX_IRQS; i++) {
    bx_irq_handler_name[i] = NULL;
    }

  for (i=0; i < 256; i++) {
    bx_interrupt_table[i] = NULL;
    }
}




#if BX_EXTERNAL_ENVIRONMENT==0
/* manually invoke an interrupt */
  void
bx_invoke_interrupt(int interrupt)
{
  (*bx_interrupt_table[interrupt])(interrupt);
}
#endif



#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_PSEUDO_INT_Ib(void)
{
  Bit8u imm8;

#if BX_CPU >= 2
  if ( bx_protected_mode() && !bx_v8086_mode()) {
    bx_panic("PSEUDO-INT not supported in protected mode!\n");
    }
#endif

  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_interrupt_table[imm8]) {
#ifdef WIN32DEBUG2
    bx_printf("INVOKE_INT:::imm8=%02X [%08lX]\n", imm8, bx_interrupt_table[imm8]);
#endif
    (*bx_interrupt_table[imm8])(imm8);
    }
  else {
    bx_panic("*** no PSEUDO-INT handler defined for INT %02x!\n",
      imm8);
    }
}
#endif



#if BX_EXTERNAL_ENVIRONMENT==0
  void
bx_reset_cpu(void)
{
#if BX_CPU > 2
  bx_panic("reset_cpu: not implemented for 386\n");
#endif

  if (bx_v8086_mode()) bx_panic("main: v8086 mode unsupported\n");

  if (bx_dbg.reset)
    bx_printf("CPU RESET encountered.\n");

  /* status and control flags register set */
  bx_set_CF(0);
  bx_cpu.eflags.bit1 = 1;
  bx_set_PF(0);
  bx_cpu.eflags.bit3 = 0;
  bx_set_AF(0);
  bx_cpu.eflags.bit5 = 0;
  bx_set_ZF(0);
  bx_set_SF(0);
  bx_cpu.eflags.tf = 0;
  bx_cpu.eflags.if_ = 0;
  bx_cpu.eflags.df = 0;
  bx_set_OF(0);
#if BX_CPU >= 2
  bx_cpu.eflags.iopl = 0;
  bx_cpu.eflags.nt = 0;
#endif
  bx_cpu.eflags.bit15 = 0;

  bx_cpu.inhibit_interrupts = 0;
  bx_single_step_event = 0;
#if 0
  /* ??? clear out old pending interrupts */
  bx_cpu.INTR = 0;
#endif

  /* MSW */
#if BX_CPU >= 2
  bx_cpu.cr0.ts = 0; /* no task switch */
  bx_cpu.cr0.em = 0; /* emulate math coprocessor */
  bx_cpu.cr0.mp = 0; /* wait instructions not trapped */
  bx_cpu.cr0.pe = 0; /* real mode */
#endif
#if BX_CPU >= 3
  bx_cpu.cr0.pg = 0; /* paging disabled */
#endif

#if BX_CPU >= 3
  bx_cpu.cr3 = 0;
#endif

  /* ??? 0 for 8086 */
  bx_cpu.eip    = 0x0000FFF0; /* instruction pointer */

  /* CS selector */
  bx_cpu.cs.selector.value =     0xf000;
  bx_cpu.cs.cache.valid =     1;
#if BX_CPU >= 2
  bx_cpu.cs.selector.index =     0x0000;
  bx_cpu.cs.cache.u.segment.base           = 0x000F0000;
  bx_cpu.cs.cache.u.segment.limit          =     0xFFFF;
  bx_cpu.cs.cache.u.segment.limit_scaled   =     0xFFFF;
  bx_cpu.cs.cache.dpl =          0; /* ??? */
#endif

  bx_cpu.ds.selector.value =     0x0000;
  bx_cpu.ds.cache.valid =     1;
#if BX_CPU >= 2
  bx_cpu.ds.selector.index =     0x0000;
  bx_cpu.ds.cache.u.segment.base           = 0x00000000;
  bx_cpu.ds.cache.u.segment.limit          =     0xFFFF;
  bx_cpu.ds.cache.u.segment.limit_scaled   =     0xFFFF;
  bx_cpu.ds.cache.dpl =          0; /* ??? */
#endif

  bx_cpu.ss.selector.value =     0x0000;
  bx_cpu.ss.cache.valid =     1;
#if BX_CPU >= 2
  bx_cpu.ss.selector.index =     0x0000;
  bx_cpu.ss.cache.u.segment.base           = 0x00000000;
  bx_cpu.ss.cache.u.segment.limit          =     0xFFFF;
  bx_cpu.ss.cache.u.segment.limit_scaled   =     0xFFFF;
  bx_cpu.ss.cache.dpl =          0; /* ??? */
#endif

  bx_cpu.es.selector.value =     0x0000;
  bx_cpu.es.cache.valid =     1;
#if BX_CPU >= 2
  bx_cpu.es.selector.index =     0x0000;
  bx_cpu.es.cache.u.segment.base           = 0x00000000;
  bx_cpu.es.cache.u.segment.limit          =     0xFFFF;
  bx_cpu.es.cache.u.segment.limit_scaled   =     0xFFFF;
  bx_cpu.es.cache.dpl =          0; /* ??? */
#endif

#if BX_CPU >= 3
  /* FS */
  bx_cpu.fs.selector.value =     0x0000;
  bx_cpu.fs.selector.index =     0x0000;
  bx_cpu.fs.cache.valid    =     1;
  bx_cpu.fs.cache.u.segment.base           = 0x00000000;
  bx_cpu.fs.cache.u.segment.limit          =     0xFFFF;
  bx_cpu.fs.cache.u.segment.limit_scaled   =     0xFFFF;
  bx_cpu.fs.cache.dpl =          0; /* ??? */

  /* GS */
  bx_cpu.gs.selector.value =     0x0000;
  bx_cpu.gs.selector.index =     0x0000;
  bx_cpu.gs.cache.valid    =     1;
  bx_cpu.gs.cache.u.segment.base           = 0x00000000;
  bx_cpu.gs.cache.u.segment.limit          =     0xFFFF;
  bx_cpu.gs.cache.u.segment.limit_scaled   =     0xFFFF;
  bx_cpu.gs.cache.dpl =          0; /* ??? */
#endif

#if BX_CPU >= 2
  bx_cpu.idtr.base         = 0x00000000;
  bx_cpu.idtr.limit        =     0x03FF;
#endif

  bx_cpu.EXT = 0;
}
#endif

  void
bx_set_INTR(Boolean value)
{
#if BX_EXTERNAL_ENVIRONMENT==0
  bx_cpu.INTR = value;
  bx_async_event = 1;
#else
  external_set_INTR(value);
#endif
}

#if BX_EXTERNAL_ENVIRONMENT==1
  Bit32u
bx_inp(Bit16u addr, unsigned io_len)
{
  Bit8u handle;
  Bit32u ret;

  handle = bx_io_handler_id[addr];
  ret = (* bx_io_handler[handle].read_funct)((Bit32u) addr, io_len);

  if (bx_dbg.io)
    bx_printf("IO: inp(%04x) = %08x\n", (unsigned) addr, (unsigned) ret);
  return(ret);
}

  void
bx_outp(Bit16u addr, Bit32u value, unsigned io_len)
{
  Bit8u handle;

  if (bx_dbg.io)
    bx_printf("IO: outp(%04x) = %08x\n", (unsigned) addr, (unsigned) value);

  handle = bx_io_handler_id[addr];
  (* bx_io_handler[handle].write_funct)((Bit32u) addr, value, io_len);
}
#endif




#if BX_EXTERNAL_ENVIRONMENT==1
  void
bx_set_enable_a20(Bit8u value)
{
  external_set_enable_a20( value );
}
#endif

#if BX_EXTERNAL_ENVIRONMENT==1
  Boolean
bx_get_enable_a20(void)
{
  return( external_get_enable_a20() );
}
#endif

#if BX_EXTERNAL_ENVIRONMENT==1
  void
bx_reset_cpu(void)
{
  external_reset_cpu();
}
#endif


#if (BX_EXTERNAL_ENVIRONMENT==0) && (BX_EMULATE_HGA_DUMPS>0)
  void
bx_emulate_hga_dumps_timer(void)
{
  void bx_hga_set_video_memory(Bit8u *ptr);

  bx_hga_set_video_memory(&bx_phy_memory[0xb0000]);
}
#endif

  static void
parse_bochsrc(void)
{
  FILE *fd;
  int ret;
  char *ptr;
  int i, string_i;
  char line[512];
  char string[512];
  char *params[40];
  int num_params;

#ifdef WIN32
  ptr = getenv("windir");
#else
  ptr = getenv("HOME");
#endif
  if (!ptr) {
#ifdef WIN32
    fprintf(stderr, "could not get environment variable 'windir'.\n");
#else
    fprintf(stderr, "could not get environment variable 'HOME'.\n");
#endif
    exit(1);
    }

  strcpy(bochsrc_path, BOCHS_INI);
  fd = fopen(bochsrc_path, "r");

  if (!fd) {
    fprintf(stderr, "could not open file '%s', trying home directory.\n",
      bochsrc_path);

    strcpy(bochsrc_path, ptr);
    strcat(bochsrc_path, "/");
    strcat(bochsrc_path, BOCHS_INI);

    fd = fopen(bochsrc_path, "r");
    if (!fd) {
#if 0 /* bill */
      fprintf(stderr, "could not open file '%s'.\n", bochsrc_path);
#else
      fprintf(stderr, "could not open file '%s', trying $BOCHSRC.\n", bochsrc_path);
      ptr = getenv("BOCHSRC");
      if (ptr)   
        {
          strcpy (bochsrc_path, ptr);
          fd = fopen(bochsrc_path, "r");
        }
      if (!ptr || !fd)
        {   
          fprintf(stderr, "could not open file '%s', exiting.\n", bochsrc_path);
#endif
          exit(1);
        }
#if 1 /* bill */
     else fprintf(stderr, "using rc file '%s'.\n", bochsrc_path);
    }
#endif
    else
      fprintf(stderr, "using rc file '%s'.\n", bochsrc_path);
    }
  else
    fprintf(stderr, "using rc file '%s'.\n", bochsrc_path);
  
  do {
    num_params = 0;
    ret = fscanf(fd, "%[^\n]s\n", line);
    line[sizeof(line)-1] = '\0';
    (void) getc(fd); /* trash newline */
    if (ret != 1) continue;
    ptr = strtok(line, ":");
    while (ptr) {
      string_i = 0;
      for (i=0; i<strlen(ptr); i++) {
        if (!isspace(ptr[i])) {
          string[string_i++] = ptr[i];
          }
        }
      string[string_i] = '\0';
      strcpy(ptr, string);
      params[num_params++] = ptr;
      ptr = strtok(NULL, ",");
      }
    parse_line(num_params, &params[0]);
    } while (!feof(fd));
}

  static void
parse_line(int num_params, char *params[])
{
  int i;

  if (num_params < 1) return;

  if (params[0][0] == '#') return; /* comment */
  else if (!strcmp(params[0], "floppya")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "file=", 5)) {
        strcpy(bx_options.floppya.path, &params[i][5]);
        bx_options.floppya.type = BX_FLOPPY_AUTO;
        }
      else if (!strncmp(params[i], "1_44=", 5)) {
        strcpy(bx_options.floppya.path, &params[i][5]);
        bx_options.floppya.type = BX_FLOPPY_1_44;
        }
      else if (!strncmp(params[i], "1_2=", 4)) {
        strcpy(bx_options.floppya.path, &params[i][4]);
        bx_options.floppya.type = BX_FLOPPY_1_2;
        }
      else if (!strncmp(params[i], "0_720=", 6)) {
        strcpy(bx_options.floppya.path, &params[i][6]);
        bx_options.floppya.type = BX_FLOPPY_0_720;
        }
      else {
        fprintf(stderr, ".bochsrc: floppya attribute '%s' not understood.\n",
          params[i]);
        exit(1);
        }
      }
    }

  else if (!strcmp(params[0], "floppyb")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "file=", 5)) {
        strcpy(bx_options.floppyb.path, &params[i][5]);
        bx_options.floppyb.type = BX_FLOPPY_AUTO;
        }
      else if (!strncmp(params[i], "1_44=", 5)) {
        strcpy(bx_options.floppyb.path, &params[i][5]);
        bx_options.floppyb.type = BX_FLOPPY_1_44;
        }
      else if (!strncmp(params[i], "1_2=", 4)) {
        strcpy(bx_options.floppyb.path, &params[i][4]);
        bx_options.floppyb.type = BX_FLOPPY_1_2;
        }
      else if (!strncmp(params[i], "0_720=", 6)) {
        strcpy(bx_options.floppyb.path, &params[i][6]);
        bx_options.floppyb.type = BX_FLOPPY_0_720;
        }
      else {
        fprintf(stderr, ".bochsrc: floppyb attribute '%s' not understood.\n",
          params[i]);
        exit(1);
        }
      }
    }

  else if (!strcmp(params[0], "diskc")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "file=", 5)) {
        strcpy(bx_options.diskc.path, &params[i][5]);
        }
      }
    }
  else if (!strcmp(params[0], "boot")) {
    if (!strcmp(params[1], "a") ||
        !strcmp(params[1], "c")) {
      strcpy(bx_options.bootdrive, params[1]);
      }
    else {
      fprintf(stderr, ".bochsrc: boot directive with unknown boot device '%s'.\n",
        params[1]);
      fprintf(stderr, "          use 'a', or 'c'.\n");
      exit(1);
      }
    }
  else if (!strcmp(params[0], "log")) {
    if (num_params != 2) {
      fprintf(stderr, ".bochsrc: log directive has wrong # args.\n");
      exit(1);
      }
    strcpy(logfilename, params[1]);
    }
  else if (!strcmp(params[0], "romimage")) {
    if (num_params != 2) {
      fprintf(stderr, ".bochsrc: romimage directive: wrong # args.\n");
      exit(1);
      }
    bx_options.rom.path = strdup(params[1]);
    }
  else if (!strcmp(params[0], "hga_update_interval")) {
    if (num_params != 2) {
      fprintf(stderr, ".bochsrc: hga_update_interval directive: wrong # args.\n");
      exit(1);
      }
    bx_options.hga_update_interval = atol(params[1]);
    if (bx_options.hga_update_interval < 50000) {
      fprintf(stderr, ".bochsrc: hga_update_interval not big enough!\n");
      exit(1);
      }
    }
  else {
    fprintf(stderr, ".bochsrc: directive '%s' not understood\n", params[0]);
    }
}
