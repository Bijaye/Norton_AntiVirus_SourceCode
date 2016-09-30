/*---------------------------------------------------------------------------*/
/* This module contains the top-level routines for doing algorithmic         */
/* detection of known viruses in files. It also calls the entry point        */
/* scanner which looks for standard entry-point search patterns at the entry */
/* point of files.                                                           */
/* Basically, the main routine is passed all that we know so far about the   */
/* file including a buffer that might contain the interesting parts of the   */
/* file if we're lucky (and which at the very least contains the first 64    */
/* bytes of the file). The routine gets the region around the entry point    */
/* of the file, file I/O if necessary, then passes this entry point buffer   */
/* to various routines which check it for known viruses which can't be       */
/* detected with bulk-scan search patterns.                                  */
/* (One of these routines checks for entry-point-only search patterns.       */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 used to replace all occurrences with malloc(), strdup(), argv(),
       hmalloc(), cmalloc(), free()
*/
#ifdef S_H
#include "mymalloc.h"
#endif
#if defined(T_OS2)&&defined(WS_16)&&defined(VIRSCAN)
#include "dbmalloc.h"
#endif
#ifndef __UNIX__
#include <io.h>
#include <share.h>
#endif
#include <fcntl.h>
/*
 * Useful types.
 */
#include "common.h"
typedef unsigned int bOOlean;
typedef unsigned short int word;
typedef unsigned char  CR_BYTE;
typedef unsigned short CR_WORD;
typedef unsigned long  CR_ULONG;

#define SIZE_TMP_LINE_BUF   512          /* Length of working buffer used */
                /*   reading signature file and */
                /*   filelist file */
#define MAX_SIZE_SIGN       (SIZE_TMP_LINE_BUF/2)

#include "vsdefs.h"
#include "vspdtype.h"
#include "vspddefs.h"
#ifdef CR_DO_SCANNER   //riad//
#include "mtedetu.pro"
#include "v2p6detu.pro"
#include "tremdetu.pro"
#include "mpcdetu.pro"
#include "eptscan.pro"
#include "sbgdetu.pro"
#include "1260detu.pro"
#include "tpedetu.pro"
#include "smgdetu.pro"
#include "natdetu.pro"
#include "sigcmp.pro"
#endif //riad//

#include "algscan.pro"
#include "cr_fscan.pro"
#include "vstypes.h"

/* Private routines */

static void load_from_file(char *filename, int *fh,     /* File involved */
            char *buf,                   /* Put data here */
            long startfrom,     /* This offset from file start */
            size_t MAXOFF,      /* Read at most this many bytes */
            char *oldbuf, unsigned long oblen, int ob_wholefile,
            size_t *buflen,     /* How many bytes really read? */
            int *retc);         /* Non-zero if real error */

/* Public stuff */

//int algdet_verbose = 0;

#ifdef CR_DO_SCANNER       //riad//
extern int scan_V2P6;
extern int scan_DA_MtE;
extern int scan_Tremor;
extern int scan_psmpc;
extern int scan_epoint;
extern int scan_sbg;
extern int scan_Washburn;
extern int scan_tpe;
extern int scan_smg;
extern int scan_natas;
extern int code_run;

extern unsigned int vs_scalpel;
extern unsigned int always_warn;
extern unsigned int extra_guru;
                /* A bOOlean, set when a precise match to */
                /*   search pattern is found in the current */
                /*   object. Reset when a new object is */
                /*   checked */
extern int found_precise_match;     

extern int object_sigfound_cnt;     /* The number of valid viral signatures */
                /*   found in the object that was just scanned. */
                /*   When scanning an object, it is the running */
                /*   total of signatures found. */
#else    //riad//
//unsigned int always_warn = FALSE;
unsigned int vs_scalpel = FALSE;
unsigned int extra_guru = FALSE;
#endif  //riad//

//riad//
extern   int   x86_prefetch_list[];
extern   int   x86_prefetch_index;
extern   int   _CodeModifPrefetch;

/* Maximum size of startup code to examine */
#define MAXOFF 2048
#define BEFORE 2048
#define OLD_MAXOFF 600
#define HLEN 30

/* Simulated load point for CodeRunner */
#define START_SEG 0x3000

#include "epttypes.h"

#define MIN(A,B)            ((A) < (B) ? (A) : (B))
#define MAX(A,B)            ((A) > (B) ? (A) : (B))

/* These patterns, at certain offsets, are considered strong-enough
 * indications that the entry point code is for a filetype that we'd
 * rather not run through coderunner.
 */
