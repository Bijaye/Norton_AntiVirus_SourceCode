///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//                   Coderunner hacks for StopConditions and Stuff               //
//                            IBM Anti Virus Stuff                               //
//                               Riad Souissi                                    //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////

// WHEN compiling, define W95_32 W_32 CR_DEBUG (HEX_DUMPER) CR_DBGTF             //

#define BUFSIZE               0x5000
#define HEAD_LEN              128

#define MYDEBUG1              "mydebug1.out"
#define MYDEBUG2              "mydebug2.out"
#define MYDEBUG3              "trace.out"
#define FILEINDEX             "files.opt"
#define FOPTIONLONG           "options_l.opt"
#define FOPTIONSHORT          "options_s.opt"


///////////////////////////////////////////////////////////////////////////////////

/* Index Table, keeping track of some events */
#ifdef CR_MOREFEATURES
#ifndef NEW1
   extern MIN_MAX_INDEX _MinMaxTable;
#endif
#else
#ifndef CR_DO_SCANNER
   ULONG _NumOpcodes;
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////

#ifndef CR_DO_SCANNER   //riad//

extern int alg_scan(
             char *testfilename,                // "\dodo\file.com"
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

#if NEW1

extern CR_VARIOUS_VARIABLES   crvv;
#else
extern WORD    _ActiveInstructions;
extern WORD    _IdleInstructions;
extern WORD    _IdleLoops;
extern ULONG   _IdleLoopLinearAddr;
#ifdef CR_MOREFEATURES2
extern MINMAX  *_ulPrevMemoryMods;
#else
//extern ULONG   _ulPrevMemoryMods[NUM_PREV_MEMORY_MODS];
#endif

#endif
///////////////////////////////////////////////////////////////////////////////////

#ifdef CR_MOREFEATURES
#ifndef NEW1
extern int     _Fulldecryption;
#endif
extern struct  cr_ini _Default_ini;
extern WORD    total_rw_traps;
extern WORD    total_exec_traps;
ULONG          _nget_in_table, _nupdate_modif;     // debug info only
#endif

///////////////////////////////////////////////////////////////////////////////////

int            debug_option=     0;
FILE           *mydebug2;                 // where the patterns will be stored
FILE           *mydebug3;                 // where the tracing will be splashed
ULONG          _RetCode;                  // debug info only
clock_t        datime;                    // used for timing
clock_t        currentime;

#ifndef NEW1
int            _OpcodeSize;               // this is the opcode size counter
                                          // used to precisely determine the
                                          // min and max of executed sections
                                          // in _MinMaxTable.minmax
#endif /* NEW1 */

//#define        OPT_ALLFILES      0x01
//#define        OPT_ONEFILE       0x02
//#define        OPT_FROMINDEX     0x04
//#define        OPT_DIRECTORY     0x08
//#define        OPT_SUBDIRS       0x10
//#define        OPT_WOW           0x20
//#define        OPT_SAVEHEX       0x40
//#define        OPT_DECRYPT       0x80
int            com_options =     0;       // well, attributes from command line options

int            dojob(char*, FILE*, char*); 
int            dojob_on_dir(char*, FILE*);
#ifdef CR_MOREFEATURES
int            mystuff_getoptions(struct cr_ini *, char *);
int            printdebug(char *, int);
int            printdebug1(FILE *);
#endif

#ifdef __UNIX__
#define SLASH  '/'
#define ASLASH '\\'
#else
#define SLASH  '\\'
#define ASLASH '/'
#endif

///////////////////////////////////////////////////////////////////////////////////

#ifndef CR_DO_SCANNER

#define ABORT_ITERATOR  -2
#define ABORT_NOW       -3

int     StopEmulation = 0;

void  set_signal_int(int sig)
{
   if((++StopEmulation) >= 2) 
   {
      machine_stuff *cur = _MinMaxTable.mstuff, *other;

      printf("\n\n\t\t[Program interrupted by user]\n\n");
      free_almost_evrything();
      while(cur != NULL)
      {
         other  = cur; cur = cur->next; free_machine_state(other);
      }
      exit(-1);
   }
   else printf("\n\n\t\t[Program got an interrupt signal]\n\n");
   signal(SIGINT, set_signal_int);
}
#endif
   
#ifdef __UNIX__
LONG filelength(int handler)
{
  struct stat statos;

  if(fstat(handler, &statos) == -1) return -1L;
  return (LONG)statos.st_size;
}
#endif
          
///////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
   FILE *indexfile, *mydebug1;
   char filename[256] = {'\0'};
   char dirname[256]  = {'\0'};
   char *c;
   char **argvv = argv+1;
   int  numfiles = 0;

   datime = clock();
   // Let's set the signal handlet for SIGINT (Ctrl C)
   // One SIGINT will abort the loop, 2 abort the entire program
   signal(SIGINT, set_signal_int);

   if(argc < 2)
   {
usage:
      if( (c=strrchr(argv[0], SLASH)) == NULL) c = argv[0];
      else c++;
      fprintf(stderr,   "\nUsage: %s [-f file] or [-d dir] [-s] [-a] [-decrypt] [t1 or t2] [-timeout seconds]\n"
                        "\n\tf: specify filename\n"
                        "\td: specify directory\n"
                        "\ts: proceed subdirectories ?\n"
                        "\ta: emulate everyfile ?\n"
                        "\tdecrypt: generates the unencrypted version of the file (if possible)\n"
                        "\tt1,t2: debug or debug&trace or nothing\n"
                        "\tsome fancy options: [-wow 1/2]   to live longer\n"
                        "\t                    [-b]         to save the bytes of executed regions\n"
                        "\t                    [-nodebug]   to use lighter outputing\n"
                        "\n\tIf no -f or -a, %s is used\n"
                        "\texample: '%s -a -d .'\n", c, FILEINDEX, c);
      return -1;
   }

   debug_option = 0;

   while((argvv!=NULL & *argvv!=NULL))
   {
      switch(**argvv)
      {
      case 't':
         switch(*(*argvv+1))
         {
         case '1':
            debug_option = 1;
            break;
         case '2':
            debug_option = 2;
            break;
         }
         break;
      case '-':
         switch(*(*argvv+1))
         {
         case 'f':
            argvv++;
            strcpy(filename, *argvv);
            com_options |= OPT_ONEFILE;
            break;
         case 'd':
            if(!strcmp(*argvv+1, "decrypt")) 
            {  com_options |= OPT_DECRYPT; break;  }
            argvv++;
            strcpy(dirname, *argvv);
            com_options |= OPT_DIRECTORY;
            break;
         case 's':
            com_options |= OPT_SUBDIRS;
            break;
         case 'a':
            com_options |= OPT_ALLFILES;
            break;
         case 'w':
            if(!strcmp(*argvv+1, "wow")) com_options |= OPT_WOW;
            else goto usage;
            argvv++;
            _Default_ini.MaxDeepness = atoi(*argvv);
            break;
         case 'b':
            com_options |= OPT_SAVEHEX;
            break;
         case 'n':
            if(!strcmp(*argvv+1, "nodebug")) 
            {  com_options |= OPT_NODEBUG; break;  }
            else goto usage;
			break;
		 case 't':
			 if(!strcmp(*argvv+1, "timeout")) {
				 argvv++;
				 timeout = atoi(*argvv);
				 starttime = time(NULL);
				 //printf ("\n TIMEOUT is %d\n\n", timeout);
			 }
			 else goto usage;
			 break;
         default:
            goto usage;
         }
         break;
      default:
         goto usage;
      }
      argvv++;
   }

   /* print out the version number (date) */
   if(!(com_options&OPT_NODEBUG))
      printf("\nVersion [%s] {%s}\n\n", __DATE__, __TIME__);

   // test conflicting options
   if((com_options & OPT_DIRECTORY) && (com_options & OPT_ONEFILE)) 
      goto usage;
   if((com_options & OPT_DECRYPT) && (com_options & OPT_WOW))
      goto usage;

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

   if((mydebug3=fopen(MYDEBUG3, "w")) == NULL)
   {
      fprintf(stderr, "\tCan't create (%s)\n", MYDEBUG3);
      return -1;
   }

#ifdef CR_MOREFEATURES
   if(mystuff_getoptions(&_Default_ini, argv[0]) == -1) return -1;
#endif
   remove("cr_debug.out");
   remove("mbuff.hex");
   
   // Ok, let's emulate a filename alone
   if(com_options & OPT_ONEFILE) 
   {
      dojob(filename, mydebug1, "");
      numfiles = 1;
      goto damned;
   }
   // Ok, let's emulate a directory (and it's subdirs)
   if(com_options & OPT_DIRECTORY)
   {
      numfiles = dojob_on_dir(dirname, mydebug1);
      goto damned;
   }
   // Ok, let's get the files from a input filename
   else 
   {
      if((indexfile=fopen(FILEINDEX, "r")) == NULL)
      {
         fprintf(stderr, "\n\tError opening (%s)\n", FILEINDEX);
         return -1;
      }
      while(!StopEmulation && fgets(filename, 256, indexfile) != NULL)
      {
         if(*filename == '#' || *filename == '\n' || *filename == '\r') 
            continue;

         if((c=strpbrk(filename, "\n\r")) != NULL)
            filename[(unsigned)(c-filename)] = '\0';

         if(dojob(filename, mydebug1, "") != -1) numfiles++;
      }
      fclose(indexfile);
   }

damned:

   fclose(mydebug1);
   fclose(mydebug2);
   fclose(mydebug3);

  /* printf("\nGot %d files. Time for processing: %2.1f\n", 
   *   numfiles, getclock(datime,clock()));
   */
   printf("\n");
   return 1;
}

