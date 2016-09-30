#ifdef MACNEWAPI
/* ll.h */
#ifndef LL_H
#define LL_H

typedef int ll_filetype;

ll_filetype ou_fopen(char *filename);
void ou_fclose(ll_filetype );
int ou_ReadFile(ll_filetype , long , void *, int );
int ou_WriteFile(ll_filetype , long , void *, int );

//#define ELL_MALLOC_FAILED     (VS_ERROR_BASE + 0x360)

#endif
#else
/* ll.h */
#ifndef LL_H
#define LL_H

#ifdef T_VXD
   int ou_fopen( char *filename );
   PFILEDATA ou_fBufferOpen(unsigned long filehandle );

   long ou_fgetfilesize( unsigned long filehandle );

   void ou_fclose(int stream);
   void ou_fBufferClose(unsigned long filehandle, PFILEDATA pFileData );

   int ou_ReadFile(unsigned long file, long fpos, void *buf, int len);
   int os_ReadFile(unsigned long file, long fpos, void *buf, int len);

   int ou_WriteFile(unsigned long file, long fpos, void *buf, int len);

   extern int errno;
#endif

#ifndef T_VXD

//#if defined(OLE2_DEBUG) || defined(WFW_DEBUG) || defined(MACRO_DEBUG) || defined(DEBUG) || defined(XL_DEBUG) || defined(VBA3_DEBUG)
//   #define DEBUG
//#endif

#include <stdio.h>

typedef unsigned long   DWORD;

#ifndef T_OS2

 #if defined(WATC)
 #else
  typedef unsigned char   BYTE;
 #endif

  typedef unsigned char   UCHAR;
#endif

#if defined(WATC)
#else
typedef short unsigned  int WORD;
#endif

typedef int             HANDLE;

// include definitions for buffered I/O
#include "iavvxd1c.h"

#ifdef STREAM_FILE_ACCESS
   FILE *ou_fopen(char *filename);
   void ou_fclose(FILE *stream);

   int ou_ReadFile(FILE *file, long fpos, void *buf, size_t len);
   int ou_WriteFile(FILE *file, long fpos, void *buf, size_t len);
#else

   int ou_fopen(char *filename);
   PFILEDATA ou_fBufferOpen(unsigned long filehandle );

   long ou_fgetfilesize( unsigned long filehandle );

   void ou_fclose(int stream);
   void ou_fBufferClose(unsigned long filehandle, PFILEDATA pFileData );

   int ou_ReadFile(unsigned long file, long fpos, void *buf, int len);

   int ou_WriteFile(unsigned long file, long fpos, void *buf, int len);
   int os_ReadFile(unsigned long file, long fpos, void *buf, int len);

#endif

#ifndef __DEBUG_ALLOC__
   #ifdef DBMALLOC
      #include "dbmalloc.h"
   #else
      #ifdef S_H
         #include "mymalloc.h"
         #undef malloc
         #define malloc(x) (((x)>262144)?(NULL):mymalloc(x))
      #else
         void *ou_MemAlloc(size_t size);
         int ou_SafeFree(void *p);

         #define malloc(x) ou_MemAlloc((x))
         #define free(x)   ou_SafeFree((x))
      #endif
   #endif
#else
   #include <malloc.h>
#endif

#ifdef RANDOM_MEM_FAIL
#undef malloc
#undef free
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#define malloc(size)  ((rand() % RANDOM_MEM_FAIL == 3)?NULL:malloc(size))
#endif

#if defined(DEBUG)
   #define NEED_PRINTLINE
   #include "printlin.h"
#endif

#endif
#define ELL_MALLOC_FAILED       (VS_ERROR_BASE + 0x360)

#endif
#endif
