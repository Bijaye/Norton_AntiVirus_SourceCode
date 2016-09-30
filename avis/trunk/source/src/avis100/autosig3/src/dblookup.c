/* dblookup.c                                                                          */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <limits.h>
#include <process.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include "triedb.h"
#include "dbutil.h"
#include "proto.h"
#if      !defined(LOOKUP_DEBUG)
#define  LOOKUP_DEBUG  0
#endif

/*=====================================================================================*/

unsigned long fread_cnt = 0l;          /* Diagnostic; all fread()s counted during      */
                                       /* lookup                                       */
unsigned long lookups_cnt = 0l;

/* Default noisy. Set to one to turn off messages during lookup.                       */

unsigned int quiet_mode = 1;

/* Set to 1 to use indexed binary search. Default is off, i.e. by default will do
 * a full binary search of database files.
                                                                                       */

unsigned int use_index = 1;

/* Set to 1 to load indexes from single combined index file. By default, if
 * indexes are used they in multiple index files.
                                                                                       */

unsigned int use_single_index = 1;
unsigned int gngram_length;                  /* bsearch() compare function needs ngram length*/
unsigned char *sbuf[SBUFLEN];                /* Used to load multiple records from database  */
FILE *db_fp[MAX_NGRAM_LEN-2][256];           /* Save open file pointers to database files    */
const char *database_root = "c:\\triedb\\";  /* Default database root                        */

static BIGUINT ngramcounts[NMAX+1];    /* 1 based to simplify indexing                 */
static BIGUINT unigrams[256];
static BIGUINT bigrams[65536];

/* Used by indexed binary search lookups. Indexes are variable length, typdef          */
/* is in triedb.h                                                                      */

index indexes[MAX_NGRAM_LEN-2][256];

int compare_ngram(const void *p1,const void *p2)
{
   return  memcmp(p1, p2, gngram_length);
}

void init_db_fp(void)
{
   int i,j;

   for (i = 0; i < 256; ++i)
   {
      for (j = 0; j <= MAX_NGRAM_LEN-3; ++j)
      {
         db_fp[j][i] = NULL;
      }
   }
}

/* Load a table of with tablelen lines of form "uint biguint".
 */
void
load_table(char *tablefilename,
           BIGUINT tablearray[],
           unsigned int tablelen,
           unsigned int tablemin,
           unsigned int tablemax)
{
   char table_pathname[1040];
   int tableind;
   BIGUINT count;
   unsigned int i;
   FILE *infile;

   strcpy(table_pathname, database_root);
   strcat(table_pathname, tablefilename);
   if (!(infile = fopen(table_pathname, "r"))) {fputs(table_pathname,stderr);perror(" load_table() open");exit(8);}
   for (i=0; i<tablelen; ++i)
   {
      if (fscanf(infile, BIGUINT_DUNL, &tableind, &count) != 2) {fputs(table_pathname,stderr);perror(" scanf");exit(8);}
      assert(tableind >= (int)tablemin && tableind <= (int)tablemax);
      tablearray[tableind] = count;
   }
   fclose(infile);
}

