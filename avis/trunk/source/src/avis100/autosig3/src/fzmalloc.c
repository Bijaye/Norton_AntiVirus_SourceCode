/*

  Source code for fzmalloc low-overhead malloc routines.
  See fzmalloc.pro for useage and pro/con stuff.

  92/09/03 -- Written, DC
  92/09/03 (_after_ dawn) -- Bill suggests getting rid of the
     setup routine and its required estimate, and just using
     a smaller chunksize.  OK.
  92/09/03 (even later) -- Add fzstrdup(), BA
  92/09/11 -- Add slots, so various routines can all use fz stuff
     at once, albeit awkwardly.  Also assert()s to catch abuses.  DC
  92/09/15 -- Add oustanding_for_chunk, to catch more abuses, and to
     free up a chunk when it's empty.  Also automatically shut down a
     slot when it's empty (no more outstanding allocs).  So now fzfree()
     sometimes frees up some memory.

  92/10/12 -- Temporarily took out assert()s for first ship of IBMAVD
                                                                                       */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "fzmalloc.h"

/* The number of different slots supported, and an initializer for index.              */

#define  SLOTS         3
#define  INDEX_INIT    {                                                              \
                       -1 , -1 , -1 }

/* The granularity of the underlying malloc()s that are done.  Making                  */
/* this larger reduces the number of real malloc()s done, and increases                */
/* the expected waste in the last chunk.  For Borland C++, this should                 */
/* be four less than a multiple of 16.                                                 */

#define  CHUNKMAX      (8192-64)

/* The number of outstanding fzmallocs, for error checking                             */

static unsigned long outstanding_for_slot[SLOTS];

/* The number of the chunk we're using now, -1 for unsetup.                            */

static int index[SLOTS] = INDEX_INIT;

/* How much of the one we're now using we've used                                      */

static unsigned int count[SLOTS];

/* A slot is an array of these; a buffer pointer and an outstanding count              */

typedef struct
{
   char *databuf;
   int outstanding_for_chunk;
} slot_element;

/* Array of pointers to the chunks (constantly realloc()ed)                            */

static slot_element *array[SLOTS];

/* How big this chunk is; always CHUNKMAX in this imp.                                 */

#define  chunksize(x)  CHUNKMAX

/* How much slop to tolerate at the end of a chunk.  Lowering this                     */
/* reduces waste from unused chunk-ends, but can increase the number                   */
/* of times fzmalloc() just does a direct malloc().                                    */

#define  MAXSLOP       1024

/** Start up the system, and allocate the first chunk for this slot.  *                */
/** Called by the first fzmalloc() on a slot.                         *                */

static int fzmalloc_setup(unsigned int slot)
{
   if (slot >= SLOTS)
      return 1;

   /************************************************************************************/
   /* Get the array to hold the chunkpointers; starts at size one                      */
   /************************************************************************************/

   array[slot] = (slot_element *)malloc(sizeof(slot_element));
   if (array[slot] == NULL)
      return 1;

   /************************************************************************************/
   /* Get the actual data space                                                        */
   /************************************************************************************/

   array[slot][0].databuf = (char *)malloc(chunksize(0));
   array[slot][0].outstanding_for_chunk = 0;
   if (array[slot][0].databuf == NULL)
      return 1;
   index[slot] = 0;                    /* We're using the first one                    */
   count[slot] = 0;                    /* Haven't used any of it yet                   */
   outstanding_for_slot[slot] = 0L;    /* None outstanding                             */
   return 0;
}                                      /* end fzmalloc_setup()                         */

/** Just like malloc.  Allocates some memory, generally with zero bytes *              */
/** of overhead, but sometimes just does a malloc().                    *              */

