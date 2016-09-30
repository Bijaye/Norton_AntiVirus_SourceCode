#ifdef MACNEWAPI
#else
/* df.h -- header file for DocFile (OLE2 files) access functions */

#ifndef DF_H
#define DF_H

#include "common.h"

/* flags for FindFirst call */
#define SFF_SUBDIRSEARCH 0x1000          /* include substorages in search */
#define SFF_ROOT           0x02          /* include RootEntry entry */
#define SFF_STORAGE        0x01          /* include storage entries */
#define SFF_STREAM            0          /* include streams (default) */


#ifdef T_VXD
  #define MAXSFFLEVEL     7
#else
  #ifdef WS_16
    #define MAXSFFLEVEL     7
  #else
    #define MAXSFFLEVEL     32
  #endif
#endif

#define STRPAT_LEFTJUST  1
#define STRPAT_RIGHTJUST 2
#define STRPAT_ANYWHERE 0

typedef struct strpat_t {
   char  str[256];
   int   type;
} STRPAT;

typedef enum STGTY_t {
  df_STGTY_INVALID = 0,
  df_STGTY_STORAGE = 1,
  df_STGTY_STREAM = 2,
  df_STGTY_LOCKBYTES = 3,
  df_STGTY_PROPERTY = 4,
  df_STGTY_ROOT = 5,
} df_STGTY;

typedef enum tagDECOLOR {
  DE_RED = 0,
  DE_BLACK = 1,
} DECOLOR;

typedef struct df_time_t {
  dword part1, part2;
} DF_TIME;

typedef struct storage_t {
  word    usName[0x20];  /* name */
  word    _cb;           /* length of name in bytes, not words */
  byte    _mse;          /* attribute as defined by STGTY (was ucAttr)*/
  byte    _bflags;       /* "color" of entry as taken from DECOLOR */
  INT32   lBrother;      /* (_sidLeftSib: left sibling) comes before
                            sister in list, -1 = last */
  INT32   lSister;       /* (_sidRightSib: right sibling) storage
                            entry on the same directory level, -1 = last */
  INT32   lDaughter;     /* (_sidChild) first storage entry on
                            sub-storage, -1 = none */
  byte    _clsId[16];    /* CLSID of this storage if _mse=STGTY_STORAGE */
  INT32   _dwUserFlags;  /* user defined flags */
  DF_TIME _T_createTime; /* create time */
  DF_TIME _T_modTime;    /* modify time */
  INT32   lATentry;      /* first AT entry of stream (_sectStart) */
  INT32   lLen;          /* length of stream */
  INT32   _dptPropType;
} STORAGE;

typedef struct sff_t {
  STORAGE *SE;
  int      lenSE;
  byte     ucAttr;
  char    *szPath;
  word    *uszPath;
  INT32    lATentry;                    /* first AT entry of stream */
  INT32    lLen;                        /* length of stream */
  STRPAT  *SearchPat;
  INT32    iNode;                       /* current node */
  int      iDepth;                      /* counter to prevent inf */
} SFF;

#define MAXSFFDEPTH 100

typedef struct AT_t {
    int    sector;     /* sector number of the currently loaded sector */
    int    sectorsize; /* size in bytes of sector, either 512 or 64 */
    int    sectorbits; /* log2(sectorsize), either 9 or 6 */
    INT32 *pi;         /* buffer for a section of the AT */
    INT32  len;        /* number of entries */
    INT32 *AA;         /* pointer to the allocation array */
} _AT;

typedef struct dfheader_t {
   dword    dwID0, dwID1;                /* 0000 constant ID string */
   word     wUnknown08[8];               /* 0008 */
   dword    dwType;                      /* 0018 type ID ? */
   word     wUnknown1C;                  /* 001C */
   INT16    siPSectorSizeBits;           /* 001E Log2 of Primary sector size */
   INT16    siSSectorSizeBits;           /* 0020 Log2 of Secondary sector size */
   word     wUnknown22[7];               /* 0022 */
   INT32    liRootStorage;               /* 0030 sector number of storage */
   word     wUnknown34[4];               /* 0034 */
   INT32    liSATsector;                 /* 003C sector number of SAT */
   dword    dwUnknown40;                 /* 0040 */
   INT32    liFirstAuxATAA;              /* 0044 */
   INT32    liNumAuxATAA;                /* 0048 */
   INT32    liATAA[0x6D];                /* 004C Allocation array of allocation table */
} OLE2HEADER;

typedef struct df_t {
#ifdef STREAM_FILE_ACCESS
   FILE    *file;
#else
   int      file;
   unsigned long pwdmac_handles;
#endif
   char    *szFileName;
   _AT      PAT;  /* Primary Allocation Table buffer (PAT) */
   _AT      SAT;  /* Secondary Allocation Table buffer (SAT) */
   INT32    iRootStorage;
   INT32    iSATsector;     /* first sector number SAT in PAT */
   INT32    iPDATAsector;   /* sector number of the first primary data sector */
   INT32    iSDATAsector;   /* first sector of secondary Data area */
   STORAGE *SE;                          /* array of storage entries */
   int      lenSE;                       /* length in entries of storage */
   int      iOpenStreamCounter;
} DOCFILE;

typedef struct dfstream_t {
   DOCFILE *dffile;
   INT32    iNode;                          /* index of the storage entry */
   INT32    iFirst;                         /* first sector in AT */
   _AT     *AT;
   long     lPos,                           /* current stream pointer */
            lLen;                           /* length of stream */
   byte     ucAttr;
   char    *szPath;
} OLE2STREAM;

