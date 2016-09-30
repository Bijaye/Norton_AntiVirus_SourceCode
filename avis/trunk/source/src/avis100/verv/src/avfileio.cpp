// standard cross-platform file API for IBM AntiVirus.

#include "avfileio.h"

#include <stdlib.h>
#include <io.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

#if defined(_cplusplus) || defined(__cplusplus)
   extern "C" {
#endif

static const char NO_NAME[] = "";

static void opendir_init(DIR* pDIR);

//--- directory searching routines --------------------------------------------

DIR* opendir (const char* zDirName) {
   DIR* pDir;
   size_t iPathLength;

   iPathLength = strlen(zDirName);
   if (iPathLength > _MAX_PATH) {
     errno = ENAMETOOLONG;
     return NULL;
   }
   
   pDir = (DIR*)malloc(sizeof(DIR));
   if (!pDir) {
      errno = ENOMEM;
      return NULL;
   }

   memcpy(pDir->zPattern, zDirName, iPathLength);
   if (pDir->zPattern[iPathLength-1] != '\\') 
      pDir->zPattern[iPathLength++] = '\\';
   pDir->zPattern[iPathLength++] = '*';
   pDir->zPattern[iPathLength] = '\0';

   opendir_init(pDir);
   if (pDir->handle == -1) {
      free (pDir);
      return NULL;
   }
   return pDir;
}


DIRENT* readdir (DIR* pDir) {
   int rc;

   if (!pDir || pDir->handle == -1) {
      errno = EBADF;
      return NULL;
   }

   if (pDir->dirEntry.d_name == NO_NAME) {   // 1st time
      pDir->dirEntry.d_name = pDir->info.name;
      return &pDir->dirEntry;
   }

   rc = _findnext (pDir->handle, &pDir->info);

   return rc ? NULL : &pDir->dirEntry;
}


void rewinddir (DIR* pDir) {
   if (pDir && _findclose(pDir->handle) == 0)
      opendir_init(pDir);
}


int closedir (DIR* pDir) {
   int rc;

   if (pDir) {
      rc = _findclose (pDir->handle);
      free (pDir);
      if (!rc)
         return 0;
   }
   errno = EBADF;
   return -1;
}


//--- local functions ---------------------------------------------------------


static void opendir_init(DIR* pDir) {
   pDir->handle = _findfirst (pDir->zPattern, &pDir->info);
   pDir->dirEntry.d_name = NO_NAME;
}


#if defined(_cplusplus) || defined(__cplusplus)
   }   // extern "C" {
#endif