void *fzmalloc(size_t size,unsigned int slot)
{
   char *answer;

   if (slot >= SLOTS)
      return  malloc(size);

   if (index[slot] == -1)
   {                                   /* Set up if we haven't                         */
      if (fzmalloc_setup(slot))
         return  malloc(size);
   }

   /************************************************************************************/
   /* Remember we've seen one more for this slot                                       */
   /************************************************************************************/

   outstanding_for_slot[slot]++;

   /************************************************************************************/
   /* Bail out if we ran out of memory for the arrays                                  */
   /************************************************************************************/

   if (array[slot] == NULL)
      return  malloc(size);

   /************************************************************************************/
   /* Will it not fit in the current chunk?                                            */
   /************************************************************************************/

   if ((count[slot]+size) > chunksize(index))
   {
      if (size > MAXSLOP)
         return  malloc(size);         /* No big ones at borders                       */

      index[slot]++;

      /*********************************************************************************/
      /* Get a new chunk                                                               */
      /*********************************************************************************/

      array[slot] = realloc(array[slot], (index[slot]+1)*sizeof(slot_element));
      if (array[slot] == NULL)
         return  malloc(size);

      array[slot][index[slot]].databuf = (char *)malloc(chunksize(index[slot]));
      array[slot][index[slot]].outstanding_for_chunk = 0;
      count[slot] = 0;
   }
   array[slot][index[slot]].outstanding_for_chunk++;/* One more alloc                  */
   answer = array[slot][index[slot]].databuf+count[slot];
   count[slot] += size;
   return (void *)answer;
}                                      /* end fzmalloc                                 */
#if      0

/** Undo an fzmalloc().  Often does nothing, but sometimes frees a     *               */
/** pointer that fzmalloc() malloc()ed, sometimes frees a chunk if     *               */
/** it's now empty, and sometimes shuts down the whole slot.           *               */

void fzfree(void *p,unsigned int slot)
{
   int i,inchunk;

   /************************************************************************************/
   /* If some furrin slot, just free() it                                              */
   /************************************************************************************/

   if (slot >= SLOTS)
   {
      free(p);
      return ;
   }

   /************************************************************************************/
   /* One fewer outstanding assert(outstanding_for_slot[slot]>0);                      */
   /************************************************************************************/

   outstanding_for_slot[slot]--;

   /************************************************************************************/
   /* Hideously non-portable, but I don't know a portable way to ask if p is within the*/
   /* b-byte block pointed to by q. Anyway, if p is within any of our chunks, so note. */
   /************************************************************************************/

   inchunk = 0;
   for (i = 0; i <= index[slot]; i++)
   {
      if ((FP_SEG(p) == FP_SEG(array[slot][i].databuf)) &&
          (FP_OFF(p) < (FP_OFF(array[slot][i].databuf)+chunksize(i))))
      {

         /******************************************************************************/
         /* assert(array[slot][i].outstanding_for_chunk>0);                            */
         /******************************************************************************/

         if (0 == --array[slot][i].outstanding_for_chunk)
         {                             /* Chunk empty now                              */
            free(array[slot][i].databuf);
            array[slot][i].databuf = NULL;
         }
         inchunk = 1;
         break;                        /* Out of the for loop                          */
      }                                /* endif                                        */
   }                                   /* end for                                      */

   /************************************************************************************/
   /* If it's not in any of our chunks, it must have been malloc()ed, so free it.      */
   /************************************************************************************/

   if (!inchunk)
      free(p);

   /************************************************************************************/
   /* If there are no more outstanding allocs for this slot, take it upon ourselves to */
   /* shut it down.                                                                    */
   /************************************************************************************/

   if (outstanding_for_slot[slot] == 0L)
      fzmalloc_done(slot);
}                                      /* end fzfree                                   */
#endif

/** All done, free all chunks (the user is responsible for having fzfree()d *          */
/** all fzmalloc()ed pointers before doing this call.                       *          */

int fzmalloc_done(unsigned int slot)
{
   int i;

   if (slot >= SLOTS)
      return 1;
   if (index[slot] == -1)
      return 1;

   /************************************************************************************/
   /* Die if any still outstanding assert(outstanding_for_slot[slot]==0L);             */
   /************************************************************************************/

   for (i = 0; i <= index[slot]; i++)
   {
      if (array[slot][i].databuf)
         free(array[slot][i].databuf);
      array[slot][i].databuf = NULL;
   }
   free(array[slot]);
   array[slot] = NULL;
   index[slot] = -1;
   return 0;
}                                      /* end fzmalloc_done                            */

/** Replacement for strdup.                                                 *          */

char *fzstrdup(char *srcstr,unsigned int slot)
{
   char *tmpstr;

   if (slot >= SLOTS)
      return  strdup(srcstr);

   if ((tmpstr = fzmalloc(strlen(srcstr)+1, slot)) != NULL)
      strcpy(tmpstr, srcstr);
   return  tmpstr;
}
