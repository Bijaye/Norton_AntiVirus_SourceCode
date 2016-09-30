/* hash.c                                                                              */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include "proto.h"
#include "dblookup.h"

/*-------------------------------------------------------------------------------------*/
/* from fillhash.c                                                                     */
/* fillhash adds the appropriate n-grams from the given virus region
   virseg into the incremental hash table, carefully avoiding n-grams
   that have already been tallied in the permanent hash table.                         */

int fillhash(struct sigbyte virseg[],int vlength,struct htable *hashtable[],struct htable
              *global_hashtable[],unsigned char screen[])
{
   static struct htable *hashcell;
   int hash,hash2;
   int i,j;
   int match;
   int some_incremental_ngrams = 0;
   int filter = 0;
   int ngramsize;
   unsigned char vs[NMAX];

   /************************************************************************************/
   /* initialize man                                                                   */
   /************************************************************************************/

   hash = hash2 = 0;
   memset(vs, 0, NMAX *sizeof(unsigned char));

   /************************************************************************************/
   /* Loop over all bytes in virseg                                                    */
   /************************************************************************************/

   for (i = 0; i < vlength; i++)
   {
      for (j = 0; (j < NMAX) && ((i+j) < vlength) && virseg[i+j].qvalid; j++)
      {
         ;
      }
      ngramsize = j;
      for (j = 0; j < ngramsize; j++)
         vs[j] = virseg[i+j].hex;
      if (ngramsize >= 2)
      {
         // TRACEIT("FILLHASH", 0, "hash2=%d\n", vs[0]*256 + vs[1]);
         hash2 = vs[0]*256+vs[1];
      }

      /*********************************************************************************/
      /* If trigram or longer, consider adding it to the incremental hash table.       */
      /*********************************************************************************/

      if (ngramsize >= 3)
      {
         hash = ((hash2 << 8)+hash2+vs[2]+(ngramsize>=4?vs[3]:0))&HASHSIZE_MASK;

         /******************************************************************************/
         /* Determine whether the n-gram in question is already included in the        */
         /* permanent hash table.                                                      */
         /******************************************************************************/

         filter = 0;
         if (global_hashtable[hash])
         {
            hashcell = global_hashtable[hash];
            while (!(match = (hashcell->ngramsize == ngramsize) &&
                             !memcmp(hashcell->code, vs, sizeof(unsigned char)*ngramsize))
                   && hashcell->nextp)
               hashcell = hashcell->nextp;
            if (match)
               filter = 1;
         }
         if (!filter)                  /* The n-gram isn't in the permanent hash table,*/
                                       /* so try to add it to incremental hash table.  */
         {
            some_incremental_ngrams = 1;

            /***************************************************************************/
            /* The right-most bit of screen represents: indexed bigram is the first two*/
            /* bytes of some trigram in the incremental hash table (0=No., 1=Yes.)     */
            /***************************************************************************/

            if (((unsigned char)screen[hash2]&0x01) == 0x00)
               screen[hash2] += 1;
            hash2 = vs[1]*256+vs[2];

            /***************************************************************************/
            /* The second-right-most bit of screen represents: indexed bigram is the   */
            /* last two bytes of some trigram in the incremental hash table (0=No.,    */
            /* 1=Yes.)                                                                 */
            /***************************************************************************/

            if (((unsigned char)screen[hash2]&0x02) == 0x00)
               screen[hash2] += 2;

            /***************************************************************************/
            /* Find appropriate hash table chain, look at each element to establish    */
            /* whether the n-gram has already been added. If not, add it.              */
            /***************************************************************************/

            if (hashtable[hash])
            {
               hashcell = hashtable[hash];
               while (!(match = (hashcell->ngramsize >= ngramsize) &&
                                !memcmp(hashcell->code, vs, sizeof(unsigned char)*ngramsize)
                       ) &&
                      hashcell->nextp)
               {
                  //int x;
                  //for (x=0; x<hashcell->ngramsize; ++x)
                  //   printf("%02X", hashcell->code[x]);
                  //printf(" ");
                  hashcell = hashcell->nextp;
               }
               //printf(" !!\n");
               if (!match)             /* If not already in incremental hash table, add*/
                                       /* it.                                          */
                  hashcell->nextp = new_hashcell(vs, ngramsize);
            }
            else
               hashtable[hash] = new_hashcell(vs, ngramsize);
         }
      }
   }
   if (some_incremental_ngrams)
      return 0;                        /* We will need to scan corpus                  */
   else
      return 1;                        /* We won't need to scan corpus                 */
}

