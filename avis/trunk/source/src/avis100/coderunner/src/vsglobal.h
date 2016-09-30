/* Note: many of these global variables are holdovers from the
 * old VIRSCAN program.
 */

bOOlean
signature_has_been_found = FALSE;   /* SIG_FOUND takes precedence over */
                                    /* other errors */

/* This is what is triggered by the -GURU option of the old virscan program
 * and of IBMAVSP. When used, expect false positives if certain
 * other AV products (e.g. CPAV) are used.
 */
bOOlean
always_warn = FALSE;                /* If this bOOlean is set, always warn */
                                    /*   if a signature is found, even if */
                                    /*   the indicated virus is not normally */
                                    /*   found in executables of the */
                                    /*   executable's type. */
                                    /*   (Type is derived from EXE header */

/* This is not used by the IBMAV product.
 * Should in fact probably be removed but
 * it doesn't take much code space.
 */
bOOlean
u_warn_if_compressed = FALSE;       /* If this bOOlean is set, warn about */
                                    /*   files that contain signatures */
                                    /*   marked with the "compressed" */
                                    /*   keyword. */

/* This variable is not used by the IBMAV product. I've #ifdef-ed out much
 * of the code involved with this support but it should properly be removed
 * completely; the signature file is now too large to support it.
 */
bOOlean
mutation_support = FALSE;           /* If this bOOlean is set via */
                                    /*   undocumented option -mf or */
                                    /*   option -m, */
                                    /*   crude mutant detection support */
                                    /*   is enabled. The support involves */
                                    /*   breaking signatures into 11 byte */
                                    /*   fragments. */
/* This is still useful, and could be used by IBMAV?
 */
bOOlean
tolerance_allowed = TRUE;           /* If this bOOlean is set via */
                                    /*   undocumented option -mt */
                                    /*   or option -m, */
                                    /*   some mismatched bytes are allowed */

/* This is I believe used by IBMAVSP in an undocumented option, and is
 * certainly useful for testing.
 */
bOOlean
more_tolerance_allowed = FALSE;     /* If this bOOlean is set via */
                                    /*   undocumented option -mm */
                                    /*   even more mismatched bytes are */
                                    /*   allowed than if -mt is specified */

/* I'm not sure how this is used by the current code. Best not to touch it.
 */
bOOlean
default_mutant_detection = TRUE;    /* Unless this bOOlean is set to FALSE with */
                                    /*   any of the mutant detection switches */
                                    /*   (-m, -mm, -mt, -mf), only the default */
                                    /*   mutant detection will be used. */

/* These variables were added to support IBMAVSD (which only loads boot virus
 * search patterns and sits in memory until a OS/2 shutdown happens). Other
 * versions could use them too.
 */
bOOlean                             /* Set these to false if we don't want */
inst_boot_signatures = TRUE;        /* signatures of this type loaded into */
bOOlean                             /* in-memory hash tables. Save memory! */
inst_file_signatures = TRUE;
bOOlean
inst_mem_signatures = TRUE;         /* Signatures marked "scan memory" */
bOOlean
inst_all_signatures = TRUE;         /* This must be set to false or the */
                                    /* of the previous bOOleans don't matter */

/* This is an icky way to control the boot sector ignoring function, but was
 * forced by the requirement that the interface to the virscan back end not
 * change in a backwards-incompatible way.
 */
bOOlean bs_ignore_mode = FALSE;     /* If this bOOlean is set, the boot */
                                    /*   sector scanning functions will */
                                    /*   operate in "add-to-ignore-list" mode */
                                    /*   instead of scanning the boot record. */
bOOlean ignore_bsignore = FALSE;    /* If this bOOlean is set, the boot */
                                    /*   sector scanning functions will */
                                    /*   ignore the contents of any present   */
                                    /*   bsignore.lst file.                   */

/* All these algorithmic detectors can be turned off through these global
 * variables. No IBMAV programs support this that I know of, though I might
 * add a -NALG to IBMAVSP at some point.
 */
bOOlean
scan_DA_MtE = TRUE;                 /* Unless this bOOlean is set to FALSE with */
                                    /*   -NMTE, virscan will look for the Dark */
                                    /*   Avenger MuTation Engine in files */
                                    /*   Also set false by "-NALG" */

bOOlean
scan_Washburn = TRUE;               /* Can be turned off with the -NALG option */

bOOlean
scan_V2P6 = TRUE;                   /* Can be turned off with the -NALG option */

bOOlean
scan_Tremor = TRUE;                 /* Can be turned off with the -NALG option */

bOOlean
scan_psmpc = TRUE;                  /* Can be turned off with the -NALG option */

bOOlean
scan_epoint = TRUE;                 /* Can be turned off with the -NEPT option */