void load_indexes(void)
{
   FILE *ifp;
   unsigned int rc;

   if (!use_single_index)
   {
      char idx_name[256];
      unsigned long ifl;
      unsigned int i;
      unsigned int ngram_length;

      memset(indexes, 0, sizeof(indexes));
      for (ngram_length = 3; ngram_length <= MAX_NGRAM_LEN; ++ngram_length)
      {
         for (i = 0; i < 256; ++i)
         {
            sprintf(idx_name, "%strie%01u_%02X.idx", database_root, ngram_length, i);

            /***************************************************************************/
            /* printf("idx_name=(%s)\n", idx_name);                                    */
            /***************************************************************************/

            ifp = fopen(idx_name, "rb");
            if (ifp == NULL)
            {
               fprintf(stderr,"failed open of combined index file (%s)\n", idx_name);
               exit(1);
            }
            setbuf(ifp, NULL);
            ifl = filelength(fileno(ifp));
            if (ifl != IDXLEN *sizeof(unsigned long))
            {
               fprintf(stderr, "bad filelength\n");
               exit(1);
            }

            /***************************************************************************/
            /* assert(ifl==IDXLEN*sizeof(unsigned long));                              */
            /***************************************************************************/

            rc = fread(&indexes[ngram_length-3][i], 1, ifl, ifp);
            if (rc != ifl)
            {
               fprintf(stderr,"failed read of index file (%s)\n", idx_name);
               exit(1);
            }
            fclose(ifp);
         }
      }
   }
   else                                /* Load individual index files                  */
   {
      char combindx_name[256];

      sprintf(combindx_name, "%strie.idx", database_root);
      ifp = fopen(combindx_name, "rb");
      if (ifp == NULL)
      {
         fprintf(stderr,"failed open of index file (%s)\n", combindx_name);
         exit(1);
      }

      /*********************************************************************************/
      /* setbuf(ifp, NULL);                                                            */
      /*********************************************************************************/

      rc = fread(indexes, sizeof(indexes), 1, ifp);
      if (rc != 1)
      {
         fprintf(stderr, "fread failed\n");
         exit(1);
      }

      /*********************************************************************************/
      /* assert(rc == 1);                                                              */
      /*********************************************************************************/

      fclose(ifp);
   }
}

