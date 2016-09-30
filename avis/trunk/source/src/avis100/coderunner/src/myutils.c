#ifdef Cr_MOREFEATURES2 //riad//
   #ifndef CR_MOREFEATURES
   #define CR_MOREFEATURES
   #endif
#endif

#ifndef CR_DO_SCANNER   //riad//
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <time.h>
   #include <signal.h>
   #ifdef __UNIX__
      #include <dirent.h>
      #include <sys/types.h>
      #include <sys/stat.h>
      #include <unistd.h>
      #define _stat stat
      #ifndef _S_IFDIR
         #define _S_IFDIR S_IFDIR
      #endif
      #define  DIREC DIR *
   #else  /* WIN32 */
      #include <io.h>
      #include <direct.h>
      #define  DIREC long
   #endif /* WIN32 */
#endif

/*#include "common.h"*/
#include "cr_all.h"

extern FILE *mydebug3;

//#define BACKOFFSET   2  /* size of a word */

///////////////////////////////////////////////////////////////////////////////////

#ifdef CR_MOREFEATURES  //riad//

MINMAX *is_in_exe_section(ULONG current)
{
   int      i = 0;
   register MINMAX   *cur = &_MinMaxTable.minmax;;

   while(cur != NULL)
   {
      if(cur->present != -1)
         if(current >= cur->min && current <= cur->max) return cur;
      cur = cur->next;
   }
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////////

// Fred: This was originally included in get_new_exe_section
// I changed that because the final call to get_new_exe_section is really just a way of packing the sections
// (that is, merging the close sections together)
// This call allocated an unneeded extra section that caused an error in the final report ("Wrong MinMax Section")

void pack_sections (void)
{
   MINMAX   *other, *cur;
   int      backoffset = BACKOFFSET;

   // let's merge close sections
   for(cur = &_MinMaxTable.minmax; cur != NULL; cur = cur->next)
   {
      int j;
      if(cur->present == -1) continue;

      for(j=1;j<=backoffset;j++)
         if((other=is_in_exe_section((ULONG)(cur->min-j))) != NULL)
         {
            if(cur != other)
            {
               cur->min = __min(cur->min, other->min);
               cur->max = __max(cur->max, other->max);
               other->present = -1;
               _MinMaxTable.nsections--;
               break;
            }
         }

      //if(other && other->present == -1) continue;
      for(j=1;j<=backoffset;j++)
         if((other=is_in_exe_section((ULONG)(cur->max+j))) != NULL)
         {
            if(cur != other)
            {
               cur->min = __min(cur->min, other->min);
               cur->max = __max(cur->max, other->max);
               other->present = -1;
               _MinMaxTable.nsections--;
               break;
            }
         }
   }
}

///////////////////////////////////////////////////////////////////////////////////

// this routine isn't called often, not time consuming
// so it's good to merge close sections if we are running out
// of them...

MINMAX *get_new_exe_section(void)
{
   MINMAX   *other, *cur;

   pack_sections(); // Merge any two sections close to one another

   cur = &_MinMaxTable.minmax;
   do
   {
      other = cur;
      if(cur->present == -1) return cur;
      cur = cur->next;
   } while(cur != NULL);

   // so let's then allocate a new section damned
   // cur is the pointer to the last section

   // Let's do some few initialisations in case you know

   cur         = other;
   other       = allocate(MINMAX);
   if(other == NULL) return NULL; /* not enough space */
   other->next = NULL;
   cur->next   = other;
   return other;
}

///////////////////////////////////////////////////////////////////////////////////
#ifdef CR_MOREFEATURES2 //riad//

MINMAX *is_in_active_section(ULONG current)
{
   register MINMAX   *curr = _MinMaxTable.ulPrevMemoryMods;

   while(curr != NULL)
   {
      if(curr->present != -1)
         if(current >= curr->min && current <= curr->max) return curr;
      curr = curr->next;
   }
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////////

// this routine isn't called often as well. Not time consuming
// so it's good to do extra checks here.

MINMAX *get_new_active_section(ULONG MMLA)
{
   int      backoffset = 4;   /* size of a DWORD */
   MINMAX   *other, *cur;

   // let's merge close sections first

   cur = _MinMaxTable.ulPrevMemoryMods;

   while(cur != NULL)
   {
      int j;

      if(cur->present != -1)
      {
         for(j = 0; j <= backoffset; j++)
            if((other=is_in_active_section((ULONG)(cur->min-(ULONG)j))) != NULL)
            {
               if(cur != other)
               {
                  other->min = __min(cur->min, other->min);
                  other->max = __max(cur->max, other->max);
                  _MinMaxTable.ulsections--;
                  cur->present = -1;
                  break;   /* break from for() */
               }
            }
      }
      if(cur->present != -1)
      {
         for(j = 0; j <= backoffset; j++)
            if((other=is_in_active_section((ULONG)(cur->max+(ULONG)j))) != NULL)
            {
               if(cur != other)
               {
                  other->min = __min(cur->min, other->min);
                  other->max = __max(cur->max, other->max);
                  _MinMaxTable.ulsections--;
                  cur->present = -1;
                  break;   /* break from for() */
               }
            }
      }
      cur = cur->next;
   }

   // Now, let's check (again?) for vacant sections
   cur = _MinMaxTable.ulPrevMemoryMods;

   while(cur != NULL)
   {
      other = cur;
      if(cur->present == -1) return cur;
      cur = cur->next;
   }

   // If we got too many sections already, send the last one !
   if(other != NULL && (_MinMaxTable.ulsections+1) > MINMAXACTIVE) 
      return other;

   if(backoffset < BACKOFFSET)
   {
   #ifndef CR_DO_SCANNER
   #ifdef MYDEBUG
      fprintf(stderr, "(****) UpdateModificationRecords() failed. (%lX)\n", MMLA);
   #endif
      _nupdate_modif++;
   #endif
      backoffset = BACKOFFSET;
      //goto do_get2_again;
   }

   // For the moment, let's take the smallest section as victim
   //for(i= 0; i < MINMAXACTIVE; i++)
   //{
   //   ULONG diff = _ulPrevMemoryMods[i].min-_ulPrevMemoryMods[i].max;
   //
   //   if(diff <= 1L) return i;
   //   if((_ulPrevMemoryMods[found].min-_ulPrevMemoryMods[found].max) > diff)
   //      found = i;
   //}
   // Extreme situation
   //return (found);

   // so let's then allocate a new section damned
   // cur is the pointer to the last section

   // Let's do some few initialisations in case you know

   cur = other;
   other = allocate(MINMAX);

   if(other == NULL) return NULL; /* this is frightfull */

   other->next = NULL;

   if(_MinMaxTable.ulPrevMemoryMods != NULL) cur->next = other;
   else _MinMaxTable.ulPrevMemoryMods = other;

   return other;
}

#endif   /* CR_MOREFEATURES2 */

#endif   /* CR_MOREFEATURES */

///////////////////////////////////////////////////////////////////////////////////

jmp_stuff *get_jmp_in_list(ULONG address)
{
   jmp_stuff *other = NULL, *cur = _MinMaxTable.jmpstuff;

   while(cur != NULL)
   {
      other = cur;
      if(cur->address == address) return cur;
      cur = cur->next;
   }

   // Well, we have a new address, create new section 
   cur               = allocate(jmp_stuff);
   if(cur == NULL)   return NULL; /* no memory, no fun... */
   cur->next         = NULL;  // this has to be initialized here
   cur->jumpcond     = -1;    // kif kif
   cur->counterup    = 0;
   cur->counterdown  = 0;
   cur->banned       = 0;
   //cur->bannedown    = 0;
   cur->number       = ++(_MinMaxTable.njumps);
   cur->j_IdleLoops   = 0;
   if(other == NULL) _MinMaxTable.jmpstuff = cur;

   // this is also important
   else other->next  = cur;

   return cur;
}

///////////////////////////////////////////////////////////////////////////////////

machine_stuff *update_machine_state(MACHINE_STATE  *pMachState, 
                                    RETURN_DATA    *pRetData,
                                    signed char    offset,
                                    WORD           TempIP)
{
   machine_stuff *cur = _MinMaxTable.curmstuff, *temp;
   MEMORY_AREA *malist, *newlist;
   int firstime = 1;

   // allocate the machine state first, check if error
   temp = allocate(machine_stuff);
   if(temp == NULL) return NULL;
   if(cur == NULL) _MinMaxTable.curmstuff = _MinMaxTable.mstuff = temp;

   // now, if it's not the first time, it means we created this
   // machine state because we encountered a conditionnal JUMP
   if(pMachState && pRetData)
   {
      // quickly initialize elements of the new structure

      memcpy(&(temp->ms), pMachState, sizeof(MACHINE_STATE));
      memcpy(&(temp->rd), pRetData, sizeof(RETURN_DATA));
      memcpy(temp->PQBuff, pMachState->pPQBuff, sizeof(temp->PQBuff));

      // now allocate "malist" buffers, and update prefetch queue

      malist = pMachState->pMemA;
      newlist = NULL;

      // allocate malist list one by one

      while(malist != NULL)
      {
         BYTE *bubuff;   // int and not char  or BYTE !!
         MEMORY_AREA *loc;

         // first, allocate memory for the stucture MEMORY_AREA
         loc = allocate(MEMORY_AREA);
         if(loc == NULL) return NULL;
         memcpy(loc, malist, sizeof(MEMORY_AREA));

         // but unlike a static table, the buffer inside is FREEABLE
         // commented, because even non-freable are freed in FreeAllMemroyAreas()
         loc->freeable = TRUE;

         // then allocate memory for the data buffer
         bubuff = (BYTE *)malloc(malist->BuffSize);
         if(bubuff == NULL) return NULL;
         memcpy(bubuff, malist->pBuff, malist->BuffSize);

         // if first pass in this loop, initialize the first pointer of the list
         if(firstime) { firstime = 0; temp->ms.pMemA = loc; }

         // initialize the buffer pointer
         loc->pBuff = bubuff;

         // link the previous one with the the current one
         if(newlist) newlist->pNextArea = loc;

         newlist = loc;

         // now get the _IVT and _CR_PSP of this state machine
         //if(malist->pBuff == _IVT)     temp->_IVT     = loc->pBuff;
         //if(malist->pBuff == _CR_PSP)  temp->_CR_PSP  = loc->pBuff;

         // now go to the next one
         malist = malist->pNextArea;
      }

      if(newlist) newlist->pNextArea = NULL;

      // paranoia check
      if(&(cur->ms) != pMachState || &(cur->rd) != pRetData)
      {
         fprintf(stderr, "\n\n\tDamned, an illegal error happened in update_machine()"
                         ", time to fix bugs !\n\n");
         return NULL;
      }

      if(_MinMaxTable.dojump == MODEJUMP)
      {
         // in this case, we save the other case where there is no JUMP
         // as we return temp, 
         // in principle, it should be TempIP += offset;
         // but we don't do that, so rIP = TempIP
#ifdef NEW1
         temp->ms.Regs.EIP.WORDREG.RX = TempIP;
#else
         temp->ms.Regs.wordregs.RegIP = TempIP;
#endif   /* NEW1 */
      }
      else
      {
         // in this case, we save the other case where there was a JUMP
#ifdef NEW1
         temp->ms.Regs.EIP.WORDREG.RX = TempIP+offset;
#else
         temp->ms.Regs.wordregs.RegIP = TempIP+offset;
#endif   /* NEW1 */
      }

      // save some global variables
      temp->m_ActiveInstructions  = _ActiveInstructions;
      temp->m_IdleInstructions    = _IdleInstructions;
      temp->m_IdleLoops           = _IdleLoops;
      temp->m_IdleLoopLinearAddr  = _IdleLoopLinearAddr;
      temp->NumOfDiffInst        = _MinMaxTable.NumOfDiffInst;
      temp->NumOfRealActiveInst  = _MinMaxTable.NumOfRealActiveInst;
      temp->NumOfRealIdleInst    = _MinMaxTable.NumOfRealIdleInst;
      temp->NumOfDiffLoops       = _MinMaxTable.NumOfDiffLoops;

      temp->ms._CR_PSP     = (temp->ms.pMemA)->pNextArea->pBuff;
      temp->ms._IVT        = (temp->ms.pMemA)->pNextArea->pNextArea->pBuff;
      temp->ms._CR_BIOSDA  = (temp->ms.pMemA)->pNextArea->pNextArea->pNextArea->pBuff;
   }

   temp->ms.pPQBuff       = temp->PQBuff;
   // This was a dangerous attempt to do nothing
   temp->ms.pOp1          = NULL;
   temp->ms.pOp2          = NULL;
   temp->ms.pOp3          = NULL;
   temp->ms.pOp4          = NULL;
   temp->ms.pOp5          = NULL;
   temp->ms.pPrevIFetchMA = 0l;
   temp->ms.pPrevDFetchMA = 0l;
   temp->number           = ++(_MinMaxTable.nmachines);

   // now update "next" and "previous" pointer in the list and "curmstuff"

   temp->curjmp = _MinMaxTable.curjmp;

   if(!pMachState || !pRetData)
   {
      temp->previous          = NULL;
      temp->next              = NULL;
      _MinMaxTable.mstuff     = temp;
      _MinMaxTable.curmstuff  = temp;

      // when first machine state, return the new allocated one
      return temp;
   }
   else
   {
      // some debug stuff to be aware of the debug stuff and stuff
      fprintf(mydebug3, "--> [%u->%u] New machine State [jump %u] at [%lX %04X:%04X] at op=%lu\n", 
         _MinMaxTable.curmstuff->number, temp->number, (_MinMaxTable.curjmp) -> number,
         (_MinMaxTable.curjmp) -> address, rCS, rIP, pRetData->ops);
      fflush(mydebug3);

      temp->previous          = _MinMaxTable.curmstuff->previous; /* or cur->previous */
      temp->next              = _MinMaxTable.curmstuff;           /* or cur */
      cur->previous           = temp;

      // update _MinMaxTable.mstuff (the top of the linked list)
      if(_MinMaxTable.mstuff == cur) _MinMaxTable.mstuff = temp;

      temp->ms.ExecutionBroken= TRUE;  // very important !!

      // when not new machine state, return the current running one instead
      return cur;
   }

   return  NULL;
}

///////////////////////////////////////////////////////////////////////////////////

int free_machine_state(machine_stuff *momo)
{
   //jmp_stuff   *cucur = momo->curjmp;

   //if(cucur) cucur->jumpcond = 2;

   if(momo != NULL && momo->ms.pMemA != NULL) 
   FreeAllMemoryAreas (&(momo->ms.pMemA));
   free(momo);

   return 1;
}

///////////////////////////////////////////////////////////////////////////////////

int free_almost_evrything(void)
{
   MINMAX    *temp,  *cur  = &_MinMaxTable.minmax;
   jmp_stuff *temp2, *cur2 = _MinMaxTable.jmpstuff;   

   cur = cur->next;  // don't free the first one !
   while(cur != NULL)
   {
      temp = cur;
      cur = cur->next;
      free(temp);
   }
   _MinMaxTable.minmax.next = NULL;

   cur = _MinMaxTable.ulPrevMemoryMods;
   while(cur != NULL)
   {
      temp = cur;
      cur = cur->next;
      free(temp);
   }
   _MinMaxTable.ulPrevMemoryMods = NULL;

   while(cur2 != NULL)
   {
      temp2 = cur2;
      cur2 = cur2->next;
      free(temp2);
   }
   _MinMaxTable.jmpstuff = NULL;

   return 1;
}

///////////////////////////////////////////////////////////////////////////////////

int save_hex_regions(char *filename, char *locbuf, int passes)
{
   // Ok, using _MinMAxTable.minmax and ulPrevModificationRecords
   // and _MinMaxtable.AbsoluteOffset, we can determine the hex values
   // of each byte of the executed regions specifiying whether they belong
   // to a Self Modified region or no
   
   FILE     *hexout, *hexin, *sections;
   MINMAX   *inminmax; 
   ULONG    absoff = _MinMaxTable.AbsoluteOffset;
   ULONG    linoff = _MinMaxTable.LinearOffsetOfEntry;

   int      state  = 2;    // = 0 if no modified encountered
                           // = 1 if so
                           // = 2 just for initialization


   if(_MinMaxTable.AbsoluteOffset == (ULONG)(-1)) 
   {
      fprintf(stderr, "\tError, AbsoluteOffset = -1\n");
      return -1;
   }


   if((hexin=fopen(filename, "rb")) == NULL)
   {
      fprintf(stderr, "\tCan't reopen (%s)\n", filename);
      return -1;
   }

   // Open HEXSUFFIX1 filename
   if(strrchr(filename, '\\') != NULL) filename = strrchr(filename, '\\') + 1;
   if(passes)  sprintf(locbuf, "%s.%d.%s", filename, passes, HEXSUFFIX1);
   else        sprintf(locbuf, "%s.%s", filename, HEXSUFFIX1);

   if((hexout=fopen(locbuf, "w")) == NULL)
   {
      fprintf(stderr, "\tCan't create (%s)\n", locbuf);
      fclose(hexin);
      return -1;
   }

   // Open HEXSUFFIX2 filename
   if(passes)  sprintf(locbuf, "%s.%d.%s", filename, passes, HEXSUFFIX2);
   else        sprintf(locbuf, "%s.%s", filename, HEXSUFFIX2);

   if((sections=fopen(locbuf, "w")) == NULL)
   {
      fprintf(stderr, "\tCan't create (%s)\n", locbuf);
      fclose(hexout);
      fclose(hexin);
      return -1;
   }

   if(1)
   {
      ULONG  order = 0;
      MINMAX *cur, *localmin = &_MinMaxTable.minmax; 
      int end = 0, i;
      LONG abs, prevabs = -1;

      while(end != 1)
      {
         cur   = &_MinMaxTable.minmax;
         end   = 1;
         i     = 0;
         while(cur != NULL)
         {
            if(cur->present != -1 && cur->min >= order)
            {
               if((localmin->min < order) || (cur->min < localmin->min)) 
               {
                  localmin = cur;
                  end = 0;
               }
            }
            i++;
            cur = cur->next;
         }

         // now start grabbing these bytes
         inminmax = localmin;

         if(!end) 
         {
            LONG j;
            int c;
            MINMAX *temp;
      
            if((inminmax->min > inminmax->max) || (inminmax->max > 0xFFFFFUL))
            {
               fprintf(stderr, "\n*** FATAL error: wrong minmax section\n");
               inminmax->present = -1;
            }
            if(inminmax->present != -1)
            {
               for(j=inminmax->min;j<=inminmax->max;j++)
               {
                  // get the modified section of this byte if it's there
                  temp = is_in_active_section((ULONG)j);

                  // first get the offset of the byte in the file
                  abs = (LONG)absoff + (j-(LONG)linoff);

                  if(abs >= 0)
                  {
                     // fetch the byte
                     fseek(hexin, (ULONG)abs, SEEK_SET);
                     c = fgetc(hexin);
                     if(c != EOF) goto byte_working;
                  }

                  // Ok, something isn't working, maybe the byte
                  // is addressed externally.
                  if(temp != NULL && temp->offsetfromfile != 0 
                     && (temp->max-temp->min) > 9)
                  {
                     // first adjust regarding the offsetfromfile
                     abs = (LONG)absoff + ((j-(LONG)temp->offsetfromfile));
                     // then adjust regarding the linear address offset
                     abs = abs-(LONG)linoff;
                     if(abs < 0) continue;
                     // fetch the byte
                     fseek(hexin, (ULONG)abs, SEEK_SET);
                     c = fgetc(hexin);
                     if(c == EOF) continue;
                  }
                  else continue;

byte_working:
                  // a new section ? modifed section ?
                  if(temp != NULL)
                  {
                     // A new modifed section
                     if(state != 1)
                     {
                        state = 1;
                        fprintf(hexout, "\n[* %+6lu %+6lX]: ", abs, j);
                        /* close the previous section and add the new one*/
                        if(prevabs != -1) fprintf(sections, "%+6lu]\n", prevabs);
                        fprintf(sections, "* [%+6lu ", abs);
                     }
                  }
                  else
                  {
                     // A new non-modified section
                     if(state != 0)
                     {
                        state = 0;
                        fprintf(hexout, "\n[  %+6lu %+6lX]: ", abs, j);
                        /* close the previous section and add the new one*/
                        if(prevabs != -1) fprintf(sections, "%+6lu]\n", prevabs);
                        fprintf(sections, "  [%+6lu ", abs);
                     }
                  }

                  // now printf in hex the byte
                  fprintf(hexout, "%02X", c);
                  // save the old location
                  prevabs = abs;
               }  /* for(;;) */
            }  /* if() */
            state = 2;
         }  /* if() */
         // update the order (we will process sections above this address)
         order = localmin->min+1;
      }  /* while() */
      // no finish the line for the ".sections" filename
      fprintf(sections, "%+6lu]\n", prevabs);
   }  /* if() */
   
   fclose(hexin);
   fclose(hexout);
   fclose(sections);

   if(!(com_options&OPT_NODEBUG))
      printf("\n-----> The hex output is in (%s/patch)\n", locbuf);

   return 1;
}

///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////

extern int GetOpcodeFromAreaInList (ULONG InputLA, MACHINE_STATE *pMachState);


int patch_filename(char *filename, char *locbuf)
{
   // We are going to use the actual filename, and _MinMaxTable.curmstuff
   // to create a new filename with the extention .patch by patching the 
   // changes in pMaList into the old filename.

   FILE        *oldfile, *newfile;
   ULONG       position = 0;
   ULONG       absoff = _MinMaxTable.AbsoluteOffset;
   ULONG       linoff = _MinMaxTable.LinearOffsetOfEntry;

   machine_stuff *curmachine = _MinMaxTable.curmstuff;

   if((oldfile=fopen(filename, "rb")) == NULL)
   {
      fprintf(stderr, "\r***patch_filename: Can't open (%s) for reading\n", filename);
      return -1;
   }

   if(strrchr(filename, SLASH) != NULL) filename = strrchr(filename, SLASH) + 1;
   sprintf(locbuf, "%s.%s", filename, PATCHSUFFIX);
   
   if((newfile=fopen(locbuf, "wb")) == NULL)
   {
      fprintf(stderr, "\r***patch_filename: Can't open (%s) for writing\n", locbuf);
      fclose(oldfile);
      return -1;
   }

   // now, read oldfile, translate the absolute offset to a linear address.
   // from this address, get the byte in pMAList, (check first if it has been
   // modified).

   for(;;)
   {
      ULONG linearadd;
      int   c, cc;

      c = fgetc(oldfile);
      if(c == EOF) break; /* the party is over buddy */

      /* compose the linear address as the byte would appear if loaded */
      linearadd = linoff + (position - absoff);

      /* now fetch the byte in pMAList if it exists */
      cc = GetOpcodeFromAreaInList(linearadd, &(curmachine->ms));

      /* is it a valid byte (that has been loaded, eventually modified) ? */
      if(cc == -1) cc = c;
      cc = (int)((unsigned char)cc);

      if(fputc(cc, newfile) == EOF) break; 

      position++;
   }

   // now, free the current machien state structure;

   free_machine_state(curmachine);

   if(!(com_options&OPT_NODEBUG))
      printf("\n-----> The patched file is in (%s)\n", locbuf);

   fclose(oldfile);
   fclose(newfile);

   return 1;
}

///////////////////////////////////////////////////////////////////////////////////

// this is jce stuff //

/* jce */
/*=========================================================================*/
/* finds a free, usable area for the given amount of desired memory 
   and returns the segment and offset
   (right now, just finds highest linear address, but could find
    a whole somewhere)
 */

int GetFreeMemoryBlock (MEMORY_AREA *pMA_,     /* start of list */
         WORD area_size,        /* size of area requested */
         WORD *Segment,         /* segment returned here */
         WORD *Offset           /* offset returned here */
         )
{
/*-------------------------------------------------------------------------*/
   MEMORY_AREA    *pMA;                   /* Starting memory area to search*/
/*-------------------------------------------------------------------------*/

   ULONG MaxLA=0, TestLA;
   WORD offset;

   // MaxLA is the maximum Linear Address found in all the malist
   // linked list
   for (pMA=pMA_; pMA; pMA=pMA->pNextArea)
      {
      if( (TestLA=MALinearAddr(pMA)+pMA->BuffSize-1) > MaxLA)
      MaxLA = TestLA;
      }

   if(MaxLA == 0)    /* no region found */
     return(0);      /* return "nothing found" */
   else              /* return "found" */
     {
       MaxLA++;      /* next byte is free */
       if(MaxLA%0x10 != 0)  MaxLA += 0x10-(MaxLA%0x10);   /* round to 0x10 byte boundary */
       if(Offset == NULL) Offset = 0;
       else offset = *Offset = 0x100;    /* want .com file load point */
       *Segment = (WORD)((MaxLA-(offset)) >> 4); 

       //printf("\n\nMaxLA=%lX\tSegment=%X\n\n",MaxLA,*Segment);
       return(1);  
     }
}
/*=========================================================================*/