DOCFILE *df_OpenFile(char* fname);
DOCFILE *df_OpenFStream(unsigned long fIn);
int df_CloseFile(DOCFILE*);
OLE2STREAM *df_OpenStream(DOCFILE *, char *);
int df_CloseStream(OLE2STREAM *);
int df_SeekStream(OLE2STREAM *os, long fpos);
long df_ReadStream(OLE2STREAM *os, char *buf, int count);
long df_WriteStream(OLE2STREAM *os, char *buf, int count);
SFF *df_StreamFindFirst(DOCFILE *of, STRPAT *SearchPat);
int df_StreamFindNext(SFF *Sff);
int df_StreamFindClose(SFF *Sff);
int df_StreamSetSize(OLE2STREAM *os, INT32 newsize);
long df_DemangleFPos(DOCFILE *of, _AT *AT, INT32 iFirst, long fpos);
INT32 df_AA(DOCFILE *of, _AT *AT, INT32 iFirst, int iWant);
int df_DeleteNode(STORAGE SE[], int lenSE, INT32 iNode);
int df_GetSDATAsector(DOCFILE *of);
int df_LoadSE(DOCFILE *of);
int df_DeleteStream(DOCFILE *of, char *szName);
int df_StorageSetSize(DOCFILE *of, INT32 iEntry, INT32 newsize);
int df_DelChain(DOCFILE *of, struct AT_t *AT, INT32 iFirst);
//#ifdef TRACING
void df_tracef(char* pFormat, ...);
//#endif
#ifdef MCV
void printline(dword label, byte *line, int length);
extern int iDebugLevel;
#endif

extern int df_errno;

#define VS_ERROR_BASE 0

/* df PARSER ERRORS (they start with 800 decimal or 0x320 hex) */
#define EOLE2_OPEN_FAILED              (VS_ERROR_BASE + 0x320)
#define EOLE2_MALLOC_FAILED            (VS_ERROR_BASE + 0x321)
#define EOLE2_SETVBUF_FAILED           (VS_ERROR_BASE + 0x322)
#define EOLE2_PREMATURE_EOF            (VS_ERROR_BASE + 0x323)
#define EOLE2_NOT_OLE2_DOC             (VS_ERROR_BASE + 0x324)
#define EOLE2_LOADING_PAT_FAILED       (VS_ERROR_BASE + 0x325)
#define EOLE2_LOADING_SAT_FAILED       (VS_ERROR_BASE + 0x326)
#define EOLE2_LOADING_STORAGE_FAILED    (VS_ERROR_BASE + 0x327)
#define EOLE2_LOADING_SDATA_FAILED     (VS_ERROR_BASE + 0x328)
#define EOLE2_STORAGEENTRY_INVALID     (VS_ERROR_BASE + 0x329)
#define EOLE2_ACCESS_CHAIN_INVALID     (VS_ERROR_BASE + 0x32a)
#define EOLE2_INVALID_ACCESS_TABLE     (VS_ERROR_BASE + 0x32b)
#define EOLE2_INTSIZE_EXCEEDED                  (VS_ERROR_BASE + 0x32c)
#define EOLE2_GENERAL_FAILURE          (VS_ERROR_BASE + 0x32d)
#define EOLE2_OPEN_STREAM_FAILED       (VS_ERROR_BASE + 0x32e)

#ifdef __cplusplus
  #define INLINE inline
#else
  #define INLINE
#endif

#define LINELENGTH 16
#define PRIMARYSECTORSIZE 512
#define PRIMARYSECTORSIZE_BITS 9
#define SECONDARYSECTORSIZE 64
#define SECONDARYSECTORSIZE_BITS 6
#define OLE2_ARBITRARY_HINUM 511

#define df_AAlen(of,AT,iFirst) (df_AA((of), (AT), (iFirst), -1))

#ifdef OV
extern int iDebugLevel;
#define OVTRACEF0(n, s)\
  if (iDebugLevel > n) { printf(s);  }
#define OVTRACEF1(n, s, p1)\
  if (iDebugLevel > n) { printf(s, p1);  }
#define OVTRACEF2(n, s, p1, p2)\
  if (iDebugLevel > n) { printf(s, p1, p2);  }
#define OVTRACEF3(n, s, p1, p2, p3)\
  if (iDebugLevel > n) { printf(s, p1, p2, p3);  }
#define OVTRACEF4(n, s, p1, p2, p3, p4)\
  if (iDebugLevel > n) { printf(s, p1, p2, p3, p4);  }
#define OVTRACEF5(n, s, p1, p2, p3, p4, p5)\
  if (iDebugLevel > n) { printf(s, p1, p2, p3, p4, p5);  }
#define OVDPrintInt(n, v)\
   if (VDebugLevel > n) {\
      OVPrintInt(v);\
   }
#define OVDprintline(n, b, p, l)   \
if (iDebugLevel > n) {           \
   printline(b, p, l);           \
}
#define OVPrintInt(x) printf("%s = %i (0x%X)\n", #x, x, x)
#else
#define OVTRACEF0(n, s)
#define OVTRACEF1(n, s, p1)
#define OVTRACEF2(n, s, p1, p2)
#define OVTRACEF3(n, s, p1, p2, p3)
#define OVTRACEF4(n, s, p1, p2, p3, p4)
#define OVTRACEF5(n, s, p1, p2, p3, p4, p5)
#define OVDPrintInt(n, v)
#define OVDprintline(n, b, p, l)
#define OVPrintInt(x)
#endif

#endif

#endif