unsigned char FExclude_1[] =          /* PKLITE in COM, a couple of versions */
{
   0xFF^0xFE, 0xFF^0x01, 0xFF^0xB8, 0xFF^0xFF,
   0xFF^0xFF, 0xFF^0xBA, 0xFF^0xFF, 0xFF^0xFF,
   0xFF^0x3B, 0xFF^0xC4, 0xFF^0x73, 0xFF^0xFF,
   0xFF^0x8B, 0xFF^0xC4, 0xFF^0x2D, 0xFF^0xFF,
   0xFF^0x03, 0xFF^0xFE, 0xFF^0x01, 0xFF^0x25,
   0xFF^0xF0, 0xFF^0xFF, 0xFF^0x8B, 0xFF^0xF8,
   0xFF^0xB9, 0xFF^0xFF, 0xFF^0x00
};
unsigned char FExclude_2[] =          /* Some old EXEPACK */
{
   0xFF^0x8C, 0xFF^0xC0, 0xFF^0x05, 0xFF^0x10,
   0xFF^0x00, 0xFF^0x0E, 0xFF^0x1F, 0xFF^0xA3,
   0xFF^0x04, 0xFF^0x00, 0xFF^0x03, 0xFF^0x06,
   0xFF^0x0C, 0xFF^0x00, 0xFF^0x8E, 0xFF^0xC0,
   0xFF^0x8B, 0xFF^0x0E, 0xFF^0x06, 0xFF^0x00,
   0xFF^0x8B, 0xFF^0xF9, 0xFF^0x4F, 0xFF^0x8B
};
unsigned char FExclude_3[] =          /* PKLITE.91 EXE? */
{
   0xFF^0xB8, 0xFF^0xFF, 0xFF^0xFF, 0xFF^0xBA,
   0xFF^0xFF, 0xFF^0xFF, 0xFF^0x05, 0xFF^0x00,
   0xFF^0x00, 0xFF^0x3B, 0xFF^0x06, 0xFF^0x02,
   0xFF^0x00, 0xFF^0x73, 0xFF^0x1A, 0xFF^0x2D,
   0xFF^0x20, 0xFF^0x00, 0xFF^0xFA, 0xFF^0x8E,
   0xFF^0xD0, 0xFF^0xFB, 0xFF^0x2D, 0xFF^0x25
};
unsigned char FExclude_4[] =          /* PKLITE.92 EXE? */
{
   0xFF^0xB8, 0xFF^0xFF, 0xFF^0xFF, 0xFF^0xBA,
   0xFF^0xFF, 0xFF^0xFF, 0xFF^0x05, 0xFF^0x00,
   0xFF^0x00, 0xFF^0x3B, 0xFF^0x06, 0xFF^0x02,
   0xFF^0x00, 0xFF^0x72, 0xFF^0x1B, 0xFF^0xB4,
   0xFF^0x09, 0xFF^0xBA, 0xFF^0x18, 0xFF^0x01,
   0xFF^0xCD, 0xFF^0x21, 0xFF^0xCD, 0xFF^0x20
};
unsigned char FExclude_5[] =          /* LZ91 */
{
   0xFF^0x06, 0xFF^0x0E, 0xFF^0x1F, 0xFF^0x8B,
   0xFF^0x0E, 0xFF^0x0C, 0xFF^0x00, 0xFF^0x8B,
   0xFF^0xF1, 0xFF^0x4E, 0xFF^0x89, 0xFF^0xF7,
   0xFF^0x8C, 0xFF^0xDB, 0xFF^0x03, 0xFF^0x1E,
   0xFF^0x0A, 0xFF^0x00, 0xFF^0x8E, 0xFF^0xC3,
   0xFF^0xFD, 0xFF^0xF3, 0xFF^0xA4, 0xFF^0x53
};
unsigned char FExclude_6[] =          /* LZ90 */
{
   0xFF^0x06, 0xFF^0x0E, 0xFF^0x1F, 0xFF^0x8B,
   0xFF^0x0E, 0xFF^0x0C, 0xFF^0x00, 0xFF^0x8B,
   0xFF^0xF1, 0xFF^0x4E, 0xFF^0x89, 0xFF^0xF7,
   0xFF^0x8C, 0xFF^0xDB, 0xFF^0x03, 0xFF^0x1E,
   0xFF^0x0A, 0xFF^0x00, 0xFF^0x8E, 0xFF^0xC3,
   0xFF^0xB4, 0xFF^0x00, 0xFF^0x31, 0xFF^0xED
};
unsigned char FExclude_7[] =          /* Another old EXEPACK */
{
   0xFF^0x8C, 0xFF^0xC0, 0xFF^0x05, 0xFF^0x10,
   0xFF^0x00, 0xFF^0x0E, 0xFF^0x1F, 0xFF^0xA3,
   0xFF^0x04, 0xFF^0x00, 0xFF^0x03, 0xFF^0x06,
   0xFF^0x0E, 0xFF^0x00, 0xFF^0x8E, 0xFF^0xC0,
   0xFF^0x8B, 0xFF^0x0E, 0xFF^0x06, 0xFF^0x00,
   0xFF^0x8B, 0xFF^0xF9, 0xFF^0x4F, 0xFF^0x8B
};
unsigned char FExclude_8[] =          /* Some strange but OK entry point code */
{
   0xFF^0x9C, 0xFF^0x55, 0xFF^0x56, 0xFF^0x8C,
   0xFF^0xCD, 0xFF^0x83, 0xFF^0xC5, 0xFF^0x10,
   0xFF^0x8D, 0xFF^0xB6, 0xFF^0xFF, 0xFF^0xFF,
   0xFF^0x56, 0xFF^0xBE, 0xFF^0xFF, 0xFF^0x00,
   0xFF^0x56, 0xFF^0xCB, 0xFF^0x4D, 0xFF^0x45,
   0xFF^0x4D, 0xFF^0x4F, 0xFF^0x52, 0xFF^0x59
};
unsigned char FExclude_9[] =          /* PKLITE.93 EXE? */
{
   0xFF^0xB8, 0xFF^0xFF, 0xFF^0xFF, 0xFF^0xBA,
   0xFF^0xFF, 0xFF^0xFF, 0xFF^0x05, 0xFF^0x00,
   0xFF^0x00, 0xFF^0x3B, 0xFF^0x06, 0xFF^0x02,
   0xFF^0x00, 0xFF^0x73, 0xFF^0x1A, 0xFF^0x2D,
   0xFF^0x20, 0xFF^0x00, 0xFF^0xFA, 0xFF^0x8E,
   0xFF^0xD0, 0xFF^0xFB, 0xFF^0x2D, 0xFF^0x19
};
unsigned char FExclude_10[] =          /* PKLITE.93 EXE another flavor */
{
   0xFF^0x50, 0xFF^0xB8, 0xFF^0x20, 0xFF^0x05,
   0xFF^0xBA, 0xFF^0xD3, 0xFF^0x02, 0xFF^0x05,
   0xFF^0x00, 0xFF^0x00, 0xFF^0x3B, 0xFF^0x06,
   0xFF^0x02, 0xFF^0x00, 0xFF^0x72, 0xFF^0x2A,
   0xFF^0xB4, 0xFF^0x09, 0xFF^0xBA, 0xFF^0x1C,
   0xFF^0x01, 0xFF^0xCD, 0xFF^0x21, 0xFF^0xB8
};
unsigned char FExclude_11[] =          /* PKLITE V1.03 or V1.05 */
{
   0xFF^0xB8, 0xFF^0xFF, 0xFF^0xFF, 0xFF^0xBA,
   0xFF^0xFF, 0xFF^0xFF, 0xFF^0x8C, 0xFF^0xDB,
   0xFF^0x03, 0xFF^0xD8, 0xFF^0x3B, 0xFF^0x1E,
   0xFF^0x02, 0xFF^0x00, 0xFF^0x73, 0xFF^0x1D,
   0xFF^0x83, 0xFF^0xEB, 0xFF^0xFF, 0xFF^0xFA,
   0xFF^0x8E, 0xFF^0xD3, 0xFF^0xBC, 0xFF^0x00
};
unsigned char FExclude_12[] =          /* LINK V3.69 /EXEPACK */
{
   0xFF^0x8B, 0xFF^0xE8, 0xFF^0x8C, 0xFF^0xC0,
   0xFF^0x05, 0xFF^0x10, 0xFF^0x00, 0xFF^0x0E,
   0xFF^0x1F, 0xFF^0xA3, 0xFF^0x04, 0xFF^0x00,
   0xFF^0x03, 0xFF^0x06, 0xFF^0x0C, 0xFF^0x00,
   0xFF^0x8E, 0xFF^0xC0, 0xFF^0x8B, 0xFF^0x0E,
   0xFF^0x06, 0xFF^0x00, 0xFF^0x8B, 0xFF^0xF9
};
unsigned char FExclude_13[] =          /* PKLITE V1.12 -e */
{
   0xFF^0xBA, 0xFF^0xFF, 0xFF^0xFF, 0xFF^0xB8,
   0xFF^0xFF, 0xFF^0xFF, 0xFF^0x05, 0xFF^0x00,
   0xFF^0x00, 0xFF^0x3B, 0xFF^0x06, 0xFF^0x02,
   0xFF^0x00, 0xFF^0x73, 0xFF^0x1A, 0xFF^0x2D,
   0xFF^0x20, 0xFF^0x00, 0xFF^0xFA, 0xFF^0x8E,
   0xFF^0xD0, 0xFF^0xFB, 0xFF^0x2D, 0xFF^0x19
};
unsigned char FExclude_14[] =          /* PROTECT! EXE/COM V1.0 */
{
   0xFF^0xE8, 0xFF^0xCB, 0xFF^0x00, 0xFF^0xFF,
   0xFF^0x8C, 0xFF^0xC8, 0xFF^0x2E, 0xFF^0x2B,
   0xFF^0x06, 0xFF^0x8B, 0xFF^0x00, 0xFF^0x8B,
   0xFF^0xF8, 0xFF^0x2E, 0xFF^0x03, 0xFF^0x06,
   0xFF^0xCC, 0xFF^0x00, 0xFF^0x50, 0xFF^0x2E,
   0xFF^0x8B, 0xFF^0x1E, 0xFF^0xCA, 0xFF^0x00
};
unsigned char FExclude_15[] =          /* ICE V1.00 */
{
   0xFF^0xEB, 0xFF^0x0A, 0xFF^0xFE, 0xFF^0x0A,
   0xFF^0xBE, 0xFF^0x26, 0xFF^0x01, 0xFF^0x8B,
   0xFF^0xFF, 0xFF^0x8B, 0xFF^0x0E, 0xFF^0x08,
   0xFF^0x01, 0xFF^0x8B, 0xFF^0x16, 0xFF^0x02,
   0xFF^0x01, 0xFF^0xB8, 0xFF^0x77, 0xFF^0x01,
   0xFF^0x50, 0xFF^0xFC, 0xFF^0xAD, 0xFF^0x33,
   0xFF^0xC2, 0xFF^0xAB, 0xFF^0x8B, 0xFF^0xD0,
   0xFF^0xE2, 0xFF^0xF8
};
unsigned char FExclude_16[] =          /* EMX? Unknown... Various versions */
{
   0xFF^0xB8, 0xFF^0xFF, 0xFF^0xFF, 0xFF^0x8E,
   0xFF^0xD8, 0xFF^0x8C, 0xFF^0x06, 0xFF^0x00,
   0xFF^0x00, 0xFF^0xE8, 0xFF^0xFF, 0xFF^0x00,
   0xFF^0xE8, 0xFF^0xFF, 0xFF^0x00, 0xFF^0xE8,
   0xFF^0xFF, 0xFF^0x00, 0xFF^0xE8, 0xFF^0xFF,
   0xFF^0x00, 0xFF^0xE8, 0xFF^0xFF, 0xFF^0xFF,
   0xFF^0xE8,
};
unsigned char FExclude_17[] =          /* Some strange version of PKLITE in EXE */
{
   0xFF^0xB8, 0xFF^0xFF, 0xFF^0xFF, 0xFF^0xBA,
   0xFF^0xFF, 0xFF^0xFF, 0xFF^0x05, 0xFF^0xFF,
   0xFF^0xFF, 0xFF^0x3B, 0xFF^0x06, 0xFF^0x02,
   0xFF^0x00, 0xFF^0x72, 0xFF^0xFF, 0xFF^0xB4,
   0xFF^0x09, 0xFF^0xBA, 0xFF^0xFF, 0xFF^0x01,
   0xFF^0xCD, 0xFF^0x21, 0xFF^0xB4, 0xFF^0x4C,
   0xFF^0xCD, 0xFF^0x21
};

