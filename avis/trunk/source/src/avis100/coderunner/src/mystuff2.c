
#ifndef CR_MOREFEATURES
   #define CR_MOREFEATURES
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <time.h>

#include "common.h"
#include "cr_defs.h"

////////////////////////////////////////////////////////////

// WHEN compiling, define W95_32 W_32 CR_DEBUG (HEX_DUMPER) CR_DBGTF

#define BUFSIZE               0x5000
#define HEAD_LEN              128

#define MYDEBUG1              "mydebug1.out"
#define MYDEBUG2              "mydebug2.out"
//#define FILEINDEX              "f:\\dev\\progs\\stopcond\\release\\files.opt"
#define FILEINDEX             "files.opt"

////////////////////////////////////////////////////////////

/* Index Table, keeping track of some events
 */
   MIN_MAX_INDEX _MinMaxTable;

////////////////////////////////////////////////////////////

extern 
int alg_scan(char *testfilename,                // "\dodo\file.com"
             char *rootfilename,                // not necesary
             unsigned char *buf,                // NULL
             char *oldbuf,                      // used by load_from_file
             unsigned long oblen,               // sizeof(oldbuf) or HEAD_LEN
             int ob_wholefile,                  // FALSE
             int inside_compressed,             // not necessary FALSE
             unsigned char saved_head[],        // first HEAD_LEN bits of file
             unsigned int saved_head_len,       // MIN(HEAD_LEN,oblen)
             unsigned long filelen,             // filezise
             unsigned long saved_epoint);       // not necessary (-1L)


extern WORD    _ActiveInstructions;
extern WORD    _IdleInstructions;
extern WORD    _IdleLoops;
extern ULONG   _IdleLoopLinearAddr;
extern _MINMAX _ulPrevMemoryMods[MINMAXACTIVE];

int   debug_option = 0;
FILE  *mydebug2;                    // where the patterns are stored

////////////////////////////////////////////////////////////

_MINMAX *is_in_table(ULONG current)
{
   int i;

#define _MinMaxCur _MinMaxTable.minmax[i]

   for(i=0;i<MINMAXSIZE;i++)
   {
      if(_MinMaxCur.present != -1)
         if(current >= _MinMaxCur.min && current <= _MinMaxCur.max)
            return &(_MinMaxCur);
   }

   return NULL;
}

_MINMAX *get_in_table(void)
{
   int i;

#define _MinMaxCur _MinMaxTable.minmax[i]

   for(i=0;i<MINMAXSIZE;i++)
   {
      if(_MinMaxCur.present == -1) return &(_MinMaxCur);
   }

   return NULL;
}
             