/* Add a new item to the hash table                                                    */

struct htable *new_hashcell(unsigned char seg[],int ngramsize)
{
   int j;
   struct htable *cell = NULL;
   //TRACEIT("new_hashcell", 0, "ngramsize=%d - seg[0]=%02X\n", ngramsize, seg[0]);

   if ((cell = (struct htable *)ALLOC(struct htable, 1)) != NULL)
   {
      for (j = 0; j < (ngramsize-2); j++)
         cell->count[j] = 0;
      cell->ngramsize = ngramsize;
      for (j = 0; j < ngramsize; j++)
         cell->code[j] = seg[j];
      cell->nextp = NULL;
   }
   else
      SIGPRINT(stderr, "**  Failure to allocate hashcell.\n");
   return (cell);
}

/*-------------------------------------------------------------------------------------*/
/* from hashprint.c                                                                    */

int hashprint(struct htable *hashtable[], char *hashfile, int count_threshold)
{
   FILE *output;
   static struct htable *hashcell;
   int flag;
   int i,j;

   SIGPRINT(stdout, "Printing hastable (%s)...", hashfile);
   output = fopen_check(hashfile, "wb");
   if (output == NULL)
   {
      return -1;
   }
   for (i = 0; i < HASHSIZE; i++)
   {
      unsigned char collision = 0x77;

      hashcell = hashtable[i];
      flag = 0;
      if (hashcell)
      {
         if (hashcell->count[0] >= (BIGINT) count_threshold)
         {
            flag = 1;
            if (fwrite(&i, sizeof(long), 1, output) < 1)
            {
               printf("\n");
               SIGPRINT(stderr, "**  Couldn't write to hash.table for some reason.\n");
               break;
            }
         }
         while (hashcell)
         {
            if (hashcell->count[0] >= (BIGINT) count_threshold)
            {
               if (collision == 0x88)
                  fwrite(&collision, sizeof(char), 1, output);

               /************************************************************************/
               /* print the index if we didn't                                         */
               /************************************************************************/

               if (flag == 0)
               {
                  flag = 1;
                  fwrite(&i, sizeof(long), 1, output);
               }

               /************************************************************************/
               /* print the ngram size                                                 */
               /************************************************************************/

               fwrite(&(hashcell->ngramsize), sizeof(unsigned short), 1, output);

               /************************************************************************/
               /* print the ngram string                                               */
               /************************************************************************/

               fwrite(hashcell->code, sizeof(char), hashcell->ngramsize, output);

               /************************************************************************/
               /* and print the counts                                                 */
               /************************************************************************/

               for (j = 0; j < (hashcell->ngramsize-2); j++)
               {
                  fwrite(&(hashcell->count[j]), sizeof(BIGINT), 1, output);
#if 0
                  {
                  int k;
                  printf("NG: ");
                  for (k = 0; k <= j+2; ++k)
                  {
                     printf("%02X", hashcell->code[k]);
                  }
                  printf(" : %I64d\n", hashcell->count[j]);
                  }
#endif
               }
               collision = 0x88;
            }
            hashcell = hashcell->nextp;
         }
         collision = 0x77;
         if (flag)
            fwrite(&collision, sizeof(char), 1, output);
      }
   }
   fprintf(stdout, "Done\n");
   fclose(output);
   return 1;
}

/*-------------------------------------------------------------------------------------*/
/* from loadhash.c                                                                     */