unsigned char FJExclude_1[] =          /* TINYPROG V3.X */
{
   0xFF^0xEB,
   0xFF^0xFE, 0XFF^0x80,
   0xFF^0x83, 0xFF^0xEC, 0xFF^0x10,
   0xFF^0x83, 0xFF^0xE4, 0xFF^0xE0,
   0xFF^0x8B, 0xFF^0xEC, 0xFF^0x50,
   0xFF^0xBE, 0xFF^0x05, 0xFF^0x01,
   0xFF^0x03, 0xFF^0x36, 0xFF^0x01,
   0xFF^0x01, 0xFF^0x8C, 0xFF^0xD2
};

//riad//
unsigned char FExclude_18[] =          /* PKLITE.93 EXE another flavor again !*/
{
   0xFF^0x50, 0xFF^0xB8, 0xFF^0xFF, 0xFF^0xFF,
   0xFF^0xBA, 0xFF^0xFF, 0xFF^0xFF, 0xFF^0x05,
   0xFF^0x00, 0xFF^0x00, 0xFF^0x3B, 0xFF^0x06,
   0xFF^0x02, 0xFF^0x00, 0xFF^0x72, 0xFF^0x32,
   0xFF^0xB4, 0xFF^0x09, 0xFF^0xBA, 0xFF^0x1C,
   0xFF^0x01, 0xFF^0xCD, 0xFF^0x21, 0xFF^0xB8
};
#define FExclude_1_Offset 0x0000
#define FExclude_2_Offset 0x0000
#define FExclude_3_Offset 0x0000
#define FExclude_4_Offset 0x0000
#define FExclude_5_Offset 0x0000
#define FExclude_6_Offset 0x0000
#define FExclude_7_Offset 0x0000
#define FExclude_8_Offset 0x0000
#define FExclude_9_Offset 0x0000
#define FExclude_10_Offset 0x0000
#define FExclude_11_Offset 0x0000
#define FExclude_12_Offset 0x0000
#define FExclude_13_Offset 0x0000
#define FExclude_14_Offset 0x0000
#define FExclude_15_Offset 0x0000
#define FExclude_16_Offset 0x0000
#define FExclude_17_Offset 0x0000
#define FExclude_18_Offset 0x0000
#define FJExclude_1_Offset 0x0000

