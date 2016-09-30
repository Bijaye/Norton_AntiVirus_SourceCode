/* UNARJ.H, UNARJ, R JUNG, 02/17/93
 * Include file
 * Copyright (c) 1990 by Robert K Jung.  All rights reserved.
 *
 *   This code may be freely used in programs that are NOT ARJ archivers
 *   (both compress and extract ARJ archives).
 *
 *   If you wish to distribute a modified version of this program, you
 *   MUST indicate that it is a modified version both in the program and
 *   source code.
 *
 *   If you modify this program, I would appreciate a copy of the new
 *   source code.  I am holding the copyright on the source code, so
 *   please do not delete my name from the program files or from the
 *   documentation.
 *
 * Modification history:
 * Date      Programmer  Description of modification.
 * 04/05/91  R. Jung     Rewrote code.
 * 04/23/91  M. Adler    Portabilized.
 * 04/29/91  R. Jung     Added volume label support.
 * 05/30/91  R. Jung     Added SEEK_END definition.
 * 06/03/91  R. Jung     Changed arguments in get_mode_str() and
 *                       set_ftime_mode().
 * 06/28/91  R. Jung     Added new HOST OS numbers.
 * 07/08/91  R. Jung     Added default_case_path() and strlower().
 * 07/21/91  R. Jung     Fixed #endif _QC comment.
 * 08/27/91  R. Jung     Added #ifdef for COHERENT.
 * 09/01/91  R. Jung     Added new host names.
 * 12/03/91  R. Jung     Added BACKUP_FLAG.
 * 04/06/92  R. Jung     Added ARCHIMEDES.
 * 02/17/93  R. Jung     Improved ARJ header information.  Added ARJ_M_VERSION.
 * 02/08/98  Symantec    Converted to C++ lib for recursive & multithread use.
 *
 */

#ifndef _ARH_DEF_
#define _ARH_DEF_

#include <stdio.h>
#include <limits.h>

typedef unsigned char  uchar;   /*  8 bits or more */
typedef unsigned int   uint;    /* 16 - 32 bits or more */
typedef unsigned short ushort;  /* 16 bits or more */
typedef unsigned long  ulong;   /* 32 bits or more */

#define USHRT_BIT   (CHAR_BIT * sizeof(ushort))

/* ********************************************************* */
/* Environment definitions (implementation dependent)        */
/* ********************************************************* */

#if defined(__MSDOS__) || defined(WIN32)

#define OS                  0
#define PATH_SEPARATORS     "\\:"
#define PATH_CHAR           '\\'
#define FIX_PARITY(c)       c &= ASCII_MASK
#define DEFAULT_DIR         ""
#define SWITCH_CHARS        "-"
#define MAXSFX              25000L  /* size of self-extracting prefix */
#define ARJ_SUFFIX          ".ARJ"

#elif defined(_OS2)

#define OS                  0           /* ??? */
#define PATH_SEPARATORS     "\\:"
#define PATH_CHAR           '\\'
#define FIX_PARITY(c)       c &= ASCII_MASK
#define DEFAULT_DIR         ""
#define SWITCH_CHARS        "-/"
#define MAXSFX              25000L
#define ARJ_SUFFIX          ".ARJ"

#elif defined(__CI)

#define OS                  1
#define PATH_SEPARATORS     ">"
#define PATH_CHAR           '>'
#define FIX_PARITY(c)       c |= ~ASCII_MASK
#define DEFAULT_DIR         "*>"
#define SWITCH_CHARS        "-"
#define MAXSFX              500000L
#define ARJ_SUFFIX          ".ARJ"

#else

#define OS                  2
#define PATH_SEPARATORS     "/"
#ifndef PATH_CHAR
#define PATH_CHAR           '\\'
#endif
#define FIX_PARITY(c)       c &= ASCII_MASK
#define DEFAULT_DIR         ""
#define SWITCH_CHARS        "-"
#define MAXSFX              500000L
#define ARJ_SUFFIX          ".arj"

#endif

#define FNAME_MAX           512
#define ARJ_DOT             '.'

/* Error levels */

#if 0
#ifndef ERROR_DEFINES