#define  MAX_HASHLINE_LEN 100000
int loadhash(struct htable *hashtable[],char *hashfile)
{
   FILE *hfile;
   char *hashline;
   long i,k;
   struct htable *hashcell;
   int linecount = 0;

   hashline = (char *)ALLOC(char, MAX_HASHLINE_LEN);
   SIGPRINT(stdout, "Loading hashtable (%s)...\n", hashfile);
   hfile = fopen_check(hashfile, "rb");
   if (hfile == NULL)
   {
      return -1;
   }
   for (; ; )
   {
      unsigned char collision;
      linecount++;

      /*********************************************************************************/
      /* first read the index                                                          */
      /*********************************************************************************/

      if (!fread(&i, sizeof(long), 1, hfile))
         break;
      hashtable[i] = (struct htable *)ALLOC(struct htable, 1);
      if (i >= HASHSIZE)
      {
         SIGPRINT(stderr, "*** ERROR, Hash Table is corrupted (line %d)\n", linecount);
         goto bad_end;
      }

      hashcell = hashtable[i];

ht_collision:

      /*********************************************************************************/
      /* size of the next ngram                                                        */
      /*********************************************************************************/

      if (!fread(&(hashcell->ngramsize), sizeof(unsigned short), 1, hfile))
         break;
      if (hashcell->ngramsize > NMAX)
      {
         SIGPRINT(stderr, "*** ERROR, hash table corrupted (line %d)\n", linecount);
         goto bad_end;
      }

      if (!fread(hashcell->code, sizeof(char), hashcell->ngramsize, hfile))
         break;
      for (k = 3; k <= hashcell->ngramsize; k++)
      {
         if (!fread(&(hashcell->count[k-3]), sizeof(BIGINT), 1, hfile))
            break;
      }

      /******************************************************************************/
      /* check if we have a collision here                                          */
      /******************************************************************************/

      if (!fread(&collision, sizeof(unsigned char), 1, hfile))
         break;
      if (collision == 0x77)
         continue;
      if (collision == 0x88)           /* 0x88 is the code of a collision              */
      {
         hashcell->nextp = (struct htable *)ALLOC(struct htable, 1);
         hashcell = hashcell->nextp;
         goto ht_collision;
      }
      else
      {
         SIGPRINT(stderr,
                  "*** ERROR !! hashtable corrupted, collision code wrong (%02X, line %d)\n",
                  collision, linecount);
         goto bad_end;
      }
   }

   printf("Done\n");
   fclose(hfile);
   free(hashline);
   return 1;

bad_end:
   fclose(hfile);
   free(hashline);
   return -1;
}

/*-------------------------------------------------------------------------------------*/
/* from mergehash.c                                                                    */

void mergehash(struct htable *hashtable[],struct htable *global_hashtable[])
{
   static struct htable *hashcell;
   int i;

   for (i = 0; i < HASHSIZE; i++)
   {
      hashcell = hashtable[i];
      if (hashcell == NULL)
         hashtable[i] = global_hashtable[i];
      else
      {
         while (hashcell->nextp)
            hashcell = hashcell->nextp;
         hashcell->nextp = global_hashtable[i];
      }
   }
   return ;
}

/*-------------------------------------------------------------------------------------*/
/* from misc_util.c                                                                    */

void freehash(struct htable *hashtable[])/* frees the hashcells, not the hastable      */
                                       /* itself totaly                                */
{
   int i;
   struct htable *hashcell;
   struct htable *hashcell2;

   if (hashtable == NULL)
      return ;
   for (i = 0; i < HASHSIZE; i++)
   {
      hashcell = hashtable[i];
      while (hashcell)
      {
         hashcell2 = hashcell->nextp;
         free(hashcell);
         hashcell = hashcell2;
      }
      hashtable[i] = NULL;
   }
}

/*-------------------------------------------------------------------------------------*/
/* from getngram.c                                                                     */

BIGUINT getngram(struct sigbyte *sbytes,int n,BIGUINT bigrams[],struct htable *hashtable[])
{
   unsigned int hash;
   int match;
   int k;
   struct htable *hashcell;

   hash = sbytes[0].hex*256 + sbytes[1].hex;
   if (n == 2)
      return (bigrams[hash]);
   else
   {
      hash = ((hash << 8)+hash+sbytes[2].hex+(n>=4?sbytes[3].hex:0))&HASHSIZE_MASK;
      if (hashtable[hash])
      {
         hashcell = hashtable[hash];
         if (0 != (match = (hashcell->ngramsize >= n)))
         {
            for (k = 0; k < n; k++)
               match = match && (hashcell->code[k] == sbytes[k].hex);
         }
         while (!match && hashcell->nextp)
         {
            hashcell = hashcell->nextp;
            if (0 != (match = (hashcell->ngramsize >= n)))
               for (k = 0; k < n; k++)
                  match = match && (hashcell->code[k] == sbytes[k].hex);
         }
         if (match)
            return (hashcell->count[n-3]);
      }
   }
   return (0);
}

/*-------------------------------------------------------------------------------------*/
/* from ngramslookup.c                                                                 */