typedef struct
{
   unsigned char *exclude;
   int exclude_len;
   int exclude_offset;
} exclude_descriptor;

#define NUM_FExclude       18
#define NUM_FJExclude       1

#define MEDES(key, N) { key, sizeof(key), N }
exclude_descriptor v_FExclude[NUM_FExclude] =
{
  MEDES(FExclude_1, FExclude_1_Offset),
  MEDES(FExclude_2, FExclude_2_Offset),
  MEDES(FExclude_3, FExclude_3_Offset),
  MEDES(FExclude_4, FExclude_4_Offset),
  MEDES(FExclude_5, FExclude_5_Offset),
  MEDES(FExclude_6, FExclude_6_Offset),
  MEDES(FExclude_7, FExclude_7_Offset),
  MEDES(FExclude_8, FExclude_8_Offset),
  MEDES(FExclude_9, FExclude_9_Offset),
  MEDES(FExclude_10, FExclude_10_Offset),
  MEDES(FExclude_11, FExclude_11_Offset),
  MEDES(FExclude_12, FExclude_12_Offset),
  MEDES(FExclude_13, FExclude_13_Offset),
  MEDES(FExclude_14, FExclude_14_Offset),
  MEDES(FExclude_15, FExclude_15_Offset),
  MEDES(FExclude_16, FExclude_16_Offset),
  MEDES(FExclude_17, FExclude_17_Offset),
  MEDES(FExclude_18, FExclude_18_Offset)
};
exclude_descriptor v_FJExclude[NUM_FJExclude] =
{
  MEDES(FJExclude_1, FJExclude_1_Offset)
};


/* Returns TRUE if any patterns in the standard exclusion list were found
 * in the buffer.
 */

extern bOOlean match_vcomplex_sig(byte *, byte *, int);

bOOlean
in_fexclusion_list(unsigned char *buf, unsigned int buflen)
{
   int i;

   for (i=0; i<NUM_FExclude; ++i)
   {
      if (match_vcomplex_sig(v_FExclude[i].exclude,
              &buf[v_FExclude[i].exclude_offset],
              v_FExclude[i].exclude_len))
      {
#if !ESDC
    if (extra_guru) printf("FExclude: %d\n", i+1);
#endif
    return 1;
      }
   }
   if (buf[0] == 0xE9)
   {
      utwobytes targ;
      targ = 3 + buf[1] + (((utwobytes)buf[2]) << 8);
      if (targ+MAX_SIZE_SIGN < buflen)
      {
    for (i=0; i<NUM_FJExclude; ++i)
    {
       if (match_vcomplex_sig(v_FJExclude[i].exclude,
               &buf[v_FJExclude[i].exclude_offset+targ],
               v_FJExclude[i].exclude_len))
       {
#if !ESDC
          if (extra_guru) printf("FJExclude: %d\n", i+1);
#endif
          return 1;
       }
    }
      }
   }
   return 0;
}

