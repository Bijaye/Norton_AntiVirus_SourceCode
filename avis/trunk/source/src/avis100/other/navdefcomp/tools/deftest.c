#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <process.h>
#include <string.h>

#include <windows.h>

#include "dev\navdefs.h"
#include "dev\navsdk.h"
#include "navsdkr.h"

//-----------------------------------------------------------------------------
// program will try any file listed on the command line after any options,
// then any listed in stdin
//-----------------------------------------------------------------------------

static int ARGN;        // current argument
static int READ_STDIN;
static int DO_REPAIR;   // what to do if infected; set by -r argument
static int VERBOSE;


static WORD scanFile   (const VIRUSLIBINIT* pVirLib, const char* zFname);
static WORD repairFile (const VIRUSLIBINIT* pVirLib, const char* zFname);

static const char* getNextFile (int argc, const char* argv[]);
static const char* scanResult (WORD rc);
static void printLibInfo (const VIRUSLIBINIT* pVLI);

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
   VIRUSLIBINIT hVirLib;
   const char*  zAppID;
   const char*  zFname;
   WORD         rc;

   // parse args
   for (ARGN = 1; ARGN < argc && argv[ARGN][0] == '-'; ARGN++) {
      if (!stricmp(argv[ARGN],"-r")) {
         DO_REPAIR = 1;
      }
      if (!stricmp(argv[ARGN],"-v")) {
         VERBOSE = 1;
      }
      if (!stricmp(argv[ARGN],"-stdin")) {
         READ_STDIN = 1;
      }
   }

   // get NAV-assigned app. ID (for accessing the virus definitions)
   zAppID = getenv("NAVSDK_APPID");
   if (!zAppID) {
      fprintf(stderr,"App ID environment variable NAVSDK_APPID not found\n");
      return 8;
   }
   printf ("using app. ID %s\n",zAppID); 

   // scanner initialization
   hVirLib.Progress                 = NULL;
   hVirLib.dwInterfaceVersion       = VIRLIB_INTERFACE_VERSION;
   hVirLib.bDisableNAVAPBySession   = FALSE;
   hVirLib.bManageVirusDefs         = TRUE;
   hVirLib.wHeuristicScanningLevel  = VIRLIB_HEURISTICS_NONE;
   strcpy(hVirLib.szAppID,zAppID);

   rc = VirusLibraryInit(&hVirLib);
   if (rc != VIRLIB_OK) {
      fprintf(stderr,"Error %d from VirusLibraryInit.\n",rc);
      return 8;
   }
   if (VERBOSE) printLibInfo(&hVirLib);

   // main loop
   while ((zFname = getNextFile(argc, argv))) {
      scanFile (&hVirLib, zFname);
   }

   // clean up & exit
   VirusLibraryClose(hVirLib.hScannerHandle);
   return 0;
}

//-----------------------------------------------------------------------------

static WORD scanFile (const VIRUSLIBINIT* pVirLib, const char* zFname) {
   char zVname[_MAX_PATH*2+1];
   WORD rc;

   rc = VirusLibraryScanFile(pVirLib->hScannerHandle,zFname,zVname);

   if(rc == VIRLIB_CLEAN) {
      printf ("%s: clean\n",zFname);
   }
   else if (rc == VIRLIB_INFECTED) {
      printf ("%s: infected with %s;",zFname,zVname);
 
      if (DO_REPAIR) {
         rc = repairFile (pVirLib, zFname);
      }
      else printf(" repair not tried\n");
   }else {
      printf ("%s: %d - %s\n",zFname, (int)rc, scanResult(rc));
   }
   return rc;
}

//-----------------------------------------------------------------------------

static WORD repairFile (const VIRUSLIBINIT* pVirLib, const char* zFname) {
   WORD rc;

   rc = VirusLibraryRepairFile(pVirLib->hScannerHandle,zFname);

   if (rc == VIRLIB_OK) {
      printf (" repaired\n");
   }
   else if (rc == VIRLIB_CANT_REPAIR) {
      printf (" cannot repair\n");
   }
   else {
      printf (" repair failed: %d\n", (int)rc);
   }
   return rc;
}

//-----------------------------------------------------------------------------

static const char* getNextFile (int argc, const char* argv[]) {
  static char zFname[_MAX_PATH*2+1];

  if (READ_STDIN)
     return gets(zFname);
  else if (ARGN < argc)
     return argv[ARGN++];

  return NULL;
}

//-----------------------------------------------------------------------------

static const char* scanResult (WORD rc) {
   switch (rc) {
      case VIRLIB_CLEAN : return "clean";
      case VIRLIB_INFECTED: return "infected";
      case VIRLIB_ERROR: return "ERROR";
      case VIRLIB_NOT_ENAB: return "NOT ENABLED";
      case VIRLIB_NO_LOGICAL: return "NO LOGICAL ?";
      default: break;
   }
   return "unknown return code";
}

//-----------------------------------------------------------------------------

static void printLibInfo (const VIRUSLIBINIT* pVLI) {
    printf ("Information from VirusLibraryInit():\n");
    printf(" Definitions from <%s>\n",pVLI->szVirusDefDir);
    printf(" Definitions level %04d-%02d-%02d (%d)\n",
      pVLI->wVirusDateYear,pVLI->wVirusDateMonth,pVLI->wVirusDateDay,pVLI->dwVirusDefRev);
    printf(" ScanSysArea %s supported.\n",(pVLI->FuncEnable.SysAreaFlags) & VIRLIB_SCANSYSAREA ? "is" : "not");
    printf(" Logical drives %s supported.\n",(pVLI->FuncEnable.SysAreaFlags) & VIRLIB_LOGICALDRV ? "are" : "not");
    printf(" ScanMem %s supported.\n",(pVLI->FuncEnable.MemFlags) & VIRLIB_SCANMEMORY ? "is" : "not");
    printf(" ScanFile %s supported.\n",(pVLI->FuncEnable.FileFlags) & VIRLIB_SCANFILES ? "is" : "not");
    printf(" DeleteFile %s supported.\n",(pVLI->FuncEnable.FileFlags) & VIRLIB_DELFILES ? "is" : "not");
}

//-----------------------------------------------------------------------------