unsigned long lookup_ngram_count(unsigned char *ngram,
                                 unsigned int len,
                                 const char *the_database_root)
{
   char db_name[256];
   unsigned long rv=0;
   unsigned long fl;
   unsigned long rmin;
   unsigned char test_ngram[8];
  // unsigned int i;
   unsigned long rcount;
   unsigned long rlen;
   unsigned long rnum;
   unsigned long probe;
   unsigned long nelem;
   int cmp;
   unsigned int first_probe = 1;
   static unsigned int first_lookup = 1;
   FILE *fp;
   size_t rc;
   int local_fread_cnt = 0;
   static unsigned long open_cnt=0l;

   ++lookups_cnt;                      // for statistics
   if (the_database_root != NULL)
   {
      database_root = the_database_root;
   }
#if      LOOKUP_DEBUG
   if (!quiet_mode)
   {
      printf("looking up: ");
      for (i = 0; i < len; ++i)
      {
         printf("%02X", ngram[i]);
      }
      printf("\n");
   }
#endif
#if 0
   {
      int i;
      printf("looking up: ");
      for (i = 0; i < len; ++i)
      {
         printf("%02X", ngram[i]);
      }
      printf("\n");
   }
#endif
   if (first_lookup)
   {
      init_db_fp();
      if (use_index)
      {
         load_indexes();
      }
      load_table("unigrams", unigrams, 256, 0, 255);
      load_table("bigrams", bigrams, 65536, 0, 65535);
      load_table("ngram.counts", ngramcounts, NMAX, 1, NMAX);
      first_lookup = 0;
   }
   /* These casts should go, if we convert to BIGUINT everywhere! */
   if (len == 1)
      return((unsigned long)unigrams[*((unsigned char *)(&ngram[0]))]);
   if (len == 2)
      return((unsigned long)bigrams[*((unsigned short *)(&ngram[0]))]);

  // Open the right file depending on the first ngram byte
   sprintf(db_name, "%strie%01u_%02X.out", database_root, len, ngram[0]);
   if (db_fp[len-3][ngram[0]])
   {
      fp = db_fp[len-3][ngram[0]];
   }
   else
   {
        //printf("Opening %1u-%02X\n", len, ngram[0]);
      fp = db_fp[len-3][ngram[0]] = fopen(db_name, "rb");
      ++open_cnt;
   }
   if (fp == NULL)
   {
      fprintf(stderr,"failed open of database file (%s)\n", db_name);
      fprintf(stderr,"open_cnt=%lu\n", open_cnt);
      exit(1);
   }
#if      !BUFFERED_LOOKUP
   setbuf(fp, NULL);
#endif
   fl = filelength(fileno(fp));
   rlen = (len-1+LEN_COUNT);
   rcount = fl/rlen;
   rmin = 0l;
   nelem = rcount;
   while (nelem > 0)
   {
      if (use_index && first_probe)
      {
         first_probe = 0;
         rmin = indexes[len-3][ngram[0]][KEY(&ngram[1])]/rlen;
         rnum = 0;
         if (ngram[1] != 255)
         {
            nelem = indexes[len-3][ngram[0]][KEY(&ngram[1])+1]/rlen-rmin;
         }
         else
         {
            nelem = rcount-rmin;
         }
      }
      else
      {
         rnum = nelem/2;
      }
      probe = rmin+rnum;

      /*********************************************************************************/
      /* printf("probe=%lu, nelem=%lu, rnum=%lu\n", probe, nelem, rnum);               */
      /*********************************************************************************/

      if (nelem *rlen <= SBUFLEN)
      {
         unsigned char *rec;

         if (fseek(fp, rmin *rlen, SEEK_SET))
         {
            perror("Can't fseek");
            exit(1);
         }
         ;
         rc = fread(sbuf, rlen, nelem, fp);
         if (rc != nelem)
         {
            fprintf(stderr, "fread failed\n");
            exit(1);
         }

         /******************************************************************************/
         /* assert(rc == nelem);                                                       */
         /******************************************************************************/

         ++fread_cnt;
         ++local_fread_cnt;
         gngram_length = len-1;
         rec = (unsigned char *)bsearch(&ngram[1], sbuf, nelem, rlen, compare_ngram);
         if (rec != NULL)
         {
            rv = rec[len-1];
         }
         break;
      }
      else
      {
         if (fseek(fp, probe *rlen, SEEK_SET))
         {
            perror("Can't fseek");
            exit(1);
         }
         ;
         rc = fread(test_ngram, rlen, 1, fp);
         if (rc != 1)
         {
            fprintf(stderr, "fread failed\n");
            exit(1);
         }

         /******************************************************************************/
         /* assert(rc == 1);                                                           */
         /******************************************************************************/

         ++fread_cnt;
         ++local_fread_cnt;

         /******************************************************************************/
         /* pr_dbngram(test_ngram, len, ngram[0]);                                     */
         /******************************************************************************/

         cmp = memcmp(&ngram[1], test_ngram, len-1);
         if (cmp == 0)
         {
            rv = test_ngram[rlen-1];
            break;
         }
         else
         {
            if (cmp < 0)
            {

               /************************************************************************/
               /* printf("key < probe\n");                                             */
               /************************************************************************/

               nelem = rnum;
            }
            else
            {

               /************************************************************************/
               /* printf("key > probe\n");                                             */
               /************************************************************************/

               rmin = probe+1;
               nelem = nelem-rnum-1;
            }
         }
      }
   }
  // The minimize open files policy has a slight overhead because we open and close .out files for each ngram lookup.
  // Instead we prefer the LINEAR_CLOSE_POLICY which closes the file for a given first byte only when the next first
  // byte has been encountered.
  // This policy fits the order in which we do the lookups, that is, sorted lexicographically by ngram in increasing order

#if MINIMIZE_OPEN_FILES
  fclose(fp);
  db_fp[len-3][ngram[0]] = NULL;
#elif LINEAR_CLOSE_POLICY
  if(ngram[0]) {
          unsigned char prev = ngram[0]-1; // The conversion from int to unsigned char is alright
          // search the previous open file for an ngram of the same length and with a lower first byte
          for (; prev < ngram[0] && db_fp[len-3][prev] == NULL ; --prev);
          // if found, close it
          if (prev < ngram[0] && db_fp[len-3][prev]) {
                  //printf("Closing %1u-%02X\n", len, prev);
                  fclose(db_fp[len-3][prev]);
                  db_fp[len-3][prev] = NULL;
          }
  }
#endif

//  printf("local fread count: %d\n", local_fread_cnt);
   rv = cnt_from_logcnt(rv);
   if (!quiet_mode)
   {
#if 0
      printf("%ccnt == %lu\n", rv == 0?'-':'+', rv);
#else
      printf("%ccnt == %I64u\n", rv == 0?'-':'+', rv);
#endif
   }
   return  rv;
}