#if 0
typedef struct s_EXE_header
{
   utwobytes signature;                         /* 5a4d */
   utwobytes length_mod_512;                    /* just so */
   utwobytes file_size;                         /* ... in 512 byte paragraphs */
   utwobytes num_relocation_table_items;        /* just so */
   utwobytes size_of_header;                    /* ... in 16 byte paragraphs */
   utwobytes min_num_paragraphs;                /* ... needed above program */
   utwobytes max_num_paragraphs;                /* ... desired above program */
   utwobytes stack_segment_displacement;        /* ... 16 byte paragraphs */
   utwobytes stack_pointer_upon_entry;          /* just so */
   utwobytes utwobytes_checksum;                     /* negative sum of all utwobytess */
   utwobytes instruction_pointer_upon_entry;    /* just so */
   utwobytes code_segment_displacement;         /* ... 16 byte paragraphs */
   utwobytes offset_relocation_table;           /* offset of first relocation item */
   utwobytes overlay_number;                    /* 0 for resident part of program */
} EXE_header;
#endif

typedef struct { CR_WORD off; CR_WORD seg; } LONG_PTR;
LONG_PTR compacted_relocation_table[9] = { { 0xFFFF, 0xFFFF} };

static char *cur_filename = "";
static int cur_fh = (-1);
static char *cur_oldbuf = NULL;
static unsigned long cur_oblen = 0l;
static int cur_ob_wholefile = 0;
static unsigned long cur_header_size = 0;

/* Initial state number for the MtE detector */
#define START 0
/* How many bytes before the entrypoint to read for Natas detector */
#define NATAS_OFFSET (0x30)

/* The top level algorithmic file-infector-detector. Gets entry point area
 * then calls other modules.
 * testfilename: The true name of the file, for file I/O if necessary (could
 *               be temp file created by decompression routines).
 * rootfilename: The name of the file for display purposes.
 * buf:          If not null, program uses this buffer, else it allocates its
 *               own MAXOFF-size buffer. Note: if it is not NULL, buf must be
 *               at least MAXOFF in size.
 * oldbuf:       Either the entire file in a large buffer, or the first 64
 *               bytes of the file.
 * oblen:        Length of the useful contents of oldbuf.
 * ob_wholefile: TRUE if the whole file is in the buffer.
 * inside_compressed: TRUE if we're scanning a decompressed file. This bOOlean
 *               is passed on down to eptscan, which into continues to pass
 *               it down if a virus is found; this "inside_compressed" bit is
 *               returned in the "vs_virus_found" structure passed back to the
 *               module calling vs_test_file().
 */
