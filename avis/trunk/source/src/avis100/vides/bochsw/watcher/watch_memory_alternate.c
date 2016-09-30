#include "watcher.h"

__inline void set_guard(DWORD addr, WORD size, WORD marker) /* requ: size > 0 */
// maybe TO DO: check for conflicts
{
   int shift;
   int tbl_addr = addr >> 5; /* we have an array of DWORDS, one bit for each byte in memory */
   if((shift = addr & 0x1f) + size <= 0x20)
      memwatch[tbl_addr] |= (0xffffffff >> (32 - size)) << shift;
   else
   {
      int remain = (size - shift) & 0x1f; /* remaining bytes at the end after the alignement */
      int tbl_size = (size - shift) >> 5;
      memwatch[tbl_addr] |= 0xffffffff << shift;
      memwatch[tbl_addr + 1 + tbl_size] |= 0xffffffff >> (32 - remain);
      memset(&memwatch[tbl_addr + 1], 0xff, tbl_size);
   }
   /* create a new entry in memwatch_descr */
   add_memwatch_descr(addr, size, marker);
}

__inline void add_memwatch_descr(DWORD start, WORD size, WORD marker)
{
   int aux_count;
   if((aux_count = memwatch_descr_count - MEMWATCH_DESCR_SIZE) < 0)
   { /* the first array is not full, place the descr here */
      memwatch_descr[memwatch_descr_count].start = start;
      memwatch_descr[memwatch_descr_count].size = size;
      memwatch_descr[memwatch_descr_count++].marker = marker;
   }
   else
   { /* look in the extra array */
      Memwatch_extra_descr_array_t *extra_array = memwatch_extra_descr_array; /* keep track of the extra array */
      Memwatch_extra_descr_array_t **extra_array_link = &memwatch_extra_descr_array; /* and what points to it */
      while(extra_array) /* as long as there is one */
      {
         if(aux_count < extra_array->size)
         { /* this extra array is not full, place the descr here */
            extra_array->array[aux_count].start = start;
            extra_array->array[aux_count].size = size;
            extra_array->array[aux_count].marker = marker;
            memwatch_descr_count++;
            return;
         }
         else
         { /* this extra array is full, go to the next */
            aux_count = aux_count - extra_array->size;
            extra_array_link = &(extra_array->next_extra);
            extra_array = extra_array->next_array;
         }
      }
      /* all arrays are full, create a new extra array and a structure */
      Memwatch_descr_t new_array;
      if(extra_array = malloc(sizeof Memwatch_extra_descr_array_t) && new_array = malloc(EXTRA_DESCR_ARRAY_SIZE * 8))
      {
         *extra_array_link = extra_array; /* set the link */
         extra_array->array = new_array; /* set the structure describing the new array */
         extra_array->next_extra = NULL;
         extra_array->size = EXTRA_DESCR_ARRAY_SIZE;
         new_array->start = start; /* place our descr as the first element of this new array */
         new_array->size = size;
         new_array->marker = marker;
         memwatch_descr_count++;
      }
      else
         PANIC("watcher.c - out of memory\n");
   }
}

__inline void dismiss_guard(DWORD addr) // TO DO: if conflict checking, include marker
{
   int shift;
   /* remove the entry in memwatch_descr */
   int size = remove_memwatch_descr(addr);

   int tbl_addr = addr >> 5; /* we have an array of DWORDS, one bit for each byte in memory */
   if((shift = addr & 0x1f) + size <= 0x20)
      memwatch[tbl_addr] &= _rotl((0xffffffff << size), shift);
   else
   {
      int remain = (size - shift) & 0x1f; /* remaining bytes at the end after the alignement */
      int tbl_size = (size - shift) >> 5;
      memwatch[tbl_addr] &= 0xffffffff >> (32 - shift);
      memwatch[tbl_addr + 1 + tbl_size] &= 0xffffffff << remain;
      memset(&memwatch[tbl_addr + 1], 0xff, tbl_size);
   }
   break;
}

__inline int remove_memwatch_descr(DWORD start) /* returns the size */
{
   int aux_count;
   for(aux_count = 0; aux_count < MEMWATCH_DESCR_SIZE; aux_count++)
   {
      if(memwatch_descr[aux_count].start == start)
         ;
      
      if((aux_count = memwatch_descr_count - MEMWATCH_DESCR_SIZE) < 0)
   { /* the first array is not full, place the descr here */
      memwatch_descr[memwatch_descr_count].start = start;
      memwatch_descr[memwatch_descr_count].size = size;
      memwatch_descr[memwatch_descr_count++].marker = marker;
   }
   else
   { /* look in the extra array */
      Memwatch_extra_descr_array_t *extra_array = memwatch_extra_descr_array; /* keep track of the extra array */
      Memwatch_extra_descr_array_t **extra_array_link = &memwatch_extra_descr_array; /* and what points to it */
      while(extra_array) /* as long as there is one */
      {
         if(aux_count < extra_array->size)
         { /* this extra array is not full, place the descr here */
            extra_array->array[aux_count].start = start;
            extra_array->array[aux_count].size = size;
            extra_array->array[aux_count].marker = marker;
            memwatch_descr_count++;
            return;
         }
         else
         { /* this extra array is full, go to the next */
            aux_count = aux_count - extra_array->size;
            extra_array_link = &(extra_array->next_extra);
            extra_array = extra_array->next_array;
         }
      }
      /* all arrays are full, create a new extra array and a structure */
      Memwatch_descr_t new_array;
      if(extra_array = malloc(sizeof Memwatch_extra_descr_array_t) && new_array = malloc(EXTRA_DESCR_ARRAY_SIZE * 8))
      {
         *extra_array_link = extra_array; /* set the link */
         extra_array->array = new_array; /* set the structure describing the new array */
         extra_array->next_extra = NULL;
         extra_array->size = EXTRA_DESCR_ARRAY_SIZE;
         new_array->start = start; /* place our descr as the first element of this new array */
         new_array->size = size;
         new_array->marker = marker;
         memwatch_descr_count++;
      }
      else
         PANIC("watcher.c - out of memory\n");
   }