bOOlean
scan_sbg = TRUE;                    /* Can be turned off with the -NALG option */

bOOlean
scan_tpe = TRUE;                    /* Can be turned off with the -NALG option */

bOOlean
scan_smg = TRUE;                    /* Can be turned off with the -NALG option */

bOOlean
scan_natas = TRUE;                  /* Can be turned off with the -NALG option */

bOOlean
scan_algboot = TRUE;                /* Can be turned off with the -NALG or -NAB option */

bOOlean
code_run = TRUE;                    /* Can be turned off with the -NALG option */

bOOlean
vs_call_scanner = TRUE;

bOOlean
vs_memscan_dbg = FALSE;

#if !ESDC
bOOlean
vs_scalpel_dbg = FALSE;

bOOlean use_old_alg_boot = FALSE;
#endif

/* These are the documented way to control the scanning of compressed stuff.
 * Globals were used to assure code compatibility with old versions of
 * front ends.
 */
bOOlean
scan_self_decomp_programs = FALSE;  /* Can be turned on with the ???? */
bOOlean
scan_compressed_archives = FALSE;   /* Can be turned off with the ???? */
bOOlean
vs_checking_only_programs = FALSE;  /* Used to control what files are scanned */
                                    /* We use the normal traverser filespec only if */
                                    /* programs are being checked ? */

/* This variable, if set to true, forces the scanner to keep only part of
 * each search pattern data structure in memory, if the search pattern
 * came from a binary signature file. It must be set before initializing the
 * scanner. If a partial match is found, the rest of the search pattern is
 * read from the binary signature file.
 */
bOOlean
compact_sigs_in_memory = FALSE;     /* Can be turned on somehow */

int sigfound_cnt=0;                 /* The number of valid viral signatures */
                                    /*   found. At program termination, */
                                    /*   the total count of signatures found */

int csigfound_cnt=0;                /* The number of valid compressor */
                                    /*   signatures found. At program */
                                    /*   termination, the total count of */
                                    /* compressor signatures found. */

bOOlean loaded_addenda_lst = FALSE; /* Set to TRUE if addenda.lst was loaded */

void (*compressed_file_callback)(char *) = NULL;
                                    /* If set to something other than NULL */
                                    /*   the scanning code will call this */
                                    /*   function every time it starts to scan */
                                    /*   a file that it has just decompressed, */
                                    /*   giving it a fake name composed of the */
                                    /*   root file name and an "innards" file */
                                    /*   name. */

#if !WATC
bOOlean external_scan_termination = FALSE;
#endif

/*
 * The following global variables are used to remember stuff when scanning
 * a particular object.
 */
int object_sigfound_cnt=0;          /* The number of valid viral signatures */
                                    /*   found in the object that was just scanned. */
                                    /*   When scanning an object, it is the running */
                                    /*   total of signatures found. */
int object_vfound_cnt=0;            /* The number of times that a virus-found */
                                    /*   message was displayed or virus-found */
                                    /*   bucket added, for the object */

int found_precise_match=FALSE;      /* Set to TRUE if a signature was matched */
                                    /*   precisely during the scan of an object */

bOOlean tmp_swmd=FALSE;             /* Set to true if active sig_with_min_diffs */
                                    /*   is a temporary signature */
sigdat *sig_with_min_diffs=NULL;    /* Set to point to the signature data structure */
                                    /*   with the fewest mismatches. */

int min_mismatched_bytes=INT_MAX;   /* The number of mismatched bytes for the */
                                    /*   signature with the fewest mismatches. */

long offset_swmd=0l;                /* Offset into object where the signature data */
                                    /*   structure with the fewest mismatches */
                                    /*   was found. */

char *comp_tmp_path = NULL;         /* Used by the decompression code. If */
                                    /*   non-nul, the scanner will try to */
                                    /*   temporary files to this path */

char *obj_name = NULL;              /* These are set if a signature is partially */
char *obj_vp_name = NULL;           /*   matched */
int obj_type = T_FILE;
bOOlean obj_message_output = FALSE; /* Has a message been output for this object yet? */

char *current_sigfilename=NULL;     /* Sent at the beginning of */
                                    /*   get_signatures() and */
                                    /*   binary_get_full_sig() */

/*
 * Global variables used by the scanning routines.
 */
char tmplinebuf[SIZE_TMP_LINE_BUF]; /* fgets() reads to this buffer */

/* Lookup tables must be in the same segment in PC versions for performance
 * reasons, so this is forced by putting both tables in a single object.
 */
/*byte first_sig_chars[SIZE_FSC];*/ /* This table is a enables a quick test */
                                    /*   of whether or not a byte is the */
                                    /*   first byte of a signature. It */
                                    /*   is a performance hack that is only */
                                    /*   particularly significant if the */
                                    /*   pointers in 'hash_table' are 4 byte */
                                    /*   pointers and the machine can compare */
                                    /*   only 2 bytes at a time */
