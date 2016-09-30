/*=====================================================================================*/
/* This module builds database files for the specified first bytes.                    */
/* A "trie" data structure is used for each specified first byte. The base             */
/* of each trie is 65536 entries long, so each initial node in the trie                */
/* is effectively for a trigram.                                                       */
/*=====================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <limits.h>
#include <process.h>
#include <math.h>
#include <limits.h>
#include "fzmalloc.h"
#include "triedb.h"
#include "dbutil.h"
#include <malloc.h>
#include <assert.h>

/* Defines to use the low-overhead memory routines. Also some front-end
 * routines to handle some pointers that have to use the real malloc/free.
 * These are essential even for machines with lots of real memory, e.g.
 * without these a 512MB machine would run out of memory on a real corpus.
                                                                                       */

#ifndef  FZM
#define  FZM           1
#endif
#if      1
#define  SLOT          0
#define  mfzmalloc(x)  fzmalloc(x, SLOT)
#define  mfzfree(x)    fzfree(x, SLOT)
#define  mfzmalloc_done() fzmalloc(SLOT)
#define  mfzstrdup(x)  fzstrdup(x, SLOT)
#else
#define  mfzmalloc(x)  malloc(x)
#define  mfzfree(x)    free(x)
#define  mfzmalloc_done()
#define  mfzstrdup(x)  strdup(x)
#endif
void pr_usage(void)
{
   printf("usage: trie [options] fn1 fn2 ... | @fnlist\n");
   printf("options:\n -q         : quiet\n -fbXX : ngrams with first byte XX\n");
   printf("           -ctng      : count total ngrams\n");
   printf("           -cug       : count unigrams\n");
   printf("           -cbg       : count bigrams\n");
   printf("           -dbr=path  : output database directory, backslash terminated.\n");
}

typedef struct s_trie_el
{
   unsigned long count;
   struct s_trie_el *next_sibling;
   unsigned char b;
   struct s_trie_el *daughters;        /* Not allocated for leaf nodes, to save memory */
} trie_el;

/* Array should actually be allocated. This is a arbitrary limit.
                                                                                       */

#define  MAX_FIRST_BYTES 16
trie_el *gtrie[MAX_FIRST_BYTES][65536];
unsigned int trie_cnt = 0;
unsigned char first_bytes[MAX_FIRST_BYTES];
char *database_root = ".\\";

#if 0
typedef unsigned long BIGUINT;
typedef          long BIGINT;
#define BIGUINT_U "%u"
#define BIGUINT_UNL "%u"
#define BIGUINT_DU  "%d %u"
#define BIGUINT_DUNL "%d %u\n"
#else
typedef unsigned __int64 BIGUINT;
typedef          __int64 BIGINT;
#define BIGUINT_U "%I64u"
#define BIGUINT_UNL "%I64d\n"
#define BIGUINT_DU  "%d %I64u"
#define BIGUINT_DUNL "%d %I64u\n"
#endif

unsigned int count_total_ngrams = 0;
unsigned int count_unigrams = 0;
unsigned int count_bigrams = 0;

static BIGUINT ngramcounts[MAX_NGRAM_LEN+1];    /* 1 based to simplify indexing        */
static BIGUINT unigrams[256];
static BIGUINT bigrams[65536];

/* Bookkeeping; is also used to decide when to sort the trie                           */

unsigned long node_count = 0l;
unsigned long total_alloc = 0l;
unsigned long node_count_prev = 0l;
unsigned int quiet_mode = 0;
#define  MD            MAX_NGRAM_LEN
#define  WMD           (MD-1)
void init_trie(void)
{
   unsigned int i;
   unsigned int tnum;

   for (tnum = 0; tnum < trie_cnt; ++tnum)
   {
      for (i = 0; i < 65536; ++i)
      {
         gtrie[tnum][i] = mfzmalloc(sizeof(trie_el));
         ++node_count;
         total_alloc += sizeof(trie_el);
         memset(gtrie[tnum][i], 0, sizeof(trie_el));
         gtrie[tnum][i]->b = (unsigned char)(i >> 8);
      }
   }
   if (count_total_ngrams) memset(ngramcounts, 0, sizeof(ngramcounts));
   if (count_unigrams) memset(unigrams, 0, sizeof(unigrams));
   if (count_bigrams) memset(bigrams, 0, sizeof(bigrams));
}

/* Load a table of with tablelen lines of form "uint biguint".
 */