#define ERROR_OK        0       /* success */
#define ERROR_WARN      1       /* minor problem (file not found) */
#define ERROR_FAIL      2       /* fatal error */
#define ERROR_CRC       3       /* CRC error */
#define ERROR_SECURE    4       /* ARJ security invalid or not found */
#define ERROR_WRITE     5       /* disk full */
#define ERROR_OPEN      6       /* can't open file */
#define ERROR_USER      7       /* user specified bad parameters */
#define ERROR_MEMORY    8       /* not enough memory */

#endif
#endif


/* ********************************************************* */
/* end of environmental defines                              */
/* ********************************************************* */

/* ********************************************************* */
/*
/* Structure of archive main header (low order byte first):
/*
/*  2  header id (comment and local file) = 0x60, 0xEA
/*  2  basic header size (from 'first_hdr_size' thru 'comment' below)
/*         = first_hdr_size + strlen(filename) + 1 + strlen(comment) + 1
/*         = 0 if end of archive
/*
/*  1  first_hdr_size (size up to 'extra data')
/*  1  archiver version number
/*  1  minimum archiver version to extract
/*  1  host OS     (0 = MSDOS, 1 = PRIMOS, 2 = UNIX, 3 = AMIGA, 4 = MACDOS)
/*               (5 = OS/2, 6 = APPLE GS, 7 = ATARI ST, 8 = NEXT)
/*               (9 = VAX VMS)
/*  1  arj flags (0x01 = GARBLED_FLAG, 0x02 = OLD_SECURED_FLAG)
/*               (0x04 = VOLUME_FLAG,  0x08 = EXTFILE_FLAG)
/*               (0x10 = PATHSYM_FLAG, 0x20 = BACKUP_FLAG)
/*               (0x40 = SECURED_FLAG)
/*  1  arj security version (2 = current)
/*  1  file type            (2 = comment header)
/*  1  ?                   ]
/*  4  date time stamp created
/*  4  date time stamp modified
/*  4  archive size up to the end of archive marker
/*  4  file position of security envelope data
/*  2  entryname position in filename
/*  2  length in bytes of trailing security data
/*  2  host data
/*  ?  extra data
/*
/*  ?  archive filename (null-terminated)
/*  ?  archive comment  (null-terminated)
/*
/*  4  basic header CRC
/*
/*  2  1st extended header size (0 if none)
/*  ?  1st extended header
/*  4  1st extended header's CRC
/*  ...
/*
/*
/* Structure of archive file header (low order byte first):
/*
/*  2  header id (comment and local file) = 0x60, 0xEA
/*  2  basic header size (from 'first_hdr_size' thru 'comment' below)
/*         = first_hdr_size + strlen(filename) + 1 + strlen(comment) + 1
/*         = 0 if end of archive
/*
/*  1  first_hdr_size (size up to 'extra data')
/*  1  archiver version number
/*  1  minimum archiver version to extract
/*  1  host OS     (0 = MSDOS, 1 = PRIMOS, 2 = UNIX, 3 = AMIGA, 4 = MACDOS)
/*               (5 = OS/2, 6 = APPLE GS, 7 = ATARI ST, 8 = NEXT)
/*               (9 = VAX VMS)
/*  1  arj flags (0x01 = GARBLED_FLAG, 0x02 = NOT USED)
/*               (0x04 = VOLUME_FLAG,  0x08 = EXTFILE_FLAG)
/*               (0x10 = PATHSYM_FLAG, 0x20 = BACKUP_FLAG)
/*               (0x40 = NOT USED)
/*  1  method    (0 = stored, 1 = compressed most ... 4 compressed fastest)
/*  1  file type (0 = binary, 1 = text, 2 = comment header, 3 = directory)
/*               (4 = label)
/*  1  garble password modifier
/*  4  date time stamp modified
/*  4  compressed size
/*  4  original size
/*  4  original file's CRC
/*  2  entryname position in filename
/*  2  file access mode
/*  2  host data
/*  ?  extra data
/*     4 bytes for extended file position
/*
/*  ?  filename (null-terminated)
/*  ?  comment    (null-terminated)
/*
/*  4  basic header CRC
/*
/*  2  1st extended header size (0 if none)
/*  ?  1st extended header
/*  4  1st extended header's CRC
/*  ...
/*  ?  compressed file
/*
/* ********************************************************* */
/* ********************************************************* */
/*                                                           */
/*     Time stamp format:                                    */
/*                                                           */
/*      31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16      */
/*     |<---- year-1980 --->|<- month ->|<--- day ---->|     */
/*                                                           */
/*      15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0      */
/*     |<--- hour --->|<---- minute --->|<- second/2 ->|     */
/*                                                           */
/* ********************************************************* */