int alg_scan(char *testfilename,                                                // "\dodo\file.com"
        char *rootfilename,                                                // not necesary
        unsigned char *buf,                                                // NULL
        char *oldbuf,                                                              // used by load_from_file
        unsigned long oblen,                                       // sizeof(oldbuf) or HEAD_LEN
        int ob_wholefile,                                          // FALSE
        int inside_compressed,                                     // not necessary FALSE
        unsigned char saved_head[],                        // first HEAD_LEN bits of file
        unsigned int saved_head_len,                       // MIN(HEAD_LEN,oblen)
        unsigned long filelen,                                     // filezise
        unsigned long saved_epoint)                        // not necessary (-1L)
{
   long targ;               /* Offset into file of beginning of buf to read */
   int rc = 0;              /* Holds return codes for various detectors */
   int freeit = 0;          /* Set to 1 if we need to free "buf" on exit */
   size_t buflen = 0;       /* Buffer length, as seen by older algorithmic */
             /*  detectors that are passed a buffer such that */
             /*  buf[0] is the entry point. */
   size_t full_buflen = 0;  /* Buffer length, for real, i.e. the number of */
             /*  bytes read by the call to load_from_file() */
   int retc;                /* Return value from load_from_file() */
   int entry_point_type = EPT_DEFAULT; /* Classify type of entry point */
   utwobytes initip=0x0100;     /* Initial ip, used by SMEG detector */
   utwobytes initcs=START_SEG;  /* Initial cs */
   utwobytes initsp=0xFFFE;     /* Initial sp */
   utwobytes initss=START_SEG;  /* Initial ss */
   utwobytes initesds=START_SEG;/* Initial es and ds */
   unsigned char *vbuf;     /* Part of the buffer read from file, starting */
             /*  at the entry point and "buflen" bytes long */
   unsigned char *saved_vbuf = NULL; /* This buffer is a copy of the entry point */
             /*  used by the algorithmic polymorphic engine */
             /*  detectors after coderunner has been done. */
   unsigned int ept_offset_into_buf = 0; /* buf[ept_offset_into_buf] == vbuf[0] */
   int buf_dec = 0;         /* The amount by which "targ" is decremented */
             /*  before getting a buffer from the file. Is */
             /*  equal to ept_offset_into_buf */

   cur_fh = -1;

   /* Allocate a working buffer if we weren't passed one. Since we might be */
   /* doing a bulk scan on it in the coderunner portion, we need to provide a */
   /* little extra at the end */
   if (buf==NULL) {
      buf = (unsigned char *)malloc(MAXOFF+BEFORE+MAX_SIZE_SIGN-1);
      freeit = 1;
   }
   if (buf==NULL) {
      /*if (algdet_verbose) fprintf(stderr,"Out of memory in algdet.\n");*/
      return 200;
   }

#if 0
   /* Load beginning of file, so that we can determine where the entry */
   /*  is. */
   load_from_file(testfilename,&cur_fh,(char *)buf,0l,HLEN,oldbuf,oblen,ob_wholefile,&full_buflen,&retc);

   if (retc) {
      /*if (algdet_verbose) fprintf(stderr,"Error reading header.\n");*/
      rc = retc;
      goto done_alg_scan;
   }
#else
   memcpy(buf, saved_head, saved_head_len);
   full_buflen = saved_head_len;
#endif

   if (full_buflen < HLEN) {
      /*if (algdet_verbose) fprintf(stderr,"Very short file.\n");*/
      rc = 0;
      goto done_alg_scan;
   }

   cur_filename = testfilename;
   cur_oldbuf = oldbuf;
   cur_oblen = oblen;
   cur_ob_wholefile = ob_wholefile;
   cur_header_size = 0l;

   /* Determine format, find real entrypoint. Set entry_point_type variable,
    * and index into buffer where initial IP should point.
    */
   targ = 0;
   initip = 0x0100;
   if ((buf[0]=='M')&&(buf[1]=='Z') ||       /* EXE format */
       (buf[0]=='Z')&&(buf[1]=='M')) 
   {
      targ = (((long)buf[9])<<8) + (long)buf[8];    /* Header size, in paragraphs */
      targ = targ<<4;                               /*  cvt to bytes              */
      targ += (((long)buf[0x16] + (((long)buf[0x17])<<8))<<4);
      cur_header_size = targ;   /* Offset of code segment into file */
      targ += (((long)buf[0x14] + (((long)buf[0x15])<<8)));
      if (targ>0x00100000l) 
      {                         /* Kludge, due to common virus bug */
         targ -= 0x00100000l;   /* Make it reasonable.             */
      }
      entry_point_type = EPT_EXE;
      initip =  ((EXE_header *)buf)->instruction_pointer_upon_entry;
      initcs += ((EXE_header *)buf)->code_segment_displacement;
      initsp =  ((EXE_header *)buf)->stack_pointer_upon_entry;
      initss += ((EXE_header *)buf)->stack_segment_displacement;
      initesds -= 0x10;
   } 
   else if (buf[0]==0xE9)                    /* E9-COM format */                
   { 
      targ = 3 + buf[1] + (((utwobytes)buf[2]) << 8);
      entry_point_type = EPT_E9COM;
      initip = (unsigned short)(targ + 0x0100);
   }
// else if (buf[0]==0xE8)                    /* E8-COM format */                   
// {      
//    targ = 3 + buf[1] + (((utwobytes)buf[2]) << 8);
//    entry_point_type = EPT_E8COM;
//    initip = (unsigned short)(targ + 0x0100);
// } 
// else if ((buf[0]==0x4d)&&(buf[1]==0xE9))  /* Stupid POGUE's start */
// {  
//    targ = 4 + buf[2] + (((utwobytes)buf[3]) << 8);
//    entry_point_type = EPT_POGUE;
//    initip = (unsigned short)(targ + 0x0100);
// }
   else if ((buf[0]==0xFF)&&(buf[1]==0xFF))  /* Device driver */ 
   {
      targ = buf[6] + (((utwobytes)buf[7]) << 8); /* Take Strategy entrypoint */
      entry_point_type = EPT_DEVICE_DRIVER;
      initip = (unsigned short)(targ + 0x0100);
   }
   

   /* You'd think this should be 65536-256, but a few viruses (e.g. v2p6) infect */
   /* files just less than 64K and make them > 64K in size. So we're allowing */
   /* for 64K plus a more than reasonable size of virus. */
   if (entry_point_type != EPT_EXE &&
       filelen > (unsigned long)((unsigned long)65536l + 16384l))
   {
      /*if (algdet_verbose) fprintf(stderr,"Non-EXE file > 64K.\n");*/
      rc = 0;
      goto done_alg_scan;
   }

   /* Now the coderunner-based detector wants to see things back *before* */
   /* the entrypoint. Oblige it, by decrementing the "targ" variable, i.e. */
   /* the offset into the file where we start the read. Record the changes. */
   if (targ>0) {
     buf_dec = (int) MIN(targ, ((long)BEFORE));
     targ -= buf_dec;
     if (entry_point_type == EPT_EXE)
     {
   unsigned int size_of_header = ((EXE_header *)saved_head)->size_of_header << 4;
   if (targ < (long)size_of_header)
   {
      buf_dec -= (int) ((long)size_of_header - targ);
      targ = size_of_header;
   }
     }
     if (buf_dec < 0) buf_dec = 0;
     ept_offset_into_buf = buf_dec;
   }

   /* Load the region around the entrypoint, up to +-2K */
   load_from_file(testfilename,&cur_fh,(char *)buf,targ,MAXOFF+buf_dec,oldbuf,oblen,ob_wholefile,&full_buflen,&retc);

   if (retc) {
      /*if (algdet_verbose) fprintf(stderr,"Entrypoint not found.\n");*/
      rc = 0;
      goto done_alg_scan;
   }

   /* Fix buflen up for older detectors, and set up a pointer */
   /*  that they can use */
   if (full_buflen >= ept_offset_into_buf)
      buflen = full_buflen-ept_offset_into_buf;
   else
      buflen = 0;
   if (buflen == 0) {
      rc = 0;
      /*if (algdet_verbose) fprintf(stderr,"Entrypoint past end of file.\n");*/
      goto done_alg_scan;
   }
   vbuf = &buf[ept_offset_into_buf];
   saved_vbuf = (unsigned char *)malloc(OLD_MAXOFF);
   if (saved_vbuf==NULL) {
      /*if (algdet_verbose) fprintf(stderr,"Out of memory in algdet.\n");*/
      fprintf(stderr,"Out of memory in algdet.\n");   //riad//
      rc = 200;
      goto done_alg_scan;
   }
   memcpy(saved_vbuf, vbuf, MIN(buflen, OLD_MAXOFF));

   if (in_fexclusion_list(vbuf, buflen))
      goto done_alg_scan;

#ifdef CR_DO_SCANNER       //riad//
   /* First call the search-pattern-at-entry-point scanner. */
   if (scan_epoint)
   {
      ept_look_in_buf(vbuf,
            rootfilename,
            entry_point_type,
            targ,
            inside_compressed,
            filelen,
            saved_epoint,
            T_FILE,
            testfilename);
   }
   /* Don't bother to continue if entry point scan found something */
   if (found_precise_match)
   {
      goto done_alg_scan;
   }

   /* Now call all the algorithmic detectors in sequence */
   /* Only bother to call them if they haven't been turned off and the */
   /* entry-point type is correct. */
   if (scan_Washburn && ((entry_point_type == EPT_E9COM) || always_warn))
   {
      rc = scan_for_1260(MIN(buflen,64), vbuf);
      if (rc == 65)
      {
    goto done_alg_scan;
      }
   }
   if (scan_V2P6 && ((entry_point_type == EPT_E9COM) || always_warn))
   {
      rc = v2p6_look_in_buf(vbuf);
      if (rc == 20)
      {
    goto done_alg_scan;
      }
   }
#if INC_TREMDETU
   if (scan_Tremor)
   {
      rc = trem_look_in_buf(vbuf,MIN(buflen,OLD_MAXOFF));
      if (rc == 30)
      {
    goto done_alg_scan;
      }
   }
#endif
#if INC_SBGDETU
   if (scan_sbg)
   {
      rc = sbg_look_in_buf(vbuf, 0, MIN(buflen,OLD_MAXOFF), entry_point_type);
      if (rc == 1)
      {
    rc = 60;
    goto done_alg_scan;
      }
   }
#endif
#if INC_SMGDETU
   if (scan_smg &&
       (entry_point_type==EPT_EXE || entry_point_type==EPT_E9COM))
   {
#if 0
extern int smgdet_ops;
extern int smgdet_meth;
#endif
      rc = smg_look_in_buf(vbuf, initip, buflen, initip, entry_point_type);
#if 0
     printf("rc %2d ops %2d meth %2d\n",
        rc, smgdet_ops, smgdet_meth);
#endif
      if (rc == 1)
      {
    rc = 80;
    goto done_alg_scan;
      }
   }
#endif
#if INC_NATDETU
   if (scan_natas && ept_offset_into_buf >= NATAS_OFFSET)
   {
      rc = nat_look_in_buf(vbuf-NATAS_OFFSET, NATAS_OFFSET, buflen, entry_point_type);
      if (rc == 1)
      {
    rc = 90;
    goto done_alg_scan;
      }
   }
#endif
#endif  //riad//

#ifdef CR_DO_SCANNER    //riad//
   if (code_run)
#else
   if(1)
#endif
   {
      int frrc;
      unsigned int ExeFile;

      ExeFile = ((saved_head[0] == 0x4D && saved_head[1] == 0x5A) ||
       (saved_head[0] == 0x5A && saved_head[1] == 0x4D));

      frrc = 
         filerun_and_scan(full_buflen,/*  need saved_head */
               ept_offset_into_buf,
               testfilename,        /* used by ReadFileRegion */
               entry_point_type==EPT_EXE ? 1 : 0,        
                                    /* only by the scanner */
               ExeFile,             /* need saved_head */
               START_SEG,
               ExeFile ? (((EXE_header *)saved_head)->size_of_header << 4) : 0,
               filelen,             /* provided in alg_scan */
               (CR_WORD *)compacted_relocation_table,
               ReadFileRegion,
               targ,
               T_FILE,
               buf,                 /* i can give NULL in alg_scan */
               rootfilename,        /* only by the scanner */
               inside_compressed,   /* i don't care */
               initesds,
               initcs,
               initip,
               initss,
               initsp,
               vs_scalpel,          /* don't care */
               saved_epoint);       /* only by the scanner */


#define  cr_SUCCESS                    (0)
#define  cr_ERROR                      (-1)     /* General error.             */
#define  cr_ERROR_CANT_OPEN_FILE       (-2)     /* Cannot open some file.     */
#define  cr_ERROR_CANT_READ_FILE       (-3)
#define  cr_ERROR_CANT_WRITE_FILE      (-4)
#define  cr_ERROR_CANT_CREATE_FILE     (-5)
#define  cr_ERROR_NOT_ENOUGH_MEMORY    (-8)
#define  cr_ERROR_TOO_MANY_EXEC_TRAPS  (-9)
#define  cr_ERROR_TOO_MANY_RW_TRAPS    (-10)
#define  cr_ERROR_EXEC_TRAP_OUTSIDE    (-11)
#define  cr_ERROR_TRAP_ADDR_EXISTS     (-12)
#define  cr_ERROR_NON_TRAP_TERMINATION (-13)
      switch(frrc)
      {
    case(cr_SUCCESS):
       break;
    case(cr_ERROR): /* I think these are OK?? */
    case(cr_ERROR_TOO_MANY_EXEC_TRAPS):
    case(cr_ERROR_TOO_MANY_RW_TRAPS):
    case(cr_ERROR_EXEC_TRAP_OUTSIDE):
    case(cr_ERROR_TRAP_ADDR_EXISTS):
    case(cr_ERROR_NON_TRAP_TERMINATION):
       break;
    case(cr_ERROR_CANT_WRITE_FILE):
    case(cr_ERROR_CANT_CREATE_FILE):
       rc = 50;
       break;
    case(cr_ERROR_CANT_OPEN_FILE):
    case(cr_ERROR_CANT_READ_FILE):
       rc = 51;
       break;
    case(cr_ERROR_NOT_ENOUGH_MEMORY):
       rc = 200;
       break;
    default:
       rc = 200;
       break;
      }
   }
#ifdef CR_DO_SCANNER       //riad//
   if (object_sigfound_cnt)
   {
      goto done_alg_scan;
   }
   if (scan_DA_MtE)
   {
      unsigned int i;

      /* Find first different byte */
      for (i=1; (i<MIN(buflen,OLD_MAXOFF)) && (saved_vbuf[i]==saved_vbuf[0]); i++) {}

      /* Ignore if less than 16 */
      if (i<16) i=0;

      if (i<MIN(buflen,OLD_MAXOFF))
      {
    rc = mte_look_in_buf(saved_vbuf,(int)i,MIN(buflen,OLD_MAXOFF),START,0);
    if (rc == 10)
    {
       goto done_alg_scan;
    }
      }
   }
   if (scan_psmpc)
   {
      rc = mpc_look_in_buf(saved_vbuf);
      if (rc == 48)
      {
    goto done_alg_scan;
      }
   }
   if (scan_tpe)
   {
      rc = tpe_look_in_buf(saved_vbuf, 0, MIN(buflen,OLD_MAXOFF), entry_point_type);
      if (rc == 1)
      {
    rc = 70;
    goto done_alg_scan;
      }
   }
#endif //riad//

done_alg_scan:
   memset(buf, 0, full_buflen);
   if (freeit) free(buf);
   if (saved_vbuf)
   {
      memset(saved_vbuf, 0, MIN(buflen, OLD_MAXOFF));
      free(saved_vbuf);
   }
   if (cur_fh!=(-1)) { close(cur_fh); cur_fh=(-1); }
   return rc;

}   /* end main */