void
save_table(char *tablefilename, BIGUINT tablearray[], unsigned int tablelen, unsigned int tablebase)
{
   char table_pathname[1040];
   unsigned int i;
   FILE *outfile;

   strcpy(table_pathname, database_root);
   strcat(table_pathname, tablefilename);
   if  (!(outfile = fopen(table_pathname, "w"))) {fputs(table_pathname,stderr);perror(" save_table() open");exit(8);}
   for (i=0; i<tablelen; ++i)
   {
      fprintf(outfile, BIGUINT_DUNL, i+tablebase, tablearray[i]);
   }
   fclose(outfile);
}

/* When building the trie, we sort by count so that lookups are
 * faster. When done, we sort by value since that's what we want in
 * our database.
                                                                                       */

#define  SORT_BY_COUNT 1
#define  SORT_BY_BYTE  2

/* We want to sort by decreasing count, not increasing!
 * Used by qsort.
                                                                                       */

int compare_by_count(const void *p1, const void *p2)
{
   trie_el *t_el1 = *((trie_el **)p1);
   trie_el *t_el2 = *((trie_el **)p2);

   if (t_el1->count > t_el2->count)
      return -1;
   else
   if (t_el1->count < t_el2->count)
      return 1;
   else
      return 0;
}

/* Used by qsort.
                                                                                       */

int compare_by_byte(const void *p1, const void *p2)
{
   trie_el *t_el1 = *((trie_el **)p1);
   trie_el *t_el2 = *((trie_el **)p2);

   if (t_el1->b < t_el2->b)
      return -1;
   else
   if (t_el1->b > t_el2->b)
      return 1;
   else
      return 0;
}

/* Depths for recursive routines do not count first byte, so
 * they are 2, 3, ..., MAX_NGRAM_LENGTH-1
 * Recursive routine.
                                                                                       */

void sort_daughters(trie_el *trie, int sort_by, unsigned int depth)
{
   trie_el *t_el = NULL;
   trie_el *sort_array[256];
   unsigned int sort_array_cnt = 0;
   unsigned int i;

   if (depth == MAX_NGRAM_LEN-1)
      return ;
   if (trie->daughters)
   {

      /*********************************************************************************/
      /* Build an array to sort                                                        */
      /*********************************************************************************/

      for (t_el = trie->daughters;
           t_el != NULL;
           t_el = t_el->next_sibling)
      {
         sort_array[sort_array_cnt++] = t_el;
      }

      /*********************************************************************************/
      /* Sort the array using qsort                                                    */
      /*********************************************************************************/

      switch (sort_by)
      {
         case (SORT_BY_COUNT) :
            qsort(sort_array,
                  sort_array_cnt,
                  sizeof(trie_el *),
                  compare_by_count);
            break;
         case (SORT_BY_BYTE) :
            qsort(sort_array,
                  sort_array_cnt,
                  sizeof(trie_el *),
                  compare_by_byte);
            break;
      }

      /*********************************************************************************/
      /* Re-link the list                                                              */
      /*********************************************************************************/

      for (i = 0; i < sort_array_cnt-1; ++i)
         sort_array[i]->next_sibling = sort_array[i+1];
      sort_array[sort_array_cnt-1]->next_sibling = NULL;
      trie->daughters = sort_array[0];
   }
   for (t_el = trie->daughters;
        t_el != NULL;
        t_el = t_el->next_sibling)
   {
      sort_daughters(t_el, sort_by, depth+1);
   }
}

/* Passed an ngram, looks up each component ngram from 4grams on up.
 * If found, count is incremented, otherwise a new node is inserted.
 * The top level routine (insert) just handles the 3grams.
 * Recursive routine.
                                                                                       */

void insert_trie(unsigned char *ngram, unsigned int len,
                 trie_el *trie, unsigned int depth)
{
   trie_el *t_el;
   trie_el *prev;
   unsigned char first_byte;

   first_byte = *ngram;
   for (prev = t_el = trie->daughters;
        t_el != NULL;
        t_el = t_el->next_sibling)
   {
      if (t_el->b == first_byte)
      {
         /* If rollover, fix */
         if (++(t_el->count)==0l) t_el->count = 0xFFFFFFF0;
         if (len > 1)
            insert_trie(&ngram[1], len-1, t_el, depth+1);

         /******************************************************************************/
         /* Swap node we just incremented with head of list if appropriate             */
         /******************************************************************************/

         if (t_el != trie->daughters &&
             (t_el->count > trie->daughters->count *2))
         {

            /***************************************************************************/
            /* printf("Swapping1, cnt=%u/%u\n", t_el->count, trie->daughters->count);  */
            /***************************************************************************/

            prev->next_sibling = t_el->next_sibling;
            t_el->next_sibling = trie->daughters;
            trie->daughters = t_el;
         }
         return ;
      }
      prev = t_el;
   }
   if (t_el == NULL)
   {
      size_t alloc_size;

      /*********************************************************************************/
      /* printf("depth=%u\n", depth);                                                  */
      /*********************************************************************************/

      if (depth+1 < MAX_NGRAM_LEN-1)
         alloc_size = sizeof(trie_el);
      else
         alloc_size = sizeof(trie_el)-sizeof(trie_el *);
      t_el = mfzmalloc(alloc_size);
      total_alloc += alloc_size;
      memset(t_el, 0, alloc_size);
      ++node_count;
      t_el->b = first_byte;
      t_el->count = 1;

      /*********************************************************************************/
      /* printf("NN: %02X\n", t_el->b);                                                */
      /*********************************************************************************/

      if (prev)
      {
         prev->next_sibling = t_el;
      }
      else
      {
         trie->daughters = t_el;
      }
      if (len > 1)
         insert_trie(&ngram[1], len-1, t_el, depth+1);
   }
}