/*sigdat *hash_table[SIZE_HT];*/    /* 'Hash value' is the first byte of a */
                                    /*   signature. Test for null may be */
                                    /*   more expensive than test of a */
                                    /*   byte, hence other table */
lookup_tables lookup_table;

sigdat *epoint_list_head = NULL;    /* This is the head of linked list into */
                                    /*   which we insert search patterns that */
                                    /*   we check for only at the entry point */
                                    /*   of files. */


unsigned int size_work_buf = SIZE_WORK_BUF;
                                    /* Can be adjusted early on in program */
                                    /* execution, i.e. before workbuf is */
                                    /* allocated. */

/* Normally, assume length is SIZE_WORK_BUF+MAX_SIZE_SIGN-1 */
byte *workbuf = NULL;               /*[SIZE_WORK_BUF+2*MAX_SIZE_SIGN]*/
                                    /* Buffer for file reads. */
                                    /*   Note that the size is larger */
                                    /*   than a multiple of a power of two */
                                    /*   This is */
                                    /*   so that a scan past the end of the */
                                    /*   buffer works. There's also room to */
                                    /*   fill with zeros past the end of */
                                    /*   the buffer */
int sig_count = 0;                  /* Total count of signatures stored in */
                                    /*    hash table */
int whole_sig_count = 0;            /* Total count of whole signatures */
int frag_sig_count = 0;             /* Total count of signatures fragments */

/*
 * Count of things scanned. (Does ESDC need these?)
 */

unsigned long total_scanlength = 0l;/* Keep statistics */

/* I believe this variable is used by IBMAVD.
 */
#if MEM_SCAN
bOOlean
do_high_memory_scan = TRUE;         /* Memory scanning above A000 can be */
                                    /* turned off with command line */
                                    /* option "-nhms" */
#endif


unsigned int OS_major_ver = 0;
unsigned int OS_minor_ver = 0;

bOOlean vs_scalpel = FALSE;         /* Can be set/unset with environment */
                                    /*   variable VS_SCALPEL=ON/VS_SCALPEL=OFF */

#if ESDC
char *vs_version="Virus scanning library version 1.52";
#endif

/* Various global variables not used by the IBMAV version of the code.
 */
#if !ESDC
bOOlean bc_verbose_mode;            /* Make bootchk code noisy/quiet */
unsigned long filecnt = 0l;
int master_brecord_count = 0;       /* Count of master boot records scanned */
int system_bsector_count = 0;       /* Count of system boot sectors scanned */
int bootman_bsector_count = 0;      /* Count of bootmanager boot sectors scanned */
bOOlean
have_scanned_system_memory = FALSE; /* Set to true when system memory is */
                                    /*   scanned. */
int infected_object_cnt=0;          /* Rough count of the number of infected */
                                    /*   objects (objects in which signatures */
                                    /*   were found) count; An object is counted */
                                    /*   as long as it's name isn't the same as */
                                    /*   the name of a previous object; this */
                                    /*   means that if an infected drive is */
                                    /*   scanned twice, infected objects may */
                                    /*   be counted twice. */
int compressed_object_cnt=0;        /* Like infected_object_count, except */
                                    /*   is count of compressed things */

bOOlean csim_debug = FALSE;         /* Set to true with -csimd */

#if SELF_TEST
bOOlean
do_self_test = TRUE;                /* Self-test can be turned off with */
                                    /* command line option "-nst" */
#else
bOOlean
do_self_test = FALSE;               /* Self-test can be turned off with */
                                    /* command line option "-nst" */
#endif
bOOlean
cont_open_error = FALSE;            /* If this bOOlean is set TRUE */
                                    /*   via a command line switch, */
                                    /*   virscan will continue the scan */
                                    /*   even if it can't open a particular */
                                    /*   for scanning */
bOOlean
verbose = FALSE;                    /* If this bOOlean is set TRUE */
                                    /*   via a command line switch, */
                                    /*   file names will be listed as */
                                    /*   they are processed, and 'quiet' */
                                    /*   is forced FALSE */

bOOlean
disp_bootsector = FALSE;            /* If this bOOlean is set via */
                                    /*   undocumented option -vv, */
                                    /*   a hex dump of a boot sector is */
                                    /*   displayed after it is read. */
                                    /*   Both master boot records and */
                                    /*   system boot sectors are displayed */
bOOlean  user_spec_removable[26] =
{ FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE };
bOOlean                             /* Set with command line option /X */
test_mode = FALSE;                  /*   Virscan will terminate as if it */
                                    /*   found virus signatures. It */
                                    /*   will say that 0 virus signatures */
                                    /*   were found, but otherwise will act */
                                    /*   as if a virus were found */