/*==========================================================================*/
/** Load from the file named <filename>, using the FILE variable <*f>,     **/
/** into <buf>, from offset <startfrom> in the file, for at most <maxread> **/
/** bytes, oldbuf stuff as in the call to look_for_mte.  Return the count  **/
/** of bytes actually read in readct, or return a look_for_mte return code **/
/** in <retc> in case of a real error.                                     **/

static void load_from_file(char *filename, int *fh,     /* File involved */
            char *buf,                   /* Put data here */
            long startfrom,     /* This offset from file start */
            size_t maxread,     /* Read at most this many bytes */
            char *oldbuf, unsigned long oblen, int ob_wholefile,
            size_t *readct,     /* How many bytes really read? */
            int *retc)          /* Non-zero if real error */
{
   /* Can we just get all of it from the buffer? */
   if ((oldbuf)&&(startfrom<=oblen)&&((startfrom+maxread)<=oblen)) {
      *readct = maxread;
      *retc = 0;
      memcpy(buf,oldbuf+(int)startfrom,maxread);
      /*if (algdet_verbose) fprintf(stderr,"Entire read from buffer.\n");*/
      return;
   }

   /* Can we perhaps get all that there is to be gotten from the buffer? */
   if ((oldbuf)&&(ob_wholefile)&&(startfrom<=oblen)) {
      *readct = (size_t)oblen-(size_t)startfrom;
      *retc = 0;
      memcpy(buf,oldbuf+(int)startfrom,*readct);
      /*if (algdet_verbose) fprintf(stderr,"Read to eof from buffer.\n");*/
      return;
   }

   /* Do we perhaps know for sure that we can't do it at all? */
   if ((oldbuf)&&(ob_wholefile)&&(startfrom>oblen)) {
      *readct = 0;
      *retc = 50;
      /*if (algdet_verbose) fprintf(stderr,"Seek beyond EOF.\n");*/
      return;
   }

   /* Well, none of that worked, so we have to do file I/O.  Tsk! */

   /* Open the file if we need to. */
   if (*fh == (-1)) {
    /*printf("opening file (%s) in load_from_file, sf=%lu, mr=%u\n", filename, startfrom, maxread);*/
#ifdef __UNIX__
      *fh = open(filename, O_RDONLY);
#else
      *fh = sopen(filename,O_RDONLY | O_BINARY, SH_DENYNO);
#endif
      if (*fh==(-1)) {
    /*if (algdet_verbose) fprintf(stderr,"Error opening %s.\n",filename);*/
    *retc = 50;
    return;
      } /* endif */
   }

   /* Seek to the start point */
   if (lseek((*fh),startfrom,SEEK_SET) != startfrom) {
      /*if (algdet_verbose) fprintf(stderr,"LSEEK failed.\n");*/
      *retc = 50;
      return;
   }

   /* Read the stuff */
   *readct = read((*fh),buf,maxread);

   /* Changed for 2.4.1 on FEB/27/96, to check for read failure */
   if (*readct == -1)
   {
      *retc = 50;
   }
   else
   {
      *retc = 0;
   }

   /*if (algdet_verbose) fprintf(stderr,"Read from file.\n");*/
   return;

}  /* end load_from_file */

/*==========================================================================*/
/* Return 0, or 50 if some sort of problem including incomplete read.            */

int ReadFileRegion(CR_ULONG abs_offset,
          CR_BYTE  *buffer,
          CR_WORD  bytes_to_read)
{
   size_t readct;
   int retc;

   load_from_file(cur_filename,
        &cur_fh,
        (char *)buffer,
        abs_offset,
        bytes_to_read,
        cur_oldbuf,
        cur_oblen,
        cur_ob_wholefile,
        &readct,
        &retc);

   if (readct != bytes_to_read)
      retc = 50;

   return retc;
}