/* Recursive routine                                                                   */

void pr_trie(trie_el *trie, unsigned int depth)
{
   trie_el *t_el;
   char blanks[256];

   if (depth == MAX_NGRAM_LEN-1)
   {
      printf("\n");
      return ;
   }
   for (t_el = trie->daughters;
        t_el != NULL;
        t_el = t_el->next_sibling)
   {
      if (t_el != trie->daughters)
      {
         memset(blanks, ' ', (depth-1)*11+3);
         blanks[(depth-1)*11+3] = '\0';
         printf(blanks);
      }
      printf("%02X (%05X) ", t_el->b, t_el->count);
      pr_trie(t_el, depth+1);
   }
   if (trie->daughters == NULL)
      printf("\n");
}

/* Recursive routine                                                                   */

void write_trie(trie_el *trie, unsigned int depth,
                FILE *fpa[], unsigned char work_ngram[MD])
{
   trie_el *t_el;
   unsigned int work_depth;
   unsigned char log_count;

   if (depth == MAX_NGRAM_LEN-1)
      return ;
   for (t_el = trie->daughters;
        t_el != NULL;
        t_el = t_el->next_sibling)
   {
      work_ngram[depth] = t_el->b;
      work_depth = depth+1;

      /*********************************************************************************/
      /* fwrite(&work_depth, 1, 1, fpa[work_depth]);                                   */
      /*********************************************************************************/

      fwrite(work_ngram, work_depth, 1, fpa[work_depth]);
      log_count = (unsigned char)logcnt(t_el->count);
      fwrite(&log_count, 1, 1, fpa[work_depth]);
      write_trie(t_el, work_depth, fpa, work_ngram);
   }
}

void write_gtrie(void)
{
   unsigned int i;
   trie_el *t_el;
   FILE *fpa[MD];
   unsigned char work_ngram[WMD];
   unsigned int depth = 2;
   char fname[20];
   char fullpathfname[200];
   unsigned char log_count;
   unsigned int tnum;

   for (tnum = 0; tnum < trie_cnt; ++tnum)
   {
      for (i = 2; i <= WMD; ++i)
      {
         sprintf(fname, "trie%01u_%02X.out", i+1, first_bytes[tnum]);
         strcpy(fullpathfname, database_root);
         strcat(fullpathfname, fname);
         fpa[i] = fopen(fullpathfname, "wb");

         /******************************************************************************/
         /* setvbuf(fpa[i], NULL, 32768, _IOFBF);                                      */
         /******************************************************************************/

         setvbuf(fpa[i], NULL, _IOFBF, 32768);
      }
      for (i = 0; i < 65536; ++i)
      {
         t_el = gtrie[tnum][i];
         if (t_el->count)
         {
            work_ngram[0] = (unsigned char)(i >> 8);
            work_ngram[1] = (unsigned char)i;

            fwrite(work_ngram, depth, 1, fpa[depth]);
            log_count = (unsigned char)logcnt(t_el->count);
            fwrite(&log_count, 1, 1, fpa[depth]);
            write_trie(t_el, depth, fpa, work_ngram);
         }
      }
      for (i = 2; i <= WMD; ++i)
      {
         fclose(fpa[i]);
      }
   }
}

void pr_gtrie(void)
{
   unsigned int i;
   trie_el *t_el;
   unsigned int tnum;

   for (tnum = 0; tnum < trie_cnt; ++tnum)
   {
      for (i = 0; i < 65536; ++i)
      {
         t_el = gtrie[tnum][i];
         if (t_el->count)
         {
            printf("%02X %02X (%05X) ",
                   (unsigned char)(i >> 8),
                   (unsigned char)i,
                   t_el->count);
            pr_trie(t_el, 2);
         }
      }
   }
}