// This functions closes all db files that are still open (should be no more than 3)
// it also releases some resources (memory) and prints some statistics
// (it is called from ngrams.c after the ngrams_lookup() call)
void close_all_db_files(void)
{
   int i,j;

   for (i = 0; i < 256; ++i)
   {
      for (j = 0; j <= MAX_NGRAM_LEN-3; ++j)
      {
         if (db_fp[j][i])
         {
            /*printf("Closing remaining %1d-%02X\n", j+3, i);*/
            fclose((db_fp[j][i]));
         }
      }
   }
   fprintf(stderr,"total fread count: %ul\n", fread_cnt);
   fprintf(stderr,"total lookups count: %ul\n", lookups_cnt);
   fprintf(stderr,"average fread count per lookup: %f\n", (double)fread_cnt/(double)lookups_cnt);
}

/* Score buffer for exact matches, no wildcards and no mismatches.
 * Will be a negative number; larger absolute value is better.
 * Should be used for buffers NMAX in length and shorter; it will
 * work with longer buffers but is exponential in length.
 */
double
scoreshortbuf(unsigned char *buf,
              unsigned int len,
              const char *dbr) /* database root */
{
   double rval = 0l;
   unsigned long ngcnt;

   if ((len <= NMAX) && (ngcnt = lookup_ngram_count(buf, len, dbr)))
   {
#if 0
   {
      unsigned int i;
      for (i = 0; i<NMAX-len; ++i)
         printf("   ");
      for (i=0; i<len; ++i)
         printf("%02X ", buf[i]);
      printf("[%lu]\n", ngcnt);
   }
#endif

      return log(ngcnt) - log((double)((BIGINT)ngramcounts[len]));
   }
   else
   {
#if 0
   {
      unsigned int i;
      for (i = 0; i<NMAX-len; ++i)
         printf("   ");
      for (i=0; i<len; ++i)
         printf("%02X ", buf[i]);
      printf("[?]\n", ngcnt);
   }
#endif
//      assert(len >= 3);
      return scoreshortbuf(&buf[0], len-1, dbr) +
             scoreshortbuf(&buf[1], len-1, dbr) -
             scoreshortbuf(&buf[1], len-2, dbr);
   }
}

/* Score buffer for exact matches, no wildcards and no mismatches.
 * Will be a negative number; larger absolute value is better.
 * "dbr" needs a trailing backslash.
 */
double
scorebuf(unsigned char *buf,
         unsigned int len,
         const char *dbr) /* database root */
{
   if (len <= NMAX)
      return scoreshortbuf(buf, len, dbr);
   else
   {
      unsigned int i;
      double rval = 0.0;
      for (i=0; i<len-(NMAX-1); ++i)
         rval += scoreshortbuf(&buf[i], NMAX, dbr);
      for (i=1; i<len-(NMAX-1); ++i)
         rval -= scoreshortbuf(&buf[i], NMAX-1, dbr);
      return rval;
   }
}

/*=====================================================================================*/
/* Everything to the #endif is debug code for the lookup routines, and can be          */
/* ignored.                                                                            */
/*=====================================================================================*/

#if      LOOKUP_DEBUG
#include "bcounts.h"

void fill_buf(unsigned char *buf,unsigned int buf_len)
{
#if      0
   unsigned int i;

   for (i = 0; i < buf_len; ++i)
      buf[i] = (unsigned char)myrand();
#else
   fill_buf_bytefreq(buf, buf_len);
#endif
}

void pr_dbngram(unsigned char *ngram,unsigned int len,unsigned char first_byte)
{
   int i;

   printf("%02X ", first_byte);
   for (i = 0; i < len-1; ++i)
      printf("%02X", ngram[i]);
   printf("(%02X)\n", ngram[len-1]);
}

