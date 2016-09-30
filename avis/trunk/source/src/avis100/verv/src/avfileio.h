// standard cross-platform file API for IBM AntiVirus.

#ifndef AVFILEIO_H
#define AVFILEIO_H

#include <stdlib.h>
#include <io.h>

#if defined(_cplusplus) || defined(__cplusplus)
   extern "C" {
#endif

//--- directory searching routines --------------------------------------------

typedef struct dirent {
   const char* d_name;
} DIRENT;

typedef struct DIR {
   long               handle;
   struct _finddata_t info;
   DIRENT             dirEntry;
   char               zPattern [_MAX_PATH+5];
} DIR;


DIR*    opendir   (const char* zDirName);

DIRENT* readdir   (DIR* pDir);

void    rewinddir (DIR* pDir);

int     closedir  (DIR* pDir);


#if defined(_cplusplus) || defined(__cplusplus)
}
#endif
#endif