/* Recursive routine                                                                   */

unsigned long sum_daughters(trie_el *trie, unsigned int depth)
{
   trie_el *t_el = NULL;
   unsigned long rv = 0l;

   if (depth == MAX_NGRAM_LEN-1)
      return 0l;
   for (t_el = trie->daughters;
        t_el != NULL;
        t_el = t_el->next_sibling)
   {
      rv += 1;
      rv += t_el->b;
      rv += sum_daughters(t_el, depth+1);
   }
   return  rv;
}

/* Diagnostic routine to detect corruption.                                            */

unsigned long sum_gtrie(void)
{
   unsigned int i;
   unsigned long rv = 0l;
   unsigned int tnum;

   for (tnum = 0; tnum < trie_cnt; ++tnum)
   {
      for (i = 0; i < 65536; ++i)
      {
         rv += 1;
         rv += gtrie[tnum][i]->b;
         rv += sum_daughters(gtrie[tnum][i], 2);
      }
   }
   return  rv;
}

/* Top level trie sort routine.
                                                                                       */

void sort_gtrie(int sort_by)
{
   unsigned int i;
   unsigned int tnum;

   /************************************************************************************/
   /* printf("sum_gtrie=%lu\n", sum_gtrie());                                          */
   /************************************************************************************/

   for (tnum = 0; tnum < trie_cnt; ++tnum)
   {
      for (i = 0; i < 65536; ++i)
      {
         sort_daughters(gtrie[tnum][i], sort_by, 2);
      }
   }

   /************************************************************************************/
   /* printf("sum_gtrie=%lu\n", sum_gtrie()); printf("\n");                            */
   /************************************************************************************/

}

/* Passed an ngram, looks up each component ngram from 3grams on up.
 * If found, count is incremented, otherwise a new node is inserted.
 * This is actually the top level routine that just handles the 3grams.
 * No inserts are done by this routine, since all 3grams have already
 * been allocated.
                                                                                       */

void insert(unsigned char *ngram, unsigned int len, unsigned int tnum)
{
   unsigned long w;
   trie_el *work_el;

   w = ((unsigned long)ngram[0] << 8)+(unsigned long)ngram[1];

   /************************************************************************************/
   /* printf("w=%04X\n", w);                                                           */
   /************************************************************************************/

   work_el = gtrie[tnum][(unsigned int)w];
   ++work_el->count;
   if (len > 2)
   {
      insert_trie(&ngram[2], len-2, work_el, 2);
   }
}

/* Allocate a global 8 megabyte file I/O buffer initially.
 * Grow it as required. Files are read in their entirety into the buffer.
 * There may be memory fragmentation problems here if we're not careful.
                                                                                       */

unsigned char *fbuffer = NULL;
#define  FBUFFER_INIT_SIZE ((long)(1024l*1024l*8l))
long fbuffer_size = FBUFFER_INIT_SIZE;

void
extra_counting(long fl, unsigned char *fbuf)
{
   if (count_total_ngrams)
   {
      int i;
      for (i=1; i<=MAX_NGRAM_LEN; ++i)
         ngramcounts[i] += (BIGUINT)(fl-i+1);
   }
   if (count_unigrams)
   {
      long i;
      for (i=0; i<fl; ++i)
         unigrams[fbuf[i]] += 1;
   }
   if (count_bigrams)
   {
      long i;
      for (i=0; i<fl-1; ++i)
         bigrams[*((unsigned short *)(&fbuf[i]))] += 1;
   }
}

/* Process all the ngrams in a file, adding them to our data structures.
                                                                                       */