#define CODE_BIT          16

#define NULL_CHAR       '\0'
#define MAXMETHOD          4

#define ARJ_VERSION        3
#define ARJ_M_VERSION      6    /* ARJ version that supports modified date. */
#define ARJ_X_VERSION      3    /* decoder version */
#define ARJ_X1_VERSION     1
#define DEFAULT_METHOD     1
#ifndef DEFAULT_TYPE
#define DEFAULT_TYPE       0    /* if type_sw is selected */
#endif
#define HEADER_ID     0xEA60
#define HEADER_ID_HI    0xEA
#define HEADER_ID_LO    0x60
#define FIRST_HDR_SIZE    30
#define FIRST_HDR_SIZE_V  34
#define COMMENT_MAX     2048
#define HEADERSIZE_MAX   (FIRST_HDR_SIZE + 10 + FNAME_MAX + COMMENT_MAX)
#define BINARY_TYPE        0    /* This must line up with binary/text strings */
#define TEXT_TYPE          1
#define COMMENT_TYPE       2
#define DIR_TYPE           3
#define LABEL_TYPE         4

#define GARBLE_FLAG     0x01
#define VOLUME_FLAG     0x04
#define EXTFILE_FLAG    0x08
#define PATHSYM_FLAG    0x10
#define BACKUP_FLAG     0x20

typedef ulong UCRC;     /* CRC-32 */

#define CRC_MASK        0xFFFFFFFFL

#define ARJ_PATH_CHAR   '/'

#define FA_RDONLY       0x01            /* Read only attribute */
#define FA_HIDDEN       0x02            /* Hidden file */
#define FA_SYSTEM       0x04            /* System file */
#define FA_LABEL        0x08            /* Volume label */
#define FA_DIREC        0x10            /* Directory */
#define FA_ARCH         0x20            /* Archive */

#if 0
#define HOST_OS_NAMES1 "MS-DOS","PRIMOS","UNIX","AMIGA","MAC-OS","OS/2"
#define HOST_OS_NAMES2 "APPLE GS","ATARI ST","NEXT","VAX VMS"
#define HOST_OS_NAMES  { HOST_OS_NAMES1, HOST_OS_NAMES2, NULL }
#endif

#define HOST_OS_MSDOS   0
#define HOST_OS_PRIMOS  1
#define HOST_OS_UNIX    2
#define HSOT_OS_AMIGA   3
#define HOST_OS_MACOS   4
#define HOST_OS_OS2     5
#define HOST_OS_APPLEGS 6
#define HOST_OS_ATARIST    7
#define HOST_OS_NEXT    8
#define HOST_OS_VMS     9
#define HOST_OS_WIN95   10

/* Timestamp macros */

#define get_tx(m,d,h,n) (((ulong)m<<21)+((ulong)d<<16)+((ulong)h<<11)+(n<<5))
#define get_tstamp(y,m,d,h,n,s) ((((ulong)(y-1980))<<25)+get_tx(m,d,h,n)+(s/2))

#define ts_year(ts)  ((uint)((ts >> 25) & 0x7f) + 1980)
#define ts_month(ts) ((uint)(ts >> 21) & 0x0f)      /* 1..12 means Jan..Dec */
#define ts_day(ts)   ((uint)(ts >> 16) & 0x1f)      /* 1..31 means 1st..31st */
#define ts_hour(ts)  ((uint)(ts >> 11) & 0x1f)
#define ts_min(ts)   ((uint)(ts >> 5) & 0x3f)
#define ts_sec(ts)   ((uint)((ts & 0x1f) * 2))

