/* triedb.h                                                                            */
/* Sure hope we don't have to go above 5, because database will
 * get enormous.
                                                                                       */

#define  MAX_NGRAM_LEN 5

/* We'd prefer to use only one byte for the count. 8 bits seems
 * enough resolution for a log count to produce decent results.
                                                                                       */

#define  LEN_COUNT     1
#if      0                             /* 8 bits of index                              */
#define  IDXLEN        0x100
#define  MAXKEY        (IDXLEN-1)
#define  KEY(buf)      (((unsigned char *)(buf))[0])
#endif
#if      0                             /* 9 bits of index                              */
#define  IDXLEN        0x200
#define  MAXKEY        (IDXLEN-1)
#define  KEY(buf)      ((((unsigned int)(((unsigned char *)(buf))[0])) << 1) + \
                          (((unsigned int)(((unsigned char *)(buf))[1])) >> 7) )
#endif
#if      0                             /* 10 bits of index                             */
#define  IDXLEN        0x400
#define  MAXKEY        (IDXLEN-1)
#define  KEY(buf)      ((((unsigned int)(((unsigned char *)(buf))[0])) << 2) + \
                          (((unsigned int)(((unsigned char *)(buf))[1])) >> 6) )
#endif
#if      0                             /* 11 bits of index                             */
#define  IDXLEN        0x800
#define  MAXKEY        (IDXLEN-1)
#define  KEY(buf)      ((((unsigned int)(((unsigned char *)(buf))[0])) << 3) + \
                          (((unsigned int)(((unsigned char *)(buf))[1])) >> 5) )
#endif
#if      1                             /* 12 bits of index                             */
#define  IDXLEN        0x1000
#define  MAXKEY        (IDXLEN-1)
#define  KEY(buf)      ((((unsigned int)(((unsigned char *)(buf))[0])) << 4) + \
                          (((unsigned int)(((unsigned char *)(buf))[1])) >> 4) )
#endif

/* The structure for a individual database file's index.
                                                                                       */

typedef unsigned long index[IDXLEN];

/* Size of buffer in bytes used to finish up a binary database search; if
 * record window fits in this buffer it is read entirely into the buffer
 * and a binary search is done on a memory array.
                                                                                       */

#define  SBUFLEN       (8192)

/* Should record freads in dblookup be buffered or not? Timings indicate
 * they should not be buffered.
                                                                                       */

#define  BUFFERED_LOOKUP 1

/* We'd prefer to keep database files open, but some compilers/environments
 * run out of file handles if we keep all database files open. This
 * tells the database lookup to close database files right after looking up
 * an ngram.
 */
#define MINIMIZE_OPEN_FILES 0

/* This is to close files for ngram starting with XX-1 when ngram starting
 * with byte XX is encountered. Optim for ordered lookups.
 */
#define LINEAR_CLOSE_POLICY 1