int proc_fname(char *fname)
{
   FILE *fp = NULL;
   long fl;
   unsigned long total_fl = 0l;
   int rv = 0;
   int rc;
   long i;
   unsigned long tnum;
   static unsigned long file_count = 0;

   printf("processing (%s)\n", fname);
   fp = fopen(fname, "rb");
   if (fp == NULL)
   {
      rv = 1;
      goto done_proc_fname;
   }

   setbuf(fp, NULL);
   fl = filelength(fileno(fp));
   if (fl < MD)
      goto done_proc_fname;

   total_fl += (unsigned long)fl;
   if (fbuffer == NULL || fl > fbuffer_size)
   {
      if (fbuffer != NULL)
         free(fbuffer);
      fbuffer_size = fl > fbuffer_size?fl : fbuffer_size;
      fbuffer = malloc(fbuffer_size);
   }
   if (fbuffer == NULL)
   {
      rv = 1;
      goto done_proc_fname;
   }

   rc = fread(fbuffer, 1, fl, fp);
   if (rc != fl)
   {
      rv = 1;
      goto done_proc_fname;
   }

   extra_counting(fl, fbuffer);

   for (tnum = 0; tnum < trie_cnt; ++tnum)
   {
      for (i = 0; i < fl-(MD-1); ++i)
      {
         if (fbuffer[i] == first_bytes[tnum])
            insert(&fbuffer[i+1], WMD, tnum);
      }
      for (i = fl-(MD-1); i < fl-(MD-3); ++i)
      {
         if (fbuffer[i] == first_bytes[tnum])
            insert(&fbuffer[i+1], fl-(i+1), tnum);
      }
   }
   if (((file_count++%(8*(1+((unsigned long)sqrt((double)file_count))))) == 0) &&
       (node_count > node_count_prev+(unsigned long)sqrt((double)node_count)))
   {
      printf("sorting tree\n");
      sort_gtrie(SORT_BY_COUNT);
      node_count_prev = node_count;
   }
   printf("node_count = %lu, total_alloc=%lu\n", node_count, total_alloc);
done_proc_fname:
   if (fp != NULL)
      fclose(fp);
   return  rv;
}

void proc_fname_list(char *fname_list, unsigned int *proc_cnt)
{
   FILE *lfp = NULL;
   char worknbuf[256];
   char fname[256];

   lfp = fopen(fname_list, "r");
   if (lfp == NULL)
      goto done_proc_fname_list;

   for (; ; )
   {
      if (fgets(worknbuf, 256, lfp) == NULL)
         goto done_proc_fname_list;

      strcpy(fname, worknbuf);
      if (fname[strlen(fname)-1] == '\n')
         fname[strlen(fname)-1] = '\0';
      if (proc_fname(fname) == 0)
         ++(*proc_cnt);
   }
done_proc_fname_list:
   if (lfp != NULL)
      fclose(lfp);
   return ;
}

int main(int argc, char **argv)
{
   unsigned int proc_cnt = 0;
   long i;

   if (argc < 2)
   {
      pr_usage();
      return 1;
   }

   /************************************************************************************/
   /* First pass through arguments                                                     */
   /************************************************************************************/

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
            if (!strnicmp(&argv[i][1], "fb", 2))
            {
               int first;

               if (trie_cnt >= MAX_FIRST_BYTES)
               {
                  fprintf(stderr, "too many first bytes\n");
                  exit(1);
               };
               sscanf(&argv[i][3], "%02X", &first);
               first_bytes[trie_cnt] = (unsigned char)first;
               printf("first byte=%02X\n", first_bytes[trie_cnt]);
               ++trie_cnt;
               break;
            }
            else
            if (!strnicmp(&argv[i][1], "ctng", 4))
            {
               count_total_ngrams = 1;
               break;
            }
            else
            if (!strnicmp(&argv[i][1], "cug", 3))
            {
               count_unigrams = 1;
               break;
            }
            else
            if (!strnicmp(&argv[i][1], "cbg", 3))
            {
               count_bigrams = 1;
               break;
            }
            else
            if (!strnicmp(&argv[i][1], "dbr=", 4))
            {
               database_root = strdup(&argv[i][5]);
               break;
            }
            pr_usage();
            return 1;
            break;
         default  :
            break;
      }
   }

   /************************************************************************************/
   /* Look for 0x01 by default. Just for testing.                                      */
   /************************************************************************************/

   if (trie_cnt < 1)
   {
      trie_cnt = 1;
      first_bytes[0] = 0x01;
   }
   init_trie();

   /************************************************************************************/
   /* Second pass through arguments                                                    */
   /************************************************************************************/

   for (i = 1; i < argc; ++i)
   {
      switch (argv[i][0])
      {
         case ('-') :
         case ('/') :
            break;
         case ('@') :
            proc_fname_list(&argv[i][1], &proc_cnt);
            break;
         default  :
            if (proc_fname(argv[i]) == 0)
               ++proc_cnt;
            break;
      }
   }
   sort_gtrie(SORT_BY_BYTE);
   if (!quiet_mode)
      pr_gtrie();
   write_gtrie();

   if (count_total_ngrams) save_table("ngram.counts", &ngramcounts[1], MAX_NGRAM_LEN, 1);
   if (count_unigrams) save_table("unigrams", unigrams, 256, 0);
   if (count_bigrams) save_table("bigrams", bigrams, 65536, 0);
   /************************************************************************************/
   /* At this point, we should free the trie, but since we're finishing anyway...      */
   /************************************************************************************/

   if (fbuffer)
      free(fbuffer);
   return 0;
}