struct ArjHeaderInfo
{
    char   filename[FNAME_MAX];
    char   comment[COMMENT_MAX];
    uchar  arj_nbr;     // Version number of ARJ used to create
    uchar  arj_x_nbr;   // Version number of ARJ needed to extract
    uchar  host_os;     // HOST_OS_MSDOS, etc.
    uchar  arj_flags;   // GARBLE_FLAG, etc.
    short  method;        // (0 = stored, 1 = compressed most ... 4 compressed fastest)
    int    file_type;   // BINARY_TYPE, TEXT_TYPE, etc.
    ulong  time_stamp;  // Modified time; use ts_year() etc. to extract fields.
    long   compsize;    // Compressed file size 
    long   origsize;    // Original file size
    UCRC   file_crc;    // Original file CRC
    short  entry_pos;   // Index of final component in filename    
    uint   file_mode;    // OS specific attribute bits, e.g. FA_RDONLY
    ushort host_data;    // OS specific data
};

class CUnArj
{
public:
    CUnArj();
    ~CUnArj();
    int Open(char* arjfile);
    int Close();
    int ReadMainHeader(ArjHeaderInfo* pinfo);
    int ReadFileHeader(ArjHeaderInfo* pinfo);
    int CheckFileFlags();
    int ExtractFile(char* outfile);
    int SkipFile();

protected:
    void Init();
    void GetHeaderInfo(ArjHeaderInfo* pinfo);

    long origsize;
    long compsize;

    UCRC crc;

    FILE* arcfile;
    FILE* outfile;

    ushort bitbuf;

    uchar subbitbuf;
    uchar header[HEADERSIZE_MAX];

    char arc_name[FNAME_MAX];

    int bitcount;
    int file_type;
    int no_output;
    int error_count;

    /* unarj.c */

#if 0
    void   strlower(char* str);
    void   strupper(char* str);
#endif

    void*  malloc_msg(int size);
    void   disp_clock(void);
    void   error(char* fmt, char* arg);
    void   fillbuf(int n);
    ushort getbits(int n);
    int    fwrite_txt_crc(uchar* p, int n);
    void   init_getbits(void);

    void  make_crctable(void);
    void  crc_buf(char* str, int len);
    void  strparity(uchar* p);
    FILE* fopen_msg(char* name, char* mode);
    int   fget_byte(FILE* f, uint *puiByte);
    int   fget_word(FILE* f, ushort *pusWord);
    int   fget_longword(FILE* f, ulong *pulLongWord);
    void  fread_crc(uchar* p, int n, FILE* f);
    void  decode_path(char* name);
    void  get_date_str(char* str, ulong tstamp);
    int   parse_path(char* pathname, char* path, char* entry);
    void  strncopy(char* to, char* from, int len);
    uint  get_word(void);
    ulong get_longword(void);
    long  find_header(FILE* fd);
    int   read_header(int first, FILE* fd, char* name);
    void  skip(void);
    int   unstore(void);
    int   check_flags(void);
    int   extract(void);
    int   test(void);
    uint  ratio(long a, long b);
    void  list_start(void);
    void  list_arc(int count);
    void  execute_cmd(void);
    void  help(void);

    char   filename[FNAME_MAX];
    char   comment[COMMENT_MAX];
    char*  hdr_filename;
    char*  hdr_comment;

    ushort headersize;
    uchar  first_hdr_size;
    uchar  arj_nbr;
    uchar  arj_x_nbr;
    uchar  host_os;
    uchar  arj_flags;
    short  method;
    uint   file_mode;
    ulong  time_stamp;
    ulong  modified_time_stamp;
    short  entry_pos;
    ushort host_data;
    uchar  *get_ptr;
    UCRC   file_crc;
    UCRC   header_crc;

    long   first_hdr_pos;
    long   torigsize;
    long   tcompsize;

    int    clock_inx;

    UCRC   crctable[UCHAR_MAX + 1];


    /* environ.c */

    FILE*  file_open(char* name, char* mode);
    int    file_read(char* buf, int size, int nitems, FILE* stream);
    int    file_seek(FILE* stream, long offset, int mode);
    long   file_tell(FILE* stream);
    int    file_write(char* buf, int size, int nitems, FILE* stream);
    void*  xmalloc(int size);
    void   case_path(char* name);
    void   default_case_path(char* name);
    int    file_exists(char* name);
    void   get_mode_str(char* str, uint fmode);
    int    set_ftime_mode(char* name, ulong timestamp, uint fmode, uint host);
#if defined(UNIX)
    long    gettz();
    long    mstonix(ulong tstamp);
#endif