int ngramslookup(struct htable *hashtable[],char *database_root)
{
#ifdef   NGRAMSLOOKUP_STATISTICS
   int total_lookups_count = 0;
   int total_buckets_count = 0;
   int total_items_count = 0;
   int local_items_count = 0;
   int avr_bucket_use = 0;
   int max_bucket_use = -1;
   int max_bucket_key = -1;
   void *lastngram = "\0\0\0\0\0";
   int lastsign = 0,thissign = 0;
   int inversions = 0;
#endif
   int i;
   int ngram_len;
   clock_t timing = clock();

   for (i = HASHSIZE; i--; )
   {
      struct htable *httmp;
#ifdef   NGRAMSLOOKUP_STATISTICS

      local_items_count = 0;
      if (hashtable[i])
         ++total_buckets_count;
#endif
      for (httmp = hashtable[i]; httmp != NULL; httmp = httmp->nextp)
      {
#ifdef   NGRAMSLOOKUP_STATISTICS
         ++local_items_count;
         ++total_items_count;
         thissign = memcmp(lastngram, httmp->code, 3) > 0?1:-1;
         inversions += (thissign != lastsign);
         thissign = lastsign;
         lastngram = httmp->code;
#endif
         for (ngram_len = 3; ngram_len <= httmp->ngramsize; ++ngram_len)
         {
#ifdef   NGRAMSLOOKUP_STATISTICS
            ++total_lookups_count;
#endif
            httmp->count[ngram_len-3] = (BIGINT) lookup_ngram_count(httmp->code,
                                                                    ngram_len,
                                                                    database_root);
         }
      }
#ifdef   NGRAMSLOOKUP_STATISTICS
      if (local_items_count)
      {
         avr_bucket_use += local_items_count;
         max_bucket_use = local_items_count > max_bucket_use ?
            (max_bucket_key = i, local_items_count) : max_bucket_use;
         //printf("  HT Key %04X -> %d items\n", i, local_items_count);
      }
#endif
   }
   timing = (clock()-timing)/CLOCKS_PER_SEC;
   SIGPRINT(stdout, "Lookup time (%d.%02d)\n", timing/60, timing%60);
#ifdef   NGRAMSLOOKUP_STATISTICS
   printf("Total non-empty buckets count: %d\n", total_buckets_count);
   printf("Total items (ngrams) count: %d\n", total_items_count);
   printf("Total lookups count: %d\n", total_lookups_count);
   printf("Average # of items in used buckets: %d\n",
          avr_bucket_use/total_buckets_count);
   printf("Maximum # of items in a bucket: %d (for key %04X)\n",
          max_bucket_use, max_bucket_use);
   printf("%d trigram inversions\n", inversions);
#endif
   return 0;
}

// htipp: pointer to pointer to hash-table item
int compare_trigrams(const void *htipp1,const void *htipp2)
{
   return  memcmp((*(struct htable **)htipp1)->code,
                  (*(struct htable **)htipp2)->code,
                  3);
}

// It's OK to compare 5-grams since the remaining bytes in a n-grams with n < 5 are reset anyway
int compare_5grams(const void *htipp1,const void *htipp2)
{
   return  memcmp((*(struct htable **)htipp1)->code,
                  (*(struct htable **)htipp2)->code,
                  5);
}

int ngramslookup_sorted(struct htable *hashtable[],char *database_root)
{
   int i;
   int ngram_len;
   struct htable *httmp;

#define  HTARRAY_SZ 32768
#define  HTARRAY_SZ_INCR (HTARRAY_SZ * sizeof(struct htable *))
   int htarrayindex = 0;
   int htarraysz = HTARRAY_SZ_INCR;
   struct htable **htarray = (struct htable **)malloc(htarraysz);
   clock_t timing = clock();
#ifdef   NGRAMSLOOKUP_STATISTICS
   int numreallocs = 0;
#endif
        //assert(htarray != NULL);

   if (htarray == NULL)
   {
      fprintf(stderr, "malloc failed\n");
      exit(1);
   }
   for (i = 0; i < HASHSIZE; ++i)
   {
      for (httmp = hashtable[i]; httmp != NULL; httmp = httmp->nextp)
      {
         assert(htarrayindex < HTARRAY_SZ);
         htarray[htarrayindex++] = httmp;
         if (htarrayindex >= htarraysz)
         {
            htarraysz += HTARRAY_SZ_INCR;
            htarray = (struct htable **)realloc((void *)htarray, htarraysz);
            //assert(htarray != NULL);
            if (htarray == NULL)
            {
               fprintf(stderr, "realloc failed\n");
               exit(1);
            }
#ifdef   NGRAMSLOOKUP_STATISTICS
            ++numreallocs;
#endif
         }
      }
   }
#ifdef   NGRAMSLOOKUP_STATISTICS
   printf("Sorting hash-table items array\n");
#endif
   qsort(htarray, htarrayindex, sizeof(struct htable *), compare_5grams);
#ifdef   NGRAMSLOOKUP_STATISTICS
   printf("Performing lookups in order\n");
#endif
   for (i = 0; i < htarrayindex; ++i)
   {
      httmp = htarray[i];
#ifdef   NGRAMSLOOKUP_STATISTICS
      //printf("%02X  ", httmp->code[0]);
#endif
      for (ngram_len = 3; ngram_len <= httmp->ngramsize; ++ngram_len)
      {
         httmp->count[ngram_len-3] = (BIGINT) lookup_ngram_count(httmp->code,
                                                                 ngram_len,
                                                                 database_root);
      }
   }
   free(htarray);
#ifdef   NGRAMSLOOKUP_STATISTICS
   //printf("\n");
   printf("There were %d reallocations\n", numreallocs);
#endif
   timing = (clock()-timing)/CLOCKS_PER_SEC;
   SIGPRINT(stdout, "Lookup time (%d.%02d)\n", timing/60, timing%60);
   return 0;
}