int compare_by_maxgrams(const void *p1,const void *p2)
{
   return  memcmp(p1, p2, MAX_NGRAM_LEN);
}

unsigned int test_count = 1000;
#define  TEST_BYTE_COUNT (MAX_NGRAM_LEN*test_count)
unsigned char *testbuf = NULL;         /* [TEST_BYTE_COUNT+MAX_NGRAM_LEN-1]            */
unsigned int load_from_file = 0;       /* Test buffer loaded from file                 */
void pr_usage(void)
{
   fprintf(stderr,"usage: lookup [options]\n");
   fprintf(stderr,"options: -q -i -si -lfffname.exe -lurpath\n");
}

int main(int argc,char **argv)
{
   unsigned int i;
   unsigned int j;
   unsigned int len;
   unsigned char ngram[8];
   unsigned long lookup_cnt = 0l;
   unsigned int test_buf_len = TEST_BYTE_COUNT;

   quiet_mode = 0;
   use_index = 0;
   use_single_index = 0;
   testbuf = (unsigned char *)malloc(TEST_BYTE_COUNT+MAX_NGRAM_LEN-1);
   assert(testbuf != NULL);
   for (i = 1; i < argc; ++i)
   {
      switch (argv[i][0])
      {
         case ('-') :
         case ('/') :
            if (toupper(argv[i][1]) == 'Q')
            {
               quiet_mode = 1;
               break;
            }
            else
            if (!strnicmp(&argv[i][1], "lff", 3))
            {
               FILE *fp;
               int rc;

               fp = fopen(&argv[i][4], "rb");
               if (fp == NULL)
               {
                  fprintf(stderr,"Error opening (%s)\n", &argv[i][4]);
                  return 1;
               }
               rc = fread(testbuf, 1, test_count, fp);
               load_from_file = 1;
               test_buf_len = test_count < rc?test_count:rc;
               break;
            }
            else
            if (!strnicmp(&argv[i][1], "lur", 3))
            {
               database_root = &argv[i][4];
               break;
            }
            else
            if (toupper(argv[i][1]) == 'I')
            {
               use_index = 1;
               break;
            }
            else
            if (!strnicmp(&argv[i][1], "si", 2))
            {
               use_index = 1;
               use_single_index = 1;
               break;
            }
            else
            if (!strnicmp(&argv[i][1], "tc", 2))
            {
               sscanf(&argv[i][3], "%u", test_count);
               testbuf = (unsigned char *)realloc(testbuf, TEST_BYTE_COUNT+MAX_NGRAM_LEN
                  -1);
               assert(testbuf != NULL);
               break;
            }
            pr_usage();
            return 1;
            break;
         default  :
            pr_usage();
            return 1;
            break;
      }
   }
   if (!load_from_file)
   {
      fill_buf(testbuf, TEST_BYTE_COUNT);
      qsort(testbuf, sizeof(testbuf)/MAX_NGRAM_LEN, MAX_NGRAM_LEN, compare_by_maxgrams);
   }
   init_db_fp();
   for (i = 0; i < test_buf_len; i += load_from_file?1:MAX_NGRAM_LEN)
   {
      len = MAX_NGRAM_LEN;
      for (j = 0; j < len; ++j)
      {
         ngram[j] = testbuf[i+j];
      }
      lookup_ngram_count(ngram, len-2, NULL);
      lookup_ngram_count(ngram, len-1, NULL);
      lookup_ngram_count(ngram, len, NULL);
      lookup_cnt += 3;
   }
   printf("fread_cnt=%lu, lookup_cnt=%lu\n", fread_cnt, lookup_cnt);
   {
      int i,j;

      for (i = 0; i < 256; ++i)
      {
         for (j = 0; j <= MAX_NGRAM_LEN-3; ++j)
         {
            if (db_fp[j][i])
               fclose((db_fp[j][i]));
         }
      }
   }
   return 0;
}

#endif