////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
   char bigbuff[BUFSIZE];
   BYTE saved_head[HEAD_LEN];
   FILE *indexfile, *curfile, *mydebug1;
   char filename[256], *p;
   char *c;
   unsigned long num;
   long filesize;
   int i;
   time_t datime;

   time(&datime);

   if(argc < 2)
   {
usage:
      if( (c=strrchr(argv[0], '\\')) == NULL) c = argv[0];
      else c++;
      fprintf(stderr, "\nUsage: %s [d/t/n]\t(debug/debug&trace/nothing)\n", c);
      return -1;
   }

   switch(*argv[1])
   {
   case 'd':
      debug_option = 1;
      break;
   case 't':
      debug_option = 2;
      break;
   case 'n':
      break;
   default:
      goto usage;
   }
   
   if((indexfile=fopen(FILEINDEX, "r")) == NULL)
   {
      fprintf(stderr, "\n\tError opening %s\n", FILEINDEX);
      return -1;
   }

   if((mydebug1=fopen(MYDEBUG1, "w")) == NULL)
   {
      fprintf(stderr, "\tCan't create (%s)\n", MYDEBUG1);
      return -1;
   }

   if((mydebug2=fopen(MYDEBUG2, "w")) == NULL)
   {
      fprintf(stderr, "\tCan't create (%s)\n", MYDEBUG2);
      return -1;
   }

   remove("cr_debug.out");
   remove("mbuff.hex");
   
   while(fgets(filename, 256, indexfile) != NULL)
   {
      if(*filename == '#' || *filename == '\n' || *filename == '\r') continue;
      // some initialisation, put that in cr_fscan() rather

      if((c=strpbrk(filename, "\n\r")) != NULL)
         filename[(unsigned)(c-filename)] = '\0';
      
      
      if((curfile=fopen(filename, "rb")) == NULL)
      {
         fprintf(stderr, "\tCan't open (%s)\n", filename);
         continue;
      }
      
      // read the begining of the file
      if( (num=fread(bigbuff, 1, BUFSIZE, curfile)) == 0)
      {
get_off:
         fprintf(stderr, "\tCan't read this file (%s)\n", filename);
         fclose(curfile);
         continue;
      }

      if(num < (unsigned long)HEAD_LEN) goto get_off;    // for COM & EXE & XXX
      if((filesize=filelength(fileno(curfile))) == -1L)
      {
         fprintf(stderr, "\tError with filelenght(%s)\n", filename);
         fclose(curfile);
         continue;
      }
         
      memcpy(saved_head, bigbuff, HEAD_LEN);
      fclose(curfile);
      
      fprintf(mydebug1, "\n##############################\n"
                        "\nProcessing (%s)\n", filename);
#ifdef MYDEBUG
      fprintf(mydebug2, "\n\n#################################################################\n"
                        "\n# %s\n", filename);
#endif
      printf("Processing (%s)\n", filename);

      alg_scan(filename,
               filename,
               NULL,
               bigbuff,
               num,
               FALSE,
               FALSE,
               saved_head,
               HEAD_LEN,
               (unsigned long)filesize,
               -1L);

      fprintf(mydebug1, "\tActiveInst\t\t=%u\n"
                        "\tIdleInst\t\t=%u\n"
                        "\tIdleLoop\t\t=%u\n",
                        _ActiveInstructions, 
                        _IdleInstructions, 
                        _IdleLoops);

      fprintf(mydebug1, "\n\tNumOfTotalInst\t=%lu\n"
                        "\tNumOfDiffInst\t=%lu\n"
                        "\tNumOfDiffLoops\t=%lu\n"
                        "\tNumOfRealActive\t=%lu\n",
                        _MinMaxTable.NumOfTotalInst, 
                        _MinMaxTable.NumOfDiffInst,
                        _MinMaxTable.NumOfDiffLoops,
                        _MinMaxTable.NumOfRealActiveInst);
               
      fprintf(mydebug1, "\n\tThe Different Sections (%d):\n",
                        _MinMaxTable.nsections);

      if((p=strrchr(filename, '\\')) == NULL) p = filename;
      else 
      {
         *p = '/';
         if((p=strrchr(filename, '\\')) == NULL) p = filename;
         else *(p++) = '/';
      }

      fprintf(mydebug2, "%-25s"
                        "%-5u %-5u %-3u - %-5u %-5u - %-4u %-3u - %-2u\n",
                        p,
                        _MinMaxTable.NumOfTotalInst,
                        _MinMaxTable.NumOfDiffInst,
                        _MinMaxTable.NumOfDiffLoops,
                        _ActiveInstructions,
                        _MinMaxTable.NumOfRealActiveInst,
                        _MinMaxTable.NumOfRealIdleInst, //_IdleInstructions,
                        _IdleLoops,
                        _MinMaxTable.nsections);


#define _MinMaxCur _MinMaxTable.minmax[i]

      for(i=0;i<MINMAXSIZE;i++)
      {
         if(_MinMaxCur.present != -1)
            fprintf(mydebug1, "\t[%.2d] min=%6lX max=%6lX\n", i,
                  _MinMaxCur.min, _MinMaxCur.max);
      }

      fprintf(mydebug1, "\n\tThe Modified Sections:\n");

#undef  _MinMaxCur
#define _MinMaxCur _ulPrevMemoryMods[i]

      for(i=0;i<MINMAXACTIVE;i++)
      {
         if(_MinMaxCur.present != -1)
            fprintf(mydebug1, "\t[%.2d] min=%6lX max=%6lX\n", i,
                  _MinMaxCur.min, _MinMaxCur.max);
      }
      fflush(mydebug1);
      fflush(mydebug2);
   }
   
   fclose(indexfile);
   fclose(mydebug1);
   fclose(mydebug2);

   printf("\nTime processing this mess: %u\n", time(NULL)-datime);
   
   return 1;
}
      