time_t scan_starttime;              /* Time at which actual scan started */
int prevnamelen = 0;                /* For pretty printing of filenames */

bOOlean
do_pause_if_found = TRUE;           /* If unset with command line option -npif */
                                    /* virscan will not pause with a question */
                                    /* if a "bad" virus is found in memory */


bOOlean
do_memory_scan = TRUE;              /* Memory scanning can be turned off */
                                    /* with command line option "-nms" */

bOOlean
beep_if_sig_found = TRUE;           /* Normally, virscan will beep if a */
                                    /*   virus signature is found. This beep */
                                    /*   can be turned off with undocumented */
                                    /*   command line option -nb */

bOOlean
cont_access_denied = FALSE;        /* If set with command line option -cad, */
                                    /*   virscan will continue the scan even */
                                    /*   if it isn't allowed to scan down */
                                    /*   some subdirectory subtree. */


bOOlean
cr_works = CR_WORKS_DEF;            /* If this is set to FALSE with the */
                                    /*   command line option "-np", then */
                                    /*   virscan reverts to the old summary */
                                    /*   behavior, without the display */
                                    /*   of object's names as they are */
                                    /*   scanned. */
                                    /* If carriage return doesn't works as */
                                    /*   it is supposed to, for example */
                                    /*   in the VM/CMS version, this */
                                    /*   switch should be initialized to */
                                    /*   FALSE when compiling that version. */

bOOlean
quiet = FALSE;                      /* If this bOOlean is set TRUE */
                                    /*   via a command line switch, */
                                    /*   only warning messages (if any) or */
                                    /*   error messages (if any), */
                                    /*   the byte and file count */
                                    /*   messages will be displayed, and */
                                    /*   'verbose' is forced FALSE. */

bOOlean
very_quiet = FALSE;                 /* Don't display *any* warnings about */
                                    /*   signatures that are found */

bOOlean product_version = FALSE;    /* These version flags are automatically */
bOOlean os2_prod_version = FALSE;   /*   the message file. Do Not Set Manually! */
                                    /*   They are set by the code that reads */
                                    /*   the signature file. */

char
file_specification[128] = { 0};     /* The default file specification is set */
                                    /*   if no non-default specification is */
                                    /*   found on the first pass through the */
                                    /*   command line. */
                                    /*   Can be set to something else with */
                                    /*   command line option -w */
bOOlean
explicit_searches_only = FALSE;     /* If this bOOlean is set via */
                                    /*   undocumented option -e, searches */
                                    /*   are only done of things specified */
                                    /*   on the command line. */
bOOlean
have_tested_master_boot_recs=FALSE; /* Make sure that master boot records */
                                    /*   are only tested once. */
bOOlean
do_mbr_scan = TRUE;                 /* Automatic scanning of the master boot */
                                    /* record when in DOS mode and a drive */
                                    /* letter is >= C: is specified can be */
                                    /* turned off with the -NMBRS switch */
bOOlean
list_positives = FALSE;             /* If this bOOlean is set TRUE */
                                    /*   via a command line switch, */
                                    /*   program will generate a list */
                                    /*   of names of files in which a viral */
                                    /*   was found */
bOOlean
list_vpositives = FALSE;            /* If this bOOlean is set TRUE */
                                    /*   via a command line switch, */
                                    /*   program will generate a list */
                                    /*   of names of files in which a viral */
                                    /*   was found, along with VERV codes */
char *positives_filename = NULL;    /* Routine 'warning_msg()' writes names */
                                    /*   of files in which were found viral */
                                    /*   signatures to this file, if the */
                                    /*   list_positives bOOlean is TRUE */
char *vpositives_filename = NULL;   /* Routine 'warning_msg()' writes names */
                                    /*   of files in which were found viral */
                                    /*   signatures to this file, along with */
                                    /*   VERV codes. */
bOOlean
create_logfile = FALSE;             /* If this bOOlean is set TRUE */
                                    /*   via a command line switch, */
                                    /*   program will generate a log file, */
                                    /*   with at least one line per object */
                                    /*   scanned. (Multiple lines if there */
                                    /*   were multiple virus hits in the */
                                    /*   object */
char *logfilename = NULL;           /* If create_logfile is TRUE, then a */
                                    /*   logfile by this name is created */
                                    /*   and maintained during the scan */

bOOlean
erase_infected_files = FALSE;       /* Set to true with /EINF command line */
                                    /*   switch. */
#if FREQ_COUNTS
long ccounts[SIZE_FSC];             /* Development use only */
long ht_hit_count = 0l;
#endif
#endif