    /* decode.c */

    #define THRESHOLD    3
    #define DDICSIZ      26624
    #define MAXDICBIT   16
    #define MATCHBIT     8
    #define MAXMATCH   256
    #define NC          (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD)
    #define NP          (MAXDICBIT + 1)
    #define CBIT         9
    #define NT          (CODE_BIT + 3)
    #define PBIT         5
    #define TBIT         5

    #if NT > NP
    #define NPT NT
    #else
    #define NPT NP
    #endif

    #define CTABLESIZE  4096
    #define PTABLESIZE   256

    #define STRTP          9
    #define STOPP         13

    #define STRTL          0
    #define STOPL          7

    uchar* text;

    short  getlen;
    short  getbuf;

    ushort left[2 * NC - 1];
    ushort right[2 * NC - 1];
    uchar  c_len[NC];
    uchar  pt_len[NPT];

    ushort c_table[CTABLESIZE];
    ushort pt_table[PTABLESIZE];
    short  blocksize;

    int    decode(void);
    int    decode_f(void);

    int    make_table(int nchar, uchar* bitlen, int tablebits, ushort* table, int tablesize);
    int    read_pt_len(int nn, int nbit, int i_special);
    int    read_c_len(void);
    int    decode_c(ushort *pus);
    ushort decode_p(void);
    void   decode_start(void);
    short  decode_ptr(void);
    short  decode_len(void);
};


#define ARJERR_NONE          0  // No error
#define ARJERR_CANTOPEN      1  // Can't open file
#define ARJERR_CANTREAD      2  // Can't read file or unexpected end of file
#define ARJERR_NOTARJ        3  // File is not an ARJ archive
#define ARJERR_BADHEADER     4  // Bad header
#define ARJERR_BADHEADERCRC  5  // Bad header CRC
#define ARJERR_BADCOMMENT    6  // Bad comment header
#define ARJERR_BADTABLE      7  // Bad Huffman code
#define ARJERR_NOMEMORY      8  // Out of memory
#define ARJERR_NOMOREFILES   9  // No more files in ARJ archive
#define ARJERR_ENCRYPTED     10 // File is password encrypted (non-fatal)
#define ARJERR_UNKNOWNMETHOD 11 // Unsupported method (non-fatal)
#define ARJERR_UNKNOWNTYPE   12 // Unsupported file type (non-fatal)
#define ARJERR_UNKNOWNVER    13 // Unsupported version (non-fatal)
#define ARJERR_CANTCREATE    14 // Can't create file (non-fatal)
#define ARJERR_CANTWRITE     15 // Can't write file. Disk full?
#define ARJERR_CRCERROR      16 // Bad file CRC (non-fatal; file still exists)
#define ARJERR_MULTIVOLUME   17 // File is split across ARJ volumes
#define ARJERR_EXTENSION     18 // File is an extension of a split file


/* Message strings */

#if 0

extern char M_VERSION [];

extern char M_ARCDATE [];
extern char M_ARCDATEM[];
extern char M_BADCOMND[];
extern char M_BADCOMNT[];
extern char M_BADHEADR[];
extern char M_BADTABLE[];
extern char M_CANTOPEN[];
extern char M_CANTREAD[];
extern char M_CANTWRIT[];
extern char M_CRCERROR[];
extern char M_CRCOK   [];
extern char M_DIFFHOST[];
extern char M_ENCRYPT [];
extern char M_ERRORCNT[];
extern char M_EXTRACT [];
extern char M_FEXISTS [];
extern char M_HEADRCRC[];
extern char M_NBRFILES[];
extern char M_NOMEMORY[];
extern char M_NOTARJ  [];
extern char M_PROCARC [];
extern char M_SKIPPED [];
extern char M_SUFFIX  [];
extern char M_TESTING [];
extern char M_UNKNMETH[];
extern char M_UNKNTYPE[];
extern char M_UNKNVERS[];

#endif

#endif

/* end UNARJ.H */