///////////////////////////////////////////////////////////////////////////////////

int dojob_on_dir(char *dirname, FILE *mydebug1)
{
   #ifdef __UNIX__
   struct dirent *namelist;
   struct _stat statos;
   #else  /* WIN32 */
   struct _finddata_t namelist;
   #endif /* WIN32 */
   int   numfiles = 0;
   int   rez = 0;
   DIREC dir;
   char  *dirfilename;
   static char currentdir[124];
   static char BigZero[1024];

   // We are going to make two passes, one for files, next for directories
   int flag = 0;

   #ifndef __UNIX__ /* WIN32 */
   strcpy(currentdir, dirname);
   if(currentdir[strlen(currentdir)-1] == SLASH) strcat(currentdir, "*.*");
   else strcat(currentdir, "\\*.*");
   #endif

   #ifdef __UNIX__
   if((dir=opendir(dirname)) == NULL)
   #else  /* WIN32 */
   if((dir=_findfirst(currentdir, &namelist)) == -1)
   #endif /* WIN32 */
   {
      getcwd(BigZero, 1024);
      fprintf(stderr, "\r\tCan't OPEN the directory (%s%c%s)\n", 
         BigZero, SLASH, dirname);
      return -1;
   }

   chdir(dirname);
   getcwd(BigZero, 1024);
   strcat(BigZero, "\\");

next_pass:

   #ifdef __UNIX__
   while((namelist=readdir(dir)) != NULL)
   {
      if(StopEmulation) return numfiles;
      dirfilename = namelist->d_name;
   #else  /* WIN32 */
   while(_findnext(dir, &namelist) == 0)
   {
      if(StopEmulation) return numfiles;
      dirfilename = namelist.name;
   #endif /* WIN32 */

      if(*dirfilename == '\0' || *dirfilename == '.' )   continue;
      if(IsPrefix(dirfilename, "."HEXSUFFIX1) || 
         IsPrefix(dirfilename, "."HEXSUFFIX2) ||
         IsPrefix(dirfilename, "."PATCHSUFFIX)) continue;

   #ifdef __UNIX__
      if(_stat(dirfilename, &statos)  != 0) continue;
      if((statos.st_mode & _S_IFDIR) && (com_options & OPT_SUBDIRS))
   #else  /* WIN32 */
      if((namelist.attrib & _A_SUBDIR) && (com_options & OPT_SUBDIRS)) 
   #endif /* WIN32 */
      {
         if(!flag) continue;

         if((rez = dojob_on_dir(dirfilename, mydebug1)) >= 0) numfiles += rez;

         //getcwd(BigZero, 1024);
         //strcat(BigZero, "\\");
         //fprintf(mydebug2, "*** (%s)\n", BigZero);
         continue;
      }
      if(com_options & OPT_ALLFILES) 
      { 
         if(!strchr(dirfilename, '.')) continue; 
      }
      else
      {
         if(!IsPrefix(dirfilename, ".com") && !IsPrefix(dirfilename, ".exe"))
            continue;
      }
      // Ok, now we got a mostly working filename
      if(!numfiles && !flag) fprintf(mydebug2, "\n*** (%s)\n", BigZero);
      if(!flag) if(dojob(dirfilename, mydebug1, BigZero) >= 0) numfiles++;

   }

   if(!flag)
   {
      // Now scan subdirectories
      flag = 1;
   #ifdef __UNIX__
      rewinddir(dir);
   #else
      chdir("..");
      dir = _findfirst(currentdir, &namelist);
      chdir(dirname);
   #endif
      goto next_pass;
   }

   #ifdef __UNIX__
   if(closedir(dir) != 0)
   #else  /* WIN32 */
   if(_findclose(dir) == -1)
   #endif /* WIN32 */
   {
      fprintf(stderr, "\n\tUnable to close directory (%s)\n", dirname);
   }

   chdir("..");
   
   return numfiles;
}

///////////////////////////////////////////////////////////////////////////////////

int dojob(char *filename, FILE *mydebug1, char *bigstuff)
{
   static char    locbuf[81];
   char           bigbuff[BUFSIZE];
   BYTE           saved_head[HEAD_LEN];
   FILE           *curfile;
   char           *pistache;
   unsigned long  num;
   long           filesize;
   MINMAX         *cur;
   int            passes = 0;       // when we emulate several times the same file
// Fred: This is horrible programming style to declare this prototype here, I know...
// but bad programming style is a tradition in the immune system programs
   void			  pack_sections (void);

   if(*bigbuff == '\0') // I think it should be bigstuff here (?)
   {
      if((pistache=strrchr(filename, SLASH)) == NULL) pistache = filename;
      else 
      {
         *pistache = ASLASH;
         if((pistache=strrchr(filename, SLASH)) == NULL) pistache = filename;
         else { *strrchr(filename, ASLASH) = SLASH; }
      }
   }
   else pistache = filename;

   x86_prefetch_index = 0;

for( ; ; )
   {
   #ifdef __UNIX__
   fflush(stdout);
   #endif

   _CodeModifPrefetch = 0;

   if((curfile=fopen(filename, "rb")) == NULL)
   {
      fprintf(stderr, "\r*** Can't OPEN %-60s\n", filename);
      return -1;
   }
   
   // read the begining of the file
   if( (num=fread(bigbuff, 1, BUFSIZE, curfile)) == 0)
   {
get_off:
      fprintf(stderr, "\r*** Can't READ %-60s\n", filename);
      fclose(curfile);
      return -1;
   }

   if(num < (unsigned long)HEAD_LEN) goto get_off;    // for COM & EXE & XXX
   if((filesize=filelength(fileno(curfile))) == -1L)
   {
      fprintf(stderr, "\r*** Error with FILELENGHT %-50s\n", filename);
      fclose(curfile);
      return -1;
   }
      
   memcpy(saved_head, bigbuff, HEAD_LEN);
   fclose(curfile);
   
   fprintf(mydebug1, "\n##############################\n"
                     "\nProcessing (%s)\n", filename);
   #ifdef MYDEBUG
   fprintf(mydebug2, "\n\n############################\n"
                     "\n# %s\n", filename);
   #endif

   // even if we initialize evrything in cr_fscan(), we do that
   // here to avoid using previous values when aborting the
   // emulation of a file because of something.
#ifdef CR_MOREFEATURES  //riad//
   _MinMaxTable.nsections           = 0;
   _MinMaxTable.NumOfLastDiffInst   = 0;
   _MinMaxTable.NumOfDiffInst       = 0;
   _MinMaxTable.NumOfDiffLoops      = 0;
   _MinMaxTable.NumOfRealActiveInst = 0;
   _MinMaxTable.NumOfTotalInst      = 0;
   _MinMaxTable.NumOfRealIdleInst   = 0;
   cur = &_MinMaxTable.minmax;
   while(cur != NULL) { cur->present = -1; cur = cur->next; }
   //for(i=0;i<MINMAXSIZE;i++) _MinMaxTable.minmax[i].present = -1;    
#else 
   _NumOpcodes = 0L;
#endif
#ifdef CR_MOREFEATURES2
   //for(i=0;i<MINMAXACTIVE;++i) 
   //{
   //   _ulPrevMemoryMods[i].present = -1;
   //}
#endif

   _ActiveInstructions = 0;
   _IdleInstructions = 0;
   _IdleLoops = 0;

#ifdef CR_MOREFEATURES
   total_rw_traps    = 0;
   total_exec_traps  = 0;
   _nget_in_table = _nupdate_modif = 0;
#endif

   currentime = clock();

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

   // to link close section before terminating
   //get_new_exe_section();
   // Fred: This was a bug, what you really need here is just to pack the sections, not allocate a new one
   pack_sections();
   get_new_active_section(0);

   sprintf(locbuf,"Done [%04u] (%s%s)  (%2.1f)", 
      (unsigned int)getclock(datime, clock()), 
      bigstuff+(strlen(bigstuff)<40?0:(strlen(bigstuff)-40)),
      (*bigstuff)?filename+(strlen(filename)<10?0:(strlen(filename)-10)):
      filename+(strlen(filename)<55?0:(strlen(filename)-55)),
      getclock(currentime,clock()));
   printf("\r%-79s",locbuf);


   // First, printf some pretty useful results in mydebug1.out
   printdebug1(mydebug1);

   // Then, some less but still useful results in mydebug2.out
   printdebug(pistache, 0);

   // Now save if asked the HEX values of Executed regions before
   // we free memory space
   if(com_options & OPT_SAVEHEX) save_hex_regions(filename, locbuf, passes);

   // Patch the filename if OPT_DECRYPT has been specified
   if(com_options & OPT_DECRYPT) patch_filename(filename, locbuf);

   // Free allocated sections for executed code and stuff
   // Also free the linked list stuff
   free_almost_evrything();

   fflush(mydebug1);
   fflush(mydebug2);

   // Prefetch Queue check

   x86_prefetch_index++;

   if(_CodeModifPrefetch == 0) break;
#ifdef CR_DO_SCANNER
   if(found_precise_match) break;
#else
   // Well, to emulate the scanner, let's say that a working code
   // that happens to decrypt non-neglictaly, is okay
   if(_ActiveInstructions >= 60) break;
#endif

   printf("\n*** Code SELF MODIF Prefetch Queue at %d [%d]\n", 
      x86_prefetch_index-1, x86_prefetch_list[x86_prefetch_index-1]);
   fprintf(mydebug2,"*** Code SELF MODIF Prefetch Queue at %d [%d]\n", 
      x86_prefetch_index-1, x86_prefetch_list[x86_prefetch_index-1]);

   // for safety, change the name of the output file now
   passes++;

   // We finnished with the prefetch queue ?
   if(x86_prefetch_list[x86_prefetch_index] == -1) break;
   }

   return 1;
}

#endif /* CR_DO_SCANNER */ //riad//

///////////////////////////////////////////////////////////////////////////////////

int printdebug(char *pistache, int number)
{
   char abort_explanation[64];
   ULONG _RetCode = _MinMaxTable.RetCode;
#ifdef CR_MOREFEATURES2
   ULONG trash;
#endif

  *abort_explanation = '\0';

   strcat(abort_explanation, "[");
   if (_RetCode & READ_TRAP)              strcat(abort_explanation, "R_TP ");
   if (_RetCode & WRITE_TRAP)             strcat(abort_explanation, "W_TP ");
   if (_RetCode & EXECUTE_TRAP)           strcat(abort_explanation, "E_TP ");
   if (_RetCode & UNSUPPORTED_OPCODE)     strcat(abort_explanation, "U_OP ");
   if (_RetCode & MAXOPS_REACHED)         strcat(abort_explanation, "MAXOP ");
   //if (_RetCode & TRACE_FLAG_SET)         strcat(abort_explanation, "TR_FL ");
   if (_RetCode & ZERODIVIDE)             strcat(abort_explanation, "ZEROD ");
   if (_RetCode & DIVIDE_ERROR)           strcat(abort_explanation, "DVERR ");
   //if (_RetCode & PORT_IO)                strcat(abort_explanation, "PR_IO ");
   //if (_RetCode & CANNOT_EXECUTE_INT)     strcat(abort_explanation, "NOINT ");
   if (_RetCode & USER_ABORT)             strcat(abort_explanation, "ABORT ");
   //if (_RetCode & HLT_EXECUTED)           strcat(abort_explanation, "HEXEC ");
   //if (_RetCode & CODE_SELF_MODIFIED)     strcat(abort_explanation, "SMODF ");
   if (_RetCode & BREAK_CONDITION)        strcat(abort_explanation, "SCOND ");
   if (_RetCode & INVALID_PARAMETER)      strcat(abort_explanation, "BPARM ");
   if (_RetCode & TEST_TSR)               strcat(abort_explanation, "ABORT TSR");

   if (_RetCode & USER_ABORT) 
   {
      if (_RetCode & USER_CANCEL) strcat(abort_explanation, "SKIP ");
      else sprintf(abort_explanation+strlen(abort_explanation),"%X:%X", 
                  _MinMaxTable.interrupt1, _MinMaxTable.interrupt2);
   }
   strcat(abort_explanation, "]");


#ifdef CR_MOREFEATURES  //riad//
#ifdef CR_MOREFEATURES2
   {
      MINMAX  *cur = _MinMaxTable.ulPrevMemoryMods;

      trash = 0;

      while(cur != NULL)
      {
         if(cur->present != -1) trash = __max(trash, cur->max-cur->min);
         cur = cur->next;
      }
   }
#endif
#endif   /* CR_MOREFEATURES */

   fprintf(mydebug2, "%+3u %-26s %-15s "
               "%-7u %-4u %-2u | %-4u/%-4u [%4u] {%1u} | %+4u/%-4u IdlL=%-2u | Sect=%-2u"
               " | RWtp=%+2u/%-2u Tmiss=%+4lu/%-4lu | Time=%2.1f\n",
               number, pistache,
               abort_explanation,
#ifdef CR_MOREFEATURES
               _MinMaxTable.NumOfTotalInst,
               _MinMaxTable.NumOfDiffInst,
               _MinMaxTable.NumOfDiffLoops,
               _ActiveInstructions,
               _MinMaxTable.NumOfRealActiveInst,
#else
               _NumOpcodes,0,0, _ActiveInstructions,0,
#endif
#ifdef CR_MOREFEATURES2
               (int)trash,
#else
               0,
#endif
#ifdef CR_MOREFEATURES
               _MinMaxTable.NumOfDiffDecryptors,
               _MinMaxTable.NumOfRealIdleInst, _IdleInstructions,
               _IdleLoops,
               _MinMaxTable.nsections,
               total_rw_traps, total_exec_traps,
               _nget_in_table, _nupdate_modif,
#else             
               0, 0, _IdleInstructions, _IdleLoops,0,0,0,0,0,
#endif
               getclock(currentime, clock()));

   fflush(mydebug2);

   return 1;
}

///////////////////////////////////////////////////////////////////////////////////

int printdebug1(FILE *mydebug1)
{
   int i;
   ULONG  order = 0;
   MINMAX *cur, *localmin = &_MinMaxTable.minmax; 
   int end = 0, ii;
   extern BYTE *tsr_test_code;

   fprintf(mydebug1, "\tActiveInst\t\t=%u\n"
                     "\tIdleInst\t\t=%u\n"
                     "\tIdleLoop\t\t=%u\n",
                     _ActiveInstructions, 
                     _IdleInstructions, 
                     _IdleLoops);

#ifdef CR_MOREFEATURES  //riad//

   fprintf(mydebug1, "\n\tTotDiffInst\t=%lu\n\n"
                     "\tNumOfTotalInst\t=%lu\n"
                     "\tNumOfDiffLoops\t=%lu\n"
                     "\tNumOfRealActive\t=%lu\n",
                     /*_MinMaxTable.NumOfDiffInst*/ _MinMaxTable.TotDiffInst,
                     _MinMaxTable.NumOfTotalInst, 
                     _MinMaxTable.NumOfDiffLoops,
                     _MinMaxTable.NumOfRealActiveInst);

   fprintf(mydebug1, "\n\tMaxRWTrapsToRecover\t=%u\n"
                     "\tMaxExecTrapsToRecover\t=%u\n",
                     total_rw_traps, total_exec_traps);                       
            
   fprintf(mydebug1, "\n\tThe Different Sections (%d):\n",
                     _MinMaxTable.nsections);

   // First, printf out the executed sections in the order they have been created

   cur = &_MinMaxTable.minmax;
   i = 0;

   while(cur != NULL)
   {
      if(cur->present != -1)
         fprintf(mydebug1, "\t[%.2d] min=%6lX max=%6lX\n", i, cur->min, cur->max);
      else fprintf(mydebug1, "\t[%.2d] min=(null) max=(null)\n", i);
      i++;
      cur = cur->next;
   }

   // Then, try to order them, andn print them out

   fprintf(mydebug1, "\n\tOrdered Different Sections (%d):\n",
                     _MinMaxTable.nsections);

   while(end != 1)
   {
      cur   = &_MinMaxTable.minmax;
      end   = 1;
      i     = 0;
      ii    = 0;
      while(cur != NULL)
      {
         if(cur->present != -1 && cur->min >= order)
         {
            if((localmin->min < order) || (cur->min < localmin->min)) 
            {
               localmin = cur;
               ii = i;
               end = 0;
            }
         }
         i++;
         cur = cur->next;
      }
      if(!end) fprintf(mydebug1, "\t[%.2d] min=%6lX max=%6lX  %c\n", ii++,
               localmin->min, localmin->max,
               ((localmin->max-localmin->min)==0x47)?'T':' ');
      order = localmin->min+1;
   }

   fprintf(mydebug1, "\n\tThe Modified Sections(%d):\n",
                     _MinMaxTable.ulsections);

#ifdef CR_MOREFEATURES2

   // Now, print out the modified/moved sections

   cur = _MinMaxTable.ulPrevMemoryMods;

   while(cur != NULL)
   {
      if(cur->present != -1)
      {
         fprintf(mydebug1, "\t[%.2d] min=%6lX max=%6lX  %c\n", i,
               cur->min, cur->max, cur->offsetfromfile?'Y':' ');
      }
      cur = cur->next;
   }
#endif

#endif   /* CR_MOREFEATURES */

   return 1;
}

///////////////////////////////////////////////////////////////////////////////////

#ifdef CR_MOREFEATURES  //riad//

#define str2dec(stringos)  strtoul(stringos,NULL,0)

int mystuff_getoptions(struct cr_ini *_defaultini, char *fullpath)
{
   FILE *file;
   int state = 0;
   char BigZero[256];
   char *filename;

   filename = (com_options&OPT_DECRYPT)?FOPTIONSHORT:FOPTIONLONG;

   /* look for the option file in the current, or the program directory */

   strcpy(BigZero, fullpath);

   if(strrchr(BigZero, SLASH))
      {
      *(strrchr(BigZero, SLASH)+1) = '\0';
      strcat(BigZero, filename);
      }
   else strcat(BigZero, filename);

   if((file=fopen(BigZero, "r")) == NULL)
      {
      /* try the current directory */
      strcpy(BigZero, filename);
      if((file=fopen(BigZero, "r")) == NULL)
         {
         fprintf(stderr, "\n\tCan't open (%s)\n", BigZero);
         return -1;
         }
      }

   /* fetch the values of each element in the correct order */

   while(fgets(BigZero, sizeof(BigZero), file) != NULL)
      {
      if(*BigZero == '#' || *BigZero == '\n') continue;
      if(state==0) 
         { 
         _defaultini->MaxActiveInstructions = (ULONG)str2dec(BigZero);
         state = 1;
         continue;
         }
      if(state==1) { 
         _defaultini->MaxIdleInstructions = (WORD)str2dec(BigZero);
         state = 2;
         continue;
         }
      if(state==2) { 
         _defaultini->MaxIdleLoops = (WORD)str2dec(BigZero);
         state = 3;
         continue;
         }
      if(state==3) { 
         _defaultini->MaxRWTrapsToRecover = (WORD)str2dec(BigZero);
         state = 4;
         continue;
         }
      if(state==4) { 
         _defaultini->MaxExecTrapsToRecover = (WORD)str2dec(BigZero);
         state = 5;
         continue;
         }
      if(state==5) { 
         _defaultini->ExeTrapHalfBuffSize = (WORD)str2dec(BigZero);
         state = 6;
         continue;
         }
      if(state==6) { 
         //_defaultini->ulFlagsIgnoreConditions = (ULONG)str2dec(BigZero);
         state = 7;
         continue;
         }
      if(state==7) { 
         _defaultini->RWTrapHalfBuffSize = (WORD)str2dec(BigZero);
         state = 8;
         continue;
         }
      if(state==8) { 
         _defaultini->MaxCXForLoops = (WORD)str2dec(BigZero);
         state = 9;
         continue;
         }
      if(state==9) {
         _Fulldecryption = (int)str2dec(BigZero);
         state = 10;
         continue;
         }
      if(state==10) {
         _defaultini->MaxOpcodes = (ULONG)str2dec(BigZero);
         state = 11;
         continue;
         }
      if(state=11) {
         _defaultini->MaxIdleBeforeSkip = (WORD)str2dec(BigZero);
         state = 12;
         }
      }

   if(state != 12)
      {
      fprintf(stderr, "\nInvalid structure of (%s)\n", filename);
      fclose(file);
      return -1;
      }
#ifndef CR_DO_SCANNER   //riad//
   if(!(com_options&OPT_NODEBUG))
   {
      printf("\t\tMaxActive   = %u\n", _defaultini->MaxActiveInstructions);
      printf("\t\tMaxIdle     = %u\n", _defaultini->MaxIdleInstructions);
      printf("\t\tMaxIdloop   = %u\n", _defaultini->MaxIdleLoops);
      printf("\t\tMaxRW       = %u\n", _defaultini->MaxRWTrapsToRecover);
      printf("\t\tMaxExec     = %u\n", _defaultini->MaxExecTrapsToRecover);
      printf("\t\tMaxOpcodes  = %lX\n", _defaultini->MaxOpcodes);
      printf("\t\tFullDecrypt = %s\n\n", _Fulldecryption?"YES":"NO");
      printf("\t\tMax Deep    = %d\n", _defaultini->MaxDeepness);
      printf("\t\tMaxIdleSkip = %u\n\n", _defaultini->MaxIdleBeforeSkip);
   }
#endif
   fclose(file);
   return 1;
}
#endif /* CR_MOREFEATURES */  //riad//

///////////////////////////////////////////////////////////////////////////////////