/*-------------------------------------------------------------------------------------*/
/* from compute_scores.c                                                               */

double *compute_scores(struct sigbyte virseg[],
                       int seglength,
                       int siglength,
                       double *unigrams,
                       BIGUINT *bigrams,
                       struct htable *hashtable[],
                       BIGUINT *ngramcounts)
{
   int i,j,k,n;

   /************************************************************************************/
   /* riad: ~4 or 8 megs of data in the stack, this is madness... and most systems     */
   /* would say : STACK OVERFLOW (may be not unix or aix)                              */
   /************************************************************************************/

   double(*ngram)[NMAX];
   /* riad: +15 to fix memory corruption              */
   int fixedwildcard[MAX_MISMATCHES+2+15];
   int validsig;
   double prob;
   int fixedmismatches;
   double *score;
   // ALLOC uses calloc, which initializes the array.

   ngram = ALLOC(double, NMAX *MAXSAMPLEFILELEN);
   memset(fixedwildcard, 0, MAX_MISMATCHES+2);

   /************************************************************************************/
   /* Get unigram log-probabilities                                                    */
   /************************************************************************************/

   for (i = 0; i < seglength; i++)
   {
      if (virseg[i].qvalid == 1)
         ngram[i][0] = unigrams[virseg[i].hex];
   }

   /************************************************************************************/
   /* Calculate logs of probabilities for ngrams, n>1                                  */
   /************************************************************************************/

   for (n = 2; n <= NMAX; n++)
   {
      for (i = 0; i < (seglength-n+1); i++)
      {
         for (k = 0, validsig = 1; (k < n) && (virseg[i+k].qvalid == 1); k++)
         {
            ;
         }
         if (k < n)
            validsig = 0;
         if (validsig)
         {
            prob = (double)((BIGINT)getngram(&(virseg[i]), n, bigrams, hashtable));
            if (prob != 0.0)
               ngram[i][n-1] = log(prob)-log((double)((BIGINT)(ngramcounts[n-1])));
            else
            {
               ngram[i][n-1] = ngram[i][n-2]+ngram[i+1][n-2];
               if (n > 2)
                  ngram[i][n-1] -= ngram[i+1][n-3];
            }
         }
         else
         {
            ngram[i][n-1] = 0.0;
         }
      }
   }

   /************************************************************************************/
   /* Calculate the score (= estimated log of probability) of each string of length    */
   /* siglength in virus segment.                                                      */
   /************************************************************************************/

   score = (double *)ALLOC(double, (seglength-siglength+2));
   for (j = 0; j < (seglength-siglength+1); j++)
   {
      for (k = 0, fixedmismatches = 0; k < siglength; k++)
      {
         if ((fixedmismatches+1) >= (MAX_MISMATCHES+2))
         {
            SIGPRINT(stderr,
                     "*   CORRUPTION (memory) in compute_scores.c (fixedmismatches=%d)\n",
                     fixedmismatches);
         }
         if (virseg[j+k].qvalid == 0)
            fixedwildcard[++fixedmismatches] = j+k;
      }
      fixedwildcard[0] = j-1;
      fixedwildcard[fixedmismatches+1] = j+siglength;
      if (fixedmismatches > (MAX_MISMATCHES-2))
         score[j] = 0.0;
      else
         score[j] = mismatchscore(fixedwildcard, fixedmismatches, ngram);
   }
   free(ngram);
   //free(fixedwildcard);
   return (score);
}

/*-------------------------------------------------------------------------------------*/
