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
      int remain = (size + shift - 32) & 0x1f; /* remaining bytes at the end after the alignement */
      int tbl_size = (size + shift - 32) >> 5;
      memwatch[tbl_addr] |= 0xffffffff << shift;
      memwatch[tbl_addr + 1 + tbl_size] |= 0xffffffff >> (32 - remain);
      memset(&memwatch[tbl_addr + 1], 0xff, tbl_size);
   }


   /* create a new entry in memwatch_descr */
   if(memwatch_descr_count >= MEMWATCH_DESCR_SIZE)
      PANIC("watch_memory.c - memwatch descriptor array is full\n");
   // TO DO: find a way to tell asax without crashing
   else
   {
      memwatch_descr[memwatch_descr_count].start = addr;
      memwatch_descr[memwatch_descr_count].size = size;
      memwatch_descr[memwatch_descr_count++].marker = marker;
   }
}


__inline void dismiss_guard(DWORD addr) // TO DO: if conflict checking, include marker
{
   DWORD aux_count, shift, tbl_addr;
   int size = 0;
   
   /* remove the entry in memwatch_descr */
   for(aux_count = 0; aux_count < memwatch_descr_count; aux_count++)
   {
      if(memwatch_descr[aux_count].start == addr)
      {
         size = memwatch_descr[aux_count].size;
         memwatch_descr[--memwatch_descr_count].start = memwatch_descr[aux_count].start;
         memwatch_descr[memwatch_descr_count].size = memwatch_descr[aux_count].size;
         memwatch_descr[memwatch_descr_count].marker = memwatch_descr[aux_count].marker;
         break;
      }
   }
   if(!size)
   {
      /* That means there was no such entry in memwatch_descr. You could uncomment the following
         for debugging purposes, otherwise simply return. */
      // PANIC("watch_memory.c - could not find memwatch descriptor to be removed!");
      return;
   }
   
   /* clean the bits in memwatch array */
   tbl_addr = addr >> 5; /* we have an array of DWORDS, one bit for each byte in memory */
   if((shift = addr & 0x1f) + size <= 0x20)
      memwatch[tbl_addr] &= _rotl((0xffffffff << size), shift);
   else
   {
      int remain = (size + shift - 32) & 0x1f; /* remaining bytes at the end after the alignement */
      int tbl_size = (size + shift - 32) >> 5;
      memwatch[tbl_addr] &= 0xffffffff >> (32 - shift);
      memwatch[tbl_addr + 1 + tbl_size] &= 0xffffffff << remain;
      memset(&memwatch[tbl_addr + 1], 0xff, tbl_size);
   }
}


__inline int memarea_is_guarded(unsigned int len, unsigned int shift, unsigned int tbl_addr)
{

   DWORD remain = (len + shift - 32) & 0x1f;
   DWORD tbl_len = (len + shift - 32) >> 5;
   return((memwatch[tbl_addr] & 0xffffffff << shift)
      && (memwatch[tbl_addr + 1 + tbl_len] & 0xffffffff >> (32 - remain))
      && memwatch_scan((DWORD) &memwatch[tbl_addr + 1], tbl_len));
}

__inline int memwatch_scan(DWORD index, DWORD tbl_len)
{
	// Do that in C...

	_asm
	{
	push es
	push ds
	pop es
	xor eax,eax
	cld
	mov ecx, tbl_len
	mov edi, index
	repne scasd
	mov eax, ecx /* ecx is 0 if the whole area is 0 */
	pop es
	}
}

__inline int note_memaccess(unsigned int addr, unsigned int len, unsigned int rw, void *data)
{
   char is_write = (rw == BX_WRITE);
   DWORD thedata=0;

   report_tag = MEM_ACCESS;
   nadf_record[41].object = NULL;
   nadf_record[42].object = &addr;
   nadf_record[43].object = &len;
   nadf_record[44].object = (unsigned char *) &is_write;
   if (len<5) memcpy(&thedata,data,len); // Do not simply give the ptr data because it may be the end of an array
										 // that should not be cast to a DWORD !!!
   nadf_record[32].object = &thedata;

   submit_record(nadf_record);
   return(cancel_instruction);
}
